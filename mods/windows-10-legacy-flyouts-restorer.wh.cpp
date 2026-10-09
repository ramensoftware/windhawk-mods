// ==WindhawkMod==
// @id              windows-10-legacy-flyouts-restorer
// @name            Windows 10 legacy flyouts on Win11 24H2 restorer
// @description     This mod restores the Windows 10 network and battery flyouts in the private Windows 10 shell running on Windows 11 24H2+
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @license         GPL-3.0
// @architecture    x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lole32 -loleaut32 -lgdi32 -lshell32 -ladvapi32 -luser32 -lwintrust -lcrypt32 -lwininet -lbcrypt -lcomctl32 -luuid -lwlanapi -lruntimeobject
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Windows 10 legacy flyouts on Win11 24H2 restorer

This mod restores the Windows 10 network and battery flyouts (network icon with its menu, battery
icon with the flyout, volume icon) on Windows 11 24H2. It does not patch the Windows 11 shell: it
runs inside the *private* Windows 10 shell, so it **requires**
[Windows 10 taskbar on Win11 24H2](https://windhawk.net/mods/win10-taskbar-on-win11-24h2) (by
Anixx) to be installed and running. In that mod's terminology this mod is the tray half of the
setup: it uses the same verified copy of the Windows 10 files, the same data folder, and it does
not start anything by itself. Outside the private shell the mod loads and does nothing: no hook is
installed, nothing is written, so the Windows 11 shell is never touched.

What it does, inside the private shell:

- The Windows 10 tray modules (`pnidui.dll` for the network icon, `stobject.dll` for battery and
  volume) are downloaded from the Microsoft symbol server, verified by pinned SHA-256 and, by
  default, by Authenticode signature, then loaded. The strings those two modules ask their `.mui`
  for are served in memory, because this build ships no MUI for them.
- A left click on the network or the battery icon is consumed at the icon and turned into the
  authentic Windows 10 flyout request (`CLSID_ImmersiveShell` -> `ShellExperienceManagerFactory`
  -> the flyout experience of that icon). Right click, middle click and every other message pass
  through unchanged. The right-click menu of the network icon is built from the two entries
  `pnidui` itself expects, because its menu resource is gone in this build.
- The flyout window drawn by `ShellExperienceHost.exe` is made to use the Windows 10 template set,
  otherwise that host tears the flyout down a moment after it has built it.

Not included on purpose: the three quick-action tiles (Wi-Fi, airplane mode, hotspot) keep the
stock Windows 11 template, styling and behaviour; the Action Center button and its animation are
left to dedicated mods.

## Behaviour worth knowing

- **Network pages are answered by the flyout.** Inside the private shell, every launch of
  `ms-settings:network*`, `ms-settings:wifi`, `vpn`, `airplanemode`, `mobilehotspot`,
  `ms-availablenetworks:` or the network control-panel items is turned into a request for the
  Windows 10 flyout and reported as started, for *any* caller in that process - not only the
  tray icon. So Win+R with `ms-settings:wifi` in the Windows 10 shell opens the flyout instead of
  the Settings page. This is the point of the mod (that page does not exist in the Windows 10
  shell any more), but it is a global rewrite in that process.
- **The download stays.** Files are fetched at run time from `msdl.microsoft.com` into the shared
  store; nothing else is fetched. A failed download is retried with a backoff (1 minute, doubling
  up to 15), never on every tick.
- **What is written and where it persists.** The verified files go to the data folder. The tray
  state backup and one-time marker normally go to this mod's Windhawk storage; if that path is
  unavailable, the code falls back to the shared data folder. Windhawk removes its mod storage on
  uninstall, so copy the backup elsewhere first if you may need it later. The "Peek at desktop"
  toggle is process-local: this mod does not write `DisablePreviewDesktop` or `EnableAeroPeek`.
  While active, its registry-query hook supplies the toggled values to the private shell; on normal
  unload those virtual values are dropped and the shell is notified to reread the real settings. A
  crash cannot leave registry values changed by this menu. The optional tray reset
  (`ForceNetworkTrayResetTraySettings`) is a separate, deliberate, one-time deletion of shared
  shell state after a complete backup; it is not undone automatically - see its setting.
- **In `ShellExperienceHost.exe`.** The template-set change is reverted when the mod is disabled.
  The module that was patched is also *pinned* (`GET_MODULE_HANDLE_EX_FLAG_PIN`), and there is no
  documented way to un-pin a module: `Windows.UI.QuickActions.dll` therefore stays loaded until
  `ShellExperienceHost.exe` ends. Nothing is written to disk and no other process is touched.
- **Architecture.** `@architecture x86-64`: everything this mod does - the byte patterns of the
  tray modules, the QuickActions pattern, the import-table entry of `pnidui` - is written for x64.
  Note that on ARM64 Windhawk still loads an `x86-64` mod into `ShellExperienceHost.exe`
  natively; there the pattern does not match, so the mod logs that and writes nothing, but a
  flyout opened in that process will not be held open. Use an x64 Windows if you need this mod.

## Example Screenshot (Battery Flyout)

![Battery flyout example](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/batteryflyoutexample.png)

## Credits

- [Anixx](https://github.com/Anixx) - the Windows 10 taskbar mod whose verified files and data
  folder this mod shares, and whose tray-module loading it follows
- ExplorerPatcher (by valinet) - research for the Windows 10 flyouts running on Windows 11
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ForceNetworkTrayIcon: true
  $name: Force the network icon into the taskbar
  $description: >-
    This setting forces the network icon into the taskbar. If the native PNI (pnidui.dll)
    does not register the network icon within the delay below, the mod registers it itself
    with Shell_NotifyIconW, its own owner window and the system GUID of the network icon,
    and verifies with Shell_NotifyIconGetRect that the icon really is in the taskbar. When
    the native icon appears, the forced one is retired by itself. ms-availablenetworks: is
    never used as a substitute.
- ForceNetworkTrayDelaySec: 12
  $name: Wait before forcing the icon (seconds)
  $description: >-
    This setting controls the wait before forcing the icon. It is how long the native icon
    is given before the mod registers it itself (0-600).
- ForceNetworkTrayResetTraySettings: false
  $name: Reset the saved tray state if the icon still does not appear
  $description: >-
    This setting resets the saved tray state if the icon still does not appear. Off by
    default; opt in only if the icon still does not appear with everything else on. Last
    resort: IconStreams/PastIconsStream under TrayNotify are permanently deleted once, but
    only after every existing value has been read completely as REG_BINARY and a complete
    backup plus a one-time marker have been written. If a read or backup fails, neither
    value is deleted. This is still a real, permanent change to shared shell state: it
    survives disabling or uninstalling the mod (Windows recreates both values the next
    time it needs them), and it is not undone automatically. The real registry values under
    TrayNotify are never touched by the other steps, only by this explicit, one-time, opt-in
    action.
- ShellOpGuardTimeoutMs: 1500
  $name: Time limit for the guarded shell operations (ms)
  $description: >-
    This setting sets the time limit for the guarded shell operations. The shell operations
    started from a menu (for example "Customize notification area icons") run on a service
    thread with this time cap (200-10000).
- ProvideTrayModules: true
  $name: Load the Windows 10 tray modules
  $description: >-
    This setting loads the Windows 10 tray modules. Downloads pnidui.dll and stobject.dll,
    verifies them (SHA-256 and signature) and loads them in the private Windows 10 shell:
    network icon with its menu, volume icon with the battery flyout.
- themeScheme: auto
  $name: Colour scheme of the tray menus
  $description: >-
    This setting sets the colour scheme of the menus this mod draws (the right-click menu of
    the network icon and the one of the battery icon). auto follows the Windows app theme
    (Settings > Personalization > Colors, the same AppsUseLightTheme value Explorer itself
    uses); light/dark force the Windows 10 light or dark owner-drawn menu regardless of the
    current system theme.
  $options:
  - auto: Follow the Windows theme
  - light: Always light
  - dark: Always dark
- LogTrayActivity: false
  $name: Log every tray operation
  $description: >-
    This setting logs every tray operation. Writes every LoadLibrary, class factory and menu
    operation to the log. Useful to see which module the shell is asking for; it can be
    verbose.
- RequireSignature: true
  $name: Check the signature of the downloaded files
  $description: >-
    This setting checks the signature of the downloaded files. Every file is checked against
    its pinned SHA-256 first. With this on, the Authenticode signature is checked as well
    (Microsoft signer); with it off a matching hash is enough.
- DownloadTimeoutSec: 20
  $name: Download timeout (seconds)
  $description: >-
    This setting sets the download timeout. Connection, receive and send timeout of the
    downloads of the Windows 10 shell files.
*/
// ==/WindhawkModSettings==

// The change history of this mod lives in the pull request that publishes it, not in the
// source: what used to be here was a long, partly Italian log of the component this code grew
// out of, and it described features (a userinit shell redirection, a per-user "Shell" value, a
// watchdog, an emergency hotkey, the Win+X menu, UWP taskbar buttons) that this mod does not
// have at all. That misled every reader and every review of it, so it is gone. The pieces this
// file still borrows the *shape* of are credited where they are used: the tray modules and the
// store folder come from the Windows 10 taskbar mod by Anixx (win10-taskbar-on-win11-24h2),
// and the flyout call chain follows what ExplorerPatcher (valinet) documented about the
// Windows 10 shell.

// winsock2.h belongs before windows.h. windows.h is compiled with WIN32_LEAN_AND_MEAN (see
// @compilerOptions), so it does not pull MinGW's winsock 1 in and winsock2.h is happy to be
// included here. Nothing in this mod uses the socket API (the downloads go through WinINet);
// the header is kept because some of the shell headers below expect the winsock types.
#include <winsock2.h>

#include <windows.h>
#include <Unknwn.h>
#include <oaidl.h>      // IDispatch, DISPID, DISPPARAMS (the toggle of "show desktop")
#include <combaseapi.h>
#include <bcrypt.h>
#include <wincrypt.h>
#include <wintrust.h>
#include <wininet.h>
#include <shellapi.h>
#include <shlobj.h>     // SHParseDisplayName / SHOpenFolderAndSelectItems
#include <commctrl.h>   // SetWindowSubclass, the subclasses of the shell's windows
#include <strsafe.h>
#include <tlhelp32.h>   // the snapshot of the process (loaded modules and such)
#include <winternl.h>   // UNICODE_STRING for the LdrLoadDll hook
#include <wlanapi.h>    // adapter and signal read of the NLM network fallback
#include <iphlpapi.h>   // adapter mapping of the NLM network fallback
#include <netlistmgr.h> // the NLM state of the network fallback
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// Only C++/WinRT's base header is used, and only for winrt::com_ptr and
// winrt::hresult_error - the two types the NLM lookup and the class-factory
// resolution are written with. No WinRT component header is included: MinGW's
// toolchain ships winrt/base.h, while the component headers (for example
// winrt/Windows.UI.Xaml.h) are not there and broke the build.
#include <winrt/base.h>

#include <algorithm>
#include <atomic>
#include <mutex>
#include <string>
#include <vector>

#include <windhawk_api.h>
#include <windhawk_utils.h>

// The code resolves IP Helper dynamically; keep these stable GetAdaptersAddresses flags
// available even when an SDK target macro hides their declarations.
#ifndef GAA_FLAG_SKIP_ANYCAST
#define GAA_FLAG_SKIP_ANYCAST 0x00000002
#endif
#ifndef GAA_FLAG_SKIP_MULTICAST
#define GAA_FLAG_SKIP_MULTICAST 0x00000004
#endif
#ifndef GAA_FLAG_SKIP_DNS_SERVER
#define GAA_FLAG_SKIP_DNS_SERVER 0x00000008
#endif

// The shell this module lives in. In the monolith g_unloading lived among the globals of
// the shell-services section; here it is the only flag of that kind.
static std::atomic<bool> g_unloading{false};

// Options of this module. They are read once and copied into the fields of the monolith
// configuration the tray/menu code already uses (g_cfg), so those blocks stay as they are.
static bool g_logTrayActivity = false;

// _ReturnAddress is what the caller of a hook is read with; clang and gcc have the
// builtin, MSVC needs the intrinsic header.
#if defined(_MSC_VER) && !defined(__clang__) && !defined(__GNUC__)
#include <intrin.h>
#endif

// The exception boundaries of this mod: C++ try/catch only. No VEH is registered, no
// structured-exception syntax is used, no CONTEXT record is rewritten, no recovery from a
// native fault is claimed. The namespace stays at global scope: Wh_ModInit uses it.
namespace CppGuard {

static constexpr DWORD kCppExceptionCode = 0xE06D7363u;
static std::atomic<unsigned int> g_cppExceptionLogs{0};

// Catches only language-level C++ and C++/WinRT exceptions. Hardware/OS
// exceptions are not translated to C++ and must remain unhandled for diagnosis.
static bool RunGuarded(const wchar_t* tag, void (*fn)(void*), void* ctx,
                       DWORD* outFault) noexcept {
    if (outFault) *outFault = 0;
    if (!fn) return false;
    try {
        fn(ctx);
        return true;
    } catch (winrt::hresult_error const& ex) {
        if (outFault) *outFault = kCppExceptionCode;
        if (g_cppExceptionLogs.fetch_add(1, std::memory_order_relaxed) < 8)
            Wh_Log(L"[cpp-guard] %s: C++/WinRT exception (0x%08X)",
                   tag ? tag : L"mod operation", static_cast<unsigned>(ex.code()));
    } catch (std::exception const&) {
        if (outFault) *outFault = kCppExceptionCode;
        if (g_cppExceptionLogs.fetch_add(1, std::memory_order_relaxed) < 8)
            Wh_Log(L"[cpp-guard] %s: std::exception caught", tag ? tag : L"mod operation");
    } catch (...) {
        if (outFault) *outFault = kCppExceptionCode;
        if (g_cppExceptionLogs.fetch_add(1, std::memory_order_relaxed) < 8)
            Wh_Log(L"[cpp-guard] %s: C++ exception caught", tag ? tag : L"mod operation");
    }
    return false;
}

}  // namespace CppGuard

namespace RestorerTaskbar {

// Declared at the end of the namespace, needed here by the tray block that calls it earlier.
static bool IsLegacyShellProcess();

#if defined(__clang__) || defined(__GNUC__)
#define WhReturnAddress() __builtin_return_address(0)
#elif defined(_MSC_VER)
#define WhReturnAddress() ::_ReturnAddress()
#else
#error Unsupported compiler for WhReturnAddress
#endif

// ===========================================================================
// What this mod downloads, and under which pinned identity
// ===========================================================================
//
// Every file comes from the Microsoft symbol server, where a binary is named by
// <TimeDateStamp><SizeOfImage> (the "symbol id") instead of by its version. The id is what the
// URL is built from, the SHA-256 is what the file is checked against; the two together pin one
// exact build, and no other file name is ever fetched.
//
// The build chosen for explorer.exe is 10.0.19039.1, the one
// "Win10 taskbar on Win11 24H2 or 25H2" (Anixx) downloads: same id, same hash, same folder, so
// the two mods share a single verified copy whoever needs it first.

// One file of the store: the name it has on disk, the symbol id, the pinned hash.
struct PinnedFile {
    const wchar_t* name;
    const wchar_t* symbolId;
    const wchar_t* sha256;
};

// The two tray modules, asked for by every load of those names in the private shell.
static const PinnedFile kTrayFiles[] = {
    // pnidui.dll 10.0.19041.7663, stobject.dll 10.0.19041.7663 - the Windows 10 builds of the
    // network icon and of the battery / volume tray pair.
    { L"pnidui.dll",  L"CC2D6BBC219000", L"7c8fa315e73e22c0d66c1b424118e3441251fe3c0e2b557e4dbf16166d14411c" },
    { L"stobject.dll", L"465AE25A52000", L"7c0037535c4da20ae15b4df9662f9330a989d4e2f36284aac5c9c3bce429e118" },
};

// explorer.exe 10.0.19039.1 - the private Windows 10 shell, the same file the taskbar mod
// downloads. It is not loaded here, only made sure to exist: the taskbar mod starts it.
static const PinnedFile kExplorerFile = {
    L"explorer.exe", L"7AC6EEC3442000",
    L"58f78b5f90efc75d6c7d3d85bc8b36983fe410406f217619dbe2384130d65bfe",
};

// windows.ui.search.dll is deliberately not loaded: the Search submenu changes the standard
// SearchboxTaskbarMode setting through public Win32 APIs and does not inject a separate Search
// host. Its verified identity is retained for reference: id = 4D6D1C59e5000,
// sha256 = 8950639236b5000973ff14f7a57577066f4c075a52e8582fe11b8eb75526522e.

static const wchar_t* kSymbolUrlFmt = L"https://msdl.microsoft.com/download/symbols/%s/%s/%s";

// What the settings amount to, in the only form the rest of the source reads them. The
// paths are buffers because every file of the store is named after them.
static struct {
    wchar_t storePath[MAX_PATH];       // the verified files, shared with the taskbar mod
    wchar_t explorerPath[MAX_PATH];    // storePath\explorer.exe, the private shell
    bool provideTrayDlls;              // download + load pnidui.dll / stobject.dll
    bool requireSignature;             // a file whose signature fails is not used
    int  downloadTimeoutSec;
    bool forceNetworkTrayIcon;             // register the network icon if pnidui never does
    int  forceNetworkTrayDelaySec;         // how long to wait before stepping in (s)
    bool forceNetworkTrayResetTraySettings;// clear the stored tray state once, with a backup
    int  shellOpGuardTimeoutMs;            // time cap of a shell::: operation
} g_cfg = {};

// The real path of the running image, captured before the hooks that can spoof it. Every
// "am I the private shell?" answer comes from this.
static wchar_t g_realExePath[MAX_PATH] = {};

// ===========================================================================
// RAII
// ===========================================================================

class ScopedHandle {
public:
    explicit ScopedHandle(HANDLE h = nullptr) : m_h(h) {}
    ~ScopedHandle() { reset(); }
    ScopedHandle(const ScopedHandle&) = delete;
    ScopedHandle& operator=(const ScopedHandle&) = delete;
    HANDLE get() const { return m_h; }
    bool valid() const { return m_h != nullptr && m_h != INVALID_HANDLE_VALUE; }
    void reset(HANDLE h = nullptr) {
        if (valid()) CloseHandle(m_h);
        m_h = h;
    }
private:
    HANDLE m_h;
};

class ScopedRegKey {
public:
    ScopedRegKey() = default;
    ~ScopedRegKey() { reset(); }
    ScopedRegKey(const ScopedRegKey&) = delete;
    ScopedRegKey& operator=(const ScopedRegKey&) = delete;
    HKEY* put() { reset(); return &m_h; }
    HKEY get() const { return m_h; }
    HKEY release() noexcept { HKEY h = m_h; m_h = nullptr; return h; }
    bool valid() const { return m_h != nullptr; }
    void reset() { if (m_h) { RegCloseKey(m_h); m_h = nullptr; } }
private:
    HKEY m_h = nullptr;
};

class ScopedGdiObj {
public:
    explicit ScopedGdiObj(HGDIOBJ o = nullptr) : m_o(o) {}
    ~ScopedGdiObj() { if (m_o) DeleteObject(m_o); }
    ScopedGdiObj(const ScopedGdiObj&) = delete;
    ScopedGdiObj& operator=(const ScopedGdiObj&) = delete;
    HGDIOBJ get() const { return m_o; }
private:
    HGDIOBJ m_o;
};

class ScopedMenu {
public:
    explicit ScopedMenu(HMENU menu = nullptr) : m_menu(menu) {}
    ~ScopedMenu() { if (m_menu) DestroyMenu(m_menu); }
    ScopedMenu(const ScopedMenu&) = delete;
    ScopedMenu& operator=(const ScopedMenu&) = delete;
    HMENU get() const { return m_menu; }
    HMENU release() { HMENU menu = m_menu; m_menu = nullptr; return menu; }
private:
    HMENU m_menu;
};

class ScopedInternet {
public:
    explicit ScopedInternet(HINTERNET h = nullptr) : m_h(h) {}
    ~ScopedInternet() { if (m_h) InternetCloseHandle(m_h); }
    ScopedInternet(const ScopedInternet&) = delete;
    ScopedInternet& operator=(const ScopedInternet&) = delete;
    HINTERNET get() const { return m_h; }
    bool valid() const { return m_h != nullptr; }
private:
    HINTERNET m_h;
};

// Restores the previously selected GDI object (SelectObject is sticky).
class ScopedSelectedObject {
public:
    ScopedSelectedObject(HDC hdc, HGDIOBJ obj) : m_hdc(hdc), m_old(nullptr) {
        if (obj) m_old = SelectObject(hdc, obj);
    }
    ~ScopedSelectedObject() { if (m_old && m_old != HGDI_ERROR) SelectObject(m_hdc, m_old); }
    ScopedSelectedObject(const ScopedSelectedObject&) = delete;
    ScopedSelectedObject& operator=(const ScopedSelectedObject&) = delete;
private:
    HDC m_hdc;
    HGDIOBJ m_old;
};

// ===========================================================================

// The File Explorer ribbon switch (the "classicRibbonUI" branch of explorer-frame-classic by
// m417z) used to live here. It is removed: it has nothing to do with the flyouts, users who
// want the classic ribbon have that mod, and a mod must not change an unrelated part of the
// shell behind a setting the README does not mention.
// These boundaries handle C++ exceptions; no native-fault recovery is installed.
// ===========================================================================

namespace NativeUi {
// Only the process role is kept here: every other member of this namespace (the ribbon gate,
// the registry virtualization and its key-path helpers) had no caller left and is removed.
static bool privateExplorer = false;

}  // namespace NativeUi

static bool Sha256File(const wchar_t* path, std::wstring& outHex) {
    ScopedHandle file(CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
                                  OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr));
    if (!file.valid()) {
        Wh_Log(L"[hash] open failed (%lu): %s", GetLastError(), path);
        return false;
    }

    BCRYPT_ALG_HANDLE alg = nullptr;
    if (BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) != 0) {
        Wh_Log(L"[hash] BCryptOpenAlgorithmProvider failed");
        return false;
    }

    bool ok = false;
    BCRYPT_HASH_HANDLE hash = nullptr;
    BYTE hashObj[512] = {};
    DWORD hashObjLen = 0, cb = 0;
    BYTE digest[32] = {};
    BYTE buffer[64 * 1024];

    if (BCryptGetProperty(alg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&hashObjLen, sizeof(hashObjLen), &cb, 0) == 0 &&
        hashObjLen <= sizeof(hashObj) &&
        BCryptCreateHash(alg, &hash, hashObj, hashObjLen, nullptr, 0, 0) == 0) {
        for (;;) {
            DWORD read = 0;
            if (!ReadFile(file.get(), buffer, sizeof(buffer), &read, nullptr)) break;
            if (read == 0) {
                if (BCryptFinishHash(hash, digest, sizeof(digest), 0) == 0) ok = true;
                break;
            }
            if (BCryptHashData(hash, buffer, read, 0) != 0) break;
        }
        BCryptDestroyHash(hash);
    }
    BCryptCloseAlgorithmProvider(alg, 0);

    if (ok) {
        static const wchar_t* kHex = L"0123456789abcdef";
        outHex.clear();
        outHex.reserve(64);
        for (BYTE b : digest) {
            outHex.push_back(kHex[b >> 4]);
            outHex.push_back(kHex[b & 0xF]);
        }
    }
    return ok;
}

// Outcome of the signature check.
// NB: the most common Windows components (pnidui.dll, stobject.dll, ...) do NOT
// carry an embedded signature in the file: they are signed through the system
// catalog, which on a Windows 11 machine does not exist for Windows 10 files.
// Verified with osslsigncode: explorer.exe has an embedded Microsoft signature,
// pnidui/stobject/windows.ui.search have none. That is why a missing embedded
// signature is not an error, but it is reported: the identity of the file stays
// guaranteed by the pinned SHA-256 hash and the HTTPS origin (msdl.microsoft.com).
enum class SigResult { Ok, None, Bad };

static SigResult VerifyMicrosoftSignature(const wchar_t* path, bool* signerIsMicrosoft) {
    if (signerIsMicrosoft) *signerIsMicrosoft = false;

    WINTRUST_FILE_INFO fileInfo = {};
    fileInfo.cbStruct = sizeof(fileInfo);
    fileInfo.pcwszFilePath = path;

    WINTRUST_DATA data = {};
    data.cbStruct = sizeof(data);
    data.dwUIChoice = WTD_UI_NONE;
    data.fdwRevocationChecks = WTD_REVOKE_NONE;   // offline: no network at logon
    data.dwUnionChoice = WTD_CHOICE_FILE;
    data.pFile = &fileInfo;
    data.dwProvFlags = WTD_CACHE_ONLY_URL_RETRIEVAL | WTD_REVOCATION_CHECK_NONE;
    data.dwStateAction = WTD_STATEACTION_VERIFY;

    static const GUID kGenericVerifyV2 = {
    0xaac56b, 0xcd44, 0x11d0,
    {0x8c, 0xc2, 0x00, 0xc0, 0x4f, 0xc2, 0x95, 0xee}
};
GUID action = kGenericVerifyV2;    LONG status = WinVerifyTrust(nullptr, &action, &data);
    data.dwStateAction = WTD_STATEACTION_CLOSE;
    WinVerifyTrust(nullptr, &action, &data);

    if (status != ERROR_SUCCESS) {
        // 0x800B0100 TRUST_E_NOSIGNATURE: no embedded signature (catalog)
        if ((DWORD)status == (DWORD)TRUST_E_NOSIGNATURE) {
            Wh_Log(L"[sig] no embedded signature (component signed through the catalog): %s", path);
            return SigResult::None;
        }
        Wh_Log(L"[sig] signature NOT valid (0x%08X): %s", status, path);
        return SigResult::Bad;
    }

    // The signer of the file, read from the embedded certificate
    HCERTSTORE store = nullptr;
    HCRYPTMSG  msg = nullptr;
    if (CryptQueryObject(CERT_QUERY_OBJECT_FILE, path,
                         CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
                         CERT_QUERY_FORMAT_FLAG_BINARY, 0,
                         nullptr, nullptr, nullptr, &store, &msg, nullptr)) {
        DWORD size = 0;
        if (CryptMsgGetParam(msg, CMSG_SIGNER_INFO_PARAM, 0, nullptr, &size) && size) {
            std::string buf(size, '\0');
            if (CryptMsgGetParam(msg, CMSG_SIGNER_INFO_PARAM, 0, &buf[0], &size)) {
                CMSG_SIGNER_INFO* si = (CMSG_SIGNER_INFO*)&buf[0];
                CERT_INFO ci = {};
                ci.Issuer = si->Issuer;
                ci.SerialNumber = si->SerialNumber;
                PCCERT_CONTEXT ctx = CertFindCertificateInStore(
                    store, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                    CERT_FIND_SUBJECT_CERT, &ci, nullptr);
                if (ctx) {
                    wchar_t name[256] = {};
                    if (CertGetNameStringW(ctx, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, nullptr, name, _countof(name)) > 1) {
                        Wh_Log(L"[sig] signer: %s", name);
                        if (signerIsMicrosoft) *signerIsMicrosoft = (wcsstr(name, L"Microsoft") != nullptr);
                    }
                    CertFreeCertificateContext(ctx);
                }
            }
        }
        if (msg) CryptMsgClose(msg);
        if (store) CertCloseStore(store, 0);
    }
    return SigResult::Ok;
}

// A download must never hold up the unload of the mod.
//
// The downloads run on this mod's services thread and a single call here can block for the
// whole connect/receive timeout. Wh_ModBeforeUninit may not return while that thread runs -
// Windhawk unmaps the module right after, and the thread would execute code that is gone - so
// the session handle of the download in progress is published here: the unload takes it over
// and closes it, which makes the blocked call return. The exchange gives the handle to exactly
// one of the two parties, so it is never closed twice. On top of that the read loop looks at
// g_unloading between the chunks, so a download that is making progress stops at the next one.
static std::atomic<HINTERNET> g_downloadSession{nullptr};

static void CancelActiveDownload() noexcept {
    try {
        const HINTERNET session = g_downloadSession.exchange(nullptr, std::memory_order_acq_rel);
        if (!session) return;
        Wh_Log(L"[dl] the mod is being unloaded: the download in progress is cancelled");
        InternetCloseHandle(session);
    } catch (...) {
    }
}

static bool HttpDownloadToFile(const wchar_t* url, const wchar_t* destPath, DWORD timeoutSec) {
    if (g_unloading.load(std::memory_order_acquire)) return false;   // nothing starts while unloading

    HINTERNET session = InternetOpenW(L"Win10TaskbarClean/0.2", INTERNET_OPEN_TYPE_PRECONFIG,
                                       nullptr, nullptr, 0);
    if (!session) {
        Wh_Log(L"[dl] InternetOpen failed (%lu)", GetLastError());
        return false;
    }
    g_downloadSession.store(session, std::memory_order_release);
    struct ScopedSession {
        std::atomic<HINTERNET>& slot;
        HINTERNET self;
        ~ScopedSession() {
            if (slot.exchange(nullptr, std::memory_order_acq_rel) == self) InternetCloseHandle(self);
        }
    } sessionGuard{g_downloadSession, session};

    DWORD ms = (timeoutSec ? timeoutSec : 20) * 1000;
    InternetSetOptionW(session, INTERNET_OPTION_CONNECT_TIMEOUT, &ms, sizeof(ms));
    InternetSetOptionW(session, INTERNET_OPTION_RECEIVE_TIMEOUT, &ms, sizeof(ms));
    InternetSetOptionW(session, INTERNET_OPTION_SEND_TIMEOUT, &ms, sizeof(ms));

    ScopedInternet request(InternetOpenUrlW(session, url, nullptr, 0,
                                            INTERNET_FLAG_NO_UI | INTERNET_FLAG_RELOAD, 0));
    if (!request.valid()) {
        Wh_Log(L"[dl] opening the URL failed (%lu): %s", GetLastError(), url);
        return false;
    }

    DWORD status = 0, len = sizeof(status);
    if (!HttpQueryInfoW(request.get(), HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                        &status, &len, nullptr) || status != 200) {
        Wh_Log(L"[dl] HTTP response %lu for %s", status, url);
        return false;
    }

    ScopedHandle file(CreateFileW(destPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                                  FILE_ATTRIBUTE_TEMPORARY, nullptr));
    if (!file.valid()) {
        Wh_Log(L"[dl] creating the file failed (%lu): %s", GetLastError(), destPath);
        return false;
    }

    const DWORD kMaxBytes = 64u * 1024u * 1024u;   // a sanity limit
    BYTE buffer[64 * 1024];
    DWORD total = 0;
    for (;;) {
        if (g_unloading.load(std::memory_order_acquire)) {
            Wh_Log(L"[dl] unloading: the download is interrupted after %lu bytes", total);
            return false;
        }
        DWORD got = 0;
        if (!InternetReadFile(request.get(), buffer, sizeof(buffer), &got)) {
            Wh_Log(L"[dl] read failed (%lu)", GetLastError());
            return false;
        }
        if (got == 0) break;
        total += got;
        if (total > kMaxBytes) {
            Wh_Log(L"[dl] file beyond the allowed limit: aborted");
            return false;
        }
        DWORD written = 0;
        if (!WriteFile(file.get(), buffer, got, &written, nullptr) || written != got) {
            Wh_Log(L"[dl] write failed (%lu)", GetLastError());
            return false;
        }
    }
    Wh_Log(L"[dl] %lu bytes downloaded", total);
    return total > 0;
}

static bool FileMatchesPin(const wchar_t* path, const wchar_t* pin) {
    std::wstring hex;
    if (!Sha256File(path, hex)) return false;
    bool ok = (_wcsicmp(hex.c_str(), pin) == 0);
    if (!ok) Wh_Log(L"[hash] file does not match: %s", path);
    return ok;
}

// Downloads (if needed) and verifies a file. On failure the temporary
// file is removed and the function returns false (fail-closed).
static bool EnsureVerifiedFile(const wchar_t* dir, const wchar_t* fileName,
                               const wchar_t* symbolId, const wchar_t* sha256Pin,
                               wchar_t* outPath, size_t outPathCount) {
    wchar_t target[MAX_PATH] = {};
    _snwprintf_s(target, _countof(target), _TRUNCATE, L"%s\\%s", dir, fileName);

    if (GetFileAttributesW(target) != INVALID_FILE_ATTRIBUTES && FileMatchesPin(target, sha256Pin)) {
        wcscpy_s(outPath, outPathCount, target);
        return true;
    }

    wchar_t url[512] = {};
    _snwprintf_s(url, _countof(url), _TRUNCATE, kSymbolUrlFmt, fileName, symbolId, fileName);

    wchar_t temp[MAX_PATH] = {};
    _snwprintf_s(temp, _countof(temp), _TRUNCATE, L"%s.part", target);

    if (g_unloading.load(std::memory_order_acquire)) return false;
    Wh_Log(L"[dl] downloading %s ...", url);
    if (!HttpDownloadToFile(url, temp, (DWORD)g_cfg.downloadTimeoutSec)) {
        DeleteFileW(temp);
        return false;
    }
    if (g_unloading.load(std::memory_order_acquire)) {
        DeleteFileW(temp);
        return false;
    }

    if (!FileMatchesPin(temp, sha256Pin)) {
        DeleteFileW(temp);
        return false;
    }

    if (g_cfg.requireSignature) {
        bool ms = false;
        SigResult sig = VerifyMicrosoftSignature(temp, &ms);
        if (sig == SigResult::Bad || (sig == SigResult::Ok && !ms)) {
            // Signature present but not valid, or signer other than Microsoft:
            // the file is discarded.
            Wh_Log(L"[sig] file rejected: %s", fileName);
            DeleteFileW(temp);
            return false;
        }
        if (sig == SigResult::None)
            Wh_Log(L"[sig] accepted on the pinned hash (catalog-only signature): %s", fileName);
    }

    if (!MoveFileExW(temp, target, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        Wh_Log(L"[dl] MoveFileEx failed (%lu)", GetLastError());
        DeleteFileW(temp);
        return false;
    }

    wcscpy_s(outPath, outPathCount, target);
    return true;
}

static bool EnsureDirectory(const wchar_t* dir) {
    DWORD attr = GetFileAttributesW(dir);
    if (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY)) return true;
    if (CreateDirectoryW(dir, nullptr)) return true;
    Wh_Log(L"[fs] CreateDirectory failed (%lu): %s", GetLastError(), dir);
    return false;
}



#define IDM_MOD_SHOWDESKTOP 0x7C74
// The show desktop button panel: "Show desktop" and "Peek at desktop". Explorer's own
// ids for the two are 0x1A2D and 0x1A2E; the mod uses its own so that the two never mix.

#define IDM_MOD_PEEK          0x7C75

// Forward declaration needed by ShowBatteryMenu (defined further below).
static bool HandleClassicMenuCommand(UINT id);

// === MULTI-LANGUAGE SUPPORT for tray context menus (battery, network,
// clock, show-desktop) and for the strings served to the tray modules.
// Language detected from GetUserDefaultUILanguage, fallback English.
// 15 languages: IT, EN, FR, ES, DE, PT, NL, RU, JA, PL, DA, SV, NO, FI, TR.

enum UiLangId {
    LANG_IT = 0, LANG_EN, LANG_FR, LANG_ES, LANG_DE,
    LANG_PT, LANG_NL, LANG_RU, LANG_JA, LANG_PL,
    LANG_DA, LANG_SV, LANG_NO, LANG_FI, LANG_TR, LANG_COUNT
};

static UiLangId DetectUiLang() {
    LANGID lid = (LANGID)(GetUserDefaultUILanguage() & 0xFFFF);
    WORD primary = PRIMARYLANGID(lid);
    switch (primary) {
        case LANG_ITALIAN:    return LANG_IT;
        case LANG_ENGLISH:    return LANG_EN;
        case LANG_FRENCH:     return LANG_FR;
        case LANG_SPANISH:    return LANG_ES;
        case LANG_GERMAN:     return LANG_DE;
        case LANG_PORTUGUESE: return LANG_PT;
        case LANG_DUTCH:      return LANG_NL;
        case LANG_RUSSIAN:    return LANG_RU;
        case LANG_JAPANESE:   return LANG_JA;
        case LANG_POLISH:     return LANG_PL;
        case LANG_DANISH:     return LANG_DA;
        case LANG_SWEDISH:    return LANG_SV;
        case LANG_NORWEGIAN:  return LANG_NO;
        case LANG_FINNISH:    return LANG_FI;
        case LANG_TURKISH:    return LANG_TR;
        default:              return LANG_EN;
    }
}

// Battery right-click menu entries. Command ids are stobject's own
// (101 = Power Options, 102 = Windows Mobility Center); when the user
// selects an entry we forward WM_COMMAND to stobject's tray window so
// it handles them natively, just like Windows 10.
static const wchar_t* const kBatteryPowerOptions[LANG_COUNT] = {
    L"&Opzioni risparmio energia",
    L"&Power Options",
    L"&Options d'alimentation",
    L"&Opciones de energía",
    L"&Energieoptionen",
    L"Opções de &Energia",
    L"&Energiebeheer",
    L"&Электропитание",
    L"電源オプション(&O)",
    L"Opcje &zasilania",
    L"&Strømstyring", L"&Energialternativ", L"&Strømalternativer", L"&Virranhallinta", L"&Güç Seçenekleri",
};
static const wchar_t* const kBatteryMobilityCenter[LANG_COUNT] = {
    L"&Centro PC portatile Windows",
    L"Windows Mobility &Center",
    L"Centre de mobilité W&indows",
    L"Centro de movilidad de W&indows",
    L"Windows-&Mobility Center",
    L"Centro de Mobilidade do Windows (&M)",
    L"Windows Mobi&lity Center",
    L"Центр мобильности &Windows",
    L"Windows モビリティ センター(&M)",
    L"Centrum mobilności w systemie &Windows",
    L"Windows &Mobilitetscenter", L"Windows &Mobility Center", L"Windows &Mobilitetssenter", L"Windowsin &liikkuvuuskeskus", L"Windows &Mobility Center",
};

// Network right-click (pnidui resource 3014): troubleshoot (3107), settings (3109)
static const wchar_t* const kNetTroubleshoot[LANG_COUNT] = {
    L"Risoluzione &problemi",
    L"&Troubleshoot problems",
    L"&Résoudre les problèmes",
    L"&Solucionar problemas",
    L"Problem&behandlung",
    L"&Resolver Problemas",
    L"Problemen &oplossen",
    L"&Диагностика неполадок",
    L"トラブルシューティング(&T)",
    L"Rozwiąż &problemy",
    L"&Løs problemer", L"&Felsök problem", L"&Løs problemer", L"&Vianmääritys", L"&Sorun giderme",
};
static const wchar_t* const kNetOpenSettings[LANG_COUNT] = {
    L"Apri impostazioni &Rete e Internet",
    L"Open Network &Internet settings",
    L"Ouvrir les paramètres &réseau et Internet",
    L"Abrir configuración de &Red e Internet",
    L"Netzwerk- und &Interneteinstellungen öffnen",
    L"Abrir Definições de &Rede e Internet",
    L"Netwerk- en &internetinstellingen openen",
    L"Открыть параметры &сети и Интернета",
    L"ネットワークとインターネットの設定を開く(&I)",
    L"Otwórz ustawienia &sieci i Internetu",
    L"Åbn indstillinger for &netværk og internet", L"Öppna inställningar för &nätverk och internet", L"Åpne innstillinger for &nettverk og Internett", L"Avaa verkon ja Internetin &asetukset", L"&Ağ ve Internet ayarlarını aç",
};

// Show-desktop button
static const wchar_t* const kShowDesktopText[LANG_COUNT] = {
    L"Mostra &desktop",
    L"&Show the desktop",
    L"&Afficher le bureau",
    L"&Mostrar el escritorio",
    L"&Desktop anzeigen",
    L"Mostrar o &Ambiente de Trabalho",
    L"&Bureaublad weergeven",
    L"&Показать рабочий стол",
    L"デスクトップを表示(&S)",
    L"Pokaż &pulpit",
    L"Vis &skrivebordet", L"&Visa skrivbordet", L"Vis &skrivebordet", L"&Näytä työpöytä", L"&Masaüstünü göster",
};
static const wchar_t* const kPeekText[LANG_COUNT] = {
    L"Visualizza desktop con &Aero Peek",
    L"Peek at &desktop (Aero Peek)",
    L"Afficher un &aperçu du bureau (Aero Peek)",
    L"Echar un vistazo al &escritorio (Aero Peek)",
    L"Desktop mit Aero &Peek anzeigen",
    L"Observar o A&mbiente de Trabalho (Aero Peek)",
    L"Bu&reaublad bekijken (Aero Peek)",
    L"Просмотреть рабочий стол с помощ&ью Aero Peek",
    L"デスクトップをプレビュー(&D) (Aero Peek)",
    L"Rzuć okiem na pulpit (&Aero Peek)",
    L"Kig på &skrivebordet (Aero Peek)", L"Granska &skrivbordet (Aero Peek)", L"Titt på &skrivebordet (Aero Peek)", L"Työpöydän &esikatselu (Aero Peek)", L"&Masaüstüne bak (Aero Peek)",
};

// ===========================================================================
// ImmersiveMenu - the Windows 10 look of this mod's own menus.
//
// The "Non Immersive Taskbar Context Menu" mod makes the taskbar menus classic by taking
// MFT_OWNERDRAW off the entries and clearing the background (hbrBack). This does the opposite:
// Every entry becomes MFT_OWNERDRAW, the menu gets a background taken from the
// "ImmersiveStart::Menu" theme (light/dark), and entries, selection and separators are drawn
// with the colours and the font of that theme. The owner window is subclassed only while the
// menu is open, to answer WM_MEASUREITEM and WM_DRAWITEM for the entries created here alone.
// With no usable theme the menu stays an ordinary popup.
//
// Proportions, spacing and colours are taken from Windows 10 reference
// screenshots of the power-user (Win+X) menu: 32 px rows, text 35 px inside the
// left edge, one chevron 11 px from the right edge, separator 2 px inside the item
// rectangle, F9F9F9/black in the light scheme and 2B2B2B/white with a 414141
// highlight in the dark one. The OS submenu arrow is clipped away so the chevron
// is not drawn twice.
// ===========================================================================
namespace ImmersiveMenu {

using ThemeHandle = void*;
static constexpr int kPartBackground = 9;      // MENU_POPUPBACKGROUND
static constexpr int kPartItem = 14;           // MENU_POPUPITEM
static constexpr int kStateNormal = 1;
static constexpr int kStateHot = 2;
static constexpr int kStateDisabled = 3;
static constexpr int kPropBorderColor = 3801;  // TMT_BORDERCOLOR
static constexpr int kPropFillColor = 3802;    // TMT_FILLCOLOR
static constexpr int kPropTextColor = 3803;    // TMT_TEXTCOLOR
static constexpr int kPropFont = 210;          // TMT_FONT
static constexpr UINT_PTR kSubclassId = 0x494D4D31;   // "IMM1"

struct Api {
    ThemeHandle (WINAPI* openTheme)(HWND, LPCWSTR) = nullptr;
    ThemeHandle (WINAPI* openThemeForDpi)(HWND, LPCWSTR, UINT) = nullptr;
    HRESULT (WINAPI* closeTheme)(ThemeHandle) = nullptr;
    HRESULT (WINAPI* drawBackground)(ThemeHandle, HDC, int, int, const RECT*, const RECT*) = nullptr;
    HRESULT (WINAPI* getFont)(ThemeHandle, HDC, int, int, int, LOGFONTW*) = nullptr;
    HRESULT (WINAPI* getColor)(ThemeHandle, int, int, int, COLORREF*) = nullptr;
    bool ready = false;
};
static Api g_api;

static bool LoadApi() noexcept {
    try {
        if (g_api.ready) return true;
        HMODULE module = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!module) return false;
        g_api.openTheme = reinterpret_cast<ThemeHandle (WINAPI*)(HWND, LPCWSTR)>(
            GetProcAddress(module, "OpenThemeData"));
        g_api.openThemeForDpi = reinterpret_cast<ThemeHandle (WINAPI*)(HWND, LPCWSTR, UINT)>(
            GetProcAddress(module, "OpenThemeDataForDpi"));
        g_api.closeTheme = reinterpret_cast<HRESULT (WINAPI*)(ThemeHandle)>(
            GetProcAddress(module, "CloseThemeData"));
        g_api.drawBackground =
            reinterpret_cast<HRESULT (WINAPI*)(ThemeHandle, HDC, int, int, const RECT*, const RECT*)>(
                GetProcAddress(module, "DrawThemeBackground"));
        g_api.getFont = reinterpret_cast<HRESULT (WINAPI*)(ThemeHandle, HDC, int, int, int, LOGFONTW*)>(
            GetProcAddress(module, "GetThemeFont"));
        g_api.getColor = reinterpret_cast<HRESULT (WINAPI*)(ThemeHandle, int, int, int, COLORREF*)>(
            GetProcAddress(module, "GetThemeColor"));
        g_api.ready = g_api.openTheme && g_api.closeTheme && g_api.getColor;
        return g_api.ready;
    } catch (...) {
        return false;
    }
}

struct ItemData {
    wchar_t text[128];
    bool separator;
    bool submenu;
};

struct Session {
    HWND owner = nullptr;
    ThemeHandle theme = nullptr;
    int dpi = 96;
    LOGFONTW font = {};
    HFONT hfont = nullptr;
    HBRUSH brush = nullptr;
    // Colours of the Windows 10 menu: the light scheme is the one the menu has
    // always used (F9F9F9 / black text), the dark one is the Windows 10 dark menu
    // (2B2B2B / white text, 414141 highlight) as in the reference screenshot. The
    // theme of the shell refines them when it is available, so the menu follows the
    // Windows app theme or, when it is not "Auto", the skin's Colour scheme.
    bool light = true;
    COLORREF fill = RGB(249, 249, 249);
    COLORREF textNormal = RGB(0, 0, 0);
    COLORREF textDisabled = RGB(160, 160, 160);
    COLORREF border = RGB(205, 205, 205);
    COLORREF separator = RGB(205, 205, 205);
    COLORREF hotFill = RGB(229, 229, 229);
    int itemHeight = 0;
    // Proportions measured on the Windows 10 reference screenshot (uploads/image-2.png,
    // 100% DPI) and used as they are, so the recreation has the proportions of the
    // original: 32 px rows, text 32 px inside the item rectangle, the submenu chevron
    // 6 px from its right edge, the separator line inset by 8 px, and a menu about
    // 258 px wide - the width of the reference menu, from which the shell subtracts
    // its own right-hand gutter when the items are measured.
    int padLeft = 0;
    int padRight = 0;
    int chevronInset = 0;
    int separatorInset = 0;
    int row = 0;                 // position inside the menu, for the log only
    std::vector<ItemData*> items;
};
static Session* g_session = nullptr;

// Which scheme the menus of this mod use: the right-click menu of the network icon and
// the one of the battery icon. "auto" follows the Windows app theme (Settings >
// Personalization > Colors, the AppsUseLightTheme value Explorer itself uses), "light"
// and "dark" force the Windows 10 light or dark menu. Both colour sets are always
// available: if the shell theme cannot be opened, the menu is painted with the
// Windows 10 constants of the selected scheme instead of an unstyled popup.
static bool IsLightTheme() noexcept {
    try {
        WindhawkUtils::StringSetting scheme(Wh_GetStringSetting(L"themeScheme"));
        if (scheme.get()) {
            if (_wcsicmp(scheme.get(), L"light") == 0) return true;
            if (_wcsicmp(scheme.get(), L"dark") == 0) return false;
        }
    } catch (...) {
    }
    DWORD value = 1, size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size) == ERROR_SUCCESS)
        return value != 0;
    return true;
}

// The same sequence as pnidui: the light/dark variant, then the neutral one, then "Menu".
static ThemeHandle OpenMenuTheme(HWND owner, int dpi) noexcept {
    const wchar_t* candidates[3] = {
        IsLightTheme() ? L"LightMode_ImmersiveStart::Menu" : L"DarkMode_ImmersiveStart::Menu",
        L"ImmersiveStart::Menu",
        L"Menu" };
    for (const wchar_t* name : candidates) {
        ThemeHandle theme = nullptr;
        if (g_api.openThemeForDpi) theme = g_api.openThemeForDpi(owner, name, (UINT)dpi);
        if (!theme && g_api.openTheme) theme = g_api.openTheme(owner, name);
        if (theme) {
            static int logged = 0;
            if (logged++ < 3) Wh_Log(L"[menu] immersive menu: theme class '%s'", name);
            return theme;
        }
    }
    return nullptr;
}

static void End(Session& s) noexcept {
    for (ItemData* d : s.items) delete d;
    s.items.clear();
    if (s.brush) { DeleteObject(s.brush); s.brush = nullptr; }
    if (s.hfont) { DeleteObject(s.hfont); s.hfont = nullptr; }
    if (s.theme && g_api.closeTheme) { g_api.closeTheme(s.theme); s.theme = nullptr; }
}

static void Prepare(Session& s, HMENU menu) noexcept {
    const int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; ++i) {
        wchar_t buffer[128] = {};
        MENUITEMINFOW info = {};
        info.cbSize = sizeof(info);
        info.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_SUBMENU;
        info.dwTypeData = buffer;
        info.cch = _countof(buffer) - 1;
        if (!GetMenuItemInfoW(menu, i, TRUE, &info)) continue;
        ItemData* data = new (std::nothrow) ItemData{};
        if (!data) continue;
        data->separator = (info.fType & MFT_SEPARATOR) != 0;
        data->submenu = info.hSubMenu != nullptr;
        wcsncpy_s(data->text, buffer, _TRUNCATE);
        s.items.push_back(data);

        MENUITEMINFOW set = {};
        set.cbSize = sizeof(set);
        set.fMask = MIIM_FTYPE | MIIM_DATA;
        set.fType = info.fType | MFT_OWNERDRAW;   // the opposite of ApplyClassicMenu
        set.dwItemData = reinterpret_cast<ULONG_PTR>(data);
        SetMenuItemInfoW(menu, i, TRUE, &set);
        if (info.hSubMenu) Prepare(s, info.hSubMenu);
    }
    MENUINFO mi = {};
    mi.cbSize = sizeof(mi);
    mi.fMask = MIM_BACKGROUND;
    mi.hbrBack = s.brush;                     // the background of the theme, never null
    SetMenuInfo(menu, &mi);
}

static bool Begin(Session& s, HMENU menu, HWND owner) noexcept {
    try {
        if (!owner || !LoadApi()) return false;
        s.owner = owner;
        const UINT dpi = GetDpiForWindow(owner);
        s.dpi = dpi >= 96 && dpi <= 480 ? (int)dpi : 96;
        // Windows 10 constants for the selected scheme: they stay in place if the
        // theme cannot be opened or does not carry a colour, so both themes are
        // supported even on builds where the immersive menu theme is missing.
        s.light = IsLightTheme();
        s.fill = s.light ? RGB(249, 249, 249) : RGB(43, 43, 43);
        s.textNormal = s.light ? RGB(0, 0, 0) : RGB(255, 255, 255);
        s.textDisabled = s.light ? RGB(120, 120, 120) : RGB(160, 160, 160);
        s.border = s.light ? RGB(205, 205, 205) : RGB(128, 128, 128);
        s.separator = s.light ? RGB(205, 205, 205) : RGB(128, 128, 128);
        s.hotFill = s.light ? RGB(229, 229, 229) : RGB(65, 65, 65);
        s.theme = OpenMenuTheme(owner, s.dpi);
        if (!s.theme) {
            // No immersive theme on this build: paint the menu with the Windows 10
            // colours anyway instead of falling back to a plain popup.
            Wh_Log(L"[menu] immersive theme unavailable: the menu keeps the Windows 10 %s colours",
                   s.light ? L"light" : L"dark");
        }

        HDC dc = GetDC(owner);
        if (!s.theme || !g_api.getFont ||
            FAILED(g_api.getFont(s.theme, dc, kPartItem, 0, kPropFont, &s.font)) ||
            s.font.lfHeight == 0) {
            NONCLIENTMETRICSW info = {};
            info.cbSize = sizeof(info);
            if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0))
                s.font = info.lfMenuFont;
            else
                s.font.lfHeight = -MulDiv(9, s.dpi, 72);
            wcscpy_s(s.font.lfFaceName, LF_FACESIZE, L"Segoe UI");
        }
        if (s.theme) {
            g_api.getColor(s.theme, kPartBackground, 0, kPropFillColor, &s.fill);
            g_api.getColor(s.theme, kPartItem, kStateNormal, kPropTextColor, &s.textNormal);
            g_api.getColor(s.theme, kPartItem, kStateDisabled, kPropTextColor, &s.textDisabled);
            COLORREF hot = s.hotFill;
            if (g_api.getColor(s.theme, kPartItem, kStateHot, kPropFillColor, &hot) == S_OK)
                s.hotFill = hot;
        }
        // Colour of the separator line for the scheme in use. In the light scheme the
        // border colour the menu theme gives to its own popups is used when it is
        // available (it is the colour the earlier builds showed); in the dark scheme the
        // grey measured on the Windows 10 dark reference screenshot is used, which is
        // clearly visible on the 2B2B2B background.
        if (s.light && s.theme && g_api.getColor) {
            COLORREF themed = s.separator;
            if (g_api.getColor(s.theme, kPartBackground, 0, kPropBorderColor, &themed) == S_OK &&
                themed != s.fill)
                s.separator = themed;
        }
        static int themeLogs = 0;
        if (themeLogs++ < 6)
            Wh_Log(L"[menu] %s menu: background RGB(%u,%u,%u), highlight RGB(%u,%u,%u), "
                   L"text RGB(%u,%u,%u), separator RGB(%u,%u,%u)",
                   s.light ? L"light" : L"dark",
                   GetRValue(s.fill), GetGValue(s.fill), GetBValue(s.fill),
                   GetRValue(s.hotFill), GetGValue(s.hotFill), GetBValue(s.hotFill),
                   GetRValue(s.textNormal), GetGValue(s.textNormal), GetBValue(s.textNormal),
                   GetRValue(s.separator), GetGValue(s.separator), GetBValue(s.separator));
        s.hfont = CreateFontIndirectW(&s.font);
        int textHeight = abs(s.font.lfHeight);
        if (s.hfont && dc) {
            HGDIOBJ old = SelectObject(dc, s.hfont);
            TEXTMETRICW tm = {};
            if (GetTextMetricsW(dc, &tm)) textHeight = tm.tmHeight;
            if (old) SelectObject(dc, old);
        }
        if (dc) ReleaseDC(owner, dc);
        // The proportions the menu is drawn with, in device independent pixels so that the
        // menu looks the same at every display scale: rows 32 px tall, text taken from the
        // item rectangle, chevron 6 px from its right edge, separator line inset by 8 px.
        // A row never becomes shorter than the text plus 8 px, so a large font or an East
        // Asian face cannot clip the glyphs.
        s.itemHeight = MulDiv(32, s.dpi, 96);
        const int minItemHeight = textHeight + MulDiv(8, s.dpi, 96);
        if (s.itemHeight < minItemHeight) s.itemHeight = minItemHeight;
        s.padLeft = MulDiv(32, s.dpi, 96);
        s.padRight = MulDiv(10, s.dpi, 96);
        s.chevronInset = MulDiv(4, s.dpi, 96);
        s.separatorInset = MulDiv(8, s.dpi, 96);
        s.brush = CreateSolidBrush(s.fill);
        if (!s.hfont || !s.brush) return false;
        Prepare(s, menu);
        return true;
    } catch (...) {
        return false;
    }
}

static bool Owns(const Session* s, const ItemData* d) noexcept {
    if (!s || !d) return false;
    for (const ItemData* item : s->items)
        if (item == d) return true;
    return false;
}

static void Measure(Session* s, HWND hwnd, MEASUREITEMSTRUCT* m, const ItemData* d) noexcept {
    if (d->separator) {
        m->itemWidth = 1;
        m->itemHeight = MulDiv(9, s->dpi, 96);
        return;
    }
    int width = 0;
    HDC dc = GetDC(hwnd);
    if (dc) {
        HGDIOBJ old = SelectObject(dc, s->hfont);
        RECT rc = {};
        DrawTextW(dc, d->text, -1, &rc, DT_CALCRECT | DT_SINGLELINE | DT_LEFT);
        width = rc.right - rc.left;
        if (old) SelectObject(dc, old);
        ReleaseDC(hwnd, dc);
    }
    width += s->padLeft + s->padRight;
    if (d->submenu) width += s->chevronInset + MulDiv(14, s->dpi, 96);
    // 242 px plus the ~16 px gutter the shell keeps on the right of the item
    // rectangle gives the 258 px wide menu of the Windows 10 reference screenshot.
    const int minWidth = MulDiv(242, s->dpi, 96);
    m->itemWidth = static_cast<UINT>(width < minWidth ? minWidth : width);
    m->itemHeight = static_cast<UINT>(s->itemHeight);
}

static void Draw(Session* s, DRAWITEMSTRUCT* di, const ItemData* d) noexcept {
    HDC dc = di->hDC;
    RECT rc = di->rcItem;
    // Start every item from a clean clip region: the submenu rows take their own
    // rectangle out of the clip below (it is the documented way of stopping the
    // shell from painting a second arrow on top of ours), and a clip left over from
    // an earlier paint of the same row would otherwise erase parts of this one.
    SelectClipRgn(dc, nullptr);
    FillRect(dc, &rc, s->brush);
    if (d->separator) {
        // One pixel line of the separator colour of the scheme, always painted here.
        // The menu theme cannot be trusted for this part: on the build that reported
        // missing separators the themed MENU_POPUPSEPARATOR answered success without
        // painting anything, so the section is drawn directly and never depends on the
        // theme. Position and inset are the ones measured on the Windows 10 reference
        // menu (a 1 px line, 8 px inside the item rectangle, in the middle of the row).
        const int y = (rc.top + rc.bottom) / 2;
        RECT r = { rc.left + s->separatorInset, y, rc.right - s->separatorInset, y + 1 };
        HBRUSH line = CreateSolidBrush(s->separator);
        if (line) {
            FillRect(dc, &r, line);
            DeleteObject(line);
        } else {
            // Only possible when the process is out of GDI objects: the row keeps the
            // menu background (a null brush would leave a black line behind).
            Wh_Log(L"[menu] separator brush not available: the line is skipped");
        }
        return;
    }
    const bool disabled = (di->itemState & (ODS_GRAYED | ODS_DISABLED)) != 0;
    const bool selected = (di->itemState & ODS_SELECTED) != 0 && !disabled;
    COLORREF textColor = disabled ? s->textDisabled : s->textNormal;
    if (selected) {
        if (!s->theme || !g_api.drawBackground ||
            FAILED(g_api.drawBackground(s->theme, dc, kPartItem, kStateHot, &rc, nullptr))) {
            // Fallback of the selected row: the Windows 10 highlight colour of the
            // scheme in use (light 229,229,229 / dark 65,65,65), never an inverted
            // background, which used to turn the row black on a light menu.
            HBRUSH hot = CreateSolidBrush(s->hotFill);
            if (hot) { FillRect(dc, &rc, hot); DeleteObject(hot); }
        }
        COLORREF hotText = textColor;
        if (s->theme &&
            g_api.getColor(s->theme, kPartItem, kStateHot, kPropTextColor, &hotText) == S_OK)
            textColor = hotText;
    }
    HGDIOBJ old = SelectObject(dc, s->hfont);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, textColor);
    RECT tr = rc;
    tr.left += s->padLeft;
    tr.right -= s->padRight;
    DrawTextW(dc, d->text, -1, &tr, DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_HIDEPREFIX);
    if (d->submenu) {
        // One chevron only: the shell glyph Windows 10 shows for a submenu, at the
        // same distance from the right edge (see the reference screenshot).
        RECT ar = rc;
        ar.right -= s->chevronInset;
        HFONT arrowFont = CreateFontW(s->font.lfHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                      DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0,
                                      L"Segoe MDL2 Assets");
        if (arrowFont) {
            SelectObject(dc, arrowFont);
            DrawTextW(dc, L"\xE76C", -1, &ar, DT_SINGLELINE | DT_VCENTER | DT_RIGHT);
            SelectObject(dc, s->hfont);
            DeleteObject(arrowFont);
        }
    }
    if (old) SelectObject(dc, old);
    if (d->submenu) {
        // "one arrow, not two". The shell paints its own submenu arrow after
        // WM_DRAWITEM has returned, on top of the chevron drawn just above: the
        // documented way to stop it is to take the item rectangle out of the clip
        // region of this DC before returning, so only our chevron survives.
        ExcludeClipRect(dc, rc.left, rc.top, rc.right, rc.bottom);
    }
}

static LRESULT CALLBACK OwnerProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                  UINT_PTR, DWORD_PTR) {
    try {
        Session* s = g_session;
        if (s && msg == WM_MEASUREITEM && lParam) {
            auto* m = reinterpret_cast<MEASUREITEMSTRUCT*>(lParam);
            const auto* d = reinterpret_cast<const ItemData*>(m->itemData);
            if (m->CtlType == ODT_MENU && Owns(s, d)) {
                Measure(s, hwnd, m, d);
                return TRUE;
            }
        } else if (s && msg == WM_DRAWITEM && lParam) {
            auto* di = reinterpret_cast<DRAWITEMSTRUCT*>(lParam);
            const auto* d = reinterpret_cast<const ItemData*>(di->itemData);
            if (di->CtlType == ODT_MENU && Owns(s, d)) {
                Draw(s, di, d);
                return TRUE;
            }
        }
    } catch (...) {
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

// TrackPopupMenuEx with TPM_RETURNCMD, but with immersive entries. Must be called by the
// thread that owns "owner". With no usable theme it shows the ordinary popup.
static UINT Track(HMENU menu, HWND owner, int x, int y, UINT flags) noexcept {
    Session session;
    try {
        flags = (flags & ~TPM_NONOTIFY) | TPM_RETURNCMD;
        if (g_session || !Begin(session, menu, owner)) {
            End(session);
            return static_cast<UINT>(TrackPopupMenuEx(menu, flags, x, y, owner, nullptr));
        }
        g_session = &session;
        SetWindowSubclass(owner, OwnerProc, kSubclassId, 0);
        const UINT result = static_cast<UINT>(TrackPopupMenuEx(menu, flags, x, y, owner, nullptr));
        RemoveWindowSubclass(owner, OwnerProc, kSubclassId);
        g_session = nullptr;
        End(session);
        return result;
    } catch (...) {
        // The session must not survive an exception: the owner window would keep the
        // subclass and the next menu would be drawn with the flags of this one.
        if (g_session == &session) {
            RemoveWindowSubclass(owner, OwnerProc, kSubclassId);
            g_session = nullptr;
        }
        End(session);
        Wh_Log(L"[menu] exception while showing the menu: the standard popup is used");
        return static_cast<UINT>(TrackPopupMenuEx(menu, flags, x, y, owner, nullptr));
    }
}

}  // namespace ImmersiveMenu

#define IDM_MOD_BATTERY_POWER       0x7C78
#define IDM_MOD_BATTERY_MOBILITY    0x7C79

static int g_batteryMenuLogs = 0;
static void ShowBatteryMenu(HWND serviceWindow) {
    const UiLangId lang = DetectUiLang();
    HMENU menu = CreatePopupMenu();
    if (!menu) return;
    AppendMenuW(menu, MF_STRING, IDM_MOD_BATTERY_POWER,    kBatteryPowerOptions[lang]);
    AppendMenuW(menu, MF_STRING, IDM_MOD_BATTERY_MOBILITY, kBatteryMobilityCenter[lang]);
    if (g_batteryMenuLogs < 5) {
        g_batteryMenuLogs++;
        Wh_Log(L"[battery] context menu shown (language %d)", (int)lang);
    }

    POINT pt = {};
    GetCursorPos(&pt);

    // Owner for the THEME: Shell_TrayWnd is what makes Windows apply the Explorer
    // theme (the same look as the audio / volume menu). With no taskbar window the
    // service window of stobject is used instead.
    HWND themeOwner = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!themeOwner) themeOwner = serviceWindow;

    // Documented by Microsoft: for a tray icon menu the foreground window has to be the
    // owner of the menu, otherwise the menu does not get the right theme (and may not
    // close when the user clicks outside it).
    SetForegroundWindow(themeOwner);

    const BOOL picked = static_cast<BOOL>(
        ImmersiveMenu::Track(menu, themeOwner, pt.x, pt.y, TPM_RIGHTBUTTON));
    DestroyMenu(menu);
    if (!picked) return;

    const UINT cmd = (UINT)(UINT_PTR)picked;
    UINT realId = 0;
    switch (cmd) {
        case IDM_MOD_BATTERY_POWER:    realId = 101; break;
        case IDM_MOD_BATTERY_MOBILITY: realId = 102; break;
        default: HandleClassicMenuCommand(cmd); return;
    }
    // Commands 101/102 belong to stobject: they are sent back to it on ITS own
    // service window, not on Shell_TrayWnd.
    if (realId && serviceWindow && IsWindow(serviceWindow)) {
        SendMessageW(serviceWindow, WM_COMMAND, MAKEWPARAM(realId, 0), 0);
        Wh_Log(L"[battery] sent WM_COMMAND %u to stobject tray window", realId);
    }
}

static int g_classicCmdLogs = 0;

static bool ToggleDesktopLikeWin10() {
    // CLSID_Shell and IID_IDispatch written by hand so as not to depend on the
    // uuid libraries in the build flags.
    static const GUID kClsidShell = { 0x13709620, 0xC279, 0x11CE,
                                      { 0xA4, 0x9E, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 } };
    static const GUID kIidDispatch = { 0x00020400, 0x0000, 0x0000,
                                       { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };
    static const GUID kIidNull = { 0x00000000, 0x0000, 0x0000,
                                   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };
    IDispatch* shell = nullptr;
    const HRESULT hr = CoCreateInstance(kClsidShell, nullptr,
                                        CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
                                        kIidDispatch, (void**)&shell);
    if (SUCCEEDED(hr) && shell) {
        OLECHAR* name = const_cast<OLECHAR*>(L"ToggleDesktop");
        DISPID dispid = 0;
        if (SUCCEEDED(shell->GetIDsOfNames(kIidNull, &name, 1, LOCALE_USER_DEFAULT, &dispid))) {
            DISPPARAMS params = {};
            VARIANT result = {};
            const HRESULT hrInvoke = shell->Invoke(dispid, kIidNull, LOCALE_USER_DEFAULT,
                                                   DISPATCH_METHOD, &params, &result, nullptr, nullptr);
            shell->Release();
            return SUCCEEDED(hrInvoke);
        }
        shell->Release();
    }
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (tray) {
        DWORD_PTR ignored = 0;
        SendMessageTimeoutW(tray, WM_COMMAND, MAKEWPARAM(IDM_MOD_SHOWDESKTOP, 0), 0,
                            SMTO_ABORTIFHUNG, 1000, &ignored);
        return true;
    }
    return false;
}

// --- the show desktop button and the clock: the two menus of Windows 10 ----
// Everything here is cosmetic on the surface and careful underneath. The Peek
// checkbox is virtualized for this shell process; its two registry values are never
// written. What the Windows 10 menus contain, and the ids Explorer uses, come from
// the shipped binary (see the note at the top of the file).

// (the four texts and the insertion helper are defined above, together with
// the ids, because the menu is built before this code runs)

// HKCU keys the shell itself uses for Peek (Explorer\Advanced\DisablePreviewDesktop
// and DWM\EnableAeroPeek). Explorer keeps the answer in its settings cache, so a
// change is announced with the group name its own code compares against
// ("SettingsCacheChangeMessage"), which is how the toggle applies without a
// shell restart on the builds that listen for it.
static const wchar_t* const kPeekKeyEsc = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";
static const wchar_t* const kPeekKeyDwm = L"Software\\Microsoft\\Windows\\DWM";
static const wchar_t* const kPeekCacheGroup = L"SettingsCacheChangeMessage";
static const wchar_t kPeekEscNtSuffix[] = L"\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";
static const wchar_t kPeekDwmNtSuffix[] = L"\\Microsoft\\Windows\\DWM";

class ScopedHKey {
public:
    explicit ScopedHKey(HKEY k = nullptr) : m_k(k) {}
    ~ScopedHKey() { if (m_k) RegCloseKey(m_k); }
    ScopedHKey(const ScopedHKey&) = delete;
    ScopedHKey& operator=(const ScopedHKey&) = delete;
    HKEY* receive() { return &m_k; }
    HKEY get() const { return m_k; }
    bool valid() const { return m_k != nullptr; }
private:
    HKEY m_k;
};

static bool ReadRegDwordHkcu(const wchar_t* sub, const wchar_t* value, DWORD* out) {
    if (!sub || !value || !out) return false;
    ScopedHKey key;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, sub, 0, KEY_QUERY_VALUE, key.receive()) != ERROR_SUCCESS)
        return false;
    if (!key.valid()) return false;
    DWORD type = 0, size = sizeof(DWORD), v = 0;
    const LONG r = RegQueryValueExW(key.get(), value, nullptr, &type, (BYTE*)&v, &size);
    if (r != ERROR_SUCCESS || type != REG_DWORD || size != sizeof(DWORD)) return false;
    *out = v;
    return true;
}

// RegQueryValueExW receives an open HKEY, not the key's path. NtQueryKey is used
// only to identify the two exact HKCU keys; if it is unavailable, the menu is
// disabled rather than falling back to persistent registry writes. The query hook
// itself follows Microsoft's documented buffer-size contract (including the
// ERROR_MORE_DATA probe).
using RegQueryValueExW_t = LSTATUS (WINAPI*)(HKEY, LPCWSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
using NtQueryKey_t = NTSTATUS (NTAPI*)(HANDLE, ULONG, PVOID, ULONG, PULONG);

static constexpr ULONG kPeekKeyNameInformation = 3;  // KEY_INFORMATION_CLASS::KeyNameInformation
static constexpr size_t kPeekNtKeyNameChars = 1024;
struct PeekKeyNameInformation {
    ULONG NameLength;
    WCHAR Name[kPeekNtKeyNameChars];
};

static RegQueryValueExW_t g_regQueryValueExWOriginal = nullptr;
static NtQueryKey_t g_ntQueryKey = nullptr;
static wchar_t g_currentUserSoftwareNtPath[512] = {};
static size_t g_currentUserSoftwareNtPathChars = 0;
static std::atomic<bool> g_peekRegistryHookRegistered{false};
static std::atomic<bool> g_peekRegistryHookReady{false};
static std::atomic<bool> g_peekDisableOverrideActive{false};
static std::atomic<DWORD> g_peekVirtualDisablePreviewDesktop{0};
static std::atomic<bool> g_peekAeroOverrideActive{false};
static std::atomic<DWORD> g_peekVirtualEnableAeroPeek{1};

static bool CaptureCurrentUserSoftwareNtPath() noexcept {
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll) return false;
    g_ntQueryKey = reinterpret_cast<NtQueryKey_t>(GetProcAddress(ntdll, "NtQueryKey"));
    if (!g_ntQueryKey) return false;

    // Use a documented Win32 open for the current user's Software key, then cache
    // its canonical object-manager name once. The hook can compare against this
    // immutable prefix without allocating or opening registry keys on every query.
    ScopedHKey software;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software", 0, KEY_QUERY_VALUE,
                      software.receive()) != ERROR_SUCCESS || !software.valid())
        return false;

    PeekKeyNameInformation info = {};
    ULONG returned = 0;
    const NTSTATUS status = g_ntQueryKey(software.get(), kPeekKeyNameInformation, &info,
                                         static_cast<ULONG>(sizeof(info)), &returned);
    if (status < 0 || info.NameLength > sizeof(info.Name) ||
        info.NameLength % sizeof(WCHAR) != 0)
        return false;

    size_t chars = info.NameLength / sizeof(WCHAR);
    if (chars && info.Name[chars - 1] == L'\0') --chars;
    if (!chars || chars >= _countof(g_currentUserSoftwareNtPath)) return false;

    memcpy(g_currentUserSoftwareNtPath, info.Name, chars * sizeof(WCHAR));
    g_currentUserSoftwareNtPath[chars] = L'\0';
    g_currentUserSoftwareNtPathChars = chars;
    return true;
}

static bool IsCurrentUserPeekKey(HKEY key, const wchar_t* ntSuffix) noexcept {
    if (!g_ntQueryKey || !ntSuffix || !g_currentUserSoftwareNtPathChars) return false;

    PeekKeyNameInformation info = {};
    ULONG returned = 0;
    const NTSTATUS status = g_ntQueryKey(key, kPeekKeyNameInformation, &info,
                                         static_cast<ULONG>(sizeof(info)), &returned);
    if (status < 0 || info.NameLength > sizeof(info.Name) ||
        info.NameLength % sizeof(WCHAR) != 0)
        return false;

    size_t chars = info.NameLength / sizeof(WCHAR);
    if (chars && info.Name[chars - 1] == L'\0') --chars;
    const size_t suffixChars = wcslen(ntSuffix);
    if (chars != g_currentUserSoftwareNtPathChars + suffixChars) return false;
    return _wcsnicmp(info.Name, g_currentUserSoftwareNtPath,
                     g_currentUserSoftwareNtPathChars) == 0 &&
           _wcsnicmp(info.Name + g_currentUserSoftwareNtPathChars,
                     ntSuffix, suffixChars) == 0;
}

static bool TryGetVirtualPeekValue(HKEY key, LPCWSTR valueName, DWORD* value) noexcept {
    if (!valueName || !value || g_unloading.load(std::memory_order_acquire)) return false;

    const bool isDisableValue =
        _wcsicmp(valueName, L"DisablePreviewDesktop") == 0 &&
        g_peekDisableOverrideActive.load(std::memory_order_acquire);
    const bool isAeroValue =
        _wcsicmp(valueName, L"EnableAeroPeek") == 0 &&
        g_peekAeroOverrideActive.load(std::memory_order_acquire);
    if (!isDisableValue && !isAeroValue) return false;

    const wchar_t* suffix = isDisableValue ? kPeekEscNtSuffix : kPeekDwmNtSuffix;
    if (!IsCurrentUserPeekKey(key, suffix) ||
        g_unloading.load(std::memory_order_acquire))
        return false;

    if (isDisableValue) {
        if (!g_peekDisableOverrideActive.load(std::memory_order_acquire)) return false;
        *value = g_peekVirtualDisablePreviewDesktop.load(std::memory_order_acquire);
    } else {
        if (!g_peekAeroOverrideActive.load(std::memory_order_acquire)) return false;
        *value = g_peekVirtualEnableAeroPeek.load(std::memory_order_acquire);
    }
    return true;
}

static LSTATUS WINAPI RegQueryValueExW_Hook(HKEY key, LPCWSTR valueName, LPDWORD reserved,
                                           LPDWORD type, LPBYTE data,
                                           LPDWORD dataSize) noexcept {
    if (!reserved) {
        DWORD virtualValue = 0;
        if (TryGetVirtualPeekValue(key, valueName, &virtualValue)) {
            // Per RegQueryValueExW, lpcbData may be NULL only when lpData is NULL.
            if (data && !dataSize) return ERROR_INVALID_PARAMETER;
            if (type) *type = REG_DWORD;
            if (!data) {
                if (dataSize) *dataSize = sizeof(virtualValue);
                return ERROR_SUCCESS;
            }
            const DWORD capacity = *dataSize;
            if (capacity < sizeof(virtualValue)) {
                *dataSize = sizeof(virtualValue);
                return ERROR_MORE_DATA;
            }
            memcpy(data, &virtualValue, sizeof(virtualValue));
            *dataSize = sizeof(virtualValue);
            return ERROR_SUCCESS;
        }
    }

    return g_regQueryValueExWOriginal
               ? g_regQueryValueExWOriginal(key, valueName, reserved, type, data, dataSize)
               : ERROR_INVALID_FUNCTION;
}

static bool InstallPeekRegistryVirtualization() {
    if (!CaptureCurrentUserSoftwareNtPath()) {
        Wh_Log(L"[menu] Peek virtualization unavailable: the current user's registry key "
               L"could not be identified; its menu entry will be disabled");
        return false;
    }
    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(RegQueryValueExW),
                            reinterpret_cast<void*>(RegQueryValueExW_Hook),
                            reinterpret_cast<void**>(&g_regQueryValueExWOriginal))) {
        Wh_Log(L"[menu] Peek virtualization unavailable: RegQueryValueExW could not be hooked; "
               L"its menu entry will be disabled");
        return false;
    }

    g_peekRegistryHookRegistered.store(true, std::memory_order_release);
    Wh_Log(L"[menu] Peek registry-query hook registered; Windhawk applies it after "
           L"Wh_ModInit returns");
    return true;
}

// Peek is ON when DisablePreviewDesktop is 0, exactly as the shell reads it.
static bool PeekAtDesktopEnabled() {
    DWORD disable = 0;
    if (!ReadRegDwordHkcu(kPeekKeyEsc, L"DisablePreviewDesktop", &disable)) return true;
    return disable == 0;
}

// The shell greys the entry out when Aero Peek is off at system level.
static bool PeekAeroAllowed() {
    DWORD on = 1;
    if (!ReadRegDwordHkcu(kPeekKeyDwm, L"EnableAeroPeek", &on)) return true;
    return on != 0;
}

static void NotifyPeekSettingsChanged() noexcept {
    DWORD_PTR ignored = 0;
    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)kPeekCacheGroup,
                        SMTO_ABORTIFHUNG, 800, &ignored);
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (tray)
        SendMessageTimeoutW(tray, WM_SETTINGCHANGE, 0, (LPARAM)kPeekCacheGroup,
                            SMTO_ABORTIFHUNG, 800, &ignored);
}

static bool TogglePeekAtDesktop(bool* nowEnabled) {
    if (g_unloading.load(std::memory_order_acquire) ||
        !g_peekRegistryHookReady.load(std::memory_order_acquire)) {
        if (nowEnabled) *nowEnabled = PeekAtDesktopEnabled();
        return false;
    }

    const bool want = !PeekAtDesktopEnabled();
    g_peekVirtualDisablePreviewDesktop.store(want ? 0 : 1, std::memory_order_release);
    g_peekDisableOverrideActive.store(true, std::memory_order_release);
    if (want) {
        // The old shell enables Aero Peek when this item is switched on. Virtualize that
        // write too, so the setting is temporary and the real registry remains untouched.
        g_peekVirtualEnableAeroPeek.store(1, std::memory_order_release);
        g_peekAeroOverrideActive.store(true, std::memory_order_release);
    }

    // Verify that the hook can see the exact open-key handles before reporting success. If
    // key-name inspection fails on a future Windows build, clear the override instead of
    // showing a checkbox that appears to work but does not affect the shell.
    if (PeekAtDesktopEnabled() != want || (want && !PeekAeroAllowed())) {
        g_peekDisableOverrideActive.store(false, std::memory_order_release);
        g_peekAeroOverrideActive.store(false, std::memory_order_release);
        g_peekRegistryHookReady.store(false, std::memory_order_release);
        NotifyPeekSettingsChanged();
        if (nowEnabled) *nowEnabled = PeekAtDesktopEnabled();
        return false;
    }

    NotifyPeekSettingsChanged();
    if (nowEnabled) *nowEnabled = want;
    return true;
}

// Called from Wh_ModBeforeUninit while the hook is still installed. Dropping the virtual
// values before notifying the shell makes its next query see the real, untouched registry.
static void ClearPeekOverridesOnUnload() noexcept {
    g_peekRegistryHookReady.store(false, std::memory_order_release);
    const bool hadDisableOverride =
        g_peekDisableOverrideActive.exchange(false, std::memory_order_acq_rel);
    const bool hadAeroOverride =
        g_peekAeroOverrideActive.exchange(false, std::memory_order_acq_rel);
    if (hadDisableOverride || hadAeroOverride) {
        NotifyPeekSettingsChanged();
        Wh_Log(L"[menu] process-local Peek overrides cleared; the real registry values were "
               L"never modified");
    }
}

// ===========================================================================
// ShellOpGuard - isolates shell namespace launches from menu callbacks.
//
// The caller has a configurable wait cap; a timed-out ShellExecute operation is
// not forcibly terminated. Its worker and thread handle stay tracked, and unload
// joins every outstanding worker before the mod DLL can be released. This avoids
// returning from Wh_ModUninit while a worker can still execute mod code.
// C++ try/catch is used; no SEH/VEH handler or native access-violation recovery is installed.
//
// Rules:
//   * ShellExecute runs on a worker, not on the menu thread;
//   * the configured timeout bounds the caller's wait, not the OS operation;
//   * on unload, all workers are joined (unload can wait for a slow Shell API);
//   * no C++ exception escapes these wrappers. Success is reported only when
//     ShellExecuteW returns a value > 32.
// ===========================================================================
namespace ShellOpGuard {

enum class Outcome : int {
    Completed = 0,
    Failed,
    TimedOut,
    CppException,
    NotStarted,
};

using Body = void (WINAPI*)(void*);

struct WorkSpec {
    Body body = nullptr;
    void* context = nullptr;
    void (*contextDelete)(void*) = nullptr;
    INT_PTR (*contextResult)(const void*) = nullptr;
};

struct WorkItem {
    WorkSpec spec{};
    unsigned long fault = 0;
    std::atomic<int> state{0};   // 0 = running, 1 = finished, 2 = detached by caller
};

static std::atomic<int> g_liveWorkers{0};
static std::atomic<int> g_detachedWorkers{0};
static std::atomic<bool> g_guardStopping{false};
static std::atomic<unsigned int> g_faultLogs{0};
static std::atomic<unsigned int> g_activeRunCalls{0};
static std::mutex g_runGateMutex;
static std::mutex g_workerHandlesMutex;
static std::vector<HANDLE> g_detachedWorkerHandles;

static const wchar_t* OutcomeName(Outcome outcome) noexcept {
    switch (outcome) {
        case Outcome::Completed:    return L"completed";
        case Outcome::Failed:       return L"not completed";
        case Outcome::TimedOut:     return L"late; worker retained until it exits";
        case Outcome::CppException: return L"C++ exception";
        default:                    return L"not started";
    }
}

static bool TryBeginRun() noexcept {
    try {
        std::lock_guard<std::mutex> lock(g_runGateMutex);
        if (g_guardStopping.load(std::memory_order_acquire)) return false;
        g_activeRunCalls.fetch_add(1, std::memory_order_acq_rel);
        return true;
    } catch (...) {
        return false;
    }
}

static void EndRun() noexcept {
    g_activeRunCalls.fetch_sub(1, std::memory_order_acq_rel);
}

class ActiveRunScope {
public:
    ActiveRunScope() noexcept = default;
    ~ActiveRunScope() { if (m_active) EndRun(); }
    ActiveRunScope(const ActiveRunScope&) = delete;
    ActiveRunScope& operator=(const ActiveRunScope&) = delete;
    void activate() noexcept { m_active = true; }
private:
    bool m_active = false;
};

static void DeleteContextNoThrow(const WorkSpec& spec) noexcept {
    if (!spec.contextDelete || !spec.context) return;
    try {
        spec.contextDelete(spec.context);
    } catch (...) {
        Wh_Log(L"[shell-guard] C++ exception while releasing an operation context");
    }
}

// The thread handle is retained even after the caller's timeout. A signaled
// handle can be closed immediately; otherwise Wh_ModUninit waits on it.
static void KeepWorkerHandleUntilExit(HANDLE thread) noexcept {
    if (!thread) return;
    if (WaitForSingleObject(thread, 0) == WAIT_OBJECT_0) {
        CloseHandle(thread);
        return;
    }

    bool stored = false;
    try {
        std::lock_guard<std::mutex> lock(g_workerHandlesMutex);
        for (size_t i = 0; i < g_detachedWorkerHandles.size();) {
            if (WaitForSingleObject(g_detachedWorkerHandles[i], 0) == WAIT_OBJECT_0) {
                CloseHandle(g_detachedWorkerHandles[i]);
                g_detachedWorkerHandles.erase(g_detachedWorkerHandles.begin() + i);
            } else {
                ++i;
            }
        }
        g_detachedWorkerHandles.push_back(thread);
        stored = true;
    } catch (...) {
        Wh_Log(L"[shell-guard] could not track a late worker; waiting for it before returning");
    }

    if (!stored) {
        // Safer to delay the caller in this rare allocation/locking failure than
        // to let the DLL unload while this worker can still execute its code.
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
}

static DWORD WINAPI WorkThreadProc(LPVOID raw) noexcept {
    WorkItem* item = static_cast<WorkItem*>(raw);
    g_liveWorkers.fetch_add(1, std::memory_order_acq_rel);
    try {
        DWORD cppFault = 0;
        if (!CppGuard::RunGuarded(L"shell:::", item->spec.body,
                                  item->spec.context, &cppFault)) {
            item->fault = cppFault ? cppFault : CppGuard::kCppExceptionCode;
        }
    } catch (...) {
        item->fault = CppGuard::kCppExceptionCode;
    }

    int expected = 0;
    if (!item->state.compare_exchange_strong(expected, 1, std::memory_order_acq_rel)) {
        // The caller timed out and transferred WorkItem/context ownership here.
        DeleteContextNoThrow(item->spec);
        delete item;
        g_detachedWorkers.fetch_sub(1, std::memory_order_acq_rel);
    }
    g_liveWorkers.fetch_sub(1, std::memory_order_acq_rel);
    return 0;
}

// Executes a shell operation on a worker. A caller timeout does not terminate
// the worker: its handle is retained so the module can join it before unload.
static Outcome Run(PCWSTR what, const WorkSpec& spec, DWORD timeoutMs,
                   INT_PTR* outCode, DWORD* outError) noexcept {
    if (outCode) *outCode = 0;
    if (outError) *outError = 0;
    if (!spec.body || !spec.context) {
        DeleteContextNoThrow(spec);
        return Outcome::NotStarted;
    }
    if (!TryBeginRun()) {
        DeleteContextNoThrow(spec);
        return Outcome::NotStarted;
    }
    ActiveRunScope activeRun;
    activeRun.activate();

    if (timeoutMs < 200) timeoutMs = 200;

    WorkItem* item = nullptr;
    try {
        item = new WorkItem();
    } catch (...) {
        Wh_Log(L"[shell-guard] %s: work item cannot be allocated; operation cancelled", what);
        DeleteContextNoThrow(spec);
        return Outcome::NotStarted;
    }
    item->spec = spec;

    HANDLE thread = CreateThread(nullptr, 0, WorkThreadProc, item, 0, nullptr);
    if (!thread) {
        const DWORD error = GetLastError();
        DeleteContextNoThrow(spec);
        delete item;
        Wh_Log(L"[shell-guard] %s: service thread not created (%lu)", what, error);
        return Outcome::NotStarted;
    }

    const DWORD waited = WaitForSingleObject(thread, timeoutMs);
    Outcome outcome = Outcome::Completed;
    unsigned long fault = 0;
    if (waited == WAIT_OBJECT_0) {
        if (outCode && spec.contextResult) *outCode = spec.contextResult(item->spec.context);
        fault = item->fault;
        if (fault) outcome = Outcome::CppException;
        DeleteContextNoThrow(item->spec);
        item->spec.context = nullptr;
        delete item;
        item = nullptr;
    } else if (waited == WAIT_TIMEOUT) {
        int expected = 0;
        if (item->state.compare_exchange_strong(expected, 2, std::memory_order_acq_rel)) {
            g_detachedWorkers.fetch_add(1, std::memory_order_acq_rel);
            item = nullptr;  // worker now owns item and its context
            outcome = Outcome::TimedOut;
        } else {
            // Finished concurrently with the timeout, but the thread may still be
            // executing its final instructions; keep its handle until termination.
            if (outCode && spec.contextResult) *outCode = spec.contextResult(item->spec.context);
            fault = item->fault;
            if (fault) outcome = Outcome::CppException;
            DeleteContextNoThrow(item->spec);
            item->spec.context = nullptr;
            delete item;
            item = nullptr;
        }
    } else {
        int expected = 0;
        if (item->state.compare_exchange_strong(expected, 2, std::memory_order_acq_rel)) {
            g_detachedWorkers.fetch_add(1, std::memory_order_acq_rel);
            item = nullptr;
        } else {
            fault = item->fault;
            DeleteContextNoThrow(item->spec);
            item->spec.context = nullptr;
            delete item;
            item = nullptr;
        }
        outcome = Outcome::Failed;
    }

    if (waited == WAIT_OBJECT_0) {
        CloseHandle(thread);
    } else {
        KeepWorkerHandleUntilExit(thread);
    }

    if (fault && g_faultLogs.fetch_add(1, std::memory_order_acq_rel) < 8)
        Wh_Log(L"[shell-guard] %s: C++ exception caught (0x%08X)", what, fault);
    if (waited == WAIT_TIMEOUT && outcome == Outcome::TimedOut)
        Wh_Log(L"[shell-guard] %s: caller timed out after %lu ms; worker continues, "
               L"its handle is retained for safe unload", what, timeoutMs);
    else if (waited != WAIT_OBJECT_0 && outcome == Outcome::Failed)
        Wh_Log(L"[shell-guard] %s: wait failed (%lu); worker handle retained for safe unload",
               what, GetLastError());
    return outcome;
}

// Called from Wh_ModBeforeUninit, before Windhawk removes the hooks. New shell operations are refused.
static void BeginShutdown() noexcept {
    try {
        std::lock_guard<std::mutex> lock(g_runGateMutex);
        g_guardStopping.store(true, std::memory_order_release);
    } catch (...) {
        g_guardStopping.store(true, std::memory_order_release);
    }
}

// Wait for all Run callers to finish registering their handles, then join every
// timed-out worker. A slow Shell API can delay unload; it cannot outlive the DLL.
static void Shutdown() noexcept {
    BeginShutdown();
    while (g_activeRunCalls.load(std::memory_order_acquire) != 0) Sleep(10);

    for (;;) {
        HANDLE thread = nullptr;
        try {
            std::lock_guard<std::mutex> lock(g_workerHandlesMutex);
            if (g_detachedWorkerHandles.empty()) break;
            thread = g_detachedWorkerHandles.back();
            g_detachedWorkerHandles.pop_back();
        } catch (...) {
            Wh_Log(L"[shell-guard] worker-list lock raised a C++ exception; retrying");
            Sleep(10);
            continue;
        }
        if (thread) {
            const DWORD waited = WaitForSingleObject(thread, INFINITE);
            if (waited != WAIT_OBJECT_0)
                Wh_Log(L"[shell-guard] WaitForSingleObject on a worker failed (%lu)", GetLastError());
            CloseHandle(thread);
        }
    }
    Wh_Log(L"[shell-guard] all late shell workers have exited; safe to unload");
}

// ---- the concrete body: open a shell URL / name-space panel ------------------
// the strings are copied into the context, not referenced. A worker that
// overruns the timeout is detached and keeps running: it must not read a caller
// buffer (or a caller stack frame) that no longer exists.
struct OpenUriCall {
    wchar_t uri[512] = {};
    INT_PTR code = 0;
    DWORD error = 0;
};

// The worker is a plain thread: COM is not initialized on it, and a "shell:::" target is
// resolved through COM (a shell namespace extension), so the call can fail depending on
// whether the thread happens to carry an apartment. It is initialized here, around the call.
static void OpenUriBody(void* raw) {
    OpenUriCall* call = static_cast<OpenUriCall*>(raw);
    SetLastError(0);
    const HRESULT co = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const HINSTANCE result = ShellExecuteW(nullptr, L"open", call->uri,
                                           nullptr, nullptr, SW_SHOWNORMAL);
    call->code = reinterpret_cast<INT_PTR>(result);
    call->error = GetLastError();
    if (SUCCEEDED(co)) CoUninitialize();
}

static void DeleteOpenUriCall(void* raw) { delete static_cast<OpenUriCall*>(raw); }
static INT_PTR ResultOfOpenUriCall(const void* raw) {
    return static_cast<const OpenUriCall*>(raw)->code;
}

static DWORD GuardTimeoutMs() noexcept {
    int configured = g_cfg.shellOpGuardTimeoutMs;
    if (configured < 200) configured = 200;
    if (configured > 10000) configured = 10000;
    return static_cast<DWORD>(configured);
}

// Opens a "shell:::" URL or a shell name-space panel without ever propagating an
// exception and without blocking the caller beyond the time cap.
static bool OpenShellUriGuarded(const wchar_t* uri, const wchar_t* what) noexcept {
    if (!uri || !*uri) return false;
    OpenUriCall* call = nullptr;
    try {
        call = new OpenUriCall();
    } catch (...) {
        Wh_Log(L"[shell-guard] %s: context cannot be allocated", what);
        return false;
    }
    wcsncpy_s(call->uri, uri, _TRUNCATE);   // copy: the worker can outlive its caller
    if (!call->uri[0]) { delete call; return false; }

    const WorkSpec spec{ OpenUriBody, call, DeleteOpenUriCall, ResultOfOpenUriCall };
    INT_PTR code = 0;
    DWORD error = 0;
    const Outcome outcome = Run(what, spec, GuardTimeoutMs(), &code, &error);

    if (outcome == Outcome::Completed && code > 32) {
        Wh_Log(L"[shell-guard] %s: opened (%s)", what, uri);
        return true;
    }
    if (outcome == Outcome::TimedOut || outcome == Outcome::Failed) return false;
    if (code <= 32) {
        Wh_Log(L"[shell-guard] %s: the shell refused to open it (code %ld, error %lu): %s",
               what, static_cast<long>(code), error, uri);
    } else {
        Wh_Log(L"[shell-guard] %s: %s", what, OutcomeName(outcome));
    }
    return false;
}

// ---- the concrete body: a command with arguments (msdt.exe and the like) ------
struct RunCommandCall {
    wchar_t file[320] = {};
    wchar_t params[512] = {};
    wchar_t verb[16] = L"open";   // "open", or "runas" for the administrative entries
    INT_PTR code = 0;
    DWORD error = 0;
};

static void RunCommandBody(void* raw) {
    RunCommandCall* call = static_cast<RunCommandCall*>(raw);
    SetLastError(0);
    const HRESULT co = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);   // see OpenUriBody
    const HINSTANCE result = ShellExecuteW(nullptr, call->verb, call->file, call->params,
                                           nullptr, SW_SHOWNORMAL);
    call->code = reinterpret_cast<INT_PTR>(result);
    call->error = GetLastError();
    if (SUCCEEDED(co)) CoUninitialize();
}

static void DeleteRunCommandCall(void* raw) { delete static_cast<RunCommandCall*>(raw); }
static INT_PTR ResultOfRunCommandCall(const void* raw) {
    return static_cast<const RunCommandCall*>(raw)->code;
}

// Runs an external command with the same protection as the shell::: operations: a service
// thread, a time cap and a C++ catch (no SEH, no VEH).
static bool RunCommandGuarded(const wchar_t* file, const wchar_t* params,
                              const wchar_t* verb, const wchar_t* what) noexcept {
    const wchar_t* callVerb = (verb && *verb) ? verb : L"open";
    if (!file || !*file) return false;
    RunCommandCall* call = nullptr;
    try {
        call = new RunCommandCall();
    } catch (...) {
        Wh_Log(L"[shell-guard] %s: context cannot be allocated", what);
        return false;
    }
    // Copies: the context outlives the caller when the worker is detached.
    wcsncpy_s(call->file, file, _TRUNCATE);
    if (params) wcsncpy_s(call->params, params, _TRUNCATE);
    if (verb && *verb) wcsncpy_s(call->verb, verb, _TRUNCATE);
    if (!call->file[0]) { delete call; return false; }
    const WorkSpec spec{ RunCommandBody, call, DeleteRunCommandCall, ResultOfRunCommandCall };
    INT_PTR code = 0;
    DWORD error = 0;
    const Outcome outcome = Run(what, spec, GuardTimeoutMs(), &code, &error);
    if (outcome == Outcome::Completed && code > 32) {
        Wh_Log(L"[shell-guard] %s: started (%s %s %s)", what, callVerb, file,
               params ? params : L"");
        return true;
    }
    if (outcome == Outcome::TimedOut || outcome == Outcome::Failed) return false;
    Wh_Log(L"[shell-guard] %s: start failed (code %ld, error %lu)", what,
           static_cast<long>(code), error);
    return false;
}

}  // namespace ShellOpGuard

// The class of the show desktop button: it tells that window apart from every other
// window of the tray, which is what the menus of this mod hang on.
static bool IsShowDesktopClass(const wchar_t* cls) {
    return cls && _wcsicmp(cls, L"TrayShowDesktopButtonWClass") == 0;
}

static int g_peekMenuLogs = 0;

// The menu of the show desktop button: the same two entries of Windows 10, with
// the same behaviour (checkmark, greyed out when Aero Peek is off).
static void ShowShowDesktopMenu(HWND owner) {
    const UiLangId lang = DetectUiLang();
    HMENU menu = CreatePopupMenu();
    if (!menu) return;
    AppendMenuW(menu, MF_STRING, IDM_MOD_SHOWDESKTOP, kShowDesktopText[lang]);
    SetMenuDefaultItem(menu, IDM_MOD_SHOWDESKTOP, FALSE);
    UINT flags = MF_STRING | (PeekAtDesktopEnabled() ? MF_CHECKED : 0);
    if (!g_peekRegistryHookReady.load(std::memory_order_acquire) || !PeekAeroAllowed())
        flags |= MF_GRAYED;
    AppendMenuW(menu, flags, IDM_MOD_PEEK, kPeekText[lang]);
    if (g_peekMenuLogs < 5) {
        g_peekMenuLogs++;
        Wh_Log(L"[menu] show desktop button: menu shown (peek %s, Aero Peek %s)",
               PeekAtDesktopEnabled() ? L"on" : L"off", PeekAeroAllowed() ? L"available" : L"off");
    }
    POINT pt = {};
    if (!GetCursorPos(&pt)) GetCursorPos(&pt);
    const UINT command = (UINT)TrackPopupMenuEx(
        menu, TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y, owner, nullptr);
    if (command) HandleClassicMenuCommand(command);
    DestroyMenu(menu);
}

static bool HandleClassicMenuCommand(UINT id) {
    try {
        const wchar_t* label = nullptr;
        bool ok = false;
        switch (id) {

        case IDM_MOD_SHOWDESKTOP:
            label = L"show desktop";
            ok = ToggleDesktopLikeWin10();
            break;
        case IDM_MOD_PEEK: {
            bool nowOn = false;
            ok = TogglePeekAtDesktop(&nowOn);
            if (ok)
                Wh_Log(L"[menu] peek at desktop turned %s", nowOn ? L"on" : L"off");
            else
                Wh_Log(L"[menu] peek toggle was not applied; process-local virtualization is "
                       L"unavailable");
            label = L"peek at desktop";
            break;
        }

        // The two entries of the battery menu are not handled here: ShowBatteryMenu maps
        // them onto the power commands of stobject itself. Claiming the id and doing nothing
        // used to leave the caller with a menu that reported a command that never ran.
        default:
            return false;
        }
        if (g_classicCmdLogs < 10) {
            g_classicCmdLogs++;
            Wh_Log(L"[menu] classic entry chosen: %s (%s)", label, ok ? L"done" : L"failed");
        }
        return true;
    } catch (...) {
        return false;
    }
}

// --- network: guarded routing for non-PNI network launches -------------------
//
// The old generic workaround can still rewrite a network-settings launch from another
// caller. A click delivered to pnidui's real PNIHiddenWnd is different: the URI
// fallbacks of this mod are blocked below, and the dynamic icon fallback never
// installs its own click handler or launches a URI.
//
// The language-indicator fix that used to be described next to this one is not part of
// this mod: it lives in the separate mod "windows-10-language-flyout-guard", and the
// hooks it needed were never registered here.
static const wchar_t* kNetworkSettingsUri = L"ms-settings:network";
static const wchar_t* kNetworkFlyoutUri = L"ms-availablenetworks:";

// The comparison used to be exact: "ms-settings:network-wifi" or
// "ms-settings:network-status" slipped through and opened Settings.
static bool IsNetworkSettingsUri(LPCWSTR target) {
    return target && _wcsnicmp(target, kNetworkSettingsUri, wcslen(kNetworkSettingsUri)) == 0;
}

static bool IsPniduiCallerAddress(const void* address) noexcept {
    try {
        HMODULE caller = nullptr;
        const DWORD flags = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
        if (!address || !GetModuleHandleExW(flags, (LPCWSTR)address, &caller) || !caller) return false;
        HMODULE pnidui = GetModuleHandleW(L"pnidui.dll");
        return pnidui && caller == pnidui;
    } catch (...) { return false; }
}

static bool IsPniduiNetworkUri(LPCWSTR target) {
    return IsNetworkSettingsUri(target) ||
           (target && _wcsnicmp(target, kNetworkFlyoutUri, wcslen(kNetworkFlyoutUri)) == 0);
}

// Every network page of this shell is answered with the Windows 10 flyout, for every
// caller. The list is explicit on purpose: a target that is not recognised here is left alone.
static bool IsBlockedNetworkPage(LPCWSTR target) {
    if (IsPniduiNetworkUri(target)) return true;   // ms-settings:network* and ms-availablenetworks:
    static const wchar_t* const kPages[] = {
        L"ms-settings:wifi",          L"ms-settings:ethernet",    L"ms-settings:vpn",
        L"ms-settings:airplanemode",  L"ms-settings:datausage",   L"ms-settings:cellulardata",
        L"ms-settings:mobilehotspot", L"ms-settings:proximity",   L"ms-settings:nfctransactions",
    };
    for (const wchar_t* page : kPages)
        if (target && _wcsnicmp(target, page, wcslen(page)) == 0) return true;
    return false;
}

// The click on the network icon opens the genuine Windows 10 flyout through the shell
// experience manager. It is defined further down, inside NetworkTrayForce (it needs the tray
// icon rectangle and the PNI registration), and declared here in that same namespace: a
// declaration at this level would name a different function, which nothing ever defines.
namespace NetworkTrayForce {
static HRESULT OpenAuthenticNetworkFlyout(const wchar_t* reason) noexcept;
static UINT ShowNetworkFlyoutMessage() noexcept;
static bool RequestNetworkFlyout() noexcept;
}
typedef HINSTANCE(WINAPI* ShellExecuteW_t)(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, INT);
static ShellExecuteW_t ShellExecuteW_Original = nullptr;

static HINSTANCE WINAPI ShellExecuteW_Hook(HWND hwnd, LPCWSTR operation, LPCWSTR file,
                                           LPCWSTR parameters, LPCWSTR directory, INT show) {
    try {
        if (IsBlockedNetworkPage(file)) {
            // No page, ever, and nothing that can switch it back off. Every
            // ms-settings:network* / ms-availablenetworks: target of this shell becomes a
            // request for the Windows 10 flyout, and the caller is told that it succeeded so
            // that it does not try anything else. Settings is where this used to end: from
            // here it can no longer be reached, by pnidui or by anyone else.
            const bool fromPnidui = IsPniduiCallerAddress(WhReturnAddress());
            Wh_Log(L"[network] %s: no page is launched, the Windows 10 flyout is requested "
                   L"(caller pnidui: %d)", file ? file : L"(null)", fromPnidui ? 1 : 0);
            NetworkTrayForce::RequestNetworkFlyout();
            return (HINSTANCE)33;   // > 32 = success: the caller does not retry
        }
    } catch (...) {
        Wh_Log(L"[network] exception while rewriting, using the original target");
    }
    return ShellExecuteW_Original(hwnd, operation, file, parameters, directory, show);
}

typedef BOOL(WINAPI* ShellExecuteExW_t)(SHELLEXECUTEINFOW*);
static ShellExecuteExW_t ShellExecuteExW_Original = nullptr;          // aggancio globale
static ShellExecuteExW_t ShellExecuteExW_TargetedOriginal = nullptr;  // the targeted hook (IAT of pnidui)

static BOOL HandleNetworkShellExecute(SHELLEXECUTEINFOW* info, ShellExecuteExW_t original,
                                      bool fromPnidui) {
    if (!info || !original) return FALSE;
    try {
        if (IsBlockedNetworkPage(info->lpFile)) {
            // See ShellExecuteW_Hook. No setting, no forwarding, no page.
            Wh_Log(L"[network] %s: no page is launched, the Windows 10 flyout is requested "
                   L"(caller pnidui: %d)", info->lpFile ? info->lpFile : L"(null)",
                   fromPnidui ? 1 : 0);
            NetworkTrayForce::RequestNetworkFlyout();
            info->hInstApp = (HINSTANCE)33;
            return TRUE;
        }
    } catch (...) {
        Wh_Log(L"[network] exception while rewriting, using the original target");
    }
    return original(info);
}

static BOOL WINAPI ShellExecuteExW_Hook(SHELLEXECUTEINFOW* info) {
    return HandleNetworkShellExecute(info, ShellExecuteExW_Original,
                                     IsPniduiCallerAddress(WhReturnAddress()));
}

static BOOL WINAPI ShellExecuteExW_TargetedHook(SHELLEXECUTEINFOW* info) {
    return HandleNetworkShellExecute(info, ShellExecuteExW_TargetedOriginal,
                                     IsPniduiCallerAddress(WhReturnAddress()));
}

// Defined further down with the rest of the IAT helpers of the reference mod.
static void** FindIatSlot(HMODULE module, const char* dllHint, const char* funcHint);

// [diag-fix] The attempt to take over the import table entry of ShellExecuteExW in pnidui used
// to happen once, from Wh_ModInit. If pnidui.dll was not loaded at that moment - a real restart
// of explorer.exe loads it later than a plain reload of the mod in a process that has been up
// for a while - the attempt failed for good and was never repeated: the click on the icon then
// fell back to the native behaviour (it opens Settings), because the global hook on
// ShellExecuteExW alone is not enough (pnidui keeps its own copy of the address). It is now safe
// to call more than once: when pnidui.dll is not there yet nothing is marked as finished, so a
// later call can still succeed.
static std::atomic<bool> g_pniduiShellExecuteExResolved{false};

// [fix] The import table entry of pnidui stays ours while the mod is loaded: it is put back by
// Wh_ModUninit (RestorePniduiShellExecuteExIatOnUnload, below), like every other import table
// entry this mod borrows.
static void** g_pniduiShellExecuteExIatSlot = nullptr;

static void TryHookPniduiShellExecuteExIat(const wchar_t* reason) {
    // While the mod is going away the entry must not be taken any more - the restore
    // runs on the unload path, and a write landing after it would leave the IAT of pnidui
    // pointing into an image that is about to be unmapped.
    if (g_unloading.load(std::memory_order_acquire)) return;
    if (g_pniduiShellExecuteExResolved.load(std::memory_order_acquire)) return;
    HMODULE pnidui = GetModuleHandleW(L"pnidui.dll");
    if (!pnidui) return;
    if (g_pniduiShellExecuteExResolved.exchange(true, std::memory_order_acq_rel)) return;

    void** slot = FindIatSlot(pnidui, "SHELL32", "ShellExecuteExW");
    if (slot && *slot) {
        void* target = *slot;
        if (target == (void*)ShellExecuteExW) {
            Wh_Log(L"[network] ShellExecuteExW: pnidui calls the same address we hook "
                   L"(0x%p, %s)", target, reason);
        } else {
            // [fix] This used to be a Wh_SetFunctionHook(): that only works when
            // Wh_ApplyHookOperations runs afterwards, and the engine does that once, right
            // after Wh_ModInit returns (see the note on Wh_ApplyHookOperations below for why
            // this mod never calls it by hand: calling it from a thread other than Wh_ModInit's
            // races with the engine's own operations on the same queue, and in the past it took
            // explorer.exe down at every enable and disable). A registration made later than
            // that - like this one, when pnidui is loaded after Wh_ModInit has already returned
            // - therefore stayed queued and inert: the targeted hook never took effect. Fixed
            // by writing that one entry directly (a single pointer, like the bytes
            // PatchQuickActionsTemplates rewrites), which works no matter when it runs.
            DWORD oldProtect = 0;
            if (VirtualProtect(slot, sizeof(void*), PAGE_READWRITE, &oldProtect)) {
                ShellExecuteExW_TargetedOriginal = reinterpret_cast<ShellExecuteExW_t>(target);
                *slot = reinterpret_cast<void*>(&ShellExecuteExW_TargetedHook);
                DWORD ignored = 0;
                VirtualProtect(slot, sizeof(void*), oldProtect, &ignored);
                g_pniduiShellExecuteExIatSlot = slot;
                Wh_Log(L"[network] ShellExecuteExW: the import table entry used by pnidui now "
                       L"points to this mod (0x%p, %s)", target, reason);
            } else {
                Wh_Log(L"[network] ShellExecuteExW: the import table entry used by pnidui could "
                       L"not be made writable (%s)", reason);
            }
        }
    } else {
        Wh_Log(L"[network] ShellExecuteExW: pnidui loaded but its ShellExecuteExW IAT slot "
               L"was not found (%s)", reason);
    }
}

// Puts the import table entry of pnidui back the way it was, while this module is still
// mapped. Called from Wh_ModUninit, after every thread of the mod has really stopped.
static void RestorePniduiShellExecuteExIatOnUnload() noexcept {
    void** slot = g_pniduiShellExecuteExIatSlot;
    if (!slot || !ShellExecuteExW_TargetedOriginal) return;
    g_pniduiShellExecuteExIatSlot = nullptr;
    DWORD oldProtect = 0;
    if (VirtualProtect(slot, sizeof(void*), PAGE_READWRITE, &oldProtect)) {
        // The entry is put back only if it still points at this mod's hook. Anything
        // else that was written there in the meantime (by this mod or by another one) is left
        // alone: overwriting it would hand the caller something that is not the original.
        const void* original = reinterpret_cast<const void*>(ShellExecuteExW_TargetedOriginal);
        if (InterlockedCompareExchangePointer(slot, const_cast<void*>(original),
                                              reinterpret_cast<void*>(&ShellExecuteExW_TargetedHook)) ==
            reinterpret_cast<void*>(&ShellExecuteExW_TargetedHook))
            FlushInstructionCache(GetCurrentProcess(), slot, sizeof(void*));
        DWORD ignored = 0;
        VirtualProtect(slot, sizeof(void*), oldProtect, &ignored);
    }
}

// Installs the two entry points of the network click. The global hook covers every
// caller; the slot pnidui calls through its import table is hooked as well, because a
// module that keeps a copy of the address would otherwise slip past the global one.
static bool InstallShellExecuteExHooks() {
    bool installed = false;

    if (Wh_SetFunctionHook((void*)ShellExecuteExW, (void*)ShellExecuteExW_Hook,
                           (void**)&ShellExecuteExW_Original))
        installed = true;

    TryHookPniduiShellExecuteExIat(L"Wh_ModInit");
    if (!GetModuleHandleW(L"pnidui.dll"))
        Wh_Log(L"[network] ShellExecuteExW: pnidui not loaded yet, the global hook stays "
               L"for now (retried as the process goes on)");

    return installed;
}

// --- the right-click menu of the native network icon: pnidui's resource 3014 -----
// evidence (pnidui.dll 10.0.19041.7663, disassembled): on a right click pnidui calls
// LoadMenuW(<pnidui>, 3014), takes submenu 0, shows it with TrackPopupMenu and handles
// the two entries itself (3107 troubleshoot problems, 3109 open network settings).
// Resource 3014 is not inside pnidui.dll but in its MUI, and this build ships no MUI
// for it: LoadMenuW returns NULL, pnidui stops there, and the right click opens nothing.
// The menu is therefore built here, in the language of the UI, with the two command ids
// pnidui expects - so the menu is pnidui's own, with pnidui's own handlers, and this mod
// invents nothing. The hook answers one (module, resource) pair; every other LoadMenuW
// of this process goes to the original untouched.
typedef HMENU(WINAPI* LoadMenuW_t)(HINSTANCE, LPCWSTR);
static LoadMenuW_t LoadMenuW_Original = nullptr;
static int g_netMenuServed = 0;

static HMENU BuildNetworkIconMenu() {
    const UiLangId lang = DetectUiLang();
    HMENU bar = CreateMenu();
    HMENU popup = CreatePopupMenu();
    if (!bar || !popup) {
        if (bar) DestroyMenu(bar);
        if (popup) DestroyMenu(popup);
        return nullptr;
    }
    AppendMenuW(popup, MF_STRING, 3107, kNetTroubleshoot[lang]);
    AppendMenuW(popup, MF_STRING, 3109, kNetOpenSettings[lang]);
    AppendMenuW(bar, MF_POPUP | MF_STRING, (UINT_PTR)popup, L"");
    return bar;
}

static HMENU WINAPI LoadMenuW_Hook(HINSTANCE hInstance, LPCWSTR lpMenuName) {
    try {
        const HMODULE pnidui = GetModuleHandleW(L"pnidui.dll");
        if (pnidui && IS_INTRESOURCE(lpMenuName) && (HMODULE)hInstance == pnidui &&
            (UINT)(ULONG_PTR)lpMenuName == 3014) {
            HMENU menu = BuildNetworkIconMenu();
            if (menu) {
                if (g_netMenuServed < 3) {
                    g_netMenuServed++;
                    Wh_Log(L"[network] the menu of the network icon is handed to pnidui "
                           L"(resource 3014: troubleshoot, network settings)");
                }
                return menu;
            }
            Wh_Log(L"[network] resource 3014 asked for, the menu could not be built");
        }
    } catch (...) {
        Wh_Log(L"[network] exception while building the menu of the network icon");
    }
    return LoadMenuW_Original(hInstance, lpMenuName);
}

// The two entry points of the click are registered here, once, and only from
// Wh_ModInit, together with every other hook of this mod (see the note there). Both shims
// never forward a page to the shell: every ms-settings:network* and ms-availablenetworks:
// target of this shell is answered with a request for the Windows 10 flyout and with a
// success result, so nothing else is launched. The registration is not tied to a setting:
// Windhawk writes the settings of a mod when the mod is installed, so a setting added by a
// later version reads as 0 until they are saved again - and that is exactly how the old
// path (the Settings page) stayed active.
static void InstallNetworkClickHooks() {
    static bool done = false;
    if (done) return;
    done = true;

    if (Wh_SetFunctionHook((void*)ShellExecuteW, (void*)ShellExecuteW_Hook,
                           (void**)&ShellExecuteW_Original)) {
        Wh_Log(L"[init] network click hook active on ShellExecuteW: no page can be opened "
               L"from a network target, the Windows 10 flyout is requested instead");
    } else {
        Wh_Log(L"[init] ShellExecuteW hook unavailable: the click may reach the shell page");
    }

    if (InstallShellExecuteExHooks()) {
        Wh_Log(L"[init] network click hook active on ShellExecuteEx, and on the address "
               L"pnidui itself calls");
    } else {
        Wh_Log(L"[init] ShellExecuteEx hooks unavailable: the click may reach the shell page");
    }

    if (Wh_SetFunctionHook((void*)LoadMenuW, (void*)LoadMenuW_Hook,
                           (void**)&LoadMenuW_Original)) {
        Wh_Log(L"[init] the menu resource of pnidui is served (LoadMenuW)");
    } else {
        Wh_Log(L"[init] LoadMenuW hook unavailable: the right click on the network icon "
               L"stays without a menu");
    }
}

// -@ right click on the network icon (supervision only) ------------------
// the real menu belongs to pnidui and arrives from the hook above. Nothing is
// invented here and no message is absorbed.
// It only serves two purposes:
//   1. to find pnidui's service window again. It is an ordinary hidden
//      top-level window (CreateWindowExW with style 0 and parent 0), so it
//      is NOT under HWND_MESSAGE: the earlier search could not find it;
//   2. to note in the log the first times the right click arrives there,
//      so the log says whether the click reaches pnidui or gets lost earlier.
// The tray callback message carries the mouse message in lParam:
// the right button is recognised without knowing the exact number.
typedef BOOL(WINAPI* Shell_NotifyIconW_t)(DWORD, PNOTIFYICONDATAW);
static Shell_NotifyIconW_t Shell_NotifyIconW_Original = nullptr;
static int g_netIconLogs = 0;
struct SeenTrayIcon { HWND wnd; UINT id; };
static SeenTrayIcon g_trayIconSeen[24] = {};
static int g_trayIconSeenCount = 0;
static HWND g_netIconWnd = nullptr;
static UINT g_netIconMsg = 0;
static bool g_netMenuLogged = false;
static int g_netClickLogs = 0;

// Remember which (hwnd,id) tray-icon registrations are the battery meter
// (tooltip at registration time), so the right-click can be intercepted
// and served with our multilang menu.
static bool IsBatteryTooltipText(const wchar_t* tip) {
    if (!tip || !*tip) return false;
    const wchar_t* markers[] = {
        L"Power", L"Alimentazione", L"Aliment", L"Battery", L"Batterie",
        L"Batteria", L"Energie", L"Énergie", L"Energ", L"Akku",
        L"Stroom", L"Питан", L"Батар",
        L"電源", L"Zasilan",
    };
    for (const wchar_t* m : markers) {
        if (wcsstr(tip, m)) return true;
    }
    return false;
}
struct TrayOwnerInfo { HWND wnd; UINT id; bool isBattery; };
static TrayOwnerInfo g_trayOwnerInfo[24] = {};
static int g_trayOwnerInfoCount = 0;
static TrayOwnerInfo* FindTrayOwnerInfo(HWND wnd, UINT id) {
    for (int i = 0; i < g_trayOwnerInfoCount; i++)
        if (g_trayOwnerInfo[i].wnd == wnd && g_trayOwnerInfo[i].id == id)
            return &g_trayOwnerInfo[i];
    return nullptr;
}
static void RememberTrayOwner(HWND wnd, UINT id, const wchar_t* tip) {
    TrayOwnerInfo* t = FindTrayOwnerInfo(wnd, id);
    if (!t) {
        if (g_trayOwnerInfoCount >= (int)_countof(g_trayOwnerInfo)) return;
        t = &g_trayOwnerInfo[g_trayOwnerInfoCount++];
        t->wnd = wnd; t->id = id; t->isBattery = false;
    }
    if (IsBatteryTooltipText(tip)) t->isBattery = true;
}
// The battery of this shell does not announce itself with a text: its registration arrives
// with an empty one (window SystemTray_Main, id 1225, guid {7820AE75-...}, text ""). A guess
// based on the text therefore never caught it, and the icon was never known as the battery.
// What names it is the GUID, which does not depend on the language of the system: it is the
// same GUID the rest of the mod uses for its own system icon.
static const GUID kBatteryTrayIconGuid = {
    0x7820AE75, 0x23E3, 0x4229, { 0x82, 0xC1, 0xE4, 0x1C, 0xB6, 0x7D, 0x5B, 0x9C }
};
// Where the battery registered itself last (window and id) and with which message it calls
// that window: the click takeover, defined further down, works on these.
static HWND g_batteryTrayIconWnd = nullptr;
static UINT g_batteryTrayIconId = 0;
static UINT g_batteryTrayIconMessage = 0;
static int g_batteryTrayIconLogs = 0;

static bool IsBatteryTrayIconRegistration(const NOTIFYICONDATAW* data) noexcept {
    try {
        if (!data || !data->hWnd) return false;
        if (!(data->uFlags & NIF_GUID)) return false;
        if (data->cbSize < NOTIFYICONDATA_V3_SIZE) return false;
        return IsEqualGUID(data->guidItem, kBatteryTrayIconGuid) != FALSE;
    } catch (...) {
        return false;
    }
}

// Marks that icon as the battery and remembers where it is. Also runs once the log budget of
// the network icon is spent: the battery must not depend on that budget.
static void RememberBatteryTrayIcon(const NOTIFYICONDATAW* data) noexcept {
    try {
        if (!IsBatteryTrayIconRegistration(data)) return;
        const bool moved =
            (g_batteryTrayIconWnd != data->hWnd) || (g_batteryTrayIconId != data->uID);
        g_batteryTrayIconWnd = data->hWnd;
        g_batteryTrayIconId = data->uID;
        if (data->uCallbackMessage) g_batteryTrayIconMessage = data->uCallbackMessage;
        TrayOwnerInfo* t = FindTrayOwnerInfo(data->hWnd, data->uID);
        if (t) t->isBattery = true;
        if (moved && g_batteryTrayIconLogs++ < 6) {
            wchar_t cls[64] = {};
            GetClassNameW(data->hWnd, cls, _countof(cls));
            Wh_Log(L"[battery] icon found: window %s, id %u, message 0x%X: its own GUID names it, "
                   L"so the empty text of this registration does not matter",
                   cls, data->uID, (unsigned)data->uCallbackMessage);
        }
    } catch (...) {
    }
}
static bool IsBatteryServiceWindow(HWND hwnd) {
    for (int i = 0; i < g_trayOwnerInfoCount; i++)
        if (g_trayOwnerInfo[i].wnd == hwnd && g_trayOwnerInfo[i].isBattery)
            return true;
    return false;
}

// The menu of the network icon has to be shown on the thread of the taskbar (the same one the
// pnidui / stobject service windows live on), with Shell_TrayWnd as its owner, like the audio
// and battery menus. The private message carries the request onto that thread.
namespace NetworkTrayForce { static void ShowNetworkIconMenuHere(HWND owner) noexcept; }
// The click on the battery icon, in the window procedure below, asks for the Windows 10
// battery flyout. That function is defined further down, together with the rest of the battery
// code; a member of a namespace is usable only after it has been declared, and the window
// procedure comes first, so the declaration stands here, next to ShowNetworkIconMenuHere.
namespace NetworkTrayForce { namespace BatteryFlyout { bool RequestBatteryFlyout() noexcept; } }
// The battery branch of the window procedure below recognizes the click with the same
// helper the battery takeover uses (it is defined with that takeover, further down).
static bool BatteryIconClickIsOurs(UINT message, WPARAM wParam, LPARAM lParam) noexcept;
static UINT NetworkMenuMessage() {
    static UINT message = RegisterWindowMessageW(L"Win10ExplorerRestorer.ShowNetworkMenu");
    return message;
}
static bool BatteryIconClickIsOurs(UINT message, WPARAM wParam, LPARAM lParam) noexcept;
static HWND g_batteryClickWnd = nullptr;
static UINT g_batteryClickMessage = 0;
static UINT g_batteryClickId = 0;
static const UINT_PTR kBatteryClickSubclassId = 80;
static int g_batteryClickLogs = 0;
static bool BatteryIconClickIsOurs(UINT message, WPARAM wParam, LPARAM lParam) noexcept {
    try {
        const UINT event = LOWORD(lParam);
        if (message == g_batteryClickMessage && g_batteryClickMessage) {
            if (g_batteryClickId && (UINT)wParam != g_batteryClickId) return false;
            return event == WM_LBUTTONUP || event == NIN_SELECT || event == NIN_KEYSELECT ||
                   event == WM_LBUTTONDOWN;
        }
        if (message != WM_LBUTTONUP && message != WM_LBUTTONDOWN) return false;
        POINT pt = {};
        if (!GetCursorPos(&pt)) return false;
        NOTIFYICONIDENTIFIER identifier = {};
        identifier.cbSize = sizeof(identifier);
        identifier.guidItem = kBatteryTrayIconGuid;
        RECT icon = {};
        if (FAILED(Shell_NotifyIconGetRect(&identifier, &icon))) return false;
        return PtInRect(&icon, pt) != FALSE;
    } catch (...) {
        return false;
    }
}
// This used to be a raw SetWindowSubclass callback (6-parameter SUBCLASSPROC). The
// window it subclasses (PNIHiddenWnd / stobject's service window) belongs to the shell's own
// thread, not this mod's, so a raw SetWindowSubclass call here is a cross-thread call into
// comctl32 state that is not thread-safe, and the subclass was also never removed on unload.
// It now uses the same WindhawkUtils::*FromAnyThread machinery and 5-parameter signature as
// PniClickSubclassProc / BatteryClickSubclassProc / TrayMenuSubclassProc below, and is torn
// down in FlyoutServicesThread alongside them.
static LRESULT NetworkIconSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                       DWORD_PTR ref) {
    (void)ref;
    try {

        if (msg == NetworkMenuMessage()) {
            NetworkTrayForce::ShowNetworkIconMenuHere(hwnd);   // runs on the thread of the bar
            return 0;
        }
        // Right-click on the battery service window: intercept and show
        // our multilang menu (same style as the network menu); do NOT let
        // stobject build its own (which ends up empty because strings
        // 150/151 are empty in the Win10 MUI).
        const bool battery = IsBatteryServiceWindow(hwnd);
        // Left click on the battery icon. The handler of stobject.dll for this click is
        // never reached: the click is consumed here and the Windows 10 battery flyout is asked
        // for with the authentic call of this shell (shell experience manager, experience
        // Windows.Internal.ShellExperience.TrayBatteryFlyout). No page, no Win32 flyout, no
        // registry value.
        // The window this subclass sits on is the SERVICE WINDOW of stobject, which owns
        // more than the battery icon (volume is on it too), and it is fed messages that are not
        // clicks at all - WM_POWERBROADCAST, WM_SETTINGCHANGE, WM_DEVICECHANGE carry a pointer
        // in lParam, whose low word can be any of those codes. So the click is recognized with
        // the same helper the battery takeover uses: the icon's own callback message with the
        // icon's own id in wParam, or a click that really falls inside the icon's rectangle.
        if (battery) {
            if (BatteryIconClickIsOurs(msg, wParam, lParam)) {
                if (g_netClickLogs < 10) {
                    g_netClickLogs++;
                    wchar_t cls[64] = {};
                    GetClassNameW(hwnd, cls, _countof(cls));
                    Wh_Log(L"[battery] left click on the battery service window (class %s): the "
                           L"Windows 10 battery flyout is requested here, and the handler of "
                           L"stobject never sees the click", cls);
                }
                NetworkTrayForce::BatteryFlyout::RequestBatteryFlyout();
                return 0;   // consumed: no Win32 flyout, no page, no registry value
            }
        }
        if (battery && (msg == WM_CONTEXTMENU || msg == WM_RBUTTONUP)) {
            if (g_netClickLogs < 8) {
                g_netClickLogs++;
                wchar_t cls[64] = {};
                GetClassNameW(hwnd, cls, _countof(cls));
                Wh_Log(L"[battery] right click on battery service window (class %s): multilang menu served", cls);
            }
            ShowBatteryMenu(hwnd);
            return 0;
        }
        if (msg == WM_RBUTTONUP || msg == WM_CONTEXTMENU) {
            if (g_netClickLogs < 6) {
                g_netClickLogs++;
                wchar_t cls[64] = {};
                GetClassNameW(hwnd, cls, _countof(cls));
                Wh_Log(L"[tray] right click reached a service window (class %s, message 0x%X): the Windows component shows the menu",
                       cls, (unsigned)msg);
            }
        }
    } catch (...) {
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

// pnidui's service window is a hidden top-level window
// (seen in the disassembly: CreateWindowExW with style 0 and parent 0).
// Who owns the window? Every tray icon delivers its clicks to the window of
// the module that registered it: pnidui for the network ("PNIHiddenWnd"), stobject
// for battery and volume (its service windows). The module is recognised
// through GWLP_HINSTANCE, so no path comparison is needed (the path spoof
// would falsify it).
static const wchar_t* TrayOwnerModuleOfWindow(HWND w) {
    HINSTANCE inst = (HINSTANCE)GetWindowLongPtrW(w, GWLP_HINSTANCE);
    if (!inst) return nullptr;
    if (inst == GetModuleHandleW(L"stobject.dll")) return L"stobject.dll";
    if (inst == GetModuleHandleW(L"pnidui.dll")) return L"pnidui.dll";
    return nullptr;
}

static HWND g_trayWnds[2] = {};
static int g_trayWndCount = 0;
static const UINT_PTR kNetworkIconSubclassId = 81;

static BOOL CALLBACK FindPniWindowProc(HWND w, LPARAM param) {
    wchar_t cls[64] = {};
    if (!GetClassNameW(w, cls, _countof(cls))) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(w, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;

    const wchar_t* owner = TrayOwnerModuleOfWindow(w);
    bool isPni = _wcsicmp(cls, L"PNIHiddenWnd") == 0;
    bool isStobject = owner && _wcsicmp(owner, L"stobject.dll") == 0;
    if (!isPni && !isStobject) return TRUE;

    // With a non-null context this is a read-only lookup for an existing PNI
    // service window. Do not subclass windows on that path.
    if (param) {
        if (isPni) {
            *reinterpret_cast<HWND*>(param) = w;
            return FALSE;
        }
        return TRUE;
    }

    for (int i = 0; i < g_trayWndCount; i++)
        if (g_trayWnds[i] == w) return TRUE;
    if (g_trayWndCount >= (int)_countof(g_trayWnds)) return FALSE;

    g_trayWnds[g_trayWndCount++] = w;
    // Cross-thread subclass of a window owned by the shell's thread - must go through
    // WindhawkUtils, like every other subclass this mod installs on a shell window. Raw
    // SetWindowSubclass cannot safely subclass a window owned by another thread.
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(w, NetworkIconSubclassProc,
                                                       kNetworkIconSubclassId)) {
        Wh_Log(L"[tray] right-click supervision: could not subclass service window class %s (%s)",
               cls, owner ? owner : L"unknown module");
        g_trayWndCount--;
        return TRUE;
    }
    Wh_Log(L"[tray] right-click supervision: service window class %s (%s)",
           cls, owner ? owner : L"unknown module");
    if (g_trayWndCount >= (int)_countof(g_trayWnds)) return FALSE;
    return TRUE;
}

static HWND FindPniHiddenWindow() {
    HWND found = nullptr;
    EnumWindows(FindPniWindowProc, (LPARAM)&found);   // includes the invisible windows
    if (found) return found;

    // fallback: should it ever be created as a message-only window
    for (HWND w = FindWindowExW(HWND_MESSAGE, nullptr, nullptr, nullptr); w;
         w = FindWindowExW(HWND_MESSAGE, w, nullptr, nullptr)) {
        wchar_t cls[128] = {};
        if (!GetClassNameW(w, cls, _countof(cls))) continue;
        DWORD pid = 0;
        GetWindowThreadProcessId(w, &pid);
        if (pid != GetCurrentProcessId()) continue;
        if (_wcsicmp(cls, L"PNIHiddenWnd") == 0) return w;
    }
    return nullptr;
}

static void InstallNetworkIconMenu() {
    if (g_trayWndCount >= (int)_countof(g_trayWnds)) return;

    // pnidui: the network service window (hidden top-level).
    HWND pni = FindPniHiddenWindow();
    if (pni && !g_netIconWnd) {
        g_netIconWnd = pni;
        if (!g_netMenuLogged) {
            g_netMenuLogged = true;
            Wh_Log(L"[network] pnidui service window found (class PNIHiddenWnd among normal windows)");
        }
    }
    // All service windows (pnidui + stobject): the right-click log
    // says which one receives the click. No message is absorbed.
    EnumWindows(FindPniWindowProc, 0);
}

// Dynamic fallback is deliberately attached to the real pnidui callback window.
// It is never created unless pnidui.dll has attempted NIM_ADD and supplied a
// live PNIHiddenWnd + callback message. The click is sent to that same PNI
// HWND/ID/callback; this preserves PNI's handler rather than installing a mod
// WndProc or URI target. Whether that handler can display the Windows 10 flyout
// on the target build is a runtime question; the candidate fails closed if PNI
// tries to fall back to a network URI.
static const GUID kSystemNetworkIconGuid = {
    0x7820AE74, 0x23E3, 0x4229, { 0x82, 0xC1, 0xE4, 0x1C, 0xB6, 0x7D, 0x5B, 0x9C }
};

enum : WORD {
    kNetworkIconDisconnected = 3020,
    kNetworkIconWifiOnline0 = 3021,
    kNetworkIconWifiLimited0 = 3027,
    kNetworkIconWiredOnline = 3048,
    kNetworkIconWiredLimited = 3051,
};

struct NetworkPniRegistration {
    HWND hwnd;
    UINT id;
    UINT callbackMessage;
    UINT version;
    UINT originalFlags;
    GUID originalGuid;
    wchar_t tip[128];
    bool hasVersion;
    bool hasTip;
    bool hasGuid;
    bool valid;
    bool nativeFallbackRequired;
    bool forced;                 // registration made by this mod, not by pnidui
    LONG generation;
};

static bool g_traySupportInstalled = false;
static SRWLOCK g_networkPniLock = SRWLOCK_INIT;
static NetworkPniRegistration g_networkPniRegistration = {};
static NetworkPniRegistration g_networkFallbackOwner = {};
static std::atomic<bool> g_networkFallbackStopping{false};
static bool g_networkFallbackAdded = false;              // ExplorerServicesThread only
static HICON g_networkFallbackIcon = nullptr;             // ExplorerServicesThread only
static WORD g_networkFallbackResource = 0;
static LONG g_networkFallbackGeneration = 0;
static ULONGLONG g_networkFallbackNextTick = 0;
static unsigned g_networkFallbackAddFailures = 0;
static LONG g_networkFallbackLogBudget = 16;
static thread_local bool g_insideNetworkIconNotifyCall = false;

class ScopedNetworkPniReadLock {
public:
    ScopedNetworkPniReadLock() { AcquireSRWLockShared(&g_networkPniLock); }
    ~ScopedNetworkPniReadLock() { ReleaseSRWLockShared(&g_networkPniLock); }
    ScopedNetworkPniReadLock(const ScopedNetworkPniReadLock&) = delete;
    ScopedNetworkPniReadLock& operator=(const ScopedNetworkPniReadLock&) = delete;
};

class ScopedNetworkPniWriteLock {
public:
    ScopedNetworkPniWriteLock() { AcquireSRWLockExclusive(&g_networkPniLock); }
    ~ScopedNetworkPniWriteLock() { ReleaseSRWLockExclusive(&g_networkPniLock); }
    ScopedNetworkPniWriteLock(const ScopedNetworkPniWriteLock&) = delete;
    ScopedNetworkPniWriteLock& operator=(const ScopedNetworkPniWriteLock&) = delete;
};

class ScopedNetworkNotifyBypass {
public:
    ScopedNetworkNotifyBypass() : m_old(g_insideNetworkIconNotifyCall) {
        g_insideNetworkIconNotifyCall = true;
    }
    ~ScopedNetworkNotifyBypass() { g_insideNetworkIconNotifyCall = m_old; }
    ScopedNetworkNotifyBypass(const ScopedNetworkNotifyBypass&) = delete;
    ScopedNetworkNotifyBypass& operator=(const ScopedNetworkNotifyBypass&) = delete;
private:
    bool m_old;
};

class ScopedNetworkIcon {
public:
    explicit ScopedNetworkIcon(HICON icon = nullptr) : m_icon(icon) {}
    ~ScopedNetworkIcon() { reset(); }
    ScopedNetworkIcon(const ScopedNetworkIcon&) = delete;
    ScopedNetworkIcon& operator=(const ScopedNetworkIcon&) = delete;
    HICON get() const { return m_icon; }
    HICON release() noexcept { HICON icon = m_icon; m_icon = nullptr; return icon; }
    void reset(HICON icon = nullptr) noexcept {
        if (m_icon) DestroyIcon(m_icon);
        m_icon = icon;
    }
private:
    HICON m_icon;
};

class ScopedNetworkModule {
public:
    explicit ScopedNetworkModule(HMODULE module = nullptr) : m_module(module) {}
    ~ScopedNetworkModule() { if (m_module) FreeLibrary(m_module); }
    ScopedNetworkModule(const ScopedNetworkModule&) = delete;
    ScopedNetworkModule& operator=(const ScopedNetworkModule&) = delete;
    HMODULE get() const { return m_module; }
private:
    HMODULE m_module;
};

class ScopedNetworkComApartment {
public:
    ScopedNetworkComApartment() : m_hr(CoInitializeEx(nullptr, COINIT_MULTITHREADED)),
                                  m_uninitialize(SUCCEEDED(m_hr)) {}
    ~ScopedNetworkComApartment() { if (m_uninitialize) CoUninitialize(); }
    ScopedNetworkComApartment(const ScopedNetworkComApartment&) = delete;
    ScopedNetworkComApartment& operator=(const ScopedNetworkComApartment&) = delete;
    bool usable() const { return SUCCEEDED(m_hr) || m_hr == RPC_E_CHANGED_MODE; }
private:
    HRESULT m_hr;
    bool m_uninitialize;
};

class ScopedNetworkWlanClient {
public:
    explicit ScopedNetworkWlanClient(HANDLE handle = nullptr) : m_handle(handle) {}
    ~ScopedNetworkWlanClient() { if (m_handle) WlanCloseHandle(m_handle, nullptr); }
    ScopedNetworkWlanClient(const ScopedNetworkWlanClient&) = delete;
    ScopedNetworkWlanClient& operator=(const ScopedNetworkWlanClient&) = delete;
    HANDLE get() const { return m_handle; }
private:
    HANDLE m_handle;
};

class ScopedNetworkWlanMemory {
public:
    explicit ScopedNetworkWlanMemory(PVOID memory = nullptr) : m_memory(memory) {}
    ~ScopedNetworkWlanMemory() { if (m_memory) WlanFreeMemory(m_memory); }
    ScopedNetworkWlanMemory(const ScopedNetworkWlanMemory&) = delete;
    ScopedNetworkWlanMemory& operator=(const ScopedNetworkWlanMemory&) = delete;
    PVOID get() const { return m_memory; }
private:
    PVOID m_memory;
};

static bool IsPniduiServiceWindow(HWND hwnd) noexcept {
    if (!hwnd || !IsWindow(hwnd)) return false;
    try {
        wchar_t cls[64] = {};
        if (!GetClassNameW(hwnd, cls, _countof(cls)) || _wcsicmp(cls, L"PNIHiddenWnd") != 0)
            return false;
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid != GetCurrentProcessId()) return false;
        const HINSTANCE classModule = (HINSTANCE)GetClassLongPtrW(hwnd, GCLP_HMODULE);
        const HINSTANCE windowInstance = (HINSTANCE)GetWindowLongPtrW(hwnd, GWLP_HINSTANCE);
        const HMODULE pnidui = GetModuleHandleW(L"pnidui.dll");
        return pnidui && (classModule == pnidui || windowInstance == pnidui);
    } catch (...) {
        return false;
    }
}

static bool CopyNetworkPniRegistration(NetworkPniRegistration* out) noexcept {
    if (!out) return false;
    try {
        ScopedNetworkPniReadLock lock;
        *out = g_networkPniRegistration;
        return out->valid;
    } catch (...) {
        return false;
    }
}

static bool CopyNotifyIconTipBounded(const NOTIFYICONDATAW* data, wchar_t* out,
                              size_t outCount) noexcept {
    if (!data || !out || outCount == 0 || !(data->uFlags & NIF_TIP)) return false;
    out[0] = L'\0';
    const size_t tipOffset = FIELD_OFFSET(NOTIFYICONDATAW, szTip);
    if (data->cbSize <= tipOffset) return false;
    const size_t suppliedBytes = (size_t)data->cbSize - tipOffset;
    const size_t boundedBytes = (std::min)(suppliedBytes, sizeof(data->szTip));
    const size_t suppliedChars = boundedBytes / sizeof(wchar_t);
    if (!suppliedChars) return false;
    const size_t copyCount = (std::min)(suppliedChars - 1, outCount - 1);
    if (!copyCount) return true;
    return wcsncpy_s(out, outCount, data->szTip, copyCount) == 0;
}

static void RecordNetworkPniRegistration(DWORD message, PNOTIFYICONDATAW data,
                                         BOOL nativeResult) noexcept {
    try {
        if (!data) return;
        if (message == NIM_ADD && IsPniduiServiceWindow(data->hWnd)) {
            NetworkPniRegistration next = {};
            next.hwnd = data->hWnd;
            next.id = data->uID;
            next.callbackMessage = data->uCallbackMessage;
            next.originalFlags = data->uFlags;
            if (data->cbSize >= NOTIFYICONDATA_V3_SIZE && (data->uFlags & NIF_GUID)) {
                next.hasGuid = true;
                next.originalGuid = data->guidItem;
            }
            next.hasTip = CopyNotifyIconTipBounded(data, next.tip, _countof(next.tip));
            next.forced = false;   // the registration is pnidui's own
            const bool nativeIconPresent = nativeResult != FALSE &&
                (data->uFlags & NIF_ICON) != 0 && data->hIcon != nullptr;
            next.nativeFallbackRequired = !nativeIconPresent;
            next.valid = next.callbackMessage != 0 &&
                         (!next.hasGuid || IsEqualGUID(next.originalGuid, kSystemNetworkIconGuid));
            {
                ScopedNetworkPniWriteLock lock;
                next.generation = g_networkPniRegistration.generation + 1;
                g_networkPniRegistration = next;
            }
            if (next.hasGuid && !IsEqualGUID(next.originalGuid, kSystemNetworkIconGuid)) {
                wchar_t guid[64] = {};
                StringFromGUID2(next.originalGuid, guid, _countof(guid));
                Wh_Log(L"[tray] PNI NIM_ADD used unexpected GUID %s; fail-closed: no dynamic replacement", guid);
            } else if (!next.callbackMessage) {
                Wh_Log(L"[tray] PNI NIM_ADD did not provide a callback message; fail-closed: no dynamic replacement");
            } else {
                Wh_Log(L"[tray] native PNI NIM_ADD %s (hWnd 0x%p, id %u, callback 0x%X, system GUID %s)",
                       nativeResult ? L"accepted" : L"failed",
                       next.hwnd, next.id, next.callbackMessage,
                       next.nativeFallbackRequired ? L"dynamic NLM fallback pending" : L"native icon active");
            }
            return;
        }

        if (message == NIM_SETVERSION) {
            ScopedNetworkPniWriteLock lock;
            if (g_networkPniRegistration.valid && data->hWnd == g_networkPniRegistration.hwnd &&
                data->uID == g_networkPniRegistration.id) {
                g_networkPniRegistration.hasVersion = true;
                g_networkPniRegistration.version = data->uVersion;
                ++g_networkPniRegistration.generation;
            }
            return;
        }

        if (message == NIM_MODIFY) {
            ScopedNetworkPniWriteLock lock;
            if (g_networkPniRegistration.valid && data->hWnd == g_networkPniRegistration.hwnd &&
                data->uID == g_networkPniRegistration.id) {
                bool changed = false;
                if (data->uFlags & NIF_MESSAGE) {
                    g_networkPniRegistration.callbackMessage = data->uCallbackMessage;
                    if (!data->uCallbackMessage) g_networkPniRegistration.valid = false;
                    changed = true;
                }
                if (data->uFlags & NIF_ICON) {
                    g_networkPniRegistration.nativeFallbackRequired =
                        nativeResult == FALSE || data->hIcon == nullptr;
                    changed = true;
                }
                if (CopyNotifyIconTipBounded(data, g_networkPniRegistration.tip,
                                      _countof(g_networkPniRegistration.tip))) {
                    g_networkPniRegistration.hasTip = true;
                    changed = true;
                }
                if (changed) ++g_networkPniRegistration.generation;
            }
            return;
        }

        if (message == NIM_DELETE) {
            ScopedNetworkPniWriteLock lock;
            if (g_networkPniRegistration.valid && data->hWnd == g_networkPniRegistration.hwnd &&
                (data->uID == g_networkPniRegistration.id ||
                 ((data->uFlags & NIF_GUID) && data->cbSize >= NOTIFYICONDATA_V3_SIZE &&
                  IsEqualGUID(data->guidItem, kSystemNetworkIconGuid)))) {
                g_networkPniRegistration.valid = false;
                g_networkPniRegistration.nativeFallbackRequired = false;
                ++g_networkPniRegistration.generation;
                Wh_Log(L"[tray] native PNI NIM_DELETE observed; dynamic fallback will be withdrawn");
            }
        }
    } catch (...) {
        Wh_Log(L"[tray] exception recording the native PNI registration");
    }
}

static BOOL CallNetworkIconNotify(DWORD message, PNOTIFYICONDATAW data) noexcept {
    try {
        ScopedNetworkNotifyBypass bypass;
        return Shell_NotifyIconW(message, data);
    } catch (...) {
        Wh_Log(L"[tray] Shell_NotifyIconW raised a C++ exception in the dynamic fallback");
        return FALSE;
    }
}

struct NetworkAdapterInfo {
    GUID id;
    IFTYPE type;
    IF_INDEX index;
};

typedef ULONG (WINAPI* NetworkGetAdaptersAddresses_t)(ULONG, ULONG, PVOID,
                                                       PIP_ADAPTER_ADDRESSES, PULONG);
typedef DWORD (WINAPI* NetworkGetBestInterface_t)(ULONG, PDWORD);

static int ReadNetworkAdapters(NetworkAdapterInfo* out, int maxCount,
                               DWORD* bestIf) noexcept {
    if (!out || maxCount <= 0 || !bestIf) return 0;
    *bestIf = 0;
    try {
        ScopedNetworkModule iphlpapi(LoadLibraryExW(L"iphlpapi.dll", nullptr,
                                                    LOAD_LIBRARY_SEARCH_SYSTEM32));
        if (!iphlpapi.get()) return 0;
        auto getAdapters = (NetworkGetAdaptersAddresses_t)GetProcAddress(
            iphlpapi.get(), "GetAdaptersAddresses");
        auto getBestInterface = (NetworkGetBestInterface_t)GetProcAddress(
            iphlpapi.get(), "GetBestInterface");
        if (getBestInterface) {
            DWORD index = 0;
            if (getBestInterface(0x01010101, &index) == NO_ERROR) *bestIf = index;
        }
        if (!getAdapters) return 0;

        ULONG bytes = 16 * 1024;
        for (int attempt = 0; attempt < 3; ++attempt) {
            std::vector<BYTE> buffer(bytes);
            ULONG supplied = bytes;
            const ULONG rc = getAdapters(AF_UNSPEC,
                GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
                nullptr, (PIP_ADAPTER_ADDRESSES)buffer.data(), &supplied);
            if (rc == ERROR_BUFFER_OVERFLOW) {
                bytes = supplied;
                continue;
            }
            if (rc != NO_ERROR) return 0;

            int count = 0;
            for (PIP_ADAPTER_ADDRESSES adapter = (PIP_ADAPTER_ADDRESSES)buffer.data();
                 adapter && count < maxCount; adapter = adapter->Next) {
                if (!adapter->AdapterName) continue;
                wchar_t text[64] = {};
                if (!MultiByteToWideChar(CP_ACP, 0, adapter->AdapterName, -1,
                                         text, _countof(text))) continue;
                GUID id = {};
                if (FAILED(CLSIDFromString(text, &id))) continue;
                out[count].id = id;
                out[count].type = adapter->IfType;
                out[count].index = adapter->IfIndex ? adapter->IfIndex : adapter->Ipv6IfIndex;
                ++count;
            }
            return count;
        }
    } catch (...) {
        Wh_Log(L"[tray] adapter enumeration exception in the NLM fallback");
    }
    return 0;
}

static int QueryNetworkWifiBars(const GUID& adapterGuid) noexcept {
    try {
        HANDLE rawClient = nullptr;
        DWORD negotiated = 0;
        const DWORD openResult = WlanOpenHandle(2, nullptr, &negotiated, &rawClient);
        ScopedNetworkWlanClient client(rawClient);
        if (openResult != ERROR_SUCCESS || !client.get()) return -1;
        PWLAN_INTERFACE_INFO_LIST rawList = nullptr;
        const DWORD enumResult = WlanEnumInterfaces(client.get(), nullptr, &rawList);
        ScopedNetworkWlanMemory listOwner(rawList);
        if (enumResult != ERROR_SUCCESS || !rawList) return -1;
        const WLAN_INTERFACE_INFO_LIST* list = rawList;
        for (DWORD i = 0; i < list->dwNumberOfItems; ++i) {
            if (!IsEqualGUID(list->InterfaceInfo[i].InterfaceGuid, adapterGuid)) continue;
            PVOID rawData = nullptr;
            DWORD bytes = 0;
            const DWORD rssiResult = WlanQueryInterface(client.get(), &adapterGuid,
                wlan_intf_opcode_rssi, nullptr, &bytes, &rawData, nullptr);
            ScopedNetworkWlanMemory rssiOwner(rawData);
            if (rssiResult == ERROR_SUCCESS && rawData && bytes >= sizeof(LONG)) {
                const LONG rssi = *(const LONG*)rawData;
                return rssi >= -55 ? 4 : rssi >= -67 ? 3 : rssi >= -75 ? 2 : rssi >= -85 ? 1 : 0;
            }

            PVOID rawConnection = nullptr;
            bytes = 0;
            const DWORD connectionResult = WlanQueryInterface(client.get(), &adapterGuid,
                wlan_intf_opcode_current_connection, nullptr, &bytes, &rawConnection, nullptr);
            ScopedNetworkWlanMemory connectionOwner(rawConnection);
            if (connectionResult == ERROR_SUCCESS && rawConnection &&
                bytes >= sizeof(WLAN_CONNECTION_ATTRIBUTES)) {
                const WLAN_CONNECTION_ATTRIBUTES* connection =
                    (const WLAN_CONNECTION_ATTRIBUTES*)rawConnection;
                const int quality = (int)connection->wlanAssociationAttributes.wlanSignalQuality;
                return quality >= 80 ? 4 : quality >= 60 ? 3 : quality >= 40 ? 2 : quality >= 20 ? 1 : 0;
            }
            return -1;
        }
    } catch (...) {
        Wh_Log(L"[tray] WLAN signal query exception in the NLM fallback");
    }
    return -1;
}

static const CLSID kClsidNetworkListManager = {
    0xDCB00C01, 0x570F, 0x4A9B, { 0x8D, 0x69, 0x19, 0x9F, 0xDB, 0xA5, 0x72, 0x3B }
};
static const IID kIidNetworkListManager = {
    0xDCB00000, 0x570F, 0x4A9B, { 0x8D, 0x69, 0x19, 0x9F, 0xDB, 0xA5, 0x72, 0x3B }
};

static bool QueryNetworkIconResource(WORD* resourceId) noexcept {
    if (!resourceId) return false;
    *resourceId = kNetworkIconDisconnected;
    try {
        ScopedNetworkComApartment apartment;
        if (!apartment.usable()) return false;

        NetworkAdapterInfo adapters[32] = {};
        DWORD bestIf = 0;
        const int adapterCount = ReadNetworkAdapters(adapters, _countof(adapters), &bestIf);

        winrt::com_ptr<INetworkListManager> manager;
        HRESULT hr = CoCreateInstance(kClsidNetworkListManager, nullptr, CLSCTX_ALL,
                                      kIidNetworkListManager, manager.put_void());
        if (FAILED(hr) || !manager) return false;
        winrt::com_ptr<IEnumNetworkConnections> connections;
        hr = manager->GetNetworkConnections(connections.put());
        if (FAILED(hr) || !connections) return false;

        bool found = false;
        bool chosenWifi = false;
        bool chosenInternet = false;
        GUID chosenAdapter = {};
        int bestScore = -1;
        const DWORD internetMask = (DWORD)NLM_CONNECTIVITY_IPV4_INTERNET |
                                   (DWORD)NLM_CONNECTIVITY_IPV6_INTERNET;
        for (;;) {
            winrt::com_ptr<INetworkConnection> connection;
            ULONG fetched = 0;
            const HRESULT next = connections->Next(1, connection.put(), &fetched);
            if (next != S_OK || fetched != 1 || !connection) break;

            VARIANT_BOOL isConnected = VARIANT_FALSE;
            if (FAILED(connection->get_IsConnected(&isConnected)) || isConnected != VARIANT_TRUE)
                continue;
            GUID adapterGuid = {};
            if (FAILED(connection->GetAdapterId(&adapterGuid))) continue;
            NLM_CONNECTIVITY connectivity = NLM_CONNECTIVITY_DISCONNECTED;
            connection->GetConnectivity(&connectivity);

            IFTYPE adapterType = 0;
            IF_INDEX adapterIndex = 0;
            for (int i = 0; i < adapterCount; ++i) {
                if (IsEqualGUID(adapters[i].id, adapterGuid)) {
                    adapterType = adapters[i].type;
                    adapterIndex = adapters[i].index;
                    break;
                }
            }
            if (adapterType == IF_TYPE_SOFTWARE_LOOPBACK || adapterType == IF_TYPE_TUNNEL)
                continue;

            const bool internet = (((DWORD)connectivity & internetMask) != 0);
            const int score = (adapterIndex && adapterIndex == bestIf ? 4 : 0) +
                              (internet ? 2 : 0) + 1;
            if (score <= bestScore) continue;
            bestScore = score;
            found = true;
            chosenWifi = adapterType == IF_TYPE_IEEE80211;
            chosenInternet = internet;
            chosenAdapter = adapterGuid;
        }

        if (!found) {
            *resourceId = kNetworkIconDisconnected;
            return true;
        }
        if (!chosenWifi) {
            *resourceId = chosenInternet ? kNetworkIconWiredOnline : kNetworkIconWiredLimited;
            return true;
        }
        int bars = QueryNetworkWifiBars(chosenAdapter);
        if (bars < 0) bars = 4;  // NLM state is authoritative; signal can be unavailable by policy.
        if (bars > 4) bars = 4;
        *resourceId = (WORD)((chosenInternet ? kNetworkIconWifiOnline0 :
                              kNetworkIconWifiLimited0) + bars);
        return true;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"[tray] NLM network-state query failed (0x%08X)", (unsigned)ex.code());
    } catch (...) {
        Wh_Log(L"[tray] NLM network-state query raised a C++ exception");
    }
    return false;
}

static void BuildNetworkFallbackNid(const NetworkPniRegistration& reg, HICON icon,
                                    NOTIFYICONDATAW* nid) noexcept {
    if (!nid) return;
    memset(nid, 0, sizeof(*nid));
    nid->cbSize = sizeof(*nid);
    nid->hWnd = reg.hwnd;
    nid->uID = reg.id;
    nid->uFlags = NIF_ICON | NIF_MESSAGE | NIF_GUID;
    nid->guidItem = kSystemNetworkIconGuid;
    nid->uCallbackMessage = reg.callbackMessage;
    nid->hIcon = icon;
    if (reg.hasTip) {
        nid->uFlags |= NIF_TIP;
        wcsncpy_s(nid->szTip, _countof(nid->szTip), reg.tip, _TRUNCATE);
    }
}

static bool DeleteNetworkFallbackIcon(const NetworkPniRegistration& reg) noexcept {
    try {
        NOTIFYICONDATAW nid = {};
        nid.cbSize = sizeof(nid);
        nid.hWnd = reg.hwnd;
        nid.uID = reg.id;
        nid.uFlags = NIF_GUID;
        nid.guidItem = kSystemNetworkIconGuid;
        return CallNetworkIconNotify(NIM_DELETE, &nid) != FALSE;
    } catch (...) {
        Wh_Log(L"[tray] exception deleting the dynamic PNI fallback icon");
        return false;
    }
}

static void ReleaseNetworkFallbackIconState() noexcept {
    g_networkFallbackAdded = false;
    g_networkFallbackResource = 0;
    g_networkFallbackGeneration = 0;
    g_networkFallbackNextTick = 0;
    g_networkFallbackOwner = {};
    if (g_networkFallbackIcon) {
        DestroyIcon(g_networkFallbackIcon);
        g_networkFallbackIcon = nullptr;
    }
}

// Declared early, defined further down (after IsOurTaskbarUp). The existing path needs it to
// recognize the owner window of the forced icon.
namespace NetworkTrayForce {
static bool IsForcedOwnerWindow(HWND hwnd) noexcept;
}

static void NetworkIconFallbackTick() noexcept {
    try {
        if (!g_traySupportInstalled || g_unloading.load() ||
            g_networkFallbackStopping.load()) return;

        NetworkPniRegistration reg = {};
        const bool haveRegistration = CopyNetworkPniRegistration(&reg);
        if (!g_cfg.provideTrayDlls) {
            if (!g_networkFallbackAdded) return;
            const bool preserveNativePniItem = haveRegistration && reg.valid &&
                !reg.nativeFallbackRequired && reg.hasGuid &&
                IsEqualGUID(reg.originalGuid, kSystemNetworkIconGuid) &&
                reg.hwnd == g_networkFallbackOwner.hwnd && reg.id == g_networkFallbackOwner.id;
            if (preserveNativePniItem) {
                ReleaseNetworkFallbackIconState();
                Wh_Log(L"[tray] dynamic fallback disabled; preserving the native PNI icon");
                return;
            }
            const ULONGLONG now = GetTickCount64();
            if (now < g_networkFallbackNextTick) return;
            g_networkFallbackNextTick = now + 5000;
            if (!DeleteNetworkFallbackIcon(g_networkFallbackOwner)) {
                Wh_Log(L"[tray] dynamic PNI fallback NIM_DELETE failed after disabling provideTrayDlls; retry in 5 s");
                return;
            }
            ReleaseNetworkFallbackIconState();
            Wh_Log(L"[tray] dynamic fallback removed after disabling provideTrayDlls");
            return;
        }
        if (!haveRegistration || !reg.valid) {
            if (g_networkFallbackAdded) {
                const ULONGLONG now = GetTickCount64();
                if (now < g_networkFallbackNextTick) return;
                g_networkFallbackNextTick = now + 5000;
                if (!DeleteNetworkFallbackIcon(g_networkFallbackOwner)) {
                    Wh_Log(L"[tray] dynamic PNI fallback NIM_DELETE failed; retry in 5 s");
                    return;
                }
                g_networkFallbackAdded = false;
                g_networkFallbackResource = 0;
                g_networkFallbackGeneration = 0;
                if (g_networkFallbackIcon) {
                    DestroyIcon(g_networkFallbackIcon);
                    g_networkFallbackIcon = nullptr;
                }
                Wh_Log(L"[tray] dynamic PNI fallback removed because its native callback registration ended");
            }
            return;
        }
        if (!reg.nativeFallbackRequired) {
            // A later native NIM_ADD may have replaced our fallback. Preserve that
            // native item only when the owner identity is the same; otherwise retire
            // our old registration so it cannot remain orphaned in the tray.
            if (g_networkFallbackAdded) {
                const bool sameIdentity = g_networkFallbackOwner.hwnd == reg.hwnd &&
                    g_networkFallbackOwner.id == reg.id && reg.hasGuid &&
                    IsEqualGUID(reg.originalGuid, kSystemNetworkIconGuid);
                if (!sameIdentity) {
                    const ULONGLONG now = GetTickCount64();
                    if (now < g_networkFallbackNextTick) return;
                    g_networkFallbackNextTick = now + 5000;
                    if (!DeleteNetworkFallbackIcon(g_networkFallbackOwner)) {
                        g_networkFallbackGeneration = reg.generation;
                        Wh_Log(L"[tray] old dynamic PNI fallback NIM_DELETE failed; retry in 5 s");
                        return;
                    }
                }
                g_networkFallbackAdded = false;
                g_networkFallbackResource = 0;
                g_networkFallbackGeneration = 0;
                if (g_networkFallbackIcon) {
                    DestroyIcon(g_networkFallbackIcon);
                    g_networkFallbackIcon = nullptr;
                }
                Wh_Log(L"[tray] native PNI icon is active; dynamic fallback retired");
            }
            return;
        }
        if ((!IsPniduiServiceWindow(reg.hwnd) && !NetworkTrayForce::IsForcedOwnerWindow(reg.hwnd)) ||
            reg.callbackMessage == 0) {
            g_networkFallbackGeneration = reg.generation;
            return;
        }

        const ULONGLONG now = GetTickCount64();
        if (now < g_networkFallbackNextTick &&
            g_networkFallbackGeneration == reg.generation) return;
        g_networkFallbackNextTick = now + 5000;

        WORD resourceId = 0;
        if (!QueryNetworkIconResource(&resourceId)) {
            if (!reg.forced) {
                g_networkFallbackGeneration = reg.generation;
                if (InterlockedDecrement(&g_networkFallbackLogBudget) >= 0)
                    Wh_Log(L"[tray] dynamic fallback waiting for Network List Manager; no URI or substitute click target is installed");
                return;
            }
            // Forced icon: Network List Manager is not answering, but the icon has to
            // appear all the same. The authentic "wired" icon of the verified
            // pnidui.dll is used; as soon as NLM answers again the next tick updates it
            // with the real state.
            resourceId = kNetworkIconWiredOnline;
            if (InterlockedDecrement(&g_networkFallbackLogBudget) >= 0)
                Wh_Log(L"[tray-force] Network List Manager unavailable: authentic icon "
                       L"3048 used so that the icon appears anyway");
        }
        const bool sameIdentity = g_networkFallbackAdded &&
            g_networkFallbackOwner.hwnd == reg.hwnd &&
            g_networkFallbackOwner.id == reg.id;
        if (g_networkFallbackAdded && !sameIdentity) {
            if (!DeleteNetworkFallbackIcon(g_networkFallbackOwner)) {
                g_networkFallbackGeneration = reg.generation;
                Wh_Log(L"[tray] old dynamic PNI fallback NIM_DELETE failed; retry in 5 s");
                return;
            }
            g_networkFallbackAdded = false;
            if (g_networkFallbackIcon) {
                DestroyIcon(g_networkFallbackIcon);
                g_networkFallbackIcon = nullptr;
            }
        }
        if (sameIdentity && resourceId == g_networkFallbackResource &&
            g_networkFallbackGeneration == reg.generation) return;

        HMODULE pnidui = GetModuleHandleW(L"pnidui.dll");
        const int cx = GetSystemMetrics(SM_CXSMICON) > 0 ? GetSystemMetrics(SM_CXSMICON) : 16;
        const int cy = GetSystemMetrics(SM_CYSMICON) > 0 ? GetSystemMetrics(SM_CYSMICON) : 16;
        ScopedNetworkIcon icon(pnidui ? (HICON)LoadImageW(pnidui, MAKEINTRESOURCEW(resourceId),
                                                          IMAGE_ICON, cx, cy, 0)
                                      : nullptr);
        if (!icon.get()) {
            // If pnidui.dll is not mapped in this process, the authentic icon is taken
            // from the verified file in the data folder instead. This is what makes the
            // icon appear even when the native SSO never started.
            wchar_t modulePath[MAX_PATH] = {};
            _snwprintf_s(modulePath, _countof(modulePath), _TRUNCATE, L"%s\\pnidui.dll",
                         g_cfg.storePath);
            icon.reset((HICON)LoadImageW(nullptr, modulePath, IMAGE_ICON, cx, cy,
                                         LR_LOADFROMFILE));
            if (icon.get()) {
                static bool loggedFromFile = false;
                if (!loggedFromFile) {
                    loggedFromFile = true;
                    Wh_Log(L"[tray-force] authentic icon %u loaded from the file %s "
                           L"(pnidui.dll is not mapped in this process)", resourceId, modulePath);
                }
            }
        }
        if (!icon.get()) {
            g_networkFallbackGeneration = reg.generation;
            if (InterlockedDecrement(&g_networkFallbackLogBudget) >= 0)
                Wh_Log(L"[tray] authentic pnidui icon resource %u could not be loaded", resourceId);
            return;
        }

        NOTIFYICONDATAW nid = {};
        BuildNetworkFallbackNid(reg, icon.get(), &nid);
        BOOL ok = FALSE;
        if (g_networkFallbackAdded) {
            ok = CallNetworkIconNotify(NIM_MODIFY, &nid);
            if (!ok) ok = CallNetworkIconNotify(NIM_ADD, &nid);
        } else {
            ok = CallNetworkIconNotify(NIM_ADD, &nid);
        }
        if (!ok) {
            ++g_networkFallbackAddFailures;
            // Keep the last registered icon and HICON owned until a delete or
            // replacement succeeds; a zero resource forces a retry if it is active.
            if (g_networkFallbackAdded) g_networkFallbackResource = 0;
            g_networkFallbackGeneration = reg.generation;
            if (g_networkFallbackAddFailures <= 6 || g_networkFallbackAddFailures % 6 == 0)
                Wh_Log(L"[tray] dynamic PNI/NLM NIM_ADD/MODIFY failed (%u), identity hWnd 0x%p id %u; retry in 5 s",
                       g_networkFallbackAddFailures, reg.hwnd, reg.id);
            return;
        }

        if (reg.hasVersion) {
            NOTIFYICONDATAW versionData = {};
            versionData.cbSize = sizeof(versionData);
            versionData.hWnd = reg.hwnd;
            versionData.uID = reg.id;
            versionData.uFlags = NIF_GUID;
            versionData.guidItem = kSystemNetworkIconGuid;
            versionData.uVersion = reg.version;
            CallNetworkIconNotify(NIM_SETVERSION, &versionData);
        }

        HICON oldIcon = g_networkFallbackIcon;
        g_networkFallbackIcon = icon.release();
        g_networkFallbackOwner = reg;
        g_networkFallbackAdded = true;
        g_networkFallbackResource = resourceId;
        g_networkFallbackGeneration = reg.generation;
        g_networkFallbackAddFailures = 0;
        if (oldIcon) DestroyIcon(oldIcon);
        Wh_Log(L"[tray] dynamic NLM fallback active: resource %u, system network GUID, native PNI callback 0x%X",
               resourceId, reg.callbackMessage);
    } catch (...) {
        Wh_Log(L"[tray] dynamic network-icon fallback exception");
    }
}

static void NetworkIconFallbackShutdown() noexcept {
    try {
        g_networkFallbackStopping.store(true);
        const bool hadResources = g_networkFallbackAdded || g_networkFallbackIcon != nullptr;
        NetworkPniRegistration current = {};
        const bool preserveNativePniItem =
            CopyNetworkPniRegistration(&current) && current.valid && !current.nativeFallbackRequired &&
            current.hasGuid && IsEqualGUID(current.originalGuid, kSystemNetworkIconGuid) &&
            current.hwnd == g_networkFallbackOwner.hwnd && current.id == g_networkFallbackOwner.id;
        if (g_networkFallbackAdded) {
            if (!preserveNativePniItem) {
                bool deleted = false;
                for (int attempt = 0; attempt < 3 && !deleted; ++attempt)
                    deleted = DeleteNetworkFallbackIcon(g_networkFallbackOwner);
                if (!deleted)
                    Wh_Log(L"[tray] dynamic PNI fallback NIM_DELETE failed during shutdown after 3 attempts");
            }
            g_networkFallbackAdded = false;
        }
        if (g_networkFallbackIcon) {
            DestroyIcon(g_networkFallbackIcon);
            g_networkFallbackIcon = nullptr;
        }
        g_networkFallbackResource = 0;
        g_networkFallbackGeneration = 0;
        if (hadResources) Wh_Log(L"[tray] dynamic PNI fallback cleaned up");
    } catch (...) {
        Wh_Log(L"[tray] dynamic network-icon fallback cleanup exception");
    }
}

class ScopedNetworkFallbackThreadCleanup {
public:
    ~ScopedNetworkFallbackThreadCleanup() { NetworkIconFallbackShutdown(); }
    ScopedNetworkFallbackThreadCleanup(const ScopedNetworkFallbackThreadCleanup&) = delete;
    ScopedNetworkFallbackThreadCleanup& operator=(const ScopedNetworkFallbackThreadCleanup&) = delete;
    ScopedNetworkFallbackThreadCleanup() = default;
};

static BOOL WINAPI Shell_NotifyIconW_Hook(DWORD message, PNOTIFYICONDATAW data) {
    if (g_insideNetworkIconNotifyCall && Shell_NotifyIconW_Original)
        return Shell_NotifyIconW_Original(message, data);
    if (g_unloading.load() && Shell_NotifyIconW_Original) {
        const BOOL result = Shell_NotifyIconW_Original(message, data);
        RecordNetworkPniRegistration(message, data, result);
        return result;
    }
    try {
        if (data && data->hWnd) {
            wchar_t cls[64] = {};
            if (GetClassNameW(data->hWnd, cls, _countof(cls)) && _wcsicmp(cls, L"PNIHiddenWnd") == 0) {
                // It is pnidui's window: the right click goes through here too.
                g_netIconWnd = data->hWnd;
                if (data->uCallbackMessage) g_netIconMsg = data->uCallbackMessage;
                InstallNetworkIconMenu();
            }
        }
        // One line per icon: the Action Center icon re-registers every
        // 15 seconds and on its own filled the log (user report).
        bool newIcon = false;
        if (data && message == NIM_ADD) {
            newIcon = true;
            for (int i = 0; i < g_trayIconSeenCount; i++)
                if (g_trayIconSeen[i].wnd == data->hWnd && g_trayIconSeen[i].id == data->uID) {
                    newIcon = false;
                    break;
                }
            if (newIcon && g_trayIconSeenCount < (int)_countof(g_trayIconSeen)) {
                g_trayIconSeen[g_trayIconSeenCount].wnd = data->hWnd;
                g_trayIconSeen[g_trayIconSeenCount].id = data->uID;
                g_trayIconSeenCount++;
            }
        }
        if (data && message == NIM_ADD && newIcon && g_netIconLogs < 24) {
            g_netIconLogs++;
            wchar_t cls[64] = {};
            if (data->hWnd) GetClassNameW(data->hWnd, cls, _countof(cls));
            wchar_t guid[64] = {};
            if ((data->uFlags & NIF_GUID) && data->cbSize >= NOTIFYICONDATA_V3_SIZE)
                StringFromGUID2(data->guidItem, guid, _countof(guid));
            wchar_t boundedTip[128] = {};
            const wchar_t* tip = L"(no text)";
            if (data->uFlags & NIF_TIP)
                tip = CopyNotifyIconTipBounded(data, boundedTip, _countof(boundedTip))
                    ? boundedTip : L"(tip unavailable)";
            RememberTrayOwner(data->hWnd, data->uID, tip);
            Wh_Log(L"[network] tray icon: window %s, id %u, message 0x%X, guid %s, text \"%s\", battery %s",
                   cls, data->uID, (unsigned)data->uCallbackMessage, guid, tip,
                   IsBatteryServiceWindow(data->hWnd) ? L"yes" : L"no");
        }
        if (data && message == NIM_MODIFY && data->hWnd && (data->uFlags & NIF_TIP)) {
            wchar_t boundedTip[128] = {};
            if (CopyNotifyIconTipBounded(data, boundedTip, _countof(boundedTip)))
                RememberTrayOwner(data->hWnd, data->uID, boundedTip);
        }
        // The battery registers itself with an empty text, so the GUID is the only thing
        // that names it; and that does not depend on the log budget above.
        if (data && (message == NIM_ADD || message == NIM_MODIFY)) RememberBatteryTrayIcon(data);
    } catch (...) {
        Wh_Log(L"[network] exception in the icon supervision");
    }
    const BOOL result = Shell_NotifyIconW_Original(message, data);
    RecordNetworkPniRegistration(message, data, result);
    return result;
}

static void InstallNetworkIconTrace() {
    if (Wh_SetFunctionHook((void*)Shell_NotifyIconW, (void*)Shell_NotifyIconW_Hook,
                           (void**)&Shell_NotifyIconW_Original))
        Wh_Log(L"[network] tray icon registration tracking active");
    else
        Wh_Log(L"[network] tray icon registration tracking not installed");
}

// --- tray module strings: the MUI that is not in the data folder --------------
// evidence (stobject.dll 10.0.19041.7664, disassembled): stobject does NOT have
// the strings inside itself (file resources: MUI, VERSION, MANIFEST) and takes
// EVERY text with LoadStringW(hInstance, id, ...) from its MUI,
// "stobject.dll.mui": that is where the entries of the battery right-click
// menu come from (ids 166, 167: "Power options", "Windows Mobility Center",
// ...) and the flyout texts (ids 206-214). In the data folder the .mui
// is not there (Microsoft's symbol server does not publish .mui files: msdl answers
// 404, as seen for pnidui) and LoadStringW returns 0: the entries stay
// without text and the menu does not draw. pnidui has the same problem and that is
// why resource 3014 has to be served by hand.
// The same is done here for the strings: they are the AUTHENTIC ones
// of the Italian Windows 10 MUI (10.0.19041.1, taken from the Microsoft
// language pack; for pnidui: 10 strings, for stobject: 57), served ONLY if
// Windows does not find its own: if the .mui is there, nothing is touched.
// One entry of the tray tables below: the text in every language of UiLangId, in
// the same order (IT, EN, FR, ES, DE, PT, NL, RU, JA, PL, DA, SV, NO, FI, TR). The
// text is served only when Windows does not find its own (see LoadStringW_Hook):
// first the Windows 10 file of the store, then this table in the language of the
// interface, then English.
struct TrayString {
    UINT id;
    const wchar_t* text[LANG_COUNT];
};

static const TrayString kStobjectStrings[] = {
    { 144,
      {
        L"Avvisa quando è necessario sostituire la batteria",   // IT
        L"Notify me when the battery needs to be replaced",   // EN
        L"M'avertir quand la batterie doit être remplacée",   // FR
        L"Notificarme cuando sea necesario reemplazar la batería",   // ES
        L"Benachrichtigen, wenn der Akku ausgetauscht werden muss",   // DE
        L"Avisar quando for necessário substituir a bateria",   // PT
        L"Mij waarschuwen wanneer de batterij moet worden vervangen",   // NL
        L"Уведомлять о необходимости замены батареи",   // RU
        L"バッテリの交換が必要なときに通知する",   // JA
        L"Powiadom, gdy bateria wymaga wymiany",   // PL
        L"Giv mig besked, når batteriet skal skiftes",   // DA
        L"Meddela när batteriet behöver bytas",   // SV
        L"Varsle når batteriet må skiftes",   // NO
        L"Ilmoita, kun akku on vaihdettava",   // FI
        L"Pilin değiştirilmesi gerektiğinde bildir"   // TR
      } },
    { 150,
      {
        L"Opzioni risparmio energia",   // IT
        L"Power options",   // EN
        L"Options d'alimentation",   // FR
        L"Opciones de energía",   // ES
        L"Energieoptionen",   // DE
        L"Opções de energia",   // PT
        L"Energiebeheer",   // NL
        L"Электропитание",   // RU
        L"電源オプション",   // JA
        L"Opcje zasilania",   // PL
        L"Strømindstillinger",   // DA
        L"Energialternativ",   // SV
        L"Alternativer for strøm",   // NO
        L"Virta-asetukset",   // FI
        L"Güç seçenekleri"   // TR
      } },
    { 151,
      {
        L"Centro PC portatile Windows",   // IT
        L"Windows Mobility Center",   // EN
        L"Centre de mobilité Windows",   // FR
        L"Centro de movilidad de Windows",   // ES
        L"Windows Mobilitätscenter",   // DE
        L"Centro de Mobilidade do Windows",   // PT
        L"Windows Mobiliteitscentrum",   // NL
        L"Центр мобильности Windows",   // RU
        L"Windows モビリティ センター",   // JA
        L"Centrum mobilności systemu Windows",   // PL
        L"Windows Mobilitetscenter",   // DA
        L"Windows Mobility Center",   // SV
        L"Windows Mobility Center",   // NO
        L"Windows Mobility Center",   // FI
        L"Windows Mobility Center"   // TR
      } },
    { 166,
      {
        L"Opzioni risparmio energia",   // IT
        L"Power options",   // EN
        L"Options d'alimentation",   // FR
        L"Opciones de energía",   // ES
        L"Energieoptionen",   // DE
        L"Opções de energia",   // PT
        L"Energiebeheer",   // NL
        L"Электропитание",   // RU
        L"電源オプション",   // JA
        L"Opcje zasilania",   // PL
        L"Strømindstillinger",   // DA
        L"Energialternativ",   // SV
        L"Alternativer for strøm",   // NO
        L"Virta-asetukset",   // FI
        L"Güç seçenekleri"   // TR
      } },
    { 167,
      {
        L"Centro PC portatile Windows",   // IT
        L"Windows Mobility Center",   // EN
        L"Centre de mobilité Windows",   // FR
        L"Centro de movilidad de Windows",   // ES
        L"Windows Mobilitätscenter",   // DE
        L"Centro de Mobilidade do Windows",   // PT
        L"Windows Mobiliteitscentrum",   // NL
        L"Центр мобильности Windows",   // RU
        L"Windows モビリティ センター",   // JA
        L"Centrum mobilności systemu Windows",   // PL
        L"Windows Mobilitetscenter",   // DA
        L"Windows Mobility Center",   // SV
        L"Windows Mobility Center",   // NO
        L"Windows Mobility Center",   // FI
        L"Windows Mobility Center"   // TR
      } },
    { 181,
      {
        L"Misuratore alimentazione",   // IT
        L"Power meter",   // EN
        L"Compteur d'alimentation",   // FR
        L"Medidor de energía",   // ES
        L"Energieanzeige",   // DE
        L"Medidor de energia",   // PT
        L"Energiemeter",   // NL
        L"Индикатор питания",   // RU
        L"電源メーター",   // JA
        L"Miernik zasilania",   // PL
        L"Strømmåler",   // DA
        L"Energimätare",   // SV
        L"Strømmåler",   // NO
        L"Virran mittari",   // FI
        L"Güç ölçer"   // TR
      } },
    { 186,
      {
        L"Combinazione per il risparmio di energia corrente: ",   // IT
        L"Current power plan: ",   // EN
        L"Mode de gestion de l'alimentation actuel : ",   // FR
        L"Plan de energía actual: ",   // ES
        L"Aktueller Energiesparplan: ",   // DE
        L"Plano de energia atual: ",   // PT
        L"Huidig energieplan: ",   // NL
        L"Текущий план электропитания: ",   // RU
        L"現在の電源プラン: ",   // JA
        L"Bieżący plan zasilania: ",   // PL
        L"Aktuel strømfunktion: ",   // DA
        L"Aktuellt energischema: ",   // SV
        L"Gjeldende strømplan: ",   // NO
        L"Nykyinen virransäästösuunnitelma: ",   // FI
        L"Geçerli güç planı: "   // TR
      } },
    { 187,
      {
        L"Sconosciuto",   // IT
        L"Unknown",   // EN
        L"Inconnu",   // FR
        L"Desconocido",   // ES
        L"Unbekannt",   // DE
        L"Desconhecido",   // PT
        L"Onbekend",   // NL
        L"Неизвестно",   // RU
        L"不明",   // JA
        L"Nieznany",   // PL
        L"Ukendt",   // DA
        L"Okänd",   // SV
        L"Ukjent",   // NO
        L"Tuntematon",   // FI
        L"Bilinmiyor"   // TR
      } },
    { 188,
      {
        L"La combinazione per il risparmio di energia corrente potrebbe ridurre le prestazioni del sistema.",   // IT
        L"The current power plan may reduce system performance.",   // EN
        L"Le mode de gestion de l'alimentation actuel peut réduire les performances du système.",   // FR
        L"El plan de energía actual puede reducir el rendimiento del sistema.",   // ES
        L"Der aktuelle Energiesparplan kann die Systemleistung verringern.",   // DE
        L"O plano de energia atual pode reduzir o desempenho do sistema.",   // PT
        L"Het huidige energieplan kan de systeemprestaties verminderen.",   // NL
        L"Текущий план электропитания может снизить производительность системы.",   // RU
        L"現在の電源プランではシステムのパフォーマンスが低下する可能性があります。",   // JA
        L"Bieżący plan zasilania może obniżyć wydajność systemu.",   // PL
        L"Den aktuelle strømfunktion kan nedsætte systemets ydeevne.",   // DA
        L"Det aktuella energischemat kan minska systemprestandan.",   // SV
        L"Gjeldende strømplan kan redusere systemytelsen.",   // NO
        L"Nykyinen virransäästösuunnitelma voi heikentää järjestelmän suorituskykyä.",   // FI
        L"Geçerli güç planı sistem performansını düşürebilir."   // TR
      } },
    { 189,
      {
        L"La combinazione per il risparmio di energia corrente potrebbe ridurre la durata della batteria.",   // IT
        L"The current power plan may reduce battery life.",   // EN
        L"Le mode de gestion de l'alimentation actuel peut réduire l'autonomie de la batterie.",   // FR
        L"El plan de energía actual puede reducir la duración de la batería.",   // ES
        L"Der aktuelle Energiesparplan kann die Akkulaufzeit verkürzen.",   // DE
        L"O plano de energia atual pode reduzir a duração da bateria.",   // PT
        L"Het huidige energieplan kan de batterijduur verkorten.",   // NL
        L"Текущий план электропитания может сократить время работы батареи.",   // RU
        L"現在の電源プランではバッテリの駆動時間が短くなる可能性があります。",   // JA
        L"Bieżący plan zasilania może skrócić czas pracy baterii.",   // PL
        L"Den aktuelle strømfunktion kan nedsætte batterilevetiden.",   // DA
        L"Det aktuella energischemat kan minska batteritiden.",   // SV
        L"Gjeldende strømplan kan redusere batterilevetiden.",   // NO
        L"Nykyinen virransäästösuunnitelma voi lyhentää akun kestoa.",   // FI
        L"Geçerli güç planı pil ömrünü kısaltabilir."   // TR
      } },
    { 190,
      {
        L"L'impostazione della luminosità corrente potrebbe ridurre la durata della batteria.",   // IT
        L"The current brightness setting may reduce battery life.",   // EN
        L"Le réglage de luminosité actuel peut réduire l'autonomie de la batterie.",   // FR
        L"La configuración de brillo actual puede reducir la duración de la batería.",   // ES
        L"Die aktuelle Helligkeitseinstellung kann die Akkulaufzeit verkürzen.",   // DE
        L"A definição de luminosidade atual pode reduzir a duração da bateria.",   // PT
        L"De huidige helderheidsinstelling kan de batterijduur verkorten.",   // NL
        L"Текущая настройка яркости может сократить время работы батареи.",   // RU
        L"現在の明るさの設定ではバッテリの駆動時間が短くなる可能性があります。",   // JA
        L"Bieżące ustawienie jasności może skrócić czas pracy baterii.",   // PL
        L"Den aktuelle indstilling for lysstyrke kan nedsætte batterilevetiden.",   // DA
        L"Den aktuella ljusstyrkeinställningen kan minska batteritiden.",   // SV
        L"Gjeldende innstilling for lysstyrke kan redusere batterilevetiden.",   // NO
        L"Nykyinen kirkkausasetus voi lyhentää akun kestoa.",   // FI
        L"Geçerli parlaklık ayarı pil ömrünü kısaltabilir."   // TR
      } },
    { 191,
      {
        L"Problema con la batteria. Il computer potrebbe spegnersi all'improvviso.",   // IT
        L"There is a problem with the battery. The computer may shut down suddenly.",   // EN
        L"Problème avec la batterie. L'ordinateur peut s'arrêter brusquement.",   // FR
        L"Hay un problema con la batería. Es posible que el equipo se apague de repente.",   // ES
        L"Es liegt ein Problem mit dem Akku vor. Der Computer kann plötzlich herunterfahren.",   // DE
        L"Existe um problema com a bateria. O computador pode desligar-se de repente.",   // PT
        L"Er is een probleem met de batterij. De computer kan plotseling uitschakelen.",   // NL
        L"Проблема с батареей. Компьютер может внезапно выключиться.",   // RU
        L"バッテリに問題があります。コンピューターが突然シャットダウンする可能性があります。",   // JA
        L"Wystąpił problem z baterią. Komputer może nagle się wyłączyć.",   // PL
        L"Der er et problem med batteriet. Computer kan lukke ned uden varsel.",   // DA
        L"Det finns ett problem med batteriet. Datorn kan stängas av plötsligt.",   // SV
        L"Det er et problem med batteriet. Datamaskinen kan slås av plutselig.",   // NO
        L"Akussa on ongelma. Tietokone voi sammua yllättäen.",   // FI
        L"Pille ilgili bir sorun var. Bilgisayar aniden kapanabilir."   // TR
      } },
    { 192,
      {
        L"Gli avvisi sullo stato delle batterie sono disabilitati. È possibile che il computer si spenga improvvisamente.",   // IT
        L"Battery status notifications are turned off. The computer may shut down unexpectedly.",   // EN
        L"Les notifications d'état de la batterie sont désactivées. L'ordinateur peut s'arrêter de façon inattendue.",   // FR
        L"Las notificaciones de estado de la batería están desactivadas. Es posible que el equipo se apague inesperadamente.",   // ES
        L"Benachrichtigungen zum Akkustatus sind deaktiviert. Der Computer kann unerwartet herunterfahren.",   // DE
        L"As notificações de estado da bateria estão desativadas. O computador pode desligar-se inesperadamente.",   // PT
        L"Meldingen over de batterijstatus zijn uitgeschakeld. De computer kan onverwacht uitschakelen.",   // NL
        L"Уведомления о состоянии батареи отключены. Компьютер может неожиданно выключиться.",   // RU
        L"バッテリの状態の通知がオフになっています。コンピューターが予期せずシャットダウンする可能性があります。",   // JA
        L"Powiadomienia o stanie baterii są wyłączone. Komputer może nieoczekiwanie się wyłączyć.",   // PL
        L"Meddelelser om batteristatus er slået fra. Computer kan lukke ned uventet.",   // DA
        L"Aviseringsfunktionen för batteristatus är avstängd. Datorn kan stängas av oväntat.",   // SV
        L"Varsler om batteristatus er slått av. Datamaskinen kan slås av uventet.",   // NO
        L"Akun tilaa koskevat ilmoitukset on poistettu käytöstä. Tietokone voi sammua odottamatta.",   // FI
        L"Pil durumu bildirimleri kapalı. Bilgisayar beklenmedik şekilde kapanabilir."   // TR
      } },
    { 206,
      {
        L"Misuratore alimentazione",   // IT
        L"Power meter",   // EN
        L"Compteur d'alimentation",   // FR
        L"Medidor de energía",   // ES
        L"Energieanzeige",   // DE
        L"Medidor de energia",   // PT
        L"Energiemeter",   // NL
        L"Индикатор питания",   // RU
        L"電源メーター",   // JA
        L"Miernik zasilania",   // PL
        L"Strømmåler",   // DA
        L"Energimätare",   // SV
        L"Strømmåler",   // NO
        L"Virran mittari",   // FI
        L"Güç ölçer"   // TR
      } },
    { 207,
      {
        L"Informazioni sul risparmio di energia",   // IT
        L"About power saving",   // EN
        L"À propos des économies d'énergie",   // FR
        L"Acerca del ahorro de energía",   // ES
        L"Informationen zur Energieeinsparung",   // DE
        L"Acerca da poupança de energia",   // PT
        L"Over energiebesparing",   // NL
        L"Об экономии энергии",   // RU
        L"省電力について",   // JA
        L"Informacje o oszczędzaniu energii",   // PL
        L"Om strømbesparelse",   // DA
        L"Om energisparande",   // SV
        L"Om strømsparing",   // NO
        L"Tietoja virransäästöstä",   // FI
        L"Güç tasarrufu hakkında"   // TR
      } },
    { 208,
      {
        L"Selezionare una combinazione per il risparmio di energia:",   // IT
        L"Select a power plan:",   // EN
        L"Sélectionner un mode de gestion de l'alimentation :",   // FR
        L"Seleccione un plan de energía:",   // ES
        L"Energiesparplan auswählen:",   // DE
        L"Selecionar um plano de energia:",   // PT
        L"Een energieplan selecteren:",   // NL
        L"Выбор плана электропитания:",   // RU
        L"電源プランを選択してください:",   // JA
        L"Wybierz plan zasilania:",   // PL
        L"Vælg en strømfunktion:",   // DA
        L"Välj ett energischema:",   // SV
        L"Velg en strømplan:",   // NO
        L"Valitse virransäästösuunnitelma:",   // FI
        L"Bir güç planı seçin:"   // TR
      } },
    { 209,
      {
        L"Altre opzioni di risparmio energia",   // IT
        L"More power options",   // EN
        L"Autres options d'alimentation",   // FR
        L"Más opciones de energía",   // ES
        L"Weitere Energieoptionen",   // DE
        L"Mais opções de energia",   // PT
        L"Meer energieopties",   // NL
        L"Дополнительные параметры электропитания",   // RU
        L"その他の電源オプション",   // JA
        L"Więcej opcji zasilania",   // PL
        L"Flere strømindstillinger",   // DA
        L"Fler energialternativ",   // SV
        L"Flere strømalternativer",   // NO
        L"Lisää virta-asetuksia",   // FI
        L"Diğer güç seçenekleri"   // TR
      } },
    { 210,
      {
        L"Combinazioni risparmio energia",   // IT
        L"Power plans",   // EN
        L"Modes de gestion de l'alimentation",   // FR
        L"Planes de energía",   // ES
        L"Energiesparpläne",   // DE
        L"Planos de energia",   // PT
        L"Energieplannen",   // NL
        L"Планы электропитания",   // RU
        L"電源プラン",   // JA
        L"Plany zasilania",   // PL
        L"Strømfunktioner",   // DA
        L"Energischeman",   // SV
        L"Strømplaner",   // NO
        L"Virransäästösuunnitelmat",   // FI
        L"Güç planları"   // TR
      } },
    { 211,
      {
        L"Alcune impostazioni sono gestite dall'amministratore del sistema.",   // IT
        L"Some settings are managed by your system administrator.",   // EN
        L"Certains paramètres sont gérés par votre administrateur système.",   // FR
        L"El administrador del sistema gestiona algunas opciones de configuración.",   // ES
        L"Einige Einstellungen werden von Ihrem Systemadministrator verwaltet.",   // DE
        L"Algumas definições são geridas pelo administrador do sistema.",   // PT
        L"Sommige instellingen worden beheerd door uw systeembeheerder.",   // NL
        L"Некоторыми параметрами управляет системный администратор.",   // RU
        L"一部の設定はシステム管理者が管理しています。",   // JA
        L"Niektóre ustawienia są zarządzane przez administratora systemu.",   // PL
        L"Nogle indstillinger administreres af systemadministratoren.",   // DA
        L"Vissa inställningar hanteras av systemadministratören.",   // SV
        L"Noen innstillinger administreres av systemansvarlig.",   // NO
        L"Järjestelmänvalvoja hallinnoi joitakin asetuksia.",   // FI
        L"Bazı ayarlar sistem yöneticiniz tarafından yönetilir."   // TR
      } },
    { 212,
      {
        L"Ragioni che impediscono la modifica di alcune impostazioni",   // IT
        L"Reasons why some settings cannot be changed",   // EN
        L"Raisons pour lesquelles certains paramètres ne peuvent pas être modifiés",   // FR
        L"Razones por las que no se pueden cambiar algunas opciones",   // ES
        L"Gründe, warum einige Einstellungen nicht geändert werden können",   // DE
        L"Motivos pelos quais algumas definições não podem ser alteradas",   // PT
        L"Redenen waarom sommige instellingen niet kunnen worden gewijzigd",   // NL
        L"Причины, по которым некоторые параметры нельзя изменить",   // RU
        L"一部の設定を変更できない理由",   // JA
        L"Powody, dla których nie można zmienić niektórych ustawień",   // PL
        L"Årsager til, at nogle indstillinger ikke kan ændres",   // DA
        L"Orsaker till att vissa inställningar inte kan ändras",   // SV
        L"Årsaker til at noen innstillinger ikke kan endres",   // NO
        L"Syyt, joiden vuoksi joitakin asetuksia ei voi muuttaa",   // FI
        L"Bazı ayarların değiştirilememesinin nedenleri"   // TR
      } },
    { 213,
      {
        L"Batterie",   // IT
        L"Batteries",   // EN
        L"Batteries",   // FR
        L"Baterías",   // ES
        L"Akkus",   // DE
        L"Baterias",   // PT
        L"Batterijen",   // NL
        L"Батареи",   // RU
        L"バッテリ",   // JA
        L"Baterie",   // PL
        L"Batterier",   // DA
        L"Batterier",   // SV
        L"Batterier",   // NO
        L"Akut",   // FI
        L"Piller"   // TR
      } },
    { 214,
      {
        L"Alcune impostazioni sono gestite dall'amministratore del sistema.",   // IT
        L"Some settings are managed by your system administrator.",   // EN
        L"Certains paramètres sont gérés par votre administrateur système.",   // FR
        L"El administrador del sistema gestiona algunas opciones de configuración.",   // ES
        L"Einige Einstellungen werden von Ihrem Systemadministrator verwaltet.",   // DE
        L"Algumas definições são geridas pelo administrador do sistema.",   // PT
        L"Sommige instellingen worden beheerd door uw systeembeheerder.",   // NL
        L"Некоторыми параметрами управляет системный администратор.",   // RU
        L"一部の設定はシステム管理者が管理しています。",   // JA
        L"Niektóre ustawienia są zarządzane przez administratora systemu.",   // PL
        L"Nogle indstillinger administreres af systemadministratoren.",   // DA
        L"Vissa inställningar hanteras av systemadministratören.",   // SV
        L"Noen innstillinger administreres av systemansvarlig.",   // NO
        L"Järjestelmänvalvoja hallinnoi joitakin asetuksia.",   // FI
        L"Bazı ayarlar sistem yöneticiniz tarafından yönetilir."   // TR
      } },
    { 227,
      {
        L"Rimozione sicura dell'hardware ed espulsione supporti",   // IT
        L"Safely Remove Hardware and Eject Media",   // EN
        L"Supprimer le périphérique en toute sécurité et éjecter le média",   // FR
        L"Quitar hardware de forma segura y expulsar el medio",   // ES
        L"Hardware sicher entfernen und Medium auswerfen",   // DE
        L"Remover hardware em segurança e ejetar suporte",   // PT
        L"Hardware veilig verwijderen en media uitwerpen",   // NL
        L"Безопасное извлечение устройства и носителя",   // RU
        L"ハードウェアを安全に取り外してメディアを取り出す",   // JA
        L"Bezpieczne usuwanie sprzętu i wysuwanie nośnika",   // PL
        L"Sikker fjernelse af hardware og udskubning af medie",   // DA
        L"Säker borttagning av maskinvara och utmatning av media",   // SV
        L"Sikker fjerning av maskinvare og utløsing av medier",   // NO
        L"Poista laitteisto turvallisesti ja poista tietoväline",   // FI
        L"Donanımı Güvenle Kaldır ve Medyayı Çıkar"   // TR
      } },
    { 231,
      {
        L"A&pri Dispositivi e stampanti",   // IT
        L"&Open Devices and Printers",   // EN
        L"&Ouvrir Périphériques et imprimantes",   // FR
        L"&Abrir dispositivos e impresoras",   // ES
        L"&Geräte und Drucker öffnen",   // DE
        L"&Abrir Dispositivos e Impressoras",   // PT
        L"Apparaten en printers &openen",   // NL
        L"&Открыть устройства и принтеры",   // RU
        L"デバイスとプリンターを開く(&O)",   // JA
        L"&Otwórz urządzenia i drukarki",   // PL
        L"&Åbn Enheder og printere",   // DA
        L"&Öppna enheter och skrivare",   // SV
        L"&Åpne enheter og skrivere",   // NO
        L"&Avaa laitteet ja tulostimet",   // FI
        L"&Aygıtlar ve Yazıcılar'ı aç"   // TR
      } },
    { 238,
      {
        L"%s",   // IT
        L"%s",   // EN
        L"%s",   // FR
        L"%s",   // ES
        L"%s",   // DE
        L"%s",   // PT
        L"%s",   // NL
        L"%s",   // RU
        L"%s",   // JA
        L"%s",   // PL
        L"%s",   // DA
        L"%s",   // SV
        L"%s",   // NO
        L"%s",   // FI
        L"%s"   // TR
      } },
    { 239,
      {
        L"Espelli %s",   // IT
        L"Eject %s",   // EN
        L"Éjecter %s",   // FR
        L"Expulsar %s",   // ES
        L"%s auswerfen",   // DE
        L"Ejetar %s",   // PT
        L"%s uitwerpen",   // NL
        L"Извлечь %s",   // RU
        L"%s を取り出す",   // JA
        L"Wysuń %s",   // PL
        L"Skub %s ud",   // DA
        L"Mata ut %s",   // SV
        L"Løs ut %s",   // NO
        L"Poista %s",   // FI
        L"%s öğesini çıkar"   // TR
      } },
    { 240,
      {
        L"   -   %s",   // IT
        L"   -   %s",   // EN
        L"   -   %s",   // FR
        L"   -   %s",   // ES
        L"   -   %s",   // DE
        L"   -   %s",   // PT
        L"   -   %s",   // NL
        L"   -   %s",   // RU
        L"   -   %s",   // JA
        L"   -   %s",   // PL
        L"   -   %s",   // DA
        L"   -   %s",   // SV
        L"   -   %s",   // NO
        L"   -   %s",   // FI
        L"   -   %s"   // TR
      } },
    { 241,
      {
        L"   -   Espelli %s",   // IT
        L"   -   Eject %s",   // EN
        L"   -   Éjecter %s",   // FR
        L"   -   Expulsar %s",   // ES
        L"   -   %s auswerfen",   // DE
        L"   -   Ejetar %s",   // PT
        L"   -   %s uitwerpen",   // NL
        L"   -   Извлечь %s",   // RU
        L"   -   %s を取り出す",   // JA
        L"   -   Wysuń %s",   // PL
        L"   -   Skub %s ud",   // DA
        L"   -   Mata ut %s",   // SV
        L"   -   Løs ut %s",   // NO
        L"   -   Poista %s",   // FI
        L"   -   %s öğesini çıkar"   // TR
      } },
    { 346,
      {
        L"Tasti permanenti",   // IT
        L"Sticky Keys",   // EN
        L"Touches rémanentes",   // FR
        L"Teclas especiales",   // ES
        L"Einrastfunktion",   // DE
        L"Teclas Persistentes",   // PT
        L"Plaktoetsen",   // NL
        L"Залипание клавиш",   // RU
        L"固定キー機能",   // JA
        L"Klawisze trwałe",   // PL
        L"Fastholdelsestaster",   // DA
        L"Fästtangenter",   // SV
        L"Festetaster",   // NO
        L"Kiinnitys",   // FI
        L"Yapışkan Tuşlar"   // TR
      } },
    { 347,
      {
        L"Controllo puntatore",   // IT
        L"Pointer control",   // EN
        L"Contrôle du pointeur",   // FR
        L"Control del puntero",   // ES
        L"Zeigersteuerung",   // DE
        L"Controlo do ponteiro",   // PT
        L"Aanwijzerbeheer",   // NL
        L"Управление указателем",   // RU
        L"ポインターの制御",   // JA
        L"Sterowanie wskaźnikiem",   // PL
        L"Styring af markør",   // DA
        L"Pekarkontroll",   // SV
        L"Pekekontroll",   // NO
        L"Osoittimen hallinta",   // FI
        L"İşaretçi denetimi"   // TR
      } },
    { 348,
      {
        L"Filtro tasti",   // IT
        L"Filter Keys",   // EN
        L"Touches filtrées",   // FR
        L"Filtros de teclas",   // ES
        L"Filtertasten",   // DE
        L"Teclas de Filtragem",   // PT
        L"Filtertoetsen",   // NL
        L"Фильтрация ввода",   // RU
        L"フィルター キー",   // JA
        L"Klawisze filtrujące",   // PL
        L"Filtertaster",   // DA
        L"Filtertangenter",   // SV
        L"Filtertaster",   // NO
        L"Suodatusavaimet",   // FI
        L"Filtre Tuşları"   // TR
      } },
    { 417,
      {
        L"Batteria %s: %s",   // IT
        L"Battery %s: %s",   // EN
        L"Batterie %s : %s",   // FR
        L"Batería %s: %s",   // ES
        L"Akku %s: %s",   // DE
        L"Bateria %s: %s",   // PT
        L"Batterij %s: %s",   // NL
        L"Батарея %s: %s",   // RU
        L"バッテリ %s: %s",   // JA
        L"Bateria %s: %s",   // PL
        L"Batteri %s: %s",   // DA
        L"Batteri %s: %s",   // SV
        L"Batteri %s: %s",   // NO
        L"Akku %s: %s",   // FI
        L"Pil %s: %s"   // TR
      } },
    { 418,
      {
        L"Batteria di breve durata %s: %s",   // IT
        L"Short battery life %s: %s",   // EN
        L"Autonomie faible %s : %s",   // FR
        L"Duración de batería baja %s: %s",   // ES
        L"Kurze Akkulaufzeit %s: %s",   // DE
        L"Bateria de curta duração %s: %s",   // PT
        L"Korte batterijduur %s: %s",   // NL
        L"Короткое время работы батареи %s: %s",   // RU
        L"バッテリ駆動時間が短い %s: %s",   // JA
        L"Krótki czas pracy baterii %s: %s",   // PL
        L"Kort batterilevetid %s: %s",   // DA
        L"Kort batteritid %s: %s",   // SV
        L"Kort batterilevetid %s: %s",   // NO
        L"Akun kesto on lyhyt %s: %s",   // FI
        L"Kısa pil ömrü %s: %s"   // TR
      } },
    { 419,
      {
        L"Non presente",   // IT
        L"Not present",   // EN
        L"Non présent",   // FR
        L"No presente",   // ES
        L"Nicht vorhanden",   // DE
        L"Não presente",   // PT
        L"Niet aanwezig",   // NL
        L"Отсутствует",   // RU
        L"存在しません",   // JA
        L"Nieobecny",   // PL
        L"Ikke til stede",   // DA
        L"Saknas",   // SV
        L"Ikke til stede",   // NO
        L"Ei paikalla",   // FI
        L"Yok"   // TR
      } },
    { 420,
      {
        L"#%1!u!",   // IT
        L"#%1!u!",   // EN
        L"#%1!u!",   // FR
        L"#%1!u!",   // ES
        L"#%1!u!",   // DE
        L"#%1!u!",   // PT
        L"#%1!u!",   // NL
        L"#%1!u!",   // RU
        L"#%1!u!",   // JA
        L"#%1!u!",   // PL
        L"#%1!u!",   // DA
        L"#%1!u!",   // SV
        L"#%1!u!",   // NO
        L"#%1!u!",   // FI
        L"#%1!u!"   // TR
      } },
    { 421,
      {
        L"La batteria in uso è quasi scarica e non può essere caricata completamente. Collegare il PC alla rete elettrica oppure arrestarlo e sostituire la batteria.",   // IT
        L"The battery in use is critically low and cannot be charged completely. Connect the PC to power or shut it down and replace the battery.",   // EN
        L"La batterie utilisée est presque déchargée et ne peut pas être rechargée complètement. Branchez le PC sur secteur ou arrêtez-le et remplacez la batterie.",   // FR
        L"La batería en uso está muy baja y no se puede cargar por completo. Conecte el equipo a la red eléctrica o apáguelo y sustituya la batería.",   // ES
        L"Der verwendete Akku ist fast leer und kann nicht vollständig geladen werden. Schließen Sie den PC an das Stromnetz an, oder fahren Sie ihn herunter und tauschen Sie den Akku aus.",   // DE
        L"A bateria em uso está quase vazia e não pode ser carregada completamente. Ligue o computador à corrente elétrica ou desligue-o e substitua a bateria.",   // PT
        L"De gebruikte batterij is bijna leeg en kan niet volledig worden opgeladen. Sluit de pc aan op het stopcontact of schakel deze uit en vervang de batterij.",   // NL
        L"Используемая батарея почти разряжена и не может быть полностью заряжена. Подключите компьютер к электросети или выключите его и замените батарею.",   // RU
        L"使用中のバッテリは残量が非常に少なく、完全に充電できません。PC を電源に接続するか、シャットダウンしてバッテリを交換してください。",   // JA
        L"Używana bateria jest prawie rozładowana i nie można jej w pełni naładować. Podłącz komputer do zasilania albo wyłącz go i wymień baterię.",   // PL
        L"Det anvendte batteri er næsten afladet og kan ikke oplades helt. Slut pc'en til strøm, eller luk den ned, og udskift batteriet.",   // DA
        L"Batteriet som används är nästan slut och kan inte laddas helt. Anslut datorn till elnätet eller stäng av den och byt batteri.",   // SV
        L"Batteriet som brukes er nesten tomt og kan ikke lades helt opp. Koble PC-en til strøm, eller slå den av og bytt batteri.",   // NO
        L"Käytössä oleva akku on lähes tyhjä, eikä sitä voi ladata täyteen. Kytke tietokone sähköverkkoon tai sammuta se ja vaihda akku.",   // FI
        L"Kullanılan pilin şarjı neredeyse bitti ve tam olarak şarj edilemiyor. PC'yi güç kaynağına bağlayın veya kapatıp pili değiştirin."   // TR
      } },
    { 422,
      {
        L"Provare a sostituire la batteria",   // IT
        L"Try replacing the battery",   // EN
        L"Essayez de remplacer la batterie",   // FR
        L"Intente reemplazar la batería",   // ES
        L"Versuchen Sie, den Akku auszutauschen",   // DE
        L"Tente substituir a bateria",   // PT
        L"Probeer de batterij te vervangen",   // NL
        L"Попробуйте заменить батарею",   // RU
        L"バッテリの交換を試してください",   // JA
        L"Spróbuj wymienić baterię",   // PL
        L"Prøv at udskifte batteriet",   // DA
        L"Försök byta batteriet",   // SV
        L"Prøv å bytte batteriet",   // NO
        L"Yritä vaihtaa akku",   // FI
        L"Pili değiştirmeyi deneyin"   // TR
      } },
    { 423,
      {
        L"Collegare il computer ad una presa di corrente.",   // IT
        L"Connect the computer to a power outlet.",   // EN
        L"Connectez l'ordinateur à une prise électrique.",   // FR
        L"Conecte el equipo a una toma de corriente.",   // ES
        L"Schließen Sie den Computer an eine Steckdose an.",   // DE
        L"Ligue o computador a uma tomada elétrica.",   // PT
        L"Sluit de computer aan op een stopcontact.",   // NL
        L"Подключите компьютер к электрической розетке.",   // RU
        L"コンピューターを電源コンセントに接続してください。",   // JA
        L"Podłącz komputer do gniazda zasilania.",   // PL
        L"Slut computeren til en stikkontakt.",   // DA
        L"Anslut datorn till ett eluttag.",   // SV
        L"Koble datamaskinen til et strømuttak.",   // NO
        L"Kytke tietokone pistorasiaan.",   // FI
        L"Bilgisayarı bir prize takın."   // TR
      } },
    { 424,
      {
        L"Impostazioni sfondo del desktop",   // IT
        L"Desktop background settings",   // EN
        L"Paramètres d'arrière-plan du Bureau",   // FR
        L"Configuración del fondo de escritorio",   // ES
        L"Desktophintergrund-Einstellungen",   // DE
        L"Definições do plano de fundo da área de trabalho",   // PT
        L"Instellingen voor bureaubladachtergrond",   // NL
        L"Параметры фона рабочего стола",   // RU
        L"デスクトップの背景の設定",   // JA
        L"Ustawienia tła pulpitu",   // PL
        L"Indstillinger for skrivebordsbaggrund",   // DA
        L"Inställningar för skrivbordsbakgrund",   // SV
        L"Innstillinger for skrivebordsbakgrunn",   // NO
        L"Työpöydän taustan asetukset",   // FI
        L"Masaüstü arka planı ayarları"   // TR
      } },
    { 425,
      {
        L"Modifica le impostazioni di risparmio energia per lo sfondo del desktop.",   // IT
        L"Change power saving settings for the desktop background.",   // EN
        L"Modifiez les paramètres d'économie d'énergie pour l'arrière-plan du Bureau.",   // FR
        L"Cambie la configuración de ahorro de energía del fondo de escritorio.",   // ES
        L"Ändern Sie die Energieeinstellungen für den Desktophintergrund.",   // DE
        L"Altere as definições de poupança de energia do plano de fundo.",   // PT
        L"Wijzig energiebesparingsinstellingen voor de bureaubladachtergrond.",   // NL
        L"Измените параметры энергосбережения для фона рабочего стола.",   // RU
        L"デスクトップの背景の省電力設定を変更します。",   // JA
        L"Zmień ustawienia oszczędzania energii tła pulpitu.",   // PL
        L"Skift strømbesparelsesindstillinger for skrivebordsbaggrunden.",   // DA
        L"Ändra energisparinställningar för skrivbordsbakgrunden.",   // SV
        L"Endre strømsparingsinnstillinger for skrivebordsbakgrunnen.",   // NO
        L"Muuta työpöydän taustan virransäästöasetuksia.",   // FI
        L"Masaüstü arka planı için güç tasarrufu ayarlarını değiştirin."   // TR
      } },
    { 426,
      {
        L"Presentazione",   // IT
        L"Slide show",   // EN
        L"Diaporama",   // FR
        L"Presentación",   // ES
        L"Diashow",   // DE
        L"Apresentação de diapositivos",   // PT
        L"Diavoorstelling",   // NL
        L"Слайд-шоу",   // RU
        L"スライド ショー",   // JA
        L"Pokaz slajdów",   // PL
        L"Diasshow",   // DA
        L"Bildspel",   // SV
        L"Lysbildeframvisning",   // NO
        L"Diaesitys",   // FI
        L"Slayt gösterisi"   // TR
      } },
    { 427,
      {
        L"Specificare quando si desidera che sia disponibile la diapositiva di sfondo del desktop.",   // IT
        L"Specify when you want the desktop background slide show to be available.",   // EN
        L"Spécifiez quand vous souhaitez que le diaporama d'arrière-plan du Bureau soit disponible.",   // FR
        L"Especifique cuándo desea que la presentación del fondo de escritorio esté disponible.",   // ES
        L"Geben Sie an, wann die Desktophintergrund-Diashow verfügbar sein soll.",   // DE
        L"Especifique quando pretende que a apresentação de diapositivos de fundo esteja disponível.",   // PT
        L"Geef op wanneer de diavoorstelling voor de bureaubladachtergrond beschikbaar moet zijn.",   // NL
        L"Укажите, когда должно быть доступно слайд-шоу фона рабочего стола.",   // RU
        L"デスクトップの背景のスライド ショーを利用するタイミングを指定します。",   // JA
        L"Określ, kiedy pokaz slajdów tła pulpitu ma być dostępny.",   // PL
        L"Angiv, hvornår diasshowet for skrivebordsbaggrunden skal være tilgængeligt.",   // DA
        L"Ange när bildspelet för skrivbordsbakgrunden ska vara tillgängligt.",   // SV
        L"Angi når lysbildeframvisningen for skrivebordsbakgrunnen skal være tilgjengelig.",   // NO
        L"Määritä, milloin työpöydän taustan diaesitys on käytettävissä.",   // FI
        L"Masaüstü arka planı slayt gösterisinin ne zaman kullanılabilir olacağını belirtin."   // TR
      } },
    { 428,
      {
        L"Sospesa",   // IT
        L"Suspended",   // EN
        L"Suspendu",   // FR
        L"Suspendida",   // ES
        L"Angehalten",   // DE
        L"Suspensa",   // PT
        L"Onderbroken",   // NL
        L"Приостановлено",   // RU
        L"一時停止",   // JA
        L"Wstrzymane",   // PL
        L"Suspenderet",   // DA
        L"Pausat",   // SV
        L"Satt på pause",   // NO
        L"Keskeytetty",   // FI
        L"Askıya alındı"   // TR
      } },
    { 429,
      {
        L"Presentazione sospesa per risparmiare energia.",   // IT
        L"Slide show suspended to save power.",   // EN
        L"Diaporama suspendu pour économiser l'énergie.",   // FR
        L"Presentación suspendida para ahorrar energía.",   // ES
        L"Diashow wurde angehalten, um Energie zu sparen.",   // DE
        L"Apresentação suspensa para poupar energia.",   // PT
        L"Diavoorstelling onderbroken om energie te besparen.",   // NL
        L"Слайд-шоу приостановлено для экономии энергии.",   // RU
        L"省電力のためスライド ショーは一時停止されています。",   // JA
        L"Pokaz slajdów wstrzymany w celu oszczędzania energii.",   // PL
        L"Diasshowet er sat på pause for at spare strøm.",   // DA
        L"Bildspelet pausades för att spara energi.",   // SV
        L"Lysbildeframvisningen er satt på pause for å spare strøm.",   // NO
        L"Diaesitys keskeytettiin virran säästämiseksi.",   // FI
        L"Güç tasarrufu için slayt gösterisi askıya alındı."   // TR
      } },
    { 430,
      {
        L"Disponibile",   // IT
        L"Available",   // EN
        L"Disponible",   // FR
        L"Disponible",   // ES
        L"Verfügbar",   // DE
        L"Disponível",   // PT
        L"Beschikbaar",   // NL
        L"Доступно",   // RU
        L"利用可能",   // JA
        L"Dostępne",   // PL
        L"Tilgængelig",   // DA
        L"Tillgänglig",   // SV
        L"Tilgjengelig",   // NO
        L"Käytettävissä",   // FI
        L"Kullanılabilir"   // TR
      } },
    { 431,
      {
        L"Disponibile",   // IT
        L"Available",   // EN
        L"Disponible",   // FR
        L"Disponible",   // ES
        L"Verfügbar",   // DE
        L"Disponível",   // PT
        L"Beschikbaar",   // NL
        L"Доступно",   // RU
        L"利用可能",   // JA
        L"Dostępne",   // PL
        L"Tilgængelig",   // DA
        L"Tillgänglig",   // SV
        L"Tilgjengelig",   // NO
        L"Käytettävissä",   // FI
        L"Kullanılabilir"   // TR
      } },
    { 432,
      {
        L"&Sfondo per il desktop successivo",   // IT
        L"&Next desktop background",   // EN
        L"Arrière-plan du Bureau suiva&nt",   // FR
        L"Fondo de escritorio &siguiente",   // ES
        L"&Nächster Desktophintergrund",   // DE
        L"Plano de fundo seguinte da área de &trabalho",   // PT
        L"&Volgende bureaubladachtergrond",   // NL
        L"&Следующий фон рабочего стола",   // RU
        L"次のデスクトップの背景(&N)",   // JA
        L"&Następne tło pulpitu",   // PL
        L"&Næste skrivebordsbaggrund",   // DA
        L"&Nästa skrivbordsbakgrund",   // SV
        L"&Neste skrivebordsbakgrunn",   // NO
        L"&Seuraava työpöydän tausta",   // FI
        L"&Sonraki masaüstü arka planı"   // TR
      } },
    { 433,
      {
        L"Imposta come sfondo del des&ktop",   // IT
        L"Set as desktop back&ground",   // EN
        L"Définir comme arrière-plan du Bureau (&g)",   // FR
        L"Establecer como fondo de escritorio (&g)",   // ES
        L"Als Desktophintergrund festlegen (&g)",   // DE
        L"Definir como plano de fundo (&g)",   // PT
        L"Instellen als bureaubladachtergrond (&g)",   // NL
        L"Сделать фоном рабочего стола (&g)",   // RU
        L"デスクトップの背景に設定(&G)",   // JA
        L"Ustaw jako tło pulpitu (&g)",   // PL
        L"Angiv som skrivebordsbaggrund (&g)",   // DA
        L"Ange som skrivbordsbakgrund (&g)",   // SV
        L"Angi som skrivebordsbakgrunn (&g)",   // NO
        L"Aseta työpöydän taustaksi (&g)",   // FI
        L"Masaüstü arka planı olarak ayarla (&g)"   // TR
      } },
    { 434,
      {
        L"La batteria si scarica anche se il PC è collegato alla rete elettrica.",   // IT
        L"The battery is draining even though the PC is plugged in.",   // EN
        L"La batterie se décharge même si le PC est branché.",   // FR
        L"La batería se agota aunque el equipo esté enchufado.",   // ES
        L"Der Akku wird entladen, obwohl der PC angeschlossen ist.",   // DE
        L"A bateria está a descarregar-se apesar de o computador estar ligado à corrente.",   // PT
        L"De batterij loopt leeg terwijl de pc is aangesloten.",   // NL
        L"Батарея разряжается, несмотря на подключённый компьютер.",   // RU
        L"PC が電源に接続されているのにバッテリが放電しています。",   // JA
        L"Bateria rozładowuje się mimo podłączonego zasilania.",   // PL
        L"Batteriet aflades, selv om pc'en er tilsluttet.",   // DA
        L"Batteriet laddas ur trots att datorn är ansluten.",   // SV
        L"Batteriet tappes selv om PC-en er koblet til.",   // NO
        L"Akku tyhjenee, vaikka tietokone on kytketty verkkovirtaan.",   // FI
        L"PC prize takılı olmasına rağmen pil boşalıyor."   // TR
      } },
    { 436,
      {
        L"Collegare il PC a una presa di corrente",   // IT
        L"Connect the PC to a power outlet",   // EN
        L"Branchez le PC à une prise électrique",   // FR
        L"Conecte el equipo a una toma de corriente",   // ES
        L"Schließen Sie den PC an eine Steckdose an",   // DE
        L"Ligue o computador a uma tomada elétrica",   // PT
        L"Sluit de pc aan op een stopcontact",   // NL
        L"Подключите компьютер к электрической розетке",   // RU
        L"PC を電源コンセントに接続してください",   // JA
        L"Podłącz komputer do gniazda zasilania",   // PL
        L"Slut pc'en til en stikkontakt",   // DA
        L"Anslut datorn till ett eluttag",   // SV
        L"Koble PC-en til et strømuttak",   // NO
        L"Kytke tietokone pistorasiaan",   // FI
        L"PC'yi bir prize takın"   // TR
      } },
    { 437,
      {
        L"Collegarsi alla rete elettrica o trovare un'altra fonte di alimentazione.",   // IT
        L"Connect to power or find another power source.",   // EN
        L"Branchez-vous sur secteur ou recherchez une autre source d'alimentation.",   // FR
        L"Conéctese a la red eléctrica o busque otra fuente de alimentación.",   // ES
        L"Schließen Sie den PC an das Stromnetz an, oder suchen Sie eine andere Stromquelle.",   // DE
        L"Ligue-se à corrente elétrica ou encontre outra fonte de alimentação.",   // PT
        L"Sluit aan op het stopcontact of zoek een andere energiebron.",   // NL
        L"Подключитесь к электросети или найдите другой источник питания.",   // RU
        L"電源に接続するか、別の電源を見つけてください。",   // JA
        L"Podłącz zasilanie lub znajdź inne źródło zasilania.",   // PL
        L"Slut til strøm, eller find en anden strømkilde.",   // DA
        L"Anslut till elnätet eller hitta en annan strömkälla.",   // SV
        L"Koble til strøm eller finn en annen strømkilde.",   // NO
        L"Kytke virtaan tai etsi toinen virtalähde.",   // FI
        L"Güce bağlanın veya başka bir güç kaynağı bulun."   // TR
      } },
    { 438,
      {
        L"Il livello di carica della batteria è molto basso",   // IT
        L"The battery level is very low",   // EN
        L"Le niveau de la batterie est très faible",   // FR
        L"El nivel de batería es muy bajo",   // ES
        L"Der Akkustand ist sehr niedrig",   // DE
        L"O nível da bateria está muito baixo",   // PT
        L"Het batterijniveau is zeer laag",   // NL
        L"Уровень заряда батареи очень низкий",   // RU
        L"バッテリ残量が非常に少なくなっています",   // JA
        L"Poziom naładowania baterii jest bardzo niski",   // PL
        L"Batteriniveauet er meget lavt",   // DA
        L"Batterinivån är mycket låg",   // SV
        L"Batterinivået er svært lavt",   // NO
        L"Akun varaustaso on erittäin alhainen",   // FI
        L"Pil düzeyi çok düşük"   // TR
      } },
    { 439,
      {
        L"Collegare il PC a una presa di corrente.",   // IT
        L"Connect the PC to a power outlet.",   // EN
        L"Branchez le PC à une prise électrique.",   // FR
        L"Conecte el equipo a una toma de corriente.",   // ES
        L"Schließen Sie den PC an eine Steckdose an.",   // DE
        L"Ligue o computador a uma tomada elétrica.",   // PT
        L"Sluit de pc aan op een stopcontact.",   // NL
        L"Подключите компьютер к электрической розетке.",   // RU
        L"PC を電源コンセントに接続してください。",   // JA
        L"Podłącz komputer do gniazda zasilania.",   // PL
        L"Slut pc'en til en stikkontakt.",   // DA
        L"Anslut datorn till ett eluttag.",   // SV
        L"Koble PC-en til et strømuttak.",   // NO
        L"Kytke tietokone pistorasiaan.",   // FI
        L"PC'yi bir prize takın."   // TR
      } },
    { 441,
      {
        L"La batteria è quasi scarica.",   // IT
        L"The battery is almost drained.",   // EN
        L"La batterie est presque déchargée.",   // FR
        L"La batería está casi agotada.",   // ES
        L"Der Akku ist fast leer.",   // DE
        L"A bateria está quase vazia.",   // PT
        L"De batterij is bijna leeg.",   // NL
        L"Батарея почти разряжена.",   // RU
        L"バッテリ残量がほとんどありません。",   // JA
        L"Bateria jest prawie rozładowana.",   // PL
        L"Batteriet er næsten afladet.",   // DA
        L"Batteriet är nästan slut.",   // SV
        L"Batteriet er nesten tomt.",   // NO
        L"Akku on lähes tyhjä.",   // FI
        L"Pilin şarjı neredeyse bitti."   // TR
      } },
    { 442,
      {
        L"La batteria è quasi scarica.",   // IT
        L"The battery is almost drained.",   // EN
        L"La batterie est presque déchargée.",   // FR
        L"La batería está casi agotada.",   // ES
        L"Der Akku ist fast leer.",   // DE
        L"A bateria está quase vazia.",   // PT
        L"De batterij is bijna leeg.",   // NL
        L"Батарея почти разряжена.",   // RU
        L"バッテリ残量がほとんどありません。",   // JA
        L"Bateria jest prawie rozładowana.",   // PL
        L"Batteriet er næsten afladet.",   // DA
        L"Batteriet är nästan slut.",   // SV
        L"Batteriet er nesten tomt.",   // NO
        L"Akku on lähes tyhjä.",   // FI
        L"Pilin şarjı neredeyse bitti."   // TR
      } },
};
static const TrayString kPniduiStrings[] = {
    { 18,
      {
        L"Non connesso - Nessuna connessione disponibile",   // IT
        L"Not connected - No connections are available",   // EN
        L"Non connecté - Aucune connexion disponible",   // FR
        L"Sin conexión: no hay conexiones disponibles",   // ES
        L"Nicht verbunden - Keine Verbindungen verfügbar",   // DE
        L"Sem ligação - Não existem ligações disponíveis",   // PT
        L"Niet verbonden - Geen verbindingen beschikbaar",   // NL
        L"Не подключено — нет доступных подключений",   // RU
        L"非接続 - 利用できる接続はありません",   // JA
        L"Brak połączenia — nie są dostępne żadne połączenia",   // PL
        L"Ikke forbundet – ingen forbindelser er tilgængelige",   // DA
        L"Inte ansluten – inga anslutningar är tillgängliga",   // SV
        L"Ikke tilkoblet – ingen tilkoblinger er tilgjengelige",   // NO
        L"Ei yhteyttä – yhteyksiä ei ole saatavilla",   // FI
        L"Bağlı değil - Kullanılabilir bağlantı yok"   // TR
      } },
    { 24,
      {
        L"Accesso a Internet",   // IT
        L"Internet access",   // EN
        L"Accès Internet",   // FR
        L"Acceso a Internet",   // ES
        L"Internetzugriff",   // DE
        L"Acesso à Internet",   // PT
        L"Internettoegang",   // NL
        L"Доступ к Интернету",   // RU
        L"インターネット アクセス",   // JA
        L"Dostęp do Internetu",   // PL
        L"Internettadgang",   // DA
        L"Internetåtkomst",   // SV
        L"Internett-tilgang",   // NO
        L"Internet-yhteys",   // FI
        L"İnternet erişimi"   // TR
      } },
    { 31,
      {
        L"Stato connessione - Sconosciuto",   // IT
        L"Connection status - Unknown",   // EN
        L"État de la connexion - Inconnu",   // FR
        L"Estado de conexión: desconocido",   // ES
        L"Verbindungsstatus - Unbekannt",   // DE
        L"Estado da ligação - Desconhecido",   // PT
        L"Verbindingsstatus - Onbekend",   // NL
        L"Состояние подключения — неизвестно",   // RU
        L"接続の状態 - 不明",   // JA
        L"Stan połączenia — nieznany",   // PL
        L"Forbindelsesstatus – ukendt",   // DA
        L"Anslutningsstatus – okänd",   // SV
        L"Tilkoblingsstatus – ukjent",   // NO
        L"Yhteyden tila – tuntematon",   // FI
        L"Bağlantı durumu - Bilinmiyor"   // TR
      } },
    { 51,
      {
        L"Nessun accesso a Internet",   // IT
        L"No Internet access",   // EN
        L"Aucun accès Internet",   // FR
        L"Sin acceso a Internet",   // ES
        L"Kein Internetzugriff",   // DE
        L"Sem acesso à Internet",   // PT
        L"Geen internettoegang",   // NL
        L"Нет доступа к Интернету",   // RU
        L"インターネット アクセスなし",   // JA
        L"Brak dostępu do Internetu",   // PL
        L"Ingen internettadgang",   // DA
        L"Ingen internetåtkomst",   // SV
        L"Ingen Internett-tilgang",   // NO
        L"Ei Internet-yhteyttä",   // FI
        L"İnternet erişimi yok"   // TR
      } },
    { 54,
      {
        L"Non connesso - Sono disponibili connessioni",   // IT
        L"Not connected - Connections are available",   // EN
        L"Non connecté - Des connexions sont disponibles",   // FR
        L"Sin conexión: hay conexiones disponibles",   // ES
        L"Nicht verbunden - Verbindungen sind verfügbar",   // DE
        L"Sem ligação - Existem ligações disponíveis",   // PT
        L"Niet verbonden - Verbindingen zijn beschikbaar",   // NL
        L"Не подключено — доступны подключения",   // RU
        L"非接続 - 利用可能な接続があります",   // JA
        L"Brak połączenia — są dostępne połączenia",   // PL
        L"Ikke forbundet – der er forbindelser",   // DA
        L"Inte ansluten – anslutningar finns",   // SV
        L"Ikke tilkoblet – tilkoblinger er tilgjengelige",   // NO
        L"Ei yhteyttä – yhteyksiä on saatavilla",   // FI
        L"Bağlı değil - Bağlantılar kullanılabilir"   // TR
      } },
    { 63,
      {
        L"Rete Windows",   // IT
        L"Windows Network",   // EN
        L"Réseau Windows",   // FR
        L"Red de Windows",   // ES
        L"Windows-Netzwerk",   // DE
        L"Rede Windows",   // PT
        L"Windows-netwerk",   // NL
        L"Сеть Windows",   // RU
        L"Windows ネットワーク",   // JA
        L"Sieć Windows",   // PL
        L"Windows-netværk",   // DA
        L"Windows-nätverk",   // SV
        L"Windows-nettverk",   // NO
        L"Windows-verkko",   // FI
        L"Windows Ağı"   // TR
      } },
    { 64,
      {
        L"Icona sistema di rete",   // IT
        L"Network system icon",   // EN
        L"Icône système réseau",   // FR
        L"Icono del sistema de red",   // ES
        L"Netzwerksystemsymbol",   // DE
        L"Ícone de sistema de rede",   // PT
        L"Systeempictogram netwerk",   // NL
        L"Системный значок сети",   // RU
        L"ネットワーク システム アイコン",   // JA
        L"Ikona systemowa sieci",   // PL
        L"Netværkssystemikon",   // DA
        L"Systemikon för nätverk",   // SV
        L"Systemikon for nettverk",   // NO
        L"Verkon järjestelmäkuvake",   // FI
        L"Ağ sistem simgesi"   // TR
      } },
    { 76,
      {
        L"Non connesso - Modalità aereo",   // IT
        L"Not connected - Airplane mode",   // EN
        L"Non connecté - Mode avion",   // FR
        L"Sin conexión: modo avión",   // ES
        L"Nicht verbunden - Flugzeugmodus",   // DE
        L"Sem ligação - Modo de avião",   // PT
        L"Niet verbonden - Vliegtuigmodus",   // NL
        L"Не подключено — режим полёта",   // RU
        L"非接続 - 機内モード",   // JA
        L"Brak połączenia — tryb samolotowy",   // PL
        L"Ikke forbundet – flytilstand",   // DA
        L"Inte ansluten – flygplansläge",   // SV
        L"Ikke tilkoblet – flymodus",   // NO
        L"Ei yhteyttä – lentotila",   // FI
        L"Bağlı değil - Uçak modu"   // TR
      } },
    { 77,
      {
        L"Modalità aereo",   // IT
        L"Airplane mode",   // EN
        L"Mode avion",   // FR
        L"Modo avión",   // ES
        L"Flugzeugmodus",   // DE
        L"Modo de avião",   // PT
        L"Vliegtuigmodus",   // NL
        L"Режим полёта",   // RU
        L"機内モード",   // JA
        L"Tryb samolotowy",   // PL
        L"Flytilstand",   // DA
        L"Flygplansläge",   // SV
        L"Flymodus",   // NO
        L"Lentotila",   // FI
        L"Uçak modu"   // TR
      } },
    { 3027,
      {
        L"Icona sistema di rete",   // IT
        L"Network system icon",   // EN
        L"Icône système réseau",   // FR
        L"Icono del sistema de red",   // ES
        L"Netzwerksymbol",   // DE
        L"Ícone de sistema de rede",   // PT
        L"Systeempictogram netwerk",   // NL
        L"Системный значок сети",   // RU
        L"ネットワーク システム アイコン",   // JA
        L"Ikona systemowa sieci",   // PL
        L"Netværkssystemikon",   // DA
        L"Systemikon för nätverk",   // SV
        L"Systemikon for nettverk",   // NO
        L"Verkon järjestelmäkuvake",   // FI
        L"Ağ sistem simgesi"   // TR
      } },
};

// The language actually used by the table, for the log only.
static const wchar_t* TrayLangName() {
    static const wchar_t* const names[LANG_COUNT] = {
        L"it", L"en", L"fr", L"es", L"de", L"pt", L"nl", L"ru", L"ja", L"pl",
        L"da", L"sv", L"no", L"fi", L"tr"};
    const UiLangId lang = DetectUiLang();
    return names[(lang > LANG_IT && lang < LANG_COUNT) ? lang : LANG_EN];
}

// The order is the one fixed for the module: the Windows 10 text of the store first
// (LoadStringW_Hook calls the original function before this table), then the table in
// the language of the interface, then English.
static const wchar_t* LookupTrayString(const TrayString* table, size_t count, UINT id) {
    for (size_t i = 0; i < count; i++) {
        if (table[i].id != id) continue;
        static UiLangId chosen = LANG_COUNT;      // LANG_COUNT = not looked up yet
        if (chosen == LANG_COUNT) chosen = DetectUiLang();
        const wchar_t* text = table[i].text[chosen];
        if (text && text[0]) return text;
        return table[i].text[LANG_EN];
    }
    return nullptr;
}

// Is the module one of the two redirected to the data folder? It is compared
// with the handle loaded in this process (the redirection makes it the Windows 10
// copy): no path comparisons, which the path spoof would falsify.
static const wchar_t* RedirectedTrayModule(HINSTANCE hInst) {
    if (!hInst) return nullptr;
    if (hInst == GetModuleHandleW(L"stobject.dll")) return L"stobject.dll";
    if (hInst == GetModuleHandleW(L"pnidui.dll")) return L"pnidui.dll";
    return nullptr;
}

typedef int(WINAPI* LoadStringW_t)(HINSTANCE, UINT, LPWSTR, int);
static LoadStringW_t LoadStringW_Original = nullptr;
static int g_trayTextLogs = 0;
static int g_trayTextMisses = 0;

static int WINAPI LoadStringW_Hook(HINSTANCE hInst, UINT id, LPWSTR buffer, int cch) {
    int result = LoadStringW_Original(hInst, id, buffer, cch);
    if (result > 0 || !buffer || cch <= 0) return result;   // it is there: leave it alone
    try {
        const wchar_t* moduleName = RedirectedTrayModule(hInst);
        if (!moduleName) return result;
        const wchar_t* text = (moduleName[0] == L's')
                                  ? LookupTrayString(kStobjectStrings, _countof(kStobjectStrings), id)
                                  : LookupTrayString(kPniduiStrings, _countof(kPniduiStrings), id);
        // Diagnostics: an id asked by a tray module and missing from the Windows 10
        // table as well. Without this line the failure is silent, and an empty menu and
        // a table that does not cover the id look exactly the same in the log.
        if (!text) {
            if (g_trayTextMisses < 8) {
                g_trayTextMisses++;
                Wh_Log(L"[tray] string %u requested by %s: not in the Windows 10 table", id, moduleName);
            }
            return result;
        }
        int length = (int)wcslen(text);
        if (length >= cch) length = cch - 1;
        memcpy(buffer, text, (size_t)length * sizeof(wchar_t));
        buffer[length] = 0;
        if (g_trayTextLogs < 6) {
            g_trayTextLogs++;
            Wh_Log(L"[tray] string %u served from the module table (%s, language %s: the .mui is not in the data folder)",
                   id, moduleName, TrayLangName());
        }
        return length;
    } catch (...) {
    }
    return result;
}

static void InstallTrayStrings() {
    if (Wh_SetFunctionHook((void*)LoadStringW, (void*)LoadStringW_Hook, (void**)&LoadStringW_Original))
        Wh_Log(L"[tray] tray module strings: Windows 10 MUI ready in place of the missing .mui "
               L"(%d stobject + %d pnidui strings in 15 languages, current language %s)",
               (int)_countof(kStobjectStrings), (int)_countof(kPniduiStrings), TrayLangName());
    else
        Wh_Log(L"[tray] tray module strings not served (hook not installed)");
}

// --- where the battery right-click menu comes from ----------------------------
// again from the disassembly: the battery menu is NOT a resource. stobject
// builds CreatePopupMenu + InsertMenuItemW/AppendMenuW and takes the entries from
// the registry:
//   HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Applets\SysTray\BattMeter\ContextMenu
// (numeric subkeys containing "ItemName"), while the flyout is defined in
//   ...\BattMeter\Flyout   (UseWin32BatteryFlyout = 0/1).
// If those keys do not exist (or are empty) the menu stays without entries: that is
// the first thing to rule out, and it was not in the log. This check writes into
// the log what really is on the machine.
static void LogBatteryMenuSource() {
    const wchar_t* paths[] = {
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Applets\\SysTray\\BattMeter\\ContextMenu",
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Applets\\SysTray\\BattMeter\\Flyout",
    };
    for (const wchar_t* path : paths) {
        ScopedRegKey key;
        LONG rc = RegOpenKeyExW(HKEY_LOCAL_MACHINE, path, 0, KEY_READ, key.put());
        if (rc != ERROR_SUCCESS) {
            Wh_Log(L"[battery] key missing (error %ld): %s", rc, path);
            continue;
        }

        DWORD subkeys = 0, values = 0;
        RegQueryInfoKeyW(key.get(), nullptr, nullptr, nullptr, &subkeys, nullptr, nullptr,
                         &values, nullptr, nullptr, nullptr, nullptr);
        Wh_Log(L"[battery] %s: %lu subkeys, %lu values", path, subkeys, values);

        for (DWORD i = 0; i < subkeys && i < 8; i++) {
            wchar_t name[128] = {};
            DWORD length = _countof(name);
            if (RegEnumKeyExW(key.get(), i, name, &length, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS)
                break;
            ScopedRegKey sub;
            if (RegOpenKeyExW(key.get(), name, 0, KEY_READ, sub.put()) == ERROR_SUCCESS) {
                wchar_t item[256] = {};
                DWORD bytes = sizeof(item), type = 0;
                if (RegQueryValueExW(sub.get(), L"ItemName", nullptr, &type, (LPBYTE)item, &bytes) == ERROR_SUCCESS)
                    Wh_Log(L"[battery]   entry %s: \"%s\"", name, item);
                else
                    Wh_Log(L"[battery]   entry %s: (no ItemName)", name);
            }
        }

        // This value is not read here either. It is the switch of the Windows 7 era
        // Win32 flyout of stobject.dll, not of the Windows 10 flyout this mod opens through
        // the shell experience manager.
    }
}

// --- the right-click menu: who really builds it --------------------------------
// it separates the two cases the log did not tell apart for the battery
// icon:
//   a) the click does not reach stobject -> NO TrackPopupMenu line appears;
//   b) the menu is shown but is empty (missing data: registry key or
//      MUI strings) -> the line with "0 items" appears.
// Both hooks are pass-through (they always call the original) and the log is capped.
typedef BOOL(WINAPI* TrackPopupMenuEx_t)(HMENU, UINT, int, int, HWND, LPTPMPARAMS);
static TrackPopupMenuEx_t TrackPopupMenuEx_Original = nullptr;
typedef BOOL(WINAPI* TrackPopupMenu_t)(HMENU, UINT, int, int, int, HWND, const RECT*);
static TrackPopupMenu_t TrackPopupMenu_Original = nullptr;
static int g_tpmLogs = 0;

// Defined further below (in the caller-logging block): only the signature is needed here.
static void LogCallerModule(void* caller, wchar_t* buf, size_t count);

// The real caller is not WhReturnAddress(): that one points inside our own
// module (at the hook trampoline). The stack is walked and the first
// frame that does not belong to the mod is taken: an earlier revision wrote the
// module of the mod itself in that column, which made every caller look local.
static const wchar_t* RealCallerModule(wchar_t* buf, size_t count) {
    buf[0] = 0;
    void* frames[8] = {};
    const USHORT total = CaptureStackBackTrace(0, (DWORD)_countof(frames), frames, nullptr);
    HMODULE self = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&RealCallerModule, &self);
    for (USHORT i = 0; i < total; i++) {
        HMODULE m = nullptr;
        if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                (LPCWSTR)frames[i], &m))
            continue;
        if (m == self) continue;
        LogCallerModule(frames[i], buf, count);
        break;
    }
    return buf;
}

// Fallback for the battery menu. The native path is the one of the MUI
// strings (ids 150/151 served above); this covers the case where the
// request for the strings arrives another way and the menu stays empty: the
// entries are the same stobject's code adds by itself (commands 101
// and 102, which stobject can handle), so the menu does not stay empty.
static int g_batteryFillLogs = 0;

static bool FillEmptyBatteryMenu(HMENU menu, HWND owner) {
    if (!menu || !owner) return false;
    if (GetMenuItemCount(menu) != 0) return false;
    const wchar_t* ownerModule = TrayOwnerModuleOfWindow(owner);
    if (!ownerModule || _wcsicmp(ownerModule, L"stobject.dll") != 0) return false;

    // Safety net: if the WM_CONTEXTMENU interception did not fire (subclass
    // not installed yet, different build path) we still populate the menu
    // with the two entries in the user's language.
    const UiLangId lang = DetectUiLang();
    AppendMenuW(menu, MF_STRING, 101, kBatteryPowerOptions[lang]);
    AppendMenuW(menu, MF_STRING, 102, kBatteryMobilityCenter[lang]);
    if (g_batteryFillLogs < 3) {
        g_batteryFillLogs++;
        Wh_Log(L"[battery] empty context menu: added two multilang entries (\"%s\", \"%s\")",
               kBatteryPowerOptions[lang], kBatteryMobilityCenter[lang]);
    }
    return true;
}

static void LogPopupMenuCall(const wchar_t* api, HMENU menu, HWND owner) {
    if (g_tpmLogs >= 3) return;
    const wchar_t* ownerModule = owner ? TrayOwnerModuleOfWindow(owner) : nullptr;
    const int count = menu ? GetMenuItemCount(menu) : -1;
    // The menus of the tray modules are logged and, in any case, the empty ones:
    // those explain "the right click does nothing".
    if (!ownerModule && count != 0) return;

    g_tpmLogs++;
    wchar_t cls[64] = {};
    if (owner) GetClassNameW(owner, cls, _countof(cls));
    wchar_t who[64] = {};
    RealCallerModule(who, _countof(who));
    Wh_Log(L"[tray] %s: %d items, caller %s, window %s (%s)", api, count,
           who[0] ? who : L"unknown module", cls[0] ? cls : L"no class",
           ownerModule ? ownerModule : L"not a tray window");
}

// The battery menu stays the Windows 10 one: only the power
// entries (commands 101 and 102 of stobject). The classic window-arrangement
// entries are only in the taskbar menu.
//
// These two hooks are global - every popup menu of the process passes here - so they
// only complete the battery menu and log it. The chosen command is NOT post-processed any
// more: the ids this mod invents are handled by the menus that show them (ShowShowDesktopMenu
// calls HandleClassicMenuCommand itself, ShowBatteryMenu maps its own two entries onto
// stobject's power commands). Claiming the ids here returned FALSE for a command of stobject
// that was never executed, which killed "Power Options" and "Windows Mobility Center" for
// everybody, and it would have swallowed the same numeric id in any other shell menu.
static BOOL WINAPI TrackPopupMenuEx_Hook(HMENU menu, UINT flags, int x, int y, HWND owner,
                                         LPTPMPARAMS params) {
    try {
        FillEmptyBatteryMenu(menu, owner);
        LogPopupMenuCall(L"TrackPopupMenuEx", menu, owner);
    } catch (...) {
    }
    return TrackPopupMenuEx_Original(menu, flags, x, y, owner, params);
}

static BOOL WINAPI TrackPopupMenu_Hook(HMENU menu, UINT flags, int x, int y, int reserved,
                                       HWND owner, const RECT* rect) {
    try {
        FillEmptyBatteryMenu(menu, owner);
        LogPopupMenuCall(L"TrackPopupMenu", menu, owner);
    } catch (...) {
    }
    return TrackPopupMenu_Original(menu, flags, x, y, reserved, owner, rect);
}

static bool g_trackPopupMenuExHooked = false;
static bool g_trackPopupMenuHooked = false;

static void InstallPopupMenuTrace() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) return;
    void* ex = (void*)GetProcAddress(user32, "TrackPopupMenuEx");
    void* plain = (void*)GetProcAddress(user32, "TrackPopupMenu");
    if (!g_trackPopupMenuExHooked && ex)
        g_trackPopupMenuExHooked = Wh_SetFunctionHook(ex, (void*)TrackPopupMenuEx_Hook,
                                                       (void**)&TrackPopupMenuEx_Original);
    if (!g_trackPopupMenuHooked && plain)
        g_trackPopupMenuHooked = Wh_SetFunctionHook(plain, (void*)TrackPopupMenu_Hook,
                                                     (void**)&TrackPopupMenu_Original);
    if (g_trackPopupMenuExHooked || g_trackPopupMenuHooked)
        Wh_Log(L"[tray] popup menu supervision active (TrackPopupMenuEx %s, TrackPopupMenu %s)",
               g_trackPopupMenuExHooked ? L"yes" : L"no",
               g_trackPopupMenuHooked ? L"yes" : L"no");
    else
        Wh_Log(L"[tray] popup menu supervision not installed");
}

// ===========================================================================
// LEGACY TRAY: network and volume icons in the Windows 10 shell
//
// An early OLECMDID_NEW attempt did not make the icon appear, and a static URI-click
// shim was withdrawn. What is tried instead asks the native Shell Service Object path to
// start PNI, and only creates a dynamic NLM icon if a real PNIHiddenWnd callback has been
// observed. Shell_NotifyIconW tracing stays diagnostic; no independent click target is
// fabricated.
// ===========================================================================

static const IID kIidClassFactory = {0x00000001, 0x0000, 0x0000,
                                     {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

static bool TrayRedirectTarget(const wchar_t* moduleName, std::wstring& target) {
    if (!moduleName || !*moduleName) return false;
    // With the tray modules switched off the Windows 10 copies must not be used by
    // anything either - the setting used to stop the download only, and a store that was
    // filled earlier kept having its DLLs loaded and every load of those names redirected.
    if (!g_cfg.provideTrayDlls) return false;

    const wchar_t* base = moduleName;
    for (const wchar_t* p = moduleName; *p; p++)
        if (*p == L'\\' || *p == L'/') base = p + 1;

    const bool wanted = _wcsicmp(base, L"pnidui.dll") == 0 || _wcsicmp(base, L"stobject.dll") == 0;
    if (!wanted) return false;

    target = std::wstring(g_cfg.storePath) + L"\\" + base;
    if (_wcsicmp(moduleName, target.c_str()) == 0) return false;   // it is already ours
    return GetFileAttributesW(target.c_str()) != INVALID_FILE_ATTRIBUTES;
}

typedef HMODULE(WINAPI* LoadLibraryW_t)(LPCWSTR);
static LoadLibraryW_t LoadLibraryW_Original = nullptr;

typedef HMODULE(WINAPI* LoadLibraryExW_t)(LPCWSTR, HANDLE, DWORD);
static LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

static HMODULE WINAPI LoadLibraryW_Hook(LPCWSTR name) {
    try {
        std::wstring target;
        if (TrayRedirectTarget(name, target)) {
            HMODULE module = LoadLibraryW_Original(target.c_str());
            if (module) {
                Wh_Log(L"[tray] %s -> %s", name, target.c_str());
                return module;
            }
        }
    } catch (...) {
    }
    return LoadLibraryW_Original(name);
}

static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    try {
        std::wstring target;
        if (TrayRedirectTarget(name, target)) {
            HMODULE module = LoadLibraryExW_Original(target.c_str(), nullptr, flags);
            if (module) {
                Wh_Log(L"[tray] %s -> %s", name, target.c_str());
                return module;
            }
        }
    } catch (...) {
    }
    return LoadLibraryExW_Original(name, file, flags);
}

// ntdll: it catches loads that do not go through the kernelbase functions.
typedef LONG(NTAPI* LdrLoadDll_t)(PWSTR, PULONG, UNICODE_STRING*, HMODULE*);
static LdrLoadDll_t LdrLoadDll_Original = nullptr;
typedef VOID(NTAPI* RtlInitUnicodeString_t)(UNICODE_STRING*, PCWSTR);
static RtlInitUnicodeString_t RtlInitUnicodeString_Original = nullptr;

static LONG NTAPI LdrLoadDll_Hook(PWSTR searchPath, PULONG flags, UNICODE_STRING* name, HMODULE* handle) {
    try {
        if (name && name->Buffer) {
            std::wstring asked(name->Buffer, name->Length / sizeof(wchar_t));
            std::wstring target;
            if (TrayRedirectTarget(asked.c_str(), target)) {
                UNICODE_STRING redirected = {};
                RtlInitUnicodeString_Original(&redirected, target.c_str());
                LONG status = LdrLoadDll_Original(searchPath, flags, &redirected, handle);
                if (status >= 0) {
                    Wh_Log(L"[tray] (ntdll) %s -> %s", asked.c_str(), target.c_str());
                    return status;
                }
            }
        }
    } catch (...) {
    }
    return LdrLoadDll_Original(searchPath, flags, name, handle);
}

typedef HRESULT(STDAPICALLTYPE* DllGetClassObject_t)(REFCLSID, REFIID, void**);
typedef HRESULT(WINAPI* CoCreateInstance_t)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
static CoCreateInstance_t CoCreateInstance_Original = nullptr;

static volatile LONG g_shimProbes = 0;
static HMODULE FindPrivateTrayModule(const wchar_t* moduleName) noexcept;

static bool GetLegacyClassFactory(REFCLSID clsid, winrt::com_ptr<IClassFactory>& factory) noexcept {
    try {
        const wchar_t* modules[] = { L"pnidui.dll", L"stobject.dll" };
        for (const wchar_t* name : modules) {
            HMODULE module = FindPrivateTrayModule(name);
            if (!module) continue;
            auto getClassObject = (DllGetClassObject_t)GetProcAddress(module, "DllGetClassObject");
            if (!getClassObject) continue;

            winrt::com_ptr<IClassFactory> candidate;
            const HRESULT hr = getClassObject(clsid, kIidClassFactory, candidate.put_void());
            if (SUCCEEDED(hr) && candidate.get()) {
                factory = std::move(candidate);
                return true;
            }
        }
    } catch (...) {
        Wh_Log(L"[tray] legacy class-factory lookup exception");
    }
    return false;
}

static bool IsPrivateExplorerProcess();

static const CLSID kNetworkTraySsoClsid = {
    0xC2796011, 0x81BA, 0x4148, { 0x8F, 0xCA, 0xC6, 0x64, 0x32, 0x45, 0x11, 0x3F }
};
static std::atomic<bool> g_networkSsoActivationSeen{false};
static std::atomic<ULONGLONG> g_networkSsoActivationObservedAt{0};

static bool GetPniduiClassFactory(REFCLSID clsid,
                                  winrt::com_ptr<IClassFactory>& factory) noexcept {
    try {
        HMODULE module = FindPrivateTrayModule(L"pnidui.dll");
        if (!module) return false;
        auto getClassObject = (DllGetClassObject_t)GetProcAddress(module, "DllGetClassObject");
        if (!getClassObject) return false;
        winrt::com_ptr<IClassFactory> candidate;
        const HRESULT hr = getClassObject(clsid, kIidClassFactory, candidate.put_void());
        if (FAILED(hr) || !candidate.get()) return false;
        factory = std::move(candidate);
        return true;
    } catch (...) {
        Wh_Log(L"[tray] pnidui class-factory lookup exception");
        return false;
    }
}

static void DescribeModuleAtAddress(const void* address, wchar_t* out, size_t cch) noexcept {
    if (!out || cch == 0) return;
    out[0] = L'\0';
    try {
        HMODULE module = nullptr;
        const DWORD flags = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
        if (!address || !GetModuleHandleExW(flags, (LPCWSTR)address, &module) || !module) {
            wcscpy_s(out, cch, L"sconosciuto");
            return;
        }
        wchar_t path[MAX_PATH] = {};
        if (!GetModuleFileNameW(module, path, _countof(path))) {
            wcscpy_s(out, cch, L"module-without-path");
            return;
        }
        const wchar_t* base = wcsrchr(path, L'\\');
        base = base ? base + 1 : path;
        wcsncpy_s(out, cch, base, _TRUNCATE);
    } catch (...) {
        wcscpy_s(out, cch, L"sconosciuto");
    }
}

static HRESULT WINAPI CoCreateInstance_Hook(REFCLSID clsid, LPUNKNOWN outer, DWORD context,
                                            REFIID iid, LPVOID* ppv) {
    const bool networkSso = IsEqualCLSID(clsid, kNetworkTraySsoClsid) != FALSE;
    if (networkSso && NativeUi::privateExplorer && g_cfg.provideTrayDlls) {
        if (!g_networkSsoActivationSeen.exchange(true))
            g_networkSsoActivationObservedAt.store(GetTickCount64());
        wchar_t caller[128] = {};
        DescribeModuleAtAddress(WhReturnAddress(), caller, _countof(caller));
        if (!ppv) return E_POINTER;
        *ppv = nullptr;
        try {
            winrt::com_ptr<IClassFactory> factory;
            if (GetPniduiClassFactory(clsid, factory)) {
                const HRESULT created = factory->CreateInstance(outer, iid, ppv);
                if (SUCCEEDED(created)) {
                    Wh_Log(L"[tray] Network Tray SSO routed to the pinned pnidui.dll factory: 0x%08X (caller %s)",
                           (unsigned)created, caller);
                    return created;
                }
                if (*ppv) {
                    ((IUnknown*)*ppv)->Release();
                    *ppv = nullptr;
                }
                Wh_Log(L"[tray] pinned pnidui.dll factory failed for Network Tray SSO: 0x%08X (caller %s); trying COM registration",
                       (unsigned)created, caller);
            } else {
                Wh_Log(L"[tray] Network Tray SSO requested by %s, but pnidui.dll has no usable class factory; trying COM registration",
                       caller);
            }
        } catch (...) {
            Wh_Log(L"[tray] Network Tray SSO pnidui activation raised a C++ exception (caller %s)", caller);
            if (*ppv) {
                ((IUnknown*)*ppv)->Release();
                *ppv = nullptr;
            }
        }
    }

    HRESULT hr;
    try {
        hr = CoCreateInstance_Original(clsid, outer, context, iid, ppv);
    } catch (...) {
        Wh_Log(L"[tray] original COM activation raised a C++ exception");
        return E_FAIL;  // Never activate twice after an exception.
    }
    if (networkSso) {
        if (NativeUi::privateExplorer && g_cfg.provideTrayDlls) {
            wchar_t caller[128] = {};
            DescribeModuleAtAddress(WhReturnAddress(), caller, _countof(caller));
            Wh_Log(L"[tray] Network Tray SSO original COM activation -> 0x%08X (caller %s)",
                   (unsigned)hr, caller);
        }
        return hr;
    }

    // The legacy DLL fallback must NEVER leak into native Explorer windows.
    if (!NativeUi::privateExplorer ||
        (hr != REGDB_E_CLASSNOTREG && hr != HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND))) return hr;
    if (InterlockedIncrement(&g_shimProbes) > 64) return hr;
    try {
        winrt::com_ptr<IClassFactory> factory;
        if (!GetLegacyClassFactory(clsid, factory)) return hr;
        HRESULT created = factory->CreateInstance(outer, iid, ppv);
        if (SUCCEEDED(created)) {
            wchar_t guid[64] = {};
            StringFromGUID2(clsid, guid, _countof(guid));
            Wh_Log(L"[tray] unregistered class created with the Windows 10 DLLs: %s", guid);
            return created;
        }
    } catch (...) { Wh_Log(L"[tray] legacy class factory C++ exception"); }
    return hr;
}
static bool EnsureCoCreateHook() noexcept {
    try {
        if (CoCreateInstance_Original) return true;
        return Wh_SetFunctionHook((void*)CoCreateInstance, (void*)CoCreateInstance_Hook,
                                  (void**)&CoCreateInstance_Original);
    } catch (...) { Wh_Log(L"[tray] COM hook installation exception"); return false; }
}

// The modules that draw the two icons of the restored tray: pnidui.dll is the
// network icon, stobject.dll the volume/battery one. They are loaded by this mod
// into the Windows 10 shell, so if they are not loaded the icons do not exist.
// Load pnidui before stobject so the exact Windows 10 SSO factory is available
// if stobject requests the network class during its own initialization.
static const wchar_t* const kTrayIconModules[] = { L"pnidui.dll", L"stobject.dll" };
static bool g_trayWrongPathLoadAttempted[_countof(kTrayIconModules)] = {};

static std::wstring ExpectedPrivateTrayModulePath(const wchar_t* moduleName) {
    std::wstring path = g_cfg.storePath;
    while (!path.empty() && (path.back() == L'\\' || path.back() == L'/'))
        path.pop_back();
    path += L"\\";
    path += moduleName;
    return path;
}

static bool GetTrayModulePath(HMODULE module, wchar_t* path, size_t cch) noexcept {
    if (!path || cch == 0) return false;
    path[0] = L'\0';
    if (!module || cch > MAXDWORD) return false;
    try {
        const DWORD length = GetModuleFileNameW(module, path, (DWORD)cch);
        if (!length || length >= cch) {
            path[0] = L'\0';
            return false;
        }
        return true;
    } catch (...) {
        path[0] = L'\0';
        return false;
    }
}

static bool IsPrivateTrayModulePath(HMODULE module, const wchar_t* moduleName) noexcept {
    try {
        if (!moduleName || !g_cfg.storePath[0]) return false;
        const std::wstring expected = ExpectedPrivateTrayModulePath(moduleName);
        wchar_t actual[MAX_PATH] = {};
        return GetTrayModulePath(module, actual, _countof(actual)) &&
               _wcsicmp(actual, expected.c_str()) == 0;
    } catch (...) {
        return false;
    }
}

// Find the exact pinned module, even if another DLL with the same basename was
// loaded earlier. The snapshot handle is RAII-owned; returned HMODULE is borrowed.
static HMODULE FindPrivateTrayModule(const wchar_t* moduleName) noexcept {
    try {
        if (!moduleName || !*moduleName || !g_cfg.storePath[0]) return nullptr;
        if (HMODULE byName = GetModuleHandleW(moduleName)) {
            if (IsPrivateTrayModulePath(byName, moduleName)) return byName;
        }

        HANDLE rawSnapshot = INVALID_HANDLE_VALUE;
        for (int attempt = 0; attempt < 3; ++attempt) {
            rawSnapshot = CreateToolhelp32Snapshot(
                TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
            if (rawSnapshot != INVALID_HANDLE_VALUE || GetLastError() != ERROR_BAD_LENGTH)
                break;
        }
        ScopedHandle snapshot(rawSnapshot);
        if (!snapshot.valid()) return nullptr;

        MODULEENTRY32W entry = {};
        entry.dwSize = sizeof(entry);
        if (!Module32FirstW(snapshot.get(), &entry)) return nullptr;
        do {
            if (_wcsicmp(entry.szModule, moduleName) == 0 &&
                IsPrivateTrayModulePath(entry.hModule, moduleName))
                return entry.hModule;
        } while (Module32NextW(snapshot.get(), &entry));
    } catch (...) {
    }
    return nullptr;
}

static bool VerifyPinnedTrayFile(const wchar_t* moduleName) noexcept {
    try {
        if (!moduleName) return false;
        const PinnedFile* pin = nullptr;
        for (const PinnedFile& candidate : kTrayFiles) {
            if (_wcsicmp(candidate.name, moduleName) == 0) {
                pin = &candidate;
                break;
            }
        }
        if (!pin) {
            Wh_Log(L"[tray] no SHA-256 pin is configured for %s", moduleName);
            return false;
        }
        const std::wstring path = ExpectedPrivateTrayModulePath(moduleName);
        std::wstring actualHash;
        if (!Sha256File(path.c_str(), actualHash)) return false;
        if (_wcsicmp(actualHash.c_str(), pin->sha256) != 0) {
            Wh_Log(L"[tray] refusing SSO activation: %s hash mismatch (expected %s, got %s)",
                   moduleName, pin->sha256, actualHash.c_str());
            return false;
        }
        return true;
    } catch (...) {
        Wh_Log(L"[tray] exception verifying the pinned hash for %s", moduleName);
        return false;
    }
}

static bool AnyTrayIconModuleMissing() {
    for (const wchar_t* name : kTrayIconModules)
        if (!FindPrivateTrayModule(name)) return true;
    return false;
}

// What the data folder really contains. Without this line "the icon does not
// appear" has no visible cause in the log.
static void ReportTrayStoreFile(const wchar_t* name) {
    std::wstring full = std::wstring(g_cfg.storePath) + L"\\" + name;
    WIN32_FILE_ATTRIBUTE_DATA fad = {};
    if (GetFileAttributesExW(full.c_str(), GetFileExInfoStandard, &fad)) {
        Wh_Log(L"[tray] data folder: %s present (%lu bytes)", name,
               (unsigned long)fad.nFileSizeLow);
    } else {
        Wh_Log(L"[tray] data folder: %s MISSING (%lu): the related icon cannot be "
               L"created in this session", name, GetLastError());
    }
}

static void LoadTrayModules(const wchar_t* why) {
    try {
        if (!g_cfg.provideTrayDlls) return;   // see TrayRedirectTarget
        for (size_t i = 0; i < _countof(kTrayIconModules); ++i) {
            const wchar_t* name = kTrayIconModules[i];
            if (FindPrivateTrayModule(name)) continue;

            HMODULE existing = GetModuleHandleW(name);
            if (existing && g_trayWrongPathLoadAttempted[i]) continue;

            const std::wstring full = ExpectedPrivateTrayModulePath(name);
            if (existing) {
                wchar_t existingPath[MAX_PATH] = {};
                if (GetTrayModulePath(existing, existingPath, _countof(existingPath)) &&
                    _wcsicmp(existingPath, full.c_str()) != 0) {
                    Wh_Log(L"[tray] %s already mapped from %s; requesting the pinned private copy",
                           name, existingPath);
                }
            }

            HMODULE module = LoadLibraryW_Original ? LoadLibraryW_Original(full.c_str())
                                                   : LoadLibraryW(full.c_str());
            if (module && IsPrivateTrayModulePath(module, name)) {
                Wh_Log(L"[tray] %s loaded from the private pinned path (%s)", name, why);
            } else if (module) {
                wchar_t actualPath[MAX_PATH] = {};
                GetTrayModulePath(module, actualPath, _countof(actualPath));
                g_trayWrongPathLoadAttempted[i] = true;
                Wh_Log(L"[tray] %s request did not yield the private file (mapped %s); native SSO will wait",
                       name, actualPath[0] ? actualPath : L"path unavailable");
            } else {
                Wh_Log(L"[tray] %s not loaded (%lu)", name, GetLastError());
            }
        }
    } catch (...) {
        Wh_Log(L"[tray] exception while loading the private pinned tray modules");
    }
}

static ULONGLONG g_nextNetworkSsoEnableTry = 0;
static ULONGLONG g_nextNetworkPniWindowScan = 0;
static ULONGLONG g_nextNetworkSsoModuleScan = 0;
static ULONGLONG g_networkSsoEnableAcceptedAt = 0;
static HMODULE g_privatePniduiForSso = nullptr;
static HMODULE g_privateStobjectForSso = nullptr;
static unsigned g_networkSsoEnableAttempts = 0;
static bool g_networkSsoModulesWaitLogged = false;
static bool g_networkSsoModulesReadyLogged = false;
static bool g_networkSsoPinsChecked = false;
static bool g_networkSsoPinsRejected = false;
static bool g_networkSsoEnableAccepted = false;
static bool g_networkSsoEnableUnavailableLogged = false;
static bool g_networkSsoEnableGiveUpLogged = false;
static bool g_networkSsoWaitLogged = false;

// Shell32's legacy SHEnableServiceObject export asks the current taskbar to
// enable an SSO. Do not treat a same-named System32 DLL as the pinned module:
// the log showed the private stobject redirect arriving after the old S_OK.
// The current candidate pins stobject 10.0.19041.7664 (SHA-256 in kTrayFiles); the
// Windows-To-Go-slot rewrite used by the separate 26100 project is not applied:
// this exact file has no Windows-To-Go sentinel and already contains the Network
// tray SSO CLSID. We only enable it after both exact private DLLs are mapped.
typedef HRESULT (WINAPI* SHEnableServiceObject_t)(REFCLSID, BOOL);
static void EnsureNativeNetworkTraySso() noexcept {
    try {
        if (!NativeUi::privateExplorer || !g_cfg.provideTrayDlls || g_unloading.load()) return;

        const ULONGLONG now = GetTickCount64();
        if (!g_privatePniduiForSso || !g_privateStobjectForSso ||
            now >= g_nextNetworkSsoModuleScan) {
            g_nextNetworkSsoModuleScan = now + 1000;
            g_privatePniduiForSso = FindPrivateTrayModule(L"pnidui.dll");
            g_privateStobjectForSso = FindPrivateTrayModule(L"stobject.dll");
        }
        if (!g_privatePniduiForSso || !g_privateStobjectForSso) {
            if (!g_networkSsoModulesWaitLogged) {
                g_networkSsoModulesWaitLogged = true;
                HMODULE anyPnidui = GetModuleHandleW(L"pnidui.dll");
                HMODULE anyStobject = GetModuleHandleW(L"stobject.dll");
                wchar_t pniPath[MAX_PATH] = {};
                wchar_t stPath[MAX_PATH] = {};
                if (anyPnidui) GetTrayModulePath(anyPnidui, pniPath, _countof(pniPath));
                if (anyStobject) GetTrayModulePath(anyStobject, stPath, _countof(stPath));
                Wh_Log(L"[tray] SSO bootstrap waiting for the pinned private tray DLLs (pnidui=%s, stobject=%s)",
                       pniPath[0] ? pniPath : L"not loaded",
                       stPath[0] ? stPath : L"not loaded");
            }
            return;
        }
        if (!g_networkSsoModulesReadyLogged) {
            g_networkSsoModulesReadyLogged = true;
            wchar_t pniPath[MAX_PATH] = {};
            wchar_t stPath[MAX_PATH] = {};
            GetTrayModulePath(g_privatePniduiForSso, pniPath, _countof(pniPath));
            GetTrayModulePath(g_privateStobjectForSso, stPath, _countof(stPath));
            Wh_Log(L"[tray] SSO bootstrap verified private DLL paths: pnidui=%s; stobject=%s",
                   pniPath, stPath);
        }
        if (!g_networkSsoPinsChecked) {
            g_networkSsoPinsChecked = true;
            const bool pniPinned = VerifyPinnedTrayFile(L"pnidui.dll");
            const bool stPinned = VerifyPinnedTrayFile(L"stobject.dll");
            g_networkSsoPinsRejected = !pniPinned || !stPinned;
            if (!g_networkSsoPinsRejected)
                Wh_Log(L"[tray] private pnidui.dll and stobject.dll match their pinned SHA-256 values");
        }
        if (g_networkSsoPinsRejected) return;

        NetworkPniRegistration registration = {};
        // The forced icon must not stop the native attempt: the SSO keeps being asked
        // for, so the real PNI can still take over.
        if (CopyNetworkPniRegistration(&registration) && !registration.forced) return;
        if (now >= g_nextNetworkPniWindowScan) {
            g_nextNetworkPniWindowScan = now + 2000;
            if (HWND pni = FindPniHiddenWindow()) {
                static bool reported = false;
                if (!reported) {
                    reported = true;
                    Wh_Log(L"[tray] PNIHiddenWnd found at 0x%p, but no qualifying PNI callback registration has been captured", pni);
                }
            }
        }

        const bool activationSeen = g_networkSsoActivationSeen.load();
        const ULONGLONG activationObservedAt = g_networkSsoActivationObservedAt.load();
        if (activationSeen && activationObservedAt && now - activationObservedAt < 15000) return;
        if (g_networkSsoEnableAccepted) {
            if (!g_networkSsoWaitLogged && g_networkSsoEnableAcceptedAt &&
                now - g_networkSsoEnableAcceptedAt >= 15000) {
                g_networkSsoWaitLogged = true;
                Wh_Log(L"[tray] SHEnableServiceObject on the pinned private DLLs returned S_OK, but no PNIHiddenWnd/NIM_ADD followed after 15 s; no URI substitute is used. The mod escalation ladder registers the icon in place of the native PNI");
            }
            return;
        }
        if (g_networkSsoEnableAttempts >= 6) {
            if (!g_networkSsoEnableGiveUpLogged) {
                g_networkSsoEnableGiveUpLogged = true;
                Wh_Log(L"[tray] native Network Tray SSO enable did not succeed after 6 attempts; no URI click substitute will be used");
            }
            return;
        }
        if (now < g_nextNetworkSsoEnableTry) return;
        g_nextNetworkSsoEnableTry = now + 5000;
        ++g_networkSsoEnableAttempts;

        HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
        auto enable = shell32 ? (SHEnableServiceObject_t)GetProcAddress(shell32, "SHEnableServiceObject") : nullptr;
        if (!enable) {
            if (!g_networkSsoEnableUnavailableLogged) {
                g_networkSsoEnableUnavailableLogged = true;
                Wh_Log(L"[tray] shell32 does not export SHEnableServiceObject; native SSO bootstrap unavailable");
            }
            g_networkSsoEnableAttempts = 6;
            g_networkSsoEnableGiveUpLogged = true;
            return;
        }

        const HRESULT hr = enable(kNetworkTraySsoClsid, TRUE);
        Wh_Log(L"[tray] SHEnableServiceObject(Network Tray SSO) attempt %u -> 0x%08X",
               g_networkSsoEnableAttempts, (unsigned)hr);
        if (SUCCEEDED(hr)) {
            g_networkSsoEnableAccepted = true;
            g_networkSsoEnableAcceptedAt = now;
        }
    } catch (...) {
        Wh_Log(L"[tray] exception while enabling the native Network Tray SSO");
    }
}

// The hook part of the tray support, split out so that it is registered in
// Wh_ModInit, where the Windhawk API wants every hook to be set - the engine applies the
// queue automatically right after Wh_ModInit returns. Everything that needs a running shell
// stays in InstallTraySupport below, on the mod's own thread. The registrations carry their
// own once-only guard, so calling the hook part from either place is harmless: Wh_ModInit
// gets there first.
static void InstallTraySupportHooks() {
    static bool done = false;
    if (done) return;
    done = true;
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (kernelBase) {
        void* target = (void*)GetProcAddress(kernelBase, "LoadLibraryW");
        if (target) Wh_SetFunctionHook(target, (void*)LoadLibraryW_Hook, (void**)&LoadLibraryW_Original);
        target = (void*)GetProcAddress(kernelBase, "LoadLibraryExW");
        if (target) Wh_SetFunctionHook(target, (void*)LoadLibraryExW_Hook, (void**)&LoadLibraryExW_Original);
    }

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (ntdll) {
        LdrLoadDll_Original = (LdrLoadDll_t)GetProcAddress(ntdll, "LdrLoadDll");
        RtlInitUnicodeString_Original = (RtlInitUnicodeString_t)GetProcAddress(ntdll, "RtlInitUnicodeString");
        if (LdrLoadDll_Original && RtlInitUnicodeString_Original)
            Wh_SetFunctionHook((void*)LdrLoadDll_Original, (void*)LdrLoadDll_Hook, (void**)&LdrLoadDll_Original);
    }

    if (EnsureCoCreateHook())
        Wh_Log(L"[tray] network icon support active");
    else
        Wh_Log(L"[tray] tray support not installed");

    // The strings (and therefore the menus) of the tray modules come from their .mui,
    // which is not in the data folder: the Windows 10 one is served instead.
    InstallTrayStrings();
    InstallPopupMenuTrace();

    // Observe icon registrations before loading the private DLLs, so the log
    // records whether pnidui actually submits a native NIM_ADD in this process.
    InstallNetworkIconTrace();
}

static void InstallTraySupport() {
    g_traySupportInstalled = true;
    InstallTraySupportHooks();   // already registered in Wh_ModInit: a no-op here

    LogBatteryMenuSource();

    for (const wchar_t* name : kTrayIconModules) ReportTrayStoreFile(name);

    // The DLLs are loaded at once: the tray creates the icons at shell
    // start-up, so they must be ready before, not after.
    LoadTrayModules(L"at shell start");
    if (AnyTrayIconModuleMissing()) {
        Wh_Log(L"[tray] a tray module is missing: the network / volume icon will not be "
               L"created in this session unless the file appears. If the data folder is "
               L"empty, run the mod once from the Windows 11 shell with internet access: "
               L"the file is downloaded and verified there");
    }

}

// The icons are created at shell start-up: if a module was not in the data folder
// yet (the folder is filled by the native shell, which can be working at the same
// time), giving up would leave the icon missing for the whole session. The load is
// retried for a minute and every attempt is written to the log.
static void RetryTrayModulesIfNeeded() {
    if (!g_traySupportInstalled || g_unloading) return;
    static ULONGLONG nextTry = 0;
    static ULONGLONG giveUpAt = 0;
    const ULONGLONG now = GetTickCount64();

    if (!AnyTrayIconModuleMissing()) {
        if (giveUpAt) {
            Wh_Log(L"[tray] tray modules available again: the network / volume icons "
                   L"can be created now");
        }
        nextTry = 0;
        giveUpAt = 0;
        return;
    }
    if (!giveUpAt) giveUpAt = now + 60000;
    if (now > giveUpAt || now < nextTry) return;
    nextTry = now + 10000;
    LoadTrayModules(L"recupero differito");
}

static void** FindIatSlot(HMODULE module, const char* dllHint, const char* funcHint) {
#ifdef _WIN64
    if (!module || !dllHint || !funcHint) return nullptr;
    BYTE* base = (BYTE*)module;
    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)base;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return nullptr;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return nullptr;
    DWORD rva = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if (!rva) return nullptr;

    size_t hintLen = strlen(dllHint);
    for (PIMAGE_IMPORT_DESCRIPTOR imp = (PIMAGE_IMPORT_DESCRIPTOR)(base + rva); imp->Name; imp++) {
        const char* dll = (const char*)(base + imp->Name);
        bool dllOk = false;
        for (const char* p = dll; *p && !dllOk; p++)
            dllOk = _strnicmp(p, dllHint, hintLen) == 0;
        if (!dllOk) continue;

        PIMAGE_THUNK_DATA oft = (PIMAGE_THUNK_DATA)(base + (imp->OriginalFirstThunk ? imp->OriginalFirstThunk
                                                                                    : imp->FirstThunk));
        PIMAGE_THUNK_DATA ft = (PIMAGE_THUNK_DATA)(base + imp->FirstThunk);
        for (; oft->u1.AddressOfData; oft++, ft++) {
            if (oft->u1.Ordinal & IMAGE_ORDINAL_FLAG) continue;
            const char* name = (const char*)(base + oft->u1.AddressOfData + sizeof(WORD));
            if (_stricmp(name, funcHint) == 0) return (void**)&ft->u1.Function;
        }
    }
#else
    (void)module; (void)dllHint; (void)funcHint;
#endif
    return nullptr;
}

// The image name of the module that made a call: the columns of the log that name a
// caller use it.
static void LogCallerModule(void* caller, wchar_t* buf, size_t count) {
    buf[0] = 0;
    if (!caller) return;
    HMODULE mod = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCWSTR)caller, &mod) || !mod)
        return;
    wchar_t path[MAX_PATH] = {};
    GetModuleFileNameW(mod, path, _countof(path));
    const wchar_t* base = wcsrchr(path, L'\\');
    wcsncpy_s(buf, count, base ? base + 1 : path, _TRUNCATE);
}

// OpenSettingsPage and ToggleWifiRadio are gone. Both were unused, and the first one
// launched ms-settings: pages through the original ShellExecuteW, i.e. around the hooks.

namespace NetworkTrayForce {

static constexpr wchar_t kTrayNotifyKey[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\TrayNotify";
static constexpr wchar_t kOwnerClass[] = L"Win10Restorer_NetworkIconWnd";
static constexpr UINT kCallbackMessage = WM_APP + 0x51;
static constexpr UINT kIconId = 1;
static constexpr UINT kMsgQuitOwner = WM_APP + 0x52;
static constexpr int kVerifyIntervalMs = 5000;
static constexpr int kMaxRetryLog = 6;

static std::atomic<bool> g_stopping{false};
static std::atomic<bool> g_reinstallRequested{false};
static HWND g_ownerWindow = nullptr;
static HANDLE g_ownerThread = nullptr;
static DWORD g_ownerThreadId = 0;
static UINT g_taskbarCreatedMessage = 0;
static std::mutex g_ownerLock;
static bool g_guaranteesApplied = false;
static bool g_forcedPublished = false;
static bool g_retiredLogged = false;
static bool g_verifiedLogged = false;
static bool g_ownerRunning = false;
static bool g_taskbarWasUp = false;
static ULONGLONG g_firstTick = 0;
static ULONGLONG g_nextVerify = 0;
static ULONGLONG g_nextGuaranteeTry = 0;
static int g_verifyFailures = 0;
static int g_publishCount = 0;

// State of the authentic flyout. At start-up the mod does not touch the bar: the possible
// reset of TrayNotify fires only if, after the wait, the forced icon has not appeared
// (the "notification overflow at start-up" behaviour is unchanged).
static std::atomic<bool> g_escalate{false};
static int g_clickLogNotes = 0;
static ULONGLONG g_lastClickTick = 0;

static int ClampValue(int value, int low, int high) noexcept {
    return value < low ? low : (value > high ? high : value);
}

// ------------------------------------------------------------- registro ----
static void NudgeTray() noexcept {
    try {
        DWORD_PTR ignored = 0;
        SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                            reinterpret_cast<LPARAM>(L"TraySettings"),
                            SMTO_ABORTIFHUNG | SMTO_NORMAL, 400, &ignored);
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (tray)
            SendMessageTimeoutW(tray, WM_SETTINGCHANGE, 0,
                                reinterpret_cast<LPARAM>(L"TraySettings"),
                                SMTO_ABORTIFHUNG | SMTO_NORMAL, 400, &ignored);
    } catch (...) {
    }
}

static bool PublishForcedRegistration() noexcept;   // defined below

// The two files this mod writes for itself (the tray-state backup and the marker that
// says the reset already ran) are not part of the shared binary store: they belong to this
// mod's own storage, the folder Windhawk removes together with the mod. The store folder is
// shared with the Windows 10 taskbar mod and survives an uninstall, so it is used only when
// the mod storage is not available.
static bool ResolveStateDir(wchar_t* out, size_t count) noexcept {
    try {
        wchar_t storage[MAX_PATH] = {};
        if (Wh_GetModStoragePath(storage, _countof(storage)) && storage[0] &&
            EnsureDirectory(storage)) {
            wcsncpy_s(out, count, storage, _TRUNCATE);
            return true;
        }
    } catch (...) {
        Wh_Log(L"[tray-force] exception while resolving this mod's storage folder");
    }
    if (!g_cfg.storePath[0]) return false;
    wcsncpy_s(out, count, g_cfg.storePath, _TRUNCATE);
    return true;
}

// Backup of the binary state of the notification area and its deletion, once. Fail closed:
// every value that exists must be read completely as REG_BINARY and the complete backup plus
// the one-time marker must be durable before either value is removed.
static bool BackupAndResetTrayValuesOnce() noexcept {
    try {
        const wchar_t* kValues[] = { L"IconStreams", L"PastIconsStream" };
        ScopedHKey key;
        const LONG openStatus = RegOpenKeyExW(HKEY_CURRENT_USER, kTrayNotifyKey, 0,
                                               KEY_READ | KEY_SET_VALUE, key.receive());
        if (openStatus == ERROR_FILE_NOT_FOUND) return true;  // no state to reset
        if (openStatus != ERROR_SUCCESS || !key.valid()) {
            Wh_Log(L"[tray-force] TrayNotify could not be opened (%ld): no values are deleted",
                   openStatus);
            return false;
        }

        wchar_t stateDir[MAX_PATH] = {};
        if (!ResolveStateDir(stateDir, _countof(stateDir))) {
            Wh_Log(L"[tray-force] no folder for this mod's state files: the tray state is left "
                   L"as it is");
            return false;
        }

        wchar_t marker[MAX_PATH] = {};
        wchar_t backup[MAX_PATH] = {};
        if (_snwprintf_s(marker, _countof(marker), _TRUNCATE,
                         L"%s\\tray-state-reset.done", stateDir) < 0 || !marker[0] ||
            _snwprintf_s(backup, _countof(backup), _TRUNCATE,
                         L"%s\\tray-state-backup.bin", stateDir) < 0 || !backup[0]) {
            Wh_Log(L"[tray-force] state-file path is too long: the tray state is left as it is");
            return false;
        }
        if (GetFileAttributesW(marker) != INVALID_FILE_ATTRIBUTES) return true;

        // Keep the existing binary format: UTF-16 value name (including NUL), DWORD byte
        // count, then the exact REG_BINARY bytes. Registry query sizes are probed first and
        // rechecked on the data read, as required by RegQueryValueExW's size contract.
        static constexpr DWORD kMaxSavedValueBytes = 16 * 1024 * 1024;
        std::vector<BYTE> dump;
        bool anyValue = false;
        for (const wchar_t* value : kValues) {
            DWORD type = 0, size = 0;
            const LONG probe = RegQueryValueExW(key.get(), value, nullptr, &type, nullptr, &size);
            if (probe == ERROR_FILE_NOT_FOUND) continue;
            if (probe != ERROR_SUCCESS) {
                Wh_Log(L"[tray-force] could not size TrayNotify\\%s (%ld): no values are deleted",
                       value, probe);
                return false;
            }
            if (type != REG_BINARY || size > kMaxSavedValueBytes) {
                Wh_Log(L"[tray-force] TrayNotify\\%s is not a supported REG_BINARY value "
                       L"(type %lu, %lu bytes): no values are deleted",
                       value, type, size);
                return false;
            }

            std::vector<BYTE> data(size);
            DWORD actualType = type;
            DWORD actualSize = size;
            const LONG read = RegQueryValueExW(key.get(), value, nullptr, &actualType,
                                               size ? data.data() : nullptr, &actualSize);
            if (read != ERROR_SUCCESS || actualType != REG_BINARY || actualSize != size) {
                Wh_Log(L"[tray-force] TrayNotify\\%s changed or could not be read fully "
                       L"(%ld): no values are deleted",
                       value, read);
                return false;
            }

            const size_t nameChars = wcslen(value) + 1;
            const BYTE* name = reinterpret_cast<const BYTE*>(value);
            dump.insert(dump.end(), name, name + nameChars * sizeof(wchar_t));
            dump.insert(dump.end(), reinterpret_cast<const BYTE*>(&size),
                        reinterpret_cast<const BYTE*>(&size) + sizeof(size));
            dump.insert(dump.end(), data.begin(), data.end());
            anyValue = true;
        }

        if (anyValue) {
            // CREATE_NEW preserves any older backup rather than silently overwriting it.
            bool backupWritten = false;
            {
                ScopedHandle file(CreateFileW(backup, GENERIC_WRITE, 0, nullptr, CREATE_NEW,
                                              FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                                              nullptr));
                if (!file.valid()) {
                    Wh_Log(L"[tray-force] tray state backup could not be created (%lu): "
                           L"no clearing", GetLastError());
                    return false;
                }
                DWORD written = 0;
                backupWritten = WriteFile(file.get(), dump.data(),
                                          static_cast<DWORD>(dump.size()), &written, nullptr) &&
                                written == dump.size() && FlushFileBuffers(file.get());
            }
            if (!backupWritten) {
                // This file was created by this attempt, and the handle is closed now.
                // Remove the incomplete copy so a later retry can make a fresh backup.
                DeleteFileW(backup);
                Wh_Log(L"[tray-force] incomplete or unflushed backup: no clearing");
                return false;
            }
            Wh_Log(L"[tray-force] tray state saved to %s (%lu bytes)", backup,
                   static_cast<unsigned long>(dump.size()));
        }

        // Record the attempt before deletion. If Explorer or the host is terminated between
        // deletion and the next instruction, the next run cannot overwrite the only backup
        // or repeat the destructive reset. A marker-write failure also means no deletion.
        const char markerText[] = "Windhawk tray reset attempt; backup: tray-state-backup.bin\r\n";
        HANDLE markerHandle = CreateFileW(marker, GENERIC_WRITE, 0, nullptr, CREATE_NEW,
                                          FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, nullptr);
        if (markerHandle == INVALID_HANDLE_VALUE) {
            const DWORD error = GetLastError();
            if (error == ERROR_FILE_EXISTS || error == ERROR_ALREADY_EXISTS) return true;
            Wh_Log(L"[tray-force] one-time reset marker could not be created (%lu): "
                   L"no clearing", error);
            return false;
        }
        bool markerWritten = false;
        {
            ScopedHandle markerFile(markerHandle);
            DWORD written = 0;
            markerWritten = WriteFile(markerFile.get(), markerText,
                                      static_cast<DWORD>(sizeof(markerText) - 1),
                                      &written, nullptr) &&
                            written == sizeof(markerText) - 1 &&
                            FlushFileBuffers(markerFile.get());
        }
        if (!markerWritten) {
            DeleteFileW(marker);
            Wh_Log(L"[tray-force] one-time reset marker could not be flushed: no clearing");
            return false;
        }

        if (!anyValue) {
            Wh_Log(L"[tray-force] no saved TrayNotify binary values were present; no clearing "
                   L"was needed");
            return true;
        }

        int removed = 0;
        for (const wchar_t* value : kValues) {
            const LONG result = RegDeleteValueW(key.get(), value);
            if (result == ERROR_SUCCESS) {
                ++removed;
            } else if (result != ERROR_FILE_NOT_FOUND) {
                Wh_Log(L"[tray-force] could not delete TrayNotify\\%s (%ld); the complete "
                       L"backup is retained", value, result);
            }
        }
        if (removed == 0) {
            Wh_Log(L"[tray-force] tray state was not cleared; the complete backup and attempt "
                   L"marker are retained");
            return false;
        }
        Wh_Log(L"[tray-force] tray state cleared (%d values) after a complete backup; "
               L"Windows will rebuild the saved notification-area state", removed);
        return true;
    } catch (...) {
        Wh_Log(L"[tray-force] exception while backing up or resetting the tray state; "
               L"no further deletion is attempted");
        return false;
    }
}

static bool ApplyVisibilityGuarantees() noexcept {
    try {
        if (g_guaranteesApplied) return true;

        const DWORD now = GetTickCount();
        if (g_nextGuaranteeTry && now < g_nextGuaranteeTry) return false;

        bool ok = true;

        if (g_escalate.load(std::memory_order_acquire)) {
            // Binary state of the bar (with a verified backup) - once, as the last escalation.
            if (g_cfg.forceNetworkTrayResetTraySettings && !BackupAndResetTrayValuesOnce())
                ok = false;
        }

        NudgeTray();

        g_guaranteesApplied = ok;
        if (!ok) {
            g_nextGuaranteeTry = now + 30000;
            Wh_Log(L"[tray-force] some visibility guarantees were not applied: "
                   L"new attempt in 30 s");
        } else {
            Wh_Log(L"[tray-force] visibility guarantees applied (optional tray state reset)");
        }
        return ok;
    } catch (...) {
        Wh_Log(L"[tray-force] exception in the visibility guarantees");
        return false;
    }
}

// ------------------------------------------------- the owner window ------
// ------------------------------------------ the authentic Win10 flyout ---
// Windows 10 opens the network flyout by asking the shell's factory for the experience
// "Windows.Internal.ShellExperience.NetworkFlyout" and calling ShowFlyout on it: that is
// exactly the path pnidui.dll takes (the string sits in its own table). This repeats that
// path, with no substitute URI:
//   CoCreateInstance(CLSID_ImmersiveShell, IID_IServiceProvider)
//   -> QueryService(CLSID_ShellExperienceManagerFactory)
//   -> GetExperienceManager("...NetworkFlyout")
//   -> QueryInterface(IID_NetworkFlyoutExperienceManager) -> ShowFlyout(rect)
static const GUID kClsidImmersiveShell =
    { 0xc2f03a33, 0x21f5, 0x47fa, { 0xb4, 0xbb, 0x15, 0x63, 0x62, 0xa2, 0xf2, 0x39 } };
static const GUID kIidServiceProvider =
    { 0x6d5140c1, 0x7436, 0x11ce, { 0x80, 0x34, 0x00, 0xaa, 0x00, 0x60, 0x09, 0xfa } };
static const GUID kClsidShellExperienceManagerFactory =
    { 0x2e8fcb18, 0xa0ee, 0x41ad, { 0x8e, 0xf8, 0x77, 0xfb, 0x3a, 0x37, 0x0c, 0xa5 } };
static const GUID kIidNetworkFlyoutExperienceManager =
    { 0xc9ddc674, 0xb44b, 0x4c67, { 0x9d, 0x79, 0x2b, 0x23, 0x7d, 0x9b, 0xe0, 0x5a } };

struct Win10FlyoutRect { float x, y, width, height; };

struct ServiceProviderVtbl {
    HRESULT (STDMETHODCALLTYPE* QueryInterface)(void*, REFIID, void**);
    ULONG   (STDMETHODCALLTYPE* AddRef)(void*);
    ULONG   (STDMETHODCALLTYPE* Release)(void*);
    HRESULT (STDMETHODCALLTYPE* QueryService)(void*, REFGUID, REFIID, void**);
};

struct ExperienceManagerFactoryVtbl {
    HRESULT (STDMETHODCALLTYPE* QueryInterface)(void*, REFIID, void**);
    ULONG   (STDMETHODCALLTYPE* AddRef)(void*);
    ULONG   (STDMETHODCALLTYPE* Release)(void*);
    HRESULT (STDMETHODCALLTYPE* GetIids)(void*, ULONG*, IID**);
    HRESULT (STDMETHODCALLTYPE* GetRuntimeClassName)(void*, void**);
    HRESULT (STDMETHODCALLTYPE* GetTrustLevel)(void*, int*);
    HRESULT (STDMETHODCALLTYPE* GetExperienceManager)(void*, void*, void**);
};

struct ExperienceManagerVtbl {
    HRESULT (STDMETHODCALLTYPE* QueryInterface)(void*, REFIID, void**);
    ULONG   (STDMETHODCALLTYPE* AddRef)(void*);
    ULONG   (STDMETHODCALLTYPE* Release)(void*);
    HRESULT (STDMETHODCALLTYPE* GetIids)(void*, ULONG*, IID**);
    HRESULT (STDMETHODCALLTYPE* GetRuntimeClassName)(void*, void**);
    HRESULT (STDMETHODCALLTYPE* GetTrustLevel)(void*, int*);
    HRESULT (STDMETHODCALLTYPE* ShowFlyout)(void*, void*);
    HRESULT (STDMETHODCALLTYPE* HideFlyout)(void*);
};

class ScopedUnknownRef {
public:
    ScopedUnknownRef() = default;
    explicit ScopedUnknownRef(void* pointer) noexcept : m_pointer(pointer) {}
    ~ScopedUnknownRef() { reset(); }
    ScopedUnknownRef(const ScopedUnknownRef&) = delete;
    ScopedUnknownRef& operator=(const ScopedUnknownRef&) = delete;
    void* get() const noexcept { return m_pointer; }
    void reset(void* pointer = nullptr) noexcept {
        if (m_pointer) {
            auto vtbl = *reinterpret_cast<ServiceProviderVtbl* const*>(m_pointer);
            if (vtbl && vtbl->Release) vtbl->Release(m_pointer);
        }
        m_pointer = pointer;
    }
private:
    void* m_pointer = nullptr;
};

static bool QueryIconRect(const NetworkPniRegistration& reg, RECT* out) noexcept;   // defined below

class ScopedFlyoutCom {
public:
    ScopedFlyoutCom() noexcept {
        m_hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        m_uninit = SUCCEEDED(m_hr);
    }
    ~ScopedFlyoutCom() { if (m_uninit) CoUninitialize(); }
    ScopedFlyoutCom(const ScopedFlyoutCom&) = delete;
    ScopedFlyoutCom& operator=(const ScopedFlyoutCom&) = delete;
private:
    HRESULT m_hr = E_FAIL;
    bool m_uninit = false;
};

static std::atomic<unsigned int> g_networkFlyoutFailureLogs{0};

static void LogNetworkFlyoutFailure(PCWSTR stage, HRESULT hr) noexcept {
    if (g_networkFlyoutFailureLogs.fetch_add(1, std::memory_order_relaxed) < 12)
        Wh_Log(L"[tray-force] network flyout activation failed at %s (0x%08X)",
               stage, static_cast<unsigned>(hr));
}

static HRESULT InvokeWin10NetworkFlyout(const RECT* anchor) noexcept {
    try {
        ScopedFlyoutCom com;
        void* shell = nullptr;
        HRESULT hr = CoCreateInstance(kClsidImmersiveShell, nullptr,
                                      CLSCTX_NO_CODE_DOWNLOAD | CLSCTX_LOCAL_SERVER,
                                      kIidServiceProvider, &shell);
        ScopedUnknownRef shellRef(shell);
        if (FAILED(hr) || !shell) {
            LogNetworkFlyoutFailure(L"CoCreateInstance(ImmersiveShell)",
                                    FAILED(hr) ? hr : E_NOINTERFACE);
            return FAILED(hr) ? hr : E_NOINTERFACE;
        }

        auto sp = *reinterpret_cast<ServiceProviderVtbl* const*>(shell);
        if (!sp || !sp->QueryService) {
            LogNetworkFlyoutFailure(L"IServiceProvider::QueryService (vtable)", E_NOINTERFACE);
            return E_NOINTERFACE;
        }
        void* factory = nullptr;
        hr = sp->QueryService(shell, kClsidShellExperienceManagerFactory,
                              kClsidShellExperienceManagerFactory, &factory);
        ScopedUnknownRef factoryRef(factory);
        if (FAILED(hr) || !factory) {
            LogNetworkFlyoutFailure(L"QueryService(ShellExperienceManagerFactory)",
                                    FAILED(hr) ? hr : E_NOINTERFACE);
            return FAILED(hr) ? hr : E_NOINTERFACE;
        }

        HMODULE combase = GetModuleHandleW(L"combase.dll");
        auto createString = combase ? (HRESULT (WINAPI*)(const wchar_t*, UINT32, void**))
                                          GetProcAddress(combase, "WindowsCreateString")
                                    : nullptr;
        auto deleteString = combase ? (HRESULT (WINAPI*)(void*))
                                          GetProcAddress(combase, "WindowsDeleteString")
                                    : nullptr;
        if (!createString) {
            const HRESULT missing = HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
            LogNetworkFlyoutFailure(L"WindowsCreateString lookup", missing);
            return missing;
        }

        static const wchar_t kExperience[] = L"Windows.Internal.ShellExperience.NetworkFlyout";
        void* name = nullptr;
        hr = createString(kExperience, static_cast<UINT32>(wcslen(kExperience)), &name);
        if (FAILED(hr) || !name) {
            LogNetworkFlyoutFailure(L"WindowsCreateString(NetworkFlyout)",
                                    FAILED(hr) ? hr : E_FAIL);
            return FAILED(hr) ? hr : E_FAIL;
        }

        auto factoryVtbl = *reinterpret_cast<ExperienceManagerFactoryVtbl* const*>(factory);
        HRESULT hrManager = E_NOINTERFACE;
        void* manager = nullptr;
        if (factoryVtbl && factoryVtbl->GetExperienceManager)
            hrManager = factoryVtbl->GetExperienceManager(factory, name, &manager);
        if (deleteString) deleteString(name);
        ScopedUnknownRef managerRef(manager);
        if (FAILED(hrManager) || !manager) {
            LogNetworkFlyoutFailure(L"GetExperienceManager(NetworkFlyout)",
                                    FAILED(hrManager) ? hrManager : E_NOINTERFACE);
            return FAILED(hrManager) ? hrManager : E_NOINTERFACE;
        }

        auto managerVtbl = *reinterpret_cast<ExperienceManagerVtbl* const*>(manager);
        if (!managerVtbl || !managerVtbl->QueryInterface) {
            LogNetworkFlyoutFailure(L"IExperienceManager::QueryInterface (vtable)", E_NOINTERFACE);
            return E_NOINTERFACE;
        }
        void* flyout = nullptr;
        hr = managerVtbl->QueryInterface(manager, kIidNetworkFlyoutExperienceManager, &flyout);
        ScopedUnknownRef flyoutRef(flyout);
        if (FAILED(hr) || !flyout) {
            LogNetworkFlyoutFailure(L"QueryInterface(NetworkFlyoutExperienceManager)",
                                    FAILED(hr) ? hr : E_NOINTERFACE);
            return FAILED(hr) ? hr : E_NOINTERFACE;
        }

        auto flyoutVtbl = *reinterpret_cast<ExperienceManagerVtbl* const*>(flyout);
        if (!flyoutVtbl || !flyoutVtbl->ShowFlyout) {
            LogNetworkFlyoutFailure(L"ShowFlyout (vtable)", E_NOINTERFACE);
            return E_NOINTERFACE;
        }
        Win10FlyoutRect rect = {};
        if (anchor) {
            rect.x = static_cast<float>(anchor->left);
            rect.y = static_cast<float>(anchor->top);
            rect.width = static_cast<float>(anchor->right - anchor->left);
            rect.height = static_cast<float>(anchor->bottom - anchor->top);
        }
        // ShowFlyout is a private shell API. Do not call it a second time after a
        // failed HRESULT: it may already have initiated rendering, and a duplicate
        // request can close or race the flyout that is being created.
        const HRESULT showResult = flyoutVtbl->ShowFlyout(flyout, &rect);
        if (FAILED(showResult)) LogNetworkFlyoutFailure(L"IExperienceManager::ShowFlyout", showResult);
        return showResult;
    } catch (...) {
        Wh_Log(L"[tray-force] C++ exception while opening the network flyout");
        return E_FAIL;
    }
}

// ------------------------------------ the menu of the network icon ----------
// the two entries of the original Windows 10 menu, with the destinations this mod asks for:
//   * "Troubleshoot problems" -> NETWORK problems (not hardware): the Microsoft
//     "Internet Connections" troubleshooter, msdt.exe /id NetworkDiagnosticsWeb;
//   * "Open Network && Internet settings" ->
//     shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}.
// Both go through the guard (a service thread plus a time cap). The menu handlers use
// try/catch for C++ exceptions only: native access faults are not intercepted.

static void RunNetworkMenuAction(UINT command) noexcept {
    try {
        if (command == 1) {
            Wh_Log(L"[tray-force] network icon menu: network troubleshooting "
                   L"(Internet Connections - NetworkDiagnosticsWeb)");
            ShellOpGuard::RunCommandGuarded(L"msdt.exe", L"/id NetworkDiagnosticsWeb", L"open",
                                            L"network: troubleshooting (Internet Connections)");
        } else if (command == 2) {
            Wh_Log(L"[tray-force] network icon menu: network and Internet settings "
                   L"(shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D})");
            ShellOpGuard::OpenShellUriGuarded(L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}",
                                              L"network: network and Internet settings");
        }
    } catch (...) {
        Wh_Log(L"[tray-force] exception while running the network menu entry");
    }
}

// ------------------- the menu of the network icon: the look of the audio menu -------
// the same mechanism as the volume / battery menu (ShowBatteryMenu): a native popup built
// with CreatePopupMenu + TrackPopupMenuEx, owned by Shell_TrayWnd. Windows then applies the
// Explorer theme (the Windows 10 look) by itself, with no custom drawing. Entries and
// destinations unchanged: 1 = troubleshoot, 2 = network and Internet settings (see
// RunNetworkMenuAction).
static int g_networkMenuLogs = 0;
static void ShowNetworkIconMenuHere(HWND owner) noexcept {
    try {
        HMENU menu = CreatePopupMenu();
        if (!menu) return;
        const LANGID lang = GetUserDefaultUILanguage();
        const bool italian = PRIMARYLANGID(lang) == LANG_ITALIAN;
        AppendMenuW(menu, MF_STRING, 1,
                    italian ? L"Risoluzione dei problemi" : L"Troubleshoot problems");
        AppendMenuW(menu, MF_STRING, 2,
                    italian ? L"Apri impostazioni di rete e Internet"
                            : L"Open Network && Internet settings");

        POINT point = {};
        GetCursorPos(&point);

        // TrackPopupMenuEx needs the owner window to belong to the calling thread. This
        // code runs on the thread of the mod's owner window (not on the one of the bar):
        // With Shell_TrayWnd the call used to fail and the menu never appeared. The mod's
        // window is therefore used; Shell_TrayWnd only when it happens to be on the same
        // thread (Explorer theme, like the audio menu).
        HWND menuOwner = owner;
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (tray && IsWindow(tray) &&
            GetWindowThreadProcessId(tray, nullptr) == GetCurrentThreadId())
            menuOwner = tray;
        if (!menuOwner || !IsWindow(menuOwner)) {
            DestroyMenu(menu);
            return;
        }
        SetForegroundWindow(menuOwner);

        const BOOL picked = static_cast<BOOL>(
            ImmersiveMenu::Track(menu, menuOwner, point.x, point.y, TPM_RIGHTBUTTON));
        if (!picked && g_networkMenuLogs++ < 3)
            Wh_Log(L"[tray-force] TrackPopupMenuEx: no choice (error %lu)", GetLastError());
        DestroyMenu(menu);
        // The null message that lets the menu close properly (MSDN).
        PostMessageW(menuOwner, WM_NULL, 0, 0);
        if (picked) RunNetworkMenuAction(static_cast<UINT>(picked));
    } catch (...) {
        Wh_Log(L"[tray-force] exception in the network icon menu");
    }
}
// Called on the thread of the mod's owner window: it forwards the request to a
// (subclassed) service window that lives on the thread of the bar, so the menu has
// Shell_TrayWnd as its owner and the same look as the audio menu.
static void ShowNetworkIconMenu(HWND owner) noexcept {
    try {
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        const DWORD trayThread = tray ? GetWindowThreadProcessId(tray, nullptr) : 0;
        if (trayThread) {
            for (int i = 0; i < g_trayWndCount; ++i) {
                HWND w = g_trayWnds[i];
                if (w && IsWindow(w) && GetWindowThreadProcessId(w, nullptr) == trayThread &&
                    PostMessageW(w, NetworkMenuMessage(), 0, 0)) {
                    if (g_networkMenuLogs < 3)
                        Wh_Log(L"[tray-force] network menu forwarded to the bar thread");
                    return;
                }
            }
        }
        if (g_networkMenuLogs++ < 3)
            Wh_Log(L"[tray-force] no service window on the bar thread: "
                   L"menu shown by the mod thread");
    } catch (...) {
    }
    ShowNetworkIconMenuHere(owner);
}
// ===========================================================================
// The click on the network icon: the genuine Windows 10 flyout, and nothing else.
//
// The Windows 10 shell opens its network flyout with a call, not with a URI:
//   CoCreateInstance(CLSID_ImmersiveShell, IID_IServiceProvider)
//   -> QueryService(CLSID_ShellExperienceManagerFactory)
//   -> GetExperienceManager(L"Windows.Internal.ShellExperience.NetworkFlyout")
//   -> QueryInterface(IID_INetworkFlyoutExperienceManager) -> ShowFlyout(rect)
// (the same path as the reference implementation; InvokeWin10NetworkFlyout above).
//
// The click is taken over at the icon itself (see the click interception above) and
// every network URI of this shell is answered with a request for this flyout. Nothing else
// is left that can end on a page: the native pnidui handler and the ms-settings /
// ms-availablenetworks targets are what opened Settings or the Windows 11 panel. If the
// flyout never opens, nothing at all is launched and the log names the step that failed.
//
// A hook cannot sleep or call COM: the request travels as a registered window message to
// the owner window of the icon, the same way the icon menu does it (NetworkMenuMessage).
// ===========================================================================

// The icon owner window lives in this namespace but is defined further down.
static bool EnsureOwnerWindow() noexcept;

static UINT ShowNetworkFlyoutMessage() noexcept {
    static UINT message = RegisterWindowMessageW(L"Win10ExplorerRestorer.ShowNetworkFlyout");
    return message;
}

// Asks the flyout to the owner window of the icon: that window runs on the thread with the
// message loop, which is where the shell expects ShowFlyout to be called from.
static bool RequestNetworkFlyout() noexcept {
    try {
        if (g_unloading.load(std::memory_order_acquire)) return false;
        HWND owner = g_ownerWindow;
        if (!owner || !IsWindow(owner)) {
            if (!EnsureOwnerWindow()) return false;
            owner = g_ownerWindow;
        }
        if (!owner || !IsWindow(owner)) return false;
        const UINT message = ShowNetworkFlyoutMessage();
        if (!message) return false;
        return PostMessageW(owner, message, 0, 0) != FALSE;
    } catch (...) {
        return false;
    }
}

// The genuine Windows 10 network flyout, anchored on the tray icon. Returns the HRESULT of
// the last call: the caller decides whether to retry.
static HRESULT OpenAuthenticNetworkFlyout(const wchar_t* reason) noexcept {
    try {
        if (g_unloading.load(std::memory_order_acquire)) return E_ABORT;

        NetworkPniRegistration reg = {};
        const bool have = CopyNetworkPniRegistration(&reg);
        RECT anchor = {};
        bool haveAnchor = have && reg.valid && QueryIconRect(reg, &anchor);

        if (!haveAnchor) {
            // The registration was never captured (the icon is pnidui's own): the system
            // GUID still gives the icon rectangle.
            NOTIFYICONIDENTIFIER identifier = {};
            identifier.cbSize = sizeof(identifier);
            identifier.guidItem = kSystemNetworkIconGuid;
            if (SUCCEEDED(Shell_NotifyIconGetRect(&identifier, &anchor))) haveAnchor = true;
        }

        HRESULT hr = InvokeWin10NetworkFlyout(haveAnchor ? &anchor : nullptr);
        if (FAILED(hr) && haveAnchor) {
            // As in the reference implementation: when the call anchored on the icon is
            // refused, it is asked once more without a rectangle.
            Wh_Log(L"[network] %s: the call anchored on the icon failed (0x%08X): asking "
                   L"again without the anchor", reason ? reason : L"click", (unsigned)hr);
            hr = InvokeWin10NetworkFlyout(nullptr);
        }
        Wh_Log(L"[network] %s: the Windows 10 network flyout call returned 0x%08X%s",
               reason ? reason : L"click", (unsigned)hr,
               haveAnchor ? L", anchored on the tray icon" : L", without an anchor");
        return hr;
    } catch (...) {
        Wh_Log(L"[network] exception while opening the Windows 10 network flyout");
        return E_FAIL;
    }
}

// ===========================================================================
// The battery icon: the genuine Windows 10 battery flyout, and nothing else.
//
// The Windows 10 shell opens this flyout exactly the way it opens the network one - with a
// call, not with a page, not with a substitute window and not with a registry value:
//
//   CoCreateInstance(CLSID_ImmersiveShell, IID_IServiceProvider)
//   -> QueryService(CLSID_ShellExperienceManagerFactory)
//   -> GetExperienceManager(L"Windows.Internal.ShellExperience.TrayBatteryFlyout")
//   -> QueryInterface(IID_TrayBatteryFlyoutExperienceManager) -> ShowFlyout(rect)
//
// The experience name and the interface id are the ones twinui.dll registers for the
// Windows 10 battery flyout: the same table the Windows 10 network flyout comes from.
//
// the Win32 path is gone: answering UseWin32BatteryFlyout = 1 on
// RegQueryValueExW made stobject.dll show its Windows 7 era Win32 flyout - that is not the
// Windows 10 flyout, it is the compatible behaviour of an older shell. No registry value is
// read or answered here any more, and the flyout that opens is the one of this shell.
// ===========================================================================
namespace BatteryFlyout {

// The power icon of the notification area of this shell (0x7820AE75; 0x7820AE74 is the
// network icon the rest of this mod pins, 0x7820AE73 the volume one).
static const GUID kSystemBatteryIconGuid = {
    0x7820AE75, 0x23E3, 0x4229, { 0x82, 0xC1, 0xE4, 0x1C, 0xB6, 0x7D, 0x5B, 0x9C }
};
static const GUID kIidTrayBatteryFlyoutExperienceManager = {
    0x0A73AEDC, 0x1C68, 0x410D, { 0x8D, 0x53, 0x63, 0xAF, 0x80, 0x95, 0x1E, 0x8F }
};

static std::atomic<unsigned int> g_batteryFailureLogs{0};
static int g_batteryClickLogs = 0;
static ULONGLONG g_lastBatteryClickTick = 0;

static void LogBatteryFlyoutFailure(PCWSTR stage, HRESULT hr) noexcept {
    if (g_batteryFailureLogs.fetch_add(1, std::memory_order_relaxed) < 12)
        Wh_Log(L"[battery] Windows 10 battery flyout activation failed at %s (0x%08X)",
               stage, static_cast<unsigned>(hr));
}

// The rectangle of the battery icon of this shell. The system GUID names the icon even when
// its registration was never captured by this mod.
static bool QueryBatteryIconRect(RECT* out) noexcept {
    try {
        if (!out) return false;
        NOTIFYICONIDENTIFIER identifier = {};
        identifier.cbSize = sizeof(identifier);
        identifier.guidItem = kSystemBatteryIconGuid;
        return SUCCEEDED(Shell_NotifyIconGetRect(&identifier, out));
    } catch (...) {
        return false;
    }
}

// The authentic call: the one the Windows 10 shell makes for this icon. Same chain as
// InvokeWin10NetworkFlyout above, with the battery experience name and interface id.
static HRESULT InvokeWin10BatteryFlyout(const RECT* anchor) noexcept {
    try {
        ScopedFlyoutCom com;
        void* shell = nullptr;
        HRESULT hr = CoCreateInstance(kClsidImmersiveShell, nullptr,
                                      CLSCTX_NO_CODE_DOWNLOAD | CLSCTX_LOCAL_SERVER,
                                      kIidServiceProvider, &shell);
        ScopedUnknownRef shellRef(shell);
        if (FAILED(hr) || !shell) {
            LogBatteryFlyoutFailure(L"CoCreateInstance(ImmersiveShell)",
                                    FAILED(hr) ? hr : E_NOINTERFACE);
            return FAILED(hr) ? hr : E_NOINTERFACE;
        }

        auto sp = *reinterpret_cast<ServiceProviderVtbl* const*>(shell);
        if (!sp || !sp->QueryService) {
            LogBatteryFlyoutFailure(L"IServiceProvider::QueryService (vtable)", E_NOINTERFACE);
            return E_NOINTERFACE;
        }
        void* factory = nullptr;
        hr = sp->QueryService(shell, kClsidShellExperienceManagerFactory,
                              kClsidShellExperienceManagerFactory, &factory);
        ScopedUnknownRef factoryRef(factory);
        if (FAILED(hr) || !factory) {
            LogBatteryFlyoutFailure(L"QueryService(ShellExperienceManagerFactory)",
                                    FAILED(hr) ? hr : E_NOINTERFACE);
            return FAILED(hr) ? hr : E_NOINTERFACE;
        }

        HMODULE combase = GetModuleHandleW(L"combase.dll");
        auto createString = combase ? (HRESULT(WINAPI*)(const wchar_t*, UINT32, void**))
                                          GetProcAddress(combase, "WindowsCreateString")
                                    : nullptr;
        auto deleteString = combase ? (HRESULT(WINAPI*)(void*))
                                          GetProcAddress(combase, "WindowsDeleteString")
                                    : nullptr;
        if (!createString) {
            const HRESULT missing = HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
            LogBatteryFlyoutFailure(L"WindowsCreateString lookup", missing);
            return missing;
        }

        static const wchar_t kExperience[] = L"Windows.Internal.ShellExperience.TrayBatteryFlyout";
        void* name = nullptr;
        hr = createString(kExperience, static_cast<UINT32>(wcslen(kExperience)), &name);
        if (FAILED(hr) || !name) {
            LogBatteryFlyoutFailure(L"WindowsCreateString(TrayBatteryFlyout)",
                                    FAILED(hr) ? hr : E_FAIL);
            return FAILED(hr) ? hr : E_FAIL;
        }

        auto factoryVtbl = *reinterpret_cast<ExperienceManagerFactoryVtbl* const*>(factory);
        HRESULT hrManager = E_NOINTERFACE;
        void* manager = nullptr;
        if (factoryVtbl && factoryVtbl->GetExperienceManager)
            hrManager = factoryVtbl->GetExperienceManager(factory, name, &manager);
        if (deleteString) deleteString(name);
        ScopedUnknownRef managerRef(manager);
        if (FAILED(hrManager) || !manager) {
            LogBatteryFlyoutFailure(L"GetExperienceManager(TrayBatteryFlyout)",
                                    FAILED(hrManager) ? hrManager : E_NOINTERFACE);
            return FAILED(hrManager) ? hrManager : E_NOINTERFACE;
        }

        auto managerVtbl = *reinterpret_cast<ExperienceManagerVtbl* const*>(manager);
        if (!managerVtbl || !managerVtbl->QueryInterface) {
            LogBatteryFlyoutFailure(L"IExperienceManager::QueryInterface (vtable)", E_NOINTERFACE);
            return E_NOINTERFACE;
        }
        void* flyout = nullptr;
        hr = managerVtbl->QueryInterface(manager, kIidTrayBatteryFlyoutExperienceManager, &flyout);
        ScopedUnknownRef flyoutRef(flyout);
        if (FAILED(hr) || !flyout) {
            LogBatteryFlyoutFailure(L"QueryInterface(TrayBatteryFlyoutExperienceManager)",
                                    FAILED(hr) ? hr : E_NOINTERFACE);
            return FAILED(hr) ? hr : E_NOINTERFACE;
        }

        auto flyoutVtbl = *reinterpret_cast<ExperienceManagerVtbl* const*>(flyout);
        if (!flyoutVtbl || !flyoutVtbl->ShowFlyout) {
            LogBatteryFlyoutFailure(L"ShowFlyout (vtable)", E_NOINTERFACE);
            return E_NOINTERFACE;
        }
        Win10FlyoutRect rect = {};
        if (anchor) {
            rect.x = static_cast<float>(anchor->left);
            rect.y = static_cast<float>(anchor->top);
            rect.width = static_cast<float>(anchor->right - anchor->left);
            rect.height = static_cast<float>(anchor->bottom - anchor->top);
        }
        // ShowFlyout is a private shell API: it is not called a second time after a failed
        // HRESULT, exactly as in the network flyout path.
        const HRESULT showResult = flyoutVtbl->ShowFlyout(flyout, &rect);
        if (FAILED(showResult)) LogBatteryFlyoutFailure(L"IExperienceManager::ShowFlyout", showResult);
        return showResult;
    } catch (...) {
        Wh_Log(L"[battery] C++ exception while opening the Windows 10 battery flyout");
        return E_FAIL;
    }
}

// The call anchored on the battery icon. When it is refused, it is asked once more without a
// rectangle, as the reference implementation does for the network flyout.
static HRESULT OpenAuthenticBatteryFlyout(const wchar_t* reason) noexcept {
    try {
        if (g_unloading.load(std::memory_order_acquire)) return E_ABORT;
        RECT anchor = {};
        const bool haveAnchor = QueryBatteryIconRect(&anchor);
        HRESULT hr = InvokeWin10BatteryFlyout(haveAnchor ? &anchor : nullptr);
        if (FAILED(hr) && haveAnchor) {
            Wh_Log(L"[battery] %s: the call anchored on the icon failed (0x%08X): asking again "
                   L"without the anchor", reason ? reason : L"click", static_cast<unsigned>(hr));
            hr = InvokeWin10BatteryFlyout(nullptr);
        }
        Wh_Log(L"[battery] %s: the Windows 10 battery flyout call returned 0x%08X%s",
               reason ? reason : L"click", static_cast<unsigned>(hr),
               haveAnchor ? L", anchored on the tray icon" : L", without an anchor");
        return hr;
    } catch (...) {
        Wh_Log(L"[battery] exception while opening the Windows 10 battery flyout");
        return E_FAIL;
    }
}

static UINT ShowBatteryFlyoutMessage() noexcept {
    static UINT message = RegisterWindowMessageW(L"Win10ExplorerRestorer.ShowBatteryFlyout");
    return message;
}

// Asks the flyout to the owner window of the icon: the window the network flyout uses, on the
// thread with the message loop. A window procedure of the shell never sleeps and never calls
// COM, so from there the request is only posted.
//
// Not `static`: the window procedure of the battery icon comes earlier in the file than this
// definition and calls this function, so its declaration stands there (see the declaration next
// to ShowNetworkIconMenuHere) and the two have to name the same function.
bool RequestBatteryFlyout() noexcept {
    try {
        if (g_unloading.load(std::memory_order_acquire)) return false;
        HWND owner = g_ownerWindow;
        if (!owner || !IsWindow(owner)) {
            if (!EnsureOwnerWindow()) return false;
            owner = g_ownerWindow;
        }
        if (!owner || !IsWindow(owner)) return false;
        const UINT message = ShowBatteryFlyoutMessage();
        return message != 0 && PostMessageW(owner, message, 0, 0) != FALSE;
    } catch (...) {
        return false;
    }
}

// Served on the owner window of the icon, exactly like the network request.
static bool OpenNativeBatteryFlyout(HWND owner, PCWSTR how) noexcept {
    (void)owner;
    try {
        if (g_unloading.load(std::memory_order_acquire)) return false;
        const ULONGLONG now = GetTickCount64();
        if (now - g_lastBatteryClickTick < 300) return true;   // the same click, delivered twice
        g_lastBatteryClickTick = now;

        HRESULT hr = E_FAIL;
        for (int attempt = 0; attempt < 3; ++attempt) {
            hr = OpenAuthenticBatteryFlyout(attempt == 0 ? L"click on the battery icon"
                                                         : L"click on the battery icon (retry)");
            if (SUCCEEDED(hr)) break;
            Sleep(150);   // the shell may not be ready yet (first click after the logon)
        }
        if (SUCCEEDED(hr)) {
            if (g_batteryClickLogs++ < 4)
                Wh_Log(L"[battery] %s: the Windows 10 battery flyout opened through the shell "
                       L"experience manager (0x%08X)", how ? how : L"left click",
                       static_cast<unsigned>(hr));
            return true;
        }
        Wh_Log(L"[battery] %s: the Windows 10 battery flyout did not open (0x%08X). Nothing "
               L"else is started: no page, no Win32 flyout, no registry value. The log line "
               L"above names the step that failed", how ? how : L"left click",
               static_cast<unsigned>(hr));
    } catch (...) {
        Wh_Log(L"[battery] exception while opening the flyout");
    }
    return true;   // the click is consumed: it must not end anywhere else
}

// There is nothing to install - the flyout is opened by the call above, every time the
// click arrives. The name stays because Wh_ModInit calls it, and the log line says what the
// battery icon opens now.
static void Install() noexcept {
    Wh_Log(L"[battery] the battery icon opens the Windows 10 battery flyout "
           L"(Windows.Internal.ShellExperience.TrayBatteryFlyout) through the shell experience "
           L"manager: the same authentic call the network flyout goes through; no registry "
           L"value and no Win32 flyout is involved");
}

}  // namespace BatteryFlyout
// ===========================================================================
// The click on the network icon never reaches pnidui's own handler.
//
// The tray icon is registered by pnidui.dll together with the window that receives its
// callbacks (`hWnd`/`uCallbackMessage` of the SNI registration, recorded above). On this
// build, pnidui's handler for the left click ends on a page: it is the one thing that
// opened Settings no matter what the ShellExecute hooks answered, because the page is not
// necessarily launched through shell32!ShellExecuteW/ShellExecuteExW.
//
// The window is subclassed instead, and the left click is consumed before that handler can
// see it:
//
//   * a left click becomes a request for the Windows 10 flyout, posted to the mod's owner
//     window (a window procedure of the shell never sleeps and never calls COM, so the
//     request is only posted from here);
//   * the right click and everything else are passed straight through, so the native menu
//     of the icon is exactly what it was;
//   * nothing of the icon changes: registration, image, tooltip and menu are untouched.
//
// The subclass lives on the shell's window, so it is installed and removed with
// WindhawkUtils' cross-thread helpers, and it is removed while the module is still loaded.
// ===========================================================================
static HWND g_pniClickWnd = nullptr;    // the icon window whose click this mod answers
static UINT g_pniClickMessage = 0;      // the message the icon sends its callbacks in
static const UINT_PTR kPniClickSubclassId = 78;
static int g_pniClickLogs = 0;

static LRESULT PniClickSubclassProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam,
                                    DWORD_PTR refData) {
    (void)refData;
    try {
        if (message == g_pniClickMessage && g_pniClickMessage) {
            const UINT event = LOWORD(lParam);   // NIN_* or WM_*
            const bool leftDown = (event == WM_LBUTTONDOWN || event == WM_LBUTTONDBLCLK);
            const bool leftUp = (event == WM_LBUTTONUP || event == NIN_SELECT ||
                                 event == NIN_KEYSELECT);
            if (leftDown || leftUp) {
                if (leftDown) return 0;   // the press belongs to the click answered below
                const bool requested = RequestNetworkFlyout();
                if (g_pniClickLogs++ < 4) {
                    if (requested)
                        Wh_Log(L"[network] the click on the network icon is answered here: the "
                               L"Windows 10 flyout is opened and pnidui's own handler never "
                               L"sees the click");
                    else
                        Wh_Log(L"[network] the click on the network icon is answered here, but "
                               L"the flyout request could not be posted: the click stays "
                               L"consumed and nothing else is opened");
                }
                return 0;   // consumed: no page, and no native handler that could open one
            }
        }
        if (message == WM_NCDESTROY && hwnd == g_pniClickWnd) {
            g_pniClickWnd = nullptr;
            g_pniClickMessage = 0;
        }
    } catch (...) {
        Wh_Log(L"[network] exception while answering a click on the network icon");
    }
    return DefSubclassProc(hwnd, message, wParam, lParam);
}

// Called at every tray tick, on the mod's own thread: the icon is registered again whenever
// the bar is recreated, with a new window, and the subclass follows it. Re-arming is limited
// to once every 10 s, which is also what repairs a subclass the shell has dropped.
static void ArmNetworkIconClickSubclass() noexcept {
    try {
        if (g_unloading.load(std::memory_order_acquire)) return;
        NetworkPniRegistration reg = {};
        if (!CopyNetworkPniRegistration(&reg) || !reg.valid) return;
        if (!reg.hwnd || !reg.callbackMessage || !IsWindow(reg.hwnd)) return;
        if (!IsPniduiServiceWindow(reg.hwnd)) return;   // only pnidui's own icon window
        if (g_pniClickWnd == reg.hwnd && g_pniClickMessage == reg.callbackMessage) {
            // The subclass is believed to be in place, but nothing ever checked it, and
            // the comment above promised a repair that this early return made impossible.
            // Every 10 s the same (proc, id) pair is set again: SetWindowSubclass with a pair
            // that already exists only refreshes it, and with a pair the shell dropped it
            // installs it again, with no gap in between (nothing is removed first).
            static ULONGLONG s_lastReassert = 0;
            const ULONGLONG now = GetTickCount64();
            if (now - s_lastReassert < 10000) return;
            s_lastReassert = now;
            if (!WindhawkUtils::SetWindowSubclassFromAnyThread(reg.hwnd, PniClickSubclassProc,
                                                               kPniClickSubclassId)) {
                Wh_Log(L"[network] the click subclass could not be refreshed: it is armed again");
                g_pniClickWnd = nullptr;   // falls through to a full arm below
            } else {
                return;
            }
        }

        // The flyout is opened by the owner window of the mod, so that window is made ready
        // here, on this thread: the window procedure of the shell is not the place where a
        // window is created or waited for.
        if (!EnsureOwnerWindow()) {
            Wh_Log(L"[network] the owner window is not available: the click of the network "
                   L"icon stays with the shell for now");
            return;
        }

        if (g_pniClickWnd && IsWindow(g_pniClickWnd))
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_pniClickWnd,
                                                             PniClickSubclassProc);
        g_pniClickWnd = nullptr;
        g_pniClickMessage = reg.callbackMessage;
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(reg.hwnd, PniClickSubclassProc,
                                                          kPniClickSubclassId)) {
            g_pniClickWnd = reg.hwnd;
            Wh_Log(L"[network] the click of the network icon is answered by this mod from now "
                   L"on (window 0x%p, callback message 0x%X): the Windows 10 flyout opens and "
                   L"no page can", (void*)reg.hwnd, reg.callbackMessage);
        } else {
            g_pniClickMessage = 0;
            Wh_Log(L"[network] the click of the network icon stays with the shell: the "
                   L"subclass could not be installed");
        }
    } catch (...) {
        Wh_Log(L"[network] exception while taking over the click of the network icon");
    }
}

// Runs on the mod's tray thread while the module is still loaded, together with the rest of
// the teardown: no window procedure of the shell may outlive this module.
static void DisarmNetworkIconClickSubclass() noexcept {
    try {
        if (g_pniClickWnd && IsWindow(g_pniClickWnd)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_pniClickWnd,
                                                             PniClickSubclassProc);
            Wh_Log(L"[network] the click of the network icon goes back to the shell");
        }
    } catch (...) {
    }
    g_pniClickWnd = nullptr;
    g_pniClickMessage = 0;
}
// --- the click on the battery icon -------------------------------------------------
// the battery is not on a service window of pnidui: on this build it registers itself on
// SystemTray_Main. The takeover of the network icon click, which only works on the windows of
// pnidui, therefore never saw it: the click reached the shell's handler and the request of the
// mod was never made (no [battery] line in the log). The takeover is armed here, on the window
// and the id the battery's GUID has named, and it consumes the click: the only thing that
// opens is the Windows 10 battery flyout.


// Is the click the battery icon's? Two ways, both narrowed to that one icon: the callback
// message, which carries the icon id in wParam (so another icon of the same window is not
// touched), or a click that came directly to the window, which counts only when it falls
// inside the rectangle of the icon.


// The shape of the procedure is the one this file already uses for the network icon (the two
// WindhawkUtils functions and the click takeover defined above).
static LRESULT BatteryClickSubclassProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam,
                                        DWORD_PTR refData) {
    (void)refData;
    try {
        if (BatteryIconClickIsOurs(message, wParam, lParam)) {
            const bool press = (LOWORD(lParam) == WM_LBUTTONDOWN || message == WM_LBUTTONDOWN);
            if (press) return 0;   // the press belongs to the click answered below
            if (g_batteryClickLogs++ < 6) {
                wchar_t cls[64] = {};
                GetClassNameW(hwnd, cls, _countof(cls));
                Wh_Log(L"[battery] left click taken here (window %s, id %u): the handler of the "
                       L"shell does not see it, so nothing else can open", cls, g_batteryClickId);
            }
            BatteryFlyout::RequestBatteryFlyout();
            return 0;   // consumed: no page and no flyout of the shell
        }
        if (message == WM_NCDESTROY && hwnd == g_batteryClickWnd) {
            g_batteryClickWnd = nullptr;
            g_batteryClickMessage = 0;
        }
    } catch (...) {
        Wh_Log(L"[battery] exception while answering a click on the battery icon");
    }
    return DefSubclassProc(hwnd, message, wParam, lParam);
}

// Called from the icon tick, on the thread of the mod: as for the network icon, the owner
// window of the mod is prepared here and never inside a window procedure of the shell.
// Re-arming on every tick is not needed: the takeover stays until window and id change.
static void ArmBatteryIconClickSubclass() noexcept {
    try {
        if (g_unloading.load(std::memory_order_acquire)) return;
        if (!g_batteryTrayIconWnd || !IsWindow(g_batteryTrayIconWnd)) return;
        if (g_batteryClickWnd == g_batteryTrayIconWnd &&
            g_batteryClickId == g_batteryTrayIconId &&
            g_batteryClickMessage == g_batteryTrayIconMessage)
            return;
        if (!EnsureOwnerWindow()) return;

        if (g_batteryClickWnd && IsWindow(g_batteryClickWnd))
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_batteryClickWnd,
                                                             BatteryClickSubclassProc);
        g_batteryClickWnd = nullptr;
        g_batteryClickMessage = g_batteryTrayIconMessage;
        g_batteryClickId = g_batteryTrayIconId;
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(g_batteryTrayIconWnd,
                                                          BatteryClickSubclassProc,
                                                          kBatteryClickSubclassId)) {
            g_batteryClickWnd = g_batteryTrayIconWnd;
            Wh_Log(L"[battery] the click of the battery icon is answered by this mod from now on "
                   L"(window 0x%p, id %u, message 0x%X): the Windows 10 battery flyout opens, and "
                   L"nothing else", (void*)g_batteryClickWnd, g_batteryClickId,
                   (unsigned)g_batteryClickMessage);
        } else {
            g_batteryClickMessage = 0;
            g_batteryClickId = 0;
            Wh_Log(L"[battery] the click of the battery icon stays with the shell: the subclass "
                   L"could not be installed");
        }
    } catch (...) {
        Wh_Log(L"[battery] exception while taking over the click of the battery icon");
    }
}

// On the thread of the mod, while this module is still mapped: no window procedure of the
// shell may outlive this module.
static void DisarmBatteryIconClickSubclass() noexcept {
    try {
        if (g_batteryClickWnd && IsWindow(g_batteryClickWnd)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_batteryClickWnd,
                                                             BatteryClickSubclassProc);
            Wh_Log(L"[battery] the click of the battery icon goes back to the shell");
        }
    } catch (...) {
    }
    g_batteryClickWnd = nullptr;
    g_batteryClickMessage = 0;
    g_batteryClickId = 0;
}

static bool OpenNativeNetworkFlyout(HWND owner, bool rightClick) noexcept {
    try {
        if (g_unloading.load(std::memory_order_acquire)) return false;
        const ULONGLONG now = GetTickCount64();
        if (now - g_lastClickTick < 300) return true;   // the same click, delivered twice
        g_lastClickTick = now;

        NetworkPniRegistration reg = {};
        const bool have = CopyNetworkPniRegistration(&reg);

        if (rightClick) {
            // The right click stays as it was: the menu of pnidui's native callback, then
            // the mod's own. No flyout and no URI.
            if (have && reg.valid && reg.callbackMessage && IsPniduiServiceWindow(reg.hwnd)) {
                PostMessageW(reg.hwnd, reg.callbackMessage, static_cast<WPARAM>(reg.id),
                             MAKELPARAM(WM_RBUTTONUP, 0));
                Wh_Log(L"[tray-force] right click forwarded to the native PNI callback (0x%X)",
                       reg.callbackMessage);
                return true;
            }
            ShowNetworkIconMenu(owner);
            return true;
        }

        // The left click opens the Windows 10 flyout and nothing else. The click is
        // no longer forwarded to pnidui's native callback: on this build that path ends on a
        // URI and opens Settings. No substitute page is ever launched. Three attempts, 150 ms
        // apart, as in the reference implementation.
        HRESULT hr = E_FAIL;
        for (int attempt = 0; attempt < 3; ++attempt) {
            hr = OpenAuthenticNetworkFlyout(attempt == 0 ? L"left click on the icon"
                                                         : L"left click on the icon (retry)");
            if (SUCCEEDED(hr)) break;
            Sleep(150);   // the shell may not be ready yet (first click after the logon)
        }
        if (SUCCEEDED(hr)) {
            if (g_clickLogNotes++ < 4)
                Wh_Log(L"[tray-force] left click: Windows 10 network flyout opened "
                       L"through the shell experience manager (0x%08X)",
                       static_cast<unsigned>(hr));
            return true;
        }
        Wh_Log(L"[tray-force] left click: the network flyout did not open (0x%08X). Nothing "
               L"else is launched: no Settings, no ms-availablenetworks. The log lines of the "
               L"call above name the step that failed", static_cast<unsigned>(hr));
    } catch (...) {
        Wh_Log(L"[tray-force] exception while opening the flyout");
    }
    return true;   // the click is consumed: it must not end on a URI
}

static LRESULT CALLBACK OwnerWindowProc(HWND hwnd, UINT message, WPARAM wParam,
                                        LPARAM lParam) noexcept {
    try {
        // [diag-fix] Negligible cost after the first success (one atomic load): a safety net
        // for the case where pnidui.dll is loaded after InstallNetworkClickHooks (see the note
        // there), for instance after a real restart of explorer.exe rather than a plain reload
        // of the mod. This runs for every message, so the catch-up happens in practice at
        // once, long before the user can click the icon.
        TryHookPniduiShellExecuteExIat(L"OwnerWindowProc");
        if (g_taskbarCreatedMessage && message == g_taskbarCreatedMessage) {
            // The bar has been recreated: the forced registration has to be redone.
            Wh_Log(L"[tray-force] the notification bar has been recreated: "
                   L"the forced icon is published again");
            g_forcedPublished = false;
            g_verifiedLogged = false;
            g_firstTick = 0;
            return 0;
        }
        if (message == ShowNetworkFlyoutMessage()) {
            // A hook (or a module of the bar) has asked for the flyout: it is
            // opened here, on the thread of the icon's owner window, as a click would do.
            if (!g_unloading.load(std::memory_order_acquire)) OpenNativeNetworkFlyout(hwnd, false);
            return 0;
        }
        if (message == NetworkTrayForce::BatteryFlyout::ShowBatteryFlyoutMessage()) {
            // The click on the battery icon has asked for the Windows 10 battery
            // flyout: it is opened here, on the thread of the icon's owner window, with the
            // same authentic call the network flyout goes through.
            if (!g_unloading.load(std::memory_order_acquire))
                NetworkTrayForce::BatteryFlyout::OpenNativeBatteryFlyout(hwnd, L"left");
            return 0;
        }
        if (message == kCallbackMessage) {
            if (g_unloading.load(std::memory_order_acquire)) return 0;
            const UINT event = LOWORD(lParam);          // NIN_* or WM_*
            const UINT iconIdHi = HIWORD(lParam);       // version-4 only: icon ID here
            // Every callback from the icon is logged with its decoded event and the
            // version-4 icon-ID word, except plain mouse-move hover (by far the most frequent
            // one and never actionable), so a click that reaches here but does not match any
            // branch below is still visible without flooding the log.
            if (event != WM_MOUSEMOVE) {
                Wh_Log(L"[diag] icon callback: event=0x%04X iconIdHi=%u wParam=0x%p kIconId=%u",
                       event, iconIdHi, (void*)wParam, (unsigned)kIconId);
            }
            if (event == WM_LBUTTONUP || event == NIN_SELECT || event == NIN_KEYSELECT)
                OpenNativeNetworkFlyout(hwnd, false);
            else if (event == WM_RBUTTONUP || event == WM_CONTEXTMENU)
                OpenNativeNetworkFlyout(hwnd, true);   // pnidui's own menu (same entries)
            // WM_MOUSEMOVE / WM_LBUTTONDOWN / anything else: no action (the log above has
            // already said what matters, apart from the mouse-move).
            return 0;
        }
        // Diagnosed via a live DbgView capture: on at least one system, a second, genuine
        // registration for the same system network icon (GUID) appears some minutes after
        // this mod's forced one - almost certainly the authentic pnidui.dll (downloaded and
        // loaded by this same mod) completing its own native registration once its internal
        // state machine is ready. From that point on, real clicks are delivered to this same
        // owner window using THAT registration's callback message number instead of
        // kCallbackMessage - a RegisterWindowMessage-allocated value (high range, like
        // taskbarCreated), different on every boot, so it cannot be matched by a fixed
        // constant. It is still unmistakably an icon-callback payload for this exact icon:
        // HIWORD(lParam) carries the icon ID exactly like the version-4 convention used
        // above, and LOWORD(lParam) carries the same NIN_*/WM_* event codes. Rather than
        // depend on discovering the exact message (which is not a fixed value we can name
        // ahead of time), any unrecognized message in the RegisterWindowMessage range whose
        // payload decodes to our own icon ID is treated the same way a direct kCallbackMessage
        // callback would be. This keeps the flyout working even while a second, independent
        // registration for the same icon is alive in this process.
        if (message >= 0xC000 && message != g_taskbarCreatedMessage &&
            message != ShowNetworkFlyoutMessage() &&
            message != NetworkTrayForce::BatteryFlyout::ShowBatteryFlyoutMessage()) {
            const UINT event = LOWORD(lParam);
            const UINT iconIdHi = HIWORD(lParam);
            if (iconIdHi == kIconId &&
                (event == WM_LBUTTONUP || event == NIN_SELECT || event == NIN_KEYSELECT ||
                 event == WM_RBUTTONUP || event == WM_CONTEXTMENU) &&
                !g_unloading.load(std::memory_order_acquire)) {
                Wh_Log(L"[diag] unrecognized message 0x%04X decodes as an icon callback for "
                       L"this icon (event=0x%04X): treated like kCallbackMessage", message,
                       event);
                if (event == WM_LBUTTONUP || event == NIN_SELECT || event == NIN_KEYSELECT)
                    OpenNativeNetworkFlyout(hwnd, false);
                else
                    OpenNativeNetworkFlyout(hwnd, true);
                return 0;
            }
        }
        if (message == kMsgQuitOwner) {
            DestroyWindow(hwnd);
            return 0;
        }
        if (message == WM_DESTROY) {

            PostQuitMessage(0);
            return 0;
        }
    } catch (...) {
        Wh_Log(L"[tray-force] exception in the owner window of the icon");
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

// The class belongs to this module, not to the host.
//
// It used to be registered with the instance of explorer.exe and with the result of
// RegisterClassW ignored ("reuse the class if it is already there"). A class left behind by an
// earlier load whose thread never reached its own UnregisterClassW - a timeout on unload, a
// killed process - is exactly what must NOT be reused: its lpfnWndProc points into an image
// that Windhawk has already unmapped, and the first message to the window jumps into freed
// code. Registered under the module's own handle the class dies with the module instead, and a
// collision is reported as the failure it is.
static HINSTANCE OwnerClassInstance() {
    HINSTANCE instance = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(&OwnerWindowProc), &instance) ||
        !instance) {
        instance = GetModuleHandleW(nullptr);
    }
    return instance;
}

static DWORD WINAPI OwnerThreadProc(LPVOID) noexcept {
    HWND window = nullptr;
    HINSTANCE instance = nullptr;
    bool classRegistered = false;
    try {
        instance = OwnerClassInstance();
        WNDCLASSW wc = {};
        wc.lpfnWndProc = OwnerWindowProc;
        wc.hInstance = instance;
        wc.lpszClassName = kOwnerClass;
        if (!RegisterClassW(&wc)) {
            Wh_Log(L"[tray-force] the window class of the forced icon could not be registered "
                   L"(%lu): a class of this name is already there, the icon is not forced",
                   GetLastError());
        } else {
            classRegistered = true;
            window = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kOwnerClass, L"",
                                     WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, instance, nullptr);
            if (!window)
                Wh_Log(L"[tray-force] owner window not created (%lu): the forced icon "
                       L"not possible", GetLastError());
        }
        std::lock_guard<std::mutex> lock(g_ownerLock);
        g_ownerWindow = window;
        g_ownerThreadId = GetCurrentThreadId();
    } catch (...) {
        Wh_Log(L"[tray-force] exception in the creation of the owner window");
    }

    if (window) {
        MSG msg = {};
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    try {
        std::lock_guard<std::mutex> lock(g_ownerLock);
        g_ownerWindow = nullptr;
        g_ownerThreadId = 0;
        g_ownerRunning = false;
    } catch (...) {
    }
    // Only the class this thread registered is taken away: one that another owner had
    // registered before (the failure above) is not ours to destroy.
    if (classRegistered && instance) UnregisterClassW(kOwnerClass, instance);
    return 0;
}

static bool EnsureOwnerWindow() noexcept {
    try {
        if (g_unloading.load(std::memory_order_acquire)) return false;
        {
            std::lock_guard<std::mutex> lock(g_ownerLock);
            if (g_ownerWindow && IsWindow(g_ownerWindow)) return true;
            if (g_ownerRunning) return false;   // in creazione
            if (g_ownerThread) {
                if (WaitForSingleObject(g_ownerThread, 0) != WAIT_OBJECT_0) return false;
                CloseHandle(g_ownerThread);
                g_ownerThread = nullptr;
            }
            g_ownerRunning = true;
        }
        if (!g_taskbarCreatedMessage)
            g_taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");

        HANDLE thread = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_ownerLock);
            thread = CreateThread(nullptr, 0, OwnerThreadProc, nullptr, 0, nullptr);
            if (!thread) {
                g_ownerRunning = false;
                Wh_Log(L"[tray-force] owner thread not created (%lu)", GetLastError());
                return false;
            }
            g_ownerThread = thread;
        }
        // Short wait: the window is needed right away, for the registration
        const ULONGLONG start = GetTickCount64();
        while (GetTickCount64() - start < 2000) {
            std::lock_guard<std::mutex> lock(g_ownerLock);
            if (g_ownerWindow && IsWindow(g_ownerWindow)) return true;
            if (!g_ownerRunning) return false;
            Sleep(10);
        }
        Wh_Log(L"[tray-force] the owner window did not appear within 2 s");
        return false;
    } catch (...) {
        Wh_Log(L"[tray-force] exception while creating the owner window");
        return false;
    }
}

// ------------------------------------------------------ registrazione ------
static bool BuildTipText(wchar_t* out, size_t outCount) noexcept {
    if (!out || outCount < 2) return false;
    out[0] = L'\0';
    try {
        NetworkPniRegistration reg = {};
        if (CopyNetworkPniRegistration(&reg) && reg.hasTip && reg.tip[0]) {
            wcsncpy_s(out, outCount, reg.tip, _TRUNCATE);
            return true;
        }
        HMODULE pnidui = GetModuleHandleW(L"pnidui.dll");
        if (pnidui) {
            static const UINT kTipIds[] = { 3001, 3002, 3003, 3004, 3005, 3006, 3007, 3008,
                                            3010, 3011, 3012 };
            for (UINT id : kTipIds) {
                wchar_t buffer[128] = {};
                if (LoadStringW(pnidui, id, buffer, _countof(buffer)) > 0 && buffer[0]) {
                    wcsncpy_s(out, outCount, buffer, _TRUNCATE);
                    return true;
                }
            }
        }
        const LANGID lang = GetUserDefaultUILanguage();
        const wchar_t* fallback = (PRIMARYLANGID(lang) == LANG_ITALIAN) ? L"Rete" : L"Network";
        wcsncpy_s(out, outCount, fallback, _TRUNCATE);
        return true;
    } catch (...) {
        wcsncpy_s(out, outCount, L"Network", _TRUNCATE);
        return true;
    }
}

static bool PublishForcedRegistration() noexcept {
    try {
        HWND owner = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_ownerLock);
            owner = g_ownerWindow;
        }
        if (!owner || !IsWindow(owner)) return false;

        NetworkPniRegistration reg = {};
        reg.hwnd = owner;
        reg.id = kIconId;
        reg.callbackMessage = kCallbackMessage;
        reg.hasTip = BuildTipText(reg.tip, _countof(reg.tip));
        reg.hasGuid = false;                 // the GUID is applied by the NIM_ADD path
        reg.hasVersion = true;   // modern protocol: NIN_SELECT / NIN_KEYSELECT
        reg.version = NOTIFYICON_VERSION_4;
        reg.valid = true;
        reg.nativeFallbackRequired = true;   // the icon has to be drawn by the mod
        reg.forced = true;
        reg.generation = 0;
        {
            ScopedNetworkPniWriteLock lock;
            reg.generation = g_networkPniRegistration.generation + 1;
            g_networkPniRegistration = reg;
        }
        ++g_publishCount;
        Wh_Log(L"[tray-force] forced registration published (owner window 0x%p): "
               L"the existing NLM path draws the authentic icon and keeps it updated", owner);
        return true;
    } catch (...) {
        Wh_Log(L"[tray-force] exception while publishing the forced registration");
        return false;
    }
}

static bool QueryIconRect(const NetworkPniRegistration& reg, RECT* out) noexcept {
    try {
        NOTIFYICONIDENTIFIER identifier = {};
        identifier.cbSize = sizeof(identifier);
        identifier.hWnd = reg.hwnd;
        identifier.uID = reg.id;
        identifier.guidItem = kSystemNetworkIconGuid;
        HRESULT hr = Shell_NotifyIconGetRect(&identifier, out);
        if (FAILED(hr)) {
            identifier.guidItem = GUID_NULL;
            hr = Shell_NotifyIconGetRect(&identifier, out);
        }
        return SUCCEEDED(hr);
    } catch (...) {
        return false;
    }
}

static void VerifyForcedIcon() noexcept {
    try {
        const ULONGLONG now = GetTickCount64();
        if (now < g_nextVerify) return;
        g_nextVerify = now + kVerifyIntervalMs;

        NetworkPniRegistration reg = {};
        if (!CopyNetworkPniRegistration(&reg) || !reg.valid || !reg.forced) return;
        if (!reg.hwnd || !IsWindow(reg.hwnd)) return;

        RECT rect = {};
        if (QueryIconRect(reg, &rect)) {
            if (!g_verifiedLogged) {
                g_verifiedLogged = true;
                Wh_Log(L"[tray-force] VERIFIED: the network icon is present in the bar "
                       L"(rectangle %ld,%ld,%ld,%ld)", rect.left, rect.top, rect.right,
                       rect.bottom);
            }
            g_verifyFailures = 0;
            return;
        }
        ++g_verifyFailures;
        if (g_verifyFailures == 1 || g_verifyFailures % kMaxRetryLog == 0)
            Wh_Log(L"[tray-force] the icon is still not in the bar (attempt %d): "
                   L"the registration is published again", g_verifyFailures);
        // Re-publish: the NLM path tries NIM_ADD/MODIFY again
        if (!PublishForcedRegistration()) return;
    } catch (...) {
        Wh_Log(L"[tray-force] exception while verifying the icon");
    }
}

static void RetireForcedIcon(PCWSTR reason) noexcept {
    try {
        if (!g_forcedPublished) return;
        NetworkPniRegistration current = {};
        if (CopyNetworkPniRegistration(&current) && current.valid && !current.forced) {
            // The native PNI has taken over: the existing path retires the forced icon by
            // itself (different identity).
            if (!g_retiredLogged) {
                g_retiredLogged = true;
                Wh_Log(L"[tray-force] %s: the forced icon is withdrawn, the native one applies",
                       reason ? reason : L"native icon active");
            }
        }
        g_forcedPublished = false;
        g_verifiedLogged = false;
        g_verifyFailures = 0;
    } catch (...) {
    }
}

// ---------------------------------------------------------------- lifecycle --
static bool IsForcedOwnerWindow(HWND hwnd) noexcept {
    try {
        std::lock_guard<std::mutex> lock(g_ownerLock);
        return hwnd && g_ownerWindow && hwnd == g_ownerWindow;
    } catch (...) {
        return false;
    }
}

static void OnTaskbarTransition(bool up) noexcept {
    if (up == g_taskbarWasUp) return;
    g_taskbarWasUp = up;
    if (up) {
        g_firstTick = 0;
        g_forcedPublished = false;
        g_verifiedLogged = false;
        g_verifyFailures = 0;
        g_nextVerify = 0;
        g_guaranteesApplied = false;
        Wh_Log(L"[tray-force] the mod bar is active: escalation ladder rearmed");
    } else {
        Wh_Log(L"[tray-force] the mod bar is no longer active: waiting");
    }
}


static void Tick() noexcept {
    try {
        if (!g_cfg.forceNetworkTrayIcon || g_unloading.load(std::memory_order_acquire) ||
            g_stopping.load(std::memory_order_acquire)) return;
        const bool taskbarUp = IsLegacyShellProcess();
        OnTaskbarTransition(taskbarUp);
        if (!taskbarUp) return;
        if (g_reinstallRequested.exchange(false, std::memory_order_acq_rel)) {
            PublishForcedRegistration();
            g_forcedPublished = true;
            g_verifiedLogged = false;
            g_verifyFailures = 0;
        }

        ApplyVisibilityGuarantees();

        // The click on the network icon is taken over at the icon itself. This runs on the
        // thread of the mod, so no window is ever created or waited for inside a window
        // procedure of the shell (see the click takeover, above).
        ArmNetworkIconClickSubclass();
        ArmBatteryIconClickSubclass();

        NetworkPniRegistration reg = {};
        const bool have = CopyNetworkPniRegistration(&reg);
        const bool native = have && reg.valid && !reg.forced && IsPniduiServiceWindow(reg.hwnd);
        if (native) {
            RetireForcedIcon(L"native PNI icon active");
            return;
        }

        const ULONGLONG now = GetTickCount64();
        if (!g_firstTick) {
            g_firstTick = now;
            Wh_Log(L"[tray-force] supervision started: if within %d s the native PNI does not "
                   L"registers the icon, the mod registers it",
                   ClampValue(g_cfg.forceNetworkTrayDelaySec, 0, 600));
            return;
        }
        const ULONGLONG delayMs =
            static_cast<ULONGLONG>(ClampValue(g_cfg.forceNetworkTrayDelaySec, 0, 600)) * 1000;
        if (now - g_firstTick < delayMs) return;

        // The escalation is armed only if the icon has not appeared within the wait + 8 s.
        // Before that moment the bar is not touched.
        if (!g_escalate.load(std::memory_order_acquire) && !g_verifiedLogged &&
            now - g_firstTick > delayMs + 8000) {
            g_escalate.store(true, std::memory_order_release);
            Wh_Log(L"[tray-force] the icon did not appear within %llu s: enabling the guarantees on the "
                   L"bar (state saved and restorable). Up to here the bar has not been "
                   L"touched", (now - g_firstTick) / 1000);
        }

        if (!EnsureOwnerWindow()) return;

        if (!g_forcedPublished || !have || !reg.forced)
            g_forcedPublished = PublishForcedRegistration();

        if (g_forcedPublished) VerifyForcedIcon();
    } catch (...) {
        Wh_Log(L"[tray-force] exception in the escalation ladder loop");
    }
}

static void Shutdown() noexcept {
    try {
        if (g_stopping.exchange(true, std::memory_order_acq_rel)) return;

        // 1) retire the forced icon only (if a native one has replaced it, the hWnd/uID
        //    identity is not ours and nothing is touched).
        NetworkPniRegistration reg = {};
        const bool ours = CopyNetworkPniRegistration(&reg) && reg.valid && reg.forced &&
                          IsForcedOwnerWindow(reg.hwnd);
        if (ours) {
            NOTIFYICONDATAW nid = {};
            nid.cbSize = sizeof(nid);
            nid.hWnd = reg.hwnd;
            nid.uID = reg.id;
            nid.uFlags = NIF_GUID;
            nid.guidItem = kSystemNetworkIconGuid;
            if (CallNetworkIconNotify(NIM_DELETE, &nid))
                Wh_Log(L"[tray-force] forced icon removed");
            else
                Wh_Log(L"[tray-force] removing the forced icon failed");
        }

        // 2) close the owner window and join the thread completely. A timeout would not be
        // safe: the thread is still running code of this mod.
        HANDLE thread = nullptr;
        HWND window = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_ownerLock);
            window = g_ownerWindow;
            thread = g_ownerThread;
            g_ownerThread = nullptr;
        }
        if (window && IsWindow(window)) {
            PostMessageW(window, WM_CANCELMODE, 0, 0);
            PostMessageW(window, kMsgQuitOwner, 0, 0);
        }
        if (thread) {
            const DWORD waited = WaitForSingleObject(thread, INFINITE);
            if (waited != WAIT_OBJECT_0)
                Wh_Log(L"[tray-force] waiting for the owner thread failed (%lu)", GetLastError());
            CloseHandle(thread);
        }
    } catch (...) {
        Wh_Log(L"[tray-force] exception during unload");
    }
}

static void SettingsChanged() noexcept {
    try {
        g_stopping.store(false, std::memory_order_release);
        g_guaranteesApplied = false;
    } catch (...) {
    }
}

}  // namespace NetworkTrayForce

// To be used ONLY with g_realExePath: after the path spoof GetModuleFileNameW
// answers with the fake path, and asking it whether we are the private shell
// gave the wrong answer (bug seen in the 14:49 log).
static bool IsProcessImageName(const wchar_t* path, const wchar_t* expected) noexcept {
    if (!path || !expected) return false;
    const wchar_t* base = wcsrchr(path, L'\\');
    return _wcsicmp(base ? base + 1 : path, expected) == 0;
}

static bool IsPrivateExplorerProcess() {
    if (g_realExePath[0] == 0) return false;
    return _wcsicmp(g_realExePath, g_cfg.explorerPath) == 0;
}

// ---- explorer.exe ---------------------------------------------------------

// The Windows 10 shell is an explorer.exe that is not the system one: the file name alone is
// not enough, because the Windows 11 shell - and every File Explorer window the user opens in
// its own process - is an explorer.exe as well. The comparison is therefore made on the full
// path of the running image.
static bool IsLegacyShellProcess() {
    if (g_realExePath[0] == 0) {
        wchar_t path[MAX_PATH] = {};
        if (GetModuleFileNameW(nullptr, path, _countof(path))) wcscpy_s(g_realExePath, path);
    }
    const wchar_t* path = g_realExePath;
    if (!path[0]) return false;
    if (IsPrivateExplorerProcess()) return true;

    // The full path of the system shell is <Windows>\explorer.exe. GetWindowsDirectoryW is the
    // documented source of <Windows>, with SystemRoot as a fallback. Note the doubled backslash
    // below: "\explorer.exe" is the ESC escape sequence, such a literal never matches a path,
    // and the mod would then run in the Windows 11 shell as well.
    wchar_t windowsDir[MAX_PATH] = {};
    DWORD dirChars = GetWindowsDirectoryW(windowsDir, _countof(windowsDir));
    if (!dirChars || dirChars >= _countof(windowsDir)) {
        dirChars = GetEnvironmentVariableW(L"SystemRoot", windowsDir, _countof(windowsDir));
        if (!dirChars || dirChars >= _countof(windowsDir)) return false;
    }
    while (dirChars > 0 && windowsDir[dirChars - 1] == L'\\') {
        windowsDir[--dirChars] = L'\0';
    }

    static constexpr wchar_t kSystemShellTail[] = L"\\explorer.exe";
    const size_t pathChars = wcslen(path);
    const bool isSystemShell =
        pathChars == dirChars + (_countof(kSystemShellTail) - 1) &&
        _wcsnicmp(path, windowsDir, dirChars) == 0 &&
        _wcsicmp(path + dirChars, kSystemShellTail) == 0;
    if (isSystemShell) return false;

    // Any other explorer.exe is the private Windows 10 shell of the companion mod
    // (win10-taskbar-on-win11-24h2), which lives in the store folder, not in <Windows>.
    return IsProcessImageName(path, L"explorer.exe");
}

}

using namespace RestorerTaskbar;

// ---------------------------------------------------------------------------
// Where the downloaded Windows 10 files live
//
// The folder the Windows 10 taskbar mod by Anixx uses for its own download:
// %ProgramData%\Windhawk\Engine\ModsWritable\LegacyStore. Both mods then read and write
// a single set of verified files instead of keeping two copies of the same system
// binaries: a file already there is reused as soon as it matches this mod's pin, and
// only what is missing is downloaded into that folder. If the folder cannot be created
// or written (no rights over ProgramData), this mod's own storage is used as a
// fallback, so the mod keeps working on its own.
// ---------------------------------------------------------------------------
static const wchar_t kSharedStorePath[] =
    L"%ProgramData%\\Windhawk\\Engine\\ModsWritable\\LegacyStore";

static bool DirectoryAcceptsWrites(const wchar_t* dir) {
    wchar_t probe[MAX_PATH] = {};
    _snwprintf_s(probe, _countof(probe), _TRUNCATE, L"%s\\.w10er-write-probe", dir);
    HANDLE file = CreateFileW(probe, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                              FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    CloseHandle(file);
    return true;
}

static bool ResolveStorePath(wchar_t* out, size_t count) {
    out[0] = 0;
    wchar_t shared[MAX_PATH] = {};
    if (ExpandEnvironmentStringsW(kSharedStorePath, shared, _countof(shared)) > 0 && shared[0]) {
        if (EnsureDirectory(shared) && DirectoryAcceptsWrites(shared)) {
            Wh_Log(L"[store] shared folder: %s (the same one the Win10 taskbar mod uses)", shared);
            wcsncpy_s(out, count, shared, _TRUNCATE);
            return true;
        }
        Wh_Log(L"[store] the shared folder is not writable (%s): this mod's own storage is used",
               shared);
    }
    wchar_t storage[MAX_PATH] = {};
    if (!Wh_GetModStoragePath(storage, _countof(storage)) || !storage[0]) return false;
    _snwprintf_s(out, count, _TRUNCATE, L"%s\\Win10Shell", storage);
    Wh_Log(L"[store] private folder: %s", out);
    return true;
}

static void LoadFlyoutSettings() {
    // Data folder: the folder the Windows 10 taskbar mod uses for its own download, so
    // the two mods share one set of verified files (see ResolveStorePath above).
    if (!ResolveStorePath(g_cfg.storePath, _countof(g_cfg.storePath))) g_cfg.storePath[0] = 0;
    _snwprintf_s(g_cfg.explorerPath, _countof(g_cfg.explorerPath), _TRUNCATE,
                 L"%s\\explorer.exe", g_cfg.storePath);

    g_cfg.provideTrayDlls = Wh_GetIntSetting(L"ProvideTrayModules") != 0;
    g_cfg.requireSignature = Wh_GetIntSetting(L"RequireSignature") != 0;
    g_cfg.downloadTimeoutSec = Wh_GetIntSetting(L"DownloadTimeoutSec");
    if (g_cfg.downloadTimeoutSec <= 0) g_cfg.downloadTimeoutSec = 20;

    // The reference mod's tray/network options: without these the code that forces the
    // network icon is switched off (the two flags are read in other blocks).
    g_cfg.forceNetworkTrayIcon = Wh_GetIntSetting(L"ForceNetworkTrayIcon") != 0;
    g_cfg.forceNetworkTrayDelaySec = Wh_GetIntSetting(L"ForceNetworkTrayDelaySec");
    if (g_cfg.forceNetworkTrayDelaySec < 0) g_cfg.forceNetworkTrayDelaySec = 0;
    if (g_cfg.forceNetworkTrayDelaySec > 600) g_cfg.forceNetworkTrayDelaySec = 600;
    g_cfg.forceNetworkTrayResetTraySettings =
        Wh_GetIntSetting(L"ForceNetworkTrayResetTraySettings") != 0;
    g_cfg.shellOpGuardTimeoutMs = Wh_GetIntSetting(L"ShellOpGuardTimeoutMs");
    if (g_cfg.shellOpGuardTimeoutMs < 200) g_cfg.shellOpGuardTimeoutMs = 200;
    if (g_cfg.shellOpGuardTimeoutMs > 10000) g_cfg.shellOpGuardTimeoutMs = 10000;
    g_logTrayActivity = Wh_GetIntSetting(L"LogTrayActivity") != 0;
}
// ---------------------------------------------------------------------------
// The two tray windows that own a menu of their own
//
// The show desktop button and the clock are children of the taskbar, created by the
// shell's own thread: the same cross-thread subclass machinery the indicator uses in
// the other module is used here (WindhawkUtils, five-parameter procedure). If the
// shell has shown no menu for the click, this mod shows the Windows 10 one - the two
// fallback menus defined above, which exist for exactly this case.
// ---------------------------------------------------------------------------
typedef LRESULT (*IndicatorSubclassFn)(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);

class ScopedWindowSubclass {
public:
    ScopedWindowSubclass(HWND w, IndicatorSubclassFn proc, DWORD_PTR ref, bool* installed)
        : m_w(w), m_proc(proc), m_ref(ref), m_installed(installed) {
        if (m_installed) *m_installed = false;
        if (m_w) WindhawkUtils::RemoveWindowSubclassFromAnyThread(m_w, m_proc);
    }
    ~ScopedWindowSubclass() {
        if (!m_w) return;
        const bool ok = WindhawkUtils::SetWindowSubclassFromAnyThread(m_w, m_proc, m_ref);
        if (m_installed) *m_installed = ok;
    }
    ScopedWindowSubclass(const ScopedWindowSubclass&) = delete;
    ScopedWindowSubclass& operator=(const ScopedWindowSubclass&) = delete;
private:
    HWND m_w;
    IndicatorSubclassFn m_proc;
    DWORD_PTR m_ref;
    bool* m_installed;
};

// explorer creates the cell and its buttons at different times (the 18:14 log
// shows a first, empty instance of the cell before the real one, and the button
// appearing afterwards), so the scan runs on every tick and reports what it
// finds; while nothing is there it says so, so an absent cell never looks like a

// --- the two tray windows that own those menus -----------------------------
// the show desktop button and the clock are children of the taskbar, created by
// the shell's own thread: the cross-thread subclass machinery of WindhawkUtils is used
// (five-parameter procedure), and the
// same rules apply - every call inside try/catch, RAII for the handles.

static const wchar_t* const kTrayMenuClasses[] = {
    L"TrayShowDesktopButtonWClass",
    L"TrayClockWClass",
    L"ClockButton",
};
static const int kTrayMenuClassCount = (int)(sizeof(kTrayMenuClasses) / sizeof(kTrayMenuClasses[0]));

struct TrayMenuTarget {
    HWND wnd;
    bool subclassed;
};
static TrayMenuTarget g_trayMenuTargets[8] = {};
static int g_trayMenuTargetCount = 0;
static const UINT_PTR kTrayMenuSubclassId = 79;
static DWORD g_lastTrayMenuArm = 0;

static bool IsTrayMenuClass(const wchar_t* cls) {
    if (!cls || !cls[0]) return false;
    for (int i = 0; i < kTrayMenuClassCount; i++)
        if (_wcsicmp(cls, kTrayMenuClasses[i]) == 0) return true;
    return false;
}

static int TrackedMenuIndex(HWND w) {
    for (int i = 0; i < g_trayMenuTargetCount; i++)
        if (g_trayMenuTargets[i].wnd == w) return i;
    return -1;
}

static void UntrackMenuTarget(int idx) {
    if (idx < 0 || idx >= g_trayMenuTargetCount) return;
    for (int i = idx; i + 1 < g_trayMenuTargetCount; i++)
        g_trayMenuTargets[i] = g_trayMenuTargets[i + 1];
    g_trayMenuTargets[--g_trayMenuTargetCount] = TrayMenuTarget{};
}

struct TrayMenuEnumCtx {
    HWND found[8];
    int count;
};

static BOOL CALLBACK TrayMenuEnumProc(HWND w, LPARAM param) {
    TrayMenuEnumCtx* ctx = (TrayMenuEnumCtx*)param;
    if (!ctx || ctx->count >= (int)(sizeof(ctx->found) / sizeof(ctx->found[0]))) return FALSE;
    wchar_t cls[64] = {};
    GetClassNameW(w, cls, _countof(cls));
    if (IsTrayMenuClass(cls)) ctx->found[ctx->count++] = w;
    return TRUE;
}

// Answers the right click on those two windows. If explorer has already shown a
// menu for this very window in this click, nothing is done here: the entries
// have been completed in the hook instead, and a second menu would be wrong.
static LRESULT TrayMenuSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                    DWORD_PTR refData) {
    (void)wParam;
    (void)refData;
    try {
        switch (msg) {
        case WM_CONTEXTMENU: {
            wchar_t cls[64] = {};
            GetClassNameW(hwnd, cls, _countof(cls));
            if (IsShowDesktopClass(cls)) {
                // Windows 10 gives this button its own small menu: that one is the
                // mod's, because explorer has none.
                ShowShowDesktopMenu(hwnd);
                return 0;
            }

            break;
        }
        case WM_NCDESTROY: {
            const int idx = TrackedMenuIndex(hwnd);
            if (idx >= 0) {
                UntrackMenuTarget(idx);
                Wh_Log(L"[menu] tray menu window 0x%p went away: supervision removed", (void*)hwnd);
            }
            break;
        }
        default:
            break;
        }
        (void)lParam;
    } catch (...) {
        Wh_Log(L"[menu] exception while answering a tray menu request");
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

static void ArmTrayMenuSubclass() {
    try {
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (!tray) return;
        TrayMenuEnumCtx ctx = {};
        EnumChildWindows(tray, TrayMenuEnumProc, (LPARAM)&ctx);

        bool added = false;
        for (int i = 0; i < ctx.count; i++) {
            HWND w = ctx.found[i];
            if (!w || !IsWindow(w)) continue;
            if (TrackedMenuIndex(w) >= 0) continue;
            if (g_trayMenuTargetCount >= (int)(sizeof(g_trayMenuTargets) / sizeof(g_trayMenuTargets[0])))
                break;
            wchar_t cls[64] = {};
            GetClassNameW(w, cls, _countof(cls));
            g_trayMenuTargets[g_trayMenuTargetCount].wnd = w;
            g_trayMenuTargets[g_trayMenuTargetCount].subclassed = false;
            g_trayMenuTargetCount++;
            added = true;
            Wh_Log(L"[menu] tray menu window found (%s 0x%p): the mod answers its right click",
                   cls[0] ? cls : L"window", (void*)w);
        }

        for (int i = g_trayMenuTargetCount - 1; i >= 0; i--) {
            wchar_t cls[64] = {};
            HWND w = g_trayMenuTargets[i].wnd;
            if (!w || !IsWindow(w) || !GetClassNameW(w, cls, _countof(cls)) || !IsTrayMenuClass(cls))
                UntrackMenuTarget(i);
        }

        const DWORD now = GetTickCount();
        if (!added && now - g_lastTrayMenuArm < 10000) return;
        g_lastTrayMenuArm = now;
        for (int i = 0; i < g_trayMenuTargetCount; i++) {
            HWND w = g_trayMenuTargets[i].wnd;
            if (!w || !IsWindow(w)) continue;
            wchar_t cls[64] = {};
            GetClassNameW(w, cls, _countof(cls));
            bool installed = false;
            {
                ScopedWindowSubclass slot(w, TrayMenuSubclassProc, kTrayMenuSubclassId, &installed);
            }
            const bool first = !g_trayMenuTargets[i].subclassed;
            g_trayMenuTargets[i].subclassed = installed;
            if (installed && first)
                Wh_Log(L"[menu] subclass installed on %s 0x%p (its menu is the mod's)", cls, (void*)w);
            else if (!installed) {
                static DWORD lastReport = 0;
                if (!lastReport || now - lastReport > 30000) {
                    lastReport = now;
                    Wh_Log(L"[menu] the shell did not accept the subclass on %s 0x%p, retrying",
                           cls, (void*)w);
                }
            }
        }
    } catch (...) {
        Wh_Log(L"[menu] exception while supervising the tray menu windows");
    }
}

// ---------------------------------------------------------------------------
// The Windows 10 tray DLLs: this module downloads and verifies them itself
//
// The data folder is this mod's own storage (…\Win10Shell). A file already there is reused
// (same SHA-256, same signature); nothing assumes that another mod ran first. If a download
// fails the tray part stays off instead of holding up the shell, and it is retried later.
// ---------------------------------------------------------------------------
static bool g_trayStoreReady = false;

static bool EnsureTrayStoreFile(const wchar_t* name, const wchar_t* symbolId,
                                const wchar_t* sha256) {
    wchar_t path[MAX_PATH] = {};
    if (EnsureVerifiedFile(g_cfg.storePath, name, symbolId, sha256, path, _countof(path))) {
        Wh_Log(L"[store] %s ready (%s)", name, path);
        return true;
    }
    Wh_Log(L"[store] %s not available: the download or the verification failed", name);
    return false;
}

static bool PrepareTrayStore() {
    if (g_trayStoreReady) return true;
    if (!g_cfg.storePath[0]) {
        Wh_Log(L"[store] no data folder available: the Windows 10 tray DLLs cannot be fetched");
        return false;
    }
    if (!EnsureDirectory(g_cfg.storePath)) return false;

    bool complete = true;
    for (const PinnedFile& file : kTrayFiles) {
        // The unload has priority over the store: what is not there yet is simply missing,
        // and the next load of the mod starts from the files that survived on disk.
        if (g_unloading.load(std::memory_order_acquire)) return false;
        if (!EnsureTrayStoreFile(file.name, file.symbolId, file.sha256)) complete = false;
    }
    // explorer.exe: the private Windows 10 shell itself. Same symbol-server entry and
    // same pin the Windows 10 taskbar mod uses, so the file and its locale folder are
    // shared with it: whoever needs it first downloads it, the other one finds it
    // verified and does not download it again.
    if (g_cfg.provideTrayDlls && !g_unloading.load(std::memory_order_acquire)) {
        wchar_t explorerPath[MAX_PATH] = {};
        if (EnsureVerifiedFile(g_cfg.storePath, kExplorerFile.name, kExplorerFile.symbolId,
                               kExplorerFile.sha256, explorerPath, _countof(explorerPath))) {
            Wh_Log(L"[store] explorer.exe ready (%s)", explorerPath);
        } else {
            Wh_Log(L"[store] explorer.exe not available: the private shell cannot start until "
                   L"the file is there (the Win10 taskbar mod downloads the same one)");
        }
    }

    g_trayStoreReady = complete;
    return complete;
}

// ---------------------------------------------------------------------------
// This module's own thread
//
// The hooks are registered in Wh_ModInit, on the engine's thread; this thread only does the
// waiting work: the tray watch, the icon registration it forces, the timed shell operations.
// Nothing on it can block a shell hook, and it never holds up a thread of the shell: it pumps
// messages for the hidden owner window that owns the forced tray icon.
// ---------------------------------------------------------------------------
static HANDLE g_stopEvent = nullptr;
static HANDLE g_servicesThread = nullptr;
static DWORD g_trayThreadId = 0;

// The retry of a failed download, with a backoff.
//
// A missing file used to be asked again on every tick of this thread, that is five times a
// second: an offline machine, a proxy that blocks msdl.microsoft.com, a non-200 answer or the
// HTML page of a captive portal meant a download attempt per tick, each one re-hashing the
// files that are already in the store (explorer.exe among them). The first retry now waits a
// minute and the wait doubles up to fifteen minutes; a settings change starts over at once.
static constexpr DWORD kTrayStoreRetryBaseMs = 60000;
static constexpr DWORD kTrayStoreRetryMaxMs = 15 * 60000;
static ULONGLONG g_trayStoreNextTry = 0;
static DWORD g_trayStoreRetryDelayMs = kTrayStoreRetryBaseMs;
static int g_trayStoreFailures = 0;

static void ResetTrayStoreBackoff() noexcept {
    g_trayStoreNextTry = 0;
    g_trayStoreRetryDelayMs = kTrayStoreRetryBaseMs;
    g_trayStoreFailures = 0;
}

static bool TrayStoreAttemptIsDue() noexcept {
    return GetTickCount64() >= g_trayStoreNextTry;
}

static void NoteTrayStoreAttempt(bool ok) noexcept {
    if (ok) {
        ResetTrayStoreBackoff();
        return;
    }
    ++g_trayStoreFailures;
    const ULONGLONG now = GetTickCount64();
    g_trayStoreNextTry = now + g_trayStoreRetryDelayMs;
    if (g_trayStoreRetryDelayMs < kTrayStoreRetryMaxMs) {
        g_trayStoreRetryDelayMs *= 2;
        if (g_trayStoreRetryDelayMs > kTrayStoreRetryMaxMs)
            g_trayStoreRetryDelayMs = kTrayStoreRetryMaxMs;
    }
    Wh_Log(L"[store] attempt %d failed: the Windows 10 files are asked again in %lu s",
           g_trayStoreFailures, (g_trayStoreRetryDelayMs / 1000));
}

static void TrayThreadWork(bool firstRun) {
    if (firstRun) {
        // Which process this is, in the terms the tray code uses: the private Windows 10
        // shell. Without this the redirect decisions answer "not our shell" and the tray
        // support never runs.
        NativeUi::privateExplorer = IsPrivateExplorerProcess();
        Wh_Log(L"[flyout] process role: private shell=%d", NativeUi::privateExplorer ? 1 : 0);
        Wh_Log(L"[flyout] the Windows 10 tray is being restored in this shell "
               L"(tray modules=%s)",
               g_cfg.provideTrayDlls ? L"on" : L"off");
    }

    // The DLLs come from this mod: downloaded if missing, verified against the pinned
    // SHA-256 (and signature) if present. Nothing is assumed to be already there.
    if (g_cfg.provideTrayDlls) {
        // While the backoff of a failed attempt is running the store is not asked again: the
        // tray work goes on with whatever is on disk, without touching the network.
        if (g_trayStoreReady || TrayStoreAttemptIsDue())
            NoteTrayStoreAttempt(PrepareTrayStore());
    } else {
        static bool reported = false;
        if (!reported) {
            reported = true;
            Wh_Log(L"[store] the tray modules are switched off by the settings");
        }
    }

    if (!g_traySupportInstalled) InstallTraySupport();

    // The click on the network icon is taken over at the icon itself, and this
    // thread registers nothing. The interception is a window subclass, installed by
    // ArmNetworkIconClickSubclass further down: it runs on this thread, so no window is ever
    // created or waited for inside a window procedure of the shell. The two entry points of
    // the click are registered in Wh_ModInit together with every other hook of this mod (see
    // the note there): a registration made here comes after Wh_ModInit returned, so the
    // engine has already applied the hook queue and the hook would stay inert, and doing it
    // from this thread is also what made explorer.exe crash whenever the mod was enabled or
    // disabled, because the hook queue is then operated on by two threads at once.

    // The clock and the show desktop button: if the shell shows no menu of its own,
    // the Windows 10 one of this mod answers the right click.
    ArmTrayMenuSubclass();

    RetryTrayModulesIfNeeded();

    // The three steps the reference mod runs on every tray tick while the private shell
    // is up. The last one is the ladder that FORCES the network icon: the SSO of
    // pnidui.dll is nudged first, then the state fallback is given a chance, and only
    // after the waiting time does the mod register the icon itself (and verify it).
    // Without these three calls the support is "active" but nothing is ever registered,
    // which is exactly what the log showed.
    if (IsLegacyShellProcess()) {
        try {
            EnsureNativeNetworkTraySso();
            NetworkIconFallbackTick();
            NetworkTrayForce::Tick();
        } catch (...) {
            Wh_Log(L"[tray] exception in the network icon step: the shell is unaffected");
        }
    }

    // No hook is registered from this thread any more, so
    // there is nothing to apply here either. Calling Wh_ApplyHookOperations from a worker
    // while the engine is initializing or unloading the mod is what made Explorer die on
    // every enable and every disable: the queue is applied by the engine itself at the end
    // of Wh_ModInit, where every Wh_SetFunctionHook of this mod now lives.
    if (g_logTrayActivity && firstRun)
        Wh_Log(L"[tray] per-operation logging is on: every load and menu operation is logged");
}

static DWORD WINAPI FlyoutServicesThread(LPVOID) {
    g_trayThreadId = GetCurrentThreadId();
    MSG queueInit = {};
    PeekMessageW(&queueInit, nullptr, 0, 0, PM_NOREMOVE);

    // The engine owns the settings while it is tearing the mod down: the thread has nothing to
    // read any more, and a settings call here could be the last thing it does.
    if (!g_unloading.load(std::memory_order_acquire)) LoadFlyoutSettings();
    try {
        TrayThreadWork(true);
    } catch (...) {
        Wh_Log(L"[flyout] exception in the tray thread: the shell is unaffected");
    }

    for (;;) {
        if (g_unloading.load(std::memory_order_acquire)) break;
        // 200 ms: the period of the tray ticks is unchanged, but unloading does not
        // wait for a long timeout when the stop event is already set.
        if (WaitForSingleObject(g_stopEvent, 200) == WAIT_OBJECT_0) break;
        if (g_unloading.load(std::memory_order_acquire)) break;
        try {
            TrayThreadWork(false);
        } catch (...) {
            Wh_Log(L"[flyout] exception in the tray thread: the shell is unaffected");
        }
    }

    // TEARDOWN ON THIS THREAD, WHILE THE MODULE IS STILL LOADED.
    //
    // Everything the tray work created belongs to this thread: the forced network icon, the
    // hidden owner window and its thread, the fallback icon, and the subclasses installed on
    // the shell's clock / show-desktop windows. An early revision retired the first three
    // from Wh_ModUninit, that is on the engine's thread and after Windhawk had already removed the
    // hooks, and it never removed the subclasses at all: the window procedure of those shell
    // windows still pointed into this module when it was unloaded, so the next message to the
    // clock (which repaints every second) jumped into freed code and took explorer.exe with it.
    // No window procedure of the shell may outlive this module.
    // The click takeover is a subclass on a window of the shell: left in place, its window
    // procedure would point into this module once the module is unmapped. It is removed
    // First.
    // The backoff is this thread's own state: the next load of the mod starts from scratch.
    ResetTrayStoreBackoff();

    NetworkTrayForce::DisarmNetworkIconClickSubclass();
    NetworkTrayForce::DisarmBatteryIconClickSubclass();
    // The right-click supervision subclass on the pnidui/stobject service windows
    // (g_trayWnds[]) used to be installed with a raw SetWindowSubclass and never removed here -
    // the window procedure kept pointing into this module after unload, which could take the
    // next message to that window (and the shell with it) into freed code. It is removed the
    // same way every other shell-window subclass in this thread is.
    for (int i = g_trayWndCount - 1; i >= 0; i--) {
        HWND wnd = g_trayWnds[i];
        if (wnd && IsWindow(wnd)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(wnd, NetworkIconSubclassProc);
            Wh_Log(L"[tray] right-click supervision subclass removed from 0x%p while unloading the mod",
                   (void*)wnd);
        }
        g_trayWnds[i] = nullptr;
    }
    g_trayWndCount = 0;
    NetworkIconFallbackShutdown();
    NetworkTrayForce::Shutdown();
    for (int i = g_trayMenuTargetCount - 1; i >= 0; i--) {
        HWND wnd = g_trayMenuTargets[i].wnd;
        if (wnd) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(wnd, TrayMenuSubclassProc);
            Wh_Log(L"[menu] subclass removed from 0x%p while unloading the mod", (void*)wnd);
        }
        UntrackMenuTarget(i);
    }

    return 0;
}

// ===========================================================================
// The process that draws the flyouts: the Windows 10 template set.
//
// The network flyout and the battery flyout are not drawn by Explorer: ShellExperienceHost.exe
// draws them. That process starts, the mod asks this shell for the Windows 10 flyout, the
// window appears and is taken down again at once (a new ShellExperienceHost.exe for every
// click) - and the battery one does not even get that far, because its flyout makes that
// process go away while it is being built.
//
// The reason is in Windows.UI.QuickActions.dll, the module this shell builds the flyout with:
// from build 25951 on it enters the flyout through the new "Control Center" template set, which
// the Windows 10 flyouts cannot be built with. ExplorerPatcher fixes exactly this on these
// builds ("Fix battery flyout crashing on 25951+") by making the shell load the older
// QuickActions template set instead: five bytes are turned into NOPs, eight bytes are copied
// from the older template loader and one call is pointed at it. This block does the same, on
// the module the shell has loaded, the moment that module is loaded.
//
// The module is NOT loaded, and cannot be loaded, when the mod starts: a LoadLibrary of it from
// the mod init fails (the log of the previous round says "not available (error 1114)"). So the
// mod does not load it: it watches the loads of this process (LdrLoadDll) and patches the
// module as soon as it appears, which is before the shell has built anything with it.
//
// Nothing else is touched: only those bytes of that one module, in memory, in this process.
// If the patterns are not found (a build this code does not know) nothing at all is written
// and the log says so.
// ===========================================================================
// ===========================================================================
// RAII: the resources this mod borrows and gives back.
//
// The mod writes into memory at two places, and at both the write is a pair of brackets:
// The page comes back as it was when the bracket closes.
//
//   * ScopedWriteProtect: makes a page writable and puts it back the way it was when the
//     object goes out of scope - also when the middle of the block leaves with a return,
//     or when the code raises a C++ exception (which the caller catches higher up).
//
// It sits in front of the block that borrows it (the Windows 10 template set of the flyout
// process), because that block must not have to know how a page is unlocked.
// ScopedImportRedirect - the same idea for one entry of the import table - went away
// together with NetworkUxHostPatch, its only user.
// ===========================================================================
class ScopedWriteProtect {
public:
    ScopedWriteProtect(void* address, size_t size) noexcept {
        if (!address || !size) return;
        m_address = address;
        m_size = size;
        m_ok = VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &m_previous) != FALSE;
    }
    ~ScopedWriteProtect() { Restore(); }
    ScopedWriteProtect(const ScopedWriteProtect&) = delete;
    ScopedWriteProtect& operator=(const ScopedWriteProtect&) = delete;
    explicit operator bool() const noexcept { return m_ok; }
    void Restore() noexcept {
        if (!m_ok) return;
        DWORD ignored = 0;
        VirtualProtect(m_address, m_size, m_previous, &ignored);
        m_ok = false;
    }
private:
    void* m_address = nullptr;
    size_t m_size = 0;
    DWORD m_previous = 0;
    bool m_ok = false;
};

namespace FlyoutHostPatch {

static std::atomic<bool> g_patched{false};
static std::atomic<int> g_logs{0};
// What this copy of the mod wrote, so that it can be given back. Only the bytes written by
// this very instance are restored: a site that was already carrying them (the mod reloaded
// on a settings change, or the shell loaded the module again) was not touched here and is
// left as it is.
static unsigned char g_savedBytes[80];
static unsigned char* g_writtenAt = nullptr;
static std::atomic<bool> g_canRestore{false};
// The module has been pinned in memory (GetModuleHandleExW with
// GET_MODULE_HANDLE_EX_FLAG_PIN). Once per process: a pin is not redone and not undone.
static std::atomic<bool> g_pinned{false};

// ---- searching in the bytes: 'x' = exact byte, '?' = anything ------------------
static bool MaskMatches(const unsigned char* at, const unsigned char* pattern,
                        const char* mask) noexcept {
    for (size_t i = 0; mask[i]; ++i)
        if (mask[i] == 'x' && at[i] != pattern[i]) return false;
    return true;
}

static unsigned char* FindPattern(unsigned char* begin, size_t size,
                                  const unsigned char* pattern, const char* mask) noexcept {
    if (!begin || !pattern || !mask) return nullptr;
    const size_t length = strlen(mask);
    if (!length || size < length) return nullptr;
    for (size_t i = 0; i + length <= size; ++i)
        if (MaskMatches(begin + i, pattern, mask)) return begin + i;
    return nullptr;
}

static unsigned char* TextSectionOf(HMODULE module, size_t* size) noexcept {
    if (size) *size = 0;
    if (!module) return nullptr;
    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(module);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return nullptr;
    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS*>(
        reinterpret_cast<unsigned char*>(module) + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return nullptr;
    auto* section = IMAGE_FIRST_SECTION(nt);
    for (unsigned int i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++section) {
        if (!section->SizeOfRawData) continue;
        const char* name = reinterpret_cast<const char*>(section->Name);
        if ((section->Characteristics & IMAGE_SCN_CNT_CODE) || strncmp(name, ".text", 5) == 0) {
            if (size) *size = section->SizeOfRawData;
            return reinterpret_cast<unsigned char*>(module) + section->VirtualAddress;
        }
    }
    return nullptr;
}

// ---- the swap of the template set (ExplorerPatcher, build 25951 and later) ----
//
// The older template loader ("ref new QuickActions::QuickActionTemplates()"):
//   48 89 45 50 BA 90 00 00 00 8D 4A E8 FF 15 ?? ?? ?? ?? 48 89 45 58 48 8B C8 E8 ?? ?? ?? ?? 48 8B F0
// the five bytes of the getter call that follows are read from here; the
// place that builds the flyout with the new toolset, and that is changed:
//   ... the 5 bytes of "call LoadComponent" become NOP, the 8 bytes of the "mov edx / lea rcx"
//   pair are copied from the older loader (the size the template needs), and the last call is
//   pointed at the older getter.
static const unsigned char kSourcePattern[] = {
    0x48, 0x89, 0x45, 0x50, 0xBA, 0x90, 0x00, 0x00, 0x00, 0x8D, 0x4A, 0xE8, 0xFF, 0x15,
    0x00, 0x00, 0x00, 0x00, 0x48, 0x89, 0x45, 0x58, 0x48, 0x8B, 0xC8, 0xE8, 0x00, 0x00,
    0x00, 0x00, 0x48, 0x8B, 0xF0,
};
static const char kSourceMask[] = "xxxxxxxxxxxxxx????xxxxxxxx????xxx";

static const unsigned char kTargetPattern[] = {
    0x48, 0x8B, 0xD0, 0x48, 0x8B, 0xCB, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x90, 0x4D, 0x85,
    0x00, 0x74, 0x10, 0x49, 0x8B, 0x00, 0x49, 0x8B, 0x00, 0x48, 0x8B, 0x40, 0x10, 0xE8,
    0x00, 0x00, 0x00, 0x00, 0x90, 0x49, 0x8B, 0xCC,
    0xE8, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8B, 0xD3, 0x48, 0x8B, 0xCE, 0xE8, 0x00, 0x00,
    0x00, 0x00, 0xBA, 0xB8, 0x00, 0x00, 0x00, 0x8D, 0x4A, 0xE8, 0xFF, 0x15, 0x00, 0x00,
    0x00, 0x00, 0x48, 0x89, 0x45, 0x00,
    0x48, 0x8B, 0xC8, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x4C, 0x8B,
};
static const char kTargetMask[] = "xxxxxxx????xxx?xxxx?xx?xxxxx????xxxx"
                                  "x????xxxxxxx????xxxxxxxxxx????xxx?"
                                  "xxxx????xx";

// The same mask with the groups this patch rewrites left free: indices 6-10 (the five NOPs
// that replaced the "call LoadComponent": in the full mask 6 is 'x' and 7-10 are already
// '?'), 52-59 (the eight bytes copied from the older loader: in the full mask 52-61 are 'x',
// and the only byte that really changes is 53, 0xB8 -> 0x90) and 74-77 (the rel32, already
// '?'). It exists to recognize a module that ALREADY CARRIES the patch by its content,
// instead of trusting the base address (ASLR can hand the same address to a new copy) or a
// flag (which says "I did it", not "the module in memory is rewritten"). The search with the
// intact mask has to come FIRST: a site that is already rewritten does not show up there.
static const char kTargetMaskRelaxed[] = "xxxxxx?????xxx?xxxx?xx?xxxxx????xxxx"
                                         "x????xxxxxxx????????????xx????xxx?"
                                         "xxxx????xx";

// The two masks describe the same number of pattern bytes: if either line stops holding, the
// file does not compile.
static_assert(sizeof(kTargetPattern) == sizeof(kTargetMask) - 1, "kTargetMask: 80 bytes");
static_assert(sizeof(kTargetPattern) == sizeof(kTargetMaskRelaxed) - 1,
              "kTargetMaskRelaxed: 80 bytes");
static_assert(sizeof(kTargetMaskRelaxed) == sizeof(kTargetMask), "the two masks: 81 bytes");

// ===========================================================================
// (A) THE MODULE IS PINNED IN MEMORY (documented by Microsoft).
//
// A COM DLL can be unloaded once DllCanUnloadNow answers S_OK (no object is in use any more)
// and the delay of CoFreeUnusedLibrariesEx has run out: ten minutes by default, treated as
// zero for apartment components. If Windows.UI.QuickActions.dll were unloaded and loaded
// again while this mod is in place, the new copy - mapped again from the file on disk -
// would not carry the bytes rewritten here.
//
// The documented answer is GetModuleHandleExW with GET_MODULE_HANDLE_EX_FLAG_PIN: "the
// module stays loaded until the process terminates, regardless of the number of calls to
// FreeLibrary", and the reference count is not incremented (nothing to FreeLibrary: this mod
// never calls it, not even on disable). GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS says the
// first parameter is an address inside the module: the base of the module is passed, which is
// an address inside it.
//
// The pin is NOT reversible and only lives in the process where it was made, that is
// ShellExperienceHost.exe (the process that draws the flyouts): while that process lives the
// module stays loaded. What this mod does give back on unload is the patched bytes
// (FlyoutHostPatch::Uninstall); the pin cannot be taken back, there is no documented way to
// do it. Nothing else is touched: no other process, nothing on disk. If the call fails the
// mod logs it and goes on: the patch holds while the module stays loaded, and the
// recognition by content (B) covers the case of a new copy.
// ===========================================================================
static void PinModuleOrLog(HMODULE module, const wchar_t* tag, const wchar_t* name,
                           std::atomic<bool>& alreadyPinned) noexcept {
    try {
        if (!module || alreadyPinned.load(std::memory_order_relaxed)) return;
        HMODULE pinned = nullptr;
        if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN |
                                    GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                                reinterpret_cast<LPCWSTR>(module), &pinned) ||
            !pinned) {
            Wh_Log(L"%s %s (0x%p) could not be pinned (error %lu): if the shell unloads it, a new "
                   L"copy would not carry what was done here", tag, name, (void*)module,
                   GetLastError());
            return;
        }
        alreadyPinned.store(true, std::memory_order_relaxed);
        Wh_Log(L"%s %s (0x%p) is pinned: it stays loaded until ShellExperienceHost.exe ends (the "
               L"pin is not reversible and is not undone when the mod is disabled)", tag, name,
               (void*)module);
    } catch (...) {
        Wh_Log(L"%s exception while pinning a module", tag);
    }
}

// ===========================================================================
// (B) THE SIGNATURE OF THE WRITES, READ FROM THE MODULE (an empirical observation: it is not
// a documented property of the module). The five bytes 6..10 are NOP (the "call
// LoadComponent" is gone) and the eight bytes 52..59 are the ones copied from the older
// loader (source + 4): nothing else writes them. The rel32 at 74..77 is not checked, because
// without the getter it cannot be recomputed, and the two writes above are enough.
// ===========================================================================
static bool SiteLooksPatched(const unsigned char* target, const unsigned char* source) noexcept {
    if (!target || !source) return false;
    for (int i = 0; i < 5; ++i)
        if (target[6 + i] != 0x90) return false;
    return memcmp(target + 52, source + 4, 8) == 0;
}

static void PatchQuickActionsTemplates(HMODULE module) noexcept {
    try {
        if (!module) return;

        // (C): the early "if g_patched, return" is gone: that flag says "I did it", not "the
        // module in memory is rewritten", and after a reload of the mod the flags are new
        // while the bytes stay written. The module is what is looked at (first the intact
        // form, then the written one), never the base address.
        size_t size = 0;
        unsigned char* text = TextSectionOf(module, &size);
        if (!text) {
            Wh_Log(L"[flyout-host] the code of Windows.UI.QuickActions.dll was not found: "
                   L"nothing is patched");
            return;
        }

        // The older copy of the template loader is never rewritten by either side: if it is
        // not there, this is not a build this code knows.
        unsigned char* source = FindPattern(text, size, kSourcePattern, kSourceMask);
        unsigned char* getter = nullptr;
        if (source) {
            getter = source + 25;                                   // the call to the getter
            getter += 5 + *reinterpret_cast<int*>(getter + 1);       // follow its rel32
        }
        if (!source || !getter) {
            Wh_Log(L"[flyout-host] Windows.UI.QuickActions.dll (0x%p): the loader of the Windows 10 "
                   L"template set was not found (source %s): nothing is patched", (void*)module,
                   source ? L"yes" : L"no");
            return;
        }

        // First the INTACT FORM. The full mask wants the 0xE8 at index 6 (the start of the
        // "call LoadComponent"): a site that is already rewritten does not show up in this
        // search, so the two paths cannot be mixed up.
        unsigned char* target = FindPattern(text, size, kTargetPattern, kTargetMask);
        if (!target) {
            // (B) - THEN THE FORM THAT ALREADY CARRIES THE PATCH: this is the reload of the
            // mod. Windhawk reloads the mod in processes that are already running (here
            // ShellExperienceHost.exe); a reload gives the bytes back and writes them again,
            // and the flags of this new copy start at zero. Without this path the state would
            // stay "not patched", the Windows 10 name of the button would never be asked for
            // and the flyout would be built half empty (inert buttons).
            unsigned char* already = FindPattern(text, size, kTargetPattern, kTargetMaskRelaxed);
            if (already && SiteLooksPatched(already, source)) {
                g_patched.store(true, std::memory_order_relaxed);
                Wh_Log(L"[flyout-host] Windows.UI.QuickActions.dll (0x%p) already carries the "
                       L"Windows 10 template set (it survived a reload of the mod, or the shell "
                       L"loaded it again): nothing is written, the state is taken as it is",
                       (void*)module);
                PinModuleOrLog(module, L"[flyout-host]", L"Windows.UI.QuickActions.dll", g_pinned);
                return;
            }
            if (already) {
                // The site is there but its bytes are neither the intact ones nor the ones of
                // this patch: what that copy is stays unknown, so nothing is written and
                // nothing is pinned.
                Wh_Log(L"[flyout-host] Windows.UI.QuickActions.dll (0x%p): the site is there but "
                       L"its bytes are none of the two forms this code knows: nothing is written",
                       (void*)module);
                return;
            }
            Wh_Log(L"[flyout-host] the Windows 10 template set of Windows.UI.QuickActions.dll "
                   L"(0x%p) was not found (source yes, target no): nothing is patched",
                   (void*)module);
            return;
        }

        // RAII: the page comes back executable and readable the way it was when this object
        // goes out of scope, and the write below no longer has a hand-written pair of
        // VirtualProtect calls somebody could forget to close (also when leaving early with a
        // return, or on a C++ exception caught higher up).
        ScopedWriteProtect writable(target, 80);
        if (!writable) {
            Wh_Log(L"[flyout-host] those bytes could not be changed (error %lu)", GetLastError());
            return;
        }
        memcpy(g_savedBytes, target, sizeof(g_savedBytes));   // what Wh_ModBeforeUninit gives back
        g_writtenAt = target;
        for (int i = 0; i < 5; ++i) target[6 + i] = 0x90;            // call LoadComponent
        memcpy(target + 52, source + 4, 8);                          // mov edx / lea rcx
        *reinterpret_cast<int*>(target + 74) =
            static_cast<int>(reinterpret_cast<long long>(getter) -
                             reinterpret_cast<long long>(target + 78));
        writable.Restore();   // at once: the write is over here
        FlushInstructionCache(GetCurrentProcess(), target, 80);
        g_patched.store(true, std::memory_order_relaxed);
        g_canRestore.store(true, std::memory_order_release);
        Wh_Log(L"[flyout-host] this process builds the flyouts with the Windows 10 template set "
               L"(written now, module 0x%p): the flyout is drawn and stays on screen, network and "
               L"battery", (void*)module);
        // (A): the copy that has just been rewritten will not be unloaded while this process lives.
        PinModuleOrLog(module, L"[flyout-host]", L"Windows.UI.QuickActions.dll", g_pinned);
    } catch (...) {
        Wh_Log(L"[flyout-host] exception while changing the template set");
    }
}

// ---- the module shows up: it is taken as soon as it appears -------------------
// the name of the module, without the path and ignoring the case: the shell loads these
// modules with the full path of the package.
static bool NameIsModuleNamed(const wchar_t* text, size_t length,
                              const wchar_t* name) noexcept {
    if (!text || !name || !length) return false;
    size_t start = 0;
    for (size_t i = 0; i < length; ++i)
        if (text[i] == L'\\' || text[i] == L'/') start = i + 1;
    const size_t kLength = wcslen(name);
    if (length - start != kLength) return false;
    for (size_t i = 0; i < kLength; ++i) {
        wchar_t a = text[start + i];
        wchar_t b = name[i];
        if (a >= L'A' && a <= L'Z') a += 32;
        if (b >= L'A' && b <= L'Z') b += 32;
        if (a != b) return false;
    }
    return true;
}

// The 80 bytes this instance wrote, back as they were: the flyout of Windows 11 is built the
// Windows 11 way again as soon as the mod is off. Restoring is the same kind of operation as
// patching (the bytes of a running function are replaced and the instruction cache flushed),
// so it carries no risk the patch did not carry. What cannot be taken back is the pin of the
// module (see (A) above): the module stays loaded until the process ends, and the next
// load of the mod re-patches whatever it finds.
static void Uninstall() noexcept {
    try {
        if (!g_canRestore.exchange(false, std::memory_order_acq_rel)) return;
        unsigned char* target = g_writtenAt;
        g_writtenAt = nullptr;
        if (!target) return;
        ScopedWriteProtect writable(target, sizeof(g_savedBytes));
        if (!writable) {
            Wh_Log(L"[flyout-host] the original bytes could not be written back (error %lu)",
                   GetLastError());
            return;
        }
        memcpy(target, g_savedBytes, sizeof(g_savedBytes));
        FlushInstructionCache(GetCurrentProcess(), target, sizeof(g_savedBytes));
        g_patched.store(false, std::memory_order_relaxed);
        Wh_Log(L"[flyout-host] the bytes of Windows.UI.QuickActions.dll were given back: this "
               L"process builds the flyouts its own way again");
    } catch (...) {
        Wh_Log(L"[flyout-host] exception while giving the patched bytes back");
    }
}

static bool NameIsQuickActions(const wchar_t* text, size_t length) noexcept {
    return NameIsModuleNamed(text, length, L"Windows.UI.QuickActions.dll");
}

static void OnModuleLoaded(HMODULE module, const wchar_t* text, size_t length) noexcept {
    try {
        if (!module) return;
        if (NameIsQuickActions(text, length)) {
            if (g_logs.fetch_add(1) < 8)
                Wh_Log(L"[flyout-host] Windows.UI.QuickActions.dll is here (0x%p): the template "
                       L"set is changed before the shell builds anything with it",
                       (void*)module);
            PatchQuickActionsTemplates(module);
            return;
        }
    } catch (...) {
    }
}

// The same two lines the mod already uses for the redirect of the tray modules: the type is
// the one of <winternl.h>, included at the top of the file.
typedef LONG(NTAPI* LdrLoadDll_t)(PWSTR, PULONG, UNICODE_STRING*, HMODULE*);
static LdrLoadDll_t LdrLoadDll_Original = nullptr;

static LONG NTAPI LdrLoadDll_Hook(PWSTR searchPath, PULONG flags, UNICODE_STRING* name, HMODULE* handle) {
    const LONG status = LdrLoadDll_Original(searchPath, flags, name, handle);
    try {
        if (status >= 0 && name && name->Buffer && handle && *handle)
            OnModuleLoaded(*handle, name->Buffer, name->Length / sizeof(wchar_t));
    } catch (...) {
    }
    return status;
}

// Called from Wh_ModInit, inside the ShellExperienceHost.exe branch: hooks are registered
// while Wh_ModInit runs (Windhawk documentation), and the template module comes later.
static void Install() noexcept {
    try {
        // A flyout may have been built before the mod started: then the module is already here.
        HMODULE loaded = GetModuleHandleW(L"Windows.UI.QuickActions.dll");
        if (loaded) PatchQuickActionsTemplates(loaded);

        HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
        void* target = ntdll ? reinterpret_cast<void*>(GetProcAddress(ntdll, "LdrLoadDll"))
                             : nullptr;
        if (target && Wh_SetFunctionHook(target, reinterpret_cast<void*>(LdrLoadDll_Hook),
                                         reinterpret_cast<void**>(&LdrLoadDll_Original)))
            Wh_Log(L"[flyout-host] the flyout module is watched (LdrLoadDll)");
    } catch (...) {
        Wh_Log(L"[flyout-host] exception while arming the flyout module watch");
    }
}

}  // namespace FlyoutHostPatch
// ===========================================================================
// NetworkUxHostPatch (the swap of the button name inside NetworkUX.dll) and NetworkUxSkin
// (the graphical skin of the resource dictionary) were removed from this mod: see the note
// in the publishing pull request. The swap of the template set of
// Windows.UI.QuickActions.dll above (FlyoutHostPatch) stays, because without it the flyout
// is not built at all on these builds.
// ===========================================================================
BOOL Wh_ModInit() {
    try {
        LoadFlyoutSettings();

        wchar_t exePath[MAX_PATH] = {};
        GetModuleFileNameW(nullptr, exePath, _countof(exePath));
        if (g_realExePath[0] == 0) wcscpy_s(g_realExePath, exePath);
        Wh_Log(L"[flyout] init: Windows 10 legacy flyouts restorer");
        Wh_Log(L"[flyout] process: %s", exePath);
        Wh_Log(L"[flyout] data folder: %s",
               g_cfg.storePath[0] ? g_cfg.storePath : L"(not available)");

        // ShellExperienceHost.exe hosts the flyouts this mod draws (network, battery). This
        // mod is loaded there as well, and in that process only the flyout-host patches (and
        // optionally the experimental square-corners hook) run: it is not the shell process,
        // and nothing else of the mod applies to it.
        // Is the running process this image? The flyout host belongs to another process, so
        // the name of the image is what tells the two apart.
        if (IsProcessImageName(g_realExePath, L"ShellExperienceHost.exe")) {
            Wh_Log(L"[flyout] ShellExperienceHost: only the flyout-host patches run here");
            // This process is the one that draws the flyout: it has to build it the Windows 10
            // way, otherwise the flyout it shows is torn down again after a moment.
            FlyoutHostPatch::Install();
            return TRUE;
        }

        if (!IsLegacyShellProcess()) {
            Wh_Log(L"[flyout] this is not the private Windows 10 shell: nothing to do here");
            return TRUE;
        }

        // Peek values are virtualized only in the private shell process. If either path
        // identification or the documented Win32 query hook is unavailable, the menu item is
        // disabled; this feature never falls back to writing the user's registry.
        InstallPeekRegistryVirtualization();

        // The battery icon opens the Windows 10 battery flyout. The hook
        // is installed here, inside Wh_ModInit, so that the engine applies it at once (see
        // the note below), and it is not tied to a setting: with a settings list saved by an
        // older version a new setting reads as 0 and the battery click did nothing.
        NetworkTrayForce::BatteryFlyout::Install();

        // THE REGISTRATION POINT OF EVERY HOOK OF THIS MOD.
        //
        // Windhawk wiki, "Creating a new mod": Wh_SetFunctionHook "can't be called after
        // Wh_ModBeforeUninit returns"; Wh_ApplyHookOperations "is called automatically by
        // Windhawk after Wh_ModInit" and, in its own words, "ideally, all hooks should be
        // set in Wh_ModInit and this function should never be used". The "Mod lifetime" page
        // shows the same order: Wh_ModInit, the implicit apply, and only afterwards are the
        // hooks removed, between Wh_ModBeforeUninit and Wh_ModUninit.
        //
        // An earlier revision registered the tray hooks from the services thread instead, that is after
        // Wh_ModInit had returned: the engine had already applied the queue, so they stayed
        // queued and inert, and the mod then called Wh_ApplyHookOperations in a loop from
        // that thread while the engine was loading or unloading hooks of its own. The hook
        // queue of the engine (MinHook) is not meant to be operated by two threads at once,
        // which is what took explorer.exe down on every enable and on every disable.
        // Nothing outside Wh_ModInit touches the hook queue any more.
        InstallTraySupportHooks();

        // The click on the network icon: ShellExecuteW and ShellExecuteExW.
        InstallNetworkClickHooks();

        g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!g_stopEvent) {
            Wh_Log(L"[flyout] the stop event could not be created");
            return TRUE;
        }
        g_servicesThread = CreateThread(nullptr, 0, FlyoutServicesThread, nullptr, 0, nullptr);
        if (!g_servicesThread)
            Wh_Log(L"[flyout] the services thread could not be created (%lu)", GetLastError());
        return TRUE;
    } catch (...) {
        Wh_Log(L"[flyout] init exception: the shell is left as it is");
        return TRUE;
    }
}

// Windhawk calls this only after applying the hook operations queued by Wh_ModInit.
// Marking the virtual setting ready here avoids a brief race when the mod is loaded into
// an Explorer process that is already running.
void Wh_ModAfterInit() {
    if (g_peekRegistryHookRegistered.load(std::memory_order_acquire)) {
        g_peekRegistryHookReady.store(true, std::memory_order_release);
        Wh_Log(L"[menu] Peek process-local registry virtualization is active; the real "
               L"registry values remain untouched");
    }
}

// The unload, in the order the Windhawk documentation describes it.
//
// "Mod lifetime": Wh_ModBeforeUninit, then the engine removes the hooks, then Wh_ModUninit;
// the API pages add that Wh_SetFunctionHook, Wh_RemoveFunctionHook and Wh_ApplyHookOperations
// can no longer be used once Wh_ModBeforeUninit has returned. So the threads of this mod are
// stopped and joined in Wh_ModBeforeUninit - there is no other place left where it can be
// done - and Wh_ModUninit only finishes what is left, without blocking: an early revision waited
// INFINITE there, that is after the hooks were already gone and the module was about to be
// unloaded, and it closed the handles of a thread it had not seen exit.
// This used to give up after 10 s and let the unload continue with the thread still
// running - Windhawk unmaps the module when Wh_ModUninit returns, and a thread inside that
// image then executes code that is gone (explorer.exe with it). Ten seconds is not an
// unlikely wait either: a slow download can hold the tick for a minute. The long operations
// of the thread are now cancellable (the download of the Windows 10 files is closed by
// CancelActiveDownload, every loop looks at g_unloading), so the join is unconditional.
static bool JoinServicesThread(PCWSTR where) {
    if (!g_servicesThread) return true;
    ULONGLONG nextNote = GetTickCount64() + 30000;
    for (;;) {
        if (WaitForSingleObject(g_servicesThread, 1000) == WAIT_OBJECT_0) return true;
        const ULONGLONG now = GetTickCount64();
        if (now >= nextNote) {
            nextNote = now + 30000;
            Wh_Log(L"[flyout] the services thread is still finishing its work (%s): the unload "
                   L"waits for it, the module must not go away under a running thread", where);
        }
    }
}

void Wh_ModBeforeUninit() {
    // Called by Windhawk "when the mod is about to be unloaded, before the Windhawk engine
    // removes hooks": from the return of this callback on, no hook operation is allowed any
    // more, so the mod has to be quiet here already.
    g_unloading.store(true, std::memory_order_seq_cst);
    // First the operations that can block: the join below has no timeout, so whatever the
    // services thread is waiting on has to be released here.
    CancelActiveDownload();
    ShellOpGuard::BeginShutdown();
    ClearPeekOverridesOnUnload();
    // The bytes written in ShellExperienceHost.exe by this copy of the mod are its own
    // doing, and are given back before the image goes away. The module pin cannot be
    // undone (no documented way exists); that is noted in the README and in the log.
    FlyoutHostPatch::Uninstall();
    if (g_stopEvent) SetEvent(g_stopEvent);
    JoinServicesThread(L"Wh_ModBeforeUninit");
}

void Wh_ModUninit() {
    // Called "when the mod is about to be unloaded, after the Windhawk engine removes hooks",
    // and the module is unloaded when this returns: no hook function may be used here, and a
    // callback that blocks forever would hold the unload. The thread was stopped above, so
    // this is a bounded check that normally finds it already gone.
    const bool threadStopped = JoinServicesThread(L"Wh_ModUninit");
    // The import table entry of pnidui is given back here, after every thread of this
    // mod has really stopped - the owner window used to be able to take it over again from
    // inside that window procedure, which would have left it pointing into the unloaded image.
    // Wh_ModUninit still runs with the module mapped, so the hook it names is alive.
    RestorePniduiShellExecuteExIatOnUnload();
    if (threadStopped) {
        if (g_servicesThread) {
            CloseHandle(g_servicesThread);
            g_servicesThread = nullptr;
        }
        // The stop event belongs to the thread: it is closed only once the thread is gone.
        if (g_stopEvent) {
            CloseHandle(g_stopEvent);
            g_stopEvent = nullptr;
        }
    }
    // The timed-out ShellExecute workers of this mod are joined here (bounded).
    ShellOpGuard::Shutdown();
    g_trayThreadId = 0;
    Wh_Log(L"[flyout] unloaded");
}

void Wh_ModSettingsChanged() {
    LoadFlyoutSettings();
    // The user may have just switched the tray modules back on, or changed the timeout:
    // A store that is still incomplete is tried again at once.
    ResetTrayStoreBackoff();
    NetworkTrayForce::SettingsChanged();
    Wh_Log(L"[flyout] settings reloaded: tray modules=%s",
           g_cfg.provideTrayDlls ? L"on" : L"off");
}
