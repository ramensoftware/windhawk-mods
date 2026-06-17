// ==WindhawkMod==
// @id             win7-network-flyout-recreation
// @name           Windows 7 Network Flyout Recreation
// @description    This mod recreates the Windows 7 network flyout panel, replacing the modern Windows 10/11 flyout, along with the Windows 8 flyout as a configurable fallback
// @version        1.3.1
// @author         babamohammed
// @github         https://github.com/babamohammed2022
// @include        explorer.exe
// @architecture   x86-64
// @compilerOptions -lgdi32 -ldwmapi -luxtheme -lole32 -lshell32 -luser32 -lcomctl32 -liphlpapi -lnetapi32 -lwlanapi -luuid
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Windows 7 Network Flyout Recreation

This Windhawk mod recreates the Windows 7 network flyout panel, replacing the modern Windows 10/11 network flyout with a classic interface.

## Features

- **Wi-Fi network list** — Shows all available networks with signal strength
- **Connect/Disconnect** — Connect to networks with password support
- **Privacy mode** — Hide real SSIDs (shows as Network 1, Network 2...)
- **Windows 7 style tooltips** — Full network info on hover (SSID, signal, security, radio type)
- **Network status & properties** — Right-click context menu for quick access
- **Keyboard navigation** — Arrow keys, Enter, Escape support
- **Auto-refresh** — Configurable network list refresh interval
- **Registry fallback** — Optional Windows 8 style flyout (ReplaceVan method)
- **Language support** — English and Italian (auto-detect or manual override)

## Hotkeys

| Key | Action |
|-----|--------|
| **Ctrl+H** | Toggle network flyout |

*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- language: 0
  $name: Language
  $description: Force language override (0 = auto-detect, 1 = English, 2 = Italian)
  $options:
    - 0: Auto-detect
    - 1: English
    - 2: Italian
- interceptNativeFlyout: true
  $name: Intercept native network flyout
  $description: Replace the Windows 10/11 network flyout with this classic one when clicking the tray icon
- privacyMode: false
  $name: Privacy mode (hide network names)
  $description: Show all networks as Network 1, Network 2... instead of real SSIDs
- useRegistryMethod: true
  $name: Use Registry ReplaceVan method
  $description: Sets ReplaceVan=2 to enable Windows 8 style network flyout as fallback. Automatically removed when the mod is disabled.
- redirectNetworkContextMenu: true
  $name: Redirect network context menu
  $description: Redirect network tray context menu to classic network connections
- refreshInterval: 3000
  $name: Refresh interval (ms)
  $description: How often to automatically refresh the network list (0 = disable auto-refresh)
- enableHotkey: false
  $name: Enable global hotkey (Ctrl+H)
  $description: Register Ctrl+H to toggle the network flyout. Disabled by default to avoid conflicts with browsers and editors.
- useRoundedCorners: false
  $name: Use rounded corners
  $description: Apply rounded corners to the flyout window (disabled by default for classic theme compatibility)
*/
// ==/WindhawkModSettings==

// Version 1.3.1 - Fixed connection password prompt and UI freezes
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
#include <shlwapi.h>
#include <iphlpapi.h>
#include <netlistmgr.h>
#include <windhawk_utils.h>
#include <process.h>

// -------------------------------------------------------
// Layout Constants
// -------------------------------------------------------
#define WINDOW_WIDTH        340
#define WINDOW_HEIGHT       405
#define HEADER_HEIGHT       105
#define FOOTER_HEIGHT       60
#define LIST_Y_START        (HEADER_HEIGHT + 1)
#define LIST_Y_END          (WINDOW_HEIGHT - FOOTER_HEIGHT)
#define LIST_MAX_HEIGHT     (LIST_Y_END - LIST_Y_START)
#define WIFI_LABEL_Y        (HEADER_HEIGHT - 24)
#define ROW_HEIGHT_NORMAL   26
#define ROW_HEIGHT_EXPANDED 74

#define IDC_CONN_BUTTON     1002
#define IDC_AUTO_CHECKBOX   1003
#define HOTKEY_ID           9001
#define WM_REFRESH_DATA     (WM_USER + 100)
#define WM_SAFE_CLOSE       (WM_USER + 101)
#define WM_SHOW_FLYOUT      (WM_USER + 102)
#define WM_CONNECTION_RESULT (WM_USER + 103)
#define WM_CONNECTION_TIMEOUT (WM_USER + 104)
#define WM_ASYNC_CONNECT_COMPLETE (WM_USER + 105)

#define IDM_CONNECT         2001
#define IDM_DISCONNECT      2002
#define IDM_STATUS          2003
#define IDM_PROPERTIES      2004

#define REG_PATH_NETWORK    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Control Panel\\Settings\\Network"
#define REG_VALUE_REPLACEVAN L"ReplaceVan"

#define TRAY_NETWORK_ID 2
#define CLICK_DEBOUNCE_MS 600
#define WM_HOTKEY_SETTINGS_CHANGED (WM_USER + 200)
#define INIT_DELAY_MS 1000
#define CONNECTION_TIMEOUT_MS 15000
// Connection related definitions
#define WLAN_REASON_CODE_INVALID_PROFILE    0x00038001  // 229377
// BASE 64
// Refresh normal - base64
static const WCHAR* REFRESH_ICON_NORMAL_BASE64 = L"iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsIAAA7CARUoSoAAAAAZdEVYdFNvZnR3YXJlAFBhaW50Lk5FVCA1LjEuMTITAUd0AAAAuGVYSWZJSSoACAAAAAUAGgEFAAEAAABKAAAAGwEFAAEAAABSAAAAKAEDAAEAAAACAAAAMQECABEAAABaAAAAaYcEAAEAAABsAAAAAAAAAPJ2AQDoAwAA8nYBAOgDAABQYWludC5ORVQgNS4xLjEyAAADAACQBwAEAAAAMDIzMAGgAwABAAAAAQAAAAWgBAABAAAAlgAAAAAAAAACAAEAAgAEAAAAUjk4AAIABwAEAAAAMDEwMAAAAACDfy8cctDT3wAAAN9JREFUOE9joAkw3nrrPwhDuaSB6Ree/zddcZVozUxQGg4WHn/OkKLAB+URBigGzDzy8D/j9z8MPEyMDNeuXVtx9epVMagUToBiwILDTxkYfv5hePr2O4OWllaEtrb2K6gUGFi2H8PwGiOIsKw/hNPPxxvtMNTAxEAAzrCs2IthyPEOZ4hmPHJwA0DAsnAHwpZ+D4hmJDF0AFODAiyzN2HV0LX8wn+QHC55ooB54rL/M9aeJ8+AqcuO/zcMmYKhGSMh4QJTF25nSPTRgPJIAIdPXFZXME/6D8JQoUEFGBgAn8daV7VTN5UAAAAASUVORK5CYII=////v7+/r6+vj4+Pz8/P7+/v39/TO12sjo8fHx8fn5+ZfQ5zWo1erq6ubm5vf398jh7jiXzpnI4+Li4tLS0unp6ZnE4TmOyqXK5NbW1tXV1e/v7zmHxoq32/Pz8/T09Pb29jl/wvDw8Dl4vTlxuDlrstjY2Iqn0Ofn5+Pj48/Pz9/f3+jo6KS21Tdhppitz9PT07u7u9vb2+iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAMAAAAoLQ9TAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAABRUExURev0/TO12pfQ5zWo1TiXzpnI45nE4TmOyqfL5Orz/DmHxo663Dl/wkuKyESGxTl4vTlxuDlrsoqn0Ddhppitz5WmxzFUlCpHfpKgvMPI0yA3YglAoVgAAAAJcEhZcwAADsIAAA7CARUoSoAAAAAZdEVYdFNvZnR3YXJlAFBhaW50Lk5FVCA1LjEuMTITAUd0AAAAuGVYSWZJSSoACAAAAAUAGgEFAAEAAABKAAAAGwEFAAEAAABSAAAAKAEDAAEAAAACAAAAMQECABEAAABaAAAAaYcEAAEAAABsAAAAAAAAAPJ2AQDoAwAA8nYBAOgDAABQYWludC5ORVQgNS4xLjEyAAADAACQBwAEAAAAMDIzMAGgAwABAAAAAQAAAAWgBAABAAAAlgAAAAAAAAACAAEAAgAEAAAAUjk4AAIABwAEAAAAMDEwMAAAAACDfy8cctDT3wAAAFRJREFUKFN9yEkSgDAIRFHibJyIs7n/QS2goxvLt2h+Qd+cQ0CWI5KiREBVNy3SeN/Z1aVeDKOGfSbxHMHMOsI+QXcOdl/LioBtRyTHiTBXjKh/RDeDBAMcwXjgKAAAAABJRU5ErkJggg==//9T3qpBX7Ilk83uLCM4kMlo+rsnBAwjm0Mq6DJfQLFkpoJWrlRrdes3IRvNVrtjaxySi/dCx+kCroQDzxcMvQ+gBoHPJPoEviVg3EMYhqBEgAk/QIRBDRhGETBStpQynkyteDZfLFdrdVb4ySjxTHez3e2XCRkRHFzF4Xg6xyCB0C7XK843Vzn7oEu18P74+xB26ZmYOnsBTi4RDe3fqLQAAAAASUVORK5CYII=";

