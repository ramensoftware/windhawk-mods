// ==WindhawkMod==
// @id             settings-to-control-panel
// @name           Redirect Settings to Control Panel
// @description    This mod forces the classic Control Panel to open instead of Windows 10/11 Settings app using native components
// @version        10.0.36
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

**Note**: The mod has been tested on Windows 10 1809, Windows 10 21H2, Windows 11 23H2, Windows 11 24H2 and Windows 11 25H2 and the tests confirm that the mod is more functional on Windows 10 but both should not cause issues.

---

## Features

- The mod redirects many `ms-settings:` links to the classic Control Panel
- The mod includes an anti-loop protection (stops windows from reopening endlessly)
- The mod includes a configurable fallback behavior for unmapped links
- The mod includes an experimental tray menu detection (which works on some builds)
- The mod includes defensive handling around its own logic: guarded hook installers, RAII cleanup for handles/COM/environment blocks, and safe fallbacks when an optional hook or launch fails. This guards against failed initialization and allocation failures inside the mod's own code, not against Explorer crashes in general (an access violation elsewhere in the process is not something a C++ `try`/`catch` can intercept)


**Note**: This mod is a best-effort implementation. It aims to intercept and redirect as many `ms-settings:` links as possible, but due to differences between Windows 10 and Windows 11, as well as changes introduced by Microsoft in each build, some redirects may not work perfectly in all environments.

---

## Limitations

- The system tray context menu redirect only supports the Win32 taskbar (the one from Windows 10). However, in some Windows 11 configurations if explorer is restarted the network system tray redirect might not work.
- The device & printers system tray redirect may not work on some Windows 11 configurations, as Microsoft hardcoded the redirect to the Settings app in certain shell code paths. This could change in future if correct documentation is found.
- The mod is not compatible with 32 bit based operating systems. It requires a 64-bit version of Windows (x64 or ARM64).
- The `ms-settings:display` group (Display, display-advanced, display-advanced-graphics, display-adapter-properties, display-resolution, screenrotation) all map to the same classic Display/Screen Resolution applet that the **Classic Display Control Panel Restorer** mod also restores. If you use that mod, turn off the "Redirect Display Pages" setting here to avoid the two mods fighting over the same pages.

---

**Recommendation**: For a better experience on Windows 11 (and Windows 10 if necessary), it is recommended to pair this mod with some of the hereby suggested implementations:

