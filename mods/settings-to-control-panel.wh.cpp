// ==WindhawkMod==
// @id             settings-to-control-panel
// @name           Redirect Settings to Control Panel
// @description    This mod forces the classic Control Panel to open instead of Windows 10/11 Settings app using native components.
// @version        10.0.35
// @author         babamohammed
// @github         https://github.com/babamohammed2022
// @include        explorer.exe
// @architecture   x86-64
// @compilerOptions -lcomctl32 -lpsapi -lole32
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Redirect Settings → Control Panel
## Screenshot
![Image](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/Senza%20nome.png)
---
## About
This mod intercepts modern `ms-settings:` links (the ones that open the
Settings app) and redirects them to their corresponding classic Control
Panel pages, using only native Windows components.

---

## Compatibility

- **Windows 10** – Mostly complete support
- **Windows 11** – Partial support

**Note**: The mod has been tested on Windows 10 1809, Windows 10 21H2, Windows 11 23H2 and Windows 11 24H2 and the tests confirm that the mod is more functional on Windows 10 but both should not cause issues.

---

## Features

- Redirects many `ms-settings:` links to the classic Control Panel
- Anti-loop protection (stops windows from reopening endlessly)
- Configurable fallback behavior for unmapped links
- Tray menu detection (experimental)

**Note**: This mod is a best-effort implementation. It aims to intercept and redirect as many `ms-settings:` links as possible, but due to differences between Windows 10 and Windows 11, as well as changes introduced by Microsoft in each build, some redirects may not work perfectly in all environments.

---

## Limitations

- The system tray context menu redirect only supports the Win32 taskbar (the one from Windows 10). However, in some Windows 11 configurations if explorer is restarted the network system tray redirect might not work.
- The device & printers system tray redirect may not work on some Windows 11 configurations, as Microsoft hardcoded the redirect to the Settings app in certain shell code paths. This could change in future if correct documentation is found.
- The mod is not compatible with 32 bit based operating systems. It requires a 64-bit version of Windows (x64 or ARM64).

---

**Recommendation**: For a better experience on Windows 11 (and Windows 10 if necessary), it is recommended to pair this mod with Anixx's **[Restore the classic Personalization and other CPLs](https://windhawk.net/mods/restore-classic-cpls)** that re-enables some of the classic applets from older Windows versions. Some other suggested mods are:

- **[Windows 7/8.1 Action Center Recreation](https://windhawk.net/mods/win7-action-center-recreation)** – recreates the classic Windows 7/8.1 Action Center tray icon and flyout with real-time security status monitoring along with a partial restore of a link inside the Action Center Control Panel page.
- **[Classic Taskbar and Start Menu Properties](https://windhawk.net/mods/classic-taskbar-properties)** – recreates the classic Windows 7 "Taskbar and Start Menu Properties" dialog for Windows 10 and 11.
- **[Windows 7 Network Flyout Recreation](https://windhawk.net/mods/win7-network-flyout-recreation)** – recreates the classic Windows 7 network flyout with Wi-Fi list, signal strength, and connection support and, if enabled, partial restore of some links inside the classic "Network and Sharing Center" Control Panel page.

All of these mods are **reversible** and help make Windows 10 and 11 look more like Windows 7 and classic versions of Windows without replacing system files.

---

## Credits

- m417z – Code reviews and feedback
- Anixx – Testing on Windows 11 23H2 and the original toolbar subclassing approach
- sebastian08dm08-cpu - Testing on Windows 10 1809
- dbilanoski – CLSID documentation
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- EnableRedirects: true
  $name: Enable Redirects
  $description: "This setting turns the mod on or off. When disabled, Settings opens normally as usual."
- RedirectSystemTray: false
  $name: Redirect System Tray Audio/Network/Device & Printers (EXPERIMENTAL)
  $description: "If this setting is enabled, right-clicking the Audio, Network, or Devices & Printers icon near the clock and choosing 'Open Sound settings', 'Open Network settings', or 'Open devices and printers' will open the classic Control Panel instead of the Settings app. It is primarily recommended on Windows 10. Note: the network redirect may stop working after Explorer restarts on certain builds."
- UIOnlyRedirects: false
  $name: Non-Invasive Mode
  $description: "This setting changes the behavior of the mod by only redirecting Settings links clicked in the UI. Programs and background processes that open Settings directly are not affected. It is recommended on Windows 11 for safety. On Windows 10, leaving this off gives better coverage as it has more parts of the Control Panel compared to the successor."
- FallbackMode: "2"
  $name: Behavior for Unmapped Links
  $description: "This setting changes the fallback method (what to do when a Settings page has no classic Control Panel equivalent). It is recommended to put 'Pass through' on both Windows 10 and 11, so unmapped pages still open normally instead of silently failing."
  $options:
  - "0": Ignore (silent fail)
  - "1": Open the Control Panel (control.exe)
  - "2": Pass through to the modern Settings application (ms-settings.exe)
- Win11CompatibilityMode: false
  $name: Windows 11 Compatibility Mode
  $description: "This is a safer mode for Windows 11. When enabled, only redirects pages that are known to work correctly, and opens the standard Control Panel as a fallback for everything else. Helps avoid redirect loops and blank pages. Recommended on Windows 11. Not needed on Windows 10."
- MaxLaunchesPerUri: 3
  $name: Anti-Loop Limit (per window, every 5 seconds)
  $description: "This is a safety measure: if the same window gets opened too many times within a few seconds, the mod stops reopening it. Do not set this to 0 — without this limit, a redirect loop can open windows endlessly and freeze Explorer."
- ComActivationRedirect: false
  $name: COM-activation Redirect (EXPERIMENTAL)
  $description: "This setting intercepts Settings launches that happen through the COM interface rather than the normal shell. On Windows 11, this affects all app launches process-wide, so only enable it if you have a specific issue it fixes (such as tray icons opening Settings instead of Control Panel on certain builds). On Windows 10 this setting has no effect."
- LegacyNameMappingFix: true
  $name: Fix Legacy Name Mapping
  $description: "This option fixes a shell issue where certain classic Control Panel pages show up blank or silently redirect to the modern Settings app. Recommended on both Windows 10 and 11."
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <commctrl.h>
#include <psapi.h>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>

// Manually defined GUIDs to avoid requiring -luuid / static ole32 linkage.
// {45BA127D-10A8-46EA-8AB7-56EA9078943C} = CLSID_ApplicationActivationManager
static const CLSID CLSID_ApplicationActivationManager_STC =
    { 0x45ba127d, 0x10a8, 0x46ea, { 0x8a, 0xb7, 0x56, 0xea, 0x90, 0x78, 0x94, 0x3c } };
// {2E941141-7F97-4756-BA1D-9DECDE894A3D} = IID_IApplicationActivationManager
static const IID IID_IApplicationActivationManager_STC =
    { 0x2e941141, 0x7f97, 0x4756, { 0xba, 0x1d, 0x9d, 0xec, 0xde, 0x89, 0x4a, 0x3d } };

// TrackPopupMenuEx hook (DLL-based fallback method)
using TrackPopupMenuEx_t = BOOL(WINAPI*)(HMENU, UINT, int, int, HWND, const TPMPARAMS*);
static TrackPopupMenuEx_t g_origTrackPopupMenuEx = nullptr;

// Set on WM_RBUTTONUP by the subclass proc so TrackPopupMenuEx knows the icon type.
static int g_trayContextType = 0;
static DWORD g_trayContextTick = 0;
static std::mutex g_trayContextMutex;
static constexpr DWORD TRAY_CONTEXT_MAX_AGE_MS = 1500;

// x86-64 only (@architecture x86-64); _WIN64 is always defined.
#define ICMH_CALL __cdecl

using ICMH_CAODTM_t = bool(ICMH_CALL*)(HMENU, HWND);
// CDevicesAndPrintersFolder::_HandleContextMenu has a different second parameter
// (unsigned int, not HWND), so it gets its own correctly-typed function pointer type.
using ICMH_HCM_t = bool(ICMH_CALL*)(void* /*pThis*/, HMENU, UINT);
static ICMH_CAODTM_t g_icmhOrig_SndVolSSO = nullptr;
static ICMH_CAODTM_t g_icmhOrig_pnidui    = nullptr;
static ICMH_HCM_t g_icmhOrig_Shell32Devices = nullptr;
static bool g_pniduiHookInstalled = false;
static std::mutex g_pniduiHookMutex;
static HANDLE g_traySubclassWatchdogThread = nullptr;
static HWND g_lastShellTrayWnd = nullptr;
static HANDLE g_stopEvent = nullptr;

static bool ICMH_CALL ICMH_hook_SndVolSSO(HMENU m, HWND w);
static bool ICMH_CALL ICMH_hook_pnidui(HMENU m, HWND w);
static bool ICMH_CALL ICMH_hook_Shell32Devices(void* pThis, HMENU m, UINT u);

// Constants
#define PERS_ROOT       L"explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}"
#define PERS_WALLPAPER  L"explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921} -Microsoft.Personalization\\pageWallpaper"
#define PERS_COLORS     L"explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921} -Microsoft.Personalization\\pageColorization"

#define SYSTEM_PROPS_CLSID  L"shell:::{BB06C0E4-D293-4f75-8A90-CB05B6477EEE}"
#define NOTIF_AREA_CLSID    L"shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}"
#define WIN11_PASSTHROUGH   L"__PASSTHROUGH__"
#define EASE_OF_ACCESS      L"explorer shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"

using CreateProcessW_t = BOOL(WINAPI*)(LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION);
static CreateProcessW_t CreateProcessW_orig = nullptr;

using ShellExecuteW_t = HINSTANCE(WINAPI*)(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, INT);
using ShellExecuteExW_t = BOOL(WINAPI*)(SHELLEXECUTEINFOW*);
static ShellExecuteExW_t ShellExecuteExW_orig = nullptr;
static ShellExecuteW_t ShellExecuteW_orig = nullptr;

struct ResolveResult {
    std::wstring target;
    bool intercept;
};

static thread_local int g_hookDepth = 0;

struct HookGuard {
    HookGuard() { ++g_hookDepth; }
    ~HookGuard() { --g_hookDepth; }
    bool IsReentrant() const { return g_hookDepth > 1; }
};

static std::wstring ToLower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(), ::towlower);
    return s;
}

static bool IsShellProcess() {
    static int isShell = -1;
    if (isShell == -1) {
        HWND hShellWnd = GetShellWindow();
        if (hShellWnd) {
            DWORD shellPid = 0;
            GetWindowThreadProcessId(hShellWnd, &shellPid);
            isShell = (shellPid == GetCurrentProcessId()) ? 1 : 0;
        } else {
            // Early startup: check command line for factory/worker flags
            std::wstring cmd = ToLower(GetCommandLineW());
            if (cmd.find(L" /factory") != std::wstring::npos || 
                cmd.find(L" /separate") != std::wstring::npos ||
                cmd.find(L" /nodeuse") != std::wstring::npos) {
                isShell = 0;
            } else {
                isShell = 1;
            }
        }
    }
    return isShell == 1;
}

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
    static int isChild = -1;
    if (isChild == -1) {
        isChild = (GetEnvironmentVariableW(L"WH_STC_NOREDIRECT", nullptr, 0) > 0) ? 1 : 0;
    }
    return isChild == 1;
}

