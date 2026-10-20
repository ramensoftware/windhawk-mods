// ==WindhawkMod==
// @id              win8-network-flyout-restorer
// @name            Windows 8 Network Flyout Restorer
// @description     This mod restores the classic Windows 8 DirectUI Network Flyout (van.dll) on Windows 10 and Windows 11 (24H2+ included)
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @license         GPL-3.0
// @architecture    x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lole32 -loleaut32 -luuid -lshlwapi -lshell32 -lwinhttp -lwintrust -lcrypt32 -lbcrypt -lcomctl32 -lwlanapi -ladvapi32 -luser32 -lgdi32 -lurlmon
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 8 Network Flyout Restorer (Win11 24H2 x64)

> **Best-effort mod:** Windows 11 24H2 (`26100+`) permanently removed `van.dll`, `pnidui.dll`,
> `wlanmm.dll`, `wlanconn.dll`, `dot3mm.dll`, `rasmm.dll`, `datusage.dll` and
> `SetProxyCredential.dll` from `System32`. This mod brings back the classic Windows 8 DirectUI
> side-pane Network Flyout (`ShowVAN`) inside `explorer.exe` on a best-effort basis; future
> Windows updates may change internal shell or DirectUI behavior.

Restores the fast, lightweight **Windows 8 / 8.1 Network Flyout (`van.dll`)** and the classic
**Network system tray icon (`pnidui.dll`)** on **Windows 11 24H2 64-bit (`10.0.26100.x`)**,
without touching `C:\Windows\System32` and without UAC elevation.

## What it restores

- **Windows 8 DirectUI Network Flyout (`van.dll!ShowVAN`)** — side flyout with Wi-Fi networks,
  Ethernet adapters, VPN / Dial-up connections, Airplane mode toggle, signal bars and inline
  WPA2 / WPA3 / Enterprise password prompts (`wlanconn.dll` + `xwizards.dll`).
- **Classic Network tray icon (`pnidui.dll`)** — left click opens (or toggles) the Windows 8
  flyout; right click opens network options. If the classic icon cannot be hosted, the mod
  automatically shows its own **live fallback tray icon** (Network List Manager status) so a
  clickable network icon is always available.
- **Permissive first-run setup** — downloads the required genuine Microsoft `x64` binaries from
  the official **Microsoft Symbol Server** (`msdl.microsoft.com`) into the mod's private
  storage folder, then hydrates them (re-embeds the missing `.mun` resources, strips the MUI
  split and embeds localized `it-IT` / `en-US` strings). Every file is structurally validated
  (`MZ` + `PE\0\0` + `x64` machine). SHA-256 digests are checked and logged; by default a
  mismatch is only a warning (set *Strict artifact verification* to make it fatal).
  `WinVerifyTrust` runs as **diagnostic telemetry only**, because MSDL binaries are
  catalog-signed and do not carry an embedded Authenticode signature.
- **Multi-explorer coordination** — all `explorer.exe` instances share a named provisioning
  mutex and generation-based cache folders, so a second `explorer.exe` never fights the first
  over half-written files. Only the `explorer.exe` that owns the taskbar (`Shell_TrayWnd`)
  hosts the tray icon; the others forward flyout requests to it through a registered window
  message. If the taskbar owner changes (restart / crash), hosting automatically migrates.
- **Four trigger layers** — (1) `WindowsCreateString` interception of the modern
  `Windows.Internal.ShellExperience.NetworkFlyout` activation, (2) a mouse hook that hit-tests
  clicks on the system network icon rectangle, (3) subclassing of the classic `pnidui.dll`
  tray icon window, (4) `ms-availablenetworks:` URI interception — plus an optional
  `Win+Shift+N` hotkey.

## Requirements

- Windows 11 24H2 x64 (`10.0.26100`+), Windhawk 1.4+.
- Internet access on first load (Microsoft Symbol Server). Subsequent starts use the cache
  with zero network traffic.

## Notes

- All COM registrations this mod needs are written to per-user
  `HKCU\Software\Classes\CLSID` (tagged with a `WindhawkWin8Van` marker and removed when the
  last mod instance unloads). No `HKLM` writes, no admin rights.
- The flyout UI language follows the Windows display language (Italian / English), or can be
  forced in settings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableLegacyTrayIcon: true
  $name: "Enable classic network tray icon (pnidui.dll)"
  $description: "Hosts the classic Windows network status icon in the notification area so left-clicking opens the Windows 8 Network Flyout. If it cannot be hosted, a built-in fallback icon with live status is shown instead."
- interceptAvailableNetworksUri: true
  $name: "Redirect ms-availablenetworks to Windows 8 flyout"
  $description: "Intercepts ms-availablenetworks URI launches inside explorer.exe (shortcuts, taskbar triggers, ExplorerPatcher ReplaceVan=2) and opens van.dll instead."
- uiLanguage: auto
  $name: "Flyout UI language"
  $description: "Language used for hydrated DirectUI network strings and connection prompts."
  $options:
  - auto: "Automatic (match Windows display language - Italian / English)"
  - it: "Italiano"
  - en: "English"
- hideDuplicateWlanAdapters: false
  $name: "Hide secondary virtual Wi-Fi adapters"
  $description: "Filters extra virtual WLAN interfaces beyond the primary adapter inside the flyout when virtual hotspot or container adapters duplicate entries."
- enableHotkey: false
  $name: "Enable Win+Shift+N hotkey"
  $description: "Registers a global Win+Shift+N shortcut that opens (or closes) the Windows 8 Network Flyout."
- strictVerification: false
  $name: "Strict artifact verification"
  $description: "Requires exact SHA-256 digests for every downloaded Microsoft Symbol Server file. When off (default), files are accepted if they are structurally valid x64 PE images and digest mismatches are only logged as warnings."
*/
// ==/WindhawkModSettings==

#include <cstddef>
using std::nullptr_t;
#include <windhawk_utils.h>

#include <windows.h>
#include <bcrypt.h>
#include <sddl.h>
#include <commctrl.h>
#include <netlistmgr.h>
#include <objbase.h>
#include <objidl.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <softpub.h>
#include <urlmon.h>
#include <winhttp.h>
#include <winstring.h>
#include <wintrust.h>
#include <wlanapi.h>
#include <winternl.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cwctype>
#include <filesystem>
#include <map>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

#if defined(__clang__)
#define WH_NO_DESTROY [[clang::no_destroy]]
#else
#define WH_NO_DESTROY
#endif

