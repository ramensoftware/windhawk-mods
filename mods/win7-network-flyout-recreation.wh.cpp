// ==WindhawkMod==
// @id             win7-network-flyout-recreation
// @name           Windows 7 Network Flyout Recreation
// @description    This mod recreates the Windows 7 network flyout panel, replacing the modern Windows 10/11 flyout, along with the Windows 8 flyout as a configurable fallback
// @version        1.1.7
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

## Screenshot

![Windows 7 Network Flyout](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/network-flyout.PNG)

## Settings

| Setting | Description |
|---------|-------------|
| Language | Force language (Auto-detect, English, Italian) |
| Intercept native flyout | Replace Windows 10/11 network flyout with classic one |
| Privacy mode | Hide real network names |
| Use Registry ReplaceVan | Enable Windows 8 style flyout as fallback |
| Redirect network context menu | Redirect tray context menu to classic network connections |
| Refresh interval | Auto-refresh interval in ms (0 = disable) |

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
*/
// ==/WindhawkModSettings==

// This version is based on the 1.1.2 version.
#ifndef UNICODE
#define UNICODE
#endif
#include <windows.h>
#include <windowsx.h>       // GET_X_LPARAM, GET_Y_LPARAM
#include <wlanapi.h>        // WlanOpenHandle, WlanEnumInterfaces, ecc.
#include <objbase.h>        // CoInitialize, CoCreateInstance
#include <uxtheme.h>        // SetWindowTheme (non usato direttamente ma incluso)
#include <dwmapi.h>         // DwmExtendFrameIntoClientArea, DwmIsCompositionEnabled
#include <strsafe.h>        // StringCchCopyW, StringCchPrintfW
#include <shellapi.h>       // ShellExecuteExW, ShellExecuteW, ExtractIconExW
#include <commctrl.h>       // ToolbarWindow32, TOOLTIPS_CLASS, TTM_*
#include <math.h>           // fmin, ceil (usati nei calcoli layout)
#include <windhawk_api.h>   // Wh_Log, Wh_SetFunctionHook, Wh_GetIntSetting
#include <shlwapi.h>        // StrStrIW (non direttamente, ma utile per path)
#include <iphlpapi.h>       // GetAdaptersInfo
#include <netlistmgr.h>     // INetworkListManager, NLM_CONNECTIVITY


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
#define FOOTER_TEXT_Y_OFFSET 15

#define IDC_CONN_BUTTON     1002
#define IDC_AUTO_CHECKBOX   1003
#define HOTKEY_ID           9001
#define WM_REFRESH_DATA     (WM_USER + 100)
#define WM_SAFE_CLOSE       (WM_USER + 101)
#define WM_SHOW_FLYOUT      (WM_USER + 102)
#define WM_INSTALL_TRAY     (WM_USER + 103)
#define WM_CHECK_CONNECTION (WM_USER + 104)
#define WM_CONNECT_FAILED   (WM_USER + 105) 
#define IDM_CONNECT         2001
#define IDM_DISCONNECT      2002
#define IDM_STATUS          2003
#define IDM_PROPERTIES      2004

#define REG_PATH_NETWORK    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Control Panel\\Settings\\Network"
#define REG_VALUE_REPLACEVAN L"ReplaceVan"

#define TRAY_NETWORK_ID 2
#define CLICK_DEBOUNCE_MS 600
#define WM_HOTKEY_SETTINGS_CHANGED (WM_USER + 200)

static INetworkListManager* g_pNLM = NULL;

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
    BOOL enableHotkey;  // <-- NEW
} g_Settings = { TRUE, FALSE, TRUE, TRUE, 3000, 0, FALSE };  

static bool s_settingsSavedOnce = false;

void LoadSettings() {
    int raw_intercept  = Wh_GetIntSetting(L"interceptNativeFlyout");
    int raw_privacy    = Wh_GetIntSetting(L"privacyMode");
    int raw_registry   = Wh_GetIntSetting(L"useRegistryMethod");
    int raw_redirectCtx= Wh_GetIntSetting(L"redirectNetworkContextMenu");
    int raw_refresh    = Wh_GetIntSetting(L"refreshInterval");
    int raw_language   = Wh_GetIntSetting(L"language");
    int raw_enableHotkey = Wh_GetIntSetting(L"enableHotkey");  // <-- questo c'è già

    if (!s_settingsSavedOnce &&
        raw_intercept == 0 && raw_privacy == 0 &&
        raw_registry  == 0 && raw_redirectCtx == 0 && 
        raw_refresh == 0 && raw_language == 0) {
        Wh_Log(L"Settings not yet saved in Windhawk panel - using hardcoded defaults "
               L"(intercept:1 privacy:0 registry:1 redirectCtx:1 refresh:3000 language:0 hotkey:0)");
        g_Settings.interceptNativeFlyout      = TRUE;
        g_Settings.privacyMode               = FALSE;
        g_Settings.useRegistryMethod         = TRUE;
        g_Settings.redirectNetworkContextMenu = TRUE;
        g_Settings.refreshInterval            = 3000;
        g_Settings.language                  = 0;
        g_Settings.enableHotkey              = FALSE;  // <-- AGGIUNGI QUESTA RIGA
    } else {
        g_Settings.interceptNativeFlyout      = raw_intercept   != 0;
        g_Settings.privacyMode               = raw_privacy     != 0;
        g_Settings.useRegistryMethod         = raw_registry    != 0;
        g_Settings.redirectNetworkContextMenu = raw_redirectCtx != 0;
        g_Settings.refreshInterval            = raw_refresh >= 0 ? raw_refresh : 3000;
        g_Settings.language                  = raw_language;
        g_Settings.enableHotkey              = raw_enableHotkey != 0;  // <-- AGGIUNGI QUESTA RIGA
    }

    // Aggiorna il log per includere enableHotkey
    Wh_Log(L"Settings loaded - intercept:%d privacy:%d registry:%d redirectCtx:%d refresh:%d language:%d hotkey:%d",
           g_Settings.interceptNativeFlyout, g_Settings.privacyMode,
           g_Settings.useRegistryMethod, g_Settings.redirectNetworkContextMenu,
           g_Settings.refreshInterval, g_Settings.language,
           g_Settings.enableHotkey);  // <-- AGGIUNGI questo parametro
}

// -------------------------------------------------------
// Strutture
// -------------------------------------------------------
typedef struct {
    HWND     hWndFlyout;
    HANDLE   hWlanClient;
    HANDLE   hHotkeyThread;
    DWORD    dwHotkeyThreadId;
    HWND     hTrayNotifyWnd;
    volatile LONG refCount;
    volatile LONG isUninitializing;
    CRITICAL_SECTION csLock;
} ModContext;

typedef struct {
    WCHAR ssid[33];
    BOOL  isConnected;
    BOOL  isSecured;
    ULONG signalQuality;
    GUID  interfaceGuid;
    DOT11_BSS_TYPE dot11BssType;
    BOOL  hasProfile;
    BOOL  hasInternetAccess;
    BOOL  isConnecting;
    BOOL  isDisconnecting;
} WifiNetworkItem;

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
    Wh_Log(L"Windows build: %lu, IsWin11: %d", osvi.dwBuildNumber, g_isWin11);
}

// -------------------------------------------------------
// Variabili Globali
// -------------------------------------------------------
static ModContext g_Ctx        = {0};
static BOOL       g_Initialized = FALSE;

HWND g_hWndFlyout          = NULL;
HWND g_hWndButtonConnect   = NULL;
HWND g_hWndCheckboxConnect = NULL;
BOOL g_bListExpanded        = TRUE;

HFONT g_hFontNormal    = NULL;
HFONT g_hFontBold      = NULL;
HFONT g_hFontTitle     = NULL;
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

BOOL  g_IsConnectingAsynchronously = FALSE;
WCHAR g_PendingSsid[33]            = {0};
int   g_PendingConnectIndex        = -1;
int   g_ConnectRetryCount          = 0;
HWND  g_hTooltip = NULL;

BOOL g_bInitialRefreshDone = FALSE;
UINT_PTR g_RefreshTimer = 0;
UINT_PTR g_ConnectCheckTimer = 0;

WNDPROC G_OldToolbarWndProc = nullptr;
HWND    G_hSubclassedToolbar = nullptr;

using TrackPopupMenuEx_t = BOOL(WINAPI*)(HMENU, UINT, int, int, HWND, const TPMPARAMS*);
static TrackPopupMenuEx_t g_origTrackPopupMenuEx = nullptr;

static WCHAR g_TooltipBuffer[1024] = {0};

// -------------------------------------------------------
// Localizzazione - Sistema dinamico basato su array
// -------------------------------------------------------

// Enum per indicizzare tutte le stringhe localizzabili
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
    STR_COUNT
} LocaleStringId;

// Struttura per un language pack
typedef struct {
    LANGID langId;
    const WCHAR* strings[STR_COUNT];
} LocalePack;

