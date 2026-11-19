// ==WindhawkMod==
// @id              win8-network-flyout-restorer
// @name            Windows 8 Network Flyout Restorer
// @description     This mod restores the Windows 8 Network Flyout on Windows 10 and Windows 11 (24H2+ included)
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
# Windows 8 Network Flyout Restorer

This mod restores the Windows 8-style network panel from the taskbar network icon.
The panel provides access to Wi-Fi networks, wired connections, VPN connections
and Airplane mode, where supported by Windows and the installed hardware.

This modification has been tested on Windows 10 21H2, Windows 11 24H2 and Windows 11 25H2.

On builds that still contain the legacy network components, the mod uses them
directly and downloads nothing; the result is usually closer to the original
Windows 8 panel, since the system's own providers are used. On newer builds,
the required Microsoft components are downloaded from Microsoft's Symbol Server
and verified with SHA-256 before use.

![winserverpicture](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/refs/heads/main/winserverpicture.png)

Note: This is a best-effort modification. The Windows 8 network flyout and its dependencies had been removed in Windows 11 24H2+ and the restoration is done within modern Windows' limitations.
## Requirements

- 64-bit Windows 10 (build 17763 or later) or Windows 11.
- Windhawk 1.4 or later.
- An internet connection for the first download, if additional components are needed.
- Native ARM64 Windows is not supported: fallback artifacts are AMD64-only and
  the native component route is unavailable on ARM64 builds.

## How it works

Compatible Windows components already on the computer are used when available.
Otherwise, the required Microsoft components are downloaded from Microsoft's
Symbol Server and kept in Windhawk's private storage. System files are not replaced.
Compatibility depends on the Windows build; the panel may not work on every system.

Downloaded files are checked with SHA-256 before use. Damaged cached downloads are
replaced automatically when possible. If verification still fails, the downloaded
panel remains inactive rather than loading an unverified file.

## Options

The classic network icon is optional and disabled by default. If enabling it
causes Explorer to restart, leave it disabled and use the alternative network icon.
The settings control the classic network icon, the optional Win+Shift+N shortcut
and the panel language. The downloaded panel supports English, Italian, Spanish,
French and Brazilian Portuguese. When built-in Windows components are used, their
original Windows language is retained. Restart Explorer after changing the panel language.

## Privacy and system changes

The mod does not create or modify entries in the live HKCU/HKLM registry and
does not create system-wide COM registrations. In fallback mode, registry access
needed by the legacy components is redirected to a private application hive
loaded with `RegLoadAppKeyW` from a unique file in this mod's Windhawk storage.
Each `explorer.exe` instance gets its own hive file, so concurrent Explorer
processes do not contend for one backing file. The hive is not mounted under
HKCU/HKLM and is not visible to other processes. On normal unload the mod closes
the hive and attempts to remove its file; a crash can leave an orphaned hive
file in mod storage. Downloaded components are also kept in Windhawk storage to
avoid downloading them at every start.

If you used an earlier release, its `HKCU\Software\Windhawk\Mods\win8-network-flyout-restorer\VirtualRegistry`
subtree (or the older `windows-8-network-flyout-restorer` name) may still be
present. This release does not delete old registry data automatically; after
all older mod/Explorer instances have stopped, remove those legacy entries
manually if you want to clean them up.

## Troubleshooting

If a handled C++ error occurs, the mod stops accepting requests for that session
and attempts to shut down its features. Restart Explorer before trying again.
This does not protect Explorer from every possible error in legacy Windows components.

If the panel does not open, check the internet connection and Windhawk's mod log.
An SHA-256 mismatch is not ignored. Disabling the mod and restarting Explorer
restores the normal Windows behavior. If shutdown gets stuck, some resources may
be kept until Explorer exits to avoid unloading code that is still running.
## Credits 
- AdministratoX - Testing on Windows 11 25H2
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableLegacyTrayIcon: false
  $name: "Enable classic network tray icon (pnidui.dll)"
  $description: "This setting enables the classic network icon when downloaded components are used. The built-in Windows mode uses the system icon; an alternative icon is shown if no clickable network icon is available."
- interceptAvailableNetworksUri: true
  $name: "Redirect ms-availablenetworks to Windows 8 flyout"
  $description: "This setting redirects ms-availablenetworks links opened by Explorer to the Windows 8 network panel."
- uiLanguage: auto
  $name: "Flyout UI language"
  $description: "This setting selects the language of the downloaded network panel and its connection prompts. Built-in Windows components keep their original language. Restart Explorer after changing the panel language."
  $options:
  - auto: "Automatic (match Windows UI language; unsupported languages fall back to English)"
  - it: "Italiano"
  - en: "English"
  - es: "Español"
  - fr: "Français"
  - pt: "Português (Brasil)"
- hideDuplicateWlanAdapters: false
  $name: "Hide secondary virtual Wi-Fi adapters"
  $description: "This setting hides additional Wi-Fi interfaces in the panel when virtual hotspot or container adapters create duplicate entries, keeping only the first interface reported by Windows."
- enableHotkey: false
  $name: "Enable Win+Shift+N hotkey"
  $description: "This setting enables the Win+Shift+N shortcut to open or close the Windows 8 network panel."
*/
// ==/WindhawkModSettings==

#include <cstddef>
using std::nullptr_t;
#include <windhawk_utils.h>

#include <windows.h>
#include <bcrypt.h>
#include <wincrypt.h>
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
#include <exception>
#include <new>
#include <map>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

#if defined(__clang__)
#define WIN8_NET_NO_DESTROY [[clang::no_destroy]]
#else
#define WIN8_NET_NO_DESTROY
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
constexpr const wchar_t* kHydrationStampName = L"hydrated_v1_2_1_integrity.txt";
constexpr const wchar_t* kHydrationManifestName = L"runtime.sha256";
// XWizards is copied into the private application hive without inheriting
// the machine key's security descriptor.
constexpr const wchar_t* kXWizardsHiveSubKey = L"XWizardsV2";
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
// the original resource sections); the five columns provide best-effort
// en/it/es/fr/pt hydration. DirectUI font descriptors (e.g.
// "11;semibold;none;segoe ui") must never be translated.
// ===========================================================================
struct LocalizedResourceEntry {
    const wchar_t* moduleLower;
    UINT id;
    std::array<const wchar_t*, 5> text;  // EN, IT, ES, FR, PT
};