namespace {

// ===========================================================================
// Constants: MSDL artifacts, managed DLLs, GUIDs, COM coclasses
// ===========================================================================

struct MsdlArtifact {
    const wchar_t* localRawName;
    const wchar_t* msdlRelativeUrl;
    const wchar_t* expectedSha256;
};

// Exact x64 binaries served by the public Microsoft Symbol Server (msdl.microsoft.com).
constexpr MsdlArtifact kMsdlArtifacts[] = {
    {
        L"raw_22621_pnidui.dll",
        L"pnidui.dll/C506D2E820b000/pnidui.dll",
        L"0F929CF218142A2AAB06D31D7FAC4DB5C419FAF153A41C7F24D7C72EF338C76E",
    },
    {
        L"raw_22621_van.dll",
        L"van.dll/C35E3F6D57000/van.dll",
        L"D7EDDACF443E55242D67D0B5A52E690DC4EA856B57D3FB34CC4D0D41D6A4806D",
    },
    {
        L"raw_17763_van.dll",
        L"van.dll/250CE7B78a000/van.dll",
        L"744F4CADF842F44224E3EEEC73B2077ED8EDCA8850FAF3DAA22EDC74A36F05FB",
    },
    {
        L"raw_17763_wlanmm.dll",
        L"wlanmm.dll/92CAC11B419000/wlanmm.dll",
        L"95C5693CF073A7D839A0786A8C64424545FEF614CA1B4ECE607607813E0687AA",
    },
    {
        L"raw_22621_wlanconn.dll",
        L"wlanconn.dll/AA74D22183000/wlanconn.dll",
        L"538F3CA113C70C9543FB977B3A7907C4EE0CE9A61A185C737BA3418CE3ABA01A",
    },
    {
        L"raw_22621_dot3mm.dll",
        L"dot3mm.dll/B15F217F33000/dot3mm.dll",
        L"EDB12003D8CD68C938E48455D350466119CD477A038BE7253F544EA32CAC41FD",
    },
    {
        L"raw_22621_rasmm.dll",
        L"rasmm.dll/A818CD5D50000/rasmm.dll",
        L"A816606119899CC645F86B3515541EEC791F7622B9AE7F28C5F28B96011BBE63",
    },
    {
        L"raw_22621_datusage.dll",
        L"datusage.dll/414A7774f000/datusage.dll",
        L"838ECC100275B0700582603773D83613A1F7DA5738E155F1CE78E344C7A2A3F4",
    },
    {
        L"raw_22621_SetProxyCredential.dll",
        L"setproxycredential.dll/8A8CF97Af000/setproxycredential.dll",
        L"87BEC59A7C1707F11320B29DA7E4E585FD9B603C47B682C3F1205FBF3B84056F",
    },
};

constexpr const wchar_t* kRuntimeDllNames[] = {
    L"pnidui.dll",
    L"van.dll",
    L"wlanmm.dll",
    L"wlanconn.dll",
    L"dot3mm.dll",
    L"rasmm.dll",
    L"datusage.dll",
    L"SetProxyCredential.dll",
};
constexpr size_t kRuntimeDllCount = ARRAYSIZE(kRuntimeDllNames);

// Hydrated runtime images live in versioned generation folders so that a
// re-hydration never has to overwrite a DLL that a running explorer.exe still
// has mapped (that always fails with a sharing violation on a mapped image).
constexpr const wchar_t* kHydrationStampName = L"hydrated_v1_1_1.txt";
constexpr const wchar_t* kVirtualRegRootSubkey =
    L"Software\\Windhawk\\Mods\\windows-8-network-flyout-restorer\\VirtualRegistry";
constexpr const wchar_t* kInstancePidValueName = L"InstancePids";
constexpr const wchar_t* kComMarkerValueName = L"WindhawkWin8Van";
constexpr const wchar_t* kProvisionMutexName = L"Local\\Windhawk_Win8NetworkFlyout_ProvisionMutex";
constexpr const wchar_t* kVanHostWndClassName = L"WindhawkWin8VanHostWnd";
constexpr const wchar_t* kVanCtrlWndClassName = L"WindhawkWin8VanCtrlWnd";
constexpr const wchar_t* kVanIpcMessageName = L"Windhawk.Win8NetworkFlyout.Show";

// {7820AE74-23E3-4229-82C1-E41CB67D5B9C} - Win11 system network tray icon GUID
constexpr GUID kModernNetworkIconGuid = {
    0x7820AE74, 0x23E3, 0x4229, {0x82, 0xC1, 0xE4, 0x1C, 0xB6, 0x7D, 0x5B, 0x9C}};

// {8B9D20EE-ACA2-4895-8B27-D2B2DFAA707D} - VAN master component key
constexpr GUID CLSID_VanMasterComponent = {
    0x8B9D20EE, 0xACA2, 0x4895, {0x8B, 0x27, 0xD2, 0xB2, 0xDF, 0xAA, 0x70, 0x7D}};

// {0DEC7D0C-BD63-4759-AA64-ADEFD70DCF9A} - CLSID_PopupWindowFactory (Windows.UI.Immersive.dll)
constexpr GUID CLSID_PopupWindowFactory = {
    0x0DEC7D0C, 0xBD63, 0x4759, {0xAA, 0x64, 0xAD, 0xEF, 0xD7, 0x0D, 0xCF, 0x9A}};

// {C2796011-81BA-4148-8FCA-C6643245113F} - pnidui.dll CLSID_NetworkTray
constexpr GUID CLSID_LegacyNetworkTray = {
    0xC2796011, 0x81BA, 0x4148, {0x8F, 0xCA, 0xC6, 0x64, 0x32, 0x45, 0x11, 0x3F}};

// {8E282AAE-ACE9-4821-83AD-849C1D08939D} - private activation IID implemented
// by pnidui.dll's CNwSSO (CLSID_NetworkTray). Verified present in the pnidui
// 22621.2605 binary (.rdata) and matched against its PDB:
//   virtual HRESULT CNwSSO::Start(DWORD flags, void** out1, DWORD* out2)
//   virtual HRESULT CNwSSO::Stop()
// pnidui 22621 does NOT implement IOleCommandTarget at all (verified: the
// IID is absent from the binary), which is why the previous
// CreateInstance(IID_IOleCommandTarget) failed with E_NOINTERFACE (0x80004002).
constexpr GUID IID_NetworkTrayPrivate = {
    0x8E282AAE, 0xACE9, 0x4821, {0x83, 0xAD, 0x84, 0x9C, 0x1D, 0x08, 0x93, 0x9D}};

// Minimal vtable view of pnidui.dll's private NetworkTray interface. On x64
// (RCX=this, RDX/R8/R9=args) Activate/Cleanup below are ABI-identical to
// CNwSSO::Start/Stop. Same activation pattern as the community
// "restore-legacy-network-tray" mod (proven against pnidui 22000; the IID
// and the vtable layout are unchanged in 22621).
struct INetworkTrayPrivate : IUnknown {
    // CNwSSO::Start(DWORD flags, void** out1, DWORD* out2)
    virtual HRESULT STDMETHODCALLTYPE Activate(DWORD flags, void* out1, void* out2) = 0;
    // CNwSSO::Stop()
    virtual HRESULT STDMETHODCALLTYPE Cleanup() = 0;
};

// {5326DDDC-EC38-428D-B219-CE6DADB35DE3} - van.dll CGenericCFETask (XWizard task)
constexpr GUID CLSID_VanGenericCFETask = {
    0x5326DDDC, 0xEC38, 0x428D, {0xB2, 0x19, 0xCE, 0x6D, 0xAD, 0xB3, 0x5D, 0xE3}};

// {38A98528-6CBF-4CA9-8DC0-B1E1D10F7B1B} - van.dll View Available Networks
constexpr GUID CLSID_VanViewAvailableNetworks = {
    0x38A98528, 0x6CBF, 0x4CA9, {0x8D, 0xC0, 0xB1, 0xE1, 0xD1, 0x0F, 0x7B, 0x1B}};

// Media managers enumerated from HKLM\SYSTEM\CurrentControlSet\Control\VAN
// {1B02C1F5-555B-4802-96A7-ADDDCCBCA38A} - dot3mm.dll (Ordinal 100)
constexpr GUID CLSID_Dot3MediaManager = {
    0x1B02C1F5, 0x555B, 0x4802, {0x96, 0xA7, 0xAD, 0xDD, 0xCC, 0xBC, 0xA3, 0x8A}};

// {6705C562-0AE7-40EA-8474-F39DAB1813D0} - rasmm.dll (Ordinal 200)
constexpr GUID CLSID_RasMediaManager = {
    0x6705C562, 0x0AE7, 0x40EA, {0x84, 0x74, 0xF3, 0x9D, 0xAB, 0x18, 0x13, 0xD0}};

// {E1DE74AD-C368-4104-ADB1-57D00577247A} - wlanmm.dll (Ordinal 300)
constexpr GUID CLSID_WlanMediaManager = {
    0xE1DE74AD, 0xC368, 0x4104, {0xAD, 0xB1, 0x57, 0xD0, 0x05, 0x77, 0x24, 0x7A}};

// {DD06A84F-83BD-4D01-8AB9-2389FEA0869E} - wlanmm.dll CDot11AdHocMgr
constexpr GUID CLSID_WlanAdHocManager = {
    0xDD06A84F, 0x83BD, 0x4D01, {0x8A, 0xB9, 0x23, 0x89, 0xFE, 0xA0, 0x86, 0x9E}};

// {B22C6CFA-D5A9-4219-AE22-44E03AC7054B} - wlanmm.dll AvNet class
constexpr GUID CLSID_WlanAvNet = {
    0xB22C6CFA, 0xD5A9, 0x4219, {0xAE, 0x22, 0x44, 0xE0, 0x3A, 0xC7, 0x05, 0x4B}};

// wlanconn.dll COM & XWizard classes
constexpr GUID CLSID_WlanPreCFEPage = {
    0xD34BD155, 0x6D84, 0x443E, {0x83, 0xEE, 0x04, 0xC7, 0x68, 0x23, 0x77, 0xE4}};
constexpr GUID CLSID_WlanUnsecuredConnectPage = {
    0xD34BD150, 0x6D84, 0x443E, {0x83, 0xEE, 0x04, 0xC7, 0x68, 0x23, 0x77, 0xE4}};
constexpr GUID CLSID_WlanConnectingProgressPage = {
    0xBC271022, 0x28CD, 0x468A, {0xBB, 0xB0, 0xEF, 0x70, 0x91, 0xD8, 0x62, 0x5E}};
constexpr GUID CLSID_WlanCredentialPage = {
    0x102D1D13, 0xBD88, 0x435F, {0x99, 0xC1, 0xDE, 0x69, 0xF1, 0x88, 0x61, 0x68}};
constexpr GUID CLSID_WlanNullSsidPage = {
    0x820825FD, 0x1358, 0x4705, {0x8F, 0xEB, 0x56, 0xB5, 0xCE, 0xE8, 0xBF, 0xD5}};
constexpr GUID CLSID_WlanCouldNotConnectPage = {
    0xE0CBC546, 0x8950, 0x43DD, {0x99, 0xA9, 0xA4, 0x18, 0xDB, 0x7D, 0xD0, 0xB5}};
constexpr GUID CLSID_WlanSetNetworkLocationPage = {
    0xFB2FDC4F, 0x7DD0, 0x4ACE, {0x9B, 0x1E, 0x36, 0xE5, 0xC2, 0xF9, 0x8D, 0x27}};
constexpr GUID CLSID_WlanManualConnectPage = {
    0x9D71A98F, 0x1E45, 0x42EC, {0xAF, 0x3D, 0xFA, 0x13, 0xBE, 0xFB, 0x95, 0x5C}};
constexpr GUID CLSID_WlanWisprSignupPage = {
    0x58DDEF94, 0x53B5, 0x4BD5, {0xAF, 0x79, 0xC4, 0x58, 0x1B, 0xBD, 0xF7, 0xAC}};
constexpr GUID CLSID_WlanEnterProxyCredsPage = {
    0x7C28841A, 0xAB97, 0x498E, {0x98, 0xC4, 0xF6, 0xF1, 0x43, 0x36, 0x6F, 0xBC}};
constexpr GUID CLSID_WlanNonUiPage = {
    0x9DD35758, 0xAAF5, 0x4894, {0xAD, 0xAA, 0x77, 0xA4, 0x92, 0xF5, 0x4E, 0x0A}};
constexpr GUID CLSID_WlanBalloonHandler = {
    0x348C9CFF, 0xCB90, 0x4024, {0xAE, 0x22, 0xF1, 0x72, 0x59, 0xA3, 0x93, 0x4B}};

// Network List Manager (fallback tray icon status)
constexpr GUID kCLSID_NetworkListManager = {
    0xDCB00C01, 0x570F, 0x4A9B, {0x8D, 0x69, 0x19, 0x9F, 0xDB, 0xA5, 0x72, 0x3B}};
constexpr GUID kIID_INetworkListManager = {
    0xDCB00000, 0x570F, 0x4A9B, {0x8D, 0x69, 0x19, 0x9F, 0xDB, 0xA5, 0x72, 0x3B}};

// The Windows SDK names the INetworkListManager property accessors
// get_IsConnectedToInternet / get_IsConnected, while the mingw-w64 widl
// headers drop the get_ prefix. SFINAE picks whichever one exists.
template <typename T>
auto NlmIsConnectedToInternet(T* nlm, VARIANT_BOOL* out, int)
    -> decltype(nlm->get_IsConnectedToInternet(out)) {
    return nlm->get_IsConnectedToInternet(out);
}
template <typename T>
auto NlmIsConnectedToInternet(T* nlm, VARIANT_BOOL* out, long)
    -> decltype(nlm->IsConnectedToInternet(out)) {
    return nlm->IsConnectedToInternet(out);
}
template <typename T>
auto NlmIsConnected(T* nlm, VARIANT_BOOL* out, int)
    -> decltype(nlm->get_IsConnected(out)) {
    return nlm->get_IsConnected(out);
}
template <typename T>
auto NlmIsConnected(T* nlm, VARIANT_BOOL* out, long)
    -> decltype(nlm->IsConnected(out)) {
    return nlm->IsConnected(out);
}

struct InProcComEntry {
    GUID clsid;
    const wchar_t* dllBaseName;
    const wchar_t* threadingModel;
    const wchar_t* description;
};

constexpr InProcComEntry kInProcComTable[] = {
    {CLSID_LegacyNetworkTray, L"pnidui.dll", L"Both", L"Network Tray"},
    {CLSID_VanGenericCFETask, L"van.dll", L"Apartment", L"Connect to a network"},
    {CLSID_VanViewAvailableNetworks, L"van.dll", L"Apartment", L"View Available Networks"},
    {CLSID_Dot3MediaManager, L"dot3mm.dll", L"Both", L"Wired Network Media Manager"},
    {CLSID_RasMediaManager, L"rasmm.dll", L"Both", L"RAS Media Manager"},
    {CLSID_WlanMediaManager, L"wlanmm.dll", L"Both", L"Wireless Network Media Manager"},
    {CLSID_WlanAdHocManager, L"wlanmm.dll", L"Both", L"Dot11 AdHoc Manager"},
    {CLSID_WlanAvNet, L"wlanmm.dll", L"Both", L"Available Network Class"},
    {CLSID_WlanPreCFEPage, L"wlanconn.dll", L"Apartment", L"WLAN Pre-CFE Page"},
    {CLSID_WlanUnsecuredConnectPage, L"wlanconn.dll", L"Apartment", L"Unsecured Net Connect Control Page"},
    {CLSID_WlanConnectingProgressPage, L"wlanconn.dll", L"Apartment", L"Connecting Progress Wizard Page"},
    {CLSID_WlanCredentialPage, L"wlanconn.dll", L"Apartment", L"Wireless Credential Wizard Page"},
    {CLSID_WlanNullSsidPage, L"wlanconn.dll", L"Apartment", L"Hidden Network SSID Wizard Page"},
    {CLSID_WlanCouldNotConnectPage, L"wlanconn.dll", L"Apartment", L"Could Not Connect Wizard Page"},
    {CLSID_WlanSetNetworkLocationPage, L"wlanconn.dll", L"Apartment", L"Set Network Location Wizard Page"},
    {CLSID_WlanManualConnectPage, L"wlanconn.dll", L"Apartment", L"Manual Connect Wizard Page"},
    {CLSID_WlanWisprSignupPage, L"wlanconn.dll", L"Apartment", L"Hotspot Sign-In Wizard Page"},
    {CLSID_WlanEnterProxyCredsPage, L"wlanconn.dll", L"Apartment", L"Enter Proxy Credentials Wizard Page"},
    {CLSID_WlanNonUiPage, L"wlanconn.dll", L"Apartment", L"WLAN Non-UI Connection Page"},
    {CLSID_WlanBalloonHandler, L"wlanconn.dll", L"Apartment", L"WLAN Balloon Handler"},
};
constexpr size_t kInProcComCount = ARRAYSIZE(kInProcComTable);

// xwreg.dll verified tagXW_COMPONENT_TYPE values: HOST = 1, TASK = 2, PAGE = 3
constexpr DWORD kXwComponentTypeTask = 2;
constexpr DWORD kXwComponentTypePage = 3;

// ===========================================================================
// Localized UI string tables
// -------------------------------------------------------------------------
// The MSDL binaries are MUI-split: their RT_STRING tables live in .mui files
// that the symbol server does not serve, and the 22621 van.dll/wlanmm.dll
// have every UI resource stripped into .mun files. The English texts below
// are the authentic Microsoft resource strings for these modules (dumped from
// the original resource sections); the Italian column provides the it-IT
// hydration. DirectUI font descriptors (e.g. "11;semibold;none;segoe ui")
// must never be translated.
// ===========================================================================
struct LocalizedResourceEntry {
    const wchar_t* moduleLower;
    UINT id;
    const wchar_t* enText;
    const wchar_t* itText;
};

constexpr LocalizedResourceEntry kHydratedStrings[] = {
    // --- Mandatory DirectUI font specs (DO NOT translate syntax!) ---
    {L"van.dll", 2200, L"11;normal;none;segoe ui", L"11;normal;none;segoe ui"},
    {L"van.dll", 2201, L"11;semibold;none;segoe ui", L"11;semibold;none;segoe ui"},
    {L"van.dll", 2203, L"16;semilight;none;segoe ui", L"16;semilight;none;segoe ui"},
    {L"wlanconn.dll", 2101, L"11;semibold;none;segoe ui", L"11;semibold;none;segoe ui"},
    {L"setproxycredential.dll", 18420, L"11;semibold;none;segoe ui", L"11;semibold;none;segoe ui"},
    {L"setproxycredential.dll", 18421, L"11;normal;none;segoe ui", L"11;normal;none;segoe ui"},

    // --- van.dll flyout headers, states, buttons, airplane mode ---
    {L"van.dll", 2001, L"No connections are available", L"Nessuna connessione disponibile"},
    {L"van.dll", 2002, L"Connections are available", L"Connessioni disponibili"},
    {L"van.dll", 2003, L"Discovering networks...", L"Individuazione delle reti in corso..."},
    {L"van.dll", 2005, L"Not connected", L"Non connesso"},
    {L"van.dll", 2007, L"Unified Connection Manager", L"Unified Connection Manager"},
    {L"van.dll", 2008, L"Please select a network first before creating a connection profile.",
     L"Selezionare prima una rete per creare un profilo di connessione."},
    {L"van.dll", 2009, L"Not connected - Connections are available",
     L"Non connesso - Connessioni disponibili"},
    {L"van.dll", 2010, L"Not connected - No connections are available",
     L"Non connesso - Nessuna connessione disponibile"},
    {L"van.dll", 2012, L"Information sent over this network might be visible to others.",
     L"Le informazioni inviate su questa rete potrebbero essere visibili ad altri utenti."},
    {L"van.dll", 2013, L"Connections", L"Connessioni"},
    {L"van.dll", 2014, L"No networks found", L"Nessuna rete trovata"},
    {L"van.dll", 2015, L" (Off)", L" (disattivata)"},
    {L"van.dll", 2016, L"Airplane mode", L"Modalit\x00e0 aereo"},
    {L"van.dll", 2017, L"On", L"Attivata"},
    {L"van.dll", 2018, L"Off", L"Disattivata"},
    {L"van.dll", 2102, L"Troubleshoot", L"Risoluzione dei problemi"},
    {L"van.dll", 2103, L"Networks", L"Reti"},
    {L"van.dll", 2300, L"Connect", L"Connetti"},
    {L"van.dll", 2301, L"Disconnect", L"Disconnetti"},
    {L"van.dll", 2302, L"Turn off wireless", L"Disattiva wireless"},
    {L"van.dll", 2303, L"Diagnose", L"Diagnostica"},
    {L"van.dll", 2304, L"Status", L"Stato"},
    {L"van.dll", 2305, L"Properties", L"Propriet\x00e0"},
    {L"van.dll", 2400, L"Connect To", L"Connessione a"},
    {L"van.dll", 2401,
     L"Shows available wireless networks and dial-up and VPN connections that you can connect to.",
     L"Mostra le reti wireless disponibili e le connessioni remote e VPN a cui \x00e8 possibile "
     L"connettersi."},
    {L"van.dll", 2402, L"Open Without Diagnostics", L"Apri senza diagnostica"},
    {L"van.dll", 3001, L"&Connect", L"&Connetti"},
    {L"van.dll", 3002, L"&Disconnect", L"&Disconnetti"},
    {L"van.dll", 3003, L"Connect automatically", L"Connetti automaticamente"},
    {L"van.dll", 3011, L"Internet access", L"Accesso a Internet"},
    {L"van.dll", 3012, L"No Internet access", L"Nessun accesso a Internet"},
    {L"van.dll", 3013, L"Internet and domain access", L"Accesso Internet e al dominio"},
    {L"van.dll", 3014, L"Domain access only", L"Solo accesso al dominio"},
    {L"van.dll", 3015, L"No network access", L"Nessun accesso alla rete"},
    {L"van.dll", 3020, L"Action needed", L"Azione richiesta"},
    {L"van.dll", 3021, L"Connected", L"Connesso"},
    {L"van.dll", 3022, L"Limited", L"Limitato"},
    {L"van.dll", 3100, L"This is a new router that has not been configured.",
     L"Si tratta di un nuovo router non configurato."},
    {L"van.dll", 3101, L"&Configure", L"&Configura"},
    {L"van.dll", 3102, L"Configure", L"Configura"},
    {L"van.dll", 4001, L"View Available Networks", L"Visualizza reti disponibili"},
    {L"van.dll", 4005, L"Media Manager List", L"Elenco gestori supporti"},
    {L"van.dll", 4006, L"Network Category Header", L"Intestazione categoria di rete"},
    {L"van.dll", 4007, L"Available Network List", L"Elenco reti disponibili"},
    {L"van.dll", 4010, L"Configure", L"Configura"},
    {L"van.dll", 4011, L"Connect", L"Connetti"},
    {L"van.dll", 4012, L"Disconnect", L"Disconnetti"},
    {L"van.dll", 4013, L"Connect automatically checkbox",
     L"Casella di controllo Connetti automaticamente"},
    {L"van.dll", 4014, L"%1!s! %2!s!", L"%1!s! %2!s!"},
    {L"van.dll", 4015, L"%1!s! %2!s!, %3!s!", L"%1!s! %2!s!, %3!s!"},
    {L"van.dll", 4017, L"Signal strength: %1!d! bar out of 5",
     L"Intensit\x00e0 del segnale: %1!d! barra su 5"},
    {L"van.dll", 4018, L"Signal strength: %1!d! 'bars' out of 5",
     L"Intensit\x00e0 del segnale: %1!d! barre su 5"},
    {L"van.dll", 4019, L"Link Pane", L"Riquadro collegamento"},
    {L"van.dll", 4025, L"Connected Network List", L"Elenco reti connesse"},
    {L"van.dll", 4030, L"Connect to a Network", L"Connessione a una rete"},
    {L"van.dll", 4031, L"Disconnect from a Network", L"Disconnessione da una rete"},
    {L"van.dll", 4032, L"Not Connectable", L"Non connettibile"},
    {L"van.dll", 4033, L"Configure a Network", L"Configurazione di una rete"},
    {L"van.dll", 5001, L"View Connection Settings", L"Visualizza impostazioni connessione"},

    // --- wlanconn.dll inline password / credential / hidden SSID prompts ---
    {L"wlanconn.dll", 550, L"Connect to a Network", L"Connessione a una rete"},
    {L"wlanconn.dll", 551, L"Disconnect from a Network", L"Disconnessione da una rete"},
    {L"wlanconn.dll", 552, L"Not Connectable", L"Non connettibile"},
    {L"wlanconn.dll", 553, L"Connect", L"Connetti"},
    {L"wlanconn.dll", 554, L"Disconnect", L"Disconnetti"},
    {L"wlanconn.dll", 555, L"&Connect", L"&Connetti"},
    {L"wlanconn.dll", 556, L"&Disconnect", L"&Disconnetti"},
    {L"wlanconn.dll", 557, L"Connection not available", L"Connessione non disponibile"},
    {L"wlanconn.dll", 558, L"Other people might be able to see info you send over this network",
     L"Altri utenti potrebbero visualizzare le informazioni inviate su questa rete"},
    {L"wlanconn.dll", 559, L"Connect automatically checkbox",
     L"Casella di controllo Connetti automaticamente"},
    {L"wlanconn.dll", 560, L"Connect automatically", L"Connetti automaticamente"},
    {L"wlanconn.dll", 562, L"Connecting to the network is taking longer than usual",
     L"La connessione alla rete richiede pi\x00f9 tempo del previsto"},
    {L"wlanconn.dll", 563, L"Name", L"Nome"},
    {L"wlanconn.dll", 564, L"Open web browser", L"Apri il browser Web"},
    {L"wlanconn.dll", 565, L"No, open a web browser to complete the connection",
     L"No, aprire il browser Web per completare la connessione"},
    {L"wlanconn.dll", 570, L"Wireless network name", L"Nome della rete wireless"},
    {L"wlanconn.dll", 571, L"User name", L"Nome utente"},
    {L"wlanconn.dll", 572, L"Password", L"Password"},
    {L"wlanconn.dll", 573,
     L"To try again, tap or click the link below to complete the connection or re-enter your "
     L"user name and password.",
     L"Per riprovare, toccare o fare clic sul collegamento seguente per completare la "
     L"connessione oppure reimmettere nome utente e password."},
    {L"wlanconn.dll", 574, L"View error details", L"Visualizza dettagli errore"},
    {L"wlanconn.dll", 575, L"Hide error details", L"Nascondi dettagli errore"},
    {L"wlanconn.dll", 576, L"Can\x2019t connect", L"Impossibile connettersi"},
    {L"wlanconn.dll", 577, L"The user name or password didn\x2019t work. Please check it and try again.",
     L"Nome utente o password non validi. Controllare e riprovare."},
    {L"wlanconn.dll", 578, L"Something went wrong. Please try again.",
     L"Si \x00e8 verificato un errore. Riprovare."},
    {L"wlanconn.dll", 579, L"There wasn\x2019t a response. Please try again.",
     L"Nessuna risposta. Riprovare."},
    {L"wlanconn.dll", 580, L"A message from your wireless provider:",
     L"Messaggio dal provider di servizi wireless:"},
    {L"wlanconn.dll", 590, L"Estimated usage", L"Utilizzo stimato"},
    {L"wlanconn.dll", 591, L"Reset", L"Reimposta"},
    {L"wlanconn.dll", 592, L"User name", L"Nome utente"},
    {L"wlanconn.dll", 593, L"Password", L"Password"},
    {L"wlanconn.dll", 594, L"Proxy server: %1", L"Server proxy: %1"},
    {L"wlanconn.dll", 595, L"The username or password didn\x2019t work. Please try again.",
     L"Nome utente o password non validi. Riprovare."},
    {L"wlanconn.dll", 596, L"Your proxy server sign-in info is required.",
     L"Sono necessarie le informazioni di accesso al server proxy."},
    {L"wlanconn.dll", 597, L"Continue", L"Continua"},
    {L"wlanconn.dll", 598,
     L"Some information changed since the last time you connected. We need additional "
     L"information to complete the connection.",
     L"Alcune informazioni sono state modificate dall'ultima connessione. Per completare la "
     L"connessione sono necessarie informazioni aggiuntive."},
    {L"wlanconn.dll", 599, L"Your proxy server sign-in info is required to connect to %1.",
     L"Per connettersi a %1 sono necessarie le informazioni di accesso al server proxy."},
    {L"wlanconn.dll", 1000, L"C&lose", L"&Chiudi"},
    {L"wlanconn.dll", 1001, L"Checking network requirements",
     L"Verifica dei requisiti di rete in corso"},
    {L"wlanconn.dll", 1002, L"Connecting", L"Connessione in corso"},
    {L"wlanconn.dll", 1006, L"Can\x2019t connect to this network",
     L"Impossibile connettersi a questa rete"},
    {L"wlanconn.dll", 1008, L"Verifying and connecting", L"Verifica e connessione in corso"},
    {L"wlanconn.dll", 1013, L"The connection is limited", L"La connessione \x00e8 limitata"},
    {L"wlanconn.dll", 1014, L"We can\x2019t connect to the Internet right now. Please try again.",
     L"Non \x00e8 possibile connettersi a Internet in questo momento. Riprovare."},
    {L"wlanconn.dll", 1023, L"Enter the name (SSID) for the network",
     L"Immettere il nome (SSID) della rete"},
    {L"wlanconn.dll", 1027, L"Invalid Network Name", L"Nome di rete non valido"},
    {L"wlanconn.dll", 1029, L"Reinsert your wireless network adapter",
     L"Reinserire la scheda di rete wireless"},
    {L"wlanconn.dll", 1030, L"Can\x2019t connect because the wireless network adapter was removed.",
     L"Impossibile connettersi perch\x00e9 la scheda di rete wireless \x00e8 stata rimossa."},
    {L"wlanconn.dll", 1031, L"Sign in to connect", L"Accedere per connettersi"},
    {L"wlanconn.dll", 1032, L"Close", L"Chiudi"},
    {L"wlanconn.dll", 1033, L"Sign in", L"Accedi"},
    {L"wlanconn.dll", 1036, L"Help me solve connection problems",
     L"Guida per la risoluzione dei problemi di connessione"},
    {L"wlanconn.dll", 1039, L"Your device is locked. Unlock it, and then try again.",
     L"Il dispositivo \x00e8 bloccato. Sbloccarlo e quindi riprovare."},
    {L"wlanconn.dll", 1040,
     L"Windows can\x2019t authenticate your connection right now. Please try again later.",
     L"Windows non \x00e8 in grado di autenticare la connessione in questo momento. Riprovare "
     L"pi\x00f9 tardi."},
    {L"wlanconn.dll", 1041, L"Authentication failed", L"Autenticazione non riuscita"},
    {L"wlanconn.dll", 1042, L"Do you have a user name and password for this Wi-Fi hotspot?",
     L"Si dispone di nome utente e password per questo hotspot Wi-Fi?"},
    {L"wlanconn.dll", 1043,
     L"Your network administrator doesn\x2019t allow simultaneous connections to your workplace "
     L"network and another network.",
     L"L'amministratore di rete non consente connessioni simultanee alla rete aziendale e a "
     L"un'altra rete."},
    {L"wlanconn.dll", 1044, L"Sign-in info for proxy server",
     L"Informazioni di accesso per il server proxy"},
    {L"wlanconn.dll", 1046, L"Sign in to connect to %1", L"Accedere per connettersi a %1"},
    {L"wlanconn.dll", 1047, L"Forget network", L"Dimentica rete"},
    {L"wlanconn.dll", 1100, L"Can\x2019t set up mobile hotspot.",
     L"Impossibile configurare l'hotspot mobile."},
    {L"wlanconn.dll", 1101, L"To enable sharing, contact your mobile operator to activate this feature.",
     L"Per abilitare la condivisione, contattare l'operatore di telefonia mobile per attivare "
     L"questa funzionalit\x00e0."},
    {L"wlanconn.dll", 1102, L"Can\x2019t connect to mobile broadband.",
     L"Impossibile connettersi alla rete a banda larga mobile."},
    {L"wlanconn.dll", 1103, L"Can\x2019t connect to mobile broadband while roaming.",
     L"Impossibile connettersi alla rete a banda larga mobile durante il roaming."},
    {L"wlanconn.dll", 5000, L"Connect to a network wizard",
     L"Procedura guidata Connessione a una rete"},

    // --- wlanmm.dll Wi-Fi media manager strings ---
    {L"wlanmm.dll", 150, L"Security Type", L"Tipo di sicurezza"},
    {L"wlanmm.dll", 200, L"Your network administrator has blocked you from connecting to this network.",
     L"L'amministratore di rete ha bloccato la connessione a questa rete."},
    {L"wlanmm.dll", 201, L"The capability of your network adapter does not match the requirements of this network.",
     L"Le funzionalit\x00e0 della scheda di rete non corrispondono ai requisiti di questa rete."},
    {L"wlanmm.dll", 202,
     L"The settings saved on this computer for the network do not match the requirements of the network.",
     L"Le impostazioni salvate nel computer per la rete non corrispondono ai requisiti della "
     L"rete."},
    {L"wlanmm.dll", 203, L"Connecting", L"Connessione in corso"},
    {L"wlanmm.dll", 205, L"Acquiring IP Address", L"Acquisizione dell'indirizzo IP in corso"},
    {L"wlanmm.dll", 206, L"Limited access", L"Accesso limitato"},
    {L"wlanmm.dll", 207, L"Connected", L"Connesso"},
    {L"wlanmm.dll", 208, L"Connected", L"Connesso"},
    {L"wlanmm.dll", 209, L"Disconnecting", L"Disconnessione in corso"},
    {L"wlanmm.dll", 210, L"Waiting for users", L"In attesa di utenti"},
    {L"wlanmm.dll", 211, L"This connection is not allowed by your administrator.",
     L"Questa connessione non \x00e8 consentita dall'amministratore."},
    {L"wlanmm.dll", 299, L"802.11 Available Network Manager", L"Gestore reti disponibili 802.11"},
    {L"wlanmm.dll", 300, L"Wi-Fi", L"Wi-Fi"},
    {L"wlanmm.dll", 306, L"SSID", L"SSID"},
    {L"wlanmm.dll", 307, L"Hidden network", L"Rete nascosta"},
    {L"wlanmm.dll", 308, L"Non broadcast", L"Non broadcast"},
    {L"wlanmm.dll", 400, L"Unknown", L"Sconosciuto"},
    {L"wlanmm.dll", 450, L"Type", L"Tipo"},
    {L"wlanmm.dll", 601, L"Show estimated data usage", L"Mostra utilizzo dati stimato"},
    {L"wlanmm.dll", 602, L"Hide estimated data usage", L"Nascondi utilizzo dati stimato"},
    {L"wlanmm.dll", 603, L"Set as metered connection", L"Imposta come connessione a consumo"},
    {L"wlanmm.dll", 604, L"Set as non-metered connection",
     L"Imposta come connessione non a consumo"},
    {L"wlanmm.dll", 605, L"Forget this network", L"Dimentica questa rete"},
    {L"wlanmm.dll", 5000, L"Unknown Authentication", L"Autenticazione sconosciuta"},
    {L"wlanmm.dll", 5001, L"Unsecured", L"Non protetta"},
    {L"wlanmm.dll", 5004, L"WPA-PSK", L"WPA-PSK"},
    {L"wlanmm.dll", 5005, L"WPA2", L"WPA2"},
    {L"wlanmm.dll", 5006, L"WPA2-PSK", L"WPA2-PSK"},
    {L"wlanmm.dll", 5007, L"802.1x", L"802.1x"},
    {L"wlanmm.dll", 5010, L"Unknown Encryption", L"Crittografia sconosciuta"},
    {L"wlanmm.dll", 5011, L"None", L"Nessuna"},
    {L"wlanmm.dll", 5013, L"TKIP", L"TKIP"},

    // --- dot3mm.dll wired media manager strings ---
    {L"dot3mm.dll", 300, L"Wired", L"Ethernet"},

    // --- rasmm.dll VPN / Dial-up media manager strings ---
    {L"rasmm.dll", 204, L"Connecting", L"Connessione in corso"},
    {L"rasmm.dll", 205, L"Disconnecting", L"Disconnessione in corso"},
    {L"rasmm.dll", 206, L"Connected", L"Connesso"},
    {L"rasmm.dll", 207, L"Connected", L"Connesso"},
    {L"rasmm.dll", 208, L"Connected", L"Connesso"},
    {L"rasmm.dll", 209, L"Connected", L"Connesso"},
    {L"rasmm.dll", 210,
     L"This connection is not available because there is a problem with your modem or network adapter.",
     L"La connessione non \x00e8 disponibile perch\x00e9 si \x00e8 verificato un problema con il "
     L"modem o la scheda di rete."},
    {L"rasmm.dll", 211, L"Dormant", L"Inattiva"},
    {L"rasmm.dll", 212, L"Dormant", L"Inattiva"},
    {L"rasmm.dll", 213, L"Action needed", L"Azione richiesta"},
    {L"rasmm.dll", 214, L"Unable to set up VPN Connection.",
     L"Impossibile impostare la connessione VPN."},
    {L"rasmm.dll", 300, L"Dial-up and VPN", L"Connessioni remote e VPN"},
    {L"rasmm.dll", 301, L"This connection requires an active Internet connection.",
     L"Per questa connessione \x00e8 necessaria una connessione Internet attiva."},
    {L"rasmm.dll", 302,
     L"Your network administrator has blocked you from connecting to this network.",
     L"L'amministratore di rete ha bloccato la connessione a questa rete."},
    {L"rasmm.dll", 310, L"Dial-up and VPN", L"Connessioni remote e VPN"},
    {L"rasmm.dll", 501, L"Clear saved sign-in info", L"Cancella le informazioni di accesso salvate"},

    // --- datusage.dll estimated data usage strings ---
    {L"datusage.dll", 101, L"%1 since a moment ago", L"%1 da un istante"},
    {L"datusage.dll", 102, L"%1 since 1 minute ago", L"%1 da 1 minuto"},
    {L"datusage.dll", 103, L"%1 since %2!d! minutes ago", L"%1 da %2!d! minuti"},
    {L"datusage.dll", 104, L"%1 since 1 hour ago", L"%1 da 1 ora"},
    {L"datusage.dll", 105, L"%1 since %2!d! hours ago", L"%1 da %2!d! ore"},
    {L"datusage.dll", 106, L"%1 since 1 day ago", L"%1 da 1 giorno"},
    {L"datusage.dll", 107, L"%1 since %2!d! days ago", L"%1 da %2!d! giorni"},
    {L"datusage.dll", 108, L"%1 during last 60 days", L"%1 negli ultimi 60 giorni"},
    {L"datusage.dll", 111, L"%s MB", L"%s MB"},
    {L"datusage.dll", 112, L"%s GB", L"%s GB"},
    {L"datusage.dll", 113, L"%s TB", L"%s TB"},
    {L"datusage.dll", 115, L"Over 1000 TB", L"Oltre 1000 TB"},

    // --- SetProxyCredential.dll proxy credential prompt strings ---
    {L"setproxycredential.dll", 18401, L"Networks", L"Reti"},
    {L"setproxycredential.dll", 18402, L"Enter proxy server credentials",
     L"Immettere le credenziali del server proxy"},
    {L"setproxycredential.dll", 18403, L"User name", L"Nome utente"},
    {L"setproxycredential.dll", 18404, L"Password", L"Password"},
    {L"setproxycredential.dll", 18405, L"Proxy server: %1", L"Server proxy: %1"},
    {L"setproxycredential.dll", 18406, L"The username or password was incorrect. Please try again.",
     L"Nome utente o password non corretti. Riprovare."},
    {L"setproxycredential.dll", 18408, L"Cancel", L"Annulla"},
    {L"setproxycredential.dll", 18409, L"Checking network requirements",
     L"Verifica dei requisiti di rete in corso"},
    {L"setproxycredential.dll", 18410, L"SPCPage", L"SPCPage"},

    // --- pnidui.dll classic tray tooltip strings ---
    {L"pnidui.dll", 2, L"Not connected - No connections are available",
     L"Non connesso - Nessuna connessione disponibile"},
    {L"pnidui.dll", 8, L"Internet access", L"Accesso a Internet"},
    {L"pnidui.dll", 15, L"Connection status - Unknown", L"Stato connessione - Sconosciuto"},
    {L"pnidui.dll", 35, L"No Internet access", L"Nessun accesso a Internet"},
    {L"pnidui.dll", 38, L"Not connected - Connections are available",
     L"Non connesso - Connessioni disponibili"},
    {L"pnidui.dll", 47, L"Windows Networking", L"Rete di Windows"},
    {L"pnidui.dll", 48, L"Network System Icon", L"Icona di sistema Rete"},
    {L"pnidui.dll", 60, L"Not connected - Wireless communication is turned off",
     L"Non connesso - Comunicazione wireless disattivata"},
    {L"pnidui.dll", 3011, L"Network System Icon", L"Icona di sistema Rete"},
};
constexpr size_t kHydratedStringCount = ARRAYSIZE(kHydratedStrings);

// ===========================================================================
// Global state
// ===========================================================================
enum class UiLangMode : int {
    Auto = 0,
    Italian = 1,
    English = 2,
};
enum class HostRole : int {
    Undetermined = 0,
    Host = 1,
    Secondary = 2,
};

std::atomic<bool> g_enableLegacyTrayIcon{true};
std::atomic<bool> g_interceptAvailableNetworksUri{true};
std::atomic<int> g_uiLangMode{static_cast<int>(UiLangMode::Auto)};
std::atomic<bool> g_hideDuplicateWlanAdapters{false};
std::atomic<bool> g_enableHotkey{false};
std::atomic<bool> g_strictVerification{false};

std::atomic<bool> g_shuttingDown{false};
std::atomic<bool> g_vanStoreReady{false};
std::atomic<DWORD> g_staThreadId{0};
std::atomic<int> g_hostRole{static_cast<int>(HostRole::Undetermined)};
std::atomic<ULONGLONG> g_lastShowVanTick{0};
std::atomic<HWND> g_fallbackTrayHwnd{nullptr};

// Session handle for WinHTTP kept alive across downloads so that
// Wh_ModBeforeUninit() can close it to abort any in-flight transfer.
std::atomic<HINTERNET> g_winhttpSession{nullptr};

// Precomputed full store paths, published (via the g_vanStoreReady release
// store) exactly once before the hooks start redirecting, then read-only.
// This lets Hook_LdrLoadDll stay 100% stack-based and lock-free.
WH_NO_DESTROY WCHAR g_managedFullPaths[kRuntimeDllCount][600] = {};

WH_NO_DESTROY std::mutex g_stateMutex;
WH_NO_DESTROY std::filesystem::path g_vanStoreBaseDir;   // ...\VanStore
WH_NO_DESTROY std::filesystem::path g_rawCacheDir;       // ...\MsdlRawCache
WH_NO_DESTROY std::filesystem::path g_vanStoreDir;       // ...\VanStore\gen_N
WH_NO_DESTROY std::vector<HMODULE> g_loadedVanModules;
WH_NO_DESTROY std::wstring g_hkcuNtPrefix;               // "\Registry\User\S-1-5-..."

HANDLE g_stopEvent = nullptr;
HANDLE g_workerThread = nullptr;
// Set in Wh_ModUninit when the STA worker fails to exit in time: managed
// modules must then stay loaded (leak) to avoid use-after-free crashes.
std::atomic<bool> g_workerStillRunning{false};
HANDLE g_provisionMutex = nullptr;
DLL_DIRECTORY_COOKIE g_dllDirectoryCookie = nullptr;
UINT g_wmShowVanIpc = 0;
HMODULE g_vanModule = nullptr;

constexpr UINT WM_APP_SHOW_VAN = WM_APP + 0x381;
constexpr UINT WM_APP_SYNC_TRAY = WM_APP + 0x382;
constexpr UINT WM_APP_FALLBACK_TRAY_CB = WM_APP + 0x384;
constexpr UINT_PTR kStaTimerHostReeval = 1;           // 5s hosting re-evaluation
constexpr UINT_PTR kStaTimerFallbackRefresh = 2;      // 2s fallback icon refresh
constexpr int kHotkeyId = 0x56414E31;                 // 'VAN1'

// van.dll x64 exports - verified by dumping the PE export directory of the
// MSDL van.dll 22621.2605 binary:
//   ord 1  HideVAN                    (PLAIN, undecorated names!)
//   ord 2  ShowVAN
//   ord 3  ShutdownVAN
//   ord 4  VanUIManager_CreateInstance
//   ord 5-8 DllCanUnloadNow / DllGetClassObject / DllRegisterServer /
//           DllUnregisterServer
//   ord 9  RunVANW
// The PDB carries the internal C++ symbols (?ShowVAN@@YAJ_N0PEAUIPopupBase@@@Z,
// ?HideVAN@@YAXXZ, ?ShutdownVAN@@YAXXZ) but the EXPORT DIRECTORY exports plain
// names, so GetProcAddress with the decorated spellings returns NULL - that
// was the "ShowVAN export missing" failure. Signatures (from the PDB):
//   long/HRESULT ShowVAN(bool bWait, bool b2, IPopupBase* pPopupBase)
//   void HideVAN()
//   void ShutdownVAN()
// Resolution order used at runtime: plain name -> export ordinal -> decorated
// name (kept for tolerance against other builds).
using ShowVAN_t = HRESULT(WINAPI*)(bool, bool, void*);
using HideVAN_t = void(WINAPI*)();
using ShutdownVAN_t = void(WINAPI*)();

ShowVAN_t g_pfnShowVAN = nullptr;
HideVAN_t g_pfnHideVAN = nullptr;
ShutdownVAN_t g_pfnShutdownVAN = nullptr;

// ntdll helpers resolved at init (no import-lib dependency needed).
using NtOpenKey_t = NTSTATUS(NTAPI*)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES);
using NtOpenKeyEx_t = NTSTATUS(NTAPI*)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, ULONG);
using NtQueryKey_t = NTSTATUS(NTAPI*)(HANDLE, ULONG, PVOID, ULONG, PULONG);
NtOpenKey_t g_pfnNtOpenKey = nullptr;
NtOpenKeyEx_t g_pfnNtOpenKeyEx = nullptr;
NtQueryKey_t g_pfnNtQueryKey = nullptr;

