// ==WindhawkMod==
// @id classic-taskbar-properties
// @name Classic Taskbar and Start Menu Properties
// @description This mod recreates the classic "Taskbar and Start Menu Properties" dialog from Windows 7 (Taskbar, Start Menu, Toolbars tabs) in Windows 10 and 11.
// @version 3.5.0
// @author babamohammed
// @github https://github.com/babamohammed2022
// @include explorer.exe
// @compilerOptions -lgdi32 -lcomctl32 -luser32 -lole32 -lshlwapi -lshell32 -luxtheme -ldwmapi -luuid -loleaut32 -lmsimg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Taskbar and Start Menu Properties

This Windhawk mod attempts to recreate the classic Windows 7 "Taskbar and Start Menu Properties" dialog
for Windows 10 and 11.
The mod has been tested on Windows 10 1809, Windows 10 21H2, Windows 11 23H2 and Windows 11 24H2.

## Screenshot (with the Aero theme)
![Screenshot](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win7classictaskbar.png)
## Screenshot (with the Classic theme)
![Screenshot](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/classichteme.PNG)
## Screenshot (with a Windows 8-like theme)
![Screenshot](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win81takbar4s.PNG)

**IMPORTANT**: This mod is a best-effort recreation that works within the limitations of modern Windows.
Some features may work partially or not at all depending on your system configuration,
Windows version, and installed modifications.

## What This Mod Does
- Lock/Unlock the taskbar
- Auto-hide the taskbar
- Use small icons/buttons
- Configure taskbar button grouping (Always combine, Combine when full, Never combine) - applies to
  every taskbar on every monitor, not just the primary one
- Configure Aero Peek
- Address/Links/Tablet PC toolbars (Desktop toolbar is currently not functional)
- Vertical taskbar support: only **Top** and **Bottom** positions are functional (Left and right positions could be implemented in future)
- Supports 5 languages (English, Italian, Spanish, French and Russian)

## What This Mod Tries To Do
- Start Menu customization (where supported by the OS)

## What This Mod Does NOT Do
- It cannot guarantee 100% original behavior from older Windows versions

## Known Issues
- **Left/Right taskbar position**: currently disabled (greyed out, non-selectable) in the
  position combo box. Only Top and Bottom rotation is supported for now; Left/Right requires
  more work due to the added complexity and may be added in a future version. In addition, in some configurations the taskbar can't be moved to the top if using the ExplorerPatcher taskbar.
- **Desktop toolbar**: The desktop toolbar is currently not available.
- **Some Start Menu settings**: May not work on all Windows versions as it heavily depends on user's configuration (for example OpenShell, Explorer7, ExplorerPatcher and other).
- **Tablet Input PC**: Works partially
## Credits 
- Anixx - Testing on Windows 11 23H2 with Classic Theme
- sebastian08dm08-cpu - Testing on Windows 10 1809
- SnowyCxmet - Fixed the Taskbar buttons combine setting (Always combine / Combine when full / Never combine) to make it apply to all monitors
- m417z - Code review
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Language
  $description: User interface language. Use "auto" for automatic detection.
  $options:
    - auto: Auto
    - en: English
    - it: Italiano
    - fr: Français
    - es: Español
    - ru: Русский
    - de: Deutsch
    - pt: Português
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <strsafe.h>
#include <uxtheme.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <dwmapi.h>
#include <servprov.h>
#include <ocidl.h>
#include <shlguid.h>
#include <objsafe.h>
#include <atomic>
#include <algorithm>
#include <vector>
#include <mutex>
#include <utility>
#define INITGUID
#include <shlobj.h>
#include <knownfolders.h>
#undef INITGUID
#include <objbase.h>

#ifndef SIID_TASKBAR
#define SIID_TASKBAR 39
#define IDC_PNL_BOTTOM 9001
#endif

namespace StuckRects {
    constexpr DWORD SETTINGS_EDGE_OFFSET = 12;
}

namespace DialogSizes {
    constexpr short MAIN_WIDTH = 262;
    constexpr short MAIN_HEIGHT = 271;
    constexpr short START_WIDTH = 252;
    constexpr short START_HEIGHT = 310;
}

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#ifndef ETDT_ENABLE
#define ETDT_ENABLE 0x00000002
#endif
#ifndef ETDT_USETABTEXTURE
#define ETDT_USETABTEXTURE 0x00000004
#endif
#ifndef ETDT_ENABLETAB
#define ETDT_ENABLETAB (ETDT_ENABLE | ETDT_USETABTEXTURE)
#endif

class ComInitializer {
public:
    ComInitializer() : m_initialized(false) {
        HRESULT hr = OleInitialize(NULL);
        m_initialized = SUCCEEDED(hr);
        if (!m_initialized) {
            Wh_Log(L"ComInitializer: OleInitialize failed with 0x%08X", hr);
        }
    }
    ~ComInitializer() {
        if (m_initialized) {
            OleUninitialize();
        }
    }
    bool IsInitialized() const { return m_initialized; }
    ComInitializer(const ComInitializer&) = delete;
    ComInitializer& operator=(const ComInitializer&) = delete;
private:
    bool m_initialized;
};

class RegKey {
public:
    RegKey() : m_hKey(NULL) {}
    explicit RegKey(HKEY hKey) : m_hKey(hKey) {}
    ~RegKey() { Close(); }
    
    bool Open(HKEY hRoot, LPCWSTR subKey, REGSAM samDesired = KEY_READ) {
        Close();
        return RegOpenKeyExW(hRoot, subKey, 0, samDesired, &m_hKey) == ERROR_SUCCESS;
    }
    
    bool Create(HKEY hRoot, LPCWSTR subKey, REGSAM samDesired = KEY_SET_VALUE | KEY_READ) {
        Close();
        DWORD disposition = 0;
        return RegCreateKeyExW(hRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, samDesired, NULL, &m_hKey, &disposition) == ERROR_SUCCESS;
    }
    
    DWORD GetDWord(LPCWSTR valueName, DWORD defaultValue = 0) const {
        if (!m_hKey) return defaultValue;
        DWORD value = defaultValue;
        DWORD size = sizeof(DWORD);
        RegQueryValueExW(m_hKey, valueName, NULL, NULL, (LPBYTE)&value, &size);
        return value;
    }
    
    bool SetDWord(LPCWSTR valueName, DWORD value) const {
        if (!m_hKey) return false;
        return RegSetValueExW(m_hKey, valueName, 0, REG_DWORD, (const BYTE*)&value, sizeof(DWORD)) == ERROR_SUCCESS;
    }
    
    bool QueryValue(LPCWSTR valueName, BYTE* data, DWORD* size) const {
        if (!m_hKey) return false;
        return RegQueryValueExW(m_hKey, valueName, NULL, NULL, data, size) == ERROR_SUCCESS;
    }
    
    bool DeleteValue(LPCWSTR valueName) const {
        if (!m_hKey) return false;
        return RegDeleteValueW(m_hKey, valueName) == ERROR_SUCCESS;
    }
    
    void Close() {
        if (m_hKey) {
            RegCloseKey(m_hKey);
            m_hKey = NULL;
        }
    }
    
    HKEY Get() const { return m_hKey; }
    operator HKEY() const { return m_hKey; }
    
    RegKey(const RegKey&) = delete;
    RegKey& operator=(const RegKey&) = delete;
    
private:
    HKEY m_hKey;
};

struct WindowsVersion {
    DWORD majorVersion;
    DWORD minorVersion;
    DWORD buildNumber;
    DWORD ubr;
    
    bool IsWindows10() const { return majorVersion == 10 && buildNumber >= 10240 && buildNumber < 22000; }
    bool IsWindows11() const { return majorVersion == 10 && buildNumber >= 22000; }
    
    bool IsSupported() const {
        return (majorVersion == 10 && buildNumber >= 10240) ||
               (majorVersion == 10 && buildNumber >= 22000);
    }
    
    bool IsBuildAtLeast(DWORD minBuild) const {
        return buildNumber >= minBuild;
    }
};

static WindowsVersion g_windowsVersion = {0, 0, 0, 0};

static bool GetWindowsVersion(WindowsVersion* outVersion) {
    if (!outVersion) return false;
    
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) return false;
    
    using RtlGetVersion_t = LONG (WINAPI*)(PRTL_OSVERSIONINFOW);
    auto RtlGetVersion = (RtlGetVersion_t)GetProcAddress(hNtdll, "RtlGetVersion");
    if (!RtlGetVersion) return false;
    
    RTL_OSVERSIONINFOW osvi = {};
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    if (RtlGetVersion(&osvi) != 0) return false;
    
    outVersion->majorVersion = osvi.dwMajorVersion;
    outVersion->minorVersion = osvi.dwMinorVersion;
    outVersion->buildNumber = osvi.dwBuildNumber;
    
    RegKey regKey;
    if (regKey.Open(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\NT\\CurrentVersion")) {
        outVersion->ubr = regKey.GetDWord(L"UBR", 0);
    }
    
    Wh_Log(L"Windows version: %lu.%lu.%lu (UBR: %lu)", 
           outVersion->majorVersion, outVersion->minorVersion, 
           outVersion->buildNumber, outVersion->ubr);
    
    return true;
}

static HANDLE g_hActCtx = INVALID_HANDLE_VALUE;

static void EnsureThemeActCtx() {
    if (g_hActCtx != INVALID_HANDLE_VALUE) return;
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) { g_hActCtx = INVALID_HANDLE_VALUE; return; }
    ACTCTXW actCtx = {};
    actCtx.cbSize = sizeof(actCtx);
    actCtx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_HMODULE_VALID;
    actCtx.hModule = hShell32;
    actCtx.lpResourceName = MAKEINTRESOURCEW(124);
    HANDLE h = CreateActCtxW(&actCtx);
    g_hActCtx = (h == INVALID_HANDLE_VALUE) ? INVALID_HANDLE_VALUE : h;
}

static BOOL CALLBACK ApplyExplorerThemeEnumProc(HWND hwnd, LPARAM lParam) {
    wchar_t cls[64] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (_wcsicmp(cls, L"SysTabControl32") == 0 || _wcsicmp(cls, L"SysListView32") == 0 ||
        _wcsicmp(cls, L"Button") == 0 || _wcsicmp(cls, L"ComboBox") == 0)
        SetWindowTheme(hwnd, L"Explorer", nullptr);
    return TRUE;
}

static void ApplyExplorerThemeToChildren(HWND hwndParent) {
    if (!IsWindow(hwndParent)) return;
    EnumChildWindows(hwndParent, ApplyExplorerThemeEnumProc, 0);
}

static void ApplyDarkTitlebar(HWND hwnd) {
    if (!IsWindow(hwnd)) return;
    RegKey regKey;
    BOOL useDark = FALSE;
    if (regKey.Open(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize")) {
        useDark = (regKey.GetDWord(L"AppsUseLightTheme", 1) == 0);
    }
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDark, sizeof(useDark));
}

static const HINSTANCE SHELL_EXECUTE_SUCCESS = (HINSTANCE)33;

static thread_local int g_hookDepth = 0;
struct HookGuard {
    HookGuard() { ++g_hookDepth; }
    ~HookGuard() { --g_hookDepth; }
    bool IsReentrant() const { return g_hookDepth > 1; }
};

// NOTE: the previous child-process redirect machinery (BuildChildEnvironment /
// g_childEnvBlock / IsChildProcess) was a leftover from the settings-to-control-panel
// base. g_childEnvBlock was built but never passed to any CreateProcess call, so
// IsChildProcess() was always false and the guards at the top of the ShellExecute*
// hooks were dead code (the hooks now open the dialog in-process). Removed.

static HWND g_hwndMain = NULL;
static HFONT g_hFontUi = NULL;
static LONG volatile g_dialogOpen = 0;
static int g_currentTab = 0;
static HWND g_hwndStartCustom = NULL;
static LONG volatile g_startCustomOpen = 0;
static HFONT g_hStartFontUi = NULL;
static HANDLE g_dialogThread = NULL;
static std::atomic<bool> g_modUnloading{false};

// ---------------------------------------------------------------------------
// Registro centrale di tutte le SetWindowSubclass installate dal mod.
//
// Perch esiste: la pulizia basata su EnumWindows (UnsubclassExistingTaskbars,
// il controllo sulla toolbar Desktop, ecc.) dipende dal *ritrovare* le finestre
// allo scarico. Se una finestra  nascosta, su un monitor secondario, su un
// desktop virtuale diverso, o smette di rispondere ai criteri di ricerca per
// una qualunque particolarit della configurazione dell'utente, quella pulizia
// la salta e la subclass resta agganciata a codice ormai scaricato -> crash di
// explorer.exe (0xc0000005) al primo messaggio ricevuto da quella finestra.
//
// Questo registro elimina il problema alla radice: ogni volta che il mod
// installa una subclass, la annota qui; ogni volta che la rimuove (in modo
// naturale, es. WM_NCDESTROY, o disattivando una feature) la toglie dal
// registro. Allo scarico del mod, invece di *cercare* le finestre, si scorre
// semplicemente questo elenco e si rimuove esattamente e solo ci che risulta
// ancora presente, ovunque si trovi. Nessuna finestra pu sfuggire alla pulizia
// indipendentemente da monitor, desktop virtuali, visibilit o versione di
// Windows.
static std::mutex g_subclassRegistryMutex;
static std::vector<std::pair<HWND, void*>> g_subclassRegistry;

// Template: le proc del mod hanno tutte 5 parametri (manca il dwRefData finale
// di SUBCLASSPROC, che ha 6 parametri), esattamente il motivo per cui il resto
// del file le forza con (SUBCLASSPROC)NomeProc quando le passa a
// RemoveWindowSubclass. Usando un template qui, ogni puntatore a funzione viene
// accettato cos com' e convertito internamente, senza dover mettere un cast
// esplicito a ogni singola chiamata di Track/Untrack.
template <typename ProcT>
static void TrackSubclass(HWND hWnd, ProcT proc) {
    if (!hWnd) return;
    void* key = reinterpret_cast<void*>(proc);
    std::lock_guard<std::mutex> lock(g_subclassRegistryMutex);
    for (auto& entry : g_subclassRegistry) {
        if (entry.first == hWnd && entry.second == key) return; // gi tracciata
    }
    g_subclassRegistry.emplace_back(hWnd, key);
}

template <typename ProcT>
static void UntrackSubclass(HWND hWnd, ProcT proc) {
    void* key = reinterpret_cast<void*>(proc);
    std::lock_guard<std::mutex> lock(g_subclassRegistryMutex);
    g_subclassRegistry.erase(
        std::remove_if(g_subclassRegistry.begin(), g_subclassRegistry.end(),
                        [&](const std::pair<HWND, void*>& entry) {
                            return entry.first == hWnd && entry.second == key;
                        }),
        g_subclassRegistry.end());
}

// Rimuove incondizionatamente ogni subclass ancora presente nel registro.
// Va chiamata come ultimo passo di Wh_ModUninit:  la rete di sicurezza finale,
// indipendente da qualunque logica di ricerca/enumerazione delle finestre.
static void RemoveAllTrackedSubclasses() {
    std::vector<std::pair<HWND, void*>> pending;
    {
        std::lock_guard<std::mutex> lock(g_subclassRegistryMutex);
        pending.swap(g_subclassRegistry);
    }
    for (auto& entry : pending) {
        if (entry.first && IsWindow(entry.first)) {
            // Wh_ModUninit runs on an arbitrary Windhawk thread, not the
            // thread that owns the window. Plain RemoveWindowSubclass fails
            // silently in that case (it only works from the window's own
            // thread), leaving the subclass pointing at the DLL image that's
            // about to be unmapped -> crash on the next message. Use the
            // any-thread-safe variant unconditionally here.
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(entry.first, (WindhawkUtils::WH_SUBCLASSPROC)entry.second);
        }
    }
}


using ShellExecuteExW_t = BOOL(WINAPI*)(SHELLEXECUTEINFOW*);
static ShellExecuteExW_t ShellExecuteExW_orig = nullptr;
using ShellExecuteW_t = HINSTANCE(WINAPI*)(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, INT);
static ShellExecuteW_t ShellExecuteW_orig = nullptr;

static constexpr LPCWSTR kAdvKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";
static constexpr LPCWSTR kPolicyKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer";
static constexpr LPCWSTR kStuckKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StuckRects3";
static constexpr LPCWSTR kSMKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer";
static constexpr LPCWSTR kStartFolderKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Start";

static const WCHAR* kAddrBandGUID = L"{01E04581-4EEE-11D0-BFE9-00AA005B4383}";
static const WCHAR* kLinksBandGUID = L"{4B5C7545-4275-11D1-B92B-00A0C90312E1}";
static const WCHAR* kDeskBandGUID  = L"{D82BE2B0-5764-11D0-A96E-00C04FD705A2}";

static const DWORD kPowerValues[] = { 2, 4, 16, 64, 256, 512, 1024 };

#define IDC_TAB_MAIN 3000
#define IDC_CHK_LOCK 3001
#define IDC_CHK_HIDE 3002
#define IDC_CHK_SMALL 3003
#define IDC_COMBO_LOCATION 3004
#define IDC_COMBO_BUTTONS 3005
#define IDC_BTN_CUST_NOTIF 3006
#define IDC_CHK_AEROPEEK 3007
#define IDC_LINK_HELP 3008
#define IDC_BTN_APPLY 3009
#define IDC_GRP_APPEARANCE 4001
#define IDC_GRP_NOTIF 4002
#define IDC_GRP_AERO 4003
#define IDC_TXT_LOCATION 4004
#define IDC_TXT_BUTTONS 4005
#define IDC_TXT_NOTIF 4006
#define IDC_TXT_AERO 4007
#define IDC_TXT_START_INFO 5001
#define IDC_BTN_START_CUST 5002
#define IDC_TXT_POWER_LABEL 5003
#define IDC_COMBO_POWER 5004
#define IDC_GRP_PRIVACY 5005
#define IDC_CHK_MRU_PROG 5006
#define IDC_CHK_MRU_ITEMS 5007
#define IDC_TXT_TOOLBARS_INFO 6001
#define IDC_LST_TOOLBARS 6002
#define IDC_START_GRP_TILES 7001
#define IDC_CHK_MORE_TILES 7002
#define IDC_CHK_APP_LIST 7003
#define IDC_CHK_RECENT_APPS 7004
#define IDC_CHK_FULLSCREEN 7005
#define IDC_CHK_RECENT_ITEMS 7006
#define IDC_CHK_ACCOUNT_NOTIF 7007
#define IDC_GRP_SEARCH 7008
#define IDC_CHK_SEARCH_PROGRAMS 7009
#define IDC_CHK_SEARCH_FILES 7010
#define IDC_START_GRP_FOLDERS 7011
#define IDC_CHK_FOLDER_SETTINGS 7012
#define IDC_CHK_FOLDER_DOCS 7013
#define IDC_CHK_FOLDER_DOWNLOADS 7014
#define IDC_CHK_FOLDER_MUSIC 7015
#define IDC_CHK_FOLDER_PICS 7016
#define IDC_CHK_FOLDER_VIDEOS 7017
#define IDC_CHK_FOLDER_NETWORK 7018
#define IDC_CHK_FOLDER_PERSONAL 7019
#define IDC_START_BTN_APPLY 7020
#define IDC_START_STATIC_INFO 7021

struct Strings {
    WCHAR title[128];
    WCHAR tab_taskbar[64], tab_start[64], tab_toolbars[64];
    WCHAR btn_ok[32], btn_cancel[32], btn_apply[32];
    WCHAR grp_appearance[64], chk_lock[64], chk_hide[64], chk_small[64];
    WCHAR txt_location[64], txt_buttons[64];
    WCHAR grp_notif[64], txt_notif[128], btn_cust_notif[32];
    WCHAR grp_aero[64], txt_aero[256], chk_aero[64], link_help[128];
    WCHAR pos_bottom[32], pos_left[32], pos_right[32], pos_top[32];
    WCHAR btn_always_combine[64], btn_combine_full[64], btn_never_combine[64];
    WCHAR start_info[256], btn_start_cust[32];
    WCHAR txt_power_label[64];
    WCHAR power_shutdown[32], power_restart[32], power_sleep[32], power_hibernate[32];
    WCHAR power_logoff[32], power_lock[32], power_switchuser[32];
    WCHAR grp_privacy[32];
    WCHAR chk_mru_prog[128], chk_mru_items[128];
    WCHAR toolbars_info[128];
    WCHAR toolbar_address[32], toolbar_links[32], toolbar_tabletpc[32], toolbar_desktop[32];
    WCHAR about_title[64], about_text[4096];
    WCHAR warn_position_title[64], warn_position_text[256];
    WCHAR start_custom_title[64];
    WCHAR start_grp_tiles[64];
    WCHAR start_chk_more_tiles[64], start_chk_app_list[64];
    WCHAR start_chk_recent_apps[64], start_chk_fullscreen[64];
    WCHAR start_chk_recent_items[64], start_chk_account_notif[64];
    WCHAR start_grp_search[64];
    WCHAR start_chk_search_programs[64], start_chk_search_files[64];
    WCHAR start_grp_folders[64];
    WCHAR start_chk_folder_settings[32], start_chk_folder_docs[32];
    WCHAR start_chk_folder_downloads[32], start_chk_folder_music[32];
    WCHAR start_chk_folder_pics[32], start_chk_folder_videos[32];
    WCHAR start_chk_folder_network[32], start_chk_folder_personal[32];
    WCHAR start_info_restart[256];
    WCHAR start_msg_saved[512], start_msg_saved_title[64];
};

static Strings g_str;
static WCHAR g_language[8] = L"auto";

struct StartSetting {
    LPCWSTR regKey;
    LPCWSTR regValue;
    LPCWSTR policyValue;
    DWORD defaultValue;
    bool invertedLogic;
    int controlId;
};

struct StartFolder {
    LPCWSTR folderRegValue;
    int controlId;
};

struct DialogControlBinding {
    int controlId;
    const WCHAR* text;
};

static const StartSetting g_startSettings[] = {
    { kAdvKey, L"Start_ShowMoreTiles", NULL, 0, false, IDC_CHK_MORE_TILES },
    { kAdvKey, L"Start_ShowAppList", NULL, 1, false, IDC_CHK_APP_LIST },
    { kAdvKey, L"Start_TrackProgs", L"ClearRecentProgHistory", 1, false, IDC_CHK_RECENT_APPS },
    { kAdvKey, L"ForceStartSize", NULL, 0, false, IDC_CHK_FULLSCREEN },
    { kAdvKey, L"Start_TrackDocs", L"ClearRecentDocsHistory", 1, false, IDC_CHK_RECENT_ITEMS },
    { kAdvKey, L"Start_NotifyNewApps", NULL, 1, false, IDC_CHK_ACCOUNT_NOTIF },
    { kAdvKey, L"Start_SearchPrograms", NULL, 1, false, IDC_CHK_SEARCH_PROGRAMS },
    { kAdvKey, L"Start_SearchFiles", NULL, 1, false, IDC_CHK_SEARCH_FILES },
};

static const StartFolder g_startFolders[] = {
    { L"SettingsVisibility", IDC_CHK_FOLDER_SETTINGS },
    { L"DocumentsVisibility", IDC_CHK_FOLDER_DOCS },
    { L"DownloadsVisibility", IDC_CHK_FOLDER_DOWNLOADS },
    { L"MusicVisibility", IDC_CHK_FOLDER_MUSIC },
    { L"PicturesVisibility", IDC_CHK_FOLDER_PICS },
    { L"VideosVisibility", IDC_CHK_FOLDER_VIDEOS },
    { L"NetworkVisibility", IDC_CHK_FOLDER_NETWORK },
    { L"UserFolderVisibility", IDC_CHK_FOLDER_PERSONAL },
};

static void ShowTaskbarProperties();
static void ShowStartCustomDialog(HWND parent);
static INT_PTR CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
static INT_PTR CALLBACK StartCustomDlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
static DWORD WINAPI DialogThreadProc(LPVOID);
static HWND FindDesktopToolbarWindow();
static void ApplyClassicDesktopLayout(HWND hBand);
static LRESULT CALLBACK DesktopBandSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR uIdSubclass);
static bool IsEdgeVertical(DWORD edge);
class TaskbarSettingsProvider {
public:
    static DWORD RegGetDWordSafe(HKEY hRoot, LPCWSTR sub, LPCWSTR name, DWORD def) {
        RegKey regKey;
        if (!regKey.Open(hRoot, sub)) return def;
        return regKey.GetDWord(name, def);
    }

    static bool RegSetDWordSafe(HKEY hRoot, LPCWSTR sub, LPCWSTR name, DWORD v) {
        RegKey regKey;
        if (!regKey.Create(hRoot, sub)) return false;
        return regKey.SetDWord(name, v);
    }

    static bool GetLockState() { return RegGetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarSizeMove", 1) == 0; }
    static void SetLockState(bool lock) { RegSetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarSizeMove", lock ? 0 : 1); }
    static bool GetSmallIcons() { return RegGetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarSmallIcons", 0) != 0; }
    static void SetSmallIcons(bool s) { RegSetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarSmallIcons", s ? 1 : 0); }
    static DWORD GetGlomLevel() { return RegGetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarGlomLevel", 0); }

    // Windows keeps taskbar button combining as TWO separate registry values:
    // "TaskbarGlomLevel" governs the taskbar on the PRIMARY monitor, while
    // "MMTaskbarGlomLevel" governs every taskbar on SECONDARY monitors. Only
    // ever writing the first one (the original behavior here) meant the combo
    // box worked on monitor 1 but silently did nothing on monitor 2+. Both
    // values are now written together so the setting is uniform across every
    // taskbar regardless of monitor.
    static void SetGlomLevel(DWORD lvl) {
        RegSetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarGlomLevel", lvl);
        RegSetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"MMTaskbarGlomLevel", lvl);
    }

    static DWORD GetTaskbarEdge() {
        DWORD edge = 3;
        RegKey regKey;
        if (regKey.Open(HKEY_CURRENT_USER, kStuckKey)) {
            DWORD sz = 0;
            if (regKey.QueryValue(L"Settings", NULL, &sz) && sz >= StuckRects::SETTINGS_EDGE_OFFSET + sizeof(DWORD)) {
                std::vector<BYTE> d(sz);
                if (regKey.QueryValue(L"Settings", d.data(), &sz)) {
                    edge = *reinterpret_cast<DWORD*>(&d[StuckRects::SETTINGS_EDGE_OFFSET]);
                    if (edge > 3) edge = 3;
                }
            }
        }
        return edge;
    }

    static bool GetAeroPeekEnabled() {
        return RegGetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"DisablePreviewDesktop", 0) == 0;
    }
    static void SetAeroPeekEnabled(bool e) {
        RegSetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"DisablePreviewDesktop", e ? 0 : 1);
    }

    static DWORD GetPowerAction() { return RegGetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"Start_PowerButtonAction", 2); }
    static void SetPowerAction(DWORD v) { RegSetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"Start_PowerButtonAction", v); }

    static bool GetStartMruProgs() { return RegGetDWordSafe(HKEY_CURRENT_USER, kSMKey, L"Start_TrackProgs", 1) != 0; }
    static bool GetStartMruItems() { return RegGetDWordSafe(HKEY_CURRENT_USER, kSMKey, L"Start_TrackDocs", 1) != 0; }
    static void SetStartMruProgs(bool v) { RegSetDWordSafe(HKEY_CURRENT_USER, kSMKey, L"Start_TrackProgs", v ? 1 : 0); }
    static void SetStartMruItems(bool v) { RegSetDWordSafe(HKEY_CURRENT_USER, kSMKey, L"Start_TrackDocs", v ? 1 : 0); }

    static bool GetStartFolderVisible(LPCWSTR valueName) {
        return RegGetDWordSafe(HKEY_CURRENT_USER, kStartFolderKey, valueName, 1) != 0;
    }
    static void SetStartFolderVisible(LPCWSTR valueName, bool visible) {
        RegSetDWordSafe(HKEY_CURRENT_USER, kStartFolderKey, valueName, visible ? 1 : 0);
    }