// Language packs disponibili
static const LocalePack g_Locales[] = {
    // Inglese (predefinito) - LANGID 0x0409
    { 0x0409, {
        L"Currently connected to:",                    // 0  STR_CURRENT_CONNECTED
        L"Internet access",                            // 1  STR_INTERNET_ACCESS
        L"Wireless Network Connection",                // 2  STR_WIFI_HEADER
        L"Connected",                                  // 3  STR_CONNECTED_TEXT
        L"Open Network and Sharing Center",            // 4  STR_OPEN_SHARING_CENTER
        L"Connect",                                    // 5  STR_BTN_CONNECT
        L"Disconnect",                                 // 6  STR_BTN_DISCONNECT
        L"Connect",                                    // 7  STR_CTX_CONNECT
        L"Disconnect",                                 // 8  STR_CTX_DISCONNECT
        L"Status",                                     // 9  STR_CTX_STATUS
        L"Properties",                                 // 10 STR_CTX_PROPERTIES
        L"No connections available",                   // 11 STR_NO_CONNECTIONS
        L"Connections are available",                  // 12 STR_CONNECTIONS_AVAILABLE
        L"Connect automatically",                      // 13 STR_CHK_CONNECT_AUTO
        L"Connect to a Network",                       // 14 STR_PWD_TITLE
        L"Type the network security key",              // 15 STR_PWD_INSTRUCTIONS
        L"Security key:",                              // 16 STR_PWD_LABEL
        L"Hide characters",                            // 17 STR_PWD_HIDE_CHARS
        L"OK",                                         // 18 STR_PWD_OK
        L"Cancel",                                     // 19 STR_PWD_CANCEL
        L"Connection Failed",                          // 20 STR_PWD_FAILED_TITLE
        L"The network security key isn't correct. Please try again.", // 21 STR_PWD_FAILED_WRONG
        L"Failed to connect to %s",                    // 22 STR_PWD_CONNECTION_FAILED
        L"Network %d",                                 // 23 STR_NETWORK_PRIVACY_FMT
        L"Security type:",                             // 24 STR_SECURITY_TYPE
        L"Signal strength:",                           // 25 STR_SIGNAL_STRENGTH
        L"Radio type:",                                // 26 STR_RADIO_TYPE
        L"Excellent",                                  // 27 STR_SIG_EXCELLENT
        L"Good",                                       // 28 STR_SIG_GOOD
        L"Fair",                                       // 29 STR_SIG_FAIR
        L"Poor",                                       // 30 STR_SIG_POOR
        L"No signal",                                  // 31 STR_SIG_NONE
        L"Connecting...",                              // 32 STR_CONNECTING
        L"Disconnecting...",                           // 33 STR_DISCONNECTING
        L"Status: Connected",                          // 34 STR_STATUS_CONNECTED
        L"Status: Connecting...",                      // 35 STR_STATUS_CONNECTING
        L"Status: Not connected",                      // 36 STR_STATUS_NOT_CONNECTED
        L"Error",                                      // 37 STR_ERROR_TITLE
        L"Unable to save network profile (code: %lu)", // 38 STR_PROFILE_SAVE_FAILED
        L"Connection error (code: %lu)",               // 39 STR_CONNECTION_ERROR
        L"Connection timeout",                         // 40 STR_TIMEOUT_ERROR
    }},
    // Italiano - LANGID 0x0410
    { 0x0410, {
        L"Attualmente connesso a:",                    // 0  STR_CURRENT_CONNECTED
        L"Accesso a Internet",                         // 1  STR_INTERNET_ACCESS
        L"Connessione rete wireless",                  // 2  STR_WIFI_HEADER
        L"Connesso",                                   // 3  STR_CONNECTED_TEXT
        L"Apri Centro connessioni di rete e condivisione", // 4  STR_OPEN_SHARING_CENTER
        L"Connetti",                                   // 5  STR_BTN_CONNECT
        L"Disconnetti",                                // 6  STR_BTN_DISCONNECT
        L"Connetti",                                   // 7  STR_CTX_CONNECT
        L"Disconnetti",                                // 8  STR_CTX_DISCONNECT
        L"Stato",                                      // 9  STR_CTX_STATUS
        L"Propriet\u00E0",                            // 10 STR_CTX_PROPERTIES
        L"Nessuna connessione disponibile",            // 11 STR_NO_CONNECTIONS
        L"Connessioni disponibili",                    // 12 STR_CONNECTIONS_AVAILABLE
        L"Connetti automaticamente",                   // 13 STR_CHK_CONNECT_AUTO
        L"Connetti a una rete",                        // 14 STR_PWD_TITLE
        L"Digitare la chiave di sicurezza di rete",    // 15 STR_PWD_INSTRUCTIONS
        L"Chiave di sicurezza:",                       // 16 STR_PWD_LABEL
        L"Nascondi caratteri",                         // 17 STR_PWD_HIDE_CHARS
        L"OK",                                         // 18 STR_PWD_OK
        L"Annulla",                                    // 19 STR_PWD_CANCEL
        L"Impossibile connettersi",                    // 20 STR_PWD_FAILED_TITLE
        L"La chiave di sicurezza di rete non \u00E8 corretta. Riprova.", // 21 STR_PWD_FAILED_WRONG
        L"Connessione a %s fallita",                   // 22 STR_PWD_CONNECTION_FAILED
        L"Rete %d",                                    // 23 STR_NETWORK_PRIVACY_FMT
        L"Tipo di sicurezza:",                         // 24 STR_SECURITY_TYPE
        L"Intensit\u00E0 del segnale:",               // 25 STR_SIGNAL_STRENGTH
        L"Tipo di radio:",                             // 26 STR_RADIO_TYPE
        L"Eccellente",                                 // 27 STR_SIG_EXCELLENT
        L"Buono",                                      // 28 STR_SIG_GOOD
        L"Discreto",                                   // 29 STR_SIG_FAIR
        L"Scarso",                                     // 30 STR_SIG_POOR
        L"Nessun segnale",                             // 31 STR_SIG_NONE
        L"Connessione in corso...",                    // 32 STR_CONNECTING
        L"Disconnessione in corso...",                 // 33 STR_DISCONNECTING
        L"Stato: Connesso",                            // 34 STR_STATUS_CONNECTED
        L"Stato: Connessione in corso...",             // 35 STR_STATUS_CONNECTING
        L"Stato: Non connesso",                        // 36 STR_STATUS_NOT_CONNECTED
        L"Errore",                                     // 37 STR_ERROR_TITLE
        L"Impossibile salvare il profilo di rete (codice: %lu)", // 38 STR_PROFILE_SAVE_FAILED
        L"Errore di connessione (codice: %lu)",        // 39 STR_CONNECTION_ERROR
        L"Timeout durante la connessione",             // 40 STR_TIMEOUT_ERROR
    }}
};
// Puntatore al language pack attivo (default: inglese)
static const LocalePack* g_CurrentLocalePack = &g_Locales[0];

// Macro per accesso rapido alle stringhe localizzate
#define LOC(id) (g_CurrentLocalePack->strings[id])

static const LocalePack* FindLocalePack(LANGID langId)
{
    LANGID primaryLang = PRIMARYLANGID(langId);

    const size_t count = ARRAYSIZE(g_Locales);

    for (size_t i = 0; i < count; ++i) {
        if (g_Locales[i].langId == langId) {
            return &g_Locales[i];
        }
    }

    for (size_t i = 0; i < count; ++i) {
        if (PRIMARYLANGID(g_Locales[i].langId) == primaryLang) {
            return &g_Locales[i];
        }
    }

    return &g_Locales[0];
}

void DetermineLocale() {
    switch (g_Settings.language) {
        case 1:
            g_CurrentLocalePack = FindLocalePack(0x0409);
            Wh_Log(L"Language forced to English");
            break;
        case 2:
            g_CurrentLocalePack = FindLocalePack(0x0410);
            Wh_Log(L"Language forced to Italian");
            break;
        default:
            {
                LANGID userLangId = GetUserDefaultUILanguage();
                g_CurrentLocalePack = FindLocalePack(userLangId);
                Wh_Log(L"Language auto-detected: LANGID 0x%04X", userLangId);
            }
            break;
    }
}

// Converte qualità segnale in stringa descrittiva stile Windows 7
static const WCHAR* SignalQualityToString(ULONG quality) {
    if (quality > 80) return LOC(STR_SIG_EXCELLENT);
    if (quality > 60) return LOC(STR_SIG_GOOD);
    if (quality > 40) return LOC(STR_SIG_FAIR);
    if (quality > 20) return LOC(STR_SIG_POOR);
    return LOC(STR_SIG_NONE);
}

// -------------------------------------------------------
// Prototipi
// -------------------------------------------------------
void RefreshWifiData(HANDLE hClient);
void UpdateLayoutGeometry();
void HandleNativeConnection(HANDLE hClient, int index);
BOOL SafeToAccessUI();
void SafeCleanup();
void ToggleFlyoutWindow();
void InitTooltip(HWND hwnd);
void UpdateTooltipForRow(HWND hwnd, int index);
BOOL GetRowRect(int index, RECT* rcRow);
void InstallTrayInterception();
void RemoveTrayInterception();
void InitRefreshButtonRect();
BOOL GetAdapterNameForInterface(GUID interfaceGuid, WCHAR* adapterName, DWORD bufferSize);
void OpenNetworkStatusForInterface(GUID interfaceGuid);
void OpenNetworkPropertiesForInterface(GUID interfaceGuid);
void SetKeyboardFocus(int index);
void ClearKeyboardFocus();
BOOL IsInternetConnected();

// -------------------------------------------------------
// Verifica connessione internet
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
// Focus tastiera
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
// Adapter / network status helpers
// -------------------------------------------------------
BOOL GetAdapterNameForInterface(GUID interfaceGuid, WCHAR* adapterName, DWORD bufferSize) {
    PIP_ADAPTER_INFO pAdapterInfo = NULL;
    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    DWORD dwStatus;
    pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
    if (!pAdapterInfo) return FALSE;
    dwStatus = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
    if (dwStatus == ERROR_BUFFER_OVERFLOW) {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
        if (!pAdapterInfo) return FALSE;
        dwStatus = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
    }
    if (dwStatus == NO_ERROR) {
        PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
        while (pAdapter) {
            WCHAR wideAdapterName[256];
            MultiByteToWideChar(CP_ACP, 0, pAdapter->AdapterName, -1, wideAdapterName, 256);
            GUID adapterGuid;
            if (CLSIDFromString(wideAdapterName, &adapterGuid) == S_OK) {
                if (IsEqualGUID(adapterGuid, interfaceGuid)) {
                    MultiByteToWideChar(CP_ACP, 0, pAdapter->Description, -1, adapterName, bufferSize);
                    free(pAdapterInfo);
                    return TRUE;
                }
            }
            pAdapter = pAdapter->Next;
        }
    }
    if (pAdapterInfo) free(pAdapterInfo);
    return FALSE;
}