constexpr LocalizedResourceEntry kHydratedStrings[] = {
    // Font descriptors and localized strings. Font syntax is invariant across locales.
    {L"van.dll", 2200, {
        L"11;normal;none;segoe ui",
        L"11;normal;none;segoe ui",
        L"11;normal;none;segoe ui",
        L"11;normal;none;segoe ui",
        L"11;normal;none;segoe ui",
    }},
    {L"van.dll", 2201, {
        L"11;semibold;none;segoe ui",
        L"11;semibold;none;segoe ui",
        L"11;semibold;none;segoe ui",
        L"11;semibold;none;segoe ui",
        L"11;semibold;none;segoe ui",
    }},
    {L"van.dll", 2203, {
        L"16;semilight;none;segoe ui",
        L"16;semilight;none;segoe ui",
        L"16;semilight;none;segoe ui",
        L"16;semilight;none;segoe ui",
        L"16;semilight;none;segoe ui",
    }},
    {L"wlanconn.dll", 2101, {
        L"11;semibold;none;segoe ui",
        L"11;semibold;none;segoe ui",
        L"11;semibold;none;segoe ui",
        L"11;semibold;none;segoe ui",
        L"11;semibold;none;segoe ui",
    }},
    {L"setproxycredential.dll", 18420, {
        L"11;semibold;none;segoe ui",
        L"11;semibold;none;segoe ui",
        L"11;semibold;none;segoe ui",
        L"11;semibold;none;segoe ui",
        L"11;semibold;none;segoe ui",
    }},
    {L"setproxycredential.dll", 18421, {
        L"11;normal;none;segoe ui",
        L"11;normal;none;segoe ui",
        L"11;normal;none;segoe ui",
        L"11;normal;none;segoe ui",
        L"11;normal;none;segoe ui",
    }},
    // --- van.dll flyout strings ---
    {L"van.dll", 2001, {
        L"No connections are available",
        L"Nessuna connessione disponibile",
        L"No hay conexiones disponibles",
        L"Aucune connexion disponible",
        L"Nenhuma conex\u00e3o dispon\u00edvel",
    }},
    {L"van.dll", 2002, {
        L"Connections are available",
        L"Connessioni disponibili",
        L"Hay conexiones disponibles",
        L"Des connexions sont disponibles",
        L"H\u00e1 conex\u00f5es dispon\u00edveis",
    }},
    {L"van.dll", 2003, {
        L"Discovering networks...",
        L"Individuazione delle reti in corso...",
        L"Detectando redes...",
        L"D\u00e9tection des r\u00e9seaux...",
        L"Detectando redes...",
    }},
    {L"van.dll", 2005, {
        L"Not connected",
        L"Non connesso",
        L"No conectado",
        L"Non connect\u00e9",
        L"N\u00e3o conectado",
    }},
    {L"van.dll", 2007, {
        L"Unified Connection Manager",
        L"Unified Connection Manager",
        L"Unified Connection Manager",
        L"Unified Connection Manager",
        L"Unified Connection Manager",
    }},
    {L"van.dll", 2008, {
        L"Please select a network first before creating a connection profile.",
        L"Selezionare prima una rete per creare un profilo di connessione.",
        L"Seleccione primero una red antes de crear un perfil de conexi\u00f3n.",
        L"S\u00e9lectionnez d'abord un r\u00e9seau avant de cr\u00e9er un profil de connexion.",
        L"Selecione primeiro uma rede antes de criar um perfil de conex\u00e3o.",
    }},
    {L"van.dll", 2009, {
        L"Not connected - Connections are available",
        L"Non connesso - Connessioni disponibili",
        L"No conectado: hay conexiones disponibles",
        L"Non connect\u00e9 : des connexions sont disponibles",
        L"N\u00e3o conectado: h\u00e1 conex\u00f5es dispon\u00edveis",
    }},
    {L"van.dll", 2010, {
        L"Not connected - No connections are available",
        L"Non connesso - Nessuna connessione disponibile",
        L"No conectado: no hay conexiones disponibles",
        L"Non connect\u00e9 : aucune connexion disponible",
        L"N\u00e3o conectado: nenhuma conex\u00e3o dispon\u00edvel",
    }},
    {L"van.dll", 2012, {
        L"Information sent over this network might be visible to others.",
        L"Le informazioni inviate su questa rete potrebbero essere visibili ad altri utenti.",
        L"La informaci\u00f3n enviada por esta red podr\u00eda ser visible para otras personas.",
        L"Les informations envoy\u00e9es sur ce r\u00e9seau peuvent \u00eatre visibles par d'autres.",
        L"As informa\u00e7\u00f5es enviadas nesta rede podem ser vis\u00edveis para outras pessoas.",
    }},
    {L"van.dll", 2013, {
        L"Connections",
        L"Connessioni",
        L"Conexiones",
        L"Connexions",
        L"Conex\u00f5es",
    }},
    {L"van.dll", 2014, {
        L"No networks found",
        L"Nessuna rete trovata",
        L"No se encontraron redes",
        L"Aucun r\u00e9seau trouv\u00e9",
        L"Nenhuma rede encontrada",
    }},
    {L"van.dll", 2015, {
        L" (Off)",
        L" (disattivata)",
        L" (Desactivado)",
        L" (D\u00e9sactiv\u00e9)",
        L" (Desativado)",
    }},
    {L"van.dll", 2016, {
        L"Airplane mode",
        L"Modalit\u00e0 aereo",
        L"Modo avi\u00f3n",
        L"Mode avion",
        L"Modo avi\u00e3o",
    }},
    {L"van.dll", 2017, {
        L"On",
        L"Attivata",
        L"Activado",
        L"Activ\u00e9",
        L"Ativado",
    }},
    {L"van.dll", 2018, {
        L"Off",
        L"Disattivata",
        L"Desactivado",
        L"D\u00e9sactiv\u00e9",
        L"Desativado",
    }},
    {L"van.dll", 2102, {
        L"Troubleshoot",
        L"Risoluzione dei problemi",
        L"Solucionar problemas",
        L"R\u00e9soudre les probl\u00e8mes",
        L"Solucionar problemas",
    }},
    {L"van.dll", 2103, {
        L"Networks",
        L"Reti",
        L"Redes",
        L"R\u00e9seaux",
        L"Redes",
    }},
    {L"van.dll", 2300, {
        L"Connect",
        L"Connetti",
        L"Conectar",
        L"Se connecter",
        L"Conectar",
    }},
    {L"van.dll", 2301, {
        L"Disconnect",
        L"Disconnetti",
        L"Desconectar",
        L"Se d\u00e9connecter",
        L"Desconectar",
    }},
    {L"van.dll", 2302, {
        L"Turn off wireless",
        L"Disattiva wireless",
        L"Desactivar la conexi\u00f3n inal\u00e1mbrica",
        L"D\u00e9sactiver le sans fil",
        L"Desativar sem fio",
    }},
    {L"van.dll", 2303, {
        L"Diagnose",
        L"Diagnostica",
        L"Diagnosticar",
        L"Diagnostiquer",
        L"Diagnosticar",
    }},
    {L"van.dll", 2304, {
        L"Status",
        L"Stato",
        L"Estado",
        L"\u00c9tat",
        L"Status",
    }},
    {L"van.dll", 2305, {
        L"Properties",
        L"Propriet\u00e0",
        L"Propiedades",
        L"Propri\u00e9t\u00e9s",
        L"Propriedades",
    }},
    {L"van.dll", 2400, {
        L"Connect To",
        L"Connessione a",
        L"Conectar a",
        L"Se connecter \u00e0",
        L"Conectar-se a",
    }},
    {L"van.dll", 2401, {
        L"Shows available wireless networks and dial-up and VPN connections that you can connect to.",
        L"Mostra le reti wireless disponibili e le connessioni remote e VPN a cui \u00e8 "
        L"possibile connettersi.",
        L"Muestra las redes inal\u00e1mbricas disponibles y las conexiones de acceso telef\u00f3nico "
        L"y VPN a las que puede conectarse.",
        L"Affiche les r\u00e9seaux sans fil disponibles et les connexions d'acc\u00e8s \u00e0 distance et "
        L"VPN auxquelles vous pouvez vous connecter.",
        L"Mostra as redes sem fio dispon\u00edveis e as conex\u00f5es dial-up e VPN \u00e0s quais voc\u00ea "
        L"pode se conectar.",
    }},
    {L"van.dll", 2402, {
        L"Open Without Diagnostics",
        L"Apri senza diagnostica",
        L"Abrir sin diagn\u00f3stico",
        L"Ouvrir sans diagnostic",
        L"Abrir sem diagn\u00f3stico",
    }},
    {L"van.dll", 3001, {
        L"&Connect",
        L"&Connetti",
        L"&Conectar",
        L"&Se connecter",
        L"&Conectar",
    }},
    {L"van.dll", 3002, {
        L"&Disconnect",
        L"&Disconnetti",
        L"&Desconectar",
        L"&Se d\u00e9connecter",
        L"&Desconectar",
    }},
    {L"van.dll", 3003, {
        L"Connect automatically",
        L"Connetti automaticamente",
        L"Conectar autom\u00e1ticamente",
        L"Se connecter automatiquement",
        L"Conectar automaticamente",
    }},
    {L"van.dll", 3011, {
        L"Internet access",
        L"Accesso a Internet",
        L"Acceso a Internet",
        L"Acc\u00e8s Internet",
        L"Acesso \u00e0 Internet",
    }},
    {L"van.dll", 3012, {
        L"No Internet access",
        L"Nessun accesso a Internet",
        L"Sin acceso a Internet",
        L"Pas d'acc\u00e8s Internet",
        L"Sem acesso \u00e0 Internet",
    }},
    {L"van.dll", 3013, {
        L"Internet and domain access",
        L"Accesso Internet e al dominio",
        L"Acceso a Internet y al dominio",
        L"Acc\u00e8s \u00e0 Internet et au domaine",
        L"Acesso \u00e0 Internet e ao dom\u00ednio",
    }},
    {L"van.dll", 3014, {
        L"Domain access only",
        L"Solo accesso al dominio",
        L"Solo acceso al dominio",
        L"Acc\u00e8s au domaine uniquement",
        L"Somente acesso ao dom\u00ednio",
    }},
    {L"van.dll", 3015, {
        L"No network access",
        L"Nessun accesso alla rete",
        L"Sin acceso a la red",
        L"Pas d'acc\u00e8s r\u00e9seau",
        L"Sem acesso \u00e0 rede",
    }},
    {L"van.dll", 3020, {
        L"Action needed",
        L"Azione richiesta",
        L"Se necesita acci\u00f3n",
        L"Action requise",
        L"A\u00e7\u00e3o necess\u00e1ria",
    }},
    {L"van.dll", 3021, {
        L"Connected",
        L"Connesso",
        L"Conectado",
        L"Connect\u00e9",
        L"Conectado",
    }},
    {L"van.dll", 3022, {
        L"Limited",
        L"Limitato",
        L"Limitado",
        L"Limit\u00e9",
        L"Limitado",
    }},
    {L"van.dll", 3100, {
        L"This is a new router that has not been configured.",
        L"Si tratta di un nuovo router non configurato.",
        L"Este es un router nuevo que no se ha configurado.",
        L"Il s'agit d'un nouveau routeur qui n'a pas \u00e9t\u00e9 configur\u00e9.",
        L"Este \u00e9 um roteador novo que ainda n\u00e3o foi configurado.",
    }},
    {L"van.dll", 3101, {
        L"&Configure",
        L"&Configura",
        L"&Configurar",
        L"&Configurer",
        L"&Configurar",
    }},
    {L"van.dll", 3102, {
        L"Configure",
        L"Configura",
        L"Configurar",
        L"Configurer",
        L"Configurar",
    }},
    {L"van.dll", 4001, {
        L"View Available Networks",
        L"Visualizza reti disponibili",
        L"Ver redes disponibles",
        L"Afficher les r\u00e9seaux disponibles",
        L"Exibir redes dispon\u00edveis",
    }},
    {L"van.dll", 4005, {
        L"Media Manager List",
        L"Elenco gestori supporti",
        L"Lista de administradores de medios",
        L"Liste des gestionnaires de m\u00e9dias",
        L"Lista de gerenciadores de m\u00eddia",
    }},
    {L"van.dll", 4006, {
        L"Network Category Header",
        L"Intestazione categoria di rete",
        L"Encabezado de categor\u00eda de red",
        L"En-t\u00eate de cat\u00e9gorie de r\u00e9seau",
        L"Cabe\u00e7alho de categoria de rede",
    }},
    {L"van.dll", 4007, {
        L"Available Network List",
        L"Elenco reti disponibili",
        L"Lista de redes disponibles",
        L"Liste des r\u00e9seaux disponibles",
        L"Lista de redes dispon\u00edveis",
    }},
    {L"van.dll", 4010, {
        L"Configure",
        L"Configura",
        L"Configurar",
        L"Configurer",
        L"Configurar",
    }},
    {L"van.dll", 4011, {
        L"Connect",
        L"Connetti",
        L"Conectar",
        L"Se connecter",
        L"Conectar",
    }},
    {L"van.dll", 4012, {
        L"Disconnect",
        L"Disconnetti",
        L"Desconectar",
        L"Se d\u00e9connecter",
        L"Desconectar",
    }},
    {L"van.dll", 4013, {
        L"Connect automatically checkbox",
        L"Casella di controllo Connetti automaticamente",
        L"Casilla de verificaci\u00f3n Conectar autom\u00e1ticamente",
        L"Case \u00e0 cocher Se connecter automatiquement",
        L"Caixa de sele\u00e7\u00e3o Conectar automaticamente",
    }},
    {L"van.dll", 4014, {
        L"%1!s! %2!s!",
        L"%1!s! %2!s!",
        L"%1!s! %2!s!",
        L"%1!s! %2!s!",
        L"%1!s! %2!s!",
    }},
    {L"van.dll", 4015, {
        L"%1!s! %2!s!, %3!s!",
        L"%1!s! %2!s!, %3!s!",
        L"%1!s! %2!s!, %3!s!",
        L"%1!s! %2!s!, %3!s!",
        L"%1!s! %2!s!, %3!s!",
    }},
    {L"van.dll", 4017, {
        L"Signal strength: %1!d! bar out of 5",
        L"Intensit\u00e0 del segnale: %1!d! barra su 5",
        L"Intensidad de la se\u00f1al: %1!d! barra de 5",
        L"Intensit\u00e9 du signal : %1!d! barre sur 5",
        L"Intensidade do sinal: %1!d! barra de 5",
    }},
    {L"van.dll", 4018, {
        L"Signal strength: %1!d! 'bars' out of 5",
        L"Intensit\u00e0 del segnale: %1!d! barre su 5",
        L"Intensidad de la se\u00f1al: %1!d! barras de 5",
        L"Intensit\u00e9 du signal : %1!d! barres sur 5",
        L"Intensidade do sinal: %1!d! barras de 5",
    }},
    {L"van.dll", 4019, {
        L"Link Pane",
        L"Riquadro collegamento",
        L"Panel de v\u00ednculo",
        L"Volet de liaison",
        L"Painel de link",
    }},
    {L"van.dll", 4025, {
        L"Connected Network List",
        L"Elenco reti connesse",
        L"Lista de redes conectadas",
        L"Liste des r\u00e9seaux connect\u00e9s",
        L"Lista de redes conectadas",
    }},
    {L"van.dll", 4030, {
        L"Connect to a Network",
        L"Connessione a una rete",
        L"Conectarse a una red",
        L"Se connecter \u00e0 un r\u00e9seau",
        L"Conectar-se a uma rede",
    }},
    {L"van.dll", 4031, {
        L"Disconnect from a Network",
        L"Disconnessione da una rete",
        L"Desconectarse de una red",
        L"Se d\u00e9connecter d'un r\u00e9seau",
        L"Desconectar-se de uma rede",
    }},
    {L"van.dll", 4032, {
        L"Not Connectable",
        L"Non connettibile",
        L"No conectable",
        L"Non connectable",
        L"N\u00e3o conect\u00e1vel",
    }},
    {L"van.dll", 4033, {
        L"Configure a Network",
        L"Configurazione di una rete",
        L"Configurar una red",
        L"Configurer un r\u00e9seau",
        L"Configurar uma rede",
    }},
    {L"van.dll", 5001, {
        L"View Connection Settings",
        L"Visualizza impostazioni connessione",
        L"Ver configuraci\u00f3n de conexi\u00f3n",
        L"Afficher les param\u00e8tres de connexion",
        L"Exibir configura\u00e7\u00f5es de conex\u00e3o",
    }},
    // --- wlanconn.dll connection prompts ---
    {L"wlanconn.dll", 550, {
        L"Connect to a Network",
        L"Connessione a una rete",
        L"Conectarse a una red",
        L"Se connecter \u00e0 un r\u00e9seau",
        L"Conectar-se a uma rede",
    }},
    {L"wlanconn.dll", 551, {
        L"Disconnect from a Network",
        L"Disconnessione da una rete",
        L"Desconectarse de una red",
        L"Se d\u00e9connecter d'un r\u00e9seau",
        L"Desconectar-se de uma rede",
    }},
    {L"wlanconn.dll", 552, {
        L"Not Connectable",
        L"Non connettibile",
        L"No conectable",
        L"Non connectable",
        L"N\u00e3o conect\u00e1vel",
    }},
    {L"wlanconn.dll", 553, {
        L"Connect",
        L"Connetti",
        L"Conectar",
        L"Se connecter",
        L"Conectar",
    }},
    {L"wlanconn.dll", 554, {
        L"Disconnect",
        L"Disconnetti",
        L"Desconectar",
        L"Se d\u00e9connecter",
        L"Desconectar",
    }},
    {L"wlanconn.dll", 555, {
        L"&Connect",
        L"&Connetti",
        L"&Conectar",
        L"&Se connecter",
        L"&Conectar",
    }},
    {L"wlanconn.dll", 556, {
        L"&Disconnect",
        L"&Disconnetti",
        L"&Desconectar",
        L"&Se d\u00e9connecter",
        L"&Desconectar",
    }},
    {L"wlanconn.dll", 557, {
        L"Connection not available",
        L"Connessione non disponibile",
        L"Conexi\u00f3n no disponible",
        L"Connexion non disponible",
        L"Conex\u00e3o n\u00e3o dispon\u00edvel",
    }},
    {L"wlanconn.dll", 558, {
        L"Other people might be able to see info you send over this network",
        L"Altri utenti potrebbero visualizzare le informazioni inviate su questa rete",
        L"Es posible que otras personas puedan ver la informaci\u00f3n que env\u00eda por esta red",
        L"D'autres personnes peuvent voir les informations que vous envoyez sur ce r\u00e9seau",
        L"Outras pessoas podem ver as informa\u00e7\u00f5es que voc\u00ea envia nesta rede",
    }},
    {L"wlanconn.dll", 559, {
        L"Connect automatically checkbox",
        L"Casella di controllo Connetti automaticamente",
        L"Casilla de verificaci\u00f3n Conectar autom\u00e1ticamente",
        L"Case \u00e0 cocher Se connecter automatiquement",
        L"Caixa de sele\u00e7\u00e3o Conectar automaticamente",
    }},
    {L"wlanconn.dll", 560, {
        L"Connect automatically",
        L"Connetti automaticamente",
        L"Conectar autom\u00e1ticamente",
        L"Se connecter automatiquement",
        L"Conectar automaticamente",
    }},
    {L"wlanconn.dll", 562, {
        L"Connecting to the network is taking longer than usual",
        L"La connessione alla rete richiede pi\u00f9 tempo del previsto",
        L"La conexi\u00f3n a la red est\u00e1 tardando m\u00e1s de lo habitual",
        L"La connexion au r\u00e9seau prend plus de temps que d'habitude",
        L"A conex\u00e3o \u00e0 rede est\u00e1 demorando mais que o normal",
    }},
    {L"wlanconn.dll", 563, {
        L"Name",
        L"Nome",
        L"Nombre",
        L"Nom",
        L"Nome",
    }},
    {L"wlanconn.dll", 564, {
        L"Open web browser",
        L"Apri il browser Web",
        L"Abrir el explorador web",
        L"Ouvrir le navigateur web",
        L"Abrir o navegador da Web",
    }},
    {L"wlanconn.dll", 565, {
        L"No, open a web browser to complete the connection",
        L"No, aprire il browser Web per completare la connessione",
        L"No, abrir un explorador web para completar la conexi\u00f3n",
        L"Non, ouvrir un navigateur web pour terminer la connexion",
        L"N\u00e3o, abrir um navegador da Web para concluir a conex\u00e3o",
    }},
    {L"wlanconn.dll", 570, {
        L"Wireless network name",
        L"Nome della rete wireless",
        L"Nombre de la red inal\u00e1mbrica",
        L"Nom du r\u00e9seau sans fil",
        L"Nome da rede sem fio",
    }},
    {L"wlanconn.dll", 571, {
        L"User name",
        L"Nome utente",
        L"Nombre de usuario",
        L"Nom d'utilisateur",
        L"Nome de usu\u00e1rio",
    }},
    {L"wlanconn.dll", 572, {
        L"Password",
        L"Password",
        L"Contrase\u00f1a",
        L"Mot de passe",
        L"Senha",
    }},
    {L"wlanconn.dll", 573, {
        L"To try again, tap or click the link below to complete the connection or re-enter "
        L"your user name and password.",
        L"Per riprovare, toccare o fare clic sul collegamento seguente per completare la "
        L"connessione oppure reimmettere nome utente e password.",
        L"Para volver a intentarlo, pulse o haga clic en el v\u00ednculo siguiente para "
        L"completar la conexi\u00f3n o vuelva a escribir el nombre de usuario y la contrase\u00f1a.",
        L"Pour r\u00e9essayer, appuyez ou cliquez sur le lien ci-dessous pour terminer la "
        L"connexion ou entrez \u00e0 nouveau votre nom d'utilisateur et votre mot de passe.",
        L"Para tentar novamente, toque ou clique no link abaixo para concluir a conex\u00e3o ou "
        L"digite novamente seu nome de usu\u00e1rio e senha.",
    }},
    {L"wlanconn.dll", 574, {
        L"View error details",
        L"Visualizza dettagli errore",
        L"Ver detalles del error",
        L"Afficher les d\u00e9tails de l'erreur",
        L"Exibir detalhes do erro",
    }},
    {L"wlanconn.dll", 575, {
        L"Hide error details",
        L"Nascondi dettagli errore",
        L"Ocultar detalles del error",
        L"Masquer les d\u00e9tails de l'erreur",
        L"Ocultar detalhes do erro",
    }},
    {L"wlanconn.dll", 576, {
        L"Can\u2019t connect",
        L"Impossibile connettersi",
        L"No se puede conectar",
        L"Impossible de se connecter",
        L"N\u00e3o \u00e9 poss\u00edvel conectar",
    }},
    {L"wlanconn.dll", 577, {
        L"The user name or password didn\u2019t work. Please check it and try again.",
        L"Nome utente o password non validi. Controllare e riprovare.",
        L"El nombre de usuario o la contrase\u00f1a no son correctos. Compru\u00e9belos y vuelva a intentarlo.",
        L"Le nom d'utilisateur ou le mot de passe n'a pas fonctionn\u00e9. V\u00e9rifiez-le et r\u00e9essayez.",
        L"O nome de usu\u00e1rio ou a senha n\u00e3o funcionaram. Verifique e tente novamente.",
    }},
    {L"wlanconn.dll", 578, {
        L"Something went wrong. Please try again.",
        L"Si \u00e8 verificato un errore. Riprovare.",
        L"Algo sali\u00f3 mal. Int\u00e9ntelo de nuevo.",
        L"Un probl\u00e8me est survenu. Veuillez r\u00e9essayer.",
        L"Algo deu errado. Tente novamente.",
    }},
    {L"wlanconn.dll", 579, {
        L"There wasn\u2019t a response. Please try again.",
        L"Nessuna risposta. Riprovare.",
        L"No hubo respuesta. Int\u00e9ntelo de nuevo.",
        L"Il n'y a pas eu de r\u00e9ponse. Veuillez r\u00e9essayer.",
        L"N\u00e3o houve resposta. Tente novamente.",
    }},
    {L"wlanconn.dll", 580, {
        L"A message from your wireless provider:",
        L"Messaggio dal provider di servizi wireless:",
        L"Un mensaje de su proveedor de servicios inal\u00e1mbricos:",
        L"Un message de votre fournisseur de services sans fil :",
        L"Uma mensagem do seu provedor de servi\u00e7os sem fio:",
    }},
    {L"wlanconn.dll", 590, {
        L"Estimated usage",
        L"Utilizzo stimato",
        L"Uso estimado",
        L"Utilisation estim\u00e9e",
        L"Uso estimado",
    }},
    {L"wlanconn.dll", 591, {
        L"Reset",
        L"Reimposta",
        L"Restablecer",
        L"R\u00e9initialiser",
        L"Redefinir",
    }},
    {L"wlanconn.dll", 592, {
        L"User name",
        L"Nome utente",
        L"Nombre de usuario",
        L"Nom d'utilisateur",
        L"Nome de usu\u00e1rio",
    }},
    {L"wlanconn.dll", 593, {
        L"Password",
        L"Password",
        L"Contrase\u00f1a",
        L"Mot de passe",
        L"Senha",
    }},
    {L"wlanconn.dll", 594, {
        L"Proxy server: %1",
        L"Server proxy: %1",
        L"Servidor proxy: %1",
        L"Serveur proxy : %1",
        L"Servidor proxy: %1",
    }},
    {L"wlanconn.dll", 595, {
        L"The username or password didn\u2019t work. Please try again.",
        L"Nome utente o password non validi. Riprovare.",
        L"El nombre de usuario o la contrase\u00f1a no son correctos. Int\u00e9ntelo de nuevo.",
        L"Le nom d'utilisateur ou le mot de passe n'a pas fonctionn\u00e9. Veuillez r\u00e9essayer.",
        L"O nome de usu\u00e1rio ou a senha n\u00e3o funcionaram. Tente novamente.",
    }},
    {L"wlanconn.dll", 596, {
        L"Your proxy server sign-in info is required.",
        L"Sono necessarie le informazioni di accesso al server proxy.",
        L"Se requiere la informaci\u00f3n de inicio de sesi\u00f3n del servidor proxy.",
        L"Les informations de connexion au serveur proxy sont requises.",
        L"As informa\u00e7\u00f5es de logon do servidor proxy s\u00e3o necess\u00e1rias.",
    }},
    {L"wlanconn.dll", 597, {
        L"Continue",
        L"Continua",
        L"Continuar",
        L"Continuer",
        L"Continuar",
    }},
    {L"wlanconn.dll", 598, {
        L"Some information changed since the last time you connected. We need additional "
        L"information to complete the connection.",
        L"Alcune informazioni sono state modificate dall'ultima connessione. Per completare "
        L"la connessione sono necessarie informazioni aggiuntive.",
        L"Alguna informaci\u00f3n ha cambiado desde la \u00faltima vez que se conect\u00f3. Necesitamos "
        L"informaci\u00f3n adicional para completar la conexi\u00f3n.",
        L"Certaines informations ont chang\u00e9 depuis votre derni\u00e8re connexion. Nous avons "
        L"besoin d'informations suppl\u00e9mentaires pour terminer la connexion.",
        L"Algumas informa\u00e7\u00f5es foram alteradas desde a \u00faltima vez que voc\u00ea se conectou. "
        L"Precisamos de informa\u00e7\u00f5es adicionais para concluir a conex\u00e3o.",
    }},
    {L"wlanconn.dll", 599, {
        L"Your proxy server sign-in info is required to connect to %1.",
        L"Per connettersi a %1 sono necessarie le informazioni di accesso al server proxy.",
        L"Se requiere la informaci\u00f3n de inicio de sesi\u00f3n del servidor proxy para conectarse a %1.",
        L"Les informations de connexion au serveur proxy sont requises pour se connecter \u00e0 %1.",
        L"As informa\u00e7\u00f5es de logon do servidor proxy s\u00e3o necess\u00e1rias para se conectar a %1.",
    }},
    {L"wlanconn.dll", 1000, {
        L"C&lose",
        L"&Chiudi",
        L"C&errar",
        L"F&ermer",
        L"F&echar",
    }},
    {L"wlanconn.dll", 1001, {
        L"Checking network requirements",
        L"Verifica dei requisiti di rete in corso",
        L"Comprobando los requisitos de red",
        L"V\u00e9rification de la configuration r\u00e9seau",
        L"Verificando os requisitos de rede",
    }},
    {L"wlanconn.dll", 1002, {
        L"Connecting",
        L"Connessione in corso",
        L"Conectando",
        L"Connexion en cours",
        L"Conectando",
    }},
    {L"wlanconn.dll", 1006, {
        L"Can\u2019t connect to this network",
        L"Impossibile connettersi a questa rete",
        L"No se puede conectar a esta red",
        L"Impossible de se connecter \u00e0 ce r\u00e9seau",
        L"N\u00e3o \u00e9 poss\u00edvel conectar a esta rede",
    }},
    {L"wlanconn.dll", 1008, {
        L"Verifying and connecting",
        L"Verifica e connessione in corso",
        L"Comprobando y conectando",
        L"V\u00e9rification et connexion en cours",
        L"Verificando e conectando",
    }},
    {L"wlanconn.dll", 1013, {
        L"The connection is limited",
        L"La connessione \u00e8 limitata",
        L"La conexi\u00f3n es limitada",
        L"La connexion est limit\u00e9e",
        L"A conex\u00e3o \u00e9 limitada",
    }},
    {L"wlanconn.dll", 1014, {
        L"We can\u2019t connect to the Internet right now. Please try again.",
        L"Non \u00e8 possibile connettersi a Internet in questo momento. Riprovare.",
        L"No podemos conectarnos a Internet en este momento. Int\u00e9ntelo de nuevo.",
        L"Nous ne pouvons pas nous connecter \u00e0 Internet pour le moment. Veuillez r\u00e9essayer.",
        L"N\u00e3o \u00e9 poss\u00edvel conectar \u00e0 Internet agora. Tente novamente.",
    }},
    {L"wlanconn.dll", 1023, {
        L"Enter the name (SSID) for the network",
        L"Immettere il nome (SSID) della rete",
        L"Escriba el nombre (SSID) de la red",
        L"Entrez le nom (SSID) du r\u00e9seau",
        L"Digite o nome (SSID) da rede",
    }},
    {L"wlanconn.dll", 1027, {
        L"Invalid Network Name",
        L"Nome di rete non valido",
        L"Nombre de red no v\u00e1lido",
        L"Nom de r\u00e9seau non valide",
        L"Nome de rede inv\u00e1lido",
    }},
    {L"wlanconn.dll", 1029, {
        L"Reinsert your wireless network adapter",
        L"Reinserire la scheda di rete wireless",
        L"Vuelva a insertar el adaptador de red inal\u00e1mbrica",
        L"R\u00e9ins\u00e9rez votre carte r\u00e9seau sans fil",
        L"Reinsira o adaptador de rede sem fio",
    }},
    {L"wlanconn.dll", 1030, {
        L"Can\u2019t connect because the wireless network adapter was removed.",
        L"Impossibile connettersi perch\u00e9 la scheda di rete wireless \u00e8 stata rimossa.",
        L"No se puede conectar porque se quit\u00f3 el adaptador de red inal\u00e1mbrica.",
        L"Impossible de se connecter car la carte r\u00e9seau sans fil a \u00e9t\u00e9 retir\u00e9e.",
        L"N\u00e3o \u00e9 poss\u00edvel conectar porque o adaptador de rede sem fio foi removido.",
    }},
    {L"wlanconn.dll", 1031, {
        L"Sign in to connect",
        L"Accedere per connettersi",
        L"Iniciar sesi\u00f3n para conectarse",
        L"Se connecter pour vous connecter",
        L"Entrar para conectar",
    }},
    {L"wlanconn.dll", 1032, {
        L"Close",
        L"Chiudi",
        L"Cerrar",
        L"Fermer",
        L"Fechar",
    }},
    {L"wlanconn.dll", 1033, {
        L"Sign in",
        L"Accedi",
        L"Iniciar sesi\u00f3n",
        L"Se connecter",
        L"Entrar",
    }},
    {L"wlanconn.dll", 1036, {
        L"Help me solve connection problems",
        L"Guida per la risoluzione dei problemi di connessione",
        L"Ay\u00fadame a solucionar problemas de conexi\u00f3n",
        L"Aidez-moi \u00e0 r\u00e9soudre les probl\u00e8mes de connexion",
        L"Ajudar-me a resolver problemas de conex\u00e3o",
    }},
    {L"wlanconn.dll", 1039, {
        L"Your device is locked. Unlock it, and then try again.",
        L"Il dispositivo \u00e8 bloccato. Sbloccarlo e quindi riprovare.",
        L"El dispositivo est\u00e1 bloqueado. Desbloqu\u00e9elo y vuelva a intentarlo.",
        L"Votre appareil est verrouill\u00e9. D\u00e9verrouillez-le, puis r\u00e9essayez.",
        L"Seu dispositivo est\u00e1 bloqueado. Desbloqueie-o e tente novamente.",
    }},
    {L"wlanconn.dll", 1040, {
        L"Windows can\u2019t authenticate your connection right now. Please try again later.",
        L"Windows non \u00e8 in grado di autenticare la connessione in questo momento. Riprovare pi\u00f9 tardi.",
        L"Windows no puede autenticar la conexi\u00f3n en este momento. Int\u00e9ntelo de nuevo m\u00e1s tarde.",
        L"Windows ne peut pas authentifier votre connexion pour le moment. R\u00e9essayez plus tard.",
        L"O Windows n\u00e3o pode autenticar sua conex\u00e3o agora. Tente novamente mais tarde.",
    }},
    {L"wlanconn.dll", 1041, {
        L"Authentication failed",
        L"Autenticazione non riuscita",
        L"Error de autenticaci\u00f3n",
        L"\u00c9chec de l'authentification",
        L"Falha na autentica\u00e7\u00e3o",
    }},
    {L"wlanconn.dll", 1042, {
        L"Do you have a user name and password for this Wi-Fi hotspot?",
        L"Si dispone di nome utente e password per questo hotspot Wi-Fi?",
        L"\u00bfTiene un nombre de usuario y una contrase\u00f1a para este punto de acceso Wi-Fi?",
        L"Disposez-vous d'un nom d'utilisateur et d'un mot de passe pour ce point d'acc\u00e8s Wi-Fi ?",
        L"Voc\u00ea tem um nome de usu\u00e1rio e uma senha para este hotspot Wi-Fi?",
    }},
    {L"wlanconn.dll", 1043, {
        L"Your network administrator doesn\u2019t allow simultaneous connections to your "
        L"workplace network and another network.",
        L"L'amministratore di rete non consente connessioni simultanee alla rete aziendale "
        L"e a un'altra rete.",
        L"El administrador de red no permite conexiones simult\u00e1neas a la red del trabajo y a otra red.",
        L"Votre administrateur r\u00e9seau n'autorise pas les connexions simultan\u00e9es \u00e0 votre "
        L"r\u00e9seau professionnel et \u00e0 un autre r\u00e9seau.",
        L"O administrador de rede n\u00e3o permite conex\u00f5es simult\u00e2neas \u00e0 rede do trabalho e a outra rede.",
    }},
    {L"wlanconn.dll", 1044, {
        L"Sign-in info for proxy server",
        L"Informazioni di accesso per il server proxy",
        L"Informaci\u00f3n de inicio de sesi\u00f3n para el servidor proxy",
        L"Informations de connexion pour le serveur proxy",
        L"Informa\u00e7\u00f5es de logon para o servidor proxy",
    }},
    {L"wlanconn.dll", 1046, {
        L"Sign in to connect to %1",
        L"Accedere per connettersi a %1",
        L"Inicie sesi\u00f3n para conectarse a %1",
        L"Connectez-vous pour vous connecter \u00e0 %1",
        L"Entre para se conectar a %1",
    }},
    {L"wlanconn.dll", 1047, {
        L"Forget network",
        L"Dimentica rete",
        L"Olvidar red",
        L"Oublier le r\u00e9seau",
        L"Esquecer rede",
    }},
    {L"wlanconn.dll", 1100, {
        L"Can\u2019t set up mobile hotspot.",
        L"Impossibile configurare l'hotspot mobile.",
        L"No se puede configurar el punto de acceso m\u00f3vil.",
        L"Impossible de configurer le partage de connexion mobile.",
        L"N\u00e3o \u00e9 poss\u00edvel configurar o hotspot m\u00f3vel.",
    }},
    {L"wlanconn.dll", 1101, {
        L"To enable sharing, contact your mobile operator to activate this feature.",
        L"Per abilitare la condivisione, contattare l'operatore di telefonia mobile per "
        L"attivare questa funzionalit\u00e0.",
        L"Para habilitar el uso compartido, p\u00f3ngase en contacto con su operador m\u00f3vil para "
        L"activar esta caracter\u00edstica.",
        L"Pour activer le partage, contactez votre op\u00e9rateur mobile pour activer cette fonctionnalit\u00e9.",
        L"Para habilitar o compartilhamento, contate sua operadora de celular para ativar "
        L"este recurso.",
    }},
    {L"wlanconn.dll", 1102, {
        L"Can\u2019t connect to mobile broadband.",
        L"Impossibile connettersi alla rete a banda larga mobile.",
        L"No se puede conectar a la banda ancha m\u00f3vil.",
        L"Impossible de se connecter \u00e0 la large bande mobile.",
        L"N\u00e3o \u00e9 poss\u00edvel conectar \u00e0 banda larga m\u00f3vel.",
    }},
    {L"wlanconn.dll", 1103, {
        L"Can\u2019t connect to mobile broadband while roaming.",
        L"Impossibile connettersi alla rete a banda larga mobile durante il roaming.",
        L"No se puede conectar a la banda ancha m\u00f3vil durante la itinerancia.",
        L"Impossible de se connecter \u00e0 la large bande mobile en itin\u00e9rance.",
        L"N\u00e3o \u00e9 poss\u00edvel conectar \u00e0 banda larga m\u00f3vel em roaming.",
    }},
    {L"wlanconn.dll", 5000, {
        L"Connect to a network wizard",
        L"Procedura guidata Connessione a una rete",
        L"Asistente para conectarse a una red",
        L"Assistant Se connecter \u00e0 un r\u00e9seau",
        L"Assistente para conectar-se a uma rede",
    }},
    // --- wlanmm.dll Wi-Fi strings ---
    {L"wlanmm.dll", 150, {
        L"Security Type",
        L"Tipo di sicurezza",
        L"Tipo de seguridad",
        L"Type de s\u00e9curit\u00e9",
        L"Tipo de seguran\u00e7a",
    }},
    {L"wlanmm.dll", 200, {
        L"Your network administrator has blocked you from connecting to this network.",
        L"L'amministratore di rete ha bloccato la connessione a questa rete.",
        L"El administrador de red le ha bloqueado la conexi\u00f3n a esta red.",
        L"Votre administrateur r\u00e9seau vous a emp\u00each\u00e9 de vous connecter \u00e0 ce r\u00e9seau.",
        L"O administrador de rede bloqueou voc\u00ea de se conectar a esta rede.",
    }},
    {L"wlanmm.dll", 201, {
        L"The capability of your network adapter does not match the requirements of this network.",
        L"Le funzionalit\u00e0 della scheda di rete non corrispondono ai requisiti di questa rete.",
        L"La funcionalidad del adaptador de red no coincide con los requisitos de la red.",
        L"Les fonctionnalit\u00e9s de votre carte r\u00e9seau ne correspondent pas aux exigences de ce r\u00e9seau.",
        L"Os recursos do adaptador de rede n\u00e3o correspondem aos requisitos desta rede.",
    }},
    {L"wlanmm.dll", 202, {
        L"The settings saved on this computer for the network do not match the requirements "
        L"of the network.",
        L"Le impostazioni salvate nel computer per la rete non corrispondono ai requisiti della rete.",
        L"La configuraci\u00f3n guardada en este equipo para la red no coincide con los "
        L"requisitos de la red.",
        L"Les param\u00e8tres enregistr\u00e9s sur cet ordinateur pour le r\u00e9seau ne correspondent pas "
        L"aux exigences du r\u00e9seau.",
        L"As configura\u00e7\u00f5es salvas neste computador para a rede n\u00e3o correspondem aos "
        L"requisitos da rede.",
    }},
    {L"wlanmm.dll", 203, {
        L"Connecting",
        L"Connessione in corso",
        L"Conectando",
        L"Connexion en cours",
        L"Conectando",
    }},
    {L"wlanmm.dll", 205, {
        L"Acquiring IP Address",
        L"Acquisizione dell'indirizzo IP in corso",
        L"Obteniendo direcci\u00f3n IP",
        L"Obtention de l'adresse IP",
        L"Obtendo endere\u00e7o IP",
    }},
    {L"wlanmm.dll", 206, {
        L"Limited access",
        L"Accesso limitato",
        L"Acceso limitado",
        L"Acc\u00e8s limit\u00e9",
        L"Acesso limitado",
    }},
    {L"wlanmm.dll", 207, {
        L"Connected",
        L"Connesso",
        L"Conectado",
        L"Connect\u00e9",
        L"Conectado",
    }},
    {L"wlanmm.dll", 208, {
        L"Connected",
        L"Connesso",
        L"Conectado",
        L"Connect\u00e9",
        L"Conectado",
    }},
    {L"wlanmm.dll", 209, {
        L"Disconnecting",
        L"Disconnessione in corso",
        L"Desconectando",
        L"D\u00e9connexion en cours",
        L"Desconectando",
    }},
    {L"wlanmm.dll", 210, {
        L"Waiting for users",
        L"In attesa di utenti",
        L"Esperando a los usuarios",
        L"En attente d'utilisateurs",
        L"Aguardando usu\u00e1rios",
    }},
    {L"wlanmm.dll", 211, {
        L"This connection is not allowed by your administrator.",
        L"Questa connessione non \u00e8 consentita dall'amministratore.",
        L"El administrador no permite esta conexi\u00f3n.",
        L"Cette connexion n'est pas autoris\u00e9e par votre administrateur.",
        L"Esta conex\u00e3o n\u00e3o \u00e9 permitida pelo administrador.",
    }},
    {L"wlanmm.dll", 299, {
        L"802.11 Available Network Manager",
        L"Gestore reti disponibili 802.11",
        L"Administrador de redes disponibles 802.11",
        L"Gestionnaire de r\u00e9seaux disponibles 802.11",
        L"Gerenciador de redes dispon\u00edveis 802.11",
    }},
    {L"wlanmm.dll", 300, {
        L"Wi-Fi",
        L"Wi-Fi",
        L"Wi-Fi",
        L"Wi-Fi",
        L"Wi-Fi",
    }},
    {L"wlanmm.dll", 306, {
        L"SSID",
        L"SSID",
        L"SSID",
        L"SSID",
        L"SSID",
    }},
    {L"wlanmm.dll", 307, {
        L"Hidden network",
        L"Rete nascosta",
        L"Red oculta",
        L"R\u00e9seau masqu\u00e9",
        L"Rede oculta",
    }},
    {L"wlanmm.dll", 308, {
        L"Non broadcast",
        L"Non broadcast",
        L"No difundida",
        L"Non diffus\u00e9",
        L"N\u00e3o transmitida",
    }},
    {L"wlanmm.dll", 400, {
        L"Unknown",
        L"Sconosciuto",
        L"Desconocido",
        L"Inconnu",
        L"Desconhecido",
    }},
    {L"wlanmm.dll", 450, {
        L"Type",
        L"Tipo",
        L"Tipo",
        L"Type",
        L"Tipo",
    }},
    {L"wlanmm.dll", 601, {
        L"Show estimated data usage",
        L"Mostra utilizzo dati stimato",
        L"Mostrar uso estimado de datos",
        L"Afficher l'utilisation estim\u00e9e des donn\u00e9es",
        L"Mostrar uso estimado de dados",
    }},
    {L"wlanmm.dll", 602, {
        L"Hide estimated data usage",
        L"Nascondi utilizzo dati stimato",
        L"Ocultar uso estimado de datos",
        L"Masquer l'utilisation estim\u00e9e des donn\u00e9es",
        L"Ocultar uso estimado de dados",
    }},
    {L"wlanmm.dll", 603, {
        L"Set as metered connection",
        L"Imposta come connessione a consumo",
        L"Establecer como conexi\u00f3n de uso medido",
        L"D\u00e9finir comme connexion limit\u00e9e",
        L"Definir como conex\u00e3o limitada",
    }},
    {L"wlanmm.dll", 604, {
        L"Set as non-metered connection",
        L"Imposta come connessione non a consumo",
        L"Establecer como conexi\u00f3n no de uso medido",
        L"D\u00e9finir comme connexion non limit\u00e9e",
        L"Definir como conex\u00e3o n\u00e3o limitada",
    }},
    {L"wlanmm.dll", 605, {
        L"Forget this network",
        L"Dimentica questa rete",
        L"Olvidar esta red",
        L"Oublier ce r\u00e9seau",
        L"Esquecer esta rede",
    }},
    {L"wlanmm.dll", 5000, {
        L"Unknown Authentication",
        L"Autenticazione sconosciuta",
        L"Autenticaci\u00f3n desconocida",
        L"Authentification inconnue",
        L"Autentica\u00e7\u00e3o desconhecida",
    }},
    {L"wlanmm.dll", 5001, {
        L"Unsecured",
        L"Non protetta",
        L"No segura",
        L"Non s\u00e9curis\u00e9",
        L"N\u00e3o protegida",
    }},
    {L"wlanmm.dll", 5004, {
        L"WPA-PSK",
        L"WPA-PSK",
        L"WPA-PSK",
        L"WPA-PSK",
        L"WPA-PSK",
    }},
    {L"wlanmm.dll", 5005, {
        L"WPA2",
        L"WPA2",
        L"WPA2",
        L"WPA2",
        L"WPA2",
    }},
    {L"wlanmm.dll", 5006, {
        L"WPA2-PSK",
        L"WPA2-PSK",
        L"WPA2-PSK",
        L"WPA2-PSK",
        L"WPA2-PSK",
    }},
    {L"wlanmm.dll", 5007, {
        L"802.1x",
        L"802.1x",
        L"802.1x",
        L"802.1x",
        L"802.1x",
    }},
    {L"wlanmm.dll", 5010, {
        L"Unknown Encryption",
        L"Crittografia sconosciuta",
        L"Cifrado desconocido",
        L"Chiffrement inconnu",
        L"Criptografia desconhecida",
    }},
    {L"wlanmm.dll", 5011, {
        L"None",
        L"Nessuna",
        L"Ninguno",
        L"Aucun",
        L"Nenhum",
    }},
    {L"wlanmm.dll", 5013, {
        L"TKIP",
        L"TKIP",
        L"TKIP",
        L"TKIP",
        L"TKIP",
    }},
    // --- dot3mm.dll wired networking ---
    {L"dot3mm.dll", 300, {
        L"Wired",
        L"Ethernet",
        L"Cable",
        L"Filaire",
        L"Com fio",
    }},
    // --- rasmm.dll VPN / dial-up ---
    {L"rasmm.dll", 204, {
        L"Connecting",
        L"Connessione in corso",
        L"Conectando",
        L"Connexion en cours",
        L"Conectando",
    }},
    {L"rasmm.dll", 205, {
        L"Disconnecting",
        L"Disconnessione in corso",
        L"Desconectando",
        L"D\u00e9connexion en cours",
        L"Desconectando",
    }},
    {L"rasmm.dll", 206, {
        L"Connected",
        L"Connesso",
        L"Conectado",
        L"Connect\u00e9",
        L"Conectado",
    }},
    {L"rasmm.dll", 207, {
        L"Connected",
        L"Connesso",
        L"Conectado",
        L"Connect\u00e9",
        L"Conectado",
    }},
    {L"rasmm.dll", 208, {
        L"Connected",
        L"Connesso",
        L"Conectado",
        L"Connect\u00e9",
        L"Conectado",
    }},
    {L"rasmm.dll", 209, {
        L"Connected",
        L"Connesso",
        L"Conectado",
        L"Connect\u00e9",
        L"Conectado",
    }},
    {L"rasmm.dll", 210, {
        L"This connection is not available because there is a problem with your modem or "
        L"network adapter.",
        L"La connessione non \u00e8 disponibile perch\u00e9 si \u00e8 verificato un problema con il modem "
        L"o la scheda di rete.",
        L"Esta conexi\u00f3n no est\u00e1 disponible porque hay un problema con el m\u00f3dem o el adaptador de red.",
        L"Cette connexion n'est pas disponible car un probl\u00e8me est survenu avec votre modem "
        L"ou votre carte r\u00e9seau.",
        L"Esta conex\u00e3o n\u00e3o est\u00e1 dispon\u00edvel porque h\u00e1 um problema com o modem ou o adaptador de rede.",
    }},
    {L"rasmm.dll", 211, {
        L"Dormant",
        L"Inattiva",
        L"Inactiva",
        L"Inactive",
        L"Inativa",
    }},
    {L"rasmm.dll", 212, {
        L"Dormant",
        L"Inattiva",
        L"Inactiva",
        L"Inactive",
        L"Inativa",
    }},
    {L"rasmm.dll", 213, {
        L"Action needed",
        L"Azione richiesta",
        L"Se necesita acci\u00f3n",
        L"Action requise",
        L"A\u00e7\u00e3o necess\u00e1ria",
    }},
    {L"rasmm.dll", 214, {
        L"Unable to set up VPN Connection.",
        L"Impossibile impostare la connessione VPN.",
        L"No se puede configurar la conexi\u00f3n VPN.",
        L"Impossible de configurer la connexion VPN.",
        L"N\u00e3o \u00e9 poss\u00edvel configurar a conex\u00e3o VPN.",
    }},
    {L"rasmm.dll", 300, {
        L"Dial-up and VPN",
        L"Connessioni remote e VPN",
        L"Acceso telef\u00f3nico y VPN",
        L"Acc\u00e8s \u00e0 distance et VPN",
        L"Discagem e VPN",
    }},
    {L"rasmm.dll", 301, {
        L"This connection requires an active Internet connection.",
        L"Per questa connessione \u00e8 necessaria una connessione Internet attiva.",
        L"Esta conexi\u00f3n requiere una conexi\u00f3n a Internet activa.",
        L"Cette connexion n\u00e9cessite une connexion Internet active.",
        L"Esta conex\u00e3o requer uma conex\u00e3o \u00e0 Internet ativa.",
    }},
    {L"rasmm.dll", 302, {
        L"Your network administrator has blocked you from connecting to this network.",
        L"L'amministratore di rete ha bloccato la connessione a questa rete.",
        L"El administrador de red le ha bloqueado la conexi\u00f3n a esta red.",
        L"Votre administrateur r\u00e9seau vous a emp\u00each\u00e9 de vous connecter \u00e0 ce r\u00e9seau.",
        L"O administrador de rede bloqueou voc\u00ea de se conectar a esta rede.",
    }},
    {L"rasmm.dll", 310, {
        L"Dial-up and VPN",
        L"Connessioni remote e VPN",
        L"Acceso telef\u00f3nico y VPN",
        L"Acc\u00e8s \u00e0 distance et VPN",
        L"Discagem e VPN",
    }},
    {L"rasmm.dll", 501, {
        L"Clear saved sign-in info",
        L"Cancella le informazioni di accesso salvate",
        L"Borrar la informaci\u00f3n de inicio de sesi\u00f3n guardada",
        L"Effacer les informations de connexion enregistr\u00e9es",
        L"Limpar as informa\u00e7\u00f5es de logon salvas",
    }},
    // --- datusage.dll data usage ---
    {L"datusage.dll", 101, {
        L"%1 since a moment ago",
        L"%1 da un istante",
        L"%1 desde hace un momento",
        L"%1 depuis un instant",
        L"%1 h\u00e1 pouco",
    }},
    {L"datusage.dll", 102, {
        L"%1 since 1 minute ago",
        L"%1 da 1 minuto",
        L"%1 desde hace 1 minuto",
        L"%1 depuis 1 minute",
        L"%1 h\u00e1 1 minuto",
    }},
    {L"datusage.dll", 103, {
        L"%1 since %2!d! minutes ago",
        L"%1 da %2!d! minuti",
        L"%1 desde hace %2!d! minutos",
        L"%1 depuis %2!d! minutes",
        L"%1 h\u00e1 %2!d! minutos",
    }},
    {L"datusage.dll", 104, {
        L"%1 since 1 hour ago",
        L"%1 da 1 ora",
        L"%1 desde hace 1 hora",
        L"%1 depuis 1 heure",
        L"%1 h\u00e1 1 hora",
    }},
    {L"datusage.dll", 105, {
        L"%1 since %2!d! hours ago",
        L"%1 da %2!d! ore",
        L"%1 desde hace %2!d! horas",
        L"%1 depuis %2!d! heures",
        L"%1 h\u00e1 %2!d! horas",
    }},
    {L"datusage.dll", 106, {
        L"%1 since 1 day ago",
        L"%1 da 1 giorno",
        L"%1 desde hace 1 d\u00eda",
        L"%1 depuis 1 jour",
        L"%1 h\u00e1 1 dia",
    }},
    {L"datusage.dll", 107, {
        L"%1 since %2!d! days ago",
        L"%1 da %2!d! giorni",
        L"%1 desde hace %2!d! d\u00edas",
        L"%1 depuis %2!d! jours",
        L"%1 h\u00e1 %2!d! dias",
    }},
    {L"datusage.dll", 108, {
        L"%1 during last 60 days",
        L"%1 negli ultimi 60 giorni",
        L"%1 durante los \u00faltimos 60 d\u00edas",
        L"%1 au cours des 60 derniers jours",
        L"%1 nos \u00faltimos 60 dias",
    }},
    {L"datusage.dll", 111, {
        L"%s MB",
        L"%s MB",
        L"%s MB",
        L"%s Mo",
        L"%s MB",
    }},
    {L"datusage.dll", 112, {
        L"%s GB",
        L"%s GB",
        L"%s GB",
        L"%s Go",
        L"%s GB",
    }},
    {L"datusage.dll", 113, {
        L"%s TB",
        L"%s TB",
        L"%s TB",
        L"%s To",
        L"%s TB",
    }},
    {L"datusage.dll", 115, {
        L"Over 1000 TB",
        L"Oltre 1000 TB",
        L"M\u00e1s de 1000 TB",
        L"Plus de 1000 To",
        L"Mais de 1000 TB",
    }},
    // --- setproxycredential.dll proxy prompt ---
    {L"setproxycredential.dll", 18401, {
        L"Networks",
        L"Reti",
        L"Redes",
        L"R\u00e9seaux",
        L"Redes",
    }},
    {L"setproxycredential.dll", 18402, {
        L"Enter proxy server credentials",
        L"Immettere le credenziali del server proxy",
        L"Escriba las credenciales del servidor proxy",
        L"Entrez les informations d'identification du serveur proxy",
        L"Digite as credenciais do servidor proxy",
    }},
    {L"setproxycredential.dll", 18403, {
        L"User name",
        L"Nome utente",
        L"Nombre de usuario",
        L"Nom d'utilisateur",
        L"Nome de usu\u00e1rio",
    }},
    {L"setproxycredential.dll", 18404, {
        L"Password",
        L"Password",
        L"Contrase\u00f1a",
        L"Mot de passe",
        L"Senha",
    }},
    {L"setproxycredential.dll", 18405, {
        L"Proxy server: %1",
        L"Server proxy: %1",
        L"Servidor proxy: %1",
        L"Serveur proxy : %1",
        L"Servidor proxy: %1",
    }},
    {L"setproxycredential.dll", 18406, {
        L"The username or password was incorrect. Please try again.",
        L"Nome utente o password non corretti. Riprovare.",
        L"El nombre de usuario o la contrase\u00f1a no son correctos. Int\u00e9ntelo de nuevo.",
        L"Le nom d'utilisateur ou le mot de passe est incorrect. Veuillez r\u00e9essayer.",
        L"O nome de usu\u00e1rio ou a senha est\u00e3o incorretos. Tente novamente.",
    }},
    {L"setproxycredential.dll", 18408, {
        L"Cancel",
        L"Annulla",
        L"Cancelar",
        L"Annuler",
        L"Cancelar",
    }},
    {L"setproxycredential.dll", 18409, {
        L"Checking network requirements",
        L"Verifica dei requisiti di rete in corso",
        L"Comprobando los requisitos de red",
        L"V\u00e9rification de la configuration r\u00e9seau",
        L"Verificando os requisitos de rede",
    }},
    {L"setproxycredential.dll", 18410, {
        L"SPCPage",
        L"SPCPage",
        L"SPCPage",
        L"SPCPage",
        L"SPCPage",
    }},
    // --- pnidui.dll tray strings ---
    {L"pnidui.dll", 2, {
        L"Not connected - No connections are available",
        L"Non connesso - Nessuna connessione disponibile",
        L"No conectado: no hay conexiones disponibles",
        L"Non connect\u00e9 : aucune connexion disponible",
        L"N\u00e3o conectado: nenhuma conex\u00e3o dispon\u00edvel",
    }},
    {L"pnidui.dll", 8, {
        L"Internet access",
        L"Accesso a Internet",
        L"Acceso a Internet",
        L"Acc\u00e8s Internet",
        L"Acesso \u00e0 Internet",
    }},
    {L"pnidui.dll", 15, {
        L"Connection status - Unknown",
        L"Stato connessione - Sconosciuto",
        L"Estado de conexi\u00f3n: desconocido",
        L"\u00c9tat de la connexion : inconnu",
        L"Status da conex\u00e3o: desconhecido",
    }},
    {L"pnidui.dll", 35, {
        L"No Internet access",
        L"Nessun accesso a Internet",
        L"Sin acceso a Internet",
        L"Pas d'acc\u00e8s Internet",
        L"Sem acesso \u00e0 Internet",
    }},
    {L"pnidui.dll", 38, {
        L"Not connected - Connections are available",
        L"Non connesso - Connessioni disponibili",
        L"No conectado: hay conexiones disponibles",
        L"Non connect\u00e9 : des connexions sont disponibles",
        L"N\u00e3o conectado: h\u00e1 conex\u00f5es dispon\u00edveis",
    }},
    {L"pnidui.dll", 47, {
        L"Windows Networking",
        L"Rete di Windows",
        L"Redes de Windows",
        L"Mise en r\u00e9seau Windows",
        L"Rede do Windows",
    }},
    {L"pnidui.dll", 48, {
        L"Network System Icon",
        L"Icona di sistema Rete",
        L"Icono del sistema Red",
        L"Ic\u00f4ne syst\u00e8me R\u00e9seau",
        L"\u00cdcone do sistema Rede",
    }},
    {L"pnidui.dll", 60, {
        L"Not connected - Wireless communication is turned off",
        L"Non connesso - Comunicazione wireless disattivata",
        L"No conectado: la comunicaci\u00f3n inal\u00e1mbrica est\u00e1 desactivada",
        L"Non connect\u00e9 : la communication sans fil est d\u00e9sactiv\u00e9e",
        L"N\u00e3o conectado: a comunica\u00e7\u00e3o sem fio est\u00e1 desativada",
    }},
    {L"pnidui.dll", 3011, {
        L"Network System Icon",
        L"Icona di sistema Rete",
        L"Icono del sistema Red",
        L"Ic\u00f4ne syst\u00e8me R\u00e9seau",
        L"\u00cdcone do sistema Rede",
    }},
};
constexpr size_t kHydratedStringCount = ARRAYSIZE(kHydratedStrings);

