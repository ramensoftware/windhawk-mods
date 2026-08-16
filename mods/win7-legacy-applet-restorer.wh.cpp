// ==WindhawkMod==
// @id              win7-legacy-applet-restorer
// @name            Windows 7 Legacy Applet Restorer
// @description     This mod restores some classic Control Panel applets and localized Windows 7 task links using native components
// @version         2.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @include         control.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lshlwapi -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
## About
This mod restores classic Control Panel applets and classic task links in Category View, including:

* Personalization, with localized classic Windows 7 task links
* Notification area icons (intended for the Windows 10 taskbar)
* Network Connections
* Printers and Faxes
* HomeGroup (legacy, partially functional)
* BitLocker Drive Encryption
* Tablet PC Settings

Additionally, the mod can suppress legacy Control Panel items that are broken or no longer functional on Windows 10/11 such as "Company Settings Sync", Windows To Go, Infrared and Work Folders when the corresponding settings are enabled.
The optional "Restore Classic Task Links" setting restores localized, classic task links for these sections in Category View.
## Screenshot of the Restored Applets

![Restored Voices](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/restoredvoices.png)

## Screenshot (for the HomeGroup and Network Connections applets with the corresponding task links)

![screenshot](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/legacyappet.png)

## Notes
The mod has been tested on Windows 10 1809, Windows 10 21H2 and Windows 11 24H2.

BitLocker Drive Encryption and Tablet PC Settings default to **Automatic**: they are only added when the applet exists on the machine *and* Control Panel does not already show it, so no duplicate entries appear on editions and devices where Windows lists them by itself (e.g. Pro/Enterprise with a TPM, or a pen/touch-capable device). Whether the applet is already shown is asked of the shell itself (`IOpenControlPanel::GetPath`), because the `ControlPanel\NameSpace` registry key alone is not reliable — on Windows 10 LTSC 2021 it is present even though the applet is not displayed.

If the automatic detection is wrong on your edition, each of the two applets has an **Always add** / **Never add** override in the settings. "Always add" still does nothing when the applet is genuinely not installed (e.g. Windows Home), since the entry would have no name, icon or target.

**⚠️ Do not enable this mod together with "Restore the classic Personalization and other CPLs" (restore-classic-cpls) by Anixx.** Both mods inject the same CLSIDs into the Control Panel, potentially conflicting with each other.

The mod does not commit to restoring task links that open the Settings app instead of the classic Control Panel UI, because restoring such links would defeat the mod's purpose (contributing to the Control Panel restoration).

## Credits
This mod is based on a fork of the original mod by Anixx (https://github.com/Anixx) and parts of the implementation are taken from aubymori (https://github.com/aubymori)'s Control Panel script.
Credits to m417z for the code review and enhancing the mod.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enablePersonalization: true
  $name: Personalization
  $description: This setting adds the "Personalization" icon to the Control Panel
- enableNotificationIcons: true
  $name: Notification area icons
  $description: This setting adds the "Notification area icons" icon to the Control Panel (intended for Windows 10 taskbar)
- enableNetworkConnections: true
  $name: Network connections
  $description: This setting adds the "Network connections" icon to the Control Panel
- enablePrintersAndFaxes: true
  $name: Printers and Faxes
  $description: This setting adds the "Printers and Faxes" icon to the Control Panel
- enableHomeGroup: true
  $name: HomeGroup
  $description: This setting restores navigation to the HomeGroup page only when Windows still registers its legacy CLSID. For this mod, successful page availability satisfies the feature goal and preserves compatibility with present or future external HomeGroup-restoration projects; networking functionality is not implied.
- bitLockerMode: auto
  $name: BitLocker Drive Encryption
  $description: Adds the "BitLocker Drive Encryption" icon to the Control Panel (System and Security category). "Automatic" adds it only when the applet exists on this machine and Control Panel does not already show it, so no duplicate entry appears. If the detection gets it wrong on your edition, force it with "Always add" or "Never add".
  $options:
  - auto: Automatic (add it only if Control Panel doesn't already show it)
  - always: Always add
  - never: Never add
- tabletPcMode: auto
  $name: Tablet PC Settings
  $description: Adds the "Tablet PC Settings" icon to the Control Panel (Hardware and Sound category). "Automatic" adds it only when the applet exists on this machine and Control Panel does not already show it, so no duplicate entry appears. If the detection gets it wrong on your device, force it with "Always add" or "Never add".
  $options:
  - auto: Automatic (add it only if Control Panel doesn't already show it)
  - always: Always add
  - never: Never add
- enableCategoryAppearanceLinks: true
  $name: Restore Category Appearance Links
  $description: This setting restores the classic "Change the theme", "Change desktop background", and "Adjust screen resolution" links directly under the Appearance and Personalization category on the main Control Panel home page.
- suppressCompanySync: true
  $name: Suppress the "Company Settings Sync" broken icon
  $description: This setting removes the {98F2AB62-0E29-4E4C-8EE7-B542E66740B1} non-functional icon
- suppressWindowsToGo: false
  $name: Suppress Windows To Go
  $description: This setting hides Windows To Go when that legacy Control Panel item is registered on this Windows installation
- suppressInfrared: false
  $name: Suppress Infrared
  $description: This setting hides the legacy Infrared Control Panel item when it is registered on this Windows installation
- suppressWorkFolders: false
  $name: Suppress Work Folders
  $description: This setting hides the legacy Work Folders Control Panel item when it is registered on this Windows installation
- restoreClassicTaskLinks: true
  $name: Restore Classic Task Links
  $description: This setting restores the localized, classic task links for Personalization and other sections in category view
- restoreWin7CategoryTaskLinks: true
  $name: Restore Windows 7 Category Task Links
  $description: This setting restores the classic task links under all Control Panel categories (System and Security, Programs, User Accounts, Clock/Language/Region, Ease of Access) like Windows 7 had
*/
// ==/WindhawkModSettings==

// If a reviewer or automated program is reading the comments, feel free to point out any missing link that could be restored
/* Author's note
This mod enhances Anixx's "Restore the classic Personalization and other CPLs" 
(restore-classic-cpls.wh.cpp) by adding HomeGroup, classic task links, Windows 7 
category task links, and a shell::: command redirect hook.

Based on a fork of Anixx's work, its primary goal is to restore Windows 7-style 
links on Windows 10/11. Currently tested only on Windows 10 1809 (hardware limited 
to 4 GB RAM, HDD; unable to run Windows 11).

New features include the reintroduction of HomeGroup in Control Panel — absent 
from the original mod. To prevent conflicts, this mod will auto-disable if 
Anixx's mod is detected and log a warning.

The original author (Anixx) was contacted about merging these changes but did 
not reply. As a precaution, automatic detection ensures only one mod runs at a time.

The mod has been tested on Windows 10 1809 x64 (build 17763) The hook drives
shell32's own applet ranking, through its parameters where shell32 exposes
that ranking as its own function, and through the applet list where shell32
inlines it. The second path needs two struct offsets, which it recovers by
disassembling the comparator rather than assuming them, so neither path
carries a value that has to be revisited per build. If the offsets cannot be
read, or the comparator symbol cannot be resolved at all, the rest of the mod
runs with stock applet ordering.
*/
#include <string>
#include <string_view>
#include <algorithm>
#include <regex>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <atomic>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <memory>
#include <new>
#include <shellapi.h>
#include <shobjidl.h>   // IOpenControlPanel, CLSID_OpenControlPanel
#include <shlwapi.h>
#include <commctrl.h>
#include <windhawk_utils.h>

struct Settings {
    std::atomic<bool> enablePersonalization;
    std::atomic<bool> enableNotificationIcons;
    std::atomic<bool> enableNetworkConnections;
    std::atomic<bool> enablePrintersAndFaxes;
    std::atomic<bool> enableHomeGroup;
    // Tri-state (AppletMode): the user can override the automatic detection in
    // both directions, because "does Control Panel already show this applet?"
    // cannot be answered with total confidence on every edition.
    std::atomic<int> bitLockerMode;
    std::atomic<int> tabletPcMode;
    std::atomic<bool> enableCategoryAppearanceLinks;
    std::atomic<bool> suppressCompanySync;
    std::atomic<bool> suppressWindowsToGo;
    std::atomic<bool> suppressInfrared;
    std::atomic<bool> suppressWorkFolders;
    std::atomic<bool> restoreClassicTaskLinks;
    std::atomic<bool> restoreWin7CategoryTaskLinks;
} g_settings;

// Wh_Log is already a cheap no-op check (if (g_logsOn)) when logging is off,
// so there is nothing to gain by compiling diagnostics out entirely — doing so
// only prevented getting a log out of this mod when a user reports a problem.

// Windows version info
typedef LONG (WINAPI *RtlGetVersion_t)(PRTL_OSVERSIONINFOW);
static DWORD g_winBuild = 0;

void DetectWindowsVersion() {
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (hNtdll) {
        auto pRtlGetVersion = (RtlGetVersion_t)GetProcAddress(hNtdll, "RtlGetVersion");
        if (pRtlGetVersion) {
            RTL_OSVERSIONINFOW osvi = { sizeof(osvi) };
            if (pRtlGetVersion(&osvi) == 0) {
                g_winBuild = osvi.dwBuildNumber;
            }
        }
    }
}

std::wstring g_personalizationName;
// HomeGroup is only injected when Windows itself still registers its legacy CLSID.
// The entry is intentionally retained for users of external HomeGroup-restoration
// projects. The mod's narrowly defined goal is satisfied when the legacy page can
// be opened: this preserves a navigation target for present or future restoration
// projects without claiming that the removed networking service itself works. If
// Windows no longer exposes the CLSID, no virtual replacement is made.
static std::atomic<bool> g_homeGroupClsidAvailable{ false };
// BitLocker and Tablet PC Settings are real, unmodified Windows CLSIDs; they
// are simply not always *registered* (BitLocker needs Pro/Enterprise+TPM,
// Tablet PC Settings needs a touch/pen-capable device). Unlike HomeGroup,
// these two are injected through a *virtual* CLSID that mirrors the real
// applet, so "the CLSID is registered" is not a sufficient condition: on the
// machines where Windows lists the applet itself (BitLocker under System and
// Security on Pro/Enterprise, Tablet PC Settings under Hardware and Sound on
// pen/touch devices), a virtual twin would show up as a second,
// identical-looking entry in the same category.
//
// Detecting "Control Panel already shows this" purely from the registry turned
// out to be wrong in practice: on Windows 10 LTSC 2021 (Enterprise) the
// ControlPanel\\NameSpace registration for BitLocker is present while the
// applet is not displayed anywhere, so a registry-only check silently dropped
// an applet the user did want. The authoritative answer comes from the shell
// itself (IOpenControlPanel::GetPath), with the registry only as a fallback -
// and the user can override the verdict in either direction, see AppletMode.
enum class AppletMode { Auto = 0, Always = 1, Never = 2 };

// Result of the automatic detection, computed once in Wh_ModInit: "the applet
// is launchable here AND Control Panel doesn't already show it".
static std::atomic<bool> g_bitlockerAutoDetected{ false };
static std::atomic<bool> g_tabletPcAutoDetected{ false };
// Whether the real applet exists at all on this machine. Even "Always add"
// cannot conjure an applet that isn't installed - the entry would open nothing
// and its name/icon couldn't be copied - so this gates the override too.
static std::atomic<bool> g_bitlockerClsidRegistered{ false };
static std::atomic<bool> g_tabletPcClsidRegistered{ false };
// Effective verdict (auto detection combined with the user's override). This
// is what every injection site gates on, and it is recomputed whenever the
// settings change.
static std::atomic<bool> g_injectBitlockerApplet{ false };
static std::atomic<bool> g_injectTabletPcApplet{ false };

// Forward declaration
bool EnsureClassicTaskLinksFile();
std::wstring g_classicTaskLinksFilePath;

// Forward declarations (defined further below; KeyTracker::Track needs them)
std::wstring ToLower(const std::wstring& str);
bool ContainsRelevantKeywordInsensitive(const std::wstring& path);

// Tracks the "virtual path" behind every HKEY the mod cares about (both real
// keys opened through the hooked Reg* APIs, and fully synthetic/fake keys we
// hand back for injected CLSIDs). A single lock guards all state so the
// "is this fake?" check and the "what's its path?" lookup can never observe
// two different snapshots of the data. Fake-handle memory is owned via
// unique_ptr, so it is always freed exactly once, from exactly one place —
// no manual new/delete pairing to get wrong.
//
// The lock is a shared_mutex, not a plain mutex: these hooks sit on every
// registry call made by explorer.exe from many threads at once, and lookups
// (GetPath/IsFake/IsFakeAndGetPath) outnumber mutations (Track/Untrack/
// CreateFake/FreeFake) by orders of magnitude. An exclusive mutex here turned
// the whole process's registry traffic into a single serialization point.
class KeyTracker {
public:
    std::wstring GetPath(HKEY hKey) const {
        if (std::wstring special = SpecialRootPath(hKey); !special.empty()) return special;
        std::shared_lock<std::shared_mutex> lock(mutex_);
        auto it = paths_.find(hKey);
        return it != paths_.end() ? it->second : std::wstring();
    }

    bool IsFake(HKEY hKey) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return fakeOwners_.count(hKey) != 0;
    }

    // Combines IsFake + GetPath under a single lock acquisition, for callers
    // (like RegOpenKeyExWHook) that would otherwise take the mutex twice in a
    // row for the same key on every call.
    bool IsFakeAndGetPath(HKEY hKey, std::wstring& outPath) const {
        if (std::wstring special = SpecialRootPath(hKey); !special.empty()) {
            outPath = special;
            return false;
        }
        std::shared_lock<std::shared_mutex> lock(mutex_);
        bool isFake = fakeOwners_.count(hKey) != 0;
        auto it = paths_.find(hKey);
        outPath = it != paths_.end() ? it->second : std::wstring();
        return isFake;
    }

    void Track(HKEY hKey, const std::wstring& path) {
        if (!hKey || IsSpecialRoot(hKey)) return;
        if (!ContainsRelevantKeywordInsensitive(path)) return;
        std::unique_lock<std::shared_mutex> lock(mutex_);
        paths_[hKey] = path;
    }

    void Untrack(HKEY hKey) {
        if (!hKey || IsSpecialRoot(hKey)) return;
        std::unique_lock<std::shared_mutex> lock(mutex_);
        paths_.erase(hKey);
    }

    // Allocates a small owned object to serve as an opaque, unique HKEY value.
    // Returns nullptr on allocation failure instead of throwing, so callers
    // running inside a hook (on an arbitrary explorer.exe thread) never have
    // to deal with a C++ exception unwinding through foreign code.
    HKEY CreateFake(const std::wstring& path) {
        std::unique_ptr<int> owned(new (std::nothrow) int(1));
        if (!owned) return nullptr;
        HKEY fake = reinterpret_cast<HKEY>(owned.get());
        std::unique_lock<std::shared_mutex> lock(mutex_);
        paths_[fake] = path;
        fakeOwners_[fake] = std::move(owned);
        return fake;
    }

    // Returns true if the handle was one of ours (and has now been released),
    // so callers don't need a separate IsFake() acquisition first.
    bool FreeFake(HKEY hKey) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        if (fakeOwners_.erase(hKey) == 0) return false;  // unique_ptr frees the memory
        paths_.erase(hKey);
        return true;
    }

    // Called once from Wh_ModUninit. Deliberately does NOT delete the
    // fake-handle memory: shell32/explorer may still be holding a stale HKEY
    // across a disable/re-enable cycle, and freeing it here could let a
    // future allocation reuse the same address, turning a stale handle into
    // a dangling alias for a live object. Leaking a handful of ints is
    // strictly safer than that use-after-free, so we only release ownership
    // (no delete) and drop our own bookkeeping.
    void ClearWithoutFreeing() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        paths_.clear();
        for (auto& kv : fakeOwners_) {
            int* intentionallyLeakedHandle = kv.second.release();
            (void)intentionallyLeakedHandle;
        }
        fakeOwners_.clear();
    }

private:
    static bool IsSpecialRoot(HKEY hKey) {
        auto v = (uintptr_t)hKey;
        return v >= 0x80000000 && v <= 0x80000004;
    }
    static std::wstring SpecialRootPath(HKEY hKey) {
        switch ((uintptr_t)hKey) {
            case 0x80000000: return L"HKEY_CLASSES_ROOT";
            case 0x80000001: return L"HKEY_CURRENT_USER";
            case 0x80000002: return L"HKEY_LOCAL_MACHINE";
            case 0x80000003: return L"HKEY_USERS";
            case 0x80000004: return L"HKEY_CURRENT_CONFIG";
            default: return L"";
        }
    }

    mutable std::shared_mutex mutex_;
    std::unordered_map<HKEY, std::wstring> paths_;
    std::unordered_map<HKEY, std::unique_ptr<int>> fakeOwners_;
};

static KeyTracker g_keyTracker;

// Pre-computed lowercase GUID strings for fast comparison
std::wstring g_personalizationGuidLower;
std::wstring g_notificationIconsGuidLower;
std::wstring g_networkConnectionsGuidLower;
std::wstring g_printersAndFaxesGuidLower;
std::wstring g_homeGroupGuidLower;
std::wstring g_displayGuidLower;
std::wstring g_realPersonalizationGuidLower;
std::wstring g_suppressedGuidLower;
std::wstring g_windowsToGoGuidLower;
std::wstring g_infraredGuidLower;
std::wstring g_workFoldersGuidLower;

// Pre-computed path suffixes (built once in InitDisplayNames instead of being
// concatenated from "clsid\\" / "controlpanel\\namespace\\" + guid on every
// ClassifyPath call, which runs on Explorer's registry hot path).
std::wstring g_personalizationClsidSuffix;
std::wstring g_personalizationDefaultIconSuffix;
std::wstring g_personalizationShellSuffix;
std::wstring g_personalizationShellOpenSuffix;
std::wstring g_personalizationOpenCommandSuffix;
std::wstring g_personalizationNsSuffix;

std::wstring g_realPersonalizationClsidSuffix;
std::wstring g_displayClsidSuffix;

std::wstring g_suppressedClsidSuffix,  g_suppressedNsSuffix;
std::wstring g_windowsToGoClsidSuffix, g_windowsToGoNsSuffix;
std::wstring g_infraredClsidSuffix,    g_infraredNsSuffix;
std::wstring g_workFoldersClsidSuffix, g_workFoldersNsSuffix;

std::wstring g_notificationIconsClsidSuffix;
std::wstring g_networkConnectionsClsidSuffix;
std::wstring g_printersAndFaxesClsidSuffix;
std::wstring g_homeGroupClsidSuffix;

static const std::wstring kPersonalizationGuid     = L"{580722ff-16a7-44c1-bf74-7e1acd00f4f9}";
static const std::wstring kNotificationIconsGuid   = L"{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}";
static const std::wstring kNetworkConnectionsGuid  = L"{7007acc7-3202-11d1-aad2-00805fc1270e}";
static const std::wstring kPrintersAndFaxesGuid    = L"{2227a280-3aea-1069-a2de-08002b30309d}";
static const std::wstring kHomeGroupGuid           = L"{67ca7650-96e6-4fdd-bb43-a8e774f73a57}";
static const std::wstring kDisplayGuid             = L"{c55584f4-7c7f-44f2-9a6d-913076f34c6a}"; // Also used as RealDisplayGuid
static const std::wstring kRealPersonalizationGuid = L"{ed834ed6-4b5a-4bfe-8f11-a626dcb6a921}";
static const std::wstring kSuppressedGuid          = L"{98f2ab62-0e29-4e4c-8ee7-b542e66740b1}";
static const std::wstring kWindowsToGoGuid          = L"{8e0c279d-0bd1-43c3-9ebd-31c3dc5b8a77}";
static const std::wstring kInfraredGuid             = L"{a0275511-0e86-4eca-97c2-ecd8f1221d08}";
static const std::wstring kWorkFoldersGuid          = L"{ecdb0924-4208-451e-8ee0-373c0956de16}";
static const std::wstring kBitLockerGuid            = L"{d9ef8727-cac2-4e60-809e-86f80a666c91}";
static const std::wstring kTabletPcSettingsGuid     = L"{80f3f1d5-feca-45f3-bc32-752c152e456e}";
// Documented canonical names for the two applets above; this is the form
// IOpenControlPanel::GetPath resolves most reliably (see IsShownByControlPanel).
static const std::wstring kBitLockerCanonicalName   = L"Microsoft.BitLockerDriveEncryption";
static const std::wstring kTabletPcCanonicalName    = L"Microsoft.TabletPCSettings";
// Own, made-up CLSIDs for the *virtual* Control Panel entries that mirror the
// two real applets above. We don't inject the real GUIDs directly (Explorer's
// Category View never asks about them at all on Win10/11 - confirmed by
// tracing: no open/query hits either way, unlike Network Connections/Printers/
// HomeGroup, whose real CLSIDs Explorer does probe for category data even
// though it doesn't list them by default). So instead we register brand-new
// CLSIDs, exactly like the Personalization entry above, whose name/icon are
// copied at runtime from the real applet and whose command re-launches the
// real applet via "explorer shell:::{realGuid}" - same command form already
// used for the HomeGroup task links.
static const std::wstring kBitLockerVirtualGuid     = L"{c62d8e9b-1f6a-4a6b-9a4c-8e6a7b2df301}";
static const std::wstring kTabletPcVirtualGuid      = L"{f3a91d47-6b52-4c9e-9d0a-1c7e5f2b6a84}";

static const DWORD kCategoryAppearance      = 1;
static const DWORD kCategoryHardware        = 2;
static const DWORD kCategoryNetwork         = 3;
static const DWORD kCategorySystemSecurity  = 5; // Matches the id used by the System-and-Security task group further below

std::wstring ToLower(const std::wstring& str) {
    std::wstring res = str;
    for (auto& c : res) c = towlower(c);
    return res;
}