    static bool GetToolbarEnabled(LPCWSTR name) {
        if (wcscmp(name, L"TabletPC") == 0)
            return RegGetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\TabletTip\\1.7", L"TipbandDesiredVisibility", 0) != 0;
        const WCHAR* guid = nullptr;
        if (wcscmp(name, L"Address") == 0) guid = kAddrBandGUID;
        else if (wcscmp(name, L"Links") == 0) guid = kLinksBandGUID;
        else if (wcscmp(name, L"Desktop") == 0) guid = kDeskBandGUID;
        else return false;
        RegKey regKey;
        if (regKey.Open(HKEY_CURRENT_USER, L"Software\\Microsoft\\Internet Explorer\\Toolbar")) {
            DWORD sz = 0;
            return regKey.QueryValue(guid, NULL, &sz);
        }
        return false;
    }

    static void SetToolbarEnabled(LPCWSTR name, bool enable) {
        if (wcscmp(name, L"TabletPC") == 0) {
            RegSetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\TabletTip\\1.7", L"TipbandDesiredVisibility", enable ? 1 : 0);
            return;
        }
        const WCHAR* guid = nullptr;
        if (wcscmp(name, L"Address") == 0) guid = kAddrBandGUID;
        else if (wcscmp(name, L"Links") == 0) guid = kLinksBandGUID;
        else if (wcscmp(name, L"Desktop") == 0) guid = kDeskBandGUID;
        else return;
        RegKey regKey;
        if (regKey.Create(HKEY_CURRENT_USER, L"Software\\Microsoft\\Internet Explorer\\Toolbar")) {
            if (enable) regKey.SetDWord(guid, 0);
            else regKey.DeleteValue(guid);
        }
    }
};

static bool IsSettingLockedByPolicy(LPCWSTR policyValue) {
    if (!policyValue) return false;
    return TaskbarSettingsProvider::RegGetDWordSafe(HKEY_CURRENT_USER, kPolicyKey, policyValue, 0) != 0
        || TaskbarSettingsProvider::RegGetDWordSafe(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", policyValue, 0) != 0;
}

static bool GetStartSettingState(const StartSetting& s) {
    if (s.policyValue) {
        DWORD pol = TaskbarSettingsProvider::RegGetDWordSafe(HKEY_CURRENT_USER, kPolicyKey, s.policyValue, 0);
        if (!pol) pol = TaskbarSettingsProvider::RegGetDWordSafe(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", s.policyValue, 0);
        if (pol) return false;
    }
    DWORD v = TaskbarSettingsProvider::RegGetDWordSafe(HKEY_CURRENT_USER, s.regKey, s.regValue, s.defaultValue);
    return s.invertedLogic ? (v == 0) : (v != 0);
}

static void SetStartSettingState(const StartSetting& s, bool enabled) {
    if (s.policyValue && IsSettingLockedByPolicy(s.policyValue)) return;
    DWORD v = s.invertedLogic ? (enabled ? 0 : 1) : (enabled ? 1 : 0);
    TaskbarSettingsProvider::RegSetDWordSafe(HKEY_CURRENT_USER, s.regKey, s.regValue, v);
}

static HICON GetSystemIcon(int iconId) {
    SHSTOCKICONINFO info = {};
    info.cbSize = sizeof(info);
    if (SHGetStockIconInfo((SHSTOCKICONID)iconId, SHGSI_ICON | SHGSI_SMALLICON, &info) == S_OK)
        return info.hIcon;
    return NULL;
}

static BOOL CALLBACK SetFontChildProc(HWND hwnd, LPARAM lParam) {
    if (!IsWindow(hwnd)) return TRUE;
    SendMessageW(hwnd, WM_SETFONT, (WPARAM)lParam, TRUE);
    return TRUE;
}

static const CLSID CLSID_CTP_AddressBand =
    {0x01E04581,0x4EEE,0x11D0,{0xBF,0xE9,0x00,0xAA,0x00,0x5B,0x43,0x83}};
static const CLSID CLSID_CTP_LinksBand =
    {0x0E5CBF21,0xD15F,0x11D0,{0x83,0x01,0x00,0xAA,0x00,0x5B,0x43,0x83}};
static const CLSID CLSID_CTP_TrayDeskBand =
    {0xE6442437,0x6C68,0x4F52,{0x94,0xDD,0x2C,0xFE,0xD2,0x67,0xEF,0xB9}};
static const IID IID_CTP_ITrayDeskBand =
    {0x6D67E846,0x5B9C,0x4DB8,{0x9C,0xBC,0xDD,0xE1,0x2F,0x42,0x54,0xF1}};

struct CTP_ITrayDeskBand : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE ShowDeskBand(REFCLSID clsid) = 0;
    virtual HRESULT STDMETHODCALLTYPE HideDeskBand(REFCLSID clsid) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsDeskBandShown(REFCLSID clsid) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeskBandRegistrationChanged() = 0;
};

enum CtpBandOp { CTP_BAND_HIDE = 0, CTP_BAND_SHOW = 1, CTP_BAND_QUERY = 2 };

static bool NativeDeskBandOp(const CLSID& band, CtpBandOp op, bool* pShown) {
    ComInitializer comInit;
    if (!comInit.IsInitialized()) {
        Wh_Log(L"NativeDeskBandOp: COM initialization failed");
        return false;
    }
    
    bool ok = false;
    CTP_ITrayDeskBand* pTray = nullptr;
    
    HRESULT hr = CoCreateInstance(CLSID_CTP_TrayDeskBand, NULL,
        CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
        IID_CTP_ITrayDeskBand, (void**)&pTray);
    
    if (SUCCEEDED(hr) && pTray) {
        switch (op) {
        case CTP_BAND_SHOW: {
            HRESULT r = pTray->ShowDeskBand(band);
            ok = SUCCEEDED(r);
            if (!ok) Wh_Log(L"NativeDeskBandOp: ShowDeskBand failed with 0x%08X", r);
            break;
        }
        case CTP_BAND_HIDE: {
            HRESULT r = pTray->HideDeskBand(band);
            ok = SUCCEEDED(r);
            if (!ok) Wh_Log(L"NativeDeskBandOp: HideDeskBand failed with 0x%08X", r);
            break;
        }
        case CTP_BAND_QUERY: {
            HRESULT q = pTray->IsDeskBandShown(band);
            if (pShown) *pShown = (q == S_OK);
            ok = SUCCEEDED(q);
            break;
        }
        }
        pTray->Release();
    } else {
        Wh_Log(L"NativeDeskBandOp: CoCreateInstance failed with 0x%08X", hr);
    }
    return ok;
}

static void RefreshNativeDeskBandRegistration() {
    ComInitializer comInit;
    if (!comInit.IsInitialized()) {
        Wh_Log(L"RefreshNativeDeskBandRegistration: COM initialization failed");
        return;
    }
    
    CTP_ITrayDeskBand* pTray = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_CTP_TrayDeskBand, NULL,
        CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
        IID_CTP_ITrayDeskBand, (void**)&pTray);
    if (SUCCEEDED(hr) && pTray) {
        hr = pTray->DeskBandRegistrationChanged();
        if (FAILED(hr)) {
            Wh_Log(L"RefreshNativeDeskBandRegistration: DeskBandRegistrationChanged failed with 0x%08X", hr);
        }
        pTray->Release();
    } else {
        Wh_Log(L"RefreshNativeDeskBandRegistration: CoCreateInstance failed with 0x%08X", hr);
    }
}

static void SetFontAllChildren(HWND hwnd, HFONT hf) {
    if (!IsWindow(hwnd)) return;
    EnumChildWindows(hwnd, SetFontChildProc, (LPARAM)hf);
}

static const CLSID CLSID_DesktopBand =
    {0xD82BE2B0,0x5764,0x11D0,{0xA9,0x6E,0x00,0xC0,0x4F,0xD7,0x05,0xA2}};

static bool IsNativeDesktopToolbarShown() {
    // Prima controlla se la finestra  gi presente nella gerarchia.
    HWND hBand = FindDesktopToolbarWindow();
    if (hBand && IsWindow(hBand)) return true;

    // In alternativa interroga ITrayDeskBand (pu restituire true anche se la
    // finestra non  ancora stata trovata, ad es. subito dopo l'abilitazione).
    bool shown = false;
    if (NativeDeskBandOp(CLSID_DesktopBand, CTP_BAND_QUERY, &shown)) {
        return shown;
    }

    // Fallback: leggi il registro (compatibilit con versioni precedenti).
    return TaskbarSettingsProvider::GetToolbarEnabled(L"Desktop");
}
// Trova la finestra della Desktop Band (approccio ibrido)
static HWND FindDesktopToolbarWindow() {
    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!hTray || !IsWindow(hTray)) return NULL;
    
    // Cerca nel ReBar (approccio Win2003: la band  un figlio del ReBar)
    HWND hReBar = FindWindowExW(hTray, NULL, L"ReBarWindow32", NULL);
    if (hReBar && IsWindow(hReBar)) {
        HWND hBand = FindWindowExW(hReBar, NULL, L"ToolbarWindow32", L"Desktop");
        if (hBand && IsWindow(hBand)) return hBand;
    }
    
    // Fallback: cerca direttamente (approccio moderno)
    HWND hBand = FindWindowExW(hTray, NULL, L"ToolbarWindow32", L"Desktop");
    if (hBand && IsWindow(hBand)) return hBand;
    
    return NULL;
}

// Applica il layout classico (da Win2003)
static void ApplyClassicDesktopLayout(HWND hBand) {
    if (!hBand || !IsWindow(hBand)) return;
    
    // Forza il ridimensionamento per mostrare il contenuto del desktop
    RECT rc;
    GetWindowRect(hBand, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    
    // Se la taskbar  verticale (sinistra/destra), la band deve essere pi alta
    DWORD edge = TaskbarSettingsProvider::GetTaskbarEdge();
    if (edge == 0 || edge == 2) { // Sinistra o Destra
        // Forza layout verticale (come Win2003)
        SetWindowPos(hBand, NULL, 0, 0, width, std::max(height, 200), 
                     SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
    
    // Aggiorna la finestra
    InvalidateRect(hBand, NULL, TRUE);
    UpdateWindow(hBand);
}

// Subclass per la Desktop Band (comportamento classico)
static LRESULT CALLBACK DesktopBandSubclassProc(HWND hWnd, UINT uMsg, 
    WPARAM wParam, LPARAM lParam, DWORD_PTR uIdSubclass) {
    
    switch (uMsg) {
        case WM_SIZE: {
            // Comportamento Win2003: quando la band viene ridimensionata,
            // il contenuto del desktop si adatta
            if (IsEdgeVertical(TaskbarSettingsProvider::GetTaskbarEdge())) {
                // Layout verticale (taskbar a sinistra/destra)
                RECT rc;
                GetClientRect(hWnd, &rc);
                // Forza il refresh del contenuto
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
        }
        case WM_CONTEXTMENU: {
            // Comportamento Win2003: menu contestuale del desktop
            // Permette di aprire il desktop in Esplora File
            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            
            HMENU hMenu = CreatePopupMenu();
            AppendMenuW(hMenu, MF_STRING, 1, L"Apri desktop");
            AppendMenuW(hMenu, MF_STRING, 2, L"Visualizza come");
            AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenuW(hMenu, MF_STRING, 3, L"Aggiorna");
            
            int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, 0, hWnd, NULL);
            DestroyMenu(hMenu);
            
            if (cmd == 1) {
                // Apri il desktop in Esplora File
                ShellExecuteW(hWnd, L"open", L"explorer.exe", 
                              L"shell:Desktop", NULL, SW_SHOWNORMAL);
            } else if (cmd == 3) {
                // Aggiorna il desktop
                InvalidateRect(hWnd, NULL, TRUE);
                UpdateWindow(hWnd);
            }
            return 0;
        }
        case WM_NCDESTROY:
            RemoveWindowSubclass(hWnd, (SUBCLASSPROC)DesktopBandSubclassProc, uIdSubclass);
            UntrackSubclass(hWnd, DesktopBandSubclassProc);
            break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Helper per verificare se la taskbar  verticale
static bool IsEdgeVertical(DWORD edge) {
    return edge == 0 || edge == 2; // Sinistra o Destra
}
static bool ShowNativeDesktopToolbar() {
    Wh_Log(L"[DesktopToolbar] Show requested");

    // Passo 1: usa ITrayDeskBand::ShowDeskBand  esattamente lo stesso percorso
    // usato per Address e Links. SHLoadInProc  uno stub su Win10/11 e non
    // attiva nulla; il percorso COM/ITrayDeskBand  quello ancora funzionante.
    bool result = NativeDeskBandOp(CLSID_DesktopBand, CTP_BAND_SHOW, NULL);

    if (result) {
        Wh_Log(L"[DesktopToolbar] ShowDeskBand(CLSID_DesktopBand) riuscito");
    } else {
        Wh_Log(L"[DesktopToolbar] ShowDeskBand fallito, tento fallback registro");
    }

    // Passo 2: aggiorna il registro (sia per lo stato che per il fallback).
    TaskbarSettingsProvider::SetToolbarEnabled(L"Desktop", true);

    // Passo 3: notifica la taskbar.
    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hTray && IsWindow(hTray)) {
        SendNotifyMessageW(hTray, WM_SETTINGCHANGE, 0, (LPARAM)L"Taskbar");
    }
    SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Taskbar");

    // Passo 4: aspetta che la finestra appaia e applica il subclassing.
    // La band potrebbe non essere ancora visibile subito dopo ShowDeskBand,
    // proviamo fino a 10 volte con 50 ms di intervallo.
    HWND hBand = NULL;
    for (int i = 0; i < 10 && !hBand; i++) {
        hBand = FindDesktopToolbarWindow();
        if (!hBand) Sleep(50);
    }

    if (hBand && IsWindow(hBand)) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hBand, DesktopBandSubclassProc, 0);
        TrackSubclass(hBand, DesktopBandSubclassProc);
        Wh_Log(L"[DesktopToolbar] Desktop band trovata e subclassata");
        ApplyClassicDesktopLayout(hBand);
    } else {
        Wh_Log(L"[DesktopToolbar] Desktop band non trovata dopo ShowDeskBand");
    }

    return result;
}

static bool HideNativeDesktopToolbar() {
    Wh_Log(L"[DesktopToolbar] Hide requested");

    // Rimuovi il subclassing prima di nascondere la finestra.
    HWND hBand = FindDesktopToolbarWindow();
    if (hBand && IsWindow(hBand)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hBand,
            (WindhawkUtils::WH_SUBCLASSPROC)DesktopBandSubclassProc);
        UntrackSubclass(hBand, DesktopBandSubclassProc);
        Wh_Log(L"[DesktopToolbar] Desktop band unsubclassata");
    }

    // Usa ITrayDeskBand::HideDeskBand  simmetrico rispetto a ShowDeskBand.
    bool result = NativeDeskBandOp(CLSID_DesktopBand, CTP_BAND_HIDE, NULL);
    if (!result) {
        Wh_Log(L"[DesktopToolbar] HideDeskBand fallito (non fatale)");
    }

    TaskbarSettingsProvider::SetToolbarEnabled(L"Desktop", false);

    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hTray && IsWindow(hTray)) {
        SendNotifyMessageW(hTray, WM_SETTINGCHANGE, 0, (LPARAM)L"Taskbar");
    }
    SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Taskbar");
    return true;
}

