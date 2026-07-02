// ==WindhawkMod==
// @id             win7-network-flyout-recreation
// @name           Windows 7 Network Flyout Recreation
// @description    This mod recreates the Windows 7 network flyout for Windows 10 and 11
// @version        2.8.7
// @author         babamohammed
// @github         https://github.com/babamohammed2022
// @include        explorer.exe
// @architecture   x86-64
// @compilerOptions -lgdi32 -ldwmapi -luxtheme -lole32 -lshell32 -luser32 -lcomctl32 -liphlpapi -lwlanapi -luuid -lpsapi
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Windows 7 Network Flyout Recreation

This mod recreates the classic Windows 7 network flyout on Windows 10 and 11, replacing the modern flyout with a recreation of the familiar, lightweight alternative from Windows 7.

Screenshot of the light theme:

![Screenshot](https://raw.githubusercontent.com/babamohammed2022/gtasashtml/main/light.png)


Screenshot of the dark theme:

![Screenshot](https://raw.githubusercontent.com/babamohammed2022/gtasashtml/main/dark.png)


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
- **Dual Theme Support (NEW)**: Includes both light and dark themes, with the dark theme created specifically for late-night use and, if present, dark Aero theme.
## Requirements

- **Windows 10** with the native taskbar
- **Windows 11** with the Windows 10 taskbar (via [ExplorerPatcher](https://github.com/valinet/ExplorerPatcher))
- The network icon must be visible in the main system tray (overflow menu not supported)

> **Note:** This mod is unlikely to work with other taskbar mods (e.g. Retrobar) or heavily customized configurations.

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
## Changelog (version 2.8.7)
- Added custom dark theme for the entire flyout UI
- Added support for German and Portuguese languages
- Implemented dark theme for password dialog and context menus
- Improved the UI layout with dynamic refresh button positioning
- Enhanced visual consistency with Windows 7 original design

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
- useRoundedCorners: false
  $name: Rounded corners
  $description: Give the flyout window rounded corners. Looks better on Windows 11 or with the Aero theme enabled. Disabled by default for classic theme compatibility.
- theme: light
  $name: Theme
  $description: Select the network flyout's theme
  $options:
    - light: Light (Classic Windows 7)
    - dark: Dark (Custom)
*/
// ==/WindhawkModSettings==
#ifndef UNICODE
#define UNICODE
#endif
#include <windows.h>
#include <windowsx.h>
#include <wlanapi.h>
#include <objbase.h>
#include <uxtheme.h>
#include <dwmapi.h>
#include <strsafe.h>
#include <shellapi.h>
#include <commctrl.h>
#include <math.h>
#include <windhawk_api.h>
#include <iphlpapi.h>
#include <netlistmgr.h>
#include <windhawk_utils.h>
#include <process.h>
#include <psapi.h>

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


void InitGlobalFonts();
void FreeGlobalFonts();
void InitRefreshButtonRect(void);
void RecalcArrowRect();
void ApplyNativeControlsTheme();

void RecalcDpiMetrics(UINT dpi) {
    g_dpi = dpi ? dpi : 96;

    WINDOW_WIDTH        = ScaleDpi(WINDOW_WIDTH_BASE);
    WINDOW_HEIGHT       = ScaleDpi(WINDOW_HEIGHT_BASE);
    HEADER_HEIGHT       = ScaleDpi(HEADER_HEIGHT_BASE);
    FOOTER_HEIGHT       = ScaleDpi(FOOTER_HEIGHT_BASE);
    LIST_Y_START        = HEADER_HEIGHT + 1;
    LIST_Y_END          = WINDOW_HEIGHT - FOOTER_HEIGHT;
    WIFI_LABEL_Y         = HEADER_HEIGHT - ScaleDpi(24);
    ROW_HEIGHT_NORMAL    = ScaleDpi(ROW_HEIGHT_NORMAL_BASE);
    ROW_HEIGHT_EXPANDED  = ScaleDpi(ROW_HEIGHT_EXPANDED_BASE);

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

static HICON g_hIconRefreshNormal = NULL;
static INetworkListManager* g_pNLM = NULL;
// Custom network icon: left in code but disabled (stability issues).

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

// -------------------------------------------------------
// Settings
// -------------------------------------------------------
struct ModSettings {
    BOOL interceptNativeFlyout;
    BOOL privacyMode;
    int  refreshInterval;
    int  language;
    BOOL enableHotkey;
    BOOL useRoundedCorners;
    int  theme;  // 0=light, 1=dark
} g_Settings = { TRUE, FALSE, 3000, 0, FALSE, FALSE, 0 };

void LoadSettings() {
    int raw_intercept  = Wh_GetIntSetting(L"interceptNativeFlyout");
    int raw_privacy    = Wh_GetIntSetting(L"privacyMode");
    int raw_refresh    = Wh_GetIntSetting(L"refreshInterval");
    LPCWSTR lang = Wh_GetStringSetting(L"language");
    int raw_language = 0;
    if (lang) {
        if (_wcsicmp(lang, L"en") == 0)      raw_language = 1;
        else if (_wcsicmp(lang, L"it") == 0) raw_language = 2;
        else if (_wcsicmp(lang, L"es") == 0) raw_language = 3;
        else if (_wcsicmp(lang, L"fr") == 0) raw_language = 4;
        else if (_wcsicmp(lang, L"ru") == 0) raw_language = 5;
        else if (_wcsicmp(lang, L"de") == 0) raw_language = 6;
        else if (_wcsicmp(lang, L"pt") == 0) raw_language = 7;
        Wh_FreeStringSetting(lang);
    }
    int raw_enableHotkey = Wh_GetIntSetting(L"enableHotkey");
    int raw_roundedCorners = Wh_GetIntSetting(L"useRoundedCorners");

    LPCWSTR theme = Wh_GetStringSetting(L"theme");
    int raw_theme = 0;
    if (theme) {
        if (_wcsicmp(theme, L"dark") == 0) raw_theme = 1;
        else raw_theme = 0;
        Wh_FreeStringSetting(theme);
    }

    g_Settings.interceptNativeFlyout      = raw_intercept   != 0;
    g_Settings.privacyMode               = raw_privacy     != 0;
    g_Settings.refreshInterval            = raw_refresh;
    g_Settings.language                  = raw_language;
    g_Settings.enableHotkey              = raw_enableHotkey != 0;
    g_Settings.useRoundedCorners         = raw_roundedCorners != 0;
    g_Settings.theme                     = raw_theme;

    if (g_Settings.refreshInterval > 0 && g_Settings.refreshInterval < 1000) {
        g_Settings.refreshInterval = 1000;
    }
}
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

// Apply (or restore) dark menu theme.
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
    return (g_Settings.theme == 1) ? RGB(30, 30, 30) : RGB(235, 244, 253);
}
COLORREF GetContentBgColor() {
    return (g_Settings.theme == 1) ? RGB(20, 20, 20) : RGB(255, 255, 255);
}
COLORREF GetFooterBgColor() {
    return (g_Settings.theme == 1) ? RGB(30, 30, 30) : RGB(235, 244, 253);
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
    DWORD operationStartTime;  // For timeout detection
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

HWND g_hWndFlyout          = NULL;
HWND g_hWndButtonConnect   = NULL;
HWND g_hWndCheckboxConnect = NULL;
BOOL g_bListExpanded        = TRUE;

// (WM_SETFONT) sia dal rendering owner-draw in tema scuro (WM_DRAWITEM).

// serve gia' ad ApplyNativeControlsTheme, definita subito sotto.
HFONT g_hFontButton    = NULL;


// con cui g_hWndButtonConnect e' attualmente creato (owner-draw o nativo).
// ricreare il bottone con il tipo giusto.
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
        // Se siamo in tema scuro, usa il tema DarkMode_Explorer
        // che gestisce automaticamente hover, pressed, disabled
        if (g_Settings.theme == 1) {
            SetWindowTheme(g_hWndButtonConnect, L"DarkMode_Explorer", NULL);
            // Rende il pulsante nativo ma con tema scuro
            // Windows gestirà automaticamente hover, click, ecc.
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
int  g_NetworkCount           = 0;
BOOL g_IsHoveringLink         = FALSE;
BOOL g_IsHoveringRefresh      = FALSE;
BOOL g_IsHoveringArrow        = FALSE;
int g_ScrollPos = 0;
int  g_SelectedRowIndex       = -1;
int  g_HoveredRowIndex        = -1;
int  g_KeyboardSelectedIndex  = -1;
int  g_ContextMenuTargetIndex = -1;

RECT g_rcRefreshButton = { 0 };
RECT g_rcArrowButton = { 0 };
RECT g_rcCheckboxLabel = { 0 };
BOOL g_bShowCheckboxLabel = FALSE;

HICON g_hIconNetworkMap  = NULL;
HICON g_hIconSignalBars[6] = { NULL };
HICON g_hIconRefreshWin7 = NULL;

int   g_PendingConnectIndex = -1;
HWND  g_hTooltip = NULL;

UINT_PTR g_RefreshTimer = 0;
UINT_PTR g_TimeoutTimer = 0;  HWND G_hSubclassedToolbar = nullptr;



static BYTE* g_pniduiBase = NULL;
static BYTE* g_pniduiEnd  = NULL;

static HANDLE g_hConnectMutex = NULL;
static HMODULE g_hGdiPlus = NULL;
static ULONG_PTR g_gdiplusToken = 0;

static void* g_pBitmapSignalBars[6] = { NULL };

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
void RecalcDpiMetrics(UINT dpi);
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
static HICON CreateIconFromBase64PNG(const WCHAR* base64Str) { // Base64 decode function to create HICON
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
        
        if (pStartup && pFromStream && pToHICON && pDispose && pShutdown) {
            ULONG_PTR token = 0;
            struct { DWORD Version; void* Callback; BOOL Suppress; } input = {1, NULL, FALSE};
            
            if (pStartup(&token, &input, NULL) == 0) {
                void* bitmap = NULL;
                if (pFromStream(stream, &bitmap) == 0) {
                    pToHICON(bitmap, &hIcon);
                    pDispose(bitmap);
                }
                pShutdown(token);
            }
        }
        FreeLibrary(hGdi);
    }
    stream->Release();
    return hIcon;
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
    // Dimensioni base del pulsante (in pixel a 96 DPI)
    const int buttonSize = ScaleDpi(16);
    const int margin = ScaleDpi(8);
    
    // Calcola se c'è una barra di scorrimento
    int totalListHeight = GetTotalListHeight();
    int availableHeight = LIST_Y_END - LIST_Y_START;
    BOOL hasScrollbar = (totalListHeight > availableHeight);
    
    // Posizione X di base (dal bordo destro)
    int baseX = WINDOW_WIDTH - margin - buttonSize;
    
    // Se NON c'è la barra di scorrimento, sposta il pulsante del 4% più a destra
    // e poi dell'1.3% più a sinistra (netto: +2.7% verso destra)
    if (!hasScrollbar) {
        baseX += (WINDOW_WIDTH * 4) / 100;     // +4% della larghezza finestra verso destra
        baseX -= (WINDOW_WIDTH * 13) / 1000;   // -1.3% della larghezza finestra verso sinistra
    }
    
    // Limite di sicurezza per evitare che esca completamente dallo schermo a destra
    if (baseX + buttonSize > WINDOW_WIDTH) {
        baseX = WINDOW_WIDTH - buttonSize;
    }
    
    // Ripristinate le altezze originali esatte (altezza da terra precedente)
    g_rcRefreshButton.left   = baseX;
    g_rcRefreshButton.top    = ScaleDpi(2);  // <--- Altezza originale di prima
    g_rcRefreshButton.right  = baseX + buttonSize;
    g_rcRefreshButton.bottom = ScaleDpi(24); // <--- Altezza originale di prima
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

// Initialize GDI\+.
static BOOL InitGdiPlusRendering() {
    if (g_hGdiPlus) return TRUE;
    
    g_hGdiPlus = LoadLibraryW(L"gdiplus.dll");
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

// Shutdown GDI+ and free cached bitmaps
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
    // Free cached GDI+ bitmaps
    if (g_hGdiPlus && pGdipDisposeImage) {
        for (int i = 0; i < 6; i++) {
            if (g_pBitmapSignalBars[i]) {
                pGdipDisposeImage(g_pBitmapSignalBars[i]);
                g_pBitmapSignalBars[i] = NULL;
            }
        }
    }

    if (g_hIconRefreshNormal) { DestroyIcon(g_hIconRefreshNormal); g_hIconRefreshNormal = NULL; }
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
// WLAN data refresh (now populates authAlgorithm & cipherAlgorithm)
// -------------------------------------------------------
void RefreshWifiData(HANDLE hClient) {
    if (!hClient) return;
    static DWORD lastValidRefresh = 0;
    DWORD now = GetTickCount();

    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
    if (WlanEnumInterfaces(hClient, NULL, &pIfList) != ERROR_SUCCESS) return;

    WifiNetworkItem tempList[50];
    int tempCount = 0;
    ZeroMemory(tempList, sizeof(tempList));

    for (DWORD i = 0; i < pIfList->dwNumberOfItems; i++) {
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

                // Move connected to top.
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
                    // Preserva lo stato di connessione per le operazioni in corso
                    if (g_NetworkList[e].connState == CONN_STATE_CONNECTING ||
                        g_NetworkList[e].connState == CONN_STATE_DISCONNECTING ||
                        g_NetworkList[e].connState == CONN_STATE_ERROR) {
                        tempList[t].connState = g_NetworkList[e].connState;
                        tempList[t].operationStartTime = g_NetworkList[e].operationStartTime;
                    }
                    // IMPORTANTE: preserva hasProfile dalla lista esistente

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

        // MODIFICA: Bordo aggiornato per l'altezza di 20 pixel (50 + 20 + 1 = 71)
        if (g_Settings.theme == 1) {
            HPEN hPen = CreatePen(PS_SOLID, 1, RGB(75, 75, 85)); 
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
            HBRUSH hOldBr = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
            
            // Racchiude l'Edit control posizionato a (145, 50) con la nuova altezza (245, 20)
            Rectangle(hdc, 144, 49, 145 + 245 + 1, 50 + 20 + 1);
            
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
            WS_CHILD|WS_VISIBLE, 15, 15, 380, 20, hwnd, (HMENU)200, cs->hInstance, NULL);
        SendMessageW(hInstr, WM_SETFONT, (WPARAM)hFontDlg, TRUE);
        
        HWND hLabel = CreateWindowExW(0, WC_STATICW, LOC(STR_PWD_LABEL),
            WS_CHILD|WS_VISIBLE, 15, 53, 125, 18, hwnd, NULL, cs->hInstance, NULL);
        SendMessageW(hLabel, WM_SETFONT, (WPARAM)hFontDlg, TRUE);
        
        BOOL bDarkPwd = (g_Settings.theme == 1);

        DWORD dwEditExStyle = bDarkPwd ? 0 : WS_EX_CLIENTEDGE;
        
        // MODIFICA: Altezza ridotta da 22 a 20 pixel
        HWND hEdit = CreateWindowExW(dwEditExStyle, WC_EDITW, L"",
            WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL,
            145, 50, 245, 20, hwnd, (HMENU)101, cs->hInstance, NULL);
        SendMessageW(hEdit, WM_SETFONT, (WPARAM)hFontDlg, TRUE);
        SendMessageW(hEdit, EM_SETPASSWORDCHAR, 0x25CF, 0);
        if (bDarkPwd) SetWindowTheme(hEdit, L"DarkMode_Explorer", NULL);
        SetFocus(hEdit);
        
        HWND hCheck = CreateWindowExW(0, WC_BUTTONW, L"",
            WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX, 135, 80, 18, 18, hwnd, (HMENU)102, cs->hInstance, NULL);
        SendMessageW(hCheck, BM_SETCHECK, BST_CHECKED, 0);
        if (bDarkPwd) SetWindowTheme(hCheck, L"DarkMode_Explorer", NULL);
        
        HWND hCheckText = CreateWindowExW(0, WC_STATICW, LOC(STR_PWD_HIDE_CHARS),
            WS_CHILD|WS_VISIBLE|SS_NOTIFY, 156, 80, 200, 18, hwnd, (HMENU)103, cs->hInstance, NULL);
        SendMessageW(hCheckText, WM_SETFONT, (WPARAM)hFontDlg, TRUE);
        
        if (bDarkPwd) {
            InvalidateRect(hCheck, NULL, TRUE);
            InvalidateRect(hCheckText, NULL, TRUE);
            UpdateWindow(hwnd);
        }
        
        RECT rcClient; GetClientRect(hwnd, &rcClient);
        int btnW = 85, btnH = 24, btnY = rcClient.bottom - 35;
        HWND hBtnOk = CreateWindowExW(0, WC_BUTTONW, LOC(STR_PWD_OK),
            WS_CHILD|WS_VISIBLE|(bDarkPwd ? BS_OWNERDRAW : BS_DEFPUSHBUTTON),
            rcClient.right - btnW - 15, btnY, btnW, btnH, hwnd, (HMENU)IDOK, cs->hInstance, NULL);
        SendMessageW(hBtnOk, WM_SETFONT, (WPARAM)hFontDlg, TRUE);
        if (bDarkPwd) SetWindowTheme(hBtnOk, L"DarkMode_Explorer", NULL);
        HWND hBtnCancel = CreateWindowExW(0, WC_BUTTONW, LOC(STR_PWD_CANCEL),
            WS_CHILD|WS_VISIBLE|(bDarkPwd ? BS_OWNERDRAW : 0),
            rcClient.right - (btnW * 2) - 25, btnY, btnW, btnH,
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
        if (hwndCtrl == g_hWndCheckboxConnect && g_SelectedRowIndex >= 0) {
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
        if (hwndBtn == g_hWndCheckboxConnect && g_SelectedRowIndex >= 0) {
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
    int dlgW=420, dlgH=180;
    
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
    // NON applicare DarkMode_Explorer all'intero dialogo
    // SetWindowTheme(hDlg, L"DarkMode_Explorer", NULL);  <-- RIMOSSO
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
                case L'"': StringCchCatW(dst, dstSize, L"&quot;"); d += 6; break;
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

    // Detect enterprise networks.
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
    // Se conosciamo il BSSID esatto (risolto in RefreshWifiData), lo passiamo



    // facendo apparire la riconnessione come "non funzionante".
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
    WaitForSingleObject(g_hConnectThread, 5000);
    CloseHandle(g_hConnectThread);
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
    

    // if (item->connState == CONN_STATE_ERROR) {
    //     item->connState = CONN_STATE_IDLE;
    // }
    

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

        // (DISCONNECTION_TIMEOUT_MS) è molto più corto di quello di

        // per non aggiungere un ritardo extra percepibile sopra ai 4s.
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
                if (g_Ctx.hWlanClient) RefreshWifiData(g_Ctx.hWlanClient);
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
    
    // Post to flyout thread.
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

    // Post clean disconnect to flyout thread.
    PostMessageW(hFlyout, WM_ASYNC_CONNECT_COMPLETE, 0, (LPARAM)ERROR_SUCCESS);

    if (g_TimeoutTimer && hFlyout) {
        // Wake timeout loop immediately.
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
// -------------------------------------------------------
// Signal icon drawing
// -------------------------------------------------------

// Draw icon with bicubic interpolation.
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
                    HMODULE hMsImg32 = LoadLibraryW(L"msimg32.dll");
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
// -------------------------------------------------------
// Tooltip fade
// -------------------------------------------------------
#define TOOLTIP_FADE_TIMER_ID  9100
#define TOOLTIP_FADE_STEP      40    // alpha increment per tick
#define TOOLTIP_FADE_INTERVAL  20    // ms between ticks (~12 steps = ~240ms)

static BYTE  g_ttAlpha     = 255;
static BOOL  g_ttFading    = FALSE;

// Tooltip subclass: intercept WM_SHOWWINDOW to start fade-in.
static LRESULT CALLBACK TooltipSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR) {
    if (uMsg == WM_SHOWWINDOW && wParam) {
        // Start fade-in: set alpha to 0 and kick off a timer.
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
            // Fully opaque: stop timer and remove layered flag.
            KillTimer(hWnd, TOOLTIP_FADE_TIMER_ID);
            g_ttAlpha  = 255;
            g_ttFading = FALSE;
            SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
        }
        return 0;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// -------------------------------------------------------
// Tooltip
// -------------------------------------------------------
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
    if (index < 0 || index >= g_NetworkCount || !g_bListExpanded) return FALSE;
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
    
    int totalHeight = GetTotalListHeight(); // Scrollbar offset
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
    
    if (g_SelectedRowIndex < 0 || g_SelectedRowIndex >= g_NetworkCount) {
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
    int rowHeight = ROW_HEIGHT_EXPANDED;  // La riga selezionata è sempre espansa
    
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
    
    int btnX = WINDOW_WIDTH - 114 - scrollbarOffset;  // X position
    int chkX = 18;
    int chkYOffset = 0;
    // Se la barra di scorrimento è attiva, sposta il checkbox "Connetti automaticamente"
    // dell'1.9% + 0.5% più a sinistra e dell'1.3% più in alto (altrimenti resta come sta)
    if (scrollbarOffset > 0) {
        chkX -= (WINDOW_WIDTH * 19) / 1000;   // -1.9% della larghezza finestra verso sinistra
        chkX -= (WINDOW_WIDTH * 5) / 1000;    // -0.5% della larghezza finestra verso sinistra
        chkYOffset -= (WINDOW_HEIGHT * 13) / 1000;  // -1.3% dell'altezza finestra verso l'alto
    }
    int btnY = rowYRelative + 35;  
    int chkY = rowYRelative + 36 + chkYOffset;
    
    if (btnY < LIST_Y_START) btnY = LIST_Y_START + 2;
    if (btnY > LIST_Y_END - 24) btnY = LIST_Y_END - 24;
    if (chkY < LIST_Y_START) chkY = LIST_Y_START + 2;
    if (chkY > LIST_Y_END - 22) chkY = LIST_Y_END - 22;
    
    // Checkbox
    if (g_hWndCheckboxConnect && IsWindow(g_hWndCheckboxConnect)) {
        if (!isConnected && !isConnecting) {

            // largo 160px (come prima) il suo WM_CTLCOLORSTATIC riempie l'intera

            int boxSize = ScaleDpi(13);
            int chkNativeW = boxSize + ScaleDpi(4);
            MoveWindow(g_hWndCheckboxConnect, chkX, chkY, chkNativeW, 20, TRUE);
            ShowWindow(g_hWndCheckboxConnect, SW_SHOW);

            // nativo, cosi' il colore del testo non dipende dal rendering a tema

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
    
    // Pulsante Connect/Disconnect
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
    if (index < 0 || index >= g_NetworkCount) return;

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

            // verifica se l'ultima riga è scoperta e in tal caso scrolla quel poco che serve
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
        if (g_Ctx.hWlanClient) {
            RefreshWifiData(g_Ctx.hWlanClient);
            ClampScrollPos();
            UpdateLayoutGeometry();
            InvalidateRect(hwnd, NULL, FALSE);
        }
        } else if (wParam == 1002) {
            CheckConnectionTimeouts();
            UpdateLayoutGeometry();
            InvalidateRect(hwnd, NULL, FALSE);
        }
        break;
    case WM_SHOW_FLYOUT:
    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
    if (g_Ctx.hWlanClient) {
        RefreshWifiData(g_Ctx.hWlanClient);
        UpdateLayoutGeometry();
        InvalidateRect(hwnd, NULL, TRUE);
    }
    break;
    case WM_REFRESH_DATA: {
    if (g_Ctx.hWlanClient) {
        RefreshWifiData(g_Ctx.hWlanClient);
        ClampScrollPos();
        UpdateLayoutGeometry();
        InvalidateRect(hwnd, NULL, TRUE);
    }

        break;
    }
        case WM_ASYNC_CONNECT_COMPLETE: {
    BOOL opSuccess = (BOOL)wParam;
    DWORD errorCode = (DWORD)lParam;
    
    Wh_Log(L"Async connect/disconnect complete: success=%d, error=%lu (0x%08X)", 
           opSuccess, errorCode, errorCode);

    // A disconnect notification arrives as opSuccess=0, errorCode=ERROR_SUCCESS.
    // Treat this as a confirmed clean disconnect rather than a connection failure.
    if (!opSuccess && errorCode == ERROR_SUCCESS) {
        // Disconnection confirmed by WLAN notification
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
        RefreshWifiData(g_Ctx.hWlanClient);
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
                0x00038001,  // WLAN_REASON_CODE_INVALID_PROFILE
                0x00038002,  // MSM_REASON_CODE_INVALID_PROFILE_SCHEMA
                0x00028001,  // MSM_REASON_CODE_AUTH_FAILURE
                0x00028002,  // MSM_REASON_CODE_ASSOC_FAILURE (spesso auth-related)
                0x00030001,  // MSM_REASON_CODE_KEY_MISMATCH
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
    
    RefreshWifiData(g_Ctx.hWlanClient);
    UpdateLayoutGeometry();
    InvalidateRect(hwnd, NULL, TRUE);
    break;
}
    case WM_GETDLGCODE:
        return DLGC_WANTARROWS | DLGC_WANTCHARS;
    case WM_KEYDOWN: {
        switch (wParam) {
            case VK_UP:
                if (g_bListExpanded && g_NetworkCount > 0) {
                    int newIndex = (g_KeyboardSelectedIndex > 0) ? g_KeyboardSelectedIndex - 1 : g_NetworkCount - 1;
                    SetKeyboardFocus(newIndex);
                    InvalidateRect(hwnd, NULL, TRUE);
                }
                return 0;
            case VK_DOWN:
                if (g_bListExpanded && g_NetworkCount > 0) {
                    int newIndex = (g_KeyboardSelectedIndex < g_NetworkCount - 1) ? g_KeyboardSelectedIndex + 1 : 0;
                    SetKeyboardFocus(newIndex);
                    InvalidateRect(hwnd, NULL, TRUE);
                }
                return 0;
            case VK_RETURN:
                if (g_KeyboardSelectedIndex >= 0 && g_KeyboardSelectedIndex < g_NetworkCount)
                    ConnectToNetwork(g_KeyboardSelectedIndex);
                return 0;
            case VK_LEFT:
                ShowWindow(hwnd, SW_HIDE);
                return 0;
            case VK_RIGHT:
                if (g_KeyboardSelectedIndex >= 0 && g_KeyboardSelectedIndex < g_NetworkCount) {
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
int totalHeight = GetTotalListHeight();
int visibleHeight = LIST_Y_END - LIST_Y_START;

SCROLLINFO si = { sizeof(SCROLLINFO), SIF_RANGE | SIF_PAGE | SIF_POS, 0, totalHeight, (UINT)visibleHeight, g_ScrollPos };
SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
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

        BOOL isAnyConnected = (g_NetworkCount > 0 && g_NetworkList[0].connState == CONN_STATE_CONNECTED);
        SetBkMode(hdc, TRANSPARENT);
       if (isAnyConnected) {
    SelectObject(hdc, g_hFontNormal); SetTextColor(hdc, GetTextColor());
    TextOutW(hdc, 8, 8, LOC(STR_CURRENT_CONNECTED), lstrlenW(LOC(STR_CURRENT_CONNECTED)));
    SelectObject(hdc, g_hFontBold);
    SetTextColor(hdc, GetTextColor());
    WCHAR displaySsid[33]; GetDisplaySSID(0, displaySsid, 33);
    DrawTextWithWrap(hdc, displaySsid, 56, ScaleDpi(36), WINDOW_WIDTH - 70, 18);
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
{
    int totalHeight = GetTotalListHeight();
    int visibleHeight = LIST_Y_END - LIST_Y_START;
    BOOL hasScrollbar = (totalHeight > visibleHeight);
    int scrollbarOffset = hasScrollbar ? ScaleDpi(13) : 0;
    int roundedCornersOffset = g_Settings.useRoundedCorners ? (WINDOW_WIDTH * 2) / 100 : 0;

    // Se la barra di scorrimento NON è attiva, sposta il pulsante del 4% più a destra
    // e poi dell'1.3% più a sinistra (netto: +2.7% verso destra)
    int scrollbarShift = hasScrollbar ? 0 : (((WINDOW_WIDTH * 4) / 100) - ((WINDOW_WIDTH * 13) / 1000));

    g_rcRefreshButton.right = WINDOW_WIDTH - ScaleDpi(19) - scrollbarOffset - roundedCornersOffset + scrollbarShift;
    g_rcRefreshButton.left  = g_rcRefreshButton.right - ScaleDpi(21);

    // Limite di sicurezza per evitare che esca completamente dallo schermo a destra
    if (g_rcRefreshButton.right > WINDOW_WIDTH) {
        int overflow = g_rcRefreshButton.right - WINDOW_WIDTH;
        g_rcRefreshButton.right -= overflow;
        g_rcRefreshButton.left  -= overflow;
    }
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
        
        // Disegna l'icona refresh dal base64, ricolorata per il tema scuro
        if (!g_hIconRefreshNormal) g_hIconRefreshNormal = CreateIconFromBase64PNG(REFRESH_ICON_NORMAL_BASE64);
        if (g_hIconRefreshNormal) {
            if (g_Settings.theme == 0) {
                // Tema chiaro: disegna direttamente
                DrawIconEx(hdc, g_rcRefreshButton.left+2, g_rcRefreshButton.top+3,
                           g_hIconRefreshNormal, 0, 0, 0, NULL, DI_NORMAL);
            } else {
                // Tema scuro: ricolora i pixel usando GDI
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
                    // Luminosita: piu e scuro, piu l'azzurro e intenso
                    int lum = ((int)pr * 299 + (int)pg * 587 + (int)pb * 114) / 1000;
                    int t = 255 - lum; // t alto = pixel scuro = colore pieno
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
    // CLIPPING
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
    rcStatus.right = rcRow.right - 39 - scrollbarOffset;
    rcStatus.left   = rcRow.left + 80;
    rcStatus.top    = rcRow.top + 6;
    rcStatus.bottom = rcStatus.top + 18;

    DrawTextW(hdc, LOC(STR_CONNECTED_TEXT), -1, &rcStatus,
              DT_RIGHT | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
}
else if (isTransitioning) {
    // Stato di transizione: riga propria sotto il nome, tutta larghezza disponibile
    SelectObject(hdc, g_hFontNormal);
    SetTextColor(hdc, GetSecondaryTextColor());

    const WCHAR* transitionText = (item->connState == CONN_STATE_CONNECTING)
        ? LOC(STR_CONNECTING) : LOC(STR_DISCONNECTING);

    RECT rcTransition;
    rcTransition.left   = rcRow.left + 10;
    rcTransition.right  = rcRow.right - 10;
    rcTransition.top    = rcRow.top + 24;   // sotto il nome, che è a rcRow.top+6
    rcTransition.bottom = rcTransition.top + 18;

    DrawTextW(hdc, transitionText, -1, &rcTransition,
              DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
}
                
DrawNativeSignalIcon(hdc, rcRow.right - 10 - scrollbarOffset, rcRow.top+2, item->signalQuality);    }
        }
                SelectClipRgn(hdc, NULL);
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

    // Solo per tema scuro
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

    // Disegna su un bitmap di memoria
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

    // Testo
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
    
    if (hwndCtrl == g_hWndCheckboxConnect && g_SelectedRowIndex >= 0) {
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
    
    // Gestione label del dialogo password: azzurre per entrambi i temi
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
    
    if (hwndBtn == g_hWndCheckboxConnect && g_SelectedRowIndex >= 0) {
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
        
        g_IsHoveringLink    = PtInRect(&rcF, pt) != 0;
        g_IsHoveringRefresh = PtInRect(&g_rcRefreshButton, pt) != 0;
        g_IsHoveringArrow   = PtInRect(&g_rcArrowButton,   pt) != 0;
        
        if (g_hWndButtonConnect && IsWindow(g_hWndButtonConnect) && IsWindowVisible(g_hWndButtonConnect)) {
            RECT rcConnect;
            GetWindowRect(g_hWndButtonConnect, &rcConnect);
            POINT ptScreen = pt;
            ClientToScreen(hwnd, &ptScreen);
            g_IsHoveringConnectButton = PtInRect(&rcConnect, ptScreen) != 0;
        } else {
            g_IsHoveringConnectButton = FALSE;
        }
        
        int newHovered = (my >= LIST_Y_START && my < LIST_Y_END) ? HitTestRows(mx,my) : -1;
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
        if (PtInRect(&g_rcRefreshButton,pt)) {
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
        RefreshWifiData(g_Ctx.hWlanClient);
        ClampScrollPos();
        UpdateLayoutGeometry();
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);
    } else {
        Wh_Log(L"Manual refresh skipped: WLAN client not available");
    }
    break;
}
        if (PtInRect(&g_rcArrowButton,pt)) {
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
        if (g_bListExpanded && ly >= LIST_Y_START && ly < LIST_Y_END) {
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
        if (g_bListExpanded && ry >= LIST_Y_START && ry < LIST_Y_END) {
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
} // =====================================================================
// Network icon detection via pnidui.dll
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
            // Lazy WLAN open on first show.
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
            if (g_Ctx.hWlanClient) RefreshWifiData(g_Ctx.hWlanClient);
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
    // Open WLAN handle off the init path to avoid blocking explorer.exe.
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


BOOL Wh_ModInit() {
    Wh_Log(L"=== Wh_ModInit v2.8.7 ===");
    DetectWindowsVersion();
    LoadSettings();

    DarkContextMenu::Init();

    ZeroMemory(&g_Ctx, sizeof(g_Ctx));
    InitializeCriticalSection(&g_Ctx.csLock);

    g_hConnectMutex = CreateMutexW(NULL, FALSE, L"Local\\Win7NetFlyout_ConnectMutex");
    DetermineLocale();
    g_uTaskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
    LoadSystemIcons();
    InitGdiPlusRendering();

    if (!IsExplorerProcess()) {
        g_Initialized = TRUE;
        return TRUE;
    }

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

    // WlanOpenHandle is done on the hotkey thread (see HotkeyThreadProc).
    g_Initialized = TRUE;
    return TRUE;
}
void Wh_ModSettingsChanged() {
    BOOL oldRoundedCorners = g_Settings.useRoundedCorners;
    int  oldTheme          = g_Settings.theme;
    LoadSettings();
    DetermineLocale();

    // Sync context menu dark/light state with theme setting.
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
        if (g_RefreshTimer) {
            KillTimer(g_hWndFlyout, g_RefreshTimer);
            g_RefreshTimer = 0;
        }
        if (g_Settings.refreshInterval > 0) {
            g_RefreshTimer = SetTimer(g_hWndFlyout, 1000, g_Settings.refreshInterval, NULL);
        }
        PostMessageW(g_hWndFlyout, WM_REFRESH_DATA, 0, 0);
        InvalidateRect(g_hWndFlyout, NULL, TRUE);
    }
}
void Wh_ModUninit() {
    SafeCleanup();
    DeleteCriticalSection(&g_Ctx.csLock);

    DarkContextMenu::Uninit();

    UnregisterClassW(L"Win7NetworkFlyoutSafe", GetModuleHandle(NULL));
    UnregisterClassW(L"Win7NetPwdClass", GetModuleHandle(NULL));
    UnregisterClassW(L"Win7NetFlyoutHidden", GetModuleHandle(NULL));
}
