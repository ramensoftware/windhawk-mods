// ==WindhawkMod==
// @id              windows-update-control-panel-restorer
// @name            Windows Update Control Panel Page Restorer
// @description     This mod restores the Windows Update Control Panel page in Windows 10 and Windows 11
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @include         control.exe
// @architecture    x86-64
// @compilerOptions -lwininet -ladvapi32 -lole32 -luuid -loleaut32 -lgdi32 -lcomctl32 -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- Language: auto
  $name: Language
  $description: This setting changes the language shown on the restored page. Auto detects the system language automatically, otherwise English is used as the fallback if a language is not recognized.
  $options:
    - auto: Auto (detect system language)
    - en: English
    - it: Italiano
    - es: Español
    - fr: Français
    - tr: Türkçe
    - ru: Русский
    - pt: Português
    - zh: 中文
    - pl: Polski
    - nl: Nederlands
- ShowServiceNotice: true
  $name: Show recreated interface
  $description: This setting shows the mod recreated interface, a best-effort recreation of the classic Windows Update Control Panel page. Disable this to hide the recreated interface.
- UpdatePageSkin: windows7
  $name: Update page skin
  $description: This setting chooses the status banner and applet icon skin. Windows 7 keeps the current shield-style status icons and uses the supplied applet logo, Windows 8.1 uses the included Windows Update icon. The available-updates and disabled-service fallback notices are unchanged.
  $options:
    - windows7: Windows 7 (current)
    - windows81: Windows 8.1
- ShowAvailableUpdates: true
  $name: Show available-updates banner
  $description: This setting shows the updates available state, an amber and orange strip with an exclamation shield, when Windows reports pending available updates. Enabled by default.
- LinkSystemSettingsText: false
  $name: Link system settings text
  $description: This setting makes the system settings part of the recommendation text a blue link that opens Windows Update in the Settings app. Disabled by default.
- RemoveLegacyBrokenOption: true
  $name: Remove Legacy Broken Option Fix
  $description: When Windows Update is unavailable or disabled, the restored page shows a legacy red Check for updates for your PC box whose button cannot work, because the service is stopped. With this enabled, that broken legacy box is removed so only the Turn on automatic updating box remains, plus the blue settings link when the recreated interface is shown. Disable to keep the legacy box.
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
## Windows Update Control Panel Restorer

This mod adds a best-effort classic Windows Update page back to Control Panel on
Windows 10 and Windows 11 by reimplementing it where possible and by partially recreating the legacy interface. The mod utilizes a Windows 8.1 UI dll with the modern
Windows Update backend/status layer, without replacing system files or writing
real Control Panel registration keys.

This is a reimplementation, not the original Windows Update client. Some buttons
and small visual details are limited due to modern Windows versions' architecutre, and more details may be
improved in future versions.

The mod has been tested on Windows 10 1809, Windows 10 21H2, Windows 11 24H2 and Windows 11 25H2.

## **Screenshot**

![Windows Update Control Panel Restorer](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/windowsupdate.PNG)

## **Key Features**

- Classic Windows Update interface with real-time status.
- Option to choose between Windows 7 or Windows 8.1 visual style.
- Clear notification when Windows Update is unavailable.
- Optional banner for available updates and link to settings.
- Option to hide outdated "Check for updates" box when the service is disabled.
- Read-only view of classic update settings; changes must be made via the modern Settings app.
- **Multilingual UI**: English, Italian, Spanish, French, Turkish, Russian, Portuguese, Chinese, Polish, Dutch, or auto-detect.
- **"Check for updates" feature**: click the sidebar link to run a 12-second scan in a small window with a progress bar. The main page remains stable; results appear automatically upon completion.
- **"Updates FAQ" link**: opens a compact window with ten common questions and answers about Windows updates.
- **Native sidebar**: preserves the original Windows Control Panel sidebar without adding or modifying external elements.


## **Notes**

- **The Control Panel entry appears immediately.** Registration is published
  when the mod is enabled and is completely independent of the payload
  download, so "Windows Update" is in Control Panel right away - on first run,
  with no internet connection, and even if the download fails. Only the classic
  page's own content needs the payload; if it is not ready yet, opening the item
  shows a translated explanation and offers to open Windows Update in Settings.
- After the first successful setup the verified payload is cached and reused
  with no network access at all, so the page works fully offline afterwards.
- Installing updates is still handled by the modern Settings app.
- The "last checked" and "updates were installed" times come from Windows
  Update's recorded registry timestamps when those values are available.
- **Why the mod downloads a DLL:** The restored page is the real Windows 8.1
  Windows Update Control Panel UI (wucltux.dll), loaded privately from a
  verified copy obtained from the Microsoft Symbol Server. That DLL is a
  **necessary dependency**: it is what renders the classic page, and the whole
  point of this mod is to bring that original page back. Without it there is
  nothing to show, so the mod cannot function offline or without this payload.
  The download is a one-time fetch (retried up to a few times), the file is
  pinned to a known SHA-256 and its PE machine type is validated before it is
  ever loaded, and it is kept as a private copy outside System32 and nothing is
  installed to or replaced in the operating system. The Microsoft Symbol Server
  URL is tied to this exact historical build; if Microsoft removes that file,
  first-time setup will no longer work unless the verified payload was cached.

## **Stability and safety design**

- **Nothing is written to the real registry or to system files.** Every key and
  value is served from an in-memory virtualization layer, and each synthetic
  handle is backed by a genuine per-process *volatile* key that is deleted on
  unload, so no trace remains.
- **Setup never blocks the shell.** The download runs on a background thread,
  is serialized across processes with a named mutex, is bounded per call, per
  attempt and by an overall deadline, and aborts immediately on shutdown, so a
  slow or captive-portal network can never hang Explorer or delay sign-out.
- **Every failure is contained.** The setup worker, the language rebuild worker
  and both mod entry points have hard exception boundaries, so no failure can
  reach `std::terminate` and take Explorer down. Any step that fails degrades to
  a working fallback instead of an error.
- **Payload integrity is pinned.** The download goes to a private temporary file
  that is size-, PE-machine- and SHA-256-checked against a pinned digest before
  it is atomically moved into place and loaded from outside System32.
- **Clean teardown.** Disabling the mod hides the entry immediately, stops the
  workers, and removes the files it created. A file still mapped by another
  process is left alone and retried on a later load or unload - never
  force-deleted and never scheduled machine-wide.

**Credits**

- **Yvor** - Testing on Windows 10 21H2.
- **Cips** - Testing on Windows 11 25H2.
- **Allison** - Suggestions for the implementation of the native Control Panel navigation links.

If any issues are encountered, please report them to the author of the mod.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#include <windowsx.h>  // GET_Y_LPARAM
#include <wininet.h>
#include <wincrypt.h>
#include <combaseapi.h>
#include <winnls.h>
#include <winsvc.h>
#include <shlobj.h>
#include <servprov.h>
#include <shellapi.h>
#include <commctrl.h>
#include <richedit.h>   // CHARFORMAT2W / EM_SETCHARFORMAT for the FAQ RichEdit
#include <objidl.h>
#include <oaidl.h>
#include <oleauto.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <new>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <atomic>
#include <algorithm>
#include <optional>
#include <vector>
#include <windhawk_utils.h>

// Per-call diagnostic tracing is intentionally omitted from release builds so
// the ShellExecute, COM, registry and DirectUI hook paths remain lightweight.

// Use RtlGetVersion as the authoritative source. Unlike GetVersionEx and the
// version-helper macros, it is not affected by the executable's compatibility
// manifest. The registry values are read independently and can only upgrade the
// detected build; this prevents a compatibility shim from making Windows 11
// (build 22000 or later) look like Windows 10.
struct RealWindowsVersion {
    DWORD major = 0;
    DWORD minor = 0;
    DWORD build = 0;
    bool valid = false;
};

static bool ParseBuildNumber(const wchar_t* text, DWORD& build) {
    if (!text || !*text) return false;
    wchar_t* end = nullptr;
    const unsigned long value = wcstoul(text, &end, 10);
    while (end && (*end == L' ' || *end == L'\t')) ++end;
    if (!end || end == text || *end != L'\0' || value == 0) return false;
    build = static_cast<DWORD>(value);
    return true;
}

static RealWindowsVersion DetectRealWindowsVersion() {
    RealWindowsVersion result{};

    using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOEXW*);
    if (HMODULE ntdll = GetModuleHandleW(L"ntdll.dll")) {
        if (auto rtlGetVersion = reinterpret_cast<RtlGetVersion_t>(
                GetProcAddress(ntdll, "RtlGetVersion"))) {
            OSVERSIONINFOEXW version{};
            version.dwOSVersionInfoSize = sizeof(version);
            if (rtlGetVersion(&version) >= 0) {
                result.major = version.dwMajorVersion;
                result.minor = version.dwMinorVersion;
                result.build = version.dwBuildNumber;
                result.valid = result.major != 0;
            }
        }
    }

    // Query the native 64-bit CurrentVersion key as an independent fallback.
    // This mod is x64-only, but KEY_WOW64_64KEY also makes the intent explicit.
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0,
            KEY_READ | KEY_WOW64_64KEY, &key) == ERROR_SUCCESS) {
        DWORD registryMajor = 0;
        DWORD registryMinor = 0;
        DWORD type = 0;
        DWORD size = sizeof(registryMajor);
        if (RegQueryValueExW(key, L"CurrentMajorVersionNumber", nullptr, &type,
                            reinterpret_cast<LPBYTE>(&registryMajor), &size) != ERROR_SUCCESS ||
            type != REG_DWORD) {
            registryMajor = 0;
        }
        type = 0;
        size = sizeof(registryMinor);
        if (RegQueryValueExW(key, L"CurrentMinorVersionNumber", nullptr, &type,
                            reinterpret_cast<LPBYTE>(&registryMinor), &size) != ERROR_SUCCESS ||
            type != REG_DWORD) {
            registryMinor = 0;
        }

        DWORD registryBuild = 0;
        wchar_t buildText[64] = {};
        type = 0;
        size = sizeof(buildText);
        bool haveRegistryBuild =
            RegQueryValueExW(key, L"CurrentBuildNumber", nullptr, &type,
                             reinterpret_cast<LPBYTE>(buildText), &size) == ERROR_SUCCESS &&
            (type == REG_SZ || type == REG_EXPAND_SZ) &&
            ParseBuildNumber(buildText, registryBuild);
        if (!haveRegistryBuild) {
            buildText[0] = L'\0';
            type = 0;
            size = sizeof(buildText);
            haveRegistryBuild =
                RegQueryValueExW(key, L"CurrentBuild", nullptr, &type,
                                 reinterpret_cast<LPBYTE>(buildText), &size) == ERROR_SUCCESS &&
                (type == REG_SZ || type == REG_EXPAND_SZ) &&
                ParseBuildNumber(buildText, registryBuild);
        }
        RegCloseKey(key);

        // Never downgrade a valid RtlGetVersion result. A higher native registry
        // build is useful when another compatibility layer has altered the API.
        if (!result.valid && registryMajor != 0) {
            result.major = registryMajor;
            result.minor = registryMinor;
            result.valid = true;
        } else if (registryMajor > result.major) {
            result.major = registryMajor;
            result.minor = registryMinor;
        }
        if (haveRegistryBuild && registryBuild > result.build) {
            result.build = registryBuild;
            if (!result.valid && registryBuild >= 10240) {
                result.major = 10;
                result.minor = 0;
                result.valid = true;
            }
        }
    }

    return result;
}

static const RealWindowsVersion& GetRealWindowsVersion() {
    static const RealWindowsVersion version = DetectRealWindowsVersion();
    return version;
}

static bool IsWindows10() {
    const RealWindowsVersion& version = GetRealWindowsVersion();
    return version.valid && version.major == 10 &&
           version.build >= 10240 && version.build < 22000;
}

static bool IsWindows11OrLater() {
    const RealWindowsVersion& version = GetRealWindowsVersion();
    return version.valid &&
           (version.major > 10 ||
            (version.major == 10 && version.build >= 22000));
}

static bool ShellExecuteSucceeded(HINSTANCE result) {
    return reinterpret_cast<INT_PTR>(result) > 32;
}

static bool TryShellExecute(HWND hwnd, PCWSTR file, PCWSTR parameters = nullptr) {
    return ShellExecuteSucceeded(ShellExecuteW(
        hwnd, L"open", file, parameters, nullptr, SW_SHOWNORMAL));
}

// A shell namespace target is considered available only when its CLSID is
// registered. The subsequent ShellExecute result is checked too, so a removed
// or disabled page cannot strand the user on a broken classic target.
static bool IsShellClsidRegistered(PCWSTR clsid) {
    if (!clsid || !*clsid) return false;
    wchar_t keyPath[64] = {};
    if (swprintf_s(keyPath, ARRAYSIZE(keyPath), L"CLSID\\%s", clsid) < 0)
        return false;

    HKEY key = nullptr;
    const LSTATUS status = RegOpenKeyExW(
        HKEY_CLASSES_ROOT, keyPath, 0, KEY_READ, &key);
    if (status == ERROR_SUCCESS) RegCloseKey(key);
    return status == ERROR_SUCCESS;
}

static bool TryOpenRegisteredShellClsid(HWND hwnd, PCWSTR clsid) {
    if (!IsShellClsidRegistered(clsid)) return false;
    wchar_t target[64] = {};
    if (swprintf_s(target, ARRAYSIZE(target), L"shell:::%s", clsid) < 0)
        return false;
    // Keep the legacy page isolated in Explorer on Windows 10, as before.
    return TryShellExecute(hwnd, L"explorer.exe", target);
}

static void OpenSettingsWithFallback(HWND hwnd, PCWSTR primary,
                                     PCWSTR secondary, PCWSTR finalFallback) {
    if (primary && *primary && TryShellExecute(hwnd, primary)) return;
    if (secondary && *secondary &&
        (!primary || _wcsicmp(primary, secondary) != 0) &&
        TryShellExecute(hwnd, secondary)) return;
    if (finalFallback && *finalFallback &&
        (!primary || _wcsicmp(primary, finalFallback) != 0) &&
        (!secondary || _wcsicmp(secondary, finalFallback) != 0)) {
        TryShellExecute(hwnd, finalFallback);
    }
}

enum class InstalledUpdatesDestination {
    History,
    HiddenUpdates,
    UninstallUpdates,
};

static PCWSTR ModernInstalledUpdatesUri(InstalledUpdatesDestination destination) {
    switch (destination) {
        case InstalledUpdatesDestination::UninstallUpdates:
            return L"ms-settings:windowsupdate-uninstallupdates";
        case InstalledUpdatesDestination::HiddenUpdates:
            // Current Windows releases have no equivalent "restore hidden"
            // page. Windows Update home is the safest supported destination.
            return L"ms-settings:windowsupdate";
        case InstalledUpdatesDestination::History:
        default:
            return L"ms-settings:windowsupdate-history";
    }
}

static void OpenModernInstalledUpdates(HWND hwnd,
                                       InstalledUpdatesDestination destination) {
    const PCWSTR primary = ModernInstalledUpdatesUri(destination);
    // Uninstall/optional subpages vary by Windows release. Update History is the
    // closest stable fallback, followed by the Windows Update landing page.
    OpenSettingsWithFallback(hwnd, primary,
                             L"ms-settings:windowsupdate-history",
                             L"ms-settings:windowsupdate");
}

// Windows 11 always uses Settings for update history/installed-update links.
// Windows 10 retains the classic Installed Updates CLSID when it is registered;
// Settings is used if the namespace target is missing or cannot be launched.
static void OpenInstalledUpdates(HWND hwnd,
                                 InstalledUpdatesDestination destination) {
    static constexpr PCWSTR kInstalledUpdatesClsid =
        L"{D450A8A1-9568-45C7-9C0E-B4F9FB4537BD}";

    if (IsWindows11OrLater()) {
        OpenModernInstalledUpdates(hwnd, destination);
        return;
    }
    // An unknown/future version is deliberately treated as modern. Only a
    // positively identified Windows 10 build may use the legacy namespace.
    if (!IsWindows10() ||
        !TryOpenRegisteredShellClsid(hwnd, kInstalledUpdatesClsid)) {
        OpenModernInstalledUpdates(hwnd, destination);
    }
}

// Security and Maintenance has no complete Settings equivalent. On Windows 11
// (and on an unknown/future build), avoid control.exe entirely and open Windows
// Security. On Windows 10, preserve the classic page only when its CLSID exists;
// otherwise use Settings. This also avoids the OlderUI.dll/control.exe failure
// reported when a removed classic page is invoked on Windows 11.
static void OpenSecurityAndMaintenance(HWND hwnd) {
    static constexpr PCWSTR kSecurityAndMaintenanceClsid =
        L"{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}";

    if (IsWindows10() &&
        IsShellClsidRegistered(kSecurityAndMaintenanceClsid) &&
        TryShellExecute(hwnd, L"control.exe", L"/name Microsoft.ActionCenter")) {
        return;
    }
    OpenSettingsWithFallback(hwnd, L"ms-settings:windowsdefender",
                             L"ms-settings:privacy", L"ms-settings:");
}

// -----------------------------------------------------------------------------
// Private, verified Windows 8.1 UI payload.
// wucltux.dll 7.9.9600.17415 (winblue_r4.141028-1500), x64.
// Source: Microsoft Symbol Server. The blob redirect is normal for msdl URLs.
// -----------------------------------------------------------------------------
static const wchar_t* kDllName = L"wucltux.dll";
static const wchar_t* kDownloadUrl =
    L"https://msdl.microsoft.com/download/symbols/wucltux.dll/"
    L"54503A411a9000/wucltux.dll";
static const wchar_t* kExpectedSha256 =
    L"2B9928A0928D73786F68166B3EF785C0055BD6E73C5583913703A5D8DF61BE4C";
static const DWORD kMinDllSize = 65536;
static const DWORD kDownloadTimeoutMs = 20000;
static const int kMaxDownloadAttempts = 3;
static const DWORD kRetryDelayMs = 3000;
// Hard ceiling across ALL attempts. Each individual WinInet call is already
// bounded, but a captive portal that trickles bytes can keep a single transfer
// technically "alive" indefinitely; this bounds the whole setup.
static const ULONGLONG kOverallSetupDeadlineMs = 120000;
// Refuse an implausibly large response instead of filling the user's disk.
static const ULONGLONG kMaxDownloadBytes = 32ull * 1024 * 1024;

// The private Windows 8.1 payload is AMD64-only. Windhawk can still inject an
// ARM64 build into the native shell on ARM64 Windows, so reject that process
// before installing hooks, starting workers, or touching the network.
static bool IsRunningAsAmd64() {
#if defined(_M_ARM64) || defined(__aarch64__)
    return false;
#else
    using IsWow64Process2_t = BOOL(WINAPI*)(HANDLE, USHORT*, USHORT*);
    USHORT processMachine = IMAGE_FILE_MACHINE_UNKNOWN;
    USHORT nativeMachine = IMAGE_FILE_MACHINE_UNKNOWN;
    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    if (kernel32) {
        auto isWow64Process2 = reinterpret_cast<IsWow64Process2_t>(
            GetProcAddress(kernel32, "IsWow64Process2"));
        if (isWow64Process2 &&
            isWow64Process2(GetCurrentProcess(), &processMachine, &nativeMachine)) {
            const USHORT actualMachine = processMachine == IMAGE_FILE_MACHINE_UNKNOWN
                                             ? nativeMachine
                                             : processMachine;
            return actualMachine == IMAGE_FILE_MACHINE_AMD64;
        }
    }
#if defined(_M_X64) || defined(__x86_64__)
    return true;
#else
    return false;
#endif
#endif
}

// Windows Update's classic Control Panel namespace item.
static const wchar_t* kAppletClsid = L"{36eef7db-88ad-4e81-ad49-0e313f0c35f8}";
static const wchar_t* kLayoutFolderClsid = L"{328B0346-7EAF-4BBE-A479-7CB88A095F5B}";
static const wchar_t* kDisplayName = L"Windows Update";
static const wchar_t* kApplicationName = L"Microsoft.WindowsUpdate";
// XMLFILE resource 100 names this COM element provider. It is required to
// construct the legacy DirectUI page; omitting it produces "Unable to load page".
static const wchar_t* kElementProviderClsid = L"{cfbc05bc-1b9e-4693-a49c-4e7181d69e0a}";
// The INF ships 0xa0000000 (SFGAO_FOLDER | SFGAO_HASSUBFOLDER), which was
// enough on Windows 7 where wucltux.dll was a real, fully registered system
// component. On Windows 10/11 the Control Panel host additionally requires
// SFGAO_BROWSABLE (0x08000000) to navigate the item in place; without it the
// shell resolves the CLSID, reads every value (as the log confirms) and then
// refuses to browse it, surfacing the generic "no app associated" error.
// SFGAO_DROPTARGET|SFGAO_CANLINK (0x000001a0) match what the Performance
// Information and Tools Restorer uses for the same shdocvw-hosted layout
// folder, so the item behaves like a normal Control Panel applet.
static const DWORD kShellFolderAttributes = 0xA80001A0;
static const DWORD kInitResourceId = 100;

static const GUID kAppletFolderGuid = {0x36eef7db, 0x88ad, 0x4e81,
                                       {0xad, 0x49, 0x0e, 0x31, 0x3f, 0x0c, 0x35, 0xf8}};
static const GUID kElementProviderGuid = {0xcfbc05bc, 0x1b9e, 0x4693,
                                         {0xa4, 0x9c, 0x4e, 0x71, 0x81, 0xd6, 0x9e, 0x0a}};
static const IID IID_IClassFactory_GUID = {0x00000001, 0x0000, 0x0000,
                                           {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

// Payload (wucltux.dll) verified + loaded. Gates ONLY the values that require
// the DLL to exist (InitPropertyBag\ResourceDLL and the element provider's
// InProcServer32).
static std::atomic<bool> g_verified{false};
// Control Panel registration is live. Set at the end of Wh_ModInit, before the
// background setup worker has done anything, so the "Windows Update" item shows
// up in Control Panel immediately and stays visible even when the machine is
// offline or the download fails. Keeping this separate from g_verified is what
// makes the entry independent of the network.
static std::atomic<bool> g_registrationReady{false};
static std::atomic<HMODULE> g_module{nullptr};
static std::atomic<const std::wstring*> g_dllPath{nullptr};
static std::atomic<bool> g_stopping{false};
// Setup completion is separate from payload readiness: false means a short wait
// can still turn an otherwise transient COM activation failure into success; true
// means setup definitively ended and the existing offline/failure notice should be
// shown immediately. The worker thread id prevents a re-entrant COM activation
// during LoadLibrary from waiting on the very worker that must make progress.
static std::atomic<bool> g_setupFinished{false};
static std::atomic<DWORD> g_setupWorkerThreadId{0};
static HANDLE g_stopEvent = nullptr;
// DirectUI resolves resstr(...) via XResourceProvider, bypassing LoadStringW.
// This private resource copy supplies the embedded MUI string blocks to it.
static std::mutex g_resourceMutex;
static std::wstring g_resourcePath;
static std::atomic<HMODULE> g_resourceModule{nullptr};
[[clang::no_destroy]] static std::optional<std::thread> g_setupThread;

static std::mutex g_rebuildMutex;
[[clang::no_destroy]] static std::optional<std::thread> g_rebuildThread;
// Set by Wh_ModSettingsChanged to interrupt an in-flight rebuild's retry
// backoff before joining it, so two quick language changes in a row don't
// stall settings-apply for up to ~10s. Distinct from g_stopEvent: signalling
// g_stopEvent here would also be observed by the unrelated download/setup
// workers, which is not what a settings change should do.
static HANDLE g_rebuildAbortEvent = nullptr;

// Which icon skin to use for the normal Windows Update status banner.
// 0 = Windows 7/current shield/check icons, 1 = Windows 8.1 update icon.
static constexpr int kUpdatePageSkinWindows7 = 0;
static constexpr int kUpdatePageSkinWindows81 = 1;
static std::atomic<int> g_updatePageSkin{kUpdatePageSkinWindows7};
static bool IsWindows81Skin() {
    return g_updatePageSkin.load() == kUpdatePageSkinWindows81;
}

// Whether to show the "updates available" (amber) banner when Windows reports
// pending available updates. Controlled only by the "ShowAvailableUpdates"
// setting (enabled by default); no global hotkey is registered.
static std::atomic<bool> g_showAvailableUpdates{false};

// Optional, conservative bridge to the modern Settings app. When enabled, only
// the translated "system settings" phrase in the up-to-date recommendation is
// made clickable; the text is otherwise unchanged. Disabled by default.
static std::atomic<bool> g_linkSystemSettingsText{false};

// Whether to remove the broken legacy "Check for updates for your PC" red box
// (moduleCheckForUpdates) when Windows Update is unavailable/disabled and only
// the "Turn on automatic updating" box is shown. With the service stopped that
// legacy box's button cannot work, so it is removed by default. Controlled by
// the "RemoveLegacyBrokenOption" setting ("Remove Legacy Broken Option Fix").
static std::atomic<bool> g_removeLegacyBrokenOption{true};

// Windows owns the one visible Control Panel pane. Its links are patched
// in-place when wucltux publishes the per-layout ControlPanelNavLinks object
// (see PSPropertyBag_WriteUnknownHook below).




// Currently selected language code (default "en"). Declared early because the
// embedded string table below resolves strings per language at runtime. Loaded
// from the mod settings in LoadLanguageSetting().
// Published as an index: UI hooks may run on several Explorer threads. Never
// share a mutable std::wstring with them.
enum class Language : int { en, it, es, fr, tr, ru, pt, zh, pl, nl };
static std::atomic<Language> g_language{Language::en};
static Language LanguageFromCode(const std::wstring& value) {
    if (value == L"it") return Language::it;
    if (value == L"es") return Language::es;
    if (value == L"fr") return Language::fr;
    if (value == L"tr") return Language::tr;
    if (value == L"ru") return Language::ru;
    if (value == L"pt") return Language::pt;
    if (value == L"zh") return Language::zh;
    if (value == L"pl") return Language::pl;
    if (value == L"nl") return Language::nl;
    return Language::en;
}
static const wchar_t* LanguageCode() {
    static constexpr const wchar_t* kCodes[] = { L"en", L"it", L"es", L"fr", L"tr", L"ru", L"pt", L"zh", L"pl", L"nl" };
    return kCodes[static_cast<int>(g_language.load(std::memory_order_acquire))];
}
static std::wstring CurrentLanguage() { return LanguageCode(); }
static bool LanguageIs(PCWSTR code) { return wcscmp(LanguageCode(), code) == 0; }

// Whether to show the mod's "service not available" notice (the shield box).
// Controlled by the "ShowServiceNotice" setting (default on).
static std::atomic<bool> g_showServiceNotice{true};

// Cached Windows Update "last successful detection" timestamp, formatted for
// the classic page (see LastCheckForUpdatesText).
static std::wstring g_lastQueryTimeText;
static std::mutex g_lastQueryTimeMutex;

// Cached, background-gathered status values so the Control Panel UI thread never
// has to do blocking service work during page rendering (see GatherBackgroundStatus).
// They are computed once on the setup thread and read from the render path.
static std::atomic<bool> g_cachedWuAvailable{false};
static std::atomic<bool> g_cachedWuServiceProbed{false};
static std::mutex g_statusMutex;
static std::wstring g_cachedLastInstall;
static bool g_lastInstallComputed = false;

// Embedded from the matching Windows 8.1 en-US wucltux.dll.mui string table,
// expanded to ten languages: en, it, es, fr, tr, ru, pt, zh, pl, nl.
struct WucltuxEmbeddedString {
    UINT id;
    const wchar_t* en;
    const wchar_t* it;
    const wchar_t* es;
    const wchar_t* fr;
    const wchar_t* tr;
    const wchar_t* ru;
    const wchar_t* pt;
    const wchar_t* zh;
    const wchar_t* pl;
    const wchar_t* nl;
};
static const WucltuxEmbeddedString kWucltuxMuiStrings[] = {
    { 1, L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Центр обновления Windows", L"Windows Update", L"Windows 更新", L"Windows Update", L"Windows Update" },
    { 2, L"Delivers software updates and drivers, and provides automatic updating options.", L"Fornisce aggiornamenti software e driver e offre opzioni di aggiornamento automatico.", L"Proporciona actualizaciones de software y controladores, y ofrece opciones de actualización automática.", L"Fournit les mises à jour logicielles et les pilotes, et propose des options de mise à jour automatique.", L"Yazılım güncellemeleri ve sürücüler sunar ve otomatik güncelleme seçenekleri sağlar.", L"Поставляет обновления программного обеспечения и драйверов, а также предоставляет параметры автоматического обновления.", L"Fornece atualizações de software e drivers e oferece opções de atualização automática.", L"提供软件更新和驱动程序，并提供自动更新选项。", L"Dostarcza aktualizacje oprogramowania i sterowników oraz udostępnia opcje automatycznej aktualizacji.", L"Levert software-updates en stuurprogramma's en biedt opties voor automatische updates." },
    { 3, L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Центр обновления Windows", L"Windows Update", L"Windows 更新", L"Windows Update", L"Windows Update" },
    { 4, L"Check for software and driver updates, choose automatic updating settings, or view installed updates.", L"Cerca aggiornamenti di software e driver, scegli le impostazioni di aggiornamento automatico o visualizza gli aggiornamenti installati.", L"Busca actualizaciones de software y controladores, elige la configuración de actualización automática o consulta las actualizaciones instaladas.", L"Recherchez les mises à jour logicielles et de pilotes, choisissez les paramètres de mise à jour automatique ou consultez les mises à jour installées.", L"Yazılım ve sürücü güncellemelerini denetleyin, otomatik güncelleme ayarlarını seçin veya yüklü güncellemeleri görüntüleyin.", L"Проверьте наличие обновлений программного обеспечения и драйверов, выберите параметры автоматического обновления или просмотрите установленные обновления.", L"Verifique atualizações de software e drivers, escolha as configurações de atualização automática ou consulte as atualizações instaladas.", L"检查软件和驱动程序更新、选择自动更新设置或查看已安装的更新。", L"Sprawdź aktualizacje oprogramowania i sterowników, wybierz ustawienia automatycznej aktualizacji lub wyświetl zainstalowane aktualizacje.", L"Controleer op software- en stuurprogramma-updates, kies automatische update-instellingen of bekijk geïnstalleerde updates." },
    { 71, L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Центр обновления Windows", L"Windows Update", L"Windows 更新", L"Windows Update", L"Windows Update" },
    { 73, L"Change settings", L"Cambia impostazioni", L"Cambiar la configuración", L"Modifier les paramètres", L"Ayarları değiştir", L"Изменить параметры", L"Alterar configurações", L"更改设置", L"Zmień ustawienia", L"Instellingen wijzigen" },
    { 74, L"View update history", L"Visualizza cronologia aggiornamenti", L"Ver historial de actualizaciones", L"Afficher l'historique des mises à jour", L"Güncelleme geçmişini görüntüle", L"Просмотр журнала обновлений", L"Exibir histórico de atualizações", L"查看更新历史记录", L"Wyświetl historię aktualizacji", L"Updategeschiedenis weergeven" },
    { 75, L"Restore hidden updates", L"Ripristina aggiornamenti nascosti", L"Restaurar actualizaciones ocultas", L"Restaurer les mises à jour masquées", L"Gizli güncellemeleri geri yükle", L"Восстановить скрытые обновления", L"Restaurar atualizações ocultas", L"还原隐藏的更新", L"Przywróć ukryte aktualizacje", L"Verborgen updates herstellen" },
    { 78, L"Select updates to install", L"Seleziona gli aggiornamenti da installare", L"Seleccionar las actualizaciones que se van a instalar", L"Sélectionner les mises à jour à installer", L"Yüklenecek güncellemeleri seçin", L"Выбор обновлений для установки", L"Selecione as atualizações a instalar", L"选择要安装的更新", L"Wybierz aktualizacje do zainstalowania", L"Updates selecteren om te installeren" },
    { 81, L"1 important update", L"1 aggiornamento importante", L"1 actualización importante", L"1 mise à jour importante", L"1 önemli güncelleme", L"1 важное обновление", L"1 atualização importante", L"1 个重要更新", L"1 ważna aktualizacja", L"1 belangrijke update" },
    { 82, L"%d important updates", L"%d aggiornamenti importanti", L"%d actualizaciones importantes", L"%d mises à jour importantes", L"%d önemli güncelleme", L"%d важных обновлений", L"%d atualizações importantes", L"%d 个重要更新", L"%d ważnych aktualizacji", L"%d belangrijke updates" },
    { 83, L"%1, %2", L"%1, %2", L"%1, %2", L"%1, %2", L"%1, %2", L"%1, %2", L"%1, %2", L"%1，%2", L"%1, %2", L"%1, %2" },
    { 84, L"1 update", L"1 aggiornamento", L"1 actualización", L"1 mise à jour", L"1 güncelleme", L"1 обновление", L"1 atualização", L"1 个更新", L"1 aktualizacja", L"1 update" },
    { 85, L"%d updates", L"%d aggiornamenti", L"%d actualizaciones", L"%d mises à jour", L"%d güncelleme", L"%d обновлений", L"%d atualizações", L"%d 个更新", L"%d aktualizacje", L"%d updates" },
    { 86, L"%s MB", L"%s MB", L"%s MB", L"%s Mo", L"%s MB", L"%s МБ", L"%s MB", L"%s MB", L"%s MB", L"%s MB" },
    { 87, L"%s KB", L"%s KB", L"%s KB", L"%s Ko", L"%s KB", L"%s КБ", L"%s KB", L"%s KB", L"%s KB", L"%s KB" },
    { 88, L"1 hour", L"1 ora", L"1 hora", L"1 heure", L"1 saat", L"1 час", L"1 hora", L"1 小时", L"1 godzina", L"1 uur" },
    { 89, L"%1!lu! hours", L"%1!lu! ore", L"%1!lu! horas", L"%1!lu! heures", L"%1!lu! saat", L"%1!lu! ч.", L"%1!lu! horas", L"%1!lu! 小时", L"%1!lu! godz.", L"%1!lu! uur" },
    { 92, L"%1!lu! minutes", L"%1!lu! minuti", L"%1!lu! minutos", L"%1!lu! minutes", L"%1!lu! dakika", L"%1!lu! мин.", L"%1!lu! minutos", L"%1!lu! 分钟", L"%1!lu! min.", L"%1!lu! minuten" },
    { 93, L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2" },
    { 94, L"Download and install updates for your PC", L"Scarica e installa aggiornamenti per il tuo PC", L"Descargar e instalar actualizaciones para su PC", L"Télécharger et installer les mises à jour de votre PC", L"Bilgisayarınız için güncellemeleri indirip yükleyin", L"Загрузить и установить обновления для компьютера", L"Baixar e instalar atualizações para seu PC", L"下载并安装适用于你的电脑的更新", L"Pobierz i zainstaluj aktualizacje dla komputera", L"Updates voor uw pc downloaden en installeren" },
    { 95, L"Install updates for your PC", L"Installa aggiornamenti per il tuo PC", L"Instalar actualizaciones para su PC", L"Installer les mises à jour de votre PC", L"Bilgisayarınız için güncellemeleri yükleyin", L"Установить обновления для компьютера", L"Instalar atualizações para seu PC", L"安装适用于你的电脑的更新", L"Zainstaluj aktualizacje dla komputera", L"Updates voor uw pc installeren" },
    { 96, L"Not configured (not recommended)", L"Non configurato (sconsigliato)", L"No configurado (no recomendado)", L"Non configuré (non recommandé)", L"Yapılandırılmadı (önerilmez)", L"Не настроено (не рекомендуется)", L"Não configurado (não recomendado)", L"未配置（不推荐）", L"Nieskonfigurowano (niezalecane)", L"Niet geconfigureerd (niet aanbevolen)" },
    { 97, L"Never check for updates (not recommended)", L"Non controllare mai gli aggiornamenti (sconsigliato)", L"No comprobar nunca las actualizaciones (no recomendado)", L"Ne jamais rechercher les mises à jour (non recommandé)", L"Güncellemeleri hiç denetleme (önerilmez)", L"Никогда не проверять обновления (не рекомендуется)", L"Nunca verificar atualizações (não recomendado)", L"从不检查更新（不推荐）", L"Nigdy nie sprawdzaj aktualizacji (niezalecane)", L"Nooit naar updates zoeken (niet aanbevolen)" },
    { 98, L"Notify you to download and install new updates", L"Ti informa prima di scaricare e installare nuovi aggiornamenti", L"Notificarle para descargar e instalar nuevas actualizaciones", L"Vous avertir pour télécharger et installer les nouvelles mises à jour", L"Yeni güncellemeleri indirmek ve yüklemek için sizi bilgilendirir", L"Уведомлять о необходимости загрузки и установки новых обновлений", L"Notificá-lo para baixar e instalar novas atualizações", L"通知你下载并安装新更新", L"Powiadamiaj przed pobraniem i zainstalowaniem nowych aktualizacji", L"U op de hoogte stellen om nieuwe updates te downloaden en te installeren" },
    { 99, L"Notify you to install new updates", L"Ti informa prima di installare nuovi aggiornamenti", L"Notificarle para instalar nuevas actualizaciones", L"Vous avertir pour installer les nouvelles mises à jour", L"Yeni güncellemeleri yüklemek için sizi bilgilendirir", L"Уведомлять о необходимости установки новых обновлений", L"Notificá-lo para instalar novas atualizações", L"通知你安装新更新", L"Powiadamiaj przed zainstalowaniem nowych aktualizacji", L"U op de hoogte stellen om nieuwe updates te installeren" },
    { 100, L"Automatically install new updates every day at %s (recommended)", L"Installa automaticamente nuovi aggiornamenti ogni giorno alle ore %s (consigliato)", L"Instalar automáticamente nuevas actualizaciones todos los días a las %s (recomendado)", L"Installer automatiquement les nouvelles mises à jour chaque jour à %s (recommandé)", L"Yeni güncellemeleri her gün %s saatinde otomatik olarak yükle (önerilir)", L"Автоматически устанавливать новые обновления каждый день в %s (рекомендуется)", L"Instalar automaticamente novas atualizações todos os dias às %s (recomendado)", L"每天在 %s 自动安装新更新（推荐）", L"Automatycznie instaluj nowe aktualizacje codziennie o %s (zalecane)", L"Installeer nieuwe updates elke dag om %s automatisch (aanbevolen)" },
    { 101, L"Automatically install new updates every Sunday at %s", L"Installa automaticamente nuovi aggiornamenti ogni domenica alle ore %s", L"Instalar automáticamente nuevas actualizaciones cada domingo a las %s", L"Installer automatiquement les nouvelles mises à jour chaque dimanche à %s", L"Yeni güncellemeleri her Pazar %s saatinde otomatik olarak yükle", L"Автоматически устанавливать новые обновления каждое воскресенье в %s", L"Instalar automaticamente novas atualizações todos os domingos às %s", L"每星期日 %s 自动安装新更新", L"Automatycznie instaluj nowe aktualizacje w każdą niedzielę o %s", L"Installeer nieuwe updates elke zondag om %s automatisch" },
    { 102, L"Automatically install new updates every Monday at %s", L"Installa automaticamente nuovi aggiornamenti ogni lunedì alle ore %s", L"Instalar automáticamente nuevas actualizaciones cada lunes a las %s", L"Installer automatiquement les nouvelles mises à jour chaque lundi à %s", L"Yeni güncellemeleri her Pazartesi %s saatinde otomatik olarak yükle", L"Автоматически устанавливать новые обновления каждый понедельник в %s", L"Instalar automaticamente novas atualizações todas as segundas-feiras às %s", L"每星期一 %s 自动安装新更新", L"Automatycznie instaluj nowe aktualizacje w każdy poniedziałek o %s", L"Installeer nieuwe updates elke maandag om %s automatisch" },
    { 103, L"Automatically install new updates every Tuesday at %s", L"Installa automaticamente nuovi aggiornamenti ogni martedì alle ore %s", L"Instalar automáticamente nuevas actualizaciones cada martes a las %s", L"Installer automatiquement les nouvelles mises à jour chaque mardi à %s", L"Yeni güncellemeleri her Salı %s saatinde otomatik olarak yükle", L"Автоматически устанавливать новые обновления каждый вторник в %s", L"Instalar automaticamente novas atualizações todas as terças-feiras às %s", L"每星期二 %s 自动安装新更新", L"Automatycznie instaluj nowe aktualizacje w każdy wtorek o %s", L"Installeer nieuwe updates elke dinsdag om %s automatisch" },
    { 104, L"Automatically install new updates every Wednesday at %s", L"Installa automaticamente nuovi aggiornamenti ogni mercoledì alle ore %s", L"Instalar automáticamente nuevas actualizaciones cada miércoles a las %s", L"Installer automatiquement les nouvelles mises à jour chaque mercredi à %s", L"Yeni güncellemeleri her Çarşamba %s saatinde otomatik olarak yükle", L"Автоматически устанавливать новые обновления каждую среду в %s", L"Instalar automaticamente novas atualizações todas as quartas-feiras às %s", L"每星期三 %s 自动安装新更新", L"Automatycznie instaluj nowe aktualizacje w każdą środę o %s", L"Installeer nieuwe updates elke woensdag om %s automatisch" },
    { 105, L"Automatically install new updates every Thursday at %s", L"Installa automaticamente nuovi aggiornamenti ogni giovedì alle ore %s", L"Instalar automáticamente nuevas actualizaciones cada jueves a las %s", L"Installer automatiquement les nouvelles mises à jour chaque jeudi à %s", L"Yeni güncellemeleri her Perşembe %s saatinde otomatik olarak yükle", L"Автоматически устанавливать новые обновления каждый четверг в %s", L"Instalar automaticamente novas atualizações todas as quintas-feiras às %s", L"每星期四 %s 自动安装新更新", L"Automatycznie instaluj nowe aktualizacje w każdy czwartek o %s", L"Installeer nieuwe updates elke donderdag om %s automatisch" },
    { 106, L"Automatically install new updates every Friday at %s", L"Installa automaticamente nuovi aggiornamenti ogni venerdì alle ore %s", L"Instalar automáticamente nuevas actualizaciones cada viernes a las %s", L"Installer automatiquement les nouvelles mises à jour chaque vendredi à %s", L"Yeni güncellemeleri her Cuma %s saatinde otomatik olarak yükle", L"Автоматически устанавливать новые обновления каждую пятницу в %s", L"Instalar automaticamente novas atualizações todas as sextas-feiras às %s", L"每星期五 %s 自动安装新更新", L"Automatycznie instaluj nowe aktualizacje w każdy piątek o %s", L"Installeer nieuwe updates elke vrijdag om %s automatisch" },
    { 107, L"Automatically install new updates every Saturday at %s", L"Installa automaticamente nuovi aggiornamenti ogni sabato alle ore %s", L"Instalar automáticamente nuevas actualizaciones cada sábado a las %s", L"Installer automatiquement les nouvelles mises à jour chaque samedi à %s", L"Yeni güncellemeleri her Cumartesi %s saatinde otomatik olarak yükle", L"Автоматически устанавливать новые обновления каждую субботу в %s", L"Instalar automaticamente novas atualizações todos os sábados às %s", L"每星期六 %s 自动安装新更新", L"Automatycznie instaluj nowe aktualizacje w każdą sobotę o %s", L"Installeer nieuwe updates elke zaterdag om %s automatisch" },
    { 109, L"Today at %s", L"Oggi alle ore %s", L"Hoy a las %s", L"Aujourd'hui à %s", L"Bugün %s", L"Сегодня в %s", L"Hoje às %s", L"今天 %s", L"Dziś o %s", L"Vandaag om %s" },
    { 110, L"Yesterday at %s", L"Ieri alle ore %s", L"Ayer a las %s", L"Hier à %s", L"Dün %s", L"Вчера в %s", L"Ontem às %s", L"昨天 %s", L"Wczoraj o %s", L"Gisteren om %s" },
    { 111, L"%1 at %2", L"%1 alle ore %2", L"%1 a las %2", L"%1 à %2", L"%1 %2", L"%1 в %2", L"%1 às %2", L"%1 %2", L"%1 o %2", L"%1 om %2" },
    { 112, L"Error code %X", L"Codice di errore %X", L"Código de error %X", L"Code d'erreur %X", L"Hata kodu %X", L"Код ошибки %X", L"Código de erro %X", L"错误代码 %X", L"Kod błędu %X", L"Foutcode %X" },
    { 113, L"Without the latest updates, your PC is more vulnerable to security attacks and performance problems.", L"Senza gli aggiornamenti più recenti, il tuo PC è più vulnerabile ad attacchi alla sicurezza e a problemi di prestazioni.", L"Sin las actualizaciones más recientes, su PC es más vulnerable a ataques de seguridad y problemas de rendimiento.", L"Sans les dernières mises à jour, votre PC est plus vulnérable aux attaques de sécurité et aux problèmes de performances.", L"En son güncellemeler olmadan bilgisayarınız güvenlik saldırılarına ve performans sorunlarına karşı daha savunmasızdır.", L"Без последних обновлений компьютер более уязвим к атакам и проблемам с производительностью.", L"Sem as atualizações mais recentes, seu PC fica mais vulnerável a ataques de segurança e problemas de desempenho.", L"没有最新更新，你的电脑更容易受到安全攻击和性能问题的影响。", L"Bez najnowszych aktualizacji komputer jest bardziej narażony na ataki i problemy z wydajnością.", L"Zonder de nieuwste updates is uw pc kwetsbaarder voor beveiligingsaanvallen en prestatieproblemen." },
    { 114, L"%1 - %2", L"%1 - %2", L"%1 - %2", L"%1 - %2", L"%1 - %2", L"%1 — %2", L"%1 - %2", L"%1 - %2", L"%1 - %2", L"%1 - %2" },
    { 115, L"Recommended", L"Consigliato", L"Recomendado", L"Recommandé", L"Önerilen", L"Рекомендуется", L"Recomendado", L"推荐", L"Zalecane", L"Aanbevolen" },
    { 116, L"Name", L"Nome", L"Nombre", L"Nom", L"Ad", L"Имя", L"Nome", L"名称", L"Nazwa", L"Naam" },
    { 117, L"Type", L"Tipo", L"Tipo", L"Type", L"Tür", L"Тип", L"Tipo", L"类型", L"Typ", L"Type" },
    { 118, L"Published", L"Pubblicato", L"Publicado", L"Publiée", L"Yayınlanma", L"Опубликовано", L"Publicado", L"已发布", L"Opublikowano", L"Gepubliceerd" },
    { 119, L"Important", L"Importante", L"Importante", L"Importante", L"Önemli", L"Важное", L"Importante", L"重要", L"Ważne", L"Belangrijk" },
    { 120, L"Optional", L"Facoltativo", L"Opcional", L"Facultatif", L"İsteğe bağlı", L"Необязательное", L"Opcional", L"可选", L"Opcjonalne", L"Optioneel" },
    { 121, L"Today", L"Oggi", L"Hoy", L"Aujourd'hui", L"Bugün", L"Сегодня", L"Hoje", L"今天", L"Dziś", L"Vandaag" },
    { 122, L"Yesterday", L"Ieri", L"Ayer", L"Hier", L"Dün", L"Вчера", L"Ontem", L"昨天", L"Wczoraj", L"Gisteren" },
    { 123, L"Status", L"Stato", L"Estado", L"État", L"Durum", L"Состояние", L"Status", L"状态", L"Stan", L"Status" },
    { 124, L"Date installed", L"Data di installazione", L"Fecha de instalación", L"Date d'installation", L"Yükleme tarihi", L"Дата установки", L"Data de instalação", L"安装日期", L"Data instalacji", L"Installatiedatum" },
    { 125, L"Succeeded", L"Riuscito", L"Correcta", L"Réussie", L"Başarılı", L"Успешно", L"Bem-sucedido", L"成功", L"Powodzenie", L"Geslaagd" },
    { 126, L"Failed", L"Non riuscito", L"Error", L"Échec", L"Başarısız", L"Неудачно", L"Falhou", L"失败", L"Niepowodzenie", L"Mislukt" },
    { 127, L"Canceled", L"Annullato", L"Cancelada", L"Annulée", L"İptal edildi", L"Отменено", L"Cancelada", L"已取消", L"Anulowano", L"Geannuleerd" },
    { 128, L"Downloading updates...", L"Download degli aggiornamenti in corso...", L"Descargando actualizaciones...", L"Téléchargement des mises à jour...", L"Güncellemeler indiriliyor...", L"Загрузка обновлений...", L"Baixando atualizações...", L"正在下载更新...", L"Pobieranie aktualizacji...", L"Updates downloaden..." },
    { 129, L"Downloading %1!lu! updates (%2 total, %3!lu!%% complete)", L"Download di %1!lu! aggiornamenti (%2 in totale, %3!lu!%% completato)", L"Descargando %1!lu! actualizaciones (%2 en total, %3!lu!%% completadas)", L"Téléchargement de %1!lu! mises à jour (%2 au total, %3!lu!%% effectué)", L"%1!lu! güncelleme indiriliyor (toplam %2, %3!lu!%% tamamlandı)", L"Загрузка обновлений: %1!lu! (%2 всего, выполнено %3!lu!%%)", L"Baixando %1!lu! atualizações (%2 no total, %3!lu!%% concluído)", L"正在下载 %1!lu! 个更新（共 %2 个，已完成 %3!lu!%%）", L"Pobieranie %1!lu! aktualizacji (łącznie %2, %3!lu!%% ukończono)", L"%1!lu! updates downloaden (%2 totaal, %3!lu!%% voltooid)" },
    { 130, L"Downloading 1 update (%2 total, %3!lu!%% complete)", L"Download di 1 aggiornamento (%2 in totale, %3!lu!%% completato)", L"Descargando 1 actualización (%2 en total, %3!lu!%% completada)", L"Téléchargement de 1 mise à jour (%2 au total, %3!lu!%% effectué)", L"1 güncelleme indiriliyor (toplam %2, %3!lu!%% tamamlandı)", L"Загрузка 1 обновления (%2 всего, выполнено %3!lu!%%)", L"Baixando 1 atualização (%2 no total, %3!lu!%% concluído)", L"正在下载 1 个更新（共 %2 个，已完成 %3!lu!%%）", L"Pobieranie 1 aktualizacji (łącznie %2, %3!lu!%% ukończono)", L"1 update downloaden (%2 totaal, %3!lu!%% voltooid)" },
    { 131, L"&Stop download", L"&Interrompi download", L"&Detener descarga", L"&Arrêter le téléchargement", L"İndirmeyi &durdur", L"&Остановить загрузку", L"&Parar download", L"&停止下载", L"&Zatrzymaj pobieranie", L"Download &stoppen" },
    { 132, L"Installing updates...", L"Installazione aggiornamenti in corso...", L"Instalando actualizaciones...", L"Installation des mises à jour...", L"Güncellemeler yükleniyor...", L"Установка обновлений...", L"Instalando atualizações...", L"正在安装更新...", L"Instalowanie aktualizacji...", L"Updates installeren..." },
    { 133, L"Installing update %1!lu! of %2!lu!...", L"Installazione aggiornamento %1!lu! di %2!lu!...", L"Instalando actualización %1!lu! de %2!lu!...", L"Installation de la mise à jour %1!lu! sur %2!lu!...", L"%2!lu! güncellemeden %1!lu! yükleniyor...", L"Установка обновления %1!lu! из %2!lu!...", L"Instalando atualização %1!lu! de %2!lu!...", L"正在安装 %2!lu! 个更新中的第 %1!lu! 个...", L"Instalowanie aktualizacji %1!lu! z %2!lu!...", L"Update %1!lu! van %2!lu! installeren..." },
    { 134, L"Preparing to install...", L"Preparazione installazione...", L"Preparando la instalación...", L"Préparation de l'installation...", L"Yüklemeye hazırlanıyor...", L"Подготовка к установке...", L"Preparando a instalação...", L"正在准备安装...", L"Przygotowywanie do instalacji...", L"Installatie voorbereiden..." },
    { 135, L"(Uninstall:) %s", L"(Disinstallazione:) %s", L"(Desinstalar:) %s", L"(Désinstaller:) %s", L"(Kaldır:) %s", L"(Удаление:) %s", L"(Desinstalar:) %s", L"（卸载：）%s", L"(Odinstaluj:) %s", L"(Verwijderen:) %s" },
    { 136, L"%1 (%2!lu!)", L"%1 (%2!lu!)", L"%1 (%2!lu!)", L"%1 (%2!lu!)", L"%1 (%2!lu!)", L"%1 (%2!lu!)", L"%1 (%2!lu!)", L"%1（%2!lu!）", L"%1 (%2!lu!)", L"%1 (%2!lu!)" },
    { 137, L"Updates", L"Aggiornamenti", L"Actualizaciones", L"Mises à jour", L"Güncellemeler", L"Обновления", L"Atualizações", L"更新", L"Aktualizacje", L"Updates" },
    { 138, L"Total missing updates: %1!lu!", L"Aggiornamenti mancanti totali: %1!lu!", L"Actualizaciones que faltan en total: %1!lu!", L"Mises à jour manquantes au total : %1!lu!", L"Toplam eksik güncelleme: %1!lu!", L"Всего отсутствует обновлений: %1!lu!", L"Atualizações ausentes no total: %1!lu!", L"缺少的更新总数：%1!lu!", L"Łącznie brakujących aktualizacji: %1!lu!", L"Totaal ontbrekende updates: %1!lu!" },
    { 139, L"Code %1!X!", L"Codice %1!X!", L"Código %1!X!", L"Code %1!X!", L"Kod %1!X!", L"Код %1!X!", L"Código %1!X!", L"代码 %1!X!", L"Kod %1!X!", L"Code %1!X!" },
    { 140, L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2" },
    { 141, L"Succeeded: 1 update", L"Riuscito: 1 aggiornamento", L"Correctas: 1 actualización", L"Réussie : 1 mise à jour", L"Başarılı: 1 güncelleme", L"Успешно: 1 обновление", L"Bem-sucedida: 1 atualização", L"成功：1 个更新", L"Powodzenie: 1 aktualizacja", L"Geslaagd: 1 update" },
    { 142, L"Succeeded: %1!lu! updates", L"Riuscito: %1!lu! aggiornamenti", L"Correctas: %1!lu! actualizaciones", L"Réussies : %1!lu! mises à jour", L"Başarılı: %1!lu! güncelleme", L"Успешно: %1!lu! обновлений", L"Bem-sucedidas: %1!lu! atualizações", L"成功：%1!lu! 个更新", L"Powodzenie: %1!lu! aktualizacji", L"Geslaagd: %1!lu! updates" },
    { 143, L"Failed: 1 update", L"Non riuscito: 1 aggiornamento", L"Error: 1 actualización", L"Échec : 1 mise à jour", L"Başarısız: 1 güncelleme", L"Неудачно: 1 обновление", L"Falhou: 1 atualização", L"失败：1 个更新", L"Niepowodzenie: 1 aktualizacja", L"Mislukt: 1 update" },
    { 144, L"Failed: %1!lu! updates", L"Non riuscito: %1!lu! aggiornamenti", L"Error: %1!lu! actualizaciones", L"Échec : %1!lu! mises à jour", L"Başarısız: %1!lu! güncelleme", L"Неудачно: %1!lu! обновлений", L"Falhou: %1!lu! atualizações", L"失败：%1!lu! 个更新", L"Niepowodzenie: %1!lu! aktualizacji", L"Mislukt: %1!lu! updates" },
    { 145, L"Canceled: 1 update", L"Annullato: 1 aggiornamento", L"Cancelada: 1 actualización", L"Annulée : 1 mise à jour", L"İptal edildi: 1 güncelleme", L"Отменено: 1 обновление", L"Cancelada: 1 atualização", L"已取消：1 个更新", L"Anulowano: 1 aktualizacja", L"Geannuleerd: 1 update" },
    { 146, L"Canceled: %1!lu! updates", L"Annullato: %1!lu! aggiornamenti", L"Canceladas: %1!lu! actualizaciones", L"Annulées : %1!lu! mises à jour", L"İptal edildi: %1!lu! güncelleme", L"Отменено: %1!lu! обновлений", L"Canceladas: %1!lu! atualizações", L"已取消：%1!lu! 个更新", L"Anulowano: %1!lu! aktualizacji", L"Geannuleerd: %1!lu! updates" },
    { 147, L"Not needed: 1 update", L"Non necessario: 1 aggiornamento", L"No necesario: 1 actualización", L"Non nécessaire : 1 mise à jour", L"Gerekli değil: 1 güncelleme", L"Не требуется: 1 обновление", L"Não necessária: 1 atualização", L"不需要：1 个更新", L"Niepotrzebne: 1 aktualizacja", L"Niet nodig: 1 update" },
    { 148, L"Not needed: %1!lu! updates", L"Non necessario: %1!lu! aggiornamenti", L"No necesarios: %1!lu! actualizaciones", L"Non nécessaires : %1!lu! mises à jour", L"Gerekli değil: %1!lu! güncelleme", L"Не требуется: %1!lu! обновлений", L"Não necessárias: %1!lu! atualizações", L"不需要：%1!lu! 个更新", L"Niepotrzebne: %1!lu! aktualizacji", L"Niet nodig: %1!lu! updates" },
    { 152, L"Windows could not search for new updates", L"Windows non ha potuto cercare nuovi aggiornamenti", L"Windows no pudo buscar nuevas actualizaciones", L"Windows n'a pas pu rechercher de nouvelles mises à jour", L"Windows yeni güncellemeleri arayamadı", L"Windows не удалось найти новые обновления", L"O Windows não pôde procurar novas atualizações", L"Windows 无法搜索新更新", L"System Windows nie mógł wyszukać nowych aktualizacji", L"Windows kon geen nieuwe updates zoeken" },
    { 157, L"&Restore update", L"&Ripristina aggiornamento", L"&Restaurar actualización", L"&Restaurer la mise à jour", L"Güncellemeyi &geri yükle", L"&Восстановить обновление", L"&Restaurar atualização", L"&还原更新", L"&Przywróć aktualizację", L"Update &herstellen" },
    { 158, L"&Hide update", L"&Nascondi aggiornamento", L"&Ocultar actualización", L"&Masquer la mise à jour", L"Güncellemeyi &gizle", L"&Скрыть обновление", L"&Ocultar atualização", L"&隐藏更新", L"&Ukryj aktualizację", L"Update &verbergen" },
    { 159, L"Downloading and installing updates...", L"Download e installazione aggiornamenti in corso...", L"Descargando e instalando actualizaciones...", L"Téléchargement et installation des mises à jour...", L"Güncellemeler indiriliyor ve yükleniyor...", L"Загрузка и установка обновлений...", L"Baixando e instalando atualizações...", L"正在下载并安装更新...", L"Pobieranie i instalowanie aktualizacji...", L"Updates downloaden en installeren..." },
    { 160, L"If you opt out, you won't receive updates from %1 anymore. Do you want to continue?", L"Se annulli l'iscrizione, non riceverai più aggiornamenti da %1. Continuare?", L"Si opta por no participar, dejará de recibir actualizaciones de %1. ¿Desea continuar?", L"Si vous vous désabonnez, vous ne recevrez plus de mises à jour de %1. Voulez-vous continuer ?", L"Abone olmaktan çıkarsanız artık %1 güncellemelerini alamazsınız. Devam etmek istiyor musunuz?", L"Если отказаться, вы больше не будете получать обновления от %1. Продолжить?", L"Se você desativar a assinatura, não receberá mais atualizações de %1. Deseja continuar?", L"如果选择退出，你将不再收到来自 %1 的更新。是否要继续？", L"Jeśli zrezygnujesz, nie będziesz już otrzymywać aktualizacji od %1. Czy chcesz kontynuować?", L"Als u zich afmeldt, ontvangt u geen updates meer van %1. Wilt u doorgaan?" },
    { 161, L"Your administrator requires this update to be installed by %1 at %2.", L"L'amministratore richiede l'installazione di questo aggiornamento entro %1 alle ore %2.", L"El administrador exige que esta actualización se instale antes del %1 a las %2.", L"Votre administrateur exige que cette mise à jour soit installée d'ici le %1 à %2.", L"Yöneticiniz bu güncellemenin %2 tarihinde %1 saatine kadar yüklenmesini gerektiriyor.", L"Администратор требует установить это обновление до %1 в %2.", L"Seu administrador exige que esta atualização seja instalada até %1 às %2.", L"管理员要求此更新必须在 %1 %2 之前安装。", L"Administrator wymaga zainstalowania tej aktualizacji do %1 o %2.", L"Uw beheerder vereist dat deze update vóór %1 om %2 is geïnstalleerd." },
    { 162, L"Your administrator requires this update to be installed by today at %s.", L"L'amministratore richiede l'installazione di questo aggiornamento entro oggi alle ore %s.", L"El administrador exige que esta actualización se instale hoy a las %s.", L"Votre administrateur exige que cette mise à jour soit installée aujourd'hui à %s.", L"Yöneticiniz bu güncellemenin bugün %s saatine kadar yüklenmesini gerektiriyor.", L"Администратор требует установить это обновление сегодня в %s.", L"Seu administrador exige que esta atualização seja instalada hoje às %s.", L"管理员要求此更新必须在今天 %s 之前安装。", L"Administrator wymaga zainstalowania tej aktualizacji dziś o %s.", L"Uw beheerder vereist dat deze update vandaag om %s is geïnstalleerd." },
    { 163, L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Центр обновления Windows", L"Windows Update", L"Windows 更新", L"Windows Update", L"Windows Update" },
    { 164, L"Please read and accept the license terms (%1!lu! of %2!lu!)", L"Leggere e accettare i termini di licenza (%1!lu! di %2!lu!)", L"Lea y acepte los términos de licencia (%1!lu! de %2!lu!)", L"Veuillez lire et accepter les termes du contrat de licence (%1!lu! sur %2!lu!)", L"Lisans koşullarını okuyun ve kabul edin (%2!lu! sözleşmeden %1!lu!)", L"Прочтите и примите условия лицензии (%1!lu! из %2!lu!)", L"Leia e aceite os termos da licença (%1!lu! de %2!lu!)", L"请阅读并接受许可条款（共 %2!lu! 项，第 %1!lu! 项）", L"Przeczytaj i zaakceptuj postanowienia licencyjne (%1!lu! z %2!lu!)", L"Lees en accepteer de licentievoorwaarden (%1!lu! van %2!lu!)" },
    { 165, L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Центр обновления Windows", L"Windows Update", L"Windows 更新", L"Windows Update", L"Windows Update" },
    { 166, L"These updates won't be installed.", L"Questi aggiornamenti non verranno installati.", L"Estas actualizaciones no se instalarán.", L"Ces mises à jour ne seront pas installées.", L"Bu güncellemeler yüklenmeyecek.", L"Эти обновления не будут установлены.", L"Estas atualizações não serão instaladas.", L"这些更新将不会安装。", L"Te aktualizacje nie zostaną zainstalowane.", L"Deze updates worden niet geïnstalleerd." },
    { 167, L"&Don't ask me again to install these updates", L"&Non chiedermi più di installare questi aggiornamenti", L"No &volver a preguntar si deseo instalar estas actualizaciones", L"Ne plus &demander d'installer ces mises à jour", L"Bu güncellemeleri yüklememi bir daha &sorma", L"&Больше не спрашивать об установке этих обновлений", L"Não &perguntar novamente sobre estas atualizações", L"&不再询问是否安装这些更新", L"&Nie pytaj ponownie o instalowanie tych aktualizacji", L"Niet &opnieuw vragen deze updates te installeren" },
    { 168, L"%1. ", L"%1. ", L"%1. ", L"%1. ", L"%1. ", L"%1. ", L"%1. ", L"%1.", L"%1. ", L"%1. " },
    { 169, L"You need to provide administrator permission", L"È necessario fornire l'autorizzazione di amministratore", L"Debe proporcionar permiso de administrador", L"Vous devez fournir une autorisation d'administrateur", L"Yönetici izni sağlamanız gerekir", L"Требуется разрешение администратора", L"Você precisa fornecer permissão de administrador", L"你需要提供管理员权限", L"Musisz podać uprawnienia administratora", L"U moet beheerdersmachtiging verstrekken" },
    { 170, L"To complete this task, you need to sign in with an administrator account or ask an administrator to complete the task for you.", L"Per completare l'attività, devi accedere con un account amministratore o chiedere a un amministratore di completarla per te.", L"Para completar esta tarea, debe iniciar sesión con una cuenta de administrador o pedirle a un administrador que la complete por usted.", L"Pour terminer cette tâche, vous devez vous connecter avec un compte administrateur ou demander à un administrateur de la terminer pour vous.", L"Bu görevi tamamlamak için bir yönetici hesabıyla oturum açmanız veya bir yöneticiden bu görevi sizin için tamamlamasını istemeniz gerekir.", L"Для выполнения этой задачи необходимо войти с учетной записью администратора или попросить администратора выполнить ее за вас.", L"Para concluir esta tarefa, você precisa entrar com uma conta de administrador ou pedir a um administrador que a conclua para você.", L"要完成此任务，你需要使用管理员帐户登录，或请求管理员为你完成此任务。", L"Aby ukończyć to zadanie, musisz zalogować się na konto administratora lub poprosić administratora o ukończenie go za Ciebie.", L"Om deze taak te voltooien moet u zich aanmelden met een beheerdersaccount of een beheerder vragen deze taak voor u te voltooien." },
    { 171, L"Restart now to finish installing updates.", L"Riavvia ora per terminare l'installazione degli aggiornamenti.", L"Reinicie ahora para terminar de instalar las actualizaciones.", L"Redémarrez maintenant pour terminer l'installation des mises à jour.", L"Güncellemelerin yüklenmesini bitirmek için şimdi yeniden başlatın.", L"Перезапустите компьютер сейчас, чтобы завершить установку обновлений.", L"Reinicie agora para concluir a instalação das atualizações.", L"立即重启以完成更新安装。", L"Uruchom ponownie teraz, aby zakończyć instalowanie aktualizacji.", L"Start nu opnieuw op om de installatie van updates te voltooien." },
    { 173, L"1 optional update", L"1 aggiornamento facoltativo", L"1 actualización opcional", L"1 mise à jour facultative", L"1 isteğe bağlı güncelleme", L"1 необязательное обновление", L"1 atualização opcional", L"1 个可选更新", L"1 opcjonalna aktualizacja", L"1 optionele update" },
    { 174, L"%1!lu! optional updates", L"%1!lu! aggiornamenti facoltativi", L"%1!lu! actualizaciones opcionales", L"%1!lu! mises à jour facultatives", L"%1!lu! isteğe bağlı güncelleme", L"%1!lu! необязательных обновлений", L"%1!lu! atualizações opcionais", L"%1!lu! 个可选更新", L"%1!lu! opcjonalnych aktualizacji", L"%1!lu! optionele updates" },
    { 178, L"For Windows and other products from %s", L"Per Windows e altri prodotti di %s", L"Para Windows y otros productos de %s", L"Pour Windows et d'autres produits de %s", L"Windows ve %s diğer ürünleri için", L"Для Windows и других продуктов %s", L"Para Windows e outros produtos da %s", L"适用于 %s 的 Windows 及其他产品", L"Dla systemu Windows i innych produktów firmy %s", L"Voor Windows en andere producten van %s" },
    { 179, L"From %s", L"Da %s", L"De %s", L"De %s", L"%s sürümünden", L"От %s", L"Da %s", L"来自 %s", L"Od %s", L"Van %s" },
    { 180, L"You will receive updates from %1.", L"Riceverai aggiornamenti da %1.", L"Recibirá actualizaciones de %1.", L"Vous recevrez des mises à jour de %1.", L"%1 güncellemelerini alacaksınız.", L"Вы будете получать обновления от %1.", L"Você receberá atualizações de %1.", L"你将收到来自 %1 的更新。", L"Będziesz otrzymywać aktualizacje od %1.", L"U ontvangt updates van %1." },
    { 182, L"Managed by your system administrator", L"Gestito dall'amministratore di sistema", L"Administrado por el administrador del sistema", L"Géré par votre administrateur système", L"Sistem yöneticiniz tarafından yönetiliyor", L"Управляется системным администратором", L"Gerenciado pelo administrador do sistema", L"由你的系统管理员管理", L"Zarządzane przez administratora systemu", L"Beheerd door uw systeembeheerder" },
    { 183, L"More updates are available.", L"Sono disponibili altri aggiornamenti.", L"Hay más actualizaciones disponibles.", L"D'autres mises à jour sont disponibles.", L"Daha fazla güncelleme kullanılabilir.", L"Доступно больше обновлений.", L"Há mais atualizações disponíveis.", L"还有更多更新可用。", L"Dostępnych jest więcej aktualizacji.", L"Er zijn meer updates beschikbaar." },
    { 184, L"%1 (Failed)", L"%1 (non riuscito)", L"%1 (Error)", L"%1 (échec)", L"%1 (başarısız)", L"%1 (сбой)", L"%1 (falhou)", L"%1（失败）", L"%1 (niepowodzenie)", L"%1 (mislukt)" },
    { 185, L"Pending restart", L"Riavvio in sospeso", L"Reinicio pendiente", L"Redémarrage en attente", L"Yeniden başlatma bekleniyor", L"Ожидается перезапуск", L"Reinicialização pendente", L"待重启", L"Oczekiwanie na ponowne uruchomienie", L"Opnieuw opstarten in behandeling" },
    { 187, L"For Windows only.", L"Solo per Windows.", L"Solo para Windows.", L"Windows uniquement.", L"Yalnızca Windows için.", L"Только для Windows.", L"Somente para Windows.", L"仅适用于 Windows。", L"Tylko dla systemu Windows.", L"Alleen voor Windows." },
    { 188, L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Центр обновления Windows", L"Windows Update", L"Windows 更新", L"Windows Update", L"Windows Update" },
    { 189, L"Check for updates managed by your system administrator", L"Controlla gli aggiornamenti gestiti dall'amministratore di sistema", L"Comprobar actualizaciones administradas por el administrador del sistema", L"Rechercher les mises à jour gérées par votre administrateur système", L"Sistem yöneticiniz tarafından yönetilen güncellemeleri denetleyin", L"Проверить обновления, управляемые системным администратором", L"Verificar atualizações gerenciadas pelo administrador do sistema", L"检查由系统管理员管理的更新", L"Sprawdź aktualizacje zarządzane przez administratora systemu", L"Controleer op updates die door uw systeembeheerder worden beheerd" },
    { 191, L"Windows Update ran into a problem.", L"Windows Update ha riscontrato un problema.", L"Windows Update tuvo un problema.", L"Windows Update a rencontré un problème.", L"Windows Update bir sorunla karşılaştı.", L"Центр обновления Windows столкнулся с проблемой.", L"O Windows Update encontrou um problema.", L"Windows 更新遇到问题。", L"Windows Update napotkał problem.", L"Windows Update heeft een probleem ondervonden." },
    { 222, L"Restarting in: %1!d! min, %2!d! sec", L"Riavvio tra: %1!d! min, %2!d! sec", L"Reiniciando en: %1!d! min, %2!d! seg", L"Redémarrage dans : %1!d! min, %2!d! s", L"Şu süre içinde yeniden başlatılıyor: %1!d! dk, %2!d! sn", L"Перезапуск через: %1!d! мин, %2!d! с", L"Reiniciando em: %1!d! min, %2!d! seg", L"正在重启：%1!d! 分 %2!d! 秒", L"Uruchamianie ponownie za: %1!d! min, %2!d! s", L"Opnieuw starten over: %1!d! min, %2!d! sec" },
    { 223, L"&Restart", L"&Riavvia", L"&Reiniciar", L"&Redémarrer", L"Yeniden &başlat", L"&Перезапустить", L"&Reiniciar", L"&重启", L"&Uruchom ponownie", L"&Opnieuw starten" },
    { 226, L"Windows can't update important files and services while the system is using them. Make sure to save your files before restarting.", L"Windows non può aggiornare file e servizi importanti mentre il sistema li sta utilizzando. Salva i file prima di riavviare.", L"Windows no puede actualizar archivos y servicios importantes mientras el sistema los está usando. Asegúrese de guardar sus archivos antes de reiniciar.", L"Windows ne peut pas mettre à jour des fichiers et services importants pendant que le système les utilise. Veillez à enregistrer vos fichiers avant de redémarrer.", L"Sistem bunları kullanırken Windows önemli dosyaları ve hizmetleri güncelleyemez. Yeniden başlatmadan önce dosyalarınızı kaydettiğinizden emin olun.", L"Windows не может обновить важные файлы и службы, пока они используются системой. Перед перезапуском обязательно сохраните файлы.", L"O Windows não pode atualizar arquivos e serviços importantes enquanto o sistema os está usando. Salve seus arquivos antes de reiniciar.", L"系统正在使用重要文件和服务时，Windows 无法更新它们。请确保在重启前保存文件。", L"System Windows nie może zaktualizować ważnych plików i usług, gdy są używane. Przed ponownym uruchomieniem zapisz pliki.", L"Windows kan geen belangrijke bestanden en services bijwerken terwijl het systeem ze gebruikt. Sla uw bestanden op voordat u opnieuw opstart." },
    { 227, L"Your PC needs to restart to finish installing important updates. If you've already saved everything, you can restart now. Otherwise, you should take a moment to save your work.", L"Il tuo PC deve essere riavviato per completare l'installazione degli aggiornamenti importanti. Se hai già salvato tutto, puoi riavviare ora. In caso contrario, prenditi un momento per salvare il lavoro.", L"Su PC debe reiniciarse para terminar de instalar las actualizaciones importantes. Si ya guardó todo, puede reiniciar ahora. De lo contrario, debe tomarse un momento para guardar su trabajo.", L"Votre PC doit redémarrer pour terminer l'installation des mises à jour importantes. Si vous avez déjà tout enregistré, vous pouvez redémarrer maintenant. Sinon, prenez un moment pour enregistrer votre travail.", L"Bilgisayarınızın önemli güncellemelerin yüklenmesini bitirmek için yeniden başlatılması gerekiyor. Her şeyi kaydettiyseniz şimdi yeniden başlatabilirsiniz. Aksi takdirde, çalışmanızı kaydetmek için bir dakikanızı ayırın.", L"Компьютер необходимо перезапустить, чтобы завершить установку важных обновлений. Если вы уже сохранили все, можно перезапустить сейчас. В противном случае сохраните свою работу.", L"Seu PC precisa ser reiniciado para concluir a instalação de atualizações importantes. Se já salvou tudo, reinicie agora. Caso contrário, reserve um momento para salvar seu trabalho.", L"你的电脑需要重启才能完成重要更新的安装。如果已保存所有内容，现在即可重启。否则，请花点时间保存你的工作。", L"Komputer należy ponownie uruchomić, aby zakończyć instalowanie ważnych aktualizacji. Jeśli wszystko zapisałeś, uruchom ponownie teraz. W przeciwnym razie poświęć chwilę na zapisanie pracy.", L"Uw pc moet opnieuw worden gestart om de installatie van belangrijke updates te voltooien. Als u alles al hebt opgeslagen, kunt u nu opnieuw starten. Anders neemt u even de tijd om uw werk op te slaan." },
    { 235, L"Restarting in %1!d! minutes, %2!d! seconds", L"Riavvio tra %1!d! minuti e %2!d! secondi", L"Reiniciando en %1!d! minutos y %2!d! segundos", L"Redémarrage dans %1!d! minutes et %2!d! secondes", L"%1!d! dakika %2!d! saniye içinde yeniden başlatılıyor", L"Перезапуск через %1!d! мин и %2!d! с", L"Reiniciando em %1!d! minutos e %2!d! segundos", L"将在 %1!d! 分 %2!d! 秒后重启", L"Uruchamianie ponowne za %1!d! minut i %2!d! sekund", L"Opnieuw starten over %1!d! minuten en %2!d! seconden" },
    { 236, L"Restarting in %1!d! seconds", L"Riavvio tra %1!d! secondi", L"Reiniciando en %1!d! segundos", L"Redémarrage dans %1!d! secondes", L"%1!d! saniye içinde yeniden başlatılıyor", L"Перезапуск через %1!d! с", L"Reiniciando em %1!d! segundos", L"将在 %1!d! 秒后重启", L"Uruchamianie ponowne za %1!d! sekund", L"Opnieuw starten over %1!d! seconden" },
    { 237, L"&Close", L"&Chiudi", L"&Cerrar", L"&Fermer", L"&Kapat", L"&Закрыть", L"&Fechar", L"&关闭", L"&Zamknij", L"&Sluiten" },
    { 238, L"Updates are available", L"Sono disponibili aggiornamenti", L"Hay actualizaciones disponibles", L"Des mises à jour sont disponibles", L"Güncellemeler kullanılabilir", L"Доступны обновления", L"Há atualizações disponíveis", L"有可用更新", L"Dostępne są aktualizacje", L"Er zijn updates beschikbaar" },
    { 239, L"Restart to finish updating your PC", L"Riavvia per completare l'aggiornamento del PC", L"Reinicie para terminar de actualizar su PC", L"Redémarrez pour terminer la mise à jour de votre PC", L"Bilgisayarınızın güncellemesini bitirmek için yeniden başlatın", L"Перезапустите компьютер, чтобы завершить обновление", L"Reinicie para concluir a atualização do PC", L"重启以完成电脑更新", L"Uruchom ponownie, aby zakończyć aktualizację komputera", L"Start opnieuw op om het bijwerken van uw pc te voltooien" },
    { 240, L"Go to Windows Update to install the updates now.", L"Vai a Windows Update per installare gli aggiornamenti ora.", L"Vaya a Windows Update para instalar las actualizaciones ahora.", L"Accédez à Windows Update pour installer les mises à jour maintenant.", L"Güncellemeleri şimdi yüklemek için Windows Update'e gidin.", L"Перейдите в Центр обновления Windows, чтобы установить обновления сейчас.", L"Vá para Windows Update para instalar as atualizações agora.", L"转到 Windows 更新以立即安装更新。", L"Przejdź do Windows Update, aby teraz zainstalować aktualizacje.", L"Ga naar Windows Update om de updates nu te installeren." },
    { 241, L"Windows Update couldn't install the updates automatically. Go to Windows Update to install them now.", L"Windows Update non ha potuto installare automaticamente gli aggiornamenti. Vai a Windows Update per installarli ora.", L"Windows Update no pudo instalar las actualizaciones automáticamente. Vaya a Windows Update para instalarlas ahora.", L"Windows Update n'a pas pu installer automatiquement les mises à jour. Accédez à Windows Update pour les installer maintenant.", L"Windows Update güncellemeleri otomatik olarak yükleyemedi. Bunları şimdi yüklemek için Windows Update'e gidin.", L"Центру обновления Windows не удалось установить обновления автоматически. Перейдите в Центр обновления Windows, чтобы установить их сейчас.", L"O Windows Update não conseguiu instalar as atualizações automaticamente. Vá para Windows Update para instalá-las agora.", L"Windows 更新无法自动安装更新。请转到 Windows 更新立即安装。", L"Windows Update nie mógł automatycznie zainstalować aktualizacji. Przejdź do Windows Update, aby je teraz zainstalować.", L"Windows Update kon de updates niet automatisch installeren. Ga naar Windows Update om ze nu te installeren." },
    { 242, L"Save your work, and restart your PC now to finish installing important updates. If you choose 'Later', your PC will automatically restart in 1 day.", L"Salva il lavoro e riavvia il PC ora per completare l'installazione degli aggiornamenti importanti. Se scegli 'Dopo', il PC si riavvierà automaticamente entro 1 giorno.", L"Guarde su trabajo y reinicie su PC ahora para terminar de instalar las actualizaciones importantes. Si elige 'Más tarde', su PC se reiniciará automáticamente en 1 día.", L"Enregistrez votre travail et redémarrez votre PC maintenant pour terminer l'installation des mises à jour importantes. Si vous choisissez 'Plus tard', votre PC redémarrera automatiquement dans 1 jour.", L"Çalışmanızı kaydedin ve önemli güncellemelerin yüklenmesini bitirmek için bilgisayarınızı şimdi yeniden başlatın. 'Sonra' seçerseniz bilgisayarınız 1 gün içinde otomatik olarak yeniden başlatılır.", L"Сохраните работу и перезапустите компьютер, чтобы завершить установку важных обновлений. Если вы выберете «Позже», компьютер будет автоматически перезапущен через 1 день.", L"Salve seu trabalho e reinicie o PC agora para concluir a instalação de atualizações importantes. Se escolher 'Mais tarde', seu PC reiniciará automaticamente em 1 dia.", L"保存你的工作，然后立即重启电脑以完成重要更新的安装。如果选择“稍后”，你的电脑将在 1 天后自动重启。", L"Zapisz swoją pracę i uruchom ponownie komputer, aby zakończyć instalowanie ważnych aktualizacji. Jeśli wybierzesz 'Później', komputer uruchomi się ponownie automatycznie za 1 dzień.", L"Sla uw werk op en start uw pc nu opnieuw op om de installatie van belangrijke updates te voltooien. Als u 'Later' kiest, wordt uw pc automatisch binnen 1 dag opnieuw gestart." },
    { 243, L"&Install", L"&Installa", L"&Instalar", L"&Installer", L"&Yükle", L"&Установить", L"&Instalar", L"&安装", L"&Zainstaluj", L"&Installeren" },
    { 244, L"Windows Update needs your help", L"Windows Update richiede il tuo intervento", L"Windows Update necesita su ayuda", L"Windows Update a besoin de votre aide", L"Windows Update yardımınızı gerektiriyor", L"Центру обновления Windows нужна ваша помощь", L"O Windows Update precisa da sua ajuda", L"Windows 更新需要你的帮助", L"Windows Update wymaga Twojej pomocy", L"Windows Update heeft uw hulp nodig" },
    { 245, L"Windows Update hasn't been able to check for new updates for the last 30 days. Go to Windows Update to resolve this issue.", L"Windows Update non è riuscito a cercare nuovi aggiornamenti negli ultimi 30 giorni. Vai a Windows Update per risolvere il problema.", L"Windows Update no ha podido buscar nuevas actualizaciones en los últimos 30 días. Vaya a Windows Update para resolver este problema.", L"Windows Update n'a pas pu rechercher de nouvelles mises à jour pendant les 30 derniers jours. Accédez à Windows Update pour résoudre ce problème.", L"Windows Update son 30 gündür yeni güncellemeleri denetleyemedi. Bu sorunu çözmek için Windows Update'e gidin.", L"Центр обновления Windows не мог проверять наличие новых обновлений последние 30 дней. Перейдите в Центр обновления Windows, чтобы решить эту проблему.", L"O Windows Update não consegue verificar novas atualizações há 30 dias. Vá para Windows Update para resolver esse problema.", L"Windows 更新在过去 30 天内无法检查新更新。请转到 Windows 更新以解决此问题。", L"Windows Update nie mógł sprawdzać nowych aktualizacji przez ostatnie 30 dni. Przejdź do Windows Update, aby rozwiązać ten problem.", L"Windows Update kan al 30 dagen geen nieuwe updates controleren. Ga naar Windows Update om dit probleem op te lossen." },
    { 246, L"Go to &Windows Update", L"Vai a &Windows Update", L"Ir a &Windows Update", L"Accéder à &Windows Update", L"&Windows Update'e git", L"Перейти в &Центр обновления Windows", L"Ir para &Windows Update", L"转到 &Windows 更新", L"Przejdź do &Windows Update", L"Naar &Windows Update gaan" },
    { 247, L"Your PC needs to restart to install a firmware update. If you've already saved everything, you can restart now. Otherwise, you should take a moment to save your work.", L"Il tuo PC deve essere riavviato per installare un aggiornamento del firmware. Se hai già salvato tutto, puoi riavviare ora. In caso contrario, prenditi un momento per salvare il lavoro.", L"Su PC debe reiniciarse para instalar una actualización de firmware. Si ya guardó todo, puede reiniciar ahora. De lo contrario, debe tomarse un momento para guardar su trabajo.", L"Votre PC doit redémarrer pour installer une mise à jour du firmware. Si vous avez déjà tout enregistré, vous pouvez redémarrer maintenant. Sinon, prenez un moment pour enregistrer votre travail.", L"Bilgisayarınızın bir üretici yazılımı güncellemesi yüklemek için yeniden başlatılması gerekiyor. Her şeyi kaydettiyseniz şimdi yeniden başlatabilirsiniz. Aksi takdirde, çalışmanızı kaydetmek için bir dakikanızı ayırın.", L"Компьютер необходимо перезапустить, чтобы установить обновление встроенного ПО. Если вы уже сохранили все, можно перезапустить сейчас. В противном случае сохраните свою работу.", L"Seu PC precisa ser reiniciado para instalar uma atualização de firmware. Se já salvou tudo, reinicie agora. Caso contrário, reserve um momento para salvar seu trabalho.", L"你的电脑需要重启才能安装固件更新。如果已保存所有内容，现在即可重启。否则，请花点时间保存你的工作。", L"Komputer należy ponownie uruchomić, aby zainstalować aktualizację oprogramowania układowego. Jeśli wszystko zapisałeś, uruchom ponownie teraz. W przeciwnym razie poświęć chwilę na zapisanie pracy.", L"Uw pc moet opnieuw worden gestart om een firmware-update te installeren. Als u alles al hebt opgeslagen, kunt u nu opnieuw starten. Anders neemt u even de tijd om uw werk op te slaan." },
    { 248, L"This will update your PC's hardware to help make it more stable. You can install this update by going to Windows Update.", L"Questo aggiornerà l'hardware del tuo PC per renderlo più stabile. Puoi installare questo aggiornamento andando in Windows Update.", L"Esto actualizará el hardware de su PC para ayudarlo a ser más estable. Puede instalar esta actualización yendo a Windows Update.", L"Cela mettra à jour le matériel de votre PC pour le rendre plus stable. Vous pouvez installer cette mise à jour en accédant à Windows Update.", L"Bu, bilgisayarınızın donanımını daha kararlı hale getirmek için güncelleyecektir. Bu güncellemeyi Windows Update'e giderek yükleyebilirsiniz.", L"Это обновит оборудование компьютера, чтобы сделать его более стабильным. Вы можете установить это обновление, перейдя в Центр обновления Windows.", L"Isso atualizará o hardware do seu PC para torná-lo mais estável. Você pode instalar esta atualização indo para Windows Update.", L"这将更新你电脑的硬件以使其更稳定。你可以通过转到 Windows 更新来安装此更新。", L"To zaktualizuje sprzęt komputera, aby był bardziej stabilny. Tę aktualizację możesz zainstalować, przechodząc do Windows Update.", L"Dit werkt de hardware van uw pc bij om deze stabieler te maken. U kunt deze update installeren door naar Windows Update te gaan." },
    { 249, L"Restarting in %1!d! minute, %2!d! seconds", L"Riavvio tra %1!d! minuto e %2!d! secondi", L"Reiniciando en %1!d! minuto y %2!d! segundos", L"Redémarrage dans %1!d! minute et %2!d! secondes", L"%1!d! dakika %2!d! saniye içinde yeniden başlatılıyor", L"Перезапуск через %1!d! мин и %2!d! с", L"Reiniciando em %1!d! minuto e %2!d! segundos", L"将在 %1!d! 分 %2!d! 秒后重启", L"Uruchamianie ponowne za %1!d! minutę i %2!d! sekund", L"Opnieuw starten over %1!d! minuut en %2!d! seconden" },
    { 250, L"Restarting in %1!d! minutes, %2!d! second", L"Riavvio tra %1!d! minuti e %2!d! secondo", L"Reiniciando en %1!d! minutos y %2!d! segundo", L"Redémarrage dans %1!d! minutes et %2!d! seconde", L"%1!d! dakika %2!d! saniye içinde yeniden başlatılıyor", L"Перезапуск через %1!d! мин и %2!d! с", L"Reiniciando em %1!d! minutos e %2!d! segundo", L"将在 %1!d! 分 %2!d! 秒后重启", L"Uruchamianie ponowne za %1!d! minut i %2!d! sekundę", L"Opnieuw starten over %1!d! minuten en %2!d! seconde" },
    { 251, L"Restarting in %1!d! minute, %2!d! second", L"Riavvio tra %1!d! minuto e %2!d! secondo", L"Reiniciando en %1!d! minuto y %2!d! segundo", L"Redémarrage dans %1!d! minute et %2!d! seconde", L"%1!d! dakika %2!d! saniye içinde yeniden başlatılıyor", L"Перезапуск через %1!d! мин и %2!d! с", L"Reiniciando em %1!d! minuto e %2!d! segundo", L"将在 %1!d! 分 %2!d! 秒后重启", L"Uruchamianie ponowne za %1!d! minutę i %2!d! sekundę", L"Opnieuw starten over %1!d! minuut en %2!d! seconde" },
    { 252, L"Restarting in %1!d! second", L"Riavvio tra %1!d! secondo", L"Reiniciando en %1!d! segundo", L"Redémarrage dans %1!d! seconde", L"%1!d! saniye içinde yeniden başlatılıyor", L"Перезапуск через %1!d! с", L"Reiniciando em %1!d! segundo", L"将在 %1!d! 秒后重启", L"Uruchamianie ponowne za %1!d! sekundę", L"Opnieuw starten over %1!d! seconde" },
    { 253, L"A firmware update is available", L"È disponibile un aggiornamento del firmware", L"Hay una actualización de firmware disponible", L"Une mise à jour du firmware est disponible", L"Bir üretici yazılımı güncellemesi kullanılabilir", L"Доступно обновление встроенного ПО", L"Há uma atualização de firmware disponível", L"有可用的固件更新", L"Dostępna jest aktualizacja oprogramowania układowego", L"Er is een firmware-update beschikbaar" },
    { 254, L"&Later", L"&Dopo", L"&Más tarde", L"&Plus tard", L"&Sonra", L"&Позже", L"&Mais tarde", L"&稍后", L"&Później", L"&Later" },
    { 300, L"%1 is also available", L"È disponibile anche %1", L"%1 también está disponible", L"%1 est également disponible", L"%1 de kullanılabilir", L"%1 также доступно", L"%1 também está disponível", L"%1 也可用", L"%1 jest również dostępne", L"%1 is ook beschikbaar" },
    { 301, L"%1 are also available", L"%1 sono anche disponibili", L"%1 también están disponibles", L"%1 sont également disponibles", L"%1 de kullanılabilir", L"%1 также доступны", L"%1 também estão disponíveis", L"%1 也可用", L"%1 są również dostępne", L"%1 zijn ook beschikbaar" },
    { 302, L"%1 is available", L"%1 è disponibile", L"%1 está disponible", L"%1 est disponible", L"%1 kullanılabilir", L"%1 доступно", L"%1 está disponível", L"%1 可用", L"%1 jest dostępne", L"%1 is beschikbaar" },
    { 303, L"%1 are available", L"%1 sono disponibili", L"%1 están disponibles", L"%1 sont disponibles", L"%1 kullanılabilir", L"%1 доступны", L"%1 estão disponíveis", L"%1 可用", L"%1 są dostępne", L"%1 zijn beschikbaar" },
    { 304, L"No important updates available", L"Nessun aggiornamento importante disponibile", L"No hay actualizaciones importantes disponibles", L"Aucune mise à jour importante disponible", L"Önemli güncelleme yok", L"Нет доступных важных обновлений", L"Nenhuma atualização importante disponível", L"没有可用的重要更新", L"Brak ważnych aktualizacji", L"Geen belangrijke updates beschikbaar" },
    { 305, L"There are no important updates available for your PC.", L"Non sono disponibili aggiornamenti importanti per il tuo PC.", L"No hay actualizaciones importantes disponibles para su PC.", L"Aucune mise à jour importante n'est disponible pour votre PC.", L"Bilgisayarınız için önemli güncelleme yok.", L"Нет доступных важных обновлений для вашего компьютера.", L"Não há atualizações importantes disponíveis para seu PC.", L"没有适用于你电脑的重要更新。", L"Brak ważnych aktualizacji dla Twojego komputera.", L"Er zijn geen belangrijke updates beschikbaar voor uw pc." },
    { 306, L"There are no updates available for your PC.", L"Non sono disponibili aggiornamenti per il tuo PC.", L"No hay actualizaciones disponibles para su PC.", L"Aucune mise à jour n'est disponible pour votre PC.", L"Bilgisayarınız için güncelleme yok.", L"Нет доступных обновлений для вашего компьютера.", L"Não há atualizações disponíveis para seu PC.", L"没有适用于你电脑的更新。", L"Brak aktualizacji dla Twojego komputera.", L"Er zijn geen updates beschikbaar voor uw pc." },
    { 308, L"%d optional updates", L"%d aggiornamenti facoltativi", L"%d actualizaciones opcionales", L"%d mises à jour facultatives", L"%d isteğe bağlı güncelleme", L"%d необязательных обновлений", L"%d atualizações opcionais", L"%d 个可选更新", L"%d opcjonalnych aktualizacji", L"%d optionele updates" },
    { 318, L"Important updates", L"Aggiornamenti importanti", L"Actualizaciones importantes", L"Mises à jour importantes", L"Önemli güncellemeler", L"Важные обновления", L"Atualizações importantes", L"重要更新", L"Ważne aktualizacje", L"Belangrijke updates" },
    { 319, L"Important and recommended updates", L"Aggiornamenti importanti e consigliati", L"Actualizaciones importantes y recomendadas", L"Mises à jour importantes et recommandées", L"Önemli ve önerilen güncellemeler", L"Важные и рекомендуемые обновления", L"Atualizações importantes e recomendadas", L"重要和推荐更新", L"Ważne i zalecane aktualizacje", L"Belangrijke en aanbevolen updates" },
    { 320, L"Optional updates", L"Aggiornamenti facoltativi", L"Actualizaciones opcionales", L"Mises à jour facultatives", L"İsteğe bağlı güncellemeler", L"Необязательные обновления", L"Atualizações opcionais", L"可选更新", L"Opcjonalne aktualizacje", L"Optionele updates" },
    { 321, L"Recommended and optional updates", L"Aggiornamenti consigliati e facoltativi", L"Actualizaciones recomendadas y opcionales", L"Mises à jour recommandées et facultatives", L"Önerilen ve isteğe bağlı güncellemeler", L"Рекомендуемые и необязательные обновления", L"Atualizações recomendadas e opcionais", L"推荐和可选更新", L"Zalecane i opcjonalne aktualizacje", L"Aanbevolen en optionele updates" },
    { 323, L"Total selected: %s, %s (%s)", L"Selezionati in totale: %s, %s (%s)", L"Seleccionados en total: %s, %s (%s)", L"Sélectionnés au total : %s, %s (%s)", L"Toplam seçilen: %s, %s (%s)", L"Выбрано всего: %s, %s (%s)", L"Selecionadas no total: %s, %s (%s)", L"总选定的项：%s、%s（%s）", L"Łącznie wybrano: %s, %s (%s)", L"Totaal geselecteerd: %s, %s (%s)" },
    { 324, L"It is recommended to use the system settings to configure updates.", L"Si consiglia di utilizzare le impostazioni del sistema per configurare gli aggiornamenti.", L"Se recomienda usar la configuración del sistema para configurar las actualizaciones.", L"Il est recommandé d'utiliser les paramètres du système pour configurer les mises à jour.", L"Güncellemeleri yapılandırmak için sistem ayarlarını kullanmanız önerilir.", L"Рекомендуется использовать параметры системы для настройки обновлений.", L"Recomenda-se usar as configurações do sistema para configurar as atualizações.", L"建议使用系统设置来配置更新。", L"Zaleca się korzystanie z ustawień systemowych w celu skonfigurowania aktualizacji.", L"Het wordt aanbevolen om de systeeminstellingen te gebruiken om updates te configureren." },
    { 325, L"Importance", L"Importanza", L"Importancia", L"Importance", L"Önem", L"Важность", L"Importância", L"重要性", L"Ważność", L"Belangrijkheid" },
    { 326, L"Size", L"Dimensione", L"Tamaño", L"Taille", L"Boyut", L"Размер", L"Tamanho", L"大小", L"Rozmiar", L"Grootte" },
    { 327, L"Select optional updates to install", L"Seleziona gli aggiornamenti facoltativi da installare", L"Seleccionar las actualizaciones opcionales que se van a instalar", L"Sélectionner les mises à jour facultatives à installer", L"Yüklenecek isteğe bağlı güncellemeleri seçin", L"Выбор необязательных обновлений для установки", L"Selecione as atualizações opcionais a instalar", L"选择要安装的可选更新", L"Wybierz opcjonalne aktualizacje do zainstalowania", L"Selecteer optionele updates om te installeren" },
    { 328, L"Review optional updates", L"Rivedi gli aggiornamenti facoltativi", L"Revisar las actualizaciones opcionales", L"Passer en revue les mises à jour facultatives", L"İsteğe bağlı güncellemeleri gözden geçirin", L"Просмотр необязательных обновлений", L"Revisar atualizações opcionais", L"查看可选更新", L"Przejrzyj opcjonalne aktualizacje", L"Optionele updates controleren" },
    { 329, L"Select important updates to install", L"Seleziona gli aggiornamenti importanti da installare", L"Seleccionar las actualizaciones importantes que se van a instalar", L"Sélectionner les mises à jour importantes à installer", L"Yüklenecek önemli güncellemeleri seçin", L"Выбор важных обновлений для установки", L"Selecione as atualizações importantes a instalar", L"选择要安装的重要更新", L"Wybierz ważne aktualizacje do zainstalowania", L"Selecteer belangrijke updates om te installeren" },
    { 330, L"Review important updates", L"Rivedi gli aggiornamenti importanti", L"Revisar las actualizaciones importantes", L"Passer en revue les mises à jour importantes", L"Önemli güncellemeleri gözden geçirin", L"Просмотр важных обновлений", L"Revisar atualizações importantes", L"查看重要更新", L"Przejrzyj ważne aktualizacje", L"Belangrijke updates controleren" },
    { 331, L"Review all important updates", L"Rivedi tutti gli aggiornamenti importanti", L"Revisar todas las actualizaciones importantes", L"Passer en revue toutes les mises à jour importantes", L"Tüm önemli güncellemeleri gözden geçirin", L"Просмотр всех важных обновлений", L"Revisar todas as atualizações importantes", L"查看所有重要更新", L"Przejrzyj wszystkie ważne aktualizacje", L"Alle belangrijke updates controleren" },
    { 333, L"Downloaded", L"Scaricato", L"Descargada", L"Téléchargée", L"İndirildi", L"Загружено", L"Baixado", L"已下载", L"Pobrano", L"Gedownload" },
    { 334, L"Install updates automatically (recommended)", L"Installa gli aggiornamenti automaticamente (scelta consigliata)", L"Instalar actualizaciones automáticamente (recomendado)", L"Installer automatiquement les mises à jour (recommandé)", L"Güncellemeleri otomatik olarak yükle (önerilir)", L"Автоматически устанавливать обновления (рекомендуется)", L"Instalar atualizações automaticamente (recomendado)", L"自动安装更新（推荐）", L"Automatycznie instaluj aktualizacje (zalecane)", L"Updates automatisch installeren (aanbevolen)" },
    { 335, L"Download updates but let me choose whether to install them", L"Scarica gli aggiornamenti ma consenti di scegliere se installarli", L"Descargar actualizaciones pero permitirme elegir si instalarlas", L"Télécharger les mises à jour mais me laisser choisir de les installer", L"Güncellemeleri indir, ancak bunları yükleyip yüklemeyeceğimi ben seçeyim", L"Загружать обновления, но я сам решу, устанавливать ли их", L"Baixar atualizações, mas deixar que eu escolha se desejo instalá-las", L"下载更新，但让我选择是否安装", L"Pobieraj aktualizacje, ale pozwól mi wybrać, czy je zainstalować", L"Updates downloaden, maar mij laten kiezen of ik ze wil installeren" },
    { 336, L"Check for updates but let me choose whether to download and install them", L"Verifica la disponibilità di aggiornamenti ma consenti di scegliere se scaricarli e installarli", L"Comprobar actualizaciones pero permitirme elegir si descargarlas e instalarlas", L"Rechercher les mises à jour mais me laisser choisir de les télécharger et de les installer", L"Güncellemeleri denetle, ancak bunları indirip yükleyip yüklemeyeceğimi ben seçeyim", L"Проверять обновления, но я сам решу, загружать и устанавливать ли их", L"Verificar atualizações, mas deixar que eu escolha se desejo baixá-las e instalá-las", L"检查更新，但让我选择是否下载和安装", L"Sprawdzaj aktualizacje, ale pozwól mi wybrać, czy je pobrać i zainstalować", L"Controleren op updates, maar mij laten kiezen of ik ze wil downloaden en installeren" },
    { 337, L"Never check for updates (not recommended)", L"Non verificare mai la disponibilità di aggiornamenti (scelta sconsigliata)", L"No comprobar nunca las actualizaciones (no recomendado)", L"Ne jamais rechercher les mises à jour (non recommandé)", L"Güncellemeleri hiç denetleme (önerilmez)", L"Никогда не проверять обновления (не рекомендуется)", L"Nunca verificar atualizações (não recomendado)", L"从不检查更新（不推荐）", L"Nigdy nie sprawdzaj aktualizacji (niezalecane)", L"Nooit naar updates zoeken (niet aanbevolen)" },
    { 338, L"Please select an option:", L"Seleziona un'opzione:", L"Seleccione una opción:", L"Veuillez sélectionner une option :", L"Lütfen bir seçenek seçin:", L"Выберите вариант:", L"Selecione uma opção:", L"请选择选项：", L"Wybierz opcję:", L"Selecteer een optie:" },
    { 339, L"Download and install your selected updates", L"Scarica e installa gli aggiornamenti selezionati", L"Descargar e instalar las actualizaciones seleccionadas", L"Télécharger et installer les mises à jour sélectionnées", L"Seçilen güncellemeleri indirip yükleyin", L"Загрузить и установить выбранные обновления", L"Baixar e instalar as atualizações selecionadas", L"下载并安装所选更新", L"Pobierz i zainstaluj wybrane aktualizacje", L"Download de geselecteerde updates en installeer ze" },
    { 340, L"Install your selected updates", L"Installa gli aggiornamenti selezionati", L"Instalar las actualizaciones seleccionadas", L"Installer les mises à jour sélectionnées", L"Seçilen güncellemeleri yükleyin", L"Установить выбранные обновления", L"Instalar as atualizações selecionadas", L"安装所选更新", L"Zainstaluj wybrane aktualizacje", L"Installeer de geselecteerde updates" },
    { 341, L"%1!lu! important updates", L"%1!lu! aggiornamenti importanti", L"%1!lu! actualizaciones importantes", L"%1!lu! mises à jour importantes", L"%1!lu! önemli güncelleme", L"%1!lu! важных обновлений", L"%1!lu! atualizações importantes", L"%1!lu! 个重要更新", L"%1!lu! ważnych aktualizacji", L"%1!lu! belangrijke updates" },
    { 345, L"Windows Update can't check for updates because the service is not running. You may need to restart your PC.", L"Windows Update non può cercare aggiornamenti perché il servizio non è in esecuzione. Potrebbe essere necessario riavviare il PC.", L"Windows Update no puede buscar actualizaciones porque el servicio no se está ejecutando. Es posible que deba reiniciar su PC.", L"Windows Update ne peut pas rechercher les mises à jour car le service n'est pas en cours d'exécution. Vous devrez peut-être redémarrer votre PC.", L"Windows Update, hizmet çalışmadığı için güncellemeleri denetleyemiyor. Bilgisayarınızı yeniden başlatmanız gerekebilir.", L"Центр обновления Windows не может проверить наличие обновлений, так как служба не запущена. Возможно, потребуется перезапустить компьютер.", L"O Windows Update não pode verificar atualizações porque o serviço não está em execução. Talvez seja necessário reiniciar o PC.", L"Windows 更新无法检查更新，因为服务未运行。你可能需要重启电脑。", L"Windows Update nie może sprawdzać aktualizacji, ponieważ usługa nie jest uruchomiona. Może być konieczne ponowne uruchomienie komputera.", L"Windows Update kan geen updates controleren omdat de service niet wordt uitgevoerd. Mogelijk moet u uw pc opnieuw starten." },
    { 347, L"Before Windows Update can check for updates, you must first configure Windows Update's settings. You can do this using the 'Change settings' link located below the 'Check for updates' link.", L"Prima che Windows Update possa cercare aggiornamenti, devi configurare le impostazioni di Windows Update. Puoi farlo usando il collegamento 'Cambia impostazioni' situato sotto il collegamento 'Controlla aggiornamenti'.", L"Antes de que Windows Update pueda buscar actualizaciones, primero debe configurar los parámetros de Windows Update. Puede hacerlo mediante el vínculo 'Cambiar la configuración' situado debajo del vínculo 'Buscar actualizaciones'.", L"Avant que Windows Update puisse rechercher des mises à jour, vous devez d'abord configurer les paramètres de Windows Update. Vous pouvez le faire à l'aide du lien « Modifier les paramètres » situé sous le lien « Rechercher les mises à jour ».", L"Windows Update güncellemeleri denetlemeden önce Windows Update ayarlarını yapılandırmanız gerekir. Bunu 'Denetle' bağlantısının altında bulunan 'Ayarları değiştir' bağlantısını kullanarak yapabilirsiniz.", L"Прежде чем Центр обновления Windows сможет проверить наличие обновлений, необходимо настроить его параметры. Это можно сделать с помощью ссылки «Изменить параметры», расположенной ниже ссылки «Проверить наличие обновлений».", L"Antes que o Windows Update possa verificar atualizações, você deve primeiro configurar as configurações do Windows Update. Você pode fazer isso usando o link 'Alterar configurações' localizado abaixo do link 'Verificar atualizações'.", L"在 Windows 更新可以检查更新之前，必须先配置 Windows 更新的设置。你可以使用位于“检查更新”链接下方的“更改设置”链接进行配置。", L"Zanim Windows Update będzie mógł sprawdzać aktualizacje, musisz najpierw skonfigurować ustawienia Windows Update. Możesz to zrobić, używając łącza 'Zmień ustawienia' znajdującego się poniżej łącza 'Sprawdź aktualizacje'.", L"Voordat Windows Update updates kan controleren, moet u eerst de instellingen van Windows Update configureren. U kunt dit doen met de koppeling 'Instellingen wijzigen' onder de koppeling 'Controleren op updates'." },
    { 348, L"Windows Update is already checking for, downloading, or installing updates.", L"Windows Update sta già cercando, scaricando o installando aggiornamenti.", L"Windows Update ya está comprobando, descargando o instalando actualizaciones.", L"Windows Update recherche, télécharge ou installe déjà des mises à jour.", L"Windows Update zaten güncellemeleri denetliyor, indiriyor veya yüklüyor.", L"Центр обновления Windows уже проверяет, загружает или устанавливает обновления.", L"O Windows Update já está verificando, baixando ou instalando atualizações.", L"Windows 更新已在检查、下载或安装更新。", L"Windows Update już sprawdza, pobiera lub instaluje aktualizacje.", L"Windows Update controleert al op updates, downloadt of installeert deze." },
    { 349, L"Windows Update can't check for updates because settings on this PC are controlled by your system administrator.", L"Windows Update non può cercare aggiornamenti perché le impostazioni di questo PC sono controllate dall'amministratore di sistema.", L"Windows Update no puede buscar actualizaciones porque la configuración de este PC está controlada por el administrador del sistema.", L"Windows Update ne peut pas rechercher les mises à jour car les paramètres de ce PC sont contrôlés par votre administrateur système.", L"Windows Update, bu bilgisayardaki ayarlar sistem yöneticiniz tarafından denetlendiği için güncellemeleri denetleyemiyor.", L"Центр обновления Windows не может проверить наличие обновлений, так как параметры этого компьютера управляются системным администратором.", L"O Windows Update não pode verificar atualizações porque as configurações deste PC são controladas pelo administrador do sistema.", L"Windows 更新无法检查更新，因为此电脑上的设置由你的系统管理员控制。", L"Windows Update nie może sprawdzać aktualizacji, ponieważ ustawienia tego komputera są kontrolowane przez administratora systemu.", L"Windows Update kan geen updates controleren omdat de instellingen op deze pc door uw systeembeheerder worden beheerd." },
    { 350, L"Check for updates", L"Controlla aggiornamenti", L"Buscar actualizaciones", L"Rechercher des mises à jour", L"Güncellemeleri denetle", L"Проверить наличие обновлений", L"Verificar atualizações", L"检查更新", L"Sprawdź aktualizacje", L"Controleren op updates" },
    { 351, L"Change settings", L"Cambia impostazioni", L"Cambiar la configuración", L"Modifier les paramètres", L"Ayarları değiştir", L"Изменить параметры", L"Alterar configurações", L"更改设置", L"Zmień ustawienia", L"Instellingen wijzigen" },
    { 352, L"View update history", L"Visualizza cronologia aggiornamenti", L"Ver historial de actualizaciones", L"Afficher l'historique des mises à jour", L"Güncelleme geçmişini görüntüle", L"Просмотр журнала обновлений", L"Exibir histórico de atualizações", L"查看更新历史记录", L"Wyświetl historię aktualizacji", L"Updategeschiedenis weergeven" },
    { 353, L"Restore hidden updates", L"Ripristina aggiornamenti nascosti", L"Restaurar actualizaciones ocultas", L"Restaurer les mises à jour masquées", L"Gizli güncellemeleri geri yükle", L"Восстановить скрытые обновления", L"Restaurar atualizações ocultas", L"还原隐藏的更新", L"Przywróć ukryte aktualizacje", L"Verborgen updates herstellen" },
    { 355, L"Security Center", L"Centro sicurezza", L"Centro de seguridad", L"Centre de sécurité", L"Güvenlik Merkezi", L"Центр безопасности", L"Central de Segurança", L"安全中心", L"Centrum zabezpieczeń", L"Beveiligingscentrum" },
    { 356, L"Installed Updates", L"Aggiornamenti installati", L"Actualizaciones instaladas", L"Mises à jour installées", L"Yüklü Güncellemeler", L"Установленные обновления", L"Atualizações Instaladas", L"已安装的更新", L"Zainstalowane aktualizacje", L"Geïnstalleerde updates" },
    { 358, L"Add features to %WINDOWS_SHORT%", L"Aggiungi funzionalità a %WINDOWS_SHORT%", L"Agregar características a %WINDOWS_SHORT%", L"Ajouter des fonctionnalités à %WINDOWS_SHORT%", L"%WINDOWS_SHORT% için özellik ekle", L"Добавить компоненты в %WINDOWS_SHORT%", L"Adicionar recursos ao %WINDOWS_SHORT%", L"向 %WINDOWS_SHORT% 添加功能", L"Dodaj funkcje do %WINDOWS_SHORT%", L"Functies toevoegen aan %WINDOWS_SHORT%" },
    { 371, L"%1 selected", L"%1 selezionati", L"%1 seleccionados", L"%1 sélectionnés", L"%1 seçildi", L"Выбрано: %1", L"%1 selecionadas", L"已选定 %1", L"Wybrano: %1", L"%1 geselecteerd" },
    { 372, L"%1 selected", L"%1 selezionati", L"%1 seleccionados", L"%1 sélectionnés", L"%1 seçildi", L"Выбрано: %1", L"%1 selecionadas", L"已选定 %1", L"Wybrano: %1", L"%1 geselecteerd" },
    { 373, L"The updates were installed", L"Gli aggiornamenti sono stati installati", L"Las actualizaciones se instalaron", L"Les mises à jour ont été installées", L"Güncellemeler yüklendi", L"Обновления установлены", L"As atualizações foram instaladas", L"更新已安装", L"Aktualizacje zostały zainstalowane", L"De updates zijn geïnstalleerd" },
    { 374, L"Some updates were not installed", L"Alcuni aggiornamenti non sono stati installati", L"Algunas actualizaciones no se instalaron", L"Certaines mises à jour n'ont pas été installées", L"Bazı güncellemeler yüklenmedi", L"Некоторые обновления не были установлены", L"Algumas atualizações não foram instaladas", L"某些更新未安装", L"Niektóre aktualizacje nie zostały zainstalowane", L"Sommige updates zijn niet geïnstalleerd" },
    { 375, L"1 pending important update", L"1 aggiornamento importante in sospeso", L"1 actualización importante pendiente", L"1 mise à jour importante en attente", L"1 bekleyen önemli güncelleme", L"1 ожидающее важное обновление", L"1 atualização importante pendente", L"1 个待处理的重要更新", L"1 oczekująca ważna aktualizacja", L"1 lopende belangrijke update" },
    { 376, L"%d pending important updates", L"%d aggiornamenti importanti in sospeso", L"%d actualizaciones importantes pendientes", L"%d mises à jour importantes en attente", L"%d bekleyen önemli güncelleme", L"%d ожидающих важных обновлений", L"%d atualizações importantes pendentes", L"%d 个待处理的重要更新", L"%d oczekujących ważnych aktualizacji", L"%d lopende belangrijke updates" },
    { 377, L"%1!lu! pending important updates", L"%1!lu! aggiornamenti importanti in sospeso", L"%1!lu! actualizaciones importantes pendientes", L"%1!lu! mises à jour importantes en attente", L"%1!lu! bekleyen önemli güncelleme", L"%1!lu! ожидающих важных обновлений", L"%1!lu! atualizações importantes pendentes", L"%1!lu! 个待处理的重要更新", L"%1!lu! oczekujących ważnych aktualizacji", L"%1!lu! lopende belangrijke updates" },
    { 378, L"There was a problem checking for updates.", L"Si è verificato un problema durante la ricerca degli aggiornamenti.", L"Hubo un problema al buscar actualizaciones.", L"Un problème est survenu lors de la recherche des mises à jour.", L"Güncellemeler denetlenirken bir sorun oluştu.", L"При проверке обновлений возникла проблема.", L"Houve um problema ao verificar atualizações.", L"检查更新时出现问题。", L"Wystąpił problem podczas sprawdzania aktualizacji.", L"Er is een probleem opgetreden bij het controleren op updates." },
    { 380, L"&Hide updates", L"&Nascondi aggiornamenti", L"&Ocultar actualizaciones", L"&Masquer les mises à jour", L"Güncellemeleri &gizle", L"&Скрыть обновления", L"&Ocultar atualizações", L"&隐藏更新", L"&Ukryj aktualizacje", L"Updates &verbergen" },
    { 381, L"&Restore updates", L"&Ripristina aggiornamenti", L"&Restaurar actualizaciones", L"&Restaurer les mises à jour", L"Güncellemeleri &geri yükle", L"&Восстановить обновления", L"&Restaurar atualizações", L"&还原更新", L"&Przywróć aktualizacje", L"Updates &herstellen" },
    { 382, L"Updates are available for your PC", L"Sono disponibili aggiornamenti per il tuo PC", L"Hay actualizaciones disponibles para su PC", L"Des mises à jour sont disponibles pour votre PC", L"Bilgisayarınız için güncellemeler kullanılabilir", L"Для вашего компьютера доступны обновления", L"Há atualizações disponíveis para seu PC", L"有适用于你电脑的更新", L"Dostępne są aktualizacje dla Twojego komputera", L"Er zijn updates beschikbaar voor uw pc" },
    { 383, L"Important updates are available for your PC", L"Sono disponibili aggiornamenti importanti per il tuo PC", L"Hay actualizaciones importantes disponibles para su PC", L"Des mises à jour importantes sont disponibles pour votre PC", L"Bilgisayarınız için önemli güncellemeler kullanılabilir", L"Для вашего компьютера доступны важные обновления", L"Há atualizações importantes disponíveis para seu PC", L"有适用于你电脑的重要更新", L"Dostępne są ważne aktualizacje dla Twojego komputera", L"Er zijn belangrijke updates beschikbaar voor uw pc" },
    { 384, L"%1 selected, %2", L"%1 selezionati, %2", L"%1 seleccionados, %2", L"%1 sélectionnés, %2", L"%1 seçildi, %2", L"Выбрано: %1, %2", L"%1 selecionadas, %2", L"已选定 %1，%2", L"Wybrano: %1, %2", L"%1 geselecteerd, %2" },
    { 385, L"There was a problem getting the list of updates for your PC. To continue, please reopen Windows Update. (Error code: %1!X!)", L"Si è verificato un problema durante il recupero dell'elenco degli aggiornamenti per il tuo PC. Per continuare, riapri Windows Update. (Codice di errore: %1!X!)", L"Hubo un problema al obtener la lista de actualizaciones para su PC. Para continuar, vuelva a abrir Windows Update. (Código de error: %1!X!)", L"Un problème est survenu lors de l'obtention de la liste des mises à jour pour votre PC. Pour continuer, rouvrez Windows Update. (Code d'erreur : %1!X!)", L"Bilgisayarınız için güncelleme listesi alınırken bir sorun oluştu. Devam etmek için Windows Update'i yeniden açın. (Hata kodu: %1!X!)", L"При получении списка обновлений для компьютера возникла проблема. Чтобы продолжить, снова откройте Центр обновления Windows. (Код ошибки: %1!X!)", L"Houve um problema ao obter a lista de atualizações para seu PC. Para continuar, reabra o Windows Update. (Código de erro: %1!X!)", L"获取你电脑的更新列表时出现问题。若要继续，请重新打开 Windows 更新。（错误代码：%1!X!）", L"Wystąpił problem podczas pobierania listy aktualizacji dla Twojego komputera. Aby kontynuować, otwórz ponownie Windows Update. (Kod błędu: %1!X!)", L"Er is een probleem opgetreden bij het ophalen van de updatelijst voor uw pc. Open Windows Update opnieuw om verder te gaan. (Foutcode: %1!X!)" },
    { 386, L"System Firmware Update - %s", L"Aggiornamento firmware di sistema - %s", L"Actualización de firmware del sistema - %s", L"Mise à jour du firmware système - %s", L"Sistem Üretici Yazılımı Güncellemesi - %s", L"Обновление системного встроенного ПО - %s", L"Atualização de firmware do sistema - %s", L"系统固件更新 - %s", L"Aktualizacja oprogramowania układowego systemu - %s", L"Systeemfirmware-update - %s" },
    { 387, L"System Hardware Update - %s", L"Aggiornamento hardware di sistema - %s", L"Actualización de hardware del sistema - %s", L"Mise à jour du matériel système - %s", L"Sistem Donanımı Güncellemesi - %s", L"Обновление системного оборудования - %s", L"Atualização de hardware do sistema - %s", L"系统硬件更新 - %s", L"Aktualizacja sprzętu systemu - %s", L"Systeemhardware-update - %s" },
    { 388, L"These updates are released by PC manufacturers to help improve the stability and performance of PC hardware.", L"Questi aggiornamenti vengono rilasciati dai produttori di PC per migliorare stabilità e prestazioni dell'hardware del PC.", L"Estas actualizaciones las publican los fabricantes de PC para ayudar a mejorar la estabilidad y el rendimiento del hardware del PC.", L"Ces mises à jour sont publiées par les fabricants de PC pour améliorer la stabilité et les performances du matériel du PC.", L"Bu güncellemeler, bilgisayar donanımının kararlılığını ve performansını iyileştirmek için PC üreticileri tarafından yayımlanır.", L"Эти обновления выпускаются производителями ПК для повышения стабильности и производительности оборудования.", L"Estas atualizações são lançadas pelos fabricantes de PCs para melhorar a estabilidade e o desempenho do hardware.", L"这些更新由电脑制造商发布，有助于提高电脑硬件的稳定性和性能。", L"Te aktualizacje są publikowane przez producentów komputerów, aby poprawić stabilność i wydajność sprzętu.", L"Deze updates worden door pc-fabrikanten uitgebracht om de stabiliteit en prestaties van pc-hardware te verbeteren." },
    { 450, L"Check", L"Controlla", L"Marcar", L"Cocher", L"İşaretle", L"Отметить", L"Marcar", L"选中", L"Zaznacz", L"Aanvinken" },
    { 451, L"Uncheck", L"Deseleziona", L"Desmarcar", L"Décocher", L"İşareti kaldır", L"Снять отметку", L"Desmarcar", L"取消选中", L"Odznacz", L"Uitvinken" },
    { 452, L"%1, %2", L"%1, %2", L"%1, %2", L"%1, %2", L"%1, %2", L"%1, %2", L"%1, %2", L"%1，%2", L"%1, %2", L"%1, %2" },
    { 453, L"Important update list, 1 update", L"Elenco aggiornamenti importanti, 1 aggiornamento", L"Lista de actualizaciones importantes, 1 actualización", L"Liste des mises à jour importantes, 1 mise à jour", L"Önemli güncelleme listesi, 1 güncelleme", L"Список важных обновлений: 1 обновление", L"Lista de atualizações importantes, 1 atualização", L"重要更新列表，1 个更新", L"Lista ważnych aktualizacji, 1 aktualizacja", L"Belangrijke updatelijst, 1 update" },
    { 454, L"Important update list, %d updates", L"Elenco aggiornamenti importanti, %d aggiornamenti", L"Lista de actualizaciones importantes, %d actualizaciones", L"Liste des mises à jour importantes, %d mises à jour", L"Önemli güncelleme listesi, %d güncelleme", L"Список важных обновлений: %d обновлений", L"Lista de atualizações importantes, %d atualizações", L"重要更新列表，%d 个更新", L"Lista ważnych aktualizacji, %d aktualizacji", L"Belangrijke updatelijst, %d updates" },
    { 455, L"Optional update list, 1 update", L"Elenco aggiornamenti facoltativi, 1 aggiornamento", L"Lista de actualizaciones opcionales, 1 actualización", L"Liste des mises à jour facultatives, 1 mise à jour", L"İsteğe bağlı güncelleme listesi, 1 güncelleme", L"Список необязательных обновлений: 1 обновление", L"Lista de atualizações opcionais, 1 atualização", L"可选更新列表，1 个更新", L"Lista opcjonalnych aktualizacji, 1 aktualizacja", L"Optionele updatelijst, 1 update" },
    { 456, L"Optional update list, %d updates", L"Elenco aggiornamenti facoltativi, %d aggiornamenti", L"Lista de actualizaciones opcionales, %d actualizaciones", L"Liste des mises à jour facultatives, %d mises à jour", L"İsteğe bağlı güncelleme listesi, %d güncelleme", L"Список необязательных обновлений: %d обновлений", L"Lista de atualizações opcionais, %d atualizações", L"可选更新列表，%d 个更新", L"Lista opcjonalnych aktualizacji, %d aktualizacji", L"Optionele updatelijst, %d updates" },
    { 457, L"Important update list, 1 update", L"Elenco aggiornamenti importanti, 1 aggiornamento", L"Lista de actualizaciones importantes, 1 actualización", L"Liste des mises à jour importantes, 1 mise à jour", L"Önemli güncelleme listesi, 1 güncelleme", L"Список важных обновлений: 1 обновление", L"Lista de atualizações importantes, 1 atualização", L"重要更新列表，1 个更新", L"Lista ważnych aktualizacji, 1 aktualizacja", L"Belangrijke updatelijst, 1 update" },
    { 458, L"Important update list, %d updates", L"Elenco aggiornamenti importanti, %d aggiornamenti", L"Lista de actualizaciones importantes, %d actualizaciones", L"Liste des mises à jour importantes, %d mises à jour", L"Önemli güncelleme listesi, %d güncelleme", L"Список важных обновлений: %d обновлений", L"Lista de atualizações importantes, %d atualizações", L"重要更新列表，%d 个更新", L"Lista ważnych aktualizacji, %d aktualizacji", L"Belangrijke updatelijst, %d updates" },
    { 459, L"Optional update list, 1 update", L"Elenco aggiornamenti facoltativi, 1 aggiornamento", L"Lista de actualizaciones opcionales, 1 actualización", L"Liste des mises à jour facultatives, 1 mise à jour", L"İsteğe bağlı güncelleme listesi, 1 güncelleme", L"Список необязательных обновлений: 1 обновление", L"Lista de atualizações opcionais, 1 atualização", L"可选更新列表，1 个更新", L"Lista opcjonalnych aktualizacji, 1 aktualizacja", L"Optionele updatelijst, 1 update" },
    { 460, L"Optional update list, %d updates", L"Elenco aggiornamenti facoltativi, %d aggiornamenti", L"Lista de actualizaciones opcionales, %d actualizaciones", L"Liste des mises à jour facultatives, %d mises à jour", L"İsteğe bağlı güncelleme listesi, %d güncelleme", L"Список необязательных обновлений: %d обновлений", L"Lista de atualizações opcionais, %d atualizações", L"可选更新列表，%d 个更新", L"Lista opcjonalnych aktualizacji, %d aktualizacji", L"Optionele updatelijst, %d updates" },
    { 475, L"&Give me updates for other Microsoft products when I update Windows", L"&Dammi aggiornamenti per altri prodotti Microsoft quando aggiorno Windows", L"&Darme actualizaciones para otros productos de Microsoft cuando actualizo Windows", L"Me donner les mises à jour pour d'autres produits Microsoft quand je mets à jour Windows", L"Windows'u güncellediğimde diğer Microsoft ürünleri için güncellemeler &ver", L"&Предоставлять обновления для других продуктов Microsoft при обновлении Windows", L"&Dar-me atualizações para outros produtos da Microsoft ao atualizar o Windows", L"更新 Windows 时为我提供其他 Microsoft 产品的更新（&G）", L"&Daj mi aktualizacje dla innych produktów Microsoft, gdy aktualizuję Windows", L"Geef mij updates voor andere Microsoft-producten wanneer ik Windows bijwerk (&G)" },
    { 480, L"Download is pending — select this update to start downloading it", L"Download in sospeso: seleziona questo aggiornamento per avviare il download", L"La descarga está pendiente: seleccione esta actualización para empezar a descargarla", L"Téléchargement en attente — sélectionnez cette mise à jour pour commencer le téléchargement", L"İndirme bekleniyor — indirmeye başlamak için bu güncellemeyi seçin", L"Загрузка ожидает — выберите это обновление, чтобы начать загрузку", L"Download pendente — selecione esta atualização para iniciar o download", L"下载处于挂起状态 — 选择此更新以开始下载", L"Pobieranie w toku — wybierz tę aktualizację, aby rozpocząć pobieranie", L"Download in behandeling — selecteer deze update om het downloaden te starten" },
    { 481, L"Total selected: %s (%s)", L"Selezionati in totale: %s (%s)", L"Seleccionados en total: %s (%s)", L"Sélectionnés au total : %s (%s)", L"Toplam seçilen: %s (%s)", L"Выбрано всего: %s (%s)", L"Selecionadas no total: %s (%s)", L"总选定的项：%s（%s）", L"Łącznie wybrano: %s (%s)", L"Totaal geselecteerd: %s (%s)" },
    { 482, L"See all available updates", L"Visualizza tutti gli aggiornamenti disponibili", L"Ver todas las actualizaciones disponibles", L"Voir toutes les mises à jour disponibles", L"Tüm kullanılabilir güncellemeleri gör", L"Просмотреть все доступные обновления", L"Ver todas as atualizações disponíveis", L"查看所有可用更新", L"Zobacz wszystkie dostępne aktualizacje", L"Alle beschikbare updates bekijken" },
    { 483, L"%1 will be installed.", L"%1 verrà installato.", L"%1 se instalará.", L"%1 sera installée.", L"%1 yüklenecek.", L"%1 будет установлено.", L"%1 será instalada.", L"将安装 %1。", L"%1 zostanie zainstalowana.", L"%1 wordt geïnstalleerd." },
    { 1000, L"Standard User Control", L"Controllo utente standard", L"Control de usuario estándar", L"Contrôle d'utilisateur standard", L"Standart Kullanıcı Denetimi", L"Стандартный контроль пользователя", L"Controle de usuário padrão", L"标准用户控制", L"Kontrola standardowego użytkownika", L"Standaardgebruikersbeheer" },
    { 1001, L"Allow standard users to install programs and updates with Windows Update.", L"Consenti agli utenti standard di installare programmi e aggiornamenti con Windows Update.", L"Permitir que los usuarios estándar instalen programas y actualizaciones con Windows Update.", L"Permettre aux utilisateurs standard d'installer des programmes et des mises à jour avec Windows Update.", L"Standart kullanıcıların Windows Update ile program ve güncelleme yüklemesine izin verin.", L"Разрешить стандартным пользователям устанавливать программы и обновления с помощью Центра обновления Windows.", L"Permitir que usuários padrão instalem programas e atualizações com o Windows Update.", L"允许标准用户使用 Windows 更新安装程序和更新。", L"Zezwól standardowym użytkownikom na instalowanie programów i aktualizacji za pomocą Windows Update.", L"Sta standaardgebruikers programma's en updates installeren met Windows Update." },
    { 1100, L"Choose your Windows Update settings", L"Scegli le impostazioni di Windows Update", L"Elija la configuración de Windows Update", L"Choisissez vos paramètres Windows Update", L"Windows Update ayarlarınızı seçin", L"Выбор параметров Центра обновления Windows", L"Escolha suas configurações do Windows Update", L"选择 Windows 更新设置", L"Wybierz ustawienia Windows Update", L"Kies uw Windows Update-instellingen" },
    { 1102, L"When your PC is online, Windows can automatically check for important updates and install them using these settings. When new updates are available, you can also choose to install them when you shut down your PC.", L"Quando il PC è online, Windows può cercare automaticamente gli aggiornamenti importanti e installarli usando queste impostazioni. Quando sono disponibili nuovi aggiornamenti, puoi anche scegliere di installarli all'arresto del PC.", L"Cuando su PC esté en línea, Windows puede buscar automáticamente actualizaciones importantes e instalarlas con esta configuración. Cuando haya nuevas actualizaciones disponibles, también puede elegir instalarlas al apagar su PC.", L"Lorsque votre PC est en ligne, Windows peut rechercher automatiquement les mises à jour importantes et les installer à l'aide de ces paramètres. Lorsque de nouvelles mises à jour sont disponibles, vous pouvez également choisir de les installer à l'arrêt de votre PC.", L"Bilgisayarınız çevrimiçiyken Windows önemli güncellemeleri otomatik olarak denetleyebilir ve bu ayarları kullanarak yükleyebilir. Yeni güncellemeler kullanılabilir olduğunda, bilgisayarınızı kapattığınızda bunları yüklemeyi de seçebilirsiniz.", L"Когда компьютер подключен к Интернету, Windows может автоматически проверять наличие важных обновлений и устанавливать их с помощью этих параметров. Когда доступны новые обновления, вы также можете установить их при завершении работы компьютера.", L"Quando seu PC estiver online, o Windows pode verificar automaticamente atualizações importantes e instalá-las usando estas configurações. Quando novas atualizações estiverem disponíveis, você também pode escolher instalá-las ao desligar o PC.", L"当你的电脑在线时，Windows 可以自动检查重要更新并使用这些设置进行安装。当有新更新可用时，你也可以选择在关闭电脑时安装它们。", L"Gdy komputer jest online, system Windows może automatycznie sprawdzać ważne aktualizacje i instalować je przy użyciu tych ustawień. Gdy dostępne są nowe aktualizacje, możesz również wybrać ich instalację podczas zamykania komputera.", L"Wanneer uw pc online is, kan Windows automatisch controleren op belangrijke updates en deze installeren met deze instellingen. Wanneer nieuwe updates beschikbaar zijn, kunt u er ook voor kiezen ze te installeren wanneer u uw pc afsluit." },
    { 1105, L"Check the Status column to ensure all important updates were successful. To remove an update, see <a id=\\\"actionViewInstalledUpdates\\\">Installed Updates</a>.", L"Controllare la colonna Stato per assicurarsi che tutti gli aggiornamenti importanti siano riusciti. Per rimuovere un aggiornamento, vedere <a id=\\\"actionViewInstalledUpdates\\\">Aggiornamenti installati</a>.", L"Compruebe la columna Estado para asegurarse de que todas las actualizaciones importantes fueron correctas. Para quitar una actualización, vea <a id=\\\"actionViewInstalledUpdates\\\">Actualizaciones instaladas</a>.", L"Vérifiez la colonne État pour vous assurer que toutes les mises à jour importantes ont réussi. Pour supprimer une mise à jour, consultez <a id=\\\"actionViewInstalledUpdates\\\">Mises à jour installées</a>.", L"Tüm önemli güncellemelerin başarılı olduğundan emin olmak için Durum sütununu denetleyin. Bir güncellemeyi kaldırmak için <a id=\\\"actionViewInstalledUpdates\\\">Yüklü Güncellemeler</a> bölümüne bakın.", L"Проверьте столбец «Состояние», чтобы убедиться, что все важные обновления установлены успешно. Чтобы удалить обновление, см. <a id=\\\"actionViewInstalledUpdates\\\">Установленные обновления</a>.", L"Verifique a coluna Status para garantir que todas as atualizações importantes foram concluídas. Para remover uma atualização, veja <a id=\\\"actionViewInstalledUpdates\\\">Atualizações instaladas</a>.", L"检查“状态”列以确保所有重要更新均成功。若要删除某个更新，请参阅<a id=\\\"actionViewInstalledUpdates\\\">已安装的更新</a>。", L"Sprawdź kolumnę Stan, aby upewnić się, że wszystkie ważne aktualizacje zakończyły się pomyślnie. Aby usunąć aktualizację, zobacz <a id=\\\"actionViewInstalledUpdates\\\">Zainstalowane aktualizacje</a>.", L"Controleer de kolom Status om ervoor te zorgen dat alle belangrijke updates zijn geslaagd. Raadpleeg <a id=\\\"actionViewInstalledUpdates\\\">Geïnstalleerde updates</a> om een update te verwijderen." },
    { 1107, L"Recommended updates", L"Aggiornamenti consigliati", L"Actualizaciones recomendadas", L"Mises à jour recommandées", L"Önerilen güncellemeler", L"Рекомендуемые обновления", L"Atualizações recomendadas", L"推荐更新", L"Zalecane aktualizacje", L"Aanbevolen updates" },
    { 1108, L"Windows can't update important files and services while the system is using them. Save any open files, and then restart the PC.", L"Windows non può aggiornare file e servizi importanti mentre il sistema li sta utilizzando. Salva i file aperti e riavvia il PC.", L"Windows no puede actualizar archivos y servicios importantes mientras el sistema los está usando. Guarde los archivos abiertos y reinicie el PC.", L"Windows ne peut pas mettre à jour des fichiers et services importants pendant que le système les utilise. Enregistrez les fichiers ouverts, puis redémarrez le PC.", L"Sistem bunları kullanırken Windows önemli dosyaları ve hizmetleri güncelleyemez. Açık dosyaları kaydedin ve bilgisayarı yeniden başlatın.", L"Windows не может обновить важные файлы и службы, пока они используются системой. Сохраните открытые файлы и перезапустите компьютер.", L"O Windows não pode atualizar arquivos e serviços importantes enquanto o sistema os está usando. Salve os arquivos abertos e reinicie o PC.", L"系统正在使用重要文件和服务时，Windows 无法更新它们。请保存打开的文件，然后重启电脑。", L"System Windows nie może zaktualizować ważnych plików i usług, gdy są używane. Zapisz otwarte pliki i uruchom ponownie komputer.", L"Windows kan geen belangrijke bestanden en services bijwerken terwijl het systeem ze gebruikt. Sla geopende bestanden op en start de pc opnieuw op." },
    { 1110, L"Windows can't update important files and services while the system is using them. Make sure to save your files before restarting.", L"Windows non può aggiornare file e servizi importanti mentre il sistema li sta utilizzando. Salva i file prima di riavviare.", L"Windows no puede actualizar archivos y servicios importantes mientras el sistema los está usando. Asegúrese de guardar sus archivos antes de reiniciar.", L"Windows ne peut pas mettre à jour des fichiers et services importants pendant que le système les utilise. Veillez à enregistrer vos fichiers avant de redémarrer.", L"Sistem bunları kullanırken Windows önemli dosyaları ve hizmetleri güncelleyemez. Yeniden başlatmadan önce dosyalarınızı kaydettiğinizden emin olun.", L"Windows не может обновить важные файлы и службы, пока они используются системой. Перед перезапуском обязательно сохраните файлы.", L"O Windows não pode atualizar arquivos e serviços importantes enquanto o sistema os está usando. Salve seus arquivos antes de reiniciar.", L"系统正在使用重要文件和服务时，Windows 无法更新它们。请确保在重启前保存文件。", L"System Windows nie może zaktualizować ważnych plików i usług, gdy są używane. Przed ponownym uruchomieniem zapisz pliki.", L"Windows kan geen belangrijke bestanden en services bijwerken terwijl het systeem ze gebruikt. Sla uw bestanden op voordat u opnieuw opstart." },
    { 1112, L"Update type: ", L"Tipo di aggiornamento: ", L"Tipo de actualización: ", L"Type de mise à jour : ", L"Güncelleme türü: ", L"Тип обновления: ", L"Tipo de atualização: ", L"更新类型：", L"Typ aktualizacji: ", L"Updatetype: " },
    { 1117, L"Give me &recommended updates the same way I receive important updates", L"Dammi gli aggiornamenti &consigliati come ricevo gli aggiornamenti importanti", L"Darme actualizaciones &recomendadas de la misma manera que recibo las importantes", L"Donnez-moi les mises à jour &recommandées de la même manière que je reçois les importantes", L"Önemli güncellemeleri aldığım şekilde &önerilen güncellemeleri de ver", L"Предоставлять &рекомендуемые обновления так же, как важные", L"Dar-me atualizações &recomendadas da mesma forma que recebo as importantes", L"以接收重要更新的相同方式为我提供推荐更新（&r）", L"Daj mi &zalecane aktualizacje w taki sam sposób, jak otrzymuję ważne", L"Geef mij &aanbevolen updates op dezelfde manier als ik belangrijke ontvang" },
    { 1118, L"Restore hidden updates", L"Ripristina aggiornamenti nascosti", L"Restaurar actualizaciones ocultas", L"Restaurer les mises à jour masquées", L"Gizli güncellemeleri geri yükle", L"Восстановить скрытые обновления", L"Restaurar atualizações ocultas", L"还原隐藏的更新", L"Przywróć ukryte aktualizacje", L"Verborgen updates herstellen" },
    { 1119, L"Cancel", L"Annulla", L"Cancelar", L"Annuler", L"İptal", L"Отмена", L"Cancelar", L"取消", L"Anuluj", L"Annuleren" },
    { 1121, L"Installation date: ", L"Data di installazione: ", L"Fecha de instalación: ", L"Date d'installation : ", L"Yükleme tarihi: ", L"Дата установки: ", L"Data de instalação: ", L"安装日期：", L"Data instalacji: ", L"Installatiedatum: " },
    { 1124, L"Installation status: ", L"Stato dell'installazione: ", L"Estado de instalación: ", L"État de l'installation : ", L"Yükleme durumu: ", L"Состояние установки: ", L"Status de instalação: ", L"安装状态：", L"Stan instalacji: ", L"Installatiestatus: " },
    { 1125, L"Error details: ", L"Dettagli errore: ", L"Detalles del error: ", L"Détails de l'erreur : ", L"Hata ayrıntıları: ", L"Сведения об ошибке: ", L"Detalhes do erro: ", L"错误详细信息：", L"Szczegóły błędu: ", L"Foutdetails: " },
    { 1128, L"See also", L"Vedi anche", L"Ver también", L"Voir aussi", L"Ayrıca bakınız", L"См. также", L"Consulte também", L"另请参阅", L"Zobacz też", L"Zie ook" },
    { 1131, L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Центр обновления Windows", L"Windows Update", L"Windows 更新", L"Windows Update", L"Windows Update" },
    { 1132, L"&Install updates", L"&Installa aggiornamenti", L"&Instalar actualizaciones", L"&Installer les mises à jour", L"Güncellemeleri &yükle", L"&Установить обновления", L"&Instalar atualizações", L"&安装更新", L"&Zainstaluj aktualizacje", L"Updates &installeren" },
    { 1135, L"Select the updates you want to install", L"Seleziona gli aggiornamenti da installare", L"Seleccione las actualizaciones que desea instalar", L"Sélectionnez les mises à jour à installer", L"Yüklemek istediğiniz güncellemeleri seçin", L"Выберите обновления для установки", L"Selecione as atualizações que deseja instalar", L"选择要安装的更新", L"Wybierz aktualizacje do zainstalowania", L"Selecteer de updates die u wilt installeren" },
    { 1136, L"Try &again", L"Riprova", L"&Reintentar", L"&Réessayer", L"&Yeniden dene", L"&Повторить", L"&Tentar novamente", L"&重试", L"&Spróbuj ponownie", L"&Opnieuw proberen" },
    { 1140, L"Checking for updates...", L"Ricerca aggiornamenti in corso...", L"Buscando actualizaciones...", L"Recherche des mises à jour...", L"Güncellemeler denetleniyor...", L"Проверка наличия обновлений...", L"Verificando atualizações...", L"正在检查更新...", L"Sprawdzanie aktualizacji...", L"Controleren op updates..." },
    { 1141, L"&Stop installation", L"&Interrompi installazione", L"&Detener instalación", L"&Arrêter l'installation", L"Yüklemeyi &durdur", L"&Остановить установку", L"&Parar instalação", L"&停止安装", L"&Zatrzymaj instalację", L"Installatie &stoppen" },
    { 1144, L"Most recent check for updates:", L"Ultima ricerca aggiornamenti:", L"Última búsqueda de actualizaciones:", L"Dernière recherche des mises à jour :", L"En son güncelleme denetimi:", L"Последняя проверка обновлений:", L"Verificação mais recente de atualizações:", L"最近检查更新：", L"Ostatnie sprawdzanie aktualizacji:", L"Meest recente controle op updates:" },
    { 1145, L"Updates were installed:", L"Aggiornamenti installati:", L"Actualizaciones instaladas:", L"Mises à jour installées :", L"Yüklenen güncellemeler:", L"Установленные обновления:", L"Atualizações instaladas:", L"已安装更新：", L"Zainstalowane aktualizacje:", L"Geïnstalleerde updates:" },
    { 1146, L"%lastSuccessfulInstallTime%", L"%lastSuccessfulInstallTime%", L"%lastSuccessfulInstallTime%", L"%lastSuccessfulInstallTime%", L"%lastSuccessfulInstallTime%", L"%lastSuccessfulInstallTime%", L"%lastSuccessfulInstallTime%", L"%lastSuccessfulInstallTime%", L"%lastSuccessfulInstallTime%", L"%lastSuccessfulInstallTime%" },
    { 1149, L"Turn on automatic updating", L"Attiva l'aggiornamento automatico", L"Activar la actualización automática", L"Activer la mise à jour automatique", L"Otomatik güncellemeyi aç", L"Включить автоматическое обновление", L"Ativar a atualização automática", L"启用自动更新", L"Włącz automatyczną aktualizację", L"Automatische update inschakelen" },
    { 1150, L"Turn on &automatic updates", L"Attiva gli aggiornamenti &automatici", L"Activar las actualizaciones &automáticas", L"Activer les mises à jour &automatiques", L"&Otomatik güncellemeleri aç", L"Включить &автоматические обновления", L"Ativar atualizações &automáticas", L"启用自动更新（&a）", L"Włącz &automatyczne aktualizacje", L"Automatische updates inschakelen (&a)" },
    { 1152, L"Always install the latest updates to enhance your PC's security and performance.", L"Installa sempre gli aggiornamenti più recenti per migliorare sicurezza e prestazioni del tuo PC.", L"Instale siempre las actualizaciones más recientes para mejorar la seguridad y el rendimiento de su PC.", L"Installez toujours les dernières mises à jour pour améliorer la sécurité et les performances de votre PC.", L"Bilgisayarınızın güvenliğini ve performansını artırmak için en son güncellemeleri her zaman yükleyin.", L"Всегда устанавливайте последние обновления для повышения безопасности и производительности компьютера.", L"Instale sempre as atualizações mais recentes para melhorar a segurança e o desempenho do seu PC.", L"始终安装最新更新以增强电脑的安全性和性能。", L"Zawsze instaluj najnowsze aktualizacje, aby zwiększyć bezpieczeństwo i wydajność komputera.", L"Installeer altijd de nieuwste updates om de beveiliging en prestaties van uw pc te verbeteren." },
    { 1153, L"Updates are not being installed automatically", L"Gli aggiornamenti non vengono installati automaticamente", L"Las actualizaciones no se están instalando automáticamente", L"Les mises à jour ne sont pas installées automatiquement", L"Güncellemeler otomatik olarak yüklenmiyor", L"Обновления не устанавливаются автоматически", L"As atualizações não estão sendo instaladas automaticamente", L"更新未自动安装", L"Aktualizacje nie są instalowane automatycznie", L"Updates worden niet automatisch geïnstalleerd" },
    { 1154, L"Turn on automatic updating to help improve the security and performance of your PC and allow standard users to install updates on this PC.", L"Attiva l'aggiornamento automatico per migliorare sicurezza e prestazioni del tuo PC e consentire agli utenti standard di installare aggiornamenti su questo PC.", L"Active la actualización automática para mejorar la seguridad y el rendimiento de su PC y permitir que los usuarios estándar instalen actualizaciones en este PC.", L"Activez la mise à jour automatique pour améliorer la sécurité et les performances de votre PC et permettre aux utilisateurs standard d'installer des mises à jour sur ce PC.", L"Bilgisayarınızın güvenliğini ve performansını artırmak ve standart kullanıcıların bu bilgisayara güncelleme yüklemesine izin vermek için otomatik güncellemeyi açın.", L"Включите автоматическое обновление, чтобы повысить безопасность и производительность компьютера и разрешить стандартным пользователям устанавливать обновления.", L"Ative a atualização automática para melhorar a segurança e o desempenho do seu PC e permitir que usuários padrão instalem atualizações neste PC.", L"启用自动更新以帮助提高电脑的安全性和性能，并允许标准用户在此电脑上安装更新。", L"Włącz automatyczną aktualizację, aby poprawić bezpieczeństwo i wydajność komputera i zezwolić standardowym użytkownikom na instalowanie aktualizacji.", L"Schakel automatische updates in om de beveiliging en prestaties van uw pc te verbeteren en standaardgebruikers updates op deze pc te laten installeren." },
    { 1156, L"&Restore", L"&Ripristina", L"&Restaurar", L"&Restaurer", L"&Geri yükle", L"&Восстановить", L"&Restaurar", L"&还原", L"&Przywróć", L"&Herstellen" },
    { 1157, L"Review your update history", L"Rivedi la cronologia degli aggiornamenti", L"Revise su historial de actualizaciones", L"Passez en revue votre historique des mises à jour", L"Güncelleme geçmişinizi gözden geçirin", L"Просмотрите журнал обновлений", L"Revise seu histórico de atualizações", L"查看你的更新历史记录", L"Przejrzyj historię aktualizacji", L"Bekijk uw updategeschiedenis" },
    { 1158, L"OK", L"OK", L"Aceptar", L"OK", L"Tamam", L"ОК", L"OK", L"确定", L"OK", L"OK" },
    { 1162, L"Let me choose", L"Lascia decidere a me", L"Permítame elegir", L"Laissez-moi choisir", L"Ben seçeyim", L"Я выбираю сам", L"Deixe-me escolher", L"让我选择", L"Pozwól mi wybrać", L"Laat mij kiezen" },
    { 1163, L"You decide which updates are installed automatically, when they happen, and who can install updates.", L"Sei tu a decidere quali aggiornamenti vengono installati automaticamente, quando e chi può installarli.", L"Usted decide qué actualizaciones se instalan automáticamente, cuándo y quién puede instalarlas.", L"Vous décidez quelles mises à jour sont installées automatiquement, quand et qui peut les installer.", L"Hangi güncellemelerin otomatik olarak yükleneceğine, ne zaman yükleneceğine ve kimlerin yükleyebileceğine siz karar verirsiniz.", L"Вы решаете, какие обновления устанавливать автоматически, когда и кто может их устанавливать.", L"Você decide quais atualizações são instaladas automaticamente, quando e quem pode instalá-las.", L"你决定自动安装哪些更新、何时安装以及谁可以安装更新。", L"Ty decydujesz, które aktualizacje są instalowane automatycznie, kiedy i kto może je instalować.", L"U bepaalt welke updates automatisch worden geïnstalleerd, wanneer en wie ze kan installeren." },
    { 1170, L"Error(s) found:", L"Errore/i trovato/i:", L"Error(es) encontrado(s):", L"Erreur(s) détectée(s) :", L"Bulunan hata(lar):", L"Найдены ошибки:", L"Erro(s) encontrado(s):", L"找到的错误：", L"Znaleziono błędy:", L"Fout(en) gevonden:" },
    { 1173, L"Get help with this error", L"Ottieni assistenza per questo errore", L"Obtener ayuda con este error", L"Obtenir de l'aide sur cette erreur", L"Bu hata için yardım alın", L"Получить справку об этой ошибке", L"Obter ajuda com este erro", L"获取此错误的帮助", L"Uzyskaj pomoc dotyczącą tego błędu", L"Help bij deze fout krijgen" },
    { 1174, L"Some settings are managed by your system administrator. ", L"Alcune impostazioni sono gestite dall'amministratore di sistema. ", L"Algunos parámetros los administra el administrador del sistema. ", L"Certains paramètres sont gérés par votre administrateur système. ", L"Bazı ayarlar sistem yöneticiniz tarafından yönetiliyor. ", L"Некоторые параметры управляются системным администратором. ", L"Algumas configurações são gerenciadas pelo administrador do sistema. ", L"某些设置由你的系统管理员管理。 ", L"Niektóre ustawienia są zarządzane przez administratora systemu. ", L"Sommige instellingen worden door uw systeembeheerder beheerd. " },
    { 1175, L"More information.", L"Maggiori informazioni.", L"Más información.", L"Plus d'informations.", L"Daha fazla bilgi.", L"Дополнительные сведения.", L"Mais informações.", L"更多信息。", L"Więcej informacji.", L"Meer informatie." },
    { 1176, L"After you restore updates, you can install them. We recommend restoring all important updates.", L"Dopo aver ripristinato gli aggiornamenti, puoi installarli. Si consiglia di ripristinare tutti gli aggiornamenti importanti.", L"Después de restaurar las actualizaciones, puede instalarlas. Se recomienda restaurar todas las actualizaciones importantes.", L"Après avoir restauré les mises à jour, vous pouvez les installer. Nous recommandons de restaurer toutes les mises à jour importantes.", L"Güncellemeleri geri yükledikten sonra bunları yükleyebilirsiniz. Tüm önemli güncellemelerin geri yüklenmesini öneririz.", L"После восстановления обновлений вы можете установить их. Рекомендуем восстановить все важные обновления.", L"Após restaurar as atualizações, você pode instalá-las. Recomendamos restaurar todas as atualizações importantes.", L"还原更新后，你可以安装它们。我们建议还原所有重要更新。", L"Po przywróceniu aktualizacji możesz je zainstalować. Zalecamy przywrócenie wszystkich ważnych aktualizacji.", L"Nadat u updates hebt hersteld, kunt u ze installeren. We raden aan alle belangrijke updates te herstellen." },
    { 1177, L"Updates help improve the security and performance of your computer.  It's important to install them as soon as they become available.", L"Gli aggiornamenti aiutano a migliorare sicurezza e prestazioni del computer. È importante installarli appena disponibili.", L"Las actualizaciones ayudan a mejorar la seguridad y el rendimiento del equipo. Es importante instalarlas tan pronto como estén disponibles.", L"Les mises à jour améliorent la sécurité et les performances de votre ordinateur. Il est important de les installer dès qu'elles sont disponibles.", L"Güncellemeler bilgisayarınızın güvenliğini ve performansını artırmaya yardımcı olur. Kullanılabilir olduklarında bunları yüklemek önemlidir.", L"Обновления помогают повысить безопасность и производительность компьютера. Важно устанавливать их сразу после появления.", L"As atualizações ajudam a melhorar a segurança e o desempenho do computador. É importante instalá-las assim que estiverem disponíveis.", L"更新有助于提高电脑的安全性和性能。一旦可用，立即安装非常重要。", L"Aktualizacje pomagają poprawić bezpieczeństwo i wydajność komputera. Ważne jest, aby instalować je, gdy tylko będą dostępne.", L"Updates verbeteren de beveiliging en prestaties van uw computer. Het is belangrijk ze te installeren zodra ze beschikbaar zijn." },
    { 1178, L"Install updates automatically (recommended)", L"Installa aggiornamenti automaticamente (consigliato)", L"Instalar actualizaciones automáticamente (recomendado)", L"Installer automatiquement les mises à jour (recommandé)", L"Güncellemeleri otomatik olarak yükle (önerilir)", L"Автоматически устанавливать обновления (рекомендуется)", L"Instalar atualizações automaticamente (recomendado)", L"自动安装更新（推荐）", L"Automatycznie instaluj aktualizacje (zalecane)", L"Updates automatisch installeren (aanbevolen)" },
    { 1181, L"Install new Windows Update software", L"Installa nuovo software di Windows Update", L"Instalar software nuevo de Windows Update", L"Installer un nouveau logiciel Windows Update", L"Yeni Windows Update yazılımını yükle", L"Установить новое программное обеспечение Центра обновления Windows", L"Instalar novo software do Windows Update", L"安装新的 Windows 更新软件", L"Zainstaluj nowe oprogramowanie Windows Update", L"Nieuw Windows Update-software installeren" },
    { 1182, L"&Install now", L"&Installa ora", L"&Instalar ahora", L"&Installer maintenant", L"Şimdi &yükle", L"&Установить сейчас", L"&Instalar agora", L"&立即安装", L"&Zainstaluj teraz", L"Nu &installeren" },
    { 1183, L"Sometimes, Windows Update itself needs to be updated. To continue, you'll need to do this now. Your automatic update settings won't change at all.", L"A volte è necessario aggiornare Windows Update stesso. Per continuare, dovrai farlo ora. Le impostazioni di aggiornamento automatico non cambieranno.", L"A veces es necesario actualizar el propio Windows Update. Para continuar, tendrá que hacerlo ahora. La configuración de actualización automática no cambiará.", L"Parfois, Windows Update lui-même doit être mis à jour. Pour continuer, vous devrez le faire maintenant. Vos paramètres de mise à jour automatique ne changeront pas.", L"Bazen Windows Update'in kendisinin güncellenmesi gerekir. Devam etmek için bunu şimdi yapmanız gerekir. Otomatik güncelleme ayarlarınız hiç değişmeyecek.", L"Иногда сам Центр обновления Windows нуждается в обновлении. Чтобы продолжить, сделайте это сейчас. Ваши параметры автоматического обновления не изменятся.", L"Às vezes, o próprio Windows Update precisa ser atualizado. Para continuar, você precisará fazer isso agora. Suas configurações de atualização automática não mudarão.", L"有时 Windows 更新本身需要更新。若要继续，你现在需要执行此操作。自动更新设置将完全不变。", L"Czasami sam Windows Update wymaga aktualizacji. Aby kontynuować, musisz to zrobić teraz. Ustawienia automatycznej aktualizacji w żaden sposób się nie zmienią.", L"Soms moet Windows Update zelf worden bijgewerkt. Om verder te gaan, moet u dit nu doen. Uw instellingen voor automatische updates veranderen niet." },
    { 1184, L"To finish installing this update, Windows Update will automatically close and reopen.", L"Per completare l'installazione di questo aggiornamento, Windows Update si chiuderà e riaprirà automaticamente.", L"Para terminar de instalar esta actualización, Windows Update se cerrará y volverá a abrir automáticamente.", L"Pour terminer l'installation de cette mise à jour, Windows Update se fermera et se rouvrira automatiquement.", L"Bu güncellemenin yüklenmesini bitirmek için Windows Update otomatik olarak kapanıp yeniden açılacaktır.", L"Чтобы завершить установку этого обновления, Центр обновления Windows автоматически закроется и снова откроется.", L"Para concluir a instalação desta atualização, o Windows Update fechará e reabrirá automaticamente.", L"若要完成此更新的安装，Windows 更新将自动关闭并重新打开。", L"Aby zakończyć instalowanie tej aktualizacji, Windows Update zamknie się i otworzy ponownie automatycznie.", L"Om de installatie van deze update te voltooien, wordt Windows Update automatisch gesloten en opnieuw geopend." },
    { 1185, L"Check for updates for your PC", L"Controlla gli aggiornamenti per il tuo PC", L"Buscar actualizaciones para su PC", L"Rechercher des mises à jour pour votre PC", L"Bilgisayarınız için güncellemeleri denetleyin", L"Проверить наличие обновлений для компьютера", L"Verificar atualizações para seu PC", L"检查适用于你电脑的更新", L"Sprawdź aktualizacje dla swojego komputera", L"Controleren op updates voor uw pc" },
    { 1186, L"&Check for updates", L"&Controlla aggiornamenti", L"&Buscar actualizaciones", L"&Rechercher des mises à jour", L"Güncellemeleri &denetle", L"&Проверить наличие обновлений", L"&Verificar atualizações", L"&检查更新", L"&Sprawdź aktualizacje", L"&Controleren op updates" },
    { 1188, L"&Restart now", L"&Riavvia ora", L"&Reiniciar ahora", L"&Redémarrer maintenant", L"Şimdi yeniden &başlat", L"&Перезапустить сейчас", L"&Reiniciar agora", L"&立即重启", L"&Uruchom ponownie teraz", L"Nu &opnieuw starten" },
    { 1195, L"More information: ", L"Maggiori informazioni: ", L"Más información: ", L"Plus d'informations : ", L"Daha fazla bilgi: ", L"Дополнительные сведения: ", L"Mais informações: ", L"更多信息：", L"Więcej informacji: ", L"Meer informatie: " },
    { 1196, L"Help and Support: ", L"Guida e supporto: ", L"Ayuda y soporte: ", L"Aide et support : ", L"Yardım ve Destek: ", L"Справка и поддержка: ", L"Ajuda e Suporte: ", L"帮助和支持：", L"Pomoc i obsługa techniczna: ", L"Help en ondersteuning: " },
    { 1197, L"Download size: ", L"Dimensioni download: ", L"Tamaño de descarga: ", L"Taille du téléchargement : ", L"İndirme boyutu: ", L"Размер загрузки: ", L"Tamanho do download: ", L"下载大小：", L"Rozmiar pobierania: ", L"Downloadgrootte: " },
    { 1198, L"You may need to restart your computer for this update to take effect.", L"Potrebbe essere necessario riavviare il computer affinché questo aggiornamento abbia effetto.", L"Es posible que deba reiniciar el equipo para que esta actualización surta efecto.", L"Vous devrez peut-être redémarrer votre ordinateur pour que cette mise à jour prenne effet.", L"Bu güncellemenin geçerli olması için bilgisayarınızı yeniden başlatmanız gerekebilir.", L"Возможно, потребуется перезапустить компьютер, чтобы это обновление вступило в силу.", L"Talvez seja necessário reiniciar o computador para que esta atualização entre em vigor.", L"可能需要重启电脑才能应用此更新。", L"Może być konieczne ponowne uruchomienie komputera, aby ta aktualizacja została zastosowana.", L"Mogelijk moet u uw computer opnieuw starten voordat deze update van kracht wordt." },
    { 1199, L"To continue using Windows Update, you need to install this update. After installing it, you might still need to install other important updates for your computer.", L"Per continuare a usare Windows Update, devi installare questo aggiornamento. Dopo l'installazione, potresti dover installare altri aggiornamenti importanti per il computer.", L"Para seguir usando Windows Update, debe instalar esta actualización. Después de instalarla, es posible que deba instalar otras actualizaciones importantes para su equipo.", L"Pour continuer à utiliser Windows Update, vous devez installer cette mise à jour. Après l'installation, vous devrez peut-être encore installer d'autres mises à jour importantes pour votre ordinateur.", L"Windows Update'i kullanmaya devam etmek için bu güncellemeyi yüklemeniz gerekir. Yükledikten sonra bilgisayarınız için başka önemli güncellemeler yüklemeniz gerekebilir.", L"Чтобы продолжить использовать Центр обновления Windows, установите это обновление. После установки могут потребоваться другие важные обновления для компьютера.", L"Para continuar usando o Windows Update, você precisa instalar esta atualização. Após instalá-la, talvez você ainda precise instalar outras atualizações importantes para seu computador.", L"若要继续使用 Windows 更新，你需要安装此更新。安装后，你可能仍需要为你的电脑安装其他重要更新。", L"Aby nadal korzystać z Windows Update, musisz zainstalować tę aktualizację. Po jej zainstalowaniu może być konieczne zainstalowanie innych ważnych aktualizacji dla komputera.", L"Om Windows Update te blijven gebruiken, moet u deze update installeren. Na de installatie moet u mogelijk nog andere belangrijke updates voor uw computer installeren." },
    { 1201, L"Print", L"Stampa", L"Imprimir", L"Imprimer", L"Yazdır", L"Печать", L"Imprimir", L"打印", L"Drukuj", L"Afdrukken" },
    { 1202, L"I &accept the license terms", L"I &accetto i termini di licenza", L"Acepto los términos de licencia", L"J'&accepte les termes du contrat de licence", L"Lisans koşullarını &kabul ediyorum", L"Я принимаю условия лицензии", L"&Aceito os termos da licença", L"我接受许可条款（&a）", L"&Akceptuję postanowienia licencyjne", L"Ik ga akkoord met de licentievoorwaarden (&a)" },
    { 1203, L"I &decline", L"&Non accetto", L"&Rechazo", L"Je &refuse", L"&Reddediyorum", L"&Не принимаю", L"&Recuso", L"我拒绝（&d）", L"&Odrzucam", L"Ik &weiger" },
    { 1204, L"Help", L"Guida", L"Ayuda", L"Aide", L"Yardım", L"Справка", L"Ajuda", L"帮助", L"Pomoc", L"Help" },
    { 1205, L"Install important and recommended updates as they become available. Allow standard users to install updates on this computer.", L"Installa gli aggiornamenti importanti e consigliati appena disponibili. Consenti agli utenti standard di installare aggiornamenti su questo computer.", L"Instale las actualizaciones importantes y recomendadas tan pronto como estén disponibles. Permita que los usuarios estándar instalen actualizaciones en este equipo.", L"Installez les mises à jour importantes et recommandées dès qu'elles sont disponibles. Autorisez les utilisateurs standard à installer des mises à jour sur cet ordinateur.", L"Önemli ve önerilen güncellemeleri kullanılabilir olduklarında yükleyin. Standart kullanıcıların bu bilgisayara güncelleme yüklemesine izin verin.", L"Устанавливайте важные и рекомендуемые обновления по мере их появления. Разрешите стандартным пользователям устанавливать обновления на этом компьютере.", L"Instale atualizações importantes e recomendadas assim que estiverem disponíveis. Permita que usuários padrão instalem atualizações neste computador.", L"及时安装重要和推荐更新。允许标准用户在此电脑上安装更新。", L"Instaluj ważne i zalecane aktualizacje, gdy tylko będą dostępne. Zezwól standardowym użytkownikom na instalowanie aktualizacji na tym komputerze.", L"Installeer belangrijke en aanbevolen updates zodra ze beschikbaar zijn. Sta standaardgebruikers toe updates op deze computer te installeren." },
    { 1209, L"Note: Windows Update might update itself automatically first when checking for other updates.  You can visit the Microsoft website to read the privacy statement online.", L"Nota: Windows Update potrebbe aggiornarsi automaticamente prima di controllare altri aggiornamenti.  È possibile visitare il sito Web di Microsoft per leggere l'informativa sulla privacy online.", L"Nota: Windows Update podría actualizarse automáticamente antes de buscar otras actualizaciones.  Puede visitar el sitio web de Microsoft para leer la declaración de privacidad en línea.", L"Remarque : Windows Update peut d'abord se mettre à jour automatiquement lors de la recherche d'autres mises à jour.  Vous pouvez visiter le site Web de Microsoft pour lire la déclaration de confidentialité en ligne.", L"Not: Diğer güncellemeleri denetlerken Windows Update önce kendini otomatik olarak güncelleyebilir.  Gizlilik bildirimini çevrimiçi okumak için Microsoft web sitesini ziyaret edebilirsiniz.", L"Примечание. При проверке других обновлений Центр обновления Windows может сначала обновиться автоматически.  Вы можете посетить веб-сайт Майкрософт, чтобы прочитать политику конфиденциальности в Интернете.", L"Observação: o Windows Update pode se atualizar automaticamente antes de verificar outras atualizações.  Você pode visitar o site da Microsoft para ler a declaração de privacidade online.", L"注意：检查其他更新时，Windows 更新可能会先自动更新自身。您可以访问 Microsoft 网站阅读在线隐私声明。", L"Uwaga: podczas sprawdzania innych aktualizacji Windows Update może najpierw zaktualizować się automatycznie.  Możesz odwiedzić witrynę firmy Microsoft, aby przeczytać politykę prywatności online.", L"Opmerking: Windows Update kan zichzelf eerst automatisch bijwerken bij het controleren op andere updates.  U kunt de Microsoft-website bezoeken om de privacyverklaring online te lezen." },
    { 1210, L"There aren't any hidden updates.", L"Non ci sono aggiornamenti nascosti.", L"No hay actualizaciones ocultas.", L"Il n'y a aucune mise à jour masquée.", L"Gizli güncelleme yok.", L"Нет скрытых обновлений.", L"Não há atualizações ocultas.", L"没有任何隐藏的更新。", L"Nie ma ukrytych aktualizacji.", L"Er zijn geen verborgen updates." },
    { 1211, L"You have not tried to install any updates for your computer.", L"Non hai provato a installare alcun aggiornamento per il computer.", L"No ha intentado instalar ninguna actualización para su equipo.", L"Vous n'avez pas essayé d'installer de mises à jour pour votre ordinateur.", L"Bilgisayarınız için herhangi bir güncelleme yüklemeyi denemediniz.", L"Вы не пытались установить обновления для компьютера.", L"Você não tentou instalar atualizações para seu computador.", L"你尚未尝试为你的电脑安装任何更新。", L"Nie próbowałeś zainstalować żadnych aktualizacji dla swojego komputera.", L"U hebt niet geprobeerd updates voor uw computer te installeren." },
    { 1213, L"Troubleshoot problems with installing updates", L"Risolvi i problemi di installazione degli aggiornamenti", L"Solucionar problemas con la instalación de actualizaciones", L"Résoudre les problèmes d'installation des mises à jour", L"Güncellemelerin yüklenmesiyle ilgili sorunları giderin", L"Устранение проблем с установкой обновлений", L"Solucionar problemas com a instalação de atualizações", L"解决更新安装问题", L"Rozwiązywanie problemów z instalacją aktualizacji", L"Problemen met het installeren van updates oplossen" },
    { 1218, L"Learn about installing Windows updates", L"Scopri come installare gli aggiornamenti di Windows", L"Obtener información sobre la instalación de actualizaciones de Windows", L"En savoir plus sur l'installation des mises à jour Windows", L"Windows güncellemelerini yükleme hakkında bilgi edinin", L"Подробнее об установке обновлений Windows", L"Saiba mais sobre como instalar atualizações do Windows", L"了解如何安装 Windows 更新", L"Dowiedz się, jak instalować aktualizacje systemu Windows", L"Meer informatie over het installeren van Windows-updates" },
    { 1224, L"Help", L"Guida", L"Ayuda", L"Aide", L"Yardım", L"Справка", L"Ajuda", L"帮助", L"Pomoc", L"Help" },
    { 1225, L"You receive updates: ", L"Ricevi aggiornamenti: ", L"Recibe actualizaciones: ", L"Vous recevez des mises à jour : ", L"Güncellemeleri şuradan alırsınız: ", L"Вы получаете обновления: ", L"Você recebe atualizações: ", L"你收到的更新：", L"Otrzymujesz aktualizacje: ", L"U ontvangt updates: " },
    { 1226, L"You checked online for updates from %1.", L"Hai controllato online la presenza di aggiornamenti da %1.", L"Comprobó en línea si había actualizaciones de %1.", L"Vous avez recherché en ligne les mises à jour de %1.", L"%1 güncellemeleri için çevrimiçi denetim yaptınız.", L"Вы проверили наличие обновлений от %1 в Интернете.", L"Você verificou online atualizações de %1.", L"你已在线检查来自 %1 的更新。", L"Sprawdziłeś online aktualizacje od %1.", L"U hebt online gecontroleerd op updates van %1." },
    { 1227, L"Check online for updates from %1", L"Controlla online gli aggiornamenti da %1", L"Comprobar en línea las actualizaciones de %1", L"Rechercher en ligne les mises à jour de %1", L"%1 güncellemeleri için çevrimiçi denetleyin", L"Проверить в Интернете наличие обновлений от %1", L"Verificar online as atualizações de %1", L"在线检查来自 %1 的更新", L"Sprawdź online aktualizacje od %1", L"Online controleren op updates van %1" },
    { 1232, L"&Important updates", L"Aggiornamenti &importanti", L"Actualizaciones &importantes", L"Mises à jour &importantes", L"&Önemli güncellemeler", L"&Важные обновления", L"Atualizações &importantes", L"重要更新（&I）", L"&Ważne aktualizacje", L"Belangrijke updates (&I)" },
    { 1233, L"&I", L"&I", L"&Yo", L"&J", L"&B", L"&Я", L"&E", L"&我", L"&Ja", L"&I" },
    { 1234, L"Cancel", L"Annulla", L"Cancelar", L"Annuler", L"İptal", L"Отмена", L"Cancelar", L"取消", L"Anuluj", L"Annuleren" },
    { 1235, L"Install", L"Installa", L"Instalar", L"Installer", L"Yükle", L"Установить", L"Instalar", L"安装", L"Zainstaluj", L"Installeren" },
    { 1236, L"Updates will be automatically downloaded in the background when your PC is not on a metered Internet connection.", L"Gli aggiornamenti verranno scaricati automaticamente in background quando il PC non è connesso a una connessione Internet a consumo.", L"Las actualizaciones se descargarán automáticamente en segundo plano cuando su PC no tenga una conexión a Internet con datos limitados.", L"Les mises à jour seront automatiquement téléchargées en arrière-plan lorsque votre PC n'est pas sur une connexion Internet limitée.", L"Bilgisayarınız ölçümlemeli bir İnternet bağlantısında değilken güncellemeler arka planda otomatik olarak indirilir.", L"Обновления будут автоматически загружаться в фоновом режиме, когда компьютер не подключен к лимитируемому подключению к Интернету.", L"As atualizações serão baixadas automaticamente em segundo plano quando seu PC não estiver em uma conexão à Internet medida.", L"当你的电脑未使用按流量计费的 Internet 连接时，更新将在后台自动下载。", L"Aktualizacje będą automatycznie pobierane w tle, gdy komputer nie korzysta z taryfowanego połączenia internetowego.", L"Updates worden automatisch op de achtergrond gedownload wanneer uw pc geen datalimiet-verbinding heeft." },
    { 1246, L"More information", L"Maggiori informazioni", L"Más información", L"Plus d'informations", L"Daha fazla bilgi", L"Дополнительные сведения", L"Mais informações", L"更多信息", L"Więcej informacji", L"Meer informatie" },
    { 1247, L"More information (2)", L"Maggiori informazioni (2)", L"Más información (2)", L"Plus d'informations (2)", L"Daha fazla bilgi (2)", L"Дополнительные сведения (2)", L"Mais informações (2)", L"更多信息 (2)", L"Więcej informacji (2)", L"Meer informatie (2)" },
    { 1248, L"More information (3)", L"Maggiori informazioni (3)", L"Más información (3)", L"Plus d'informations (3)", L"Daha fazla bilgi (3)", L"Дополнительные сведения (3)", L"Mais informações (3)", L"更多信息 (3)", L"Więcej informacji (3)", L"Meer informatie (3)" },
    { 1249, L"Support information", L"Informazioni sul supporto", L"Información de soporte", L"Informations de support", L"Destek bilgileri", L"Сведения о поддержке", L"Informações de suporte", L"支持信息", L"Informacje o pomocy", L"Ondersteuningsinformatie" },
    { 1250, L"Horizontal", L"Orizzontale", L"Horizontal", L"Horizontal", L"Yatay", L"Горизонтальный", L"Horizontal", L"水平", L"Poziomy", L"Horizontaal" },
    { 1251, L"Used to change horizontal viewing area", L"Usato per modificare l'area di visualizzazione orizzontale", L"Se usa para cambiar el área de visualización horizontal", L"Utilisé pour modifier la zone d'affichage horizontale", L"Yatay görüntüleme alanını değiştirmek için kullanılır", L"Используется для изменения горизонтальной области просмотра", L"Usado para alterar a área de exibição horizontal", L"用于更改水平查看区域", L"Używane do zmiany poziomego obszaru wyświetlania", L"Wordt gebruikt om het horizontale weergavegebied te wijzigen" },
    { 1252, L"Vertical", L"Verticale", L"Vertical", L"Vertical", L"Dikey", L"Вертикальный", L"Vertical", L"垂直", L"Pionowy", L"Verticaal" },
    { 1253, L"Used to change vertical viewing area", L"Usato per modificare l'area di visualizzazione verticale", L"Se usa para cambiar el área de visualización vertical", L"Utilisé pour modifier la zone d'affichage verticale", L"Dikey görüntüleme alanını değiştirmek için kullanılır", L"Используется для изменения вертикальной области просмотра", L"Usado para alterar a área de exibição vertical", L"用于更改垂直查看区域", L"Używane do zmiany pionowego obszaru wyświetlania", L"Wordt gebruikt om het verticale weergavegebied te wijzigen" },
    { 1254, L"Let me choose my settings", L"Lasciami scegliere le impostazioni", L"Permítame elegir mi configuración", L"Laissez-moi choisir mes paramètres", L"Ayarlarımı ben seçeyim", L"Я сам выберу параметры", L"Deixe-me escolher minhas configurações", L"让我选择我的设置", L"Pozwól mi wybrać ustawienia", L"Laat mij mijn instellingen kiezen" },
    { 1255, L"No updates are selected.", L"Nessun aggiornamento selezionato.", L"No hay actualizaciones seleccionadas.", L"Aucune mise à jour sélectionnée.", L"Hiçbir güncelleme seçilmedi.", L"Не выбрано ни одного обновления.", L"Nenhuma atualização selecionada.", L"未选择任何更新。", L"Nie wybrano żadnych aktualizacji.", L"Er zijn geen updates geselecteerd." },
    { 1256, L"There are no updates available for your PC.", L"Non sono disponibili aggiornamenti per il tuo PC.", L"No hay actualizaciones disponibles para su PC.", L"Aucune mise à jour n'est disponible pour votre PC.", L"Bilgisayarınız için güncelleme yok.", L"Нет доступных обновлений для вашего компьютера.", L"Não há atualizações disponíveis para seu PC.", L"没有适用于你电脑的更新。", L"Brak aktualizacji dla Twojego komputera.", L"Er zijn geen updates beschikbaar voor uw pc." },
    { 1259, L"Update is ready to install", L"L'aggiornamento è pronto per l'installazione", L"La actualización está lista para instalarse", L"La mise à jour est prête à être installée", L"Güncelleme yüklenmeye hazır", L"Обновление готово к установке", L"A atualização está pronta para instalação", L"更新已准备好安装", L"Aktualizacja jest gotowa do instalacji", L"De update is klaar om te worden geïnstalleerd" },
    { 1260, L"Update is ready to download", L"L'aggiornamento è pronto per il download", L"La actualización está lista para descargarse", L"La mise à jour est prête à être téléchargée", L"Güncelleme indirilmeye hazır", L"Обновление готово к загрузке", L"A atualização está pronta para download", L"更新已准备好下载", L"Aktualizacja jest gotowa do pobrania", L"De update is klaar om te worden gedownload" },
    { 1264, L"You may need to restart your PC after installing this update.", L"Potrebbe essere necessario riavviare il PC dopo l'installazione di questo aggiornamento.", L"Es posible que deba reiniciar su PC después de instalar esta actualización.", L"Vous devrez peut-être redémarrer votre PC après l'installation de cette mise à jour.", L"Bu güncellemeyi yükledikten sonra bilgisayarınızı yeniden başlatmanız gerekebilir.", L"Возможно, потребуется перезапустить компьютер после установки этого обновления.", L"Talvez seja necessário reiniciar o PC após instalar esta atualização.", L"安装此更新后，你可能需要重启电脑。", L"Po zainstalowaniu tej aktualizacji może być konieczne ponowne uruchomienie komputera.", L"Mogelijk moet u uw pc opnieuw starten na het installeren van deze update." },
    { 1265, L"You will need to restart your PC after installing this update.", L"Dovrai riavviare il PC dopo l'installazione di questo aggiornamento.", L"Tendrá que reiniciar su PC después de instalar esta actualización.", L"Vous devrez redémarrer votre PC après l'installation de cette mise à jour.", L"Bu güncellemeyi yükledikten sonra bilgisayarınızı yeniden başlatmanız gerekecek.", L"Вам потребуется перезапустить компьютер после установки этого обновления.", L"Você precisará reiniciar o PC após instalar esta atualização.", L"安装此更新后，你将需要重启电脑。", L"Będziesz musiał ponownie uruchomić komputer po zainstalowaniu tej aktualizacji.", L"U moet uw pc opnieuw starten na het installeren van deze update." },
    { 1266, L"Published: ", L"Pubblicato: ", L"Publicado: ", L"Publiée : ", L"Yayınlanma: ", L"Опубликовано: ", L"Publicado: ", L"已发布：", L"Opublikowano: ", L"Gepubliceerd: " },
    { 1267, L"Important", L"Importante", L"Importante", L"Importante", L"Önemli", L"Важное", L"Importante", L"重要", L"Ważne", L"Belangrijk" },
    { 1268, L"Optional", L"Facoltativo", L"Opcional", L"Facultatif", L"İsteğe bağlı", L"Необязательное", L"Opcional", L"可选", L"Opcjonalne", L"Optioneel" },
    { 1269, L"Recommended Update", L"Aggiornamento consigliato", L"Actualización recomendada", L"Mise à jour recommandée", L"Önerilen Güncelleme", L"Рекомендуемое обновление", L"Atualização recomendada", L"推荐更新", L"Zalecana aktualizacja", L"Aanbevolen update" },
    { 1270, L"No updates are available.", L"Nessun aggiornamento disponibile.", L"No hay actualizaciones disponibles.", L"Aucune mise à jour disponible.", L"Kullanılabilir güncelleme yok.", L"Нет доступных обновлений.", L"Nenhuma atualização disponível.", L"没有可用更新。", L"Brak dostępnych aktualizacji.", L"Er zijn geen updates beschikbaar." },
    { 1272, L"", L"", L"", L"", L"", L"", L"", L"", L"", L"" },
    { 1273, L"%rebootPendingForInstall%", L"%rebootPendingForInstall%", L"%rebootPendingForInstall%", L"%rebootPendingForInstall%", L"%rebootPendingForInstall%", L"%rebootPendingForInstall%", L"%rebootPendingForInstall%", L"%rebootPendingForInstall%", L"%rebootPendingForInstall%", L"%rebootPendingForInstall%" },
    { 1279, L"There was a problem getting the list of updates for your PC. To continue, please reopen Windows Update", L"Si è verificato un problema durante il recupero dell'elenco degli aggiornamenti per il tuo PC. Per continuare, riapri Windows Update", L"Hubo un problema al obtener la lista de actualizaciones para su PC. Para continuar, vuelva a abrir Windows Update", L"Un problème est survenu lors de l'obtention de la liste des mises à jour pour votre PC. Pour continuer, rouvrez Windows Update", L"Bilgisayarınız için güncelleme listesi alınırken bir sorun oluştu. Devam etmek için Windows Update'i yeniden açın", L"При получении списка обновлений для компьютера возникла проблема. Чтобы продолжить, снова откройте Центр обновления Windows", L"Houve um problema ao obter a lista de atualizações para seu PC. Para continuar, reabra o Windows Update", L"获取你电脑的更新列表时出现问题。若要继续，请重新打开 Windows 更新", L"Wystąpił problem podczas pobierania listy aktualizacji dla Twojego komputera. Aby kontynuować, otwórz ponownie Windows Update", L"Er is een probleem opgetreden bij het ophalen van de updatelijst voor uw pc. Open Windows Update opnieuw om verder te gaan" },
    { 1281, L"Time until reboot", L"Tempo prima del riavvio", L"Tiempo hasta el reinicio", L"Temps avant redémarrage", L"Yeniden başlatmaya kalan süre", L"Время до перезагрузки", L"Tempo até a reinicialização", L"距重启的时间", L"Czas do ponownego uruchomienia", L"Tijd tot opnieuw opstarten" },
    { 1284, L"Res&ume", L"&Riprendi", L"&Reanudar", L"&Reprendre", L"&Sürdür", L"&Возобновить", L"&Retomar", L"&继续", L"&Wznów", L"&Hervatten" },
    { 1288, L"Updates will be automatically installed during the maintenance window.", L"Gli aggiornamenti verranno installati automaticamente durante la finestra di manutenzione.", L"Las actualizaciones se instalarán automáticamente durante la ventana de mantenimiento.", L"Les mises à jour seront automatiquement installées pendant la fenêtre de maintenance.", L"Güncellemeler bakım penceresi sırasında otomatik olarak yüklenecek.", L"Обновления будут автоматически установлены в окно обслуживания.", L"As atualizações serão instaladas automaticamente durante a janela de manutenção.", L"更新将在维护时段自动安装。", L"Aktualizacje będą automatycznie instalowane w oknie konserwacji.", L"Updates worden automatisch geïnstalleerd tijdens het onderhoudsvenster." },
    { 64501, L"Every day", L"Ogni giorno", L"Todos los días", L"Tous les jours", L"Her gün", L"Каждый день", L"Todos os dias", L"每天", L"Codziennie", L"Elke dag" },
    { 64502, L"Every Sunday", L"Ogni domenica", L"Todos los domingos", L"Tous les dimanches", L"Her Pazar", L"Каждое воскресенье", L"Todos os domingos", L"每星期日", L"W każdą niedzielę", L"Elke zondag" },
    { 64503, L"Every Monday", L"Ogni lunedì", L"Todos los lunes", L"Tous les lundis", L"Her Pazartesi", L"Каждый понедельник", L"Todas as segundas-feiras", L"每星期一", L"W każdy poniedziałek", L"Elke maandag" },
    { 64504, L"Every Tuesday", L"Ogni martedì", L"Todos los martes", L"Tous les mardis", L"Her Salı", L"Каждый вторник", L"Todas as terças-feiras", L"每星期二", L"W każdy wtorek", L"Elke dinsdag" },
    { 64505, L"Every Wednesday", L"Ogni mercoledì", L"Todos los miércoles", L"Tous les mercredis", L"Her Çarşamba", L"Каждую среду", L"Todas as quartas-feiras", L"每星期三", L"W każdą środę", L"Elke woensdag" },
    { 64506, L"Every Thursday", L"Ogni giovedì", L"Todos los jueves", L"Tous les jeudis", L"Her Perşembe", L"Каждый четверг", L"Todas as quintas-feiras", L"每星期四", L"W każdy czwartek", L"Elke donderdag" },
    { 64507, L"Every Friday", L"Ogni venerdì", L"Todos los viernes", L"Tous les vendredis", L"Her Cuma", L"Каждую пятницу", L"Todas as sextas-feiras", L"每星期五", L"W każdy piątek", L"Elke vrijdag" },
    { 64508, L"Every Saturday", L"Ogni sabato", L"Todos los sábados", L"Tous les samedis", L"Her Cumartesi", L"Каждую субботу", L"Todos os sábados", L"每星期六", L"W każdą sobotę", L"Elke zaterdag" },
    { 64531, L"Enter the credentials for proxy authentication", L"Immetti le credenziali per l'autenticazione proxy", L"Especifique las credenciales para la autenticación de proxy", L"Saisissez les informations d'identification pour l'authentification du proxy", L"Proxy kimlik doğrulaması için kimlik bilgilerini girin", L"Введите учетные данные для проверки подлинности прокси-сервера", L"Insira as credenciais para a autenticação de proxy", L"输入代理身份验证的凭据", L"Wprowadź poświadczenia do uwierzytelniania serwera proxy", L"Voer de referenties in voor proxyverificatie" },
    { 64532, L"Password Required", L"Password richiesta", L"Se requiere contraseña", L"Mot de passe requis", L"Parola gerekli", L"Требуется пароль", L"Senha necessária", L"需要密码", L"Wymagane hasło", L"Wachtwoord vereist" },
    { 20000, L"Modern Update Status", L"Stato aggiornamenti moderno", L"Estado de actualizaciones modernas", L"État des mises à jour moderne", L"Modern Güncelleme Durumu", L"Состояние современных обновлений", L"Status de atualizações modernas", L"现代更新状态", L"Nowoczesny stan aktualizacji", L"Moderne updatestatus" },
    { 20001, L"Available Updates", L"Aggiornamenti disponibili", L"Actualizaciones disponibles", L"Mises à jour disponibles", L"Kullanılabilir Güncellemeler", L"Доступные обновления", L"Atualizações disponíveis", L"可用更新", L"Dostępne aktualizacje", L"Beschikbare updates" },
    { 20002, L"Update History", L"Cronologia aggiornamenti", L"Historial de actualizaciones", L"Historique des mises à jour", L"Güncelleme Geçmişi", L"Журнал обновлений", L"Histórico de atualizações", L"更新历史记录", L"Historia aktualizacji", L"Updategeschiedenis" },
    { 20003, L"Check for Updates", L"Controlla aggiornamenti", L"Buscar actualizaciones", L"Rechercher des mises à jour", L"Güncellemeleri Denetle", L"Проверить наличие обновлений", L"Verificar atualizações", L"检查更新", L"Sprawdź aktualizacje", L"Controleren op updates" },
    { 20004, L"View Installed Updates", L"Visualizza aggiornamenti installati", L"Ver actualizaciones instaladas", L"Afficher les mises à jour installées", L"Yüklü Güncellemeleri Görüntüle", L"Просмотр установленных обновлений", L"Exibir atualizações instaladas", L"查看已安装的更新", L"Wyświetl zainstalowane aktualizacje", L"Geïnstalleerde updates bekijken" },
    { 20005, L"Last Check: %s", L"Ultimo controllo: %s", L"Última comprobación: %s", L"Dernière vérification : %s", L"Son denetim: %s", L"Последняя проверка: %s", L"Última verificação: %s", L"上次检查：%s", L"Ostatnie sprawdzenie: %s", L"Laatste controle: %s" },
    { 20006, L"Important: %d", L"Importanti: %d", L"Importantes: %d", L"Importantes : %d", L"Önemli: %d", L"Важные: %d", L"Importantes: %d", L"重要：%d", L"Ważne: %d", L"Belangrijk: %d" },
    { 20007, L"Optional: %d", L"Facoltativi: %d", L"Opcionales: %d", L"Facultatifs : %d", L"İsteğe bağlı: %d", L"Необязательные: %d", L"Opcionais: %d", L"可选：%d", L"Opcjonalne: %d", L"Optioneel: %d" },
    { 20008, L"Your PC is up to date!", L"Il tuo PC è aggiornato!", L"Su PC está actualizado.", L"Votre PC est à jour !", L"Bilgisayarınız güncel!", L"Ваш компьютер обновлен!", L"Seu PC está atualizado!", L"你的电脑已是最新！", L"Twój komputer jest aktualny!", L"Uw pc is up-to-date!" },
    { 20009, L"Checking for updates...", L"Ricerca aggiornamenti in corso...", L"Buscando actualizaciones...", L"Recherche des mises à jour...", L"Güncellemeler denetleniyor...", L"Проверка наличия обновлений...", L"Verificando atualizações...", L"正在检查更新...", L"Sprawdzanie aktualizacji...", L"Controleren op updates..." },
    { 20010, L"No updates found.", L"Nessun aggiornamento trovato.", L"No se encontraron actualizaciones.", L"Aucune mise à jour trouvée.", L"Güncelleme bulunamadı.", L"Обновления не найдены.", L"Nenhuma atualização encontrada.", L"未找到更新。", L"Nie znaleziono aktualizacji.", L"Geen updates gevonden." },
    { 20020, L"Get updates for other Microsoft products.", L"Ottieni aggiornamenti per altri prodotti Microsoft.", L"Obtén actualizaciones para otros productos de Microsoft.", L"Obtenez des mises à jour pour d'autres produits Microsoft.", L"Diğer Microsoft ürünleri için güncellemeleri alın.", L"Получайте обновления для других продуктов Microsoft.", L"Obtenha atualizações para outros produtos da Microsoft.", L"获取其他 Microsoft 产品的更新。", L"Pobierz aktualizacje dla innych produktów Microsoft.", L"Ontvang updates voor andere Microsoft-producten." },
    { 20021, L"Find out more", L"Scopri di più", L"Obtén más información", L"En savoir plus", L"Daha fazla bilgi edinin", L"Узнать больше", L"Saiba mais", L"了解更多信息", L"Dowiedz się więcej", L"Meer informatie" },
    { 20022, L"There are updates available", L"Sono disponibili aggiornamenti", L"Hay actualizaciones disponibles", L"Des mises à jour sont disponibles", L"Güncellemeler mevcut", L"Доступны обновления", L"Há atualizações disponíveis", L"有可用更新", L"Są dostępne aktualizacje", L"Er zijn updates beschikbaar" },
    { 20023, L"Go to Windows Settings to install them", L"Vai alle impostazioni di Windows per installarli", L"Ve a la configuración de Windows para instalarlas", L"Accédez aux paramètres Windows pour les installer", L"Bunları yüklemek için Windows Ayarları'na gidin", L"Перейдите в параметры Windows, чтобы установить их", L"Vá para as configurações do Windows para instalá-las", L"转到 Windows 设置以安装它们", L"Przejdź do ustawień systemu Windows, aby je zainstalować", L"Ga naar Windows-instellingen om ze te installeren" },
    { 20024, L"Updates: frequently asked questions", L"Aggiornamenti: domande frequenti", L"Actualizaciones: preguntas frecuentes", L"Mises à jour : questions fréquentes", L"Güncelleştirmeler: sık sorulan sorular", L"Обновления: часто задаваемые вопросы", L"Atualizações: perguntas frequentes", L"更新：常见问题", L"Aktualizacje: najczęściej zadawane pytania", L"Updates: veelgestelde vragen" },
    { 1190, L"Microsoft Update", L"Microsoft Update", L"Microsoft Update", L"Microsoft Update", L"Microsoft Update", L"Центр обновления Microsoft", L"Microsoft Update", L"Microsoft 更新", L"Microsoft Update", L"Microsoft Update" },
    { 1191, L"Give me updates for other Microsoft products when I update Windows", L"Dammi aggiornamenti per altri prodotti Microsoft quando aggiorno Windows", L"Darme actualizaciones para otros productos de Microsoft cuando actualizo Windows", L"Me donner les mises à jour pour d'autres produits Microsoft quand je mets à jour Windows", L"Windows'u güncellediğimde diğer Microsoft ürünleri için güncellemeler ver", L"Предоставлять обновления для других продуктов Microsoft при обновлении Windows", L"Dar-me atualizações para outros produtos da Microsoft ao atualizar o Windows", L"更新 Windows 时为我提供其他 Microsoft 产品的更新", L"Daj mi aktualizacje dla innych produktów Microsoft, gdy aktualizuję Windows", L"Geef mij updates voor andere Microsoft-producten wanneer ik Windows bijwerk" },
    { 64540, L"To view the update history, choose one of the following settings:", L"Per visualizzare la cronologia degli aggiornamenti, scegliere una delle seguenti impostazioni:", L"Para ver el historial de actualizaciones, elija una de las siguientes opciones:", L"Pour afficher l'historique des mises à jour, choisissez l'une des options suivantes :", L"Güncelleme geçmişini görüntülemek için aşağıdaki seçeneklerden birini seçin:", L"Чтобы просмотреть журнал обновлений, выберите один из следующих параметров:", L"Para ver o histórico de atualizações, escolha uma das seguintes opções:", L"要查看更新历史记录，请选择以下选项之一：", L"Aby wyświetlić historię aktualizacji, wybierz jedną z następujących opcji:", L"Om de updategeschiedenis weer te geven, kiest u een van de volgende opties:" },
    { 64541, L"View update history", L"Visualizza cronologia aggiornamenti", L"Ver historial de actualizaciones", L"Afficher l'historique des mises à jour", L"Güncelleme geçmişini görüntüle", L"Просмотреть журнал обновлений", L"Ver histórico de atualizações", L"查看更新历史记录", L"Wyświetl historię aktualizacji", L"Updategeschiedenis weergeven" },
    { 64542, L"Manage updates from the system settings", L"Gestisci gli aggiornamenti dalle impostazioni di sistema", L"Administrar las actualizaciones desde la configuración del sistema", L"Gérer les mises à jour à partir des paramètres du système", L"Güncellemeleri sistem ayarlarından yönetin", L"Управление обновлениями из параметров системы", L"Gerenciar atualizações nas configurações do sistema", L"在系统设置中管理更新", L"Zarządzaj aktualizacjami w ustawieniach systemu", L"Updates beheren via de systeeminstellingen" },
    { 0, nullptr }
};


static wchar_t HexUpper(BYTE nibble) {
    return nibble < 10 ? static_cast<wchar_t>(L'0' + nibble)
                       : static_cast<wchar_t>(L'A' + nibble - 10);
}


static bool ComputeSha256(const std::wstring& path, BYTE digest[32]) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    HCRYPTPROV provider = 0;
    HCRYPTHASH hash = 0;
    bool ok = false;
    if (CryptAcquireContextW(&provider, nullptr, nullptr, PROV_RSA_AES,
                             CRYPT_VERIFYCONTEXT) &&
        CryptCreateHash(provider, CALG_SHA_256, 0, 0, &hash)) {
        BYTE buffer[65536];
        DWORD read = 0;
        bool readOk = true;
        while (ReadFile(file, buffer, sizeof(buffer), &read, nullptr) && read) {
            if (!CryptHashData(hash, buffer, read, 0)) { readOk = false; break; }
        }
        DWORD size = 32;
        ok = readOk && CryptGetHashParam(hash, HP_HASHVAL, digest, &size, 0) && size == 32;
    }
    if (hash) CryptDestroyHash(hash);
    if (provider) CryptReleaseContext(provider, 0);
    CloseHandle(file);
    return ok;
}


static bool IsValidPayload(const std::wstring& path) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER fileSize{};
    IMAGE_DOS_HEADER dos{};
    DWORD read = 0;
    bool ok = GetFileSizeEx(file, &fileSize) && fileSize.QuadPart >= kMinDllSize &&
              ReadFile(file, &dos, sizeof(dos), &read, nullptr) &&
              read == sizeof(dos) && dos.e_magic == IMAGE_DOS_SIGNATURE;
    DWORD signature = 0;
    WORD machine = 0;
    if (ok) {
        LARGE_INTEGER offset{};
        offset.QuadPart = dos.e_lfanew;
        ok = SetFilePointerEx(file, offset, nullptr, FILE_BEGIN) &&
             ReadFile(file, &signature, sizeof(signature), &read, nullptr) &&
             read == sizeof(signature) && signature == IMAGE_NT_SIGNATURE &&
             ReadFile(file, &machine, sizeof(machine), &read, nullptr) &&
             read == sizeof(machine) && machine == IMAGE_FILE_MACHINE_AMD64;
    }
    CloseHandle(file);
    if (!ok) return false;
    BYTE digest[32];
    if (!ComputeSha256(path, digest)) return false;
    for (int i = 0; i < 32; ++i) {
        if (kExpectedSha256[i * 2] != HexUpper(digest[i] >> 4) ||
            kExpectedSha256[i * 2 + 1] != HexUpper(digest[i] & 15)) return false;
    }
    return true;
}


static const std::wstring& StoreDir() {
    static const std::wstring path = [] {
        std::vector<wchar_t> buffer(32768);
        size_t length = Wh_GetModStoragePath(buffer.data(), buffer.size());
        if (length == 0 || length >= buffer.size()) {
            Wh_Log(L"Windows Update Restorer: Wh_GetModStoragePath failed");
            return std::wstring();
        }
        return std::wstring(buffer.data(), length);
    }();
    return path;
}


// WinINet synchronous calls do not observe g_stopping until they return. Keep the
// active handles published so teardown can close them and break a blocked read.
static std::mutex g_downloadMutex;
static HINTERNET g_downloadInternet = nullptr;
static HINTERNET g_downloadUrl = nullptr;

static void CloseActiveDownloadHandles() {
    HINTERNET url = nullptr, internet = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_downloadMutex);
        url = g_downloadUrl; g_downloadUrl = nullptr;
        internet = g_downloadInternet; g_downloadInternet = nullptr;
    }
    if (url) InternetCloseHandle(url);
    if (internet) InternetCloseHandle(internet);
}

// Removes handles only if this worker still owns them. If teardown already took
// them, it is solely responsible for closing them, avoiding a double-close race.
static void CloseOwnedDownloadHandles(HINTERNET url, HINTERNET internet) {
    bool closeUrl = false, closeInternet = false;
    {
        std::lock_guard<std::mutex> lock(g_downloadMutex);
        if (g_downloadUrl == url) { g_downloadUrl = nullptr; closeUrl = true; }
        if (g_downloadInternet == internet) { g_downloadInternet = nullptr; closeInternet = true; }
    }
    if (closeUrl && url) InternetCloseHandle(url);
    if (closeInternet && internet) InternetCloseHandle(internet);
}

static bool DownloadWithTimeout(const std::wstring& destination) {
    HINTERNET internet = InternetOpenW(L"Windhawk Windows Update Restorer",
                                       INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!internet) return false;
    {
        std::lock_guard<std::mutex> lock(g_downloadMutex);
        if (g_stopping.load()) { InternetCloseHandle(internet); return false; }
        g_downloadInternet = internet;
    }
    DWORD timeout = kDownloadTimeoutMs;
    InternetSetOptionW(internet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOptionW(internet, INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOptionW(internet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));
    HINTERNET url = InternetOpenUrlW(internet, kDownloadUrl, nullptr, 0,
                                     INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE |
                                         INTERNET_FLAG_NO_UI,
                                     0);
    if (!url) { CloseOwnedDownloadHandles(nullptr, internet); return false; }
    {
        std::lock_guard<std::mutex> lock(g_downloadMutex);
        if (g_stopping.load()) {
            // The stop path either owns the parent already or will take both.
            // Publish the child before returning so it can abort it as well.
            g_downloadUrl = url;
        } else {
            g_downloadUrl = url;
        }
    }
    if (g_stopping.load()) { CloseOwnedDownloadHandles(url, internet); return false; }
    bool ok = false;
    DWORD status = 0, statusLength = sizeof(status), headerIndex = 0;
    if (HttpQueryInfoW(url, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &status,
                       &statusLength, &headerIndex) && status == HTTP_STATUS_OK) {
        HANDLE file = CreateFileW(destination.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                                  CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file != INVALID_HANDLE_VALUE) {
            BYTE buffer[65536];
            DWORD available = 0, read = 0, written = 0;
            const ULONGLONG started = GetTickCount64();
            ULONGLONG total = 0;
            ok = true;
            for (;;) {
                if (g_stopping.load() || GetTickCount64() - started > kDownloadTimeoutMs ||
                    !InternetQueryDataAvailable(url, &available, 0, 0)) { ok = false; break; }
                if (!available) break;
                if (available > sizeof(buffer)) available = sizeof(buffer);
                if (!InternetReadFile(url, buffer, available, &read) || !read ||
                    !WriteFile(file, buffer, read, &written, nullptr) || written != read) {
                    ok = false; break;
                }
                total += read;
                if (total > kMaxDownloadBytes) {
                    // Wrong/hostile endpoint: stop rather than keep writing. The
                    // SHA-256 check would reject it anyway.
                    Wh_Log(L"Windows Update Restorer: download exceeded the size limit; aborting");
                    ok = false;
                    break;
                }
            }
            CloseHandle(file);
        }
    }
    CloseOwnedDownloadHandles(url, internet);
    return ok;
}


static bool EnsurePayload(std::wstring& outPath) {
    std::wstring dir = StoreDir();
    if (dir.empty()) return false;
    const std::wstring finalPath = dir + L"\\" + kDllName;
    // Offline-first: a previously verified copy is reused with no network access
    // at all, so after the first successful run the page keeps working with no
    // internet connection.
    if (GetFileAttributesW(finalPath.c_str()) != INVALID_FILE_ATTRIBUTES &&
        IsValidPayload(finalPath)) {
        Wh_Log(L"Windows Update Restorer: reusing the cached verified payload (no download needed)");
        outPath = finalPath;
        return true;
    }
    if (g_stopping.load()) return false;
    // Per-process unique temp name: two processes that both slip past the setup
    // mutex (e.g. its 60s wait timed out) must never write the same temp file.
    const std::wstring temporaryPath =
        finalPath + L"." + std::to_wstring(GetCurrentProcessId()) + L".tmp";
    const ULONGLONG setupStarted = GetTickCount64();
    for (int attempt = 1; attempt <= kMaxDownloadAttempts && !g_stopping.load(); ++attempt) {
        if (GetTickCount64() - setupStarted > kOverallSetupDeadlineMs) {
            Wh_Log(L"Windows Update Restorer: overall setup deadline reached; giving up for this session");
            break;
        }
        DeleteFileW(temporaryPath.c_str());
        Wh_Log(L"Downloading verified Windows 8.1 wucltux.dll, attempt %d/%d", attempt,
               kMaxDownloadAttempts);
        if (DownloadWithTimeout(temporaryPath) && IsValidPayload(temporaryPath)) {
            // Another process may have completed and loaded the same payload
            // while we were downloading; replacing a mapped DLL fails. Treat an
            // already-valid destination as success rather than an error.
            if (MoveFileExW(temporaryPath.c_str(), finalPath.c_str(),
                            MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
                outPath = finalPath;
                Wh_Log(L"Windows Update Restorer: payload downloaded and verified (SHA-256 match)");
                return true;
            }
            const DWORD moveError = GetLastError();
            if (IsValidPayload(finalPath)) {
                Wh_Log(L"Windows Update Restorer: another process installed the verified payload first (err=%u); reusing it",
                       moveError);
                DeleteFileW(temporaryPath.c_str());
                outPath = finalPath;
                return true;
            }
            Wh_Log(L"Windows Update Restorer: could not install the verified payload (err=%u)", moveError);
        }
        DeleteFileW(temporaryPath.c_str());
        if (attempt < kMaxDownloadAttempts && g_stopEvent &&
            WaitForSingleObject(g_stopEvent, kRetryDelayMs) == WAIT_OBJECT_0) break;
    }
    return false;
}


// -----------------------------------------------------------------------------
// Private resource-module builder (adapted from the Performance Information and
// Tools Restorer). UpdateResource cannot normally modify a MUI-configured PE,
// hence DisableMuiConfigInPrivateCopy is applied only to the private copy.
// -----------------------------------------------------------------------------
class UniqueWinHandle {
public:
    UniqueWinHandle() = default;
    explicit UniqueWinHandle(HANDLE handle) : handle_(handle) {}
    ~UniqueWinHandle() { Reset(); }

    UniqueWinHandle(const UniqueWinHandle&) = delete;
    UniqueWinHandle& operator=(const UniqueWinHandle&) = delete;

    UniqueWinHandle(UniqueWinHandle&& other) noexcept
        : handle_(other.Release()) {}
    UniqueWinHandle& operator=(UniqueWinHandle&& other) noexcept {
        if (this != &other) Reset(other.Release());
        return *this;
    }


    bool IsValid() const { return handle_ && handle_ != INVALID_HANDLE_VALUE; }

    HANDLE Get() const { return handle_; }
    HANDLE Release() {

        HANDLE result = handle_;
        handle_ = INVALID_HANDLE_VALUE;
        return result;
    }

    void Reset(HANDLE handle = INVALID_HANDLE_VALUE) {

        if (IsValid()) CloseHandle(handle_);
        handle_ = handle;
    }


private:
    HANDLE handle_ = INVALID_HANDLE_VALUE;
};

class ScopedTemporaryFile {

public:
    explicit ScopedTemporaryFile(std::wstring path) : path_(std::move(path)) {}
    ~ScopedTemporaryFile() {

        if (!committed_ && !path_.empty()) DeleteFileW(path_.c_str());
    }

    ScopedTemporaryFile(const ScopedTemporaryFile&) = delete;
    ScopedTemporaryFile& operator=(const ScopedTemporaryFile&) = delete;
    void Commit() { committed_ = true; }


private:
    std::wstring path_;
    bool committed_ = false;
};

class ResourceUpdateTransaction {

public:
    explicit ResourceUpdateTransaction(const std::wstring& path)
        : update_(BeginUpdateResourceW(path.c_str(), FALSE)) {}
    ~ResourceUpdateTransaction() {

        if (update_) EndUpdateResourceW(update_, TRUE);

    }

    ResourceUpdateTransaction(const ResourceUpdateTransaction&) = delete;
    ResourceUpdateTransaction& operator=(const ResourceUpdateTransaction&) = delete;


    bool IsValid() const { return update_ != nullptr; }

    HANDLE Get() const { return update_; }

    bool Commit() {

        if (!update_) return false;
        HANDLE update = update_;
        update_ = nullptr;
        return EndUpdateResourceW(update, FALSE) != FALSE;
    }


private:
    HANDLE update_ = nullptr;
};

template <typename T>
static bool ReadPeValue(const std::vector<BYTE>& file, size_t offset, T& value) {

    if (offset > file.size() || file.size() - offset < sizeof(T)) return false;
    memcpy(&value, file.data() + offset, sizeof(T));
    return true;
}


// UpdateResource intentionally restricts LN/MUI binaries. Rename the private
// copy's named "MUI" RC-config resource to the unused name "CUI" first. This
// changes only the copy and makes it a normal resource PE. Note: this depends on
// undocumented layout details of the specific Microsoft binary at the symbol
// server URL; it may need updating if that binary ever changes. It is confined
// to a private copy, so the blast radius is small.
static bool DisableMuiConfigInPrivateCopy(const std::wstring& path) {
    UniqueWinHandle file(CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE,
                                     FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                                     FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!file.IsValid()) return false;


    LARGE_INTEGER size = {};
    if (!GetFileSizeEx(file.Get(), &size) || size.QuadPart <= 0 ||
        size.QuadPart > 64 * 1024 * 1024) {

        return false;
    }


    std::vector<BYTE> bytes(static_cast<size_t>(size.QuadPart));
    DWORD bytesRead = 0;
    if (!ReadFile(file.Get(), bytes.data(), static_cast<DWORD>(bytes.size()),
                  &bytesRead, nullptr) ||
        bytesRead != bytes.size()) {

        return false;
    }


    IMAGE_DOS_HEADER dos = {};
    if (!ReadPeValue(bytes, 0, dos) || dos.e_magic != IMAGE_DOS_SIGNATURE ||
        dos.e_lfanew < 0) {

        return false;
    }


    const size_t ntOffset = static_cast<size_t>(dos.e_lfanew);
    DWORD signature = 0;
    IMAGE_FILE_HEADER fileHeader = {};
    if (!ReadPeValue(bytes, ntOffset, signature) ||
        signature != IMAGE_NT_SIGNATURE ||
        !ReadPeValue(bytes, ntOffset + sizeof(DWORD), fileHeader)) {

        return false;
    }


    const size_t optionalOffset = ntOffset + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER);
    WORD optionalMagic = 0;
    if (!ReadPeValue(bytes, optionalOffset, optionalMagic)) return false;


    DWORD resourceRva = 0;
    if (optionalMagic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        IMAGE_OPTIONAL_HEADER64 optional = {};
        if (!ReadPeValue(bytes, optionalOffset, optional) ||
            optional.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_RESOURCE) {

            return false;
        }
        resourceRva =
            optional.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress;
    } else if (optionalMagic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        IMAGE_OPTIONAL_HEADER32 optional = {};
        if (!ReadPeValue(bytes, optionalOffset, optional) ||
            optional.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_RESOURCE) {

            return false;
        }
        resourceRva =
            optional.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress;
    } else {

        return false;
    }
    if (!resourceRva) return false;


    const size_t sectionOffset = optionalOffset + fileHeader.SizeOfOptionalHeader;
    DWORD resourceRaw = 0;
    for (WORD i = 0; i < fileHeader.NumberOfSections; ++i) {
        IMAGE_SECTION_HEADER section = {};
        if (!ReadPeValue(bytes, sectionOffset + i * sizeof(IMAGE_SECTION_HEADER),
                         section)) {

            return false;
        }
        const DWORD virtualSize = section.Misc.VirtualSize;
        const DWORD span = virtualSize > section.SizeOfRawData ? virtualSize
                                                               : section.SizeOfRawData;
        if (resourceRva >= section.VirtualAddress &&
            resourceRva - section.VirtualAddress < span) {

            resourceRaw = section.PointerToRawData +
                          (resourceRva - section.VirtualAddress);

            break;
        }
    }
    if (!resourceRaw || resourceRaw >= bytes.size()) return false;


    IMAGE_RESOURCE_DIRECTORY root = {};
    if (!ReadPeValue(bytes, resourceRaw, root)) return false;
    const DWORD entryCount = static_cast<DWORD>(root.NumberOfNamedEntries) +
                             root.NumberOfIdEntries;
    const size_t entriesOffset = resourceRaw + sizeof(root);


    size_t muiFirstCharacterOffset = 0;
    for (DWORD i = 0; i < entryCount; ++i) {
        IMAGE_RESOURCE_DIRECTORY_ENTRY entry = {};
        if (!ReadPeValue(bytes, entriesOffset + i * sizeof(entry), entry)) {

            return false;
        }
        DWORD nameField = 0;
        memcpy(&nameField, &entry, sizeof(nameField));
        if (!(nameField & 0x80000000u)) continue;
        const size_t stringOffset = resourceRaw + (nameField & 0x7FFFFFFFu);

        WORD length = 0;
        if (!ReadPeValue(bytes, stringOffset, length) || length != 3) continue;
        WCHAR name[3] = {};
        if (stringOffset + sizeof(WORD) > bytes.size() ||
            bytes.size() - (stringOffset + sizeof(WORD)) < sizeof(name)) {

            return false;
        }

        memcpy(name, bytes.data() + stringOffset + sizeof(WORD), sizeof(name));
        if (name[0] == L'M' && name[1] == L'U' && name[2] == L'I') {
            muiFirstCharacterOffset = stringOffset + sizeof(WORD);
            break;
        }
    }
    if (!muiFirstCharacterOffset) return false;


    LARGE_INTEGER position = {};
    position.QuadPart = static_cast<LONGLONG>(muiFirstCharacterOffset);

    if (!SetFilePointerEx(file.Get(), position, nullptr, FILE_BEGIN)) return false;
    const WCHAR replacement = L'C';
    DWORD written = 0;
    return WriteFile(file.Get(), &replacement, sizeof(replacement), &written,
                     nullptr) &&
           written == sizeof(replacement);

}



// -----------------------------------------------------------------------------
// Embedded MUI strings. The matching MUI file is not downloaded: the classic
// page receives its strings from this table, keeping the mod self-contained.
// -----------------------------------------------------------------------------
using LoadStringW_t = int(WINAPI*)(HINSTANCE, UINT, LPWSTR, int);
static LoadStringW_t LoadStringWOriginal = nullptr;

// Returns the string for the currently selected language (g_language), falling
// back to English for any unknown code.
static const wchar_t* EmbeddedMuiString(UINT id) {
    for (const auto* item = kWucltuxMuiStrings; item->en; ++item) {
        if (item->id != id) continue;
        if (LanguageIs(L"it")) return item->it;
        if (LanguageIs(L"es")) return item->es;
        if (LanguageIs(L"fr")) return item->fr;
        if (LanguageIs(L"tr")) return item->tr;
        if (LanguageIs(L"ru")) return item->ru;
        if (LanguageIs(L"pt")) return item->pt;
        if (LanguageIs(L"zh")) return item->zh;
        if (LanguageIs(L"pl")) return item->pl;
        if (LanguageIs(L"nl")) return item->nl;
        return item->en; // default / fallback
    }
    return nullptr;
}

// Returns the translated Control Panel InfoTip (the grey tooltip shown on hover
// over the "Windows Update" item in the Control Panel) for the currently
// selected language. English is the fallback for any unknown code.
static const wchar_t* InfoTipForLanguage() {
    if (LanguageIs(L"it"))
        return L"Controlla gli aggiornamenti e visualizza la cronologia degli aggiornamenti.";
    if (LanguageIs(L"es"))
        return L"Busca actualizaciones y consulta el historial de actualizaciones.";
    if (LanguageIs(L"fr"))
        return L"Recherchez les mises à jour et consultez l'historique des mises à jour.";
    if (LanguageIs(L"tr"))
        return L"Güncellemeleri denetleyin ve güncelleme geçmişini görüntüleyin.";
    if (LanguageIs(L"ru"))
        return L"Проверьте наличие обновлений и просмотрите журнал обновлений.";
    if (LanguageIs(L"pt"))
        return L"Verifique atualizações e consulte o histórico de atualizações.";
    if (LanguageIs(L"zh"))
        return L"检查更新并查看更新历史记录。";
    if (LanguageIs(L"pl"))
        return L"Sprawdź aktualizacje i wyświetl historię aktualizacji.";
    if (LanguageIs(L"nl"))
        return L"Controleer op updates en bekijk de updategeschiedenis.";
    return L"Check for updates and view update history."; // en / fallback
}

static bool IsWucltuxInstance(HINSTANCE instance) {
    if (!instance) return false;
    const ULONG_PTR raw = reinterpret_cast<ULONG_PTR>(instance);
    return reinterpret_cast<HMODULE>(raw & ~static_cast<ULONG_PTR>(3)) == g_module.load();
}
static int CopyEmbeddedString(const wchar_t* text, LPWSTR buffer, int bufferChars) {
    if (!text) return 0;
    const int length = static_cast<int>(wcslen(text));
    if (bufferChars == 0) {
        if (!buffer) return 0;
        *reinterpret_cast<LPCWSTR*>(buffer) = text;
        return length;
    }
    if (!buffer || bufferChars < 1) return 0;
    const int copied = length < bufferChars - 1 ? length : bufferChars - 1;
    if (copied) memcpy(buffer, text, static_cast<size_t>(copied) * sizeof(wchar_t));
    buffer[copied] = 0;
    return copied;
}
static int WINAPI LoadStringWHook(HINSTANCE instance, UINT id, LPWSTR buffer, int bufferChars) {
    if (IsWucltuxInstance(instance)) {
        if (const wchar_t* text = EmbeddedMuiString(id))
            return CopyEmbeddedString(text, buffer, bufferChars);
    }
    return LoadStringWOriginal(instance, id, buffer, bufferChars);
}


// Build an RT_STRING payload block (16 consecutive string IDs).
static bool BuildWucltuxStringBlock(UINT blockId, std::vector<BYTE>& output) {
    output.clear();
    bool hasText = false;
    for (UINT index = 0; index < 16; ++index) {
        const UINT id = (blockId - 1) * 16 + index;
        const wchar_t* text = EmbeddedMuiString(id);
        const WORD length = text ? static_cast<WORD>(wcslen(text)) : 0;
        const BYTE* lengthBytes = reinterpret_cast<const BYTE*>(&length);
        output.insert(output.end(), lengthBytes, lengthBytes + sizeof(length));
        if (length) {
            const BYTE* textBytes = reinterpret_cast<const BYTE*>(text);
            output.insert(output.end(), textBytes, textBytes + length * sizeof(wchar_t));
            hasText = true;
        }
    }
    return hasText;
}


// Verifies that a candidate .mres file at `path` actually contains our
// embedded strings (loads it as a data/image resource module and checks a
// known string id resolves to the expected text). Returns the loaded module
// on success (caller takes ownership) or nullptr on any failure, in which
// case any partially-loaded module is freed.
static HMODULE ValidateEmbeddedMuiResourceModule(const std::wstring& path) {
    HMODULE module = LoadLibraryExW(path.c_str(), nullptr,
                                    LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    if (!module) return nullptr;
    // Pick a stable, always-present id from the table (id 1, "Windows Update")
    // and confirm it resolves via a real resource lookup, not just that the
    // file loaded.
    const wchar_t* expected = EmbeddedMuiString(1);
    wchar_t buffer[64] = {};
    const int copied = LoadStringW(module, 1, buffer, ARRAYSIZE(buffer));
    if (copied <= 0 || !expected || wcscmp(buffer, expected) != 0) {
        FreeLibrary(module);
        return nullptr;
    }
    return module;
}

// Scans the storage directory for an already-built, still-valid embedded-mui
// module from an earlier generation in this same process and reuses it
// instead of building a new one. This matters because we deliberately never
// FreeLibrary the modules we load (a Control Panel page can keep a live
// reference into one), so every rebuild leaves the previous file locked in
// memory forever - repeatedly creating fresh files on every mod
// enable/disable cycle is exactly the kind of rapid file churn that trips
// ransomware-protection heuristics in AV/EDR products. Reusing a known-good
// existing file avoids that churn entirely.
static bool ReuseExistingEmbeddedMuiResourceModule(const std::wstring& sourceDir) {
    // Only reuse files built for the current language. The filename embeds the
    // language code so a module built for one language is never mistaken for
    // another (their ID-1 string is often identical, so validating only against
    // ID-1 is not enough to tell them apart).
    const std::wstring prefix =
        L"wucltux.embedded-mui-" + std::to_wstring(GetCurrentProcessId()) + L"-" +
        CurrentLanguage() + L"-";
    const std::wstring pattern = sourceDir + L"\\" + prefix + L"*.mres";

    WIN32_FIND_DATAW findData{};
    HANDLE find = FindFirstFileW(pattern.c_str(), &findData);
    if (find == INVALID_HANDLE_VALUE) return false;
    bool reused = false;
    do {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        const std::wstring candidate = sourceDir + L"\\" + findData.cFileName;
        if (HMODULE module = ValidateEmbeddedMuiResourceModule(candidate)) {
            std::lock_guard<std::mutex> lock(g_resourceMutex);
            g_resourcePath = candidate;
            g_resourceModule.store(module);
            reused = true;
            Wh_Log(L"Windows Update Restorer: reusing existing embedded MUI resource module: %s",
                   candidate.c_str());
            break;
        }
    } while (!reused && FindNextFileW(find, &findData));
    FindClose(find);
    return reused;
}

static bool BuildEmbeddedMuiResourceModule(const std::wstring& sourcePath) {
    const size_t sourceSlash = sourcePath.find_last_of(L"\\/");
    if (sourceSlash == std::wstring::npos) return false;
    if (ReuseExistingEmbeddedMuiResourceModule(sourcePath.substr(0, sourceSlash))) return true;
    // Do the actual file work without holding g_resourceMutex: it can take
    // several seconds now (see the retry loop below), and holding the lock
    // that long would stall the UI thread if it calls
    // EmbeddedMuiResourceModule() (via XResourceProviderCreateHook) while a
    // rebuild is in progress. We only touch g_resourcePath - the one piece
    // of shared state - briefly, under the lock, at the very end. We also
    // deliberately do NOT clear g_resourcePath up front: if a previous
    // build already succeeded, its (still-loaded) file stays usable for
    // string lookups while this rebuild attempt is in flight.
    const size_t slash = sourcePath.find_last_of(L"\\/");
    if (slash == std::wstring::npos) return false;
    // The destination filename must be unique per *build attempt*, not just
    // per process: if the mod is reloaded without restarting explorer.exe,
    // the previous build's file can still be mapped in memory (loaded via
    // LOAD_LIBRARY_AS_DATAFILE) and reusing its name makes MoveFileExW fail
    // with ERROR_ACCESS_DENIED (5).
    static std::atomic<uint32_t> generation{0};
    // Deliberately NOT a .dll extension: several AV/EDR products (Defender
    // Controlled Folder Access, Attack Surface Reduction "block unknown
    // executables" rules, etc.) can permanently deny creation/rename of
    // newly-written .dll/.exe files by a process like explorer.exe - not a
    // transient lock, so no amount of retrying helps. LoadLibraryExW with
    // LOAD_LIBRARY_AS_DATAFILE|LOAD_LIBRARY_AS_IMAGE_RESOURCE doesn't care
    // about the extension, only the PE content, so give the file a
    // non-executable-looking extension to sidestep extension-based heuristics.
    const std::wstring destination = sourcePath.substr(0, slash + 1) +
                                     L"wucltux.embedded-mui-" +
                                     std::to_wstring(GetCurrentProcessId()) + L"-" +
                                     CurrentLanguage() + L"-" +
                                     std::to_wstring(generation.fetch_add(1) + 1) + L".mres";
    const std::wstring temporary = destination + L".tmp";
    ScopedTemporaryFile temporaryGuard(temporary);
    DeleteFileW(temporary.c_str());
    if (!CopyFileW(sourcePath.c_str(), temporary.c_str(), FALSE)) return false;
    if (!DisableMuiConfigInPrivateCopy(temporary)) {
        Wh_Log(L"Windows Update Restorer: could not neutralize MUI config in private copy");
        return false;
    }
    ResourceUpdateTransaction update(temporary);
    if (!update.IsValid()) {
        Wh_Log(L"Windows Update Restorer: BeginUpdateResource failed (%u)", GetLastError());
        return false;
    }
    std::vector<BYTE> block;
    // en-US plus it-IT: the latter prevents the Italian Control Panel resource
    // lookup from missing the module before it can fall back to English.
    static const WORD languages[] = {0x0000, 0x0409, 0x0410};
    int blocksWritten = 0;
    int blocksFailed = 0;
    for (UINT blockId = 1; blockId <= 1251; ++blockId) {  // Extended to include WUA strings
        if (!BuildWucltuxStringBlock(blockId, block)) continue;
        for (WORD language : languages) {
            if (!UpdateResourceW(update.Get(), RT_STRING, MAKEINTRESOURCEW(blockId), language,
                                 block.data(), static_cast<DWORD>(block.size()))) {
                // Don't abort the whole build on a single failed block: log it
                // and keep going, so a transient/localized failure (e.g. AV
                // briefly locking the temp file) doesn't leave the page with
                // zero embedded strings.
                Wh_Log(L"Windows Update Restorer: UpdateResource failed (block=%u lang=%04X err=%u)",
                       blockId, language, GetLastError());
                ++blocksFailed;
                continue;
            }
            ++blocksWritten;
        }
    }
    if (blocksWritten == 0) {
        Wh_Log(L"Windows Update Restorer: no string blocks could be written, aborting embedded MUI build");
        return false;
    }
    if (blocksFailed > 0) {
        Wh_Log(L"Windows Update Restorer: embedded MUI build had %d failed block writes (continuing with %d successful)",
               blocksFailed, blocksWritten);
    }
    if (!update.Commit()) {
        Wh_Log(L"Windows Update Restorer: EndUpdateResource failed (%u)", GetLastError());
        return false;
    }
    DeleteFileW(destination.c_str());
    // MoveFileExW can fail with ERROR_ACCESS_DENIED (5) right after the
    // resource-patched file is written, typically because AV/EDR real-time
    // protection holds it open for an on-write scan. That scan can take
    // several seconds (cloud lookups, sandboxing), not milliseconds, so use
    // a longer exponential backoff. This runs on the background setup
    // thread, so a multi-second wait here does not block the UI.
    static const int kMaxMoveAttempts = 10;
    static const DWORD kInitialMoveRetryDelayMs = 200;
    static const DWORD kMaxMoveRetryDelayMs = 2000;
    bool moved = false;
    DWORD lastMoveError = 0;
    DWORD retryDelayMs = kInitialMoveRetryDelayMs;
    for (int attempt = 1; attempt <= kMaxMoveAttempts; ++attempt) {
        if (MoveFileExW(temporary.c_str(), destination.c_str(),
                        MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
            moved = true;
            break;
        }
        lastMoveError = GetLastError();
        if (lastMoveError != ERROR_ACCESS_DENIED && lastMoveError != ERROR_SHARING_VIOLATION) {
            break;  // Not a transient-lock error; retrying won't help.
        }
        if (g_stopping.load()) break;
        if (attempt < kMaxMoveAttempts) {
            Wh_Log(L"Windows Update Restorer: activating embedded MUI module failed (%u), retrying in %ums (%d/%d)",
                   lastMoveError, retryDelayMs, attempt, kMaxMoveAttempts);
            // Wait on both the global teardown event and the per-rebuild abort
            // event: a newer Wh_ModSettingsChanged call signals the latter to
            // cut this stale rebuild's backoff short instead of waiting out
            // the join.
            HANDLE waitOn[2];
            DWORD waitCount = 0;
            if (g_stopEvent) waitOn[waitCount++] = g_stopEvent;
            if (g_rebuildAbortEvent) waitOn[waitCount++] = g_rebuildAbortEvent;
            if (waitCount > 0 &&
                WaitForMultipleObjects(waitCount, waitOn, FALSE, retryDelayMs) < WAIT_OBJECT_0 + waitCount) {
                break;
            }
            retryDelayMs = retryDelayMs < kMaxMoveRetryDelayMs / 2 ? retryDelayMs * 2 : kMaxMoveRetryDelayMs;
        }
    }
    if (!moved) {
        Wh_Log(L"Windows Update Restorer: activating embedded MUI module failed (%u) after %d attempts",
               lastMoveError, kMaxMoveAttempts);
        return false;
    }
    temporaryGuard.Commit();
    {
        std::lock_guard<std::mutex> lock(g_resourceMutex);
        g_resourcePath = destination;
    }
    Wh_Log(L"Windows Update Restorer: embedded MUI resource module ready (%d blocks): %s",
           blocksWritten, destination.c_str());
    return true;
}


static HMODULE EmbeddedMuiResourceModule() {
    std::lock_guard<std::mutex> lock(g_resourceMutex);
    if (HMODULE module = g_resourceModule.load()) return module;
    if (g_resourcePath.empty()) return nullptr;
    HMODULE module = LoadLibraryExW(g_resourcePath.c_str(), nullptr,
                                   LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    if (!module) Wh_Log(L"Windows Update Restorer: loading private MUI resource module failed (%u)", GetLastError());
    g_resourceModule.store(module);
    return module;
}

// Rebuilds the embedded MUI resource module for the currently selected language
// and reloads it. The classic page reads its strings from this module, which is
// built once at startup; changing the language therefore requires a rebuild. We
// run this on the background rebuild thread so it does not block the UI.
static void RebuildEmbeddedMuiForLanguage() {
    std::lock_guard<std::mutex> lock(g_rebuildMutex);
    if (g_stopping.load()) return;
    const std::wstring* path = g_dllPath.load();
    if (!path || path->empty()) {
        Wh_Log(L"Windows Update Restorer: no wucltux.dll path available for language rebuild");
        return;
    }
    if (!BuildEmbeddedMuiResourceModule(*path)) {
        Wh_Log(L"Windows Update Restorer: language rebuild of embedded MUI module failed");
        return;
    }
    // Point the cached module handle at the newly built file. We deliberately
    // never FreeLibrary the old module (a page may still reference it); it is
    // simply replaced as the source for future lookups.
    {
        std::lock_guard<std::mutex> rl(g_resourceMutex);
        g_resourceModule.store(nullptr);
    }
    EmbeddedMuiResourceModule();
    Wh_Log(L"Windows Update Restorer: embedded MUI module rebuilt for language %s", LanguageCode());
}


// -----------------------------------------------------------------------------
// Embedded warning shield (user supplied image, white outer background removed,
// converted to a small multi-size ICO and stored as base64 in this source).
// -----------------------------------------------------------------------------
static const UINT kLegacyWarningShieldIconId = 61002;
static const char kLegacyWarningShieldIcoBase64[] =
    "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"
    "5gAAAABJRU5ErkJggg==";


// -----------------------------------------------------------------------------
// Embedded "updates installed / up to date" green shield-with-check icon
// (user supplied image, background removed). Used by the Win7-style banner when
// the system is fully up to date.
// -----------------------------------------------------------------------------
static const UINT kUpdatesInstalledIconId = 61003;
// The icon is stored as a raw PNG with transparency (not wrapped in an ICO) so
// GDI+ can decode it and scale it with HighQualityBicubic interpolation at the
// requested size. A single string literal (concatenated across lines).
static const char kUpdatesInstalledPngBase64[] =
    "iVBORw0KGgoAAAANSUhEUgAAAOYAAADmCAYAAADBavm7AADwqElEQVR4nOy9d9xl"
    "11nf+11rl9PfPjPvFI1mRhp1yZJl2cY2GFzAxgVT7GAILbTQTAIh98INcD+5ISSU"
    "hM9NQhLIBZIQTC82xWDiYIOL5CbJ6tM1mvb2U3dda90/1l777HPmnRlZ8mvj4PV+"
    "zuc9ZZ99dlnPetrv+T2CHRjHjx8nTVMajQZ5nmOM4ejRo895f6dOnSLLMrTWeJ6H"
    "lPKa3/E8jyzLntfvnjhxAq01vu8zGo0Iw5AwDNFao7W+6ndvuOGGq35ujEEIse1n"
    "58+fRylTvtZak2UZcRwTxzFpmvL0008bgDRN8X2fNE3J85xWq0Ucxwgh8H2f2dlZ"
    "lpeXxb59+9i1axdh6F/xmE6dOoUQAiEEeZ5f9fi/cH/t/b3SPXy+48p36fns1PcJ"
    "goAgCBBCoJR6XvvzPK+cyFJK0jQlDMNrfsfzvOf1u0opjDGEYVjeAGMMUspnNXmu"
    "Ntwk3G7kuZ0sSZIwGAzo9/sMB5EZjUbEcUyWZTQaDYQQBH6NIAjQDU2e5wgh0Aoa"
    "jQZZltHd6rO12TNPPXmcer1Op9Oh3W6Lm2+5gcXFxYnfdcIhpaRWq131+L9wf3d2"
    "7IhgulW8ugo+n5FlGVmWIaUsL2Kaplf9TpIktNvt5/W79XrdCsVwWN6oPM/xfR9j"
    "zDW+ffVxtWuSpikXL14kjmP6vaEZjUal0Pm+TxiG1Ov1cjIFQYDnecJNKK01aZoa"
    "t/JnWUaSJKRpyubmJsPh0KyuXWDXrl3iuuuuY/fu3dRqNWZmZojjmCiKrnl9v3B/"
    "d3bsiGAGQVCaAs1m83mtPsYYarUaYRiSZRme5+H7/jVNLd/38f3nfnrGGBqNBmAn"
    "gVuhhRDUarVr/v5zGVrD+vo6GxsbnHvmQilYUkparRb1ep1arSZ830cIK4BKKbTW"
    "GANKabIsN0IIAQLP86nXAzzPQylFHMckSYLWivX1dba2tsz58+dZWloShw8fJssy"
    "Op0O9XqdOI6vcaxfuL87OXbEQJ5ebdI0pd/vs7q6Sq1We1arkZSSKIrYt28fzWaT"
    "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"
    "qsaPpzzC8cMJwAFHYOUswsinZJYpdcrc/c17uGLnbl76opeKxaUlmn6LnsIGZqfn"
    "EEIr8szcLN///vf5tV/7NSGlZPPmzSkXkuu6p+0WfeTIkVMef7ryjKVLms3mCqKj"
    "KFAYwqTjB3h+iGGZ+GGAYVhs2DBCX6WX6ZNTdNo+fhBRLBY5euIooHj9q18rqqKP"
    "k49MYwUOeSuHEIpiuYAXtTEc7a5qF0krpUjItoToCtqsk+9cZ98l43yeZj7QbexM"
    "ZWBgIZQuJJZIlNAVI0DMAK57Mhoygsij4zXZcuFmvvLgV/inB/6JwnlFzCHdrj1p"
    "qoSShEISYaIwUZEEEYIItTp6PpZt0vQalGoFyMHt37+N+w8+QHm4jMxLjJyDaVtg"
    "aG5VKQSotXhfn/4+M2sp15PsHj/7mcueykpLufr76eOWkNimxHYNgqhD3s7jBi6T"
    "+2bwJwN++vVvIZ93mZ6fpFDJ0VrycaSrUymOyXe+8x21efNmtm7dSrlcplAoUKlU"
    "KBaLZ9TCPcEtny15Rjl/LEs3afF9H8cu0G57+F6oe0lKHVCRlkmr1eH8888X83OL"
    "WIaDISzcfE6MnhxXR44dZuOGTbxkz3VMHZ7Gimz6ewdRSmHn3BV4WYijpbHLKZTU"
    "6JgzbAqz1t5YCBG7mCEQlz8pGUdjDV3niU4hGIYOr/pRm47fIVQ+wtQt5Rxh0Gos"
    "ccG15/CF+7/Ee7/0PnZceQ6V3jL1hUWMSAeSFCahMHTFNRJJgCTAazcxhMS2bZqt"
    "FpW+MnPNeb7yza8gXQO75NJWAYVyAdtx8Fqd1P2UmW3Fyv332kGh7LFVz1knD3m6"
    "sfz8ZQLvtc53EhHOHpcKoiDU/VQtgR969PX14UiH2WOznLflAm68/mVivr5E229R"
    "KBeYnJjCsXXUNVQRd911F694xSuYmZlhZGSEpaWldI95pvWuP/LUIqC7TCc5Ic1G"
    "oOslXcciCn2itqfr9SJJ3s1RLlcZ3jDI3icexQ+aKufaSCGYnV2kXm/zM297m/Dr"
    "Pke+eZiRYCMlUcM0bOy8geuauMLGDlxAEsoIYXUwbR8LE0tZcR1gdt9grhgJ3N1A"
    "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";

// -----------------------------------------------------------------------------
// Embedded Windows 8.1-style Windows Update status icon (user supplied PNG).
// Used when UpdatePageSkin is set to windows81. Stored as a raw PNG so GDI+
// can scale it with HighQualityBicubic interpolation at render time.
// -----------------------------------------------------------------------------
static const UINT kWindows81UpdateStatusIconId = 61005;
static const char kWindows81UpdateStatusPngBase64[] =
    "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";
// -----------------------------------------------------------------------------
// Embedded Control Panel applet logos. The Windows 7 logo is the user supplied
// image with the edge-connected white background removed. Both ICO payloads are
// multi-size and were resampled bicubically, so the shell can pick a matching
// size instead of scaling a single bitmap. The Windows 8.1 ICO is generated from
// the already embedded Windows 8.1 status PNG.
// -----------------------------------------------------------------------------
static const wchar_t* kAppletLogoWin7FileName = L"wuapplet-win7.ico";
static const wchar_t* kAppletLogoWin81FileName = L"wuapplet-win81.ico";
static const wchar_t* kAppletTasksXmlFileName = L"wuapplet-tasks-v2.xml";
static const char kAppletLogoWin7IcoBase64[] =
    "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"
    "v0+Ae1XW/weVWA8bq2S85gAAAABJRU5ErkJggg==";
static const char kAppletLogoWin81IcoBase64[] =
    "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"
    "FKGUT5AAAAAASUVORK5CYII=";


// -----------------------------------------------------------------------------
// Embedded "service unavailable" shield (user supplied image, white background
// removed -> transparent). Shown when the Windows Update service is disabled or
// missing, so the user still gets a friendly, translated notice instead of the
// native (often cryptic) red "automatic updates are off" box. Stored as a raw
// PNG so GDI+ can decode it and scale it with HighQualityBicubic at render time.
static const UINT kWuDisabledShieldIconId = 61004;
static const char kWuDisabledShieldPngBase64[] =
    "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"
    "vt36/wHH7B4fpaoEyQAAAABJRU5ErkJggg=="
    "";


static int Base64Digit(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}
static std::vector<BYTE> DecodeBase64Icon(const char* source) {
    std::vector<BYTE> out; int value = 0, bits = -8;
    for (const char* p = source; p && *p; ++p) { if (*p == '=') break; int d = Base64Digit(*p); if (d < 0) continue; value = (value << 6) + d; bits += 6; if (bits >= 0) { out.push_back(static_cast<BYTE>((value >> bits) & 0xFF)); bits -= 8; } }
    return out;
}

// Base64 encoder (used to feed an extracted PNG back into the GDI+ bicubic
// path, which consumes base64).
static std::string Base64Encode(const BYTE* data, size_t len) {
    static const char kAlphabet[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    out.reserve(((len + 2) / 3) * 4);
    size_t i = 0;
    while (i + 2 < len) {
        const DWORD v = (static_cast<DWORD>(data[i]) << 16) |
                        (static_cast<DWORD>(data[i + 1]) << 8) |
                        static_cast<DWORD>(data[i + 2]);
        out += kAlphabet[(v >> 18) & 63];
        out += kAlphabet[(v >> 12) & 63];
        out += kAlphabet[(v >> 6) & 63];
        out += kAlphabet[v & 63];
        i += 3;
    }
    if (i < len) {
        const DWORD v = static_cast<DWORD>(data[i]) << 16 |
                        (i + 1 < len ? static_cast<DWORD>(data[i + 1]) << 8 : 0);
        out += kAlphabet[(v >> 18) & 63];
        out += kAlphabet[(v >> 12) & 63];
        out += (i + 1 < len) ? kAlphabet[(v >> 6) & 63] : '=';
        out += '=';
    }
    return out;
}
#pragma pack(push, 1)
struct EmbeddedIconHeader { WORD reserved, type, count; };
struct EmbeddedIconEntry { BYTE width, height, colors, reserved; WORD planes, bitCount; DWORD bytes, offset; };
#pragma pack(pop)
static std::mutex g_statusIconMutex;
static std::unordered_map<ULONGLONG, HICON> g_statusIconCache;

// -----------------------------------------------------------------------------
// GDI+ HighQualityBicubic icon rendering (mirrors win7-network-flyout-recreation)
// -----------------------------------------------------------------------------
// Instead of loading the PNG as a fixed-size icon and letting DirectUI upscale it
// (which blurs it), we decode the PNG with GDI+ and scale it to the requested size
// using InterpolationModeHighQualityBicubic (mode 7). This gives a crisp icon at
// any DPI. We resolve gdiplus.dll once and reuse the pointers/startup token.
static HMODULE g_hGdiPlus = NULL;
static ULONG_PTR g_gdiplusToken = 0;
static std::mutex g_gdiPlusMutex;
typedef int (WINAPI *GdipCreateBitmapFromStreamFunc)(IStream*, void**);
typedef int (WINAPI *GdipCreateBitmapFromScan0Func)(int, int, int, int, const void*, void**);
typedef int (WINAPI *GdipGetImageGraphicsContextFunc)(void*, void**);
typedef int (WINAPI *GdipSetInterpolationModeFunc)(void*, int);
typedef int (WINAPI *GdipSetPixelOffsetModeFunc)(void*, int);
typedef int (WINAPI *GdipGraphicsClearFunc)(void*, unsigned int);
typedef int (WINAPI *GdipDrawImageRectIFunc)(void*, void*, int, int, int, int);
typedef int (WINAPI *GdipCreateHICONFromBitmapFunc)(void*, HICON*);
typedef int (WINAPI *GdipDeleteGraphicsFunc)(void*);
typedef int (WINAPI *GdipDisposeImageFunc)(void*);
static GdipCreateBitmapFromStreamFunc pGdipCreateBitmapFromStream = NULL;
static GdipCreateBitmapFromScan0Func pGdipCreateBitmapFromScan0 = NULL;
static GdipGetImageGraphicsContextFunc pGdipGetImageGraphicsContext = NULL;
static GdipSetInterpolationModeFunc pGdipSetInterpolationMode = NULL;
static GdipSetPixelOffsetModeFunc pGdipSetPixelOffsetMode = NULL;
static GdipGraphicsClearFunc pGdipGraphicsClear = NULL;
static GdipDrawImageRectIFunc pGdipDrawImageRectI = NULL;
static GdipCreateHICONFromBitmapFunc pGdipCreateHICONFromBitmap = NULL;
static GdipDeleteGraphicsFunc pGdipDeleteGraphics = NULL;
static GdipDisposeImageFunc pGdipDisposeImage = NULL;

static BOOL InitGdiPlusRendering() {
    std::lock_guard<std::mutex> lock(g_gdiPlusMutex);
    if (g_hGdiPlus) return TRUE;
    g_hGdiPlus = LoadLibraryExW(L"gdiplus.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_hGdiPlus) { Wh_Log(L"Windows Update Restorer: GDI+ failed to load"); return FALSE; }
    pGdipCreateBitmapFromStream = (GdipCreateBitmapFromStreamFunc)GetProcAddress(g_hGdiPlus, "GdipCreateBitmapFromStream");
    pGdipCreateBitmapFromScan0 = (GdipCreateBitmapFromScan0Func)GetProcAddress(g_hGdiPlus, "GdipCreateBitmapFromScan0");
    pGdipGetImageGraphicsContext = (GdipGetImageGraphicsContextFunc)GetProcAddress(g_hGdiPlus, "GdipGetImageGraphicsContext");
    pGdipSetInterpolationMode = (GdipSetInterpolationModeFunc)GetProcAddress(g_hGdiPlus, "GdipSetInterpolationMode");
    pGdipSetPixelOffsetMode = (GdipSetPixelOffsetModeFunc)GetProcAddress(g_hGdiPlus, "GdipSetPixelOffsetMode");
    pGdipGraphicsClear = (GdipGraphicsClearFunc)GetProcAddress(g_hGdiPlus, "GdipGraphicsClear");
    pGdipDrawImageRectI = (GdipDrawImageRectIFunc)GetProcAddress(g_hGdiPlus, "GdipDrawImageRectI");
    pGdipCreateHICONFromBitmap = (GdipCreateHICONFromBitmapFunc)GetProcAddress(g_hGdiPlus, "GdipCreateHICONFromBitmap");
    pGdipDeleteGraphics = (GdipDeleteGraphicsFunc)GetProcAddress(g_hGdiPlus, "GdipDeleteGraphics");
    pGdipDisposeImage = (GdipDisposeImageFunc)GetProcAddress(g_hGdiPlus, "GdipDisposeImage");
    if (!pGdipCreateBitmapFromStream || !pGdipCreateBitmapFromScan0 || !pGdipGetImageGraphicsContext ||
        !pGdipSetInterpolationMode || !pGdipSetPixelOffsetMode || !pGdipGraphicsClear ||
        !pGdipDrawImageRectI || !pGdipCreateHICONFromBitmap || !pGdipDeleteGraphics || !pGdipDisposeImage) {
        Wh_Log(L"Windows Update Restorer: GDI+ missing function pointers");
        FreeLibrary(g_hGdiPlus); g_hGdiPlus = NULL; return FALSE;
    }
    typedef int (WINAPI *GdiplusStartupFunc)(ULONG_PTR*, const void*, void*);
    GdiplusStartupFunc pStartup = (GdiplusStartupFunc)GetProcAddress(g_hGdiPlus, "GdiplusStartup");
    if (!pStartup) { FreeLibrary(g_hGdiPlus); g_hGdiPlus = NULL; return FALSE; }
    struct { DWORD Version; void* Callback; BOOL Suppress; } si = {1, NULL, FALSE};
    if (pStartup(&g_gdiplusToken, &si, NULL) != 0) {
        FreeLibrary(g_hGdiPlus); g_hGdiPlus = NULL; return FALSE;
    }
    Wh_Log(L"Windows Update Restorer: GDI+ initialized for bicubic icons");
    return TRUE;
}

static void ShutdownGdiPlusRendering() {
    std::lock_guard<std::mutex> lock(g_gdiPlusMutex);
    if (g_hGdiPlus) {
        typedef void (WINAPI *GdiplusShutdownFunc)(ULONG_PTR);
        GdiplusShutdownFunc pShutdown = (GdiplusShutdownFunc)GetProcAddress(g_hGdiPlus, "GdiplusShutdown");
        if (pShutdown && g_gdiplusToken) pShutdown(g_gdiplusToken);
        FreeLibrary(g_hGdiPlus);
        g_hGdiPlus = NULL; g_gdiplusToken = 0;
    }
}

// Decodes a base64 PNG and returns an HICON scaled to targetWidth/Height using
// GDI+ HighQualityBicubic interpolation. Returns NULL on any failure.
static HICON CreateIconFromBase64PngBicubic(const char* base64Str, int targetWidth, int targetHeight) {
    if (!InitGdiPlusRendering() || !pGdipCreateBitmapFromStream || !pGdipCreateHICONFromBitmap)
        return NULL;

    std::vector<BYTE> data = DecodeBase64Icon(base64Str);
    if (data.empty()) return NULL;

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, data.size());
    if (!hMem) return NULL;
    void* pMem = GlobalLock(hMem);
    if (!pMem) { GlobalFree(hMem); return NULL; }
    memcpy(pMem, data.data(), data.size());
    GlobalUnlock(hMem);
    IStream* stream = NULL;
    CreateStreamOnHGlobal(hMem, TRUE, &stream);
    if (!stream) { GlobalFree(hMem); return NULL; }

    HICON hIcon = NULL;
    void* srcBitmap = NULL;
    if (pGdipCreateBitmapFromStream(stream, &srcBitmap) == 0 && srcBitmap) {
        bool scaled = false;
        if (targetWidth > 0 && targetHeight > 0 && pGdipCreateBitmapFromScan0 &&
            pGdipGetImageGraphicsContext && pGdipSetInterpolationMode &&
            pGdipSetPixelOffsetMode && pGdipGraphicsClear && pGdipDrawImageRectI &&
            pGdipDeleteGraphics) {
            void* dstBitmap = NULL;
            if (pGdipCreateBitmapFromScan0(targetWidth, targetHeight, 0, 0x00E200B, NULL, &dstBitmap) == 0 && dstBitmap) {
                void* graphics = NULL;
                if (pGdipGetImageGraphicsContext(dstBitmap, &graphics) == 0 && graphics) {
                    pGdipSetInterpolationMode(graphics, 7);   // HighQualityBicubic
                    pGdipSetPixelOffsetMode(graphics, 3);     // HalfPixel
                    pGdipGraphicsClear(graphics, 0);
                    scaled = pGdipDrawImageRectI(graphics, srcBitmap, 0, 0, targetWidth, targetHeight) == 0;
                    pGdipDeleteGraphics(graphics);
                }
                if (scaled) pGdipCreateHICONFromBitmap(dstBitmap, &hIcon);
                pGdipDisposeImage(dstBitmap);
            }
        }
        if (!scaled) pGdipCreateHICONFromBitmap(srcBitmap, &hIcon);
        pGdipDisposeImage(srcBitmap);
    }
    stream->Release();
    return hIcon;
}

// Defined below (in the icon-file helper section); declared here so the
// "Checking for updates..." state can load the skinned applet logo.
static std::wstring EnsureAppletLogoIconFile(bool windows81Skin);

// Renders the skinned applet logo (Windows 7 or Windows 8.1) at the requested
// size using GDI+ HighQualityBicubic interpolation. The generated .ico stores
// PNG-compressed entries; the largest entry is extracted, base64-encoded and
// fed through the same bicubic path used by the status icons, so the FAQ
// window's header icon stays crisp at any DPI instead of being downscaled by
// GDI (which blurs it). Falls back to a plain LoadImageW of the .ico, then to
// the embedded status PNG, on any failure.
static HICON CreateAppletLogoIconBicubic(bool windows81Skin, int width, int height) {
    if (width <= 0 || height <= 0)
        width = height = GetSystemMetrics(SM_CXICON);

    const std::wstring path = EnsureAppletLogoIconFile(windows81Skin);
    std::vector<BYTE> ico;
    if (!path.empty()) {
        HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
                                  nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                                  nullptr);
        if (file != INVALID_HANDLE_VALUE) {
            LARGE_INTEGER size{};
            if (GetFileSizeEx(file, &size) && size.QuadPart > 0 &&
                size.QuadPart < 4 * 1024 * 1024) {
                ico.resize(static_cast<size_t>(size.QuadPart));
                DWORD read = 0;
                if (!ReadFile(file, ico.data(), static_cast<DWORD>(ico.size()),
                              &read, nullptr) ||
                    read != ico.size())
                    ico.clear();
            }
            CloseHandle(file);
        }
    }

    if (ico.size() >= 6) {
        const WORD count = static_cast<WORD>(ico[4]) |
                           (static_cast<WORD>(ico[5]) << 8);
        // Find the entry with the largest effective size (0 in the header
        // means 256). Downscaling a larger source with bicubic looks best.
        size_t bestOffset = 0;
        size_t bestBytes = 0;
        int bestSize = -1;
        for (int i = 0; i < count && i < 64; ++i) {
            const size_t base = 6 + static_cast<size_t>(i) * 16;
            if (base + 16 > ico.size()) break;
            const BYTE w = ico[base];
            const int eff = w == 0 ? 256 : static_cast<int>(w);
            const DWORD bytes = static_cast<DWORD>(ico[base + 8]) |
                                (static_cast<DWORD>(ico[base + 9]) << 8) |
                                (static_cast<DWORD>(ico[base + 10]) << 16) |
                                (static_cast<DWORD>(ico[base + 11]) << 24);
            const DWORD offset = static_cast<DWORD>(ico[base + 12]) |
                                 (static_cast<DWORD>(ico[base + 13]) << 8) |
                                 (static_cast<DWORD>(ico[base + 14]) << 16) |
                                 (static_cast<DWORD>(ico[base + 15]) << 24);
            if (eff > bestSize) {
                bestSize = eff;
                bestOffset = offset;
                bestBytes = bytes;
            }
        }
        // PNG-compressed entry: extract it and scale with GDI+ bicubic.
        if (bestBytes >= 8 && bestOffset + bestBytes <= ico.size() &&
            ico[bestOffset] == 0x89 && ico[bestOffset + 1] == 'P' &&
            ico[bestOffset + 2] == 'N' && ico[bestOffset + 3] == 'G') {
            const std::string b64 = Base64Encode(
                ico.data() + bestOffset, bestBytes);
            if (HICON icon = CreateIconFromBase64PngBicubic(b64.c_str(), width, height))
                return icon;
        }
    }

    // Fallback: plain .ico load, then the embedded status PNG.
    if (!path.empty()) {
        if (HICON icon = reinterpret_cast<HICON>(LoadImageW(
                nullptr, path.c_str(), IMAGE_ICON, width, height,
                LR_LOADFROMFILE | LR_DEFAULTCOLOR)))
            return icon;
    }
    const char* png = windows81Skin ? kWindows81UpdateStatusPngBase64
                                    : kUpdatesInstalledPngBase64;
    return png ? CreateIconFromBase64PngBicubic(png, width, height) : nullptr;
}

static HICON GetStatusIcon(UINT id, int requestedWidth, int requestedHeight) {
    const int width = requestedWidth > 0 ? requestedWidth : GetSystemMetrics(SM_CXICON);
    const int height = requestedHeight > 0 ? requestedHeight : GetSystemMetrics(SM_CYICON);
    const ULONGLONG cacheKey = (static_cast<ULONGLONG>(id) << 48) |
                               (static_cast<ULONGLONG>(width & 0xFFFFFF) << 24) |
                               static_cast<ULONGLONG>(height & 0xFFFFFF);

    std::lock_guard<std::mutex> lock(g_statusIconMutex);
    if (auto it = g_statusIconCache.find(cacheKey); it != g_statusIconCache.end())
        return it->second;

    HICON icon = nullptr;
    if (id == kLegacyWarningShieldIconId) {
        std::vector<BYTE> ico = DecodeBase64Icon(kLegacyWarningShieldIcoBase64);
        if (ico.size() >= sizeof(EmbeddedIconHeader) + sizeof(EmbeddedIconEntry)) {
            const auto* header = reinterpret_cast<const EmbeddedIconHeader*>(ico.data());
            const auto* entry = reinterpret_cast<const EmbeddedIconEntry*>(
                ico.data() + sizeof(EmbeddedIconHeader));
            if (header->type == 1 && header->count && entry->offset <= ico.size() &&
                entry->bytes <= ico.size() - entry->offset) {
                icon = CreateIconFromResourceEx(ico.data() + entry->offset, entry->bytes,
                                                TRUE, 0x00030000, width, height,
                                                LR_DEFAULTCOLOR);
            }
        }
    } else {
        const char* png = id == kUpdatesInstalledIconId
                              ? kUpdatesInstalledPngBase64
                              : id == kWindows81UpdateStatusIconId
                                    ? kWindows81UpdateStatusPngBase64
                                    : id == kWuDisabledShieldIconId
                                          ? kWuDisabledShieldPngBase64
                                          : nullptr;
        if (png) icon = CreateIconFromBase64PngBicubic(png, width, height);
    }
    if (icon) g_statusIconCache.emplace(cacheKey, icon);
    return icon;
}

using LoadImageW_t = decltype(&LoadImageW);
static LoadImageW_t LoadImageWOriginalForLegacyWarningIcon = nullptr;
static HANDLE WINAPI LoadImageWHookForLegacyWarningIcon(HINSTANCE instance, LPCWSTR name, UINT type, int cx, int cy, UINT flags) {
    // Only substitute our private status icons when the request actually targets
    // the shell32.dll resource library, which is how our DirectUI XML references
    // them (icon(... library(shell32.dll))). This keeps other modules in
    // explorer.exe that happen to use one of the same numeric IDs unaffected.
    if (instance != GetModuleHandleW(L"shell32.dll")) return LoadImageWOriginalForLegacyWarningIcon(instance, name, type, cx, cy, flags);
    if (type == IMAGE_ICON && IS_INTRESOURCE(name)) {
        const UINT id = static_cast<UINT>(reinterpret_cast<UINT_PTR>(name));
        if (id == kLegacyWarningShieldIconId || id == kUpdatesInstalledIconId ||
            id == kWindows81UpdateStatusIconId || id == kWuDisabledShieldIconId) {
            if (HICON icon = GetStatusIcon(id, cx, cy)) return CopyIcon(icon);
        }
    }
    return LoadImageWOriginalForLegacyWarningIcon(instance, name, type, cx, cy, flags);
}
static void InstallLegacyWarningIconHook() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) user32 = LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32) if (void* p = reinterpret_cast<void*>(GetProcAddress(user32, "LoadImageW")))
        WindhawkUtils::SetFunctionHook(reinterpret_cast<LoadImageW_t>(p), LoadImageWHookForLegacyWarningIcon, &LoadImageWOriginalForLegacyWarningIcon);
}

// -----------------------------------------------------------------------------
// DirectUI XML patch.  The legacy page is still rendered by wucltux.dll, but
// its original commands are no longer reliable on modern Windows.  We add a
// small, in-page command group whose NavigateButton routes through our private
// ShellExecute command protocol below.  Nothing is written to the DLL on disk.
// -----------------------------------------------------------------------------
#ifdef _WIN64
#define WU_DUI_THISCALL __cdecl
#else
#define WU_DUI_THISCALL __thiscall
#endif


using DUISetXML_t = HRESULT(WU_DUI_THISCALL*)(void*, const WCHAR*, HINSTANCE, HINSTANCE);
using DUISetXMLFromResource_t = HRESULT(WU_DUI_THISCALL*)(
    void*, PCWSTR, PCWSTR, HMODULE, HINSTANCE, HINSTANCE);
static DUISetXML_t DUISetXMLOriginal = nullptr;
static DUISetXMLFromResource_t DUISetXMLFromResourceOriginal = nullptr;
static thread_local int g_inWuXmlPatch = 0;

// Conservative RAII guard for g_inWuXmlPatch: increments on construction and
// guarantees the decrement on destruction, so the re-entrancy guard is released
// even if the body returns early or throws. Prevents double-patching.
class WuXmlPatchGuard {
public:
    WuXmlPatchGuard() { ++g_inWuXmlPatch; }
    WuXmlPatchGuard(const WuXmlPatchGuard&) = delete;
    WuXmlPatchGuard& operator=(const WuXmlPatchGuard&) = delete;
    ~WuXmlPatchGuard() { --g_inWuXmlPatch; }
};

static std::wstring LoadDirectUiResourceXml(HMODULE module, PCWSTR name, PCWSTR type) {
    if (!module || !name || !type) return {};
    HRSRC resource = FindResourceW(module, name, type);
    if (!resource) return {};
    HGLOBAL loaded = LoadResource(module, resource);
    const DWORD bytes = loaded ? SizeofResource(module, resource) : 0;
    const char* data = loaded ? static_cast<const char*>(LockResource(loaded)) : nullptr;
    if (!data || !bytes) return {};
    int chars = MultiByteToWideChar(CP_UTF8, 0, data, static_cast<int>(bytes), nullptr, 0);
    UINT cp = CP_UTF8;
    if (chars <= 0) { cp = CP_ACP; chars = MultiByteToWideChar(cp, 0, data, static_cast<int>(bytes), nullptr, 0); }
    if (chars <= 0) return {};
    std::wstring xml(static_cast<size_t>(chars), L'\0');
    MultiByteToWideChar(cp, 0, data, static_cast<int>(bytes), &xml[0], chars);
    while (!xml.empty() && (xml.back() == L'\0' || xml.back() == L'\r' || xml.back() == L'\n')) xml.pop_back();
    return xml;
}

// -----------------------------------------------------------------------------
// Multilingual service message (10 languages + English fallback)
// -----------------------------------------------------------------------------
// The currently selected language (a Windhawk setting). The default "auto"
// detects the system language automatically; any specific code overrides it.
// English is always the fallback for any unknown code. Declared near the top of
// the file (see g_language above), where the embedded string table uses it.

// Maps a Windows LANGID (its primary language) to one of the supported codes.
// Returns L"" when the language is not one of the ten supported ones.
static std::wstring LanguageCodeFromLangId(USHORT langId) {
    switch (PRIMARYLANGID(langId)) {
        case LANG_ITALIAN: return L"it";
        case LANG_SPANISH: return L"es";
        case LANG_FRENCH: return L"fr";
        case LANG_TURKISH: return L"tr";
        case LANG_RUSSIAN: return L"ru";
        case LANG_PORTUGUESE: return L"pt";
        case LANG_CHINESE: return L"zh";
        case LANG_POLISH: return L"pl";
        case LANG_DUTCH: return L"nl";
        case LANG_ENGLISH: return L"en";
        default: return L"";
    }
}

// Detects the user's UI language (falls back to the system default UI language,
// then to the default locale). Returns a supported code, or L"" if none matches.
static std::wstring DetectSystemLanguage() {
    std::wstring code = LanguageCodeFromLangId(GetUserDefaultUILanguage());
    if (!code.empty()) return code;
    code = LanguageCodeFromLangId(GetSystemDefaultUILanguage());
    if (!code.empty()) return code;
    code = LanguageCodeFromLangId(GetUserDefaultLCID());
    return code;
}

// Loads the message language from the mod settings. The "auto" value (or an
// empty one) triggers automatic system-language detection. English is the final
// fallback if nothing matches.
// Removes task-XML files left by builds that predate native ControlPanelNavLinks.
static void CleanupControlPanelTasksXmlFile();

static void LogCurrentSettings() {
    Wh_Log(L"WUR: SETTINGS: Language=%s Skin=%s ShowServiceNotice=%d ShowAvailable=%d LinkSettings=%d RemoveLegacy=%d NativeNavLinks=1",
        LanguageCode(),
        IsWindows81Skin() ? L"81" : L"7",
        (int)g_showServiceNotice.load(),
        (int)g_showAvailableUpdates.load(),
        (int)g_linkSystemSettingsText.load(),
        (int)g_removeLegacyBrokenOption.load()
    );
    // Remove a stale file left by an older build. This version does not publish
    // System.Software.TasksFileUrl: it uses the page's native per-layout links.
    CleanupControlPanelTasksXmlFile();
}

static void LoadLanguageSetting() {
    auto languageSetting = WindhawkUtils::StringSetting::make(L"Language");
    PCWSTR lang = languageSetting.get();
    std::wstring value = *lang ? lang : L"auto";
    for (auto& c : value) c = towlower(c);
    if (value == L"auto" || value.empty()) {
        std::wstring detected = DetectSystemLanguage();
        g_language.store(LanguageFromCode(detected.empty() ? L"en" : detected), std::memory_order_release);
    } else {
        g_language.store(LanguageFromCode(value), std::memory_order_release);
    }

    auto skinSetting = WindhawkUtils::StringSetting::make(L"UpdatePageSkin");
    PCWSTR skin = skinSetting.get();
    std::wstring skinValue = *skin ? skin : L"windows7";
    for (auto& c : skinValue) c = towlower(c);
    g_updatePageSkin.store(
        (skinValue == L"windows81" || skinValue == L"windows8.1")
            ? kUpdatePageSkinWindows81
            : kUpdatePageSkinWindows7);

    // Whether the "service not available" shield notice is shown.
    g_showServiceNotice.store(Wh_GetIntSetting(L"ShowServiceNotice") != 0);
    g_showAvailableUpdates.store(Wh_GetIntSetting(L"ShowAvailableUpdates") != 0);
    g_linkSystemSettingsText.store(Wh_GetIntSetting(L"LinkSystemSettingsText") != 0);
    g_removeLegacyBrokenOption.store(Wh_GetIntSetting(L"RemoveLegacyBrokenOption") != 0);
    // One sidebar only: leave the Windows-owned pane visible and don't publish
    // the obsolete TasksFileUrl/private-DirectUI alternatives.
    LogCurrentSettings();
}

// Escapes XML special characters so a translation string is safe to embed in the
// DirectUI document.
static std::wstring XmlEscape(const wchar_t* s) {
    std::wstring out;
    if (!s) return out;
    for (const wchar_t* p = s; *p; ++p) {
        switch (*p) {
            case L'&': out += L"&amp;"; break;
            case L'<': out += L"&lt;"; break;
            case L'>': out += L"&gt;"; break;
            case L'"': out += L"&quot;"; break;
            default: out += *p; break;
        }
    }
    return out;
}


// Returns the three pieces of string 324 ("It is recommended to use the system
// settings to configure updates.") so the middle phrase can become a blue link
// without changing the translated words. The suffix is rendered as its own row
// to avoid DirectUI wrapping only the final word in narrow Control Panel windows.
struct SettingsRecommendationLinkParts {
    const wchar_t* before;
    const wchar_t* link;
    const wchar_t* after;
};

static SettingsRecommendationLinkParts SelectSettingsRecommendationLinkParts() {
    static const std::unordered_map<std::wstring, SettingsRecommendationLinkParts> kParts = {
        { L"en", { L"It is recommended to use the ", L"system settings", L"to configure updates." } },
        { L"it", { L"Si consiglia di utilizzare le ", L"impostazioni del sistema", L"per configurare gli aggiornamenti." } },
        { L"es", { L"Se recomienda usar la ", L"configuración del sistema", L"para configurar las actualizaciones." } },
        { L"fr", { L"Il est recommandé d'utiliser les ", L"paramètres du système", L"pour configurer les mises à jour." } },
        { L"tr", { L"Güncellemeleri yapılandırmak için ", L"sistem ayarlarını", L"kullanmanız önerilir." } },
        { L"ru", { L"Рекомендуется использовать ", L"параметры системы", L"для настройки обновлений." } },
        { L"pt", { L"Recomenda-se usar as ", L"configurações do sistema", L"para configurar atualizações." } },
        { L"zh", { L"建议使用", L"系统设置", L"来配置更新。" } },
        { L"pl", { L"Zaleca się korzystanie z ", L"ustawień systemowych", L"w celu skonfigurowania aktualizacji." } },
        { L"nl", { L"Het wordt aanbevolen om de ", L"systeeminstellingen", L"te gebruiken om updates te configureren." } },
    };

    auto it = kParts.find(CurrentLanguage());
    if (it == kParts.end()) it = kParts.find(L"en");
    return it->second;
}

static std::wstring BuildPlainStatusDescriptionXml(const wchar_t* desc) {
    return L"<element sheet=\"wuappstyle\" class=\"cp_content_text\" contentalign=\"wrapleft\" content=\""
        + XmlEscape(desc ? desc : L"") +
        L"\"/>";
}

static std::wstring BuildLinkedSettingsRecommendationXml() {
    const SettingsRecommendationLinkParts parts = SelectSettingsRecommendationLinkParts();
    std::wstring xml =
        L"<element layout=\"flowlayout(1)\" contentalign=\"wrapleft\">"
        L"<element layout=\"flowlayout(0,0,0,2)\" contentalign=\"wrapleft\">";
    if (parts.before && *parts.before) {
        xml +=
            L"<element sheet=\"wuappstyle\" class=\"cp_content_text\" content=\""
            + XmlEscape(parts.before) +
            L"\"/>";
    }
    xml +=
        L"<NavigateButton layout=\"flowlayout()\" shellexecute=\"ms-settings:windowsupdate\">"
        L"<Button sheet=\"wu_cp_style\" class=\"cp_content_link\" active=\"mouse | keyboard\" content=\""
        + XmlEscape(parts.link ? parts.link : L"system settings") +
        L"\"/></NavigateButton></element>";
    if (parts.after && *parts.after) {
        xml +=
            L"<element sheet=\"wuappstyle\" class=\"cp_content_text\" contentalign=\"wrapleft\" margin=\"rect(0,1rp,0,0)\" content=\""
            + XmlEscape(parts.after) +
            L"\"/>";
    }
    xml += L"</element>";
    return xml;
}


static const wchar_t* SelectChangeWindowsUpdateSettingsLinkText() {
    static const std::unordered_map<std::wstring, const wchar_t*> kTexts = {
        { L"en", L"Change Windows Update settings" },
        { L"it", L"Cambia impostazioni di Windows Update" },
        { L"es", L"Cambiar la configuración de Windows Update" },
        { L"fr", L"Modifier les paramètres de Windows Update" },
        { L"tr", L"Windows Update ayarlarını değiştir" },
        { L"ru", L"Изменить параметры Центра обновления Windows" },
        { L"pt", L"Alterar configurações do Windows Update" },
        { L"zh", L"更改 Windows 更新设置" },
        { L"pl", L"Zmień ustawienia Windows Update" },
        { L"nl", L"Windows Update-instellingen wijzigen" },
    };
    auto it = kTexts.find(CurrentLanguage());
    if (it == kTexts.end()) it = kTexts.find(L"en");
    return it->second;
}

static std::wstring BuildWindowsUpdateSettingsPageParams() {
    return L"shell:::" + std::wstring(kAppletClsid) + L"\\pageSettings";
}

static std::wstring BuildChangeWindowsUpdateSettingsLinkXml() {
    return
        L"<NavigateButton layout=\"flowlayout()\" margin=\"rect(0,5rp,0,0)\" "
        L"shellexecute=\"%SystemRoot%\\explorer.exe\" shellexecuteparams=\"" +
        BuildWindowsUpdateSettingsPageParams() +
        L"\">"
        L"<Button sheet=\"wu_cp_style\" class=\"cp_content_link\" active=\"mouse | keyboard\" content=\"" +
        XmlEscape(SelectChangeWindowsUpdateSettingsLinkText()) +
        L"\"/></NavigateButton>";
}

// -----------------------------------------------------------------------------
// Blue link to the Windows Update settings child page ("Open Windows Update
// settings"). Shown only when the recreated hub cannot be built (Windows Update
// service unavailable / AU not configured) and only the red warning box remains
// on the page, so the user still gets a one-click path to the classic settings
// page (shell:::{36EEF7DB-88AD-4E81-AD49-0E313F0C35F8}\\pageSettings).
//
// Additionally, there is this explorer shell:::{1138506a_b949_46a7_b6c0_ee26499fdeaf} which I don't know what was used for
// The link is NOT attached to the native moduleAUNotConfigured element: wucltux
// re-shows/re-sizes that module at runtime and overrides any XML added inside
// it (and re-appends it, pushing siblings below it). Instead, when only the red
// box would be shown, the native module is collapsed to a zero-size element and
// BuildRedBoxFallbackXml() renders a faithful recreation of the red box with
// this link directly below it - a self-contained module wucltux does not touch.
// -----------------------------------------------------------------------------
static const wchar_t* SelectOpenWindowsUpdateSettingsLinkText() {
    static const std::unordered_map<std::wstring, const wchar_t*> kTexts = {
        { L"en", L"Open Windows Update settings" },
        { L"it", L"Apri Impostazioni di Windows Update" },
        { L"es", L"Abrir la configuración de Windows Update" },
        { L"fr", L"Ouvrir les paramètres de Windows Update" },
        { L"tr", L"Windows Update ayarlarını aç" },
        { L"ru", L"Открыть параметры Центра обновления Windows" },
        { L"pt", L"Abrir as configurações do Windows Update" },
        { L"zh", L"打开 Windows 更新设置" },
        { L"pl", L"Otwórz ustawienia Windows Update" },
        { L"nl", L"Windows Update-instellingen openen" },
    };
    auto it = kTexts.find(CurrentLanguage());
    if (it == kTexts.end()) it = kTexts.find(L"en");
    return it->second;
}

// Forward declaration: defined below (used by BuildRedBoxFallbackXml).
static const wchar_t* WuOptionText(DWORD opt) {
    switch (opt) {
        case 4: return EmbeddedMuiString(334);  // Install updates automatically (recommended)
        case 3: return EmbeddedMuiString(335);  // Download updates but let me choose...
        case 2: return EmbeddedMuiString(336);  // Check for updates but let me choose...
        case 1: return EmbeddedMuiString(337);  // Never check for updates (not recommended)
        default: return EmbeddedMuiString(334);
    }
}

// Replaced with native ComboBox

static std::wstring BuildOpenWindowsUpdateSettingsLinkXml() {
    // Opens the classic settings child page via the shell URI directly
    // (shell:::{36EEF7DB-...}\\pageSettings). We deliberately do NOT launch
    // "%SystemRoot%\explorer.exe" with shellexecuteparams: that spawns a NEW
    // explorer.exe process (which reloads Windhawk mods and then hands the
    // command over), which is unstable inside the shell and crashes explorer.
    // A bare shell: URI is dispatched by the existing shell instance - same
    // mechanism as ms-settings: and https:// links used elsewhere in the mod.
    return
        L"<element layoutpos=\"top\" layout=\"flowlayout()\" margin=\"rect(12rp,8rp,12rp,0)\">"
        L"<NavigateButton layout=\"flowlayout()\" shellexecute=\"shell:::"
        + std::wstring(kAppletClsid) + L"\\pageSettings\">"
        L"<Button sheet=\"wu_cp_style\" class=\"cp_content_link\" active=\"mouse | keyboard\" content=\""
        + XmlEscape(SelectOpenWindowsUpdateSettingsLinkText()) +
        L"\"/></NavigateButton></element>";
}

// -----------------------------------------------------------------------------
// Faithful recreation of the native red "Automatic updates are not configured"
// box (moduleAUNotConfigured, UIFILE 123) with the blue settings link below it.
// Used when Windows Update is unavailable / AU is not configured: the native
// module is collapsed to a zero-size element (atom stays resolvable -> no
// S_FALSE, no provider fallback re-materialization, as proven by the hub path)
// and this self-contained module is rendered instead. The inner atoms
// (actionTurnOnAU, actionAdvancedAUSettings, ...) are kept identical so
// wucltux's code-behind still finds them and the "Turn on automatic updating"
// button keeps working; the module root gets a unique atom wucltux ignores, so
// nothing is moved, resized or overridden at runtime and the link stays put.
//
// includeLink controls whether the blue "Open Windows Update settings" link is
// appended below the box: it is shown with "Show recreated interface" ON and
// omitted with it OFF. The box itself is always rendered - even with the
// recreated interface disabled, the user must still get the "Turn on automatic
// updating" box (the native one is re-shown/overridden by the provider and
// unreliable on modern builds).
// -----------------------------------------------------------------------------
static std::wstring BuildRedBoxFallbackXml(bool includeLink = true) {
    std::wstring xml =
        L"<element id=\"atom(wuamodern_redbox_fallback)\" sheet=\"wuappstyle\" layoutpos=\"top\" "
        L"layout=\"borderlayout()\" margin=\"rect(0,12rp,0,12rp)\">"
        // --- the red box itself (native moduleborder1 structure) ---
        L"<element class=\"moduleborder1\" layoutpos=\"top\" layout=\"borderlayout()\">"
        L"<element id=\"atom(areaModuleColorBox)\" layoutpos=\"left\" layout=\"borderlayout()\" "
        L"sheet=\"wuappstyle\" class=\"security_box_gradient_red\"/>"
        L"<element id=\"atom(areaModuleIcon)\" layoutpos=\"left\" layout=\"borderlayout()\" "
        L"padding=\"rect(12rp,12rp,4rp,0)\" contentalign=\"topleft\">"
        L"<element layoutpos=\"top\" layout=\"borderlayout()\" contentalign=\"topleft\">"
        L"<viewer class=\"wuapp_module_img\">"
        L"<element id=\"atom(elementRedModuleIcon)\" class=\"wuapp_module_img\" layoutpos=\"top\" "
        L"content=\"icon(105,48rp,48rp,library(imageres.dll))\"/>"
        L"</viewer></element></element>"
        L"<element layoutpos=\"top\" layout=\"flowlayout(1,0,0,2)\" padding=\"rect(0,12rp,12rp,4rp)\">"
        L"<element class=\"wuapp_module_instruction\" content=\"resstr(1149)\"/>"
        L"</element>"
        L"<element layoutpos=\"top\" layout=\"flowlayout(1,0,0,2)\" padding=\"rect(2rp,0,48rp,0)\">"
        L"<element class=\"wuapp_content_title\" content=\"resstr(1153)\"/>"
        L"</element>"
        L"<element layoutpos=\"top\" layout=\"flowlayout(1,0,0,2)\" padding=\"rect(2rp,12rp,48rp,0)\">"
        L"<element sheet=\"wu_cp_style\" class=\"cp_content_text\" content=\"resstr(1154)\"/>"
        L"</element>"
        L"<element layoutpos=\"top\" layout=\"borderlayout()\" padding=\"rect(0,7rp,18rp,18rp)\">"
        L"<element layoutpos=\"right\" layout=\"borderlayout()\" padding=\"rect(0,0,0,0)\">"
        L"<element layoutpos=\"top\" layout=\"flowlayout(0,0,2,2)\" padding=\"rect(0,0,0,0)\">"
        L"<viewer>"
        L"<CCPushButton id=\"atom(actionTurnOnAU)\" layoutpos=\"right\" active=\"mouse | keyboard\" "
        L"sheet=\"wu_cp_style\" shortcut=\"auto\" content=\"resstr(1150)\"/>"
        L"</viewer></element>"
        L"<element id=\"atom(actionAdvancedAUSettingsArea)\" layoutpos=\"top\" layout=\"flowlayout(0,0,2,2)\" "
        L"padding=\"rect(0,2rp,0,0)\">"
        L"<NavigateButton id=\"atom(actionAdvancedAUSettings)\" layoutpos=\"top\" layout=\"flowlayout()\" "
        L"padding=\"rect(0,0,0,0)\" navigationtargetroot=\"\" navigationtargetrelative=\"pageSettings\">"
        L"<Button id=\"atom(actionAdvancedAUSettingsText)\" sheet=\"wu_cp_style\" class=\"cp_content_link\" "
        L"active=\"mouse | keyboard\" content=\"resstr(1254)\"/>"
        L"</NavigateButton></element>"
        L"</element></element></element>";
    // --- our blue link, directly below the red box (only with recreated UI on) ---
    // Opens the settings page, which now shows the classic option list and the
    // blue "change update frequency" link that opens the Win32 dialog.
    if (includeLink) xml += BuildOpenWindowsUpdateSettingsLinkXml();
    xml += L"</element>";
    return xml;
}

static bool IsWindowsUpdatePageXml(const std::wstring& xml) {
    // The start/status and automatic-update pages do not all share one action
    // name. These are stable string/action references in the Win 8.1 wucltux
    // XMLFILE resource and cover both the normal landing page and the
    // "Turn on automatic updating" page shown on modern Windows.
    return xml.find(L"actionCheckForUpdates") != std::wstring::npos ||
           xml.find(L"actionViewInstalledUpdates") != std::wstring::npos ||
           xml.find(L"resstr(1100)") != std::wstring::npos ||
           xml.find(L"resstr(1149)") != std::wstring::npos ||
           xml.find(L"resstr(1150)") != std::wstring::npos ||
           xml.find(L"resstr(1153)") != std::wstring::npos;
}


static bool FindElementEnd(const std::wstring& xml, size_t start, size_t& end) {
    int depth = 0;
    for (size_t pos = start; pos < xml.size();) {
        if (xml.compare(pos, 8, L"<element") == 0) {
            size_t gt = xml.find(L'>', pos); if (gt == std::wstring::npos) return false;
            if (gt == pos || xml[gt - 1] != L'/') ++depth;
            pos = gt + 1;
        } else if (xml.compare(pos, 10, L"</element>") == 0) {
            if (--depth == 0) { end = pos + 10; return true; }
            pos += 10;
        } else ++pos;
    }
    return false;
}

// -----------------------------------------------------------------------------
// "Important updates" selector on the classic settings page (pageSettings).
//
// On modern Windows the legacy combobox (atom(auOptionSelectorCombobox)) is
// populated by wucltux code-behind that no longer works (the Windows Update
// service is stopped/broken), so the "Important updates" section shows an
// empty box. The atom cannot be removed (the page would fail to load), so we
// collapse it to a zero-size element (atom stays resolvable, same trick as the
// hub modules) and render our own list of the four classic options instead,
// reading the current AUOptions value from the registry. The selector is
// intentionally read-only; modern Windows Settings owns policy changes.
//
// AUOptions values (Windows Update Agent / classic Control Panel):
//   4 = install updates automatically (recommended)          -> resstr(334)
//   3 = download updates but let me choose whether to install -> resstr(335)
//   2 = check for updates but let me choose download/install  -> resstr(336)
//   1 = never check for updates (not recommended)             -> resstr(337)
// -----------------------------------------------------------------------------
static const wchar_t* kWuRestorerProtocol = L"wurestorer:";

static DWORD ReadAuOptionsValue() {
    // Windows 10/11 honour the Group-Policy key first; read it when present so
    // the shown selection reflects the effective state.
    HKEY hPolicy = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate\\AU",
            0, KEY_READ, &hPolicy) == ERROR_SUCCESS) {
        DWORD noAuto = 0, size = sizeof(noAuto);
        if (RegQueryValueExW(hPolicy, L"NoAutoUpdate", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(&noAuto), &size) == ERROR_SUCCESS && noAuto != 0) {
            RegCloseKey(hPolicy);
            return 1; // never check
        }
        RegCloseKey(hPolicy);
    }

    DWORD auOptions = 4; // default: recommended
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD noAuto = 0, size = sizeof(noAuto);
        if (RegQueryValueExW(hKey, L"NoAutoUpdate", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(&noAuto), &size) == ERROR_SUCCESS && noAuto != 0) {
            auOptions = 1; // never check
        } else {
            size = sizeof(auOptions);
            if (RegQueryValueExW(hKey, L"AUOptions", nullptr, nullptr,
                                 reinterpret_cast<LPBYTE>(&auOptions), &size) != ERROR_SUCCESS ||
                auOptions < 1 || auOptions > 4) {
                auOptions = 4;
            }
        }
        RegCloseKey(hKey);
    }
    return auOptions;
}

static const wchar_t* SelectReadOnlySettingsNote() {
    static const std::unordered_map<std::wstring, const wchar_t*> kTexts = {
        { L"en", L"These settings are read-only here. Use Windows Settings to make changes." },
        { L"it", L"Queste impostazioni sono di sola lettura. Usa Impostazioni di Windows per modificarle." },
        { L"es", L"Esta configuración es de solo lectura. Use Configuración de Windows para cambiarla." },
        { L"fr", L"Ces paramètres sont en lecture seule. Utilisez les Paramètres Windows pour les modifier." },
        { L"tr", L"Bu ayarlar burada salt okunurdur. Değişiklik yapmak için Windows Ayarları'nı kullanın." },
        { L"ru", L"Здесь эти параметры доступны только для чтения. Изменяйте их в Параметрах Windows." },
        { L"pt", L"Estas configurações são somente leitura. Use as Configurações do Windows para alterá-las." },
        { L"zh", L"这些设置在此处为只读。请使用 Windows 设置进行更改。" },
        { L"pl", L"Te ustawienia są tutaj tylko do odczytu. Zmień je w Ustawieniach systemu Windows." },
        { L"nl", L"Deze instellingen zijn hier alleen-lezen. Wijzig ze via Windows-instellingen." },
    };
    auto it = kTexts.find(CurrentLanguage());
    if (it == kTexts.end()) it = kTexts.find(L"en");
    return it->second;
}

static const wchar_t* SelectUpdateIntroText() {
    static const std::unordered_map<std::wstring, const wchar_t*> kTexts = {
        { L"en", L"Windows Update keeps your PC secure and reliable by installing the latest updates." },
        { L"it", L"Windows Update mantiene il PC sicuro e affidabile installando gli aggiornamenti più recenti." },
        { L"es", L"Windows Update mantiene su PC seguro y fiable instalando las actualizaciones más recientes." },
        { L"fr", L"Windows Update maintient votre PC sûr et fiable en installant les dernières mises à jour." },
        { L"tr", L"Windows Update, bilgisayarınızı en son güncellemeleri yükleyerek güvenli ve güvenilir tutar." },
        { L"ru", L"Центр обновления Windows поддерживает компьютер в безопасности и стабильной работе, устанавливая последние обновления." },
        { L"pt", L"O Windows Update mantém seu PC seguro e confiável instalando as atualizações mais recentes." },
        { L"zh", L"Windows 更新通过安装最新更新，让您的电脑保持安全和稳定。" },
        { L"pl", L"Windows Update utrzymuje komputer bezpieczny i niezawodny, instalując najnowsze aktualizacje." },
        { L"nl", L"Windows Update houdt uw pc veilig en betrouwbaar door de nieuwste updates te installeren." },
    };
    auto it = kTexts.find(CurrentLanguage());
    if (it == kTexts.end()) it = kTexts.find(L"en");
    return it->second;
}

// Renders the introduction above the native ComboBox. UIFILE 125 places the
// combobox inline in the icon flow row. Keep that structure, widen the control
// for translated options, and add the explanatory text as a separate top row.
static std::wstring BuildUpdateIntroTextXml() {
    return
        L"<element layoutpos=\"top\" layout=\"flowlayout(0,0,0,2)\" margin=\"rect(0,0,0,6rp)\">"
        L"<element id=\"atom(auUpdateIntroText)\" sheet=\"wuappstyle\" class=\"cp_content_text\" content=\""
        + XmlEscape(SelectUpdateIntroText()) +
        L"\"/><element layoutpos=\"top\" sheet=\"wuappstyle\" class=\"cp_content_text\" "
        L"content=\"" + XmlEscape(SelectReadOnlySettingsNote()) +
        L"\"/></element>";
}


// =============================================================================
// Native Win32 Drop-down ComboBox for Settings Page ("pageSettings")
// =============================================================================
// The native combobox is per-window state: each Explorer window runs its own
// UI thread, so the DirectUI parent handle and the population flags must be
// thread-local. A shared plain global made two windows with the settings page
// open fight over the same flags (one combobox staying empty, or re-population
// every 200 ms).
static thread_local HWND g_hwndDirectUiParent = nullptr;
// thread_local, not a shared atomic: this describes whether *this* Explorer
// window's settings page is active. DUISetXMLHook/DUISetXMLFromResourceHook
// fire for every DirectUI document parsed anywhere in explorer.exe, so a
// shared flag meant any other shell window's non-WU page would unconditionally
// switch off the combobox repair for the settings page open on a different
// thread (see DestroySettingsComboboxImpl's g_isSettingsPageActive reset).
static thread_local bool g_isSettingsPageActive = false;

static LRESULT CALLBACK SettingsDirectUiSubclassProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData);

// Subclass APIs and timers are thread-affine. This message is synchronously
// delivered by the owner thread before an any-thread subclass removal.
static const UINT g_wmUiTeardown = RegisterWindowMessageW(L"Windhawk.WUControlPanelRestorer.UiTeardown");
static std::mutex g_subclassWindowsMutex;
static std::vector<HWND> g_settingsSubclassWindows;

// =============================================================================
// NATIVE DIRECTUI COMBOBOX REPAIR (No Win32 Overlay)
// =============================================================================

typedef ATOM (WINAPI *DirectUI_StrToID_t)(const wchar_t* str);
typedef void* (*DirectUI_FindDescendent_t)(void* element, ATOM atom);
typedef int (*DirectUI_Combobox_AddString_t)(void* combobox, const wchar_t* str);
typedef HRESULT (*DirectUI_Combobox_SetSelection_t)(void* combobox, int index);
typedef HRESULT (*DirectUI_Element_SetEnabled_t)(void* element, bool enabled);

static DirectUI_StrToID_t pStrToID = nullptr;
static DirectUI_FindDescendent_t pFindDescendent = nullptr;
static DirectUI_Combobox_AddString_t pAddString = nullptr;
static DirectUI_Combobox_SetSelection_t pSetSelection = nullptr;
static DirectUI_Element_SetEnabled_t pSetEnabled = nullptr;

static thread_local bool g_nativeComboPopulated = false;
static thread_local void* g_lastComboPtr = nullptr;

// Resolve only the DirectUI exports required to populate the read-only selector.
static void EnsureDui70ComboboxExports() {
    if (pStrToID) return;
    HMODULE dui70 = GetModuleHandleW(L"dui70.dll");
    if (!dui70) dui70 = LoadLibraryExW(L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!dui70) return;
    pStrToID = reinterpret_cast<DirectUI_StrToID_t>(GetProcAddress(dui70, "StrToID"));
    pFindDescendent = reinterpret_cast<DirectUI_FindDescendent_t>(
        GetProcAddress(dui70, "?FindDescendent@Element@DirectUI@@QEAAPEAV12@G@Z"));
    pAddString = reinterpret_cast<DirectUI_Combobox_AddString_t>(
        GetProcAddress(dui70, "?AddString@Combobox@DirectUI@@QEAAHPEBG@Z"));
    pSetSelection = reinterpret_cast<DirectUI_Combobox_SetSelection_t>(
        GetProcAddress(dui70, "?SetSelection@Combobox@DirectUI@@QEAAJH@Z"));
    pSetEnabled = reinterpret_cast<DirectUI_Element_SetEnabled_t>(
        GetProcAddress(dui70, "?SetEnabled@Element@DirectUI@@QEAAJ_N@Z"));
}

static void InitDirectUIExports() {
    EnsureDui70ComboboxExports();
}

// Tears down the native combobox subclasses. The subclass and the timer are
// thread-affine, so a window belonging to another thread must never be torn
// down from here unless the whole mod is unloading. With currentThreadOnly set,
// only windows owned by the calling thread are touched (used by the DirectUI
// XML patch path, which runs for EVERY DirectUI document parsed in explorer.exe
// - including folder windows and shell dialogs on other threads).
static void DestroySettingsComboboxImpl(bool currentThreadOnly) {
    g_isSettingsPageActive = false;
    std::vector<HWND> windows;
    { std::lock_guard lock(g_subclassWindowsMutex); windows = g_settingsSubclassWindows; }
    for (HWND hwnd : windows) if (IsWindow(hwnd)) {
        if (currentThreadOnly &&
            GetWindowThreadProcessId(hwnd, nullptr) != GetCurrentThreadId())
            continue;
        // Ask the owner thread to stop its timer, then remove the wrapper with
        // Windhawk's cross-thread-safe helper.
        SendMessageW(hwnd, g_wmUiTeardown, 0, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hwnd, SettingsDirectUiSubclassProc);
    }
    if (!currentThreadOnly) {
        std::lock_guard lock(g_subclassWindowsMutex);
        g_settingsSubclassWindows.clear();
    }
    // Reset only this thread's state. Other threads reset their own copies in
    // SettingsDirectUiSubclassProc when their window is torn down.
    g_hwndDirectUiParent = nullptr;
    g_nativeComboPopulated = false;
    g_lastComboPtr = nullptr;
}

// Full teardown (Wh_ModUninit): every window, regardless of owning thread.
static void DestroySettingsCombobox() {
    DestroySettingsComboboxImpl(false);
}

// Teardown restricted to the calling thread (DirectUI page-load path).
static void DestroySettingsComboboxOnThisThread() {
    DestroySettingsComboboxImpl(true);
}

static LRESULT CALLBACK SettingsDirectUiSubclassProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == g_wmUiTeardown || uMsg == WM_NCDESTROY) {
        KillTimer(hwnd, 889);
        {
            std::lock_guard lock(g_subclassWindowsMutex);
            auto it = std::remove(g_settingsSubclassWindows.begin(),
                                  g_settingsSubclassWindows.end(), hwnd);
            g_settingsSubclassWindows.erase(it, g_settingsSubclassWindows.end());
        }
        if (g_hwndDirectUiParent == hwnd) g_hwndDirectUiParent = nullptr;
        g_nativeComboPopulated = false;
        g_lastComboPtr = nullptr;
        // Runs on this window's own owning thread (SendMessageW above is
        // synchronous cross-thread), so this is the thread-local copy for
        // that thread - matches the "reset only this thread's state" contract.
        g_isSettingsPageActive = false;
        if (uMsg == g_wmUiTeardown) return 0;
    }

    if (g_isSettingsPageActive && uMsg == WM_TIMER && wParam == 889) {
        InitDirectUIExports();
        if (pStrToID && pFindDescendent && pAddString && pSetSelection) {
            void* root = reinterpret_cast<void*>(GetWindowLongPtrW(hwnd, 0));
            ATOM atom = pStrToID(L"auOptionSelectorCombobox");
            void* combo = root && atom ? pFindDescendent(root, atom) : nullptr;
            if (combo) {
                if (g_lastComboPtr != combo) {
                    g_lastComboPtr = combo;
                    g_nativeComboPopulated = false;
                }
                if (!g_nativeComboPopulated) {
                    // AddString returns the actual item index. The legacy page can
                    // already contain four empty resource-backed entries, so selecting
                    // hard-coded indexes 0..3 leaves the combobox visibly blank even
                    // though the translated fallback strings were appended correctly.
                    const int optionIndices[] = {
                        pAddString(combo, WuOptionText(4)),
                        pAddString(combo, WuOptionText(3)),
                        pAddString(combo, WuOptionText(2)),
                        pAddString(combo, WuOptionText(1)),
                    };
                    if (optionIndices[0] >= 0 && optionIndices[1] >= 0 &&
                        optionIndices[2] >= 0 && optionIndices[3] >= 0) {
                        const DWORD current = ReadAuOptionsValue();
                        const int optionSlot = current == 4 ? 0 : current == 3 ? 1
                                                       : current == 2 ? 2 : 3;
                        if (SUCCEEDED(pSetSelection(combo, optionIndices[optionSlot]))) {
                            g_nativeComboPopulated = true;
                        }
                    }
                }
                // The legacy page is presentational only on modern Windows.
                if (pSetEnabled) pSetEnabled(combo, false);
            }
        }
    }
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

struct EnumSettingsDirectUiCtx {
    HWND verified = nullptr;
};

static bool IsSettingsDirectUiWindow(HWND hwnd) {
    if (!IsWindow(hwnd) ||
        GetWindowThreadProcessId(hwnd, nullptr) != GetCurrentThreadId()) {
        return false;
    }
    wchar_t cls[64] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (_wcsicmp(cls, L"DirectUIHWND") != 0) return false;

    RECT rc{};
    GetClientRect(hwnd, &rc);
    if (rc.right - rc.left <= 260 || rc.bottom - rc.top <= 150) return false;

    EnsureDui70ComboboxExports();
    if (!pStrToID || !pFindDescendent) return false;
    void* root = reinterpret_cast<void*>(GetWindowLongPtrW(hwnd, 0));
    ATOM atom = pStrToID(L"auOptionSelectorCombobox");
    return root && atom && pFindDescendent(root, atom);
}

static BOOL CALLBACK EnumSettingsDirectUiProc(HWND hwnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<EnumSettingsDirectUiCtx*>(lParam);
    if (IsSettingsDirectUiWindow(hwnd)) {
        ctx->verified = hwnd;
        return FALSE;
    }
    return TRUE;
}

static HWND FindSettingsDirectUiHwnd() {
    EnumSettingsDirectUiCtx ctx;
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND top, LPARAM lParam) -> BOOL {
            auto* ctx = reinterpret_cast<EnumSettingsDirectUiCtx*>(lParam);
            if (IsSettingsDirectUiWindow(top)) {
                ctx->verified = top;
                return FALSE;
            }
            EnumChildWindows(top, EnumSettingsDirectUiProc, lParam);
            return ctx->verified ? FALSE : TRUE;
        },
        reinterpret_cast<LPARAM>(&ctx));
    return ctx.verified;
}

static void InitializeNativeSettingsCombobox(HWND hwndParent) {
    g_isSettingsPageActive = true;
    if (!hwndParent || !IsSettingsDirectUiWindow(hwndParent))
        hwndParent = FindSettingsDirectUiHwnd();
    if (!hwndParent) return;

    g_hwndDirectUiParent = hwndParent;
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(
            hwndParent, SettingsDirectUiSubclassProc, 0)) {
        {
            std::lock_guard lock(g_subclassWindowsMutex);
            if (std::find(g_settingsSubclassWindows.begin(),
                          g_settingsSubclassWindows.end(), hwndParent) ==
                g_settingsSubclassWindows.end()) {
                g_settingsSubclassWindows.push_back(hwndParent);
            }
        }
        SetTimer(hwndParent, 889, 200, nullptr);
    }
}

static std::wstring PatchSettingsPageXml(const std::wstring& input) {
    InitializeNativeSettingsCombobox(nullptr);

    std::wstring out = input;
    const size_t cbAtom = out.find(L"atom(auOptionSelectorCombobox)");
    if (cbAtom == std::wstring::npos) return input;

    // Expand the native combobox while preserving the original XML structure
    size_t tagStart = out.rfind(L"<combobox", cbAtom);
    if (tagStart == std::wstring::npos) tagStart = out.rfind(L"<ComboBox", cbAtom);
    if (tagStart == std::wstring::npos) tagStart = out.rfind(L"<COMBOBOX", cbAtom);
    if (tagStart != std::wstring::npos) {
        size_t widthPos = out.find(L"width=\"10rp\"", tagStart);
        if (widthPos != std::wstring::npos && widthPos < cbAtom) {
            out.replace(widthPos, 12, L"width=\"285rp\"");
        }
    }

    // Insert the intro text above the icon/combobox flow row as a separate top row
    // Keep the inline placeholder in its original flow row.
    // Find the flow row containing the icons and placeholder.
    const size_t placeholderPos = out.find(L"atom(auOptionSelectorPlaceholder)");
    const size_t rowStart = out.rfind(
        L"<element layoutpos=\"top\" layout=\"flowlayout(0,0,0,2)\" margin=\"rect(0,14rp,0,0)\">", placeholderPos != std::wstring::npos ? placeholderPos : cbAtom);
    if (rowStart != std::wstring::npos) {
        out.insert(rowStart, BuildUpdateIntroTextXml());
    }

    // Hide the red warning shield: blank the icon element INSIDE the
    // atom(auOptionSelectorWarningIcon) viewer (zero-size, empty content).
    const size_t warnAtom = out.find(L"atom(auOptionSelectorWarningIcon)");
    if (warnAtom != std::wstring::npos) {
        const size_t iconPos = out.find(L"content=\"icon(105,", warnAtom);
        if (iconPos != std::wstring::npos) {
            const size_t tagStart = out.rfind(L"<element", iconPos);
            const size_t tagEnd = out.find(L"/>", tagStart);
            if (tagStart != std::wstring::npos && tagEnd != std::wstring::npos) {
                const std::wstring hidden =
                    L"<element sheet=\"wuappstyle\" class=\"aupsp_left_img\" width=\"0rp\" height=\"0rp\" content=\"\"/>";
                out.replace(tagStart, tagEnd + 2 - tagStart, hidden);
            }
        }
    }
    return out;
}

// "Choose how to install updates" - group box title of the classic dialog.
static const wchar_t* SelectChooseHowToInstallUpdatesLabel() {
    static const std::unordered_map<std::wstring, const wchar_t*> kTexts = {
        { L"en", L"Choose how to install updates" },
        { L"it", L"Scegli come installare gli aggiornamenti" },
        { L"es", L"Elija cómo instalar las actualizaciones" },
        { L"fr", L"Choisissez comment installer les mises à jour" },
        { L"tr", L"Güncellemelerin nasıl yükleneceğini seçin" },
        { L"ru", L"Выберите способ установки обновлений" },
        { L"pt", L"Escolha como instalar as atualizações" },
        { L"zh", L"选择如何安装更新" },
        { L"pl", L"Wybierz sposób instalowania aktualizacji" },
        { L"nl", L"Kies hoe updates worden geïnstalleerd" },
    };
    auto it = kTexts.find(CurrentLanguage());
    if (it == kTexts.end()) it = kTexts.find(L"en");
    return it->second;
}

// Private command protocol used only by this applet's navigation links. Known
// commands are consumed and return 33 (ShellExecute success is any value > 32);
// unknown commands report failure with hInstApp set.
static void ShowWuSettingsDialog(HWND parent);
static void ShowWuFaqDialog(HWND parent);
static void StartWuUpdateCheck(HWND host);
static HICON LoadAppletLogoIconForShell(int size);
using ShellExecuteExW_t = BOOL(WINAPI*)(SHELLEXECUTEINFOW*);
static ShellExecuteExW_t ShellExecuteExWOriginal = nullptr;
static BOOL WINAPI ShellExecuteExWHook(SHELLEXECUTEINFOW* info) {
    // Once teardown has begun, stop handling our private protocol: the handlers
    // create windows/dialogs whose procedures live in this image.
    if (!g_stopping.load() && info && info->lpFile &&
        wcsncmp(info->lpFile, kWuRestorerProtocol, wcslen(kWuRestorerProtocol)) == 0) {
        const wchar_t* p = info->lpFile + wcslen(kWuRestorerProtocol);
        if (wcscmp(p, L"opensettings") == 0) {
            // Open the classic settings dialog as an ADDITIONAL window on top
            // of the settings page (which stays open - we do not navigate away).
            ShowWuSettingsDialog(info->hwnd);
            info->hInstApp = reinterpret_cast<HINSTANCE>(static_cast<UINT_PTR>(33));
            return TRUE;
        } else if (wcscmp(p, L"check") == 0) {
            // "Check for updates" micro-feature (see StartWuUpdateCheck): opens
            // a small Win32 dialog with a native progress bar that runs the
            // ~12-second check; the page itself is left untouched.
            StartWuUpdateCheck(info->hwnd);
            info->hInstApp = reinterpret_cast<HINSTANCE>(static_cast<UINT_PTR>(33));
            return TRUE;
        } else if (wcscmp(p, L"faq") == 0) {
            ShowWuFaqDialog(info->hwnd);
            info->hInstApp = reinterpret_cast<HINSTANCE>(static_cast<UINT_PTR>(33));
            return TRUE;
        } else if (wcscmp(p, L"history") == 0) {
            OpenInstalledUpdates(info->hwnd, InstalledUpdatesDestination::History);
            info->hInstApp = reinterpret_cast<HINSTANCE>(static_cast<UINT_PTR>(33));
            return TRUE;
        } else if (wcscmp(p, L"hidden") == 0) {
            OpenInstalledUpdates(info->hwnd, InstalledUpdatesDestination::HiddenUpdates);
            info->hInstApp = reinterpret_cast<HINSTANCE>(static_cast<UINT_PTR>(33));
            return TRUE;
        } else if (wcscmp(p, L"installed") == 0) {
            OpenInstalledUpdates(info->hwnd, InstalledUpdatesDestination::UninstallUpdates);
            info->hInstApp = reinterpret_cast<HINSTANCE>(static_cast<UINT_PTR>(33));
            return TRUE;
        } else if (wcscmp(p, L"security") == 0) {
            OpenSecurityAndMaintenance(info->hwnd);
            info->hInstApp = reinterpret_cast<HINSTANCE>(static_cast<UINT_PTR>(33));
            return TRUE;
        }
        info->hInstApp = reinterpret_cast<HINSTANCE>(SE_ERR_FNF);
        SetLastError(ERROR_FILE_NOT_FOUND);
        return FALSE;
    }
    return ShellExecuteExWOriginal(info);
}

using ShellExecuteW_t = HINSTANCE(WINAPI*)(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, INT);
static ShellExecuteW_t ShellExecuteWOriginal = nullptr;
static HINSTANCE WINAPI ShellExecuteWHook(HWND hwnd, LPCWSTR operation, LPCWSTR file,
                                          LPCWSTR parameters, LPCWSTR directory, INT show) {
    if (!g_stopping.load() && file &&
        wcsncmp(file, kWuRestorerProtocol, wcslen(kWuRestorerProtocol)) == 0) {
        const wchar_t* p = file + wcslen(kWuRestorerProtocol);
        bool handled = true;
        if (wcscmp(p, L"opensettings") == 0) {
            ShowWuSettingsDialog(hwnd);
        } else if (wcscmp(p, L"check") == 0) {
            StartWuUpdateCheck(hwnd);
        } else if (wcscmp(p, L"faq") == 0) {
            ShowWuFaqDialog(hwnd);
        } else if (wcscmp(p, L"history") == 0) {
            OpenInstalledUpdates(hwnd, InstalledUpdatesDestination::History);
        } else if (wcscmp(p, L"hidden") == 0) {
            OpenInstalledUpdates(hwnd, InstalledUpdatesDestination::HiddenUpdates);
        } else if (wcscmp(p, L"installed") == 0) {
            OpenInstalledUpdates(hwnd, InstalledUpdatesDestination::UninstallUpdates);
        } else if (wcscmp(p, L"security") == 0) {
            OpenSecurityAndMaintenance(hwnd);
        } else {
            handled = false;
        }
        if (handled) return reinterpret_cast<HINSTANCE>(static_cast<UINT_PTR>(33));
    }
    return ShellExecuteWOriginal(hwnd, operation, file, parameters, directory, show);
}

// -----------------------------------------------------------------------------
// Native Control Panel navigation links
// -----------------------------------------------------------------------------
// wucltux publishes a private CControlPanelNavLinks object by writing it to the
// page's SID_PerLayoutPropertyBag under "ControlPanelNavLinks". Windows then
// builds the standard blue Control Panel sidebar from that object. Earlier
// versions of this mod tried to hide that pane and injected a second DirectUI
// copy; current Windows builds can recreate the native pane after the XML pass,
// which leaves two sidebars.
//
// Keep the Windows-owned pane instead. Immediately before PROPSYS stores the
// object, redirect the four normal WU task links to the safe handlers already
// implemented by this mod. This follows the same mechanism as the classic
// CControlPanelNavLinks::AddLinkNotify/AddLinkShellEx sample, but it patches the
// real wucltux object in place. Therefore its vtable and lifetime remain owned by
// wucltux -- important when a Windhawk mod is disabled while Explorer stays up.
//
// These layouts are private Windows ABI. They are intentionally limited to the
// one pinned x64 wucltux.dll payload used by this mod. The offsets match that
// payload and the reverse-engineered CControlPanelNavLinks implementation.
struct NativeControlPanelNavLinkCommand {
    int execType;                   // 0=none/notify, 1=ShellExecute, 2=navigate
    DWORD reserved;
    PWSTR appletOrCommand;
    PWSTR arguments;
};

struct NativeControlPanelNavLink {
    int list;                       // 0=Tasks, 1=See also, 2=other
    DWORD reserved;
    PWSTR name;
    PWSTR auxiliaryArguments;
    HICON icon;
    NativeControlPanelNavLinkCommand command;
    BYTE privateState[0x33];        // tail used by the pinned private ABI
};

struct NativeControlPanelNavLinks {
    void* vtable;
    HDPA links;
    LONG referenceCount;
};

static_assert(sizeof(void*) == 8, "The pinned wucltux navigation ABI is x64-only");
static_assert(offsetof(NativeControlPanelNavLink, command) == 32,
              "Unexpected CControlPanelNavLink layout");
static_assert(sizeof(NativeControlPanelNavLink) == 0x70,
              "Unexpected CControlPanelNavLink size");
static_assert(offsetof(NativeControlPanelNavLinks, links) == 8,
              "Unexpected CControlPanelNavLinks layout");

static std::wstring NormalizeNativeNavLabel(PCWSTR text) {
    std::wstring normalized;
    if (!text) return normalized;
    for (; *text; ++text) {
        if (*text == L'&') continue;  // keyboard accelerator marker
        normalized += towlower(*text);
    }
    return normalized;
}

static bool NativeNavLabelEquals(PCWSTR actual, UINT resourceId) {
    const wchar_t* expected = EmbeddedMuiString(resourceId);
    return expected && NormalizeNativeNavLabel(actual) ==
                           NormalizeNativeNavLabel(expected);
}

static HRESULT DuplicateNativeNavString(PCWSTR source, PWSTR* destination) {
    if (!source || !destination) return E_INVALIDARG;
    *destination = nullptr;
    const size_t chars = wcslen(source) + 1;
    if (chars > SIZE_MAX / sizeof(wchar_t)) return E_OUTOFMEMORY;
    auto* copy = static_cast<PWSTR>(
        CoTaskMemAlloc(chars * sizeof(wchar_t)));
    if (!copy) return E_OUTOFMEMORY;
    memcpy(copy, source, chars * sizeof(wchar_t));
    *destination = copy;
    return S_OK;
}

static HRESULT RedirectNativeNavLink(NativeControlPanelNavLink* link,
                                     PCWSTR command) {
    if (!link || !command || !*command) return E_INVALIDARG;

    // PSPropertyBag_WriteUnknown can be reached more than once for one page.
    // Don't allocate a second copy if this link was already redirected.
    if (link->command.execType == 1 && link->command.appletOrCommand &&
        _wcsicmp(link->command.appletOrCommand, command) == 0) {
        return S_FALSE;
    }

    PWSTR commandCopy = nullptr;
    HRESULT hr = DuplicateNativeNavString(command, &commandCopy);
    if (FAILED(hr)) return hr;
    PWSTR argumentsCopy = nullptr;
    hr = DuplicateNativeNavString(L"", &argumentsCopy);
    if (FAILED(hr)) {
        CoTaskMemFree(commandCopy);
        return hr;
    }

    // Do not free the previous union fields: AddLinkNotify stores notification
    // data there rather than two allocated strings. Once execType is changed to
    // ShellExecute, wucltux's own link destructor owns these CoTaskMem buffers.
    link->command.execType = 1;  // CPNAVTYPE_ShellExec
    link->command.reserved = 0;
    link->command.appletOrCommand = commandCopy;
    link->command.arguments = argumentsCopy;
    return S_OK;
}

// Appends the FAQ task link to a wucltux-owned sidebar. Defined below (after
// CreateNativeNavLink); declared here so the redirect can call it.
static void AppendFaqNavLinkIfMissing(NativeControlPanelNavLinks* navLinks);

static unsigned RedirectNativeControlPanelNavLinks(IUnknown* unknown) {
    if (!unknown) return 0;
    auto* navLinks = reinterpret_cast<NativeControlPanelNavLinks*>(unknown);
    if (!navLinks->links) return 0;

    const int count = DPA_GetPtrCount(navLinks->links);
    // A genuine WU navigation list is tiny. Refuse an implausible object rather
    // than walking memory if a future payload changes this private layout.
    if (count <= 0 || count > 32) {
        Wh_Log(L"WUR: native ControlPanelNavLinks rejected (count=%d)", count);
        return 0;
    }

    static constexpr PCWSTR kFallbackCommands[] = {
        L"wurestorer:check",
        L"shell:::{36EEF7DB-88AD-4E81-AD49-0E313F0C35F8}\\pageSettings",
        L"wurestorer:history",
        L"wurestorer:hidden",
    };

    unsigned patched = 0;
    unsigned taskOrdinal = 0;
    for (int index = 0; index < count; ++index) {
        auto* link = reinterpret_cast<NativeControlPanelNavLink*>(
            DPA_GetPtr(navLinks->links, index));
        if (!link) continue;

        PCWSTR target = nullptr;
        if (link->list == 0) {
            // Prefer the translated label, since WU can omit one task in some
            // states. The ordinal fallback covers older resource variants.
            if (NativeNavLabelEquals(link->name, 350)) {
                target = kFallbackCommands[0];       // Check for updates
            } else if (NativeNavLabelEquals(link->name, 351) ||
                       NativeNavLabelEquals(link->name, 73)) {
                target = kFallbackCommands[1];       // Classic WU settings page
            } else if (NativeNavLabelEquals(link->name, 352) ||
                       NativeNavLabelEquals(link->name, 74)) {
                target = kFallbackCommands[2];       // View update history
            } else if (NativeNavLabelEquals(link->name, 353) ||
                       NativeNavLabelEquals(link->name, 75)) {
                target = kFallbackCommands[3];       // Installed Updates
            } else if (taskOrdinal < ARRAYSIZE(kFallbackCommands)) {
                target = kFallbackCommands[taskOrdinal];
            }
            ++taskOrdinal;
        } else if (link->list == 1) {
            if (NativeNavLabelEquals(link->name, 355)) {
                target = L"wurestorer:security";
            } else if (NativeNavLabelEquals(link->name, 356) ||
                       NativeNavLabelEquals(link->name, 20004)) {
                target = L"wurestorer:installed";
            }
        }

        if (target) {
            const HRESULT hr = RedirectNativeNavLink(link, target);
            if (hr == S_OK) ++patched;
        }
    }

    // Append the mod's own "Updates: frequently asked questions" link below
    // "Restore hidden updates" when wucltux built the task list itself (the
    // rebuilt list already contains it, see CreateNativeControlPanelNavLinks).
    AppendFaqNavLinkIfMissing(navLinks);
    return patched;
}

// Builds a complete native navigation object when Windows 10 21H2 doesn't call
// wucltux's own PopulateControlPanelNavLinks routine. The native vtable RVA is
// from the exact SHA-256-pinned wucltux.dll payload above. A deliberately high
// reference count keeps Windows from invoking a cross-CRT deleting destructor;
// the object is only a few kilobytes and lives until this host process exits.
static constexpr ULONG_PTR kControlPanelNavLinksVtableRva = 0x2350;
static constexpr LONG kPinnedNavLinksReferenceCount = 0x10000000;

// The fabricated CControlPanelNavLinks object and its links are tiny (24 bytes
// and 0x70 bytes). VirtualAlloc would reserve a full 64 KB region per object
// (~448 KB per page navigation); CoTaskMemAlloc is the right tool for objects
// this size and is what the rest of the private ABI already frees with.
static int CALLBACK DestroyUnpublishedNativeNavLink(void* item, void*) {
    auto* link = static_cast<NativeControlPanelNavLink*>(item);
    if (!link) return 1;
    CoTaskMemFree(link->name);
    CoTaskMemFree(link->auxiliaryArguments);
    CoTaskMemFree(link->command.appletOrCommand);
    CoTaskMemFree(link->command.arguments);
    CoTaskMemFree(link);
    return 1;
}

static NativeControlPanelNavLink* CreateNativeNavLink(
    int list, PCWSTR name, PCWSTR command, PCWSTR arguments = L"") {
    auto* link = static_cast<NativeControlPanelNavLink*>(
        CoTaskMemAlloc(sizeof(NativeControlPanelNavLink)));
    if (!link) return nullptr;
    memset(link, 0, sizeof(*link));

    link->list = list;
    link->command.execType = 1;  // CPNAVTYPE_ShellExec
    HRESULT hr = DuplicateNativeNavString(name ? name : L"", &link->name);
    if (SUCCEEDED(hr))
        hr = DuplicateNativeNavString(command ? command : L"",
                                      &link->command.appletOrCommand);
    if (SUCCEEDED(hr))
        hr = DuplicateNativeNavString(arguments ? arguments : L"",
                                      &link->command.arguments);
    if (FAILED(hr)) {
        DestroyUnpublishedNativeNavLink(link, nullptr);
        return nullptr;
    }
    return link;
}

static void DestroyUnpublishedNativeNavLinks(NativeControlPanelNavLinks* links) {
    if (!links) return;
    if (links->links) {
        DPA_DestroyCallback(links->links,
                            reinterpret_cast<PFNDAENUMCALLBACK>(
                                DestroyUnpublishedNativeNavLink),
                            nullptr);
    }
    CoTaskMemFree(links);
}

static void AppendFaqNavLinkIfMissing(NativeControlPanelNavLinks* navLinks) {
    if (!navLinks || !navLinks->links) return;

    // The same list can be published more than once; never insert a duplicate.
    const int count = DPA_GetPtrCount(navLinks->links);
    for (int index = 0; index < count; ++index) {
        auto* link = reinterpret_cast<NativeControlPanelNavLink*>(
            DPA_GetPtr(navLinks->links, index));
        if (link && link->command.execType == 1 && link->command.appletOrCommand &&
            _wcsicmp(link->command.appletOrCommand, L"wurestorer:faq") == 0)
            return;
    }

    const wchar_t* label = EmbeddedMuiString(20024);
    auto* faqLink = CreateNativeNavLink(
        0, label ? label : L"Updates: frequently asked questions",
        L"wurestorer:faq", L"");
    if (faqLink && DPA_InsertPtr(navLinks->links, 0x7fffffff, faqLink) == -1)
        DestroyUnpublishedNativeNavLink(faqLink, nullptr);
}

static NativeControlPanelNavLinks* CreateNativeControlPanelNavLinks() {
    HMODULE module = g_module.load(std::memory_order_acquire);
    if (!module) return nullptr;

    auto* links = static_cast<NativeControlPanelNavLinks*>(
        CoTaskMemAlloc(sizeof(NativeControlPanelNavLinks)));
    if (!links) return nullptr;
    memset(links, 0, sizeof(*links));
    links->vtable = reinterpret_cast<BYTE*>(module) +
                    kControlPanelNavLinksVtableRva;
    links->referenceCount = kPinnedNavLinksReferenceCount;
    links->links = DPA_Create(8);
    if (!links->links) {
        DestroyUnpublishedNativeNavLinks(links);
        return nullptr;
    }

    struct Definition {
        int list;
        UINT labelId;
        PCWSTR fallback;
        PCWSTR command;
        PCWSTR arguments;
    };
    static constexpr Definition definitions[] = {
        {0, 350, L"Check for updates", L"wurestorer:check", L""},
        {0, 351, L"Change settings",
         L"shell:::{36EEF7DB-88AD-4E81-AD49-0E313F0C35F8}\\pageSettings", L""},
        {0, 352, L"View update history", L"wurestorer:history", L""},
        {0, 353, L"Restore hidden updates",
         L"wurestorer:hidden", L""},
        {0, 20024, L"Updates: frequently asked questions", L"wurestorer:faq", L""},
        {1, 355, L"Security Center", L"wurestorer:security", L""},
        {1, 356, L"Installed Updates",
         L"wurestorer:installed", L""},
    };

    for (const auto& definition : definitions) {
        const wchar_t* label = EmbeddedMuiString(definition.labelId);
        auto* link = CreateNativeNavLink(
            definition.list, label ? label : definition.fallback,
            definition.command, definition.arguments);
        if (!link || DPA_InsertPtr(links->links, 0x7fffffff, link) == -1) {
            if (link) DestroyUnpublishedNativeNavLink(link, nullptr);
            DestroyUnpublishedNativeNavLinks(links);
            return nullptr;
        }
    }
    return links;
}

using PSPropertyBag_WriteUnknown_t = HRESULT(WINAPI*)(IPropertyBag*, PCWSTR,
                                                       IUnknown*);
static PSPropertyBag_WriteUnknown_t PSPropertyBag_WriteUnknownOriginal = nullptr;

static bool IsPrivateWucltuxAddress(const void* address) {
    if (!address) return false;
    HMODULE callerModule = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<PCWSTR>(address), &callerModule)) {
        return false;
    }
    return callerModule && callerModule == g_module.load(std::memory_order_acquire);
}

static HRESULT WINAPI PSPropertyBag_WriteUnknownHook(IPropertyBag* bag,
                                                      PCWSTR propertyName,
                                                      IUnknown* value) {
    // Check the object's vtable rather than the return address. The first call
    // can pass through the PE delay-load helper, so its immediate caller isn't
    // necessarily wucltux even though the object itself always is.
    const auto* navLinks =
        reinterpret_cast<const NativeControlPanelNavLinks*>(value);
    if (propertyName && value &&
        wcscmp(propertyName, L"ControlPanelNavLinks") == 0 &&
        IsPrivateWucltuxAddress(navLinks->vtable)) {
        try {
            // This bag now has a real wucltux-owned list, so PublishNative-
            // NavigationLinks must not allocate a replacement (see there). The
            // bag is reference-held so the marker pointer stays valid.
            static thread_local IPropertyBag* wucltuxWroteToBag = nullptr;
            if (wucltuxWroteToBag != bag) {
                if (wucltuxWroteToBag) wucltuxWroteToBag->Release();
                bag->AddRef();
                wucltuxWroteToBag = bag;
            }
            const unsigned patched = RedirectNativeControlPanelNavLinks(value);
            Wh_Log(L"WUR: published native ControlPanelNavLinks (redirected=%u)",
                   patched);
        } catch (...) {
            // Never let an allocation or malformed private object escape through
            // the PROPSYS ABI. The untouched list remains a valid fallback.
            Wh_Log(L"WUR: could not redirect native ControlPanelNavLinks; using wucltux links");
        }
    }
    return PSPropertyBag_WriteUnknownOriginal(bag, propertyName, value);
}

static void InstallNativeControlPanelNavLinksHook() {
    HMODULE propsys = GetModuleHandleW(L"propsys.dll");
    if (!propsys)
        propsys = LoadLibraryExW(L"propsys.dll", nullptr,
                                 LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!propsys) {
        Wh_Log(L"WUR: propsys.dll unavailable; native WU navigation remains unmodified");
        return;
    }
    void* target = reinterpret_cast<void*>(
        GetProcAddress(propsys, "PSPropertyBag_WriteUnknown"));
    if (!target) {
        Wh_Log(L"WUR: PSPropertyBag_WriteUnknown not found; native WU navigation remains unmodified");
        return;
    }
    WindhawkUtils::SetFunctionHook(
        reinterpret_cast<PSPropertyBag_WriteUnknown_t>(target),
        PSPropertyBag_WriteUnknownHook,
        &PSPropertyBag_WriteUnknownOriginal);
}

// Windows 10 21H2 can create the native sidebar but skip wucltux's private
// PopulateControlPanelNavLinks call, leaving the pane empty. CElementWithSite is
// where the page receives the shell site used by the sample's _punkSite call.
// Hook that exact method in the pinned payload and publish our complete list.
//
// kCElementWithSiteSetSiteRva (0x26960) and kControlPanelNavLinksVtableRva
// (0x2350) are RVAs into the EXACT SHA-256-pinned wucltux.dll payload declared
// at the top of this file (7.9.9600.17415, winblue_r4.141028-1500). They were
// verified against that precise build; a different payload would need new RVAs.
static const GUID kSidPerLayoutPropertyBag = {
    0xa46e5c25, 0xc09c, 0x4ca8,
    {0x9a, 0x53, 0x49, 0xcf, 0x7f, 0x86, 0x55, 0x25}};
static constexpr ULONG_PTR kCElementWithSiteSetSiteRva = 0x26960;

static HRESULT PublishNativeNavigationLinks(IUnknown* site) {
    if (!site) return E_INVALIDARG;

    IServiceProvider* services = nullptr;
    HRESULT hr = site->QueryInterface(IID_PPV_ARGS(&services));
    if (FAILED(hr) || !services) return FAILED(hr) ? hr : E_NOINTERFACE;

    IPropertyBag* bag = nullptr;
    hr = services->QueryService(kSidPerLayoutPropertyBag, IID_PPV_ARGS(&bag));
    services->Release();
    if (FAILED(hr) || !bag) return FAILED(hr) ? hr : E_NOINTERFACE;

    // On builds where wucltux DOES publish its own list for this bag (Windows
    // 11), PSPropertyBag_WriteUnknownHook records the bag here; publishing a
    // second list afterwards would only allocate and leak. Only publish when
    // wucltux has not already supplied one. The pointer is reference-held so a
    // recycled bag address can never be misread as "already written".
    static thread_local IPropertyBag* wucltuxWroteToBag = nullptr;
    if (wucltuxWroteToBag == bag) {
        bag->Release();
        return S_FALSE;
    }

    // CElementWithSite::SetSite can be called for several page elements sharing
    // one per-layout bag. Publish only once per UI thread and bag. The bag is
    // reference-held (AddRef) so the stored pointer stays valid; comparing a
    // raw released pointer could collide with a recycled bag address and leave
    // the sidebar empty.
    static thread_local IPropertyBag* lastPublishedBag = nullptr;
    if (lastPublishedBag == bag) {
        bag->Release();
        return S_FALSE;
    }

    NativeControlPanelNavLinks* links = CreateNativeControlPanelNavLinks();
    if (!links) {
        bag->Release();
        return E_OUTOFMEMORY;
    }

    PSPropertyBag_WriteUnknown_t writer = PSPropertyBag_WriteUnknownOriginal;
    if (!writer) {
        HMODULE propsys = GetModuleHandleW(L"propsys.dll");
        if (propsys) {
            writer = reinterpret_cast<PSPropertyBag_WriteUnknown_t>(
                GetProcAddress(propsys, "PSPropertyBag_WriteUnknown"));
        }
    }
    if (!writer) {
        DestroyUnpublishedNativeNavLinks(links);
        bag->Release();
        return HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
    }

    hr = writer(bag, L"ControlPanelNavLinks",
                reinterpret_cast<IUnknown*>(links));
    if (SUCCEEDED(hr)) {
        if (lastPublishedBag) lastPublishedBag->Release();
        bag->AddRef();
        lastPublishedBag = bag;
        Wh_Log(L"WUR: complete native ControlPanelNavLinks list published");
        // The list intentionally remains process-lifetime. Its vtable belongs to
        // the payload, which this mod already keeps mapped after page creation.
    } else {
        DestroyUnpublishedNativeNavLinks(links);
        Wh_Log(L"WUR: native ControlPanelNavLinks publication failed (hr=0x%08X)",
               static_cast<unsigned>(hr));
    }
    bag->Release();
    return hr;
}

using CElementWithSiteSetSite_t = HRESULT(WINAPI*)(void*, IUnknown*);
static CElementWithSiteSetSite_t CElementWithSiteSetSiteOriginal = nullptr;
static std::atomic<bool> g_setSiteHookInstalled{false};

static HRESULT WINAPI CElementWithSiteSetSiteHook(void* self, IUnknown* site) {
    if (!CElementWithSiteSetSiteOriginal) return E_FAIL;
    const HRESULT hr = CElementWithSiteSetSiteOriginal(self, site);
    if (SUCCEEDED(hr) && site && !g_stopping.load()) {
        const HRESULT publishHr = PublishNativeNavigationLinks(site);
        if (FAILED(publishHr)) {
            Wh_Log(L"WUR: per-layout navigation bag unavailable (hr=0x%08X)",
                   static_cast<unsigned>(publishHr));
        }
    }
    return hr;
}

static bool InstallWucltuxSetSiteHook(HMODULE module) {
    if (!module) return false;
    bool expected = false;
    if (!g_setSiteHookInstalled.compare_exchange_strong(expected, true))
        return true;

    void* target = reinterpret_cast<BYTE*>(module) +
                   kCElementWithSiteSetSiteRva;
    if (!Wh_SetFunctionHook(
            target, reinterpret_cast<void*>(CElementWithSiteSetSiteHook),
            reinterpret_cast<void**>(&CElementWithSiteSetSiteOriginal))) {
        g_setSiteHookInstalled.store(false);
        Wh_Log(L"WUR: failed to register CElementWithSite::SetSite hook");
        return false;
    }
    if (!Wh_ApplyHookOperations()) {
        g_setSiteHookInstalled.store(false);
        Wh_Log(L"WUR: failed to apply CElementWithSite::SetSite hook");
        return false;
    }
    Wh_Log(L"WUR: CElementWithSite::SetSite hook active; native links will be published");
    return true;
}

// =============================================================================
// Classic "Change settings" dialog (Win32).
// -----------------------------------------------------------------------------
// Modeled on the classic-taskbar-properties mod: a real Win32 dialog built from
// an in-memory DLGTEMPLATE and shown with CreateDialogIndirectParamW - no
// DirectUI involved. It replaces the broken DirectUI pageSettings page: any
// navigation that would open shell:::{CLSID}\\pageSettings is intercepted by the
// ShellExecute hooks above and this dialog is shown instead.
//
// The dialog shows the four classic important-update modes and related options
// as a read-only snapshot. Modern Windows owns these machine-wide settings, so
// changes must be made in the Windows Settings app.
// =============================================================================
enum {
    kWuDlgSettings = 0x7701,
    kWuCtlCombo = 0x7710,
    kWuCtlOptionLabel = 0x7711,
    kWuCtlRecommended = 0x7712,
    kWuCtlMsProducts = 0x7713,
    kWuCtlAllUsers = 0x7714,
    kWuCtlNote = 0x7715,
};

// The classic settings dialog can be opened from several Explorer windows,
// each running on its own UI thread. Track every live dialog in a mutex-guarded
// container (a plain global HWND would let two windows create two dialogs and
// leave one of them registered with mod code after the image is unmapped).
static std::mutex g_wuSettingsDlgMutex;
static std::vector<HWND> g_wuSettingsDlgs;
static DWORD g_wuDlgAuOptions = 4;
static bool g_wuDlgRecommended = false;
static bool g_wuDlgMsProducts = false;
static bool g_wuDlgAllUsers = false;

static void RegisterWuSettingsDialog(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_wuSettingsDlgMutex);
    g_wuSettingsDlgs.push_back(hwnd);
}

static void UnregisterWuSettingsDialog(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_wuSettingsDlgMutex);
    auto it = std::remove(g_wuSettingsDlgs.begin(), g_wuSettingsDlgs.end(), hwnd);
    g_wuSettingsDlgs.erase(it, g_wuSettingsDlgs.end());
}

// Returns the first live settings dialog, or nullptr. Used to re-foreground an
// existing dialog instead of opening a second one.
static HWND FindLiveWuSettingsDialog() {
    std::lock_guard<std::mutex> lock(g_wuSettingsDlgMutex);
    for (HWND hwnd : g_wuSettingsDlgs)
        if (IsWindow(hwnd)) return hwnd;
    return nullptr;
}

// Closes every live settings dialog. Wh_ModUninit runs on an arbitrary
// Windhawk thread while the dialogs belong to Explorer UI threads, so use
// SendMessageTimeoutW (bounded, cross-thread safe) and never send while
// holding the lock.
static void CloseAllWuSettingsDialogs() {
    std::vector<HWND> dialogs;
    {
        std::lock_guard<std::mutex> lock(g_wuSettingsDlgMutex);
        dialogs.swap(g_wuSettingsDlgs);
    }
    for (HWND hwnd : dialogs)
        if (IsWindow(hwnd))
            SendMessageTimeoutW(hwnd, WM_CLOSE, 0, 0, SMTO_ABORTIFHUNG, 5000,
                                nullptr);
}

// True when the ShellExecute target is our applet's settings child page
// (shell:::{CLSID}\\pageSettings), either as a bare shell: URI or as
// "%SystemRoot%\explorer.exe" + shellexecuteparams.
static const wchar_t* SelectRecommendedUpdatesLabel() {
    static const std::unordered_map<std::wstring, const wchar_t*> kTexts = {
        { L"en", L"Give me recommended updates the same way I receive important updates" },
        { L"it", L"Fornisci gli aggiornamenti consigliati nello stesso modo in cui ricevo gli aggiornamenti importanti" },
        { L"es", L"Proporcionar actualizaciones recomendadas de la misma manera que recibo las actualizaciones importantes" },
        { L"fr", L"Me donner les mises à jour recommandées de la même manière que les mises à jour importantes" },
        { L"tr", L"Önemli güncellemelerle aynı şekilde önerilen güncellemeleri de ver" },
        { L"ru", L"Предоставлять рекомендуемые обновления так же, как и важные" },
        { L"pt", L"Dar-me atualizações recomendadas da mesma forma que recebo as importantes" },
        { L"zh", L"以接收重要更新的相同方式为我提供推荐更新" },
        { L"pl", L"Zapewniaj zalecane aktualizacje w taki sam sposób, jak ważne" },
        { L"nl", L"Geef mij aanbevolen updates op dezelfde manier als belangrijke updates" },
    };
    auto it = kTexts.find(CurrentLanguage());
    if (it == kTexts.end()) it = kTexts.find(L"en");
    return it->second;
}

// Classic option text for an AUOptions value (1..4), multilingual via resstr.


static void ReadAuxAuValues(bool& recommended, bool& msProducts, bool& allUsers) {
    recommended = false;
    msProducts = false;
    allUsers = false;
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD v = 0, sz = sizeof(v);
        if (RegQueryValueExW(hKey, L"IncludeRecommendedUpdates", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(&v), &sz) == ERROR_SUCCESS && v)
            recommended = true;
        sz = sizeof(v);
        if (RegQueryValueExW(hKey, L"MicrosoftUpdate", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(&v), &sz) == ERROR_SUCCESS && v)
            msProducts = true;
        sz = sizeof(v);
        if (RegQueryValueExW(hKey, L"AllowAllUsers", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(&v), &sz) == ERROR_SUCCESS && v)
            allUsers = true;
        RegCloseKey(hKey);
    }
}


static void CloseWuSettingsDialog(HWND hwnd) {
    DestroyWindow(hwnd);
}

static std::wstring StripAmpersand(const wchar_t* s) {
    std::wstring out;
    if (!s) return out;
    for (const wchar_t* p = s; *p; ++p) {
        if (*p != L'&') out += *p;
    }
    return out;
}

static INT_PTR CALLBACK WuSettingsDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_INITDIALOG: {
            RegisterWuSettingsDialog(hwnd);
            HWND hCombo = GetDlgItem(hwnd, kWuCtlCombo);
            if (hCombo) {
                SendMessageW(hCombo, CB_RESETCONTENT, 0, 0);
                SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)WuOptionText(4));
                SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)WuOptionText(3));
                SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)WuOptionText(2));
                SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)WuOptionText(1));
                int sel = 0;
                if (g_wuDlgAuOptions == 4) sel = 0;
                else if (g_wuDlgAuOptions == 3) sel = 1;
                else if (g_wuDlgAuOptions == 2) sel = 2;
                else if (g_wuDlgAuOptions == 1) sel = 3;
                SendMessageW(hCombo, CB_SETCURSEL, sel, 0);
            }
            CheckDlgButton(hwnd, kWuCtlRecommended, g_wuDlgRecommended ? BST_CHECKED : BST_UNCHECKED);
            CheckDlgButton(hwnd, kWuCtlMsProducts, g_wuDlgMsProducts ? BST_CHECKED : BST_UNCHECKED);
            CheckDlgButton(hwnd, kWuCtlAllUsers, g_wuDlgAllUsers ? BST_CHECKED : BST_UNCHECKED);
            EnableWindow(hCombo, FALSE);
            EnableWindow(GetDlgItem(hwnd, kWuCtlRecommended), FALSE);
            EnableWindow(GetDlgItem(hwnd, kWuCtlMsProducts), FALSE);
            EnableWindow(GetDlgItem(hwnd, kWuCtlAllUsers), FALSE);
            return TRUE;
        }
        case WM_CTLCOLORSTATIC: {
            // The privacy note is drawn in grey, like the original page.
            HDC hdc = reinterpret_cast<HDC>(wParam);
            HWND hCtl = reinterpret_cast<HWND>(lParam);
            if (hCtl == GetDlgItem(hwnd, kWuCtlNote)) {
                SetTextColor(hdc, RGB(90, 90, 90));
                SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
                return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_BTNFACE));
            }
            break;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                    CloseWuSettingsDialog(hwnd);
                    return TRUE;
                case IDCANCEL:
                    DestroyWindow(hwnd);
                    return TRUE;
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return TRUE;
        case WM_DESTROY:
            UnregisterWuSettingsDialog(hwnd);
            return TRUE;
    }
    return FALSE;
}

static void ShowWuSettingsDialog(HWND parent) {
    if (HWND existing = FindLiveWuSettingsDialog()) {
        SetForegroundWindow(existing);
        return;
    }

    g_wuDlgAuOptions = ReadAuOptionsValue();
    ReadAuxAuValues(g_wuDlgRecommended, g_wuDlgMsProducts, g_wuDlgAllUsers);

    const int kControls = 10; // group, desc, label, combo, 3 checkboxes, note, OK, Cancel
    BYTE* buf = new (std::nothrow) BYTE[4096];
    if (!buf) return;
    BYTE* p = buf;
    const BYTE* const bufEnd = buf + 4096; // upper bound for every write below
    auto align4 = [](BYTE*& ptr) { ptr = reinterpret_cast<BYTE*>((reinterpret_cast<UINT_PTR>(ptr) + 3) & ~static_cast<UINT_PTR>(3)); };

    LPDLGTEMPLATEW pDlg = reinterpret_cast<LPDLGTEMPLATEW>(p);
    pDlg->style = DS_SETFONT | DS_MODALFRAME | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU;
    pDlg->dwExtendedStyle = 0;
    pDlg->cdit = 0;
    pDlg->x = 0; pDlg->y = 0;
    pDlg->cx = 380; pDlg->cy = 272;
    p += sizeof(DLGTEMPLATE);
    *(WORD*)p = 0; p += 2;                       // no menu
    *(WORD*)p = 0; p += 2;                       // no class
    *(WORD*)p = 0; p += 2;                       // empty title (set later)
    *(WORD*)p = 8; p += 2;                       // font point size
    const wchar_t kFont[] = L"Segoe UI";
    memcpy(p, kFont, sizeof(kFont)); p += sizeof(kFont);

    bool templateValid = true;
    auto addCtrl = [&](DWORD style, DWORD exStyle, short x, short y, short cx, short cy,
                       WORD id, LPCWSTR cls, LPCWSTR cap) {
        if (!templateValid) return;
        align4(p);
        if (p + sizeof(DLGITEMTEMPLATE) > bufEnd) {
            templateValid = false;
            return;
        }
        LPDLGITEMTEMPLATE pi = reinterpret_cast<LPDLGITEMTEMPLATE>(p);
        pi->style = WS_CHILD | WS_VISIBLE | style;
        pi->dwExtendedStyle = exStyle;
        pi->x = x; pi->y = y; pi->cx = cx; pi->cy = cy; pi->id = id;
        p += sizeof(DLGITEMTEMPLATE);
        const size_t clsBytes = (wcslen(cls) + 1) * sizeof(wchar_t);
        if (p + clsBytes > bufEnd) {
            templateValid = false;
            return;
        }
        memcpy(p, cls, clsBytes);
        p += clsBytes;
        const size_t captionBytes = (wcslen(cap) + 1) * sizeof(wchar_t);
        if (p + captionBytes + sizeof(WORD) > bufEnd) {
            templateValid = false;
            return;
        }
        memcpy(p, cap, captionBytes);
        p += captionBytes;
        *reinterpret_cast<WORD*>(p) = 0;  // no creation data
        p += sizeof(WORD);
        ++pDlg->cdit;
    };

    // Translated texts (all with fallbacks so no label is ever empty)
    const std::wstring grp = SelectChooseHowToInstallUpdatesLabel();
    const std::wstring title = StripAmpersand(
        EmbeddedMuiString(351) ? EmbeddedMuiString(351) : L"Change settings");
    const std::wstring importantLabel = StripAmpersand(
        EmbeddedMuiString(1232) ? EmbeddedMuiString(1232) : L"Important updates");
    const wchar_t* descText = EmbeddedMuiString(1102);
    if (!descText) descText = L"When your PC is online, Windows can automatically check for important updates and install them using these settings.";
    const wchar_t* noteText = SelectReadOnlySettingsNote();
    const wchar_t* allUsersText = EmbeddedMuiString(1001);
    if (!allUsersText) allUsersText = L"Allow all users to install updates on this computer";
    wchar_t okText[64] = L"OK";
    wchar_t cancelText[64] = L"Cancel";
    {
        HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
        if (shell32) {
            LoadStringW(shell32, 800, okText, ARRAYSIZE(okText));       // IDS_OK
            LoadStringW(shell32, 801, cancelText, ARRAYSIZE(cancelText)); // IDS_CANCEL
        }
    }

    // --- Windows 7-style layout ---
    // "Choose how to install updates" group box with description + combobox.
    addCtrl(BS_GROUPBOX | WS_TABSTOP, 0, 8, 6, 352, 122, 0x7F00, L"Button", grp.c_str());
    addCtrl(SS_LEFT, 0, 18, 18, 332, 38, 0x7F01, L"Static", descText);              // description (wraps)
    addCtrl(SS_LEFT, 0, 18, 62, 240, 12, 0x7F02, L"Static", importantLabel.c_str()); // "Important updates:"
    addCtrl(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 0, 18, 76, 332, 120, kWuCtlCombo, L"ComboBox", L"");
    // Options below the group box (classic checkboxes)
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 16, 138, 344, 14, kWuCtlRecommended, L"Button", SelectRecommendedUpdatesLabel());
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 16, 156, 344, 14, kWuCtlMsProducts, L"Button",
            EmbeddedMuiString(475) ? EmbeddedMuiString(475) : L"Give me updates for other Microsoft products");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 16, 174, 344, 14, kWuCtlAllUsers, L"Button", allUsersText);
    // Privacy note (grey)
    addCtrl(SS_LEFT, 0, 16, 194, 344, 28, kWuCtlNote, L"Static", noteText);
    // OK / Cancel - bottom right, classic size, always visible.
    addCtrl(BS_DEFPUSHBUTTON | WS_TABSTOP, 0, 248, 248, 60, 16, IDOK, L"Button", okText);
    addCtrl(BS_PUSHBUTTON | WS_TABSTOP, 0, 312, 248, 60, 16, IDCANCEL, L"Button", cancelText);

    if (!templateValid || pDlg->cdit != kControls) {
        Wh_Log(L"Windows Update Restorer: settings dialog template buffer was too small");
        delete[] buf;
        return;
    }

    HWND hwnd = CreateDialogIndirectParamW(GetModuleHandleW(nullptr),
                                           reinterpret_cast<LPDLGTEMPLATE>(buf),
                                           parent, WuSettingsDlgProc, 0);
    if (!hwnd) {
        Wh_Log(L"Windows Update Restorer: classic settings dialog creation FAILED (err=%u)", GetLastError());
    }
    delete[] buf;

    if (hwnd && IsWindow(hwnd)) {
        SetWindowTextW(hwnd, title.c_str());
        ShowWindow(hwnd, SW_SHOWNORMAL);
        SetForegroundWindow(hwnd);
    }
}

// =============================================================================
// Shared header band for the mod's small dialogs
// -----------------------------------------------------------------------------
// Light-blue gradient header (Windows Update style) with the skinned applet
// logo (drawn from a GDI+ HighQualityBicubic icon, so it stays crisp) and a
// bold dark-blue title. Used by the FAQ window and the update-check window.
// =============================================================================

static void FillVerticalGradient(HDC hdc, const RECT& rc, COLORREF top, COLORREF bottom) {
    const int height = static_cast<int>(rc.bottom - rc.top);
    if (height <= 0) return;
    const int steps = height < 64 ? height : 64;
    if (steps <= 1) {
        HBRUSH brush = CreateSolidBrush(top);
        FillRect(hdc, &rc, brush);
        DeleteObject(brush);
        return;
    }
    const int stepH = (height + steps - 1) / steps;
    for (int i = 0; i < steps; ++i) {
        const int y = static_cast<int>(rc.top) + i * stepH;
        const int h = (std::min)(stepH, static_cast<int>(rc.bottom) - y);
        const BYTE t = static_cast<BYTE>(i * 255 / (steps - 1));
        const COLORREF color = RGB(
            (GetRValue(top) * (255 - t) + GetRValue(bottom) * t) / 255,
            (GetGValue(top) * (255 - t) + GetGValue(bottom) * t) / 255,
            (GetBValue(top) * (255 - t) + GetBValue(bottom) * t) / 255);
        HBRUSH brush = CreateSolidBrush(color);
        RECT row = { rc.left, y, rc.right, y + h };
        FillRect(hdc, &row, brush);
        DeleteObject(brush);
    }
}

// Paints the standard light-blue header band used by the FAQ / check dialogs:
// gradient, accent line, applet logo (bicubic) and bold dark-blue title.
static void WuPaintDialogHeader(HDC hdc, const RECT& client, int headerHeight,
                                HICON icon, int iconSize, HFONT titleFont,
                                const wchar_t* title) {
    RECT header = { client.left, client.top, client.right, headerHeight };
    FillVerticalGradient(hdc, header, RGB(241, 247, 255), RGB(202, 224, 248));

    HPEN linePen = CreatePen(PS_SOLID, 1, RGB(163, 207, 245));
    HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, linePen));
    MoveToEx(hdc, client.left, headerHeight, nullptr);
    LineTo(hdc, client.right, headerHeight);
    SelectObject(hdc, oldPen);
    DeleteObject(linePen);

    if (icon) {
        const int y = (headerHeight - iconSize) / 2;
        DrawIconEx(hdc, 16, y, icon, iconSize, iconSize, 0, nullptr, DI_NORMAL);
    }
    HFONT oldFont = titleFont ? static_cast<HFONT>(SelectObject(hdc, titleFont))
                              : nullptr;
    SetTextColor(hdc, RGB(0, 70, 130));
    SetBkMode(hdc, TRANSPARENT);
    RECT titleRect = { 16 + iconSize + 10, 0, client.right - 14, headerHeight };
    DrawTextW(hdc, title ? title : L"", -1, &titleRect,
              DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS);
    if (oldFont) SelectObject(hdc, oldFont);
}

static HFONT WuCreateHeaderFont() {
    return CreateFontW(-16, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
}

// Applies the dialog's own font (Segoe UI, from the template's DS_SETFONT) to
// a programmatically-created child control, so every label, button and the
// RichEdit match the dialog's text instead of falling back to the system font.
static void WuApplyDialogFont(HWND dlg, HWND ctrl) {
    if (!ctrl) return;
    HFONT font = reinterpret_cast<HFONT>(SendMessageW(dlg, WM_GETFONT, 0, 0));
    if (font)
        SendMessageW(ctrl, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
}

// WM_CTLCOLORSTATIC handler for the mod's white dialogs: the default dialog
// background brush is COLOR_BTNFACE (gray), so STATIC labels would sit on a
// gray rectangle. Return a white brush and paint the text with a transparent
// background so black text always has a clean white backing.
static LRESULT WuOnCtlColorStatic(HDC hdc) {
    SetTextColor(hdc, RGB(0, 0, 0));
    SetBkColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);
    return reinterpret_cast<LRESULT>(GetStockObject(WHITE_BRUSH));
}

// =============================================================================
// "Updates: frequently asked questions" window (Win32)
// -----------------------------------------------------------------------------
// A small normal window (in-memory DLGTEMPLATE, same technique as the classic
// settings dialog) that answers ten generic questions about Windows updates.
// Opened by the sidebar link below "Restore hidden updates" (wurestorer:faq).
// It has the light-blue gradient header (with the bicubic applet logo) on a
// white body; the questions are bold (read-only RichEdit with per-line
// character formatting). All ten supported languages are provided; English is
// the fallback for any unknown code.
// =============================================================================
enum { kWuCtlFaqBody = 0x7810 };

// The FAQ window can be opened from several Explorer windows (each on its own
// thread); track every live window in a guarded container so teardown closes
// them all.
static std::mutex g_wuFaqDlgMutex;
static std::vector<HWND> g_wuFaqDlgs;

static void RegisterWuFaqDialog(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_wuFaqDlgMutex);
    g_wuFaqDlgs.push_back(hwnd);
}
static void UnregisterWuFaqDialog(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_wuFaqDlgMutex);
    auto it = std::remove(g_wuFaqDlgs.begin(), g_wuFaqDlgs.end(), hwnd);
    g_wuFaqDlgs.erase(it, g_wuFaqDlgs.end());
}
static HWND FindLiveWuFaqDialog() {
    std::lock_guard<std::mutex> lock(g_wuFaqDlgMutex);
    for (HWND hwnd : g_wuFaqDlgs)
        if (IsWindow(hwnd)) return hwnd;
    return nullptr;
}
static void CloseAllWuFaqDialogs() {
    std::vector<HWND> dialogs;
    {
        std::lock_guard<std::mutex> lock(g_wuFaqDlgMutex);
        dialogs.swap(g_wuFaqDlgs);
    }
    for (HWND hwnd : dialogs)
        if (IsWindow(hwnd))
            SendMessageTimeoutW(hwnd, WM_CLOSE, 0, 0, SMTO_ABORTIFHUNG, 5000,
                                nullptr);
}

static HWND g_wuFaqParent = nullptr;
static HICON g_wuFaqIconBig = nullptr;
static HICON g_wuFaqIconSmall = nullptr;
static HICON g_wuFaqHeaderIcon = nullptr;  // 32x32 bicubic logo for the header
static HFONT g_wuFaqTitleFont = nullptr;

struct WuFaqEntry {
    const wchar_t* q;
    const wchar_t* a;
};

// The ten generic questions and their short answers, per language.
static const WuFaqEntry* SelectFaqEntries() {
    struct LangFaq {
        const wchar_t* code;
        WuFaqEntry entries[10];
    };
    static const LangFaq kFaqs[] = {
        { L"en", {
            { L"1. Why do I need Windows updates?", L"Updates fix security issues and bugs and add new features, keeping your PC protected and stable." },
            { L"2. Are Windows updates free?", L"Yes. Important and recommended updates are free of charge from Microsoft." },
            { L"3. How do I check for updates?", L"Open Windows Update in Control Panel and click \"Check for updates\". You can also use Settings > Windows Update." },
            { L"4. When are updates installed?", L"By default Windows installs important updates automatically. You can change this in \"Change settings\"." },
            { L"5. Why does my PC restart after updates?", L"Some updates change important system files that are in use, so Windows needs to restart to apply them." },
            { L"6. Where can I see installed updates?", L"In Windows Update, click \"View update history\" to see the list of installed updates." },
            { L"7. Can I uninstall an update?", L"Yes, from \"Installed Updates\". Some important updates cannot be uninstalled." },
            { L"8. How long do updates take to install?", L"Most updates take a few minutes. Large updates can take longer; do not turn off your PC while they install." },
            { L"9. What happens if I don't install updates?", L"Without the latest updates, your PC is more vulnerable to security attacks and performance problems." },
            { L"10. How do I change update settings?", L"Use \"Change settings\" in Windows Update, or open Settings > Windows Update." },
        } },
        { L"it", {
            { L"1. Perché servono gli aggiornamenti di Windows?", L"Correggono problemi di sicurezza e bug e aggiungono nuove funzionalità, mantenendo il PC protetto e stabile." },
            { L"2. Gli aggiornamenti di Windows sono gratuiti?", L"Sì. Gli aggiornamenti importanti e consigliati sono gratuiti." },
            { L"3. Come controllo gli aggiornamenti?", L"Apri Windows Update nel Pannello di controllo e fai clic su \"Controlla aggiornamenti\". Puoi anche usare Impostazioni > Windows Update." },
            { L"4. Quando vengono installati gli aggiornamenti?", L"Di norma Windows installa automaticamente gli aggiornamenti importanti. Puoi modificarlo in \"Cambia impostazioni\"." },
            { L"5. Perché il PC si riavvia dopo gli aggiornamenti?", L"Alcuni aggiornamenti modificano file di sistema in uso; Windows deve riavviarsi per applicarli." },
            { L"6. Dove posso vedere gli aggiornamenti installati?", L"In Windows Update fai clic su \"Visualizza cronologia aggiornamenti\" per vedere l'elenco degli aggiornamenti installati." },
            { L"7. Posso disinstallare un aggiornamento?", L"Sì, da \"Aggiornamenti installati\". Alcuni aggiornamenti importanti non possono essere disinstallati." },
            { L"8. Quanto tempo richiede l'installazione degli aggiornamenti?", L"La maggior parte richiede pochi minuti. Gli aggiornamenti grandi possono richiedere più tempo; non spegnere il PC durante l'installazione." },
            { L"9. Cosa succede se non installo gli aggiornamenti?", L"Senza gli aggiornamenti più recenti, il PC è più vulnerabile ad attacchi alla sicurezza e problemi di prestazioni." },
            { L"10. Come posso modificare le impostazioni degli aggiornamenti?", L"Usa \"Cambia impostazioni\" in Windows Update oppure apri Impostazioni > Windows Update." },
        } },
        { L"es", {
            { L"1. ¿Por qué necesito actualizaciones de Windows?", L"Corrigen problemas de seguridad y errores y añaden nuevas funciones, manteniendo el PC protegido y estable." },
            { L"2. ¿Las actualizaciones de Windows son gratuitas?", L"Sí. Las actualizaciones importantes y recomendadas son gratuitas." },
            { L"3. ¿Cómo busco actualizaciones?", L"Abra Windows Update en el Panel de control y haga clic en \"Buscar actualizaciones\". También puede usar Configuración > Windows Update." },
            { L"4. ¿Cuándo se instalan las actualizaciones?", L"Normalmente Windows instala las actualizaciones importantes automáticamente. Puede cambiarlo en \"Cambiar la configuración\"." },
            { L"5. ¿Por qué se reinicia el PC después de las actualizaciones?", L"Algunas actualizaciones cambian archivos del sistema en uso; Windows necesita reiniciarse para aplicarlas." },
            { L"6. ¿Dónde veo las actualizaciones instaladas?", L"En Windows Update, haga clic en \"Ver historial de actualizaciones\" para ver la lista de actualizaciones instaladas." },
            { L"7. ¿Puedo desinstalar una actualización?", L"Sí, desde \"Actualizaciones instaladas\". Algunas actualizaciones importantes no se pueden desinstalar." },
            { L"8. ¿Cuánto tarda en instalarse una actualización?", L"La mayoría tarda unos minutos. Las actualizaciones grandes pueden tardar más; no apague el PC mientras se instalan." },
            { L"9. ¿Qué pasa si no instalo las actualizaciones?", L"Sin las actualizaciones más recientes, su PC es más vulnerable a ataques de seguridad y problemas de rendimiento." },
            { L"10. ¿Cómo cambio la configuración de actualizaciones?", L"Use \"Cambiar la configuración\" en Windows Update o abra Configuración > Windows Update." },
        } },
        { L"fr", {
            { L"1. Pourquoi ai-je besoin des mises à jour Windows ?", L"Elles corrigent des problèmes de sécurité et des bogues et ajoutent de nouvelles fonctions, gardant le PC protégé et stable." },
            { L"2. Les mises à jour Windows sont-elles gratuites ?", L"Oui. Les mises à jour importantes et recommandées sont gratuites." },
            { L"3. Comment rechercher des mises à jour ?", L"Ouvrez Windows Update dans le Panneau de configuration et cliquez sur \"Rechercher des mises à jour\". Vous pouvez aussi utiliser Paramètres > Windows Update." },
            { L"4. Quand les mises à jour sont-elles installées ?", L"En général, Windows installe automatiquement les mises à jour importantes. Vous pouvez le modifier dans \"Modifier les paramètres\"." },
            { L"5. Pourquoi le PC redémarre-t-il après les mises à jour ?", L"Certaines mises à jour modifient des fichiers système en cours d'utilisation ; Windows doit redémarrer pour les appliquer." },
            { L"6. Où voir les mises à jour installées ?", L"Dans Windows Update, cliquez sur \"Afficher l'historique des mises à jour\" pour voir la liste des mises à jour installées." },
            { L"7. Puis-je désinstaller une mise à jour ?", L"Oui, à partir de \"Mises à jour installées\". Certaines mises à jour importantes ne peuvent pas être désinstallées." },
            { L"8. Combien de temps prend l'installation des mises à jour ?", L"La plupart prennent quelques minutes. Les mises à jour volumineuses peuvent prendre plus de temps ; n'éteignez pas le PC pendant l'installation." },
            { L"9. Que se passe-t-il si je n'installe pas les mises à jour ?", L"Sans les dernières mises à jour, votre PC est plus vulnérable aux attaques de sécurité et aux problèmes de performances." },
            { L"10. Comment modifier les paramètres des mises à jour ?", L"Utilisez \"Modifier les paramètres\" dans Windows Update ou ouvrez Paramètres > Windows Update." },
        } },
        { L"tr", {
            { L"1. Windows güncelleştirmelerine neden ihtiyacım var?", L"Güvenlik sorunlarını ve hataları düzeltir, yeni özellikler ekler; bilgisayarınızı korumalı ve kararlı tutar." },
            { L"2. Windows güncelleştirmeleri ücretsiz mi?", L"Evet. Önemli ve önerilen güncelleştirmeler ücretsizdir." },
            { L"3. Güncelleştirmeleri nasıl denetlerim?", L"Denetim Masası'nda Windows Update'i açın ve \"Güncelleştirmeleri denetle\"ye tıklayın. Ayarlar > Windows Update'i de kullanabilirsiniz." },
            { L"4. Güncelleştirmeler ne zaman yüklenir?", L"Varsayılan olarak Windows önemli güncelleştirmeleri otomatik yükler. Bunu \"Ayarları değiştir\"den değiştirebilirsiniz." },
            { L"5. Güncelleştirmelerden sonra bilgisayar neden yeniden başlatılır?", L"Bazı güncelleştirmeler kullanımdaki sistem dosyalarını değiştirir; Windows'un bunları uygulamak için yeniden başlatılması gerekir." },
            { L"6. Yüklü güncelleştirmeleri nerede görebilirim?", L"Windows Update'te \"Güncelleme geçmişini görüntüle\"ye tıklayarak yüklü güncelleştirmelerin listesini görebilirsiniz." },
            { L"7. Bir güncelleştirmeyi kaldırabilir miyim?", L"Evet, \"Yüklü Güncelleştirmeler\" bölümünden. Bazı önemli güncelleştirmeler kaldırılamaz." },
            { L"8. Güncelleştirmelerin yüklenmesi ne kadar sürer?", L"Çoğu birkaç dakika sürer. Büyük güncelleştirmeler daha uzun sürebilir; yükleme sırasında bilgisayarı kapatmayın." },
            { L"9. Güncelleştirmeleri yüklemezsem ne olur?", L"En son güncelleştirmeler olmadan bilgisayarınız güvenlik saldırılarına ve performans sorunlarına karşı daha savunmasızdır." },
            { L"10. Güncelleştirme ayarlarını nasıl değiştiririm?", L"Windows Update'te \"Ayarları değiştir\"i kullanın veya Ayarlar > Windows Update'i açın." },
        } },
        { L"ru", {
            { L"1. Зачем нужны обновления Windows?", L"Они устраняют проблемы безопасности и ошибки и добавляют новые функции, сохраняя компьютер защищённым и стабильным." },
            { L"2. Обновления Windows бесплатны?", L"Да. Важные и рекомендуемые обновления бесплатны." },
            { L"3. Как проверить наличие обновлений?", L"Откройте Центр обновления Windows в панели управления и нажмите «Проверить наличие обновлений». Можно также использовать Параметры > Центр обновления Windows." },
            { L"4. Когда устанавливаются обновления?", L"Обычно Windows устанавливает важные обновления автоматически. Это можно изменить в «Изменении параметров»." },
            { L"5. Почему компьютер перезапускается после обновлений?", L"Некоторые обновления изменяют используемые системные файлы, и Windows требуется перезапуск для их применения." },
            { L"6. Где посмотреть установленные обновления?", L"В Центре обновления Windows нажмите «Просмотр журнала обновлений», чтобы увидеть список установленных обновлений." },
            { L"7. Можно ли удалить обновление?", L"Да, через «Установленные обновления». Некоторые важные обновления удалить нельзя." },
            { L"8. Сколько времени занимает установка обновлений?", L"Большинство обновлений устанавливается за несколько минут. Крупные могут занять больше времени; не выключайте компьютер во время установки." },
            { L"9. Что будет, если не устанавливать обновления?", L"Без последних обновлений компьютер более уязвим к атакам и проблемам с производительностью." },
            { L"10. Как изменить параметры обновлений?", L"Используйте «Изменение параметров» в Центре обновления Windows или откройте Параметры > Центр обновления Windows." },
        } },
        { L"pt", {
            { L"1. Por que preciso de atualizações do Windows?", L"Elas corrigem problemas de segurança e erros e adicionam novos recursos, mantendo o PC protegido e estável." },
            { L"2. As atualizações do Windows são gratuitas?", L"Sim. As atualizações importantes e recomendadas são gratuitas." },
            { L"3. Como verifico atualizações?", L"Abra o Windows Update no Painel de controle e clique em \"Verificar atualizações\". Você também pode usar Configurações > Windows Update." },
            { L"4. Quando as atualizações são instaladas?", L"Normalmente o Windows instala as atualizações importantes automaticamente. Você pode alterar isso em \"Alterar configurações\"." },
            { L"5. Por que o PC reinicia após as atualizações?", L"Algumas atualizações alteram arquivos do sistema em uso; o Windows precisa reiniciar para aplicá-las." },
            { L"6. Onde vejo as atualizações instaladas?", L"No Windows Update, clique em \"Ver histórico de atualizações\" para ver a lista de atualizações instaladas." },
            { L"7. Posso desinstalar uma atualização?", L"Sim, em \"Atualizações instaladas\". Algumas atualizações importantes não podem ser desinstaladas." },
            { L"8. Quanto tempo demora para instalar as atualizações?", L"A maioria leva alguns minutos. Atualizações grandes podem demorar mais; não desligue o PC durante a instalação." },
            { L"9. O que acontece se eu não instalar as atualizações?", L"Sem as atualizações mais recentes, seu PC fica mais vulnerável a ataques de segurança e problemas de desempenho." },
            { L"10. Como altero as configurações de atualização?", L"Use \"Alterar configurações\" no Windows Update ou abra Configurações > Windows Update." },
        } },
        { L"zh", {
            { L"1. 为什么需要 Windows 更新？", L"更新可修复安全问题和错误并添加新功能，让您的电脑保持受保护和稳定。" },
            { L"2. Windows 更新是免费的吗？", L"是的。重要更新和推荐更新都是免费的。" },
            { L"3. 如何检查更新？", L"在控制面板中打开 Windows 更新，然后点击“检查更新”。也可以使用“设置 > Windows 更新”。" },
            { L"4. 更新何时安装？", L"默认情况下，Windows 会自动安装重要更新。您可以在“更改设置”中修改。" },
            { L"5. 为什么更新后电脑会重新启动？", L"某些更新会更改正在使用的系统文件，Windows 需要重启才能应用它们。" },
            { L"6. 在哪里查看已安装的更新？", L"在 Windows 更新中，点击“查看更新历史记录”即可看到已安装更新的列表。" },
            { L"7. 可以卸载更新吗？", L"可以，通过“已安装的更新”。某些重要更新无法卸载。" },
            { L"8. 安装更新需要多长时间？", L"大多数更新只需几分钟。大型更新可能需要更长时间；安装期间请不要关闭电脑。" },
            { L"9. 如果不安装更新会怎样？", L"没有最新更新，您的电脑更容易受到安全攻击并出现性能问题。" },
            { L"10. 如何更改更新设置？", L"请使用 Windows 更新中的“更改设置”，或打开“设置 > Windows 更新”。" },
        } },
        { L"pl", {
            { L"1. Po co są aktualizacje systemu Windows?", L"Usuwają problemy z bezpieczeństwem i błędy oraz dodają nowe funkcje, utrzymując komputer chronionym i stabilnym." },
            { L"2. Czy aktualizacje systemu Windows są bezpłatne?", L"Tak. Ważne i zalecane aktualizacje są bezpłatne." },
            { L"3. Jak sprawdzić aktualizacje?", L"Otwórz Windows Update w Panelu sterowania i kliknij „Sprawdź aktualizacje”. Możesz też użyć Ustawienia > Windows Update." },
            { L"4. Kiedy instalowane są aktualizacje?", L"Domyślnie system Windows automatycznie instaluje ważne aktualizacje. Możesz to zmienić w „Zmień ustawienia”." },
            { L"5. Dlaczego komputer uruchamia się ponownie po aktualizacjach?", L"Niektóre aktualizacje zmieniają używane pliki systemowe; system Windows musi się ponownie uruchomić, aby je zastosować." },
            { L"6. Gdzie zobaczyć zainstalowane aktualizacje?", L"W Windows Update kliknij „Wyświetl historię aktualizacji”, aby zobaczyć listę zainstalowanych aktualizacji." },
            { L"7. Czy mogę odinstalować aktualizację?", L"Tak, przez „Zainstalowane aktualizacje”. Niektórych ważnych aktualizacji nie można odinstalować." },
            { L"8. Ile czasu zajmuje instalacja aktualizacji?", L"Większość zajmuje kilka minut. Duże aktualizacje mogą trwać dłużej; nie wyłączaj komputera podczas instalacji." },
            { L"9. Co się stanie, jeśli nie zainstaluję aktualizacji?", L"Bez najnowszych aktualizacji komputer jest bardziej narażony na ataki i problemy z wydajnością." },
            { L"10. Jak zmienić ustawienia aktualizacji?", L"Użyj „Zmień ustawienia” w Windows Update lub otwórz Ustawienia > Windows Update." },
        } },
        { L"nl", {
            { L"1. Waarom heb ik Windows-updates nodig?", L"Ze verhelpen beveiligingsproblemen en fouten en voegen nieuwe functies toe, zodat uw pc beschermd en stabiel blijft." },
            { L"2. Zijn Windows-updates gratis?", L"Ja. Belangrijke en aanbevolen updates zijn gratis." },
            { L"3. Hoe controleer ik op updates?", L"Open Windows Update in het Configuratiescherm en klik op 'Controleren op updates'. U kunt ook Instellingen > Windows Update gebruiken." },
            { L"4. Wanneer worden updates geïnstalleerd?", L"Standaard installeert Windows belangrijke updates automatisch. U kunt dit wijzigen bij 'Instellingen wijzigen'." },
            { L"5. Waarom start de pc opnieuw op na updates?", L"Sommige updates wijzigen systeembestanden die in gebruik zijn; Windows moet opnieuw opstarten om ze toe te passen." },
            { L"6. Waar kan ik geïnstalleerde updates zien?", L"Klik in Windows Update op 'Updategeschiedenis weergeven' om de lijst met geïnstalleerde updates te zien." },
            { L"7. Kan ik een update verwijderen?", L"Ja, via 'Geïnstalleerde updates'. Sommige belangrijke updates kunnen niet worden verwijderd." },
            { L"8. Hoe lang duurt het installeren van updates?", L"De meeste duren een paar minuten. Grote updates kunnen langer duren; zet de pc niet uit tijdens de installatie." },
            { L"9. Wat gebeurt er als ik geen updates installeer?", L"Zonder de nieuwste updates is uw pc kwetsbaarder voor beveiligingsaanvallen en prestatieproblemen." },
            { L"10. Hoe wijzig ik de update-instellingen?", L"Gebruik 'Instellingen wijzigen' in Windows Update of open Instellingen > Windows Update." },
        } },
    };
    const std::wstring code = CurrentLanguage();
    for (const auto& faq : kFaqs) {
        if (code == faq.code) return faq.entries;
    }
    return kFaqs[0].entries;
}

// Applies bold to each question line in the RichEdit body. The text is built
// as "<N>. Question\r\nAnswer\r\n\r\n..." and each question range is selected
// and re-formatted with CFE_BOLD.
static void PopulateFaqRichEdit(HWND rich, const WuFaqEntry* entries) {
    std::wstring text;
    struct Range { LONG start; LONG end; };
    std::vector<Range> boldRanges;
    boldRanges.reserve(10);
    for (int index = 0; index < 10; ++index) {
        if (index) text += L"\r\n\r\n";
        const LONG start = static_cast<LONG>(text.size());
        text += entries[index].q;
        boldRanges.push_back({ start, static_cast<LONG>(text.size()) });
        text += L"\r\n";
        text += entries[index].a;
    }
    SetWindowTextW(rich, text.c_str());

    // First make sure the whole text is regular (not bold), then apply bold
    // ONLY to the question ranges - the answers must stay in normal weight.
    CHARFORMAT2W regular{};
    regular.cbSize = sizeof(regular);
    regular.dwMask = CFM_BOLD;
    regular.dwEffects = 0;  // clear bold
    SendMessageW(rich, EM_SETSEL, 0, -1);
    SendMessageW(rich, EM_SETCHARFORMAT, SCF_SELECTION,
                 reinterpret_cast<LPARAM>(&regular));

    CHARFORMAT2W bold{};
    bold.cbSize = sizeof(bold);
    bold.dwMask = CFM_BOLD;
    bold.dwEffects = CFE_BOLD;
    for (const auto& range : boldRanges) {
        SendMessageW(rich, EM_SETSEL, range.start, range.end);
        SendMessageW(rich, EM_SETCHARFORMAT, SCF_SELECTION,
                     reinterpret_cast<LPARAM>(&bold));
    }
    // Clear the selection so the last question is not left highlighted.
    SendMessageW(rich, EM_SETSEL, static_cast<WPARAM>(-1), 0);
    SendMessageW(rich, EM_SCROLLCARET, 0, 0);
}

static INT_PTR CALLBACK WuFaqDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_INITDIALOG: {
            RegisterWuFaqDialog(hwnd);

            // Window icon + header logo (GDI+ HighQualityBicubic).
            g_wuFaqIconBig = CreateAppletLogoIconBicubic(
                IsWindows81Skin(), GetSystemMetrics(SM_CXICON),
                GetSystemMetrics(SM_CYICON));
            if (g_wuFaqIconBig)
                SendMessageW(hwnd, WM_SETICON, ICON_BIG,
                             reinterpret_cast<LPARAM>(g_wuFaqIconBig));
            g_wuFaqIconSmall = CreateAppletLogoIconBicubic(
                IsWindows81Skin(), GetSystemMetrics(SM_CXSMICON),
                GetSystemMetrics(SM_CYSMICON));
            if (g_wuFaqIconSmall)
                SendMessageW(hwnd, WM_SETICON, ICON_SMALL,
                             reinterpret_cast<LPARAM>(g_wuFaqIconSmall));
            g_wuFaqHeaderIcon = CreateAppletLogoIconBicubic(
                IsWindows81Skin(), 32, 32);
            g_wuFaqTitleFont = WuCreateHeaderFont();

            // Read-only RichEdit body below the header (bold questions),
            // with a clean 3D edge. All controls are laid out from the client
            // rectangle in pixels, so nothing can drift from the header or the
            // Close button (mixing dialog units and pixels was what made the
            // layout look wrong).
            LoadLibraryW(L"riched20.dll");
            RECT rc{};
            GetClientRect(hwnd, &rc);
            const int cw = rc.right - rc.left;
            const int ch = rc.bottom - rc.top;
            const int bodyTop = 62;
            const int bodyBottom = ch - 36;
            const int bodyHeight = bodyBottom > bodyTop ? bodyBottom - bodyTop : 120;
            HWND rich = CreateWindowExW(
                WS_EX_CLIENTEDGE, L"RichEdit20W", L"",
                WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY |
                    WS_VSCROLL | ES_AUTOVSCROLL,
                14, bodyTop, cw - 28, bodyHeight,
                hwnd, reinterpret_cast<HMENU>(kWuCtlFaqBody),
                GetModuleHandleW(nullptr), nullptr);
            if (rich) {
                // Segoe UI (the dialog font), not the default GUI font.
                WuApplyDialogFont(hwnd, rich);
                SendMessageW(rich, EM_SETBKGNDCOLOR, 0, RGB(255, 255, 255));
                SendMessageW(rich, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN,
                             MAKELPARAM(8, 8));
                PopulateFaqRichEdit(rich, SelectFaqEntries());
            }

            // Close button at the bottom right, from the client rect (same
            // approach as the check/result windows, so it always sits neatly
            // below the RichEdit).
            const wchar_t* closeText = EmbeddedMuiString(237);  // "&Close"
            if (!closeText) closeText = L"Close";
            const std::wstring closeLabel = StripAmpersand(closeText);
            const int btnW = 68;
            const int btnH = 23;
            HWND btnClose = CreateWindowExW(
                0, L"BUTTON", closeLabel.c_str(),
                WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP,
                cw - 12 - btnW, ch - btnH - 10, btnW, btnH, hwnd,
                reinterpret_cast<HMENU>(IDOK),
                GetModuleHandleW(nullptr), nullptr);
            WuApplyDialogFont(hwnd, btnClose);

            // Center the window over the Control Panel window that opened it.
            if (g_wuFaqParent && IsWindow(g_wuFaqParent)) {
                RECT parentRect{};
                RECT dlgRect{};
                GetWindowRect(g_wuFaqParent, &parentRect);
                GetWindowRect(hwnd, &dlgRect);
                const int w = dlgRect.right - dlgRect.left;
                const int h = dlgRect.bottom - dlgRect.top;
                const int x = parentRect.left + ((parentRect.right - parentRect.left) - w) / 2;
                const int y = parentRect.top + ((parentRect.bottom - parentRect.top) - h) / 2;
                SetWindowPos(hwnd, nullptr, x, y, 0, 0,
                             SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            }
            return TRUE;
        }
        case WM_ERASEBKGND:
            // White body (the header is painted in WM_PAINT).
            {
                RECT rc{};
                GetClientRect(hwnd, &rc);
                FillRect(reinterpret_cast<HDC>(wParam), &rc,
                         static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
            }
            return TRUE;
        case WM_PAINT: {
            PAINTSTRUCT ps{};
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc{};
            GetClientRect(hwnd, &rc);
            FillRect(hdc, &rc, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
            wchar_t title[128] = {};
            GetWindowTextW(hwnd, title, ARRAYSIZE(title));
            WuPaintDialogHeader(hdc, rc, 56, g_wuFaqHeaderIcon, 32,
                                g_wuFaqTitleFont, title);
            EndPaint(hwnd, &ps);
            return TRUE;
        }
        case WM_CTLCOLOREDIT:
            return reinterpret_cast<INT_PTR>(GetStockObject(WHITE_BRUSH));
        case WM_CTLCOLORSTATIC:
            return WuOnCtlColorStatic(reinterpret_cast<HDC>(wParam));
        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                DestroyWindow(hwnd);
                return TRUE;
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return TRUE;
        case WM_DESTROY:
            UnregisterWuFaqDialog(hwnd);
            if (g_wuFaqTitleFont) {
                DeleteObject(g_wuFaqTitleFont);
                g_wuFaqTitleFont = nullptr;
            }
            if (g_wuFaqHeaderIcon) {
                DestroyIcon(g_wuFaqHeaderIcon);
                g_wuFaqHeaderIcon = nullptr;
            }
            if (g_wuFaqIconBig) {
                DestroyIcon(g_wuFaqIconBig);
                g_wuFaqIconBig = nullptr;
            }
            if (g_wuFaqIconSmall) {
                DestroyIcon(g_wuFaqIconSmall);
                g_wuFaqIconSmall = nullptr;
            }
            return TRUE;
    }
    return FALSE;
}

static void ShowWuFaqDialog(HWND parent) {
    if (HWND existing = FindLiveWuFaqDialog()) {
        SetForegroundWindow(existing);
        return;
    }
    g_wuFaqParent = parent;

    BYTE* buf = new (std::nothrow) BYTE[4096];
    if (!buf) return;
    BYTE* p = buf;
    const BYTE* const bufEnd = buf + 4096;

    LPDLGTEMPLATEW pDlg = reinterpret_cast<LPDLGTEMPLATEW>(p);
    pDlg->style = DS_SETFONT | DS_MODALFRAME | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU;
    pDlg->dwExtendedStyle = 0;
    pDlg->cdit = 0;  // all controls are created in WM_INITDIALOG from pixels
    pDlg->x = 0; pDlg->y = 0;
    // Compact: gradient header + scrollable body for the ten questions.
    // Height reduced ~30% so it fits comfortably on small screens (1368x768).
    pDlg->cx = 420; pDlg->cy = 280;
    p += sizeof(DLGTEMPLATE);
    *(WORD*)p = 0; p += 2;                       // no menu
    *(WORD*)p = 0; p += 2;                       // no class
    *(WORD*)p = 0; p += 2;                       // empty title (set later)
    *(WORD*)p = 9; p += 2;                       // font point size (Segoe UI 9)
    const wchar_t kFont[] = L"Segoe UI";
    if (p + sizeof(kFont) > bufEnd) {
        delete[] buf;
        return;
    }
    memcpy(p, kFont, sizeof(kFont)); p += sizeof(kFont);

    const std::wstring title = StripAmpersand(
        EmbeddedMuiString(20024) ? EmbeddedMuiString(20024)
                                 : L"Updates: frequently asked questions");

    HWND hwnd = CreateDialogIndirectParamW(GetModuleHandleW(nullptr),
                                           reinterpret_cast<LPDLGTEMPLATE>(buf),
                                           parent, WuFaqDlgProc, 0);
    if (!hwnd) {
        Wh_Log(L"Windows Update Restorer: FAQ dialog creation FAILED (err=%u)",
               GetLastError());
    }
    delete[] buf;

    if (hwnd && IsWindow(hwnd)) {
        SetWindowTextW(hwnd, title.c_str());
        ShowWindow(hwnd, SW_SHOWNORMAL);
        SetForegroundWindow(hwnd);
    }
}

// =============================================================================
// "Check for updates" micro-feature (Win32 check window + result message)
// -----------------------------------------------------------------------------
// Clicking "Check for updates" in the sidebar opens a SMALL Win32 window with
// a light-blue header (bicubic applet logo + title) and a NATIVE progress bar
// (msctls_progress32). The DirectUI page is never touched. When the ~12-second
// check finishes, the window AUTO-CLOSES and a small result window appears
// with a personalized translated message and a "Reopen Windows Update" button.
// A check only starts if no other check is in progress.
// =============================================================================
static constexpr ULONGLONG kWuCheckDurationMs = 12000;  // 10-15 s target
static constexpr DWORD kWuCheckTimerMs = 100;           // progress animation tick
static constexpr UINT_PTR kWuCheckTimerId = 893;
static constexpr WORD kWuCtlCheckLabel = 0x77A0;
static constexpr WORD kWuCtlCheckProgress = 0x77A1;
static constexpr WORD kWuCtlResultLabel = 0x77B0;

// The check result is derived from the same simple registry state the banner
// uses. These helpers are defined later (status section); forward-declared here
// so the completion path can classify the outcome and the result window can
// show the last-install date for the result message.
static bool IsPendingWindowsUpdate();
static bool IsUpdatesAvailable();
static std::wstring ComputeLastInstallTime();

// Outcome of the last "Check for updates" run, shown in the result window.
enum WuCheckOutcome {
    kCheckNoUpdates = 0,
    kCheckUpdatesFound = 1,
    kCheckPendingRestart = 2,
};
static std::atomic<int> g_checkOutcome{kCheckNoUpdates};

static std::atomic<bool> g_checkingForUpdates{false};
static std::atomic<ULONGLONG> g_checkStartedTick{0};
static std::mutex g_checkFrameMutex;
static HWND g_checkFrame = nullptr;  // Control Panel frame used by "Reopen"

// The check and result windows share this container and the icon/font handles
// (only one of them exists at a time - the check window closes before the
// result window opens).
static std::mutex g_wuCheckDlgMutex;
static std::vector<HWND> g_wuCheckDlgs;
static HICON g_wuCheckIconBig = nullptr;
static HICON g_wuCheckIconSmall = nullptr;
static HICON g_wuCheckHeaderIcon = nullptr;  // 28x28 bicubic logo for the header
static HFONT g_wuCheckTitleFont = nullptr;

static void RegisterWuCheckDialog(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_wuCheckDlgMutex);
    g_wuCheckDlgs.push_back(hwnd);
}
static void UnregisterWuCheckDialog(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_wuCheckDlgMutex);
    auto it = std::remove(g_wuCheckDlgs.begin(), g_wuCheckDlgs.end(), hwnd);
    g_wuCheckDlgs.erase(it, g_wuCheckDlgs.end());
}
static HWND FindLiveWuCheckDialog() {
    std::lock_guard<std::mutex> lock(g_wuCheckDlgMutex);
    for (HWND hwnd : g_wuCheckDlgs)
        if (IsWindow(hwnd)) return hwnd;
    return nullptr;
}
static void CloseAllWuCheckDialogs() {
    std::vector<HWND> dialogs;
    {
        std::lock_guard<std::mutex> lock(g_wuCheckDlgMutex);
        dialogs.swap(g_wuCheckDlgs);
    }
    for (HWND hwnd : dialogs)
        if (IsWindow(hwnd))
            SendMessageTimeoutW(hwnd, WM_CLOSE, 0, 0, SMTO_ABORTIFHUNG, 5000,
                                nullptr);
}

// Translated completion messages for the result window. The first sentence
// reports the outcome of the check ("no new updates found" / "updates were
// found" / "updates need a restart"); the second sentence keeps the advice to
// reopen Windows Update if the status changed.
struct WuCheckResultTexts {
    const wchar_t* noUpdates;
    const wchar_t* updatesFound;
    const wchar_t* pendingRestart;
};
static const WuCheckResultTexts* SelectWuCheckResultTexts() {
    static const struct { const wchar_t* code; WuCheckResultTexts t; } kTexts[] = {
        { L"en", { L"No new updates were found for your PC. If the status has changed, close and reopen Windows Update to refresh the page.",
                   L"New updates were found for your PC. Close and reopen Windows Update to see them.",
                   L"Updates have been downloaded and require a restart of your PC. Close and reopen Windows Update after restarting." } },
        { L"it", { L"Non sono stati rilevati nuovi aggiornamenti per il tuo PC. Se lo stato è cambiato, chiudi e riapri Windows Update per aggiornare la pagina.",
                   L"Sono stati rilevati nuovi aggiornamenti per il tuo PC. Chiudi e riapri Windows Update per visualizzarli.",
                   L"Sono stati scaricati aggiornamenti che richiedono il riavvio del PC. Dopo il riavvio, chiudi e riapri Windows Update." } },
        { L"es", { L"No se encontraron nuevas actualizaciones para su PC. Si el estado ha cambiado, cierre y vuelva a abrir Windows Update para actualizar la página.",
                   L"Se encontraron nuevas actualizaciones para su PC. Cierre y vuelva a abrir Windows Update para verlas.",
                   L"Se descargaron actualizaciones que requieren reiniciar el PC. Después de reiniciar, cierre y vuelva a abrir Windows Update." } },
        { L"fr", { L"Aucune nouvelle mise à jour n'a été trouvée pour votre PC. Si l'état a changé, fermez puis rouvrez Windows Update pour actualiser la page.",
                   L"De nouvelles mises à jour ont été trouvées pour votre PC. Fermez puis rouvrez Windows Update pour les voir.",
                   L"Des mises à jour ont été téléchargées et nécessitent un redémarrage de votre PC. Après le redémarrage, fermez puis rouvrez Windows Update." } },
        { L"tr", { L"Bilgisayarınız için yeni güncelleştirme bulunamadı. Durum değiştiyse sayfayı güncellemek için Windows Update'i kapatıp yeniden açın.",
                   L"Bilgisayarınız için yeni güncelleştirmeler bulundu. Bunları görmek için Windows Update'i kapatıp yeniden açın.",
                   L"Güncelleştirmeler indirildi ve bilgisayarınızın yeniden başlatılması gerekiyor. Yeniden başlattıktan sonra Windows Update'i kapatıp yeniden açın." } },
        { L"ru", { L"Новых обновлений для вашего компьютера не найдено. Если состояние изменилось, закройте и снова откройте Центр обновления Windows, чтобы обновить страницу.",
                   L"Найдены новые обновления для вашего компьютера. Закройте и снова откройте Центр обновления Windows, чтобы увидеть их.",
                   L"Обновления загружены и требуют перезапуска компьютера. После перезапуска закройте и снова откройте Центр обновления Windows." } },
        { L"pt", { L"Nenhuma nova atualização foi encontrada para seu PC. Se o status mudou, feche e reabra o Windows Update para atualizar a página.",
                   L"Foram encontradas novas atualizações para seu PC. Feche e reabra o Windows Update para vê-las.",
                   L"Atualizações foram baixadas e exigem a reinicialização do PC. Após reiniciar, feche e reabra o Windows Update." } },
        { L"zh", { L"未发现适用于你电脑的新更新。如果状态已更改，请关闭并重新打开 Windows 更新以刷新页面。",
                   L"已发现适用于你电脑的新更新。请关闭并重新打开 Windows 更新以查看它们。",
                   L"更新已下载，需要重启你的电脑。重启后，请关闭并重新打开 Windows 更新。" } },
        { L"pl", { L"Nie znaleziono nowych aktualizacji dla Twojego komputera. Jeśli stan się zmienił, zamknij i ponownie otwórz Windows Update, aby odświeżyć stronę.",
                   L"Znaleziono nowe aktualizacje dla Twojego komputera. Zamknij i ponownie otwórz Windows Update, aby je zobaczyć.",
                   L"Aktualizacje zostały pobrane i wymagają ponownego uruchomienia komputera. Po ponownym uruchomieniu zamknij i ponownie otwórz Windows Update." } },
        { L"nl", { L"Er zijn geen nieuwe updates gevonden voor uw pc. Als de status is gewijzigd, sluit Windows Update en open het opnieuw om de pagina te vernieuwen.",
                   L"Er zijn nieuwe updates gevonden voor uw pc. Sluit Windows Update en open het opnieuw om ze te zien.",
                   L"Updates zijn gedownload en vereisen een herstart van uw pc. Start opnieuw op en open Windows Update daarna opnieuw." } },
    };
    const std::wstring code = CurrentLanguage();
    for (const auto& item : kTexts) {
        if (code == item.code) return &item.t;
    }
    return &kTexts[0].t;
}

// Picks the result message for the stored outcome (kCheckNoUpdates /
// kCheckUpdatesFound / kCheckPendingRestart).
static const wchar_t* SelectWuCheckResultText() {
    const WuCheckResultTexts* t = SelectWuCheckResultTexts();
    switch (g_checkOutcome.load(std::memory_order_acquire)) {
        case kCheckUpdatesFound: return t->updatesFound;
        case kCheckPendingRestart: return t->pendingRestart;
        default: return t->noUpdates;
    }
}

// Translated "Reopen Windows Update" button label.
static const wchar_t* SelectWuCheckReopenButtonText() {
    static const std::unordered_map<std::wstring, const wchar_t*> kTexts = {
        { L"en", L"Reopen Windows Update" },
        { L"it", L"Riapri Windows Update" },
        { L"es", L"Volver a abrir Windows Update" },
        { L"fr", L"Rouvrir Windows Update" },
        { L"tr", L"Windows Update'i Yeniden Aç" },
        { L"ru", L"Открыть Центр обновления Windows" },
        { L"pt", L"Reabrir Windows Update" },
        { L"zh", L"重新打开 Windows 更新" },
        { L"pl", L"Otwórz ponownie Windows Update" },
        { L"nl", L"Windows Update opnieuw openen" },
    };
    auto it = kTexts.find(CurrentLanguage());
    if (it == kTexts.end()) it = kTexts.find(L"en");
    return it->second;
}

// Ensures the msctls_progress32 window class is registered before we create the
// native progress bar.
static void EnsureProgressClassRegistered() {
    static bool done = false;
    if (done) return;
    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_PROGRESS_CLASS };
    InitCommonControlsEx(&icc);
    done = true;
}

// Posts the shell view refresh (FCIDM_REFRESH) to the Control Panel frame.
// Used by the "Reopen Windows Update" button in the result window.
static void PostControlPanelRefresh(HWND frame) {
    if (!frame || !IsWindow(frame))
        frame = FindWindowW(L"ControlPanelWindowClass", nullptr);
    if (!frame || !IsWindow(frame)) return;
    PostMessageW(frame, WM_COMMAND, MAKEWPARAM(0xA220, 0), 0);
    HWND root = GetAncestor(frame, GA_ROOT);
    if (root && root != frame && IsWindow(root))
        PostMessageW(root, WM_COMMAND, MAKEWPARAM(0xA220, 0), 0);
}

// Shared icon/font setup + centering for the check and result windows.
static void SetupWuCheckWindow(HWND hwnd, const wchar_t* title) {
    g_wuCheckIconBig = CreateAppletLogoIconBicubic(
        IsWindows81Skin(), GetSystemMetrics(SM_CXICON),
        GetSystemMetrics(SM_CYICON));
    if (g_wuCheckIconBig)
        SendMessageW(hwnd, WM_SETICON, ICON_BIG,
                     reinterpret_cast<LPARAM>(g_wuCheckIconBig));
    g_wuCheckIconSmall = CreateAppletLogoIconBicubic(
        IsWindows81Skin(), GetSystemMetrics(SM_CXSMICON),
        GetSystemMetrics(SM_CYSMICON));
    if (g_wuCheckIconSmall)
        SendMessageW(hwnd, WM_SETICON, ICON_SMALL,
                     reinterpret_cast<LPARAM>(g_wuCheckIconSmall));
    g_wuCheckHeaderIcon = CreateAppletLogoIconBicubic(
        IsWindows81Skin(), 28, 28);
    g_wuCheckTitleFont = WuCreateHeaderFont();
    SetWindowTextW(hwnd, title);

    HWND frame = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_checkFrameMutex);
        frame = g_checkFrame;
    }
    if (frame && IsWindow(frame)) {
        RECT parentRect{};
        RECT dlgRect{};
        GetWindowRect(frame, &parentRect);
        GetWindowRect(hwnd, &dlgRect);
        const int w = dlgRect.right - dlgRect.left;
        const int h = dlgRect.bottom - dlgRect.top;
        const int x = parentRect.left + ((parentRect.right - parentRect.left) - w) / 2;
        const int y = parentRect.top + ((parentRect.bottom - parentRect.top) - h) / 2;
        SetWindowPos(hwnd, nullptr, x, y, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

static void DestroyWuCheckWindowResources() {
    if (g_wuCheckTitleFont) {
        DeleteObject(g_wuCheckTitleFont);
        g_wuCheckTitleFont = nullptr;
    }
    if (g_wuCheckHeaderIcon) {
        DestroyIcon(g_wuCheckHeaderIcon);
        g_wuCheckHeaderIcon = nullptr;
    }
    if (g_wuCheckIconBig) {
        DestroyIcon(g_wuCheckIconBig);
        g_wuCheckIconBig = nullptr;
    }
    if (g_wuCheckIconSmall) {
        DestroyIcon(g_wuCheckIconSmall);
        g_wuCheckIconSmall = nullptr;
    }
}

// -----------------------------------------------------------------------------
static void ShowWuCheckResultDialog();

// -----------------------------------------------------------------------------
// Last-install date of Windows Update packages
// -----------------------------------------------------------------------------
// The legacy timestamp (Auto Update\Results\Install\LastSuccessTime) is
// deprecated/empty on many Windows 10/11 builds. The reliable source that
// works on every Windows version is the Installed-Updates list in the
// registry: each KB package installed through Windows Update appears under
// HKLM\...\Uninstall with ParentKeyName="Update" and an InstallDate
// ("YYYYMMDD"). We scan those keys and keep the most recent date. The scan is
// a few dozen quick registry reads, so it is safe to run on the dialog thread.
// -----------------------------------------------------------------------------
// Format all displayed dates/times with the current Windows user's regional
// settings. DATE_SHORTDATE respects the configured date order/separator, while
// TIME_NOSECONDS preserves the classic page's minute-level precision and honors
// the user's 12/24-hour clock and AM/PM designator.
static std::wstring FormatWindowsRegionalDate(const SYSTEMTIME& st) {
    wchar_t date[128] = {};
    if (GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &st, nullptr,
                        date, ARRAYSIZE(date), nullptr) > 0 ||
        GetDateFormatW(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, nullptr, date,
                       ARRAYSIZE(date)) > 0) {
        return date;
    }

    // The locale APIs should not fail for a validated SYSTEMTIME, but retain a
    // deterministic, unambiguous fallback rather than hiding an available date.
    swprintf_s(date, L"%04u-%02u-%02u", st.wYear, st.wMonth, st.wDay);
    return date;
}

static std::wstring FormatWindowsRegionalDateTime(const SYSTEMTIME& st) {
    std::wstring result = FormatWindowsRegionalDate(st);
    wchar_t time[128] = {};
    if (GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &st, nullptr,
                        time, ARRAYSIZE(time)) <= 0 &&
        GetTimeFormatW(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &st, nullptr, time,
                       ARRAYSIZE(time)) <= 0) {
        swprintf_s(time, L"%02u:%02u", st.wHour, st.wMinute);
    }
    if (*time) {
        if (!result.empty()) result += L" ";
        result += time;
    }
    return result;
}

static bool ParseFixedDecimalWord(const std::wstring& text, size_t position,
                                  size_t digits, WORD& value) {
    if (position > text.size() || digits > text.size() - position) return false;
    unsigned parsed = 0;
    for (size_t i = 0; i < digits; ++i) {
        const wchar_t ch = text[position + i];
        if (ch < L'0' || ch > L'9') return false;
        parsed = parsed * 10 + static_cast<unsigned>(ch - L'0');
    }
    if (parsed > 0xffff) return false;
    value = static_cast<WORD>(parsed);
    return true;
}

// WUA commonly stores REG_SZ result times as ISO-like
// "yyyy-MM-dd HH:mm:ss" or "yyyy-MM-ddTHH:mm:ss(.fff)Z" strings. Convert only
// those unambiguous forms; an unknown/already-localized string is left untouched.
static bool TryParseWuaTimestamp(const std::wstring& text, SYSTEMTIME& parsed,
                                 bool& hasTime, bool& isUtc) {
    size_t begin = 0;
    size_t end = text.size();
    while (begin < end && (text[begin] == L' ' || text[begin] == L'\t')) ++begin;
    while (end > begin && (text[end - 1] == L' ' || text[end - 1] == L'\t')) --end;
    if (end - begin < 10) return false;

    SYSTEMTIME st{};
    if (!ParseFixedDecimalWord(text, begin, 4, st.wYear) ||
        !ParseFixedDecimalWord(text, begin + 5, 2, st.wMonth) ||
        !ParseFixedDecimalWord(text, begin + 8, 2, st.wDay)) {
        return false;
    }
    const wchar_t dateSeparator = text[begin + 4];
    if ((dateSeparator != L'-' && dateSeparator != L'/') ||
        text[begin + 7] != dateSeparator) {
        return false;
    }

    hasTime = false;
    isUtc = false;
    size_t position = begin + 10;
    while (position < end && (text[position] == L' ' || text[position] == L'\t'))
        ++position;
    if (position < end) {
        if (text[position] == L'T' || text[position] == L't') {
            ++position;
        } else if (position == begin + 10) {
            return false;
        }
        if (end - position < 5 ||
            !ParseFixedDecimalWord(text, position, 2, st.wHour) ||
            text[position + 2] != L':' ||
            !ParseFixedDecimalWord(text, position + 3, 2, st.wMinute)) {
            return false;
        }
        position += 5;
        hasTime = true;
        if (position < end && text[position] == L':') {
            if (!ParseFixedDecimalWord(text, position + 1, 2, st.wSecond))
                return false;
            position += 3;
        }
        if (position < end && text[position] == L'.') {
            ++position;
            const size_t fractionalStart = position;
            while (position < end && text[position] >= L'0' &&
                   text[position] <= L'9') {
                ++position;
            }
            if (position == fractionalStart) return false;
        }
        while (position < end && (text[position] == L' ' || text[position] == L'\t'))
            ++position;
        if (position < end && (text[position] == L'Z' || text[position] == L'z')) {
            isUtc = true;
            ++position;
        } else if (end - position == 3 &&
                   (text[position] == L'U' || text[position] == L'u') &&
                   (text[position + 1] == L'T' || text[position + 1] == L't') &&
                   (text[position + 2] == L'C' || text[position + 2] == L'c')) {
            isUtc = true;
            position += 3;
        }
        while (position < end && (text[position] == L' ' || text[position] == L'\t'))
            ++position;
        if (position != end) return false;
    }

    FILETIME validation{};
    if (!SystemTimeToFileTime(&st, &validation)) return false;
    if (isUtc && hasTime) {
        SYSTEMTIME local{};
        if (!SystemTimeToTzSpecificLocalTime(nullptr, &st, &local)) return false;
        st = local;
    }
    parsed = st;
    return true;
}

static std::wstring FormatStoredWuaTimestampForDisplay(const std::wstring& text) {
    SYSTEMTIME st{};
    bool hasTime = false;
    bool isUtc = false;
    if (!TryParseWuaTimestamp(text, st, hasTime, isUtc)) return text;
    return hasTime ? FormatWindowsRegionalDateTime(st)
                   : FormatWindowsRegionalDate(st);
}

static std::wstring ComputeLastInstallDateFromUninstall() {
    struct Best {
        SYSTEMTIME st{};
        bool found = false;
    } best;

    static constexpr PCWSTR kRoots[] = {
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall",
        L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall",
    };
    for (PCWSTR root : kRoots) {
        HKEY hRoot = nullptr;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, root, 0, KEY_READ, &hRoot) !=
            ERROR_SUCCESS)
            continue;
        for (DWORD index = 0; index < 4096; ++index) {
            wchar_t subName[256] = {};
            DWORD subLen = ARRAYSIZE(subName);
            if (RegEnumKeyExW(hRoot, index, subName, &subLen, nullptr, nullptr,
                              nullptr, nullptr) != ERROR_SUCCESS)
                break;
            HKEY hSub = nullptr;
            if (RegOpenKeyExW(hRoot, subName, 0, KEY_READ, &hSub) !=
                ERROR_SUCCESS)
                continue;
            wchar_t parent[64] = {};
            DWORD type = 0;
            DWORD size = sizeof(parent);
            const bool isWuUpdate =
                RegQueryValueExW(hSub, L"ParentKeyName", nullptr, &type,
                                 reinterpret_cast<LPBYTE>(parent), &size) ==
                    ERROR_SUCCESS &&
                type == REG_SZ && _wcsicmp(parent, L"Update") == 0;
            if (isWuUpdate) {
                wchar_t install[16] = {};
                size = sizeof(install);
                if (RegQueryValueExW(hSub, L"InstallDate", nullptr, &type,
                                     reinterpret_cast<LPBYTE>(install),
                                     &size) == ERROR_SUCCESS &&
                    type == REG_SZ && wcslen(install) >= 8) {
                    const int year = (install[0] - L'0') * 1000 +
                                     (install[1] - L'0') * 100 +
                                     (install[2] - L'0') * 10 +
                                     (install[3] - L'0');
                    const int month = (install[4] - L'0') * 10 +
                                      (install[5] - L'0');
                    const int day = (install[6] - L'0') * 10 +
                                    (install[7] - L'0');
                    if (year >= 2000 && year <= 2100 && month >= 1 &&
                        month <= 12 && day >= 1 && day <= 31) {
                        SYSTEMTIME st{};
                        st.wYear = static_cast<WORD>(year);
                        st.wMonth = static_cast<WORD>(month);
                        st.wDay = static_cast<WORD>(day);
                        if (!best.found ||
                            st.wYear > best.st.wYear ||
                            (st.wYear == best.st.wYear &&
                             st.wMonth > best.st.wMonth) ||
                            (st.wYear == best.st.wYear &&
                             st.wMonth == best.st.wMonth &&
                             st.wDay > best.st.wDay)) {
                            best.st = st;
                            best.found = true;
                        }
                    }
                }
            }
            RegCloseKey(hSub);
        }
        RegCloseKey(hRoot);
    }

    if (!best.found) return L"";
    return FormatWindowsRegionalDate(best.st);
}

// Check window: small, header + native progress bar, no buttons (auto-closes).
// -----------------------------------------------------------------------------
static INT_PTR CALLBACK WuCheckDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_INITDIALOG: {
            RegisterWuCheckDialog(hwnd);
            SetupWuCheckWindow(hwnd,
                EmbeddedMuiString(1) ? EmbeddedMuiString(1) : L"Windows Update");

            RECT rc{};
            GetClientRect(hwnd, &rc);
            const int w = rc.right - rc.left;

            // "Checking for updates..." label below the 40px header.
            HWND label = CreateWindowExW(
                0, L"STATIC", L"",
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                16, 44, w - 32, 18, hwnd,
                reinterpret_cast<HMENU>(kWuCtlCheckLabel),
                GetModuleHandleW(nullptr), nullptr);
            const wchar_t* checkingText = EmbeddedMuiString(20009);
            SetWindowTextW(label, checkingText ? checkingText
                                               : L"Checking for updates...");
            WuApplyDialogFont(hwnd, label);  // Segoe UI

            // Native Win32 progress bar.
            EnsureProgressClassRegistered();
            HWND bar = CreateWindowExW(
                0, L"msctls_progress32", L"",
                WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
                16, 64, w - 32, 14, hwnd,
                reinterpret_cast<HMENU>(kWuCtlCheckProgress),
                GetModuleHandleW(nullptr), nullptr);
            if (bar) {
                SendMessageW(bar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
                SendMessageW(bar, PBM_SETBARCOLOR, 0, RGB(76, 175, 80));
                SendMessageW(bar, PBM_SETPOS, 0, 0);
            }

            SetTimer(hwnd, kWuCheckTimerId, kWuCheckTimerMs, nullptr);
            return TRUE;
        }
        case WM_ERASEBKGND: {
            RECT rc{};
            GetClientRect(hwnd, &rc);
            FillRect(reinterpret_cast<HDC>(wParam), &rc,
                     static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
            return TRUE;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps{};
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc{};
            GetClientRect(hwnd, &rc);
            FillRect(hdc, &rc, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
            wchar_t title[128] = {};
            GetWindowTextW(hwnd, title, ARRAYSIZE(title));
            WuPaintDialogHeader(hdc, rc, 40, g_wuCheckHeaderIcon, 28,
                                g_wuCheckTitleFont, title);
            EndPaint(hwnd, &ps);
            return TRUE;
        }
        case WM_CTLCOLORSTATIC:
            return WuOnCtlColorStatic(reinterpret_cast<HDC>(wParam));
        case WM_TIMER:
            if (wParam == kWuCheckTimerId) {
                const ULONGLONG now = GetTickCount64();
                const ULONGLONG start =
                    g_checkStartedTick.load(std::memory_order_acquire);
                const ULONGLONG elapsed = now >= start ? now - start : 0;
                if (elapsed >= kWuCheckDurationMs) {
                    // Finished: clear the flag, AUTO-CLOSE the window and show
                    // the personalized result message. We deliberately do NOT
                    // stamp "now" as the last-check time here: this loop only
                    // animates a progress bar and classifies the outcome from
                    // registry state that predates it (see IsPendingWindowsUpdate
                    // / IsUpdatesAvailable below) - no query actually ran, so
                    // recording a fresh timestamp would misreport a check that
                    // never happened. LastCheckForUpdatesText() continues to
                    // report Windows Update's own recorded scan time instead.
                    KillTimer(hwnd, kWuCheckTimerId);
                    // Classify the outcome from the same simple registry
                    // state the banner uses (pending restart / updates
                    // available / nothing new), then show the result window.
                    int outcome = kCheckNoUpdates;
                    if (IsPendingWindowsUpdate()) {
                        outcome = kCheckPendingRestart;
                    } else if (IsUpdatesAvailable()) {
                        outcome = kCheckUpdatesFound;
                    }
                    g_checkOutcome.store(outcome, std::memory_order_release);
                    g_checkingForUpdates.store(false, std::memory_order_release);
                    DestroyWindow(hwnd);
                    ShowWuCheckResultDialog();
                    return TRUE;
                }
                const int percent =
                    static_cast<int>((elapsed * 100ull) / kWuCheckDurationMs);
                HWND bar = GetDlgItem(hwnd, kWuCtlCheckProgress);
                if (bar && IsWindow(bar))
                    SendMessageW(bar, PBM_SETPOS, (std::min)(percent, 100), 0);
                return TRUE;
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return TRUE;
        case WM_DESTROY:
            KillTimer(hwnd, kWuCheckTimerId);
            UnregisterWuCheckDialog(hwnd);
            // Closing mid-check must let the user start a new check later.
            g_checkingForUpdates.store(false, std::memory_order_release);
            DestroyWuCheckWindowResources();
            return TRUE;
    }
    return FALSE;
}

// -----------------------------------------------------------------------------
// Result window: personalized message + "Reopen Windows Update" / Close.
// -----------------------------------------------------------------------------
static void ShowWuCheckResultDialog();

static INT_PTR CALLBACK WuCheckResultDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_INITDIALOG: {
            RegisterWuCheckDialog(hwnd);
            SetupWuCheckWindow(hwnd,
                EmbeddedMuiString(1) ? EmbeddedMuiString(1) : L"Windows Update");

            RECT rc{};
            GetClientRect(hwnd, &rc);
            const int w = rc.right - rc.left;
            const int h = rc.bottom - rc.top;

            // Personalized message (multiline + word wrap, so the outcome text
            // AND the installed-updates date paragraph are both visible).
            HWND label = CreateWindowExW(
                0, L"STATIC", L"",
                WS_CHILD | WS_VISIBLE | SS_LEFT | SS_EDITCONTROL,
                16, 42, w - 32, (h - 80 > 48 ? h - 80 : 48), hwnd,
                reinterpret_cast<HMENU>(kWuCtlResultLabel),
                GetModuleHandleW(nullptr), nullptr);
            // Personalized outcome message + the date of the last installed
            // updates. Primary source: the Installed-Updates registry list
            // (ParentKeyName=="Update" entries), which works on every Windows
            // version; fallback: the legacy Auto Update results timestamp.
            std::wstring resultMsg = SelectWuCheckResultText();
            std::wstring lastInstall = ComputeLastInstallDateFromUninstall();
            if (lastInstall.empty()) lastInstall = ComputeLastInstallTime();
            // Always show the "Updates were installed:" line: the date when it
            // is available, otherwise the universal "N/A" placeholder.
            {
                const wchar_t* installedLabel = EmbeddedMuiString(1145);
                resultMsg += L"\r\n\r\n";
                resultMsg += installedLabel ? installedLabel
                                            : L"Updates were installed:";
                resultMsg += L" ";
                resultMsg += lastInstall.empty() ? L"N/A" : lastInstall;
                // Only append a period when the message does not already end
                // with a sentence terminator (e.g. Chinese uses "。").
                const wchar_t last = resultMsg.back();
                if (last != L'.' && last != L'。' && last != L'!' &&
                    last != L'？' && last != L'?') {
                    resultMsg += L".";
                }
            }
            SetWindowTextW(label, resultMsg.c_str());
            WuApplyDialogFont(hwnd, label);  // Segoe UI

            // Two buttons side by side at the bottom right, both using Segoe
            // UI (the dialog font). Each button is sized to fit its translated
            // label - measured with the SAME font the button will render with,
            // so long labels like the Spanish "Reopen" text never clip and the
            // two buttons can never overlap.
            const wchar_t* closeText = EmbeddedMuiString(237);
            if (!closeText) closeText = L"Close";
            const std::wstring closeLabel = StripAmpersand(closeText);
            const std::wstring reopenLabel =
                StripAmpersand(SelectWuCheckReopenButtonText());
            HFONT dlgFont = reinterpret_cast<HFONT>(
                SendMessageW(hwnd, WM_GETFONT, 0, 0));
            HDC dc = GetDC(hwnd);
            HFONT oldFont = dlgFont
                                ? static_cast<HFONT>(SelectObject(dc, dlgFont))
                                : nullptr;
            auto textWidth = [&](const std::wstring& s) -> int {
                SIZE sz{};
                GetTextExtentPoint32W(dc, s.c_str(),
                                      static_cast<int>(s.size()), &sz);
                return static_cast<int>(sz.cx);
            };
            const int bwReopen = (std::max)(100, textWidth(reopenLabel) + 26);
            const int bwClose = (std::max)(76, textWidth(closeLabel) + 26);
            if (oldFont) SelectObject(dc, oldFont);
            ReleaseDC(hwnd, dc);

            const int bh = 24;
            const int gap = 8;
            const int by = h - bh - 12;
            const int bxClose = w - 12 - bwClose;
            const int bxReopen = bxClose - gap - bwReopen;
            HWND btnReopen = CreateWindowExW(
                0, L"BUTTON", reopenLabel.c_str(),
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                bxReopen, by, bwReopen, bh, hwnd,
                reinterpret_cast<HMENU>(IDOK),
                GetModuleHandleW(nullptr), nullptr);
            HWND btnClose = CreateWindowExW(
                0, L"BUTTON", closeLabel.c_str(),
                WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP,
                bxClose, by, bwClose, bh, hwnd,
                reinterpret_cast<HMENU>(IDCANCEL),
                GetModuleHandleW(nullptr), nullptr);
            WuApplyDialogFont(hwnd, btnReopen);
            WuApplyDialogFont(hwnd, btnClose);
            return TRUE;
        }
        case WM_ERASEBKGND: {
            RECT rc{};
            GetClientRect(hwnd, &rc);
            FillRect(reinterpret_cast<HDC>(wParam), &rc,
                     static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
            return TRUE;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps{};
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc{};
            GetClientRect(hwnd, &rc);
            FillRect(hdc, &rc, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
            wchar_t title[128] = {};
            GetWindowTextW(hwnd, title, ARRAYSIZE(title));
            WuPaintDialogHeader(hdc, rc, 40, g_wuCheckHeaderIcon, 28,
                                g_wuCheckTitleFont, title);
            EndPaint(hwnd, &ps);
            return TRUE;
        }
        case WM_CTLCOLORSTATIC:
            return WuOnCtlColorStatic(reinterpret_cast<HDC>(wParam));
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK: {
                    HWND frame = nullptr;
                    {
                        std::lock_guard<std::mutex> lock(g_checkFrameMutex);
                        frame = g_checkFrame;
                    }
                    DestroyWindow(hwnd);
                    PostControlPanelRefresh(frame);
                    return TRUE;
                }
                case IDCANCEL:
                    DestroyWindow(hwnd);
                    return TRUE;
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return TRUE;
        case WM_DESTROY:
            UnregisterWuCheckDialog(hwnd);
            DestroyWuCheckWindowResources();
            return TRUE;
    }
    return FALSE;
}

// Builds a small dialog (template with no controls; everything is created in
// WM_INITDIALOG from the client rect, so buttons can never overlap).
static HWND CreateWuCheckTemplateDialog(HWND frame, int cx, int cy,
                                        DLGPROC proc) {
    BYTE* buf = new (std::nothrow) BYTE[4096];
    if (!buf) return nullptr;
    BYTE* p = buf;
    const BYTE* const bufEnd = buf + 4096;

    LPDLGTEMPLATEW pDlg = reinterpret_cast<LPDLGTEMPLATEW>(p);
    pDlg->style = DS_SETFONT | DS_MODALFRAME | DS_CENTER | WS_POPUP |
                  WS_CAPTION | WS_SYSMENU;
    pDlg->dwExtendedStyle = 0;
    pDlg->cdit = 0;
    pDlg->x = 0; pDlg->y = 0;
    pDlg->cx = static_cast<short>(cx);
    pDlg->cy = static_cast<short>(cy);
    p += sizeof(DLGTEMPLATE);
    *(WORD*)p = 0; p += 2;                       // no menu
    *(WORD*)p = 0; p += 2;                       // no class
    *(WORD*)p = 0; p += 2;                       // empty title (set later)
    *(WORD*)p = 9; p += 2;                       // font point size (Segoe UI 9)
    const wchar_t kFont[] = L"Segoe UI";
    if (p + sizeof(kFont) > bufEnd) {
        delete[] buf;
        return nullptr;
    }
    memcpy(p, kFont, sizeof(kFont)); p += sizeof(kFont);

    HWND hwnd = CreateDialogIndirectParamW(
        GetModuleHandleW(nullptr), reinterpret_cast<LPDLGTEMPLATE>(buf),
        frame, proc, 0);
    delete[] buf;
    return hwnd;
}

static void ShowWuCheckResultDialog() {
    HWND hwnd = CreateWuCheckTemplateDialog(nullptr, 320, 96,
                                            WuCheckResultDlgProc);
    if (!hwnd) {
        Wh_Log(L"Windows Update Restorer: result dialog creation FAILED (err=%u)",
               GetLastError());
        return;
    }
    ShowWindow(hwnd, SW_SHOWNORMAL);
    SetForegroundWindow(hwnd);
}

// Starts the "Check for updates" flow: opens the small Win32 window with the
// native progress bar. Only runs if no check (or result) window is open.
static void StartWuUpdateCheck(HWND host) {
    if (g_stopping.load(std::memory_order_acquire)) return;
    if (FindLiveWuCheckDialog()) return;

    bool expected = false;
    if (!g_checkingForUpdates.compare_exchange_strong(expected, true)) return;
    g_checkStartedTick.store(GetTickCount64());

    HWND frame = host;
    if (!frame || !IsWindow(frame))
        frame = FindWindowW(L"ControlPanelWindowClass", nullptr);
    {
        std::lock_guard<std::mutex> lock(g_checkFrameMutex);
        g_checkFrame = frame;
    }

    HWND hwnd = CreateWuCheckTemplateDialog(frame, 300, 56, WuCheckDlgProc);
    if (!hwnd) {
        Wh_Log(L"Windows Update Restorer: check dialog creation FAILED (err=%u)",
               GetLastError());
        g_checkingForUpdates.store(false, std::memory_order_release);
        return;
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);
    SetForegroundWindow(hwnd);
}



// The old hand-built WindhawkVistaNavigationPane was removed. Navigation is
// supplied by Windows through the native ControlPanelNavLinks property bag.



// -----------------------------------------------------------------------------
// Windows Update service availability
// -----------------------------------------------------------------------------
// The classic page is restored on systems where the legacy service is broken,
// but on systems where Windows Update is actually running we should NOT show the
// obsolete "service not available" notice. We detect this by checking the
// Windows Update service (wuauserv): if it exists and is not disabled, updates
// can still be processed, so the warning is suppressed. If the service is
// missing (uninstalled) or disabled, the notice is shown. The result is cached
// briefly so we do not hit the service manager on every page render.
static std::atomic<bool> g_wuAvailable{false};
static std::atomic<ULONGLONG> g_wuCheckedTick{static_cast<ULONGLONG>(-1)}; // -1 = never probed yet
static constexpr ULONGLONG kWuCheckIntervalMs = 5000;

// RAII wrapper for Service Control Manager handles. This keeps the service
// detection path exception/early-return safe and avoids repeating
// CloseServiceHandle on every branch.
class ScopedServiceHandle {
public:
    ScopedServiceHandle() = default;
    explicit ScopedServiceHandle(SC_HANDLE handle) : handle_(handle) {}
    ~ScopedServiceHandle() { Reset(); }

    ScopedServiceHandle(const ScopedServiceHandle&) = delete;
    ScopedServiceHandle& operator=(const ScopedServiceHandle&) = delete;

    ScopedServiceHandle(ScopedServiceHandle&& other) noexcept
        : handle_(other.Release()) {}
    ScopedServiceHandle& operator=(ScopedServiceHandle&& other) noexcept {
        if (this != &other) Reset(other.Release());
        return *this;
    }

    bool IsValid() const { return handle_ != nullptr; }
    SC_HANDLE Get() const { return handle_; }

    SC_HANDLE Release() {
        SC_HANDLE result = handle_;
        handle_ = nullptr;
        return result;
    }

    void Reset(SC_HANDLE handle = nullptr) {
        if (handle_) CloseServiceHandle(handle_);
        handle_ = handle;
    }

private:
    SC_HANDLE handle_ = nullptr;
};

static bool ProbeWindowsUpdateServiceAvailable() {
    // Called only by the setup worker; SCM RPC is not allowed on the render path.
    const ULONGLONG now = GetTickCount64();
    const ULONGLONG last = g_wuCheckedTick.load();
    if (last != static_cast<ULONGLONG>(-1) && now - last < kWuCheckIntervalMs)
        return g_wuAvailable.load();

    bool available = false;
    ScopedServiceHandle scm(OpenSCManagerW(nullptr, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT));
    if (scm.IsValid()) {
        ScopedServiceHandle svc(OpenServiceW(scm.Get(), L"wuauserv", SERVICE_QUERY_CONFIG));
        if (svc.IsValid()) {
            DWORD needed = 0;
            if (!QueryServiceConfigW(svc.Get(), nullptr, 0, &needed) &&
                GetLastError() == ERROR_INSUFFICIENT_BUFFER && needed != 0) {
                std::vector<BYTE> buffer(needed);
                if (QueryServiceConfigW(svc.Get(), reinterpret_cast<QUERY_SERVICE_CONFIGW*>(buffer.data()),
                                        needed, &needed)) {
                    const auto* cfg = reinterpret_cast<const QUERY_SERVICE_CONFIGW*>(buffer.data());
                    // Available unless explicitly disabled.
                    available = (cfg->dwStartType != SERVICE_DISABLED);
                }
            }
        }
    }

    g_wuAvailable.store(available);
    g_wuCheckedTick.store(now);
    return available;
}

static bool IsWindowsUpdateServiceAvailable() {
    // Rendering must not make an SCM RPC. The setup worker publishes the probe;
    // until it finishes, preserve the optimistic state used by the native page.
    const bool probed = g_cachedWuServiceProbed.load(std::memory_order_acquire);
    return !probed || g_cachedWuAvailable.load(std::memory_order_acquire);
}

// -----------------------------------------------------------------------------
// Pending-update detection (mirrors the Win7 Action Center Recreation mod)
// -----------------------------------------------------------------------------
// Like that mod, we do NOT run a full WUA COM search (which can be slow or fail
// on these systems). Instead we read the standard registry flags that Windows
// sets when updates have been downloaded and a reboot is required to apply them.
// Returns true if there are pending updates that need a restart.
static bool IsPendingWindowsUpdate() {    // Key 1: CBS / component-based servicing reboot pending.
    {
        HKEY hKey = nullptr;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Component Based Servicing\\RebootPending",
                0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return true;
        }
    }
    // Key 2: Windows Update auto-update reboot required.
    {
        HKEY hKey = nullptr;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\RebootRequired",
                0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return true;
        }
    }
    return false;
}

// Returns true when Windows Update has downloaded/awaiting-install updates that
// do not (yet) require a reboot. We do NOT treat mere existence of the Auto
// Update\Results\Download / \Install keys as "updates available" (they exist on
// essentially every install). Instead we require a real staged result: a non-empty
// LastSuccessTime or Result value recorded inside one of those keys. Only
// meaningful when Windows Update is available and no reboot is pending.
// Forward declaration (defined below).
static std::wstring ReadWuaResultString(const wchar_t* subkey, const wchar_t* valueName);

static bool ResultKeyHasStagedUpdate(const wchar_t* subkey) {
    const wchar_t* values[] = { L"LastSuccessTime", L"Success", L"Result" };
    for (const wchar_t* v : values) {
        if (!ReadWuaResultString(subkey, v).empty()) return true;
    }
    return false;
}

// Reads the raw FILETIME behind a Windows Update results value (Result keys
// store this as REG_BINARY FILETIME on modern Windows), so we can compare two
// timestamps chronologically instead of just checking "is it non-empty".
// Returns 0 (as a ULONGLONG) when the value is missing or not a FILETIME.
static ULONGLONG ReadWuaResultFileTimeRaw(const wchar_t* subkey, const wchar_t* valueName) {
    HKEY hKey = nullptr;
    ULONGLONG out = 0;
    const std::wstring path =
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\Results\\"
        + std::wstring(subkey);
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, path.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD type = 0;
        FILETIME ft{};
        DWORD size = sizeof(ft);
        if (RegQueryValueExW(hKey, valueName, nullptr, &type,
                             reinterpret_cast<LPBYTE>(&ft), &size) == ERROR_SUCCESS &&
            type == REG_BINARY && size == sizeof(FILETIME)) {
            ULARGE_INTEGER u{};
            u.LowPart = ft.dwLowDateTime;
            u.HighPart = ft.dwHighDateTime;
            out = u.QuadPart;
        }
        RegCloseKey(hKey);
    }
    return out;
}

static bool IsUpdatesAvailable() {
    if (IsPendingWindowsUpdate()) return false;
    if (!ResultKeyHasStagedUpdate(L"Download") && !ResultKeyHasStagedUpdate(L"Install")) return false;
    // Both Results\Download and Results\Install are populated on essentially
    // every Windows install that has ever taken an update, so mere presence
    // (the check above) cannot tell "staged and not yet installed" apart from
    // "installed a while ago" - the latter is not something we want to flag
    // as "updates available". Compare the two timestamps: only report
    // available updates when the last successful download is strictly newer
    // than the last successful install (or there is a download but no
    // recorded install at all), i.e. something was fetched that the install
    // step has not caught up to yet.
    const ULONGLONG downloadTime = ReadWuaResultFileTimeRaw(L"Download", L"LastSuccessTime");
    if (downloadTime == 0) return false;
    const ULONGLONG installTime = ReadWuaResultFileTimeRaw(L"Install", L"LastSuccessTime");
    return downloadTime > installTime;
}

// Reads a REG_SZ value from the Windows Update results registry (Auto Update).
// Used to fill the Win7-style info lines below the status banner.
static std::wstring ReadWuaResultString(const wchar_t* subkey, const wchar_t* valueName) {
    HKEY hKey = nullptr;
    std::wstring out;
    const std::wstring path =
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\Results\\"
        + std::wstring(subkey);
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, path.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD type = 0, size = 0;
        if (RegQueryValueExW(hKey, valueName, nullptr, &type, nullptr, &size) == ERROR_SUCCESS &&
            size > 0 && size < 4096) {
            if (type == REG_SZ || type == REG_EXPAND_SZ) {
                out.resize((size + sizeof(wchar_t) - 1) / sizeof(wchar_t));  // round up to a whole wchar
                DWORD written = size;
                if (RegQueryValueExW(hKey, valueName, nullptr, &type,
                                     reinterpret_cast<LPBYTE>(&out[0]), &written) == ERROR_SUCCESS) {
                    while (!out.empty() && out.back() == L'\0') out.pop_back();
                    if (!out.empty())
                        out = FormatStoredWuaTimestampForDisplay(out);
                } else {
                    out.clear();
                }
            } else if (type == REG_BINARY && size == sizeof(FILETIME)) {
                FILETIME ft{};
                DWORD written = size;
                if (RegQueryValueExW(hKey, valueName, nullptr, &type,
                                     reinterpret_cast<LPBYTE>(&ft), &written) == ERROR_SUCCESS) {
                    SYSTEMTIME st{};
                    FILETIME local{};
                    if (FileTimeToLocalFileTime(&ft, &local) &&
                        FileTimeToSystemTime(&local, &st)) {
                        out = FormatWindowsRegionalDateTime(st);
                    }
                }
            }
        }
        RegCloseKey(hKey);
    }
    return out;
}

// Returns Windows Update's recorded scan time, falling back to the render time
// only when modern Windows has no compatible timestamp value.
static std::wstring LastCheckForUpdatesText() {
    std::lock_guard<std::mutex> lock(g_lastQueryTimeMutex);
    if (g_lastQueryTimeText.empty()) {
        g_lastQueryTimeText = ReadWuaResultString(L"Detect", L"LastSuccessTime");
        if (g_lastQueryTimeText.empty()) {
            SYSTEMTIME st{};
            GetLocalTime(&st);
            g_lastQueryTimeText = FormatWindowsRegionalDateTime(st);
        }
    }
    return g_lastQueryTimeText;
}

// The classic page used the Auto Update Results\Install registry timestamps.
// Avoid the WUA QueryHistory RPC here so mod unload can always join its worker.
static std::wstring ComputeLastInstallTime() {
    static constexpr PCWSTR values[] = {
        L"LastSuccessTime", L"Success", L"InstallTime"};
    for (PCWSTR value : values) {
        std::wstring timestamp = ReadWuaResultString(L"Install", value);
        if (!timestamp.empty()) return timestamp;
    }
    return L"";
}

// Returns the cached "Updates were installed" value computed on the background
// thread. Never does blocking work here, so it is safe to call from the render path.
static std::wstring LastInstallTimeText() {
    // A blank value is neutral and the next render uses the completed cache.
    std::lock_guard<std::mutex> lock(g_statusMutex);
    return g_lastInstallComputed ? g_cachedLastInstall : std::wstring();
}

// Background gathering (called from the setup thread): probes the Windows Update
// service and reads the registry timestamps, caching the results so the Control
// Panel UI thread never does blocking service work while rendering the page.
static void GatherBackgroundStatus() {
    // Probe the service (SCM RPC) and cache the outcome.
    const bool available = ProbeWindowsUpdateServiceAvailable();
    g_cachedWuAvailable.store(available);
    g_cachedWuServiceProbed.store(true);

    // Read timestamps without the cache mutex so renderers never wait on the worker.
    // Prefer the Installed Updates list, which remains populated on Windows 10/11;
    // retain the legacy Auto Update result timestamp as a conservative fallback.
    if (g_stopping.load()) return;
    std::wstring lastInstall = ComputeLastInstallDateFromUninstall();
    if (lastInstall.empty()) lastInstall = ComputeLastInstallTime();
    if (g_stopping.load()) return;
    {
        std::lock_guard<std::mutex> lock(g_statusMutex);
        g_cachedLastInstall = std::move(lastInstall);
        g_lastInstallComputed = true;
    }
}


// Applies the top-level Windows Update page XML patch. The settings child page
// (pageSettings) is intentionally left untouched to guarantee it always loads.
static std::wstring PatchModernWuPageXmlImpl(const std::wstring& input) {
    // The outer ControlPanelNavPane is deliberately left untouched. Links are
    // published through the pane's per-layout ControlPanelNavLinks object.

    if (!IsWindowsUpdatePageXml(input)) {
        DestroySettingsComboboxOnThisThread();
        return input;
    }

    // Settings child page (pageSettings, UIFILE 125): embeds the native Win32 ComboBox
    if (input.find(L"atom(pageSettings)") != std::wstring::npos)
        return PatchSettingsPageXml(input);

    // Strictly isolate the ComboBox: destroy it immediately on any other page
    DestroySettingsComboboxOnThisThread();

    // Keep the Windows 7/8.1 native navigation pane. Do not inject or strip any
    // DirectUI sidebar; only the document-area content is patched below.
    std::wstring withNavPane = input;

    if (withNavPane.find(L"wuamodern_best_effort") != std::wstring::npos)
        return withNavPane;

    // Place the new hub *after* the legacy warning module, in the normal white
    // document area. It deliberately does not alter the red legacy card. The
    // same anchor is reused below when only the red box is shown.
    const std::wstring module = L"<element id=\"atom(moduleAUNotConfigured)\"";
    const size_t moduleStart = withNavPane.find(module);

    const bool wuAvailable = IsWindowsUpdateServiceAvailable();

    // If Windows Update is unavailable (service disabled/uninstalled or AU not
    // configured), the recreated hub must disappear entirely and dynamically -
    // not via a manual setting toggle. The user only sees the red warning box,
    // so we replace it with our own faithful recreation that adds the blue link
    // to the Windows Update settings page directly below it.
    //
    // We cannot attach the link to the native module: wucltux re-shows/re-sizes
    // moduleAUNotConfigured at runtime and overrides whatever XML is added
    // inside it, and re-appending it pushes any sibling after it below the box.
    // Instead, collapse the native module to a zero-size element (its atom stays
    // resolvable, so no S_FALSE and no provider fallback re-materialization -
    // the same trick the hub path uses when the service is available) and render
    // our self-contained red box + link module (BuildRedBoxFallbackXml), which
    // wucltux does not touch: the link reliably stays directly under the box.
    if (!wuAvailable) {
        if (moduleStart == std::wstring::npos) return withNavPane;
        // Remove the broken legacy "Check for updates for your PC" red box
        // (moduleCheckForUpdates) when the setting is enabled: with the service
        // stopped its "Check for updates" button cannot work, and it would
        // duplicate the box shown below. Collapsing it to a zero-size element
        // keeps the atom resolvable (no S_FALSE) and renders nothing, exactly
        // like the moduleAUNotConfigured collapse above/below.
        std::wstring patched = withNavPane;
        if (g_removeLegacyBrokenOption.load()) {
            const std::wstring checkModule = L"<element id=\"atom(moduleCheckForUpdates)\"";
            const size_t checkStart = patched.find(checkModule);
            if (checkStart != std::wstring::npos) {
                size_t checkEnd = 0;
                if (FindElementEnd(patched, checkStart, checkEnd)) {
                    const std::wstring emptiedCheck =
                        L"<element id=\"atom(moduleCheckForUpdates)\" width=\"0rp\" height=\"0rp\" visible=\"false\"/>";
                    patched.replace(checkStart, checkEnd - checkStart, emptiedCheck);
                }
            }
        }

        // Always render our self-contained "Turn on automatic updating" box
        // recreation - both with "Show recreated interface" ON and OFF. The
        // native module (moduleAUNotConfigured) is collapsed to a zero-size
        // element (atom stays resolvable, no S_FALSE) because the provider
        // re-shows/re-sizes it at runtime and it is unreliable on modern
        // builds. With the recreated interface ON the blue settings link is
        // included below the box; with it OFF the box is shown without the
        // link (the user explicitly wants the "Turn on automatic updating"
        // box to be present in this state too).
        // moduleCheckForUpdates can appear before moduleAUNotConfigured. If it
        // was collapsed above, the replacement changed the XML length, so the
        // moduleStart offset computed from withNavPane is now stale. Re-find the
        // AU module in the modified string before replacing/inserting; otherwise
        // the recreated surface can be skipped (or the wrong element parsed).
        const size_t currentModuleStart = patched.find(module);
        if (currentModuleStart == std::wstring::npos) return patched;
        size_t moduleEnd = 0;
        if (!FindElementEnd(patched, currentModuleStart, moduleEnd)) return patched;
        const std::wstring emptiedModule =
            L"<element id=\"atom(moduleAUNotConfigured)\" width=\"0rp\" height=\"0rp\" visible=\"false\"/>";
        patched.replace(currentModuleStart, moduleEnd - currentModuleStart, emptiedModule);
        const size_t insertAt = currentModuleStart + emptiedModule.size();
        // The Windows-owned navigation pane remains untouched. Inject only the
        // recreated content module into the document area.
        patched.insert(insertAt, BuildRedBoxFallbackXml(g_showServiceNotice.load()));
        return patched;
    }

    // With the recreated hub disabled, leave the page and its Windows-owned
    // navigation pane exactly as they are.
    if (!g_showServiceNotice.load()) {
        return withNavPane;
    }

    if (moduleStart == std::wstring::npos) {
        // No anchor on this page (e.g. sub-pages like history) - do not inject, return as-is
        return withNavPane;
    }

    // The native Control Panel navigation pane is intentionally untouched.
    size_t recalcModuleStart = withNavPane.find(module);
    if (recalcModuleStart == std::wstring::npos) {
        return withNavPane;
    }
    std::wstring patched = withNavPane;
    size_t insertAt = recalcModuleStart;
    if (wuAvailable) {
        // When Windows Update is available and updates are applied, the native red
        // "automatic updates are off" box (moduleAUNotConfigured) is misleading and
        // must be suppressed. The DirectUI provider re-shows modules at runtime, so
        // simply adding visible="false" in XML is overridden and does NOT hide it.
        //
        // Fully erasing the <element>...</element> (previous approach) removes the
        // atom(moduleAUNotConfigured) id from the tree entirely. On Windows 11 24H2
        // the provider apparently still expects to resolve that id while finishing
        // its own setup pass; when it can't, DUISetXML returns S_FALSE (hr=1)
        // instead of S_OK, and the provider's fallback is to re-materialize the
        // native red module from its own internal template - producing the legacy
        // box duplicated alongside our recreated hub.
        //
        // Fix: keep the id present but collapse the element to an empty,
        // self-closing, zero-sized node. The atom still resolves (no S_FALSE,
        // no fallback re-show), but there is nothing left to render.
        size_t moduleEnd = 0;
        if (!FindElementEnd(patched, recalcModuleStart, moduleEnd)) return input;
        const std::wstring emptiedModule =
            L"<element id=\"atom(moduleAUNotConfigured)\" width=\"0rp\" height=\"0rp\" visible=\"false\"/>";
        patched.replace(recalcModuleStart, moduleEnd - recalcModuleStart, emptiedModule);
        insertAt = recalcModuleStart + emptiedModule.size();
    }

    std::wstring hub;
    {
        // Windows Update is available (already guaranteed above): replicate the
        // classic Windows 7 header.
        //  - A colored rectangle (green = up to date, orange = pending updates
        //    or updates available) with a shield/check icon, the "Windows Update"
        //    title and a status line ("No important updates available", etc.).
        //  - Below it, three info lines: most recent check, updates installed,
        //    and which updates you receive.
        const bool pending = IsPendingWindowsUpdate();
        // Only surface the "updates available" (amber) state when the user
        // enabled it (the setting defaults to on).
        const bool updatesAvailable = IsUpdatesAvailable() && g_showAvailableUpdates.load();
        // Icon skin: Windows 7/current uses the existing green/check and warning
        // shields. Windows 8.1 uses the supplied Windows Update icon instead
        // of those two embedded status shields. The amber available-updates
        // shield and the disabled-service/fallback notice are intentionally unchanged.
        UINT iconId = kLegacyWarningShieldIconId;
        if (updatesAvailable) {
            iconId = 105;
        } else if (IsWindows81Skin()) {
            iconId = kWindows81UpdateStatusIconId;
        } else if (!pending) {
            iconId = kUpdatesInstalledIconId;
        }
        const wchar_t* statusText = nullptr;
        const wchar_t* desc = L"";
        if (pending) {
            statusText = EmbeddedMuiString(185);       // "Pending restart"
            desc = EmbeddedMuiString(226);
        } else if (updatesAvailable) {
            statusText = EmbeddedMuiString(20022);     // "There are updates available"
            desc = EmbeddedMuiString(20023);           // "Go to Windows Settings to install them"
            if (!desc) desc = L"Go to Windows Settings to install them";
        } else {
            statusText = EmbeddedMuiString(304);       // "No important updates available"
            desc = EmbeddedMuiString(324);             // "It is recommended to use the system settings..."
        }
        if (!statusText) statusText = L"";
        if (!desc) desc = L"";
        const bool linkSettingsRecommendation =
            !pending && !updatesAvailable && g_linkSystemSettingsText.load();
        const std::wstring descXml = linkSettingsRecommendation
            ? BuildLinkedSettingsRecommendationXml()
            : BuildPlainStatusDescriptionXml(desc);

        wchar_t iconSpec[64];
        swprintf_s(iconSpec, L"icon(%u,48rp,48rp,library(shell32.dll))", iconId);

        // Info lines below the banner, stacked as a vertical column (each line is
        // a layoutpos="top" row inside the borderlayout parent, like perfcenter).
        std::wstring lastCheck = LastCheckForUpdatesText();
        std::wstring lastInstall = LastInstallTimeText();
        // "You receive updates:" + "For Windows only."
        const wchar_t* recvLabel = EmbeddedMuiString(1225);   // "You receive updates: "
        const wchar_t* winOnly = EmbeddedMuiString(187);      // "For Windows only."
        std::wstring recvVal = (winOnly ? winOnly : L"For Windows only.");

        std::wstring infoBlock;
        auto addInfoLine = [&](const wchar_t* label, const std::wstring& value) {
            // Always render the label row; append the value only when available.
            infoBlock +=
                L"<element layoutpos=\"top\" layout=\"flowlayout()\" margin=\"rect(0,5rp,0,0)\">"
                L"<element sheet=\"wuappstyle\" class=\"cp_content_text\" content=\""
                + XmlEscape(label) +
                L"\"/>";
            if (!value.empty()) {
                infoBlock +=
                    L"<element sheet=\"wuappstyle\" class=\"cp_content_text\" content=\""
                    + XmlEscape(value.c_str()) +
                    L"\"/>";
            }
            infoBlock += L"</element>";
        };
        if (!lastCheck.empty() && lastCheck.back() != L'.') lastCheck += L".";
        addInfoLine(EmbeddedMuiString(1144), lastCheck); // "Most recent check for updates:"
        // "Updates were installed:" row, always shown, with a history link.
        // The private handler keeps the classic CLSID on supported Windows 10
        // systems and opens Settings on Windows 11 or whenever that CLSID is absent.
        {
            infoBlock +=
                L"<element layoutpos=\"top\" layout=\"flowlayout()\" margin=\"rect(0,5rp,0,0)\">"
                L"<element sheet=\"wuappstyle\" class=\"cp_content_text\" content=\""
                + XmlEscape(EmbeddedMuiString(1145)) +
                L"\"/>";
            std::wstring installVal = lastInstall.empty() ? L"N/A" : lastInstall;
            if (!lastInstall.empty() && installVal.back() != L'.') installVal += L".";
            infoBlock +=
                L"<element sheet=\"wuappstyle\" class=\"cp_content_text\" content=\""
                + XmlEscape(installVal.c_str()) +
                L"\"/>";
            infoBlock +=
                L"<NavigateButton layout=\"flowlayout()\" shellexecute=\"wurestorer:history\">"
                L"<Button sheet=\"wu_cp_style\" class=\"cp_content_link\" active=\"mouse | keyboard\" content=\""
                + std::wstring(EmbeddedMuiString(74) ? EmbeddedMuiString(74) : L"View update history") +
                L"\"/></NavigateButton>";
            infoBlock += L"</element>";
        }
        {
            infoBlock +=
                L"<element layoutpos=\"top\" layout=\"flowlayout()\" margin=\"rect(0,5rp,0,0)\">"
                L"<element sheet=\"wuappstyle\" class=\"cp_content_text\" content=\""
                + XmlEscape(recvLabel) +
                L"\"/><element sheet=\"wuappstyle\" class=\"cp_content_text\" content=\""
                + XmlEscape(recvVal.c_str()) +
                L"\"/></element>";
        }

        // Footer: "Get updates for other Microsoft products." + azzurro
        // "Find out more" link (opens microsoft.com), inside a light-blue bordered box.
        {
            infoBlock +=
                L"<element layoutpos=\"top\" layout=\"borderlayout()\" bordercolor=\"RGB(163,207,245)\" "
                L"borderthickness=\"rect(1rp,1rp,1rp,1rp)\" padding=\"rect(12rp,10rp,12rp,10rp)\" margin=\"rect(0,14rp,0,0)\">"
                L"<element layoutpos=\"top\" layout=\"flowlayout(0,0,0,2)\">"
                L"<element sheet=\"wuappstyle\" class=\"cp_content_text\" content=\""
                + std::wstring(EmbeddedMuiString(20020) ? EmbeddedMuiString(20020) : L"Get updates for other Microsoft products.") +
                L"\"/>"
                L"<NavigateButton layout=\"flowlayout()\" shellexecute=\"https://www.microsoft.com\">"
                L"<Button sheet=\"wu_cp_style\" class=\"cp_content_link\" active=\"mouse | keyboard\" content=\""
                + std::wstring(EmbeddedMuiString(20021) ? EmbeddedMuiString(20021) : L"Find out more") +
                L"\"/></NavigateButton>"
                L"</element></element>";
        }

        // The status rectangle: bordered module (grey border) with a colored strip,
        // icon and a large azzurro (light-blue) status title. Everything sits in a
        // borderlayout parent so children with layoutpos="top" stack in a column.
        // The side strip is orange when there are pending updates (restart
        // pending) or available updates, green when the PC is up to date.
        std::wstring colorClass =
            (pending || updatesAvailable) ? L"orange" : L"green";
        hub =
            L"<element id=\"atom(wuamodern_best_effort)\" layoutpos=\"top\" layout=\"borderlayout()\" "
            L"margin=\"rect(12rp,14rp,12rp,0)\">"
            // --- status rectangle (grey border from wuappstyle moduleborder1) ---
            L"<element sheet=\"wuappstyle\" class=\"moduleborder1\" layoutpos=\"top\" layout=\"borderlayout()\">";
        if (colorClass == L"orange") {
            hub +=
                L"<element layoutpos=\"left\" background=\"RGB(240,145,10)\" width=\"16rp\"/>";
        } else {
            hub +=
                L"<element layoutpos=\"left\" sheet=\"wuappstyle\" class=\"security_box_gradient_"
                + colorClass +
                L"\" width=\"16rp\"/>";
        }
        hub +=
            L"<element layoutpos=\"client\" layout=\"borderlayout()\" padding=\"rect(12rp,15rp,12rp,15rp)\">"
            L"<element layoutpos=\"top\" layout=\"borderlayout()\">"
            L"<viewer layoutpos=\"left\" padding=\"rect(0,0,12rp,0)\">"
            L"<element content=\"" + std::wstring(iconSpec) + L"\"/></viewer>"
            L"<element layoutpos=\"client\" layout=\"flowlayout(1)\" contentalign=\"wrapleft\">"
            L"<element sheet=\"wuappstyle\" class=\"wuapp_content_title\" foreground=\"gtc(CONTROLPANELSTYLE,10,1,3803)\" margin=\"rect(0,-3rp,0,0)\" contentalign=\"wrapleft\" content=\""
            + XmlEscape(statusText) +
            L"\"/>"
            + descXml +
            BuildChangeWindowsUpdateSettingsLinkXml() +
            L"</element></element>"
            L"</element>"
            L"</element>"
            // --- info lines below the rectangle, each its own column row ---
            + infoBlock +
            L"</element>";
    }

    // Inject only the content hub. Windows owns the left pane and consumes the
    // ControlPanelNavLinks object published by wucltux.
    patched.insert(insertAt, hub);
    // The recreated hub owns the top-level content. Retain native atom IDs for
    // provider compatibility, but never let their original visual modules render.
    // Keep provider-owned modules intact; only the specific AU module above is
    // replaced. Collapsing all module(...) ancestors invalidates the WU page on
    // current Windows builds and produces S_FALSE.
    return patched;
}

// =============================================================================
// DirectUI page-content hooks (the native Control Panel sidebar is not modified)
// =============================================================================










// Forward declaration: the exception-safe wrapper is defined below the two
// DirectUI hooks, but both hooks call it.
static std::wstring PatchModernWuPageXml(const std::wstring& input) noexcept;

static HRESULT WU_DUI_THISCALL DUISetXMLHook(void* parser, const WCHAR* xml,
                                              HINSTANCE resourceModule,
                                              HINSTANCE hInstance) {
    if (!DUISetXMLOriginal) return E_FAIL;
    if (g_inWuXmlPatch || !xml) {
        return DUISetXMLOriginal(parser, xml, resourceModule, hInstance);
    }
    std::wstring patched = PatchModernWuPageXml(xml);
    if (patched == xml) return DUISetXMLOriginal(parser, xml, resourceModule, hInstance);
    WuXmlPatchGuard guard;
    HRESULT hr = DUISetXMLOriginal(parser, patched.c_str(), resourceModule, hInstance);
    // Never leave the host with a failed XML tree. S_FALSE is not a FAILED
    // HRESULT and is used by some DirectUI resource paths as a soft result.
    if (FAILED(hr)) {
        Wh_Log(L"Windows Update Restorer: patched XML rejected (hr=0x%08X); retrying original XML",
               static_cast<unsigned>(hr));
        hr = DUISetXMLOriginal(parser, xml, resourceModule, hInstance);
        if (FAILED(hr))
            Wh_Log(L"Windows Update Restorer: original XML also failed (hr=0x%08X)", static_cast<unsigned>(hr));
    }
    if (patched.find(L"atom(pageSettings)") != std::wstring::npos) {
        InitializeNativeSettingsCombobox(nullptr);
    } else {
        DestroySettingsComboboxOnThisThread();
    }
    return hr;
}

static std::wstring PatchModernWuPageXml(const std::wstring& input) noexcept {
    try {
        return PatchModernWuPageXmlImpl(input);
    } catch (...) {
        // Never let an allocation/parser exception cross the DirectUI boundary.
        Wh_Log(L"WUR: exception in page XML patch; returning original XML");
        return input;
    }
}

static HRESULT WU_DUI_THISCALL DUISetXMLFromResourceHook(
    void* parser, PCWSTR resourceName, PCWSTR resourceType, HMODULE resourceModule,
    HINSTANCE hInstance1, HINSTANCE hInstance2) {
    if (!DUISetXMLFromResourceOriginal) return E_FAIL;
    if (!DUISetXMLOriginal || g_inWuXmlPatch) {
        return DUISetXMLFromResourceOriginal(parser, resourceName, resourceType, resourceModule,
                                             hInstance1, hInstance2);
    }
    std::wstring xml = LoadDirectUiResourceXml(resourceModule, resourceName, resourceType);
    // Do not rely solely on a numeric resource ID: different legacy builds use
    // different XMLFILE IDs. The two stock action names identify the WU landing page.
    if (xml.empty() || !IsWindowsUpdatePageXml(xml)) {
        return DUISetXMLFromResourceOriginal(parser, resourceName, resourceType, resourceModule,
                                             hInstance1, hInstance2);
    }
    std::wstring patched = PatchModernWuPageXml(xml);
    if (patched == xml) return DUISetXMLFromResourceOriginal(parser, resourceName, resourceType, resourceModule,
                                                              hInstance1, hInstance2);
    WuXmlPatchGuard guard;
    HRESULT hr = DUISetXMLOriginal(parser, patched.c_str(), reinterpret_cast<HINSTANCE>(resourceModule), hInstance1);
    // A failed injected tree must never replace the working page. S_FALSE is
    // not an HRESULT failure and is returned by some DirectUI builds for a
    // partially handled resource.
    if (FAILED(hr)) {
        Wh_Log(L"Windows Update Restorer: patched resource XML rejected (hr=0x%08X); retrying original resource",
               static_cast<unsigned>(hr));
        hr = DUISetXMLOriginal(parser, xml.c_str(), reinterpret_cast<HINSTANCE>(resourceModule), hInstance1);
        if (FAILED(hr))
            Wh_Log(L"Windows Update Restorer: original resource XML also failed (hr=0x%08X)", static_cast<unsigned>(hr));
    }
    if (patched.find(L"atom(pageSettings)") != std::wstring::npos) {
        InitializeNativeSettingsCombobox(nullptr);
    } else {
        DestroySettingsComboboxOnThisThread();
    }
    if (hr == S_FALSE) {
        // wucltux uses S_FALSE as a soft result on some builds. Returning it can
        // make the host show "Unable to load page" despite a usable XML tree.
        return S_OK;
    }
    return hr;
}

static void InstallModernWuXmlPatchHook() {
    HMODULE dui70 = GetModuleHandleW(L"dui70.dll");
    if (!dui70) dui70 = LoadLibraryExW(L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!dui70) return;
    for (const char* name : {
#ifdef _WIN64
#endif
             "?SetXML@DUIXmlParser@DirectUI@@QEAAJPEBGPEAUHINSTANCE__@@1@Z",

             "?SetXML@DUIXmlParser@DirectUI@@QAAJPBGPAUHINSTANCE__@@1@Z" }) {
        if (FARPROC proc = GetProcAddress(dui70, name)) { DUISetXMLOriginal = reinterpret_cast<DUISetXML_t>(proc); break; }
    }
    if (!DUISetXMLOriginal) { Wh_Log(L"Windows Update Restorer: DirectUI SetXML not found"); return; }
    // Some wucltux pages call SetXML directly rather than _SetXMLFromResource.
    // Hook both paths; the thread-local guard prevents double patching.
    DUISetXML_t setXmlTarget = DUISetXMLOriginal;
    WindhawkUtils::SetFunctionHook(setXmlTarget, DUISetXMLHook, &DUISetXMLOriginal);
    for (const char* name : {
#ifdef _WIN64
#endif
             "?_SetXMLFromResource@DUIXmlParser@DirectUI@@IEAAJPEBG0PEAUHINSTANCE__@@11@Z",

             "?_SetXMLFromResource@DUIXmlParser@DirectUI@@IAEJPBG0PAUHINSTANCE__@@11@Z" }) {
        if (FARPROC proc = GetProcAddress(dui70, name)) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<DUISetXMLFromResource_t>(proc),
                                           DUISetXMLFromResourceHook,
                                           &DUISetXMLFromResourceOriginal);
            break;
        }
    }
    if (!DUISetXMLFromResourceOriginal)
        Wh_Log(L"Windows Update Restorer: DirectUI _SetXMLFromResource hook failed");
}

using XResourceProviderCreate_t = HRESULT(*)(HINSTANCE, LPCWSTR, LPCWSTR, LPCWSTR, void**);
static XResourceProviderCreate_t XResourceProviderCreateOriginal = nullptr;
static HRESULT XResourceProviderCreateHook(HINSTANCE instance, LPCWSTR resourceName,
                                           LPCWSTR resourceType, LPCWSTR stylesheetName,
                                           void** provider) {
    HINSTANCE resourceInstance = instance;
    if (IsWucltuxInstance(instance)) {
        if (HMODULE embedded = EmbeddedMuiResourceModule())
            resourceInstance = reinterpret_cast<HINSTANCE>(embedded);
    }
    return XResourceProviderCreateOriginal(resourceInstance, resourceName, resourceType,
                                           stylesheetName, provider);
}

// -----------------------------------------------------------------------------
// Shell presentation hooks for the restored Control Panel page.
// -----------------------------------------------------------------------------
// The legacy page definition uses indirect strings/icons such as
// "@wucltux.dll,-73" and "wucltux.dll,-1" for child pages (for example
// shell:::{36EEF7DB-88AD-4E81-AD49-0E313F0C35F8}\\pageSettings). On modern
// systems those shell-level lookups can fail or use the wrong icon, even though
// DirectUI's in-page strings are already supplied by our embedded MUI table.
// These hooks keep the breadcrumb/page title and page icon consistent with the
// selected skin without modifying the verified wucltux.dll payload.
static bool IsWucltuxPathString(PCWSTR path) {
    if (!path || !*path) return false;
    // No-allocation case-insensitive scan: this gates SHLoadIndirectString,
    // which runs for essentially every shell item display name and package
    // resource in the process, so building/lowercasing a std::wstring copy
    // per call here is a real hot-path cost. Same substring search as
    // before, just without the copy.
    static constexpr wchar_t kNeedle[] = L"wucltux.dll";
    static constexpr size_t kNeedleLen = ARRAYSIZE(kNeedle) - 1;
    size_t haystackLen = wcslen(path);
    if (haystackLen < kNeedleLen) return false;
    for (size_t start = 0; start <= haystackLen - kNeedleLen; ++start) {
        size_t i = 0;
        for (; i < kNeedleLen; ++i) {
            if (towlower(path[start + i]) != kNeedle[i]) break;
        }
        if (i == kNeedleLen) return true;
    }
    return false;
}

static bool TryParseWucltuxIndirectStringId(PCWSTR source, UINT& id) {
    id = 0;
    if (!IsWucltuxPathString(source)) return false;
    const wchar_t* comma = wcsrchr(source, L',');
    if (!comma || !comma[1]) return false;
    int parsed = _wtoi(comma + 1);
    if (parsed < 0) parsed = -parsed;
    if (parsed <= 0) return false;
    id = static_cast<UINT>(parsed);
    return true;
}

using SHLoadIndirectString_t = HRESULT(WINAPI*)(PCWSTR, PWSTR, UINT, void**);
static SHLoadIndirectString_t SHLoadIndirectStringOriginal = nullptr;
static HRESULT WINAPI SHLoadIndirectStringHook(PCWSTR source, PWSTR outBuf,
                                               UINT outChars, void** reserved) {
    UINT id = 0;
    if (TryParseWucltuxIndirectStringId(source, id)) {
        if (const wchar_t* text = EmbeddedMuiString(id)) {
            if (outBuf && outChars) {
                CopyEmbeddedString(text, outBuf, static_cast<int>(outChars));
            }
            return S_OK;
        }
    }
    return SHLoadIndirectStringOriginal(source, outBuf, outChars, reserved);
}

// Forward declaration (defined below, in the icon-file helper section).
static std::wstring EnsureAppletLogoIconFile(bool windows81Skin);

static HICON LoadAppletLogoIconForShell(int size) {
    if (size <= 0) size = GetSystemMetrics(SM_CXICON);
    const std::wstring iconPath = EnsureAppletLogoIconFile(IsWindows81Skin());
    if (iconPath.empty()) return nullptr;
    return reinterpret_cast<HICON>(LoadImageW(nullptr, iconPath.c_str(), IMAGE_ICON,
                                              size, size,
                                              LR_LOADFROMFILE | LR_DEFAULTCOLOR));
}

using ExtractIconExW_t = UINT(WINAPI*)(LPCWSTR, int, HICON*, HICON*, UINT);
static ExtractIconExW_t ExtractIconExWOriginal = nullptr;
static UINT WINAPI ExtractIconExWHook(LPCWSTR file, int iconIndex,
                                      HICON* largeIcons, HICON* smallIcons,
                                      UINT icons) {
    if (IsWucltuxPathString(file)) {
        if (iconIndex == -1 || icons == 0) return 1;
        UINT loaded = 0;
        if (largeIcons) {
            largeIcons[0] = LoadAppletLogoIconForShell(GetSystemMetrics(SM_CXICON));
            if (largeIcons[0]) loaded = 1;
        }
        if (smallIcons) {
            smallIcons[0] = LoadAppletLogoIconForShell(GetSystemMetrics(SM_CXSMICON));
            if (smallIcons[0]) loaded = 1;
        }
        return loaded;
    }
    return ExtractIconExWOriginal(file, iconIndex, largeIcons, smallIcons, icons);
}

using PrivateExtractIconsW_t = UINT(WINAPI*)(LPCWSTR, int, int, int, HICON*, UINT*, UINT, UINT);
static PrivateExtractIconsW_t PrivateExtractIconsWOriginal = nullptr;
static UINT WINAPI PrivateExtractIconsWHook(LPCWSTR file, int iconIndex,
                                            int cxIcon, int cyIcon, HICON* icons,
                                            UINT* iconIds, UINT iconCount,
                                            UINT flags) {
    if (IsWucltuxPathString(file)) {
        if (iconIndex == -1 || iconCount == 0) return 1;
        const int size = cxIcon > 0 ? cxIcon : (cyIcon > 0 ? cyIcon : GetSystemMetrics(SM_CXICON));
        UINT loaded = 0;
        if (icons) {
            icons[0] = LoadAppletLogoIconForShell(size);
            if (icons[0]) loaded = 1;
        }
        if (iconIds) iconIds[0] = 0;
        return loaded;
    }
    return PrivateExtractIconsWOriginal(file, iconIndex, cxIcon, cyIcon, icons,
                                        iconIds, iconCount, flags);
}

using SHDefExtractIconW_t = HRESULT(WINAPI*)(LPCWSTR, int, UINT, HICON*, HICON*, UINT);
static SHDefExtractIconW_t SHDefExtractIconWOriginal = nullptr;
static HRESULT WINAPI SHDefExtractIconWHook(LPCWSTR iconFile, int iconIndex,
                                            UINT flags, HICON* largeIcon,
                                            HICON* smallIcon, UINT iconSize) {
    if (IsWucltuxPathString(iconFile)) {
        const int largeSize = LOWORD(iconSize) ? LOWORD(iconSize) : GetSystemMetrics(SM_CXICON);
        const int smallSize = HIWORD(iconSize) ? HIWORD(iconSize) : GetSystemMetrics(SM_CXSMICON);
        bool loaded = false;
        if (largeIcon) {
            *largeIcon = LoadAppletLogoIconForShell(largeSize);
            loaded = loaded || *largeIcon;
        }
        if (smallIcon) {
            *smallIcon = LoadAppletLogoIconForShell(smallSize);
            loaded = loaded || *smallIcon;
        }
        return loaded ? S_OK : E_FAIL;
    }
    return SHDefExtractIconWOriginal(iconFile, iconIndex, flags, largeIcon,
                                     smallIcon, iconSize);
}

static void InstallShellPresentationHooks() {
    HMODULE shlwapi = GetModuleHandleW(L"shlwapi.dll");
    if (!shlwapi) shlwapi = LoadLibraryExW(L"shlwapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (shlwapi) {
        if (void* p = reinterpret_cast<void*>(GetProcAddress(shlwapi, "SHLoadIndirectString"))) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<SHLoadIndirectString_t>(p),
                                           SHLoadIndirectStringHook,
                                           &SHLoadIndirectStringOriginal);
        }
    }

    HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
    if (!shell32) shell32 = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (shell32) {
        if (void* p = reinterpret_cast<void*>(GetProcAddress(shell32, "ExtractIconExW"))) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<ExtractIconExW_t>(p),
                                           ExtractIconExWHook,
                                           &ExtractIconExWOriginal);
        }
        if (void* p = reinterpret_cast<void*>(GetProcAddress(shell32, "SHDefExtractIconW"))) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<SHDefExtractIconW_t>(p),
                                           SHDefExtractIconWHook,
                                           &SHDefExtractIconWOriginal);
        }
        // Private "wurestorer:" command protocol for the classic option
        // selector on the settings page (DirectUI NavigateButton -> ShellExecute).
        if (void* p = reinterpret_cast<void*>(GetProcAddress(shell32, "ShellExecuteExW"))) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<ShellExecuteExW_t>(p),
                                           ShellExecuteExWHook,
                                           &ShellExecuteExWOriginal);
        }
        if (void* p = reinterpret_cast<void*>(GetProcAddress(shell32, "ShellExecuteW"))) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<ShellExecuteW_t>(p),
                                           ShellExecuteWHook,
                                           &ShellExecuteWOriginal);
        }
    }

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) user32 = LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32) {
        if (void* p = reinterpret_cast<void*>(GetProcAddress(user32, "PrivateExtractIconsW"))) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<PrivateExtractIconsW_t>(p),
                                           PrivateExtractIconsWHook,
                                           &PrivateExtractIconsWOriginal);
        }
    }
}


// -----------------------------------------------------------------------------
// In-memory Control Panel registration. This is the same conservative design as
// the Performance Information and Tools mod: no RegSetValue and no real CLSID.
// -----------------------------------------------------------------------------
static std::wstring ToLower(std::wstring text) {
    for (auto& c : text) c = towlower(c);
    return text;
}
static bool EndsWith(const std::wstring& value, const std::wstring& suffix) {
    return value.size() >= suffix.size() &&
           value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}
static bool IsRootKey(HKEY key) {
    const uintptr_t value = reinterpret_cast<uintptr_t>(key);
    return value >= 0x80000000 && value <= 0x80000004;
}
static const wchar_t* RootPathLiteral(HKEY key) {
    switch (reinterpret_cast<uintptr_t>(key)) {
        case 0x80000000: return L"HKEY_CLASSES_ROOT";
        case 0x80000001: return L"HKEY_CURRENT_USER";
        case 0x80000002: return L"HKEY_LOCAL_MACHINE";
        case 0x80000003: return L"HKEY_USERS";
        case 0x80000004: return L"HKEY_CURRENT_CONFIG";
        default: return nullptr;
    }
}

// Allocation-free text gate for the process-wide registry hooks.
static bool ContainsRelevantKeywordInsensitive(PCWSTR path) {
    if (!path) return false;
    // "shell extensions" is required for the Shell Extensions\Approved key:
    // without it that path is filtered out before it ever reaches the
    // virtualization layer, and the shell then treats the CLSID as unapproved.
    static constexpr PCWSTR needles[] = {L"clsid", L"controlpanel",
                                         L"shell extensions"};
    for (const wchar_t* at = path; *at; ++at) {
        for (PCWSTR needle : needles) {
            size_t i = 0;
            while (needle[i] && at[i] && towlower(at[i]) == needle[i]) ++i;
            if (!needle[i]) return true;
        }
    }
    return false;
}

using RegOpenKeyExW_t = decltype(&RegOpenKeyExW);
using RegOpenKeyW_t = decltype(&RegOpenKeyW);
using RegCreateKeyExW_t = decltype(&RegCreateKeyExW);
using RegCloseKey_t = decltype(&RegCloseKey);
using RegQueryValueExW_t = decltype(&RegQueryValueExW);
using RegGetValueW_t = decltype(&RegGetValueW);
using RegEnumKeyW_t = decltype(&RegEnumKeyW);
using RegEnumKeyExW_t = decltype(&RegEnumKeyExW);
using RegQueryInfoKeyW_t = decltype(&RegQueryInfoKeyW);
static RegOpenKeyExW_t RegOpenKeyExWOriginal = nullptr;
static RegOpenKeyW_t RegOpenKeyWOriginal = nullptr;
static RegCreateKeyExW_t RegCreateKeyExWOriginal = nullptr;
static RegCloseKey_t RegCloseKeyOriginal = nullptr;
static RegQueryValueExW_t RegQueryValueExWOriginal = nullptr;
static RegGetValueW_t RegGetValueWOriginal = nullptr;
static RegEnumKeyW_t RegEnumKeyWOriginal = nullptr;
static RegEnumKeyExW_t RegEnumKeyExWOriginal = nullptr;
static RegQueryInfoKeyW_t RegQueryInfoKeyWOriginal = nullptr;

// Each synthetic key is backed by a genuine handle to this per-process volatile
// key. The mod-owned parent is directly below Software; no Software\Windhawk
// intermediate key is created, which also works with portable installations.
static constexpr PCWSTR kVirtualKeyParentPath =
    L"Software\\WindhawkWindowsUpdateControlPanelRestorer";
static constexpr PCWSTR kVirtualKeyOwnerMarker = L"WindhawkOwnerPid";
static HKEY g_virtualKeyRoot = nullptr;
static std::mutex g_virtualKeyRootMutex;

enum { kKeyFlagsInformationClass = 4 };
struct KeyFlagsInformation {
    ULONG userFlags;
    ULONG keyFlags;
    ULONG controlFlags;
};
static constexpr ULONG kKeyFlagVolatile = 0x1;

static bool IsVolatileKeyHandle(HKEY key) {
    static auto ntQueryKey = reinterpret_cast<LONG(WINAPI*)(
        HANDLE, int, PVOID, ULONG, PULONG)>(
        GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryKey"));
    if (!ntQueryKey) return false;
    KeyFlagsInformation information{};
    ULONG needed = 0;
    return ntQueryKey(reinterpret_cast<HANDLE>(key),
                      kKeyFlagsInformationClass, &information,
                      sizeof(information), &needed) == 0 &&
           (information.keyFlags & kKeyFlagVolatile) != 0;
}

static bool HasCurrentVirtualKeyOwner(HKEY key) {
    DWORD value = 0;
    DWORD type = 0;
    DWORD size = sizeof(value);
    return RegQueryValueExWOriginal &&
           RegQueryValueExWOriginal(
               key, kVirtualKeyOwnerMarker, nullptr, &type,
               reinterpret_cast<BYTE*>(&value), &size) == ERROR_SUCCESS &&
           type == REG_DWORD && size == sizeof(value) &&
           value == GetCurrentProcessId();
}

static std::wstring QueryNativeRegistryPath(HKEY key) {
    static auto ntQueryKey = reinterpret_cast<LONG(WINAPI*)(
        HANDLE, int, PVOID, ULONG, PULONG)>(
        GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryKey"));
    if (!ntQueryKey || !key || IsRootKey(key)) return {};

    // KeyNameInformation (3): ULONG NameLength followed by UTF-16 bytes.
    ULONG needed = 0;
    ntQueryKey(reinterpret_cast<HANDLE>(key), 3, nullptr, 0, &needed);
    if (needed <= sizeof(ULONG) || needed > 65536) return {};
    std::vector<BYTE> buffer(needed + sizeof(wchar_t));
    if (ntQueryKey(reinterpret_cast<HANDLE>(key), 3, buffer.data(), needed,
                   &needed) != 0) {
        return {};
    }
    const ULONG nameBytes = *reinterpret_cast<const ULONG*>(buffer.data());
    if (nameBytes > needed - sizeof(ULONG) || nameBytes % sizeof(wchar_t))
        return {};
    const wchar_t* name = reinterpret_cast<const wchar_t*>(
        buffer.data() + sizeof(ULONG));
    return std::wstring(name, nameBytes / sizeof(wchar_t));
}

static std::wstring VirtualKeyLeafName() {
    return L"VirtualKeys-" + std::to_wstring(GetCurrentProcessId());
}
static std::wstring VirtualKeyPath() {
    return std::wstring(kVirtualKeyParentPath) + L"\\" + VirtualKeyLeafName();
}

static HKEY EnsureVirtualKeyRoot() {
    std::lock_guard lock(g_virtualKeyRootMutex);
    if (g_virtualKeyRoot) return g_virtualKeyRoot;
    if (!RegCreateKeyExWOriginal || !RegOpenKeyExWOriginal ||
        !RegCloseKeyOriginal || !RegQueryValueExWOriginal) {
        return nullptr;
    }

    HKEY root = nullptr;
    const std::wstring path = VirtualKeyPath();
    if (RegOpenKeyExWOriginal(HKEY_CURRENT_USER, path.c_str(), 0,
                              KEY_READ | KEY_WRITE, &root) == ERROR_SUCCESS) {
        if (IsVolatileKeyHandle(root) && HasCurrentVirtualKeyOwner(root)) {
            g_virtualKeyRoot = root;
            return root;
        }
        RegCloseKeyOriginal(root);
        root = nullptr;

        // Never adopt a persistent or foreign key at our disposable path. This
        // is best-effort cleanup: if it fails, RegCreateKeyEx below still
        // returns a usable handle to the existing key, and failing the whole
        // virtualization layer over it would silently hide the applet.
        HKEY parent = nullptr;
        if (RegOpenKeyExWOriginal(HKEY_CURRENT_USER, kVirtualKeyParentPath, 0,
                                  KEY_WRITE | DELETE, &parent) == ERROR_SUCCESS) {
            RegDeleteTreeW(parent, VirtualKeyLeafName().c_str());
            RegCloseKeyOriginal(parent);
        }
    }

    DWORD disposition = 0;
    const LSTATUS createStatus = RegCreateKeyExWOriginal(
        HKEY_CURRENT_USER, path.c_str(), 0, nullptr, REG_OPTION_VOLATILE,
        KEY_READ | KEY_WRITE, nullptr, &root, &disposition);
    if (createStatus != ERROR_SUCCESS || !root) {
        Wh_Log(L"Windows Update Restorer: could not create the volatile backing key (err=%d); "
               L"the Control Panel item cannot be virtualized",
               static_cast<int>(createStatus));
        return nullptr;
    }

    // The owner marker is defence-in-depth (the path already embeds the PID), so
    // a failure to stamp it must not disable the whole mod. Likewise the
    // volatile recheck: REG_OPTION_VOLATILE was requested, and on some systems
    // NtQueryKey's KeyFlagsInformation is not reliable enough to justify
    // refusing to run.
    const DWORD pid = GetCurrentProcessId();
    RegSetValueExW(root, kVirtualKeyOwnerMarker, 0, REG_DWORD,
                   reinterpret_cast<const BYTE*>(&pid), sizeof(pid));

    g_virtualKeyRoot = root;
    return root;
}

static void ReleaseVirtualKeyRoot() {
    HKEY root = nullptr;
    {
        std::lock_guard lock(g_virtualKeyRootMutex);
        root = g_virtualKeyRoot;
        g_virtualKeyRoot = nullptr;
    }
    if (root && RegCloseKeyOriginal) RegCloseKeyOriginal(root);
    if (!RegOpenKeyExWOriginal || !RegCloseKeyOriginal) return;

    HKEY parent = nullptr;
    if (RegOpenKeyExWOriginal(HKEY_CURRENT_USER, kVirtualKeyParentPath, 0,
                              KEY_WRITE | DELETE, &parent) == ERROR_SUCCESS) {
        RegDeleteKeyW(parent, VirtualKeyLeafName().c_str());
        RegCloseKeyOriginal(parent);
    }
    HKEY software = nullptr;
    if (RegOpenKeyExWOriginal(HKEY_CURRENT_USER, L"Software", 0, DELETE,
                              &software) == ERROR_SUCCESS) {
        RegDeleteKeyW(software, L"WindhawkWindowsUpdateControlPanelRestorer");
        RegCloseKeyOriginal(software);
    }
}

// True when a path mentions either of our CLSIDs, so we can trace every single
// registry touch the shell makes against them - successful or not.
static bool MentionsOurClsid(const std::wstring& path) {
    const std::wstring lower = ToLower(path);
    return lower.find(L"36eef7db-88ad-4e81-ad49-0e313f0c35f8") != std::wstring::npos ||
           lower.find(L"cfbc05bc-1b9e-4693-a49c-4e7181d69e0a") != std::wstring::npos;
}

class KeyTracker {
public:
    // Lock-free negative gate for hot registry calls. The counting table is
    // exact: every mutator (Track/CreateVirtual/Untrack/CloseVirtual/
    // ClearWithoutFreeing) increments or decrements the bucket count exactly
    // once per map entry, under mutex_. So a bucket reading 0 means no tracked
    // key currently hashes into it, which means `key` cannot be in paths_.
    // There are no false negatives, so we can return false immediately instead
    // of taking the shared lock on the overwhelmingly common unrelated-HKEY
    // path. (Release on the increment publishes the map insert; acquire on the
    // load synchronizes with it, so a nonzero read is a guaranteed hit.)
    bool MightContain(HKEY key) const {
        if (!key || IsRootKey(key)) return false;
        return presence_[Bucket(key)].load(std::memory_order_acquire) != 0;
    }
    bool GetPathAndFake(HKEY key, std::wstring& path, bool& isFake) const {
        if (const wchar_t* root = RootPathLiteral(key)) {
            path = root;
            isFake = false;
            return true;
        }
        std::shared_lock lock(mutex_);
        auto found = paths_.find(key);
        if (found == paths_.end()) {
            path.clear();
            isFake = false;
            return false;
        }
        path = found->second;
        isFake = fake_.count(key) != 0;
        return true;
    }
    bool IsFake(HKEY key) const {
        if (!key || IsRootKey(key)) return false;
        std::shared_lock lock(mutex_);
        return fake_.count(key) != 0;
    }
    HKEY CreateVirtual(const std::wstring& path) {
        if (!EnsureVirtualKeyRoot()) {
            return nullptr;
        }
        HKEY backing = nullptr;
        const LSTATUS backingStatus = RegOpenKeyExWOriginal(
            HKEY_CURRENT_USER, VirtualKeyPath().c_str(), 0, KEY_READ, &backing);
        if (backingStatus != ERROR_SUCCESS) {
            return nullptr;
        }
        std::unique_lock lock(mutex_);
        const bool inserted = paths_.find(backing) == paths_.end();
        paths_[backing] = path;
        fake_.insert(backing);
        if (inserted) presence_[Bucket(backing)].fetch_add(1, std::memory_order_release);
        return backing;
    }
    void Track(HKEY key, const std::wstring& path) {
        if (!key || IsRootKey(key)) return;
        // Child opens under our CLSID use keyword-free subkey names
        // ("ShellFolder", "InProcServer32", "Instance", "DefaultIcon"), so they
        // can only be virtualized through a tracked parent handle. Never drop a
        // path that mentions our CLSIDs, whatever the cheap keyword gate says.
        if (!ContainsRelevantKeywordInsensitive(path.c_str()) &&
            !MentionsOurClsid(path)) {
            return;
        }
        std::unique_lock lock(mutex_);
        const bool inserted = paths_.find(key) == paths_.end();
        paths_[key] = path;
        if (inserted) presence_[Bucket(key)].fetch_add(1, std::memory_order_release);
    }
    void Untrack(HKEY key) {
        if (!key || IsRootKey(key) || !MightContain(key)) return;
        std::unique_lock lock(mutex_);
        if (paths_.erase(key))
            presence_[Bucket(key)].fetch_sub(1, std::memory_order_release);
        fake_.erase(key);
    }
    void CloseVirtual(HKEY key) {
        bool owned = false;
        {
            std::unique_lock lock(mutex_);
            owned = fake_.erase(key) != 0;
            if (paths_.erase(key))
                presence_[Bucket(key)].fetch_sub(1, std::memory_order_release);
        }
        if (owned) RegCloseKeyOriginal(key);
    }
    void ClearWithoutFreeing() {
        std::unique_lock lock(mutex_);
        for (const auto& [key, path] : paths_)
            presence_[Bucket(key)].fetch_sub(1, std::memory_order_release);
        paths_.clear();
        fake_.clear();
    }

private:
    static constexpr size_t kPresenceBuckets = 256;
    static size_t Bucket(HKEY key) {
        const uintptr_t value = reinterpret_cast<uintptr_t>(key);
        return ((value >> 3) ^ (value >> 13) ^ (value >> 23)) &
               (kPresenceBuckets - 1);
    }

    mutable std::shared_mutex mutex_;
    std::unordered_map<HKEY, std::wstring> paths_;
    std::unordered_set<HKEY> fake_;
    mutable std::atomic<unsigned> presence_[kPresenceBuckets]{};
};
static KeyTracker g_keys;

static std::wstring g_clsidSuffix;
static std::wstring g_defaultIconSuffix;
static std::wstring g_inprocSuffix;
static std::wstring g_instanceSuffix;
static std::wstring g_propertyBagSuffix;
static std::wstring g_shellFolderSuffix;
static std::wstring g_namespaceSuffix;
static std::wstring g_providerSuffix;
static std::wstring g_providerInprocSuffix;

static void InitPaths() {
    const std::wstring clsid = ToLower(kAppletClsid);
    g_clsidSuffix = L"clsid\\" + clsid;
    g_defaultIconSuffix = g_clsidSuffix + L"\\defaulticon";
    g_inprocSuffix = g_clsidSuffix + L"\\inprocserver32";
    g_instanceSuffix = g_clsidSuffix + L"\\instance";
    g_propertyBagSuffix = g_instanceSuffix + L"\\initpropertybag";
    g_shellFolderSuffix = g_clsidSuffix + L"\\shellfolder";
    g_namespaceSuffix = L"controlpanel\\namespace\\" + clsid;
    g_providerSuffix = L"clsid\\" + ToLower(kElementProviderClsid);
    g_providerInprocSuffix = g_providerSuffix + L"\\inprocserver32";
}


enum class Node { None, Root, Icon, Inproc, Instance, Bag, ShellFolder, Namespace, Provider, ProviderInproc };
static Node Classify(const std::wstring& path) {

    const auto lower = ToLower(path);
    if (EndsWith(lower, g_namespaceSuffix)) return Node::Namespace;
    if (EndsWith(lower, g_propertyBagSuffix)) return Node::Bag;
    if (EndsWith(lower, g_instanceSuffix)) return Node::Instance;
    if (EndsWith(lower, g_shellFolderSuffix)) return Node::ShellFolder;
    if (EndsWith(lower, g_inprocSuffix)) return Node::Inproc;
    if (EndsWith(lower, g_defaultIconSuffix)) return Node::Icon;
    if (EndsWith(lower, g_clsidSuffix)) return Node::Root;
    if (EndsWith(lower, g_providerInprocSuffix)) return Node::ProviderInproc;
    if (EndsWith(lower, g_providerSuffix)) return Node::Provider;
    return Node::None;
}
static bool IsNamespaceParent(const std::wstring& path) {

    return EndsWith(ToLower(path), L"controlpanel\\namespace");
}

// HKLM\...\Shell Extensions\Approved. The INF registers the applet CLSID here,
// and an explorer.exe running with "enforce approved shell extensions" (the
// default on many managed/Enterprise systems, and the state Windows 10 21H2
// ships with for Control Panel namespace items) refuses to instantiate a
// namespace CLSID that is absent from it. The entry is enumerated successfully
// but never bound, which is exactly the "entry injected, nothing appears"
// symptom. Serving this value is what makes the item actually materialize.
static bool IsApprovedKey(const std::wstring& path) {
    return EndsWith(ToLower(path), L"shell extensions\\approved");
}
static bool IsTarget(const std::wstring& path) { return Classify(path) != Node::None; }


static LSTATUS PutString(LPBYTE data, LPDWORD bytes, const std::wstring& text) {

    if (!bytes) return ERROR_INVALID_PARAMETER;
    DWORD needed = static_cast<DWORD>((text.size() + 1) * sizeof(wchar_t));
    if (!data) { *bytes = needed; return ERROR_SUCCESS; }
    if (*bytes < needed) { *bytes = needed; return ERROR_MORE_DATA; }
    memcpy(data, text.c_str(), needed); *bytes = needed; return ERROR_SUCCESS;
}
static LSTATUS PutDword(LPBYTE data, LPDWORD bytes, DWORD value) {
    if (!bytes) return ERROR_INVALID_PARAMETER;
    if (!data) { *bytes = sizeof(value); return ERROR_SUCCESS; }
    if (*bytes < sizeof(value)) { *bytes = sizeof(value); return ERROR_MORE_DATA; }
    *reinterpret_cast<DWORD*>(data) = value; *bytes = sizeof(value); return ERROR_SUCCESS;
}
static std::wstring ShdocvwPath() {
    wchar_t system[MAX_PATH] = {};
    GetSystemDirectoryW(system, ARRAYSIZE(system));
    return std::wstring(system) + L"\\shdocvw.dll";
}

static std::mutex g_appletLogoIconMutex;
static std::wstring g_appletLogoIconPaths[2];
static bool g_appletLogoIconInitialized[2] = {false, false};

static bool FileHasExpectedSize(const std::wstring& path, DWORD expectedSize) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER size{};
    const bool ok = GetFileSizeEx(file, &size) && size.QuadPart == expectedSize;
    CloseHandle(file);
    return ok;
}

static bool WriteBinaryFile(const std::wstring& path, const std::vector<BYTE>& data) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    const bool ok = WriteFile(file, data.data(), static_cast<DWORD>(data.size()), &written, nullptr) &&
                    written == static_cast<DWORD>(data.size());
    CloseHandle(file);
    return ok;
}

static std::wstring EnsureAppletLogoIconFile(bool windows81Skin) {
    std::lock_guard<std::mutex> lock(g_appletLogoIconMutex);
    const size_t index = windows81Skin ? 1 : 0;
    if (g_appletLogoIconInitialized[index]) return g_appletLogoIconPaths[index];
    g_appletLogoIconInitialized[index] = true;

    const std::wstring& dir = StoreDir();
    if (dir.empty()) return L"";
    const wchar_t* fileName = windows81Skin ? kAppletLogoWin81FileName
                                            : kAppletLogoWin7FileName;
    const char* base64 = windows81Skin ? kAppletLogoWin81IcoBase64
                                      : kAppletLogoWin7IcoBase64;
    std::vector<BYTE> data = DecodeBase64Icon(base64);
    if (data.empty()) return L"";
    const std::wstring path = dir + L"\\" + fileName;
    if (!FileHasExpectedSize(path, static_cast<DWORD>(data.size())) &&
        !WriteBinaryFile(path, data)) {
        Wh_Log(L"Windows Update Restorer: failed to write applet logo icon %s (err=%u)",
               fileName, GetLastError());
        return L"";
    }
    g_appletLogoIconPaths[index] = path;
    return path;
}

static std::wstring AppletDefaultIconValue(const std::wstring& fallbackPayload) {
    const std::wstring iconPath = EnsureAppletLogoIconFile(IsWindows81Skin());
    if (iconPath.empty()) return fallbackPayload + L",-1";
    return L"\"" + iconPath + L"\",0";
}

// Deletes one mod-generated file, logging (but never failing on) a file that is
// still mapped by another process. Nothing machine-wide is scheduled: a locked
// file is simply retried by the sweep on a later load/unload.
static bool DeleteGeneratedFile(const std::wstring& path) {
    if (DeleteFileW(path.c_str())) return true;
    const DWORD error = GetLastError();
    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) return true;
    Wh_Log(L"Windows Update Restorer: could not delete %s (err=%u, still in use); will retry on a later load/unload",
           path.c_str(), error);
    return false;
}

static void CleanupAppletLogoIconFiles() {
    const std::wstring dir = StoreDir();
    if (dir.empty()) return;
    DeleteGeneratedFile(dir + L"\\" + kAppletLogoWin7FileName);
    DeleteGeneratedFile(dir + L"\\" + kAppletLogoWin81FileName);
    std::lock_guard<std::mutex> lock(g_appletLogoIconMutex);
    for (size_t index = 0; index < 2; ++index) {
        g_appletLogoIconPaths[index].clear();
        g_appletLogoIconInitialized[index] = false;
    }
}


static void CleanupControlPanelTasksXmlFile() {
    const std::wstring dir = StoreDir();
    if (dir.empty()) return;
    DeleteGeneratedFile(dir + L"\\" + kAppletTasksXmlFileName);
}

// Sweep of leftovers from processes that were killed without ever running
// Wh_ModUninit (explorer.exe restarts, logoff, crashes). Called at load time,
// which is exactly the moment those files are no longer mapped, satisfying the
// "retry on next start" half of the cleanup contract. The verified payload
// itself is deliberately kept: it is the expensive, network-dependent artifact
// and reusing it is what lets the mod work offline on later runs.
static void SweepStaleGeneratedFiles() {
    const std::wstring dir = StoreDir();
    if (dir.empty()) return;
    unsigned removed = 0;
    // Partial downloads only; the verified wucltux.dll is intentionally cached.
    static constexpr PCWSTR kStalePatterns[] = {L"\\*.tmp", L"\\wucltux.dll.tmp"};
    for (PCWSTR pattern : kStalePatterns) {
        WIN32_FIND_DATAW findData{};
        HANDLE find = FindFirstFileW((dir + pattern).c_str(), &findData);
        if (find == INVALID_HANDLE_VALUE) continue;
        do {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
            if (DeleteGeneratedFile(dir + L"\\" + findData.cFileName)) ++removed;
        } while (FindNextFileW(find, &findData));
        FindClose(find);
    }
    if (removed)
        Wh_Log(L"Windows Update Restorer: removed %u stale temporary file(s) from a previous session", removed);
}



static bool ProvideValue(const std::wstring& path, const std::wstring& name,
                         LPDWORD type, LPBYTE data, LPDWORD bytes, LSTATUS& result) {
    // Publish the lightweight Control Panel metadata immediately, even while
    // the setup worker is still validating/loading the payload. Explorer can
    // enumerate and cache applets very early in process startup; waiting for the
    // MUI build here makes the entry miss that one-time enumeration pass.
    //
    // Everything that does not strictly need wucltux.dll (display name, icon,
    // InfoTip, category, task links, shell-folder attributes) is answered from
    // constants, so the Control Panel item appears and is browsable even with no
    // network at all. Only ResourceDLL and the provider's InProcServer32 are
    // withheld until the payload is verified.
    const std::wstring* payload = g_dllPath.load(std::memory_order_acquire);
    const bool payloadReady = payload && !payload->empty() && g_verified.load();
    // Approved is keyed by CLSID *value name*, not by a subkey, so it is handled
    // before the node classification below.
    if (IsApprovedKey(path)) {
        if (_wcsicmp(name.c_str(), kAppletClsid) == 0) {
            if (type) *type = REG_SZ;
            result = PutString(data, bytes, kDisplayName);
            return true;
        }
        if (_wcsicmp(name.c_str(), kElementProviderClsid) == 0) {
            if (type) *type = REG_SZ;
            result = PutString(data, bytes, kDisplayName);
            return true;
        }
        return false;
    }

    const Node node = Classify(path);
    std::wstring value;
    DWORD number = 0;
    bool isDword = false;
    switch (node) {
        case Node::Namespace:
            if (name.empty()) value = kDisplayName; else return false;
            break;
        case Node::Root:
            if (name.empty()) value = kDisplayName;
            else if (name == L"LocalizedString")
                value = payloadReady ? L"@" + *payload + L",-1" : kDisplayName;
            else if (name == L"InfoTip") value = InfoTipForLanguage();
            else if (name == L"System.ApplicationName") value = kApplicationName;
            // Requested right after the display name during enumeration. The
            // shell uses it to order the item in the Control Panel grid; the
            // real Windows Update applet ships index 5.
            else if (name == L"SortOrderIndex") { isDword = true; number = 5; }
            // Probed by the shell right after binding. The stock Control Panel
            // applets define them; leaving them unanswered makes the item look
            // like a plain COM object rather than a namespace folder.
            else if (name == L"NoRecentDocs") value.clear();
            else if (name == L"NoStaticDefaultVerb") value.clear();
            else if (name == L"System.ControlPanel.Category") value = L"5,10";
            else if (name == L"System.Software.TasksFileUrl") {
                // Do not publish the separate task-file mechanism. The one
                // visible Windows pane is supplied by ControlPanelNavLinks from
                // wucltux and redirected by PSPropertyBag_WriteUnknownHook.
                return false;
            }
            else return false;
            break;
        case Node::Icon:
            if (!name.empty()) return false;
            value = AppletDefaultIconValue(payloadReady ? *payload : ShdocvwPath());
            break;
        case Node::Inproc:
            if (name.empty()) value = ShdocvwPath();
            else if (name == L"ThreadingModel") value = L"Apartment";
            else return false;
            break;
        case Node::Instance:
            if (name != L"CLSID") return false;
            value = kLayoutFolderClsid; break;
        case Node::Bag:
            if (name == L"ResourceDLL") {
                if (!payloadReady) return false;
                value = *payload;
            } else if (name == L"ResourceID") {
                isDword = true;
                number = kInitResourceId;
            } else return false;
            break;
        case Node::ShellFolder:
            if (name == L"Attributes") { isDword = true; number = kShellFolderAttributes; }
            else if (name == L"WantsParseDisplayName") value.clear();
            else return false;
            break;
        case Node::Provider:
            if (!name.empty()) return false;
            value.clear();
            break;
        case Node::ProviderInproc:
            // The element provider's server path is the one value that is
            // meaningless without the payload. Never dereference `payload`
            // unguarded: registration goes live before the background download
            // finishes, so this is routinely reached with no payload yet.
            if (name.empty()) {
                if (!payloadReady) return false;
                value = *payload;
            } else if (name == L"ThreadingModel") value = L"Apartment";
            else return false;
            break;
        default: return false;
    }
    if (type) *type = isDword ? REG_DWORD : REG_SZ;
    result = isDword ? PutDword(data, bytes, number) : PutString(data, bytes, value);
    return true;
}




static bool WantsWrite(REGSAM access) {
    return (access & (KEY_SET_VALUE | KEY_CREATE_SUB_KEY | KEY_CREATE_LINK)) != 0;
}

static LSTATUS OpenVirtual(HKEY key, LPCWSTR subKey, DWORD options,
                           REGSAM access, PHKEY out) {
    if (!out) return ERROR_INVALID_PARAMETER;

    const bool textRelevant = ContainsRelevantKeywordInsensitive(subKey);
    if (IsRootKey(key) && !textRelevant)
        return RegOpenKeyExWOriginal(key, subKey, options, access, out);
    if (!IsRootKey(key) && !textRelevant && !g_keys.MightContain(key))
        return RegOpenKeyExWOriginal(key, subKey, options, access, out);

    try {
        std::wstring full;
        bool isFake = false;
        g_keys.GetPathAndFake(key, full, isFake);
        if (subKey && *subKey) {
            if (!full.empty()) full += L"\\";
            full += subKey;
        }
        if (isFake) {
            if (!g_registrationReady.load() || !IsTarget(full))
                return ERROR_FILE_NOT_FOUND;
            if (WantsWrite(access)) return ERROR_ACCESS_DENIED;
            HKEY virtualKey = g_keys.CreateVirtual(full);
            if (!virtualKey) return ERROR_OUTOFMEMORY;
            *out = virtualKey;
            return ERROR_SUCCESS;
        }

        LSTATUS status = RegOpenKeyExWOriginal(key, subKey, options, access, out);
        if (status == ERROR_SUCCESS && *out) {
            // Track real keys at virtualized paths so their child opens and
            // value reads continue through the lightweight virtualization path.
            g_keys.Track(*out, full);
        } else if (status == ERROR_FILE_NOT_FOUND && g_registrationReady.load() &&
                   IsTarget(full)) {
            if (WantsWrite(access)) return ERROR_ACCESS_DENIED;
            HKEY virtualKey = g_keys.CreateVirtual(full);
            if (!virtualKey) return ERROR_OUTOFMEMORY;
            *out = virtualKey;
            return ERROR_SUCCESS;
        }
        return status;
    } catch (...) {
        return ERROR_NOT_ENOUGH_MEMORY;
    }
}

// Small negative cache of registry handles that were already resolved and are
// NOT the ControlPanel\NameSpace parent. RegQueryInfoKeyW runs constantly in
// explorer.exe; remembering the answer avoids two NtQueryKey calls and an
// allocation on every unrelated key. Bounded: when it grows past the cap it is
// cleared wholesale (it is only a performance hint). Declared here (before
// RegCloseKeyHook) so the handle can be evicted the moment it closes - the
// kernel is free to recycle the numeric HKEY value for an unrelated key right
// after, and a stale "known not the NameSpace parent" entry would then be a
// false positive for that new key, not just a harmless miss.
static std::mutex g_nonNamespaceCacheMutex;
static std::unordered_set<HKEY> g_nonNamespaceKeys;
static constexpr size_t kNonNamespaceCacheMax = 512;

static bool IsKnownNonNamespaceKey(HKEY key) {
    if (!key) return false;
    std::lock_guard<std::mutex> lock(g_nonNamespaceCacheMutex);
    return g_nonNamespaceKeys.count(key) != 0;
}

static void RememberNonNamespaceKey(HKEY key) {
    if (!key) return;
    std::lock_guard<std::mutex> lock(g_nonNamespaceCacheMutex);
    if (g_nonNamespaceKeys.size() >= kNonNamespaceCacheMax)
        g_nonNamespaceKeys.clear();
    g_nonNamespaceKeys.insert(key);
}

static void ForgetNonNamespaceKey(HKEY key) {
    if (!key) return;
    std::lock_guard<std::mutex> lock(g_nonNamespaceCacheMutex);
    g_nonNamespaceKeys.erase(key);
}

static LSTATUS WINAPI RegOpenKeyExWHook(HKEY key, LPCWSTR subKey, DWORD options,
                                        REGSAM access, PHKEY out) {
    return OpenVirtual(key, subKey, options, access, out);
}
static LSTATUS WINAPI RegOpenKeyWHook(HKEY key, LPCWSTR subKey, PHKEY out) {
    return OpenVirtual(key, subKey, 0, MAXIMUM_ALLOWED, out);
}
static LSTATUS WINAPI RegCloseKeyHook(HKEY key) {
    if (g_keys.MightContain(key) && g_keys.IsFake(key)) {
        g_keys.CloseVirtual(key);
        return ERROR_SUCCESS;
    }
    // Untrack BEFORE closing: once RegCloseKeyOriginal returns, the kernel may
    // recycle the same numeric handle value for a concurrent RegOpenKeyExW on
    // another thread. Untracking first guarantees the stale entry can never be
    // erased for a handle that now legitimately belongs to someone else.
    g_keys.Untrack(key);
    // Same reasoning applies to the non-namespace negative cache: evict this
    // handle before the close returns, so a recycled value can never inherit
    // a stale "definitely not the NameSpace parent" verdict.
    ForgetNonNamespaceKey(key);
    return RegCloseKeyOriginal(key);
}
static LSTATUS WINAPI RegQueryValueExWHook(
    HKEY key, LPCWSTR valueName, LPDWORD reserved, LPDWORD type,
    LPBYTE data, LPDWORD bytes) {
    if (IsRootKey(key) || !g_keys.MightContain(key))
        return RegQueryValueExWOriginal(key, valueName, reserved, type, data, bytes);

    std::wstring path;
    bool isFake = false;
    g_keys.GetPathAndFake(key, path, isFake);
    LSTATUS result = ERROR_FILE_NOT_FOUND;
    if (g_registrationReady.load() && !path.empty() &&
        ProvideValue(path, valueName ? valueName : L"", type, data, bytes, result)) {
        return result;
    }
    if (isFake) return ERROR_FILE_NOT_FOUND;
    return RegQueryValueExWOriginal(key, valueName, reserved, type, data, bytes);
}
static LSTATUS WINAPI RegGetValueWHook(
    HKEY key, LPCWSTR subKey, LPCWSTR valueName, DWORD flags, LPDWORD type,
    PVOID data, LPDWORD bytes) {
    const bool textRelevant = ContainsRelevantKeywordInsensitive(subKey);
    if (IsRootKey(key) && !textRelevant)
        return RegGetValueWOriginal(key, subKey, valueName, flags, type, data, bytes);
    if (!IsRootKey(key) && !textRelevant && !g_keys.MightContain(key))
        return RegGetValueWOriginal(key, subKey, valueName, flags, type, data, bytes);

    std::wstring path;
    bool isFake = false;
    g_keys.GetPathAndFake(key, path, isFake);
    if (subKey && *subKey) {
        if (!path.empty()) path += L"\\";
        path += subKey;
    }
    LSTATUS result = ERROR_FILE_NOT_FOUND;
    if (g_registrationReady.load() && !path.empty() &&
        ProvideValue(path, valueName ? valueName : L"", type,
                     static_cast<LPBYTE>(data), bytes, result)) {
        return result;
    }
    if (isFake) return ERROR_FILE_NOT_FOUND;
    return RegGetValueWOriginal(key, subKey, valueName, flags, type, data, bytes);
}

static bool VirtualSubkey(Node node, DWORD index, std::wstring& name) {
    if (node == Node::Root) {
        static constexpr PCWSTR names[] = {
            L"DefaultIcon", L"InProcServer32", L"Instance", L"ShellFolder"};
        if (index < ARRAYSIZE(names)) {
            name = names[index];
            return true;
        }
    } else if (node == Node::Instance && index == 0) {
        name = L"InitPropertyBag";
        return true;
    } else if (node == Node::Provider && index == 0) {
        name = L"InProcServer32";
        return true;
    }
    return false;
}

// Counts exactly what the next hook/original API exposes. RegEnumKeyExW is
// stateless for an explicit (HKEY,index), so this composes with the other
// Control Panel restorer mods and gives the same answer on every pass.
static DWORD CountOriginalNamespaceEntries(HKEY key, bool& alreadyPresent) {
    alreadyPresent = false;
    wchar_t subKey[256];
    DWORD index = 0;
    for (; index < 4096; ++index) {
        DWORD chars = ARRAYSIZE(subKey);
        LSTATUS status = RegEnumKeyExWOriginal(key, index, subKey, &chars,
                                               nullptr, nullptr, nullptr, nullptr);
        if (status == ERROR_NO_MORE_ITEMS) break;
        if (status != ERROR_SUCCESS) break;
        if (_wcsicmp(subKey, kAppletClsid) == 0) alreadyPresent = true;
    }
    return index;
}

static LSTATUS WINAPI RegEnumKeyExWHook(
    HKEY key, DWORD index, LPWSTR name, LPDWORD chars, LPDWORD reserved,
    LPWSTR cls, LPDWORD classChars, PFILETIME time) {
    std::wstring keyPath;
    bool isFake = false;
    const bool known = g_keys.MightContain(key) &&
                       g_keys.GetPathAndFake(key, keyPath, isFake);
    if (isFake) {
        std::wstring sub;
        if (!VirtualSubkey(Classify(keyPath), index, sub)) {
            return ERROR_NO_MORE_ITEMS;
        }
        if (!name || !chars) return ERROR_INVALID_PARAMETER;
        if (*chars <= sub.size()) {
            *chars = static_cast<DWORD>(sub.size() + 1);
            return ERROR_MORE_DATA;
        }
        wcscpy_s(name, *chars, sub.c_str());
        *chars = static_cast<DWORD>(sub.size());
        if (time) GetSystemTimeAsFileTime(time);
        return ERROR_SUCCESS;
    }

    const LSTATUS status = RegEnumKeyExWOriginal(
        key, index, name, chars, reserved, cls, classChars, time);
    if (status != ERROR_NO_MORE_ITEMS || !g_registrationReady.load()) return status;

    // A namespace HKEY can predate our RegOpenKey hooks or be opened through an
    // unhooked API. Resolve its native name only at enumeration exhaustion, so
    // ordinary RegEnumKeyExW calls retain the allocation-free fast path.
    if (!known) {
        keyPath = QueryNativeRegistryPath(key);
        if (!IsNamespaceParent(keyPath)) return status;
        g_keys.Track(key, keyPath);
    } else if (!IsNamespaceParent(keyPath)) {
        return status;
    }

    bool alreadyPresent = false;
    const DWORD originalCount = CountOriginalNamespaceEntries(key, alreadyPresent);
    if (alreadyPresent || index != originalCount) return ERROR_NO_MORE_ITEMS;
    if (!name || !chars) return ERROR_INVALID_PARAMETER;
    const DWORD required = static_cast<DWORD>(wcslen(kAppletClsid) + 1);
    if (*chars < required) {
        *chars = required;
        return ERROR_MORE_DATA;
    }
    wcscpy_s(name, *chars, kAppletClsid);
    *chars = required - 1;
    if (time) GetSystemTimeAsFileTime(time);
    return ERROR_SUCCESS;
}

// Thin wrapper over the RegEnumKeyExW hook so callers using the older
// RegEnumKeyW entry point see the injected entry too. Without this, a shell
// code path that enumerates through RegEnumKeyW never observes the applet.
static LSTATUS WINAPI RegEnumKeyWHook(HKEY key, DWORD index, LPWSTR name,
                                      DWORD chars) {
    DWORD size = chars;
    const LSTATUS status =
        RegEnumKeyExWHook(key, index, name, &size, nullptr, nullptr, nullptr,
                          nullptr);
    // RegEnumKeyW reports a too-small buffer as ERROR_MORE_DATA with no size
    // out-parameter, which matches RegEnumKeyExW's contract closely enough.
    return status;
}

static LSTATUS WINAPI RegQueryInfoKeyWHook(
    HKEY key, LPWSTR cls, LPDWORD classChars, LPDWORD reserved,
    LPDWORD subKeys, LPDWORD maxSubKey, LPDWORD maxClass, LPDWORD values,
    LPDWORD maxValueName, LPDWORD maxValueData, LPDWORD security,
    PFILETIME time) {
    std::wstring keyPath;
    bool isFake = false;
    const bool known = g_keys.MightContain(key) &&
                       g_keys.GetPathAndFake(key, keyPath, isFake);
    if (isFake) {
        const Node node = Classify(keyPath);
        if (subKeys) *subKeys = node == Node::Root ? 4
                                  : (node == Node::Instance || node == Node::Provider)
                                        ? 1
                                        : 0;
        // Values are queryable through the W hooks but aren't enumerable on the
        // empty backing key, so don't invite callers to call RegEnumValueW.
        if (values) *values = 0;
        if (maxSubKey) *maxSubKey = 32;
        if (maxClass) *maxClass = 0;
        if (maxValueName) *maxValueName = 0;
        if (maxValueData) *maxValueData = 0;
        if (cls && classChars && *classChars) cls[0] = 0;
        if (classChars) *classChars = 0;
        if (time) GetSystemTimeAsFileTime(time);
        return ERROR_SUCCESS;
    }

    const LSTATUS status = RegQueryInfoKeyWOriginal(
        key, cls, classChars, reserved, subKeys, maxSubKey, maxClass, values,
        maxValueName, maxValueData, security, time);
    if (status != ERROR_SUCCESS || !g_registrationReady.load()) return status;

    if (!known) {
        // RegQueryInfoKeyW is called constantly on unrelated keys. Resolving the
        // native path costs two NtQueryKey round trips plus an up-to-64 KB
        // buffer, so remember keys that are NOT the ControlPanel\NameSpace
        // parent and skip them on later calls. The cache is keyed by the handle
        // value; it is only a performance hint, so a recycled handle that was
        // cached negative just falls back to resolving once more - harmless.
        if (IsKnownNonNamespaceKey(key)) return status;
        keyPath = QueryNativeRegistryPath(key);
        if (!IsNamespaceParent(keyPath)) {
            RememberNonNamespaceKey(key);
            return status;
        }
        g_keys.Track(key, keyPath);
    } else if (!IsNamespaceParent(keyPath)) {
        return status;
    }

    if (subKeys) {
        bool alreadyPresent = false;
        const DWORD originalCount =
            CountOriginalNamespaceEntries(key, alreadyPresent);
        *subKeys = (std::max)(*subKeys, originalCount);
        if (!alreadyPresent) ++*subKeys;
    }
    if (maxSubKey && *maxSubKey < wcslen(kAppletClsid))
        *maxSubKey = static_cast<DWORD>(wcslen(kAppletClsid));
    return status;
}



// -----------------------------------------------------------------------------
// Graceful degradation when the payload is not (yet) available
// -----------------------------------------------------------------------------
// The Control Panel item is registered before - and independently of - the
// wucltux.dll download, so it is always visible. If the user opens it while the
// payload is missing (first run with no internet, symbol server unreachable,
// download still in flight), the classic page cannot be constructed. Rather
// than letting the shell emit a bare "unable to load page", explain what is
// happening and offer the modern Settings page as a working alternative.
static const wchar_t* PayloadNoticeCaption() {
    if (LanguageIs(L"it")) return L"Windows Update";
    return L"Windows Update";
}

static const wchar_t* PayloadNoticeText() {
    if (LanguageIs(L"it"))
        return L"La pagina classica di Windows Update non e' ancora pronta.\n\n"
               L"Al primo avvio la mod deve scaricare un componente verificato "
               L"(wucltux.dll) dal server dei simboli Microsoft. Verifica la "
               L"connessione a Internet e riprova tra qualche istante, oppure "
               L"riavvia Esplora risorse.\n\n"
               L"Vuoi aprire Windows Update nelle Impostazioni?";
    if (LanguageIs(L"es"))
        return L"La pagina clasica de Windows Update aun no esta lista.\n\n"
               L"En el primer inicio, el mod debe descargar un componente "
               L"verificado (wucltux.dll) del servidor de simbolos de Microsoft. "
               L"Comprueba tu conexion a Internet e intentalo de nuevo, o "
               L"reinicia el Explorador.\n\n"
               L"Quieres abrir Windows Update en Configuracion?";
    if (LanguageIs(L"fr"))
        return L"La page classique de Windows Update n'est pas encore prete.\n\n"
               L"Au premier demarrage, le mod doit telecharger un composant "
               L"verifie (wucltux.dll) depuis le serveur de symboles Microsoft. "
               L"Verifiez votre connexion Internet et reessayez, ou redemarrez "
               L"l'Explorateur.\n\n"
               L"Voulez-vous ouvrir Windows Update dans les Parametres ?";
    if (LanguageIs(L"tr"))
        return L"Klasik Windows Update sayfasi henuz hazir degil.\n\n"
               L"Ilk calistirmada mod, Microsoft sembol sunucusundan dogrulanmis "
               L"bir bilesen (wucltux.dll) indirmelidir. Internet baglantinizi "
               L"kontrol edip tekrar deneyin veya Dosya Gezgini'ni yeniden "
               L"baslatin.\n\n"
               L"Windows Update'i Ayarlar'da acmak ister misiniz?";
    if (LanguageIs(L"ru"))
        return L"\u041a\u043b\u0430\u0441\u0441\u0438\u0447\u0435\u0441\u043a\u0430\u044f \u0441\u0442\u0440\u0430\u043d\u0438\u0446\u0430 \u0426\u0435\u043d\u0442\u0440\u0430 \u043e\u0431\u043d\u043e\u0432\u043b\u0435\u043d\u0438\u044f Windows \u0435\u0449\u0451 \u043d\u0435 \u0433\u043e\u0442\u043e\u0432\u0430.\n\n"
               L"\u041f\u0440\u0438 \u043f\u0435\u0440\u0432\u043e\u043c \u0437\u0430\u043f\u0443\u0441\u043a\u0435 \u043c\u043e\u0434\u0443 \u043d\u0443\u0436\u043d\u043e \u0437\u0430\u0433\u0440\u0443\u0437\u0438\u0442\u044c \u043f\u0440\u043e\u0432\u0435\u0440\u0435\u043d\u043d\u044b\u0439 \u043a\u043e\u043c\u043f\u043e\u043d\u0435\u043d\u0442 (wucltux.dll) \u0441 \u0441\u0435\u0440\u0432\u0435\u0440\u0430 \u0441\u0438\u043c\u0432\u043e\u043b\u043e\u0432 Microsoft. \u041f\u0440\u043e\u0432\u0435\u0440\u044c\u0442\u0435 \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u0435 \u043a \u0418\u043d\u0442\u0435\u0440\u043d\u0435\u0442\u0443 \u0438 \u043f\u043e\u0432\u0442\u043e\u0440\u0438\u0442\u0435 \u043f\u043e\u043f\u044b\u0442\u043a\u0443.\n\n"
               L"\u041e\u0442\u043a\u0440\u044b\u0442\u044c \u0426\u0435\u043d\u0442\u0440 \u043e\u0431\u043d\u043e\u0432\u043b\u0435\u043d\u0438\u044f Windows \u0432 \u041f\u0430\u0440\u0430\u043c\u0435\u0442\u0440\u0430\u0445?";
    if (LanguageIs(L"pt"))
        return L"A pagina classica do Windows Update ainda nao esta pronta.\n\n"
               L"Na primeira execucao, o mod precisa baixar um componente "
               L"verificado (wucltux.dll) do servidor de simbolos da Microsoft. "
               L"Verifique sua conexao com a Internet e tente novamente.\n\n"
               L"Deseja abrir o Windows Update nas Configuracoes?";
    if (LanguageIs(L"zh"))
        return L"\u7ecf\u5178 Windows \u66f4\u65b0\u9875\u9762\u5c1a\u672a\u5c31\u7eea\u3002\n\n"
               L"\u9996\u6b21\u8fd0\u884c\u65f6\uff0c\u6b64\u6a21\u7ec4\u9700\u8981\u4ece Microsoft \u7b26\u53f7\u670d\u52a1\u5668\u4e0b\u8f7d\u7ecf\u8fc7\u9a8c\u8bc1\u7684\u7ec4\u4ef6 (wucltux.dll)\u3002\u8bf7\u68c0\u67e5\u60a8\u7684 Internet \u8fde\u63a5\u540e\u91cd\u8bd5\u3002\n\n"
               L"\u662f\u5426\u5728\u201c\u8bbe\u7f6e\u201d\u4e2d\u6253\u5f00 Windows \u66f4\u65b0\uff1f";
    if (LanguageIs(L"pl"))
        return L"Klasyczna strona Windows Update nie jest jeszcze gotowa.\n\n"
               L"Przy pierwszym uruchomieniu mod musi pobrac zweryfikowany "
               L"skladnik (wucltux.dll) z serwera symboli firmy Microsoft. "
               L"Sprawdz polaczenie internetowe i sprobuj ponownie.\n\n"
               L"Czy chcesz otworzyc Windows Update w Ustawieniach?";
    if (LanguageIs(L"nl"))
        return L"De klassieke Windows Update-pagina is nog niet gereed.\n\n"
               L"Bij de eerste start moet de mod een geverifieerd onderdeel "
               L"(wucltux.dll) downloaden van de Microsoft-symboolserver. "
               L"Controleer je internetverbinding en probeer het opnieuw.\n\n"
               L"Wil je Windows Update openen in Instellingen?";
    return L"The classic Windows Update page isn't ready yet.\n\n"
           L"On first run the mod needs to download a verified component "
           L"(wucltux.dll) from the Microsoft Symbol Server. Check your internet "
           L"connection and try again in a moment, or restart Explorer.\n\n"
           L"Do you want to open Windows Update in Settings instead?";
}

// One notice at a time, and never on a shell thread: the message box is modal
// and would otherwise block the Control Panel window that is mid-navigation.
static std::atomic<bool> g_payloadNoticeOpen{false};

// The strings are snapshotted on the caller's side so the dialog thread never
// reads mod state that Wh_ModSettingsChanged could change underneath it.
struct PayloadNoticeParams {
    std::wstring caption;
    std::wstring text;
};

// The notice thread's handle is owned by the mod and joined in Wh_ModUninit.
// The mod must NOT take an extra reference on its own image (the Windhawk
// runtime unloads the mod with a single FreeLibrary once Wh_ModUninit returns,
// and a self-reference would leak the whole image, its registry virtualization
// and the mapped payload). Instead, teardown closes the message box and waits
// for the thread to finish before the image can be unmapped.
static std::mutex g_noticeThreadMutex;
static HANDLE g_noticeThread = nullptr;

static DWORD WINAPI PayloadNoticeThreadProc(LPVOID param) {
    {
        std::unique_ptr<PayloadNoticeParams> p(
            reinterpret_cast<PayloadNoticeParams*>(param));
        const int answer = MessageBoxW(nullptr, p->text.c_str(), p->caption.c_str(),
                                       MB_YESNO | MB_ICONINFORMATION |
                                           MB_SETFOREGROUND | MB_DEFBUTTON1);
        if (answer == IDYES) {
            // ms-settings: is present on every Windows 10/11 build and never
            // depends on the payload, so this always gives the user a way out.
            ShellExecuteW(nullptr, L"open", L"ms-settings:windowsupdate", nullptr,
                          nullptr, SW_SHOWNORMAL);
        }
        g_payloadNoticeOpen.store(false, std::memory_order_release);
    }
    // No self-reference, no FreeLibraryAndExitThread: Wh_ModUninit joins this
    // thread (closing the modal box if needed) before the image is unmapped.
    return 0;
}

static void ShowPayloadUnavailableNotice() {
    if (g_stopping.load()) return;
    bool expected = false;
    if (!g_payloadNoticeOpen.compare_exchange_strong(expected, true)) return;

    std::unique_ptr<PayloadNoticeParams> params;
    try {
        params = std::make_unique<PayloadNoticeParams>();
        params->caption = PayloadNoticeCaption();
        params->text = PayloadNoticeText();
    } catch (...) {
        g_payloadNoticeOpen.store(false, std::memory_order_release);
        return;
    }

    HANDLE thread = CreateThread(nullptr, 0, PayloadNoticeThreadProc,
                                 params.get(), 0, nullptr);
    if (!thread) {
        g_payloadNoticeOpen.store(false, std::memory_order_release);
        return;
    }
    // Ownership of the params (and the handle) is transferred to the mod.
    // Assign the released pointer to a variable first (clang-tidy flags a bare
    // `(void)params.release()`); the thread deletes it in PayloadNoticeThreadProc.
    PayloadNoticeParams* threadOwnedParams = params.release();
    (void)threadOwnedParams;
    std::lock_guard<std::mutex> lock(g_noticeThreadMutex);
    if (g_noticeThread) CloseHandle(g_noticeThread);  // one notice at a time
    g_noticeThread = thread;
}

// Closes the payload-notice message box (if any) and waits for its thread to
// finish. Called from Wh_ModUninit before the image is released.
static void WaitForPayloadNoticeThread() {
    HANDLE thread = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_noticeThreadMutex);
        std::swap(thread, g_noticeThread);
    }
    if (!thread) return;
    const DWORD tid = GetThreadId(thread);
    // Re-post in a loop: the message box may not exist yet on the first pass.
    while (WaitForSingleObject(thread, 50) == WAIT_TIMEOUT) {
        EnumThreadWindows(tid, [](HWND hwnd, LPARAM) -> BOOL {
            PostMessageW(hwnd, WM_CLOSE, 0, 0);
            return TRUE;
        }, 0);
    }
    CloseHandle(thread);
}

// shdocvw.dll implements the standard layout-folder class used by old CPL items.
using CoCreateInstance_t = HRESULT(WINAPI*)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
static CoCreateInstance_t CoCreateInstanceOriginal = nullptr;       // combase
static CoCreateInstance_t CoCreateInstanceOriginalOle32 = nullptr;  // ole32
// CoGetClassObject is the other activation entry point: the shell's folder
// binding often goes through it instead of CoCreateInstance, in which case a
// CoCreateInstance-only hook never fires - which is exactly what the log shows.
using CoGetClassObject_t = HRESULT(WINAPI*)(REFCLSID, DWORD, LPVOID, REFIID, LPVOID*);
static CoGetClassObject_t CoGetClassObjectOriginal = nullptr;
static CoGetClassObject_t CoGetClassObjectOriginalOle32 = nullptr;
// Defined later (before SetupWorker); declared here so the CoCreateInstance
// hook can lazily load wucltux.dll if the page is constructed in a process that
// skipped the eager heavy setup (e.g. a shell/explorer.exe fallback).
static HMODULE EnsurePrivateModuleLoaded();

// Registration deliberately becomes visible before asynchronous payload setup.
// A shell:: child-page launch can therefore start a fresh Explorer process and
// request our virtual folder a few milliseconds before g_verified is published.
// Give a fast cached setup a small grace period; once the worker has published
// the verified path/module and is only finalizing hooks, MUI resources and the
// status cache, allow a longer bounded wait. Never wait on the setup worker itself
// and never turn a genuine completed/offline failure into a repeated delay.
static bool WaitForPayloadReadinessWindow() {
    if (g_verified.load(std::memory_order_acquire)) return true;
    if (g_stopping.load(std::memory_order_acquire) ||
        g_setupFinished.load(std::memory_order_acquire) ||
        g_setupWorkerThreadId.load(std::memory_order_acquire) == GetCurrentThreadId()) {
        return false;
    }

    constexpr ULONGLONG kInitialSetupGraceMs = 250;
    constexpr ULONGLONG kFinalizationGraceMs = 3000;
    const ULONGLONG started = GetTickCount64();
    ULONGLONG finalizationStarted = 0;

    for (;;) {
        if (g_verified.load(std::memory_order_acquire)) return true;
        if (g_stopping.load(std::memory_order_acquire) ||
            g_setupFinished.load(std::memory_order_acquire)) {
            break;
        }

        const ULONGLONG now = GetTickCount64();
        if (!finalizationStarted &&
            (g_module.load(std::memory_order_acquire) != nullptr ||
             g_dllPath.load(std::memory_order_acquire) != nullptr)) {
            finalizationStarted = now;
        }

        const bool timedOut = finalizationStarted
            ? now - finalizationStarted >= kFinalizationGraceMs
            : now - started >= kInitialSetupGraceMs;
        if (timedOut) break;
        Sleep(10);
    }

    return g_verified.load(std::memory_order_acquire);
}

static HRESULT HandleCoCreateInstance(REFCLSID clsid, LPUNKNOWN outer, DWORD context,
                                      REFIID iid, LPVOID* result,
                                      CoCreateInstance_t original) {
    const bool isWUFolder = IsEqualGUID(clsid, kAppletFolderGuid);
    const bool isWUProvider = IsEqualGUID(clsid, kElementProviderGuid);
    if (!isWUFolder && !isWUProvider)
        return original(clsid, outer, context, iid, result);

    const bool isElementProvider = IsEqualGUID(clsid, kElementProviderGuid);
    if (!g_verified.load(std::memory_order_acquire)) {
        Wh_Log(L"Windows Update Restorer: %s requested while payload setup is still publishing readiness; waiting briefly",
               isElementProvider ? L"WUAppElementProvider" : L"Windows Update folder");
        if (WaitForPayloadReadinessWindow()) {
            Wh_Log(L"Windows Update Restorer: payload became ready during COM activation; continuing");
        }
    }
    if (!g_verified.load(std::memory_order_acquire)) {
        // The item is registered independently of the payload, so a first-time
        // offline setup can still legitimately land here. Only after the bounded
        // synchronization above fails do we show the existing Settings fallback.
        Wh_Log(L"Windows Update Restorer: %s requested but wucltux.dll did not become ready; showing fallback notice",
               isElementProvider ? L"WUAppElementProvider" : L"Windows Update folder");
        ShowPayloadUnavailableNotice();
        return REGDB_E_CLASSNOTREG;
    }


    // The namespace folder itself comes from shdocvw. The XMLFILE resource in
    // wucltux.dll then creates cfbc05bc-... (WUAppElementProvider), which must
    // come from the private legacy module.
    HMODULE server = nullptr;
    if (IsEqualGUID(clsid, kAppletFolderGuid)) {
        server = GetModuleHandleW(L"shdocvw.dll");
        if (!server) server = LoadLibraryExW(L"shdocvw.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    } else {
        // Defensive fallback: normally SetupWorker has already loaded
        // wucltux.dll into g_module. If for any reason it has not (e.g. the
        // element provider is requested before the background setup finished),
        // load it on demand so the page can still be constructed.
        server = g_module.load();
        if (!server) server = EnsurePrivateModuleLoaded();
    }
    if (!server) {
        if (isElementProvider)
            Wh_Log(L"Windows Update Restorer: WUAppElementProvider requested but private wucltux.dll module is not loaded");
        return REGDB_E_CLASSNOTREG;
    }
    auto getClassObject = reinterpret_cast<HRESULT(WINAPI*)(REFCLSID, REFIID, LPVOID*)>(
        GetProcAddress(server, "DllGetClassObject"));
    if (!getClassObject) {
        if (isElementProvider)
            Wh_Log(L"Windows Update Restorer: WUAppElementProvider requested but DllGetClassObject export was not found in wucltux.dll");
        return REGDB_E_CLASSNOTREG;
    }
    // The namespace folder is implemented by shdocvw's standard layout folder,
    // which is registered under kLayoutFolderClsid rather than our synthetic
    // applet CLSID. Remap exactly like HandleCoGetClassObject below, otherwise
    // shdocvw answers CLASS_E_CLASSNOTAVAILABLE.
    CLSID effective = clsid;
    if (IsEqualGUID(clsid, kAppletFolderGuid)) {
        CLSID layout{};
        if (SUCCEEDED(CLSIDFromString(kLayoutFolderClsid, &layout)))
            effective = layout;
    }

    IClassFactory* factory = nullptr;
    HRESULT hr = getClassObject(effective, IID_IClassFactory_GUID, reinterpret_cast<void**>(&factory));
    if (FAILED(hr)) {
        if (isElementProvider)
            Wh_Log(L"Windows Update Restorer: WUAppElementProvider DllGetClassObject failed (hr=0x%08X)",
                   static_cast<unsigned>(hr));
        return hr;
    }
    hr = factory->CreateInstance(outer, iid, result);
    factory->Release();
    return hr;
}


// Per-module wrappers. Each target keeps its own original pointer so a call
// into combase can never be forwarded through ole32's trampoline (or vice
// versa), which is how the Performance Information and Tools Restorer does it.
static HRESULT WINAPI CoCreateInstanceHookCombase(REFCLSID clsid, LPUNKNOWN outer,
                                                  DWORD context, REFIID iid,
                                                  LPVOID* result) {
    return HandleCoCreateInstance(clsid, outer, context, iid, result,
                                  CoCreateInstanceOriginal);
}

static HRESULT WINAPI CoCreateInstanceHookOle32(REFCLSID clsid, LPUNKNOWN outer,
                                                DWORD context, REFIID iid,
                                                LPVOID* result) {
    return HandleCoCreateInstance(clsid, outer, context, iid, result,
                                  CoCreateInstanceOriginalOle32);
}

// The shell binds a namespace folder by asking for its class *factory*, not by
// calling CoCreateInstance. With only a CoCreateInstance hook installed, that
// request goes to the real COM runtime, finds no registered server for our
// synthetic CLSID, and fails with "no app associated" - precisely the observed
// behaviour. Serve the factory ourselves from the right in-proc server.
static HRESULT HandleCoGetClassObject(REFCLSID clsid, DWORD context, LPVOID reserved,
                                      REFIID iid, LPVOID* result,
                                      CoGetClassObject_t original) {
    const bool isWUFolder = IsEqualGUID(clsid, kAppletFolderGuid);
    const bool isWUProvider = IsEqualGUID(clsid, kElementProviderGuid);
    if (!isWUFolder && !isWUProvider)
        return original(clsid, context, reserved, iid, result);

    if (!g_verified.load(std::memory_order_acquire)) {
        Wh_Log(L"Windows Update Restorer: class factory requested while payload setup is still publishing readiness; waiting briefly");
        if (WaitForPayloadReadinessWindow()) {
            Wh_Log(L"Windows Update Restorer: payload became ready during class-factory activation; continuing");
        }
    }
    if (!g_verified.load(std::memory_order_acquire)) {
        Wh_Log(L"Windows Update Restorer: class factory requested but the payload did not become ready; showing fallback notice");
        ShowPayloadUnavailableNotice();
        return REGDB_E_CLASSNOTREG;
    }

    // The namespace folder is implemented by shdocvw's standard layout folder;
    // only the element provider comes from the private wucltux.dll copy.
    HMODULE server = nullptr;
    if (isWUFolder) {
        server = GetModuleHandleW(L"shdocvw.dll");
        if (!server)
            server = LoadLibraryExW(L"shdocvw.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    } else {
        server = g_module.load();
        if (!server) server = EnsurePrivateModuleLoaded();
    }
    if (!server) {
        Wh_Log(L"Windows Update Restorer: no in-proc server available for the class factory");
        return REGDB_E_CLASSNOTREG;
    }

    auto getClassObject = reinterpret_cast<HRESULT(WINAPI*)(REFCLSID, REFIID, LPVOID*)>(
        GetProcAddress(server, "DllGetClassObject"));
    if (!getClassObject) {
        Wh_Log(L"Windows Update Restorer: DllGetClassObject not exported by the in-proc server");
        return REGDB_E_CLASSNOTREG;
    }

    // For the layout folder, shdocvw registers the standard folder under a
    // different CLSID, so ask it for that one rather than for our namespace id.
    CLSID effective = clsid;
    if (isWUFolder) {
        CLSID layout{};
        if (SUCCEEDED(CLSIDFromString(kLayoutFolderClsid, &layout))) effective = layout;
    }

    const HRESULT hr = getClassObject(effective, iid, result);
    Wh_Log(L"Windows Update Restorer: served class factory (hr=0x%08X)",
           static_cast<unsigned>(hr));
    return hr;
}

static HRESULT WINAPI CoGetClassObjectHookCombase(REFCLSID clsid, DWORD context,
                                                  LPVOID reserved, REFIID iid,
                                                  LPVOID* result) {
    return HandleCoGetClassObject(clsid, context, reserved, iid, result,
                                  CoGetClassObjectOriginal);
}

static HRESULT WINAPI CoGetClassObjectHookOle32(REFCLSID clsid, DWORD context,
                                                LPVOID reserved, REFIID iid,
                                                LPVOID* result) {
    return HandleCoGetClassObject(clsid, context, reserved, iid, result,
                                  CoGetClassObjectOriginalOle32);
}

static void* RegistryFunction(const char* name) {
    HMODULE module = GetModuleHandleW(L"kernelbase.dll");
    void* function = module ? reinterpret_cast<void*>(GetProcAddress(module, name)) : nullptr;
    if (!function) {
        module = GetModuleHandleW(L"advapi32.dll");
        if (!module) module = LoadLibraryExW(L"advapi32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (module) function = reinterpret_cast<void*>(GetProcAddress(module, name));
    }
    return function;
}


// Loads the private wucltux.dll module on demand from g_dllPath. Used as a
// defensive fallback from the CoCreateInstance hook: normally SetupWorker loads
// the module eagerly into g_module; if the element provider is requested before
// that finishes, this loads it right away so the page can still be constructed.
// Fast (a plain module load), so it is safe to call from the CoCreateInstance
// hook. Returns the module or nullptr.
static HMODULE EnsurePrivateModuleLoaded() {
    if (HMODULE m = g_module.load()) return m;
    const std::wstring* path = g_dllPath.load();
    if (!path || path->empty()) return nullptr;
    HMODULE module = LoadLibraryExW(path->c_str(), nullptr, 0);
    if (!module) {
        Wh_Log(L"Windows Update Restorer: lazy LoadLibraryEx of wucltux failed (%u)", GetLastError());
        return nullptr;
    }
    HMODULE expected = nullptr;
    if (g_module.compare_exchange_strong(expected, module)) return module;
    FreeLibrary(module);  // another thread won the race; do not leak ours
    return g_module.load();
}

// RAII for the cross-process setup mutex, so an early return or an exception can
// never leave the named mutex held and wedge every other explorer.exe/control.exe.
class ScopedSetupMutex {
public:
    ScopedSetupMutex() {
        handle_ = CreateMutexW(nullptr, FALSE,
                               L"Windhawk.WindowsUpdateControlPanelRestorer.Setup");
        if (!handle_) return;
        HANDLE waitOn[2] = {handle_, g_stopEvent};
        const DWORD count = g_stopEvent ? 2 : 1;
        const DWORD wait = WaitForMultipleObjects(count, waitOn, FALSE, 60000);
        if (wait == WAIT_OBJECT_0 || wait == WAIT_ABANDONED) {
            owned_ = true;              // WAIT_ABANDONED still transfers ownership
        } else if (wait == WAIT_OBJECT_0 + 1) {
            aborted_ = true;            // teardown requested while queueing
        }
        // On timeout we proceed unowned: the download always lands on a private
        // temp file and is atomically moved into place, so a concurrent setup is
        // still correct, just redundant.
    }
    ~ScopedSetupMutex() {
        if (handle_) {
            if (owned_) ReleaseMutex(handle_);
            CloseHandle(handle_);
        }
    }
    ScopedSetupMutex(const ScopedSetupMutex&) = delete;
    ScopedSetupMutex& operator=(const ScopedSetupMutex&) = delete;
    bool aborted() const { return aborted_; }

private:
    HANDLE handle_ = nullptr;
    bool owned_ = false;
    bool aborted_ = false;
};

static void SetupWorkerImpl() {
    // Serialize first-time setup across every process running this mod (several
    // explorer.exe instances plus control.exe can start at once), so two of them
    // never download and rename the same file concurrently.
    ScopedSetupMutex setupLock;
    if (setupLock.aborted() || g_stopping.load()) {
        Wh_Log(L"Windows Update Restorer: setup aborted before it started (mod is unloading)");
        return;
    }

    std::wstring path;
    if (!EnsurePayload(path) || g_stopping.load()) {
        // Not fatal: the Control Panel entry stays registered and visible, and
        // opening it shows the localized notice with a link to Settings.
        Wh_Log(L"Windows Update Restorer: wucltux.dll was not downloaded or failed verification. "
               L"The Control Panel entry stays available; check the internet connection and "
               L"restart Explorer to retry.");
        return;
    }
    // The page's XMLFILE creates WUAppElementProvider through DllGetClassObject,
    // so this must be an executable module load, not LOAD_LIBRARY_AS_DATAFILE.
    // It remains a private copy outside System32.
    HMODULE module = LoadLibraryExW(path.c_str(), nullptr, 0);
    if (!module) {
        Wh_Log(L"Windows Update Restorer: LoadLibraryEx failed (%u)", GetLastError());
        return;
    }

    // Publish the path before the module: the CoCreateInstance fallback reads
    // g_dllPath to load the payload lazily, and a non-null g_module with no path
    // would make that fallback unable to recover. Allocation failure here must
    // not take the process down, so it is contained.
    std::wstring* published = nullptr;
    try {
        published = new std::wstring(path);
    } catch (...) {
        Wh_Log(L"Windows Update Restorer: out of memory publishing the payload path");
        FreeLibrary(module);
        return;
    }
    if (const std::wstring* previous =
            g_dllPath.exchange(published, std::memory_order_acq_rel)) {
        delete previous;  // never leak on a re-entrant/second setup
    }
    g_module.store(module);

    // The setup worker starts from Wh_ModAfterInit, so dynamic hook operations
    // are legal here. Install this before g_verified exposes the page.
    if (!g_stopping.load()) InstallWucltuxSetSiteHook(module);

    if (!BuildEmbeddedMuiResourceModule(path)) {
        // Non-fatal: the page still renders using the DLL's own resources.
        Wh_Log(L"Windows Update Restorer: embedded MUI resource module could not be built; "
               L"falling back to the payload's native resources");
    }

    // Finish the cached status first so the page's first render can show the
    // installed-update date (or N/A) without doing registry/SCM work on the UI
    // thread. Publishing g_verified before this step allowed a newly opened page
    // to race the worker and permanently render an empty value.
    if (!g_stopping.load()) GatherBackgroundStatus();
    if (g_stopping.load()) return;

    g_verified.store(true, std::memory_order_release);
    Wh_Log(L"Windows Update Restorer ready: verified Windows 8.1 wucltux.dll loaded privately");
}

// Hard exception boundary: this runs on a std::thread, where an escaping
// exception would call std::terminate and take explorer.exe down with it.
static void SetupWorker() {
    g_setupWorkerThreadId.store(GetCurrentThreadId(), std::memory_order_release);
    try {
        SetupWorkerImpl();
    } catch (const std::exception& e) {
        Wh_Log(L"Windows Update Restorer: setup failed with an exception (%S); "
               L"the Control Panel entry remains available", e.what());
    } catch (...) {
        Wh_Log(L"Windows Update Restorer: setup failed with an unknown exception; "
               L"the Control Panel entry remains available");
    }
    // Publish this on every return path (success, offline failure or exception).
    // COM requests that raced setup will observe g_verified first; later requests
    // after a real failure skip the grace period and get the fallback immediately.
    g_setupFinished.store(true, std::memory_order_release);
    g_setupWorkerThreadId.store(0, std::memory_order_release);
}


static void CleanupGeneratedResourceModuleFiles(bool includeCurrentProcess);

BOOL Wh_ModInit() {
    try {
        g_setupFinished.store(false, std::memory_order_release);
        g_setupWorkerThreadId.store(0, std::memory_order_release);
        if (!IsRunningAsAmd64()) {
            Wh_Log(L"Windows Update Restorer not started: the process is not AMD64");
            return FALSE;
        }

        // Informational only - the mod never refuses to run on a build it does
        // not recognize. Targets Windows 10 21H2+ and every Windows 11 build;
        // all behaviour is feature-probed rather than version-gated.
        {
            OSVERSIONINFOEXW osInfo{sizeof(osInfo)};
            using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOEXW*);
            if (HMODULE ntdll = GetModuleHandleW(L"ntdll.dll")) {
                if (auto rtlGetVersion = reinterpret_cast<RtlGetVersion_t>(
                        GetProcAddress(ntdll, "RtlGetVersion"))) {
                    if (rtlGetVersion(&osInfo) == 0) {
                        Wh_Log(L"Windows Update Restorer 1.0.0: starting on Windows %lu.%lu build %lu",
                               osInfo.dwMajorVersion, osInfo.dwMinorVersion,
                               osInfo.dwBuildNumber);
                    }
                }
            }
        }

        // Remove unlocked files left by Explorer/control.exe processes that
        // exited without Wh_ModUninit. Files still mapped elsewhere are skipped.
        CleanupGeneratedResourceModuleFiles(false);
        // Same contract for partial downloads left by a killed process.
        SweepStaleGeneratedFiles();
        LoadLanguageSetting();
        InitPaths();
        void* openEx = RegistryFunction("RegOpenKeyExW");
        void* open = RegistryFunction("RegOpenKeyW");
        void* create = RegistryFunction("RegCreateKeyExW");
        void* close = RegistryFunction("RegCloseKey");
        void* query = RegistryFunction("RegQueryValueExW");
        void* get = RegistryFunction("RegGetValueW");
        void* enumerate = RegistryFunction("RegEnumKeyExW");
        void* enumerateOld = RegistryFunction("RegEnumKeyW");
        void* info = RegistryFunction("RegQueryInfoKeyW");
        if (!openEx || !open || !create || !close || !query || !get ||
            !enumerate || !info) return FALSE;
        RegCreateKeyExWOriginal = reinterpret_cast<RegCreateKeyExW_t>(create);
        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegOpenKeyExW_t>(openEx), RegOpenKeyExWHook, &RegOpenKeyExWOriginal);
        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegOpenKeyW_t>(open), RegOpenKeyWHook, &RegOpenKeyWOriginal);
        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegCloseKey_t>(close), RegCloseKeyHook, &RegCloseKeyOriginal);
        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegQueryValueExW_t>(query), RegQueryValueExWHook, &RegQueryValueExWOriginal);
        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegGetValueW_t>(get), RegGetValueWHook, &RegGetValueWOriginal);
        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegEnumKeyExW_t>(enumerate), RegEnumKeyExWHook, &RegEnumKeyExWOriginal);
        if (enumerateOld) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<RegEnumKeyW_t>(enumerateOld),
                                           RegEnumKeyWHook, &RegEnumKeyWOriginal);
        }
        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegQueryInfoKeyW_t>(info), RegQueryInfoKeyWHook, &RegQueryInfoKeyWOriginal);


        // Hook COM activation on BOTH combase.dll (the real implementation) and
        // ole32.dll (the legacy forwarder). The shell's Control Panel binding
        // path can reach either one, and hooking only combase leaves the other
        // route unhooked - the log showed activation never being intercepted.
        // Cover CoGetClassObject too: folder binding asks for a class factory
        // rather than calling CoCreateInstance.
        HMODULE combase = GetModuleHandleW(L"combase.dll");
        if (!combase) combase = LoadLibraryExW(L"combase.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        HMODULE ole32 = GetModuleHandleW(L"ole32.dll");
        if (!ole32) ole32 = LoadLibraryExW(L"ole32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

        void* createCombase = combase ? reinterpret_cast<void*>(
                                            GetProcAddress(combase, "CoCreateInstance"))
                                      : nullptr;
        void* createOle32 = ole32 ? reinterpret_cast<void*>(
                                        GetProcAddress(ole32, "CoCreateInstance"))
                                  : nullptr;
        if (createCombase) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<CoCreateInstance_t>(createCombase),
                                           CoCreateInstanceHookCombase,
                                           &CoCreateInstanceOriginal);
        }
        if (createOle32 && createOle32 != createCombase) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<CoCreateInstance_t>(createOle32),
                                           CoCreateInstanceHookOle32,
                                           &CoCreateInstanceOriginalOle32);
        }

        void* factoryCombase = combase ? reinterpret_cast<void*>(
                                             GetProcAddress(combase, "CoGetClassObject"))
                                       : nullptr;
        void* factoryOle32 = ole32 ? reinterpret_cast<void*>(
                                         GetProcAddress(ole32, "CoGetClassObject"))
                                   : nullptr;
        if (factoryCombase) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<CoGetClassObject_t>(factoryCombase),
                                           CoGetClassObjectHookCombase,
                                           &CoGetClassObjectOriginal);
        }
        if (factoryOle32 && factoryOle32 != factoryCombase) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<CoGetClassObject_t>(factoryOle32),
                                           CoGetClassObjectHookOle32,
                                           &CoGetClassObjectOriginalOle32);
        }
        Wh_Log(L"Windows Update Restorer: COM hooks installed (combase=%d/%d, ole32=%d/%d)",
               createCombase ? 1 : 0, factoryCombase ? 1 : 0,
               createOle32 ? 1 : 0, factoryOle32 ? 1 : 0);
        // wucltux.dll imports LoadStringW through the normal User32 API. Its
        // original MUI is embedded above, so no external .mui file is needed.
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (!user32) user32 = LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (user32) {
            if (void* loadString = reinterpret_cast<void*>(GetProcAddress(user32, "LoadStringW")))
                WindhawkUtils::SetFunctionHook(reinterpret_cast<LoadStringW_t>(loadString),
                                               LoadStringWHook, &LoadStringWOriginal);
        }


        // DirectUI's resstr(...) goes through XResourceProvider rather than
        // LoadStringW. Redirect it to the private DLL carrying the embedded MUI.
        HMODULE dui70 = GetModuleHandleW(L"dui70.dll");
        if (!dui70) dui70 = LoadLibraryExW(L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (dui70) {
            void* providerCreate = reinterpret_cast<void*>(GetProcAddress(
                dui70, "?Create@XResourceProvider@DirectUI@@SAJPEAUHINSTANCE__@@PEBG11PEAPEAV12@@Z"));
            if (providerCreate) {
                WindhawkUtils::SetFunctionHook(
                    reinterpret_cast<XResourceProviderCreate_t>(providerCreate),
                    XResourceProviderCreateHook, &XResourceProviderCreateOriginal);
            } else {

                Wh_Log(L"Windows Update Restorer: XResourceProvider::Create was not found");
            }
        }

        // Prepare shutdown signalling before the setup worker starts.
        g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        // Manual-reset, initially unsignaled; Wh_ModSettingsChanged sets it
        // to abort a stale in-flight rebuild and resets it before starting
        // the next one (see there).
        g_rebuildAbortEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        g_stopping.store(false);
        // Only patch the safe WUAppPage content anchor (never the outer Control Panel host).
        InstallModernWuXmlPatchHook();
        InstallShellPresentationHooks();
        InstallNativeControlPanelNavLinksHook();
        InstallLegacyWarningIconHook();
        // Publish the Control Panel registration BEFORE the background setup
        // starts. Every value the shell needs to draw and browse the item is
        // served from constants, so "Windows Update" is present in Control Panel
        // the moment the mod is enabled - no download, no network, no waiting.
        // The payload only gates the page's own content (see g_verified).
        g_registrationReady.store(true, std::memory_order_release);
        Wh_Log(L"Windows Update Restorer: Control Panel registration is live (payload setup continues in the background)");

        // Setup starts in Wh_ModAfterInit. This matters because the cached
        // payload can load immediately and its SetSite hook must then be applied
        // dynamically; Wh_ApplyHookOperations isn't legal before ModInit returns.
        return TRUE;
    } catch (...) {
        // Never leave a half-initialized mod behind: make every hook inert.
        g_registrationReady.store(false);
        g_verified.store(false);
        g_stopping.store(true);
        g_setupFinished.store(true, std::memory_order_release);
        g_setupWorkerThreadId.store(0, std::memory_order_release);
        Wh_Log(L"Windows Update Restorer: initialization failed; the mod is inactive");
        return FALSE;
    }
}

void Wh_ModAfterInit() {
    // If the worker thread cannot be created, the registration still works and
    // the existing fallback notice explains that the payload isn't ready.
    try {
        g_setupThread.emplace(SetupWorker);
    } catch (...) {
        g_setupFinished.store(true, std::memory_order_release);
        g_setupWorkerThreadId.store(0, std::memory_order_release);
        Wh_Log(L"Windows Update Restorer: could not start the setup thread; "
               L"the Control Panel entry stays available and the payload will "
               L"be retried on the next start");
    }
}

// Called whenever the user changes the mod settings (e.g. picks a different
// language). We reload the flag and, if the language actually changed, rebuild
// the embedded MUI module in the background so the classic page reflects the
// new language without a full mod restart.
void Wh_ModSettingsChanged() {
    // Windhawk calls this on its own thread while the shell keeps running, so a
    // thrown exception here would propagate into the host. Contain everything.
    try {
        const std::wstring oldLanguage = CurrentLanguage();
        LoadLanguageSetting();

        if (oldLanguage == CurrentLanguage()) return;
        // Language changed: the tasks file embeds translated labels.
        CleanupControlPanelTasksXmlFile();
        if (g_stopping.load()) return;  // teardown in progress: don't start work

        // Ensure a previous rebuild (if any) has finished before starting a new
        // one, so we never run two builds concurrently. Signal it to abort its
        // retry backoff first: otherwise two language changes in quick
        // succession can stall this call (Windhawk's own thread) for up to
        // ~10s while the stale rebuild works through kMaxMoveAttempts.
        if (g_rebuildThread && g_rebuildThread->joinable()) {
            if (g_rebuildAbortEvent) SetEvent(g_rebuildAbortEvent);
            g_rebuildThread->join();
            // Reset before starting the new rebuild below, or its own retry
            // waits would immediately fall through as "aborted".
            if (g_rebuildAbortEvent) ResetEvent(g_rebuildAbortEvent);
        }
        g_rebuildThread.reset();
        g_rebuildThread.emplace([] {
            // Same hard boundary as the setup worker: an exception escaping a
            // std::thread would call std::terminate and kill explorer.exe.
            try {
                RebuildEmbeddedMuiForLanguage();
            } catch (...) {
                Wh_Log(L"Windows Update Restorer: language rebuild failed; keeping the previous strings");
            }
        });
    } catch (...) {
        Wh_Log(L"Windows Update Restorer: settings change could not be applied");
    }
}


// Sweep generated embedded-MUI files from previous processes at startup and
// again on controlled unload. A second Explorer/control.exe process can be
// building its resource file concurrently, so never touch files owned by a PID
// that is still alive even when DeleteFile would happen to succeed.
static DWORD GeneratedResourceFilePid(PCWSTR fileName) {
    constexpr PCWSTR prefix = L"wucltux.embedded-mui-";
    if (wcsncmp(fileName, prefix, wcslen(prefix)) != 0) return 0;
    wchar_t* end = nullptr;
    const unsigned long pid = wcstoul(fileName + wcslen(prefix), &end, 10);
    return end && end != fileName + wcslen(prefix) && *end == L'-'
               ? static_cast<DWORD>(pid)
               : 0;
}

static bool IsProcessStillRunning(DWORD pid) {
    if (!pid) return false;
    HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pid);
    if (!process) {
        // Access denied isn't evidence that a process has exited; skip the file
        // conservatively. ERROR_INVALID_PARAMETER is the normal dead-PID case.
        return GetLastError() != ERROR_INVALID_PARAMETER;
    }
    const bool running = WaitForSingleObject(process, 0) == WAIT_TIMEOUT;
    CloseHandle(process);
    return running;
}

static void CleanupGeneratedResourceModuleFiles(bool includeCurrentProcess) {
    const std::wstring& dir = StoreDir();
    if (dir.empty()) return;
    const std::wstring pattern = dir + L"\\wucltux.embedded-mui-*";

    unsigned deleted = 0;
    unsigned active = 0;
    unsigned locked = 0;
    WIN32_FIND_DATAW findData{};
    HANDLE find = FindFirstFileW(pattern.c_str(), &findData);
    if (find == INVALID_HANDLE_VALUE) return;
    do {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        const DWORD ownerPid = GeneratedResourceFilePid(findData.cFileName);
        if (ownerPid &&
            ((ownerPid == GetCurrentProcessId() && !includeCurrentProcess) ||
             (ownerPid != GetCurrentProcessId() &&
              IsProcessStillRunning(ownerPid)))) {
            ++active;
            continue;
        }

        const std::wstring fullPath = dir + L"\\" + findData.cFileName;
        if (DeleteFileW(fullPath.c_str())) {
            ++deleted;
        } else if (GetLastError() != ERROR_FILE_NOT_FOUND) {
            ++locked;
        }
    } while (FindNextFileW(find, &findData));
    FindClose(find);

    if (deleted || active || locked) {
        Wh_Log(L"Windows Update Restorer: embedded MUI cleanup: deleted=%u active-skipped=%u locked=%u",
               deleted, active, locked);
    }
}

void Wh_ModUninit() {
  try {
    // Stop answering registry queries first. The Control Panel item then
    // disappears immediately on disable, and no hook can hand out a synthetic
    // key or read state that the rest of this function is about to release.
    g_registrationReady.store(false);
    g_verified.store(false);
    // Signal teardown before touching any thread, so workers observe it while
    // we are still tearing the UI down.
    g_stopping.store(true);
    g_setupFinished.store(true, std::memory_order_release);
    if (g_stopEvent) SetEvent(g_stopEvent);
    // Unblocks a WinInet call that is stuck mid-download so the join below
    // returns promptly instead of waiting out the receive timeout.
    CloseActiveDownloadHandles();

    // The payload-notice thread owns a modal message box and lives in this
    // image. Close the box and join the thread before anything else, so the
    // image can be unmapped safely. (No self-reference is taken; see
    // ShowPayloadUnavailableNotice.)
    WaitForPayloadNoticeThread();

    // Modeless dialog proc and all subclass callbacks are mod code: make them
    // unreachable before Windhawk unloads this image. Wh_ModUninit runs on an
    // arbitrary Windhawk thread, so DestroyWindow() here would fail (the dialog
    // belongs to an Explorer UI thread) and leave WuSettingsDlgProc registered
    // in a just-unloaded image. SendMessage(WM_CLOSE) runs the proc on the
    // dialog's owning thread, where it calls DestroyWindow safely.
    CloseAllWuSettingsDialogs();
    CloseAllWuFaqDialogs();
    DestroySettingsCombobox();

    // Close any "Check for updates" dialog that is still open (its proc lives
    // in this image). Closing it also clears the in-progress flag, so a later
    // load can start a fresh check.
    CloseAllWuCheckDialogs();
    g_checkingForUpdates.store(false, std::memory_order_release);

    // Both workers poll g_stopping and their blocking waits are bounded, so
    // these joins return quickly. They must complete before the image is
    // unmapped: a running worker's code lives in this image.
    if (g_setupThread && g_setupThread->joinable()) g_setupThread->join();
    g_setupThread.reset();
    if (g_rebuildThread && g_rebuildThread->joinable()) g_rebuildThread->join();
    g_rebuildThread.reset();
    if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }
    if (g_rebuildAbortEvent) { CloseHandle(g_rebuildAbortEvent); g_rebuildAbortEvent = nullptr; }
    delete g_dllPath.exchange(nullptr);
    // Deliberately do not unload the datafile while a Control Panel page can cache it.
    g_module.store(nullptr);
    {
        std::lock_guard<std::mutex> lock(g_resourceMutex);
        CleanupGeneratedResourceModuleFiles(true);
    }
    CleanupAppletLogoIconFiles();
    CleanupControlPanelTasksXmlFile();
    {
        std::lock_guard lock(g_statusIconMutex);
        for (auto& [key, icon] : g_statusIconCache) DestroyIcon(icon);
        g_statusIconCache.clear();
    }
    ShutdownGdiPlusRendering();
    g_keys.ClearWithoutFreeing();
    ReleaseVirtualKeyRoot();
    Wh_Log(L"Windows Update Restorer: unloaded cleanly; no registry or system files were modified");
  } catch (...) {
    // Teardown must never throw into Windhawk. Whatever failed above, the
    // registration flags are already cleared, so the mod is inert.
    Wh_Log(L"Windows Update Restorer: exception during teardown (mod is already inert)");
  }
}