bool EndsWith(const std::wstring& str, const std::wstring& suffix) {
    if (str.size() < suffix.size()) return false;
    return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// Minimal RAII wrapper around HKEY: closes the key on scope exit no matter
// how the scope is left (early return, thrown exception from a caller up the
// stack, etc.), so callers never need a manual RegCloseKey to remember.
class ScopedHKey {
public:
    ScopedHKey() = default;
    explicit ScopedHKey(HKEY key) : key_(key) {}
    ScopedHKey(const ScopedHKey&) = delete;
    ScopedHKey& operator=(const ScopedHKey&) = delete;
    ScopedHKey(ScopedHKey&& other) noexcept : key_(other.key_) { other.key_ = nullptr; }
    ScopedHKey& operator=(ScopedHKey&& other) noexcept {
        if (this != &other) { Close(); key_ = other.key_; other.key_ = nullptr; }
        return *this;
    }
    ~ScopedHKey() { Close(); }

    HKEY* AddressOf() { Close(); return &key_; }
    // Returns the currently held HKEY WITHOUT closing it. AddressOf() closes
    // the key first (it's meant for output parameters like RegOpenKeyExW), so
    // using it to read an already-open handle would silently close that key
    // and hand nullptr to the next registry call.
    HKEY Get() const { return key_; }
    explicit operator bool() const { return key_ != nullptr; }

private:
    void Close() { if (key_) { RegCloseKey(key_); key_ = nullptr; } }
    HKEY key_ = nullptr;
};

bool IsRegisteredClsid(const std::wstring& guid) {
    ScopedHKey key;
    const std::wstring path = L"CLSID\\" + guid;
    const LSTATUS status = RegOpenKeyExW(HKEY_CLASSES_ROOT, path.c_str(), 0, KEY_READ, key.AddressOf());
    return status == ERROR_SUCCESS && key;
}

// True when Windows itself already lists this CLSID as a Control Panel item.
// Control Panel enumerates CLSID-based items from
// ...\\Explorer\\ControlPanel\\NameSpace (HKLM for machine-wide items, HKCU for
// per-user ones), which is exactly the registration that makes BitLocker Drive
// Encryption or Tablet PC Settings appear on their supported configurations.
// If the entry is there, this mod must NOT inject a virtual twin for it, or the
// user ends up with two identical entries in the same category.
bool IsListedInControlPanelNameSpace(const std::wstring& guid) {
    static const wchar_t kNameSpacePrefix[] =
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ControlPanel\\NameSpace\\";
    const std::wstring subKey = std::wstring(kNameSpacePrefix) + guid;
    for (HKEY root : { HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER }) {
        ScopedHKey key;
        // This mod is x86-64 only, so there is no WOW64 view to worry about.
        if (RegOpenKeyExW(root, subKey.c_str(), 0, KEY_READ, key.AddressOf()) == ERROR_SUCCESS)
            return true;
    }
    return false;
}

// Asks the shell whether Control Panel actually displays this item, instead of
// inferring it from the registry. IOpenControlPanel::GetPath resolves a Control
// Panel item to its path and fails when the item is not part of the current
// item list - which is exactly the question this mod needs answered. This is
// the case the registry check got wrong on LTSC 2021, where the NameSpace key
// exists but the applet is not shown.
//
// pszName is documented as "the item's canonical name or its GUID", but the
// GUID form is the unreliable one: shell32 runs the string through
// COpenControlPanel::_MapLegacyName and a canonical-name lookup, and namespace
// items are addressed with the ::{GUID} moniker form rather than a bare
// {GUID}. So each candidate spelling is tried in turn - canonical name first,
// then ::{GUID}, then the bare GUID - and the first one the shell can parse
// wins. If none of them parse, the probe reports "no answer" instead of
// guessing, and the caller falls back to the registry hint.
//
// Returns true only when the shell gave a usable verdict, with outListed set.
// Only ever called from Wh_ModInit / the cached-probe path, before this mod's
// hooks are installed, so the shell's own registry reads can't re-enter them.
bool IsShownByControlPanel(const std::wstring& canonicalName, const std::wstring& guid,
                           bool& outListed) {
    // Defined locally rather than pulled from the SDK's CLSID_OpenControlPanel /
    // IID_IOpenControlPanel: those symbols live in uuid.lib, which Windhawk's
    // clang toolchain does not link by default, so referencing them fails at
    // link time with "undefined symbol: CLSID_OpenControlPanel". The values are
    // fixed, documented interface identifiers, so defining them here costs
    // nothing and avoids adding a library dependency just for two GUIDs.
    static const CLSID kClsidOpenControlPanel =
        { 0x06622d85, 0x6856, 0x4460, { 0x8d, 0xe1, 0xa8, 0x19, 0x21, 0xb4, 0x1c, 0x4b } };
    static const IID kIidOpenControlPanel =
        { 0xd11ad862, 0x66de, 0x4df4, { 0xbf, 0x6c, 0x1f, 0x56, 0x21, 0x99, 0x6a, 0xf1 } };

    // The shell's Control Panel object is apartment-threaded; initialize an STA
    // for the duration of the probe and undo it only if we created it.
    const HRESULT initHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (initHr == RPC_E_CHANGED_MODE) {
        Wh_Log(L"  COM already initialized in a different mode; skipping shell probe");
        return false;
    }
    const bool weInitialized = SUCCEEDED(initHr);

    bool answered = false;
    IOpenControlPanel* openControlPanel = nullptr;
    HRESULT hr = CoCreateInstance(kClsidOpenControlPanel, nullptr, CLSCTX_INPROC_SERVER,
                                  kIidOpenControlPanel, (void**)&openControlPanel);
    if (SUCCEEDED(hr) && openControlPanel) {
        const std::wstring monikerForm = L"::" + guid;
        const std::wstring* candidates[] = { &canonicalName, &monikerForm, &guid };

        for (const std::wstring* candidate : candidates) {
            if (candidate->empty()) continue;

            wchar_t path[MAX_PATH] = {};
            hr = openControlPanel->GetPath(candidate->c_str(), path, ARRAYSIZE(path));
            if (SUCCEEDED(hr)) {
                outListed = true;
                answered = true;
                Wh_Log(L"  GetPath(\"%s\") -> \"%s\" (item IS shown)", candidate->c_str(), path);
                break;
            }
            if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
                // The shell understood the name and says the item isn't there.
                outListed = false;
                answered = true;
                Wh_Log(L"  GetPath(\"%s\") -> ERROR_FILE_NOT_FOUND (item is NOT shown)", candidate->c_str());
                break;
            }
            // E_INVALIDARG and friends mean "shell couldn't parse this name",
            // NOT "the item is absent" - never treat it as a verdict. Try the
            // next spelling instead.
            Wh_Log(L"  GetPath(\"%s\") not understood, hr=0x%08lX; trying next form",
                candidate->c_str(), (unsigned long)hr);
        }

        if (!answered)
            Wh_Log(L"  No spelling of the item name was understood by the shell; no verdict");

        openControlPanel->Release();
    } else {
        Wh_Log(L"  CoCreateInstance(CLSID_OpenControlPanel) failed, hr=0x%08lX", (unsigned long)hr);
    }

    if (weInitialized) CoUninitialize();
    return answered;
}

// The verdict only changes when the machine's configuration changes (an edition
// upgrade, a digitizer being attached), so it is persisted in the mod's local
// storage and the shell probe is skipped entirely on subsequent starts. This
// matters because Wh_ModInit runs on the target's main thread *before* the
// process executes a single instruction of its own, and GetPath forces the
// shell to build its whole Control Panel item list - a cost that would
// otherwise be paid at every logon, every Explorer restart and every
// control.exe launch.
//
// The cache is keyed by Windows build so a feature update re-probes once, and
// stores a tri-state (unknown / not-shown / shown) plus the CLSID-registered
// bit. Users who suspect a stale verdict have two escape hatches: the
// "Always add"/"Never add" override, or simply saving the mod settings, which
// discards the cache and re-probes once (see Wh_ModSettingsChanged). The cache
// is deliberately NOT cleared in Wh_ModUninit, which also runs on every normal
// process exit and would defeat the caching entirely.
enum class CachedVerdict { Unknown = 0, NotShown = 1, Shown = 2 };

std::wstring MakeVerdictValueName(const wchar_t* key) {
    return std::wstring(L"appletVerdict_") + key;
}
std::wstring MakeVerdictBuildValueName(const wchar_t* key) {
    return std::wstring(L"appletVerdictBuild_") + key;
}

// Runs the shell probe at most once per Windows build and remembers the answer.
// Returns the "inject the virtual applet" verdict.
bool DetectVirtualAppletNeededCached(const std::wstring& realGuid,
                                     const std::wstring& canonicalName,
                                     const wchar_t* storageKey, const wchar_t* logName,
                                     std::atomic<bool>& outClsidRegistered) {
    // Cheap and always current: a registry key existence check, no shell work.
    const bool registeredClsid = IsRegisteredClsid(realGuid);
    outClsidRegistered.store(registeredClsid);
    if (!registeredClsid) {
        Wh_Log(L"%s: CLSID is absent on this edition/device; applet will not be injected", logName);
        return false;
    }

    const std::wstring verdictName = MakeVerdictValueName(storageKey);
    const std::wstring buildName   = MakeVerdictBuildValueName(storageKey);

    const int cachedVerdict = Wh_GetIntValue(verdictName.c_str(), (int)CachedVerdict::Unknown);
    const int cachedBuild   = Wh_GetIntValue(buildName.c_str(), 0);
    if (cachedVerdict != (int)CachedVerdict::Unknown && cachedBuild == (int)g_winBuild) {
        const bool shown = (cachedVerdict == (int)CachedVerdict::Shown);
        Wh_Log(L"%s: using cached verdict from build %d (applet is %s); shell not probed",
            logName, cachedBuild, shown ? L"already shown" : L"not shown");
        return !shown;
    }

    Wh_Log(L"%s: no cached verdict for build %u; probing the shell once", logName, g_winBuild);

    bool listed = false;
    if (IsShownByControlPanel(canonicalName, realGuid, listed)) {
        Wh_Log(L"%s: shell reports the applet is %s", logName,
            listed ? L"already shown; virtual entry skipped to avoid a duplicate"
                   : L"not shown; virtual entry will be injected");
        Wh_SetIntValue(verdictName.c_str(),
            (int)(listed ? CachedVerdict::Shown : CachedVerdict::NotShown));
        Wh_SetIntValue(buildName.c_str(), (int)g_winBuild);
        return !listed;
    }

    // No verdict: fall back to the registry hint and deliberately do NOT cache
    // it, so a later start can still get a real answer from the shell.
    const bool registered = IsListedInControlPanelNameSpace(realGuid);
    Wh_Log(L"%s: shell gave no verdict, falling back to the registry hint (%s). "
           L"Use the \"Always add\"/\"Never add\" setting if this is wrong.",
        logName, registered ? L"registered, assuming already shown" : L"not registered, injecting");
    return !registered;
}

// Combines the automatic detection with the user's explicit override.
bool ResolveAppletInjection(AppletMode mode, bool autoDetected, bool clsidRegistered,
                            const wchar_t* logName) {
    switch (mode) {
        case AppletMode::Always:
            if (!clsidRegistered) {
                Wh_Log(L"%s: \"Always add\" requested, but the applet is not installed here; ignoring", logName);
                return false;
            }
            Wh_Log(L"%s: forced ON by settings (auto detection said %d)", logName, (int)autoDetected);
            return true;
        case AppletMode::Never:
            Wh_Log(L"%s: forced OFF by settings", logName);
            return false;
        case AppletMode::Auto:
        default:
            return autoDetected;
    }
}

bool IsHomeGroupAvailable() {
    return g_settings.enableHomeGroup.load() && g_homeGroupClsidAvailable.load();
}

// Reads a REG_SZ/REG_EXPAND_SZ value from an already-open key via the plain,
// unhooked registry API (only ever called from InitDisplayNames, before this
// mod's own hooks are installed - see call site).
bool ReadStringValue(HKEY key, const wchar_t* valueName, std::wstring& out) {
    DWORD type = 0, size = 0;
    if (RegQueryValueExW(key, valueName, nullptr, &type, nullptr, &size) != ERROR_SUCCESS || size == 0)
        return false;
    if (type != REG_SZ && type != REG_EXPAND_SZ) return false;
    std::wstring buffer(size / sizeof(wchar_t) + 1, L'\0');
    if (RegQueryValueExW(key, valueName, nullptr, &type, (LPBYTE)buffer.data(), &size) != ERROR_SUCCESS)
        return false;
    buffer.resize(wcslen(buffer.c_str()));
    out = std::move(buffer);
    return true;
}

// Copies the display name and icon of a REAL, already-registered CLSID, so a
// virtual entry mirroring it always matches whatever this Windows build
// actually ships - no hardcoded resource indices to go stale between builds.
// Name resolution follows the same "LocalizedString wins, indirect strings
// get resolved, plain (Default) value is the fallback" rule Explorer itself
// uses for CLSID display names.
bool ReadRealClsidNameAndIcon(const std::wstring& realGuid, std::wstring& outName, std::wstring& outIcon) {
    ScopedHKey clsidKey;
    const std::wstring clsidPath = L"CLSID\\" + realGuid;
    LSTATUS openStatus = RegOpenKeyExW(HKEY_CLASSES_ROOT, clsidPath.c_str(), 0, KEY_READ, clsidKey.AddressOf());
    if (openStatus != ERROR_SUCCESS) {
        Wh_Log(L"  [%s] RegOpenKeyExW failed, status=%ld", realGuid.c_str(), openStatus);
        return false;
    }

    std::wstring rawName;
    bool gotLocalized = ReadStringValue(clsidKey.Get(), L"LocalizedString", rawName);
    Wh_Log(L"  [%s] LocalizedString: %s (raw=\"%s\")", realGuid.c_str(),
        gotLocalized ? L"found" : L"absent", rawName.c_str());
    if (!gotLocalized) {
        bool gotDefault = ReadStringValue(clsidKey.Get(), nullptr, rawName);
        Wh_Log(L"  [%s] (Default): %s (raw=\"%s\")", realGuid.c_str(),
            gotDefault ? L"found" : L"absent", rawName.c_str());
    }

    if (rawName.empty()) {
        Wh_Log(L"  [%s] No usable name value at all", realGuid.c_str());
        return false;
    }

    if (rawName[0] == L'@') {
        wchar_t resolved[512] = { 0 };
        HRESULT hr = SHLoadIndirectString(rawName.c_str(), resolved, ARRAYSIZE(resolved), nullptr);
        Wh_Log(L"  [%s] SHLoadIndirectString(\"%s\") -> hr=0x%08lX, result=\"%s\"",
            realGuid.c_str(), rawName.c_str(), (unsigned long)hr, resolved);
        if (SUCCEEDED(hr) && resolved[0]) {
            outName = resolved;
        } else {
            return false;
        }
    } else {
        outName = rawName;
    }

    ScopedHKey iconKey;
    const std::wstring iconPath = clsidPath + L"\\DefaultIcon";
    if (RegOpenKeyExW(HKEY_CLASSES_ROOT, iconPath.c_str(), 0, KEY_READ, iconKey.AddressOf()) == ERROR_SUCCESS) {
        bool gotIcon = ReadStringValue(iconKey.Get(), nullptr, outIcon);
        Wh_Log(L"  [%s] DefaultIcon: %s (raw=\"%s\")", realGuid.c_str(),
            gotIcon ? L"found" : L"absent", outIcon.c_str());
    } else {
        Wh_Log(L"  [%s] DefaultIcon subkey missing", realGuid.c_str());
    }
    return true;
}

// A Control Panel entry this mod registers from scratch under its own CLSID
// (same technique as Personalization above), whose identity (name/icon) is
// copied at runtime from a real applet and whose command re-launches that
// real applet. Used for applets Explorer's Category View never queries on
// its own (see the long comment by kBitLockerVirtualGuid).
struct VirtualApplet {
    std::wstring guidLower;
    std::wstring clsidSuffix, defaultIconSuffix, shellSuffix, shellOpenSuffix, openCommandSuffix, nsSuffix;
    std::wstring displayName;
    std::wstring iconValue;
    std::wstring infoTip;       // Indirect-string resource shown as the Win7-style description tooltip
    std::wstring openCommand;
    DWORD category = 0;
    std::atomic<bool>* enabledSetting = nullptr;
};

static std::vector<VirtualApplet> g_virtualApplets;

// Resolves an indirect-string resource reference ("@dll,-id") to its actual
// localized text using the same API Explorer uses. Returns an empty string on
// failure.
std::wstring ResolveIndirectString(const std::wstring& indirect) {
    if (indirect.empty() || indirect[0] != L'@') return indirect;
    wchar_t resolved[512] = { 0 };
    HRESULT hr = SHLoadIndirectString(indirect.c_str(), resolved, ARRAYSIZE(resolved), nullptr);
    if (SUCCEEDED(hr) && resolved[0]) {
        Wh_Log(L"  SHLoadIndirectString(\"%s\") -> \"%s\"", indirect.c_str(), resolved);
        return resolved;
    }
    Wh_Log(L"  SHLoadIndirectString(\"%s\") failed, hr=0x%08lX", indirect.c_str(), (unsigned long)hr);
    return L"";
}

// Builds one VirtualApplet entry by copying the real applet's name/icon.
// On Windows 10/11 the legacy CLSID keys are often mere COM stubs: the key
// exists (so IsRegisteredClsid succeeds) but carries no LocalizedString or
// (Default) value, because Windows now resolves these applets through their
// canonical name straight from the resource DLL. When the registry read comes
// up empty, we fall back to the well-known resource references for that
// applet (same technique Personalization already uses with themecpl.dll).
// The infotip fallback is the resource reference Explorer shows as the
// Win7-style description ("white text") under the applet link; Windows
// localizes it automatically for every installed UI language.
// Returns false only if both paths fail.
bool AddVirtualApplet(const std::wstring& virtualGuid, const std::wstring& realGuid,
                      DWORD category, std::atomic<bool>* enabledSetting,
                      const std::wstring& fallbackNameIndirect = L"",
                      const std::wstring& fallbackIcon = L"",
                      const std::wstring& fallbackInfoTip = L"") {
    std::wstring name, icon;
    bool gotFromRegistry = ReadRealClsidNameAndIcon(realGuid, name, icon);
    if (!gotFromRegistry || name.empty()) {
        if (!fallbackNameIndirect.empty()) {
            Wh_Log(L"  [%s] registry name unavailable, using resource fallback \"%s\"",
                realGuid.c_str(), fallbackNameIndirect.c_str());
            name = ResolveIndirectString(fallbackNameIndirect);
        }
    }

    // Independent of how the name was obtained: ReadRealClsidNameAndIcon treats
    // a missing DefaultIcon subkey as non-fatal (returns true with an empty
    // icon), which is the common case for the Win10/11 stub CLSID keys. Nesting
    // this inside the name-failure branch above meant that a CLSID with a
    // LocalizedString but no DefaultIcon produced an entry with no icon at all,
    // because TryProvideValue refuses to serve an empty DefaultIcon value.
    if (icon.empty() && !fallbackIcon.empty()) {
        Wh_Log(L"  [%s] no DefaultIcon in the registry, using resource fallback \"%s\"",
            realGuid.c_str(), fallbackIcon.c_str());
        icon = fallbackIcon;
    }

    if (name.empty()) {
        Wh_Log(L"  [%s] no usable name from registry or fallback", realGuid.c_str());
        return false;
    }

    // InfoTip: the Win7-style description that appears below the applet name
    // in Control Panel category view and in the hover tooltip. We pre-resolve
    // the "@dll,-id" indirect string here so Explorer receives plain, already
    // localized text regardless of whether it resolves indirect strings for
    // synthetic CLSIDs. The original indirect reference is kept too as a
    // secondary value (infoTipIndirect) for parity with real applets.
    std::wstring infoTipResolved;
    if (!fallbackInfoTip.empty()) {
        infoTipResolved = ResolveIndirectString(fallbackInfoTip);
        Wh_Log(L"  [%s] InfoTip resolved: \"%s\"", realGuid.c_str(),
            infoTipResolved.empty() ? L"(empty)" : infoTipResolved.c_str());
    }

    VirtualApplet applet;
    applet.guidLower = ToLower(virtualGuid);
    applet.clsidSuffix       = L"clsid\\" + applet.guidLower;
    applet.defaultIconSuffix = applet.clsidSuffix + L"\\defaulticon";
    applet.shellSuffix       = applet.clsidSuffix + L"\\shell";
    applet.shellOpenSuffix   = applet.shellSuffix + L"\\open";
    applet.openCommandSuffix = applet.shellOpenSuffix + L"\\command";
    applet.nsSuffix          = L"controlpanel\\namespace\\" + applet.guidLower;
    applet.displayName = name;
    applet.iconValue = icon;
    applet.infoTip = infoTipResolved.empty() ? fallbackInfoTip : infoTipResolved;
    applet.openCommand = L"explorer.exe shell:::" + realGuid;
    applet.category = category;
    applet.enabledSetting = enabledSetting;
    g_virtualApplets.push_back(std::move(applet));
    return true;
}

// Allocation-free case-insensitive check for the registry-hook hot path.
// Most keys opened by Explorer are unrelated, so avoid creating a lowercase
// std::wstring until a path is actually relevant to this mod. Both needles
// are checked in a single pass over the string instead of one full scan per
// needle, halving the towlower/compare work in the common not-found case.
bool ContainsRelevantKeywordInsensitive(const std::wstring& path) {
    static const wchar_t kClsid[] = L"clsid";
    static const wchar_t kControlPanel[] = L"controlpanel";
    static const size_t kClsidLen = wcslen(kClsid);
    static const size_t kControlPanelLen = wcslen(kControlPanel);

    const auto matchesAt = [&path](size_t i, const wchar_t* needle, size_t needleLength) {
        if (i + needleLength > path.size()) return false;
        size_t j = 0;
        while (j < needleLength && towlower(path[i + j]) == needle[j]) ++j;
        return j == needleLength;
    };

    for (size_t i = 0; i < path.size(); ++i) {
        if (matchesAt(i, kClsid, kClsidLen)) return true;
        if (matchesAt(i, kControlPanel, kControlPanelLen)) return true;
    }
    return false;
}

// Guards g_classicTaskLinksFilePath. EnsureClassicTaskLinksFile() is called
// eagerly from Wh_ModInit (before any hook is installed) and from
// Wh_ModSettingsChanged, but also lazily from the registry hooks by way of
// GetOrCreateClassicTaskLinksFilePath(), i.e. on arbitrary explorer.exe
// threads. Without this lock, two threads could race on the check-then-write
// below, or one thread could read a half-written path while another is
// (re)generating the file.
static std::mutex g_taskLinksMutex;

// Thread-safe accessor for readers (TryProvideValue and friends) that just
// want the current path without regenerating anything.
std::wstring GetClassicTaskLinksFilePath() {
    std::lock_guard<std::mutex> lock(g_taskLinksMutex);
    return g_classicTaskLinksFilePath;
}