// ===========================================================================
// Global state
// ===========================================================================
enum class UiLangMode : int {
    Auto = 0,
    Italian = 1,
    English = 2,
    Spanish = 3,
    French = 4,
    Portuguese = 5,
};
enum class HostRole : int {
    Undetermined = 0,
    Host = 1,
    Secondary = 2,
};

std::atomic<bool> g_enableLegacyTrayIcon{false};
std::atomic<bool> g_interceptAvailableNetworksUri{true};
std::atomic<int> g_uiLangMode{static_cast<int>(UiLangMode::Auto)};
std::atomic<bool> g_hideDuplicateWlanAdapters{false};
std::atomic<bool> g_enableHotkey{false};


std::atomic<bool> g_shuttingDown{false};
std::atomic<bool> g_vanStoreReady{false};
std::atomic<bool> g_privateRegistryHiveReady{false};
HKEY g_privateRegistryHive = nullptr;  // RegLoadAppKeyW: isolated from HKCU/HKLM
std::filesystem::path g_privateRegistryHiveFile;  // unique to this mod instance
std::atomic<bool> g_nativeSystemVan{false};
std::atomic<bool> g_unloadSafetyFailure{false};
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
WIN8_NET_NO_DESTROY WCHAR g_managedFullPaths[kRuntimeDllCount][600] = {};

WIN8_NET_NO_DESTROY std::mutex g_stateMutex;
WIN8_NET_NO_DESTROY std::filesystem::path g_vanStoreBaseDir;   // ...\VanStore
WIN8_NET_NO_DESTROY std::filesystem::path g_rawCacheDir;       // ...\MsdlRawCache
WIN8_NET_NO_DESTROY std::filesystem::path g_vanStoreDir;       // hydrated generation or System32
WIN8_NET_NO_DESTROY std::filesystem::path g_nativeSystemDir;
WIN8_NET_NO_DESTROY std::vector<HMODULE> g_loadedVanModules;
// Published after LoadLibrary returns; hook-side caller checks never take a
// mutex or enter the loader to identify a module.
std::atomic<ULONG_PTR> g_managedImageBegin[kRuntimeDllCount]{};
std::atomic<ULONG_PTR> g_managedImageEnd[kRuntimeDllCount]{};

WIN8_NET_NO_DESTROY std::vector<HANDLE> g_verifiedRuntimeFileHandles;
WIN8_NET_NO_DESTROY std::wstring g_hkcuNtPrefix;               // "\Registry\User\S-1-5-..."

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
constexpr UINT_PTR kStaTimerFlyoutDataRefresh = 3;    // 30s flyout data refresh (when closed)
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
using NtCreateKey_t = NTSTATUS(NTAPI*)(
    PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, ULONG, PUNICODE_STRING, ULONG, PULONG);
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

// COM activation must not borrow the registry/loader recursion guard: a
// provider's factory/constructor can legitimately read its redirected registry
// and activate other managed classes while it initializes.
constexpr unsigned kMaxManagedComActivationDepth = 16;
thread_local const GUID* g_comActivationStack[kMaxManagedComActivationDepth]{};
thread_local unsigned g_comActivationDepth = 0;

bool CanEnterManagedComActivation(REFCLSID clsid) noexcept {
    if (g_comActivationDepth >= kMaxManagedComActivationDepth) return false;
    for (unsigned i = 0; i < g_comActivationDepth; ++i)
        if (IsEqualGUID(*g_comActivationStack[i], clsid)) return false;
    return true;
}

struct ScopedComActivationGuard {
    explicit ScopedComActivationGuard(REFCLSID clsid) noexcept {
        g_comActivationStack[g_comActivationDepth++] = &clsid;
    }
    ~ScopedComActivationGuard() noexcept {
        g_comActivationStack[--g_comActivationDepth] = nullptr;
    }
    ScopedComActivationGuard(const ScopedComActivationGuard&) = delete;
    ScopedComActivationGuard& operator=(const ScopedComActivationGuard&) = delete;
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

int GetEffectiveLanguageIndex() noexcept {
    switch (static_cast<UiLangMode>(g_uiLangMode.load(std::memory_order_relaxed))) {
        case UiLangMode::Italian:
            return 1;
        case UiLangMode::English:
            return 0;
        case UiLangMode::Spanish:
            return 2;
        case UiLangMode::French:
            return 3;
        case UiLangMode::Portuguese:
            return 4;
        case UiLangMode::Auto:
        default:
            switch (PRIMARYLANGID(GetUserDefaultUILanguage())) {
                case LANG_ITALIAN: return 1;
                case LANG_SPANISH: return 2;
                case LANG_FRENCH: return 3;
                case LANG_PORTUGUESE: return 4;
                default: return 0;
            }
    }
}

const wchar_t* GetLanguageCode(int languageIndex) noexcept {
    static constexpr const wchar_t* kLanguageCodes[] = {
        L"en", L"it", L"es", L"fr", L"pt",
    };
    if (languageIndex < 0 || languageIndex >= static_cast<int>(ARRAYSIZE(kLanguageCodes))) {
        return L"en";
    }
    return kLanguageCodes[languageIndex];
}

int GetLanguageIndexFromCode(const wchar_t* code) noexcept {
    if (!code) return -1;
    if (_wcsicmp(code, L"en") == 0) return 0;
    if (_wcsicmp(code, L"it") == 0) return 1;
    if (_wcsicmp(code, L"es") == 0) return 2;
    if (_wcsicmp(code, L"fr") == 0) return 3;
    if (_wcsicmp(code, L"pt") == 0) return 4;
    return -1;
}

const wchar_t* GetHydratedStringFor(
    const wchar_t* moduleLower, UINT id, int languageIndex) noexcept {
    if (languageIndex < 0 || languageIndex >= 5) languageIndex = 0;
    for (size_t i = 0; i < kHydratedStringCount; ++i) {
        const LocalizedResourceEntry& e = kHydratedStrings[i];
        if (e.id == id && _wcsicmp(e.moduleLower, moduleLower) == 0) {
            return e.text[static_cast<size_t>(languageIndex)];
        }
    }
    return nullptr;
}

void LoadModSettings() {
    g_enableLegacyTrayIcon.store(
        Wh_GetIntSetting(L"enableLegacyTrayIcon") != 0, std::memory_order_relaxed);
    g_interceptAvailableNetworksUri.store(
        Wh_GetIntSetting(L"interceptAvailableNetworksUri") != 0, std::memory_order_relaxed);

    // uiLanguage is a string option in the Windhawk settings block; reading it
    // as an integer silently leaves the selection at Auto for every value.
    PCWSTR uiLanguage = Wh_GetStringSetting(L"uiLanguage");
    UiLangMode languageMode = UiLangMode::Auto;
    if (_wcsicmp(uiLanguage, L"it") == 0) {
        languageMode = UiLangMode::Italian;
    } else if (_wcsicmp(uiLanguage, L"en") == 0) {
        languageMode = UiLangMode::English;
    } else if (_wcsicmp(uiLanguage, L"es") == 0) {
        languageMode = UiLangMode::Spanish;
    } else if (_wcsicmp(uiLanguage, L"fr") == 0) {
        languageMode = UiLangMode::French;
    } else if (_wcsicmp(uiLanguage, L"pt") == 0) {
        languageMode = UiLangMode::Portuguese;
    }
    Wh_FreeStringSetting(uiLanguage);
    g_uiLangMode.store(static_cast<int>(languageMode), std::memory_order_relaxed);

    g_hideDuplicateWlanAdapters.store(
        Wh_GetIntSetting(L"hideDuplicateWlanAdapters") != 0, std::memory_order_relaxed);
    g_enableHotkey.store(
        Wh_GetIntSetting(L"enableHotkey") != 0, std::memory_order_relaxed);
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

// Checked copies instead of compiler-specific SEH. ReadProcessMemory reports
// unreadable ranges as an API failure; never dereference the source PE directly.
bool TryReadManagedImageSize(HMODULE module, DWORD* imageSize) noexcept {
    if (!module || !imageSize) return false;
    *imageSize = 0;
    IMAGE_DOS_HEADER dos{};
    IMAGE_NT_HEADERS64 nt{};
    SIZE_T copied = 0;
    if (!ReadProcessMemory(GetCurrentProcess(), module, &dos, sizeof(dos), &copied) ||
        copied != sizeof(dos)) {
        Wh_Log(L"PE DOS header read failed (%lu)", GetLastError());
        return false;
    }
    if (dos.e_magic != IMAGE_DOS_SIGNATURE || dos.e_lfanew <= 0 ||
        dos.e_lfanew > 1024 * 1024) return false;
    const ULONG_PTR base = reinterpret_cast<ULONG_PTR>(module);
    const ULONG_PTR offset = static_cast<ULONG_PTR>(dos.e_lfanew);
    if (base > static_cast<ULONG_PTR>(-1) - offset) return false;
    copied = 0;
    if (!ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<const void*>(base + offset),
                           &nt, sizeof(nt), &copied) || copied != sizeof(nt)) {
        Wh_Log(L"PE NT header read failed (%lu)", GetLastError());
        return false;
    }
    if (nt.Signature != IMAGE_NT_SIGNATURE ||
        nt.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC ||
        !nt.OptionalHeader.SizeOfImage ||
        base > static_cast<ULONG_PTR>(-1) - nt.OptionalHeader.SizeOfImage) return false;
    *imageSize = nt.OptionalHeader.SizeOfImage;
    return true;
}

// Phase diagnostics only, NOT exception handlers. An unmatched enter/leave
// pair helps locate synchronous faults; Windows retains normal crash reporting.
HRESULT ObserveShowVanCall(ShowVAN_t function) {
    if (!function) return E_POINTER;
    Wh_Log(L"Legacy call enter: ShowVAN (thread %lu)", GetCurrentThreadId());
    const HRESULT result = function(false, false, nullptr);
    Wh_Log(L"Legacy call leave: ShowVAN hr=0x%08lX", static_cast<unsigned long>(result));
    return result;
}

void ObserveVanVoidCall(HideVAN_t function, const wchar_t* operation) {
    if (!function) return;
    Wh_Log(L"Legacy call enter: %s (thread %lu)", operation, GetCurrentThreadId());
    function();
    Wh_Log(L"Legacy call leave: %s", operation);
}

HRESULT ObserveTrayActivate(INetworkTrayPrivate* tray, BYTE* context) {
    if (!tray || !context) return E_POINTER;
    Wh_Log(L"Legacy call enter: pnidui Activate (thread %lu)", GetCurrentThreadId());
    const HRESULT result = tray->Activate(4, context + 0x18, context + 0x10);
    Wh_Log(L"Legacy call leave: pnidui Activate hr=0x%08lX", static_cast<unsigned long>(result));
    return result;
}

void PublishManagedImage(HMODULE module, const wchar_t* canonicalName) noexcept {
    if (!module || !canonicalName) return;
    DWORD imageSize = 0;
    if (!TryReadManagedImageSize(module, &imageSize)) {
        Wh_Log(L"Cannot publish managed image %s; PE probe failed", canonicalName);
        return;
    }
    for (size_t i = 0; i < kRuntimeDllCount; ++i) {
        if (_wcsicmp(canonicalName, kRuntimeDllNames[i]) == 0) {
            const ULONG_PTR begin = reinterpret_cast<ULONG_PTR>(module);
            g_managedImageEnd[i].store(begin + imageSize,
                                      std::memory_order_relaxed);
            g_managedImageBegin[i].store(begin, std::memory_order_release);
            break;
        }
    }
}

bool IsCallerManagedVanModule(void* address) noexcept {
    const ULONG_PTR caller = reinterpret_cast<ULONG_PTR>(address);
    for (size_t i = 0; i < kRuntimeDllCount; ++i) {
        const ULONG_PTR begin = g_managedImageBegin[i].load(std::memory_order_acquire);
        if (begin && caller >= begin &&
            caller < g_managedImageEnd[i].load(std::memory_order_relaxed)) return true;
    }
    return false;
}

bool IsManagedVanCallContext(void* caller) noexcept {
    const DWORD sta = g_staThreadId.load(std::memory_order_acquire);
    return (sta != 0 && GetCurrentThreadId() == sta) || IsCallerManagedVanModule(caller);
}

// Own exactly one LoadLibrary/GetModuleHandleEx reference, never a borrowed handle.
struct ScopedRuntimeModuleReference {
    HMODULE value;
    explicit ScopedRuntimeModuleReference(HMODULE module) noexcept : value(module) {}
    ScopedRuntimeModuleReference(const ScopedRuntimeModuleReference&) = delete;
    ScopedRuntimeModuleReference& operator=(const ScopedRuntimeModuleReference&) = delete;
    ~ScopedRuntimeModuleReference() noexcept { if (value) FreeLibrary(value); }
    HMODULE release() noexcept { HMODULE result = value; value = nullptr; return result; }
};

bool AdoptManagedRuntimeReference(HMODULE owned, const wchar_t* canonicalName) {
    ScopedRuntimeModuleReference reference(owned);
    bool added = false;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (std::find(g_loadedVanModules.begin(), g_loadedVanModules.end(), owned) ==
                g_loadedVanModules.end()) {
            // If allocation throws, the local reference is released and no
            // caller range is published for a potentially unloaded image.
            g_loadedVanModules.push_back(owned);
            reference.release();
            added = true;
        }
    }
    PublishManagedImage(owned, canonicalName);
    return added;
}

void RefreshManagedDependencyImages() {
    // Run only after LoadLibrary returns, never from the LdrLoadDll hook.
    // Imported DLLs do not necessarily pass through our LoadLibrary hooks.
    // GetModuleHandleEx takes a temporary reference; transfer it to our normal
    // runtime ownership list or release it before returning. No mod-image pin.
    for (size_t i = 0; i < kRuntimeDllCount; ++i) {
        const wchar_t* expected = g_managedFullPaths[i];
        if (!expected[0]) continue;
        HMODULE module = nullptr;
        if (!GetModuleHandleExW(0, expected, &module)) continue;
        struct ModuleReference {
            HMODULE value;
            ~ModuleReference() noexcept { if (value) FreeLibrary(value); }
        } reference{module};
        wchar_t actual[1024]{};
        const DWORD length = GetModuleFileNameW(module, actual, ARRAYSIZE(actual));
        if (!length || length >= ARRAYSIZE(actual) ||
            CompareStringOrdinal(actual, -1, expected, -1, TRUE) != CSTR_EQUAL) {
            Wh_Log(L"Managed dependency rejected: expected=%s, actual=%s", expected,
                   length && length < ARRAYSIZE(actual) ? actual : L"<unavailable>");
            continue;
        }
        bool added = false;
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            if (std::find(g_loadedVanModules.begin(), g_loadedVanModules.end(), module) ==
                    g_loadedVanModules.end()) {
                g_loadedVanModules.push_back(module);
                reference.value = nullptr; // balanced by normal runtime DLL cleanup
                added = true;
            }
        }
        PublishManagedImage(module, kRuntimeDllNames[i]);
        if (added) Wh_Log(L"Managed imported dependency recognized: %s", actual);
    }
}

HMODULE LoadManagedDll(const wchar_t* canonicalName) {
    const std::filesystem::path fullPath = GetManagedDllPath(canonicalName);
    ScopedHookGuard guard;
    HMODULE hMod = LoadLibraryExW(
        fullPath.c_str(), nullptr,
        LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hMod) {
        // Internal users borrow the reference kept in our runtime list.
        // Repeated loads release their redundant reference instead of leaking it.
        AdoptManagedRuntimeReference(hMod, canonicalName);
        RefreshManagedDependencyImages();
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

// ReplaceVan=2 can use the operating system's own VAN stack without MSDL
// downloads on builds that still ship a loadable System32 van.dll. The build
// cutoff is only one conservative guard: every expected runtime component and
// the required exports are checked as well. This avoids treating "pre-23H2"
// as a sufficient compatibility test (25346 is the documented removal build).
bool DetectNativeVanSupport(std::filesystem::path* outSystemDirectory, DWORD* outBuild) {
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOW*);
    auto pRtlGetVersion = hNtdll ? reinterpret_cast<RtlGetVersion_t>(
        GetProcAddress(hNtdll, "RtlGetVersion")) : nullptr;
    OSVERSIONINFOW version{};
    version.dwOSVersionInfoSize = sizeof(version);
    if (!pRtlGetVersion || pRtlGetVersion(&version) != 0 || version.dwMajorVersion != 10) {
        Wh_Log(L"Native VAN route unavailable: could not identify a supported Windows 10/11 build");
        return false;
    }
    if (outBuild) {
        *outBuild = version.dwBuildNumber;
    }
    if (version.dwBuildNumber < 17763 || version.dwBuildNumber >= 25346) {
        Wh_Log(L"Native VAN route unavailable on Windows build %lu (supported probe range: "
               L"17763..25345)", static_cast<unsigned long>(version.dwBuildNumber));
        return false;
    }

    WCHAR systemDirectory[MAX_PATH]{};
    const UINT systemDirectoryLength = GetSystemDirectoryW(systemDirectory, ARRAYSIZE(systemDirectory));
    if (systemDirectoryLength == 0 || systemDirectoryLength >= ARRAYSIZE(systemDirectory)) {
        Wh_Log(L"Native VAN route unavailable: GetSystemDirectoryW failed (%lu)", GetLastError());
        return false;
    }
    const std::filesystem::path systemDir(systemDirectory);

    for (const wchar_t* dll : kRuntimeDllNames) {
        const std::filesystem::path componentPath = systemDir / dll;
        const DWORD attributes = GetFileAttributesW(componentPath.c_str());
        if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY)) {
            Wh_Log(L"Native VAN route unavailable: System32 component %s is missing", dll);
            return false;
        }
    }

    HMODULE hVan = LoadLibraryExW(L"van.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hVan) {
        Wh_Log(L"Native VAN route unavailable: System32 van.dll failed to load (%lu)", GetLastError());
        return false;
    }
    const bool exportsAvailable = GetProcAddress(hVan, "ShowVAN") != nullptr &&
                                  GetProcAddress(hVan, "HideVAN") != nullptr;
    FreeLibrary(hVan);
    if (!exportsAvailable) {
        Wh_Log(L"Native VAN route unavailable: van.dll lacks the expected ShowVAN/HideVAN exports");
        return false;
    }

    if (outSystemDirectory) {
        *outSystemDirectory = systemDir;
    }
    Wh_Log(L"Native ReplaceVan=2 path is available on build %lu (%s)",
           static_cast<unsigned long>(version.dwBuildNumber), systemDir.c_str());
    return true;
}