static void InitLocalization() {
    enum Lang { LANG_EN, LANG_IT, LANG_FR, LANG_ES, LANG_RU, LANG_DE, LANG_PT };
    Lang lang = LANG_EN;
    if (wcscmp(g_language, L"it") == 0) lang = LANG_IT;
    else if (wcscmp(g_language, L"fr") == 0) lang = LANG_FR;
    else if (wcscmp(g_language, L"es") == 0) lang = LANG_ES;
    else if (wcscmp(g_language, L"ru") == 0) lang = LANG_RU;
    else if (wcscmp(g_language, L"de") == 0) lang = LANG_DE;
    else if (wcscmp(g_language, L"pt") == 0) lang = LANG_PT;
    else if (wcscmp(g_language, L"en") == 0) lang = LANG_EN;
    else {
        DWORD uiLang = GetUserDefaultUILanguage() & 0xFF;
        if (uiLang == 0x10) lang = LANG_IT;
        else if (uiLang == 0x0C) lang = LANG_FR;
        else if (uiLang == 0x0A) lang = LANG_ES;
        else if (uiLang == 0x19) lang = LANG_RU;
        else if (uiLang == 0x07) lang = LANG_DE;
        else if (uiLang == 0x16) lang = LANG_PT;
    }

    StringCchCopyW(g_str.btn_ok, 32, L"OK");

    switch (lang) {
        case LANG_IT: StringCchCopyW(g_str.tab_taskbar, 64, L"Barra delle applicazioni"); StringCchCopyW(g_str.tab_start, 64, L"Menu Start"); StringCchCopyW(g_str.tab_toolbars, 64, L"Barre degli strumenti"); break;
        case LANG_FR: StringCchCopyW(g_str.tab_taskbar, 64, L"Barre des t\u00e2ches"); StringCchCopyW(g_str.tab_start, 64, L"Menu D\u00e9marrer"); StringCchCopyW(g_str.tab_toolbars, 64, L"Barres d'outils"); break;
        case LANG_ES: StringCchCopyW(g_str.tab_taskbar, 64, L"Barra de tareas"); StringCchCopyW(g_str.tab_start, 64, L"Inicio"); StringCchCopyW(g_str.tab_toolbars, 64, L"Barras de herramientas"); break;
        case LANG_RU: StringCchCopyW(g_str.tab_taskbar, 64, L"\u041f\u0430\u043d\u0435\u043b\u044c \u0437\u0430\u0434\u0430\u0447"); StringCchCopyW(g_str.tab_start, 64, L"\u041c\u0435\u043d\u044e \u041f\u0443\u0441\u043a"); StringCchCopyW(g_str.tab_toolbars, 64, L"\u041f\u0430\u043d\u0435\u043b\u0438 \u0438\u043d\u0441\u0442\u0440\u0443\u043c\u0435\u043d\u0442\u043e\u0432"); break;
        case LANG_DE: StringCchCopyW(g_str.tab_taskbar, 64, L"Taskleiste"); StringCchCopyW(g_str.tab_start, 64, L"Startmen\u00fc"); StringCchCopyW(g_str.tab_toolbars, 64, L"Symbolleisten"); break;
        case LANG_PT: StringCchCopyW(g_str.tab_taskbar, 64, L"Barra de tarefas"); StringCchCopyW(g_str.tab_start, 64, L"Menu Iniciar"); StringCchCopyW(g_str.tab_toolbars, 64, L"Barras de ferramentas"); break;
        default: StringCchCopyW(g_str.tab_taskbar, 64, L"Taskbar"); StringCchCopyW(g_str.tab_start, 64, L"Start Menu"); StringCchCopyW(g_str.tab_toolbars, 64, L"Toolbars");
    }

    switch (lang) {
        case LANG_IT: StringCchCopyW(g_str.title, 128, L"Propriet\u00e0 della barra delle applicazioni e del menu Start"); break;
        case LANG_FR: StringCchCopyW(g_str.title, 128, L"Propri\u00e9t\u00e9s de la barre des t\u00e2ches et du menu D\u00e9marrer"); break;
        case LANG_ES: StringCchCopyW(g_str.title, 128, L"Propiedades de la barra de tareas e Inicio"); break;
        case LANG_RU: StringCchCopyW(g_str.title, 128, L"\u0421\u0432\u043e\u0439\u0441\u0442\u0432\u0430 \u043f\u0430\u043d\u0435\u043b\u0438 \u0437\u0430\u0434\u0430\u0447 \u0438 \u043c\u0435\u043d\u044e \u041f\u0443\u0441\u043a"); break;
        case LANG_DE: StringCchCopyW(g_str.title, 128, L"Eigenschaften von Taskleiste und Startmen\u00fc"); break;
        case LANG_PT: StringCchCopyW(g_str.title, 128, L"Propriedades da Barra de Tarefas e do Menu Iniciar"); break;
        default: StringCchCopyW(g_str.title, 128, L"Taskbar and Start Menu Properties");
    }

    switch (lang) {
        case LANG_IT: StringCchCopyW(g_str.btn_cancel, 32, L"Annulla"); StringCchCopyW(g_str.btn_apply, 32, L"Applica"); break;
        case LANG_FR: StringCchCopyW(g_str.btn_cancel, 32, L"Annuler"); StringCchCopyW(g_str.btn_apply, 32, L"Appliquer"); break;
        case LANG_ES: StringCchCopyW(g_str.btn_cancel, 32, L"Cancelar"); StringCchCopyW(g_str.btn_apply, 32, L"Aplicar"); break;
        case LANG_RU: StringCchCopyW(g_str.btn_cancel, 32, L"\u041e\u0442\u043c\u0435\u043d\u0430"); StringCchCopyW(g_str.btn_apply, 32, L"\u041f\u0440\u0438\u043c\u0435\u043d\u0438\u0442\u044c"); break;
        case LANG_DE: StringCchCopyW(g_str.btn_cancel, 32, L"Abbrechen"); StringCchCopyW(g_str.btn_apply, 32, L"\u00dcbernehmen"); break;
        case LANG_PT: StringCchCopyW(g_str.btn_cancel, 32, L"Cancelar"); StringCchCopyW(g_str.btn_apply, 32, L"Aplicar"); break;
        default: StringCchCopyW(g_str.btn_cancel, 32, L"Cancel"); StringCchCopyW(g_str.btn_apply, 32, L"Apply");
    }

    switch (lang) {
        case LANG_IT:
            StringCchCopyW(g_str.grp_appearance, 64, L"Aspetto della barra delle applicazioni");
            StringCchCopyW(g_str.chk_lock, 64, L"Blocca la barra delle applicazioni");
            StringCchCopyW(g_str.chk_hide, 64, L"Nascondi automaticamente la barra delle applicazioni");
            StringCchCopyW(g_str.chk_small, 64, L"Usa icone piccole");
            StringCchCopyW(g_str.txt_location, 64, L"Posizione della barra delle applicazioni sullo schermo:");
            StringCchCopyW(g_str.txt_buttons, 64, L"Pulsanti della barra\ndelle applicazioni:");
            StringCchCopyW(g_str.pos_bottom, 32, L"In basso");
            StringCchCopyW(g_str.pos_left, 32, L"A sinistra");
            StringCchCopyW(g_str.pos_right, 32, L"A destra");
            StringCchCopyW(g_str.pos_top, 32, L"In alto");
            StringCchCopyW(g_str.btn_always_combine, 64, L"Combina sempre, nascondi etichette");
            StringCchCopyW(g_str.btn_combine_full, 64, L"Combina se la barra delle applicazioni \u00e8 piena");
            StringCchCopyW(g_str.btn_never_combine, 64, L"Mai combinare");
            break;
        case LANG_FR:
            StringCchCopyW(g_str.grp_appearance, 64, L"Apparence de la barre des t\u00e2ches");
            StringCchCopyW(g_str.chk_lock, 64, L"Verrouiller la barre des t\u00e2ches");
            StringCchCopyW(g_str.chk_hide, 64, L"Masquer automatiquement la barre");
            StringCchCopyW(g_str.chk_small, 64, L"Utiliser de petites ic\u00f4nes");
            StringCchCopyW(g_str.txt_location, 64, L"Position de la barre \u00e0 l'\u00e9cran:");
            StringCchCopyW(g_str.txt_buttons, 64, L"Boutons de la barre\n\u00e0 l'\u00e9cran:");
            StringCchCopyW(g_str.pos_bottom, 32, L"En bas");
            StringCchCopyW(g_str.pos_left, 32, L"\u00e0 gauche");
            StringCchCopyW(g_str.pos_right, 32, L"\u00e0 droite");
            StringCchCopyW(g_str.pos_top, 32, L"En haut");
            StringCchCopyW(g_str.btn_always_combine, 64, L"Toujours combiner, masquer les \u00e9tiquettes");
            StringCchCopyW(g_str.btn_combine_full, 64, L"Combiner lorsque la barre est pleine");
            StringCchCopyW(g_str.btn_never_combine, 64, L"Ne jamais combiner");
            break;
        case LANG_ES:
            StringCchCopyW(g_str.grp_appearance, 64, L"Aspecto de la barra de tareas");
            StringCchCopyW(g_str.chk_lock, 64, L"Bloquear la barra de tareas");
            StringCchCopyW(g_str.chk_hide, 64, L"Ocultar autom\u00e1ticamente la barra");
            StringCchCopyW(g_str.chk_small, 64, L"Usar iconos peque\u00f1os");
            StringCchCopyW(g_str.txt_location, 64, L"Ubicaci\u00f3n de la barra en la pantalla:");
            StringCchCopyW(g_str.txt_buttons, 64, L"Botones de la barra\nde tareas:");
            StringCchCopyW(g_str.pos_bottom, 32, L"Abajo");
            StringCchCopyW(g_str.pos_left, 32, L"Izquierda");
            StringCchCopyW(g_str.pos_right, 32, L"Derecha");
            StringCchCopyW(g_str.pos_top, 32, L"Arriba");
            StringCchCopyW(g_str.btn_always_combine, 64, L"Combinar siempre, ocultar etiquetas");
            StringCchCopyW(g_str.btn_combine_full, 64, L"Combinar cuando la barra est\u00e1 llena");
            StringCchCopyW(g_str.btn_never_combine, 64, L"No combinar nunca");
            break;
        case LANG_RU:
            StringCchCopyW(g_str.grp_appearance, 64, L"\u0412\u0438\u0434 \u043f\u0430\u043d\u0435\u043b\u0438 \u0437\u0430\u0434\u0430\u0447");
            StringCchCopyW(g_str.chk_lock, 64, L"\u0417\u0430\u043a\u0440\u0435\u043f\u0438\u0442\u044c \u043f\u0430\u043d\u0435\u043b\u044c \u0437\u0430\u0434\u0430\u0447");
            StringCchCopyW(g_str.chk_hide, 64, L"\u0410\u0432\u0442\u043e\u043c\u0430\u0442\u0438\u0447\u0435\u0441\u043a\u0438 \u0441\u043a\u0440\u044b\u0432\u0430\u0442\u044c \u043f\u0430\u043d\u0435\u043b\u044c");
            StringCchCopyW(g_str.chk_small, 64, L"\u0418\u0441\u043f\u043e\u043b\u044c\u0437\u043e\u0432\u0430\u0442\u044c \u043c\u0435\u043b\u043a\u0438\u0435 \u0437\u043d\u0430\u0447\u043a\u0438");
            StringCchCopyW(g_str.txt_location, 64, L"\u041f\u043e\u043b\u043e\u0436\u0435\u043d\u0438\u0435 \u043f\u0430\u043d\u0435\u043b\u0438 \u043d\u0430 \u044d\u043a\u0440\u0430\u043d\u0435:");
            StringCchCopyW(g_str.txt_buttons, 64, L"\u041a\u043d\u043e\u043f\u043a\u0438 \u043f\u0430\u043d\u0435\u043b\u0438\n\u0437\u0430\u0434\u0430\u0447:");
            StringCchCopyW(g_str.pos_bottom, 32, L"\u0421\u043d\u0438\u0437\u0443");
            StringCchCopyW(g_str.pos_left, 32, L"\u0421\u043b\u0435\u0432\u0430");
            StringCchCopyW(g_str.pos_right, 32, L"\u0421\u043f\u0440\u0430\u0432\u0430");
            StringCchCopyW(g_str.pos_top, 32, L"\u0421\u0432\u0435\u0440\u0445\u0443");
            StringCchCopyW(g_str.btn_always_combine, 64, L"\u0412\u0441\u0435\u0433\u0434\u0430 \u0433\u0440\u0443\u043f\u043f\u0438\u0440\u043e\u0432\u0430\u0442\u044c, \u0441\u043a\u0440\u044b\u0432\u0430\u0442\u044c \u043c\u0435\u0442\u043a\u0438");
            StringCchCopyW(g_str.btn_combine_full, 64, L"\u0413\u0440\u0443\u043f\u043f\u0438\u0440\u043e\u0432\u0430\u0442\u044c \u043f\u0440\u0438 \u0437\u0430\u043f\u043e\u043b\u043d\u0435\u043d\u0438\u0438");
            StringCchCopyW(g_str.btn_never_combine, 64, L"\u041d\u0435 \u0433\u0440\u0443\u043f\u043f\u0438\u0440\u043e\u0432\u0430\u0442\u044c");
            break;
        case LANG_DE:
            StringCchCopyW(g_str.grp_appearance, 64, L"Erscheinungsbild der Taskleiste");
            StringCchCopyW(g_str.chk_lock, 64, L"Taskleiste fixieren");
            StringCchCopyW(g_str.chk_hide, 64, L"Taskleiste automatisch ausblenden");
            StringCchCopyW(g_str.chk_small, 64, L"Kleine Symbole verwenden");
            StringCchCopyW(g_str.txt_location, 64, L"Position der Taskleiste auf dem Bildschirm:");
            StringCchCopyW(g_str.txt_buttons, 64, L"Schaltfl\u00e4chen der\nTaskleiste:");
            StringCchCopyW(g_str.pos_bottom, 32, L"Unten");
            StringCchCopyW(g_str.pos_left, 32, L"Links");
            StringCchCopyW(g_str.pos_right, 32, L"Rechts");
            StringCchCopyW(g_str.pos_top, 32, L"Oben");
            StringCchCopyW(g_str.btn_always_combine, 64, L"Immer kombinieren, Beschriftungen ausblenden");
            StringCchCopyW(g_str.btn_combine_full, 64, L"Kombinieren, wenn die Taskleiste voll ist");
            StringCchCopyW(g_str.btn_never_combine, 64, L"Nie kombinieren");
            break;
        case LANG_PT:
            StringCchCopyW(g_str.grp_appearance, 64, L"Apar\u00eancia da barra de tarefas");
            StringCchCopyW(g_str.chk_lock, 64, L"Bloquear a barra de tarefas");
            StringCchCopyW(g_str.chk_hide, 64, L"Ocultar automaticamente a barra de tarefas");
            StringCchCopyW(g_str.chk_small, 64, L"Usar bot\u00f5es pequenos");
            StringCchCopyW(g_str.txt_location, 64, L"Localiza\u00e7\u00e3o da barra de tarefas na tela:");
            StringCchCopyW(g_str.txt_buttons, 64, L"Bot\u00f5es da barra\nde tarefas:");
            StringCchCopyW(g_str.pos_bottom, 32, L"Parte inferior");
            StringCchCopyW(g_str.pos_left, 32, L"Esquerda");
            StringCchCopyW(g_str.pos_right, 32, L"Direita");
            StringCchCopyW(g_str.pos_top, 32, L"Parte superior");
            StringCchCopyW(g_str.btn_always_combine, 64, L"Sempre combinar, ocultar r\u00f3tulos");
            StringCchCopyW(g_str.btn_combine_full, 64, L"Combinar quando a barra estiver cheia");
            StringCchCopyW(g_str.btn_never_combine, 64, L"Nunca combinar");
            break;
        default:
            StringCchCopyW(g_str.grp_appearance, 64, L"Taskbar appearance");
            StringCchCopyW(g_str.chk_lock, 64, L"Lock the taskbar");
            StringCchCopyW(g_str.chk_hide, 64, L"Auto-hide the taskbar");
            StringCchCopyW(g_str.chk_small, 64, L"Use small taskbar buttons");
            StringCchCopyW(g_str.txt_location, 64, L"Taskbar location on screen:");
            StringCchCopyW(g_str.txt_buttons, 64, L"Taskbar buttons:");
            StringCchCopyW(g_str.pos_bottom, 32, L"Bottom");
            StringCchCopyW(g_str.pos_left, 32, L"Left");
            StringCchCopyW(g_str.pos_right, 32, L"Right");
            StringCchCopyW(g_str.pos_top, 32, L"Top");
            StringCchCopyW(g_str.btn_always_combine, 64, L"Always combine, hide labels");
            StringCchCopyW(g_str.btn_combine_full, 64, L"Combine when taskbar is full");
            StringCchCopyW(g_str.btn_never_combine, 64, L"Never combine");
    }

    switch (lang) {
        case LANG_IT:
            StringCchCopyW(g_str.grp_notif, 64, L"Area di notifica");
            StringCchCopyW(g_str.txt_notif, 128, L"Consente di personalizzare le icone e le notifiche visualizzate nell'area di notifica della barra delle applicazioni.");
            StringCchCopyW(g_str.btn_cust_notif, 32, L"Personalizza...");
            break;
        case LANG_FR:
            StringCchCopyW(g_str.grp_notif, 64, L"Zone de notification");
            StringCchCopyW(g_str.txt_notif, 128, L"Permet de personnaliser les ic\u00f4nes et les notifications affich\u00e9es dans la zone de notification de la barre des t\u00e2ches.");
            StringCchCopyW(g_str.btn_cust_notif, 32, L"Personnaliser...");
            break;
        case LANG_ES:
            StringCchCopyW(g_str.grp_notif, 64, L"\u00c1rea de notificaci\u00f3n");
            StringCchCopyW(g_str.txt_notif, 128, L"Permite personalizar los iconos y notificaciones que aparecen en el \u00e1rea de notificaci\u00f3n de la barra de tareas.");
            StringCchCopyW(g_str.btn_cust_notif, 32, L"Personalizar...");
            break;
        case LANG_RU:
            StringCchCopyW(g_str.grp_notif, 64, L"\u041e\u0431\u043b\u0430\u0441\u0442\u044c \u0443\u0432\u0435\u0434\u043e\u043c\u043b\u0435\u043d\u0438\u0439");
            StringCchCopyW(g_str.txt_notif, 128, L"\u041f\u043e\u0437\u0432\u043e\u043b\u044f\u0435\u0442 \u043d\u0430\u0441\u0442\u0440\u043e\u0438\u0442\u044c \u0437\u043d\u0430\u0447\u043a\u0438 \u0438 \u0443\u0432\u0435\u0434\u043e\u043c\u043b\u0435\u043d\u0438\u044f, \u043e\u0442\u043e\u0431\u0440\u0430\u0436\u0430\u0435\u043c\u044b\u0435 \u0432 \u043e\u0431\u043b\u0430\u0441\u0442\u0438 \u0443\u0432\u0435\u0434\u043e\u043c\u043b\u0435\u043d\u0438\u0439 \u043f\u0430\u043d\u0435\u043b\u0438 \u0437\u0430\u0434\u0430\u0447.");
            StringCchCopyW(g_str.btn_cust_notif, 32, L"\u041d\u0430\u0441\u0442\u0440\u043e\u0438\u0442\u044c...");
            break;
        case LANG_DE:
            StringCchCopyW(g_str.grp_notif, 64, L"Infobereich");
            StringCchCopyW(g_str.txt_notif, 128, L"Legt fest, welche Symbole und Benachrichtigungen im Infobereich der Taskleiste angezeigt werden.");
            StringCchCopyW(g_str.btn_cust_notif, 32, L"Anpassen...");
            break;
        case LANG_PT:
            StringCchCopyW(g_str.grp_notif, 64, L"\u00c1rea de notifica\u00e7\u00e3o");
            StringCchCopyW(g_str.txt_notif, 128, L"Personalize quais \u00edcones e notifica\u00e7\u00f5es aparecem na \u00e1rea de notifica\u00e7\u00e3o da barra de tarefas.");
            StringCchCopyW(g_str.btn_cust_notif, 32, L"Personalizar...");
            break;
        default:
            StringCchCopyW(g_str.grp_notif, 64, L"Notification area");
            StringCchCopyW(g_str.txt_notif, 128, L"Customize which icons and notifications appear in the notification area of the taskbar.");
            StringCchCopyW(g_str.btn_cust_notif, 32, L"Customize...");
    }

    switch (lang) {
        case LANG_IT:
            StringCchCopyW(g_str.grp_aero, 64, L"Anteprima del desktop con Aero Peek");
            StringCchCopyW(g_str.txt_aero, 256, L"Consente di visualizzare temporaneamente il desktop portando il puntatore del mouse sul pulsante Mostra desktop alla fine della barra delle applicazioni.");
            StringCchCopyW(g_str.chk_aero, 64, L"Usa Aero Peek per visualizzare l'anteprima del desktop");
            StringCchCopyW(g_str.link_help, 128, L"<a>Come personalizzare la barra delle applicazioni?</a>");
            break;
        case LANG_FR:
            StringCchCopyW(g_str.grp_aero, 64, L"Aper\u00e7u du bureau avec Aero Peek");
            StringCchCopyW(g_str.txt_aero, 256, L"Permet d'afficher temporairement le bureau en pla\u00e7ant le pointeur de la souris sur le bouton Afficher le bureau \u00e0 l'extr\u00e9mit\u00e9 de la barre des t\u00e2ches.");
            StringCchCopyW(g_str.chk_aero, 64, L"Utiliser Aero Peek pour afficher l'aper\u00e7u du bureau");
            StringCchCopyW(g_str.link_help, 128, L"<a>Comment personnaliser la barre des t\u00e2ches ?</a>");
            break;
        case LANG_ES:
            StringCchCopyW(g_str.grp_aero, 64, L"Vista previa del escritorio con Aero Peek");
            StringCchCopyW(g_str.txt_aero, 256, L"Permite ver temporalmente el escritorio al colocar el puntero del mouse sobre el bot\u00f3n Mostrar escritorio al final de la barra de tareas.");
            StringCchCopyW(g_str.chk_aero, 64, L"Usar Aero Peek para obtener una vista previa del escritorio");
            StringCchCopyW(g_str.link_help, 128, L"<a>\u00bfC\u00f3mo personalizar la barra de tareas?</a>");
            break;
        case LANG_RU:
            StringCchCopyW(g_str.grp_aero, 64, L"\u041f\u0440\u0435\u0434\u043f\u0440\u043e\u0441\u043c\u043e\u0442\u0440 \u0440\u0430\u0431\u043e\u0447\u0435\u0433\u043e \u0441\u0442\u043e\u043b\u0430 \u0441 Aero Peek");
            StringCchCopyW(g_str.txt_aero, 256, L"\u041f\u043e\u0437\u0432\u043e\u043b\u044f\u0435\u0442 \u0432\u0440\u0435\u043c\u0435\u043d\u043d\u043e \u043e\u0442\u043e\u0431\u0440\u0430\u0437\u0438\u0442\u044c \u0440\u0430\u0431\u043e\u0447\u0438\u0439 \u0441\u0442\u043e\u043b \u043f\u0440\u0438 \u043d\u0430\u0432\u0435\u0434\u0435\u043d\u0438\u0438 \u0443\u043a\u0430\u0437\u0430\u0442\u0435\u043b\u044f \u043c\u044b\u0448\u0438 \u043d\u0430 \u043a\u043d\u043e\u043f\u043a\u0443 \u041f\u043e\u043a\u0430\u0437\u0430\u0442\u044c \u0440\u0430\u0431\u043e\u0447\u0438\u0439 \u0441\u0442\u043e\u043b \u0432 \u043a\u043e\u043d\u0446\u0435 \u043f\u0430\u043d\u0435\u043b\u0438 \u0437\u0430\u0434\u0430\u0447.");
            StringCchCopyW(g_str.chk_aero, 64, L"\u0418\u0441\u043f\u043e\u043b\u044c\u0437\u043e\u0432\u0430\u0442\u044c Aero Peek \u0434\u043b\u044f \u043f\u0440\u0435\u0434\u043f\u0440\u043e\u0441\u043c\u043e\u0442\u0440\u0430 \u0440\u0430\u0431\u043e\u0447\u0435\u0433\u043e \u0441\u0442\u043e\u043b\u0430");
            StringCchCopyW(g_str.link_help, 128, L"<a>\u041a\u0430\u043a \u043d\u0430\u0441\u0442\u0440\u043e\u0438\u0442\u044c \u043f\u0430\u043d\u0435\u043b\u044c \u0437\u0430\u0434\u0430\u0447?</a>");
            break;
        case LANG_DE:
            StringCchCopyW(g_str.grp_aero, 64, L"Desktopvorschau mit Aero Peek");
            StringCchCopyW(g_str.txt_aero, 256, L"Zeigt den Desktop vor\u00fcbergehend an, wenn Sie den Mauszeiger auf die Schaltfl\u00e4che \u201cDesktop anzeigen\u201d am Ende der Taskleiste bewegen.");
            StringCchCopyW(g_str.chk_aero, 64, L"Aero Peek zur Desktopvorschau verwenden");
            StringCchCopyW(g_str.link_help, 128, L"<a>Wie passe ich die Taskleiste an?</a>");
            break;
        case LANG_PT:
            StringCchCopyW(g_str.grp_aero, 64, L"Pr\u00e9-visualizar a \u00e1rea de trabalho com o Aero Peek");
            StringCchCopyW(g_str.txt_aero, 256, L"Exibe temporariamente a \u00e1rea de trabalho ao mover o ponteiro do mouse at\u00e9 o bot\u00e3o Mostrar \u00e1rea de trabalho, no final da barra de tarefas.");
            StringCchCopyW(g_str.chk_aero, 64, L"Usar o Aero Peek para pr\u00e9-visualizar a \u00e1rea de trabalho");
            StringCchCopyW(g_str.link_help, 128, L"<a>Como personalizar a barra de tarefas?</a>");
            break;
        default:
            StringCchCopyW(g_str.grp_aero, 64, L"Preview desktop with Aero Peek");
            StringCchCopyW(g_str.txt_aero, 256, L"Temporarily view the desktop when you move your mouse to the Show desktop button at the end of the taskbar.");
            StringCchCopyW(g_str.chk_aero, 64, L"Use Aero Peek to preview the desktop");
            StringCchCopyW(g_str.link_help, 128, L"<a>How do I customize the taskbar?</a>");
    }

    switch (lang) {
        case LANG_IT:
            StringCchCopyW(g_str.start_info, 256, L"Per personalizzare l'aspetto dei collegamenti, delle icone e dei menu nel menu Start, fare clic su Personalizza.");
            StringCchCopyW(g_str.btn_start_cust, 32, L"Personalizza...");
            StringCchCopyW(g_str.txt_power_label, 64, L"Azione pulsante di alimentazione:");
            StringCchCopyW(g_str.power_shutdown, 32, L"Arresta il sistema"); StringCchCopyW(g_str.power_restart, 32, L"Riavvia");
            StringCchCopyW(g_str.power_sleep, 32, L"Sospensione"); StringCchCopyW(g_str.power_hibernate, 32, L"Ibernazione");
            StringCchCopyW(g_str.power_logoff, 32, L"Disconnetti"); StringCchCopyW(g_str.power_lock, 32, L"Blocca");
            StringCchCopyW(g_str.power_switchuser, 32, L"Cambia utente");
            StringCchCopyW(g_str.grp_privacy, 32, L"Privacy");
            StringCchCopyW(g_str.chk_mru_prog, 128, L"Archivia e visualizza i programmi aperti di recente nel menu Start");
            StringCchCopyW(g_str.chk_mru_items, 128, L"Archivia e visualizza gli elementi aperti di recente nel menu Start e nella barra delle applicazioni");
            break;
        case LANG_FR:
            StringCchCopyW(g_str.start_info, 256, L"Pour personnaliser l'apparence des liens, des ic\u00f4nes et des menus dans le menu D\u00e9marrer, cliquez sur Personnaliser.");
            StringCchCopyW(g_str.btn_start_cust, 32, L"Personnaliser...");
            StringCchCopyW(g_str.txt_power_label, 64, L"Action du bouton d'alimentation :");
            StringCchCopyW(g_str.power_shutdown, 32, L"Arr\u00eater"); StringCchCopyW(g_str.power_restart, 32, L"Red\u00e9marrer");
            StringCchCopyW(g_str.power_sleep, 32, L"Veille"); StringCchCopyW(g_str.power_hibernate, 32, L"Hiberner");
            StringCchCopyW(g_str.power_logoff, 32, L"D\u00e9connexion"); StringCchCopyW(g_str.power_lock, 32, L"Verrouiller");
            StringCchCopyW(g_str.power_switchuser, 32, L"Changer d'utilisateur");
            StringCchCopyW(g_str.grp_privacy, 32, L"Confidentialit\u00e9");
            StringCchCopyW(g_str.chk_mru_prog, 128, L"Stocker et afficher les programmes r\u00e9cemment ouverts dans le menu D\u00e9marrer");
            StringCchCopyW(g_str.chk_mru_items, 128, L"Stocker et afficher les \u00e9l\u00e9ments r\u00e9cemment ouverts dans le menu D\u00e9marrer et la barre des t\u00e2ches");
            break;
        case LANG_ES:
            StringCchCopyW(g_str.start_info, 256, L"Para personalizar el aspecto de los v\u00ednculos, iconos y men\u00fas en el men\u00fa Inicio, haga clic en Personalizar.");
            StringCchCopyW(g_str.btn_start_cust, 32, L"Personalizar...");
            StringCchCopyW(g_str.txt_power_label, 64, L"Acci\u00f3n del bot\u00f3n de encendido:");
            StringCchCopyW(g_str.power_shutdown, 32, L"Apagar"); StringCchCopyW(g_str.power_restart, 32, L"Reiniciar");
            StringCchCopyW(g_str.power_sleep, 32, L"Suspender"); StringCchCopyW(g_str.power_hibernate, 32, L"Hibernar");
            StringCchCopyW(g_str.power_logoff, 32, L"Cerrar sesi\u00f3n"); StringCchCopyW(g_str.power_lock, 32, L"Bloquear");
            StringCchCopyW(g_str.power_switchuser, 32, L"Cambiar usuario");
            StringCchCopyW(g_str.grp_privacy, 32, L"Privacidad");
            StringCchCopyW(g_str.chk_mru_prog, 128, L"Almacenar y mostrar programas abiertos recientemente en el men\u00fa Inicio");
            StringCchCopyW(g_str.chk_mru_items, 128, L"Almacenar y mostrar elementos abiertos recientemente en el men\u00fa Inicio y la barra de tareas");
            break;
        case LANG_RU:
            StringCchCopyW(g_str.start_info, 256, L"\u0427\u0442\u043e\u0431\u044b \u043d\u0430\u0441\u0442\u0440\u043e\u0438\u0442\u044c \u0432\u0438\u0434 \u0441\u0441\u044b\u043b\u043e\u043a, \u0437\u043d\u0430\u0447\u043a\u043e\u0432 \u0438 \u043c\u0435\u043d\u044e \u0432 \u043c\u0435\u043d\u044e \u041f\u0443\u0441\u043a, \u043d\u0430\u0436\u043c\u0438\u0442\u0435 \u041d\u0430\u0441\u0442\u0440\u043e\u0438\u0442\u044c.");
            StringCchCopyW(g_str.btn_start_cust, 32, L"\u041d\u0430\u0441\u0442\u0440\u043e\u0438\u0442\u044c...");
            StringCchCopyW(g_str.txt_power_label, 64, L"\u0414\u0435\u0439\u0441\u0442\u0432\u0438\u0435 \u043a\u043d\u043e\u043f\u043a\u0438 \u043f\u0438\u0442\u0430\u043d\u0438\u044f:");
            StringCchCopyW(g_str.power_shutdown, 32, L"\u0417\u0430\u0432\u0435\u0440\u0448\u0435\u043d\u0438\u0435 \u0440\u0430\u0431\u043e\u0442\u044b"); StringCchCopyW(g_str.power_restart, 32, L"\u041f\u0435\u0440\u0435\u0437\u0430\u0433\u0440\u0443\u0437\u043a\u0430");
            StringCchCopyW(g_str.power_sleep, 32, L"\u0421\u043e\u043d"); StringCchCopyW(g_str.power_hibernate, 32, L"\u0413\u0438\u0431\u0435\u0440\u043d\u0430\u0446\u0438\u044f");
            StringCchCopyW(g_str.power_logoff, 32, L"\u0412\u044b\u0445\u043e\u0434"); StringCchCopyW(g_str.power_lock, 32, L"\u0411\u043b\u043e\u043a\u0438\u0440\u043e\u0432\u043a\u0430");
            StringCchCopyW(g_str.power_switchuser, 32, L"\u0421\u043c\u0435\u043d\u0430 \u043f\u043e\u043b\u044c\u0437\u043e\u0432\u0430\u0442\u0435\u043b\u044f");
            StringCchCopyW(g_str.grp_privacy, 32, L"\u041a\u043e\u043d\u0444\u0438\u0434\u0435\u043d\u0446\u0438\u0430\u043b\u044c\u043d\u043e\u0441\u0442\u044c");
            StringCchCopyW(g_str.chk_mru_prog, 128, L"\u0425\u0440\u0430\u043d\u0438\u0442\u044c \u0438 \u043f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0442\u044c \u043d\u0435\u0434\u0430\u0432\u043d\u043e \u043e\u0442\u043a\u0440\u044b\u0442\u044b\u0435 \u043f\u0440\u043e\u0433\u0440\u0430\u043c\u043c\u044b \u0432 \u043c\u0435\u043d\u044e \u041f\u0443\u0441\u043a");
            StringCchCopyW(g_str.chk_mru_items, 128, L"\u0425\u0440\u0430\u043d\u0438\u0442\u044c \u0438 \u043f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0442\u044c \u043d\u0435\u0434\u0430\u0432\u043d\u043e \u043e\u0442\u043a\u0440\u044b\u0442\u044b\u0435 \u044d\u043b\u0435\u043c\u0435\u043d\u0442\u044b \u0432 \u043c\u0435\u043d\u044e \u041f\u0443\u0441\u043a \u0438 \u043d\u0430 \u043f\u0430\u043d\u0435\u043b\u0438 \u0437\u0430\u0434\u0430\u0447");
            break;
        case LANG_DE:
            StringCchCopyW(g_str.start_info, 256, L"Um festzulegen, wie Links, Symbole und Men\u00fcs im Startmen\u00fc aussehen, klicken Sie auf Anpassen.");
            StringCchCopyW(g_str.btn_start_cust, 32, L"Anpassen...");
            StringCchCopyW(g_str.txt_power_label, 64, L"Aktion der Ein/Aus-Schaltfl\u00e4che:");
            StringCchCopyW(g_str.power_shutdown, 32, L"Herunterfahren"); StringCchCopyW(g_str.power_restart, 32, L"Neu starten");
            StringCchCopyW(g_str.power_sleep, 32, L"Energie sparen"); StringCchCopyW(g_str.power_hibernate, 32, L"Ruhezustand");
            StringCchCopyW(g_str.power_logoff, 32, L"Abmelden"); StringCchCopyW(g_str.power_lock, 32, L"Sperren");
            StringCchCopyW(g_str.power_switchuser, 32, L"Benutzer wechseln");
            StringCchCopyW(g_str.grp_privacy, 32, L"Datenschutz");
            StringCchCopyW(g_str.chk_mru_prog, 128, L"Zuletzt ge\u00f6ffnete Programme im Startmen\u00fc speichern und anzeigen");
            StringCchCopyW(g_str.chk_mru_items, 128, L"Zuletzt ge\u00f6ffnete Elemente im Startmen\u00fc und in der Taskleiste speichern und anzeigen");
            break;
        case LANG_PT:
            StringCchCopyW(g_str.start_info, 256, L"Para personalizar a apar\u00eancia de links, \u00edcones e menus no Menu Iniciar, clique em Personalizar.");
            StringCchCopyW(g_str.btn_start_cust, 32, L"Personalizar...");
            StringCchCopyW(g_str.txt_power_label, 64, L"A\u00e7\u00e3o do bot\u00e3o de energia:");
            StringCchCopyW(g_str.power_shutdown, 32, L"Desligar"); StringCchCopyW(g_str.power_restart, 32, L"Reiniciar");
            StringCchCopyW(g_str.power_sleep, 32, L"Suspender"); StringCchCopyW(g_str.power_hibernate, 32, L"Hibernar");
            StringCchCopyW(g_str.power_logoff, 32, L"Encerrar sess\u00e3o"); StringCchCopyW(g_str.power_lock, 32, L"Bloquear");
            StringCchCopyW(g_str.power_switchuser, 32, L"Trocar usu\u00e1rio");
            StringCchCopyW(g_str.grp_privacy, 32, L"Privacidade");
            StringCchCopyW(g_str.chk_mru_prog, 128, L"Armazenar e exibir programas abertos recentemente no Menu Iniciar");
            StringCchCopyW(g_str.chk_mru_items, 128, L"Armazenar e exibir itens abertos recentemente no Menu Iniciar e na barra de tarefas");
            break;
        default:
            StringCchCopyW(g_str.start_info, 256, L"To customize how links, icons, and menus look in the Start menu, click Customize.");
            StringCchCopyW(g_str.btn_start_cust, 32, L"Customize...");
            StringCchCopyW(g_str.txt_power_label, 64, L"Power button action:");
            StringCchCopyW(g_str.power_shutdown, 32, L"Shut down"); StringCchCopyW(g_str.power_restart, 32, L"Restart");
            StringCchCopyW(g_str.power_sleep, 32, L"Sleep"); StringCchCopyW(g_str.power_hibernate, 32, L"Hibernate");
            StringCchCopyW(g_str.power_logoff, 32, L"Log off"); StringCchCopyW(g_str.power_lock, 32, L"Lock");
            StringCchCopyW(g_str.power_switchuser, 32, L"Switch user");
            StringCchCopyW(g_str.grp_privacy, 32, L"Privacy");
            StringCchCopyW(g_str.chk_mru_prog, 128, L"Store and display recently opened programs in the Start menu");
            StringCchCopyW(g_str.chk_mru_items, 128, L"Store and display recently opened items in the Start menu and the taskbar");
    }

    switch (lang) {
        case LANG_IT:
            StringCchCopyW(g_str.toolbars_info, 128, L"Selezionare le barre degli strumenti da aggiungere alla barra delle applicazioni.");
            StringCchCopyW(g_str.toolbar_address, 32, L"Indirizzo"); StringCchCopyW(g_str.toolbar_links, 32, L"Collegamenti");
            StringCchCopyW(g_str.toolbar_tabletpc, 32, L"Pannello input Tablet PC"); StringCchCopyW(g_str.toolbar_desktop, 32, L"Desktop");
            break;
        case LANG_FR:
            StringCchCopyW(g_str.toolbars_info, 128, L"S\u00e9lectionnez les barres d'outils \u00e0 ajouter \u00e0 la barre des t\u00e2ches.");
            StringCchCopyW(g_str.toolbar_address, 32, L"Adresse"); StringCchCopyW(g_str.toolbar_links, 32, L"Liens");
            StringCchCopyW(g_str.toolbar_tabletpc, 32, L"Panneau de saisie Tablet PC"); StringCchCopyW(g_str.toolbar_desktop, 32, L"Bureau");
            break;
        case LANG_ES:
            StringCchCopyW(g_str.toolbars_info, 128, L"Seleccione las barras de herramientas para agregar a la barra de tareas.");
            StringCchCopyW(g_str.toolbar_address, 32, L"Direcci\u00f3n"); StringCchCopyW(g_str.toolbar_links, 32, L"V\u00ednculos");
            StringCchCopyW(g_str.toolbar_tabletpc, 32, L"Panel de entrada Tablet PC"); StringCchCopyW(g_str.toolbar_desktop, 32, L"Escritorio");
            break;
        case LANG_RU:
            StringCchCopyW(g_str.toolbars_info, 128, L"\u0412\u044b\u0431\u0435\u0440\u0438\u0442\u0435 \u043f\u0430\u043d\u0435\u043b\u0438 \u0438\u043d\u0441\u0442\u0440\u0443\u043c\u0435\u043d\u0442\u043e\u0432 \u0434\u043b\u044f \u0434\u043e\u0431\u0430\u0432\u043b\u0435\u043d\u0438\u044f \u043d\u0430 \u043f\u0430\u043d\u0435\u043b\u044c \u0437\u0430\u0434\u0430\u0447.");
            StringCchCopyW(g_str.toolbar_address, 32, L"\u0410\u0434\u0440\u0435\u0441"); StringCchCopyW(g_str.toolbar_links, 32, L"\u0421\u0441\u044b\u043b\u043a\u0438");
            StringCchCopyW(g_str.toolbar_tabletpc, 32, L"\u041f\u0430\u043d\u0435\u043b\u044c \u0432\u0432\u043e\u0434\u0430 \u043f\u043b\u0430\u043d\u0448\u0435\u0442\u0430"); StringCchCopyW(g_str.toolbar_desktop, 32, L"\u0420\u0430\u0431\u043e\u0447\u0438\u0439 \u0441\u0442\u043e\u043b");
            break;
        case LANG_DE:
            StringCchCopyW(g_str.toolbars_info, 128, L"W\u00e4hlen Sie aus, welche Symbolleisten der Taskleiste hinzugef\u00fcgt werden sollen.");
            StringCchCopyW(g_str.toolbar_address, 32, L"Adresse"); StringCchCopyW(g_str.toolbar_links, 32, L"Links");
            StringCchCopyW(g_str.toolbar_tabletpc, 32, L"Tablet-PC-Eingabebereich"); StringCchCopyW(g_str.toolbar_desktop, 32, L"Desktop");
            break;
        case LANG_PT:
            StringCchCopyW(g_str.toolbars_info, 128, L"Selecione quais barras de ferramentas adicionar \u00e0 barra de tarefas.");
            StringCchCopyW(g_str.toolbar_address, 32, L"Endere\u00e7o"); StringCchCopyW(g_str.toolbar_links, 32, L"Links");
            StringCchCopyW(g_str.toolbar_tabletpc, 32, L"Painel de Entrada do Tablet PC"); StringCchCopyW(g_str.toolbar_desktop, 32, L"\u00c1rea de trabalho");
            break;
        default:
            StringCchCopyW(g_str.toolbars_info, 128, L"Select which toolbars to add to the taskbar.");
            StringCchCopyW(g_str.toolbar_address, 32, L"Address"); StringCchCopyW(g_str.toolbar_links, 32, L"Links");
            StringCchCopyW(g_str.toolbar_tabletpc, 32, L"Tablet PC Input Panel"); StringCchCopyW(g_str.toolbar_desktop, 32, L"Desktop");
    }

    switch (lang) {
        case LANG_IT:
            StringCchCopyW(g_str.about_title, 64, L"Informazioni sulla mod");
            StringCchCopyW(g_str.about_text, 4096, L"Classic Taskbar Properties\r\n\r\nQuesta mod per Windhawk ricrea la classica finestra \"Propriet\u00e0 della barra delle applicazioni e del menu Start\" ispirata alle versioni classiche di Windows.\r\n\r\nFunzionalit\u00e0 attualmente disponibili:\r\n- Blocca la barra delle applicazioni\r\n- Nascondi automaticamente la barra\r\n- Usa icone piccole\r\n- Configura la combinazione dei pulsanti\r\n- Configura Aero Peek\r\n- Accesso rapido alle impostazioni dell'area di notifica\r\n- Barre degli strumenti native (Indirizzo, Collegamenti, Tablet PC)\r\n- Rotazione barra (solo Alto/Basso)\r\n\r\nLimitazioni note:\r\n- Posizioni Sinistra e Destra non supportate\r\n- Barra degli strumenti Desktop non disponibile\r\n- Alcune impostazioni richiedono il riavvio di Explorer.");
            StringCchCopyW(g_str.warn_position_title, 64, L"Posizione barra delle applicazioni");
            StringCchCopyW(g_str.warn_position_text, 256, L"La modifica della posizione verr\u00e0 applicata al prossimo riavvio di Explorer.");
            StringCchCopyW(g_str.start_custom_title, 64, L"Personalizza menu Start");
            StringCchCopyW(g_str.start_grp_tiles, 64, L"Riquadri e comportamento");
            StringCchCopyW(g_str.start_chk_more_tiles, 64, L"Mostra pi\u00f9 riquadri nel menu Start");
            StringCchCopyW(g_str.start_chk_app_list, 64, L"Mostra elenco app nel menu Start");
            StringCchCopyW(g_str.start_chk_recent_apps, 64, L"Mostra app aggiunte di recente");
            StringCchCopyW(g_str.start_chk_fullscreen, 64, L"Usa Start a schermo intero");
            StringCchCopyW(g_str.start_chk_recent_items, 64, L"Mostra elementi recenti nelle Jump List");
            StringCchCopyW(g_str.start_chk_account_notif, 64, L"Mostra notifiche account");
            StringCchCopyW(g_str.start_grp_search, 64, L"Ricerca");
            StringCchCopyW(g_str.start_chk_search_programs, 64, L"Includi programmi nei risultati di ricerca");
            StringCchCopyW(g_str.start_chk_search_files, 64, L"Includi file nei risultati di ricerca");
            StringCchCopyW(g_str.start_grp_folders, 64, L"Cartelle da visualizzare in Start");
            StringCchCopyW(g_str.start_chk_folder_settings, 32, L"Impostazioni");
            StringCchCopyW(g_str.start_chk_folder_docs, 32, L"Documenti");
            StringCchCopyW(g_str.start_chk_folder_downloads, 32, L"Download");
            StringCchCopyW(g_str.start_chk_folder_music, 32, L"Musica");
            StringCchCopyW(g_str.start_chk_folder_pics, 32, L"Immagini");
            StringCchCopyW(g_str.start_chk_folder_videos, 32, L"Video");
            StringCchCopyW(g_str.start_chk_folder_network, 32, L"Rete");
            StringCchCopyW(g_str.start_chk_folder_personal, 32, L"Cartella personale");
            StringCchCopyW(g_str.start_info_restart, 256, L"Nota: alcune modifiche potrebbero richiedere il riavvio di Explorer o il logout per essere applicate completamente.");
            StringCchCopyW(g_str.start_msg_saved, 512, L"Impostazioni del menu Start salvate.\n\nAlcune modifiche potrebbero richiedere il logout o il riavvio di Explorer per avere effetto completo.");
            StringCchCopyW(g_str.start_msg_saved_title, 64, L"Impostazioni salvate");
            break;
        case LANG_FR:
            StringCchCopyW(g_str.about_title, 64, L"\u00c0 propos de cette modification");
            StringCchCopyW(g_str.about_text, 4096, L"Classic Taskbar Properties\r\n\r\nCette modification Windhawk recr\u00e9e la fen\u00eatre classique \"Propri\u00e9t\u00e9s de la barre des t\u00e2ches et du menu D\u00e9marrer\" inspir\u00e9e des versions classiques de Windows.\r\n\r\nFonctionnalit\u00e9s disponibles :\r\n- Verrouiller la barre des t\u00e2ches\r\n- Masquer automatiquement\r\n- Petites ic\u00f4nes\r\n- Configuration du groupement des boutons\r\n- Configuration d'Aero Peek\r\n- Acc\u00e8s rapide aux param\u00e8tres de la zone de notification\r\n- Barres d'outils natives (Adresse, Liens, Tablet PC)\r\n- Rotation de la barre (Haut/Bas uniquement)\r\n\r\nLimitations connues :\r\n- Positions Gauche et Droite non prises en charge\r\n- Barre d'outils Bureau non disponible\r\n- Certains param\u00e8tres n\u00e9cessitent un red\u00e9marrage d'Explorer.");
            StringCchCopyW(g_str.warn_position_title, 64, L"Position de la barre");
            StringCchCopyW(g_str.warn_position_text, 256, L"Le changement de position sera appliqu\u00e9 apr\u00e8s le red\u00e9marrage d'Explorer.");
            StringCchCopyW(g_str.start_custom_title, 64, L"Personnaliser le menu D\u00e9marrer");
            StringCchCopyW(g_str.start_grp_tiles, 64, L"Vignettes et comportement");
            StringCchCopyW(g_str.start_chk_more_tiles, 64, L"Afficher plus de vignettes dans le menu D\u00e9marrer");
            StringCchCopyW(g_str.start_chk_app_list, 64, L"Afficher la liste des applications");
            StringCchCopyW(g_str.start_chk_recent_apps, 64, L"Afficher les applications r\u00e9cemment ajout\u00e9es");
            StringCchCopyW(g_str.start_chk_fullscreen, 64, L"Utiliser le menu D\u00e9marrer en plein \u00e9cran");
            StringCchCopyW(g_str.start_chk_recent_items, 64, L"Afficher les \u00e9l\u00e9ments r\u00e9cents dans les Jump Lists");
            StringCchCopyW(g_str.start_chk_account_notif, 64, L"Afficher les notifications du compte");
            StringCchCopyW(g_str.start_grp_search, 64, L"Recherche");
            StringCchCopyW(g_str.start_chk_search_programs, 64, L"Inclure les programmes dans la recherche");
            StringCchCopyW(g_str.start_chk_search_files, 64, L"Inclure les fichiers dans la recherche");
            StringCchCopyW(g_str.start_grp_folders, 64, L"Dossiers \u00e0 afficher dans D\u00e9marrer");
            StringCchCopyW(g_str.start_chk_folder_settings, 32, L"Param\u00e8tres");
            StringCchCopyW(g_str.start_chk_folder_docs, 32, L"Documents");
            StringCchCopyW(g_str.start_chk_folder_downloads, 32, L"T\u00e9l\u00e9chargements");
            StringCchCopyW(g_str.start_chk_folder_music, 32, L"Musique");
            StringCchCopyW(g_str.start_chk_folder_pics, 32, L"Images");
            StringCchCopyW(g_str.start_chk_folder_videos, 32, L"Vid\u00e9os");
            StringCchCopyW(g_str.start_chk_folder_network, 32, L"R\u00e9seau");
            StringCchCopyW(g_str.start_chk_folder_personal, 32, L"Dossier personnel");
            StringCchCopyW(g_str.start_info_restart, 256, L"Remarque : certaines modifications peuvent n\u00e9cessiter un red\u00e9marrage d'Explorer ou une d\u00e9connexion pour prendre pleinement effet.");
            StringCchCopyW(g_str.start_msg_saved, 512, L"Param\u00e8tres du menu D\u00e9marrer enregistr\u00e9s.\n\nCertaines modifications peuvent n\u00e9cessiter une d\u00e9connexion ou un red\u00e9marrage d'Explorer pour prendre pleinement effet.");
            StringCchCopyW(g_str.start_msg_saved_title, 64, L"Param\u00e8tres enregistr\u00e9s");
            break;
        case LANG_ES:
            StringCchCopyW(g_str.about_title, 64, L"Acerca de esta modificaci\u00f3n");
            StringCchCopyW(g_str.about_text, 4096, L"Classic Taskbar Properties\r\n\r\nEsta modificaci\u00f3n de Windhawk recrea la ventana cl\u00e1sica \"Propiedades de la barra de tareas y del men\u00fa Inicio\" inspirada en las versiones cl\u00e1sicas de Windows.\r\n\r\nFunciones disponibles :\r\n- Bloquear la barra de tareas\r\n- Ocultar autom\u00e1ticamente\r\n- Iconos peque\u00f1os\r\n- Configurar agrupaci\u00f3n de botones\r\n- Configurar Aero Peek\r\n- Acceso r\u00e1pido a la configuraci\u00f3n del \u00e1rea de notificaci\u00f3n\r\n- Barras de herramientas nativas (Direcci\u00f3n, V\u00ednculos, Tablet PC)\r\n- Rotaci\u00f3n de la barra (solo Arriba/Abajo)\r\n\r\nLimitaciones conocidas :\r\n- Posiciones Izquierda y Derecha no soportadas\r\n- Barra de herramientas Escritorio no disponible\r\n- Algunas configuraciones requieren reiniciar Explorer.");
            StringCchCopyW(g_str.warn_position_title, 64, L"Posici\u00f3n de la barra");
            StringCchCopyW(g_str.warn_position_text, 256, L"El cambio de posici\u00f3n se aplicar\u00e1 despu\u00e9s de reiniciar Explorer.");
            StringCchCopyW(g_str.start_custom_title, 64, L"Personalizar men\u00fa Inicio");
            StringCchCopyW(g_str.start_grp_tiles, 64, L"Mosaicos y comportamiento");
            StringCchCopyW(g_str.start_chk_more_tiles, 64, L"Mostrar m\u00e1s mosaicos en Inicio");
            StringCchCopyW(g_str.start_chk_app_list, 64, L"Mostrar lista de aplicaciones en Inicio");
            StringCchCopyW(g_str.start_chk_recent_apps, 64, L"Mostrar aplicaciones agregadas recientemente");
            StringCchCopyW(g_str.start_chk_fullscreen, 64, L"Usar Inicio en pantalla completa");
            StringCchCopyW(g_str.start_chk_recent_items, 64, L"Mostrar elementos recientes en Jump Lists");
            StringCchCopyW(g_str.start_chk_account_notif, 64, L"Mostrar notificaciones de cuenta");
            StringCchCopyW(g_str.start_grp_search, 64, L"B\u00fasqueda");
            StringCchCopyW(g_str.start_chk_search_programs, 64, L"Incluir programas en los resultados de b\u00fasqueda");
            StringCchCopyW(g_str.start_chk_search_files, 64, L"Incluir archivos en los resultados de b\u00fasqueda");
            StringCchCopyW(g_str.start_grp_folders, 64, L"Carpetas para mostrar en Inicio");
            StringCchCopyW(g_str.start_chk_folder_settings, 32, L"Configuraci\u00f3n");
            StringCchCopyW(g_str.start_chk_folder_docs, 32, L"Documentos");
            StringCchCopyW(g_str.start_chk_folder_downloads, 32, L"Descargas");
            StringCchCopyW(g_str.start_chk_folder_music, 32, L"M\u00fasica");
            StringCchCopyW(g_str.start_chk_folder_pics, 32, L"Im\u00e1genes");
            StringCchCopyW(g_str.start_chk_folder_videos, 32, L"V\u00eddeos");
            StringCchCopyW(g_str.start_chk_folder_network, 32, L"Red");
            StringCchCopyW(g_str.start_chk_folder_personal, 32, L"Carpeta personal");
            StringCchCopyW(g_str.start_info_restart, 256, L"Nota: algunos cambios pueden requerir reiniciar Explorer o cerrar sesi\u00f3n para que surtan efecto completo.");
            StringCchCopyW(g_str.start_msg_saved, 512, L"Configuraci\u00f3n del men\u00fa Inicio guardada.\n\nAlgunos cambios pueden requerir cerrar sesi\u00f3n o reiniciar Explorer para que surtan efecto completo.");
            StringCchCopyW(g_str.start_msg_saved_title, 64, L"Configuraci\u00f3n guardada");
            break;
        case LANG_RU:
            StringCchCopyW(g_str.about_title, 64, L"\u041e \u0434\u0430\u043d\u043d\u043e\u0439 \u043c\u043e\u0434\u0438\u0444\u0438\u043a\u0430\u0446\u0438\u0438");
            StringCchCopyW(g_str.about_text, 4096, L"Classic Taskbar Properties\r\n\r\n\u0414\u0430\u043d\u043d\u0430\u044f \u043c\u043e\u0434\u0438\u0444\u0438\u043a\u0430\u0446\u0438\u044f Windhawk \u0432\u043e\u0441\u0441\u043e\u0437\u0434\u0430\u0435\u0442 \u043a\u043b\u0430\u0441\u0441\u0438\u0447\u0435\u0441\u043a\u043e\u0435 \u043e\u043a\u043d\u043e \"\u0421\u0432\u043e\u0439\u0441\u0442\u0432\u0430 \u043f\u0430\u043d\u0435\u043b\u0438 \u0437\u0430\u0434\u0430\u0447 \u0438 \u043c\u0435\u043d\u044e \u041f\u0443\u0441\u043a\", \u0432\u0434\u043e\u0445\u043d\u043e\u0432\u043b\u0435\u043d\u043d\u043e\u0435 \u043a\u043b\u0430\u0441\u0441\u0438\u0447\u0435\u0441\u043a\u0438\u043c\u0438 \u0432\u0435\u0440\u0441\u0438\u044f\u043c\u0438 Windows.\r\n\r\n\u0414\u043e\u0441\u0442\u0443\u043f\u043d\u044b\u0435 \u0432\u043e\u0437\u043c\u043e\u0436\u043d\u043e\u0441\u0442\u0438:\r\n- \u0417\u0430\u043a\u0440\u0435\u043f\u043b\u0435\u043d\u0438\u0435 \u043f\u0430\u043d\u0435\u043b\u0438 \u0437\u0430\u0434\u0430\u0447\r\n- \u0410\u0432\u0442\u043e\u043c\u0430\u0442\u0438\u0447\u0435\u0441\u043a\u043e\u0435 \u0441\u043a\u0440\u044b\u0442\u0438\u0435\r\n- \u041c\u0435\u043b\u043a\u0438\u0435 \u0437\u043d\u0430\u0447\u043a\u0438\r\n- \u041d\u0430\u0441\u0442\u0440\u043e\u0439\u043a\u0430 \u0433\u0440\u0443\u043f\u043f\u0438\u0440\u043e\u0432\u043a\u0438 \u043a\u043d\u043e\u043f\u043e\u043a\r\n- \u041d\u0430\u0441\u0442\u0440\u043e\u0439\u043a\u0430 Aero Peek\r\n- \u0411\u044b\u0441\u0442\u0440\u044b\u0439 \u0434\u043e\u0441\u0442\u0443\u043f \u043a \u043d\u0430\u0441\u0442\u0440\u043e\u0439\u043a\u0430\u043c \u043e\u0431\u043b\u0430\u0441\u0442\u0438 \u0443\u0432\u0435\u0434\u043e\u043c\u043b\u0435\u043d\u0438\u0439\r\n- \u041f\u0430\u043d\u0435\u043b\u0438 \u0438\u043d\u0441\u0442\u0440\u0443\u043c\u0435\u043d\u0442\u043e\u0432 (\u0410\u0434\u0440\u0435\u0441, \u0421\u0441\u044b\u043b\u043a\u0438, \u041f\u043b\u0430\u043d\u0448\u0435\u0442)\r\n- \u041f\u043e\u0432\u043e\u0440\u043e\u0442 \u043f\u0430\u043d\u0435\u043b\u0438 (\u0442\u043e\u043b\u044c\u043a\u043e \u0412\u0432\u0435\u0440\u0445/\u0412\u043d\u0438\u0437)\r\n\r\n\u0418\u0437\u0432\u0435\u0441\u0442\u043d\u044b\u0435 \u043e\u0433\u0440\u0430\u043d\u0438\u0447\u0435\u043d\u0438\u044f:\r\n- \u041f\u043e\u043b\u043e\u0436\u0435\u043d\u0438\u044f \u0421\u043b\u0435\u0432\u0430 \u0438 \u0421\u043f\u0440\u0430\u0432\u0430 \u043d\u0435 \u043f\u043e\u0434\u0434\u0435\u0440\u0436\u0438\u0432\u0430\u044e\u0442\u0441\u044f\r\n- \u041f\u0430\u043d\u0435\u043b\u044c \u0420\u0430\u0431\u043e\u0447\u0438\u0439 \u0441\u0442\u043e\u043b \u043d\u0435\u0434\u043e\u0441\u0442\u0443\u043f\u043d\u0430\r\n- \u041d\u0435\u043a\u043e\u0442\u043e\u0440\u044b\u0435 \u043d\u0430\u0441\u0442\u0440\u043e\u0439\u043a\u0438 \u0442\u0440\u0435\u0431\u0443\u044e\u0442 \u043f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u043a\u0430 Explorer.");
            StringCchCopyW(g_str.warn_position_title, 64, L"\u041f\u043e\u043b\u043e\u0436\u0435\u043d\u0438\u0435 \u043f\u0430\u043d\u0435\u043b\u0438 \u0437\u0430\u0434\u0430\u0447");
            StringCchCopyW(g_str.warn_position_text, 256, L"\u0418\u0437\u043c\u0435\u043d\u0435\u043d\u0438\u0435 \u043f\u043e\u043b\u043e\u0436\u0435\u043d\u0438\u044f \u0431\u0443\u0434\u0435\u0442 \u043f\u0440\u0438\u043c\u0435\u043d\u0435\u043d\u043e \u043f\u043e\u0441\u043b\u0435 \u043f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u043a\u0430 Explorer.");
            StringCchCopyW(g_str.start_custom_title, 64, L"\u041d\u0430\u0441\u0442\u0440\u043e\u0439\u043a\u0430 \u043c\u0435\u043d\u044e \u041f\u0443\u0441\u043a");
            StringCchCopyW(g_str.start_grp_tiles, 64, L"\u041f\u043b\u0438\u0442\u043a\u0438 \u0438 \u043f\u043e\u0432\u0435\u0434\u0435\u043d\u0438\u0435");
            StringCchCopyW(g_str.start_chk_more_tiles, 64, L"\u041f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0442\u044c \u0431\u043e\u043b\u044c\u0448\u0435 \u043f\u043b\u0438\u0442\u043e\u043a \u0432 \u043c\u0435\u043d\u044e \u041f\u0443\u0441\u043a");
            StringCchCopyW(g_str.start_chk_app_list, 64, L"\u041f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0442\u044c \u0441\u043f\u0438\u0441\u043e\u043a \u043f\u0440\u0438\u043b\u043e\u0436\u0435\u043d\u0438\u0439 \u0432 \u043c\u0435\u043d\u044e \u041f\u0443\u0441\u043a");
            StringCchCopyW(g_str.start_chk_recent_apps, 64, L"\u041f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0442\u044c \u043d\u0435\u0434\u0430\u0432\u043d\u043e \u0434\u043e\u0431\u0430\u0432\u043b\u0435\u043d\u043d\u044b\u0435 \u043f\u0440\u0438\u043b\u043e\u0436\u0435\u043d\u0438\u044f");
            StringCchCopyW(g_str.start_chk_fullscreen, 64, L"\u0418\u0441\u043f\u043e\u043b\u044c\u0437\u043e\u0432\u0430\u0442\u044c \u041f\u0443\u0441\u043a \u043d\u0430 \u0432\u0435\u0441\u044c \u044d\u043a\u0440\u0430\u043d");
            StringCchCopyW(g_str.start_chk_recent_items, 64, L"\u041f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0442\u044c \u043d\u0435\u0434\u0430\u0432\u043d\u0438\u0435 \u044d\u043b\u0435\u043c\u0435\u043d\u0442\u044b \u0432 \u0441\u043f\u0438\u0441\u043a\u0430\u0445 \u043f\u0435\u0440\u0435\u0445\u043e\u0434\u043e\u0432");
            StringCchCopyW(g_str.start_chk_account_notif, 64, L"\u041f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0442\u044c \u0443\u0432\u0435\u0434\u043e\u043c\u043b\u0435\u043d\u0438\u044f \u0443\u0447\u0435\u0442\u043d\u043e\u0439 \u0437\u0430\u043f\u0438\u0441\u0438");
            StringCchCopyW(g_str.start_grp_search, 64, L"\u041f\u043e\u0438\u0441\u043a");
            StringCchCopyW(g_str.start_chk_search_programs, 64, L"\u0412\u043a\u043b\u044e\u0447\u0430\u0442\u044c \u043f\u0440\u043e\u0433\u0440\u0430\u043c\u043c\u044b \u0432 \u0440\u0435\u0437\u0443\u043b\u044c\u0442\u0430\u0442\u044b \u043f\u043e\u0438\u0441\u043a\u0430");
            StringCchCopyW(g_str.start_chk_search_files, 64, L"\u0412\u043a\u043b\u044e\u0447\u0430\u0442\u044c \u0444\u0430\u0439\u043b\u044b \u0432 \u0440\u0435\u0437\u0443\u043b\u044c\u0442\u0430\u0442\u044b \u043f\u043e\u0438\u0441\u043a\u0430");
            StringCchCopyW(g_str.start_grp_folders, 64, L"\u041f\u0430\u043f\u043a\u0438 \u0434\u043b\u044f \u043e\u0442\u043e\u0431\u0440\u0430\u0436\u0435\u043d\u0438\u044f \u0432 \u041f\u0443\u0441\u043a\u0435");
            StringCchCopyW(g_str.start_chk_folder_settings, 32, L"\u041f\u0430\u0440\u0430\u043c\u0435\u0442\u0440\u044b");
            StringCchCopyW(g_str.start_chk_folder_docs, 32, L"\u0414\u043e\u043a\u0443\u043c\u0435\u043d\u0442\u044b");
            StringCchCopyW(g_str.start_chk_folder_downloads, 32, L"\u0417\u0430\u0433\u0440\u0443\u0437\u043a\u0438");
            StringCchCopyW(g_str.start_chk_folder_music, 32, L"\u041c\u0443\u0437\u044b\u043a\u0430");
            StringCchCopyW(g_str.start_chk_folder_pics, 32, L"\u0418\u0437\u043e\u0431\u0440\u0430\u0436\u0435\u043d\u0438\u044f");
            StringCchCopyW(g_str.start_chk_folder_videos, 32, L"\u0412\u0438\u0434\u0435\u043e");
            StringCchCopyW(g_str.start_chk_folder_network, 32, L"\u0421\u0435\u0442\u044c");
            StringCchCopyW(g_str.start_chk_folder_personal, 32, L"\u041b\u0438\u0447\u043d\u0430\u044f \u043f\u0430\u043f\u043a\u0430");
            StringCchCopyW(g_str.start_info_restart, 256, L"\u041f\u0440\u0438\u043c\u0435\u0447\u0430\u043d\u0438\u0435: \u043d\u0435\u043a\u043e\u0442\u043e\u0440\u044b\u0435 \u0438\u0437\u043c\u0435\u043d\u0435\u043d\u0438\u044f \u043c\u043e\u0433\u0443\u0442 \u043f\u043e\u0442\u0440\u0435\u0431\u043e\u0432\u0430\u0442\u044c \u043f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u043a\u0430 Explorer \u0438\u043b\u0438 \u0432\u044b\u0445\u043e\u0434\u0430 \u0438\u0437 \u0441\u0438\u0441\u0442\u0435\u043c\u044b \u0434\u043b\u044f \u043f\u043e\u043b\u043d\u043e\u0433\u043e \u043f\u0440\u0438\u043c\u0435\u043d\u0435\u043d\u0438\u044f.");
            StringCchCopyW(g_str.start_msg_saved, 512, L"\u041d\u0430\u0441\u0442\u0440\u043e\u0439\u043a\u0438 \u043c\u0435\u043d\u044e \u041f\u0443\u0441\u043a \u0441\u043e\u0445\u0440\u0430\u043d\u0435\u043d\u044b.\n\n\u041d\u0435\u043a\u043e\u0442\u043e\u0440\u044b\u0435 \u0438\u0437\u043c\u0435\u043d\u0435\u043d\u0438\u044f \u043c\u043e\u0433\u0443\u0442 \u043f\u043e\u0442\u0440\u0435\u0431\u043e\u0432\u0430\u0442\u044c \u0432\u044b\u0445\u043e\u0434\u0430 \u0438\u0437 \u0441\u0438\u0441\u0442\u0435\u043c\u044b \u0438\u043b\u0438 \u043f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u043a\u0430 Explorer \u0434\u043b\u044f \u043f\u043e\u043b\u043d\u043e\u0433\u043e \u043f\u0440\u0438\u043c\u0435\u043d\u0435\u043d\u0438\u044f.");
            StringCchCopyW(g_str.start_msg_saved_title, 64, L"\u041d\u0430\u0441\u0442\u0440\u043e\u0439\u043a\u0438 \u0441\u043e\u0445\u0440\u0430\u043d\u0435\u043d\u044b");
            break;
        case LANG_DE:
            StringCchCopyW(g_str.about_title, 64, L"\u00dcber diese Mod");
            StringCchCopyW(g_str.about_text, 4096, L"Classic Taskbar Properties\r\n\r\nDiese Windhawk-Mod erstellt den klassischen Dialog \"Eigenschaften von Taskleiste und Startmen\u00fc\" aus \u00e4lteren Windows-Versionen nach.\r\n\r\nAktuell verf\u00fcgbare Funktionen :\r\n- Taskleiste fixieren\r\n- Taskleiste automatisch ausblenden\r\n- Kleine Symbole verwenden\r\n- Gruppierung der Taskleisten-Schaltfl\u00e4chen konfigurieren\r\n- Aero Peek konfigurieren\r\n- Schnellzugriff auf Infobereich-Einstellungen\r\n- Native Symbolleisten (Adresse, Links, Tablet PC)\r\n- Rotation der Taskleiste (nur oben/unten)\r\n\r\nBekannte Einschr\u00e4nkungen :\r\n- Links- und Rechtsposition werden nicht unterst\u00fctzt\r\n- Desktop-Symbolleiste derzeit nicht verf\u00fcgbar\r\n- Manche Einstellungen erfordern einen Explorer-Neustart.");
            StringCchCopyW(g_str.warn_position_title, 64, L"Position der Taskleiste");
            StringCchCopyW(g_str.warn_position_text, 256, L"Die \u00c4nderung der Position wird nach einem Neustart von Explorer wirksam.");
            StringCchCopyW(g_str.start_custom_title, 64, L"Startmen\u00fc anpassen");
            StringCchCopyW(g_str.start_grp_tiles, 64, L"Kacheln und Verhalten");
            StringCchCopyW(g_str.start_chk_more_tiles, 64, L"Mehr Kacheln im Startmen\u00fc anzeigen");
            StringCchCopyW(g_str.start_chk_app_list, 64, L"App-Liste im Startmen\u00fc anzeigen");
            StringCchCopyW(g_str.start_chk_recent_apps, 64, L"K\u00fcrzlich hinzugef\u00fcgte Apps anzeigen");
            StringCchCopyW(g_str.start_chk_fullscreen, 64, L"Startmen\u00fc im Vollbildmodus verwenden");
            StringCchCopyW(g_str.start_chk_recent_items, 64, L"Zuletzt ge\u00f6ffnete Elemente in Sprungliste anzeigen");
            StringCchCopyW(g_str.start_chk_account_notif, 64, L"Kontobenachrichtigungen anzeigen");
            StringCchCopyW(g_str.start_grp_search, 64, L"Suche");
            StringCchCopyW(g_str.start_chk_search_programs, 64, L"Programme in Suchergebnisse einbeziehen");
            StringCchCopyW(g_str.start_chk_search_files, 64, L"Dateien in Suchergebnisse einbeziehen");
            StringCchCopyW(g_str.start_grp_folders, 64, L"Im Startmen\u00fc anzuzeigende Ordner");
            StringCchCopyW(g_str.start_chk_folder_settings, 32, L"Einstellungen");
            StringCchCopyW(g_str.start_chk_folder_docs, 32, L"Dokumente");
            StringCchCopyW(g_str.start_chk_folder_downloads, 32, L"Downloads");
            StringCchCopyW(g_str.start_chk_folder_music, 32, L"Musik");
            StringCchCopyW(g_str.start_chk_folder_pics, 32, L"Bilder");
            StringCchCopyW(g_str.start_chk_folder_videos, 32, L"Videos");
            StringCchCopyW(g_str.start_chk_folder_network, 32, L"Netzwerk");
            StringCchCopyW(g_str.start_chk_folder_personal, 32, L"Pers\u00f6nlicher Ordner");
            StringCchCopyW(g_str.start_info_restart, 256, L"Hinweis: Einige \u00c4nderungen erfordern m\u00f6glicherweise einen Explorer-Neustart oder eine Abmeldung, um vollst\u00e4ndig wirksam zu werden.");
            StringCchCopyW(g_str.start_msg_saved, 512, L"Startmen\u00fc-Einstellungen gespeichert.\n\nEinige \u00c4nderungen erfordern m\u00f6glicherweise eine Abmeldung oder einen Explorer-Neustart, um vollst\u00e4ndig wirksam zu werden.");
            StringCchCopyW(g_str.start_msg_saved_title, 64, L"Einstellungen gespeichert");
            break;
        case LANG_PT:
            StringCchCopyW(g_str.about_title, 64, L"Sobre esta modifica\u00e7\u00e3o");
            StringCchCopyW(g_str.about_text, 4096, L"Classic Taskbar Properties\r\n\r\nEsta modifica\u00e7\u00e3o do Windhawk recria a cl\u00e1ssica janela \"Propriedades da Barra de Tarefas e do Menu Iniciar\" inspirada nas vers\u00f5es cl\u00e1ssicas do Windows.\r\n\r\nRecursos atualmente dispon\u00edveis :\r\n- Bloquear a barra de tarefas\r\n- Ocultar automaticamente a barra de tarefas\r\n- Usar \u00edcones pequenos\r\n- Configurar o agrupamento de bot\u00f5es da barra de tarefas\r\n- Configurar o Aero Peek\r\n- Acesso r\u00e1pido \u00e0s configura\u00e7\u00f5es da \u00e1rea de notifica\u00e7\u00e3o\r\n- Barras de ferramentas nativas (Endere\u00e7o, Links, Tablet PC)\r\n- Rota\u00e7\u00e3o da barra de tarefas (apenas superior/inferior)\r\n\r\nLimita\u00e7\u00f5es conhecidas :\r\n- Posi\u00e7\u00f5es esquerda e direita n\u00e3o suportadas\r\n- Barra de ferramentas da \u00e1rea de trabalho indispon\u00edvel no momento\r\n- Algumas configura\u00e7\u00f5es exigem reiniciar o Explorer.");
            StringCchCopyW(g_str.warn_position_title, 64, L"Posi\u00e7\u00e3o da barra de tarefas");
            StringCchCopyW(g_str.warn_position_text, 256, L"A mudan\u00e7a de posi\u00e7\u00e3o ser\u00e1 aplicada ap\u00f3s reiniciar o Explorer.");
            StringCchCopyW(g_str.start_custom_title, 64, L"Personalizar Menu Iniciar");
            StringCchCopyW(g_str.start_grp_tiles, 64, L"Blocos e comportamento");
            StringCchCopyW(g_str.start_chk_more_tiles, 64, L"Mostrar mais blocos no Menu Iniciar");
            StringCchCopyW(g_str.start_chk_app_list, 64, L"Mostrar lista de aplicativos no Menu Iniciar");
            StringCchCopyW(g_str.start_chk_recent_apps, 64, L"Mostrar aplicativos adicionados recentemente");
            StringCchCopyW(g_str.start_chk_fullscreen, 64, L"Usar o Menu Iniciar em tela cheia");
            StringCchCopyW(g_str.start_chk_recent_items, 64, L"Mostrar itens abertos recentemente nas Listas de Atalhos");
            StringCchCopyW(g_str.start_chk_account_notif, 64, L"Mostrar notifica\u00e7\u00f5es da conta");
            StringCchCopyW(g_str.start_grp_search, 64, L"Pesquisa");
            StringCchCopyW(g_str.start_chk_search_programs, 64, L"Incluir programas nos resultados da pesquisa");
            StringCchCopyW(g_str.start_chk_search_files, 64, L"Incluir arquivos nos resultados da pesquisa");
            StringCchCopyW(g_str.start_grp_folders, 64, L"Pastas a mostrar no Menu Iniciar");
            StringCchCopyW(g_str.start_chk_folder_settings, 32, L"Configura\u00e7\u00f5es");
            StringCchCopyW(g_str.start_chk_folder_docs, 32, L"Documentos");
            StringCchCopyW(g_str.start_chk_folder_downloads, 32, L"Downloads");
            StringCchCopyW(g_str.start_chk_folder_music, 32, L"M\u00fasica");
            StringCchCopyW(g_str.start_chk_folder_pics, 32, L"Imagens");
            StringCchCopyW(g_str.start_chk_folder_videos, 32, L"V\u00eddeos");
            StringCchCopyW(g_str.start_chk_folder_network, 32, L"Rede");
            StringCchCopyW(g_str.start_chk_folder_personal, 32, L"Pasta pessoal");
            StringCchCopyW(g_str.start_info_restart, 256, L"Nota: algumas altera\u00e7\u00f5es podem exigir reiniciar o Explorer ou encerrar a sess\u00e3o para ter efeito completo.");
            StringCchCopyW(g_str.start_msg_saved, 512, L"Configura\u00e7\u00f5es do Menu Iniciar salvas.\n\nAlgumas altera\u00e7\u00f5es podem exigir encerrar a sess\u00e3o ou reiniciar o Explorer para ter efeito completo.");
            StringCchCopyW(g_str.start_msg_saved_title, 64, L"Configura\u00e7\u00f5es salvas");
            break;
        default:
            StringCchCopyW(g_str.about_title, 64, L"About this mod");
            StringCchCopyW(g_str.about_text, 4096, L"Classic Taskbar Properties\r\n\r\nThis Windhawk mod recreates the classic \"Taskbar and Start Menu Properties\" dialog inspired by classic Windows versions.\r\n\r\nCurrently available features:\r\n- Lock the taskbar\r\n- Auto-hide the taskbar\r\n- Use small icons\r\n- Configure taskbar button grouping\r\n- Configure Aero Peek\r\n- Quick access to notification area settings\r\n- Native toolbars (Address, Links, Tablet PC)\r\n- Taskbar rotation (Top/Bottom only)\r\n\r\nKnown limitations:\r\n- Left and Right positions not supported\r\n- Desktop toolbar is currently not available\r\n- Some settings require Explorer restart.");
            StringCchCopyW(g_str.warn_position_title, 64, L"Taskbar position");
            StringCchCopyW(g_str.warn_position_text, 256, L"The taskbar position change will be applied after restarting Explorer.");
            StringCchCopyW(g_str.start_custom_title, 64, L"Customize Start Menu");
            StringCchCopyW(g_str.start_grp_tiles, 64, L"Tiles and behavior");
            StringCchCopyW(g_str.start_chk_more_tiles, 64, L"Show more tiles on Start");
            StringCchCopyW(g_str.start_chk_app_list, 64, L"Show app list in Start menu");
            StringCchCopyW(g_str.start_chk_recent_apps, 64, L"Show recently added apps");
            StringCchCopyW(g_str.start_chk_fullscreen, 64, L"Use Start full screen");
            StringCchCopyW(g_str.start_chk_recent_items, 64, L"Show recently opened items in Jump Lists");
            StringCchCopyW(g_str.start_chk_account_notif, 64, L"Show account notifications");
            StringCchCopyW(g_str.start_grp_search, 64, L"Search");
            StringCchCopyW(g_str.start_chk_search_programs, 64, L"Include programs in search results");
            StringCchCopyW(g_str.start_chk_search_files, 64, L"Include files in search results");
            StringCchCopyW(g_str.start_grp_folders, 64, L"Folders to show on Start");
            StringCchCopyW(g_str.start_chk_folder_settings, 32, L"Settings");
            StringCchCopyW(g_str.start_chk_folder_docs, 32, L"Documents");
            StringCchCopyW(g_str.start_chk_folder_downloads, 32, L"Downloads");
            StringCchCopyW(g_str.start_chk_folder_music, 32, L"Music");
            StringCchCopyW(g_str.start_chk_folder_pics, 32, L"Pictures");
            StringCchCopyW(g_str.start_chk_folder_videos, 32, L"Videos");
            StringCchCopyW(g_str.start_chk_folder_network, 32, L"Network");
            StringCchCopyW(g_str.start_chk_folder_personal, 32, L"Personal folder");
            StringCchCopyW(g_str.start_info_restart, 256, L"Note: some changes may require an Explorer restart or logout to take full effect.");
            StringCchCopyW(g_str.start_msg_saved, 512, L"Start menu settings saved.\n\nSome changes may require logout or Explorer restart to take full effect.");
            StringCchCopyW(g_str.start_msg_saved_title, 64, L"Settings saved");
            break;
    }
}