// Refresh hover icon - base64
// static const WCHAR* REFRESH_ICON_HOVER_BASE64 = L"iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAAZdEVYdFNvZnR3YXJlAFBhaW50Lk5FVCA1LjEuMTITAUd0AAAAuGVYSWZJSSoACAAAAAUAGgEFAAEAAABKAAAAGwEFAAEAAABSAAAAKAEDAAEAAAACAAAAMQECABEAAABaAAAAaYcEAAEAAABsAAAAAAAAAGAAAAABAAAAYAAAAAEAAABQYWludC5ORVQgNS4xLjEyAAADAACQBwAEAAAAMDIzMAGgAwABAAAAAQAAAAWgBAABAAAAlgAAAAAAAAACAAEAAgAEAAAAUjk4AAIABwAEAAAAMDEwMAAAAADZp5qVybcLXwAAAqBJREFUOE99Uk9IFGEU/83sruMsO7O6mlkWWrkUBCV56JBUJyHo4CE6dOtWQUeNLl2CCKFz3qSgWxQE1aUQIgLJlMoOUora5m7qus7s7Mw3O/96b1bLDvXg8c333vu97/feb6SL954N78tqoz2tacgAIvL/mUQeki9WbBSM6giuj72KvhaNSIRRZAd/3KG7s+O+07mWMYyV87s0dO3WUREhDLfhwvWxbjooW4Lcxabjw9jO08m1jGGszJQC4sQeEv8EDTG3buH2VBHLhg1h2/ApwbQ5z+d2PWNlnpkTfkRF9LFRE3gws4qhDgWa2IQlXCqO4t3wGTvXUoBjvDcKNJqkFRkLxSqWVmyk6C2PCpNNCppTMkIhkEQA4fnwmAGjyeIG/J0KPMx8LODtdAnzP2v4VnBQsRR06ioq5SpuPZpFqWRAgQ9R936rJdMjMOtANZBRMlzcmVqDaru4+mIJth+hVLZxc/wTHi+YuPHwC34UTfhBEGMYG++AKX23gf6+Ljy5cAAhbf/5pV4cy+fw5n0BXVoSeWKYk0N8mF2DRPw9AjI2HoHnD2ini2aI3sOduH/lOPQWGZNzBQyc6sH5093IwsNgfxtO9mWxsLoRY9hiBrxVYosgkcJSDbBUFcuOjCCTw6qXgKAFdqZq0PUE3lHTKpr+VoHl80jYWF9qYksq0i3t0DUNqjAxOTGBwYH9qNMImfY9UDM6gi0ZGjIS0KOAH3IT0lwi2SjlU1yIOs6e6ceRowex4clQtSwxluK6uAGfCWrDYIcQDQ/i06yHWFdyMHPd+Fzx0dzagSBJQm5hGCtdG3sdXT53AkpGQ51Y8O+50xrvNH5bNr43JSS4VhXjL6chDd19Ory3RR891JaBxFXbiH8Z1bD+82ULK5vmyC9i+q1W4vC7zgAAAABJRU5ErkJggg==/u7szO7s7O7sZmMajaIFf4KQh4APvlhpfbBaSgVpBUHBxyIIPogvgti+iD8PloL00VJCnwxWRRSKgdA+WGgRxZhsG1E3k0Sz2WR3s//z4zl3f7KzUgrpYb+Zvfee891vvntmBCi+vn7fSy3WUbNdHq45gqqCHb0afjh5QIijV255iWQvzn4xjI3JSDNlbWFli7h8+zGWs4sQey/c9n46tQ8fmAYKda+ZsraIaQJv8yUc//4hFNfzsD5hIEekbEQnVDgIC1tCODVUqlXUHQcO1XTnMpiDuZhToTFAQt1uuC404eHjh68kJgs12LUqbMd9P7cDzMUhifk/79KCQ6QBz8aN51kohTrG9m3BBreM5cIKbNv25XajybtK7NClBVDC1EIJI4/mcGKzidfWPKZn5ik7INGZ2w36yWhYQdG5SKV4MrMCUbYRUQQpLVCmAkULwhUKdOEiHnCgedSidZt8d9u1rWgopgn2JywcJFUHBvXjyO8WULUxu1jG0OB27B4aRELXYCgudBJ+8Ls/ZV7QpUOtrHrPXBw+K2TBpUcSqNgSd/6Ya8/FIzoMTWmv8900QtBRR5k6hjmavA3iGvWKVRKwih5Gz+wmpfX3wPMVB/js29988zyORCKwyRLmYC6Otsccc2WBN9RWo+f2+IrlmOLw+THffAs8P0D92xkdHtNFkOoyqV8qYvSbT+hRaBO6P516gb9fz8v/jIO71su11jrjxVym0W5NL1Y9JuddCATUIGarKtJEfvPip0il5/FPpoiX+TrGpzOYsBbRH1Phllbw1d5N+OvZJH68O4bUm4Lk8HnMA5u2YjikmtvKqgYw/nIJ6YpAcmATIj29CEZjiEXp9c/lUStksfPDHqQzOZh9/QjHE7LeR8z6ebcWWLlCykNGDJF4Ut7VoI4tZhADpo4bI/dx5MAgAiEbJU9FtGcd5URlbcsLnxXd5I03TZX3cMDDzOQU9uw/idLyDI59+REm0gswzB5oepg4FFnnU8ybdBN3I0dn1b9tK8YfXJdIWQtQ9ChCUVM+nXzzKK8puEMx2/EfmM47+HV6CfcmZpFaqkrfpVp6Im5fzmkrVuhb8Ha5iF19Gn0qyYJ/AdVAkC1hUhjv7UeibwOMWAIBLSTXOIc5mIs5xbGrv3g6HdDpz4exzgw391tbZPJlXLv7GJVclk8IOErk1opovCT/IxRq1Y1RDz+fOSTeATe7heJTThHzAAAAAElFTkSuQmCC";

// Handle delle icone create dal base64
static HICON g_hIconRefreshNormal = NULL;
static HICON g_hIconRefreshHover = NULL;
static INetworkListManager* g_pNLM = NULL;


// -------------------------------------------------------
// Connection State Machine (simplified)
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
    BOOL useRegistryMethod;
    BOOL redirectNetworkContextMenu;
    int  refreshInterval;
    int  language;
    BOOL enableHotkey;
    BOOL useRoundedCorners;
} g_Settings = { TRUE, FALSE, TRUE, TRUE, 3000, 0, FALSE, FALSE };

static bool s_settingsSavedOnce = false;

void LoadSettings() {
    int raw_intercept  = Wh_GetIntSetting(L"interceptNativeFlyout");
    int raw_privacy    = Wh_GetIntSetting(L"privacyMode");
    int raw_registry   = Wh_GetIntSetting(L"useRegistryMethod");
    int raw_redirectCtx= Wh_GetIntSetting(L"redirectNetworkContextMenu");
    int raw_refresh    = Wh_GetIntSetting(L"refreshInterval");
    int raw_language   = Wh_GetIntSetting(L"language");
    int raw_enableHotkey = Wh_GetIntSetting(L"enableHotkey");
    int raw_roundedCorners = Wh_GetIntSetting(L"useRoundedCorners");

    if (!s_settingsSavedOnce &&
        raw_intercept == 0 && raw_privacy == 0 &&
        raw_registry  == 0 && raw_redirectCtx == 0 && 
        raw_refresh == 0 && raw_language == 0 && raw_roundedCorners == 0) {
        g_Settings.interceptNativeFlyout      = TRUE;
        g_Settings.privacyMode               = FALSE;
        g_Settings.useRegistryMethod         = TRUE;
        g_Settings.redirectNetworkContextMenu = TRUE;
        g_Settings.refreshInterval            = 3000;
        g_Settings.language                  = 0;
        g_Settings.enableHotkey              = FALSE;
        g_Settings.useRoundedCorners         = FALSE;
    } else {
        g_Settings.interceptNativeFlyout      = raw_intercept   != 0;
        g_Settings.privacyMode               = raw_privacy     != 0;
        g_Settings.useRegistryMethod         = raw_registry    != 0;
        g_Settings.redirectNetworkContextMenu = raw_redirectCtx != 0;
        g_Settings.refreshInterval            = raw_refresh;
        g_Settings.language                  = raw_language;
        g_Settings.enableHotkey              = raw_enableHotkey != 0;
        g_Settings.useRoundedCorners         = raw_roundedCorners != 0;
    }

    if (g_Settings.refreshInterval > 0 && g_Settings.refreshInterval < 1000) {
        g_Settings.refreshInterval = 1000;
    }
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
    
    // SINGLE STATE FIELD - the only truth
    ConnectionState connState;
    DWORD operationStartTime;  // For timeout detection
} WifiNetworkItem;

// Async connection context for non-blocking operations
typedef struct {
    HWND hWndNotify;
    GUID interfaceGuid;
    WCHAR ssid[33];
    WCHAR password[65];
    BOOL hasProfile;
    BOOL isSecured;     
    DOT11_BSS_TYPE dot11BssType;
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
}

// -------------------------------------------------------
// Global Variables (cleaned up)
// -------------------------------------------------------
static ModContext g_Ctx        = {0};
static BOOL       g_Initialized = FALSE;

HWND g_hWndFlyout          = NULL;
HWND g_hWndButtonConnect   = NULL;
HWND g_hWndCheckboxConnect = NULL;
BOOL g_bListExpanded        = TRUE;

HFONT g_hFontNormal    = NULL;
HFONT g_hFontBold      = NULL;
HFONT g_hFontUnderline = NULL;
HFONT g_hFontButton    = NULL;
HFONT g_hFontCheckbox  = NULL;
HFONT g_hFontArrow     = NULL;

WifiNetworkItem g_NetworkList[50];
int  g_NetworkCount           = 0;
BOOL g_IsHoveringLink         = FALSE;
BOOL g_IsHoveringRefresh      = FALSE;
BOOL g_IsHoveringArrow        = FALSE;

int  g_SelectedRowIndex       = -1;
int  g_HoveredRowIndex        = -1;
int  g_KeyboardSelectedIndex  = -1;
int  g_ContextMenuTargetIndex = -1;

RECT g_rcRefreshButton = { 0 };
RECT g_rcArrowButton = { 0 };

HICON g_hIconNetworkMap  = NULL;
HICON g_hIconSignalBars[6] = { NULL };
HICON g_hIconRefreshWin7 = NULL;

// Single pending connection tracking
int   g_PendingConnectIndex = -1;
HWND  g_hTooltip = NULL;

UINT_PTR g_RefreshTimer = 0;
UINT_PTR g_TimeoutTimer = 0;  // Single global timeout timer

HWND G_hSubclassedToolbar = nullptr;
UINT_PTR G_SubclassId = 0;

// Mutex per prevenire operazioni concorrenti
static HANDLE g_hConnectMutex = NULL;

LRESULT CALLBACK ToolbarWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass);

using TrackPopupMenuEx_t = BOOL(WINAPI*)(HMENU, UINT, int, int, HWND, const TPMPARAMS*);
static TrackPopupMenuEx_t g_origTrackPopupMenuEx = nullptr;

static WCHAR g_TooltipBuffer[1024] = {0};