struct ModSettings {
    bool enableRedirects = true;
    bool redirectSystemTray = false;
    bool uiOnlyRedirects = false;
    int fallbackMode = 2;
    bool win11CompatibilityMode = false;
    int maxLaunchesPerUri = 3;
    bool comActivationRedirect = false;
    bool legacyNameMappingFix = true;
};

static ModSettings g_settings;

static bool ICMH_CALL ICMH_hook_SndVolSSO(HMENU m, HWND w) {
    if (!g_settings.enableRedirects || !g_settings.redirectSystemTray)
        return g_icmhOrig_SndVolSSO ? g_icmhOrig_SndVolSSO(m, w) : true;
    return false;
}

static bool ICMH_CALL ICMH_hook_pnidui(HMENU m, HWND w) {
    if (!g_settings.enableRedirects || !g_settings.redirectSystemTray)
        return g_icmhOrig_pnidui ? g_icmhOrig_pnidui(m, w) : true;
    return false;
}

static bool ICMH_CALL ICMH_hook_Shell32Devices(void* pThis, HMENU m, UINT u) {
    if (!g_settings.enableRedirects || !g_settings.redirectSystemTray)
        return g_icmhOrig_Shell32Devices ? g_icmhOrig_Shell32Devices(pThis, m, u) : true;
    return false;
}

static void LoadSettings() {
    g_settings.enableRedirects = Wh_GetIntSetting(L"EnableRedirects") != 0;
    g_settings.redirectSystemTray = Wh_GetIntSetting(L"RedirectSystemTray") != 0;
    g_settings.uiOnlyRedirects = Wh_GetIntSetting(L"UIOnlyRedirects") != 0;

    WindhawkUtils::StringSetting fallbackSetting(Wh_GetStringSetting(L"FallbackMode"));
    PCWSTR fallbackStr = fallbackSetting;
    if (fallbackStr[0] != L'\0') {
        int mode = _wtoi(fallbackStr);
        g_settings.fallbackMode = (mode >= 0 && mode <= 2) ? mode : 2;
    } else {
        g_settings.fallbackMode = 2;
    }

    g_settings.win11CompatibilityMode = Wh_GetIntSetting(L"Win11CompatibilityMode") != 0;

    int ml = Wh_GetIntSetting(L"MaxLaunchesPerUri");
    g_settings.maxLaunchesPerUri = (ml >= 0 && ml <= 20) ? ml : 3;

    g_settings.comActivationRedirect = Wh_GetIntSetting(L"ComActivationRedirect") != 0;
    g_settings.legacyNameMappingFix = Wh_GetIntSetting(L"LegacyNameMappingFix") != 0;
}

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
}

struct BounceRecord {
    DWORD lastRedirectTick = 0;
};

static std::mutex g_bounceGuardMtx;
static std::unordered_map<std::wstring, BounceRecord> g_bounceGuard;

static constexpr DWORD BOUNCE_WINDOW_MS = 3000;

static void BounceGuardRecord(const std::wstring& uri) {
    std::lock_guard<std::mutex> lk(g_bounceGuardMtx);
    g_bounceGuard[uri].lastRedirectTick = GetTickCount();
}

static bool BounceGuardIsBounce(const std::wstring& uri) {
    std::lock_guard<std::mutex> lk(g_bounceGuardMtx);
    auto it = g_bounceGuard.find(uri);
    if (it == g_bounceGuard.end()) return false;
    DWORD elapsed = GetTickCount() - it->second.lastRedirectTick;
    if (elapsed < BOUNCE_WINDOW_MS) {
        it->second.lastRedirectTick = 0;
        return true;
    }
    return false;
}

struct LaunchRecord {
    int count = 0;
    DWORD firstTick = 0;
};

static std::mutex g_loopGuardMtx;
static std::unordered_map<std::wstring, LaunchRecord> g_loopGuard;

static constexpr DWORD LOOP_WINDOW_MS = 5000;

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

    return false;
}

static const std::unordered_set<std::wstring> g_win11SafeClsids = {
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
    L"shell:::{ecdb0924-4208-451e-8ee0-373c0956de16}",
    L"shell:::{ed7ba470-8e54-465e-825c-99712043e01c}",
    L"shell:::{f02c1a0d-be21-4350-88b0-7367fc96ef3c}",
};