// Creates a self-contained task list used by Control Panel to display the
// classic task links below the Personalization item.
bool EnsureClassicTaskLinksFile() {
    std::lock_guard<std::mutex> lock(g_taskLinksMutex);
    if (!g_classicTaskLinksFilePath.empty()) {
        DWORD attributes = GetFileAttributesW(g_classicTaskLinksFilePath.c_str());
        if (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY))
            return true;
        // The file vanished (temp cleanup, another instance's Wh_ModUninit, …);
        // fall through and regenerate instead of trusting the stale path.
        g_classicTaskLinksFilePath.clear();
    }

    wchar_t tempPath[MAX_PATH] = {};
    DWORD length = GetTempPathW(MAX_PATH, tempPath);
    if (!length || length >= MAX_PATH) return false;

    g_classicTaskLinksFilePath = std::wstring(tempPath) +
                                L"WindhawkClassicPersonalizationTasks.xml";

    struct TaskLinkTexts {
        const wchar_t* locale;
        const char* theme;
        const char* desktopBackground;
        const char* windowColors;
        const char* soundEffects;
        const char* screenSaver;
        const char* systemIcons;
        const char* restoreDefaultIconBehaviors;
        const char* networkStatus;
        const char* connectNetwork;
        const char* viewNetworkComputers;
        const char* addWirelessDevice;
        const char* addPrinter;
        const char* setDefaultPrinters;
        const char* changePrinterSettings;
        const char* viewDevicesPrinters;
        const char* chooseHomeGroup;
        const char* sharePrinters;
        const char* adjustScreenResolution;
        const char* reviewComputerStatus;
        const char* backUpComputer;
        const char* findAndFixProblems;
        const char* checkFirewallStatus;
        const char* uninstallProgram;
        const char* turnWindowsFeatures;
        const char* changeAccountPicture;
        const char* addRemoveAccounts;
        const char* setParentalControls;
        const char* changeDateTime;
        const char* changeInputMethod;
        const char* letWindowsSuggest;
        const char* changeHomePage;
        const char* manageBrowserAddons;
        const char* deleteBrowsingHistory;
        const char* bitlockerManage;
        const char* tabletCalibrate;
        const char* tabletPenTouch;
    };

    // Hard-coded localized Windows 7-style labels. The selected entry follows
    // the current Windows UI language; English is the fallback.
    // Complete static catalog for 30 common Windows UI languages. It has no
    // MUI/resource dependency; English remains the fallback for other locales.
    // Review corrections can be made one row at a time without altering logic.
     static const TaskLinkTexts kTaskLinkTexts[] = {
        { L"en", "Change the theme", "Change desktop background", "Change window glass colors", "Change sound effects", "Change screen saver", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Choose homegroup and sharing options", "Share printers", "Adjust screen resolution", "Review your computer's status", "Back up your computer", "Find and fix problems", "Check firewall status", "Uninstall a program", "Turn Windows features on or off", "Change account picture", "Add or remove user accounts", "Set up parental controls for any user", "Change the date and time", "Change input methods", "Let Windows suggest settings for you", "Change home page", "Manage browser add-ons", "Delete browsing history and cookies", "Manage BitLocker", "Calibrate the screen for pen or touch input", "Pen and touch settings" },
        { L"it", "Cambia tema", "Cambia sfondo del desktop", "Cambia colore delle finestre", "Cambia effetti sonori", "Cambia salvaschermo", "Attiva o disattiva le icone di sistema", "Ripristina comportamento icone predefinito", "Visualizza stato e attività della rete", "Connetti a una rete", "Visualizza computer e dispositivi di rete", "Aggiungi un dispositivo wireless alla rete", "Aggiungi una stampante", "Configura stampanti predefinite", "Modifica impostazioni stampante", "Visualizza dispositivi e stampanti", "Scegli gruppo home e opzioni di condivisione", "Condividi stampanti", "Regola risoluzione schermo", "Controlla stato del computer", "Esegui backup del computer", "Trova e correggi problemi", "Verifica stato firewall", "Disinstalla un programma", "Attiva o disattiva funzionalità di Windows", "Cambia immagine account", "Aggiungi o rimuovi account utente", "Configura controllo parentale", "Cambia data e ora", "Cambia metodo di input", "Consenti a Windows di suggerire le impostazioni", "Cambia home page", "Gestisci componenti aggiuntivi del browser", "Elimina cronologia e cookie", "Gestisci BitLocker", "Calibra lo schermo per l'input penna o tocco", "Impostazioni penna e tocco" },
        { L"es", "Cambiar tema", "Cambiar fondo de escritorio", "Cambiar color de las ventanas", "Cambiar efectos de sonido", "Cambiar protector de pantalla", "Activar o desactivar iconos del sistema", "Restaurar comportamiento predeterminado de iconos", "Ver estado y tareas de red", "Conectarse a una red", "Ver equipos y dispositivos de red", "Agregar un dispositivo inalámbrico a la red", "Agregar una impresora", "Configurar impresoras predeterminadas", "Cambiar configuración de impresora", "Ver dispositivos e impresoras", "Elegir grupo en el hogar y opciones de uso compartido", "Compartir impresoras", "Ajustar resolución de pantalla", "Revisar estado del equipo", "Hacer copia de seguridad del equipo", "Encontrar y solucionar problemas", "Comprobar estado del firewall", "Desinstalar un programa", "Activar o desactivar características de Windows", "Cambiar imagen de cuenta", "Agregar o quitar cuentas de usuario", "Configurar control parental", "Cambiar fecha y hora", "Cambiar métodos de entrada", "Permitir que Windows sugiera configuraciones", "Cambiar página principal", "Administrar complementos del navegador", "Eliminar historial de exploración y cookies", "Administrar BitLocker", "Calibrar la pantalla para la entrada de lápiz o táctil", "Configuración de lápiz y entrada táctil" },
        { L"fr", "Changer le thème", "Changer l'arrière-plan du bureau", "Changer les couleurs des vitres", "Changer les effets sonores", "Changer l'économiseur d'écran", "Activer ou désactiver les icônes du système", "Restaurer les comportements des icônes par défaut", "Afficher l'état et les tâches du réseau", "Connectez-vous à un réseau", "Afficher les ordinateurs et les appareils du réseau", "Ajouter un appareil sans fil au réseau", "Ajouter une imprimante", "Configurer les imprimantes par défaut", "Modifier les paramètres de l'imprimante", "Afficher les appareils et les imprimantes", "Choisissez le groupe résidentiel et les options de partage", "Partager des imprimantes", "Ajuster la résolution de l'écran", "Vérifiez l'état de votre ordinateur", "Sauvegardez votre ordinateur", "Rechercher et résoudre les problèmes", "Vérifier l'état du pare-feu", "Désinstaller un programme", "Activer ou désactiver des fonctionnalités Windows", "Changer la photo du compte", "Ajouter ou supprimer des comptes d'utilisateurs", "Configurer le contrôle parental pour n'importe quel utilisateur", "Changer la date et l'heure", "Changer les méthodes de saisie", "Laissez Windows vous suggérer des paramètres", "Modifier la page d'accueil", "Gérer les modules complémentaires du navigateur", "Supprimer l'historique de navigation et les cookies", "Gérer BitLocker", "Calibrer l'écran pour la saisie au stylet ou tactile", "Paramètres du stylet et de l'entrée tactile" },
        { L"de", "Design ändern", "Desktop-Hintergrund ändern", "Fensterfarbe ändern", "Soundeffekte ändern", "Bildschirmschoner ändern", "Systemsymbole ein- oder ausschalten", "Standardverhalten von Symbolen wiederherstellen", "Netzwerkstatus und -aufgaben anzeigen", "Mit einem Netzwerk verbinden", "Netzwerkcomputer und -geräte anzeigen", "Drahtloses Gerät zum Netzwerk hinzufügen", "Drucker hinzufügen", "Standarddrucker einrichten", "Druckereinstellungen ändern", "Geräte und Drucker anzeigen", "Heimnetzgruppen- und Freigabeoptionen auswählen", "Drucker freigeben", "Bildschirmauflösung anpassen", "Computerstatus überprüfen", "Computer sichern", "Probleme suchen und beheben", "Firewall-Status überprüfen", "Programm deinstallieren", "Windows-Funktionen aktivieren oder deaktivieren", "Kontobild ändern", "Benutzerkonten hinzufügen oder entfernen", "Kindersicherung für beliebige Benutzer einrichten", "Datum und Uhrzeit ändern", "Eingabemethoden ändern", "Windows-Einstellungen vorschlagen lassen", "Startseite ändern", "Browser-Add-Ons verwalten", "Browserverlauf und Cookies löschen", "BitLocker verwalten", "Bildschirm für Stift- oder Toucheingabe kalibrieren", "Stift- und Berührungseinstellungen" },
        { L"pt-BR", "Mude o tema", "Alterar plano de fundo da área de trabalho", "Alterar as cores dos vidros das janelas", "Alterar efeitos sonoros", "Alterar protetor de tela", "Ativar ou desativar ícones do sistema", "Restaurar comportamentos padrão dos ícones", "Visualize o status e as tarefas da rede", "Conecte-se a uma rede", "Ver computadores e dispositivos de rede", "Adicione um dispositivo sem fio à rede", "Adicionar uma impressora", "Configurar impressoras padrão", "Alterar configurações da impressora", "Ver dispositivos e impressoras", "Escolha opções de grupo doméstico e compartilhamento", "Compartilhar impressoras", "Ajustar a resolução da tela", "Revise o status do seu computador", "Faça backup do seu computador", "Encontre e corrija problemas", "Verifique o status do firewall", "Desinstalar um programa", "Ativar ou desativar recursos do Windows", "Alterar imagem da conta", "Adicionar ou remover contas de usuário", "Configure o controle dos pais para qualquer usuário", "Alterar a data e hora", "Alterar métodos de entrada", "Deixe o Windows sugerir configurações para você", "Alterar página inicial", "Gerenciar complementos do navegador", "Excluir histórico de navegação e cookies", "Gerenciar BitLocker", "Calibrar a tela para entrada por caneta ou toque", "Configurações de Caneta e Toque" },
        { L"pt-PT", "Mude o tema", "Alterar o fundo da área de trabalho", "Alterar as cores dos vidros das janelas", "Alterar efeitos sonoros", "Alterar protetor de ecrã", "Ativar ou desativar os ícones do sistema", "Restaurar os comportamentos padrão dos ícones", "Visualize o estado e as tarefas da rede", "Ligue-se a uma rede", "Ver computadores e dispositivos de rede", "Adicione um dispositivo sem fios à rede", "Adicionar uma impressora", "Configurar impressoras padrão", "Alterar as definições da impressora", "Ver dispositivos e impressoras", "Escolha as opções de grupo doméstico e partilha", "Partilhar impressoras", "Ajustar a resolução do ecrã", "Reveja o estado do seu computador", "Faça cópias de segurança do seu computador", "Encontre e corrija problemas", "Verifique o estado do firewall", "Desinstalar um programa", "Ativar ou desativar funcionalidades do Windows", "Alterar imagem da conta", "Adicionar ou remover contas de utilizador", "Configure o controlo parental para qualquer utilizador", "Alterar a data e hora", "Alterar métodos de entrada", "Deixe o Windows sugerir-lhe definições", "Alterar página inicial", "Gerir suplementos do navegador", "Eliminar histórico de navegação e cookies", "Gerir o BitLocker", "Calibrar o ecrã para entrada de caneta ou toque", "Definições de Caneta e Toque" },
        { L"nl", "Verander het thema", "Bureaubladachtergrond wijzigen", "Verander de kleuren van vensterglas", "Verander geluidseffecten", "Schermbeveiliging wijzigen", "Systeempictogrammen in- of uitschakelen", "Herstel het standaardpictogramgedrag", "Bekijk de netwerkstatus en taken", "Maak verbinding met een netwerk", "Bekijk netwerkcomputers en apparaten", "Voeg een draadloos apparaat toe aan het netwerk", "Voeg een printer toe", "Standaardprinters instellen", "Wijzig de printerinstellingen", "Bekijk apparaten en printers", "Kies thuisgroep- en deelopties", "Deel printers", "Pas de schermresolutie aan", "Controleer de status van uw computer", "Maak een back-up van uw computer", "Problemen vinden en oplossen", "Controleer de firewallstatus", "Een programma verwijderen", "Schakel Windows-functies in of uit", "Accountafbeelding wijzigen", "Gebruikersaccounts toevoegen of verwijderen", "Stel ouderlijk toezicht in voor elke gebruiker", "Wijzig de datum en tijd", "Wijzig invoermethoden", "Laat Windows instellingen voor u voorstellen", "Startpagina wijzigen", "Browser-invoegtoepassingen beheren", "Browsergeschiedenis en cookies verwijderen", "BitLocker beheren", "Scherm kalibreren voor pen- of aanraakinvoer", "Instellingen voor pen en aanraking" },
        { L"pl", "Zmień motyw", "Zmień tło pulpitu", "Zmień kolory szyb okiennych", "Zmień efekty dźwiękowe", "Zmień wygaszacz ekranu", "Włącz lub wyłącz ikony systemowe", "Przywróć domyślne zachowanie ikon", "Wyświetl stan sieci i zadania", "Połącz się z siecią", "Wyświetl komputery i urządzenia sieciowe", "Dodaj urządzenie bezprzewodowe do sieci", "Dodaj drukarkę", "Skonfiguruj drukarki domyślne", "Zmień ustawienia drukarki", "Wyświetl urządzenia i drukarki", "Wybierz grupę domową i opcje udostępniania", "Udostępnij drukarki", "Dostosuj rozdzielczość ekranu", "Sprawdź stan swojego komputera", "Utwórz kopię zapasową komputera", "Znajdź i rozwiąż problemy", "Sprawdź stan zapory", "Odinstaluj program", "Włącz lub wyłącz funkcje systemu Windows", "Zmień zdjęcie konta", "Dodaj lub usuń konta użytkowników", "Skonfiguruj kontrolę rodzicielską dla dowolnego użytkownika", "Zmień datę i godzinę", "Zmień metody wprowadzania", "Pozwól systemowi Windows zasugerować ustawienia", "Zmień stronę główną", "Zarządzaj dodatkami przeglądarki", "Usuń historię przeglądania i pliki cookie", "Zarządzaj funkcją BitLocker", "Skalibruj ekran pod kątem wprowadzania piórem lub dotykiem", "Ustawienia pióra i dotyku" },
        { L"ru", "Изменить тему", "Изменить фон рабочего стола", "Изменить цвет оконного стекла", "Изменить звуковые эффекты", "Изменить заставку", "Включить или выключить системные значки", "Восстановить поведение значков по умолчанию", "Просмотреть состояние сети и задачи", "Подключиться к сети", "Просмотреть сетевые компьютеры и устройства", "Добавить беспроводное устройство в сеть", "Добавить принтер", "Настроить принтеры по умолчанию", "Изменить настройки принтера", "Просмотреть устройства и принтеры", "Выбрать домашнюю группу и параметры общего доступа", "Предоставить общий доступ к принтерам", "Настроить разрешение экрана", "Проверить состояние компьютера", "Создать резервную копию компьютера", "Найти и устранить проблемы", "Проверить состояние брандмауэра", "Удалить программу", "Включить или выключить компоненты Windows", "Изменить изображение аккаунта", "Добавить или удалить учетные записи пользователей", "Настроить родительский контроль для любого пользователя", "Изменить дату и время", "Изменить методы ввода", "Разрешить Windows предлагать параметры", "Изменить домашнюю страницу", "Управление надстройками браузера", "Удаление журнала браузера и файлов cookie", "Управление BitLocker", "Калибровка экрана для пера или сенсорного ввода", "Параметры пера и сенсорного ввода" },
        { L"uk", "Змінити тему", "Змінити фон робочого столу", "Змінити колір віконного скла", "Змінити звукові ефекти", "Змінити заставку", "Увімкнути або вимкнути системні значки", "Відновити поведінку піктограм за замовчуванням", "Переглянути стан мережі та завдання", "Підключитися до мережі", "Переглянути мережеві комп'ютери й пристрої", "Додати бездротовий пристрій до мережі", "Додати принтер", "Налаштувати принтери за замовчуванням", "Змінити налаштування принтера", "Переглянути пристрої та принтери", "Вибрати домашню групу та параметри спільного доступу", "Спільно використовувати принтери", "Налаштувати роздільну здатність екрана", "Перевірити стан комп'ютера", "Створити резервну копію комп'ютера", "Знайти й усунути проблеми", "Перевірити стан брандмауера", "Видалити програму", "Увімкнути або вимкнути компоненти Windows", "Змінити зображення облікового запису", "Додати або видалити облікові записи користувачів", "Налаштувати батьківський контроль для будь-якого користувача", "Змінити дату й час", "Змінити методи введення", "Дозволити Windows пропонувати параметри", "Змінити домашню сторінку", "Керування надбудовами браузера", "Видалити журнал браузера та файли cookie", "Керування BitLocker", "Калібрування екрана для пера або сенсорного введення", "Параметри пера та сенсорного введення" },
        { L"tr", "Temayı değiştir", "Masaüstü arka planını değiştir", "Pencere camı renklerini değiştirme", "Ses efektlerini değiştir", "Ekran koruyucuyu değiştir", "Sistem simgelerini açma veya kapatma", "Varsayılan simge davranışlarını geri yükle", "Ağ durumunu ve görevlerini görüntüleyin", "Bir ağa bağlanma", "Ağ bilgisayarlarını ve cihazlarını görüntüleyin", "Ağa kablosuz cihaz ekleme", "Yazıcı ekle", "Varsayılan yazıcıları ayarlama", "Yazıcı ayarlarını değiştirin", "Cihazları ve yazıcıları görüntüleyin", "Ev grubu ve paylaşım seçeneklerini seçin", "Yazıcıları paylaş", "Ekran çözünürlüğünü ayarlayın", "Bilgisayarınızın durumunu inceleyin", "Bilgisayarınızı yedekleyin", "Sorunları bulun ve düzeltin", "Güvenlik duvarı durumunu kontrol edin", "Bir programı kaldırma", "Windows özelliklerini açma veya kapatma", "Hesap resmini değiştir", "Kullanıcı hesaplarını ekleme veya kaldırma", "Herhangi bir kullanıcı için ebeveyn denetimlerini ayarlayın", "Tarihi ve saati değiştirme", "Giriş yöntemlerini değiştirin", "Windows'un sizin için ayarlar önermesine izin verin", "Giriş sayfasını değiştir", "Tarayıcı eklentilerini yönet", "Göz atma geçmişini ve tanımlama bilgilerini sil", "BitLocker'ı Yönet", "Ekranı kalem veya dokunmatik giriş için kalibre et", "Kalem ve dokunmatik ayarları" },
        { L"ar", "تغيير الموضوع", "تغيير خلفية سطح المكتب", "تغيير ألوان زجاج النوافذ", "تغيير المؤثرات الصوتية", "تغيير شاشة التوقف", "تشغيل أيقونات النظام أو إيقاف تشغيلها", "استعادة سلوكيات الأيقونة الافتراضية", "عرض حالة الشبكة والمهام", "الاتصال بالشبكة", "عرض أجهزة الكمبيوتر والأجهزة المتصلة بالشبكة", "إضافة جهاز لاسلكي إلى الشبكة", "إضافة طابعة", "إعداد الطابعات الافتراضية", "تغيير إعدادات الطابعة", "عرض الأجهزة والطابعات", "اختيار مجموعة المشاركة المنزلية وخيارات المشاركة", "مشاركة الطابعات", "ضبط دقة الشاشة", "مراجعة حالة الكمبيوتر", "إنشاء نسخة احتياطية للكمبيوتر", "البحث عن المشاكل وإصلاحها", "التحقق من حالة جدار الحماية", "إلغاء تثبيت برنامج", "تشغيل ميزات Windows أو إيقاف تشغيلها", "تغيير صورة الحساب", "إضافة أو إزالة حسابات المستخدمين", "إعداد الضوابط الأبوية لأي مستخدم", "تغيير التاريخ والوقت", "تغيير طرق الإدخال", "السماح لـ Windows باقتراح الإعدادات", "تغيير الصفحة الرئيسية", "إدارة الوظائف الإضافية للمتصفح", "حذف محفوظات الاستعراض وملفات تعريف الارتباط", "إدارة BitLocker", "معايرة الشاشة لإدخال القلم أو اللمس", "إعدادات القلم واللمس" },
        { L"he", "שנה את הנושא", "שנה רקע שולחן העבודה", "שנה את צבעי זכוכית החלון", "שנה אפקטים קוליים", "שנה שומר מסך", "הפעל או כבה את סמלי המערכת", "שחזר את התנהגויות ברירת המחדל של סמלים", "הצג את מצב הרשת ומשימות", "התחבר לרשת", "הצג מחשבים והתקנים ברשת", "הוסף התקן אלחוטי לרשת", "הוסף מדפסת", "הגדר מדפסות ברירת מחדל", "שנה את הגדרות המדפסת", "הצג מכשירים ומדפסות", "בחר קבוצה ביתית ואפשרויות שיתוף", "שתף מדפסות", "התאם את רזולוציית המסך", "בדוק את מצב המחשב שלך", "גבה את המחשב שלך", "מצא ותקן בעיות", "בדוק את מצב חומת האש", "הסר התקנה של תוכנית", "הפעל או כבה את תכונות Windows", "שנה את תמונת החשבון", "הוסף או הסר חשבונות משתמש", "הגדר בקרת הורים עבור כל משתמש", "שנה את התאריך והשעה", "שנה שיטות קלט", "תן ל-Windows להציע עבורך הגדרות", "שנה דף בית", "נהל תוספות דפדפן", "מחק היסטוריית גלישה וקובצי Cookie", "נהל את BitLocker", "כייל את המסך עבור קלט עט או מגע", "הגדרות עט ומגע" },
        { L"ja", "テーマを変更する", "デスクトップの背景を変更する", "窓ガラスの色を変更する", "効果音を変更する", "スクリーンセーバーを変更する", "システムアイコンをオンまたはオフにする", "デフォルトのアイコン動作を復元する", "ネットワークのステータスとタスクを表示する", "ネットワークに接続する", "ネットワークのコンピュータとデバイスを表示する", "ワイヤレスデバイスをネットワークに追加する", "プリンターを追加する", "デフォルトのプリンターを設定する", "プリンターの設定を変更する", "デバイスとプリンターを表示する", "ホームグループと共有オプションを選択する", "プリンターを共有する", "画面解像度を調整する", "コンピュータのステータスを確認する", "コンピュータをバックアップする", "問題を見つけて解決する", "ファイアウォールのステータスを確認する", "プログラムをアンインストールする", "Windows の機能をオンまたはオフにする", "アカウントの写真を変更する", "ユーザーアカウントの追加または削除", "任意のユーザーに対してペアレントコントロールを設定する", "日付と時刻を変更する", "入力方法を変更する", "Windows が設定を提案してくれるようにする", "ホーム ページの変更", "ブラウザーのアドオンの管理", "閲覧の履歴と Cookie の削除", "BitLocker の管理", "ペンまたはタッチ入力用に画面を調整する", "ペンとタッチの設定" },
        { L"ko", "테마 변경", "데스크탑 배경 변경", "창유리 색상 변경", "음향 효과 변경", "화면 보호기 변경", "시스템 아이콘 켜기 또는 끄기", "기본 아이콘 동작 복원", "네트워크 상태 및 작업 보기", "네트워크에 연결", "네트워크 컴퓨터 및 장치 보기", "네트워크에 무선 장치 추가", "프린터 추가", "기본 프린터 설정", "프린터 설정 변경", "장치 및 프린터 보기", "홈 그룹 및 공유 옵션 선택", "프린터 공유", "화면 해상도 조정", "컴퓨터 상태 검토", "컴퓨터 백업", "문제 찾기 및 수정", "방화벽 상태 확인", "프로그램 제거", "Windows 기능 켜기 또는 끄기", "계정 사진 변경", "사용자 계정 추가 또는 제거", "모든 사용자에 대해 자녀 보호 기능 설정", "날짜 및 시간 변경", "입력 방법 변경", "Windows에서 설정을 제안하도록 허용", "홈 페이지 변경", "브라우저 추가 기능 관리", "검색 기록 및 쿠키 삭제", "BitLocker 관리", "펜 또는 터치 입력용 화면 보정", "펜 및 터치 설정" },
        { L"zh-CN", "更改主题", "更改桌面背景", "改变窗玻璃颜色", "改变音效", "更改屏幕保护程序", "打开或关闭系统图标", "恢复默认图标行为", "查看网络状态和任务", "连接到网络", "查看网络计算机和设备", "将无线设备添加到网络", "添加打印机", "设置默认打印机", "更改打印机设置", "查看设备和打印机", "选择家庭组和共享选项", "共享打印机", "调整屏幕分辨率", "查看计算机的状态", "备份您的计算机", "发现并解决问题", "检查防火墙状态", "卸载程序", "打开或关闭 Windows 功能", "更改账户图片", "添加或删除用户帐户", "为任何用户设置家长控制", "更改日期和时间", "更改输入法", "让 Windows 为您建议设置", "更改主页", "管理浏览器加载项", "删除浏览历史记录和 Cookie", "管理 BitLocker", "校准笔和触摸输入的屏幕", "笔和触摸设置" },
        { L"zh-TW", "更改主題", "更改桌面背景", "改變窗玻璃顏色", "改變音效", "更改螢幕保護程式", "開啟或關閉系統圖標", "恢復預設圖示行為", "查看網路狀態和任務", "連接網路", "查看網路電腦和設備", "將無線設備新增至網絡", "新增印表機", "設定預設印表機", "變更印表機設定", "查看設備和印表機", "選擇家庭群組和共享選項", "共用印表機", "調整螢幕解析度", "查看計算機的狀態", "備份您的計算機", "發現並解決問題", "檢查防火牆狀態", "解除安裝程式", "開啟或關閉 Windows 功能", "更改帳戶圖片", "新增或刪除使用者帳戶", "為任何使用者設定家長監護", "更改日期和時間", "更改輸入法", "讓 Windows 為您建議設定", "變更首頁", "管理瀏覽器附加元件", "刪除瀏覽歷程記錄和 Cookie", "管理 BitLocker", "校正手寫筆或觸控輸入的畫面", "手寫筆與觸控設定" },
        { L"cs", "Změnit téma", "Změnit pozadí plochy", "Změnit barvu okenního skla", "Změnit zvukové efekty", "Změnit spořič obrazovky", "Zapnout nebo vypnout systémové ikony", "Obnovit výchozí chování ikon", "Zobrazit stav sítě a úlohy", "Připojit se k síti", "Zobrazit síťové počítače a zařízení", "Přidat bezdrátové zařízení do sítě", "Přidat tiskárnu", "Nastavit výchozí tiskárny", "Změnit nastavení tiskárny", "Zobrazit zařízení a tiskárny", "Vybrat domácí skupinu a možnosti sdílení", "Sdílet tiskárny", "Upravit rozlišení obrazovky", "Zkontrolovat stav počítače", "Zálohovat počítač", "Najít a opravit problémy", "Zkontrolovat stav brány firewall", "Odinstalovat program", "Zapnout nebo vypnout funkce systému Windows", "Změnit obrázek účtu", "Přidat nebo odebrat uživatelské účty", "Nastavit rodičovskou kontrolu pro libovolného uživatele", "Změnit datum a čas", "Změnit metody zadávání", "Nechat Windows navrhnout nastavení", "Změnit domovskou stránku", "Spravovat doplňky prohlížeče", "Odstranit historii procházení a soubory cookie", "Spravovat BitLocker", "Kalibrovat obrazovku pro pero nebo dotykové zadávání", "Nastavení pera a dotyku" },
        { L"da", "Skift tema", "Skift skrivebordsbaggrund", "Skift vinduesglasfarver", "Skift lydeffekter", "Skift pauseskærm", "Slå systemikoner til eller fra", "Gendan standard ikonadfærd", "Se netværksstatus og opgaver", "Opret forbindelse til et netværk", "Se netværkscomputere og -enheder", "Tilføj en trådløs enhed til netværket", "Tilføj en printer", "Konfigurer standardprintere", "Skift printerindstillinger", "Se enheder og printere", "Vælg hjemmegruppe og delingsmuligheder", "Del printere", "Juster skærmopløsningen", "Gennemgå din computers status", "Sikkerhedskopier din computer", "Find og ret problemer", "Tjek firewall-status", "Afinstaller et program", "Slå Windows-funktioner til eller fra", "Skift kontobillede", "Tilføj eller fjern brugerkonti", "Konfigurer forældrekontrol for enhver bruger", "Skift dato og klokkeslæt", "Skift indtastningsmetoder", "Lad Windows foreslå indstillinger for dig", "Skift startside", "Administrer browser-tilføjelser", "Slet browserhistorik og cookies", "Administrer BitLocker", "Kalibrér skærmen til pen- eller trykindtastning", "Indstillinger for pen og tryk" },
        { L"fi", "Vaihda teemaa", "Vaihda työpöydän tausta", "Vaihda ikkunalasien väriä", "Muuta äänitehosteita", "Vaihda näytönsäästäjä", "Ota järjestelmäkuvakkeet käyttöön tai poista ne käytöstä", "Palauta oletuskuvakkeiden toimintatavat", "Tarkastele verkon tilaa ja tehtäviä", "Yhdistä verkkoon", "Tarkastele verkon tietokoneita ja laitteita", "Lisää langaton laite verkkoon", "Lisää tulostin", "Aseta oletustulostimet", "Muuta tulostimen asetuksia", "Tarkastele laitteita ja tulostimia", "Valitse kotiryhmä- ja jakamisasetukset", "Jaa tulostimia", "Säädä näytön resoluutiota", "Tarkista tietokoneesi tila", "Varmuuskopioi tietokoneesi", "Etsi ja korjaa ongelmat", "Tarkista palomuurin tila", "Poista ohjelman asennus", "Ota Windowsin ominaisuudet käyttöön tai poista ne käytöstä", "Vaihda tilikuvaa", "Lisää tai poista käyttäjätilejä", "Määritä lapsilukko kaikille käyttäjille", "Muuta päivämäärää ja kellonaikaa", "Muuta syöttötapoja", "Anna Windowsin ehdottaa asetuksia puolestasi", "Vaihda aloitussivua", "Hallinnoi selaimen apuohjelmia", "Poista selaushistoria ja evästeet", "Hallitse BitLockeria", "Kalibroi näyttö kynä- tai kosketussyöttöä varten", "Kynä- ja kosketusasetukset" },
        { L"el", "Αλλαγή θέματος", "Αλλαγή φόντου επιφάνειας εργασίας", "Αλλαγή χρωμάτων τζαμιών παραθύρων", "Αλλαγή ηχητικών εφέ", "Αλλαγή προφύλαξης οθόνης", "Ενεργοποίηση ή απενεργοποίηση εικονιδίων συστήματος", "Επαναφορά προεπιλεγμένων συμπεριφορών εικονιδίων", "Προβολή κατάστασης και εργασιών δικτύου", "Σύνδεση σε δίκτυο", "Προβολή υπολογιστών και συσκευών δικτύου", "Προσθήκη ασύρματης συσκευής στο δίκτυο", "Προσθήκη εκτυπωτή", "Ρύθμιση προεπιλεγμένων εκτυπωτών", "Αλλαγή ρυθμίσεων εκτυπωτή", "Προβολή συσκευών και εκτυπωτών", "Επιλογή οικιακής ομάδας και επιλογών κοινής χρήσης", "Κοινή χρήση εκτυπωτών", "Προσαρμογή ανάλυσης οθόνης", "Έλεγχος κατάστασης υπολογιστή", "Δημιουργία αντιγράφου ασφαλείας υπολογιστή", "Εύρεση και επιδιόρθωση προβλημάτων", "Έλεγχος κατάστασης τείχους προστασίας", "Απεγκατάσταση προγράμματος", "Ενεργοποίηση ή απενεργοποίηση δυνατοτήτων των Windows", "Αλλαγή εικόνας λογαριασμού", "Προσθήκη ή κατάργηση λογαριασμών χρηστών", "Ρύθμιση γονικού ελέγχου για οποιονδήποτε χρήστη", "Αλλαγή ημερομηνίας και ώρας", "Αλλαγή μεθόδων εισαγωγής", "Να επιτρέπεται στα Windows να προτείνουν ρυθμίσεις", "Αλλαγή αρχικής σελίδας", "Διαχείριση προσθηκών προγράμματος περιήγησης", "Διαγραφή ιστορικού περιήγησης και cookies", "Διαχείριση BitLocker", "Βαθμονόμηση της οθόνης για είσοδο με πένα ή αφή", "Ρυθμίσεις πένας και αφής" },
        { L"hu", "Változtasd meg a témát", "Az asztal hátterének módosítása", "Az ablaküveg színének megváltoztatása", "Hanghatások módosítása", "Képernyővédő módosítása", "A rendszerikonok be- és kikapcsolása", "Az alapértelmezett ikonviselkedés visszaállítása", "Megtekintheti a hálózat állapotát és a feladatokat", "Csatlakozzon egy hálózathoz", "Tekintse meg a hálózati számítógépeket és eszközöket", "Adjon hozzá egy vezeték nélküli eszközt a hálózathoz", "Nyomtató hozzáadása", "Állítsa be az alapértelmezett nyomtatókat", "A nyomtató beállításainak módosítása", "Eszközök és nyomtatók megtekintése", "Válassza ki az otthoni csoportot és a megosztási beállításokat", "Nyomtatók megosztása", "Állítsa be a képernyő felbontását", "Tekintse át számítógépe állapotát", "Készítsen biztonsági másolatot a számítógépről", "Keresse meg és javítsa ki a problémákat", "Ellenőrizze a tűzfal állapotát", "Távolítson el egy programot", "Kapcsolja be vagy ki a Windows szolgáltatásait", "Fiókkép módosítása", "Felhasználói fiókok hozzáadása vagy eltávolítása", "Szülői felügyelet beállítása bármely felhasználó számára", "Módosítsa a dátumot és az időt", "Beviteli módszerek módosítása", "Hagyja, hogy a Windows beállításokat javasoljon Önnek", "Kezdőlap módosítása", "Böngésző-bővítmények kezelése", "Böngészési előzmények és cookie-k törlése", "BitLocker kezelése", "A képernyő kalibrálása toll- vagy érintéses bevitelhez", "Toll- és érintésbeállítások" },
        { L"nb", "Endre tema", "Endre skrivebordsbakgrunn", "Endre fargene på vinduets glass", "Endre lydeffekter", "Bytt skjermsparer", "Slå systemikoner på eller av", "Gjenopprett standard ikonatferd", "Se nettverksstatus og oppgaver", "Koble til et nettverk", "Se nettverksdatamaskiner og enheter", "Legg til en trådløs enhet i nettverket", "Legg til en skriver", "Sett opp standardskrivere", "Endre skriverinnstillinger", "Se enheter og skrivere", "Velg hjemmegruppe og delingsalternativer", "Del skrivere", "Juster skjermoppløsningen", "Se gjennom datamaskinens status", "Sikkerhetskopier datamaskinen", "Finn og fiks problemer", "Sjekk brannmurstatus", "Avinstaller et program", "Slå Windows-funksjoner på eller av", "Endre kontobilde", "Legg til eller fjern brukerkontoer", "Sett opp foreldrekontroll for alle brukere", "Endre dato og klokkeslett", "Endre inndatametoder", "La Windows foreslå innstillinger for deg", "Endre startside", "Administrer nettlesertillegg", "Slett nettleserhistorikk og informasjonskapsler", "Behandle BitLocker", "Kalibrer skjermen for penn- eller berøringsinndata", "Innstillinger for penn og berøring" },
        { L"ro", "Schimbați tema", "Schimbați fundalul desktopului", "Schimbați culorile geamului", "Schimbați efectele sonore", "Schimbați economizorul de ecran", "Activați sau dezactivați pictogramele de sistem", "Restabiliți comportamentul implicit al pictogramelor", "Vizualizați starea rețelei și sarcinile", "Conectați-vă la o rețea", "Vizualizați computerele și dispozitivele din rețea", "Adăugați un dispozitiv fără fir în rețea", "Adăugați o imprimantă", "Configurați imprimante implicite", "Modificați setările imprimantei", "Vizualizați dispozitivele și imprimantele", "Alegeți grupul de acasă și opțiunile de partajare", "Partajați imprimante", "Reglați rezoluția ecranului", "Examinați starea computerului dvs", "Faceți o copie de rezervă a computerului", "Găsiți și rezolvați problemele", "Verificați starea firewallului", "Dezinstalează un program", "Activați sau dezactivați funcțiile Windows", "Schimbați imaginea contului", "Adăugați sau eliminați conturi de utilizator", "Configurați controale parentale pentru orice utilizator", "Schimbați data și ora", "Schimbați metodele de introducere", "Lăsați Windows să vă sugereze setări", "Modificare pagina de pornire", "Gestionați suplimentele browserului", "Ștergeți istoricul de navigare și modulele cookie", "Gestionați BitLocker", "Calibrați ecranul pentru introducerea cu stiloul sau atingerea", "Setări pentru stilou și atingere" },
        { L"sv", "Ändra temat", "Ändra skrivbordsbakgrund", "Ändra fönsterglasfärger", "Ändra ljudeffekter", "Byt skärmsläckare", "Slå på eller av systemikoner", "Återställ standardikonbeteenden", "Visa nätverksstatus och uppgifter", "Anslut till ett nätverk", "Visa nätverksdatorer och enheter", "Lägg till en trådlös enhet i nätverket", "Lägg till en skrivare", "Konfigurera standardskrivare", "Ändra skrivarinställningar", "Visa enheter och skrivare", "Välj hemgrupp och delningsalternativ", "Dela skrivare", "Justera skärmupplösningen", "Granska din dators status", "Säkerhetskopiera din dator", "Hitta och åtgärda problem", "Kontrollera brandväggens status", "Avinstallera ett program", "Slå på eller av Windows-funktioner", "Byt kontobild", "Lägg till eller ta bort användarkonton", "Ställ in föräldrakontroll för alla användare", "Ändra datum och tid", "Ändra inmatningsmetoder", "Låt Windows föreslå inställningar åt dig", "Ändra startsida", "Hantera webbläsartillägg", "Ta bort webbhistorik och cookies", "Hantera BitLocker", "Kalibrera skärmen för penn- eller pekindata", "Inställningar för penna och pekning" },
        { L"vi", "Thay đổi chủ đề", "Thay đổi hình nền máy tính", "Thay đổi màu kính cửa sổ", "Thay đổi hiệu ứng âm thanh", "Thay đổi trình bảo vệ màn hình", "Bật hoặc tắt biểu tượng hệ thống", "Khôi phục hành vi biểu tượng mặc định", "Xem trạng thái và nhiệm vụ mạng", "Kết nối với mạng", "Xem máy tính và thiết bị mạng", "Thêm thiết bị không dây vào mạng", "Thêm máy in", "Thiết lập máy in mặc định", "Thay đổi cài đặt máy in", "Xem thiết bị và máy in", "Chọn nhóm nhà và tùy chọn chia sẻ", "Chia sẻ máy in", "Điều chỉnh độ phân giải màn hình", "Xem lại trạng thái máy tính của bạn", "Sao lưu máy tính của bạn", "Tìm và khắc phục sự cố", "Kiểm tra trạng thái tường lửa", "Gỡ cài đặt một chương trình", "Bật hoặc tắt các tính năng của Windows", "Thay đổi ảnh tài khoản", "Thêm hoặc xóa tài khoản người dùng", "Thiết lập quyền kiểm soát của phụ huynh cho bất kỳ người dùng nào", "Thay đổi ngày và giờ", "Thay đổi phương thức nhập", "Hãy để Windows đề xuất cài đặt cho bạn", "Thay đổi trang chủ", "Quản lý tiện ích bổ sung của trình duyệt", "Xóa lịch sử duyệt web và cookie", "Quản lý BitLocker", "Hiệu chỉnh màn hình cho nhập bằng bút hoặc cảm ứng", "Cài đặt bút và cảm ứng" },
        { L"id", "Ubah temanya", "Ubah latar belakang desktop", "Mengubah warna kaca jendela", "Ubah efek suara", "Ubah screen saver", "Mengaktifkan atau menonaktifkan ikon sistem", "Pulihkan perilaku ikon default", "Lihat status dan tugas jaringan", "Hubungkan ke jaringan", "Lihat komputer dan perangkat jaringan", "Tambahkan perangkat nirkabel ke jaringan", "Tambahkan pencetak", "Siapkan printer default", "Ubah pengaturan pencetak", "Lihat perangkat dan printer", "Pilih homegroup dan opsi berbagi", "Bagikan printer", "Sesuaikan resolusi layar", "Tinjau status komputer Anda", "Cadangkan komputer Anda", "Temukan dan perbaiki masalah", "Periksa status firewall", "Copot pemasangan suatu program", "Mengaktifkan atau menonaktifkan fitur Windows", "Ubah gambar akun", "Menambah atau menghapus akun pengguna", "Siapkan kontrol orang tua untuk pengguna mana pun", "Ubah tanggal dan waktu", "Ubah metode masukan", "Biarkan Windows menyarankan pengaturan untuk Anda", "Ubah beranda", "Kelola pengaya browser", "Hapus riwayat penjelajahan dan cookie", "Kelola BitLocker", "Kalibrasi layar untuk input pena atau sentuhan", "Pengaturan pena dan sentuhan" },
        { L"th", "เปลี่ยนธีม", "เปลี่ยนพื้นหลังเดสก์ท็อป", "เปลี่ยนสีกระจกหน้าต่าง", "เปลี่ยนเอฟเฟกต์เสียง", "เปลี่ยนภาพพักหน้าจอ", "เปิดหรือปิดไอคอนระบบ", "คืนค่าลักษณะการทำงานของไอคอนเริ่มต้น", "ดูสถานะเครือข่ายและงาน", "เชื่อมต่อกับเครือข่าย", "ดูคอมพิวเตอร์และอุปกรณ์เครือข่าย", "เพิ่มอุปกรณ์ไร้สายเข้ากับเครือข่าย", "เพิ่มเครื่องพิมพ์", "ตั้งค่าเครื่องพิมพ์เริ่มต้น", "เปลี่ยนการตั้งค่าเครื่องพิมพ์", "ดูอุปกรณ์และเครื่องพิมพ์", "เลือกโฮมกรุ๊ปและตัวเลือกการแชร์", "แบ่งปันเครื่องพิมพ์", "ปรับความละเอียดหน้าจอ", "ตรวจสอบสถานะของคอมพิวเตอร์ของคุณ", "สำรองข้อมูลคอมพิวเตอร์ของคุณ", "ค้นหาและแก้ไขปัญหา", "ตรวจสอบสถานะไฟร์วอลล์", "ถอนการติดตั้งโปรแกรม", "เปิดหรือปิดคุณสมบัติ Windows", "เปลี่ยนรูปบัญชี", "เพิ่มหรือลบบัญชีผู้ใช้", "ตั้งค่าการควบคุมโดยผู้ปกครองสำหรับผู้ใช้ทุกคน", "เปลี่ยนวันที่และเวลา", "เปลี่ยนวิธีการป้อนข้อมูล", "ให้ Windows แนะนำการตั้งค่าให้กับคุณ", "เปลี่ยนโฮมเพจ", "จัดการส่วนเสริมของเบราว์เซอร์", "ลบประวัติการเรียกดูและคุกกี้", "จัดการ BitLocker", "ปรับเทียบหน้าจอสำหรับการป้อนด้วยปากกาหรือการสัมผัส", "การตั้งค่าปากกาและการสัมผัส" },
        { L"hi", "थीम बदलें", "डेस्कटॉप पृष्ठभूमि बदलें", "खिड़की के शीशे का रंग बदलें", "ध्वनि प्रभाव बदलें", "स्क्रीन सेवर बदलें", "सिस्टम आइकन चालू या बंद करें", "डिफ़ॉल्ट आइकन व्यवहार पुनर्स्थापित करें", "नेटवर्क स्थिति और कार्य देखें", "किसी नेटवर्क से कनेक्ट करें", "नेटवर्क कंप्यूटर और डिवाइस देखें", "नेटवर्क में एक वायरलेस डिवाइस जोड़ें", "एक प्रिंटर जोड़ें", "डिफ़ॉल्ट प्रिंटर सेट करें", "प्रिंटर सेटिंग बदलें", "डिवाइस और प्रिंटर देखें", "होमग्रुप और साझाकरण विकल्प चुनें", "प्रिंटर साझा करें", "स्क्रीन रिज़ॉल्यूशन समायोजित करें", "अपने कंप्यूटर की स्थिति की समीक्षा करें", "अपने कंप्यूटर का बैकअप लें", "समस्याएं ढूंढें और ठीक करें", "फ़ायरवॉल स्थिति जाँचें", "किसी प्रोग्राम को अनइंस्टॉल करें", "विंडोज़ सुविधाओं को चालू या बंद करें", "खाता चित्र बदलें", "उपयोगकर्ता खाते जोड़ें या हटाएँ", "किसी भी उपयोगकर्ता के लिए अभिभावकीय नियंत्रण सेट करें", "दिनांक और समय बदलें", "इनपुट पद्धतियाँ बदलें", "विंडोज़ को आपके लिए सेटिंग्स सुझाने दें", "मुख पृष्ठ बदलें", "ब्राउज़र ऐड-ऑन प्रबंधित करें", "ब्राउज़िंग इतिहास और कुकीज़ हटाएं", "BitLocker प्रबंधित करें", "पेन या स्पर्श इनपुट के लिए स्क्रीन कैलिब्रेट करें", "पेन और स्पर्श सेटिंग्स" },
    };

    wchar_t localeName[LOCALE_NAME_MAX_LENGTH] = {};
    const LANGID uiLanguage = GetUserDefaultUILanguage();
    if (!LCIDToLocaleName(MAKELCID(uiLanguage, SORT_DEFAULT), localeName,
                          LOCALE_NAME_MAX_LENGTH, 0)) {
        wcscpy_s(localeName, L"en-US");
    }
    const TaskLinkTexts* texts = &kTaskLinkTexts[0];
    for (const auto& candidate : kTaskLinkTexts) {
        size_t prefixLength = wcslen(candidate.locale);
        if (_wcsnicmp(localeName, candidate.locale, prefixLength) == 0 &&
            (localeName[prefixLength] == L'\0' || localeName[prefixLength] == L'-')) {
            texts = &candidate;
            break;
        }
    }


    static const char kTaskListTemplate[] = R"xml(<?xml version="1.0" encoding="utf-8"?>
<applications xmlns="http://schemas.microsoft.com/windows/cpltasks/v1" xmlns:sh="http://schemas.microsoft.com/windows/tasks/v1">
  <application id="{580722ff-16a7-44c1-bf74-7e1acd00f4f9}">
    <sh:task id="{D4F4A001-0D35-4CB6-A21F-BC1661200001}"><sh:name>{THEME}</sh:name><sh:keywords>theme;personalization</sh:keywords><sh:controlpanel name="Microsoft.Personalization"/></sh:task>
    <sh:task id="{D4F4A002-0D35-4CB6-A21F-BC1661200002}"><sh:name>{BACKGROUND}</sh:name><sh:keywords>desktop;background;wallpaper</sh:keywords><sh:command>explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}\pageWallpaper</sh:command></sh:task>
    <sh:task id="{D4F4A003-0D35-4CB6-A21F-BC1661200003}"><sh:name>{COLORS}</sh:name><sh:keywords>window;color;glass;colorization</sh:keywords><sh:command>explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}\pageColorization</sh:command></sh:task>
    <sh:task id="{D4F4A004-0D35-4CB6-A21F-BC1661200004}"><sh:name>{SOUNDS}</sh:name><sh:keywords>sound;audio;effects</sh:keywords><sh:command>rundll32.exe shell32.dll,Control_RunDLL mmsys.cpl,,2</sh:command></sh:task>
    <sh:task id="{D4F4A005-0D35-4CB6-A21F-BC1661200005}"><sh:name>{SCREENSAVER}</sh:name><sh:keywords>screen saver;screensaver</sh:keywords><sh:command>rundll32.exe shell32.dll,Control_RunDLL desk.cpl,,@screensaver</sh:command></sh:task>
    <category id="1">
       <sh:task idref="{D4F4A001-0D35-4CB6-A21F-BC1661200001}"/>
       <sh:task idref="{D4F4A002-0D35-4CB6-A21F-BC1661200002}"/>
       <sh:task idref="{D4F4A003-0D35-4CB6-A21F-BC1661200003}"/>
       <sh:task idref="{D4F4A004-0D35-4CB6-A21F-BC1661200004}"/>
       <sh:task idref="{D4F4A005-0D35-4CB6-A21F-BC1661200005}"/>
    </category>
  </application>
  <application id="Microsoft.NotificationAreaIcons">
    <sh:task id="{D4F4B001-0D35-4CB6-A21F-BC1661200001}"><sh:name>{SYSTEMICONS}</sh:name><sh:controlpanel name="Microsoft.NotificationAreaIcons" page="SystemIcons"/></sh:task>
    <sh:task id="{D4F4B002-0D35-4CB6-A21F-BC1661200002}"><sh:name>{RESTOREICONS}</sh:name><sh:controlpanel name="Microsoft.NotificationAreaIcons"/></sh:task>
    <!-- This applet intentionally has no Control Panel category. Category 0
         keeps it out of Appearance and Personalization while retaining its
         search task links. -->
    <category id="0">
       <sh:task idref="{D4F4B001-0D35-4CB6-A21F-BC1661200001}"/>
       <sh:task idref="{D4F4B002-0D35-4CB6-A21F-BC1661200002}"/>
    </category>
  </application>
  <application id="{7007acc7-3202-11d1-aad2-00805fc1270e}">
    <sh:task id="{D4F4C001-0D35-4CB6-A21F-BC1661200001}"><sh:name>{NETWORKSTATUS}</sh:name><sh:controlpanel name="Microsoft.NetworkAndSharingCenter"/></sh:task>
    <sh:task id="{D4F4C002-0D35-4CB6-A21F-BC1661200002}"><sh:name>{CONNECTNETWORK}</sh:name><sh:controlpanel name="Microsoft.NetworkConnections"/></sh:task>
    <sh:task id="{D4F4C003-0D35-4CB6-A21F-BC1661200003}"><sh:name>{VIEWCOMPUTERS}</sh:name><sh:controlpanel name="Microsoft.NetworkAndSharingCenter"/></sh:task>
    <sh:task id="{D4F4C004-0D35-4CB6-A21F-BC1661200004}"><sh:name>{ADDWIRELESS}</sh:name><sh:controlpanel name="Microsoft.NetworkAndSharingCenter"/></sh:task>
    <category id="3"><sh:task idref="{D4F4C001-0D35-4CB6-A21F-BC1661200001}"/><sh:task idref="{D4F4C002-0D35-4CB6-A21F-BC1661200002}"/><sh:task idref="{D4F4C003-0D35-4CB6-A21F-BC1661200003}"/><sh:task idref="{D4F4C004-0D35-4CB6-A21F-BC1661200004}"/></category>
  </application>
  <application id="{2227a280-3aea-1069-a2de-08002b30309d}">
    <sh:task id="{D4F4D001-0D35-4CB6-A21F-BC1661200001}"><sh:name>{ADDPRINTER}</sh:name><sh:controlpanel name="Microsoft.DevicesAndPrinters"/></sh:task>
    <sh:task id="{D4F4D002-0D35-4CB6-A21F-BC1661200002}"><sh:name>{DEFAULTPRINTERS}</sh:name><sh:controlpanel name="Microsoft.DevicesAndPrinters"/></sh:task>
    <sh:task id="{D4F4D003-0D35-4CB6-A21F-BC1661200003}"><sh:name>{PRINTERSETTINGS}</sh:name><sh:controlpanel name="Microsoft.DevicesAndPrinters"/></sh:task>
    <sh:task id="{D4F4D004-0D35-4CB6-A21F-BC1661200004}"><sh:name>{DEVICESPRINTERS}</sh:name><sh:controlpanel name="Microsoft.DevicesAndPrinters"/></sh:task>
    <category id="2"><sh:task idref="{D4F4D001-0D35-4CB6-A21F-BC1661200001}"/><sh:task idref="{D4F4D002-0D35-4CB6-A21F-BC1661200002}"/><sh:task idref="{D4F4D003-0D35-4CB6-A21F-BC1661200003}"/><sh:task idref="{D4F4D004-0D35-4CB6-A21F-BC1661200004}"/></category>
  </application>
  <application id="{67ca7650-96e6-4fdd-bb43-a8e774f73a57}">
    <sh:task id="{D4F4E001-0D35-4CB6-A21F-BC1661200001}"><sh:name>{CHOOSEHOMEGROUP}</sh:name><sh:command>explorer.exe shell:::{67ca7650-96e6-4fdd-bb43-a8e774f73a57}</sh:command></sh:task>
    <sh:task id="{D4F4E002-0D35-4CB6-A21F-BC1661200002}"><sh:name>{SHAREPRINTERS}</sh:name><sh:command>explorer.exe shell:::{67ca7650-96e6-4fdd-bb43-a8e774f73a57}</sh:command></sh:task>
    <category id="3"><sh:task idref="{D4F4E001-0D35-4CB6-A21F-BC1661200001}"/><sh:task idref="{D4F4E002-0D35-4CB6-A21F-BC1661200002}"/></category>
  </application>
{CATEGORY_TASK_LINKS_BLOCK}
{DISPLAY_APPLICATION_BLOCK}
{VIRTUAL_APPLET_TASKS_BLOCK}
</applications>
)xml";

    std::string taskList = kTaskListTemplate;
    auto replaceAll = [&taskList](const char* token, const char* value) {
        size_t position = 0;
        while ((position = taskList.find(token, position)) != std::string::npos) {
            taskList.replace(position, strlen(token), value);
            position += strlen(value);
        }
    };
    
    // Windows 7 Category Task Links
    if (g_settings.restoreWin7CategoryTaskLinks.load()) {
        replaceAll("{CATEGORY_TASK_LINKS_BLOCK}",
            "  <!-- System and Security (Category 5) -->\n"
            "  <application id=\"{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}\">\n"
            "    <sh:task id=\"{D4F4F001-0D35-4CB6-A21F-BC1661200001}\"><sh:name>{REVIEWSTATUS}</sh:name><sh:command>rundll32.exe shell32.dll,Control_RunDLL wscui.cpl</sh:command></sh:task>\n"
            "    <sh:task id=\"{D4F4F002-0D35-4CB6-A21F-BC1661200002}\"><sh:name>{BACKUPCOMPUTER}</sh:name><sh:command>sdclt.exe</sh:command></sh:task>\n"
            "    <sh:task id=\"{D4F4F003-0D35-4CB6-A21F-BC1661200003}\"><sh:name>{FINDFIXPROBLEMS}</sh:name><sh:controlpanel name=\"Microsoft.Troubleshooting\"/></sh:task>\n"
            "    <sh:task id=\"{D4F4F004-0D35-4CB6-A21F-BC1661200004}\"><sh:name>{CHECKFIREWALL}</sh:name><sh:controlpanel name=\"Microsoft.WindowsFirewall\"/></sh:task>\n"
            "    <category id=\"5\">\n"
            "       <sh:task idref=\"{D4F4F001-0D35-4CB6-A21F-BC1661200001}\"/>\n"
            "       <sh:task idref=\"{D4F4F002-0D35-4CB6-A21F-BC1661200002}\"/>\n"
            "       <sh:task idref=\"{D4F4F003-0D35-4CB6-A21F-BC1661200003}\"/>\n"
            "       <sh:task idref=\"{D4F4F004-0D35-4CB6-A21F-BC1661200004}\"/>\n"
            "    </category>\n"
            "  </application>\n"
            "  <!-- Programs (Category 8) -->\n"
            "  <application id=\"{7B81BE6A-CE2B-4C39-9987-2B3D27A6CF15}\">\n"
            "    <sh:task id=\"{D4F4F101-0D35-4CB6-A21F-BC1661200001}\"><sh:name>{UNINSTALLPROGRAM}</sh:name><sh:controlpanel name=\"Microsoft.ProgramsAndFeatures\"/></sh:task>\n"
            "    <sh:task id=\"{D4F4F102-0D35-4CB6-A21F-BC1661200002}\"><sh:name>{TURNWINDOWSFEATURES}</sh:name><sh:command>optionalfeatures.exe</sh:command></sh:task>\n"
            "    <category id=\"8\">\n"
            "       <sh:task idref=\"{D4F4F101-0D35-4CB6-A21F-BC1661200001}\"/>\n"
            "       <sh:task idref=\"{D4F4F102-0D35-4CB6-A21F-BC1661200002}\"/>\n"
            "    </category>\n"
            "  </application>\n"
            "  <!-- User Accounts (Category 9) -->\n"
            "  <application id=\"{60632754-C523-4B62-B45C-4172DA012619}\">\n"
            "    <sh:task id=\"{D4F4F201-0D35-4CB6-A21F-BC1661200001}\"><sh:name>{CHANGEACCTPICTURE}</sh:name><sh:controlpanel name=\"Microsoft.UserAccounts\"/></sh:task>\n"
            "    <sh:task id=\"{D4F4F202-0D35-4CB6-A21F-BC1661200002}\"><sh:name>{ADDREMOVEACCOUNTS}</sh:name><sh:command>netplwiz.exe</sh:command></sh:task>\n"
            "    <sh:task id=\"{D4F4F203-0D35-4CB6-A21F-BC1661200003}\"><sh:name>{SETPARENTALCONTROLS}</sh:name><sh:command>rundll32.exe shell32.dll,Control_RunDLL %SystemRoot%\\System32\\parentalcontrols.cpl</sh:command></sh:task>\n"
            "    <category id=\"9\">\n"
            "       <sh:task idref=\"{D4F4F201-0D35-4CB6-A21F-BC1661200001}\"/>\n"
            "       <sh:task idref=\"{D4F4F202-0D35-4CB6-A21F-BC1661200002}\"/>\n"
            "       <sh:task idref=\"{D4F4F203-0D35-4CB6-A21F-BC1661200003}\"/>\n"
            "    </category>\n"
            "  </application>\n"
            "  <!-- Clock, Language, and Region (Category 6) -->\n"
            "  <!-- No single real CLSID covers this whole category; the two tasks -->\n"
            "  <!-- below are attached to their actual owning applets instead. -->\n"
            "  <application id=\"{E2E7934B-DCE5-43C4-9576-7FE4F75E7480}\">\n"
            "    <sh:task id=\"{D4F4F301-0D35-4CB6-A21F-BC1661200001}\"><sh:name>{CHANGEDATETIME}</sh:name><sh:controlpanel name=\"Microsoft.DateAndTime\"/></sh:task>\n"
            "    <category id=\"6\">\n"
            "       <sh:task idref=\"{D4F4F301-0D35-4CB6-A21F-BC1661200001}\"/>\n"
            "    </category>\n"
            "  </application>\n"
            "  <application id=\"{62D8ED13-C9D0-4CE8-A914-47DD628FB1B0}\">\n"
            "    <sh:task id=\"{D4F4F302-0D35-4CB6-A21F-BC1661200002}\"><sh:name>{CHANGEINPUTMETHOD}</sh:name><sh:controlpanel name=\"Microsoft.RegionAndLanguage\" page=\"Input\"/></sh:task>\n"
            "    <category id=\"6\">\n"
            "       <sh:task idref=\"{D4F4F302-0D35-4CB6-A21F-BC1661200002}\"/>\n"
            "    </category>\n"
            "  </application>\n"
            "  <!-- Internet Options (Category 3) -->\n"
            "  <application id=\"Microsoft.InternetOptions\">\n"
            "    <sh:task id=\"{D4F4F501-0D35-4CB6-A21F-BC1661200001}\"><sh:name>{CHANGEHOMEPAGE}</sh:name><sh:command>rundll32.exe shell32.dll,Control_RunDLL inetcpl.cpl,,0</sh:command></sh:task>\n"
            "    <sh:task id=\"{D4F4F502-0D35-4CB6-A21F-BC1661200002}\"><sh:name>{MANAGEBROWSERADDONS}</sh:name><sh:command>rundll32.exe shell32.dll,Control_RunDLL inetcpl.cpl,,5</sh:command></sh:task>\n"
            "    <sh:task id=\"{D4F4F503-0D35-4CB6-A21F-BC1661200003}\"><sh:name>{DELETEBROWSINGHISTORY}</sh:name><sh:command>rundll32.exe shell32.dll,Control_RunDLL inetcpl.cpl,,0</sh:command></sh:task>\n"
            "    <category id=\"3\">\n"
            "       <sh:task idref=\"{D4F4F501-0D35-4CB6-A21F-BC1661200001}\"/>\n"
            "       <sh:task idref=\"{D4F4F502-0D35-4CB6-A21F-BC1661200002}\"/>\n"
            "       <sh:task idref=\"{D4F4F503-0D35-4CB6-A21F-BC1661200003}\"/>\n"
            "    </category>\n"
            "  </application>\n"
"  <!-- Ease of Access (Category 7) -->\n"
            "  <application id=\"{D555645E-D4F8-4C29-A827-D93C859C4F2A}\">\n"
            "    <sh:task id=\"{D4F4F401-0D35-4CB6-A21F-BC1661200001}\"><sh:name>{LETWINDOWSSUGGEST}</sh:name><sh:controlpanel name=\"Microsoft.EaseOfAccessCenter\"/></sh:task>\n"
            "    <category id=\"7\">\n"
            "       <sh:task idref=\"{D4F4F401-0D35-4CB6-A21F-BC1661200001}\"/>\n"
            "    </category>\n"
            "  </application>");
    } else {
        replaceAll("{CATEGORY_TASK_LINKS_BLOCK}", "");
    }

    // Classic Win7 "blue task links" for the two virtual applets. The block is
    // only emitted when the corresponding applet is both available on this
    // machine and enabled in settings, mirroring how GetNamespaceClsids()
    // gates the icon itself. Application ids are the virtual CLSIDs, so the
    // tasks attach to exactly the entries this mod injects.
    std::string virtualTaskBlock;
    if (g_settings.restoreClassicTaskLinks.load()) {
        if (g_injectBitlockerApplet.load()) {
            virtualTaskBlock +=
                "  <!-- BitLocker Drive Encryption (System and Security, Category 5) -->\n"
                "  <application id=\"{c62d8e9b-1f6a-4a6b-9a4c-8e6a7b2df301}\">\n"
                "    <sh:task id=\"{D4F4A010-0D35-4CB6-A21F-BC1661200010}\"><sh:name>{BITLOCKERMANAGE}</sh:name>"
                "<sh:keywords>bitlocker;encryption</sh:keywords>"
                "<sh:command>explorer.exe shell:::{D9EF8727-CAC2-4E60-809E-86F80A666C91}</sh:command></sh:task>\n"
                "    <category id=\"5\"><sh:task idref=\"{D4F4A010-0D35-4CB6-A21F-BC1661200010}\"/></category>\n"
                "  </application>\n";
        }
        if (g_injectTabletPcApplet.load()) {
            virtualTaskBlock +=
                "  <!-- Tablet PC Settings (Hardware and Sound, Category 2) -->\n"
                "  <application id=\"{f3a91d47-6b52-4c9e-9d0a-1c7e5f2b6a84}\">\n"
                "    <sh:task id=\"{D4F4A011-0D35-4CB6-A21F-BC1661200011}\"><sh:name>{TABLETCALIBRATE}</sh:name>"
                "<sh:keywords>tablet;calibrate;touch;pen</sh:keywords>"
                "<sh:command>explorer.exe shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}</sh:command></sh:task>\n"
                "    <sh:task id=\"{D4F4A012-0D35-4CB6-A21F-BC1661200012}\"><sh:name>{TABLETPENTOUCH}</sh:name>"
                "<sh:keywords>pen;touch;tablet</sh:keywords>"
                "<sh:command>explorer.exe shell:::{F82DF8F7-8B9F-442E-A48C-818EA735FF9B}</sh:command></sh:task>\n"
                "    <category id=\"2\"><sh:task idref=\"{D4F4A011-0D35-4CB6-A21F-BC1661200011}\"/>"
                "<sh:task idref=\"{D4F4A012-0D35-4CB6-A21F-BC1661200012}\"/></category>\n"
                "  </application>\n";
        }
    }
    replaceAll("{VIRTUAL_APPLET_TASKS_BLOCK}", virtualTaskBlock.c_str());

    if (g_settings.enableCategoryAppearanceLinks.load()) {
        replaceAll("{DISPLAY_APPLICATION_BLOCK}", 
            "  <application id=\"{c55584f4-7c7f-44f2-9a6d-913076f34c6a}\">\n"
            "    <sh:task id=\"{D4F4A006-0D35-4CB6-A21F-BC1661200006}\"><sh:name>{ADJUSTRESOLUTION}</sh:name><sh:keywords>resolution;screen;display;monitor</sh:keywords><sh:command>explorer.exe shell:::{C55584F4-7C7F-44f2-9A6D-913076F34C6A}</sh:command></sh:task>\n"
            "    <category id=\"1\">\n"
            "       <sh:task idref=\"{D4F4A006-0D35-4CB6-A21F-BC1661200006}\"/>\n"
            "    </category>\n"
            "  </application>\n"
            "  <application id=\"{ed834ed6-4b5a-4bfe-8f11-a626dcb6a921}\">\n"
            "    <sh:task id=\"{D4F4A001-0D35-4CB6-A21F-BC1661200001}\"><sh:name>{THEME}</sh:name><sh:keywords>theme;personalization</sh:keywords><sh:controlpanel name=\"Microsoft.Personalization\"/></sh:task>\n"
            "    <sh:task id=\"{D4F4A002-0D35-4CB6-A21F-BC1661200002}\"><sh:name>{BACKGROUND}</sh:name><sh:keywords>desktop;background;wallpaper</sh:keywords><sh:command>explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}\\pageWallpaper</sh:command></sh:task>\n"
            "    <category id=\"1\">\n"
            "       <sh:task idref=\"{D4F4A001-0D35-4CB6-A21F-BC1661200001}\"/>\n"
            "       <sh:task idref=\"{D4F4A002-0D35-4CB6-A21F-BC1661200002}\"/>\n"
            "    </category>\n"
            "  </application>");
    } else {
        replaceAll("{DISPLAY_APPLICATION_BLOCK}", "");
    }

    replaceAll("{THEME}", texts->theme);
    replaceAll("{BACKGROUND}", texts->desktopBackground);
    replaceAll("{COLORS}", texts->windowColors);
    replaceAll("{SOUNDS}", texts->soundEffects);
    replaceAll("{SCREENSAVER}", texts->screenSaver);
    replaceAll("{SYSTEMICONS}", texts->systemIcons);
    replaceAll("{RESTOREICONS}", texts->restoreDefaultIconBehaviors);
    replaceAll("{NETWORKSTATUS}", texts->networkStatus);
    replaceAll("{CONNECTNETWORK}", texts->connectNetwork);
    replaceAll("{VIEWCOMPUTERS}", texts->viewNetworkComputers);
    replaceAll("{ADDWIRELESS}", texts->addWirelessDevice);
    replaceAll("{ADDPRINTER}", texts->addPrinter);
    replaceAll("{DEFAULTPRINTERS}", texts->setDefaultPrinters);
    replaceAll("{PRINTERSETTINGS}", texts->changePrinterSettings);
    replaceAll("{DEVICESPRINTERS}", texts->viewDevicesPrinters);
    replaceAll("{CHOOSEHOMEGROUP}", texts->chooseHomeGroup);
    replaceAll("{SHAREPRINTERS}", texts->sharePrinters);
    replaceAll("{ADJUSTRESOLUTION}", texts->adjustScreenResolution);
    replaceAll("{REVIEWSTATUS}", texts->reviewComputerStatus);
    replaceAll("{BACKUPCOMPUTER}", texts->backUpComputer);
    replaceAll("{FINDFIXPROBLEMS}", texts->findAndFixProblems);
    replaceAll("{CHECKFIREWALL}", texts->checkFirewallStatus);
    replaceAll("{UNINSTALLPROGRAM}", texts->uninstallProgram);
    replaceAll("{TURNWINDOWSFEATURES}", texts->turnWindowsFeatures);
    replaceAll("{CHANGEACCTPICTURE}", texts->changeAccountPicture);
    replaceAll("{ADDREMOVEACCOUNTS}", texts->addRemoveAccounts);
    replaceAll("{SETPARENTALCONTROLS}", texts->setParentalControls);
    replaceAll("{CHANGEDATETIME}", texts->changeDateTime);
    replaceAll("{CHANGEINPUTMETHOD}", texts->changeInputMethod);
    replaceAll("{LETWINDOWSSUGGEST}", texts->letWindowsSuggest);
    replaceAll("{CHANGEHOMEPAGE}", texts->changeHomePage);
    replaceAll("{MANAGEBROWSERADDONS}", texts->manageBrowserAddons);
    replaceAll("{DELETEBROWSINGHISTORY}", texts->deleteBrowsingHistory);
    replaceAll("{BITLOCKERMANAGE}", texts->bitlockerManage);
    replaceAll("{TABLETCALIBRATE}", texts->tabletCalibrate);
    replaceAll("{TABLETPENTOUCH}", texts->tabletPenTouch);


    const std::wstring targetPath = g_classicTaskLinksFilePath;
    const std::wstring temporaryPath = targetPath + L".tmp." + std::to_wstring(GetCurrentProcessId());
    std::ofstream file(temporaryPath.c_str(), std::ios::binary | std::ios::trunc);
    if (!file) {
        g_classicTaskLinksFilePath.clear();
        return false;
    }

    file.write(taskList.data(), static_cast<std::streamsize>(taskList.size()));
    const bool wroteFile = file.good();
    file.close();
    if (!wroteFile || !MoveFileExW(temporaryPath.c_str(), targetPath.c_str(), MOVEFILE_REPLACE_EXISTING)) {
        DeleteFileW(temporaryPath.c_str());
        g_classicTaskLinksFilePath.clear();
        return false;
    }
    const bool ok = true;
    Wh_Log(L"XML atomically replaced: %s (size=%zu)",
        targetPath.c_str(), taskList.size());
    Wh_Log(L"Locale selected: %s", texts->locale);
    Wh_Log(L"CatTaskLinks=%d CatAppearance=%d",
        g_settings.restoreWin7CategoryTaskLinks.load(), g_settings.enableCategoryAppearanceLinks.load());
    return ok;
}

