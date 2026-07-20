// ==WindhawkMod==
// @id             win7-network-flyout-recreation
// @name           Windows 7 Network Flyout Recreation
// @description    This mod recreates the Windows 7 network flyout for Windows 10 and 11 along with some more configurable restorations
// @version        3.2.0
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

This mod recreates the classic Windows 7 network flyout on Windows 10 and 11, replacing the modern flyout with a recreation of the familiar, lightweight alternative from Windows 7.

Screenshot of the light theme:

![Screenshot](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/pic.PNG)

Screenshot of the dark theme:

![Screenshot](https://raw.githubusercontent.com/babamohammed2022/gtasashtml/main/dark.png)

Screenshot of the restored Control Panel links:

![Screenshot](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/immagine.webp)

The mod has been tested on Windows 10 21H2, Windows 10 1809, Windows 11 23H2, Windows 11 24H2 and Windows 11 25H2.


## Features
- **Wi-Fi network list**: Shows all available networks with live signal strength
- **Connect/Disconnect**: Connect to secured and open networks with password support
- **Privacy mode**: Hide real network names (shows as Network 1, Network 2...)
- **Windows 7 style tooltips**: Full network info on hover (SSID, signal, security type)
- **Right-click context menu**: Quick access to network status and properties
- **Keyboard navigation**: Full Arrow keys, Enter, and Escape support
- **Auto-refresh**: Periodically refreshes the network list at a configurable interval
- **Language support**: English, Italian, Spanish, French, Russian, German, Portuguese or auto-detect
- **DPI aware**: Scales correctly on high-DPI and mixed-DPI setups
- **Rounded corners**: Optional modern look for Windows 11 or Aero theme
- **Dual Theme Support**: Includes both light and dark themes, with the dark theme created specifically for late-night use and, if present, dark Aero theme.
- **NEW: Ethernet Support**: The mod should now properly show the flyout for Ethernet connection.
- **NEW: Classic Network Center links**: Optionally restores the Windows 7 “Connect to a network” and HomeGroup/sharing links with their custom artwork.


## Requirements
- **Windows 10** with the native taskbar
- **Windows 11** with the Windows 10 taskbar (via [ExplorerPatcher](https://github.com/valinet/ExplorerPatcher) or similar mods)
- The network icon must be visible in the main system tray (overflow menu not supported)

> **Note:** This mod is unlikely to work with some taskbar mods (e.g. Retrobar because they don't use the ToolbarWindow32) or heavily customized and unstable configurations.

## Known limitations
- **Overflow menu**: The network icon must be in the main system tray, not hidden in the overflow menu.
- **Auto-reconnect checkbox**: May not work reliably on all setups. If the network doesn't reconnect automatically, try connecting manually.

## Hotkeys
| Key | Action |
|-----|--------|
| **Ctrl+H** | Toggle network flyout (disabled by default) |

## Credits
- **m417z** — Code review
- **Anixx** — Testing on Windows 11 23H2 and feedback
- **sebastian08dm08-cpu** — Testing on Windows 10 1809

If you encounter issues, please report them on the author of the mod.
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
  $name: Restore classic Network Center links
  $description: Add the Windows 7 “Connect to a network” and HomeGroup/sharing links to the ''Network and Sharing Center'' page in the Control Panel.
- theme: light
  $name: Theme
  $description: Select the network flyout's theme
  $options:
    - light: Light (Classic Windows 7)
    - dark: Dark (Custom)
*/
// ==/WindhawkModSettings==
// ## Changelog 
// - 3.2.0: Fixed "Restore classic Network Center links" not taking effect at runtime
//   until a reload (hooks are now installed unconditionally in Wh_ModInit and gated
//   by the existing enable/disable flags).
// - 3.2.0: Removed a dead #ifndef UNICODE preprocessor block (Windhawk always compiles
//   with UNICODE defined, and <psapi.h> was already included unconditionally below).
// - 3.1.0: Network Center artwork is now handled safely in memory.
// - 3.1.0: Saved this source as UTF-8 and corrected the Russian language name in settings.
// - 3.1.0: Added supplied Network Center PNG artwork as in-memory Base64 icons; no DLL or temporary file is used.
// - Added full Ethernet / wired LAN connection detection and classic Windows 7 UI support
// - Added compact flyout layout when no Wi-Fi networks are available (or Ethernet only), matching original Windows 7 LAN design
// - Enhanced system stability by supporting systems without Wi-Fi adapters
// - Restored the 2 links inside the "Network and Sharing Center" control panel page like in Windows 7
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

#include <strsafe.h>
#include <shellapi.h>
#include <commctrl.h>
#include <windhawk_api.h>
#include <netlistmgr.h>
#include <windhawk_utils.h>
#include <process.h>
#include <shlwapi.h>
#include <string>

// =========================================================
// Dark context menu support (right-click menu only)
#define WINDOW_WIDTH_BASE        300
#define WINDOW_HEIGHT_BASE       405
#define HEADER_HEIGHT_BASE       105
#define FOOTER_HEIGHT_BASE       60
#define ROW_HEIGHT_NORMAL_BASE   26
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

// Define settings early so they are available to RecalcDpiMetrics and UI helpers
struct ModSettings {
    BOOL interceptNativeFlyout;
    BOOL privacyMode;
    int  refreshInterval;
    int  language;
    BOOL enableHotkey;
    BOOL useRoundedCorners;
    int  theme;          // 0=light, 1=dark
} g_Settings = { TRUE, FALSE, 3000, 0, FALSE, TRUE, 0 };

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

    int raw_enableHotkey = Wh_GetIntSetting(L"enableHotkey");
    int raw_roundedCorners = Wh_GetIntSetting(L"useRoundedCorners");
    WindhawkUtils::StringSetting theme = WindhawkUtils::StringSetting::make(L"theme");
    int raw_theme = (_wcsicmp(theme.get(), L"dark") == 0) ? 1 : 0;
    
    g_Settings.interceptNativeFlyout     = raw_intercept   != 0;
    g_Settings.privacyMode              = raw_privacy     != 0;
    g_Settings.refreshInterval           = raw_refresh;
    g_Settings.language                 = raw_language;
    g_Settings.enableHotkey             = raw_enableHotkey != 0;
    g_Settings.useRoundedCorners        = raw_roundedCorners != 0;
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
static const WCHAR* REFRESH_ICON_NORMAL_BASE64 = L"iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsIAAA7CARUoSoAAAAAZdEVYdFNvZnR3YXJlAFBhaW50Lk5FVCA1LjEuMTITAUd0AAAAuGVYSWZJSSoACAAAAAUAGgEFAAEAAABKAAAAGwEFAAEAAABSAAAAKAEDAAEAAAACAAAAMQECABEAAABaAAAAaYcEAAEAAABsAAAAAAAAAPJ2AQDoAwAA8nYBAOgDAABQYWludC5ORVQgNS4xLjEyAAADAACQBwAEAAAAMDIzMAGgAwABAAAAAQAAAAWgBAABAAAAlgAAAAAAAAACAAEAAgAEAAAAUjk4AAIABwAEAAAAMDEwMAAAAACDfy8cctDT3wAAAN9JREFUOE9joAkw3nrrPwhDuaSB6Ree/zddcZVozUxQGg4WHn/OkKLAB+URBigGzDzy8D/j9z8MPEyMDNeuXVtx9epVMagUToBiwILDTxkYfv5hePr2O4OWllaEtrb2K6gUGFi2H8PwGiOIsKw/hNPPxxvtMNTAxEAAzrCs2IthyPEOZ4hmPHJwA0DAsnAHwpZ+D4hmJDF0AFODAiyzN2HV0LX8wn+QHC55ooB54rL/M9aeJ8+AqcuO/zcMmYKhGSMh4QJTF25nSPTRgPJIAIdPXFZXME/6D8JQoUEFGBgAn8daV7VTN5UAAAAASUVORK5CYII=////v7+/r6+vj4+Pz8/P7+/v39/TO12sjo8fHx8fn5+ZfQ5zWo1erq6ubm5vf398jh7jiXzpnI4+Li4tLS0unp6ZnE4TmOyqXK5NbW1tXV1e/v7zmHxoq32/Pz8/T09Pb29jl/wvDw8Dl4vTlxuDlrstjY2Iqn0Ofn5+Pj48/Pz9/f3+jo6KS21Tdhppitz9PT07u7u9vb2+iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAMAAAAoLQ9TAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAABRUExURev0/TO12pfQ5zWo1TiXzpnI45nE4TmOyqfL5Orz/DmHxo663Dl/wkuKyESGxTl4vTlxuDlrsoqn0Ddhppitz5WmxzFUlCpHfpKgvMPI0yA3YglAoVgAAAAJcEhZcwAADsIAAA7CARUoSoAAAAAZdEVYdFNvZnR3YXJlAFBhaW50Lk5FVCA1LjEuMTITAUd0AAAAuGVYSWZJSSoACAAAAAUAGgEFAAEAAABKAAAAGwEFAAEAAABSAAAAKAEDAAEAAAACAAAAMQECABEAAABaAAAAaYcEAAEAAABsAAAAAAAAAPJ2AQDoAwAA8nYBAOgDAABQYWludC5ORVQgNS4xLjEyAAADAACQBwAEAAAAMDIzMAGgAwABAAAAAQAAAAWgBAABAAAAlgAAAAAAAAACAAEAAgAEAAAAUjk4AAIABwAEAAAAMDEwMAAAAACDfy8cctDT3wAAAFRJREFUKFN9yEkSgDAIRFHibJyIs7n/QS2goxvLt2h+Qd+cQ0CWI5KiREBVNy3SeN/Z1aVeDKOGfSbxHMHMOsI+QXcOdl/LioBtRyTHiTBXjKh/RDeDBAMcwXjgKAAAAABJRU5ErkJggg==//9T3qpBX7Ilk83uLCM4kMlo+rsnBAwjm0Mq6DJfQLFkpoJWrlRrdes3IRvNVrtjaxySi/dCx+kCroQDzxcMvQ+gBoHPJPoEviVg3EMYhqBEgAk/QIRBDRhGETBStpQynkyteDZfLFdrdVb4ySjxTHez3e2XCRkRHFzF4Xg6xyCB0C7XK843Vzn7oEu18P74+xB26ZmYOnsBTi4RDe3fqLQAAAAASUVORK5CYII=";


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

static HICON g_hIconNetworkCenterConnect = NULL;
static HICON g_hIconNetworkCenterHomegroup = NULL;

static HICON g_hIconRefreshNormal = NULL;
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

void Init() {
    g_hUxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (g_hUxtheme) {
        pSetPreferredAppMode = (SetPreferredAppMode_T)GetProcAddress(g_hUxtheme, MAKEINTRESOURCEA(135));
        pFlushMenuThemes     = (FlushMenuThemes_T)GetProcAddress(g_hUxtheme, MAKEINTRESOURCEA(136));
    }
}

void OnSettingsChanged() {
    return;
}

void Uninit() {
    Apply(FALSE);
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
    return (g_Settings.theme == 1) ? RGB(30, 30, 30) : RGB(255, 255, 255);
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
            Wh_Log(L"Win11: SysPager found but no ToolbarWindow32 — partial legacy taskbar");
        } else if (hNotify) {
            Wh_Log(L"Win11: TrayNotifyWnd found but no SysPager — modern taskbar only");
        } else if (hTray) {
            Wh_Log(L"Win11: Shell_TrayWnd found but no TrayNotifyWnd — unusual configuration");
        } else {
            Wh_Log(L"Win11: Shell_TrayWnd not found — taskbar not ready yet");
        }
        WCHAR epPniduiPath[MAX_PATH];
        StringCchPrintfW(epPniduiPath, ARRAYSIZE(epPniduiPath),
                         L"C:\\Program Files\\ExplorerPatcher\\pnidui.dll");
        if (GetFileAttributesW(epPniduiPath) != INVALID_FILE_ATTRIBUTES) {
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
HICON g_hIconSignalBars[6] = { NULL };
HICON g_hIconRefreshWin7 = NULL;
int   g_PendingConnectIndex = -1;
HWND  g_hTooltip = NULL;
UINT_PTR g_RefreshTimer = 0;
UINT_PTR g_TimeoutTimer = 0;
HWND G_hSubclassedToolbar = nullptr;
static BYTE* g_pniduiBase = NULL;
static BYTE* g_pniduiEnd  = NULL;
static HANDLE g_hConnectMutex = NULL;
static HMODULE g_hGdiPlus = NULL;
static ULONG_PTR g_gdiplusToken = 0;
static void* g_pBitmapSignalBars[6] = { NULL };

// Ethernet status variables
static BOOL  g_EthernetConnected = FALSE;
static WCHAR g_EthernetNetworkName[64] = {0};
static BOOL  g_EthernetHasInternet = FALSE;

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
        L"No connections available",
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
        L"Nessuna connessione disponibile",
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
        L"Intensit\u00E0 del segnale:",
        L"Tipo di radio:",
        L"Eccellente",
        L"Buono",
        L"Discreto",
        L"Scarso",
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
        L"Risoluzione dei problemi",
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
        L"No hay conexiones disponibles",
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
        L"Aucune connexion disponible",
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
        L"\u041D\u0435\u0442 \u0434\u043E\u0441\u0442\u0443\u043F\u043D\u044B\u0445 \u043F\u043E\u0434\u043A\u043B\u044E\u0447\u0435\u043D\u0438\u0439",
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
        L"Keine Verbindungen",
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
        L"Sem liga\u00E7\u00F5es",
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

static HICON CreateIconFromBase64PNG(const WCHAR* base64Str, int targetWidth = 0,
                                      int targetHeight = 0) {
    static const WCHAR* tbl = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int len = lstrlenW(base64Str);
    while (len > 0 && base64Str[len-1] == L'=') len--;
    DWORD outLen = (len * 3) / 4;
    BYTE* data = (BYTE*)malloc(outLen);
    if (!data) return NULL;
    DWORD val = 0;
    int bits = -8, pos = 0;
    for (int i = 0; i < len; i++) {
        const WCHAR* p = wcschr(tbl, base64Str[i]);
        if (!p) continue;
        val = (val << 6) | (DWORD)(p - tbl);
        bits += 6;
        if (bits >= 0) { data[pos++] = (val >> bits) & 0xFF; bits -= 8; }
    }
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, outLen);
    if (!hMem) { free(data); return NULL; }
    memcpy(GlobalLock(hMem), data, outLen);
    GlobalUnlock(hMem);
    free(data);
    IStream* stream = NULL;
    CreateStreamOnHGlobal(hMem, TRUE, &stream);
    if (!stream) { GlobalFree(hMem); return NULL; }

    HICON hIcon = NULL;
    HMODULE hGdi = LoadLibraryExW(L"gdiplus.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hGdi) {
        typedef int (WINAPI *GdiplusStartupFunc)(ULONG_PTR*, const void*, void*);
        typedef int (WINAPI *GdipCreateBitmapFromStreamFunc)(IStream*, void**);
        typedef int (WINAPI *GdipCreateHICONFromBitmapFunc)(void*, HICON*);
        typedef int (WINAPI *GdipDisposeImageFunc)(void*);
        typedef void (WINAPI *GdiplusShutdownFunc)(ULONG_PTR);
        GdiplusStartupFunc pStartup = (GdiplusStartupFunc)GetProcAddress(hGdi, "GdiplusStartup");
        GdipCreateBitmapFromStreamFunc pFromStream = (GdipCreateBitmapFromStreamFunc)GetProcAddress(hGdi, "GdipCreateBitmapFromStream");
        GdipCreateHICONFromBitmapFunc pToHICON = (GdipCreateHICONFromBitmapFunc)GetProcAddress(hGdi, "GdipCreateHICONFromBitmap");
        GdipDisposeImageFunc pDispose = (GdipDisposeImageFunc)GetProcAddress(hGdi, "GdipDisposeImage");
        GdiplusShutdownFunc pShutdown = (GdiplusShutdownFunc)GetProcAddress(hGdi, "GdiplusShutdown");
        GdipCreateBitmapFromScan0Func pCreateBitmap =
            (GdipCreateBitmapFromScan0Func)GetProcAddress(hGdi, "GdipCreateBitmapFromScan0");
        GdipGetImageGraphicsContextFunc pGetGraphics =
            (GdipGetImageGraphicsContextFunc)GetProcAddress(hGdi, "GdipGetImageGraphicsContext");
        GdipSetInterpolationModeFunc pSetInterpolation =
            (GdipSetInterpolationModeFunc)GetProcAddress(hGdi, "GdipSetInterpolationMode");
        GdipSetPixelOffsetModeFunc pSetPixelOffset =
            (GdipSetPixelOffsetModeFunc)GetProcAddress(hGdi, "GdipSetPixelOffsetMode");
        GdipGraphicsClearFunc pClear =
            (GdipGraphicsClearFunc)GetProcAddress(hGdi, "GdipGraphicsClear");
        GdipDrawImageRectIFunc pDraw =
            (GdipDrawImageRectIFunc)GetProcAddress(hGdi, "GdipDrawImageRectI");
        GdipDeleteGraphicsFunc pDeleteGraphics =
            (GdipDeleteGraphicsFunc)GetProcAddress(hGdi, "GdipDeleteGraphics");

        if (pStartup && pFromStream && pToHICON && pDispose && pShutdown) {
            ULONG_PTR token = 0;
            struct { DWORD Version; void* Callback; BOOL Suppress; } input = {1, NULL, FALSE};
            if (pStartup(&token, &input, NULL) == 0) {
                void* srcBitmap = NULL;
                if (pFromStream(stream, &srcBitmap) == 0 && srcBitmap) {
                    bool scaled = false;
                    if (targetWidth > 0 && targetHeight > 0 && pCreateBitmap && pGetGraphics &&
                        pSetInterpolation && pSetPixelOffset && pClear && pDraw && pDeleteGraphics) {
                        void* dstBitmap = NULL;
                        if (pCreateBitmap(targetWidth, targetHeight, 0, 0x00E200B, NULL, &dstBitmap) == 0 &&
                            dstBitmap) {
                            void* graphics = NULL;
                            if (pGetGraphics(dstBitmap, &graphics) == 0 && graphics) {
                                // 7 is GDI+'s HighQualityBicubic mode; 3 matches the
                                // pixel-offset mode used by DrawIconBicubic below.
                                pSetInterpolation(graphics, 7);
                                pSetPixelOffset(graphics, 3);
                                pClear(graphics, 0);
                                scaled = pDraw(graphics, srcBitmap, 0, 0,
                                               targetWidth, targetHeight) == 0;
                                pDeleteGraphics(graphics);
                            }
                            if (scaled)
                                pToHICON(dstBitmap, &hIcon);
                            pDispose(dstBitmap);
                        }
                    }
                    if (!scaled)
                        pToHICON(srcBitmap, &hIcon);
                    pDispose(srcBitmap);
                }
                pShutdown(token);
            }
        }
        FreeLibrary(hGdi);
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
    if (!g_pNLM) {
        CoCreateInstance(CLSID_NetworkListManager, NULL, CLSCTX_INPROC_SERVER,
                         IID_INetworkListManager, (void**)&g_pNLM);
    }
    if (!g_pNLM) return FALSE;
    NLM_CONNECTIVITY connectivity;
    if (FAILED(g_pNLM->GetConnectivity(&connectivity))) return FALSE;
    return (connectivity & NLM_CONNECTIVITY_IPV4_INTERNET) ||
           (connectivity & NLM_CONNECTIVITY_IPV6_INTERNET);
}

// -------------------------------------------------------
// Keyboard focus
// -------------------------------------------------------
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
    const int buttonSize = ScaleDpi(16);
    const int margin = ScaleDpi(8);
    int totalListHeight = GetTotalListHeight();
    int availableHeight = LIST_Y_END - LIST_Y_START;
    BOOL hasScrollbar = (totalListHeight > availableHeight);
    int baseX = WINDOW_WIDTH - margin - buttonSize;
    if (!hasScrollbar) {
        baseX += (WINDOW_WIDTH * 4) / 100;     
        baseX -= (WINDOW_WIDTH * 13) / 1000;   
    }
    if (baseX + buttonSize > WINDOW_WIDTH) {
        baseX = WINDOW_WIDTH - buttonSize;
    }
    g_rcRefreshButton.left   = baseX;
    g_rcRefreshButton.top    = ScaleDpi(2);
    g_rcRefreshButton.right  = baseX + buttonSize;
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
        ExtractIconExW(L"netshell.dll", 120, &g_hIconNetworkMap, NULL, 1);
    for (int i = 0; i < 6; i++)
        if (!g_hIconSignalBars[i])
            ExtractIconExW(L"netshell.dll", 152 + i, &g_hIconSignalBars[i], NULL, 1);
    if (!g_hIconRefreshWin7)
        ExtractIconExW(L"shell32.dll", 238, &g_hIconRefreshWin7, NULL, 1);
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
    if (!pGdipCreateBitmapFromHICON || !pGdipSetInterpolationMode || !pGdipDrawImageRectI ||
        !pGdipDeleteGraphics || !pGdipCreateBitmapFromScan0 || !pGdipGetImageGraphicsContext ||
        !pGdipSetPixelOffsetMode || !pGdipGraphicsClear || !pGdipCreateHBITMAPFromBitmap ||
        !pGdipDisposeImage) {
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
    if (g_hGdiPlus && pGdipDisposeImage) {
        for (int i = 0; i < 6; i++) {
            if (g_pBitmapSignalBars[i]) {
                pGdipDisposeImage(g_pBitmapSignalBars[i]);
                g_pBitmapSignalBars[i] = NULL;
            }
        }
    }
    if (g_hIconRefreshNormal) { DestroyIcon(g_hIconRefreshNormal); g_hIconRefreshNormal = NULL; }
    if (g_hIconNetworkCenterConnect) { DestroyIcon(g_hIconNetworkCenterConnect); g_hIconNetworkCenterConnect = NULL; }
    if (g_hIconNetworkCenterHomegroup) { DestroyIcon(g_hIconNetworkCenterHomegroup); g_hIconNetworkCenterHomegroup = NULL; }
    if (g_hIconNetworkMap) { DestroyIcon(g_hIconNetworkMap); g_hIconNetworkMap = NULL; }
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
    LeaveCriticalSection(&g_Ctx.csLock);
    if (g_NetworkCount > 0 && g_NetworkList[0].connState == CONN_STATE_CONNECTED) {
        g_NetworkList[0].hasInternetAccess = IsInternetConnected();
    }
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

    // 1. Find physical operational Ethernet adapter GUID via GetAdaptersAddresses
    BOOL foundPhysicalEthernet = FALSE;
    GUID physicalEthernetGuid = {0};
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

void RefreshNetworkData() {
    if (g_Ctx.hWlanClient) {
        RefreshWifiData(g_Ctx.hWlanClient);
    } else {
        EnterCriticalSection(&g_Ctx.csLock);
        g_NetworkCount = 0;
        LeaveCriticalSection(&g_Ctx.csLock);
    }
    UpdateEthernetStatus();
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
    
    DWORD waitResult = WaitForSingleObject(g_hConnectMutex, 10000);
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
            ReleaseMutex(g_hConnectMutex);
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
    
    ReleaseMutex(g_hConnectMutex);
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

void WINAPI WlanNotificationCallback(PWLAN_NOTIFICATION_DATA data, PVOID context) {
    ModContext* ctx = (ModContext*)context;
    if (!ctx || ctx->isUninitializing || !data) return;
    if (data->NotificationSource != WLAN_NOTIFICATION_SOURCE_ACM) return;
    HWND hFlyout = g_hWndFlyout;
    if (!hFlyout || !IsWindow(hFlyout)) return;
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
            PostMessageW(hFlyout, WM_ASYNC_CONNECT_COMPLETE, 
                         (connData->wlanReasonCode == ERROR_SUCCESS) ? 1 : 0,
                         (LPARAM)connData->wlanReasonCode);
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
            PostMessageW(hFlyout, WM_ASYNC_CONNECT_COMPLETE, 0, (LPARAM)ERROR_SUCCESS);
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
    PostMessageW(hFlyout, WM_REFRESH_DATA, 0, 0);
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
    int yPos = top + (ScaleDpi(26) - iconSize) / 2;
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

void UpdateTooltipForRow(HWND hwnd, int index) {
    if (!g_hTooltip) InitTooltip(hwnd);
    for (int i = 0; i < 50; i++) {
        TOOLINFOW ti = {0};
        ti.cbSize = sizeof(TOOLINFOW);
        ti.hwnd   = hwnd;
        ti.uId    = (UINT_PTR)(i + 1);
        SendMessage(g_hTooltip, TTM_DELTOOL, 0, (LPARAM)&ti);
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

static bool InitPniduiInfo() {
    if (g_pniduiBase) return true;
    HMODULE hPnidui = GetModuleHandleW(L"C:\\Program Files\\ExplorerPatcher\\pnidui.dll");
    if (!hPnidui) {
        hPnidui = GetModuleHandleW(L"pnidui.dll");
    }
    if (!hPnidui) {
        Wh_Log(L"pnidui.dll not loaded — network icon detection unavailable");
        return false;
    }
    MODULEINFO mi{};
    if (!GetModuleInformation(GetCurrentProcess(), hPnidui, &mi, sizeof(mi))) {
        Wh_Log(L"GetModuleInformation failed for pnidui.dll");
        return false;
    }
    g_pniduiBase = (BYTE*)mi.lpBaseOfDll;
    g_pniduiEnd  = g_pniduiBase + mi.SizeOfImage;
    Wh_Log(L"pnidui.dll found at %p-%p", g_pniduiBase, g_pniduiEnd);
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
    int btnH = ScaleDpi(16), btnW = ScaleDpi(22);
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
        if (isConnecting) {
            MoveWindow(g_hWndButtonConnect, btnX + 50, btnY, 40, 22, TRUE);
            SetWindowTextW(g_hWndButtonConnect, L"...");
            ShowWindow(g_hWndButtonConnect, SW_SHOW);
            EnableWindow(g_hWndButtonConnect, FALSE);
        } else if (isConnected) {
            MoveWindow(g_hWndButtonConnect, btnX, btnY, 92, 22, TRUE);
            SetWindowTextW(g_hWndButtonConnect, LOC(STR_BTN_DISCONNECT));
            ShowWindow(g_hWndButtonConnect, SW_SHOW);
            EnableWindow(g_hWndButtonConnect, TRUE);
        } else {
            MoveWindow(g_hWndButtonConnect, btnX, btnY, 92, 22, TRUE);
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
        RefreshNetworkData();
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
                    Wh_Log(L"Auth failure for '%s' (code 0x%08X) — saved password likely wrong, resetting profile", 
                           item->ssid, errorCode);
                    item->hasProfile = FALSE;
                    item->connState = CONN_STATE_ERROR;
                    item->operationStartTime = 0;
                    MessageBoxW(hwnd, LOC(STR_PWD_FAILED_WRONG), LOC(STR_PWD_FAILED_TITLE), 
                               MB_OK | MB_ICONERROR);
                } else if (isAuthFailure && !item->hasProfile) {
                    Wh_Log(L"Auth failure for '%s' (code 0x%08X) — user-entered password was wrong", 
                           item->ssid, errorCode);
                    item->connState = CONN_STATE_ERROR;
                    item->operationStartTime = 0;
                    MessageBoxW(hwnd, LOC(STR_PWD_FAILED_WRONG), LOC(STR_PWD_FAILED_TITLE), 
                               MB_OK | MB_ICONERROR);
                } else {
                    Wh_Log(L"Non-auth failure for '%s' (code 0x%08X) — keeping profile intact", 
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
        HDC     hdc     = CreateCompatibleDC(hdcReal);
        HBITMAP hBmp    = CreateCompatibleBitmap(hdcReal, WINDOW_WIDTH, WINDOW_HEIGHT);
        HBITMAP hOldBmp = (HBITMAP)SelectObject(hdc, hBmp);
        
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

        HPEN hPenSep = CreatePen(PS_SOLID, 1, (g_Settings.theme == 1) ? RGB(70,70,75) : RGB(214,223,234));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPenSep);
        MoveToEx(hdc, 0, HEADER_HEIGHT, NULL); LineTo(hdc, WINDOW_WIDTH, HEADER_HEIGHT);
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
            TextOutW(hdc, 8, 8, LOC(STR_CURRENT_CONNECTED), lstrlenW(LOC(STR_CURRENT_CONNECTED)));
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
            
            DrawTextWithWrap(hdc, displayName, 56, ScaleDpi(36), WINDOW_WIDTH - 70, 18);
            SelectObject(hdc, g_hFontNormal); SetTextColor(hdc, GetSecondaryTextColor());
            TextOutW(hdc, 56, ScaleDpi(54), LOC(STR_INTERNET_ACCESS), lstrlenW(LOC(STR_INTERNET_ACCESS)));
        } else {
            SelectObject(hdc, g_hFontNormal); SetTextColor(hdc, GetTextColor());
            TextOutW(hdc, 8, 8, LOC(STR_NO_CONNECTIONS), lstrlenW(LOC(STR_NO_CONNECTIONS)));
            SelectObject(hdc, g_hFontBold);
            SetTextColor(hdc, GetTextColor());
            TextOutW(hdc, 56, ScaleDpi(36), LOC(STR_CONNECTIONS_AVAILABLE), lstrlenW(LOC(STR_CONNECTIONS_AVAILABLE)));
        }
        
        int iconSize = ScaleDpi(35*1.05); 
        HICON hLargeIcon = isAnyConnected ? g_hIconNetworkMap : g_hIconSignalBars[0];
        if (hLargeIcon) DrawIconEx(hdc, 14, 37, hLargeIcon, iconSize, iconSize, 0, NULL, DI_NORMAL);

        if (showWifiList) {
            int totalHeight = GetTotalListHeight();
            int visibleHeight = LIST_Y_END - LIST_Y_START;
            BOOL hasScrollbar = (totalHeight > visibleHeight);
            int scrollbarOffset = hasScrollbar ? ScaleDpi(13) : 0;
            int roundedCornersOffset = g_Settings.useRoundedCorners ? (WINDOW_WIDTH * 2) / 100 : 0;
            int scrollbarShift = hasScrollbar ? 0 : (((WINDOW_WIDTH  *4) / 100) - ((WINDOW_WIDTH*  13) / 1000));
            g_rcRefreshButton.right = WINDOW_WIDTH - ScaleDpi(19) - scrollbarOffset - roundedCornersOffset + scrollbarShift;
            g_rcRefreshButton.left  = g_rcRefreshButton.right - ScaleDpi(21);
            if (g_rcRefreshButton.right > WINDOW_WIDTH) {
                int overflow = g_rcRefreshButton.right - WINDOW_WIDTH;
                g_rcRefreshButton.right -= overflow;
                g_rcRefreshButton.left  -= overflow;
            }

            if (g_IsHoveringRefresh) {
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
            
            if (!g_hIconRefreshNormal) g_hIconRefreshNormal = CreateIconFromBase64PNG(REFRESH_ICON_NORMAL_BASE64);
            if (g_hIconRefreshNormal) {
                if (g_Settings.theme == 0) {
                    DrawIconEx(hdc, g_rcRefreshButton.left+2, g_rcRefreshButton.top+3,
                               g_hIconRefreshNormal, 0, 0, 0, NULL, DI_NORMAL);
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
            SelectObject(hdc, g_hFontNormal); SetTextColor(hdc, GetSecondaryTextColor());
            TextOutW(hdc, 14, HEADER_HEIGHT - 24, LOC(STR_WIFI_HEADER), lstrlenW(LOC(STR_WIFI_HEADER)));
            
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
            SelectObject(hdc, g_hFontArrow); SetTextColor(hdc, (g_Settings.theme == 1) ? RGB(180, 180, 180) : RGB(50, 50, 50));
            LPCWSTR arrowChar = g_bListExpanded ? L"6" : L"5";
            RECT rcArrowText = g_rcArrowButton; rcArrowText.top += 2;
            DrawTextW(hdc, arrowChar, 1, &rcArrowText, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
            
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
                    DrawTextWithWrap(hdc, ssidBuf, rcRow.left - ScaleDpi(2), rcRow.top+6, 
                                     rcRow.right - rcRow.left - 10, 18);       
                    WifiNetworkItem* item = &g_NetworkList[i];
                    BOOL isTransitioning = (item->connState == CONN_STATE_CONNECTING ||
                                            item->connState == CONN_STATE_DISCONNECTING);
                    if (item->connState == CONN_STATE_CONNECTED) {
                        SelectObject(hdc, g_hFontBold);
                        SetTextColor(hdc, (g_Settings.theme == 1) ? GetTextColor() : RGB(0, 0, 0));
                        RECT rcStatus;
                        rcStatus.right  = rcRow.right - 39 - scrollbarOffset;
                        rcStatus.left   = rcRow.left + 80;
                        rcStatus.top    = rcRow.top + 6;
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
        SelectObject(hdc, hOldBmp); DeleteObject(hBmp); DeleteDC(hdc);
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
                        if (msg == WM_LBUTTONUP) {
                            static DWORD lastClickTime = 0;
                            DWORD currentTime = GetTickCount();
                            if (currentTime - lastClickTime > CLICK_DEBOUNCE_MS) {
                                lastClickTime = currentTime;
                                if (g_hWndFlyout && IsWindow(g_hWndFlyout) && IsWindowVisible(g_hWndFlyout)) {
                                    ShowWindow(g_hWndFlyout, SW_HIDE);
                                    ClearKeyboardFocus();
                                } else {
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
            
            RefreshNetworkData();
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
            Wh_Log(L"WLAN service unavailable — will retry lazily on first flyout open");
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
            Sleep(1000);
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
    if (g_hConnectMutex) { CloseHandle(g_hConnectMutex); g_hConnectMutex = NULL; }
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
static bool g_addConnect = true;
static bool g_addHomegroup = true;
static bool g_hookInstalled = false;
static bool g_iconHookInstalled = false;

using LoadImageW_t = decltype(&LoadImageW);
static LoadImageW_t LoadImageW_Orig = nullptr;

// ---------------------------------------------------------------------------
// Strings / XML
// ---------------------------------------------------------------------------
struct LangPack {
    WORD lang;
    const wchar_t *cTitle, *cDesc, *hTitle, *hDesc;
};

static const LangPack kLang[] = {
    {0x09, L"Connect to a Network",
     L"Connect to an available wireless, VPN, or dial-up network.",
     L"Choose Homegroup and Sharing Options",
     L"View or change your homegroup settings and network sharing preferences."},
    {0x10, L"Connessione a una rete",
     L"Connettere o riconnettere una rete wireless, VPN o di accesso remoto disponibile.",
     L"Selezione delle opzioni del gruppo home e della condivisione",
     L"Accedere alle impostazioni del gruppo home e configurare le opzioni di condivisione della rete."},
    {0x0c, L"Se connecter \u00e0 un r\u00e9seau",
     L"Connectez-vous aux r\u00e9seaux sans fil, VPN ou distants disponibles.",
     L"Choisir les options de groupe r\u00e9sidentiel et de partage",
     L"Affichez ou modifiez les param\u00e8tres de groupe r\u00e9sidentiel et de partage."},
    {0x0a, L"Conectar a una red",
     L"Con\u00e9ctese a redes inal\u00e1mbricas, VPN o de acceso telef\u00f3nico disponibles.",
     L"Elegir opciones de grupo en el hogar y uso compartido",
     L"Vea o cambie la configuraci\u00f3n del grupo en el hogar y uso compartido de red."},
    {0x19, L"\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u0435 \u043a \u0441\u0435\u0442\u0438",
     L"\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u0435 \u043a \u0434\u043e\u0441\u0442\u0443\u043f\u043d\u044b\u043c \u0431\u0435\u0441\u043f\u0440\u043e\u0432\u043e\u0434\u043d\u044b\u043c \u0441\u0435\u0442\u044f\u043c, VPN \u0438\u043b\u0438 \u0441\u0435\u0442\u044f\u043c \u0443\u0434\u0430\u043b\u0451\u043d\u043d\u043e\u0433\u043e \u0434\u043e\u0441\u0442\u0443\u043f\u0430.",
     L"\u0412\u044b\u0431\u043e\u0440 \u043f\u0430\u0440\u0430\u043c\u0435\u0442\u0440\u043e\u0432 \u0434\u043e\u043c\u0430\u0448\u043d\u0435\u0439 \u0433\u0440\u0443\u043f\u043f\u044b \u0438 \u043e\u0431\u0449\u0435\u0433\u043e \u0434\u043e\u0441\u0442\u0443\u043f\u0430",
     L"\u041f\u0440\u043e\u0441\u043c\u043e\u0442\u0440 \u0438\u043b\u0438 \u0438\u0437\u043c\u0435\u043d\u0435\u043d\u0438\u0435 \u043f\u0430\u0440\u0430\u043c\u0435\u0442\u0440\u043e\u0432 \u0434\u043e\u043c\u0430\u0448\u043d\u0435\u0439 \u0433\u0440\u0443\u043f\u043f\u044b \u0438 \u043e\u0431\u0449\u0435\u0433\u043e \u0434\u043e\u0441\u0442\u0443\u043f\u0430 \u043a \u0441\u0435\u0442\u0438."},
    {0x07, L"Mit einem Netzwerk verbinden",
     L"Verbindung mit verf\u00fcgbaren Drahtlos-, VPN- oder DF\u00dc-Netzwerken herstellen.",
     L"Heimnetzgruppen- und Freigabeoptionen ausw\u00e4hlen",
     L"Einstellungen f\u00fcr Heimnetzgruppen und Netzwerkfreigaben anzeigen oder \u00e4ndern."},
    {0x16, L"Ligar a uma rede",
     L"Ligue-se a redes sem fios, VPN ou de acesso telef\u00f3nico dispon\u00edveis.",
     L"Escolher op\u00e7\u00f5es de Grupo Dom\u00e9stico e partilha",
     L"Veja ou altere as defini\u00e7\u00f5es do Grupo Dom\u00e9stico e da partilha de rede."},
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

static std::wstring Patch(const std::wstring& in) {
    if (!g_addConnect && !g_addHomegroup)
        return in;
    std::wstring xml = in;
    size_t createMark = xml.find(L"atom(createnewbtn)");
    size_t diagMark = xml.find(L"atom(diagnosebtn)");
    if (createMark == std::wstring::npos || diagMark == std::wstring::npos)
        return in;

    size_t c0 = 0, c1 = 0, d0 = 0, d1 = 0;
    if (!FindOuterElement(xml, createMark, c0, c1) || !FindOuterElement(xml, diagMark, d0, d1))
        return in;
    if (c1 <= c0 || d1 <= d0 || !(c1 <= d0 || d1 <= c0))
        return in;

    std::wstring createBlock = xml.substr(c0, c1 - c0);
    std::wstring diagBlock = xml.substr(d0, d1 - d0);

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

static bool IsNetCenter(HMODULE h) {
    if (!h)
        return false;
    wchar_t path[MAX_PATH];
    if (!GetModuleFileNameW(h, path, MAX_PATH))
        return false;
    return _wcsicmp(PathFindFileNameW(path), L"netcenter.dll") == 0;
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
    return FAILED(hr) ? SetXMLFromResource_Orig(t, n, tp, m, p4, p5) : hr;
}

static bool HookAll() {
    if (g_hookInstalled)
        return true;

    HMODULE dui = LoadLibraryExW(L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!dui)
        return false;

    // SetXML is public; resolving it directly avoids a no-op hook on a hot
    // DirectUI path merely to obtain a trampoline.
    for (auto n : {"?SetXML@DUIXmlParser@DirectUI@@QEAAJPEBGPEAUHINSTANCE__@@1@Z",
                   "?SetXML@DUIXmlParser@DirectUI@@QAAJPBGPAUHINSTANCE__@@1@Z"}) {
        if (FARPROC p = GetProcAddress(dui, n)) {
            SetXML = reinterpret_cast<SetXML_t>(p);
            break;
        }
    }
    if (!SetXML)
        return false;

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
    g_hookInstalled = SetXMLFromResource_Orig != nullptr;
    if (g_hookInstalled && !g_iconHookInstalled) {
        g_iconHookInstalled = WindhawkUtils::SetFunctionHook(
            LoadImageW, LoadImageW_Hook, &LoadImageW_Orig);
        if (!g_iconHookInstalled)
            Wh_Log(L"Network Center links: custom icon hook unavailable; using Windows icons");
    }
    return g_hookInstalled;
}

static bool Init() {
    bool enabled = Wh_GetIntSetting(L"restoreClassicNetworkCenterLinks") != 0;
    g_addConnect = enabled;
    g_addHomegroup = enabled;

    // Always install the DirectUI + LoadImageW hooks here, unconditionally,
    // regardless of whether the feature is currently enabled. Per the
    // Windhawk API, all hooks should be set in Wh_ModInit, since
    // Wh_ApplyHookOperations() is called automatically right after it;
    // hooks registered later (e.g. from Wh_ModSettingsChanged) stay pending
    // and never actually activate. g_addConnect/g_addHomegroup already make
    // Patch() a no-op when the feature is off, so installing the hooks
    // unconditionally has no behavioral effect while it's disabled, and lets
    // it turn on and off correctly at runtime without a mod reload.
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
    // The hooks were already installed unconditionally in Init(); toggling
    // the setting only needs to flip these flags, which Patch() checks.
}

#undef NCL_THISCALL

}  // namespace Win7NetworkCenterLinks


BOOL Wh_ModInit() {
    Wh_Log(L"=== Wh_ModInit v3.2.0 ===");
    DetectWindowsVersion();
    LoadSettings();
    DetermineLocale();

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
    ZeroMemory(&g_Ctx, sizeof(g_Ctx));
    InitializeCriticalSection(&g_Ctx.csLock);
    g_hConnectMutex = CreateMutexW(NULL, FALSE, L"Local\\Win7NetFlyout_ConnectMutex");
    g_uTaskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
    LoadSystemIcons();
    InitGdiPlusRendering();
    InitGlobalFonts();
    InitRefreshButtonRect();
    RecalcArrowRect();
    InstallTrayInterceptionInternal();
    g_Ctx.hHotkeyThread = CreateThread(NULL, 0, HotkeyThreadProc, &g_Ctx, 0, &g_Ctx.dwHotkeyThreadId);
    if (!g_Ctx.hHotkeyThread) {
        if (g_hConnectMutex) { CloseHandle(g_hConnectMutex); g_hConnectMutex = NULL; }
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

    DarkContextMenu::OnSettingsChanged();

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
        PostMessageW(g_hWndFlyout, WM_REFRESH_DATA, 0, 0);
        InvalidateRect(g_hWndFlyout, NULL, TRUE);
    }
}

void Wh_ModUninit() {
    if (!g_IsExplorerHost) {
        // control.exe can create only the in-memory Network Center icons.
        FreeSystemIcons();
        return;
    }

    SafeCleanup();
    DeleteCriticalSection(&g_Ctx.csLock);
    DarkContextMenu::Uninit();
    UnregisterClassW(L"Win7NetworkFlyoutSafe", GetModuleHandle(NULL));
    UnregisterClassW(L"Win7NetPwdClass", GetModuleHandle(NULL));
}