static const std::unordered_set<std::wstring> g_win11LoopClsids = {
    L"shell:::{17cd9488-1228-4b2f-88ce-4298e93e0966}",
    L"shell:::{80f3f1d5-feca-45f3-bc32-752c152e456e}",
    L"shell:::{9fe63afd-59cf-4419-9775-abcc3849f861}",
    L"shell:::{bb06c0e4-d293-4f75-8a90-cb05b6477eee}",
    L"shell:::{ed834ed6-4b5a-4bfe-8f11-a626dcb6a921}",
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

static HWND g_hTrayToolbar = nullptr;
static std::mutex g_traySubclassMutex;
static std::mutex g_trayDllInfoMutex;
static std::mutex g_shellTrayWndMutex;
static BYTE* g_sndVolSSOBase = nullptr;
static BYTE* g_sndVolSSOEnd = nullptr;
static BYTE* g_pniduiBase = nullptr;
static BYTE* g_pniduiEnd = nullptr;

static bool InitTrayDllInfo() {
    std::lock_guard<std::mutex> lk(g_trayDllInfoMutex);

    if (!g_sndVolSSOBase) {
        HMODULE hSndVol = GetModuleHandleW(L"SndVolSSO.dll");
        if (hSndVol) {
            MODULEINFO mi{};
            if (GetModuleInformation(GetCurrentProcess(), hSndVol, &mi, sizeof(mi))) {
                g_sndVolSSOBase = (BYTE*)mi.lpBaseOfDll;
                g_sndVolSSOEnd = g_sndVolSSOBase + mi.SizeOfImage;
            }
        }
    }

    if (!g_pniduiBase) {
        HMODULE hPniDui = GetModuleHandleW(L"pnidui.dll");
        if (hPniDui) {
            MODULEINFO mi{};
            if (GetModuleInformation(GetCurrentProcess(), hPniDui, &mi, sizeof(mi))) {
                g_pniduiBase = (BYTE*)mi.lpBaseOfDll;
                g_pniduiEnd = g_pniduiBase + mi.SizeOfImage;
            }
        }
    }

    return (g_sndVolSSOBase != nullptr || g_pniduiBase != nullptr);
}
static int GetTrayButtonType(HWND hToolbar, int buttonIndex) {
    if (buttonIndex < 0) return 0;
    InitTrayDllInfo();

    TBBUTTON tb{};
    if (!SendMessageW(hToolbar, TB_GETBUTTON, buttonIndex, (LPARAM)&tb)) return 0;
    if (!tb.dwData) return 0;

    HWND hIconWnd = *(HWND*)tb.dwData;
    if (!hIconWnd || !IsWindow(hIconWnd)) return 0;

    wchar_t className[256]{};
    if (!GetClassNameW(hIconWnd, className, 256)) return 0;
    if (wcsncmp(className, L"ATL:", 4) != 0) {
        return 0;
    }

    const wchar_t* hexPart = className + 4;
    ULONG_PTR addr = 0;

    while (*hexPart) {
        wchar_t c = *hexPart;
        int digit = 0;
        if (c >= L'0' && c <= L'9') digit = c - L'0';
        else if (c >= L'A' && c <= L'F') digit = 10 + (c - L'A');
        else if (c >= L'a' && c <= L'f') digit = 10 + (c - L'a');
        else break;
        addr = (addr << 4) | digit;
        hexPart++;
    }

    BYTE* sndVolBase = nullptr;
    BYTE* sndVolEnd = nullptr;
    BYTE* pniduiBase = nullptr;
    BYTE* pniduiEnd = nullptr;
    {
        std::lock_guard<std::mutex> lk(g_trayDllInfoMutex);
        sndVolBase = g_sndVolSSOBase;
        sndVolEnd = g_sndVolSSOEnd;
        pniduiBase = g_pniduiBase;
        pniduiEnd = g_pniduiEnd;
    }

    if (sndVolBase && addr >= (ULONG_PTR)sndVolBase && addr < (ULONG_PTR)sndVolEnd)
        return 1; // Audio
    if (pniduiBase && addr >= (ULONG_PTR)pniduiBase && addr < (ULONG_PTR)pniduiEnd)
        return 2; // Network

    return 0;
}
static void OpenClassicSoundPanel() {
    SHELLEXECUTEINFOW sei = {};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = L"open";
    sei.lpFile = L"control.exe";
    sei.lpParameters = L"mmsys.cpl,,0";
    sei.nShow = SW_SHOWNORMAL;
    ShellExecuteExW_orig(&sei);
}

static void OpenClassicNetworkConnections() {
    SHELLEXECUTEINFOW sei = {};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_INVOKEIDLIST;
    sei.lpVerb = L"open";
    sei.lpFile = L"explorer.exe";
    sei.lpParameters = L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}";
    sei.nShow = SW_SHOWNORMAL;
    ShellExecuteExW_orig(&sei);
}

static void OpenClassicDevicesAndPrinters() {
    SHELLEXECUTEINFOW sei = {};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_INVOKEIDLIST;
    sei.lpVerb = L"open";
    sei.lpFile = L"explorer.exe";
    sei.lpParameters = L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}";
    sei.nShow = SW_SHOWNORMAL;
    ShellExecuteExW_orig(&sei);
}

static LRESULT CALLBACK TrayToolbarSubclassProc(
    HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData)
{
    if (msg == WM_RBUTTONUP) {
        POINT pt;
        pt.x = (int)(short)LOWORD(lParam);
        pt.y = (int)(short)HIWORD(lParam);
        int hitIndex = (int)SendMessageW(hwnd, TB_HITTEST, 0, (LPARAM)&pt);

        if (hitIndex >= 0) {
            int buttonType = GetTrayButtonType(hwnd, hitIndex);
            if (buttonType == 1) {
                std::lock_guard<std::mutex> lk(g_trayContextMutex);
                g_trayContextType = 1;
                g_trayContextTick = GetTickCount();
            }
            else if (buttonType == 2) {
                std::lock_guard<std::mutex> lk(g_trayContextMutex);
                g_trayContextType = 2;
                g_trayContextTick = GetTickCount();
            }
        }
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

static HWND FindTrayToolbar() {
    HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hTray) return nullptr;
    DWORD pid = 0;
    GetWindowThreadProcessId(hTray, &pid);
    if (pid != GetCurrentProcessId()) return nullptr;
    HWND hNotify = FindWindowExW(hTray, nullptr, L"TrayNotifyWnd", nullptr);
    if (!hNotify) return nullptr;
    HWND hSysPager = FindWindowExW(hNotify, nullptr, L"SysPager", nullptr);
    if (hSysPager) {
        HWND hToolbar = FindWindowExW(hSysPager, nullptr, L"ToolbarWindow32", nullptr);
        if (hToolbar) return hToolbar;
    }
    return FindWindowExW(hNotify, nullptr, L"ToolbarWindow32", nullptr);
}

static void SetupTraySubclass() {
    HWND hToolbar;
    {
        std::lock_guard<std::mutex> lk(g_traySubclassMutex);
        if (g_hTrayToolbar && IsWindow(g_hTrayToolbar)) return;
        g_hTrayToolbar = nullptr;
        hToolbar = FindTrayToolbar();
    }
    if (!hToolbar || !InitTrayDllInfo()) return;
    BOOL ok = WindhawkUtils::SetWindowSubclassFromAnyThread(hToolbar, TrayToolbarSubclassProc, 0);
    if (ok) {
        std::lock_guard<std::mutex> lk(g_traySubclassMutex);
        g_hTrayToolbar = hToolbar;
    }
}

static void RemoveTraySubclass() {
    HWND h;
    {
        std::lock_guard<std::mutex> lk(g_traySubclassMutex);
        h = g_hTrayToolbar;
        g_hTrayToolbar = nullptr;
    }
    if (h) WindhawkUtils::RemoveWindowSubclassFromAnyThread(h, TrayToolbarSubclassProc);
}
static bool IsAddressInModule(void* address, const wchar_t* moduleName) {
    HMODULE hModule = nullptr;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)address, &hModule)) {
        HMODULE hTarget = GetModuleHandleW(moduleName);
        return (hModule != nullptr && hModule == hTarget);
    }
    return false;
}