// Timestamp (GetTickCount64) of the last time the cached path was validated or
// a (re)generation was attempted. Used to throttle the filesystem check below.
// 0 is a sentinel meaning "never checked, or explicitly invalidated"; it always
// forces a check, which matters in the first few seconds after boot when
// GetTickCount64() is itself smaller than the recheck interval.
static std::atomic<ULONGLONG> g_taskLinksLastCheckTick{ 0 };
static constexpr ULONGLONG kTaskLinksRecheckIntervalMs = 5000;

// Accessor used by the registry hooks. Unlike GetClassicTaskLinksFilePath(),
// this one heals two failure modes instead of silently serving nothing:
//   * the initial write failed (path cleared, feature dead for the process);
//   * the XML was deleted under us while Explorer was running (Disk Cleanup,
//     Storage Sense, a temp cleaner), which would make every classic task link
//     disappear until a settings change or an Explorer restart.
// It runs on Explorer's registry hot path, so the filesystem is touched at most
// once every kTaskLinksRecheckIntervalMs; in between, the cached path is
// returned with no syscall at all. A missing path is always retried, but the
// same throttle keeps a permanently failing regeneration from being hammered.
std::wstring GetOrCreateClassicTaskLinksFilePath() {
    std::wstring cached = GetClassicTaskLinksFilePath();

    const ULONGLONG now = GetTickCount64();
    ULONGLONG last = g_taskLinksLastCheckTick.load(std::memory_order_relaxed);
    // Applies whether or not a path is cached. Skipping the throttle for an
    // empty path meant that when the XML could not be written at all (%TEMP%
    // not writable, a security product blocking it, disk full) every single
    // Control-Panel-related registry query rebuilt the multi-KB template and
    // re-attempted the file I/O, on Explorer's registry hot path.
    if (last != 0 && now - last < kTaskLinksRecheckIntervalMs)
        return cached;

    // Only one thread per interval performs the check; the others keep using
    // the cached value (or an empty string, retried on the next interval).
    if (!g_taskLinksLastCheckTick.compare_exchange_strong(last, now, std::memory_order_relaxed))
        return cached;

    if (!cached.empty()) {
        const DWORD attributes = GetFileAttributesW(cached.c_str());
        if (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY))
            return cached;
        Wh_Log(L"Task links file missing (%s); regenerating", cached.c_str());
    }

    // EnsureClassicTaskLinksFile() takes g_taskLinksMutex itself, so it must be
    // called without holding it here; it re-checks the cached path under the
    // lock, so a concurrent regeneration costs nothing but the lock.
    if (!EnsureClassicTaskLinksFile()) {
        Wh_Log(L"Task links file could not be (re)generated; will retry later");
        return std::wstring();
    }
    return GetClassicTaskLinksFilePath();
}