// -------------------------------------------------------
// Localization (unchanged)
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
    }}
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
void RefreshWifiData(HANDLE hClient);
void UpdateLayoutGeometry();
void ConnectToNetwork(int index);
void ConnectToNetworkAsync(int index);
void DisconnectFromNetwork(int index);
void CheckConnectionTimeouts(void);
BOOL SafeToAccessUI(void);
void SafeCleanup(void);
void ToggleFlyoutWindow(void);
void InitTooltip(HWND hwnd);
void UpdateTooltipForRow(HWND hwnd, int index);
BOOL GetRowRect(int index, RECT* rcRow);
void InstallTrayInterception(void);
void RemoveTrayInterception(void);
void InitRefreshButtonRect(void);
void SetKeyboardFocus(int index);
void ClearKeyboardFocus(void);
BOOL IsInternetConnected(void);
static BOOL AskForPasswordAndConnect(int index);
// Base64 handling
// Funzione per decodificare base64 e creare HICON
static HICON CreateIconFromBase64PNG(const WCHAR* base64Str) {
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
    HMODULE hGdi = LoadLibraryW(L"gdiplus.dll");
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
            // DWORD gdiplusVersion = 1;
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

// -------------------------------------------------------
// Refresh button rect
// -------------------------------------------------------
void InitRefreshButtonRect() {
    g_rcRefreshButton.right  = WINDOW_WIDTH - 20;
    g_rcRefreshButton.left   = g_rcRefreshButton.right - 22;
    g_rcRefreshButton.top    = 8;
    g_rcRefreshButton.bottom = 30;
}

// -------------------------------------------------------
// Registry hooks (unchanged)
// -------------------------------------------------------
typedef LONG (WINAPI *RegQueryValueExW_t)(HKEY, LPCWSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
RegQueryValueExW_t Real_RegQueryValueExW = NULL;

typedef LONG (WINAPI *RegGetValueW_t)(HKEY, LPCWSTR, LPCWSTR, DWORD, LPDWORD, PVOID, LPDWORD);
RegGetValueW_t Real_RegGetValueW = NULL;

LONG WINAPI Hook_RegQueryValueExW(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, 
                                   LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    if (!g_Settings.useRegistryMethod)
        return Real_RegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
    
    if (lpValueName && _wcsicmp(lpValueName, REG_VALUE_REPLACEVAN) == 0) {
        if (lpType) *lpType = REG_DWORD;
        if (lpData && lpcbData) {
            if (*lpcbData >= sizeof(DWORD)) {
                *(DWORD*)lpData = 2;
                *lpcbData = sizeof(DWORD);
                return ERROR_SUCCESS;
            } else {
                *lpcbData = sizeof(DWORD);
                return ERROR_MORE_DATA;
            }
        }
        if (lpcbData) { *lpcbData = sizeof(DWORD); return ERROR_SUCCESS; }
    }
    return Real_RegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

LONG WINAPI Hook_RegGetValueW(HKEY hkey, LPCWSTR lpSubKey, LPCWSTR lpValue, DWORD dwFlags, 
                               LPDWORD pdwType, PVOID pvData, LPDWORD pcbData) {
    if (!g_Settings.useRegistryMethod)
        return Real_RegGetValueW(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
    
    if (lpValue && _wcsicmp(lpValue, REG_VALUE_REPLACEVAN) == 0) {
        if (pdwType) *pdwType = REG_DWORD;
        if (pvData && pcbData) {
            if (*pcbData >= sizeof(DWORD)) {
                *(DWORD*)pvData = 2;
                *pcbData = sizeof(DWORD);
                return ERROR_SUCCESS;
            } else {
                *pcbData = sizeof(DWORD);
                return ERROR_MORE_DATA;
            }
        }
        if (pcbData) { *pcbData = sizeof(DWORD); return ERROR_SUCCESS; }
    }
    return Real_RegGetValueW(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
}

// -------------------------------------------------------
// TrackPopupMenuEx Hook (unchanged)
// -------------------------------------------------------
static constexpr UINT CMD_OPEN_NETWORK_SETTINGS_TRAY = 3109;
static constexpr UINT CMD_NETWORK_STATUS_TRAY         = 3108;
static constexpr UINT CMD_NETWORK_DIAGNOSTICS_TRAY    = 3110;
static thread_local int g_trackPopupHookDepth = 0;

static BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y,
                                         HWND hWnd, const TPMPARAMS* lptpm) {
    if (!g_Settings.redirectNetworkContextMenu)
        return g_origTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);
    if (g_trackPopupHookDepth > 0)
        return g_origTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);
    
    g_trackPopupHookDepth++;
    UINT modifiedFlags = uFlags | TPM_RETURNCMD;
    BOOL result = g_origTrackPopupMenuEx(hMenu, modifiedFlags, x, y, hWnd, lptpm);
    
    if (result > 0) {
        UINT cmd = (UINT)result;
        switch (cmd) {
            case CMD_OPEN_NETWORK_SETTINGS_TRAY:
            case CMD_NETWORK_STATUS_TRAY:
                ShellExecuteW(NULL, L"open", L"explorer.exe", 
                    L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}", NULL, SW_SHOWNORMAL);
                g_trackPopupHookDepth--;
                return 0;
            case CMD_NETWORK_DIAGNOSTICS_TRAY:
                ShellExecuteW(NULL, L"open", L"msdt.exe", 
                    L"-id NetworkDiagnosticsWeb", NULL, SW_SHOWNORMAL);
                g_trackPopupHookDepth--;
                return 0;
        }
        if (!(uFlags & TPM_RETURNCMD)) {
            SendMessageW(hWnd, WM_COMMAND, (WPARAM)cmd, 0);
            g_trackPopupHookDepth--;
            return TRUE;
        }
    }
    g_trackPopupHookDepth--;
    return result;
}

// -------------------------------------------------------
// SSID display helper
// -------------------------------------------------------
static void GetDisplaySSID(int index, WCHAR* buf, int bufLen) {
    if (g_Settings.privacyMode)
        StringCchPrintfW(buf, bufLen, LOC(STR_NETWORK_PRIVACY_FMT), index + 1);
    else
        StringCchCopyW(buf, bufLen, g_NetworkList[index].ssid);
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

void FreeSystemIcons() {
    if (g_hIconRefreshNormal) { DestroyIcon(g_hIconRefreshNormal); g_hIconRefreshNormal = NULL; }
    if (g_hIconRefreshHover) { DestroyIcon(g_hIconRefreshHover); g_hIconRefreshHover = NULL; }
    if (g_hIconNetworkMap) { DestroyIcon(g_hIconNetworkMap); g_hIconNetworkMap = NULL; }
    for (int i = 0; i < 6; i++)
        if (g_hIconSignalBars[i]) { DestroyIcon(g_hIconSignalBars[i]); g_hIconSignalBars[i] = NULL; }
    if (g_hIconRefreshWin7) { DestroyIcon(g_hIconRefreshWin7); g_hIconRefreshWin7 = NULL; }
}

void InitGlobalFonts() {
    if (g_hFontNormal) return;
    g_hFontNormal    = CreateFontW(-12,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    g_hFontBold      = CreateFontW(-12,0,0,0,FW_BOLD,  0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    g_hFontUnderline = CreateFontW(-12,0,0,0,FW_NORMAL,0,1,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    g_hFontButton    = CreateFontW(-12,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    g_hFontCheckbox  = CreateFontW(-11,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    g_hFontArrow     = CreateFontW(-11,0,0,0,FW_NORMAL,0,0,0,SYMBOL_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Marlett");
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
// WLAN data refresh (preserves connection state)
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
                    int converted = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)network.dot11Ssid.ucSSID, (int)len, tempList[tempCount].ssid, 32);
                    if (converted <= 0) {
                        size_t copyLen = (len < 32u) ? len : (size_t)32u;
                        for (size_t k = 0; k < copyLen; k++)
                            tempList[tempCount].ssid[k] = (WCHAR)(BYTE)network.dot11Ssid.ucSSID[k];
                        tempList[tempCount].ssid[copyLen] = L'\0';
                    } else {
                        tempList[tempCount].ssid[converted] = L'\0';
                    }
                }

                // Check duplicates
                BOOL duplicate = FALSE;
                for (int d = 0; d < tempCount; d++) {
                    if (wcscmp(tempList[d].ssid, tempList[tempCount].ssid) == 0) {
                        if (network.wlanSignalQuality > tempList[d].signalQuality)
                            tempList[d].signalQuality = network.wlanSignalQuality;
                        if (network.dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED)
                            tempList[d].connState = CONN_STATE_CONNECTED;
                        tempList[d].isSecured = network.bSecurityEnabled;
                        tempList[d].dot11BssType = network.dot11BssType;
                        duplicate = TRUE;
                        break;
                    }
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

                if (pProfList) {
                    for (DWORD p = 0; p < pProfList->dwNumberOfItems; p++) {
                        if (wcscmp(pProfList->ProfileInfo[p].strProfileName, tempList[tempCount].ssid) == 0) {
                            tempList[tempCount].hasProfile = TRUE;
                            break;
                        }
                    }
                }

                // Move connected network to top
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

    // Preserve connection state for pending operations
    EnterCriticalSection(&g_Ctx.csLock);
    if (tempCount > 0 && tempCount <= 50) {
        // Keep existing connection states for networks being connected/disconnected
        for (int t = 0; t < tempCount; t++) {
            for (int e = 0; e < g_NetworkCount; e++) {
                if (wcscmp(tempList[t].ssid, g_NetworkList[e].ssid) == 0) {
                    if (g_NetworkList[e].connState == CONN_STATE_CONNECTING ||
                        g_NetworkList[e].connState == CONN_STATE_DISCONNECTING ||
                        g_NetworkList[e].connState == CONN_STATE_ERROR) {
                        tempList[t].connState = g_NetworkList[e].connState;
                        tempList[t].operationStartTime = g_NetworkList[e].operationStartTime;
                    }
                    // Preserve hasProfile status to prevent re-asking password
                    if (g_NetworkList[e].hasProfile) {
                        tempList[t].hasProfile = TRUE;
                    }
                    break;
                }
            }
        }
        CopyMemory(g_NetworkList, tempList, sizeof(WifiNetworkItem) * tempCount);
        g_NetworkCount = tempCount;
    } else if (tempCount == 0) {
        if (now - lastValidRefresh > 30000) {
            ZeroMemory(g_NetworkList, sizeof(g_NetworkList));
            g_NetworkCount = 0;
        }
    }
    lastValidRefresh = now;
    LeaveCriticalSection(&g_Ctx.csLock);

    if (g_NetworkCount > 0 && g_NetworkList[0].connState == CONN_STATE_CONNECTED) {
        g_NetworkList[0].hasInternetAccess = IsInternetConnected();
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

LRESULT CALLBACK Win7PasswordWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    PasswordDlgData* data = (PasswordDlgData*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    switch (uMsg) {
    case WM_NCHITTEST: {
        LRESULT r = DefWindowProcW(hwnd, uMsg, wParam, lParam);
        if (r==HTBOTTOM||r==HTBOTTOMLEFT||r==HTBOTTOMRIGHT||
            r==HTLEFT||r==HTRIGHT||r==HTTOP||r==HTTOPLEFT||r==HTTOPRIGHT)
            return HTCLIENT;
        return r;
    }
    case WM_CREATE: {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lParam;
        data = (PasswordDlgData*)cs->lpCreateParams;
        if (!data) return -1;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)data);
        HFONT hFontDlg = CreateFontW(-12,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
        
        CreateWindowExW(0, WC_STATICW, LOC(STR_PWD_INSTRUCTIONS),
            WS_CHILD|WS_VISIBLE, 15,15,380,20, hwnd,(HMENU)200,cs->hInstance,NULL);
        CreateWindowExW(0, WC_STATICW, LOC(STR_PWD_LABEL),
            WS_CHILD|WS_VISIBLE, 15,53,115,18, hwnd,NULL,cs->hInstance,NULL);
        HWND hEdit = CreateWindowExW(WS_EX_CLIENTEDGE, WC_EDITW, L"",
            WS_CHILD|WS_VISIBLE|ES_PASSWORD|ES_AUTOHSCROLL,
            135,50,255,22, hwnd,(HMENU)101,cs->hInstance,NULL);
        SendMessageW(hEdit, WM_SETFONT, (WPARAM)hFontDlg, TRUE);
        SetFocus(hEdit);
        
        HWND hCheck = CreateWindowExW(0, WC_BUTTONW, LOC(STR_PWD_HIDE_CHARS),
            WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX, 135,80,200,18, hwnd,(HMENU)102,cs->hInstance,NULL);
        SendMessageW(hCheck, WM_SETFONT, (WPARAM)hFontDlg, TRUE);
        
        RECT rcClient; GetClientRect(hwnd, &rcClient);
        int btnW=85, btnH=24, btnY=rcClient.bottom-35;
        HWND hBtnOk = CreateWindowExW(0, WC_BUTTONW, LOC(STR_PWD_OK),
            WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
            rcClient.right-btnW-15, btnY, btnW,btnH, hwnd,(HMENU)IDOK,cs->hInstance,NULL);
        SendMessageW(hBtnOk, WM_SETFONT, (WPARAM)hFontDlg, TRUE);
        HWND hBtnCancel = CreateWindowExW(0, WC_BUTTONW, LOC(STR_PWD_CANCEL),
            WS_CHILD|WS_VISIBLE, rcClient.right-(btnW*2)-25, btnY, btnW,btnH,
            hwnd,(HMENU)IDCANCEL,cs->hInstance,NULL);
        SendMessageW(hBtnCancel, WM_SETFONT, (WPARAM)hFontDlg, TRUE);
        break;
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdc = (HDC)wParam;
        HWND hwndCtrl = (HWND)lParam;
        
        // Per la checkbox "Hide characters" (id 102) o altri controlli
        if (GetDlgCtrlID(hwndCtrl) == 102) {
            // Sfondo trasparente per la checkbox
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(0, 0, 0));
            return (INT_PTR)GetStockObject(HOLLOW_BRUSH);
        }
        
        // Per l'istruzione in alto (id 200) - testo blu
        if (GetDlgCtrlID(hwndCtrl) == 200) {
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(0, 51, 153));
            return (INT_PTR)GetStockObject(NULL_BRUSH);
        }
        
        // Per tutti gli altri controlli statici (label "Security key:")
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0));
        return (INT_PTR)GetStockObject(NULL_BRUSH);
    }
    case WM_CTLCOLORBTN: {
        HDC hdc = (HDC)wParam;
        HWND hwndBtn = (HWND)lParam;
        
        // Per la checkbox "Hide characters" - gestione aggiuntiva per i pulsanti
        if (GetDlgCtrlID(hwndBtn) == 102) {
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(0, 0, 0));
            return (INT_PTR)GetStockObject(HOLLOW_BRUSH);
        }
        
        // Per i pulsanti OK e Cancel - sfondo standard
        return (INT_PTR)DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    
    case WM_COMMAND: {
        if (LOWORD(wParam) == 102) {
            HWND hEdit = GetDlgItem(hwnd, 101);
            BOOL checked = SendMessageW((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED;
            SendMessageW(hEdit, EM_SETPASSWORDCHAR, checked ? 0 : L'*', 0);
            InvalidateRect(hEdit, NULL, TRUE);
            return 0;
        }
        if (LOWORD(wParam) == IDOK) {
            if (data) { 
                GetDlgItemTextW(hwnd, 101, data->passwordBuffer, data->bufferSize); 
                // Trim whitespace
                WCHAR* p = data->passwordBuffer;
                while (*p == L' ' || *p == L'\t') p++;
                if (*p == L'\0') {
                    // Empty password - show warning and don't close
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
        DestroyWindow(hwnd); return 0;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}
BOOL PromptNetworkPassword(HWND hParent, WCHAR* passwordBuffer, DWORD bufferSize) {
    if (!SafeToAccessUI()) return FALSE;
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
    
    if (!hDlg) return FALSE;
    ShowWindow(hDlg, SW_SHOW);
    EnableWindow(hParent, FALSE);
    
    MSG msg;
    while (IsWindow(hDlg) && GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    
    EnableWindow(hParent, TRUE);
    SetForegroundWindow(hParent);
    return data.confirmed;
}

// Thread proc per connessione asincrona
static unsigned int __stdcall AsyncConnectThreadProc(void* pParam) {
    AsyncConnectContext* ctx = (AsyncConnectContext*)pParam;
    if (!ctx) return 1;
    
    // Ottieni il mutex per prevenire operazioni concorrenti
    DWORD waitResult = WaitForSingleObject(g_hConnectMutex, 10000);
    if (waitResult != WAIT_OBJECT_0) {
        Wh_Log(L"AsyncConnectThreadProc: Could not acquire mutex (timeout or error)");
        if (ctx->hWndNotify) {
            PostMessageW(ctx->hWndNotify, WM_ASYNC_CONNECT_COMPLETE, 0, (LPARAM)ERROR_TIMEOUT);
        }
        free(ctx);
        return 1;
    }
    
    // Salva il profilo se necessario
    if (ctx->isSecured && !ctx->hasProfile) {
        // Dichiarazione delle variabili necessarie
        WCHAR xmlProfile[2048] = {0};
        WCHAR escapedSsid[256] = {0};
        WCHAR escapedPwd[256] = {0};
        DWORD dwReason = 0;
        
        // Escape dei caratteri XML nell'SSID
        for (size_t i = 0, d = 0; ctx->ssid[i] && d < 255; i++) {
            switch (ctx->ssid[i]) {
                case L'&': StringCchCopyW(escapedSsid + d, 256 - d, L"&amp;"); d += 5; break;
                case L'<': StringCchCopyW(escapedSsid + d, 256 - d, L"&lt;"); d += 4; break;
                case L'>': StringCchCopyW(escapedSsid + d, 256 - d, L"&gt;"); d += 4; break;
                case L'"': StringCchCopyW(escapedSsid + d, 256 - d, L"&quot;"); d += 6; break;
                case L'\'': StringCchCopyW(escapedSsid + d, 256 - d, L"&apos;"); d += 6; break;
                default: escapedSsid[d++] = ctx->ssid[i]; break;
            }
        }
        
        // Escape dei caratteri XML nella password
        for (size_t i = 0, d = 0; ctx->password[i] && d < 255; i++) {
            switch (ctx->password[i]) {
                case L'&': StringCchCopyW(escapedPwd + d, 256 - d, L"&amp;"); d += 5; break;
                case L'<': StringCchCopyW(escapedPwd + d, 256 - d, L"&lt;"); d += 4; break;
                case L'>': StringCchCopyW(escapedPwd + d, 256 - d, L"&gt;"); d += 4; break;
                case L'"': StringCchCopyW(escapedPwd + d, 256 - d, L"&quot;"); d += 6; break;
                case L'\'': StringCchCopyW(escapedPwd + d, 256 - d, L"&apos;"); d += 6; break;
                default: escapedPwd[d++] = ctx->password[i]; break;
            }
        }
        
        // Crea il profilo XML
        StringCchPrintfW(xmlProfile, ARRAYSIZE(xmlProfile),
            L"<?xml version=\"1.0\"?>"
            L"<WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\">"
            L"<name>%s</name>"
            L"<SSIDConfig><SSID><name>%s</name></SSID></SSIDConfig>"
            L"<connectionType>ESS</connectionType>"
            L"<connectionMode>auto</connectionMode>"
            L"<MSM><security>"
            L"<authEncryption>"
            L"<authentication>WPA2PSK</authentication>"
            L"<encryption>AES</encryption>"
            L"<useOneX>false</useOneX>"
            L"</authEncryption>"
            L"<sharedKey>"
            L"<keyType>passPhrase</keyType>"
            L"<protected>false</protected>"
            L"<keyMaterial>%s</keyMaterial>"
            L"</sharedKey>"
            L"</security></MSM>"
            L"</WLANProfile>",
            escapedSsid, escapedSsid, escapedPwd);
        
        DWORD setProfileResult = WlanSetProfile(g_Ctx.hWlanClient, &ctx->interfaceGuid, 
            0, xmlProfile, NULL, TRUE, NULL, &dwReason);
        
        Wh_Log(L"WlanSetProfile for %s returned: %lu, reason: %lu", ctx->ssid, setProfileResult, dwReason);
        
        if (setProfileResult != ERROR_SUCCESS) {
            Wh_Log(L"WlanSetProfile failed: %lu", setProfileResult);
            if (ctx->hWndNotify) {
                PostMessageW(ctx->hWndNotify, WM_ASYNC_CONNECT_COMPLETE, 0, (LPARAM)setProfileResult);
            }
            ReleaseMutex(g_hConnectMutex);
            free(ctx);
            return 1;
        }
        Wh_Log(L"Profile saved successfully for %s", ctx->ssid);
    }
    
    // Esegui la connessione
    WLAN_CONNECTION_PARAMETERS params;
    ZeroMemory(&params, sizeof(params));
    params.wlanConnectionMode = (ctx->hasProfile || ctx->isSecured) ? 
        wlan_connection_mode_profile : wlan_connection_mode_discovery_unsecure;
    params.strProfile = ctx->ssid;
    params.dot11BssType = ctx->dot11BssType;
    params.dwFlags = 0;
    
    DWORD res = WlanConnect(g_Ctx.hWlanClient, &ctx->interfaceGuid, &params, NULL);
    Wh_Log(L"WlanConnect for %s returned: %lu (0x%08X)", ctx->ssid, res, res);
    
    if (ctx->hWndNotify) {
        if (res == ERROR_SUCCESS) {
            // IMPORTANTE: WlanConnect restituisce ERROR_SUCCESS anche quando
            // la connessione non è ancora stabilita. Significa solo che la
            // richiesta è stata accettata. Il vero risultato arriverà via
            // notifica WLAN (WlanNotificationCallback).
            // NON inviare WM_ASYNC_CONNECT_COMPLETE con success=1 qui!
            Wh_Log(L"WlanConnect request accepted for %s - waiting for WLAN notification", ctx->ssid);
        } else {
            // Solo se WlanConnect fallisce immediatamente, notifica errore
            Wh_Log(L"WlanConnect immediate failure for %s: %lu", ctx->ssid, res);
            PostMessageW(ctx->hWndNotify, WM_ASYNC_CONNECT_COMPLETE, 0, (LPARAM)res);
        }
    }
    
    ReleaseMutex(g_hConnectMutex);
    free(ctx);
    return 0;
}
// Funzione che chiede la password e avvia la connessione asincrona
static BOOL AskForPasswordAndConnect(int index) {
    if (index < 0 || index >= g_NetworkCount || !g_Ctx.hWlanClient) return FALSE;
    
    WifiNetworkItem* item = &g_NetworkList[index];
    
    // Se la rete è protetta e non ha un profilo, chiedi SEMPRE la password
    if (item->isSecured && !item->hasProfile) {
        WCHAR password[65] = {0};
        
        if (!PromptNetworkPassword(g_hWndFlyout, password, ARRAYSIZE(password) - 1)) {
            // L'utente ha annullato
            Wh_Log(L"User cancelled password for %s", item->ssid);
            return FALSE;
        }
        
        // Verifica che la password non sia vuota
        BOOL isEmpty = TRUE;
        for (int i = 0; i < 64 && password[i]; i++) {
            if (password[i] != L' ' && password[i] != L'\t') {
                isEmpty = FALSE;
                break;
            }
        }
        
        if (isEmpty) {
            Wh_Log(L"Empty password provided for %s", item->ssid);
            MessageBoxW(g_hWndFlyout, LOC(STR_PWD_EMPTY), LOC(STR_ERROR_TITLE), MB_OK | MB_ICONWARNING);
            return FALSE;
        }
        
        // Crea contesto per connessione asincrona
        AsyncConnectContext* ctx = (AsyncConnectContext*)calloc(1, sizeof(AsyncConnectContext));
        if (!ctx) return FALSE;
        
        ctx->hWndNotify = g_hWndFlyout;
        ctx->interfaceGuid = item->interfaceGuid;
        ctx->dot11BssType = item->dot11BssType;
        ctx->hasProfile = FALSE;
        StringCchCopyW(ctx->ssid, 33, item->ssid);
        StringCchCopyW(ctx->password, 65, password);
        
        // Imposta lo stato
        item->connState = CONN_STATE_CONNECTING;
        item->operationStartTime = GetTickCount();
        g_PendingConnectIndex = index;
        
        if (!g_TimeoutTimer && g_hWndFlyout) {
            g_TimeoutTimer = SetTimer(g_hWndFlyout, 1002, 5000, NULL);
        }
        
        UpdateLayoutGeometry();
        if (g_hWndFlyout) InvalidateRect(g_hWndFlyout, NULL, TRUE);
        
        // Avvia thread asincrono
        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, AsyncConnectThreadProc, ctx, 0, NULL);
        if (!hThread) {
            Wh_Log(L"Failed to create async connect thread");
            item->connState = CONN_STATE_ERROR;
            g_PendingConnectIndex = -1;
            free(ctx);
            return FALSE;
        }
        CloseHandle(hThread); // Il thread si libera da solo
        
        return TRUE;
    }
    
    // Rete non protetta o ha già un profilo
    item->connState = CONN_STATE_CONNECTING;
    item->operationStartTime = GetTickCount();
    g_PendingConnectIndex = index;
    
    if (!g_TimeoutTimer && g_hWndFlyout) {
        g_TimeoutTimer = SetTimer(g_hWndFlyout, 1002, 5000, NULL);
    }
    
    UpdateLayoutGeometry();
    if (g_hWndFlyout) InvalidateRect(g_hWndFlyout, NULL, TRUE);
    
    // Avvia connessione asincrona anche per reti aperte
    AsyncConnectContext* ctx = (AsyncConnectContext*)calloc(1, sizeof(AsyncConnectContext));
    if (!ctx) return FALSE;
    
    ctx->hWndNotify = g_hWndFlyout;
    ctx->interfaceGuid = item->interfaceGuid;
    ctx->dot11BssType = item->dot11BssType;
    ctx->hasProfile = item->hasProfile;
    ctx->isSecured = item->isSecured;
    StringCchCopyW(ctx->ssid, 33, item->ssid);
    // Password vuota per reti aperte
    ctx->password[0] = L'\0';
    
    HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, AsyncConnectThreadProc, ctx, 0, NULL);
    if (!hThread) {
        Wh_Log(L"Failed to create async connect thread for open network");
        item->connState = CONN_STATE_ERROR;
        g_PendingConnectIndex = -1;
        free(ctx);
        return FALSE;
    }
    CloseHandle(hThread);
    
    return TRUE;
}

void ConnectToNetwork(int index) {
    if (index < 0 || index >= g_NetworkCount || !g_Ctx.hWlanClient) return;
    
    WifiNetworkItem* item = &g_NetworkList[index];
    
    // If already connected, disconnect instead (Windows 7 behavior)
    if (item->connState == CONN_STATE_CONNECTED) {
        DisconnectFromNetwork(index);
        return;
    }
    
    // If already connecting, ignore
    if (item->connState == CONN_STATE_CONNECTING) {
        Wh_Log(L"Already connecting to %s, ignoring", item->ssid);
        return;
    }
    
    // Reset error state
    if (item->connState == CONN_STATE_ERROR) {
        item->connState = CONN_STATE_IDLE;
    }
    
    // Usa la funzione che gestisce correttamente la richiesta password
    AskForPasswordAndConnect(index);
}

void DisconnectFromNetwork(int index) {
    if (index < 0 || index >= g_NetworkCount || !g_Ctx.hWlanClient) return;
    
    WifiNetworkItem* item = &g_NetworkList[index];
    if (item->connState != CONN_STATE_CONNECTED) return;
    
    item->connState = CONN_STATE_DISCONNECTING;
    item->operationStartTime = GetTickCount();
    
    // Start timeout timer if not running
    if (!g_TimeoutTimer && g_hWndFlyout) {
        g_TimeoutTimer = SetTimer(g_hWndFlyout, 1002, 5000, NULL);
    }
    
    UpdateLayoutGeometry();
    if (g_hWndFlyout) InvalidateRect(g_hWndFlyout, NULL, TRUE);
    
    // Esegui la disconnessione in modo asincrono
    DWORD res = WlanDisconnect(g_Ctx.hWlanClient, &item->interfaceGuid, NULL);
    if (res != ERROR_SUCCESS) {
        Wh_Log(L"WlanDisconnect failed: %lu", res);
        item->connState = CONN_STATE_ERROR;
        UpdateLayoutGeometry();
        if (g_hWndFlyout) InvalidateRect(g_hWndFlyout, NULL, TRUE);
    }
}

void CheckConnectionTimeouts() {
    if (!g_Ctx.hWlanClient) return;
    
    DWORD now = GetTickCount();
    BOOL anyPending = FALSE;
    BOOL needsRefresh = FALSE;
    
    for (int i = 0; i < g_NetworkCount; i++) {
        if ((g_NetworkList[i].connState == CONN_STATE_CONNECTING ||
             g_NetworkList[i].connState == CONN_STATE_DISCONNECTING) &&
            g_NetworkList[i].operationStartTime > 0) {
            
            if (now - g_NetworkList[i].operationStartTime > CONNECTION_TIMEOUT_MS) {
                // Timeout!
                Wh_Log(L"Connection timeout for %s", g_NetworkList[i].ssid);
                g_NetworkList[i].connState = CONN_STATE_ERROR;
                g_NetworkList[i].operationStartTime = 0;
                g_PendingConnectIndex = -1;
                needsRefresh = TRUE;
                
                if (g_hWndFlyout && IsWindow(g_hWndFlyout)) {
                    PostMessageW(g_hWndFlyout, WM_CONNECTION_TIMEOUT, 0, 0);
                }
            } else {
                anyPending = TRUE;
            }
        }
    }
    
    // Stop timeout timer if no pending operations
    if (!anyPending && g_TimeoutTimer && g_hWndFlyout) {
        KillTimer(g_hWndFlyout, g_TimeoutTimer);
        g_TimeoutTimer = 0;
    }
    
    if (needsRefresh && g_hWndFlyout) {
        PostMessageW(g_hWndFlyout, WM_REFRESH_DATA, 0, 0);
    }
}

// -------------------------------------------------------
// WLAN Notification Callback
// -------------------------------------------------------
void WINAPI WlanNotificationCallback(PWLAN_NOTIFICATION_DATA data, PVOID context) {
    ModContext* ctx = (ModContext*)context;
    if (!ctx || ctx->isUninitializing || !data) return;
    
    switch(data->NotificationSource) {
        case WLAN_NOTIFICATION_SOURCE_ACM:
            if (data->NotificationCode == wlan_notification_acm_connection_complete) {
                if (data->pData && data->dwDataSize >= sizeof(WLAN_CONNECTION_NOTIFICATION_DATA)) {
                    PWLAN_CONNECTION_NOTIFICATION_DATA pConnData = 
                        (PWLAN_CONNECTION_NOTIFICATION_DATA)data->pData;
                    
                    Wh_Log(L"WLAN connection complete: reason=%lu (0x%08X)", 
                           pConnData->wlanReasonCode, pConnData->wlanReasonCode);
                    
                    if (pConnData->wlanReasonCode == ERROR_SUCCESS) {
                        Wh_Log(L"Connection SUCCESS");
                        // ... gestione successo ...
                    } else {
                        Wh_Log(L"Connection FAILED: reason=%lu", pConnData->wlanReasonCode);
                        
                        // Mappa codici errore comuni
                        switch(pConnData->wlanReasonCode) {
                            case 0x00038001: // Invalid profile
                                Wh_Log(L"Error: Invalid profile - will clear saved profile");
                                break;
                            case 0x00048005: // Security missing
                                Wh_Log(L"Error: Security credentials missing");
                                break;
                            case ERROR_INVALID_PASSWORD:
                                Wh_Log(L"Error: Invalid password");
                                break;
                        }
                        // ... gestione errore ...
                    }
                }
            }
            break;
    }
}
// -------------------------------------------------------
// Signal icon drawing
// -------------------------------------------------------
void DrawNativeSignalIcon(HDC hdc, int right, int top, ULONG quality) {
    int idx = 0;
    if      (quality > 80) idx = 5;
    else if (quality > 60) idx = 4;
    else if (quality > 40) idx = 3;
    else if (quality > 20) idx = 2;
    else if (quality > 0)  idx = 1;
    if (g_hIconSignalBars[idx])
        DrawIconEx(hdc, right-24, top+4, g_hIconSignalBars[idx], 16, 16, 0, NULL, DI_NORMAL);
}

// -------------------------------------------------------
// Tooltip
// -------------------------------------------------------
void InitTooltip(HWND hwnd) {
    if (g_hTooltip) return;
    g_hTooltip = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        hwnd, NULL, GetModuleHandle(NULL), NULL);
    SendMessage(g_hTooltip, TTM_SETMAXTIPWIDTH,   0, 300);
    SendMessage(g_hTooltip, TTM_SETDELAYTIME, TTDT_AUTOPOP, 10000);
    SendMessage(g_hTooltip, TTM_SETDELAYTIME, TTDT_INITIAL, 400);
    SendMessage(g_hTooltip, TTM_SETDELAYTIME, TTDT_RESHOW,  200);
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

// -------------------------------------------------------
// Row layout helpers
// -------------------------------------------------------
BOOL GetRowRect(int index, RECT* rcRow) {
    if (index < 0 || index >= g_NetworkCount || !g_bListExpanded) return FALSE;
    int y = LIST_Y_START;
    for (int i = 0; i < index; i++)
        y += (i == g_SelectedRowIndex) ? ROW_HEIGHT_EXPANDED : ROW_HEIGHT_NORMAL;
    if (y >= LIST_Y_END) return FALSE;
    rcRow->left   = 10;
    rcRow->top    = y;
    rcRow->right  = WINDOW_WIDTH - 10;
    rcRow->bottom = (int)fmin(y + ((index == g_SelectedRowIndex) ? ROW_HEIGHT_EXPANDED : ROW_HEIGHT_NORMAL), LIST_Y_END);
    return TRUE;
}

int HitTestRows(int x, int y) {
    for (int i = 0; i < g_NetworkCount; i++) {
        RECT rc;
        if (GetRowRect(i, &rc) && x>=rc.left && x<=rc.right && y>=rc.top && y<=rc.bottom) return i;
    }
    return -1;
}

void RecalcArrowRect() {
    int labelMidY = WIFI_LABEL_Y + (HEADER_HEIGHT - WIFI_LABEL_Y) / 2;
    int btnH=16, btnW=22;
    int margineDestroFreccia = 24;
    g_rcArrowButton.right  = WINDOW_WIDTH - margineDestroFreccia;
    g_rcArrowButton.left   = g_rcArrowButton.right - btnW;
    g_rcArrowButton.top    = labelMidY - btnH/2;
    g_rcArrowButton.bottom = labelMidY + btnH/2;
}

void UpdateLayoutGeometry() {
    if (!SafeToAccessUI()) return;
    if (g_SelectedRowIndex == -1) {
        if (g_hWndButtonConnect && IsWindow(g_hWndButtonConnect))   
            ShowWindow(g_hWndButtonConnect, SW_HIDE);
        if (g_hWndCheckboxConnect && IsWindow(g_hWndCheckboxConnect)) 
            ShowWindow(g_hWndCheckboxConnect, SW_HIDE);
        return;
    }
    RECT rcRow;
    if (!GetRowRect(g_SelectedRowIndex, &rcRow)) {
        if (g_hWndButtonConnect && IsWindow(g_hWndButtonConnect))   
            ShowWindow(g_hWndButtonConnect, SW_HIDE);
        if (g_hWndCheckboxConnect && IsWindow(g_hWndCheckboxConnect)) 
            ShowWindow(g_hWndCheckboxConnect, SW_HIDE);
        return;
    }
    
    WifiNetworkItem* item = &g_NetworkList[g_SelectedRowIndex];
    
    // Show connect button and checkbox for non-connected networks
    if (item->connState == CONN_STATE_IDLE || item->connState == CONN_STATE_ERROR) {
        if (g_hWndCheckboxConnect && IsWindow(g_hWndCheckboxConnect)) {
            MoveWindow(g_hWndCheckboxConnect, rcRow.left+8, rcRow.top+36, 160, 20, TRUE);
            SetWindowTextW(g_hWndCheckboxConnect, LOC(STR_CHK_CONNECT_AUTO));
            ShowWindow(g_hWndCheckboxConnect, SW_SHOW);
        }
        if (g_hWndButtonConnect && IsWindow(g_hWndButtonConnect)) {
            MoveWindow(g_hWndButtonConnect, rcRow.right-90, rcRow.top+35, 82, 22, TRUE);
            SetWindowTextW(g_hWndButtonConnect, LOC(STR_BTN_CONNECT));
            ShowWindow(g_hWndButtonConnect, SW_SHOW);
            EnableWindow(g_hWndButtonConnect, TRUE);
        }
    }
    // Show connecting state
    else if (item->connState == CONN_STATE_CONNECTING) {
        if (g_hWndCheckboxConnect && IsWindow(g_hWndCheckboxConnect)) 
            ShowWindow(g_hWndCheckboxConnect, SW_HIDE);
        if (g_hWndButtonConnect && IsWindow(g_hWndButtonConnect)) {
            MoveWindow(g_hWndButtonConnect, rcRow.right-100, rcRow.top+35, 100, 22, TRUE);
            SetWindowTextW(g_hWndButtonConnect, LOC(STR_CONNECTING));
            ShowWindow(g_hWndButtonConnect, SW_SHOW);
            EnableWindow(g_hWndButtonConnect, FALSE);
        }
    }
    // Show disconnect for connected networks
    else {
        if (g_hWndCheckboxConnect && IsWindow(g_hWndCheckboxConnect)) 
            ShowWindow(g_hWndCheckboxConnect, SW_HIDE);
        if (g_hWndButtonConnect && IsWindow(g_hWndButtonConnect)) {
            MoveWindow(g_hWndButtonConnect, rcRow.right-90, rcRow.top+35, 82, 22, TRUE);
            SetWindowTextW(g_hWndButtonConnect, LOC(STR_BTN_DISCONNECT));
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
    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMenu, MF_STRING, IDM_PROPERTIES, LOC(STR_CTX_PROPERTIES));
    
    int cmd = TrackPopupMenu(hMenu, TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, pt.x, pt.y, 0, hwnd, NULL);
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
            WS_CHILD|BS_PUSHBUTTON, 0,0,0,0, hwnd,(HMENU)IDC_CONN_BUTTON,GetModuleHandle(NULL),NULL);
        SendMessageW(g_hWndButtonConnect, WM_SETFONT, (WPARAM)g_hFontButton, TRUE);
        
        g_hWndCheckboxConnect = CreateWindowExW(0, WC_BUTTONW, L"",
            WS_CHILD|BS_AUTOCHECKBOX, 0,0,0,0, hwnd,(HMENU)IDC_AUTO_CHECKBOX,GetModuleHandle(NULL),NULL);
        SendMessageW(g_hWndCheckboxConnect, WM_SETFONT, (WPARAM)g_hFontCheckbox, TRUE);
        SendMessageW(g_hWndCheckboxConnect, BM_SETCHECK, BST_CHECKED, 0);
        
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
                UpdateLayoutGeometry();
                InvalidateRect(hwnd, NULL, TRUE);
            }
        } else if (wParam == 1002) {
            CheckConnectionTimeouts();
            UpdateLayoutGeometry();
            InvalidateRect(hwnd, NULL, TRUE);
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
    case WM_ASYNC_CONNECT_COMPLETE: {
        BOOL success = (BOOL)wParam;
        DWORD reason = (DWORD)lParam;
        
        Wh_Log(L"WM_ASYNC_CONNECT_COMPLETE: success=%d, reason=%lu", success, reason);
        
        if (success) {
            if (g_PendingConnectIndex >= 0 && g_PendingConnectIndex < g_NetworkCount) {
                g_NetworkList[g_PendingConnectIndex].connState = CONN_STATE_CONNECTED;
                g_NetworkList[g_PendingConnectIndex].operationStartTime = 0;
                g_NetworkList[g_PendingConnectIndex].hasProfile = TRUE;
                g_PendingConnectIndex = -1;
            }
            RefreshWifiData(g_Ctx.hWlanClient);
        } else {
            if (g_PendingConnectIndex >= 0 && g_PendingConnectIndex < g_NetworkCount) {
                if (reason == ERROR_INVALID_PASSWORD || reason == 0x00040025) {
                    g_NetworkList[g_PendingConnectIndex].hasProfile = FALSE;
                    g_NetworkList[g_PendingConnectIndex].connState = CONN_STATE_ERROR;
                    g_NetworkList[g_PendingConnectIndex].operationStartTime = 0;
                    MessageBoxW(hwnd, LOC(STR_PWD_FAILED_WRONG), LOC(STR_PWD_FAILED_TITLE), MB_OK | MB_ICONERROR);
                } else {
                    g_NetworkList[g_PendingConnectIndex].connState = CONN_STATE_ERROR;
                    g_NetworkList[g_PendingConnectIndex].operationStartTime = 0;
                    WCHAR errMsg[256];
                    StringCchPrintfW(errMsg, 256, LOC(STR_CONNECTION_ERROR), reason);
                    MessageBoxW(hwnd, errMsg, LOC(STR_ERROR_TITLE), MB_OK | MB_ICONERROR);
                }
                g_PendingConnectIndex = -1;
            }
            RefreshWifiData(g_Ctx.hWlanClient);
        }
        if (g_TimeoutTimer) {
            KillTimer(hwnd, g_TimeoutTimer);
            g_TimeoutTimer = 0;
        }
        UpdateLayoutGeometry();
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }
    case WM_CONNECTION_RESULT: {
        BOOL success = (BOOL)wParam;
        DWORD reason = (DWORD)lParam;
        
        if (success) {
            RefreshWifiData(g_Ctx.hWlanClient);
        } else {
            if (reason == ERROR_INVALID_PASSWORD || reason == 0x00040025) {
                MessageBoxW(hwnd, LOC(STR_PWD_FAILED_WRONG), LOC(STR_PWD_FAILED_TITLE), MB_OK | MB_ICONERROR);
            } else {
                WCHAR errMsg[256];
                StringCchPrintfW(errMsg, 256, LOC(STR_CONNECTION_ERROR), reason);
                MessageBoxW(hwnd, errMsg, LOC(STR_ERROR_TITLE), MB_OK | MB_ICONERROR);
            }
            RefreshWifiData(g_Ctx.hWlanClient);
        }
        UpdateLayoutGeometry();
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }
    case WM_CONNECTION_TIMEOUT: {
        MessageBoxW(hwnd, LOC(STR_CONNECTION_TIMEOUT_MSG), LOC(STR_TIMEOUT_ERROR), MB_OK | MB_ICONERROR);
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
    case WM_PAINT: {
        if (!SafeToAccessUI()) break;
        PAINTSTRUCT ps;
        HDC hdcReal = BeginPaint(hwnd, &ps);
        HDC     hdc     = CreateCompatibleDC(hdcReal);
        HBITMAP hBmp    = CreateCompatibleBitmap(hdcReal, WINDOW_WIDTH, WINDOW_HEIGHT);
        HBITMAP hOldBmp = (HBITMAP)SelectObject(hdc, hBmp);

        RECT rcHeader  = {0, 0, WINDOW_WIDTH, HEADER_HEIGHT};
        HBRUSH hBrH = CreateSolidBrush(RGB(235,244,253)); FillRect(hdc, &rcHeader, hBrH); DeleteObject(hBrH);
        RECT rcContent = {0, HEADER_HEIGHT, WINDOW_WIDTH, LIST_Y_END};
        HBRUSH hBrC = CreateSolidBrush(RGB(255,255,255)); FillRect(hdc, &rcContent, hBrC); DeleteObject(hBrC);
        RECT rcFooter = GetFooterRect();
        HBRUSH hBrF = CreateSolidBrush(RGB(225,230,242));
        FillRect(hdc, &rcFooter, hBrF); DeleteObject(hBrF);
        
        HPEN hPenSep = CreatePen(PS_SOLID, 1, RGB(214,223,234));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPenSep);
        MoveToEx(hdc, 0, HEADER_HEIGHT, NULL); LineTo(hdc, WINDOW_WIDTH, HEADER_HEIGHT);
        SelectObject(hdc, hOldPen); DeleteObject(hPenSep);

        HPEN hPenBevelDark  = CreatePen(PS_SOLID, 1, RGB(180,193,210));
        HPEN hPenBevelLight = CreatePen(PS_SOLID, 1, RGB(255,255,255));

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
            SelectObject(hdc, g_hFontNormal); SetTextColor(hdc, RGB(0,0,0));
            TextOutW(hdc, 56, 18, LOC(STR_CURRENT_CONNECTED), lstrlenW(LOC(STR_CURRENT_CONNECTED)));
            SelectObject(hdc, g_hFontBold);
            WCHAR displaySsid[33]; GetDisplaySSID(0, displaySsid, 33);
            TextOutW(hdc, 56, 34, displaySsid, lstrlenW(displaySsid));
            SelectObject(hdc, g_hFontNormal); SetTextColor(hdc, RGB(110,110,110));
            TextOutW(hdc, 56, 50, LOC(STR_INTERNET_ACCESS), lstrlenW(LOC(STR_INTERNET_ACCESS)));
        } else {
            SelectObject(hdc, g_hFontBold); SetTextColor(hdc, RGB(0,0,0));
            TextOutW(hdc, 56, 22, LOC(STR_NO_CONNECTIONS), lstrlenW(LOC(STR_NO_CONNECTIONS)));
            SelectObject(hdc, g_hFontNormal);
            TextOutW(hdc, 56, 40, LOC(STR_CONNECTIONS_AVAILABLE), lstrlenW(LOC(STR_CONNECTIONS_AVAILABLE)));
        }
                HICON hLargeIcon = isAnyConnected ? g_hIconNetworkMap : g_hIconSignalBars[0];
        if (hLargeIcon) DrawIconEx(hdc, 14, 20, hLargeIcon, 32, 32, 0, NULL, DI_NORMAL);

        // Hover con effetto azzurro/blu
        if (g_IsHoveringRefresh) {
            RECT rcBtn = g_rcRefreshButton;
            
            // Sfondo base azzurro chiarissimo
            HBRUSH hBrBg = CreateSolidBrush(RGB(220, 238, 252));
            FillRect(hdc, &rcBtn, hBrBg);
            DeleteObject(hBrBg);
            
            // Bordo azzurro esterno
            HPEN hPenOuter = CreatePen(PS_SOLID, 1, RGB(174, 212, 243));
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPenOuter);
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
            RoundRect(hdc, rcBtn.left, rcBtn.top, rcBtn.right, rcBtn.bottom, 4, 4);
            
            // Linea bianca interna
            RECT rcInner = rcBtn;
            rcInner.left += 1; rcInner.top += 1; 
            rcInner.right -= 1; rcInner.bottom -= 1;
            HPEN hPenInner = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
            SelectObject(hdc, hPenInner);
            RoundRect(hdc, rcInner.left, rcInner.top, rcInner.right, rcInner.bottom, 3, 3);
            
            SelectObject(hdc, hOldPen);
            SelectObject(hdc, hOldBrush);
            DeleteObject(hPenOuter);
            DeleteObject(hPenInner);
        }
        
        // Icona normale base64
        if (!g_hIconRefreshNormal) g_hIconRefreshNormal = CreateIconFromBase64PNG(REFRESH_ICON_NORMAL_BASE64);
        if (g_hIconRefreshNormal)
            DrawIconEx(hdc, g_rcRefreshButton.left+2, g_rcRefreshButton.top+3,
                       g_hIconRefreshNormal, 0, 0, 0, NULL, DI_NORMAL);

        SelectObject(hdc, g_hFontNormal); SetTextColor(hdc, RGB(90,100,110));
        TextOutW(hdc, 14, HEADER_HEIGHT - 24, LOC(STR_WIFI_HEADER), lstrlenW(LOC(STR_WIFI_HEADER)));
        
        if (g_IsHoveringArrow) {
            HBRUSH hBrA  = CreateSolidBrush(RGB(230,240,255));
            HPEN   hPenA = CreatePen(PS_SOLID, 1, RGB(180,210,245));
            HPEN   hOldPA = (HPEN)SelectObject(hdc, hPenA);
            HBRUSH hOldBA = (HBRUSH)SelectObject(hdc, hBrA);
            RoundRect(hdc, g_rcArrowButton.left, g_rcArrowButton.top,
                      g_rcArrowButton.right, g_rcArrowButton.bottom, 2, 2);
            SelectObject(hdc, hOldPA); SelectObject(hdc, hOldBA);
            DeleteObject(hBrA); DeleteObject(hPenA);
        }
        SelectObject(hdc, g_hFontArrow); SetTextColor(hdc, RGB(50,50,50));
        LPCWSTR arrowChar = g_bListExpanded ? L"6" : L"5";
        RECT rcArrowText = g_rcArrowButton; rcArrowText.top += 2;
        DrawTextW(hdc, arrowChar, 1, &rcArrowText, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        if (g_bListExpanded) {
            for (int i = 0; i < g_NetworkCount; i++) {
                RECT rcRow;
                if (!GetRowRect(i, &rcRow)) continue;
                BOOL isSelected = (i == g_SelectedRowIndex);
                BOOL isHovered  = (i == g_HoveredRowIndex);
                BOOL hasKeyboardFocus = (i == g_KeyboardSelectedIndex);
                
                if (isSelected || isHovered) {
                    RECT rcFullRow = rcRow; rcFullRow.left = 0; rcFullRow.right = WINDOW_WIDTH - 5;
                    HBRUSH hBrBg  = CreateSolidBrush(isSelected ? RGB(228,241,252) : RGB(242,247,253));
                    HPEN   hPenBg = CreatePen(PS_SOLID, 1, isSelected ? RGB(174,212,243) : RGB(216,231,248));
                    HPEN   hOldP  = (HPEN)SelectObject(hdc, hPenBg);
                    HBRUSH hOldB  = (HBRUSH)SelectObject(hdc, hBrBg);
                    RoundRect(hdc, rcFullRow.left, rcFullRow.top, rcFullRow.right, rcFullRow.bottom, 3, 3);
                    SelectObject(hdc, hOldP); SelectObject(hdc, hOldB);
                    DeleteObject(hBrBg); DeleteObject(hPenBg);
                }
                if (hasKeyboardFocus && !isSelected)
                    DrawFocusRectangle(hdc, &rcRow);
                
                WCHAR ssidBuf[33]; GetDisplaySSID(i, ssidBuf, 33);
                SelectObject(hdc, isSelected ? g_hFontBold : g_hFontNormal);
                SetTextColor(hdc, RGB(0,0,255));
                TextOutW(hdc, rcRow.left+10, rcRow.top+6, ssidBuf, lstrlenW(ssidBuf));
                
                WifiNetworkItem* item = &g_NetworkList[i];
                const WCHAR* statusText = NULL;
                switch (item->connState) {
                    case CONN_STATE_CONNECTED:    statusText = LOC(STR_CONNECTED_TEXT); break;
                    case CONN_STATE_CONNECTING:   statusText = LOC(STR_CONNECTING); break;
                    case CONN_STATE_DISCONNECTING: statusText = LOC(STR_DISCONNECTING); break;
                    default: break;
                }
                
                if (statusText) {
                    SelectObject(hdc, item->connState == CONN_STATE_CONNECTED ? g_hFontBold : g_hFontNormal);
                    SetTextColor(hdc, item->connState == CONN_STATE_CONNECTED ? RGB(0,0,0) : RGB(128,128,128));
                    int textX = isSelected ? (rcRow.left+10)   : (rcRow.right-110);
                    int textY = isSelected ? (rcRow.top+22)    : (rcRow.top+6);
                    TextOutW(hdc, textX, textY, statusText, lstrlenW(statusText));
                }
                
                DrawNativeSignalIcon(hdc, rcRow.right-10, rcRow.top+2, item->signalQuality);
            }
        }

        SelectObject(hdc, g_IsHoveringLink ? g_hFontUnderline : g_hFontNormal);
        SetTextColor(hdc, RGB(14,75,184));
        const wchar_t* footerText = LOC(STR_OPEN_SHARING_CENTER);
        SIZE textSize; GetTextExtentPoint32W(hdc, footerText, lstrlenW(footerText), &textSize);
        int centerX = (WINDOW_WIDTH - textSize.cx) / 2;
        int footerTextYC = (WINDOW_HEIGHT - FOOTER_HEIGHT) + ((FOOTER_HEIGHT - textSize.cy) / 2) - 5;
        TextOutW(hdc, centerX, footerTextYC, footerText, lstrlenW(footerText));

        BitBlt(hdcReal, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdc, 0, 0, SRCCOPY);
        SelectObject(hdc, hOldBmp); DeleteObject(hBmp); DeleteDC(hdc);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdc = (HDC)wParam;
        HWND hwndCtrl = (HWND)lParam;
        
        // Per la checkbox "Connect automatically" nel flyout principale
        if (hwndCtrl == g_hWndCheckboxConnect && g_SelectedRowIndex >= 0) {
            WifiNetworkItem* item = &g_NetworkList[g_SelectedRowIndex];
            
            // Applica sfondo azzurro solo per reti non connesse (IDLE o ERROR)
            if (item->connState == CONN_STATE_IDLE || item->connState == CONN_STATE_ERROR) {
                SetBkColor(hdc, RGB(228, 241, 252));
                SetBkMode(hdc, OPAQUE);
                SetTextColor(hdc, RGB(0, 0, 0));
                
                static HBRUSH hBrushCheckbox = NULL;
                if (!hBrushCheckbox) {
                    hBrushCheckbox = CreateSolidBrush(RGB(228, 241, 252));
                }
                return (INT_PTR)hBrushCheckbox;
            } else {
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, RGB(0, 0, 0));
                return (INT_PTR)GetStockObject(HOLLOW_BRUSH);
            }
        }
        
        // Per tutti gli altri controlli statici
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0));
        return (INT_PTR)GetStockObject(NULL_BRUSH);
    }
    case WM_CTLCOLORBTN: {
        HDC hdc = (HDC)wParam;
        HWND hwndBtn = (HWND)lParam;
        
        // Gestione aggiuntiva per la checkbox (è un BS_AUTOCHECKBOX, quindi è un button)
        if (hwndBtn == g_hWndCheckboxConnect && g_SelectedRowIndex >= 0) {
            WifiNetworkItem* item = &g_NetworkList[g_SelectedRowIndex];
            
            if (item->connState == CONN_STATE_IDLE || item->connState == CONN_STATE_ERROR) {
                SetBkColor(hdc, RGB(228, 241, 252));
                SetBkMode(hdc, OPAQUE);
                SetTextColor(hdc, RGB(0, 0, 0));
                
                static HBRUSH hBrushCheckboxBtn = NULL;
                if (!hBrushCheckboxBtn) {
                    hBrushCheckboxBtn = CreateSolidBrush(RGB(228, 241, 252));
                }
                return (INT_PTR)hBrushCheckboxBtn;
            } else {
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, RGB(0, 0, 0));
                return (INT_PTR)GetStockObject(HOLLOW_BRUSH);
            }
        }
        
        // Per i pulsanti normali (Connect/Disconnect)
        return (INT_PTR)DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    case WM_REFRESH_DATA: {
        if (g_Ctx.hWlanClient) {
            RefreshWifiData(g_Ctx.hWlanClient);
            UpdateLayoutGeometry();
            InvalidateRect(hwnd, NULL, TRUE);
        }
        break;
    }
    case WM_MOUSEMOVE: {
        int mx = LOWORD(lParam), my = HIWORD(lParam);
        POINT pt = {mx,my};
        RECT rcF = GetFooterRect();
        BOOL wasLink    = g_IsHoveringLink;
        BOOL wasRefresh = g_IsHoveringRefresh;
        BOOL wasArrow   = g_IsHoveringArrow;
        int  wasHov     = g_HoveredRowIndex;
        g_IsHoveringLink    = PtInRect(&rcF, pt) != 0;
        g_IsHoveringRefresh = PtInRect(&g_rcRefreshButton, pt) != 0;
        g_IsHoveringArrow   = PtInRect(&g_rcArrowButton,   pt) != 0;
        int newHovered = (my >= LIST_Y_START && my < LIST_Y_END) ? HitTestRows(mx,my) : -1;
        g_HoveredRowIndex = newHovered;
        if (newHovered != wasHov)
            UpdateTooltipForRow(hwnd, newHovered);
        SetCursor(LoadCursor(NULL, (g_IsHoveringLink || g_IsHoveringRefresh || g_IsHoveringArrow) ? IDC_HAND : IDC_ARROW));
        if (wasLink!=g_IsHoveringLink || wasRefresh!=g_IsHoveringRefresh ||
            wasArrow!=g_IsHoveringArrow || wasHov!=g_HoveredRowIndex) {
            InvalidateRect(hwnd,NULL,FALSE);
            TRACKMOUSEEVENT tme = {sizeof(TRACKMOUSEEVENT),TME_LEAVE,hwnd,0};
            TrackMouseEvent(&tme);
        }
        break;
    }
    case WM_MOUSELEAVE:
        g_IsHoveringLink = g_IsHoveringRefresh = g_IsHoveringArrow = FALSE;
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
            PostMessageW(hwnd,WM_REFRESH_DATA,0,0); break;
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
                }
                InvalidateRect(hwnd,NULL,FALSE);
            }
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
            ClearKeyboardFocus();
            ShowWindow(hwnd,SW_HIDE);
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
        if (g_hTooltip) { DestroyWindow(g_hTooltip); g_hTooltip = NULL; }
        g_hWndFlyout = g_hWndButtonConnect = g_hWndCheckboxConnect = NULL;
        break;
    }
    return DefWindowProcW(hwnd,uMsg,wParam,lParam);
}

// -------------------------------------------------------
// ToolbarWindow32 subclassing (unchanged)
// -------------------------------------------------------
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
                    if (tb.idCommand == TRAY_NETWORK_ID) {
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

void InstallTrayInterception() {
    if (!IsExplorerProcess()) return;
    HWND hTray = NULL;
    for (int attempt = 0; attempt < 10 && !hTray; attempt++) {
        hTray = FindWindowW(L"Shell_TrayWnd", NULL);
        if (!hTray) Sleep(500);
    }
    if (!hTray) return;
    HWND hNotify  = FindWindowExW(hTray,    NULL, L"TrayNotifyWnd",   NULL);
    HWND hSysPager= hNotify ? FindWindowExW(hNotify,  NULL, L"SysPager",        NULL) : NULL;
    HWND hToolbar = hSysPager ? FindWindowExW(hSysPager,NULL, L"ToolbarWindow32", NULL) : NULL;
    HWND hTarget = hToolbar ? hToolbar : (hNotify ? hNotify : hTray);
    if (!hTarget) return;
    G_hSubclassedToolbar = hTarget;
    WindhawkUtils::SetWindowSubclassFromAnyThread(hTarget, ToolbarWndProc, (DWORD_PTR)&G_SubclassId);
}

void RemoveTrayInterception() {
    if (G_hSubclassedToolbar) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(G_hSubclassedToolbar, ToolbarWndProc);
        G_SubclassId = 0;
        G_hSubclassedToolbar = nullptr;
    }
}

// -------------------------------------------------------
// Toggle flyout
// -------------------------------------------------------
void ToggleFlyoutWindow() {
    EnterCriticalSection(&g_Ctx.csLock);
    if (!g_Ctx.isUninitializing) {
        if (!g_hWndFlyout || !IsWindow(g_hWndFlyout)) {
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
            DWORD dwStyle = WS_POPUP;
            if (g_Settings.useRoundedCorners) dwStyle |= WS_THICKFRAME;
            AdjustWindowRectEx(&rcClient, dwStyle, FALSE, dwExStyle);
            g_hWndFlyout = CreateWindowExW(dwExStyle, wc.lpszClassName, L"", dwStyle,
                0, 0, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top,
                NULL, NULL, hInst, NULL);
        }
        if (IsWindowVisible(g_hWndFlyout)) {
            ClearKeyboardFocus();
            ShowWindow(g_hWndFlyout, SW_HIDE);
        } else {
            DetermineLocale();
            LoadSettings();
            g_SelectedRowIndex = g_HoveredRowIndex = -1;
            ClearKeyboardFocus();
            g_bListExpanded = TRUE;
            if (g_hWndButtonConnect && IsWindow(g_hWndButtonConnect))
                ShowWindow(g_hWndButtonConnect, SW_HIDE);
            if (g_hWndCheckboxConnect && IsWindow(g_hWndCheckboxConnect))
                ShowWindow(g_hWndCheckboxConnect, SW_HIDE);
            if (g_Ctx.hWlanClient) RefreshWifiData(g_Ctx.hWlanClient);
            UpdateLayoutGeometry();
            PositionWindowNearTray(g_hWndFlyout);
            ShowWindow(g_hWndFlyout, SW_SHOW);
            SetForegroundWindow(g_hWndFlyout);
            InvalidateRect(g_hWndFlyout,NULL,TRUE);
        }
    }
    LeaveCriticalSection(&g_Ctx.csLock);
}

// -------------------------------------------------------
// Hotkey thread (unchanged)
// -------------------------------------------------------
DWORD WINAPI HotkeyThreadProc(LPVOID lpParam) {
    ModContext* ctx = (ModContext*)lpParam;
    if (!ctx) return 1;
    
    auto UpdateHotkeyRegistration = [](BOOL shouldRegister) {
        UnregisterHotKey(NULL, HOTKEY_ID);
        if (shouldRegister) RegisterHotKey(NULL, HOTKEY_ID, MOD_CONTROL | MOD_NOREPEAT, 'H');
    };
    
    UpdateHotkeyRegistration(g_Settings.enableHotkey);
    UINT uTaskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
    MSG msg = {0};
    
    while (GetMessageW(&msg, NULL, 0, 0)) {
        if (msg.message == WM_HOTKEY && msg.wParam == HOTKEY_ID && !ctx->isUninitializing)
            ToggleFlyoutWindow();
        if (msg.message == WM_HOTKEY_SETTINGS_CHANGED)
            UpdateHotkeyRegistration(g_Settings.enableHotkey);
        if (msg.message == uTaskbarCreated && !ctx->isUninitializing) {
            if (G_hSubclassedToolbar) RemoveTrayInterception();
            Sleep(1000);
            InstallTrayInterception();
            UpdateHotkeyRegistration(g_Settings.enableHotkey);
        }
        TranslateMessage(&msg); DispatchMessageW(&msg);
    }
    UnregisterHotKey(NULL, HOTKEY_ID);
    return 0;
}

// -------------------------------------------------------
// Cleanup
// -------------------------------------------------------
void SafeCleanup() {
    if (InterlockedExchange(&g_Ctx.isUninitializing,1L)) return;
    RemoveTrayInterception();
    if (g_Ctx.dwHotkeyThreadId) PostThreadMessageW(g_Ctx.dwHotkeyThreadId,WM_QUIT,0,0);
    if (g_Ctx.hHotkeyThread) {
        WaitForSingleObject(g_Ctx.hHotkeyThread,3000);
        CloseHandle(g_Ctx.hHotkeyThread);
        g_Ctx.hHotkeyThread=NULL; g_Ctx.dwHotkeyThreadId=0;
    }
    if (g_hWndFlyout && IsWindow(g_hWndFlyout)) {
        SendMessageW(g_hWndFlyout,WM_SAFE_CLOSE,0,0);
        for (int i=0; i<50 && IsWindow(g_hWndFlyout); i++) {
            MSG msg;
            while (PeekMessageW(&msg,NULL,0,0,PM_REMOVE)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
        }
        if (IsWindow(g_hWndFlyout)) DestroyWindow(g_hWndFlyout);
    }
    if (g_hConnectMutex) { CloseHandle(g_hConnectMutex); g_hConnectMutex = NULL; }
    if (g_Ctx.hWlanClient) { WlanCloseHandle(g_Ctx.hWlanClient,NULL); g_Ctx.hWlanClient=NULL; }
    FreeSystemIcons();
    FreeGlobalFonts();
    g_hWndFlyout=g_hWndButtonConnect=g_hWndCheckboxConnect=NULL;
    g_Initialized=FALSE;
}

// -------------------------------------------------------
// Windhawk entry points
// -------------------------------------------------------
BOOL Wh_ModInit() {
    Wh_Log(L"=== Wh_ModInit v1.3.1 ===");
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        Wh_Log(L"CoInitializeEx failed: 0x%08X", hr);
    }
    DetectWindowsVersion();
    LoadSettings();
    ZeroMemory(&g_Ctx,sizeof(g_Ctx));
    InitializeCriticalSection(&g_Ctx.csLock);
    static DWORD lastInitTime = 0;
    DWORD currentTime = GetTickCount();

    if (lastInitTime > 0 && (currentTime - lastInitTime) < 2000) {
        Wh_Log(L"Wh_ModInit called too quickly after previous init (%lu ms) - ignoring", 
        currentTime - lastInitTime);
        return TRUE; 
    }
    lastInitTime = currentTime;
    g_hConnectMutex = CreateMutexW(NULL, FALSE, L"Local\\Win7NetFlyout_ConnectMutex");
    if (!g_hConnectMutex) {
        Wh_Log(L"Failed to create connect mutex");
    }
    
    DetermineLocale();
    
    HMODULE hAdvapi32 = GetModuleHandleW(L"advapi32.dll");
    if (hAdvapi32) {
        void* pRegQueryValueExW = (void*)GetProcAddress(hAdvapi32, "RegQueryValueExW");
        if (pRegQueryValueExW) {
            Wh_SetFunctionHook(pRegQueryValueExW, (void*)Hook_RegQueryValueExW, (void**)&Real_RegQueryValueExW);
        }
        void* pRegGetValueW = (void*)GetProcAddress(hAdvapi32, "RegGetValueW");
        if (pRegGetValueW) {
            Wh_SetFunctionHook(pRegGetValueW, (void*)Hook_RegGetValueW, (void**)&Real_RegGetValueW);
        }
    }
    
    if (!IsExplorerProcess()) {
        InstallTrayInterception();
        g_Initialized = TRUE;
        return TRUE;
    }
    
    InitGlobalFonts();
    LoadSystemIcons();
    InitRefreshButtonRect();
    RecalcArrowRect();

    if (g_Settings.redirectNetworkContextMenu) {
        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        if (hUser32) {
            void* pFn = (void*)GetProcAddress(hUser32, "TrackPopupMenuEx");
            if (pFn) {
                Wh_SetFunctionHook(pFn, (void*)TrackPopupMenuEx_Hook, (void**)&g_origTrackPopupMenuEx);
            }
        }
    }
    
    Sleep(INIT_DELAY_MS);
    InstallTrayInterception();
    
    DWORD dwMaxClient=2, dwCurVer=0;
    for (int wlanAttempt = 0; wlanAttempt < 10; wlanAttempt++) {
        DWORD wlanResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVer, &g_Ctx.hWlanClient);
        if (wlanResult == ERROR_SUCCESS) {
            WlanRegisterNotification(g_Ctx.hWlanClient, WLAN_NOTIFICATION_SOURCE_ALL, TRUE,
                                     WlanNotificationCallback, &g_Ctx, NULL, NULL);
            break;
        }
        Sleep(2000);
    }
    
    g_Ctx.hHotkeyThread = CreateThread(NULL,0,HotkeyThreadProc,&g_Ctx,0,&g_Ctx.dwHotkeyThreadId);
    if (!g_Ctx.hHotkeyThread) {
        if (g_Ctx.hWlanClient) { WlanCloseHandle(g_Ctx.hWlanClient,NULL); g_Ctx.hWlanClient=NULL; }
        if (g_hConnectMutex) { CloseHandle(g_hConnectMutex); g_hConnectMutex = NULL; }
        DeleteCriticalSection(&g_Ctx.csLock);
        CoUninitialize();
        return FALSE;
    }
    g_Initialized=TRUE;
    return TRUE;
}

void Wh_ModSettingsChanged() {
    s_settingsSavedOnce = true;
    BOOL oldRoundedCorners = g_Settings.useRoundedCorners;
    LoadSettings();
    DetermineLocale();
    
    if (oldRoundedCorners != g_Settings.useRoundedCorners) {
        if (g_hWndFlyout && IsWindow(g_hWndFlyout)) {
            BOOL wasVisible = IsWindowVisible(g_hWndFlyout);
            SendMessageW(g_hWndFlyout, WM_SAFE_CLOSE, 0, 0);
            if (wasVisible) ToggleFlyoutWindow();
        }
    }
    
    if (g_Ctx.dwHotkeyThreadId) {
        PostThreadMessageW(g_Ctx.dwHotkeyThreadId, WM_HOTKEY_SETTINGS_CHANGED, 0, 0);
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
    CoUninitialize();
}