- **[Windows 7/8.1 Action Center Recreation](https://windhawk.net/mods/win7-action-center-recreation)** – recreates the classic Windows 7/8.1 Action Center tray icon and flyout with real-time security status monitoring along with a partial restore of a link inside the Action Center Control Panel page.
- **[Classic Taskbar and Start Menu Properties](https://windhawk.net/mods/classic-taskbar-properties)** – recreates the classic Windows 7 "Taskbar and Start Menu Properties" dialog for Windows 10 and 11.
- **[Windows 7 Network Flyout Recreation](https://windhawk.net/mods/win7-network-flyout-recreation)** – recreates the classic Windows 7 network flyout with Wi-Fi list, signal strength, and connection support and, if enabled, partial restore of some links inside the classic "Network and Sharing Center" Control Panel page.
- **[Windows 7 "Open With" Dialog](https://windhawk.net/mods/win7-open-with-dialog)** – recreates the classic Windows 7 "Open With" dialog for Windows 10 and 11.
- **[Classic Display Control Panel Restorer](https://windhawk.net/mods/win7-display-control-panel-restorer)** – restores the classic Display and Screen Resolution Control Panel pages.
- **[Windows 11 HomeGroup Restorer](https://windhawk.net/mods/win11-home-group-restorer)** – restores the classic HomeGroup applet on Windows 11.
- **[Windows Update Control Panel Restorer](https://windhawk.net/mods/windows-update-control-panel-restorer)** – restores the classic Windows Update Control Panel page on Windows 10/11.
- **[Performance Information and Tools Restorer](https://windhawk.net/mods/performance-info-tools-restorer)** – restores the classic "Performance Information and Tools" applet.
- **[Windows 7 Legacy Applet Restorer](https://windhawk.net/mods/win7-legacy-applet-restorer)** – restores various legacy Windows 7 Control Panel applets (Credits to Anixx for the original mod).

All of these mods are **reversible** and help make Windows 10 and 11 look more like Windows 7 and classic versions of Windows without replacing system files.

---

## Credits


- m417z – Code reviews and feedback
- Anixx – Testing on Windows 11 23H2 and the original toolbar subclassing approach
- sebastian08dm08-cpu - Testing on Windows 10 1809
- Cips_35 - Testing on Windows 11 25H2
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
- RedirectDisplayPages: true
  $name: Redirect Display Pages
  $description: "Controls the ms-settings:display group (Display, display-advanced, display-advanced-graphics, display-adapter-properties, display-resolution, screenrotation). Turn this off if you also use the Classic Display Control Panel Restorer mod, since both mods target the same classic Display/Screen Resolution pages."
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
#include <atomic>
#include <exception>
#include <cwctype>
#include <cstddef>
// ## Stability hardening (10.0.36)

// This maintenance update does not change the redirect map or the settings behavior. It hardens the existing implementation with scoped resource cleanup, exception boundaries around hooks and worker code, synchronized settings/map access, bounded guard caches, safer tray data reads, and unload-aware cleanup. When an optional path cannot be completed, it falls back to the original Windows behavior rather than risking Explorer stability.

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
static std::atomic_bool g_pniduiHookInstalled{false};
static std::mutex g_pniduiHookMutex;
static HANDLE g_traySubclassWatchdogThread = nullptr;
static HWND g_lastShellTrayWnd = nullptr;
static HANDLE g_stopEvent = nullptr;

static bool ICMH_CALL ICMH_hook_SndVolSSO(HMENU m, HWND w);
static bool ICMH_CALL ICMH_hook_pnidui(HMENU m, HWND w);
static bool ICMH_CALL ICMH_hook_Shell32Devices(void* pThis, HMENU m, UINT u);
// Applies a hook operation queued after Wh_ModInit already returned; see the
// definition further down for details.
static bool ApplyLateHookIfNeeded();

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
// A failed registration means the public export remains unhooked and is a
// valid last-resort launcher. If registration succeeded but the trampoline is
// not available yet, do not call the public export (that would recurse).
static std::atomic_bool g_shellExecuteExHookRegistered{false};
static std::atomic_bool g_shellExecuteHookRegistered{false};
static std::atomic_bool g_createProcessHookRegistered{false};

// The mod runs inside Explorer. Treat every resource and callback as host-process
// infrastructure: a failed optional path must not leave a live handle, a thread, or
// an exception crossing an Explorer/COM/Win32 boundary.
static std::atomic_bool g_unloading{false};
// Set once after the initial in-Wh_ModInit hook batch has been queued.
// Windhawk applies hook operations registered during Wh_ModInit
// automatically once it returns; anything registered afterwards (from the
// watchdog thread, tray recreation, or a settings change) needs an explicit
// Wh_ApplyHookOperations() call, which this flag lets us gate on.
static std::atomic_bool g_modInitComplete{false};
// static std::atomic<int> g_activeHookCalls{0};  


class ScopedHandle {
public:
    ScopedHandle() noexcept = default;
    explicit ScopedHandle(HANDLE value) noexcept : value_(value) {}
    ~ScopedHandle() { reset(); }

    ScopedHandle(const ScopedHandle&) = delete;
    ScopedHandle& operator=(const ScopedHandle&) = delete;

    ScopedHandle(ScopedHandle&& other) noexcept : value_(other.release()) {}
    ScopedHandle& operator=(ScopedHandle&& other) noexcept {
        if (this != &other) {
            reset(other.release());
        }
        return *this;
    }

    HANDLE get() const noexcept { return value_; }
    explicit operator bool() const noexcept {
        return value_ != nullptr && value_ != INVALID_HANDLE_VALUE;
    }

    HANDLE release() noexcept {
        HANDLE value = value_;
        value_ = nullptr;
        return value;
    }

    void reset(HANDLE value = nullptr) noexcept {
        if (*this) {
            CloseHandle(value_);
        }
        value_ = value;
    }

private:
    HANDLE value_ = nullptr;
};

class ScopedProcessInformation {
public:
    PROCESS_INFORMATION* get() noexcept { return &value_; }
    const PROCESS_INFORMATION& value() const noexcept { return value_; }

    ScopedProcessInformation(const ScopedProcessInformation&) = delete;
    ScopedProcessInformation& operator=(const ScopedProcessInformation&) = delete;
    ScopedProcessInformation() = default;

    ~ScopedProcessInformation() {
        if (value_.hThread) {
            CloseHandle(value_.hThread);
        }
        if (value_.hProcess) {
            CloseHandle(value_.hProcess);
        }
    }

private:
    PROCESS_INFORMATION value_{};
};

class ScopedEnvironmentStrings {
public:
    explicit ScopedEnvironmentStrings(LPWCH value = nullptr) noexcept : value_(value) {}
    ~ScopedEnvironmentStrings() {
        if (value_) {
            FreeEnvironmentStringsW(value_);
        }
    }

    ScopedEnvironmentStrings(const ScopedEnvironmentStrings&) = delete;
    ScopedEnvironmentStrings& operator=(const ScopedEnvironmentStrings&) = delete;

    LPWCH get() const noexcept { return value_; }

private:
    LPWCH value_ = nullptr;
};

class ScopedCoInitialize {
public:
    explicit ScopedCoInitialize(DWORD flags) noexcept : result_(CoInitializeEx(nullptr, flags)) {}
    ~ScopedCoInitialize() {
        if (SUCCEEDED(result_)) {
            CoUninitialize();
        }
    }

    HRESULT result() const noexcept { return result_; }
    bool initialized() const noexcept { return SUCCEEDED(result_); }

    ScopedCoInitialize(const ScopedCoInitialize&) = delete;
    ScopedCoInitialize& operator=(const ScopedCoInitialize&) = delete;

private:
    HRESULT result_ = E_FAIL;
};

template <typename T>
class ScopedComPtr {
public:
    ScopedComPtr() = default;
    ~ScopedComPtr() { reset(); }

    ScopedComPtr(const ScopedComPtr&) = delete;
    ScopedComPtr& operator=(const ScopedComPtr&) = delete;

    T* get() const noexcept { return value_; }
    T** put() noexcept {
        reset();
        return &value_;
    }
    explicit operator bool() const noexcept { return value_ != nullptr; }

    void reset(T* value = nullptr) noexcept {
        if (value_) {
            value_->Release();
        }
        value_ = value;
    }

private:
    T* value_ = nullptr;
};

static BOOL CallOriginalShellExecuteExW(SHELLEXECUTEINFOW* pei) {
    if (ShellExecuteExW_orig) {
        return ShellExecuteExW_orig(pei);
    }
    if (!g_shellExecuteExHookRegistered.load(std::memory_order_acquire)) {
        // No detour was installed, so calling the export cannot re-enter us.
        return ShellExecuteExW(pei);
    }
    SetLastError(ERROR_PROC_NOT_FOUND);
    return FALSE;
}

static HINSTANCE CallOriginalShellExecuteW(HWND hwnd, LPCWSTR op, LPCWSTR file,
                                            LPCWSTR params, LPCWSTR dir, INT show) {
    if (ShellExecuteW_orig) {
        return ShellExecuteW_orig(hwnd, op, file, params, dir, show);
    }
    if (!g_shellExecuteHookRegistered.load(std::memory_order_acquire)) {
        return ShellExecuteW(hwnd, op, file, params, dir, show);
    }
    SetLastError(ERROR_PROC_NOT_FOUND);
    return (HINSTANCE)(INT_PTR)SE_ERR_FNF;
}

static BOOL CallOriginalCreateProcessW(
    LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation) {
    if (CreateProcessW_orig) {
        return CreateProcessW_orig(lpApplicationName, lpCommandLine,
            lpProcessAttributes, lpThreadAttributes, bInheritHandles,
            dwCreationFlags, lpEnvironment, lpCurrentDirectory,
            lpStartupInfo, lpProcessInformation);
    }
    if (!g_createProcessHookRegistered.load(std::memory_order_acquire)) {
        return CreateProcessW(lpApplicationName, lpCommandLine,
            lpProcessAttributes, lpThreadAttributes, bInheritHandles,
            dwCreationFlags, lpEnvironment, lpCurrentDirectory,
            lpStartupInfo, lpProcessInformation);
    }
    SetLastError(ERROR_PROC_NOT_FOUND);
    return FALSE;
}

static BOOL CallOriginalTrackPopupMenuEx(
    BOOL (WINAPI* original)(HMENU, UINT, int, int, HWND, const TPMPARAMS*),
    HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, const TPMPARAMS* lptpm) {
    return original ? original(hMenu, uFlags, x, y, hWnd, lptpm) : FALSE;
}

static bool CallOriginalSndVolSSO(HMENU menu, HWND window) {
    return g_icmhOrig_SndVolSSO ? g_icmhOrig_SndVolSSO(menu, window) : true;
}

static bool CallOriginalPnidui(HMENU menu, HWND window) {
    return g_icmhOrig_pnidui ? g_icmhOrig_pnidui(menu, window) : true;
}

static bool CallOriginalShell32Devices(void* self, HMENU menu, UINT value) {
    return g_icmhOrig_Shell32Devices ? g_icmhOrig_Shell32Devices(self, menu, value) : true;
}

// Read shell-owned, undocumented pointer data through the kernel instead of
// dereferencing a possibly stale toolbar payload directly. This is best-effort:
// a failed read only disables icon recognition for that click.
// The mod targets x86-64 only. Use the integer-sized alias for pointer-sized
// reads so clang-tidy does not mistake an intentional pointer-width copy for
// a stale `sizeof(pointer)`/array-length bug.
static constexpr SIZE_T NATIVE_POINTER_BYTES = sizeof(ULONG_PTR);

static bool TryReadProcessMemory(const void* address, void* output, SIZE_T size) noexcept {
    if (!address || !output || size == 0) {
        return false;
    }
    SIZE_T read = 0;
    return ReadProcessMemory(GetCurrentProcess(), address, output, size, &read) && read == size;
}

static bool IsExecutableAddress(const void* address) noexcept {
    if (!address) {
        return false;
    }
    MEMORY_BASIC_INFORMATION mbi{};
    if (!VirtualQuery(address, &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT) {
        return false;
    }
    DWORD protect = mbi.Protect & 0xFF;
    return protect == PAGE_EXECUTE || protect == PAGE_EXECUTE_READ ||
           protect == PAGE_EXECUTE_READWRITE || protect == PAGE_EXECUTE_WRITECOPY;
}

struct ResolveResult {
    std::wstring target;
    bool intercept;
};

static thread_local int g_hookDepth = 0;

struct HookGuard {
    HookGuard() noexcept { ++g_hookDepth; }
    ~HookGuard() noexcept {
        if (g_hookDepth > 0) {
            --g_hookDepth;
        }
    }
    bool IsReentrant() const noexcept { return g_hookDepth > 1; }
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
    // Use a local block and publish it only after it is complete. If allocation
    // fails or the inherited block is unexpectedly huge, passing nullptr later
    // intentionally preserves the parent environment instead of launching a
    // child with a truncated environment.
    try {
        std::wstring environmentBlock;
        ScopedEnvironmentStrings currentEnvironment(GetEnvironmentStringsW());

        for (LPWCH p = currentEnvironment.get(); p && *p; ) {
            std::wstring entry(p);
            constexpr size_t kMaxEnvironmentChars = 32767;
            if (environmentBlock.size() + entry.size() + 2 > kMaxEnvironmentChars) {
                Wh_Log(L"[STABILITY] Child environment is too large; inheriting it unchanged");
                g_childEnvBlock.clear();
                return;
            }

            // Environment names are case-insensitive on Windows. Do not pass
            // an inherited copy of our recursion-suppression marker onward.
            if (_wcsnicmp(entry.c_str(), L"WH_STC_NOREDIRECT=", 18) != 0) {
                environmentBlock.append(entry);
                environmentBlock.push_back(L'\0');
            }
            p += entry.length() + 1;
        }

        constexpr wchar_t kNoRedirectVariable[] = L"WH_STC_NOREDIRECT=1";
        if (environmentBlock.size() + wcslen(kNoRedirectVariable) + 2 > 32767) {
            Wh_Log(L"[STABILITY] Child environment marker does not fit; inheriting environment");
            g_childEnvBlock.clear();
            return;
        }

        environmentBlock.append(kNoRedirectVariable);
        environmentBlock.push_back(L'\0');
        environmentBlock.push_back(L'\0');
        g_childEnvBlock.swap(environmentBlock);
    } catch (const std::exception&) {
        g_childEnvBlock.clear();
        Wh_Log(L"[STABILITY] BuildChildEnvironment caught std::exception; using inherited environment");
    } catch (...) {
        g_childEnvBlock.clear();
        Wh_Log(L"[STABILITY] BuildChildEnvironment caught an unknown exception; using inherited environment");
    }
}

static LPVOID ChildEnvironmentBlock() noexcept {
    return g_childEnvBlock.empty()
        ? nullptr
        : const_cast<wchar_t*>(g_childEnvBlock.c_str());
}

static bool IsChildProcess() {
    static int isChild = -1;
    if (isChild == -1) {
        isChild = (GetEnvironmentVariableW(L"WH_STC_NOREDIRECT", nullptr, 0) > 0) ? 1 : 0;
    }
    return isChild == 1;
}

struct ModSettings {
    std::atomic_bool enableRedirects{true};
    std::atomic_bool redirectSystemTray{false};
    std::atomic_bool uiOnlyRedirects{false};
    std::atomic_int fallbackMode{2};
    std::atomic_bool win11CompatibilityMode{false};
    std::atomic_int maxLaunchesPerUri{3};
    std::atomic_bool comActivationRedirect{false};
    std::atomic_bool legacyNameMappingFix{true};
    std::atomic_bool redirectDisplayPages{true};
};

static ModSettings g_settings;

static bool RedirectsEnabled() noexcept {
    return g_settings.enableRedirects.load(std::memory_order_acquire);
}
static bool RedirectSystemTrayEnabled() noexcept {
    return g_settings.redirectSystemTray.load(std::memory_order_acquire);
}
static bool UiOnlyRedirectsEnabled() noexcept {
    return g_settings.uiOnlyRedirects.load(std::memory_order_acquire);
}
static int FallbackMode() noexcept {
    return g_settings.fallbackMode.load(std::memory_order_acquire);
}
static bool Win11CompatibilityModeEnabled() noexcept {
    return g_settings.win11CompatibilityMode.load(std::memory_order_acquire);
}
static int MaxLaunchesPerUri() noexcept {
    return g_settings.maxLaunchesPerUri.load(std::memory_order_acquire);
}
static bool ComActivationRedirectEnabled() noexcept {
    return g_settings.comActivationRedirect.load(std::memory_order_acquire);
}
static bool LegacyNameMappingFixEnabled() noexcept {
    return g_settings.legacyNameMappingFix.load(std::memory_order_acquire);
}
static bool RedirectDisplayPagesEnabled() noexcept {
    return g_settings.redirectDisplayPages.load(std::memory_order_acquire);
}

static void DisableRedirectsAfterSettingsFailure() noexcept {
    g_settings.enableRedirects.store(false, std::memory_order_release);
    g_settings.redirectSystemTray.store(false, std::memory_order_release);
    g_settings.uiOnlyRedirects.store(false, std::memory_order_release);
    g_settings.fallbackMode.store(2, std::memory_order_release);
    g_settings.win11CompatibilityMode.store(false, std::memory_order_release);
    g_settings.maxLaunchesPerUri.store(3, std::memory_order_release);
    g_settings.comActivationRedirect.store(false, std::memory_order_release);
    g_settings.legacyNameMappingFix.store(true, std::memory_order_release);
    g_settings.redirectDisplayPages.store(true, std::memory_order_release);
}

static void LoadSettings() {
    try {
        const bool enableRedirects = Wh_GetIntSetting(L"EnableRedirects") != 0;
        const bool redirectSystemTray = Wh_GetIntSetting(L"RedirectSystemTray") != 0;
        const bool uiOnlyRedirects = Wh_GetIntSetting(L"UIOnlyRedirects") != 0;

        int fallbackMode = 2;
        WindhawkUtils::StringSetting fallbackSetting(Wh_GetStringSetting(L"FallbackMode"));
        PCWSTR fallbackStr = fallbackSetting;
        if (fallbackStr && fallbackStr[0] != L'\0') {
            int mode = _wtoi(fallbackStr);
            fallbackMode = (mode >= 0 && mode <= 2) ? mode : 2;
        }

        const bool win11CompatibilityMode = Wh_GetIntSetting(L"Win11CompatibilityMode") != 0;
        const int configuredMaxLaunches = Wh_GetIntSetting(L"MaxLaunchesPerUri");
        const int maxLaunchesPerUri = (configuredMaxLaunches >= 0 && configuredMaxLaunches <= 20)
            ? configuredMaxLaunches : 3;
        const bool comActivationRedirect = Wh_GetIntSetting(L"ComActivationRedirect") != 0;
        const bool legacyNameMappingFix = Wh_GetIntSetting(L"LegacyNameMappingFix") != 0;
        const bool redirectDisplayPages = Wh_GetIntSetting(L"RedirectDisplayPages") != 0;

        g_settings.enableRedirects.store(enableRedirects, std::memory_order_release);
        g_settings.redirectSystemTray.store(redirectSystemTray, std::memory_order_release);
        g_settings.uiOnlyRedirects.store(uiOnlyRedirects, std::memory_order_release);
        g_settings.fallbackMode.store(fallbackMode, std::memory_order_release);
        g_settings.win11CompatibilityMode.store(win11CompatibilityMode, std::memory_order_release);
        g_settings.maxLaunchesPerUri.store(maxLaunchesPerUri, std::memory_order_release);
        g_settings.comActivationRedirect.store(comActivationRedirect, std::memory_order_release);
        g_settings.legacyNameMappingFix.store(legacyNameMappingFix, std::memory_order_release);
        g_settings.redirectDisplayPages.store(redirectDisplayPages, std::memory_order_release);
    } catch (const std::exception&) {
        DisableRedirectsAfterSettingsFailure();
        Wh_Log(L"[STABILITY] LoadSettings caught std::exception; redirects are disabled for safety");
    } catch (...) {
        DisableRedirectsAfterSettingsFailure();
        Wh_Log(L"[STABILITY] LoadSettings caught an unknown exception; redirects are disabled for safety");
    }
}

static bool ICMH_CALL ICMH_hook_SndVolSSO(HMENU m, HWND w) {
    if (g_unloading.load(std::memory_order_acquire) || !RedirectsEnabled() || !RedirectSystemTrayEnabled())
        return CallOriginalSndVolSSO(m, w);
    // Nothing here can throw, so there's no exception to guard against.
    return false;
}

static bool ICMH_CALL ICMH_hook_pnidui(HMENU m, HWND w) {
    if (g_unloading.load(std::memory_order_acquire) || !RedirectsEnabled() || !RedirectSystemTrayEnabled())
        return CallOriginalPnidui(m, w);
    // Nothing here can throw, so there's no exception to guard against.
    return false;
}

static bool ICMH_CALL ICMH_hook_Shell32Devices(void* pThis, HMENU m, UINT u) {
    if (g_unloading.load(std::memory_order_acquire) || !RedirectsEnabled() || !RedirectSystemTrayEnabled())
        return CallOriginalShell32Devices(pThis, m, u);
    // Nothing here can throw, so there's no exception to guard against.
    return false;
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
static constexpr size_t MAX_GUARD_RECORDS = 512;

static void PruneBounceGuardLocked(DWORD now) {
    if (g_bounceGuard.size() < MAX_GUARD_RECORDS) {
        return;
    }
    for (auto it = g_bounceGuard.begin(); it != g_bounceGuard.end(); ) {
        if (now - it->second.lastRedirectTick >= BOUNCE_WINDOW_MS) {
            it = g_bounceGuard.erase(it);
        } else {
            ++it;
        }
    }
    if (g_bounceGuard.size() >= MAX_GUARD_RECORDS) {
        // A hostile/very unusual stream of unique URIs must never turn into a
        // permanent Explorer allocation. Clearing only sacrifices temporary
        // bounce suppression for the oldest busy window.
        g_bounceGuard.clear();
    }
}

static void BounceGuardRecord(const std::wstring& uri) {
    try {
        std::lock_guard<std::mutex> lk(g_bounceGuardMtx);
        DWORD now = GetTickCount();
        PruneBounceGuardLocked(now);
        g_bounceGuard[uri].lastRedirectTick = now;
    } catch (const std::exception&) {
        Wh_Log(L"[STABILITY] Bounce guard allocation failed; skipping this record");
    } catch (...) {
        Wh_Log(L"[STABILITY] Bounce guard failed unexpectedly; skipping this record");
    }
}

static bool BounceGuardIsBounce(const std::wstring& uri) {
    try {
        std::lock_guard<std::mutex> lk(g_bounceGuardMtx);
        auto it = g_bounceGuard.find(uri);
        if (it == g_bounceGuard.end()) return false;
        DWORD elapsed = GetTickCount() - it->second.lastRedirectTick;
        if (elapsed < BOUNCE_WINDOW_MS) {
            it->second.lastRedirectTick = 0;
            return true;
        }
        return false;
    } catch (...) {
        return false;
    }
}

struct LaunchRecord {
    int count = 0;
    DWORD firstTick = 0;
};

static std::mutex g_loopGuardMtx;
static std::unordered_map<std::wstring, LaunchRecord> g_loopGuard;

static constexpr DWORD LOOP_WINDOW_MS = 5000;

static bool LoopGuardAllow(const std::wstring& target) {
    const int maxLaunches = MaxLaunchesPerUri();
    if (maxLaunches <= 0) return true;

    try {
        std::lock_guard<std::mutex> lk(g_loopGuardMtx);
        DWORD now = GetTickCount();
        if (g_loopGuard.size() >= MAX_GUARD_RECORDS) {
            for (auto it = g_loopGuard.begin(); it != g_loopGuard.end(); ) {
                if (now - it->second.firstTick >= LOOP_WINDOW_MS) {
                    it = g_loopGuard.erase(it);
                } else {
                    ++it;
                }
            }
            if (g_loopGuard.size() >= MAX_GUARD_RECORDS) {
                g_loopGuard.clear();
            }
        }

        auto& rec = g_loopGuard[target];
        if (rec.count == 0 || (now - rec.firstTick) >= LOOP_WINDOW_MS) {
            rec.count = 1;
            rec.firstTick = now;
            return true;
        }

        if (rec.count < maxLaunches) {
            rec.count++;
            return true;
        }

        return false;
    } catch (const std::exception&) {
        // Prefer a one-off pass-through launch over an exception escaping a
        // shell hook. The normal anti-loop limit remains active when memory is
        // available again.
        Wh_Log(L"[STABILITY] Loop guard allocation failed; allowing one launch");
        return true;
    } catch (...) {
        Wh_Log(L"[STABILITY] Loop guard failed unexpectedly; allowing one launch");
        return true;
    }
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

    HWND hIconWnd = nullptr;
    if (!TryReadProcessMemory(reinterpret_cast<const void*>(tb.dwData),
                              &hIconWnd, NATIVE_POINTER_BYTES) ||
        !hIconWnd || !IsWindow(hIconWnd)) {
        return 0;
    }

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
    CallOriginalShellExecuteExW(&sei);
}

static void OpenClassicNetworkConnections() {
    SHELLEXECUTEINFOW sei = {};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_INVOKEIDLIST;
    sei.lpVerb = L"open";
    sei.lpFile = L"explorer.exe";
    sei.lpParameters = L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}";
    sei.nShow = SW_SHOWNORMAL;
    CallOriginalShellExecuteExW(&sei);
}

static void OpenClassicDevicesAndPrinters() {
    SHELLEXECUTEINFOW sei = {};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_INVOKEIDLIST;
    sei.lpVerb = L"open";
    sei.lpFile = L"explorer.exe";
    sei.lpParameters = L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}";
    sei.nShow = SW_SHOWNORMAL;
    CallOriginalShellExecuteExW(&sei);
}

static LRESULT CALLBACK TrayToolbarSubclassProc(
    HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData)
{
    if (!g_unloading.load(std::memory_order_acquire) && msg == WM_RBUTTONUP) {
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
    if (g_unloading.load(std::memory_order_acquire)) return;
    try {
        HWND hToolbar;
        {
            std::lock_guard<std::mutex> lk(g_traySubclassMutex);
            if (g_hTrayToolbar && IsWindow(g_hTrayToolbar)) return;
            g_hTrayToolbar = nullptr;
            hToolbar = FindTrayToolbar();
        }
        if (!hToolbar || !IsWindow(hToolbar) || !InitTrayDllInfo() ||
            g_unloading.load(std::memory_order_acquire)) {
            return;
        }
        BOOL ok = WindhawkUtils::SetWindowSubclassFromAnyThread(hToolbar, TrayToolbarSubclassProc, 0);
        bool removeImmediately = false;
        if (ok) {
            // Re-check while holding the same mutex used by teardown, so an
            // unload cannot slip between the check and publishing the HWND.
            std::lock_guard<std::mutex> lk(g_traySubclassMutex);
            if (!g_unloading.load(std::memory_order_acquire)) {
                g_hTrayToolbar = hToolbar;
            } else {
                removeImmediately = true;
            }
        }
        if (removeImmediately) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hToolbar, TrayToolbarSubclassProc);
        }
    } catch (const std::exception&) {
        Wh_Log(L"[STABILITY] SetupTraySubclass caught std::exception");
    } catch (...) {
        Wh_Log(L"[STABILITY] SetupTraySubclass caught an unknown exception");
    }
}

static void RemoveTraySubclass() {
    try {
        HWND h;
        {
            std::lock_guard<std::mutex> lk(g_traySubclassMutex);
            h = g_hTrayToolbar;
            g_hTrayToolbar = nullptr;
        }
        if (h && IsWindow(h)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(h, TrayToolbarSubclassProc);
        }
    } catch (...) {
        // Teardown is best-effort. The system subclass helper owns any remaining
        // bookkeeping if the shell window disappeared concurrently.
        Wh_Log(L"[STABILITY] RemoveTraySubclass caught an exception");
    }
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
    if (g_unloading.load(std::memory_order_acquire) || !pOrig ||
        !RedirectSystemTrayEnabled() || !RedirectsEnabled()) {
        return CallOriginalTrackPopupMenuEx(pOrig, hMenu, uFlags, x, y, hWnd, lptpm);
    }

    HookGuard guard;
    if (guard.IsReentrant())
        return CallOriginalTrackPopupMenuEx(pOrig, hMenu, uFlags, x, y, hWnd, lptpm);

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
        return CallOriginalTrackPopupMenuEx(pOrig, hMenu, uFlags, x, y, hWnd, lptpm);

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
        return CallOriginalTrackPopupMenuEx(pOrig, hMenu, uFlags, x, y, hWnd, lptpm);
    }

    UINT originalId = GetMenuItemID(hMenu, targetIndex);
    bool callerWantedReturnCmd = (uFlags & TPM_RETURNCMD) != 0;
    uFlags |= TPM_RETURNCMD;
    
    BOOL result = CallOriginalTrackPopupMenuEx(pOrig, hMenu, uFlags, x, y, hWnd, lptpm);
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
static std::mutex g_mappingsMutex;

static bool InitMappings() {
    try {
    const bool w11 = g_isWin11;

    std::unordered_map<std::wstring, std::wstring> mappings = {
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

    mappings[L"ms-settings:backup"] = L"control.exe /name Microsoft.BackupAndRestore";
    mappings[L"ms-settings:network-advancedsettings"] = L"control.exe /name Microsoft.NetworkAndSharingCenter";

    if (g_isWin11) {
        mappings[L"ms-settings:recovery"] = L"shell:::{26EE0668-A00A-44D7-9371-BEB064C98683}\\0\\::{9FE63AFD-59CF-4419-9775-ABCC3849F861}";
    }

    {
        std::lock_guard<std::mutex> lk(g_mappingsMutex);
        g_mappings.swap(mappings);
    }
    return true;
    } catch (const std::exception&) {
        Wh_Log(L"[STABILITY] InitMappings caught std::exception; keeping the previous map");
    } catch (...) {
        Wh_Log(L"[STABILITY] InitMappings caught an unknown exception; keeping the previous map");
    }
    return false;
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
    if (Win11CompatibilityModeEnabled() && !IsClsidSafeOnWin11(clsPart)) {
        return L"control.exe";
    }
    return target;
}

static bool HandleFallback(const std::wstring& uri) {
    switch (FallbackMode()) {
        case 0: return true;
        case 1: {
            std::wstring cmd = L"control.exe";
            STARTUPINFOW si = {};
            si.cb = sizeof(si);
            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_SHOWNORMAL;
            ScopedProcessInformation pi;
            if (!CallOriginalCreateProcessW(nullptr, cmd.data(), nullptr, nullptr,
                                            FALSE, 0, nullptr, nullptr, &si, pi.get())) {
                Wh_Log(L"[STABILITY] Fallback control.exe launch failed (%lu)", GetLastError());
            }
            return true;
        }
        default: return false;
    }
}

static void LaunchTarget(const std::wstring& command) {
    if (!LoopGuardAllow(command)) return;

    try {
        std::wstring lower = ToLower(command);

        if (lower.find(L"explorer shell:::") != std::wstring::npos) {
            SHELLEXECUTEINFOW sei = {};
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_FLAG_NO_UI;
            sei.lpVerb = L"open";
            sei.lpFile = L"explorer.exe";
            sei.lpParameters = command.c_str() + 9;
            sei.nShow = SW_SHOWNORMAL;
            CallOriginalShellExecuteExW(&sei);
            return;
        }

        if (lower.find(L"rundll32.exe ") == 0) {
            wchar_t rundll32Path[MAX_PATH]{};
            if (GetSystemDirectoryW(rundll32Path, ARRAYSIZE(rundll32Path))) {
                wcscat_s(rundll32Path, ARRAYSIZE(rundll32Path), L"\\rundll32.exe");
            } else {
                wcscpy_s(rundll32Path, ARRAYSIZE(rundll32Path), L"rundll32.exe");
            }
            SHELLEXECUTEINFOW sei = {};
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_FLAG_NO_UI;
            sei.lpVerb = L"open";
            sei.lpFile = rundll32Path;
            sei.lpParameters = command.c_str() + 13;
            sei.nShow = SW_SHOWNORMAL;
            CallOriginalShellExecuteExW(&sei);
            return;
        }

        bool isFullCmdLine = (lower.find(L"explorer.exe ") != std::wstring::npos) ||
                             (lower.find(L"control.exe /") != std::wstring::npos);
        if (isFullCmdLine) {
            STARTUPINFOW si = {};
            si.cb = sizeof(si);
            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_SHOWNORMAL;
            ScopedProcessInformation pi;
            std::wstring mutableCmd = command;
            if (!CallOriginalCreateProcessW(nullptr, mutableCmd.data(), nullptr, nullptr,
                                            FALSE, CREATE_UNICODE_ENVIRONMENT,
                                            ChildEnvironmentBlock(), nullptr, &si, pi.get())) {
                Wh_Log(L"[STABILITY] Full command redirect launch failed (%lu)", GetLastError());
            }
            return;
        }

        if (command == L"devmgmt.msc" || command == L"compmgmt.msc" ||
            command == L"slui.exe" || command == L"OptionalFeatures.exe") {
            CallOriginalShellExecuteW(nullptr, L"open", command.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            return;
        }

        STARTUPINFOW si = {};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_SHOWNORMAL;
        std::wstring cmdLine;

        if (command.find(L".msc") != std::wstring::npos) {
            cmdLine = L"mmc.exe \"" + command + L"\"";
        } else if (command.find(L".cpl") != std::wstring::npos) {
            CallOriginalShellExecuteW(nullptr, L"open", L"control.exe", command.c_str(), nullptr, SW_SHOWNORMAL);
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
            CallOriginalShellExecuteExW(&sei);
            return;
        } else if (command.empty()) {
            cmdLine = L"control.exe";
        } else {
            cmdLine = L"control.exe " + command;
        }

        if (!cmdLine.empty()) {
            std::wstring mutableCmd = cmdLine;
            ScopedProcessInformation pi;
            if (!CallOriginalCreateProcessW(nullptr, mutableCmd.data(), nullptr, nullptr,
                                            FALSE, CREATE_UNICODE_ENVIRONMENT,
                                            ChildEnvironmentBlock(), nullptr, &si, pi.get())) {
                Wh_Log(L"[STABILITY] Redirect target launch failed (%lu)", GetLastError());
            }
        }
    } catch (const std::exception&) {
        Wh_Log(L"[STABILITY] LaunchTarget caught std::exception; target was skipped");
    } catch (...) {
        Wh_Log(L"[STABILITY] LaunchTarget caught an unknown exception; target was skipped");
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

// The classic Display / Screen Resolution pages this group maps to are also
// what the companion "Classic Display Control Panel Restorer" mod restores.
// Gated behind RedirectDisplayPages so the two mods can coexist: when off,
// these URIs fall through to HandleFallback instead of being redirected here.
static bool IsDisplayGroupUri(const std::wstring& uri) {
    static const std::wstring_view kDisplayGroupUris[] = {
        L"ms-settings:display",
        L"ms-settings:display-advanced",
        L"ms-settings:display-advanced-graphics",
        L"ms-settings:display-adapter-properties",
        L"ms-settings:display-resolution",
        L"ms-settings:screenrotation",
    };
    for (const auto& candidate : kDisplayGroupUris) {
        if (uri == candidate) return true;
    }
    return false;
}

static ResolveResult ResolveUri(const std::wstring& uri, HWND hwnd) {
    if (uri == L"ms-settings:personalization-background") {
        if (BounceGuardIsBounce(uri)) return {L"", true};
        std::wstring t = ApplyWin11Filter(ResolvePersonalizationBackground(hwnd));
        BounceGuardRecord(uri);
        return {t, true};
    }

    std::wstring mappedTarget;
    bool mappingFound = false;
    if (IsDisplayGroupUri(uri) && !RedirectDisplayPagesEnabled()) {
    return {L"", false};
}
if (true) {
    std::lock_guard<std::mutex> lk(g_mappingsMutex);
    auto it = g_mappings.find(uri);
    if (it != g_mappings.end()) {
        mappedTarget = it->second;
        mappingFound = true;
    }
    }

    if (mappingFound) {
        bool useBounceGuard = ShouldApplyBounceGuard(uri);
        if (useBounceGuard && BounceGuardIsBounce(uri)) {
            bool handled = HandleFallback(uri);
            return {L"", handled};
        }
        std::wstring t = ApplyWin11Filter(mappedTarget);
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

static HRESULT CallOriginalActivateApplication(IUnknown* pThis, LPCWSTR appUserModelId,
                                                LPCWSTR arguments, DWORD options,
                                                DWORD* processId) noexcept {
    return g_origActivateApplication
        ? g_origActivateApplication(pThis, appUserModelId, arguments, options, processId)
        : E_FAIL;
}

HRESULT STDMETHODCALLTYPE AAM_ActivateApplication_hook(
    IUnknown* pThis,
    LPCWSTR appUserModelId,
    LPCWSTR arguments,
    DWORD options,
    DWORD* processId)
{
    if (g_unloading.load(std::memory_order_acquire) || !RedirectsEnabled() ||
        !ComActivationRedirectEnabled()) {
        return CallOriginalActivateApplication(pThis, appUserModelId, arguments, options, processId);
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

    return CallOriginalActivateApplication(pThis, appUserModelId, arguments, options, processId);
}

static void InstallAAMHook() {
    if (g_unloading.load(std::memory_order_acquire) || !ComActivationRedirectEnabled()) return;
    try {
        std::lock_guard<std::mutex> lk(g_aamHookMutex);
        if (g_aamHookInstalled || g_unloading.load(std::memory_order_acquire)) return;

        ScopedCoInitialize com(COINIT_APARTMENTTHREADED);
        if (!com.initialized() && com.result() != RPC_E_CHANGED_MODE) {
            Wh_Log(L"[AAM-HOOK] CoInitializeEx failed: 0x%08X", com.result());
            return;
        }

        ScopedComPtr<IUnknown> aam;
        HRESULT hr = CoCreateInstance(
            CLSID_ApplicationActivationManager_STC,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_IApplicationActivationManager_STC,
            reinterpret_cast<void**>(aam.put()));

        if (FAILED(hr) || !aam) {
            Wh_Log(L"[AAM-HOOK] CoCreateInstance failed: 0x%08X", hr);
            return;
        }

        IApplicationActivationManagerVtbl* vtbl = nullptr;
        ActivateApplication_t target = nullptr;
        const void* activateApplicationSlot = nullptr;
        if (TryReadProcessMemory(aam.get(), &vtbl, NATIVE_POINTER_BYTES) && vtbl) {
            // Calculate the slot address without dereferencing the private
            // vtable pointer. The following ReadProcessMemory performs the
            // only read and fails safely if the object changed underneath us.
            activateApplicationSlot = reinterpret_cast<const BYTE*>(vtbl) +
                offsetof(IApplicationActivationManagerVtbl, ActivateApplication);
        }
        if (!activateApplicationSlot ||
            !TryReadProcessMemory(activateApplicationSlot, &target, NATIVE_POINTER_BYTES) ||
            !IsExecutableAddress(reinterpret_cast<const void*>(target))) { 
            Wh_Log(L"[AAM-HOOK] Invalid IApplicationActivationManager vtable; hook skipped");
            return;
        }

        if (WindhawkUtils::SetFunctionHook(target, AAM_ActivateApplication_hook,
                                            &g_origActivateApplication)) {
            if (ApplyLateHookIfNeeded()) {
                g_aamHookInstalled = true;
                Wh_Log(L"[AAM-HOOK] Successfully installed");
            } else {
                Wh_Log(L"[AAM-HOOK] Registered but Wh_ApplyHookOperations failed; will retry later");
            }
        } else {
            Wh_Log(L"[AAM-HOOK] SetFunctionHook failed; COM redirect remains disabled");
        }
    } catch (const std::exception&) {
        Wh_Log(L"[AAM-HOOK] Install caught std::exception; feature skipped");
    } catch (...) {
        Wh_Log(L"[AAM-HOOK] Install caught an unknown exception; feature skipped");
    }
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

static bool CallOriginalMapLegacyName(void* pThis, LPCWSTR pszLegacyName,
                                      LPWSTR pszNewName, UINT uLen,
                                      bool* nameChanged) noexcept {
    return COpenControlPanel__MapLegacyName_orig
        ? COpenControlPanel__MapLegacyName_orig(pThis, pszLegacyName, pszNewName, uLen, nameChanged)
        : false;
}

bool COpenControlPanel__MapLegacyName_hook(
    void    *pThis,
    LPCWSTR  pszLegacyName,
    LPWSTR   pszNewName,
    UINT     uLen,
    bool    *nameChanged)
{
    if (g_unloading.load(std::memory_order_acquire) || !LegacyNameMappingFixEnabled()) {
        return CallOriginalMapLegacyName(pThis, pszLegacyName, pszNewName, uLen, nameChanged);
    }

    if (!ShouldSuppressLegacyNameMapping(pszLegacyName)) {
        return CallOriginalMapLegacyName(pThis, pszLegacyName, pszNewName, uLen, nameChanged);
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
    if (g_unloading.load(std::memory_order_acquire) || IsChildProcess() || !RedirectsEnabled() || !pei) {
        return CallOriginalShellExecuteExW(pei);
    }

    HookGuard guard;
    if (guard.IsReentrant()) return CallOriginalShellExecuteExW(pei);

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
        return CallOriginalShellExecuteExW(pei);

    if (!uri.empty()) {
        auto result = ResolveUri(uri, pei->hwnd);
        if (result.intercept) {
            if (!result.target.empty()) LaunchTarget(result.target);
            if (pei->fMask & SEE_MASK_NOCLOSEPROCESS) pei->hProcess = nullptr;
            return TRUE;
        }
    }
    return CallOriginalShellExecuteExW(pei);
}
HINSTANCE WINAPI ShellExecuteW_hook(HWND hwnd, LPCWSTR op, LPCWSTR file, LPCWSTR params, LPCWSTR dir, INT show) {
    if (g_unloading.load(std::memory_order_acquire) || IsChildProcess() || !RedirectsEnabled()) {
        return CallOriginalShellExecuteW(hwnd, op, file, params, dir, show);
    }

    HookGuard guard;
    if (guard.IsReentrant()) return CallOriginalShellExecuteW(hwnd, op, file, params, dir, show);

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
        return CallOriginalShellExecuteW(hwnd, op, file, params, dir, show);

    if (!uri.empty()) {
        auto result = ResolveUri(uri, hwnd);
        if (result.intercept) {
            if (!result.target.empty()) LaunchTarget(result.target);
            return (HINSTANCE)33;
        }
    }
    return CallOriginalShellExecuteW(hwnd, op, file, params, dir, show);
}
BOOL WINAPI CreateProcessW_hook(LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
                                 LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                 BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
                                 LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo,
                                 LPPROCESS_INFORMATION lpProcessInformation) {
    if (g_unloading.load(std::memory_order_acquire) || IsChildProcess() || !RedirectsEnabled() ||
        UiOnlyRedirectsEnabled()) {
        return CallOriginalCreateProcessW(lpApplicationName, lpCommandLine,
            lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags,
            lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
    }

    HookGuard guard;
    if (guard.IsReentrant()) {
        return CallOriginalCreateProcessW(lpApplicationName, lpCommandLine,
            lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags,
            lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
    }

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
    return CallOriginalCreateProcessW(lpApplicationName, lpCommandLine,
        lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags,
        lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}
// If a late (post-Wh_ModInit) registration succeeds, the operation still
// needs to be applied explicitly - Windhawk only auto-applies the batch that
// was queued while Wh_ModInit was running. Returns whether the hook is
// actually active and safe to mark "installed".
static bool ApplyLateHookIfNeeded() {
    if (!g_modInitComplete.load(std::memory_order_acquire)) {
        // Still inside the initial Wh_ModInit batch; Windhawk will apply it
        // automatically once Wh_ModInit returns.
        return true;
    }
    if (!Wh_ApplyHookOperations()) {
        Wh_Log(L"[STABILITY] Wh_ApplyHookOperations failed for a late hook batch");
        return false;
    }
    return true;
}

static std::atomic_bool g_pniduiHookFailed{false};

static bool TryInstallPniduiHook() {
    if (g_unloading.load(std::memory_order_acquire)) return false;
    try {
        std::lock_guard<std::mutex> lk(g_pniduiHookMutex);
        if (g_unloading.load(std::memory_order_acquire)) return false;

        if (g_pniduiHookInstalled.load(std::memory_order_acquire)) {
            return true;
        }
        // Only retry module/symbol resolution if we haven't already tried
        // and failed - repeated HookSymbols calls against the same module
        // invalidate its symbol cache and force a slow re-resolution.
        if (g_pniduiHookFailed.load(std::memory_order_acquire)) {
            return false;
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
        if (!result) {
            g_pniduiHookFailed.store(true, std::memory_order_release);
            return false;
        }
        result = ApplyLateHookIfNeeded();
        if (result) {
            g_pniduiHookInstalled.store(true, std::memory_order_release);
        }
        return result;
    } catch (const std::exception&) {
        Wh_Log(L"[STABILITY] pnidui symbol hook setup caught std::exception");
    } catch (...) {
        Wh_Log(L"[STABILITY] pnidui symbol hook setup caught an unknown exception");
    }
    return false;
}

static std::atomic_bool g_sndVolSSOHookInstalled{false};
static std::atomic_bool g_sndVolSSOHookFailed{false};
static std::atomic_bool g_shell32HooksInstalled{false};
static std::atomic_bool g_shell32HooksFailed{false};
static std::mutex g_immersiveMenuHookMutex;
static std::mutex g_shell32HookMutex;

static void InstallImmersiveMenuHooks() {
    if (g_unloading.load(std::memory_order_acquire)) return;
    try {
        std::lock_guard<std::mutex> installLock(g_immersiveMenuHookMutex);
        if (g_unloading.load(std::memory_order_acquire)) return;
        if (!g_sndVolSSOHookInstalled.load(std::memory_order_acquire) &&
            !g_sndVolSSOHookFailed.load(std::memory_order_acquire)) {
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
                    if (ApplyLateHookIfNeeded()) {
                        g_sndVolSSOHookInstalled.store(true, std::memory_order_release);
                    }
                } else {
                    g_sndVolSSOHookFailed.store(true, std::memory_order_release);
                }
            }
        }

        if (!g_pniduiHookInstalled.load(std::memory_order_acquire)) {
            TryInstallPniduiHook();
        }
    } catch (const std::exception&) {
        Wh_Log(L"[STABILITY] Immersive menu hook setup caught std::exception");
    } catch (...) {
        Wh_Log(L"[STABILITY] Immersive menu hook setup caught an unknown exception");
    }
}

// Combines all shell32.dll hooks into a single stable HookSymbols call per the
// Windhawk API best practice. The hook list is intentionally not conditional
// on settings or OS version, to keep Windhawk symbol caching valid.
static void InstallShell32Hooks() {
    if (g_unloading.load(std::memory_order_acquire)) return;
    try {
    std::lock_guard<std::mutex> lk(g_shell32HookMutex);
    if (g_unloading.load(std::memory_order_acquire)) return;
    if (g_shell32HooksInstalled.load(std::memory_order_acquire)) return;
    // Already tried and the symbols weren't hookable - don't keep re-resolving
    // the module on every settings change / tray recreation.
    if (g_shell32HooksFailed.load(std::memory_order_acquire)) return;

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
        if (ApplyLateHookIfNeeded()) {
            g_shell32HooksInstalled.store(true, std::memory_order_release);
            Wh_Log(L"[SHELL32-HOOKS] Installed shell32 hook set");
        }
    } else {
        g_shell32HooksFailed.store(true, std::memory_order_release);
    }
    } catch (const std::exception&) {
        Wh_Log(L"[STABILITY] shell32 hook setup caught std::exception");
    } catch (...) {
        Wh_Log(L"[STABILITY] shell32 hook setup caught an unknown exception");
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
    if (g_unloading.load(std::memory_order_acquire)) return;
    try {
        RemoveTraySubclass();

        {
            std::lock_guard<std::mutex> lk(g_trayDllInfoMutex);
            g_sndVolSSOBase = nullptr;
            g_sndVolSSOEnd  = nullptr;
            g_pniduiBase    = nullptr;
            g_pniduiEnd     = nullptr;
        }

        if (RedirectSystemTrayEnabled()) {
            SetupTraySubclass();
        }

if (RedirectSystemTrayEnabled()) {
    InstallImmersiveMenuHooks();
}        InstallShell32Hooks();
    } catch (const std::exception&) {
        Wh_Log(L"[STABILITY] ReinitializeTrayRedirect caught std::exception");
    } catch (...) {
        Wh_Log(L"[STABILITY] ReinitializeTrayRedirect caught an unknown exception");
    }
}

static void PerformBackgroundInit(bool skipSleep = false) {
    if (g_unloading.load(std::memory_order_acquire)) return;
    if (!skipSleep) {
        HANDLE stopEvent = g_stopEvent;
        if (stopEvent && WaitForSingleObject(stopEvent, 200) == WAIT_OBJECT_0) {
            return;
        }
    }

    if (g_unloading.load(std::memory_order_acquire)) return;
if (RedirectSystemTrayEnabled()) {
    InstallImmersiveMenuHooks();
}    if (g_unloading.load(std::memory_order_acquire)) return;
    InstallShell32Hooks();

if (!g_unloading.load(std::memory_order_acquire) && g_isWin11 && ComActivationRedirectEnabled()) {
    InstallAAMHook();
}
}

static DWORD WINAPI TraySubclassWatchdogThread(LPVOID) {
    try {
        PerformBackgroundInit();

        const int   FAST_PHASE_CHECKS   = 60;
        const DWORD FAST_INTERVAL_MS    = 500;
        const DWORD SLOW_INTERVAL_MS    = 3000;

        int tick = 0;
        while (!g_unloading.load(std::memory_order_acquire)) {
            DWORD interval = tick < FAST_PHASE_CHECKS ? FAST_INTERVAL_MS : SLOW_INTERVAL_MS;
            HANDLE stopEvent = g_stopEvent;
            if (!stopEvent) break;
            DWORD wait = WaitForSingleObject(stopEvent, interval);
            if (wait == WAIT_OBJECT_0) break;
            if (wait == WAIT_FAILED) {
                Wh_Log(L"[STABILITY] Tray watchdog wait failed (%lu)", GetLastError());
                break;
            }

            tick++;

            if (HasTrayBeenRecreated()) {
                ReinitializeTrayRedirect();
                continue;
            }

            if (!RedirectSystemTrayEnabled()) continue;

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
    } catch (const std::exception&) {
        Wh_Log(L"[STABILITY] Tray watchdog caught std::exception and is stopping");
    } catch (...) {
        Wh_Log(L"[STABILITY] Tray watchdog caught an unknown exception and is stopping");
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
    if (!CreateWindowExW_Original) {
        SetLastError(ERROR_PROC_NOT_FOUND);
        return nullptr;
    }

    HWND hwnd = CreateWindowExW_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (g_unloading.load(std::memory_order_acquire) || !RedirectSystemTrayEnabled() || !hwnd ||
        !lpClassName || IS_INTRESOURCE(lpClassName) || lpClassName[0] != L'T') {
        return hwnd;
    }

    HookGuard guard;
    if (guard.IsReentrant() || wcscmp(lpClassName, L"ToolbarWindow32") != 0) {
        return hwnd;
    }

    bool trayToolbarMissing = false;
    {
        std::lock_guard<std::mutex> lk(g_traySubclassMutex);
        trayToolbarMissing = (g_hTrayToolbar == nullptr);
    }
    if (trayToolbarMissing) {
        SetupTraySubclass();
    }
    return hwnd;
}
BOOL Wh_ModInit() {
    try {
        g_unloading.store(false, std::memory_order_release);
        g_shellExecuteExHookRegistered.store(false, std::memory_order_release);
        g_shellExecuteHookRegistered.store(false, std::memory_order_release);
        g_createProcessHookRegistered.store(false, std::memory_order_release);
        DetectWindowsVersion();
        LoadSettings();
        BuildChildEnvironment();
        if (!InitMappings()) {
            Wh_Log(L"[STABILITY] Initial URI map construction failed");
            return FALSE;
        }

        ScopedHandle stopEvent(CreateEventW(nullptr, TRUE, FALSE, nullptr));
        if (!stopEvent) {
            Wh_Log(L"[STABILITY] CreateEventW failed (%lu)", GetLastError());
            return FALSE;
        }

        HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
        if (!hShell32) hShell32 = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!hShell32) {
            Wh_Log(L"[STABILITY] shell32.dll is unavailable");
            return FALSE;
        }

        FARPROC pExW = GetProcAddress(hShell32, "ShellExecuteExW");
        FARPROC pW = GetProcAddress(hShell32, "ShellExecuteW");
        if (!pExW || !pW) {
            Wh_Log(L"[STABILITY] Required ShellExecute exports are unavailable");
            return FALSE;
        }

        // From here hooks can be queued, so do not return FALSE: optional
        // failures are logged and their hook bodies remain pass-through.
        g_stopEvent = stopEvent.release();

        const bool shellExecuteExHookInstalled = WindhawkUtils::SetFunctionHook(
            (ShellExecuteExW_t)pExW, ShellExecuteExW_hook, &ShellExecuteExW_orig);
        g_shellExecuteExHookRegistered.store(shellExecuteExHookInstalled, std::memory_order_release);
        if (!shellExecuteExHookInstalled) {
            Wh_Log(L"[STABILITY] ShellExecuteExW hook could not be installed");
        }
        const bool shellExecuteHookInstalled = WindhawkUtils::SetFunctionHook(
            (ShellExecuteW_t)pW, ShellExecuteW_hook, &ShellExecuteW_orig);
        g_shellExecuteHookRegistered.store(shellExecuteHookInstalled, std::memory_order_release);
        if (!shellExecuteHookInstalled) {
            Wh_Log(L"[STABILITY] ShellExecuteW hook could not be installed");
        }

        HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
        if (!hKernel32) hKernel32 = LoadLibraryExW(L"kernel32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (hKernel32) {
            void* pCPW = (void*)GetProcAddress(hKernel32, "CreateProcessW");
            if (pCPW) {
                const bool createProcessHookInstalled = WindhawkUtils::SetFunctionHook(
                    (CreateProcessW_t)pCPW, CreateProcessW_hook, &CreateProcessW_orig);
                g_createProcessHookRegistered.store(createProcessHookInstalled, std::memory_order_release);
                if (!createProcessHookInstalled) {
                    Wh_Log(L"[STABILITY] CreateProcessW hook could not be installed");
                }
            }
        } else {
            Wh_Log(L"[STABILITY] kernel32.dll unavailable; process interception skipped");
        }

        if (IsShellProcess()) {
            if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                                 &CreateWindowExW_Original)) {
                Wh_Log(L"[STABILITY] CreateWindowExW hook could not be installed");
            }
            if (RedirectSystemTrayEnabled()) {
                SetupTraySubclass();
            }

            HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
            if (!hUser32) hUser32 = LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
            if (hUser32) {
                FARPROC pTrackPopupMenuEx = GetProcAddress(hUser32, "TrackPopupMenuEx");
                if (pTrackPopupMenuEx && !WindhawkUtils::SetFunctionHook(
                        (TrackPopupMenuEx_t)pTrackPopupMenuEx, TrackPopupMenuEx_Hook,
                        &g_origTrackPopupMenuEx)) {
                    Wh_Log(L"[STABILITY] TrackPopupMenuEx hook could not be installed");
                }
            }

            // Queue all symbol/vtable hooks that might be needed before Wh_ModInit
            // returns, so Windhawk can apply them as part of normal initialization
            // without explicit Wh_ApplyHookOperations calls later.
            PerformBackgroundInit(true);
            // Everything queued above rides along with Windhawk's automatic
            // post-Wh_ModInit apply. Anything installed after this point is a
            // late registration and must call Wh_ApplyHookOperations() itself.
            g_modInitComplete.store(true, std::memory_order_release);

            {
                std::lock_guard<std::mutex> lk(g_shellTrayWndMutex);
                g_lastShellTrayWnd = nullptr;
            }
            g_traySubclassWatchdogThread = CreateThread(nullptr, 0, TraySubclassWatchdogThread,
                                                         nullptr, 0, nullptr);
            if (!g_traySubclassWatchdogThread) {
                Wh_Log(L"[STABILITY] Tray watchdog thread was not created (%lu)", GetLastError());
            }
        }

        return TRUE;
    } catch (const std::exception&) {
        // If no hook was queued this cleanly unloads; if a rare exception came
        // after a hook was queued, remain loaded in transparent mode so the
        // engine can remove the queued operations normally.
        Wh_Log(L"[STABILITY] Wh_ModInit caught std::exception");
    } catch (...) {
        Wh_Log(L"[STABILITY] Wh_ModInit caught an unknown exception");
    }

    g_unloading.store(true, std::memory_order_release);
    if (g_stopEvent) SetEvent(g_stopEvent);
    return TRUE;
}
static void StopTraySubclassWatchdog() {
    // The worker can resolve symbols and queue optional hooks. It must be gone
    // before Wh_ModBeforeUninit returns, because Windhawk may remove hooks as
    // soon as that callback finishes.
    if (g_traySubclassWatchdogThread) {
        DWORD wait = WaitForSingleObject(g_traySubclassWatchdogThread, INFINITE);
        if (wait != WAIT_OBJECT_0) {
            Wh_Log(L"[STABILITY] Watchdog join failed (%lu)", GetLastError());
        }
        CloseHandle(g_traySubclassWatchdogThread);
        g_traySubclassWatchdogThread = nullptr;
    }
}

void Wh_ModBeforeUninit() {
    // This callback still runs while original trampolines and subclass helpers
    // are valid. Stop new custom work first, then remove our external window
    // callback before Windhawk removes function hooks.
    try {
        g_unloading.store(true, std::memory_order_release);
        if (g_stopEvent) {
            SetEvent(g_stopEvent);
        }
        StopTraySubclassWatchdog();
        RemoveTraySubclass();
    } catch (const std::exception&) {
        Wh_Log(L"[STABILITY] Wh_ModBeforeUninit caught std::exception");
    } catch (...) {
        Wh_Log(L"[STABILITY] Wh_ModBeforeUninit caught an unknown exception");
    }
}



void Wh_ModUninit() {
    try {
        g_unloading.store(true, std::memory_order_release);
        if (g_stopEvent) {
            SetEvent(g_stopEvent);
        }

        // The watchdog executes mod code. Do not allow the DLL to unload while
        // it is still alive; it only waits on g_stopEvent and exits promptly.
        if (g_traySubclassWatchdogThread) {
            DWORD wait = WaitForSingleObject(g_traySubclassWatchdogThread, INFINITE);
            if (wait != WAIT_OBJECT_0) {
                Wh_Log(L"[STABILITY] Watchdog join failed (%lu)", GetLastError());
            }
            CloseHandle(g_traySubclassWatchdogThread);
            g_traySubclassWatchdogThread = nullptr;
        }
RemoveTraySubclass();

if (g_stopEvent) {
    CloseHandle(g_stopEvent);
    g_stopEvent = nullptr;
}

{
    std::lock_guard<std::mutex> lk(g_bounceGuardMtx);
    g_bounceGuard.clear();
}
{
    std::lock_guard<std::mutex> lk(g_loopGuardMtx);
    g_loopGuard.clear();
}
{
    std::lock_guard<std::mutex> lk(g_mappingsMutex);
    g_mappings.clear();
}
g_childEnvBlock.clear();
    } catch (const std::exception&) {
        Wh_Log(L"[STABILITY] Wh_ModUninit caught std::exception");
    } catch (...) {
        Wh_Log(L"[STABILITY] Wh_ModUninit caught an unknown exception");
    }
}
void Wh_ModSettingsChanged() {
    if (g_unloading.load(std::memory_order_acquire)) return;
    try {
        LoadSettings();
        InitMappings();

        if (IsShellProcess()) {
            if (RedirectSystemTrayEnabled()) {
                SetupTraySubclass();
            } else {
                RemoveTraySubclass();
            }
            PerformBackgroundInit(true);
        }
    } catch (const std::exception&) {
        Wh_Log(L"[STABILITY] Wh_ModSettingsChanged caught std::exception; previous state is retained where possible");
    } catch (...) {
        Wh_Log(L"[STABILITY] Wh_ModSettingsChanged caught an unknown exception; previous state is retained where possible");
    }
}