// KEY_INFORMATION_CLASS::KeyNameInformation == 3: returns the full NT path of
// a registry key handle (e.g. "\Registry\User\S-1-5-21-..."). NtQueryObject
// with ObjectNameInformation does NOT return key names - that is why the old
// HKCU prefix capture logged an empty string.
constexpr ULONG kKeyNameInformationClass = 3;
struct WhKeyNameInformation {
    ULONG NameLength;  // in bytes, without any terminating NUL
    WCHAR Name[1];     // the full key path follows inline
};

// Thread-local recursion guard for registry & module redirection hooks.
thread_local bool g_inHookGuard = false;

struct ScopedHookGuard {
    bool active = false;
    ScopedHookGuard() noexcept {
        if (!g_inHookGuard) {
            g_inHookGuard = true;
            active = true;
        }
    }
    ~ScopedHookGuard() noexcept {
        if (active) {
            g_inHookGuard = false;
        }
    }
};

// ===========================================================================
// Small utilities
// ===========================================================================
std::wstring ToLowerAscii(std::wstring_view sv) {
    std::wstring out(sv);
    std::transform(out.begin(), out.end(), out.begin(), [](wchar_t c) {
        return (c >= L'A' && c <= L'Z') ? (c + (L'a' - L'A')) : c;
    });
    return out;
}

bool StartsWithInsensitive(std::wstring_view text, std::wstring_view prefix) noexcept {
    if (text.size() < prefix.size()) {
        return false;
    }
    for (size_t i = 0; i < prefix.size(); ++i) {
        wchar_t a = text[i];
        wchar_t b = prefix[i];
        if (a >= L'A' && a <= L'Z') a += (L'a' - L'A');
        if (b >= L'A' && b <= L'Z') b += (L'a' - L'A');
        if (a != b) {
            return false;
        }
    }
    return true;
}

std::wstring GuidToUpperBraceString(const GUID& guid) {
    wchar_t buf[64]{};
    swprintf(buf, ARRAYSIZE(buf),
             L"{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
             guid.Data1, guid.Data2, guid.Data3,
             guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
             guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    return std::wstring(buf);
}

bool IsEffectiveItalian() noexcept {
    switch (static_cast<UiLangMode>(g_uiLangMode.load(std::memory_order_relaxed))) {
        case UiLangMode::Italian:
            return true;
        case UiLangMode::English:
            return false;
        case UiLangMode::Auto:
        default:
            return PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_ITALIAN;
    }
}

const wchar_t* GetHydratedStringFor(const wchar_t* moduleLower, UINT id, bool italian) noexcept {
    for (size_t i = 0; i < kHydratedStringCount; ++i) {
        const LocalizedResourceEntry& e = kHydratedStrings[i];
        if (e.id == id && _wcsicmp(e.moduleLower, moduleLower) == 0) {
            return italian ? e.itText : e.enText;
        }
    }
    return nullptr;
}

void LoadModSettings() {
    g_enableLegacyTrayIcon.store(
        Wh_GetIntSetting(L"enableLegacyTrayIcon") != 0, std::memory_order_relaxed);
    g_interceptAvailableNetworksUri.store(
        Wh_GetIntSetting(L"interceptAvailableNetworksUri") != 0, std::memory_order_relaxed);
    g_uiLangMode.store(
        Wh_GetIntSetting(L"uiLanguage"), std::memory_order_relaxed);
    g_hideDuplicateWlanAdapters.store(
        Wh_GetIntSetting(L"hideDuplicateWlanAdapters") != 0, std::memory_order_relaxed);
    g_enableHotkey.store(
        Wh_GetIntSetting(L"enableHotkey") != 0, std::memory_order_relaxed);
    g_strictVerification.store(
        Wh_GetIntSetting(L"strictVerification") != 0, std::memory_order_relaxed);
}

// Lookup hydrated runtime DLL name if `rawName` targets one of the 8 VAN DLLs.
// Pure stack - safe to call under loader lock.
const wchar_t* MatchManagedDllCanonicalName(std::wstring_view rawName) noexcept {
    if (rawName.empty()) {
        return nullptr;
    }
    size_t slash = rawName.find_last_of(L"\\/");
    std::wstring_view base =
        (slash == std::wstring_view::npos) ? rawName : rawName.substr(slash + 1);
    if (base.empty() || base.size() >= 60) {
        return nullptr;
    }

    wchar_t candidate[64]{};
    memcpy(candidate, base.data(), base.size() * sizeof(wchar_t));
    candidate[base.size()] = L'\0';
    if (wcschr(candidate, L'.') == nullptr) {
        wcscat(candidate, L".dll");
    }

    for (const wchar_t* managed : kRuntimeDllNames) {
        if (_wcsicmp(candidate, managed) == 0) {
            return managed;
        }
    }
    return nullptr;
}

std::filesystem::path GetManagedDllPath(const wchar_t* canonicalName) {
    std::lock_guard<std::mutex> lock(g_stateMutex);
    return g_vanStoreDir / canonicalName;
}

HMODULE LoadManagedDll(const wchar_t* canonicalName) {
    const std::filesystem::path fullPath = GetManagedDllPath(canonicalName);
    ScopedHookGuard guard;
    HMODULE hMod = LoadLibraryExW(
        fullPath.c_str(), nullptr,
        LOAD_WITH_ALTERED_SEARCH_PATH | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
    if (hMod) {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (std::find(g_loadedVanModules.begin(), g_loadedVanModules.end(), hMod) ==
            g_loadedVanModules.end()) {
            g_loadedVanModules.push_back(hMod);
        }
    }
    return hMod;
}

template <typename FnPtr>
FnPtr ResolveExportAddress(HMODULE hMod, const char* exportName) noexcept {
    if (!hMod || !exportName) {
        return nullptr;
    }
    return reinterpret_cast<FnPtr>(reinterpret_cast<void*>(GetProcAddress(hMod, exportName)));
}

// Resolves a van.dll export: plain name first (what the 22621 export
// directory actually contains), then the export ordinal, then the historical
// decorated spelling for forward compatibility with other builds.
template <typename FnPtr>
FnPtr ResolveVanExport(HMODULE hMod, const char* plainName, int ordinal,
                       const char* decoratedName) noexcept {
    if (FnPtr p = ResolveExportAddress<FnPtr>(hMod, plainName)) {
        return p;
    }
    if (ordinal > 0 && hMod) {
        // GetProcAddress ordinal form: LOWORD of the LPCSTR.
        if (auto* p = reinterpret_cast<void*>(GetProcAddress(
                hMod, reinterpret_cast<LPCSTR>(
                          static_cast<UINT_PTR>(static_cast<unsigned short>(ordinal)))))) {
            return reinterpret_cast<FnPtr>(p);
        }
    }
    return ResolveExportAddress<FnPtr>(hMod, decoratedName);
}

// ===========================================================================
// Permissive artifact validation
// -------------------------------------------------------------------------
// Requirements (all modes): structurally valid x64 PE image
//   (size >= 4096, 'MZ', 'PE\0\0', IMAGE_FILE_MACHINE_AMD64, PE32+ optional
//   header). SHA-256 digests are always computed and logged; in permissive
//   mode (default) a mismatch is a warning, in strict mode it is fatal.
// WinVerifyTrust runs as diagnostic telemetry only: MSDL binaries are
// catalog-signed and return TRUST_E_NOSIGNATURE (0x800B0100) because the
// build-time catalog is not present on the user's machine.
// ===========================================================================
bool IsPe64File(const std::filesystem::path& path) {
    HANDLE hFile = CreateFileW(
        path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }
    // Stack buffer only - this must stay cheap and loader-lock friendly.
    BYTE head[1024]{};
    DWORD bytesRead = 0;
    bool ok = false;
    if (ReadFile(hFile, head, sizeof(head), &bytesRead, nullptr) && bytesRead >= 0x200) {
        const WORD* w = reinterpret_cast<const WORD*>(head);
        if (w[0] == 0x5A4D) {  // 'MZ'
            DWORD e_lfanew = *reinterpret_cast<const DWORD*>(head + 0x3C);
            if (e_lfanew + 6 + sizeof(IMAGE_FILE_HEADER) + 2 <= bytesRead) {
                const BYTE* pe = head + e_lfanew;
                if (memcmp(pe, "PE\0\0", 4) == 0) {
                    const IMAGE_FILE_HEADER* fh =
                        reinterpret_cast<const IMAGE_FILE_HEADER*>(pe + 4);
                    if (fh->Machine == IMAGE_FILE_MACHINE_AMD64 &&
                        fh->SizeOfOptionalHeader >= sizeof(IMAGE_OPTIONAL_HEADER64)) {
                        const WORD optMagic =
                            *reinterpret_cast<const WORD*>(pe + 4 + sizeof(IMAGE_FILE_HEADER));
                        ok = (optMagic == 0x20B);  // PE32+ magic
                    }
                }
            }
        }
    }
    CloseHandle(hFile);

    if (ok) {
        std::error_code ec;
        if (std::filesystem::file_size(path, ec) < 4096) {
            ok = false;
        }
    }
    return ok;
}

bool ComputeFileSha256Hex(const std::filesystem::path& filePath, std::wstring& outUpperHex) {
    HANDLE hFile = CreateFileW(
        filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
        OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    BCRYPT_ALG_HANDLE hAlg = nullptr;
    BCRYPT_HASH_HANDLE hHash = nullptr;
    bool ok = false;

    if (BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0)) &&
        BCRYPT_SUCCESS(BCryptCreateHash(hAlg, &hHash, nullptr, 0, nullptr, 0, 0))) {
        std::vector<BYTE> buffer(64 * 1024);
        DWORD bytesRead = 0;
        bool readOk = true;
        while (ReadFile(hFile, buffer.data(), static_cast<DWORD>(buffer.size()), &bytesRead,
                        nullptr)) {
            if (bytesRead == 0) {
                break;
            }
            if (!BCRYPT_SUCCESS(BCryptHashData(hHash, buffer.data(), bytesRead, 0))) {
                readOk = false;
                break;
            }
        }

        std::array<BYTE, 32> digest{};
        if (readOk &&
            BCRYPT_SUCCESS(BCryptFinishHash(hHash, digest.data(),
                                            static_cast<ULONG>(digest.size()), 0))) {
            static constexpr wchar_t kHex[] = L"0123456789ABCDEF";
            outUpperHex.clear();
            outUpperHex.reserve(64);
            for (BYTE b : digest) {
                outUpperHex.push_back(kHex[(b >> 4) & 0xF]);
                outUpperHex.push_back(kHex[b & 0xF]);
            }
            ok = true;
        }
    }

    if (hHash) {
        BCryptDestroyHash(hHash);
    }
    if (hAlg) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
    }
    CloseHandle(hFile);
    return ok;
}

// Diagnostic-only Authenticode check. MSDL files are catalog-signed, so this
// normally reports TRUST_E_NOSIGNATURE; the result is only logged.
void LogAuthenticodeDiagnostic(const std::filesystem::path& filePath) {
    WINTRUST_FILE_INFO fileInfo{};
    fileInfo.cbStruct = sizeof(fileInfo);
    fileInfo.pcwszFilePath = filePath.c_str();

    WINTRUST_DATA trustData{};
    trustData.cbStruct = sizeof(trustData);
    trustData.dwUIChoice = WTD_UI_NONE;
    trustData.fdwRevocationChecks = WTD_REVOKE_NONE;
    trustData.dwUnionChoice = WTD_CHOICE_FILE;
    trustData.pFile = &fileInfo;
    trustData.dwStateAction = WTD_STATEACTION_VERIFY;
    trustData.dwProvFlags = WTD_CACHE_ONLY_URL_RETRIEVAL;

    GUID policyGuid = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    LONG status =
        WinVerifyTrust(static_cast<HWND>(INVALID_HANDLE_VALUE), &policyGuid, &trustData);

    trustData.dwStateAction = WTD_STATEACTION_CLOSE;
    WinVerifyTrust(static_cast<HWND>(INVALID_HANDLE_VALUE), &policyGuid, &trustData);

    Wh_Log(L"Authenticode telemetry for %s: WinVerifyTrust=0x%08lX (catalog-signed MSDL "
           L"binaries report TRUST_E_NOSIGNATURE=0x800B0100, which is expected)",
           filePath.filename().c_str(), static_cast<unsigned long>(status));
}

struct VerifyOutcome {
    bool ok;
    bool shaMatched;
};

VerifyOutcome CheckArtifactFile(const std::filesystem::path& path, const wchar_t* expectedSha256) {
    VerifyOutcome out{false, false};
    std::error_code ec;
    if (!std::filesystem::exists(path, ec)) {
        return out;
    }

    if (!IsPe64File(path)) {
        Wh_Log(L"PE64 structural validation rejected %s", path.filename().c_str());
        return out;
    }

    std::wstring actualHex;
    if (!ComputeFileSha256Hex(path, actualHex)) {
        Wh_Log(L"WARNING: could not compute SHA-256 of %s (%lu) - accepting valid x64 PE image",
               path.filename().c_str(), GetLastError());
        out.ok = !g_strictVerification.load(std::memory_order_relaxed);
        return out;
    }

    if (_wcsicmp(actualHex.c_str(), expectedSha256) == 0) {
        out.ok = true;
        out.shaMatched = true;
        return out;
    }

    if (g_strictVerification.load(std::memory_order_relaxed)) {
        Wh_Log(L"Strict verification: SHA-256 mismatch on %s (got %s, expected %s)",
               path.filename().c_str(), actualHex.c_str(), expectedSha256);
        return out;
    }

    Wh_Log(L"WARNING: SHA-256 mismatch on %s (got %s, expected %s). The file is a structurally "
           L"valid x64 PE image; accepting it in permissive mode.",
           path.filename().c_str(), actualHex.c_str(), expectedSha256);
    out.ok = true;
    return out;
}

// ===========================================================================
// Cancellable MSDL downloads (WinHTTP primary, URLDownloadToFileW fallback)
// ===========================================================================
void AbortWinHttpSession() {
    HINTERNET hSession = g_winhttpSession.exchange(nullptr, std::memory_order_acq_rel);
    if (hSession) {
        WinHttpCloseHandle(hSession);
        Wh_Log(L"WinHTTP session closed - any in-flight MSDL download is aborted");
    }
}

HINTERNET AcquireWinHttpSession() {
    HINTERNET hSession = g_winhttpSession.load(std::memory_order_acquire);
    if (hSession) {
        return hSession;
    }
    std::lock_guard<std::mutex> lock(g_stateMutex);
    hSession = g_winhttpSession.load(std::memory_order_acquire);
    if (hSession) {
        return hSession;
    }
    hSession = WinHttpOpen(
        L"Windhawk Win8NetworkFlyout/1.1 (Microsoft-Symbol-Server client)",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (hSession) {
        // resolve / connect / send / receive timeouts
        WinHttpSetTimeouts(hSession, 10000, 10000, 30000, 30000);
        g_winhttpSession.store(hSession, std::memory_order_release);
    }
    return hSession;
}

bool DownloadMsdlFileWithWinHttp(const wchar_t* relativeTail,
                                 const std::filesystem::path& destTmpPath) {
    HINTERNET hSession = AcquireWinHttpSession();
    if (!hSession) {
        Wh_Log(L"WinHttpOpen failed (%lu)", GetLastError());
        return false;
    }

    HINTERNET hConnect = WinHttpConnect(
        hSession, L"msdl.microsoft.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        return false;
    }

    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect, L"GET", relativeTail, nullptr, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        return false;
    }

    bool ok = false;
    HANDLE hOut = INVALID_HANDLE_VALUE;

    if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA,
                           0, 0, 0) &&
        WinHttpReceiveResponse(hRequest, nullptr)) {
        DWORD httpStatus = 0;
        DWORD statusSize = sizeof(httpStatus);
        if (WinHttpQueryHeaders(hRequest,
                                WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                                WINHTTP_HEADER_NAME_BY_INDEX, &httpStatus, &statusSize,
                                WINHTTP_NO_HEADER_INDEX) &&
            httpStatus == 200) {
            hOut = CreateFileW(
                destTmpPath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                FILE_ATTRIBUTE_NORMAL, nullptr);
            if (hOut != INVALID_HANDLE_VALUE) {
                std::vector<BYTE> buf(64 * 1024);
                DWORD bytesRead = 0;
                ok = true;
                for (;;) {
                    // Aborted session (Wh_ModBeforeUninit) makes ReadData fail fast.
                    if (g_shuttingDown.load(std::memory_order_acquire) ||
                        !g_winhttpSession.load(std::memory_order_acquire)) {
                        ok = false;
                        break;
                    }
                    if (!WinHttpReadData(hRequest, buf.data(),
                                         static_cast<DWORD>(buf.size()), &bytesRead)) {
                        ok = false;
                        break;
                    }
                    if (bytesRead == 0) {
                        break;
                    }
                    DWORD written = 0;
                    if (!WriteFile(hOut, buf.data(), bytesRead, &written, nullptr) ||
                        written != bytesRead) {
                        ok = false;
                        break;
                    }
                }
            }
        } else {
            Wh_Log(L"HTTP status %lu when downloading %s", httpStatus, relativeTail);
        }
    } else {
        Wh_Log(L"WinHttpSendRequest/ReceiveResponse failed (%lu) for %s", GetLastError(),
               relativeTail);
    }

    if (hOut != INVALID_HANDLE_VALUE) {
        CloseHandle(hOut);
    }
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);

    if (!ok) {
        DeleteFileW(destTmpPath.c_str());
    }
    return ok;
}

bool DownloadMsdlFileWithUrlMon(const wchar_t* relativeTail,
                                const std::filesystem::path& destTmpPath) {
    if (g_shuttingDown.load(std::memory_order_acquire)) {
        return false;
    }
    std::wstring url = L"https://msdl.microsoft.com/download/symbols/";
    url += relativeTail;
    DeleteFileW(destTmpPath.c_str());
    HRESULT hr = URLDownloadToFileW(nullptr, url.c_str(), destTmpPath.c_str(), 0, nullptr);
    if (FAILED(hr) || g_shuttingDown.load(std::memory_order_acquire)) {
        Wh_Log(L"URLDownloadToFileW fallback failed (0x%08lX) for %s",
               static_cast<unsigned long>(hr), relativeTail);
        DeleteFileW(destTmpPath.c_str());
        return false;
    }
    return true;
}