// Reads a tri-state applet setting. Unknown/missing values fall back to Auto,
// which is the documented default.
AppletMode ReadAppletMode(const wchar_t* settingName) {
    AppletMode mode = AppletMode::Auto;
    if (PCWSTR value = Wh_GetStringSetting(settingName)) {
        if (wcscmp(value, L"always") == 0)     mode = AppletMode::Always;
        else if (wcscmp(value, L"never") == 0) mode = AppletMode::Never;
        Wh_FreeStringSetting(value);
    }
    return mode;
}

void LoadSettings() {
    g_settings.enablePersonalization.store(Wh_GetIntSetting(L"enablePersonalization"));
    g_settings.enableNotificationIcons.store(Wh_GetIntSetting(L"enableNotificationIcons"));
    g_settings.enableNetworkConnections.store(Wh_GetIntSetting(L"enableNetworkConnections"));
    g_settings.enablePrintersAndFaxes.store(Wh_GetIntSetting(L"enablePrintersAndFaxes"));
    g_settings.enableHomeGroup.store(Wh_GetIntSetting(L"enableHomeGroup"));
    g_settings.bitLockerMode.store((int)ReadAppletMode(L"bitLockerMode"));
    g_settings.tabletPcMode.store((int)ReadAppletMode(L"tabletPcMode"));
    // The effective verdicts depend on both the (fixed) auto detection and the
    // (changeable) override, so they are refreshed on every settings load.
    g_injectBitlockerApplet.store(ResolveAppletInjection(
        (AppletMode)g_settings.bitLockerMode.load(), g_bitlockerAutoDetected.load(),
        g_bitlockerClsidRegistered.load(), L"BitLocker Drive Encryption"));
    g_injectTabletPcApplet.store(ResolveAppletInjection(
        (AppletMode)g_settings.tabletPcMode.load(), g_tabletPcAutoDetected.load(),
        g_tabletPcClsidRegistered.load(), L"Tablet PC Settings"));
    g_settings.enableCategoryAppearanceLinks.store(Wh_GetIntSetting(L"enableCategoryAppearanceLinks"));
    g_settings.suppressCompanySync.store(Wh_GetIntSetting(L"suppressCompanySync"));
    g_settings.suppressWindowsToGo.store(Wh_GetIntSetting(L"suppressWindowsToGo"));
    g_settings.suppressInfrared.store(Wh_GetIntSetting(L"suppressInfrared"));
    g_settings.suppressWorkFolders.store(Wh_GetIntSetting(L"suppressWorkFolders"));
    g_settings.restoreClassicTaskLinks.store(Wh_GetIntSetting(L"restoreClassicTaskLinks"));
    g_settings.restoreWin7CategoryTaskLinks.store(Wh_GetIntSetting(L"restoreWin7CategoryTaskLinks"));
}