// ===========================================================================
// Pinned artifact validation
// ---------------------------------------------------------------------------
// Every downloaded MSDL image must be a structurally valid x64 PE and match
// its compiled-in SHA-256 pin. Verification opens the file with sharing that
// permits readers only; holding that handle prevents writes, replacement and
// deletion between validation and copying/loading.
// WinVerifyTrust is diagnostic only: MSDL binaries are catalog-signed and do
// not carry an embedded Authenticode signature.
// ===========================================================================
struct ScopedFileHandle {
    HANDLE value = INVALID_HANDLE_VALUE;

    ScopedFileHandle() = default;
    explicit ScopedFileHandle(HANDLE h) : value(h) {}
    ~ScopedFileHandle() { reset(); }
    ScopedFileHandle(const ScopedFileHandle&) = delete;
    ScopedFileHandle& operator=(const ScopedFileHandle&) = delete;
    ScopedFileHandle(ScopedFileHandle&& other) noexcept : value(other.release()) {}
    ScopedFileHandle& operator=(ScopedFileHandle&& other) noexcept {
        if (this != &other) {
            reset(other.release());
        }
        return *this;
    }

    bool valid() const noexcept { return value != nullptr && value != INVALID_HANDLE_VALUE; }
    HANDLE get() const noexcept { return value; }
    HANDLE release() noexcept {
        HANDLE h = value;
        value = INVALID_HANDLE_VALUE;
        return h;
    }
    void reset(HANDLE h = INVALID_HANDLE_VALUE) noexcept {
        if (valid()) {
            CloseHandle(value);
        }
        value = h;
    }
};

bool IsPe64Handle(HANDLE hFile) {
    if (hFile == nullptr || hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    LARGE_INTEGER fileSize{};
    if (!GetFileSizeEx(hFile, &fileSize) || fileSize.QuadPart < 4096) {
        return false;
    }

    LARGE_INTEGER zero{};
    if (!SetFilePointerEx(hFile, zero, nullptr, FILE_BEGIN)) {
        return false;
    }

    // Stack buffer only - this must stay cheap and loader-lock friendly.
    BYTE head[1024]{};
    DWORD bytesRead = 0;
    if (!ReadFile(hFile, head, sizeof(head), &bytesRead, nullptr) || bytesRead < 0x200) {
        return false;
    }

    const WORD mz = *reinterpret_cast<const WORD*>(head);
    if (mz != 0x5A4D) {  // 'MZ'
        return false;
    }
    const DWORD peOffset = *reinterpret_cast<const DWORD*>(head + 0x3C);
    constexpr size_t kMinimumPeHeader = 4 + sizeof(IMAGE_FILE_HEADER) + sizeof(WORD);
    if (peOffset > bytesRead || bytesRead - peOffset < kMinimumPeHeader) {
        return false;
    }

    const BYTE* pe = head + peOffset;
    if (memcmp(pe, "PE\0\0", 4) != 0) {
        return false;
    }
    const auto* fileHeader = reinterpret_cast<const IMAGE_FILE_HEADER*>(pe + 4);
    if (fileHeader->Machine != IMAGE_FILE_MACHINE_AMD64 ||
        fileHeader->SizeOfOptionalHeader < sizeof(IMAGE_OPTIONAL_HEADER64)) {
        return false;
    }
    const WORD optionalMagic = *reinterpret_cast<const WORD*>(
        pe + 4 + sizeof(IMAGE_FILE_HEADER));
    return optionalMagic == IMAGE_NT_OPTIONAL_HDR64_MAGIC;
}

bool IsPe64File(const std::filesystem::path& path) {
    ScopedFileHandle file(CreateFileW(
        path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr));
    return file.valid() && IsPe64Handle(file.get());
}

// CNG first; CryptoAPI is a compatibility fallback, never a verification bypass.
// Both backends hash the entire file from offset zero and reject read failures.
bool ComputeFileSha256Hex(HANDLE hFile, std::wstring& outUpperHex) noexcept {
    outUpperHex.clear();
    if (!hFile || hFile == INVALID_HANDLE_VALUE) return false;
    struct HashResources {
        BCRYPT_ALG_HANDLE algorithm = nullptr;
        BCRYPT_HASH_HANDLE hash = nullptr;
        HCRYPTPROV provider = 0;
        HCRYPTHASH legacyHash = 0;
        ~HashResources() {
            if (hash) BCryptDestroyHash(hash);
            if (algorithm) BCryptCloseAlgorithmProvider(algorithm, 0);
            if (legacyHash) CryptDestroyHash(legacyHash);
            if (provider) CryptReleaseContext(provider, 0);
        }
    } resources;
    LARGE_INTEGER zero{};
    try {
        std::array<BYTE, 32> digest{};
        std::array<BYTE, 64 * 1024> buffer{};
        bool complete = false;
        for (int backend = 0; backend < 2 && !complete; ++backend) {
            if (!SetFilePointerEx(hFile, zero, nullptr, FILE_BEGIN)) return false;
            if (backend == 0) {
                if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(
                        &resources.algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0)) ||
                    !BCRYPT_SUCCESS(BCryptCreateHash(resources.algorithm,
                        &resources.hash, nullptr, 0, nullptr, 0, 0))) {
                    Wh_Log(L"SHA-256 CNG initialization failed; trying CryptoAPI");
                    continue;
                }
            } else {
                Wh_Log(L"Retrying SHA-256 with the Windows CryptoAPI backend");
                if (!CryptAcquireContextW(&resources.provider, nullptr,
                        MS_ENH_RSA_AES_PROV_W, PROV_RSA_AES, CRYPT_VERIFYCONTEXT) ||
                    !CryptCreateHash(resources.provider, CALG_SHA_256, 0, 0,
                        &resources.legacyHash)) return false;
            }
            bool hashOk = true;
            for (;;) {
                DWORD bytesRead = 0;
                if (!ReadFile(hFile, buffer.data(), static_cast<DWORD>(buffer.size()),
                              &bytesRead, nullptr)) {
                    Wh_Log(L"SHA-256 file read failed (%lu)", GetLastError());
                    SetFilePointerEx(hFile, zero, nullptr, FILE_BEGIN);
                    return false;
                }
                if (bytesRead == 0) break;
                hashOk = backend == 0
                    ? BCRYPT_SUCCESS(BCryptHashData(resources.hash, buffer.data(), bytesRead, 0))
                    : !!CryptHashData(resources.legacyHash, buffer.data(), bytesRead, 0);
                if (!hashOk) break;
            }
            if (!hashOk) continue;
            DWORD digestSize = static_cast<DWORD>(digest.size());
            complete = backend == 0
                ? BCRYPT_SUCCESS(BCryptFinishHash(resources.hash, digest.data(), digestSize, 0))
                : (!!CryptGetHashParam(resources.legacyHash, HP_HASHVAL,
                       digest.data(), &digestSize, 0) && digestSize == digest.size());
        }
        if (!complete || !SetFilePointerEx(hFile, zero, nullptr, FILE_BEGIN)) return false;
        static constexpr wchar_t hex[] = L"0123456789ABCDEF";
        std::wstring result(64, L'0');
        for (size_t i = 0; i < digest.size(); ++i) {
            result[2 * i] = hex[digest[i] >> 4];
            result[2 * i + 1] = hex[digest[i] & 15];
        }
        outUpperHex.swap(result);
        return true;
    } catch (...) {
        // C++ allocation/library exceptions only: no SEH or access-violation recovery.
        Wh_Log(L"C++ exception while computing SHA-256; file rejected");
        SetFilePointerEx(hFile, zero, nullptr, FILE_BEGIN);
        outUpperHex.clear();
        return false;
    }
}