bool DownloadMsdlFile(const wchar_t* relativeTail, const std::filesystem::path& destTmpPath) {
    if (DownloadMsdlFileWithWinHttp(relativeTail, destTmpPath)) {
        return true;
    }
    if (g_shuttingDown.load(std::memory_order_acquire)) {
        return false;
    }
    // Permissive fallback for exotic proxy / TLS configurations.
    return DownloadMsdlFileWithUrlMon(relativeTail, destTmpPath);
}

// ===========================================================================
// PE resource hydration (.mun UIFILE/ICON/BITMAP/REGISTRY transplant, MUI
// strip, RT_STRING embedding in authentic 16-entry bundles)
// ===========================================================================
struct DonorResourceCopyCtx {
    HANDLE hUpdate = nullptr;
    int copiedCount = 0;
};

BOOL CALLBACK EnumResLangCopyProc(
    HMODULE hModule, LPCWSTR lpType, LPCWSTR lpName, WORD wIDLanguage, LONG_PTR lParam) {
    auto* ctx = reinterpret_cast<DonorResourceCopyCtx*>(lParam);
    HRSRC hRes = FindResourceExW(hModule, lpType, lpName, wIDLanguage);
    if (!hRes) {
        return TRUE;
    }
    HGLOBAL hGlobal = LoadResource(hModule, hRes);
    if (!hGlobal) {
        return TRUE;
    }
    DWORD cbData = SizeofResource(hModule, hRes);
    void* pData = LockResource(hGlobal);
    if (pData && cbData > 0) {
        if (UpdateResourceW(ctx->hUpdate, lpType, lpName, wIDLanguage, pData, cbData)) {
            ++ctx->copiedCount;
        }
    }
    return TRUE;
}

BOOL CALLBACK EnumResNameCopyProc(
    HMODULE hModule, LPCWSTR lpType, LPWSTR lpName, LONG_PTR lParam) {
    EnumResourceLanguagesW(hModule, lpType, lpName, EnumResLangCopyProc, lParam);
    return TRUE;
}

bool CopyResourcesFromDonorPe(const std::filesystem::path& donorPath, HANDLE hUpdateTarget) {
    HMODULE hDonor = LoadLibraryExW(
        donorPath.c_str(), nullptr,
        LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    if (!hDonor) {
        Wh_Log(L"Could not open donor PE %s (%lu)", donorPath.filename().c_str(), GetLastError());
        return false;
    }

    DonorResourceCopyCtx ctx{hUpdateTarget, 0};
    // The 22621 van.dll / wlanmm.dll have every UI resource stripped into
    // .mun files the symbol server does not serve; the 17763 donors still
    // carry the DirectUI layouts, icons and registry templates embedded.
    const LPCWSTR typesToCopy[] = {
        L"UIFILE",
        MAKEINTRESOURCEW(2),   // RT_BITMAP
        MAKEINTRESOURCEW(3),   // RT_ICON
        MAKEINTRESOURCEW(14),  // RT_GROUP_ICON
        L"REGISTRY",
    };
    for (LPCWSTR resType : typesToCopy) {
        EnumResourceNamesW(hDonor, resType, EnumResNameCopyProc,
                           reinterpret_cast<LONG_PTR>(&ctx));
    }
    FreeLibrary(hDonor);
    Wh_Log(L"Transplanted %d donor resources into %s", ctx.copiedCount,
           donorPath.filename().c_str());
    return ctx.copiedCount > 0;
}

struct MuiDeleteEntry {
    std::wstring nameStr;
    ULONG_PTR nameInt = 0;
    bool isIntName = false;
    WORD langId = 0;
};

BOOL CALLBACK EnumMuiLangCollectProc(
    HMODULE, LPCWSTR, LPCWSTR lpName, WORD wIDLanguage, LONG_PTR lParam) {
    auto* vec = reinterpret_cast<std::vector<MuiDeleteEntry>*>(lParam);
    MuiDeleteEntry item{};
    if (IS_INTRESOURCE(lpName)) {
        item.isIntName = true;
        item.nameInt = reinterpret_cast<ULONG_PTR>(lpName);
    } else {
        item.nameStr = lpName;
    }
    item.langId = wIDLanguage;
    vec->push_back(std::move(item));
    return TRUE;
}

BOOL CALLBACK EnumMuiNameCollectProc(
    HMODULE hModule, LPCWSTR lpType, LPWSTR lpName, LONG_PTR lParam) {
    EnumResourceLanguagesW(hModule, lpType, lpName, EnumMuiLangCollectProc, lParam);
    return TRUE;
}

bool HydrateRuntimePe(
    const std::filesystem::path& targetPePath,
    const wchar_t* canonicalModuleLower,
    bool useItalian,
    const std::filesystem::path* optionalDonorPe) {
    // Collect existing "MUI" manifest resources so hydration can remove the
    // MUI split - without this the loader keeps looking for <dll>.mui files
    // that do not exist next to the hydrated copy and every string misses.
    std::vector<MuiDeleteEntry> muiEntries;
    if (HMODULE hInspect = LoadLibraryExW(
            targetPePath.c_str(), nullptr,
            LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE)) {
        EnumResourceNamesW(
            hInspect, L"MUI", EnumMuiNameCollectProc, reinterpret_cast<LONG_PTR>(&muiEntries));
        FreeLibrary(hInspect);
    }

    // Standard Win32 RT_STRING layout: bundle N holds string IDs
    // N*16 .. N*16+15 (bundle id = stringId / 16 + 1, slot = stringId % 16).
    std::map<UINT, std::array<std::wstring, 16>> bundles;
    for (size_t i = 0; i < kHydratedStringCount; ++i) {
        const LocalizedResourceEntry& e = kHydratedStrings[i];
        if (_wcsicmp(e.moduleLower, canonicalModuleLower) == 0) {
            const UINT bundleId = (e.id / 16u) + 1u;
            const UINT slot = e.id % 16u;
            bundles[bundleId][slot] = useItalian ? e.itText : e.enText;
        }
    }

    HANDLE hUpdate = BeginUpdateResourceW(targetPePath.c_str(), FALSE);
    if (!hUpdate) {
        Wh_Log(L"BeginUpdateResourceW failed (%lu) on %s", GetLastError(),
               targetPePath.c_str());
        return false;
    }

    for (const auto& m : muiEntries) {
        LPCWSTR lpName = m.isIntName ? MAKEINTRESOURCEW(m.nameInt) : m.nameStr.c_str();
        UpdateResourceW(hUpdate, L"MUI", lpName, m.langId, nullptr, 0);
    }

    if (optionalDonorPe) {
        CopyResourcesFromDonorPe(*optionalDonorPe, hUpdate);
    }

    const WORD neutralLang = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
    const WORD enUsLang = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
    const WORD userLang = GetUserDefaultUILanguage();
    const LPCWSTR kRtStringW = MAKEINTRESOURCEW(6);  // RT_STRING

    for (const auto& [bundleId, strings] : bundles) {
        std::vector<WORD> rawBundle;
        rawBundle.reserve(256);
        for (const std::wstring& s : strings) {
            const WORD len = static_cast<WORD>((std::min<size_t>)(s.size(), 4095));
            rawBundle.push_back(len);
            if (len > 0) {
                rawBundle.insert(rawBundle.end(), s.data(), s.data() + len);
            }
        }
        const DWORD cb = static_cast<DWORD>(rawBundle.size() * sizeof(WORD));
        UpdateResourceW(hUpdate, kRtStringW, MAKEINTRESOURCEW(bundleId), neutralLang,
                        rawBundle.data(), cb);
        UpdateResourceW(hUpdate, kRtStringW, MAKEINTRESOURCEW(bundleId), enUsLang,
                        rawBundle.data(), cb);
        if (userLang != neutralLang && userLang != enUsLang) {
            UpdateResourceW(hUpdate, kRtStringW, MAKEINTRESOURCEW(bundleId), userLang,
                            rawBundle.data(), cb);
        }
    }

    if (!EndUpdateResourceW(hUpdate, FALSE)) {
        Wh_Log(L"EndUpdateResourceW failed (%lu) on %s", GetLastError(), targetPePath.c_str());
        return false;
    }

    // Post-hydration sanity check: the file must still be a valid x64 PE.
    if (!IsPe64File(targetPePath)) {
        Wh_Log(L"Hydrated %s failed PE64 post-validation", targetPePath.filename().c_str());
        return false;
    }
    return true;
}

// ===========================================================================
// VanStore provisioning (shared across all explorer.exe instances)
// ===========================================================================
struct HydrationSpec {
    const wchar_t* rawSource;
    const wchar_t* runtimeDest;
    const wchar_t* donorRaw;      // optional donor with embedded .mun resources
    const wchar_t* altRawSource;  // optional alternate base hydrated WITHOUT a
                                  // donor when the primary hydration fails
};

// wlanmm.dll is provisioned DIRECTLY from the 17763 raw image: that build
// embeds all 220 ICON + 22 GROUP_ICON + REGISTRY(100/101) resources natively
// (that is why it doubles as the donor), so it needs NO donor transplant.
// Transplanting those 244 donor resources into the resource-stripped 22621
// wlanmm base reproducibly fails on some machines with EndUpdateResourceW ->
// ERROR_INVALID_DATA (13), while every small hydration (pnidui, van,
// wlanconn, ...) succeeds. Basing wlanmm on 17763 keeps the same COM classes
// and the same media-manager interfaces and eliminates the large transplant.
constexpr HydrationSpec kHydrationSpecs[] = {
    {L"raw_22621_pnidui.dll", L"pnidui.dll", nullptr, nullptr},
    {L"raw_22621_van.dll", L"van.dll", L"raw_17763_van.dll", L"raw_17763_van.dll"},
    {L"raw_17763_wlanmm.dll", L"wlanmm.dll", nullptr, nullptr},
    {L"raw_22621_wlanconn.dll", L"wlanconn.dll", nullptr, nullptr},
    {L"raw_22621_dot3mm.dll", L"dot3mm.dll", nullptr, nullptr},
    {L"raw_22621_rasmm.dll", L"rasmm.dll", nullptr, nullptr},
    {L"raw_22621_datusage.dll", L"datusage.dll", nullptr, nullptr},
    {L"raw_22621_SetProxyCredential.dll", L"SetProxyCredential.dll", nullptr, nullptr},
};

std::filesystem::path GenDirPath(int gen) {
    std::wstring name;
    name.reserve(24);
    name += L"gen_";
    wchar_t num[16]{};
    swprintf(num, ARRAYSIZE(num), L"%d", gen);
    name += num;
    return g_vanStoreBaseDir / name;
}

bool IsGenDirComplete(const std::filesystem::path& dir) {
    std::error_code ec;
    if (!std::filesystem::is_directory(dir, ec)) {
        return false;
    }
    for (const wchar_t* dll : kRuntimeDllNames) {
        if (!std::filesystem::exists(dir / dll, ec)) {
            return false;
        }
    }
    return true;
}

bool ReadHydrationStamp(int& outGen, bool& outItalian) {
    const std::filesystem::path stampPath = g_vanStoreBaseDir / kHydrationStampName;
    HANDLE hFile = CreateFileW(
        stampPath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }
    WCHAR buf[128]{};
    DWORD bytesRead = 0;
    BOOL ok = ReadFile(hFile, buf, sizeof(buf) - sizeof(WCHAR), &bytesRead, nullptr);
    CloseHandle(hFile);
    if (!ok) {
        return false;
    }
    buf[bytesRead / sizeof(WCHAR)] = L'\0';

    outGen = 0;
    outItalian = false;
    bool haveGen = false;
    bool haveLang = false;
    WCHAR* ctx = nullptr;
    for (WCHAR* line = wcstok_s(buf, L"\r\n", &ctx); line;
         line = wcstok_s(nullptr, L"\r\n", &ctx)) {
        if (StartsWithInsensitive(line, L"gen=") && line[4] != L'\0') {
            outGen = _wtoi(line + 4);
            haveGen = true;
        } else if (StartsWithInsensitive(line, L"lang=")) {
            outItalian = StartsWithInsensitive(line + 5, L"it");
            haveLang = true;
        }
    }
    return haveGen && haveLang && outGen >= 1 && outGen <= 50;
}

bool WriteHydrationStamp(int gen, bool italian) {
    const std::filesystem::path stampPath = g_vanStoreBaseDir / kHydrationStampName;
    WCHAR buf[64]{};
    int n = swprintf(buf, ARRAYSIZE(buf), L"gen=%d\r\nlang=%s\r\n", gen,
                     italian ? L"it" : L"en");
    if (n <= 0) {
        return false;
    }
    HANDLE hFile = CreateFileW(
        stampPath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }
    DWORD written = 0;
    BOOL ok = WriteFile(hFile, buf, n * sizeof(WCHAR), &written, nullptr);
    CloseHandle(hFile);
    return ok && written == static_cast<DWORD>(n * sizeof(WCHAR));
}

bool EnsureRawArtifact(const MsdlArtifact& art, const std::wstring& pidTidSuffix) {
    const std::filesystem::path rawPath = g_rawCacheDir / art.localRawName;
    if (CheckArtifactFile(rawPath, art.expectedSha256).ok) {
        return true;
    }
    if (g_shuttingDown.load(std::memory_order_acquire)) {
        return false;
    }

    // Stale or missing: remove a structurally broken file so the download
    // can replace it cleanly.
    std::error_code ec;
    if (std::filesystem::exists(rawPath, ec) && !IsPe64File(rawPath)) {
        Wh_Log(L"Removing structurally invalid cached %s", art.localRawName);
        DeleteFileW(rawPath.c_str());
    }

    const std::filesystem::path tmpPath =
        g_rawCacheDir / (std::wstring(art.localRawName) + L".part." + pidTidSuffix);
    Wh_Log(L"Downloading %s from MSDL...", art.msdlRelativeUrl);
    if (!DownloadMsdlFile(art.msdlRelativeUrl, tmpPath)) {
        Wh_Log(L"Failed downloading %s", art.msdlRelativeUrl);
        return false;
    }
    if (!CheckArtifactFile(tmpPath, art.expectedSha256).ok) {
        DeleteFileW(tmpPath.c_str());
        Wh_Log(L"Verification rejected downloaded %s", art.localRawName);
        return false;
    }

    LogAuthenticodeDiagnostic(tmpPath);

    if (!MoveFileExW(tmpPath.c_str(), rawPath.c_str(),
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        // Another explorer.exe may hold the raw file open as a data image;
        // permissive recovery: accept the existing file if it validates.
        Wh_Log(L"MoveFileExW to %s failed (%lu) - trying to reuse existing file",
               art.localRawName, GetLastError());
        if (CheckArtifactFile(rawPath, art.expectedSha256).ok) {
            DeleteFileW(tmpPath.c_str());
            return true;
        }
        return false;
    }
    return true;
}

void CleanupOldGenerations(int keepGen) {
    std::error_code ec;
    for (int g = 1; g <= 50; ++g) {
        if (g == keepGen) {
            continue;
        }
        std::filesystem::path dir = GenDirPath(g);
        if (std::filesystem::is_directory(dir, ec)) {
            // Best effort: in-use images enter POSIX-style delete-pending and
            // disappear once the last mapping closes.
            std::filesystem::remove_all(dir, ec);
        }
    }
    // Remove the legacy v1.0.0 flat files (VanStore\*.dll) if present.
    for (const wchar_t* dll : kRuntimeDllNames) {
        std::filesystem::remove(g_vanStoreBaseDir / dll, ec);
    }
}

bool TryUseGeneration(int gen) {
    if (gen < 1 || gen > 50) {
        return false;
    }
    const std::filesystem::path dir = GenDirPath(gen);
    if (!IsGenDirComplete(dir)) {
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        g_vanStoreDir = dir;
    }
    return true;
}

bool EnsureVanStoreProvisioned() {
    const bool useItalian = IsEffectiveItalian();

    // Fast path (no mutex): valid stamp + complete generation folder.
    {
        int gen = 0;
        bool stampItalian = false;
        if (ReadHydrationStamp(gen, stampItalian) && stampItalian == useItalian &&
            TryUseGeneration(gen)) {
            Wh_Log(L"Hydrated VanStore cache already valid (generation %d, lang=%s)", gen,
                   stampItalian ? L"it" : L"en");
            return true;
        }
    }

    Wh_Log(L"Provisioning verified Microsoft VAN x64 binaries into %s...",
           g_vanStoreBaseDir.c_str());

    // Slow path: coordinate with every other explorer.exe through the named
    // mutex; abort immediately when the mod is being disabled.
    HANDLE waitHandles[2] = {g_provisionMutex, g_stopEvent};
    DWORD waitRes = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);
    if (waitRes != WAIT_OBJECT_0 && waitRes != WAIT_ABANDONED) {
        Wh_Log(L"Provisioning canceled (stop signaled)");
        return false;
    }

    bool success = false;
    do {
        // Another explorer.exe may have finished provisioning while we waited.
        int gen = 0;
        bool stampItalian = false;
        if (ReadHydrationStamp(gen, stampItalian) && stampItalian == useItalian &&
            TryUseGeneration(gen)) {
            Wh_Log(L"Another explorer.exe finished provisioning (generation %d) - reusing it",
                   gen);
            success = true;
            break;
        }

        std::error_code ec;
        std::filesystem::create_directories(g_vanStoreBaseDir, ec);
        std::filesystem::create_directories(g_rawCacheDir, ec);

        WCHAR pidTidSuffix[48]{};
        swprintf(pidTidSuffix, ARRAYSIZE(pidTidSuffix), L"%lu.%lu", GetCurrentProcessId(),
                 GetCurrentThreadId());

        int newGen = (gen >= 1 && gen < 50) ? (gen + 1) : 1;
        // Never hydrate into a generation folder that already exists on disk:
        // an older explorer.exe may still have its DLLs mapped from it, and
        // MoveFileExW would then fail with ERROR_ACCESS_DENIED (5) or, worse,
        // silently swap binaries under a live process. When the stamp is
        // missing (e.g. first run after a mod update), scan the existing
        // gen_<N> folders and allocate max+1 instead of reusing gen_1.
        int maxExistingGen = 0;
        for (int g = 1; g <= 50; ++g) {
            std::error_code probeEc;
            if (std::filesystem::is_directory(GenDirPath(g), probeEc)) {
                maxExistingGen = (std::max)(maxExistingGen, g);
            }
        }
        if (maxExistingGen >= newGen) {
            if (maxExistingGen >= 50) {
                Wh_Log(L"No free VanStore generation slot (1..50) - reusing existing");
                break;
            }
            newGen = maxExistingGen + 1;
            Wh_Log(L"Stamp was missing - allocating fresh generation %d (existing "
                   L"folders kept untouched)",
                   newGen);
        }
        const std::filesystem::path targetDir = GenDirPath(newGen);
        std::filesystem::create_directories(targetDir, ec);

        // 1. Raw MSDL cache
        bool rawsOk = true;
        for (const auto& art : kMsdlArtifacts) {
            if (g_shuttingDown.load(std::memory_order_acquire)) {
                break;
            }
            if (!EnsureRawArtifact(art, pidTidSuffix)) {
                rawsOk = false;
                break;
            }
        }
        if (!rawsOk || g_shuttingDown.load(std::memory_order_acquire)) {
            Wh_Log(L"Raw MSDL cache provisioning did not complete");
            break;
        }

        // 2. Hydrate each runtime image into the new generation folder.
        bool hydrateOk = true;
        for (const auto& spec : kHydrationSpecs) {
            if (g_shuttingDown.load(std::memory_order_acquire)) {
                hydrateOk = false;
                break;
            }
            const std::filesystem::path src = g_rawCacheDir / spec.rawSource;
            const std::filesystem::path dstTmp = targetDir /
                (std::wstring(spec.runtimeDest) + L".building." + pidTidSuffix);
            const std::filesystem::path dstFinal = targetDir / spec.runtimeDest;

            if (!CopyFileW(src.c_str(), dstTmp.c_str(), FALSE)) {
                Wh_Log(L"CopyFileW failed (%lu) for %s", GetLastError(), spec.runtimeDest);
                hydrateOk = false;
                break;
            }

            std::filesystem::path donorPath;
            std::filesystem::path* donorPtr = nullptr;
            if (spec.donorRaw) {
                donorPath = g_rawCacheDir / spec.donorRaw;
                donorPtr = &donorPath;
            }

            const std::wstring lowerName = ToLowerAscii(spec.runtimeDest);
            bool hydrated = false;
            const wchar_t* lastBaseName = spec.rawSource;
            if (HydrateRuntimePe(dstTmp, lowerName.c_str(), useItalian, donorPtr)) {
                hydrated = true;
            } else {
                DeleteFileW(dstTmp.c_str());
                Wh_Log(L"Hydration failed for %s (base %s)", spec.runtimeDest,
                       spec.rawSource);
                if (spec.altRawSource && wcscmp(spec.altRawSource, spec.rawSource) != 0) {
                    // Alternate base whose own embedded resources make the
                    // donor transplant unnecessary (van.dll: 17763 base).
                    lastBaseName = spec.altRawSource;
                    const std::filesystem::path altSrc = g_rawCacheDir / spec.altRawSource;
                    if (CopyFileW(altSrc.c_str(), dstTmp.c_str(), FALSE) &&
                        HydrateRuntimePe(dstTmp, lowerName.c_str(), useItalian, nullptr)) {
                        hydrated = true;
                        Wh_Log(L"Hydration succeeded with alternate base %s for %s",
                               spec.altRawSource, spec.runtimeDest);
                    } else {
                        DeleteFileW(dstTmp.c_str());
                    }
                }
                if (!hydrated) {
                    // Permissive last resort: a plain raw copy keeps COM,
                    // exports and (for 17763 bases) the embedded resources
                    // alive; string lookups then fall back to the
                    // LoadStringW hook safety net.
                    const std::filesystem::path lastSrc = g_rawCacheDir / lastBaseName;
                    if (CopyFileW(lastSrc.c_str(), dstTmp.c_str(), FALSE)) {
                        hydrated = true;
                        Wh_Log(L"WARNING: using unhydrated raw copy of %s for %s",
                               lastBaseName, spec.runtimeDest);
                    }
                }
            }
            if (!hydrated) {
                Wh_Log(L"All hydration strategies failed for %s", spec.runtimeDest);
                hydrateOk = false;
                break;
            }

            if (!MoveFileExW(dstTmp.c_str(), dstFinal.c_str(),
                             MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
                // ERROR_ACCESS_DENIED (5) and ERROR_SHARING_VIOLATION (32) are
                // transient here: Defender is often still scanning the freshly
                // written PE image, or a dying explorer.exe holds a short-lived
                // mapping. Retry with increasing backoff before giving up.
                DWORD moveErr = GetLastError();
                bool finalized = false;
                if (moveErr == ERROR_ACCESS_DENIED || moveErr == ERROR_SHARING_VIOLATION) {
                    const DWORD backoffMs[] = {200, 600, 1200};
                    for (DWORD backoff : backoffMs) {
                        Sleep(backoff);
                        if (g_shuttingDown.load(std::memory_order_acquire)) {
                            break;  // mod is unloading - abort promptly
                        }
                        if (MoveFileExW(dstTmp.c_str(), dstFinal.c_str(),
                                        MOVEFILE_REPLACE_EXISTING |
                                            MOVEFILE_WRITE_THROUGH)) {
                            finalized = true;
                            break;
                        }
                        moveErr = GetLastError();
                    }
                }
                if (!finalized) {
                    DeleteFileW(dstTmp.c_str());
                    Wh_Log(L"Failed finalizing hydrated %s (%lu)", spec.runtimeDest,
                           moveErr);
                    hydrateOk = false;
                    break;
                }
                Wh_Log(L"Finalizing %s required AV/lock retries - succeeded",
                       spec.runtimeDest);
            }
        }
        if (!hydrateOk) {
            break;
        }

        // 3. Publish the stamp + switch this process over to the new folder.
        if (!WriteHydrationStamp(newGen, useItalian)) {
            Wh_Log(L"Failed writing hydration stamp");
            break;
        }
        if (!TryUseGeneration(newGen)) {
            break;
        }
        Wh_Log(L"VanStore provisioning and PE hydration completed (generation %d, lang=%s)",
               newGen, useItalian ? L"it" : L"en");
        CleanupOldGenerations(newGen);
        success = true;
    } while (false);

    ReleaseMutex(g_provisionMutex);

    if (success) {
        return true;
    }

    // Permissive fallback: reuse the newest complete generation of any
    // language (wrong-language strings still beat a missing flyout).
    for (int g = 50; g >= 1; --g) {
        if (TryUseGeneration(g)) {
            Wh_Log(L"WARNING: falling back to existing VanStore generation %d", g);
            return true;
        }
    }
    Wh_Log(L"VanStore provisioning failed and no usable generation exists");
    return false;
}

// ===========================================================================
// Per-user registry overlay + real HKCU COM registrations
// -------------------------------------------------------------------------
// Everything is written under HKCU (zero UAC, zero HKLM writes):
//   1. <overlay>\Control_VAN\{manager CLSID}  - Active/Ordinal/PreLogon/...
//      plus the master {8B9D20EE-...} key with the 100/200/300 ordinal map,
//   2. <overlay>\XWizards\... - seeded from the real HKLM tree + our
//      task/page component graph,
//   3. <overlay>\CLSID\{...} - mirror used to satisfy HKCR/HKLM-style
//      lookups through the registry hooks,
//   4. REAL HKCU\Software\Classes\CLSID\{...}\InProcServer32 entries so COM
//      activation succeeds natively even without the hooks. Every key we own
//      is tagged with a WindhawkWin8Van marker and removed at unload.
// ===========================================================================
void RegWriteSz(HKEY hRoot, const std::wstring& subKey, const wchar_t* valueName,
                const std::wstring& value, DWORD type = REG_SZ) {
    HKEY hKey = nullptr;
    if (RegCreateKeyExW(hRoot, subKey.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE,
                        KEY_SET_VALUE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        RegSetValueExW(
            hKey, valueName, 0, type, reinterpret_cast<const BYTE*>(value.c_str()),
            static_cast<DWORD>((value.size() + 1) * sizeof(wchar_t)));
        RegCloseKey(hKey);
    }
}

void RegWriteDword(HKEY hRoot, const std::wstring& subKey, const wchar_t* valueName,
                   DWORD value) {
    HKEY hKey = nullptr;
    if (RegCreateKeyExW(hRoot, subKey.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE,
                        KEY_SET_VALUE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        RegSetValueExW(hKey, valueName, 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value),
                       sizeof(value));
        RegCloseKey(hKey);
    }
}

void WriteXwComponentNode(
    HKEY hOverlayRoot, const GUID& compGuid, DWORD classType, const wchar_t* name,
    const std::filesystem::path& dllPath) {
    const std::wstring base = L"XWizards\\Components\\" + GuidToUpperBraceString(compGuid);
    RegWriteSz(hOverlayRoot, base, nullptr, name);
    RegWriteSz(hOverlayRoot, base, L"Class Description", name);
    RegWriteDword(hOverlayRoot, base, L"Class Type", classType);
    RegWriteDword(hOverlayRoot, base, L"Behavior", 0);
    RegWriteSz(hOverlayRoot, base, L"File Name", dllPath.wstring(), REG_EXPAND_SZ);
}

void WriteXwLink(
    HKEY hOverlayRoot, const GUID& parentGuid, DWORD parentType, const wchar_t* parentName,
    const GUID& childGuid, DWORD childType, const wchar_t* childName, DWORD ordinal) {
    const std::wstring pStr = GuidToUpperBraceString(parentGuid);
    const std::wstring cStr = GuidToUpperBraceString(childGuid);

    const std::wstring childKey = L"XWizards\\Components\\" + pStr + L"\\Children\\" + cStr;
    RegWriteSz(hOverlayRoot, childKey, nullptr, childName);
    RegWriteSz(hOverlayRoot, childKey, L"Class Description", childName);
    RegWriteDword(hOverlayRoot, childKey, L"Class Type", childType);
    RegWriteDword(hOverlayRoot, childKey, L"Ordinal", ordinal);
    RegWriteDword(hOverlayRoot, childKey, L"User Flags", 0);

    const std::wstring parentKey = L"XWizards\\Components\\" + cStr + L"\\Parents\\" + pStr;
    RegWriteSz(hOverlayRoot, parentKey, nullptr, parentName);
    RegWriteSz(hOverlayRoot, parentKey, L"Class Description", parentName);
    RegWriteDword(hOverlayRoot, parentKey, L"Class Type", parentType);
}

void WriteRealHkcuComRegistration(const GUID& clsid, const wchar_t* dllFullPath,
                                  const wchar_t* threadingModel, const wchar_t* description) {
    const std::wstring guidStr = GuidToUpperBraceString(clsid);
    const std::wstring base = L"Software\\Classes\\CLSID\\" + guidStr;
    RegWriteSz(HKEY_CURRENT_USER, base, nullptr, description);
    RegWriteSz(HKEY_CURRENT_USER, base, kComMarkerValueName, L"1");
    RegWriteSz(HKEY_CURRENT_USER, base + L"\\InProcServer32", nullptr, dllFullPath);
    RegWriteSz(HKEY_CURRENT_USER, base + L"\\InProcServer32", L"ThreadingModel", threadingModel);
}

void DeleteRealHkcuComRegistration(const GUID& clsid) {
    const std::wstring guidStr = GuidToUpperBraceString(clsid);
    const std::wstring base = L"Software\\Classes\\CLSID\\" + guidStr;
    // Only delete when the marker is present so user-created keys survive.
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, base.c_str(), 0, KEY_QUERY_VALUE, &hKey) ==
        ERROR_SUCCESS) {
        wchar_t marker[8]{};
        DWORD size = sizeof(marker);
        DWORD type = 0;
        bool tagged =
            RegQueryValueExW(hKey, kComMarkerValueName, nullptr, &type,
                             reinterpret_cast<BYTE*>(marker), &size) == ERROR_SUCCESS &&
            type == REG_SZ;
        RegCloseKey(hKey);
        if (tagged) {
            RegDeleteTreeW(HKEY_CURRENT_USER, base.c_str());
        }
    }
}

bool PopulateVirtualRegistryOverlay() {
    ScopedHookGuard guard;

    HKEY hOverlayRoot = nullptr;
    if (RegCreateKeyExW(
            HKEY_CURRENT_USER, kVirtualRegRootSubkey, 0, nullptr, REG_OPTION_NON_VOLATILE,
            KEY_ALL_ACCESS, nullptr, &hOverlayRoot, nullptr) != ERROR_SUCCESS) {
        return false;
    }

    // 1a. HKLM\SYSTEM\CurrentControlSet\Control\VAN\{CLSID} media managers.
    struct MediaManagerSpec {
        GUID clsid;
        DWORD ordinal;
        const wchar_t* dllName;
    };
    const MediaManagerSpec managers[] = {
        {CLSID_Dot3MediaManager, 100, L"dot3mm.dll"},
        {CLSID_RasMediaManager, 200, L"rasmm.dll"},
        {CLSID_WlanMediaManager, 300, L"wlanmm.dll"},
    };
    for (const auto& mm : managers) {
        const std::wstring sub = L"Control_VAN\\" + GuidToUpperBraceString(mm.clsid);
        RegWriteDword(hOverlayRoot, sub, L"Active", 1);
        RegWriteDword(hOverlayRoot, sub, L"Ordinal", mm.ordinal);
        RegWriteDword(hOverlayRoot, sub, L"PreLogon", 1);
        RegWriteDword(hOverlayRoot, sub, L"NotInOOBE", 0);
        RegWriteSz(hOverlayRoot, sub, L"DisplayName", (g_vanStoreDir / mm.dllName).wstring());
    }

    // 1b. Master VAN component key {8B9D20EE-...}: Active=0 plus the
    //     100/200/300 ordinal->CLSID string map (matches the layout of a
    //     real Windows machine with the View Available Networks stack).
    {
        const std::wstring master = L"Control_VAN\\" + GuidToUpperBraceString(CLSID_VanMasterComponent);
        RegWriteDword(hOverlayRoot, master, L"Active", 0);
        RegWriteSz(hOverlayRoot, master, L"100", GuidToUpperBraceString(CLSID_Dot3MediaManager));
        RegWriteSz(hOverlayRoot, master, L"200", GuidToUpperBraceString(CLSID_RasMediaManager));
        RegWriteSz(hOverlayRoot, master, L"300", GuidToUpperBraceString(CLSID_WlanMediaManager));
    }

    // 1c. VAN settings key (HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\VAN)
    RegWriteDword(hOverlayRoot, L"Software_VAN", L"Initialized", 1);

    // 2. Seed overlay XWizards from the machine's real HKLM tree (read-only
    //    copy) so built-in Root / Wizard Types / system components stay
    //    visible, then add our task + page component graph.
    {
        HKEY hRealXw = nullptr;
        HKEY hVirtXw = nullptr;
        if (RegOpenKeyExW(
                HKEY_LOCAL_MACHINE,
                L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\XWizards", 0, KEY_READ,
                &hRealXw) == ERROR_SUCCESS) {
            if (RegCreateKeyExW(
                    hOverlayRoot, L"XWizards", 0, nullptr, REG_OPTION_NON_VOLATILE,
                    KEY_ALL_ACCESS, nullptr, &hVirtXw, nullptr) == ERROR_SUCCESS) {
                RegCopyTreeW(hRealXw, nullptr, hVirtXw);
                RegCloseKey(hVirtXw);
            }
            RegCloseKey(hRealXw);
        } else {
            RegCreateKeyExW(
                hOverlayRoot, L"XWizards", 0, nullptr, REG_OPTION_NON_VOLATILE,
                KEY_ALL_ACCESS, nullptr, &hVirtXw, nullptr);
            if (hVirtXw) {
                RegCloseKey(hVirtXw);
            }
        }
    }

    const std::filesystem::path vanDll = g_vanStoreDir / L"van.dll";
    const std::filesystem::path wlanConnDll = g_vanStoreDir / L"wlanconn.dll";

    const wchar_t* taskName = L"Connect to a network";
    WriteXwComponentNode(
        hOverlayRoot, CLSID_VanGenericCFETask, kXwComponentTypeTask, taskName, vanDll);

    struct WlanPageLink {
        GUID guid;
        const wchar_t* name;
    };
    const WlanPageLink pages[] = {
        {CLSID_WlanPreCFEPage, L"WLAN Pre-CFE Page"},
        {CLSID_WlanUnsecuredConnectPage, L"Unsecured Net Connect Control Page"},
        {CLSID_WlanConnectingProgressPage, L"Connecting Progress Wizard Page"},
        {CLSID_WlanCredentialPage, L"Wireless Credential Wizard Page"},
        {CLSID_WlanNullSsidPage, L"Hidden Network SSID Wizard Page"},
        {CLSID_WlanCouldNotConnectPage, L"Could Not Connect Wizard Page"},
        {CLSID_WlanSetNetworkLocationPage, L"Set Network Location Wizard Page"},
        {CLSID_WlanManualConnectPage, L"Manual Connect Wizard Page"},
        {CLSID_WlanWisprSignupPage, L"Hotspot Sign-In Wizard Page"},
        {CLSID_WlanEnterProxyCredsPage, L"Enter Proxy Credentials Wizard Page"},
        {CLSID_WlanNonUiPage, L"WLAN Non-UI Connection Page"},
        {CLSID_WlanBalloonHandler, L"WLAN Balloon Handler"},
    };

    DWORD ordinal = 1;
    for (const auto& pg : pages) {
        WriteXwComponentNode(
            hOverlayRoot, pg.guid, kXwComponentTypePage, pg.name, wlanConnDll);
        WriteXwLink(
            hOverlayRoot, CLSID_VanGenericCFETask, kXwComponentTypeTask, taskName, pg.guid,
            kXwComponentTypePage, pg.name, ordinal++);
    }

    // 3. Overlay CLSID mirror (used by the HKCR / HKLM registry redirects).
    for (const auto& entry : kInProcComTable) {
        const std::wstring clsidBase =
            L"CLSID\\" + GuidToUpperBraceString(entry.clsid);
        const std::wstring inproc = clsidBase + L"\\InProcServer32";
        RegWriteSz(hOverlayRoot, clsidBase, nullptr, entry.description);
        RegWriteSz(hOverlayRoot, inproc, nullptr, (g_vanStoreDir / entry.dllBaseName).wstring());
        RegWriteSz(hOverlayRoot, inproc, L"ThreadingModel", entry.threadingModel);
    }

    RegCloseKey(hOverlayRoot);

    // 4. Real per-user COM registrations (native COM activation support).
    for (const auto& entry : kInProcComTable) {
        WriteRealHkcuComRegistration(
            entry.clsid, (g_vanStoreDir / entry.dllBaseName).wstring().c_str(),
            entry.threadingModel, entry.description);
    }

    // 4b. Conditional: CLSID_PopupWindowFactory (Windows.UI.Immersive.dll) is
    //     required by CFlyoutListUi::CreateCharmWindow. Register a per-user
    //     fallback only when the OS itself has no registration.
    {
        HKEY hKey = nullptr;
        const std::wstring check = L"CLSID\\" + GuidToUpperBraceString(CLSID_PopupWindowFactory) +
                                   L"\\InProcServer32";
        if (RegOpenKeyExW(HKEY_CLASSES_ROOT, check.c_str(), 0, KEY_QUERY_VALUE, &hKey) !=
            ERROR_SUCCESS) {
            WCHAR immersivePath[MAX_PATH * 2]{};
            if (ExpandEnvironmentStringsW(L"%SystemRoot%\\System32\\Windows.UI.Immersive.dll",
                                          immersivePath,
                                          ARRAYSIZE(immersivePath)) > 0) {
                WriteRealHkcuComRegistration(
                    CLSID_PopupWindowFactory, immersivePath, L"Both",
                    L"Windows.UI.Immersive Popup Window Factory");
                Wh_Log(L"Registered per-user CLSID_PopupWindowFactory fallback "
                       L"(missing from HKCR on this system)");
            }
        } else {
            RegCloseKey(hKey);
        }
    }

    return true;
}

// ---------------------------------------------------------------------------
// Instance tracking (crash-healing PID list stored in the overlay root) so
// registry artifacts are only removed when the LAST mod instance unloads.
// ---------------------------------------------------------------------------
void PruneAndAddInstancePid() {
    ScopedHookGuard guard;
    HKEY hKey = nullptr;
    if (RegCreateKeyExW(
            HKEY_CURRENT_USER, kVirtualRegRootSubkey, 0, nullptr, REG_OPTION_NON_VOLATILE,
            KEY_QUERY_VALUE | KEY_SET_VALUE, nullptr, &hKey, nullptr) != ERROR_SUCCESS) {
        return;
    }

    wchar_t existing[4096]{};
    DWORD size = sizeof(existing) - sizeof(wchar_t);
    DWORD type = 0;
    if (RegQueryValueExW(
            hKey, kInstancePidValueName, nullptr, &type,
            reinterpret_cast<BYTE*>(existing), &size) != ERROR_SUCCESS ||
        type != REG_SZ) {
        existing[0] = L'\0';
    }

    wchar_t updated[4096]{};
    size_t pos = 0;
    const DWORD myPid = GetCurrentProcessId();

    // Keep only PIDs that are still alive.
    wchar_t* ctx = nullptr;
    for (wchar_t* tok = wcstok_s(existing, L";", &ctx); tok;
         tok = wcstok_s(nullptr, L";", &ctx)) {
        DWORD pid = static_cast<DWORD>(wcstoul(tok, nullptr, 10));
        if (pid == 0 || pid == myPid) {
            continue;
        }
        HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!hProc) {
            continue;  // dead or inaccessible explorer - drop it
        }
        CloseHandle(hProc);
        int n = swprintf(updated + pos, ARRAYSIZE(updated) - pos, L"%lu;", pid);
        if (n <= 0) {
            break;
        }
        pos += static_cast<size_t>(n);
    }
    swprintf(updated + pos, ARRAYSIZE(updated) - pos, L"%lu;", myPid);

    RegSetValueExW(
        hKey, kInstancePidValueName, 0, REG_SZ, reinterpret_cast<const BYTE*>(updated),
        static_cast<DWORD>((wcslen(updated) + 1) * sizeof(wchar_t)));
    RegCloseKey(hKey);
}

