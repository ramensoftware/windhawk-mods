// ==WindhawkMod==
// @id             settings-to-control-panel
// @name           Redirect Settings to Control Panel
// @description    This mod forces the classic Control Panel to open instead of Windows 10/11 Settings app using native components.
// @version        10.0.30
// @author         babamohammed
// @github         https://github.com/babamohammed2022
// @include        explorer.exe
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
---

## Limitations

- The system tray context menu redirect only supports the Win32 taskbar (the one from Windows 10). However, in some Windows 11 configurations if explorer is restarted the network system tray redirect might not work.
- The device & printers system tray redirect may not work on some Windows 11 configurations, as Microsoft hardcoded the redirect to the Settings app in certain shell code paths. This could change in future if correct documentation is found.

**Note**: For a better experience on Windows 11, it is recommended to pair this mod with Anixx's **[Restore the classic Personalization and other CPLs](https://windhawk.net/mods/restore-classic-cpls)** that re-enables some of the classic applets from older Windows versions.

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
  $description: "Turns the mod on or off. When disabled, Settings opens normally as usual."
- RedirectSystemTray: false
  $name: Redirect System Tray Audio/Network/Device & Printers (EXPERIMENTAL)
  $description: "If enabled, right-clicking the Audio or Network or Device & Printers icon near the clock and choosing 'Open Sound settings' or 'Open Network settings' or 'Open devices and printers' will open the classic panel instead of the Settings app."
- UIOnlyRedirects: false
  $name: Non-Invasive Mode
  $description: "Only redirects clicks made in the UI. Doesn't touch programs that open Settings in other ways."
- FallbackMode: "2"
  $name: Behavior for Unmapped Links
  $description: "What to do when a Settings page has no classic equivalent."
  $options:
  - "0": Ignore (silent fail)
  - "1": Open the Control Panel (control.exe)
  - "2": Pass through to the modern Settings application (ms-settings.exe)"
- Win11CompatibilityMode: false
  $name: Windows 11 Compatibility Mode
  $description: "Safer mode for Windows 11. When enabled, only uses proven redirects while everything else opens the standard Control Panel page as a fallback to avoid loops or other issues."
- MaxLaunchesPerUri: 3
  $name: Anti-Loop Limit (per window, every 5 seconds)
  $description: "Safety measure: if the same window gets opened too many times within a few seconds, the mod stops reopening it. Set to 0 to disable this limit."
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

// Custom IDs for tray menu redirection (TrackPopupMenuEx method)
#define TRAY_CUSTOM_ID_AUDIO    65001
#define TRAY_CUSTOM_ID_NETWORK  65002
#define TRAY_CUSTOM_ID_DEVICES  65003

// TrackPopupMenuEx hook (DLL-based fallback method)
using TrackPopupMenuEx_t = BOOL(WINAPI*)(HMENU, UINT, int, int, HWND, const TPMPARAMS*);
static TrackPopupMenuEx_t g_origTrackPopupMenuEx = nullptr;

// Set on WM_RBUTTONUP by the subclass proc so TrackPopupMenuEx knows the icon type.
static int g_trayContextType = 0;
static DWORD g_trayContextTick = 0;
static std::mutex g_trayContextMutex;
static constexpr DWORD TRAY_CONTEXT_MAX_AGE_MS = 1500;

using ICMH_CAODTM_t = bool(__fastcall*)(HMENU, HWND);
static ICMH_CAODTM_t g_icmhOrig_SndVolSSO = nullptr;
static ICMH_CAODTM_t g_icmhOrig_pnidui    = nullptr;
static ICMH_CAODTM_t g_icmhOrig_Shell32Devices = nullptr;
static bool g_pniduiHookInstalled = false;
static std::mutex g_pniduiHookMutex;
static HANDLE g_pniduiRetryThread = nullptr;
static volatile bool g_pniduiRetryStop = false;
static HANDLE g_traySubclassWatchdogThread = nullptr;
static volatile bool g_traySubclassWatchdogStop = false;
static HWND g_lastShellTrayWnd = nullptr;

static bool __fastcall ICMH_CAODTM_hook(HMENU, HWND);

// Constants
static const HINSTANCE SHELL_EXECUTE_SUCCESS = (HINSTANCE)33;
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
// NtUserTrackPopupMenuEx fallback (syscall level)
using NtUserTrackPopupMenuEx_t = BOOL(WINAPI*)(HMENU, UINT, int, int, HWND, const TPMPARAMS*);
static NtUserTrackPopupMenuEx_t g_origNtUserTrackPopupMenuEx = nullptr;

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