bool OpenVerifiedFile(
    const std::filesystem::path& path, const wchar_t* expectedSha256,
    ScopedFileHandle* keepOpen, std::wstring* actualHash = nullptr) try {
    ScopedFileHandle file(CreateFileW(
        path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr));
    if (!file.valid()) {
        Wh_Log(L"Could not open %s for pinned read-only verification (%lu)",
               path.filename().c_str(), GetLastError());
        return false;
    }

    if (!IsPe64Handle(file.get())) {
        Wh_Log(L"PE64 structural validation rejected %s", path.filename().c_str());
        return false;
    }

    std::wstring computedHash;
    if (!ComputeFileSha256Hex(file.get(), computedHash)) {
        Wh_Log(L"Could not compute mandatory SHA-256 of %s (%lu)",
               path.filename().c_str(), GetLastError());
        return false;
    }

    if (expectedSha256 && _wcsicmp(computedHash.c_str(), expectedSha256) != 0) {
        Wh_Log(L"SHA-256 pin mismatch on %s (got %s, expected %s)",
               path.filename().c_str(), computedHash.c_str(), expectedSha256);
        return false;
    }

    if (actualHash) {
        *actualHash = std::move(computedHash);
    }
    if (keepOpen) {
        *keepOpen = std::move(file);
    }
    return true;
} catch (const std::bad_alloc&) {
    Wh_Log(L"C++ allocation failure during file verification; file rejected");
    return false;
} catch (const std::filesystem::filesystem_error& error) {
    Wh_Log(L"C++ filesystem exception during file verification (code %d); file rejected",
           error.code().value());
    return false;
} catch (...) {
    Wh_Log(L"Unexpected C++ exception during file verification; file rejected");
    return false;
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
    if (!expectedSha256 || wcslen(expectedSha256) != 64) return {false, false};
    for (const wchar_t* c = expectedSha256; *c; ++c) {
        if (!((*c >= L'0' && *c <= L'9') || (*c >= L'a' && *c <= L'f') ||
              (*c >= L'A' && *c <= L'F'))) return {false, false};
    }
    ScopedFileHandle file;
    const bool ok = OpenVerifiedFile(path, expectedSha256, &file);
    return {ok, ok};  // A successful result always means the exact SHA-256 pin matched.
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
    // WinHttpOpenRequest needs the full server-relative object path, not just
    // the artifact tail. Keep this identical to the URLMon fallback endpoint.
    const std::wstring requestPath =
        std::wstring(L"/download/symbols/") + relativeTail;
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
        hConnect, L"GET", requestPath.c_str(), nullptr, WINHTTP_NO_REFERER,
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

class ScopedDataModule {
  public:
    explicit ScopedDataModule(HMODULE module) noexcept : module_(module) {}
    ~ScopedDataModule() noexcept { if (module_) FreeLibrary(module_); }
    ScopedDataModule(const ScopedDataModule&) = delete;
    ScopedDataModule& operator=(const ScopedDataModule&) = delete;
  private:
    HMODULE module_;
};

class ScopedResourceUpdate {
  public:
    explicit ScopedResourceUpdate(HANDLE update) noexcept : update_(update) {}
    ~ScopedResourceUpdate() noexcept { if (update_) EndUpdateResourceW(update_, TRUE); }
    BOOL commit() noexcept {
        HANDLE update = update_;
        update_ = nullptr;
        return EndUpdateResourceW(update, FALSE);
    }
    ScopedResourceUpdate(const ScopedResourceUpdate&) = delete;
    ScopedResourceUpdate& operator=(const ScopedResourceUpdate&) = delete;
  private:
    HANDLE update_;
};

thread_local bool g_muiCollectionFailed = false;

struct MuiDeleteEntry {
    std::wstring nameStr;
    ULONG_PTR nameInt = 0;
    bool isIntName = false;
    WORD langId = 0;
};

BOOL CALLBACK EnumMuiLangCollectProc(
    HMODULE, LPCWSTR, LPCWSTR lpName, WORD wIDLanguage, LONG_PTR lParam) try {
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
    return g_muiCollectionFailed ? FALSE : TRUE;
} catch (...) {
    g_muiCollectionFailed = true;
    return FALSE;
}

BOOL CALLBACK EnumMuiNameCollectProc(
    HMODULE hModule, LPCWSTR lpType, LPWSTR lpName, LONG_PTR lParam) {
    EnumResourceLanguagesW(hModule, lpType, lpName, EnumMuiLangCollectProc, lParam);
    return g_muiCollectionFailed ? FALSE : TRUE;
}

bool HydrateRuntimePe(
    const std::filesystem::path& targetPePath,
    const wchar_t* canonicalModuleLower,
    int languageIndex,
    const std::filesystem::path* optionalDonorPe) try {
    // Collect existing "MUI" manifest resources so hydration can remove the
    // MUI split - without this the loader keeps looking for <dll>.mui files
    // that do not exist next to the hydrated copy and every string misses.
    std::vector<MuiDeleteEntry> muiEntries;
    g_muiCollectionFailed = false;
    if (HMODULE hInspect = LoadLibraryExW(
            targetPePath.c_str(), nullptr,
            LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE)) {
        ScopedDataModule inspectOwner(hInspect);
        EnumResourceNamesW(
            hInspect, L"MUI", EnumMuiNameCollectProc, reinterpret_cast<LONG_PTR>(&muiEntries));
        if (g_muiCollectionFailed) {
            Wh_Log(L"C++ exception collecting MUI resources; generation rejected");
            return false;
        }
    }

    // Standard Win32 RT_STRING layout: bundle N holds string IDs
    // N*16 .. N*16+15 (bundle id = stringId / 16 + 1, slot = stringId % 16).
    const size_t safeLanguageIndex =
        (languageIndex >= 0 && languageIndex < 5) ? static_cast<size_t>(languageIndex) : 0;
    std::map<UINT, std::array<std::wstring, 16>> bundles;
    for (size_t i = 0; i < kHydratedStringCount; ++i) {
        const LocalizedResourceEntry& e = kHydratedStrings[i];
        if (_wcsicmp(e.moduleLower, canonicalModuleLower) == 0) {
            const UINT bundleId = (e.id / 16u) + 1u;
            const UINT slot = e.id % 16u;
            bundles[bundleId][slot] = e.text[safeLanguageIndex];
        }
    }

    HANDLE hUpdate = BeginUpdateResourceW(targetPePath.c_str(), FALSE);
    if (!hUpdate) {
        Wh_Log(L"BeginUpdateResourceW failed (%lu) on %s", GetLastError(),
               targetPePath.c_str());
        return false;
    }

    ScopedResourceUpdate updateOwner(hUpdate);

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

    if (!updateOwner.commit()) {
        Wh_Log(L"EndUpdateResourceW failed (%lu) on %s", GetLastError(), targetPePath.c_str());
        return false;
    }

    // Post-hydration sanity check: the file must still be a valid x64 PE.
    if (!IsPe64File(targetPePath)) {
        Wh_Log(L"Hydrated %s failed PE64 post-validation", targetPePath.filename().c_str());
        return false;
    }
    return true;
} catch (const std::bad_alloc&) {
    Wh_Log(L"C++ allocation failure while preparing runtime resources; image rejected");
    return false;
} catch (const std::filesystem::filesystem_error& error) {
    Wh_Log(L"C++ filesystem exception while preparing runtime resources (code %d); image rejected",
           error.code().value());
    return false;
} catch (...) {
    Wh_Log(L"Unexpected C++ exception while preparing runtime resources; image rejected");
    return false;
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

const MsdlArtifact* FindMsdlArtifact(const wchar_t* localRawName) {
    if (!localRawName) {
        return nullptr;
    }
    for (const auto& artifact : kMsdlArtifacts) {
        if (_wcsicmp(artifact.localRawName, localRawName) == 0) {
            return &artifact;
        }
    }
    return nullptr;
}

bool CopyVerifiedRawArtifact(const wchar_t* localRawName, const std::filesystem::path& destination) {
    const MsdlArtifact* artifact = FindMsdlArtifact(localRawName);
    if (!artifact) {
        Wh_Log(L"No compiled-in hash pin for raw source %s", localRawName ? localRawName : L"(null)");
        return false;
    }

    const std::filesystem::path source = g_rawCacheDir / artifact->localRawName;
    ScopedFileHandle verifiedSource;
    if (!OpenVerifiedFile(source, artifact->expectedSha256, &verifiedSource)) {
        Wh_Log(L"Refusing to copy unverified raw artifact %s", localRawName);
        return false;
    }
    if (!CopyFileW(source.c_str(), destination.c_str(), FALSE)) {
        Wh_Log(L"CopyFileW failed (%lu) for verified source %s", GetLastError(), localRawName);
        return false;
    }
    return true;
}

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

bool ReadHydrationManifest(
    const std::filesystem::path& dir,
    std::array<std::wstring, kRuntimeDllCount>& outHashes) {
    ScopedFileHandle file(CreateFileW(
        (dir / kHydrationManifestName).c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr));
    if (!file.valid()) {
        return false;
    }

    WCHAR buffer[2048]{};
    DWORD bytesRead = 0;
    if (!ReadFile(file.get(), buffer, sizeof(buffer) - sizeof(WCHAR), &bytesRead, nullptr) ||
        bytesRead == 0 || bytesRead % sizeof(WCHAR) != 0 ||
        bytesRead >= sizeof(buffer) - sizeof(WCHAR)) {
        return false;
    }
    buffer[bytesRead / sizeof(WCHAR)] = L'\0';

    std::array<bool, kRuntimeDllCount> seen{};
    WCHAR* context = nullptr;
    for (WCHAR* line = wcstok_s(buffer, L"\r\n", &context); line;
         line = wcstok_s(nullptr, L"\r\n", &context)) {
        WCHAR* separator = wcschr(line, L'=');
        if (!separator) {
            return false;
        }
        *separator = L'\0';
        const wchar_t* hash = separator + 1;
        if (wcslen(hash) != 64) {
            return false;
        }
        for (const wchar_t* pHash = hash; *pHash; ++pHash) {
            if (!iswxdigit(*pHash)) {
                return false;
            }
        }

        bool matched = false;
        for (size_t i = 0; i < kRuntimeDllCount; ++i) {
            if (_wcsicmp(line, kRuntimeDllNames[i]) == 0) {
                if (seen[i]) {
                    return false;
                }
                outHashes[i] = hash;
                seen[i] = true;
                matched = true;
                break;
            }
        }
        if (!matched) {
            return false;
        }
    }

    return std::all_of(seen.begin(), seen.end(), [](bool value) { return value; });
}

bool WriteHydrationManifest(const std::filesystem::path& dir) {
    std::wstring contents;
    for (const wchar_t* dll : kRuntimeDllNames) {
        ScopedFileHandle file;
        std::wstring hash;
        if (!OpenVerifiedFile(dir / dll, nullptr, &file, &hash) || hash.size() != 64) {
            Wh_Log(L"Could not hash hydrated runtime image %s", dll);
            return false;
        }
        contents += dll;
        contents += L'=';
        contents += hash;
        contents += L"\r\n";
    }

    const std::filesystem::path manifestPath = dir / kHydrationManifestName;
    const std::filesystem::path tempPath = dir / (std::wstring(kHydrationManifestName) +
        L".tmp." + std::to_wstring(GetCurrentProcessId()));
    ScopedFileHandle file(CreateFileW(
        tempPath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
        nullptr));
    if (!file.valid()) {
        Wh_Log(L"Could not create hydrated image manifest (%lu)", GetLastError());
        return false;
    }

    const size_t bytesToWrite = contents.size() * sizeof(wchar_t);
    DWORD bytesWritten = 0;
    const BOOL writeOk = bytesToWrite <= MAXDWORD &&
        WriteFile(file.get(), contents.data(), static_cast<DWORD>(bytesToWrite),
                  &bytesWritten, nullptr) &&
        bytesWritten == bytesToWrite && FlushFileBuffers(file.get());
    file.reset();
    if (!writeOk || !MoveFileExW(
            tempPath.c_str(), manifestPath.c_str(),
            MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        Wh_Log(L"Could not publish hydrated image manifest (%lu)", GetLastError());
        DeleteFileW(tempPath.c_str());
        return false;
    }
    return true;
}

bool VerifyHydratedGeneration(
    const std::filesystem::path& dir, std::vector<HANDLE>* outLockedHandles) {
    std::array<std::wstring, kRuntimeDllCount> expectedHashes{};
    if (!ReadHydrationManifest(dir, expectedHashes)) {
        Wh_Log(L"Hydrated generation has no valid SHA-256 manifest");
        return false;
    }

    std::vector<HANDLE> lockedHandles;
    lockedHandles.reserve(kRuntimeDllCount);
    for (size_t i = 0; i < kRuntimeDllCount; ++i) {
        ScopedFileHandle file;
        if (!OpenVerifiedFile(
                dir / kRuntimeDllNames[i], expectedHashes[i].c_str(), &file)) {
            for (HANDLE handle : lockedHandles) {
                CloseHandle(handle);
            }
            Wh_Log(L"Hydrated generation verification failed for %s", kRuntimeDllNames[i]);
            return false;
        }
        lockedHandles.push_back(file.release());
    }

    if (outLockedHandles) {
        *outLockedHandles = std::move(lockedHandles);
    } else {
        for (HANDLE handle : lockedHandles) {
            CloseHandle(handle);
        }
    }
    return true;
}

bool ReadHydrationStamp(int& outGen, int& outLanguageIndex) {
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
    outLanguageIndex = -1;
    bool haveGen = false;
    WCHAR* ctx = nullptr;
    for (WCHAR* line = wcstok_s(buf, L"\r\n", &ctx); line;
         line = wcstok_s(nullptr, L"\r\n", &ctx)) {
        if (StartsWithInsensitive(line, L"gen=") && line[4] != L'\0') {
            outGen = _wtoi(line + 4);
            haveGen = true;
        } else if (StartsWithInsensitive(line, L"lang=")) {
            outLanguageIndex = GetLanguageIndexFromCode(line + 5);
        }
    }
    return haveGen && outLanguageIndex >= 0 && outGen >= 1 && outGen <= 50;
}

bool WriteHydrationStamp(int gen, int languageIndex) {
    const std::filesystem::path stampPath = g_vanStoreBaseDir / kHydrationStampName;
    WCHAR buf[64]{};
    int n = swprintf(buf, ARRAYSIZE(buf), L"gen=%d\r\nlang=%s\r\n", gen,
                     GetLanguageCode(languageIndex));
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
    bool verifiedDownload = false;
    for (int attempt = 1; attempt <= 2; ++attempt) {
        if (g_shuttingDown.load(std::memory_order_acquire)) break;
        DeleteFileW(tmpPath.c_str());
        Wh_Log(L"Downloading %s from MSDL (attempt %d/2)...", art.msdlRelativeUrl, attempt);
        if (DownloadMsdlFile(art.msdlRelativeUrl, tmpPath) &&
            CheckArtifactFile(tmpPath, art.expectedSha256).ok) {
            verifiedDownload = true;
            break;
        }
        Wh_Log(L"Download or SHA-256 verification failed for %s (attempt %d/2)",
               art.localRawName, attempt);
        DeleteFileW(tmpPath.c_str());
    }
    if (!verifiedDownload) {
        DeleteFileW(tmpPath.c_str());
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
    return CheckArtifactFile(rawPath, art.expectedSha256).ok;
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

    std::vector<HANDLE> verifiedHandles;
    if (!VerifyHydratedGeneration(dir, &verifiedHandles)) {
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        for (HANDLE handle : g_verifiedRuntimeFileHandles) {
            if (handle && handle != INVALID_HANDLE_VALUE) {
                CloseHandle(handle);
            }
        }
        g_verifiedRuntimeFileHandles.clear();
        g_verifiedRuntimeFileHandles.swap(verifiedHandles);
        g_vanStoreDir = dir;
    }
    return true;
}

bool EnsureVanStoreProvisioned() {
    const int languageIndex = GetEffectiveLanguageIndex();

    // Fast path (no mutex): valid stamp + complete generation folder.
    {
        int gen = 0;
        int stampLanguageIndex = -1;
        if (ReadHydrationStamp(gen, stampLanguageIndex) &&
            stampLanguageIndex == languageIndex && TryUseGeneration(gen)) {
            Wh_Log(L"Hydrated VanStore cache already valid (generation %d, lang=%s)", gen,
                   GetLanguageCode(stampLanguageIndex));
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
        int stampLanguageIndex = -1;
        if (ReadHydrationStamp(gen, stampLanguageIndex) &&
            stampLanguageIndex == languageIndex && TryUseGeneration(gen)) {
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
            const std::filesystem::path dstTmp = targetDir /
                (std::wstring(spec.runtimeDest) + L".building." + pidTidSuffix);
            const std::filesystem::path dstFinal = targetDir / spec.runtimeDest;

            if (!CopyVerifiedRawArtifact(spec.rawSource, dstTmp)) {
                Wh_Log(L"Refusing to hydrate unverified source %s for %s",
                       spec.rawSource, spec.runtimeDest);
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
            ScopedFileHandle donorVerification;
            bool donorVerified = true;
            if (donorPtr) {
                const MsdlArtifact* donorArtifact = FindMsdlArtifact(spec.donorRaw);
                donorVerified = donorArtifact && OpenVerifiedFile(
                    *donorPtr, donorArtifact->expectedSha256, &donorVerification);
                if (!donorVerified) {
                    Wh_Log(L"Refusing unverified donor %s for %s", spec.donorRaw,
                           spec.runtimeDest);
                }
            }
            if (donorVerified &&
                HydrateRuntimePe(dstTmp, lowerName.c_str(), languageIndex, donorPtr)) {
                hydrated = true;
            } else {
                DeleteFileW(dstTmp.c_str());
                Wh_Log(L"Hydration failed for %s (base %s)", spec.runtimeDest,
                       spec.rawSource);
                if (spec.altRawSource && wcscmp(spec.altRawSource, spec.rawSource) != 0) {
                    // Alternate base whose own embedded resources make the
                    // donor transplant unnecessary (van.dll: 17763 base).
                    lastBaseName = spec.altRawSource;
                    if (CopyVerifiedRawArtifact(spec.altRawSource, dstTmp) &&
                        HydrateRuntimePe(dstTmp, lowerName.c_str(), languageIndex, nullptr)) {
                        hydrated = true;
                        Wh_Log(L"Hydration succeeded with alternate base %s for %s",
                               spec.altRawSource, spec.runtimeDest);
                    } else {
                        DeleteFileW(dstTmp.c_str());
                    }
                }
                if (!hydrated) {
                    // Verified raw fallback: a plain raw copy keeps COM,
                    // exports and (for 17763 bases) the embedded resources
                    // alive; string lookups then fall back to the
                    // LoadStringW hook safety net.
                    if (CopyVerifiedRawArtifact(lastBaseName, dstTmp)) {
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

        // 3. Record every hydrated image digest before publishing the generation.
        //    The raw MSDL pins above are the trust anchor; this manifest detects
        //    corruption/tampering of the generated cache before it is loaded.
        if (!WriteHydrationManifest(targetDir)) {
            Wh_Log(L"Failed writing hydrated image SHA-256 manifest");
            break;
        }
        if (!WriteHydrationStamp(newGen, languageIndex)) {
            Wh_Log(L"Failed writing hydration stamp");
            break;
        }
        if (!TryUseGeneration(newGen)) {
            break;
        }
        Wh_Log(L"VanStore provisioning and PE hydration completed (generation %d, lang=%s)",
               newGen, GetLanguageCode(languageIndex));
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
// Private application registry hive (isolated from the live registry)
// -------------------------------------------------------------------------
// In fallback mode, definitions are written only to a private application hive
// loaded with RegLoadAppKeyW from Windhawk mod storage. Win32 and native key
// opens/creates for selected legacy HKLM/HKCR paths are redirected to that hive;
// write-capable opens fail closed if it is unavailable. It is never mounted under
// HKCU/HKLM, and this code does not clean or otherwise write any old registry
// entries left by previous versions. Managed classes are also activated by the
// in-proc CoGetClassObject/CoCreateInstance hooks.
// ===========================================================================
// Small noncopyable owners. Destructors only release the resource they own;
// UI/legacy teardown remains explicit on its owning STA, never on an arbitrary thread.
class ScopedRegistryKey {
  public:
    explicit ScopedRegistryKey(HKEY key) noexcept : key_(key) {}
    ~ScopedRegistryKey() noexcept { if (key_) RegCloseKey(key_); }
    ScopedRegistryKey(const ScopedRegistryKey&) = delete;
    ScopedRegistryKey& operator=(const ScopedRegistryKey&) = delete;
  private:
    HKEY key_;
};

void DisableModAfterCppException(const wchar_t* phase) noexcept {
    g_vanStoreReady.store(false, std::memory_order_release);
    // Keep private-hive redirection alive until owning-thread cleanup finishes:
    // legacy teardown must not fall through to real system/COM registry keys.
    g_shuttingDown.store(true, std::memory_order_release);
    if (g_stopEvent) SetEvent(g_stopEvent);
    Wh_Log(L"C++ exception in %s: mod disabled for this session; STA cleanup requested", phase);
}

class ScopedStaComApartment {
  public:
    ScopedStaComApartment() noexcept
        : result_(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)) {}
    ~ScopedStaComApartment() noexcept { if (SUCCEEDED(result_)) CoUninitialize(); }
    HRESULT result() const noexcept { return result_; }
    ScopedStaComApartment(const ScopedStaComApartment&) = delete;
    ScopedStaComApartment& operator=(const ScopedStaComApartment&) = delete;
  private:
    HRESULT result_;
};

template <typename Interface>
class ScopedComReference {
  public:
    explicit ScopedComReference(Interface* value) noexcept : value_(value) {}
    ~ScopedComReference() noexcept {
        if (!value_) return;
        try { value_->Release(); }
        catch (...) {
            // The COM ABI promises HRESULTs, not C++ throws. Retain resources
            // if a foreign implementation violates that contract during release.
            g_unloadSafetyFailure.store(true, std::memory_order_release);
            DisableModAfterCppException(L"COM temporary Release");
        }
    }
    ScopedComReference(const ScopedComReference&) = delete;
    ScopedComReference& operator=(const ScopedComReference&) = delete;
  private:
    Interface* value_;
};

// Private-hive write failures are counted so PopulatePrivateRegistryHive can
// refuse to report success for a partially written tree. Only the first few
// failures are logged (compact, non-verbose).
std::atomic<unsigned> g_privateHiveWriteFailures{0};
std::atomic<unsigned> g_privateHiveWriteFailureLogs{0};

void NotePrivateHiveWriteFailure(const std::wstring& subKey, LSTATUS status) noexcept {
    g_privateHiveWriteFailures.fetch_add(1, std::memory_order_relaxed);
    if (g_privateHiveWriteFailureLogs.fetch_add(1, std::memory_order_relaxed) < 6) {
        Wh_Log(L"[registry] private-hive write failed: key=%.80s status=%ld",
               subKey.c_str(), static_cast<long>(status));
    }
}

bool RegWriteSz(HKEY hRoot, const std::wstring& subKey, const wchar_t* valueName,
                const std::wstring& value, DWORD type = REG_SZ) {
    HKEY hKey = nullptr;
    LSTATUS status = RegCreateKeyExW(hRoot, subKey.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE,
                                     KEY_SET_VALUE, nullptr, &hKey, nullptr);
    if (status != ERROR_SUCCESS) {
        NotePrivateHiveWriteFailure(subKey, status);
        return false;
    }
    ScopedRegistryKey owner(hKey);
    status = RegSetValueExW(
        hKey, valueName, 0, type, reinterpret_cast<const BYTE*>(value.c_str()),
        static_cast<DWORD>((value.size() + 1) * sizeof(wchar_t)));
    if (status != ERROR_SUCCESS) {
        NotePrivateHiveWriteFailure(subKey, status);
        return false;
    }
    return true;
}

bool RegWriteDword(HKEY hRoot, const std::wstring& subKey, const wchar_t* valueName,
                   DWORD value) {
    HKEY hKey = nullptr;
    LSTATUS status = RegCreateKeyExW(hRoot, subKey.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE,
                                     KEY_SET_VALUE, nullptr, &hKey, nullptr);
    if (status != ERROR_SUCCESS) {
        NotePrivateHiveWriteFailure(subKey, status);
        return false;
    }
    ScopedRegistryKey owner(hKey);
    status = RegSetValueExW(hKey, valueName, 0, REG_DWORD,
                            reinterpret_cast<const BYTE*>(&value), sizeof(value));
    if (status != ERROR_SUCCESS) {
        NotePrivateHiveWriteFailure(subKey, status);
        return false;
    }
    return true;
}

// Recursively copies values and subkeys from hSrc (opened with KEY_READ) into
// hDst without copying security descriptors. New subkeys are created with NULL
// security attributes, so they inherit the ACL of their direct parent
// (RegCreateKeyEx documentation). Returns the first error encountered but keeps
// copying the remaining entries.
LSTATUS CopyRegistryTreeWithoutSecurity(HKEY hSrc, HKEY hDst, int depth) {
    if (depth > 16) {
        return ERROR_BAD_PATHNAME;
    }
    LSTATUS result = ERROR_SUCCESS;

    DWORD subKeyCount = 0, maxSubKeyLen = 0, valueCount = 0, maxValueNameLen = 0,
          maxValueDataLen = 0;
    LSTATUS status = RegQueryInfoKeyW(
        hSrc, nullptr, nullptr, nullptr, &subKeyCount, &maxSubKeyLen, nullptr, &valueCount,
        &maxValueNameLen, &maxValueDataLen, nullptr, nullptr);
    if (status != ERROR_SUCCESS) {
        return status;
    }

    std::vector<wchar_t> nameBuf(std::max(maxValueNameLen, maxSubKeyLen) + 2);
    std::vector<BYTE> dataBuf(maxValueDataLen + 2);

    for (DWORD i = 0; i < valueCount; ++i) {
        DWORD nameLen = static_cast<DWORD>(nameBuf.size());
        DWORD dataLen = static_cast<DWORD>(dataBuf.size());
        DWORD type = 0;
        status = RegEnumValueW(hSrc, i, nameBuf.data(), &nameLen, nullptr, &type,
                               dataBuf.data(), &dataLen);
        if (status != ERROR_SUCCESS) {
            if (result == ERROR_SUCCESS) result = status;
            continue;
        }
        status = RegSetValueExW(hDst, nameBuf.data(), 0, type, dataBuf.data(), dataLen);
        if (status != ERROR_SUCCESS && result == ERROR_SUCCESS) {
            result = status;
        }
    }

    for (DWORD i = 0; i < subKeyCount; ++i) {
        DWORD nameLen = static_cast<DWORD>(nameBuf.size());
        status = RegEnumKeyExW(hSrc, i, nameBuf.data(), &nameLen, nullptr, nullptr, nullptr,
                               nullptr);
        if (status != ERROR_SUCCESS) {
            if (result == ERROR_SUCCESS) result = status;
            continue;
        }
        HKEY hSrcChild = nullptr;
        status = RegOpenKeyExW(hSrc, nameBuf.data(), 0, KEY_READ, &hSrcChild);
        if (status != ERROR_SUCCESS) {
            if (result == ERROR_SUCCESS) result = status;
            continue;
        }
        HKEY hDstChild = nullptr;
        status = RegCreateKeyExW(hDst, nameBuf.data(), 0, nullptr, REG_OPTION_NON_VOLATILE,
                                 KEY_ALL_ACCESS, nullptr, &hDstChild, nullptr);
        if (status == ERROR_SUCCESS) {
            status = CopyRegistryTreeWithoutSecurity(hSrcChild, hDstChild, depth + 1);
            RegCloseKey(hDstChild);
        }
        RegCloseKey(hSrcChild);
        if (status != ERROR_SUCCESS && result == ERROR_SUCCESS) {
            result = status;
        }
    }
    return result;
}

void WriteXwComponentNode(
    HKEY hPrivateHiveRoot, const GUID& compGuid, DWORD classType, const wchar_t* name,
    const std::filesystem::path& dllPath) {
    const std::wstring base = std::wstring(kXWizardsHiveSubKey) + L"\\Components\\" +
                              GuidToUpperBraceString(compGuid);
    RegWriteSz(hPrivateHiveRoot, base, nullptr, name);
    RegWriteSz(hPrivateHiveRoot, base, L"Class Description", name);
    RegWriteDword(hPrivateHiveRoot, base, L"Class Type", classType);
    RegWriteDword(hPrivateHiveRoot, base, L"Behavior", 0);
    RegWriteSz(hPrivateHiveRoot, base, L"File Name", dllPath.wstring(), REG_EXPAND_SZ);
}

void WriteXwLink(
    HKEY hPrivateHiveRoot, const GUID& parentGuid, DWORD parentType, const wchar_t* parentName,
    const GUID& childGuid, DWORD childType, const wchar_t* childName, DWORD ordinal) {
    const std::wstring pStr = GuidToUpperBraceString(parentGuid);
    const std::wstring cStr = GuidToUpperBraceString(childGuid);

    const std::wstring childKey = std::wstring(kXWizardsHiveSubKey) + L"\\Components\\" +
                                  pStr + L"\\Children\\" + cStr;
    RegWriteSz(hPrivateHiveRoot, childKey, nullptr, childName);
    RegWriteSz(hPrivateHiveRoot, childKey, L"Class Description", childName);
    RegWriteDword(hPrivateHiveRoot, childKey, L"Class Type", childType);
    RegWriteDword(hPrivateHiveRoot, childKey, L"Ordinal", ordinal);
    RegWriteDword(hPrivateHiveRoot, childKey, L"User Flags", 0);

    const std::wstring parentKey = std::wstring(kXWizardsHiveSubKey) + L"\\Components\\" +
                                   cStr + L"\\Parents\\" + pStr;
    RegWriteSz(hPrivateHiveRoot, parentKey, nullptr, parentName);
    RegWriteSz(hPrivateHiveRoot, parentKey, L"Class Description", parentName);
    RegWriteDword(hPrivateHiveRoot, parentKey, L"Class Type", parentType);
}

bool OpenPrivateRegistryHive() {
    if (g_privateRegistryHive) {
        return true;
    }

    // Wh_GetModStoragePath is per-mod storage. Keep one directory per user,
    // then a GUID-named hive file per Explorer/mod instance. RegLoadAppKeyW
    // holds its backing file exclusively for the lifetime of the loaded hive,
    // so sharing a path between explorer.exe processes is not safe.
    std::wstring userSid;
    const size_t sidSeparator = g_hkcuNtPrefix.find_last_of(L'\\');
    if (sidSeparator != std::wstring::npos && sidSeparator + 1 < g_hkcuNtPrefix.size()) {
        userSid = g_hkcuNtPrefix.substr(sidSeparator + 1);
    }
    if (userSid.empty()) {
        Wh_Log(L"Cannot identify the current user's SID for the private registry hive");
        return false;
    }

    GUID hiveInstanceId{};
    const HRESULT guidStatus = CoCreateGuid(&hiveInstanceId);
    if (FAILED(guidStatus)) {
        Wh_Log(L"Could not generate a unique private-hive file name (0x%08lX)",
               static_cast<unsigned long>(guidStatus));
        return false;
    }

    const std::filesystem::path storageDir = g_vanStoreBaseDir.parent_path() / userSid;
    const std::wstring fileName =
        L"win8-network-flyout-private-" + std::to_wstring(GetCurrentProcessId()) + L"-" +
        GuidToUpperBraceString(hiveInstanceId) + L".hiv";
    const std::filesystem::path hiveFile = storageDir / fileName;
    std::error_code ec;
    std::filesystem::create_directories(storageDir, ec);
    if (ec) {
        Wh_Log(L"Could not create private-hive storage directory (code %d)", ec.value());
        return false;
    }

    constexpr unsigned kLoadAttempts = 5;
    LSTATUS status = ERROR_GEN_FAILURE;
    HKEY hive = nullptr;
    for (unsigned attempt = 0; attempt < kLoadAttempts; ++attempt) {
        hive = nullptr;
        status = RegLoadAppKeyW(hiveFile.c_str(), &hive, KEY_ALL_ACCESS, 0, 0);
        if (status == ERROR_SUCCESS) {
            break;
        }

        if ((status != ERROR_SHARING_VIOLATION && status != ERROR_LOCK_VIOLATION) ||
            attempt + 1 >= kLoadAttempts) {
            break;
        }

        const DWORD delayMs = 50u << attempt;  // 50, 100, 200, 400 ms
        Wh_Log(L"RegLoadAppKeyW hit a transient file lock; retry %u/%u after %lu ms",
               attempt + 1, kLoadAttempts, static_cast<unsigned long>(delayMs));
        if (g_stopEvent) {
            if (WaitForSingleObject(g_stopEvent, delayMs) == WAIT_OBJECT_0) {
                break;
            }
        } else {
            Sleep(delayMs);
        }
    }

    if (status != ERROR_SUCCESS) {
        Wh_Log(L"RegLoadAppKeyW failed for this instance's private hive (%ld)",
               static_cast<long>(status));
        std::filesystem::remove(hiveFile, ec);
        return false;
    }

    g_privateRegistryHive = hive;
    g_privateRegistryHiveFile = hiveFile;
    Wh_Log(L"Loaded this Explorer instance's private application hive from Windhawk storage");
    return true;
}

bool PopulatePrivateRegistryHiveOnce() {
    ScopedHookGuard guard;
    g_privateHiveWriteFailures.store(0, std::memory_order_relaxed);

    HKEY hPrivateHiveRoot = g_privateRegistryHive;
    if (!hPrivateHiveRoot) {
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
        RegWriteDword(hPrivateHiveRoot, sub, L"Active", 1);
        RegWriteDword(hPrivateHiveRoot, sub, L"Ordinal", mm.ordinal);
        RegWriteDword(hPrivateHiveRoot, sub, L"PreLogon", 1);
        RegWriteDword(hPrivateHiveRoot, sub, L"NotInOOBE", 0);
        RegWriteSz(hPrivateHiveRoot, sub, L"DisplayName", (g_vanStoreDir / mm.dllName).wstring());
    }

    // 1b. Master VAN component key {8B9D20EE-...}: Active=0 plus the
    //     100/200/300 ordinal->CLSID string map (matches the layout of a
    //     real Windows machine with the View Available Networks stack).
    {
        const std::wstring master = L"Control_VAN\\" + GuidToUpperBraceString(CLSID_VanMasterComponent);
        RegWriteDword(hPrivateHiveRoot, master, L"Active", 0);
        RegWriteSz(hPrivateHiveRoot, master, L"100", GuidToUpperBraceString(CLSID_Dot3MediaManager));
        RegWriteSz(hPrivateHiveRoot, master, L"200", GuidToUpperBraceString(CLSID_RasMediaManager));
        RegWriteSz(hPrivateHiveRoot, master, L"300", GuidToUpperBraceString(CLSID_WlanMediaManager));
    }

    // 1c. VAN settings key (HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\VAN)
    RegWriteDword(hPrivateHiveRoot, L"Software_VAN", L"Initialized", 1);

    // 2. Seed private-hive XWizards from the machine's real HKLM tree (read-only
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
                    hPrivateHiveRoot, kXWizardsHiveSubKey, 0, nullptr, REG_OPTION_NON_VOLATILE,
                    KEY_ALL_ACCESS, nullptr, &hVirtXw, nullptr) == ERROR_SUCCESS) {
                // RegCopyTreeW also copies the source security descriptor
                // (Microsoft docs, RegCopyTree remarks), which would make the
                // private-hive key read-only. Copy values and subkeys by
                // hand instead: RegCreateKeyExW with NULL security attributes
                // gives every new key the ACL inherited from its parent.
                const LSTATUS copyStatus = CopyRegistryTreeWithoutSecurity(hRealXw, hVirtXw, 0);
                if (copyStatus != ERROR_SUCCESS) {
                    Wh_Log(L"[registry] XWizards seed copy incomplete: status=%ld",
                           static_cast<long>(copyStatus));
                }
                RegCloseKey(hVirtXw);
            }
            RegCloseKey(hRealXw);
        } else {
            RegCreateKeyExW(
                hPrivateHiveRoot, kXWizardsHiveSubKey, 0, nullptr, REG_OPTION_NON_VOLATILE,
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
        hPrivateHiveRoot, CLSID_VanGenericCFETask, kXwComponentTypeTask, taskName, vanDll);

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
            hPrivateHiveRoot, pg.guid, kXwComponentTypePage, pg.name, wlanConnDll);
        WriteXwLink(
            hPrivateHiveRoot, CLSID_VanGenericCFETask, kXwComponentTypeTask, taskName, pg.guid,
            kXwComponentTypePage, pg.name, ordinal++);
    }

    // 3. Private-hive CLSID mirror (used by the HKCR / HKLM registry redirects).
    for (const auto& entry : kInProcComTable) {
        const std::wstring clsidBase =
            L"CLSID\\" + GuidToUpperBraceString(entry.clsid);
        const std::wstring inproc = clsidBase + L"\\InProcServer32";
        RegWriteSz(hPrivateHiveRoot, clsidBase, nullptr, entry.description);
        RegWriteSz(hPrivateHiveRoot, inproc, nullptr, (g_vanStoreDir / entry.dllBaseName).wstring());
        RegWriteSz(hPrivateHiveRoot, inproc, L"ThreadingModel", entry.threadingModel);
    }

    // 4. Read-back verification: every media manager must be visible with
    //    Active=1, and no write may have failed. A silently incomplete
    //    Control_VAN tree makes van.dll find zero providers ("No connections
    //    are available") without any other symptom.
    const unsigned failures = g_privateHiveWriteFailures.load(std::memory_order_relaxed);
    unsigned verifiedManagers = 0;
    for (const auto& mm : managers) {
        const std::wstring sub = L"Control_VAN\\" + GuidToUpperBraceString(mm.clsid);
        DWORD active = 0;
        DWORD size = sizeof(active);
        if (RegGetValueW(hPrivateHiveRoot, sub.c_str(), L"Active", RRF_RT_REG_DWORD, nullptr,
                         &active, &size) == ERROR_SUCCESS && active == 1) {
            ++verifiedManagers;
        }
    }
    // Only the Control_VAN media managers are mandatory. Failures copying
    // optional XWizards data are reported as warnings, not as a reason to stay
    // passive.
    if (verifiedManagers != ARRAYSIZE(managers)) {
        Wh_Log(L"[private hive] incomplete: verified=%u/%u failures=%u",
               verifiedManagers, static_cast<unsigned>(ARRAYSIZE(managers)), failures);
        return false;
    }

    Wh_Log(L"[private hive] verified: media managers=%u/%u, non-critical write failures=%u",
           verifiedManagers, static_cast<unsigned>(ARRAYSIZE(managers)), failures);
    return true;
}

// Populates the private hive. On a partial first attempt, discard only the
// mod-owned hive subtrees and retry once. No key in HKCU/HKLM is created,
// modified, or deleted here. The hive is unique to this Explorer instance.
bool PopulatePrivateRegistryHive() {
    if (!g_privateRegistryHive) {
        return false;
    }
    if (PopulatePrivateRegistryHiveOnce()) {
        return true;
    }

    ScopedHookGuard guard;
    const wchar_t* const ownedSubkeys[] = {
        L"Control_VAN", L"Software_VAN", kXWizardsHiveSubKey, L"CLSID",
    };
    bool cleared = true;
    for (const wchar_t* subkey : ownedSubkeys) {
        const LSTATUS status = RegDeleteTreeW(g_privateRegistryHive, subkey);
        if (status != ERROR_SUCCESS && status != ERROR_FILE_NOT_FOUND &&
            status != ERROR_PATH_NOT_FOUND) {
            cleared = false;
            Wh_Log(L"[private hive] could not reset %s (%ld)", subkey,
                   static_cast<long>(status));
        }
    }
    if (!cleared) {
        Wh_Log(L"[private hive] reset failed; refusing to use a potentially partial hive");
        return false;
    }

    Wh_Log(L"[private hive] rebuilding owned subtrees after an incomplete population");
    const bool ok = PopulatePrivateRegistryHiveOnce();
    Wh_Log(L"[private hive] rebuild result: %s", ok ? L"ok" : L"failed");
    return ok;
}

// ===========================================================================
// Private-hive path mapping: Win32 open/create + NT NtOpenKey/NtOpenKeyEx/NtCreateKey
// ===========================================================================
struct PrivateHivePrefixMap {
    const wchar_t* regPrefix;   // queried subkey prefix (case-insensitive)
    const wchar_t* hiveSubKey;  // hive subkey it maps to
};

// Selected HKLM accesses and the managed CLSID mirror are redirected into the
// private application hive. Native COM activation is handled separately by the hooks.
constexpr PrivateHivePrefixMap kHklmHiveMaps[] = {
    {L"SYSTEM\\CurrentControlSet\\Control\\VAN", L"Control_VAN"},
    {L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\XWizards", kXWizardsHiveSubKey},
    {L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\VAN", L"Software_VAN"},
    {L"SOFTWARE\\Classes\\CLSID", L"CLSID"},
};
constexpr PrivateHivePrefixMap kHkcrHiveMaps[] = {
    {L"CLSID", L"CLSID"},
    {L"Software\\Classes\\CLSID", L"CLSID"},
};
constexpr PrivateHivePrefixMap kHkcuHiveMaps[] = {
    {L"Software\\Classes\\CLSID", L"CLSID"},
};

// Stack-only private-hive path builder (loader-lock friendly).
bool BuildPrivateHivePath(HKEY hKey, const wchar_t* subKey, wchar_t* out, size_t cchOut) {
    if (!subKey || !subKey[0]) {
        return false;
    }
    const PrivateHivePrefixMap* maps = nullptr;
    size_t mapCount = 0;
    if (hKey == HKEY_LOCAL_MACHINE) {
        maps = kHklmHiveMaps;
        mapCount = ARRAYSIZE(kHklmHiveMaps);
    } else if (hKey == HKEY_CLASSES_ROOT) {
        maps = kHkcrHiveMaps;
        mapCount = ARRAYSIZE(kHkcrHiveMaps);
    } else if (hKey == HKEY_CURRENT_USER) {
        maps = kHkcuHiveMaps;
        mapCount = ARRAYSIZE(kHkcuHiveMaps);
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
        int n = swprintf(out, cchOut, L"%s%s", maps[m].hiveSubKey, rest);
        if (n > 0 && static_cast<size_t>(n) < cchOut) {
            return true;
        }
    }
    return false;
}

using RegOpenKeyExW_t = LSTATUS(WINAPI*)(HKEY, LPCWSTR, DWORD, REGSAM, PHKEY);
RegOpenKeyExW_t g_origRegOpenKeyExW = nullptr;
std::atomic<unsigned> g_controlVanOpenLogs{0};

constexpr ACCESS_MASK kRegistryWriteAccessMask =
    KEY_SET_VALUE | KEY_CREATE_SUB_KEY | KEY_CREATE_LINK | DELETE | WRITE_DAC | WRITE_OWNER |
    MAXIMUM_ALLOWED;

bool HasRegistryWriteAccess(ACCESS_MASK desiredAccess) noexcept {
    return (desiredAccess & kRegistryWriteAccessMask) != 0;
}

LSTATUS WINAPI Hook_RegOpenKeyExW(
    HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) {
    if (phkResult && lpSubKey && lpSubKey[0] &&
        g_privateRegistryHiveReady.load(std::memory_order_acquire) && !g_inHookGuard &&
        IsManagedVanCallContext(__builtin_return_address(0))) {
        wchar_t mapped[1024];
        if (BuildPrivateHivePath(hKey, lpSubKey, mapped, ARRAYSIZE(mapped))) {
            ScopedHookGuard guard;
            HKEY hive = g_privateRegistryHive;
            if (!hive) {
                return HasRegistryWriteAccess(samDesired) ? ERROR_FILE_NOT_FOUND
                                                          : g_origRegOpenKeyExW(
                                                                hKey, lpSubKey, ulOptions,
                                                                samDesired, phkResult);
            }
            LSTATUS st = g_origRegOpenKeyExW(
                hive, mapped, ulOptions, samDesired, phkResult);
            // Compact diagnostic: does VAN actually reach the private
            // Control_VAN tree? Only the first few opens are logged.
            if (wcsstr(mapped, L"Control_VAN") &&
                g_controlVanOpenLogs.fetch_add(1, std::memory_order_relaxed) < 8) {
                Wh_Log(L"[registry] Control_VAN open: status=%ld access=0x%lX",
                       static_cast<long>(st), static_cast<unsigned long>(samDesired));
            }
            if (st == ERROR_SUCCESS) {
                return st;  // genuine kernel HKEY - RegNotifyChangeKeyValue works
            }
            // Preserve read-only fallback behavior, but never hand out a live
            // system/class key for a write-capable request when the private hive is
            // missing or inaccessible.
            if (HasRegistryWriteAccess(samDesired)) {
                return st;
            }
        }
    }
    return g_origRegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

using RegOpenKeyExA_t = LSTATUS(WINAPI*)(HKEY, LPCSTR, DWORD, REGSAM, PHKEY);
RegOpenKeyExA_t g_origRegOpenKeyExA = nullptr;

LSTATUS WINAPI Hook_RegOpenKeyExA(
    HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) {
    if (phkResult && lpSubKey && lpSubKey[0] &&
        g_privateRegistryHiveReady.load(std::memory_order_acquire) && !g_inHookGuard &&
        IsManagedVanCallContext(__builtin_return_address(0))) {
        wchar_t subWide[512];
        int n = MultiByteToWideChar(CP_ACP, 0, lpSubKey, -1, subWide, ARRAYSIZE(subWide));
        if (n > 0) {
            wchar_t mapped[1024];
            if (BuildPrivateHivePath(hKey, subWide, mapped, ARRAYSIZE(mapped))) {
                ScopedHookGuard guard;
                HKEY hive = g_privateRegistryHive;
                if (!hive) {
                    return HasRegistryWriteAccess(samDesired) ? ERROR_FILE_NOT_FOUND
                                                              : g_origRegOpenKeyExA(
                                                                    hKey, lpSubKey, ulOptions,
                                                                    samDesired, phkResult);
                }
                LSTATUS st = g_origRegOpenKeyExW(
                    hive, mapped, ulOptions, samDesired, phkResult);
                if (st == ERROR_SUCCESS) {
                    return st;
                }
                if (HasRegistryWriteAccess(samDesired)) {
                    return st;
                }
            }
        }
    }
    return g_origRegOpenKeyExA(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

using RegCreateKeyExW_t = LSTATUS(WINAPI*)(
    HKEY, LPCWSTR, DWORD, LPWSTR, DWORD, REGSAM, LPSECURITY_ATTRIBUTES, PHKEY, LPDWORD);
RegCreateKeyExW_t g_origRegCreateKeyExW = nullptr;

LSTATUS WINAPI Hook_RegCreateKeyExW(
    HKEY hKey, LPCWSTR lpSubKey, DWORD reserved, LPWSTR lpClass, DWORD options,
    REGSAM samDesired, LPSECURITY_ATTRIBUTES securityAttributes, PHKEY phkResult,
    LPDWORD disposition) {
    if (phkResult && lpSubKey && g_privateRegistryHiveReady.load(std::memory_order_acquire) &&
        !g_inHookGuard &&
        IsManagedVanCallContext(__builtin_return_address(0))) {
        wchar_t mapped[1024];
        if (BuildPrivateHivePath(hKey, lpSubKey, mapped, ARRAYSIZE(mapped))) {
            ScopedHookGuard guard;
            // A mapped create must never fall through to the live HKLM/HKCR
            // key, even if the private application hive cannot be written.
            HKEY hive = g_privateRegistryHive;
            if (!hive) {
                return ERROR_FILE_NOT_FOUND;
            }
            return g_origRegCreateKeyExW(
                hive, mapped, reserved, lpClass, options, samDesired,
                securityAttributes, phkResult, disposition);
        }
    }
    return g_origRegCreateKeyExW(
        hKey, lpSubKey, reserved, lpClass, options, samDesired, securityAttributes,
        phkResult, disposition);
}

using RegCreateKeyExA_t = LSTATUS(WINAPI*)(
    HKEY, LPCSTR, DWORD, LPSTR, DWORD, REGSAM, LPSECURITY_ATTRIBUTES, PHKEY, LPDWORD);
RegCreateKeyExA_t g_origRegCreateKeyExA = nullptr;

LSTATUS WINAPI Hook_RegCreateKeyExA(
    HKEY hKey, LPCSTR lpSubKey, DWORD reserved, LPSTR lpClass, DWORD options,
    REGSAM samDesired, LPSECURITY_ATTRIBUTES securityAttributes, PHKEY phkResult,
    LPDWORD disposition) {
    if (phkResult && lpSubKey && g_privateRegistryHiveReady.load(std::memory_order_acquire) &&
        !g_inHookGuard &&
        IsManagedVanCallContext(__builtin_return_address(0))) {
        wchar_t subWide[512];
        const int wideCount = MultiByteToWideChar(
            CP_ACP, 0, lpSubKey, -1, subWide, ARRAYSIZE(subWide));
        if (wideCount > 0) {
            wchar_t mapped[1024];
            if (BuildPrivateHivePath(hKey, subWide, mapped, ARRAYSIZE(mapped))) {
                char mappedAnsi[4096];
                const int ansiCount = WideCharToMultiByte(
                    CP_ACP, 0, mapped, -1, mappedAnsi, ARRAYSIZE(mappedAnsi), nullptr, nullptr);
                if (ansiCount <= 0) {
                    const DWORD error = GetLastError();
                    return error ? static_cast<LSTATUS>(error) : ERROR_NO_UNICODE_TRANSLATION;
                }
                ScopedHookGuard guard;
                HKEY hive = g_privateRegistryHive;
                if (!hive) {
                    return ERROR_FILE_NOT_FOUND;
                }
                return g_origRegCreateKeyExA(
                    hive, mappedAnsi, reserved, lpClass, options, samDesired,
                    securityAttributes, phkResult, disposition);
            }
        }
    }
    return g_origRegCreateKeyExA(
        hKey, lpSubKey, reserved, lpClass, options, samDesired, securityAttributes,
        phkResult, disposition);
}

// ---------------------------------------------------------------------------
// RegGetValueW virtualization for ReplaceVan.
// ---------------------------------------------------------------------------
// The native route presents ReplaceVan=2 only to reads of this exact key from
// this explorer.exe process. No HKLM/HKCU value is created or modified. When
// using downloaded/hydrated modules, their own pnidui code continues to see 0
// so the managed classic tray is not suppressed by a third-party ReplaceVan=1.
using RegGetValueW_t =
    LONG(WINAPI*)(HKEY, LPCWSTR, LPCWSTR, DWORD, LPDWORD, PVOID, LPDWORD);
RegGetValueW_t g_origRegGetValueW = nullptr;

constexpr wchar_t kReplaceVanSettingsSubKey[] =
    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Control Panel\\Settings\\Network";


LONG ReturnVirtualReplaceVanDword(
    DWORD value, DWORD flags, LPDWORD type, PVOID data, LPDWORD dataSize) {
    if ((flags & RRF_RT_REG_DWORD) == 0 && (flags & RRF_RT_ANY) != 0) {
        return ERROR_UNSUPPORTED_TYPE;
    }
    if (data && !dataSize) {
        return ERROR_INVALID_PARAMETER;
    }
    if (data && *dataSize < sizeof(value)) {
        *dataSize = sizeof(value);
        return ERROR_MORE_DATA;
    }
    if (type) {
        *type = REG_DWORD;
    }
    if (data) {
        memcpy(data, &value, sizeof(value));
    }
    if (dataSize) {
        *dataSize = sizeof(value);
    }
    return ERROR_SUCCESS;
}

LONG WINAPI Hook_RegGetValueW(
    HKEY hkey, LPCWSTR subKey, LPCWSTR value, DWORD flags, LPDWORD type,
    PVOID data, LPDWORD dataSize) {
    if (value && _wcsicmp(value, L"ReplaceVan") == 0 &&
        (hkey == HKEY_LOCAL_MACHINE || hkey == HKEY_CURRENT_USER) && subKey &&
        _wcsicmp(subKey, kReplaceVanSettingsSubKey) == 0 &&
        g_nativeSystemVan.load(std::memory_order_acquire) &&
        !g_shuttingDown.load(std::memory_order_acquire)) {
        return ReturnVirtualReplaceVanDword(2, flags, type, data, dataSize);
    }

    if (value && _wcsicmp(value, L"ReplaceVan") == 0 &&
        (hkey == HKEY_LOCAL_MACHINE || hkey == HKEY_CURRENT_USER) && subKey &&
        _wcsicmp(subKey, kReplaceVanSettingsSubKey) == 0 &&
        g_origRegGetValueW && !g_nativeSystemVan.load(std::memory_order_acquire) &&
        IsCallerManagedVanModule(__builtin_return_address(0))) {
        return ReturnVirtualReplaceVanDword(0, flags, type, data, dataSize);
    }
    return g_origRegGetValueW(hkey, subKey, value, flags, type, data, dataSize);
}

// ---------------------------------------------------------------------------
// NT-level open/create redirection (stack-only, no heap, no locks)
// ---------------------------------------------------------------------------
bool BuildRedirectedNtKeyPath(
    const wchar_t* fullPath, size_t fullPathLen, wchar_t* out, size_t cchOut,
    USHORT* outLengthBytes) {
    struct NtPrefixMap {
        const wchar_t* from;
        const wchar_t* to;  // private-hive-relative path component
    };
    static constexpr NtPrefixMap kMaps[] = {
        {L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Control\\VAN", L"Control_VAN"},
        {L"\\Registry\\Machine\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\XWizards",
         kXWizardsHiveSubKey},
        {L"\\Registry\\Machine\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\VAN",
         L"Software_VAN"},
        {L"\\Registry\\Machine\\SOFTWARE\\Classes\\CLSID", L"CLSID"},
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

        const wchar_t* suffix = atEnd ? L"" : fullPath + fromLen;
        int n = swprintf(out, cchOut, L"%s%s", m.to, suffix);
        if (n <= 0 || static_cast<size_t>(n) >= cchOut) {
            return false;
        }
        *outLengthBytes = static_cast<USHORT>(n * sizeof(wchar_t));
        return true;
    }

    // Also redirect the per-user half of HKCR when a native caller addresses
    // HKCU\Software\Classes\CLSID directly. Keep this stack-only because
    // the hook may run while loader-related locks are held.
    static constexpr wchar_t kHkcuClsidSuffix[] = L"\\Software\\Classes\\CLSID";
    const size_t hkcuPrefixLen = g_hkcuNtPrefix.size();
    const size_t clsidSuffixLen = ARRAYSIZE(kHkcuClsidSuffix) - 1;
    if (hkcuPrefixLen != 0 && fullPathLen >= hkcuPrefixLen + clsidSuffixLen &&
        StartsWithInsensitive(std::wstring_view(fullPath, fullPathLen), g_hkcuNtPrefix) &&
        _wcsnicmp(fullPath + hkcuPrefixLen, kHkcuClsidSuffix, clsidSuffixLen) == 0) {
        const size_t matchedLen = hkcuPrefixLen + clsidSuffixLen;
        const bool atEnd = fullPathLen == matchedLen;
        const bool boundary = atEnd || fullPath[matchedLen] == L'\\';
        if (boundary) {
            const wchar_t* suffix = atEnd ? L"" : fullPath + matchedLen;
            int n = swprintf(out, cchOut, L"CLSID%s", suffix);
            if (n <= 0 || static_cast<size_t>(n) >= cchOut) {
                return false;
            }
            *outLengthBytes = static_cast<USHORT>(n * sizeof(wchar_t));
            return true;
        }
    }
    return false;
}

// Builds the full source NT path for an OBJECT_ATTRIBUTES (including
// RootDirectory-relative opens) and, if it targets a virtualized key, returns
// the corresponding key name relative to g_privateRegistryHive.
bool TryBuildRedirectedNtKeyPath(
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
        // Resolve the root handle name so relative opens of live registry keys can
        // be mapped too. Private-hive handles do not match these source paths
        // and therefore pass through unchanged.
        if (!g_pfnNtQueryKey) {
            return false;
        }
        alignas(ULONG) BYTE queryBuf[1024];
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
        if (returned < offsetof(WhKeyNameInformation, Name) ||
            nameInfo->NameLength > sizeof(queryBuf) - offsetof(WhKeyNameInformation, Name) ||
            nameInfo->NameLength > returned - offsetof(WhKeyNameInformation, Name) ||
            nameInfo->NameLength == 0 ||
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

// RegSetValueExA/W and native writers ultimately use NtSetValueKey. A handle
// opened before our hooks were enabled must not bypass path redirection.
// Private-hive handles retain ordinary registry API behavior; no fake HKEYs.
using NtSetValueKey_t = NTSTATUS(NTAPI*)(HANDLE, PUNICODE_STRING, ULONG, ULONG, PVOID, ULONG);
NtSetValueKey_t g_origNtSetValueKey = nullptr;

NTSTATUS NTAPI Hook_NtSetValueKey(HANDLE key, PUNICODE_STRING valueName,
    ULONG titleIndex, ULONG type, PVOID data, ULONG dataSize) {
    if (!g_inHookGuard && g_privateRegistryHiveReady.load(std::memory_order_acquire) &&
        g_pfnNtQueryKey && g_privateRegistryHive &&
        (GetCurrentThreadId() == g_staThreadId.load(std::memory_order_acquire) ||
         IsCallerManagedVanModule(__builtin_return_address(0)))) {
        // Scope the extra stale-handle guard to the mod's STA/direct managed
        // native callers, not arbitrary Explorer components using real keys.
        alignas(ULONG) BYTE nameBuffer[4096];
        ULONG returned = 0;
        const NTSTATUS query = g_pfnNtQueryKey(key, kKeyNameInformationClass,
            nameBuffer, sizeof(nameBuffer), &returned);
        if (query == 0) {
            const auto* info = reinterpret_cast<const WhKeyNameInformation*>(nameBuffer);
            const size_t header = offsetof(WhKeyNameInformation, Name);
            if (info->NameLength <= sizeof(nameBuffer) - header &&
                info->NameLength % sizeof(wchar_t) == 0) {
                wchar_t redirected[2200];
                USHORT length = 0;
                if (BuildRedirectedNtKeyPath(info->Name,
                        info->NameLength / sizeof(wchar_t), redirected,
                        ARRAYSIZE(redirected), &length)) {
                    // Do not silently grant rights by reopening a read-only real
                    // handle against our writable private hive. Deny stale real handles;
                    // callers must reopen through the ordinary redirection hooks.
                    return static_cast<NTSTATUS>(0xC0000022L); // STATUS_ACCESS_DENIED
                }
            }
        }
    }
    return g_origNtSetValueKey(key, valueName, titleIndex, type, data, dataSize);
}

NtCreateKey_t g_origNtCreateKey = nullptr;

NTSTATUS NTAPI Hook_NtCreateKey(
    PHANDLE KeyHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes,
    ULONG TitleIndex, PUNICODE_STRING Class, ULONG CreateOptions, PULONG Disposition) {
    if (KeyHandle && g_privateRegistryHiveReady.load(std::memory_order_acquire) &&
        !g_inHookGuard && g_privateRegistryHive &&
        IsManagedVanCallContext(__builtin_return_address(0))) {
        wchar_t redirected[1100];
        USHORT redirectedLen = 0;
        if (TryBuildRedirectedNtKeyPath(
                ObjectAttributes, redirected, ARRAYSIZE(redirected), &redirectedLen)) {
            ScopedHookGuard guard;
            UNICODE_STRING us{};
            us.Buffer = redirected;
            us.Length = redirectedLen;
            us.MaximumLength = redirectedLen + sizeof(wchar_t);

            OBJECT_ATTRIBUTES oa{};
            oa.Length = sizeof(oa);
            oa.RootDirectory = g_privateRegistryHive;
            oa.ObjectName = &us;
            oa.Attributes = ObjectAttributes->Attributes;
            oa.SecurityDescriptor = ObjectAttributes->SecurityDescriptor;
            oa.SecurityQualityOfService = ObjectAttributes->SecurityQualityOfService;

            // Never fall back to the original target after a mapped create
            // fails; that could write into a live machine/class registration.
            return g_origNtCreateKey(
                KeyHandle, DesiredAccess, &oa, TitleIndex, Class, CreateOptions, Disposition);
        }
    }
    return g_origNtCreateKey(
        KeyHandle, DesiredAccess, ObjectAttributes, TitleIndex, Class, CreateOptions,
        Disposition);
}

NtOpenKey_t g_origNtOpenKey = nullptr;

NTSTATUS NTAPI Hook_NtOpenKey(PHANDLE KeyHandle, ACCESS_MASK DesiredAccess,
                              POBJECT_ATTRIBUTES ObjectAttributes) {
    if (KeyHandle && g_privateRegistryHiveReady.load(std::memory_order_acquire) &&
        !g_inHookGuard && g_privateRegistryHive &&
        IsManagedVanCallContext(__builtin_return_address(0))) {
        wchar_t redirected[1100];
        USHORT redirectedLen = 0;
        if (TryBuildRedirectedNtKeyPath(
                ObjectAttributes, redirected, ARRAYSIZE(redirected), &redirectedLen)) {
            ScopedHookGuard guard;
            UNICODE_STRING us{};
            us.Buffer = redirected;
            us.Length = redirectedLen;
            us.MaximumLength = redirectedLen + sizeof(wchar_t);

            OBJECT_ATTRIBUTES oa{};
            oa.Length = sizeof(oa);
            oa.RootDirectory = g_privateRegistryHive;
            oa.ObjectName = &us;
            oa.Attributes = ObjectAttributes->Attributes;
            oa.SecurityDescriptor = ObjectAttributes->SecurityDescriptor;
            oa.SecurityQualityOfService = ObjectAttributes->SecurityQualityOfService;

            NTSTATUS st = g_origNtOpenKey(KeyHandle, DesiredAccess, &oa);
            if (NT_SUCCESS(st)) {
                return st;  // real kernel HKEY into the private application hive
            }
            // Read-only lookups may still consult the system key. A write-capable
            // open must fail closed instead of returning a live machine key.
            if (HasRegistryWriteAccess(DesiredAccess)) {
                return st;
            }
        }
    }
    return g_origNtOpenKey(KeyHandle, DesiredAccess, ObjectAttributes);
}

NtOpenKeyEx_t g_origNtOpenKeyEx = nullptr;

NTSTATUS NTAPI Hook_NtOpenKeyEx(PHANDLE KeyHandle, ACCESS_MASK DesiredAccess,
                                POBJECT_ATTRIBUTES ObjectAttributes, ULONG OpenOptions) {
    if (KeyHandle && g_privateRegistryHiveReady.load(std::memory_order_acquire) &&
        !g_inHookGuard && g_privateRegistryHive &&
        IsManagedVanCallContext(__builtin_return_address(0))) {
        wchar_t redirected[1100];
        USHORT redirectedLen = 0;
        if (TryBuildRedirectedNtKeyPath(
                ObjectAttributes, redirected, ARRAYSIZE(redirected), &redirectedLen)) {
            ScopedHookGuard guard;
            UNICODE_STRING us{};
            us.Buffer = redirected;
            us.Length = redirectedLen;
            us.MaximumLength = redirectedLen + sizeof(wchar_t);

            OBJECT_ATTRIBUTES oa{};
            oa.Length = sizeof(oa);
            oa.RootDirectory = g_privateRegistryHive;
            oa.ObjectName = &us;
            oa.Attributes = ObjectAttributes->Attributes;
            oa.SecurityDescriptor = ObjectAttributes->SecurityDescriptor;
            oa.SecurityQualityOfService = ObjectAttributes->SecurityQualityOfService;

            NTSTATUS st = g_origNtOpenKeyEx(KeyHandle, DesiredAccess, &oa, OpenOptions);
            if (NT_SUCCESS(st)) {
                return st;
            }
            if (HasRegistryWriteAccess(DesiredAccess)) {
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
        g_vanStoreReady.load(std::memory_order_acquire) &&
        !g_nativeSystemVan.load(std::memory_order_acquire) && !g_inHookGuard &&
        IsManagedVanCallContext(__builtin_return_address(0))) {
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

HMODULE WINAPI Hook_LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) try {
    if (lpLibFileName && g_vanStoreReady.load(std::memory_order_acquire) &&
        !g_nativeSystemVan.load(std::memory_order_acquire) && !g_inHookGuard &&
        IsManagedVanCallContext(__builtin_return_address(0))) {
        if (const wchar_t* managed = MatchManagedDllCanonicalName(lpLibFileName)) {
            ScopedHookGuard guard;
            const std::filesystem::path target = GetManagedDllPath(managed);
            DWORD effectiveFlags = dwFlags;
            if ((effectiveFlags & (LOAD_LIBRARY_AS_DATAFILE |
                                   LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
                                   LOAD_LIBRARY_AS_IMAGE_RESOURCE)) == 0) {
                // Use the target DLL's own directory plus System32 and any
                // AddDllDirectory paths. Avoid combining the incompatible
                // LOAD_WITH_ALTERED_SEARCH_PATH flag with LOAD_LIBRARY_SEARCH_*.
                effectiveFlags &= ~(LOAD_WITH_ALTERED_SEARCH_PATH |
                                    LOAD_LIBRARY_SEARCH_APPLICATION_DIR |
                                    LOAD_LIBRARY_SEARCH_SYSTEM32 |
                                    LOAD_LIBRARY_SEARCH_USER_DIRS |
                                    LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
                effectiveFlags |= LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR |
                                  LOAD_LIBRARY_SEARCH_SYSTEM32;
            }
            HMODULE hMod = g_origLoadLibraryExW(target.c_str(), hFile, effectiveFlags);
            ScopedRuntimeModuleReference callerReference(hMod);
            if (hMod && (effectiveFlags & (LOAD_LIBRARY_AS_DATAFILE |
                                           LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
                                           LOAD_LIBRARY_AS_IMAGE_RESOURCE)) == 0) {
                // The LoadLibrary reference belongs to the caller, which may
                // FreeLibrary it. Keep an independent, balanced runtime reference
                // so our published address range cannot outlive the loaded image.
                HMODULE retained = nullptr;
                if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                        reinterpret_cast<LPCWSTR>(hMod), &retained)) {
                    const DWORD error = GetLastError();
                    callerReference.release();
                    FreeLibrary(hMod);
                    SetLastError(error);
                    return nullptr;
                }
                AdoptManagedRuntimeReference(retained, managed);
                RefreshManagedDependencyImages();
            }
            return callerReference.release();
        }
    }
    return g_origLoadLibraryExW(lpLibFileName, hFile, dwFlags);
} catch (...) {
    // An operational C++ failure requests normal STA cleanup. It does not,
    // by itself, prove that callback/class teardown has failed.
    DisableModAfterCppException(L"Hook_LoadLibraryExW");
    SetLastError(ERROR_NOT_ENOUGH_MEMORY); return nullptr;
}

using LoadLibraryW_t = HMODULE(WINAPI*)(LPCWSTR);
LoadLibraryW_t g_origLoadLibraryW = nullptr;

HMODULE WINAPI Hook_LoadLibraryW(LPCWSTR lpLibFileName) try {
    if (lpLibFileName && g_vanStoreReady.load(std::memory_order_acquire) &&
        !g_nativeSystemVan.load(std::memory_order_acquire) && !g_inHookGuard &&
        IsManagedVanCallContext(__builtin_return_address(0))) {
        if (const wchar_t* managed = MatchManagedDllCanonicalName(lpLibFileName)) {
            HMODULE module = LoadManagedDll(managed);
            if (!module) return nullptr;
            HMODULE callerOwned = nullptr;
            // LoadManagedDll returns a runtime-owned borrowed handle. Preserve
            // LoadLibraryW's contract by returning a separate caller reference.
            if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                    reinterpret_cast<LPCWSTR>(module), &callerOwned)) return nullptr;
            return callerOwned;
        }
    }
    return g_origLoadLibraryW(lpLibFileName);
} catch (...) {
    // An operational C++ failure requests normal STA cleanup. It does not,
    // by itself, prove that callback/class teardown has failed.
    DisableModAfterCppException(L"Hook_LoadLibraryW");
    SetLastError(ERROR_NOT_ENOUGH_MEMORY); return nullptr;
}

// ===========================================================================
// In-proc COM resolution
// ===========================================================================
HRESULT TryGetInProcManagedClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) {
    if (!ppv) return E_POINTER;
    *ppv = nullptr;
    for (size_t i = 0; i < kInProcComCount; ++i) {
        if (IsEqualGUID(rclsid, kInProcComTable[i].clsid)) {
            HMODULE hMod = LoadManagedDll(kInProcComTable[i].dllBaseName);
            if (!hMod) {
                const DWORD error = GetLastError();
                return HRESULT_FROM_WIN32(error ? error : ERROR_MOD_NOT_FOUND);
            }
            using DllGetClassObject_t = HRESULT(WINAPI*)(REFCLSID, REFIID, LPVOID*);
            auto pfn = ResolveExportAddress<DllGetClassObject_t>(hMod, "DllGetClassObject");
            if (!pfn) {
                return CLASS_E_CLASSNOTAVAILABLE;
            }
            const HRESULT result = pfn(rclsid, riid, ppv);
            if (IsEqualGUID(rclsid, CLSID_WlanMediaManager)) {
                Wh_Log(L"WLAN legacy provider factory: hr=0x%08lX, object=%p",
                       static_cast<unsigned long>(result), *ppv);
            }
            return result;
        }
    }
    return CLASS_E_CLASSNOTAVAILABLE;
}

using CoGetClassObject_t = HRESULT(WINAPI*)(REFCLSID, DWORD, LPVOID, REFIID, LPVOID*);
CoGetClassObject_t g_origCoGetClassObject = nullptr;

HRESULT TryGetPopupWindowFactoryClassObject(REFIID riid, LPVOID* ppv) {
    if (!ppv) {
        return E_POINTER;
    }
    *ppv = nullptr;

    // Keep a module reference for the lifetime of Explorer; class objects may
    // outlive the hook call that obtained them.
    static HMODULE hImmersive = []() -> HMODULE {
        HMODULE h = GetModuleHandleW(L"Windows.UI.Immersive.dll");
        if (!h) {
            h = LoadLibraryExW(
                L"Windows.UI.Immersive.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        }
        return h;
    }();
    if (!hImmersive) {
        const DWORD error = GetLastError();
        return HRESULT_FROM_WIN32(error ? error : ERROR_MOD_NOT_FOUND);
    }

    using DllGetClassObject_t = HRESULT(WINAPI*)(REFCLSID, REFIID, LPVOID*);
    auto pfn = reinterpret_cast<DllGetClassObject_t>(
        GetProcAddress(hImmersive, "DllGetClassObject"));
    return pfn ? pfn(CLSID_PopupWindowFactory, riid, ppv) : CLASS_E_CLASSNOTAVAILABLE;
}

std::atomic<unsigned> g_providerComTraceCount{0};
// Counts activations of the three media managers requested by managed VAN code.
// The first-show watchdog uses it to detect "ShowVAN returned S_OK but VAN
// never asked for a single provider" (empty Control_VAN enumeration).
std::atomic<unsigned> g_providerActivations{0};
std::atomic<unsigned> g_popupFactoryLogCount{0};

void TraceProviderComGate(const wchar_t* api, REFCLSID clsid, void* caller) noexcept {
    if (!IsEqualGUID(clsid, CLSID_WlanMediaManager) &&
        !IsEqualGUID(clsid, CLSID_Dot3MediaManager) &&
        !IsEqualGUID(clsid, CLSID_RasMediaManager)) return;
    if (IsManagedVanCallContext(caller) &&
        g_vanStoreReady.load(std::memory_order_acquire)) {
        g_providerActivations.fetch_add(1, std::memory_order_relaxed);
    }
    if (g_providerComTraceCount.fetch_add(1, std::memory_order_relaxed) >= 24) return;
    const wchar_t* provider = IsEqualGUID(clsid, CLSID_WlanMediaManager) ? L"WLAN" :
        IsEqualGUID(clsid, CLSID_Dot3MediaManager) ? L"Ethernet" : L"RAS";
    Wh_Log(L"Provider COM gate: api=%s, provider=%s, thread=%lu, caller=%p, "
           L"context=%u, ready=%u, guard=%u, depth=%u",
           api, provider, GetCurrentThreadId(), caller,
           IsManagedVanCallContext(caller) ? 1u : 0u,
           g_vanStoreReady.load(std::memory_order_acquire) ? 1u : 0u,
           g_inHookGuard ? 1u : 0u, g_comActivationDepth);
}

HRESULT WINAPI Hook_CoGetClassObject(
    REFCLSID rclsid, DWORD dwClsContext, LPVOID pvReserved, REFIID riid, LPVOID* ppv) try {
    TraceProviderComGate(L"CoGetClassObject", rclsid, __builtin_return_address(0));
    if (ppv && (dwClsContext & CLSCTX_INPROC_SERVER) != 0 && g_vanStoreReady.load(std::memory_order_acquire) &&
        !g_nativeSystemVan.load(std::memory_order_acquire) &&
        !g_shuttingDown.load(std::memory_order_acquire) && !g_inHookGuard &&
        IsManagedVanCallContext(__builtin_return_address(0)) &&
        CanEnterManagedComActivation(rclsid)) {
        ScopedComActivationGuard activationGuard(rclsid);
        HRESULT hr = TryGetInProcManagedClassObject(rclsid, riid, ppv);
        if (SUCCEEDED(hr)) {
            return hr;
        }
    }

    HRESULT hr = g_origCoGetClassObject(rclsid, dwClsContext, pvReserved, riid, ppv);
    if (FAILED(hr) && ppv && IsEqualGUID(rclsid, CLSID_PopupWindowFactory) &&
        (dwClsContext & CLSCTX_INPROC_SERVER) != 0 &&
        g_vanStoreReady.load(std::memory_order_acquire) &&
        !g_nativeSystemVan.load(std::memory_order_acquire) &&
        !g_shuttingDown.load(std::memory_order_acquire) && !g_inHookGuard &&
        IsManagedVanCallContext(__builtin_return_address(0)) &&
        CanEnterManagedComActivation(rclsid)) {
        ScopedComActivationGuard activationGuard(rclsid);
        HRESULT fallback = TryGetPopupWindowFactoryClassObject(riid, ppv);
        if (g_popupFactoryLogCount.fetch_add(1, std::memory_order_relaxed) < 4) {
            Wh_Log(L"[com] PopupWindowFactory: original hr=0x%08lX fallback hr=0x%08lX",
                   static_cast<unsigned long>(hr), static_cast<unsigned long>(fallback));
        }
        if (SUCCEEDED(fallback)) {
            return fallback;
        }
    }
    return hr;
} catch (...) {
    // An operational C++ failure requests normal STA cleanup. It does not,
    // by itself, prove that callback/class teardown has failed.
    DisableModAfterCppException(L"Hook_CoGetClassObject");
    if (ppv) *ppv = nullptr; return E_UNEXPECTED;
}

using CoCreateInstance_t = HRESULT(WINAPI*)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
CoCreateInstance_t g_origCoCreateInstance = nullptr;

HRESULT WINAPI Hook_CoCreateInstance(
    REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID* ppv) try {
    TraceProviderComGate(L"CoCreateInstance", rclsid, __builtin_return_address(0));
    if (ppv && (dwClsContext & CLSCTX_INPROC_SERVER) != 0 && g_vanStoreReady.load(std::memory_order_acquire) &&
        !g_nativeSystemVan.load(std::memory_order_acquire) &&
        !g_shuttingDown.load(std::memory_order_acquire) && !g_inHookGuard &&
        IsManagedVanCallContext(__builtin_return_address(0)) &&
        CanEnterManagedComActivation(rclsid)) {
        ScopedComActivationGuard activationGuard(rclsid);
        IClassFactory* pFactory = nullptr;
        if (SUCCEEDED(TryGetInProcManagedClassObject(
                rclsid, IID_IClassFactory, reinterpret_cast<void**>(&pFactory))) &&
            pFactory) {
            ScopedComReference<IClassFactory> factoryOwner(pFactory);
            HRESULT hr = pFactory->CreateInstance(pUnkOuter, riid, ppv);
            if (IsEqualGUID(rclsid, CLSID_WlanMediaManager)) {
                Wh_Log(L"WLAN legacy provider CreateInstance: hr=0x%08lX, object=%p",
                       static_cast<unsigned long>(hr), *ppv);
            }
            return hr;
        }
    }

    HRESULT hr = g_origCoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);
    if (FAILED(hr) && ppv && IsEqualGUID(rclsid, CLSID_PopupWindowFactory) &&
        (dwClsContext & CLSCTX_INPROC_SERVER) != 0 &&
        g_vanStoreReady.load(std::memory_order_acquire) &&
        !g_nativeSystemVan.load(std::memory_order_acquire) &&
        !g_shuttingDown.load(std::memory_order_acquire) && !g_inHookGuard &&
        IsManagedVanCallContext(__builtin_return_address(0)) &&
        CanEnterManagedComActivation(rclsid)) {
        ScopedComActivationGuard activationGuard(rclsid);
        IClassFactory* pFactory = nullptr;
        HRESULT factoryHr = TryGetPopupWindowFactoryClassObject(
            IID_IClassFactory, reinterpret_cast<void**>(&pFactory));
        if (SUCCEEDED(factoryHr) && pFactory) {
            ScopedComReference<IClassFactory> factoryOwner(pFactory);
            hr = pFactory->CreateInstance(pUnkOuter, riid, ppv);
        }
    }
    return hr;
} catch (...) {
    // An operational C++ failure requests normal STA cleanup. It does not,
    // by itself, prove that callback/class teardown has failed.
    DisableModAfterCppException(L"Hook_CoCreateInstance");
    if (ppv) *ppv = nullptr; return E_UNEXPECTED;
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

    const int languageIndex = GetEffectiveLanguageIndex();
    for (size_t i = 0; i < kHydratedStringCount; ++i) {
        const LocalizedResourceEntry& e = kHydratedStrings[i];
        if (e.id == uID && _wcsicmp(e.moduleLower, managed) == 0) {
            const wchar_t* text = e.text[static_cast<size_t>(languageIndex)];
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
    // Classic Windows 8 VAN side-pane window classes hosted inside explorer.exe.
    // Include the visible title in each supported UI language so the toggle
    // continues to work after selecting Spanish, French or Portuguese.
    for (const wchar_t* cls : {L"VANUIFrame", L"NativeHWNDHost", L"DirectUIHWND"}) {
        for (const wchar_t* title : {L"Networks", L"Reti", L"Redes", L"Réseaux"}) {
            HWND hWnd = FindWindowExW(nullptr, nullptr, cls, title);
            if (hWnd && IsWindowVisible(hWnd)) {
                DWORD pid = 0;
                GetWindowThreadProcessId(hWnd, &pid);
                if (pid == GetCurrentProcessId()) {
                    return true;
                }
            }
        }
    }
    return false;
}

// First-show watchdog state (STA thread only, except the atomics).
std::atomic<bool> g_firstShowChecked{false};
std::atomic<bool> g_showWatchdogRetryDone{false};
// Set when ShutdownVAN has run on an image that could not be unloaded: any new
// ShowVAN on it would touch freed internal state, so flyout requests are refused.
std::atomic<bool> g_vanRuntimeStale{false};
UINT_PTR g_showWatchdogTimerId = 0;
constexpr UINT kShowWatchdogDelayMs = 2500;

void InvokeShowVanOnStaThread(bool allowToggleClose) {
    if (g_shuttingDown.load(std::memory_order_acquire)) {
        return;
    }
    if (g_vanRuntimeStale.load(std::memory_order_acquire)) {
        Wh_Log(L"[supervisor] flyout request ignored: VAN runtime is stopped (restart Explorer)");
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
        wchar_t loadedVanPath[1024]{};
        const DWORD loadedPathLength = GetModuleFileNameW(
            g_vanModule, loadedVanPath, ARRAYSIZE(loadedVanPath));
        unsigned recognized = 0;
        for (size_t i = 0; i < kRuntimeDllCount; ++i)
            if (g_managedImageBegin[i].load(std::memory_order_acquire)) ++recognized;
        Wh_Log(L"VAN load provenance: path=%s, recognized private/native images=%u",
               loadedPathLength && loadedPathLength < ARRAYSIZE(loadedVanPath)
                   ? loadedVanPath : L"<unavailable>", recognized);
        Wh_Log(L"Hydrated van.dll loaded; ShowVAN=%p HideVAN=%p ShutdownVAN=%p",
               reinterpret_cast<void*>(g_pfnShowVAN), reinterpret_cast<void*>(g_pfnHideVAN),
               reinterpret_cast<void*>(g_pfnShutdownVAN));
    }

    if (!g_pfnShowVAN) {
        Wh_Log(L"ShowVAN export missing in van.dll");
        return;
    }

    if (allowToggleClose && g_pfnHideVAN && IsVanFlyoutWindowVisible()) {
        ObserveVanVoidCall(g_pfnHideVAN, L"HideVAN");
        return;
    }

    // ShowVAN spawns and manages its internal RunVANUI thread by itself;
    // never create a parallel VAN UI thread here.
    const bool armWatchdog = !g_nativeSystemVan.load(std::memory_order_acquire) &&
                             !g_firstShowChecked.exchange(true, std::memory_order_acq_rel);
    if (armWatchdog) {
        g_providerActivations.store(0, std::memory_order_relaxed);
    }
    HRESULT hr = ObserveShowVanCall(g_pfnShowVAN);
    if (FAILED(hr)) {
        Wh_Log(L"ShowVAN returned HRESULT 0x%08lX", static_cast<unsigned long>(hr));
    }
    if (armWatchdog && SUCCEEDED(hr)) {
        // Thread timer (no window): delivered to the STA message pump below.
        g_showWatchdogTimerId = SetTimer(nullptr, 0, kShowWatchdogDelayMs, nullptr);
        if (!g_showWatchdogTimerId) {
            Wh_Log(L"[supervisor] first-show watchdog could not be armed (%lu)", GetLastError());
        }
    }
}

// Unloads van.dll and its provider DLLs (everything except pnidui.dll, which
// backs the live tray icon) so the next ShowVAN starts from a pristine image.
// This mirrors what disabling/enabling the mod does and, unlike a bare
// ShutdownVAN + ShowVAN, also discards any stale global state inside van.dll.
// Runs on the STA thread only.
// BoundedShutdownVan runs van.dll's ShutdownVAN on a throwaway helper thread
// with a 2 s timeout so the STA message pump is never blocked. It is used by
// both the watchdog reload path and the teardown path.
void BoundedShutdownVan() {
    if (!g_pfnShutdownVAN) {
        return;
    }
    struct ShutdownVanHelper {
        ShutdownVAN_t fn;
        static DWORD WINAPI ThreadProc(LPVOID param) {
            auto* self = reinterpret_cast<ShutdownVanHelper*>(param);
            Wh_Log(L"Legacy call enter: ShutdownVAN (helper thread %lu)",
                   GetCurrentThreadId());
            self->fn();
            Wh_Log(L"Legacy call leave: ShutdownVAN");
            return 0;
        }
    } helper{g_pfnShutdownVAN};
    HANDLE hHelper = CreateThread(nullptr, 0, ShutdownVanHelper::ThreadProc,
                                  &helper, 0, nullptr);
    if (hHelper) {
        const DWORD waitResult = WaitForSingleObject(hHelper, 2000);
        if (waitResult != WAIT_OBJECT_0) {
            Wh_Log(L"ShutdownVAN did not return within 2s; abandoning helper "
                   L"thread and continuing teardown");
        }
        CloseHandle(hHelper);
    } else {
        Wh_Log(L"Could not create the ShutdownVAN helper thread (%lu); "
               L"skipping ShutdownVAN", GetLastError());
    }
    g_pfnShutdownVAN = nullptr;  // never touch it again
}

bool ReloadManagedVanRuntimeOnSta() {
    BoundedShutdownVan();

    g_pfnShowVAN = nullptr;
    g_pfnHideVAN = nullptr;
    g_pfnShutdownVAN = nullptr;
    g_vanModule = nullptr;

    // Stop treating these images as managed callers before they can unload.
    for (size_t i = 0; i < kRuntimeDllCount; ++i) {
        if (_wcsicmp(kRuntimeDllNames[i], L"pnidui.dll") == 0) continue;
        g_managedImageBegin[i].store(0, std::memory_order_release);
        g_managedImageEnd[i].store(0, std::memory_order_relaxed);
    }

    std::vector<HMODULE> toFree;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        std::vector<HMODULE> keep;
        for (HMODULE module : g_loadedVanModules) {
            wchar_t path[1024]{};
            const DWORD length = GetModuleFileNameW(module, path, ARRAYSIZE(path));
            const wchar_t* base = length && length < ARRAYSIZE(path) ? wcsrchr(path, L'\\') : nullptr;
            base = base ? base + 1 : path;
            if (_wcsicmp(base, L"pnidui.dll") == 0) {
                keep.push_back(module);
            } else {
                toFree.push_back(module);
            }
        }
        g_loadedVanModules.swap(keep);
    }
    {
        ScopedHookGuard guard;
        for (auto it = toFree.rbegin(); it != toFree.rend(); ++it) {
            FreeLibrary(*it);
        }
    }

    // Diagnostic: if van.dll is still mapped, something else holds a reference
    // and the reload cannot reset its internal state.
    HMODULE stillMapped = nullptr;
    const size_t vanIndex = 1;  // kRuntimeDllNames[1] == van.dll
    const bool mapped = g_managedFullPaths[vanIndex][0] &&
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           g_managedFullPaths[vanIndex], &stillMapped) && stillMapped;
    Wh_Log(L"[supervisor] managed runtime reloaded: released=%u van.dll still mapped=%u",
           static_cast<unsigned>(toFree.size()), mapped ? 1u : 0u);
    if (mapped) {
        // The image survived (someone holds a reference), so its globals are
        // still in the post-ShutdownVAN state. Calling ShowVAN on it again is
        // a use-after-shutdown risk: forbid any further ShowVAN this session.
        g_vanRuntimeStale.store(true, std::memory_order_release);
    }
    return !mapped;
}

unsigned g_watchdogAttempt = 0;          // STA thread only
bool g_watchdogDelayServed = false;      // STA thread only
constexpr UINT kWatchdogLateRetryDelayMs = 10000;

void ArmShowWatchdog(UINT delayMs) {
    g_showWatchdogTimerId = SetTimer(nullptr, 0, delayMs, nullptr);
    if (!g_showWatchdogTimerId) {
        Wh_Log(L"[supervisor] watchdog timer could not be armed (%lu)", GetLastError());
    }
}

// Runs on the STA thread ~2.5 s after the first ShowVAN of the session. At
// boot, ShowVAN can return S_OK while van.dll never even opens Control_VAN or
// activates a provider (log evidence: private hive verified 3/3, zero opens, zero
// COM activations, and a ShutdownVAN+ShowVAN retry changes nothing; only a
// full reload of the DLLs recovers). So: reload the managed runtime and show
// again, at most twice (the second time after a pause in case this is timing).
// Later shows are not checked: VAN keeps its providers and legitimately does
// not re-activate them.
void EvaluateShowWatchdogOnSta() {
    if (g_shuttingDown.load(std::memory_order_acquire)) {
        return;
    }
    const unsigned activations = g_providerActivations.load(std::memory_order_relaxed);
    if (activations != 0) {
        Wh_Log(L"[supervisor] watchdog ok: providers=%u attempt=%u", activations, g_watchdogAttempt);
        g_watchdogAttempt = 0;
        g_watchdogDelayServed = false;
        return;
    }
    if (g_watchdogAttempt >= 2) {
        Wh_Log(L"[supervisor] watchdog gave up after %u reloads; VAN stays as is for this session",
               g_watchdogAttempt);
        return;
    }
    if (g_watchdogAttempt == 1 && !g_watchdogDelayServed) {
        g_watchdogDelayServed = true;
        Wh_Log(L"[supervisor] watchdog: reload did not help; waiting %u ms before the last attempt",
               kWatchdogLateRetryDelayMs);
        ArmShowWatchdog(kWatchdogLateRetryDelayMs);
        return;
    }
    ++g_watchdogAttempt;
    Wh_Log(L"[supervisor] watchdog: no provider activated; reloading managed runtime (attempt %u)",
           g_watchdogAttempt);

    const bool cleanReload = ReloadManagedVanRuntimeOnSta();
    if (!cleanReload) {
        Wh_Log(L"[supervisor] van.dll could not be unloaded; refusing to call ShowVAN on a stopped image");
        g_watchdogAttempt = 2;
        return;
    }
    if (g_shuttingDown.load(std::memory_order_acquire)) {
        return;
    }
    g_providerActivations.store(0, std::memory_order_relaxed);
    InvokeShowVanOnStaThread(/*allowToggleClose=*/false);
    ArmShowWatchdog(kShowWatchdogDelayMs);
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
std::atomic<HWND> g_pnidTrayHwnd{nullptr};
std::atomic<UINT> g_pnidTrayCallbackMsg{0};

// Matches WindhawkUtils::WH_SUBCLASSPROC: (HWND, UINT, WPARAM, LPARAM,
// DWORD_PTR dwRefData). The wrapper removes the subclass on WM_NCDESTROY;
// explicit removal is also performed during normal teardown.
LRESULT CALLBACK PnidTrayWndSubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR /*dwRefData*/) try {
    if (uMsg == WM_NCDESTROY) {
        HWND expected = hWnd;
        if (g_pnidTrayHwnd.compare_exchange_strong(
                expected, nullptr, std::memory_order_acq_rel)) {
            g_pnidTrayCallbackMsg.store(0, std::memory_order_release);
        }
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    const UINT callbackMessage = g_pnidTrayCallbackMsg.load(std::memory_order_acquire);
    if (!g_shuttingDown.load(std::memory_order_acquire) && callbackMessage != 0 &&
        uMsg == callbackMessage) {
        const UINT notifyEvent = LOWORD(lParam);
        if (notifyEvent == WM_LBUTTONUP || notifyEvent == NIN_SELECT ||
            notifyEvent == NIN_KEYSELECT || notifyEvent == WM_LBUTTONDBLCLK) {
            RequestShowVanFlyout(/*allowToggleClose=*/true);
            return 0;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
} catch (...) {
    DisableModAfterCppException(L"pnidui subclass callback");
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void RemovePnidTraySubclass() {
    const HWND hWnd = g_pnidTrayHwnd.exchange(nullptr, std::memory_order_acq_rel);
    g_pnidTrayCallbackMsg.store(0, std::memory_order_release);
    if (!hWnd || !IsWindow(hWnd)) {
        return;
    }
    // This Windhawk helper returns void, not BOOL. It synchronously asks
    // the owning window thread to remove the subclass; there is no result
    // value to negate or use as a success/failure test.
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(
        hWnd, PnidTrayWndSubclassProc);
}

using Shell_NotifyIconW_t = BOOL(WINAPI*)(DWORD, PNOTIFYICONDATAW);
Shell_NotifyIconW_t g_origShell_NotifyIconW = nullptr;

BOOL WINAPI Hook_Shell_NotifyIconW(DWORD dwMessage, PNOTIFYICONDATAW lpData) {
    try {
        if (!g_inHookGuard && lpData && lpData->hWnd &&
            (dwMessage == NIM_ADD || dwMessage == NIM_MODIFY) &&
            (lpData->uFlags & NIF_MESSAGE) && lpData->uCallbackMessage != 0 &&
            g_vanStoreReady.load(std::memory_order_acquire) &&
            !g_shuttingDown.load(std::memory_order_acquire)) {
            const HWND hFallback = g_fallbackTrayHwnd.load(std::memory_order_acquire);
            if (lpData->hWnd != hFallback) {
                // pnidui.dll's CLSID_NetworkTray registers its callback window
                // from the STA worker thread of this process.
                const DWORD staTid = g_staThreadId.load(std::memory_order_acquire);
                if (staTid != 0 && GetCurrentThreadId() == staTid) {
                    ScopedHookGuard guard;
                    // Track the candidate before the helper: a throwing helper
                    // might already have installed a callback that needs teardown.
                    g_pnidTrayHwnd.store(lpData->hWnd, std::memory_order_release);
                    g_pnidTrayCallbackMsg.store(lpData->uCallbackMessage, std::memory_order_release);
                    if (WindhawkUtils::SetWindowSubclassFromAnyThread(
                            lpData->hWnd, PnidTrayWndSubclassProc, 0)) {
                        g_pnidTrayHwnd.store(lpData->hWnd, std::memory_order_release);
                    } else {
                        HWND expected = lpData->hWnd;
                        g_pnidTrayHwnd.compare_exchange_strong(
                            expected, nullptr, std::memory_order_acq_rel);
                        g_pnidTrayCallbackMsg.store(0, std::memory_order_release);
                        Wh_Log(L"Could not subclass the pnidui tray callback window");
                    }
                }
            }
        }
    } catch (...) {
        g_unloadSafetyFailure.store(true, std::memory_order_release);
        DisableModAfterCppException(L"Hook_Shell_NotifyIconW subclass setup");
    }
    try {
        return g_origShell_NotifyIconW(dwMessage, lpData);
    } catch (...) {
        g_unloadSafetyFailure.store(true, std::memory_order_release);
        DisableModAfterCppException(L"Hook_Shell_NotifyIconW original API");
        SetLastError(ERROR_GEN_FAILURE);
        return FALSE;
    }
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

LRESULT CALLBACK VanTrayMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) try {
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
} catch (...) {
    DisableModAfterCppException(L"tray mouse callback");
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
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

std::atomic<unsigned> g_wlanEnumLogCount{0};

DWORD WINAPI Hook_WlanEnumInterfaces(
    HANDLE hClientHandle, PVOID pReserved, PWLAN_INTERFACE_INFO_LIST* ppInterfaceList) {
    const DWORD err = g_origWlanEnumInterfaces(hClientHandle, pReserved, ppInterfaceList);
    const bool managedCaller = IsCallerManagedVanModule(__builtin_return_address(0));
    if (managedCaller && g_wlanEnumLogCount.fetch_add(1, std::memory_order_relaxed) < 8) {
        const DWORD count = err == ERROR_SUCCESS && ppInterfaceList && *ppInterfaceList
            ? (*ppInterfaceList)->dwNumberOfItems : 0;
        Wh_Log(L"Legacy WLAN enumeration: error=%lu, interfaces=%lu", err, count);
    }
    if (managedCaller && err == ERROR_SUCCESS && ppInterfaceList && *ppInterfaceList &&
        g_hideDuplicateWlanAdapters.load(std::memory_order_relaxed) &&
        (*ppInterfaceList)->dwNumberOfItems > 1) {
        (*ppInterfaceList)->dwNumberOfItems = 1;
        (*ppInterfaceList)->dwIndex = 0;
    }
    return err;
}

// Diagnostic-only: preserve the exact WLAN result and caller-owned buffer.
using WlanGetAvailableNetworkList_t = DWORD(WINAPI*)(
    HANDLE, const GUID*, DWORD, PVOID, PWLAN_AVAILABLE_NETWORK_LIST*);
WlanGetAvailableNetworkList_t g_origWlanGetAvailableNetworkList = nullptr;
std::atomic<unsigned> g_wlanNetworkLogCount{0};

DWORD WINAPI Hook_WlanGetAvailableNetworkList(
    HANDLE client, const GUID* interfaceGuid, DWORD flags, PVOID reserved,
    PWLAN_AVAILABLE_NETWORK_LIST* networks) {
    const DWORD error = g_origWlanGetAvailableNetworkList(
        client, interfaceGuid, flags, reserved, networks);
    if (IsCallerManagedVanModule(__builtin_return_address(0)) &&
        g_wlanNetworkLogCount.fetch_add(1, std::memory_order_relaxed) < 8) {
        const DWORD count = error == ERROR_SUCCESS && networks && *networks
            ? (*networks)->dwNumberOfItems : 0;
        Wh_Log(L"Legacy WLAN available networks: error=%lu, networks=%lu", error, count);
    }
    return error;
}

// --- NEW: WlanConnect hook (connection attempt tracing) ---
using WlanConnect_t = DWORD(WINAPI*)(HANDLE, const GUID*, const PWLAN_CONNECTION_PARAMETERS, PVOID);
WlanConnect_t g_origWlanConnect = nullptr;

DWORD WINAPI Hook_WlanConnect(
    HANDLE hClientHandle, const GUID* pInterfaceGuid,
    const PWLAN_CONNECTION_PARAMETERS pConnectionParameters, PVOID pReserved) {
    if (IsCallerManagedVanModule(__builtin_return_address(0))) {
        Wh_Log(L"[connect] WlanConnect ENTER: mode=%d, profile=%s",
               pConnectionParameters ? static_cast<int>(pConnectionParameters->wlanConnectionMode) : -1,
               (pConnectionParameters && pConnectionParameters->strProfile)
                   ? pConnectionParameters->strProfile : L"<null>");
    }
    const DWORD err = g_origWlanConnect(hClientHandle, pInterfaceGuid, pConnectionParameters, pReserved);
    if (IsCallerManagedVanModule(__builtin_return_address(0))) {
        Wh_Log(L"[connect] WlanConnect LEAVE: err=%lu (0=success)", err);
    }
    return err;
}
// --- END NEW ---
// ===========================================================================
// Smart fallback tray icon (live NLM status, hosted by the mod itself)
// ===========================================================================
constexpr wchar_t kFallbackTrayWndClass[] = L"WindhawkWin8VanFallbackTrayWnd";
constexpr UINT kFallbackTrayIconUid = 0x56414E32;  // 'VAN2'
constexpr GUID kFallbackTrayIconGuid = {
    0x6B1D8A44, 0x37C2, 0x4E20, {0x8F, 0x0D, 0x9A, 0x11, 0x08, 0x6A, 0x1F, 0x33}};

bool RegisterOwnedWindowClass(
    const wchar_t* className, HINSTANCE instance, WNDPROC windowProc) {
    if (!className || !instance || !windowProc) {
        return false;
    }

    WNDCLASSW wc{};
    wc.lpfnWndProc = windowProc;
    wc.hInstance = instance;
    wc.lpszClassName = className;
    if (RegisterClassW(&wc) != 0) {
        return true;
    }
    if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        return false;
    }

    // Never blindly reuse an existing class: after a bad unload its WndProc
    // could point into an unmapped mod image. Reuse only an exact registration.
    WNDCLASSW existing{};
    return GetClassInfoW(instance, className, &existing) &&
           existing.hInstance == instance && existing.lpfnWndProc == windowProc;
}

void UnregisterOwnedWindowClass(const wchar_t* className, HINSTANCE instance) {
    if (!className || !instance) {
        return;
    }
    if (UnregisterClassW(className, instance)) {
        return;
    }
    const DWORD error = GetLastError();
    if (error != ERROR_CLASS_DOES_NOT_EXIST) {
        g_unloadSafetyFailure.store(true, std::memory_order_release);
        Wh_Log(L"UnregisterClassW(%s) failed (%lu); the mod image must stay mapped",
               className, error);
    }
}

class FallbackTrayIcon {
  public:
    bool Start() {
        if (m_added) {
            return true;
        }

        HMODULE hMod = nullptr;
        if (!GetModuleHandleExW(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                reinterpret_cast<LPCWSTR>(&FallbackTrayIcon::WndProc), &hMod) ||
            !hMod) {
            Wh_Log(L"Could not resolve the mod instance for the fallback tray class");
            return false;
        }
        m_hInstance = hMod;
        if (!RegisterOwnedWindowClass(
                kFallbackTrayWndClass, m_hInstance, &FallbackTrayIcon::WndProc)) {
            Wh_Log(L"Fallback tray window class registration failed (%lu)", GetLastError());
            m_hInstance = nullptr;
            return false;
        }
        m_classRegistered = true;

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
        if (m_classRegistered) {
            UnregisterOwnedWindowClass(kFallbackTrayWndClass, m_hInstance);
            m_classRegistered = false;
            m_hInstance = nullptr;
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
        const wchar_t* tip = GetHydratedStringFor(L"pnidui.dll", stringId, GetEffectiveLanguageIndex());
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
        HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) try {
        if (!g_shuttingDown.load(std::memory_order_acquire) &&
            uMsg == WM_APP_FALLBACK_TRAY_CB) {
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
    } catch (...) {
    DisableModAfterCppException(L"fallback tray window callback");
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

    static void ShowContextMenu(HWND hWnd) {
        static constexpr const wchar_t* kOpenLabels[] = {
            L"Open network flyout", L"Apri riquadro reti", L"Abrir panel de redes",
            L"Ouvrir le volet Réseau", L"Abrir painel de rede",
        };
        static constexpr const wchar_t* kSettingsLabels[] = {
            L"Network settings", L"Impostazioni di rete", L"Configuración de red",
            L"Paramètres réseau", L"Configurações de rede",
        };
        const int languageIndex = GetEffectiveLanguageIndex();
        HMENU menu = CreatePopupMenu();
        AppendMenuW(menu, MF_STRING, 1, kOpenLabels[languageIndex]);
        AppendMenuW(menu, MF_STRING, 2, kSettingsLabels[languageIndex]);
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
    HINSTANCE m_hInstance = nullptr;
    bool m_classRegistered = false;
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

        ScopedComReference<IClassFactory> factoryOwner(pFactory);

        // pnidui 22621 has no IOleCommandTarget: activation goes through the
        // private CNwSSO interface (Start/Stop).
        INetworkTrayPrivate* pTray = nullptr;
        hr = pFactory->CreateInstance(
            nullptr, IID_NetworkTrayPrivate, reinterpret_cast<void**>(&pTray));
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
        // Publish ownership before foreign activation: if it throws, STA
        // cleanup can still see the object and context (or park on failure).
        m_trayObject = pTray;
        BYTE* base = reinterpret_cast<BYTE*>(m_trayContext);
        Wh_Log(L"Starting optional legacy pnidui tray object");
        hr = ObserveTrayActivate(pTray, base);
        if (FAILED(hr)) {
            Wh_Log(L"NetworkTray Start(4) returned 0x%08lX",
                   static_cast<unsigned long>(hr));
            LocalFree(m_trayContext);
            m_trayContext = nullptr;
            m_trayObject = nullptr;
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
        RemovePnidTraySubclass();

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
HMODULE g_staClassModule = nullptr;
bool g_staClassRegistered[2] = {false, false};
bool g_hotkeyRegistered = false;
int g_hostMissCount = 0;

void ReevaluateHostingOnSta();
void RefreshFlyoutDataIfClosed();

LRESULT CALLBACK VanStaWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) try {
    if (uMsg == WM_NCDESTROY) {
        if (g_staCtrlWnd == hWnd) {
            g_staCtrlWnd = nullptr;
        }
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    if (g_shuttingDown.load(std::memory_order_acquire))
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);

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
        if (wParam == kStaTimerFlyoutDataRefresh) {
            RefreshFlyoutDataIfClosed();
            return 0;
        }
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
} catch (...) {
    DisableModAfterCppException(L"STA window callback");
    return 0;
}

HWND CreateStaWindow(const wchar_t* className) {
    int classIndex = -1;
    if (_wcsicmp(className, kVanHostWndClassName) == 0) {
        classIndex = 0;
    } else if (_wcsicmp(className, kVanCtrlWndClassName) == 0) {
        classIndex = 1;
    }
    if (classIndex < 0) {
        Wh_Log(L"Refusing to create an STA window with an unknown class");
        return nullptr;
    }

    HMODULE hMod = nullptr;
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&VanStaWndProc), &hMod) ||
        !hMod) {
        Wh_Log(L"Could not resolve the mod instance for STA window class %s", className);
        return nullptr;
    }
    if (g_staClassModule && g_staClassModule != hMod) {
        Wh_Log(L"STA window class module changed unexpectedly");
        return nullptr;
    }

    if (!g_staClassRegistered[classIndex]) {
        if (!RegisterOwnedWindowClass(className, hMod, &VanStaWndProc)) {
            Wh_Log(L"STA window class registration failed for %s (%lu)",
                   className, GetLastError());
            return nullptr;
        }
        g_staClassModule = hMod;
        g_staClassRegistered[classIndex] = true;
    }

    // Message-only window: timers, hotkeys and IPC messages only.
    return CreateWindowExW(
        0, className, L"", WS_OVERLAPPED, 0, 0, 0, 0, HWND_MESSAGE, nullptr, hMod, nullptr);
}

void UnregisterStaWindowClasses() {
    if (!g_staClassModule) {
        return;
    }
    if (g_staClassRegistered[0]) {
        UnregisterOwnedWindowClass(kVanHostWndClassName, g_staClassModule);
        g_staClassRegistered[0] = false;
    }
    if (g_staClassRegistered[1]) {
        UnregisterOwnedWindowClass(kVanCtrlWndClassName, g_staClassModule);
        g_staClassRegistered[1] = false;
    }
    g_staClassModule = nullptr;
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
    const bool wantClassic = g_enableLegacyTrayIcon.load(std::memory_order_relaxed) &&
                             !g_nativeSystemVan.load(std::memory_order_acquire);
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

// Lightweight polling: refresh WLAN data when the flyout is closed so the
// available-network list stays populated (e.g. after Explorer restarts).
void EnterHostModeOnSta() {
    Wh_Log(L"This explorer.exe takes over as flyout/tray host (owns Shell_TrayWnd)");
    // Hard reset completo: quando Explorer si riavvia e questo processo diventa
    // il nuovo host del flyout, ricarica la mod da zero così riparte pulita
    // come se fosse appena installata.
    // Use the same full unload as the watchdog reload: ShutdownVAN, FreeLibrary
    // of van.dll and its providers, and the "still mapped" stale guard. A partial
    // reset here would leave a post-ShutdownVAN van.dll mapped and reachable.
    if (ReloadManagedVanRuntimeOnSta()) {
        // Image is gone: the next InvokeShowVanOnStaThread loads a fresh copy.
        g_vanRuntimeStale.store(false, std::memory_order_release);
    }
    // If van.dll is still mapped, ReloadManagedVanRuntimeOnSta has set
    // g_vanRuntimeStale and flyout requests stay refused until Explorer restarts.
    g_watchdogAttempt = 0;
    g_watchdogDelayServed = false;
    g_providerActivations.store(0, std::memory_order_relaxed);
    g_firstShowChecked.store(false, std::memory_order_release);

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
    SetTimer(g_staCtrlWnd, kStaTimerFlyoutDataRefresh, 30000, nullptr);

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
            KillTimer(g_staCtrlWnd, kStaTimerFlyoutDataRefresh);
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

struct WlanStartupObservation {
    DWORD error = ERROR_SUCCESS;
    DWORD interfaces = 0;
    DWORD readyInterfaces = 0;
};

bool IsWlanStartupUsable(const WlanStartupObservation& observation) noexcept {
    // A disconnected/radio-off interface is sufficient. Never require an
    // existing association, an IP address, or a nonempty list of nearby SSIDs.
    return observation.error == ERROR_SUCCESS && observation.readyInterfaces != 0;
}

WlanStartupObservation ProbeWlanStartup() {
    struct ProbeResources {
        HANDLE client = nullptr;
        PWLAN_INTERFACE_INFO_LIST list = nullptr;
        ~ProbeResources() noexcept {
            if (list) WlanFreeMemory(list);
            if (client) WlanCloseHandle(client, nullptr);
        }
    } resources;
    WlanStartupObservation observation;
    DWORD negotiated = 0;
    observation.error = WlanOpenHandle(2, nullptr, &negotiated, &resources.client);
    if (observation.error != ERROR_SUCCESS) return observation;
    observation.error = g_origWlanEnumInterfaces
        ? g_origWlanEnumInterfaces(resources.client, nullptr, &resources.list)
        : WlanEnumInterfaces(resources.client, nullptr, &resources.list);
    if (observation.error != ERROR_SUCCESS || !resources.list) return observation;
    observation.interfaces = resources.list->dwNumberOfItems;
    for (DWORD i = 0; i < observation.interfaces; ++i)
        if (resources.list->InterfaceInfo[i].isState != wlan_interface_state_not_ready)
            ++observation.readyInterfaces;
    return observation;
}

void RefreshFlyoutDataIfClosed() {
    // Only touch WLAN state when the flyout is closed: this keeps the
    // available-network list fresh without interfering with saved-network
    // connections or an open credential prompt.
    if (IsVanFlyoutWindowVisible()) {
        return;
    }
    const auto obs = ProbeWlanStartup();
    if (obs.error != ERROR_SUCCESS) {
        Wh_Log(L"[flyout data refresh] WLAN probe failed: error=%lu, "
               L"interfaces=%u, ready=%u",
               obs.error, obs.interfaces, obs.readyInterfaces);
    }
}

bool WaitForWlanStartupOnSta() {
    // Retry the public readiness query, not VAN activation. No extra thread,
    // service restart, fake connection, or repeated ShutdownVAN cycle.
    unsigned consecutiveReady = 0;
    DWORD lastError = static_cast<DWORD>(-1);
    DWORD lastCount = static_cast<DWORD>(-1);
    DWORD lastReady = static_cast<DWORD>(-1);
    for (unsigned attempt = 0; attempt < 30; ++attempt) {
        if (g_shuttingDown.load(std::memory_order_acquire)) return false;
        const WlanStartupObservation sample = ProbeWlanStartup();
        if (sample.error != lastError || sample.interfaces != lastCount ||
            sample.readyInterfaces != lastReady) {
            Wh_Log(L"WLAN startup probe: error=%lu, interfaces=%lu, ready=%lu",
                   sample.error, sample.interfaces, sample.readyInterfaces);
            lastError = sample.error; lastCount = sample.interfaces; lastReady = sample.readyInterfaces;
        }
        consecutiveReady = IsWlanStartupUsable(sample) ? consecutiveReady + 1 : 0;
        if (consecutiveReady >= 2) {
            Wh_Log(L"WLAN startup ready; allowing first legacy runtime activation");
            return true;
        }
        const DWORD wait = WaitForSingleObject(g_stopEvent, 500);
        if (wait == WAIT_OBJECT_0 || g_shuttingDown.load(std::memory_order_acquire)) return false;
        if (wait == WAIT_FAILED) {
            Wh_Log(L"WLAN startup wait failed (%lu); stopping startup", GetLastError());
            return false;
        }
    }
    // Non-WLAN systems and disabled adapters must still be able to show wired
    // connections. A budget expiry is diagnostic, never invented Wi-Fi state.
    Wh_Log(L"WLAN readiness retry budget exhausted; proceeding without assuming Wi-Fi exists");
    return !g_shuttingDown.load(std::memory_order_acquire);
}

void RunVanStaWorkerBody() {
    MSG dummyMsg{};
    PeekMessageW(&dummyMsg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    // 1. Prefer a compatible System32 VAN stack; only download/hydrate MSDL
    //    components when the native capability probe rejected this build.
    const bool nativeMode = g_nativeSystemVan.load(std::memory_order_acquire);
    const bool provisioned = nativeMode || EnsureVanStoreProvisioned();
    if ((!provisioned) ||
        g_shuttingDown.load(std::memory_order_acquire)) {
        Wh_Log(L"VAN runtime not ready - flyout restorer staying passive this session");
        return;
    }

    // 2. Publish all full paths and the STA thread id before enabling hooks.
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        for (size_t i = 0; i < kRuntimeDllCount; ++i) {
            const std::filesystem::path full = g_vanStoreDir / kRuntimeDllNames[i];
            const std::wstring s = full.wstring();
            const size_t n = (std::min<size_t>)(s.size(), 599);
            memcpy(g_managedFullPaths[i], s.c_str(), n * sizeof(wchar_t));
            g_managedFullPaths[i][n] = L'\0';
        }
        g_staThreadId.store(GetCurrentThreadId(), std::memory_order_release);
    }
    if (nativeMode) {
        Wh_Log(L"Native System32 VAN runtime ready at %s", g_vanStoreDir.c_str());
    } else {
        Wh_Log(L"Verified VanStore ready at %s", g_vanStoreDir.c_str());
    }

    // 3. The downloaded route uses a private application hive. Native
    //    ReplaceVan mode leaves registry state unchanged and only virtualizes
    //    the exact RegGetValueW result in-process.
    if (!nativeMode) {
        if (g_hkcuNtPrefix.empty()) {
            Wh_Log(L"HKCU NT path is unavailable; refusing fallback activation to keep "
                   L"registry redirection fail-closed");
            g_staThreadId.store(0, std::memory_order_release);
            return;
        }
        if (!OpenPrivateRegistryHive()) {
            Wh_Log(L"Private application hive could not be opened; staying passive");
            g_staThreadId.store(0, std::memory_order_release);
            return;
        }
        if (!PopulatePrivateRegistryHive()) {
            Wh_Log(L"Private registry hive population failed; staying passive; this instance's "
                   L"hive file will be removed on normal unload");
            g_staThreadId.store(0, std::memory_order_release);
            return;
        }
        g_privateRegistryHiveReady.store(true, std::memory_order_release);
    }
    if (!WaitForWlanStartupOnSta() || g_shuttingDown.load(std::memory_order_acquire)) {
        Wh_Log(L"WLAN startup cancelled; legacy runtime was not activated");
        return;
    }
    g_vanStoreReady.store(true, std::memory_order_release);
    Wh_Log(L"Managed runtime enabled; classic tray requested=%d (disable it for crash isolation)",
           g_enableLegacyTrayIcon.load(std::memory_order_relaxed) ? 1 : 0);

    // 4. Managed images are loaded by absolute path with
    // LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32.
    // Do not add a private legacy directory to Explorer's process-wide DLL
    // search list: unrelated shell extensions must not resolve DLLs from it.
    Wh_Log(L"Private DLL resolution enabled without process-wide AddDllDirectory");

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
        if (waitRes == WAIT_FAILED) {
            Wh_Log(L"STA wait failed (%lu); disabling mod for this session", GetLastError());
            g_shuttingDown.store(true, std::memory_order_release);
            break;
        }
        if (waitRes == WAIT_OBJECT_0 || g_shuttingDown.load(std::memory_order_acquire)) {
            break;
        }
        if (waitRes == WAIT_OBJECT_0 + 1) {
            MSG msg{};
            while (!g_shuttingDown.load(std::memory_order_acquire) &&
                   PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
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
                if (msg.message == WM_TIMER && msg.hwnd == nullptr &&
                    g_showWatchdogTimerId != 0 && msg.wParam == g_showWatchdogTimerId) {
                    KillTimer(nullptr, g_showWatchdogTimerId);
                    g_showWatchdogTimerId = 0;
                    EvaluateShowWatchdogOnSta();
                    continue;
                }
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }

}

void CleanupVanStaOnOwningThread() {
    // 7. Deterministic shutdown (all on the owning STA thread):
    //    tray first, then the VAN UI thread via van.dll's own ShutdownVAN.
    if (g_showWatchdogTimerId != 0) {
        KillTimer(nullptr, g_showWatchdogTimerId);
        g_showWatchdogTimerId = 0;
    }
    RemoveTrayMouseHook();
    if (g_hotkeyRegistered && g_staCtrlWnd) {
        UnregisterHotKey(g_staCtrlWnd, kHotkeyId);
        g_hotkeyRegistered = false;
    }
    g_trayHostController.Stop();
    g_fallbackTrayIcon.Stop();
    RemovePnidTraySubclass();
    if (g_staCtrlWnd) {
        KillTimer(g_staCtrlWnd, kStaTimerHostReeval);
        KillTimer(g_staCtrlWnd, kStaTimerFallbackRefresh);
        KillTimer(g_staCtrlWnd, kStaTimerFlyoutDataRefresh);
        DestroyWindow(g_staCtrlWnd);
        g_staCtrlWnd = nullptr;
    }
    BoundedShutdownVan();
    UnregisterStaWindowClasses();
    g_hostRole.store(static_cast<int>(HostRole::Undetermined), std::memory_order_release);
    g_vanStoreReady.store(false, std::memory_order_release);
    g_privateRegistryHiveReady.store(false, std::memory_order_release);
    g_staThreadId.store(0, std::memory_order_release);

}

DWORD WINAPI VanStaWorkerThreadProc(LPVOID) noexcept {
    ScopedStaComApartment apartment;
    if (FAILED(apartment.result())) {
        Wh_Log(L"STA COM initialization failed (0x%08lX); panel remains inactive",
               static_cast<unsigned long>(apartment.result()));
        return 0;
    }
    Wh_Log(L"STA worker initialized with C++ failure containment");
    try {
        RunVanStaWorkerBody();
    } catch (const std::exception&) {
        DisableModAfterCppException(L"STA worker (std::exception)");
    } catch (...) {
        DisableModAfterCppException(L"STA worker (unknown C++ exception)");
    }
    try {
        CleanupVanStaOnOwningThread();
        if (g_unloadSafetyFailure.load(std::memory_order_acquire)) {
            // Diagnostic only. The worker must always reach return 0 so the
            // unconditional join in Wh_ModUninit can complete. Resources that
            // could not be released cleanly are left to process teardown.
            Wh_Log(L"Incomplete callback/COM teardown reported; STA worker exits anyway "
                   L"(unreleased resources are left to process teardown)");
        }
    } catch (...) {
        DisableModAfterCppException(L"STA teardown");
        g_unloadSafetyFailure.store(true, std::memory_order_release);
        Wh_Log(L"STA cleanup threw; STA worker exits anyway so unload can complete. "
               L"Restart Explorer before using this mod again.");
    }
    return 0;
}
}


// ===========================================================================
// Windhawk lifecycle
// ===========================================================================
BOOL Wh_ModInit() try {
    Wh_Log(L"Initializing Windows 8 Network Flyout Restorer v1.0.0...");
    Wh_Log(L"Flyout code revision: wifi-dependency-ownership-r3; COM gate tracing enabled");

    // Install the required write guard before allocating mod-owned resources.
    HMODULE registryNtdll = GetModuleHandleW(L"ntdll.dll");
    const auto setValue = registryNtdll
        ? GetProcAddress(registryNtdll, "NtSetValueKey") : nullptr;
    if (!setValue || !Wh_SetFunctionHook(
            reinterpret_cast<void*>(setValue), reinterpret_cast<void*>(Hook_NtSetValueKey),
            reinterpret_cast<void**>(&g_origNtSetValueKey))) {
        Wh_Log(L"Required registry write protection could not be installed");
        return FALSE;
    }

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
        g_nativeSystemDir.clear();
    }

    g_vanStoreReady.store(false, std::memory_order_release);
    g_privateRegistryHiveReady.store(false, std::memory_order_release);
    g_workerStillRunning.store(false, std::memory_order_release);
    g_unloadSafetyFailure.store(false, std::memory_order_release);
    g_pnidTrayHwnd.store(nullptr, std::memory_order_release);
    g_pnidTrayCallbackMsg.store(0, std::memory_order_release);
    g_wlanEnumLogCount.store(0, std::memory_order_relaxed);
    g_wlanNetworkLogCount.store(0, std::memory_order_relaxed);
    g_providerComTraceCount.store(0, std::memory_order_relaxed);
    g_providerActivations.store(0, std::memory_order_relaxed);
    g_firstShowChecked.store(false, std::memory_order_relaxed);
    g_showWatchdogRetryDone.store(false, std::memory_order_relaxed);
    g_showWatchdogTimerId = 0;
    g_privateHiveWriteFailures.store(0, std::memory_order_relaxed);
    g_privateHiveWriteFailureLogs.store(0, std::memory_order_relaxed);
    g_controlVanOpenLogs.store(0, std::memory_order_relaxed);

    std::filesystem::path nativeSystemDir;
    DWORD nativeBuild = 0;
    if (DetectNativeVanSupport(&nativeSystemDir, &nativeBuild)) {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        g_nativeSystemDir = nativeSystemDir;
        g_vanStoreDir = nativeSystemDir;
        g_nativeSystemVan.store(true, std::memory_order_release);
        Wh_Log(L"Using native System32 VAN components on build %lu; downloads will be skipped",
               static_cast<unsigned long>(nativeBuild));
    } else {
        g_nativeSystemVan.store(false, std::memory_order_release);
        Wh_Log(L"Native VAN capability probe failed; verified MSDL fallback will be used");
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

    if (!g_nativeSystemVan.load(std::memory_order_acquire)) {
        // Cross-explorer provisioning mutex (abandoned instances are recovered).
        g_provisionMutex = CreateMutexW(nullptr, FALSE, kProvisionMutexName);
        if (!g_provisionMutex) {
            Wh_Log(L"CreateMutexW(provision) failed (%lu); refusing unsafe fallback init",
                   GetLastError());
            CloseHandle(g_stopEvent);
            g_stopEvent = nullptr;
            return FALSE;
        }
    }
    // Resolve ntdll helpers + capture the NT path of HKCU (e.g.
    // "\Registry\User\S-1-5-21-...") for matching the per-user Classes path.
    // Redirected keys themselves are opened relative to the private hive handle.
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
                RegOpenKeyExW(HKEY_CURRENT_USER, L"Software", 0, KEY_QUERY_VALUE, &hKey);
            if (openRes != ERROR_SUCCESS) {
                Wh_Log(L"Prefix capture: RegOpenKeyExW(HKCU\\Software) failed with %ld", openRes);
            } else {
                BYTE buf[1024];
                ULONG returned = 0;
                // Query a real child key handle, then trim the known suffix to
                // recover the HKCU root path. A null subkey can return a pseudo-handle
                // that NtQueryKey rejects with STATUS_INVALID_HANDLE.
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
                        static constexpr wchar_t kSoftwareSuffix[] = L"\\Software";
                        constexpr size_t kSuffixLength = ARRAYSIZE(kSoftwareSuffix) - 1;
                        if (g_hkcuNtPrefix.size() >= kSuffixLength &&
                            _wcsicmp(g_hkcuNtPrefix.c_str() +
                                         g_hkcuNtPrefix.size() - kSuffixLength,
                                     kSoftwareSuffix) == 0) {
                            g_hkcuNtPrefix.resize(g_hkcuNtPrefix.size() - kSuffixLength);
                        } else {
                            g_hkcuNtPrefix.clear();
                            Wh_Log(L"Prefix capture: HKCU\\Software suffix was unexpected");
                        }
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
        RegCreateKeyExW_t pRegCreateW = nullptr;
        RegCreateKeyExA_t pRegCreateA = nullptr;
        if (hKernelBase) {
            pRegOpenW = ResolveExportAddress<RegOpenKeyExW_t>(hKernelBase, "RegOpenKeyExW");
            pRegOpenA = ResolveExportAddress<RegOpenKeyExA_t>(hKernelBase, "RegOpenKeyExA");
            pRegCreateW =
                ResolveExportAddress<RegCreateKeyExW_t>(hKernelBase, "RegCreateKeyExW");
            pRegCreateA =
                ResolveExportAddress<RegCreateKeyExA_t>(hKernelBase, "RegCreateKeyExA");
        }
        if (!pRegOpenW && hAdvapi32) {
            pRegOpenW = ResolveExportAddress<RegOpenKeyExW_t>(hAdvapi32, "RegOpenKeyExW");
        }
        if (!pRegOpenA && hAdvapi32) {
            pRegOpenA = ResolveExportAddress<RegOpenKeyExA_t>(hAdvapi32, "RegOpenKeyExA");
        }
        if (!pRegCreateW && hAdvapi32) {
            pRegCreateW = ResolveExportAddress<RegCreateKeyExW_t>(hAdvapi32, "RegCreateKeyExW");
        }
        if (!pRegCreateA && hAdvapi32) {
            pRegCreateA = ResolveExportAddress<RegCreateKeyExA_t>(hAdvapi32, "RegCreateKeyExA");
        }
        if (pRegOpenW) {
            WindhawkUtils::SetFunctionHook(
                pRegOpenW, Hook_RegOpenKeyExW, &g_origRegOpenKeyExW);
        }
        if (pRegOpenA) {
            WindhawkUtils::SetFunctionHook(
                pRegOpenA, Hook_RegOpenKeyExA, &g_origRegOpenKeyExA);
        }
        if (pRegCreateW) {
            WindhawkUtils::SetFunctionHook(
                pRegCreateW, Hook_RegCreateKeyExW, &g_origRegCreateKeyExW);
        }
        if (pRegCreateA) {
            WindhawkUtils::SetFunctionHook(
                pRegCreateA, Hook_RegCreateKeyExA, &g_origRegCreateKeyExA);
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
        NtCreateKey_t pNtCreateKey = reinterpret_cast<NtCreateKey_t>(
            reinterpret_cast<void*>(GetProcAddress(hNtdll, "NtCreateKey")));
        if (pNtCreateKey) {
            WindhawkUtils::SetFunctionHook(
                pNtCreateKey, Hook_NtCreateKey, &g_origNtCreateKey);
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
        const auto availableNetworks = ResolveExportAddress<WlanGetAvailableNetworkList_t>(
            hWlanApi, "WlanGetAvailableNetworkList");
        if (availableNetworks) {
            WindhawkUtils::SetFunctionHook(availableNetworks,
                Hook_WlanGetAvailableNetworkList, &g_origWlanGetAvailableNetworkList);
        }
        const auto wlanConnect = ResolveExportAddress<WlanConnect_t>(hWlanApi, "WlanConnect");
        if (wlanConnect) {
            WindhawkUtils::SetFunctionHook(wlanConnect, Hook_WlanConnect, &g_origWlanConnect);
        }
    }

    Wh_Log(L"All hooks installed; starting the STA worker");
    return TRUE;
} catch (...) {
    DisableModAfterCppException(L"initialization");
    // No worker has started: pending hooks are discarded by Windhawk after FALSE.
    if (g_provisionMutex) { CloseHandle(g_provisionMutex); g_provisionMutex = nullptr; }
    if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }
    return FALSE;
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

void Wh_ModSettingsChanged() try {
    const int previousLanguage = GetEffectiveLanguageIndex();
    LoadModSettings();
    const int currentLanguage = GetEffectiveLanguageIndex();

    const DWORD tid = g_staThreadId.load(std::memory_order_acquire);
    if (tid != 0) {
        PostThreadMessageW(tid, WM_APP_SYNC_TRAY, 0, 0);
    }

    if (previousLanguage != currentLanguage &&
        !g_nativeSystemVan.load(std::memory_order_acquire)) {
        // The live fallback tray labels switch immediately. Downloaded
        // resources are rebuilt for the new language after Explorer restarts.
        std::error_code ec;
        std::filesystem::remove(g_vanStoreBaseDir / kHydrationStampName, ec);
        Wh_Log(L"Flyout language changed to %s - hydration stamp invalidated",
               GetLanguageCode(currentLanguage));
    }
} catch (const std::filesystem::filesystem_error& error) {
    Wh_Log(L"C++ filesystem exception applying settings (code %d)", error.code().value());
    DisableModAfterCppException(L"settings update");
} catch (...) {
    DisableModAfterCppException(L"settings update");
}
void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Windows 8 Network Flyout Restorer...");

    // 1. Signal shutdown to every layer. Hooks check g_shuttingDown before
    //    dispatching, the STA pump exits on g_stopEvent/WM_QUIT, and any
    //    in-flight MSDL transfer is aborted immediately.
    g_shuttingDown.store(true, std::memory_order_release);
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    const DWORD tid = g_staThreadId.load(std::memory_order_acquire);
    if (tid != 0) {
        PostThreadMessageW(tid, WM_QUIT, 0, 0);
    }
    AbortWinHttpSession();

    // 2. Wait for the STA worker unconditionally. The worker is designed to
    //    always terminate: ShutdownVAN runs on a disposable helper thread with
    //    a bounded wait, and the STA thread never parks. There is therefore no
    //    timeout bail-out, no image pin, and no leaked reference here.
    if (g_workerThread) {
        const DWORD workerWait = WaitForSingleObject(g_workerThread, INFINITE);
        if (workerWait != WAIT_OBJECT_0) {
            Wh_Log(L"CRITICAL: WaitForSingleObject on the STA worker failed (%lu)",
                   GetLastError());
        }
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    // 3. Teardown note: g_unloadSafetyFailure is only a diagnostic here. It
    //    must never translate into a pin, an extra reference, or an infinite
    //    sleep: those would make the mod permanently un-unloadable, which is
    //    exactly what the unloadability contract forbids. Any resource that
    //    could not be released cleanly is left to process teardown.
    if (g_unloadSafetyFailure.load(std::memory_order_acquire)) {
        Wh_Log(L"Window callback/COM teardown reported incomplete; continuing normal "
               L"unload (unreleased resources are left to process teardown)");
    }

    // 4. Remove the DLL directory cookie, if any. No-op when never set.
    if (g_dllDirectoryCookie) {
        RemoveDllDirectory(g_dllDirectoryCookie);
        g_dllDirectoryCookie = nullptr;
    }

    // 5. Close the cross-process coordination handles. The private application
    //    hive file is mod data; unmount it after all managed modules are released.
    //    No live HKCU/HKLM registry keys are created or removed by teardown.
    if (g_provisionMutex) {
        CloseHandle(g_provisionMutex);
        g_provisionMutex = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }

    // 6. Drop the resolved van.dll exports. The module itself is released
    //    below through the balanced runtime reference list.
    g_pfnShowVAN = nullptr;
    g_pfnHideVAN = nullptr;
    g_pfnShutdownVAN = nullptr;
    g_vanModule = nullptr;

    // 7. Release every managed runtime module and every verified-file handle
    //    under the hook guard, so the redirection hooks do not observe
    //    half-torn state while FreeLibrary/CloseHandle run.
    {
        ScopedHookGuard guard;
        std::vector<HMODULE> modulesToFree;
        std::vector<HANDLE> verifiedFilesToClose;
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            modulesToFree.swap(g_loadedVanModules);
            verifiedFilesToClose.swap(g_verifiedRuntimeFileHandles);
        }
        for (size_t i = 0; i < kRuntimeDllCount; ++i) {
            g_managedImageBegin[i].store(0, std::memory_order_release);
            g_managedImageEnd[i].store(0, std::memory_order_relaxed);
        }
        for (auto it = modulesToFree.rbegin(); it != modulesToFree.rend(); ++it) {
            if (*it) {
                FreeLibrary(*it);
            }
        }
        for (HANDLE handle : verifiedFilesToClose) {
            if (handle && handle != INVALID_HANDLE_VALUE) {
                CloseHandle(handle);
            }
        }
    }

    // Stop redirection before unmounting the isolated application hive.
    g_privateRegistryHiveReady.store(false, std::memory_order_release);
    if (g_privateRegistryHive) {
        const LSTATUS closeStatus = RegCloseKey(g_privateRegistryHive);
        g_privateRegistryHive = nullptr;
        if (closeStatus != ERROR_SUCCESS) {
            Wh_Log(L"RegCloseKey for the private application hive returned %ld",
                   static_cast<long>(closeStatus));
        }
    }
    if (!g_privateRegistryHiveFile.empty()) {
        std::error_code ec;
        std::filesystem::remove(g_privateRegistryHiveFile, ec);
        if (ec) {
            Wh_Log(L"Could not remove this instance's private-hive file (code %d)", ec.value());
        }
        g_privateRegistryHiveFile.clear();
    }

    // Publish the final "not ready" state so any late hook entry fails closed.
    g_vanStoreReady.store(false, std::memory_order_release);
    Wh_Log(L"Windows 8 Network Flyout Restorer unloaded cleanly");
}
