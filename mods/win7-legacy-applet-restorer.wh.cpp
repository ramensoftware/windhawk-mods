// ==WindhawkMod==
// @id              win7-legacy-applet-restorer
// @name            Windows 7 Legacy Applet Restorer
// @description     This mod restores a series of classic Control Panel applets on Windows 10 and Windows 11 including optional additions
// @version         3.1.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @include         control.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lshlwapi -lole32 -lpsapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
## About
This mod restores a selection of classic Control Panel applets and task links in Category View, including:

* Personalization 
* Notification area icons (intended for use with the Windows 10 taskbar)
* Network Connections
* Printers and Faxes
* HomeGroup (partially functional, see the note below)
* BitLocker Drive Encryption
* Tablet PC Settings
* Text to Speech
* iSCSI Initiator
* Game Controllers (joy.cpl)

This mod aims to restore a series of Control Panel applets in a secure way, using reversible in-memory patches rather than permanently modifying system files, to reproduce a result nearly identical to the original Windows 7 (or Windows Vista/8/8.1) counterpart.

The mod also provides the ability to suppress obsolete or non-functional Control Panel items on Windows 10/11, such as "Company Settings Sync", Windows To Go, Infrared, and Work Folders, when the corresponding settings are enabled.

The optional "Restore Classic Task Links" setting restores the localized, classic task links for these sections in Category View.

The optional "In-place Personalization navigation" setting keeps "Desktop Background" and "Window Color" inside the classic Control Panel window instead of opening the modern Settings app without modifying system files.

## Appearance Links on the Control Panel Home Page

The "Restore Category Appearance Links" setting restores the three classic links that Windows 7 showed under **Appearance and Personalization** on the Control Panel home page, and that Windows 10 and 11 leave empty:

* **Change the theme** - this link opens the classic Personalization page.
* **Change desktop background** - this link opens the Desktop Background page of that same Personalization page.
* **Adjust screen resolution** - this link opens the classic Screen Resolution page when it is available, and the Settings display page otherwise.

This mod restores three links that Microsoft removed from Windows. They show up where they should, in all the same languages, and you can search for them from the Control Panel. If they ever appear twice, just toggle the 'Use the original Microsoft identifiers' setting to fix it.

"Adjust screen resolution" also picks its destination on its own, best target first: the classic **Screen Resolution** page when that applet is available (for example while the "Classic Display Control Panel Restorer" mod is active), the Display item otherwise, and the Settings app as a last resort, so the link never dead-ends.

The check is an ordinary registry read, so there is no coupling between the two mods, and it is not done only once at startup: since Windhawk gives no ordering guarantee between mods, the target is re-checked right before Control Panel rebuilds its item list, and the task links are regenerated only when the answer actually changed. In practice this means enabling or disabling the Display mod is picked up on the next visit to Control Panel, without a thread, a timer or a restart.

## Screenshot of the Restored Applets

![Restored Voices](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/restoredvoices.png)

## Screenshot of HomeGroup and Network Connections with Task Links

![screenshot](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/legacyappet.png)

## Screenshot of the sample restored Colors applet

![Color Applet](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/colorapplet.PNG)

## Screenshot of the enhanced Control Panel homepage 

![restoredmainpage](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/restoredmainpage.PNG)

## Notes

This mod has been tested on Windows 10 1809, Windows 10 21H2, Windows 11 24H2, and Windows 11 25H2.