struct ModSettings {
    bool enableRedirects = true;
    bool redirectSystemTray = false;
    bool uiOnlyRedirects = false;
    int fallbackMode = 2;
    bool win11CompatibilityMode = false;
    int maxLaunchesPerUri = 3;
    bool comActivationRedirect = true;
    bool legacyNameMappingFix = true;
};

static ModSettings g_settings;

static bool __fastcall ICMH_CAODTM_hook(HMENU, HWND) {
    if (!g_settings.redirectSystemTray) return true;
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
    L"shell:::{ecd0924-4208-451e-8ee0-373c0956de16}",
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

static std::wstring ToLower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(), ::towlower);
    return s;
}

static HWND g_hTrayToolbar = nullptr;
static BYTE* g_sndVolSSOBase = nullptr;
static BYTE* g_sndVolSSOEnd = nullptr;
static BYTE* g_pniduiBase = nullptr;
static BYTE* g_pniduiEnd = nullptr;

static bool InitTrayDllInfo() {
    if (g_sndVolSSOBase && g_pniduiBase) return true;

    HMODULE hSndVol = GetModuleHandleW(L"SndVolSSO.dll");
    if (hSndVol) {
        MODULEINFO mi{};
        if (GetModuleInformation(GetCurrentProcess(), hSndVol, &mi, sizeof(mi))) {
            g_sndVolSSOBase = (BYTE*)mi.lpBaseOfDll;
            g_sndVolSSOEnd = g_sndVolSSOBase + mi.SizeOfImage;
        }
    }

    HMODULE hPniDui = GetModuleHandleW(L"pnidui.dll");
    if (hPniDui) {
        MODULEINFO mi{};
        if (GetModuleInformation(GetCurrentProcess(), hPniDui, &mi, sizeof(mi))) {
            g_pniduiBase = (BYTE*)mi.lpBaseOfDll;
            g_pniduiEnd = g_pniduiBase + mi.SizeOfImage;
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

    if (g_sndVolSSOBase && addr >= (ULONG_PTR)g_sndVolSSOBase && addr < (ULONG_PTR)g_sndVolSSOEnd)
        return 1; // Audio
    if (g_pniduiBase && addr >= (ULONG_PTR)g_pniduiBase && addr < (ULONG_PTR)g_pniduiEnd)
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
    if (g_hTrayToolbar) return;
    if (!InitTrayDllInfo()) return;
    HWND hToolbar = FindTrayToolbar();
    if (!hToolbar) return;
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(hToolbar, TrayToolbarSubclassProc, 0)) {
        g_hTrayToolbar = hToolbar;
    }
}

static void RemoveTraySubclass() {
    if (g_hTrayToolbar) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_hTrayToolbar, TrayToolbarSubclassProc);
        g_hTrayToolbar = nullptr;
    }
}

static void* GetReturnAddress() {
    void* stackTrace[3];
    WORD frames = CaptureStackBackTrace(0, 3, stackTrace, NULL);
    if (frames >= 3) return stackTrace[2];
    return nullptr;
}