static void LoadLanguageSetting() {
    LPCWSTR lang = Wh_GetStringSetting(L"language");
    if (lang && wcslen(lang) > 0 && wcslen(lang) < 8) {
        StringCchCopyW(g_language, 8, lang);
    } else {
        StringCchCopyW(g_language, 8, L"auto");
    }
    Wh_FreeStringSetting(lang);
}

static void ApplyToolbars(bool addr, bool links, bool tablet, bool desk) {
    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hTray && !IsWindow(hTray)) hTray = NULL;
    
    NativeDeskBandOp(CLSID_CTP_AddressBand, addr ? CTP_BAND_SHOW : CTP_BAND_HIDE, NULL);
    TaskbarSettingsProvider::SetToolbarEnabled(L"Address", addr);
    
    NativeDeskBandOp(CLSID_CTP_LinksBand, links ? CTP_BAND_SHOW : CTP_BAND_HIDE, NULL);
    TaskbarSettingsProvider::SetToolbarEnabled(L"Links", links);
    
    // Gestione desktop con subclassing
    if (desk) {
        ShowNativeDesktopToolbar();
    } else {
        HideNativeDesktopToolbar();
    }
    // NOTA: non impostare pi TaskbarSettingsProvider::SetToolbarEnabled(L"Desktop", desk)
    // perch lo fanno gi Show/HideNativeDesktopToolbar
    
    TaskbarSettingsProvider::SetToolbarEnabled(L"TabletPC", tablet);
    
    SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Taskbar");
    if (hTray && IsWindow(hTray)) {
        SendNotifyMessageW(hTray, WM_SETTINGCHANGE, 0, (LPARAM)L"Taskbar");
    }
}