bool RemoveInstancePidAndCheckLast() {
    ScopedHookGuard guard;
    HKEY hKey = nullptr;
    if (RegCreateKeyExW(
            HKEY_CURRENT_USER, kVirtualRegRootSubkey, 0, nullptr, REG_OPTION_NON_VOLATILE,
            KEY_QUERY_VALUE | KEY_SET_VALUE, nullptr, &hKey, nullptr) != ERROR_SUCCESS) {
        return false;  // conservative: assume other instances exist
    }

    wchar_t existing[4096]{};
    DWORD size = sizeof(existing) - sizeof(wchar_t);
    DWORD type = 0;
    if (RegQueryValueExW(
            hKey, kInstancePidValueName, nullptr, &type,
            reinterpret_cast<BYTE*>(existing), &size) != ERROR_SUCCESS ||
        type != REG_SZ) {
        RegCloseKey(hKey);
        return true;
    }

    wchar_t updated[4096]{};
    size_t pos = 0;
    bool othersAlive = false;
    const DWORD myPid = GetCurrentProcessId();

    wchar_t* ctx = nullptr;
    for (wchar_t* tok = wcstok_s(existing, L";", &ctx); tok;
         tok = wcstok_s(nullptr, L";", &ctx)) {
        DWORD pid = static_cast<DWORD>(wcstoul(tok, nullptr, 10));
        if (pid == 0 || pid == myPid) {
            continue;
        }
        HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!hProc) {
            continue;
        }
        CloseHandle(hProc);
        othersAlive = true;
        int n = swprintf(updated + pos, ARRAYSIZE(updated) - pos, L"%lu;", pid);
        if (n <= 0) {
            break;
        }
        pos += static_cast<size_t>(n);
    }

    if (othersAlive) {
        RegSetValueExW(
            hKey, kInstancePidValueName, 0, REG_SZ,
            reinterpret_cast<const BYTE*>(updated),
            static_cast<DWORD>((wcslen(updated) + 1) * sizeof(wchar_t)));
    } else {
        RegDeleteValueW(hKey, kInstancePidValueName);
    }
    RegCloseKey(hKey);
    return !othersAlive;
}

void CleanupRegistryArtifacts() {
    ScopedHookGuard guard;
    // Overlay tree.
    RegDeleteTreeW(HKEY_CURRENT_USER, kVirtualRegRootSubkey);
    // Tagged per-user COM registrations (marker-checked, never blanket).
    for (const auto& entry : kInProcComTable) {
        DeleteRealHkcuComRegistration(entry.clsid);
    }
    DeleteRealHkcuComRegistration(CLSID_PopupWindowFactory);
    Wh_Log(L"Registry overlay and tagged HKCU COM registrations removed");
}

// ===========================================================================
// Registry path mapping: Win32 (RegOpenKeyExW/A) + NT (NtOpenKey/NtOpenKeyEx)
// ===========================================================================
struct OverlayPrefixMap {
    const wchar_t* regPrefix;   // queried subkey prefix (case-insensitive)
    const wchar_t* overlaySub;  // overlay subkey it maps to
    bool trailingSep;           // prefix already ends with a separator
};