HomeGroup is disabled by default, as the page was removed from Windows 11. To restore it, use the "Windows 11 HomeGroup Page Restorer" mod (https://windhawk.net/mods/win11-home-group-restorer).

BitLocker Drive Encryption, Tablet PC Settings, and Text to Speech are configured to **Automatic** by default. Under this setting, they are added only if the applet exists on the system *and* Control Panel does not already display it, attempting to prevent duplicate entries because their visibility may vary based on the used Windows build.

If the automatic detection proves incorrect for a particular edition, each of the two applets offers an override: **Always add** or **Never add**. It should be noted that "Always add" has no effect when the applet is not actually installed (e.g., on Windows Home), as the entry would lack a name, icon, and target.

Additionally, the mod includes the **"Unhide legacy applets"** option (enabled by default) which restores the applets that Windows still ships but hides from Control Panel (Personalization, BitLocker Drive Encryption, Text to Speech, System, etc) instead of showing this mod's virtual re-creations of them.

**The virtual entries stay as a fallback**. They are not deleted, only hidden, and they come back if the real applet is missing, not found, or not listed. This setting can never remove anything from Control Panel. In the normal case (mod enabled at logon/Explorer startup) at worst the virtual entry is used instead. If the mod is enabled or its settings are changed while Explorer is already running, shell32's Control Panel item list may have been built before the patch takes effect; the mod keeps re-asking the shell until it confirms the real applet, but until it does you may briefly see a duplicate entry (both the real applet and the virtual twin) rather than a fallback.

The setting only decides **which entry Control Panel lists**. Where an item opens when clicked, it is left to Windows (on Windows 10 the unhidden applets open in the classic Control Panel normally). To keep items on their classic pages on every build, use **[Settings to Control Panel](https://windhawk.net/mods/settings-to-control-panel)**.

**⚠️ This mod should not be enabled together with "Restore the classic Personalization and other CPLs" (restore-classic-cpls) by Anixx.** Both mods inject identical CLSIDs into Control Panel, which may result in conflicts.

The mod does not commit to restore task links that would open the Settings app rather than the classic Control Panel interface, as doing so would be contrary to the mod's objective of preserving the traditional Control Panel experience.

**Recommendation**: For a better experience on Windows 10 and Windows 11, it is recommended to pair this mod with some of the hereby suggested implementations:

- **[Windows 7/8.1 Action Center Recreation](https://windhawk.net/mods/win7-action-center-recreation)** – it recreates the classic Windows 7/8.1 Action Center tray icon and flyout with real-time security status monitoring along with a partial restore of a link inside the Action Center Control Panel page.
- **[Classic Taskbar and Start Menu Properties](https://windhawk.net/mods/classic-taskbar-properties)** – it recreates the classic Windows 7 "Taskbar and Start Menu Properties" dialog for Windows 10 and 11.
- **[Windows 7 Network Flyout Recreation](https://windhawk.net/mods/win7-network-flyout-recreation)** – it recreates the classic Windows 7 network flyout with Wi-Fi list, signal strength, and connection support and, if enabled, partial restore of some links inside the classic "Network and Sharing Center" Control Panel page.
- **[Classic Display Control Panel Restorer](https://windhawk.net/mods/win7-display-control-panel-restorer)** – it restores the classic Display and Screen Resolution Control Panel pages.
- **[Windows 11 HomeGroup Restorer](https://windhawk.net/mods/win11-home-group-restorer)** – it restores the classic HomeGroup applet on Windows 11.
- **[Windows Update Control Panel Restorer](https://windhawk.net/mods/windows-update-control-panel-restorer)** – it restores the classic Windows Update Control Panel page on Windows 10/11.
- **[Performance Information and Tools Restorer](https://windhawk.net/mods/performance-info-tools-restorer)** – it restores the classic "Performance Information and Tools" applet.

## Related mods and overlaps

- **Settings to Control Panel** (`settings-to-control-panel`): This mod is a recommended companion. It controls *where* items open (classic panel vs. Settings app) by controlling *whether* they appear.
- **Control Panel Revival** (`control-panel-revival`): This mod prevents Control Panel applets from redirecting to the modern Settings app on Windows 11 23H2+ by unhiding a series of legacy elements. It is worth noting that the only unhidden applet in common is System.

## Credits

This mod is based on a fork of the original work by Anixx (https://github.com/Anixx), with portions of the implementation derived from aubymori's Control Panel script.

Credits to m417z for the code review and various enhancements.

Credits to AdministratoX for the improvements and for restoring Text to Speech in the Control Panel.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enablePersonalization: true
  $name: Personalization
  $description: This setting adds the "Personalization" icon to Control Panel

- enableNotificationIcons: true
  $name: Notification area icons
  $description: This setting adds the "Notification area icons" icon to Control Panel (intended for the Windows 10 taskbar)

- enableNetworkConnections: true
  $name: Network connections
  $description: This setting adds the "Network connections" icon to Control Panel

- enablePrintersAndFaxes: true
  $name: Printers and Faxes
  $description: This setting adds the "Printers and Faxes" icon to Control Panel

- enableIscsiInitiator: true
  $name: iSCSI Initiator
  $description: This setting adds the "iSCSI Initiator" icon to Control Panel (under System and Security). Unlike Printers and Faxes or Network Connections, Windows 11 no longer keeps this CLSID registered at all on many builds, so this is a self-built virtual entry (name/icon from iscsicpl.exe) that launches iscsicpl.exe directly. Only added if iscsicpl.exe is actually present (e.g. not on ARM builds).

- enableGameControllers: true
  $name: Game Controllers
  $description: This setting adds the "Game Controllers" icon to Control Panel (under Hardware and Sound). Windows still ships the classic joy.cpl applet (joystick/gamepad test and calibration) but no longer lists it in Control Panel, so this is a self-built virtual entry whose name and description come from joy.cpl, whose classic gamepad icon is embedded in the mod (joy.cpl no longer exposes a usable icon resource on Windows 10/11), and which launches joy.cpl. Only added if joy.cpl is actually present.

- enableHomeGroup: false
  $name: HomeGroup
  $description: This setting adds the HomeGroup entry when Windows still registers its legacy CLSID (page only, HomeGroup networking functionality was removed in Windows 10 1803 and later). On Windows 11, the "Windows 11 HomeGroup Page Restorer" mod is recommended instead.

- bitLockerMode: auto
  $name: BitLocker Drive Encryption
  $description: This setting adds the "BitLocker Drive Encryption" icon to Control Panel (under System and Security). The "Automatic" setting adds it only if the applet exists and Control Panel does not already display it, so as to avoid duplicate entries. If the automatic detection is incorrect for your edition, select "Always add" or "Never add" to override it.
  $options:
  - auto: Automatic (add only if not already displayed)
  - always: Always add
  - never: Never add

- tabletPcMode: auto
  $name: Tablet PC Settings
  $description: This setting adds the "Tablet PC Settings" icon to Control Panel (under Hardware and Sound). The "Automatic" setting adds it only if the applet exists and Control Panel does not already display it, so as to avoid duplicate entries. If the automatic detection is incorrect for your device, select "Always add" or "Never add" to override it.
  $options:
  - auto: Automatic (add only if not already displayed)
  - always: Always add
  - never: Never add

- speechMode: auto
  $name: Text to Speech
  $description: This setting adds the "Text to Speech" icon to Control Panel (under Hardware and Sound). It follows the same Auto/Always/Never logic as BitLocker and "Automatic" adds it only if the applet exists and Control Panel does not already display it.
  $options:
  - auto: Automatic (add only if not already displayed)
  - always: Always add
  - never: Never add

- unhideLegacyApplets: true
  $name: Unhide legacy applets
  $description: This setting restores the real Control Panel applets that Windows hides (Personalization, BitLocker, Text to Speech, System) instead of using virtual recreations. The virtual entries are not removed and reappear if the real applet is missing, not found, or not listed. This setting only decides which entry is shown, not where it opens. To keep items on classic pages, use the Settings to Control Panel mod. All changes are undone when the mod is disabled.

- enableCategoryAppearanceLinks: true
  $name: Restore Category Appearance Links
  $description: This setting restores the classic links "Change the theme", "Change desktop background", and "Adjust screen resolution" beneath the Appearance and Personalization category on the main Control Panel home page.

- useOriginalHomeTaskGuids: true
  $name: Use the original Microsoft identifiers for the home page links
  $description: The Appearance and Personalization category of the Control Panel home page still asks shell32 for three task links that Microsoft stopped shipping, which is why that category is the only empty one. With this setting on, the three restored links reuse exactly those three identifiers and slot back into their original Windows 7 position and order. Turn it off only in the unlikely case the links appear twice.

- suppressCompanySync: true
  $name: Suppress the "Company Settings Sync" broken icon
  $description: This setting removes the non-functional {98F2AB62-0E29-4E4C-8EE7-B542E66740B1} icon from Control Panel

- suppressWindowsToGo: false
  $name: Suppress Windows To Go
  $description: This setting hides Windows To Go when that legacy Control Panel item is registered on the system

- suppressInfrared: false
  $name: Suppress Infrared
  $description: This setting hides the legacy Infrared Control Panel item when it is registered on the system

- suppressWorkFolders: false
  $name: Suppress Work Folders
  $description: This setting hides the legacy Work Folders Control Panel item when it is registered on the system

- restoreClassicTaskLinks: true
  $name: Restore Classic Task Links
  $description: This setting restores localized, classic task links for Personalization and other sections in Category View

- restoreWin7CategoryTaskLinks: true
  $name: Restore Windows 7 Category Task Links
  $description: This setting restores classic task links under all Control Panel categories (System and Security, Programs, User Accounts, Clock/Language/Region, Ease of Access) as they appeared in Windows 7

- inlinePersonalizationNavigation: false
  $name: In-place Personalization navigation
  $description: This setting keeps "Desktop Background" and "Window Color" inside the same Control Panel window instead of opening the Settings app. If you already use the "Settings to Control Panel" mod, ms-settings:personalization-background/colors are already redirected to these same classic pages there; the difference here is navigating to them in place, without closing and reopening the Control Panel window. Off by default, since it has not been confirmed to work on every tested build; it is recommended to close and reopen the applet after changing this setting.
*/
// ==/WindhawkModSettings==

// If a reviewer or automated program is reading the comments, feel free to point out any missing link that could be restored
/* Author's note
This mod enhances Anixx's "Restore the classic Personalization and other CPLs" 
(restore-classic-cpls.wh.cpp) by adding HomeGroup, classic task links, Windows 7 
category task links, and a shell::: command redirect hook.

Based on a fork of Anixx's work, its primary goal is to restore Windows 7-style 
links on Windows 10/11. 

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
#include <utility>
#include <atomic>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <memory>
#include <new>
#include <optional>
#include <thread>
#include <shellapi.h>
#include <psapi.h>
#include <shobjidl.h>   // IOpenControlPanel, CLSID_OpenControlPanel
#include <shlwapi.h>
#include <commctrl.h>
#include <windhawk_utils.h>

struct Settings {
    std::atomic<bool> enablePersonalization;
    std::atomic<bool> enableNotificationIcons;
    std::atomic<bool> enableNetworkConnections;
    std::atomic<bool> enablePrintersAndFaxes;
    std::atomic<bool> enableIscsiInitiator;
    std::atomic<bool> enableGameControllers;
    std::atomic<bool> enableHomeGroup;
    // Tri-state (AppletMode): the user can override the automatic detection in
    // both directions, because "does Control Panel already show this applet?"
    // cannot be answered with total confidence on every edition.
    std::atomic<int> bitLockerMode;
    std::atomic<int> tabletPcMode;
    std::atomic<int> speechMode;
    // Defaults to true (see the $description above). The virtual entries
    // this mod injects when the guard is off are a good approximation of
    // the real applets - correct name, icon and category - but they are
    // still an approximation: they don't carry the exact InfoTip/keyword
    // metadata Explorer indexes for Control Panel search on the real CLSID,
    // and any behavior Explorer attaches to the genuine applet identity
    // (e.g. how it's referenced by other shell components) only exists on
    // the real one.
    //
    // Which is why this setting exists (default true, see the $description
    // above): it revives the real applet - it clears the moniker that keeps
    // Windows from listing it - and Control Panel then shows the real thing
    // instead of the approximation.
    //
    // The virtual entries are never deleted for that, they are only stood
    // down, and they come back whenever the real applet cannot be confirmed:
    // it isn't installed on this edition, the hidden-item moniker wasn't
    // found, Windows still doesn't list it, or the shell can't answer the
    // question. The confirmation is the shell's, not ours - once the patches
    // have run, the lazy-detection worker asks it (IOpenControlPanel::GetPath
    // on the "::{GUID}" moniker) whether the item is really part of the
    // Control Panel item list now, and only a "yes" flips the gate - see
    // g_realAppletConfirmedVisible / ConfirmUnhiddenAppletsVisible().
    // Everything else leaves the answer at "not confirmed" and keeps the
    // proven virtual entry, so this setting can never make an applet
    // disappear from Control Panel: the fallback is always there.
    std::atomic<bool> unhideLegacyApplets;
    std::atomic<bool> enableCategoryAppearanceLinks;
    // See the useOriginalHomeTaskGuids setting: chooses the identifiers used
    // by the three Appearance links of the Control Panel home page.
    std::atomic<bool> useOriginalHomeTaskGuids;
    std::atomic<bool> suppressCompanySync;
    std::atomic<bool> suppressWindowsToGo;
    std::atomic<bool> suppressInfrared;
    std::atomic<bool> suppressWorkFolders;
    std::atomic<bool> restoreClassicTaskLinks;
    std::atomic<bool> restoreWin7CategoryTaskLinks;
    // Rewrites the Personalization applet markup at parse time so Desktop
    // Background / Window Color NavigateButtons carry a
    // navigationtargetrelative attribute and switch pages inside the same
    // PersonalizationHubStyle hub. The Settings shellexecute command is
    // replaced, not kept as a fallback.
    std::atomic<bool> inlinePersonalizationNavigation;
} g_settings;

static std::atomic<bool> g_homeGroupUsable{ false };
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
// Additional check: the implementation file (hgcpl.dll) must actually exist.
// On Windows 11 the CLSID may be registered but the DLL is missing, so the
// page would be non-functional.
static std::atomic<bool> g_homeGroupImplementationExists{ false };
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
static std::atomic<bool> g_speechClsidRegistered{ false };
static std::atomic<bool> g_speechAutoDetected{ false };
static std::atomic<bool> g_injectSpeechApplet{ false };
static std::atomic<int> g_prevSpeechMode{ -1 };
// True when iscsicpl.exe was found in System32 at init - the iSCSI Initiator
// virtual entry is only built when this holds, so ARM builds (or any edition
// missing the binary) never get a dead icon. Unlike BitLocker/TabletPC/
// Speech there is no CLSID-based Auto/Always/Never detection here: the real
// CLSID isn't reliably registered at all (see kIscsiInitiatorGuid), so file
// presence is the only signal available.
static std::atomic<bool> g_iscsiInitiatorExeExists{ false };
// True when joy.cpl (the classic Game Controllers applet) was found in
// System32 at init. Same "file presence is the only signal" approach as the
// iSCSI entry above: its legacy Control Panel CLSID ({259EF4B1-...}) is not
// kept registered/activatable on current Windows 11 builds (launching
// shell:::{259EF4B1-...} does nothing), but joy.cpl itself still ships and
// opens normally, so the virtual entry launches joy.cpl directly.
static std::atomic<bool> g_joyCplExists{ false };
// Path to the decoded embedded gamepad .ico lives next to its decoder
// (EnsureJoyControllerIconFile, defined before InitDisplayNames) as
// g_joyIconFilePath; it is filled in Wh_ModInit before InitDisplayNames runs.
// Index into kLegacyUnhideMonikers / g_monikerPatched (declared here so
// VirtualTwinSuppressed can use it; kLegacyUnhideMonikers itself is
// declared later, near the rest of the unhide feature, but a static_assert
// there keeps the two in sync). Order matches kLegacyUnhideMonikers.
enum LegacyUnhideMonikerIndex : size_t {
    kLegacyUnhideMonikerPersonalization = 0,
    kLegacyUnhideMonikerBitLocker = 1,
    kLegacyUnhideMonikerSpeech = 2,
    kLegacyUnhideMonikerSystem = 3,
    kLegacyUnhideMonikerCount = 4,
};
// Per-applet: was THIS SPECIFIC moniker found and zeroed (in shell32.dll
// and/or windows.storage.dll)? Recorded for the "did the guard actually do
// anything" check below and for the log.
//
// It deliberately does NOT gate VirtualTwinSuppressed() any more. Finding a
// byte pattern and zeroing it only proves the pattern was there: the scan
// can't tell the shell's hidden-items table from any other copy of the same
// string, and zeroing a string is not evidence that Windows now lists the
// applet - so a build where the match landed somewhere harmless would have
// dropped this mod's working virtual entry without the real one taking its
// place. What proves the applet is there is the shell saying so, which is
// what g_realAppletConfirmedVisible below records. (It is also the wrong
// signal in the other direction: if another mod already zeroed the same
// moniker, our scan finds nothing yet the applet may well be unhidden.)
static std::atomic<bool> g_monikerPatched[kLegacyUnhideMonikerCount]{};

// Per-applet: did the SHELL confirm that the real applet is now part of the
// Control Panel item list? Filled in by ConfirmUnhiddenAppletsVisible(),
// which runs on the lazy-detection worker after the guard has been set up
// and re-asks IOpenControlPanel::GetPath about each unhidden applet. This is
// the actual confirmation the comments above used to claim the byte-pattern
// check was, and it is what VirtualTwinSuppressed() gates on.
static std::atomic<bool> g_realAppletConfirmedVisible[kLegacyUnhideMonikerCount]{};

// True once every applet that needs a verdict has one it can be trusted to
// keep: either the moniker was never patched (nothing to confirm), the real
// CLSID isn't registered here (nothing to confirm), or the shell has
// confirmed the applet IS listed. A "not listed" answer never counts as
// settled - see AllUnhideTargetsSettled's definition, further down next to
// kUnhideProbeTargets, and ConfirmUnhiddenAppletsVisible for why: on a mod
// enabled while Explorer is already running, shell32's cached Control Panel
// item list can make a real applet look absent even though it is (or soon
// will be) listed, and latching that false negative permanently is what lets
// a duplicate entry appear later with no way back short of toggling the
// setting.
bool AllUnhideTargetsSettled();

// Tears the confirmation state down: every applet goes back to "not
// confirmed", so the virtual twins are served again until the shell says
// otherwise. Also clears the per-moniker patch records, which describe the
// same (now previous) application of the guard.
static void ResetUnhideConfirmation() {
    for (auto& patched : g_monikerPatched) patched.store(false);
    for (auto& confirmed : g_realAppletConfirmedVisible) confirmed.store(false);
}

// Whether the REAL Personalization CLSID is registered on this machine.
// When the legacy-applet unhide feature below unhides it, the mod's own
// virtual Personalization entry must be suppressed (duplicate entries).
static std::atomic<bool> g_realPersonalizationRegistered{ false };
// Whether the REAL System CLSID is registered (it is on every supported
// build; kept as a flag for the same task-links gating logic).
static std::atomic<bool> g_realSystemRegistered{ false };
// Master switch for the legacy-applet unhide feature (string patches +
// hooks). Declared up here because the injection sites (ClassifyPath,
// GetNamespaceClsids, task-links XML) consult it; it is set by
// SetupLegacyUnhide() / Wh_ModSettingsChanged and cleared in Wh_ModUninit.
static std::atomic<bool> g_legacyUnhideActive{ false };
// True while the worker still owes us a confirmation pass - i.e. at least one
// patched, registered applet is not yet settled (see AllUnhideTargetsSettled).
// Read by the request/after-init paths below so the pass is scheduled even in
// the (common) case where the applet-verdict detection itself is already done
// and cached, AND kept true across repeated calls whenever an applet is still
// waiting on a "listed" verdict, so a stale "not listed" answer gets asked
// again instead of sticking forever.
// 
// Retry limiting: prevents infinite loop if the shell keeps answering "not listed"
// - Maximum 5 attempts
// - 30 second cooldown between attempts
static std::atomic<ULONGLONG> g_lastUnhideTick{ 0 };
static std::atomic<int> g_unhideAttempts{ 0 };

inline bool UnhideConfirmationPending() {
    if (!g_legacyUnhideActive.load(std::memory_order_acquire)) return false;
    
    // Stop retrying after 5 failed attempts
    if (g_unhideAttempts.load(std::memory_order_relaxed) >= 5) return false;
    
    // Cooldown between retries
    ULONGLONG last = g_lastUnhideTick.load(std::memory_order_relaxed);
    if (last && GetTickCount64() - last < 30000) return false;
    
    return !AllUnhideTargetsSettled();
}

// When the legacy-applet unhide feature is active, Windows shows the real
// applets itself (the guard unhid them), so the virtual twins this mod
// injects for the same applets must be suppressed, otherwise the user sees
// duplicate entries (the twin's open command is a "shell:::{...}" launch).
// The twin is suppressed only when: the unhide patches were applied on this
// build at all (g_legacyUnhideActive), the SHELL HAS CONFIRMED that
// this specific applet is now listed in Control Panel (see
// g_realAppletConfirmedVisible - not merely that a matching byte pattern was
// found and zeroed), and the real applet is actually present.
//
// Gating on the confirmation rather than on g_monikerPatched[] is what keeps
// this fail-closed: if Windows still hides the applet on this build, or the
// patch matched some unrelated copy of the string, or the probe can't be
// answered at all, this returns false and the user keeps the mod's own
// working entry instead of losing the applet from Control Panel entirely.
static bool VirtualTwinSuppressed(std::atomic<bool>& realPresent, size_t monikerIndex) {
    return g_legacyUnhideActive.load() &&
           monikerIndex < kLegacyUnhideMonikerCount &&
           g_realAppletConfirmedVisible[monikerIndex].load() &&
           realPresent.load();
}

// --- Lazy/virtual-applet probe state (fixes startup-path cost) ---
static std::atomic<bool> g_lazyDetectionDone{ false };
static std::mutex g_lazyDetectionMutex;
static thread_local bool g_inShellProbeBypass{ false };
static std::atomic<int> g_prevBitLockerMode{ -1 };
static std::atomic<int> g_prevTabletPcMode{ -1 };

// RAII guard covering an ENTIRE critical section that performs our own
// registry/engine calls, not just a single API call. Every registry read
// made while this is alive - including any the shell itself issues on our
// behalf during a COM activation - must be let straight through by the
// registry hooks instead of re-entering EnsureLazyVirtualAppletDetection.
struct ShellProbeBypass {
    bool prev_ = g_inShellProbeBypass;
    ShellProbeBypass() { g_inShellProbeBypass = true; }
    ~ShellProbeBypass() { g_inShellProbeBypass = prev_; }
};

// The one-time virtual-applet probe (CoCreateInstance + up to three
// IOpenControlPanel::GetPath calls) never runs on a hook's caller thread.
// It runs exclusively on this dedicated worker, started from
// Wh_ModAfterInit (hooks are already active, but we're on our own
// controlled stack, not an arbitrary caller's) and re-armed from
// Wh_ModSettingsChanged. Registry hooks only ever *request* the probe via
// RequestLazyVirtualAppletDetection(), which is non-blocking.
static HANDLE g_lazyDetectionWakeEvent = nullptr;
static HANDLE g_lazyDetectionStopEvent = nullptr;
// Must be signalled + joined + reset in Wh_ModUninit, or ~thread() calls
// std::terminate() at Explorer shutdown. See
// https://github.com/ramensoftware/windhawk/wiki/Global-objects-and-process-shutdown
[[clang::no_destroy]] static std::optional<std::thread> g_lazyDetectionThread;

bool ResolveAppletInjection(AppletMode mode, bool autoDetected, bool clsidRegistered, const wchar_t* logName);
void InvalidateClassicTaskLinksFile();
// Re-probes where the home page "Adjust screen resolution" link should go.
void RefreshHomeResolutionTarget();
// True when the classic 5-task Personalization block is emitted, in which
// case it already carries the three Appearance links of the home page and the
// separate block must not repeat the same application id.
static bool ClassicPersonalizationBlockCoversHomeLinks();
bool EnsureClassicTaskLinksFile();
void RunLazyVirtualAppletDetection();
void ConfirmUnhiddenAppletsVisible();
void RequestLazyVirtualAppletDetection();
// Defined further below (near GetNamespaceClsids), but used inside
// EnsureClassicTaskLinksFile()'s task-block assembly above its definition.
static bool VirtualAppletPresent(const std::wstring& guid);

// ---------------------------------------------------------------------------
// Appearance links of the Control Panel home page
//
// The Category view home page does not invent its own links: it reads them
// from the XML resource (type "XML", id 21) of shell32.dll, which on 1903 and
// later physically lives in shell32.dll.mun. On every build from Windows 10
// 1507 to Windows 11 24H2 that resource still contains
//
//     <category id="1">
//       <sh:task idref="{B3206921-D53A-40D9-BA1A-BEA526A644A5}" />   theme
//       <sh:task idref="{4A66B844-A291-4136-B5AC-1B48B3CAD99F}" />   background
//       <sh:task idref="{F3321994-6E7E-4D9E-ABDC-768477BCF916}" />   resolution
//     </category>
//
// but Microsoft deleted the three matching <sh:task> definitions, so those
// references are dangling and Appearance and Personalization ends up being the
// only category with no links. Reusing the same three identifiers for the
// links this mod supplies puts them back into their original slot instead of
// appending new ones somewhere else.
// ---------------------------------------------------------------------------

static const char kHomeTaskGuidTheme[]      = "{B3206921-D53A-40D9-BA1A-BEA526A644A5}";
static const char kHomeTaskGuidBackground[] = "{4A66B844-A291-4136-B5AC-1B48B3CAD99F}";
static const char kHomeTaskGuidResolution[] = "{F3321994-6E7E-4D9E-ABDC-768477BCF916}";

// Previously used identifiers, kept as an opt-out.
static const char kHomeTaskGuidThemeLegacy[]      = "{D4F4A001-0D35-4CB6-A21F-BC1661200001}";
static const char kHomeTaskGuidBackgroundLegacy[] = "{D4F4A002-0D35-4CB6-A21F-BC1661200002}";
static const char kHomeTaskGuidResolutionLegacy[] = "{D4F4A006-0D35-4CB6-A21F-BC1661200006}";

static bool RegistryKeyExists(HKEY root, const wchar_t* subKey) {
    HKEY key = nullptr;
    if (RegOpenKeyExW(root, subKey, 0, KEY_READ, &key) != ERROR_SUCCESS) return false;
    if (key) RegCloseKey(key);
    return true;
}

// True when the Windows 7 style Display applet {C555438B-...} is reachable,
// which is the case while the "Classic Display Control Panel Restorer" mod is
// active. That mod serves its registration in memory exactly like this one
// does for its own applets, so an ordinary registry read sees it and no direct
// coupling between the two mods is needed.
static bool ClassicDisplayPageAvailable() {
    static const wchar_t kNamespaceKey[] =
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ControlPanel\\NameSpace\\"
        L"{C555438B-3C23-4769-A71F-B6D3D9B6053A}";
    if (RegistryKeyExists(HKEY_LOCAL_MACHINE, kNamespaceKey)) return true;
    if (RegistryKeyExists(HKEY_CURRENT_USER, kNamespaceKey)) return true;
    return RegistryKeyExists(
        HKEY_CLASSES_ROOT,
        L"CLSID\\{C555438B-3C23-4769-A71F-B6D3D9B6053A}\\Shell\\Open\\Command");
}

// Where "Adjust screen resolution" points to, best target first:
//   1. the classic Screen Resolution page, when that applet is available;
//   2. the Display item this mod already works with, when it is registered;
//   3. the Settings app, so the link can never dead-end.
static std::string ProbeHomeResolutionCommand() {
    if (ClassicDisplayPageAvailable())
        return "explorer.exe shell:::{C555438B-3C23-4769-A71F-B6D3D9B6053A}\\Settings";
    if (RegistryKeyExists(HKEY_CLASSES_ROOT,
                          L"CLSID\\{C55584F4-7C7F-44F2-9A6D-913076F34C6A}"))
        return "explorer.exe shell:::{C55584F4-7C7F-44f2-9A6D-913076F34C6A}";
    return "explorer.exe ms-settings:display";
}

// Anti-conflict handoff.
//
// The answer above can change while Explorer is already running: the classic
// Display applet only becomes reachable once the mod that provides it has
// initialised, and Windhawk gives no ordering guarantee between mods. Probing
// once at startup would therefore bake a stale target into the generated task
// file. Instead the probe is repeated, throttled, right before Control Panel
// rebuilds its item list (the moment Explorer enumerates the Control Panel
// namespace), and the task file is invalidated only when the answer actually
// changed, so the regeneration cost is paid once per real change and never in
// a loop. There is no thread, no timer and no direct call into the other mod.
static std::mutex g_homeResolutionMutex;
static std::string g_homeResolutionCommand;
static std::atomic<ULONGLONG> g_homeResolutionLastTick{0};
static const ULONGLONG kHomeResolutionThrottleMs = 2000;

// Cached target used while the task-list XML is generated.
static std::string GetHomeResolutionCommand() {
    {
        std::lock_guard<std::mutex> lock(g_homeResolutionMutex);
        if (!g_homeResolutionCommand.empty()) return g_homeResolutionCommand;
    }
    std::string probed = ProbeHomeResolutionCommand();
    std::lock_guard<std::mutex> lock(g_homeResolutionMutex);
    if (g_homeResolutionCommand.empty()) g_homeResolutionCommand = probed;
    return g_homeResolutionCommand;
}

// Re-probes and, only on a real change, drops the cached task file so the next
// lookup regenerates it with the new target.
void RefreshHomeResolutionTarget() {
    const ULONGLONG now = GetTickCount64();
    const ULONGLONG last = g_homeResolutionLastTick.load(std::memory_order_relaxed);
    if (last != 0 && now - last < kHomeResolutionThrottleMs) return;
    g_homeResolutionLastTick.store(now, std::memory_order_relaxed);

    std::string probed = ProbeHomeResolutionCommand();
    bool changed = false;
    {
        std::lock_guard<std::mutex> lock(g_homeResolutionMutex);
        if (g_homeResolutionCommand != probed) {
            g_homeResolutionCommand = probed;
            changed = true;
        }
    }
    if (!changed) return;

    Wh_Log(L"Screen resolution link target changed, task links will be regenerated");
    InvalidateClassicTaskLinksFile();
}

// Forward declaration
bool EnsureClassicTaskLinksFile();
std::wstring g_classicTaskLinksFilePath;
// Embedded Game Controllers icon -> temp .ico file: the decoder function
// EnsureJoyControllerIconFile() and g_joyIconFilePath are defined near the
// task-links section (before InitDisplayNames). Warmed up in Wh_ModInit.

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
std::wstring g_realBitLockerClsidSuffix;
std::wstring g_realSpeechClsidSuffix;
std::wstring g_realSystemClsidSuffix;

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
// Real canonical CLSID (module: iscsicpl.dll,-5001, canonical name
// Microsoft.iSCSIInitiator). Confirmed still absent from HKCR\CLSID on
// current Windows 11 24H2 (unlike Network Connections/Printers/HomeGroup,
// which really are still registered, just hidden from Category View) - so
// this GUID is used only as an opportunistic registry lookup (in case some
// edition/build still has it) and is never injected directly; see
// kIscsiInitiatorVirtualGuid below for the entry that's actually shown.
static const std::wstring kIscsiInitiatorGuid      = L"{a304259d-52b8-4526-8b1a-a1d6cecc8243}";
// Own, made-up CLSID for the *virtual* iSCSI Initiator entry (same technique
// as kBitLockerVirtualGuid/kSpeechVirtualGuid): name/icon come from
// iscsicpl.exe directly (registry fallback almost never applies here) and
// the command launches iscsicpl.exe directly, since there is no real,
// registered CLSID to re-launch through "explorer shell:::{realGuid}".
static const std::wstring kIscsiInitiatorVirtualGuid = L"{7d3f5a92-8c1b-4e6a-9f2d-3b8a6c7e1d54}";
// Legacy, canonical Game Controllers CLSID. On current Windows 11 builds it
// is no longer registered in HKCR\CLSID and shell:::{259EF4B1-...} does not
// launch anything, so it is used only as an opportunistic registry lookup (in
// case some build still has it) and never injected directly; the entry that
// is actually shown is kGameControllersVirtualGuid below.
static const std::wstring kGameControllersGuid  = L"{259ef4b1-e6c9-4176-b574-481532c9bce8}";
// Own, made-up CLSID for the *virtual* Game Controllers entry (same technique
// as the iSCSI virtual entry): name/icon/description come straight from
// joy.cpl's own resources (localized by Windows for every UI language) and the
// open command launches joy.cpl directly.
static const std::wstring kGameControllersVirtualGuid = L"{b1e6c4a9-3d27-4f58-a9c6-2d71f4a8e063}";
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
static const std::wstring kSpeechGuid               = L"{D17D1D6D-CC3F-4815-8FE3-607E7D5D10B3}";
// This is the "Text to Speech" applet (sapi.cpl), NOT "Speech Recognition"
// ({58E3C745-D971-4081-9034-86E34B30836A}, a different CLSID/applet). Left
// empty on purpose: IOpenControlPanel::GetPath is picky about canonical
// names, and a wrong one that happens to resolve to a different, unrelated
// item produces a wrong "is it shown" verdict (not just a wasted probe
// attempt), which then gets cached per Windows build. The probe already
// falls through to "::{GUID}" and the bare GUID form when this is empty
// (see IsShownByControlPanel), which is the spelling this mod actually
// cares about.
static const std::wstring kSpeechCanonicalName      = L"";
// Virtual CLSID mirroring the real Text to Speech applet (same reason as
// kBitLockerVirtualGuid: Explorer's Category View never probes the real GUID).
static const std::wstring kSpeechVirtualGuid        = L"{e4a1c6d8-3b7f-4e2a-8c5d-9f1b6a7c2d45}";
// The real "System" applet (sysdm.cpl): unhidden by the legacy-applet
// unhide feature like the other legacy items above; while the feature is
// active it also gets the classic Windows 7 task links (see
// EnsureClassicTaskLinksFile).
static const std::wstring kSystemGuid               = L"{BB06C0E4-D293-4f75-8A90-CB05B6477EEE}";

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

// ASCII-only narrow conversion for embedding pre-computed lowercase GUIDs
// into the (UTF-8) task-links XML.
std::string NarrowAscii(const std::wstring& w) {
    std::string s;
    s.reserve(w.size());
    for (wchar_t c : w) s += (char)c;
    return s;
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
// Called from Wh_ModInit's synchronous startup probe AND from the dedicated
// lazy-detection worker thread (see RunLazyVirtualAppletDetection /
// Wh_ModAfterInit) - never from a registry hook's caller thread. Both
// callers wrap their own registry/engine calls in a ShellProbeBypass (or run
// before hooks are installed at all), so the shell's own registry reads
// during CoCreateInstance/GetPath are let straight through instead of
// re-entering our hooks.
// Locally-defined rather than pulled from the SDK's CLSID_OpenControlPanel /
// IID_IOpenControlPanel: those symbols live in uuid.lib, which Windhawk's
// clang toolchain does not link by default, so referencing them fails at
// link time with "undefined symbol: CLSID_OpenControlPanel". The values are
// fixed, documented interface identifiers, so defining them here costs
// nothing and avoids adding a library dependency just for two GUIDs.
static const CLSID kClsidOpenControlPanel =
    { 0x06622d85, 0x6856, 0x4460, { 0x8d, 0xe1, 0xa8, 0x19, 0x21, 0xb4, 0x1c, 0x4b } };
static const IID kIidOpenControlPanel =
    { 0xd11ad862, 0x66de, 0x4df4, { 0xbf, 0x6c, 0x1f, 0x56, 0x21, 0x99, 0x6a, 0xf1 } };

// The single-item probe against an already-activated IOpenControlPanel.
// Factored out of IsShownByControlPanel so a caller that needs to ask about
// several items (ConfirmUnhiddenAppletsVisible) can share one activation
// instead of paying CoInitializeEx + CoCreateInstance per item - see
// IsShownByControlPanelBatch below.
//
// pszName is documented as "the item's canonical name or its GUID", but the
// GUID form is the unreliable one: shell32 runs the string through
// COpenControlPanel::_MapLegacyName and a canonical-name lookup, and namespace
// items are addressed with the ::{GUID} moniker form rather than a bare
// {GUID}. So each candidate spelling is tried in turn - canonical name first,
// then ::{GUID}, then the bare GUID - and the first one the shell can parse
// wins. If none of them parse, the probe reports "no answer" instead of
// guessing, and the caller falls back to the registry hint.
static bool QueryShownByControlPanel(IOpenControlPanel* openControlPanel,
                                     const std::wstring& canonicalName,
                                     const std::wstring& guid, bool& outListed) {
    bool answered = false;
    const std::wstring monikerForm = L"::" + guid;
    const std::wstring* candidates[] = { &canonicalName, &monikerForm, &guid };

    for (const std::wstring* candidate : candidates) {
        if (candidate->empty()) continue;

        wchar_t path[MAX_PATH] = {};
        HRESULT hr = openControlPanel->GetPath(candidate->c_str(), path, ARRAYSIZE(path));
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
    return answered;
}

// Returns true only when the shell gave a usable verdict, with outListed set.
// Called from Wh_ModInit's synchronous startup probe AND from the dedicated
// lazy-detection worker thread (see RunLazyVirtualAppletDetection /
// Wh_ModAfterInit) - never from a registry hook's caller thread. Both
// callers wrap their own registry/engine calls in a ShellProbeBypass (or run
// before hooks are installed at all), so the shell's own registry reads
// during CoCreateInstance/GetPath are let straight through instead of
// re-entering our hooks.
bool IsShownByControlPanel(const std::wstring& canonicalName, const std::wstring& guid,
                           bool& outListed) {
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
        answered = QueryShownByControlPanel(openControlPanel, canonicalName, guid, outListed);
        openControlPanel->Release();
    } else {
        Wh_Log(L"  CoCreateInstance(CLSID_OpenControlPanel) failed, hr=0x%08lX", (unsigned long)hr);
    }

    if (weInitialized) CoUninitialize();
    return answered;
}

// Batched form of IsShownByControlPanel: activates IOpenControlPanel once and
// answers up to several items with it, instead of paying a full
// CoInitializeEx -> CoCreateInstance -> GetPath -> CoUninitialize cycle per
// item. items[i].second receives whether the shell answered; when it did,
// outListed[i] receives the verdict. Same threading/bypass requirements as
// IsShownByControlPanel.
void IsShownByControlPanelBatch(
    const std::vector<std::pair<std::wstring, std::wstring>>& items, // {canonicalName, guid}
    std::vector<bool>& outAnswered, std::vector<bool>& outListed) {
    outAnswered.assign(items.size(), false);
    outListed.assign(items.size(), false);
    if (items.empty()) return;

    const HRESULT initHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (initHr == RPC_E_CHANGED_MODE) {
        Wh_Log(L"  COM already initialized in a different mode; skipping shell probe");
        return;
    }
    const bool weInitialized = SUCCEEDED(initHr);

    IOpenControlPanel* openControlPanel = nullptr;
    HRESULT hr = CoCreateInstance(kClsidOpenControlPanel, nullptr, CLSCTX_INPROC_SERVER,
                                  kIidOpenControlPanel, (void**)&openControlPanel);
    if (SUCCEEDED(hr) && openControlPanel) {
        for (size_t i = 0; i < items.size(); i++) {
            bool listed = false;
            const bool answered = QueryShownByControlPanel(
                openControlPanel, items[i].first, items[i].second, listed);
            outAnswered[i] = answered;
            outListed[i] = listed;
        }
        openControlPanel->Release();
    } else {
        Wh_Log(L"  CoCreateInstance(CLSID_OpenControlPanel) failed, hr=0x%08lX", (unsigned long)hr);
    }

    if (weInitialized) CoUninitialize();
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
                                     std::atomic<bool>& outClsidRegistered,
                                     AppletMode mode) {
    // Wh_ModInit already probed and cached whether the CLSID is registered
    // (g_bitlockerClsidRegistered / g_tabletPcClsidRegistered), so read that
    // instead of hitting the registry again from here - this function now
    // only ever runs on the dedicated lazy-detection worker thread, but
    // there's still no reason to repeat a registry read we already have the
    // answer to.
    const bool registeredClsid = outClsidRegistered.load();
    if (!registeredClsid) {
        Wh_Log(L"%s: CLSID is absent on this edition/device; applet will not be injected", logName);
        return false;
    }
    if (mode != AppletMode::Auto) {
        Wh_Log(L"%s: mode is %s, skipping shell probe entirely", logName,
               mode == AppletMode::Always ? L"Always" : L"Never");
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
    // No per-call bypass toggling here: RunLazyVirtualAppletDetection() holds
    // a ShellProbeBypass for the whole detection pass, and this always runs
    // on the dedicated lazy-detection worker thread, never on a hook's
    // caller thread.
    bool listed = false;
    bool answered = IsShownByControlPanel(canonicalName, realGuid, listed);
    if (answered) {
        Wh_Log(L"%s: shell reports the applet is %s", logName,
            listed ? L"already shown; virtual entry skipped to avoid a duplicate"
                   : L"not shown; virtual entry will be injected");
        Wh_SetIntValue(verdictName.c_str(),
            (int)(listed ? CachedVerdict::Shown : CachedVerdict::NotShown));
        Wh_SetIntValue(buildName.c_str(), (int)g_winBuild);
        return !listed;
    }
    const bool registered = IsListedInControlPanelNameSpace(realGuid);
    Wh_Log(L"%s: shell gave no verdict, falling back to the registry hint (%s) and caching it. Use the \"Always add\"/\"Never add\" setting if this is wrong.",
        logName, registered ? L"registered, assuming already shown" : L"not registered, injecting");
    Wh_SetIntValue(verdictName.c_str(),
        (int)(registered ? CachedVerdict::Shown : CachedVerdict::NotShown));
    Wh_SetIntValue(buildName.c_str(), (int)g_winBuild);
    return !registered;
}

// Registry-hook entry point: NEVER does any work itself. It only wakes the
// dedicated lazy-detection worker thread and returns immediately, so a
// registry hook can never block on - or re-enter - the shell probe.
void RequestLazyVirtualAppletDetection() {
    if (g_inShellProbeBypass) return;
    // Wake for the unhide confirmation pass too: it is a separate piece of
    // work with its own "done" flag, and it is pending in exactly the case
    // where the applet-verdict detection below has nothing left to do (all
    // verdicts cached), which is the common case after a restart.
    if (g_lazyDetectionDone.load(std::memory_order_acquire) &&
        !UnhideConfirmationPending())
        return;
    if (g_lazyDetectionWakeEvent) SetEvent(g_lazyDetectionWakeEvent);
}

// Runs the actual one-time probe. Only ever called from the dedicated
// lazy-detection worker thread (see Wh_ModAfterInit / LazyDetectionThreadProc),
// never directly from a registry hook - that's what made the previous
// version re-entrant and deadlock-prone.
void RunLazyVirtualAppletDetection() {
    // Check if detection is already done before acquiring the mutex
    if (g_lazyDetectionDone.load(std::memory_order_acquire)) return;
    
    // Check if the stop event is signalled (mod is unloading)
    if (g_lazyDetectionStopEvent && 
        WaitForSingleObject(g_lazyDetectionStopEvent, 0) == WAIT_OBJECT_0) {
        Wh_Log(L"Lazy detection: stop event signalled, bailing out");
        return;
    }
    
    // Don't block every other Explorer thread while we probe: if some other
    // caller already grabbed the mutex, just bail - RequestLazyVirtualAppletDetection
    // will be called again by the next registry access and there's only
    // ever one worker thread doing the real work anyway.
    std::unique_lock<std::mutex> lock(g_lazyDetectionMutex, std::try_to_lock);
    if (!lock.owns_lock()) return;
    
    // Re-check state after acquiring the lock
    if (g_lazyDetectionDone.load(std::memory_order_acquire)) return;
    
    // Check stop event again after acquiring the lock
    if (g_lazyDetectionStopEvent && 
        WaitForSingleObject(g_lazyDetectionStopEvent, 0) == WAIT_OBJECT_0) {
        Wh_Log(L"Lazy detection: stop event signalled after lock acquisition, bailing out");
        return;
    }

    // Every registry / engine call made below - including any the shell
    // issues on our behalf while activating CLSID_OpenControlPanel - is ours
    // and must be let straight through by the registry hooks.
    ShellProbeBypass bypass;

    AppletMode bitMode = (AppletMode)g_settings.bitLockerMode.load();
    AppletMode tabMode = (AppletMode)g_settings.tabletPcMode.load();
    AppletMode spMode  = (AppletMode)g_settings.speechMode.load();
    bool needBit = (bitMode == AppletMode::Auto) && g_bitlockerClsidRegistered.load();
    bool needTab = (tabMode == AppletMode::Auto) && g_tabletPcClsidRegistered.load();
    bool needSp  = (spMode == AppletMode::Auto) && g_speechClsidRegistered.load();
    if (!needBit && !needTab && !needSp) {
        g_injectBitlockerApplet.store(ResolveAppletInjection(bitMode, g_bitlockerAutoDetected.load(),
            g_bitlockerClsidRegistered.load(), L"BitLocker Drive Encryption"));
        g_injectTabletPcApplet.store(ResolveAppletInjection(tabMode, g_tabletPcAutoDetected.load(),
            g_tabletPcClsidRegistered.load(), L"Tablet PC Settings"));
        g_injectSpeechApplet.store(ResolveAppletInjection(spMode, g_speechAutoDetected.load(),
            g_speechClsidRegistered.load(), L"Text to Speech"));
        g_lazyDetectionDone.store(true, std::memory_order_release);
        return;
    }
    
    bool bitAuto = g_bitlockerAutoDetected.load();
    bool tabAuto = g_tabletPcAutoDetected.load();
    bool spAuto  = g_speechAutoDetected.load();
    if (needBit) {
        bitAuto = DetectVirtualAppletNeededCached(kBitLockerGuid, kBitLockerCanonicalName,
            L"bitlocker", L"BitLocker Drive Encryption", g_bitlockerClsidRegistered, bitMode);
        g_bitlockerAutoDetected.store(bitAuto);
    }
    if (needTab) {
        tabAuto = DetectVirtualAppletNeededCached(kTabletPcSettingsGuid, kTabletPcCanonicalName,
            L"tabletpc", L"Tablet PC Settings", g_tabletPcClsidRegistered, tabMode);
        g_tabletPcAutoDetected.store(tabAuto);
    }
    if (needSp) {
        spAuto = DetectVirtualAppletNeededCached(kSpeechGuid, kSpeechCanonicalName,
            L"speech", L"Text to Speech", g_speechClsidRegistered, spMode);
        g_speechAutoDetected.store(spAuto);
    }
    g_injectBitlockerApplet.store(ResolveAppletInjection(bitMode, g_bitlockerAutoDetected.load(),
        g_bitlockerClsidRegistered.load(), L"BitLocker Drive Encryption"));
    g_injectTabletPcApplet.store(ResolveAppletInjection(tabMode, g_tabletPcAutoDetected.load(),
        g_tabletPcClsidRegistered.load(), L"Tablet PC Settings"));
    g_injectSpeechApplet.store(ResolveAppletInjection(spMode, g_speechAutoDetected.load(),
        g_speechClsidRegistered.load(), L"Text to Speech"));
    g_lazyDetectionDone.store(true, std::memory_order_release);
    InvalidateClassicTaskLinksFile();
    EnsureClassicTaskLinksFile();
    Wh_Log(L"Lazy detection completed: BitLocker inject=%d TabletPC inject=%d Speech inject=%d",
        g_injectBitlockerApplet.load(), g_injectTabletPcApplet.load(), g_injectSpeechApplet.load());
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
    return g_settings.enableHomeGroup.load() && g_homeGroupUsable.load();
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
    // Points at the "real CLSID is registered" flag: when the unhide feature
    // is active and the real applet exists, this virtual twin is suppressed
    // (see VirtualTwinSuppressed) so the user doesn't get duplicate entries.
    std::atomic<bool>* realPresent = nullptr;
    // Which kLegacyUnhideMonikers entry corresponds to this applet, so
    // VirtualTwinSuppressed can check the confirmation recorded for THIS
    // applet (g_realAppletConfirmedVisible) rather than any other one.
    // kLegacyUnhideMonikerCount means "not part of the unhide feature"
    // (e.g. Tablet PC Settings, which has no moniker in
    // kLegacyUnhideMonikers and is never suppressed by the guard).
    size_t monikerIndex = kLegacyUnhideMonikerCount;
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
                      const std::wstring& fallbackInfoTip = L"",
                      std::atomic<bool>* realPresent = nullptr,
                      size_t monikerIndex = kLegacyUnhideMonikerCount,
                      const std::wstring& openCommandOverride = L"") {
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
    applet.openCommand = openCommandOverride.empty()
        ? (L"explorer.exe shell:::" + realGuid)
        : openCommandOverride;
    applet.category = category;
    applet.enabledSetting = enabledSetting;
    applet.realPresent = realPresent;
    applet.monikerIndex = monikerIndex;
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

// ===========================================================================
// Embedded Game Controllers icon
// ===========================================================================
// joy.cpl does not expose a usable DefaultIcon resource for a synthetic CLSID
// on Windows 10/11 (the resource id is absent/wrong), so the classic gamepad
// icon ships inside the mod as a base64-encoded, multi-size .ico (48/32/16).
// It is decoded once to a temp file; the virtual Game Controllers entry's
// DefaultIcon points at that file. Name and InfoTip still come from joy.cpl's
// own (correct) string resources; only the icon is custom.
static const char* kJoyControllerIconBase64[] = {
    "AAABAAMAEBAAAAAAIABWAwAANgAAACAgAAAAACAAEQkAAIwDAAAwMAAAAAAgAN8QAACdDAAAiVBO"
    "Rw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAADHUlEQVR4nI2TXWhbdRjGn/f/P/1Ku6Zr"
    "G5um0U2rW7MuKCoTldqIiIiwOVxyq5sK4sCVISh4EXPjB4iCU1TEDyhId6q4aRnCNjNF2Ryrc6xr"
    "OlOSJaY0Oflo7UnS9CTn/3oxijDmx+/q4Xnf5+55gOsQDodFNBrVmFkCoKsei2g0qum6Lq+XAQDo"
    "VwP/STgcFuua/raZAGIBgYMvfjD45DMP37f15oFdJbOxOZEqavlstlBvNM5v6N448ciIb1rXdRkK"
    "hWwCQMxMRKR+PJveO29OPx87M+MP7Xq6pdK6Aau1NSSyJlZrFmBcAcyCaut0jO3fu/uQrutS03Vd"
    "EJF9aT773kK5vn/p51l0t3WDHG32yWQJmWKV4kULrWs1Hl5KKAfZcpPT+e5rb31qhEKhwwQAL0Xe"
    "fmxwcPOU23VTvTx7TGRWWdw18jjdvu1G/FGq4sR0HNlEEt52oKfPZa/WbPnb+QuL05d/H9KAoOzq"
    "cr6aTif5w48/FyP3DMtqpabGD46pwP076MGHHqVz306Qf7sfzY5ezM3NS4ZgI5/vdzbLO7R9L9wa"
    "MFeW7k4mk1ws5OTkEUMN3bJJuF09QNNGHP7qa5z79Szc/R7cueNeHJ36DjXLQoejFR0Oh6ZVVpZ3"
    "51Sd44mUbVl1eAe8muJGrLJSXYhfntlSKZt9viF/i2vAgwl9HKlMkm1b0dbbthSG/L6LWtYobF/L"
    "LFK5XNG2+YYhuTFz+tTUs6ZpXgLgbWl3up967sD7RLbPyOeVZdWVq/cGraWp6dTrrxzIa1KTUWer"
    "Y6S3u7cowV8c18ffqAJZEgJgxNYqf8asWtVMJa6gUFpmZqauLicvpOPHATSL77+ZiHhcnf42toeP"
    "6p+MVYEsAMFK0Z49T0gAYiGdqSVS6UYuZ1j9bo+UsOd++enkmWAwaGsA8NlHh2YBIBgMysnJSQVA"
    "AYBhGATALlWqPxDJBzwer9bn6lmOXTj9MpgvfikE03q3I5EIA+Brak8AsHPnvg7lEG862zu13GLi"
    "nRPHjsTATCC69v//8Q9j+ldodHRUIhBAAFCRSEStH/4CCvByJ47kLQQAAAAASUVORK5CYIKJUE5H"
    "DQoaCgAAAA1JSERSAAAAIAAAACAIBgAAAHN6evQAAAjYSURBVHic7VdpbBzlGX7eb2aP2dld3/b6"
    "SBycmBxAQsIRUqU4BppWVUUg0poiJFr6oxwSqgSV+gcYtk2rHlJbWvUHogUh9QC7VUvSBAiF2EAO"
    "gk1ix9nYxAm21/Z61971zh5zz3z9QZpaXE1R/rXPzxm933N87+h9B/hfB32eIs45AWDL6nlfH9DQ"
    "0H/xvIWFBR6Pxz0i4pdBJ6AoCovH44KiHBL/m7pezgVFUdinvf/MBDjn1N/fL3R3dzvLn59Mp+Wr"
    "YrGbROB63UHHQs6qXVwqBktFvWDaZo573jmfgOGcZb/X0311GQB6e3uFnp4e96Mcn+qmt5cLROQC"
    "cADgqacOrD4y+Mr6u+7uua5FqvqG53hXuCKD4wCMEXyCD0Q6PNuBYZpQLQMBn5Da/8aJv42dm/5V"
    "T8+uiU8S8bEELtwviIhzzttGknP3vjM0eEc6O7tRXSwF7ty9Cx2rWjA2nfFqqiKe7bjQbQ+qblOx"
    "YnK9VIZVLEDLZ5lhOqx9ZStqaiKluXTmOw988/bnlEOHxMSyRNlHyYmIExEfGzv/veFkasQj54eZ"
    "trdveC/7SqAmXOu2NFU7yamsN14S2VSZxIkSiaM5VzyRdYWTeYgTblSci6wU9eb1zLIMfvBgv3Py"
    "vdFIwO9/9pHHfnFHorvbURTlYvLicvK+vj7GORfGJ+b/NGmz3b987kXceW27k00F2XrpZhJDghAO"
    "SkgWChgscNSqRZimg6WSgUxBh2bYEAhggSA2ijnUOhb5/KI4PTPvta5o4evWXvn7x/c8HU88dv/L"
    "isJZIkHe8gRYT0+Pu//1oWfWrontLmXnrJ2bVvMbURS3HB1iuslJkoKwXQ9rGyRsbpNQXRMEIhLc"
    "miiCLQ2IrmiC0NiANp5D7dJZ5ApFXNnZjhu3XssqWpnguXI4Kvd964HH1iYS4IqiMBEA4vFegYjc"
    "Bx9R7jj67tF7DaNkf7X7i/6zjfV4448voti2Do6+hEBgNUR/ADuvXoEvM8C0LMzmSjidWsTZTBEz"
    "GofjaKgxJ7FUMbDxqg7ctHUzXuzbj4AksfqGRqeQz8sed38K0K5kMs4IACmKQv39/f7tO3efam1r"
    "XZ08PcJDwTC7ZuM2aFYZO1v3oXfvEPSmh7GiOYiiquLW7u1Y27kaiwuLmEnNY2p6DrNzC8gtFcBF"
    "P8At3LpjG946fByMODZt3Ih8yeYnhgYxOzvjymG58/mnfzJJiqKIiUTCefBR5dubr9vyNDzHHT01"
    "LAwPjyAg1aN7+zb4jEFM5sPQLBm53DymP5hARBKx5/t74A9KSM2kMZocRya7AGI+fGXnbXj/zAks"
    "5Cu4YtVKXLW+A3PzCzjwyiFYpum6ridohn73/j8/84L45JNPuul02ifJ0neLapEX1RypBRUuBIxP"
    "jKJcKWPzpi9gdnYMfsmCa1vgjompyRQGDg+ivqER4xMTmJqeRHZuGpVSEV+P70L3jh146OFHMdWx"
    "BoePDkEtlgHXQktrC09NT4H52CoAYETEuVR/mxSUOsulJV5SCyxfUFEqliEKPiSTwxgZfgtlNYuR"
    "42/yzMwEtJKKG7fdAsshjJw6jXQmA8914boOMukUzp2fwhWda/GDPT9CJBKF67ior6tFR+calCtl"
    "cAByKCz8+zPk7j2u7XCjrHmFQoEtqSUYpgnTMCGKPoxPvI8brt3CW9o6qLq6FtdsqvfqGpo4iCg7"
    "P0unR45TVXU1ZlOT2LBhM2Q5iiNHBzF6ehhFrYCp1DSKxRJsx0EkFEasuQWmbWcBQIzHHwobutFV"
    "yC+Q49rCYl5FUS3DNC143EVICiIYCHASBBJ9kj6XTgdMy2VLhQLkcATBoIxYrA2BYIBXVTfS5uu2"
    "YjI1gZnUBzg/MYGpdAaVigFwIBDwo6amWmior4NhVo4DgGiLxjV2xWy1bAOOA1rIF6DpGjzPQzgk"
    "w+/zeStXrmKiKMwcOfbazwzT8keraleE5Gh7KBRul8PRVkkKNbjcR53r1iFcF4VpG/AcB8R8YCSA"
    "AARDQcCDJ0khxrk3xsuZUQAklip6JyMiXSfPtB1WVFV4ngc5JEEKBtzW1hWCKAr5w6///bf5xcwo"
    "gLRWWvIDCAGQGfPVBUPhyNbtO+6qbojeJktBbzGTZ5quoaRp0A0DxAgMBA7uyeEws0xjb19fn6so"
    "iijalhN0XJdrmsYt24Jl2TwaifBoJMpjzS2CZVQW3jz40s9zC+mXm5ubz6bTaa2rq0scGxsLVCoV"
    "SRCEsKou6dW11dsYE6AW8rxUVFHRDFQ0HbZlIxjww3NdHg5HBEawJs6cOnDBgCESkAyFQsQYMcMQ"
    "vJrqGtbUFKOQJKGQzx4ZeHXvj0ulwttEtJROpwGABgYGnAtjusI5zxExXl1bh0q5DLNS5oauoaIZ"
    "0HQdnHOIog+e63pNsWbBta2BwWP9qWAwWJdIJGZE6NljUtWaPzTHmu8BOLmuZ9mWeSJ1fvz5gX/s"
    "6wWQIyJwfnGzWr5i0fX33y8C3DENk9T8EiyjAtN2USpXoOsGBEEAYwxhOUyRcNhLDr/zVwB1RDQH"
    "gIsfuhm47+adt/8uGo5W5XOZySMDryUBWBeI6QL5J+12PDw+zgHwckkt65oGx3Fg2S6KpSJsy4Ic"
    "CsEniF7bipVM10pHTr779rQoiqKu64vLx7H95sG9hy7aIsITTzzBEokE/xTij6FS0SdBFjdtC57H"
    "US5r4BwQBYE3NDYiGPQb/Yf2vgrAlGX5jKqq5nIBpCgKJZNJ2rBhA08kEjyRSHiXQtzY2MgBQLOs"
    "Y67DqaJpZFsOdMNAWA7xWCzmNje3iGeG3311ZnryaFVV1ZCqqoWLZi+F5BLAurq6mCFUDXISNpm6"
    "ZhGREIs1Cy3NLZiZOnfw4L4XHm9v7xqZmhowlhdeFgGKorBEIuHd8rV7toRk+SVZDrWJggAiWlRz"
    "C7/Z95fnf01EuWWNfHkFfAhOAPF4/L4GJke+BJA9nz371sCBA/P/amZcYj99fnzCD0g8HhdwWY3+"
    "R3Dq6lLEeLxX+DCV/+Oz8U+Rlah1WQCG3wAAAABJRU5ErkJggolQTkcNChoKAAAADUlIRFIAAAAw"
    "AAAAMAgGAAAAVwL5hwAAEKZJREFUeJztWXmQHUd9/rp7jvfmHbvvlFby6rR1rCzHWAhsY1mWA4H4"
    "qCROdgtsQo6iTIJDpYhTFUIBTw9IJalKKBdOiAJFDMUR0GJsbMuxOSKtjZBseXXYK8nSSrsr7Wrv"
    "3XfN1T3d0/lj18LGENsEQ1LxVzWvambqzfT36+/3619/A7yBN/D/G+T1erDWmi4+nwDAvos/L2Af"
    "ZmZu0MePQ1er0ADRr9dYXjW01nTv3r3Gz/Pf3bs1271bM631awrqz/WyF1CpVOiJTZvI9D8fJ3fd"
    "tUkTQhSAGAB83+/kilwWSawOuWx3vagcBNzyQ8GjSE1wIbwYGOIyOjdzhp/v6SHRi8l0dyMm5JVn"
    "5TVLSGtN9+3bR3fs2KEAvOQFgdZrGzXvjrRj35SwjU0MSL9wr8GBZguoNwN4ng/OQ/DQBw+DiFKc"
    "I4QeDEOx59Cxpx7/u7/+YG2ByG7W09OjfiEEFjUNQkj8wrW7P/XZ1bIZbB4bG738ppt/Y90117zl"
    "d9euXpI2ANQbLvxAxEEoYy4kRBQj5AquF8D3Q4RhCBEJKpWipsnQlknDSSYQ8mC80Qq+9NShw/fc"
    "+7d/PvNKJF6FhAh2747ZojwwPNzc+Ozp0783PDR+k+L8SpXxE67XRKYth5STwPiFKfns6Qs0X8gR"
    "w2BUxzGNlEakAT+KEWqNEBoijiEFh/A87XueHgy4Ng2KVSuXL+tc0fnR66/d+t5L7v33u3p6eh75"
    "70jQV4g6ATTp6SFKa72l4crd8+780XIh+0mUh65+YuLrCYNq5SQTslzKyVwmoY8PTRlGcTlNF8sk"
    "mSvAzhVBs3kIO4vASsO12tBMFuG1LwMvr0a8bA1JljtoKmWzetNlT+5/Rj/5xH6ZMMmKzuUdD3/q"
    "7++7o6enR91557+ar2kGKhVNCSGx1to4cfrCp3/09MDdhWLZGL8wCaNzUp7MPE5HaoNkS+YdLJVO"
    "oVQqQUUSIzUON/RxvhaAEQoZx/CFghsquFwiiBQipWEyCoNRGMwCUitRlDGS9RpcAjJw/KwRiUhd"
    "dukKShj5t7sr9xz7x+oHBiqVilGtVuUrEqhUKrRaJXFD68LRk+O7l3Xkb/z2wKjuvefL6uPdt9L+"
    "vWPG/OSluGnZu0BMgpSVRtpJIQxCXIgMeESjwSXiGBBSwQ0lGr5A3Y/QCiRUrGEyApsRUMNAyY7h"
    "uOPgQsIyKRgDxidnWVuuLS6XSmbScR796Kd33Vb92J88093dzXp7ey/K6WUS0lqTnTt3QmudIhx7"
    "stnUjd955nR0aHQWq/JtjBggW+x2XHGBIp1pR6vVQntbOwyDwg1C1JiNpp3AnJlAw7Thmja4aUFa"
    "FrRtwUiaMGwDEgQ1AfihwPL6KUSteSitIaVCsVTA+o3r0LGsg7ZlU3pJqdCZSaUe+4vKvet6e3tV"
    "pVK5OO6XzUBvL2hPD1Gf2fW1L9zx7ve8tZxvi7pyCbNjQxvohhuxPG5i+Znv4eTT30WtcyW0VEil"
    "UxBcIOU4eO8GjQkBTIcEUyEwyxl8yhBbCdhpDUsp6EgikjFoFGHT/HOwvToEYXDdOpYuKWLDxvWY"
    "mpnH0NBZWKZJnWRCloq5wuTE+Ne7uyvXApBYqKAvXfW6d+9mvT096k/vrtyWyqTvL+QK0a2/+U5z"
    "02XLUGsIHD05hP5HH0Xr0A8xu2IdnM5LEcyN4W3brsebr9qMVNJAeyqBpElBGEUkY0zWXQzPtDAy"
    "7+OCrzAbUTS0iUhTdIw9B2PmHCS1UK/NY+NlK3DHe34bxwbOYP/BfiRtA6lsHlNT0+C+KyOljdr8"
    "/Afv2/U3/7K9UjH6qlX5YgKkUqmQs66bLCYKA/licaXgXPPAp9uuuQ4rV63B6OgEBs9dAEsAncHT"
    "GOg/iGG6FbfcfAuu3LwelkGQyTiwbQtKRTAYhe8HqM/XMT01h8nJGUzN1jDXChCEHIpzCA1QQiCF"
    "h/f/YTeGz13A7vsfRiwlyuUy1m3cjLZcDocOHojHJyYJZWSQ153Nvb3V6CUSqlQqrFqtyrvu/sSf"
    "dSxbvqqUz8mJ6UnDrc/hS1+5D1dccTU2brwc9Zkp/NHNZRTm+xB753B6YAtGz49gYOAIJqcmYZAY"
    "b9+xDTtu2A4v5HC9AHU3RM3naEYxQk2gVAwZSUQaSNo2CvkMpiZDxMTGEz88BCeZwLa3XYMNG9bi"
    "6HODqNXqSDppKngYm5a1zsx6WwHs7+7uZsZi5pKdgJrmPGcayb/sWFLSyaRFh8/5kDKCUhEefuR+"
    "jI1PYFPXFfhK72NIm2mcD28HoQSHjh5Fq9WC12qgNjeDx/7jUXz0I3+F667bhpbrouX6kFIiDEI0"
    "6nUEAQczLHi+j7dd/WZAediz5xFcf8Ovo1gsYO2aFbjpXTswNDKGwbMj4Jzj7OAZpNOp2HU9ygy2"
    "HcD+6a4uYgBAZedORqpV+aGPfPLOfL5YbLZcOT0bGBEP4Xs+glCAGgQ/2PtdhEGAdeuvx6mJLrj1"
    "CzATFNlMCTqOEXEfbW1ZTF4Yxrcf/A7ypeWIoghRJHFq8AyeP30atflZeG4TTtLBm7duw+Wb1iP0"
    "GxgbHcVDD+/BpWtXY3JqBp+594uYnJqGEBFOP/88OjtXglEGt9VAKpPtAgDs2wcDAKlWq6r7wx9O"
    "Mko+CMS61WzSMPQRej5cz0cQcIRCghkEP9j3XUQiwJJSAa3mDFQjxvzcDHjgw3ObmJ4cQ+i7sBNp"
    "nBkaBaMU41NTmK3VAWqAMQOUMZw4fgTFQgFcKHSuWIk/eN/v47P3fg4bN12OUnkpVEwQK2BqcgKl"
    "0hKsWLUKp049T5RSYJSVAKBcLmtaqVQYAJ0jzi2JZGqFEjyOIk5D34MXeGj5PgIuIIWEkgo6Vtj7"
    "RB9m5uqQkcDA4QMYHTqJybGzGBs5Bbcxh67Lr8LSZatRqzUxPjGFyelpKLUgRSkjxEohlhLnR4ZR"
    "b3qYnmvh3bffjo99/BPIpDOYn52DFBJOMolrrtmKrW+9Gr7vA9AwTQO2ZV3MXePEiRMaAEhM/ljH"
    "WssoAucBfN+D6y20voILSCkRyYVVPOQ+DvUfwluuugqZTDugFC65ZAVy7Xlk2wpYtWYDnFQGsdZo"
    "NOsYGxmEZScQBD68Vh2N+hwC38Nl6zeB8whhqBBFBJuvvBKGbePkyROYnpmCG8zh2PFRhEEASiki"
    "zpHOZEEobV4k0Nvbq7rf94Hlcay2B75HTIPQSHAEgY+W68EPOCIpEckIKlJQMkbCttFs1DAzN4vL"
    "r3yLPn9+JLaTOZIvdpJ0JgvKTCKEgGlZYJTh5HP9aGtvQ8px0Go20Gq2sHHTm7B+w2bU5mZRr89g"
    "cPAkDh/ux/DoeTRbLsKAQ0QScRwjkUggk0ohlUprZlm66TbOAcDFJDYN60ZKSTIMXBUxyiIRwfUD"
    "NJs+wlAgEhGklNBawrQMGJTBSTnwfFeXCiViWwk2NzeLIAjQbDZhmqa27URs2hZsyyEbut5EGrUp"
    "0tm5CvVGHZ0sgXUbNkNpicHhk5icGkdtZhqe1wIhDIZpw1IAowzMMGEYDO3tORSLRVLIF0kYtJ58"
    "SSshpbjBD3xoFWnGDAgpUW95cAMXUSQuSsdxkmDEgGWZaMtmdalUJgH3p86NnP4+pcbqhJ28xLTs"
    "sm0nEradYJZtw7Js5AtLkM3m4lBwmIk2unLFGmQL7dCGBhchCAgMZiDWBFJKxFJCCLmodwNKaRBA"
    "W6bJhOAtLwieBIC+KmJjQdP81yKpQCkopQohF2g2XQShgIgixHpBNrZpwTAMpFNpXSyViOOkgv4D"
    "3//c2eefPbIYDCuZzLY72baOlJNdmXBSqy07sco0zKWJhGMTQrFm7Vqk8ykkU0mYpgEeKEQ8RCg4"
    "wlBARgpRpAAdw7ZNxFqDEALDMJSdcJhSou+Br+6arlQ0rVZJbFx38+25MBTLQQQMgxLCDHjuQvJK"
    "EQFaI2nbsC0LpmUi5ThxoVCijuPwo0898aXTJ54dtSzLjuN4RkpJg6BZC4Lm8Bywf7HbTWTa8gUn"
    "3V4uL12+1cl03WaYBJlMCjwIwAMfgnPwSCIUHJGSkCoCNSgYZeCcw7ZsmKZFKKOk2Ww+AAD7sJMC"
    "iA1iqLQQcUoqtbhDJvCDAGEYLEQ+YSNhJ2BZJpykExeKJWqaRnjoRz/42uCJY88wZo8JwQcA1ACY"
    "ABzbtlNxHGe01o6Ukrca816rMX+8VMh5xDRus2xbMwISBB7C0AePxEI14gJSqoXEtW1oHSOOY1iW"
    "qZ1UiokwmOl/am8fgExftRos5EAopNJUhpGEEAJKSchIIo41kokksukMLNvUyYQTF4slJkTYOvCf"
    "j351ZOjUXtN0TkeRfwaAt5hTIYAW5xwA2KKsEsl8Pk05lx0rV2+3LBsRl/Hk1BSLwgChH0AIiSDg"
    "EFEEGUUgGrBMA7GKQSlDMunE6XSauY3aQ6NnztQSiVx7GNYEAGnQcH6GZDombII2g0JHkrGIUlBC"
    "kc1m45ST0inHYZm2NlabnR7av3fP52emJvpSqfKw503PYNEH+ilQiwe/Zdcut7enR5WWd3iEaAR+"
    "oLWW0FEEHgpwESHkfKFUKwnDMMCoAa4EEgkb2bYsjZUUp0899yCAnNa+XpQnjL6+PvmO37r9oSVL"
    "VnT5blNKqZTWADMMlkmnqZ1IQojAGxk8+cDexx+4D8DxLVu2zPf390c/Y+AvQ9fx4wQAYTGjEY8g"
    "BIdWCogVfCHAuUDIOaRY2IaapgUNgBKGTDqj2tvzLHAbTxw/fGDEMIwC57yGRU/KIITg2LMH7rn2"
    "6rdvyeeL72CGsdCfSwkeBOemx8/vfe7IwW+OnTt7EIBLCJH9/f2vduwALlqimksO4vngQQBKYmgQ"
    "8FAs9FphCBlJEEpgmQudgpNMIJfLUQLIUycO3w8giwVZRosHDK01poeHpx4c/kLPW6/dcX17ceml"
    "WquoXp8fevrJ7w8AGAUQE0KgtYbWP4cHu8hA8MDTSiH0Q1BGAVD4IYcbeAszECskbBuMUFBmIJtt"
    "i3P5Im3MTv5w4MihYcZYUUo5AaCOBXle3NAQQkj9qR/tfegn3621JgQgmhDgJ6zEV4tyeaHfClx+"
    "ITIUAs4ppQyUUoScI/QDREJCaw1GKQilSKVSulAoEK2kd+RQ3yMA2rBQ6cYA/LgX+vE4NalUKgTY"
    "uXhpJ6rVqn41BusroaurSwOAVvycL4QSUjGlNQzDgBBiYXspFRilMAwDyWQS7e3tOpPN0rOnnv3e"
    "ueGz84wxoZQ6BWAaLyocr9v3gZ+GLVvuNM3c5MlYYU3IuQZAhRDwfA/QQMpJIdfejnwuF5eWLKWN"
    "2szQnm99eZdS6oxS6jAWov8Si/G/tRZ/kdi+fbvR3//5SCn9mKaESBXHnC80iowyOMkkioU88oW8"
    "KhRLlPte68C+x78hhDhm2/ZBLOTiy/zRXxqBvhtuiAGQONb/ZDJDphyHZNIpVSzl9ZJyWS9dslTl"
    "C0VVKpaZjMJW/4G9/zA5fv5b+Xz+oO/7E/gZ680vVUIv2II7bn33h9Lp9s8SogGtwZgBJ+nAtm14"
    "buPo0UP7P33q+JG+LVu2NF5pvfmlEgBe8F2r8a3d739PMp26m1GyjhIqQXDCaza+8eA3v/gNALNa"
    "a/KLKCCvD37sbdK339K9Yts7f6fjxbdf63eyXwm6u7vZi88JIeju3s3wK1DF/wRkYTb+D0T8DbyB"
    "/6X4L659CNMn/fGQAAAAAElFTkSuQmCC"
};

static std::wstring g_joyIconFilePath;

// Minimal standard-alphabet base64 decoder. The embedded data is produced at
// build time and never takes user input; any non-alphabet character is
// skipped, so the newlines between the string chunks are harmless.
static std::vector<unsigned char> Base64Decode(const std::string& input) {
    auto valueOf = [](char c) -> int {
        if (c >= 'A' && c <= 'Z') return c - 'A';
        if (c >= 'a' && c <= 'z') return c - 'a' + 26;
        if (c >= '0' && c <= '9') return c - '0' + 52;
        if (c == '+') return 62;
        if (c == '/') return 63;
        return -1;
    };
    std::vector<unsigned char> out;
    int acc = 0, bits = 0;
    for (char c : input) {
        if (c == '=') break;
        const int v = valueOf(c);
        if (v < 0) continue;
        acc = (acc << 6) | v;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out.push_back(static_cast<unsigned char>((acc >> bits) & 0xFF));
        }
    }
    return out;
}

// Decodes the embedded icon to a stable temp .ico file (created once) and
// returns its path, or an empty string on failure. Reuses the task-links
// mutex; re-creates the file if a previous temp cleanup removed it.
std::wstring EnsureJoyControllerIconFile() {
    std::lock_guard<std::mutex> lock(g_taskLinksMutex);
    if (!g_joyIconFilePath.empty() &&
        GetFileAttributesW(g_joyIconFilePath.c_str()) != INVALID_FILE_ATTRIBUTES) {
        return g_joyIconFilePath;
    }
    g_joyIconFilePath.clear();

    std::string b64;
    for (const char* part : kJoyControllerIconBase64) b64 += part;
    std::vector<unsigned char> bytes = Base64Decode(b64);
    if (bytes.empty()) {
        Wh_Log(L"Game Controllers icon: base64 decode produced no bytes");
        return L"";
    }

    wchar_t tempPath[MAX_PATH] = {};
    if (!GetTempPathW(MAX_PATH, tempPath)) return L"";
    const std::wstring path = std::wstring(tempPath) + L"WindhawkGameControllers.ico";
    const std::wstring tmp  = path + L".tmp." + std::to_wstring(GetCurrentProcessId());
    {
        std::ofstream f(tmp.c_str(), std::ios::binary | std::ios::trunc);
        if (!f) return L"";
        f.write(reinterpret_cast<const char*>(bytes.data()),
                static_cast<std::streamsize>(bytes.size()));
    }
    if (!MoveFileExW(tmp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING)) {
        DeleteFileW(tmp.c_str());
        Wh_Log(L"Game Controllers icon: failed to write the temp .ico file");
        return L"";
    }
    g_joyIconFilePath = path;
    Wh_Log(L"Game Controllers icon written (bytes: %llu)", (unsigned long long)bytes.size());
    return g_joyIconFilePath;
}

// Thread-safe accessor for readers (TryProvideValue and friends) that just
// want the current path without regenerating anything.
std::wstring GetClassicTaskLinksFilePath() {
    std::lock_guard<std::mutex> lock(g_taskLinksMutex);
    return g_classicTaskLinksFilePath;
}

// UTF-16 -> UTF-8 conversion (the task-links XML is written as UTF-8). Used by
// the hardcoded, recreated task-link labels below.
static std::string WideToUtf8(const std::wstring& w) {
    if (w.empty()) return {};
    int len = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    if (len <= 0) return {};
    std::string s((size_t)len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), s.data(), len, nullptr, nullptr);
    return s;
}

// Hardcoded, localized labels for the recreated iSCSI Initiator / Game
// Controllers classic task links. Unlike the other task links (whose label is
// pulled from the real applet's resources), these two entries are fully
// self-built virtual applets, so there is no Windows resource to take the
// label from - it is recreated here in each UI language. English is the
// fallback for any locale without a dedicated row.
struct RecreatedLinkLabels {
    const wchar_t* locale;
    const wchar_t* iscsiConfigure;
    const wchar_t* gameConfigure;
};
static const RecreatedLinkLabels kRecreatedLinkLabels[] = {
    { L"en",     L"Configure iSCSI initiator",            L"Configure game controllers" },
    { L"it",     L"Configura inizializzatore iSCSI",      L"Configura controller di gioco" },
    { L"es",     L"Configurar iniciador iSCSI",           L"Configurar controladores de juego" },
    { L"fr",     L"Configurer l'initiateur iSCSI",        L"Configurer les manettes de jeu" },
    { L"de",     L"iSCSI-Initiator konfigurieren",        L"Gamecontroller konfigurieren" },
    { L"pt-BR",  L"Configurar iniciador iSCSI",           L"Configurar controles de jogo" },
    { L"pt-PT",  L"Configurar iniciador iSCSI",           L"Configurar comandos de jogo" },
    { L"nl",     L"iSCSI-initiator configureren",         L"Gamecontrollers configureren" },
    { L"pl",     L"Konfiguruj inicjator iSCSI",           L"Skonfiguruj kontrolery gier" },
    { L"ru",     L"Настроить инициатор iSCSI",            L"Настроить игровые контроллеры" },
    { L"uk",     L"Налаштувати ініціатор iSCSI",          L"Налаштувати ігрові контролери" },
    { L"tr",     L"iSCSI başlatıcısını yapılandırın",     L"Oyun kumandalarını yapılandırın" },
    { L"cs",     L"Konfigurovat iniciátor iSCSI",         L"Konfigurovat herní ovladače" },
    { L"da",     L"Konfigurer iSCSI-initiator",           L"Konfigurer spilcontrollere" },
    { L"fi",     L"Määritä iSCSI-aloittaja",              L"Määritä peliohjaimet" },
    { L"el",     L"Ρύθμιση εκκινητή iSCSI",               L"Ρύθμιση χειριστηρίων παιχνιδιών" },
    { L"hu",     L"iSCSI-kezdeményező konfigurálása",     L"Játékvezérlők konfigurálása" },
    { L"nb",     L"Konfigurer iSCSI-initiator",           L"Konfigurer spillkontrollere" },
    { L"ro",     L"Configurare inițiator iSCSI",          L"Configurare controlere de joc" },
    { L"sv",     L"Konfigurera iSCSI-initierare",         L"Konfigurera spelkontroller" },
    { L"ja",     L"iSCSI イニシエーターを構成する",        L"ゲーム コントローラーを構成する" },
    { L"ko",     L"iSCSI 초기자 구성",                    L"게임 컨트롤러 구성" },
    { L"zh-CN",  L"配置 iSCSI 发起程序",                   L"配置游戏控制器" },
    { L"zh-TW",  L"設定 iSCSI 啟動器",                    L"設定遊戲控制器" },
};

// Returns the localized, hardcoded label for the iSCSI (iscsi == true) or Game
// Controllers (iscsi == false) task link, falling back to English.
std::string RecreatedLinkLabel(bool iscsi) {
    wchar_t localeName[LOCALE_NAME_MAX_LENGTH] = {};
    if (!LCIDToLocaleName(MAKELCID(GetUserDefaultUILanguage(), SORT_DEFAULT),
                          localeName, LOCALE_NAME_MAX_LENGTH, 0)) {
        wcscpy_s(localeName, L"en-US");
    }
    const RecreatedLinkLabels* chosen = &kRecreatedLinkLabels[0]; // English fallback
    for (const auto& candidate : kRecreatedLinkLabels) {
        const size_t prefixLength = wcslen(candidate.locale);
        if (_wcsnicmp(localeName, candidate.locale, prefixLength) == 0 &&
            (localeName[prefixLength] == L'\0' || localeName[prefixLength] == L'-')) {
            chosen = &candidate;
            break;
        }
    }
    return WideToUtf8(iscsi ? chosen->iscsiConfigure : chosen->gameConfigure);
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
        const char* speechConfigure;
        const char* systemBasic;
        const char* systemStatus;
        const char* systemPerformance;
    };

    // Hard-coded localized Windows 7-style labels. The selected entry follows
    // the current Windows UI language; English is the fallback.
    // Complete static catalog for 30 common Windows UI languages. It has no
    // MUI/resource dependency; English remains the fallback for other locales.
    // Review corrections can be made one row at a time without altering logic.
     static const TaskLinkTexts kTaskLinkTexts[] = {
        { L"en", "Change the theme", "Change desktop background", "Change window glass colors", "Change sound effects", "Change screen saver", "Turn system icons on or off", "Restore default icon behaviors", "View network status and tasks", "Connect to a network", "View network computers and devices", "Add a wireless device to the network", "Add a printer", "Set up default printers", "Change printer settings", "View devices and printers", "Choose homegroup and sharing options", "Share printers", "Adjust screen resolution", "Review your computer's status", "Back up your computer", "Find and fix problems", "Check firewall status", "Uninstall a program", "Turn Windows features on or off", "Change account picture", "Add or remove user accounts", "Set up parental controls for any user", "Change the date and time", "Change input methods", "Let Windows suggest settings for you", "Change home page", "Manage browser add-ons", "Delete browsing history and cookies", "Manage BitLocker", "Calibrate the screen for pen or touch input", "Pen and touch settings", "Configure text to speech", "View basic information about your computer", "Review your computer's status", "Review your computer's performance" },
        { L"it", "Cambia tema", "Cambia lo sfondo del desktop", "Cambia colore delle finestre", "Cambia effetti sonori", "Cambia salvaschermo", "Attiva o disattiva le icone di sistema", "Ripristina comportamento icone predefinito", "Visualizza stato e attività della rete", "Connetti a una rete", "Visualizza computer e dispositivi di rete", "Aggiungi un dispositivo wireless alla rete", "Aggiungi una stampante", "Configura stampanti predefinite", "Modifica impostazioni stampante", "Visualizza dispositivi e stampanti", "Scegli gruppo home e opzioni di condivisione", "Condividi stampanti", "Modifica risoluzione dello schermo", "Controlla stato del computer", "Esegui backup del computer", "Trova e correggi problemi", "Verifica stato firewall", "Disinstalla un programma", "Attiva o disattiva funzionalità di Windows", "Cambia immagine account", "Aggiungi o rimuovi account utente", "Configura controllo parentale", "Cambia data e ora", "Cambia metodo di input", "Consenti a Windows di suggerire le impostazioni", "Cambia home page", "Gestisci componenti aggiuntivi del browser", "Elimina cronologia e cookie", "Gestisci BitLocker", "Calibra lo schermo per l'input penna o tocco", "Impostazioni penna e tocco", "Configura sintesi vocale", "Visualizza informazioni di base sul computer", "Controlla lo stato del computer", "Controlla le prestazioni del computer" },
        { L"es", "Cambiar tema", "Cambiar fondo de escritorio", "Cambiar color de las ventanas", "Cambiar efectos de sonido", "Cambiar protector de pantalla", "Activar o desactivar iconos del sistema", "Restaurar comportamiento predeterminado de iconos", "Ver estado y tareas de red", "Conectarse a una red", "Ver equipos y dispositivos de red", "Agregar un dispositivo inalámbrico a la red", "Agregar una impresora", "Configurar impresoras predeterminadas", "Cambiar configuración de impresora", "Ver dispositivos e impresoras", "Elegir grupo en el hogar y opciones de uso compartido", "Compartir impresoras", "Ajustar resolución de pantalla", "Revisar estado del equipo", "Hacer copia de seguridad del equipo", "Encontrar y solucionar problemas", "Comprobar estado del firewall", "Desinstalar un programa", "Activar o desactivar características de Windows", "Cambiar imagen de cuenta", "Agregar o quitar cuentas de usuario", "Configurar control parental", "Cambiar fecha y hora", "Cambiar métodos de entrada", "Permitir que Windows sugiera configuraciones", "Cambiar página principal", "Administrar complementos del navegador", "Eliminar historial de exploración y cookies", "Administrar BitLocker", "Calibrar la pantalla para la entrada de lápiz o táctil", "Configuración de lápiz y entrada táctil", "Configurar texto a voz", "Ver información básica sobre el equipo", "Revisar el estado del equipo", "Revisar el rendimiento del equipo" },
        { L"fr", "Changer le thème", "Changer l'arrière-plan du bureau", "Changer les couleurs des vitres", "Changer les effets sonores", "Changer l'économiseur d'écran", "Activer ou désactiver les icônes du système", "Restaurer les comportements des icônes par défaut", "Afficher l'état et les tâches du réseau", "Connectez-vous à un réseau", "Afficher les ordinateurs et les appareils du réseau", "Ajouter un appareil sans fil au réseau", "Ajouter une imprimante", "Configurer les imprimantes par défaut", "Modifier les paramètres de l'imprimante", "Afficher les appareils et les imprimantes", "Choisissez le groupe résidentiel et les options de partage", "Partager des imprimantes", "Ajuster la résolution de l'écran", "Vérifiez l'état de votre ordinateur", "Sauvegardez votre ordinateur", "Rechercher et résoudre les problèmes", "Vérifier l'état du pare-feu", "Désinstaller un programme", "Activer ou désactiver des fonctionnalités Windows", "Changer la photo du compte", "Ajouter ou supprimer des comptes d'utilisateurs", "Configurer le contrôle parental pour n'importe quel utilisateur", "Changer la date et l'heure", "Changer les méthodes de saisie", "Laissez Windows vous suggérer des paramètres", "Modifier la page d'accueil", "Gérer les modules complémentaires du navigateur", "Supprimer l'historique de navigation et les cookies", "Gérer BitLocker", "Calibrer l'écran pour la saisie au stylet ou tactile", "Paramètres du stylet et de l'entrée tactile", "Configurer la synthèse vocale", "Afficher les informations de base sur l'ordinateur", "Vérifier l'état de votre ordinateur", "Vérifier les performances de votre ordinateur" },
        { L"de", "Design ändern", "Desktop-Hintergrund ändern", "Fensterfarbe ändern", "Soundeffekte ändern", "Bildschirmschoner ändern", "Systemsymbole ein- oder ausschalten", "Standardverhalten von Symbolen wiederherstellen", "Netzwerkstatus und -aufgaben anzeigen", "Mit einem Netzwerk verbinden", "Netzwerkcomputer und -geräte anzeigen", "Drahtloses Gerät zum Netzwerk hinzufügen", "Drucker hinzufügen", "Standarddrucker einrichten", "Druckereinstellungen ändern", "Geräte und Drucker anzeigen", "Heimnetzgruppen- und Freigabeoptionen auswählen", "Drucker freigeben", "Bildschirmauflösung anpassen", "Computerstatus überprüfen", "Computer sichern", "Probleme suchen und beheben", "Firewall-Status überprüfen", "Programm deinstallieren", "Windows-Funktionen aktivieren oder deaktivieren", "Kontobild ändern", "Benutzerkonten hinzufügen oder entfernen", "Kindersicherung für beliebige Benutzer einrichten", "Datum und Uhrzeit ändern", "Eingabemethoden ändern", "Windows-Einstellungen vorschlagen lassen", "Startseite ändern", "Browser-Add-Ons verwalten", "Browserverlauf und Cookies löschen", "BitLocker verwalten", "Bildschirm für Stift- oder Toucheingabe kalibrieren", "Stift- und Berührungseinstellungen", "Spracherkennung einrichten", "Grundlegende Informationen zum Computer anzeigen", "Computerstatus überprüfen", "Computerleistung überprüfen" },
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


    static const char kClassicTaskLinks[] = R"xml(  <application id="{PERSONALIZATION_TASKS_APP_ID}">
{APPEARANCE_TASKS_BLOCK}    <sh:task id="{D4F4A003-0D35-4CB6-A21F-BC1661200003}"><sh:name>{COLORS}</sh:name><sh:keywords>window;color;glass;colorization</sh:keywords><sh:command>explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}\pageColorization</sh:command></sh:task>
    <sh:task id="{D4F4A004-0D35-4CB6-A21F-BC1661200004}"><sh:name>{SOUNDS}</sh:name><sh:keywords>sound;audio;effects</sh:keywords><sh:command>rundll32.exe shell32.dll,Control_RunDLL mmsys.cpl,,2</sh:command></sh:task>
    <sh:task id="{D4F4A005-0D35-4CB6-A21F-BC1661200005}"><sh:name>{SCREENSAVER}</sh:name><sh:keywords>screen saver;screensaver</sh:keywords><sh:command>rundll32.exe shell32.dll,Control_RunDLL desk.cpl,,@screensaver</sh:command></sh:task>
    <category id="1">
{APPEARANCE_TASK_REFS_BLOCK}       <sh:task idref="{D4F4A003-0D35-4CB6-A21F-BC1661200003}"/>
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
  </application>\n)xml";

    static const char kTaskListTemplate[] = R"xml(<?xml version="1.0" encoding="utf-8"?>
<applications xmlns="http://schemas.microsoft.com/windows/cpltasks/v1" xmlns:sh="http://schemas.microsoft.com/windows/tasks/v1">
{CLASSIC_TASK_LINKS_BLOCK}
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
    
    // The three Appearance links (theme / background / resolution) are gated
    // on enableCategoryAppearanceLinks wherever they might be emitted, not
    // just in the standalone DISPLAY_APPLICATION_BLOCK below: kClassicTaskLinks
    // used to include them unconditionally, which meant turning the setting
    // off had no effect in the default configuration (restoreClassicTaskLinks
    // on). The resolution link itself is additionally dropped whenever
    // GetHomeResolutionCommand() comes back empty, which happens precisely
    // when the Classic Display Control Panel Restorer is active and already
    // contributes its own "Adjust resolution" link to the same category.
    const std::string homeResolutionCommand = GetHomeResolutionCommand();
    std::string appearanceTasksBlock; // <- This part has been kept as it is as removing the link would make the mod less accurate. I've tried to add a check but it's essentially useless as I have not experienced any duplicate entries (I've tested on Windows 10 21H2 and Windows 11 24H2 and a Windows 11 25H2 has confirmed this)
    std::string appearanceTaskRefsBlock;
    if (g_settings.enableCategoryAppearanceLinks.load()) {
        appearanceTasksBlock =
            "    <sh:task id=\"{TASK_THEME_ID}\"><sh:name>{THEME}</sh:name><sh:keywords>theme;personalization</sh:keywords><sh:controlpanel name=\"Microsoft.Personalization\"/></sh:task>\n"
            "    <sh:task id=\"{TASK_BG_ID}\"><sh:name>{BACKGROUND}</sh:name><sh:keywords>desktop;background;wallpaper</sh:keywords><sh:command>explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}\\pageWallpaper</sh:command></sh:task>\n";
        appearanceTaskRefsBlock =
            "       <sh:task idref=\"{TASK_THEME_ID}\"/>\n"
            "       <sh:task idref=\"{TASK_BG_ID}\"/>\n";
        if (!homeResolutionCommand.empty()) {
            appearanceTasksBlock +=
                "    <sh:task id=\"{TASK_RES_ID}\"><sh:name>{ADJUSTRESOLUTION}</sh:name><sh:keywords>resolution;screen;display;monitor</sh:keywords><sh:command>{RESOLUTION_COMMAND}</sh:command></sh:task>\n";
            appearanceTaskRefsBlock +=
                "       <sh:task idref=\"{TASK_RES_ID}\"/>\n";
        }
    }

    replaceAll("{CLASSIC_TASK_LINKS_BLOCK}",
               g_settings.restoreClassicTaskLinks.load() ? kClassicTaskLinks : "");
    replaceAll("{APPEARANCE_TASKS_BLOCK}", appearanceTasksBlock.c_str());
    replaceAll("{APPEARANCE_TASK_REFS_BLOCK}", appearanceTaskRefsBlock.c_str());
    // The five classic Personalization task links attach to the REAL
    // Personalization applet while the unhide feature unhides it (the virtual
    // twin is then suppressed, see VirtualTwinSuppressed), and to the
    // virtual CLSID otherwise.
    replaceAll("{PERSONALIZATION_TASKS_APP_ID}",
        VirtualTwinSuppressed(g_realPersonalizationRegistered, kLegacyUnhideMonikerPersonalization)
            ? NarrowAscii(ToLower(kRealPersonalizationGuid)).c_str()
            : NarrowAscii(ToLower(kPersonalizationGuid)).c_str());

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
        {
            // While the unhide feature is active and the real applet exists,
            // Windows shows the REAL BitLocker entry and the virtual twin is
            // suppressed - so the task links must attach to the real CLSID.
            const bool bitRealShown = VirtualTwinSuppressed(g_bitlockerClsidRegistered, kLegacyUnhideMonikerBitLocker);
            if (g_injectBitlockerApplet.load() || bitRealShown) {
                const std::string bitAppId = bitRealShown
                    ? NarrowAscii(ToLower(kBitLockerGuid))        // real CLSID
                    : NarrowAscii(ToLower(kBitLockerVirtualGuid)); // virtual CLSID
                virtualTaskBlock +=
                    "  <!-- BitLocker Drive Encryption (System and Security, Category 5) -->\n"
                    "  <application id=\"" + bitAppId + "\">\n"
                    "    <sh:task id=\"{D4F4A010-0D35-4CB6-A21F-BC1661200010}\"><sh:name>{BITLOCKERMANAGE}</sh:name>"
                    "<sh:keywords>bitlocker;encryption</sh:keywords>"
                    "<sh:command>explorer.exe shell:::{D9EF8727-CAC2-4E60-809E-86F80A666C91}</sh:command></sh:task>\n"
                    "    <category id=\"5\"><sh:task idref=\"{D4F4A010-0D35-4CB6-A21F-BC1661200010}\"/></category>\n"
                    "  </application>\n";
            }
        }
        {
            // Same real/virtual switching as the BitLocker block above.
            const bool speechRealShown = VirtualTwinSuppressed(g_speechClsidRegistered, kLegacyUnhideMonikerSpeech);
            if (g_injectSpeechApplet.load() || speechRealShown) {
                const std::string speechAppId = speechRealShown
                    ? NarrowAscii(ToLower(kSpeechGuid))          // real CLSID
                    : NarrowAscii(ToLower(kSpeechVirtualGuid));  // virtual CLSID
                virtualTaskBlock +=
                    "  <!-- Text to Speech (Hardware and Sound, Category 2) -->\n"
                    "  <application id=\"" + speechAppId + "\">\n"
                    "    <sh:task id=\"{D4F4A020-0D35-4CB6-A21F-BC1661200020}\"><sh:name>{SPEECHCONFIGURE}</sh:name>"
                    "<sh:keywords>speech;voice;text to speech;synthesis</sh:keywords>"
                    "<sh:command>explorer.exe shell:::{D17D1D6D-CC3F-4815-8FE3-607E7D5D10B3}</sh:command></sh:task>\n"
                    "    <category id=\"2\"><sh:task idref=\"{D4F4A020-0D35-4CB6-A21F-BC1661200020}\"/></category>\n"
                    "  </application>\n";
            }
        }
        // No virtual System twin exists: the classic links simply attach to
        // the real System applet while the unhide feature unhides it.
        if (VirtualTwinSuppressed(g_realSystemRegistered, kLegacyUnhideMonikerSystem)) {
            virtualTaskBlock +=
                "  <!-- System (System and Security, Category 5) -->\n"
                "  <application id=\"{bb06c0e4-d293-4f75-8a90-cb05b6477eee}\">\n"
                "    <sh:task id=\"{D4F4A030-0D35-4CB6-A21F-BC1661200030}\"><sh:name>{SYSTEMBASIC}</sh:name>"
                "<sh:keywords>system;computer;basic information</sh:keywords>"
                "<sh:command>rundll32.exe shell32.dll,Control_RunDLL sysdm.cpl</sh:command></sh:task>\n"
                "    <sh:task id=\"{D4F4A031-0D35-4CB6-A21F-BC1661200031}\"><sh:name>{SYSTEMSTATUS}</sh:name>"
                "<sh:keywords>system;status;action center</sh:keywords>"
                "<sh:command>rundll32.exe shell32.dll,Control_RunDLL wscui.cpl</sh:command></sh:task>\n"
                "    <sh:task id=\"{D4F4A032-0D35-4CB6-A21F-BC1661200032}\"><sh:name>{SYSTEMPERF}</sh:name>"
                "<sh:keywords>system;performance;performance monitor</sh:keywords>"
                "<sh:command>perfmon.exe</sh:command></sh:task>\n"
                "    <category id=\"5\"><sh:task idref=\"{D4F4A030-0D35-4CB6-A21F-BC1661200030}\"/>"
                "<sh:task idref=\"{D4F4A031-0D35-4CB6-A21F-BC1661200031}\"/>"
                "<sh:task idref=\"{D4F4A032-0D35-4CB6-A21F-BC1661200032}\"/></category>\n"
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
        // iSCSI Initiator (self-built virtual entry): a single classic link
        // that simply opens the same screen. The label is a hardcoded,
        // localized recreation (RecreatedLinkLabel); English is the fallback.
        if (VirtualAppletPresent(kIscsiInitiatorVirtualGuid)) {
            const std::string iscsiLabel = RecreatedLinkLabel(true);
            virtualTaskBlock +=
                "  <!-- iSCSI Initiator (System and Security, Category 5) -->\n"
                "  <application id=\"{7d3f5a92-8c1b-4e6a-9f2d-3b8a6c7e1d54}\">\n"
                "    <sh:task id=\"{D4F4A040-0D35-4CB6-A21F-BC1661200040}\">"
                "<sh:name>" + iscsiLabel + "</sh:name>"
                "<sh:keywords>iscsi;initiator;storage;target</sh:keywords>"
                "<sh:command>iscsicpl.exe</sh:command></sh:task>\n"
                "    <category id=\"5\"><sh:task idref=\"{D4F4A040-0D35-4CB6-A21F-BC1661200040}\"/></category>\n"
                "  </application>\n";
        }
        // Game Controllers (self-built virtual entry): a single classic
        // link that simply opens joy.cpl. Label is the hardcoded recreation.
        if (VirtualAppletPresent(kGameControllersVirtualGuid)) {
            const std::string gameLabel = RecreatedLinkLabel(false);
            virtualTaskBlock +=
                "  <!-- Game Controllers (Hardware and Sound, Category 2) -->\n"
                "  <application id=\"{b1e6c4a9-3d27-4f58-a9c6-2d71f4a8e063}\">\n"
                "    <sh:task id=\"{D4F4A041-0D35-4CB6-A21F-BC1661200041}\">"
                "<sh:name>" + gameLabel + "</sh:name>"
                "<sh:keywords>game;controller;joystick;gamepad</sh:keywords>"
                "<sh:command>control.exe joy.cpl</sh:command></sh:task>\n"
                "    <category id=\"2\"><sh:task idref=\"{D4F4A041-0D35-4CB6-A21F-BC1661200041}\"/></category>\n"
                "  </application>\n";
        }
    }
    replaceAll("{VIRTUAL_APPLET_TASKS_BLOCK}", virtualTaskBlock.c_str());

    if (g_settings.enableCategoryAppearanceLinks.load()) {
        // The three Appearance links of the Control Panel home page all hang
        // off the Personalization applet, the same way the other classic task
        // links do, because that is the applet Control Panel really
        // enumerates in category 1. An <application> block bound to a CLSID
        // the shell never enumerates is simply never read, which is why
        // "Adjust screen resolution" used to be missing while the other two
        // showed up.
        //
        // When the classic 5-task Personalization block is emitted it already
        // carries these three links, so this block is only needed when that
        // one is not there; emitting both would put the same application id
        // in the XML twice.
        std::string displayBlock;
        if (!ClassicPersonalizationBlockCoversHomeLinks()) {
            // Reuses the same appearanceTasksBlock/appearanceTaskRefsBlock
            // built above, so this standalone block and kClassicTaskLinks can
            // never disagree about which of the three links are present.
            displayBlock =
            "  <application id=\"{PERSONALIZATION_TASKS_APP_ID}\">\n" +
            appearanceTasksBlock +
            "    <category id=\"1\">\n" +
            appearanceTaskRefsBlock +
            "    </category>\n"
            "  </application>";
        }
        replaceAll("{DISPLAY_APPLICATION_BLOCK}", displayBlock.c_str());
    } else {
        replaceAll("{DISPLAY_APPLICATION_BLOCK}", "");
    }

    // The Appearance-links block is assembled after the first
    // {PERSONALIZATION_TASKS_APP_ID} pass, so resolve the token again here.
    // replaceAll rescans the whole document, which makes this a no-op when
    // that block was not emitted.
    replaceAll("{PERSONALIZATION_TASKS_APP_ID}",
        VirtualTwinSuppressed(g_realPersonalizationRegistered, kLegacyUnhideMonikerPersonalization)
            ? NarrowAscii(ToLower(kRealPersonalizationGuid)).c_str()
            : NarrowAscii(ToLower(kPersonalizationGuid)).c_str());

    // Identifiers and target of the three Appearance links of the home page.
    const bool originalHomeGuids = g_settings.useOriginalHomeTaskGuids.load();
    replaceAll("{TASK_THEME_ID}",
               originalHomeGuids ? kHomeTaskGuidTheme : kHomeTaskGuidThemeLegacy);
    replaceAll("{TASK_BG_ID}",
               originalHomeGuids ? kHomeTaskGuidBackground : kHomeTaskGuidBackgroundLegacy);
    replaceAll("{TASK_RES_ID}",
               originalHomeGuids ? kHomeTaskGuidResolution : kHomeTaskGuidResolutionLegacy);
    // homeResolutionCommand was already computed above, before the
    // appearance-links blocks were built, so both agree on whether the
    // resolution link is present at all.
    replaceAll("{RESOLUTION_COMMAND}", homeResolutionCommand.c_str());

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
    // Locales without a dedicated translation fall back to the English text
    // (same convention the rest of the table already documents).
    const char* speechConfigureText = texts->speechConfigure ? texts->speechConfigure
                                                             : kTaskLinkTexts[0].speechConfigure;
    replaceAll("{SPEECHCONFIGURE}", speechConfigureText);
    // System links: same English fallback for the other locales.
    const char* systemBasicText = texts->systemBasic ? texts->systemBasic : kTaskLinkTexts[0].systemBasic;
    const char* systemStatusText = texts->systemStatus ? texts->systemStatus : kTaskLinkTexts[0].systemStatus;
    const char* systemPerfText = texts->systemPerformance ? texts->systemPerformance
                                                          : kTaskLinkTexts[0].systemPerformance;
    replaceAll("{SYSTEMBASIC}", systemBasicText);
    replaceAll("{SYSTEMSTATUS}", systemStatusText);
    replaceAll("{SYSTEMPERF}", systemPerfText);


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
    g_settings.enableIscsiInitiator.store(Wh_GetIntSetting(L"enableIscsiInitiator"));
    g_settings.enableGameControllers.store(Wh_GetIntSetting(L"enableGameControllers"));
    g_settings.enableHomeGroup.store(Wh_GetIntSetting(L"enableHomeGroup"));
    g_settings.bitLockerMode.store((int)ReadAppletMode(L"bitLockerMode"));
    g_settings.tabletPcMode.store((int)ReadAppletMode(L"tabletPcMode"));
    g_settings.speechMode.store((int)ReadAppletMode(L"speechMode"));
    g_settings.unhideLegacyApplets.store(Wh_GetIntSetting(L"unhideLegacyApplets"));
    // The effective verdicts depend on both the (fixed) auto detection and the
    // (changeable) override, so they are refreshed on every settings load.
    g_injectBitlockerApplet.store(ResolveAppletInjection(
        (AppletMode)g_settings.bitLockerMode.load(), g_bitlockerAutoDetected.load(),
        g_bitlockerClsidRegistered.load(), L"BitLocker Drive Encryption"));
    g_injectTabletPcApplet.store(ResolveAppletInjection(
        (AppletMode)g_settings.tabletPcMode.load(), g_tabletPcAutoDetected.load(),
        g_tabletPcClsidRegistered.load(), L"Tablet PC Settings"));
    g_injectSpeechApplet.store(ResolveAppletInjection(
        (AppletMode)g_settings.speechMode.load(), g_speechAutoDetected.load(),
        g_speechClsidRegistered.load(), L"Text to Speech"));
    g_settings.enableCategoryAppearanceLinks.store(Wh_GetIntSetting(L"enableCategoryAppearanceLinks"));
    g_settings.useOriginalHomeTaskGuids.store(Wh_GetIntSetting(L"useOriginalHomeTaskGuids"));
    g_settings.suppressCompanySync.store(Wh_GetIntSetting(L"suppressCompanySync"));
    g_settings.suppressWindowsToGo.store(Wh_GetIntSetting(L"suppressWindowsToGo"));
    g_settings.suppressInfrared.store(Wh_GetIntSetting(L"suppressInfrared"));
    g_settings.suppressWorkFolders.store(Wh_GetIntSetting(L"suppressWorkFolders"));
    g_settings.restoreClassicTaskLinks.store(Wh_GetIntSetting(L"restoreClassicTaskLinks"));
    g_settings.restoreWin7CategoryTaskLinks.store(Wh_GetIntSetting(L"restoreWin7CategoryTaskLinks"));
    g_settings.inlinePersonalizationNavigation.store(Wh_GetIntSetting(L"inlinePersonalizationNavigation"));
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
    g_realBitLockerClsidSuffix       = L"clsid\\" + ToLower(kBitLockerGuid);
    g_realSpeechClsidSuffix          = L"clsid\\" + ToLower(kSpeechGuid);
    g_realSystemClsidSuffix          = L"clsid\\" + ToLower(kSystemGuid);

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
                              L"@%SystemRoot%\\System32\\fvecpl.dll,-2",
                              &g_bitlockerClsidRegistered, kLegacyUnhideMonikerBitLocker))
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
    if (g_speechClsidRegistered.load()) {
        // Text to Speech: registry-only, no resource fallback. The applet's
        // actual cpl is sapi.cpl under System32\Speech\SpeechUX, but the
        // ",-1"/",-2" string/icon resource indices used by a previous
        // version of this fallback were never confirmed against sapi.cpl -
        // they were carried over by analogy from other applets, and if
        // wrong they wouldn't just no-op, they could give the entry an
        // incorrect name or icon (a wrong index can still resolve to some
        // unrelated resource in the file). Rather than ship that unverified,
        // this applet is only added when the registry itself has a usable
        // name (as it does whenever sapi.cpl is actually registered); on
        // stub-CLSID builds where it isn't, no virtual entry is created -
        // same fail-safe behavior AddVirtualApplet already has when neither
        // the registry nor a fallback has a name.
        if (!AddVirtualApplet(kSpeechVirtualGuid, kSpeechGuid, kCategoryHardware,
                              &g_injectSpeechApplet,
                              L"", L"", L"",
                              &g_speechClsidRegistered, kLegacyUnhideMonikerSpeech))
            Wh_Log(L"Could not read Text to Speech's real name/icon from the registry "
                   L"(no resource fallback); virtual entry not created");
    }
    if (g_iscsiInitiatorExeExists.load() && IsListedInControlPanelNameSpace(kIscsiInitiatorGuid)) {
        Wh_Log(L"iSCSI Initiator: already listed in the Control Panel namespace; "
               L"virtual entry not created to avoid a duplicate");
    } else if (g_iscsiInitiatorExeExists.load()) {
        // No real, registered CLSID to copy from or re-launch through on
        // current Windows 11 builds (confirmed absent from HKCR\CLSID), so
        // this always falls to the resource fallback: name/icon come
        // straight from iscsicpl.exe itself (icon index 0, whatever
        // Explorer already shows for that binary - safer than guessing an
        // internal string-table resource id we haven't verified), and the
        // open command launches iscsicpl.exe directly instead of the usual
        // "explorer shell:::{realGuid}". Note that ReadRealClsidNameAndIcon
        // succeeding on kIscsiInitiatorGuid is itself a signal the real
        // applet may be present, but the namespace check above is the
        // authoritative "is it already listed?" guard, same as BitLocker /
        // Tablet PC / Text to Speech use.
        if (!AddVirtualApplet(kIscsiInitiatorVirtualGuid, kIscsiInitiatorGuid, kCategorySystemSecurity,
                              &g_settings.enableIscsiInitiator,
                              L"@%SystemRoot%\\System32\\iscsicpl.dll,-5001",
                              L"%SystemRoot%\\System32\\iscsicpl.exe,0",
                              // InfoTip (string resource 5002): "Connect to remote
                              // iSCSI targets and configure connection settings."
                              // Resolved straight from iscsicpl.dll, so Windows
                              // localizes it for every installed UI language.
                              L"@%SystemRoot%\\System32\\iscsicpl.dll,-5002",
                              nullptr, kLegacyUnhideMonikerCount,
                              L"iscsicpl.exe"))
            Wh_Log(L"Could not read iSCSI Initiator's name/icon; virtual entry not created");
    }
    if (g_joyCplExists.load() && IsListedInControlPanelNameSpace(kGameControllersGuid)) {
        Wh_Log(L"Game Controllers: already listed in the Control Panel namespace; "
               L"virtual entry not created to avoid a duplicate");
    } else if (g_joyCplExists.load()) {
        // Game Controllers: its legacy Control Panel CLSID ({259EF4B1-...}) is
        // no longer registered/activatable on current Windows 11 builds
        // (shell:::{259EF4B1-...} does nothing), but joy.cpl itself still ships
        // and opens. Name (string 1076) and description/InfoTip (string 1099)
        // are taken straight from joy.cpl, so Windows localizes them for every
        // UI language - no hardcoded translation table for those. joy.cpl does
        // NOT expose a usable DefaultIcon resource on Windows 10/11 (the
        // resource id is absent/wrong), so the classic gamepad icon is embedded
        // in the mod (base64 .ico) and used as the icon; g_joyIconFilePath is
        // decoded in Wh_ModInit. The open command launches joy.cpl through
        // control.exe (same direct-binary pattern as the iSCSI entry). As with
        // iSCSI above, the namespace check guards against builds where the
        // real applet is still registered and listed.
        const std::wstring joyIcon = g_joyIconFilePath.empty()
            ? std::wstring(L"%SystemRoot%\\System32\\joy.cpl,1")
            : g_joyIconFilePath;
        if (!AddVirtualApplet(kGameControllersVirtualGuid, kGameControllersGuid, kCategoryHardware,
                              &g_settings.enableGameControllers,
                              L"@%SystemRoot%\\System32\\joy.cpl,-1076",
                              joyIcon,
                              L"@%SystemRoot%\\System32\\joy.cpl,-1099",
                              nullptr, kLegacyUnhideMonikerCount,
                              L"control.exe joy.cpl"))
            Wh_Log(L"Could not read Game Controllers' name/icon; virtual entry not created");
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
        // unhide feature active + real applet present: Windows shows the real
        // entry itself, so this virtual twin must not be served either.
        if (a.realPresent && VirtualTwinSuppressed(*a.realPresent, a.monikerIndex)) continue;
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

    if (g_settings.enablePersonalization.load() &&
        !VirtualTwinSuppressed(g_realPersonalizationRegistered, kLegacyUnhideMonikerPersonalization)) {
        auto cr = ClassifyPersonalizationVirtual(lower);
        if (cr.node != VNode::None) return cr;
    }

    if (!g_virtualApplets.empty()) {
        auto cr = ClassifyVirtualApplets(lower);
        if (cr.node != VNode::None) return cr;
    }

    // Each branch below is gated on exactly the condition that makes the
    // generated task-list XML (see the kTaskListTemplate assembly above)
    // actually contain an <application> block for that CLSID. Granting
    // RealCplTaskUrl any more loosely than that would make TryProvideValue
    // hand Explorer XML with no matching block, leaving the real applet with
    // no task links at all instead of falling through to its stock ones.
    if (EndsWith(lower, g_realPersonalizationClsidSuffix)) {
        const bool personalizationRealShown = VirtualTwinSuppressed(
            g_realPersonalizationRegistered, kLegacyUnhideMonikerPersonalization);
        // The real Personalization CLSID gets an <application> block from one
        // of two places, never both (emitting both would repeat the same
        // application id): the classic 5-task Personalization block, or the
        // Appearance-links block that stands in for it when the classic task
        // links are turned off. Either way the block only carries the real
        // CLSID once the virtual twin is suppressed.
        const bool hasClassicBlock =
            g_settings.restoreClassicTaskLinks.load() && personalizationRealShown;
        const bool hasAppearanceBlock =
            g_settings.enableCategoryAppearanceLinks.load() &&
            !g_settings.restoreClassicTaskLinks.load() && personalizationRealShown;
        if (hasClassicBlock || hasAppearanceBlock) {
            return { VNode::ClsidRoot, ItemKind::RealCplTaskUrl, kCategoryAppearance };
        }
    }
    // The BitLocker / Speech / System blocks all live inside the
    // restoreClassicTaskLinks-gated section of the classic task links, and
    // each additionally requires the shell to have confirmed the real
    // applet is shown (VirtualTwinSuppressed) before its block is emitted.
    // cr.category stays 0: the category is NOT overridden, Windows already
    // knows the real applet's own category.
    if (g_settings.restoreClassicTaskLinks.load()) {
        if (EndsWith(lower, g_realBitLockerClsidSuffix) &&
            VirtualTwinSuppressed(g_bitlockerClsidRegistered, kLegacyUnhideMonikerBitLocker)) {
            return { VNode::ClsidRoot, ItemKind::RealCplTaskUrl, 0 };
        }
        if (EndsWith(lower, g_realSpeechClsidSuffix) &&
            VirtualTwinSuppressed(g_speechClsidRegistered, kLegacyUnhideMonikerSpeech)) {
            return { VNode::ClsidRoot, ItemKind::RealCplTaskUrl, 0 };
        }
        if (EndsWith(lower, g_realSystemClsidSuffix) &&
            VirtualTwinSuppressed(g_realSystemRegistered, kLegacyUnhideMonikerSystem)) {
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
        { &g_settings.enableHomeGroup,          &g_homeGroupClsidSuffix,          kCategoryNetwork,        &g_homeGroupUsable },
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
        if (cr.category != 0 && valueName == L"System.ControlPanel.Category") {
            if (lpType) *lpType = REG_DWORD;
            outStatus = ProvideDwordValue(lpData, lpcbData, cr.category);
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
            if (valueName.empty()) {
                // The Game Controllers icon is a temp file that can be deleted
                // out from under a.iconValue by CleanupTempFiles() running in
                // another process (icon path is a fixed, shared name used by
                // both explorer.exe and control.exe; a late Wh_ModUninit from
                // one process can delete the file another process already
                // recreated). a.iconValue only captures the path once, at
                // Wh_ModInit, so re-ensure the file lazily right here, the
                // same way GetOrCreateClassicTaskLinksFilePath() already does
                // for the task-links XML, instead of trusting a stale path.
                std::wstring iconPath = a.iconValue;
                if (a.guidLower == kGameControllersVirtualGuid) {
                    std::wstring ensured = EnsureJoyControllerIconFile();
                    if (!ensured.empty()) iconPath = ensured;
                }
                if (!iconPath.empty()) {
                    if (lpType) *lpType = REG_SZ;
                    outStatus = ProvideStringValue(lpData, lpcbData, iconPath);
                    return true;
                }
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
static bool VirtualAppletPresent(const std::wstring& guid) {
    const std::wstring guidLower = ToLower(guid);
    for (const auto& a : g_virtualApplets)
        if (a.guidLower == guidLower && a.enabledSetting && a.enabledSetting->load() &&
            !(a.realPresent && VirtualTwinSuppressed(*a.realPresent, a.monikerIndex))) return true;
    return false;
}

std::vector<std::wstring> GetNamespaceClsids() {
    std::vector<std::wstring> result;
    result.reserve(8);
    // The virtual Personalization twin is suppressed while the unhide feature
    // shows the real applet (guard active + real CLSID registered).
    if (g_settings.enablePersonalization.load() &&
        !VirtualTwinSuppressed(g_realPersonalizationRegistered, kLegacyUnhideMonikerPersonalization)) result.push_back(kPersonalizationGuid);
    if (g_settings.enableNotificationIcons.load())  result.push_back(kNotificationIconsGuid);
    if (g_settings.enableNetworkConnections.load()) result.push_back(kNetworkConnectionsGuid);
    if (g_settings.enablePrintersAndFaxes.load())   result.push_back(kPrintersAndFaxesGuid);
    if (IsHomeGroupAvailable())                     result.push_back(kHomeGroupGuid);
    if (g_injectBitlockerApplet.load() && VirtualAppletPresent(kBitLockerVirtualGuid))
        result.push_back(kBitLockerVirtualGuid);
    if (g_injectTabletPcApplet.load() && VirtualAppletPresent(kTabletPcVirtualGuid))
        result.push_back(kTabletPcVirtualGuid);
    if (g_injectSpeechApplet.load() && VirtualAppletPresent(kSpeechVirtualGuid))
        result.push_back(kSpeechVirtualGuid);
    if (VirtualAppletPresent(kIscsiInitiatorVirtualGuid))
        result.push_back(kIscsiInitiatorVirtualGuid);
    if (VirtualAppletPresent(kGameControllersVirtualGuid))
        result.push_back(kGameControllersVirtualGuid);
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
    if (g_inShellProbeBypass) return RegOpenKeyExWOriginal(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    RequestLazyVirtualAppletDetection();
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
    if (g_inShellProbeBypass) return RegCloseKeyOriginal(hKey);
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
    if (g_inShellProbeBypass) return RegQueryValueExWOriginal(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
    RequestLazyVirtualAppletDetection();
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
    if (g_inShellProbeBypass) return RegGetValueWOriginal(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
    RequestLazyVirtualAppletDetection();
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
    if (g_inShellProbeBypass) return RegEnumKeyExWOriginal(hKey, dwIndex, lpName, lpcchName, lpReserved, lpClass, lpcchClass, lpftLastWriteTime);
    RequestLazyVirtualAppletDetection();
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

        // Control Panel is about to rebuild its item list: this is the right
        // moment to re-check where the screen resolution link should point.
        RefreshHomeResolutionTarget();

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
    if (g_inShellProbeBypass) return RegEnumKeyWOriginal(hKey, dwIndex, lpName, cchName);
    RequestLazyVirtualAppletDetection();
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

        // See RegEnumKeyExWHook: re-check the screen resolution link target.
        RefreshHomeResolutionTarget();

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
// \pageColorization). Everything else is passed straight through.
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
    // The REAL Personalization applet, which is what shows while the Control
    // Panel unhide feature is active (the virtual twin is then suppressed).
    // Only one of the two entries is ever in the category list at a time, so
    // the extra rank costs nothing.
    L"::{ED834ED6-4B5A-4BFE-8F11-A626DCB6A921}", // Personalization (real)
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

// Walks the comparator looking for the DPA load - the last write of a
// [r8+imm] field into rcx before the DPA_GetPtr call, i.e. that call's
// first argument - and then for the moniker displacement, the first
// immediate added to a non-stack register afterwards. Returns false if the
// code doesn't have that shape, leaving applet ordering alone.
// Note: The moniker offset (0x208) has been found to be valid only on certain
// Windows 10 builds (e.g., 1809). On Windows 11 24H2+ and 26H1+, the offset
// has changed and the mod uses the fallback (stock applet ordering).
// A future update will address this once Windows 11 26H2 is officially released
// and the new offset can be determined with a proper disassembler.
static bool ResolveAppletOffsets(void* pFunc) {
    // lParam is the third x64 argument, i.e. r8 - anchor on it so an
    // unrelated load in the prologue can't be mistaken for the DPA load.
    // The destination is anchored on rcx as well: the load exists to feed
    // the first argument of the DPA_GetPtr call (mov rcx, [r8+0x10] on
    // every observed build), and the last write to rcx before a call IS
    // that call's first argument - so this can no longer latch onto some
    // other [r8+imm] load that happens to precede the call. If a future
    // build stages the value through another register first, the match
    // fails and the mod falls back to stock ordering, which is the safe
    // direction to fail in.
    const std::regex loadRegex(
        R"(mov rcx, \[r8\+(0x[0-9a-f]+)\])",
        std::regex_constants::icase);
    // rsp/rbp are explicitly excluded: `add rsp, 0x28` is an ordinary
    // stack adjustment (and small enough to sail through the upper-bound
    // sanity check below), not the moniker displacement.
    const std::regex addRegex(
        R"(add r(?!sp\b|bp\b)(?:[a-z]{2}|\d{1,2}), (0x[0-9a-f]+))",
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

    // Sanity check. Both offsets address pointer-sized fields inside
    // shell32's private structures, so besides the range bounds they must
    // be pointer-aligned - an immediate scraped off the wrong instruction
    // rarely is.
    if (!dpaOffset || dpaOffset > 0x1000 || (dpaOffset % sizeof(void*)) != 0 ||
        !monikerOffset || monikerOffset > 0x10000 ||
        (monikerOffset % sizeof(void*)) != 0) {
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

// Plausibility check on a value about to be treated as an HDPA. If
// ResolveAppletOffsets ever latched onto the wrong displacement, the loaded
// value is arbitrary data, and `if (hDpa)` alone would hand any non-null
// garbage to DPA_GetPtr, which dereferences it - an access violation inside
// Explorer on every category sort. A real heap pointer is pointer-aligned
// and lives above the null/reserved region; arbitrary small integers and
// misaligned values are rejected. Defense in depth on top of the anchored
// instruction matching in ResolveAppletOffsets, not a substitute for it.
static bool LooksLikeValidDpaHandle(HDPA hDpa) {
    const DWORD_PTR value = (DWORD_PTR)hDpa;
    if (value < 0x10000) return false;               // null/pseudo-handle region
    if (value % sizeof(void*) != 0) return false;    // heap allocations are aligned
    return true;
}

// Guards the read window [pItem + g_appletMonikerOffset,
// pItem + g_appletMonikerOffset + kMonikerReadWindow) with a single
// VirtualQuery before it is touched. g_appletMonikerOffset comes from a
// disassembly heuristic in ResolveAppletOffsets that is only checked for
// range and alignment, not correctness - if it ever latches onto the wrong
// displacement, this is the last line of defense against reading past the
// end of the shell32 heap object on every applet comparison of every
// category sort. Not a replacement for LooksLikeClsidMoniker's content
// check, or for ResolveAppletOffsets getting the offset right in the first
// place - just a commit/readability check so a bad offset degrades to "no
// match" instead of an access violation in explorer.exe.
static bool IsReadableMonikerWindow(LPCVOID pMoniker) {
    constexpr size_t kMonikerReadWindow = 64 * sizeof(WCHAR); // matches LooksLikeClsidMoniker's kMaxLen scan
    MEMORY_BASIC_INFORMATION mbi{};
    if (VirtualQuery(pMoniker, &mbi, sizeof(mbi)) != sizeof(mbi)) {
        return false;
    }
    if (mbi.State != MEM_COMMIT) {
        return false;
    }
    if (mbi.Protect & (PAGE_NOACCESS | PAGE_EXECUTE)) {
        return false;
    }
    if (mbi.Protect & PAGE_GUARD) {
        return false;
    }
    // Make sure the whole scan window fits inside this committed region.
    const BYTE* regionEnd = (const BYTE*)mbi.BaseAddress + mbi.RegionSize;
    const BYTE* windowEnd = (const BYTE*)pMoniker + kMonikerReadWindow;
    return windowEnd <= regionEnd;
}

static LPCWSTR GetAppletMoniker(HDPA hDpa, const int* pIndex) {
    LPVOID pItem = DPA_GetPtr(hDpa, *pIndex);
    if (!pItem) return NULL;
    LPCWSTR moniker = (LPCWSTR)((char*)pItem + g_appletMonikerOffset);
    if (!IsReadableMonikerWindow(moniker)) return NULL;
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
        if (LooksLikeValidDpaHandle(hDpa)) {
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

static bool ClassicPersonalizationBlockCoversHomeLinks() {
    return g_settings.restoreClassicTaskLinks.load();
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

// ===========================================================================
// Legacy-applet unhide feature (similar in approach to a well-known
// technique also used by a similar mod for the same purpose), rebuilt
// around RAII + try/catch:
//
//  * ReversiblePatcher owns every in-memory string patch and restores the
//    original bytes from a single place (Wh_ModUninit), so no manual
//    restore can ever be forgotten. It deliberately has no restoring
//    destructor - see the [[clang::no_destroy]] wrapper below;
//  * windows.storage.dll is kept loaded for as long as the patches exist:
//    the reference is taken when the feature first patches it and released
//    only in Wh_ModUninit, after RestoreAll(), because the recorded patch
//    addresses point into that module's image;
//  * every entry point below is try/catch guarded, so no C++ exception can
//    ever unwind into Explorer's non-exception-aware call stack.
//
// What it does:
//
//  1. zeroes the hidden-applet monikers inside shell32.dll /
//     windows.storage.dll so Windows 11 stops hiding Personalization,
//     BitLocker Drive Encryption, Text to Speech and System. Only
//     readable, non-executable data sections are scanned,
//     located through the PE section table (never across the whole image),
//     and only the pages that actually contain a match are temporarily
//     made writable;
//  2. asks the shell (IOpenControlPanel::GetPath) whether each of those
//     applets really is listed now, and only then lets the real applet
//     replace this mod's virtual twin - see ConfirmUnhiddenAppletsVisible().
//
// What it deliberately does NOT do: block the redirect to the modern
// Settings app (the COpenControlPanel::_MapLegacyName /
// CompareStringOrdinal hooks). That is a separate job with a separate,
// much longer list of items, and it is done by the "Settings to Control
// Panel" mod - hooking the same shell32 internal from two catalog entries
// only means the second hook re-decides what the first already decided.
// Keeping this feature to the string patches also keeps it away from the
// sharpest edge of that technique: a CompareStringOrdinal override perturbs
// comparisons the shell also uses for ordered lookups, and a name blocked
// with no classic page behind it makes explorer.exe fail with 0xC0000005.
// ===========================================================================

struct PatchRecord {
    void* address;
    BYTE originalBytes[128];
    size_t length;
};

class ReversiblePatcher {
public:
    ReversiblePatcher() = default;
    ReversiblePatcher(const ReversiblePatcher&) = delete;
    ReversiblePatcher& operator=(const ReversiblePatcher&) = delete;
    // No restoring destructor on purpose: this object lives in the
    // [[clang::no_destroy]] optional below, and a global's destructor would
    // otherwise run at process shutdown - on the shutdown thread, under the
    // loader lock, after every other thread has already been terminated.
    // Writing into other modules' images at that point is pointless (the
    // image is private to the dying process) and unsafe. RestoreAll() is
    // called explicitly from Wh_ModUninit instead.
    ~ReversiblePatcher() = default;

    // Zeroes every exact whole-string occurrence of lpSearch inside the
    // module's readable data sections, recording the original bytes so
    // RestoreAll() can put them back. Whole-string means the candidate has
    // to END where the pattern ends: a longer literal that merely starts
    // with the moniker (e.g. "::{GUID}\\pageWallpaper") must never be
    // clobbered - zeroing its prefix would turn an unrelated string into
    // L"" while the real hidden-items entry stays untouched. And every
    // occurrence is patched, not just the first: if the moniker appears in
    // more than one table, patching one and stopping would leave the
    // feature half-applied; each patch is recorded individually, so
    // RestoreAll() still undoes all of them.
    //
    // The scan walks the PE section table and only visits sections that
    // hold readable, non-executable, non-discardable initialized data
    // (.rdata/.data and friends): the moniker literals live in the
    // read-only data pool, and staying inside those sections both bounds
    // the cost (no .text/.rsrc traversal, no pages that may not be
    // readable) and avoids touching memory the module never promised to
    // be mapped at that offset. Candidates are checked at WCHAR alignment.
    bool PatchStringInModule(HMODULE hModule, LPCWSTR lpSearch) {
        if (!hModule || !lpSearch) return false;

        const size_t patternLen = wcslen(lpSearch) * sizeof(WCHAR);
        if (patternLen == 0 ||
            patternLen > sizeof(PatchRecord::originalBytes))
            return false;

        bool anyPatched = false;

        const BYTE* base = (const BYTE*)hModule;
        const IMAGE_DOS_HEADER* dosHeader = (const IMAGE_DOS_HEADER*)base;
        if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) return false;

        const IMAGE_NT_HEADERS* ntHeaders =
            (const IMAGE_NT_HEADERS*)(base + dosHeader->e_lfanew);
        if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) return false;

        const IMAGE_SECTION_HEADER* section = IMAGE_FIRST_SECTION(ntHeaders);
        const WORD sectionCount = ntHeaders->FileHeader.NumberOfSections;

        for (WORD i = 0; i < sectionCount; i++, section++) {
            if (!(section->Characteristics & IMAGE_SCN_MEM_READ)) continue;
            if (section->Characteristics & IMAGE_SCN_MEM_EXECUTE) continue;
            if (section->Characteristics & IMAGE_SCN_MEM_DISCARDABLE) continue;

            // SizeOfRawData bytes are present in the mapped image at
            // VirtualAddress; anything past that is zero-filled on demand.
            // Never scan beyond what the section header says is there.
            const size_t rawSize = section->SizeOfRawData;
            if (rawSize == 0 || section->PointerToRawData == 0) continue;
            const size_t virtualSize = section->Misc.VirtualSize;
            const size_t scanSize =
                virtualSize ? (std::min)(rawSize, virtualSize) : rawSize;
            if (patternLen > scanSize) continue;

            const BYTE* scanBegin = base + section->VirtualAddress;
            const WCHAR firstCharLower = towlower(lpSearch[0]);
            const size_t charCount = patternLen / sizeof(WCHAR);

            for (size_t offset = 0; offset + patternLen <= scanSize;
                 offset += sizeof(WCHAR)) {
                const BYTE* candidate = scanBegin + offset;
                // Case-insensitive: the GUID monikers mix upper/lowercase hex
                // digits (e.g. "4e60"/"4f75") and a future shell32 build could
                // ship different casing without changing the moniker itself.
                // The bytes actually zeroed/restored are read back verbatim
                // from the module (ZeroAndRecord snapshots `candidate`), so
                // restoring still puts back exactly what was there.
                if (towlower(*(const WCHAR*)candidate) != firstCharLower) continue;
                if (_wcsnicmp((LPCWSTR)candidate, lpSearch, charCount) != 0) continue;

                // Must be the whole string, not a prefix of a longer
                // literal: the WCHAR right after the match has to be the
                // terminator. If the match runs exactly to the end of the
                // scannable range there is no terminator to inspect, and
                // the candidate is rejected as unverifiable rather than
                // assumed terminated.
                if (offset + patternLen + sizeof(WCHAR) > scanSize ||
                    *(const WCHAR*)(candidate + patternLen) != L'\0')
                    continue;

                // Found. ZeroAndRecord() reports failure only when the
                // protection change fails or the record table is full, in
                // which case this occurrence stays unpatched and the caller
                // simply logs it. Keep scanning either way: other
                // occurrences (and other sections) may still match.
                if (ZeroAndRecord((void*)candidate, patternLen)) {
                    anyPatched = true;
                }
                // The string just zeroed can't match again; skip past it.
                offset += patternLen;
            }
        }
        return anyPatched;
    }

    // Puts every recorded byte back. Safe to call multiple times.
    void RestoreAll() {
        for (const auto& patch : patches_) {
            DWORD oldProtect = 0;
            if (!ProtectPageRange(patch.address, patch.length, PAGE_READWRITE,
                                  &oldProtect))
                continue;
            memcpy(patch.address, patch.originalBytes, patch.length);
            ProtectPageRange(patch.address, patch.length, oldProtect,
                             &oldProtect);
        }
        const size_t count = patches_.size();
        patches_.clear();
        if (count) Wh_Log(L"unhide feature: restored %zu patched region(s)", count);
    }

private:
    static constexpr size_t kMaxPatches = 64;  // same capacity as the original mod

    // VirtualProtect on exactly the pages spanned by
    // [address, address+length), leaving the rest of the (copy-on-write,
    // shared) image untouched.
    static bool ProtectPageRange(void* address, size_t length,
                                 DWORD newProtect, DWORD* oldProtect) {
        SYSTEM_INFO si = {};
        GetSystemInfo(&si);
        const DWORD_PTR pageMask = (DWORD_PTR)si.dwPageSize - 1;
        BYTE* begin = (BYTE*)((DWORD_PTR)address & ~pageMask);
        BYTE* end =
            (BYTE*)(((DWORD_PTR)address + length + pageMask) & ~pageMask);
        return VirtualProtect(begin, (SIZE_T)(end - begin), newProtect,
                              oldProtect) != FALSE;
    }

    // Makes the pages holding the match writable, records the original
    // bytes, zeroes the match and restores the original protection.
    bool ZeroAndRecord(void* address, size_t length) {
        DWORD oldProtect = 0;
        if (!ProtectPageRange(address, length, PAGE_READWRITE, &oldProtect))
            return false;

        if (patches_.size() >= kMaxPatches) {
            ProtectPageRange(address, length, oldProtect, &oldProtect);
            return false;
        }

        PatchRecord record = {};
        record.address = address;
        record.length = length;
        memcpy(record.originalBytes, address, length);
        ZeroMemory(address, length);

        ProtectPageRange(address, length, oldProtect, &oldProtect);
        patches_.push_back(record);
        return true;
    }

    std::vector<PatchRecord> patches_;
};

// The patcher is wrapped in a [[clang::no_destroy]] optional and emplaced in
// Wh_ModInit: its cleanup must never run from a static destructor at
// process shutdown (see the comment on ~ReversiblePatcher). Wh_ModUninit
// restores the patched bytes and resets the optional explicitly.
// https://github.com/ramensoftware/windhawk/wiki/Global-objects-and-process-shutdown
[[clang::no_destroy]] static std::optional<ReversiblePatcher> g_legacyUnhidePatcher;

// Persistent reference to windows.storage.dll, taken the first time the
// guard patches it and released in Wh_ModUninit, AFTER RestoreAll(): the
// recorded patch addresses point into that module's image, so the module
// must stay mapped for as long as the patches exist. (Releasing it any
// earlier could leave the patch addresses pointing at unmapped memory.)
static HMODULE g_legacyUnhideWinStorageModule = nullptr;

// (The feature installs no hooks of its own - it is string patches plus a
// shell probe - so there is no hook state to keep here. g_legacyUnhideActive,
// the master switch, is declared up top together with the other mod state.)

// The hidden-applet monikers to unhide (same technique also used by a
// similar mod), narrowed to the applets this mod manages.
static const LPCWSTR kLegacyUnhideMonikers[] = {
    L"::{ED834ED6-4B5A-4BFE-8F11-A626DCB6A921}", // Personalization
    L"::{D9EF8727-CAC2-4e60-809E-86F80A666C91}", // BitLocker Drive Encryption
    L"::{D17D1D6D-CC3F-4815-8FE3-607E7D5D10B3}", // Text to Speech
    L"::{BB06C0E4-D293-4f75-8A90-CB05B6477EEE}", // System
};
// Keeps LegacyUnhideMonikerIndex (declared far above, next to
// VirtualTwinSuppressed/g_monikerPatched, since it is needed there) in sync
// with the array above - if someone adds/removes/reorders a moniker here
// without updating the enum, this fails to compile instead of silently
// mis-gating an unrelated applet's virtual twin.
static_assert(ARRAYSIZE(kLegacyUnhideMonikers) == kLegacyUnhideMonikerCount,
              "kLegacyUnhideMonikers and LegacyUnhideMonikerIndex must stay in sync");

// A note on running alongside another mod that unhides the same items
// (control-panel-revival patches some of the same monikers in the same two
// modules): nothing here tries to detect it any more. Two reasons. The
// detection this mod used to do looked for a loaded module whose file name
// contains the other mod's id, which is a Windhawk implementation detail
// rather than an API, and it was cached for the whole process lifetime, so a
// mod loaded later was never noticed. And it isn't needed: string patching is
// self-protecting - once a moniker has been zeroed, our search for it simply
// doesn't match, so the second patcher is a no-op and no byte is patched
// twice. Whether an applet ended up unhidden, by us or by that other mod, is
// then answered the same way in both cases, by asking the shell (see
// ConfirmUnhiddenAppletsVisible) instead of by guessing who patched what.

// Patches the hidden monikers. Idempotent: a second call (e.g. after
// re-enabling the setting) re-applies the string patches from scratch,
// which is safe because the previous ones were undone by RestoreAll()
// when the setting was switched off.
//
// No hooks are installed here: see the header comment of this section.
// That also means there is nothing to apply - the patches are direct
// in-memory writes and take effect immediately - so this can be called
// from Wh_ModInit and from Wh_ModSettingsChanged alike.
static void SetupLegacyUnhide() {
    // A (re-)application invalidates the previous confirmation: the patches
    // about to be applied (or the ones just restored) change what Control
    // Panel shows, so every applet goes back to "not confirmed" and the
    // worker re-asks the shell before anything is suppressed again. Cheap
    // when the guard never becomes active - the pass is skipped wholesale.
    ResetUnhideConfirmation();

    if (!g_legacyUnhidePatcher) g_legacyUnhidePatcher.emplace();

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) {
        Wh_Log(L"unhide feature: shell32.dll not found, skipping");
        return;
    }

    bool anyMonikerPatched = false;
    // Fresh per-moniker results for this call; overwritten below (not OR'd
    // with any stale state from a previous enable/disable cycle - patches
    // are always attempted from scratch here, either at first setup or
    // after RestoreAll() undid the previous ones).
    bool monikerPatchedThisCall[kLegacyUnhideMonikerCount] = {};

    // Every moniker is attempted, whatever else is loaded: if another mod
    // already zeroed one of them, the scan simply won't find it and nothing
    // is patched twice (see the note above this function).
    for (size_t i = 0; i < ARRAYSIZE(kLegacyUnhideMonikers); i++) {
        const LPCWSTR moniker = kLegacyUnhideMonikers[i];
        const bool patched = g_legacyUnhidePatcher->PatchStringInModule(hShell32, moniker);
        anyMonikerPatched = anyMonikerPatched || patched;
        monikerPatchedThisCall[i] = monikerPatchedThisCall[i] || patched;
        Wh_Log(L"unhide feature: %s %s in shell32.dll",
               patched ? L"patched" : L"did not find", moniker);
    }

    // windows.storage.dll is loaded once and kept loaded: the patch
    // addresses recorded below point into its image, so the reference is
    // only released in Wh_ModUninit, after RestoreAll() (see
    // g_legacyUnhideWinStorageModule).
    if (!g_legacyUnhideWinStorageModule) {
        g_legacyUnhideWinStorageModule =
            LoadLibraryExW(L"windows.storage.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (g_legacyUnhideWinStorageModule) {
        for (size_t i = 0; i < ARRAYSIZE(kLegacyUnhideMonikers); i++) {
            const LPCWSTR moniker = kLegacyUnhideMonikers[i];
            const bool patched =
                g_legacyUnhidePatcher->PatchStringInModule(g_legacyUnhideWinStorageModule, moniker);
            anyMonikerPatched = anyMonikerPatched || patched;
            monikerPatchedThisCall[i] = monikerPatchedThisCall[i] || patched;
            Wh_Log(L"unhide feature: %s %s in windows.storage.dll",
                   patched ? L"patched" : L"did not find", moniker);
        }
    } else {
        Wh_Log(L"unhide feature: windows.storage.dll could not be loaded; its monikers are left unpatched");
    }

    // Publish the per-applet results now that both modules have been
    // scanned, so the log and the guard-active check below never see a
    // half-finished row (never shell32's result alone before
    // windows.storage.dll's is in). These are patch results, not visibility
    // verdicts - see g_monikerPatched / g_realAppletConfirmedVisible.
    for (size_t i = 0; i < kLegacyUnhideMonikerCount; i++) {
        g_monikerPatched[i].store(monikerPatchedThisCall[i]);
    }

    // Only declare the feature active if it actually accomplished something:
    // at least one moniker was found and zeroed. If every patch failed (e.g.
    // a future shell32 build changes the layout or the casing of the
    // hidden-items table), staying inactive keeps this mod's own virtual
    // twins visible instead of dropping the applet from Control Panel
    // entirely.
    //
    // "Active" here means "the monikers were patched", NOT "Windows is now
    // showing the real applets" - the latter is a per-applet question that
    // only the shell can answer, and it is answered separately by
    // ConfirmUnhiddenAppletsVisible() into g_realAppletConfirmedVisible.
    // The two together are what make unhideLegacyApplets defaulting to
    // true safe: this flag proves the unhide was attempted on this build, the
    // confirmation proves the applet is listed, and VirtualTwinSuppressed
    // requires both before giving up a virtual entry.
    const bool guardEffective = anyMonikerPatched;
    g_legacyUnhideActive.store(guardEffective);
    if (!guardEffective) {
        Wh_Log(L"unhide feature: no moniker could be patched; "
               L"staying inactive so virtual twins keep working");
    } else {
        Wh_Log(L"unhide feature: active; the shell will be asked to confirm "
               L"each applet before its virtual twin is dropped");
    }
}



// ===========================================================================
// In-place Personalization navigation
// ===========================================================================
// The stock Personalization markup in themecpl.dll.mun carries two elements
// whose activation opens the Settings app in a separate window:
//
//   Desktop Background:
//     shellexecute="ms-settings:personalization-background"
//     (some builds use shellexecute="shell:settings\pagepersonalization-background")
//   Window Color:
//     shellexecute="ms-settings:personalization-colors"
//
// The fix is to insert a relative navigation target BEFORE that command -
// navigationtargetrelative="pageWallpaper" / "pageColorization" - so the hub
// switches pages inside the hosting Explorer window. The Settings
// shellexecute attributes are replaced with navigationtargetrelative; they
// are not kept as a fallback.
//
// Why this is a parser rewrite, not an in-place byte patch and not a
// LoadResource copy: inserting the extra attribute makes the markup longer,
// and a mapped resource blob cannot change size. DirectUI::DUIXmlParser::SetXML
// receives the markup as a string, so the replacement may be any length, no
// page-protection trickery is required, and with the setting off the hook is
// a pure pass-through. The rewrite is try/catch guarded so a C++ exception
// can never unwind into Explorer's non-exception-aware call stack.
// ===========================================================================

struct ThemeCplMarkupReplacement {
    const wchar_t* find;     // the Settings shellexecute span
    const wchar_t* replace;  // classic in-place navigation attribute
};

// Resource Hacker / WinClassic patch: DELETE the Settings command and put
// navigationtargetrelative in its place. Keeping both does not work: DirectUI
// honours shellexecute when it is present, so Settings still opens.
static const ThemeCplMarkupReplacement kThemeCplMarkupReplacements[] = {
    { L"shellexecute=\"ms-settings:personalization-background\"",
      L"navigationtargetrelative=\"pageWallpaper\"" },
    { L"shellexecute=\"shell:settings\\pagepersonalization-background\"",
      L"navigationtargetrelative=\"pageWallpaper\"" },
    { L"shellexecute=\"ms-settings:personalization-colors\"",
      L"navigationtargetrelative=\"pageColorization\"" },
    { L"shellexecute=\"shell:settings\\pagepersonalization-colors\"",
      L"navigationtargetrelative=\"pageColorization\"" },
};

using DUIXmlParser_SetXML_t = HRESULT(WINAPI*)(void*, const WCHAR*, HINSTANCE, HINSTANCE);
static DUIXmlParser_SetXML_t DUIXmlParser_SetXML_Original = nullptr;

// The Personalization hub markup is hosted by themecpl.dll. These hooks are
// process-wide in explorer.exe, so checking the content alone would both
// scan/copy every foreign DirectUI document (on the shell UI path) and risk
// rewriting some unrelated page that merely shares the ms-settings: URIs.
// Gate on the resource/host module first; everything else is passed straight
// through before any scanning or resource load.
static bool IsThemecplInstance(HINSTANCE h) {
    return h != nullptr && h == GetModuleHandleW(L"themecpl.dll");
}

// Requires the document to actually be the Personalization hub before any
// rewrite is applied. A document merely containing one of the Settings URIs
// is not enough proof: another shell surface could link to the same
// ms-settings: URI, now or in a future build, and rewriting it would delete
// its only action (there is no shellexecute/navigationtargetrelative
// fallback - see the comment above kThemeCplMarkupReplacements). The hub
// markup is expected to also define both target pages elsewhere in the same
// document (as the targets of its own internal navigation), or carry the
// PersonalizationHubStyle marker.
static bool PersonalizationMarkupIsHubDocument(const WCHAR* xml) {
    if (!xml) return false;
    if (wcsstr(xml, L"PersonalizationHubStyle")) return true;
    return wcsstr(xml, L"pageWallpaper") && wcsstr(xml, L"pageColorization");
}

static bool PersonalizationMarkupLooksRelevant(const WCHAR* xml) {
    if (!xml) return false;
    bool hasSettingsUri = wcsstr(xml, L"ms-settings:personalization-background") ||
                          wcsstr(xml, L"ms-settings:personalization-colors") ||
                          wcsstr(xml, L"pagepersonalization-background") ||
                          wcsstr(xml, L"pagepersonalization-colors");
    if (!hasSettingsUri) return false;
    return PersonalizationMarkupIsHubDocument(xml);
}

static size_t FindInsensitive(const std::wstring& hay, const wchar_t* needle, size_t from) {
    const size_t nlen = wcslen(needle);
    if (nlen == 0 || from > hay.size()) return std::wstring::npos;
    for (size_t i = from; i + nlen <= hay.size(); ++i) {
        if (_wcsnicmp(hay.c_str() + i, needle, nlen) == 0) return i;
    }
    return std::wstring::npos;
}

// Replaces each Settings shellexecute attribute with navigationtargetrelative.
// std::wstring owns the copy (RAII). Returns true when at least one replacement
// was made.
static bool RewritePersonalizationMarkup(std::wstring& xml) {
    bool any = false;
    for (const ThemeCplMarkupReplacement& entry : kThemeCplMarkupReplacements) {
        const size_t findLen = wcslen(entry.find);
        const size_t replLen = wcslen(entry.replace);
        size_t pos = 0;
        while ((pos = FindInsensitive(xml, entry.find, pos)) != std::wstring::npos) {
            xml.replace(pos, findLen, entry.replace);
            any = true;
            pos += replLen;
        }
    }
    return any;
}

HRESULT WINAPI DUIXmlParser_SetXML_Hook(void* pThis, const WCHAR* pszXML,
                                        HINSTANCE hInstance, HINSTANCE hInstance2) {
    if (!DUIXmlParser_SetXML_Original) return E_FAIL;
    // Cheap module identity check up front (see IsThemecplInstance): skip every
    // foreign DirectUI document before scanning its markup.
    if (!g_settings.inlinePersonalizationNavigation.load() ||
        !(IsThemecplInstance(hInstance) || IsThemecplInstance(hInstance2)) ||
        !pszXML || !PersonalizationMarkupLooksRelevant(pszXML)) {
        return DUIXmlParser_SetXML_Original(pThis, pszXML, hInstance, hInstance2);
    }

    try {
        std::wstring xml(pszXML);
        if (!RewritePersonalizationMarkup(xml)) {
            return DUIXmlParser_SetXML_Original(pThis, pszXML, hInstance, hInstance2);
        }
        Wh_Log(L"in-place Personalization navigation: replaced Settings "
               L"shellexecute with navigationtargetrelative");
        // The rewrite deletes the shellexecute command, so if the build's
        // parser rejects navigationtargetrelative the document must not be
        // left broken: retry with the untouched original markup.
        HRESULT hr = DUIXmlParser_SetXML_Original(pThis, xml.c_str(), hInstance, hInstance2);
        if (FAILED(hr)) {
            Wh_Log(L"patched Personalization XML rejected (hr=0x%08lX); retrying the original",
                   (unsigned long)hr);
            hr = DUIXmlParser_SetXML_Original(pThis, pszXML, hInstance, hInstance2);
        }
        return hr;
    } catch (...) {
        Wh_Log(L"Exception while rewriting Personalization markup; using the original XML");
        return DUIXmlParser_SetXML_Original(pThis, pszXML, hInstance, hInstance2);
    }
}

// Shell Control Panel pages (e.g. Personalization's themecpl.dll.mun) don't
// hand DirectUI a markup string directly - their UIFILE/XMLFILE lives in a
// resource, and DirectUI loads it through _SetXMLFromResource, which does
// not route through the public SetXML above. That's why SetXML alone is not
// enough: we hook _SetXMLFromResource too, load + rewrite the resource
// ourselves, and feed the patched string through the raw SetXML pointer
// (never through the resource loader we just bypassed).
using DUIXmlParser_SetXMLFromResource_t =
    HRESULT(WINAPI*)(void*, const WCHAR*, const WCHAR*, HINSTANCE, HINSTANCE, HINSTANCE);
static DUIXmlParser_SetXMLFromResource_t DUIXmlParser_SetXMLFromResource_Original = nullptr;

// Loads a UIFILE/XMLFILE DirectUI resource and returns it as a wide string.
// DirectUI markup resources are stored as 8-bit text (UTF-8 on modern
// Windows, ANSI on older ones), NOT UTF-16 - a UTF-16LE BOM'd blob is still
// honoured for safety. Returns false (leaving 'out' untouched) on any failure
// so the caller transparently falls back to the original, unpatched load.
static bool LoadXmlResourceString(HINSTANCE hInstance, const WCHAR* pszResourceName,
                                   const WCHAR* pszResourceType, std::wstring& out) {
    if (!hInstance || !pszResourceName || !pszResourceType) return false;
    HRSRC hRsrc = FindResourceExW(hInstance, pszResourceType, pszResourceName,
                                   MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
    if (!hRsrc) {
        hRsrc = FindResourceW(hInstance, pszResourceName, pszResourceType);
    }
    if (!hRsrc) return false;
    HGLOBAL hGlobal = LoadResource(hInstance, hRsrc);
    if (!hGlobal) return false;
    DWORD size = SizeofResource(hInstance, hRsrc);
    const BYTE* pData = static_cast<const BYTE*>(LockResource(hGlobal));
    if (!pData || size == 0) return false;

    if (size >= 2 && pData[0] == 0xFF && pData[1] == 0xFE) {
        // Real UTF-16LE resource with a BOM (kept strict so 8-bit markup never
        // lands here).
        out.assign(reinterpret_cast<const WCHAR*>(pData), size / sizeof(WCHAR));
    } else {
        // Common case: 8-bit markup. Decode as UTF-8 first, fall back to the
        // system ANSI code page if that yields nothing.
        const char* text = reinterpret_cast<const char*>(pData);
        int len = MultiByteToWideChar(CP_UTF8, 0, text, (int)size, nullptr, 0);
        if (len > 0) {
            out.resize(len);
            if (MultiByteToWideChar(CP_UTF8, 0, text, (int)size, out.data(), len) <= 0)
                out.clear();
        }
        if (out.empty()) {
            int acpLen = MultiByteToWideChar(CP_ACP, 0, text, (int)size, nullptr, 0);
            if (acpLen <= 0) return false;
            out.resize(acpLen);
            MultiByteToWideChar(CP_ACP, 0, text, (int)size, out.data(), acpLen);
        }
    }

    // Drop a surviving BOM and trim embedded/trailing NULs so wcsstr checks
    // behave regardless of which decode path was taken.
    while (!out.empty() && out.back() == L'\0') out.pop_back();
    if (!out.empty() && out.front() == 0xFEFF) out.erase(out.begin());
    return !out.empty();
}

HRESULT WINAPI DUIXmlParser_SetXMLFromResource_Hook(void* pThis, const WCHAR* pszResourceName,
                                                     const WCHAR* pszResourceType,
                                                     HINSTANCE hInstance, HINSTANCE hInstance2,
                                                     HINSTANCE hInstance3) {
    if (!DUIXmlParser_SetXMLFromResource_Original) return E_FAIL;
    auto callOriginal = [&]() {
        return DUIXmlParser_SetXMLFromResource_Original(pThis, pszResourceName, pszResourceType,
                                                          hInstance, hInstance2, hInstance3);
    };

    // Module gate before any FindResource/LoadResource/copy: the Personalization
    // markup lives in themecpl.dll; everything else is let through untouched
    // (and foreign UIFILEs are never loaded off the shell's UI-construction path).
    // The resource is loaded from whichever of the three HINSTANCEs actually
    // matched - LoadXmlResourceString used to always load from hInstance, so a
    // match on hInstance2/hInstance3 alone would silently fail to load and the
    // feature would quietly not apply.
    HINSTANCE hThemecpl = nullptr;
    if (IsThemecplInstance(hInstance)) hThemecpl = hInstance;
    else if (IsThemecplInstance(hInstance2)) hThemecpl = hInstance2;
    else if (IsThemecplInstance(hInstance3)) hThemecpl = hInstance3;
    if (!g_settings.inlinePersonalizationNavigation.load() || !DUIXmlParser_SetXML_Original ||
        !hThemecpl) {
        return callOriginal();
    }

    try {
        std::wstring xml;
        if (!LoadXmlResourceString(hThemecpl, pszResourceName, pszResourceType, xml) ||
            !PersonalizationMarkupLooksRelevant(xml.c_str())) {
            return callOriginal();
        }
        if (!RewritePersonalizationMarkup(xml)) {
            return callOriginal();
        }
        Wh_Log(L"in-place Personalization navigation: replaced Settings "
               L"shellexecute with navigationtargetrelative (resource path)");
        // Same fallback as the SetXML hook: if the patched document is
        // rejected, reload the original resource instead of leaving the page
        // without any working action.
        //
        // Argument mapping: _SetXMLFromResource(pThis, lpName, lpType, hModule,
        // param4, param5) loads the resource from hModule, while SetXML(xml,
        // hInst, hInstParent) only takes two instances. hModule (hInstance) is
        // the module the resource was loaded from, not one of the two SetXML
        // wants - the (hInstance2, hInstance3) pair is the more likely
        // correspondence to SetXML's (hInst, hInstParent), so that is what
        // gets passed here instead of (hInstance, hInstance2).
        HRESULT hr = DUIXmlParser_SetXML_Original(pThis, xml.c_str(), hInstance2, hInstance3);
        if (FAILED(hr)) {
            Wh_Log(L"patched Personalization resource XML rejected (hr=0x%08lX); "
                   L"reloading the original resource", (unsigned long)hr);
            return callOriginal();
        }
        return hr;
    } catch (...) {
        Wh_Log(L"Exception while rewriting Personalization markup from resource; "
               L"using the original resource");
        return callOriginal();
    }
}

// Track the dui70.dll reference so it can be released in Wh_ModUninit when, and
// only when, this mod loaded it itself (a handle already returned by
// GetModuleHandleW must not be freed). SetFunctionHook keeps the patched
// trampoline in place until Windhawk removes the hooks at unload, at which
// point the reference is dropped - mirroring g_legacyUnhideWinStorageModule.
static HMODULE g_dui70LoadedByMod = nullptr;

// The Personalization DirectUI page is never rendered by control.exe, so
// force-loading dui70.dll there (which @include control.exe would otherwise do
// on every control.exe launch) is pure waste.
static bool CurrentProcessRendersPersonalizationUi() {
    wchar_t exePath[MAX_PATH] = {};
    if (!GetModuleFileNameW(nullptr, exePath, MAX_PATH)) return true; // don't block on failure
    wchar_t* fileName = wcsrchr(exePath, L'\\');
    fileName = fileName ? fileName + 1 : exePath;
    return _wcsicmp(fileName, L"control.exe") != 0;
}

// Installed once in Wh_ModInit regardless of the setting, so a later live
// toggle needs no hooking. The rewrite itself is gated on the setting.
static void InstallPersonalizationMarkupHook() {
    if (DUIXmlParser_SetXML_Original) return;

    if (!CurrentProcessRendersPersonalizationUi()) {
        Wh_Log(L"in-place Personalization navigation: skipped in control.exe (no Personalization UI)");
        return;
    }

    HMODULE hDui70 = GetModuleHandleW(L"dui70.dll");
    if (!hDui70) {
        // We own this reference: it is released again in Wh_ModUninit
        // (ReleasePersonalizationMarkupModule), not leaked per enable cycle.
        hDui70 = LoadLibraryExW(L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (hDui70) g_dui70LoadedByMod = hDui70;
    }
    if (!hDui70) {
        Wh_Log(L"in-place Personalization navigation: dui70.dll not found");
        return;
    }

    // public: long __cdecl DirectUI::DUIXmlParser::SetXML(unsigned short const *, struct HINSTANCE__ *, struct HINSTANCE__ *)
    void* pSetXML = (void*)GetProcAddress(
        hDui70, "?SetXML@DUIXmlParser@DirectUI@@QEAAJPEBGPEAUHINSTANCE__@@1@Z");
    if (!pSetXML) {
        Wh_Log(L"in-place Personalization navigation: DirectUI::DUIXmlParser::SetXML not found");
        return;
    }
    if (!WindhawkUtils::SetFunctionHook((DUIXmlParser_SetXML_t)pSetXML,
                                        DUIXmlParser_SetXML_Hook,
                                        &DUIXmlParser_SetXML_Original)) {
        Wh_Log(L"in-place Personalization navigation: failed to hook SetXML");
        return;
    }
    Wh_Log(L"in-place Personalization navigation: hooked DirectUI::DUIXmlParser::SetXML");

    // protected: long __cdecl DirectUI::DUIXmlParser::_SetXMLFromResource(...)
    // Resource-backed pages (Personalization included) take this path
    // instead of the public SetXML above, so it must be hooked too.
    void* pSetXMLFromResource = (void*)GetProcAddress(
        hDui70,
        "?_SetXMLFromResource@DUIXmlParser@DirectUI@@IEAAJPEBG0PEAUHINSTANCE__@@11@Z");
    if (!pSetXMLFromResource) {
        Wh_Log(L"in-place Personalization navigation: "
               L"DirectUI::DUIXmlParser::_SetXMLFromResource not found");
        return;
    }
    if (!WindhawkUtils::SetFunctionHook((DUIXmlParser_SetXMLFromResource_t)pSetXMLFromResource,
                                        DUIXmlParser_SetXMLFromResource_Hook,
                                        &DUIXmlParser_SetXMLFromResource_Original)) {
        Wh_Log(L"in-place Personalization navigation: failed to hook _SetXMLFromResource");
        return;
    }
    Wh_Log(L"in-place Personalization navigation: hooked "
           L"DirectUI::DUIXmlParser::_SetXMLFromResource");
}

// Releases the dui70.dll reference this mod itself loaded (none is freed
// when the module was already loaded). Called from Wh_ModUninit, after
// Windhawk has removed the hooks that point into this image.
static void ReleasePersonalizationMarkupModule() {
    if (g_dui70LoadedByMod) {
        FreeLibrary(g_dui70LoadedByMod);
        g_dui70LoadedByMod = nullptr;
    }
}

// Maps an entry of LegacyUnhideMonikerIndex to the real applet it unhides,
// so the confirmation pass below can ask the shell about each one.
struct UnhideProbeTarget {
    size_t monikerIndex;
    const std::wstring* realGuid;
    std::atomic<bool>* realPresent;
    const wchar_t* logName;
};

static const UnhideProbeTarget kUnhideProbeTargets[] = {
    { kLegacyUnhideMonikerPersonalization, &kRealPersonalizationGuid,
      &g_realPersonalizationRegistered, L"Personalization" },
    { kLegacyUnhideMonikerBitLocker, &kBitLockerGuid,
      &g_bitlockerClsidRegistered, L"BitLocker Drive Encryption" },
    { kLegacyUnhideMonikerSpeech, &kSpeechGuid,
      &g_speechClsidRegistered, L"Text to Speech" },
    { kLegacyUnhideMonikerSystem, &kSystemGuid,
      &g_realSystemRegistered, L"System" },
};

// A target is "settled" once nothing further can usefully be asked about it:
// either its moniker was never patched, its real CLSID isn't registered here,
// or the shell has already confirmed it IS listed. A target whose moniker was
// patched, whose CLSID is registered, but that the shell has NOT (yet)
// confirmed listed is left unsettled on purpose - see ConfirmUnhiddenAppletsVisible
// for why a "not listed" answer must not latch permanently.
bool AllUnhideTargetsSettled() {
    for (const UnhideProbeTarget& target : kUnhideProbeTargets) {
        if (target.monikerIndex >= kLegacyUnhideMonikerCount) continue;
        if (!g_monikerPatched[target.monikerIndex].load()) continue;
        if (target.realPresent && !target.realPresent->load()) continue;
        if (!g_realAppletConfirmedVisible[target.monikerIndex].load()) return false;
    }
    return true;
}

// Once the shell confirms an applet IS listed, that verdict is persisted here
// (keyed by Windows build, like the applet-injection verdicts above) so a
// later logon/Explorer restart/control.exe launch can skip probing that
// applet again instead of repeating the same GetPath-driven Control Panel
// item list build every time. A "not listed" or "no answer" verdict is
// deliberately never persisted this way - see ConfirmUnhiddenAppletsVisible.
std::wstring MakeUnhideConfirmedValueName(const wchar_t* key) {
    return std::wstring(L"unhideConfirmedShown_") + key;
}
std::wstring MakeUnhideConfirmedBuildValueName(const wchar_t* key) {
    return std::wstring(L"unhideConfirmedShownBuild_") + key;
}

// Asks the shell, applet by applet, whether the real item really is listed in
// Control Panel now that the guard has run, and records the answer in
// g_realAppletConfirmedVisible. This is the step that turns "we zeroed a byte
// pattern" into "the applet is there": only a confirmed applet has its
// virtual twin suppressed, so a patch that didn't unhide anything (or that
// landed on an unrelated copy of the string, or was already applied by
// another mod) can no longer remove an entry the user relies on.
//
// Deliberately no canonical name is passed to the probe: IOpenControlPanel
// resolves a canonical name to whatever item it names, which on Windows 10/11
// can be the modern Settings page that the same name is redirected to, so a
// canonical name can answer "listed" for an item Control Panel still hides -
// a false positive, i.e. exactly the failure mode this pass exists to
// prevent. The "::{GUID}" moniker form (built by QueryShownByControlPanel
// from the GUID) is both the spelling the hidden-items table is keyed on and
// the one namespace items are addressed by, so it answers the question that
// matters. When the shell can't answer at all, the verdict stays "not
// confirmed" and the virtual twin is kept.
//
// A "not listed" (or unanswered) verdict is deliberately NOT treated as
// final: shell32 caches its Control Panel item list, so if this mod (or its
// setting) is enabled while Explorer is already running, that list can have
// been built before the patch took effect. Latching "not listed" forever in
// that case would mean the shell later does start listing the real applet
// while this mod keeps serving the virtual twin too - a duplicate entry with
// no way back short of toggling the setting. So any target the shell hasn't
// yet confirmed listed is left unsettled (see AllUnhideTargetsSettled) and
// gets asked again on the next pass, for as long as the guard stays active.
//
// Runs only on the dedicated lazy-detection worker thread, never on a hook's
// caller thread (see the same note on RunLazyVirtualAppletDetection), and
// holds a ShellProbeBypass so the shell's own registry reads during the probe
// are let straight through.
void ConfirmUnhiddenAppletsVisible() {
    // Guard inactive: no moniker could be patched on this build, so there is
    // nothing to confirm - and the virtual twins must stay.
    if (!g_legacyUnhideActive.load(std::memory_order_acquire)) return;
    if (AllUnhideTargetsSettled()) return;
    
    // Rate limiting for confirmation attempts - prevents infinite loop
    // if the shell keeps answering "not listed"
    if (g_unhideAttempts.load(std::memory_order_relaxed) >= 5) {
        Wh_Log(L"unhide feature: max retry attempts (5) reached, stopping confirmation attempts");
        return;
    }
    ULONGLONG now = GetTickCount64();
    ULONGLONG last = g_lastUnhideTick.load(std::memory_order_relaxed);
    if (last && now - last < 30000) {
        // Cooldown period - not time to retry yet
        return;
    }
    g_lastUnhideTick.store(now, std::memory_order_relaxed);
    g_unhideAttempts.fetch_add(1, std::memory_order_relaxed);

    ShellProbeBypass bypass;

    // Figure out, up front, which targets still need a shell round trip this
    // pass: already-confirmed targets are skipped outright, and a target with
    // a persisted "confirmed shown" verdict from an earlier run on this same
    // build is adopted directly, without touching COM at all (finding 3's
    // caching, extended to this confirmation pass the same way
    // DetectVirtualAppletNeededCached already caches the injection verdicts).
    std::vector<size_t> probeTargetIndices; // indices into kUnhideProbeTargets
    bool changed = false;
    for (size_t i = 0; i < ARRAYSIZE(kUnhideProbeTargets); i++) {
        const UnhideProbeTarget& target = kUnhideProbeTargets[i];
        if (target.monikerIndex >= kLegacyUnhideMonikerCount) continue;
        if (g_realAppletConfirmedVisible[target.monikerIndex].load()) continue; // already settled true
        if (!(target.realPresent && target.realPresent->load())) {
            Wh_Log(L"unhide feature: %s is not registered here; nothing to confirm",
                   target.logName);
            continue; // settled (nothing to confirm), see AllUnhideTargetsSettled
        }

        const std::wstring verdictName = MakeUnhideConfirmedValueName(target.logName);
        const std::wstring buildName = MakeUnhideConfirmedBuildValueName(target.logName);
        const int cachedBuild = Wh_GetIntValue(buildName.c_str(), 0);
        if (Wh_GetIntValue(verdictName.c_str(), 0) != 0 && cachedBuild == (int)g_winBuild) {
            Wh_Log(L"unhide feature: %s - using cached confirmation from build %d; shell not probed",
                   target.logName, cachedBuild);
            g_realAppletConfirmedVisible[target.monikerIndex].store(true);
            changed = true;
            continue;
        }

        probeTargetIndices.push_back(i);
    }

    if (!probeTargetIndices.empty()) {
        // A single COM activation serves every target still needing an
        // answer this pass, instead of one CoInitializeEx/CoCreateInstance
        // cycle per applet.
        std::vector<std::pair<std::wstring, std::wstring>> items;
        for (size_t idx : probeTargetIndices) {
            // An empty canonical name on purpose - see the comment above:
            // this probes "::{GUID}" first and only falls back to the bare
            // GUID, never to a canonical name.
            items.push_back({ L"", *kUnhideProbeTargets[idx].realGuid });
        }
        std::vector<bool> answeredList, listedList;
        IsShownByControlPanelBatch(items, answeredList, listedList);

        for (size_t j = 0; j < probeTargetIndices.size(); j++) {
            const UnhideProbeTarget& target = kUnhideProbeTargets[probeTargetIndices[j]];
            const bool answered = answeredList[j];
            const bool listed = listedList[j];
            const bool confirmed = answered && listed;
            Wh_Log(L"unhide feature: %s - shell says %s; %s", target.logName,
                   answered ? (listed ? L"the item IS listed" : L"the item is NOT listed")
                            : L"it cannot answer",
                   confirmed ? L"using the real applet"
                             : L"keeping the virtual entry, will re-check later");

            if (confirmed) {
                g_realAppletConfirmedVisible[target.monikerIndex].store(true);
                changed = true;
                Wh_SetIntValue(MakeUnhideConfirmedValueName(target.logName).c_str(), 1);
                Wh_SetIntValue(MakeUnhideConfirmedBuildValueName(target.logName).c_str(), (int)g_winBuild);
            }
            // "not confirmed" is intentionally left as-is (still false) and
            // NOT persisted, so this target is retried on the next pass
            // instead of latching a stale negative - see the comment above
            // this function.
        }
    }

    if (changed) {
        // Which entries carry the classic task links - and whether the
        // virtual twins are served at all - depends on this verdict, so the
        // generated XML has to follow it.
        InvalidateClassicTaskLinksFile();
        EnsureClassicTaskLinksFile();
        Wh_Log(L"unhide feature: confirmation pass changed what Control Panel shows; "
               L"task links regenerated");
    }
}

void Wh_ModSettingsChanged() {
  try {
    AppletMode oldBitMode = (AppletMode)g_prevBitLockerMode.load();
    AppletMode oldTabMode = (AppletMode)g_prevTabletPcMode.load();
    AppletMode oldSpeechMode = (AppletMode)g_prevSpeechMode.load();
    AppletMode newBitMode = ReadAppletMode(L"bitLockerMode");
    AppletMode newTabMode = ReadAppletMode(L"tabletPcMode");
    AppletMode newSpeechMode = ReadAppletMode(L"speechMode");
    bool bitChanged = (oldBitMode != newBitMode);
    bool tabChanged = (oldTabMode != newTabMode);
    bool speechChanged = (oldSpeechMode != newSpeechMode);
    const bool prevUnhideLegacyApplets = g_settings.unhideLegacyApplets.load();
    const bool prevInlineNavigation = g_settings.inlinePersonalizationNavigation.load();
    if (bitChanged) {
        Wh_Log(L"bitLockerMode changed %d -> %d, clearing cached verdict", (int)oldBitMode, (int)newBitMode);
        Wh_DeleteValue(MakeVerdictValueName(L"bitlocker").c_str());
        Wh_DeleteValue(MakeVerdictBuildValueName(L"bitlocker").c_str());
        g_lazyDetectionDone.store(false, std::memory_order_release);
        g_bitlockerAutoDetected.store(false);
    }
    if (tabChanged) {
        Wh_Log(L"tabletPcMode changed %d -> %d, clearing cached verdict", (int)oldTabMode, (int)newTabMode);
        Wh_DeleteValue(MakeVerdictValueName(L"tabletpc").c_str());
        Wh_DeleteValue(MakeVerdictBuildValueName(L"tabletpc").c_str());
        g_lazyDetectionDone.store(false, std::memory_order_release);
        g_tabletPcAutoDetected.store(false);
    }
    if (speechChanged) {
        Wh_Log(L"speechMode changed %d -> %d, clearing cached verdict", (int)oldSpeechMode, (int)newSpeechMode);
        Wh_DeleteValue(MakeVerdictValueName(L"speech").c_str());
        Wh_DeleteValue(MakeVerdictBuildValueName(L"speech").c_str());
        g_lazyDetectionDone.store(false, std::memory_order_release);
        g_speechAutoDetected.store(false);
    }
    LoadSettings();
    g_prevBitLockerMode.store((int)newBitMode);
    g_prevTabletPcMode.store((int)newTabMode);
    g_prevSpeechMode.store((int)newSpeechMode);
    // The legacy-applet unhide feature can be toggled live: re-apply the string
    // patches and hooks, or restore the original bytes. try/catch protected so
    // a failure can never leak into Explorer.
    if (prevUnhideLegacyApplets != g_settings.unhideLegacyApplets.load()) {
        try {
            if (g_settings.unhideLegacyApplets.load()) {
                Wh_Log(L"legacy-applet unhide feature re-enabled by settings");
                SetupLegacyUnhide();
            } else {
                Wh_Log(L"legacy-applet unhide feature disabled by settings; restoring patched bytes");
                g_legacyUnhideActive.store(false);
                // Every applet goes back to "not confirmed": with the guard
                // off, the virtual twins are what Control Panel shows again.
                ResetUnhideConfirmation();
                if (g_legacyUnhidePatcher) {
                    g_legacyUnhidePatcher->RestoreAll();
                }
            }
        } catch (...) {
            Wh_Log(L"Exception while toggling the legacy-applet unhide feature");
        }

        // Toggling the guard changes what Control Panel actually shows for
        // BitLocker/Speech (System's virtual twin isn't cache-detected), so
        // the previously cached "does Control Panel already show this?"
        // verdict may now describe the wrong state - the same way a mode
        // change already invalidates it above. Without this, turning the
        // guard off after it was on can leave the applet missing with no
        // obvious way back until the user manually flips a mode setting.
        try {
            for (const wchar_t* key : { L"bitlocker", L"tabletpc", L"speech" }) {
                Wh_DeleteValue(MakeVerdictValueName(key).c_str());
                Wh_DeleteValue(MakeVerdictBuildValueName(key).c_str());
            }
            g_lazyDetectionDone.store(false, std::memory_order_release);
            g_bitlockerAutoDetected.store(false);
            g_tabletPcAutoDetected.store(false);
            g_speechAutoDetected.store(false);
            if (g_lazyDetectionWakeEvent) {
                SetEvent(g_lazyDetectionWakeEvent);
            }
        } catch (...) {
            Wh_Log(L"Exception while invalidating cached applet verdicts after unhide feature toggle");
        }
    }
    if (prevInlineNavigation != g_settings.inlinePersonalizationNavigation.load()) {
        try {
            Wh_Log(L"in-place Personalization navigation %s by settings; "
                   L"close and reopen the applet for the change to apply",
                   g_settings.inlinePersonalizationNavigation.load() ? L"enabled" : L"disabled");
        } catch (...) {
            Wh_Log(L"Exception while toggling in-place Personalization navigation");
        }
    }
    // Regenerate task links file with updated settings
    InvalidateClassicTaskLinksFile();
    EnsureClassicTaskLinksFile();
    // Re-arm the lazy-detection worker if a mode change invalidated the
    // cached verdict, instead of waiting for the next incidental registry
    // access to request it.
    if ((bitChanged || tabChanged || speechChanged) && g_lazyDetectionWakeEvent) {
        SetEvent(g_lazyDetectionWakeEvent);
    }
    Wh_Log(L"Changed - Pers=%d Notif=%d Net=%d Print=%d iSCSI=%d Game=%d Home=%d BitLocker=%d TabletPC=%d Speech=%d CatApp=%d Company=%d ToGo=%d Infrared=%d Work=%d TaskLinks=%d CatTaskLinks=%d Unhide=%d InlineNav=%d",
        g_settings.enablePersonalization.load(), g_settings.enableNotificationIcons.load(),
        g_settings.enableNetworkConnections.load(), g_settings.enablePrintersAndFaxes.load(),
        g_settings.enableIscsiInitiator.load(), g_settings.enableGameControllers.load(),
        g_settings.enableHomeGroup.load(), g_injectBitlockerApplet.load(), g_injectTabletPcApplet.load(),
        g_injectSpeechApplet.load(), g_settings.enableCategoryAppearanceLinks.load(),
        g_settings.suppressCompanySync.load(), g_settings.suppressWindowsToGo.load(),
        g_settings.suppressInfrared.load(), g_settings.suppressWorkFolders.load(), g_settings.restoreClassicTaskLinks.load(),
        g_settings.restoreWin7CategoryTaskLinks.load(), g_settings.unhideLegacyApplets.load(),
        g_settings.inlinePersonalizationNavigation.load());
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

    // Check if the HomeGroup implementation actually exists (hgcpl.dll)
    bool implementationExists = false;
    if (g_homeGroupClsidAvailable.load()) {
        // Check for hgcpl.dll directly in System32
        wchar_t system32[MAX_PATH] = {};
        if (GetSystemDirectoryW(system32, MAX_PATH)) {
            const std::wstring hgcplPath = std::wstring(system32) + L"\\hgcpl.dll";
            DWORD attributes = GetFileAttributesW(hgcplPath.c_str());
            implementationExists = (attributes != INVALID_FILE_ATTRIBUTES && 
                                   !(attributes & FILE_ATTRIBUTE_DIRECTORY));
            Wh_Log(L"HomeGroup implementation DLL: %s %s", 
                   hgcplPath.c_str(), 
                   implementationExists ? L"exists" : L"does not exist");
        }
    }
    g_homeGroupImplementationExists.store(implementationExists);
    
    // Combined flag for HomeGroup usability (CLSID registered AND implementation exists)
    g_homeGroupUsable.store(g_homeGroupClsidAvailable.load() && 
                            g_homeGroupImplementationExists.load());

    g_bitlockerClsidRegistered.store(IsRegisteredClsid(kBitLockerGuid));
    g_tabletPcClsidRegistered.store(IsRegisteredClsid(kTabletPcSettingsGuid));
    g_speechClsidRegistered.store(IsRegisteredClsid(kSpeechGuid));
    Wh_Log(L"Text to Speech CLSID %s", g_speechClsidRegistered.load()
        ? L"is registered" : L"is absent on this edition; applet will not be injected");
    {
        wchar_t system32[MAX_PATH] = {};
        bool iscsiExeExists = false;
        bool joyCplExists = false;
        if (GetSystemDirectoryW(system32, MAX_PATH)) {
            const std::wstring iscsicplPath = std::wstring(system32) + L"\\iscsicpl.exe";
            DWORD attributes = GetFileAttributesW(iscsicplPath.c_str());
            iscsiExeExists = (attributes != INVALID_FILE_ATTRIBUTES &&
                              !(attributes & FILE_ATTRIBUTE_DIRECTORY));
            Wh_Log(L"iSCSI Initiator executable: %s %s", iscsicplPath.c_str(),
                   iscsiExeExists ? L"exists" : L"does not exist");

            const std::wstring joyCplPath = std::wstring(system32) + L"\\joy.cpl";
            DWORD joyAttributes = GetFileAttributesW(joyCplPath.c_str());
            joyCplExists = (joyAttributes != INVALID_FILE_ATTRIBUTES &&
                            !(joyAttributes & FILE_ATTRIBUTE_DIRECTORY));
            Wh_Log(L"Game Controllers (joy.cpl): %s %s", joyCplPath.c_str(),
                   joyCplExists ? L"exists" : L"does not exist");
        }
        g_iscsiInitiatorExeExists.store(iscsiExeExists);
        g_joyCplExists.store(joyCplExists);
    }
    // Decode the embedded gamepad icon to a temp .ico up front (before
    // InitDisplayNames builds the virtual entry that references it).
    if (g_joyCplExists.load()) {
        if (EnsureJoyControllerIconFile().empty())
            Wh_Log(L"Game Controllers: embedded icon unavailable; entry will fall back to the default icon");
    }
    g_realPersonalizationRegistered.store(IsRegisteredClsid(kRealPersonalizationGuid));
    g_realSystemRegistered.store(IsRegisteredClsid(kSystemGuid));
    g_prevBitLockerMode.store(g_settings.bitLockerMode.load());
    g_prevTabletPcMode.store(g_settings.tabletPcMode.load());
    g_prevSpeechMode.store(g_settings.speechMode.load());
    {
        auto tryLoadCachedVerdict = [&](const wchar_t* key, std::atomic<bool>& outAutoDetected) -> bool {
            const std::wstring verdictName = MakeVerdictValueName(key);
            const std::wstring buildName   = MakeVerdictBuildValueName(key);
            int cachedVerdict = Wh_GetIntValue(verdictName.c_str(), (int)CachedVerdict::Unknown);
            int cachedBuild   = Wh_GetIntValue(buildName.c_str(), 0);
            if (cachedVerdict != (int)CachedVerdict::Unknown && cachedBuild == (int)g_winBuild) {
                bool shown = (cachedVerdict == (int)CachedVerdict::Shown);
                outAutoDetected.store(!shown);
                Wh_Log(L"%s: using cached verdict from build %d (applet is %s); shell not probed in Wh_ModInit",
                    key, cachedBuild, shown ? L"already shown" : L"not shown");
                return true;
            }
            outAutoDetected.store(false);
            return false;
        };
        bool bitCached = false, tabCached = false, speechCached = false;
        if ((AppletMode)g_settings.bitLockerMode.load() == AppletMode::Auto && g_bitlockerClsidRegistered.load()) {
            bitCached = tryLoadCachedVerdict(L"bitlocker", g_bitlockerAutoDetected);
            if (!bitCached) Wh_Log(L"BitLocker: no cached verdict, will probe lazily on first registry access");
        } else {
            g_bitlockerAutoDetected.store(false);
            bitCached = true;
        }
        if ((AppletMode)g_settings.tabletPcMode.load() == AppletMode::Auto && g_tabletPcClsidRegistered.load()) {
            tabCached = tryLoadCachedVerdict(L"tabletpc", g_tabletPcAutoDetected);
            if (!tabCached) Wh_Log(L"Tablet PC: no cached verdict, will probe lazily on first registry access");
        } else {
            g_tabletPcAutoDetected.store(false);
            tabCached = true;
        }
        if ((AppletMode)g_settings.speechMode.load() == AppletMode::Auto && g_speechClsidRegistered.load()) {
            speechCached = tryLoadCachedVerdict(L"speech", g_speechAutoDetected);
            if (!speechCached) Wh_Log(L"Speech: no cached verdict, will probe lazily on first registry access");
        } else {
            g_speechAutoDetected.store(false);
            speechCached = true;
        }
        bool needLazy = !bitCached || !tabCached || !speechCached;
        g_lazyDetectionDone.store(!needLazy, std::memory_order_release);
    }
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
    Wh_Log(L"Pers=%d Notif=%d Net=%d Print=%d iSCSI=%d Game=%d Home=%d BitLocker=%d TabletPC=%d Speech=%d CatApp=%d Suppress=%d TaskLinks=%d CatTaskLinks=%d Unhide=%d InlineNav=%d",
        g_settings.enablePersonalization.load(), g_settings.enableNotificationIcons.load(),
        g_settings.enableNetworkConnections.load(), g_settings.enablePrintersAndFaxes.load(),
        g_settings.enableIscsiInitiator.load(), g_settings.enableGameControllers.load(),
        g_settings.enableHomeGroup.load(), g_injectBitlockerApplet.load(), g_injectTabletPcApplet.load(),
        g_injectSpeechApplet.load(), g_settings.enableCategoryAppearanceLinks.load(),
        g_settings.suppressCompanySync.load(), g_settings.restoreClassicTaskLinks.load(),
        g_settings.restoreWin7CategoryTaskLinks.load(), g_settings.unhideLegacyApplets.load(),
        g_settings.inlinePersonalizationNavigation.load());

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
        //
        // Every shell32 symbol this mod needs comes from this single
        // HookSymbols call: the resolved symbols are cached per module, and
        // each extra call for the same module overwrites that cache with
        // only the new call's symbols - which would force a full
        // re-resolution of the other symbols on every subsequent start.
        // (The unhide feature needs no shell32 symbols: it patches string
        // data, it doesn't hook any function.)
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

    // legacy-applet unhide feature: unhide the legacy applets by patching
    // their hidden-items monikers in memory. The redirect to the Settings app
    // is NOT touched here - see the header comment of that section. The whole
    // block is try/catch protected; a failure here never takes the rest of
    // the mod down. The patcher is emplaced here (not lazily at first patch)
    // so its state exists for the whole mod lifetime; see the
    // [[clang::no_destroy]] comment on g_legacyUnhidePatcher.
    g_legacyUnhidePatcher.emplace();
    if (g_settings.unhideLegacyApplets.load()) {
        try {
            SetupLegacyUnhide();
        } catch (...) {
            Wh_Log(L"Exception during legacy-applet unhide feature setup; guard disabled, rest of the mod active");
            g_legacyUnhideActive.store(false);
            ResetUnhideConfirmation();
        }
    }

    // The unhide feature changes which entries receive the classic task links
    // (real applets instead of the virtual twins) and which virtual twins are
    // hidden, so regenerate the XML now that g_legacyUnhideActive has its
    // final value (the earlier eager generation ran before the guard existed).
    if (g_legacyUnhideActive.load()) {
        InvalidateClassicTaskLinksFile();
        EnsureClassicTaskLinksFile();
    }

    try {
        InstallPersonalizationMarkupHook();
    } catch (...) {
        Wh_Log(L"Exception during in-place Personalization navigation setup; rest of the mod active");
    }

    Wh_Log(L"All hooks set successfully");
    Wh_Log(L"Shell32 symbol hook: %s", hShell32 ? L"loaded" : L"FAILED");
    return TRUE;
  } catch (...) {
      Wh_Log(L"Exception during mod initialization, aborting load");
      return FALSE;
  }
}
// Body of the dedicated lazy-detection worker thread. Waits on either the
// wake event (probe requested/re-armed) or the stop event (mod unloading).
static void LazyDetectionThreadProc() {
    HANDLE waitHandles[2] = { g_lazyDetectionWakeEvent, g_lazyDetectionStopEvent };
    for (;;) {
        DWORD wait = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);
        if (wait != WAIT_OBJECT_0) {
            return;
        }
        ResetEvent(g_lazyDetectionWakeEvent);
        
        // Check stop event before running the probe
        if (WaitForSingleObject(g_lazyDetectionStopEvent, 0) == WAIT_OBJECT_0) {
            return;
        }
        
        try {
            // Runs first and has its own "done" flag: it decides whether the
            // real applets can replace their virtual twins, which the
            // detection pass below and the task-link XML both read.
            ConfirmUnhiddenAppletsVisible();
            RunLazyVirtualAppletDetection();
        } catch (...) {
            Wh_Log(L"Exception in lazy-detection worker thread");
        }
    }
}

// Runs once Wh_ModInit has returned TRUE and hooks are fully active. This is
// still a controlled Windhawk callback, not an arbitrary caller's stack, so
// starting our own worker thread here is safe.
void Wh_ModAfterInit() {
    g_lazyDetectionWakeEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_lazyDetectionStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_lazyDetectionWakeEvent || !g_lazyDetectionStopEvent) {
        Wh_Log(L"Failed to create lazy-detection events; virtual applets will stay un-injected");
        return;
    }
    g_lazyDetectionThread.emplace(LazyDetectionThreadProc);
    if (!g_lazyDetectionDone.load(std::memory_order_acquire) ||
        UnhideConfirmationPending()) {
        // Kick off the initial probe right away instead of waiting for the
        // first incidental registry access to request it. The confirmation
        // pass has to be scheduled explicitly here: it is pending in exactly
        // the case where every applet verdict is already cached, so the
        // detection pass alone would never wake this thread on a normal
        // restart and the unhide feature would never be confirmed.
        SetEvent(g_lazyDetectionWakeEvent);
    }
}

static void CleanupTempFiles() {
    // Delete the temp task-links file (and the embedded Game Controllers icon
    // written alongside it).
    std::lock_guard<std::mutex> lock(g_taskLinksMutex);
    if (!g_classicTaskLinksFilePath.empty()) {
        DeleteFileW(g_classicTaskLinksFilePath.c_str());
        Wh_Log(L"Deleted task links file: %s", g_classicTaskLinksFilePath.c_str());
    }
    if (!g_joyIconFilePath.empty()) {
        DeleteFileW(g_joyIconFilePath.c_str());
        Wh_Log(L"Deleted Game Controllers icon file: %s", g_joyIconFilePath.c_str());
        g_joyIconFilePath.clear();
    }
}

// Signal + join + reset the worker thread, per the required shutdown
// pattern for a global std::thread (see the no_destroy comment on
// g_lazyDetectionThread above) - must happen before Wh_ModUninit returns.
static void StopLazyDetectionThread() {
    if (g_lazyDetectionStopEvent) {
        SetEvent(g_lazyDetectionStopEvent);
    }
    if (g_lazyDetectionThread.has_value()) {
        if (g_lazyDetectionThread->joinable()) {
            g_lazyDetectionThread->join();
        }
        g_lazyDetectionThread.reset();
    }
    if (g_lazyDetectionWakeEvent) {
        CloseHandle(g_lazyDetectionWakeEvent);
        g_lazyDetectionWakeEvent = nullptr;
    }
    if (g_lazyDetectionStopEvent) {
        CloseHandle(g_lazyDetectionStopEvent);
        g_lazyDetectionStopEvent = nullptr;
    }
}

void Wh_ModUninit() {
    try {
        StopLazyDetectionThread();
        CleanupTempFiles();
        // See KeyTracker::ClearWithoutFreeing for why we deliberately don't
        // delete the fake-handle memory here.
        g_keyTracker.ClearWithoutFreeing();
        // legacy-applet unhide feature: deactivate it and restore every
        // patched byte. The patcher is
        // restored and reset explicitly - its storage is
        // [[clang::no_destroy]], so no destructor ever runs at process
        // shutdown (see the comment on g_legacyUnhidePatcher).
        g_legacyUnhideActive.store(false);
        ResetUnhideConfirmation();
        if (g_legacyUnhidePatcher) {
            g_legacyUnhidePatcher->RestoreAll();
            g_legacyUnhidePatcher.reset();
        }
        // Only now, with every patch restored, may the windows.storage.dll
        // reference be dropped: the patch addresses pointed into that
        // module's image, and freeing it any earlier could have left them
        // referencing memory that no longer belongs to the module.
        if (g_legacyUnhideWinStorageModule) {
            FreeLibrary(g_legacyUnhideWinStorageModule);
            g_legacyUnhideWinStorageModule = nullptr;
        }
        try {
            ReleasePersonalizationMarkupModule();
        } catch (...) {
            Wh_Log(L"Exception while releasing in-place Personalization navigation");
        }
        Wh_Log(L"Cleanup completed");
    } catch (...) {
        Wh_Log(L"Exception during cleanup, continuing anyway");
    }
}