static bool IsAddressInModule(void* address, const wchar_t* moduleName) {
    HMODULE hModule = nullptr;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)address, &hModule)) {
        wchar_t path[MAX_PATH];
        if (GetModuleFileNameW(hModule, path, MAX_PATH))
            return (wcsstr(path, moduleName) != nullptr);
    }
    return false;
}
BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, const TPMPARAMS* lptpm) {
    if (!g_settings.redirectSystemTray || !g_settings.enableRedirects)
        return g_origTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);

    HookGuard guard;
    if (guard.IsReentrant())
        return g_origTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);

    // --- Primary: subclass flag set on WM_RBUTTONUP (language-independent) ---
    int contextType;
    {
        std::lock_guard<std::mutex> lk(g_trayContextMutex);
        contextType = g_trayContextType;
        DWORD tick = g_trayContextTick;
        g_trayContextType = 0;
        g_trayContextTick = 0;
        if (contextType != 0 && GetTickCount() - tick > TRAY_CONTEXT_MAX_AGE_MS) {
            contextType = 0;
        }
    }

    bool isAudioMenu   = (contextType == 1);
    bool isNetworkMenu = (contextType == 2);
    bool isDeviceMenu  = false;

    // --- Fallback: DLL return-address detection (language-independent) ---
    if (!isAudioMenu && !isNetworkMenu) {
        void* retAddr = GetReturnAddress();
        int itemCount = GetMenuItemCount(hMenu);
        if (itemCount > 0) {
            if (IsAddressInModule(retAddr, L"SndVolSSO.dll")) {
                if (itemCount <= 6) isAudioMenu = true;
            }
            else if (IsAddressInModule(retAddr, L"pnidui.dll")) {
                if (itemCount <= 6) isNetworkMenu = (itemCount >= 2 && itemCount <= 5);
            }
            else if (IsAddressInModule(retAddr, L"dxgi.dll")) {
                // dxgi.dll handles both Network flyout (IDs 3107/3109) and 
                // Device/Safely Remove Hardware flyout (ID 215 = "Open Devices and Printers")
                if (itemCount == 2 && GetMenuItemID(hMenu, 0) == 3107 && GetMenuItemID(hMenu, 1) == 3109) {
                    isNetworkMenu = true;
                }
                // Check for device menu: item ID 215 is the language-independent
                // identifier for "Open Devices and Printers"
                else if (GetMenuItemID(hMenu, 0) == 215) {
                    isDeviceMenu = true;
                    Wh_Log(L"[TRAY-HOOK] Device menu detected via dxgi.dll + ID 215");
                }
            }
            else if (IsAddressInModule(retAddr, L"shell32.dll")) {
                // NUOVO: Rilevamento shell32.dll per Win11 23H2
                // Cerca l'ID 215 in qualsiasi posizione del menu
                for (int i = 0; i < itemCount; i++) {
                    UINT itemId = GetMenuItemID(hMenu, i);
                    if (itemId == 215) {
                        isDeviceMenu = true;
                        Wh_Log(L"[TRAY-HOOK] Device menu detected via shell32.dll + ID 215 at index %d", i);
                        break;
                    }
                }
            }
            else if (IsAddressInModule(retAddr, L"hotplug.dll")) {
                // Fallback for older Windows versions where hotplug.dll handles the menu
                isDeviceMenu = true;
                Wh_Log(L"[TRAY-HOOK] Device menu detected via hotplug.dll");
            }
        }
    }

    if (!isAudioMenu && !isNetworkMenu && !isDeviceMenu)
        return g_origTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);

    int itemCount = GetMenuItemCount(hMenu);
    const wchar_t* menuKind = isAudioMenu ? L"AUDIO" : (isNetworkMenu ? L"NETWORK" : L"DEVICE");
    Wh_Log(L"[TRAY-HOOK] %s menu, %d items", menuKind, itemCount);

    // Find target item without any text matching.
    // Audio: first item.
    // Network: last non-separator item.
    // Device: item with ID 215 ("Open Devices and Printers") — language-independent.
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
        // Find item with ID 215 — the language-independent ID for "Open Devices and Printers"
        for (int i = 0; i < itemCount; i++) {
            if (GetMenuItemID(hMenu, i) == 215) {
                targetIndex = i;
                break;
            }
        }
        // Fallback: if ID 215 not found (unlikely), use first non-separator item
        if (targetIndex == -1) {
            for (int i = 0; i < itemCount; i++) {
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
    }
    
    if (targetIndex == -1) {
        Wh_Log(L"[TRAY-HOOK] No valid menu item found for %s menu", menuKind);
        return g_origTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);
    }

    Wh_Log(L"[TRAY-HOOK] %s menu target index=%d, ID=%u", menuKind, targetIndex, GetMenuItemID(hMenu, targetIndex));

    UINT originalId = GetMenuItemID(hMenu, targetIndex);

    // NOTE: we intentionally do NOT swap the item's ID (via SetMenuItemInfoW)
    // anymore. This menu item still gets its text/icon painted by pnidui.dll
    // through WM_DRAWITEM, which is keyed off the item's real ID — replacing
    // it with our own custom ID left pnidui.dll unable to look up what to
    // draw for it, so the item rendered with no text (it still worked,
    // since selection is ID-driven, but visually looked broken). Comparing
    // directly against the original ID avoids touching the item at all.
    bool callerWantedReturnCmd = (uFlags & TPM_RETURNCMD) != 0;
    uFlags |= TPM_RETURNCMD;
    BOOL result     = g_origTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);
    int selectedId  = (int)result;

    if (selectedId == (int)originalId) {
        Wh_Log(L"[TRAY-HOOK] User selected target item, redirecting");
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
// Hook a livello syscall per catturare menu che bypassano TrackPopupMenuEx
BOOL WINAPI NtUserTrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, const TPMPARAMS* lptpm) {
    // Se non è un menu della system tray, passa all'originale
    if (!g_settings.redirectSystemTray || !g_settings.enableRedirects) {
        return g_origNtUserTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);
    }
    
    HookGuard guard;
    if (guard.IsReentrant()) {
        return g_origNtUserTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);
    }
    
    // --- Rilevamento menu usando la stessa logica di TrackPopupMenuEx_Hook ---
    int contextType;
    {
        std::lock_guard<std::mutex> lk(g_trayContextMutex);
        contextType = g_trayContextType;
        DWORD tick = g_trayContextTick;
        g_trayContextType = 0;
        g_trayContextTick = 0;
        if (contextType != 0 && GetTickCount() - tick > TRAY_CONTEXT_MAX_AGE_MS) {
            contextType = 0;
        }
    }
    
    bool isAudioMenu   = (contextType == 1);
    bool isNetworkMenu = (contextType == 2);
    bool isDeviceMenu  = false;
    
    // Fallback: DLL return-address detection
    if (!isAudioMenu && !isNetworkMenu) {
        void* retAddr = GetReturnAddress();
        int itemCount = GetMenuItemCount(hMenu);
        if (itemCount > 0) {
            if (IsAddressInModule(retAddr, L"SndVolSSO.dll")) {
                if (itemCount <= 6) isAudioMenu = true;
            }
            else if (IsAddressInModule(retAddr, L"pnidui.dll")) {
                if (itemCount <= 6) isNetworkMenu = (itemCount >= 2 && itemCount <= 5);
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
            else if (IsAddressInModule(retAddr, L"hotplug.dll")) {
                isDeviceMenu = true;
            }
        }
    }
    
    if (!isAudioMenu && !isNetworkMenu && !isDeviceMenu) {
        return g_origNtUserTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);
    }
    
    // Trova l'item target
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
        if (targetIndex == -1) {
            for (int i = 0; i < itemCount; i++) {
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
    }
    
    if (targetIndex == -1) {
        return g_origNtUserTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);
    }
    
    UINT originalId = GetMenuItemID(hMenu, targetIndex);
    bool callerWantedReturnCmd = (uFlags & TPM_RETURNCMD) != 0;
    uFlags |= TPM_RETURNCMD;
    
    BOOL result = g_origNtUserTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);
    int selectedId = (int)result;
    
    if (selectedId == (int)originalId) {
        Wh_Log(L"[SYSCALL-HOOK] User selected target item, redirecting");
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

static bool IsMsSettings(const wchar_t* s) {
    if (!s) return false;
    return ToLower(s).find(L"ms-settings:") != std::wstring::npos;
}

static bool IsShellClsid(const wchar_t* s) {
    if (!s) return false;
    return ToLower(s).find(L"shell:::") != std::wstring::npos;
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

static IApplicationActivationManagerVtbl g_origAAMVtbl = {};
static bool g_aamHookInstalled = false;
static std::mutex g_aamHookMutex;

HRESULT STDMETHODCALLTYPE AAM_ActivateApplication_hook(
    IUnknown* pThis,
    LPCWSTR appUserModelId,
    LPCWSTR arguments,
    DWORD options,
    DWORD* processId)
{
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
        if (result.intercept && !result.target.empty()) {
            LaunchTarget(result.target);
            if (processId) *processId = GetCurrentProcessId();
            Wh_Log(L"[AAM-HOOK] Redirected to: %s", result.target.c_str());
            return S_OK;
        }
        Wh_Log(L"[AAM-HOOK] No mapping found, falling back to original");
    }

    // Not a Settings activation we can handle — call original
    if (g_origAAMVtbl.ActivateApplication) {
        return g_origAAMVtbl.ActivateApplication(pThis, appUserModelId, arguments, options, processId);
    }
    return E_FAIL;
}

static void InstallAAMHook() {
    std::lock_guard<std::mutex> lk(g_aamHookMutex);
    if (g_aamHookInstalled) return;

    IUnknown* pAAM = nullptr;
    HRESULT hr = CoCreateInstance(
        CLSID_ApplicationActivationManager_STC,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_IApplicationActivationManager_STC,
        (void**)&pAAM);

    if (FAILED(hr) || !pAAM) {
        Wh_Log(L"[AAM-HOOK] CoCreateInstance(CLSID_ApplicationActivationManager) failed: 0x%08X", hr);
        return;
    }

    IApplicationActivationManagerVtbl* vtbl = *(IApplicationActivationManagerVtbl**)pAAM;
    if (!vtbl || IsBadReadPtr(vtbl, sizeof(*vtbl))) {
        Wh_Log(L"[AAM-HOOK] Invalid vtable pointer");
        pAAM->Release();
        return;
    }

    // Save original vtable
    g_origAAMVtbl = *vtbl;

    // Make the vtable page writable
    DWORD oldProtect;
    if (!VirtualProtect(vtbl, sizeof(*vtbl), PAGE_READWRITE, &oldProtect)) {
        Wh_Log(L"[AAM-HOOK] VirtualProtect failed");
        pAAM->Release();
        return;
    }

    // Patch ActivateApplication entry
    vtbl->ActivateApplication = AAM_ActivateApplication_hook;

    VirtualProtect(vtbl, sizeof(*vtbl), oldProtect, &oldProtect);
    
    g_aamHookInstalled = true;
    Wh_Log(L"[AAM-HOOK] Successfully installed");
    pAAM->Release();
}

bool (*COpenControlPanel__MapLegacyName_orig)(void*, LPCWSTR, LPWSTR, UINT, bool*);
bool COpenControlPanel__MapLegacyName_hook(
    void    *pThis,
    LPCWSTR  pszLegacyName,
    LPWSTR   pszNewName,
    UINT     uLen,
    bool    *nameChanged)
{
    // Always tell the caller the name was NOT changed — this forces
    // Explorer to use the original legacy Control Panel path.
    *nameChanged = false;
    *pszNewName = L'\0';
    Wh_Log(L"[MAP-LEGACY] Suppressed mapping for: %s",
           pszLegacyName ? pszLegacyName : L"(null)");
    return false;
}

static bool InstallLegacyNameHook() {
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) {
        Wh_Log(L"[MAP-LEGACY] shell32.dll not loaded");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK shell32_dll_hook = {{
        L"private: bool __cdecl COpenControlPanel::_MapLegacyName"
        L"(unsigned short const *,unsigned short *,unsigned int,bool *)"
    },
    (void**)&COpenControlPanel__MapLegacyName_orig,
    (void*)COpenControlPanel__MapLegacyName_hook,
    false};

    if (WindhawkUtils::HookSymbols(hShell32, &shell32_dll_hook, 1)) {
        Wh_Log(L"[MAP-LEGACY] Hook installed successfully");
        return true;
    } else {
        Wh_Log(L"[MAP-LEGACY] Failed to install hook (symbol may differ on this build)");
        return false;
    }
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

    if (IsMsSettings(rest.c_str())) return NormalizeUri(rest);
    if (IsShellClsid(rest.c_str())) return ToLower(rest);
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
    if (IsMsSettings(pei->lpFile)) uri = NormalizeUri(pei->lpFile);
    else if (IsMsSettings(pei->lpParameters)) uri = NormalizeUri(pei->lpParameters);
    else if (IsShellClsid(pei->lpFile)) uri = ToLower(pei->lpFile);
    else if (IsShellClsid(pei->lpParameters)) uri = ToLower(pei->lpParameters);

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
        return SHELL_EXECUTE_SUCCESS;
    }
    
    std::wstring uri;
    if (IsMsSettings(file)) uri = NormalizeUri(file);
    else if (IsMsSettings(params)) uri = NormalizeUri(params);
    else if (IsShellClsid(file)) uri = ToLower(file);
    else if (IsShellClsid(params)) uri = ToLower(params);

    if (uri == L"ms-settings:taskbar")
        return ShellExecuteW_orig(hwnd, op, file, params, dir, show);

    if (!uri.empty()) {
        auto result = ResolveUri(uri, hwnd);
        if (result.intercept) {
            if (!result.target.empty()) LaunchTarget(result.target);
            return SHELL_EXECUTE_SUCCESS;
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

        // Handles cases like the classic Control Panel's "See also" links,
        // which Explorer launches as a new "explorer.exe ms-settings:..."
        // (or "explorer.exe shell:::{...}") process via CreateProcessW
        // instead of going through ShellExecuteW/ShellExecuteExW.
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
static volatile bool g_pniduiRetryRunning = false;


static bool TryInstallPniduiHook() {
    std::lock_guard<std::mutex> lk(g_pniduiHookMutex);
    
    if (g_pniduiHookInstalled) {
        return true;
    }
    
    HMODULE hMod = GetModuleHandleW(L"pnidui.dll");
    if (!hMod) {
        hMod = LoadLibraryExW(L"pnidui.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!hMod) {
            Wh_Log(L"[PNIDUI-HOOK] pnidui.dll not loaded yet");
            return false;
        }
    }
    
    Wh_Log(L"[PNIDUI-HOOK] pnidui.dll loaded at 0x%p", hMod);

    WindhawkUtils::SYMBOL_HOOK pnidui_dll_hooks[] = {{
        {
            L"bool "
#ifdef _WIN64
            L"__cdecl"
#else
            L"__stdcall"
#endif
            L" ImmersiveContextMenuHelper::CanApplyOwnerDrawToMenu"
            L"(struct HMENU__ *,struct HWND__ *)"
        },
        (void**)&g_icmhOrig_pnidui,
        (void*)(ICMH_CAODTM_t)ICMH_CAODTM_hook,
        false
    }};

    bool result = WindhawkUtils::HookSymbols(hMod, pnidui_dll_hooks, 1);
    if (result) {
        Wh_Log(L"[PNIDUI-HOOK] Successfully installed pnidui.dll hook");
        g_pniduiHookInstalled = true;
    } else {
        Wh_Log(L"[PNIDUI-HOOK] Failed to install pnidui.dll hook");
    }
    return result;
}

static DWORD WINAPI PniduiRetryThread(LPVOID) {
    if (g_pniduiRetryRunning) {
        Wh_Log(L"[PNIDUI-HOOK] Retry thread already running, exiting");
        return 0;
    }
    g_pniduiRetryRunning = true;
    
    Wh_Log(L"[PNIDUI-HOOK] Retry thread started - waiting for pnidui.dll to load");
    
    const int MAX_WAIT_CHECKS = 60;
    const DWORD CHECK_INTERVAL = 500;
    bool dllLoaded = false;
    
    for (int i = 0; i < MAX_WAIT_CHECKS && !g_pniduiRetryStop; i++) {
        if (GetModuleHandleW(L"pnidui.dll") != nullptr) {
            dllLoaded = true;
            Wh_Log(L"[PNIDUI-HOOK] pnidui.dll detected after %dms", i * CHECK_INTERVAL);
            break;
        }
        Sleep(CHECK_INTERVAL);
    }
    
    if (!dllLoaded) {
        Wh_Log(L"[PNIDUI-HOOK] pnidui.dll not loaded after timeout, giving up");
        g_pniduiRetryRunning = false;
        return 0;
    }
    
    if (TryInstallPniduiHook()) {
        Wh_Log(L"[PNIDUI-HOOK] Hook installed successfully after waiting for DLL");
    } else {
        Wh_Log(L"[PNIDUI-HOOK] Failed to install hook even though DLL is loaded");
    }
    
    g_pniduiRetryRunning = false;
    Wh_Log(L"[PNIDUI-HOOK] Retry thread exiting");
    return 0;
}

static void InstallImmersiveMenuHooks() {
    struct DllHook {
        const wchar_t*   dll;
        ICMH_CAODTM_t*  orig;
    } targets[] = {
        { L"SndVolSSO.dll", &g_icmhOrig_SndVolSSO },
    };

    for (auto& t : targets) {
        HMODULE hMod = LoadLibraryExW(t.dll, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!hMod) continue;

        WindhawkUtils::SYMBOL_HOOK sndVolSSO_dll_hooks[] = {{
            {
                L"bool "
#ifdef _WIN64
                L"__cdecl"
#else
                L"__stdcall"
#endif
                L" ImmersiveContextMenuHelper::CanApplyOwnerDrawToMenu"
                L"(struct HMENU__ *,struct HWND__ *)"
            },
            (void**)t.orig,
            (void*)(ICMH_CAODTM_t)ICMH_CAODTM_hook,
            false
        }};

        WindhawkUtils::HookSymbols(hMod, sndVolSSO_dll_hooks, 1);
    }

    if (!TryInstallPniduiHook()) {
        if (!g_pniduiRetryRunning && !g_pniduiRetryThread) {
            g_pniduiRetryStop = false;
            g_pniduiRetryThread = CreateThread(nullptr, 0, PniduiRetryThread, nullptr, 0, nullptr);
            if (g_pniduiRetryThread) {
                Wh_Log(L"[PNIDUI-HOOK] Retry thread created");
            } else {
                Wh_Log(L"[PNIDUI-HOOK] Failed to create retry thread");
            }
        } else {
            Wh_Log(L"[PNIDUI-HOOK] Retry thread already running or hook already installed");
        }
    }

    if (g_isWin11) {
        HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
        if (hShell32) {
            WindhawkUtils::SYMBOL_HOOK shell32_dll_hooks[] = {{
                {
                    L"bool "
#ifdef _WIN64
                    L"__cdecl"
#else
                    L"__stdcall"
#endif
                    L" CDevicesAndPrintersFolder::_HandleContextMenu"
                    L"(struct HMENU__ *,unsigned int)"
                },
                (void**)&g_icmhOrig_Shell32Devices,
                (void*)(ICMH_CAODTM_t)ICMH_CAODTM_hook,
                false
            }};
            
            WindhawkUtils::HookSymbols(hShell32, shell32_dll_hooks, 1);
        }
    }
}

static void InstallSyscallFallback() {
    HMODULE hWin32u = GetModuleHandleW(L"win32u.dll");
    if (!hWin32u) {
        hWin32u = LoadLibraryW(L"win32u.dll");
        if (!hWin32u) {
            Wh_Log(L"[SYSCALL-HOOK] win32u.dll not found");
            return;
        }
    }
    
    FARPROC pNtUserTrackPopupMenuEx = GetProcAddress(hWin32u, "NtUserTrackPopupMenuEx");
    if (!pNtUserTrackPopupMenuEx) {
        Wh_Log(L"[SYSCALL-HOOK] NtUserTrackPopupMenuEx not found by name");
        return;
    }
    
    if (Wh_SetFunctionHook((void*)pNtUserTrackPopupMenuEx, 
                          (void*)NtUserTrackPopupMenuEx_Hook, 
                          (void**)&g_origNtUserTrackPopupMenuEx)) {
        Wh_Log(L"[SYSCALL-HOOK] NtUserTrackPopupMenuEx hooked successfully");
    } else {
        Wh_Log(L"[SYSCALL-HOOK] Failed to hook NtUserTrackPopupMenuEx");
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
// ---------------------------------------------------------------------------
// Tray subclass watchdog
// ---------------------------------------------------------------------------

static bool HasTrayBeenRecreated() {
    HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hTray) {
        return false;
    }

    bool recreated = (g_lastShellTrayWnd != nullptr && hTray != g_lastShellTrayWnd);
    g_lastShellTrayWnd = hTray;
    return recreated;
}

static void ReinitializeTrayRedirect() {
    Wh_Log(L"[TRAY-WATCHDOG] Reinitializing tray redirect state");

    RemoveTraySubclass();

    g_sndVolSSOBase = nullptr;
    g_sndVolSSOEnd  = nullptr;
    g_pniduiBase    = nullptr;
    g_pniduiEnd     = nullptr;

    {
        std::lock_guard<std::mutex> lk(g_pniduiHookMutex);
        g_pniduiHookInstalled = false;
    }

    if (g_settings.redirectSystemTray) {
        SetupTraySubclass();
    }

    if (!TryInstallPniduiHook()) {
        if (!g_pniduiRetryRunning && !g_pniduiRetryThread) {
            g_pniduiRetryStop = false;
            g_pniduiRetryThread = CreateThread(nullptr, 0, PniduiRetryThread, nullptr, 0, nullptr);
        }
    }

    InstallImmersiveMenuHooks();
}

static DWORD WINAPI TraySubclassWatchdogThread(LPVOID) {
    Wh_Log(L"[TRAY-WATCHDOG] Watchdog thread started");

    const int   FAST_PHASE_CHECKS   = 60;
    const DWORD FAST_INTERVAL_MS    = 500;
    const DWORD SLOW_INTERVAL_MS    = 3000;

    int tick = 0;
    while (!g_traySubclassWatchdogStop) {
        Sleep(tick < FAST_PHASE_CHECKS ? FAST_INTERVAL_MS : SLOW_INTERVAL_MS);
        tick++;
        if (g_traySubclassWatchdogStop) break;

        if (HasTrayBeenRecreated()) {
            Wh_Log(L"[TRAY-WATCHDOG] Shell_TrayWnd recreation detected, reinitializing");
            ReinitializeTrayRedirect();
            continue;
        }

        if (!g_settings.redirectSystemTray) continue;

        if (g_hTrayToolbar && !IsWindow(g_hTrayToolbar)) {
            Wh_Log(L"[TRAY-WATCHDOG] Tray toolbar handle no longer valid, resetting");
            g_hTrayToolbar = nullptr;
        }

        if (!g_hTrayToolbar) {
            SetupTraySubclass();
        }
    }

    Wh_Log(L"[TRAY-WATCHDOG] Watchdog thread exiting");
    return 0;
}
HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hwnd = CreateWindowExW_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    
    if (g_settings.redirectSystemTray && hwnd && !g_hTrayToolbar && lpClassName && !IS_INTRESOURCE(lpClassName)) {
        if (wcscmp(lpClassName, L"ToolbarWindow32") == 0) {
            SetupTraySubclass();
        }
    }
    
    return hwnd;
}


BOOL Wh_ModInit() {
    Wh_Log(L"Initializing the Redirect Settings to Control Panel mod...");

    DetectWindowsVersion();
    LoadSettings();
    BuildChildEnvironment();
    InitMappings();

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32) return FALSE;

    FARPROC pExW = GetProcAddress(hShell32, "ShellExecuteExW");
    FARPROC pW = GetProcAddress(hShell32, "ShellExecuteW");
    if (!pExW || !pW) return FALSE;

    Wh_SetFunctionHook((void*)pExW, (void*)ShellExecuteExW_hook, (void**)&ShellExecuteExW_orig);
    Wh_SetFunctionHook((void*)pW, (void*)ShellExecuteW_hook, (void**)&ShellExecuteW_orig);

    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!hKernel32) hKernel32 = LoadLibraryW(L"kernel32.dll");
    if (hKernel32) {
        FARPROC pCPW = GetProcAddress(hKernel32, "CreateProcessW");
        if (pCPW) Wh_SetFunctionHook((void*)pCPW, (void*)CreateProcessW_hook, (void**)&CreateProcessW_orig);
    }

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original);
    if (g_settings.redirectSystemTray) {
        SetupTraySubclass();
    }

    InstallImmersiveMenuHooks();    
    InstallSyscallFallback();

    if (g_settings.comActivationRedirect && g_isWin11) {
        InstallAAMHook();
    }

    if (g_settings.legacyNameMappingFix) {
        InstallLegacyNameHook();
    }

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) hUser32 = LoadLibraryW(L"user32.dll");
    if (hUser32) {
        void* pTrackPopupMenuEx = (void*)GetProcAddress(hUser32, "TrackPopupMenuEx");
        if (pTrackPopupMenuEx) {
            Wh_SetFunctionHook(pTrackPopupMenuEx, (void*)TrackPopupMenuEx_Hook, (void**)&g_origTrackPopupMenuEx);
        }
    }
    // Initialize the Shell_TrayWnd reference for the watchdog
    g_lastShellTrayWnd = FindWindowW(L"Shell_TrayWnd", nullptr);

    g_traySubclassWatchdogStop = false;
    g_traySubclassWatchdogThread = CreateThread(nullptr, 0, TraySubclassWatchdogThread, nullptr, 0, nullptr);
    if (g_traySubclassWatchdogThread) {
        Wh_Log(L"[TRAY-WATCHDOG] Watchdog thread created");
    } else {
        Wh_Log(L"[TRAY-WATCHDOG] Failed to create watchdog thread");
    }
    return TRUE;
}

