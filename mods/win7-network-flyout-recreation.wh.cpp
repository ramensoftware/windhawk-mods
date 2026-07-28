// ==WindhawkMod==
// @id             win7-network-flyout-recreation
// @name           Windows 7 Network Flyout Recreation
// @description    This mod recreates the Windows 7 network flyout for Windows 10 and 11 including the Network Sharing Center Control Panel page
// @version        4.0.0
// @author         babamohammed
// @github         https://github.com/babamohammed2022
// @include        explorer.exe
// @include        control.exe
// @architecture   x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lgdi32 -ldwmapi -luxtheme -lole32 -lshell32 -luser32 -lcomctl32 -liphlpapi -lwlanapi -luuid -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7 Network Flyout Recreation

This mod recreates the classic Windows 7 network flyout on Windows 10 and 11, replacing the modern flyout with an accurate recreation of the familiar, lightweight alternative from Windows 7.

Screenshot of the light theme:

![Screenshot](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/networkflyoutwin7.PNG)

Screenshot of the dark theme:

![Screenshot](https://raw.githubusercontent.com/babamohammed2022/gtasashtml/main/dark.png)

Screenshot of the restored Control Panel Network Sharing Center:

![Screenshot](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/controlpanelpage.png)

The mod has been tested on Windows 10 21H2, Windows 10 1809, Windows 11 23H2, Windows 11 24H2 and Windows 11 25H2.


## Features
- **Wi-Fi network list**: Shows all available networks with live signal strength
- **Connect/Disconnect**: Connect to secured and open networks with password support
- **Privacy mode**: Hide real network names (shows as Network 1, Network 2...)
- **Classic tooltips**: Full network info on hover (SSID, signal, security type)
- **Right-click context menu**: Quick access to network status and properties
- **Keyboard navigation**: Full Arrow keys, Enter, and Escape support
- **Auto-refresh**: Periodically refreshes the network list at a configurable interval
- **Language support**: English, Italian, Spanish, French, Russian, German, Portuguese, Polish, Dutch, Romanian or auto-detect
- **DPI aware**: Scales correctly on high-DPI and mixed-DPI setups
- **Rounded corners**: Optional modern look for Windows 11 or Aero theme
- **Dual Theme Support**: Includes both light and dark themes, with the dark theme created specifically for late-night use and, if present, dark Aero theme.
- **Ethernet Support**: The mod should now properly show the flyout for Ethernet connection.
- **Classic Network Center links**: Optionally restores the Windows 7 “Connect to a network” and HomeGroup/sharing links with their custom artwork.
- **Restored classic Home/Public/Work network location icons**: The location icon shown in the flyout now matches the type of network (Public, Home, Work) and can be configured in the mod's options. 
- **Restored classic network map**: Restores the Windows 7-style visual map in the Network and Sharing Center, with PC/network/Internet icons, connection lines, and Home/Public/Work location icons.

## Requirements
- **Windows 10** with the native taskbar
- **Windows 11** with the Windows 10 taskbar (via [ExplorerPatcher](https://github.com/valinet/ExplorerPatcher) or similar mods)
- The network icon must be visible in the main system tray (overflow menu not supported)

**Note:** This mod is unlikely to work with some taskbar mods (e.g. Retrobar because they don't use the ToolbarWindow32) or heavily customized and unstable configurations.

## Known limitations
- **Overflow menu**: The network icon must be in the main system tray, not hidden in the overflow menu.
- **Auto-reconnect checkbox**: May not work reliably on all setups. If the network doesn't reconnect automatically, try connecting manually.
- **Control Panel refresh**: Live refresh is now driven by `INetworkListManagerEvents`, so the Network Map section normally updates automatically on connectivity changes. In rare cases (e.g. very rapid transitions) it may lag slightly behind; closing and re-opening the Control Panel window always shows the current state.

## Hotkeys
| Key | Action |
|-----|--------|
| **Ctrl+H** | Toggle network flyout (disabled by default) |

## Credits
- **m417z** - Code review
- **Anixx** - Testing on Windows 11 23H2 and providing feedback
- **sebastian08dm08-cpu** - Testing on Windows 10 1809
- **Kichura** - Visual analysis

If you encounter issues, please report them to the author of the mod.
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- language: auto
  $name: Language
  $description: User interface language, follows your system language by default
  $options:
    - auto: Auto-detect
    - en: English
    - it: Italiano
    - es: Español
    - fr: Français
    - ru: Русский
    - de: Deutsch
    - pt: Português
    - pl: Polski
    - nl: Nederlands
    - ro: Română
- interceptNativeFlyout: true
  $name: Intercept system network flyout
  $description: When you click the network icon in the tray, show this classic flyout instead of the Windows one. Requires the Windows 10 taskbar (native on Win10, or via ExplorerPatcher on Win11).
- privacyMode: false
  $name: Privacy mode
  $description: Hide real network names so all networks are shown as "Network 1", "Network 2", etc.
- refreshInterval: 3000
  $name: Auto-refresh interval (milliseconds)
  $description: How often to refresh the network list automatically. Set to 0 to disable auto-refresh. Minimum 1000 ms if enabled.
- enableHotkey: false
  $name: Enable Ctrl+H hotkey
  $description: Press Ctrl+H from anywhere to toggle the network flyout. Disabled by default to avoid conflicts with browser and editor shortcuts.
- useRoundedCorners: true
  $name: Rounded corners
  $description: Give the flyout window rounded corners, matching the look of the original Windows 7 flyout. Enabled by default since Windows 7 itself used rounded corners. Disable this for a more strictly classic/square theme look.
- restoreClassicNetworkCenterLinks: true
  $name: Restore Windows 7 Network Center layout
  $description: This setting replaces the layout of the current Network and Sharing Center page in the Control Panel with an accurate recreation of the Windows 7 one, featuring the classic links "Connect to a network" and "HomeGroup" along with the visual network map with PC, network, and Internet icons.
- useNetworkLocationIcons: true
  $name: Network location icons (Home / Public / Work)
  $description: Show the classic Windows 7 network location icon in the flyout header based on the active network profile (house = Home, bench = Public, buildings = Work). Disable to restore the original generic network icon.
- theme: light
  $name: Theme
  $description: Select the network flyout's theme
  $options:
    - light: Light (Classic Windows 7)
    - dark: Dark (Custom)
*/
// ==/WindhawkModSettings==
// ## Changelog
// - 4.0.0: Enhanced the Network Sharing center Control Panel page
// - 4.0.0: Removed legacy EnumWindows-based refresh (INetworkListManagerEvents now drives live updates).
// - 4.0.0: Control Panel Network Map layout refinements and privacy masking.
// - 3.4.0: van.dll alignment: row height 30rp + 24rp name + padding
//   rect(8rp,3rp,10rp,3rp), signal icon re-centered (matches the
//   real Windows 7 van.dll UIFILE). Refresh button, footer link
//   alignment, separator and all hover effects left unchanged.
// - 3.4.0: Network location detection now only runs while the flyout is
//   visible (it was previously re-run on every 3s auto-refresh tick even
//   while hidden), and the last detected category is kept instead of reset
//   while hidden, avoiding a generic-icon flash on reopen.
// - 3.4.0: Category detection now joins on the exact network GUID
//   (adapter -> INetwork::GetNetworkId() -> NetworkList\Profiles\{GUID})
//   instead of matching on profile display name, which isn't a unique key
//   and could pick a stale profile. This also fixes Wi-Fi being checked
//   before Ethernet when the Ethernet registry lookup missed.
// - 3.4.0: Header network-location icon is now decoded at its actual draw
//   size instead of being upscaled ~5%, removing a slight blur.
// - 3.4.0: Normal (non-hover) refresh icon is now decoded at ScaleDpi(16)
//   instead of a fixed 16px, so it no longer jumps disproportionately
//   relative to the hover icon at higher DPI.
// - 3.4.0: Registry profile name reads now use RegGetValueW, which
//   guarantees null termination, instead of RegQueryValueExW.
// - 3.4.0: The fallback network scan now fetches the adapter table once per
//   scan instead of once per connection examined.
// - 3.4.0: Corrected Public vs Work icon artwork mapping: Public now uses
//   the public/bench-style icon, while Domain/Work uses the buildings icon.
// - 3.4.0: Moved the refresh button 2.5% back to the right from the previous
//   release (net offset: 0.5% left from the original position).
// - 3.4.0: Prefer the exact registry profile category before NLM adapter
//   category, so a Public profile stays Public even if NLM reports another
//   connected/domain network elsewhere.
// - 3.4.0: Hardened network location icon detection by matching the exact
//   active adapter and registry profile before falling back, preventing a
//   domain/work network from overriding a Public profile.
// - 3.4.0: Moved the refresh button 3% further left in both light and dark themes.
// - 3.4.0: Removed external artwork credit wording; icons are treated as classic Windows 7-style assets.
// - 3.4.0: Integrated cleaned classic PNG assets for the refresh button and
//   Home/Public/Work network location icons, embedded safely as Base64.
// - 3.4.0: Saved source as UTF-8 and added a padding-safe Base64 decoder so
//   settings names such as Español, Français, Русский and Português render correctly.
// - 3.4.0: Added Ethernet support so the flyout now shows properly for
//   Ethernet connections, not just Wi-Fi.
// - 3.4.0: Added the option to restore classic Windows 7 "Connect to a
//   network" and HomeGroup/sharing links in the Network and Sharing Center.
// - 3.1.0: Earlier maintenance release predating this changelog's detailed
//   entries; history prior to 3.1.0 was not preserved.
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <windowsx.h>
#include <iphlpapi.h>
#include <ipifcons.h>
#include <wlanapi.h>
#include <objbase.h>
#include <uxtheme.h>
#include <dwmapi.h>
#include <psapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <shellapi.h>
#include <commctrl.h>
#include <windhawk_api.h>
#include <netlistmgr.h>
#include <windhawk_utils.h>
#include <process.h>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>
#include <stdlib.h>

// =========================================================
// Dark context menu support (right-click menu only)
#define WINDOW_WIDTH_BASE        300
#define WINDOW_HEIGHT_BASE       405
#define HEADER_HEIGHT_BASE       105
#define FOOTER_HEIGHT_BASE       60
#define ROW_HEIGHT_NORMAL_BASE   30
#define ROW_HEIGHT_EXPANDED_BASE 74

static UINT g_dpi = 96;
static int  WINDOW_WIDTH        = WINDOW_WIDTH_BASE;
static int  WINDOW_HEIGHT       = WINDOW_HEIGHT_BASE;
static int  HEADER_HEIGHT       = HEADER_HEIGHT_BASE;
static int  FOOTER_HEIGHT       = FOOTER_HEIGHT_BASE;
static int  LIST_Y_START        = HEADER_HEIGHT_BASE + 1;
static int  LIST_Y_END          = WINDOW_HEIGHT_BASE - FOOTER_HEIGHT_BASE;
static int  WIFI_LABEL_Y        = HEADER_HEIGHT_BASE - 24;
static int  ROW_HEIGHT_NORMAL   = ROW_HEIGHT_NORMAL_BASE;
static int  ROW_HEIGHT_EXPANDED = ROW_HEIGHT_EXPANDED_BASE;

static inline int ScaleDpi(int valueAt96dpi) {
    return MulDiv(valueAt96dpi, (int)g_dpi, 96);
}

// Shared by InitRefreshButtonRect (hit-testing) and the paint handler
// (drawing), so the two can't drift apart the way two copies of this
// expression could.
static inline int GetRefreshButtonLeftOffset() {
    return (WINDOW_WIDTH * 7) / 1000;
}

// Define settings early so they are available to RecalcDpiMetrics and UI helpers
struct ModSettings {
    BOOL interceptNativeFlyout;
    BOOL privacyMode;
    int  refreshInterval;
    int  language;
    BOOL enableHotkey;
    BOOL useRoundedCorners;
    BOOL useNetworkLocationIcons;  // TRUE = show Home/Public/Work icons; FALSE = original generic icon
    int  theme;          // 0=light, 1=dark
} g_Settings = { TRUE, FALSE, 3000, 0, FALSE, TRUE, TRUE, 0 };

void LoadSettings() {
    int raw_intercept  = Wh_GetIntSetting(L"interceptNativeFlyout");
    int raw_privacy    = Wh_GetIntSetting(L"privacyMode");
    int raw_refresh    = Wh_GetIntSetting(L"refreshInterval");
    WindhawkUtils::StringSetting lang = WindhawkUtils::StringSetting::make(L"language");
    int raw_language = 0;
    if (_wcsicmp(lang.get(), L"en") == 0)      raw_language = 1;
    else if (_wcsicmp(lang.get(), L"it") == 0) raw_language = 2;
    else if (_wcsicmp(lang.get(), L"es") == 0) raw_language = 3;
    else if (_wcsicmp(lang.get(), L"fr") == 0) raw_language = 4;
    else if (_wcsicmp(lang.get(), L"ru") == 0) raw_language = 5;
    else if (_wcsicmp(lang.get(), L"de") == 0) raw_language = 6;
    else if (_wcsicmp(lang.get(), L"pt") == 0) raw_language = 7;
    else if (_wcsicmp(lang.get(), L"pl") == 0) raw_language = 8;
    else if (_wcsicmp(lang.get(), L"nl") == 0) raw_language = 9;
    else if (_wcsicmp(lang.get(), L"ro") == 0) raw_language = 10;

    int raw_enableHotkey = Wh_GetIntSetting(L"enableHotkey");
    int raw_roundedCorners = Wh_GetIntSetting(L"useRoundedCorners");
    int raw_netLocIcons = Wh_GetIntSetting(L"useNetworkLocationIcons");
    WindhawkUtils::StringSetting theme = WindhawkUtils::StringSetting::make(L"theme");
    int raw_theme = (_wcsicmp(theme.get(), L"dark") == 0) ? 1 : 0;
    
    g_Settings.interceptNativeFlyout     = raw_intercept   != 0;
    g_Settings.privacyMode              = raw_privacy     != 0;
    g_Settings.refreshInterval           = raw_refresh;
    g_Settings.language                 = raw_language;
    g_Settings.enableHotkey             = raw_enableHotkey != 0;
    g_Settings.useRoundedCorners        = raw_roundedCorners != 0;
    g_Settings.useNetworkLocationIcons  = raw_netLocIcons != 0;
    g_Settings.theme                    = raw_theme;

    if (g_Settings.refreshInterval > 0 && g_Settings.refreshInterval < 1000) {
        g_Settings.refreshInterval = 1000;
    }
}

// Global network count defined early for RecalcDpiMetrics
int g_NetworkCount = 0;

void InitGlobalFonts();
void FreeGlobalFonts();
void InitRefreshButtonRect(void);
void RecalcArrowRect();
void ApplyNativeControlsTheme();

void FreeSystemIcons();
void LoadSystemIcons();

void RecalcDpiMetrics(UINT dpi) {
    g_dpi = dpi ? dpi : 96;
    WINDOW_WIDTH        = ScaleDpi(WINDOW_WIDTH_BASE);
    
    BOOL showWifiList = (g_NetworkCount > 0);
    int targetHeaderHeightBase = showWifiList ? HEADER_HEIGHT_BASE : 76;
    int targetWindowHeightBase = showWifiList ? WINDOW_HEIGHT_BASE : (targetHeaderHeightBase + FOOTER_HEIGHT_BASE);
    
    WINDOW_HEIGHT       = ScaleDpi(targetWindowHeightBase);
    HEADER_HEIGHT       = ScaleDpi(targetHeaderHeightBase);
    FOOTER_HEIGHT       = ScaleDpi(FOOTER_HEIGHT_BASE);
    LIST_Y_START        = HEADER_HEIGHT + 1;
    LIST_Y_END          = WINDOW_HEIGHT - FOOTER_HEIGHT;
    WIFI_LABEL_Y        = HEADER_HEIGHT - ScaleDpi(24);
    ROW_HEIGHT_NORMAL   = ScaleDpi(ROW_HEIGHT_NORMAL_BASE);
    ROW_HEIGHT_EXPANDED = ScaleDpi(ROW_HEIGHT_EXPANDED_BASE);

    // Cached icons/bitmaps (Home/Public/Work, chevrons, refresh, signal bars,
    // etc.) were decoded at the DPI in effect on first use and never freed
    // until unload, so moving the flyout to a monitor with a different DPI
    // left them stretched/blurry. Freeing them here forces LoadSystemIcons /
    // GetNetworkLocationIcon to lazily re-decode at the new DPI on next use.
    FreeSystemIcons();
    LoadSystemIcons();

    InitGlobalFonts();
    InitRefreshButtonRect();
    RecalcArrowRect();
}

#define IDC_CONN_BUTTON     1002
#define IDC_AUTO_CHECKBOX   1003
#define HOTKEY_ID           9001
#define WM_REFRESH_DATA     (WM_USER + 100)
#define WM_SAFE_CLOSE       (WM_USER + 101)
#define WM_SHOW_FLYOUT      (WM_USER + 102)
#define WM_ASYNC_CONNECT_COMPLETE (WM_USER + 105)
#define WM_TOGGLE_FLYOUT_REQUEST (WM_USER + 111)
#define WM_UPDATE_REFRESH_TIMER  (WM_USER + 112)
#define WM_UPDATE_HOTKEY       (WM_USER + 113)

static UINT g_uTaskbarCreated = 0;
static DWORD g_dwFlyoutOwnerThreadId = 0;
static HANDLE g_hConnectThread = NULL; 

#define IDM_CONNECT         2001
#define IDM_DISCONNECT      2002
#define IDM_STATUS          2003
#define IDM_PROPERTIES      2004
#define IDM_TRAY_TROUBLESHOOT      5001
#define IDM_TRAY_NETWORK_SETTINGS  5002

#define CLICK_DEBOUNCE_MS 600
#define CONNECTION_TIMEOUT_MS 18000
#define DISCONNECTION_TIMEOUT_MS 4000
#define WLAN_REASON_CODE_INVALID_PROFILE    0x00038001  // 229377

// Base64 resources
// Classic Windows 7-style refresh artwork.
// PNG RGBA/P 16x16; SHA-256: e3120b592ba3df0d2081edcc5cfba6d10cb287ed494b9d4dfc01e2523d683837
static const WCHAR* REFRESH_ICON_NORMAL_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAMAAAAoLQ9TAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAADGUExURf//"
    L"//v7+/r6+vj4+Pz8/P7+/v39/TO12sjo8fHx8fn5+ZfQ5zWo1erq6ubm5vf398jh7jiXzpnI4+Li4tLS0unp6ZnE4TmOyqXK"
    L"5NbW1tXV1e/v7zmHxoq32/Pz8/T09Pb29jl/wvDw8Dl4vTlxuDlrstjY2Iqn0Ofn5+Pj48/Pz9/f3+jo6KS21Tdhppitz9PT"
    L"07u7u9vb2+3t7ZWmxzFUlMfP4MXFxSpHfpKgvN7e3svLy8PI0yA3Yt3d3dnZ2ezs7OTk5LJJUwwAAAAJcEhZcwAADsMAAA7D"
    L"AcdvqGQAAAAZdEVYdFNvZnR3YXJlAFBhaW50Lk5FVCA1LjEuMTITAUd0AAAAuGVYSWZJSSoACAAAAAUAGgEFAAEAAABKAAAA"
    L"GwEFAAEAAABSAAAAKAEDAAEAAAACAAAAMQECABEAAABaAAAAaYcEAAEAAABsAAAAAAAAAGAAAAABAAAAYAAAAAEAAABQYWlu"
    L"dC5ORVQgNS4xLjEyAAADAACQBwAEAAAAMDIzMAGgAwABAAAAAQAAAAWgBAABAAAAlgAAAAAAAAACAAEAAgAEAAAAUjk4AAIA"
    L"BwAEAAAAMDEwMAAAAADZp5qVybcLXwAAALVJREFUKFNNj4cOgkAQRO9GOPQs2LCBgAhWrNi7//9T3qpBX7Ilk83uLCM4kMlo"
    L"+rsnBAwjm0Mq6DJfQLFkpoJWrlRrdes3IRvNVrtjaxySi/dCx+kCroQDzxcMvQ+gBoHPJPoEviVg3EMYhqBEgAk/QIRBDRhG"
    L"ETBStpQynkyteDZfLFdrdVb4ySjxTHez3e2XCRkRHFzF4Xg6xyCB0C7XK843Vzn7oEu18P74+xB26ZmYOnsBTi4RDe3fqLQA"
    L"AAAASUVORK5CYII=";

// PNG RGBA 22x22; SHA-256: 68489408513c7524a3a9e70e81dfe753c46d59e3cb2ed240faee35b619d166bb
static const WCHAR* REFRESH_ICON_HOVER_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAABYAAAAWCAYAAADEtGw7AAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAA"
    L"DsMAAA7DAcdvqGQAAAAZdEVYdFNvZnR3YXJlAFBhaW50Lk5FVCA1LjEuMTITAUd0AAAAuGVYSWZJSSoACAAAAAUAGgEFAAEA"
    L"AABKAAAAGwEFAAEAAABSAAAAKAEDAAEAAAACAAAAMQECABEAAABaAAAAaYcEAAEAAABsAAAAAAAAAGAAAAABAAAAYAAAAAEA"
    L"AABQYWludC5ORVQgNS4xLjEyAAADAACQBwAEAAAAMDIzMAGgAwABAAAAAQAAAAWgBAABAAAAlgAAAAAAAAACAAEAAgAEAAAA"
    L"Ujk4AAIABwAEAAAAMDEwMAAAAADZp5qVybcLXwAAA5hJREFUSEulVF1oFFcU/u7szO7s7O7sZmMajaIFf4KQh4APvlhpfbBa"
    L"SgVpBUHBxyIIPogvgti+iD8PloL00VJCnwxWRRSKgdA+WGgRxZhsG1E3k0Sz2WR3s//z4zl3f7KzUgrpYb+Zvfee891vvntm"
    L"BCi+vn7fSy3WUbNdHq45gqqCHb0afjh5QIijV255iWQvzn4xjI3JSDNlbWFli7h8+zGWs4sQey/c9n46tQ8fmAYKda+ZsraI"
    L"aQJv8yUc//4hFNfzsD5hIEekbEQnVDgIC1tCODVUqlXUHQcO1XTnMpiDuZhToTFAQt1uuC404eHjh68kJgs12LUqbMd9P7cD"
    L"zMUhifk/79KCQ6QBz8aN51kohTrG9m3BBreM5cIKbNv25XajybtK7NClBVDC1EIJI4/mcGKzidfWPKZn5ik7INGZ2w36yWhY"
    L"QdG5SKV4MrMCUbYRUQQpLVCmAkULwhUKdOEiHnCgedSidZt8d9u1rWgopgn2JywcJFUHBvXjyO8WULUxu1jG0OB27B4aRELX"
    L"YCgudBJ+8Ls/ZV7QpUOtrHrPXBw+K2TBpUcSqNgSd/6Ya8/FIzoMTWmv8900QtBRR5k6hjmavA3iGvWKVRKwih5Gz+wmpfX3"
    L"wPMVB/js29988zyORCKwyRLmYC6Otsccc2WBN9RWo+f2+IrlmOLw+THffAs8P0D92xkdHtNFkOoyqV8qYvSbT+hRaBO6P516"
    L"gb9fz8v/jIO71su11jrjxVym0W5NL1Y9JuddCATUIGarKtJEfvPip0il5/FPpoiX+TrGpzOYsBbRH1Phllbw1d5N+OvZJH68"
    L"O4bUm4Lk8HnMA5u2YjikmtvKqgYw/nIJ6YpAcmATIj29CEZjiEXp9c/lUStksfPDHqQzOZh9/QjHE7LeR8z6ebcWWLlCykNG"
    L"DJF4Ut7VoI4tZhADpo4bI/dx5MAgAiEbJU9FtGcd5URlbcsLnxXd5I03TZX3cMDDzOQU9uw/idLyDI59+REm0gswzB5oepg4"
    L"FFnnU8ybdBN3I0dn1b9tK8YfXJdIWQtQ9ChCUVM+nXzzKK8puEMx2/EfmM47+HV6CfcmZpFaqkrfpVp6Im5fzmkrVuhb8Ha5"
    L"iF19Gn0qyYJ/AdVAkC1hUhjv7UeibwOMWAIBLSTXOIc5mIs5xbGrv3g6HdDpz4exzgw391tbZPJlXLv7GJVclk8IOErk1opo"
    L"vCT/IxRq1Y1RDz+fOSTeATe7heJTThHzAAAAAElFTkSuQmCC";


// Network Center artwork supplied with this mod. These are data-only PNGs,
// decoded to in-memory HICONs; they are never written to disk or loaded as DLLs.
// PNG, RGBA 96x96; SHA-256: bf514a10a26094aca1ea79688e6d3738aba1c30fc88d09f7ee4798d821adfc62
static const WCHAR* NETWORK_CENTER_CONNECT_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAAGAAAABgCAYAAADimHc4AAAz6klEQVR42u29ebRl11Xe+5tr7b1Pc/u6datXSVWyeslqbdmybJUs3OCXYBCUEmIHh8YYE8eQ"
    L"EJ4Z7w1SVfDe4PHimEDgAQYMGBPiuoCDm8QOjlVuhUCysKVSZ6lUfXf70+1urTXfH3ufW1ey5A5JA0ZqjbFvc849556zvjm/2a554Pw6v86v8+v8Or/Or/Pr"
    L"/Dq//uEvVRWApacemPzzd9/5h5/9wC+9GWAPmOF959e5ZZ73Z5ydNQDzJ4/tujE+8UNj933sz/50z7t/fB8ERFDdY85v+wsJQL2ck5HQGgnXTEZcNffF3/7U"
    L"vh/6EKqRyL6wf/9ue37rqxW9UE+8srwcGq40ebPtLtzWDus7X3vLf/tXb5i5/8HHfvrGay57ZD/Yu8Cf14Dnm4Hq70cOHQaNiCxShGAmZja61291r8/+4F2f"
    L"/shv/D+33AV+/27s/+p24QXkY0tsBWJDFMd4X0RhZNy/Yptsuez0/zjw6fe++6fumsWLiO7Z853ZBVWV/fv3D0EcXucpCAAPai02MigRNg6IOEtrLFxxocbJ"
    L"0fv+42d/6UeufM3P/d5Pi0i6f/9ue9dds98WJYmI8gwau/vuuyOAubk53b17tw6xqv/2fyEA8FgLJgIrSmQj1AoiwXiN9eKLL9ALesd//HPvecO2/X/xF++6"
    L"681vfmr/buxds9+KXVAB0c997nMzrdbodRs2XHDwwgtnTgLcfvvt7rm05dlAWKM9iEj4ewGAqsqBXbssfPbbfsL08x+wd9+GrMQYYwUTRQQ1YAVjBIxgRcR5"
    L"lWRyvXt1Mv+m++/7tS/N/vuf/97dP/uL9+4Hu1s1fCOJvfvuvfb223FZlr1xbHz0g0899dDJT33qU0tJ0ugNMv/pyJjIezna6/U/v27dVMtae1RETu/Zs8fs"
    L"3bv3adpQ/zz8XfbsUdm7DxVeHI2JvoGkuO/oGT/7SQfwP//tdV178F6wBsFU1sYYsBDEYxBclkdRstHftH1p0+TpL3z6C7/783tu/bFffN8wXhDZ96wSeeBA"
    L"DXba02zQDnk22IKELaOjlm1bJm+Ok4jgA91erMY2xHtOf/GL93zullte8U+r96Z26C589KObt87MRCNbtuzIL7xw06F9+0T3PYdQztYxzu7du3V2dlZ2796N"
    L"iPjnFQAR0YWFhQvu/r9/9B81nviCSxpGoNoHYwyxrVx4W5tNYw2hLDFxTEyMxt4Qssx99tdepdNA1BQTFLGmAgAQpLKYIoQitcaOhpdsa4yOnbr7P3zl/3v3"
    L"y7e881ffLiLd/ft327t27w96TlKftrrdvLxkZ9t89nOH3b5f/Kq5cMcmJiZMuGDrFJs2JjI15ez6aesv2jG5qZVkt3/mv7xjp+pP9kXk9PA5rrnmq39kdeTW"
    L"Ilvuf/Wrf3tfHE+Yfpn9zs/82ekP72LGPPzww57dDDf677LZskbTnnbj09ZjDzyw9cj7/90nr/bHro4jg7GCkYo+jLFgQGyEsSAGTCxYI4iNEGuRKOCjCB8K"
    L"TGMSG0fV/zUGEXn6a1FFg0IIBEStaPDpij2Yr797cdfPvfv2l1/30DMlcGZmRubm5gzgJybW/6edF02/89d/+wv+V9+HZWIT5A40BppgrGJESIyOtoKfGHfZ"
    L"umlb9JftPRumomzz5rNyxx3N117ykunJydGI6YkmWy/YTGflyM9t3PqyX36aBqTLL3nyxOL7iqLMbRw3gCey3B/oLru7f/Xkg4PdtWasERT9VuzQ1wHwP37p"
    L"p3/1kic/++6J9lhZCObc5husNUhkUKvVZluQmucjW2uDNZXxjWPREAxGQGC49yqCqEJQFEHUoBrQ0uHzAvKuk+6Z6InWRcunLnvT927Y/e57Ou+7y97yM7Pp"
    L"M1/rxz/+sY9ec+Xl//hfv+fP/X/987ZtbJyu7I1JMKZJkCaeBoEWITRBRiA0IB5BTIKiJFGf79pV6lWXZFicH2mLdAc8cGqu/PLGic6YkZWvnV5c/uJrrrTb"
    L"bnnZjl/afsGmDcE7mq02S8sdt2HzRVMi0ns2R2H3/lkze9ddvt58+2x09XUAfPLf/cTHr5t74Ltt3MCpNzayGCOINRhrkEgQK4iptWP19goMay1iFRFDhYSp"
    L"hL32NUSVIFQmrgyEIkP7XTTvoUWGZlBgfavs2ifikezszf/yePvyNxK6cz/Ry7LrR8ZGdhKZT2Wdzkmfu/ft3Lnj1u//wd8LX75viyEeqyyXbUOjBUkDkgYm"
    L"aWBsBFFTxURgWkFI8Fg2bmia9/z4qNz+KhikMBhAoVA46Ayg04dumpP1l3Flv0iiYJsxOjGCJKZXrqwsfPDE6f6fbpnJ0ys3uex1/X/2QEVZQ7raY1T38sEP"
    L"fej3MeYDb3vLWz77DQH4059525+/On/i+6yNQjDBmMhWUm4EYy1YwArWGkxkscPbI8VYi7Gm2l1TG14UNCIYgxgQV1QbnfYxWZ9QZrhSUW8IPuC9IxQG74La"
    L"oiNpa4TTr/1J9Io3zW8eZ33UbLGykpHnHucD6gOnz3Q5fDxw/GjOYGA4frLkyAllqddiflFZ7lm8a4OPgQaYJsQtiAx2An7+XRP88PcZjAkYUUQkqBCCCi4Y"
    L"KT1SlkheIL3MMygMnYGSOUccx3S7A7xPydOOpkV00OJMkaZnTx195A9+4xd2/yHAb/z6r99v4MQ73/Wu7/mGRjiEIMYKYiujW/1cfzfV5osVbFRTkjU1HSnG"
    L"GCrKURCpTLdUGmRcBoM+OuhCmYJXnIfgDapKcI7gA1oowaW4Esl9ovZsV2f+fC+PXvPY+g9236gXX9D0F22N7cxkm9a4SLvZ4Lor13PzjYEkiYCI0gWKEgap"
    L"ML9QsLwcWFxWjp/0nF2E+cWC+XnH0aURzi4IJ08MWOiMMjlmUVVExBjBBBQjgcQocQPGW8KGScUQQASRGK/iSz9iSjdCEdaLwtX9HB4+wpWbJ+U2V/beWbp4"
    L"7oMf/P2NRV6c/eZu6BrvRqX6LtZgTEUxWMFE1aYTVVRkrVSaYagBiFBjEBSKlJD1kHyAOAcBVC0heLwL4BXvAsEpwQV86Qkl4AJaesnVSlhucfGX/kS3lkuy"
    L"5y/+dXTdzdu55sKCOMqJ45x2I6fdgFYzZaKVMzEC0xOGiQnLhvUJF17QotUwxFaxEqECpTNk3jPoK72+o9FwGO9REwEx6pXIxjhfCZaNEgpXCyUwtKUiWCuB"
    L"KFZGrFXRoNNtw+JyT1ujbWujkZuDd4yMjgGDB765G8pQ8i3BUG++oMONjyrtqKRfMPXmqwGMRWwE6iDvolkl7RJAPGiAoKGS9FJRF/DOo64GoawuX1bg+NLj"
    L"S18ZkEFL3tb8GNe+bJ5HXv673HjLRfgCOiks95VOX+n0PSeXCwanB3g/IJacpk0ZbXRoNyMmxyPGWh1GG4HpMcP4aMT46BhbN8RIVGJtg8hECIISUYaKVk+e"
    L"OsmDB4/z0uteRZEqWEVNALFYrdxpBZzzYsSIFxjkykRTtddbDL2BD0VWROumxp785pFwCJhEIK6zW7Xkm5r3pTbEQzdUTABj0ShB8Gi2DHkPigwJWr+4gKKE"
    L"EAhl5XoGFwiuAiKUgeA8vqwACaWv7i88lIrXDFfGaC9i68hx7j95gKn22xifDpVBF0EVvMaUISZ3I2Ql9HLoDaCfQXcAC1nJiaWSMi/Q0EfCgNHmgGZUMt4W"
    L"xtqG8VbE+Kih1YrYNrOBkfEGBHjoy3/F5o07GF+3hdzVdCtCoHqPUjsZAcF5GAyU1kQkwYnt9bsmzzN8yP/nNwWg1WphMEgsELTaaEvt8Qi2dj3FVOkFbO1m"
    L"psto1gef1ylWg6KoL9EAeEGdoCHgSo86X9FOGQhFtfGV1DuCC+C0Aic4im7MeCvn1KUX8tN/+2q2rLuafxQUJ4agiqn/o4jSiKBhlbEGzIwPU75VYsETEYgp"
    L"XJu8mKRwFTj9DPolLPYdxxY8hx5aZtw9wc+8ZRr1Jd1eD7RkYWGO9rotlB5irSja1y72EAADpCUUpdJIhBA8/W4XMcjI5NToNwWg0UgwRYWuRMLQC7LWYiNb"
    L"S72F2AIeLXqErIv4EqO1yqjWfBNQByEAQQnOE3xFPcFV2lBJf0093tcaoFAGnFOynmfDOuGLk9fyrz7zGo677+atrUvoD4R2UxAjhFBvggENw9xaWA2FVGoh"
    L"QYlMILYw0qh+F6R6vUbwaslcxMGn1hP6XcSCD4buyhI2ioibTfICylIJpnara+kfvnUDFDnkuUPF4Xyu6SAX78LK5s0bHvkWckGhopk4qTOaNQiRBRMgTqq/"
    L"K3sV1bis9gosgfpNqaLeoyGgAdSFVZ5XFwje44uadora8NYaoGVlmMtC8WnOzEWj/NHget7z3+7Ajd5ONLWdp04lzK8I0xMKQQgGjIKpaztVatOgTodhSB2H"
    L"SJUml2HaZZgWqbVdhNiAaMZE01demikpCoc1EcgozoELglet7GWdUkGk8j8MrGQQyj4NIwQXNC9S8c7NXbT1oke/hVyQYmJRsYJIhFiF2KA2wtgIcX0076Jl"
    L"iohHjFQSVr8ggq5e6pXgqLybenODH3o6a4yt84TSEQoIKvg+JC7DX7qR9zxxLX/w1ddhp1+Dbczgy4i5OceJs7Bza2Uug4FIhFALs6wpzcgQlNpDrkREa42R"
    L"VXpCqtSI80JWKhvHmqhzqLFkRYaJYsSMkGXgV3NZ5zRApKI6NbAyCBgKEtug8Eqv26XRiBtj5VgCZN8QgGYrCSYrJWqMhkIDxlrEWoxRQroEeQcxAbUG1FZa"
    L"rlTvPARQxftK6vGB4Djn4dTU40pPKBy+DKjXGgjFBCFfyRkbdZzeuZN/8zc38rnDtxOvvxGiMXwIKI6FFeHEKU96paUZQwiKRwi2Tm/IGuleBQJCrQ3D2wUl"
    L"6FCKDYEqAk5TpbE+Rl2OE0unu0IUN/AhwhWVfK3+jyEVDTNcAssdTxIbIguZ9+qKgKj/m+lLpvvfVANGLrvhd778xYPf97LmadtqjpUIQllAuoLVQohFV/+d"
    L"UImseiV48Aa8w+QOJDFFiI3WBrdyOys7oEUNigNfQHAFGmIG3ZTp9TH3jF/BOz55PWfS7yLZ+BJK3wZfReGiSj/1HDnl6Q4s8WgV8AUTUGeeZhCpJb3mJEwl"
    L"5Ku5KRNqt1sUJRDUkDohywONSCiDJxQF6SAjTsYoagCM1slFqLhvCIAoxhq6fWFDUxEJ+CKoV0fm/NFnywV9HQA3vvmHP/mHv/KeH1tYOfv+ybQfV3nKgNFx"
    L"sjInqK8kxlSunzGGVqtZy1VAbIxtQ687z1XlKUZUcGXtbg6lv/SrBhjvCUWEpikzF43ye8uX839+9OX45s0kEztwZQzGA67KnJJQFIFTp0oWVxJGmpVUGzkX"
    L"RMqa5J+sckUVi6zKjlQ2Q+oyhYohKGQZOJcjJlA6jwsFeT4gbk+R5oJTsEhVkxtqmRk+rYKFQbdkbCJGfU4ISlBlYmKq9S0WZBCRX/7ARx5ZeXT+zJPfnWaZ"
    L"etUwEjdHOt2lieNn5k/EUWSNMRSlcxunx7dMTK3vp6XrRFRxSWnH8vTRv7z+8uPvv9P3MvWliNY040uH9wqFR72jTCOMpphLJvk3j1zLH33lJqLJ64mj6SrX"
    L"Y0Cp60MiVQ3Bw+m5wNlF2DRTMZ8RqaLTVY+nrjyYmn/WADMEZbWSH6oHBFXSDLzLsUbwHvqDlLIsaUqLrKd49VipADOiBDGI1LFAXeoZdAraiSUEhw8qRVHQ"
    L"arQWvsWCDLoHzPddMfEl4EvfafXhr//ze79n5ExyZ8i6IYTInnM3PZQBH5SibxlvZRzZso1//dkr+NKRG4nXX40zk6hWBlbF1TtWi7cWQMTCSsnpBcclqSUS"
    L"wVmt6w2KGKlZR1c3dzVYWqshGlBTxRFGlIBhJYVIc5qmSRBD1q8yzU7WUeaCofr/iNbpiMoDNBoQAWcgTz1xYggqZGluBoMBUWPkL7/lmvA+CKpqDhw4YA4c"
    L"gF27qi6DgwcP6jNbWR5++GHdvXs3B2dmhANw87rUtl7a8r2TD4yFoFV0qx6tPR5XBoJC3i1Yvy7miyM7eedHr+FU/0aimR2UOlq3VAAS6iDCI8agwddWL9Dt"
    L"OE6fDgwGEa0GqK84WIeGcejpiNY++hqAakDAoLX01pkrul0lTizGOsog9AcpYhzOWYoQYaSo/lKkSrmjYFxFy9gqlZ31iVAUwyAdEBthYjSy31ZXRN0hEAD2"
    L"Pb1IGr6+HXT2HIXt369y+5v85z78a8EFCIUjqOC8Qx24EugXTF/Q5v2dC/iFP7+SXK6m0d5I4TyQIhohpklAKxBWuTHU7iKkmXL6jKfThUgCShUMqFRuzXDz"
    L"ReRpvH/OfayACeeCeQLQG3jGEwNSAgn9QQ/BUpYRaQiIeIxRBFMBUPskViqN6+cJ6jJakeCcaFHm4n0YTE1Nnn1R21IsBvWGIleCKaEUysIRlyV66Xp+/smL"
    L"+O27d2BGL8PEExSVOwSSoyFGrQOTAS2giUoM6hB1iMQ4HzO3kLPSbzDSqrJNWDnnYq6NUmsGO0dBUmuLrrqfVNkS0izngjFBVNAAhUsRxkjzhIEvsBJWAQRf"
    L"e0N14c84VnoRsVGazYALRkPpZJAXR2+77bsOvigAzNbdBmcXFmWnywhliTMtykGfsUQ4tnMTv3zsOh4cXEd7ukFajlQRrPGoDDORivjyXDpDMsTEqCSoxBgT"
    L"E1zEwlzK4qJl3VgCWla3i62lvcriSu2xqDwzcKoCSBHFSFWyK1VIuxkj26F0Dojw3hMkoTuAzJVYE+rnkNqQR2ACRqvK4EonZbJhiBs5aSoM0oxWI47rvS5e"
    L"NA3oLCz40is4QfOUqXUx94xt55ceugoz+QpedsVFZN0THD2UoUkbNYpSPs2DWc0xSB3o1ayoWvmTyysZ8ws5WzcaEF/Ti6u1wCC1dEK12cYMbUDtTYmudmeI"
    L"KnkQ0m5Bu6FoqAShLApUG3QHFh+K2uVc5TgMFYAqhsQaep0BW3cERBTvA845RkZHlp6ro+IF6w1tTU+PBRfj+zkT25t80FzKex56Je2JV7F5cobMC5s2TCJR"
    L"WdGHKqKhSmGHUFlVHARfJ/eq+6vfSwiOQVqysJCTpo4iF/I8UBSeolTyMpDngbzw5KUnKz1Z4cmKsHrlhVIUQl5fgxSch5FEcD4QQkGZDyhdTLdjyAYF2cCR"
    L"po409aRpYJCWDNLqtQyykl43Y2JUCEEQkRBFEc65z4iI37NnT/SCa8DBg7MKYNvrv7upPXo7pvipJy/kc/Mv4+rLriaK2mQZYAaMjzcZH49Z6eagjSqnVMFR"
    L"C72pPaGyjoKHd1Q0k+WGhaWSbt/TbtUaY4c+fx0VUxlKXaWfoTsqILZKHpqAKAxSS1kGjClRkUr6vcd5w0rPk8TUhre6RIbxR/U8RRzT7+fMTHqcc4QghKBM"
    L"Tk5GL2Jv6J7KkcXomatuYc+HTvJf77+I173+GtQ0GOSKaIFiUBHWTY2wstzB2KjyOIclTaSmHF8FJ+qoCaNyG1XwJSyuZHQHo1XVrnLJa5oJVWS+ulnDoklN"
    L"QYCKqzdRQQ3d1BJcgbUlISh57gkeuqml1/c0m6FKw4k9R3MiiKlTrBEMcsdoQwgqFEVh+/0uU1NTXwK46qqr9AUFYNh49MADD0weO3X2yiMbLuCw+5RMrFtP"
    L"kBHS1IP3q9UxNcLYeBsbdQjeVYQYqv7ROs9Z1zLXNnRVKRABvFdWujkrnZLEGhRTV6p0jbSHWgvsqhvKWk0Qs2oH+l1HMy5pJJaiKCnLDI9lpSMMBjkhVK6n"
    L"4OvHD7VBV7+ngwHWNCAIRVmI956yLA+9KBqwd+9eAcLx48dHgisvunDLZhIzJWJGcLnDOYfXAMGiQfFSEkWWkbGEzoqvbAABDQEkMGyt0bWNZsOQ3wRUI7rd"
    L"gpWVktFmXJVHxa7ZfFmNA1aDrzp/Qx18BakCM2scveWMjeuFyHiKYOgPehiT0Ot5+oMUDY0652VWn1vN0ODXQqUF7ZEE5wNFXmCMZd26dcmL2p6+bvPmscWT"
    L"x8OghIXlkiSKKF1JWXhQh1OL91UaWaxlbCShs9yrKWZNq/Lw65CzayNc1R8M4EnTgm6vIJ+KKvBMqKXUnJNSFNa4j1LTWOWGOjAWq5alQcllbSWoEFTJ+n1U"
    L"PQtLlnxQoqGKG0SH1GPQYS7IGNIiZyIOjI9ZnCuDiJiiyB+7+OKLH6ybesMLCsCuXbvMvn37Quint42OjowUpfhOL7VRI5AVDldUFawSVzkzARRPFEUkDaEo"
    L"XNXeosM0QaURFSJ1TijUQVRdFC9yR6eb0x9EVcOwqVIVq/wsFW0MjbCRc9zt0Tqx5rGmQa+fMz3l8UEIeHxQnI84fdaTZhkuWESqFhBjKjdX64qajSzdbsGF"
    L"F3uasdDNPXmeY4zpbN26dcBzNBg/rwAcqPvGT5084zZummRuvsPysmfTtiZFLpSuRIJQhnMA+OAJEmg0hCL3Ve5naHyDAePqsrpd1RDUIupRLEUG3W5GfxAT"
    L"RYIRWxvqmq+lih9WqUKo7IJUAVqkhmACgZwiK5maCLjiXB1hkAnzC548FJRl3aBM5fkYIwQxWCxxbOkup0xPCdYEnKsAaDabzRfthMzQyktkt0cJnJ4rKAcG"
    L"K4480yqqDMOND/gQCEHwWhJZEKuoDlNQVSwwLKBUFOQBRSVgQgJaoMbQ7xUMUk8jFrwMKah2D4cR6xAAzBrtUEoJlcvpDEEdY6OCK0swMSGULKeWTqeHicfr"
    L"XJRZY3xBSMDkJD5h0O2xbXqMwpUYY9UYodlsHv1GJ3Si5zcGOKgA3me3KaOcOHEWSPBq8IUjBF9FmEEJ3uNqMJwqGioP1GtY3WjqhKgOjS/nOh5UKrfUB0ua"
    L"5gz6GSGJCEbWUM8z0garWlEBxLCQIoa89BBKRlsJ3g8YHt5cmle6nT5jk81Ka7GVd8bQuFfRe4g8RZaycdMM3jsQoyLCYDD4fO2g2Gc79PKCGOF+P82NNjn0"
    L"xDxgSNMSGyvel2gw4JXgFR+qmqyvq0ZD5xNVFHvOCA9rzmYIAqiWiBh8CGSDjH6vBW09BwAWY0KdC6oM5jnNkFUKEixiAmkaMFplOn3dOOBcYHm+STrfJO15"
    L"iAqSpieOhChKMFFUlcJVUG8Q5xkdDThX4kpBVVm/fn38olHQ3r17ddfeXdHh3zs06vOMk2eqTep3UhrNuO7hCYSaglSVECCEqkCjXurcj64aYXAVIGLXtLkr"
    L"MjzFpYEsLRj0S6xRwqobGs4FYLXBXpX8NSAIgjVKpx+YbvUwtoFzEd7ndFf6vOH1E9x88w08+EjJI48VPP6k4+ySo7+0DPkAGopte2yUYUhpJooPoqjabrcb"
    L"pqen736uNP7zCsCePXuMiIRPfOITOzUUN3YHGSdOBKPaIi8VrCdJDF59nWcRgldCULQ+KSNSn4YyVX25IiE3bLtYU5EwFSC1NpSuJE1TIpsQTPS0IGnI919H"
    L"QXU/J1IFcd2VwI7NBY1IKQeevCjBWHBn2LL+LFe8uY0wQpq26PRGWRpcxEMHUx5+NPDoYynLiyVOjzA21iK4LiFEEoIPURTNv6jHVE+fPu16va5ckCRs2gin"
    L"T68w6DdJB5BiwcYQG6z15womw94drTY6hICYEpGo5mJ/DgQNQH1OoX5sWZQVzdnaaIjUbqJdDbrORb/nAAi1BvhI6PeUDesNSll7Vw7EkRXiQ2hqp1eISCGt"
    L"Vk/azTmZmjzGdVe0iW2DQd6m01vPYLCV0fZxnDcUZYG1NqiqfVEAuOqqqwRgcmzs5UWWRkefOhTe9Y5NZpBN0++Os7jieOzxJQ4+ssLho0q3Y8GFOiEboGHB"
    L"GqxRLK4OhuqOKuqknIYKBJE6IKuSbT5AVjhsUXlKw402a6SfGoyhLRiChFp8bBgM+mycSvAacKqkWZ/IJLTaLWujiEgMGgKl95Rl5Hu9QrNMjXMd2u3MNOMF"
    L"pjY2GKQKkpBlKUnSiPM8f3EAOHjwoACcnZvbliQxneWVYnHJxZEkhslluWC6zfXXTGObW+gtZywuC0vLLZ44tMzBh5c4dKhkYTnBFQYYVKBEAWmlGGMRiVEi"
    L"gvraVY1AYxRDcEqWDjC2gRFbNevKOWMspir4VNpwzh4MNSiKLEXeZWZmM6HogybqvRNFlzZt2vSLc3NzmhfZ68vSbbHGXJ20Irt16yb6/R7GxviAahmTF95H"
    L"kTWxSBAR226377/88suPPZcL+nxTUACIo/jqsihNuz3SxEO/7LLUOh76xbiefaKn8UjDrJ+ZkJnpllx0YeCVr5im1drOwnyXI4d7nD7bZH6xwVcfPcuhpzJO"
    L"nxZ8IRDyyh4kIE3BGFcXYRIkKC63lNaC9XXybejrl88IzGoAqE58KoLklpiU9VMxedbDGNEoisUY07v11lt/pX5//1FV4//+F39xSRBuBd0xPjHxGoXN3vsd"
    L"ZVkyMjISZVkGYEZHRxGRf7tjx47l/fv32+cqyDyvwy1UVT76px+9cqmzdH2v37s2lO6l4K+LG80N3nusEZzLabRaiI0UTDBVrsY0GgmRRRqJJ5IEG1sGhXDi"
    L"mOOJQwWDdD2HjgYOHV7h5ImCMrfgMzAFJIbWaIORdlTNRpDqUonqhlA9VxtgrS1QgjEEFzEaLfDB37qeSOaIG+1w4vhx470/eOedd75y79696cMPP6yzs18/"
    L"y0JVJ+bn5y89fvz4VFmWrwVuVtUb8zz/s1e/+tU/Mjs7a+6qT0q+4Ea4VrOD9fUhgHvvvXfTyaMnL89c9+rO8vK1lOb6XnewvdFqzLRaLWut1G2lgf7A+44P"
    L"FOUAVW/yrEezZeXaKy0jI32SRgOxEywtjvHw10oOH5lkkM3w5FNLHD/TZ35x2P5WYBs5zYYhiaIqhW1M/XYr2qnYpzpoUfRzNm1TGs2cfBCwdSnRl2UODPbt"
    L"2xdUK59p7969ctVVV8nuKrsWRGQF+JvhKV+Ae+6556J2u90TEdWKL7/h6e3ntxyzZ4+56qqr5ODBg7Jv3z7/zAPLqhp97GMfmzx16vh1ZelehcotxpgbsjxL"
    L"xkdHxk196MI5j3claZZ6tAq4NARjDdJuGRrN6ixDqzWCyhj9wTQnT8Y89Dgsd8Y5dNRx/NQCvaV+FVvEHjE5caw0GzFIhEewYhj0lGsvL/itX72atN8hihOe"
    L"OnRIm+OTbqHXuePHf/AHP6+q5tmGeawdYTAzMyO33367f7ZD2i+aG7pv39PnO6x9gQcPHhQRccA88On64mSnM/OXH/lIq0jTVxLLq8qieIXC1WXwdmRkNFFX"
    L"ZULLskTBFQ7JVryE4EwIyxh7llbzMNs2j3LFJU3Gx2aQeIqV/kYePyQ89KjhyFHh9Fnl5OkenbPzUJbQDEgjQ9OSdstgjCNgca6kLJ3feeH22J4983bg87Oz"
    L"s/INtN4/UwifORTkOwJAVWXvXmTv3u983s4zX+BwQtYQlLvuuitsGR+fq+8+CnxYVeXLX/7y9q985Svj3vtb1Zjb8izbYYy5rtVqJVmWEYzB+ZIIVTEtnBpZ"
    L"6Si9NLDQWaLR6NBIDNdf3uLmayBpjOLCFHnYwFNPzvC1J2P++m8L5paUpx5dYHpyQGwKDIL3HhGR4APt0dG570QI9+3b9y0PkHh2KlE1+9ao3DN/f76N95Bb"
    L"Z2dneTZjV6e7X9npLL9+0Bu8eqXT2U4IlzjnsGXAlIE8VmyrQavdJm42sWIgVEdTESGKlCgOTIy3GB1rY6OY0o3T64+SRCVFfookadHrdXV+eUWSdutHd9xw"
    L"w3/52Pvfn+3bt0+/HWr5diZ4PGdt9//6zd/cevmOHWPTb2g8cbs89yCkc9HUsxcdhlL/bWiR7NmzR9a0zoRnboCqJtxzbPuHj37+2sdbC+8ZZPlNl6XrpFEo"
    L"EZbRkQlcLBSao9YQRwnGWMrSMTk5xcpKh2YzQcTTbFiMjShLodFIWFxYCE4xvU5/19ve9pbP3nffffFNN91UvhDCJ8+yWUZEwlce+dr3FKK/H0fxWLvd+mpw"
    L"7qmGL9++Y8eO5W8UWOxXtbvXxAV3MWtmpXbDtBpXuZvdXMlu3Vf5h/qtaMgss2b24EE7e/W+AuDKu3ePvvyq733NNSMzP7UpGn35SEgmzvRW5NMnH2D+8HHW"
    L"pw0uCeu5MbqQPHcUoaDZaNJsNChdSZIktFqt1UL/sEgfxzFzc3Oh1WrLxRe/5PF16yb/8YYNG74GUPvz1OVF9u7dq8+0ed/uerZ5QeEjn/jLa1pj7Q9fuXVL"
    L"c6XT8yNjozeemZ8/+Pu/8Tud/fv3WxHxQxC+euTIzsiYO9Je7xPrms3FHSLZM57S6xd0jJPocKrILLNrdheBPRYe1j1cqfvWDGlSVbmLWbNmVo//8qFD1z46"
    L"efZdzpava4yPXbjIgLs7j/GFlcc5NH8C3+mj4jFlhusNeM+OH+BdO9/E0TMnEWOIIssgzQjBUxZFfdQ9QtWr98HHsRVrI0Sg3W5e5r3/xOLi4u6pqamHpYrq"
    L"XngN+MRnDnzwja/d9ZbgXOm8N3Hc4MzcmTu3bdr00eHYlSGt/M3jj+9IouSXbGTvCD7M9Qe9LzXiOBt0u7986+jmwTsn/9s778sf/tFO1nXFxtHPXxVtG4xl"
    L"jU/frpcf//GHvutBuf3rAFvVFGR2OOol+kj2t6/9r937v6c05VtlujFxpHOGRzpHw2J/EXrO2K7Q7ASijoOOJ+l4XJlCL/DxH34fV6zfSXtijF6vT6c/IMtz"
    L"ijKnu7Ssaa8XrDV2bGwEG8fkaUGjkRDHsRsfH49UdVCW5eOnTp36myiKPhNF0Zy1thnH8Veuvfba49+IEb5tAH7kZ3927Ae+5x8f/O5bX72tzHPFiMZxYo8d"
    L"O/nW7du3/vFzzb05fnb+Rxqt1u9pCMwtdzn06OEzH73gUf0vk/dvSikIG5pENkGAJoZGEfy4muNjjDw2ksVfjNV8YXf/5Sd+ctsdj60Rhg0/Nv+7r3/KLbz7"
    L"aKPzsnnbpb/Uo+z0PQMnUU9M3PeYnodeifYKGDiigULfY7Kc5ZWz/O9veCc/890/Rs8VYCIK7yidV5dnYeHUaevzjF6vO9doNn4Zo1eMtsfyHTt2/PNmszmW"
    L"ZRnWWrz3zM3NkabV2KJWq0WWZYdbrdbLbrrppvnvFISvo6BHH3uMxNyZ99JUXJGrtVZcCOQu7z3bfE9VlQNgt4l84OTp08Vyr/jA8ikXutvijQ+NzmNORn7T"
    L"tq2ycmqRnCLkolLEIpoEOzceX2hN58JGw7zeoDzS/FT/0uIXHmtL9Omdxbh5zZF9bz3SWtg0n3cZnO4qaRHoBUNqrB14TOrwKyXac9Bz6KDEZI5iUCBFILYN"
    L"oizi5KkzDFToZBkaQsgGWfB5EY0k1oZ8sDg6Nvb41m1bfuaaa6750pq0+m+LyPuiKNrknHO9Xs93u92RJEkuCyEwGAxIkuSiZrP59qeeeupXgPx5sQH/5I47"
    L"ZOB86+HDx4LP0zA1OREtLSx84ZabbvqL2sdlv6pldnbtiC53990abdkkH/rIf/+r11y6/ZK3/37zf7qzjz1lL73oAvu6+DpELYv0zXzocNytcDLr6NJyTwcm"
    L"174tkcSYItGRldHuDdbaG45EHcqZnJcUU/4y2Sqxd6YIfZv3O5xdmmf+9DzduSWKTgqpg1IRD6YMxFS9O4N8BZd3GfR6dIuM06dPqfVqxDtTZPmSazc/8PJX"
    L"vOLXp6amDkM1c/TAgQPhqquukk2bNn0V+C5VNdRzR1V17MiRIy8ty1K99+KcK/v9/pOzs7NF7aX93QF497vfnf/JJ//y8YVjJ7aK1zDZTWlEEj5z//3/pID7"
    L"77nho4fuehYK2rULf/fdd0e7Zm5+1/9IHpy+52tfvfPJzvFwdsrIQq/LxmSKbY0pNkcTXGs304pbghFZCQMOhwWOZIscy5Z1cXmgfVOEXlwQRdY+nHSttJaZ"
    L"HG0wstEyXkyx4eqN7FwBd7JH/uQiK187zdyTJ+jPL5EWjqwcgAgj6yaR9ggTrRFMmqotSvFFcXSs1f7j6c2Tv3/DDTd8ba3bu3bm6JqND2sclC7wxRc8Dvjc"
    L"X/3VlR///Bf/5Oz80pXT09PmyssuN1s2bWSy3Uonxsa+alSfSJpJO+sPPnT48Uc+cW+r5ffu2uXf8f73R+9/xzvKn/78b7/tw1+7+w8Wk8zbsRGbWSVEoTrH"
    L"kyQ0bZONzSkuaK9nW7Kei+L1bI4maUmDPjln6HFcFzkcFjghXZbo07EZIXKAw6LEkZAkMaMS0cot0bIjOton/evjjKwoZ46fRbKASwt+6537ws2X3SDHT518"
    L"Yvv27a/ctm3b6onFu+++O9q1a5evBwrqN9gjXZtWWeOOhr/LVN7njITv3rMnenBm61ulkfzmYq8X50Hd1PRUY+vWLWzesJHpqUlaSQLO/9ol2zb9FMBte26L"
    L"Njy8QZdv3fATX+w8/OtuLPZqYxuMoJFBYwtJBEmExlK1ko9EkCS04xbr4wkujKfYHq9ne7KeTckE1lhy45kPKSd0mcMscEJWWJIuhcnxtkCNp2USekdPMjYx"
    L"ycTfpiz88V/ROb3IW1/3T9n3lp/SPC9lYXFhTpD9URT9paqWrVbrieuuu+7xp8Ux+/fbb5Q+frEiYSMioXOyM3Pw0Fc/Xobw8kGWo8bSy3OW+wP6pcsmpqYj"
    L"EX38sUOH3lVq8sh75374LPsI177nTW99PDvzR2kSe8RYE0f15luII4jqnxOLxAYig0YWTQRiqYb9xTHtxigzjVE2NSd4SbSFrdEUk40RIiv0KViQAcdZ4ql4"
    L"mUMnnsSOJcTHlsh/4x7aIxMMFpe48/rX8XP/7F/S7ad0VlYYHxsjeI8xhoD2RsbH/4+rLr/81yuGefFHF8s3azV/5JFHxrJe76VJs/mqXr97W5H7Ha50kUR6"
    L"SXeQkqvBNJosdfrHQJbGRybuf/D0V8b+w6f+0w8MWjaox0gSQ2zR2GKSCgyNTQVEbJFYkDhCk3rilo0qjYkqcLD1QIdmTKvVYkMyxsbmBBujSbYkUxwfLHGo"
    L"f4x0c5uV33mAH736TpIoIuQDbrv0BloKEnmCK9VaCQ2bSBLF2mwmdmV5mcFg8BVrjDabcQhO/zhuNv9Tt9vVuV27dDeEF3Lwt3wr/f7PuFH+/R+9t/2y7Tf+"
    L"dFm4He1W65Xr101dnDSbjUHuUCMsp31+9Dd/NhzxC2bUxKRFTuEdbtjUPNz4JILYVFdUAYQ1EFVaUg0DrH42kSVuxRALzgR8LKg1JFGM6+Xcec1t/PU99/GK"
    L"7g5+8rZ/gcsKOlmPshdottrEsTI2ZjGmRPCUZY5FtNftSGQMVhQjsHHDDFk//RfX3nDDHz7DIL8gw72/aUFmOF38wIEDpjZWX1dgOXLy5Eu1KDYWaTqRu+y1"
    L"Pvdv/MyxBy74tc9+2CcNY31RTUUpfEnhCrIiJw+OwrtzU7ciA0lczSVavarZoxJbTBJj42pkjrEGjSM0qrrmTAJ+yzQzX1zkpsblXLrhYjZOzjA9NknbtDEh"
    L"Bi80ooTp6Yh100k1Bs15Bv0UVYIPjiwrQiOJjSD3R83kt1vWHg5pev/111+/vHbP9u/fP0yl+xccgGd7jKpy4MABOzc3p8/2Ir5y333XzC0t/eaJbPFV955+"
    L"xB3un2HOdUj7feMKZ5wPGkovZZGRFhm5KyiKgjzULehGamqKVgGxcbQKiIks1BO8QlCS9W28Sdhxog1dyAc5UdwkajbYGU/wE2/4YdTB+ukWM+uTai6Qc5Te"
    L"EULARFW6Oii40tNsNWi2mnQ7Hbx3JwLmgSKUszdfeul/BvzzSUnfSUVMa5fNrdUQQGZnZ5mZmZFrb7rpwT179r/2Da/Z8qHrrr1s9+JgmYWyx5LNOdo5qye7"
    L"Z+Rkd17n0o4W6YAyTaV0TlxeaUeapuR5QdFPCYTKSNtqapfU9KVWCFHV1xNaFrPSpylj0LREscGosNRf5IRLGWs0yHXAto1jtFuBsgiUVmiPTtDpdkmzAhAi"
    L"MUQRtGMbYiPBjI1GYu3WdVOTW4+fODUpIh987x//8fr3/smf/L9JEj347u/f/Ss8x1DuF1IDvukaunIP6UNJ/uX8Hb3eoBEFMzrabv4QRnZ0Bj3t+0zO5iuc"
    L"9T2OD+Z5aumUW8qWTaffI8sycWUpWZqRpSl5Vk0scc6hUo3QpB4qrokl2jRBsydcs/kyllZ6WEkwatEi8L9deis/tOv7GQyWmZ6qukOy3BHZiJGR1up5MzHn"
    L"jh0FhNJ7gqqqEoyx5EV+bGxi6o8+/bnPvWLnSy59nSsHxZm5s5f+wB13HHlek3HPZ5XrmS/qk1/60rqy03mJ8z5NiLY1rP2FyfbYyMj42OWMJHJs6QTH+gs8"
    L"1TnFk0vH3UK2zErWJ0sL6fd7pt/tSq/TIc0GeO+qIawasFMjjPiI19/xRp46egzNAmWu/PgdP8j3X3cHZe6x1tNdOYkrS7LCE9tqNurGjRtJkqTqT9WqS2Jp"
    L"ZYlup4uxMWIqmmuNtti+dSunFjsUaZbn6uL7/+beH/znd965/+8SO7xgJ+Xr3IkcOHDAQjVt5Y233LII/HX9Jw+q6if3gvzQ8SPfl6j5R5dObr9l5/jm5MbR"
    L"7SNLE1fOdHzGUkhZ8CssFX1Win7o5wM9uzTPkTPHzKn5OVlcWcaqUGQDvHFcff0VdHt9NkcbuOWK6yiLAieOyAo2ijAmQqwnjkx9+KM6Dxa8XwWBULXIB+9Q"
    L"79AAaS/oSmfFa5lJZIiCiY0ODyW/yDbg2y3Iu2d6VHVRXuq0tu6DPwP+7ISeaCuE4ivRSyKbvGcsc9oqWuUGH10Q7Lqb1l20YYp2DJFhpehzemUhfO3MEb3v"
    L"yEEOnz0mh588JJsvuoDNGy/U162/WfunV1iOVhhPmrQaiXhvjEFx9YA/g6UMQmRjrKm0ydQjtNbUPkEDVmLJ0jyKMJhYfVF4+lkmf68BeA5A9JkUuH//flN/"
    L"HMigvv0h4J+vfeynP/3pjdONiVflZfnmdLm/bVKiGzeN7ph46dhOXnPBddx39CGO54t86dRBVlb6cqPZzgWbrmSQ53QGfc4uL9LvD1yrkdBsNKTdakoSYUya"
    L"kpVl3b9racQxRJao0ayK0d7jnUNDNde01WwRx3BmeVHHxsfd33lP+Hu01n6o29peomdr8Dp79uzmheXuTcePPHlX2l3Z2h4Zv9Bbs2Op6LtmMz518frtR9SF"
    L"Uq03qCl6ZbEzipOXpIWjX5SkWUGRFzo6Pu5H2iPSasTSjiOJYyuearyalWombxxZyjwniWMmxibUROjfPvzY8uGFs1e+/c1vPvN3McLR3ycAnvEm/LN1X8zO"
    L"zrL7rruCbNhwCvhYfbGsOuVOLWyejkc9RbEgW8efdjDi3nvv3bR5fPKthStaquGOrCg35YHLFjq9aO7sKbIyUHqHMRLWTU6GqfExGWk2JIljiaOGzC11OHPq"
    L"qXD5JZeE1uhoZKyZffub33xmWCP/B0FBzwM4z2zwMnvZq+yFSZElYOmZvUZ79+6tPwNITgPvre/+RVVtnjx69NVFd+XVUaSXOMsNqTNbNUpGQijNqbl5+mlG"
    L"CEq71Qzj7RGzcfNW44yY06dO/vctk+M/u2fPHvNsh6//wVLQ80BfsiZY1K8rnR44YHft2lVNKnvG/ft377YXvP3tOzPVGyfXT7+yNTo2ttwb3NrL8h2lEh06"
    L"dlxNs/HkxumpSAf5937/G+/4yvORuv5f9oM0134q04EDB8Kz9feoamvp7NkfsLF9S6+b3/v+D3z8lx8Y3Guvf9MF6d5de/3f149HPL/Or/Pr/Dq/zq/z6/w6"
    L"v86vb7L+f8I1JKitPu4jAAAAAElFTkSuQmCC";

// PNG, RGBA 96x96; SHA-256: 90fa413b19c117b4aee7c8014d134589c34d5fb492dea8fcb056a065c338c915
static const WCHAR* NETWORK_CENTER_HOMEGROUP_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAAGAAAABgCAYAAADimHc4AAAq/ElEQVR42u28e7xlV1Xn+x1zzrXW3vs86l2VqlReYAgk8hJUHkql5CUocmmoA32vYgOSNKgg"
    L"ihf70rpzeCiXTtst+LjJFVS0xT4HwZZHq6BV1SoIJihIEjSQIpCQVCpVdc7Zj7XWfI3+Y+0qEMgDTPAD1Ph81medzzln77X3HHO8fuM3JpyRM3JGzsgZOSNn"
    L"5IyckTNyRs7IGTkj30oi3+hf4MDKil1dWkpvfddHznEL256SmX/c2tqxB5al3dc000zGmIwWzonmOML231G59m3n7ijWnvL4Sz4mIvoV33g4NCxfoXAXfz+j"
    L"AAAVEP3NP7r2ecnN/2qW3rYQDOPphI2NdWJMxKQIAppxBuYX5pHksTZTB/nbmMyJopqz6yfX/j6l+Q/uHOTP/8KPP/xvRSQDHDiwYldXl9IZBXzpBlU1yyL5"
    L"6vd+8vtcOfjzycRzcm2UJ3XQxkcJPpimjrQh4VMgpYS1jhQ0Idb25hfYumUXSEHTgk+RnGB8x7G8sND73KBKb3vmI4++/ulPf3p7fyrBfaMq4PrV1W7zJP3Z"
    L"hM2T0Ti0baqm05r1ScC3kbaNtD7iUySlhDGW7ZvPsnvPPovFTQOqQnLTJvVBqAPcuTZR2190R9fr8+p6+vNHjhTPednrD77mTa/e/wf7hgfd4eX98b7+HuYb"
    L"VQGr112ngFR9/eHs63owv6WKMaWchbZNTJpE3STqNtMGqJvM3nPO5ZGPuIBvf8h2HnxBn7O2l2bHlsouzmOrMtmFBefmF0R7pckYm06E/kM+dmP79pe86n0/"
    L"fHh5f9w3POjOKOCULC/n4XAoP/rki4/bFJ9ZuvZvd+zabgtXxMGghzEQsuJDJLSRh1x4AZc+5iFcdO4mLj634OEPXGTvrpL5QaIqlbm+ZVAKZWGkLK3pFcaW"
    L"LqUTE01H7pTffe2vXfOU+0MJ37gKAJaXl/NwqObHfujBf96ufepJC339yHnn7XaDvsR+z1IWBkHYtXM7j3zEBezdWbBjMbNzk8NoxOXMXOXYujDP5rkBm/ol"
    L"Cz3HXM/RL6Ak2YLIkc+P8p8d+uQv/7f33LzlUg5lVb3PYqf9Rk9DDx9e1oMH1T3vmQ+qn33gWe8wRfmk/uLmvdNJE0WNEVPygG87j3N3L7DoIru3FvQLYTzK"
    L"pCigQvTCdNJSTwP1ONGME76JZJ+JbTIphpRz76zPfvaGcPV/ecVfHDqEu/nmw/lb3gJOyf79EldWVuwLnrV/rYx3PHlTGa75tr07Xb9n45atm1lcGDDamIBY"
    L"5voF9cSTQ8Jg0ATBN+SUyd6QoxJ8YLrhGa9NmaxNqVRME2L+9G3TZ/zWbx3pHb6UfMYFfYksLS2lU0rYEY4+eWs/X7Nr13Yn1mURQ0qCT5ZJI6zXyriO+AAx"
    L"CCmDYohJiCkTcySnhPeBEBNTn0zKIoXb+fD/+b8+fNEs/pgzCrgLJTzrWfvXtsyPvz8lcxMykBjJKVpuv23EkVtqPn8sccdG5sS6ZzQOTBtoGqVtPU0dqGtP"
    L"0zTEGEGVJIbWp+TbrDv3DP4twG237bFnFHAXSrjsqmuKf/Pk7z4+ntbvtmVfpmOv43HkxEnPkVsmHD0RWZvAiY3A2iQxGgfqJtL6jPeR0HpCCMQYiZqJLkOR"
    L"0GTlls8dnQe49tozLugu5eQHbsqqKjfecMMft/VU1zcCJ9Za1sbK8fXIiVHmjhMNd6wn1ibKxrjl5FrNxkbDxmjMdDLG+5YmBRoyUSCQ0SiIuvt0zdw3y6Kf"
    L"9smXYj7DxF1+9aPT+ee8fsu4drLWRvK8I7aBujZUPQUyOTVohum0ZTRu2FifMhnX1LXHe09GkarAFg5tgyS/oZsX524HmP/BBynXfosrYKhDcwmXyHVcp8uy"
    L"3GUmy2QgAvzuG5764d//7EfWQq4W8aK+Z6UqI1VfQRIpBkjKpK6ZjmpGG1OmTUs9bWjbQLYWqVwHmWk2JUlu+afPvxPgMIfuk0xIvlF3+/WXXC+rS6unAbKr"
    L"rrnq3L+94RN5xzk7nhC2rl34uWM3ZzddSBvXXfTyerJ7Z5rMaaVbpZcHmFyigBgh+JamnuKbQD2Z0jQtta/JtiQNKlJhEJ81rE3ZHNc+/5M/+gPf8dKXPvCY"
    L"KtwllP3NqoDhcGguueISWZIOmfxvn3rHhZ8M1+9fz/UPFRv5KZMwEVl0LpceHzfA9wkTx8m1DdrGExrIJ3djjl+AObmT9vgWsrek6PGNJ0ZPkwOBDL0KrUrI"
    L"CedTYOyLfj72M9d/8JW/zL6h4/By/Ne0AFlZUbN6d2DZktyn8O2BlQP21I5//cevfHDN9GXieH6qwlwwLXkjgrU03mc1pNY3xBjQXLjRpJZxexKfGmIKxHFA"
    L"N3aQju6lOXIB4fazoIFkE7kqCCVo4XDG4pqY2vWx3dlv73j32/79BWfvoe5W7b5p1Liv3vSvkNUlSUv3YoEPHFC7skL+l5rqZddcVlz96KvDWz7+lgfc6o++"
    L"cizrL+xv7Vdp6ilSkeZTD/qYJmcqE41BTbDzNE2Njx4ZKOgCbSiZ+gnqHPXiHaT52yjO+ieam85mcv2DcX4vrl+hBgrJSNukVEc777x/2CV7fvjss2V64MAB"
    L"u8rqfba57pUFqKpccQi7vF8igF5zTfHqa4snnWi4aGB7TztxfJpzzrJpcYFxM3n3/Dw3vullj/jAqa7SvwBLlwMrK2Z1aSm98cNv/H7dbN6R5vJcnAbm3VwE"
    L"sae/Q4agLSEHYs6E5PGppY2eJkSmbcOkrdmYrNPGgPe+6xe0Y0qjcPTbWPvUI2H8IIiBPI3BNbEowka7c1N8xl//2Svfz4EDltXV+9Sy5Z53vZrl5W4hX/vW"
    L"G8+Zmuqnkfx/SLFwftIe3teElIlemU4DIQpxcoK+hptcjisP25n+60/91GOPgspweIUsLy/fu+xBEUUREX39Df/1Z6Zh9Iv9RVcO7HzoZ+cqLUSkQFEUiHh8"
    L"9rOFT/jkaWODzwEfA5OmoQ4NtZ8yqifUTU30gdE4kmMLbSS1m/GffRThyAX0mgG75uqjZ29PP7L6ez/5/kdddlVx7dWXh/s6rt2tAlZW1C4tSXrD1X+zd7p4"
    L"7o+Tqn9fLW7eLEzAx6SKNqGWlBJNKzTTSFMnXVtvzKiNxuBw07U7t82HX3/bG/cPT1nTPbokRYYM7bIsxzd+7D+9Rc52L2yPB513A+bKeVkwPRaKOXpmDgVC"
    L"9hxvj9OkhqiBNkfa7GlCTZsa2uiZtg1NaPGpZVJPGE0npBAZNzVt68lNIiajJjrlxoe2/Rsv/vl/86yz3vbqly8d+1dpSQ4PHnRL+yW+/tf/+tt14dwPzC+e"
    L"tUvSmEG1EQsRkx02itKPFXUdEU1oMqCZQbL43GYfUx7nwfYTx0a/8LxX/sVTlp52yQ+LyKe/2Kq+YuxgxSzLUnzN3195td+RXtg/rn5vb0/hqkJ6lOzp7eGs"
    L"wQ56DMhkNuIa1ljubE7Sak1ODVkyWUsSiZgSzgjOQsxCaQ39ytIIVKkkx0hwmZJMZKyDSz4qlzzx87e++nl/fOziAwfK1dUlf39lduYu3c7+/fENv/13l9gd"
    L"F36gt23brq29cThvO3rRnso9cHdpdm8vWOhZ+k7oV5ZeaehVFldaisLQ6xXGOnViGs0xhyO3yWPe+cf/8Bd/d/DI5mWu4K7QxBVdsauylF7/oSufrIvhxS7a"
    L"cPamPcW5i2fJ3sFu9pS72dvbw+5iD9uLbWy2iyy4AXPVHIOyTyklhZQUpsSKo5CCwjoK22U1IiAi3d0o1ijGgkgmE8RktaN6rbr5xPjtV7zjF597/eqqHx4c"
    L"3m8Fq/3yzGXFHjt2vfzsG9540US3HO5v3bTzgu05bR4Yd/bWUs7bUbF5HowV6joTc5eRqYJmJekpSFcJXog+SY7JphDDiVHY+lcfv2nLp9/8E++GK+zNN/+O"
    L"fmll+xPyE3n4oV86P+0Mf1VIMjvNDnvWYJcMzAIu99hWbWNHtZOeVBgVmjhmI6yx4SfUsUVRMkoWJecMmlCN5BxJmok5kXMizq6cEjFHUgeZopoB1XEY6+0n"
    L"j37nC17+0v++/NT/OB4Oh3L48GG93y3gwIEDrK4upU8f5w29LTt27N2U/PaFwm4eZHZtcRQ2k1TIOdOrYHFgmSstpRXKwlAWQllYSmepnKVwgi0yibrweRI2"
    L"2vnL/90r33Pl4cP743A4lC/OtAD+ZO1PtoaF9Ju5jP0BC2wptsmC2YzJjs3lHNt7m+mbEiFTa8OG1ozVk1LC4bA4eqakLyU9U1G5Hv1iQGX7VNKjtH2c7VPa"
    L"PhaHqqABRAUjFgWSGpNbyXfo7ee/7+Pveh2QD3HIfF1c0NKSpOFvf/phdrDl6VZGcX7elW0zZst8yaAURA2+SWhU+mXBvCuYLyoWypJBUTEwBQMp6eOoFErN"
    L"mBwRMpqiW1ufxE8dWX/Zf/x//+dFp3q6AJdfe7lbluV8zSc//PziLHmib6LvmZ6ddz1UlaDQNz36lDggaaRNLXXypAyFq6iKHoXtIdlBtGgy5GhJwaDRQSqw"
    L"WmGpMFpQmj49O0fPzlGYHkYNBodkgybnmjrE1vkXveL3X/H9h5cPxwMrB+zXJQaMgn11OZgrxCSpm5aqKJmvLBYIMYGP9I2lLw5yJvhACIHQeMI0zK5IO2rJ"
    L"LQxMn4oCTSJlT1hrbHHt9be+HODQoSsMwNWPujqu6Iq9vTh5II7Wc9/2rbElIhZPoLSWeTNPoSUgZE346Imxo52kDDEqKSkhZHxIhJBpWs94WjOe1ozqmrXJ"
    L"hPXxhI3xlMmkoW0TYgqcrXBSYNXiMGgOFFpKPa7105/77FBVDatfBwt49VtveByOZ4dwXEPAbmwEBv0epYXGZybTQFQDGHJK5JQxAlbBiCGpIWVFkxLbQDtp"
    L"2Dg5phm1uFzgBKMkjh5vnvrS4cH5w4fJw4NDh6CHPvjhx/e29h5XNlWe155VhfUwIaSWedOnsn2sKYiaGceGqTa0yZNTIIaZH4+JrIlEJKonkcEKyUISJasn"
    L"aMCnyLhuWJ9MOLa2xvrGmBAALShcj6qosEmsn9T5aLj9Mb/0p7/0lNXV1TQcDu9fWkrb6o/YYsEWVnPMGbEVihJyZjT1rI9bpm2mDkobMyGl7sv7TPZ5thNT"
    L"txg5kJInxhlTrY60dTI5ZFUWHjBpxo+G5Xz9se5zDESeawGlRNQiGepUgygL1RzOWlSgjYENP2YjTPCpIedA1kQmAxlVBdXTpY7mru8bFVBFs5Jj939ZMylG"
    L"pnXLaNLStJEYDcb2KFxJ2SvyWnun3nDjPzxHVc0hDt2/dUCb5eG2TYQCcXOCbwLra5YeFdNGGdd0/pzMtM00bcKHTBsTbczEkIkhEkLHx0w5k3Mm5URMGU2K"
    L"GglVWRTjqX8icOji64i/dvjKc26Uz/9YHCf6Vt3pigwwIogxGBUSkTqOGfkNJqkl5ojX0GU1KeJTIuVEnj03p+65eZb9pNnvUs5k1Y4NkXOXwcVEzJ6Apd8f"
    L"YExBZQc2hkY+d/S2J82glXz/xgDbe6yvW3JwJreGjY3ALbdOOXJry23HE2tTw7gRJnViWmcaDz5m2pDxIRO8JwRPiL77ojGRUrcgUlhM1QNxdm3USpiGx6lq"
    L"uby8nI+kW86d9nypebbq0GUkKdN6z6ge06bOfYzTlA0/YRImNLEmaiBoJGgi6qyPm5WYlZQyKUdUtdvx+dSCZzTN7jkjAsYYDAIKvol4H0lRxfuQ70gnNv/S"
    L"+6985Kl0+X6zgBPHR3luYavxbcAksJXhhAqTWGOtYB0UApqUEBQflWmrTNtE0yT8xBOagG8aQtvim4am9WCFXAjZJJImM91I2GQeAZSAD319Ln00tzllcFEz"
    L"IQcKcYyahlv0KNZUVLbgWFhjI02IBFAlq9Kqp80tIXvaHGg0d6AckUggqSflQJhZAVkhKpIEk4UYQYPixFJY21mbKjGp2FQl73TT7x9857nA3x264pC5W0tQ"
    L"lZVVzHU7Dsny/kvT3UHXX6aA1EaTK2GigeiEFAyxyMQgpxVgUXJSUlTalJm2gfHUU08j9bRhOm2ZThp80xJC6AKjtaSOiMapT5NiPP3BPn305unuzbsFZrtW"
    L"EkmU1iRiO6KNGVSoTME4NYxzQ9LQFYBkUvL41MzAuIxPnpA9ccaMjrEL0t3MQOyunEip+52m9M+gMYOQEUSEhMjoxFh3bt39KOB/HL7+HgoyEV2C9DVZgGRh"
    L"vDYhlNDrOUJMBGfpVea0AoRMminAx0DTdk3tpk74uqWetkymDaFpO27NzJ3MrJsspF5VuBMnRx8AmmuuuXXwqxuveWJdNxSUJhKIEglE6hgxamhdwGePy4bs"
    L"LFhFNTHTADlH2hgIORA0E3PAx5aQ/GyRAyGG7h4iIcTTWdypeGBE/hlMKbPYk8XgxMlCtbD/HiBcUYXf/NPrtswF8/xo0q66bN+w5eSjxktLkvnC3rvrGFBm"
    L"EzZOBupxoJ56ppPEpMlMamU0TYzGifFEmUwyG2PPeCMwXvc045Z6PGEyndLUE0Lr8V7JaglGyJVFnUFzhBC0MJEUJ58SkTga+bISe1FqIjEFCeppkufkdNz5"
    L"+dwyzQ3racy6jpmmMbWf0ISGOtRMY8041tTZM02ROra0saWNkTZGmhRocybETAiJFJWspqvo1SDJIVoiWiBqACEaEGcRETQpqHDnsdund48eX1eIiMpo/ZHl"
    L"4vb/Mti8++fcKD9iaUnSyspKca8s4M6j4Y+l2vzsyWSUcmptrCzBZ9rCIKJYe8oFdZlO8IGm9jR1TdM01E1DU3eux2cliiE5A4Zup8UIIYrLLZdcdPb0Q38K"
    L"/bNDjp+NdQphIZgOSo5MMVrgnOsCKN3rnTG4bE571VN/CxoIKRJT14xpoqcNnvb0z5E2RWISUrZkLQkpE5KgajBGKKzFkrFGO3piniksJYwxhODvKvjKVVdd"
    L"45aWvt3/1srBs7Tc/J+PnTgRcTb35nf8yq+/6x9eu/Ssh/7hV0KBv0wB5+0yHzty69qz65TEN4GqKqiqSFkUiCjGZEielDpT1tzVAdNpTdu0NE2D94kYMkEh"
    L"W6BwqBFSDKhmkveuXwQ9e/fcHwA88MIL1d1iBt63OBNIAQyBypVUWqGmy6KiTRgRnHZo5ikFAKQc8SkQUyLkiI+BNvruSoE2tPgYyVKQULpNXWJciel1rtcZ"
    L"wYrijOJyQw6BJAnnEm1qqSqb76Jnki+//NHhd9754ccGBlflYvDQ6fpGFrXWinv4YGHzO6567yf+r8ueztuXl+/BAh5xfu+/f+4zty9PppXpZYeoEH2mkfa0"
    L"BWgOpBSIKZBjJIZAXdd470lRu12YleQM2QliO/AuB4+GnGy2Mj9XfugZP/cDnzvU7HPbob2zGX80980TplOvkpMUxpEFkgilUZJmbEpYMVhjQLuQ2VmAdqlo"
    L"6nZrzF9YfB9jF8eSELIlqkHpIZIoyxI1AqbEoJQ2YVQwKFZ6RNuSg6A+oi3s3fPAwUf4kFx68aWyc+XH7cXXXadLS5IOHjzojoz3/HyT0i+EqIyPn0iosQYY"
    L"jTbCotvkiP4lIvL79+iCXvzih3z6L69de+fRk+bZk9Smpq5taQt6ZddsMa4LeDEFskZSCEQfab3vAlsW1BhyYcmlkKDLglIgtFNcKHW+FHvRRTtXHy0SHvOf"
    L"X9EXkfp7/uBpH1ycHzyhndTZWGf6RUnykFQIJuFzoCThxCKm2/5mVtkmFK+xK7RSJuZTOz8SUiZlA1JhjECGwhkKDDk7TGmx6qgsVFYhRUQVDZEmT0jWESVq"
    L"WfQ5sZ7fK4iyTITDALz1ndc989Nr5mez7T/+zvWjOaVMitiQE8YJfS1sOW3EqbWnOox3nwWJpF//pY+/+J9u/uz3boyKbYWN2To1jbOoKNZ0Oy+lSMqBnNJs"
    L"4btMIllFnCGXhjSLFTFlom8g5JTqxmzbzV++5Ve+6+pztgzNbfO3xb8BLtn14PIfJzdp6yPOJtDc0URUKYwjWksgYjGInVnArLhSIM4UkLN2sUAzURMZi1Jg"
    L"rMUZh1iHVYelxNKnUMfAVNhYQFtgKBF1oIZgGgbFBieqW2loMKG8XlXLQx/5zNYbbl17M14uGEfzqDYox9duiYq6lCEFxaeIsZD6YlIa667t2x47ctc/G1i5"
    L"WwXsGx50L/0PDzv5gp87/KvX3aivOXr72FsbSy8GjGJFsIZZeR+7vH6WrmEECkOQWfUpQk6RNIMr26blIedsN09+woU/K3JufWBlxV58XZcv99f7bw/j/NOT"
    L"0Lh+r+qIDlnJGaKxROuw0sUASV0Q7iAfBbTDgnKeWYGSzcwSMSQcYissJU57FOIopc8gb6Hym3HNVmw7j8k9JDk0OUKAPpnKNCz0G5eKKVsGvbe96k0fnM4P"
    L"XK8/v21LXdccH92RY5s0JXE+eXJMxKBkoKwKDA5nXM6qlpR++EsVIF+RCnJgxaysHHDP/LE/fds/3hyXRpOTwYkUmlqcKxEDMJu7NZAMCLNMxyoxRVC6Xdh6"
    L"JCvttE3btpT2mZd+22+96eef9KLVVczSkiRVFUH4jQ/9xp6VT/7Jx2+Xta2VdeqsldIV9E2JMQY728Fdrq4dyIb9ggJy6jKlGDHWghSoOKI4jB3gdIDRAX2z"
    L"QD8NqNqzWPRn49I8kge4okRtJvjcFYLJ4NuIbwNtE5i5FtrGk/0I56Zprm+kLCtT1zUxdhCHb1tSzhhj6fV67Nw+z5bNfTbNOTYNXL78WQ+199SU19XVA1lE"
    L"WuC5T33+u/VTR9xzRxsnQ9UvC69TRATUYFGSKil3UDQZ0BnGohkrgAi+bXShn83Tvu9Bx37yZd/3KhFhOBzqzOXpgZUD9iVLL7n12W9/wV+daCc/1LZNzkVh"
    L"NWfEKkYs1hqMhK6Xi+nAzi9yQZp11uuVLpDajFiLMz2c6dOTHj1dYCHvYT5dyJzuoigKKBVrFZyQ1BCdEKPBR0AMiEHF0baJTFbjSrwfMBkle8exdXrFBFdI"
    L"BzbOZgoAqqqirE5hUXlGXRJzL1kRoqgKcoX8ye/84L99/s98wPzDJ/OBm2+5I7m5nu2YeRlS6LIIBGO6iUTVgM4AL7KiGTYvWvnu7zw/P+TCzQt/8/4P7wHu"
    L"vOSSS77M+vbM7fq1m8Y3P6P1Tbe4zpEjWGtw2XX4zYxUK4nu/U8rIGOtwzqLNWApKZzFmh6l9Jkzm1hM57HJX8ic2c7cfIW1Xf/YGksWJSYlOYgxU0dFLBhR"
    L"MAVZIjF7kTJBtBA6RsaJtZMYE7FuBvSljCsKXAE5BVLqKu4OCEx676mJIqqqiFwhqlc877X/3yf48LWfOvDhj34mtRErTrClQTMYsaAWI4oho2YGIbvE1s3z"
    L"XHD+DrZtW8Co7Ukh5ZeW5KtLq4kh5s0/9IY/e8SbnvKX9NP3pLpJ6kqbbO52tel2tpyGTEDzF+oAFSiKTCEFSQw96XL7UoU5SrbEB7AlPIi+bKLXr5irlH4h"
    L"9KsSVWE6m6qP1tAIJDKaBarOY2h2hNAF+V5REEygKCtymqNtp9TThtLN4qKRLhvUWRGpkLzgBqV8VdxQEdHhcCgiklX1uVev2u++4AHbz/m7Gz6bbz+2ZtrQ"
    L"lfQKZI2zJohSFgXWOhYXN7Fz24CyBJIg4jCm/Yoo4oFLDsgqq+wd7HjNp7T9wEhPxn40SOryfBGhOI2cKHyJAhA5DTk7FbDgnGUgPTbl89gULmJruY2tm+cQ"
    L"C6VpOHt7j039gpiU42PLiTVD3eYuc6NDDjUDSUlOKUuLBkiasHbGilSLSAEaaX1DURiMdI0ewZ5Ge4w1SM7jr5qc2zXODzoRiW9Z+ejzLzxvyyE1KR+9c5Me"
    L"PzGVtXFL45UYOuRRBKxzGGMRuh1hjesqaPRu2NSr6cDKAbty4PcOft/VS+9o3Og5oQ0JTbbDHYSYu9B7iguqmdkXhcI4JGWIGZsLbGGwWViQHWxuHs6C2cXO"
    L"LY7tWxwxR+aqkm3zwqDItK0ytUrhlKwQkiF7gTxDfRNYZ3BOCPYUj0i7ECEOwSHiSNGRJFNVtnPJs+zQYtP8Ys9gJu/5mmbElpf3x+HwoHvR0ncctjR/dv55"
    L"Z9stm6q0dWvFpk0FC3OWwVxBVTnKqsRZg7N0d2cxpkMW9R6oqBdfd7GKiD77Qft+cofZdbT1DfikqVVim4g+zTKT7ko+kH0ktpG2DYQ6kpqIhoxEmDNzzIcH"
    L"ssBZzPeUTXOClUDfeLbPOSpjIFm8F2JIWEmUVugVll4BvULoFwWls1irOCsdHdiCWO3Scuu6LE0Fg9A2gayZsrQYI1gRxKgoUYx8+VDTV9HZOZSHw6HZfJZc"
    L"Nt9v/+ns3dvs5sUqbVqsmJ8zzM2VVL3ytNEVzuJmqaOzBWIsWTMQ7tba9h3cZ39i/0/c/pQLLn35eZvPs+MwDUUWxGc0do3dHLpmChFMNl1xFkFDwiXBquAU"
    L"FvQceuHBGKfML0JZCT5mFqqSQekobOc+fYo4Y1ioepRGqAQqESoRemKZsx3NZs4UDEzBnC2ZsyWVWHIKkCJGOxIYCG3TMRmtgUKEnnMm+npiiy3v+JrnA5aX"
    L"l7OqGhG5eeV//M33m8XqhuS3VFlFnbUyqSMCxLIgxng6dzfWdjm8dXToSHG3zzm8/3DcNxy61z31/1655Q+PPfLYaP1VGydvDz3bdypGuoZ77ghV2vl+kY5U"
    L"ZRGMQilCRZ+yuZCiWiCTUKmIySA5MtevKI2QUsS3CZehwBBzxiawKlgMOSRCE/ETJU66Qz/ChidNIlIrVXBEW4DxxK55jTMl9XRK9JmqdFgraWFhwQxc/df/"
    L"7vvPv/kFX7sFgIjk4VDN0jMfc6TIG4/btrU8vmf3FrZsGqRNCz3m53v0+gVlabFWcM5ijelyeNOZaJR7psQfXl6OcoXI7zznyp/7np3f+caztp1btNGLxJzM"
    L"qfQ2pdMXSZEZOGfVUIqhl3ZRxfMxGMQVTOrMZJKZ61U4SR2KmyCECCljxfyzppHBQIIclZgFn013JaENmVHt2diY0I5qNCplUVG6qntNFuo24EqLdWhZFYiY"
    L"d30lVvhX3VxeXpY8PHjQvfB5+z660JdXnbdnm+zetdlu21zFHdsW2L51jk2b+swt9BjMWWyRu06aVSRbpJF7x7FfJjPEvO1H3/yq7938XT+ynZ3ryVrb1j4Q"
    L"smpWJBskdwCboFhxOFcxR5/5fA5O5ogEjCgpJaatp2e162dEZTxRam+ZhETdtuRZ40XUEFLuxlSNYFy3iTpFd8hszhGNGT+N1OsBPwa8wWgXK46vNcQkunlz"
    L"32qaJify3nvNjLvHtdm/P66srNgXPfMRb5G08T27FsxnLnrAA9zWxUHcurnPts0Dtmzqs7A4YDCoqKoC40ym32Nc9g4A7NhxQO6dEva533jRlb/3I49feurZ"
    L"c2f9lavmiiZ4kRBjCppzFFKCHBQN3T37Apd3oMl1uFA8lc0ICUsWR5OUcZ2YNEIbDD4ZQhR8FEKia7nmjnHXwRAd16ljecTTvCKMJaTEtGlpg5KyhWRop90U"
    L"zqBnY0+mv/nCZz30c18J+vma6RVLS0tpePCge/EzH/nXc2W7f77id88/b6fbvFilzZvKtG3rgIWFPv1Bj2o27CylQUX3fnXaPhz3HRy6/+cHX/Hhv33V+5/w"
    L"vec89ooF3XSninMhZ9O0IQefYg45aVTNIWv0hTp2kZMjZyXlrjAKUbn9+JST48jaKLM+CtR1pg2GEM1s8TsF5NRV8TnO+EIz1DfGL4B+MUYa74maUTNjBSaH"
    L"RotvYkw5S79wf/7i53zXSy676priK+Xh/yKa3fL+/XF48KB7wf7HfgZ4/lV/9PFdF5yz6ymjSeT4yRMxZTFG1LjSUlJgk0Kk/Wqfc3j/chwOh2bmQ5df+VvD"
    L"qz5+28cuPz468bR1P/7usU6M91OIUJV9QhoQ4hzOtJhIRxZLhjokbqsD9TTTLwskFciM+ZCBNubukL8AjReaAG1UfMh4H1BNpFCT2oYcPSl6UmxRTYgtiFYI"
    L"PmMiqBfaEcwtLl63srJir9sx0vttTnioajh0yCzv3x/f+u5/fHow1asw5RNOrm3QtuOsghTWxO3bt0vyo9976Q9d8oKDB9Xtnw39fTWyb7jPHV4+fPp1r1z5"
    L"+WfdfPuRfZ/89HV7dm7d+6QTx9fz5vlHbnrYeT/pckSrXl96paNnDaWBXiX0qu7coFIs5O5Yy0hH6G1DxEdoWpg2kbrOTOrAaDSmqQOjtRHtuGEybainbTfo"
    L"pxEpC6I1BB+grbNTY/pl/MefeuMLH3r5o4l3xQ26T4imyzPK3nCo5oXPkPddddVV7y/3PvHntszb56TF7Q9LoSGEZDct7jQbx8Z3/kuedXj5cEQRljCskq5c"
    L"eu27gHd1/eGPLi79+FI+e/sTH2STOegLuyBaqAYrCUPAYDuCKCn6bmLmtBfuUMvG545X5DN1m6lnpy82TWQ6bWnbjtqSQsf6065gxpz2LzKrARxrx09OL3+0"
    L"hA6pug/mhO9NhjRruwXgtcBrr37PP/5s2e/9tBAWJyc//95BOPmrM1r6186x7PCIdGqA+6YP3GSuvfraJCIbM2Djoy9/040TW21ZDJOomUyshKJI6CwWOAuF"
    L"+yJoAyHNArCPzE5cDN19GvB1wDeBNKvCG9/iYyBZ2zV/rKKmgyhUwMeWnFu5u8W/z1zQVyIoraxglg6QEdFff8/Ht4hWg5c846JbuT9lODQsL+vwqmH/ztH/"
    L"eRNmx644naiUTsrSUZaWokg4K7jC4uSLF0CIQbtAmyJNiDR1S1tHxqOGetLQ1jWhjkymNePphEDuiAdGyFZQsRASaTLK+GS2Ddob/uHQyy6erbN+HRXw5aOu"
    L"pyYvr7j00nRfHHJxV4oH0YMHD/b+8CPbbx5NFnbGNmrRK6WYKcCWirGCc4bSfIHegkLwiRA7BdT1lLr2tNNI00aaekoznhKbrktWe080EAshW4O6rnDTJpDr"
    L"SY7T1nzb3uJ9f/2ey39glm3m+6wO+OrS1a7tOBwOzfL+/fH+W/wvyKWXXkrfuDbWnhgMdR1p6si0jkynibpO1HVkOolMxt01GgVG45bx2DMeNUwnbXdNG+rp"
    L"lGba0jQtbdtRXTpcCzAGNV1fXDWjKRJ9yJUraKej9wHs23fXbOqvy3lBs0XXr8OT9LLLrimAdrJWvbuyiy8djU9EW5gixoTx0OsLxnSHorsvYtehM0h9ZgG+"
    L"9d0VOv/fNpG2TYTZQU7ZCskZkgG13YhQagNoQMimZ1Pc9/jvuOaav4BLL70iHz68zL+KBXy95eTJm7KI6JwJf9BurJF8dKFNhCbh60g98tTjSDOOTMYNk1HN"
    L"eFQzHk9nP3f3etRQjxvqUU1TN12zPSUySjRCdGa28KZr3MSuRWqTZptKmZ+rP3nla/ZfA8O7HUr/plPA6upSOnBA7X967UV/qXnt3Zae+klI7bTF14HJesNk"
    L"rWa63jAZ14wnDZNJw2RSM55MGU9qJuMp01FNPepO0ppOa1rfkkIgC13APRV4rSFrJvqA+kxufJ4rjVzy4L3vFZF02WV3f7riN+WhfQcOdPcH7i2v2jQQ4xuv"
    L"sfVMxhPqScN0UjMZTxitN4zXp4zWxozWxkxGE+rxlHpS07YNPnh87JgOOXcZT7BCsIIWluwsSseeFh8wweesYnrFxo0Pf/juKxkOzdVXXx6/5RSwtNQVhc/8"
    L"7r1/VXLnX85V866pQ2onawTvaeuGpq5p65a2bgmtp512O70bMGmYNg116KZtomSiFVJhSYUFa8gyo8XEjMSEiRH1kgotzLZNvTe88vL9d+479EX12beSAkD0"
    L"+utX5dFP3rp+3p70ivlqNA6pzSYkbcdTfF0T6pZ20qWWzWRK03T0+qZpTu/+kGJHELaGVFpSaZDCIs7MFj9C8Ghoyd4HzVr07Yn3veRHX/I7+/YN3eHDy/c4"
    L"JfNNqoAuFlx21VXFL/yHx1x7zvnpyrN3bSpS0kDwmtqAb7udH3030xZix91R6bKaYCA7ixQFlI7sDMkKyUoHX6QMIaGth1aTBl/s2rJ25/N/5Dt/ZmlJ8s6d"
    L"19+rzM/yTSzXvuc9ed/woPv94eMPf/BDd+zeaM13jScNObVobjqCn8qMP5pJkslWyM6QS0e2hmiEXHQZT8yZHBMmZqSNiI+Euo3S4Lb2/LFHPXj+ib/8uudc"
    L"350vtHyvZsSEb3pRgSsElvNlL//Qmz/5mZOXbRzPpcY2qlOXuiE8sk0kmzpCsQFKy6zZ1sHQsy6YxIyJGeuThqaNVk2xfRDu3D6fn/gXf/JTH5+5nnuN8n4L"
    L"KOCUElYNLKWX/NQfPemzR6qV247HLWO/kcVWqqIml0nUKVlMN82T0xdNdGbICemG+rLGlLVu3ULpuGD34H897ekPe/krLnvk338tZ+N9iyjgVHraHT32utd9"
    L"8Owbjtzx/x+5NTztxEkIuSHQaC7I4npYU4hoMFJYmlgn2tTNEqfWCiU2K/PF2h2P/66Hvuktb37662dv/jUd6PctpYAvVgLAq4ef+L5P/NN1L7v9zvpxdSx2"
    L"tCkQszCZtqhvKMqConQYDXQtj4bFufkPnrWpet+zf+DhV7/oRd9xDJDhcHjvDyP8VldAh1oPzfLy8uks5a1vv+WcT9x44xNDXe/72MdvSqWbf/T81oWHt/Wo"
    L"vvmzd77jnLMW4gMfcNZGUS7+9q/84v6/P/U+X62/PyNn5IyckTNyRs7IGTkjZ+SMnJEzckbOyBn5V5b/DYLeM4xFt4qLAAAAAElFTkSuQmCC";



// ============================================================================
// Network Location Profile Icons (Home / Public / Work)
// Mutually exclusive: only one icon is shown at a time in the flyout header,
// based on the active network location category detected via NLM.
// Source: imageres.dll / pnidui.dll (Windows 7 style)
// ============================================================================

// ID_0 - Home Network (house icon) - NLM_NETWORK_CATEGORY_PRIVATE
// Classic Windows 7-style Home/private network icon, downsampled to 64x64 for compact embedding.
// PNG RGBA 64x64; SHA-256: 27448081d0b495754e822f2ced309856d61b167b9bbca8715e8160c63a52a05c
static const WCHAR* NETLOC_HOME_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAJxklEQVR4AdRWe3BU1Rn/3cfefSe7ZDe7YEJICCTBEEiCQTBGkgzaovUxNUqlo45anFCt1VFH"
    L"rSjVWgmKT6zvqaiFGgwQIEEDKhW1hDThKUmEBIQkkucm+7q799lvV2fLMFF0+kenZ/fce+453/2+3/l9j3NZ/I/b/x+A4QcvXt95Z7Fef122/shsh6//9yXz"
    L"/hsSfxID+ppfzmUUeTGrBLCxpRuv7R911HaOmZeX5c58eEHOXX8oz/loeUVuiHr9jwX1kwAwd9Q1d+45Vr2rowcmuxNV87OiAxE06JzezLHMc2ZoFQ5NtDjl"
    L"wJUvX1mw/seAOCeAKoB7qDz3sofLc1Y9XJnX1sCn/OW4OgmZqS4UuyzGGSbdNNOimWeaNUy3sfAk2WG12eAfHlz84lWzN50LxA8CuPuyGROmXzrrC4Fh3jVF"
    L"xPvcqlhYYpSZaxwsLjEqyFHD8HIqLJoORtehKho0WQbPMkhOTkZocODqNdcU7f4hEN8L4MHKwhlOo+NYefUDJTf9+RXXAl8fik+egFeLIhoJQdFkBCQFig6w"
    L"YCCwLDgDdTLO0jPLaLA5kjA2cLr0+UX5H6+IiY2DhB1nDg9Vnr/YlZ5+cPl7G52zKi/FlFk5yKlrAUwsogN9YA0CjIIJNosVJo6HgQcYVQZLaAwcAyNNCAYD"
    L"OJaDw+lEIOAvT7m68LPaqnlmnNXYs57xwIKcd6dffOm6Javf4I588h5Ch3biwLa1cGVNRPbf9sCos+CHvwHHcRA4je46zCYeglGAycCBSACrqRB0hsAx5A4W"
    L"TocDoyND805L4b3PnAWCxXft3gKP9aGFM7srfrt8ycKldzLDzeuQml2IjAsqkeyeFAfhyZ2M9Dd2gQtHYAwNwu40w2rhwekgowyxoIMj+gWOBU9McAwDaAo4"
    L"VoPDkYJQKJCv+IZa/3rzAge+a3EA987PuNwxLXf4hmfXZeYW5sLXthmeaXPgyi6ALEnw5JUkQHjzpyHt1U/AhUagD/eCNxhppyysZgZGngEPnWJCI/U6OJ4n"
    L"VsgVDAtagslkgSrLecMDpztfryrJBDX23vmZL2UUlW675c0tRnPka/g7P4U3pwQpWedD8g1BDY2BoYDy5BUnQKTOzoOzpgn60CAMgSHaIUs71UgOMNDuLQJP"
    L"hjnwtHsWAEsZwtOdJ0CCLQlKKJwqRULdW+9fXMlOyr9g2bV/egnBA5sgDZ/A1OJKMj4D4lAflGgQmi5DUyXoFPXu6QUJEJ6ifDgeb0Sk6xQkAsIyPMwUfDwZ"
    L"U2WV5PV4arIcA4IGVVfBy1FYlTFig0F378C+1n0HjGzhgzXmrqZXG6FEKNrLYHKnIzLcA10OA4pCdzHeY+usJsE9NS8BwnthEdxPb4VhaACqbwRRWYKqknGV"
    L"TFJG6FQfVJozaxEkk2E5JGLYPB3+jPnXrm4bLFqxo72RLc/MjKih/kUmixtGhwvB3mPQIkGilBTJIjQpDEYRoStRkD/iY09Gxn9AXHQB3CtroZ3sA3yjoBAg"
    L"ORVyVIRNj8CjhKCJEtqj6ehJL4d77hw8tnZ9Hb5rMRfFh85J54Ghn81OuW21UiBJ4MhxsYjW1SgBiECPSgSA7kSlK92bADGprBQZz9TCHhyFFBiBRQ0hXQ3Q"
    L"WEWrlIZDaeVwls5F6VwL8lz9qH2mKlEPEgBYjoMmM2jfU4/IqTYKojGYJnCI5TUUCTwxZJkyAWZvMizJPKjsI2ViSgJEWkUZuudUIaNvEKJfwQ4lC19MWQBL"
    L"6RyUFgsodJ9GrteASZMyYeO9WnzXdGGpx/9cjGIavbX1K7xQdwgvv/kRXlm5Bro0BqvVQCvA9TfW4MU1a3Hk6FEMRkdhZhWkpCZjVsV8fPnPDdh0SkdDehla"
    L"8iuQtHAeKkoElKQOYSqdHbkzSzC1qBJObxr2jhxS4wrpkgAgiWP0CHAp06C6ZmHvoBPP1XdC1KMgJ8bX2ro5bNjlx+XV7+OKm14gXw+BtgttdBQTrHZcMpPD"
    L"xEWFuOQiD0pTA5hs1TA9vwR5Fy6CzZUOWY6Ao4Bc8fwBa1whXRIAImIALM/Cd7oTTQ112F7/HjoPtyKqyCQmUQdSz/Pi+usuR37JfPTw08BTijFSEBHKgIgY"
    L"RXaagPwUEW6DjImTM5BdvBAWdxb8AT8CAR+iQT/kcBAes3ORp6DAGlOaACAGx6AFB7Dm8Xuw8eXleOOpB3B/9Q2UuzoQiYEAvjrRj927d1ONOAa7oIJiFgzL"
    L"go4HqBT1Rl6H0WjBhNQ02JIoo8J+hHy9iIb8EAOkn+pJWJLB8GxOiiKcD4Depmvsr1CEjx1vxmBXK1W3LlxRloma55+gYLQh7PPHRJCRmY2mw2Ec7NPpvHeC"
    L"AgRyNApNVyBL0Xgd0DkDAv4ARqk2RP0+RMIBOr4pO8QQjAaFipUMnudMLKsFSKmWYEAlJRxnpDkVobER9HccwKnPt6GvpxsiFRVu6F945FdWcPaJsKYVQae7"
    L"Fo2A4VhwREXs/TAdUmI4TIRJCIZCCBPdkhgmkDI8DgGifwQ7m7vHWEbYcPhwWzsZQwJAkt2OJJeDVAFR2lVUJeoZFgJvJMQUb645eHRtNyxWCwzke7ACWAML"
    L"mSpd7MBSiQGDIEAjDSLFQ0TTIJIeA9USl53DiZPfoK5x3/a3d3VMPtl1qDVmPNbjALrau6o6O46g6/hx2G0mTMnPhtvlROxjQ6caLoZDMVn0R7ww0onG8Aby"
    L"OweJGBCjKsKSApUMgmHIHToUAq+qCibYBOhUxP7R3B7euefokqU1mxdt3978rT/jGvEtA7esqn+/9eD+tB2bt924ub7x4y0btgSPUq7bkgSkuuzwUo/JK4wF"
    L"MmOiamsAQ8RztN/YDk0CR4a0uHGZYtNq4eBONqDv9Ag+3N2xpXfEl3bfmsZ1GKfFGYjN3/Psx73Vz334zpJH/l65767Xk5v3tlz/1tt1L23YuP3Yl+3HYyKo"
    L"yLdgNGzGcNSOMMzQ6ZBXNQbhGOXkApbR4XEaYGQZtBzsCbYcOXXz0pWbrlq2ssEXVzDOJQHgzLUVgLa0prF22eptd4T6js1Y+8H+wqbaXx949fYNaHm0Dk+U"
    L"7USW0EcV2givU8B5HicmptiRSl9IPf1BfL6v54Nevy9r2aota8/UO954XABnCt7+Wqv81Fs79l9195ez77xvIKvjs6P3X5Ry8NGrk/f+8Z31uxoaP22TurtP"
    L"UNT70XFijOJo6LZbn3z/5797cvvgmXq+b3xOAGe++M7WzuO/uKfzqYIbWx+7ddWWFUtrNl3R29eb3rRr78/2tHUtESVtym9WbX7zzHfONf5JAMZTVv1000D1"
    L"6oYPb6+pX3fbExu/PlvmXM//BgAA//9XJbi9AAAABklEQVQDAOY6Rn3kHfDOAAAAAElFTkSuQmCC";

// Classic Windows 7-style Public network icon, downsampled to 64x64 for compact embedding.
// PNG RGBA 64x64; SHA-256: 113c9bc77b957ba3836f01c61c2ea0ae24493cbdd05ce55e15ed2ae1f8219630
static const WCHAR* NETLOC_PUBLIC_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAKRklEQVR4AeyTaXQUVRbHb1VX9b4kvSVmIQRE9hEFEhRwQBQyCYswQhQOyqpsihBlVQgHXAYi"
    L"oIgRDEQQyCirAVwAkSEQEpJgWBIICSEkTdJLOkt3dXX1UlXzXh+aycyYMzNnPsyX6VO/vq/eu8u/7q0i4X/8+7+ATjvQejEroeHoWyOq982Jc14/Fu8Iimer"
    L"HGLR0V9urs3MzOw07j+daKeJJM4bLwmWwrMS5u7F1qKdPzHfrxjJFW9J5m4dWzSld1NKw4+b9agYgfivrk4FuOQ9Sp2C6XCbO2hjWpwxnquHQXn1Uxjk2msU"
    L"60tOsjX5FdU5aedu507ZVbPv1dXV+2dPrsh7Lbkmb0H81b3TVR1V2e12dUObf3ZFvWtCyTVrt45nnQqQ0nKjKFU0eOQx2e0xYw/Ze74NpcoXoYLvV+ClzPvd"
    L"HqHV5bAmuy03ZzF1pRu4htJvRcuv5322WxeoNtsPVbnpuVV7Zqyo+CJ1Alv1/WIx2JpDUtTROpvtk39LANF6K0nlqlpi9lXvjrTmv2Kw5YNZaAC5lOJ4pbnI"
    L"q++7iYsePterf3weo+m7hpH3yGVAV+jxeIFtb36KsVTO8DZc+ZBwWY8xxdkb2vNeAe7kPMJQ9UnS7b2vrq7cN3diYe5r/TvtgJs0HrOqk5Y2K/u/59H0vcWK"
    L"MojgLRDtq3yetv+6TeMo2q1svpyl8t+fqZQEexFKTV1A0zXPYxq8nDElz2QNTy5ya3pvbKPiD7h4rYNj3CCxX4EYd6EZdWuDYCk+4rPd/bxTATKfdb7SVbmI"
    L"4ByPBSijnzU/C8XBoWIVPXi7x5T0Vps0Mcfll1a425hYX3PdFNJ6ZZ3CdnmHxnZxl9pVmUl7bamkhJSIKtMFT+zouubus+G67iXxDp2cy2r7ZrklpnOgMF/t"
    L"VADjo6pZTvAFnXXp/I19A6mSD6C/J58weq6niS77SJGUOgOKRw75I/qsYKOGzWK1vZe6FAnbGInxLIsC+famFMJ2NYO2XPw8snrXYNOdHIhniwgVtMcFaUWL"
    L"T5WYB9qEnE4F5NV+uY4l9EOI+NFJsuc+LHd3nw5WIpH3cxwdbKlNAXv5contynaJozRHYi9dSzCNo4CgOL/C/GPANHCd2zBoOmNInsfqn8h26Yd4GFEJMk+D"
    L"EOGtfV7rvPKBrPH8Dq61MeU3Bfy0aboqMxPE1MwfmHaNqb5n2gx5bOp6qH7kdY7tkZ7uVfcaJkQNmRnQ9dgsqGPP8wIvEdiWNNJR8Q5tLflMcr9gj9R5bT3F"
    L"1I8D3i8XTY8HPN1egnLVOHujauAbbm3vzGBkn4OUNur63wm4lD2ja+muORk8ayk8tC7luwPvpi3hvb4VDTdqHnPcvwMUKToyNuwsS3v38NVRS77aP3rVieXf"
    L"XHJOtHujhnqjhz/lNyfN9SniPwvIowqCAsgDLmsad69wJl22MSLi+hboyZ42qLz1k3nOY+QVUQVCgC8JCbj5bcbAS9sm5bRZb/9yq/xS1n1L0++cltpx7baa"
    L"rIbCfcsubptEVn2zCLSNR6h3p/Z7fevi1BGZ81Ji0fdM7TxeJry8Mb859c3sstFv79uTknl6yZd/bhzjoOKHc7qezyqSF+5oj5sITioO/H6fSPsdz8iYe4sE"
    L"a9kWt6ulC2nLX7iGaygubK6vnM0xbV2lNAUKlRRkUhpklAQUyBK8F4j2WuDrC+LYpttbZX7LkThp6885S4Ye/WrV2DU5mVMnffTGmD5zxifrAYA6WFlJTMs8"
    L"Ypmw+siFfpMyqmLHfQT0qI+hqefSo6wh6Rmv6tHlvCpmc4S+ewPp4uCuTKX7untiYnHXWGMwIZKHbkYCuhhoMGkkoFFSEKlRQoROC2q1GgmiQPT71KzL2c3X"
    L"5khhmqpW+hrK8nQB2+kh3en8r1al7sjOSFu0ccGzI9bPH9/NbmdS1VoN0CojKKMH3HjuzZyiEcuObX5+ef7K5+audpA9pmw/0Gfeyfm2iOEztJNy62TD18B9"
    L"ohf4STU8YtZBj2g5dDWSYFbzoFcRSIwUItRSZBWgUctBIZcBJZGAGPSa2FZrEtNU8wow97J0ROuRWFnzqcIvF/y+/Nv3wFL2HbC1PwYXvzxKHxcHNACQGPxH"
    L"oAUxcuqqe3WNvmV2xZN7aiJfbC2QpMNhxzA45x4EjdIBYEzoL8ZFR0J3EwHxBgp1hwSNXAStggQt6pJOKQNDhAq0GgUoKJSWD6oYNGNP/WW6uWQvsEWbwH/j"
    L"63cGxnI/bXh1/LaVCxdqAYBAnoB/BPojRz3d5+zat6avfmH00Nqx48YDaLvBNSbB2xCRtqN9wBq2ImoR/CqfxHnlsQeNhsgLCTFRjniTKhinp8CoFkBD82hE"
    L"ADRFgFohgWiDGowmA6h1elAoVcCzjL7FYesr1Xe5e6qoKIhqSkj0h4uHIadNmxFFyxQm1FWgIABdo9Qlf1r7Zk6A856URXaxtGgGfDd62YmsQRnnlm76ofm1"
    L"EqtqWTvdZafGlFAQFxft7KInIVEfhBg0MjUdBCUtgEpOgkoqAqU12Vhd71lTMz7LLisrE1FtEgtA9m9XQrdHjRKpIpLnAX02QZDJyNKmpib/y5NGZx3YvWX+"
    L"xhWztz3wJvJOlTe98cmZwrErj+1+Yc3Zla7H5v4sRW871W8G+JSJYDIaINGsgBhNADxBCojfLahc9enxsyieQuDaD0cgog2MoFRq0BdISwBEoKRScLlctegs"
    L"6HQ6uaOHvrHY7XYvuucRIf8HFuxub/DImWsnLKz+BNM13VuiSIe/SKZCmXwytBjGANFrGpD6Xlqd2SxDMbg4JiQAJ8II6IBXKpVAUiRqGAEkmhDj5pwA4PsH"
    L"Aujej8BzxGBBwqcfv1+Vs3V9tkz0towYNhiMcT2gRd4HmMSpII8fBoTfxRN+P66D62FCnwJeYPCBwAs+hygKdwiSBH2k7kzTveprqBD3APz0eB22YWFYDCYw"
    L"fPgzJrlCFalCLx2IPOik3GngHNvktFDMsa25bW1tWDyuhWuKuA14gTcw/B8npF6/UnT+RdHvmnK7vOj1gwf3N/1GcSwAw6IzbDFYGNe7b3+zXKlSkCQFPC9A"
    L"a4vtePrEMdtXLZ0za97M9JPIH3crjIAFoD00cICQAJvN5h87ZmT104P6Hs/O3mpFh/4H4KcNFUH3uGBHHgrRm6N1MpmcEAkSCCA5p9NRA2iElZWVLmTx02Me"
    L"jo1Em7gDYbAIDHbAYGdMWAS2WEiYsCBsQ4LUWq2cROMLCjwIotBy8+ZN/BLjcxyD43E+3AFcJ9SBjsXxOnSAhGGLHTFYDAYHh8HJwuDkGE6pUCoEUYRggAcp"
    L"TVdeLjjTgnKFBWAfHI9z4bwhAej84YUFhBHQLl5jGwYH4eCO4ISYkBhXm/0A8L4vpFLiFCF43rdarXg8oTOULxyH82BCLyHa7/TCAsKEReD78BonCScN2Sf6"
    L"9br8aJxu8cBeMeOmT/5DIcqMxWGwLyYci/P8SwEo/p+uUCDa7ZgIr3HyMFhMaI38OlrshwnnEP8KAAD//5R9kyoAAAAGSURBVAMA/JzslxJ7OeIAAAAASUVO"
    L"RK5CYII=";

static const WCHAR* NETLOC_PUBLIC_OFFLINE_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAHk0lEQVR4nM2XXWwU1xXH/+fcOzP75W79BUK2AjaFlMZRolKR"
    L"hoq+ICSSxW2kttAIKS99qJAqpD5UVYTUl6bKSxrJD/2SKjWoUlFTWUICiqAlSRXioAKxAi3BGFgMbLENa7zLzs7X/eiDd9xl"
    L"+WhfovRKR3dmZ+ac3/2fc8/M0tjYGD7LwZ9p9P8HAPmoC7t3714dBMGQUupyb28vdXV17Y/jOFetVo9WKpWfTkxMmE8VII7j"
    L"7zabzdeJ6Mbc3Jxfr9c3AEC9Xl/b399/eufOnafefvvtuwDspwLAzGeMMeNa69UA1gdBACICM/f5vn8kiqLZUql0iYguM/NV"
    L"AJcAXCeif2mtFw4fPuynvvbs2VNg5l1xHN+J4/j8/v37r/4vAH3MfAPAn13X/Rozf6/RaCBJkvdd172eJMmX4zh+zhjz9RYY"
    L"iChm5lkimtmxY8cVIppKkuSTWq02UiwWXyMi22g0jgAY/a8AcRxvSpLkh8yMIAgSKSWICEKIUAhxSghxgpmV1jpnjFkBYMgY"
    L"M5QkybC19vkoira07sf8/Dyq1SqEEKSU2jQ6OroPwAWt9eVHAlhrDwohbhJRXgixU2v9dOvStiiKthERiGiemcvMfIWIpoQQ"
    L"pwDUrbUSQNEY84S1dtAYs80Y058kCYhohe/7rxERjDEnHwewRyn1HDN/4DhOnMlkUKvVLDP/MpPJTCdJMqK1XquUWgdgIxFJ"
    L"ACCiJjPfIqIpZv5ECHFSSrmOiPqDILDGmLeklFWt9VeklP98JIBSalop9ay1dpfv+25LTjLGlJIk+RjARSHEJBHVhBCste4z"
    L"xgxZa4eUUk9aa7cDeLEFBSEEaGkMMvM0ER0QQvydHtWKN27cyPl8vpDL5YZyudxbjUbjWd/3NYBZAH0AvJbzUAhRIaKLQogL"
    L"AGaEEIvGGAugC8AzRPSKUipvjDHMzEQEpRQcx3n1oQps3bo1f+LEiSaARqlUuj44OJhpNpsol8uh4zi7giCIHMcZUUo9DWDE"
    L"GLNea11KkqTUgtLMPNNKwXw2m00AoF6vzzPzz4QQvUT0lOM45+8D2L59+xpm/laj0Xhly5YtM8aYv2mtV9RqtfVRFAHA7YmJ"
    L"ibP1el0D+BiAXbdundvf3//5XC73hLV2JEmSZ4hoRGu9XilV0lojCAIIIcDMvVrr7wA4J6V831p7msbGxvDSSy9tbDabe4Ig"
    L"2BqG4RqlVHsxLufQdd2b1to3lFIXlFLTk5OTs63bDP7TEe2qVat47dq1g0KI1V1dXbuCIPh+HMew1sZCCLflVzPzV6lSqfxk"
    L"cXFxn+/7rrUWqRljYO2ST6318jEACCEaQoh5IrokpZwkonNhGH5Sr9dnp6enay0YC8Du3bv3B41G403f91Gr1f6otf6FMeZ5"
    L"Y0xfJpP5uVRKlR3H+X2xWBxRSm1USsk0aGpCiKVlGpNaQWtdIKLhKIq2tzrh7e7u7vLmzZsvKqXOKaXOW2tnms3mi67rIooi"
    L"5PP5f4yPj58C8GEKSGNjYwIAj46Orl25cuWhe/fufWF2dhbWWjiOs6xAkiTLyrSrk56nx21t2RdCVKWUq3K5nMPMsNa+WqlU"
    L"fjczM1P3fV+lABJL3wXi5Zdf3k5E35ybm/tGGIbdSZJASgnP8+B5nlVKkbUWSikopR6ASWumHTSFatXRgpTyhhDiTBiGP/7o"
    L"o4/q6QcJAeADBw6889577+1bs2bN1eHhYTiOgzAMAynlb3p6epqu68JaGzLzn7LZ7MlCoXA7n8+rbDYLx3EghEirHVJKZDIZ"
    L"ZLNZuK4LKSW01j1BEDzleV65UqkoAEK2gqfGGzZsWCmE6GdeYsvn86cnJiZ+WyqVuj3P26yU+uD48eNvArBDQ0MDfX19Q/l8"
    L"/ovZbHYDEX1JKdXbvnprLZg5VWLOGPOjd99990irBviBRlQsFvuYuTt14DjOGd/340OHDr0xODi4slKp3E1VK5fLt8rl8i0A"
    L"E57nyR07duzNZDI7fd/H4uIiPM8DACil4Ps+enp6Lhw7duwdLL2FDQBKU5BuGyOl9ESr7JkZcRxfBaCCIAinp6dvNpvNAIBO"
    L"70/3fxRF6tq1a4fjOD6cz+eDtHgBwHVdFItFeJ73uWw267VqjlMA2+ZMO44DInLbAKoAog5LAMQAVMs0AHP27NmpycnJXwFY"
    L"GBgYQKFQADMjn8+jUCjAGKPN0pZJY0J2AJC19jaAK0T0pOd5f52fnz8HIAQg0t3StgLRqp105oGBgX4pZTcRAQCEEH/RWl+U"
    L"Um5KkuStKIqSNuWsbAsOADh48OD5F1544duZTGbDnTt3zk5NTd3tCMgPseXfe3t7VziOk027ZxiGh44ePXq8t7f319Vq9V5L"
    L"rdRMWoTL+Ww2m2Z8fHwawOU0Tx1z5/F9ELlcriiEoFb+wyAILgOIqtVqmq6kPW2pAumwLecG929Pwv0gD4NiAOy6boaI0oa0"
    L"sLCwcLWVwnTVy8E7AdLg6Tk9ZH4cFAMgKWU27YTMfKFSqSy0Vm3aA7cDtI+HqdEJ9DgYiqLoD57nMTMPA3jd9/0m7t+y7TMe"
    L"+Un2mNGpTCdQ57X2hdkOe/T/gscM2zE/TqXOZx54/t9lcT9A127FpQAAAABJRU5ErkJggg==";


// Classic Windows 7-style Work/domain network icon, downsampled to 64x64 for compact embedding.
// PNG RGBA 64x64; SHA-256: aa799df96ae1d1e683945c367f3ba4b1c776f0c719af808b7b3e9dd47442c81a
static const WCHAR* NETLOC_WORK_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAIcklEQVR4AexWe4xcZRX/3e8+Z+7M7Ozs7OzOLrN9UErBwgZIMKBACpqIwaB/iImJJmpSkRQx"
    L"iiAlIhKpgjxqCyVosVQKuhRpaQVa267ddsHtY7ev7cN9tdvZ92te99657+vZjTVFEmxXE/7h5jv3u9+93znnd37nnG+G4WO+PgHw8TPw+FvHala88f6ipza1"
    L"Xbd8w57blr/cfNcDa3cs+/7zW36xbPXmF+559s3N96566/C9Kzf2f+XxpqY7HnnlvtuWv7rk5h/+PvP/KB+m2uNvN4TclpTktKRlb2eYs5sCfWq1wSIPa1L1"
    L"3cNS6s6SkmpkqtxwdO4VdxUT8ZVSFM1yTWXLrctf2vTFh9f95PLvrbl6tmCYEDhXXtPYmMosvEqNzL0K0br5iKdqUTPnMqRq61FfW4d0VRUCUUFvqArtkRTe"
    L"jdZhR1XDvOaK6i+/k5r3S02fvHvWADwwy5cU2EwEkwRURkTEwzzmVCloSIYwPyGjvlICh2DGR3XgYeZxekk6YDJK/vTLmc8XfWO6YWDwTDdGs93wps6CMyah"
    L"+GWEjCHEjLNIesOo8seheiZuz/dhsZXD59wCbrBzaPQ0zHHyiLDZ1zLLWR6KgQRdiEATVRiCgjJ42KEY+IokpMoU4ul6WGIYLUoSJ1kInYGAv0NEp8/Q7wOO"
    L"71505OcUmOYxjIfSsNQUHBJPqYAnheCpSdjhJBxaB5IK8AIMJQZHDGFEjgJiBJ4QprkC47P3DyZQQu3SJGDkwRs5CHYZvOfC0fIkU3D0InStBEZRLrA1ZAIb"
    L"18LGPOZgseAhzJlI8OwcBG7F+p1Vf9zcknnxta3JR1dtiD366Dpl6dIXRYqYI/nQYLbjwTRNFHUdmmGgbJmwHAemUYJZ1lHWSzB0DTzVWcbVobg24DowaN+w"
    L"ZcEwHVwhGDd9Y9n967++7MGDp4+/1727raXrRFdXr8h7PeFMqCt9dajzwWdear9vxer3lz78RPOtSx9bTUhmADGf4yEn6sAn6uHE07CVOExegVlRDzNWDyNW"
    L"B5taT6ec/y1Ui0NiFB28ilFOwaRmAJoGy9Suy5fK35zKF66dLGhxNVIp1damI3JITboelyGSF5aLhWtGiu4NO+3EkmZTXoJ/XUwiamNU1RE7j5BdAufZgB+g"
    L"ZJgouz5sKjKL1hxjpOKDkbUap4wq18ACmUeNKqPkBvqZkl0eZarDFn2mrDYssOKZS826uhp7YSblGh78Y0IaTZHLoCsRzBOmnZA5GkwgahOUz2qUUeeXkHTz"
    L"CPsGuOFusPHTEAvD8IsTEMnpZ7VhfNqcwvWU+8vgQBYEjDIJAeMkSeAlh5fEXqrMwUBVctULQuH5jVL9wk8JQTzN3FgKt5tDSJt5nHZcck2R0J1Nx5RI1SGW"
    L"ugRiMoOCmIBJVa6HEhjKGWChCERZhiBKKMgx9MpxbOVUtDEVx30eCHj4jk1M2lwisHAN8lCMSS6X7eay/b1c7+A4RGMKtVM9OMXHcJjJuNIY4QAoJIx5roUI"
    L"V4YQuMhqLooeB9P2YDERectF3vTgESDN53BMVCn306kIgMDHPFdDozkGXhD5MhNZnsD0OiIKpKdTzQzqNvomC7A4EVn1EkQJyJJAg+5hLuoW3wl1borcBBjo"
    L"OomOw4cwOdCNgOjmCZRM9aASzVMTY7C9AIwXAHo/h1rxxsIQPl8eQUKScSRUNfNdJEa5IIATcCgQwyWezpKKNMZCKRQJjFoaQl1QxqlAgZwfzELgB6ELBpvy"
    L"ePQnroRZPR9SOAY1JEOCh1rORJrTwY31otR/AkzP4wsTPUjTkaxFq7EjmkE7ZGIC1KI+BPiIcR4uJ3fVdExX5M5CnsqCK4yB0RwyC+iINqC6c9d4V1/fc3Ds"
    L"LNBjsQmieMQEHKLZiqZgCCpMKYKRyCUYJCd+/SJYFA2vhHE4tRCHQkkcpeMaFO0V5UlcR85AxWgyEXlORDci0MQItW4SoPqJSjw4KYz+yvnI7HuzePTosZVE"
    L"WSuGJyYBOCzsW0iMHQcGTwD5EXDTHUKdQSkHI6eerMJXKzFdEyM8FSNpNeaHcZM5TvYr0R6updOyFMj6lK961ky36KRshhKBk5yDYjwDM5xAsnefeeTI/nWk"
    L"/g5MYwgYp7DhM4MKh9VeingiiYinQynnoVpFxCb6EDEmECbKJS4AR0W3OJfF1YVBxENh7I2k0SHEAGLNGhuYamvd+0Zn665Vwy1vvjzS8tbbZ5qeautqWtVd"
    L"PLBtMndwm9Gx/c+vSXLVBpQGziJ/Ricg9LsOMIvjFY1XkVNSMKrmw61MwwtXIFyTof8GEVRxFiqJ5ohTRg1cjJPDFokcy3FUEtDFW1cMjmT71yI39DXtdOd9"
    L"p5q3fqvjnT/dseeVNTduevqRhX944NvJLevWqIVsz3eMvv0HUSyWyLlLEpCAhcb+caB8+qgtkXGe5+FzAp2AQFGMw0nMgZlcAKuSaFSi2FXRgCyTAFHG9ad2"
    L"OcGrT+7u7B38MXhl/bSxC5R/O5/ez0Z69n9p73M/u2Xfk/fce+b1Z97IHdlT4CiXgsDA8QyMcTQTPN8HRBEZHli8+enRjtd/tyYvRH8KS98Lf/zstLELlJnI"
    L"z+1l7Tt3Ft7bvbNt++aNz7369M+/+vbLzy5q2/DCnb2v/erJgXfX7jO62/XANSFFKnDLoS1u//P3t3Zmxx5yJWUt+k6eRM++cYyOls8ZvNiZ/afCiQMHRrZt"
    L"2biladUTD+5Yu3LJoVeevbn51z/4buuLj7XuX/+bVWOIPARd24WT3WdQGiyQvk1C9NB9FuNDAM63MTAwUG7e/peOv27e+Nv97266qXz6+I/Qvq0Vfe2j1EYG"
    L"7f1APml90eMjAXyEtf8p6vPtzhbABwrpfIMX+zxbABfs579t/CcAAAD//yOSCIUAAAAGSURBVAMAJ07tbrBZnd0AAAAASUVORK5CYII=";

// Windows 7-style PC icon for Network Map, higher-quality artwork, 128x128.
// PNG RGBA 128x128

// Disconnected-network artwork used by the flyout header when no network is
// connected. Source: NeTray-Aero (icon_disconnected.png), embedded as data so
// the mod has no runtime file or network dependency. The source image is 48x48
// and is decoded at the header's actual DPI-scaled draw size.

// Available-networks artwork used when Wi-Fi networks are visible but none is
// connected. Its white screenshot background was made transparent before an
// alpha-preserving Lanczos resize to the 35px header size.
static const WCHAR* AVAILABLE_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAACMAAAAjCAYAAAAe2bNZAAAGRklEQVR4nO2XXWwU1xXH/+fe+dgPs+tdbGyDXSjBAZHEqREt"
    L"H6IRaQRqElX9kAY5CouUl6iqQKr61LfFUvuYqhUPlaU+VlTVCkEb+aGNW0uAFRrTSE3oWoqjyC7BxmBnP2bXO5/39GE/sB03"
    L"bsDkiSNdzcy9M/f+5pzzP3cGeGJP7IltjtFXsUg2mxX5fH7VWvv37+d8Pk+5XE4B4K+C40vZ4/YMAeBMJnNQSmlEo1EEQQBN"
    L"0+D7viIiUa1Wpy5evFgAQNrjJLEsS+RyuZCZr5mmGSkWiyAiMDM0TUM6nUa1Wv0BgD9ZliU2G4Ysy9JXXEvLskKl1Lzv+zuO"
    L"HTvGQRCQaZqYnZ115+bmTMMwnObNmw3DuVzOW9s5NDQkdV03jh8/zq7rUnt7O8bGxnhmZsZoa2sTmwqTzWbF8PCwGhoaGojH"
    L"4791HMdnZh2NnGHm7jAMUS6XyfM8EBGax5X20DBNuTYkqlmWFUop06ZpHg3DEFLK1r2O44CZIaVstbUgjwQzPDysVlx6AHD2"
    L"7NlCoVAIBwcHvRMnThjFYpHi8TiPjIxI3/fXBXgkmGZIMpnM01LKDsMwGICUUirP8wbCMJREpAkhpJQSQogN53xomJmZGQOA"
    L"o5T6TTKZ/O7S0lJrTEoJz/MAQGdmNNumwliWZaxYULcsSwkhCpVKxTtw4EAQjUY1IoJt2zQxMaGvCgcrCFLrTftwMGvk6gHA"
    L"mTNnvDAMjcOHD4t0Oq1JKTE7O4vx8fEVucFgGUMtjECg+ugwlmUZhmGMA2i6XABQzPy053mwbVtqmgYpJZaXl1sgxAosothy"
    L"9zJe7nwXf7v3PNQGEdsQJhaLCSHEUSJapYYwDNGQMDXlKoSAYoKCrFcYoaNt8R18qz2P60vP1R8kWW/rbIurYLLZrJifn5c9"
    L"PT08Pz9PANDT06NNTU2Fvb29yGQyKJVKlEql+MqVK2JycpJWqoUZ2Jsu4tnYLfjOIIjikMKHGdHRpvtgaEgU30G7xwASXwzT"
    L"qB1rs80/deqUBAAhRMsDK71EDRAG4+C2+zgeuYw7dzUIcwDRLSZC0Yne+QoS1Ul0zF0A4v0Q/sm6hwJ3NUyzdpw+fXqPaZqH"
    L"lVKhEEICdfUUi8XGm68vVwYgKUAYhBib24vnnrqNwe05FAo3EN0Wh7a9Cydqs3hKvIXClhTu9f0UXLoLCQ/QEqth8vm8BsBj"
    L"5leTyeSvFxYWwMwbVsyVFsokYFaxVNMxVdiDfb6BRLcN2R4AWg1791VgF5NYoO8g3HoElfCvsD0TWmxNmFKpFAOAaZr24uKi"
    L"OzAw4O7cudN03boLR0dHzf8FwSAQAW33/4x996/i54c+wJ7dWyH0GihWAfwloLgM9kLotkKvmoB/6w28hCp69gKf1LYCACxr"
    L"Tc4IIaTv+2ZfXx8GBwfNarUKZsbo6OgXeoUA6OUPkCpfRVeaQa4NTXkQngIcBkICeYyIWIJbKEDV8tilPKjUMyjUHnz+fE7a"
    L"RIQgCOC6LjzPa+UHEaFZTzRNe/DFJgWE1LD89bP4SLyCy3/4HX54KIkX2v+DiMpDSgIcBfiMarUTC+0nQd/4Pq7+82NM3LgJ"
    L"MxIHAORyn4dRROQ7jhPYti1qtVoLJgxDlEol2LYNIoLv+xBCoFKpAABpusmfObpY8DslgjnEEx+GpXu9FEVJ6LAxV+zkHV2L"
    L"YZt/W7uDbhT9OVULDF8ovaWGVTCu68Y6Ojr0a9eu6devX2/1JxIJlMtljIyMPHCppqGrqwuXLl1qAXuej+cT095u/3188q+E"
    L"8d7dOA71fYatpsTb/06og5WIlvb/7r37lx/hZvlgZNuO3VRcutfa9zQAKBQKqhGKW8vLy78HEDBzC9RxnFaomtbYndGozEIp"
    L"Urt27dzx41effbHT+DZGxui9SiIReyF55Rm/Okt9R96Uf5z86O3s61/73hsv1mBMGh//48NPb8S2xG4D9f+oddLx4Y2Zo8x8"
    L"ntn9BTN3MXOO3/8J33krXmPmn8XaEqiP83lm/ubaF1wVJsuyZHd3tzY9PY3+/v7/G2J6ehpAP4ioBuB8o5tsn0dkLXly+5HX"
    L"EkT0KwCSiJrjOHfunHnhwgUPj+OPkpmJ+ZbBfFNnHtcAYPHqL/fz9MjRej8TMxuNc7nRfJsN96UW/C/r/vM0bglTFAAAAABJ"
    L"RU5ErkJggg==";

static const WCHAR* DISCONNECTED_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAMAAABg3Am1AAAAIGNIUk0AAHomAACAhAAA+gAAAIDoAAB1MAAA6mAAADqYAAAX"

    L"cJy6UTwAAAGbUExURQAAAObm5tnZ2dHR0c3NzbGxsampqbm5ucHBwZiYmLe3uJiYlpGRkb29vYeHiIyAgISFhXl5ecnJybq1"

    L"tb1XV7QoKMlsbMG8vMqkpNhFRdk5Ob6Pj8Vra950dOCNjeFhYfOMjORaWrGEhHV1db5fX96BgeSKiuV6eq2oqNKoqPGJifJ9"

    L"fdxSUptGRr8tLeF0dMVdXfFxcepgYMFWVt5hYeeHh5NoaPFpacZBQWk1NbtTU+pxce57e7CqqsidndtVVWRRUUlJSehpacla"

    L"WvJubmVlZapBQfBlZWxSUmxmZupUVPBdXfBPT+9BQe45Oes4OGhPT71NTeNKSuhbW+xJSewzM+0eHukMDOoDA4GBgaKXl81Z"

    L"WegKCnlZWcmRkeQmJuOlpeADA+IqKtFpaa9ubtoCAtMCAtoxMd+enuwVFcwCAsUCAtQ0NNdPT9E2Ns9sbIheXtgrK7oCAssv"

    L"L4ZtbdhOTmxFRURERF9fX25ubtZiYtEtLbACAs1AQIpgYNdqaq0BAcc4OLZcXI96es04OOCQkLlpaYx2dgAAAKkyvtMAAAAB"

    L"dFJOUwBA5thmAAAAAWJLR0QAiAUdSAAAAAd0SU1FB+oDFg4vASZcxSYAAAAldEVYdGRhdGU6Y3JlYXRlADIwMjYtMDMtMjJU"

    L"MTQ6NDc6MDErMDA6MDAnY+fQAAAAJXRFWHRkYXRlOm1vZGlmeQAyMDI2LTAzLTIyVDE0OjQ3OjAxKzAwOjAwVj5fbAAAACh0"

    L"RVh0ZGF0ZTp0aW1lc3RhbXAAMjAyNi0wMy0yMlQxNDo0NzowMSswMDowMAErfrMAAALXSURBVEjH7ZTpUxNBEMWHnZmdIcGw"

    L"G0AQjSFGCAYkB0iQgEdQQTxQUE4VEK8YFQVPQEFE/bft7s0CBSQuX/zk26SSrXq/7p63XcvYf5VShcENzgX3DHBp4qU8Awb5"

    L"TXnIDtI7YEiUaR6ig0N4CUdoKYod/g5wPCl8PXfgdFKzEjr4pFTeOsDlJ8AnVaW3s5p+xpXPp5T0CGAHRfKXSYdrtQ1UQYcj"

    L"5QAB58N4CFAIUAetqkoB7lk52IAIAKDxKgnsnBUlqxknvwqUBaCwAyiLGZpU7QnQynYBqzSAo1swCQ6i8E9Qw6cUAIV9sqa2"

    L"7mi9osI248GGY43H1R6gAsKnnRcwSM2J0MlQrYkEjMQbwk1NkVPRvWlSnH4E9OlQc3NLrLZVUYcz4XhbW7z97EHxmwEAtOpo"

    L"DsViLYlkqxY6ysLxeFsqla47MH6LCaja2RVKxGLnkNDdaaifSmV6zu8DcIkJCMrerkQi29eXTfZfuFj0X7q8b4XwuVosiCH2"

    L"90ZyuWy2LzcQAfeVDPjtq47RgAePrxyhXACl+68BkcukMimonhnsGbKlk5KBi4m7KSh0CDEoUOp653A6PZgbvHHz1u2Rnju2"

    L"FHYRKK4ajq6DAGhORJTdHR27d398YnJyomNoSgpeBGiFzSoYXeDwCKAAmJ55MElqfDhlcm5sA7AxEL8zyW7g0eiEA8wmpwW8"

    L"v4sAjY4dyEYAvd2jc65/fv7xQj033A5UGJaZAAMKK/Qb4snwtv/ps+dITDm7SSIAjRCFMlAvnPr50Vnwvyy8WtDFkaAqCABN"

    L"PgAk/s459fOvu2dmwV8ovFnUDiDIp1yAO4Drb39rTw+8KxSWlpaWF987T1qZ/oBt4bb6A1bUrmQCftvz5P/w8dPnL9Urq+Bf"

    L"W/uaZKWVG6f635y79VXwb5QF2Fge/N3u3cLy2sb3zR/lABYZ2fEDsbW1aZf1s5/hX7tvVwZ+s3+iP163dACZQBppAAAAAElF"

    L"TkSuQmCC";



static const WCHAR* PC_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAAIAAAACACAYAAADDPmHLAABD3ElEQVR42u29eYxc13kv+PvOOffW3"
    L"iu72RJJbWZoWbRleYtly7YoJ3l5zvbgyVB5DoIAyYtjYDx5CRI7g5nJG4rzECTBw+SvwBjDE2T+eB"
    L"nY4iR2kDheZEmUR/bIkWxLlriJNLdmN8kmu9m13brLOeebP+65t04Vi1qdie30AQpdXV1V3V3f9vt"
    L"+33KBrbN1ts7W2TpbZ+tsna2zdbbO1tk6W2frbJ2ts3W2ztbZOltn62ydrbN1ts7W2TpbZ+tsna2z"
    L"dbbO1tk6W2frbJ2ts3W2ztbZOlvnB3uYmZiZtj6Jf/5DP+yKcOjQIbGwsEBXrlzh/UeOMB56iAGAi"
    L"HhLfD+G5+GHHw7/+q//evbTzzwTvKTmEoGZxcMPPywff/xx9fjjjytmlswsDhw4IJwH2fIiPyoe4P"
    L"HHH1cPPPCAfuyxxz5Wq9X+s86yM2DeyLLsZNTvn0yS5FzU613URMtpmq5/7GMfy17J+x44cEDs27d"
    L"PAMCVK1d4//79DIB9RfK/31KAf2EFePTRR/+3xYWF34+iCEEYIk0SpEkCbS2sMZaBthRi2RpziaRs"
    L"G2M61Wr1GhFdEsznMmtXpJQrp06dWvuN3/iN+JWGGgDi8OHDtG/fvkJB+F+Dgqgflj/kypUr7IQxm"
    L"yQJp1mWpVkm2TKnqSbLLIiEkFLMqiCYDRvB3UopkBCQRGAwdJYBacoA+rt37770jSefvMDAKhGdkc"
    L"BpJeUFK8Sq1np1586dg6eeeip98MEHjcMT5iUURBw+fHjci+DHQUl+aDwAMxMR8SNf+crfzM7O/jd"
    L"plploMJAnjn8PrWYV09Mt1OtNrlZrUCpkIRQzBABihoC1IGYWDBABEFJACQGlJAQRLDOyLAMzZ1LK"
    L"TSFlB0BHCNEFcI2IzhLRGSI6Z629GIbheaXUxp49e5IfgBfhLQV4hR/iI1/5yhPTMzPvl4LM+eWL8"
    L"j/8zuNQKsTcjMbCPGF+XmFxWwvb5lvYvtjCzHQVjTpQrylUKwEq1SpXwiqECJlJsmXB1gJsmCwgpZ"
    L"SQSkIJ91VJSJl/ZWakaYosy5iIukS0CmCFiFaY+SyA7wO4JKXsWGs7zNxtt9ub9913X+/lhOx7EQD"
    L"Yt28fHzp0CEeOHOGH/gUzG/phsv7HH3+8mSbJ8WartaNeCfg7z52m//B7VwGaH9qTNQAsIBmQBFVh"
    L"NKoZmo0YrXqEhTmD7dsYSwsK2xdDLG6rYn62humpOprNBqq1OodBCKVCZpIAFDMLWBZsjCXLVhARC"
    L"QKUUu4mIYSA1hppmsJay0SUEVHKzNeEEBcArAohzllrT0spl7XWq0EQnI/jePPNb35z+moA6759++"
    L"B5kH9W5fihUIADBw6IgwcP2i984Qt3Ner171artXBmusZ/98Vv0x/9r32IxiwYBBIhAAFAgUmBEQK"
    L"kAFsBIPOfWeThnDJApaiGMVqNGDNTMeZnGNtmBRbmKlhcCDE3w5ibMphqMqZbAaamamg2pxCGVZYy"
    L"AEgxINky2FqGtSyYWQCAEAIkCFIIBEGuKFJKWGuRZRmyLLNE1AZwiYiWAVwAcJ6I1omorZTqAthg5"
    L"gthGF6+6aab+q/kc9q7dy/t37+/kJ2f0fBrUZIfChC4d+9eAoAwDG+rVCohAZCC6OrVPmAUAAIzgW"
    L"1hGBYgA5AGEQOKAZIgEoAIAFJg1MFUQWwriPsKV7qEk8sKsNapPgNSA8EAtbCP2VYHO7cn+Ilb1rF"
    L"riWlxHpibFjQzJTHVDNFqhKjVq6hUAiilGGyRexCCtYaTxDhFsQSGIEFCCDGrpJxVKnhTEOQKYoyB"
    L"MTneNMaAiAbGmLXV1dVlIUTpRcIwXCaiFSnlSqvVukZE+uDBg/blwgwR2R85BVhYWMhhtDF31KpVa"
    L"G2NhZGrlwYAZnOBQ97gnwbgOB8G5U8lBqABQbmCSAIpAVAIkASEAlAFUwiGwAAhdBJCxIQZA1S1Qa"
    L"+d4NTGALB9MMdQQiNUBrUK0Khm1KgO0KzGaFYytJpEM60K6vUKarUqVBBAqpABBhGzMRlnWcrMADM"
    L"KmpukFKSUrEmlbg2D4NYwDBEEAay1SNMURmtDRJudTme10+lcVEq1AXSIaJOILhPROSJaMcasVKvV"
    L"C0SkPc/wo6MADjUDQuwJKxUAGWdpgtVLiROWda6fX0XUIoCdblgBFsJ7OQ/DBBGE1KiGKZZmK3jbn"
    L"QHe8hMS09N1GFNHks2jnwBRAvTj/HZ1AFzYNMiyFDqLQNyHQg81FaFZ6WCqnmKmYWimJdBqKJpqVt"
    L"Goh6jXQgSBhJQCQiiQICZBIDCnaYY004U6E4iEFEJK0HwYhvNBELxFyuuNwOYeLXryySdfPHz48P+"
    L"8b9++fyxC6o+KAlDhtgTwBgAIKxXqdjdx+QoAIXIrf2kY6X3ll/j5+LcEEMFagjECAKFRIyzOA0vb"
    L"AEEMC8BYwDCgdX4/1YRUS8S6hjitYZDNI0qAQQoMEqAdM64mKUwvhjEJ2KaQMKioFDUVox520QwHq"
    L"AcZzTYFppoBTTUqaDRqqNZqqIQVBJUKpBRMZKCzlI21zAwIIZjcP0BEIgxDYYypnz179p65ubnPnz"
    L"59+gN33HHHtx5++GH54IMPmh8VIog//elPB0R0qzUWlUpA7XaE9WuUe/4Ryy1cfIEF5Mt4vEnK4V7"
    L"rhRZjgDi1yDRDCkK1AgTKfdTsXpG7cFjOoYRhwFqGtvn3mQW0UxBtKkh0BWkGJBqI0/w2cLduZpD1"
    L"E3C7D7IRFPqoyQiNoItG1aBRFWjWA5pqhJhqBtRqhKgEAkophLVpVKtVWGthreV+v49er5dVKpXw4"
    L"sWLvwTgWw4o/vB7AGYGEWHXrl3biehWYy2kJFq70kGnp/J0j18qZ/GFSgUo8JTE/zFPyH/y5xlr0R"
    L"8w2l1GnObhgwgQNPYWvirlUT7/3v2ZhYIwAxYMa4aPGc4VTVtCZiQyU0eq66WCpBpIUiBKGdcyjWw"
    L"jg76cwugYVkfQaQLuH8e/+8A2vP3t70AcxyAi6na7MMaIJEn46tWrSwBQcAs/9Apw6NAhkQNic3ul"
    L"VmswM4QEXVjpQscBqMo5uCvMkDAq5HHBXmfpPKoozEMpFq/jnF4YDICNNqMbAdoAQUErjukMkbtPY"
    L"2CUxn3R0IOUfscpR6koYBjnQazNlcNYgrYBtAmQZnX0EuBaDzi3DkRoYtt8BGMM2MXGXq8Hay2EEL"
    L"S5udn2M6sfmVoAES1UKxUCERMMrV6MAKNAsOAyDvAET8BDcxx5vHDv9nrbZQbIDs2WGAzGILFY32S"
    L"0O0CSAZXQe0t6GSaFvC/ec8W4ApGnMIWSUP5nll7FCznGMFINbDYZQhB6iFGvVaC1gbUWzIxut1sq"
    L"QxzHqwBw5MiRHw0FKFJARTQdhCGIBBurae0qA6yclY9bNHmCHLduHhP0BAxReg8eCR1pyrjWsdhoA"
    L"4MYqFe9P1SM6gD5YYleqZJPVgzAy3L992XASIKUeXgIFWG6xqhUKtBal1xCkiQgIpllGQCc+5GsBm"
    L"rm+SAIQCSs1plYu8Je6jcp+XdSYAuQGEsVPUGzJ+wR5cCIkjAstGG0O4z1TYt+JDDdBGTB3huXUhb"
    L"Jw5jrpzEvwEO9KY8p9IWH71W8jnjMi7g7QgDCuoojA/XQQAjhyCQumEcWQlCWZQxgxXOBL3vEv3QN"
    L"YGFhQTCzlES3BCrXR63ZZQCUC7i4+cpAPAEEelJhHkP7hbQKX2vzMOBR7tZadPuMq9cYnR6QubTP2"
    L"CHytz6gK9z12M+N/zz3euZhnB/HAcWNbf6XGh9MFimoBeLEol6BYxRz91/UJ4QQYObIUc4/nCDQa9"
    L"MSjrs2AFIA+OpXvnK7c43U6ydodw1q0yFIANGgeAM7TP2KOA6Zf1KiAIY0luqNp4nWA5SFQkj39ox"
    L"oYLHRttjsSCQpEE7AAUTXx3/iCVhADJ0V55RD6TGKP5U8D+J7kcJbGKdEmQbS1KDWFE7ZDIDcE8Rx"
    L"DCkliOhSEARrr0Ym4v8PobtePUlETESWiDQRGWZuMvPPJEnyF61W6wNRNIAKlLh6tY12J8Wtu6awa"
    L"0cDUhBG2SC+Lo0bcfnj3mDk8TGMAGdmLiVNUotrbcZGhzGIAaM9K3UWfJ1HMEOyyH+O8R9n775x1m"
    L"5HPYDxvAd73sOYnEtIM4taRYCZHQPISJIEURRxGIYgouWiC4qI/mUUwBf4gQMHhBO6cQKvpWn6/iQ"
    L"ZfDSOo/89GQy+Z4z+ahgGH5dKTRlrEChJl9euIdWE7Ys13HH7FFpN6SmAHcsA7PWu/jpBW8/148aK"
    L"AEamGe2uxcamRX/gwoDxhFG4eZPfrPHcuid0393bCY+bSYpih0rgK5Q2eVaijUG9FuYZgFPaJEmQZ"
    L"ZlVSsFau1pUDV9pPUD9gNx6qUzOrRvv5y1AvzdJzC/E8eCniPCmSqUCQCDqdzHo9NHrRzqJYxkEIU"
    L"kJrFzsgm2AVivAzUs1nNrWw+ZmdxTojWQAE/AAu+f6rCG80MFiGALc+zEYWlt0+xZXNy16kUCqCSQ"
    L"8kCbyOtMI8KPhfzxieDwWLqxz+34I8EJI8VRLQyBpDJAZRpwQjNGoBHlqaI0FAmAwGCDLMtRqNQA4"
    L"48nin68WULQ/eQIvwhaYuR7H8b3W2geI+I39Xu+dQaBur1QqGAwiRP0I3XbH5BwJCSmlSOJEsbXOb"
    L"VmcX+5CyhDVCmFqKsTOmxs4e76PzIyzfJ5wSxxQ0MPWu7nPo1QKO5ohkIP4JMBs0B9IrG9atHuMNM"
    L"vTMEHer/YFz6PIn2gMG0zgCMj73oH7/P2L3yGGkEfbvAYxyABhNRQZaJN7AGZGHMew1pIjhk7/s/Q"
    L"DeOCNDh06NGLlBOAab85VE/VurfkX+/3uB6UQb6zX6rAmQ5RFaLd7AEMzsyBBJISQefqSgq1B1O/n"
    L"VTkSyLIU51cihJVFhIFAoAhL22uYng5wdUOPWbwTKNs8Vyq9w1i9YIQexqgXKMFk/hxmQpJYXOs4W"
    L"jgGgmCYt48LeIQZHOMFRlhEX8DwhO5bvXuOMMP/zpg8BMQpIJBCkMkzANfX0Ov1mJml1hpBEKw4Fv"
    L"D1l4OZmQ4fPiz37dtXoPXyTS9fvtycmqrfazLzc0y417b5dqPsUr1WQ7/fR7fXQ6fd1gyQEJKISFi"
    L"2yhgNnRqkaYo0zZBpDSKBXjQAEUFIgSgaYGU1Qb0eQrn+jdnZEDuW6ti41ob1FYDhhCdGLXqkLgDP"
    L"5RfPJc/1O78MF5SFRJYZtDsWm22LKBao1WjIB3jCFzSBDKIJxA9NUAbP1Y/Ty9ZjC7XJSaAoAST6U"
    L"ArQlsGc3/r9PoQQsNZylmUdxwK+dgXYv3+/PHToELsSrXbKECZJ9w6T4o2WzYeY8YDRvKdaqyFNUm"
    L"gToxv1udftGgYLZggppcrZtdg1Wupc6Jkekl0kICSX2iylRHuzj/V1i+aMhJIEbSwqgcCOmxv4/rk"
    L"+ej0DIgEmT8jk3HtJDI3TwNYLAT52sLnrL71ArijaAN2ewbWuQj+yaDUkwsAjbzwBTrJ63xvQBLaQ"
    L"vNaGEQUYCxXMQOYUYBADM4GGEArWcJkJRFFUIH7t2sxeXwg4dOiQUUohy7JdAD6Qpsl9V6+sfUAK8"
    L"RNBGIbVSgX9QYR+t4dOmw2DSEhJUiqybJXJNNIsQ5amiOMESZqBnWslAkgICCIQUfkPa60hhIBSAp"
    L"fW2tjcZCwuEUgwjM4tdHGhivnZCnrdyBOqn1wXQiQv1mP4vPI1BrDCOQIzDCGFF2ALayR6g9wD9Po"
    L"C6YyFEGIiDzAC+sT1TB5NwgP2BmHCp4kLBdBAkhEGscXOaQOlqrCxgRCELMtKDgBAIoRoFyTQwYMH"
    L"X5sCfPjDH/7g3r17f+sv//L/+Le7du6afes9b8X27UvYuLqG9uY17illiEgAJCBIShLI0gTdeIAkT"
    L"qC1QZJl0Dq3VCEob6CkPDclrwxc+Dl2rldJYGW1jTSRqFYAIoY2GoBArSqxtBBiZbUHbU3OkZIcTe"
    L"18RWAHs0eAn/AsX3pE0KgXYEtIYsJm16DTl0hThpJ52fo6a/at3w6tfdwLMIbgbqRgJCaUErwQUHi"
    L"ANNOYbhQMYv6/FCygUgrGmDYzb74aDsBXAAnAvOMd73jg4sWLX7t8+RK96yffjSe/8U370d/+bfuR"
    L"j3xE/MEf/D7V6nXKMq20zpAmMeI4wSAeII5TZNoUVT0IIdyNiiHO8qtX/cu7cYwBWwsBghCMq+sZY"
    L"PPmB7iGC2sNpJBYXAwx1QI2NmLX0mUAyptAcxRvR6sqTGNg0HmAQmolLqCiEQt5k6dAmgHtnkWnZx"
    L"EnAoFiCJFLnEafXrJ+13kHeM/DMA30rZ/NqPsvhe+qganOG0iyNEO9wo5bYCgiDAYDaK05DEOy1i7"
    L"/5m/+Zu+3fuu3Xn0IuP/+++mJJ55AmqZ3zczMUKvVTPfuvTtoTU0Jq7X4x3/8R0RRhP/0n/4I58+f"
    L"Q6Y1jDHIsrwmXQicStdOI1buK8F4UcdonbNaRBAEXFnPi0BhKMD5PCCszbuCp6cq2L5Yx7XNNtgaZ"
    L"20WIO2UQAGcuSqiyptAhXLSENczhONYwFKeTbCBzgi9vsBmx2CQCFSrAqJ4SRG+xuO67/bHXPzLAc"
    L"JJ4NEwoE3eoEJIMN1U0Db/24kISZLAGGOllIKZV4mIixmL14QBiEgyMx85ckS+8U33kFQKcRzjIx/"
    L"5VTz6tUdw5cpVbGx2cj6ktHIxIvCJlj7u9r2faWOKQgZAhI11DUgJpYRDwRaweUt4GAgsLdRw9nwX"
    L"g2icD8gFl1u+AZABSPI2caEAq5y3CF3reOiyyEKqcqRXQBtGFAGdHhANGI0aoAJGEcSuy/fNGLdKY"
    L"40jN7g/YhNjocNYQGdAnAASGWoVwBjr0tVy1K3AAOdd/JcFeH/VCiCEaBIJqlRCZkj0owTHjx/D3W"
    L"+9B2GlgixLIUVuSZOs3FeC4r6vFOPKkKfrXGKDzBhcudKHDBSEBNha5GVv1+ELgdnZKmamQwyiwWj"
    L"5l/n6li9GrgxFqzi5TmArASFzhbDSmW/gWsYDMAVgVoj6VbS7NfQjwnSTSqIKyJWeaQIHwBOES5NL"
    L"vZM8he8JrCsCRQlQUxnqFYlU2/Lzdg0h5LqBTr4WUm/cA1SEK7xoCwziGJVKCKUC5/K1+8U8ogTj1"
    L"j1JCXyr9wGgMQY2zxsRxwkuXu6hWp2BoJzyZGNcS3f+Oyuhwra5KtauxnmGUJoUeVScdMjeg+d+D0"
    L"FZWdT540IMgSFnAEmwlUiiBN12il6viWQmhCABAoGJck8ghPMI5ELA0LzJ0wwiXyHouvRvUggoaeA"
    L"EiHtAkzJIkWOA4vNzTSHCZVEXXi0JVCrA4uJiMX/WzP8WQppZZNrCaF24GCRJeh2ou5F1O60c8QK5"
    L"VruUqmgEyTIYrREECp3NNq6uJ6hWAghBMNaWIL5oviQA2+araDUUNjez3Hr9pg8UDSIeT8s+8HP+e"
    L"iQL8GoGjkW0YCQZo9uz6PU14lhCCZTZTJHPkeNzaQz+UxkDaAT0DrMAGi0lj6WQRTUwiwmDDrBt1k"
    L"AqCWuHPIobYiWttdVar7xaEui6aqCUMsy1mZEx54wTGFIqZ/nmuhhf3B+P777lj3uB4ntmhjY5tx0"
    L"EEmtXe2h3LKrVfMzLaAtjLHSW059aG2TaoFEPMDtTGfYA+CRPme/7DSBuEKRsLLFD5q/EDF5hyOb3"
    L"tdbo9w16fUYcW6QZIzOMTOfpmTZ59TDL4B4b/Zk2eQjTJm9y0VlebNKay5spf47hfZPXADINpBkQp"
    L"4xmIw93lm3pzfr9fmGcbaXUyqtpBBnxAHfddVfpAYgIwgh0ryTQsGC2ECKfgfM6TyYCukmu3n98HC"
    L"9YlwUwA1IRVi92YWOFsCJcvxvDWnbjsQzr2BEpBeZnq1i52EeamiEZRF7DJ8TkRlAIVzjy6ghueih"
    L"/zJQQ3BiBQWzQ6WkMkgC1qoW0BCKLAg6S8JkdGlp36QxM3uXiJf1UhiL3HjRsXCwdlZszSDMgjhnN"
    L"uoK1iftMy15AVkoRM6+mabr+ajmA6zCAlCR0piFuVrhS/3/RPZmBdBUkACFy0DFpPGlS3C8E79+3r"
    L"uJXfGVjYKwFgUBkcH65C7BCqMh5B+sUIK+3WnLCI6DZDNBqBFhPk7ESr/VqBHIsPJixBL1o9LNeWD"
    L"AlNWKNQZzkXmAQGaQNAeWylTL2j6N6J0guUuJc092v5LHY79TIvZg8hFjUAZIE0JlFNdAwhl21MP8"
    L"MkyThIAiQpunKK92ZNDEEHDx4kN3fWa9X6rDviemO/yHGz//VPLa9WyKLcitx3acv69b974vW5ete"
    L"QwTj+ARQnvOfOdsBKIBS5GjQPB3T2oWBzORuNTNQijAzHboizZgr5+K+GQI+8hr4uEgZncWzF0Z4+"
    L"H5sLJJEoz/Q6McGWZJPDunMIsusc/8WWeq+zyxSbZEZmz/HTRplGsgyQGcGWWahtUWWufCh3XuW99"
    L"3PtUGaWcQpoDONQKblHAGAcpGFUgrMvAIADz/88MuNSb20B8jSjOYX5/Dv9/xnnPmHo4jmL2B2Z5R"
    L"PyHjAzQdy4259HA8UFl/wBYVSSCGQZZnjFAhJmmJlJa+9CkkwlsvGSQaDHR5hsrA2Z6KnpkJUqxL9"
    L"SA8RuLU5mT5eGYTxCkQODFp2tVca4efzfgINhkSWCfSjDP3IIEkNGAJUpMKO2is9uPAteujqycsMO"
    L"J8YHnueGMkfiTgPAdoiTgjW9lELU1hbBVuGCATiOC7Gy0FEZ/0W+9dTDKoLKXHzjh1oVBpob7wJ9+"
    L"ypQlQZySMpkiTB1NTUDVm+SQRQ8TNfCay1kMzQaZrHfykR9WNcWkuhwhoE5RmABTmjzBXAOkKdXfd"
    L"OJZRo1gP0+5lz/TQ6B1Ak6pDDHoEiFBR8rhXDAnyhKDZXAmYDowWiyCAaGMRpwVjaYapX/q+ibPUp"
    L"soT8i3C9L4Wgyfld9jgB49VJioHVHFzGCUPyALWqQc6222IABNZaBzloAwAOHz78mnkAdpok868CF"
    L"9cvwFqDW3feg/Zm2/1jryzuT1IKXwmK58dpCoARBBIbGz1cvmIQVvLCkdHshZX8A2PLsC7Ns2xBJN"
    L"FqBbi6QTA8Vg0kB/DKBNwDfCy8moDXylM0OhXKxALGWMSxQRRpJLFy3UFiLPdnEBkn7GG/V/7ZmOv"
    L"TRvc3DXEDeZ8vO4BMyLK8S7kaJphq5r2AbkIYcZz3frrPd/O1cACTmECRz64TwiBEmmawxrr1JwEi"
    L"15/tgzs/3x+3/PHswAeAlhlW63zNmyJ0exZxRJiaI0dyWBcCxjFFDqYKXqASStSqCr1+5uqo1gsFG"
    L"K3a+I15RWFo2H3nzR0Oy7HGWsSJRjTQGCQGSgFSur4tznsaiv5uIi6VYChnAsOWXoJK9E8j/eA0Vg"
    L"jIPQAjijW2NTQqIaEXDbuXB4MBhBDkwkAXr/GosUEWJaUqkT6RgFIBhEhBVDJPE3P+8RDwUiliQQB"
    L"ZayFIQElCu8NAJpH/6hwDWGvLZl7rPEEeChjsPlghBBp1hX6UgUvCx15v2UUK6FcNuRC6HKaNpXIM"
    L"OaQsCxDFBnFiUAkIUrnYTgJg41bTFG69yBKGNZLcG/jsaWHtwjOaAifkCmJMngJGkcH0QgYlFazV5"
    L"QBLkiQshCCtNQNYd70ceL0egAvA55d1i1VqhZ74aH7cwl/OE/iMIDjvbg3DANfauXIpJcHIU5/C/T"
    L"Oza7suvIBTAJc9VKsSQSCRJgaQXgNI0egxMrRRpIbGo4xpmA6OzXYxA2miEQ8MktggrQqocu6LIQp"
    L"g5zAAOZA3GgZGsUDx+4jMqJJ4XKB1reBJrDE3w270nEcaQVwGMJBSLrsswL4uHoCIgnzCpCjt5qtM"
    L"pAxAJBDHyQ2R/iRBj9PBfqgovlpmKCVx+UrkuAiUHAC72jfYWb31QoGbvwcEpCBUQkIOKawHAO2Q4"
    L"GEawwMeMVR6AoyOnnFRkNGI4wyDuIJ6amCVb8ke+CuG+ZCTRWXpuMAVBBC75woPC0zgAYwB0hTQOs"
    L"VMS8PYAJYtBESxGKJYOnVpx44dl19PCPAtVuQKIKmo60upIIQs/+FJ+f8klz9O+vj3QVQWgYCc2Vs"
    L"5fxWA2+ppGJkbuGMXD4t0sCiG5GmhhSUCW4FKINAXxRIwz8JhXPXPjnkCDxSWSjAGCmHyCV2tESca"
    L"g1gjiQW4wmVFsIzpQjgV4LIGQB5AJKIxKy9e69USvFTRGEacCFibYttskBe+eJiGMzMrpcgYs/zAA"
    L"w+8qmmglwoBpfXmIYAghIJSYbkD76Vq+zdShnGcIAoMwPk6FmMsVi/28yYOANbNUhUhwHrZANuhEh"
    L"ShwBJDKEAptyqY/fYbeMN5PKFK6CuBHvYFWJTgzmhCmhgkSYYkzcvHQ6ulsrGkTPtK4YuRYiVhrDi"
    L"E8RLz8M81Jt83JClBq26QmVEXxcxWSikArBZd3K9rT6Axht73vvdRngUIEMl8NFkI5wWGIWASCLyR"
    L"sG8EFrM0hTUGlTBENMhwfiUGwmbp/vNszlMCX+hwmICtU5C8VhEUClB2C5sh8i9YwvLzpmFmwJ6yl"
    L"CEkxwtsLdgapGmGQWyQpBokFEQ51TkUIkG4ApUnWBKl1ZMYtgIL8j0BRsvrbvRsEAHVwKDVUq7lbv"
    L"gc7aq0Qogzr6URZJIHICIKwrDiZQF5CJBSl9VAn/L1vcWNrP9GTKHOMhhjEAYK164NsL5hIANyPQL"
    L"DFHA4Rl3Efve7rZ8ZaDAJCMEQZGGLeF8yfdKbDRxv1hNjncEYThkxHBeQ90IkiUaSKEgBxwaSV+Yl"
    L"x/CJEuiVilH8vFQQMXT/KD5DO9IUYCwQxRZLcxaVIOcDio94MBiAmYtpoDOvZ7RvHAQ6ADgEgbmWu"
    L"S2coJckgSZZ/6TyMZV1AIsgkDi30kPSY1SmyTVCsBf3c0u3zjiL+G8tD1lCY/NCEQhCWFhd0Lk05v"
    L"L9OW0z2r5T9glyrjDWlKkd25yPz0OAgpKAED51m1PTuUOwZZ5PRS+iKMge66V/NNJlTCPpK8Ey0B8"
    L"YNGqEMBDo9vJ6hcgpdAYgszyVXn6tJNAEBQCkVFAqKP9QISSCIEA+K5COLCcab/bwkf6NBE8OAKZZ"
    L"TjJJBZxbbgMs8zZwzciUdfnuMAtg1yHMxgsLcN8DsKwdTz866zes9nnDoxbe/CDGPIGfHRT9d4DRB"
    L"mmikcYagczX0aMkd5DXBRwYHPUMox6QaLhMgMbA37ASmIe4NLGoBNZ53qGDctvBYK3NiOg1NYLcAA"
    L"TKYtGAs/g8/uc9AQJpmkJrUxJCBU/gnxLpTwCCpXvTGlrrHOyBce58H0C+9SJOdN5OLsh5AXYp/dA"
    L"rlKkhPGIIeSgoBMJsHJqX3hIpvi7Gl6FB8Bgw5JI4yse+DdJUI0k1goAg5XDegFy/BNFwVI1G0rth"
    L"898QG1DZTTQEgFQ6ImsLBcjc/8dlN3CapkWY3pRSrrzaYZCXKAbluaWUakTAfhGHXR+fn9YVPy+UZ"
    L"1JnUOERpJTQWueNIBYw1mC5VABGHGcQAMJQeTiAh/He4wHgPV4oSz5mJkbXBRUbRWDGikXeZIZlr3"
    L"nT8wSupmBMhjRTSFOFLCAYKRwbOIzf+cia4wBouAyFxLA8KDCuDE6JvBax/P8hpDpDvZaCrSzb2N1"
    L"GEA7DkLIsO3/y5Mn2a00BRxTg0KFDBFBQrdZKDJBnABLGyGIA0a0mwcQwYFyL96Q5AeZy2x/SOIbO"
    L"MpAQyDKLldW0TL+0BgapzqtuYliiLwRsPQKJ/cyg9AwY9gGIovBTNhZ64cBrvgM7JTFe46gYvpcQY"
    L"EvIUo0kzRAGBKXkMPaXnUGitP4hMLRenj/aH+g4vzEquFgoQdBphlYtzTefu04gay201qyUgtb6st"
    L"sJ/KoWRE9UgIWFBSISQqnAuRe6LgsoXHwhaB/k+UpQ8AU+nVxyCwCSNEWaZQjDANooXMv7QFyzA0F"
    L"njJg0QiWd6xvjAtgjhNz9607xoNBeOVh4jaNjW0QLwDYCDvN4ztbmQtEGWWqQhTqvRkKUef+IJdMQ"
    L"BxB5fH95X3it4cPYX3AFlhmZJrBNcPOigtYFk5p7T601VyoVMHMXAA4cOEBFU8/rzgKGOMBN7wpZ8"
    L"gDsUrSCivTd/43oYV8his9aZxl0mqFWDRDHhEsXM8BUAStAwkJQjuw1cWkRQy6gMFw7WfD+byrWbg"
    L"n/twuPETSeAjj0T8LDDLK0arYMrQ3STCNLhcdqClcUEkMGsIzrLkyUfYJ2pKu4aDEfTgeL0iklKRC"
    L"oDLMzQV4ZhQVB5sshHMXuDYPitcT/CT2B+TLigvotrFdKhSAIyxBQeAGf7x8Hg5NGwYpuIKN1HgJI"
    L"Ynqmhgf2dfH909dwaU0hjWowJoCBQCYCIJQQ0uaulK3bjZOPir3sYW/syxd2iQnYW9wnvJ+PNGi7u"
    L"JyPqmepQRpqKOsEDJvPBxRpctETQHY0FBTFIp8mHukNoBH2cjAAGjWD2akqUp252hIVHEDx2W46BX"
    L"jFK2FedjpYCFV2ARc8gJ8FFBY9zvEPswe6rh4wQhK5kSaSAv1ogMsXl/HJ//he9Po9dPsZ+lEVVzc"
    L"Ix168jFOn1vH9Myk6GwrQdQBVIHAdPNJCCAsCO0aQnHeg0TEddrSaEMM9L2U4oNFwUHYNSa9R39HS"
    L"JMBGQGuLNDP5cEwhZLbO2of9AQUHQL5XGFGGUU+AsUGSaMDYuWhRrTL6fS4oo6IPoADkK3id5zoFy"
    L"LMA6dUCJISwXggwfi9aaf3+Y+NDoiN9AFojSRLIvJSJiyurWFlmhGGIWq2CbVMB3rCjhZ+5fy9ICv"
    L"T6Cc6cXceRo1dx9EQfZ84BVzZq6HUkbOS57ZBAykKQBtiUJBKK7frW29BUeg85XPFNxvMExaSGHA6"
    L"NWIY1BJNJ6EyUC/+GgrXDql/5uBilgstxMj//F2NeIP95PDCYnWaEAaFjbF5k5PxKIswssywDEZ1/"
    L"PSTQRAwgpYCUQ7CSewQLIcivRI1Yt0/y+OBvyC4Ob8UkEJyHCIKgDA/9boQrnStIqhGqR6ZQDWtoT"
    L"TUwPzuFX/jZN+AX/q11+/mriOMAZ85fw5FjV3DkSBenzlm0NwKYtOU++AwQGaCcpyADtia3ZGG9f9"
    L"0jgApvwEWIKIpD+UYSa3MGM00JbGWO/P3+v1KQYpTxK7IRb0ZsqAB2JAPIOwcJSaJx86ICuOiqdn2"
    L"S1haNINZa+7pIoBsogAKRLHkAKZF34YJGPIAv6MLF+2h/HAcUPEGWa/D1wJEYEgF682sI3ryJyrf3"
    L"QkWEbqeNTruDRrOJIAihlESlmqBer+An3zaNB963BKkE1td7OHPmGr53bBNnzwKXrszh8obFxcs9m"
    L"Hbm+H0AYc7HS6kBpPnlZCDBLIerCMl6tHGx1TG/4JPWBjLLrZGEzwZSOedVWDUVYVl4bt/HCfA9AA"
    L"+LQyLvA1iYU7nQmSF4WK11JFC3Xq9feD0k0IgC7Nu3j//0T/9EF4zgMCPIFaD45eNpYIHwx8OA3wx"
    L"SCDv/AHPr57HMIPeYFlPxNsjTMwhNDYYspJAOhzGUzGN4mqRIkhTr613XRaRQq1Vx01IVu3Zsz6/L"
    L"E4QQooZ2J8P3z/Zw5FgXLxyNcfIsY+NaANNXbmVnCgT5xQFysGny4hIAJuWGRfIOYbaANRomy3GHK"
    L"GsAYkgCuSEXfyqU/DYz8rqFyXuOBwqZCbAaN29XyLJiKXRJprGUkowxq7feeuvVHyQGYAAmyzInuA"
    L"IUYmSfTwECx7mAcdp3vBmk9BhSYm5hIc9nsyxn83JyAybL0EhmwCuEDDpvrnCbmNI0RRiGudUxg4R"
    L"AoERZUIkHEaI+w9hi6US+irBWq2D3rgbuedMsxH6Ja5sJzpxt49wKYaO7gGvXFE6di3DmbB9JWwOa"
    L"gEABIQMiBlEKUXb2CljDMCYAWLrVbnbIIRR5/0gr2GieP+w9tK5bjUYmjAmANoSKspidImSZ9aaJC"
    L"MYYdr0ZF975zndmr4cFvEEWIMaKQ0WLkkEYBmXMLxRhhOkbY/58DDBacSRIpSCDoFwYlZfic8Uyji"
    L"pO4xiZ1vnjSYJ+bu0spLQqCEhJSeRrJxhSACKQYJs3lvb7A7TbfQwGA1hjUK1V0KgqvPPNCo2mxdT"
    L"UDEjUsNlp4Pwq4+iLBkePZzh2VmBzcx42AUyaADYGggS2ypBKQ5DJgSGEa2RxYNTP+0FjQNEVjcYn"
    L"ij1FIQLSBJiZJszPKSTpoHwPrTWyLONGo4E0TctpoFdycahXjAFG8l8U5Esu6CAISuGPY4BxBRhXj"
    L"FfSNSTyDhSoMERYrWJ2fh7VajW/pm+SIB4MIKUkJaXs9/sYRFG+dUwIS0QspKTCh1pvRL0SCqigiW"
    L"QwgLWMXj9Fp5vAXuyC+RKUFGg0q1iaqeON/6aJX/oZgY2NBFeuBoj1HDrJHE6fMXj+RIwLqzF6HZs"
    L"vlFAEpWIoSiApgRTFzIJz4yRKMonKgVGaUCQb6jAJQpoxqrUAQhhYY8GuKusbHfLrGL+maaCXUgDK"
    L"hTTsyDUGbiEE+flnKUjjrXjxBT0i1JcIEz5J5K54gWq1irm5OTSbTaRpim63iyhJeG5+Ht1u93x/M"
    L"Hh2dm7uzSDaMYiiCpiFEAJpkiCKY1itGUSWRD7CYZkdAZf37EkiSEGgYDjoOhhk6PWu4dz5qxBECE"
    L"OJRkVgduoy9jRruP9tLUg1hd6ghuW1Co68SDh+0uD7F1roRgpRV8P28wtNIkwRBgaBYhCnw+zD4Qm"
    L"fKsbIUEhelEwzi0ooIUi7axLYcicQMwtXjT37WqeBbogBiCjTeljzz1m/4hKnKBXAB3s+kvfn/240"
    L"Mj7+tfAoUkrMzc1hYWEB1WoVURRhZWUFWms0Gg3eNj9vqtWqeuqpp1b/+I//+H+am5u7+S1vecvs/"
    L"OLi1N4779xxy65dd7eazT3bZ2Z2W2trWZrKJI6RJkkZRvL+Tk1SShIurx3+7TnoDUNVkoiDmBENNN"
    L"bX22B7DVIC9UaIpZkGdt/fxIc/WIHhJkjOw4htOHoG+N7zMZ470sXZlQz9tnDrQFOAB4DoI5AJApm"
    L"6hqWC0RT5yhoISEmwmUWzYiEFI2UqiTibW6JwOK0oA9vXmgGMKICUkj/0oZ+1WaZdA0JR+mVXASSv"
    L"ImhHCj5+o8e45Y/3B/ihQWuNMAwxNzeHHTt2oNls4tq1a1heXka/38f09LSdnZ1FkiRidXVVvfjii"
    L"+ePHj36jUajsX1jY0M+8cQTawBW/xb4HoAvAqh98IMfXLr33ntv37Nnzz0Q4g1QaqGq1IIKglalUp"
    L"GDKEK73UaaW5MlIViI/EK/NLbqW7jWbaVU6bjTFLh4qQe6MkCoJGr1EPXGZcxMN/DTb5/Ghx+YheU"
    L"ZXN1UeP54hhdOZDh1inG5fRPW2gprl/rINjcB2wNUBqoCSmhIO3BNqbm+1MJ8YipH/8NxMOdtY6XU"
    L"hZFY/YMBgXkI0FqXCpAj9AzWmjIOjRNBk5i/SSmhn0UEQYBdu3bh5ptvRhiG2NjYwLPPPgulFJaWl"
    L"rBr1y4MBgNx9OhRnDhx4uxzzz332a9//evfBBABqANgKWWjYC+VUhxFkXnsscfWHnvssWUAjwEIp+"
    L"fnq3fu3t26++67b3nnO9959+zs7J0z8/NvAvM2o7VI0xTxYJArhcdRuIbL0XF4p8BBICHdpeUBhTh"
    L"mXE4iXLocgWgVlTBAvRFi760h3v7GCoKwDhkwwso0Vi5vw7MvzuP552O8cCLCi2cJUVsgS3UONKkL"
    L"xOu4eUfV7QRiCDHcClY0glhr1/ADOMqPwUJQmqYJtM54WPUbEj9uI+VI3C+s3c8IJmEB7fYB1ut1z"
    L"M3N4aabbkIQBLh69So2NzcBALfccguazSbW1tZw5MgR++yzz3775MmTT373u999Ko7jawCqUsqWcV"
    L"dMMsZwoVRpmjpdkE0pZVMphSRJTHt9XX9rfb39rW9965nPfOYz3wIQ3HPPPTP33nffzW9/29t+QhJ"
    L"tj6No+/Tc3P1TrVbNWsuDfp+63S4Gg0GuyPnARL4hzd/q6Dp1hMj7A4rnMIBBbN0s5QAk2pBSIAwl"
    L"Zqfr+Pl7W/iVfzON1DSwesng+CmLF88KrHdmsbG5G08/08ett1+GNdGI58yyjKWUZK3tLCwsJD9QB"
    L"XCsFBujoXWaEx8l85e3aVWrldID+JnA+ESw7+aLUFGpVLBz504sLS2BmbG+vo5Lly6h2Wxi9+7dtl"
    L"aridXVVXz1q181KysrIk1TG0XRRpqmL8Zx3HfCV8YYv/V5bMseyCkop2nKMj+qUqlQkiRVY8wAQPb"
    L"ss8+uP/vss/03velNnfe85z173vCGN4SJ1ubC6ioWtm3D4uIilnbuhCBCGsdodzro93oY9Pv5HmSt"
    L"IYWADQJUlSpbtchjP4UUIDWslxTA7/LVCMurm5BiBdVqiOmpKt5+Z4j73lZBtRohrPSR2imk8SwuX"
    L"exAClUO0hQhs9frdQ4fPpy+Xg6gUAAuQI8QQmudIctSt6LVetQvIQjCYi79OisfT/sKwU9NTWF+fh"
    L"7z8/MwxuD06dNI0xTNZhN79+61tVpNnD17Vhw7dgznz5+HUkoyM+bn59Uv//Iv/2yz2fzZU6dOfff"
    L"hhx/+r4888sgLUsqKUwLrCV54xD4BKC6qbI0xSRRFSa1Wm9q/f/+9u3fvfmuj0dg+Ozt7x9TU1FIY"
    L"hhBCoN/v49KlSzh69Ci9cOQIGvU6pqenMTM7i1q1inqziW2Li6jValBSIo5jFKmosRZw7lkqVXoKE"
    L"n4DmGuYtRrbty+gXqths91BL7IYpBp8LQPy/g5UqxJSBjnxxDkbWii2UgpRFHUOHjxoC8/7g/QAJs"
    L"syt4Ey36RlrYHWmZvjV+j3zXWrYMZn/ogI9Xod27dvx+zsLLTWWF5eRrvdxvz8PO/Zs4crlYpYXl4"
    L"WTz/9NL/44otHjx49+tV+v9++9dZb33rnnXe+9+67717cuXOnDYIAMzMzb2s2m2/LsuwPDh8+/B3n"
    L"CVIPAMnilm86A9I0bQOovuMd79j1cz/3cz99xx13vG9qampHGIbFdC0GgwH6/b516FrOz8+jXq9jZ"
    L"WUFSZLg2rVruHr1avkhK6XQaDYxOzOD7du3Y9u2bdh1661uoaVGlmVIkiRf3+IaXwUAVgrSKYSxFp"
    L"1Ox21IyZs7pQObBQWfT0brkc81juOCBmal1HoxzPNapoGuU4BirEjrdGCtQZom5RiytRb9fgfGaAd"
    L"E9HUZgPG6VGZmZrBt2zbU63XEcYwTJ05ACIHFxUXccsstLISgY8eO0QsvvGCPHTv2/xw/fvwbq6ur"
    L"JwEM7rrrrts/9KEPTb/1rW+tLS4ucq/XkxsbGzh16tTakSNHPn/mzJlVAIaIBgASbxJGAKhUq9Uwj"
    L"uOrAMKPf/zjP/+Wt7zlF5vN5ptarVaYJAl6vR5ba21xBRT/0jfFwIUQAjfffDPW19f99uuSru6027"
    L"hy+TKOHj0KBlCv1VCr1dBqtbCwsIClpSUsLCyg0WhAG4N4MEAURa6jWiMIAmRZhtWLFyGEwOzs7HB"
    L"QttiiMmH8LgfmTG4e8JsA8LGPfUy5z+D1VQMfeuihInylucWnbEze5KB1ik5n0/0ROR7w3Y61FmEY"
    L"otVqYXFxEfm1gQY4d+4ckiTB0tISLy0tcZZl4uTJk/T0008Pzp49e/TkyZOPLy8vPw9AvPGNb7ztw"
    L"x/+8Pvuu+++fTt37pRZluHSpUt48cUXN48cOXL4H/7hHz5/5cqV4no4qda6ByBGvhDYViqVMEmSTh"
    L"zH6a/92q994L777vvvlpaW3l9cUWNjY8M6JSdmluMjbb5H0zrvf5ybm0On0ykWMpeeLggCBEGxOTW"
    L"3+Ha7jZWVFRw9ehRSSjQaDSwuLuK2227DTTfdhFarhZYYFtSKNe9u0ZPPobCr9tGEbio7NzdHWZa1"
    L"v/zlL38RQPh3f/d3AYYXiDIYvZ7Oqw8BYRj0sywrPUAUdbG+fglJEuVz8q4jqLgppTA3N4f5+Xkop"
    L"dDtdnH+/HmEYYgdO3ZgdnaWoyiiZ555hp5//vn41KlT3zx27NjXe73eRQDp3r177/yVX/mVn3nnO9"
    L"/59ptuukmkaYozZ87gxIkT/WeeeeYLX/3qV7/a7XZXAHC1WpVxHHcA9N0tAZDecsst8vz581fuvPP"
    L"O1ic/+ck/nZmZ+a0wDNHP22i4UqlQpZIvHhwfa/NZzeJ7f6vZ7OxsaflpmiKO4/Lm07JhGI4A4iRJ"
    L"cObMGayurqLRaKBaraJWq5XecX5+HnNzc2g0GuWFH4tLwSdJUix/sEVerpSihYUFW6/X1WOPPfYXf"
    L"/u3f3tuYWFh7vLly4mTofFu/tWyXpkCFJcaJ5KdIj4CQLe7iX6/A60zFEOjxVBiAe6EENjY2Bhxn/"
    L"Pz81hbW8OpU6foueeeu3r27Nljy8vLT62trZ0BgLe97W13ffCDH7znAx/4wLt37txZLQR/9OjRjRd"
    L"eeOGbX//617+0urp6wtHCmTGm7zKBGEDh/tOdO3fy+fPnO3/0R3/0E3ffffdfLi4uvr/b7VoA3Gq1"
    L"ZEFt32hd/Xhhy/8+c7OLeREsRK1Ww9TUVJlyDgaD8hZFEXq93khbfKVSgRACYRiW6fPGxgauXbuG0"
    L"6dPIwxD3HTTTVhcXESz2US1WkWapidnZma2hWE4q5SSWZZhc3MTRGRvuukm9dRTT73w53/+5/9XrV"
    L"Zb7Ha7/gXrfOGbCY+9tAIUlxrPsqxjrUGWxe4DyUmgNE3KapS1Fo1GA7VaDRsbG9jc3MTU1BR27Ni"
    L"BVquFtbU1fOlLX8K5c+fs6dOnnzx+/PgXXfNisGvXrtt+/dd//YF77733HbfddpuI4xjLy8t47rnn"
    L"0u9+97tf+fKXv/w3cRxfAGCq1aqJ47gXx3HkCT1xbj+76667cPTo0fQTn/jEG9797nc/Vq1Wd25ub"
    L"hqZ+8+JBacbdSqNV0GL0OZasEt3XXjAIAhQr9dLr5FlGfr9PjqdDjqdDqIoKmI22u02lpaWCrKq/E"
    L"pE6HQ6BX/BrVaLLl68ePZTn/rUb3/0ox99y549e+5ZWFi4a25ubvfs7Oy2Z5999umPf/zjv72+vp4"
    L"wczNN02zM+vUERdDe4/yyIUAp1bbWsNYZFSlgliVlFuCuUolut4s4jtFqtXDHHXfYSqVCq6ur9OST"
    L"T9q1tTVSStGuXbto165de/bs2TNz5cqV9v33319517vetXf37t2Nfr+P48eP49lnn20///zzj3/rW"
    L"9967PLly0ed4FNjTBTHcRHnk7x1B1nxD95///3Yt2+fve2222YajcZfV6vVnf1+X6ucmptYlRzvVv"
    L"JrF5PK1vlqnLxvsXDVRYZU9DUW96WUqFQqmJ2dRZZliKII/X4fURRhc3MTm5ub2LZtGwAgCAKEYYg"
    L"gCEq2UQhBcRzz7bff/jMf/ehH40984hP/o2Myg3e9613z73nPe277q7/6q2PdbjcOgqCV5cChmCbV"
    L"wwbG66y/AImZ+wx5ogIUFaUgCNbSNKVut8NKSfT7OSeQJLlHKKjSarWKXbt2WSklra+vi+effx7Ly"
    L"8uWmcX8/Dymp6c5DEOenp5e2rFjx9L09DTe9773QQiBM2fO4Omnn9bf+c53vvHEE0/81yiKTiK/4p"
    L"XRWvuuvgB5eiy28eLiojh48KD9+7//+z9utVrv3tzc1FJKdSPBT7J4v4t5/Hl+eChCQ+HWi5J4o9E"
    L"YSft8lD89PY2pqSlorbG0tFS48ZxDcNbvA8tCCTqdjt2zZ88vfuQjHzn8xS9+8UljTP3pp5/uPv30"
    L"098CUFFKTWVZpvNaNPw1Z97408jNYPQSl+l1nrHciMOMn/7pn567+eabv7J9+/Z3NJtNPTU1o2655"
    L"Vasr1/Fd7/7bUxPT0NKaarVqux2u1heXsbKysr3T58+/eVer7e5bdu2t05PT7+r2WxubzQaCIIACw"
    L"sLeP/738/NZpO+8Y1vxM8999w/PvPMM1/tdDrHAaS1Wk0bY/ppmvY9iy+0thR6cStS1r//+7//CWP"
    L"MEenGmIiIxsfRJln+6DWNxMQrn4wDxgIL+LORfqgoWDpfIcZfUzxWCL2wfv8rEdlWq0VXr1498nu/"
    L"93v/fZZlAgAHQcBaa6PzOrAdi/W+u7djj/k/S10oNZNCAD/00EPi0UcfXf/Qhz70q1LKL3S7nTd1O"
    L"h0IQTpJEnJoVURRJNfW1nDlypVja2trXzt16tRT7k1lu91+FsAXtm/ffufc3Ny7KpXKLUqpm5577j"
    L"k8//zz//TUU099ttfrfQeADYLAZFnWHwwGkRP8iJv3BD9S8Tp8+LAEoI0xP1mtVoM0Ta2r5o0Ivlh"
    L"y4QvW337muV9fAKM7DJzAxvmO8RSyeC+lFCqVyigh5NbhOS6/VIzidb7SKaVEFEU8Ozv75ve///1v"
    L"fuSRR56TUtbiONYjE7yjQvQBjxk3bO/zUx5muB4DFEOGX/rSl1584xvf+O577733f9HafOLEieNKC"
    L"EKz2US328ULL7zw9bW1tc+ePn36iPtjqlJK4XrVyBizcfny5WOXL1/+AoDquXPntsVxbAeDwUkAut"
    L"FoUL/f72dZNvBB3ZjQ+UalziJcCSHe7T5sS26m6qXc/rjlF/MP/vfjIWC8yOU/7r9/lmUl0i94EaU"
    L"UqtVqqQgFVhjHEgXD6PVQchiGtHfv3rseeeSR71UqlTCKIm+ubeIaGP/z8hckj1/TlF6uJ5APHDgg"
    L"Dh482D1x4sQnf/EXf/ELMzMz/95aewcR9dbW1r781FNPfc0BjnoQBJRlmS1covtKQRAUWYW9du3aW"
    L"QCmUqkoIkr7/f4kazcvJfTxsU+HV6bLwosnkJFB1DGv4At70v3x8u/47EOhJIUw86mpYZioVPL1Om"
    L"6NK6SUqFarbrlGLvRitl9KWXoHv7paeBZHaQfMrDzDuJEwbyR0flVEkO8J9u/fLw4dOvQNAN/wB4c"
    L"AzDabTer1er2CxRo/Ex7nJEn0BJTKr0Lw8DmLojvWd+/XTSJ7ilFYme/ux73ApC6nQpDjo/BSSoRh"
    L"WD6v2+0iyzK0Wi1IKdHtdkcAZxEilFLlhTeklCWw9n4fGWOwurp6BUBojCkKdmZM2NYDeuPW73+md"
    L"gwTvLQCFC88dOiQ3b9/v5ydnRUnTpzgWq0mX3jhBXHhwoVur9d7NWwj3+APfNW0pd8EGcfx8Varxe"
    L"PgbzylG2/u8IU/7hnG+YPxrqZxb1IUlqSUxUUc0e12MT09jSAIcO3atetCR1F9LECj2/njKyqlaWp"
    L"feOGFKwCqWuvxiyCPe8MbCXwcLOrx+P9SClAogfFSCTMBWLyW87qqV1euXGEAaLfbTy4uLtJ4/H+5"
    L"FPCl5hcnMYXjPYz++yqlygJOvV5Ht9uFMQa9Xg/btm2DlLKsJvr9kUVNwVc8Fz5to9Gg5eXl088//"
    L"/xFIUTVFhs5bmxM1vMQdgz5F2G24AGuUwDxKoTGYxrGr/H2us6DDz5omJk+//nPf2d1dfXZ7du3k7"
    L"XWTEoBbxQaxrMCTKi+3YhJHKeW83F6genp6fL3xHGMq1evotlsYmlpqSSUJjXFBkFQ4oQgCLhSqdC"
    L"jjz76dwD6LvRMsuhxyld7Avdv2mNQJ+4QlPjRPOIzn/lMeuutt55YXFz8b5eWllSv14Pr9h1JBScB"
    L"Q/82aaZxUr3AL9kWp16vl+9R7D5KkgSuHQ3MjKmpqTIbcBO91/VTOGUwCwsL8ujRoyc//elP/58AS"
    L"GvtC9endfVLCD29gSLgx0YBnnjiCX744Yfl7//+75/ZvXv35ZtvvvnD27dvp16vZwFQYZWT8vxJa2"
    L"vGMcONhD9+v1AAP0Po9/vl+0VRBGNMqQQFL+DjAldV1UtLS+rIkSOn/+RP/uR3oijq5uMMnE4QpC/"
    L"oZOxrOpZWa7zM9tAfVQ+AQ4cOwaWs39mxY0e32Wy+/7bbbgsB6DiOSQhBPvgbt/hJIeFG1cJxRShi"
    L"drVaHckQCjBYdEQXpeHC1VcqlbK45BhC22w2baVSUceOHXvuc5/73O+cOHHitJRSGmMiT7i+ZaceY"
    L"+rffOFbjxnEj6UCFJ6AmcVP/dRPfXPbtm3fiaJo35133jkzOztLaZqaLMuIiMhnBf37N6oTTLL28Y"
    L"mogvDxuYDCG0RRNIIT+v1+mQa6djWuVCp2ZmZGRlEknnvuuc//4R/+4S9fuHBhuVarqTRNe06osac"
    L"EyZjw47HH07G6ySvCWz/SCuB4C3744YflJz/5yZO1Wu1vNjc3gyRJ7tm5c2fgsgSTJAmstTTOA0xS"
    L"hHEwOE79Fo8xc1nR88mgAgQWryuaTOI4Znepd1SrVTEYDMTp06eP/9M//dN//LM/+7ODAAYLCwtBu"
    L"90eeJY+7t4nCT6bUPvnfzUK4MIBHzhwQHzqU5/afPTRR79UqVQeuXTp0h1RFO26/fbb1cLCAlWrVS"
    L"ultOWOAjcrOIlDuI569JTA9wRFbd/fm+QsvFjmxFJKrtfrpl6vy0ajIdbX1+n48eNXjh8//l/+4i/"
    L"+4mNf+9rXnj5w4ACeeOIJiqLId9/ZBKCXjuEB/VJ1k1dyCD9Gp2j2pHxoH7/6q796z+7du//d9u3b"
    L"f/Wmm27as3fvXiwuLkIIgSiKbJqmNssyYa0trr5Fk1C/vxjD348QBAFqtVoh/HynBBHnfaesKpUKk"
    L"iTB5cuXcebMGY6i6NG1tbW/PH/+/OOf/exnLwPAgQMH1MGDB/WN5hwm8PkvlVq/6jT7x0oBiuPAYQ"
    L"mAfumXfqm1c+fOD83Ozj64ffv2u3bt2nXbHXfcUZubmxvv0rFEZLW7omnRAeXuj2zjLDyAlJKyLBN"
    L"aayqenyQJLl26hAsXLlxeW1s7t7m5+Y1er/d/f+Yzn/lm8fr9+/fLQ4cOvVSsphs8xj9IYu3HUgF8"
    L"Rdi7dy/5CxTuuuuu8L3vfe9trVbr3mq1el+tVrtbSrlnZmZmbnFxEfPz82i1WqhUKiXIKwBcUbzRW"
    L"pd9gBsbG9jY2MD6+noWRdGFNE2PRlH0TLvd/sZTTz31nePHj28UQiIifO5zn5P79++3r7ef/wd1fq"
    L"wVwA8Nhw4dEg8++OB1FsfM9Lu/+7uLjUZj99ra2k4At4dhOBcEQb1ardaEEGowGEillAiCILPW2qt"
    L"XryZpmg4AdLXWq8aY5SRJzqysrJz99re/HY3//ocfflgeOXKEfa/0w3L+VSjA+P984MABAiCOHj1a"
    L"1Dt+oMr20EMPyaNHj7J3GTf+of0wsHWImfHQQw/R3r176ciRI3T48GEsLi7yoUOHJgpu//79tH///"
    L"rKb+ujRo3zXXXexW9jMWx/p1tk6W+dH4/x/I8GcLno3JhoAAAAASUVORK5CYII=";

// Globe icon for Network Map (Internet), higher-quality artwork, 128x128.
// PNG RGBA 128x128
static const WCHAR* GLOBE_ICON_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAAIAAAACACAYAAADDPmHLAABhQ0lEQVR42u39d7jk2Vnfi35W+IUKO"
    L"3bvzpNz1IxaEpIlPCOCkBDY+JqWjY99rw9+8Ak4XBs/xwb73JnBPrY5HIJNMjhgMGCswSQLBAJpZp"
    L"RGqTXShB5Nns55xwq/sNZ67x/r96uqbnoGzWgUsKl+6qnaVbVrV9ebv+/3fZfia/QiIupLfQ+llLT"
    L"vo5QS/vTyRy76v+f/3KwStfdFRM3e/x9dAdTXkpBmLfYr+iX8D+wdvqYU4LW83HsvCu6Fe+/llvvv"
    L"V088saK4Oz635+m5+Df3AwfjYyeu3xIehEO3nJWbnzgg93Ev3Hdvoxh/Gj5ec6G/gqueuX3J63vfK"
    L"+aBBx6wn/mMJA/IA1bkHq31a6ThIuquex6wBw681yCi4L+f0KG+Bq1evcRjcvFzP3fwoF7Kc3Xgll"
    L"sCKK8V0prqO98pmbmKrtt9bgf5C5eleriSpXrednQ/TdJOqlWKEetdTV372jtfjgs3rCrZqKS3euJ"
    L"scuT65c2jr9u2NL7vf3nD6OIPdODAew3A/e89EPgTHELUV1nYL/d52vsXCP7nDh7U+9nP/v33B6Pe"
    L"4wNwD6Kf+n5WtpY+fdv8gr9+btndmWRb13e7smehm+/u5q7X6So6WUKeGTpJQmIMVhtyvQAilJWjr"
    L"oStcotx4Rit9upPPuLWvE6OhyDPhBA+p7Q8PTe/+Ond+8en7rv11mrySe+5Rx84dIv6k6gMXysKoC"
    L"6y9D+iDPc/8YReueWW8A1KOYC//33Se3b3x183v423dOfHd88tlLcuLYUrl7cnLM9b+hoSgUwU2ht"
    L"00BKCiBItSoyIaHK1yKjosFkW9PJElSGgTK16GfrclvC7nwh4leHFIl6oiwJX1usSwheU4qOJTT6y"
    L"8wr78R/9czeem1WGe7iX++5T4U8V4JW5+4tfpw4ePKiez3N14JZba6OR54LkP/prj7++kPV3p53hN"
    L"67MmTuX9kk6v1CwoDSJVyTeBO10MEErQlClq5TzJYmyKjcLKAQlUcsy3efF4xUnVkcIirObJYfPGb"
    L"L+El6MpHlGUFqCCxJQiASNsgoSCJp6VBDq+phCPWAS9b4bbtB/eN87b11t/Ja6515R992HfC0nkeq"
    L"rIHz1x/39g6DPPPOMftd111eJ1vKPf9Rf+bx67J1VuXlgNF59667rskxJh739EZfduBYSjSyoRBmd"
    L"KE1QKggKTagdShxdkxHo4JwCgcRkGJWgMCilUCh8EIpxyYnVwKeeN2yqOXrzHQbDGkSQKFMQkQCCU"
    L"iG4YBSJMiZHyopyq35eqvT9YWvbL//W/7PzYcE1TkH016oiqK+w1auXcf0cPIg6Mx8Fnxorf/+Hh7"
    L"cdW/jwX3U7njnQL+evOv65Heg6cNMtzi+bVPn0rLrzdQtqJdtDlngwIyq3ReWHGB3IWeC5Y/DC6Q5"
    L"5t+TGPZrt+QJBND5EHEwEJASMjsqQJpqtUcXvfrbgxUGPufmcqobaeUQpCBBmglSQIFaZEPx5vevy"
    L"sdp05xiP15zb3PeHxbFb/sM1C9e+78e+T42jJojmayw0qK+gu7+U9StADh48qM/Mz+t3Xfc5l9rv8"
    L"n/3R0a3H1v40PcUy4//5dG2p7fvWSpZOXx7OPy5m2X+sjU9d8fvqTdzBfWp/aT9PWTdRU5vrLKtV7"
    L"Ktb3no8Ij14Zglu52Tax1Ud45QrLFrviYhcOvewBXLc4SgJxWdQqFRBIEsjbcPPDbk8yc1em6esvC"
    L"ICCEQb0UhQRAR0tQw3lpn561PhpvvWJXx+Lw5VzrOHdnJxnNXPzw6fsNPfe+bbvm1b/1WVcbQcI+6"
    L"7777wv9oCqAudfvEE2hueYI7k9uqH/jxcO3T+R9+T7Xzse9Orj68fb14gY2N0t+6uENf99w71LG1H"
    L"uGNfwjqGXSAO5f38/rFb2JjbRUGu0h7wgaP8eLzt+HH8zxbBE7SI81TBg4qpbGA2TzF9+wP7JubR4"
    L"JCKSBEYDw6acEmFm2F935si6fWcrpzHcZjRxABEYKHECA07iBLEjbOrrJ02Wne8nW1SPp0GNdWD80"
    L"29fyLm5w5tPfDyYmv/6Hf/L/2/W6QAAfea7j/QPhqhwX1FYz5CuDBBx9Ud999txw8eFDn+X51yy24"
    L"3/oter917JG/urnv098nlz1xTd47T1GO/PqW0+JytTh/jrfLXRw+n/Fo933ctnAFN2+7jo7P8GHMx"
    L"vBFXr/yDjbkMJ859SDb1PVcOb+Xa+beSDW8gRe3XmS9Fk6srvD5YcLAW75l5TjvvnaOcZVjUGhRiA"
    L"S0VqAUQSl0Ejg/EN770QFD08MpjXNCaCy/VQLxAVGQp5bB2Zq8f5JvfPcGth6ysbkzbC2+wLo9oo8"
    L"+sizDZ27/pbnzd//LX/4X+aG2auCr6A3MV1L4gLryxRd5YnilOdEd6rtet1KdWvjrb/mkf+inRrf8"
    L"wd9Vez+7rGQtjIoRw3JdaztWS3M1S4ninD3Cmn2cReXZblLKasyoXqVy50h1lyW7wkZxnDWXcMwNO"
    L"FWukiphobdJ2lnjxpV5bt51GYfPlZwLGqULNkrNVYsJqU7RSmG0QSuD0hqMIkstla548lTFyKeYLE"
    L"EZSKwBDDZViKhYtypFWXl6CwnVqMuThwJ7rqlJus8qf36PCuPL/I6rV1XvqlOvOz1+4cC1d/zT6qk"
    L"Hf/qR+97+dseB9xoO3S9/YhXgZVy/vjjuv7++0vyZG3HVNp8P07/298cLX/jX5f4P3D7QT4RqXMqw"
    L"2NDDek15V9O1mm1ZigRBS2DJZBjxDPwAqzw9VZOJo/CKwp8HOty8sJ+VtE8/6bBaHqarHLndwbjuk"
    L"rHC8QEc1QnHVlOkdLxhnyM3PZTWaK3Q2iBa0cs0RzaEn/zwkHPMYedypA7oSlONx6RsUBearJsQmr"
    L"JSG01RBfKeJZR9zp5WXHnTKrvn1hlvdvXTn71ePffJvf6G28Jc55ZPvOsnfuX069/ydf/liUf/za0"
    L"n4/ckCu77iiqA/jIK/48kfM88g/lzN+vyxx994bof+1er/2mw+/F/Uez/je3D8HwYjcZ6k3N6rIZU"
    L"3mC0Zj5JSRX0U0U3NQzxlKRc3bucN/RvZ2/nGs5Jn8TsRcs1dM1OzozPU3hDbgyJ0RwZnWHkhV2dy"
    L"1jJEjacoNKMq7Z5vvPOdTpWMMZgjcZoi9WGuSzF6oT3HRqx2Vmgs61HJZaifI6FlU9w2TWHuPKtH2"
    L"DnzueoC4fJNMoolFYkqWZce9J5y3CY0i2v5i0738q1V55i960fZikJ5umP7pRt/X64+c8//O6tK3/"
    L"y9//8P37+r9nExDLxHtF/ojzAvffee6nSTl3s/p8Bc9026vL1f/EdLzzqf/HM/CfeunHt/SHrjhmU"
    L"W7pSmyz3LYmFfqpZ6Wb0E00n1RijWVZLXGa3s+h3cfz063j65O3odJ5tWZ9idAcPn7iRsetSBc18v"
    L"slG+Tw77DxJsshaCNzYvZEHjwsf34JkrkvutnjdjpwlM4/VFqUMShmsMXxh1fPvPrvJ4doyt9RlWN"
    L"bkViEhY30L+p1z7O4nnFvNGZe70YlGaJJJVCwsDBhJOPRUxbky8Ka9VzG/NGT+xmeoN+bUp993g7r"
    L"uxtTvuPMP5kb+8F+4/Lof2vHj//bffvCXv1M5EA33fUVCgvoyeIBZ4XPwIDrfj7oF1Pf916e+e/OM"
    L"/Msz/U/Mn93zi34uNyb0KkZs0jOGHI2rPD2bMp9maITlJOfabB/F+nY+/8IVnBtchd1+G2me4MvzW"
    L"OtwIYfubmopcNWAGzqHeedln6NvO+zJ3srR0SZ/cGyeQ+USncUOKk3ZOr3JW7ef49v3wZLajRfBKs"
    L"1mFfiRT61zXOcszucUhYfg0UFI0RivGK8O0DJEkZH1Org6EILgAwQXS0UfhNRotNOcPLHOrTcMWe7"
    L"nfO7Yc9zyxnVOHLyc557u8bYDh4PseB+rx3bowx+6+7evWP7Ov/dv/rZ6Xrxo+PJjBubLIPxZYEfn"
    L"+RPqlh079P/2y5//P8fr9l+cVB/Pz13z875rxdQM8MmI+W7GXNYhNwm9LMEAqTH0leWmZC/nT1zHh"
    L"59/Jyz/WXr7riBftNAxZMvLZNkiuTvB9d3HuTadx+Z9jmz12dMdo2Qnv3J4Bx/dWuJMtkx/pU9pNA"
    L"4hzVJGW2OuXvQsmhyjc4yGtbHns+uBut+l8oJWoDQNWhhAe7J+iun0MJnFeyGomYRHxZZklmrcuKY"
    L"sPcsrfc6eT3jhmEYPrubYU33mdg1ZO7KdzRNLavmqoNT8Qb9y1Ymbnn987VtuuPznP/L0I91TvPe9"
    L"hvu/vMmh+XILn1tW0u//Dy/8kGb+H6yPn+PMtf9GOl1ntHHUbky/l7K9N4d4oZcYOipDi5CZwPXpV"
    L"Zw9dT2fOv5NLF5xA3YRKiU4rRFtyBLBF5rL849zw/Jv8saFgjd2Uz496HFMX8nj4xXqhSXS7Yvo3F"
    L"AGQSsFCvLUsD7WVHXJVUsl89aijKOqNZ887xgkGVqpaTzTxJ9FIYEGBGr/0wrR8bWiFGjwtWL30lm"
    L"u2l1w9lhBSHt08hSHI/EOZdYZHU8IW56l/Z+jlrHO8sotXP/0jrPHz35DePxnfn/tF77+3IED7zWH"
    L"vowVgnkNEz8108TR+/fv4dFB0v2Z+4/9WJ7N/68bayfCiWv/vSq7L+rl7hxO17hQ0e0lZGmO844kW"
    L"EDoJBrlAzYYilPvYmP5NvpLmi0P1hq00SSZISs83fKz7Ok/wLJxrIazrOgVTlV7GczPs2wCytegFC"
    L"6xMctvkrWgoNtJObsReHGr4rPnCjbrDQbVmMdWM0IvQ1SDEEaZThVCqXh/cpUm9kcMQWmF0ZZivMr"
    L"Nd5zn1quEFz/vGZwekzmHjCs2ngfqESvf8UH0zs+DV1R1R8+n+Mv2H1sZd09/mz70Ix/4yKe++cyX"
    L"UwnMa5D4XZzwqZN79qjPc77zn37phR/v9Ba/e+PcSX94z0+breXPqoV8nizTbNUbZInFucCwKnFlo"
    L"KoB76AS8Alni/NcmWX4ravZSnvMz6U4pTFNtp0WFW/ufZ7t3UP0zBxzyTy5WaQTtjMuHuHrF5/ijU"
    L"mPx4Y5yVwPB2itUboRnAR0knNeL3A69HjyrOXRDQv9HjqziHCBF9CA1ip2FBtFiPejV1FaRZSw8Ji"
    L"yJGxmPPKgZ2SH3PS2VZIycOIZQakEHbpc8eazFG/5LfzAYY3HGI+Vrs515nfsP7a8bs59O4/95O99"
    L"/OA3nP1yKYF5jTN/Dh48qPM9e7J//SNP/Eg2N/83yvUN/2Tnp8z6yqcwwbAw38GbCqvBojFYfAXVK"
    L"FCPPFUl+FrwNfR0D8mOcGfuGQwuZ6hz+nMpOlF0jUXGimvMY2zvb7KUbWc5X0GsMJCn2dt7ipXccd"
    L"XcTk5sdTiX9jGJARWFqJVGKUUwYBJNZiHPEtI8RxKD1xoFmGjUqCbLVwqUNJauQCsdO4UuwKhGNgt"
    L"ko6BaL9EBUqNY6ASerLfo7tLs3VVjwpgTzwW27VV0rj6CCetsy1I6JqVrUjos6i65W77z2NKWnH33"
    L"+CM//N8+/eQ7Vu+55x790EMPyVdNAUREvYTwAdTPHUTv378n+cc/9PB9ttf9W2os4UX/2/rUrt9RW"
    L"it6nYSsqyA45tIOC3kfowEcdakpBxmJVczlmp6xzGUJXdNhU7/A7fkJQn05G8kSPS/owRpL5uNctn"
    L"iI5WSZRM8hqs+J0YCtcIqd+TYStcx8skhZLfOFKmVhPsehmjwgeoGphDXBapzR0AgfVJPYNRre0FU"
    L"atDiiP7VHBhVslMigREp/QbvQVRVvffcqe7IOBz+T8YZvO4V53W+TDpY58vQSu15/BNRZemaOFENH"
    L"WwIVZwunO6T+ijevLg/U2turh3/2v/7Og985gns0r6ESvFagg/q5nzuo/+Z+5Pt++GN/iyT9e2ok4"
    L"bnB+9ULO35BmbxCdE3WMYgT0ixje7aDbWaBTmKxoimrKISFnmEhy1jIEzpG0SWho/vM90uu7Z1FRh"
    L"X76k9wY+dneMviR9hl58n1MqlagJAzbzVX5nuZ05exkKzgVMrtS+vc6DcZrw6Zt6CtxhqFNWCNRhu"
    L"NsvGqzRTU0UaB0TEDnDymQauI/2+VhLUxslWB97FaaF6nlEJpIbEZD/7uIktLDntS8bnf2EOvvoOV"
    L"Nx1Gq5qO6tNNNTUVozDEhYpKFZik4PSoMLpW/vXf89Tt2//ar/wCSriH+3gtSanmS3T9jeUfNN/7F"
    L"99UP+bf9h5H8iMGnT5f/aZ6Yee/V0m/QFtNp5ezkKbYoOimltRCZjQuODZGBUWl6HcV2zsJc0nGnM"
    L"2YNxmZMuzL59nOfj4yvh3pLvO67OPsSk6wkO6ko/chpKBS+sk8XbNA124j0V0MOVoL3pznzl6HzUH"
    L"CybxLnhoEiV6gtfDGzbet4dnnNKDbZ11ABlW0+sKBlymPLcROIrFhiHcBm1pcITz5mEP5RdaPdTh/"
    L"POXwwW3UJ+bZ/ZazuPx5MqVRWoCUbrJIqg2kgWFhtU/XXbXv/A1644Xtv/5zH/ydu+4Se/jwa+MFv"
    L"lQPoO6990H9vW9+U/2X/tkHvn5QuB9LVa93tPyIHNn3n1S6zaHThKyTMdfJ8AheAsOtivWNDU6un2"
    L"Vja4h2mn6u6GWxGSNobBOj53RCbgJVPUet97Ddn2RerbPSvZptnX3kWUIn6dNPFklMl04yRzedI7U"
    L"5Whm8F8a1xrPBHUuKpdGYPHgSq0mMwhqF0QqtNdo0VYJRk4pB64jxgxAKh9+KglcSrT16BiZVATMJ"
    L"oTIa5zxK56h6J9pa8gXN8Ohu1Npelm4pUP0X6IimRFEhGAt93cGKpT59PbnfxxX2enPjzWN3zTec/"
    L"d4bb/tP3/PQQ/e5Awfe+5p4b/ul/PL996PuvffucO6y39vz1Ge3fmR++65d5zeeDsevvV+nOx1JyO"
    L"j0cxb6OcoDRcBYjVEKRKO9xJoaQGtya+jkhlR7yiDsThfYrXM2whiTPcG35stU5gTb85xOugdNjkb"
    L"QRmGVxiqL0Z1II5YxooUqVAQviPbc3DnDufE2PiI5uYZSGvjWRUsXFD5ogg6IEQQd6WCFJ4xqpGjo"
    L"YUahRCA0rkOr2NbX0f1LiD83BCJqJwgBV3i8UygjiHWkSxvsy66n1oH14iQbCGMpcKEk6RR0Nnp8/"
    L"NcXeeM7K3Xj/kVddTv+cG/zR1/3rT/56fvvf8/nImT8paGF+otN/i5l/SsrDypjdfjcB479n1ln7o"
    L"2bmyf9c5f9ey2XH6WT5XSXEpYXc1KrCRIoXYmXmlQrUiNoqym9wmtNLzP0OxqjhUWzlxvTu7iKt7M"
    L"Q/jJ9dTMhOc987zNc0ff00r0oyXBotOqjpUPlx0QHmpKEhI7qkilDAuR6jrVxzvOnPBsV5FqTasg0"
    L"WKVIjGpywdbqNUrrSB4tHL6oES9NXG+tXDENGkyyRKWkEb6gJkzCGBZibhBDQ9ZJOf+53fzGv7iW0"
    L"Ym7WepcyQ6dx+rIZOhTd2Avd9z1V4Z84g80n/vUNl13OipNdV9G1c9fe+2/yuDeLzkf0K+kArgwH3"
    L"hQf+M3vt29/l0/+ZeSvPs3wrgKR5d/XZdXPM783AKJgV6WgoVet0PeydBWxV57iMzcjbKkCB6lLX1"
    L"rqesOx9cSdLGP488vs3nyek5tLpAUr2dXfiu5XUGrJVzQCBZBUUqNSIpVHerQ4QujOQ5uLfLZ4XYe"
    L"G6zw7GAHHytv5IPja9hkO+eTDt1Ek2lFoiAxNCGgSeBUyxASQlkT6tAggS3QI81X3nA8VTO40DwmF"
    L"ylEJJfE14gCdHzFaFhjOxY7WuLDP9+nHN6BU/N0Q8bCUsoHfzHnN/5hn6q3yp99p2bt2ZylWzK970"
    L"7n6kLfEXpb/xzuCwcOvEd/2ZPAi5O/99x/v/6p7313+G+fWbpcmeyX+vNL21aXPinnrvt9nS9qsi5"
    L"00gTTNeRpRt+kGBF8GbCi6BpNqYRxEIy2aKPx2jCocpSxzCW7uHVuL7oSbN4j1z06XEM3NxirQSms"
    L"7hIo0NQonWCUYRSWOCs7GGRL1P0lNtU851hgfnEBJY47++dRusPG2GNEkNTgp3lbIyUIdSCUjuBCd"
    L"PMi4APKCxJihich+ncJAr7J+gIQIleQBioWicmhSGQRycxrqtLT6WtGZzVnHktYesOQXXOBpz+ywi"
    L"d+a4muW+Cx3zboyzZ5y9cv8IEfGlM7VDmow3gw/Lqdl3/Lhz7yhz9xGO7R8OqSwi8pBxgPwj9d2jV"
    L"/lbMbfuPajxq9rSLJLDZN6PeXWMy6aHH4ssboQLdjsd4zUjWlh26akimDTRRZYpnrexCPlSGuPIOr"
    L"d5CnLxDcPsT3UdJBKY3GIJRoUVidoTFUBLYlQq5GPCGWzFh6PYvqWYa157IwIreKNyTn2L0l/OF4N"
    L"7aXYb0QGkvwgHeCq0PjxhWiBGniueip928nBNUFOGjj+tvXeom5QvN8612EgDaKTtdQDQPzOxSDwj"
    L"OoCozW9IfXYrSK3jMs8Oi/VRRnz0NI2TquFSYobY0pRoOf3L//Z9908OAJz4VDNV8+Bbj77gfNQw+"
    L"9x91294++O83S/ylNdBhzRo97p+nOpyRWkRiLEsE0H8eJI9VCr6cZu5rxyDGXZGzLDHmiqQARj4hj"
    L"dZCwK7cYFuhl28nzVbJK2LFjHaNyYI5aj3Hi0dqhVA4qwfnz1DqnYpnMGnKgRPBApsClXT5Z9cjcG"
    L"quSkeQZLghGgW54mSEAImjVsH51I9SmydNi/SIKRWhifBMCZOJAZryJanKAwKQ2bO4rrTBe2P3tz7"
    L"N46+e5Ps/oG0Fcxq63nOWh31/hxFNCkJJ+p8sz9wvpvCeEkmCUNrnyvjZ3nCuOfS/80x+HAwbu91+"
    L"WHKCdn7/33nvV3Xc/GL75r/5iz9fuB+eWF4xzI05e/n7F4gCrwaBQBuow5uzoLBvVkBBqlAp4PLWu"
    L"0VbTSyxz1rCQmKaFKhROE3zOfKdg+/bA+uJjbJqKhQXHqEwZFwuEchEJnloGeAqU0njGeLUBKmVnq"
    L"jBlTVUHEhdIgcRofH8O1+9xvrOLen6ZpJehAdvYjfjGYpEJ8N8OjbTp3GwcDEAgeg9ai59RiYkiKI"
    L"kDxU15CGrSTHIhsG1XydzCJh1TMC8d+mnGxpZhawOS1OJrxcZ6QSAwXq9wY4d4Ie3kSiVB6mr8j69"
    L"44/+9C+4PMRS8xgowu1HjwQfR9913Xzj83PG/3lmYf71GhbWVz+rh3i+QaAMS8EnMjAtXM0aoqhII"
    L"lHVN5UrEK1TQOAK1CKPgGddglGFr3GMl12TZWY7KI4yzz7F7fiuG4GIeaytCskEtG8AqWmkSnZKQY"
    L"biRM24XR+se16fCNW4TM9hEnGCUYAgYBUluUakhxCIP8ULwTbbeuurZudQZ1nZQrWCbZK9N7pBmLL"
    L"m1ci5QhPa9RKKHUcR8QNmEJ375WtTW1Rxz6xQEilBgupB2NTZJ0MaQdxLm5npkSZ80y+J3rZXO+3l"
    L"Iss52VWz8g/gnD6nXXAGm1o968MF7/Rv/2k9sU8L3ZZ1MarPB+pUfwywFRHt8Hcg7FrGKEsEESAUS"
    L"0SAeheBqwXnHhnes18JGpdgoYavI8MGyvSfMm4Segqtzi6tHrJmzFPkTjDmJMy9S6cNolZKZnZRhO"
    L"2vhegb2OkYyx5Ip6dqCIjhKDKkGJYKWeKskZvhaaFg8M2FTaCyeiE9I67CbPv+sD5jKuhkZmzWaGb"
    L"0RpirTZIVCLAVFKepRypknd7GqKp4tj3JkdI7Ldwz55m+0FLWjN5+Rp31GZpO7/8lxVvYp6mhTzG2"
    L"f10tXJpJk5m/e9JafvOLVeIEv+sUP8qBWSsnGU4P/d9afuyoxRgZzz+nCnseKQAgorbFK4XzAorAN"
    L"QibBE0KgcNHaK9EUdez6rhaBcuSpfc0ViwVXdSwj1+HZUcZq1eFYNQJ1CJscQZmzeNlCZESieoz8l"
    L"RxnJ5udRaq0izGaIy7lYNXnpF6gM9+fCCR++Y0FSszKHRCa2ly1Ap+x4CivmAgSQsziZ6y6tX6kSQ"
    L"r/CMlOJiWGaqpomWw6UDjn8UFTn1wmy3ucG61R1wVPDZ5mz+0l/WWL1RkhG/IN/+gx7vqWp7jrz2p"
    L"8ZUgSCF6rxVtKv/31g7nRxtnvBYQDr8wL6C+2DPzwD36D++bv+8WeBP+/Zt0ER8lgx5OId9RbHjd2"
    L"MQRUHl85cAE/rvDiKZTHIVQi1AScj+52GGrWC4cYw/Zeyg3dDvOqw87UcNVizbb+JrvzEbnRQEViO"
    L"vTV1fTUHQS5ghf9HMMsZ1MpKsBnKbrbY6mX0+vYWQOcCDQIeIQSiXRuiYMeQSJaJ0gz7TEj6NB4gn"
    L"YsbCL4C12/MFW0iUNo4RORmcAS73kHed/w9Cczqs/dyVV7riQxHaQjrJ0qqUYaVwp3fc8L3P2WLep"
    L"TXcwbHmFpW8AP51EhZ/z8Hr39O45Id/+zf73PZ1f4tfv9KwGH7BeTA9x994Na5L5w5GNH/3zS7V1v"
    L"jQlFdloPsyOIUgTfuHYnlFUgyxOqoqJWwhjBuZo0aLaKQF0bTIgMXF8lWG1Ic0PXwNBXbGpLX0oCn"
    L"k1JWA6KkkDXbCdxu/j0aDdlNh9ZvElOAlgR6kab49xe/MLDJFmbXh1QCDgPPoQo3NAkc42GhNZa29"
    L"q9UZSY7Suk+T19oZdHZJohyEwVMPEaF5YIgFC5Gl0sc+QzY0bjEWbhPOdPKD79Wx43SNh2zSo3ve0"
    L"Y3dESq5QcNkfY/901q0cTiq0lzhzdpaus8Lve6lYGD7//OwfP8jMcuF9zP/41UQCltNxzz/8vPPjg"
    L"Pfr6r5PvzjupBIck+zbYubKLk2oNlceOqUcQcVSlp/QFIdW4WpMqw3AIo0FC3rMoayi8wzlPL03pm"
    L"pyx92wGGNcVW6VjPs25IktZVj06SpPIFTywtZdz3V0sdixKQa6i8JSCukmyTXNtPa0HQjP06SSWhn"
    L"WI2I1vn2/yAReIzF6RBtuJCaJMwkib98dKIRB7AjKzSkpkNgGcUYZGoSboUJNvlGNP3vGceWqOFx6"
    L"+HZ2UDDccstVjadHxlgNHWO4IaZnTNY5b82U2bjnPrptXkarH2c156iRhcMsmp247/t08Kz/Hr6kv"
    L"uhz8IpDAe/RDD90XfuOBd97hRf+ztJOZpOuUO6dUcmQngz1fIPTHZN0EkyqM1TjnIA/YYMl8QhDF5"
    L"nmFcgaPUBU11EKiDJ08I00MtRbGOnDae7TO2J112G13Yetr6MkeHh7dxMnOXpa7hq0Zi2qqr4nAI3"
    L"AXxRQrO6FGUYowFBiLUErsvFUBnI+3dUPlFhcILhBCIPgQrd0L4iPiJ2F6f4IMekG16F9g4iFCCM3"
    L"zUcHi74IEFaNMiFlmXQd8lcSYXHXo7Rhy83e9yJ0HHueNt62yLewiKEdRFIzqMRuFRxVJzLOMwrtM"
    L"09uQ8cjsqR4dfWgw/s3D8F4DfzyF7I/NAQ4cuEUBjEf+PWneTyC4OoyVDBN02Y9ZdEObibVt1HgTD"
    L"GEsjLZqVs+XlMP4xfphIFUpvSTHWkPlHIPgKBBGQWNMxmKacM6POVkXHN24hveu38qZdA/djuZsO5"
    L"UrEeSpiMlkKVCKUAZhDIwExgIFMBJhIDAKQinEXCSAawTvpMkBnOBDmHgBCTIRVJsgRgHG+2ESImR"
    L"q+Q3cO4GAm9dO3T8T/9SGhuDB1dFzKhsYnc1ILnuBsPdJ0noJrTR1FShcRVAeqwNiBScVwZWgA8Hl"
    L"fvmOdTV/+5G/iAD3PCGvRQhQ99//Hv/mv/cjnXMfHf0FlQiu9LqqClKTUfohqlsjzZy9DgqpPUoLv"
    L"hCcAzeCEDRGafAKrQWrApWvIBj6XYW1URA2aDKBtXrMWBxzyRrz3XnK7HryTs26FxIFNQodYmOlxe"
    L"NCg/o14N0kH3BATVSGkvh3yhAVIAo/Wr7zsSSMhuzxvlWAFsMP0cobYesmoWzdvg6t8Nv4zwXVQpj"
    L"1/vJHYUMJgrUagqK3c4SeO8eSX8Qbz2Y9YFyNUFhSk5EaT92EFmUqjHJo6evlPedZvv3Md/Ah+QF+"
    L"UA2/GHj45T1AQzpY+9jgzcaY67X24iunFQFXBLqXjUiSgN8weOepy5q6dNQ+UFaBqvIQFKkkGDTOO"
    L"1BCCIGqCqAgTQwKRRCFCzWD8YizowGLao4r0stZHdcEHOue6LIlkkqiFUeLL4JQhijkEVCIMBJhBA"
    L"yBQYCxF0o/FX4ZYiLovMSmjwso37j7IAQfJhYeJm6cmcqhaepMsv5pA2iaQDJRhtnkcJJUhqZX0JS"
    L"XqKhI133XF+juOoryFic1RT3AB4NWndjw1hatBK0MxoC1FblOdS+zsnTTxuV7r//lP4vAF0Ma+aJ6"
    L"Ac6rd+TdngoE50pnk65HRkLJmGrkUJlGdRS+ipM03guuAlVrqKEqA9b4iLAqRVlKbL0GhXeC1xEpL"
    L"H1BJ8nYv3wdO+RyDh6/glW1jYWepxRB2Sa901OAxhC9gG86rZqI79NYXS1ROXyTBNZNBeCEGK6aTp"
    L"6aSndSDbQoYQigJOIAuo3dLZoXmAI8jTK0Ll/aEDATRqSpK9sqIUiY/tlaYeZLuleeYcl3WErnWas"
    L"2UMU4JowCzitU8JhQ47QieFChQhuFcx2/eNOGXbzm1Dcff5r3f+kh4P73+LvuecAee99D36ytIC6o"
    L"UDmccxinKcKIgKDSpuwSkLEibAmlglwSfBHwzmF7KSKB0bgmSQ2psYgOOHEMSyjGJXv7K9wxfyXj8"
    L"1fx0c0/Qzp/I4vLOYPaIV6BF4LRhIbK5ZXCKvBNFaBVUwE0ZuhbN9/cdxIzfdfW/j62eY1v3XtM/r"
    L"yXSQUQrT80ChGfUxOIMDSK0SiBn3H/IUw8gWpApFiLNoow0y5u8wqjYDSqCS9exZ47MgbFBsV4nQX"
    L"JqBlQ1J6qLqncgMoXmDTHeaFGSHONUgumt/0snatPvQMkuf/XVP3HhQH9ctk/wOnfffhqtLoVEwi1"
    L"1945klQwGJyum764IhSRZDnc9AyOOxZdnyuybVzV38lSv0/taoxYdi8sY1VM/lQM1AzqipV8gSvVV"
    L"Tz55I08ceZddJZvI5tPWHeBsRNcCFQuUDlPUXuKKjCuPeM6MKwDIxdvh04Yehh4GHhh6IXCC2MvFB"
    L"7KEHBOcC6gnMc4j/UBKwEbhMQHdBC0jxl8m8T5RmHanv70uQtHxWQmCWyrgQmqOPEIMsUNWmVpcg/"
    L"tOnzyZ67k7KZiXJ7GFA6jLCJCVW9SuiHrxQbDUIAKeKup66igqe6RdaBz+fkb9i5+6OYo9nvUq/UA"
    L"GgiuLr7e9nqZiARf11pEKIeeTALKtloeCDYCQfWm58rlXey0K6x+eg+jUzvZ803HSXduQJEwPNbh2"
    L"ss9Z83zDH2FVppdvXn2bl3HI0+8lbndd7G8s8sIhys01iqStj1joi57DUEHvAbXkDENU0qWajAYaQ"
    L"CeIGpa2zcWrRqhaYlXFQQJHh9Ah6mFEwKhBQzabu7k+YYMwrS2lzBLAGkIJmFGKbhI+L7xIEHw3oM"
    L"37HzjkLXkRRbPV8xnO/FogneE4BElBBsribKumcu7iA8Y7RCt1ajUPr98YBdvevqtxx/m83fdhX7o"
    L"IcKrUIBbBMAjr0uSBERCqL0OXlCYSfnjfWTNGIFq7Flaydh1+jaO3f91dJb3sdhd4vwDQzp7D5N1D"
    L"aPzN1OvF2y/5sNcd/mTPFc/wy69g3PP/Rk6S2+jty1ltaoRo0lT8EGjnEeJjm7ZKExQiFGEoAgalG"
    L"o28qlI324dXmi8n5+Be0MTt63ErSOxtg+Tmpy25g9CcBGuFe8RH/FgJYIO8Xd0uxokEENU+z4SJij"
    L"TRNCT0MBMfiATfMD7QJoaVGHo33IMv3WSjGW0UozrgqIuqJzHZoa5ZI7hcANRgdQatNTUrkTZCpGO"
    L"ZHs30DtW3xD5G4SHHnrlHkDBe/xdd91jj2yEN2odEO+1dwElEhM/F9C1RSsNRiNVjJ+dNMG9eDnL+"
    L"TUMpGRQbtFdnEcGr2dUCp2VDibZzuqLf5Hh6vPsvvoTWFapwjWk8wmrA4dONRahrgWREIczxCOi0U"
    L"FoloBOuHyTSZ8WaW9IuhP5NJbXbvVSbcIWBOV9kwTGcjCCNwHvw+R+8DItAfyUOIKPTSQdZiBAmXY"
    L"SJyFBZAYPaGllzesiCSrS0sWgdp5izzUHsUNNZSoG5QZBNMNixHA8oGd6ZHmGT7KmpR5wxlEUA2wy"
    L"R6o7Optbx247/yaQ5L4fVHXDXZJXoACR/nDabl9Q+swNaCFUQXnnEeWRrQQSRUqOVhonEXD1UmM29"
    L"sL6XpwaMB4IpqPZ2hhi5zzaZJTrBbrn6S91QL+Rs4d3sP2GX0NTM67AWY/xCqUCWsBLQ8EWPdnipT"
    L"RYLXjdTO5eNKU7e/ESCRlhRuhTtxwfMyKIVzGhchETiOVhpIgFF0OGCgqa3ECFaYcpSJNETsKDTMN"
    L"AuBAYmjSeJCpbaJQOAas0N/6lp5nXJyl8xjh4MgVpagjDimFRobMOeTchSXJUqHGhJu1ahsOa4CpQ"
    L"uer0hO6+zSsv48jKUTgB975kIvgSChB/oVw/d41SzCsleOejKxSJEywqfkHBC1IHRDzduZy+n6Pct"
    L"IhEDCCWV+DLgBiPMhqphc2tiv58SZIGttwmJgXnAl4EpQJOdCwdReOapYwhTAc2ROumFm4VYNK8iE"
    L"QNmSFkNGBPG2uZIHkBHwRNtGYXBFd7yqa55V2g9XoSANcCQgHl43voxhOIZ4Y0OlsaTnOH0HwXbWk"
    L"YGqQxeKGTJlRmizB/mJ5fYaXTZeRGkZOIJ+iaLMvQKqX2Hm1txC20YIwGU1PVY3TWU7VT4uYGvfSK"
    L"h27nMCc4cEhx/yupAu56MD5eV7cZmxlEgnivYsIStVcbzbDawNU1wQeS1HBLto8zv7WNemRBHPgI/"
    L"+J9U4eFSYxVPqBqgbqgZoBIjbim9KoDvvZ4J9Q+4OpAXQVcFe+7KlBXnrr21E6onaeum+y+fW0dqO"
    L"vm5zrg65j51y7gXKCoA+NJ9QBDJ7GqcEJZB2on+AgTEnz874SZvkDcD9j0Di5ADSO0K2HaNwi+7SX"
    L"PJInN+0jTzKrGQv/OF+h0zzNHn3FVU4cadGBcD8FCJ7PUZUFVF1hjcUEIKhDwBHGMizEeRQhpsNsL"
    L"zO61GwHuOnOzeoU5wN3AQ0jw+3RiESR453RowBAlGpsoBsmQypXoYOj2cjqfeh354dvoXp2zuVlgb"
    L"Ba/NOdR3hN8QNcBkgBeU4+FbfMbbE97PLM2JnTB176h22sgYCQW+GIaMmbrBXQ75g1ay4RrN13Xry"
    L"5o015Yok2TuLrN9n2D/vlYzYQGGQyiGsuOU786NODPrHB9NGnVVhlhiiO0Ap8NOUGi1QffKgJ470l"
    L"I6ZkFnIxwoQJJ0CpON+VJyrguKKsaGaaIFoqyQgM+NTjlqKoxiXOgcsl2DMgWix2vDgh66FCsAJRs"
    L"p+2utZ0xEbQxYBwu2UIlGqymLB3lYBtzc/Osr26isCjdZM9eEO+jy3KRXx9cwOaOc2dXyOb6DIc1S"
    L"WNFoWXOBo0YhbZgQ4P4aEVoZ/eUQukQk0DdVACTZkugrQcF+SN1OiHgGnBG2kqgEaZ3YRrLXZz9V0"
    L"FQTiaJW2wi+AbOjTqkWrgxCME17x3iBlKZEfbE0zXuPyDoYKlLj7EDEqWx3lKWQlmPSUwK2lOlNTp"
    L"xIAmj2lMrhw46YhEq4H2Fq2ucgpJNgiquBHjobgIPvSIPcL+P/Hd1GUqQICoiW2GSzPikQO8aEcYK"
    L"09Fop6hlk6IoqW1NlhhMkIZ0EZovJypEqANoTekquskyLz77HvLlJUJVxWxdxcTPB0GHiPvWRmNMs"
    L"+PHx508vpnFa3MB1y5yaJNeNZ3YDTQZNzJNzvwMOudjcBbHhAwotTTzi9EbxDIxgjsxrDET0hrhtx"
    L"WDZyL0GBKaFrEQ281tKPEhdlINpEvn2JZ7rEvRNmB9gg8BlTiCBGyW0el3qQrwOGye46ipncOLx2s"
    L"YFQXjeqTK8QiXDq4GUfygCi+FCOqX2vWz/3t+NhG4TPCE4KMCtDWtClCkJKf2oZUmnFd0vGU+dNBW"
    L"Mz/fIUk1OE/wMT5JiO1W5QMGsKS40lAqg8p3MZKU8cghLsb/UIdpLlAGQuXxVcDVnuAiAdVXHld56"
    L"spTlZ66jM+72lHXnqrJE6oq4CqPqz3exfzAV6HZRBLwdSC4uJkkeAh1zENwDVDjZBqzHTHpbev4li"
    L"fgZuhlMzlAaJRr0mrwrfCJIadZF61NTbZ9E8ok7q02ijLUhJCgyQg+waoenXwOH0w84kYaZnVVUFY"
    L"F2gS8Lxlt1Xpz6MCOV94J6cv1A18SCKpf2EwRWUaBNAoQmiaIrz0SHPbQNSwtn2Q1fZb1QUHR3aQc"
    L"DHHG0+8tkVvI0x5BunGVaq6olaKna0JyhDzvUg0WUd0Mck9VKTQ6QqNGN1O00/gfXKwAggGl/XSuv"
    L"x3UjB3n2ZHNCfIWb+JeoGiGasrQaUo1FRQhBJSbWjuhvd9UAa55bMbi26S2JYdEhWmAoDBTCrpG4V"
    L"wEfsKEYKJRK5vsun5AGGvqbsKgHlEHoWPm4vIJVBNOFGIs43FNCJ6iLhmNxtQVkCRgC3wVSQbBFv0"
    L"TUcblK1aAgXgL0oshIEQYEkFJ3JdXlAXZ5hJzn3kbyTsKjq8dobztceY+0efc6SXmlxZIpcta/1HK"
    L"K59mZ34Z42fejMmXqKs1yvwx0nyL+cuGnDt8J6n7s9hFYVh5tNeERJAQgR9tVGTvmoi7K6fRppm2p"
    L"Rm3JoYBJnx9NcO/Y7LSbdKGRV2AC9AseWz7/uKniZ5uEULXPieT1nFUjKa8nLCGovCnbr4JBS4QHB"
    L"Nv2FYBRsNoLXDm8UX27QdVDdmsRiR0AB09lIoLteoQiW61VIzXq7hgy4eISBuPcjXGGwxCndZJ2aZ"
    L"G8goVwNdjJUESNQFTmgIWhcJQjCq0NuTndtA5eR3dK87w1MZpbr51g8W1yzDjRYbqLKduei87b9/A"
    L"feY7CWWHkFSoepHu8N1sbozZPP8IV7zpD1l7IqNefRsLS56Rd9SlQtsAVkVF0Gq6nEn7OGCi2x5BM"
    L"3PXQIDtno92kY9qARulolVOYMIo6ElLN3iUn2byDaGwcf+hoYG1FUOM+6otbSceg0mSOaGTBcH7WF"
    L"a2ISA05BMJQi01SVjgmd++guUbz7KkzxN0wNrAuF7D6JxSHKNySCAw8FsUowIfPGiF94pKAqr2GBt"
    L"wtY1KrqtXzweQ1VIJXl3Q6GgQwnZQqihKrE1JnrqGlasPs1qc5ql9DzP3+vOc2+xR7nyBMHeWhdNv"
    L"ZPOzr6OzTbE5dGivUbJFf7mPKu/ihY8usu+m5xhsHWXr5NfTW9qNSWqKWkULtBJ7pU0JiFLR1Tdlo"
    L"Pjpzr4WC0Jd+FkjaTPMEDMnY3oNPBzQDXdPtUjfpDcQ3btu8gHlZWrxTZxXbgoITbmDM16gjpWB90"
    L"0SGNpwIlS1R+caKXPGm136yxm5yXEyoizHoEYMqyHDcYFOFCNfUReOWim8lpjjiJ/8/+oiwuhBOWX"
    L"+mHXAL6kAanksnFVKQpjOw8t06k3pmME6X6NPzNN5ZD833P0i54dnOHbFRzELirCm2N7pIM9fTaq3"
    L"UxRjVEhiXV0rNtdKuotC176BU49fwdXf+B/YefWzjJ/7Ns6Mbwbtm08YvYCv41InDJNlDdLM8MnM9"
    L"G1obpU01s6F3HzxMy5xAuPGXb9TDvmU+Kma8fDoBWIu0CpJcBEXaCuKtqSkSfBEmrV3TebvG+EHP1"
    L"WYIBFpDeMe585asiWPcTVbozWkrPF1YFzXOBdJjgPxVAFQHi+B2jnKWjBWUFYoy0AZBBs0cP7VE0I"
    L"kiEhoZuja0WaZYcMoRTEqSZIU//nLWH18F8mcY89dH+WMeQadK3osUR3ejtKBYhCXIrSuVHkYbzjC"
    L"/Igs7/HsR+5mbt/T6HoT8YGOFaRIcQokETodGoQuNHsCYBL4Wwi4WcwQt3c3XkDCdOSLSXcoPjIha"
    L"TTNHaEp+RoBNSBR+7OakENkyvp1XNBJlIlwo+V711QZLkxCAM3vhxn+gBJNPQyMqi1MNWRYV5FQU3"
    L"qCxKGVygklTS4gnjoJFC4wqsE4SDRULiDagEvETA9TfWUKYAa5VDJ2wQuqSbgmB+u0AHejEIOtEUm"
    L"akqYZ2i+Rf/ItzL3jJHqvZ+7R65Cj19Lfm1BboXIBZZoM2gS0MUgRUD1Frt/A6PCfQWUlaRdks2Jl"
    L"z8fYky9Tj7fz9LlFkoWcQR0tpl3mpFS7mKmtCJoBTBU9VksQnc7/TZk5LV6vZhA+Gqy/9Q4qTIXa4"
    L"v2qxQPClGMgM61kCXH+LzQlZpgAQLMwcJNINvPl4oV67BkWI1RRMxahriKUbUSovTRNt0BRebwDnw"
    L"llHbkO3sX/XISIgdLOTK68QgUY1tud5ehIJGzT7S78SYNl2nqNniJQlzWuDugFTXpuN/NfeCPnug9"
    L"QbBswuvGDjNevYZ47KHqaURGbGW1/QFtFXZxG9DyV7pHUmlCcxcsYs3CCzq0f4K7sTWx/4jIeeuw6"
    L"Ov05ygpsDnXl4/en22VNrQeYLnIOTHGhNhRM9jY0PX5mu3ltadj09ZWLYUC1WEbw09JQmlju2yqgq"
    L"fnriFuEFvVzfsb1N95jUp4KwXuyrsFsqymrIdpZzlaueY/4/jbEKWcnnmEZqBs6WNXMMOqgCEojBE"
    L"ItSJl4w+lX5wFu/AtvKJ/+1cNrInIZWotSDddGLhyZbifi28MSxqMKO9/BfOFmFvYc5dnlz8BbA5n"
    L"vYz/5N8jOv41eXxgVAWUAHRCnCbINrI7vZwXPHMoucPrRt7F54koedl2s1iTGwGiLnb11BuPtBJPh"
    L"gp/s8JHJWtc41KlaZZWZ4c2LkkAkWrai5e9d1NZ1YYIfqFmih5+SSnENONSUexFUaoUf8C6Wmbg4d"
    L"zChhiFN8mbQaUAWTmECrFeOjcrhJIaMLIAVME0FUTthGAJFQ8ULOtDDQNAE6wg1hLGtLdteMS1cAD"
    L"5839udeHW02YDQ9F1lChWqWe8S8W5U/A+X5Rg7XqL32bdzU7Kf3eMrKIqSzbf/V/T8Y+gykBgNdcy"
    L"q68JTV4Z6rAilpyoco1FCVaR4trN+8k7C+q2MVm+idinV5irrw0coh+tRIHWI0yHttRak9kglkahS"
    L"B6QdBCjb18aflfNQNywP1/QqmteqOpZWk05mHUNXWxZK2x9xYYLs+Ub4bSs5VM1tkzuEZnIocgDCl"
    L"CKkBFdozLM3cS4knKpqXGoImYnTTRrGIVA4qDyUKhJvY48qsGPHAtZakkSBrsWQkIR8w7GrfhVzAQ"
    L"dMNIJwWHxUAG2b07TavdAX65XSNMs3KQtP7Sr0+jaKX34Xyw//BexaQtnbQF/xeaq1MZomk26+9FC"
    L"5OFTiJH75laMeV4y2KmpXMi4G1OWIelxh0x3Um+9E2yWkLKEOhNojziN1fB+pG+J/7ZmOAfko8HYO"
    L"oBFq+zelalvUvlGOmM0rF2KjwdNUArFnIL5RNtd0EuswTfom95u2cpiyjCcUtDYGaAjiCKXFv3Aty"
    L"/M7KYJnVDmUApMqxiFEQiueQgXGDbHEh8BKtpfk0/vZoW5kl7maeVmWJEnB2acOoauZofQvMgTcdb"
    L"PiIZAQzsb9eCLGGhQ6MoImLv/CFsJke5qCYlzhQ0aeLuHOJ/TLvaSra8iJm1nsd1kdV6DTqFRtSAn"
    L"NOhUd18xE4Kdh4zRL+31wDLVC25pqpKdr3XR0/xoF2jfrXdqVLKGJ92oSAtTszgBkMvWDEAkfs+TP"
    L"pmUsLQTckkDaDL6N8a3br5vs/4KKoMEEgp+4/ra8VsTeg+p5Vg9rkv/8Fi77Js0LxZOMRrYhtAQSB"
    L"T4F5+L/xfmaleUF0k/eSf3I6yjOnKes5ule0wlb/cq4LR4Fgf3/i+Ug9SsuA63NTnjnIBgTlylHK2"
    L"AmwVIXKYO0Y9oBqsKh9YjeaJ7lx7+N/uUfR87txPka4wLB+pi9iyE2+Jsv2ygIOuIAplm3aSL5UzW"
    L"JntQKTDOjrTQqaslkQ+cUFFIT/VLTgDWZHWhPCiUwVYIwwwBuuAOTnGDCDI6gS2i4A6HpAfhmuFQm"
    L"mIBcMGgaWtKoml0iKSgNtavIh0v4J5fYDJ7LvqXk2PAwro74hRNBi8LVQlHVdPuGxaO3MnjyGnq7H"
    L"TLcTsfMMxg/hy+6mHq+ANgPHHxlfIAY+Q3qc94VPvjMGGPEGK2k2ZJ1cTIwqxDSrmPRiqoK9LxB6o"
    L"yTp8+zjzjoaAnoECir5rswgrI64g00DZem9av0zK2WBuVtH2PyGJPVbU2l0u55p/UmM3s92snihja"
    L"gQrs+5sLSsOXs01QB7cKIKbbfYAJOCLWf4v4NqkhLLm2GTqZ7h2auzVIqXweGbkR/McE/fTvjrTm2"
    L"v+P3ebF6kkxn0eP4mixVXDnfoyMrbL3/JtK0w+bGgHQxJzddys5Jo7csnO8fATh4cLe8wiogHlmm8"
    L"+wZNx5uBS+LSWpi6ab8JJ1WFzeRZ/bmyYx11bWH+U3scs3YnqantzMoC6w4rFHUPq6NFR9RPjEKm+"
    L"h4iicK1xAvhOkq93aPH7oJD0o1A2Kq8RYz0p0Awo2QJ4oqk1JwMsUjM9M7E6G3JE8mtXuow3R0rFG"
    L"AlvAxafj41vKbCSM1u2AoNGj1dMJU6YizDjZKevMWdfpq8rWr2L7vRTbOlQTnWJnvss/sY/3JHawe"
    L"XSKttlH5Ii7Ytgkktbjupq6/sDzunrr8Q408/atCAo889tzm7ut2fyF43ozSYhKrlHFN5jptuChRF"
    L"2oB07Vo4oEkkB29AftoCXtPEp64iX6/i4ih6zNcVjEOEU7LTIb4BEQTeqfR2qLdIt4rTBbwPgIgSq"
    L"tJfwA9PcJFVIyPanZdK2HymaR1/6ImIYHJ9o9ZIqdMDHVS8oUZq2/5AFVoBkUh+LgtBTell0/QQqb"
    L"s0Mnf1TOPNQsk24JrPKzo9oW1T1zB4v43sXzFUcbhLPOHb+DFj9yOHuwm14LXY6rKkfaT5sgaR2oy"
    L"1OryYC//r3PPvuoFEQfeazh0n59betMtJsnfYjvGI2hf1s1ctI7sHKMbV6ubY1gMSjXLlnX8ua6Ev"
    L"JfBySswixuM1FHYXKRITrCx60N0pUtSL2G0YWQfpdr9Ycyup+jNDZHVKxCzTmf5OAwTOkkPRfPFNj"
    L"N9uuXaN268RfWmtxc/1qJ9bdI3rfvFM2UBN0zgSZyvm3Zu2/Ovp7z+4MPM7ZQMcgEtmOkC6ckce7s"
    L"xSIXJVFP7OqWFTr3C+Km9lCcWsONFNj/6BjpqD0EPGY0HVGXcu5jklizPEVX6euGsLp/c9nuPHn/n"
    L"f+YeNC8zGfLSHuDME/FzafP54GrEWW0Si7EGqcP0UAWZcf16Zm3HzHpUgI3VgoUli3v2Gqpbf4+1a"
    L"3+bfrqIXRqz/gXYPbqW0dDTv+okm7e8n6LukH3275BUfUYrH2LlHR8gOXEHz//+N9FLt9HVGeQBCS"
    L"puzcqF2jtcCarZ7z89yUhdMO8wxYOaL/wiuFwahC/41o1Pt3lMRsX9lOgRLoJ2I72sQRJbYc6MgE+"
    L"TDdrFAtPP29DiIVCMHZUpyLsp+vz1VKcup9exDIabVEUVTzSzcSuLTg1KFHXYkuJEHz1e/Cwo2f++"
    L"v2kPQv3KFaBJBBMtD9VVOfYu66SdVEyaKl+GCani4tWK6IuBImlO3VAMByXzC33CE99Ies0Kdm0vb"
    L"msR9XUPcPb5Z5Deaczuj/Ls5zfozgnZqIatM4TlwxzcfIqd/YKVb1tDj1cYPnsF4bm3QGeL7Tc+z+"
    L"DoXqzaR2lHuMpPIeF2JkapCaGFSVv7omWOE3ZQuwlEplhNuJDdE4JMPYFMm0NxOdU02ZvkEJNFoWF"
    L"mj6j80aQwurCG4BLfezgYoU38eXNzjASPNnFQRqFQicZYi4hDVR0rz6x4VVcfaxJA/yp3BMXt0xvn"
    L"37k5t1R9m0mzfTa3AUH7qj0YSUFz6nasxzWxgFcopuFAt4+LwnshNwvoM5djqxUylhhtFKiv+106r"
    L"3+Uw+XzSFDoBHbcdoSF2z/D6rZnGZ6t2TpbMio3OZc+zrYbj2OTIfb6z3PVOz5Blgw5/GiXfrJAmi"
    L"rKwk9x/skYGBMBI7O8/Yb318b3BrYVp6a9fTfl+7UlX2hnBdosXyIq1zJnZ2N9hKWbJdTt481ZD6o"
    L"NBar5ndkSUTX5QRBC8DNr6Zt8wUDSzUg7Ca4oxRWipORUWtp/cPbsQx4e+lKWRB0w8NNhfttbLtcm"
    L"vctkOmhjdKjd5AAFTTxESSndCD3mAzH+N0qBjs8rjQRFVXt8UFRloNNNYZSzcdnHOO2OwDaNVAa37"
    L"vE7txhk65x6bgspAibRhMoy2Cw5V2yRXnkUN/ciz589i922zjVfd5L1w4bxsd3Mzxvq0hHqKXcvwr"
    L"F+arlt5j4j+MnjLWTrI4ljQvCYyfZF2vKuEfxMnJ96xHbbeJix9DA5eawVMu1rdDvf2Ah/ogjT21k"
    L"PoRND2s8wRlMNKz8eOz0eDn/1ucf++W82Zw2GL3lDiM3M+31V/ECorEm6KSa1hLIBNfSMv9ct8jbd"
    L"3DU5N2GS7DSDm96D0lTDwGD+MdbGR1FJTUoeeXLzimHqWDtfosYGOk0nTTWJXqU5sbVFnkO/k3K0P"
    L"EfVKbn6PRUnP3qC8w9/HfO9HQyGBd5J452Y8gCaLpxq9/mJuoAtFGa6hoQLF0Yxuxyq5RJIa+FNa7"
    L"ddH4+fgE5MLPpClx/pbKHps87kBXJRqJjZU6oVoDUmS0gz05BNao14bJJ9AOBMk8d9CVvCovZc+Rd"
    L"7n0Z4wldxAsHmKToxDUfgouPCG9BFy8WTOvGL143L080adYWmUscQu0aiwW9WSFJgd9RoAraZEPZV"
    L"zLDxoDKF6ivSBUtVG4rCYURxdnXAp44dor7zg+x59++xcf4MvbSDUWrSJwi1w5UOV3mkltg1m5A2/"
    L"IS7Fzt47ZhaQx33fkKQDd43B0qEhnAyTTBbi9f46fbxyDSJtb6a8Qo6NJbecltmwKK48yaympr30M"
    L"zgH0aT5gkmsQQXQghoVxSnBoPV3wN46KH7/JeoAAgcMA/dd58zWv26OAi1F5Mk6NRGNz/jAFRz1Cq"
    L"2BlVP2g86rmrkgr1ZDdtYKYEiRTmN6IA4hyo9uhbqwxXVqRhuqARfxjannbOo3OBGQqBmtClsjEuC"
    L"Cbi85rHTp1nd+3lWvvUPOL92FJxHOTAuJSUlNQnWMKFoOx+FH1zsIkZcv5lFcI3AQ5gCO00/QLelm"
    L"yYOzJomLjfxf7oZLsT4rttaP6B1QExoUEyZOWuorapaSHv6/ko3MLmKB07Y1JB00rjHqaoCWLTWv3"
    L"H20E8POPBe88UcIPFFhICbBSDJeG9ZjX/Alyo1aSJJnqrQDHA0HfhpT0D0pL5VIrQorLRWgCUEweY"
    L"aayweh0ocXjT1pkVbRbFRI6OGDCouuujgIK/RwVCPXDzLr2+QXHBBUVaBTl+xa2fKs+dOcdkVn2Hn"
    L"d21hNy4DrylHivroZRRnF7DlCjoPDNYdxqoG25ru+xGm62EnC6bbxc9qllo0ZZZM4jnTtfAzaFJTE"
    L"ckU/Wtt5wJMQCYjbcyScGcasLpZlJXkKWmW4IoKX3vj60pqJ//5NT4x5L4A9+hnHrnvyStv+kd/EJ"
    L"z+VvEh2CwxrqybFRozZ+YhKLGgDDLB7S8sdUQ8SWqZ6/dY23iW8vZHUL04kh1ckwSV0nw6gzgdk7K"
    L"62R8w0HjtUV2FlLG0cwVsbQl+CJ1M0CPNc2fO0svXyLYdxOYWaseem3eyN5/nuV9+G/rkG+nPCVvr"
    L"FVqrGP4vOPpjZgfsrLDa8nHSzQtcyL2K7ls1Q4MyoVTIbKBouJbTw6faY+rVrPIx3S6uVEPI0RqTG"
    L"tJ+HreOF86D1aEafuroZ9OPxSla5V8jBQCI20KNtT8XavduXymV9DJslhKqtkU6y7sKTSmoLvxiJM"
    L"QmjVb053PWNo9x6tafQF33NM5nuCpOEstQgzfoTnvwXoM5WKiKElsqwrzHlRrlQBLfIHgwbA70FAR"
    L"jEsa+YrRZIedjp/B08jTX7Fjitu8OHPzZmuzcG+n0DMNBFb/gME0U1eyBwe2wSVvTq7g3QM0mfc1S"
    L"mhbbl9myTzVonzDNi9o29UWglZoJBVER1MQrxY0skPZSsizBV466drGVkCQ/A/cG3nNLewTSa7ErG"
    L"Jqds2r39r/1QhVO/Dll7C5jtWhrlW9iZ4sMTs/TU5M+sWrMRqkIH/T6GX7sOXPzvydc+yiuSiOPfd"
    L"3CWDVn7BnENSdwaSDV6I5Cco/xKX49zvvpfkOJquOwppZAUIK3caGS8gopDTrX6A5Yn7K+WTNKxlz"
    L"xllXOP7FAMt5BCPFsQGYS1QnLuHXRKjTTyNMYPRvDdRvLmT43OWhah+Y1Tb6gYvxXuiWzNr+vGzBN"
    L"gzHqgiabahpfNk3oLXQxVlMNq1DVQftyfHj1XPjbo7MPVRy6/7U/MAIO6EOH3lMlifpXoUb5ukZbR"
    L"ZqnEZUys8fiXFiyyGQZThxr0sGwNfcZuONJ6tJGClVpCANBSkEKQWyI+wd9swrWeUKpCIUjiCddsO"
    L"gFTxBPqBx4jVKCrzVeAWLwY3DjQIgz2UgRj65TTnH8XMET64eZ/4ZH8DKafG6lBTEB0e0pYXENpVI"
    L"B3SZiOkzj+ATWDZPsHiWInvSZLxD0BBeYdDFbBZj+rDUY2xxiaRqehI5nLmqjybo5aZoQXKAua0GM"
    L"kqB+5Oyh+wYcOPBFJX+vQgHuD4BauMz+F/HjL/hSqVD5YDKDzuwM23ZSRE9w9vYgBomTEwQ8OE3nz"
    L"E50YgkmQKlQXkfOXS3IAPyGJxQBGQkyFsI4EMbgVYXpRqAprCrChkLGgTAU/FCQUiGlRWobF0o5jx"
    L"vXuKHHFRU1FdrD+bMbbG6M6KRZpKqrdh18AOXRyqN1iBn4rLCbDpNqr2Y2U7/wimHyfGvZWk+Frlv"
    L"hm3g1VmNTg7EGbTTWGhJrmt1ImqST0pnLIv279CGgta+LZzX1fwQU99//io6SfUWnh3PggDn5mz9d"
    L"3XDTtw4F/R3aILaTKK11BHYCk/m92ZOzRNoTuWN2m3SEnruKelDQu+MUIpZs3KdrOhgszvmYW9SNG"
    L"25OGIntTkVQNarIkE2FK2uUU0gJUjUxs4qeQ9UxEpo50Bh81dC60QQR8rLH0sMH8OM5TKpIUxPLO9"
    L"MsyJuk/DNo3MQdz5ZtjcAnZZtMjhRuhT7JKQwTDoPSF7p3Y2JjZ3IEfbMBzdjGUxhDd6FDlqeE2lM"
    L"MC/FeaV/V33/ogX/yMAcOGA4d+jIqwKFDgKhf+o/uCw9+9Oi3pklnT9a1odPLVJZYssSSZ4Y8t2RZ"
    L"QidPyPJ428kt3W5Cr2tJE8PiXBe9/0n8npOYeg7rEgwJViyaeOZAdKe66Tsw6TG0UGgiHYLyiLmA7"
    L"BP3xoa4WYRcMLoxP6PRSTyhvCoq9slVdNIe9Rs+TLcrhHMLJB2F9ik2TdFKYa2gTeO2TbMW3LQ9fJ"
    L"mST/TMMfQTK58eJDlpk7TH1Lbja1qhtKCtmbr9ZuxNNzxIrRXaGrJeRqeXIUA5KoNzSpeDrUcOPTP"
    L"6O2zcHTj006/44EjDK74cMr/wC/9HfdPrvuPwaFj8Fa2VJHmibWKxRpEaTZZa8sySZwl5npB34s9p"
    L"akhs3C/U6WR4F9hceqoBVgxSCSoojLKYxOAl7hWaxRmVjnzBQIXWFqsTXKjjCeBGx2KkYfBGIp1Ch"
    L"nriRbQxaAzaGTaLddbmHufc8DD14jHSGw5jb32C+R0jdJVgF0pMYknI0NqS6ARr4jXJIxFFpJmcak"
    L"8N1VMPoWaUgtaq1ayiRCHbRJMk0eVr3R5fr5tdSNEjZHlGb6GDNoa6rGRcVBJ80JXnb5z7zPd/gQM"
    L"79Cu1/lerAAL36Bef+YfPXn71N90QJHmd0uJ1arXQ9MJdmJ6ohVxwxFpEABUmUXTrnYSBpdh+FN2N"
    L"litVU5+mCYm2zfycn4YQHXGhOOFdY20OTuKodHMMvLIKcc0BAQ6UbY56dwYqhRu5eNR8klCNBfGWY"
    L"T1irTrJujuNXH6S+RtOM7zsc9TLJ+hefYJk32nsvmOku4+RLYyRoouhh00MaWawmcJ7PxF63FqtJl"
    L"Y/9QrNwZEatAGTaEximoRPTYZedaswBmxq6c3npFmCBGE8KoP31pSD4a898f6/98/hHs2hn35Vx8i"
    L"/6rODQ0BddeX2H3j6uTPfpExnRdsQjDXa22YnkISmwWIm3a242D3WueW4xMwJyxu34o6vsXXV57Fd"
    L"BVu+6b0HMptiOoZxOWLsCjxNXqBBKg3WUcqATM1FyBaPVrqpOoBURa5h3WwA0QFlFToxkWdYxdeTg"
    L"kETShBjOH/ScSo8hbUGz2GyxJDOG0wn9vfnbI9tV9/G+NHbGOgz6KJLv7wJ5zXehwl+MEtLuwAyp3"
    L"XrGtPi+mrmCOKZI+iM0XR7sd2LCK52QdDa1ePzkmf/kD9m/PvL4AFarsAB89gj/3Ltxlveda4ow3d"
    L"oJJjUKGWUagkSU9i0Qb4mXcLoCn0IJKnFVl2qzjl8OoyhoGHUtC4yMSlGm6aWaEbKmnxAdA1GkZDH"
    L"o1wlNJzA6D5pdv7oNBInGhS1Of+3GR8zceYBDUk3jdVFIfGEEx+Pb61qKEpHcIphXXKeI9S7n2Gw4"
    L"0nSuXXy07cSMLFNa/RFlt+6dWJmn2iS1EwSvon1t0uvjEYbhTGaTi+n083AKLwLMh6XImK0K+v/43"
    L"P/9X/7APfco3novvAVVgCAQ8ABc/i5f/XIvsvvvllIb1P4oLNEo1TTJg2TOnNyDq+0m2ub3RNKkas"
    L"5pDaMdx5BpaDKmYSuYf9aZUmSFGUUoam5W8ApqBKMIbUdgneTuQBpJnOVitamMjUZbQeFsgaVaoyx"
    L"MWQ0hXGcTWkXYjdCSTQmU9jEYMSAN1Te41NH163QO3tnXDunZOrqG8G3U8wmMSSJwTbxXjV1vjKRK"
    L"zG53yh+K3xlYg4zGhYhkJhyOPjdd/3nv/n3H7oPxUP3Ctz3qqWovwTvIXCzhCDqbftv+NuuHD5bV8"
    L"GEygWdGFQT10RN++e+IVAQorsWAq6s8RUsrF9H9/w+VMdjOhZtTbwaE78UE08l76Y95rJ5cpvFU0l"
    L"FQ7A4RjhdkKU5WgwheLAamyXoLE7Jh8rHpo9V6FSjsihtH3xcNGH0lCSWBXTXYLoGk1m0Nlhlscag"
    L"rInJYZJiU0Va7ECRgo5dOtUK0kbFs4khyyxZbrG5QacGnZgY+xMdQ0HzmLYGk2jyXodOL0Mnscwoq"
    L"zoEZU3tymP9XTv/1n2q7Top+SqEgNlQcI9++OH/fXDTre96bDxyfwWUNkahE6PajZztoMhsB22mp0"
    L"KQQJpkmOEcVf8crjuISZ9M4c92958iWmGaZPGYeqWa9WtCoIg4edLBJjYqgRFUFoWOVqg0KiaNN5r"
    L"ulohKlvYyVKIR14SQhtWjCBgdERslBl8rvA309ApLh96JoocL7gKrt4khyWx096mJXsjoJrNvLF6b"
    L"Zv9xm/Ur8m5Gp5dikljflmUtZeWpSxdMov8/H/xXBz75pbr+10gBpvnAC8/8+AtXXfcN65WTd4v4k"
    L"KZWmcSoMLM6lZk1LsxsGIh4v5CrBcw4p1o5hXQcKqhJ9q8a1o5u8HGNwmhDYmN/32iLSgxelQTtME"
    L"mKkSSikKqJw825htQ6Cr+FXqVxxQ38G7tu0S1jIyxrGkpbXPKs0SajOzfP8lPfRL51DU5XhCCYxGB"
    L"STZpaksxgjY6s3cYzRI/Wxvzp423szzspeTdrCDeaqvIUtQuiElOX5T976Gf/p5/lwAHDT/+08Bpc"
    L"DK/JJeYDx174iU/uu+rubUj6ZiF4m1ptEjM5em16aq7iwmOY46EMRkOqOrjOBm7hfBSAiZwA5RpEU"
    L"cvUO6hoSTY1JFkavYLOQDxelZAqbJJjQhL/pmo3gcZcQBuNDlEQKm2OfmmoOVprdKIw1kyILmI0Js"
    L"lI8z5zS/PMPfcmOsdeh84Dde1JcxsF37h304YB01q8mlp++3OL8VtN3snIGrevlMZ5T1HWXunM+Lr"
    L"8lf/9J/7y//f+ex803NKDQ4fkS60AXkMFAHgCQP/YD930hx//5JnXIclNorzTidU2tdP9+M3hORe2"
    L"isNkXi+1KcEZwtwW9AtUX1AdopCk7as3oImxDdk0WpVRkWCSJB3SJMMm8Rw2kxqsjh5BBRMxASRWB"
    L"T4moqE9ti3XsQnVEFyNVihvMElGnvfpdBbomUW6T99J/9iddOYSvAvR1Sdt3jIVfJvDzApd2+ZnG8"
    L"EBm2o6nYysk6Js/Ls+BMZl7UVnpi5GHx32X/zuH3v7W0uuwPLxj/+Rza6v9qJ4TS8xUP/dv/vzix9"
    L"75Pj7bN59q+0ol3ZSqxRUoxrXTBbpmS2f0rCHlVJ0eik204zzM4wve4pi9zFcZzXu9x9pwgBkoJu6"
    L"Xjczgy1BQxDrY3Op2WkUnIsrUxJQzqKcRSSylknAlTVBCWKasrNr4iKr2mJUirWaJE1JOhkqMVhnW"
    L"Xrkm8hHV6O7jrKomQD/qtlZ1KyracfV2vu0i7bU9OtPrCbLLNboZs+hwtWO0bjyQawZb2w+NRg+9Z"
    L"5P/sw/fpaFhZSNjZqZBTUXXcNXWQEgnjZ2X/jLf/nHdz5zbOt3km5vf5Irl3QSq7WmLirqoo6btlE"
    L"NEbJxkw1onuWWtJPgsyHjhROMr3mCcukYPgSCM7CRIiMbR8ibTpo4wUsdY74OiPVIEhoEUCGmnWXQ"
    L"KGmydGNjnpGZSD1zCp0aTDdBeY22doLWKRXzhk6xg6XH3gU6pyqr6JmMbhZYNo0c1TAg2qFVNcPoU"
    L"ROCD0liyVM7URghrssfFlH4xcbGkY2zB//nT/27/+vRTmc5G49XK6a7SSdnX1+47OaCA0u/kiHgws"
    L"rg8ce/f/Dm/d/+31Y3hneB2Yd4p63RNrUYq6dHpTRdMpnw5og783zAkpHVi6hRBz1fo3olWmmSrEu"
    L"a9dCqg1IJuh1CET3TXxdU2lihaLS2mCxBZSY+b0GaU7iwcR+hSkClCpsokiwCNVpplLYoScEkdAZ7"
    L"6axfhxePKNU0cZqQ1Lr/SRmomzCg46GXJjakbGLpdFLyThKxiGb7inOBUVUHbGbKwdbR84cf/Duf+"
    L"Y8//GSazs2X5Ua7ekFddHspYvYXbdxfBg9woSf49m//8Z3Hzm/8WtLtvc3m4rIstSaLuwLLssQV7b"
    L"BpzOyVbieMYiKW9xNMqnHJFuHyI9Q3vogYhQk9giiqrZrxsIrC9DUiJY4SLy527nwDDSviefPtQgl"
    L"0tH6rMZ2oHMbYSMAI7XcbyzSlLFoStIWFI3fSO3M7hZQRTGoSxpawocy029Pi+m21oVAkiSFNbcP2"
    L"UZOJtLJ2FJXzSqdmtH7++eOfe98/euzXfu7JpNvN6tGonLH6urnvZrzB7M9yUXj4SnuACz3B009//"
    L"+DPveu7fv3o8bUbRfTNiPdKiVKJUUmaRIhX2k3ksx+58QZ15Bwm9FDrS+ilCjvnsapD0u2RdTKSJM"
    L"HmHdIkxejYSdSZRktcKq2NwRqLTiw2syTdlKybkfYy0l5KmufYxEZBikFhUMpgtEUbi5EUrQ1pvUD"
    L"/yB0YneHFN+Vd3G6iJslf09ixpsEDYt2fpJa8m5BnSczym9JPUJTOSelFlM7MaOPs55974Of/yZPv"
    L"++UXrc17rhzPWv5UMy8Ywb3AA8jXiAe40BM88MAD9u/8ww/+aJLnfzvvJphMhzRLtU4M+EBd1FRlP"
    L"TlAwdA2SeL/OckNnfmE+uoXCdcdjUfIJTk6s0jpCME0R8DXBClxakwto0hUqZv1EEagE61eWROHK4"
    L"OKZxJOxttsc9VoZVEmwUiKsZbsyduZG1yLUxXeE1230ZNWb/QETWenecyYBhNoWD2i1WQw1QWhqGp"
    L"xAVCJ2jj54gOP/uq/+NHTT3x2FZOl+LKeifUXX9uD0etL3PeXSBK/0h7gQk/wC7/wP8vZ4x/+3Z27"
    L"3rxWlP7tSpsE8R7Q2kZ83BgTdxCHtqnDZAcwAsYmSKfCLZyhqseEomr2+kQShZcIDSvfNnr8dD+gF"
    L"kgUKomuNx4BP51b1MZgVIK2FqMt1mYkJic1HZKkQ/L0TfTXr4E0HjihE9MoUqzhlW4svnkswr/R4p"
    L"PURkVpvYRWVEEovA+iU+1qL+vHD/3Kwz/5d35q/cXnx43w/YyRysxsu3oJy+cSCeBXMwRcOGsOBzQ"
    L"Ms1PH3/expeXrPuN98iZIVqTdrKCVirh5grUGrSJEHAHESMK0xqJTT7V8nKoexMOolIqrUdBxe5aO"
    L"LtkojbHxVC2VBOiAzhshSVMJNDHeaBuTxNSgpUNSL5CGeRI3j60WMC9eSff8legOVJWPQjbTJRhMh"
    L"G+wqSXNLJ08JUmTJkFUKGvBxAmCwnmpRMB2dDUanDv16B/+2Ed++O/9ej0aG2OMEl/PrrMOLyNMdY"
    L"lS8OKS8GUBo6+QAqDgkIINDQtz508/fKRriw96Wdwhwd4sqHhUhyilmszapgabJDEpI45UaaVRec1"
    L"4+UXqetSMH2iMSeIJWybBWIu1tsHVA2Jd9ARK0FlbiqnIDEoNJkkwNsHqnGRtF73nb6R36jrytT3Y"
    L"s7tJTu4mG29Hp7EdPAvgtCWgTqKbz3NLnqcxyZvxDjRMpSoECh9C0ImGRG2dfvETj9//Yz/8+P3/7"
    L"lFjTEe0DuL9RTN0Fwjy4jJPLlKUcImy8GvBA8wmMKWGztzGxmF//vSnPtbJF047l98gkszH0awIyE"
    L"5g3sREzoA1pEmCdAoGnaepqwJpjngBiRm8TrBJSpLEmG21IpgqYgCEyaljysZ6P7r7hMTmpKcup/f"
    L"UbeTFTqhTwjgDlwAGryLUGxs9UfDGGmwahzPzPCHLEmxqJxg/jeBRilqg8CHUgtJZV5XDwdqJzz3w"
    L"ix/9l9/zH88/+/S6SdOOd85N8PJLx255CVc/a/GXun7NKEDbfrbgNGQdUP311SdfqIZHP6v1XO59c"
    L"hVYI+IlDsVI9AgNVp5lHXS/YJy8QCjj0KgPdXMAg585IzDu30V5RNUEXCwzm4RPG0tqUzKb00m6pM"
    L"U20uevJCmWqKqKyjlC8883Zw2ZJNb7bRmXdxLSNCVt1uYo03iFpnUtSuFk6u512tN1Ucj64S985NH"
    L"/8v/81OO/+rOf8k5SY1LtXeUuYfWXEvJLuXp3idIwfC16gPYaGRUEA7ZXVRtu/fxjj3q39YwK2ZL3"
    L"2S7QSuJ0U2jzwCgExcgcIVQVRgwGjRIVj7UNHl+XhKrAVxVVNaIqC+q6Rqq43UM5jZGE1ObkeoGs2"
    L"IY+tguzth1XgvcBo2NSaJOELLMkDcE1yxKSJqnTdspTaE6xjNC0igsdo8ULKu2q4FGbJ1889NwHfu"
    L"nfPfyvf+C/bRx5fqCTJJcQgsgfcflcwtXPuvVZ63YXVQWzt+GLqQIUX7mLnnoAUqAD9IE5MIvAAvh"
    L"Mw9zyjjfeObd86zd05nfe1JmbI801SWpD3rOq08/U6c5DjPxhtLKRlp3oCeVLmZgTKBtLMdFxyDQ2"
    L"ZyKRI81zOnqFdH0vdrCELnqEcUpd+RmWjpk2cxqkrkX4aBs8OiZ+ouNAuCeIU0qC0tqkHVxdMzxz/"
    L"Jnjn/mDP/j8r/7EZwiM0TrV4EII9UVCvJRgwwzuf6ky0M2UgNVMKXixF/iaUYDW6yRA3lx7M4qwAD"
    L"IHIdWaueXlO27pL9/6Z/L5XTd1+ks2m1MsLvdkkB6W1fxDSjmtjMljCWZ1s37+IhAmMZGFkyUkaUa"
    L"a9uiwQrZ2OelgOwSDb04LF6SZy9ITAdO+90QBzMTqRam4BlgpCVopTKq0zRhvrIfh2WNPnnzkgY98"
    L"4Xd+/rHgwggwOklCqOtZwcmMwPwlhB0uYeGXEr6bUYCLhR++ykDQJcOAabxA2ihBd0YR5kHPQegCG"
    L"dCbW7jiyvltr7uz09t3y9LOXUv5Yp9zyceo02dQ1gWVKJWojtI2iTW9jjsGJBFsljOfXE2HHVjdxe"
    L"oOie9jXA9fK2rnI8+gqc/RzeaTBsOflHgqgjiiVVwnqLWI0RqToHSKdxXjtdX1zWNPPXH8Mx/4zJF"
    L"Pf/AFYAxYba0E51rrDDMCu9jC65exdn9RnL8YBHIX/Y58MZXAV1IBLg4FpgkHSSPoTqMIM8qg5+Jt"
    L"SIA8SZLti0u3X7e486YbdLLrGtORHgtDQu8MPjsP2ZYo40SnWnWynXT1HtVVe+jr3aS6eRuJ7N9As"
    L"8277dI1eD6z+L0xBIKIUmCMiFJKtFEqTYGEuhhRbm4Mh2cPP3f+2c8/dezg7z813ji/Bni01lpr1w"
    L"j+UhZcXcLK65cRtrtISWY9wMWC/6LLwK+GAlzsCexMXtDmBhcpg+6B6UOdN6/pZUlnuT9/81Wduau"
    L"u6PR27jX93rakr610C5LlQNfsoG9WsEmCMh7vg0jQEVea9Oz1hQKPG8ujkHWs3bEWnTS7C3xFXYxd"
    L"sbm6Olo7emT9hUNHzz3zyRdGm+fXGkEoba0QqENwF7tkfwmBuRmvUF8im68vsvZwife8uCXsX6J0/"
    L"JpRgItzAv0SitDmCBcqhNZdAl0IrTLkxrCUJMvzc/PX703z3Tv7i9vnu/PbV7w2PZ2nxmaZNiZFaR"
    L"vje0vYMHpC/5LJsGacEg7ifD2qgq/G43p87sx4c3VjvHrs7ObJx46NVs9uefFbjRCU1taj8SGEmhC"
    L"qGSFeqn9fvYRCuJdQAPcSHiS8TPdPXok1fjUv6qIuV6sIsznCbMKYzShFF607YLqEOm2eS5przygz"
    L"h07zNO13Ov0dC0m2PK9Np4O2CUbrdtm0Vrpd/CFI8OKKsixWN4ut42tVOSx8VdTeVwOgmLEur23qC"
    L"KEOBE8I5UUCvVTr9uLH6pdI6i4OAeES5d3FJSGvRvhfCwrARY0NfZEyzOYJs94hm1GGLD5mcrTkGp"
    L"OF4BMI6czvmZmrvqi9+nL1d7PVUXutbSA0p/+GMCvoMGPp/mUE+VKhIFxCGcIl3PvFgr/4c6pXKvy"
    L"vFQV4OcBIX0IZzIxgkxkvMesxkuljJkFLojGzv98ogFwKCGsXvvrIFA3yEvF21kLdJR7zl8jM3UWJ"
    L"n7vo/cJFrv1SdC95GWiYP+kKcCmvMJswqosAJT0j1FYhzEU5hb3otXoGAb2YWvVSWHu4RMy9VPI1K"
    L"9xwCTAnvIRVhz9G4PJaCfxPigJc6vNd3Au/mBmjL3Lx5qLXzFj+RJkuvsrLhAW5RIkVXub2YmH6l3"
    L"iPl+v4verY/t+LAnBRbFN/jJe4OHy8HIFSX0Lw6iU8AS/jgi+OyeGLaOWGL4LAIV9pC/uTdFEvQXS"
    L"4FFNGXaL05CWeV5fOCV7yvvpjhPfFCPhVJ3B/evnTy59e/vTyp5c/vXwJl/8/YzklMybh7aEAAAAA"
    L"SUVORK5CYII=";

// Disabled/offline variant of the globe. Generated from the same embedded
// globe artwork with luminance conversion, keeping alpha intact, so the no-
// Internet state matches the gray Windows 7 visual rather than hiding it.
static const WCHAR* GLOBE_ICON_OFFLINE_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAAIAAAACACAYAAADDPmHLAAA2SklEQVR4nO19eZAdx3nfr7tn5s07dhcLYIFdAGtSokBQNCmR"
    L"okgx0cFURNtJUUmUOD6UxCnbKUWhJavKZVfssuM45VTicjmOD9mibDmSpZQsS3Zs0hZ12CorOmylBN4gKYCgAAJLYrHA3vv2"
    L"HTPT3fnjzTf7vd6e997iIn18VVNv3kzPTE9/99E94oMf/CBeiXD//feLy73HAw88YOk+DzzwgL38Xv3NA/lyd+BqAici2r//"
    L"/vsF33+5+vZKAfFySwCOBM6x1xL+NkuH4OXuAIcrifxjx46JbreLG2+8EYuLi2JtbU3s2rULAJCmqQCARqOBZrOJt7/97QjD"
    L"0K6uriIMQ2uttQsLC5ifn7cA0Gw2/8YSyMtCADtEtABg899SmJubE0EQiDiORRzH9rWvfa35nd/5HXP06NHL6isA3HfffeKl"
    L"l15SnU7H3nDDDebLX/7y3xiieEVJgBx8iJbwEMG5c+dkGIZiamrK7NmzR3/sYx8rkHLkyJHK6173uloYhvsAzAohppRS40qp"
    L"hpSyKoSIhBCBMQbGmNRa29Vab2qt1wAsdzqds3Ecz9Xr9fbDDz/cApABwPHjxwEAN910kwKAG264wTz88MN/bYnhqtsAl8Dt"
    L"7n4f4s+dOyer1SomJibMb/3Wb2kAOHTokJyZmZkCcGsYhjdGUXS7lPLGIAgOBEEwo5SqB0EApVSxCSEghICUPTvYGAOtNbTW"
    L"yLIMaZqmS0tLKwBeAnDSWvsEgOeiKDq6a9eu8w899FBCfTp8+LBUSom/jsTwSpEAgv26AygAiIsXL8qJiQnz0EMPpQDwpje9"
    L"qX7XXXe9PoqivxeG4T8Iw/CWKIqur1QqiKIIUkoI0Ud71lpr8/tbABBCIMsyZFkGpZSw1kIpJaIokkmShLn02Afgdmvt92qt"
    L"0e12V8+fP3/87rvv/pqU8qu1Wu2vvvjFLy4CPelw+PBhuX//fnzta18zV2+4rhy83BLAx/HF/6WlJWGMEfV6Pf3Yxz5m3/Oe"
    L"98QnT558gzHmPinl26Mour1arUaE8ByMtdYAENZaYYwR1loIIQRrAwCQUmJzcxOdTgcA0O120el0EAQBAFilFABYYwwRpeT9"
    L"1FrDGPMigC9JKT8zPj7+xS984QvLAHDPPfcIrbV44okn7CvZXriqBFCC/EFIBwBkWSbX19dlHMfJxz/+cfu2t73t+iRJ/pHW"
    L"+nuyLHtzo9GoCCEQhiHGxsaMEMJKKYWUUqCHeAghYEyPCQnx1lpYa/ukA/1aa6G1RqfTwerqKgAgDEOkaep2jySIMcYo0QMY"
    L"Y5Bl2SljzOeMMZ947LHHvk4XvOUtb5GvVEK4KgQwgOu3cTn/XVlZEdbaAvFvfetbb9Va/xul1PcIIV61trYGay127dqllVIC"
    L"gNi9e7fgEiA36gr93mw20W63oZRCvV5HFEXoaYIecKIAesSSZRkuXLiAbreLMAxhrS2Iia5hYIUQxhgja7WayFVKprX+YpIk"
    L"H6nVap/56le/2gaAW265RT799NOvKNVwRQlgBIPP5X4BwC4tLUkhhNy1a1f24Q9/WL/lLW95ndb63UKI75dS7q1UKtBam/X1"
    L"dRvHsazX6yKOY2RZhjAMEQQBut0ugiBAFEVYXV1FmqbFcaUUtNaIoghCCDQaDVSrVReREEL0EcPi4iKazSaCIIDWGsAW8vmv"
    L"lBJpmqLRaJjJyUmbpqnKsgztdhvtdvvrSZL85uzs7B8++OCD3XvuuUe0Wi1x9OjRVwQhXEsCEL7f9fV1Wa1W8ZGPfCS55557"
    L"XpMkybullD8cx/HeJEmQpqmu1WrSWiu63S5qtVrBjWNjYxgbG0O320VuwCHLMrRaLRhj0O12YYwpCIAgyzLMzMwgjuNtREBA"
    L"auLcuXPodDoIwxBZlnkJgNp3Oh3EcYypqSlrrTXGGAlAbGxsYHNz8yta61985JFHPgsAs7OzamVlxbzcauFa5AIE3+bn5wUA"
    L"LC0tifX1dTk+Pp5duHChcvfdd9+fZdnnK5XKf4zjeK/ugZVSqizLRBzHqFQqaLVaUEphfHy8EPGbm5sIggDGGKyvr0MIgXq9"
    L"jkOHDmF2dhaTk5OYnJxErVaDlBJBEKDT6RTuoJSyQDjtAz2JsG/fPiilYIzpa+faEMYYxHGMJEkwPz8vVA+EtdaMjY2Zqamp"
    L"t1Wr1c/ceeedH7/zzjtvnpub081m0x4+fPhlzcdcsYcPMfiK3ziOsb6+roQQ8pOf/GRy7NixuxcXF/8giqIPRlF0AwCTJIlN"
    L"kkQJIQQX8VprstDR6XTQ6XSQJElh8KVpCqUUkiTBxsYGWq0WrLWIogi7du3C9PQ0KpUKhBBIkgSrq6vbkM/jA1JKKKWKZ/Jj"
    L"REjUXghRqBlrLc6ePUtEI7XW0lqrd+/ejampqR8IguBLd9xxx/vvv//+6OTJk2Z2dlZdKTzsFK4IAQwx+vrOCSHU+Ph4Nj4+"
    L"Ht51110/ZYx5KI7j7zLGmCRJTLfblWmaChLZSqnCUAvDsNC3JI7Jem+327DWYmJiAuPj46jVauh2uwWBpGla3CMIArTbbWxu"
    L"blKfCuRyJHe7XZw5c6awJ4CeyKdn5/GDPgNSa130c3FxEUopVKtVCCHU8vKyWFhY0GNjY/t27979a9/4xjf+6K677nr93Nyc"
    L"BiAajcY1T4RdNgGMaPELAKLZbKpPfepT3fn5+cPPPPPM/5ZS/kIu7k2WZcQphRVPg07RO9L9cRyj0WigUqkUOl4IUXA/XS+E"
    L"KOyAKIoKY05KiWq1in379m0T63QfIQQWFxchhEClUoG1tiCEWq2GiYmJ4vmu5CBJlWUZhBCYmJhAvV5Ho9GAUkotLS3ZKIrM"
    L"vn377hNCfOGOO+74ASmlbTab9pZbbrmmKuFqPIzrfPoPrbVqNBrpvffe+10XL178E2vtO6WUBoDVWksABXdGUYQ4jvvCtkEQ"
    L"oFqtIo5jdLvdwtInkUsin3N8EAQIggBJkkBKibW1tUJN+EQ+/bZaLZw9exZJkiD3QAqRTwQlpSwMwEH3evHFF3Hu3DnEcYxd"
    L"u3bh4MGDaDQa4uzZs1IIoaempvZXq9WP33bbbb/5rne9q/L000+bRqNxzYjgaj2o4P6VlRWhtZZKKTz33HPvabVan7bW3ghA"
    L"A5BaawH0kE8GFRerQghUq1VIKbGxsYG1tTUA6BPjFL0jD6DVaqFWqyGOY0xMTCCOY5w/fx55urfwCprNJihoBGyJ8IsXLyLL"
    L"soLDyT0MgqAw9FZWVgpxT/3kG7UPwxALCwuYm5vD8vIyzp8/j3q9jomJCbz00kvKGGMmJibM5OTkj5w4ceLT99xzz6s3Nzev"
    L"GRFc1kM84r/v/8rKihRCCKWUfPbZZ3+22+3+Rpqm49ZaLaVUFLQJw7CP+4mDlFKoVCrodDpYW1tDpVLBxMQEqtUqwjAskKyU"
    L"QhiGqFarqFarhY2gtcb58+exvLwMY0xBSNba4r6kzwlxpFJIsnCOJo6vVCqoVCpFkMhFPvWdEkz1eh3tdhvLy8vQWmNhYaGw"
    L"bS5cuCCttTIMQ713795/urGx8fAtt9zyumazad7whjdcdePwkglgAPIpqieFEKLRaERPP/30LwH4mSzLhBDCKKUUD7oQF7mZ"
    L"OkJSq9VCo9FAFEUA0Cd6OafV63Xs2rULSil0Oh1sbGwUXMsRSggi1UFcDmyFjen+fHNDylx6+CRApVLB+Pg40jSFEKIIRJEh"
    L"S7GKPKeglFLZ1NTUTVLKT+/du/fwY489pintfLXgkghgWMRvaWlJTk5OAkD1m9/85q8qpd7X7XZNPljSl44Fer40IRToBWzI"
    L"vyY/n0sHMhjpGCV1KpVKIU0A9HEy9/MrlQq63S4WFxexsLCAtbU1tNvtbYYdsB3Brt7nbegZSZKgXq/j4MGD0Fqj1WoVKee1"
    L"tTUYY5CPE71vIKXUhw4dOnLw4MEvTE1Nvfb48eNXlQiuRDp4m7U/MTGBTqdTfeGFF34liqIfarfb2hijiNtJ1FLcnVw+1yKn"
    L"yFqapoW+5XF+rTW5WH2IrVaraLVaqNfrCIKgUB/0HOJQcgtJFbXb7d6gBAGklIXXwdsTsgeB1rpwT7/1rW9hcnIS09PTWF1d"
    L"xfLyciHpxsfHUa1WkWUZVzMKgD506NCrjDGf11p/1/Hjx4/fdNNN6vjx43row3cIV8LQoFBmEeETQlSeffbZ/xEEwQ91u13d"
    L"6XQKfU86mAw9lklDkiSF705ZuCzLUK1WizZU2MGNROJ2CvJYa1GtVgvxT0jmkocTDakQijbye7suYplEAHqIp76Tt0DPXltb"
    L"QxzH2LdvH6rVahG+puvJBsrfTSmlstnZ2W87cODAZ8fGxl59/Phxfeedd15xw3BHEmBYendhYUHu27cvfOyxx/5LEATvzrLM"
    L"dDodCWwlTWif+/a5DiwQTIEYQjS5YzzTx6UGIRYAkiSB1hqVSqXoYKVSQbvdLmwN6oMvD8C5ndqVcT0PRJEkc+9rrcXevXuR"
    L"ZRkWFhYwOzuLRqMBIQQ2NjaKkDY9mxgkTdNAKaWvv/76Vxlj/nhubu7eWq22ePjwYXny5Mkrlki6UhQlnnvuObl//377+OOP"
    L"v08I8WPGGLO5uSm01kUhBo+pR1HUx8la6z5OIELgXBpFUR/iyRvg91FKFTYDIbxWqxVBItf+cP13wM/d7nGKBPKopO86KSXO"
    L"nTuHKIooTwApJcbHx4vzZM8QI9DxJEmUtVbPzs6+bmpq6mOPPvooarUarmTE8IqUhJ07d05+6UtfSldXV79fCPGfpZQiN3gE"
    L"t/D5AHORTFzNa/ZoYAAUUTxKBFEMnt/T9wxgi0snJyexvr5eSB8yOAm4XTJIMpCk4tzOn+Xej2IOL7zwQhHLIBVB4Wlqz4mG"
    L"9rMsUwCy6enpf5xl2QeefPLJ983OzgbNZlNje/ncjuFyCUA8/vjj8hvf+Eb6xje+8a3dbvdXKpVKvdPpGGutJLeH/HR6WXKB"
    L"uLglxBc3ZkEhTiTEqSRBqK3LwdxVo2trtVpf/J/uS0CGH13PuZ8jnxuHPD7A9+m/e78wDAvXr9Fo9BmbpCYprU25hjAMlZQy"
    L"63Q6702S5Mm5ubkPXymj8LIIYG5uTtx+++0mDMMDy8vLvxzH8TRHPoAiyAOg4DACd7B4ho3798RttVqtOO5yvyt6eVkYD+DQ"
    L"wNLAcwTQfQgRdB2vGKY2/Bn8fdz/9EsExFUFqTXKZFIbkhzGGMzPz2N6elpMTEzIOI61lPJ/XnfddUePHz/+RKPRkM1m87Ls"
    L"gZFsgDLjLwgC8aEPfcjMz8//bBAEdyZJoo0xkvQyWeY0mIRIHiunweeinaJ6ZMETp0spCx/f5TzOldSWS4Ysy7C5uVkkaLjE"
    L"8BESsFUn6CKuDIa5iHSeELyxsYHnnnsOWZYVEo33N45jzM7OUjRTSimFUqphjPno3r17K0mSXLY9MLIR6BLB448/Lh966KHs"
    L"uuuu+z6l1L+jjF4YhkU4lxDFjTpgizNInPLATqfTgbUWrVarcAkJ8TRArpVNyLHWotvtot1uF/UC3W63OEbt3RQu4Df8uPQY"
    L"xf93wXcN0+3FO585c6aPwKIowpkzZ/D0009DCIEDBw5gc3MTY2NjcmJiItNa3yal/O9Jkpjp6enLMuQv6eKnn35a3n777fq6"
    L"6677NiHELyilQmMMgiAQPHvn5tddI9CNwRN3WmtRr9eLtrTRPbiRxAeW7smJD0ARt6cYPnG1a0NwzneNRJ9hOMo5H9CzKG2c"
    L"JAlOnTpVFK9cvHixSHo99dRTWFtbw/T0NJ5//nmkaaqklNoY8/7p6ek3P//88zqKoksmgsuyAbIs+6/VavVVAHQQBIojnSx3"
    L"AIV4DoKgzzhzS7K4EUi6ENjus/tcM24bkNQgO4TSuVJKxHEMIXp1ApyQuL521QmXMO5x+l9mCJYBEbTWGlTgShlI/l5SyiI1"
    L"DQCdTkcIIYTslcr9xoEDB+5aXFzU8E+qGQo7JoDPf/7z6vTp09np06fvU0r9ayml0VpLMva4biWgASTDhsQfEQwfMMrjc9Uh"
    L"hCiifHQ/Vyf7CIsjRUpZRN+01n3ExvtJ7S+V43cqDaamplCv1/tspT179mB+fh4bGxsFYc/PzxcM1MO/1MaY29I0fW+SJL8a"
    L"BIHKsmzHXsFIooPmzz/yyCNi79695siRI3VjzM9HUaRyHS5ccWqMKaJy7qBwn5+LamobBEHBCW683rWoqQ3tU9KIn+eRRSIs"
    L"3l8CXz/LYBRED2pDREZBKyJSpRTSNC2kmLW24H7KieRELoQQVmv9M9PT09NZlplLUQVDJQA3/s6fPy/n5uaymZmZH6xUKm8Q"
    L"QhgAkiORG1Ck58jad0Os3O0hy5fulSRJgXxqx+vySAJwCUKDQ6lfXv1DwLOIrps4qvi+EkD9P3fuHK6//nqkadqXJ+FzEUhS"
    L"8qQRABmGoQawV2v9EwB+Ii9D3xEMvYC4/9ixY+Id73iHPnz48B4AP66UsryzNHg+S51eGNiyrMmvduPoPERMPjK/hiOPF4xy"
    L"q54Gkh/39cW1Lcr6fCWBPxtAUZlkjCm8lmq1ipmZmaKWkd7zxhtvRK1WKwijUqnIWq1mlVL//sCBA9ddihQYuXGz2ZQPPPCA"
    L"3djY+LdhGL5KSmkBSPeFePTLZ1kTMumXxBrl/QnpFC2jqCEfQI5Y91lZlvUVgg4DnxHog1H0/ijXcklJ70I5CipobbVamJiY"
    L"KApIhBB4zWteg/3792P//v3cKBWNRkOPj4+PJUnyXgB2z549O/JXRyKARx55RDzxxBPZrbfeWgfwH4gieYCHqJKLejc7xg01"
    L"aptlGQAU6VgKG5Nu5PED0pGkCqjShrt/dL2rLlxk8L7shPPL2o7qMrrXBkGA5eXlouqJJFi73S4Y47rrrsPU1FRRYEKZUQDo"
    L"dDpy7969ttFo/GC9Xp+an5/XOwkODSWA+++/XywuLkoAWFxc/GdKqRtJ93NE8o2O0eCSiKdfnv7l/j0/x90xzuWdTgdpmhb1"
    L"Aj5x7kNsGeJdBLrXlLXxEQ4H3znfNfSuVCzSbrexsbGBF198saiF2L17d9E+TVMcOnQI+/fvx8TEBJRSUghhdu/ePRXH8b8E"
    L"gMnJyZEl+1Aj8IEHHrB33nmnuffee+Wf/umf/nAQBNZaa6MoKqZPu/40xc7JzyVjkLt4PARMRiLFxPlETx78abVahRXPvY2y"
    L"DJ6LCNeD8BmkLqKGifeyZw1rx20ApRSazSZWVlYghCjUYqVSwYEDB/rc62q1iiiKUK/XCxVKY1Kv13+42+3+dj7RZCQYSgBR"
    L"FMmjR4+a+fn524UQbwMgpJSSwq3krpFbRQPphn3JleFuIbfQeUKEL+cC9PQlhXJ5WRgPApVZ8ByBPtfRJymGcb57f19bt30Z"
    L"IRFz8MRUHMfYu3cvGo1GUTDC+05jyGIrSillJyYm7lhfX38zgK/Ecaw6nc5QQhgqKl796lcLAMiy7HuDIAgBZLa38kbxQq5h"
    L"Q0Biv9vtFkUTJBFczueEw9OhaZqi2WwCQFH774LPuHRtjUFtyjYfwgbZDm4bd78MuM1ELjBJWP6OPBvJjcj8HnpsbEzU6/Xv"
    L"BoDZ2dmRxNEwAhDHjx/XN998cxXAP89FriQfneLp/AX5IHO97wuXUozAfRmqrSO/l+ba8Tn6ZWKbjtHGXccy5PN+u0gZhHAf"
    L"kkdRRe796T/PXbjVUjSGpAq4BMw3GccxxsbG3tloNOp52dhQY3AgAczOzkoAWF5evlsIcWMeeZLUKQpfkgijX75PL8b1PiGC"
    L"h41pMCgKRpNCSC1wj8LdhnG2KwHctj4iKuPiYSJ/GNJ9/zljAMD09HQRzOLv7rrWdE0+jlIpZev1+rdVq9W3AcBNN900VMKP"
    L"ZC1aa79T9ea6a0IcdY5EMg++uIPOq2gA9CGTB3kodEyGTrPZLCKCnKDKkOg+15UC/HwZIn1SwN13kVjG3b7zg+5DEpUSVmRk"
    L"k/rkXhbgjVzqfPWT7xgFr8AQApibm9O33XZbAOA7KPDg6lKgv6CCEM47TeILwDZqpmPkITQaDWRZho2NDQAoKoI4Ml1XsUwC"
    L"+M6VSQkXGYMkge+6snu59x20T4EsPimVS0DuRnN1SIQjhFB5Mc13NhqNMC8ZG6gGSgmAQornz59/tRDiFtL/1vaHV6nzRAxp"
    L"mqLdbkMIUczypegegCKrx8UuLawQRRE2NjaKCSEUD/dxNycE/p+ro0GE4rqCg5A2imooa+9rN0ydnDlzpphfQBIX2LIFeHSU"
    L"S2Oqg6hUKkeCILg5x+OlEQCdM8a8VUpZsdYaa63gYp/7/oRUCl5UKhU0m00sLy/Tcm6o1WpFqROfWEm1A2tra1BKFbV/pDrK"
    L"EOgjBuIU95x73TCRPgqifdf47uX77/slZhgfHy/67kuAEdB5ZhQKrbWO4zio1+tvBoD9+/cPlPKlJ/NYPwC8Pud4w406HxUb"
    L"YwoOX1hYKAzF9fV1bGxsFKXQtD5PvV4vxFe73S5m5rhl166uL+Nk1wMp+z9IrLsIHOYy+hA+jGB815ALLESvepkCPIRorjq5"
    L"e8hzIvm9bF4B9UYAmJ6eHlg0WhYIEp1OR8/OzgbdbvfOHOFF6JeXbgH9YogWUSArlgoeaZ88B4oNxHFc3IMv5sBLtPk+9yh4"
    L"EKiv8yXZyFG5eBiRlJ3zPW9UgqN3oGnv9H4kbcmuoiJZngnlOJFSyjyHclej0QiPHj2aNhoNUbYamVcCNBoN2p0QQhzJO14E"
    L"f3yGHBdhdJ5EMS8MIf+eon3dbrd47iBOd4/7tkESY5CXMOgerroYtj+I0MrEPmekmZmZgoC5kc3Hk5iCGIOH140xInehr5dS"
    L"TgFAkiSldoCXAOiCJEluADDuIsdFGP1S9o6MFFc98PZk3VIcgb2AFxFlxqBrC5QZiaNurvHI35mOcSQOIgoX3HOu+KfxoNpF"
    L"t2yNh9vdYBD1TwghrLVWSlmPouh1ADAoRVwmAUjp3yqEUOgtwCxckcUHSuWrYV24cKHPwi/jFvp1B9VHML4AU5kt4BLQICng"
    L"C2ANkjyDVEIZUZRJEH6cxjJfRKovuAZszZukcjceWeXPZCunmbw8/yYACIKglAC8NgCjvENkAFpmAxAF5kRSuB+Uw65Wq8Vi"
    L"TfSyvn1y/+havnKnq/vpWa7n4UbFfFDGrXRPfvxyxH4ZYZT1yd3onbn4J+Kg/AjPB7ihdEakNh/XfaUdyMFLACsrKzbv5F7f"
    L"S/MsHnWAKJbmvg17WbL+iVD4un9EVK7hxxFeFhYdNODub9m+jyjKrtnJ5koW3h9gi8ApIUbqiM+kcgNq7vVO368HgDiOSz0B"
    L"LwFQebEQYpbG1uUioDdrlxI21AGiUHfyxiAiaLfbfcEidzBc7sj7to0YRuG4sv984OiX92cUIhlGIGVjQf3nFVDcLqB3o8QY"
    L"jTH3BOj+OSOJ3Ct4daPRECwxtG2AfAQgANgjR46Ey8vLs3mHhW/AyPWg6hxCujshhK7jIpyMPlIfpBJ4gghA3zGeEeP3HSYF"
    L"XMIo42gXOb42g5Bc9ryytiQJuAvI38klQC5tSS0A2JZxTdNU5nGEqWq1GjWbzS5KoLQgpNVqRdba3fnDBX8x6lin00G9Xi/C"
    L"k9xvdRdu4IRBx2iiCPftfaKfP9OnCviglUkBH4LKju2EMHztBqkQrgYISHVSYIyInZBO78aBXEIe+eQV0Tk0tNYBgJ0TgLU2"
    L"AFAnxLiDRb58u90ulmzftWsXoigqAkGcsuk4z+tTASeVgFHs36f3uZj3/eecNAiGSQMfQvl5t+0o0mLYJqUsCl92795dIJWA"
    L"iMW1HSi+womJ91kIEVprB4aCSwnAGCMAFDFHd+DcDB9VsjYajb7YO9WvUak2IYuCQGmaYnx8HO12G2ma9s0CIiTzmgGf+Hc9"
    L"g0FQhmDaL9PV7jXD1IFvc91TVzVeuHAB9Xq9z8CmseX2FY9V8L7y8c3vPTTdX0oAaZoKV/S7QJ3rdDoYHx8vOkiTHfkkkWaz"
    L"WcT5SUxR3f7a2hoajUaRSeSrfbmI5kgeFAoug0ESoOx31P1BROFDPj9HPj6Fe92Se5IKJJF5bSW3JchGyM8JDEkHlxJAEAQ2"
    L"SRLhDpg7eMb0Qr2dTgcTExNFGpPWvyMRz/UadZrXvm1uboI+7cpDx2WEwPd5HwepBR/X83NlUsE97/MOBhHOsIASvyclygD0"
    L"RVQ5x7sqmUtMAH1tB+EPGFIVbImMSixrHqTY3NwsvtxRr9f71gTY3NwsDERyXeieVAiilMLq6mrfbB7OBUL0TwXzSQPer2HI"
    L"9v3fqTQY5digCKWvj5zjXVHvShICHh10+mHhcf04DLIBLICM62IfWLs1tYmCF+12G/V6vVjlkyQCHxyOSO4Okoqge1GhiDG9"
    L"lTz5pEm3MMVnH7gE4fbd998d4Hw8+tqVqQMXWWVRxTJpQ6Ke2rgEQO3puDvxlruPrnHog0EEkAFoAdgzinVNHaLgEEX4yNLn"
    L"M3XdAk9gK53pDjJNhqjX61hbW8PKykqhL+m5wHbVwAe3DPk+SVEm/nmfBh3jhDBI7/s2Git6J14Q4yMU/gy3z/m5oUvJlRLA"
    L"wYMHu2fPnl0BMAvAinxUXb3qAuX/6Zt7VBRCASNrbcHFXC/yexJlCyGwubmJJElw8eLFvoAQlwQ+5LnBIVcqUPsyKeC7rw8R"
    L"rqjn+/z9XPHtuxf58TSOXBK4ffQhn5fo521SIcRAAvC5CRYAnnjiicxaO5ffcLAl4XSOkNvpdFCtVotEBn21i17WFXNuWRfp"
    L"NZo1SxxB08fIVy4bXBcBvnYckYOQ5hpvZYZd2XWDJA2wldwhA5hUIQ/s+NQKneOLZqPHsBBCrAkhtn36dBgBIAgClT/wDBHA"
    L"IDvABY7IxcVFdDqdAplhGBbTxPjg+xDICyB55IvPTi5DPA3UTs4P41QfIYxCNL77uMin9t1ut5j9y8Pgvr7SvaIoKtZCziOw"
    L"NiecE8vLy0k+W9jLxF4VMDk5KS5evAgAF4kAykKSZUDii5Y8IXVA3/nxJY3o5XxhXgLu8rh2g2sDuED3du9J5wbtD1IHPmQP"
    L"0/8uUCyg1WohTVPs2rWrmAnNn8v7b0xvHiF9JofGPI5jY4xRWZY9BQBRFAUAvJJgoBsopTyXP0z5fO5h4CQoiihh2YC6iHOP"
    L"0f8yX9+HYPd/Wf8vhQDod5CKGQX5vN/W9lTnyspKQQSc26md1rpgKgqekVRkoeQOgIELZXgJYG1tjfyHJ4wx2lqrpJSFIcih"
    L"jOtcEQ0ArVYLQL97wg0+d9D5S3PEc24YRjTDwCUaF0G+42WIvRTk03vQWJAn1e12sby8jPHxcXQ6nb42Usoi1kLfJqSgWi6p"
    L"lbUWWZadBYDl5eVSrvUSQJIkPbZX6mSWZRvW2l28CHGQFPBxLL0UfX+XDB4++ZHH/DnC+YC753wqw4VRJNaoSOf7O3XxBnG+"
    L"C0mSFGNljCk+bWOtLRa9pK+lAdsypVYIIbvdbjvLsr/I71c6TXxgsmBlZWVdCHGc2wGDuMsXk6eXJMudct68vJkvA+Ou6sUJ"
    L"g0KkrgvkcuCl+OBlCHOPDZpxdKmc7ztOjLK+vg6tNWq1WvHllIWFBWxsbBRekDGmb9xyNdCsVCqLg/ALlBOAnZ2dVVmWaWvt"
    L"1/MXMJz7holZfp5WEeH6rOxDC7wAkoiFCIZelFcgjer+jbKV3Y+Ou9XCZR6Ez03bCbjEt7q6ipWVFbTbbayurhbIpvUT+Pjl"
    L"KhvGmK90Op3usI9TlxqBnU5H5Ih8Mn8h6UqAUXQttSH3hmoFiPvDMCx0HAWNaOYLuYtCiCLbuLS01PfBCBp0d/2/y4Ey7neP"
    L"+dq453YCrlFLUoAin5RroSVk6BouJa21NlcXjzWbTbuxsVHqAQADCIAMQSHEl40xbWNMNQgCK6UUw+yAspcjF5CIgbKItVqt"
    L"WP+HkkKUEqUAEr385OQkrLXFmnpkEBER8dKxfECK5w8DF6m+Y8P+u+88yjiVMRX3oOg4TZd3o5sslhAkSaKttX8JAMvLywOX"
    L"iSklgCRJDABsbGycbTQaT1lr3wTAKKUUUR/v6KgDTNk/vlgzfeq1UqkUn4UBgHq9XkQRO51OkQwipJP9sHv3bjSbTVy4cKH4"
    L"ngAfNHr2KP3j7UYlCN817vhciiqga/nzfMTC5hJY3fsU74KU8uvAlkFfBoP1QxConBD+LOd6y+PV1Imdul0kwghJZFwRl/M1"
    L"grIsQ7PZ7PMe6FOypAqWlpYghMChQ4cghChKzLhO5rrZPebqd981g2wFjiAXuIfiIm+nY+fzdjg+jDE6H9eHL1y4kOQfnLx0"
    L"AmAP+Zwxpvj4o2up+zo46DywNWB8/iDX31RA6gZC+PVUUiaEKOYc7t+/v6hR9HkNvFytbCszJssMvJ1w904N6GHHCB95/3pB"
    L"AKX+DAA2NzeHUtdAAsiyzADAzMzMUQDPGGOEtdYQ1V2usUUvwf1YCn263wnmA08vzcuogB6xrK6uolqtYs+ePUV1DdOPBREM"
    L"it650sLXhvp2qaJ9EJCoH0QsdI69n7HWSq31+SRJPg/0VngZ9qxhEsAGQaCOHTuWCSH+KH9hO4gAdirWgO3VrrSRoedyL6kJ"
    L"njfn7tDGxgaklNi9e3chQbjBRH2/FDeS95G/M/8dBXwILkO6z/3m3J+PIbnpf7y8vNycnZ0dKv6BEVQALRQhpfy0MSbRWish"
    L"hCXKu9ToW37PbYNHsQGKE/gGCdgqnaL7EIFQAQkZk1NTU5icnMTk5CTGxsYKyULcM0gllCG+DCH060MqhzIEjwqc+5n4V8YY"
    L"a4z55Mg3wggEkCSJiaJIXrhw4ZsA/tz2YJsacN0ufsz7YCmLr2lSVJAPOl+Gho659QJ0jP7Tt4cphby+vo52u923ktn4+Hgx"
    L"B9+YrW/7lln0/NhOEHWpRt8oYp+4n+wknXOCMeYbq6urf3nfffeJUZeLHdUIpKDQbxtjRE/aCO9XNwa9IB9cCgDReoD5CxTI"
    L"J87kpU/5y/Z9Y88lCiot5yXUSZKg1Wqh3W5jfX0dnU4HBw8eLM67c+xcpF8LGEQwPonhcH9+WDyQJIl56qmnRl4seqSG+Zqz"
    L"Qin1eWvtk9wYdBcxICgzkEiH08wiXvzAy6CA/nWFeZUxgILb3ee59gQRD/deNjc30Ww2ceDAgdKpVxyuJBG4iB2EeHcRCH6c"
    L"FpDWWhsA0hjzQpIk/wcA5ubmRv6Y5OjLigeBPH/+fCKl/LVcCmAn3oDrMllri7VwXF+cZwe5F0DnAPR9E8BFOgG191n3nU4H"
    L"m5ubmJiY2EaoO/HRRxHXoxp3rh3hO0ain1UJWfQmf/zy0tJSc2ZmZiTjj2BkAshdQlGtVj9lrT2eE4HhXAmUR9N8+3weoKuH"
    L"eV2gL1hD3EzGIpciHNFkQ/A6e3pGu93u+6K4r58+GIawURA9qA1nLL7vcr/pyX5pjHkewO8CEPPz8zv6lOxOvi9jZ2Zm5Jkz"
    L"Z1pxHP8i2QVEkb7gUJnbRAikPABxM4VxgX4r3yfWkyQpgj8usbgratIAcklAhNButws7gH/3sAx2yvHDdLtLSL6aCE4M7rQx"
    L"25u+90svvfRSc2ZmRmIH3A/s8LuB8/PzptFoiDvuuOP3H3300R+VUr5BKWWCIJC81Ns3OO5L0Zc8KZhDL0TgK4nmHM2jhtwD"
    L"4dLERaa77gD/xC2VVlF/6D6ccPj7+IxEOu4jkkGEM+iXgNdO5P03AKTW+vG1tbWPR1Ekd8r9wM4/HGk7nY568MEHOzfccMNP"
    L"J0nyWSGE5V+04FE93y8NEGX66DhdyzmAG4SEdD5T2OVsXmpObUgKcHFK96eEFKWoG41GQQC82oZ7IQQ+wuDvydu4x932rhTw"
    L"AaXOASDLMpvrfgghfmZ9fb0zMzOjrgUBIMsyHUWRPHXq1BdmZ2d/31r7r7TWWinVVzhaxjFAf0CF4gC8+EOIrW8Pl301jDiU"
    L"u3Ak7omY+DeJAPTZENSOpBZNaqWlaicmJgq3kksproJ4X+j+LgxTJy7yfRKEi/6cGI0QQmmt//Ds2bOfy7l/x18NBS7j28HW"
    L"WjE2NvbT6+vr9wohpqSURkopfVzheyluANJMH/6NIWDrxbk9wLnbHTgAfWKbSwg6zomUCItLFbILWq1Wn64lY5err1artc3Y"
    L"HKQC3WM+zvchn3/p1FpLon9JSvmTwPCPQgyCS/rqdJIkJggC+cwzz5wJguAntdZCa22R1w26XgH/5cCRSghyvQoaAF43yAeO"
    L"xwno/yBvw+0Xvx83wpiRtS3KSJ+kJ8nFw8vu/dyN4hGcoFx/n8aQv3tOYNYYY0UPfu706dOnDh8+LKh241Lgkj87nmWZCYJA"
    L"vfDCC78L4NPGGKW1Ni5l02C6wMUpiXMu6tyaAzKA+GdqCYiT3aAUlxx0H1cvcylB9+LSxW3LvR13cotrsftcuLKNn+eGMn/f"
    L"3BVXWuvP3n333Q/kov+y0pGXTADocbu11op9+/b9qNb6+TwhYXzBIe7j82NuIQYfZB9nECHwNC/dC+hfTpVLFB4l9HE7sN2A"
    L"czm1jEtddeTeh3O9ez+fFPBJvZy5lDHmxTiO3/eJT3zCAEDZItCjwuUQACWKxKOPPnohiqJ3a62THKHbVAFQXm9PnETEwAfK"
    L"l3Ti0sCX7+fuEoBtnOVyN92XjM+y8Da/htsPXMX4RL1P5A+SDlQRTeeNMTYfJy2lfO+xY8dOHz58WF6O6C/G5nJvkNsD6tSp"
    L"U/9XSvnjWmuZ+6jW1eeAnwh4iJdPIi3jPsBPCJyjufXP7QTuKroEQH1xj3MCJJeQ+siN1kGI9yHdJ/Y555NqzK1+CeC/Pfvs"
    L"s38yMzOjTp48eUUqUS6bAIAte+DMmTO/AeADuT2gge0TPQB/QSVxP53nIthHCMUL5BxDM4/4l8zpHpzjuUvp2gPUhv77pBg9"
    L"L47jvlU6SPKUcXyZDVAm9plU01JKZa39vbvvvvvnlVLcc7vsLNUVIQAANo5jE0WRvPnmm3/CWvsnxphAa50B6NNlrvjnvzwF"
    L"DGwFP2hBaZ8Y9eltss7puYQQn+h3CZEjpW+gmGimKe40QdNa2yexfH0cZAySJONin5Avev7+1wD82Ec/+lEZRVEwPz8vc9wJ"
    L"9ntJID74wQ9e6rXbgL5Mcfvtt++6ePHiZ5RSb1ZKZUS13OBzB5j+E6L5oHKr27XsAX8RCke2z6qnNm7kkufZCWEuAdGEFdL/"
    L"7nPdPrjvysH1LKhP+XrNKkmSE0mSfO+JEyeeD8MwSntFkwYALf/Ct6sfCRwEzWbTRlEkH3/88dUjR458d7PZfBjAHQAypVRA"
    L"MXeeM3AHh1fvUGaPOINzmOve+XQ6P87VAj/PpQoFj4gAXLXDiUIIsS2+4FNR/B19UsVVkbk9pIUQKk3Ts51O50dOnjx5LgiC"
    L"yTRNE/Q4Pst/NXpIJ8TzZNBINsKVUgEFUAnZiRMnFmq12j/JsuwRrXWQZVnGLXRXJXBR7H53kCqF+XQySiTx2kQf5/GNu5Cu"
    L"bgf6xb/zNa7iPBEGDyePYuC5KoBUCT+eqz8jpVRa67lms/n+kydPfksptSfLshBABUDEfiP0mDgAoNCvDkZSC1dUAhAQEZw8"
    L"eXL+Na95zTtardYfBkHwFuSSgF6aqwQA2ziB4vAAiiXlacEpGjB3MSUuCdz9MgJxjUwOZaJ9lH3fr892ofc1xmillEqS5NTy"
    L"8vJPnTlz5ltBEOzNsqyLHkI1evP8MvQQrvPfLN/A2o0kAa4KAQBbRPD8888v3Hrrre9YWlr6XWvtO621WiklpZSC1/W7opqA"
    L"rxtIM34o9Eoc7X4nENiy+Pm9XOMM2C62fWqE9oGtXMMoUocDF/f8XM711lprpZSq2+0+OT8///Pz8/NnpJTjWZal6HE6F/0K"
    L"W4gXzkZtyCa4/JlBlwpEBMeOHVu76667vifLsg9kWaby3IEhMcqNPZdriUAos8erf7gBRsRA7iB5D+RB0DPcuMEg98+11Pnn"
    L"8lwi8HkmrlfikzSU1pVSyk6n86VTp079p/n5+fNCiDHTW7CbkB2gt3h3iC3xH7ItYG0Jr0QUpXDVJAABEcGDDz5oALz/wIED"
    L"zydJ8otBEMTWWi2lVORi8dy7zz/nXM7DyINEN9/3IZy3KTPohBDFCh1SbtUtDJICrv53+8QDPNZa02w2f+/48eMf73a7iRCi"
    L"aq1N0UMe1fiZfN9FKv/PDUCLnioYCFedAIDeDNUgCKQxJj537tyvT01NnTDG/HoYhjdKKY1SSvTKCYSXEMg14kTAAzp8IqiL"
    L"EAotjyLm3Ugi7dO8RUJ+GcG50sNHHHk/bU5MMk3TxeXl5Q+dOHHiLwCEQojAWsv1OX3uhRt3pAaALeIgAjHY8hBEfp5+t8E1"
    L"IQCgcO+sEGLXxYsX/2p8fPz7KpXKz4Zh+C/yQTFSSsldPY5wVwJwRNC+z7gCypM9LnJctcKvJdfT5WjXkCyTDKTaWFgX7Xb7"
    L"/509e/Z/LSwsnBFC1AGkdos6CWnuf9qnjYhBY8sLoOMDkQ9cQwIgsNZqAGPr6+vrAH5ucnLyyTAM3xOG4YGck43sQTGwbmCG"
    L"Ly1D40UhYI4g15YgLvZxJ1/fmK7hbXw+P7cPfDaBw/XGWiuVUjJJkpWVlZVPnzhx4nNa60RKWTfG0OqZhEBCmkS/FOBcPWgD"
    L"hhiAwLUlAE7NKYAagHhlZeXPoig61Wg03hWG4XeGYRgopaxSygohJNejNNg8b8ClAw/9ciS4Ip1LCxLrfCYRlxicqHwifpAt"
    L"kN+rMPLSNLWbm5tfnZub+4MLFy68AKAihBCmtzA34Nfvo4Bh20jWP8G1lgBkmGgACXoLGU4mSbK4vLz84VqtdjSO43dGUfT6"
    L"MAxFzvlGSildzuYForxejyKIvkAPsN0ft9YWU824KzpIp5epGoK8PwaAkPmSOpubm88uLCw8eObMmSfRq6VoGGNckU+/7s3d"
    L"cC9tGfqjgRz5rkHohWtFAFwXUce7ANrYcl8qrVbr2VarNVer1W6P4/gfRlH02jAMyS4woicSik/YcZ3Mua7MoieioF8+x5Da"
    L"cFXiXucaf66YtzkAkEopmSP+5OLi4p+fPn36kfx9YyFEZoxxF27yIck9xomAkJ85m5sjGAjXWgJQEiNDTw20sWW4GAATAIJW"
    L"q/VIu90+Ua1Wvz2O478fRdFrgyAIckRZa63NfWTBRbwvSEN2hItMN/0MoBTRZUAWPXrGbQFpmppms/nNpaWlr7744ovHrLUt"
    L"AEpKWTXGZB6uB/qR6+N4zTaO8NT5zxNFwBAiuNY2ACE6Q08F8HOcOMastWi1Wo+3Wq3nKpXK9dVq9fYwDL/dGDMZhqGgWIAQ"
    L"opAMZcgvHuJEB7k76UO6+59dR0iXhHStNZIkWd3c3HxmaWnpkcXFxdPIJZzI/XrDfdf+JA6BLtlcbqeQMCE/ZcdJJbziJAB1"
    L"iKzaFNsRn6KnGloAGgDGAOhut/vNbrd7Wkr5lTiOD9dqtSNKqRuklPUgCCRxv+wtZmEpzCylFFx3c2TziCP3JorObhmOliRP"
    L"r2mP2ACIfN7AZqfT+db6+vqJpaWlE0mSrOTvI4UQFWttmns+xJVch2v4uduHbL7Pz6fO9W6KeCBcczcQ5WKODMMuesZhG8Am"
    L"gHq+NYwx3VardbHVaj2plNpdqVReFUXRdWEYHlRK7QmCIJBSCpfr8zUF+gbEZ7Xnv0Iw1ifJorUW+b2yNE2Xu93u2WazObe2"
    L"tnY6TdMV9BAhhBBh/twkj+a5SOUIo//EEK4uT9HP7Qb9BMA5nsZ0RxNEXi4CIEnAdRZ/UU4ATQBV9NxG2ja11uutVutcq9V6"
    L"VAgxqZQar1QqB4Mg2B9F0XgURVMA6r3YUi/5hNxmcI1DlxCAXrxCa22MMe0syy4kSbKWJMnFVqv1Yrfb3bDWbuT9FyQdcm5P"
    L"sIXEBNuRlcBPED5jLnXacsLhup5zPjAC5xO8HARAQMEN7r7wQSI3cRNAnG+V/JcIoiqEqFlrV7Msq2RZ9i30EiN1IcSYECKW"
    L"UlajKJpQSo0LIapCiFDkkbgc+cWgWWu1tbabZdl6mqYrWuuO1jq11jbzvhB3aSllRuLdWttFP0JdBPqQ6koCt51r1JW5fGRH"
    L"7Bj5wMtLAMB2X5WoWWJLEgTo2QQBtrJglXyLrbVUHBGj52JVrLWhtTay1obGmCDLMsqSUaaMh0x9/eD7Bj2EG2ttlsfpyY0j"
    L"BBGnu8h0dbmP87lYd7mbI7ws0EP9HBjyLYOXmwAA/4vQABAhJNhKiSpspT8pNVqkSK21PEVapEyFEPx6IgDfBACLXBqwPmWm"
    L"9wk2jhDOoZnnmCu2XS7myDbOvmsw+gw7X4xgx/BKIAAOnAiALTVBFTBUEyfR6zv98nw5z58Xm+19RZuuIcTzylq3H7w/LmLK"
    L"jC+OXOO08R3zcbYP4a5Ff0nI9sErjQAIfC/rpkUl+sU5IVZ69vl/4nxfAoUTn089uR6MK5JdRPK2uuQe9N99lvv+VwzpHF6p"
    L"BEDAdRtHDrA1aGXZMLdu3q2S4XaART9x8efzfZ8IdnWycY77ri0z3K4Klw+CVzoBAP5B4YUS/LwvocKRKkvauseGPZ/2+fMH"
    L"ce2wNpdkwF0J+OtAAD64oobQKwBetn5f1aLQv4NXPvwdAfwth/8PjolSBNd+nFYAAAAASUVORK5CYII=";

// Red no-Internet marker for the middle of the Network -> Internet line.
// Generated for this mod and trimmed to a transparent 32x32 icon canvas.
static const WCHAR* NETWORK_NO_INTERNET_X_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAJBElEQVR4nK2We3BV1RXGv/04557c3JubXEKEJOSSm4RHeErk"
    L"lZjEUFHRCFINtVVsrU5t6zi1ttpOZ5yYcZx2xqIdmTq12mItWpRaayMJJGiCEESQVyAkBEEIKZCEJPf9Onuf3T/IZVJsCI5d"
    L"f+05Z531/db6ztn7AABeATQAeI3zx+onTzr/B2ApALQAHF8zkjVeA6qacrL7/gJ8f7Tm5cVLwEN771olz73ycqRh7pyedcCi"
    L"rwuRfPYloLxh/rx/X9jwp8in1bdHfg/cn9QmALAeeLi0+o5Xs29eZsX6+gjPylIdb/y1/8jBwyufBPa1ALwKEF9VvAoQLwIV"
    L"0xcs2Dznwe+5Er09ekpODult3IZ9jVvXPgpsJH+2Gz+fUV7xvKeywgr39hIiJGEphtJzcuIdmzYNHTxwaPUvgb1fBSKZ+zxQ"
    L"UXz9/LfmPrA2K37qJLdicaIYUylTctHbssPq3rnrcapzfnfujGlI+HxSRaKEADADARI/dco2a02Ne9bs4k0vAGVVgLgWO5Li"
    L"vwYqiksWbJ777XuzYsc6mAyGCCGAioSJ8PlF9rRpTNPYaro7EFq+642N21kwqPE0hxDhEIhSMINBEu06rs+7556cghnTN/wW"
    L"KB0PIin+HFA+b+EN7829+5uZ0aNHmQxHKKBgBoPQ0zNMGghobX97a8s/Q5FqAgDVgP07GRlbSu9ZfZMkRMSHhjjjGpQQoJpm"
    L"6R5P7PD79f2dnZ3fGsuOUeJlCxcvqp+54lZHuLOTQ0gCTYNMxGFkTTSJKbVP3v3H1ud8/lXHgASpBWgdYJUA9ifcGfWlq6qX"
    L"mZYlEoPDnOkcyhTQdB08zxM6+uFH4c/aj1TXAZ+Nhkiua4HSssUL62cuq3JEurp0KQSIxiETJmyZmYIRwtvee79xrc9/F4BE"
    L"LUAJAIyG+GlGRv2N1SuWxRNxkRj2c8Y5IEwwzmEvLEJHW9vF1v0Hb/sNsL92xI46QDwFLFm+aFHj7MrydN/RdqUkCNEYhGnC"
    L"cLtNZrNpu96vb3jA51+tlDKfIYTUARZJjjAJsQRI+XF6+pbK25ZXxaIRkfD5OeccSphglErHtOmsfc++wT2HD99SBxwAgKeB"
    L"pRU3lDTOWrLIFejokEopBs5hJkwYE9xCS7Hz1oat2x70+VYmO68DLAAgo31M3rgfSL3F7W4sL19anojFRMIf4ExjgClBCGRa"
    L"YSE7crB9cO/RjnIDsF0/d3Zr8YLrXf7u4xYACsYghYDhzjSZ3dB2ftTS9Oygb+UJpRLJzpOa/wUwGqIScDzidm9ZXDK/woxG"
    L"hBmOcsYoYFkgSlkuTx79rKNzgBDCSubMcvtOfWGBEApNg1QKmq4Lpuu87eDh5g3DvpU7gNjozscEuBLi4QzXlrLimZcg4nHO"
    L"KAWxLIAQZZs0iUABsQvnFSgl4BxmPAHE4oLpOt/be7b59VB0TPExAUZD1ACOO53OD8oKp1aKeFwIU3BGCWApEEoUAChFiJQm"
    L"4v4gVDgibO50fiAQano6GFp15iriVwUYDbEcSH0wzbltUe7kMjMas2ApSglgmQIyHoeMRJGIxgClrJTJWfRQNNb6qi+w4mqd"
    L"X3PUAlQpkBpAfzPVvrd7ZpE6kp4mD2maaidEHQTUfkDtJ5BHsrPU62mONgBMAbQWoOPVHzcBACUEqggonjAlJ0sKoWL+AFGm"
    L"CQUFQggIIbAAokBU5sSJk34BFBPAmjXOhMcFqAV4HSB+BcxfXFTQXMSoJ/j5aUUUIYoQSAVYSo3Ml5DouT7lFaZ3sSev+Slg"
    L"5hpA1o5zgF3tJeR1gHgMKF5WmN8yQ9eyho51SwYwRgmUpaBzJi2ACCEpCIEEIJWSzvw81qXU+U9Pn73pBaD7HYCtAeQ1AyQf"
    L"eAQo+kb+1I+mG1rucOcJSQlhGgGUpWDousU9ORQAzDO9VixhUtBLU5FKSVdRPjuekD0tZ3qW/RE4ORbElyxIJj4OeCs9edu9"
    L"hpbb33lCWgBTBEhYCpphE/aifLrX52884At9mFrkpZpNF6Z1yQ4FwgY//0J6DT3vxrzcpp8BnjWAfAdgVwVIiv8ImLIgN6ep"
    L"IDUlr6/rhBQEzKIEpqXADZswirx8V//F7Q8PDN1ZOzBQ/cnA0M6Ugqmc2zQhlIKkgFBggydOygJHqnd2bnbzk0D2GkDWXAFB"
    L"rxR/FMhekpPd5HU5C853dUtLgSlCkLAUuGGI1CIv333uQvOzA4N3KijrDBB7vn/g9j19Fz92FBRwMjIJRQmEpdjA8ROiMD2t"
    L"aMbkSU0/BLI2XwFBAaBmRPx+IKska+K2PId9xoWubmFZ6nLnzLAJu9fDPznf17JhcHjVGSD2DAipBegxILRxcOiOT/sGdjq9"
    L"+Zwm7aAElrR4X2e38KQ5ZpVcl9m0Fpiw+dLXQQGA1ABsMyAfAtyL3enNU9NdC3w9vUIJyTmjgLSgGzaRkZfL2y8OtTYMDd/x"
    L"ARAZvcONPjvuzcjYMntCRsXw2bPCjJt8ZBKgnIm03BzeEwjs2zHku/lNIHD5h+Q+IK3M6djudTkXBi70C0tIziiBZSnoNl04"
    L"syfxbn+gdfeQr3ojEP5f22vyWing/K7b1VjoTCsbPndeSFNwMlKLalw4sjL56WBo9+5A6JaNQJj8ALDPSUlpKHCmVoaGhoUS"
    L"koMCygKYpgnndRP5yUCw9ZVAsLp9DPErIWoAV5UrrbEgNWWpv39QKCEu1yScCYc7nZ8KRT484kpfSX5nGIfyHSnz4r6AtIRk"
    L"ZCSRciZSJ7j5F+Fw69ZQ5EtjHyuSOY8BaTMdqY15hl4aHvYLSOsyBOVM2DJc/HQwso+CISoVQChVAAAFEEZFijOVnwqGduwJ"
    L"RaqvVRwA6gCrFqDrgUBbKLyiJxLbbdgNDkoE1MibzxgsBShCIgQAf9GwvePV+ep4JCqUtGAYNn4WtG1PNHrrWJ6PF6PtqNC1"
    L"hikEpbGEKRln0O12djKW2PREPH4fFEBqamrYOl1/622Nqb8zotbreuuT06c7k4W+ivCVEABwn9udtl5jH79LiXpbY2od5xuA"
    L"S8O+dN6PnAkvatq/XuL88P9DPBnJTecnHk/6yzbt2Aua9m6ydi1A/wMm/WMxG2Z0fQAAAABJRU5ErkJggg==";



// Network location category values (from netlistmgr.h enum NLM_NETWORK_CATEGORY).
// NLM_NETWORK_CATEGORY_PUBLIC       = 0  -> Public Network  (bench icon, ID_1)
// NLM_NETWORK_CATEGORY_PRIVATE      = 1  -> Home Network    (house icon, ID_0)
// NLM_NETWORK_CATEGORY_DOMAIN_AUTHENTICATED = 2  -> Work Network    (buildings icon, ID_2)
// NOTE: Do NOT use #define here; netlistmgr.h already provides the enum type.
// Using #define would shadow the enum members with int literals and break compilation.

// Cached HICONs for the three network-location icons
static HICON g_hIconNetLocHome   = NULL;  // ID_0 - house
static HICON g_hIconNetLocPublic = NULL;  // ID_1 - bench
static HICON g_hIconNetLocWork   = NULL;  // ID_2 - buildings

// Current detected network location category for the active connection
static int g_CurrentNetworkCategory = -1;  // -1 = unknown/not connected
static int g_LastReliableNetworkCategory = -1;
static DWORD g_LastReliableNetworkCategoryTick = 0;

// ============================================================================
// Chevron arrow icons for the expand/collapse button (light theme only).
// Dark theme continues to use the Marlett font character.
// ============================================================================

// Chevron Up (list collapsed) - normal state
static const WCHAR* CHEVRON_UP_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAAA8AAAAPCAYAAAA71pVKAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAAZdEVYdFNvZnR3YXJlAFBhaW50Lk5FVCA1LjEuMTITAUd0AAAAuGVYSWZJSSoACAAAAAUAGgEFAAEAAABKAAAAGwEFAAEAAABSAAAAKAEDAAEAAAACAAAAMQECABEAAABaAAAAaYcEAAEAAABsAAAAAAAAAGAAAAABAAAAYAAAAAEAAABQYWludC5ORVQgNS4xLjEyAAADAACQBwAEAAAAMDIzMAGgAwABAAAAAQAAAAWgBAABAAAAlgAAAAAAAAACAAEAAgAEAAAAUjk4AAIABwAEAAAAMDEwMAAAAADZp5qVybcLXwAAADtJREFUOE9jGAXUBExJ/6EsrIARSmMCZI3/5mFVh10zNhuxGMAEpREAl41YDMTUDAMwjTicPApIBgwMAJGcDwcTcpGvAAAAAElFTkSuQmCC";

// Chevron Up (list collapsed) - highlight/hover state
static const WCHAR* CHEVRON_UP_HL_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAAA8AAAAPCAYAAAA71pVKAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAAZdEVYdFNvZnR3YXJlAFBhaW50Lk5FVCA1LjEuMTITAUd0AAAAuGVYSWZJSSoACAAAAAUAGgEFAAEAAABKAAAAGwEFAAEAAABSAAAAKAEDAAEAAAACAAAAMQECABEAAABaAAAAaYcEAAEAAABsAAAAAAAAAGAAAAABAAAAYAAAAAEAAABQYWludC5ORVQgNS4xLjEyAAADAACQBwAEAAAAMDIzMAGgAwABAAAAAQAAAAWgBAABAAAAlgAAAAAAAAACAAEAAgAEAAAAUjk4AAIABwAEAAAAMDEwMAAAAADZp5qVybcLXwAAAkRJREFUOE9tU89rE1EQ/rJJNiTZzW5rpdo2tDWlKT2oJ6neFG/FqwfPatB68P9QUW8iePUgCBVBL0JPLRa8iIitTUwk6Y9Uk2azDUl2s+85s0nENg58vN03883Mm/e9AP5jz99uyPyujdqhA1NTkRrXcXNxbiD2yMarlR9ys2hhcSGJ6VM6TF1FzXaQo0Tv14tIJw1cv3xmsOC9p2vyzWpBuh1PSikGwPvsX3qyKnuUbuWXH3IyrkWwcHYUISWAAEHpO8k4miGExNqXPdhWGzeupgIcg28lC6cnTJQbHvZbHqpOB9WOwME/qDjka3uYnBrBdzoGm/Li3aacnhyCJwU8jyFBC6gIprQMVVTgUQ8ebQjyCSEwlzoB5inZUh2GEYXrSnQogIMYM3rGzz6t3fLbZbCfmsDwUAzMU+ymi1BIAU2lCyLMD9/1iX2bTXAH3QnQ8KCqQTBP0aNhVKwWqk0HlYaD86NLfhDb8tbD3hcnvIMK3TujbDXBPGVmIoHi/iH26m3s1lu9UOD1xgN/Xd565K9s2zRljslvW2Ce38v9Z+syfW4c0UgQEUIoHEQ4rEDhy6BziICA6wh0XIF2y8XnTyU8zlzoXtWl9EnUyja0mAotGkEiFoYeV2GQwkxDpX/ai4d9f3nHxsXZEab5WvAlVywcoJSvUkAICRKMSWSTkhgM0rcWVfEz+wuF7O+/Eu2LyDfW9lfS9hXSdnJMh0YJbBpiiaqtfCxi/pi2j5D7xq8qt1NHveH67abGErh97firAv4AgAcsCQrs69wAAAAASUVORK5CYII=";

// Chevron Down (list expanded) - normal state
static const WCHAR* CHEVRON_DOWN_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAAA8AAAAPCAYAAAA71pVKAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAAZdEVYdFNvZnR3YXJlAFBhaW50Lk5FVCA1LjEuMTITAUd0AAAAuGVYSWZJSSoACAAAAAUAGgEFAAEAAABKAAAAGwEFAAEAAABSAAAAKAEDAAEAAAACAAAAMQECABEAAABaAAAAaYcEAAEAAABsAAAAAAAAAGAAAAABAAAAYAAAAAEAAABQYWludC5ORVQgNS4xLjEyAAADAACQBwAEAAAAMDIzMAGgAwABAAAAAQAAAAWgBAABAAAAlgAAAAAAAAACAAEAAgAEAAAAUjk4AAIABwAEAAAAMDEwMAAAAADZp5qVybcLXwAAAD1JREFUOE9jGAWUAqak/2CMDLCJAQETlMYEMMVYNMEAI5RGBdg0/JuHoRa7ZhBANgCLRsIAj5NHAcmAgQEA7m0PukT+CfYAAAAASUVORK5CYII=";

// Chevron Down (list expanded) - highlight/hover state
static const WCHAR* CHEVRON_DOWN_HL_BASE64 =
    L"iVBORw0KGgoAAAANSUhEUgAAAA8AAAAPCAYAAAA71pVKAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsIAAA7CARUoSoAAAAAZdEVYdFNvZnR3YXJlAFBhaW50Lk5FVCA1LjEuMTITAUd0AAAAuGVYSWZJSSoACAAAAAUAGgEFAAEAAABKAAAAGwEFAAEAAABSAAAAKAEDAAEAAAACAAAAMQECABEAAABaAAAAaYcEAAEAAABsAAAAAAAAAPJ2AQDoAwAA8nYBAOgDAABQYWludC5ORVQgNS4xLjEyAAADAACQBwAEAAAAMDIzMAGgAwABAAAAAQAAAAWgBAABAAAAlgAAAAAAAAACAAEAAgAEAAAAUjk4AAIABwAEAAAAMDEwMAAAAACDfy8cctDT3wAAAjpJREFUOE9tUztv01AU/uzYDkns2EALNBBRCCKFAYmtsIGYqJiQGFhBMNCVn4EKqAWBxMrAxEOCBakDalU2JFRoIWmDYrU1bZyHG9L6kcu5lxqRhiN9OrrnnO/cx/muhP/Y0zcLbHnVQ2PTh6VrKBw2cHNspK+2J/BieoktVpsYG83j2CEDlqGh4fkoU6N3H6so5k1cu3C8f8PxB7Ps1UyFBWHEGOv2gcd5/s79GbZD+bPz8/dlltGTGD1zEIosQSLIcZKMV3N0uwyzn9fgNbdx/VJB4jX4ajcxdMSC047wcyuC64dwwy7q/6DmU247wtHhAXyja3CTnr1dZFJKw6kT+7FHVXA2Ny4Slc0nwnMb1m8LP7/xCEHQxUK5hnbjF+SS3YJppijIENKxYosJsefG83QI7NubBufJXieAosigVxH45EztlPYSv7iPhafHg6YlwHmykVJRa27B7fiotQk02w/2Q1EY29zaJOUDkeNwmh1wnriz3YlwYMhAkjoqhKRCXpFwdeQuXn6/hyii44YMfhAhDEI4qy0MJmgiNy4Xpfo6vZ5ECxqRyoNETKgyXi9NUExGQuMxiIZUBmelBc4TozpfHETD8aCnNeipJLJpFUZGg0kKs0yN1hTLqCLvrHg4d3JAXEeQueSqlTrsZZcKFGRJMBaRLWpicpC+dRrnj9I6KqWNvxLt0SnX9jxp+yJpO58zoFMDjx7Rpt2m56o4vUvbPeTY+K8q071a7UAct5DL4taV3b8K+A2cwxTpxQY7TAAAAABJRU5ErkJggg==";

// Cached HICONs for the chevron arrow states (light theme)
static HICON g_hIconChevronUp      = NULL;
static HICON g_hIconChevronUpHL    = NULL;
static HICON g_hIconChevronDown    = NULL;
static HICON g_hIconChevronDownHL  = NULL;
static BOOL  g_chevronsLoaded      = FALSE;

// Forward declaration (full definition is later in the file)
static HICON CreateIconFromBase64PNG(const WCHAR* base64Str, int targetWidth, int targetHeight);

static void EnsureChevronIcons() {
    if (g_chevronsLoaded) return;
    g_chevronsLoaded = TRUE;
    int sz = ScaleDpi(16);
    g_hIconChevronUp     = CreateIconFromBase64PNG(CHEVRON_UP_BASE64, sz, sz);
    g_hIconChevronUpHL   = CreateIconFromBase64PNG(CHEVRON_UP_HL_BASE64, sz, sz);
    g_hIconChevronDown   = CreateIconFromBase64PNG(CHEVRON_DOWN_BASE64, sz, sz);
    g_hIconChevronDownHL = CreateIconFromBase64PNG(CHEVRON_DOWN_HL_BASE64, sz, sz);
}




static HICON g_hIconNetworkCenterConnect = NULL;
static HICON g_hIconNetworkCenterHomegroup = NULL;

static HICON g_hIconRefreshNormal = NULL;
static HICON g_hIconRefreshHover  = NULL;
static INetworkListManager* g_pNLM = NULL;

// -------------------------------------------------------
// Connection state
// -------------------------------------------------------
typedef enum {
    CONN_STATE_IDLE = 0,
    CONN_STATE_CONNECTING,
    CONN_STATE_CONNECTED,
    CONN_STATE_DISCONNECTING,
    CONN_STATE_ERROR
} ConnectionState;

// =========================================================
// Dark context menu support (right-click only; light theme untouched).
// =========================================================
namespace DarkContextMenu {
enum class AppMode {
    Default,
    AllowDark,
    ForceDark,
    ForceLight,
    Max
};

using FlushMenuThemes_T     = void(WINAPI*)();
using SetPreferredAppMode_T = AppMode(WINAPI*)(AppMode);
static HMODULE g_hUxtheme = NULL;
static FlushMenuThemes_T     pFlushMenuThemes    = nullptr;
static SetPreferredAppMode_T pSetPreferredAppMode = nullptr;

void Apply(BOOL dark) {
    if (!g_hUxtheme || !pSetPreferredAppMode || !pFlushMenuThemes) return;
    pFlushMenuThemes();
    pSetPreferredAppMode(dark ? AppMode::ForceDark : AppMode::Default);
}

static AppMode g_initialAppMode = AppMode::Default;

void Init() {
    g_hUxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (g_hUxtheme) {
        pSetPreferredAppMode = (SetPreferredAppMode_T)GetProcAddress(g_hUxtheme, MAKEINTRESOURCEA(135));
        pFlushMenuThemes     = (FlushMenuThemes_T)GetProcAddress(g_hUxtheme, MAKEINTRESOURCEA(136));
        // Save the current app mode so Uninit can restore it instead of
        // blindly resetting to Default (which would overwrite other mods'
        // dark-mode settings like AllowDark/ForceDark).
        if (pSetPreferredAppMode) {
            // Call with Default to get the previous mode returned, then restore it
            g_initialAppMode = pSetPreferredAppMode(AppMode::Default);
            pSetPreferredAppMode(g_initialAppMode);
        }
    }
}

void Uninit() {
    // Restore the original app mode instead of forcing Default.
    // Use pSetPreferredAppMode directly (not Apply()) so we restore
    // the exact mode that was active before the mod loaded, rather
    // than the mod's light-theme default.
    if (pSetPreferredAppMode) {
        pSetPreferredAppMode(g_initialAppMode);
        if (pFlushMenuThemes) pFlushMenuThemes();
    }
    if (g_hUxtheme) {
        FreeLibrary(g_hUxtheme);
        g_hUxtheme = NULL;
    }
}
} // namespace DarkContextMenu

// -------------------------------------------------------
// Theme color helper functions
// -------------------------------------------------------
COLORREF GetHeaderBgColor() {
    return (g_Settings.theme == 1) ? RGB(20, 20, 20) : RGB(255, 255, 255);
}

COLORREF GetContentBgColor() {
    return (g_Settings.theme == 1) ? RGB(20, 20, 20) : RGB(255, 255, 255);
}

COLORREF GetFooterBgColor() {
    return (g_Settings.theme == 1) ? RGB(30, 30, 30) : RGB(241, 245, 253);
}

COLORREF GetTextColor() {
    return (g_Settings.theme == 1) ? RGB(100, 200, 255) : RGB(0, 0, 0);
}

COLORREF GetSecondaryTextColor() {
    return (g_Settings.theme == 1) ? RGB(255, 255, 255) : RGB(110, 110, 110);
}

COLORREF GetLinkColor() {
    return (g_Settings.theme == 1) ? RGB(100, 200, 255) : RGB(14, 75, 184);
}

COLORREF GetRowSelectedColor() {
    return (g_Settings.theme == 1) ? RGB(40, 40, 50) : RGB(228, 241, 252);
}

COLORREF GetRowHoverColor() {
    return (g_Settings.theme == 1) ? RGB(35, 35, 45) : RGB(242, 247, 253);
}

COLORREF GetRowSelectedBorderColor() {
    return (g_Settings.theme == 1) ? RGB(60, 80, 120) : RGB(174, 212, 243);
}

COLORREF GetRowHoverBorderColor() {
    return (g_Settings.theme == 1) ? RGB(50, 70, 100) : RGB(216, 231, 248);
}

COLORREF GetNetworkNameColor() {
    return (g_Settings.theme == 1) ? RGB(100, 200, 255) : RGB(14, 75, 184);
}

// -------------------------------------------------------
// Structures
// -------------------------------------------------------
typedef struct {
    HWND     hWndFlyout;
    HANDLE   hWlanClient;
    HANDLE   hHotkeyThread;
    DWORD    dwHotkeyThreadId;
    volatile LONG refCount;
    volatile LONG isUninitializing;
    CRITICAL_SECTION csLock;
} ModContext;

typedef struct {
    WCHAR ssid[33];
    BOOL  isSecured;
    ULONG signalQuality;
    GUID  interfaceGuid;
    DOT11_BSS_TYPE dot11BssType;
    BOOL  hasProfile;
    BOOL  hasInternetAccess;
    DOT11_AUTH_ALGORITHM authAlgorithm;
    DOT11_CIPHER_ALGORITHM cipherAlgorithm;
    DOT11_MAC_ADDRESS bssid;
    BOOL  hasBssid;
    int   displaySuffix;
    ConnectionState connState;
    DWORD operationStartTime;  
} WifiNetworkItem;

typedef struct {
    HWND hWndNotify;
    GUID interfaceGuid;
    WCHAR ssid[33];
    WCHAR password[65];
    BOOL hasProfile;
    BOOL isSecured;     
    DOT11_BSS_TYPE dot11BssType;
    DOT11_AUTH_ALGORITHM authAlgorithm;
    DOT11_CIPHER_ALGORITHM cipherAlgorithm;
    DOT11_MAC_ADDRESS bssid;
    BOOL hasBssid;
} AsyncConnectContext;

// -------------------------------------------------------
// Windows version detection
// -------------------------------------------------------
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
    Wh_Log(L"Detected %s (build %lu.%lu.%lu)", 
           g_isWin11 ? L"Windows 11" : L"Windows 10",
           osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber);
    if (g_isWin11) {
        HWND hTray    = FindWindowW(L"Shell_TrayWnd", NULL);
        HWND hNotify  = hTray   ? FindWindowExW(hTray,   NULL, L"TrayNotifyWnd",   NULL) : NULL;
        HWND hSysPager= hNotify ? FindWindowExW(hNotify, NULL, L"SysPager",        NULL) : NULL;
        HWND hToolbar = hSysPager? FindWindowExW(hSysPager,NULL,L"ToolbarWindow32", NULL) : NULL;
        if (hToolbar) {
            int btnCount = (int)SendMessageW(hToolbar, TB_BUTTONCOUNT, 0, 0);
            Wh_Log(L"Win11: Win10 legacy taskbar detected (ToolbarWindow32 found, %d buttons)", btnCount);
        } else if (hSysPager) {
            Wh_Log(L"Win11: SysPager found but no ToolbarWindow32 - partial legacy taskbar");
        } else if (hNotify) {
            Wh_Log(L"Win11: TrayNotifyWnd found but no SysPager - modern taskbar only");
        } else if (hTray) {
            Wh_Log(L"Win11: Shell_TrayWnd found but no TrayNotifyWnd - unusual configuration");
        } else {
            Wh_Log(L"Win11: Shell_TrayWnd not found - taskbar not ready yet");
        }
        // Plain literal is enough here; this is purely an informational log
        // line, not a path used to load anything.
        static const WCHAR* kEpPniduiPath = L"C:\\Program Files\\ExplorerPatcher\\pnidui.dll";
        if (GetFileAttributesW(kEpPniduiPath) != INVALID_FILE_ATTRIBUTES) {
            Wh_Log(L"ExplorerPatcher detected: pnidui.dll found");
        } else {
            Wh_Log(L"ExplorerPatcher not detected");
        }
        WCHAR sysPniduiPath[MAX_PATH];
        GetSystemDirectoryW(sysPniduiPath, ARRAYSIZE(sysPniduiPath));
        StringCchCatW(sysPniduiPath, ARRAYSIZE(sysPniduiPath), L"\\pnidui.dll");
        if (GetFileAttributesW(sysPniduiPath) != INVALID_FILE_ATTRIBUTES) {
            Wh_Log(L"System pnidui.dll found");
        } else {
            Wh_Log(L"System pnidui.dll NOT found (Win11 24H2+ native)");
        }
    }
}

// -------------------------------------------------------
// Global variables
// -------------------------------------------------------
static ModContext g_Ctx        = {0};
static BOOL       g_Initialized = FALSE;
static BOOL       g_IsExplorerHost = FALSE;
HWND g_hWndFlyout          = NULL;
HWND g_hWndButtonConnect   = NULL;
HWND g_hWndCheckboxConnect = NULL;
BOOL g_bListExpanded       = TRUE;
HFONT g_hFontButton        = NULL;
int g_ButtonConnectIsOwnerDraw = -1;

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

static BOOL g_IsHoveringConnectButton = FALSE;

void ApplyNativeControlsTheme() {
    LPCWSTR themeName = (g_Settings.theme == 1) ? L"DarkMode_Explorer" : L"Explorer";
    
    if (g_hWndFlyout && IsWindow(g_hWndFlyout)) {
        SetWindowTheme(g_hWndFlyout, themeName, NULL);
        BOOL useDark = (g_Settings.theme == 1);
        DwmSetWindowAttribute(g_hWndFlyout, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDark, sizeof(useDark));
        SetWindowPos(g_hWndFlyout, NULL, 0, 0, 0, 0,
                     SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED|SWP_NOACTIVATE);
    }
    
    if (g_hWndCheckboxConnect && IsWindow(g_hWndCheckboxConnect))
        SetWindowTheme(g_hWndCheckboxConnect, themeName, NULL);
    if (g_hWndButtonConnect && IsWindow(g_hWndButtonConnect)) {
        if (g_Settings.theme == 1) {
            SetWindowTheme(g_hWndButtonConnect, L"DarkMode_Explorer", NULL);
        } else {
            SetWindowTheme(g_hWndButtonConnect, L"Explorer", NULL);
        }
    }
}

HFONT g_hFontNormal    = NULL;
HFONT g_hFontBold      = NULL;
HFONT g_hFontUnderline = NULL;
HFONT g_hFontCheckbox  = NULL;
HFONT g_hFontArrow     = NULL;
WifiNetworkItem g_NetworkList[50];

BOOL g_IsHoveringLink         = FALSE;
BOOL g_IsHoveringRefresh      = FALSE;
BOOL g_IsHoveringArrow        = FALSE;
int  g_ScrollPos              = 0;
int  g_SelectedRowIndex       = -1;
int  g_HoveredRowIndex        = -1;
int  g_KeyboardSelectedIndex  = -1;
int  g_ContextMenuTargetIndex = -1;
RECT g_rcRefreshButton = { 0 };
RECT g_rcArrowButton   = { 0 };
RECT g_rcCheckboxLabel = { 0 };
BOOL g_bShowCheckboxLabel = FALSE;
HICON g_hIconNetworkMap  = NULL;
HICON g_hIconGlobe       = NULL;
HICON g_hIconDisconnected = NULL;
HICON g_hIconAvailable = NULL;
// Separate cache for the DirectUI Network Map visual: LoadImageW_Hook is asked
// for these icons at whatever size DirectUI's icon() markup requests (e.g.
// 36rp, which varies with DPI), which does NOT match the fixed 48x48
// g_hIconNetworkMap/g_hIconGlobe used elsewhere (tray flyout header icon).
// Returning a CopyIcon() of the fixed 48x48 bitmap forced DirectUI to stretch
// it to 36x36 (or upscale it at >100% DPI), blurring otherwise sharp artwork.
// These are re-decoded from the same Base64 PNGs whenever the requested size
// changes, so the DUI visual always gets a native-resolution bicubic render.
static HICON g_hIconNetworkMapDUI = NULL;
static int   g_iconNetworkMapDUIW = 0, g_iconNetworkMapDUIH = 0;
static HICON g_hIconGlobeDUI = NULL;
static int   g_iconGlobeDUIW = 0, g_iconGlobeDUIH = 0;
static BOOL  g_iconGlobeDUIOnline = TRUE;
// Separate high-quality DirectUI cache for the active-network profile icon.
// Never return the flyout's fixed 35rp icon to DirectUI: it would upscale it.
static HICON g_hIconNetLocDUI = NULL;
static int   g_iconNetLocDUIW = 0, g_iconNetLocDUIH = 0;
static HICON g_hIconNoInternetXDUI = NULL;
static int   g_iconNoInternetXDUIW = 0, g_iconNoInternetXDUIH = 0;
// Size-keyed cache for the offline gray network icon, matching the pattern
// used by every other LoadImageW_Hook branch instead of re-decoding
// NETLOC_PUBLIC_OFFLINE_ICON_BASE64 on every DirectUI request.
static HICON g_hIconOfflineNetworkDUI = NULL;
static int   g_iconOfflineNetworkDUIW = 0, g_iconOfflineNetworkDUIH = 0;
static int   g_iconNetLocDUICategory = -1;
HICON g_hIconSignalBars[6] = { NULL };
HICON g_hIconRefreshWin7 = NULL;
int   g_PendingConnectIndex = -1;
HWND  g_hTooltip = NULL;
UINT_PTR g_RefreshTimer = 0;
UINT_PTR g_TimeoutTimer = 0;
// Persistent off-screen buffer for WM_PAINT, recreated only when the size
// changes (normally never, since the flyout has a fixed WINDOW_WIDTH/HEIGHT).
// Avoids a CreateCompatibleDC/CreateCompatibleBitmap pair on every repaint.
static HDC     g_hdcMemPaint = NULL;
static HBITMAP g_hbmMemPaint = NULL;
static int     g_memPaintWidth  = 0;
static int     g_memPaintHeight = 0;
HWND G_hSubclassedToolbar = nullptr;
static BYTE* g_pniduiBase = NULL;
static BYTE* g_pniduiEnd  = NULL;
// RAII deleter for HANDLE, used below for the connect mutex.
struct HandleDeleter {
    void operator()(HANDLE h) const {
        if (h && h != INVALID_HANDLE_VALUE) CloseHandle(h);
    }
};
using WinHandle = std::unique_ptr<std::remove_pointer<HANDLE>::type, HandleDeleter>;

// Small COM smart pointer for API out-parameters.  Keep ownership explicit for
// globals that deliberately outlive a scope; local COM interfaces should use
// this type so every early return and failed intermediate call releases them.
template <typename T>
class ComPtr {
public:
    ComPtr() noexcept = default;
    explicit ComPtr(T* value) noexcept : value_(value) {}
    ~ComPtr() { reset(); }
    ComPtr(const ComPtr&) = delete;
    ComPtr& operator=(const ComPtr&) = delete;
    ComPtr(ComPtr&& other) noexcept : value_(other.detach()) {}
    ComPtr& operator=(ComPtr&& other) noexcept {
        if (this != &other) reset(other.detach());
        return *this;
    }
    T* get() const noexcept { return value_; }
    T** put() noexcept { reset(); return &value_; }
    T* detach() noexcept { T* value = value_; value_ = nullptr; return value; }
    void reset(T* value = nullptr) noexcept {
        if (value_) value_->Release();
        value_ = value;
    }
    T* operator->() const noexcept { return value_; }
    explicit operator bool() const noexcept { return value_ != nullptr; }
private:
    T* value_ = nullptr;
};

struct WlanMemoryDeleter {
    void operator()(void* value) const noexcept { if (value) WlanFreeMemory(value); }
};
template <typename T>
using WlanMemoryPtr = std::unique_ptr<T, WlanMemoryDeleter>;

static WinHandle g_hConnectMutex;
static HMODULE g_hGdiPlus = NULL;
static ULONG_PTR g_gdiplusToken = 0;
static void* g_pBitmapSignalBars[6] = { NULL };
static void* g_pBitmapNetLocHome   = NULL;  // GDI+ cache for bicubic draw
static void* g_pBitmapNetLocPublic = NULL;
static void* g_pBitmapNetLocWork   = NULL;
static void* g_pBitmapNetworkMap   = NULL;  // GDI+ cache for the fallback PC icon

// Ethernet status variables
static BOOL  g_EthernetConnected = FALSE;
static WCHAR g_EthernetNetworkName[64] = {0};
static BOOL  g_EthernetHasInternet = FALSE;
static GUID  g_EthernetAdapterGuid = {0};
static BOOL  g_HasEthernetAdapterGuid = FALSE;

typedef int (WINAPI *GdipCreateBitmapFromHICONFunc)(HICON, void**);
typedef int (WINAPI *GdipSetInterpolationModeFunc)(void*, int);
typedef int (WINAPI *GdipDrawImageRectIFunc)(void*, void*, int, int, int, int);
typedef int (WINAPI *GdipDeleteGraphicsFunc)(void*);
typedef int (WINAPI *GdipCreateBitmapFromScan0Func)(int, int, int, int, const void*, void**);
typedef int (WINAPI *GdipGetImageGraphicsContextFunc)(void*, void**);
typedef int (WINAPI *GdipSetPixelOffsetModeFunc)(void*, int);
typedef int (WINAPI *GdipGraphicsClearFunc)(void*, unsigned int);
typedef int (WINAPI *GdipCreateHBITMAPFromBitmapFunc)(void*, HBITMAP*, unsigned int);
typedef int (WINAPI *GdipDisposeImageFunc)(void*);
// Used by CreateIconFromBase64PNG, which reuses the process-wide GDI+
// instance/token from InitGdiPlusRendering() instead of loading gdiplus.dll
// and starting/stopping its own GDI+ session on every call.
typedef int (WINAPI *GdipCreateBitmapFromStreamFunc)(IStream*, void**);
typedef int (WINAPI *GdipCreateHICONFromBitmapFunc)(void*, HICON*);

static GdipCreateBitmapFromHICONFunc pGdipCreateBitmapFromHICON = NULL;
static GdipSetInterpolationModeFunc pGdipSetInterpolationMode = NULL;
static GdipDrawImageRectIFunc pGdipDrawImageRectI = NULL;
static GdipDeleteGraphicsFunc pGdipDeleteGraphics = NULL;
static GdipCreateBitmapFromScan0Func pGdipCreateBitmapFromScan0 = NULL;
static GdipGetImageGraphicsContextFunc pGdipGetImageGraphicsContext = NULL;
static GdipSetPixelOffsetModeFunc pGdipSetPixelOffsetMode = NULL;
static GdipGraphicsClearFunc pGdipGraphicsClear = NULL;
static GdipCreateHBITMAPFromBitmapFunc pGdipCreateHBITMAPFromBitmap = NULL;
static GdipDisposeImageFunc pGdipDisposeImage = NULL;
static GdipCreateBitmapFromStreamFunc pGdipCreateBitmapFromStream = NULL;
static GdipCreateHICONFromBitmapFunc pGdipCreateHICONFromBitmap = NULL;

static BOOL g_inPasswordPrompt = FALSE;
LRESULT CALLBACK ToolbarWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass);
static WCHAR g_TooltipBuffer[1024] = {0};

// -------------------------------------------------------
// Localization
// -------------------------------------------------------
typedef enum {
    STR_CURRENT_CONNECTED,
    STR_INTERNET_ACCESS,
    STR_WIFI_HEADER,
    STR_CONNECTED_TEXT,
    STR_OPEN_SHARING_CENTER,
    STR_BTN_CONNECT,
    STR_BTN_DISCONNECT,
    STR_CTX_CONNECT,
    STR_CTX_DISCONNECT,
    STR_CTX_STATUS,
    STR_CTX_PROPERTIES,
    STR_NO_CONNECTIONS,
    STR_CONNECTIONS_AVAILABLE,
    STR_CHK_CONNECT_AUTO,
    STR_PWD_TITLE,
    STR_PWD_INSTRUCTIONS,
    STR_PWD_LABEL,
    STR_PWD_HIDE_CHARS,
    STR_PWD_OK,
    STR_PWD_CANCEL,
    STR_PWD_FAILED_TITLE,
    STR_PWD_FAILED_WRONG,
    STR_PWD_CONNECTION_FAILED,
    STR_NETWORK_PRIVACY_FMT,
    STR_SECURITY_TYPE,
    STR_SIGNAL_STRENGTH,
    STR_RADIO_TYPE,
    STR_SIG_EXCELLENT,
    STR_SIG_GOOD,
    STR_SIG_FAIR,
    STR_SIG_POOR,
    STR_SIG_NONE,
    STR_CONNECTING,
    STR_DISCONNECTING,
    STR_STATUS_CONNECTED,
    STR_STATUS_CONNECTING,
    STR_STATUS_NOT_CONNECTED,
    STR_ERROR_TITLE,
    STR_PROFILE_SAVE_FAILED,
    STR_CONNECTION_ERROR,
    STR_TIMEOUT_ERROR,
    STR_CONNECTION_TIMEOUT_MSG,
    STR_PWD_EMPTY,
    STR_TRAY_TROUBLESHOOT,
    STR_TRAY_NETWORK_SETTINGS,
    STR_COUNT
} LocaleStringId;

typedef struct {
    LANGID langId;
    const WCHAR* strings[STR_COUNT];
} LocalePack;

static const LocalePack g_Locales[] = {
    { 0x0409, {
        L"Currently connected to:",
        L"Internet access",
        L"Wireless Network Connection",
        L"Connected",
        L"Open Network and Sharing Center",
        L"Connect",
        L"Disconnect",
        L"Connect",
        L"Disconnect",
        L"Status",
        L"Properties",
        L"Not connected",
        L"Connections are available",
        L"Connect automatically",
        L"Connect to a Network",
        L"Type the network security key",
        L"Security key:",
        L"Hide characters",
        L"OK",
        L"Cancel",
        L"Connection Failed",
        L"The network security key isn't correct. Please try again.",
        L"Failed to connect to %s",
        L"Network %d",
        L"Security type:",
        L"Signal strength:",
        L"Radio type:",
        L"Excellent",
        L"Good",
        L"Fair",
        L"Poor",
        L"No signal",
        L"Connecting...",
        L"Disconnecting...",
        L"Status: Connected",
        L"Status: Connecting...",
        L"Status: Not connected",
        L"Error",
        L"Unable to save network profile (code: %lu)",
        L"Connection error (code: %lu)",
        L"Connection timed out",
        L"The connection attempt timed out. The network may be out of range.",
        L"Please enter a network security key.",
        L"Troubleshoot problems",
        L"Open Network and Sharing Center",
    }},
    { 0x0410, {
        L"Attualmente connesso a:",
        L"Accesso a Internet",
        L"Connessione rete wireless",
        L"Connesso",
        L"Apri Centro connessioni di rete e condivisione",
        L"Connetti",
        L"Disconnetti",
        L"Connetti",
        L"Disconnetti",
        L"Stato",
        L"Propriet\u00E0",
        L"Non connesso",
        L"Connessioni disponibili",
        L"Connetti automaticamente",
        L"Connetti a una rete",
        L"Digitare la chiave di sicurezza di rete",
        L"Chiave di sicurezza:",
        L"Nascondi caratteri",
        L"OK",
        L"Annulla",
        L"Impossibile connettersi",
        L"La chiave di sicurezza di rete non \u00E8 corretta. Riprova.",
        L"Connessione a %s fallita",
        L"Rete %d",
        L"Tipo di sicurezza:",
        L"Potenza segnale:",
        L"Tipo di radio:",
        L"Eccellente",
        L"Buona",
        L"Sufficiente",
        L"Scarsa",
        L"Nessun segnale",
        L"Connessione in corso...",
        L"Disconnessione in corso...",
        L"Stato: Connesso",
        L"Stato: Connessione in corso...",
        L"Stato: Non connesso",
        L"Errore",
        L"Impossibile salvare il profilo di rete (codice: %lu)",
        L"Errore di connessione (codice: %lu)",
        L"Timeout durante la connessione",
        L"Il tentativo di connessione \u00E8 scaduto. La rete potrebbe essere fuori portata.",
        L"Inserire una chiave di sicurezza di rete.",
        L"Risoluzione problemi",
        L"Apri Centro connessioni di rete e condivisione",
    }},
    { 0x040A, {
        L"Conectado actualmente a:",
        L"Acceso a Internet",
        L"Conexi\u00F3n de red inal\u00E1mbrica",
        L"Conectado",
        L"Abrir el Centro de redes y recursos compartidos",
        L"Conectar",
        L"Desconectar",
        L"Conectar",
        L"Desconectar",
        L"Estado",
        L"Propiedades",
        L"No conectado",
        L"Hay conexiones disponibles",
        L"Conectar autom\u00E1ticamente",
        L"Conectarse a una red",
        L"Escriba la clave de seguridad de red",
        L"Clave de seguridad:",
        L"Ocultar caracteres",
        L"Aceptar",
        L"Cancelar",
        L"Error de conexi\u00F3n",
        L"La clave de seguridad de red no es correcta. Vuelva a intentarlo.",
        L"No se pudo conectar a %s",
        L"Red %d",
        L"Tipo de seguridad:",
        L"Intensidad de la se\u00F1al:",
        L"Tipo de radio:",
        L"Excelente",
        L"Buena",
        L"Aceptable",
        L"Baja",
        L"Sin se\u00F1al",
        L"Conectando...",
        L"Desconectando...",
        L"Estado: Conectado",
        L"Estado: Conectando...",
        L"Estado: No conectado",
        L"Error",
        L"No se pudo guardar el perfil de red (c\u00F3digo: %lu)",
        L"Error de conexi\u00F3n (c\u00F3digo: %lu)",
        L"Tiempo de conexi\u00F3n agotado",
        L"Se agot\u00F3 el tiempo de espera de la conexi\u00F3n. Puede que la red est\u00E9 fuera de alcance.",
        L"Escriba una clave de seguridad de red.",
        L"Solucionar problemas",
        L"Abrir el Centro de redes y recursos compartidos",
    }},
    { 0x040C, {
        L"Connect\u00E9 actuellement \u00E0 :",
        L"Acc\u00E8s Internet",
        L"Connexion r\u00E9seau sans fil",
        L"Connect\u00E9",
        L"Ouvrir le Centre R\u00E9seau et partage",
        L"Connecter",
        L"D\u00E9connecter",
        L"Connecter",
        L"D\u00E9connecter",
        L"\u00C9tat",
        L"Propri\u00E9t\u00E9s",
        L"Non connect\u00E9",
        L"Des connexions sont disponibles",
        L"Connexion automatique",
        L"Se connecter \u00E0 un r\u00E9seau",
        L"Entrez la cl\u00E9 de s\u00E9curit\u00E9 r\u00E9seau",
        L"Cl\u00E9 de s\u00E9curit\u00E9 :",
        L"Masquer les caract\u00E8res",
        L"OK",
        L"Annuler",
        L"\u00C9chec de la connexion",
        L"La cl\u00E9 de s\u00E9curit\u00E9 r\u00E9seau est incorrecte. Veuillez r\u00E9essayer.",
        L"Impossible de se connecter \u00E0 %s",
        L"R\u00E9seau %d",
        L"Type de s\u00E9curit\u00E9 :",
        L"Intensit\u00E9 du signal :",
        L"Type de r\u00E9seau radio :",
        L"Excellent",
        L"Bon",
        L"Correct",
        L"Faible",
        L"Aucun signal",
        L"Connexion en cours...",
        L"D\u00E9connexion en cours...",
        L"\u00C9tat : Connect\u00E9",
        L"\u00C9tat : Connexion en cours...",
        L"\u00C9tat : Non connect\u00E9",
        L"Erreur",
        L"Impossible d'enregistrer le profil r\u00E9seau (code : %lu)",
        L"Erreur de connexion (code : %lu)",
        L"D\u00E9lai de connexion d\u00E9pass\u00E9",
        L"La tentative de connexion a expir\u00E9. Le r\u00E9seau est peut-\u00EAtre hors de port\u00E9e.",
        L"Veuillez entrer une cl\u00E9 de s\u00E9curit\u00E9 r\u00E9seau.",
        L"R\u00E9soudre les probl\u00E8mes",
        L"Ouvrir le Centre R\u00E9seau et partage",
    }},
    { 0x0419, {
        L"\u0422\u0435\u043A\u0443\u0449\u0435\u0435 \u043F\u043E\u0434\u043A\u043B\u044E\u0447\u0435\u043D\u0438\u0435:",
        L"\u0414\u043E\u0441\u0442\u0443\u043F \u0432 \u0418\u043D\u0442\u0435\u0440\u043D\u0435\u0442",
        L"\u0411\u0435\u0441\u043F\u0440\u043E\u0432\u043E\u0434\u043D\u0430\u044F \u0441\u0435\u0442\u044C",
        L"\u041F\u043E\u0434\u043A\u043B\u044E\u0447\u0435\u043D\u043E",
        L"\u0426\u0435\u043D\u0442\u0440 \u0441\u0435\u0442\u0435\u0439 \u0438 \u043E\u0431\u0449\u0435\u0433\u043E \u0434\u043E\u0441\u0442\u0443\u043F\u0430",
        L"\u041F\u043E\u0434\u043A\u043B\u044E\u0447\u0438\u0442\u044C",
        L"\u041E\u0442\u043A\u043B\u044E\u0447\u0438\u0442\u044C",
        L"\u041F\u043E\u0434\u043A\u043B\u044E\u0447\u0438\u0442\u044C",
        L"\u041E\u0442\u043A\u043B\u044E\u0447\u0438\u0442\u044C",
        L"\u0421\u043E\u0441\u0442\u043E\u044F\u043D\u0438\u0435",
        L"\u0421\u0432\u043E\u0439\u0441\u0442\u0432\u0430",
        L"\u041D\u0435 \u043F\u043E\u0434\u043A\u043B\u044E\u0447\u0435\u043D\u043E",
        L"\u0414\u043E\u0441\u0442\u0443\u043F\u043D\u044B\u0435 \u043F\u043E\u0434\u043A\u043B\u044E\u0447\u0435\u043D\u0438\u044F",
        L"\u0410\u0432\u0442\u043E\u043F\u043E\u0434\u043A\u043B\u044E\u0447\u0435\u043D\u0438\u0435",
        L"\u041F\u043E\u0434\u043A\u043B\u044E\u0447\u0438\u0442\u044C\u0441\u044F \u043A \u0441\u0435\u0442\u0438",
        L"\u0412\u0432\u0435\u0434\u0438\u0442\u0435 \u043A\u043B\u044E\u0447 \u0431\u0435\u0437\u043E\u043F\u0430\u0441\u043D\u043E\u0441\u0442\u0438",
        L"\u041A\u043B\u044E\u0447 \u0431\u0435\u0437\u043E\u043F\u0430\u0441\u043D\u043E\u0441\u0442\u0438:",
        L"\u0421\u043A\u0440\u044B\u0442\u044C \u0441\u0438\u043C\u0432\u043E\u043B\u044B",
        L"\u041E\u041A",
        L"\u041E\u0442\u043C\u0435\u043D\u0430",
        L"\u041E\u0448\u0438\u0431\u043A\u0430 \u043F\u043E\u0434\u043A\u043B\u044E\u0447\u0435\u043D\u0438\u044F",
        L"\u041D\u0435\u0432\u0435\u0440\u043D\u044B\u0439 \u043A\u043B\u044E\u0447. \u041F\u043E\u043F\u0440\u043E\u0431\u0443\u0439\u0442\u0435 \u0435\u0449\u0435 \u0440\u0430\u0437.",
        L"\u041D\u0435 \u0443\u0434\u0430\u043B\u043E\u0441\u044C \u043F\u043E\u0434\u043A\u043B\u044E\u0447\u0438\u0442\u044C\u0441\u044F \u043A %s",
        L"\u0421\u0435\u0442\u044C %d",
        L"\u0422\u0438\u043F \u0431\u0435\u0437\u043E\u043F\u0430\u0441\u043D\u043E\u0441\u0442\u0438:",
        L"\u0423\u0440\u043E\u0432\u0435\u043D\u044C \u0441\u0438\u0433\u043D\u0430\u043B\u0430:",
        L"\u0422\u0438\u043F \u0440\u0430\u0434\u0438\u043E:",
        L"\u041E\u0442\u043B\u0438\u0447\u043D\u044B\u0439",
        L"\u0425\u043E\u0440\u043E\u0448\u0438\u0439",
        L"\u0421\u0440\u0435\u0434\u043D\u0438\u0439",
        L"\u0421\u043B\u0430\u0431\u044B\u0439",
        L"\u041D\u0435\u0442 \u0441\u0438\u0433\u043D\u0430\u043B\u0430",
        L"\u041F\u043E\u0434\u043A\u043B\u044E\u0447\u0435\u043D\u0438\u0435...",
        L"\u041E\u0442\u043A\u043B\u044E\u0447\u0435\u043D\u0438\u0435...",
        L"\u0421\u043E\u0441\u0442\u043E\u044F\u043D\u0438\u0435: \u041F\u043E\u0434\u043A\u043B\u044E\u0447\u0435\u043D\u043E",
        L"\u0421\u043E\u0441\u0442\u043E\u044F\u043D\u0438\u0435: \u041F\u043E\u0434\u043A\u043B\u044E\u0447\u0435\u043D\u0438\u0435...",
        L"\u0421\u043E\u0441\u0442\u043E\u044F\u043D\u0438\u0435: \u041D\u0435 \u043F\u043E\u0434\u043A\u043B\u044E\u0447\u0435\u043D\u043E",
        L"\u041E\u0448\u0438\u0431\u043A\u0430",
        L"\u041E\u0448\u0438\u0431\u043A\u0430 \u0441\u043E\u0445\u0440\u0430\u043D\u0435\u043D\u0438\u044F \u043F\u0440\u043E\u0444\u0438\u043B\u044F (\u043A\u043E\u0434: %lu)",
        L"\u041E\u0448\u0438\u0431\u043A\u0430 \u043F\u043E\u0434\u043A\u043B\u044E\u0447\u0435\u043D\u0438\u044F (\u043A\u043E\u0434: %lu)",
        L"\u0412\u0440\u0435\u043C\u044F \u043E\u0436\u0438\u0434\u0430\u043D\u0438\u044F \u0438\u0441\u0442\u0435\u043A\u043B\u043E",
        L"\u041F\u043E\u0434\u043A\u043B\u044E\u0447\u0435\u043D\u0438\u0435 \u043D\u0435 \u0443\u0434\u0430\u043B\u043E\u0441\u044C. \u0421\u0435\u0442\u044C \u0432\u043D\u0435 \u0437\u043E\u043D\u044B \u0434\u0435\u0439\u0441\u0442\u0432\u0438\u044F.",
        L"\u0412\u0432\u0435\u0434\u0438\u0442\u0435 \u043A\u043B\u044E\u0447 \u0431\u0435\u0437\u043E\u043F\u0430\u0441\u043D\u043E\u0441\u0442\u0438.",
        L"\u0423\u0441\u0442\u0440\u0430\u043D\u0435\u043D\u0438\u0435 \u043D\u0435\u043F\u043E\u043B\u0430\u0434\u043E\u043A",
        L"\u0426\u0435\u043D\u0442\u0440 \u0441\u0435\u0442\u0435\u0439 \u0438 \u043E\u0431\u0449\u0435\u0433\u043E \u0434\u043E\u0441\u0442\u0443\u043F\u0430",
    }},
    { 0x0407, {
        L"Verbunden mit:",
        L"Internetzugriff",
        L"WLAN-Verbindung",
        L"Verbunden",
        L"Netzwerkcenter \u00F6ffnen",
        L"Verbinden",
        L"Trennen",
        L"Verbinden",
        L"Trennen",
        L"Status",
        L"Eigenschaften",
        L"Nicht verbunden",
        L"Verbindungen verf\u00FCgbar",
        L"Automatisch verbinden",
        L"Netzwerk verbinden",
        L"Sicherheitsschl\u00FCssel eingeben:",
        L"Schl\u00FCssel:",
        L"Verstecken",
        L"OK",
        L"Abbrechen",
        L"Fehler",
        L"Falscher Schl\u00FCssel. Versuche neu.",
        L"Verbindung zu %s fehlgeschlagen",
        L"Netzwerk %d",
        L"Sicherheit:",
        L"Signal:",
        L"Funktyp:",
        L"Exzellent",
        L"Gut",
        L"Mittel",
        L"Schwach",
        L"Kein Signal",
        L"Verbinde...",
        L"Trenne...",
        L"Status: Verbunden",
        L"Status: Verbinde...",
        L"Status: Nicht verbunden",
        L"Fehler",
        L"Profil speichern fehlgeschlagen (Code: %lu)",
        L"Verbindungsfehler (Code: %lu)",
        L"Zeit\u00FCberschreitung",
        L"Verbindungsversuch abgebrochen. Netzwerk au\u00DFer Reichweite.",
        L"Bitte Sicherheitsschl\u00FCssel eingeben.",
        L"Problembehandlung",
        L"Netzwerkcenter \u00F6ffnen",
    }},
    { 0x0816, {
        L"Ligado a:",
        L"Acesso \u00E0 Internet",
        L"Rede Wi-Fi",
        L"Ligado",
        L"Abrir Centro de Rede",
        L"Ligar",
        L"Desligar",
        L"Ligar",
        L"Desligar",
        L"Estado",
        L"Propriedades",
        L"N\u00E3o ligado",
        L"Liga\u00E7\u00F5es dispon\u00EDveis",
        L"Ligar automaticamente",
        L"Ligar \u00E0 Rede",
        L"Digite a chave de seguran\u00E7a:",
        L"Chave:",
        L"Ocultar",
        L"OK",
        L"Cancelar",
        L"Falha",
        L"Chave errada. Tente novamente.",
        L"Falha ao ligar a %s",
        L"Rede %d",
        L"Seguran\u00E7a:",
        L"Sinal:",
        L"Tipo r\u00E1dio:",
        L"Excelente",
        L"Bom",
        L"Razo\u00E1vel",
        L"Fraco",
        L"Sem sinal",
        L"A ligar...",
        L"A desligar...",
        L"Estado: Ligado",
        L"Estado: A ligar...",
        L"Estado: N\u00E3o ligado",
        L"Erro",
        L"Falha ao guardar perfil (C\u00F3digo: %lu)",
        L"Erro de liga\u00E7\u00E3o (C\u00F3digo: %lu)",
        L"Tempo excedido",
        L"Tentativa expirou. Rede fora de alcance.",
        L"Insira a chave de seguran\u00E7a.",
        L"Resolver problemas",
        L"Abrir Centro de Rede",
    }},
    { 0x0415, {
        L"Obecnie połączono z:",
        L"Dostęp do Internetu",
        L"Sieć bezprzewodowa",
        L"Połączono",
        L"Otwórz Centrum sieci i udostępniania",
        L"Połącz",
        L"Rozłącz",
        L"Połącz",
        L"Rozłącz",
        L"Stan",
        L"Właściwości",
        L"Brak połączenia",
        L"Dostępne połączenia",
        L"Połącz automatycznie",
        L"Połącz z siecią",
        L"Wprowadź klucz zabezpieczeń sieci",
        L"Klucz zabezpieczeń:",
        L"Ukryj znaki",
        L"OK",
        L"Anuluj",
        L"Błąd połączenia",
        L"Klucz zabezpieczeń jest nieprawidłowy. Spróbuj ponownie.",
        L"Nie udało się połączyć z %s",
        L"Sieć %d",
        L"Typ zabezpieczeń:",
        L"Siła sygnału:",
        L"Typ radia:",
        L"Doskonały",
        L"Dobry",
        L"Przeciętny",
        L"Słaby",
        L"Brak sygnału",
        L"Łączenie...",
        L"Rozłączanie...",
        L"Stan: Połączono",
        L"Stan: Łączenie...",
        L"Stan: Brak połączenia",
        L"Błąd",
        L"Błąd zapisu profilu sieci (kod: %lu)",
        L"Błąd połączenia (kod: %lu)",
        L"Przekroczono limit czasu",
        L"Próba połączenia wygasła. Sieć może być poza zasięgiem.",
        L"Wprowadź klucz zabezpieczeń sieci.",
        L"Rozwiązywanie problemów",
        L"Otwórz Centrum sieci i udostępniania",
    }},
    { 0x0413, {
        L"Momenteel verbonden met:",
        L"Internettoegang",
        L"Draadloze netwerkverbinding",
        L"Verbonden",
        L"Netwerkcentrum openen",
        L"Verbinden",
        L"Verbreken",
        L"Verbinden",
        L"Verbreken",
        L"Status",
        L"Eigenschappen",
        L"Niet verbonden",
        L"Verbindingen beschikbaar",
        L"Automatisch verbinden",
        L"Verbinding maken met een netwerk",
        L"Voer de netwerkbeveiligingssleutel in",
        L"Beveiligingssleutel:",
        L"Tekens verbergen",
        L"OK",
        L"Annuleren",
        L"Verbindingsfout",
        L"De beveiligingssleutel is onjuist. Probeer opnieuw.",
        L"Verbinding met %s mislukt",
        L"Netwerk %d",
        L"Beveiligingstype:",
        L"Signaalsterkte:",
        L"Radiotype:",
        L"Uitstekend",
        L"Goed",
        L"Redelijk",
        L"Zwak",
        L"Geen signaal",
        L"Verbinden...",
        L"Verbreken...",
        L"Status: Verbonden",
        L"Status: Verbinden...",
        L"Status: Niet verbonden",
        L"Fout",
        L"Netwerkprofiel opslaan mislukt (code: %lu)",
        L"Verbindingsfout (code: %lu)",
        L"Time-out",
        L"Verbindingspoging verlopen. Netwerk mogelijk buiten bereik.",
        L"Voer een netwerkbeveiligingssleutel in.",
        L"Problemen oplossen",
        L"Netwerkcentrum openen",
    }},
    { 0x0418, {
        L"Conectat în prezent la:",
        L"Acces la Internet",
        L"Conexiune rețea fără fir",
        L"Conectat",
        L"Deschide Centrul de rețea și partajare",
        L"Conectare",
        L"Deconectare",
        L"Conectare",
        L"Deconectare",
        L"Stare",
        L"Proprietăți",
        L"Neconectat",
        L"Conexiuni disponibile",
        L"Conectare automată",
        L"Conectare la o rețea",
        L"Introduceți cheia de securitate a rețelei",
        L"Cheie de securitate:",
        L"Ascundere caractere",
        L"OK",
        L"Anulare",
        L"Eroare de conectare",
        L"Cheia de securitate este incorectă. Reîncercați.",
        L"Conectare la %s eșuată",
        L"Rețea %d",
        L"Tip securitate:",
        L"Putere semnal:",
        L"Tip radio:",
        L"Excelent",
        L"Bun",
        L"Mediu",
        L"Slab",
        L"Fără semnal",
        L"Conectare...",
        L"Deconectare...",
        L"Stare: Conectat",
        L"Stare: Conectare...",
        L"Stare: Neconectat",
        L"Eroare",
        L"Salvare profil rețea eșuată (cod: %lu)",
        L"Eroare de conectare (cod: %lu)",
        L"Depășire timp",
        L"Încercarea de conectare a expirat. Rețeaua poate fi în afara razei.",
        L"Introduceți o cheie de securitate a rețelei.",
        L"Remediere probleme",
        L"Deschide Centrul de rețea și partajare",
    }},
};

static const LocalePack* g_CurrentLocalePack = &g_Locales[0];
#define LOC(id) (g_CurrentLocalePack->strings[id])

static const LocalePack* FindLocalePack(LANGID langId) {
    LANGID primaryLang = PRIMARYLANGID(langId);
    const size_t count = ARRAYSIZE(g_Locales);
    for (size_t i = 0; i < count; ++i) {
        if (g_Locales[i].langId == langId) return &g_Locales[i];
    }
    for (size_t i = 0; i < count; ++i) {
        if (PRIMARYLANGID(g_Locales[i].langId) == primaryLang) return &g_Locales[i];
    }
    return &g_Locales[0];
}

void DetermineLocale() {
    switch (g_Settings.language) {
        case 1: g_CurrentLocalePack = FindLocalePack(0x0409); break;
        case 2: g_CurrentLocalePack = FindLocalePack(0x0410); break;
        case 3: g_CurrentLocalePack = FindLocalePack(0x040A); break;
        case 4: g_CurrentLocalePack = FindLocalePack(0x040C); break;
        case 5: g_CurrentLocalePack = FindLocalePack(0x0419); break;
        case 6: g_CurrentLocalePack = FindLocalePack(0x0407); break;
        case 7: g_CurrentLocalePack = FindLocalePack(0x0816); break;
        case 8: g_CurrentLocalePack = FindLocalePack(0x0415); break;
        case 9: g_CurrentLocalePack = FindLocalePack(0x0413); break;
        case 10: g_CurrentLocalePack = FindLocalePack(0x0418); break;
        default: {
            LANGID userLangId = GetUserDefaultUILanguage();
            g_CurrentLocalePack = FindLocalePack(userLangId);
        } break;
    }
}

static const WCHAR* SignalQualityToString(ULONG quality) {
    if (quality > 80) return LOC(STR_SIG_EXCELLENT);
    if (quality > 60) return LOC(STR_SIG_GOOD);
    if (quality > 40) return LOC(STR_SIG_FAIR);
    if (quality > 20) return LOC(STR_SIG_POOR);
    return LOC(STR_SIG_NONE);
}

// -------------------------------------------------------
// Prototypes
// -------------------------------------------------------
static bool IsExplorerProcess();
void BuildWlanProfileXml(const WifiNetworkItem* item, const WCHAR* password, BOOL autoConnect, WCHAR* outXml, size_t outSize);
static BOOL XmlTagEqualsCI(const WCHAR* xml, const WCHAR* tagName, const WCHAR* expectedValue);
static BOOL ProfileSecurityMatches(const WCHAR* profileXml, DOT11_AUTH_ALGORITHM authAlgorithm, DOT11_CIPHER_ALGORITHM cipherAlgorithm);
LRESULT CALLBACK ToolbarWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass);
void RefreshWifiData(HANDLE hClient);
void UpdateLayoutGeometry(int scrollbarOffset = 0);
void ConnectToNetwork(int index);
void DisconnectFromNetwork(int index);
void CheckConnectionTimeouts(void);
BOOL SafeToAccessUI(void);
void SafeCleanup(void);
void ToggleFlyoutWindow(void);
void InitTooltip(HWND hwnd);
void UpdateTooltipForRow(HWND hwnd, int index);
BOOL GetRowRect(int index, RECT* rcRow);
BOOL InstallTrayInterception(void);
void RemoveTrayInterception(void);
void InitRefreshButtonRect(void);
void SetKeyboardFocus(int index);
void ClearKeyboardFocus(void);
BOOL IsInternetConnected(void);
static BOOL AskForPasswordAndConnect(int index);
static int GetTotalListHeight(void);  
static void LogSsidSafe(const WCHAR* prefix, const WCHAR* ssid);

static void LogSsidSafe(const WCHAR* prefix, const WCHAR* ssid) {
    if (!ssid || ssid[0] == L'\0') {
        Wh_Log(L"%s <empty>", prefix);
        return;
    }
    WCHAR safe[33] = {0};
    if (lstrlenW(ssid) <= 3) {
        StringCchPrintfW(safe, ARRAYSIZE(safe), L"%s", ssid);
    } else {
        StringCchPrintfW(safe, ARRAYSIZE(safe), L"%.3s***", ssid);
    }
    Wh_Log(L"%s %s", prefix, safe);
}

static int Base64CharValue(WCHAR ch) {
    if (ch >= L'A' && ch <= L'Z') return ch - L'A';
    if (ch >= L'a' && ch <= L'z') return ch - L'a' + 26;
    if (ch >= L'0' && ch <= L'9') return ch - L'0' + 52;
    if (ch == L'+') return 62;
    if (ch == L'/') return 63;
    return -1;
}

static BYTE* DecodeBase64W(const WCHAR* base64Str, DWORD* outLen) {
    if (!base64Str || !outLen) return NULL;
    *outLen = 0;

    int len = lstrlenW(base64Str);
    DWORD allocLen = (DWORD)((len * 3) / 4 + 4);
    BYTE* data = (BYTE*)malloc(allocLen);
    if (!data) return NULL;

    DWORD val = 0;
    int bits = -8;
    DWORD pos = 0;
    BOOL seenPadding = FALSE;

    for (int i = 0; i < len; i++) {
        WCHAR ch = base64Str[i];
        if (ch == L'=' ) {
            seenPadding = TRUE;
            continue;
        }
        if (ch == L' ' || ch == L'\t' || ch == L'\r' || ch == L'\n') {
            continue;
        }
        if (seenPadding) {
            // Padding marks the end of one Base64 payload. Stop here instead
            // of accidentally appending garbage or a second PNG to the stream.
            break;
        }
        int digit = Base64CharValue(ch);
        if (digit < 0) {
            continue;
        }
        val = ((val << 6) | (DWORD)digit) & 0x00FFFFFF;
        bits += 6;
        if (bits >= 0) {
            if (pos >= allocLen) {
                BYTE* grown = (BYTE*)realloc(data, allocLen + 1024);
                if (!grown) { free(data); return NULL; }
                data = grown;
                allocLen += 1024;
            }
            data[pos++] = (BYTE)((val >> bits) & 0xFF);
            bits -= 8;
        }
    }

    if (pos == 0) {
        free(data);
        return NULL;
    }
    *outLen = pos;
    return data;
}

static BOOL InitGdiPlusRendering();

static HICON CreateIconFromBase64PNG(const WCHAR* base64Str, int targetWidth = 0,
                                      int targetHeight = 0) {
    // Reuse the process-wide GDI+ instance/token and the function pointers
    // already resolved by InitGdiPlusRendering(), instead of doing a fresh
    // LoadLibraryExW(gdiplus.dll) + GdiplusStartup + ... + GdiplusShutdown +
    // FreeLibrary on every single icon decode. InitGdiPlusRendering() is
    // idempotent and returns immediately if already initialized, so calling
    // it here also makes this function safe to use before the mod's own
    // one-time init call happens to run.
    if (!InitGdiPlusRendering() || !pGdipCreateBitmapFromStream || !pGdipCreateHICONFromBitmap)
        return NULL;

    DWORD outLen = 0;
    BYTE* data = DecodeBase64W(base64Str, &outLen);
    if (!data || outLen == 0) return NULL;

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, outLen);
    if (!hMem) { free(data); return NULL; }
    void* pMem = GlobalLock(hMem);
    if (!pMem) { GlobalFree(hMem); free(data); return NULL; }
    memcpy(pMem, data, outLen);
    GlobalUnlock(hMem);
    free(data);
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
            if (pGdipCreateBitmapFromScan0(targetWidth, targetHeight, 0, 0x00E200B, NULL,
                                            &dstBitmap) == 0 &&
                dstBitmap) {
                void* graphics = NULL;
                if (pGdipGetImageGraphicsContext(dstBitmap, &graphics) == 0 && graphics) {
                    // 7 is GDI+'s HighQualityBicubic mode; 3 matches the
                    // pixel-offset mode used by DrawIconBicubic below.
                    pGdipSetInterpolationMode(graphics, 7);
                    pGdipSetPixelOffsetMode(graphics, 3);
                    pGdipGraphicsClear(graphics, 0);
                    scaled = pGdipDrawImageRectI(graphics, srcBitmap, 0, 0,
                                                  targetWidth, targetHeight) == 0;
                    pGdipDeleteGraphics(graphics);
                }
                if (scaled)
                    pGdipCreateHICONFromBitmap(dstBitmap, &hIcon);
                pGdipDisposeImage(dstBitmap);
            }
        }
        if (!scaled)
            pGdipCreateHICONFromBitmap(srcBitmap, &hIcon);
        pGdipDisposeImage(srcBitmap);
    }
    stream->Release();
    return hIcon;
}

static HICON CopyNetworkCenterIcon(int resourceId, int targetWidth, int targetHeight) {
    HICON* source = nullptr;
    int* cachedWidth = nullptr;
    int* cachedHeight = nullptr;
    const WCHAR* base64 = nullptr;
    static int connectCachedW = 0, connectCachedH = 0;
    static int homegroupCachedW = 0, homegroupCachedH = 0;
    if (resourceId == 22) {
        source = &g_hIconNetworkCenterConnect;
        cachedWidth = &connectCachedW;
        cachedHeight = &connectCachedH;
        base64 = NETWORK_CENTER_CONNECT_ICON_BASE64;
    } else if (resourceId == 27) {
        source = &g_hIconNetworkCenterHomegroup;
        cachedWidth = &homegroupCachedW;
        cachedHeight = &homegroupCachedH;
        base64 = NETWORK_CENTER_HOMEGROUP_ICON_BASE64;
    } else {
        return NULL;
    }

    // DirectUI requests the icon at a DPI-relative size (e.g. 24rp); fall back
    // to the classic 24x24 base size if the caller didn't specify one.
    int wantWidth  = (targetWidth  > 0) ? targetWidth  : 24;
    int wantHeight = (targetHeight > 0) ? targetHeight : 24;

    if (!*source || *cachedWidth != wantWidth || *cachedHeight != wantHeight) {
        if (*source) {
            DestroyIcon(*source);
            *source = NULL;
        }
        *source = CreateIconFromBase64PNG(base64, wantWidth, wantHeight);
        *cachedWidth = wantWidth;
        *cachedHeight = wantHeight;
    }
    return *source ? CopyIcon(*source) : NULL;
}

static void DrawTextWithWrap(HDC hdc, LPCWSTR text, int x, int y, int maxWidth, int lineHeight) {
    if (!text || text[0] == L'\0') return;
    int totalLen = lstrlenW(text);
    if (totalLen == 0) return;
    SIZE size;
    GetTextExtentPoint32W(hdc, text, totalLen, &size);
    if (size.cx <= maxWidth) {
        TextOutW(hdc, x, y, text, totalLen);
        return;
    }
    WCHAR buffer[256];

    // Guard against buffer overflow
    if (totalLen > 255) totalLen = 255;

    int lineStart = 0;
    int currentY = y;
    while (lineStart < totalLen) {
        int lineLen = 0;
        int lastGoodBreak = 0;
        for (int i = 0; lineStart + i < totalLen; i++) {
            buffer[i] = text[lineStart + i];
            buffer[i + 1] = L'\0';
            GetTextExtentPoint32W(hdc, buffer, i + 1, &size);
            if (text[lineStart + i] == L' ') {
                lastGoodBreak = i;
            }
            if (size.cx > maxWidth) {
                if (lastGoodBreak > 0) {
                    lineLen = lastGoodBreak;
                } else {
                    lineLen = i;
                }
                break;
            }
            lineLen = i + 1;
        }
        TextOutW(hdc, x, currentY, text + lineStart, lineLen);
        while (lineStart + lineLen < totalLen && text[lineStart + lineLen] == L' ') {
            lineLen++;
        }
        lineStart += lineLen;
        currentY += lineHeight;
        if (currentY > y + lineHeight * 5) break;
    }
}

// -------------------------------------------------------
// Internet check
// -------------------------------------------------------
BOOL IsInternetConnected() {
    // Create a local NLM instance instead of sharing g_pNLM, so that
    // callers from the Control Panel's DUI thread (NetworkMapVisual,
    // LoadImageW_Hook) don't cross apartments with the hotkey thread's
    // g_pNLM or race with its Release in HotkeyThreadProc exit.
    INetworkListManager* pNLM = NULL;
    if (FAILED(CoCreateInstance(CLSID_NetworkListManager, NULL, CLSCTX_INPROC_SERVER,
                                IID_INetworkListManager, (void**)&pNLM)) || !pNLM)
        return FALSE;
    NLM_CONNECTIVITY connectivity;
    HRESULT hr = pNLM->GetConnectivity(&connectivity);
    pNLM->Release();
    if (FAILED(hr)) return FALSE;
    return (connectivity & NLM_CONNECTIVITY_IPV4_INTERNET) ||
           (connectivity & NLM_CONNECTIVITY_IPV6_INTERNET);
}



// -------------------------------------------------------
// Network Location Category Detection
// Detects whether the active network is Home, Public, or Work.
// Robustness goals:
//  1) Never let an unrelated connected/VPN/domain network override the icon.
//  2) Prefer the exact adapter used by the flyout header (Ethernet first,
//     otherwise the connected Wi-Fi interface).
//  3) Fall back to the NetworkList registry profile name, then to a safe
//     public-first scan of connected NLM networks.
// -------------------------------------------------------
static BOOL IsVirtualOrNonEthernetAdapter(LPCWSTR desc, LPCWSTR name);
static void SafeSysFreeString(BSTR bstr);
static BOOL IsZeroGuidValue(const GUID* guid);

// Fetched once per fallback scan (rather than once per connection examined)
// to avoid a repeated 15 KB alloc + GetAdaptersAddresses enumeration.
struct AdapterIgnoreTable {
    PIP_ADAPTER_ADDRESSES pAddresses;
};

static BOOL AdapterIgnoreTable_Init(AdapterIgnoreTable* table) {
    table->pAddresses = NULL;
    ULONG outBufLen = 15000;
    PIP_ADAPTER_ADDRESSES pAddresses = (PIP_ADAPTER_ADDRESSES)malloc(outBufLen);
    if (!pAddresses) return FALSE;

    ULONG res = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
                                     NULL, pAddresses, &outBufLen);
    if (res == ERROR_BUFFER_OVERFLOW) {
        free(pAddresses);
        pAddresses = (PIP_ADAPTER_ADDRESSES)malloc(outBufLen);
        if (!pAddresses) return FALSE;
        res = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
                                   NULL, pAddresses, &outBufLen);
    }
    if (res != NO_ERROR) {
        free(pAddresses);
        return FALSE;
    }
    table->pAddresses = pAddresses;
    return TRUE;
}

static void AdapterIgnoreTable_Free(AdapterIgnoreTable* table) {
    if (table && table->pAddresses) {
        free(table->pAddresses);
        table->pAddresses = NULL;
    }
}

static BOOL AdapterIgnoreTable_IsIgnored(const AdapterIgnoreTable* table, const GUID* adapterGuid) {
    if (!table || !table->pAddresses || IsZeroGuidValue(adapterGuid)) return FALSE;
    for (PIP_ADAPTER_ADDRESSES pCurr = table->pAddresses; pCurr != NULL; pCurr = pCurr->Next) {
        GUID parsedAdapterGuid = {0};
        BOOL haveParsedGuid = FALSE;
        if (pCurr->AdapterName && pCurr->AdapterName[0] != '\0') {
            WCHAR wAdapterName[128];
            int convRes = MultiByteToWideChar(CP_ACP, 0, pCurr->AdapterName, -1,
                                               wAdapterName, ARRAYSIZE(wAdapterName));
            if (convRes > 0 && SUCCEEDED(IIDFromString(wAdapterName, &parsedAdapterGuid))) {
                haveParsedGuid = TRUE;
            }
        }
        if (haveParsedGuid && IsEqualGUID(parsedAdapterGuid, *adapterGuid)) {
            return IsVirtualOrNonEthernetAdapter(pCurr->Description, pCurr->FriendlyName);
        }
    }
    return FALSE;
}

static BOOL IsZeroGuidValue(const GUID* guid) {
    static const GUID zeroGuid = {0};
    return !guid || IsEqualGUID(*guid, zeroGuid);
}

static BOOL IsValidNetworkCategoryValue(int category) {
    return category == (int)NLM_NETWORK_CATEGORY_PUBLIC ||
           category == (int)NLM_NETWORK_CATEGORY_PRIVATE ||
           category == (int)NLM_NETWORK_CATEGORY_DOMAIN_AUTHENTICATED;
}

static BOOL ConnectivityIsActive(NLM_CONNECTIVITY connectivity) {
    return connectivity != NLM_CONNECTIVITY_DISCONNECTED;
}

static int StabilizeNetworkCategoryResult(int detectedCategory) {
    DWORD now = GetTickCount();
    if (IsValidNetworkCategoryValue(detectedCategory)) {
        g_LastReliableNetworkCategory = detectedCategory;
        g_LastReliableNetworkCategoryTick = now;
        return detectedCategory;
    }

    // Avoid a one-refresh flicker to the generic icon if NLM/registry is
    // temporarily unavailable while the network stack is settling.
    if (IsValidNetworkCategoryValue(g_LastReliableNetworkCategory) &&
        now - g_LastReliableNetworkCategoryTick < 30000) {
        return g_LastReliableNetworkCategory;
    }
    return -1;
}

static BOOL TryGetCategoryForAdapter(INetworkListManager* pNLM,
                                     const GUID* adapterGuid,
                                     int* outCategory,
                                     LPCWSTR reason) {
    if (!pNLM || IsZeroGuidValue(adapterGuid) || !outCategory) return FALSE;

    ComPtr<IEnumNetworks> networks;
    if (FAILED(pNLM->GetNetworks(NLM_ENUM_NETWORK_CONNECTED, networks.put())) || !networks)
        return FALSE;

    BOOL found = FALSE;
    ULONG fetched = 0;
    ComPtr<INetwork> network;
    while (!found && networks->Next(1, network.put(), &fetched) == S_OK && network) {
        NLM_NETWORK_CATEGORY category;
        if (SUCCEEDED(network->GetCategory(&category)) &&
            IsValidNetworkCategoryValue((int)category)) {
            ComPtr<IEnumNetworkConnections> connections;
            if (SUCCEEDED(network->GetNetworkConnections(connections.put())) && connections) {
                ULONG fetchedConn = 0;
                ComPtr<INetworkConnection> connection;
                while (!found && connections->Next(1, connection.put(), &fetchedConn) == S_OK && connection) {
                    GUID connAdapterId = {0};
                    if (SUCCEEDED(connection->GetAdapterId(&connAdapterId)) &&
                        IsEqualGUID(connAdapterId, *adapterGuid)) {
                        NLM_CONNECTIVITY connConnectivity = NLM_CONNECTIVITY_DISCONNECTED;
                        HRESULT hrConn = connection->GetConnectivity(&connConnectivity);
                        if (FAILED(hrConn) || ConnectivityIsActive(connConnectivity)) {
                            *outCategory = (int)category;
                            found = TRUE;
                            Wh_Log(L"Network category exact adapter match (%s): %d",
                                   reason ? reason : L"unknown", *outCategory);
                        }
                    }
                }
            }
        }
    }
    return found;
}

// Robust join that removes name matching entirely: given the exact adapter
// GUID used by the flyout header, find its INetwork via the matching
// INetworkConnection::GetAdapterId(), then read that network's own GUID via
// INetwork::GetNetworkId() and look up NetworkList\Profiles\{that GUID}
// directly (the Profiles subkey names ARE the network GUIDs). This yields
// the exact Category for the exact network - profile *names* aren't unique
// (a machine that has seen several networks named "Network"/"Network 2" can
// have stale profiles whose name collides with the current NLM friendly
// name, and the first name match would win), so this avoids that ambiguity
// while keeping the same "registry wins over NLM" property as the name-based
// path below. Checking Ethernet before Wi-Fi here (see call site) also fixes
// the case where the Ethernet name-based registry lookup misses and the
// Wi-Fi lookup would otherwise run first, potentially showing the Wi-Fi
// network's icon even though the header displays the Ethernet connection.
static BOOL TryGetCategoryFromRegistryProfileGuid(const GUID* profileGuid, int* outCategory) {
    if (!profileGuid || IsZeroGuidValue(profileGuid) || !outCategory) return FALSE;

    LPOLESTR guidStr = NULL;
    if (FAILED(StringFromCLSID(*profileGuid, &guidStr)) || !guidStr) return FALSE;

    WCHAR keyPath[256];
    StringCchPrintfW(keyPath, ARRAYSIZE(keyPath),
                     L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NetworkList\\Profiles\\%s",
                     guidStr);
    CoTaskMemFree(guidStr);

    HKEY hProfile = NULL;
    LONG openRes = RegOpenKeyExW(HKEY_LOCAL_MACHINE, keyPath, 0, KEY_READ | KEY_WOW64_64KEY, &hProfile);
    if (openRes != ERROR_SUCCESS) {
        openRes = RegOpenKeyExW(HKEY_LOCAL_MACHINE, keyPath, 0, KEY_READ, &hProfile);
    }
    if (openRes != ERROR_SUCCESS || !hProfile) return FALSE;

    DWORD category = 0;
    DWORD categorySize = sizeof(category);
    DWORD type = 0;
    BOOL found = FALSE;
    if (RegQueryValueExW(hProfile, L"Category", NULL, &type,
                         (LPBYTE)&category, &categorySize) == ERROR_SUCCESS &&
        type == REG_DWORD && IsValidNetworkCategoryValue((int)category)) {
        *outCategory = (int)category;
        found = TRUE;
        Wh_Log(L"Network category registry profile GUID join: %d", *outCategory);
    }
    RegCloseKey(hProfile);
    return found;
}

static BOOL TryGetCategoryByAdapterProfileGuid(INetworkListManager* pNLM,
                                               const GUID* adapterGuid,
                                               int* outCategory,
                                               LPCWSTR reason) {
    if (!pNLM || IsZeroGuidValue(adapterGuid) || !outCategory) return FALSE;

    IEnumNetworks* pEnum = NULL;
    if (FAILED(pNLM->GetNetworks(NLM_ENUM_NETWORK_CONNECTED, &pEnum)) || !pEnum)
        return FALSE;

    BOOL found = FALSE;
    INetwork* pNet = NULL;
    ULONG fetched = 0;
    while (!found && pEnum->Next(1, &pNet, &fetched) == S_OK && pNet) {
        BOOL adapterMatched = FALSE;
        IEnumNetworkConnections* pEnumConn = NULL;
        if (SUCCEEDED(pNet->GetNetworkConnections(&pEnumConn)) && pEnumConn) {
            INetworkConnection* pConn = NULL;
            ULONG fetchedConn = 0;
            while (!adapterMatched && pEnumConn->Next(1, &pConn, &fetchedConn) == S_OK && pConn) {
                GUID connAdapterId = {0};
                if (SUCCEEDED(pConn->GetAdapterId(&connAdapterId)) &&
                    IsEqualGUID(connAdapterId, *adapterGuid)) {
                    adapterMatched = TRUE;
                }
                pConn->Release();
            }
            pEnumConn->Release();
        }

        if (adapterMatched) {
            GUID networkId = {0};
            if (SUCCEEDED(pNet->GetNetworkId(&networkId)) &&
                TryGetCategoryFromRegistryProfileGuid(&networkId, outCategory)) {
                found = TRUE;
                Wh_Log(L"Network category exact GUID join (%s): %d",
                       reason ? reason : L"unknown", *outCategory);
            }
        }
        pNet->Release();
    }
    pEnum->Release();
    return found;
}

static BOOL TryGetCategoryFromRegistryProfileName(LPCWSTR profileName, int* outCategory) {
    if (!profileName || profileName[0] == L'\0' || !outCategory) return FALSE;

    HKEY hProfiles = NULL;
    LONG openRes = RegOpenKeyExW(
        HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NetworkList\\Profiles",
        0, KEY_READ | KEY_WOW64_64KEY, &hProfiles);
    if (openRes != ERROR_SUCCESS) {
        openRes = RegOpenKeyExW(
            HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NetworkList\\Profiles",
            0, KEY_READ, &hProfiles);
    }
    if (openRes != ERROR_SUCCESS || !hProfiles) return FALSE;

    BOOL found = FALSE;
    for (DWORD index = 0; !found; index++) {
        WCHAR subKeyName[128];
        DWORD subKeyLen = ARRAYSIZE(subKeyName);
        if (RegEnumKeyExW(hProfiles, index, subKeyName, &subKeyLen, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
            break;

        HKEY hProfile = NULL;
        if (RegOpenKeyExW(hProfiles, subKeyName, 0, KEY_READ, &hProfile) != ERROR_SUCCESS)
            continue;

        WCHAR storedName[256];
        DWORD storedNameSize = sizeof(storedName);
        DWORD type = 0;
        // RegGetValueW (unlike RegQueryValueExW) guarantees the returned
        // REG_SZ buffer is null-terminated, which _wcsicmp below relies on.
        if (RegGetValueW(hProfile, NULL, L"ProfileName",
                        RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND, &type,
                        (LPBYTE)storedName, &storedNameSize) == ERROR_SUCCESS &&
            _wcsicmp(storedName, profileName) == 0) {
            DWORD category = 0;
            DWORD categorySize = sizeof(category);
            type = 0;
            if (RegQueryValueExW(hProfile, L"Category", NULL, &type,
                                 (LPBYTE)&category, &categorySize) == ERROR_SUCCESS &&
                type == REG_DWORD && IsValidNetworkCategoryValue((int)category)) {
                *outCategory = (int)category;
                found = TRUE;
                Wh_Log(L"Network category registry profile match '%s': %d", profileName, *outCategory);
            }
        }
        RegCloseKey(hProfile);
    }

    RegCloseKey(hProfiles);
    return found;
}

static BOOL TryGetCategoryByNlmName(INetworkListManager* pNLM, LPCWSTR networkName, int* outCategory) {
    if (!pNLM || !networkName || networkName[0] == L'\0' || !outCategory) return FALSE;

    IEnumNetworks* pEnum = NULL;
    if (FAILED(pNLM->GetNetworks(NLM_ENUM_NETWORK_CONNECTED, &pEnum)) || !pEnum)
        return FALSE;

    BOOL found = FALSE;
    INetwork* pNet = NULL;
    ULONG fetched = 0;
    while (!found && pEnum->Next(1, &pNet, &fetched) == S_OK && pNet) {
        BSTR bstrName = NULL;
        if (SUCCEEDED(pNet->GetName(&bstrName)) && bstrName) {
            if (_wcsicmp(bstrName, networkName) == 0) {
                NLM_NETWORK_CATEGORY category;
                if (SUCCEEDED(pNet->GetCategory(&category)) &&
                    IsValidNetworkCategoryValue((int)category)) {
                    *outCategory = (int)category;
                    found = TRUE;
                    Wh_Log(L"Network category NLM name match '%s': %d", networkName, *outCategory);
                }
            }
            SafeSysFreeString(bstrName);
        }
        pNet->Release();
    }
    pEnum->Release();
    return found;
}

static BOOL NetworkHasUsableNonIgnoredConnection(INetwork* pNet, const AdapterIgnoreTable* ignoreTable) {
    if (!pNet) return FALSE;

    IEnumNetworkConnections* pEnumConn = NULL;
    if (FAILED(pNet->GetNetworkConnections(&pEnumConn)) || !pEnumConn) {
        // If NLM doesn't expose connections, don't discard the network.
        return TRUE;
    }

    BOOL hasUsable = FALSE;
    INetworkConnection* pConn = NULL;
    ULONG fetchedConn = 0;
    while (!hasUsable && pEnumConn->Next(1, &pConn, &fetchedConn) == S_OK && pConn) {
        NLM_CONNECTIVITY connConnectivity = NLM_CONNECTIVITY_DISCONNECTED;
        HRESULT hrConn = pConn->GetConnectivity(&connConnectivity);
        if (FAILED(hrConn) || ConnectivityIsActive(connConnectivity)) {
            GUID connAdapterId = {0};
            if (FAILED(pConn->GetAdapterId(&connAdapterId)) ||
                !AdapterIgnoreTable_IsIgnored(ignoreTable, &connAdapterId)) {
                hasUsable = TRUE;
            }
        }
        pConn->Release();
    }
    pEnumConn->Release();
    return hasUsable;
}

static BOOL TryGetCategoryBySafeFallbackScan(INetworkListManager* pNLM, int* outCategory) {
    if (!pNLM || !outCategory) return FALSE;

    BOOL sawPublic = FALSE;
    BOOL sawPrivate = FALSE;
    BOOL sawDomain = FALSE;
    int firstInternetCategory = -1;
    int firstActiveCategory = -1;

    IEnumNetworks* pEnum = NULL;
    if (FAILED(pNLM->GetNetworks(NLM_ENUM_NETWORK_CONNECTED, &pEnum)) || !pEnum)
        return FALSE;

    // Fetch the adapter table once for this whole scan instead of once per
    // connection examined - GetAdaptersAddresses is a ~15 KB alloc + kernel
    // enumeration each time.
    AdapterIgnoreTable ignoreTable;
    AdapterIgnoreTable_Init(&ignoreTable);

    INetwork* pNet = NULL;
    ULONG fetched = 0;
    while (pEnum->Next(1, &pNet, &fetched) == S_OK && pNet) {
        NLM_NETWORK_CATEGORY category;
        NLM_CONNECTIVITY connectivity = NLM_CONNECTIVITY_DISCONNECTED;
        BOOL usable = NetworkHasUsableNonIgnoredConnection(pNet, &ignoreTable);
        if (usable &&
            SUCCEEDED(pNet->GetCategory(&category)) &&
            IsValidNetworkCategoryValue((int)category) &&
            SUCCEEDED(pNet->GetConnectivity(&connectivity)) &&
            ConnectivityIsActive(connectivity)) {
            int cat = (int)category;
            if (firstActiveCategory == -1) firstActiveCategory = cat;
            if ((connectivity & (NLM_CONNECTIVITY_IPV4_INTERNET | NLM_CONNECTIVITY_IPV6_INTERNET)) &&
                firstInternetCategory == -1) {
                firstInternetCategory = cat;
            }
            if (cat == (int)NLM_NETWORK_CATEGORY_PUBLIC) sawPublic = TRUE;
            else if (cat == (int)NLM_NETWORK_CATEGORY_PRIVATE) sawPrivate = TRUE;
            else if (cat == (int)NLM_NETWORK_CATEGORY_DOMAIN_AUTHENTICATED) sawDomain = TRUE;
        }
        pNet->Release();
    }
    pEnum->Release();
    AdapterIgnoreTable_Free(&ignoreTable);

    // Safe order for fallback: Public must never be overridden by a separate
    // domain/VPN/work network. Exact adapter matching above still allows a real
    // domain-authenticated active adapter to show the Work icon.
    if (sawPublic) *outCategory = (int)NLM_NETWORK_CATEGORY_PUBLIC;
    else if (sawPrivate) *outCategory = (int)NLM_NETWORK_CATEGORY_PRIVATE;
    else if (sawDomain) *outCategory = (int)NLM_NETWORK_CATEGORY_DOMAIN_AUTHENTICATED;
    else if (firstInternetCategory != -1) *outCategory = firstInternetCategory;
    else if (firstActiveCategory != -1) *outCategory = firstActiveCategory;
    else return FALSE;

    Wh_Log(L"Network category safe fallback scan: %d", *outCategory);
    return TRUE;
}

static int DetectNetworkLocationCategory() {
    INetworkListManager* pNLM = g_pNLM;
    if (!pNLM) {
        CoCreateInstance(CLSID_NetworkListManager, NULL, CLSCTX_INPROC_SERVER,
                         IID_INetworkListManager, (void**)&pNLM);
        if (pNLM && !g_pNLM) g_pNLM = pNLM;
    }

    int detected = -1;

    // Exact GUID join first: no name-matching ambiguity, and Ethernet is
    // checked before Wi-Fi to match the header's display priority (fixes the
    // case where a missed Ethernet registry lookup let a Wi-Fi lookup run
    // first and show the wrong network's icon).
    if (pNLM && g_EthernetConnected && g_HasEthernetAdapterGuid &&
        TryGetCategoryByAdapterProfileGuid(pNLM, &g_EthernetAdapterGuid, &detected, L"Ethernet")) {
        return StabilizeNetworkCategoryResult(detected);
    }
    if (pNLM && !g_EthernetConnected) {
        for (int i = 0; i < g_NetworkCount; i++) {
            if (g_NetworkList[i].connState == CONN_STATE_CONNECTED &&
                TryGetCategoryByAdapterProfileGuid(pNLM, &g_NetworkList[i].interfaceGuid, &detected, L"Wi-Fi")) {
                return StabilizeNetworkCategoryResult(detected);
            }
        }
    }

    // The registry profile is the most stable source for the icon Windows shows
    // in Network and Sharing Center. Prefer it before NLM exact-adapter data so
    // a Public profile cannot be overridden by a separate domain/VPN network.
    if (g_EthernetConnected &&
        TryGetCategoryFromRegistryProfileName(g_EthernetNetworkName, &detected)) {
        return StabilizeNetworkCategoryResult(detected);
    }

    for (int i = 0; i < g_NetworkCount; i++) {
        if (g_NetworkList[i].connState == CONN_STATE_CONNECTED &&
            TryGetCategoryFromRegistryProfileName(g_NetworkList[i].ssid, &detected)) {
            return StabilizeNetworkCategoryResult(detected);
        }
    }

    // The header shows Ethernet before Wi-Fi when both are present, so use the
    // same priority for the exact-adapter NLM fallback.
    if (pNLM && g_EthernetConnected && g_HasEthernetAdapterGuid &&
        TryGetCategoryForAdapter(pNLM, &g_EthernetAdapterGuid, &detected, L"Ethernet")) {
        return StabilizeNetworkCategoryResult(detected);
    }

    if (pNLM && !g_EthernetConnected) {
        for (int i = 0; i < g_NetworkCount; i++) {
            if (g_NetworkList[i].connState == CONN_STATE_CONNECTED &&
                TryGetCategoryForAdapter(pNLM, &g_NetworkList[i].interfaceGuid, &detected, L"Wi-Fi")) {
                return StabilizeNetworkCategoryResult(detected);
            }
        }
    }

    if (pNLM && g_EthernetConnected &&
        TryGetCategoryByNlmName(pNLM, g_EthernetNetworkName, &detected)) {
        return StabilizeNetworkCategoryResult(detected);
    }

    if (pNLM) {
        for (int i = 0; i < g_NetworkCount; i++) {
            if (g_NetworkList[i].connState == CONN_STATE_CONNECTED &&
                TryGetCategoryByNlmName(pNLM, g_NetworkList[i].ssid, &detected)) {
                return StabilizeNetworkCategoryResult(detected);
            }
        }

        if (TryGetCategoryBySafeFallbackScan(pNLM, &detected)) {
            return StabilizeNetworkCategoryResult(detected);
        }
    }

    Wh_Log(L"Network category detection failed; using stable fallback if available");
    return StabilizeNetworkCategoryResult(-1);
}

// Get the appropriate icon for the current network location.
// If the base64-decoded icon for the detected category failed to load,
// falls back to g_hIconNetworkMap (the generic PC/computer icon).
// The corresponding GDI+ bitmap cache pointer is stored in *pppOutCache
// so the caller can pass it to DrawIconBicubic for high-quality scaling.
static HICON GetNetworkLocationIcon(void*** pppOutCache) {
    // Lazily create icons from base64 (scaled to current DPI)
    if (!g_hIconNetLocHome)
        g_hIconNetLocHome = CreateIconFromBase64PNG(NETLOC_HOME_ICON_BASE64, ScaleDpi(35), ScaleDpi(35));
    if (!g_hIconNetLocPublic)
        g_hIconNetLocPublic = CreateIconFromBase64PNG(NETLOC_PUBLIC_ICON_BASE64, ScaleDpi(35), ScaleDpi(35));
    if (!g_hIconNetLocWork)
        g_hIconNetLocWork = CreateIconFromBase64PNG(NETLOC_WORK_ICON_BASE64, ScaleDpi(35), ScaleDpi(35));
    
    HICON hIcon = NULL;
    void** ppCache = NULL;
    
    // Use the stable reliable category when the current one is momentarily
    // unknown (e.g. during a reconnect).  This keeps the correct Home/Public/Work
    // icon on screen instead of briefly flashing the generic PC icon.
    int effectiveCategory = g_CurrentNetworkCategory;
    if (!IsValidNetworkCategoryValue(effectiveCategory) &&
        IsValidNetworkCategoryValue(g_LastReliableNetworkCategory)) {
        DWORD now = GetTickCount();
        if (now - g_LastReliableNetworkCategoryTick < 30000)
            effectiveCategory = g_LastReliableNetworkCategory;
    }
    
    switch (effectiveCategory) {
        case (int)NLM_NETWORK_CATEGORY_PRIVATE:
            hIcon   = g_hIconNetLocHome;
            ppCache = &g_pBitmapNetLocHome;
            break;
        case (int)NLM_NETWORK_CATEGORY_PUBLIC:
            hIcon   = g_hIconNetLocPublic;
            ppCache = &g_pBitmapNetLocPublic;
            break;
        case (int)NLM_NETWORK_CATEGORY_DOMAIN_AUTHENTICATED:
            hIcon   = g_hIconNetLocWork;
            ppCache = &g_pBitmapNetLocWork;
            break;
        default:
            break;
    }
    
    // During the short interval after a new connection is reported but before
    // NLM/the registry expose its category, never flash the generic PC icon.
    // Public is Windows' safe default profile and provides a location icon
    // until the authoritative Home/Public/Work category replaces it.
    if (!hIcon && g_hIconNetLocPublic) {
        hIcon   = g_hIconNetLocPublic;
        ppCache = &g_pBitmapNetLocPublic;
    }
    // Defensive last resort only if decoding the location artwork itself failed.
    if (!hIcon && g_hIconNetworkMap) {
        hIcon   = g_hIconNetworkMap;
        ppCache = &g_pBitmapNetworkMap;
    }
    
    if (pppOutCache) *pppOutCache = ppCache;
    return hIcon;
}

// DirectUI draws the active-network icon itself. Decode the PNG at the exact
// size it requests, using CreateIconFromBase64PNG's HighQualityBicubic (mode
// 7) path, instead of passing it the 35rp flyout icon and letting it upscale.
static HICON CopyNetworkLocationIconForDUI(int targetWidth, int targetHeight) {
    // With no connected network, show a disabled gray public/bench icon.
    bool hasConnectedNetwork = false;
    for (int i = 0; i < g_NetworkCount; ++i)
        if (g_NetworkList[i].connState == CONN_STATE_CONNECTED) { hasConnectedNetwork = true; break; }
    if (!hasConnectedNetwork && !g_EthernetConnected) {
        int wantW = targetWidth > 0 ? targetWidth : ScaleDpi(36);
        int wantH = targetHeight > 0 ? targetHeight : ScaleDpi(36);
        return CreateIconFromBase64PNG(NETLOC_PUBLIC_OFFLINE_ICON_BASE64, wantW, wantH);
    }
    int category = g_CurrentNetworkCategory;
    if (!IsValidNetworkCategoryValue(category) &&
        IsValidNetworkCategoryValue(g_LastReliableNetworkCategory)) {
        DWORD now = GetTickCount();
        if (now - g_LastReliableNetworkCategoryTick < 30000)
            category = g_LastReliableNetworkCategory;
    }

    const WCHAR* png = NULL;
    switch (category) {
        case (int)NLM_NETWORK_CATEGORY_PRIVATE:              png = NETLOC_HOME_ICON_BASE64;   break;
        case (int)NLM_NETWORK_CATEGORY_PUBLIC:               png = NETLOC_PUBLIC_ICON_BASE64; break;
        case (int)NLM_NETWORK_CATEGORY_DOMAIN_AUTHENTICATED: png = NETLOC_WORK_ICON_BASE64;   break;
        default:                                             return NULL;
    }

    int wantW = targetWidth  > 0 ? targetWidth  : ScaleDpi(48);
    int wantH = targetHeight > 0 ? targetHeight : ScaleDpi(48);
    if (!g_hIconNetLocDUI || g_iconNetLocDUIW != wantW ||
        g_iconNetLocDUIH != wantH || g_iconNetLocDUICategory != category) {
        if (g_hIconNetLocDUI) {
            DestroyIcon(g_hIconNetLocDUI);
            g_hIconNetLocDUI = NULL;
        }
        g_hIconNetLocDUI = CreateIconFromBase64PNG(png, wantW, wantH);
        g_iconNetLocDUIW = wantW;
        g_iconNetLocDUIH = wantH;
        g_iconNetLocDUICategory = category;
    }
    return g_hIconNetLocDUI ? CopyIcon(g_hIconNetLocDUI) : NULL;
}

void SetKeyboardFocus(int index) {
    if (index < -1 || index >= g_NetworkCount) return;
    ClearKeyboardFocus();
    g_KeyboardSelectedIndex = index;
    if (index >= 0 && g_bListExpanded) {
        g_SelectedRowIndex = index;
        UpdateLayoutGeometry();
    }
    if (SafeToAccessUI() && g_hWndFlyout)
        InvalidateRect(g_hWndFlyout, NULL, TRUE);
}

void ClearKeyboardFocus() {
    g_KeyboardSelectedIndex = -1;
}

void DrawFocusRectangle(HDC hdc, const RECT* rcRow) {
    RECT rcFocus = *rcRow;
    rcFocus.left += 8;
    rcFocus.right -= 8;
    rcFocus.top += 2;
    rcFocus.bottom -= 2;
    HPEN hPen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, rcFocus.left, rcFocus.top, rcFocus.right, rcFocus.bottom);
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
}

void InitRefreshButtonRect(void) {
    // Keep hit-testing in lockstep with the paint-path geometry below.
    const int buttonWidth = ScaleDpi(21);
    const int totalListHeight = GetTotalListHeight();
    const int availableHeight = LIST_Y_END - LIST_Y_START;
    const BOOL hasScrollbar = (totalListHeight > availableHeight);
    const int scrollbarOffset = hasScrollbar ? ScaleDpi(13) : 0;
    const int roundedCornersOffset = g_Settings.useRoundedCorners ? (WINDOW_WIDTH * 2) / 100 : 0;
    // With a scrollbar, apply the requested compensation, 1% left of the prior 4% position.
    const int scrollbarShift = hasScrollbar ? ((WINDOW_WIDTH * 3) / 100)
                                             : (((WINDOW_WIDTH * 4) / 100) - ((WINDOW_WIDTH * 13) / 1000));
    const int refreshLeftOffset = GetRefreshButtonLeftOffset();
    int right = WINDOW_WIDTH - ScaleDpi(19) - scrollbarOffset - roundedCornersOffset +
                scrollbarShift - refreshLeftOffset;
    if (right > WINDOW_WIDTH)
        right = WINDOW_WIDTH;

    g_rcRefreshButton.left   = right - buttonWidth;
    g_rcRefreshButton.top    = ScaleDpi(2);
    g_rcRefreshButton.right  = right;
    g_rcRefreshButton.bottom = ScaleDpi(24);
}

// -------------------------------------------------------
// SSID display helper
// -------------------------------------------------------
static void GetDisplaySSID(int index, WCHAR* buf, int bufLen) {
    if (g_Settings.privacyMode) {
        StringCchPrintfW(buf, bufLen, LOC(STR_NETWORK_PRIVACY_FMT), index + 1);
        return;
    }
    int suffix = g_NetworkList[index].displaySuffix;
    if (suffix >= 2) {
        StringCchPrintfW(buf, bufLen, L"%s %d", g_NetworkList[index].ssid, suffix);
    } else {
        StringCchCopyW(buf, bufLen, g_NetworkList[index].ssid);
    }
}

// -------------------------------------------------------
// Icons and resources
// -------------------------------------------------------
void LoadSystemIcons() {
    if (!g_hIconNetworkMap)
        g_hIconNetworkMap = CreateIconFromBase64PNG(PC_ICON_BASE64, ScaleDpi(48), ScaleDpi(48));

    // Build an absolute System32 path instead of relying on the default
    // DLL search order for bare file names. The mod only ever injects into
    // explorer.exe/control.exe (both launched from protected system
    // directories), so this was already low-risk, but an absolute path is
    // tidier and removes any doubt.
    WCHAR sysDir[MAX_PATH];
    UINT sysDirLen = GetSystemDirectoryW(sysDir, ARRAYSIZE(sysDir));
    WCHAR netshellPath[MAX_PATH] = {0};
    WCHAR shell32Path[MAX_PATH] = {0};
    if (sysDirLen > 0 && sysDirLen < ARRAYSIZE(sysDir)) {
        StringCchCopyW(netshellPath, ARRAYSIZE(netshellPath), sysDir);
        StringCchCatW(netshellPath, ARRAYSIZE(netshellPath), L"\\netshell.dll");
        StringCchCopyW(shell32Path, ARRAYSIZE(shell32Path), sysDir);
        StringCchCatW(shell32Path, ARRAYSIZE(shell32Path), L"\\shell32.dll");
    }
    for (int i = 0; i < 6; i++)
        if (!g_hIconSignalBars[i])
            ExtractIconExW(netshellPath[0] ? netshellPath : L"netshell.dll",
                           152 + i, &g_hIconSignalBars[i], NULL, 1);
    if (!g_hIconRefreshWin7)
        ExtractIconExW(shell32Path[0] ? shell32Path : L"shell32.dll",
                       238, &g_hIconRefreshWin7, NULL, 1);
    if (!g_hIconGlobe)
        g_hIconGlobe = CreateIconFromBase64PNG(GLOBE_ICON_BASE64, ScaleDpi(48), ScaleDpi(48));
    // Keep this cache at the exact header render size. Unlike the 48px map
    // artwork it is never resampled by DrawIconEx at normal DPI.
    if (!g_hIconDisconnected)
        g_hIconDisconnected = CreateIconFromBase64PNG(DISCONNECTED_ICON_BASE64,
                                                       ScaleDpi(35), ScaleDpi(35));
    if (!g_hIconAvailable)
        g_hIconAvailable = CreateIconFromBase64PNG(AVAILABLE_ICON_BASE64,
                                                    ScaleDpi(36), ScaleDpi(36));
}

static BOOL InitGdiPlusRendering() {
    if (g_hGdiPlus) return TRUE;
    g_hGdiPlus = LoadLibraryExW(L"gdiplus.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_hGdiPlus) {
        Wh_Log(L"GDI+: failed to load gdiplus.dll");
        return FALSE;
    }
    pGdipCreateBitmapFromHICON = (GdipCreateBitmapFromHICONFunc)GetProcAddress(g_hGdiPlus, "GdipCreateBitmapFromHICON");
    pGdipSetInterpolationMode = (GdipSetInterpolationModeFunc)GetProcAddress(g_hGdiPlus, "GdipSetInterpolationMode");
    pGdipDrawImageRectI = (GdipDrawImageRectIFunc)GetProcAddress(g_hGdiPlus, "GdipDrawImageRectI");
    pGdipDeleteGraphics = (GdipDeleteGraphicsFunc)GetProcAddress(g_hGdiPlus, "GdipDeleteGraphics");
    pGdipCreateBitmapFromScan0 = (GdipCreateBitmapFromScan0Func)GetProcAddress(g_hGdiPlus, "GdipCreateBitmapFromScan0");
    pGdipGetImageGraphicsContext = (GdipGetImageGraphicsContextFunc)GetProcAddress(g_hGdiPlus, "GdipGetImageGraphicsContext");
    pGdipSetPixelOffsetMode = (GdipSetPixelOffsetModeFunc)GetProcAddress(g_hGdiPlus, "GdipSetPixelOffsetMode");
    pGdipGraphicsClear = (GdipGraphicsClearFunc)GetProcAddress(g_hGdiPlus, "GdipGraphicsClear");
    pGdipCreateHBITMAPFromBitmap = (GdipCreateHBITMAPFromBitmapFunc)GetProcAddress(g_hGdiPlus, "GdipCreateHBITMAPFromBitmap");
    pGdipDisposeImage = (GdipDisposeImageFunc)GetProcAddress(g_hGdiPlus, "GdipDisposeImage");
    pGdipCreateBitmapFromStream = (GdipCreateBitmapFromStreamFunc)GetProcAddress(g_hGdiPlus, "GdipCreateBitmapFromStream");
    pGdipCreateHICONFromBitmap = (GdipCreateHICONFromBitmapFunc)GetProcAddress(g_hGdiPlus, "GdipCreateHICONFromBitmap");
    if (!pGdipCreateBitmapFromHICON || !pGdipSetInterpolationMode || !pGdipDrawImageRectI ||
        !pGdipDeleteGraphics || !pGdipCreateBitmapFromScan0 || !pGdipGetImageGraphicsContext ||
        !pGdipSetPixelOffsetMode || !pGdipGraphicsClear || !pGdipCreateHBITMAPFromBitmap ||
        !pGdipDisposeImage || !pGdipCreateBitmapFromStream || !pGdipCreateHICONFromBitmap) {
        Wh_Log(L"GDI+: missing function pointers");
        FreeLibrary(g_hGdiPlus);
        g_hGdiPlus = NULL;
        return FALSE;
    }
    typedef int (WINAPI *GdiplusStartupFunc)(ULONG_PTR*, const void*, void*);
    GdiplusStartupFunc pStartup = (GdiplusStartupFunc)GetProcAddress(g_hGdiPlus, "GdiplusStartup");
    if (!pStartup) {
        FreeLibrary(g_hGdiPlus);
        g_hGdiPlus = NULL;
        return FALSE;
    }
    struct { DWORD Version; void* Callback; BOOL Suppress; } si = {1, NULL, FALSE};
    if (pStartup(&g_gdiplusToken, &si, NULL) != 0) {
        Wh_Log(L"GDI+: GdiplusStartup failed");
        FreeLibrary(g_hGdiPlus);
        g_hGdiPlus = NULL;
        return FALSE;
    }
    Wh_Log(L"GDI+: initialized successfully");
    return TRUE;
}

static void ShutdownGdiPlusRendering() {
    for (int i = 0; i < 6; i++) {
        if (g_pBitmapSignalBars[i]) {
            pGdipDisposeImage(g_pBitmapSignalBars[i]);
            g_pBitmapSignalBars[i] = NULL;
        }
    }
    // Free network location icon bitmap caches
    if (g_pBitmapNetLocHome)   { pGdipDisposeImage(g_pBitmapNetLocHome);   g_pBitmapNetLocHome = NULL; }
    if (g_pBitmapNetLocPublic) { pGdipDisposeImage(g_pBitmapNetLocPublic); g_pBitmapNetLocPublic = NULL; }
    if (g_pBitmapNetLocWork)   { pGdipDisposeImage(g_pBitmapNetLocWork);   g_pBitmapNetLocWork = NULL; }
    if (g_pBitmapNetworkMap)   { pGdipDisposeImage(g_pBitmapNetworkMap);   g_pBitmapNetworkMap = NULL; }
    if (g_hGdiPlus) {
        typedef void (WINAPI *GdiplusShutdownFunc)(ULONG_PTR);
        GdiplusShutdownFunc pShutdown = (GdiplusShutdownFunc)GetProcAddress(g_hGdiPlus, "GdiplusShutdown");
        if (pShutdown && g_gdiplusToken) pShutdown(g_gdiplusToken);
        FreeLibrary(g_hGdiPlus);
        g_hGdiPlus = NULL;
        g_gdiplusToken = 0;
    }
}

void FreeSystemIcons() {
    // Free network location icons
    if (g_hIconNetLocHome)   { DestroyIcon(g_hIconNetLocHome);   g_hIconNetLocHome = NULL; }
    if (g_hIconNetLocPublic) { DestroyIcon(g_hIconNetLocPublic); g_hIconNetLocPublic = NULL; }
    if (g_hIconNetLocWork)   { DestroyIcon(g_hIconNetLocWork);   g_hIconNetLocWork = NULL; }
    // Free chevron arrow icons
    if (g_hIconChevronUp)     { DestroyIcon(g_hIconChevronUp);     g_hIconChevronUp = NULL; }
    if (g_hIconChevronUpHL)   { DestroyIcon(g_hIconChevronUpHL);   g_hIconChevronUpHL = NULL; }
    if (g_hIconChevronDown)   { DestroyIcon(g_hIconChevronDown);   g_hIconChevronDown = NULL; }
    if (g_hIconChevronDownHL) { DestroyIcon(g_hIconChevronDownHL); g_hIconChevronDownHL = NULL; }
    g_chevronsLoaded = FALSE;
    if (g_hGdiPlus && pGdipDisposeImage) {
        for (int i = 0; i < 6; i++) {
            if (g_pBitmapSignalBars[i]) {
                pGdipDisposeImage(g_pBitmapSignalBars[i]);
                g_pBitmapSignalBars[i] = NULL;
            }
        }
        // Free network location icon bitmap caches
        if (g_pBitmapNetLocHome)   { pGdipDisposeImage(g_pBitmapNetLocHome);   g_pBitmapNetLocHome = NULL; }
        if (g_pBitmapNetLocPublic) { pGdipDisposeImage(g_pBitmapNetLocPublic); g_pBitmapNetLocPublic = NULL; }
        if (g_pBitmapNetLocWork)   { pGdipDisposeImage(g_pBitmapNetLocWork);   g_pBitmapNetLocWork = NULL; }
        if (g_pBitmapNetworkMap)   { pGdipDisposeImage(g_pBitmapNetworkMap);   g_pBitmapNetworkMap = NULL; }
    }
    if (g_hIconRefreshNormal) { DestroyIcon(g_hIconRefreshNormal); g_hIconRefreshNormal = NULL; }
    if (g_hIconRefreshHover)  { DestroyIcon(g_hIconRefreshHover);  g_hIconRefreshHover = NULL; }
    if (g_hIconNetworkCenterConnect) { DestroyIcon(g_hIconNetworkCenterConnect); g_hIconNetworkCenterConnect = NULL; }
    if (g_hIconNetworkCenterHomegroup) { DestroyIcon(g_hIconNetworkCenterHomegroup); g_hIconNetworkCenterHomegroup = NULL; }
    if (g_hIconNetworkMap) { DestroyIcon(g_hIconNetworkMap); g_hIconNetworkMap = NULL; }
    if (g_hIconGlobe) { DestroyIcon(g_hIconGlobe); g_hIconGlobe = NULL; }
    if (g_hIconDisconnected) { DestroyIcon(g_hIconDisconnected); g_hIconDisconnected = NULL; }
    if (g_hIconAvailable) { DestroyIcon(g_hIconAvailable); g_hIconAvailable = NULL; }
    if (g_hIconNetworkMapDUI) { DestroyIcon(g_hIconNetworkMapDUI); g_hIconNetworkMapDUI = NULL; }
    g_iconNetworkMapDUIW = g_iconNetworkMapDUIH = 0;
    if (g_hIconGlobeDUI) { DestroyIcon(g_hIconGlobeDUI); g_hIconGlobeDUI = NULL; }
    g_iconGlobeDUIW = g_iconGlobeDUIH = 0;
    g_iconGlobeDUIOnline = TRUE;
    if (g_hIconNetLocDUI) { DestroyIcon(g_hIconNetLocDUI); g_hIconNetLocDUI = NULL; }
    g_iconNetLocDUIW = g_iconNetLocDUIH = 0;
    if (g_hIconNoInternetXDUI) { DestroyIcon(g_hIconNoInternetXDUI); g_hIconNoInternetXDUI = NULL; }
    g_iconNoInternetXDUIW = g_iconNoInternetXDUIH = 0;
    if (g_hIconOfflineNetworkDUI) { DestroyIcon(g_hIconOfflineNetworkDUI); g_hIconOfflineNetworkDUI = NULL; }
    g_iconOfflineNetworkDUIW = g_iconOfflineNetworkDUIH = 0;
    g_iconNetLocDUICategory = -1;
    for (int i = 0; i < 6; i++)
        if (g_hIconSignalBars[i]) { DestroyIcon(g_hIconSignalBars[i]); g_hIconSignalBars[i] = NULL; }
    if (g_hIconRefreshWin7) { DestroyIcon(g_hIconRefreshWin7); g_hIconRefreshWin7 = NULL; }
}

void InitGlobalFonts() {
    FreeGlobalFonts(); 
    int sizeNormal = -ScaleDpi(12);
    int sizeSmall  = -ScaleDpi(11);
    g_hFontNormal    = CreateFontW(sizeNormal,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    g_hFontBold      = CreateFontW(sizeNormal,0,0,0,FW_BOLD,  0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    g_hFontUnderline = CreateFontW(sizeNormal,0,0,0,FW_NORMAL,0,1,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    g_hFontButton    = CreateFontW(sizeNormal,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    g_hFontCheckbox  = CreateFontW(sizeSmall,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    g_hFontArrow     = CreateFontW(sizeSmall,0,0,0,FW_NORMAL,0,0,0,SYMBOL_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Marlett");
}

void FreeGlobalFonts() {
    if (g_hFontNormal)    { DeleteObject(g_hFontNormal);    g_hFontNormal    = NULL; }
    if (g_hFontBold)      { DeleteObject(g_hFontBold);      g_hFontBold      = NULL; }
    if (g_hFontUnderline) { DeleteObject(g_hFontUnderline); g_hFontUnderline = NULL; }
    if (g_hFontButton)    { DeleteObject(g_hFontButton);    g_hFontButton    = NULL; }
    if (g_hFontCheckbox)  { DeleteObject(g_hFontCheckbox);  g_hFontCheckbox  = NULL; }
    if (g_hFontArrow)     { DeleteObject(g_hFontArrow);     g_hFontArrow     = NULL; }
}

BOOL SafeToAccessUI() {
    return (g_Ctx.refCount > 0 && !g_Ctx.isUninitializing && g_hWndFlyout && IsWindow(g_hWndFlyout));
}

void PositionWindowNearTray(HWND hwnd) {
    APPBARDATA abd = { sizeof(APPBARDATA) };
    SHAppBarMessage(ABM_GETTASKBARPOS, &abd);
    RECT rcWork;
    // Use the monitor where the taskbar is located for multi-monitor setups
    HMONITOR hMon = MonitorFromWindow(FindWindowW(L"Shell_TrayWnd", NULL), MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (hMon && GetMonitorInfoW(hMon, &mi))
        rcWork = mi.rcWork;
    else
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);
    int x = rcWork.right - WINDOW_WIDTH - 8;
    int y = rcWork.bottom - WINDOW_HEIGHT - 8;
    if (abd.uEdge == ABE_TOP)   y = abd.rc.bottom + 8;
    else if (abd.uEdge == ABE_LEFT)  x = abd.rc.right + 8;
    else if (abd.uEdge == ABE_RIGHT) x = abd.rc.left - WINDOW_WIDTH - 8;
    SetWindowPos(hwnd, HWND_TOPMOST, x, y, WINDOW_WIDTH, WINDOW_HEIGHT, SWP_SHOWWINDOW);
}

// -------------------------------------------------------
// Fallback flags for IP Helper & NLM in case SDK headers lack them
// -------------------------------------------------------
#ifndef GAA_FLAG_SKIP_UNICAST
#define GAA_FLAG_SKIP_UNICAST 0x0001
#endif
#ifndef GAA_FLAG_SKIP_ANYCAST
#define GAA_FLAG_SKIP_ANYCAST 0x0002
#endif
#ifndef GAA_FLAG_SKIP_MULTICAST
#define GAA_FLAG_SKIP_MULTICAST 0x0004
#endif
#ifndef GAA_FLAG_SKIP_DNS_SERVER
#define GAA_FLAG_SKIP_DNS_SERVER 0x0008
#endif
#ifndef IF_TYPE_ETHERNET_CSMACD
#define IF_TYPE_ETHERNET_CSMACD 6
#endif
#ifndef NLM_ENUM_NETWORK_CONNECTED
#define NLM_ENUM_NETWORK_CONNECTED ((NLM_ENUM_NETWORK)1)
#endif

// -------------------------------------------------------
// WLAN data refresh
// -------------------------------------------------------
static GUID g_WlanInterfaceGuids[16];
static int  g_WlanInterfaceCount = 0;

void RefreshWifiData(HANDLE hClient) {
    if (!hClient) return;
    static DWORD lastValidRefresh = 0;
    DWORD now = GetTickCount();
    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
    if (WlanEnumInterfaces(hClient, NULL, &pIfList) != ERROR_SUCCESS) return;
    
    g_WlanInterfaceCount = 0;
    if (pIfList) {
        for (DWORD i = 0; i < pIfList->dwNumberOfItems && g_WlanInterfaceCount < 16; i++) {
            g_WlanInterfaceGuids[g_WlanInterfaceCount++] = pIfList->InterfaceInfo[i].InterfaceGuid;
        }
    }
    WifiNetworkItem tempList[50];
    int tempCount = 0;
    ZeroMemory(tempList, sizeof(tempList));
    for (DWORD i = 0; pIfList && i < pIfList->dwNumberOfItems; i++) {
        WLAN_INTERFACE_INFO IfInfo = pIfList->InterfaceInfo[i];
        PWLAN_AVAILABLE_NETWORK_LIST pBssList  = NULL;
        PWLAN_PROFILE_INFO_LIST      pProfList = NULL;
        WlanGetProfileList(hClient, &IfInfo.InterfaceGuid, NULL, &pProfList);
        if (WlanGetAvailableNetworkList(hClient, &IfInfo.InterfaceGuid,
                WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_MANUAL_HIDDEN_PROFILES,
                NULL, &pBssList) == ERROR_SUCCESS) {
            for (DWORD j = 0; j < pBssList->dwNumberOfItems && tempCount < 50; j++) {
                WLAN_AVAILABLE_NETWORK network = pBssList->Network[j];
                size_t len = (size_t)network.dot11Ssid.uSSIDLength;
                
                if (len == 0) {
                    StringCchCopyW(tempList[tempCount].ssid, 33, L"Hidden Network");
                } else {
                    BYTE cleanSsid[33] = {0};
                    size_t cleanLen = (len < 32u) ? len : 32u;
                    for (size_t k = 0; k < cleanLen; k++)
                        cleanSsid[k] = (network.dot11Ssid.ucSSID[k] == 0) ? (BYTE)' ' : network.dot11Ssid.ucSSID[k];
                    cleanSsid[cleanLen] = 0;
                    int converted = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)cleanSsid, (int)cleanLen, tempList[tempCount].ssid, 32);
                    if (converted <= 0) {
                        for (size_t k = 0; k < cleanLen; k++)
                            tempList[tempCount].ssid[k] = (WCHAR)cleanSsid[k];
                        converted = (int)cleanLen;
                    }
                    tempList[tempCount].ssid[converted] = L'\0';
                }
                BOOL duplicate = FALSE;
                int sameSsidVariants = 0;
                for (int d = 0; d < tempCount; d++) {
                    BOOL sameSsid = (wcscmp(tempList[d].ssid, tempList[tempCount].ssid) == 0);
                    if (!sameSsid) continue;
                    BOOL sameSecurity =
                        (tempList[d].isSecured == (BOOL)network.bSecurityEnabled) &&
                        (tempList[d].dot11BssType == network.dot11BssType) &&
                        (tempList[d].authAlgorithm == network.dot11DefaultAuthAlgorithm) &&
                        (tempList[d].cipherAlgorithm == network.dot11DefaultCipherAlgorithm);
                    if (sameSecurity) {
                        if (network.wlanSignalQuality > tempList[d].signalQuality)
                            tempList[d].signalQuality = network.wlanSignalQuality;
                        if (network.dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED)
                            tempList[d].connState = CONN_STATE_CONNECTED;
                        duplicate = TRUE;
                        break;
                    }
                    sameSsidVariants++;
                }
                if (duplicate) continue;
                tempList[tempCount].isSecured = network.bSecurityEnabled;
                tempList[tempCount].signalQuality = network.wlanSignalQuality;
                tempList[tempCount].interfaceGuid = IfInfo.InterfaceGuid;
                tempList[tempCount].dot11BssType = network.dot11BssType;
                tempList[tempCount].hasProfile = FALSE;
                tempList[tempCount].hasInternetAccess = FALSE;
                tempList[tempCount].connState = (network.dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED) ? CONN_STATE_CONNECTED : CONN_STATE_IDLE;
                tempList[tempCount].operationStartTime = 0;
                tempList[tempCount].authAlgorithm = network.dot11DefaultAuthAlgorithm;
                tempList[tempCount].cipherAlgorithm = network.dot11DefaultCipherAlgorithm;
                tempList[tempCount].displaySuffix = (sameSsidVariants > 0) ? (sameSsidVariants + 1) : 0;
                tempList[tempCount].hasBssid = FALSE;
                ZeroMemory(tempList[tempCount].bssid, sizeof(tempList[tempCount].bssid));
                {
                    PWLAN_BSS_LIST pBssDetailList = NULL;
                    if (WlanGetNetworkBssList(hClient, &IfInfo.InterfaceGuid,
                            &network.dot11Ssid, network.dot11BssType,
                            network.bSecurityEnabled, NULL, &pBssDetailList) == ERROR_SUCCESS && pBssDetailList) {
                        LONG bestRssi = -32768L;
                        for (DWORD b = 0; b < pBssDetailList->dwNumberOfItems; b++) {
                            const WLAN_BSS_ENTRY& bss = pBssDetailList->wlanBssEntries[b];
                            if (bss.lRssi > bestRssi) {
                                bestRssi = bss.lRssi;
                                CopyMemory(tempList[tempCount].bssid, bss.dot11Bssid, sizeof(DOT11_MAC_ADDRESS));
                                tempList[tempCount].hasBssid = TRUE;
                            }
                        }
                        WlanFreeMemory(pBssDetailList);
                    }
                }
                if (pProfList) {
                    for (DWORD p = 0; p < pProfList->dwNumberOfItems; p++) {
                        if (wcscmp(pProfList->ProfileInfo[p].strProfileName, tempList[tempCount].ssid) != 0)
                            continue;
                        LPWSTR pProfileXml = NULL;
                        DWORD flags = 0;
                        if (WlanGetProfile(hClient, &IfInfo.InterfaceGuid,
                                            pProfList->ProfileInfo[p].strProfileName,
                                            NULL, &pProfileXml, &flags, NULL) == ERROR_SUCCESS) {
                            tempList[tempCount].hasProfile = ProfileSecurityMatches(
                                pProfileXml,
                                tempList[tempCount].authAlgorithm,
                                tempList[tempCount].cipherAlgorithm);
                            WlanFreeMemory(pProfileXml);
                        } else {
                            tempList[tempCount].hasProfile = FALSE;
                        }
                        break;
                    }
                }
                if (tempList[tempCount].connState == CONN_STATE_CONNECTED && tempCount > 0) {
                    WifiNetworkItem tmp;
                    CopyMemory(&tmp, &tempList[0], sizeof(WifiNetworkItem));
                    CopyMemory(&tempList[0], &tempList[tempCount], sizeof(WifiNetworkItem));
                    CopyMemory(&tempList[tempCount], &tmp, sizeof(WifiNetworkItem));
                }
                tempCount++;
            }
            WlanFreeMemory(pBssList);
        }
        if (pProfList) WlanFreeMemory(pProfList);
    }
    WlanFreeMemory(pIfList);
    {
        bool seenConnectedForInterface[64] = {false};
        GUID seenGuids[64];
        int seenCount = 0;
        for (int t = 0; t < tempCount; t++) {
            if (tempList[t].connState != CONN_STATE_CONNECTED) continue;
            int guidIndex = -1;
            for (int g = 0; g < seenCount; g++) {
                if (IsEqualGUID(seenGuids[g], tempList[t].interfaceGuid)) {
                    guidIndex = g;
                    break;
                }
            }
            if (guidIndex == -1 && seenCount < 64) {
                seenGuids[seenCount] = tempList[t].interfaceGuid;
                guidIndex = seenCount;
                seenConnectedForInterface[seenCount] = false;
                seenCount++;
            }
            if (guidIndex >= 0) {
                if (seenConnectedForInterface[guidIndex]) {
                    tempList[t].connState = CONN_STATE_IDLE;
                } else {
                    seenConnectedForInterface[guidIndex] = true;
                }
            }
        }
    }
    EnterCriticalSection(&g_Ctx.csLock);
    if (tempCount > 0 && tempCount <= 50) {
        WCHAR pendingSsid[33] = {0};
        BOOL hadPending = (g_PendingConnectIndex >= 0 && g_PendingConnectIndex < g_NetworkCount);
        if (hadPending) {
            StringCchCopyW(pendingSsid, ARRAYSIZE(pendingSsid), g_NetworkList[g_PendingConnectIndex].ssid);
            Wh_Log(L"RefreshWifiData: preserving pending state - SSID='%s', index=%d, hasProfile=%d, connState=%d", 
                   pendingSsid, g_PendingConnectIndex, 
                   g_NetworkList[g_PendingConnectIndex].hasProfile,
                   g_NetworkList[g_PendingConnectIndex].connState);
        }
        for (int t = 0; t < tempCount; t++) {
            for (int e = 0; e < g_NetworkCount; e++) {
                if (wcscmp(tempList[t].ssid, g_NetworkList[e].ssid) == 0) {
                    if (g_NetworkList[e].connState == CONN_STATE_CONNECTING ||
                        g_NetworkList[e].connState == CONN_STATE_DISCONNECTING ||
                        g_NetworkList[e].connState == CONN_STATE_ERROR) {
                        tempList[t].connState = g_NetworkList[e].connState;
                        tempList[t].operationStartTime = g_NetworkList[e].operationStartTime;
                    }
                    if (g_NetworkList[e].hasProfile) {
                        tempList[t].hasProfile = TRUE;
                    }
                    break;
                }
            }
        }
        CopyMemory(g_NetworkList, tempList, sizeof(WifiNetworkItem) * tempCount);
        g_NetworkCount = tempCount;
        if (hadPending) {
            int newIndex = -1;
            for (int n = 0; n < g_NetworkCount; n++) {
                if (wcscmp(g_NetworkList[n].ssid, pendingSsid) == 0) { newIndex = n; break; }
            }
            if (newIndex >= 0) {
                g_PendingConnectIndex = newIndex;
                Wh_Log(L"RefreshWifiData: updated g_PendingConnectIndex from %d to %d", 
                       (g_PendingConnectIndex >= 0 && g_PendingConnectIndex < g_NetworkCount) ? g_PendingConnectIndex : -1, 
                       newIndex);
            } else {
                Wh_Log(L"RefreshWifiData: pending SSID '%s' no longer in list, clearing g_PendingConnectIndex", pendingSsid);
                g_PendingConnectIndex = -1;
            }
        }
    } else if (tempCount == 0) {
        if (now - lastValidRefresh > 30000) {
            Wh_Log(L"RefreshWifiData: no networks for 30s, clearing all state");
            ZeroMemory(g_NetworkList, sizeof(g_NetworkList));
            g_NetworkCount = 0;
            g_PendingConnectIndex = -1;
        }
    }
    if (tempCount > 0) {
        lastValidRefresh = now;
    }
    // This write belongs inside the lock, like the rest of the list update
    // above: g_NetworkList is shared with the flyout thread, and writing to
    // it after LeaveCriticalSection raced with any concurrent reader.
    if (g_NetworkCount > 0 && g_NetworkList[0].connState == CONN_STATE_CONNECTED) {
        g_NetworkList[0].hasInternetAccess = IsInternetConnected();
    }
    LeaveCriticalSection(&g_Ctx.csLock);
    Wh_Log(L"Refresh complete: %d network(s) found, connected: %s, g_PendingConnectIndex=%d",
           g_NetworkCount,
           (g_NetworkCount > 0 && g_NetworkList[0].connState == CONN_STATE_CONNECTED) 
               ? L"yes" : L"no",
           g_PendingConnectIndex);
}

// -------------------------------------------------------
// Ethernet detection & dynamic window sizing
// -------------------------------------------------------
typedef void (WINAPI *SysFreeStringFunc)(BSTR);
static SysFreeStringFunc pSysFreeString = NULL;

static void SafeSysFreeString(BSTR bstr) {
    if (!bstr) return;
    if (!pSysFreeString) {
        HMODULE hOleAut32 = GetModuleHandleW(L"oleaut32.dll");
        if (!hOleAut32) hOleAut32 = LoadLibraryExW(L"oleaut32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (hOleAut32) {
            pSysFreeString = (SysFreeStringFunc)GetProcAddress(hOleAut32, "SysFreeString");
        }
    }
    if (pSysFreeString) {
        pSysFreeString(bstr);
    }
}

static BOOL ContainsKeywordCI(LPCWSTR str, LPCWSTR keyword) {
    if (!str || !keyword) return FALSE;
    int strLen = lstrlenW(str);
    int kwLen = lstrlenW(keyword);
    if (kwLen > strLen || kwLen == 0) return FALSE;
    for (int i = 0; i <= strLen - kwLen; i++) {
        BOOL match = TRUE;
        for (int j = 0; j < kwLen; j++) {
            if (towlower(str[i + j]) != towlower(keyword[j])) {
                match = FALSE;
                break;
            }
        }
        if (match) return TRUE;
    }
    return FALSE;
}

static BOOL IsVirtualOrNonEthernetAdapter(LPCWSTR desc, LPCWSTR name) {
    if (!desc && !name) return FALSE;
    const WCHAR* ignoreKeywords[] = {
        L"vmware", L"virtualbox", L"virtual", L"hyper-v", L"vethernet",
        L"loopback", L"npcap", L"tap-", L"wsl", L"bluetooth", L"wireguard",
        L"tailscale", L"openvpn", L"warp", L"pseudo", L"miniport", L"wi-fi direct",
        L"wireless", L"wlan", L"wi-fi", L"802.11"
    };
    for (size_t i = 0; i < ARRAYSIZE(ignoreKeywords); i++) {
        if (desc && ContainsKeywordCI(desc, ignoreKeywords[i])) return TRUE;
        if (name && ContainsKeywordCI(name, ignoreKeywords[i])) return TRUE;
    }
    return FALSE;
}

void UpdateEthernetStatus() {
    g_EthernetConnected = FALSE;
    g_EthernetNetworkName[0] = L'\0';
    g_EthernetHasInternet = FALSE;
    ZeroMemory(&g_EthernetAdapterGuid, sizeof(g_EthernetAdapterGuid));
    g_HasEthernetAdapterGuid = FALSE;

    // 1. Find physical operational Ethernet adapter GUID via GetAdaptersAddresses
    BOOL foundPhysicalEthernet = FALSE;
    GUID physicalEthernetGuid = {0};
    BOOL physicalEthernetGuidReliable = FALSE;
    WCHAR fallbackName[64] = L"Ethernet";
    
    ULONG outBufLen = 15000;
    PIP_ADAPTER_ADDRESSES pAddresses = (PIP_ADAPTER_ADDRESSES)malloc(outBufLen);
    if (pAddresses) {
        ULONG res = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER, NULL, pAddresses, &outBufLen);
        if (res == ERROR_BUFFER_OVERFLOW) {
            free(pAddresses);
            pAddresses = (PIP_ADAPTER_ADDRESSES)malloc(outBufLen);
            if (pAddresses) {
                res = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER, NULL, pAddresses, &outBufLen);
            }
        }
        if (res == NO_ERROR && pAddresses) {
            for (PIP_ADAPTER_ADDRESSES pCurr = pAddresses; pCurr != NULL; pCurr = pCurr->Next) {
                if ((pCurr->IfType == IF_TYPE_ETHERNET_CSMACD || pCurr->IfType == 117 || pCurr->IfType == 62) &&
                    pCurr->OperStatus == IfOperStatusUp &&
                    pCurr->FirstUnicastAddress != NULL) {
                    
                    // Check if this adapter GUID belongs to a Wi-Fi card
                    BOOL isWifiGuid = FALSE;
                    for (int w = 0; w < g_WlanInterfaceCount; w++) {
                        if (IsEqualGUID(pCurr->NetworkGuid, g_WlanInterfaceGuids[w])) {
                            isWifiGuid = TRUE;
                            break;
                        }
                    }
                    if (isWifiGuid) continue;
                    
                    // Check if description or friendly name indicates a virtual/Bluetooth/wireless adapter
                    if (IsVirtualOrNonEthernetAdapter(pCurr->Description, pCurr->FriendlyName)) {
                        continue;
                    }
                    
                    // NOTE: pCurr->NetworkGuid is an NLM-internal network identifier and is
                    // NOT the same GUID as INetworkConnection::GetAdapterId(). The adapter's
                    // real device GUID is embedded (as a string, e.g. "{4D36E972-...}") in
                    // pCurr->AdapterName. Parse that instead, or the COM name lookup below
                    // will never match and we'll silently keep the generic fallback name.
                    GUID parsedAdapterGuid = {0};
                    BOOL haveParsedGuid = FALSE;
                    if (pCurr->AdapterName && pCurr->AdapterName[0] != '\0') {
                        WCHAR wAdapterName[128];
                        int convRes = MultiByteToWideChar(CP_ACP, 0, pCurr->AdapterName, -1,
                                                           wAdapterName, ARRAYSIZE(wAdapterName));
                        if (convRes > 0 && SUCCEEDED(IIDFromString(wAdapterName, &parsedAdapterGuid))) {
                            haveParsedGuid = TRUE;
                        }
                    }

                    foundPhysicalEthernet = TRUE;
                    physicalEthernetGuid = haveParsedGuid ? parsedAdapterGuid : pCurr->NetworkGuid;
                    physicalEthernetGuidReliable = haveParsedGuid;
                    if (pCurr->FriendlyName && pCurr->FriendlyName[0] != L'\0') {
                        StringCchCopyW(fallbackName, ARRAYSIZE(fallbackName), pCurr->FriendlyName);
                    }
                    break;
                }
            }
        }
        if (pAddresses) free(pAddresses);
    }

    if (!foundPhysicalEthernet) {
        // No real physical Ethernet cable connected!
        return;
    }

    g_EthernetConnected = TRUE;
    g_EthernetHasInternet = IsInternetConnected();
    if (physicalEthernetGuidReliable && !IsZeroGuidValue(&physicalEthernetGuid)) {
        g_EthernetAdapterGuid = physicalEthernetGuid;
        g_HasEthernetAdapterGuid = TRUE;
    }
    StringCchCopyW(g_EthernetNetworkName, ARRAYSIZE(g_EthernetNetworkName), fallbackName);

    // 2. Query COM INetworkListManager to get the exact friendly network name (e.g. "Rete 2")
    if (!g_pNLM) {
        CoCreateInstance(CLSID_NetworkListManager, NULL, CLSCTX_INPROC_SERVER,
                         IID_INetworkListManager, (void**)&g_pNLM);
    }
    
    if (g_pNLM) {
        IEnumNetworks* pEnum = NULL;
        if (SUCCEEDED(g_pNLM->GetNetworks(NLM_ENUM_NETWORK_CONNECTED, &pEnum)) && pEnum) {
            INetwork* pNet = NULL;
            ULONG fetched = 0;
            BOOL matchedNlm = FALSE;
            while (pEnum->Next(1, &pNet, &fetched) == S_OK && pNet) {
                IEnumNetworkConnections* pEnumConn = NULL;
                if (SUCCEEDED(pNet->GetNetworkConnections(&pEnumConn)) && pEnumConn) {
                    INetworkConnection* pConn = NULL;
                    ULONG fetchedConn = 0;
                    while (pEnumConn->Next(1, &pConn, &fetchedConn) == S_OK && pConn) {
                        GUID connAdapterId = {0};
                        if (SUCCEEDED(pConn->GetAdapterId(&connAdapterId))) {
                            if (IsEqualGUID(connAdapterId, physicalEthernetGuid)) {
                                BSTR bstrName = NULL;
                                if (SUCCEEDED(pNet->GetName(&bstrName)) && bstrName) {
                                    StringCchCopyW(g_EthernetNetworkName, ARRAYSIZE(g_EthernetNetworkName), bstrName);
                                    SafeSysFreeString(bstrName);
                                    matchedNlm = TRUE;
                                }
                                NLM_CONNECTIVITY conn = NLM_CONNECTIVITY_DISCONNECTED;
                                pNet->GetConnectivity(&conn);
                                g_EthernetHasInternet = (conn & (NLM_CONNECTIVITY_IPV4_INTERNET | NLM_CONNECTIVITY_IPV6_INTERNET)) != 0;
                            }
                        }
                        pConn->Release();
                        if (matchedNlm) break;
                    }
                    pEnumConn->Release();
                }
                pNet->Release();
                if (matchedNlm) break;
            }
            pEnum->Release();
        }
    }
}

void UpdateFlyoutWindowSize(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;
    
    BOOL showWifiList = (g_NetworkCount > 0);
    int targetHeaderHeightBase = showWifiList ? HEADER_HEIGHT_BASE : 76;
    int targetWindowHeightBase = showWifiList ? WINDOW_HEIGHT_BASE : (targetHeaderHeightBase + FOOTER_HEIGHT_BASE);
    
    int newHeight = ScaleDpi(targetWindowHeightBase);
    int newHeader = ScaleDpi(targetHeaderHeightBase);
    
    if (newHeight != WINDOW_HEIGHT || newHeader != HEADER_HEIGHT) {
        WINDOW_HEIGHT = newHeight;
        HEADER_HEIGHT = newHeader;
        LIST_Y_START  = HEADER_HEIGHT + 1;
        LIST_Y_END    = WINDOW_HEIGHT - FOOTER_HEIGHT;
        WIFI_LABEL_Y  = HEADER_HEIGHT - ScaleDpi(24);
        
        InitRefreshButtonRect();
        RecalcArrowRect();
        
        RECT rcWork;
        // Use the near monitor instead of primary for multi-monitor setups
        HMONITOR hMon = MonitorFromWindow(FindWindowW(L"Shell_TrayWnd", NULL), MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi = { sizeof(mi) };
        if (hMon && GetMonitorInfoW(hMon, &mi))
            rcWork = mi.rcWork;
        else
            SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);
        APPBARDATA abd = { sizeof(APPBARDATA) };
        SHAppBarMessage(ABM_GETTASKBARPOS, &abd);
        
        int x = rcWork.right - WINDOW_WIDTH - 8;
        int y = rcWork.bottom - WINDOW_HEIGHT - 8;
        if (abd.uEdge == ABE_TOP)   y = abd.rc.bottom + 8;
        else if (abd.uEdge == ABE_LEFT)  x = abd.rc.right + 8;
        else if (abd.uEdge == ABE_RIGHT) x = abd.rc.left - WINDOW_WIDTH - 8;
        
        RECT rcClient = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
        DWORD dwExStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
        DWORD dwStyle   = GetWindowLongW(hwnd, GWL_STYLE);
        AdjustWindowRectEx(&rcClient, dwStyle, FALSE, dwExStyle);
        int winW = rcClient.right - rcClient.left;
        int winH = rcClient.bottom - rcClient.top;
        
        SetWindowPos(hwnd, NULL, x, y, winW, winH, SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

void RefreshNetworkData(BOOL forceDetection = FALSE) {
    if (g_Ctx.hWlanClient) {
        RefreshWifiData(g_Ctx.hWlanClient);
    } else {
        EnterCriticalSection(&g_Ctx.csLock);
        g_NetworkCount = 0;
        LeaveCriticalSection(&g_Ctx.csLock);
    }
    UpdateEthernetStatus();
    
    // Detect network location category (Home / Public / Work).
    // Skip the COM query entirely when the feature is disabled, and also
    // while the flyout isn't visible: the result is only consumed at paint
    // time, so running the registry enumeration (and, on a name-match miss,
    // up to four NLM/COM calls plus a GetAdaptersAddresses pass) on every
    // auto-refresh tick while hidden would be wasted work. WM_SHOW_FLYOUT
    // already calls RefreshNetworkData() when the flyout opens, so gating on
    // visibility here is safe. forceDetection bypasses this gate for the
    // one-off priming calls at mod startup and after a settings change, so
    // the category is already known (instead of falling back to the generic
    // PC icon) the first time the flyout is actually shown.
    BOOL isAnyConnected = (g_EthernetConnected || 
                           (g_NetworkCount > 0 && g_NetworkList[0].connState == CONN_STATE_CONNECTED));
    BOOL flyoutVisible = forceDetection ||
                         (g_hWndFlyout && IsWindow(g_hWndFlyout) && IsWindowVisible(g_hWndFlyout));
    if (isAnyConnected && g_Settings.useNetworkLocationIcons && flyoutVisible) {
        g_CurrentNetworkCategory = DetectNetworkLocationCategory();
    } else if (!isAnyConnected || !g_Settings.useNetworkLocationIcons) {
        // Don't wipe the reliable fallback just because the connection is
        // momentarily settling (e.g. during a reconnect). While the flyout is
        // merely hidden, g_CurrentNetworkCategory is left untouched below so
        // the first paint after reopening doesn't flash the generic icon.
        g_CurrentNetworkCategory = -1;
    }
    
    if (g_hWndFlyout && IsWindow(g_hWndFlyout)) {
        UpdateFlyoutWindowSize(g_hWndFlyout);
    }
}

// -------------------------------------------------------
// Password dialog
// -------------------------------------------------------
typedef struct {
    WCHAR* passwordBuffer;
    DWORD  bufferSize;
    BOOL   confirmed;
} PasswordDlgData;

static BOOL g_bPwdHoverOk     = FALSE;
static BOOL g_bPwdHoverCancel = FALSE;

LRESULT CALLBACK Win7PasswordWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    PasswordDlgData* data = (PasswordDlgData*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    switch (uMsg) {
    case WM_ERASEBKGND: {
        HDC hdc = (HDC)wParam;
        COLORREF bg = (g_Settings.theme == 1) ? RGB(20, 20, 20) : GetSysColor(COLOR_BTNFACE);
        HBRUSH hBr = CreateSolidBrush(bg);
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, hBr);
        DeleteObject(hBr);
        if (g_Settings.theme == 1) {
            HPEN hPen = CreatePen(PS_SOLID, 1, RGB(75, 75, 85)); 
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
            HBRUSH hOldBr = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
            int bx = ScaleDpi(144), by = ScaleDpi(49);
            int bw = ScaleDpi(245) + 1, bh = ScaleDpi(20) + 1;
            Rectangle(hdc, bx, by, bx + bw, by + bh);
            SelectObject(hdc, hOldBr);
            SelectObject(hdc, hOldPen);
            DeleteObject(hPen);
        }
        return 1;
    }
    case WM_NCHITTEST: {
        LRESULT r = DefWindowProcW(hwnd, uMsg, wParam, lParam);
        if (r==HTBOTTOM||r==HTBOTTOMLEFT||r==HTBOTTOMRIGHT||
            r==HTLEFT||r==HTRIGHT||r==HTTOP||r==HTTOPLEFT||r==HTTOPRIGHT)
            return HTCLIENT;
        return r;
    }
    case WM_MOUSEMOVE: {
        if (g_Settings.theme == 1) {
            HWND hBtnOk = GetDlgItem(hwnd, IDOK);
            HWND hBtnCancel = GetDlgItem(hwnd, IDCANCEL);
            POINT pt = { LOWORD(lParam), HIWORD(lParam) };
            ClientToScreen(hwnd, &pt);
            BOOL wasHoverOk = g_bPwdHoverOk, wasHoverCancel = g_bPwdHoverCancel;
            RECT rcOk, rcCancel;
            g_bPwdHoverOk = FALSE;
            g_bPwdHoverCancel = FALSE;
            if (hBtnOk && GetWindowRect(hBtnOk, &rcOk) && PtInRect(&rcOk, pt))
                g_bPwdHoverOk = TRUE;
            if (hBtnCancel && GetWindowRect(hBtnCancel, &rcCancel) && PtInRect(&rcCancel, pt))
                g_bPwdHoverCancel = TRUE;
            if (wasHoverOk != g_bPwdHoverOk && hBtnOk) InvalidateRect(hBtnOk, NULL, FALSE);
            if (wasHoverCancel != g_bPwdHoverCancel && hBtnCancel) InvalidateRect(hBtnCancel, NULL, FALSE);
            if (wasHoverOk != g_bPwdHoverOk || wasHoverCancel != g_bPwdHoverCancel) {
                TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0 };
                TrackMouseEvent(&tme);
            }
        }
        break;
    }
    case WM_MOUSELEAVE: {
        if (g_bPwdHoverOk || g_bPwdHoverCancel) {
            HWND hBtnOk = GetDlgItem(hwnd, IDOK);
            HWND hBtnCancel = GetDlgItem(hwnd, IDCANCEL);
            g_bPwdHoverOk = FALSE;
            g_bPwdHoverCancel = FALSE;
            if (hBtnOk) InvalidateRect(hBtnOk, NULL, FALSE);
            if (hBtnCancel) InvalidateRect(hBtnCancel, NULL, FALSE);
        }
        break;
    }
    case WM_CREATE: {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lParam;
        data = (PasswordDlgData*)cs->lpCreateParams;
        if (!data) return -1;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)data);
        
        HMODULE hVanDll = LoadLibraryExW(L"van.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (hVanDll) {
            HICON hIconSmall = (HICON)LoadImageW(hVanDll, MAKEINTRESOURCEW(100), IMAGE_ICON, 
                GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_SHARED);
            HICON hIconBig = (HICON)LoadImageW(hVanDll, MAKEINTRESOURCEW(100), IMAGE_ICON, 
                GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_SHARED);
            if (hIconSmall) {
                SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
            }
            if (hIconBig) {
                SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIconBig);
            }
        }
        HDC hdc = GetDC(hwnd);
        int ptPx = -MulDiv(9, GetDeviceCaps(hdc, LOGPIXELSY), 72);
        ReleaseDC(hwnd, hdc);
        HFONT hFontDlg = CreateFontW(ptPx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        SendMessageW(hwnd, WM_SETFONT, (WPARAM)hFontDlg, TRUE);
        HWND hInstr = CreateWindowExW(0, WC_STATICW, LOC(STR_PWD_INSTRUCTIONS),
            WS_CHILD|WS_VISIBLE, ScaleDpi(15), ScaleDpi(15), ScaleDpi(380), ScaleDpi(20), hwnd, (HMENU)200, cs->hInstance, NULL);
        SendMessageW(hInstr, WM_SETFONT, (WPARAM)hFontDlg, TRUE);
        
        HWND hLabel = CreateWindowExW(0, WC_STATICW, LOC(STR_PWD_LABEL),
            WS_CHILD|WS_VISIBLE, ScaleDpi(15), ScaleDpi(53), ScaleDpi(125), ScaleDpi(18), hwnd, NULL, cs->hInstance, NULL);
        SendMessageW(hLabel, WM_SETFONT, (WPARAM)hFontDlg, TRUE);
        
        BOOL bDarkPwd = (g_Settings.theme == 1);
        DWORD dwEditExStyle = bDarkPwd ? 0 : WS_EX_CLIENTEDGE;
        
        HWND hEdit = CreateWindowExW(dwEditExStyle, WC_EDITW, L"",
            WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL,
            ScaleDpi(145), ScaleDpi(50), ScaleDpi(245), ScaleDpi(20), hwnd, (HMENU)101, cs->hInstance, NULL);
        SendMessageW(hEdit, WM_SETFONT, (WPARAM)hFontDlg, TRUE);
        SendMessageW(hEdit, EM_SETPASSWORDCHAR, 0x25CF, 0);
        if (bDarkPwd) SetWindowTheme(hEdit, L"DarkMode_Explorer", NULL);
        SetFocus(hEdit);
        
        HWND hCheck = CreateWindowExW(0, WC_BUTTONW, L"",
            WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX, ScaleDpi(135), ScaleDpi(80), ScaleDpi(18), ScaleDpi(18), hwnd, (HMENU)102, cs->hInstance, NULL);
        SendMessageW(hCheck, BM_SETCHECK, BST_CHECKED, 0);
        if (bDarkPwd) SetWindowTheme(hCheck, L"DarkMode_Explorer", NULL);
        
        HWND hCheckText = CreateWindowExW(0, WC_STATICW, LOC(STR_PWD_HIDE_CHARS),
            WS_CHILD|WS_VISIBLE|SS_NOTIFY, ScaleDpi(156), ScaleDpi(80), ScaleDpi(200), ScaleDpi(18), hwnd, (HMENU)103, cs->hInstance, NULL);
        SendMessageW(hCheckText, WM_SETFONT, (WPARAM)hFontDlg, TRUE);
        
        if (bDarkPwd) {
            InvalidateRect(hCheck, NULL, TRUE);
            InvalidateRect(hCheckText, NULL, TRUE);
            UpdateWindow(hwnd);
        }
        
        RECT rcClient; GetClientRect(hwnd, &rcClient);
        int btnW = ScaleDpi(85), btnH = ScaleDpi(24), btnY = rcClient.bottom - ScaleDpi(35);
        HWND hBtnOk = CreateWindowExW(0, WC_BUTTONW, LOC(STR_PWD_OK),
            WS_CHILD|WS_VISIBLE|(bDarkPwd ? BS_OWNERDRAW : BS_DEFPUSHBUTTON),
            rcClient.right - btnW - ScaleDpi(15), btnY, btnW, btnH, hwnd, (HMENU)IDOK, cs->hInstance, NULL);
        SendMessageW(hBtnOk, WM_SETFONT, (WPARAM)hFontDlg, TRUE);
        if (bDarkPwd) SetWindowTheme(hBtnOk, L"DarkMode_Explorer", NULL);
        HWND hBtnCancel = CreateWindowExW(0, WC_BUTTONW, LOC(STR_PWD_CANCEL),
            WS_CHILD|WS_VISIBLE|(bDarkPwd ? BS_OWNERDRAW : 0),
            rcClient.right - (btnW * 2) - ScaleDpi(25), btnY, btnW, btnH,
            hwnd, (HMENU)IDCANCEL, cs->hInstance, NULL);
        SendMessageW(hBtnCancel, WM_SETFONT, (WPARAM)hFontDlg, TRUE);
        if (bDarkPwd) SetWindowTheme(hBtnCancel, L"DarkMode_Explorer", NULL);
        break;
    }
    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
        if (!pdis) break;
        if (pdis->CtlID != IDOK && pdis->CtlID != IDCANCEL) break;
        if (g_Settings.theme != 1) break;
        BOOL isPressed  = (pdis->itemState & ODS_SELECTED) != 0;
        BOOL isDisabled = (pdis->itemState & ODS_DISABLED) != 0;
        BOOL isFocused  = (pdis->itemState & ODS_FOCUS) != 0;
        BOOL isHovering = (pdis->CtlID == IDOK)     ? (g_bPwdHoverOk     && !isPressed && !isDisabled)
                        : (pdis->CtlID == IDCANCEL) ? (g_bPwdHoverCancel && !isPressed && !isDisabled)
                        : FALSE;
        HDC  hdcReal = pdis->hDC;
        RECT rc  = pdis->rcItem;
        int  w = rc.right - rc.left;
        int  h = rc.bottom - rc.top;
        if (w <= 0 || h <= 0) break;
        WCHAR szText[64];
        int textLen = GetWindowTextW(pdis->hwndItem, szText, 64);
        COLORREF bgColor;
        if (isDisabled) bgColor = RGB(50, 50, 58);
        else if (isPressed) bgColor = RGB(35, 35, 45);
        else if (isHovering) bgColor = RGB(70, 70, 85);
        else bgColor = RGB(60, 60, 72);
        COLORREF lightColor = isPressed ? RGB(25, 25, 32) : (isHovering ? RGB(95, 95, 115) : RGB(85, 85, 100));
        COLORREF darkColor  = isPressed ? RGB(60, 60, 72) : (isHovering ? RGB(35, 35, 45)  : RGB(25, 25, 32));
        COLORREF textColor  = isDisabled ? RGB(130, 130, 140) : RGB(255, 255, 255);
        COLORREF hoverBorder = isHovering ? RGB(90, 90, 120) : RGB(0, 0, 0);
        HDC hdcMem = CreateCompatibleDC(hdcReal);
        HBITMAP hBmpMem = CreateCompatibleBitmap(hdcReal, w, h);
        HBITMAP hOldBmpMem = (HBITMAP)SelectObject(hdcMem, hBmpMem);
        RECT rcLocal = {0, 0, w, h};
        HBRUSH hBrBg = CreateSolidBrush(bgColor);
        FillRect(hdcMem, &rcLocal, hBrBg);
        DeleteObject(hBrBg);
        HPEN hPenLight = CreatePen(PS_SOLID, 1, lightColor);
        HPEN hPenDark  = CreatePen(PS_SOLID, 1, darkColor);
        HPEN hPenHover = isHovering ? CreatePen(PS_SOLID, 1, hoverBorder) : NULL;
        HPEN hOldPen = (HPEN)SelectObject(hdcMem, hPenLight);
        MoveToEx(hdcMem, 0, h - 1, NULL); LineTo(hdcMem, 0, 0); LineTo(hdcMem, w - 1, 0);
        SelectObject(hdcMem, hPenDark);
        MoveToEx(hdcMem, w - 1, 0, NULL); LineTo(hdcMem, w - 1, h - 1); LineTo(hdcMem, 0, h - 1);
        if (isHovering && hPenHover) {
            SelectObject(hdcMem, hPenHover);
            MoveToEx(hdcMem, 1, 1, NULL); LineTo(hdcMem, w - 2, 1); LineTo(hdcMem, w - 2, h - 2); LineTo(hdcMem, 1, h - 2); LineTo(hdcMem, 1, 1);
            DeleteObject(hPenHover);
        }
        SelectObject(hdcMem, hOldPen); DeleteObject(hPenLight); DeleteObject(hPenDark);
        if (isFocused) {
            RECT rcFocus = rcLocal; InflateRect(&rcFocus, -3, -3);
            HBRUSH hOldBr = (HBRUSH)SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
            SetTextColor(hdcMem, RGB(150, 150, 165)); DrawFocusRect(hdcMem, &rcFocus);
            SelectObject(hdcMem, hOldBr);
        }
        SetBkMode(hdcMem, TRANSPARENT); SetTextColor(hdcMem, textColor);
        HFONT hOldFont = (HFONT)SelectObject(hdcMem, (HFONT)SendMessageW(pdis->hwndItem, WM_GETFONT, 0, 0));
        RECT rcText = rcLocal; if (isPressed) { rcText.left += 1; rcText.top += 1; }
        DrawTextW(hdcMem, szText, textLen, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdcMem, hOldFont);
        BitBlt(hdcReal, rc.left, rc.top, w, h, hdcMem, 0, 0, SRCCOPY);
        SelectObject(hdcMem, hOldBmpMem); DeleteObject(hBmpMem); DeleteDC(hdcMem);
        break;
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdc = (HDC)wParam;
        HWND hwndCtrl = (HWND)lParam;
        if (hwndCtrl == GetDlgItem(hwnd, 102) || hwndCtrl == GetDlgItem(hwnd, 103)) {
            if (g_Settings.theme == 1) {
                SetBkColor(hdc, RGB(20, 20, 20)); SetBkMode(hdc, OPAQUE); SetTextColor(hdc, RGB(255, 255, 255));
                static HBRUSH hBrushHideDark = NULL;
                if (!hBrushHideDark) hBrushHideDark = CreateSolidBrush(RGB(20, 20, 20));
                return (INT_PTR)hBrushHideDark;
            } else {
                SetBkColor(hdc, GetSysColor(COLOR_BTNFACE)); SetBkMode(hdc, OPAQUE); SetTextColor(hdc, RGB(0, 0, 0));
                static HBRUSH hBrushHideLight = NULL;
                if (!hBrushHideLight) hBrushHideLight = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
                return (INT_PTR)hBrushHideLight;
            }
        }
        if (hwndCtrl == g_hWndCheckboxConnect && g_SelectedRowIndex >= 0 && g_SelectedRowIndex < g_NetworkCount) {
            WifiNetworkItem* item = &g_NetworkList[g_SelectedRowIndex];
            if (item->connState == CONN_STATE_IDLE || item->connState == CONN_STATE_ERROR) {
                COLORREF chkBg   = (g_Settings.theme == 1) ? RGB(40, 40, 50)    : RGB(228, 241, 252);
                COLORREF chkText = (g_Settings.theme == 1) ? RGB(255, 255, 255) : RGB(0, 0, 0);
                SetBkColor(hdc, chkBg); SetBkMode(hdc, OPAQUE); SetTextColor(hdc, chkText);
                static HBRUSH hBrushCheckbox = NULL;
                if (!hBrushCheckbox) hBrushCheckbox = CreateSolidBrush(chkBg);
                return (INT_PTR)hBrushCheckbox;
            } else if (g_Settings.theme == 1) {
                COLORREF chkBg = GetFooterBgColor();
                SetBkColor(hdc, chkBg); SetBkMode(hdc, OPAQUE); SetTextColor(hdc, RGB(255, 255, 255));
                static HBRUSH hBrushCheckboxDark = NULL;
                if (!hBrushCheckboxDark) hBrushCheckboxDark = CreateSolidBrush(chkBg);
                return (INT_PTR)hBrushCheckboxDark;
            } else {
                SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(255, 255, 255));
                return (INT_PTR)GetStockObject(HOLLOW_BRUSH);
            }
        }
        if (g_Settings.theme == 1) {
            SetBkColor(hdc, RGB(20, 20, 20)); SetTextColor(hdc, RGB(100, 200, 255)); SetBkMode(hdc, OPAQUE);
            static HBRUSH hBrPwdStatic = NULL;
            if (!hBrPwdStatic) hBrPwdStatic = CreateSolidBrush(RGB(20, 20, 20));
            return (INT_PTR)hBrPwdStatic;
        } else {
            SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(14, 75, 184));
            return (INT_PTR)GetStockObject(NULL_BRUSH);
        }
    }
    case WM_CTLCOLOREDIT: {
        HDC hdc = (HDC)wParam;
        if (g_Settings.theme == 1) {
            SetBkColor(hdc, RGB(40, 40, 50));
            SetTextColor(hdc, RGB(255, 255, 255));
            SetBkMode(hdc, OPAQUE);
            static HBRUSH hBrEdit = NULL;
            if (!hBrEdit) hBrEdit = CreateSolidBrush(RGB(40, 40, 50));
            return (INT_PTR)hBrEdit;
        }
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    case WM_CTLCOLORBTN: {
        HDC hdc = (HDC)wParam; HWND hwndBtn = (HWND)lParam;
        if (hwndBtn == g_hWndCheckboxConnect && g_SelectedRowIndex >= 0 && g_SelectedRowIndex < g_NetworkCount) {
            WifiNetworkItem* item = &g_NetworkList[g_SelectedRowIndex];
            if (item->connState == CONN_STATE_IDLE || item->connState == CONN_STATE_ERROR) {
                COLORREF chkBg   = (g_Settings.theme == 1) ? RGB(40, 40, 50)    : RGB(228, 241, 252);
                COLORREF chkText = (g_Settings.theme == 1) ? RGB(255, 255, 255) : RGB(0, 0, 0);
                SetBkColor(hdc, chkBg); SetBkMode(hdc, OPAQUE); SetTextColor(hdc, chkText);
                static HBRUSH hBrushCheckboxBtn = NULL;
                if (!hBrushCheckboxBtn) hBrushCheckboxBtn = CreateSolidBrush(chkBg);
                return (INT_PTR)hBrushCheckboxBtn;
            } else {
                SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(255, 255, 255));
                return (INT_PTR)GetStockObject(HOLLOW_BRUSH);
            }
        }
        if (hwndBtn == GetDlgItem(hwnd, 102)) {
            if (g_Settings.theme == 1) {
                SetBkColor(hdc, RGB(20, 20, 20)); SetBkMode(hdc, OPAQUE);
                static HBRUSH hBrHideDark = NULL; if (!hBrHideDark) hBrHideDark = CreateSolidBrush(RGB(20, 20, 20));
                return (INT_PTR)hBrHideDark;
            } else {
                SetBkColor(hdc, GetSysColor(COLOR_BTNFACE)); SetBkMode(hdc, OPAQUE);
                static HBRUSH hBrHideLight = NULL; if (!hBrHideLight) hBrHideLight = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
                return (INT_PTR)hBrHideLight;
            }
        }
        if (hwndBtn == GetDlgItem(hwnd, IDOK) || hwndBtn == GetDlgItem(hwnd, IDCANCEL)) {
            if (g_Settings.theme == 1) {
                SetBkColor(hdc, RGB(50, 50, 60)); SetTextColor(hdc, RGB(255, 255, 255)); SetBkMode(hdc, OPAQUE);
                static HBRUSH hBrBtn = NULL; if (!hBrBtn) hBrBtn = CreateSolidBrush(RGB(50, 50, 60));
                return (INT_PTR)hBrBtn;
            }
        }
        return (INT_PTR)DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == 103) {
            HWND hCheck = GetDlgItem(hwnd, 102);
            BOOL checked = SendMessageW(hCheck, BM_GETCHECK, 0, 0) == BST_CHECKED;
            SendMessageW(hCheck, BM_SETCHECK, checked ? BST_UNCHECKED : BST_CHECKED, 0);
            SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(102, BN_CLICKED), (LPARAM)hCheck);
            return 0;
        }
        if (LOWORD(wParam) == 102) {
            HWND hEdit = GetDlgItem(hwnd, 101);
            BOOL checked = SendMessageW(GetDlgItem(hwnd, 102), BM_GETCHECK, 0, 0) == BST_CHECKED;
            SendMessageW(hEdit, EM_SETPASSWORDCHAR, checked ? 0x25CF : 0, 0);
            InvalidateRect(hEdit, NULL, TRUE);
            return 0;
        }
        if (LOWORD(wParam) == IDOK) {
            if (data) { 
                GetDlgItemTextW(hwnd, 101, data->passwordBuffer, data->bufferSize); 
                WCHAR* p = data->passwordBuffer;
                while (*p == L' ' || *p == L'\t') p++;
                if (*p == L'\0') {
                    MessageBoxW(hwnd, LOC(STR_PWD_EMPTY), LOC(STR_ERROR_TITLE), MB_OK | MB_ICONWARNING);
                    SetFocus(GetDlgItem(hwnd, 101));
                    return 0;
                }
                data->confirmed = TRUE; 
            }
            DestroyWindow(hwnd); return 0;
        }
        if (LOWORD(wParam) == IDCANCEL) {
            if (data) data->confirmed = FALSE;
            DestroyWindow(hwnd); return 0;
        }
        break;
    }
    case WM_CLOSE:
        if (data) data->confirmed = FALSE;
        DestroyWindow(hwnd); 
        return 0;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

BOOL PromptNetworkPassword(HWND hParent, WCHAR* passwordBuffer, DWORD bufferSize) {
    if (!SafeToAccessUI()) return FALSE;
    g_inPasswordPrompt = TRUE;
    HINSTANCE hInst = GetModuleHandle(NULL);
    WNDCLASSW wc = {0};
    wc.lpfnWndProc   = Win7PasswordWndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = L"Win7NetPwdClass";
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    UnregisterClassW(wc.lpszClassName, hInst);
    RegisterClassW(&wc);
    
    PasswordDlgData data = { passwordBuffer, bufferSize, FALSE };
    RECT rcWork;
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);
    int dlgW=ScaleDpi(420), dlgH=ScaleDpi(180);
    
    HWND hDlg = CreateWindowExW(
        WS_EX_DLGMODALFRAME|WS_EX_WINDOWEDGE|WS_EX_TOPMOST,
        wc.lpszClassName, LOC(STR_PWD_TITLE),
        WS_POPUP|WS_CAPTION|WS_SYSMENU,
        rcWork.right-dlgW-10, rcWork.bottom-dlgH-5, dlgW,dlgH,
        hParent, NULL, hInst, &data);
    
    if (!hDlg) {
        g_inPasswordPrompt = FALSE;
        return FALSE;
    }
    if (g_Settings.theme == 1) {
        BOOL useDark = TRUE;
        DwmSetWindowAttribute(hDlg, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDark, sizeof(useDark));
        SetWindowPos(hDlg, NULL, 0, 0, 0, 0,
                     SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED|SWP_NOACTIVATE);
    }
    ShowWindow(hDlg, SW_SHOW);
    EnableWindow(hParent, FALSE);
    
    MSG msg;
    while (IsWindow(hDlg) && GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    
    EnableWindow(hParent, TRUE);
    ShowWindow(hParent, SW_SHOW);
    SetForegroundWindow(hParent);
    
    g_inPasswordPrompt = FALSE;
    return data.confirmed;
}

void BuildWlanProfileXml(const WifiNetworkItem* item, const WCHAR* password, BOOL autoConnect, WCHAR* outXml, size_t outSize) {
    WCHAR escapedSsid[256] = {0};
    WCHAR escapedPwd[256] = {0};
    
    auto EscapeXml = [](const WCHAR* src, WCHAR* dst, size_t dstSize) {
        size_t d = 0;
        for (size_t i = 0; src[i] && d < dstSize - 6; i++) {
            if (d + 6 >= dstSize) break;
            switch (src[i]) {
                case L'&': StringCchCatW(dst, dstSize, L"&amp;"); d += 5; break;
                case L'<': StringCchCatW(dst, dstSize, L"&lt;"); d += 4; break;
                case L'>': StringCchCatW(dst, dstSize, L"&gt;"); d += 4; break;
                case L'\"': StringCchCatW(dst, dstSize, L"&quot;"); d += 6; break;
                case L'\'': StringCchCatW(dst, dstSize, L"&apos;"); d += 6; break;
                default: dst[d++] = src[i]; dst[d] = L'\0'; break;
            }
        }
    };
    EscapeXml(item->ssid, escapedSsid, ARRAYSIZE(escapedSsid));
    if (password) {
        EscapeXml(password, escapedPwd, ARRAYSIZE(escapedPwd));
    }
    const WCHAR* connMode = autoConnect ? L"auto" : L"manual";
    const WCHAR* authStr = L"open";
    const WCHAR* encStr  = L"none";
    switch (item->authAlgorithm) {
        case DOT11_AUTH_ALGO_80211_OPEN:   authStr = L"open";    break;
        case DOT11_AUTH_ALGO_80211_SHARED_KEY: authStr = L"shared"; break;
        case DOT11_AUTH_ALGO_WPA:          authStr = L"WPA";     break;
        case DOT11_AUTH_ALGO_WPA_PSK:      authStr = L"WPAPSK";  break;
        case DOT11_AUTH_ALGO_WPA3:         authStr = L"WPA3";    break;
        case DOT11_AUTH_ALGO_WPA3_SAE:     authStr = L"WPA3SAE"; break;
        case DOT11_AUTH_ALGO_RSNA:         authStr = L"WPA2";    break;
        case DOT11_AUTH_ALGO_RSNA_PSK:     authStr = L"WPA2PSK"; break;
        default:                           authStr = L"WPA2PSK"; break;
    }
    switch (item->cipherAlgorithm) {
        case DOT11_CIPHER_ALGO_NONE:       encStr = L"none"; break;
        case DOT11_CIPHER_ALGO_WEP:        encStr = L"WEP";  break;
        case DOT11_CIPHER_ALGO_WEP40:      encStr = L"WEP";  break;
        case DOT11_CIPHER_ALGO_WEP104:     encStr = L"WEP";  break;
        case DOT11_CIPHER_ALGO_TKIP:       encStr = L"TKIP"; break;
        case DOT11_CIPHER_ALGO_CCMP:       encStr = L"AES";  break;
        case DOT11_CIPHER_ALGO_WPA_USE_GROUP: encStr = L"TKIP"; break;
        default:                           encStr = L"AES";  break;
    }
    BOOL isEnterprise = (item->authAlgorithm == DOT11_AUTH_ALGO_WPA ||
                         item->authAlgorithm == DOT11_AUTH_ALGO_WPA3 ||
                         item->authAlgorithm == DOT11_AUTH_ALGO_RSNA);
    if (item->isSecured) {
        if (!isEnterprise) {
            StringCchPrintfW(outXml, outSize,
                L"<?xml version=\"1.0\"?>"
                L"<WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\">"
                L"<name>%s</name>"
                L"<SSIDConfig><SSID><name>%s</name></SSID></SSIDConfig>"
                L"<connectionType>ESS</connectionType>"
                L"<connectionMode>%s</connectionMode>"
                L"<MSM><security>"
                L"<authEncryption><authentication>%s</authentication><encryption>%s</encryption><useOneX>false</useOneX></authEncryption>"
                L"<sharedKey><keyType>passPhrase</keyType><protected>false</protected><keyMaterial>%s</keyMaterial></sharedKey>"
                L"</security></MSM></WLANProfile>",
                escapedSsid, escapedSsid, connMode, 
                authStr, encStr, escapedPwd);
        } else {
            StringCchPrintfW(outXml, outSize,
                L"<?xml version=\"1.0\"?>"
                L"<WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\">"
                L"<name>%s</name>"
                L"<SSIDConfig><SSID><name>%s</name></SSID></SSIDConfig>"
                L"<connectionType>ESS</connectionType>"
                L"<connectionMode>%s</connectionMode>"
                L"<MSM><security>"
                L"<authEncryption><authentication>%s</authentication><encryption>%s</encryption><useOneX>true</useOneX>"
                L"</security></MSM></WLANProfile>",
                escapedSsid, escapedSsid, connMode, 
                authStr, encStr);
        }
    } else {
        StringCchPrintfW(outXml, outSize,
            L"<?xml version=\"1.0\"?>"
            L"<WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\">"
            L"<name>%s</name>"
            L"<SSIDConfig><SSID><name>%s</name></SSID></SSIDConfig>"
            L"<connectionType>ESS</connectionType>"
            L"<connectionMode>%s</connectionMode>"
            L"<MSM><security><authEncryption><authentication>open</authentication><encryption>none</encryption><useOneX>false</useOneX></authEncryption></security></MSM></WLANProfile>",
            escapedSsid, escapedSsid, connMode);
    }
}

static BOOL XmlTagEqualsCI(const WCHAR* xml, const WCHAR* tagName, const WCHAR* expectedValue) {
    if (!xml || !tagName || !expectedValue) return FALSE;
    WCHAR openTag[64] = {0};
    StringCchPrintfW(openTag, ARRAYSIZE(openTag), L"<%s>", tagName);
    const WCHAR* start = wcsstr(xml, openTag);
    if (!start) return FALSE;
    start += lstrlenW(openTag);
    WCHAR closeTag[64] = {0};
    StringCchPrintfW(closeTag, ARRAYSIZE(closeTag), L"</%s>", tagName);
    const WCHAR* end = wcsstr(start, closeTag);
    if (!end || end < start) return FALSE;
    size_t len = (size_t)(end - start);
    if (len == 0 || len >= 64) return FALSE;
    WCHAR value[64] = {0};
    StringCchCopyNW(value, ARRAYSIZE(value), start, len);
    return (_wcsicmp(value, expectedValue) == 0);
}

static BOOL ProfileSecurityMatches(const WCHAR* profileXml,
                                    DOT11_AUTH_ALGORITHM authAlgorithm,
                                    DOT11_CIPHER_ALGORITHM cipherAlgorithm) {
    if (!profileXml) return FALSE;
    const WCHAR* expectedAuth = L"open";
    switch (authAlgorithm) {
        case DOT11_AUTH_ALGO_80211_OPEN:       expectedAuth = L"open";    break;
        case DOT11_AUTH_ALGO_80211_SHARED_KEY: expectedAuth = L"shared";  break;
        case DOT11_AUTH_ALGO_WPA:              expectedAuth = L"WPA";     break;
        case DOT11_AUTH_ALGO_WPA_PSK:          expectedAuth = L"WPAPSK";  break;
        case DOT11_AUTH_ALGO_WPA3:             expectedAuth = L"WPA3";    break;
        case DOT11_AUTH_ALGO_WPA3_SAE:         expectedAuth = L"WPA3SAE"; break;
        case DOT11_AUTH_ALGO_RSNA:             expectedAuth = L"WPA2";    break;
        case DOT11_AUTH_ALGO_RSNA_PSK:         expectedAuth = L"WPA2PSK"; break;
        default:                               expectedAuth = L"WPA2PSK"; break;
    }
    const WCHAR* expectedEnc = L"none";
    switch (cipherAlgorithm) {
        case DOT11_CIPHER_ALGO_NONE:          expectedEnc = L"none"; break;
        case DOT11_CIPHER_ALGO_WEP:           expectedEnc = L"WEP";  break;
        case DOT11_CIPHER_ALGO_WEP40:         expectedEnc = L"WEP";  break;
        case DOT11_CIPHER_ALGO_WEP104:        expectedEnc = L"WEP";  break;
        case DOT11_CIPHER_ALGO_TKIP:          expectedEnc = L"TKIP"; break;
        case DOT11_CIPHER_ALGO_CCMP:          expectedEnc = L"AES";  break;
        case DOT11_CIPHER_ALGO_WPA_USE_GROUP: expectedEnc = L"TKIP"; break;
        default:                              expectedEnc = L"AES";  break;
    }
    if (authAlgorithm == DOT11_AUTH_ALGO_80211_OPEN) {
        return XmlTagEqualsCI(profileXml, L"authentication", L"open");
    }
    BOOL authMatches = XmlTagEqualsCI(profileXml, L"authentication", expectedAuth);
    BOOL encMatches  = XmlTagEqualsCI(profileXml, L"encryption", expectedEnc);
    return authMatches && encMatches;
}

static unsigned int __stdcall AsyncConnectThreadProc(void* pParam) {
    AsyncConnectContext* ctx = (AsyncConnectContext*)pParam;
    if (!ctx) return 1;
    
    DWORD waitResult = WaitForSingleObject(g_hConnectMutex.get(), 10000);
    if (waitResult != WAIT_OBJECT_0) {
        Wh_Log(L"AsyncConnectThreadProc: Could not acquire mutex (timeout or error %lu)", waitResult);
        if (ctx->hWndNotify) {
            PostMessageW(ctx->hWndNotify, WM_ASYNC_CONNECT_COMPLETE, 0, (LPARAM)ERROR_TIMEOUT);
        }
        SecureZeroMemory(ctx->password, sizeof(ctx->password));
        free(ctx);
        return 1;
    }
    DWORD dwResult = ERROR_SUCCESS;
    DWORD dwReason = 0;
    
    if (ctx->isSecured && !ctx->hasProfile) {
        WCHAR xmlProfile[2048] = {0};
        BOOL autoConn = (SendMessageW(g_hWndCheckboxConnect, BM_GETCHECK, 0, 0) == BST_CHECKED);
        
        WifiNetworkItem tempItem = {{0}};
        StringCchCopyW(tempItem.ssid, ARRAYSIZE(tempItem.ssid), ctx->ssid);
        tempItem.isSecured = ctx->isSecured;
        tempItem.authAlgorithm = ctx->authAlgorithm;
        tempItem.cipherAlgorithm = ctx->cipherAlgorithm;
        BuildWlanProfileXml(&tempItem, ctx->password, autoConn, xmlProfile, ARRAYSIZE(xmlProfile));
        
        dwResult = WlanSetProfile(g_Ctx.hWlanClient, &ctx->interfaceGuid, 
            0, xmlProfile, NULL, TRUE, NULL, &dwReason);
        
        LogSsidSafe(L"WlanSetProfile for", ctx->ssid);
        Wh_Log(L"  returned: %lu (reason: %lu)", dwResult, dwReason);
        
        if (dwResult != ERROR_SUCCESS) {
            if (ctx->hWndNotify) {
                PostMessageW(ctx->hWndNotify, WM_ASYNC_CONNECT_COMPLETE, 0, (LPARAM)dwResult);
            }
            ReleaseMutex(g_hConnectMutex.get());
            SecureZeroMemory(ctx->password, sizeof(ctx->password));
            free(ctx);
            return 1;
        }
        ctx->hasProfile = TRUE; 
    }
    
    WLAN_CONNECTION_PARAMETERS params;
    ZeroMemory(&params, sizeof(params));
    params.wlanConnectionMode = wlan_connection_mode_profile;
    params.strProfile = ctx->ssid;
    params.dot11BssType = ctx->dot11BssType;
    params.dwFlags = 0;
    DOT11_BSSID_LIST bssidList;
    if (ctx->hasBssid) {
        ZeroMemory(&bssidList, sizeof(bssidList));
        bssidList.Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
        bssidList.Header.Revision = DOT11_BSSID_LIST_REVISION_1;
        bssidList.Header.Size = sizeof(bssidList);
        bssidList.uNumOfEntries = 1;
        bssidList.uTotalNumOfEntries = 1;
        CopyMemory(bssidList.BSSIDs[0], ctx->bssid, sizeof(DOT11_MAC_ADDRESS));
        params.pDesiredBssidList = &bssidList;
    }
    
    dwResult = WlanConnect(g_Ctx.hWlanClient, &ctx->interfaceGuid, &params, NULL);
    LogSsidSafe(L"WlanConnect for", ctx->ssid);
    Wh_Log(L"  returned: %lu (0x%08X), targeted BSSID: %s", dwResult, dwResult, ctx->hasBssid ? L"yes" : L"no (system choice)");
    
    if (ctx->hWndNotify) {
        PostMessageW(ctx->hWndNotify, WM_ASYNC_CONNECT_COMPLETE, (dwResult == ERROR_SUCCESS), (LPARAM)dwResult);
    }
    
    ReleaseMutex(g_hConnectMutex.get());
    SecureZeroMemory(ctx->password, sizeof(ctx->password));
    free(ctx);
    return 0;
}

// Waits for a previous async-connect thread to finish and closes its handle,
// off the flyout's UI thread. See the call site in AskForPasswordAndConnect.
static unsigned __stdcall ReapConnectThreadHandleProc(void* p) {
    HANDLE h = (HANDLE)p;
    WaitForSingleObject(h, 5000);
    CloseHandle(h);
    return 0;
}

static BOOL AskForPasswordAndConnect(int index) {
    if (index < 0 || index >= g_NetworkCount || !g_Ctx.hWlanClient) return FALSE;
    if (g_PendingConnectIndex >= 0 && g_PendingConnectIndex < g_NetworkCount && g_PendingConnectIndex != index) {
        g_NetworkList[g_PendingConnectIndex].connState = CONN_STATE_IDLE;
        g_NetworkList[g_PendingConnectIndex].operationStartTime = 0;
        Wh_Log(L"Previous pending connection %d reset", g_PendingConnectIndex);
    }
    
    WifiNetworkItem* item = &g_NetworkList[index];
    
    AsyncConnectContext* ctx = (AsyncConnectContext*)calloc(1, sizeof(AsyncConnectContext));
    if (!ctx) {
        g_PendingConnectIndex = -1;
        return FALSE;
    }
    ZeroMemory(ctx, sizeof(AsyncConnectContext));
    ctx->hWndNotify = g_hWndFlyout;
    ctx->interfaceGuid = item->interfaceGuid;
    ctx->dot11BssType = item->dot11BssType;
    ctx->hasProfile = item->hasProfile;
    ctx->isSecured = item->isSecured;
    ctx->authAlgorithm = item->authAlgorithm;
    ctx->cipherAlgorithm = item->cipherAlgorithm;
    StringCchCopyW(ctx->ssid, ARRAYSIZE(ctx->ssid), item->ssid);
    ctx->hasBssid = item->hasBssid;
    if (item->hasBssid) {
        CopyMemory(ctx->bssid, item->bssid, sizeof(DOT11_MAC_ADDRESS));
    }
    BOOL needsPassword = (item->isSecured && !item->hasProfile);
    if (needsPassword) {
        WCHAR password[65] = {0};
        if (!PromptNetworkPassword(g_hWndFlyout, password, ARRAYSIZE(password) - 1)) {
            LogSsidSafe(L"User cancelled password for", item->ssid);
            g_PendingConnectIndex = -1;
            SecureZeroMemory(ctx->password, sizeof(ctx->password));
            free(ctx);
            return FALSE;
        }
        StringCchCopyW(ctx->password, ARRAYSIZE(ctx->password), password);
        BOOL isEmpty = TRUE;
        for (int i = 0; i < 64 && password[i]; i++) {
            if (password[i] != L' ' && password[i] != L'\t') {
                isEmpty = FALSE;
                break;
            }
        }
        if (isEmpty) {
            LogSsidSafe(L"Empty password provided for", item->ssid);
            MessageBoxW(g_hWndFlyout, LOC(STR_PWD_EMPTY), LOC(STR_ERROR_TITLE), MB_OK | MB_ICONWARNING);
            g_PendingConnectIndex = -1;
            free(ctx);
            return FALSE;
        }
    } else {
        ctx->password[0] = L'\0';
    }
    
    item->connState = CONN_STATE_CONNECTING;
    item->operationStartTime = GetTickCount();
    g_PendingConnectIndex = index;
    
    Wh_Log(L"AskForPasswordAndConnect: set g_PendingConnectIndex=%d, SSID=%s, hasProfile=%d", 
           index, item->ssid, item->hasProfile);
    
    if (!g_TimeoutTimer && g_hWndFlyout && IsWindow(g_hWndFlyout)) {
        g_TimeoutTimer = SetTimer(g_hWndFlyout, 1002, 5000, NULL);
        Wh_Log(L"Timeout timer started (id=%llu)", (unsigned long long)g_TimeoutTimer);
    } else if (!g_hWndFlyout || !IsWindow(g_hWndFlyout)) {
        Wh_Log(L"WARNING: Could not start timeout timer - flyout not ready");
    }
    UpdateLayoutGeometry();
    if (g_hWndFlyout) InvalidateRect(g_hWndFlyout, NULL, TRUE);
    
    HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, AsyncConnectThreadProc, ctx, 0, NULL);
    if (!hThread) {
        Wh_Log(L"Failed to create async connect thread");
        item->connState = CONN_STATE_IDLE;
        g_PendingConnectIndex = -1;
        free(ctx);
        return FALSE;
    }
    if (g_hConnectThread) {
        // Don't block the flyout's UI thread waiting for the previous
        // WlanConnect to finish (it can still be in flight on rapid
        // reconnects to a second network). Hand the old handle off to a
        // short-lived reaper thread that waits and closes it asynchronously.
        HANDLE hOldThread = g_hConnectThread;
        HANDLE hReaper = (HANDLE)_beginthreadex(NULL, 0,
            ReapConnectThreadHandleProc, hOldThread, 0, NULL);
        if (hReaper) {
            CloseHandle(hReaper);
        } else {
            // Couldn't spin up a reaper thread; fall back to closing the
            // handle immediately. This does not terminate the still-running
            // thread, it only releases our reference to its handle.
            CloseHandle(hOldThread);
        }
    }
    g_hConnectThread = hThread;
    return TRUE;
}

void ConnectToNetwork(int index) {
    if (index < 0 || index >= g_NetworkCount || !g_Ctx.hWlanClient) return;
    WifiNetworkItem* item = &g_NetworkList[index];
    if (item->connState == CONN_STATE_CONNECTED) {
        DisconnectFromNetwork(index);
        return;
    }
    if (item->connState == CONN_STATE_CONNECTING) {
        LogSsidSafe(L"Already connecting to, ignoring", item->ssid);
        return;
    }
    for (int i = 0; i < g_NetworkCount; i++) {
        if (i != index && (g_NetworkList[i].connState == CONN_STATE_CONNECTING ||
                           g_NetworkList[i].connState == CONN_STATE_ERROR)) {
            g_NetworkList[i].connState = CONN_STATE_IDLE;
            g_NetworkList[i].operationStartTime = 0;
        }
    }
    AskForPasswordAndConnect(index);
}

void DisconnectFromNetwork(int index) {
    if (index < 0 || index >= g_NetworkCount || !g_Ctx.hWlanClient) return;
    WifiNetworkItem* item = &g_NetworkList[index];
    if (item->connState != CONN_STATE_CONNECTED && item->connState != CONN_STATE_CONNECTING) return;
    for (int i = 0; i < g_NetworkCount; i++) {
        if (i != index && (g_NetworkList[i].connState == CONN_STATE_CONNECTING ||
                           g_NetworkList[i].connState == CONN_STATE_ERROR)) {
            g_NetworkList[i].connState = CONN_STATE_IDLE;
            g_NetworkList[i].operationStartTime = 0;
        }
    }
    item->connState = CONN_STATE_DISCONNECTING;
    item->operationStartTime = GetTickCount();
    g_PendingConnectIndex = index;
    if (!g_TimeoutTimer && g_hWndFlyout && IsWindow(g_hWndFlyout)) {
        g_TimeoutTimer = SetTimer(g_hWndFlyout, 1002, 1000, NULL);
    }
    UpdateLayoutGeometry();
    if (g_hWndFlyout) InvalidateRect(g_hWndFlyout, NULL, TRUE);
    DWORD res = WlanDisconnect(g_Ctx.hWlanClient, &item->interfaceGuid, NULL);
    if (res != ERROR_SUCCESS) {
        Wh_Log(L"WlanDisconnect failed: %lu", res);
        item->connState = CONN_STATE_ERROR;
        if (g_PendingConnectIndex == index) g_PendingConnectIndex = -1;
        UpdateLayoutGeometry();
        if (g_hWndFlyout) InvalidateRect(g_hWndFlyout, NULL, TRUE);
    } else {
        LogSsidSafe(L"WlanDisconnect request successful for", item->ssid);
    }
}

void CheckConnectionTimeouts() {
    if (!g_Ctx.hWlanClient) return;
    if (g_PendingConnectIndex < 0 || g_PendingConnectIndex >= g_NetworkCount) {
        if (g_TimeoutTimer && g_hWndFlyout) {
            KillTimer(g_hWndFlyout, g_TimeoutTimer);
            g_TimeoutTimer = 0;
        }
        return;
    }
    WifiNetworkItem* item = &g_NetworkList[g_PendingConnectIndex];
    if (item->operationStartTime == 0) return;
    if (item->connState == CONN_STATE_CONNECTED) {
        LogSsidSafe(L"Timeout check: already connected, clearing pending", item->ssid);
        item->operationStartTime = 0;
        g_PendingConnectIndex = -1;
        if (g_TimeoutTimer && g_hWndFlyout) {
            KillTimer(g_hWndFlyout, g_TimeoutTimer);
            g_TimeoutTimer = 0;
        }
        return;
    }
    if (item->connState == CONN_STATE_ERROR) {
        LogSsidSafe(L"Timeout check: connection already errored, clearing pending", item->ssid);
        item->operationStartTime = 0;
        g_PendingConnectIndex = -1;
        if (g_TimeoutTimer && g_hWndFlyout) {
            KillTimer(g_hWndFlyout, g_TimeoutTimer);
            g_TimeoutTimer = 0;
        }
        if (g_hWndFlyout && IsWindow(g_hWndFlyout)) {
            InvalidateRect(g_hWndFlyout, NULL, TRUE);
            UpdateLayoutGeometry();
        }
        return;
    }
    DWORD now = GetTickCount();
    if (item->connState == CONN_STATE_DISCONNECTING) {
        if ((now - item->operationStartTime) > DISCONNECTION_TIMEOUT_MS) {
            LogSsidSafe(L"Disconnection timeout (no notification received), assuming success for", item->ssid);
            item->connState = CONN_STATE_IDLE;
            item->operationStartTime = 0;
            g_PendingConnectIndex = -1;
            if (g_TimeoutTimer && g_hWndFlyout) {
                KillTimer(g_hWndFlyout, g_TimeoutTimer);
                g_TimeoutTimer = 0;
            }
            if (g_hWndFlyout && IsWindow(g_hWndFlyout)) {
                RefreshNetworkData();
                InvalidateRect(g_hWndFlyout, NULL, TRUE);
                UpdateLayoutGeometry();
            }
        }
        return;
    }
    if ((now - item->operationStartTime) > CONNECTION_TIMEOUT_MS) {
        LogSsidSafe(L"Timeout for", item->ssid);
        Wh_Log(L"  (state=%d)", item->connState);
        item->connState = CONN_STATE_ERROR;
        item->operationStartTime = 0;
        g_PendingConnectIndex = -1;
        if (g_TimeoutTimer && g_hWndFlyout) {
            KillTimer(g_hWndFlyout, g_TimeoutTimer);
            g_TimeoutTimer = 0;
        }
        if (g_hWndFlyout && IsWindow(g_hWndFlyout)) {
            MessageBoxW(g_hWndFlyout, LOC(STR_CONNECTION_TIMEOUT_MSG), 
                       LOC(STR_TIMEOUT_ERROR), MB_OK | MB_ICONWARNING);
            InvalidateRect(g_hWndFlyout, NULL, TRUE);
            UpdateLayoutGeometry();
        }
    }
}

// Enum-based live refresh removed. Using INetworkListManagerEvents instead.

void WINAPI WlanNotificationCallback(PWLAN_NOTIFICATION_DATA data, PVOID context) {
    ModContext* ctx = (ModContext*)context;
    if (!ctx || ctx->isUninitializing || !data) return;
    if (data->NotificationSource != WLAN_NOTIFICATION_SOURCE_ACM) return;
    HWND hFlyout = g_hWndFlyout;
    // refreshNetworkCenter removed — using INetworkListManagerEvents instead
    EnterCriticalSection(&ctx->csLock);
    switch (data->NotificationCode) {
        case wlan_notification_acm_connection_start:
            Wh_Log(L"WLAN: Connection Start");
            break;
        case wlan_notification_acm_connection_complete: {
            PWLAN_CONNECTION_NOTIFICATION_DATA connData = 
                (PWLAN_CONNECTION_NOTIFICATION_DATA)data->pData;
            Wh_Log(L"WLAN: Connection Complete - Profile: %s, ReasonCode: %lu (0x%08X)", 
                   connData->strProfileName, connData->wlanReasonCode, connData->wlanReasonCode);
            if (hFlyout && IsWindow(hFlyout)) PostMessageW(hFlyout, WM_ASYNC_CONNECT_COMPLETE,
                         (connData->wlanReasonCode == ERROR_SUCCESS) ? 1 : 0,
                         (LPARAM)connData->wlanReasonCode);
            // refresh now handled by INetworkListManagerEvents
            break;
        }
        case wlan_notification_acm_connection_attempt_fail: {
            PWLAN_CONNECTION_NOTIFICATION_DATA connData = 
                (PWLAN_CONNECTION_NOTIFICATION_DATA)data->pData;
            Wh_Log(L"WLAN: Connection Attempt Failed (intermediate), Reason: %lu", 
                   connData->wlanReasonCode);
            break;
        }
        case wlan_notification_acm_disconnected: {
            PWLAN_CONNECTION_NOTIFICATION_DATA discData = 
                (PWLAN_CONNECTION_NOTIFICATION_DATA)data->pData;
            Wh_Log(L"WLAN: Disconnected (reason: %lu), g_PendingConnectIndex=%d", 
                   discData->wlanReasonCode, g_PendingConnectIndex);
            if (hFlyout && IsWindow(hFlyout)) PostMessageW(hFlyout, WM_ASYNC_CONNECT_COMPLETE, 0, (LPARAM)ERROR_SUCCESS);
            // refresh now handled by INetworkListManagerEvents
            if (g_TimeoutTimer && hFlyout) {
                PostMessageW(hFlyout, WM_TIMER, 1002, 0);
            }
            break;
        }
        case wlan_notification_acm_scan_complete:
            Wh_Log(L"WLAN: Scan complete");
            break;
        case wlan_notification_acm_scan_fail: {
            DWORD scanFailReason = (data->pData && data->dwDataSize >= sizeof(DWORD))
                ? *(DWORD*)data->pData : 0;
            Wh_Log(L"WLAN: Scan failed, reason: %lu", scanFailReason);
            break;
        }
    }
    LeaveCriticalSection(&ctx->csLock);
    // Live refresh now handled by INetworkListManagerEvents connectivity callback
    if (hFlyout && IsWindow(hFlyout)) PostMessageW(hFlyout, WM_REFRESH_DATA, 0, 0);
}

static void DrawIconBicubic(HDC hdc, int x, int y, int w, int h, HICON hIcon, void** ppCached) {
    if (!hIcon) return;
    if (!g_hGdiPlus || !pGdipCreateBitmapFromHICON || !pGdipSetInterpolationMode) {
        DrawIconEx(hdc, x, y, hIcon, w, h, 0, NULL, DI_NORMAL);
        return;
    }
    void* srcBitmap = ppCached ? *ppCached : NULL;
    if (!srcBitmap && ppCached) {
        if (pGdipCreateBitmapFromHICON(hIcon, &srcBitmap) == 0 && srcBitmap) {
            *ppCached = srcBitmap;
        }
    }
    if (!srcBitmap) {
        DrawIconEx(hdc, x, y, hIcon, w, h, 0, NULL, DI_NORMAL);
        return;
    }
    void* dstBitmap = NULL;
    if (pGdipCreateBitmapFromScan0(w, h, 0, 0x00E200B, NULL, &dstBitmap) != 0 || !dstBitmap) {
        DrawIconEx(hdc, x, y, hIcon, w, h, 0, NULL, DI_NORMAL);
        return;
    }
    void* gfx = NULL;
    if (pGdipGetImageGraphicsContext(dstBitmap, &gfx) == 0 && gfx) {
        pGdipSetInterpolationMode(gfx, 7); 
        pGdipSetPixelOffsetMode(gfx, 3);   
        pGdipGraphicsClear(gfx, 0);
        pGdipDrawImageRectI(gfx, srcBitmap, 0, 0, w, h);
        pGdipDeleteGraphics(gfx);
        HBITMAP hBmp = NULL;
        if (pGdipCreateHBITMAPFromBitmap(dstBitmap, &hBmp, 0) == 0 && hBmp) {
            HDC hdcMem = CreateCompatibleDC(hdc);
            if (hdcMem) {
                HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hBmp);
                BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
                static BOOL alphaBlendLoaded = FALSE;
                static BOOL (WINAPI *pAlphaBlend)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION) = NULL;
                if (!alphaBlendLoaded) {
                    HMODULE hMsImg32 = LoadLibraryExW(L"msimg32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
                    if (hMsImg32) pAlphaBlend = (BOOL (WINAPI *)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION))GetProcAddress(hMsImg32, "AlphaBlend");
                    alphaBlendLoaded = TRUE;
                }
                if (pAlphaBlend) {
                    pAlphaBlend(hdc, x, y, w, h, hdcMem, 0, 0, w, h, bf);
                } else {
                    BitBlt(hdc, x, y, w, h, hdcMem, 0, 0, SRCCOPY);
                }
                SelectObject(hdcMem, hOldBmp);
                DeleteDC(hdcMem);
            }
            DeleteObject(hBmp);
        }
    }
    pGdipDisposeImage(dstBitmap);
}

void DrawNativeSignalIcon(HDC hdc, int right, int top, ULONG quality) {
    int idx = 0;
    if      (quality > 80) idx = 5;
    else if (quality > 60) idx = 4;
    else if (quality > 40) idx = 3;
    else if (quality > 20) idx = 2;
    else if (quality > 0)  idx = 1;
    int iconSize = ScaleDpi(20);
    int xPos = right - iconSize - 4;
    int yPos = top + (ScaleDpi(30) - iconSize) / 2;  // ROW_HEIGHT_NORMAL_BASE=30 (van.dll)
    if (g_hIconSignalBars[idx]) {
        DrawIconBicubic(hdc, xPos, yPos, iconSize, iconSize,
                        g_hIconSignalBars[idx], &g_pBitmapSignalBars[idx]);
    }
}

#define TOOLTIP_FADE_TIMER_ID  9100
#define TOOLTIP_FADE_STEP      40    
#define TOOLTIP_FADE_INTERVAL  20    
static BYTE  g_ttAlpha     = 255;
static BOOL  g_ttFading    = FALSE;

static LRESULT CALLBACK TooltipSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR) {
    if (uMsg == WM_SHOWWINDOW && wParam) {
        g_ttAlpha  = 0;
        g_ttFading = TRUE;
        SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
        SetTimer(hWnd, TOOLTIP_FADE_TIMER_ID, TOOLTIP_FADE_INTERVAL, NULL);
    }
    if (uMsg == WM_TIMER && wParam == TOOLTIP_FADE_TIMER_ID) {
        if (g_ttAlpha < (BYTE)(255 - TOOLTIP_FADE_STEP)) {
            g_ttAlpha = (BYTE)(g_ttAlpha + TOOLTIP_FADE_STEP);
            SetLayeredWindowAttributes(hWnd, 0, g_ttAlpha, LWA_ALPHA);
        } else {
            KillTimer(hWnd, TOOLTIP_FADE_TIMER_ID);
            g_ttAlpha  = 255;
            g_ttFading = FALSE;
            SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
        }
        return 0;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void InitTooltip(HWND hwnd) {
    if (g_hTooltip) return;
    g_hTooltip = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED, TOOLTIPS_CLASS, NULL,
        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        hwnd, NULL, GetModuleHandle(NULL), NULL);
    SetLayeredWindowAttributes(g_hTooltip, 0, 255, LWA_ALPHA);
    WindhawkUtils::SetWindowSubclassFromAnyThread(g_hTooltip, TooltipSubclassProc, 0);
    SendMessage(g_hTooltip, TTM_SETMAXTIPWIDTH,   0, 300);
    SendMessage(g_hTooltip, TTM_SETDELAYTIME, TTDT_AUTOPOP, 10000);
    SendMessage(g_hTooltip, TTM_SETDELAYTIME, TTDT_INITIAL, 400);
    SendMessage(g_hTooltip, TTM_SETDELAYTIME, TTDT_RESHOW,  200);
    if (g_Settings.theme == 1) {
        SendMessage(g_hTooltip, TTM_SETTIPBKCOLOR,   (WPARAM)RGB(30, 30, 30),   0);
        SendMessage(g_hTooltip, TTM_SETTIPTEXTCOLOR, (WPARAM)RGB(100, 200, 255), 0);
        SetWindowTheme(g_hTooltip, L"DarkMode_Explorer", NULL);
    }
}

// Track the last tooltip uId so we can delete just that one instead of all 50
static UINT_PTR g_lastTooltipId = 0;

void UpdateTooltipForRow(HWND hwnd, int index) {
    if (!g_hTooltip) InitTooltip(hwnd);
    // Delete only the previously registered tool instead of all 50
    if (g_lastTooltipId) {
        TOOLINFOW ti = {0};
        ti.cbSize = sizeof(TOOLINFOW);
        ti.hwnd   = hwnd;
        ti.uId    = g_lastTooltipId;
        SendMessage(g_hTooltip, TTM_DELTOOL, 0, (LPARAM)&ti);
        g_lastTooltipId = 0;
    }
    if (index < 0 || index >= g_NetworkCount) return;
    WifiNetworkItem* item = &g_NetworkList[index];
    WCHAR ssidBuf[33];
    GetDisplaySSID(index, ssidBuf, 33);
    const WCHAR* statusText;
    switch (item->connState) {
        case CONN_STATE_CONNECTED:    statusText = LOC(STR_STATUS_CONNECTED); break;
        case CONN_STATE_CONNECTING:   statusText = LOC(STR_STATUS_CONNECTING); break;
        case CONN_STATE_DISCONNECTING: statusText = LOC(STR_DISCONNECTING); break;
        default:                      statusText = LOC(STR_STATUS_NOT_CONNECTED); break;
    }
    StringCchPrintfW(g_TooltipBuffer, 1024,
        L"SSID: %s\n%s %s\n%s %s\n%s",
        ssidBuf,
        LOC(STR_SIGNAL_STRENGTH), SignalQualityToString(item->signalQuality),
        LOC(STR_SECURITY_TYPE), item->isSecured ? L"WPA2-PSK" : L"Open",
        statusText);
    RECT rcRow;
    if (!GetRowRect(index, &rcRow)) return;
    TOOLINFOW ti = {0};
    ti.cbSize   = sizeof(TOOLINFOW);
    ti.uFlags   = TTF_SUBCLASS;
    ti.hwnd     = hwnd;
    ti.uId      = (UINT_PTR)(index + 1);
    ti.lpszText = g_TooltipBuffer;
    ti.rect     = rcRow;
    SendMessage(g_hTooltip, TTM_ADDTOOL, 0, (LPARAM)&ti);
    g_lastTooltipId = (UINT_PTR)(index + 1);
}

static int GetTotalListHeight() {
    int h = 0;
    for (int i = 0; i < g_NetworkCount; i++)
        h += (i == g_SelectedRowIndex) ? ROW_HEIGHT_EXPANDED : ROW_HEIGHT_NORMAL;
    return h;
}

static void ClampScrollPos() {
    int totalHeight = GetTotalListHeight();
    int visibleHeight = LIST_Y_END - LIST_Y_START;
    int maxScroll = (totalHeight > visibleHeight) ? (totalHeight - visibleHeight) : 0;
    if (g_ScrollPos > maxScroll) g_ScrollPos = maxScroll;
    if (g_ScrollPos < 0) g_ScrollPos = 0;
}

BOOL GetRowRect(int index, RECT* rcRow) {
    BOOL showWifiList = (g_NetworkCount > 0);
    if (!showWifiList || index < 0 || index >= g_NetworkCount || !g_bListExpanded) return FALSE;
    int y = LIST_Y_START;
    for (int i = 0; i < index; i++)
        y += (i == g_SelectedRowIndex) ? ROW_HEIGHT_EXPANDED : ROW_HEIGHT_NORMAL;
    y -= g_ScrollPos;
    int rowHeight = (index == g_SelectedRowIndex) ? ROW_HEIGHT_EXPANDED : ROW_HEIGHT_NORMAL;
    if (y + rowHeight <= LIST_Y_START) return FALSE;
    if (y >= LIST_Y_END) return FALSE;
    rcRow->left   = 10;
    rcRow->top    = y;
    rcRow->right  = WINDOW_WIDTH - 10;
    int bottom = y + rowHeight;
    if (bottom > LIST_Y_END) bottom = LIST_Y_END;
    rcRow->bottom = bottom;
    return TRUE;
}

int HitTestRows(int x, int y) {
    BOOL showWifiList = (g_NetworkCount > 0);
    if (!showWifiList) return -1;
    for (int i = 0; i < g_NetworkCount; i++) {
        RECT rc;
        if (GetRowRect(i, &rc) && x>=rc.left && x<=rc.right && y>=rc.top && y<=rc.bottom) return i;
    }
    return -1;
}

typedef struct {
    int  buttonCount;
    int  networkId;
    BOOL valid;
} ToolbarScanCache;

static ToolbarScanCache g_ToolbarCache = {0, -1, FALSE};
static void InvalidateToolbarCache() {
    g_ToolbarCache.valid = FALSE;
}

// Cache netcenter.dll base+size for caller-module range checking (DrawTextW_Hook)
static BYTE* g_netcenterBase = NULL;
static BYTE* g_netcenterEnd  = NULL;

// netcenter.dll is loaded on demand (only once the Network and Sharing
// Center page is actually opened), so g_netcenterBase/g_netcenterEnd can't
// be resolved once up front in Wh_ModInit like g_pniduiBase is - at that
// point the module usually isn't loaded yet, so they were previously left
// permanently NULL and IsInNetCenter() always returned false. Retry the
// lookup lazily instead, the same way InitPniduiInfo() resolves pnidui.dll,
// caching the result once it succeeds.
static bool EnsureNetCenterRange() {
    if (g_netcenterBase && g_netcenterEnd)
        return true;

    HMODULE h = GetModuleHandleW(L"netcenter.dll");
    if (!h)
        return false;

    MODULEINFO mi{};
    if (!GetModuleInformation(GetCurrentProcess(), h, &mi, sizeof(mi)))
        return false;

    g_netcenterBase = (BYTE*)mi.lpBaseOfDll;
    g_netcenterEnd  = g_netcenterBase + mi.SizeOfImage;
    return true;
}

static bool IsInNetCenter(void* ra) {
    if (!(g_netcenterBase && g_netcenterEnd) && !EnsureNetCenterRange())
        return false;
    return ra >= (void*)g_netcenterBase && ra < (void*)g_netcenterEnd;
}

static bool InitPniduiInfo() {
    if (g_pniduiBase) return true;

    // Bare-name lookup: matches pnidui.dll regardless of where it was loaded
    // from (AppData, ProgramData, mod folder, System32, or native on Win10),
    // so there's no need to also probe the ExplorerPatcher install path
    // first - GetModuleHandleW matches by base name, not by the directory
    // the module actually loaded from.
    HMODULE hPnidui = GetModuleHandleW(L"pnidui.dll");
    if (hPnidui) {
        Wh_Log(L"pnidui.dll found via simple name lookup");
        goto found;
    }

    // Not found in any location - log gracefully and continue without icon detection
    Wh_Log(L"pnidui.dll not found in any known location");
    Wh_Log(L"Network icon detection will be unavailable");
    return false;

found:
    // Get module information (base address and size)
    MODULEINFO mi{};
    if (!GetModuleInformation(GetCurrentProcess(), hPnidui, &mi, sizeof(mi))) {
        Wh_Log(L"GetModuleInformation failed for pnidui.dll (error: %lu)", GetLastError());
        return false;
    }
    
    g_pniduiBase = (BYTE*)mi.lpBaseOfDll;
    g_pniduiEnd  = g_pniduiBase + mi.SizeOfImage;
    Wh_Log(L"pnidui.dll loaded at %p-%p (size: %lu bytes)", 
           g_pniduiBase, g_pniduiEnd, mi.SizeOfImage);
    
    return true;
}

static BOOL IsNetworkButton(HWND hToolbar, int buttonIndex) {
    if (buttonIndex < 0 || !g_pniduiBase) return FALSE;
    TBBUTTON tb{};
    if (!SendMessageW(hToolbar, TB_GETBUTTON, (WPARAM)buttonIndex, (LPARAM)&tb)) {
        return FALSE;
    }
    if (!tb.dwData) return FALSE;
    HWND hIconWnd = *(HWND*)tb.dwData;
    if (!hIconWnd || !IsWindow(hIconWnd)) return FALSE;
    WCHAR className[256]{};
    if (!GetClassNameW(hIconWnd, className, ARRAYSIZE(className))) return FALSE;
    if (wcsncmp(className, L"ATL:", 4) != 0) return FALSE;
    const WCHAR* hexPart = className + 4;
    ULONG_PTR addr = 0;
    while (*hexPart) {
        WCHAR c = *hexPart;
        int digit = 0;
        if      (c >= L'0' && c <= L'9') digit = c - L'0';
        else if (c >= L'A' && c <= L'F') digit = 10 + (c - L'A');
        else if (c >= L'a' && c <= L'f') digit = 10 + (c - L'a');
        else break;
        addr = (addr << 4) | digit;
        hexPart++;
    }
    return (addr >= (ULONG_PTR)g_pniduiBase && addr < (ULONG_PTR)g_pniduiEnd);
}

void RecalcArrowRect() {
    int labelMidY = WIFI_LABEL_Y + (HEADER_HEIGHT - WIFI_LABEL_Y) / 2;
    int btnH = ScaleDpi(16), btnW = ScaleDpi(16);
    int totalHeight = GetTotalListHeight();
    int visibleHeight = LIST_Y_END - LIST_Y_START;
    int scrollbarOffset = (totalHeight > visibleHeight) ? ScaleDpi(15) : 0;
    int margineDestroFreccia = ScaleDpi(20) + scrollbarOffset;
    g_rcArrowButton.right  = WINDOW_WIDTH - margineDestroFreccia;
    g_rcArrowButton.left   = g_rcArrowButton.right - btnW;
    g_rcArrowButton.top    = labelMidY - btnH/2;
    g_rcArrowButton.bottom = labelMidY + btnH/2;
}

void UpdateLayoutGeometry(int scrollbarOffset) {
    if (!SafeToAccessUI()) return;
    BOOL showWifiList = (g_NetworkCount > 0);
    if (!showWifiList || g_SelectedRowIndex < 0 || g_SelectedRowIndex >= g_NetworkCount) {
        if (g_hWndButtonConnect && IsWindow(g_hWndButtonConnect))   
            ShowWindow(g_hWndButtonConnect, SW_HIDE);
        if (g_hWndCheckboxConnect && IsWindow(g_hWndCheckboxConnect)) 
            ShowWindow(g_hWndCheckboxConnect, SW_HIDE);
        g_bShowCheckboxLabel = FALSE;
        return;
    }
    int rowY = LIST_Y_START;
    for (int i = 0; i < g_SelectedRowIndex; i++) {
        rowY += (i == g_SelectedRowIndex) ? ROW_HEIGHT_EXPANDED : ROW_HEIGHT_NORMAL;
    }
    int rowYRelative = rowY - g_ScrollPos;
    int rowHeight = ROW_HEIGHT_EXPANDED;
    WifiNetworkItem* item = &g_NetworkList[g_SelectedRowIndex];
    BOOL isConnected = (item->connState == CONN_STATE_CONNECTED);
    BOOL isConnecting = (item->connState == CONN_STATE_CONNECTING || 
                         item->connState == CONN_STATE_DISCONNECTING);
    if (rowYRelative + rowHeight <= LIST_Y_START || rowYRelative >= LIST_Y_END) {
        if (g_hWndButtonConnect && IsWindow(g_hWndButtonConnect))   
            ShowWindow(g_hWndButtonConnect, SW_HIDE);
        if (g_hWndCheckboxConnect && IsWindow(g_hWndCheckboxConnect)) 
            ShowWindow(g_hWndCheckboxConnect, SW_HIDE);
        g_bShowCheckboxLabel = FALSE;
        return;
    }
    int btnX = WINDOW_WIDTH - 114 - scrollbarOffset;  
    int chkX = 18;
    int chkYOffset = 0;
    if (scrollbarOffset > 0) {
        chkX -= (WINDOW_WIDTH * 19) / 1000;   
        chkX -= (WINDOW_WIDTH * 5) / 1000;    
        chkYOffset -= (WINDOW_HEIGHT * 13) / 1000;  
    }
    int btnY = rowYRelative + 35;  
    int chkY = rowYRelative + 36 + chkYOffset;
    if (btnY < LIST_Y_START) btnY = LIST_Y_START + 2;
    if (btnY > LIST_Y_END - 24) btnY = LIST_Y_END - 24;
    if (chkY < LIST_Y_START) chkY = LIST_Y_START + 2;
    if (chkY > LIST_Y_END - 22) chkY = LIST_Y_END - 22;
    if (g_hWndCheckboxConnect && IsWindow(g_hWndCheckboxConnect)) {
        if (!isConnected && !isConnecting) {
            int boxSize = ScaleDpi(13);
            int chkNativeW = boxSize + ScaleDpi(4);
            MoveWindow(g_hWndCheckboxConnect, chkX, chkY, chkNativeW, 20, TRUE);
            ShowWindow(g_hWndCheckboxConnect, SW_SHOW);
            g_rcCheckboxLabel.left   = chkX + boxSize + ScaleDpi(5);
            g_rcCheckboxLabel.top    = chkY;
            g_rcCheckboxLabel.right  = chkX + 160;
            g_rcCheckboxLabel.bottom = chkY + 20;
            g_bShowCheckboxLabel = TRUE;
        } else {
            ShowWindow(g_hWndCheckboxConnect, SW_HIDE);
            g_bShowCheckboxLabel = FALSE;
        }
    }
    if (g_hWndButtonConnect && IsWindow(g_hWndButtonConnect)) {
        // Keep the button's rect fixed across all three states (only the
        // text/enabled state changes). Previously the "connecting" state
        // used a smaller, shifted rect (btnX+50, width 40) while
        // Connect/Disconnect used the full rect (btnX, width 92) - resizing
        // between them briefly exposed the parent's background (white)
        // before the button repainted, since MoveWindow uncovers the old
        // area for one frame. A fixed rect means there's nothing to expose.
        MoveWindow(g_hWndButtonConnect, btnX, btnY, 92, 22, TRUE);
        if (isConnecting) {
            SetWindowTextW(g_hWndButtonConnect, L"...");
            ShowWindow(g_hWndButtonConnect, SW_SHOW);
            EnableWindow(g_hWndButtonConnect, FALSE);
        } else if (isConnected) {
            SetWindowTextW(g_hWndButtonConnect, LOC(STR_BTN_DISCONNECT));
            ShowWindow(g_hWndButtonConnect, SW_SHOW);
            EnableWindow(g_hWndButtonConnect, TRUE);
        } else {
            SetWindowTextW(g_hWndButtonConnect, LOC(STR_BTN_CONNECT));
            ShowWindow(g_hWndButtonConnect, SW_SHOW);
            EnableWindow(g_hWndButtonConnect, TRUE);
        }
    }
}

void ShowContextMenu(HWND hwnd, int itemIndex, POINT pt) {
    if (itemIndex < 0 || itemIndex >= g_NetworkCount) return;
    g_ContextMenuTargetIndex = itemIndex;
    WifiNetworkItem* item = &g_NetworkList[itemIndex];
    HMENU hMenu = CreatePopupMenu();
    if (item->connState == CONN_STATE_CONNECTED) {
        AppendMenuW(hMenu, MF_STRING, IDM_DISCONNECT, LOC(STR_CTX_DISCONNECT));
        AppendMenuW(hMenu, MF_STRING, IDM_STATUS,     LOC(STR_CTX_STATUS));
    } else if (item->connState == CONN_STATE_CONNECTING) {
        AppendMenuW(hMenu, MF_STRING | MF_GRAYED, IDM_CONNECT, LOC(STR_CONNECTING));
    } else {
        AppendMenuW(hMenu, MF_STRING, IDM_CONNECT, LOC(STR_CTX_CONNECT));
    }
    AppendMenuW(hMenu, MF_STRING, IDM_PROPERTIES, LOC(STR_CTX_PROPERTIES));
    if (g_Settings.theme == 1) {
        DarkContextMenu::Apply(TRUE);
    }
    int cmd = TrackPopupMenu(hMenu, TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, pt.x, pt.y, 0, hwnd, NULL);
    if (g_Settings.theme == 1) {
        DarkContextMenu::Apply(FALSE);
    }
    if (cmd > 0) {
        switch (cmd) {
        case IDM_CONNECT:
            ConnectToNetwork(g_ContextMenuTargetIndex);
            break;
        case IDM_DISCONNECT:
            DisconnectFromNetwork(g_ContextMenuTargetIndex);
            break;
        case IDM_STATUS:
        case IDM_PROPERTIES:
            ShellExecuteW(NULL, L"open", L"explorer.exe", L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}", NULL, SW_SHOWNORMAL);
            ShowWindow(hwnd, SW_HIDE);
            break;
        }
    }
    DestroyMenu(hMenu);
}

static RECT GetFooterRect() {
    RECT rc = { 0, WINDOW_HEIGHT - FOOTER_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT };
    return rc;
}

void EnsureRowVisible(int index) {
    BOOL showWifiList = (g_NetworkCount > 0);
    if (!showWifiList || index < 0 || index >= g_NetworkCount) return;
    int visibleHeight = LIST_Y_END - LIST_Y_START;
    int totalHeight = GetTotalListHeight();
    int maxScroll = (totalHeight > visibleHeight) ? (totalHeight - visibleHeight) : 0;
    int y = LIST_Y_START;
    for (int i = 0; i < index; i++)
        y += (i == g_SelectedRowIndex) ? ROW_HEIGHT_EXPANDED : ROW_HEIGHT_NORMAL;
    int rowHeight = (index == g_SelectedRowIndex) ? ROW_HEIGHT_EXPANDED : ROW_HEIGHT_NORMAL;
    int rowTopRel = y - g_ScrollPos;
    int rowBottomRel = rowTopRel + rowHeight;
    if (rowBottomRel > visibleHeight) {
        g_ScrollPos += (rowBottomRel - visibleHeight);
    } else if (rowTopRel < 0) {
        g_ScrollPos += rowTopRel;
    }
    if (totalHeight > visibleHeight) {
        if (g_ScrollPos < maxScroll && rowBottomRel <= visibleHeight) {
            int lastRowBottomAbs = totalHeight;
            int lastRowBottomRel = lastRowBottomAbs - g_ScrollPos;
            if (lastRowBottomRel < visibleHeight) {
                int needed = visibleHeight - lastRowBottomRel;
                int newScroll = g_ScrollPos + needed;
                if (newScroll > maxScroll) newScroll = maxScroll;
                int newRowTopRel = y - newScroll;
                if (newRowTopRel >= 0) {
                    g_ScrollPos = newScroll;
                }
            }
        }
    }
    if (g_ScrollPos > maxScroll) g_ScrollPos = maxScroll;
    if (g_ScrollPos < 0) g_ScrollPos = 0;
    SetScrollPos(g_hWndFlyout, SB_VERT, g_ScrollPos, TRUE);
}

// -------------------------------------------------------
// Flyout Window Procedure
// -------------------------------------------------------
LRESULT CALLBACK FlyoutWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_NCHITTEST: {
        LRESULT r = DefWindowProc(hwnd, uMsg, wParam, lParam);
        switch (r) {
            case HTTOP: case HTTOPLEFT: case HTTOPRIGHT:
            case HTBOTTOM: case HTBOTTOMLEFT: case HTBOTTOMRIGHT:
            case HTLEFT: case HTRIGHT:
                return HTBORDER;
            default: return r;
        }
    }
    case WM_CREATE: {
        HMENU hSysMenu = GetSystemMenu(hwnd, FALSE);
        if (hSysMenu) RemoveMenu(hSysMenu, SC_CLOSE, MF_BYCOMMAND);
        
        if (g_Settings.useRoundedCorners) {
            BOOL pfEnabled = FALSE;
            if (DwmIsCompositionEnabled(&pfEnabled) == S_OK && pfEnabled) {
                DWMNCRENDERINGPOLICY pol = DWMNCRP_ENABLED;
                DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &pol, sizeof(pol));
                DWM_WINDOW_CORNER_PREFERENCE cornerPref = DWMWCP_ROUND;
                DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPref, sizeof(cornerPref));
                MARGINS margins = {0, 0, 0, 1};
                DwmExtendFrameIntoClientArea(hwnd, &margins);
            }
        }
        
        g_hWndButtonConnect = CreateWindowExW(0, WC_BUTTONW, L"",
            WS_CHILD | (g_Settings.theme == 1 ? BS_OWNERDRAW : BS_PUSHBUTTON),
            0,0,0,0, hwnd,(HMENU)IDC_CONN_BUTTON,GetModuleHandle(NULL),NULL);
        SendMessageW(g_hWndButtonConnect, WM_SETFONT, (WPARAM)g_hFontButton, TRUE);
        g_ButtonConnectIsOwnerDraw = (g_Settings.theme == 1);
        
        g_hWndCheckboxConnect = CreateWindowExW(0, WC_BUTTONW, L"",
            WS_CHILD|BS_AUTOCHECKBOX, 0,0,0,0, hwnd,(HMENU)IDC_AUTO_CHECKBOX,GetModuleHandle(NULL),NULL);
        SendMessageW(g_hWndCheckboxConnect, WM_SETFONT, (WPARAM)g_hFontCheckbox, TRUE);
        SendMessageW(g_hWndCheckboxConnect, BM_SETCHECK, BST_CHECKED, 0);
        
        ApplyNativeControlsTheme();
        RecalcArrowRect();
        InterlockedIncrement(&g_Ctx.refCount);
        InitTooltip(hwnd);
        
        if (g_Settings.refreshInterval > 0) {
            g_RefreshTimer = SetTimer(hwnd, 1000, g_Settings.refreshInterval, NULL);
        }
        break;
    }
    case WM_TIMER:
        if (wParam == 1000) {
            RefreshNetworkData();
            ClampScrollPos();
            UpdateLayoutGeometry();
            InvalidateRect(hwnd, NULL, FALSE);
        } else if (wParam == 1002) {
            CheckConnectionTimeouts();
            UpdateLayoutGeometry();
            InvalidateRect(hwnd, NULL, FALSE);
        }
        break;
    case WM_SHOW_FLYOUT:
        ShowWindow(hwnd, SW_SHOW);
        SetForegroundWindow(hwnd);
        RefreshNetworkData();
        UpdateLayoutGeometry();
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    case WM_REFRESH_DATA: {
        RefreshNetworkData(/*forceDetection=*/(BOOL)wParam);
        ClampScrollPos();
        UpdateLayoutGeometry();
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }
    case WM_ASYNC_CONNECT_COMPLETE: {
        BOOL opSuccess = (BOOL)wParam;
        DWORD errorCode = (DWORD)lParam;
        Wh_Log(L"Async connect/disconnect complete: success=%d, error=%lu (0x%08X)", 
               opSuccess, errorCode, errorCode);
        if (!opSuccess && errorCode == ERROR_SUCCESS) {
            if (g_PendingConnectIndex >= 0 && g_PendingConnectIndex < g_NetworkCount) {
                WifiNetworkItem* item = &g_NetworkList[g_PendingConnectIndex];
                if (item->connState == CONN_STATE_DISCONNECTING) {
                    LogSsidSafe(L"Disconnection confirmed by notification for", item->ssid);
                    item->connState = CONN_STATE_IDLE;
                    item->operationStartTime = 0;
                    g_PendingConnectIndex = -1;
                }
            }
            for (int i = 0; i < g_NetworkCount; i++) {
                if (i == g_PendingConnectIndex) continue;
                if (g_NetworkList[i].connState == CONN_STATE_DISCONNECTING ||
                    g_NetworkList[i].connState == CONN_STATE_CONNECTED) {
                    g_NetworkList[i].connState = CONN_STATE_IDLE;
                    g_NetworkList[i].operationStartTime = 0;
                }
            }
            if (g_TimeoutTimer) { KillTimer(hwnd, g_TimeoutTimer); g_TimeoutTimer = 0; }
            RefreshNetworkData();
            UpdateLayoutGeometry();
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        }
        if (opSuccess) {
            if (g_PendingConnectIndex >= 0 && g_PendingConnectIndex < g_NetworkCount) {
                g_NetworkList[g_PendingConnectIndex].connState = CONN_STATE_CONNECTED;
                g_NetworkList[g_PendingConnectIndex].operationStartTime = 0;
                g_PendingConnectIndex = -1;
            }
            if (g_TimeoutTimer) {
                KillTimer(hwnd, g_TimeoutTimer);
                g_TimeoutTimer = 0;
            }
            // Windows handles the "Set Network Location" prompt natively for
            // new networks and writes the category to the registry.  We just
            // re-detect on the next refresh and show the correct icon.
        } else {
            if (g_PendingConnectIndex >= 0 && g_PendingConnectIndex < g_NetworkCount) {
                WifiNetworkItem* item = &g_NetworkList[g_PendingConnectIndex];
                static const DWORD authFailureCodes[] = {
                    0x00038001,  
                    0x00038002,  
                    0x00028001,  
                    0x00028002,  
                    0x00030001,  
                };
                BOOL isAuthFailure = FALSE;
                for (size_t i = 0; i < ARRAYSIZE(authFailureCodes); i++) {
                    if (errorCode == authFailureCodes[i]) {
                        isAuthFailure = TRUE;
                        break;
                    }
                }
                if (isAuthFailure && item->hasProfile) {
                    Wh_Log(L"Auth failure for '%s' (code 0x%08X) - saved password likely wrong, resetting profile", 
                           item->ssid, errorCode);
                    item->hasProfile = FALSE;
                    item->connState = CONN_STATE_ERROR;
                    item->operationStartTime = 0;
                    MessageBoxW(hwnd, LOC(STR_PWD_FAILED_WRONG), LOC(STR_PWD_FAILED_TITLE), 
                               MB_OK | MB_ICONERROR);
                } else if (isAuthFailure && !item->hasProfile) {
                    Wh_Log(L"Auth failure for '%s' (code 0x%08X) - user-entered password was wrong", 
                           item->ssid, errorCode);
                    item->connState = CONN_STATE_ERROR;
                    item->operationStartTime = 0;
                    MessageBoxW(hwnd, LOC(STR_PWD_FAILED_WRONG), LOC(STR_PWD_FAILED_TITLE), 
                               MB_OK | MB_ICONERROR);
                } else {
                    Wh_Log(L"Non-auth failure for '%s' (code 0x%08X) - keeping profile intact", 
                           item->ssid, errorCode);
                    item->connState = CONN_STATE_ERROR;
                    item->operationStartTime = 0;
                    WCHAR errMsg[256];
                    StringCchPrintfW(errMsg, ARRAYSIZE(errMsg), 
                                   LOC(STR_CONNECTION_ERROR), errorCode);
                    MessageBoxW(hwnd, errMsg, LOC(STR_ERROR_TITLE), MB_OK | MB_ICONWARNING);
                }
                g_PendingConnectIndex = -1;
            }
            if (g_TimeoutTimer) {
                KillTimer(hwnd, g_TimeoutTimer);
                g_TimeoutTimer = 0;
            }
        }
        RefreshNetworkData();
        UpdateLayoutGeometry();
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }
    case WM_GETDLGCODE:
        return DLGC_WANTARROWS | DLGC_WANTCHARS;
    case WM_KEYDOWN: {
        BOOL showWifiList = (g_NetworkCount > 0);
        switch (wParam) {
            case VK_UP:
                if (showWifiList && g_bListExpanded && g_NetworkCount > 0) {
                    int newIndex = (g_KeyboardSelectedIndex > 0) ? g_KeyboardSelectedIndex - 1 : g_NetworkCount - 1;
                    SetKeyboardFocus(newIndex);
                    InvalidateRect(hwnd, NULL, TRUE);
                }
                return 0;
            case VK_DOWN:
                if (showWifiList && g_bListExpanded && g_NetworkCount > 0) {
                    int newIndex = (g_KeyboardSelectedIndex < g_NetworkCount - 1) ? g_KeyboardSelectedIndex + 1 : 0;
                    SetKeyboardFocus(newIndex);
                    InvalidateRect(hwnd, NULL, TRUE);
                }
                return 0;
            case VK_RETURN:
                if (showWifiList && g_KeyboardSelectedIndex >= 0 && g_KeyboardSelectedIndex < g_NetworkCount)
                    ConnectToNetwork(g_KeyboardSelectedIndex);
                return 0;
            case VK_LEFT:
                ShowWindow(hwnd, SW_HIDE);
                return 0;
            case VK_RIGHT:
                if (showWifiList && g_KeyboardSelectedIndex >= 0 && g_KeyboardSelectedIndex < g_NetworkCount) {
                    RECT rcRow;
                    if (GetRowRect(g_KeyboardSelectedIndex, &rcRow)) {
                        POINT pt = {rcRow.left + 20, rcRow.top + 13};
                        ClientToScreen(hwnd, &pt);
                        ShowContextMenu(hwnd, g_KeyboardSelectedIndex, pt);
                    }
                }
                return 0;
            case VK_ESCAPE:
                ShowWindow(hwnd, SW_HIDE);
                return 0;
        }
        break;
    }
    case WM_VSCROLL: {
        BOOL showWifiList = (g_NetworkCount > 0);
        if (!showWifiList) break;
        int totalHeight = GetTotalListHeight();
        int visibleHeight = LIST_Y_END - LIST_Y_START;
        int maxScroll = (totalHeight > visibleHeight) ? (totalHeight - visibleHeight) : 0;
        int newPos = g_ScrollPos;
        switch (LOWORD(wParam)) {
            case SB_LINEUP:    newPos -= ROW_HEIGHT_NORMAL; break;
            case SB_LINEDOWN:  newPos += ROW_HEIGHT_NORMAL; break;
            case SB_PAGEUP:    newPos -= visibleHeight; break;
            case SB_PAGEDOWN:  newPos += visibleHeight; break;
            case SB_THUMBTRACK: newPos = HIWORD(wParam); break;
        }
        if (newPos < 0) newPos = 0;
        if (newPos > maxScroll) newPos = maxScroll;
        if (newPos != g_ScrollPos) {
            g_ScrollPos = newPos;
            SetScrollPos(hwnd, SB_VERT, g_ScrollPos, TRUE);
            InvalidateRect(hwnd, NULL, FALSE);
        }
        break;
    }
    case WM_MOUSEWHEEL: {
        BOOL showWifiList = (g_NetworkCount > 0);
        if (!showWifiList) break;
        int totalHeight = GetTotalListHeight();
        int visibleHeight = LIST_Y_END - LIST_Y_START;
        int maxScroll = (totalHeight > visibleHeight) ? (totalHeight - visibleHeight) : 0;
        int newPos = g_ScrollPos - (GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA) * ROW_HEIGHT_NORMAL;
        if (newPos < 0) newPos = 0;
        if (newPos > maxScroll) newPos = maxScroll;
        if (newPos != g_ScrollPos) {
            g_ScrollPos = newPos;
            SetScrollPos(hwnd, SB_VERT, g_ScrollPos, TRUE);
            InvalidateRect(hwnd, NULL, FALSE);
        }
        break;
    }
    case WM_ERASEBKGND: {
        HDC hdcErase = (HDC)wParam;
        RECT rcClient;
        GetClientRect(hwnd, &rcClient);
        HBRUSH hBrErase = CreateSolidBrush(GetContentBgColor());
        FillRect(hdcErase, &rcClient, hBrErase);
        DeleteObject(hBrErase);
        return 1;
    }
    case WM_PAINT: {
        if (!SafeToAccessUI()) break;
        PAINTSTRUCT ps;
        HDC hdcReal = BeginPaint(hwnd, &ps);

        if (!g_hdcMemPaint || g_memPaintWidth != WINDOW_WIDTH || g_memPaintHeight != WINDOW_HEIGHT) {
            if (g_hdcMemPaint) { DeleteDC(g_hdcMemPaint); g_hdcMemPaint = NULL; }
            if (g_hbmMemPaint) { DeleteObject(g_hbmMemPaint); g_hbmMemPaint = NULL; }
            g_hdcMemPaint = CreateCompatibleDC(hdcReal);
            g_hbmMemPaint = CreateCompatibleBitmap(hdcReal, WINDOW_WIDTH, WINDOW_HEIGHT);
            g_memPaintWidth  = WINDOW_WIDTH;
            g_memPaintHeight = WINDOW_HEIGHT;
        }
        HDC     hdc     = g_hdcMemPaint;
        HBITMAP hOldBmp = (HBITMAP)SelectObject(hdc, g_hbmMemPaint);
        
        RECT rcHeader  = {0, 0, WINDOW_WIDTH, HEADER_HEIGHT};
        HBRUSH hBrH = CreateSolidBrush(GetHeaderBgColor()); FillRect(hdc, &rcHeader, hBrH); DeleteObject(hBrH);
        RECT rcContent = {0, HEADER_HEIGHT, WINDOW_WIDTH, LIST_Y_END};
        HBRUSH hBrC = CreateSolidBrush(GetContentBgColor()); FillRect(hdc, &rcContent, hBrC); DeleteObject(hBrC);
        RECT rcFooter = GetFooterRect();
        HBRUSH hBrF = CreateSolidBrush(GetFooterBgColor());
        FillRect(hdc, &rcFooter, hBrF); DeleteObject(hBrF);

        BOOL showWifiList = (g_NetworkCount > 0);
        
        if (showWifiList) {
            int totalHeight = GetTotalListHeight();
            int visibleHeight = LIST_Y_END - LIST_Y_START;
            SCROLLINFO si = { sizeof(SCROLLINFO), SIF_RANGE | SIF_PAGE | SIF_POS, 0, totalHeight, (UINT)visibleHeight, g_ScrollPos };
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
        }

        // Separator line between connection info and the WiFi list header label.
        // Drawn ABOVE the "Wireless Network Connection" label.
        int separatorY = showWifiList ? (WIFI_LABEL_Y - ScaleDpi(4)) : HEADER_HEIGHT;
        HPEN hPenSep = CreatePen(PS_SOLID, 1, (g_Settings.theme == 1) ? RGB(70,70,75) : RGB(214,223,234));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPenSep);
        MoveToEx(hdc, 0, separatorY, NULL); LineTo(hdc, WINDOW_WIDTH, separatorY);
        SelectObject(hdc, hOldPen); DeleteObject(hPenSep);
        
        HPEN hPenBevelDark  = CreatePen(PS_SOLID, 1, (g_Settings.theme == 1) ? RGB(55,55,60)  : RGB(180,193,210));
        HPEN hPenBevelLight = CreatePen(PS_SOLID, 1, (g_Settings.theme == 1) ? RGB(80,80,85)  : RGB(255,255,255));
        SelectObject(hdc, hPenBevelDark);
        MoveToEx(hdc, 0, LIST_Y_END,     NULL); LineTo(hdc, WINDOW_WIDTH, LIST_Y_END);
        SelectObject(hdc, hPenBevelLight);
        MoveToEx(hdc, 0, LIST_Y_END + 1, NULL); LineTo(hdc, WINDOW_WIDTH, LIST_Y_END + 1);
        SelectObject(hdc, hOldPen);
        DeleteObject(hPenBevelDark);
        DeleteObject(hPenBevelLight);

        BOOL isWifiConnected = (g_NetworkCount > 0 && g_NetworkList[0].connState == CONN_STATE_CONNECTED);
        BOOL isAnyConnected = (g_EthernetConnected || isWifiConnected);
        SetBkMode(hdc, TRANSPARENT);
        
        if (isAnyConnected) {
            SelectObject(hdc, g_hFontNormal); SetTextColor(hdc, GetTextColor());
            TextOutW(hdc, ScaleDpi(10), ScaleDpi(10), LOC(STR_CURRENT_CONNECTED), lstrlenW(LOC(STR_CURRENT_CONNECTED)));
            SelectObject(hdc, g_hFontBold);
            SetTextColor(hdc, GetTextColor());
            
            WCHAR displayName[64] = {0};
            BOOL showEthernetInHeader = g_EthernetConnected;
            if (showEthernetInHeader) {
                if (g_Settings.privacyMode) {
                    StringCchPrintfW(displayName, ARRAYSIZE(displayName), LOC(STR_NETWORK_PRIVACY_FMT), 1);
                } else {
                    StringCchCopyW(displayName, ARRAYSIZE(displayName), g_EthernetNetworkName);
                    if (displayName[0] == L'\0') {
                        StringCchPrintfW(displayName, ARRAYSIZE(displayName), LOC(STR_NETWORK_PRIVACY_FMT), 2);
                    }
                }
            } else {
                GetDisplaySSID(0, displayName, 33);
            }
            
            DrawTextWithWrap(hdc, displayName, ScaleDpi(56), ScaleDpi(36), WINDOW_WIDTH - ScaleDpi(70), ScaleDpi(18));
            SelectObject(hdc, g_hFontNormal);
            SetTextColor(hdc, (g_Settings.theme == 1) ? RGB(200, 200, 200) : RGB(0, 0, 0));
            TextOutW(hdc, ScaleDpi(56), ScaleDpi(52), LOC(STR_INTERNET_ACCESS), lstrlenW(LOC(STR_INTERNET_ACCESS)));
        } else {
            SelectObject(hdc, g_hFontNormal); SetTextColor(hdc, GetTextColor());
            TextOutW(hdc, ScaleDpi(10), ScaleDpi(10), LOC(STR_NO_CONNECTIONS), lstrlenW(LOC(STR_NO_CONNECTIONS)));
            // This is a status line, not a selected network name: keep it
            // normal weight. 46rp includes the prior adjustments plus a further
            // ~7% downward adjustment in the disconnected header.
            SelectObject(hdc, g_hFontNormal);
            SetTextColor(hdc, GetTextColor());
            TextOutW(hdc, ScaleDpi(55), ScaleDpi(46), LOC(STR_CONNECTIONS_AVAILABLE), lstrlenW(LOC(STR_CONNECTIONS_AVAILABLE)));
        }
        
        int iconSize = ScaleDpi(35);
        // 36rp is the nearest whole-pixel equivalent to a 2.5% increase over
        // the normal 35rp header artwork at the baseline DPI.
        int availableIconSize = ScaleDpi(36);
        BOOL drawAvailableIcon = FALSE;
        HICON hLargeIcon = NULL;
        if (isAnyConnected) {
            if (g_Settings.useNetworkLocationIcons) {
                // Show the network location icon (Home / Public / Work) based on
                // the active NLM_NETWORK_CATEGORY detected via INetworkListManager.
                // Mutually exclusive: exactly one icon per active connection.
                // If base64 decoding failed, falls back to the PC icon.
                hLargeIcon = GetNetworkLocationIcon(NULL);
            } else {
                // User prefers the original generic PC/network icon
                hLargeIcon = g_hIconNetworkMap;
            }
        } else if (g_NetworkCount > 0) {
            // Networks are available but none is connected.
            hLargeIcon = g_hIconAvailable ? g_hIconAvailable : g_hIconSignalBars[0];
            drawAvailableIcon = (g_hIconAvailable != NULL);
        } else {
            // No Wi-Fi networks are available and no Ethernet connection is active.
            hLargeIcon = g_hIconDisconnected ? g_hIconDisconnected : g_hIconSignalBars[0];
        }
        // ICON_HEADER_Y_OFFSET is the -3 fine-tune used since the original
        // layout; kept as a named constant rather than a bare magic number so
        // it stays self-explanatory. Drawn via plain DrawIconEx: bicubic
        // GDI+ scaling was tried here but corrupted the Home/Public/Work
        // (bench) icon artwork, so this reverts to the original draw path.
        const int ICON_HEADER_Y_OFFSET = -3;
        if (hLargeIcon) {
            int drawSize = drawAvailableIcon ? availableIconSize : iconSize;
            // Keep the slightly larger available-networks artwork visually centered.
            int centeringOffset = (drawSize - iconSize) / 2;
            DrawIconEx(hdc, ScaleDpi(12) - centeringOffset,
                       ScaleDpi(37) + ICON_HEADER_Y_OFFSET - centeringOffset,
                       hLargeIcon, drawSize, drawSize, 0, NULL, DI_NORMAL);
        }
        if (showWifiList) {
            int totalHeight = GetTotalListHeight();
            int visibleHeight = LIST_Y_END - LIST_Y_START;
            BOOL hasScrollbar = (totalHeight > visibleHeight);
            int scrollbarOffset = hasScrollbar ? ScaleDpi(13) : 0;
            int roundedCornersOffset = g_Settings.useRoundedCorners ? (WINDOW_WIDTH * 2) / 100 : 0;
            // A visible scrollbar would otherwise move the control left by
            // its reserved width. Compensate with a DPI-independent 3% shift (1% left of the prior position).
            int scrollbarShift = hasScrollbar ? ((WINDOW_WIDTH * 3) / 100)
                                             : (((WINDOW_WIDTH * 4) / 100) - ((WINDOW_WIDTH * 13) / 1000));
            int refreshLeftOffset = GetRefreshButtonLeftOffset();
            g_rcRefreshButton.right = WINDOW_WIDTH - ScaleDpi(19) - scrollbarOffset - roundedCornersOffset + scrollbarShift - refreshLeftOffset;
            g_rcRefreshButton.left  = g_rcRefreshButton.right - ScaleDpi(21);
            if (g_rcRefreshButton.right > WINDOW_WIDTH) {
                int overflow = g_rcRefreshButton.right - WINDOW_WIDTH;
                g_rcRefreshButton.right -= overflow;
                g_rcRefreshButton.left  -= overflow;
            }

            BOOL drewRefreshHoverImage = FALSE;
            if (g_IsHoveringRefresh && g_Settings.theme == 0) {
                if (!g_hIconRefreshHover)
                    g_hIconRefreshHover = CreateIconFromBase64PNG(REFRESH_ICON_HOVER_BASE64, ScaleDpi(22), ScaleDpi(22));
                if (g_hIconRefreshHover) {
                    DrawIconEx(hdc, g_rcRefreshButton.left, g_rcRefreshButton.top,
                               g_hIconRefreshHover, ScaleDpi(22), ScaleDpi(22), 0, NULL, DI_NORMAL);
                    drewRefreshHoverImage = TRUE;
                }
            }
            if (g_IsHoveringRefresh && !drewRefreshHoverImage) {
                RECT rcBtn = g_rcRefreshButton;
                COLORREF refreshHoverBg = (g_Settings.theme == 1) ? RGB(40, 40, 60) : RGB(220, 238, 252);
                COLORREF refreshHoverBorder = (g_Settings.theme == 1) ? RGB(60, 60, 120) : RGB(174, 212, 243);
                HBRUSH hBrBg = CreateSolidBrush(refreshHoverBg);
                HPEN   hPenBorder = CreatePen(PS_SOLID, 1, refreshHoverBorder);
                HPEN   hOldPen = (HPEN)SelectObject(hdc, hPenBorder);
                HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrBg);
                RoundRect(hdc, rcBtn.left, rcBtn.top, rcBtn.right, rcBtn.bottom, 4, 4);
                SelectObject(hdc, hOldPen);
                SelectObject(hdc, hOldBrush);
                DeleteObject(hBrBg);
                DeleteObject(hPenBorder);
            }
            
            if (!drewRefreshHoverImage) {
                if (!g_hIconRefreshNormal)
                    g_hIconRefreshNormal = CreateIconFromBase64PNG(REFRESH_ICON_NORMAL_BASE64,
                                                                   ScaleDpi(16), ScaleDpi(16));
                if (g_hIconRefreshNormal) {
                    if (g_Settings.theme == 0) {
                        int normalIconSize = ScaleDpi(16);
                        DrawIconEx(hdc, g_rcRefreshButton.left+2, g_rcRefreshButton.top+3,
                                   g_hIconRefreshNormal, normalIconSize, normalIconSize, 0, NULL, DI_NORMAL);
                    } else {
                        ICONINFO ii = {0};
                        GetIconInfo(g_hIconRefreshNormal, &ii);
                        BITMAP bm = {0};
                        GetObject(ii.hbmColor, sizeof(bm), &bm);
                        int iw = bm.bmWidth, ih = bm.bmHeight;
                        HDC hdcTmp = CreateCompatibleDC(hdc);
                        BITMAPINFO bmi = {{0}};
                        bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
                        bmi.bmiHeader.biWidth       = iw;
                        bmi.bmiHeader.biHeight      = -ih;
                        bmi.bmiHeader.biPlanes      = 1;
                        bmi.bmiHeader.biBitCount    = 32;
                        bmi.bmiHeader.biCompression = BI_RGB;
                        DWORD* pixels = NULL;
                        HBITMAP hBmpTmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&pixels, NULL, 0);
                        HBITMAP hOldBmpTmp = (HBITMAP)SelectObject(hdcTmp, hBmpTmp);
                        COLORREF bgCol = GetHeaderBgColor();
                        BYTE bgR = GetRValue(bgCol), bgG = GetGValue(bgCol), bgB = GetBValue(bgCol);
                        HBRUSH hBrTmp = CreateSolidBrush(bgCol);
                        RECT rcTmp = {0, 0, iw, ih};
                        FillRect(hdcTmp, &rcTmp, hBrTmp);
                        DeleteObject(hBrTmp);
                        DrawIconEx(hdcTmp, 0, 0, g_hIconRefreshNormal, iw, ih, 0, NULL, DI_NORMAL);
                        for (int p = 0; p < iw * ih; p++) {
                            BYTE pb = (pixels[p])       & 0xFF;
                            BYTE pg = (pixels[p] >> 8)  & 0xFF;
                            BYTE pr = (pixels[p] >> 16) & 0xFF;
                            if (abs((int)pr - bgR) < 25 && abs((int)pg - bgG) < 25 && abs((int)pb - bgB) < 25)
                                continue;
                            int lum = ((int)pr  *299 + (int)pg*  587 + (int)pb * 114) / 1000;
                            int t = 255 - lum; 
                            BYTE nr = (BYTE)(100 * t / 255);
                            BYTE ng = (BYTE)(200 * t / 255);
                            BYTE nb = (BYTE)(255 * t / 255);
                            pixels[p] = (pixels[p] & 0xFF000000) | ((DWORD)nr << 16) | ((DWORD)ng << 8) | nb;
                        }
                        BitBlt(hdc, g_rcRefreshButton.left+2, g_rcRefreshButton.top+3, iw, ih, hdcTmp, 0, 0, SRCCOPY);
                        SelectObject(hdcTmp, hOldBmpTmp);
                        DeleteObject(hBmpTmp);
                        DeleteDC(hdcTmp);
                        if (ii.hbmColor) DeleteObject(ii.hbmColor);
                        if (ii.hbmMask)  DeleteObject(ii.hbmMask);
                    }
                }
            }
            SelectObject(hdc, g_hFontNormal); SetTextColor(hdc, GetSecondaryTextColor());
int wifiLabelY = separatorY + ScaleDpi(7);
TextOutW(hdc, ScaleDpi(11), wifiLabelY, LOC(STR_WIFI_HEADER), lstrlenW(LOC(STR_WIFI_HEADER)));
            
            if (g_IsHoveringArrow) {
                COLORREF arrowHoverBg = (g_Settings.theme == 1) ? RGB(40, 40, 60) : RGB(230, 240, 255);
                COLORREF arrowHoverBorder = (g_Settings.theme == 1) ? RGB(60, 60, 120) : RGB(180, 210, 245);
                HBRUSH hBrA  = CreateSolidBrush(arrowHoverBg);
                HPEN   hPenA = CreatePen(PS_SOLID, 1, arrowHoverBorder);
                HPEN   hOldPA = (HPEN)SelectObject(hdc, hPenA);
                HBRUSH hOldBA = (HBRUSH)SelectObject(hdc, hBrA);
                RoundRect(hdc, g_rcArrowButton.left, g_rcArrowButton.top,
                          g_rcArrowButton.right, g_rcArrowButton.bottom, 2, 2);
                SelectObject(hdc, hOldPA); SelectObject(hdc, hOldBA);
                DeleteObject(hBrA); DeleteObject(hPenA);
            }
            RecalcArrowRect();
            if (g_Settings.theme == 1) {
                // Dark theme: use Marlett font character (manual style)
                SelectObject(hdc, g_hFontArrow);
                SetTextColor(hdc, RGB(180, 180, 180));
                LPCWSTR arrowChar = g_bListExpanded ? L"6" : L"5";
                RECT rcArrowText = g_rcArrowButton; rcArrowText.top += 2;
                DrawTextW(hdc, arrowChar, 1, &rcArrowText, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
            } else {
                // Light theme: use PNG chevron icons (with Marlett fallback)
                EnsureChevronIcons();
                HICON hChevron = NULL;
                if (g_bListExpanded) {
                    // List is visible -> show UP chevron (▲)
                    hChevron = g_IsHoveringArrow ? g_hIconChevronUpHL : g_hIconChevronUp;
                } else {
                    // List is hidden -> show DOWN chevron (▼)
                    hChevron = g_IsHoveringArrow ? g_hIconChevronDownHL : g_hIconChevronDown;
                }
                if (hChevron) {
                    // Draw the chevron at its natural square aspect ratio,
                    // centered within the arrow button rect.
                    int btnW = g_rcArrowButton.right - g_rcArrowButton.left;
                    int btnH = g_rcArrowButton.bottom - g_rcArrowButton.top;
                    int chevSize = (btnH < btnW) ? btnH : btnW;  // square fit
                    int chevX = g_rcArrowButton.left + (btnW - chevSize) / 2;
                    int chevY = g_rcArrowButton.top + (btnH - chevSize) / 2;
                    DrawIconEx(hdc, chevX, chevY, hChevron, chevSize, chevSize, 0, NULL, DI_NORMAL);
                } else {
                    // Fallback: Marlett font if PNG decoding failed
                    SelectObject(hdc, g_hFontArrow);
                    SetTextColor(hdc, RGB(50, 50, 50));
                    LPCWSTR arrowChar = g_bListExpanded ? L"6" : L"5";
                    RECT rcArrowText = g_rcArrowButton; rcArrowText.top += 2;
                    DrawTextW(hdc, arrowChar, 1, &rcArrowText, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
                }
            }
            
            if (g_bListExpanded) {
                HRGN hRgnClip = CreateRectRgn(0, LIST_Y_START, WINDOW_WIDTH, LIST_Y_END);
                SelectClipRgn(hdc, hRgnClip);
                DeleteObject(hRgnClip);
                
                int scrollbarOffset = (totalHeight > visibleHeight) ? ScaleDpi(16) : 0;
                UpdateLayoutGeometry(scrollbarOffset);  
                
                for (int i = 0; i < g_NetworkCount; i++) {
                    RECT rcRow;
                    if (!GetRowRect(i, &rcRow)) continue;
                    BOOL isSelected = (i == g_SelectedRowIndex);
                    BOOL isHovered  = (i == g_HoveredRowIndex);
                    BOOL hasKeyboardFocus = (i == g_KeyboardSelectedIndex);

                    if (isSelected || isHovered) {
                        RECT rcFullRow = rcRow; rcFullRow.left = 0; rcFullRow.right = WINDOW_WIDTH - 5;
                        COLORREF bgColor = isSelected ? GetRowSelectedColor() : GetRowHoverColor();
                        COLORREF borderColor = isSelected ? GetRowSelectedBorderColor() : GetRowHoverBorderColor();
                        HBRUSH hBrBg  = CreateSolidBrush(bgColor);
                        HPEN   hPenBg = CreatePen(PS_SOLID, 1, borderColor);
                        HPEN   hOldP  = (HPEN)SelectObject(hdc, hPenBg);
                        HBRUSH hOldB  = (HBRUSH)SelectObject(hdc, hBrBg);
                        RoundRect(hdc, rcFullRow.left, rcFullRow.top, rcFullRow.right, rcFullRow.bottom, 3, 3);
                        SelectObject(hdc, hOldP); SelectObject(hdc, hOldB);
                        DeleteObject(hBrBg); DeleteObject(hPenBg);
                    }
                    if (hasKeyboardFocus && !isSelected)
                        DrawFocusRectangle(hdc, &rcRow);
                    
                    WCHAR ssidBuf[33]; GetDisplaySSID(i, ssidBuf, 33);
                    BOOL isConnected = (g_NetworkList[i].connState == CONN_STATE_CONNECTED);
                    SelectObject(hdc, isConnected ? g_hFontBold : g_hFontNormal);
                    SetTextColor(hdc, GetNetworkNameColor());
                    // ROW_TEXT_Y_OFFSET is the ~1% row-text nudge (was
                    // WINDOW_HEIGHT*1.01/100 float math); WINDOW_HEIGHT is
                    // already DPI-scaled, so ScaleDpi(4) (~1% of the base
                    // 405px window height) keeps this DPI-consistent.
                    const int ROW_TEXT_Y_OFFSET = ScaleDpi(4);
                    DrawTextWithWrap(hdc, ssidBuf, rcRow.left - ScaleDpi(2), rcRow.top + ScaleDpi(3) + ROW_TEXT_Y_OFFSET,
                                     rcRow.right - rcRow.left - 10, ScaleDpi(24));
                    WifiNetworkItem* item = &g_NetworkList[i];
                    BOOL isTransitioning = (item->connState == CONN_STATE_CONNECTING ||
                                            item->connState == CONN_STATE_DISCONNECTING);
                    if (item->connState == CONN_STATE_CONNECTED) {
                        SelectObject(hdc, g_hFontBold);
                        SetTextColor(hdc, (g_Settings.theme == 1) ? GetTextColor() : RGB(0, 0, 0));
                        RECT rcStatus;
                        rcStatus.right  = rcRow.right - 39 - scrollbarOffset;
                        rcStatus.left   = rcRow.left + 80;
                        rcStatus.top    = rcRow.top + 6 + ROW_TEXT_Y_OFFSET;
                        rcStatus.bottom = rcStatus.top + 18;
                        DrawTextW(hdc, LOC(STR_CONNECTED_TEXT), -1, &rcStatus,
                                  DT_RIGHT | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
                    }
                    else if (isTransitioning) {
                        SelectObject(hdc, g_hFontNormal);
                        SetTextColor(hdc, GetSecondaryTextColor());
                        const WCHAR* transitionText = (item->connState == CONN_STATE_CONNECTING)
                            ? LOC(STR_CONNECTING) : LOC(STR_DISCONNECTING);
                        RECT rcTransition;
                        rcTransition.left   = rcRow.left + 10;
                        rcTransition.right  = rcRow.right - 10;
                        rcTransition.top    = rcRow.top + 24;   
                        rcTransition.bottom = rcTransition.top + 18;
                        DrawTextW(hdc, transitionText, -1, &rcTransition,
                                  DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
                    }
                    
                    DrawNativeSignalIcon(hdc, rcRow.right - 10 - scrollbarOffset, rcRow.top+2, item->signalQuality);    
                }
                SelectClipRgn(hdc, NULL);
            }
        } else {
            if (g_hWndButtonConnect && IsWindow(g_hWndButtonConnect))   
                ShowWindow(g_hWndButtonConnect, SW_HIDE);
            if (g_hWndCheckboxConnect && IsWindow(g_hWndCheckboxConnect)) 
                ShowWindow(g_hWndCheckboxConnect, SW_HIDE);
            g_bShowCheckboxLabel = FALSE;
        }

        SelectObject(hdc, g_IsHoveringLink ? g_hFontUnderline : g_hFontNormal);
        SetTextColor(hdc, GetLinkColor());
        const wchar_t* footerText = LOC(STR_OPEN_SHARING_CENTER);
        SIZE textSize; GetTextExtentPoint32W(hdc, footerText, lstrlenW(footerText), &textSize);
        RECT rcClient;
        GetClientRect(hwnd, &rcClient);
        int footerTop = rcClient.bottom - FOOTER_HEIGHT;
        int centerX = (rcClient.right - textSize.cx) / 2;
        int footerTextYC = footerTop + (FOOTER_HEIGHT - textSize.cy) / 2;
        if (g_Settings.useRoundedCorners) {
            footerTextYC += (FOOTER_HEIGHT * 15) / 100;
        }
        TextOutW(hdc, centerX, footerTextYC, footerText, lstrlenW(footerText));
        
        if (g_bShowCheckboxLabel) {
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, (g_Settings.theme == 1) ? RGB(255,255,255) : RGB(0,0,0));
            HFONT hOldFontChk = (HFONT)SelectObject(hdc, g_hFontCheckbox);
            DrawTextW(hdc, LOC(STR_CHK_CONNECT_AUTO), -1, &g_rcCheckboxLabel, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
            SelectObject(hdc, hOldFontChk);
        }
        BitBlt(hdcReal, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdc, 0, 0, SRCCOPY);
        SelectObject(hdc, hOldBmp);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
        if (!pdis || pdis->CtlID != IDC_CONN_BUTTON) break;
        if (g_Settings.theme != 1) break;
        BOOL isPressed  = (pdis->itemState & ODS_SELECTED) != 0;
        BOOL isDisabled = (pdis->itemState & ODS_DISABLED) != 0;
        BOOL isHovering = g_IsHoveringConnectButton && !isPressed && !isDisabled;
        
        HDC  hdcReal = pdis->hDC;
        RECT rc  = pdis->rcItem;
        int  w = rc.right - rc.left;
        int  h = rc.bottom - rc.top;
        if (w <= 0 || h <= 0) break;
        WCHAR szText[64];
        int textLen = GetWindowTextW(pdis->hwndItem, szText, 64);
        COLORREF bgColor;
        if (isDisabled) {
            bgColor = RGB(50, 50, 58);
        } else if (isPressed) {
            bgColor = RGB(35, 35, 45);
        } else if (isHovering) {
            bgColor = RGB(70, 70, 85);  
        } else {
            bgColor = RGB(60, 60, 72);
        }
        
        COLORREF lightColor = isPressed ? RGB(25, 25, 32) : (isHovering ? RGB(95, 95, 115) : RGB(85, 85, 100));
        COLORREF darkColor = isPressed ? RGB(60, 60, 72) : (isHovering ? RGB(35, 35, 45) : RGB(25, 25, 32));
        COLORREF textColor = isDisabled ? RGB(130, 130, 140) : RGB(255, 255, 255);
        COLORREF hoverBorder = isHovering ? RGB(90, 90, 120) : RGB(0,0,0);
        
        HDC hdcMem = CreateCompatibleDC(hdcReal);
        HBITMAP hBmpMem = CreateCompatibleBitmap(hdcReal, w, h);
        HBITMAP hOldBmpMem = (HBITMAP)SelectObject(hdcMem, hBmpMem);
        RECT rcLocal = {0, 0, w, h};
        HBRUSH hBrBg = CreateSolidBrush(bgColor);
        FillRect(hdcMem, &rcLocal, hBrBg);
        DeleteObject(hBrBg);
        HPEN hPenLight = CreatePen(PS_SOLID, 1, lightColor);
        HPEN hPenDark = CreatePen(PS_SOLID, 1, darkColor);
        HPEN hPenHover = isHovering ? CreatePen(PS_SOLID, 1, hoverBorder) : NULL;
        
        HPEN hOldPen = (HPEN)SelectObject(hdcMem, hPenLight);
        MoveToEx(hdcMem, 0, h - 1, NULL);
        LineTo(hdcMem, 0, 0);
        LineTo(hdcMem, w - 1, 0);
        
        SelectObject(hdcMem, hPenDark);
        MoveToEx(hdcMem, w - 1, 0, NULL);
        LineTo(hdcMem, w - 1, h - 1);
        LineTo(hdcMem, 0, h - 1);
        
        if (isHovering && hPenHover) {
            SelectObject(hdcMem, hPenHover);
            MoveToEx(hdcMem, 1, 1, NULL);
            LineTo(hdcMem, w - 2, 1);
            LineTo(hdcMem, w - 2, h - 2);
            LineTo(hdcMem, 1, h - 2);
            LineTo(hdcMem, 1, 1);
            DeleteObject(hPenHover);
        }
        
        SelectObject(hdcMem, hOldPen);
        DeleteObject(hPenLight);
        DeleteObject(hPenDark);
        
        SetBkMode(hdcMem, TRANSPARENT);
        SetTextColor(hdcMem, textColor);
        HFONT hOldFont = (HFONT)SelectObject(hdcMem, g_hFontButton);
        RECT rcText = rcLocal;
        if (isPressed) { rcText.left += 1; rcText.top += 1; }
        DrawTextW(hdcMem, szText, textLen, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdcMem, hOldFont);
        BitBlt(hdcReal, rc.left, rc.top, w, h, hdcMem, 0, 0, SRCCOPY);
        SelectObject(hdcMem, hOldBmpMem);
        DeleteObject(hBmpMem);
        DeleteDC(hdcMem);
        break;
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdc = (HDC)wParam;
        HWND hwndCtrl = (HWND)lParam;
        if (hwndCtrl == g_hWndCheckboxConnect && g_SelectedRowIndex >= 0 && g_SelectedRowIndex < g_NetworkCount) {
            WifiNetworkItem* item = &g_NetworkList[g_SelectedRowIndex];
            if (item->connState == CONN_STATE_IDLE || item->connState == CONN_STATE_ERROR) {
                COLORREF chkBg   = (g_Settings.theme == 1) ? RGB(40, 40, 50)    : RGB(228, 241, 252);
                COLORREF chkText = (g_Settings.theme == 1) ? RGB(255, 255, 255) : RGB(0, 0, 0);
                SetBkColor(hdc, chkBg);
                SetBkMode(hdc, OPAQUE);
                SetTextColor(hdc, chkText);
                static HBRUSH hBrushCheckbox = NULL;
                static COLORREF lastChkBg = (COLORREF)-1;
                if (!hBrushCheckbox || lastChkBg != chkBg) {
                    if (hBrushCheckbox) DeleteObject(hBrushCheckbox);
                    hBrushCheckbox = CreateSolidBrush(chkBg);
                    lastChkBg = chkBg;
                }
                return (INT_PTR)hBrushCheckbox;
            } else if (g_Settings.theme == 1) {
                COLORREF chkBg = GetFooterBgColor();
                SetBkColor(hdc, chkBg);
                SetBkMode(hdc, OPAQUE);
                SetTextColor(hdc, RGB(255, 255, 255));
                static HBRUSH hBrushCheckboxDark = NULL;
                static COLORREF lastChkBgDark = (COLORREF)-1;
                if (!hBrushCheckboxDark || lastChkBgDark != chkBg) {
                    if (hBrushCheckboxDark) DeleteObject(hBrushCheckboxDark);
                    hBrushCheckboxDark = CreateSolidBrush(chkBg);
                    lastChkBgDark = chkBg;
                }
                return (INT_PTR)hBrushCheckboxDark;
            } else {
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, RGB(255, 255, 255));
                return (INT_PTR)GetStockObject(HOLLOW_BRUSH);
            }
        }
        if (g_Settings.theme == 1) {
            SetBkColor(hdc, RGB(20, 20, 20));
            SetTextColor(hdc, RGB(100, 200, 255));
            SetBkMode(hdc, OPAQUE);
            static HBRUSH hBrPwdStatic = NULL;
            static COLORREF lastBg = (COLORREF)-1;
            COLORREF bg = RGB(20, 20, 20);
            if (!hBrPwdStatic || lastBg != bg) {
                if (hBrPwdStatic) DeleteObject(hBrPwdStatic);
                hBrPwdStatic = CreateSolidBrush(bg);
                lastBg = bg;
            }
            return (INT_PTR)hBrPwdStatic;
        } else {
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(14, 75, 184));
            return (INT_PTR)GetStockObject(NULL_BRUSH);
        }
    }
    case WM_CTLCOLORBTN: {
        HDC hdc = (HDC)wParam;
        HWND hwndBtn = (HWND)lParam;
        if (hwndBtn == g_hWndCheckboxConnect && g_SelectedRowIndex >= 0 && g_SelectedRowIndex < g_NetworkCount) {
            WifiNetworkItem* item = &g_NetworkList[g_SelectedRowIndex];
            if (item->connState == CONN_STATE_IDLE || item->connState == CONN_STATE_ERROR) {
                COLORREF chkBg   = (g_Settings.theme == 1) ? RGB(40, 40, 50)    : RGB(228, 241, 252);
                COLORREF chkText = (g_Settings.theme == 1) ? RGB(255, 255, 255) : RGB(0, 0, 0);
                SetBkColor(hdc, chkBg);
                SetBkMode(hdc, OPAQUE);
                SetTextColor(hdc, chkText);
                static HBRUSH hBrushCheckboxBtn = NULL;
                static COLORREF lastChkBtnBg = (COLORREF)-1;
                if (!hBrushCheckboxBtn || lastChkBtnBg != chkBg) {
                    if (hBrushCheckboxBtn) DeleteObject(hBrushCheckboxBtn);
                    hBrushCheckboxBtn = CreateSolidBrush(chkBg);
                    lastChkBtnBg = chkBg;
                }
                return (INT_PTR)hBrushCheckboxBtn;
            } else if (g_Settings.theme == 1) {
                COLORREF chkBg = GetFooterBgColor();
                SetBkColor(hdc, chkBg);
                SetBkMode(hdc, OPAQUE);
                SetTextColor(hdc, RGB(255, 255, 255));
                static HBRUSH hBrushCheckboxBtnDark = NULL;
                static COLORREF lastChkBtnBgDark = (COLORREF)-1;
                if (!hBrushCheckboxBtnDark || lastChkBtnBgDark != chkBg) {
                    if (hBrushCheckboxBtnDark) DeleteObject(hBrushCheckboxBtnDark);
                    hBrushCheckboxBtnDark = CreateSolidBrush(chkBg);
                    lastChkBtnBgDark = chkBg;
                }
                return (INT_PTR)hBrushCheckboxBtnDark;
            } else {
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, RGB(255, 255, 255));
                return (INT_PTR)GetStockObject(HOLLOW_BRUSH);
            }
        }
        return (INT_PTR)DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    case WM_MOUSEMOVE: {
        int mx = LOWORD(lParam), my = HIWORD(lParam);
        POINT pt = {mx,my};
        RECT rcF = GetFooterRect();
        BOOL wasLink    = g_IsHoveringLink;
        BOOL wasRefresh = g_IsHoveringRefresh;
        BOOL wasArrow   = g_IsHoveringArrow;
        int  wasHov     = g_HoveredRowIndex;
        BOOL wasConnectHover = g_IsHoveringConnectButton;
        
        BOOL showWifiList = (g_NetworkCount > 0);
        
        g_IsHoveringLink    = PtInRect(&rcF, pt) != 0;
        g_IsHoveringRefresh = showWifiList && PtInRect(&g_rcRefreshButton, pt) != 0;
        g_IsHoveringArrow   = showWifiList && PtInRect(&g_rcArrowButton,   pt) != 0;
        
        if (g_hWndButtonConnect && IsWindow(g_hWndButtonConnect) && IsWindowVisible(g_hWndButtonConnect)) {
            RECT rcConnect;
            GetWindowRect(g_hWndButtonConnect, &rcConnect);
            POINT ptScreen = pt;
            ClientToScreen(hwnd, &ptScreen);
            g_IsHoveringConnectButton = PtInRect(&rcConnect, ptScreen) != 0;
        } else {
            g_IsHoveringConnectButton = FALSE;
        }
        
        int newHovered = (showWifiList && my >= LIST_Y_START && my < LIST_Y_END) ? HitTestRows(mx,my) : -1;
        g_HoveredRowIndex = newHovered;
        if (newHovered != wasHov)
            UpdateTooltipForRow(hwnd, newHovered);
        
        SetCursor(LoadCursor(NULL, (g_IsHoveringLink || g_IsHoveringRefresh || g_IsHoveringArrow || g_IsHoveringConnectButton) ? IDC_HAND : IDC_ARROW));
        
        if (wasLink!=g_IsHoveringLink || wasRefresh!=g_IsHoveringRefresh ||
            wasArrow!=g_IsHoveringArrow || wasHov!=g_HoveredRowIndex ||
            wasConnectHover != g_IsHoveringConnectButton) {
            InvalidateRect(hwnd,NULL,FALSE);
            TRACKMOUSEEVENT tme = {sizeof(TRACKMOUSEEVENT),TME_LEAVE,hwnd,0};
            TrackMouseEvent(&tme);
        }
        break;
    }
    case WM_MOUSELEAVE:
        g_IsHoveringLink = g_IsHoveringRefresh = g_IsHoveringArrow = FALSE;
        g_IsHoveringConnectButton = FALSE;
        g_HoveredRowIndex = -1;
        UpdateTooltipForRow(hwnd, -1);
        SetCursor(LoadCursor(NULL,IDC_ARROW));
        InvalidateRect(hwnd,NULL,FALSE);
        break;
    case WM_LBUTTONDOWN: {
        int lx = LOWORD(lParam), ly = HIWORD(lParam);
        POINT pt = {lx,ly};
        RECT rcF = GetFooterRect();
        BOOL showWifiList = (g_NetworkCount > 0);
        
        if (showWifiList && PtInRect(&g_rcRefreshButton,pt)) {
            Wh_Log(L"Manual refresh requested");
            if (g_Ctx.hWlanClient) {
                PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
                if (WlanEnumInterfaces(g_Ctx.hWlanClient, NULL, &pIfList) == ERROR_SUCCESS) {
                    for (DWORD i = 0; i < pIfList->dwNumberOfItems; i++) {
                        DWORD scanResult = WlanScan(g_Ctx.hWlanClient, &pIfList->InterfaceInfo[i].InterfaceGuid, NULL, NULL, NULL);
                        Wh_Log(L"WlanScan requested on interface %lu: %lu", i, scanResult);
                    }
                    WlanFreeMemory(pIfList);
                }
            } else {
                Wh_Log(L"Manual refresh skipped: WLAN client not available");
            }
            RefreshNetworkData();
            ClampScrollPos();
            UpdateLayoutGeometry();
            InvalidateRect(hwnd, NULL, TRUE);
            UpdateWindow(hwnd);
            break;
        }
        if (showWifiList && PtInRect(&g_rcArrowButton,pt)) {
            g_bListExpanded = !g_bListExpanded;
            if (!g_bListExpanded) {
                if (g_hWndButtonConnect && IsWindow(g_hWndButtonConnect))
                    ShowWindow(g_hWndButtonConnect, SW_HIDE);
                if (g_hWndCheckboxConnect && IsWindow(g_hWndCheckboxConnect))
                    ShowWindow(g_hWndCheckboxConnect, SW_HIDE);
                g_SelectedRowIndex = -1;
                ClearKeyboardFocus();
            } else {
                UpdateLayoutGeometry();
            }
            InvalidateRect(hwnd,NULL,TRUE);
            break;
        }
        if (PtInRect(&rcF,pt)) {
            ShellExecuteW(NULL,L"open",L"control.exe",L"/name Microsoft.NetworkAndSharingCenter",NULL,SW_SHOWNORMAL);
            ShowWindow(hwnd,SW_HIDE);
            break;
        }
        if (showWifiList && g_bListExpanded && ly >= LIST_Y_START && ly < LIST_Y_END) {
            int ci = HitTestRows(lx,ly);
            if (ci != -1) {
                if (g_SelectedRowIndex == ci) {
                    ConnectToNetwork(ci);
                } else {
                    g_SelectedRowIndex = ci;
                    SetKeyboardFocus(g_SelectedRowIndex);
                    UpdateLayoutGeometry();
                    EnsureRowVisible(ci);
                }
                InvalidateRect(hwnd,NULL,FALSE);
            } else if (g_SelectedRowIndex != -1) {
                g_SelectedRowIndex = -1;
                ClearKeyboardFocus();
                UpdateLayoutGeometry();
                InvalidateRect(hwnd,NULL,FALSE);
            }
        } else if (g_SelectedRowIndex != -1) {
            g_SelectedRowIndex = -1;
            ClearKeyboardFocus();
            UpdateLayoutGeometry();
            InvalidateRect(hwnd,NULL,FALSE);
        }
        break;
    }
    case WM_RBUTTONDOWN: {
        int rx = LOWORD(lParam), ry = HIWORD(lParam);
        BOOL showWifiList = (g_NetworkCount > 0);
        if (showWifiList && g_bListExpanded && ry >= LIST_Y_START && ry < LIST_Y_END) {
            int ci = HitTestRows(rx,ry);
            if (ci != -1) {
                POINT ptM={rx,ry}; ClientToScreen(hwnd,&ptM);
                ShowContextMenu(hwnd,ci,ptM);
            }
        }
        break;
    }
    case WM_COMMAND: {
        int wid = LOWORD(wParam);
        if (wid == IDC_CONN_BUTTON && g_SelectedRowIndex != -1) {
            ConnectToNetwork(g_SelectedRowIndex);
            break;
        }
        break;
    }
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE) {
            if (!g_inPasswordPrompt) {
                ClearKeyboardFocus();
                ShowWindow(hwnd, SW_HIDE);
            }
        }
        break;
    case WM_SAFE_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_CLOSE:
        ShowWindow(hwnd, SW_HIDE);
        return 0;
    case WM_DESTROY:
        if (g_RefreshTimer) { KillTimer(hwnd, g_RefreshTimer); g_RefreshTimer = 0; }
        if (g_TimeoutTimer) { KillTimer(hwnd, g_TimeoutTimer); g_TimeoutTimer = 0; }
        if (g_hdcMemPaint) { DeleteDC(g_hdcMemPaint); g_hdcMemPaint = NULL; }
        if (g_hbmMemPaint) { DeleteObject(g_hbmMemPaint); g_hbmMemPaint = NULL; }
        g_memPaintWidth = g_memPaintHeight = 0;
        InterlockedDecrement(&g_Ctx.refCount);
        if (g_hTooltip) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_hTooltip, TooltipSubclassProc);
            DestroyWindow(g_hTooltip);
            g_hTooltip = NULL;
        }
        g_hWndFlyout = g_hWndButtonConnect = g_hWndCheckboxConnect = NULL;
        break;
    }
    return DefWindowProcW(hwnd,uMsg,wParam,lParam);
}

// =====================================================================
// Network icon detection via pnidui.dll & toolbar subclassing
// =====================================================================
static void DetectNetworkButtonId(HWND hToolbar, int* outButtonId) {
    *outButtonId = -1;
    int count = (int)SendMessageW(hToolbar, TB_BUTTONCOUNT, 0, 0);
    Wh_Log(L"[Discovery] Toolbar has %d buttons", count);
    for (int i = 0; i < count; i++) {
        TBBUTTON tb{};
        if (!SendMessageW(hToolbar, TB_GETBUTTON, (WPARAM)i, (LPARAM)&tb)) continue;
        if (tb.fsState & TBSTATE_HIDDEN) continue;
        if (tb.fsStyle & TBSTYLE_SEP) continue;
        if (IsNetworkButton(hToolbar, i)) {
            *outButtonId = tb.idCommand;
            WCHAR text[128] = {0};
            SendMessageW(hToolbar, TB_GETBUTTONTEXT, tb.idCommand, (LPARAM)text);
            Wh_Log(L"[Discovery] Network found: btn[%d] id=%d text='%s'", i, tb.idCommand, text);
            return;
        }
    }
    Wh_Log(L"[Discovery] Network button NOT found via pnidui.dll range");
}

LRESULT CALLBACK ToolbarWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass) {
    if (g_Settings.interceptNativeFlyout) {
        if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP || msg == WM_LBUTTONDBLCLK || msg == WM_MOUSEACTIVATE) {
            POINT pt;
            if (msg == WM_MOUSEACTIVATE) {
                DWORD dwPos = GetMessagePos();
                pt.x = GET_X_LPARAM(dwPos);
                pt.y = GET_Y_LPARAM(dwPos);
                ScreenToClient(hWnd, &pt);
            } else {
                pt.x = GET_X_LPARAM(lParam);
                pt.y = GET_Y_LPARAM(lParam);
            }
            LRESULT btnIdx = SendMessageW(hWnd, TB_HITTEST, 0, (LPARAM)&pt);
            if (btnIdx >= 0) {
                TBBUTTON tb = {0};
                if (SendMessageW(hWnd, TB_GETBUTTON, (WPARAM)btnIdx, (LPARAM)&tb)) {
                    int currentCount = (int)SendMessageW(hWnd, TB_BUTTONCOUNT, 0, 0);
                    if (currentCount != g_ToolbarCache.buttonCount) {
                        g_ToolbarCache.valid = FALSE;
                    }
                    if (!g_ToolbarCache.valid) {
                        int detectedId = -1;
                        DetectNetworkButtonId(hWnd, &detectedId);
                        g_ToolbarCache.networkId = detectedId;
                        g_ToolbarCache.buttonCount = currentCount;
                        g_ToolbarCache.valid = TRUE;
                    }
                    if (g_ToolbarCache.networkId != -1 && tb.idCommand == g_ToolbarCache.networkId) {
                        // Record flyout visibility at button-DOWN time, before
                        // WM_ACTIVATE can hide it. This prevents the race where
                        // the flyout deactivates and hides itself, then the
                        // button-UP handler sees it as hidden and reopens it.
                        static BOOL s_flyoutWasVisibleOnDown = FALSE;
                        if (msg == WM_LBUTTONDOWN) {
                            s_flyoutWasVisibleOnDown = (g_hWndFlyout && IsWindow(g_hWndFlyout) && IsWindowVisible(g_hWndFlyout));
                        }
                        if (msg == WM_LBUTTONUP) {
                            static DWORD lastClickTime = 0;
                            DWORD currentTime = GetTickCount();
                            if (currentTime - lastClickTime > CLICK_DEBOUNCE_MS) {
                                lastClickTime = currentTime;
                                if (s_flyoutWasVisibleOnDown) {
                                    // Flyout was open when user pressed -> close it
                                    if (g_hWndFlyout && IsWindow(g_hWndFlyout))
                                        ShowWindow(g_hWndFlyout, SW_HIDE);
                                    ClearKeyboardFocus();
                                } else {
                                    // Flyout was closed -> open it
                                    ToggleFlyoutWindow();
                                }
                            }
                        }
                        if (msg == WM_MOUSEACTIVATE) return MA_ACTIVATE;
                        return 0;
                    }
                }
            }
        }
    }
    return DefSubclassProc(hWnd, msg, wParam, lParam);
}

static bool IsExplorerProcess() {
    WCHAR exePath[MAX_PATH] = {};
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    WCHAR* name = wcsrchr(exePath, L'\\');
    name = name ? name + 1 : exePath;
    return _wcsicmp(name, L"explorer.exe") == 0;
}

static BOOL InstallTrayInterceptionInternal() {
    if (!IsExplorerProcess()) return TRUE;
    InitPniduiInfo();
    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!hTray) {
        Wh_Log(L"Shell_TrayWnd not found");
        return FALSE;
    }
    HWND hNotify  = FindWindowExW(hTray,    NULL, L"TrayNotifyWnd",   NULL);
    HWND hSysPager= hNotify ? FindWindowExW(hNotify,  NULL, L"SysPager",        NULL) : NULL;
    HWND hToolbar = hSysPager ? FindWindowExW(hSysPager,NULL, L"ToolbarWindow32", NULL) : NULL;
    if (!hToolbar) {
        Wh_Log(L"No ToolbarWindow32 found, cannot install tray interception");
        return FALSE;
    }
    HWND hTarget = hToolbar;
    G_hSubclassedToolbar = hTarget;
    Wh_Log(L"Subclassing %s (0x%p)", 
           hToolbar ? L"ToolbarWindow32" : L"TrayNotifyWnd", hTarget);
    WindhawkUtils::SetWindowSubclassFromAnyThread(hTarget, ToolbarWndProc, 0);
    if (hToolbar) {
        int detectedId = -1;
        DetectNetworkButtonId(hToolbar, &detectedId);
        g_ToolbarCache.networkId = detectedId;
        g_ToolbarCache.buttonCount = (int)SendMessageW(hToolbar, TB_BUTTONCOUNT, 0, 0);
        g_ToolbarCache.valid = (detectedId != -1);
    }
    return TRUE;
}

BOOL InstallTrayInterception() {
    return InstallTrayInterceptionInternal();
}

void RemoveTrayInterception() {
    if (G_hSubclassedToolbar) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(G_hSubclassedToolbar, ToolbarWndProc);
        G_hSubclassedToolbar = nullptr;
    }
    g_pniduiBase = NULL;
    g_pniduiEnd  = NULL;
}

// -------------------------------------------------------
// Toggle flyout
// -------------------------------------------------------
void ToggleFlyoutWindow() {
    DWORD dwCurrentThreadId = GetCurrentThreadId();
    BOOL flyoutAlreadyExists = (g_hWndFlyout && IsWindow(g_hWndFlyout));
    DWORD dwTargetOwnerThreadId = flyoutAlreadyExists ? g_dwFlyoutOwnerThreadId : g_Ctx.dwHotkeyThreadId;
    if (dwTargetOwnerThreadId != 0 && dwTargetOwnerThreadId != dwCurrentThreadId) {
        PostThreadMessageW(dwTargetOwnerThreadId, WM_TOGGLE_FLYOUT_REQUEST, 0, 0);
        return;
    }
    EnterCriticalSection(&g_Ctx.csLock);
    if (!g_Ctx.isUninitializing) {
        if (!g_hWndFlyout || !IsWindow(g_hWndFlyout)) {
            HDC hScreenDC = GetDC(NULL);
            UINT dpi = hScreenDC ? (UINT)GetDeviceCaps(hScreenDC, LOGPIXELSX) : 96;
            if (hScreenDC) ReleaseDC(NULL, hScreenDC);
            RecalcDpiMetrics(dpi);
            HINSTANCE hInst = GetModuleHandle(NULL);
            WNDCLASSW wc = {0};
            wc.lpfnWndProc   = FlyoutWndProc;
            wc.hInstance     = hInst;
            wc.lpszClassName = L"Win7NetworkFlyoutSafe";
            wc.hCursor       = LoadCursor(NULL,IDC_ARROW);
            wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
            UnregisterClassW(wc.lpszClassName,hInst);
            RegisterClassW(&wc);
            RECT rcClient = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
            DWORD dwExStyle = WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_LEFT;
            DWORD dwStyle = WS_POPUP | WS_CLIPCHILDREN | WS_BORDER; 
            if (g_Settings.useRoundedCorners) dwStyle |= WS_THICKFRAME;
            AdjustWindowRectEx(&rcClient, dwStyle, FALSE, dwExStyle);
            g_hWndFlyout = CreateWindowExW(dwExStyle, wc.lpszClassName, L"", dwStyle,
                0, 0, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top,
                NULL, NULL, hInst, NULL);
            if (g_hWndFlyout) {
                g_dwFlyoutOwnerThreadId = GetCurrentThreadId();
            }
        }
        if (IsWindowVisible(g_hWndFlyout)) {
            ClearKeyboardFocus();
            ShowWindow(g_hWndFlyout, SW_HIDE);
        } else {
            if (!g_Ctx.hWlanClient) {
                DWORD dwMaxClient = 2, dwCurVer = 0;
                if (WlanOpenHandle(dwMaxClient, NULL, &dwCurVer, &g_Ctx.hWlanClient) == ERROR_SUCCESS) {
                    WlanRegisterNotification(g_Ctx.hWlanClient, WLAN_NOTIFICATION_SOURCE_ALL, TRUE,
                                             WlanNotificationCallback, &g_Ctx, NULL, NULL);
                    Wh_Log(L"WLAN handle opened lazily on first flyout show");
                } else {
                    g_Ctx.hWlanClient = NULL;
                    Wh_Log(L"WLAN service still unavailable on flyout show");
                }
            }
            DetermineLocale();
            LoadSettings();
            ApplyNativeControlsTheme();
            UINT dpi = GetDpiForWindow(g_hWndFlyout);
            if (dpi < 96) dpi = 96;
            if (dpi != g_dpi) RecalcDpiMetrics(dpi);
            g_SelectedRowIndex = g_HoveredRowIndex = -1;
            ClearKeyboardFocus();
            g_bListExpanded = TRUE;
            if (g_hWndButtonConnect && IsWindow(g_hWndButtonConnect))
                ShowWindow(g_hWndButtonConnect, SW_HIDE);
            if (g_hWndCheckboxConnect && IsWindow(g_hWndCheckboxConnect))
                ShowWindow(g_hWndCheckboxConnect, SW_HIDE);
            
            // This runs before ShowWindow below, so the window isn't
            // IsWindowVisible() yet - force detection here instead of
            // relying on the visibility gate, which would otherwise skip it
            // on every single open (not just at startup) and always fall
            // back to the generic PC icon.
            RefreshNetworkData(/*forceDetection=*/TRUE);
            RecalcArrowRect();
            UpdateLayoutGeometry();
            PositionWindowNearTray(g_hWndFlyout);
            ShowWindow(g_hWndFlyout, SW_SHOW);
            SetForegroundWindow(g_hWndFlyout);
            InvalidateRect(g_hWndFlyout,NULL,TRUE);
        }
    }
    LeaveCriticalSection(&g_Ctx.csLock);
}

DWORD WINAPI HotkeyThreadProc(LPVOID lpParam) {
    ModContext* ctx = (ModContext*)lpParam;
    if (!ctx) return 1;
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    // Prime Ethernet/registry state and the Home/Public/Work category once at
    // startup (bypassing the visibility gate) so the very first time the
    // flyout is shown it already has fresh data instead of momentarily
    // falling back to the generic PC icon while everything is uninitialized.
    // This must happen here (COM-initialized, STA hotkey thread) rather than
    // in Wh_ModInit: NLM requires COM, and g_pNLM must be created on the same
    // apartment that later uses it.
    RefreshNetworkData(/*forceDetection=*/TRUE);
    {
        DWORD dwMaxClient = 2, dwCurVer = 0;
        for (int attempt = 0; attempt < 2; attempt++) {
            DWORD wlanResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVer, &ctx->hWlanClient);
            if (wlanResult == ERROR_SUCCESS) {
                WlanRegisterNotification(ctx->hWlanClient, WLAN_NOTIFICATION_SOURCE_ALL, TRUE,
                                         WlanNotificationCallback, ctx, NULL, NULL);
                Wh_Log(L"WLAN handle opened on hotkey thread (attempt %d)", attempt + 1);
                break;
            }
            Wh_Log(L"WlanOpenHandle attempt %d failed: %lu", attempt + 1, wlanResult);
            if (attempt == 0) Sleep(500);
        }
        if (!ctx->hWlanClient) {
            Wh_Log(L"WLAN service unavailable - will retry lazily on first flyout open");
        }
    }
    auto UpdateHotkeyRegistration = [](BOOL shouldRegister) {
        UnregisterHotKey(NULL, HOTKEY_ID);
        if (shouldRegister) RegisterHotKey(NULL, HOTKEY_ID, MOD_CONTROL | MOD_NOREPEAT, 'H');
    };
    
    UpdateHotkeyRegistration(g_Settings.enableHotkey);
    UINT uTaskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
    BOOL trayAlreadyHooked = (G_hSubclassedToolbar != NULL);
    UINT_PTR trayRetryTimer = trayAlreadyHooked ? 0 : SetTimer(NULL, 0, 1500, NULL);
    MSG msg = {0};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        if (trayRetryTimer && msg.message == WM_TIMER && msg.wParam == trayRetryTimer) {
            if (ctx->isUninitializing || InstallTrayInterceptionInternal()) {
                KillTimer(NULL, trayRetryTimer);
                trayRetryTimer = 0;
            }
        }
        if (msg.message == WM_HOTKEY && msg.wParam == HOTKEY_ID && !ctx->isUninitializing)
            ToggleFlyoutWindow();
        if (msg.message == WM_TOGGLE_FLYOUT_REQUEST && !ctx->isUninitializing)
            ToggleFlyoutWindow();
        if (msg.message == WM_UPDATE_HOTKEY && !ctx->isUninitializing)
            UpdateHotkeyRegistration(g_Settings.enableHotkey);
        if (msg.message == WM_UPDATE_REFRESH_TIMER && !ctx->isUninitializing) {
            if (SafeToAccessUI() && g_hWndFlyout) {
                if (g_RefreshTimer) {
                    KillTimer(g_hWndFlyout, g_RefreshTimer);
                    g_RefreshTimer = 0;
                }
                if (g_Settings.refreshInterval > 0) {
                    g_RefreshTimer = SetTimer(g_hWndFlyout, 1000, g_Settings.refreshInterval, NULL);
                }
            }
        }
        if (msg.message == uTaskbarCreated && !ctx->isUninitializing) {
            InvalidateToolbarCache();
            g_pniduiBase = NULL;
            g_pniduiEnd  = NULL;
            if (G_hSubclassedToolbar) RemoveTrayInterception();
            // Use a one-shot timer instead of Sleep(1000) to keep the
            // message loop responsive to WM_QUIT during this wait
            // (important for the 3s SafeCleanup timeout).
            HANDLE timerEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
            if (timerEvent) {
                // Use MsgWaitForMultipleObjects with a 1-second timeout
                // instead of Sleep, so WM_QUIT is still processed.
                MsgWaitForMultipleObjects(1, &timerEvent, FALSE, 1000, QS_ALLINPUT);
                CloseHandle(timerEvent);
            } else {
                Sleep(1000);
            }
            if (!InstallTrayInterceptionInternal() && !trayRetryTimer) {
                trayRetryTimer = SetTimer(NULL, 0, 1500, NULL);
            }
            UpdateHotkeyRegistration(g_Settings.enableHotkey);
        }
        TranslateMessage(&msg); DispatchMessageW(&msg);
    }
    if (trayRetryTimer) KillTimer(NULL, trayRetryTimer);
    UnregisterHotKey(NULL, HOTKEY_ID);
    if (g_pNLM) {
        g_pNLM->Release();
        g_pNLM = NULL;
    }
    CoUninitialize();
    return 0;
}

void SafeCleanup() {
    if (InterlockedExchange(&g_Ctx.isUninitializing, 1L)) return;
    RemoveTrayInterception();
    if (g_Ctx.dwHotkeyThreadId) PostThreadMessageW(g_Ctx.dwHotkeyThreadId, WM_QUIT, 0, 0);
    if (g_Ctx.hHotkeyThread) {
        WaitForSingleObject(g_Ctx.hHotkeyThread, 3000);
        CloseHandle(g_Ctx.hHotkeyThread);
        g_Ctx.hHotkeyThread = NULL; g_Ctx.dwHotkeyThreadId = 0;
    }
    if (g_hWndFlyout && IsWindow(g_hWndFlyout)) {
        SendMessageW(g_hWndFlyout, WM_SAFE_CLOSE, 0, 0);
        for (int i = 0; i < 50 && IsWindow(g_hWndFlyout); i++) {
            MSG msg;
            while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
        }
        if (IsWindow(g_hWndFlyout)) DestroyWindow(g_hWndFlyout);
    }
        if (g_hConnectThread) {
        Wh_Log(L"SafeCleanup: Waiting for connect thread to finish...");
        DWORD waitResult = WaitForSingleObject(g_hConnectThread, 5000);
        Wh_Log(L"SafeCleanup: Connect thread finished (result=%lu)", waitResult);
        CloseHandle(g_hConnectThread);
        g_hConnectThread = NULL;
    } else {
        Wh_Log(L"SafeCleanup: No pending connect thread");
    }
    if (g_Ctx.hWlanClient) { WlanCloseHandle(g_Ctx.hWlanClient, NULL); g_Ctx.hWlanClient = NULL; }
    ShutdownGdiPlusRendering();
    FreeSystemIcons();
    FreeGlobalFonts();
    g_hWndFlyout = g_hWndButtonConnect = g_hWndCheckboxConnect = NULL;
    g_dwFlyoutOwnerThreadId = 0;
    g_Initialized = FALSE;
}


// ============================================================================
// Integrated Windows 7 Network Center Links v3.2.0
// Uses only existing Windows icon resources; no embedded or temporary DLLs.
// ============================================================================
namespace Win7NetworkCenterLinks {

// Use only the icon resources that are already present in Windows. This keeps
// the mod fully source-auditable and avoids writing or loading executable code
// from a user-writable directory.
static constexpr int kConnectCustomIconId = 0x7FF1;
static constexpr int kHomegroupCustomIconId = 0x7FF2;
// Network-map row (This computer / network category / Internet). Reuses
// icons already loaded elsewhere in the mod rather than new embedded assets:
// kComputerIconId serves g_hIconNetworkMap (generic PC icon, netshell.dll
// #120) for BOTH the computer and Internet nodes as a neutral placeholder -
// there is no verified globe/Internet icon asset in this mod yet. Swap the
// Internet node to a real globe icon once one is sourced and confirmed.
// kNetMapCategoryIconId serves GetNetworkLocationIcon() (Home/Public/Work,
// already used by the flyout header).
static constexpr int kComputerIconId = 0x7FF3;
static constexpr int kNetMapCategoryIconId = 0x7FF4;
static constexpr int kGlobeIconId = 0x7FF5;
static constexpr int kNoInternetXIconId = 0x7FF6;
static constexpr int kOfflineNetworkIconId = 0x7FF8;
static bool g_addConnect = true;
static bool g_addHomegroup = true;
static bool g_addNetworkMap = true;  // Visual Network Map rectangle (now functional)
static bool g_hookInstalled = false;
static bool g_iconHookInstalled = false;

using LoadImageW_t = decltype(&LoadImageW);
static LoadImageW_t LoadImageW_Orig = nullptr;

// NetCenter renders its native active-network label with DrawTextW after the
// XML has been parsed. Hook that final draw to apply the existing privacy mode
// there too, without changing the actual Windows network profile name.
using DrawTextW_t = decltype(&DrawTextW);
static DrawTextW_t DrawTextW_Orig = nullptr;
static bool g_textHookInstalled = false;

// ---------------------------------------------------------------------------
// Strings / XML
// ---------------------------------------------------------------------------
struct LangPack {
    WORD lang;
    const wchar_t *cTitle, *cDesc, *hTitle, *hDesc, *mTitle, *mDesc, *fullMap;
    // Fallback label used by GetConnectedNetworkName() when neither Wi-Fi
    // nor Ethernet report a name, and the Network Map's Internet node label
    // in NetworkMapVisual() - both were previously hardcoded in English.
    const wchar_t *networkFallback, *internetLabel;
};

static const LangPack kLang[] = {
    {0x09, L"Connect to a Network",
     L"Connect to an available wireless, VPN, or dial-up network.",
     L"Choose Homegroup and Sharing Options",
     L"View or change your homegroup settings and network sharing preferences.",
     L"View Network Map",
     L"See a map of your network and connected devices.",
     L"View full map", L"Network", L"Internet"},
    {0x10, L"Connessione a una rete",
     L"Connettere o riconnettere una rete wireless, VPN o di accesso remoto disponibile.",
     L"Selezione delle opzioni del gruppo home e della condivisione",
     L"Accedere alle impostazioni del gruppo home e configurare le opzioni di condivisione della rete.",
     L"Visualizza mappa di rete",
     L"Visualizza una mappa della rete e dei dispositivi connessi.",
     L"Visualizza mappa completa", L"Rete", L"Internet"},
    {0x0c, L"Se connecter \u00e0 un r\u00e9seau",
     L"Connectez-vous aux r\u00e9seaux sans fil, VPN ou distants disponibles.",
     L"Choisir les options de groupe r\u00e9sidentiel et de partage",
     L"Affichez ou modifiez les param\u00e8tres de groupe r\u00e9sidentiel et de partage.",
     L"Afficher la carte du r\u00e9seau",
     L"Voir une carte de votre r\u00e9seau et des appareils connect\u00e9s.",
     L"Afficher la carte compl\u00e8te", L"R\u00e9seau", L"Internet"},
    {0x0a, L"Conectar a una red",
     L"Con\u00e9ctese a redes inal\u00e1mbricas, VPN o de acceso telef\u00f3nico disponibles.",
     L"Elegir opciones de grupo en el hogar y uso compartido",
     L"Vea o cambie la configuraci\u00f3n del grupo en el hogar y uso compartido de red.",
     L"Ver mapa de red",
     L"Vea un mapa de su red y los dispositivos conectados.",
     L"Ver mapa completo", L"Red", L"Internet"},
    {0x19, L"\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u0435 \u043a \u0441\u0435\u0442\u0438",
     L"\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u0435 \u043a \u0434\u043e\u0441\u0442\u0443\u043f\u043d\u044b\u043c \u0431\u0435\u0441\u043f\u0440\u043e\u0432\u043e\u0434\u043d\u044b\u043c \u0441\u0435\u0442\u044f\u043c, VPN \u0438\u043b\u0438 \u0441\u0435\u0442\u044f\u043c \u0443\u0434\u0430\u043b\u0451\u043d\u043d\u043e\u0433\u043e \u0434\u043e\u0441\u0442\u0443\u043f\u0430.",
     L"\u0412\u044b\u0431\u043e\u0440 \u043f\u0430\u0440\u0430\u043c\u0435\u0442\u0440\u043e\u0432 \u0434\u043e\u043c\u0430\u0448\u043d\u0435\u0439 \u0433\u0440\u0443\u043f\u043f\u044b \u0438 \u043e\u0431\u0449\u0435\u0433\u043e \u0434\u043e\u0441\u0442\u0443\u043f\u0430",
     L"\u041f\u0440\u043e\u0441\u043c\u043e\u0442\u0440 \u0438\u043b\u0438 \u0438\u0437\u043c\u0435\u043d\u0435\u043d\u0438\u0435 \u043f\u0430\u0440\u0430\u043c\u0435\u0442\u0440\u043e\u0432 \u0434\u043e\u043c\u0430\u0448\u043d\u0435\u0439 \u0433\u0440\u0443\u043f\u043f\u044b \u0438 \u043e\u0431\u0449\u0435\u0433\u043e \u0434\u043e\u0441\u0442\u0443\u043f\u0430 \u043a \u0441\u0435\u0442\u0438.",
     L"\u041f\u0440\u043e\u0441\u043c\u043e\u0442\u0440 \u043a\u0430\u0440\u0442\u044b \u0441\u0435\u0442\u0438",
     L"\u041f\u0440\u043e\u0441\u043c\u043e\u0442\u0440 \u043a\u0430\u0440\u0442\u044b \u0441\u0435\u0442\u0438 \u0438 \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0451\u043d\u043d\u044b\u0445 \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432.",
     L"\u041f\u043e\u043a\u0430\u0437\u0430\u0442\u044c \u043f\u043e\u043b\u043d\u0443\u044e \u043a\u0430\u0440\u0442\u0443", L"\u0421\u0435\u0442\u044c", L"\u0418\u043d\u0442\u0435\u0440\u043d\u0435\u0442"},
    {0x07, L"Mit einem Netzwerk verbinden",
     L"Verbindung mit verf\u00fcgbaren Drahtlos-, VPN- oder DF\u00dc-Netzwerken herstellen.",
     L"Heimnetzgruppen- und Freigabeoptionen ausw\u00e4hlen",
     L"Einstellungen f\u00fcr Heimnetzgruppen und Netzwerkfreigaben anzeigen oder \u00e4ndern.",
     L"Netzwerkkarte anzeigen",
     L"Zeigen Sie eine Karte Ihres Netzwerks und der verbundenen Ger\u00e4te an.",
     L"Vollst\u00e4ndige Karte anzeigen", L"Netzwerk", L"Internet"},
    {0x16, L"Ligar a uma rede",
     L"Ligue-se a redes sem fios, VPN ou de acesso telef\u00f3nico dispon\u00edveis.",
     L"Escolher op\u00e7\u00f5es de Grupo Dom\u00e9stico e partilha",
     L"Veja ou altere as defini\u00e7\u00f5es do Grupo Dom\u00e9stico e da partilha de rede.",
     L"Ver mapa de rede",
     L"Veja um mapa da sua rede e dispositivos ligados.",
     L"Ver mapa completo", L"Rede", L"Internet"},
    {0x15, L"Połącz z siecią",
     L"Połącz z dostępną siecią bezprzewodową, VPN lub modemową.",
     L"Wybierz opcje grupy domowej i udostępniania",
     L"Wyświetl lub zmień ustawienia grupy domowej i udostępniania sieci.",
     L"Wyświetl mapę sieci",
     L"Wyświetl mapę sieci i podłączonych urządzeń.",
     L"Wyświetl pełną mapę", L"Sieć", L"Internet"},
    {0x13, L"Verbinding maken met een netwerk",
     L"Verbinding maken met een beschikbare draadloos-, VPN- of inbelnetwerk.",
     L"Heimgroep- en delensopties kiezen",
     L"Bekijk of wijzig uw heimgroep- en netwerkinstellingen.",
     L"Netwerkkaart weergeven",
     L"Bekijk een kaart van uw netwerk en verbonden apparaten.",
     L"Volledige kaart weergeven", L"Netwerk", L"Internet"},
    {0x18, L"Conectare la o rețea",
     L"Conectați-vă la o rețea fără fir, VPN sau dial-up disponibilă.",
     L"Alegeți opțiunile de grup de domiciliu și partajare",
     L"Vizualizați sau modificați setările grupului de domiciliu și partajarea în rețea.",
     L"Vizualizare hartă rețea",
     L"Vizualizați o hartă a rețelei și dispozitivelor conectate.",
     L"Vizualizare hartă completă", L"Rețea", L"Internet"},
};

static const LangPack* GetLang() {
    WORD ui;
    switch (g_Settings.language) {
        case 1: ui = 0x09; break;  // English
        case 2: ui = 0x10; break;  // Italian
        case 3: ui = 0x0a; break;  // Spanish
        case 4: ui = 0x0c; break;  // French
        case 5: ui = 0x19; break;  // Russian
        case 6: ui = 0x07; break;  // German
        case 7: ui = 0x16; break;  // Portuguese
        case 8: ui = 0x15; break;  // Polish
        case 9: ui = 0x13; break;  // Dutch
        case 10: ui = 0x18; break; // Romanian
        default: ui = PRIMARYLANGID(GetUserDefaultUILanguage()); break;
    }
    for (const auto& p : kLang)
        if (p.lang == ui)
            return &p;
    return &kLang[0];
}

static std::wstring Esc(const wchar_t* s) {
    std::wstring o;
    for (; s && *s; ++s) {
        switch (*s) {
            case L'&': o += L"&amp;"; break;
            case L'"': o += L"&quot;"; break;
            case L'<': o += L"&lt;"; break;
            case L'>': o += L"&gt;"; break;
            default: o.push_back(*s); break;
        }
    }
    return o;
}

static std::wstring LoadUifile(HMODULE m, PCWSTR n, PCWSTR t) {
    HRSRC r = FindResourceW(m, n, t);
    if (!r)
        return {};
    HGLOBAL g = LoadResource(m, r);
    if (!g)
        return {};
    DWORD sz = SizeofResource(m, r);
    const char* d = (const char*)LockResource(g);
    if (!d || !sz)
        return {};
    int wlen = MultiByteToWideChar(CP_UTF8, 0, d, (int)sz, nullptr, 0);
    UINT cp = CP_UTF8;
    if (wlen <= 0) {
        wlen = MultiByteToWideChar(CP_ACP, 0, d, (int)sz, nullptr, 0);
        cp = CP_ACP;
    }
    if (wlen <= 0)
        return {};
    std::wstring xml(wlen, 0);
    MultiByteToWideChar(cp, 0, d, (int)sz, &xml[0], wlen);
    while (!xml.empty() && (xml.back() == 0 || xml.back() == L'\n' || xml.back() == L'\r'))
        xml.pop_back();
    return xml;
}

static std::wstring IconAttr(int fallbackIconId) {
    // Keep the stock icon IDs when the memory hook could not be installed.
    // Otherwise use private IDs so the page's Configure/Troubleshoot icons
    // (which may use 22/27 too) are never touched.
    int iconId = fallbackIconId;
    if (g_iconHookInstalled) {
        if (fallbackIconId == 22)
            iconId = kConnectCustomIconId;
        else if (fallbackIconId == 27)
            iconId = kHomegroupCustomIconId;
        else if (fallbackIconId == 30)
            iconId = kComputerIconId;  // Network Map -> PC icon
    }
    wchar_t b[96];
    swprintf_s(b, L" content=\"icon(%d,24rp,24rp)\"", iconId);
    return b;
}

static std::wstring Link(const wchar_t* title, const wchar_t* desc, const wchar_t* exe,
                         const wchar_t* params, int fallbackIconId) {
    return L"<NavigateButton layout=\"borderlayout()\" layoutpos=\"top\" "
           L"padding=\"rect(0rp,10rp,0rp,10rp)\" "
           L"shellexecute=\"" +
           std::wstring(exe) + L"\" shellexecuteparams=\"" + params +
           L"\">"
           L"<button layoutpos=\"left\" cursor=\"hand\" active=\"mouse\" "
           L"accessible=\"true\" accrole=\"graphic\"" +
           IconAttr(fallbackIconId) +
           L"/>"
           L"<element layoutpos=\"top\" layout=\"borderlayout()\" "
           L"padding=\"rect(10rp,0rp,0rp,0rp)\">"
           L"<element layoutpos=\"top\" layout=\"flowlayout()\">"
           L"<button sheet=\"cp_style\" class=\"cp_content_link\" content=\"" + Esc(title) +
           L"\"/>"
           L"</element>"
           L"<element layoutpos=\"top\" layout=\"flowlayout()\">"
           L"<element sheet=\"cp_style\" class=\"cp_content_text\" "
           L"padding=\"rect(0rp,5rp,0rp,0rp)\" content=\"" +
           Esc(desc) +
           L"\"/>"
           L"</element>"
           L"</element>"
           L"</NavigateButton>";
}

static bool FindOuterElement(const std::wstring& xml, size_t markerPos, size_t& outStart,
                             size_t& outEnd) {
    size_t start = xml.rfind(L"<element", markerPos);
    if (start == std::wstring::npos)
        return false;
    int depth = 0;
    size_t i = start;
    size_t steps = 0;
    while (i < xml.size() && ++steps < 200000) {
        if (xml.compare(i, 8, L"<element") == 0) {
            size_t gt = xml.find(L'>', i);
            if (gt == std::wstring::npos)
                return false;
            if (gt > i && xml[gt - 1] == L'/') {
                i = gt + 1;
                continue;
            }
            ++depth;
            i = gt + 1;
            continue;
        }
        if (xml.compare(i, 10, L"</element>") == 0) {
            --depth;
            i += 10;
            if (depth == 0) {
                outStart = start;
                outEnd = i;
                return true;
            }
            continue;
        }
        ++i;
    }
    return false;
}

static std::wstring GetComputerNameStr() {
    WCHAR computerName[MAX_COMPUTERNAME_LENGTH + 1] = {0};
    DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
    if (GetComputerNameW(computerName, &size)) {
        return std::wstring(computerName);
    }
    return L"PC";
}

static std::wstring GetConnectedNetworkName() {
    if (g_Settings.privacyMode) {
        WCHAR privateName[64] = {0};
        StringCchPrintfW(privateName, ARRAYSIZE(privateName),
                         LOC(STR_NETWORK_PRIVACY_FMT), 1);
        return std::wstring(privateName);
    }
    
    // Copy shared data under the critical section so reads are safe
    // against RefreshWifiData() on the flyout thread.
    EnterCriticalSection(&g_Ctx.csLock);
    std::wstring wifiName;
    for (int i = 0; i < g_NetworkCount; i++) {
        if (g_NetworkList[i].connState == 2) { // Connected
            wifiName = std::wstring(g_NetworkList[i].ssid);
            break;
        }
    }
    WCHAR ethernetName[64] = {0};
    StringCchCopyW(ethernetName, ARRAYSIZE(ethernetName), g_EthernetNetworkName);
    BOOL ethernetConnected = g_EthernetConnected;
    LeaveCriticalSection(&g_Ctx.csLock);
    
    // Check WiFi first (already copied out)
    if (!wifiName.empty())
        return wifiName;
    
    // Check Ethernet
    if (ethernetConnected && ethernetName[0] != L'\0')
        return std::wstring(ethernetName);

    return GetLang()->networkFallback;
}

static std::wstring NetworkMapVisual() {
    // DirectUI's valid endellipsis layout is left-aligned. In privacy mode,
    // two leading spaces visually center the short "PC" label beneath its icon
    // without using the unsupported contentalign="center" token.
    std::wstring pcName = g_Settings.privacyMode ? L"    PC" : GetComputerNameStr();
    std::wstring networkName = GetConnectedNetworkName();
    std::wstring internetName = GetLang()->internetLabel;
    
    // Cache connectivity once for this entire NetworkMapVisual() build,
    // used by both the route-line rendering and the globe-icon inline check.
    // Saves two COM round-trips per XML generation.
    static BOOL s_cachedConnected = FALSE;
    static DWORD s_cacheTick = 0;
    DWORD now = GetTickCount();
    BOOL isOnline;
    if (now - s_cacheTick > 500) {  // 500ms cache window
        s_cachedConnected = IsInternetConnected();
        s_cacheTick = now;
    }
    isOnline = s_cachedConnected;

    // Windows 7's "View full map" link. The original Network Map feature
    // was removed from modern Windows, therefore open the useful native
    // Network shell folder (CLSID 208D2C60-3AEA-1069-A2D7-08002B30309D).
    const wchar_t* fullMapText = GetLang()->fullMap;
    std::wstring xml;

    // Put the link in its own, full-width top row. This lets it stay at the
    // upper right without reserving horizontal space in the PC->Network->
    // Internet flow layout (which must remain on one line).
    xml += L"<element layoutpos=\"top\" layout=\"borderlayout()\" ";
    xml += L"padding=\"rect(0rp,0rp,10rp,0rp)\">";
    xml += L"<NavigateButton layoutpos=\"right\" layout=\"flowlayout()\" ";
    xml += L"shellexecute=\"%SystemRoot%\\explorer.exe\" ";
    xml += L"shellexecuteparams=\"shell:::{208D2C60-3AEA-1069-A2D7-08002B30309D}\">";
    xml += L"<button sheet=\"cp_style\" class=\"cp_content_link\" cursor=\"hand\" ";
    xml += L"active=\"mouse\" content=\"" + Esc(fullMapText) + L"\"/>";
    xml += L"</NavigateButton>";
    xml += L"</element>";

    // Move the complete network-map group 10% to the left.
    // 96rp - 10% = 86.4rp; use 86rp so it remains DPI-scalable.
    xml += L"<element layoutpos=\"top\" layout=\"flowlayout()\" ";
    xml += L"padding=\"rect(86rp,15rp,10rp,15rp)\" ";
    xml += L">";

    // PC section: 15% (about 14rp) farther right within the map.
    // Horizontally scaled to 86% (the customized map is 14% narrower).
    // The negative right side keeps the connector anchored on its right end.
    xml += L"<element layoutpos=\"left\" layout=\"borderlayout()\" ";
    // PC artwork: 1rp left; 15 + (-21) keeps the horizontal total unchanged.
    xml += L"padding=\"rect(15rp,3rp,-21rp,5rp)\">";
    xml += L"<button layoutpos=\"top\" accessible=\"true\" accrole=\"graphic\" ";
    xml += L" content=\"icon(32755,36rp,36rp)\"/>";
    xml += L"<element layoutpos=\"top\" layout=\"flowlayout()\" ";
    xml += L"padding=\"rect(0rp,2rp,0rp,0rp)\">";
    xml += L"<element sheet=\"cp_style\" class=\"cp_content_text\" ";
    // Keep DirectUI's known-valid endellipsis token. Centering is handled by
    // the icon/container geometry; this avoids an unsupported XML value.
    xml += L"width=\"69rp\" contentalign=\"endellipsis\" ";
    xml += L"content=\"" + Esc(pcName.c_str()) + L"\"/>";
    xml += L"</element>";
    xml += L"</element>";

    // Horizontally scaled to 86%: 125rp -> 108rp.
    // Its extension remains anchored to the left, not the right.
    xml += L"<element layoutpos=\"left\" width=\"108rp\" height=\"2rp\" ";
    xml += L"background=\"argb(255,135,195,235)\"/>";

    // Network section, horizontally scaled to 86% like the PC section.
    xml += L"<element layoutpos=\"left\" layout=\"borderlayout()\" ";
    xml += L"padding=\"rect(4rp,3rp,-22rp,5rp)\">";
    xml += L"<button layoutpos=\"top\" accessible=\"true\" accrole=\"graphic\" ";
    // Use a distinct hardcoded DirectUI resource while offline: DirectUI caches
    // resource 32756, so reusing it kept the colored bench after disconnect.
    xml += isOnline ? L" content=\"icon(32756,36rp,36rp)\"/>"
                                : L" content=\"icon(32760,36rp,36rp)\"/>";
    xml += L"<element layoutpos=\"top\" layout=\"flowlayout()\" ";
    xml += L"padding=\"rect(0rp,2rp,0rp,0rp)\">";
    xml += L"<element sheet=\"cp_style\" class=\"cp_content_text\" ";
    xml += L"width=\"69rp\" contentalign=\"endellipsis\" ";
    xml += L"content=\"" + Esc(networkName.c_str()) + L"\"/>";
    xml += L"</element>";
    xml += L"</element>";

    // If this connection has no Internet access, split the 108rp connector
    // around a 16rp red X at its exact midpoint. Otherwise retain one
    // uninterrupted line. NetworkMapVisual is rebuilt by DirectUI when the
    // native Network Center refreshes after a connectivity transition.
    if (!isOnline) {
        xml += L"<element layoutpos=\"left\" width=\"46rp\" height=\"2rp\" ";
        xml += L"background=\"argb(255,135,195,235)\"/>";
        xml += L"<button layoutpos=\"left\" accessible=\"true\" accrole=\"graphic\" ";
        xml += L"content=\"icon(32758,16rp,16rp)\"/>";
        xml += L"<element layoutpos=\"left\" width=\"46rp\" height=\"2rp\" ";
        xml += L"background=\"argb(255,135,195,235)\"/>";
    } else {
        xml += L"<element layoutpos=\"left\" width=\"108rp\" height=\"2rp\" ";
        xml += L"background=\"argb(255,135,195,235)\"/>";
    }

    // Internet section
    xml += L"<element layoutpos=\"left\" layout=\"borderlayout()\" ";
    // Internet artwork: one further rp right; 6 + 2 preserves the 8rp
    // container width, so its connector and label do not move.
    xml += L"padding=\"rect(6rp,5rp,2rp,5rp)\">";
    xml += L"<button layoutpos=\"top\" accessible=\"true\" accrole=\"graphic\" ";
    xml += L" content=\"icon(32757,36rp,36rp)\"/>";
    xml += L"<element layoutpos=\"top\" layout=\"flowlayout()\" ";
    xml += L"padding=\"rect(0rp,2rp,0rp,0rp)\">";
    xml += L"<element sheet=\"cp_style\" class=\"cp_content_text\" ";
    xml += L"width=\"69rp\" contentalign=\"endellipsis\" ";
    xml += L"content=\"" + Esc(internetName.c_str()) + L"\"/>";
    xml += L"</element>";
    xml += L"</element>";

    xml += L"</element>";  // flow-layout map

    return xml;
}


// Add the Windows 7-style Home/Public/Work icon to the left of the active
// connection's name. 32756 is handled by LoadImageW_Hook and is resolved at
// runtime to the current network-category icon, so it also follows changes
// between Public, Private/Home and Domain/Work profiles.
static std::wstring AddActiveNetworkLocationIcon(const std::wstring& in) {
    static const wchar_t kMarker[] = L"<element id=\"atom(ActiveNetworksSection)\"";
    size_t section = in.find(kMarker);
    if (section == std::wstring::npos)
        return in;

    size_t tagEnd = in.find(L'>', section);
    if (tagEnd == std::wstring::npos)
        return in;

    // Patch() runs once per DirectUI document, but retain this guard in case a
    // future NetCenter resource reuses the same section markup more than once.
    if (in.find(L"icon(32756,36rp,36rp)", section) != std::wstring::npos)
        return in;

    // Direct child of ActiveNetworksSection. The source PNG contains
    // Move the artwork 2% of its 36rp size left (0.72rp, rounded to 1rp)
    // and a little lower. 23 + (-13) remains 10rp, so the native text stays
    // fixed while only the icon's visual position changes.
    const std::wstring iconXml =
        L"<element layoutpos=\"left\" layout=\"borderlayout()\" "
        L"padding=\"rect(23rp,33rp,-13rp,0rp)\">"
        L"<button layoutpos=\"top\" accessible=\"true\" accrole=\"graphic\" "
        L"content=\"icon(32756,36rp,36rp)\"/>"
        L"</element>";

    std::wstring out = in;
    out.insert(tagEnd + 1, iconXml);
    return out;
}

static std::wstring Patch(const std::wstring& in) {
    if (!g_addConnect && !g_addHomegroup && !g_addNetworkMap)
        return in;
    std::wstring xml = in;

    // Add the profile-specific icon before the active connection name, matching
    // the Windows 7 Network and Sharing Center layout.
    if (g_addNetworkMap)
        xml = AddActiveNetworkLocationIcon(xml);

    // STEP 1: Insert Network Map visual rectangle (BEFORE modifying create/diagnose blocks)
    if (g_addNetworkMap) {
        size_t anchor = xml.find(L"<element id=\"atom(ActiveNetworksSection)\"");
        if (anchor != std::wstring::npos) {
            Wh_Log(L"[NetMap] Found anchor at pos=%zu, inserting Network Map", anchor);
            std::wstring mapXml = NetworkMapVisual();
            xml.insert(anchor, mapXml);
            Wh_Log(L"[NetMap] Network Map inserted (%zu chars)", mapXml.length());
        } else {
            Wh_Log(L"[NetMap] Anchor not found, skipping Network Map");
        }
    }

    // STEP 2: Manipulate create/diagnose blocks for Connect and Homegroup links
    size_t createMark = xml.find(L"atom(createnewbtn)");
    size_t diagMark = xml.find(L"atom(diagnosebtn)");
    if (createMark == std::wstring::npos || diagMark == std::wstring::npos)
        return xml;

    size_t c0 = 0, c1 = 0, d0 = 0, d1 = 0;
    if (!FindOuterElement(xml, createMark, c0, c1) ||
        !FindOuterElement(xml, diagMark, d0, d1))
        return xml;
    if (c1 <= c0 || d1 <= d0 || !(c1 <= d0 || d1 <= c0))
        return xml;

    std::wstring createBlock = xml.substr(c0, c1 - c0);
    std::wstring diagBlock   = xml.substr(d0, d1 - d0);

    if (d0 > c0) {
        xml.erase(d0, d1 - d0);
        xml.erase(c0, c1 - c0);
    } else {
        xml.erase(c0, c1 - c0);
        xml.erase(d0, d1 - d0);
    }
    size_t insertAt = (c0 < d0) ? c0 : d0;

    const LangPack* L = GetLang();
    std::wstring mid;
    if (g_addConnect)
        mid += Link(L->cTitle, L->cDesc, L"%SystemRoot%\\explorer.exe",
                    L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}", 22);
    if (g_addHomegroup)
        mid += Link(L->hTitle, L->hDesc, L"%SystemRoot%\\explorer.exe",
                    L"shell:::{67CA7650-96E6-4FDD-BB43-A8E774F73A57}", 27);

    xml.insert(insertAt, createBlock + mid + diagBlock);
    return xml;
}

// ---------------------------------------------------------------------------
// DUI hooks
// ---------------------------------------------------------------------------
#ifdef _WIN64
#define NCL_THISCALL __cdecl
#else
#define NCL_THISCALL __thiscall
#endif

using SetXML_t = HRESULT(NCL_THISCALL*)(void*, const WCHAR*, HINSTANCE, HINSTANCE);
using SetXMLFromResource_t =
    HRESULT(NCL_THISCALL*)(void*, PCWSTR, PCWSTR, HMODULE, HINSTANCE, HINSTANCE);

static SetXML_t SetXML = nullptr;
static SetXMLFromResource_t SetXMLFromResource_Orig = nullptr;
static thread_local int g_inHook = 0;

// ---------------------------------------------------------------------------
// Live refresh of the Network and Sharing Center page.
//
// SetXMLFromResource_Hook only fires once, when DirectUI first loads the
// UIFILE resource for the page (i.e. when the Control Panel window is
// opened). Nothing was previously re-invoking SetXML() after that point, so
// disconnecting/reconnecting Wi-Fi never updated the already-open page - it
// only showed the fresh state the next time the page was reopened.
//
// Fix: remember the DUI target/module/instance from the last successful
// SetXML() call, subscribe to INetworkListManager connectivity events (which
// fire on this same STA thread via control.exe's normal message pump, no
// extra window or timer needed), and on each notification reload+re-Patch()
// the resource and push it again with SetXML().
static void* g_ncTarget = nullptr;
static HMODULE g_ncModule = nullptr;
static HINSTANCE g_ncP4 = nullptr;
static IConnectionPoint* g_ncCP = nullptr;
static DWORD g_ncCookie = 0;
static bool g_ncEventsAdvised = false;

// The DirectUIHWND that currently hosts the patched Network Center XML, i.e.
// the window NetCenterPageSubclass is installed on. Tracked separately from
// g_ncTarget so Wh_ModUninit can marshal teardown to the thread that owns
// the (unmarshalled) COM connection point instead of touching it directly.
static HWND g_ncHostWindow = nullptr;
// Registered lazily: private message used to ask NetCenterPageSubclass to
// run cleanup on its own (STA) thread when the mod is being unloaded.
static UINT g_ncTeardownMsg = 0;

static void RefreshNetworkCenterXml() {
    if (!g_ncTarget || !SetXML || g_inHook)
        return;

    std::wstring xml = LoadUifile(g_ncModule, (PCWSTR)MAKEINTRESOURCE(110), L"UIFILE");
    if (xml.empty())
        return;

    std::wstring patched = Patch(xml);
    if (patched == xml)
        return;

    g_inHook++;
    SetXML(g_ncTarget, patched.c_str(), g_ncModule, g_ncP4);
    g_inHook--;
    Wh_Log(L"[NetMap] Live refresh pushed after connectivity change");
}

class NetworkEventsSink : public INetworkListManagerEvents {
   public:
    NetworkEventsSink() : m_refCount(1) {}
    virtual ~NetworkEventsSink() {}

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IDispatch ||
            riid == IID_INetworkListManagerEvents) {
            *ppv = static_cast<INetworkListManagerEvents*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&m_refCount); }
    STDMETHODIMP_(ULONG) Release() override {
        ULONG r = InterlockedDecrement(&m_refCount);
        if (r == 0) delete this;
        return r;
    }

    // These four come from IDispatch. Left without 'override' since this
    // toolchain's netlistmgr/oaidl headers don't always expose them with a
    // signature clang recognizes as virtual on this interface; they're still
    // correctly dispatched through the vtable at runtime.
    STDMETHODIMP GetTypeInfoCount(UINT* pctinfo) { *pctinfo = 0; return S_OK; }
    STDMETHODIMP GetTypeInfo(UINT, LCID, ITypeInfo**) { return E_NOTIMPL; }
    STDMETHODIMP GetIDsOfNames(REFIID, LPOLESTR*, UINT, LCID, DISPID*) { return E_NOTIMPL; }
    STDMETHODIMP Invoke(DISPID, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*) {
        return E_NOTIMPL;
    }

    STDMETHODIMP ConnectivityChanged(NLM_CONNECTIVITY) override {
        RefreshNetworkCenterXml();
        return S_OK;
    }

   private:
    LONG m_refCount;
};

static void EnsureConnectivityEventsAdvised() {
    if (g_ncEventsAdvised)
        return;

    ComPtr<INetworkListManager> nlm;
    if (FAILED(CoCreateInstance(CLSID_NetworkListManager, NULL, CLSCTX_INPROC_SERVER,
                                 IID_INetworkListManager, (void**)nlm.put())) || !nlm)
        return;

    ComPtr<IConnectionPointContainer> container;
    if (FAILED(nlm->QueryInterface(IID_IConnectionPointContainer, (void**)container.put())) || !container)
        return;

    ComPtr<IConnectionPoint> connectionPoint;
    if (FAILED(container->FindConnectionPoint(IID_INetworkListManagerEvents,
                                               connectionPoint.put())) || !connectionPoint)
        return;

    // Advise retains the sink on success. The local reference is released on
    // every path, including an Advise failure.
    ComPtr<NetworkEventsSink> sink(new NetworkEventsSink());
    DWORD cookie = 0;
    if (SUCCEEDED(connectionPoint->Advise(
            static_cast<IUnknown*>(static_cast<INetworkListManagerEvents*>(sink.get())),
            &cookie))) {
        g_ncCP = connectionPoint.detach(); // ownership moves to UnadviseConnectivityEvents.
        g_ncCookie = cookie;
        g_ncEventsAdvised = true;
        Wh_Log(L"[NetMap] Subscribed to live connectivity change events");
    }
}

// Subclass proc for the NetCenter DirectUIHWND itself (not the top-level
// frame: the frame survives navigation, so if the user navigates away from
// the Network and Sharing Center inside the same Control Panel window, the
// page's DUIXmlParser is destroyed while g_ncTarget would still point at it
// - the same use-after-free described below, without the window ever being
// closed. Subclassing the DirectUIHWND child means WM_NCDESTROY fires both
// on navigation-away and on the whole window closing.
//
// WM_NCDESTROY and the private teardown message both run on the owning STA
// thread (the former because DefWindowProc/DestroyWindow dispatch it there,
// the latter because it is only ever posted to hWnd), so it is safe to
// unadvise the COM sink - a pointer obtained on that same thread - from
// this callback.
// Forward declaration needed because NetCenterPageSubclass is defined
// before UnadviseConnectivityEvents in the file.
static void UnadviseConnectivityEvents();
static UINT GetNcTeardownMessage();

static LRESULT CALLBACK NetCenterPageSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass) {
    if (uMsg == WM_NCDESTROY || (g_ncTeardownMsg && uMsg == g_ncTeardownMsg)) {
        Wh_Log(L"[NetMap] NetCenter page closed/torn down, cleaning up live refresh state");
        UnadviseConnectivityEvents();
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, NetCenterPageSubclass);
        if (g_ncHostWindow == hWnd)
            g_ncHostWindow = nullptr;
        if (uMsg != WM_NCDESTROY)
            return 0;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Helper: find the DirectUIHWND that actually hosts the NetCenter page for
// the current thread. Called from SetXMLFromResource_Hook on the STA thread
// that owns the page.
//
// EnumThreadWindows() only enumerates top-level (non-child) windows of the
// thread, and DirectUIHWND is a child window, so it never shows up there.
// Walk the expected child chain explicitly instead, the way
// classic-explorer-statusbar does: CabinetWClass -> ShellTabWindowClass ->
// DUIViewWndClassName -> DirectUIHWND. The Control Panel's standalone host
// (#32770 / CtrlPanel) doesn't have a ShellTabWindowClass in between, so
// fall back to searching directly for DUIViewWndClassName/DirectUIHWND
// under it.
static HWND FindNetCenterHostWindow() {
    struct FindInfo { HWND found; };
    FindInfo fi = { NULL };
    EnumThreadWindows(GetCurrentThreadId(), [](HWND hwndFrame, LPARAM lp) -> BOOL {
        FindInfo* pfi = reinterpret_cast<FindInfo*>(lp);
        WCHAR cls[128];
        if (!GetClassNameW(hwndFrame, cls, 128))
            return TRUE;

        HWND shellTab = nullptr;
        if (wcscmp(cls, L"CabinetWClass") == 0)
            shellTab = FindWindowExW(hwndFrame, NULL, L"ShellTabWindowClass", NULL);

        HWND duiView = shellTab
            ? FindWindowExW(shellTab, NULL, L"DUIViewWndClassName", NULL)
            : (wcscmp(cls, L"CabinetWClass") != 0
                   ? FindWindowExW(hwndFrame, NULL, L"DUIViewWndClassName", NULL)
                   : nullptr);

        HWND directUI = duiView ? FindWindowExW(duiView, NULL, L"DirectUIHWND", NULL) : nullptr;
        if (directUI) {
            pfi->found = directUI;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&fi));
    return fi.found;
}

static void UnadviseConnectivityEvents() {
    if (g_ncCP) {
        if (g_ncEventsAdvised)
            g_ncCP->Unadvise(g_ncCookie);
        g_ncCP->Release();
        g_ncCP = nullptr;
    }
    g_ncEventsAdvised = false;
    g_ncCookie = 0;
    g_ncTarget = nullptr;
    g_ncModule = nullptr;
    g_ncP4 = nullptr;
}

// Registers (once) the private message used to ask NetCenterPageSubclass to
// tear down from Wh_ModUninit, running on an arbitrary Windhawk thread, by
// marshaling the request to the page's own STA thread via SendMessage.
static UINT GetNcTeardownMessage() {
    if (!g_ncTeardownMsg)
        g_ncTeardownMsg = RegisterWindowMessageW(L"Win7NetFlyout_NcTeardown");
    return g_ncTeardownMsg;
}

// Called from Wh_ModUninit. Marshals the subclass removal and the COM
// Unadvise/Release to the STA thread that owns g_ncCP, instead of touching
// that raw, unmarshalled pointer from whatever thread Windhawk calls
// Wh_ModUninit on.
static void TeardownNetCenterHost() {
    HWND host = g_ncHostWindow;
    if (host && IsWindow(host)) {
        SendMessageW(host, GetNcTeardownMessage(), 0, 0);
    } else {
        // No live host window (e.g. hook never fired, or it was already
        // destroyed): still clear any leftover state directly.
        UnadviseConnectivityEvents();
    }
    g_ncHostWindow = nullptr;
}

static bool IsNetCenter(HMODULE h) {
    if (!h)
        return false;
    // Fast path: compare directly against the cached module handle instead
    // of touching the filesystem on every matching LoadImageW call. Only
    // falls back to path parsing if the cache misses (e.g. netcenter.dll
    // reloaded at a different base, or not yet resolved).
    HMODULE hNetCenter = GetModuleHandleW(L"netcenter.dll");
    if (hNetCenter && hNetCenter == h)
        return true;
    wchar_t path[MAX_PATH];
    if (!GetModuleFileNameW(h, path, MAX_PATH))
        return false;
    return _wcsicmp(PathFindFileNameW(path), L"netcenter.dll") == 0;
}

// Replace every occurrence of the connected SSID/profile name, including
// compound native labels such as "Wi-Fi (PosteMobile-12673059)". This is
// deliberately a substring replacement: the Control Panel connection link
// includes the name in parentheses, whereas the active-network heading draws
// it as a standalone string.
static bool MaskConnectedNetworkText(LPCWSTR text, int textLength,
                                     std::wstring& masked) {
    if (!g_Settings.privacyMode || !text)
        return false;
    int len = (textLength >= 0) ? textLength : lstrlenW(text);
    if (len <= 0)
        return false;

    WCHAR privateName[64] = {0};
    StringCchPrintfW(privateName, ARRAYSIZE(privateName),
                     LOC(STR_NETWORK_PRIVACY_FMT), 1);
    masked.assign(text, len);
    bool changed = false;

    auto ReplaceName = [&](const WCHAR* realName) {
        if (!realName || !realName[0]) return;
        const std::wstring needle(realName);
        size_t pos = 0;
        while ((pos = masked.find(needle, pos)) != std::wstring::npos) {
            masked.replace(pos, needle.length(), privateName);
            pos += lstrlenW(privateName);
            changed = true;
        }
    };

    // Copy connected network names under the critical section
    EnterCriticalSection(&g_Ctx.csLock);
    std::vector<std::wstring> connectedNames;
    for (int i = 0; i < g_NetworkCount; ++i) {
        if (g_NetworkList[i].connState == CONN_STATE_CONNECTED)
            connectedNames.push_back(std::wstring(g_NetworkList[i].ssid));
    }
    BOOL ethernetConnected = g_EthernetConnected;
    WCHAR ethernetName[64] = {0};
    StringCchCopyW(ethernetName, ARRAYSIZE(ethernetName), g_EthernetNetworkName);
    LeaveCriticalSection(&g_Ctx.csLock);

    for (const auto& name : connectedNames)
        ReplaceName(name.c_str());
    if (ethernetConnected)
        ReplaceName(ethernetName);
    return changed;
}

static int WINAPI DrawTextW_Hook(HDC hdc, LPCWSTR text, int textLength,
                                 LPRECT rect, UINT format) {
    // Only mask text when called from netcenter.dll to avoid mangling
    // unrelated strings process-wide (e.g. filenames containing the SSID).
    void* ra = __builtin_return_address(0);
    if (ra && !IsInNetCenter(ra))
        return DrawTextW_Orig ? DrawTextW_Orig(hdc, text, textLength, rect, format) : 0;

    std::wstring masked;
    if (DrawTextW_Orig && MaskConnectedNetworkText(text, textLength, masked))
        return DrawTextW_Orig(hdc, masked.c_str(), (int)masked.length(), rect, format);
    return DrawTextW_Orig ? DrawTextW_Orig(hdc, text, textLength, rect, format) : 0;
}

// DirectUI loads icon() graphics through LoadImageW. Only the two private
// IDs emitted by IconAttr are replaced, leaving Configure a Network and
// Troubleshoot (and every other stock page icon) untouched.
static HANDLE WINAPI LoadImageW_Hook(HINSTANCE hInst, LPCWSTR name, UINT type,
                                     int width, int height, UINT flags) {
    if (type == IMAGE_ICON && hInst && name) {
        // DirectUI normally passes MAKEINTRESOURCE, but accept the equivalent
        // numeric string as well for builds that preserve the parsed token.
        int resourceId = 0;
        if (IS_INTRESOURCE(name))
            resourceId = (int)(UINT_PTR)name;
        else if (wcscmp(name, L"32753") == 0)
            resourceId = kConnectCustomIconId;
        else if (wcscmp(name, L"32754") == 0)
            resourceId = kHomegroupCustomIconId;
        else if (wcscmp(name, L"32755") == 0)
            resourceId = kComputerIconId;
        else if (wcscmp(name, L"32756") == 0)
            resourceId = kNetMapCategoryIconId;
        else if (wcscmp(name, L"32757") == 0)
            resourceId = kGlobeIconId;
        else if (wcscmp(name, L"32758") == 0)
            resourceId = kNoInternetXIconId;
        else if (wcscmp(name, L"32760") == 0)
            resourceId = kOfflineNetworkIconId;

        int sourceIconId = 0;
        if (resourceId == kConnectCustomIconId)
            sourceIconId = 22;
        else if (resourceId == kHomegroupCustomIconId)
            sourceIconId = 27;

        if (sourceIconId && IsNetCenter(hInst)) {
            if (HICON icon = CopyNetworkCenterIcon(sourceIconId, width, height))
                return icon;
            // Decoding failed: preserve the previous Windows icon fallback.
            return LoadImageW_Orig ? LoadImageW_Orig(hInst,
                MAKEINTRESOURCEW(sourceIconId), type, width, height, flags) : NULL;
        }

        if (resourceId == kComputerIconId) {
            if (!IsNetCenter(hInst))
                return LoadImageW_Orig ? LoadImageW_Orig(hInst, name, type, width, height, flags) : NULL;
            int wantW = (width  > 0) ? width  : ScaleDpi(48);
            int wantH = (height > 0) ? height : ScaleDpi(48);
            if (!g_hIconNetworkMapDUI || g_iconNetworkMapDUIW != wantW || g_iconNetworkMapDUIH != wantH) {
                if (g_hIconNetworkMapDUI) { DestroyIcon(g_hIconNetworkMapDUI); g_hIconNetworkMapDUI = NULL; }
                g_hIconNetworkMapDUI = CreateIconFromBase64PNG(PC_ICON_BASE64, wantW, wantH);
                g_iconNetworkMapDUIW = wantW;
                g_iconNetworkMapDUIH = wantH;
            }
            Wh_Log(L"[NetMap-Icon] kComputerIconId requested at %dx%d, g_hIconNetworkMapDUI=%p", wantW, wantH, g_hIconNetworkMapDUI);
            if (g_hIconNetworkMapDUI) {
                HICON copy = CopyIcon(g_hIconNetworkMapDUI);
                Wh_Log(L"[NetMap-Icon] Returning PC icon: %p", copy);
                return copy;
            }
            Wh_Log(L"[NetMap-Icon] g_hIconNetworkMapDUI is NULL");
            return NULL;
        }
        if (resourceId == kGlobeIconId) {
            if (!IsNetCenter(hInst))
                return LoadImageW_Orig ? LoadImageW_Orig(hInst, name, type, width, height, flags) : NULL;
            int wantW = (width  > 0) ? width  : ScaleDpi(48);
            int wantH = (height > 0) ? height : ScaleDpi(48);
            // The same connectivity test used by the flyout selects a gray
            // globe whenever this connection has no Internet access.
            BOOL online = IsInternetConnected();
            if (!g_hIconGlobeDUI || g_iconGlobeDUIW != wantW ||
                g_iconGlobeDUIH != wantH || g_iconGlobeDUIOnline != online) {
                if (g_hIconGlobeDUI) { DestroyIcon(g_hIconGlobeDUI); g_hIconGlobeDUI = NULL; }
                g_hIconGlobeDUI = CreateIconFromBase64PNG(
                    online ? GLOBE_ICON_BASE64 : GLOBE_ICON_OFFLINE_BASE64,
                    wantW, wantH);
                g_iconGlobeDUIW = wantW;
                g_iconGlobeDUIH = wantH;
                g_iconGlobeDUIOnline = online;
            }
            Wh_Log(L"[NetMap-Icon] Globe requested at %dx%d (online=%d), icon=%p",
                   wantW, wantH, online, g_hIconGlobeDUI);
            return g_hIconGlobeDUI ? CopyIcon(g_hIconGlobeDUI) : NULL;
        }

        if (resourceId == kNoInternetXIconId) {
            if (!IsNetCenter(hInst))
                return LoadImageW_Orig ? LoadImageW_Orig(hInst, name, type, width, height, flags) : NULL;
            int wantW = (width > 0) ? width : ScaleDpi(16);
            int wantH = (height > 0) ? height : ScaleDpi(16);
            if (!g_hIconNoInternetXDUI || g_iconNoInternetXDUIW != wantW ||
                g_iconNoInternetXDUIH != wantH) {
                if (g_hIconNoInternetXDUI) { DestroyIcon(g_hIconNoInternetXDUI); g_hIconNoInternetXDUI = NULL; }
                g_hIconNoInternetXDUI = CreateIconFromBase64PNG(
                    NETWORK_NO_INTERNET_X_BASE64, wantW, wantH);
                g_iconNoInternetXDUIW = wantW;
                g_iconNoInternetXDUIH = wantH;
            }
            return g_hIconNoInternetXDUI ? CopyIcon(g_hIconNoInternetXDUI) : NULL;
        }

        if (resourceId == kOfflineNetworkIconId) {
            if (!IsNetCenter(hInst))
                return LoadImageW_Orig ? LoadImageW_Orig(hInst, name, type, width, height, flags) : NULL;
            int wantW = width > 0 ? width : ScaleDpi(36);
            int wantH = height > 0 ? height : ScaleDpi(36);
            if (!g_hIconOfflineNetworkDUI || g_iconOfflineNetworkDUIW != wantW ||
                g_iconOfflineNetworkDUIH != wantH) {
                if (g_hIconOfflineNetworkDUI) {
                    DestroyIcon(g_hIconOfflineNetworkDUI);
                    g_hIconOfflineNetworkDUI = NULL;
                }
                g_hIconOfflineNetworkDUI = CreateIconFromBase64PNG(
                    NETLOC_PUBLIC_OFFLINE_ICON_BASE64, wantW, wantH);
                g_iconOfflineNetworkDUIW = wantW;
                g_iconOfflineNetworkDUIH = wantH;
                Wh_Log(L"[NetMap-Icon] Decoded offline gray network icon at %dx%d", wantW, wantH);
            }
            return g_hIconOfflineNetworkDUI ? CopyIcon(g_hIconOfflineNetworkDUI) : NULL;
        }

        if (resourceId == kNetMapCategoryIconId) {
            if (!IsNetCenter(hInst))
                return LoadImageW_Orig ? LoadImageW_Orig(hInst, name, type, width, height, flags) : NULL;
            // Decode at DirectUI's requested 36rp active-network size. This
            // uses the same bicubic scaling path as the PC/globe DUI caches.
            return CopyNetworkLocationIconForDUI(width, height);
        }
    }
    return LoadImageW_Orig ? LoadImageW_Orig(hInst, name, type, width, height, flags) : NULL;
}

static HRESULT NCL_THISCALL SetXMLFromResource_Hook(void* t, PCWSTR n, PCWSTR tp, HMODULE m,
                                                HINSTANCE p4, HINSTANCE p5) {
    if (!SetXMLFromResource_Orig)
        return E_FAIL;
    if (!SetXML || g_inHook)
        return SetXMLFromResource_Orig(t, n, tp, m, p4, p5);

    if (!IsNetCenter(m) || !tp || _wcsicmp(tp, L"UIFILE") || !IS_INTRESOURCE(n) ||
        (UINT)(UINT_PTR)n != 110)
        return SetXMLFromResource_Orig(t, n, tp, m, p4, p5);

    std::wstring xml = LoadUifile(m, n, tp);
    if (xml.empty() || xml.find(L"atom(NetworkCenter)") == std::wstring::npos ||
        xml.find(L"atom(diagnosebtn)") == std::wstring::npos)
        return SetXMLFromResource_Orig(t, n, tp, m, p4, p5);

    std::wstring patched = Patch(xml);
    if (patched == xml)
        return SetXMLFromResource_Orig(t, n, tp, m, p4, p5);

    g_inHook++;
    HRESULT hr = SetXML(t, patched.c_str(), m, p4);
    g_inHook--;

    if (SUCCEEDED(hr)) {
        // Remember this page instance so connectivity-change notifications
        // can re-push a freshly patched XML into it later (live refresh),
        // instead of only ever showing the state from when the page opened.
        g_ncTarget = t;
        g_ncModule = m;
        g_ncP4 = p4;
        EnsureConnectivityEventsAdvised();

        // Subclass the DirectUIHWND that actually hosts the page (not the
        // top-level frame, which survives navigation to other Control Panel
        // pages) so cleanup happens both when the Control Panel window is
        // closed and when this page's DirectUIHWND itself goes away.
        HWND hNcHost = FindNetCenterHostWindow();
        if (hNcHost) {
            if (g_ncHostWindow && g_ncHostWindow != hNcHost) {
                // A different DirectUIHWND is hosting the page now (e.g. the
                // previous one was torn down and recreated); drop the old
                // subclass explicitly rather than leaving it to rely solely
                // on WM_NCDESTROY racing with this call.
                WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_ncHostWindow, NetCenterPageSubclass);
            }
            g_ncHostWindow = hNcHost;
            WindhawkUtils::SetWindowSubclassFromAnyThread(hNcHost, NetCenterPageSubclass, 0);
            Wh_Log(L"[NetMap] Subclassed NetCenter host window (0x%p)", hNcHost);
        } else {
            Wh_Log(L"[NetMap] Could not find NetCenter host window to subclass");
        }
    }

    return FAILED(hr) ? SetXMLFromResource_Orig(t, n, tp, m, p4, p5) : hr;
}

static bool HookAll() {
    if (g_hookInstalled)
        return true;

    HMODULE dui = LoadLibraryExW(L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!dui) {
        Wh_Log(L"Network Center links: dui70.dll could not be loaded - "
               L"DirectUI may have been removed/relocated by this Windows build");
        return false;
    }

    // SetXML is public; resolving it directly avoids a no-op hook on a hot
    // DirectUI path merely to obtain a trampoline.
    for (auto n : {"?SetXML@DUIXmlParser@DirectUI@@QEAAJPEBGPEAUHINSTANCE__@@1@Z",
                   "?SetXML@DUIXmlParser@DirectUI@@QAAJPBGPAUHINSTANCE__@@1@Z"}) {
        if (FARPROC p = GetProcAddress(dui, n)) {
            SetXML = reinterpret_cast<SetXML_t>(p);
            break;
        }
    }
    if (!SetXML) {
        Wh_Log(L"Network Center links: DUIXmlParser::SetXML symbol not found in "
               L"dui70.dll - mangled signature likely changed in this Windows build");
        return false;
    }

    for (auto n : {
#ifdef _WIN64
             "?_SetXMLFromResource@DUIXmlParser@DirectUI@@IEAAJPEBG0PEAUHINSTANCE__@@11@Z",
#endif
             "?_SetXMLFromResource@DUIXmlParser@DirectUI@@IAEJPBG0PAUHINSTANCE__@@11@Z"}) {
        if (FARPROC p = GetProcAddress(dui, n)) {
            auto target = reinterpret_cast<SetXMLFromResource_t>(p);
            if (WindhawkUtils::SetFunctionHook(target, SetXMLFromResource_Hook,
                                                &SetXMLFromResource_Orig)) {
                break;
            }
        }
    }
    if (!SetXMLFromResource_Orig) {
        Wh_Log(L"Network Center links: DUIXmlParser::_SetXMLFromResource symbol "
               L"not found or hook failed - mangled signature likely changed");
    }
    g_hookInstalled = SetXMLFromResource_Orig != nullptr;
    if (g_hookInstalled && !g_iconHookInstalled) {
        g_iconHookInstalled = WindhawkUtils::SetFunctionHook(
            LoadImageW, LoadImageW_Hook, &LoadImageW_Orig);
        if (!g_iconHookInstalled)
            Wh_Log(L"Network Center links: custom icon hook unavailable; using Windows icons");
    }
    // DrawTextW is one of the hottest user32 entry points in the shell.
    // Only install this hook when privacy mode is actually in use, instead
    // of unconditionally in every explorer.exe/control.exe process where it
    // would otherwise do nothing but a pointer-range check on every call.
    if (g_hookInstalled && g_Settings.privacyMode && !g_textHookInstalled) {
        g_textHookInstalled = WindhawkUtils::SetFunctionHook(
            DrawTextW, DrawTextW_Hook, &DrawTextW_Orig);
        if (!g_textHookInstalled)
            Wh_Log(L"Network Center links: privacy text hook unavailable");
    }
    return g_hookInstalled;
}

static bool Init() {
    bool enabled = Wh_GetIntSetting(L"restoreClassicNetworkCenterLinks") != 0;
    g_addConnect = enabled;
    g_addHomegroup = enabled;
    g_addNetworkMap = enabled;  // Visual Network Map rectangle

    // Always install the DirectUI + LoadImageW hooks here, unconditionally,
    // regardless of whether the feature is currently enabled. Per the
    // Windhawk API, all hooks should be set in Wh_ModInit, since
    // Wh_ApplyHookOperations() is called automatically right after it;
    // hooks registered later (e.g. from Wh_ModSettingsChanged) stay pending
    // until the mod explicitly calls Wh_ApplyHookOperations() itself, which
    // SettingsChanged() below does when privacy mode is turned on at
    // runtime. g_addConnect/g_addHomegroup already make Patch() a no-op
    // when the feature is off, so installing the hooks unconditionally has
    // no behavioral effect while it's disabled, and lets it turn on and off
    // correctly at runtime without a mod reload. The DrawTextW hook is the
    // one exception: it's gated on privacyMode in HookAll() above since,
    // unlike the others, it sits on a very hot user32 path.
    if (!HookAll()) {
        Wh_Log(L"Network Center links: DirectUI hook was not installed");
        return false;
    }
    return true;
}

static void SettingsChanged() {
    bool enabled = Wh_GetIntSetting(L"restoreClassicNetworkCenterLinks") != 0;
    g_addConnect = enabled;
    g_addHomegroup = enabled;
    g_addNetworkMap = enabled;  // Visual Network Map rectangle

    // Privacy mode may have just been turned on at runtime: install the
    // DrawTextW hook now (it was skipped in HookAll() while privacy mode
    // was off) and explicitly apply it, since hooks registered outside
    // Wh_ModInit stay pending until Wh_ApplyHookOperations() is called.
    if (g_Settings.privacyMode && g_hookInstalled && !g_textHookInstalled) {
        g_textHookInstalled = WindhawkUtils::SetFunctionHook(
            DrawTextW, DrawTextW_Hook, &DrawTextW_Orig);
        if (g_textHookInstalled)
            Wh_ApplyHookOperations();
        else
            Wh_Log(L"Network Center links: privacy text hook unavailable");
    }
}

#undef NCL_THISCALL

}  // namespace Win7NetworkCenterLinks


BOOL Wh_ModInit() {
    Wh_Log(L"=== Wh_ModInit ===");
    DetectWindowsVersion();
    LoadSettings();
    DetermineLocale();

    // g_Ctx.csLock must be ready before Win7NetworkCenterLinks::Init() below
    // installs its DirectUI/DrawTextW hooks: those hooks run in control.exe
    // too (via SetXMLFromResource_Hook / DrawTextW_Hook -> 
    // GetConnectedNetworkName() / MaskConnectedNetworkText(), both of which
    // take this lock), and control.exe never reaches the g_IsExplorerHost
    // branch that used to initialize this critical section. Entering an
    // uninitialized CRITICAL_SECTION there was undefined behavior that, in
    // practice, deadlocked the Control Panel page.
    ZeroMemory(&g_Ctx, sizeof(g_Ctx));
    InitializeCriticalSection(&g_Ctx.csLock);

    if (!Win7NetworkCenterLinks::Init()) {
        // The flyout does not depend on this optional Control Panel feature.
        Wh_Log(L"Network Center links: DirectUI hook was not installed");
    }

    g_IsExplorerHost = IsExplorerProcess();
    if (!g_IsExplorerHost) {
        g_Initialized = TRUE;
        return TRUE;
    }

    DarkContextMenu::Init();
    g_hConnectMutex.reset(CreateMutexW(NULL, FALSE, L"Local\\Win7NetFlyout_ConnectMutex"));
    g_uTaskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
    LoadSystemIcons();
    InitGdiPlusRendering();
    InitGlobalFonts();
    InitRefreshButtonRect();
    RecalcArrowRect();
    InstallTrayInterceptionInternal();
    // Priming of Ethernet/registry state and the Home/Public/Work category is
    // done on the hotkey thread (right after CoInitializeEx), not here: this
    // is the Windhawk init thread, which has no COM apartment, so calling
    // RefreshNetworkData(TRUE) here would silently no-op the NLM half of the
    // detection (CO_E_NOTINITIALIZED) while still paying the synchronous
    // GetAdaptersAddresses + registry enumeration cost during explorer
    // startup, and risks creating g_pNLM in the wrong apartment.
    g_Ctx.hHotkeyThread = CreateThread(NULL, 0, HotkeyThreadProc, &g_Ctx, 0, &g_Ctx.dwHotkeyThreadId);
    if (!g_Ctx.hHotkeyThread) {
        DeleteCriticalSection(&g_Ctx.csLock);
        return FALSE;
    }
    g_Initialized = TRUE;
    return TRUE;
}

void Wh_ModSettingsChanged() {
    BOOL oldRoundedCorners = g_Settings.useRoundedCorners;
    int  oldTheme          = g_Settings.theme;

    LoadSettings();
    DetermineLocale();
    Win7NetworkCenterLinks::SettingsChanged();

    if (!g_IsExplorerHost)
        return;

    BOOL needRecreate = (oldRoundedCorners != g_Settings.useRoundedCorners)
                     || (oldTheme          != g_Settings.theme);

    if (needRecreate) {
        if (g_hWndFlyout && IsWindow(g_hWndFlyout)) {
            BOOL wasVisible = IsWindowVisible(g_hWndFlyout);
            SendMessageW(g_hWndFlyout, WM_SAFE_CLOSE, 0, 0);
            if (wasVisible) ToggleFlyoutWindow();
        }
        return;
    }
    if (SafeToAccessUI() && g_hWndFlyout) {
        if (g_dwFlyoutOwnerThreadId) {
            PostThreadMessageW(g_dwFlyoutOwnerThreadId, WM_UPDATE_REFRESH_TIMER, 0, 0);
        }
        // Update hotkey registration when the setting changes
        if (g_Ctx.dwHotkeyThreadId)
            PostThreadMessageW(g_Ctx.dwHotkeyThreadId, WM_UPDATE_HOTKEY, 0, 0);
        // Re-prime the category on any settings change (e.g. enabling
        // "useNetworkLocationIcons"), marshaled to the flyout thread (which
        // owns g_pNLM and all the shared network state) via a force flag on
        // WM_REFRESH_DATA, instead of calling RefreshNetworkData() directly
        // from this (Windhawk callback) thread.
        PostMessageW(g_hWndFlyout, WM_REFRESH_DATA, /*forceDetection=*/TRUE, 0);
        InvalidateRect(g_hWndFlyout, NULL, TRUE);
    }
}

void Wh_ModUninit() {
    if (!g_IsExplorerHost) {
        // control.exe can create only the in-memory Network Center icons.
        // Marshal the subclass removal + COM Unadvise/Release to the STA
        // thread that owns them (this callback can run on an arbitrary
        // Windhawk thread), instead of touching the raw, unmarshalled
        // IConnectionPoint pointer from here directly.
        Win7NetworkCenterLinks::TeardownNetCenterHost();
        FreeSystemIcons();
        DeleteCriticalSection(&g_Ctx.csLock);
        return;
    }

    Win7NetworkCenterLinks::TeardownNetCenterHost();
    SafeCleanup();
    DeleteCriticalSection(&g_Ctx.csLock);
    DarkContextMenu::Uninit();
    UnregisterClassW(L"Win7NetworkFlyoutSafe", GetModuleHandle(NULL));
    UnregisterClassW(L"Win7NetPwdClass", GetModuleHandle(NULL));
}