static void InitToolbarsList(HWND hList) {
    if (!IsWindow(hList)) return;
    
    ListView_SetExtendedListViewStyle(hList, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
    ListView_DeleteAllItems(hList);
    while (ListView_DeleteColumn(hList, 0)) {}
    
    LVCOLUMNW col = {};
    col.mask = LVCF_WIDTH | LVCF_FMT;
    col.fmt = LVCFMT_LEFT;
    col.cx = 230;
    ListView_InsertColumn(hList, 0, &col);
    
    const WCHAR* names[] = { g_str.toolbar_address, g_str.toolbar_links, g_str.toolbar_tabletpc, g_str.toolbar_desktop };
    const WCHAR* keys[]  = { L"Address", L"Links", L"TabletPC", L"Desktop" };
    const CLSID* clsids[] = { &CLSID_CTP_AddressBand, &CLSID_CTP_LinksBand, NULL, NULL };
    
    RefreshNativeDeskBandRegistration();
    
    for (int i = 0; i < 4; i++) {
        LVITEMW lvi = {};
        lvi.mask = LVIF_TEXT;
        lvi.iItem = i;
        lvi.pszText = (LPWSTR)names[i];
        ListView_InsertItem(hList, &lvi);
        
        bool checked = false;
        bool shown = false;
        bool got = false;
        
        if (i == 2) {
            shown = TaskbarSettingsProvider::GetToolbarEnabled(keys[i]);
            got = true;
        } else if (i == 3) {
            checked = IsNativeDesktopToolbarShown();
        } else if (clsids[i]) {
            got = NativeDeskBandOp(*clsids[i], CTP_BAND_QUERY, &shown);
        }
        
        if (i != 3) {
            if (got) {
                checked = shown;
            } else {
                checked = TaskbarSettingsProvider::GetToolbarEnabled(keys[i]);
            }
        }
        ListView_SetCheckState(hList, i, checked ? TRUE : FALSE);
    }
}

static const int kTaskbarCtls[] = { IDC_GRP_APPEARANCE, IDC_CHK_LOCK, IDC_CHK_HIDE, IDC_CHK_SMALL, IDC_TXT_LOCATION, IDC_COMBO_LOCATION, IDC_TXT_BUTTONS, IDC_COMBO_BUTTONS, IDC_GRP_NOTIF, IDC_TXT_NOTIF, IDC_BTN_CUST_NOTIF, IDC_GRP_AERO, IDC_TXT_AERO, IDC_CHK_AEROPEEK, IDC_LINK_HELP, 0 };
static const int kStartCtls[] = { IDC_TXT_START_INFO, IDC_BTN_START_CUST, IDC_TXT_POWER_LABEL, IDC_COMBO_POWER, IDC_GRP_PRIVACY, IDC_CHK_MRU_PROG, IDC_CHK_MRU_ITEMS, 0 };
static const int kToolbarCtls[] = { IDC_TXT_TOOLBARS_INFO, IDC_LST_TOOLBARS, 0 };

static void ShowGroup(HWND hwnd, const int* ids, bool show) {
    if (!IsWindow(hwnd)) return;
    int cmd = show ? SW_SHOW : SW_HIDE;
    for (int i = 0; ids[i]; i++) {
        HWND h = GetDlgItem(hwnd, ids[i]);
        if (h && IsWindow(h)) ShowWindow(h, cmd);
    }
}

static void SwitchTab(HWND hwnd, int tab) {
    if (!IsWindow(hwnd)) return;
    g_currentTab = tab;
    ShowGroup(hwnd, kTaskbarCtls, tab == 0);
    ShowGroup(hwnd, kStartCtls, tab == 1);
    ShowGroup(hwnd, kToolbarCtls, tab == 2);
}

// Misura i testi lunghi dei groupbox (Notifica e Aero Peek) e ridimensiona
// dinamicamente controlli e finestra in base alla lingua corrente.
// Chiamare DOPO SetDlgItemTextW e SetFontAllChildren, cos il font  gi impostato.
static void AutoFitGroupboxes(HWND hwnd) {
    if (!IsWindow(hwnd)) return;

    // Helper: client rect
    auto pixToClientRect = [&](HWND hCtrl, RECT& rcOut) -> bool {
        if (!hCtrl || !IsWindow(hCtrl)) return false;
        GetWindowRect(hCtrl, &rcOut);
        MapWindowPoints(NULL, hwnd, (LPPOINT)&rcOut, 2);
        return true;
    };

    // Calcola l'altezza necessaria per un testo dato un font e una larghezza fissa.
    auto measureTextHeight = [&](HWND hCtrl, int widthPx) -> int {
        HFONT hFont = (HFONT)SendMessageW(hCtrl, WM_GETFONT, 0, 0);
        if (!hFont) hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        WCHAR buf[1024]; buf[0] = 0;
        GetWindowTextW(hCtrl, buf, 1024);
        HDC hdc = GetDC(hCtrl);
        HFONT hOld = (HFONT)SelectObject(hdc, hFont);
        RECT rc = { 0, 0, widthPx, 0 };
        DrawTextW(hdc, buf, -1, &rc, DT_CALCRECT | DT_WORDBREAK | DT_LEFT);
        SelectObject(hdc, hOld);
        ReleaseDC(hCtrl, hdc);
        return rc.bottom - rc.top;
    };

    // Obtain standard measurements from Dialog Units (DLU) to scale perfectly with DPI
    RECT rcDlu = { 0, 0, 0, 3 };
    MapDialogRect(hwnd, &rcDlu);
    const int kPad = rcDlu.bottom; // 3 DLU padding (approx 5px at 100% DPI)

    rcDlu = { 0, 0, 0, 8 };
    MapDialogRect(hwnd, &rcDlu);
    const int kGrpTop = rcDlu.bottom; // 8 DLU top margin (approx 12px at 100% DPI)

    rcDlu = { 0, 0, 0, 3 };
    MapDialogRect(hwnd, &rcDlu);
    const int kGap = rcDlu.bottom; // 3 DLU gap between text and controls (approx 4px at 100% DPI)

    rcDlu = { 0, 0, 0, 14 };
    MapDialogRect(hwnd, &rcDlu);
    const int kBtnH = rcDlu.bottom; // 14 DLU standard button height (approx 21-23px at 100% DPI)

    rcDlu = { 0, 0, 0, 10 };
    MapDialogRect(hwnd, &rcDlu);
    const int kChkH = rcDlu.bottom; // 10 DLU standard checkbox height (approx 15px at 100% DPI)

    rcDlu = { 0, 0, 0, 8 };
    MapDialogRect(hwnd, &rcDlu);
    const int kLinkGap = rcDlu.bottom; // 8 DLU gap below Aero Peek to help link (approx 12px at 100% DPI)

    rcDlu = { 0, 0, 0, 6 };
    MapDialogRect(hwnd, &rcDlu);
    const int kTabBottomPad = rcDlu.bottom; // 6 DLU padding from help link to bottom of tab control

    rcDlu = { 0, 0, 0, 14 };
    MapDialogRect(hwnd, &rcDlu);
    const int kButtonGap = rcDlu.bottom; // 14 DLU gap between tab control and OK/Cancel buttons

    // ---- 1. GROUPBOX NOTIFICA (IDC_GRP_NOTIF / IDC_TXT_NOTIF / IDC_BTN_CUST_NOTIF) ----
    int deltaN = 0;
    RECT rcGrpN;
    HWND hGrpN = GetDlgItem(hwnd, IDC_GRP_NOTIF);
    HWND hTxtN = GetDlgItem(hwnd, IDC_TXT_NOTIF);
    HWND hBtnN = GetDlgItem(hwnd, IDC_BTN_CUST_NOTIF);
    if (hGrpN && hTxtN && hBtnN && pixToClientRect(hGrpN, rcGrpN)) {
        RECT rcTxtN, rcBtnN;
        if (pixToClientRect(hTxtN, rcTxtN) && pixToClientRect(hBtnN, rcBtnN)) {
            int txtW = rcTxtN.right - rcTxtN.left;
            int neededTextH = measureTextHeight(hTxtN, txtW);
            int innerH = std::max(neededTextH, kBtnH);
            int newGrpH = kGrpTop + kPad + innerH + kPad;
            deltaN = newGrpH - (rcGrpN.bottom - rcGrpN.top);

            SetWindowPos(hGrpN, NULL, rcGrpN.left, rcGrpN.top,
                         rcGrpN.right - rcGrpN.left, newGrpH,
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);

            SetWindowPos(hTxtN, NULL, rcTxtN.left, rcTxtN.top,
                         txtW, neededTextH,
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);

            int btnY = rcGrpN.top + kGrpTop + kPad + (innerH - kBtnH) / 2;
            SetWindowPos(hBtnN, NULL, rcBtnN.left, btnY,
                         rcBtnN.right - rcBtnN.left, kBtnH,
                         SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }

    // ---- 2. GROUPBOX AERO PEEK (IDC_GRP_AERO / IDC_TXT_AERO / IDC_CHK_AEROPEEK) ----
    int newGrpBottom = 0;
    RECT rcGrpA, rcTxtA, rcChkA;
    HWND hGrpA = GetDlgItem(hwnd, IDC_GRP_AERO);
    HWND hTxtA = GetDlgItem(hwnd, IDC_TXT_AERO);
    HWND hChkA = GetDlgItem(hwnd, IDC_CHK_AEROPEEK);
    if (hGrpA && hTxtA && hChkA &&
        pixToClientRect(hGrpA, rcGrpA) &&
        pixToClientRect(hTxtA, rcTxtA) &&
        pixToClientRect(hChkA, rcChkA))
    {
        // Calculate mathematically shifted tops based on deltaN from Groupbox 2 resizing
        int shiftedGrpTop = rcGrpA.top + deltaN;
        int shiftedTxtTop = rcTxtA.top + deltaN;

        int txtW = rcTxtA.right - rcTxtA.left;
        int neededTextH = measureTextHeight(hTxtA, txtW);
        int newGrpH = kGrpTop + kPad + neededTextH + kGap + kChkH + kPad;
        newGrpBottom = shiftedGrpTop + newGrpH;

        SetWindowPos(hGrpA, NULL, rcGrpA.left, shiftedGrpTop,
                     rcGrpA.right - rcGrpA.left, newGrpH,
                     SWP_NOZORDER | SWP_NOACTIVATE);

        SetWindowPos(hTxtA, NULL, rcTxtA.left, shiftedTxtTop,
                     txtW, neededTextH,
                     SWP_NOZORDER | SWP_NOACTIVATE);

        int chkY = shiftedGrpTop + kGrpTop + kPad + neededTextH + kGap;
        SetWindowPos(hChkA, NULL, rcChkA.left, chkY,
                     rcChkA.right - rcChkA.left, kChkH,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }

    // ---- 3. POSITION HELP LINK ----
    int linkBottom = 0;
    HWND hLink = GetDlgItem(hwnd, IDC_LINK_HELP);
    RECT rcLink;
    if (hLink && pixToClientRect(hLink, rcLink)) {
        int linkH = rcLink.bottom - rcLink.top;
        int linkY = newGrpBottom + kLinkGap;
        SetWindowPos(hLink, NULL, rcLink.left, linkY,
                     rcLink.right - rcLink.left, linkH,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        linkBottom = linkY + linkH;
    }

    // ---- 4. RESIZE TAB CONTROL ----
    int tabBottom = 0;
    HWND hTab = GetDlgItem(hwnd, IDC_TAB_MAIN);
    RECT rcTab;
    if (hTab && pixToClientRect(hTab, rcTab)) {
        int newTabH = (linkBottom + kTabBottomPad) - rcTab.top;
        SetWindowPos(hTab, NULL, rcTab.left, rcTab.top,
                     rcTab.right - rcTab.left, newTabH,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
        tabBottom = rcTab.top + newTabH;
    }

    // ---- 5. POSITION DIALOG BUTTONS ----
    int buttonY = tabBottom + kButtonGap;
    int idsToPos[] = { IDOK, IDCANCEL, IDC_BTN_APPLY };
    for (int id : idsToPos) {
        HWND hC = GetDlgItem(hwnd, id);
        RECT rc; if (hC && pixToClientRect(hC, rc)) {
            SetWindowPos(hC, NULL, rc.left, buttonY,
                         rc.right - rc.left, rc.bottom - rc.top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }

    // ---- 6. RESIZE DIALOG WINDOW ----
    HWND hApply = GetDlgItem(hwnd, IDC_BTN_APPLY);
    RECT rcApply;
    if (hApply && pixToClientRect(hApply, rcApply)) {
        RECT rcWin, rcClient;
        GetWindowRect(hwnd, &rcWin);
        GetClientRect(hwnd, &rcClient);
        int borderH = (rcWin.bottom - rcWin.top) - (rcClient.bottom - rcClient.top);
        int newClientH = rcApply.bottom + kPad * 2;
        int newWinH = newClientH + borderH;
        SetWindowPos(hwnd, NULL, 0, 0,
                     rcWin.right - rcWin.left, newWinH,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
    }

    InvalidateRect(hwnd, NULL, TRUE);
}

static void BalanceTextAndCombo(HWND hwndDlg, int idStatic, int idCombo) {
    if (!IsWindow(hwndDlg)) return;
    
    HWND hStatic = GetDlgItem(hwndDlg, idStatic);
    HWND hCombo = GetDlgItem(hwndDlg, idCombo);
    if (!hStatic || !IsWindow(hStatic) || !hCombo || !IsWindow(hCombo)) return;
    if (idCombo == IDC_COMBO_BUTTONS) return;
    
    HFONT hFont = (HFONT)SendMessageW(hStatic, WM_GETFONT, 0, 0);
    if (!hFont) hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    
    WCHAR szText[512];
    GetWindowTextW(hStatic, szText, 512);
    
    HDC hdc = GetDC(hStatic);
    HFONT hOld = (HFONT)SelectObject(hdc, hFont);
    SIZE size;
    GetTextExtentPoint32W(hdc, szText, lstrlenW(szText), &size);
    SelectObject(hdc, hOld);
    ReleaseDC(hStatic, hdc);
    
    RECT rcStatic, rcCombo;
    GetWindowRect(hStatic, &rcStatic);
    GetWindowRect(hCombo, &rcCombo);
    MapWindowPoints(NULL, hwndDlg, (LPPOINT)&rcStatic, 2);
    MapWindowPoints(NULL, hwndDlg, (LPPOINT)&rcCombo, 2);
    
    int maxRight = rcCombo.right;
    int newStaticWidth = size.cx + 8;
    int newComboX = rcStatic.left + newStaticWidth;
    int newComboWidth = maxRight - newComboX;
    if (newComboWidth < 70) {
        newComboWidth = 70;
        newStaticWidth = (maxRight - rcStatic.left) - newComboWidth;
        newComboX = rcStatic.left + newStaticWidth;
    }
    SetWindowPos(hStatic, NULL, rcStatic.left, rcStatic.top, newStaticWidth, rcStatic.bottom - rcStatic.top, SWP_NOZORDER | SWP_NOACTIVATE);
    SetWindowPos(hCombo, NULL, newComboX, rcCombo.top, newComboWidth, rcCombo.bottom - rcCombo.top, SWP_NOZORDER | SWP_NOACTIVATE);
}

static void ShowAboutDialog(HWND parent) {
    if (parent && !IsWindow(parent)) parent = NULL;
    MessageBoxW(parent, g_str.about_text, g_str.about_title, MB_OK | MB_ICONINFORMATION);
}

static INT_PTR CALLBACK StartCustomDlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_INITDIALOG: {
        g_hwndStartCustom = hwnd;
        ApplyDarkTitlebar(hwnd);
        ApplyExplorerThemeToChildren(hwnd);
        EnableThemeDialogTexture(hwnd, ETDT_DISABLE);
        
        HDC hdc = GetDC(hwnd);
        int ptPx = -MulDiv(9, GetDeviceCaps(hdc, LOGPIXELSY), 72);
        ReleaseDC(hwnd, hdc);
        
        if (g_hStartFontUi) { DeleteObject(g_hStartFontUi); g_hStartFontUi = NULL; }
        g_hStartFontUi = CreateFontW(ptPx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        
        SetWindowTextW(hwnd, g_str.start_custom_title);
        
        const DialogControlBinding bindings[] = {
            { IDC_START_GRP_TILES, g_str.start_grp_tiles }, { IDC_CHK_MORE_TILES, g_str.start_chk_more_tiles },
            { IDC_CHK_APP_LIST, g_str.start_chk_app_list }, { IDC_CHK_RECENT_APPS, g_str.start_chk_recent_apps },
            { IDC_CHK_FULLSCREEN, g_str.start_chk_fullscreen }, { IDC_CHK_RECENT_ITEMS, g_str.start_chk_recent_items },
            { IDC_CHK_ACCOUNT_NOTIF, g_str.start_chk_account_notif }, { IDC_GRP_SEARCH, g_str.start_grp_search },
            { IDC_CHK_SEARCH_PROGRAMS, g_str.start_chk_search_programs }, { IDC_CHK_SEARCH_FILES, g_str.start_chk_search_files },
            { IDC_START_GRP_FOLDERS, g_str.start_grp_folders }, { IDC_CHK_FOLDER_SETTINGS, g_str.start_chk_folder_settings },
            { IDC_CHK_FOLDER_DOCS, g_str.start_chk_folder_docs }, { IDC_CHK_FOLDER_DOWNLOADS, g_str.start_chk_folder_downloads },
            { IDC_CHK_FOLDER_MUSIC, g_str.start_chk_folder_music }, { IDC_CHK_FOLDER_PICS, g_str.start_chk_folder_pics },
            { IDC_CHK_FOLDER_VIDEOS, g_str.start_chk_folder_videos }, { IDC_CHK_FOLDER_NETWORK, g_str.start_chk_folder_network },
            { IDC_CHK_FOLDER_PERSONAL, g_str.start_chk_folder_personal }, { IDC_START_STATIC_INFO, g_str.start_info_restart },
            { IDC_START_BTN_APPLY, g_str.btn_apply }, { IDOK, g_str.btn_ok }, { IDCANCEL, g_str.btn_cancel },
        };
        for (const auto& b : bindings) {
            if (IsWindow(hwnd)) SetDlgItemTextW(hwnd, b.controlId, b.text);
        }
        
        if (g_hStartFontUi && IsWindow(hwnd)) {
            SendMessageW(hwnd, WM_SETFONT, (WPARAM)g_hStartFontUi, TRUE);
            SetFontAllChildren(hwnd, g_hStartFontUi);
        }
        
        for (const auto& s : g_startSettings) {
            if (!IsWindow(hwnd)) break;
            bool state = GetStartSettingState(s);
            bool locked = s.policyValue && IsSettingLockedByPolicy(s.policyValue);
            HWND hCtrl = GetDlgItem(hwnd, s.controlId);
            if (hCtrl && IsWindow(hCtrl)) {
                SendMessageW(hCtrl, BM_SETCHECK, state ? BST_CHECKED : BST_UNCHECKED, 0);
                EnableWindow(hCtrl, !locked);
            }
        }
        
        for (const auto& f : g_startFolders) {
            if (!IsWindow(hwnd)) break;
            bool state = TaskbarSettingsProvider::GetStartFolderVisible(f.folderRegValue);
            HWND hCtrl = GetDlgItem(hwnd, f.controlId);
            if (hCtrl && IsWindow(hCtrl)) {
                SendMessageW(hCtrl, BM_SETCHECK, state ? BST_CHECKED : BST_UNCHECKED, 0);
            }
        }
        
        if (IsWindow(hwnd)) {
            EnableWindow(GetDlgItem(hwnd, IDC_START_BTN_APPLY), FALSE);
            
            RECT rc; GetWindowRect(hwnd, &rc);
            int ww = rc.right - rc.left, wh = rc.bottom - rc.top;
            SetWindowPos(hwnd, NULL, (GetSystemMetrics(SM_CXSCREEN) - ww) / 2, (GetSystemMetrics(SM_CYSCREEN) - wh) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            
            LONG style = GetWindowLongW(hwnd, GWL_STYLE);
            style &= ~WS_THICKFRAME; style &= ~WS_MAXIMIZEBOX;
            SetWindowLongW(hwnd, GWL_STYLE, style);
            SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
        }
        return TRUE;
    }
    case WM_COMMAND: {
        if (!IsWindow(hwnd)) break;
        WORD id = LOWORD(wp); WORD act = HIWORD(wp);
        if (act == BN_CLICKED && id != IDOK && id != IDCANCEL && id != IDC_START_BTN_APPLY) {
            EnableWindow(GetDlgItem(hwnd, IDC_START_BTN_APPLY), TRUE);
        }
        if (id == IDOK) {
            SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_START_BTN_APPLY, BN_CLICKED), 0);
            if (IsWindow(hwnd)) DestroyWindow(hwnd);
        } else if (id == IDCANCEL) {
            DestroyWindow(hwnd);
        } else if (id == IDC_START_BTN_APPLY) {
            for (const auto& s : g_startSettings) {
                HWND hCtrl = GetDlgItem(hwnd, s.controlId);
                if (hCtrl && IsWindow(hCtrl) && IsWindowEnabled(hCtrl)) {
                    bool checked = (SendMessageW(hCtrl, BM_GETCHECK, 0, 0) == BST_CHECKED);
                    SetStartSettingState(s, checked);
                }
            }
            for (const auto& f : g_startFolders) {
                HWND hCtrl = GetDlgItem(hwnd, f.controlId);
                if (hCtrl && IsWindow(hCtrl)) {
                    bool checked = (SendMessageW(hCtrl, BM_GETCHECK, 0, 0) == BST_CHECKED);
                    TaskbarSettingsProvider::SetStartFolderVisible(f.folderRegValue, checked);
                }
            }
            SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Windows");
            MessageBoxW(hwnd, g_str.start_msg_saved, g_str.start_msg_saved_title, MB_OK | MB_ICONINFORMATION);
            if (IsWindow(hwnd)) EnableWindow(GetDlgItem(hwnd, IDC_START_BTN_APPLY), FALSE);
        }
        break;
    }
    case WM_DESTROY:
        if (g_hStartFontUi) { DeleteObject(g_hStartFontUi); g_hStartFontUi = NULL; }
        g_hwndStartCustom = NULL;
        InterlockedExchange(&g_startCustomOpen, 0);
        break;
    case WM_CLOSE:
        if (IsWindow(hwnd)) DestroyWindow(hwnd);
        break;
    }
    return FALSE;
}

static void ShowStartCustomDialog(HWND parent) {
    if (parent && !IsWindow(parent)) parent = NULL;
    
    if (InterlockedExchange(&g_startCustomOpen, 1)) {
        HWND hw = g_hwndStartCustom;
        if (hw && IsWindow(hw)) {
            SetForegroundWindow(hw);
            if (IsIconic(hw)) ShowWindow(hw, SW_RESTORE);
        }
        return;
    }
    
    BYTE* buf = new BYTE[4096];
    BYTE* p = buf;
    auto align4 = [](BYTE*& ptr) { ptr = (BYTE*)(((UINT_PTR)ptr + 3) & ~3); };
    LPDLGTEMPLATEW pDlg = (LPDLGTEMPLATEW)p;
    pDlg->style = DS_SETFONT | DS_MODALFRAME | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU;
    pDlg->dwExtendedStyle = 0;
    pDlg->cdit = 23;
    pDlg->x = 0; pDlg->y = 0;
    pDlg->cx = DialogSizes::START_WIDTH;
    pDlg->cy = DialogSizes::START_HEIGHT;
    p += sizeof(DLGTEMPLATE);
    *(WORD*)p = 0; p += 2;
    *(WORD*)p = 0; p += 2;
    StringCchCopyW((WCHAR*)p, 1, L""); p += 2;
    *(WORD*)p = 8; p += 2;
    StringCchCopyW((WCHAR*)p, 10, L"Segoe UI");
    p += (lstrlenW(L"Segoe UI") + 1) * 2;
    
    auto addCtrl = [&](DWORD style, DWORD exStyle, short x, short y, short cx, short cy, WORD id, LPCWSTR cls, LPCWSTR cap) {
        align4(p);
        LPDLGITEMTEMPLATE pi = (LPDLGITEMTEMPLATE)p;
        pi->style = WS_CHILD | WS_VISIBLE | style;
        pi->dwExtendedStyle = exStyle;
        pi->x = x; pi->y = y; pi->cx = cx; pi->cy = cy; pi->id = id;
        p += sizeof(DLGITEMTEMPLATE);
        StringCchCopyW((WCHAR*)p, lstrlenW(cls) + 1, cls);
        p += (lstrlenW(cls) + 1) * 2;
        StringCchCopyW((WCHAR*)p, lstrlenW(cap) + 1, cap);
        p += (lstrlenW(cap) + 1) * 2;
        *(WORD*)p = 0; p += 2;
    };
    
    addCtrl(BS_GROUPBOX, 0, 6, 4, 240, 102, IDC_START_GRP_TILES, L"Button", g_str.start_grp_tiles);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 16, 226, 11, IDC_CHK_MORE_TILES, L"Button", g_str.start_chk_more_tiles);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 29, 226, 11, IDC_CHK_APP_LIST, L"Button", g_str.start_chk_app_list);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 42, 226, 11, IDC_CHK_RECENT_APPS, L"Button", g_str.start_chk_recent_apps);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 55, 226, 11, IDC_CHK_FULLSCREEN, L"Button", g_str.start_chk_fullscreen);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 68, 226, 11, IDC_CHK_RECENT_ITEMS, L"Button", g_str.start_chk_recent_items);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 81, 226, 11, IDC_CHK_ACCOUNT_NOTIF, L"Button", g_str.start_chk_account_notif);
    addCtrl(BS_GROUPBOX, 0, 6, 109, 240, 44, IDC_GRP_SEARCH, L"Button", g_str.start_grp_search);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 121, 226, 11, IDC_CHK_SEARCH_PROGRAMS, L"Button", g_str.start_chk_search_programs);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 134, 226, 11, IDC_CHK_SEARCH_FILES, L"Button", g_str.start_chk_search_files);
    addCtrl(BS_GROUPBOX, 0, 6, 157, 240, 107, IDC_START_GRP_FOLDERS, L"Button", g_str.start_grp_folders);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 169, 110, 11, IDC_CHK_FOLDER_SETTINGS, L"Button", g_str.start_chk_folder_settings);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 182, 110, 11, IDC_CHK_FOLDER_DOCS, L"Button", g_str.start_chk_folder_docs);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 195, 110, 11, IDC_CHK_FOLDER_DOWNLOADS, L"Button", g_str.start_chk_folder_downloads);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 208, 110, 11, IDC_CHK_FOLDER_MUSIC, L"Button", g_str.start_chk_folder_music);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 130, 169, 110, 11, IDC_CHK_FOLDER_PICS, L"Button", g_str.start_chk_folder_pics);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 130, 182, 110, 11, IDC_CHK_FOLDER_VIDEOS, L"Button", g_str.start_chk_folder_videos);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 130, 195, 110, 11, IDC_CHK_FOLDER_NETWORK, L"Button", g_str.start_chk_folder_network);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 130, 208, 110, 11, IDC_CHK_FOLDER_PERSONAL, L"Button", g_str.start_chk_folder_personal);
    addCtrl(SS_LEFT, 0, 8, 268, 236, 18, IDC_START_STATIC_INFO, L"Static", L"");
    addCtrl(BS_DEFPUSHBUTTON | WS_TABSTOP, 0, 54, 291, 60, 13, IDOK, L"Button", g_str.btn_ok);
    addCtrl(BS_PUSHBUTTON | WS_TABSTOP, 0, 118, 291, 60, 13, IDCANCEL, L"Button", g_str.btn_cancel);
    addCtrl(BS_PUSHBUTTON | WS_TABSTOP, 0, 182, 291, 60, 13, IDC_START_BTN_APPLY, L"Button", g_str.btn_apply);
    
    HWND hwnd = CreateDialogIndirectParamW(GetModuleHandleW(NULL), (LPDLGTEMPLATE)buf, parent, StartCustomDlgProc, 0);
    delete[] buf;
    
    if (hwnd && IsWindow(hwnd)) {
        ShowWindow(hwnd, SW_SHOWNORMAL);
        SetForegroundWindow(hwnd);
    } else {
        InterlockedExchange(&g_startCustomOpen, 0);
    }
}