// HKLM-prefixed lookups map into the overlay; HKCR/HKCU CLSID lookups map to
// the overlay mirror (the real HKCU\Software\Classes\CLSID registrations
// exist in parallel and satisfy native COM activation on their own).
constexpr OverlayPrefixMap kHklmOverlayMaps[] = {
    {L"SYSTEM\\CurrentControlSet\\Control\\VAN", L"Control_VAN", false},
    {L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\XWizards", L"XWizards", false},
    {L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\VAN", L"Software_VAN", false},
    {L"SOFTWARE\\Classes\\CLSID\\", L"CLSID\\", true},
};
constexpr OverlayPrefixMap kHkcrOverlayMaps[] = {
    {L"CLSID\\", L"CLSID\\", true},
    {L"Software\\Classes\\CLSID\\", L"CLSID\\", true},
};

// Stack-only overlay path builder (loader-lock friendly).
bool BuildOverlayPath(HKEY hKey, const wchar_t* subKey, wchar_t* out, size_t cchOut) {
    if (!subKey || !subKey[0]) {
        return false;
    }
    const OverlayPrefixMap* maps = nullptr;
    size_t mapCount = 0;
    if (hKey == HKEY_LOCAL_MACHINE) {
        maps = kHklmOverlayMaps;
        mapCount = ARRAYSIZE(kHklmOverlayMaps);
    } else if (hKey == HKEY_CLASSES_ROOT || hKey == HKEY_CURRENT_USER) {
        maps = kHkcrOverlayMaps;
        mapCount = ARRAYSIZE(kHkcrOverlayMaps);
    } else {
        return false;
    }

    const std::wstring_view sub(subKey);
    for (size_t m = 0; m < mapCount; ++m) {
        const std::wstring_view prefix(maps[m].regPrefix);
        if (!StartsWithInsensitive(sub, prefix)) {
            continue;
        }
        const bool atEnd = (sub.size() == prefix.size());
        const bool boundary = atEnd || sub[prefix.size()] == L'\\';
        if (!boundary) {
            continue;
        }
        // `sub` is always NUL-terminated (LPCWSTR), so the suffix view behind
        // `rest` is terminated by the same NUL and can be printed directly.
        // Non-trailing prefixes consume up to (not including) the separator,
        // so `rest` carries its own leading '\\' when present.
        const wchar_t* rest = sub.data() + prefix.size();
        int n = swprintf(out, cchOut, L"%s\\%s%s", kVirtualRegRootSubkey, maps[m].overlaySub,
                         rest);
        if (n > 0) {
            return true;
        }
    }
    return false;
}

using RegOpenKeyExW_t = LSTATUS(WINAPI*)(HKEY, LPCWSTR, DWORD, REGSAM, PHKEY);
RegOpenKeyExW_t g_origRegOpenKeyExW = nullptr;

LSTATUS WINAPI Hook_RegOpenKeyExW(
    HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) {
    if (phkResult && lpSubKey && lpSubKey[0] &&
        g_vanStoreReady.load(std::memory_order_acquire) && !g_inHookGuard) {
        wchar_t mapped[1024];
        if (BuildOverlayPath(hKey, lpSubKey, mapped, ARRAYSIZE(mapped))) {
            ScopedHookGuard guard;
            LSTATUS st = g_origRegOpenKeyExW(
                HKEY_CURRENT_USER, mapped, ulOptions, samDesired, phkResult);
            if (st == ERROR_SUCCESS) {
                return st;  // genuine kernel HKEY - RegNotifyChangeKeyValue works
            }
            // Permissive: fall through to the original call on any miss.
        }
    }
    return g_origRegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

using RegOpenKeyExA_t = LSTATUS(WINAPI*)(HKEY, LPCSTR, DWORD, REGSAM, PHKEY);
RegOpenKeyExA_t g_origRegOpenKeyExA = nullptr;

LSTATUS WINAPI Hook_RegOpenKeyExA(
    HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) {
    if (phkResult && lpSubKey && lpSubKey[0] &&
        g_vanStoreReady.load(std::memory_order_acquire) && !g_inHookGuard) {
        wchar_t subWide[512];
        int n = MultiByteToWideChar(CP_ACP, 0, lpSubKey, -1, subWide, ARRAYSIZE(subWide));
        if (n > 0) {
            wchar_t mapped[1024];
            if (BuildOverlayPath(hKey, subWide, mapped, ARRAYSIZE(mapped))) {
                ScopedHookGuard guard;
                LSTATUS st = g_origRegOpenKeyExW(
                    HKEY_CURRENT_USER, mapped, ulOptions, samDesired, phkResult);
                if (st == ERROR_SUCCESS) {
                    return st;
                }
            }
        }
    }
    return g_origRegOpenKeyExA(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

// ---------------------------------------------------------------------------
// RegGetValueW hook: keep the managed pnidui/van modules convinced that the
// classic tray is NOT replaced (ReplaceVan == 0). ExplorerPatcher-style
// setups write ReplaceVan=1 to suppress the legacy network tray icon; the
// community restore-legacy-network-tray mod neutralizes the same value via
// an IAT patch. The override only fires when the caller is one of our
// managed (VanStore) modules, so Explorer and other processes are unaffected.
// ---------------------------------------------------------------------------
using RegGetValueW_t =
    LONG(WINAPI*)(HKEY, LPCWSTR, LPCWSTR, DWORD, LPDWORD, PVOID, LPDWORD);
RegGetValueW_t g_origRegGetValueW = nullptr;

bool IsCallerManagedVanModule(void* returnAddress) noexcept {
    if (!returnAddress) {
        return false;
    }
    HMODULE hCaller = nullptr;
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            static_cast<LPCWSTR>(returnAddress), &hCaller) ||
        !hCaller) {
        return false;
    }
    // Only short critical sections that never call back into registry APIs
    // hold this mutex, so acquiring it here cannot deadlock.
    std::lock_guard<std::mutex> lock(g_stateMutex);
    return std::find(
               g_loadedVanModules.begin(), g_loadedVanModules.end(), hCaller) !=
           g_loadedVanModules.end();
}

LONG WINAPI Hook_RegGetValueW(
    HKEY hkey, LPCWSTR subKey, LPCWSTR value, DWORD flags, LPDWORD type,
    PVOID data, LPDWORD dataSize) {
    if (value && _wcsicmp(value, L"ReplaceVan") == 0 && g_origRegGetValueW &&
        IsCallerManagedVanModule(__builtin_return_address(0))) {
        if (type) {
            *type = REG_DWORD;
        }
        if (data && dataSize && *dataSize < sizeof(DWORD)) {
            *dataSize = sizeof(DWORD);
            return ERROR_MORE_DATA;
        }
        if (data) {
            *reinterpret_cast<DWORD*>(data) = 0;  // 0 = keep the classic tray
        }
        if (dataSize) {
            *dataSize = sizeof(DWORD);
        }
        return ERROR_SUCCESS;
    }
    return g_origRegGetValueW(hkey, subKey, value, flags, type, data, dataSize);
}

// ---------------------------------------------------------------------------
// NT-level redirection (stack-only, no heap, no locks)
// ---------------------------------------------------------------------------
bool BuildRedirectedNtKeyPath(
    const wchar_t* fullPath, size_t fullPathLen, wchar_t* out, size_t cchOut,
    USHORT* outLengthBytes) {
    struct NtPrefixMap {
        const wchar_t* from;
        const wchar_t* to;  // overlay-relative
        bool trailingSep;
    };
    static constexpr NtPrefixMap kMaps[] = {
        {L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Control\\VAN", L"Control_VAN", false},
        {L"\\Registry\\Machine\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\XWizards",
         L"XWizards", false},
        {L"\\Registry\\Machine\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\VAN",
         L"Software_VAN", false},
        {L"\\Registry\\Machine\\SOFTWARE\\Classes\\CLSID\\", L"CLSID\\", true},
    };

    for (const NtPrefixMap& m : kMaps) {
        const size_t fromLen = wcslen(m.from);
        if (fullPathLen < fromLen || !StartsWithInsensitive(
                                         std::wstring_view(fullPath, fullPathLen), m.from)) {
            continue;
        }
        const bool atEnd = (fullPathLen == fromLen);
        const bool boundary = atEnd || fullPath[fromLen] == L'\\';
        if (!boundary) {
            continue;
        }

        int n = swprintf(out, cchOut, L"%s\\%s%s%s", g_hkcuNtPrefix.c_str(), m.to,
                         atEnd ? L"" : L"\\",
                         atEnd ? L"" : (fullPath + fromLen));
        if (n <= 0) {
            return false;
        }
        *outLengthBytes = static_cast<USHORT>(n * sizeof(wchar_t));
        return true;
    }
    return false;
}

// Builds the absolute NT path for an OBJECT_ATTRIBUTES (handles both absolute
// names and RootDirectory-relative opens) and, if it targets one of our
// virtualized keys, writes the redirected HKCU NT path into `outPath`.
bool TryBuildRedirectedNtOpenPath(
    POBJECT_ATTRIBUTES objectAttributes, wchar_t* outPath, size_t cchOut,
    USHORT* outLengthBytes) {
    if (!objectAttributes || !objectAttributes->ObjectName ||
        !objectAttributes->ObjectName->Buffer ||
        objectAttributes->ObjectName->Length == 0 ||
        objectAttributes->ObjectName->Length % sizeof(wchar_t) != 0) {
        return false;
    }

    wchar_t full[800];
    const USHORT nameLen =
        objectAttributes->ObjectName->Length / sizeof(wchar_t);

    if (objectAttributes->RootDirectory != nullptr) {
        // Resolve the root handle name so relative opens can be mapped too.
        // NtQueryKey(KeyNameInformation) is the only reliable way to obtain
        // the full NT path of a registry key handle.
        if (!g_pfnNtQueryKey) {
            return false;
        }
        BYTE queryBuf[1024];
        ULONG returned = 0;
        NTSTATUS st = g_pfnNtQueryKey(
            objectAttributes->RootDirectory, kKeyNameInformationClass, queryBuf,
            sizeof(queryBuf), &returned);
        if (st != 0) {
            // Even STATUS_BUFFER_OVERFLOW (0x80000005) partial data is
            // unusable here: treat any non-success status as a miss.
            return false;
        }
        auto* nameInfo = reinterpret_cast<WhKeyNameInformation*>(queryBuf);
        // NameLength is in bytes; compare character counts against the buffer
        // capacity (ARRAYSIZE is already in wchar_t units).
        const size_t rootLen = nameInfo->NameLength / sizeof(wchar_t);
        if (nameInfo->NameLength == 0 ||
            nameInfo->NameLength % sizeof(wchar_t) != 0 ||
            rootLen + static_cast<size_t>(nameLen) + 2 > ARRAYSIZE(full)) {
            return false;
        }
        memcpy(full, nameInfo->Name, nameInfo->NameLength);
        full[rootLen] = L'\\';
        memcpy(full + rootLen + 1, objectAttributes->ObjectName->Buffer,
               objectAttributes->ObjectName->Length);
        full[rootLen + 1 + nameLen] = L'\0';
        return BuildRedirectedNtKeyPath(
            full, rootLen + 1 + nameLen, outPath, cchOut, outLengthBytes);
    }

    if (static_cast<size_t>(nameLen) >= ARRAYSIZE(full)) {
        return false;
    }
    memcpy(full, objectAttributes->ObjectName->Buffer,
           objectAttributes->ObjectName->Length);
    full[nameLen] = L'\0';
    return BuildRedirectedNtKeyPath(full, nameLen, outPath, cchOut, outLengthBytes);
}

NtOpenKey_t g_origNtOpenKey = nullptr;

NTSTATUS NTAPI Hook_NtOpenKey(PHANDLE KeyHandle, ACCESS_MASK DesiredAccess,
                              POBJECT_ATTRIBUTES ObjectAttributes) {
    if (KeyHandle && g_vanStoreReady.load(std::memory_order_acquire) &&
        !g_inHookGuard && !g_hkcuNtPrefix.empty()) {
        wchar_t redirected[1100];
        USHORT redirectedLen = 0;
        if (TryBuildRedirectedNtOpenPath(
                ObjectAttributes, redirected, ARRAYSIZE(redirected), &redirectedLen)) {
            ScopedHookGuard guard;
            UNICODE_STRING us{};
            us.Buffer = redirected;
            us.Length = redirectedLen;
            us.MaximumLength = redirectedLen + sizeof(wchar_t);

            OBJECT_ATTRIBUTES oa{};
            oa.Length = sizeof(oa);
            oa.ObjectName = &us;
            oa.Attributes = ObjectAttributes->Attributes;
            oa.SecurityDescriptor = ObjectAttributes->SecurityDescriptor;
            oa.SecurityQualityOfService = ObjectAttributes->SecurityQualityOfService;

            NTSTATUS st = g_origNtOpenKey(KeyHandle, DesiredAccess, &oa);
            if (NT_SUCCESS(st)) {
                return st;  // real kernel HKEY into our HKCU overlay
            }
            // Permissive: fall back to the original open.
        }
    }
    return g_origNtOpenKey(KeyHandle, DesiredAccess, ObjectAttributes);
}

NtOpenKeyEx_t g_origNtOpenKeyEx = nullptr;

NTSTATUS NTAPI Hook_NtOpenKeyEx(PHANDLE KeyHandle, ACCESS_MASK DesiredAccess,
                                POBJECT_ATTRIBUTES ObjectAttributes, ULONG OpenOptions) {
    if (KeyHandle && g_vanStoreReady.load(std::memory_order_acquire) &&
        !g_inHookGuard && !g_hkcuNtPrefix.empty()) {
        wchar_t redirected[1100];
        USHORT redirectedLen = 0;
        if (TryBuildRedirectedNtOpenPath(
                ObjectAttributes, redirected, ARRAYSIZE(redirected), &redirectedLen)) {
            ScopedHookGuard guard;
            UNICODE_STRING us{};
            us.Buffer = redirected;
            us.Length = redirectedLen;
            us.MaximumLength = redirectedLen + sizeof(wchar_t);

            OBJECT_ATTRIBUTES oa{};
            oa.Length = sizeof(oa);
            oa.ObjectName = &us;
            oa.Attributes = ObjectAttributes->Attributes;
            oa.SecurityDescriptor = ObjectAttributes->SecurityDescriptor;
            oa.SecurityQualityOfService = ObjectAttributes->SecurityQualityOfService;

            NTSTATUS st = g_origNtOpenKeyEx(KeyHandle, DesiredAccess, &oa, OpenOptions);
            if (NT_SUCCESS(st)) {
                return st;
            }
        }
    }
    return g_origNtOpenKeyEx(KeyHandle, DesiredAccess, ObjectAttributes, OpenOptions);
}

// ===========================================================================
// Loader hooks: LdrLoadDll (import resolution), LoadLibraryExW/W
// -------------------------------------------------------------------------
// Hook_LdrLoadDll must stay 100% stack-based and lock-free: it can run under
// LdrpLoaderLock while import descriptors of any module are resolved.
// ===========================================================================
using LdrLoadDll_t = NTSTATUS(NTAPI*)(PWSTR, PULONG, PUNICODE_STRING, PHANDLE);
LdrLoadDll_t g_origLdrLoadDll = nullptr;

NTSTATUS NTAPI Hook_LdrLoadDll(
    PWSTR DllPath, PULONG DllCharacteristics, PUNICODE_STRING DllName, PHANDLE DllHandle) {
    if (DllName && DllName->Buffer && DllHandle &&
        g_vanStoreReady.load(std::memory_order_acquire) && !g_inHookGuard) {
        const wchar_t* buf = DllName->Buffer;
        const USHORT totalLen = DllName->Length / sizeof(wchar_t);

        // Strip any path to get the base name (import descriptors pass bare
        // names like L"wlanmm.dll"; some callers pass \??\-prefixed paths).
        const wchar_t* base = buf;
        USHORT baseLen = totalLen;
        for (USHORT i = totalLen; i > 0; --i) {
            const wchar_t ch = buf[i - 1];
            if (ch == L'\\' || ch == L'/') {
                base = buf + i;
                baseLen = static_cast<USHORT>(totalLen - i);
                break;
            }
        }
        if (baseLen >= 4 && baseLen <= 40) {
            wchar_t nameBuf[48]{};
            memcpy(nameBuf, base, baseLen * sizeof(wchar_t));
            for (size_t i = 0; i < kRuntimeDllCount; ++i) {
                if (_wcsicmp(nameBuf, kRuntimeDllNames[i]) == 0) {
                    // g_managedFullPaths was fully written before the
                    // g_vanStoreReady release store; the acquire load above
                    // makes the contents visible to this thread.
                    const size_t pathLen = wcslen(g_managedFullPaths[i]);
                    if (pathLen > 0 && pathLen < 600) {
                        wchar_t fullPath[600];
                        memcpy(fullPath, g_managedFullPaths[i], (pathLen + 1) * sizeof(wchar_t));
                        UNICODE_STRING us{};
                        us.Buffer = fullPath;
                        us.Length = static_cast<USHORT>(pathLen * sizeof(wchar_t));
                        us.MaximumLength = us.Length + sizeof(wchar_t);
                        return g_origLdrLoadDll(DllPath, DllCharacteristics, &us, DllHandle);
                    }
                }
            }
        }
    }
    return g_origLdrLoadDll(DllPath, DllCharacteristics, DllName, DllHandle);
}

using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
LoadLibraryExW_t g_origLoadLibraryExW = nullptr;

HMODULE WINAPI Hook_LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    if (lpLibFileName && g_vanStoreReady.load(std::memory_order_acquire) && !g_inHookGuard) {
        if (const wchar_t* managed = MatchManagedDllCanonicalName(lpLibFileName)) {
            ScopedHookGuard guard;
            const std::filesystem::path target = GetManagedDllPath(managed);
            DWORD effectiveFlags = dwFlags;
            if ((effectiveFlags & (LOAD_LIBRARY_AS_DATAFILE |
                                   LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
                                   LOAD_LIBRARY_AS_IMAGE_RESOURCE)) == 0) {
                // Full path + altered search: dependencies resolve from the
                // VanStore folder (all 8 hydrated DLLs live there).
                effectiveFlags &= ~LOAD_LIBRARY_SEARCH_SYSTEM32;
                effectiveFlags |= LOAD_WITH_ALTERED_SEARCH_PATH;
            }
            HMODULE hMod = g_origLoadLibraryExW(target.c_str(), hFile, effectiveFlags);
            if (hMod && (effectiveFlags & (LOAD_LIBRARY_AS_DATAFILE |
                                           LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
                                           LOAD_LIBRARY_AS_IMAGE_RESOURCE)) == 0) {
                std::lock_guard<std::mutex> lock(g_stateMutex);
                if (std::find(g_loadedVanModules.begin(), g_loadedVanModules.end(), hMod) ==
                    g_loadedVanModules.end()) {
                    g_loadedVanModules.push_back(hMod);
                }
            }
            return hMod;
        }
    }
    return g_origLoadLibraryExW(lpLibFileName, hFile, dwFlags);
}

using LoadLibraryW_t = HMODULE(WINAPI*)(LPCWSTR);
LoadLibraryW_t g_origLoadLibraryW = nullptr;

HMODULE WINAPI Hook_LoadLibraryW(LPCWSTR lpLibFileName) {
    if (lpLibFileName && g_vanStoreReady.load(std::memory_order_acquire) && !g_inHookGuard) {
        if (const wchar_t* managed = MatchManagedDllCanonicalName(lpLibFileName)) {
            return LoadManagedDll(managed);
        }
    }
    return g_origLoadLibraryW(lpLibFileName);
}

// ===========================================================================
// In-proc COM resolution
// ===========================================================================
HRESULT TryGetInProcManagedClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) {
    for (size_t i = 0; i < kInProcComCount; ++i) {
        if (IsEqualGUID(rclsid, kInProcComTable[i].clsid)) {
            HMODULE hMod = LoadManagedDll(kInProcComTable[i].dllBaseName);
            if (!hMod) {
                return HRESULT_FROM_WIN32(GetLastError());
            }
            using DllGetClassObject_t = HRESULT(WINAPI*)(REFCLSID, REFIID, LPVOID*);
            auto pfn = ResolveExportAddress<DllGetClassObject_t>(hMod, "DllGetClassObject");
            if (!pfn) {
                return CLASS_E_CLASSNOTAVAILABLE;
            }
            return pfn(rclsid, riid, ppv);
        }
    }
    return CLASS_E_CLASSNOTAVAILABLE;
}

using CoGetClassObject_t = HRESULT(WINAPI*)(REFCLSID, DWORD, LPVOID, REFIID, LPVOID*);
CoGetClassObject_t g_origCoGetClassObject = nullptr;

HRESULT WINAPI Hook_CoGetClassObject(
    REFCLSID rclsid, DWORD dwClsContext, LPVOID pvReserved, REFIID riid, LPVOID* ppv) {
    if (ppv && g_vanStoreReady.load(std::memory_order_acquire) &&
        !g_shuttingDown.load(std::memory_order_acquire) && !g_inHookGuard) {
        ScopedHookGuard guard;
        HRESULT hr = TryGetInProcManagedClassObject(rclsid, riid, ppv);
        if (SUCCEEDED(hr)) {
            return hr;
        }
    }
    return g_origCoGetClassObject(rclsid, dwClsContext, pvReserved, riid, ppv);
}

using CoCreateInstance_t = HRESULT(WINAPI*)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
CoCreateInstance_t g_origCoCreateInstance = nullptr;

HRESULT WINAPI Hook_CoCreateInstance(
    REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID* ppv) {
    if (ppv && g_vanStoreReady.load(std::memory_order_acquire) &&
        !g_shuttingDown.load(std::memory_order_acquire) && !g_inHookGuard) {
        ScopedHookGuard guard;
        IClassFactory* pFactory = nullptr;
        if (SUCCEEDED(TryGetInProcManagedClassObject(
                rclsid, IID_IClassFactory, reinterpret_cast<void**>(&pFactory))) &&
            pFactory) {
            HRESULT hr = pFactory->CreateInstance(pUnkOuter, riid, ppv);
            pFactory->Release();
            return hr;
        }
    }
    return g_origCoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);
}

// ===========================================================================
// In-memory LoadStringW fallback (safety net if RT_STRING hydration misses)
// ===========================================================================
using LoadStringW_t = int(WINAPI*)(HINSTANCE, UINT, LPWSTR, int);
LoadStringW_t g_origLoadStringW = nullptr;

int WINAPI Hook_LoadStringW(HINSTANCE hInstance, UINT uID, LPWSTR lpBuffer, int cchBufferMax) {
    int res = g_origLoadStringW(hInstance, uID, lpBuffer, cchBufferMax);
    if (res > 0 || !hInstance || !lpBuffer || cchBufferMax <= 0) {
        return res;
    }

    wchar_t modPath[MAX_PATH * 2]{};
    if (GetModuleFileNameW(hInstance, modPath, ARRAYSIZE(modPath)) == 0) {
        return 0;
    }
    const wchar_t* managed = MatchManagedDllCanonicalName(modPath);
    if (!managed) {
        return 0;
    }

    const bool useIt = IsEffectiveItalian();
    for (size_t i = 0; i < kHydratedStringCount; ++i) {
        const LocalizedResourceEntry& e = kHydratedStrings[i];
        if (e.id == uID && _wcsicmp(e.moduleLower, managed) == 0) {
            const wchar_t* text = useIt ? e.itText : e.enText;
            const size_t len = wcslen(text);
            const size_t toCopy = (std::min<size_t>)(len, static_cast<size_t>(cchBufferMax - 1));
            memcpy(lpBuffer, text, toCopy * sizeof(wchar_t));
            lpBuffer[toCopy] = L'\0';
            return static_cast<int>(toCopy);
        }
    }
    return 0;
}

// ===========================================================================
// Flyout invocation & cross-explorer routing
// ===========================================================================
bool IsVanFlyoutWindowVisible() noexcept {
    // Classic Windows 8 VAN side-pane window classes hosted inside explorer.exe
    for (const wchar_t* cls : {L"VANUIFrame", L"NativeHWNDHost", L"DirectUIHWND"}) {
        HWND hWnd = FindWindowExW(nullptr, nullptr, cls, L"Networks");
        if (!hWnd) {
            hWnd = FindWindowExW(nullptr, nullptr, cls, L"Reti");
        }
        if (hWnd && IsWindowVisible(hWnd)) {
            DWORD pid = 0;
            GetWindowThreadProcessId(hWnd, &pid);
            if (pid == GetCurrentProcessId()) {
                return true;
            }
        }
    }
    return false;
}

void InvokeShowVanOnStaThread(bool allowToggleClose) {
    if (g_shuttingDown.load(std::memory_order_acquire)) {
        return;
    }

    if (!g_vanModule) {
        g_vanModule = LoadManagedDll(L"van.dll");
        if (!g_vanModule) {
            Wh_Log(L"Failed loading hydrated van.dll (%lu)", GetLastError());
            return;
        }
        // Plain names (real 22621 export table) -> ordinals -> decorated.
        g_pfnShowVAN = ResolveVanExport<ShowVAN_t>(
            g_vanModule, "ShowVAN", 2, "?ShowVAN@@YAJ_N0PEAUIPopupBase@@@Z");
        g_pfnHideVAN =
            ResolveVanExport<HideVAN_t>(g_vanModule, "HideVAN", 1, "?HideVAN@@YAXXZ");
        g_pfnShutdownVAN = ResolveVanExport<ShutdownVAN_t>(
            g_vanModule, "ShutdownVAN", 3, "?ShutdownVAN@@YAXXZ");
        Wh_Log(L"Hydrated van.dll loaded; ShowVAN=%p HideVAN=%p ShutdownVAN=%p",
               reinterpret_cast<void*>(g_pfnShowVAN), reinterpret_cast<void*>(g_pfnHideVAN),
               reinterpret_cast<void*>(g_pfnShutdownVAN));
    }

    if (!g_pfnShowVAN) {
        Wh_Log(L"ShowVAN export missing in van.dll");
        return;
    }

    if (allowToggleClose && g_pfnHideVAN && IsVanFlyoutWindowVisible()) {
        g_pfnHideVAN();
        return;
    }

    // ShowVAN spawns and manages its internal RunVANUI thread by itself;
    // never create a parallel VAN UI thread here.
    HRESULT hr = g_pfnShowVAN(false, false, nullptr);
    if (FAILED(hr)) {
        Wh_Log(L"ShowVAN returned HRESULT 0x%08lX", static_cast<unsigned long>(hr));
    }
}

// May be called from ANY thread (hooks run on the taskbar thread etc.).
void RequestShowVanFlyout(bool allowToggleClose) {
    if (!g_vanStoreReady.load(std::memory_order_acquire) ||
        g_shuttingDown.load(std::memory_order_acquire)) {
        return;
    }

    const ULONGLONG now = GetTickCount64();
    const ULONGLONG prev = g_lastShowVanTick.exchange(now, std::memory_order_acq_rel);
    if (now - prev < 180) {
        return;
    }

    // Secondary explorer.exe instances forward to the taskbar-owning host so
    // the flyout always appears next to the real taskbar.
    if (g_hostRole.load(std::memory_order_acquire) ==
        static_cast<int>(HostRole::Secondary)) {
        HWND hHost = FindWindowExW(HWND_MESSAGE, nullptr, kVanHostWndClassName, nullptr);
        if (hHost) {
            PostMessageW(hHost, g_wmShowVanIpc, allowToggleClose ? 1 : 0, 0);
            return;
        }
        // Permissive: no host found (taskbar owner crashed?) - try locally.
    }

    const DWORD tid = g_staThreadId.load(std::memory_order_acquire);
    if (tid != 0) {
        PostThreadMessageW(tid, WM_APP_SHOW_VAN, allowToggleClose ? 1 : 0, 0);
    }
}

// ===========================================================================
// pnidui.dll tray icon subclassing (classic icon click layer)
// ===========================================================================
HWND g_pnidTrayHwnd = nullptr;
UINT g_pnidTrayCallbackMsg = 0;

// Matches WindhawkUtils::WH_SUBCLASSPROC: (HWND, UINT, WPARAM, LPARAM,
// DWORD_PTR dwRefData). The wrapper removes the subclass itself on
// WM_NCDESTROY, so no manual RemoveWindowSubclass is needed here.
LRESULT CALLBACK PnidTrayWndSubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR /*dwRefData*/) {
    if (uMsg == WM_NCDESTROY) {
        if (g_pnidTrayHwnd == hWnd) {
            g_pnidTrayHwnd = nullptr;
            g_pnidTrayCallbackMsg = 0;
        }
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    if (g_pnidTrayCallbackMsg != 0 && uMsg == g_pnidTrayCallbackMsg) {
        const UINT notifyEvent = LOWORD(lParam);
        if (notifyEvent == WM_LBUTTONUP || notifyEvent == NIN_SELECT ||
            notifyEvent == NIN_KEYSELECT || notifyEvent == WM_LBUTTONDBLCLK) {
            RequestShowVanFlyout(/*allowToggleClose=*/true);
            return 0;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

using Shell_NotifyIconW_t = BOOL(WINAPI*)(DWORD, PNOTIFYICONDATAW);
Shell_NotifyIconW_t g_origShell_NotifyIconW = nullptr;

BOOL WINAPI Hook_Shell_NotifyIconW(DWORD dwMessage, PNOTIFYICONDATAW lpData) {
    if (lpData && lpData->hWnd &&
        (dwMessage == NIM_ADD || dwMessage == NIM_MODIFY) &&
        (lpData->uFlags & NIF_MESSAGE) && lpData->uCallbackMessage != 0 &&
        g_vanStoreReady.load(std::memory_order_acquire)) {
        const HWND hFallback = g_fallbackTrayHwnd.load(std::memory_order_acquire);
        if (lpData->hWnd != hFallback) {
            // pnidui.dll's CLSID_NetworkTray registers its callback window
            // from the STA worker thread of this process.
            const DWORD staTid = g_staThreadId.load(std::memory_order_acquire);
            if (staTid != 0 && GetCurrentThreadId() == staTid) {
                g_pnidTrayHwnd = lpData->hWnd;
                g_pnidTrayCallbackMsg = lpData->uCallbackMessage;
                WindhawkUtils::SetWindowSubclassFromAnyThread(
                    lpData->hWnd, PnidTrayWndSubclassProc, 0);
            }
        }
    }
    return g_origShell_NotifyIconW(dwMessage, lpData);
}

// ===========================================================================
// Layer 1: modern flyout activation interception (WindowsCreateString)
// ===========================================================================
constexpr wchar_t kModernFlyoutActivationName[] =
    L"Windows.Internal.ShellExperience.NetworkFlyout";
constexpr UINT32 kModernFlyoutActivationLen =
    (sizeof(kModernFlyoutActivationName) / sizeof(wchar_t)) - 1;

using WindowsCreateString_t = HRESULT(WINAPI*)(PCWSTR, UINT32, HSTRING*);
WindowsCreateString_t g_origWindowsCreateString = nullptr;

HRESULT WINAPI Hook_WindowsCreateString(PCWSTR sourceString, UINT32 length, HSTRING* string) {
    // WindowsCreateString accepts non-NUL-terminated buffers with an explicit
    // length, so compare with wcsncmp and only require a terminator when the
    // caller relied on the NUL convention (length == 0 usage).
    if (sourceString && g_vanStoreReady.load(std::memory_order_acquire) &&
        !g_shuttingDown.load(std::memory_order_acquire) &&
        (length == kModernFlyoutActivationLen || length == 0) &&
        wcsncmp(sourceString, kModernFlyoutActivationName, kModernFlyoutActivationLen) == 0 &&
        (length != 0 || sourceString[kModernFlyoutActivationLen] == L'\0')) {
        Wh_Log(L"Intercepted modern NetworkFlyout activation request");
        RequestShowVanFlyout(/*allowToggleClose=*/true);
        return E_ABORT;
    }
    return g_origWindowsCreateString(sourceString, length, string);
}

// ===========================================================================
// Layer 2: WH_MOUSE hit-test on the system network icon rectangle
// ===========================================================================
HHOOK g_trayMouseHook = nullptr;

bool GetModernNetworkIconRect(RECT* outRect) {
    NOTIFYICONIDENTIFIER ident{};
    ident.cbSize = sizeof(ident);
    ident.hWnd = nullptr;
    ident.uID = 0;
    ident.guidItem = kModernNetworkIconGuid;
    return SUCCEEDED(Shell_NotifyIconGetRect(&ident, outRect));
}

bool IsNativeNetworkTrayIconVisible() {
    RECT rc{};
    return GetModernNetworkIconRect(&rc);
}

LRESULT CALLBACK VanTrayMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION &&
        (wParam == WM_LBUTTONDOWN || wParam == WM_LBUTTONUP || wParam == WM_LBUTTONDBLCLK)) {
        const auto* pm = reinterpret_cast<const MOUSEHOOKSTRUCT*>(lParam);
        if (pm) {
            RECT rc{};
            if (GetModernNetworkIconRect(&rc) && PtInRect(&rc, pm->pt)) {
                if (wParam == WM_LBUTTONUP || wParam == WM_LBUTTONDBLCLK) {
                    RequestShowVanFlyout(/*allowToggleClose=*/true);
                }
                // Swallow the click on the system network icon so the modern
                // flyout does not open on top of the classic one.
                return 1;
            }
        }
    }
    return CallNextHookEx(g_trayMouseHook, nCode, wParam, lParam);
}

// ===========================================================================
// Layer 4: ms-availablenetworks: URI interception
// ===========================================================================
using ShellExecuteExW_t = BOOL(WINAPI*)(SHELLEXECUTEINFOW*);
ShellExecuteExW_t g_origShellExecuteExW = nullptr;

BOOL WINAPI Hook_ShellExecuteExW(SHELLEXECUTEINFOW* pExecInfo) {
    if (pExecInfo && pExecInfo->lpFile &&
        g_interceptAvailableNetworksUri.load(std::memory_order_relaxed) &&
        g_vanStoreReady.load(std::memory_order_acquire)) {
        if (StartsWithInsensitive(pExecInfo->lpFile, L"ms-availablenetworks:")) {
            RequestShowVanFlyout(/*allowToggleClose=*/true);
            pExecInfo->hInstApp = reinterpret_cast<HINSTANCE>(42);
            return TRUE;
        }
    }
    return g_origShellExecuteExW(pExecInfo);
}

using CreateUri_t = HRESULT(WINAPI*)(LPCWSTR, DWORD, DWORD_PTR, IUri**);
CreateUri_t g_origCreateUri = nullptr;

HRESULT WINAPI Hook_CreateUri(LPCWSTR pwzURI, DWORD dwFlags, DWORD_PTR dwReserved, IUri** ppURI) {
    if (pwzURI &&
        g_interceptAvailableNetworksUri.load(std::memory_order_relaxed) &&
        g_vanStoreReady.load(std::memory_order_acquire)) {
        if (StartsWithInsensitive(pwzURI, L"ms-availablenetworks:")) {
            RequestShowVanFlyout(/*allowToggleClose=*/true);
            return E_ABORT;
        }
    }
    return g_origCreateUri(pwzURI, dwFlags, dwReserved, ppURI);
}

// ===========================================================================
// Optional WLAN adapter filtering
// ===========================================================================
using WlanEnumInterfaces_t = DWORD(WINAPI*)(HANDLE, PVOID, PWLAN_INTERFACE_INFO_LIST*);
WlanEnumInterfaces_t g_origWlanEnumInterfaces = nullptr;

DWORD WINAPI Hook_WlanEnumInterfaces(
    HANDLE hClientHandle, PVOID pReserved, PWLAN_INTERFACE_INFO_LIST* ppInterfaceList) {
    DWORD err = g_origWlanEnumInterfaces(hClientHandle, pReserved, ppInterfaceList);
    if (err == ERROR_SUCCESS && ppInterfaceList && *ppInterfaceList &&
        g_hideDuplicateWlanAdapters.load(std::memory_order_relaxed) &&
        (*ppInterfaceList)->dwNumberOfItems > 1) {
        (*ppInterfaceList)->dwNumberOfItems = 1;
    }
    return err;
}

// ===========================================================================
// Smart fallback tray icon (live NLM status, hosted by the mod itself)
// ===========================================================================
constexpr wchar_t kFallbackTrayWndClass[] = L"WindhawkWin8VanFallbackTrayWnd";
constexpr UINT kFallbackTrayIconUid = 0x56414E32;  // 'VAN2'
constexpr GUID kFallbackTrayIconGuid = {
    0x6B1D8A44, 0x37C2, 0x4E20, {0x8F, 0x0D, 0x9A, 0x11, 0x08, 0x6A, 0x1F, 0x33}};

class FallbackTrayIcon {
  public:
    bool Start() {
        if (m_added) {
            return true;
        }

        HMODULE hMod = nullptr;
        GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&FallbackTrayIcon::WndProc), &hMod);

        WNDCLASSW wc{};
        wc.lpfnWndProc = &FallbackTrayIcon::WndProc;
        wc.hInstance = hMod;
        wc.lpszClassName = kFallbackTrayWndClass;
        RegisterClassW(&wc);

        // A hidden top-level (not message-only) window: tray context menus
        // need SetForegroundWindow to dismiss properly.
        m_hwnd = CreateWindowExW(
            WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kFallbackTrayWndClass, L"", WS_POPUP, 0, 0,
            0, 0, nullptr, nullptr, hMod, nullptr);
        if (!m_hwnd) {
            Wh_Log(L"Fallback tray window creation failed (%lu)", GetLastError());
            return false;
        }
        SetWindowLongPtrW(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        g_fallbackTrayHwnd.store(m_hwnd, std::memory_order_release);

        m_icon = LoadFallbackIcon();
        if (!CoCreateNlm()) {
            Wh_Log(L"Network List Manager unavailable for fallback icon");
        }

        NOTIFYICONDATAW nid{};
        nid.cbSize = sizeof(nid);
        nid.hWnd = m_hwnd;
        nid.uID = kFallbackTrayIconUid;
        nid.guidItem = kFallbackTrayIconGuid;
        nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_GUID | NIF_SHOWTIP;
        nid.uCallbackMessage = WM_APP_FALLBACK_TRAY_CB;
        nid.hIcon = m_icon;
        wcscpy(nid.szTip, L"Network");
        if (!Shell_NotifyIconW(NIM_ADD, &nid)) {
            Wh_Log(L"Shell_NotifyIconW(NIM_ADD) failed for fallback icon (%lu)",
                   GetLastError());
            Stop();
            return false;
        }
        m_added = true;
        RefreshStatus();
        Wh_Log(L"Live fallback network tray icon added");
        return true;
    }

    void Stop() {
        if (m_hwnd) {
            NOTIFYICONDATAW nid{};
            nid.cbSize = sizeof(nid);
            nid.hWnd = m_hwnd;
            nid.uID = kFallbackTrayIconUid;
            nid.guidItem = kFallbackTrayIconGuid;
            nid.uFlags = NIF_GUID;
            Shell_NotifyIconW(NIM_DELETE, &nid);
        }
        g_fallbackTrayHwnd.store(nullptr, std::memory_order_release);
        m_added = false;
        if (m_nlm) {
            m_nlm->Release();
            m_nlm = nullptr;
        }
        if (m_icon) {
            DestroyIcon(m_icon);
            m_icon = nullptr;
        }
        if (m_hwnd) {
            DestroyWindow(m_hwnd);
            m_hwnd = nullptr;
        }
    }

    void RefreshStatus() {
        if (!m_added || !m_hwnd) {
            return;
        }
        bool internet = false;
        bool anyConnection = false;
        if (m_nlm) {
            VARIANT_BOOL vb = VARIANT_FALSE;
            if (SUCCEEDED(NlmIsConnectedToInternet(m_nlm, &vb, 0))) {
                internet = (vb == VARIANT_TRUE);
            }
            vb = VARIANT_FALSE;
            if (SUCCEEDED(NlmIsConnected(m_nlm, &vb, 0))) {
                anyConnection = (vb == VARIANT_TRUE);
            }
        }

        UINT stringId = 8;  // "Internet access"
        if (!internet) {
            stringId = anyConnection ? 35 : 2;  // "No Internet access" / "Not connected..."
        }
        const wchar_t* tip = GetHydratedStringFor(L"pnidui.dll", stringId, IsEffectiveItalian());
        if (!tip) {
            tip = L"Network";
        }

        NOTIFYICONDATAW nid{};
        nid.cbSize = sizeof(nid);
        nid.hWnd = m_hwnd;
        nid.uID = kFallbackTrayIconUid;
        nid.guidItem = kFallbackTrayIconGuid;
        nid.uFlags = NIF_GUID | NIF_TIP | NIF_SHOWTIP;
        wcsncpy(nid.szTip, tip, ARRAYSIZE(nid.szTip) - 1);
        nid.szTip[ARRAYSIZE(nid.szTip) - 1] = L'\0';
        Shell_NotifyIconW(NIM_MODIFY, &nid);
    }

  private:
    static HICON LoadFallbackIcon() {
        // Prefer a genuine network icon from the hydrated pnidui.dll copy.
        const std::filesystem::path pnidui = GetManagedDllPath(L"pnidui.dll");
        HMODULE hData = LoadLibraryExW(
            pnidui.c_str(), nullptr,
            LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
        if (hData) {
            static constexpr WORD kCandidateGroups[] = {
                3020, 3021, 3022, 3023, 3024, 3025, 3027, 3028, 3029, 3030, 3031, 3032,
            };
            const int cx = GetSystemMetrics(SM_CXSMICON);
            const int cy = GetSystemMetrics(SM_CYSMICON);
            for (WORD group : kCandidateGroups) {
                HICON icon = reinterpret_cast<HICON>(LoadImageW(
                    hData, MAKEINTRESOURCEW(group), IMAGE_ICON, cx, cy, 0));
                if (icon) {
                    FreeLibrary(hData);
                    return icon;
                }
            }
            FreeLibrary(hData);
        }
        return LoadIconW(nullptr, MAKEINTRESOURCEW(32512));  // IDI_APPLICATION
    }

    bool CoCreateNlm() {
        return SUCCEEDED(CoCreateInstance(
            kCLSID_NetworkListManager, nullptr, CLSCTX_ALL, kIID_INetworkListManager,
            reinterpret_cast<void**>(&m_nlm)));
    }

    static LRESULT CALLBACK WndProc(
        HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (uMsg == WM_APP_FALLBACK_TRAY_CB) {
            const UINT notifyEvent = LOWORD(lParam);
            if (notifyEvent == WM_LBUTTONUP || notifyEvent == NIN_SELECT ||
                notifyEvent == NIN_KEYSELECT || notifyEvent == WM_LBUTTONDBLCLK) {
                RequestShowVanFlyout(/*allowToggleClose=*/true);
                return 0;
            }
            if (notifyEvent == WM_RBUTTONUP || notifyEvent == WM_CONTEXTMENU) {
                ShowContextMenu(hWnd);
                return 0;
            }
        }
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    static void ShowContextMenu(HWND hWnd) {
        const bool it = IsEffectiveItalian();
        HMENU menu = CreatePopupMenu();
        AppendMenuW(
            menu, MF_STRING, 1, it ? L"Apri riquadro reti" : L"Open network flyout");
        AppendMenuW(
            menu, MF_STRING, 2, it ? L"Impostazioni di rete" : L"Network settings");
        SetForegroundWindow(hWnd);
        POINT pt{};
        GetCursorPos(&pt);
        const int cmd = TrackPopupMenu(
            menu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY, pt.x, pt.y, 0, hWnd,
            nullptr);
        DestroyMenu(menu);
        if (cmd == 1) {
            RequestShowVanFlyout(/*allowToggleClose=*/true);
        } else if (cmd == 2) {
            ShellExecuteW(
                nullptr, L"open", L"ms-settings:network-status", nullptr, nullptr,
                SW_SHOWNORMAL);
        }
    }

    HWND m_hwnd = nullptr;
    HICON m_icon = nullptr;
    INetworkListManager* m_nlm = nullptr;
    bool m_added = false;
};

// ===========================================================================
// Classic pnidui.dll NetworkTray host controller
// ===========================================================================
class NetworkTrayHostController {
  public:
    void Start() {
        if (m_trayObject) {
            return;
        }
        IClassFactory* pFactory = nullptr;
        HRESULT hr = TryGetInProcManagedClassObject(
            CLSID_LegacyNetworkTray, IID_IClassFactory, reinterpret_cast<void**>(&pFactory));
        if (FAILED(hr) || !pFactory) {
            Wh_Log(L"CLSID_NetworkTray factory failed (0x%08lX)",
                   static_cast<unsigned long>(hr));
            return;
        }

        // pnidui 22621 has no IOleCommandTarget: activation goes through the
        // private CNwSSO interface (Start/Stop).
        INetworkTrayPrivate* pTray = nullptr;
        hr = pFactory->CreateInstance(
            nullptr, IID_NetworkTrayPrivate, reinterpret_cast<void**>(&pTray));
        pFactory->Release();
        if (FAILED(hr) || !pTray) {
            Wh_Log(L"CLSID_NetworkTray CreateInstance(IID_NetworkTrayPrivate) failed "
                   L"(0x%08lX)",
                   static_cast<unsigned long>(hr));
            return;
        }

        // CNwSSO::Start(flags=4, out1, out2) with a 0x38-byte zeroed context
        // blob: out1 = ctx+0x18 (receives a void*), out2 = ctx+0x10 (receives
        // a DWORD). Runs on our STA worker thread so the object gets its
        // message pump, like the tray thread stobject used to provide.
        m_trayContext = LocalAlloc(LMEM_ZEROINIT, 0x38);
        if (!m_trayContext) {
            Wh_Log(L"NetworkTray context LocalAlloc failed (%lu)", GetLastError());
            pTray->Release();
            return;
        }
        BYTE* base = reinterpret_cast<BYTE*>(m_trayContext);
        hr = pTray->Activate(4, base + 0x18, base + 0x10);
        if (FAILED(hr)) {
            Wh_Log(L"NetworkTray Start(4) returned 0x%08lX",
                   static_cast<unsigned long>(hr));
            LocalFree(m_trayContext);
            m_trayContext = nullptr;
            pTray->Release();
            return;
        }
        m_trayObject = pTray;
        Wh_Log(L"Classic pnidui.dll Network Tray icon started (Start hr=0x%08lX).",
               static_cast<unsigned long>(hr));
    }

    void Stop() {
        if (!m_trayObject) {
            return;
        }
        if (g_pnidTrayHwnd && IsWindow(g_pnidTrayHwnd)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                g_pnidTrayHwnd, PnidTrayWndSubclassProc);
        }
        g_pnidTrayHwnd = nullptr;
        g_pnidTrayCallbackMsg = 0;

        // CNwSSO::Stop() tears the tray icon down cleanly.
        m_trayObject->Cleanup();
        LocalFree(m_trayContext);
        m_trayContext = nullptr;
        m_trayObject->Release();
        m_trayObject = nullptr;
        Wh_Log(L"Classic pnidui.dll Network Tray icon stopped.");
    }

    bool IsActive() const { return m_trayObject != nullptr; }

  private:
    INetworkTrayPrivate* m_trayObject = nullptr;
    HLOCAL m_trayContext = nullptr;
};

// ===========================================================================
// STA worker: provisioning, hosting, IPC window, timers, message pump
// ===========================================================================
#ifndef MOD_NOREPEAT
#define MOD_NOREPEAT 0x4000
#endif
NetworkTrayHostController g_trayHostController;
FallbackTrayIcon g_fallbackTrayIcon;
HWND g_staCtrlWnd = nullptr;
bool g_hotkeyRegistered = false;
int g_hostMissCount = 0;

void ReevaluateHostingOnSta();

LRESULT CALLBACK VanStaWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_NCDESTROY) {
        if (g_staCtrlWnd == hWnd) {
            g_staCtrlWnd = nullptr;
        }
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    if (uMsg == g_wmShowVanIpc && g_wmShowVanIpc != 0) {
        // Cross-process flyout request from a secondary explorer.exe.
        InvokeShowVanOnStaThread(wParam != 0);
        return 0;
    }

    if (uMsg == WM_HOTKEY && wParam == kHotkeyId) {
        RequestShowVanFlyout(/*allowToggleClose=*/true);
        return 0;
    }

    if (uMsg == WM_TIMER) {
        if (wParam == kStaTimerHostReeval) {
            ReevaluateHostingOnSta();
            return 0;
        }
        if (wParam == kStaTimerFallbackRefresh) {
            g_fallbackTrayIcon.RefreshStatus();
            return 0;
        }
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

HWND CreateStaWindow(const wchar_t* className) {
    HMODULE hMod = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&VanStaWndProc), &hMod);

    WNDCLASSW wc{};
    wc.lpfnWndProc = &VanStaWndProc;
    wc.hInstance = hMod;
    wc.lpszClassName = className;
    RegisterClassW(&wc);

    // Message-only window: timers, hotkeys and IPC messages only.
    return CreateWindowExW(
        0, className, L"", WS_OVERLAPPED, 0, 0, 0, 0, HWND_MESSAGE, nullptr, hMod, nullptr);
}

bool InstallTrayMouseHook() {
    if (g_trayMouseHook) {
        return true;
    }
    HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD pid = 0;
    if (!hTray) {
        return false;
    }
    const DWORD tid = GetWindowThreadProcessId(hTray, &pid);
    if (tid == 0 || pid != GetCurrentProcessId()) {
        return false;
    }
    HMODULE hMod = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&VanTrayMouseHookProc), &hMod);
    g_trayMouseHook = SetWindowsHookExW(WH_MOUSE, VanTrayMouseHookProc, hMod, tid);
    if (g_trayMouseHook) {
        Wh_Log(L"WH_MOUSE hook installed on the Shell_TrayWnd thread (%lu)", tid);
    }
    return g_trayMouseHook != nullptr;
}