void InitDisplayNames() {
    wchar_t buffer[256] = { 0 };
    HMODULE hTheme = LoadLibraryEx(L"themecpl.dll", nullptr, 
                                   LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hTheme) {
        if (LoadStringW(hTheme, 1, buffer, 256) && buffer[0])
            g_personalizationName = buffer;
        else
            g_personalizationName = L"Personalization";
        FreeLibrary(hTheme);
    } else {
        g_personalizationName = L"Personalization";
    }
    Wh_Log(L"Personalization display name: %s", g_personalizationName.c_str());
    
    // Pre-compute lowercase GUIDs
    g_personalizationGuidLower    = ToLower(kPersonalizationGuid);
    g_notificationIconsGuidLower  = ToLower(kNotificationIconsGuid);
    g_networkConnectionsGuidLower = ToLower(kNetworkConnectionsGuid);
    g_printersAndFaxesGuidLower   = ToLower(kPrintersAndFaxesGuid);
    g_homeGroupGuidLower          = ToLower(kHomeGroupGuid);
    g_displayGuidLower            = ToLower(kDisplayGuid);
    g_realPersonalizationGuidLower = ToLower(kRealPersonalizationGuid);
    g_suppressedGuidLower         = ToLower(kSuppressedGuid);
    g_windowsToGoGuidLower         = ToLower(kWindowsToGoGuid);
    g_infraredGuidLower            = ToLower(kInfraredGuid);
    g_workFoldersGuidLower          = ToLower(kWorkFoldersGuid);

    // Pre-compute path suffixes once so the registry hooks don't concatenate
    // "clsid\\" + guid (and friends) on every call.
    g_personalizationClsidSuffix       = L"clsid\\" + g_personalizationGuidLower;
    g_personalizationDefaultIconSuffix = g_personalizationClsidSuffix + L"\\defaulticon";
    g_personalizationShellSuffix       = g_personalizationClsidSuffix + L"\\shell";
    g_personalizationShellOpenSuffix   = g_personalizationShellSuffix + L"\\open";
    g_personalizationOpenCommandSuffix = g_personalizationShellOpenSuffix + L"\\command";
    g_personalizationNsSuffix          = L"controlpanel\\namespace\\" + g_personalizationGuidLower;

    g_realPersonalizationClsidSuffix = L"clsid\\" + g_realPersonalizationGuidLower;
    g_displayClsidSuffix             = L"clsid\\" + g_displayGuidLower;

    g_suppressedClsidSuffix  = L"clsid\\" + g_suppressedGuidLower;
    g_suppressedNsSuffix     = L"controlpanel\\namespace\\" + g_suppressedGuidLower;
    g_windowsToGoClsidSuffix = L"clsid\\" + g_windowsToGoGuidLower;
    g_windowsToGoNsSuffix    = L"controlpanel\\namespace\\" + g_windowsToGoGuidLower;
    g_infraredClsidSuffix    = L"clsid\\" + g_infraredGuidLower;
    g_infraredNsSuffix       = L"controlpanel\\namespace\\" + g_infraredGuidLower;
    g_workFoldersClsidSuffix = L"clsid\\" + g_workFoldersGuidLower;
    g_workFoldersNsSuffix    = L"controlpanel\\namespace\\" + g_workFoldersGuidLower;

    g_notificationIconsClsidSuffix  = L"clsid\\" + g_notificationIconsGuidLower;
    g_networkConnectionsClsidSuffix = L"clsid\\" + g_networkConnectionsGuidLower;
    g_printersAndFaxesClsidSuffix   = L"clsid\\" + g_printersAndFaxesGuidLower;
    g_homeGroupClsidSuffix          = L"clsid\\" + g_homeGroupGuidLower;

    g_virtualApplets.clear();
    // Built whenever the real applet exists, not only when it is currently
    // being injected: the entry's visibility is gated per query through
    // enabledSetting below, so flipping the Auto/Always/Never setting takes
    // effect without restarting Explorer.
    if (g_bitlockerClsidRegistered.load()) {
        // On Win10 19044 the CLSID key is a stub; fall back to the well-known
        // fvecpl.dll resources that carry the "BitLocker Drive Encryption"
        // localized name, icon and description (InfoTip, string id -2). The
        // "@dll,-id" references are resolved and localized by Explorer itself
        // for every installed UI language.
        if (!AddVirtualApplet(kBitLockerVirtualGuid, kBitLockerGuid, kCategorySystemSecurity,
                              &g_injectBitlockerApplet,
                              L"@%SystemRoot%\\System32\\fvecpl.dll,-1",
                              L"%SystemRoot%\\System32\\fvecpl.dll,-1",
                              L"@%SystemRoot%\\System32\\fvecpl.dll,-2"))
            Wh_Log(L"Could not read BitLocker's real name/icon; virtual entry not created");
    }
    if (g_tabletPcClsidRegistered.load()) {
        // Tablet PC Settings name/infotip/icon live in tabletpc.cpl as string
        // resources 10100 (name), 10102 (infotip) and icon group 10200.
        if (!AddVirtualApplet(kTabletPcVirtualGuid, kTabletPcSettingsGuid, kCategoryHardware,
                              &g_injectTabletPcApplet,
                              L"@%SystemRoot%\\System32\\tabletpc.cpl,-10100",
                              L"%SystemRoot%\\System32\\tabletpc.cpl,-10200",
                              L"@%SystemRoot%\\System32\\tabletpc.cpl,-10102"))
            Wh_Log(L"Could not read Tablet PC Settings' real name/icon; virtual entry not created");
    }
    Wh_Log(L"Virtual applets registered: %zu", g_virtualApplets.size());
}

// GetTrackedPath/TrackKey/UntrackKey/CreateFakeHandle/FreeFakeHandle now live
// as methods on g_keyTracker (see KeyTracker class above).


enum class VNode {
    None,
    ClsidRoot, DefaultIcon, Shell, ShellOpen, OpenCommand, NameSpaceEntry,
    ClsidRootCategoryOnly,
    Suppressed
};

enum class ItemKind { None, Personalization, CategoryOnly, Suppressed, RealCplTaskUrl, VirtualApplet };

struct ClassifyResult {
    VNode    node;
    ItemKind kind;
    DWORD    category;
    int      virtualIndex = -1;
};

bool IsSuppressedGuid(const std::wstring& guidLower) {
    return (g_settings.suppressCompanySync.load() && guidLower == g_suppressedGuidLower) ||
           (g_settings.suppressWindowsToGo.load() && guidLower == g_windowsToGoGuidLower) ||
           (g_settings.suppressInfrared.load() && guidLower == g_infraredGuidLower) ||
           (g_settings.suppressWorkFolders.load() && guidLower == g_workFoldersGuidLower);
}

bool HasActiveSuppression() {
    return g_settings.suppressCompanySync.load() ||
           g_settings.suppressWindowsToGo.load() ||
           g_settings.suppressInfrared.load() ||
           g_settings.suppressWorkFolders.load();
}

bool IsSuppressedNamespaceKey(const std::wstring& lower) {
    const size_t marker = lower.rfind(L"controlpanel\\namespace\\");
    if (marker == std::wstring::npos) return false;
    return IsSuppressedGuid(lower.substr(marker + wcslen(L"controlpanel\\namespace\\")));
}

bool IsSuppressedNamespaceEntry(LPCWSTR name) {
    return name && IsSuppressedGuid(ToLower(name));
}

// Real subkey count reported by the registry itself, via the (unhooked)
// RegQueryInfoKeyW. Used so injected virtual CLSIDs can be appended after
// the real entries instead of shifted in front of them, which would
// otherwise desync callers that size an enumeration loop from this count.
DWORD GetRealSubKeyCount(HKEY hKey) {
    DWORD count = 0;
    if (RegQueryInfoKeyW(hKey, nullptr, nullptr, nullptr, &count, nullptr, nullptr,
                          nullptr, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) {
        return 0;
    }
    return count;
}

ClassifyResult ClassifyPersonalizationVirtual(const std::wstring& lower) {
    if (EndsWith(lower, g_personalizationClsidSuffix))       return { VNode::ClsidRoot,     ItemKind::Personalization, 0 };
    if (EndsWith(lower, g_personalizationDefaultIconSuffix)) return { VNode::DefaultIcon,   ItemKind::Personalization, 0 };
    if (EndsWith(lower, g_personalizationShellSuffix))       return { VNode::Shell,          ItemKind::Personalization, 0 };
    if (EndsWith(lower, g_personalizationShellOpenSuffix))   return { VNode::ShellOpen,      ItemKind::Personalization, 0 };
    if (EndsWith(lower, g_personalizationOpenCommandSuffix)) return { VNode::OpenCommand,    ItemKind::Personalization, 0 };
    if (EndsWith(lower, g_personalizationNsSuffix))          return { VNode::NameSpaceEntry, ItemKind::Personalization, 0 };
    return { VNode::None, ItemKind::None, 0 };
}

ClassifyResult ClassifyVirtualApplets(const std::wstring& lower) {
    for (size_t i = 0; i < g_virtualApplets.size(); ++i) {
        const VirtualApplet& a = g_virtualApplets[i];
        if (!a.enabledSetting->load()) continue;
        if (EndsWith(lower, a.clsidSuffix))       return { VNode::ClsidRoot,     ItemKind::VirtualApplet, a.category, (int)i };
        if (EndsWith(lower, a.defaultIconSuffix)) return { VNode::DefaultIcon,   ItemKind::VirtualApplet, a.category, (int)i };
        if (EndsWith(lower, a.shellSuffix))       return { VNode::Shell,        ItemKind::VirtualApplet, a.category, (int)i };
        if (EndsWith(lower, a.shellOpenSuffix))   return { VNode::ShellOpen,    ItemKind::VirtualApplet, a.category, (int)i };
        if (EndsWith(lower, a.openCommandSuffix)) return { VNode::OpenCommand,  ItemKind::VirtualApplet, a.category, (int)i };
        if (EndsWith(lower, a.nsSuffix))          return { VNode::NameSpaceEntry, ItemKind::VirtualApplet, a.category, (int)i };
    }
    return { VNode::None, ItemKind::None, 0 };
}

ClassifyResult ClassifyPath(const std::wstring& path) {
    // Early out before allocating/copying a lowercase path. This function runs
    // for many unrelated registry calls in Explorer.
    if (!ContainsRelevantKeywordInsensitive(path))
        return { VNode::None, ItemKind::None, 0 };

    std::wstring lower = ToLower(path);

    struct { const std::wstring* guidLower; const std::wstring* clsidSuffix; const std::wstring* nsSuffix; } suppressibleGuids[] = {
        { &g_suppressedGuidLower,  &g_suppressedClsidSuffix,  &g_suppressedNsSuffix  },
        { &g_windowsToGoGuidLower, &g_windowsToGoClsidSuffix, &g_windowsToGoNsSuffix },
        { &g_infraredGuidLower,    &g_infraredClsidSuffix,    &g_infraredNsSuffix    },
        { &g_workFoldersGuidLower, &g_workFoldersClsidSuffix, &g_workFoldersNsSuffix },
    };
    for (auto& item : suppressibleGuids) {
        if ((EndsWith(lower, *item.clsidSuffix) || EndsWith(lower, *item.nsSuffix)) &&
            IsSuppressedGuid(*item.guidLower))
            return { VNode::Suppressed, ItemKind::Suppressed, 0 };
    }

    if (g_settings.enablePersonalization.load()) {
        auto cr = ClassifyPersonalizationVirtual(lower);
        if (cr.node != VNode::None) return cr;
    }

    if (!g_virtualApplets.empty()) {
        auto cr = ClassifyVirtualApplets(lower);
        if (cr.node != VNode::None) return cr;
    }

    if (g_settings.enableCategoryAppearanceLinks.load()) {
        if (EndsWith(lower, g_realPersonalizationClsidSuffix) ||
            EndsWith(lower, g_displayClsidSuffix)) {
            return { VNode::ClsidRoot, ItemKind::RealCplTaskUrl, 0 };
        }
    }
    // availability is nullptr for items that are always considered present
    // (they're stock CLSIDs on every supported build); it points at a
    // per-item atomic for items this mod only injects when Windows itself
    // still registers the real CLSID (HomeGroup, BitLocker, Tablet PC).
    struct { std::atomic<bool>* enabled; const std::wstring* clsidSuffix; DWORD cat; std::atomic<bool>* availability; } categoryItems[] = {
        { &g_settings.enableNotificationIcons,  &g_notificationIconsClsidSuffix,  0,                       nullptr },  // Keep it outside category view; search still exposes its tasks
        { &g_settings.enableNetworkConnections, &g_networkConnectionsClsidSuffix, kCategoryNetwork,        nullptr },
        { &g_settings.enablePrintersAndFaxes,   &g_printersAndFaxesClsidSuffix,   kCategoryHardware,       nullptr },
        { &g_settings.enableHomeGroup,          &g_homeGroupClsidSuffix,          kCategoryNetwork,        &g_homeGroupClsidAvailable },
    };
    for (auto& item : categoryItems) {
        if (!item.enabled->load()) continue;
        if (item.availability && !item.availability->load()) continue;
        if (EndsWith(lower, *item.clsidSuffix))
            return { VNode::ClsidRootCategoryOnly, ItemKind::CategoryOnly, item.cat };
    }

    return { VNode::None, ItemKind::None, 0 };
}

bool IsTargetKey(const std::wstring& path) {
    ClassifyResult cr = ClassifyPath(path);
    return cr.node != VNode::None && cr.kind != ItemKind::Suppressed;
}

bool IsNameSpaceParentKey(const std::wstring& path) {
    return EndsWith(ToLower(path), L"controlpanel\\namespace");
}

LSTATUS ProvideStringValue(LPBYTE lpData, LPDWORD lpcbData, const std::wstring& str) {
    // Sanity cap: every string this mod provides is a short, fixed constant
    // (a path, a display name, a shell::: command). A value this long can
    // only mean something upstream is wrong; refuse rather than propagate
    // an oversized buffer requirement to the caller.
    constexpr size_t kMaxReasonableChars = 4096;
    if (str.length() > kMaxReasonableChars) return ERROR_FILE_NOT_FOUND;

    DWORD requiredSize = (DWORD)((str.length() + 1) * sizeof(wchar_t));
    if (!lpcbData) return ERROR_INVALID_PARAMETER;
    if (!lpData) {
        *lpcbData = requiredSize;
        return ERROR_SUCCESS; // Standard two-call size-probe pattern
    }
    if (*lpcbData < requiredSize) {
        *lpcbData = requiredSize;
        return ERROR_MORE_DATA;
    }
    *lpcbData = requiredSize;
    memcpy(lpData, str.c_str(), requiredSize);
    return ERROR_SUCCESS;
}

LSTATUS ProvideDwordValue(LPBYTE lpData, LPDWORD lpcbData, DWORD value) {
    if (!lpcbData) return ERROR_INVALID_PARAMETER;
    if (!lpData) {
        *lpcbData = sizeof(DWORD);
        return ERROR_SUCCESS; // Standard two-call size-probe pattern
    }
    if (*lpcbData < sizeof(DWORD)) {
        *lpcbData = sizeof(DWORD);
        return ERROR_MORE_DATA;
    }
    *lpcbData = sizeof(DWORD);
    *(DWORD*)lpData = value;
    return ERROR_SUCCESS;
}

// restoreWin7CategoryTaskLinks and enableCategoryAppearanceLinks only affect
// the *content* of the generated task-links XML; the XML itself is only ever
// served when restoreClassicTaskLinks is on. Gate the four TasksFileUrl query
// sites below on this instead of on restoreClassicTaskLinks alone, so turning
// off just "Restore Classic Task Links" doesn't silently disable the other
// two settings as well.
static bool AnyTaskLinksEnabled() {
    return g_settings.restoreClassicTaskLinks.load() ||
           g_settings.restoreWin7CategoryTaskLinks.load() ||
           g_settings.enableCategoryAppearanceLinks.load();
}

bool TryProvideValue(const std::wstring& path, const std::wstring& valueName,
                     LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData, LSTATUS& outStatus) {
    ClassifyResult cr = ClassifyPath(path);
    if (cr.node == VNode::None) return false;

    if (cr.kind == ItemKind::Suppressed) {
        Wh_Log(L"Blocked CLSID query: %s", path.c_str());
        outStatus = ERROR_FILE_NOT_FOUND;
        return true;
    }

    if (cr.kind == ItemKind::RealCplTaskUrl) {
        Wh_Log(L"Providing value for: %s (value=%s)", path.c_str(), valueName.c_str());
        if (valueName == L"System.Software.TasksFileUrl" &&
            AnyTaskLinksEnabled()) {
            std::wstring taskLinksPath = GetOrCreateClassicTaskLinksFilePath();
            if (!taskLinksPath.empty()) {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, taskLinksPath);
                return true;
            }
        }
        if (valueName == L"System.ControlPanel.Category") {
            if (lpType) *lpType = REG_DWORD;
            outStatus = ProvideDwordValue(lpData, lpcbData, kCategoryAppearance);
            return true;
        }
        return false;
    }

    if (cr.kind == ItemKind::CategoryOnly) {
        Wh_Log(L"Providing value for: %s (value=%s, cat=%lu)", path.c_str(), valueName.c_str(), cr.category);
        if (valueName == L"System.ControlPanel.Category") {
            if (lpType) *lpType = REG_DWORD;
            outStatus = ProvideDwordValue(lpData, lpcbData, cr.category);
            return true;
        }
        // Category 0 is used only by the virtual Notification Area Icons applet.
        // Search uses the application name to associate task metadata with it.
        if (cr.category == 0 && valueName == L"System.ApplicationName") {
            if (lpType) *lpType = REG_SZ;
            outStatus = ProvideStringValue(lpData, lpcbData, L"Microsoft.NotificationAreaIcons");
            return true;
        }
        if (valueName == L"System.Software.TasksFileUrl" &&
            AnyTaskLinksEnabled()) {
            std::wstring taskLinksPath = GetOrCreateClassicTaskLinksFilePath();
            if (!taskLinksPath.empty()) {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, taskLinksPath);
                return true;
            }
        }
        return false;
    }

    if (cr.kind == ItemKind::Personalization) {
        Wh_Log(L"Providing value for: %s (value=%s, node=%d)", path.c_str(), valueName.c_str(), (int)cr.node);
        if (cr.node == VNode::NameSpaceEntry) {
            if (valueName.empty()) {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, g_personalizationName);
                return true;
            }
        } else if (cr.node == VNode::ClsidRoot) {
            if (valueName.empty()) {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, g_personalizationName);
                return true;
            } else if (valueName == L"InfoTip") {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, L"@%SystemRoot%\\System32\\themecpl.dll,-2#immutable1");
                return true;
            } else if (valueName == L"System.ApplicationName") {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, L"Microsoft.Personalization");
                return true;
            } else if (valueName == L"System.ControlPanel.Category") {
                if (lpType) *lpType = REG_DWORD;
                outStatus = ProvideDwordValue(lpData, lpcbData, kCategoryAppearance);
                return true;
            } else if (valueName == L"System.Software.TasksFileUrl") {
                if (lpType) *lpType = REG_SZ;
                std::wstring taskLinksPath = GetOrCreateClassicTaskLinksFilePath();
                std::wstring taskFileUrl =
                    (AnyTaskLinksEnabled() && !taskLinksPath.empty())
                        ? taskLinksPath
                        : std::wstring(L"Internal");
                outStatus = ProvideStringValue(lpData, lpcbData, taskFileUrl);
                return true;
            } else if (valueName == L"SortOrderIndex") {
                if (lpType) *lpType = REG_DWORD;
                outStatus = ProvideDwordValue(lpData, lpcbData, 1); // Place at the very top
                return true;
            }
        } else if (cr.node == VNode::DefaultIcon) {
            if (valueName.empty()) {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, L"%SystemRoot%\\System32\\themecpl.dll,-1");
                return true;
            }
        } else if (cr.node == VNode::OpenCommand) {
            if (valueName.empty()) {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, L"explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}");
                return true;
            }
        }
    }

    if (cr.kind == ItemKind::VirtualApplet) {
        if (cr.virtualIndex < 0 || (size_t)cr.virtualIndex >= g_virtualApplets.size()) return false;
        const VirtualApplet& a = g_virtualApplets[cr.virtualIndex];
        Wh_Log(L"Providing value for: %s (value=%s, node=%d, applet=%s)", path.c_str(), valueName.c_str(), (int)cr.node, a.displayName.c_str());

        if (cr.node == VNode::ClsidRoot || cr.node == VNode::NameSpaceEntry) {
            if (valueName.empty()) {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, a.displayName);
                return true;
            }
            // InfoTip (Win7-style description under the link). Served on BOTH
            // the CLSID root and the ControlPanel\\NameSpace entry because
            // Explorer's property store has been observed to query it from
            // either path. REG_SZ matches how real applets (e.g. Personalization)
            // expose it; the text is already localized by the time we reach here.
            if (valueName == L"InfoTip" && !a.infoTip.empty()) {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, a.infoTip);
                return true;
            }
            if (cr.node == VNode::ClsidRoot && valueName == L"System.ControlPanel.Category") {
                if (lpType) *lpType = REG_DWORD;
                outStatus = ProvideDwordValue(lpData, lpcbData, a.category);
                return true;
            }
            // Point the virtual applet at the generated tasks XML so Explorer
            // displays the classic blue task links beneath it (same mechanism
            // as Personalization).
            if (cr.node == VNode::ClsidRoot && valueName == L"System.Software.TasksFileUrl" &&
                AnyTaskLinksEnabled()) {
                std::wstring taskLinksPath = GetOrCreateClassicTaskLinksFilePath();
                if (!taskLinksPath.empty()) {
                    if (lpType) *lpType = REG_SZ;
                    outStatus = ProvideStringValue(lpData, lpcbData, taskLinksPath);
                    return true;
                }
            }
        } else if (cr.node == VNode::DefaultIcon) {
            if (valueName.empty() && !a.iconValue.empty()) {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, a.iconValue);
                return true;
            }
        } else if (cr.node == VNode::OpenCommand) {
            if (valueName.empty()) {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, a.openCommand);
                return true;
            }
        }
        return false;
    }
    return false;
}