static std::atomic<bool> g_taskbarPosHooksInstalled{false};
static DWORD GetTaskbarSideFromRegistry() { return TaskbarSettingsProvider::GetTaskbarEdge(); }
static void ClearRotatedIconCache();
static DWORD g_lastEdge = 3;

using TrayUI_GetStuckInfo_t = void(WINAPI*)(void* pThis, RECT* rect, DWORD* taskbarPos);
static TrayUI_GetStuckInfo_t TrayUI_GetStuckInfo_Original = nullptr;
using TrayUI__HandleSettingChange_t = void(WINAPI*)(void* pThis, void* p1, void* p2, void* p3, void* p4);
static TrayUI__HandleSettingChange_t TrayUI__HandleSettingChange_Original = nullptr;
using TrayUI_GetDockedRect_t = DWORD(WINAPI*)(void* pThis, RECT* rect, BOOL param2);
static TrayUI_GetDockedRect_t TrayUI_GetDockedRect_Original = nullptr;
using TrayUI_MakeStuckRect_t = void(WINAPI*)(void* pThis, RECT* rect, RECT* param2, SIZE param3, DWORD taskbarPos);
static TrayUI_MakeStuckRect_t TrayUI_MakeStuckRect_Original = nullptr;

static int ComputeTaskbarThickness(const RECT* rect, const MONITORINFO& mi) {
    int width = rect->right - rect->left;
    int height = rect->bottom - rect->top;
    int monitorWidth = mi.rcMonitor.right - mi.rcMonitor.left;
    int monitorHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;
    int thickness;
    if (width > 0 && width < monitorWidth) thickness = width;
    else if (height > 0 && height < monitorHeight) thickness = height;
    else thickness = height > 0 ? height : width;
    if (thickness <= 0 || thickness > 300) thickness = 40;
    return thickness;
}

static void WINAPI TrayUI_GetStuckInfo_Hook(void* pThis, RECT* rect, DWORD* taskbarPos) {
    TrayUI_GetStuckInfo_Original(pThis, rect, taskbarPos);
    if (g_taskbarPosHooksInstalled && taskbarPos) {
        *taskbarPos = GetTaskbarSideFromRegistry();
    }
}

static DWORD WINAPI TrayUI_GetDockedRect_Hook(void* pThis, RECT* rect, BOOL param2) {
    DWORD ret = TrayUI_GetDockedRect_Original(pThis, rect, param2);
    if (!g_taskbarPosHooksInstalled || !rect) return ret;
    
    DWORD edge = GetTaskbarSideFromRegistry();
    if (edge == 3) return ret;
    
    HMONITOR monitor = MonitorFromRect(rect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfo(monitor, &mi)) return ret;
    
    int thickness = ComputeTaskbarThickness(rect, mi);
    switch (edge) {
        case 0: SetRect(rect, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.left + thickness, mi.rcMonitor.bottom); break;
        case 1: SetRect(rect, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right, mi.rcMonitor.top + thickness); break;
        case 2: SetRect(rect, mi.rcMonitor.right - thickness, mi.rcMonitor.top, mi.rcMonitor.right, mi.rcMonitor.bottom); break;
    }
    return ret;
}

static void WINAPI TrayUI_MakeStuckRect_Hook(void* pThis, RECT* rect, RECT* param2, SIZE param3, DWORD taskbarPos) {
    TrayUI_MakeStuckRect_Original(pThis, rect, param2, param3, taskbarPos);
    if (!g_taskbarPosHooksInstalled || !rect) return;
    
    DWORD edge = GetTaskbarSideFromRegistry();
    if (edge == 3) return;
    
    HMONITOR monitor = MonitorFromRect(rect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfo(monitor, &mi)) return;
    
    int thickness = ComputeTaskbarThickness(rect, mi);
    switch (edge) {
        case 0: SetRect(rect, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.left + thickness, mi.rcMonitor.bottom); break;
        case 1: SetRect(rect, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right, mi.rcMonitor.top + thickness); break;
        case 2: SetRect(rect, mi.rcMonitor.right - thickness, mi.rcMonitor.top, mi.rcMonitor.right, mi.rcMonitor.bottom); break;
    }
}

static void WINAPI TrayUI__HandleSettingChange_Hook(void* pThis, void* p1, void* p2, void* p3, void* p4) {
    TrayUI__HandleSettingChange_Original(pThis, p1, p2, p3, p4);
    if (!g_taskbarPosHooksInstalled) return;
    
    DWORD edge = GetTaskbarSideFromRegistry();
    if (edge == g_lastEdge) return;
    g_lastEdge = edge;
    
    ClearRotatedIconCache();
    HWND hTray = FindWindowExW(nullptr, nullptr, L"Shell_TrayWnd", nullptr);
    if (hTray && IsWindow(hTray)) {
        PostMessageW(hTray, WM_SIZE, 0, 0);
        HWND hReBar = FindWindowExW(hTray, nullptr, L"ReBarWindow32", nullptr);
        if (hReBar && IsWindow(hReBar)) {
            PostMessageW(hReBar, WM_SIZE, 0, 0);
            HWND hTaskSw = FindWindowExW(hReBar, nullptr, L"MSTaskSwWClass", nullptr);
            if (hTaskSw && IsWindow(hTaskSw)) PostMessageW(hTaskSw, WM_SIZE, 0, 0);
        }
        HWND hTrayNotify = FindWindowExW(hTray, nullptr, L"TrayNotifyWnd", nullptr);
        if (hTrayNotify && IsWindow(hTrayNotify)) PostMessageW(hTrayNotify, WM_SIZE, 0, 0);
        InvalidateRect(hTray, nullptr, TRUE);
    }
}

static bool InstallTaskbarPositionHooks() {
    HMODULE module = GetModuleHandleW(nullptr);
    if (!module) {
        Wh_Log(L"InstallTaskbarPositionHooks: GetModuleHandle(NULL) failed");
        return false;
    }
    
    WindhawkUtils::SYMBOL_HOOK explorer_exe_hooks[] = {
        { {LR"(public: virtual void __cdecl TrayUI::GetStuckInfo(struct tagRECT *,unsigned int *))"}, &TrayUI_GetStuckInfo_Original, TrayUI_GetStuckInfo_Hook },
        { {LR"(public: void __cdecl TrayUI::_HandleSettingChange(struct HWND__ *,unsigned int,unsigned __int64,__int64))"}, &TrayUI__HandleSettingChange_Original, TrayUI__HandleSettingChange_Hook },
        { {LR"(public: virtual unsigned int __cdecl TrayUI::GetDockedRect(struct tagRECT *,int))"}, &TrayUI_GetDockedRect_Original, TrayUI_GetDockedRect_Hook },
        { {LR"(public: virtual void __cdecl TrayUI::MakeStuckRect(struct tagRECT *,struct tagRECT const *,struct tagSIZE,unsigned int))"}, &TrayUI_MakeStuckRect_Original, TrayUI_MakeStuckRect_Hook },
    };
    
    if (!WindhawkUtils::HookSymbols(module, explorer_exe_hooks, ARRAYSIZE(explorer_exe_hooks))) {
        Wh_Log(L"InstallTaskbarPositionHooks: HookSymbols failed");
        return false;
    }
    
    g_taskbarPosHooksInstalled = true;
    Wh_Log(L"InstallTaskbarPositionHooks: All 5 hooks installed");
    return true;
}

struct RotatedIconCacheEntry { HICON originalIcon = nullptr; int size = 0; bool clockwise = false; HBITMAP rotatedBitmap = nullptr; };
static std::vector<RotatedIconCacheEntry> g_rotatedIconCache;
static bool g_rotateVerticalIcons = false;

static void ClearRotatedIconCache() {
    for (auto& e : g_rotatedIconCache) {
        if (e.rotatedBitmap) DeleteObject(e.rotatedBitmap);
    }
    g_rotatedIconCache.clear();
}


static HBITMAP RotateDib90(HBITMAP hbmSrc, int w, int h, bool clockwise) {
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    std::vector<DWORD> src((size_t)w * h);
    HDC hdcScreen = GetDC(nullptr);
    if (!GetDIBits(hdcScreen, hbmSrc, 0, h, src.data(), &bi, DIB_RGB_COLORS)) {
        ReleaseDC(nullptr, hdcScreen);
        return nullptr;
    }
    int dw = h, dh = w;
    std::vector<DWORD> dst((size_t)dw * dh);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            DWORD px = src[(size_t)y * w + x];
            int dx, dy;
            if (clockwise) { dx = h - 1 - y; dy = x; }
            else { dx = y; dy = w - 1 - x; }
            dst[(size_t)dy * dw + dx] = px;
        }
    }
    BITMAPINFO biDst = bi;
    biDst.bmiHeader.biWidth = dw;
    biDst.bmiHeader.biHeight = -dh;
    void* bits = nullptr;
    HBITMAP hbmDst = CreateDIBSection(hdcScreen, &biDst, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (hbmDst && bits) memcpy(bits, dst.data(), dst.size() * sizeof(DWORD));
    ReleaseDC(nullptr, hdcScreen);
    return hbmDst;
}

static HBITMAP GetOrCreateRotatedIconBitmap(HICON hIcon, int size, bool clockwise) {
    for (auto& e : g_rotatedIconCache) {
        if (e.originalIcon == hIcon && e.size == size && e.clockwise == clockwise) {
            return e.rotatedBitmap;
        }
    }
    
    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP hbmIcon = CreateDIBSection(hdcScreen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!hbmIcon || !bits) {
        DeleteDC(hdcMem);
        ReleaseDC(nullptr, hdcScreen);
        return nullptr;
    }
    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmIcon);
    memset(bits, 0, (size_t)size * size * 4);
    DrawIconEx(hdcMem, 0, 0, hIcon, size, size, 0, nullptr, DI_NORMAL);
    SelectObject(hdcMem, hbmOld);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
    
    HBITMAP hbmRotated = RotateDib90(hbmIcon, size, size, clockwise);
    DeleteObject(hbmIcon);
    if (!hbmRotated) return nullptr;
    
    if (g_rotatedIconCache.size() > 300) ClearRotatedIconCache();
    g_rotatedIconCache.push_back({hIcon, size, clockwise, hbmRotated});
    return hbmRotated;
}

static void PaintTaskbarButtonBackground(HWND hWnd, HDC hdc, const RECT& rc, UINT state) {
    HTHEME hTheme = OpenThemeData(hWnd, L"TaskBand");
    if (hTheme) {
        int stateId = 1;
        if (state & CDIS_SELECTED) stateId = 3;
        else if (state & CDIS_HOT) stateId = 2;
        DrawThemeBackground(hTheme, hdc, 1, stateId, &rc, nullptr);
        CloseThemeData(hTheme);
    } else {
        FillRect(hdc, &rc, GetSysColorBrush(COLOR_WINDOW));
    }
}

static LRESULT HandleTaskSwCustomDraw(HWND hWndToolbar, NMTBCUSTOMDRAW* tbcd, DWORD edge) {
    if (!IsWindow(hWndToolbar)) return CDRF_DODEFAULT;
    
    switch (tbcd->nmcd.dwDrawStage) {
    case CDDS_PREPAINT: return CDRF_NOTIFYITEMDRAW;
    case CDDS_ITEMPREPAINT: {
        if (g_modUnloading) return CDRF_DODEFAULT;
        
        HDC hdc = tbcd->nmcd.hdc;
        RECT rc = tbcd->nmcd.rc;
        int cmdId = (int)tbcd->nmcd.dwItemSpec;
        int btnIndex = (int)SendMessageW(hWndToolbar, TB_COMMANDTOINDEX, cmdId, 0);
        if (btnIndex < 0) return CDRF_DODEFAULT;
        
        TBBUTTON tbb{};
        if (!SendMessageW(hWndToolbar, TB_GETBUTTON, btnIndex, (LPARAM)&tbb)) return CDRF_DODEFAULT;
        
        PaintTaskbarButtonBackground(hWndToolbar, hdc, rc, (UINT)tbcd->nmcd.uItemState);
        
        HIMAGELIST hImg = (HIMAGELIST)SendMessageW(hWndToolbar, TB_GETIMAGELIST, 0, 0);
        HICON hIcon = hImg ? ImageList_GetIcon(hImg, tbb.iBitmap, ILD_TRANSPARENT) : nullptr;
        if (hIcon) {
            int w = rc.right - rc.left, h = rc.bottom - rc.top;
            int iconSize = std::min(w, h) - 8;
            iconSize = std::max(16, std::min(iconSize, 48));
            int cx = rc.left + (w - iconSize) / 2;
            int cy = rc.top + (h - iconSize) / 2;
            
            if (g_rotateVerticalIcons) {
                bool clockwise = (edge == 0);
                HBITMAP hbm = GetOrCreateRotatedIconBitmap(hIcon, iconSize, clockwise);
                if (hbm) {
                    HDC hdcMem = CreateCompatibleDC(hdc);
                    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbm);
                    BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
                    AlphaBlend(hdc, cx, cy, iconSize, iconSize, hdcMem, 0, 0, iconSize, iconSize, bf);
                    SelectObject(hdcMem, hbmOld);
                    DeleteDC(hdcMem);
                }
            } else {
                DrawIconEx(hdc, cx, cy, hIcon, iconSize, iconSize, 0, nullptr, DI_NORMAL);
            }
            DestroyIcon(hIcon);
        }
        
        if (tbb.fsState & TBSTATE_CHECKED) {
            RECT ind = rc;
            HBRUSH hb = GetSysColorBrush(COLOR_HIGHLIGHT);
            if (edge == 0) ind.right = ind.left + 3;
            else if (edge == 2) ind.left = ind.right - 3;
            FillRect(hdc, &ind, hb);
        }
        return CDRF_SKIPDEFAULT;
    }
    }
    return CDRF_DODEFAULT;
}

static int FindRebarBandIndexForChild(HWND hReBar, HWND hWndChild) {
    if (!IsWindow(hReBar) || !IsWindow(hWndChild)) return -1;
    int count = (int)SendMessageW(hReBar, RB_GETBANDCOUNT, 0, 0);
    for (int i = 0; i < count; i++) {
        REBARBANDINFOW rbbi{};
        rbbi.cbSize = sizeof(rbbi);
        rbbi.fMask = RBBIM_CHILD;
        if (SendMessageW(hReBar, RB_GETBANDINFOW, i, (LPARAM)&rbbi) && rbbi.hwndChild == hWndChild) {
            return i;
        }
    }
    return -1;
}

static constexpr UINT_PTR kTimerIdMasterLayout = 0x57A0;
static constexpr UINT kLayoutDebounceMs = 50;

static void ArmMasterLayoutTimer(HWND hShellTrayWnd) {
    if (!hShellTrayWnd || !IsWindow(hShellTrayWnd) || g_modUnloading) return;
    SetTimer(hShellTrayWnd, kTimerIdMasterLayout, kLayoutDebounceMs, nullptr);
}