void OpenNetworkStatusForInterface(GUID interfaceGuid) {
    WCHAR adapterName[256];
    if (GetAdapterNameForInterface(interfaceGuid, adapterName, 256)) {
        HWND hNcpa = FindWindowW(NULL, L"Connessioni di rete");
        if (!hNcpa) hNcpa = FindWindowW(NULL, L"Network Connections");
        if (!hNcpa) {
            ShellExecuteW(NULL, L"open", L"control.exe", L"ncpa.cpl", NULL, SW_SHOWNORMAL);
            hNcpa = FindWindowW(NULL, L"Connessioni di rete");
            if (!hNcpa) hNcpa = FindWindowW(NULL, L"Network Connections");
        }
        if (hNcpa) {
            SetForegroundWindow(hNcpa);
            SetWindowPos(hNcpa, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
            SetWindowPos(hNcpa, HWND_NOTOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
            HWND hListView = FindWindowExW(hNcpa, NULL, L"SysListView32", NULL);
            if (hListView) {
                int itemCount = (int)SendMessageW(hListView, LVM_GETITEMCOUNT, 0, 0);
                for (int i = 0; i < itemCount; i++) {
                    WCHAR itemText[256]; LVITEMW lvi = {0};
                    lvi.mask=LVIF_TEXT; lvi.iItem=i; lvi.iSubItem=0;
                    lvi.pszText=itemText; lvi.cchTextMax=256;
                    SendMessageW(hListView, LVM_GETITEMW, 0, (LPARAM)&lvi);
                    if (wcsstr(itemText, adapterName) || wcsstr(adapterName, itemText)) {
                        ListView_SetItemState(hListView, i, LVIS_SELECTED, LVIS_SELECTED);
                        RECT rcItem;
                        SendMessageW(hListView, LVM_GETITEMRECT, (WPARAM)i, (LPARAM)&rcItem);
                        int x=(rcItem.left+rcItem.right)/2, y=(rcItem.top+rcItem.bottom)/2;
                        PostMessage(hListView, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x,y));
                        PostMessage(hListView, WM_LBUTTONUP, 0, MAKELPARAM(x,y));
                        PostMessage(hListView, WM_LBUTTONDBLCLK, MK_LBUTTON, MAKELPARAM(x,y));
                        PostMessage(hListView, WM_LBUTTONUP, 0, MAKELPARAM(x,y));
                        break;
                    }
                }
            }
        }
    } else {
        ShellExecuteW(NULL, L"open", L"control.exe", L"ncpa.cpl", NULL, SW_SHOWNORMAL);
    }
}

void OpenNetworkPropertiesForInterface(GUID interfaceGuid) {
    WCHAR adapterName[256];
    if (GetAdapterNameForInterface(interfaceGuid, adapterName, 256)) {
        HWND hNcpa = FindWindowW(NULL, L"Connessioni di rete");
        if (!hNcpa) hNcpa = FindWindowW(NULL, L"Network Connections");
        if (!hNcpa) {
            ShellExecuteW(NULL, L"open", L"control.exe", L"ncpa.cpl", NULL, SW_SHOWNORMAL);
            hNcpa = FindWindowW(NULL, L"Connessioni di rete");
            if (!hNcpa) hNcpa = FindWindowW(NULL, L"Network Connections");
        }
        if (hNcpa) {
            SetForegroundWindow(hNcpa);
            SetWindowPos(hNcpa, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
            SetWindowPos(hNcpa, HWND_NOTOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
            HWND hListView = FindWindowExW(hNcpa, NULL, L"SysListView32", NULL);
            if (hListView) {
                int itemCount = (int)SendMessageW(hListView, LVM_GETITEMCOUNT, 0, 0);
                for (int i = 0; i < itemCount; i++) {
                    WCHAR itemText[256]; LVITEMW lvi = {0};
                    lvi.mask=LVIF_TEXT; lvi.iItem=i; lvi.iSubItem=0;
                    lvi.pszText=itemText; lvi.cchTextMax=256;
                    SendMessageW(hListView, LVM_GETITEMW, 0, (LPARAM)&lvi);
                    if (wcsstr(itemText, adapterName) || wcsstr(adapterName, itemText)) {
                        ListView_SetItemState(hListView, i, LVIS_SELECTED, LVIS_SELECTED);
                        SetForegroundWindow(hNcpa);
                        INPUT inputs[4] = {{0}};
                        inputs[0].type=INPUT_KEYBOARD; inputs[0].ki.wVk=VK_MENU;
                        inputs[1].type=INPUT_KEYBOARD; inputs[1].ki.wVk=VK_RETURN;
                        inputs[2].type=INPUT_KEYBOARD; inputs[2].ki.wVk=VK_RETURN; inputs[2].ki.dwFlags=KEYEVENTF_KEYUP;
                        inputs[3].type=INPUT_KEYBOARD; inputs[3].ki.wVk=VK_MENU;   inputs[3].ki.dwFlags=KEYEVENTF_KEYUP;
                        SendInput(4, inputs, sizeof(INPUT));
                        break;
                    }
                }
            }
        }
    } else {
        ShellExecuteW(NULL, L"open", L"control.exe", L"ncpa.cpl", NULL, SW_SHOWNORMAL);
    }
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
// Registry hooks
// -------------------------------------------------------
typedef LONG (WINAPI *RegQueryValueExW_t)(HKEY, LPCWSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
RegQueryValueExW_t Real_RegQueryValueExW = NULL;

typedef LONG (WINAPI *RegGetValueW_t)(HKEY, LPCWSTR, LPCWSTR, DWORD, LPDWORD, PVOID, LPDWORD);
RegGetValueW_t Real_RegGetValueW = NULL;


LONG WINAPI Hook_RegQueryValueExW(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, 
                                   LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    // Se l'impostazione è disabilitata, comportamento trasparente
    if (!g_Settings.useRegistryMethod) {
        return Real_RegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
    }
    
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
        if (lpcbData) {
            *lpcbData = sizeof(DWORD);
            return ERROR_SUCCESS;
        }
    }
    return Real_RegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

LONG WINAPI Hook_RegGetValueW(HKEY hkey, LPCWSTR lpSubKey, LPCWSTR lpValue, DWORD dwFlags, 
                               LPDWORD pdwType, PVOID pvData, LPDWORD pcbData) {
    // Se l'impostazione è disabilitata, comportamento trasparente
    if (!g_Settings.useRegistryMethod) {
        return Real_RegGetValueW(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
    }
    
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
        if (pcbData) {
            *pcbData = sizeof(DWORD);
            return ERROR_SUCCESS;
        }
    }
    return Real_RegGetValueW(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
}


// -------------------------------------------------------
// TrackPopupMenuEx Hook (basato su settings-to-control-panel v10.0.2)
// -------------------------------------------------------

// ID comandi ESATTI del menu contestuale della tray di rete
static constexpr UINT CMD_OPEN_NETWORK_SETTINGS_TRAY = 3109;
static constexpr UINT CMD_NETWORK_STATUS_TRAY         = 3108;
static constexpr UINT CMD_NETWORK_DIAGNOSTICS_TRAY    = 3110;

// Reentry guard per evitare ricorsione
static thread_local int g_trackPopupHookDepth = 0;

static BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y,
                                         HWND hWnd, const TPMPARAMS* lptpm) {
    // Controllo immediato dell'impostazione
    if (!g_Settings.redirectNetworkContextMenu)
        return g_origTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);
    
    // Reentry guard
    if (g_trackPopupHookDepth > 0)
        return g_origTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);
    
    g_trackPopupHookDepth++;
    
    // Verifica RIGOROSA che sia la ToolbarWindow32 della tray
    if (!hWnd) {
        g_trackPopupHookDepth--;
        return g_origTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);
    }
    
    WCHAR className[64];
    if (!GetClassNameW(hWnd, className, 64) || 
        _wcsicmp(className, L"ToolbarWindow32") != 0) {
        g_trackPopupHookDepth--;
        return g_origTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);
    }
    
    // Controllo dimensione menu
    int count = GetMenuItemCount(hMenu);
    if (count < 2 || count > 6) {
        g_trackPopupHookDepth--;
        return g_origTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);
    }
    
    // Verifica pattern specifici del menu di rete
    int networkMatches = 0;
    int totalItems = 0;
    
    for (int i = 0; i < count; i++) {
        WCHAR text[128];
        if (GetMenuStringW(hMenu, i, text, 128, MF_BYPOSITION)) {
            totalItems++;
            
            if (wcsstr(text, L"Open Network & Internet") ||
                wcsstr(text, L"Open Network and Internet") ||
                wcsstr(text, L"Network & Internet settings") ||
                wcsstr(text, L"Network and Internet settings") ||
                wcsstr(text, L"Apri Centro connessioni") ||
                wcsstr(text, L"Apri impostazioni di rete") ||
                wcsstr(text, L"Troubleshoot") ||
                wcsstr(text, L"Risoluzione problemi") ||
                wcsstr(text, L"Network diagnostics") ||
                wcsstr(text, L"Diagnostica di rete")) {
                networkMatches++;
            }
        }
    }
    
    // Se non siamo sicuri al 100%, passa tutto normalmente
    if (totalItems == 0 || networkMatches < 2 || 
        (networkMatches * 100 / totalItems) < 70) {
        g_trackPopupHookDepth--;
        return g_origTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);
    }
    
    Wh_Log(L"Network tray menu identified (%d/%d matches) - intercepting", 
           networkMatches, totalItems);
    
    // Usa TPM_RETURNCMD per intercettare il comando (come fa 10.0.2)
    UINT modifiedFlags = uFlags | TPM_RETURNCMD;
    BOOL result = g_origTrackPopupMenuEx(hMenu, modifiedFlags, x, y, hWnd, lptpm);
    
    if (result > 0) {
        UINT cmd = (UINT)result;
        
        // Intercetta i comandi noti del menu di rete
        switch (cmd) {
            case CMD_OPEN_NETWORK_SETTINGS_TRAY:
                Wh_Log(L"Redirecting: Open Network Settings -> classic flyout");
                ToggleFlyoutWindow();
                g_trackPopupHookDepth--;
                return 0;
                
            case CMD_NETWORK_STATUS_TRAY:
                Wh_Log(L"Redirecting: Network Status -> ncpa.cpl");
                ShellExecuteW(NULL, L"open", L"control.exe", L"ncpa.cpl", NULL, SW_SHOWNORMAL);
                g_trackPopupHookDepth--;
                return 0;
                
            case CMD_NETWORK_DIAGNOSTICS_TRAY:
                Wh_Log(L"Redirecting: Network Diagnostics");
                ShellExecuteW(NULL, L"open", L"msdt.exe", L"-id NetworkDiagnosticsWeb", NULL, SW_SHOWNORMAL);
                g_trackPopupHookDepth--;
                return 0;
        }
        
        // IMPORTANTE: Se il chiamante originale NON aveva TPM_RETURNCMD,
        // invia manualmente WM_COMMAND per non rompere il comportamento
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
// Icone e risorse
// -------------------------------------------------------
void LoadSystemIcons() {
    if (!g_hIconNetworkMap)
        ExtractIconExW(L"netshell.dll", 120, &g_hIconNetworkMap, NULL, 1);
    for (int i = 0; i < 6; i++)
        if (!g_hIconSignalBars[i])
            ExtractIconExW(L"netshell.dll", 152 + i, &g_hIconSignalBars[i], NULL, 1);
    if (!g_hIconRefreshWin7) {
        ExtractIconExW(L"shell32.dll", 238, &g_hIconRefreshWin7, NULL, 1);
    }
}

void FreeSystemIcons() {
    if (g_hIconNetworkMap) { DestroyIcon(g_hIconNetworkMap); g_hIconNetworkMap = NULL; }
    for (int i = 0; i < 6; i++)
        if (g_hIconSignalBars[i]) { DestroyIcon(g_hIconSignalBars[i]); g_hIconSignalBars[i] = NULL; }
    if (g_hIconRefreshWin7) { DestroyIcon(g_hIconRefreshWin7); g_hIconRefreshWin7 = NULL; }
}

void InitGlobalFonts() {
    if (g_hFontNormal) return;
    g_hFontNormal    = CreateFontW(-12,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    g_hFontBold      = CreateFontW(-12,0,0,0,FW_BOLD,  0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    g_hFontTitle     = CreateFontW(-13,0,0,0,FW_BOLD,  0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    g_hFontUnderline = CreateFontW(-12,0,0,0,FW_NORMAL,0,1,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    g_hFontButton    = CreateFontW(-12,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    g_hFontCheckbox  = CreateFontW(-11,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    g_hFontArrow     = CreateFontW(-11,0,0,0,FW_NORMAL,0,0,0,SYMBOL_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Marlett");
}

void FreeGlobalFonts() {
    if (g_hFontNormal)    { DeleteObject(g_hFontNormal);    g_hFontNormal    = NULL; }
    if (g_hFontBold)      { DeleteObject(g_hFontBold);      g_hFontBold      = NULL; }
    if (g_hFontTitle)     { DeleteObject(g_hFontTitle);     g_hFontTitle     = NULL; }
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
// WLAN data
// -------------------------------------------------------
void RefreshWifiData(HANDLE hClient) {
    if (!hClient) return;

    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
    if (WlanEnumInterfaces(hClient, NULL, &pIfList) != ERROR_SUCCESS) return;

    WifiNetworkItem tempList[50];
    int tempCount = 0;

    for (DWORD i = 0; i < pIfList->dwNumberOfItems; i++) {
        WLAN_INTERFACE_INFO IfInfo = pIfList->InterfaceInfo[i];

        PWLAN_AVAILABLE_NETWORK_LIST pBssList  = NULL;
        PWLAN_PROFILE_INFO_LIST      pProfList = NULL;

        WlanGetProfileList(hClient, &IfInfo.InterfaceGuid, NULL, &pProfList);

        DWORD dwFlags = WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_MANUAL_HIDDEN_PROFILES;

        if (WlanGetAvailableNetworkList(
                hClient,
                &IfInfo.InterfaceGuid,
                dwFlags,
                NULL,
                &pBssList) == ERROR_SUCCESS) {

            for (DWORD j = 0; j < pBssList->dwNumberOfItems && tempCount < 50; j++) {

                WLAN_AVAILABLE_NETWORK network = pBssList->Network[j];

                size_t len = (size_t)network.dot11Ssid.uSSIDLength;

                if (len == 0) {
                    StringCchCopyW(
                        tempList[tempCount].ssid,
                        33,
                        L"Hidden Network"
                    );
                } else {
                    int converted = MultiByteToWideChar(
                        CP_UTF8,
                        MB_ERR_INVALID_CHARS,
                        (LPCSTR)network.dot11Ssid.ucSSID,
                        (int)len,
                        tempList[tempCount].ssid,
                        32
                    );

                    if (converted <= 0) {
                    size_t copyLen = (len < 32u) ? len : (size_t)32u;

                        for (size_t k = 0; k < copyLen; k++) {
                            tempList[tempCount].ssid[k] =
                                (WCHAR)(BYTE)network.dot11Ssid.ucSSID[k];
                        }

                        tempList[tempCount].ssid[copyLen] = L'\0';
                    } else {
                        size_t c = ((size_t)converted < 32u)
                                     ? (size_t)converted
                                     : 32u;

                        tempList[tempCount].ssid[c] = L'\0';
                    }
                }

                BOOL duplicate = FALSE;

                for (int d = 0; d < tempCount; d++) {
                    if (wcscmp(tempList[d].ssid,
                               tempList[tempCount].ssid) == 0) {

                        if (network.wlanSignalQuality >
                            tempList[d].signalQuality) {
                            tempList[d].signalQuality =
                                network.wlanSignalQuality;
                        }

                        if (network.dwFlags &
                            WLAN_AVAILABLE_NETWORK_CONNECTED) {
                            tempList[d].isConnected = TRUE;
                        }

                        duplicate = TRUE;
                        break;
                    }
                }

                if (duplicate) continue;

                tempList[tempCount].isConnected =
                    (network.dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED) != 0;

                tempList[tempCount].isSecured = network.bSecurityEnabled;
                tempList[tempCount].signalQuality = network.wlanSignalQuality;
                tempList[tempCount].interfaceGuid = IfInfo.InterfaceGuid;
                tempList[tempCount].dot11BssType = network.dot11BssType;

                tempList[tempCount].hasProfile = FALSE;
                tempList[tempCount].hasInternetAccess = FALSE;
                tempList[tempCount].isConnecting = FALSE;
                tempList[tempCount].isDisconnecting = FALSE;

                if (pProfList) {
                    for (DWORD p = 0; p < pProfList->dwNumberOfItems; p++) {
                        if (wcscmp(
                                pProfList->ProfileInfo[p].strProfileName,
                                tempList[tempCount].ssid) == 0) {
                            tempList[tempCount].hasProfile = TRUE;
                            break;
                        }
                    }
                }

                if (tempList[tempCount].isConnected && tempCount > 0) {
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

    CopyMemory(g_NetworkList,
               tempList,
               sizeof(WifiNetworkItem) * tempCount);

    g_NetworkCount = tempCount;

    if (g_NetworkCount > 0 && g_NetworkList[0].isConnected) {
        g_NetworkList[0].hasInternetAccess = IsInternetConnected();
    }

    for (int i = 0; i < g_NetworkCount; i++) {
        if (g_IsConnectingAsynchronously &&
            wcscmp(g_NetworkList[i].ssid, g_PendingSsid) == 0) {
            g_NetworkList[i].isConnecting = TRUE;
        }
    }
}
// -------------------------------------------------------
// Password dialog stile Windows 7
// -------------------------------------------------------
typedef struct {
    WCHAR* passwordBuffer;
    DWORD  bufferSize;
    BOOL   confirmed;
    WCHAR  networkName[33];
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
        HWND hInstruction = CreateWindowExW(0, WC_STATICW, LOC(STR_PWD_INSTRUCTIONS),
            WS_CHILD|WS_VISIBLE, 15,15,380,20, hwnd,(HMENU)200,cs->hInstance,NULL);
        SendMessageW(hInstruction, WM_SETFONT, (WPARAM)hFontDlg, TRUE);
        HWND hLabel = CreateWindowExW(0, WC_STATICW, LOC(STR_PWD_LABEL),
            WS_CHILD|WS_VISIBLE, 15,53,115,18, hwnd,NULL,cs->hInstance,NULL);
        SendMessageW(hLabel, WM_SETFONT, (WPARAM)hFontDlg, TRUE);
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
        BOOL pfEnabled = FALSE;
        DwmIsCompositionEnabled(&pfEnabled);
        if (pfEnabled) {
            DWMNCRENDERINGPOLICY pol = DWMNCRP_ENABLED;
            DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &pol, sizeof(pol));
        }
        HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
        if (hShell32) {
            HICON hIconLarge = (HICON)LoadImageW(hShell32, MAKEINTRESOURCE(162),
                IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_SHARED);
            HICON hIconSmall = (HICON)LoadImageW(hShell32, MAKEINTRESOURCE(162),
                IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_SHARED);
            if (hIconLarge) SendMessageW(hwnd, WM_SETICON, ICON_BIG,   (LPARAM)hIconLarge);
            if (hIconSmall) SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
            FreeLibrary(hShell32);
        } else {
            HICON hFallback = LoadIconW(NULL, IDI_APPLICATION);
            SendMessageW(hwnd, WM_SETICON, ICON_BIG,   (LPARAM)hFallback);
            SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hFallback);
        }
        break;
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdc = (HDC)wParam;
        SetBkMode(hdc, TRANSPARENT);
        if (GetDlgCtrlID((HWND)lParam) == 200)
            SetTextColor(hdc, RGB(0,51,153));
        else
            SetTextColor(hdc, RGB(0,0,0));
        return (INT_PTR)GetStockObject(NULL_BRUSH);
    }
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        if (wmId == 102) {
            HWND hEdit = GetDlgItem(hwnd, 101);
            BOOL checked = SendMessageW((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED;
            SendMessageW(hEdit, EM_SETPASSWORDCHAR, checked ? 0 : L'*', 0);
            InvalidateRect(hEdit, NULL, TRUE);
            return 0;
        }
        if (wmId == IDOK) {
            if (data) { GetDlgItemTextW(hwnd,101,data->passwordBuffer,data->bufferSize); data->confirmed=TRUE; }
            DestroyWindow(hwnd); return 0;
        } else if (wmId == IDCANCEL) {
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

BOOL PromptNetworkPassword(HWND hParent, WCHAR* passwordBuffer, DWORD bufferSize, const WCHAR* networkName) {
    if (!SafeToAccessUI()) return FALSE;
    HINSTANCE hInst = GetModuleHandle(NULL);
    WNDCLASSW wc = {0};
    wc.lpfnWndProc   = Win7PasswordWndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = L"Win7NetPwdClass";
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    wc.hIcon         = LoadIconW(NULL, IDI_APPLICATION);
    UnregisterClassW(wc.lpszClassName, hInst);
    RegisterClassW(&wc);
    PasswordDlgData data = { passwordBuffer, bufferSize, FALSE };
    StringCchCopyW(data.networkName, 33, networkName ? networkName : L"");
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

// -------------------------------------------------------
// Connessione migliorata
// -------------------------------------------------------
void HandleNativeConnection(HANDLE hClient, int index) {
    if (index < 0 || index >= g_NetworkCount || !hClient) return;
    WifiNetworkItem* item = &g_NetworkList[index];
    
    // Disconnessione
    if (item->isConnected) {
        g_IsConnectingAsynchronously = FALSE;
        g_PendingConnectIndex = -1;
        g_NetworkList[index].isDisconnecting = TRUE;
        g_ConnectRetryCount = 0;
        
        WlanDisconnect(hClient, &item->interfaceGuid, NULL);
        
        // Avvia timer di controllo disconnessione
        if (g_ConnectCheckTimer) {
            KillTimer(g_hWndFlyout, g_ConnectCheckTimer);
            g_ConnectCheckTimer = 0;
        }
        g_ConnectCheckTimer = SetTimer(g_hWndFlyout, 1001, 500, NULL);
        
        InvalidateRect(g_hWndFlyout, NULL, TRUE);
        return;
    }
    
    // Rete protetta senza profilo salvato
    if (item->isSecured && !item->hasProfile) {
        WCHAR password[65] = {0};
        if (!PromptNetworkPassword(g_hWndFlyout, password, 64, item->ssid)) return;
        
        // Escape XML per caratteri speciali
        WCHAR escapedSsid[256];
        WCHAR escapedPwd[256];
        WCHAR* p;
        
        // Escape SSID
        p = escapedSsid;
        for (WCHAR* c = item->ssid; *c && (p - escapedSsid) < 250; c++) {
            switch (*c) {
                case L'&':  wcscpy_s(p, 6, L"&amp;");  p += 5; break;
                case L'<':  wcscpy_s(p, 5, L"&lt;");   p += 4; break;
                case L'>':  wcscpy_s(p, 5, L"&gt;");   p += 4; break;
                case L'"':  wcscpy_s(p, 7, L"&quot;"); p += 6; break;
                case L'\'': wcscpy_s(p, 7, L"&apos;"); p += 6; break;
                default:    *p++ = *c; break;
            }
        }
        *p = L'\0';
        
        // Escape password
        p = escapedPwd;
        for (WCHAR* c = password; *c && (p - escapedPwd) < 250; c++) {
            switch (*c) {
                case L'&':  wcscpy_s(p, 6, L"&amp;");  p += 5; break;
                case L'<':  wcscpy_s(p, 5, L"&lt;");   p += 4; break;
                case L'>':  wcscpy_s(p, 5, L"&gt;");   p += 4; break;
                case L'"':  wcscpy_s(p, 7, L"&quot;"); p += 6; break;
                case L'\'': wcscpy_s(p, 7, L"&apos;"); p += 6; break;
                default:    *p++ = *c; break;
            }
        }
        *p = L'\0';
        
        WCHAR xmlProfile[2048];
        StringCchPrintfW(xmlProfile, 2048,
            L"<?xml version=\"1.0\"?>"
            L"<WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\">"
            L"<name>%s</name><SSIDConfig><SSID><name>%s</name></SSID></SSIDConfig>"
            L"<connectionType>ESS</connectionType><connectionMode>auto</connectionMode>"
            L"<MSM><security><authEncryption>"
            L"<authentication>WPA2PSK</authentication><encryption>AES</encryption><useOneX>false</useOneX>"
            L"</authEncryption><sharedKey><keyType>passPhrase</keyType><protected>false</protected>"
            L"<keyMaterial>%s</keyMaterial></sharedKey></security></MSM></WLANProfile>",
            escapedSsid, escapedSsid, escapedPwd);
        
        DWORD dwReason = 0;
        DWORD setResult = WlanSetProfile(hClient, &item->interfaceGuid, 0, 
                                          xmlProfile, NULL, TRUE, NULL, &dwReason);
        if (setResult != ERROR_SUCCESS) {
            WCHAR errMsg[256];
            StringCchPrintfW(errMsg, 256, LOC(STR_PROFILE_SAVE_FAILED), setResult);
            MessageBoxW(g_hWndFlyout, errMsg, LOC(STR_ERROR_TITLE), MB_OK | MB_ICONERROR);
            return;
        }
        item->hasProfile = TRUE;
    }
    
    // Avvia connessione
    g_IsConnectingAsynchronously = TRUE;
    g_PendingConnectIndex = index;
    StringCchCopyW(g_PendingSsid, 33, item->ssid);
    g_NetworkList[index].isConnecting = TRUE;
    g_ConnectRetryCount = 0;
    
    WLAN_CONNECTION_PARAMETERS params;
    ZeroMemory(&params, sizeof(params));
    if (item->hasProfile) { 
        params.wlanConnectionMode = wlan_connection_mode_profile; 
        params.strProfile = item->ssid; 
    } else {
        params.wlanConnectionMode = wlan_connection_mode_discovery_unsecure; 
    }
    params.dot11BssType = item->dot11BssType;
    
    DWORD res = WlanConnect(hClient, &item->interfaceGuid, &params, NULL);
    if (res != ERROR_SUCCESS) {
        g_IsConnectingAsynchronously = FALSE;
        g_NetworkList[index].isConnecting = FALSE;
        g_PendingConnectIndex = -1;
        
        WCHAR errMsg[256]; 
        StringCchPrintfW(errMsg, 256, LOC(STR_CONNECTION_ERROR), res);
        MessageBoxW(g_hWndFlyout, errMsg, LOC(STR_ERROR_TITLE), MB_OK | MB_ICONERROR);
        InvalidateRect(g_hWndFlyout, NULL, TRUE);
        return;
    }
    
    // Timer di fallback
    if (g_ConnectCheckTimer) {
        KillTimer(g_hWndFlyout, g_ConnectCheckTimer);
        g_ConnectCheckTimer = 0;
    }
    g_ConnectCheckTimer = SetTimer(g_hWndFlyout, 1001, 500, NULL);
    
    InvalidateRect(g_hWndFlyout, NULL, TRUE);
}

// -------------------------------------------------------
// WLAN Notification Callback
// -------------------------------------------------------
void WINAPI WlanNotificationCallback(PWLAN_NOTIFICATION_DATA data, PVOID context) {
    ModContext* ctx = (ModContext*)context;
    if (!ctx || ctx->isUninitializing) return;
    
    if (!data) return;
    
    BOOL needRefresh = FALSE;
    BOOL connectionSucceeded = FALSE;
    BOOL connectionFailed = FALSE;
    BOOL disconnected = FALSE;
    DWORD reasonCode = 0;

    switch(data->NotificationSource) {
        case WLAN_NOTIFICATION_SOURCE_ACM:
            if (data->NotificationCode == wlan_notification_acm_connection_complete) {
                needRefresh = TRUE;
                if (data->pData && data->dwDataSize >= sizeof(WLAN_CONNECTION_NOTIFICATION_DATA)) {
                    PWLAN_CONNECTION_NOTIFICATION_DATA pConnData = 
                        (PWLAN_CONNECTION_NOTIFICATION_DATA)data->pData;
                    if (pConnData->wlanReasonCode == ERROR_SUCCESS) {
                        connectionSucceeded = TRUE;
                    } else {
                        connectionFailed = TRUE;
                        reasonCode = pConnData->wlanReasonCode;
                    }
                }
            }
            else if (data->NotificationCode == wlan_notification_acm_connection_attempt_fail) {
                connectionFailed = TRUE;
                needRefresh = TRUE;
                if (data->pData && data->dwDataSize >= sizeof(DWORD)) {
                    reasonCode = *(DWORD*)data->pData;
                }
            }
            else if (data->NotificationCode == wlan_notification_acm_disconnected) {
                disconnected = TRUE;
                needRefresh = TRUE;
            }
            else if (data->NotificationCode == wlan_notification_acm_scan_complete ||
                     data->NotificationCode == wlan_notification_acm_scan_list_refresh) {
                needRefresh = TRUE;
            }
            break;

        case WLAN_NOTIFICATION_SOURCE_MSM:
            if (data->NotificationCode == wlan_notification_msm_connected) {
                connectionSucceeded = TRUE;
                needRefresh = TRUE;
            }
            else if (data->NotificationCode == wlan_notification_msm_disconnected) {
                disconnected = TRUE;
                needRefresh = TRUE;
            }
            break;
    }

    // Aggiorna lo stato globale in modo sicuro
    // IMPORTANTE: connectionSucceeded ha priorità su connectionFailed
    if (connectionSucceeded) {
        g_IsConnectingAsynchronously = FALSE;
        if (g_PendingConnectIndex >= 0 && g_PendingConnectIndex < g_NetworkCount) {
            g_NetworkList[g_PendingConnectIndex].isConnecting = FALSE;
            g_NetworkList[g_PendingConnectIndex].isConnected = TRUE;
            g_NetworkList[g_PendingConnectIndex].isDisconnecting = FALSE;
        }
        g_PendingConnectIndex = -1;
        g_ConnectRetryCount = 0;
    }
    else if (connectionFailed && g_IsConnectingAsynchronously) {
        g_IsConnectingAsynchronously = FALSE;
        if (g_PendingConnectIndex >= 0 && g_PendingConnectIndex < g_NetworkCount) {
            g_NetworkList[g_PendingConnectIndex].isConnecting = FALSE;
        }
        g_PendingConnectIndex = -1;
        g_ConnectRetryCount = 0;
    }
    else if (disconnected) {
        g_IsConnectingAsynchronously = FALSE;
        if (g_PendingConnectIndex >= 0 && g_PendingConnectIndex < g_NetworkCount) {
            g_NetworkList[g_PendingConnectIndex].isDisconnecting = FALSE;
            g_NetworkList[g_PendingConnectIndex].isConnected = FALSE;
            g_NetworkList[g_PendingConnectIndex].isConnecting = FALSE;
        }
        g_PendingConnectIndex = -1;
        g_ConnectRetryCount = 0;
    }

    // Invia messaggi alla finestra UI (MAI KillTimer da qui!)
    if (IsWindow(g_hWndFlyout)) {
        if (needRefresh) {
            PostMessageW(g_hWndFlyout, WM_REFRESH_DATA, 0, 0);
        }
        // MOSTRA ERRORE SOLO SE LA CONNESSIONE È FALLITA E NON C'È STATA UNA CONNESSIONE RIUSCITA
        if (connectionFailed && !connectionSucceeded) {
            PostMessageW(g_hWndFlyout, WM_CONNECT_FAILED, (WPARAM)reasonCode, 0);
        }
        // Notifica completamento per fermare il timer
        if (connectionSucceeded || connectionFailed || disconnected) {
            PostMessageW(g_hWndFlyout, WM_CHECK_CONNECTION, 0, 0);
        }
    }
}
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
// Tooltip stile Windows 7
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
    WCHAR securityType[64];
    StringCchCopyW(securityType, 64, item->isSecured ? L"WPA2-PSK" : L"Open");
    const WCHAR* sigStr = SignalQualityToString(item->signalQuality);
    WCHAR radioType[32];
    switch(item->dot11BssType) {
        case dot11_BSS_type_infrastructure: StringCchCopyW(radioType, 32, L"802.11n"); break;
        case dot11_BSS_type_independent:    StringCchCopyW(radioType, 32, L"802.11g"); break;
        default:                            StringCchCopyW(radioType, 32, L"802.11n"); break;
    }
    // USA LOC() - corretto per funzionare anche in auto-detect
    const WCHAR* statusText;
    if (item->isConnected) {
        statusText = LOC(STR_STATUS_CONNECTED);
    } else if (item->isConnecting) {
        statusText = LOC(STR_STATUS_CONNECTING);
    } else {
        statusText = LOC(STR_STATUS_NOT_CONNECTED);
    }
    StringCchPrintfW(g_TooltipBuffer, 1024,
        L"SSID: %s\n%s %s\n%s %s\n%s %s\n%s",
        ssidBuf,
        LOC(STR_SIGNAL_STRENGTH), sigStr,
        LOC(STR_SECURITY_TYPE),   securityType,
        LOC(STR_RADIO_TYPE),      radioType,
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
        if (g_hWndButtonConnect   && IsWindow(g_hWndButtonConnect))   ShowWindow(g_hWndButtonConnect,   SW_HIDE);
        if (g_hWndCheckboxConnect && IsWindow(g_hWndCheckboxConnect))  ShowWindow(g_hWndCheckboxConnect, SW_HIDE);
        return;
    }
    RECT rcRow;
    if (!GetRowRect(g_SelectedRowIndex, &rcRow)) {
        if (g_hWndButtonConnect   && IsWindow(g_hWndButtonConnect))   ShowWindow(g_hWndButtonConnect,   SW_HIDE);
        if (g_hWndCheckboxConnect && IsWindow(g_hWndCheckboxConnect))  ShowWindow(g_hWndCheckboxConnect, SW_HIDE);
        return;
    }
    WifiNetworkItem* item = &g_NetworkList[g_SelectedRowIndex];
    if (!item->isConnected && !item->isConnecting) {
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
    } else if (item->isConnecting) {
        if (g_hWndCheckboxConnect && IsWindow(g_hWndCheckboxConnect)) ShowWindow(g_hWndCheckboxConnect, SW_HIDE);
        if (g_hWndButtonConnect && IsWindow(g_hWndButtonConnect)) {
            MoveWindow(g_hWndButtonConnect, rcRow.right-90, rcRow.top+35, 82, 22, TRUE);
            SetWindowTextW(g_hWndButtonConnect, LOC(STR_CONNECTING));
            ShowWindow(g_hWndButtonConnect, SW_SHOW);
            EnableWindow(g_hWndButtonConnect, FALSE);
        }
    } else {
        if (g_hWndCheckboxConnect && IsWindow(g_hWndCheckboxConnect)) ShowWindow(g_hWndCheckboxConnect, SW_HIDE);
        if (g_hWndButtonConnect   && IsWindow(g_hWndButtonConnect)) {
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
    if (item->isConnected) {
        AppendMenuW(hMenu, MF_STRING, IDM_DISCONNECT, LOC(STR_CTX_DISCONNECT));
        AppendMenuW(hMenu, MF_STRING, IDM_STATUS,     LOC(STR_CTX_STATUS));
    } else if (item->isConnecting) {
        AppendMenuW(hMenu, MF_STRING | MF_GRAYED, IDM_CONNECT, LOC(STR_CONNECTING));
    } else {
        AppendMenuW(hMenu, MF_STRING, IDM_CONNECT, LOC(STR_CTX_CONNECT));
    }
    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMenu, MF_STRING, IDM_PROPERTIES, LOC(STR_CTX_PROPERTIES));
    int cmd = TrackPopupMenu(hMenu, TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, pt.x, pt.y, 0, hwnd, NULL);
    if (cmd > 0) {
        switch (cmd) {
        case IDM_CONNECT: case IDM_DISCONNECT:
            if (g_Ctx.hWlanClient) HandleNativeConnection(g_Ctx.hWlanClient, g_ContextMenuTargetIndex);
            break;
        case IDM_STATUS:
            if (g_ContextMenuTargetIndex >= 0 && g_ContextMenuTargetIndex < g_NetworkCount)
                OpenNetworkStatusForInterface(g_NetworkList[g_ContextMenuTargetIndex].interfaceGuid);
            ShowWindow(hwnd, SW_HIDE);
            break;
        case IDM_PROPERTIES:
            ShellExecuteW(NULL, L"open", L"control.exe", L"ncpa.cpl", NULL, SW_SHOWNORMAL);
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
// Window Procedure flyout
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
        Wh_Log(L"FlyoutWndProc - WM_CREATE");
        HMENU hSysMenu = GetSystemMenu(hwnd, FALSE);
        if (hSysMenu) RemoveMenu(hSysMenu, SC_CLOSE, MF_BYCOMMAND);
        g_hWndButtonConnect = CreateWindowExW(0, WC_BUTTONW, L"",
            WS_CHILD|BS_PUSHBUTTON, 0,0,0,0, hwnd,(HMENU)IDC_CONN_BUTTON,GetModuleHandle(NULL),NULL);
        SendMessageW(g_hWndButtonConnect,   WM_SETFONT,(WPARAM)g_hFontButton,  TRUE);
        g_hWndCheckboxConnect = CreateWindowExW(0, WC_BUTTONW, L"",
            WS_CHILD|BS_AUTOCHECKBOX, 0,0,0,0, hwnd,(HMENU)IDC_AUTO_CHECKBOX,GetModuleHandle(NULL),NULL);
        SendMessageW(g_hWndCheckboxConnect, WM_SETFONT,(WPARAM)g_hFontCheckbox,TRUE);
        SendMessageW(g_hWndCheckboxConnect, BM_SETCHECK,BST_CHECKED,0);
        RecalcArrowRect();
        InterlockedIncrement(&g_Ctx.refCount);
        InitTooltip(hwnd);
        if (g_Settings.refreshInterval > 0) {
            g_RefreshTimer = SetTimer(hwnd, 1000, g_Settings.refreshInterval, NULL);
            Wh_Log(L"Auto-refresh timer started: %d ms", g_Settings.refreshInterval);
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
                } else if (wParam == 1001) {
            // Timer di fallback per connessione/disconnessione
            if (!g_Ctx.hWlanClient) {
                if (g_ConnectCheckTimer) {
                    KillTimer(hwnd, g_ConnectCheckTimer);
                    g_ConnectCheckTimer = 0;
                }
                break;
            }
            
            g_ConnectRetryCount++;
            RefreshWifiData(g_Ctx.hWlanClient);
            
            if (g_ConnectRetryCount > 20) {  // 10 secondi max (20 x 500ms)
                Wh_Log(L"Connection operation timed out (fallback timer)");
                g_IsConnectingAsynchronously = FALSE;
                
                if (g_PendingConnectIndex >= 0 && g_PendingConnectIndex < g_NetworkCount) {
                    g_NetworkList[g_PendingConnectIndex].isConnecting = FALSE;
                    g_NetworkList[g_PendingConnectIndex].isDisconnecting = FALSE;
                }
                g_PendingConnectIndex = -1;
                g_ConnectRetryCount = 0;
                
                if (g_ConnectCheckTimer) {
                    KillTimer(hwnd, g_ConnectCheckTimer);
                    g_ConnectCheckTimer = 0;
                }
                
                MessageBoxW(hwnd, LOC(STR_TIMEOUT_ERROR), LOC(STR_ERROR_TITLE), MB_OK | MB_ICONERROR);
            }
            
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
    case WM_CTLCOLORSTATIC: {
        HWND hwndCtl = (HWND)lParam;
        HDC  hdc     = (HDC)wParam;
        if (hwndCtl == g_hWndCheckboxConnect) {
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(0,0,0));
            static HBRUSH hBrushRow = NULL;
            if (!hBrushRow) hBrushRow = CreateSolidBrush(RGB(228,241,252));
            return (INT_PTR)hBrushRow;
        }
        break;
    }
    case WM_CHECK_CONNECTION: {
        // Ferma il timer quando la connessione è completata
        if (g_ConnectCheckTimer && !g_IsConnectingAsynchronously) {
            KillTimer(hwnd, g_ConnectCheckTimer);
            g_ConnectCheckTimer = 0;
            g_ConnectRetryCount = 0;
            Wh_Log(L"Connection operation completed, timer stopped");
        }
        RefreshWifiData(g_Ctx.hWlanClient);
        UpdateLayoutGeometry();
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }
    // Aggiungi questa variabile globale
    static BOOL g_ConnectErrorShown = FALSE;

    // In HandleNativeConnection, quando avvii una nuova connessione:
    g_ConnectErrorShown = FALSE;  // Resetta il flag

    // Modifica WM_CONNECT_FAILED:
    case WM_CONNECT_FAILED: {
        if (SafeToAccessUI() && !g_ConnectErrorShown) {
        g_ConnectErrorShown = TRUE;
        MessageBoxW(hwnd, LOC(STR_PWD_FAILED_WRONG),
                  LOC(STR_PWD_FAILED_TITLE), MB_OK|MB_ICONERROR|MB_TOPMOST);
    }
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
                if (g_KeyboardSelectedIndex >= 0 && g_KeyboardSelectedIndex < g_NetworkCount && g_Ctx.hWlanClient)
                    HandleNativeConnection(g_Ctx.hWlanClient, g_KeyboardSelectedIndex);
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

        BOOL isAnyConnected = (g_NetworkCount > 0 && g_NetworkList[0].isConnected);
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

        if (g_IsHoveringRefresh) {
            HBRUSH hBrHov = CreateSolidBrush(RGB(200,225,245));
            HPEN hPenHov = CreatePen(PS_SOLID, 1, RGB(150,190,230));
            HPEN hOldPenHov = (HPEN)SelectObject(hdc, hPenHov);
            HBRUSH hOldBH = (HBRUSH)SelectObject(hdc, hBrHov);
            RoundRect(hdc, g_rcRefreshButton.left, g_rcRefreshButton.top,
                      g_rcRefreshButton.right, g_rcRefreshButton.bottom, 4, 4);
            SelectObject(hdc, hOldPenHov); DeleteObject(hPenHov);
            SelectObject(hdc, hOldBH); DeleteObject(hBrHov);
        }
        if (g_hIconRefreshWin7)
            DrawIconEx(hdc, g_rcRefreshButton.left+2, g_rcRefreshButton.top+3,
                       g_hIconRefreshWin7, 16, 16, 0, NULL, DI_NORMAL);

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
                
                if (g_NetworkList[i].isConnected) {
                    SelectObject(hdc, g_hFontBold); SetTextColor(hdc, RGB(0,0,0));
                    int textX = isSelected ? (rcRow.left+10)   : (rcRow.right-110);
                    int textY = isSelected ? (rcRow.top+22)    : (rcRow.top+6);
                    TextOutW(hdc, textX, textY, LOC(STR_CONNECTED_TEXT), lstrlenW(LOC(STR_CONNECTED_TEXT)));
                } else if (g_NetworkList[i].isConnecting) {
                    SelectObject(hdc, g_hFontNormal); SetTextColor(hdc, RGB(128,128,128));
                    int textX = isSelected ? (rcRow.left+10)   : (rcRow.right-110);
                    int textY = isSelected ? (rcRow.top+22)    : (rcRow.top+6);
                    TextOutW(hdc, textX, textY, LOC(STR_CONNECTING), lstrlenW(LOC(STR_CONNECTING)));
                }
                DrawNativeSignalIcon(hdc, rcRow.right-10, rcRow.top+2, g_NetworkList[i].signalQuality);
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
    case WM_REFRESH_DATA:
        if (g_Ctx.hWlanClient) {
            RefreshWifiData(g_Ctx.hWlanClient);
            UpdateLayoutGeometry();
            InvalidateRect(hwnd,NULL,TRUE);
        }
        break;
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
                ShowWindow(g_hWndButtonConnect,   SW_HIDE);
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
                g_SelectedRowIndex = (g_SelectedRowIndex==ci) ? -1 : ci;
                if (g_SelectedRowIndex >= 0)
                    SetKeyboardFocus(g_SelectedRowIndex);
                else
                    ClearKeyboardFocus();
                UpdateLayoutGeometry();
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
            if (g_Ctx.hWlanClient) HandleNativeConnection(g_Ctx.hWlanClient,g_SelectedRowIndex);
            break;
        }
        if (g_ContextMenuTargetIndex != -1) {
            switch (wid) {
            case IDM_CONNECT: case IDM_DISCONNECT:
                if (g_Ctx.hWlanClient) HandleNativeConnection(g_Ctx.hWlanClient,g_ContextMenuTargetIndex);
                break;
            case IDM_STATUS:
                if (g_ContextMenuTargetIndex >= 0 && g_ContextMenuTargetIndex < g_NetworkCount)
                    OpenNetworkStatusForInterface(g_NetworkList[g_ContextMenuTargetIndex].interfaceGuid);
                else
                    ShellExecuteW(NULL, L"open", L"control.exe", L"ncpa.cpl", NULL, SW_SHOWNORMAL);
                ShowWindow(hwnd, SW_HIDE);
                break;
            case IDM_PROPERTIES:
                if (g_ContextMenuTargetIndex >= 0 && g_ContextMenuTargetIndex < g_NetworkCount)
                    OpenNetworkPropertiesForInterface(g_NetworkList[g_ContextMenuTargetIndex].interfaceGuid);
                else
                    ShellExecuteW(NULL, L"open", L"control.exe", L"ncpa.cpl", NULL, SW_SHOWNORMAL);
                ShowWindow(hwnd, SW_HIDE);
                break;
            }
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
        if (g_RefreshTimer) {
            KillTimer(hwnd, g_RefreshTimer);
            g_RefreshTimer = 0;
        }
        if (g_ConnectCheckTimer) {
            KillTimer(hwnd, g_ConnectCheckTimer);
            g_ConnectCheckTimer = 0;
        }
        InterlockedDecrement(&g_Ctx.refCount);
        if (g_hTooltip) { DestroyWindow(g_hTooltip); g_hTooltip = NULL; }
        g_hWndFlyout = g_hWndButtonConnect = g_hWndCheckboxConnect = NULL;
        break;
    }
    
    return DefWindowProcW(hwnd,uMsg,wParam,lParam);
    
}

// -------------------------------------------------------
// ToolbarWindow32 subclassing
// -------------------------------------------------------
LRESULT CALLBACK ToolbarWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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
                                if (g_hWndFlyout && IsWindowVisible(g_hWndFlyout)) {
                                    Wh_Log(L"Network icon clicked while open — closing classic flyout");
                                    ToggleFlyoutWindow(); 
                                } else {
                                    Wh_Log(L"Network icon clicked while closed — opening classic flyout");
                                    SetTimer(hWnd, 9999, 10, NULL);
                                }
                            }
                        }
                        if (msg == WM_MOUSEACTIVATE) {
                            return MA_ACTIVATE;
                        }
                        return 0;
                    }
                }
            }
        }
    }
    if (msg == WM_TIMER && wParam == 9999) {
        KillTimer(hWnd, 9999);
        ToggleFlyoutWindow();
        return 0;
    }
    return CallWindowProcW(G_OldToolbarWndProc, hWnd, msg, wParam, lParam);
}

static bool IsExplorerProcess() {
    WCHAR exePath[MAX_PATH] = {};
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    WCHAR* name = wcsrchr(exePath, L'\\');
    name = name ? name + 1 : exePath;
    return _wcsicmp(name, L"explorer.exe") == 0;
}

void InstallTrayInterception() {
    Wh_Log(L"Installing ToolbarWindow32 interception...");
    if (!IsExplorerProcess()) {
        Wh_Log(L"Not explorer.exe - skipping ToolbarWindow32 subclassing");
        return;
    }
    HWND hTray = NULL;
    for (int attempt = 0; attempt < 10 && !hTray; attempt++) {
        hTray = FindWindowW(L"Shell_TrayWnd", NULL);
        if (!hTray) {
            Wh_Log(L"Shell_TrayWnd not found (attempt %d/10), waiting 500ms...", attempt + 1);
        }
    }
    if (!hTray) {
        Wh_Log(L"ERROR: Shell_TrayWnd not found after retries - tray subclassing skipped");
        return;
    }
    HWND hNotify  = FindWindowExW(hTray,    NULL, L"TrayNotifyWnd",   NULL);
    HWND hSysPager= hNotify ? FindWindowExW(hNotify,  NULL, L"SysPager",        NULL) : NULL;
    HWND hToolbar = hSysPager ? FindWindowExW(hSysPager,NULL, L"ToolbarWindow32", NULL) : NULL;
    HWND hTarget = hToolbar ? hToolbar : (hNotify ? hNotify : hTray);
    if (!hTarget) {
        Wh_Log(L"ERROR: tray hierarchy not found");
        return;
    }
    G_hSubclassedToolbar = hTarget;
    int nIndexSubclass = -4;
    G_OldToolbarWndProc = (WNDPROC)SetWindowLongPtrW(hTarget, nIndexSubclass, (LONG_PTR)ToolbarWndProc);
    if (G_OldToolbarWndProc)
        Wh_Log(L"ToolbarWindow32 subclassed OK (0x%p)", hTarget);
    else {
        Wh_Log(L"ERROR: subclassing failed");
        G_hSubclassedToolbar = nullptr;
    }
    if (g_Settings.redirectNetworkContextMenu) {
        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        if (hUser32) {
            void* pFn = (void*)GetProcAddress(hUser32, "TrackPopupMenuEx");
            if (pFn) {
                Wh_SetFunctionHook(pFn, (void*)TrackPopupMenuEx_Hook, (void**)&g_origTrackPopupMenuEx);
                Wh_Log(L"TrackPopupMenuEx hooked");
            }
        }
    }
    Wh_Log(L"Tray interception fully installed");
}

void RemoveTrayInterception() {
    if (G_hSubclassedToolbar && G_OldToolbarWndProc) {
        int nIndexSubclass = -4;
        SetWindowLongPtrW(G_hSubclassedToolbar, nIndexSubclass, (LONG_PTR)G_OldToolbarWndProc);
        Wh_Log(L"ToolbarWindow32 subclass removed");
        G_hSubclassedToolbar = nullptr;
        G_OldToolbarWndProc  = nullptr;
    }
}

// -------------------------------------------------------
// Toggle flyout
// -------------------------------------------------------
void ToggleFlyoutWindow() {
    Wh_Log(L"ToggleFlyoutWindow");
    EnterCriticalSection(&g_Ctx.csLock);
    if (!g_Ctx.isUninitializing) {
        if (!g_hWndFlyout || !IsWindow(g_hWndFlyout)) {
            HINSTANCE hInst = GetModuleHandle(NULL);
            WNDCLASSW wc = {0};
            wc.style         = 0;
            wc.lpfnWndProc   = FlyoutWndProc;
            wc.hInstance     = hInst;
            wc.lpszClassName = L"Win7NetworkFlyoutSafe";
            wc.hCursor       = LoadCursor(NULL,IDC_ARROW);
            wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
            UnregisterClassW(wc.lpszClassName,hInst);
            RegisterClassW(&wc);
            RECT rcClient = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
            AdjustWindowRectEx(&rcClient, WS_POPUP, FALSE, WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_LEFT);
            g_hWndFlyout = CreateWindowExW(
                WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_LEFT,
                wc.lpszClassName, L"", WS_POPUP,
                0, 0, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top,
                NULL, NULL, hInst, NULL);
            if (g_hWndFlyout) {
                SetWindowLongPtrW(g_hWndFlyout, GWL_STYLE,
                    GetWindowLongPtrW(g_hWndFlyout, GWL_STYLE) | WS_THICKFRAME);
                BOOL pfEnabled = FALSE;
                if (DwmIsCompositionEnabled(&pfEnabled) == S_OK && pfEnabled) {
                    MARGINS margins = {0, 0, 0, 1};
                    DwmExtendFrameIntoClientArea(g_hWndFlyout, &margins);
                }
            }
            Wh_Log(L"Flyout window created");
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
            ShowWindow(g_hWndButtonConnect,   SW_HIDE);
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
// Hotkey thread
// -------------------------------------------------------
DWORD WINAPI HotkeyThreadProc(LPVOID lpParam) {
    ModContext* ctx = (ModContext*)lpParam;
    if (!ctx) return 1;
    
    // Funzione helper per registrare/rimuovere l'hotkey
    auto UpdateHotkeyRegistration = [](BOOL shouldRegister) {
        // Prima rimuovi sempre (se esiste)
        UnregisterHotKey(NULL, HOTKEY_ID);
        
        if (shouldRegister) {
            if (!RegisterHotKey(NULL, HOTKEY_ID, MOD_CONTROL | MOD_NOREPEAT, 'H')) {
                Wh_Log(L"Failed to register hotkey - Ctrl+H may be in use");
            } else {
                Wh_Log(L"Hotkey registered - Ctrl+H");
            }
        } else {
            Wh_Log(L"Hotkey disabled by user setting");
        }
    };
    
    // Registra l'hotkey in base all'impostazione iniziale
    UpdateHotkeyRegistration(g_Settings.enableHotkey);
    
    UINT uTaskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
    MSG msg = {0};
    
    while (GetMessageW(&msg, NULL, 0, 0)) {
        if (msg.message == WM_HOTKEY && msg.wParam == HOTKEY_ID && !ctx->isUninitializing)
            ToggleFlyoutWindow();
        
        if (msg.message == WM_HOTKEY_SETTINGS_CHANGED) {
            // Ricarica le impostazioni e aggiorna la registrazione dell'hotkey
            Wh_Log(L"HotkeyThreadProc: Settings changed, updating hotkey registration");
            UpdateHotkeyRegistration(g_Settings.enableHotkey);
        }
        
        if (msg.message == uTaskbarCreated && !ctx->isUninitializing) {
            Wh_Log(L"WM_TASKBARCREATED received — reinstalling tray interception");
            if (G_hSubclassedToolbar) RemoveTrayInterception();
            InstallTrayInterception();
            // Re-registra l'hotkey dopo il riavvio di Explorer
            if (g_Settings.enableHotkey) {
                UpdateHotkeyRegistration(g_Settings.enableHotkey);
            }
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
    Wh_Log(L"SafeCleanup");
    if (InterlockedExchange(&g_Ctx.isUninitializing,1L)) return;
    RemoveTrayInterception();
    if (g_Ctx.dwHotkeyThreadId) PostThreadMessageW(g_Ctx.dwHotkeyThreadId,WM_QUIT,0,0);
    if (g_Ctx.hHotkeyThread) {
        WaitForSingleObject(g_Ctx.hHotkeyThread,3000);
        CloseHandle(g_Ctx.hHotkeyThread);
        g_Ctx.hHotkeyThread=NULL; g_Ctx.dwHotkeyThreadId=0;
    }
    if (g_hWndFlyout && IsWindow(g_hWndFlyout)) {
        BOOL pfEnabled = FALSE;
        if (DwmIsCompositionEnabled(&pfEnabled) == S_OK && pfEnabled) {
            MARGINS margins = {0, 0, 0, 0};
            DwmExtendFrameIntoClientArea(g_hWndFlyout, &margins);
        }
        SendMessageW(g_hWndFlyout,WM_SAFE_CLOSE,0,0);
        for (int i=0; i<50 && IsWindow(g_hWndFlyout); i++) {
            MSG msg;
            while (PeekMessageW(&msg,NULL,0,0,PM_REMOVE)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
        }
        if (IsWindow(g_hWndFlyout)) DestroyWindow(g_hWndFlyout);
    }
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
    Wh_Log(L"=== Wh_ModInit v1.1.7 ===");
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        Wh_Log(L"CoInitialize failed: 0x%08X", hr);
    }
    DetectWindowsVersion();
    LoadSettings();
    ZeroMemory(&g_Ctx,sizeof(g_Ctx));
    InitializeCriticalSection(&g_Ctx.csLock);
    DetermineLocale();
    HMODULE hAdvapi32 = GetModuleHandleW(L"advapi32.dll");
    if (hAdvapi32) {
        void* pRegQueryValueExW = (void*)GetProcAddress(hAdvapi32, "RegQueryValueExW");
        if (pRegQueryValueExW) {
            Wh_SetFunctionHook(pRegQueryValueExW, (void*)Hook_RegQueryValueExW, (void**)&Real_RegQueryValueExW);
            Wh_Log(L"RegQueryValueExW hooked");
        }
        void* pRegGetValueW = (void*)GetProcAddress(hAdvapi32, "RegGetValueW");
        if (pRegGetValueW) {
            Wh_SetFunctionHook(pRegGetValueW, (void*)Hook_RegGetValueW, (void**)&Real_RegGetValueW);
            Wh_Log(L"RegGetValueW hooked");
        }
    }
    if (!IsExplorerProcess()) {
        Wh_Log(L"Not explorer.exe - init skipped (only hook infrastructure runs)");
        InstallTrayInterception();
        g_Initialized = TRUE;
        Wh_Log(L"=== Wh_ModInit done (non-explorer) ===");
        return TRUE;
    }
    InitGlobalFonts();
    LoadSystemIcons();
    InitRefreshButtonRect();
    RecalcArrowRect();
    InstallTrayInterception();
    DWORD dwMaxClient=2, dwCurVer=0;
    if (WlanOpenHandle(dwMaxClient,NULL,&dwCurVer,&g_Ctx.hWlanClient)==ERROR_SUCCESS) {
        Wh_Log(L"WLAN OK");
        WlanRegisterNotification(g_Ctx.hWlanClient, WLAN_NOTIFICATION_SOURCE_ALL, TRUE,
                                 WlanNotificationCallback, &g_Ctx, NULL, NULL);
    } else {
        Wh_Log(L"WLAN init failed");
    }
    g_Ctx.hHotkeyThread = CreateThread(NULL,0,HotkeyThreadProc,&g_Ctx,0,&g_Ctx.dwHotkeyThreadId);
    if (!g_Ctx.hHotkeyThread) {
        Wh_Log(L"Hotkey thread failed");
        if (g_Ctx.hWlanClient) { WlanCloseHandle(g_Ctx.hWlanClient,NULL); g_Ctx.hWlanClient=NULL; }
        DeleteCriticalSection(&g_Ctx.csLock);
        CoUninitialize();
        return FALSE;
    }
    g_Initialized=TRUE;
    Wh_Log(L"=== Wh_ModInit done ===");
    return TRUE;
}

void Wh_ModSettingsChanged() {
    s_settingsSavedOnce = true;
    BOOL oldRegistryMethod = g_Settings.useRegistryMethod;  // Salva vecchio valore
    LoadSettings();
    DetermineLocale();
    
    // Se l'impostazione del registro è cambiata, logga il cambiamento
    if (oldRegistryMethod != g_Settings.useRegistryMethod) {
        Wh_Log(L"Registry hook %s", g_Settings.useRegistryMethod ? L"ACTIVE" : L"PASSIVE (transparent)");
    }
    
    // Notifica il thread dell'hotkey
    if (g_Ctx.dwHotkeyThreadId) {
        PostThreadMessageW(g_Ctx.dwHotkeyThreadId, WM_HOTKEY_SETTINGS_CHANGED, 0, 0);
    }
    
    if (SafeToAccessUI() && g_hWndFlyout) {
        if (IsWindow(g_hWndFlyout)) {
            PostMessageW(g_hWndFlyout, WM_REFRESH_DATA, 0, 0);
        }
        if (g_Settings.refreshInterval > 0) {
            g_RefreshTimer = SetTimer(g_hWndFlyout, 1000, g_Settings.refreshInterval, NULL);
            Wh_Log(L"Auto-refresh timer updated: %d ms", g_Settings.refreshInterval);
        }
        InvalidateRect(g_hWndFlyout, NULL, TRUE);
    }
}
void Wh_ModUninit() {
    Wh_Log(L"Wh_ModUninit");
    SafeCleanup();
    DeleteCriticalSection(&g_Ctx.csLock);
    CoUninitialize();
}