void RemoveTrayMouseHook() {
    if (g_trayMouseHook) {
        UnhookWindowsHookEx(g_trayMouseHook);
        g_trayMouseHook = nullptr;
    }
}

void ApplyHotkeyRegistration() {
    const bool want = g_enableHotkey.load(std::memory_order_relaxed);
    if (want && !g_hotkeyRegistered && g_staCtrlWnd) {
        g_hotkeyRegistered =
            RegisterHotKey(g_staCtrlWnd, kHotkeyId, MOD_WIN | MOD_SHIFT | MOD_NOREPEAT, L'N');
        Wh_Log(L"Win+Shift+N hotkey registration %s", g_hotkeyRegistered ? L"succeeded" : L"failed");
    } else if (!want && g_hotkeyRegistered && g_staCtrlWnd) {
        UnregisterHotKey(g_staCtrlWnd, kHotkeyId);
        g_hotkeyRegistered = false;
        Wh_Log(L"Win+Shift+N hotkey unregistered");
    }
}

void SyncTrayHostingOnSta() {
    const bool wantClassic = g_enableLegacyTrayIcon.load(std::memory_order_relaxed);
    if (wantClassic && !g_trayHostController.IsActive()) {
        g_trayHostController.Start();
    } else if (!wantClassic && g_trayHostController.IsActive()) {
        g_trayHostController.Stop();
    }

    // Smart fallback: guarantee that a clickable network icon always exists.
    const bool classicActive = g_trayHostController.IsActive();
    const bool nativeVisible = IsNativeNetworkTrayIconVisible();
    if (!classicActive && !nativeVisible) {
        g_fallbackTrayIcon.Start();
    } else if (classicActive || nativeVisible) {
        g_fallbackTrayIcon.Stop();
    }
}