// g_injectBitlockerApplet / g_injectTabletPcApplet only say the setting wants
// the applet; the applet's CLSID data (name, icon, etc.) is only actually
// built by AddVirtualApplet() when it found a name to use, and it may have
// failed to (see InitDisplayNames()). Enumerating the CLSID into
// ControlPanel\NameSpace without the backing entry existing gives Explorer a
// namespace item whose CLSID lookup falls through to the real registry and
// fails - a nameless/iconless Control Panel entry. Check that the applet was
// actually built before advertising it.
static bool VirtualAppletPresent(const std::wstring& guidLower) {
    for (const auto& a : g_virtualApplets)
        if (a.guidLower == guidLower && a.enabledSetting && a.enabledSetting->load()) return true;
    return false;
}

std::vector<std::wstring> GetNamespaceClsids() {
    std::vector<std::wstring> result;
    result.reserve(7);
    if (g_settings.enablePersonalization.load())    result.push_back(kPersonalizationGuid);
    if (g_settings.enableNotificationIcons.load())  result.push_back(kNotificationIconsGuid);
    if (g_settings.enableNetworkConnections.load()) result.push_back(kNetworkConnectionsGuid);
    if (g_settings.enablePrintersAndFaxes.load())   result.push_back(kPrintersAndFaxesGuid);
    if (IsHomeGroupAvailable())                     result.push_back(kHomeGroupGuid);
    if (g_injectBitlockerApplet.load() && VirtualAppletPresent(kBitLockerVirtualGuid))
        result.push_back(kBitLockerVirtualGuid);
    if (g_injectTabletPcApplet.load() && VirtualAppletPresent(kTabletPcVirtualGuid))
        result.push_back(kTabletPcVirtualGuid);
    return result;
}

bool GetVirtualSubKeyName(VNode node, DWORD index, std::wstring& outName) {
    switch (node) {
        case VNode::ClsidRoot:
            if (index == 0) { outName = L"DefaultIcon"; return true; }
            if (index == 1) { outName = L"Shell";       return true; }
            return false;
        case VNode::Shell:
            if (index == 0) { outName = L"Open"; return true; }
            return false;
        case VNode::ShellOpen:
            if (index == 0) { outName = L"command"; return true; }
            return false;
        default:
            return false;
    }
}