void Wh_ModUninit() {
    if (g_traySubclassWatchdogThread) {
        g_traySubclassWatchdogStop = true;
        Wh_Log(L"[TRAY-WATCHDOG] Stopping watchdog thread...");
        DWORD waitResult = WaitForSingleObject(g_traySubclassWatchdogThread, 3000);
        if (waitResult == WAIT_TIMEOUT) {
            Wh_Log(L"[TRAY-WATCHDOG] Timeout, terminating thread");
            TerminateThread(g_traySubclassWatchdogThread, 0);
        }
        CloseHandle(g_traySubclassWatchdogThread);
        g_traySubclassWatchdogThread = nullptr;
    }
    if (g_pniduiRetryThread) {
        g_pniduiRetryStop = true;
        Wh_Log(L"[PNIDUI-HOOK] Stopping retry thread...");
        DWORD waitResult = WaitForSingleObject(g_pniduiRetryThread, 3000);
        if (waitResult == WAIT_TIMEOUT) {
            Wh_Log(L"[PNIDUI-HOOK] Retry thread timeout, terminating");
            TerminateThread(g_pniduiRetryThread, 0);
        }
        CloseHandle(g_pniduiRetryThread);
        g_pniduiRetryThread = nullptr;
        g_pniduiRetryRunning = false;
    }
    
    RemoveTraySubclass();
}

void Wh_ModSettingsChanged() {
    RemoveTraySubclass();
    g_sndVolSSOBase = nullptr;
    g_sndVolSSOEnd = nullptr;
    g_pniduiBase = nullptr;
    g_pniduiEnd = nullptr;
    
    {
        std::lock_guard<std::mutex> lk(g_pniduiHookMutex);
        g_pniduiHookInstalled = false;
    }
    
    LoadSettings();
    InitMappings();
    if (g_settings.redirectSystemTray) {
        SetupTraySubclass();
        InstallImmersiveMenuHooks();
    }

    // Re-install experimental hooks if settings changed
    if (g_settings.comActivationRedirect && g_isWin11) {
        InstallAAMHook();
    }

    if (g_settings.legacyNameMappingFix) {
        InstallLegacyNameHook();
    }
}