static BOOL WINAPI CommonTrackPopupMenuEx_Hook(
    HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, const TPMPARAMS* lptpm,
    void* callerRetAddr,
    BOOL (WINAPI* pOrig)(HMENU, UINT, int, int, HWND, const TPMPARAMS*),
    const wchar_t* logPrefix)
{
    if (!pOrig) return FALSE;

    if (!g_settings.redirectSystemTray || !g_settings.enableRedirects)
        return pOrig(hMenu, uFlags, x, y, hWnd, lptpm);

    HookGuard guard;
    if (guard.IsReentrant())
        return pOrig(hMenu, uFlags, x, y, hWnd, lptpm);

    // --- Primary: subclass flag set on WM_RBUTTONUP ---
    int contextType = 0;
    {
        std::lock_guard<std::mutex> lk(g_trayContextMutex);
        if (g_trayContextType != 0 && (GetTickCount() - g_trayContextTick <= TRAY_CONTEXT_MAX_AGE_MS)) {
            contextType = g_trayContextType;
        }
        g_trayContextType = 0;
        g_trayContextTick = 0;
    }

    bool isAudioMenu   = (contextType == 1);
    bool isNetworkMenu = (contextType == 2);
    bool isDeviceMenu  = false;

    // --- Fallback: DLL return-address detection ---
    if (!isAudioMenu && !isNetworkMenu) {
        void* retAddr = callerRetAddr;
        int itemCount = GetMenuItemCount(hMenu);
        if (itemCount > 0) {
            if (IsAddressInModule(retAddr, L"SndVolSSO.dll")) {
                isAudioMenu = (itemCount <= 10);
            }
            else if (IsAddressInModule(retAddr, L"pnidui.dll")) {
                isNetworkMenu = (itemCount >= 1 && itemCount <= 20);
            }
            else if (IsAddressInModule(retAddr, L"dxgi.dll")) {
                if (itemCount == 2 && GetMenuItemID(hMenu, 0) == 3107 && GetMenuItemID(hMenu, 1) == 3109) {
                    isNetworkMenu = true;
                }
                else if (GetMenuItemID(hMenu, 0) == 215) {
                    isDeviceMenu = true;
                }
            }
            else if (IsAddressInModule(retAddr, L"shell32.dll")) {
                for (int i = 0; i < itemCount; i++) {
                    if (GetMenuItemID(hMenu, i) == 215) {
                        isDeviceMenu = true;
                        break;
                    }
                }
            }
        }
    }

    if (!isAudioMenu && !isNetworkMenu && !isDeviceMenu)
        return pOrig(hMenu, uFlags, x, y, hWnd, lptpm);

    int itemCount = GetMenuItemCount(hMenu);
    int targetIndex = -1;
    
    if (isAudioMenu) {
        targetIndex = 0;
    }
    else if (isNetworkMenu) {
        for (int i = itemCount - 1; i >= 0; i--) {
            MENUITEMINFOW miiCheck = { sizeof(MENUITEMINFOW) };
            miiCheck.fMask = MIIM_FTYPE;
            if (GetMenuItemInfoW(hMenu, i, TRUE, &miiCheck)) {
                if (!(miiCheck.fType & MFT_SEPARATOR)) {
                    targetIndex = i;
                    break;
                }
            }
        }
    }
    else if (isDeviceMenu) {
        for (int i = 0; i < itemCount; i++) {
            if (GetMenuItemID(hMenu, i) == 215) {
                targetIndex = i;
                break;
            }
        }
    }
    
    if (targetIndex == -1) {
        return pOrig(hMenu, uFlags, x, y, hWnd, lptpm);
    }

    UINT originalId = GetMenuItemID(hMenu, targetIndex);
    bool callerWantedReturnCmd = (uFlags & TPM_RETURNCMD) != 0;
    uFlags |= TPM_RETURNCMD;
    
    BOOL result     = pOrig(hMenu, uFlags, x, y, hWnd, lptpm);
    int selectedId  = (int)result;

    if (originalId != 0 && selectedId == (int)originalId) {
        Wh_Log(L"[%s] Redirecting selection", logPrefix);
        if (isAudioMenu)        OpenClassicSoundPanel();
        else if (isNetworkMenu) OpenClassicNetworkConnections();
        else                    OpenClassicDevicesAndPrinters();
        return 0;
    }

    if (selectedId != 0 && !callerWantedReturnCmd) {
        PostMessageW(hWnd, WM_COMMAND, MAKEWPARAM((WORD)selectedId, 0), 0);
        return TRUE;
    }

    return result;
}

BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, const TPMPARAMS* lptpm) {
    // Capture the real caller before entering the shared implementation.
    // Windhawk builds with Clang, so __builtin_return_address is always available.
    void* callerRetAddr = __builtin_return_address(0);
    return CommonTrackPopupMenuEx_Hook(hMenu, uFlags, x, y, hWnd, lptpm, callerRetAddr, g_origTrackPopupMenuEx, L"TRAY-HOOK");
}

static std::unordered_map<std::wstring, std::wstring> g_mappings;