void EnterHostModeOnSta() {
    Wh_Log(L"This explorer.exe takes over as flyout/tray host (owns Shell_TrayWnd)");
    g_hostRole.store(static_cast<int>(HostRole::Host), std::memory_order_release);

    if (g_staCtrlWnd) {
        DestroyWindow(g_staCtrlWnd);
        g_staCtrlWnd = nullptr;
    }
    g_staCtrlWnd = CreateStaWindow(kVanHostWndClassName);
    if (!g_staCtrlWnd) {
        Wh_Log(L"Host window creation failed (%lu)", GetLastError());
    }

    SetTimer(g_staCtrlWnd, kStaTimerHostReeval, 5000, nullptr);
    SetTimer(g_staCtrlWnd, kStaTimerFallbackRefresh, 2000, nullptr);

    InstallTrayMouseHook();
    ApplyHotkeyRegistration();
    SyncTrayHostingOnSta();
}

void EnterSecondaryModeOnSta() {
    Wh_Log(L"This explorer.exe runs as secondary (another process owns Shell_TrayWnd)");
    g_hostRole.store(static_cast<int>(HostRole::Secondary), std::memory_order_release);

    RemoveTrayMouseHook();
    if (g_hotkeyRegistered && g_staCtrlWnd) {
        UnregisterHotKey(g_staCtrlWnd, kHotkeyId);
        g_hotkeyRegistered = false;
    }
    g_trayHostController.Stop();
    g_fallbackTrayIcon.Stop();

    if (g_staCtrlWnd) {
        DestroyWindow(g_staCtrlWnd);
        g_staCtrlWnd = nullptr;
    }
    g_staCtrlWnd = CreateStaWindow(kVanCtrlWndClassName);
    SetTimer(g_staCtrlWnd, kStaTimerHostReeval, 5000, nullptr);
}

void ReevaluateHostingOnSta() {
    if (g_shuttingDown.load(std::memory_order_acquire)) {
        return;
    }
    HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD pid = 0;
    if (hTray) {
        GetWindowThreadProcessId(hTray, &pid);
        g_hostMissCount = 0;
    } else {
        pid = 0;
        ++g_hostMissCount;
    }

    const int role = g_hostRole.load(std::memory_order_relaxed);
    if (pid == GetCurrentProcessId()) {
        if (role != static_cast<int>(HostRole::Host)) {
            EnterHostModeOnSta();
        } else {
            InstallTrayMouseHook();  // re-attach after taskbar re-creation
            SyncTrayHostingOnSta();
        }
    } else if (pid != 0) {
        if (role == static_cast<int>(HostRole::Host)) {
            EnterSecondaryModeOnSta();
        }
    } else if (g_hostMissCount >= 12 && role != static_cast<int>(HostRole::Host)) {
        // No taskbar anywhere for a while - volunteer (permissive).
        EnterHostModeOnSta();
    }
}

DWORD WINAPI VanStaWorkerThreadProc(LPVOID) {
    HRESULT hrCom = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    const bool comInitialized = SUCCEEDED(hrCom);

    MSG dummyMsg{};
    PeekMessageW(&dummyMsg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    // 1. Provision the shared VanStore (coordinated via the named mutex).
    if (!EnsureVanStoreProvisioned() || g_shuttingDown.load(std::memory_order_acquire)) {
        Wh_Log(L"VanStore not ready - flyout restorer staying passive this session");
        if (comInitialized) {
            CoUninitialize();
        }
        return 0;
    }

    // 2. Publish precomputed full paths, then flip g_vanStoreReady.
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        for (size_t i = 0; i < kRuntimeDllCount; ++i) {
            const std::filesystem::path full = g_vanStoreDir / kRuntimeDllNames[i];
            const std::wstring& s = full.wstring();
            const size_t n = (std::min<size_t>)(s.size(), 599);
            memcpy(g_managedFullPaths[i], s.c_str(), n * sizeof(wchar_t));
            g_managedFullPaths[i][n] = L'\0';
        }
        g_vanStoreReady.store(true, std::memory_order_release);
        g_staThreadId.store(GetCurrentThreadId(), std::memory_order_release);
    }
    Wh_Log(L"VanStore ready at %s", g_vanStoreDir.c_str());

    // 3. Registry overlay + real per-user COM registrations (idempotent).
    if (!PopulateVirtualRegistryOverlay()) {
        Wh_Log(L"WARNING: virtual registry overlay population failed");
    }
    PruneAndAddInstancePid();

    // 4. Let LoadLibrary* resolve sub-DLLs straight from the VanStore.
    {
        const std::filesystem::path dir = GetManagedDllPath(L"van.dll").parent_path();
        g_dllDirectoryCookie = AddDllDirectory(dir.c_str());
        if (g_dllDirectoryCookie) {
            Wh_Log(L"AddDllDirectory(VanStore) active");
        }
    }

    // 5. Host election: wait (max ~12s) for the taskbar to be created, then
    //    host only if this explorer.exe owns Shell_TrayWnd.
    {
        bool decided = false;
        for (int i = 0; i < 48 && !decided &&
             !g_shuttingDown.load(std::memory_order_acquire); ++i) {
            HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
            DWORD pid = 0;
            if (hTray) {
                GetWindowThreadProcessId(hTray, &pid);
                if (pid == GetCurrentProcessId()) {
                    EnterHostModeOnSta();
                    decided = true;
                } else if (i >= 8) {
                    // Give the primary explorer time to settle, then accept
                    // the secondary role.
                    EnterSecondaryModeOnSta();
                    decided = true;
                }
            } else if (i == 47) {
                EnterHostModeOnSta();  // permissive: no taskbar anywhere
                decided = true;
            }
            if (!decided) {
                WaitForSingleObject(g_stopEvent, 250);
            }
        }
    }

    // 6. STA message + stop event pump.
    while (!g_shuttingDown.load(std::memory_order_acquire)) {
        DWORD waitRes = MsgWaitForMultipleObjectsEx(
            1, &g_stopEvent, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (waitRes == WAIT_OBJECT_0 || g_shuttingDown.load(std::memory_order_acquire)) {
            break;
        }
        if (waitRes == WAIT_OBJECT_0 + 1) {
            MSG msg{};
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    g_shuttingDown.store(true, std::memory_order_release);
                    break;
                }
                if (msg.message == WM_APP_SHOW_VAN) {
                    InvokeShowVanOnStaThread(msg.wParam != 0);
                    continue;
                }
                if (msg.message == WM_APP_SYNC_TRAY) {
                    SyncTrayHostingOnSta();
                    ApplyHotkeyRegistration();
                    continue;
                }
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }

    // 7. Deterministic shutdown (all on the owning STA thread):
    //    tray first, then the VAN UI thread via van.dll's own ShutdownVAN.
    RemoveTrayMouseHook();
    if (g_hotkeyRegistered && g_staCtrlWnd) {
        UnregisterHotKey(g_staCtrlWnd, kHotkeyId);
        g_hotkeyRegistered = false;
    }
    g_trayHostController.Stop();
    g_fallbackTrayIcon.Stop();
    if (g_staCtrlWnd) {
        KillTimer(g_staCtrlWnd, kStaTimerHostReeval);
        KillTimer(g_staCtrlWnd, kStaTimerFallbackRefresh);
        DestroyWindow(g_staCtrlWnd);
        g_staCtrlWnd = nullptr;
    }

    if (g_pfnHideVAN) {
        g_pfnHideVAN();
    }
    if (g_pfnShutdownVAN) {
        g_pfnShutdownVAN();  // joins the internal RunVANUI thread
    }

    g_hostRole.store(static_cast<int>(HostRole::Undetermined), std::memory_order_release);
    g_vanStoreReady.store(false, std::memory_order_release);
    g_staThreadId.store(0, std::memory_order_release);

    if (comInitialized) {
        CoFreeUnusedLibrariesEx(0, 0);
        CoUninitialize();
    }
    return 0;
}

}  // namespace

// ===========================================================================
// Windhawk lifecycle
// ===========================================================================
BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Windows 8 Network Flyout Restorer v1.1.2...");

    LoadModSettings();

    wchar_t storageRoot[MAX_PATH * 2]{};
    if (!Wh_GetModStoragePath(storageRoot, ARRAYSIZE(storageRoot))) {
        Wh_Log(L"Wh_GetModStoragePath failed");
        return FALSE;
    }

    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        g_vanStoreBaseDir = std::filesystem::path(storageRoot) / L"VanStore";
        g_rawCacheDir = std::filesystem::path(storageRoot) / L"MsdlRawCache";
        g_vanStoreDir = g_vanStoreBaseDir / L"gen_1";
    }

    g_shuttingDown.store(false, std::memory_order_release);
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }

    g_wmShowVanIpc = RegisterWindowMessageW(kVanIpcMessageName);
    if (g_wmShowVanIpc == 0) {
        Wh_Log(L"RegisterWindowMessageW failed (%lu) - using WM_APP fallback",
               GetLastError());
        g_wmShowVanIpc = WM_APP + 0x390;
    }

    // Cross-explorer provisioning mutex (abandoned instances are recovered).
    g_provisionMutex = CreateMutexW(nullptr, FALSE, kProvisionMutexName);
    if (!g_provisionMutex) {
        Wh_Log(L"CreateMutexW(provision) failed (%lu)", GetLastError());
    }

    // Resolve ntdll helpers + capture the NT path of HKCU (e.g.
    // "\Registry\User\S-1-5-21-...") so NtOpenKey/NtOpenKeyEx redirection can
    // build absolute redirect targets.
    {
        HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
        if (hNtdll) {
            g_pfnNtOpenKey = reinterpret_cast<NtOpenKey_t>(
                reinterpret_cast<void*>(GetProcAddress(hNtdll, "NtOpenKey")));
            g_pfnNtOpenKeyEx = reinterpret_cast<NtOpenKeyEx_t>(
                reinterpret_cast<void*>(GetProcAddress(hNtdll, "NtOpenKeyEx")));
            g_pfnNtQueryKey = reinterpret_cast<NtQueryKey_t>(
                reinterpret_cast<void*>(GetProcAddress(hNtdll, "NtQueryKey")));
        }
        if (g_pfnNtQueryKey) {
            HKEY hKey = nullptr;
            const LONG openRes =
                RegOpenKeyExW(HKEY_CURRENT_USER, nullptr, 0, KEY_QUERY_VALUE, &hKey);
            if (openRes != ERROR_SUCCESS) {
                Wh_Log(L"Prefix capture: RegOpenKeyExW(HKCU) failed with %ld", openRes);
            } else {
                BYTE buf[1024];
                ULONG returned = 0;
                // KeyNameInformation returns {ULONG NameLength; WCHAR Name[]}
                // with the full NT path, e.g. "\Registry\User\S-1-5-21-...".
                const NTSTATUS querySt = g_pfnNtQueryKey(
                    hKey, kKeyNameInformationClass, buf, sizeof(buf), &returned);
                if (querySt != 0) {
                    Wh_Log(L"Prefix capture: NtQueryKey(KeyNameInformation) returned "
                           L"0x%08lX",
                           static_cast<unsigned long>(querySt));
                } else {
                    auto* nameInfo = reinterpret_cast<WhKeyNameInformation*>(buf);
                    if (nameInfo->NameLength > 0 &&
                        nameInfo->NameLength % sizeof(wchar_t) == 0 &&
                        nameInfo->NameLength <= sizeof(buf) - sizeof(ULONG)) {
                        g_hkcuNtPrefix.assign(
                            nameInfo->Name, nameInfo->NameLength / sizeof(wchar_t));
                    } else {
                        Wh_Log(L"Prefix capture: NtQueryKey returned invalid "
                               L"NameLength %lu",
                               static_cast<unsigned long>(nameInfo->NameLength));
                    }
                }
                RegCloseKey(hKey);
            }
        }
        if (g_hkcuNtPrefix.empty()) {
            // Documented fallback: HKEY_CURRENT_USER is mapped to
            // \Registry\User\<SID> in the NT object namespace, and the SID
            // can be taken straight from the process token
            // (GetTokenInformation/ConvertSidToStringSidW).
            HANDLE token = nullptr;
            if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
                DWORD needed = 0;
                GetTokenInformation(token, TokenUser, nullptr, 0, &needed);
                if (needed > 0 && needed <= 4096) {
                    std::vector<BYTE> tokenUserBuf(needed, 0);
                    if (GetTokenInformation(token, TokenUser, tokenUserBuf.data(), needed,
                                            &needed)) {
                        wchar_t* sidStr = nullptr;
                        auto* tokenUser =
                            reinterpret_cast<TOKEN_USER*>(tokenUserBuf.data());
                        if (ConvertSidToStringSidW(tokenUser->User.Sid, &sidStr) &&
                            sidStr) {
                            g_hkcuNtPrefix = L"\\Registry\\User\\";
                            g_hkcuNtPrefix += sidStr;
                            LocalFree(sidStr);
                        }
                    }
                }
                CloseHandle(token);
            }
            if (g_hkcuNtPrefix.empty()) {
                Wh_Log(L"WARNING: HKCU NT prefix could not be captured; "
                       L"NtOpenKey relative-open redirection will be limited");
            } else {
                Wh_Log(L"HKCU NT prefix derived from the process token SID: %s",
                       g_hkcuNtPrefix.c_str());
            }
        }
        Wh_Log(L"NT hooks ready: NtOpenKey=%p NtOpenKeyEx=%p NtQueryKey=%p "
               L"HKCU NT prefix=%s",
               reinterpret_cast<void*>(g_pfnNtOpenKey),
               reinterpret_cast<void*>(g_pfnNtOpenKeyEx),
               reinterpret_cast<void*>(g_pfnNtQueryKey), g_hkcuNtPrefix.c_str());
    }

    // Install hooks. Targets are resolved explicitly and every hook is
    // optional-safe: a missing export simply skips that layer.
    HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    HMODULE hAdvapi32 = GetModuleHandleW(L"advapi32.dll");
    HMODULE hComBase = GetModuleHandleW(L"combase.dll");
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    HMODULE hUrlMon = LoadLibraryW(L"urlmon.dll");
    HMODULE hWlanApi = LoadLibraryW(L"wlanapi.dll");

    if (hKernelBase || hAdvapi32) {
        RegOpenKeyExW_t pRegOpenW = nullptr;
        RegOpenKeyExA_t pRegOpenA = nullptr;
        if (hKernelBase) {
            pRegOpenW = ResolveExportAddress<RegOpenKeyExW_t>(hKernelBase, "RegOpenKeyExW");
            pRegOpenA = ResolveExportAddress<RegOpenKeyExA_t>(hKernelBase, "RegOpenKeyExA");
        }
        if (!pRegOpenW && hAdvapi32) {
            pRegOpenW = ResolveExportAddress<RegOpenKeyExW_t>(hAdvapi32, "RegOpenKeyExW");
        }
        if (!pRegOpenA && hAdvapi32) {
            pRegOpenA = ResolveExportAddress<RegOpenKeyExA_t>(hAdvapi32, "RegOpenKeyExA");
        }
        if (pRegOpenW) {
            WindhawkUtils::SetFunctionHook(
                pRegOpenW, Hook_RegOpenKeyExW, &g_origRegOpenKeyExW);
        }
        if (pRegOpenA) {
            WindhawkUtils::SetFunctionHook(
                pRegOpenA, Hook_RegOpenKeyExA, &g_origRegOpenKeyExA);
        }
        RegGetValueW_t pRegGetValueW = nullptr;
        if (hKernelBase) {
            pRegGetValueW =
                ResolveExportAddress<RegGetValueW_t>(hKernelBase, "RegGetValueW");
        }
        if (!pRegGetValueW && hAdvapi32) {
            pRegGetValueW =
                ResolveExportAddress<RegGetValueW_t>(hAdvapi32, "RegGetValueW");
        }
        if (pRegGetValueW) {
            WindhawkUtils::SetFunctionHook(
                pRegGetValueW, Hook_RegGetValueW, &g_origRegGetValueW);
        }
        if (hKernelBase) {
            WindhawkUtils::SetFunctionHook(
                ResolveExportAddress<LoadLibraryExW_t>(hKernelBase, "LoadLibraryExW"),
                Hook_LoadLibraryExW, &g_origLoadLibraryExW);
            WindhawkUtils::SetFunctionHook(
                ResolveExportAddress<LoadLibraryW_t>(hKernelBase, "LoadLibraryW"),
                Hook_LoadLibraryW, &g_origLoadLibraryW);
        }
    }

    if (hNtdll) {
        LdrLoadDll_t pLdrLoadDll = reinterpret_cast<LdrLoadDll_t>(
            reinterpret_cast<void*>(GetProcAddress(hNtdll, "LdrLoadDll")));
        if (pLdrLoadDll) {
            WindhawkUtils::SetFunctionHook(
                pLdrLoadDll, Hook_LdrLoadDll, &g_origLdrLoadDll);
        }
        NtOpenKey_t pNtOpenKey = reinterpret_cast<NtOpenKey_t>(
            reinterpret_cast<void*>(GetProcAddress(hNtdll, "NtOpenKey")));
        if (pNtOpenKey) {
            WindhawkUtils::SetFunctionHook(
                pNtOpenKey, Hook_NtOpenKey, &g_origNtOpenKey);
        }
        NtOpenKeyEx_t pNtOpenKeyEx = reinterpret_cast<NtOpenKeyEx_t>(
            reinterpret_cast<void*>(GetProcAddress(hNtdll, "NtOpenKeyEx")));
        if (pNtOpenKeyEx) {
            WindhawkUtils::SetFunctionHook(
                pNtOpenKeyEx, Hook_NtOpenKeyEx, &g_origNtOpenKeyEx);
        }
    }

    if (hComBase) {
        WindhawkUtils::SetFunctionHook(
            ResolveExportAddress<CoGetClassObject_t>(hComBase, "CoGetClassObject"),
            Hook_CoGetClassObject, &g_origCoGetClassObject);
        WindhawkUtils::SetFunctionHook(
            ResolveExportAddress<CoCreateInstance_t>(hComBase, "CoCreateInstance"),
            Hook_CoCreateInstance, &g_origCoCreateInstance);
        WindowsCreateString_t pWindowsCreateString = ResolveExportAddress<WindowsCreateString_t>(
            hComBase, "WindowsCreateString");
        if (pWindowsCreateString) {
            WindhawkUtils::SetFunctionHook(
                pWindowsCreateString, Hook_WindowsCreateString, &g_origWindowsCreateString);
        }
    }

    if (hUser32) {
        WindhawkUtils::SetFunctionHook(
            ResolveExportAddress<LoadStringW_t>(hUser32, "LoadStringW"),
            Hook_LoadStringW, &g_origLoadStringW);
    }

    if (hShell32) {
        WindhawkUtils::SetFunctionHook(
            ResolveExportAddress<Shell_NotifyIconW_t>(hShell32, "Shell_NotifyIconW"),
            Hook_Shell_NotifyIconW, &g_origShell_NotifyIconW);
        WindhawkUtils::SetFunctionHook(
            ResolveExportAddress<ShellExecuteExW_t>(hShell32, "ShellExecuteExW"),
            Hook_ShellExecuteExW, &g_origShellExecuteExW);
    }

    if (hUrlMon) {
        WindhawkUtils::SetFunctionHook(
            ResolveExportAddress<CreateUri_t>(hUrlMon, "CreateUri"),
            Hook_CreateUri, &g_origCreateUri);
    }

    if (hWlanApi) {
        WindhawkUtils::SetFunctionHook(
            ResolveExportAddress<WlanEnumInterfaces_t>(hWlanApi, "WlanEnumInterfaces"),
            Hook_WlanEnumInterfaces, &g_origWlanEnumInterfaces);
    }

    Wh_Log(L"All hooks installed; starting the STA worker");
    return TRUE;
}

void Wh_ModAfterInit() {
    g_workerThread = CreateThread(nullptr, 0, VanStaWorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"Failed creating VanStaWorkerThreadProc (%lu)", GetLastError());
    }
}

void Wh_ModBeforeUninit() {
    // Called before Windhawk removes the inline hooks: signal the worker and
    // abort any in-flight MSDL download immediately so the hooks never run
    // against half-torn state.
    Wh_Log(L"Wh_ModBeforeUninit: signaling shutdown");
    g_shuttingDown.store(true, std::memory_order_release);
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    AbortWinHttpSession();
}

void Wh_ModSettingsChanged() {
    const bool prevIt = IsEffectiveItalian();
    LoadModSettings();
    const bool nowIt = IsEffectiveItalian();

    const DWORD tid = g_staThreadId.load(std::memory_order_acquire);
    if (tid != 0) {
        PostThreadMessageW(tid, WM_APP_SYNC_TRAY, 0, 0);
    }

    if (prevIt != nowIt) {
        // Invalidate the hydration stamp so the next explorer.exe start
        // provisions a new generation with the new language. The in-memory
        // LoadStringW fallback switches immediately.
        std::error_code ec;
        std::filesystem::remove(g_vanStoreBaseDir / kHydrationStampName, ec);
        Wh_Log(L"Flyout language changed - hydration stamp invalidated for next start");
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Windows 8 Network Flyout Restorer...");

    g_shuttingDown.store(true, std::memory_order_release);
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    const DWORD tid = g_staThreadId.load(std::memory_order_acquire);
    if (tid != 0) {
        PostThreadMessageW(tid, WM_QUIT, 0, 0);
    }

    if (g_workerThread) {
        const DWORD workerWait = WaitForSingleObject(g_workerThread, 15000);
        if (workerWait != WAIT_OBJECT_0) {
            g_workerStillRunning = true;
            Wh_Log(L"WARNING: STA worker did not exit within 15s - managed VAN "
                   L"modules will stay loaded (a controlled leak until process "
                   L"exit is far safer than unloading a library while its "
                   L"threads run)");
        }
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    AbortWinHttpSession();

    if (g_dllDirectoryCookie) {
        RemoveDllDirectory(g_dllDirectoryCookie);
        g_dllDirectoryCookie = nullptr;
    }

    // Registry artifacts are shared by every explorer.exe carrying the mod:
    // only the last live instance may remove them.
    if (RemoveInstancePidAndCheckLast()) {
        CleanupRegistryArtifacts();
    }

    if (g_provisionMutex) {
        CloseHandle(g_provisionMutex);
        g_provisionMutex = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }

    g_pfnShowVAN = nullptr;
    g_pfnHideVAN = nullptr;
    g_pfnShutdownVAN = nullptr;
    g_vanModule = nullptr;

    if (g_workerStillRunning) {
        // The STA worker (and possibly van.dll's internal RunVANUI thread) may
        // still be executing inside these modules: FreeLibrary here would be a
        // guaranteed use-after-free crash in explorer.exe. Keep them mapped
        // until the process exits.
        std::lock_guard<std::mutex> lock(g_stateMutex);
        g_loadedVanModules.clear();
        Wh_Log(L"Skipping managed module unload after worker timeout");
    } else {
        ScopedHookGuard guard;
        std::vector<HMODULE> modulesToFree;
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            modulesToFree.swap(g_loadedVanModules);
        }
        for (auto it = modulesToFree.rbegin(); it != modulesToFree.rend(); ++it) {
            if (*it) {
                FreeLibrary(*it);
            }
        }
    }

    Wh_Log(L"Windows 8 Network Flyout Restorer unloaded cleanly");
}