using RegOpenKeyExW_t = decltype(&RegOpenKeyExW);
RegOpenKeyExW_t RegOpenKeyExWOriginal;
LSTATUS WINAPI RegOpenKeyExWHook(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions,
                                 REGSAM samDesired, PHKEY phkResult) {
    std::wstring fullPath;
    if (g_keyTracker.IsFakeAndGetPath(hKey, fullPath)) {
        if (lpSubKey && *lpSubKey) {
            if (!fullPath.empty()) fullPath += L"\\";
            fullPath += lpSubKey;
        }
        if (IsTargetKey(fullPath)) {
            HKEY fake = g_keyTracker.CreateFake(fullPath);
            if (!fake) return ERROR_OUTOFMEMORY;
            Wh_Log(L"Fake handle (child): %s", fullPath.c_str());
            if (phkResult) *phkResult = fake;
            return ERROR_SUCCESS;
        }
        return ERROR_FILE_NOT_FOUND;
    }
    if (HasActiveSuppression() && lpSubKey) {
        // HasActiveSuppression() is true out of the box, so every
        // RegOpenKeyExW call in explorer.exe — not just shell32's — reaches
        // this branch. The lookup can't be skipped (the parent path is needed
        // both to decide relevance and to build the full path), but it now
        // takes the tracker's *shared* lock, so these calls no longer
        // serialize against each other; the string test still saves the
        // concatenation, the ToLower copy and the rfind for the vast majority.
        std::wstring basePath = g_keyTracker.GetPath(hKey);
        if (!basePath.empty() || ContainsRelevantKeywordInsensitive(lpSubKey)) {
            std::wstring fullPath = basePath;
            if (*lpSubKey) { if (!fullPath.empty()) fullPath += L"\\"; fullPath += lpSubKey; }
            if (IsSuppressedNamespaceKey(ToLower(fullPath))) {
                Wh_Log(L"Suppressed key: %s", fullPath.c_str());
                return ERROR_FILE_NOT_FOUND;
            }
        }
    }

    LSTATUS status = RegOpenKeyExWOriginal(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    if (status == ERROR_SUCCESS && phkResult && *phkResult) {
        // The real handle is already open at this point. Track() is the only
        // call in this branch that can throw (std::bad_alloc on map insert);
        // if it does, the handle is still returned to the caller as-is rather
        // than re-invoking RegOpenKeyExWOriginal, which would leak the handle
        // already obtained. Losing tracking for this one handle only means
        // this mod won't recognize it as "ours" on a later call.
        try {
            std::wstring basePath = g_keyTracker.GetPath(hKey);
            std::wstring fullPath = basePath;
            if (lpSubKey && *lpSubKey) { if (!fullPath.empty()) fullPath += L"\\"; fullPath += lpSubKey; }
            g_keyTracker.Track(*phkResult, fullPath);
        } catch (...) {
            Wh_Log(L"Exception tracking opened key; handle still returned untracked");
        }
    } else if (status == ERROR_FILE_NOT_FOUND && phkResult) {
        std::wstring basePath = g_keyTracker.GetPath(hKey);
        std::wstring fullPath = basePath;
        if (lpSubKey && *lpSubKey) { if (!fullPath.empty()) fullPath += L"\\"; fullPath += lpSubKey; }
        if (IsTargetKey(fullPath)) {
            HKEY fake = g_keyTracker.CreateFake(fullPath);
            if (!fake) return ERROR_OUTOFMEMORY;
            Wh_Log(L"Fake handle (not found): %s", fullPath.c_str());
            *phkResult = fake;
            return ERROR_SUCCESS;
        }
    }
    return status;
}

using RegCloseKey_t = decltype(&RegCloseKey);
RegCloseKey_t RegCloseKeyOriginal;
LSTATUS WINAPI RegCloseKeyHook(HKEY hKey) {
    // FreeFake reports whether the handle was ours, so this is one lock
    // acquisition instead of IsFake() followed by FreeFake().
    if (g_keyTracker.FreeFake(hKey)) return ERROR_SUCCESS;
    LSTATUS status = RegCloseKeyOriginal(hKey);
    g_keyTracker.Untrack(hKey);
    return status;
}

using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t RegQueryValueExWOriginal;
LSTATUS WINAPI RegQueryValueExWHook(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved,
                                    LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    // Unlike RegOpenKeyExW/RegCloseKey/ShellExecuteExW (whose blanket
    // try/catch was removed — see those hooks), this catch is kept
    // deliberately. Everything above the fallback call is a read: it only
    // writes into the caller-provided lpData/lpcbData output buffer, never
    // touches the registry or the filesystem. If TryProvideValue throws
    // partway through (e.g. std::bad_alloc building a std::wstring), the
    // fallback call below simply overwrites that same output buffer with the
    // real value — there's no external side effect to duplicate, so
    // re-invoking the original here is safe and prevents a C++ exception
    // from unwinding into shell32/Explorer, which isn't exception-aware.
    try {
        // One lookup instead of GetPath() + IsFake(): both answers come from
        // the same snapshot, and the shared lock is taken once per call.
        std::wstring path;
        const bool isFake = g_keyTracker.IsFakeAndGetPath(hKey, path);
        if (!path.empty()) {
            std::wstring valueName = lpValueName ? lpValueName : L"";
            LSTATUS outStatus;
            if (TryProvideValue(path, valueName, lpType, lpData, lpcbData, outStatus)) return outStatus;
        }

        if (isFake) return ERROR_FILE_NOT_FOUND;

        return RegQueryValueExWOriginal(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
    } catch (...) {
        Wh_Log(L"Exception caught, falling back to real API");
        return RegQueryValueExWOriginal(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
    }
}

using RegGetValueW_t = decltype(&RegGetValueW);
RegGetValueW_t RegGetValueWOriginal;
LSTATUS WINAPI RegGetValueWHook(HKEY hkey, LPCWSTR lpSubKey, LPCWSTR lpValue,
                                DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData) {
    // Same reasoning as RegQueryValueExWHook above: everything here only
    // writes into the caller's output buffer, so a fallback call after an
    // exception overwrites that buffer once with the real value instead of
    // duplicating any external side effect. Kept intentionally.
    try {
        // Single lookup for both "is it fake?" and "what's its path?".
        std::wstring path;
        const bool isFake = g_keyTracker.IsFakeAndGetPath(hkey, path);
        if (lpSubKey && *lpSubKey) { if (!path.empty()) path += L"\\"; path += lpSubKey; }
        if (!path.empty()) {
            std::wstring valueName = lpValue ? lpValue : L"";
            LSTATUS outStatus;
            if (TryProvideValue(path, valueName, pdwType, (LPBYTE)pvData, pcbData, outStatus)) return outStatus;
        }

        // Only bail out with FILE_NOT_FOUND after TryProvideValue has had a
        // chance to serve a virtual value on this fake handle.
        if (isFake) return ERROR_FILE_NOT_FOUND;

        return RegGetValueWOriginal(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
    } catch (...) {
        Wh_Log(L"Exception caught, falling back to real API");
        return RegGetValueWOriginal(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
    }
}

using RegEnumKeyExW_t = decltype(&RegEnumKeyExW);
RegEnumKeyExW_t RegEnumKeyExWOriginal;
LSTATUS WINAPI RegEnumKeyExWHook(HKEY hKey, DWORD dwIndex, LPWSTR lpName, LPDWORD lpcchName,
                                 LPDWORD lpReserved, LPWSTR lpClass, LPDWORD lpcchClass,
                                 PFILETIME lpftLastWriteTime) {
    // RegEnumKeyExW is not a cursor-based iterator with hidden progression —
    // dwIndex is an explicit caller-supplied parameter, and the API always
    // returns the same entry for the same (hKey, dwIndex) pair. The scan
    // loop below may call RegEnumKeyExWOriginal several times with various
    // indices while walking real entries, but each of those is a pure,
    // side-effect-free read; if an exception interrupts the loop, the
    // fallback call below re-issues RegEnumKeyExWOriginal with the dwIndex
    // the *caller* asked for — the same call that would have been made
    // without this mod's logic at all. Nothing is duplicated or corrupted,
    // so this catch is kept intentionally (unlike RegOpenKeyExW/RegCloseKey/
    // ShellExecuteExW, which have real external side effects to avoid
    // re-invoking).
    try {
        std::wstring fakePath;
        if (g_keyTracker.IsFakeAndGetPath(hKey, fakePath)) {
            ClassifyResult cr = ClassifyPath(fakePath);
            std::wstring subName;
            if (!GetVirtualSubKeyName(cr.node, dwIndex, subName)) return ERROR_NO_MORE_ITEMS;
            if (!lpcchName || !lpName) return ERROR_INVALID_PARAMETER;
            if (*lpcchName < subName.size() + 1) {
                *lpcchName = (DWORD)(subName.size() + 1);
                return ERROR_MORE_DATA;
            }
            wcscpy_s(lpName, *lpcchName, subName.c_str());
            *lpcchName = (DWORD)subName.size();
            if (lpftLastWriteTime) GetSystemTimeAsFileTime(lpftLastWriteTime);
            return ERROR_SUCCESS;
        }

        const std::wstring path = g_keyTracker.GetPath(hKey);
        if (!IsNameSpaceParentKey(path)) {
            return RegEnumKeyExWOriginal(hKey, dwIndex, lpName, lpcchName, lpReserved,
                                         lpClass, lpcchClass, lpftLastWriteTime);
        }

        // Real entries are enumerated first; the injected virtual CLSIDs are
        // appended only once the real entries are exhausted, so callers that
        // size a loop from RegQueryInfoKeyW's real subkey count (which we do
        // not hook) still see every real entry. Ordering of the virtual
        // items on screen comes from SortOrderIndex / the sort hook, not
        // from enumeration order.
        const std::vector<std::wstring> clsids = GetNamespaceClsids();

        auto ReturnVirtual = [&](DWORD virtualIndex) -> LSTATUS {
            if (virtualIndex >= clsids.size()) return ERROR_NO_MORE_ITEMS;
            if (!lpcchName || !lpName) return ERROR_INVALID_PARAMETER;
            const std::wstring& clsid = clsids[virtualIndex];
            if (*lpcchName < clsid.size() + 1) {
                *lpcchName = (DWORD)(clsid.size() + 1);
                return ERROR_MORE_DATA;
            }
            wcscpy_s(lpName, *lpcchName, clsid.c_str());
            *lpcchName = (DWORD)clsid.size();
            if (lpftLastWriteTime) GetSystemTimeAsFileTime(lpftLastWriteTime);
            return ERROR_SUCCESS;
        };

        if (!HasActiveSuppression()) {
            const LSTATUS status = RegEnumKeyExWOriginal(hKey, dwIndex, lpName, lpcchName, lpReserved,
                                                          lpClass, lpcchClass, lpftLastWriteTime);
            if (status != ERROR_NO_MORE_ITEMS) return status;
            const DWORD realCount = GetRealSubKeyCount(hKey);
            return ReturnVirtual(dwIndex >= realCount ? dwIndex - realCount : 0);
        }

        // Suppression active: scan real entries, skipping suppressed ones,
        // to map dwIndex onto the filtered real sequence. ControlPanel\
        // NameSpace entries are CLSIDs, so 256 wchar_t characters is more
        // than sufficient for the scan buffer.
        wchar_t scannedName[256];
        DWORD realIndex = 0;
        DWORD visibleRealIndex = 0;
        for (;;) {
            DWORD scannedCch = ARRAYSIZE(scannedName);
            const LSTATUS status = RegEnumKeyExWOriginal(
                hKey, realIndex, scannedName, &scannedCch,
                nullptr, nullptr, nullptr, nullptr);
            if (status != ERROR_SUCCESS) break; // real entries exhausted

            if (!IsSuppressedNamespaceEntry(scannedName)) {
                if (visibleRealIndex == dwIndex) {
                    // Repeat only the selected entry with caller-provided
                    // output buffers, preserving class and timestamp output.
                    return RegEnumKeyExWOriginal(hKey, realIndex, lpName, lpcchName, lpReserved,
                                                 lpClass, lpcchClass, lpftLastWriteTime);
                }
                ++visibleRealIndex;
            }
            ++realIndex;
        }

        return ReturnVirtual(dwIndex >= visibleRealIndex ? dwIndex - visibleRealIndex : 0);
    } catch (...) {
        Wh_Log(L"Exception caught, falling back to real API");
        return RegEnumKeyExWOriginal(hKey, dwIndex, lpName, lpcchName,
                                     lpReserved, lpClass, lpcchClass, lpftLastWriteTime);
    }
}

using RegEnumKeyW_t = decltype(&RegEnumKeyW);
RegEnumKeyW_t RegEnumKeyWOriginal;
LSTATUS WINAPI RegEnumKeyWHook(HKEY hKey, DWORD dwIndex, LPWSTR lpName, DWORD cchName) {
    // Same reasoning as RegEnumKeyExWHook above: dwIndex is an explicit,
    // caller-supplied parameter, not a hidden cursor, and every call here is
    // a pure read. Re-issuing the original in the catch after an exception
    // is the same call the caller would get without this mod, with no
    // external side effect duplicated — kept intentionally.
    try {
        std::wstring fakePath;
        if (g_keyTracker.IsFakeAndGetPath(hKey, fakePath)) {
            ClassifyResult cr = ClassifyPath(fakePath);
            std::wstring subName;
            if (!GetVirtualSubKeyName(cr.node, dwIndex, subName)) return ERROR_NO_MORE_ITEMS;
            if (!lpName) return ERROR_INVALID_PARAMETER;
            if (cchName <= subName.size()) return ERROR_MORE_DATA;
            wcscpy_s(lpName, cchName, subName.c_str());
            return ERROR_SUCCESS;
        }

        const std::wstring path = g_keyTracker.GetPath(hKey);
        if (!IsNameSpaceParentKey(path))
            return RegEnumKeyWOriginal(hKey, dwIndex, lpName, cchName);

        // See RegEnumKeyExWHook above: real entries first, virtual CLSIDs
        // appended after they're exhausted, so RegQueryInfoKeyW-based real
        // subkey counts stay accurate.
        const std::vector<std::wstring> clsids = GetNamespaceClsids();

        auto ReturnVirtual = [&](DWORD virtualIndex) -> LSTATUS {
            if (virtualIndex >= clsids.size()) return ERROR_NO_MORE_ITEMS;
            if (!lpName) return ERROR_INVALID_PARAMETER;
            const std::wstring& clsid = clsids[virtualIndex];
            if (cchName <= clsid.size()) return ERROR_MORE_DATA;
            wcscpy_s(lpName, cchName, clsid.c_str());
            return ERROR_SUCCESS;
        };

        if (!HasActiveSuppression()) {
            const LSTATUS status = RegEnumKeyWOriginal(hKey, dwIndex, lpName, cchName);
            if (status != ERROR_NO_MORE_ITEMS) return status;
            const DWORD realCount = GetRealSubKeyCount(hKey);
            return ReturnVirtual(dwIndex >= realCount ? dwIndex - realCount : 0);
        }

        wchar_t scannedName[256];
        DWORD realIndex = 0;
        DWORD visibleRealIndex = 0;
        for (;;) {
            const LSTATUS status = RegEnumKeyWOriginal(hKey, realIndex, scannedName,
                                                        ARRAYSIZE(scannedName));
            if (status != ERROR_SUCCESS) break; // real entries exhausted
            if (!IsSuppressedNamespaceEntry(scannedName)) {
                if (visibleRealIndex == dwIndex)
                    return RegEnumKeyWOriginal(hKey, realIndex, lpName, cchName);
                ++visibleRealIndex;
            }
            ++realIndex;
        }

        return ReturnVirtual(dwIndex >= visibleRealIndex ? dwIndex - visibleRealIndex : 0);
    } catch (...) {
        Wh_Log(L"Exception caught, falling back to real API");
        return RegEnumKeyWOriginal(hKey, dwIndex, lpName, cchName);
    }
}

// This hook only rewrites the two specific shell::: sub-page commands this
// mod itself writes into the task-links XML (\pageWallpaper and
// \pageColorization). Everything else — including shell::: commands from
// other mods or from Explorer itself — is passed straight through to the
// original API, since ShellExecuteExW is hooked process-wide and must not
// change behaviour for callers other than this mod's own links.
static const wchar_t* kOwnRedirectedCommands[] = {
    L"shell:::{ed834ed6-4b5a-4bfe-8f11-a626dcb6a921}\\pagewallpaper",
    L"shell:::{ed834ed6-4b5a-4bfe-8f11-a626dcb6a921}\\pagecolorization",
};

using ShellExecuteExW_t = BOOL(WINAPI*)(LPSHELLEXECUTEINFOW);
ShellExecuteExW_t ShellExecuteExWOriginal = nullptr;

BOOL WINAPI ShellExecuteExWHook(LPSHELLEXECUTEINFOW psei) {
    if (!psei || psei->cbSize < sizeof(SHELLEXECUTEINFOW))
        return ShellExecuteExWOriginal(psei);

    if (psei->lpFile) {
        std::wstring file = ToLower(psei->lpFile);
        if (file == L"explorer.exe" || file == L"explorer") {
            if (psei->lpParameters) {
                std::wstring params = psei->lpParameters;
                // Only match if parameters START with one of this mod's own
                // shell::: sub-page commands (after trimming) — never a
                // generic "shell:::" prefix, since that would also catch
                // commands issued by other mods or by Explorer itself.
                size_t firstNonSpace = params.find_first_not_of(L" \t");
                if (firstNonSpace != std::wstring::npos) {
                    std::wstring trimmed = params.substr(firstNonSpace);
                    std::wstring trimmedLower = ToLower(trimmed);
                    bool isOwnCommand = false;
                    for (const wchar_t* ownCmd : kOwnRedirectedCommands) {
                        if (trimmedLower.find(ownCmd) == 0) { isOwnCommand = true; break; }
                    }
                    if (isOwnCommand) {
                        std::wstring shellCommand = trimmed;
                        
                        std::wstring newFile;
                        std::wstring newParams;
                        
                        size_t spacePos = shellCommand.find(L' ');
                        if (spacePos != std::wstring::npos) {
                            newFile = shellCommand.substr(0, spacePos);
                            newParams = shellCommand.substr(spacePos + 1);
                            size_t ns = newParams.find_first_not_of(L" \t");
                            if (ns != std::wstring::npos)
                                newParams = newParams.substr(ns);
                            else
                                newParams.clear();
                        } else {
                            newFile = shellCommand;
                        }
                        
                        // Copy by cbSize, but never write past our local struct's
                        // own size even if the caller's cbSize is larger than
                        // what we know about (forward-compatibility safety net).
                        SHELLEXECUTEINFOW sei{};
                        memcpy(&sei, psei, (std::min)((size_t)psei->cbSize, sizeof(SHELLEXECUTEINFOW)));
                        sei.lpFile = newFile.c_str();
                        sei.lpParameters = newParams.empty() ? nullptr : newParams.c_str();
                        
                        Wh_Log(L"Redirected: %s %s -> %s %s",
                            psei->lpFile, psei->lpParameters ? psei->lpParameters : L"",
                            newFile.c_str(), newParams.empty() ? L"" : newParams.c_str());
                        
                        BOOL result = ShellExecuteExWOriginal(&sei);
                        // Propagate output fields back to caller
                        psei->hInstApp = sei.hInstApp;
                        if (psei->fMask & SEE_MASK_NOCLOSEPROCESS)
                            psei->hProcess = sei.hProcess;
                        return result;
                    }
                }
            }
        }
    }
    return ShellExecuteExWOriginal(psei);
}
/* Applets pulled to the front of their Control Panel category, in the order
   listed - credits to aubymori for the original ordering table. A Control
   Panel item belongs to a single category, so one flat list covers every
   category at once. */
LPCWSTR g_szAppletOrder[] = {
    /* Appearance and Personalization */
    L"::{580722ff-16a7-44c1-bf74-7e1acd00f4f9}", // Personalization (fake GUID)
};

// Control Panel item monikers appear both bare and with a leading "::", so
// comparisons skip the prefix on either side.
static LPCWSTR SkipMonikerPrefix(LPCWSTR lpszApplet) {
    return (lpszApplet[0] == L':' && lpszApplet[1] == L':') ? lpszApplet + 2
                                                            : lpszApplet;
}

// Position of an applet within g_szAppletOrder, or -1 if it isn't listed.
static int FindApplet(LPCWSTR lpszApplet) {
    if (!lpszApplet) return -1;
    LPCWSTR applet = SkipMonikerPrefix(lpszApplet);
    for (int i = 0; i < (int)ARRAYSIZE(g_szAppletOrder); i++) {
        if (0 == wcsicmp(SkipMonikerPrefix(g_szAppletOrder[i]), applet)) {
            return i;
        }
    }
    return -1;
}

// shell32 orders Control Panel applets by ranking each one against an array
// of CLSID monikers: a lower rank sorts first, and -1 ("not in the array")
// sorts last. s_SortAppletsInCategory is the comparator that drives the sort,
// and the ranking it performs may or may not live in its own function.
//
// Where shell32 keeps the ranking in s_FindAppletInSortArray, every value
// needed to override it arrives as a parameter, so nothing has to know how
// shell32 lays out its private structures. Whether that function exists is a
// per-build inlining decision, not a version progression: it is separate on
// 10.0.19041, 10.0.22621 and 10.0.26100, and inlined into the comparator on
// 10.0.17763 and 10.0.22000. Resolving the symbol therefore selects between
// the two paths below, rather than a Windows version check.
//
// The ranking function also serves Control Panel search results, which this
// mod has no business reordering, so the comparator hook doubles as a marker
// for the calls that come from a category sort.
int (WINAPI *CControlPanelAppletList_s_SortAppletsInCategory_orig)(void *, void *, LPARAM);
int (WINAPI *CControlPanelAppletList_s_FindAppletInSortArray_orig)(LPCWSTR, LPCWSTR const *, int);

static thread_local int g_categorySortDepth = 0;

class CategorySortScope {
public:
    CategorySortScope() { ++g_categorySortDepth; }
    ~CategorySortScope() { --g_categorySortDepth; }
    CategorySortScope(const CategorySortScope&) = delete;
    CategorySortScope& operator=(const CategorySortScope&) = delete;
};

int WINAPI CControlPanelAppletList_s_FindAppletInSortArray_hook(
    LPCWSTR lpszApplet, LPCWSTR const *ppszSortArray, int cSortArray
) {
    if (g_categorySortDepth == 0) {
        return CControlPanelAppletList_s_FindAppletInSortArray_orig(
            lpszApplet, ppszSortArray, cSortArray);
    }

    int iApplet = FindApplet(lpszApplet);
    if (iApplet >= 0) {
        return iApplet;
    }

    // Restored applets occupy ranks 0..N-1, so shell32's own ranks move down
    // by N. The shift is uniform, which leaves the stock applets in their
    // original relative order and moves only the restored ones, to the top of
    // their category.
    int iOrig = CControlPanelAppletList_s_FindAppletInSortArray_orig(
        lpszApplet, ppszSortArray, cSortArray);
    return iOrig >= 0 ? iOrig + (int)ARRAYSIZE(g_szAppletOrder) : -1;
}

// Everything below serves only the builds that inline the ranking. There the
// comparator gets two applet indices and the applet list, and the moniker has
// to be reached through shell32's private structures, at two offsets that the
// comparator itself spells out:
//
//     mov rcx, [r8+0x10]   the applet list's DPA, loaded for DPA_GetPtr
//     ...
//     add rsi, 0x208       the moniker within the item DPA_GetPtr returned
//
// Reading them back out of the instruction stream keeps the values tied to the
// shell32 that is actually loaded instead of the one the mod was written
// against. Both are 0x10 and 0x208 on 10.0.17763.1, 10.0.17763.9020 and
// 10.0.22000.3147, but nothing here depends on that staying true.
static size_t g_appletListDpaOffset = 0;
static size_t g_appletMonikerOffset = 0;

// Walks the comparator looking for the DPA load, which is the last register
// load before it calls DPA_GetPtr, and then for the moniker displacement,
// which is the first immediate added to a register afterwards. Returns false
// if the code doesn't have that shape, leaving applet ordering alone.
// Note: The moniker offset (0x208) has been found to be valid only on certain
// Windows 10 builds (e.g., 1809). On Windows 11 24H2+ and 26H1+, the offset
// has changed and the mod uses the fallback (stock applet ordering).
// A future update will address this once Windows 11 26H2 is officially released
// and the new offset can be determined with a proper disassembler.
static bool ResolveAppletOffsets(void* pFunc) {
    // lParam is the third x64 argument, i.e. r8 - anchor on it so an
    // unrelated load in the prologue can't be mistaken for the DPA load.
    const std::regex loadRegex(
        R"(mov r(?:[a-z]{2}|\d{1,2}), \[r8\+(0x[0-9a-f]+)\])",
        std::regex_constants::icase);
    const std::regex addRegex(
        R"(add r(?:[a-z]{2}|\d{1,2}), (0x[0-9a-f]+))",
        std::regex_constants::icase);

    size_t dpaOffset = 0;
    size_t monikerOffset = 0;
    bool pastCall = false;

    BYTE* p = (BYTE*)pFunc;
    for (int i = 0; i < 40; i++) {
        WH_DISASM_RESULT result;
        if (!Wh_Disasm(p, &result)) {
            break;
        }

        p += result.length;

        std::string_view s = result.text;
        if (s == "ret") {
            break;
        }

        std::match_results<std::string_view::const_iterator> match;
        if (!pastCall) {
            if (s.starts_with("call")) {
                pastCall = true;
            } else if (std::regex_match(s.begin(), s.end(), match, loadRegex)) {
                dpaOffset = std::stoull(match[1], nullptr, 16);
            }
        } else if (std::regex_match(s.begin(), s.end(), match, addRegex)) {
            monikerOffset = std::stoull(match[1], nullptr, 16);
            break;
        }
    }

    // Sanity check
    if (!dpaOffset || dpaOffset > 0x1000 ||
        !monikerOffset || monikerOffset > 0x10000) {
        return false;
    }

    g_appletListDpaOffset = dpaOffset;
    g_appletMonikerOffset = monikerOffset;
    return true;
}

// Bounded sanity check that a moniker pointer resolved from a computed
// offset actually looks like a CLSID moniker ("::{8-4-4-4-12}" or
// "{8-4-4-4-12}") before it's trusted. This is a plain bounded scan, not a
// per-character IsBadReadPtr/VirtualQuery probe (that pattern is TOCTOU-racy
// and gives false confidence rather than real safety) — its job is to catch
// a wrong offset match producing a plausible-looking pointer to the wrong
// field, not to substitute for memory-safety guarantees the disassembly
// match already provides.
static bool LooksLikeClsidMoniker(LPCWSTR s) {
    if (!s) return false;
    constexpr size_t kMaxLen = 64; // "::{"+36-char GUID+"}" = 41, plus slack
    size_t len = 0;
    while (len < kMaxLen && s[len] != L'\0') ++len;
    if (len >= kMaxLen || len < 4) return false;
    size_t start = (s[0] == L':' && s[1] == L':') ? 2 : 0;
    if (start + 2 >= len || s[start] != L'{') return false;
    return s[len - 1] == L'}';
}

static LPCWSTR GetAppletMoniker(HDPA hDpa, const int* pIndex) {
    LPVOID pItem = DPA_GetPtr(hDpa, *pIndex);
    if (!pItem) return NULL;
    LPCWSTR moniker = (LPCWSTR)((char*)pItem + g_appletMonikerOffset);
    return LooksLikeClsidMoniker(moniker) ? moniker : NULL;
}

int WINAPI CControlPanelAppletList_s_SortAppletsInCategory_hook(
    void *p1, void *p2, LPARAM lParam
) {
    // With a separate ranking function that hook does the reordering, and this
    // one only has to mark the sort as a category sort.
    if (CControlPanelAppletList_s_FindAppletInSortArray_orig) {
        CategorySortScope scope;
        return CControlPanelAppletList_s_SortAppletsInCategory_orig(p1, p2, lParam);
    }

    if (g_appletMonikerOffset && p1 && p2 && lParam) {
        HDPA hDpa = *(HDPA*)((BYTE*)lParam + g_appletListDpaOffset);
        if (hDpa) {
            int iApplet1 = FindApplet(GetAppletMoniker(hDpa, (const int*)p1));
            int iApplet2 = FindApplet(GetAppletMoniker(hDpa, (const int*)p2));
            if (iApplet1 >= 0 && iApplet2 >= 0) {
                return iApplet1 - iApplet2;
            } else if (iApplet1 >= 0) {
                return -1; // Move our custom applet to the top
            } else if (iApplet2 >= 0) {
                return 1;
            }
        }
    }

    return CControlPanelAppletList_s_SortAppletsInCategory_orig(p1, p2, lParam);
}


void* GetRegFunc(const char* name) {
    HMODULE hKb = GetModuleHandleW(L"kernelbase.dll");
    if (hKb) { void* p = (void*)GetProcAddress(hKb, name); if (p) return p; }
    HMODULE hAdv = GetModuleHandleW(L"advapi32.dll");
    if (!hAdv) hAdv = LoadLibraryW(L"advapi32.dll");
    if (hAdv) { void* p = (void*)GetProcAddress(hAdv, name); if (p) return p; }
    return nullptr;
}

void InvalidateClassicTaskLinksFile() {
    {
        std::lock_guard<std::mutex> lock(g_taskLinksMutex);
        g_classicTaskLinksFilePath.clear();
    }
    // Let the next hook-side lookup regenerate immediately instead of waiting
    // out the throttle interval in GetOrCreateClassicTaskLinksFilePath().
    g_taskLinksLastCheckTick.store(0, std::memory_order_relaxed);
}

void Wh_ModSettingsChanged() {
  try {
    // Saving settings is the user's way of saying "look again": discard the
    // cached shell verdicts so the next process start re-probes once. Cheap
    // (two registry writes) and only happens on an explicit user action.
    for (const wchar_t* key : { L"bitlocker", L"tabletpc" }) {
        Wh_DeleteValue(MakeVerdictValueName(key).c_str());
        Wh_DeleteValue(MakeVerdictBuildValueName(key).c_str());
    }
    LoadSettings();
    // Regenerate task links file with updated settings
    InvalidateClassicTaskLinksFile();
    EnsureClassicTaskLinksFile();
    Wh_Log(L"Changed - Pers=%d Notif=%d Net=%d Print=%d Home=%d BitLocker=%d TabletPC=%d CatApp=%d Company=%d ToGo=%d Infrared=%d Work=%d TaskLinks=%d CatTaskLinks=%d",
        g_settings.enablePersonalization.load(), g_settings.enableNotificationIcons.load(),
        g_settings.enableNetworkConnections.load(), g_settings.enablePrintersAndFaxes.load(),
        g_settings.enableHomeGroup.load(), g_injectBitlockerApplet.load(), g_injectTabletPcApplet.load(),
        g_settings.enableCategoryAppearanceLinks.load(),
        g_settings.suppressCompanySync.load(), g_settings.suppressWindowsToGo.load(),
        g_settings.suppressInfrared.load(), g_settings.suppressWorkFolders.load(), g_settings.restoreClassicTaskLinks.load(),
        g_settings.restoreWin7CategoryTaskLinks.load());
  } catch (...) {
      Wh_Log(L"Exception while applying changed settings");
  }
}

BOOL Wh_ModInit() {
  try {
    LoadSettings();

    DetectWindowsVersion();
    g_homeGroupClsidAvailable.store(IsRegisteredClsid(kHomeGroupGuid));
    Wh_Log(L"Legacy CLSID %s", g_homeGroupClsidAvailable.load() ? L"is registered; applet enabled when selected" : L"is absent; applet will not be injected");

    // Not just "is the CLSID registered": on machines where Windows already
    // shows these applets, the virtual entries would duplicate them. The
    // detection asks the shell and is cached here; LoadSettings() then applies
    // the user's Auto/Always/Never override on top of it. See the comment next
    // to g_bitlockerAutoDetected.
    g_bitlockerAutoDetected.store(DetectVirtualAppletNeededCached(
        kBitLockerGuid, kBitLockerCanonicalName, L"bitlocker",
        L"BitLocker Drive Encryption", g_bitlockerClsidRegistered));
    g_tabletPcAutoDetected.store(DetectVirtualAppletNeededCached(
        kTabletPcSettingsGuid, kTabletPcCanonicalName, L"tabletpc",
        L"Tablet PC Settings", g_tabletPcClsidRegistered));
    // Re-apply the overrides now that the auto verdicts are known (the earlier
    // LoadSettings() call ran before detection and resolved them against the
    // default-false cache).
    LoadSettings();

    // Build display names / virtual applets first: EnsureClassicTaskLinksFile()
    // generates the task links XML, and its virtual-applet task block depends
    // on knowing which virtual applets actually got built (see
    // VirtualAppletPresent()). InitDisplayNames() itself doesn't depend on the
    // XML, so this ordering doesn't cost anything.
    InitDisplayNames();

    // Generate task links file eagerly to avoid data races
    EnsureClassicTaskLinksFile();

    Wh_Log(L"=== Windows 7 Legacy Applet Restorer Init ===");
    Wh_Log(L"Windows build: %u", g_winBuild);
    Wh_Log(L"Pers=%d Notif=%d Net=%d Print=%d Home=%d BitLocker=%d TabletPC=%d CatApp=%d Suppress=%d TaskLinks=%d CatTaskLinks=%d",
        g_settings.enablePersonalization.load(), g_settings.enableNotificationIcons.load(),
        g_settings.enableNetworkConnections.load(), g_settings.enablePrintersAndFaxes.load(),
        g_settings.enableHomeGroup.load(), g_injectBitlockerApplet.load(), g_injectTabletPcApplet.load(),
        g_settings.enableCategoryAppearanceLinks.load(),
        g_settings.suppressCompanySync.load(), g_settings.restoreClassicTaskLinks.load(),
        g_settings.restoreWin7CategoryTaskLinks.load());

    void* pRegOpenKeyExW      = GetRegFunc("RegOpenKeyExW");
    void* pRegCloseKey        = GetRegFunc("RegCloseKey");
    void* pRegQueryValueExW   = GetRegFunc("RegQueryValueExW");
    void* pRegGetValueW       = GetRegFunc("RegGetValueW");
    void* pRegEnumKeyExW      = GetRegFunc("RegEnumKeyExW");
    void* pRegEnumKeyW        = GetRegFunc("RegEnumKeyW");

    if (!pRegOpenKeyExW || !pRegCloseKey || !pRegQueryValueExW ||
        !pRegGetValueW  || !pRegEnumKeyExW || !pRegEnumKeyW) {
        Wh_Log(L"Failed to get one or more registry functions");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook((RegOpenKeyExW_t)pRegOpenKeyExW,       RegOpenKeyExWHook,    &RegOpenKeyExWOriginal))    { Wh_Log(L"Failed to hook RegOpenKeyExW");    return FALSE; }
    if (!WindhawkUtils::SetFunctionHook((RegCloseKey_t)pRegCloseKey,           RegCloseKeyHook,      &RegCloseKeyOriginal))      { Wh_Log(L"Failed to hook RegCloseKey");      return FALSE; }
    if (!WindhawkUtils::SetFunctionHook((RegQueryValueExW_t)pRegQueryValueExW, RegQueryValueExWHook, &RegQueryValueExWOriginal)) { Wh_Log(L"Failed to hook RegQueryValueExW"); return FALSE; }
    if (!WindhawkUtils::SetFunctionHook((RegGetValueW_t)pRegGetValueW,         RegGetValueWHook,     &RegGetValueWOriginal))     { Wh_Log(L"Failed to hook RegGetValueW");     return FALSE; }
    if (!WindhawkUtils::SetFunctionHook((RegEnumKeyExW_t)pRegEnumKeyExW,       RegEnumKeyExWHook,    &RegEnumKeyExWOriginal))    { Wh_Log(L"Failed to hook RegEnumKeyExW");    return FALSE; }
    if (!WindhawkUtils::SetFunctionHook((RegEnumKeyW_t)pRegEnumKeyW,           RegEnumKeyWHook,      &RegEnumKeyWOriginal))      { Wh_Log(L"Failed to hook RegEnumKeyW");      return FALSE; }

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (hShell32) {
        void* pShellExecuteExW = (void*)GetProcAddress(hShell32, "ShellExecuteExW");
        if (pShellExecuteExW) {
            if (!WindhawkUtils::SetFunctionHook((ShellExecuteExW_t)pShellExecuteExW, ShellExecuteExWHook, &ShellExecuteExWOriginal)) {
                Wh_Log(L"Failed to hook ShellExecuteExW");
            }
        }

        // The ranking function is optional because whether it exists at all is
        // a per-build inlining decision. The comparator is not: it is either
        // the thing that scopes the ranking or, where the ranking is inlined,
        // the thing that does the reordering.
        void* pSortAppletsInCategory = nullptr;
        const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
            {
                {L"private: static int __cdecl CControlPanelAppletList::s_SortAppletsInCategory(int const *,int const *,__int64)"},
                &pSortAppletsInCategory,
                nullptr,  // Hooked manually below, we need the symbol address.
            },
            {
                {L"private: static int __cdecl CControlPanelAppletList::s_FindAppletInSortArray(unsigned short const *,unsigned short const * const *,int)"},
                (void**)&CControlPanelAppletList_s_FindAppletInSortArray_orig,
                (void*)CControlPanelAppletList_s_FindAppletInSortArray_hook,
                true
            },
        };

        if (!WindhawkUtils::HookSymbols(hShell32, shell32DllHooks, ARRAYSIZE(shell32DllHooks)) ||
            !pSortAppletsInCategory) {
            Wh_Log(L"Failed to hook the applet sorting functions; using stock applet ordering");
        } else {
            WindhawkUtils::SetFunctionHook(
                (decltype(CControlPanelAppletList_s_SortAppletsInCategory_orig))pSortAppletsInCategory,
                CControlPanelAppletList_s_SortAppletsInCategory_hook,
                &CControlPanelAppletList_s_SortAppletsInCategory_orig);

            if (CControlPanelAppletList_s_FindAppletInSortArray_orig) {
                Wh_Log(L"Applet ranking hooked OK");
            } else if (ResolveAppletOffsets(pSortAppletsInCategory)) {
                // Hooks are only applied once Wh_ModInit returns, so the
                // comparator's code is still unpatched at this point.
                Wh_Log(L"Applet ranking is inlined; DPA offset 0x%zX, moniker offset 0x%zX",
                    g_appletListDpaOffset, g_appletMonikerOffset);
            } else {
                Wh_Log(L"Applet ranking is inlined and its offsets could not be read; using stock applet ordering");
            }
        }

    }

    Wh_Log(L"All hooks set successfully");
    Wh_Log(L"Shell32 symbol hook: %s", hShell32 ? L"loaded" : L"FAILED");
    return TRUE;
  } catch (...) {
      Wh_Log(L"Exception during mod initialization, aborting load");
      return FALSE;
  }
}

static void CleanupTempFiles() {
    // Delete the temp task-links file
    std::lock_guard<std::mutex> lock(g_taskLinksMutex);
    if (!g_classicTaskLinksFilePath.empty()) {
        DeleteFileW(g_classicTaskLinksFilePath.c_str());
        Wh_Log(L"Deleted task links file: %s", g_classicTaskLinksFilePath.c_str());
    }
}

void Wh_ModUninit() {
    try {
        CleanupTempFiles();
        // See KeyTracker::ClearWithoutFreeing for why we deliberately don't
        // delete the fake-handle memory here.
        g_keyTracker.ClearWithoutFreeing();
        Wh_Log(L"Cleanup completed");
    } catch (...) {
        Wh_Log(L"Exception during cleanup, continuing anyway");
    }
}