static void InitMappings() {
    const bool w11 = g_isWin11;

    g_mappings = {
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
        {L"ms-settings:fonts", L"shell:::{BD84B380-8CA2-1069-AB1D-08000948F534}"},
        {L"ms-settings:display-advanced-color", L"colorcpl.exe"},
        {L"ms-settings:colorcpl", L"colorcpl.exe"},
        {L"ms-settings:display", L"rundll32.exe display.dll,ShowAdapterSettings 0"},
        {L"ms-settings:display-advanced", L"rundll32.exe display.dll,ShowAdapterSettings 0"},
        {L"ms-settings:display-advanced-graphics", L"rundll32.exe display.dll,ShowAdapterSettings 0"},
        {L"ms-settings:display-adapter-properties", L"rundll32.exe display.dll,ShowAdapterSettings 0"},
        {L"ms-settings:display-resolution", L"rundll32.exe display.dll,ShowAdapterSettings 0"},
        {L"ms-settings:screenrotation", L"rundll32.exe display.dll,ShowAdapterSettings 0"},
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
        {L"ms-settings:system-settings", L"shell:::{025A5937-A6BE-4686-A844-36FE4BEC8B6D}\\pageGlobalSettings"},
        {L"ms-settings:powersleep", L"powercfg.cpl"},
        {L"ms-settings:battery", L"powercfg.cpl"},
        {L"ms-settings:batterysaver", L"powercfg.cpl"},
        {L"ms-settings:batterysaver-settings", L"powercfg.cpl"},
        {L"ms-settings:batterysaver-usagedetails", L"powercfg.cpl"},
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
        {L"ms-settings:notifications", NOTIF_AREA_CLSID},
        {L"ms-settings:taskbar-notifications", NOTIF_AREA_CLSID},
        {L"ms-settings:taskbar-systemtray", NOTIF_AREA_CLSID},
        {L"ms-settings:notifications-systemtray", NOTIF_AREA_CLSID},
        {L"ms-settings:systemtray", NOTIF_AREA_CLSID},
        {L"ms-settings:notificationiconpreferences", NOTIF_AREA_CLSID},
        {L"ms-settings:mousetouchpad", L"main.cpl"},
        {L"ms-settings:devices-touchpad", L"main.cpl"},
        {L"ms-settings:keyboard", L"main.cpl,,1"},
        {L"ms-settings:typing", L"main.cpl,,1"},
        {L"ms-settings:pen", w11 ? L"control.exe" : L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}"},
        {L"ms-settings:pen-windowsink", w11 ? L"control.exe" : L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}"},
        {L"ms-settings:pen-windowsinksettings", w11 ? L"control.exe" : L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}"},
        {L"ms-settings:devices-touch", w11 ? L"control.exe" : L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}"},
        {L"ms-settings:autoplay", L"shell:::{9C60DE1E-E5FC-40f4-A487-460851A8D915}"},
        {L"ms-settings:printers", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:printers-scanners", L"shell:::{2227A280-3AEA-1069-A2DE-08002B30309D}"},
        {L"ms-settings:bluetooth", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:usb", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:connecteddevices", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:mobile-devices", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:camera", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:privacy-customdevices", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:network", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-wifi", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-ethernet", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-vpn", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-airplanemode", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-mobilehotspot", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-cellular", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:datausage", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-proxy", L"inetcpl.cpl,,4"},
        {L"ms-settings:network-status", L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
        {L"ms-settings:network-dialup", L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
        {L"ms-settings:firewall", L"shell:::{4026492F-2F69-46B8-B9BF-5654FC07E423}"},
        {L"ms-settings:network-firewall", L"shell:::{4026492F-2F69-46B8-B9BF-5654FC07E423}"},
        {L"ms-settings:windowsdefender", L"shell:::{4026492F-2F69-46B8-B9BF-5654FC07E423}"},
        {L"ms-settings:network-places", L"shell:::{F02C1A0D-BE21-4350-88B0-7367FC96EF3C}"},
        {L"ms-settings:yourinfo", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:yourinfo-profile", L"shell:::{59031a47-3f72-44a7-89c5-5595fe6b30ee}"},
        {L"ms-settings:emailandaccounts", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:accounts", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:startupapps", L"msconfig.exe"},
        {L"ms-settings:netplwiz", L"shell:::{7A9D77BD-5403-11d2-8785-2E0420524153}"},
        {L"ms-settings:workplace", L"shell:::{26EE0668-A00A-44D7-9371-BEB064C98683}\\0\\::{ECDB0924-4208-451E-8EE0-373C0956DE16}"},
        {L"ms-settings:defaultapps", w11 ? WIN11_PASSTHROUGH : L"shell:::{17cd9488-1228-4b2f-88ce-4298e93e0966}"},
        {L"ms-settings:dateandtime", L"timedate.cpl"},
        {L"ms-settings:dateandtime-region", L"timedate.cpl"},
        {L"ms-settings:dateandtime-addclocks", L"timedate.cpl,,1"},
        {L"ms-settings:regionlanguage", L"intl.cpl"},
        {L"ms-settings:regionformatting", L"intl.cpl"},
        {L"ms-settings:language", L"intl.cpl"},
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
        {L"ms-settings:recovery", w11 ? L"control.exe" : L"shell:::{9FE63AFD-59CF-4419-9775-ABCC3849F861}"},
        {L"ms-settings:troubleshoot", w11 ? L"msdt.exe -id DeviceDiagnostic" : L"shell:::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}"},        
        {L"ms-settings:deviceencryption", L"shell:::{D9EF8727-CAC2-4e60-809E-86F80A666C91}"},
        {L"ms-settings:gaming-gamebar", L"joy.cpl"},
        {L"ms-settings:folders", L"shell:::{6DFD7C5C-2451-11d3-A299-00C04F8EF6AF}"},
        {L"ms-settings:appsfeatures-app", L"shell:::{15eae92e-f17a-4431-9f28-805e482dafd4}"},
        {L"ms-settings:windowsupdate-history", L"shell:::{d450a8a1-9568-45c7-9c0e-b4f9fb4537bd}"},
        {L"ms-settings:troubleshoot-history", L"shell:::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}\\historyPage"},
        {L"ms-settings:keyboard-advanced", L"shell:::{26EE0668-A00A-44D7-9371-BEB064C98683}\\0\\::{725BE8F7-668E-4C7B-8F90-46BDB0936430}"},
        {L"ms-settings:keyboard-properties", L"shell:::{725BE8F7-668E-4C7B-8F90-46BDB0936430}"},
        {L"ms-settings:privacy-feedback", L"shell:::{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}\\pageReportDetails"},
        {L"ms-settings:problem-reporting-settings", L"shell:::{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}\\pageSettings"},
        {L"ms-settings:problem-reports", L"shell:::{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}\\pageProblems"},
        {L"ms-settings:reliability", L"shell:::{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}\\pageReliabilityView"},
        {L"ms-settings:speech", L"shell:::{D17D1D6D-CC3F-4815-8FE3-607E7D5D10B3}"},
        {L"ms-settings:search-diagnostics", L"shell:::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}\\searchPage"},
        {L"ms-settings:controlpanel", L"shell:::{ED7BA470-8E54-465E-825C-99712043E01C}"},
        {L"ms-settings:signinoptions", L"netplwiz"},
        {L"ms-settings:accounts-signinoptions", L"netplwiz"},
        {L"ms-settings:accounts-users", L"netplwiz"},
        {L"ms-settings:family-users", L"netplwiz"},
        {L"ms-settings:power", L"powercfg.cpl"},
        {L"ms-settings:display-hdr", L"colorcpl.exe"},
        {L"ms-settings:personalization-taskbar", NOTIF_AREA_CLSID},
        {L"ms-settings:multitasking", L"control.exe"},
        {L"ms-settings:storage", L"control.exe"},
        {L"ms-settings:storagesense", L"control.exe"},
    };

    g_mappings[L"ms-settings:backup"] = L"control.exe /name Microsoft.BackupAndRestore";
    g_mappings[L"ms-settings:network-advancedsettings"] = L"control.exe /name Microsoft.NetworkAndSharingCenter";

    if (g_isWin11) {
        g_mappings[L"ms-settings:recovery"] = L"shell:::{26EE0668-A00A-44D7-9371-BEB064C98683}\\0\\::{9FE63AFD-59CF-4419-9775-ABCC3849F861}";
    }
}

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


static std::wstring ApplyWin11Filter(const std::wstring& target) {
    if (!g_isWin11) return target;
    std::wstring lower = ToLower(target);
    if (lower.find(L"shell:::") != 0 && lower.find(L"explorer shell:::") != 0) return target;
    
    std::wstring clsPart = lower;
    if (lower.find(L"explorer ") == 0) clsPart = lower.substr(9);
    
    if (IsClsidLoopOnWin11(clsPart)) {
        if (lower.find(L"ed834ed6") != std::wstring::npos) {
            if (lower.find(L"pagewallpaper") != std::wstring::npos) return PERS_WALLPAPER;
            if (lower.find(L"pagecolorization") != std::wstring::npos) return PERS_COLORS;
            return PERS_ROOT;
        }
        if (lower.find(L"bb06c0e4") != std::wstring::npos) return L"sysdm.cpl";
        return L"control.exe";
    }
    if (g_settings.win11CompatibilityMode && !IsClsidSafeOnWin11(clsPart)) {
        return L"control.exe";
    }
    return target;
}

static bool HandleFallback(const std::wstring& uri) {
    switch (g_settings.fallbackMode) {
        case 0: return true;
        case 1: {
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
        default: return false;
    }
}

static void LaunchTarget(const std::wstring& command) {
    if (!LoopGuardAllow(command)) return;

    std::wstring lower = ToLower(command);
    
    if (lower.find(L"explorer shell:::") != std::wstring::npos) {
        SHELLEXECUTEINFOW sei = {};
        sei.cbSize = sizeof(sei);
        sei.fMask = SEE_MASK_FLAG_NO_UI;
        sei.lpVerb = L"open";
        sei.lpFile = L"explorer.exe";
        sei.lpParameters = command.c_str() + 9;
        sei.nShow = SW_SHOWNORMAL;
        ShellExecuteExW_orig(&sei);
        return;
    }
    
    if (lower.find(L"rundll32.exe ") == 0) {
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
        sei.lpParameters = command.c_str() + 13;
        sei.nShow = SW_SHOWNORMAL;
        ShellExecuteExW_orig(&sei);
        return;
    }
    
    bool isFullCmdLine = (lower.find(L"explorer.exe ") != std::wstring::npos) ||
                         (lower.find(L"control.exe /") != std::wstring::npos);
    if (isFullCmdLine) {
        STARTUPINFOW si = {};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_SHOWNORMAL;
        PROCESS_INFORMATION pi = {};
        std::wstring mutable_cmd = command;
        if (!CreateProcessW_orig(nullptr, mutable_cmd.data(), nullptr, nullptr,
                                 FALSE, CREATE_UNICODE_ENVIRONMENT,
                                 (LPVOID)g_childEnvBlock.c_str(), nullptr, &si, &pi)) {
        } else {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        return;
    }

    if (command == L"devmgmt.msc" || command == L"compmgmt.msc" ||
        command == L"slui.exe" || command == L"OptionalFeatures.exe") {
        ShellExecuteW_orig(nullptr, L"open", command.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
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
    } else if (command.find(L".cpl") != std::wstring::npos) {
        ShellExecuteW_orig(nullptr, L"open", L"control.exe", command.c_str(), nullptr, SW_SHOWNORMAL);
        return;
    } else if (command.find(L".exe") != std::wstring::npos) {
        cmdLine = command;
    } else if (command.find(L"shell:::") == 0) {
        SHELLEXECUTEINFOW sei = {};
        sei.cbSize = sizeof(sei);
        sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_INVOKEIDLIST;
        sei.lpVerb = L"open";
        sei.lpFile = L"explorer.exe";
        sei.lpParameters = command.c_str();
        sei.nShow = SW_SHOWNORMAL;
        ShellExecuteExW_orig(&sei);
        return;
    } else if (command.empty()) {
        cmdLine = L"control.exe";
    } else {
        cmdLine = L"control.exe " + command;
    }

    if (!cmdLine.empty()) {
        std::wstring mutableCmd = cmdLine;
        if (!CreateProcessW_orig(nullptr, mutableCmd.data(), nullptr, nullptr,
                                 FALSE, CREATE_UNICODE_ENVIRONMENT,
                                 (LPVOID)g_childEnvBlock.c_str(), nullptr, &si, &pi)) {
            return;
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

static bool IsPersonalizationWindow(HWND hwnd) {
    if (!hwnd) return false;
    HWND h = hwnd;
    while (h) {
        wchar_t cls[256] = {}, title[512] = {};
        GetClassNameW(h, cls, 256);
        GetWindowTextW(h, title, 512);
        std::wstring c = ToLower(cls), t = ToLower(title);
        if (c == L"progman" || c == L"workerw" || c == L"shelldll_defview") return false;
        if (c == L"cabinetwclass") return true;
        if (t.find(L"personaliz") != std::wstring::npos) return true;
        HWND parent = GetParent(h);
        if (!parent || parent == h) break;
        h = parent;
    }
    return false;
}

static std::wstring ResolvePersonalizationBackground(HWND hwnd) {
    return IsPersonalizationWindow(hwnd) ? PERS_WALLPAPER : PERS_ROOT;
}

static bool ShouldApplyBounceGuard(const std::wstring& uri) {
    return uri.find(L"personalization") != std::wstring::npos;
}

static ResolveResult ResolveUri(const std::wstring& uri, HWND hwnd) {
    if (uri == L"ms-settings:personalization-background") {
        if (BounceGuardIsBounce(uri)) return {L"", true};
        std::wstring t = ApplyWin11Filter(ResolvePersonalizationBackground(hwnd));
        BounceGuardRecord(uri);
        return {t, true};
    }
    auto it = g_mappings.find(uri);
    if (it != g_mappings.end()) {
        bool useBounceGuard = ShouldApplyBounceGuard(uri);
        if (useBounceGuard && BounceGuardIsBounce(uri)) {
            bool handled = HandleFallback(uri);
            return {L"", handled};
        }
        std::wstring t = ApplyWin11Filter(it->second);
        if (t == WIN11_PASSTHROUGH) {
            bool handled = HandleFallback(uri);
            return {L"", handled};
        }
        if (useBounceGuard) BounceGuardRecord(uri);
        return {t, true};
    }
    if (uri.find(L"ms-settings:") == 0) {
        bool handled = HandleFallback(uri);
        return {L"", handled};
    }
    return {L"", false};
}
// ===========================================================================
// EXPERIMENTAL: IApplicationActivationManager COM interception
//
// Some Windows 11 shell components (notably the system tray flyouts for
// "Open Devices and Printers") may bypass ShellExecute/CreateProcess entirely
// and instead activate the Settings app through the low-level COM interface
// IApplicationActivationManager::ActivateApplication().
//
// We install a tiny vtable-style hook on the COM object returned by
// CoCreateInstance(CLSID_ApplicationActivationManager) to inspect every
// ActivateApplication call.  When the appUserModelId matches
// "windows.immersivecontrolpanel..." (the Settings app), we:
//   1) map the ms-settings: URI embedded in the arguments to a classic CPL
//   2) launch that CPL ourselves
//   3) return S_OK to the caller (making it believe Settings was launched)
//
// This is entirely best-effort and based on reverse engineering assumptions.
// If anything unexpected happens we fall back to the original vtable entry.
// ===========================================================================

// Minimal vtable layout for IApplicationActivationManager (3 methods)
struct IApplicationActivationManagerVtbl {
    // IUnknown
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IUnknown*, REFIID, void**);
    ULONG   (STDMETHODCALLTYPE *AddRef)(IUnknown*);
    ULONG   (STDMETHODCALLTYPE *Release)(IUnknown*);
    // IApplicationActivationManager
    HRESULT (STDMETHODCALLTYPE *ActivateApplication)(
        IUnknown*,
        LPCWSTR appUserModelId,
        LPCWSTR arguments,
        DWORD options,
        DWORD* processId);
    HRESULT (STDMETHODCALLTYPE *ActivateForFile)(IUnknown*, LPCWSTR, LPCWSTR, DWORD, DWORD*);
    HRESULT (STDMETHODCALLTYPE *ActivateForProtocol)(IUnknown*, LPCWSTR, DWORD*, DWORD);
};

using ActivateApplication_t = HRESULT (STDMETHODCALLTYPE *)(
    IUnknown*,
    LPCWSTR appUserModelId,
    LPCWSTR arguments,
    DWORD options,
    DWORD* processId);

static ActivateApplication_t g_origActivateApplication = nullptr;
static bool g_aamHookInstalled = false;
static std::mutex g_aamHookMutex;

HRESULT STDMETHODCALLTYPE AAM_ActivateApplication_hook(
    IUnknown* pThis,
    LPCWSTR appUserModelId,
    LPCWSTR arguments,
    DWORD options,
    DWORD* processId)
{
    if (!g_settings.enableRedirects || !g_settings.comActivationRedirect) {
        if (g_origActivateApplication) {
            return g_origActivateApplication(pThis, appUserModelId, arguments, options, processId);
        }
        return E_FAIL;
    }

    Wh_Log(L"[AAM-HOOK] ActivateApplication: appId=%s, args=%s",
           appUserModelId ? appUserModelId : L"(null)",
           arguments ? arguments : L"(null)");

    // Is this the Settings app being activated?
    if (appUserModelId && arguments &&
        _wcsnicmp(appUserModelId, L"windows.immersivecontrolpanel", 29) == 0)
    {
        std::wstring uri = NormalizeUri(arguments);
        Wh_Log(L"[AAM-HOOK] Settings activation intercepted: %s", uri.c_str());

        auto result = ResolveUri(uri, nullptr);
        if (result.intercept) {
            if (!result.target.empty()) {
                LaunchTarget(result.target);
                Wh_Log(L"[AAM-HOOK] Redirected to: %s", result.target.c_str());
            } else {
                Wh_Log(L"[AAM-HOOK] Activation handled by fallback mode");
            }
            if (processId) *processId = GetCurrentProcessId();
            return S_OK;
        }
        Wh_Log(L"[AAM-HOOK] No mapping found, falling back to original");
    }

    // Not a Settings activation we can handle — call original
    if (g_origActivateApplication) {
        return g_origActivateApplication(pThis, appUserModelId, arguments, options, processId);
    }
    return E_FAIL;
}

static void InstallAAMHook() {
    std::lock_guard<std::mutex> lk(g_aamHookMutex);
    if (g_aamHookInstalled) return;

    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    IUnknown* pAAM = nullptr;
    HRESULT hr = CoCreateInstance(
        CLSID_ApplicationActivationManager_STC,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_IApplicationActivationManager_STC,
        (void**)&pAAM);

    if (SUCCEEDED(hr) && pAAM) {
        IApplicationActivationManagerVtbl* vtbl = *(IApplicationActivationManagerVtbl**)pAAM;
        if (vtbl) {
            if (WindhawkUtils::SetFunctionHook((ActivateApplication_t)vtbl->ActivateApplication, AAM_ActivateApplication_hook, &g_origActivateApplication)) {
                g_aamHookInstalled = true;
                Wh_Log(L"[AAM-HOOK] Successfully installed");
            }
        }
        pAAM->Release();
    } else {
        Wh_Log(L"[AAM-HOOK] CoCreateInstance failed: 0x%08X", hr);
    }
    
    if (SUCCEEDED(hrCo)) CoUninitialize();
}

bool (*COpenControlPanel__MapLegacyName_orig)(void*, LPCWSTR, LPWSTR, UINT, bool*);

static bool ShouldSuppressLegacyNameMapping(LPCWSTR pszLegacyName) {
    if (!pszLegacyName || !*pszLegacyName) return false;

    std::wstring name = ToLower(pszLegacyName);

    // Keep the fix narrowly scoped.  _MapLegacyName is a process-wide shell32
    // internal used by Control Panel name resolution, so suppressing every
    // mapping can affect unrelated Control Panel navigation.  Only suppress
    // the legacy names that this mod can launch directly and that are known
    // to be susceptible to Settings remapping/blank-page behavior.
    static const std::unordered_set<std::wstring> kNames = {
        L"system",
        L"microsoft.system",
        L"sound",
        L"microsoft.sound",
        L"backupandrestore",
        L"microsoft.backupandrestore",
        L"networkandsharingcenter",
        L"microsoft.networkandsharingcenter",
        L"personalization",
        L"microsoft.personalization",
    };

    return kNames.count(name) != 0;
}

bool COpenControlPanel__MapLegacyName_hook(
    void    *pThis,
    LPCWSTR  pszLegacyName,
    LPWSTR   pszNewName,
    UINT     uLen,
    bool    *nameChanged)
{
    if (!g_settings.legacyNameMappingFix ||
        !ShouldSuppressLegacyNameMapping(pszLegacyName))
    {
        if (COpenControlPanel__MapLegacyName_orig) {
            return COpenControlPanel__MapLegacyName_orig(
                pThis, pszLegacyName, pszNewName, uLen, nameChanged);
        }
        return false;
    }

    // Tell the caller the name was NOT changed — this forces Explorer to use
    // the original legacy Control Panel path, but only for the whitelisted
    // legacy names above.
    if (nameChanged) *nameChanged = false;
    if (pszNewName && uLen > 0) *pszNewName = L'\0';
    Wh_Log(L"[MAP-LEGACY] Suppressed mapping for: %s",
           pszLegacyName ? pszLegacyName : L"(null)");
    return false;
}
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
        if (c == L'"') { inQuotes = !inQuotes; }
        else if (c == L' ' && !inQuotes) {
            if (!current.empty()) { tokens.push_back(current); current.clear(); }
        } else { current += c; }
    }
    if (!current.empty()) tokens.push_back(current);
    if (tokens.size() != 2) return false;
    std::wstring exe = BaseNameLower(tokens[0]);
    if (exe != L"control.exe" && exe != L"control") return false;
    std::wstring arg = ToLower(tokens[1]);
    return (arg == L"system" || arg == L"microsoft.system");
}

static std::wstring ExtractExplorerLaunchUri(const std::wstring& cmdLine) {
    size_t i = 0, n = cmdLine.size();
    while (i < n && cmdLine[i] == L' ') i++;

    std::wstring exeToken;
    if (i < n && cmdLine[i] == L'"') {
        size_t end = cmdLine.find(L'"', i + 1);
        if (end == std::wstring::npos) return L"";
        exeToken = cmdLine.substr(i + 1, end - i - 1);
        i = end + 1;
    } else {
        size_t start = i;
        while (i < n && cmdLine[i] != L' ') i++;
        exeToken = cmdLine.substr(start, i - start);
    }

    if (BaseNameLower(exeToken) != L"explorer.exe") return L"";

    while (i < n && cmdLine[i] == L' ') i++;
    std::wstring rest = cmdLine.substr(i);
    while (!rest.empty() && rest.back() == L' ') rest.pop_back();
    if (rest.size() >= 2 && rest.front() == L'"' && rest.back() == L'"') {
        rest = rest.substr(1, rest.size() - 2);
    }
    if (rest.empty()) return L"";

    const wchar_t* restC = rest.c_str();
    if (ToLower(restC).find(L"ms-settings:") != std::wstring::npos) return NormalizeUri(rest);
    return L"";
}

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
    
    std::wstring uri;
    const wchar_t* f = pei->lpFile;
    const wchar_t* p = pei->lpParameters;

    if (f && ToLower(f).find(L"ms-settings:") != std::wstring::npos) uri = NormalizeUri(f);
    else if (p && ToLower(p).find(L"ms-settings:") != std::wstring::npos) uri = NormalizeUri(p);
    else if (f && ToLower(f).find(L"shell:::") != std::wstring::npos) uri = ToLower(f);
    else if (p && ToLower(p).find(L"shell:::") != std::wstring::npos) uri = ToLower(p);

    if (uri == L"ms-settings:taskbar")
        return ShellExecuteExW_orig(pei);

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

HINSTANCE WINAPI ShellExecuteW_hook(HWND hwnd, LPCWSTR op, LPCWSTR file, LPCWSTR params, LPCWSTR dir, INT show) {
    if (IsChildProcess()) return ShellExecuteW_orig(hwnd, op, file, params, dir, show);
    HookGuard guard;
    if (guard.IsReentrant()) return ShellExecuteW_orig(hwnd, op, file, params, dir, show);
    if (!g_settings.enableRedirects) return ShellExecuteW_orig(hwnd, op, file, params, dir, show);

    if (IsControlSystemParams(file, params)) {
        LaunchTarget(g_isWin11 ? L"sysdm.cpl" : SYSTEM_PROPS_CLSID);
        return (HINSTANCE)33;
    }
    
    std::wstring uri;
    if (file && ToLower(file).find(L"ms-settings:") != std::wstring::npos) uri = NormalizeUri(file);
    else if (params && ToLower(params).find(L"ms-settings:") != std::wstring::npos) uri = NormalizeUri(params);
    else if (file && ToLower(file).find(L"shell:::") != std::wstring::npos) uri = ToLower(file);
    else if (params && ToLower(params).find(L"shell:::") != std::wstring::npos) uri = ToLower(params);

    if (uri == L"ms-settings:taskbar")
        return ShellExecuteW_orig(hwnd, op, file, params, dir, show);

    if (!uri.empty()) {
        auto result = ResolveUri(uri, hwnd);
        if (result.intercept) {
            if (!result.target.empty()) LaunchTarget(result.target);
            return (HINSTANCE)33;
        }
    }
    return ShellExecuteW_orig(hwnd, op, file, params, dir, show);
}

BOOL WINAPI CreateProcessW_hook(LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
                                 LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                 BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
                                 LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo,
                                 LPPROCESS_INFORMATION lpProcessInformation) {
    if (IsChildProcess()) return CreateProcessW_orig(lpApplicationName, lpCommandLine, lpProcessAttributes, 
        lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, 
        lpStartupInfo, lpProcessInformation);
    HookGuard guard;
    if (guard.IsReentrant()) return CreateProcessW_orig(lpApplicationName, lpCommandLine, lpProcessAttributes, 
        lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, 
        lpStartupInfo, lpProcessInformation);
    if (!g_settings.enableRedirects || g_settings.uiOnlyRedirects) return CreateProcessW_orig(lpApplicationName, 
        lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, 
        lpCurrentDirectory, lpStartupInfo, lpProcessInformation);

    if (lpCommandLine) {
        std::wstring cmdLine(lpCommandLine);
        if (IsControlSystemCommand(cmdLine)) {
            LaunchTarget(g_isWin11 ? L"sysdm.cpl" : SYSTEM_PROPS_CLSID);
            if (lpProcessInformation) ZeroMemory(lpProcessInformation, sizeof(PROCESS_INFORMATION));
            SetLastError(ERROR_SUCCESS);
            return TRUE;
        }

        std::wstring uri = ExtractExplorerLaunchUri(cmdLine);
        if (!uri.empty()) {
            auto result = ResolveUri(uri, nullptr);
            if (result.intercept) {
                if (!result.target.empty()) LaunchTarget(result.target);
                if (lpProcessInformation) ZeroMemory(lpProcessInformation, sizeof(PROCESS_INFORMATION));
                SetLastError(ERROR_SUCCESS);
                return TRUE;
            }
        }
    }
    return CreateProcessW_orig(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, 
        bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}

static bool TryInstallPniduiHook() {
    std::lock_guard<std::mutex> lk(g_pniduiHookMutex);
    
    if (g_pniduiHookInstalled) {
        return true;
    }
    
    HMODULE hMod = GetModuleHandleW(L"pnidui.dll");
    if (!hMod) {
        hMod = LoadLibraryExW(L"pnidui.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!hMod) {
            return false;
        }
    }
    
    WindhawkUtils::SYMBOL_HOOK pnidui_dll_hooks[] = {{
        {
            L"bool __cdecl ImmersiveContextMenuHelper::CanApplyOwnerDrawToMenu"
            L"(struct HMENU__ *,struct HWND__ *)"
        },
        (void**)&g_icmhOrig_pnidui,
        (void*)(ICMH_CAODTM_t)ICMH_hook_pnidui,
        false
    }};

    bool result = WindhawkUtils::HookSymbols(hMod, pnidui_dll_hooks, 1);
    if (result) {
        g_pniduiHookInstalled = true;
    }
    return result;
}

static bool g_sndVolSSOHookInstalled = false;
static bool g_shell32HooksInstalled = false;
static std::mutex g_shell32HookMutex;

static void InstallImmersiveMenuHooks() {
    if (!g_sndVolSSOHookInstalled) {
        HMODULE hMod = GetModuleHandleW(L"SndVolSSO.dll");
        if (!hMod) hMod = LoadLibraryExW(L"SndVolSSO.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        
        if (hMod) {
            WindhawkUtils::SYMBOL_HOOK sndVolSSO_dll_hooks[] = {{
                {
                    L"bool __cdecl ImmersiveContextMenuHelper::CanApplyOwnerDrawToMenu"
                    L"(struct HMENU__ *,struct HWND__ *)"
                },
                (void**)&g_icmhOrig_SndVolSSO,
                (void*)(ICMH_CAODTM_t)ICMH_hook_SndVolSSO,
                false
            }};

            if (WindhawkUtils::HookSymbols(hMod, sndVolSSO_dll_hooks, 1)) {
                g_sndVolSSOHookInstalled = true;
            }
        }
    }

    if (!g_pniduiHookInstalled) {
        TryInstallPniduiHook();
    }
}

// Combines all shell32.dll hooks into a single stable HookSymbols call per the
// Windhawk API best practice. The hook list is intentionally not conditional
// on settings or OS version, to keep Windhawk symbol caching valid.
static void InstallShell32Hooks() {
    std::lock_guard<std::mutex> lk(g_shell32HookMutex);
    if (g_shell32HooksInstalled) return;

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) return;

    // Register every shell32 symbol hook that this mod might ever need in one
    // stable HookSymbols call.  Don't build the array conditionally based on
    // settings/OS version: changing the symbol list breaks Windhawk's symbol
    // cache.  Runtime decisions are made inside the hook bodies instead.
    WindhawkUtils::SYMBOL_HOOK shell32_dll_hooks[] = {
        {
            {
                L"bool __cdecl CDevicesAndPrintersFolder::_HandleContextMenu"
                L"(struct HMENU__ *,unsigned int)"
            },
            (void**)&g_icmhOrig_Shell32Devices,
            (void*)(ICMH_HCM_t)ICMH_hook_Shell32Devices,
            true
        },
        {
            {
                L"private: bool __cdecl COpenControlPanel::_MapLegacyName"
                L"(unsigned short const *,unsigned short *,unsigned int,bool *)"
            },
            (void**)&COpenControlPanel__MapLegacyName_orig,
            (void*)COpenControlPanel__MapLegacyName_hook,
            true
        },
    };

    if (WindhawkUtils::HookSymbols(
            hShell32,
            shell32_dll_hooks,
            ARRAYSIZE(shell32_dll_hooks)))
    {
        g_shell32HooksInstalled = true;
        Wh_Log(L"[SHELL32-HOOKS] Installed shell32 hook set");
    }
}
static bool HasTrayBeenRecreated() {
    HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    std::lock_guard<std::mutex> lk(g_shellTrayWndMutex);

    if (!hTray) {
        g_lastShellTrayWnd = nullptr;
        return false;
    }

    if (g_lastShellTrayWnd == nullptr) {
        g_lastShellTrayWnd = hTray;
        return true;
    }

    if (hTray != g_lastShellTrayWnd) {
        g_lastShellTrayWnd = hTray;
        return true;
    }

    return false;
}
static void ReinitializeTrayRedirect() {
    RemoveTraySubclass();

    {
        std::lock_guard<std::mutex> lk(g_trayDllInfoMutex);
        g_sndVolSSOBase = nullptr;
        g_sndVolSSOEnd  = nullptr;
        g_pniduiBase    = nullptr;
        g_pniduiEnd     = nullptr;
    }

    if (g_settings.redirectSystemTray) {
        SetupTraySubclass();
    }

    InstallImmersiveMenuHooks();
    InstallShell32Hooks();
}

static void PerformBackgroundInit(bool skipSleep = false) {
    if (!skipSleep) {
        Sleep(200);
    }

    InstallImmersiveMenuHooks();
    InstallShell32Hooks();

    if (g_isWin11) {
        InstallAAMHook();
    }

}

static DWORD WINAPI TraySubclassWatchdogThread(LPVOID) {
    PerformBackgroundInit();

    const int   FAST_PHASE_CHECKS   = 60;
    const DWORD FAST_INTERVAL_MS    = 500;
    const DWORD SLOW_INTERVAL_MS    = 3000;

    int tick = 0;
    while (true) {
        DWORD interval = tick < FAST_PHASE_CHECKS ? FAST_INTERVAL_MS : SLOW_INTERVAL_MS;
        if (WaitForSingleObject(g_stopEvent, interval) == WAIT_OBJECT_0) break;

        tick++;

        if (HasTrayBeenRecreated()) {
            ReinitializeTrayRedirect();
            continue;
        }

        if (!g_settings.redirectSystemTray) continue;

        bool needSetup = false;
        {
            std::lock_guard<std::mutex> lk(g_traySubclassMutex);
            if (g_hTrayToolbar && !IsWindow(g_hTrayToolbar)) {
                g_hTrayToolbar = nullptr;
            }
            needSetup = (g_hTrayToolbar == nullptr);
        }

        if (needSetup) {
            SetupTraySubclass();
        }
    }
    return 0;
}

using CreateWindowExW_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
static CreateWindowExW_t CreateWindowExW_Original = nullptr;

HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hwnd = CreateWindowExW_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    
    bool trayToolbarMissing = false;
    {
        std::lock_guard<std::mutex> lk(g_traySubclassMutex);
        trayToolbarMissing = (g_hTrayToolbar == nullptr);
    }

    if (g_settings.redirectSystemTray && hwnd && trayToolbarMissing && 
        lpClassName && !IS_INTRESOURCE(lpClassName) && 
        lpClassName[0] == L'T')
    {
        if (wcscmp(lpClassName, L"ToolbarWindow32") == 0) {
            SetupTraySubclass();
        }
    }
    
    return hwnd;
}

BOOL Wh_ModInit() {
    DetectWindowsVersion();
    LoadSettings();
    BuildChildEnvironment();
    InitMappings();

    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) hShell32 = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hShell32) return FALSE;

    FARPROC pExW = GetProcAddress(hShell32, "ShellExecuteExW");
    FARPROC pW = GetProcAddress(hShell32, "ShellExecuteW");
    if (!pExW || !pW) return FALSE;

    WindhawkUtils::SetFunctionHook((ShellExecuteExW_t)pExW, ShellExecuteExW_hook, &ShellExecuteExW_orig);
    WindhawkUtils::SetFunctionHook((ShellExecuteW_t)pW, ShellExecuteW_hook, &ShellExecuteW_orig);

    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!hKernel32) hKernel32 = LoadLibraryExW(L"kernel32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hKernel32) {
        void* pCPW = (void*)GetProcAddress(hKernel32, "CreateProcessW");
        if (pCPW) WindhawkUtils::SetFunctionHook((CreateProcessW_t)pCPW, CreateProcessW_hook, &CreateProcessW_orig);
    }

    if (IsShellProcess()) {
        WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Original);
        if (g_settings.redirectSystemTray) {
            SetupTraySubclass();
        }

        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        if (!hUser32) hUser32 = LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (hUser32) {
            FARPROC pTrackPopupMenuEx = GetProcAddress(hUser32, "TrackPopupMenuEx");
            if (pTrackPopupMenuEx) {
                WindhawkUtils::SetFunctionHook((TrackPopupMenuEx_t)pTrackPopupMenuEx, TrackPopupMenuEx_Hook, &g_origTrackPopupMenuEx);
            }
        }
        
        // Queue all symbol/vtable hooks that might be needed before Wh_ModInit
        // returns, so Windhawk can apply them as part of normal initialization
        // without explicit Wh_ApplyHookOperations calls later.
        PerformBackgroundInit(true);

        {
            std::lock_guard<std::mutex> lk(g_shellTrayWndMutex);
            g_lastShellTrayWnd = nullptr;
        }
        g_traySubclassWatchdogThread = CreateThread(nullptr, 0, TraySubclassWatchdogThread, nullptr, 0, nullptr);
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    if (g_traySubclassWatchdogThread) {
        WaitForSingleObject(g_traySubclassWatchdogThread, 3000);
        CloseHandle(g_traySubclassWatchdogThread);
        g_traySubclassWatchdogThread = nullptr;
    }
    
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }

    RemoveTraySubclass();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    InitMappings();

    if (IsShellProcess()) {
        if (g_settings.redirectSystemTray) {
            SetupTraySubclass();
        } else {
            RemoveTraySubclass();
        }
        PerformBackgroundInit(true);
    }
}