static void ApplyTaskSwInternalLayout(HWND hWnd, bool vertical, int thickness) {
    if (!IsWindow(hWnd)) return;
    
    static thread_local HWND s_lastHwnd = nullptr;
    static thread_local DWORD s_lastEdgeApplied = 0xFFFFFFFF;
    static thread_local int s_lastThicknessApplied = -1;
    DWORD edge = TaskbarSettingsProvider::GetTaskbarEdge();
    
    if (vertical) {
        bool alreadyApplied = (s_lastHwnd == hWnd && s_lastEdgeApplied == edge && s_lastThicknessApplied == thickness);
        if (!alreadyApplied) {
            LONG_PTR curStyle = GetWindowLongPtrW(hWnd, GWL_STYLE);
            if (!(curStyle & TBSTYLE_WRAPABLE)) {
                SetWindowLongPtrW(hWnd, GWL_STYLE, curStyle | TBSTYLE_WRAPABLE);
            }
            
            UINT dpi = GetDpiForWindow(hWnd);
            int newSize = std::max(16, thickness - MulDiv(4, dpi, 96));
            DWORD cur = (DWORD)SendMessageW(hWnd, TB_GETBUTTONSIZE, 0, 0);
            if ((int)LOWORD(cur) != newSize || (int)HIWORD(cur) != newSize) {
                SendMessageW(hWnd, TB_SETBUTTONSIZE, 0, MAKELONG(newSize, newSize));
            }
            
            int btnCount = (int)SendMessageW(hWnd, TB_BUTTONCOUNT, 0, 0);
            if (btnCount < 1) btnCount = 1;
            
            RECT rcCur{};
            GetWindowRect(hWnd, &rcCur);
            int curW = rcCur.right - rcCur.left;
            int curH = rcCur.bottom - rcCur.top;
            if (curW != thickness) {
                SetWindowPos(hWnd, NULL, 0, 0, thickness, curH, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
            }
            
            RECT rowsRect{};
            SendMessageW(hWnd, TB_SETROWS, MAKEWPARAM(btnCount, FALSE), (LPARAM)&rowsRect);
            SendMessageW(hWnd, TB_AUTOSIZE, 0, 0);
            
            HWND hReBar = GetParent(hWnd);
            if (hReBar && IsWindow(hReBar)) {
                int bandIndex = FindRebarBandIndexForChild(hReBar, hWnd);
                if (bandIndex >= 0) {
                    REBARBANDINFOW rbbi{};
                    rbbi.cbSize = sizeof(rbbi);
                    rbbi.fMask = RBBIM_CHILDSIZE;
                    SendMessageW(hReBar, RB_SETBANDINFOW, bandIndex, (LPARAM)&rbbi);
                }
            }
            
            s_lastHwnd = hWnd;
            s_lastEdgeApplied = edge;
            s_lastThicknessApplied = thickness;
        }
    } else if (s_lastEdgeApplied != edge || s_lastHwnd != hWnd) {
        LONG_PTR curStyle = GetWindowLongPtrW(hWnd, GWL_STYLE);
        if (curStyle & TBSTYLE_WRAPABLE) {
            SetWindowLongPtrW(hWnd, GWL_STYLE, curStyle & ~TBSTYLE_WRAPABLE);
        }
        RECT rowsRect{};
        SendMessageW(hWnd, TB_SETROWS, MAKEWPARAM(1, FALSE), (LPARAM)&rowsRect);
        SendMessageW(hWnd, TB_AUTOSIZE, 0, 0);
        s_lastHwnd = hWnd;
        s_lastEdgeApplied = edge;
        s_lastThicknessApplied = -1;
    }
}

static LRESULT CALLBACK TaskSwSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR uIdSubclass) {
    if (!IsWindow(hWnd)) return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    
    switch (uMsg) {
    case WM_SIZE: {
        LRESULT ret = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        if (!g_modUnloading) {
            HWND hShellTray = GetAncestor(hWnd, GA_ROOT);
            ArmMasterLayoutTimer(hShellTray);
        }
        return ret;
    }
    case WM_NCDESTROY:
        RemoveWindowSubclass(hWnd, (SUBCLASSPROC)TaskSwSubclassProc, uIdSubclass);
        UntrackSubclass(hWnd, TaskSwSubclassProc);
        break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static LRESULT CALLBACK ReBarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR uIdSubclass) {
    if (!IsWindow(hWnd)) return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    
    if (uMsg == WM_NOTIFY) {
        NMHDR* nm = (NMHDR*)lParam;
        if (nm->code == NM_CUSTOMDRAW) {
            WCHAR cls[32];
            if (GetClassNameW(nm->hwndFrom, cls, ARRAYSIZE(cls)) && _wcsicmp(cls, L"MSTaskSwWClass") == 0 && !g_modUnloading) {
                DWORD edge = TaskbarSettingsProvider::GetTaskbarEdge();
                if (IsEdgeVertical(edge)) {
                    return HandleTaskSwCustomDraw(nm->hwndFrom, (NMTBCUSTOMDRAW*)lParam, edge);
                }
            }
        }
    } else if (uMsg == WM_NCDESTROY) {
        RemoveWindowSubclass(hWnd, (SUBCLASSPROC)ReBarSubclassProc, uIdSubclass);
        UntrackSubclass(hWnd, ReBarSubclassProc);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

struct TrayChildNaturalSize { HWND hwnd = nullptr; int naturalLength = 0; };
static std::vector<TrayChildNaturalSize> g_trayChildSizes;

static int GetOrCacheTrayChildNaturalLength(HWND hChild, int currentWidth, int currentHeight, bool isHorizontalNow) {
    for (auto& e : g_trayChildSizes) {
        if (e.hwnd == hChild) {
            if (isHorizontalNow && currentWidth > 0) e.naturalLength = currentWidth;
            return e.naturalLength;
        }
    }
    int initial = isHorizontalNow ? currentWidth : std::max(currentWidth, currentHeight);
    g_trayChildSizes.push_back({hChild, initial});
    return initial;
}

static void PurgeDestroyedTrayChildCacheEntries() {
    g_trayChildSizes.erase(
        std::remove_if(g_trayChildSizes.begin(), g_trayChildSizes.end(),
            [](const TrayChildNaturalSize& e) { return !IsWindow(e.hwnd); }),
        g_trayChildSizes.end()
    );
}

static int EstimateTrayNotifyTotalLength(HWND hTrayNotify) {
    if (!hTrayNotify || !IsWindow(hTrayNotify)) return 0;
    int total = 0;
    HWND hChild = GetWindow(hTrayNotify, GW_CHILD);
    while (hChild) {
        HWND hNext = GetWindow(hChild, GW_HWNDNEXT);
        for (auto& e : g_trayChildSizes) {
            if (e.hwnd == hChild) {
                total += std::max(e.naturalLength, 1);
                break;
            }
        }
        hChild = hNext;
    }
    return total;
}

static void ForceToolbarVerticalColumn(HWND hToolbar, int thickness) {
    if (!hToolbar || !IsWindow(hToolbar)) return;
    LONG_PTR curStyle = GetWindowLongPtrW(hToolbar, GWL_STYLE);
    if (!(curStyle & TBSTYLE_WRAPABLE)) {
        SetWindowLongPtrW(hToolbar, GWL_STYLE, curStyle | TBSTYLE_WRAPABLE);
    }
    int btnCount = (int)SendMessageW(hToolbar, TB_BUTTONCOUNT, 0, 0);
    if (btnCount < 1) return;
    UINT dpi = GetDpiForWindow(hToolbar);
    int newSize = std::max(16, thickness - MulDiv(4, dpi, 96));
    DWORD cur = (DWORD)SendMessageW(hToolbar, TB_GETBUTTONSIZE, 0, 0);
    if ((int)LOWORD(cur) != newSize || (int)HIWORD(cur) != newSize) {
        SendMessageW(hToolbar, TB_SETBUTTONSIZE, 0, MAKELONG(newSize, newSize));
    }
    int expectedH = newSize * btnCount;
    SetWindowPos(hToolbar, NULL, 0, 0, thickness, expectedH, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    RECT rowsRect{};
    SendMessageW(hToolbar, TB_SETROWS, MAKEWPARAM(btnCount, FALSE), (LPARAM)&rowsRect);
    SendMessageW(hToolbar, TB_AUTOSIZE, 0, 0);
}

static void RestoreToolbarHorizontalRow(HWND hToolbar) {
    if (!hToolbar || !IsWindow(hToolbar)) return;
    LONG_PTR curStyle = GetWindowLongPtrW(hToolbar, GWL_STYLE);
    if (curStyle & TBSTYLE_WRAPABLE) {
        SetWindowLongPtrW(hToolbar, GWL_STYLE, curStyle & ~TBSTYLE_WRAPABLE);
    }
    RECT rowsRect{};
    SendMessageW(hToolbar, TB_SETROWS, MAKEWPARAM(1, FALSE), (LPARAM)&rowsRect);
    SendMessageW(hToolbar, TB_AUTOSIZE, 0, 0);
}

static void ApplyTrayNotifyInternalLayout(HWND hWnd, bool vertical, int thickness) {
    if (!hWnd || !IsWindow(hWnd)) return;
    PurgeDestroyedTrayChildCacheEntries();
    int pos = 0;
    HWND hChild = GetWindow(hWnd, GW_CHILD);
    while (hChild) {
        HWND hNext = GetWindow(hChild, GW_HWNDNEXT);
        if (IsWindow(hChild)) {
            RECT rcChild{};
            GetWindowRect(hChild, &rcChild);
            int curW = rcChild.right - rcChild.left;
            int curH = rcChild.bottom - rcChild.top;
            WCHAR cls[64] = {};
            GetClassNameW(hChild, cls, ARRAYSIZE(cls));
            HWND hInnerToolbar = nullptr;
            if (_wcsicmp(cls, L"SysPager") == 0) {
                hInnerToolbar = FindWindowExW(hChild, NULL, L"ToolbarWindow32", NULL);
            } else if (_wcsicmp(cls, L"ToolbarWindow32") == 0) {
                hInnerToolbar = hChild;
            }
            int naturalLength = GetOrCacheTrayChildNaturalLength(hChild, curW, curH, !vertical);
            if (vertical) {
                int newW = thickness;
                int newH = std::max(naturalLength, 1);
                if (hInnerToolbar && IsWindow(hInnerToolbar)) {
                    ForceToolbarVerticalColumn(hInnerToolbar, thickness);
                    RECT rcInner{};
                    GetWindowRect(hInnerToolbar, &rcInner);
                    int innerH = rcInner.bottom - rcInner.top;
                    if (innerH > newH) newH = innerH;
                    SetWindowPos(hInnerToolbar, NULL, 0, 0, thickness, innerH, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
                }
                SetWindowPos(hChild, NULL, 0, pos, newW, newH, SWP_NOZORDER | SWP_NOACTIVATE);
                pos += newH;
            } else {
                if (hInnerToolbar && IsWindow(hInnerToolbar)) {
                    RestoreToolbarHorizontalRow(hInnerToolbar);
                }
            }
        }
        hChild = hNext;
    }
}

static LRESULT CALLBACK TrayNotifySubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR uIdSubclass) {
    if (!IsWindow(hWnd)) return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    
    switch (uMsg) {
    case WM_SIZE: {
        LRESULT ret = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        if (!g_modUnloading) {
            HWND hShellTray = GetAncestor(hWnd, GA_ROOT);
            ArmMasterLayoutTimer(hShellTray);
        }
        return ret;
    }
    case WM_NCDESTROY:
        RemoveWindowSubclass(hWnd, (SUBCLASSPROC)TrayNotifySubclassProc, uIdSubclass);
        UntrackSubclass(hWnd, TrayNotifySubclassProc);
        break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static void DoApplyVerticalTaskbarLayoutAtomic(HWND hShellTrayWnd) {
    static thread_local bool s_inResize = false;
    if (g_modUnloading || s_inResize || !IsWindow(hShellTrayWnd)) return;
    s_inResize = true;
    
    DWORD edge = TaskbarSettingsProvider::GetTaskbarEdge();
    bool vertical = IsEdgeVertical(edge);
    g_rotateVerticalIcons = vertical;
    
    SendMessageW(hShellTrayWnd, WM_SETREDRAW, FALSE, 0);
    HWND hReBar = FindWindowExW(hShellTrayWnd, NULL, L"ReBarWindow32", NULL);
    HWND hTrayNotify = FindWindowExW(hShellTrayWnd, NULL, L"TrayNotifyWnd", NULL);
    
    if (hReBar && IsWindow(hReBar)) SendMessageW(hReBar, WM_SETREDRAW, FALSE, 0);
    if (hTrayNotify && IsWindow(hTrayNotify)) SendMessageW(hTrayNotify, WM_SETREDRAW, FALSE, 0);
    
    if (vertical) {
        RECT rcClient{};
        GetClientRect(hShellTrayWnd, &rcClient);
        int thickness = rcClient.right - rcClient.left;
        int totalLength = rcClient.bottom - rcClient.top;
        if (thickness < 16 || thickness > 300) thickness = 40;
        int startBottom = thickness;
        int trayNotifyLength = EstimateTrayNotifyTotalLength(hTrayNotify);
        if (trayNotifyLength <= 0 || trayNotifyLength > totalLength) {
            trayNotifyLength = std::min(187, totalLength / 2);
        }
        int reBarTop = startBottom;
        int reBarLength = totalLength - startBottom - trayNotifyLength;
        if (reBarLength < 0) reBarLength = 0;
        
        if (hReBar && IsWindow(hReBar)) {
            SetWindowPos(hReBar, NULL, 0, reBarTop, thickness, reBarLength, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        if (hTrayNotify && IsWindow(hTrayNotify)) {
            int trayNotifyTop = totalLength - trayNotifyLength;
            if (trayNotifyTop < reBarTop) trayNotifyTop = reBarTop;
            SetWindowPos(hTrayNotify, NULL, 0, trayNotifyTop, thickness, trayNotifyLength, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        if (hReBar && IsWindow(hReBar)) {
            HWND hTaskSw = FindWindowExW(hReBar, NULL, L"MSTaskSwWClass", NULL);
            if (hTaskSw && IsWindow(hTaskSw)) ApplyTaskSwInternalLayout(hTaskSw, true, thickness);
        }
        if (hTrayNotify && IsWindow(hTrayNotify)) ApplyTrayNotifyInternalLayout(hTrayNotify, true, thickness);
    } else {
        if (hReBar && IsWindow(hReBar)) {
            HWND hTaskSw = FindWindowExW(hReBar, NULL, L"MSTaskSwWClass", NULL);
            if (hTaskSw && IsWindow(hTaskSw)) ApplyTaskSwInternalLayout(hTaskSw, false, 0);
        }
        if (hTrayNotify && IsWindow(hTrayNotify)) ApplyTrayNotifyInternalLayout(hTrayNotify, false, 0);
    }
    
    if (hTrayNotify && IsWindow(hTrayNotify)) SendMessageW(hTrayNotify, WM_SETREDRAW, TRUE, 0);
    if (hReBar && IsWindow(hReBar)) SendMessageW(hReBar, WM_SETREDRAW, TRUE, 0);
    SendMessageW(hShellTrayWnd, WM_SETREDRAW, TRUE, 0);
    RedrawWindow(hShellTrayWnd, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_ERASE | RDW_UPDATENOW);
    s_inResize = false;
}

static LRESULT CALLBACK ShellTrayWndSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR uIdSubclass) {
    if (!IsWindow(hWnd)) return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    
    switch (uMsg) {
    case WM_SIZE: {
        LRESULT ret = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        if (!g_modUnloading) ArmMasterLayoutTimer(hWnd);
        return ret;
    }
    case WM_TIMER:
        if (wParam == kTimerIdMasterLayout) {
            KillTimer(hWnd, kTimerIdMasterLayout);
            DoApplyVerticalTaskbarLayoutAtomic(hWnd);
            return 0;
        }
        break;
    case WM_NCDESTROY:
        KillTimer(hWnd, kTimerIdMasterLayout);
        RemoveWindowSubclass(hWnd, (SUBCLASSPROC)ShellTrayWndSubclassProc, uIdSubclass);
        UntrackSubclass(hWnd, ShellTrayWndSubclassProc);
        break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t CreateWindowExW_Original = nullptr;

static bool IsTaskbarWindowClass(HWND hWnd) {
    WCHAR cls[32];
    return GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) && 
           (_wcsicmp(cls, L"Shell_TrayWnd") == 0 || _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0);
}

static HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    
    if (hWnd && IsWindow(hWnd) && lpClassName && !IS_INTRESOURCE(lpClassName)) {
        if (_wcsicmp(lpClassName, L"MSTaskSwWClass") == 0) {
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, TaskSwSubclassProc, 0);
            TrackSubclass(hWnd, TaskSwSubclassProc);
        } else if (_wcsicmp(lpClassName, L"ReBarWindow32") == 0 && hWndParent && IsWindow(hWndParent) && IsTaskbarWindowClass(hWndParent)) {
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, ReBarSubclassProc, 0);
            TrackSubclass(hWnd, ReBarSubclassProc);
        } else if (_wcsicmp(lpClassName, L"TrayNotifyWnd") == 0 && hWndParent && IsWindow(hWndParent) && IsTaskbarWindowClass(hWndParent)) {
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, TrayNotifySubclassProc, 0);
            TrackSubclass(hWnd, TrayNotifySubclassProc);
        } else if (IsTaskbarWindowClass(hWnd)) {
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, ShellTrayWndSubclassProc, 0);
            TrackSubclass(hWnd, ShellTrayWndSubclassProc);
        }
    }
    return hWnd;
}

static BOOL CALLBACK SubclassExistingChildrenProc(HWND hWnd, LPARAM lParam) {
    if (!IsWindow(hWnd)) return TRUE;
    WCHAR cls[32];
    if (GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) {
        if (_wcsicmp(cls, L"MSTaskSwWClass") == 0) {
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, TaskSwSubclassProc, 0);
            TrackSubclass(hWnd, TaskSwSubclassProc);
        } else if (_wcsicmp(cls, L"ReBarWindow32") == 0) {
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, ReBarSubclassProc, 0);
            TrackSubclass(hWnd, ReBarSubclassProc);
        } else if (_wcsicmp(cls, L"TrayNotifyWnd") == 0) {
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, TrayNotifySubclassProc, 0);
            TrackSubclass(hWnd, TrayNotifySubclassProc);
        }
    }
    EnumChildWindows(hWnd, SubclassExistingChildrenProc, 0);
    return TRUE;
}

static BOOL CALLBACK UnsubclassExistingChildrenProc(HWND hWnd, LPARAM lParam) {
    if (!IsWindow(hWnd)) return TRUE;
    WCHAR cls[32];
    if (GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) {
        // This runs off the EnumWindows/EnumChildWindows walk started from
        // Wh_ModUninit, i.e. on the Windhawk uninit thread rather than the
        // window's own thread. Plain RemoveWindowSubclass would silently fail
        // here, and untracking it anyway would let the entry escape the
        // RemoveAllTrackedSubclasses safety net -> use the any-thread-safe
        // variant so the removal actually happens before we untrack it.
        if (_wcsicmp(cls, L"MSTaskSwWClass") == 0) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, (WindhawkUtils::WH_SUBCLASSPROC)TaskSwSubclassProc);
            UntrackSubclass(hWnd, TaskSwSubclassProc);
        } else if (_wcsicmp(cls, L"ReBarWindow32") == 0) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, (WindhawkUtils::WH_SUBCLASSPROC)ReBarSubclassProc);
            UntrackSubclass(hWnd, ReBarSubclassProc);
        } else if (_wcsicmp(cls, L"TrayNotifyWnd") == 0) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, (WindhawkUtils::WH_SUBCLASSPROC)TrayNotifySubclassProc);
            UntrackSubclass(hWnd, TrayNotifySubclassProc);
        }
    }
    EnumChildWindows(hWnd, UnsubclassExistingChildrenProc, 0);
    return TRUE;
}

static BOOL CALLBACK SubclassExistingTaskbarsEnumProc(HWND hWnd, LPARAM lParam) {
    if (IsWindow(hWnd) && IsTaskbarWindowClass(hWnd)) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, ShellTrayWndSubclassProc, 0);
        TrackSubclass(hWnd, ShellTrayWndSubclassProc);
        EnumChildWindows(hWnd, SubclassExistingChildrenProc, 0);
    }
    return TRUE;
}

static void SubclassExistingTaskbars() {
    EnumWindows(SubclassExistingTaskbarsEnumProc, 0);
}

static BOOL CALLBACK UnsubclassExistingTaskbarsEnumProc(HWND hWnd, LPARAM lParam) {
    if (IsWindow(hWnd) && IsTaskbarWindowClass(hWnd)) {
        // Ferma sempre il timer prima di rimuovere la subclass: se un WM_TIMER
        // fosse gi in coda, verrebbe comunque scartato da RemoveWindowSubclass,
        // ma questo evita ogni ambiguit in caso di dispatch concorrente.
        KillTimer(hWnd, kTimerIdMasterLayout);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, (WindhawkUtils::WH_SUBCLASSPROC)ShellTrayWndSubclassProc);
        UntrackSubclass(hWnd, ShellTrayWndSubclassProc);
        EnumChildWindows(hWnd, UnsubclassExistingChildrenProc, 0);
    }
    return TRUE;
}

static void UnsubclassExistingTaskbars() {
    EnumWindows(UnsubclassExistingTaskbarsEnumProc, 0);
}

// Forces every taskbar (the primary Shell_TrayWnd AND every
// Shell_SecondaryTrayWnd on other monitors) to immediately re-read
// TaskbarGlomLevel/MMTaskbarGlomLevel and re-lay out its buttons. A single
// WM_SETTINGCHANGE broadcast is enough for Explorer to update the primary bar
// right away, but secondary-monitor bars often don't repaint until they get
// their own resize/redraw kick, so we walk every taskbar window explicitly
// instead of only refreshing the one found by FindWindowExW(nullptr, nullptr, ...).
static BOOL CALLBACK RefreshTaskbarGroupingEnumProc(HWND hWnd, LPARAM lParam) {
    if (!IsWindow(hWnd) || !IsTaskbarWindowClass(hWnd)) return TRUE;

    SendMessageW(hWnd, WM_SETTINGCHANGE, 0, (LPARAM)L"TraySettings");

    HWND hReBar = FindWindowExW(hWnd, NULL, L"ReBarWindow32", NULL);
    if (hReBar && IsWindow(hReBar)) {
        SendMessageW(hReBar, WM_SIZE, 0, 0);
        HWND hTaskSw = FindWindowExW(hReBar, NULL, L"MSTaskSwWClass", NULL);
        if (hTaskSw && IsWindow(hTaskSw)) {
            SendMessageW(hTaskSw, TB_AUTOSIZE, 0, 0);
            InvalidateRect(hTaskSw, NULL, TRUE);
            UpdateWindow(hTaskSw);
        }
    }
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
    return TRUE;
}

static void RefreshTaskbarGroupingOnAllScreens() {
    EnumWindows(RefreshTaskbarGroupingEnumProc, 0);
}

static void UpdateStuckRectsKey(LPCWSTR keyName, DWORD newEdge, DWORD edgeOffset) {
    WCHAR fullKey[256];
    swprintf_s(fullKey, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\%s", keyName);
    RegKey regKey;
    if (regKey.Open(HKEY_CURRENT_USER, fullKey, KEY_READ | KEY_WRITE)) {
        DWORD sz = 0;
        if (regKey.QueryValue(L"Settings", NULL, &sz) && sz >= edgeOffset + 4) {
            std::vector<BYTE> d(sz);
            if (regKey.QueryValue(L"Settings", d.data(), &sz)) {
                *(DWORD*)(d.data() + edgeOffset) = newEdge;
                regKey.SetDWord(L"Settings", (DWORD)0);
                RegSetValueExW(regKey.Get(), L"Settings", 0, REG_BINARY, d.data(), (DWORD)d.size());
            }
        }
    }
}

static void UpdateAllStuckRects(DWORD newEdge) {
    UpdateStuckRectsKey(L"StuckRects2", newEdge, 12);
    UpdateStuckRectsKey(L"StuckRects3", newEdge, 12);
    UpdateStuckRectsKey(L"StuckRectsLegacy", newEdge, 12);
}

static void RotateTaskbarPosition(DWORD newEdge) {
    if (newEdge > 3) return;
    if (newEdge == g_lastEdge) return;
    
    if (!g_windowsVersion.IsSupported()) {
        Wh_Log(L"RotateTaskbarPosition: Unsupported Windows version");
        return;
    }
    
    if (g_windowsVersion.IsBuildAtLeast(22000) && (newEdge == 0 || newEdge == 2)) {
        Wh_Log(L"RotateTaskbarPosition: Windows 11 vertical positioning may have limited support");
    }
    
    // NOTE: this used to also write TaskbarSide under ...\Advanced, but the mod
    // only ever reads the edge back from StuckRects (GetTaskbarEdge), never from
    // TaskbarSide, so that write had no effect and has been removed.
    UpdateAllStuckRects(newEdge);
    g_lastEdge = newEdge;
    
    HWND hTray = FindWindowExW(nullptr, nullptr, L"Shell_TrayWnd", nullptr);
    if (hTray && IsWindow(hTray)) {
        SendMessageW(hTray, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);
        SendMessageW(hTray, WM_SIZE, 0, 0);
        HWND hReBar = FindWindowExW(hTray, NULL, L"ReBarWindow32", NULL);
        if (hReBar && IsWindow(hReBar)) {
            SendMessageW(hReBar, WM_SIZE, 0, 0);
            HWND hTaskSw = FindWindowExW(hReBar, NULL, L"MSTaskSwWClass", NULL);
            if (hTaskSw && IsWindow(hTaskSw)) {
                SendMessageW(hTaskSw, TB_AUTOSIZE, 0, 0);
                InvalidateRect(hTaskSw, NULL, TRUE);
            }
        }
        HWND hTrayNotify = FindWindowExW(hTray, NULL, L"TrayNotifyWnd", NULL);
        if (hTrayNotify && IsWindow(hTrayNotify)) {
            SendMessageW(hTrayNotify, WM_SIZE, 0, 0);
            InvalidateRect(hTrayNotify, NULL, TRUE);
        }
        SetWindowRgn(hTray, nullptr, TRUE);
        InvalidateRect(hTray, NULL, TRUE);
        UpdateWindow(hTray);
        SendMessageW(hTray, WM_SETTINGCHANGE, 0, (LPARAM)L"TraySettings");
    }
    SendMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"TraySettings");
    SystemParametersInfoW(SPI_SETWORKAREA, 0, NULL, SPIF_SENDCHANGE);
}

static void ApplySettings(HWND hwnd) {
    if (!IsWindow(hwnd)) return;
    
    bool lock = (SendDlgItemMessageW(hwnd, IDC_CHK_LOCK, BM_GETCHECK, 0, 0) == BST_CHECKED);
    bool hide = (SendDlgItemMessageW(hwnd, IDC_CHK_HIDE, BM_GETCHECK, 0, 0) == BST_CHECKED);
    bool small_i = (SendDlgItemMessageW(hwnd, IDC_CHK_SMALL, BM_GETCHECK, 0, 0) == BST_CHECKED);
    bool aero = (SendDlgItemMessageW(hwnd, IDC_CHK_AEROPEEK, BM_GETCHECK, 0, 0) == BST_CHECKED);
    DWORD glom = (DWORD)SendDlgItemMessageW(hwnd, IDC_COMBO_BUTTONS, CB_GETCURSEL, 0, 0);
    DWORD locSel = (DWORD)SendDlgItemMessageW(hwnd, IDC_COMBO_LOCATION, CB_GETCURSEL, 0, 0);
    
    TaskbarSettingsProvider::SetLockState(lock);
    TaskbarSettingsProvider::SetSmallIcons(small_i);
    TaskbarSettingsProvider::SetGlomLevel(glom);
    TaskbarSettingsProvider::SetAeroPeekEnabled(aero);
    
    DWORD edge = (locSel == 0) ? 1 : 3;
    RotateTaskbarPosition(edge);
    
    APPBARDATA abd = { sizeof(APPBARDATA) };
    abd.hWnd = FindWindowW(L"Shell_TrayWnd", NULL);
    if (abd.hWnd && IsWindow(abd.hWnd)) {
        abd.lParam = hide ? ABS_AUTOHIDE : ABS_ALWAYSONTOP;
        SHAppBarMessage(ABM_SETSTATE, &abd);
    }
    
    DWORD powerSel = (DWORD)SendDlgItemMessageW(hwnd, IDC_COMBO_POWER, CB_GETCURSEL, 0, 0);
    if (powerSel < 7) TaskbarSettingsProvider::SetPowerAction(kPowerValues[powerSel]);
    
    bool mruProg = (SendDlgItemMessageW(hwnd, IDC_CHK_MRU_PROG, BM_GETCHECK, 0, 0) == BST_CHECKED);
    bool mruItems = (SendDlgItemMessageW(hwnd, IDC_CHK_MRU_ITEMS, BM_GETCHECK, 0, 0) == BST_CHECKED);
    TaskbarSettingsProvider::SetStartMruProgs(mruProg);
    TaskbarSettingsProvider::SetStartMruItems(mruItems);
    
    HWND hList = GetDlgItem(hwnd, IDC_LST_TOOLBARS);
    if (hList && IsWindow(hList)) {
        bool addr = (ListView_GetCheckState(hList, 0) != 0);
        bool links = (ListView_GetCheckState(hList, 1) != 0);
        bool tablet = (ListView_GetCheckState(hList, 2) != 0);
        bool desk = false;
        ApplyToolbars(addr, links, tablet, desk);
    }
    SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"TraySettings");
    // Broadcasting alone isn't enough to make "Always combine, hide labels" /
    // "Combine when full" / "Never combine" show up on secondary-monitor
    // taskbars right away, so explicitly kick every taskbar on every screen.
    RefreshTaskbarGroupingOnAllScreens();
}

static INT_PTR CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (!IsWindow(hwnd) && msg != WM_INITDIALOG) return FALSE;
    
    switch (msg) {
    case WM_INITDIALOG: {
        HICON hIcon = GetSystemIcon(SIID_TASKBAR);
        if (hIcon) {
            SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
            SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        }
        g_hwndMain = hwnd;
        g_currentTab = 0;
        ApplyDarkTitlebar(hwnd);
        ApplyExplorerThemeToChildren(hwnd);
        EnableThemeDialogTexture(hwnd, ETDT_ENABLETAB);
        
        HDC hdc = GetDC(hwnd);
        int ptPx = -MulDiv(9, GetDeviceCaps(hdc, LOGPIXELSY), 72);
        ReleaseDC(hwnd, hdc);
        
        if (g_hFontUi) { DeleteObject(g_hFontUi); g_hFontUi = NULL; }
        g_hFontUi = CreateFontW(ptPx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        
        SetWindowTextW(hwnd, g_str.title);
        
        HWND hTab = GetDlgItem(hwnd, IDC_TAB_MAIN);
        SendMessageW(hTab, TCM_DELETEALLITEMS, 0, 0);
        auto addTab = [&](int i, const WCHAR* s) {
            TCITEMW ti = { TCIF_TEXT, 0, 0, (LPWSTR)s };
            SendMessageW(hTab, TCM_INSERTITEMW, i, (LPARAM)&ti);
        };
        addTab(0, g_str.tab_taskbar);
        addTab(1, g_str.tab_start);
        addTab(2, g_str.tab_toolbars);
        
        const DialogControlBinding bindings[] = {
            { IDC_GRP_APPEARANCE, g_str.grp_appearance }, { IDC_CHK_LOCK, g_str.chk_lock },
            { IDC_CHK_HIDE, g_str.chk_hide }, { IDC_CHK_SMALL, g_str.chk_small },
            { IDC_TXT_LOCATION, g_str.txt_location }, { IDC_TXT_BUTTONS, g_str.txt_buttons },
            { IDC_GRP_NOTIF, g_str.grp_notif }, { IDC_TXT_NOTIF, g_str.txt_notif },
            { IDC_BTN_CUST_NOTIF, g_str.btn_cust_notif }, { IDC_GRP_AERO, g_str.grp_aero },
            { IDC_TXT_AERO, g_str.txt_aero }, { IDC_CHK_AEROPEEK, g_str.chk_aero },
            { IDC_TXT_START_INFO, g_str.start_info }, { IDC_BTN_START_CUST, g_str.btn_start_cust },
            { IDC_TXT_POWER_LABEL, g_str.txt_power_label }, { IDC_GRP_PRIVACY, g_str.grp_privacy },
            { IDC_CHK_MRU_PROG, g_str.chk_mru_prog }, { IDC_CHK_MRU_ITEMS, g_str.chk_mru_items },
            { IDC_TXT_TOOLBARS_INFO, g_str.toolbars_info }, { IDOK, g_str.btn_ok },
            { IDCANCEL, g_str.btn_cancel }, { IDC_BTN_APPLY, g_str.btn_apply },
            { IDC_LINK_HELP, g_str.link_help },
        };
        for (const auto& b : bindings) {
            SetDlgItemTextW(hwnd, b.controlId, b.text);
        }
        
        if (g_hFontUi) {
            SendMessageW(hwnd, WM_SETFONT, (WPARAM)g_hFontUi, TRUE);
            SetFontAllChildren(hwnd, g_hFontUi);
        }
        
        BalanceTextAndCombo(hwnd, IDC_TXT_LOCATION, IDC_COMBO_LOCATION);
        BalanceTextAndCombo(hwnd, IDC_TXT_BUTTONS, IDC_COMBO_BUTTONS);
        BalanceTextAndCombo(hwnd, IDC_TXT_POWER_LABEL, IDC_COMBO_POWER);
        AutoFitGroupboxes(hwnd);
        
        HWND hCL = GetDlgItem(hwnd, IDC_COMBO_LOCATION);
        SendMessageW(hCL, CB_RESETCONTENT, 0, 0);
        SendMessageW(hCL, CB_ADDSTRING, 0, (LPARAM)g_str.pos_top);
        SendMessageW(hCL, CB_ADDSTRING, 0, (LPARAM)g_str.pos_bottom);
        EnableWindow(hCL, TRUE);
        
        DWORD edge = TaskbarSettingsProvider::GetTaskbarEdge();
        int newIndex = (edge == 1) ? 0 : 1;
        SendMessageW(hCL, CB_SETCURSEL, (WPARAM)newIndex, 0);
        
        HWND hCB = GetDlgItem(hwnd, IDC_COMBO_BUTTONS);
        SendMessageW(hCB, CB_RESETCONTENT, 0, 0);
        SendMessageW(hCB, CB_ADDSTRING, 0, (LPARAM)g_str.btn_always_combine);
        SendMessageW(hCB, CB_ADDSTRING, 0, (LPARAM)g_str.btn_combine_full);
        SendMessageW(hCB, CB_ADDSTRING, 0, (LPARAM)g_str.btn_never_combine);
        
        HWND hCP = GetDlgItem(hwnd, IDC_COMBO_POWER);
        SendMessageW(hCP, CB_RESETCONTENT, 0, 0);
        SendMessageW(hCP, CB_ADDSTRING, 0, (LPARAM)g_str.power_shutdown);
        SendMessageW(hCP, CB_ADDSTRING, 0, (LPARAM)g_str.power_restart);
        SendMessageW(hCP, CB_ADDSTRING, 0, (LPARAM)g_str.power_sleep);
        SendMessageW(hCP, CB_ADDSTRING, 0, (LPARAM)g_str.power_hibernate);
        SendMessageW(hCP, CB_ADDSTRING, 0, (LPARAM)g_str.power_logoff);
        SendMessageW(hCP, CB_ADDSTRING, 0, (LPARAM)g_str.power_lock);
        SendMessageW(hCP, CB_ADDSTRING, 0, (LPARAM)g_str.power_switchuser);
        
        DWORD szMove = TaskbarSettingsProvider::RegGetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarSizeMove", 1);
        DWORD szSmall = TaskbarSettingsProvider::RegGetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarSmallIcons", 0);
        DWORD glom = TaskbarSettingsProvider::GetGlomLevel();
        bool aeroPeek = TaskbarSettingsProvider::GetAeroPeekEnabled();
        
        APPBARDATA abd = { sizeof(APPBARDATA) };
        abd.hWnd = FindWindowW(L"Shell_TrayWnd", NULL);
        bool isHide = false;
        if (abd.hWnd && IsWindow(abd.hWnd)) {
            isHide = (SHAppBarMessage(ABM_GETSTATE, &abd) & ABS_AUTOHIDE) != 0;
        }
        
        SendDlgItemMessageW(hwnd, IDC_CHK_LOCK, BM_SETCHECK, (szMove == 0) ? BST_CHECKED : BST_UNCHECKED, 0);
        SendDlgItemMessageW(hwnd, IDC_CHK_HIDE, BM_SETCHECK, isHide ? BST_CHECKED : BST_UNCHECKED, 0);
        SendDlgItemMessageW(hwnd, IDC_CHK_SMALL, BM_SETCHECK, (szSmall != 0) ? BST_CHECKED : BST_UNCHECKED, 0);
        SendDlgItemMessageW(hwnd, IDC_CHK_AEROPEEK, BM_SETCHECK, aeroPeek ? BST_CHECKED : BST_UNCHECKED, 0);
        SendMessageW(hCB, CB_SETCURSEL, (glom < 3) ? (WPARAM)glom : 0, 0);
        
        DWORD curPower = TaskbarSettingsProvider::GetPowerAction();
        int powerIdx = 0;
        for (int i = 0; i < 7; i++) {
            if (kPowerValues[i] == curPower) { powerIdx = i; break; }
        }
        SendMessageW(hCP, CB_SETCURSEL, powerIdx, 0);
        
        SendDlgItemMessageW(hwnd, IDC_CHK_MRU_PROG, BM_SETCHECK, TaskbarSettingsProvider::GetStartMruProgs() ? BST_CHECKED : BST_UNCHECKED, 0);
        SendDlgItemMessageW(hwnd, IDC_CHK_MRU_ITEMS, BM_SETCHECK, TaskbarSettingsProvider::GetStartMruItems() ? BST_CHECKED : BST_UNCHECKED, 0);
        
        InitToolbarsList(GetDlgItem(hwnd, IDC_LST_TOOLBARS));
        EnableWindow(GetDlgItem(hwnd, IDC_BTN_APPLY), FALSE);
        SwitchTab(hwnd, 0);
        
        RECT rc; GetWindowRect(hwnd, &rc);
        int wh = rc.bottom - rc.top;
        {
            MONITORINFO mi = { sizeof(mi) };
            HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
            GetMonitorInfo(hMon, &mi);
            const int kEdgeMargin = 8;
            int x = mi.rcWork.left + kEdgeMargin;
            int y = mi.rcWork.bottom - wh - kEdgeMargin;
            SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }
        
        LONG style = GetWindowLongW(hwnd, GWL_STYLE);
        style &= ~WS_THICKFRAME; style &= ~WS_MAXIMIZEBOX;
        SetWindowLongW(hwnd, GWL_STYLE, style);
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
        return TRUE;
    }
    case WM_GETMINMAXINFO: {
        MINMAXINFO* mmi = (MINMAXINFO*)lp;
        RECT rc; GetWindowRect(hwnd, &rc);
        mmi->ptMinTrackSize.x = mmi->ptMaxTrackSize.x = rc.right - rc.left;
        mmi->ptMinTrackSize.y = mmi->ptMaxTrackSize.y = rc.bottom - rc.top;
        return 0;
    }
    case WM_COMMAND: {
        WORD id = LOWORD(wp);
        WORD act = HIWORD(wp);
        if ((act == BN_CLICKED || act == CBN_SELCHANGE) &&
            id != IDOK && id != IDCANCEL && id != IDC_BTN_APPLY &&
            id != IDC_BTN_CUST_NOTIF && id != IDC_BTN_START_CUST) {
            EnableWindow(GetDlgItem(hwnd, IDC_BTN_APPLY), TRUE);
        }
        if (id == IDOK) {
            ApplySettings(hwnd);
            if (IsWindow(hwnd)) DestroyWindow(hwnd);
        } else if (id == IDCANCEL) {
            DestroyWindow(hwnd);
        } else if (id == IDC_BTN_APPLY) {
            ApplySettings(hwnd);
            if (IsWindow(hwnd)) EnableWindow(GetDlgItem(hwnd, IDC_BTN_APPLY), FALSE);
        } else if (id == IDC_BTN_CUST_NOTIF) {
            HINSTANCE hRes = ShellExecuteW(hwnd, L"open", L"shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}", NULL, NULL, SW_SHOW);
            if ((INT_PTR)hRes <= 32) {
                ShellExecuteW(hwnd, L"open", L"control.exe", L"/name Microsoft.NotificationAreaIcons", NULL, SW_SHOW);
            }
        } else if (id == IDC_BTN_START_CUST) {
            ShowStartCustomDialog(hwnd);
        }
        break;
    }
    case WM_NOTIFY: {
        NMHDR* hdr = (NMHDR*)lp;
        if (hdr->idFrom == IDC_TAB_MAIN && hdr->code == TCN_SELCHANGE) {
            int sel = (int)SendDlgItemMessageW(hwnd, IDC_TAB_MAIN, TCM_GETCURSEL, 0, 0);
            SwitchTab(hwnd, sel);
        }
        if (hdr->idFrom == IDC_LST_TOOLBARS && hdr->code == LVN_ITEMCHANGED) {
            NMLISTVIEW* nmlv = (NMLISTVIEW*)lp;
            if ((nmlv->uChanged & LVIF_STATE) && ((nmlv->uNewState ^ nmlv->uOldState) & LVIS_STATEIMAGEMASK)) {
                EnableWindow(GetDlgItem(hwnd, IDC_BTN_APPLY), TRUE);
            }
        }
        if (hdr->idFrom == IDC_LINK_HELP && hdr->code == NM_CLICK) {
            ShowAboutDialog(hwnd);
        }
        break;
    }
    case WM_DESTROY:
        if (g_hFontUi) { DeleteObject(g_hFontUi); g_hFontUi = NULL; }
        g_hwndMain = NULL;
        InterlockedExchange(&g_dialogOpen, 0);
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    }
    return FALSE;
}

static HWND BuildAndShowDialog() {
    BYTE* buf = new BYTE[8192];
    BYTE* p = buf;
    int controlCount = 0;
    auto align4 = [](BYTE*& ptr) { ptr = (BYTE*)(((UINT_PTR)ptr + 3) & ~3); };
    
    LPDLGTEMPLATEW pDlg = (LPDLGTEMPLATEW)p;
    pDlg->style = DS_SETFONT | DS_MODALFRAME | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU;
    pDlg->dwExtendedStyle = 0;
    pDlg->cdit = 0;
    pDlg->x = 0; pDlg->y = 0;
    pDlg->cx = DialogSizes::MAIN_WIDTH;
    pDlg->cy = DialogSizes::MAIN_HEIGHT;
    p += sizeof(DLGTEMPLATE);
    *(WORD*)p = 0; p += 2;
    *(WORD*)p = 0; p += 2;
    StringCchCopyW((WCHAR*)p, 1, L""); p += 2;
    *(WORD*)p = 9; p += 2;
    StringCchCopyW((WCHAR*)p, 10, L"Segoe UI");
    p += (lstrlenW(L"Segoe UI") + 1) * 2;
    
    auto addCtrl = [&](DWORD style, DWORD exStyle, short x, short y, short cx, short cy, WORD id, LPCWSTR cls, LPCWSTR cap) {
        align4(p);
        LPDLGITEMTEMPLATE pi = (LPDLGITEMTEMPLATE)p;
        pi->style = WS_CHILD | WS_VISIBLE | style;
        pi->dwExtendedStyle = exStyle;
        pi->x = x; pi->y = y; pi->cx = cx; pi->cy = cy; pi->id = id;
        p += sizeof(DLGITEMTEMPLATE);
        StringCchCopyW((WCHAR*)p, lstrlenW(cls) + 1, cls);
        p += (lstrlenW(cls) + 1) * 2;
        StringCchCopyW((WCHAR*)p, lstrlenW(cap) + 1, cap);
        p += (lstrlenW(cap) + 1) * 2;
        *(WORD*)p = 0; p += 2;
        controlCount++;
    };
    
    addCtrl(TCS_TABS | WS_TABSTOP, 0, 6, 6, 250, 232, IDC_TAB_MAIN, L"SysTabControl32", L"");
    addCtrl(BS_GROUPBOX, 0, 12, 22, 238, 86, IDC_GRP_APPEARANCE, L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 18, 33, 226, 10, IDC_CHK_LOCK, L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 18, 45, 226, 10, IDC_CHK_HIDE, L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 18, 57, 226, 10, IDC_CHK_SMALL, L"Button", L"");
    addCtrl(SS_LEFT | SS_EDITCONTROL, 0, 18, 73, 110, 10, IDC_TXT_LOCATION, L"Static", L"");
    addCtrl(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 0, 132, 71, 112, 100, IDC_COMBO_LOCATION, L"ComboBox", L"");
    addCtrl(SS_LEFT | SS_EDITCONTROL, 0, 18, 89, 95, 18, IDC_TXT_BUTTONS, L"Static", L"");
    addCtrl(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 0, 80, 87, 164, 70, IDC_COMBO_BUTTONS, L"ComboBox", L"");
    addCtrl(BS_GROUPBOX, 0, 12, 112, 238, 44, IDC_GRP_NOTIF, L"Button", L"");
    addCtrl(SS_LEFT, 0, 18, 123, 148, 26, IDC_TXT_NOTIF, L"Static", L"");
    addCtrl(BS_PUSHBUTTON | WS_TABSTOP, 0, 172, 126, 70, 14, IDC_BTN_CUST_NOTIF, L"Button", L"");
    addCtrl(BS_GROUPBOX, 0, 12, 158, 238, 60, IDC_GRP_AERO, L"Button", L"");
    addCtrl(SS_LEFT, 0, 18, 169, 226, 30, IDC_TXT_AERO, L"Static", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 18, 203, 226, 12, IDC_CHK_AEROPEEK, L"Button", L"");
    addCtrl(WS_TABSTOP, 0, 12, 222, 238, 10, IDC_LINK_HELP, L"SysLink", L"");
    // Standard Windows dialog button size is 50x14 DLU. The row is right-aligned to
    // x=250, the same right margin used by the group boxes/list above (12 DLU from
    // the 262-wide dialog edge), then shifted ~8% further right versus the previous
    // 84/140/196 layout - the max shift that still keeps Apply inside the dialog.
    addCtrl(BS_DEFPUSHBUTTON | WS_TABSTOP, 0, 88, 252, 50, 14, IDOK, L"Button", L"OK");
    addCtrl(BS_PUSHBUTTON | WS_TABSTOP, 0, 144, 252, 50, 14, IDCANCEL, L"Button", L"Cancel");
    addCtrl(BS_PUSHBUTTON | WS_TABSTOP, 0, 200, 252, 50, 14, IDC_BTN_APPLY, L"Button", L"Apply");
    addCtrl(SS_LEFT, 0, 14, 22, 162, 26, IDC_TXT_START_INFO, L"Static", L"");
    addCtrl(BS_PUSHBUTTON | WS_TABSTOP, 0, 180, 22, 66, 14, IDC_BTN_START_CUST, L"Button", L"");
    addCtrl(SS_LEFT, 0, 14, 56, 100, 10, IDC_TXT_POWER_LABEL, L"Static", L"");
    addCtrl(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 0, 116, 54, 130, 80, IDC_COMBO_POWER, L"ComboBox", L"");
    addCtrl(BS_GROUPBOX, 0, 12, 74, 238, 62, IDC_GRP_PRIVACY, L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP | BS_MULTILINE, 0, 18, 86, 226, 18, IDC_CHK_MRU_PROG, L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP | BS_MULTILINE, 0, 18, 108, 226, 18, IDC_CHK_MRU_ITEMS, L"Button", L"");
    addCtrl(SS_LEFT, 0, 14, 22, 234, 18, IDC_TXT_TOOLBARS_INFO, L"Static", L"");
    addCtrl(LVS_REPORT | LVS_NOCOLUMNHEADER | LVS_SINGLESEL | WS_BORDER | WS_TABSTOP, 0, 16, 44, 230, 165, IDC_LST_TOOLBARS, L"SysListView32", L"");
    
    pDlg->cdit = controlCount;
    HWND hwnd = CreateDialogIndirectParamW(GetModuleHandleW(NULL), (LPDLGTEMPLATE)buf, NULL, DlgProc, 0);
    delete[] buf;
    return hwnd;
}

static DWORD WINAPI DialogThreadProc(LPVOID) {
    EnsureThemeActCtx();
    ULONG_PTR cookie = 0;
    BOOL actCtxActive = FALSE;
    if (g_hActCtx != INVALID_HANDLE_VALUE) {
        actCtxActive = ActivateActCtx(g_hActCtx, &cookie);
    }
    INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_TAB_CLASSES | ICC_LINK_CLASS | ICC_WIN95_CLASSES | ICC_LISTVIEW_CLASSES };
    InitCommonControlsEx(&icex);
    HWND hwnd = BuildAndShowDialog();
    if (!hwnd) {
        if (actCtxActive) DeactivateActCtx(0, cookie);
        InterlockedExchange(&g_dialogOpen, 0);
        return 1;
    }
    ShowWindow(hwnd, SW_SHOWNORMAL);
    SetForegroundWindow(hwnd);
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        if (!IsDialogMessageW(hwnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (!IsWindow(hwnd)) break;
    }
    if (actCtxActive) DeactivateActCtx(0, cookie);
    g_hwndMain = NULL;
    InterlockedExchange(&g_dialogOpen, 0);
    return 0;
}

static void ShowTaskbarProperties() {
    if (InterlockedExchange(&g_dialogOpen, 1)) {
        HWND hw = g_hwndMain;
        if (hw && IsWindow(hw)) {
            SetForegroundWindow(hw);
            if (IsIconic(hw)) ShowWindow(hw, SW_RESTORE);
        }
        return;
    }
    
    if (g_dialogThread) {
        DWORD waitResult = WaitForSingleObject(g_dialogThread, 3000);
        if (waitResult == WAIT_TIMEOUT) {
            Wh_Log(L"ShowTaskbarProperties: Previous thread timed out, continuing anyway");
        }
        CloseHandle(g_dialogThread);
        g_dialogThread = NULL;
    }
    
    g_dialogThread = CreateThread(NULL, 0, DialogThreadProc, NULL, 0, NULL);
    if (!g_dialogThread) {
        Wh_Log(L"ShowTaskbarProperties: Failed to create thread");
        InterlockedExchange(&g_dialogOpen, 0);
    }
}

BOOL WINAPI ShellExecuteExW_hook(SHELLEXECUTEINFOW* pei) {
    HookGuard guard;
    if (guard.IsReentrant()) return ShellExecuteExW_orig(pei);
    
    if (pei && pei->lpFile) {
        if (_wcsnicmp(pei->lpFile, L"ms-settings:taskbar", 19) == 0) {
            ShowTaskbarProperties();
            if (pei->fMask & SEE_MASK_NOCLOSEPROCESS) pei->hProcess = NULL;
            pei->hInstApp = SHELL_EXECUTE_SUCCESS;
            return TRUE;
        }
    }
    return ShellExecuteExW_orig(pei);
}

HINSTANCE WINAPI ShellExecuteW_hook(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd) {
    HookGuard guard;
    if (guard.IsReentrant()) return ShellExecuteW_orig(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
    
    if (lpFile && _wcsnicmp(lpFile, L"ms-settings:taskbar", 19) == 0) {
        ShowTaskbarProperties();
        return SHELL_EXECUTE_SUCCESS;
    }
    return ShellExecuteW_orig(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
}

BOOL Wh_ModInit() {
    if (!GetWindowsVersion(&g_windowsVersion)) {
        Wh_Log(L"Wh_ModInit: Failed to get Windows version");
    } else if (!g_windowsVersion.IsSupported()) {
        Wh_Log(L"Wh_ModInit: Unsupported Windows version detected, some features may not work");
    } else {
        Wh_Log(L"Wh_ModInit: Windows version %lu.%lu.%lu supported",
               g_windowsVersion.majorVersion, g_windowsVersion.minorVersion, g_windowsVersion.buildNumber);
    }
    
    InterlockedExchange(&g_dialogOpen, 0);
    InterlockedExchange(&g_startCustomOpen, 0);
    LoadLanguageSetting();
    InitLocalization();
    EnsureThemeActCtx();
    g_lastEdge = TaskbarSettingsProvider::GetTaskbarEdge();
    
    InstallTaskbarPositionHooks();
    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Original);
    SubclassExistingTaskbars();
    
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (hShell32) {
        Wh_SetFunctionHook(
            (void*)GetProcAddress(hShell32, "ShellExecuteExW"),
            (void*)ShellExecuteExW_hook,
            (void**)&ShellExecuteExW_orig);
        Wh_SetFunctionHook(
            (void*)GetProcAddress(hShell32, "ShellExecuteW"),
            (void*)ShellExecuteW_hook,
            (void**)&ShellExecuteW_orig);
    }
    return TRUE;
}

void Wh_ModUninit() {
    g_modUnloading = true;
    
    if (g_taskbarPosHooksInstalled && g_lastEdge != 3) {
        TaskbarSettingsProvider::RegSetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", L"TaskbarSide", 3);
        UpdateAllStuckRects(3);
        ClearRotatedIconCache();
        HWND hTray = FindWindowExW(nullptr, nullptr, L"Shell_TrayWnd", nullptr);
        if (hTray && IsWindow(hTray)) {
            SendMessageW(hTray, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);
            SendMessageW(hTray, WM_SETTINGCHANGE, 0, (LPARAM)L"TraySettings");
        }
    }
    // Va rimossa SEMPRE e incondizionatamente, non solo quando il bordo va
    // resettato: la subclass punta a codice dentro la DLL del mod, che sta per
    // essere scaricata. Se resta agganciata, il prossimo messaggio inviato alla
    // finestra del taskbar esegue codice non pi mappato in memoria e manda in
    // crash/instabilit explorer.exe, costringendo al riavvio.
    UnsubclassExistingTaskbars();

    // Stessa identica falla poteva verificarsi sulla toolbar Desktop, se attiva:
    // la subclass veniva rimossa solo su WM_NCDESTROY o disattivazione manuale,
    // mai qui. Va tolta sempre, a prescindere dallo stato della toolbar.
    {
        HWND hBand = FindDesktopToolbarWindow();
        if (hBand && IsWindow(hBand)) {
            // Same thread caveat as above: this executes on the
            // Wh_ModUninit thread, not the Desktop toolbar's owning thread.
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hBand, (WindhawkUtils::WH_SUBCLASSPROC)DesktopBandSubclassProc);
            UntrackSubclass(hBand, DesktopBandSubclassProc);
        }
    }

    // Rete di sicurezza definitiva: le pulizie sopra si basano sul *ritrovare*
    // le finestre (EnumWindows/ricerca per classe), e possono saltarne alcune
    // in configurazioni particolari (monitor secondari, desktop virtuali,
    // finestre nascoste, versioni di Windows con gerarchie diverse). Questa
    // sweep finale, invece, scorre il registro di TUTTO ci che il mod ha
    // effettivamente subclassato durante la sua vita e lo rimuove comunque,
    // indipendentemente da dove si trovi. Qualunque cosa sia sfuggita alle
    // pulizie mirate qui sopra viene comunque tolta.
    RemoveAllTrackedSubclasses();

    g_taskbarPosHooksInstalled = false;
    
    if (g_hwndMain && IsWindow(g_hwndMain)) {
        PostMessageW(g_hwndMain, WM_CLOSE, 0, 0);
    }
    if (g_hwndStartCustom && IsWindow(g_hwndStartCustom)) {
        PostMessageW(g_hwndStartCustom, WM_CLOSE, 0, 0);
    }
    
    if (g_dialogThread) {
        DWORD waitResult = WaitForSingleObject(g_dialogThread, 3000);
        if (waitResult == WAIT_TIMEOUT) {
            Wh_Log(L"Wh_ModUninit: Dialog thread timeout");
        }
        CloseHandle(g_dialogThread);
        g_dialogThread = NULL;
    }
    
    if (g_hFontUi) { DeleteObject(g_hFontUi); g_hFontUi = NULL; }
    if (g_hStartFontUi) { DeleteObject(g_hStartFontUi); g_hStartFontUi = NULL; }
    if (g_hActCtx != INVALID_HANDLE_VALUE) {
        ReleaseActCtx(g_hActCtx);
        g_hActCtx = INVALID_HANDLE_VALUE;
    }
    
    ClearRotatedIconCache();
}

void Wh_ModSettingsChanged() {
    LoadLanguageSetting();
    InitLocalization();
}
