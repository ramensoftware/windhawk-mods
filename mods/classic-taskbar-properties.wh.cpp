// ==WindhawkMod==
// @id classic-taskbar-properties
// @name Classic Taskbar and Start Menu Properties
// @description Restores the classic "Taskbar and Start Menu Properties" dialog with full functionality. Includes all tabs (Taskbar, Navigation, Start Menu, Toolbars). Designed for Windows 10 and 11. Now with Windows 7/8.1 profile switching.
// @version 2.4.0
// @author babamohammed
// @github https://github.com/babamohammed2022
// @include explorer.exe
// @compilerOptions -lgdi32 -lcomctl32 -luser32 -lole32 -lshlwapi -lshell32 -luxtheme -ldwmapi -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Taskbar and Start Menu Properties
This Windhawk mod attempts to recreate the classic "Taskbar and Start Menu Properties" dialog 
for Windows 10 and 11, with optional Windows 7/8.1 profile switching.

**IMPORTANT**: This mod is provided as-is with no guarantees of full functionality. 
It is a best-effort recreation that works within the limitations of modern Windows. 
Some features may work partially or not at all depending on your system configuration, 
Windows version, and installed modifications.

## What This Mod Does
- Lock/Unlock the taskbar
- Auto-hide the taskbar
- Use small icons/buttons
- Configure taskbar button grouping (Always combine, Combine when full, Never combine)
- Configure Aero Peek
- Windows 7/8.1 profile switching with Navigation tab (experimental)
- **NEW**: Functional Address/Links/Desktop/Tablet PC toolbars, shown as a separate bar docked next to the system tray (since the modern taskbar no longer supports embedded classic toolbars/deskbands)

## What This Mod Tries To Do
- Start Menu customization (where supported by the OS)
- Toolbar warning option

## What This Mod Does NOT Do
- It cannot guarantee 100% original behavior from older Windows versions
- It cannot change the taskbar position reliably due to the complexity of Windows' architecture
- It cannot embed toolbars directly inside the native Windows 10/11 taskbar surface (the modern taskbar has no rebar/deskband hosting mechanism); instead, a functional companion toolbar window is docked next to the system tray
- Navigation tab features are mostly cosmetic on Windows 10/11

## Windows 7 vs Windows 8.1 Mode
The mod can switch between two UI profiles:
- **Windows 7 Mode**: 3 tabs (Taskbar, Start Menu, Toolbars)
- **Windows 8.1 Mode**: 4 tabs (Taskbar, Navigation, Start Menu, Toolbars)

These profiles only change the appearance and available options — they do not magically restore missing Windows components.
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
- profile: win7
  $name: OS Profile
  $description: Select Windows version profile for UI appearance.
  $options:
    - win7: Windows 7
    - win81: Windows 8.1
- toolbarMode: auto
  $name: Gestione Toolbar (Toolbar Mode)
  $description: Scegli come gestire le toolbar. Su Win10 o Win11 con barra legacy (ExplorerPatcher), puoi usare quelle native di Windows.
  $options:
    - auto: Automatico (Usa native se disponibili, companion altrimenti)
    - companion: Forza Toolbar Ricreata (Companion)
    - native: Forza Toolbar Native (Windows 10 / ExplorerPatcher)
- showToolbarWarning: true
  $name: Show Toolbar Warning
  $description: Show warning message when enabling classic toolbars.
  $options:
    - true: Yes
    - false: No
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <strsafe.h>
#include <uxtheme.h>
#include <dwmapi.h>

// NOTE: INITGUID must be defined before including shlobj.h/knownfolders.h so
// that FOLDERID_Links / FOLDERID_Desktop get concrete definitions in this
// translation unit. Without this, linking fails with "undefined reference"
// on MinGW toolchains that don't automatically link uuid.lib.
#define INITGUID
#include <shlobj.h>
#include <knownfolders.h>
#undef INITGUID
#include <objbase.h>

#include <vector>
#include <string>

#ifndef SIID_TASKBAR
#define SIID_TASKBAR 39
#define IDC_PNL_BOTTOM 9001
#endif

namespace StuckRects {
    constexpr DWORD SETTINGS_EDGE_OFFSET = 12;
    constexpr DWORD MIN_SETTINGS_SIZE = 16;
}

namespace DialogSizes {
    constexpr short MAIN_WIDTH = 262;
    constexpr short MAIN_HEIGHT = 290;
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

static HANDLE g_hActCtx = INVALID_HANDLE_VALUE;

static void EnsureThemeActCtx() {
    if (g_hActCtx != INVALID_HANDLE_VALUE) {
        return;
    }
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) {
        g_hActCtx = INVALID_HANDLE_VALUE;
        return;
    }
    ACTCTXW actCtx = {};
    actCtx.cbSize = sizeof(actCtx);
    actCtx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_HMODULE_VALID;
    actCtx.hModule = hShell32;
    actCtx.lpResourceName = MAKEINTRESOURCEW(124);
    HANDLE h = CreateActCtxW(&actCtx);
    if (h == INVALID_HANDLE_VALUE) {
        g_hActCtx = INVALID_HANDLE_VALUE;
        return;
    }
    g_hActCtx = h;
}

static BOOL CALLBACK ApplyExplorerThemeEnumProc(HWND hwnd, LPARAM lParam) {
    wchar_t cls[64] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (_wcsicmp(cls, L"SysTabControl32") == 0 ||
        _wcsicmp(cls, L"SysListView32") == 0) {
        SetWindowTheme(hwnd, L"Explorer", nullptr);
    }
    return TRUE;
}

static void ApplyExplorerThemeToChildren(HWND hwndParent) {
    EnumChildWindows(hwndParent, ApplyExplorerThemeEnumProc, 0);
}

static void ApplyDarkTitlebar(HWND hwnd) {
    HKEY hk;
    BOOL useDark = FALSE;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        0, KEY_READ, &hk) == ERROR_SUCCESS) {
        DWORD val = 1, sz = sizeof(DWORD);
        if (RegQueryValueExW(hk, L"AppsUseLightTheme", NULL, NULL, (LPBYTE)&val, &sz) == ERROR_SUCCESS)
            useDark = (val == 0);
        RegCloseKey(hk);
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

static wchar_t g_childEnvBlock[32768] = {0};

static void BuildChildEnvironment() {
    LPWCH curEnv = GetEnvironmentStringsW();
    size_t pos = 0;
    if (curEnv) {
        LPWCH p = curEnv;
        while (*p && pos < sizeof(g_childEnvBlock)/sizeof(wchar_t) - 100) {
            size_t len = wcslen(p);
            if (wcsncmp(p, L"WH_CTP_NOREDIRECT=", 19) != 0) {
                wcscpy_s(g_childEnvBlock + pos, sizeof(g_childEnvBlock)/sizeof(wchar_t) - pos, p);
                pos += len;
                g_childEnvBlock[pos++] = L'\0';
            }
            p += len + 1;
        }
        FreeEnvironmentStringsW(curEnv);
    }
    wcscpy_s(g_childEnvBlock + pos, sizeof(g_childEnvBlock)/sizeof(wchar_t) - pos, L"WH_CTP_NOREDIRECT=1\0");
    g_childEnvBlock[pos + 19] = L'\0';
    g_childEnvBlock[pos + 20] = L'\0';
}

static bool IsChildProcess() {
    return GetEnvironmentVariableW(L"WH_CTP_NOREDIRECT", nullptr, 0) > 0;
}

static HWND g_hwndMain = NULL;
static HFONT g_hFontUi = NULL;
static LONG volatile g_dialogOpen = 0;
static int g_currentTab = 0;

static HWND g_hwndStartCustom = NULL;
static LONG volatile g_startCustomOpen = 0;
static HFONT g_hStartFontUi = NULL;
static HANDLE g_dialogThread = NULL;

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
static const WCHAR* kLinksBandGUID = L"{0E5CBF21-15B6-11D2-9F3E-00C04FA31A86}";
static const WCHAR* kDeskBandGUID = L"{D82BE2B0-5764-11D0-A96E-00C04FD705A2}";

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
#define IDC_GRP_CORNER_NAV 8001
#define IDC_CHK_CHARMS 8002
#define IDC_CHK_SWITCH_APPS 8003
#define IDC_CHK_POWERSHELL 8004
#define IDC_GRP_START_SCREEN 8005
#define IDC_CHK_DESKTOP_START 8006
#define IDC_CHK_BG_ON_START 8007
#define IDC_CHK_START_DISPLAY 8008
#define IDC_CHK_APPS_VIEW 8009
#define IDC_CHK_SEARCH_EVERY 8010
#define IDC_CHK_DESKTOP_FIRST 8011
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
    WCHAR tab_taskbar[64], tab_start[64], tab_toolbars[64], tab_navigation[64];
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
    WCHAR toolbars_warn_title[64], toolbars_warn_text[384];
    WCHAR grp_corner_nav[64];
    WCHAR chk_charms[256];
    WCHAR chk_switch_apps[256];
    WCHAR chk_powershell[256];
    WCHAR grp_start_screen[64];
    WCHAR chk_desktop_start[256];
    WCHAR chk_bg_on_start[256];
    WCHAR chk_start_display[256];
    WCHAR chk_apps_view[256];
    WCHAR chk_search_every[256];
    WCHAR chk_desktop_first[256];
};

static Strings g_str;
static WCHAR g_language[8] = L"auto";
static WCHAR g_profile[8] = L"win7";
static WCHAR g_toolbarMode[16] = L"auto"; // Nuovo setting aggiunto
static bool g_showToolbarWarning = true;
static bool g_hideNonFunctional = false;

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

static bool IsWin81Profile() {
    return (wcscmp(g_profile, L"win81") == 0);
}

static void UpdateSearchEverywhereState(HWND hwnd);
static void ShowTaskbarProperties();
static void ShowStartCustomDialog(HWND parent);
static INT_PTR CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
static INT_PTR CALLBACK StartCustomDlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
static DWORD WINAPI DialogThreadProc(LPVOID);

class TaskbarSettingsProvider {
public:
    static DWORD RegGetDWordSafe(HKEY hRoot, LPCWSTR sub, LPCWSTR name, DWORD def) {
        HKEY hk; DWORD v = def, sz = sizeof(DWORD);
        if (RegOpenKeyExW(hRoot, sub, 0, KEY_READ, &hk) == ERROR_SUCCESS) {
            RegQueryValueExW(hk, name, NULL, NULL, (LPBYTE)&v, &sz);
            RegCloseKey(hk);
        }
        return v;
    }
    static bool RegSetDWordSafe(HKEY hRoot, LPCWSTR sub, LPCWSTR name, DWORD v) {
        HKEY hk;
        LSTATUS r = RegCreateKeyExW(hRoot, sub, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hk, NULL);
        if (r != ERROR_SUCCESS) return false;
        r = RegSetValueExW(hk, name, 0, REG_DWORD, (const BYTE*)&v, sizeof(DWORD));
        RegCloseKey(hk);
        return (r == ERROR_SUCCESS);
    }
    static bool GetLockState() { return RegGetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarSizeMove", 1) == 0; }
    static void SetLockState(bool lock) { RegSetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarSizeMove", lock ? 0 : 1); }
    static bool GetSmallIcons() { return RegGetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarSmallIcons", 0) != 0; }
    static void SetSmallIcons(bool s) { RegSetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarSmallIcons", s ? 1 : 0); }
    static DWORD GetGlomLevel() { return RegGetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarGlomLevel", 0); }
    static void SetGlomLevel(DWORD lvl) { RegSetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarGlomLevel", lvl); }
    static DWORD GetTaskbarEdge() {
        HKEY hk; DWORD result = 3;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, kStuckKey, 0, KEY_READ, &hk) == ERROR_SUCCESS) {
            DWORD sz = 0;
            RegQueryValueExW(hk, L"Settings", NULL, NULL, NULL, &sz);
            if (sz >= StuckRects::MIN_SETTINGS_SIZE) {
                BYTE* d = new BYTE[sz];
                if (RegQueryValueExW(hk, L"Settings", NULL, NULL, d, &sz) == ERROR_SUCCESS) {
                    DWORD edge = *reinterpret_cast<DWORD*>(&d[StuckRects::SETTINGS_EDGE_OFFSET]);
                    if (edge <= 3) result = edge;
                }
                delete[] d;
            }
            RegCloseKey(hk);
        }
        return result;
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
    static bool GetCharmsEnabled() {
        return RegGetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\ImmersiveShell\\EdgeUi", L"DisableCharms", 1) == 0;
    }
    static void SetCharmsEnabled(bool enabled) {
        RegSetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\ImmersiveShell\\EdgeUi", L"DisableCharms", enabled ? 0 : 1);
    }
    static bool GetCornerSwitchApps() {
        return RegGetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\ImmersiveShell\\EdgeUi", L"DisableTLcorner", 0) == 0;
    }
    static void SetCornerSwitchApps(bool enabled) {
        RegSetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\ImmersiveShell\\EdgeUi", L"DisableTLcorner", enabled ? 0 : 1);
    }
    static bool GetPowerShellWinX() {
        return RegGetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", L"DontUsePowerShellOnWinX", 0) == 0;
    }
    static void SetPowerShellWinX(bool enabled) {
        RegSetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", L"DontUsePowerShellOnWinX", enabled ? 0 : 1);
    }
    static bool GetDesktopFirst() {
        return RegGetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StartPage", L"OpenAtLogon", 0) != 0;
    }
    static void SetDesktopFirst(bool enabled) {
        RegSetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StartPage", L"OpenAtLogon", enabled ? 1 : 0);
    }
    static bool GetBackgroundOnStart() {
        return RegGetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent", L"MotionAccentId_v1.00", 1) != 0;
    }
    static void SetBackgroundOnStart(bool enabled) {
        RegSetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent", L"MotionAccentId_v1.00", enabled ? 1 : 0);
    }
    static bool GetStartOnCurrentDisplay() {
        return RegGetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StartPage", L"MonitorOverride", 0) != 0;
    }
    static void SetStartOnCurrentDisplay(bool enabled) {
        RegSetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StartPage", L"MonitorOverride", enabled ? 1 : 0);
    }
    static bool GetAppsViewAuto() {
        return RegGetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StartPage", L"MakeAllAppsDefault", 0) != 0;
    }
    static void SetAppsViewAuto(bool enabled) {
        RegSetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StartPage", L"MakeAllAppsDefault", enabled ? 1 : 0);
    }
    static bool GetSearchEverywhere() {
        return RegGetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StartPage", L"GlobalSearch", 1) != 0;
    }
    static void SetSearchEverywhere(bool enabled) {
        RegSetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StartPage", L"GlobalSearch", enabled ? 1 : 0);
    }
    static bool GetDesktopAppsFirst() {
        return RegGetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StartPage", L"DesktopFirst", 0) != 0;
    }
    static void SetDesktopAppsFirst(bool enabled) {
        RegSetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StartPage", L"DesktopFirst", enabled ? 1 : 0);
    }
    static bool GetToolbarEnabled(LPCWSTR name) {
        if (wcscmp(name, L"TabletPC") == 0)
            return RegGetDWordSafe(HKEY_CURRENT_USER, L"Software\\Microsoft\\TabletTip\\1.7", L"TipbandDesiredVisibility", 0) != 0;
        const WCHAR* guid = nullptr;
        if (wcscmp(name, L"Address") == 0) guid = kAddrBandGUID;
        else if (wcscmp(name, L"Links") == 0) guid = kLinksBandGUID;
        else if (wcscmp(name, L"Desktop") == 0) guid = kDeskBandGUID;
        else return false;
        HKEY hk;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Internet Explorer\\Toolbar", 0, KEY_READ, &hk) == ERROR_SUCCESS) {
            DWORD sz = 0;
            LONG res = RegQueryValueExW(hk, guid, NULL, NULL, NULL, &sz);
            RegCloseKey(hk);
            return (res == ERROR_SUCCESS);
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
        HKEY hk;
        if (RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Internet Explorer\\Toolbar", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE | KEY_READ, NULL, &hk, NULL) == ERROR_SUCCESS) {
            if (enable) RegSetValueExW(hk, guid, 0, REG_SZ, (const BYTE*)L"", sizeof(WCHAR));
            else RegDeleteValueW(hk, guid);
            RegCloseKey(hk);
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
    SendMessageW(hwnd, WM_SETFONT, (WPARAM)lParam, TRUE);
    return TRUE;
}

static void SetFontAllChildren(HWND hwnd, HFONT hf) {
    EnumChildWindows(hwnd, SetFontChildProc, (LPARAM)hf);
}

// =====================================================================
// Funzioni di utilità per rilevamento Toolbar Native vs Companion
// =====================================================================

// Rileva se è presente la barra delle applicazioni legacy (Win10 o Win11 con ExplorerPatcher/RetroBar)
// Verificando l'esistenza del controllo "ReBarWindow32" all'interno di "Shell_TrayWnd"
static bool IsLegacyTaskbarPresent() {
    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hTray) {
        HWND hRebar = FindWindowExW(hTray, NULL, L"ReBarWindow32", NULL);
        if (hRebar) {
            return true; 
        }
    }
    return false;
}

// Determina se dobbiamo mostrare la barra ricreata (companion)
static bool ShouldUseCompanionToolbars() {
    if (wcscmp(g_toolbarMode, L"companion") == 0) {
        return true;
    }
    if (wcscmp(g_toolbarMode, L"native") == 0) {
        return false;
    }
    // "auto" mode: Usa le toolbar native se la barra legacy/Win10 è presente, altrimenti usa la companion
    return !IsLegacyTaskbarPresent();
}
// =====================================================================
// TOOLBAR NATIVE tramite ITrayDeskBand (l'API che usa Explorer stesso)
// Funziona su Win10 e Win11 con taskbar legacy (ExplorerPatcher).
// =====================================================================

static const CLSID CLSID_CTP_TrayDeskBand =
    {0xE6442437,0x6C68,0x4F52,{0x94,0xDD,0x2C,0xFE,0xD2,0x67,0xEF,0xB9}};
static const IID IID_CTP_ITrayDeskBand =
    {0x6D67E846,0x5B9C,0x4DB8,{0x9C,0xBC,0xDD,0xE1,0x2F,0x42,0x54,0xF1}};
static const CLSID CLSID_CTP_AddressBand =
    {0x01E04581,0x4EEE,0x11D0,{0xBF,0xE9,0x00,0xAA,0x00,0x5B,0x43,0x83}};
static const CLSID CLSID_CTP_LinksBand =
    {0x0E5CBF21,0x15B6,0x11D2,{0x9F,0x3E,0x00,0xC0,0x4F,0xA3,0x1A,0x86}};
static const CLSID CLSID_CTP_DesktopBand =
    {0xD82BE2B0,0x5764,0x11D0,{0xA9,0x6E,0x00,0xC0,0x4F,0xD7,0x05,0xA2}};

// Definita a mano per evitare conflitti con SDK diversi
struct CTP_ITrayDeskBand : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE ShowDeskBand(REFCLSID clsid) = 0;
    virtual HRESULT STDMETHODCALLTYPE HideDeskBand(REFCLSID clsid) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsDeskBandShown(REFCLSID clsid) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeskBandRegistrationChanged() = 0;
};

enum CtpBandOp { CTP_BAND_HIDE = 0, CTP_BAND_SHOW = 1, CTP_BAND_QUERY = 2 };

static bool NativeDeskBandOp(const CLSID& band, CtpBandOp op, bool* pShown) {
    HRESULT hrInit = OleInitialize(NULL);
    bool needUninit = SUCCEEDED(hrInit); // RPC_E_CHANGED_MODE = COM già attivo, ok comunque
    bool ok = false;
    CTP_ITrayDeskBand* pTray = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_CTP_TrayDeskBand, NULL,
        CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
        IID_CTP_ITrayDeskBand, (void**)&pTray);
    if (SUCCEEDED(hr) && pTray) {
        switch (op) {
        case CTP_BAND_SHOW:
            ok = SUCCEEDED(pTray->ShowDeskBand(band));
            break;
        case CTP_BAND_HIDE:
            ok = SUCCEEDED(pTray->HideDeskBand(band));
            break;
        case CTP_BAND_QUERY: {
            HRESULT q = pTray->IsDeskBandShown(band);
            if (pShown) *pShown = (q == S_OK);
            ok = SUCCEEDED(q);
            break;
        }
        }
        pTray->Release();
    }
    if (needUninit) OleUninitialize();
    return ok;
}
static void InitLocalization() {
    bool useItalian = false;
    if (wcscmp(g_language, L"it") == 0) {
        useItalian = true;
    } else if (wcscmp(g_language, L"en") == 0) {
        useItalian = false;
    } else {
        useItalian = ((GetUserDefaultUILanguage() & 0xFF) == 0x10);
    }
    StringCchCopyW(g_str.title, 128, useItalian ? L"Propriet\u00e0 della barra delle applicazioni e del menu Start" : L"Taskbar and Start Menu Properties");
    StringCchCopyW(g_str.tab_taskbar, 64, useItalian ? L"Barra delle applicazioni" : L"Taskbar");
    StringCchCopyW(g_str.tab_start, 64, useItalian ? L"Menu Start" : L"Start Menu");
    StringCchCopyW(g_str.tab_toolbars, 64, useItalian ? L"Barre degli strumenti" : L"Toolbars");
    StringCchCopyW(g_str.tab_navigation, 64, useItalian ? L"Navigazione" : L"Navigation");
    StringCchCopyW(g_str.btn_ok, 32, L"OK");
    StringCchCopyW(g_str.btn_cancel, 32, useItalian ? L"Annulla" : L"Cancel");
    StringCchCopyW(g_str.btn_apply, 32, useItalian ? L"Applica" : L"Apply");
    StringCchCopyW(g_str.grp_appearance, 64, useItalian ? L"Aspetto della barra delle applicazioni" : L"Taskbar appearance");
    StringCchCopyW(g_str.chk_lock, 64, useItalian ? L"Blocca la barra delle applicazioni" : L"Lock the taskbar");
    StringCchCopyW(g_str.chk_hide, 64, useItalian ? L"Nascondi automaticamente la barra delle applicazioni" : L"Auto-hide the taskbar");
    if (IsWin81Profile()) {
        StringCchCopyW(g_str.chk_small, 64, useItalian ? L"Usa pulsanti della barra delle applicazioni piccoli" : L"Use small taskbar buttons");
    } else {
        StringCchCopyW(g_str.chk_small, 64, useItalian ? L"Usa icone piccole" : L"Use small icons");
    }
    StringCchCopyW(g_str.txt_location, 64, useItalian ? L"Posizione sullo schermo:" : L"Taskbar location:");
    StringCchCopyW(g_str.txt_buttons, 64, useItalian ? L"Pulsanti della barra\ndelle applicazioni:" : L"Taskbar buttons:");
    StringCchCopyW(g_str.grp_notif, 64, useItalian ? L"Area di notifica" : L"Notification area");
    StringCchCopyW(g_str.txt_notif, 128, useItalian ? L"Consente di personalizzare le icone e le notifiche nell'area di notifica." : L"Customize which icons and notifications appear.");
    StringCchCopyW(g_str.btn_cust_notif, 32, useItalian ? L"Personalizza..." : L"Customize...");
    StringCchCopyW(g_str.grp_aero, 64, useItalian ? L"Anteprima del desktop con Aero Peek" : L"Preview desktop with Aero Peek");
    if (IsWin81Profile()) {
        StringCchCopyW(g_str.chk_aero, 64, useItalian ? L"Usa Peek per l'anteprima del desktop su ''Mostra desktop''" : L"Use Peek to view the desktop when hovering ''Show Desktop''.");
        StringCchCopyW(g_str.txt_aero, 256, useItalian ? L"Consente di visualizzare temporaneamente il desktop portando il puntatore sul pulsante Mostra desktop alla fine della barra delle applicazioni." : L"Temporarily view the desktop when you move your mouse to the Show desktop button.");
    } else {
        StringCchCopyW(g_str.chk_aero, 64, useItalian ? L"Usa Aero Peek per visualizzare l'anteprima del desktop" : L"Use Aero Peek to preview the desktop.");
        StringCchCopyW(g_str.txt_aero, 256, useItalian ? L"Consente di visualizzare temporaneamente il desktop portando il puntatore sul pulsante Mostra desktop alla fine della barra delle applicazioni." : L"Temporarily view the desktop when you move your mouse to the Show desktop button.");
    }
    StringCchCopyW(g_str.link_help, 128, useItalian ? L"<a>Personalizzazione della barra delle applicazioni</a>" : L"<a>How do I customize the taskbar?</a>");
    StringCchCopyW(g_str.pos_left, 32, useItalian ? L"A sinistra" : L"Left");
    StringCchCopyW(g_str.pos_top, 32, useItalian ? L"In alto" : L"Top");
    StringCchCopyW(g_str.pos_right, 32, useItalian ? L"A destra" : L"Right");
    StringCchCopyW(g_str.pos_bottom, 32, useItalian ? L"In basso" : L"Bottom");
    StringCchCopyW(g_str.btn_always_combine, 64, useItalian ? L"Combina sempre, nascondi etichette" : L"Always combine, hide labels");
    StringCchCopyW(g_str.btn_combine_full, 64, useItalian ? L"Combina se la barra \u00e8 piena" : L"Combine when taskbar is full");
    StringCchCopyW(g_str.btn_never_combine, 64, useItalian ? L"Mai combinare" : L"Never combine");
    StringCchCopyW(g_str.start_info, 256, useItalian ? L"Per personalizzare l'aspetto dei collegamenti, delle icone e dei menu nel menu Start, fare clic su Personalizza." : L"To customize how links, icons, and menus look in the Start menu, click Customize.");
    StringCchCopyW(g_str.btn_start_cust, 32, useItalian ? L"Personalizza..." : L"Customize...");
    StringCchCopyW(g_str.txt_power_label, 64, useItalian ? L"Azione pulsante di alimentazione:" : L"Power button action:");
    StringCchCopyW(g_str.power_shutdown, 32, useItalian ? L"Arresta il sistema" : L"Shut down");
    StringCchCopyW(g_str.power_restart, 32, useItalian ? L"Riavvia" : L"Restart");
    StringCchCopyW(g_str.power_sleep, 32, useItalian ? L"Sospensione" : L"Sleep");
    StringCchCopyW(g_str.power_hibernate, 32, useItalian ? L"Ibernazione" : L"Hibernate");
    StringCchCopyW(g_str.power_logoff, 32, useItalian ? L"Disconnetti" : L"Log off");
    StringCchCopyW(g_str.power_lock, 32, useItalian ? L"Blocca" : L"Lock");
    StringCchCopyW(g_str.power_switchuser, 32, useItalian ? L"Cambia utente" : L"Switch user");
    StringCchCopyW(g_str.grp_privacy, 32, useItalian ? L"Privacy" : L"Privacy");
    StringCchCopyW(g_str.chk_mru_prog, 128, useItalian ? L"Archivia e visualizza i programmi aperti di recente nel menu Start" : L"Store and display recently opened programs in the Start menu");
    StringCchCopyW(g_str.chk_mru_items, 128, useItalian ? L"Archivia e visualizza gli elementi aperti di recente nel menu Start e nella barra delle applicazioni" : L"Store and display recently opened items in the Start menu and the taskbar");
    StringCchCopyW(g_str.toolbars_info, 128, useItalian ? L"Selezionare le barre degli strumenti da aggiungere alla barra delle applicazioni." : L"Select which toolbars to add to the taskbar.");
    StringCchCopyW(g_str.toolbar_address, 32, useItalian ? L"Indirizzo" : L"Address");
    StringCchCopyW(g_str.toolbar_links, 32, useItalian ? L"Collegamenti" : L"Links");
    StringCchCopyW(g_str.toolbar_tabletpc, 32, useItalian ? L"Pannello input Tablet PC" : L"Tablet PC Input Panel");
    StringCchCopyW(g_str.toolbar_desktop, 32, useItalian ? L"Desktop" : L"Desktop");
    StringCchCopyW(g_str.grp_corner_nav, 64, useItalian ? L"Esplorazione degli angoli" : L"Corner navigation");
    StringCchCopyW(g_str.chk_charms, 256, useItalian ? L"Quando si punta nell'angolo superiore destro, mostra le icone della barra dei Charm" : L"When I point to the upper-right corner, show the charms");
    StringCchCopyW(g_str.chk_switch_apps, 256, useItalian ? L"Quando si fa clic nell'angolo superiore sinistro, passa da un'app recente all'altra" : L"When I click the upper-left corner, switch between my recent apps");
    StringCchCopyW(g_str.chk_powershell, 256, useItalian ? L"Sostituisci il Prompt dei comandi con Windows PowerShell nel menu quando si fa clic con il pulsante destro del mouse nell'angolo inferiore sinistro o si preme il tasto Windows + X" : L"Replace Command Prompt with Windows PowerShell in the menu when I right-click the lower-left corner or press Windows key +X");
    StringCchCopyW(g_str.grp_start_screen, 64, useItalian ? L"Schermata Start" : L"Start screen");
    StringCchCopyW(g_str.chk_desktop_start, 256, useItalian ? L"Mostra il desktop invece della schermata Start all'accesso o alla chiusura di tutte le app in una schermata" : L"When I sign in or close all apps on a screen, go to the desktop instead of Start");
    StringCchCopyW(g_str.chk_bg_on_start, 256, useItalian ? L"Mostra lo sfondo del desktop nella schermata Start" : L"Show my desktop background on Start");
    StringCchCopyW(g_str.chk_start_display, 256, useItalian ? L"Mostra Start sullo schermo in uso quando si preme il tasto LOGO WINDOWS" : L"Show Start on the display I'm using when I press the Windows logo key");
    StringCchCopyW(g_str.chk_apps_view, 256, useItalian ? L"Mostra automaticamente la visualizzazione App quando si accede a Start" : L"Show the Apps view automatically when I go to Start");
    StringCchCopyW(g_str.chk_search_every, 256, useItalian ? L"Cerca ovunque invece che solo nelle app nella visualizzazione App" : L"Search everywhere instead of just my apps when I search from the Apps view");
    StringCchCopyW(g_str.chk_desktop_first, 256, useItalian ? L"Elenca prima le app desktop nella visualizzazione App quando \u00e8 ordinata per categoria" : L"List desktop apps first in the Apps view when it's sorted by category");

    if (useItalian) {
        StringCchCopyW(g_str.about_title, 64, L"Informazioni sulla mod");
        StringCchCopyW(g_str.about_text, 4096,
            L"Classic Taskbar Properties\r\n\r\n"
            L"Questa mod per Windhawk ripristina la classica finestra \"Propriet\u00e0 della barra delle applicazioni e del menu Start\" ispirata alle versioni classiche di Windows.\r\n\r\n"
            L"Funzionalit\u00e0 attualmente disponibili:\r\n"
            L"- Blocca la barra delle applicazioni\r\n"
            L"- Nascondi automaticamente la barra delle applicazioni\r\n"
            L"- Usa icone piccole\r\n"
            L"- Configura la combinazione dei pulsanti della barra delle applicazioni\r\n"
            L"- Configura Aero Peek\r\n"
            L"- Accesso rapido alle impostazioni dell'area di notifica\r\n"
            L"- Profilo Windows 7/8.1 con tab Navigazione\r\n"
            L"- Opzione per disabilitare le opzioni non funzionanti\r\n"
            L"- Barre degli strumenti native o ricreate (Indirizzo, Collegamenti, Desktop, Tablet PC)\r\n\r\n"
            L"Limitazioni note:\r\n"
            L"- La modifica della posizione della barra delle applicazioni non \u00e8 supportata direttamente.\r\n"
            L"- Alcune impostazioni richiedono il riavvio di Explorer.");
        StringCchCopyW(g_str.warn_position_title, 64, L"Posizione barra delle applicazioni");
        StringCchCopyW(g_str.warn_position_text, 256, L"La modifica della posizione verr\u00e0 applicata al prossimo riavvio di Explorer.");
        StringCchCopyW(g_str.start_custom_title, 64, L"Personalizza menu Start");
        StringCchCopyW(g_str.start_grp_tiles, 64, L"Riquadri e comportamento");
        StringCchCopyW(g_str.start_chk_more_tiles, 64, L"Mostra pi\u00f9 riquadri nel menu Start");
    } else {
        StringCchCopyW(g_str.about_title, 64, L"About this mod");
        StringCchCopyW(g_str.about_text, 4096,
            L"Classic Taskbar Properties\r\n\r\n"
            L"This Windhawk mod restores the classic \"Taskbar and Start Menu Properties\" dialog inspired by classic Windows versions.\r\n\r\n"
            L"Currently available features:\r\n"
            L"- Lock the taskbar\r\n"
            L"- Auto-hide the taskbar\r\n"
            L"- Use small icons\r\n"
            L"- Configure taskbar button grouping\r\n"
            L"- Configure Aero Peek\r\n"
            L"- Quick access to notification area settings\r\n"
            L"- Windows 7/8.1 profile with Navigation tab\r\n"
            L"- Option to disable non-functional options\r\n"
            L"- Native or recreated toolbars (Address, Links, Desktop, Tablet PC)\r\n\r\n"
            L"Known limitations:\r\n"
            L"- Taskbar position change is unavailable natively.\r\n"
            L"- Some settings require Explorer restart.");
        StringCchCopyW(g_str.warn_position_title, 64, L"Taskbar position");
        StringCchCopyW(g_str.warn_position_text, 256, L"The taskbar position change will be applied after restarting Explorer.");
        StringCchCopyW(g_str.start_custom_title, 64, L"Customize Start Menu");
        StringCchCopyW(g_str.start_grp_tiles, 64, L"Tiles and behavior");
        StringCchCopyW(g_str.start_chk_more_tiles, 64, L"Show more tiles on Start");
    }
    
    // Inizializza i restanti testi del menu start
    StringCchCopyW(g_str.start_chk_app_list, 64, useItalian ? L"Mostra elenco app nel menu Start" : L"Show app list in Start menu");
    StringCchCopyW(g_str.start_chk_recent_apps, 64, useItalian ? L"Mostra app aggiunte di recente" : L"Show recently added apps");
    StringCchCopyW(g_str.start_chk_fullscreen, 64, useItalian ? L"Usa Start a schermo intero" : L"Use Start full screen");
    StringCchCopyW(g_str.start_chk_recent_items, 64, useItalian ? L"Mostra elementi recenti nelle Jump List" : L"Show recently opened items in Jump Lists");
    StringCchCopyW(g_str.start_chk_account_notif, 64, useItalian ? L"Mostra notifiche account" : L"Show account notifications");
    StringCchCopyW(g_str.start_grp_search, 64, useItalian ? L"Ricerca" : L"Search");
    StringCchCopyW(g_str.start_chk_search_programs, 64, useItalian ? L"Includi programmi nei risultati di ricerca" : L"Include programs in search results");
    StringCchCopyW(g_str.start_chk_search_files, 64, useItalian ? L"Includi file nei risultati di ricerca" : L"Include files in search results");
    StringCchCopyW(g_str.start_grp_folders, 64, useItalian ? L"Cartelle da visualizzare in Start" : L"Folders to show on Start");
    StringCchCopyW(g_str.start_chk_folder_settings, 32, useItalian ? L"Impostazioni" : L"Settings");
    StringCchCopyW(g_str.start_chk_folder_docs, 32, useItalian ? L"Documenti" : L"Documents");
    StringCchCopyW(g_str.start_chk_folder_downloads, 32, useItalian ? L"Download" : L"Downloads");
    StringCchCopyW(g_str.start_chk_folder_music, 32, useItalian ? L"Musica" : L"Music");
    StringCchCopyW(g_str.start_chk_folder_pics, 32, useItalian ? L"Immagini" : L"Pictures");
    StringCchCopyW(g_str.start_chk_folder_videos, 32, useItalian ? L"Video" : L"Videos");
    StringCchCopyW(g_str.start_chk_folder_network, 32, useItalian ? L"Rete" : L"Network");
    StringCchCopyW(g_str.start_chk_folder_personal, 32, useItalian ? L"Cartella personale" : L"Personal folder");
    StringCchCopyW(g_str.start_info_restart, 256, useItalian ? L"Nota: alcune modifiche potrebbero richiedere il riavvio di Explorer per essere applicate." : L"Note: some changes may require an Explorer restart or logout to take full effect.");
    StringCchCopyW(g_str.start_msg_saved, 512, useItalian ? L"Impostazioni del menu Start salvate.\n\nAlcune modifiche potrebbero richiedere il logout o il riavvio di Explorer." : L"Start menu settings saved.\n\nSome changes may require logout or Explorer restart to take full effect.");
    StringCchCopyW(g_str.start_msg_saved_title, 64, useItalian ? L"Impostazioni salvate" : L"Settings saved");
    StringCchCopyW(g_str.toolbars_warn_title, 64, useItalian ? L"Barre degli strumenti" : L"Toolbars");
    StringCchCopyW(g_str.toolbars_warn_text, 384, useItalian ? 
        L"Nota: Se le toolbar native non sono supportate o sono state disattivate, gli strumenti verranno mostrati nella barra companion ricreata accanto all'orologio." :
        L"Note: If native toolbars are unsupported or disabled, toolbars will be shown in the recreated companion bar docked next to the tray.");
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

static void LoadProfileSetting() {
    LPCWSTR profile = Wh_GetStringSetting(L"profile");
    if (profile && wcslen(profile) > 0 && wcslen(profile) < 8) {
        StringCchCopyW(g_profile, 8, profile);
    } else {
        StringCchCopyW(g_profile, 8, L"win7");
    }
    Wh_FreeStringSetting(profile);
}

static void LoadToolbarModeSetting() {
    LPCWSTR mode = Wh_GetStringSetting(L"toolbarMode");
    if (mode && wcslen(mode) > 0 && wcslen(mode) < 16) {
        StringCchCopyW(g_toolbarMode, 16, mode);
    } else {
        StringCchCopyW(g_toolbarMode, 16, L"auto");
    }
    Wh_FreeStringSetting(mode);
}

static void LoadHideNonFunctionalSetting() {
    LPCWSTR hideNF = Wh_GetStringSetting(L"hideNonFunctional");
    if (hideNF && wcslen(hideNF) > 0) {
        g_hideNonFunctional = (wcscmp(hideNF, L"true") == 0);
    } else {
        g_hideNonFunctional = false;
    }
    Wh_FreeStringSetting(hideNF);
}

static void LoadToolbarWarningSetting() {
    LPCWSTR showWarning = Wh_GetStringSetting(L"showToolbarWarning");
    if (showWarning && wcslen(showWarning) > 0) {
        g_showToolbarWarning = (wcscmp(showWarning, L"true") == 0);
    } else {
        g_showToolbarWarning = true;
    }
    Wh_FreeStringSetting(showWarning);
}

// =====================================================================
// Real functional toolbars (Address / Links / Desktop / Tablet PC)
// Companion Window (usata solo quando native non attive/non supportate)
// =====================================================================
#define WM_RTB_REBUILD (WM_APP + 1)
#define IDC_RTB_ADDR_EDIT 9101
#define IDC_RTB_ADDR_GO 9102
#define IDC_RTB_DESK_BTN 9104
#define IDC_RTB_TABLET_BTN 9105
#define IDC_RTB_LINKS_BTN_BASE 9200
#define IDC_RTB_DESKMENU_BASE 9500

struct RealToolbarState {
    bool address = false;
    bool links = false;
    bool desktop = false;
    bool tablet = false;
};

static HWND g_hwndRealToolbarHost = NULL;
static HWND g_hwndAddrEdit = NULL;
static HWND g_hwndLinksToolbar = NULL;
static HFONT g_hRtbFont = NULL;
static HWINEVENTHOOK g_hTaskbarEventHook = NULL;
static HANDLE g_realToolbarThread = NULL;
static DWORD g_realToolbarThreadId = 0;
static RealToolbarState g_rtbState;
static std::vector<std::wstring> g_linksNames;
static std::vector<std::wstring> g_linksPaths;
static std::vector<std::wstring> g_desktopNames;
static std::vector<std::wstring> g_desktopPaths;
static std::vector<int> g_rtbSeparatorX;

#define RTB_STATE_NORMAL 0
#define RTB_STATE_HOT 1
#define RTB_STATE_PRESSED 2

static void PositionRealToolbarHost();
static void RebuildRealToolbarContents();
static void DestroyRealToolbarChildren();

static void LaunchShellPath(HWND owner, LPCWSTR path) {
    if (!path || !*path) return;
    if (ShellExecuteW_orig)
        ShellExecuteW_orig(owner, L"open", path, NULL, NULL, SW_SHOWNORMAL);
    else
        ShellExecuteW(owner, L"open", path, NULL, NULL, SW_SHOWNORMAL);
}

static void EnumerateFolderItems(REFKNOWNFOLDERID folderId,
    std::vector<std::wstring>& outNames,
    std::vector<std::wstring>& outPaths,
    int maxItems) {
    outNames.clear();
    outPaths.clear();
    PWSTR pszPath = NULL;
    if (FAILED(SHGetKnownFolderPath(folderId, 0, NULL, &pszPath)) || !pszPath)
        return;
    WCHAR search[MAX_PATH];
    StringCchPrintfW(search, MAX_PATH, L"%s\\*", pszPath);
    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(search, &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) continue;
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) continue;
            if (_wcsicmp(fd.cFileName, L"desktop.ini") == 0) continue;
            WCHAR fullPath[MAX_PATH];
            StringCchPrintfW(fullPath, MAX_PATH, L"%s\\%s", pszPath, fd.cFileName);
            WCHAR displayName[MAX_PATH];
            StringCchCopyW(displayName, MAX_PATH, fd.cFileName);
            WCHAR* ext = PathFindExtensionW(displayName);
            if (ext && (_wcsicmp(ext, L".lnk") == 0 || _wcsicmp(ext, L".url") == 0))
                *ext = L'\0';
            outNames.push_back(displayName);
            outPaths.push_back(fullPath);
            if ((int)outNames.size() >= maxItems) break;
        } while (FindNextFileW(hFind, &fd));
        FindClose(hFind);
    }
    CoTaskMemFree(pszPath);
}

static void FillVerticalGradient(HDC hdc, const RECT& rc, COLORREF top, COLORREF bottom) {
    int h = rc.bottom - rc.top;
    if (rc.right <= rc.left || h <= 0) return;
    int r1 = GetRValue(top), g1 = GetGValue(top), b1 = GetBValue(top);
    int r2 = GetRValue(bottom), g2 = GetGValue(bottom), b2 = GetBValue(bottom);
    for (int i = 0; i < h; i++) {
        int r = r1 + ((r2 - r1) * i) / h;
        int g = g1 + ((g2 - g1) * i) / h;
        int b = b1 + ((b2 - b1) * i) / h;
        RECT rcLine = { rc.left, rc.top + i, rc.right, rc.top + i + 1 };
        HBRUSH hBr = CreateSolidBrush(RGB(r, g, b));
        FillRect(hdc, &rcLine, hBr);
        DeleteObject(hBr);
    }
}

static void DrawWin7Button(HDC hdc, RECT rc, LPCWSTR text, HFONT hFont, int state) {
    COLORREF top, bottom, border;
    switch (state) {
    case RTB_STATE_PRESSED:
        top = RGB(178, 209, 244);
        bottom = RGB(137, 181, 229);
        border = RGB(65, 121, 178);
        break;
    case RTB_STATE_HOT:
        top = RGB(235, 246, 253);
        bottom = RGB(201, 228, 250);
        border = RGB(122, 180, 231);
        break;
    default:
        top = RGB(253, 253, 253);
        bottom = RGB(233, 234, 236);
        border = RGB(172, 172, 172);
        break;
    }
    FillVerticalGradient(hdc, rc, top, bottom);
    HPEN hPenBorder = CreatePen(PS_SOLID, 1, border);
    HPEN hPenOld = (HPEN)SelectObject(hdc, hPenBorder);
    HBRUSH hBrOld = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
    SelectObject(hdc, hBrOld);
    SelectObject(hdc, hPenOld);
    DeleteObject(hPenBorder);
    if (state != RTB_STATE_PRESSED) {
        HPEN hHi = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
        HPEN hHiOld = (HPEN)SelectObject(hdc, hHi);
        MoveToEx(hdc, rc.left + 1, rc.top + 1, NULL);
        LineTo(hdc, rc.right - 1, rc.top + 1);
        SelectObject(hdc, hHiOld);
        DeleteObject(hHi);
    }
    HFONT hFontOld = (HFONT)SelectObject(hdc, hFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    RECT rcText = rc;
    if (state == RTB_STATE_PRESSED) OffsetRect(&rcText, 1, 1);
    DrawTextW(hdc, text, -1, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, hFontOld);
}

static LRESULT CALLBACK Win7ButtonSubclassProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR uIdSubclass, DWORD_PTR) {
    switch (msg) {
    case WM_MOUSEMOVE: {
        LONG_PTR state = GetWindowLongPtrW(hwnd, GWLP_USERDATA);
        if (state != RTB_STATE_HOT && state != RTB_STATE_PRESSED) {
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, RTB_STATE_HOT);
            InvalidateRect(hwnd, NULL, FALSE);
        }
        TRACKMOUSEEVENT tme = { sizeof(tme) };
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = hwnd;
        TrackMouseEvent(&tme);
        break;
    }
    case WM_MOUSELEAVE:
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, RTB_STATE_NORMAL);
        InvalidateRect(hwnd, NULL, FALSE);
        break;
    case WM_LBUTTONDOWN:
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, RTB_STATE_PRESSED);
        InvalidateRect(hwnd, NULL, FALSE);
        break;
    case WM_LBUTTONUP:
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, RTB_STATE_HOT);
        InvalidateRect(hwnd, NULL, FALSE);
        break;
    case WM_NCDESTROY:
        RemoveWindowSubclass(hwnd, Win7ButtonSubclassProc, uIdSubclass);
        break;
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

static void MakeWin7OwnerDrawButton(HWND hBtn) {
    if (!hBtn) return;
    LONG_PTR style = GetWindowLongPtrW(hBtn, GWL_STYLE);
    SetWindowLongPtrW(hBtn, GWL_STYLE, style | BS_OWNERDRAW);
    SetWindowLongPtrW(hBtn, GWLP_USERDATA, RTB_STATE_NORMAL);
    SetWindowSubclass(hBtn, Win7ButtonSubclassProc, 1, 0);
}

static HWND CreateFlatToolbarButtons(HWND parent, const std::vector<std::wstring>& names, int idBase, int height) {
    HWND hTb = CreateWindowExW(0, TOOLBARCLASSNAMEW, NULL,
        WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_LIST |
        TBSTYLE_TRANSPARENT | CCS_NODIVIDER | CCS_NORESIZE | CCS_NOPARENTALIGN,
        0, 0, 0, height, parent, NULL, GetModuleHandleW(NULL), NULL);
    if (!hTb) return NULL;
    SendMessageW(hTb, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    SendMessageW(hTb, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_MIXEDBUTTONS);
    SetWindowTheme(hTb, L"Explorer", NULL);
    std::vector<TBBUTTON> btns;
    for (size_t i = 0; i < names.size(); i++) {
        int strIdx = (int)SendMessageW(hTb, TB_ADDSTRINGW, 0, (LPARAM)names[i].c_str());
        TBBUTTON b = {};
        b.iBitmap = I_IMAGENONE;
        b.idCommand = idBase + (int)i;
        b.fsState = TBSTATE_ENABLED;
        b.fsStyle = BTNS_AUTOSIZE | BTNS_SHOWTEXT;
        b.iString = strIdx;
        btns.push_back(b);
    }
    if (!btns.empty())
        SendMessageW(hTb, TB_ADDBUTTONSW, btns.size(), (LPARAM)btns.data());
    SendMessageW(hTb, TB_AUTOSIZE, 0, 0);
    return hTb;
}

static LRESULT CALLBACK AddrEditSubclassProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR, DWORD_PTR) {
    if (msg == WM_KEYDOWN && wp == VK_RETURN) {
        SendMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(IDC_RTB_ADDR_GO, BN_CLICKED), 0);
        return 0;
    }
    if (msg == WM_NCDESTROY) {
        RemoveWindowSubclass(hwnd, AddrEditSubclassProc, 1);
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

static void DestroyRealToolbarChildren() {
    if (!g_hwndRealToolbarHost) return;
    HWND h;
    while ((h = GetWindow(g_hwndRealToolbarHost, GW_CHILD)) != NULL)
        DestroyWindow(h);
    g_hwndAddrEdit = NULL;
    g_hwndLinksToolbar = NULL;
}

static void PositionRealToolbarHost() {
    if (!g_hwndRealToolbarHost) return;
    if (!g_rtbState.address && !g_rtbState.links && !g_rtbState.desktop && !g_rtbState.tablet) {
        ShowWindow(g_hwndRealToolbarHost, SW_HIDE);
        return;
    }
    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!hTray || !IsWindow(hTray) || !IsWindowVisible(hTray)) {
        ShowWindow(g_hwndRealToolbarHost, SW_HIDE);
        return;
    }
    RECT rcTray; GetWindowRect(hTray, &rcTray);
    HWND hNotify = FindWindowExW(hTray, NULL, L"TrayNotifyWnd", NULL);
    RECT rcNotify = {0};
    bool haveNotify = hNotify && GetWindowRect(hNotify, &rcNotify);
    RECT rcHost; GetWindowRect(g_hwndRealToolbarHost, &rcHost);
    int hostW = rcHost.right - rcHost.left;
    int hostW_H = rcHost.bottom - rcHost.top;
    bool vertical = (rcTray.bottom - rcTray.top) > (rcTray.right - rcTray.left);
    int x, y;
    if (!vertical) {
        int barH = rcTray.bottom - rcTray.top;
        int refLeft = haveNotify ? rcNotify.left : (rcTray.right - 160);
        x = refLeft - hostW - 4;
        if (x < rcTray.left + 2) x = rcTray.left + 2;
        y = rcTray.top + (barH - hostW_H) / 2;
    } else {
        int barW = rcTray.right - rcTray.left;
        int refTop = haveNotify ? rcNotify.top : (rcTray.bottom - 160);
        x = rcTray.left + (barW - hostW) / 2;
        y = refTop - hostW_H - 4;
        if (y < rcTray.top + 2) y = rcTray.top + 2;
    }
    SetWindowPos(g_hwndRealToolbarHost, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

static void RebuildRealToolbarContents() {
    if (!g_hwndRealToolbarHost) return;

    // Se l'utente ha configurato l'uso delle toolbar native (o la barra legacy le supporta),
    // nascondiamo istantaneamente la companion window ricreata.
    if (!ShouldUseCompanionToolbars()) {
        ShowWindow(g_hwndRealToolbarHost, SW_HIDE);
        return;
    }

    g_rtbState.address = TaskbarSettingsProvider::GetToolbarEnabled(L"Address");
    g_rtbState.links = TaskbarSettingsProvider::GetToolbarEnabled(L"Links");
    g_rtbState.desktop = TaskbarSettingsProvider::GetToolbarEnabled(L"Desktop");
    g_rtbState.tablet = TaskbarSettingsProvider::GetToolbarEnabled(L"TabletPC");
    DestroyRealToolbarChildren();
    if (!g_rtbState.address && !g_rtbState.links && !g_rtbState.desktop && !g_rtbState.tablet) {
        ShowWindow(g_hwndRealToolbarHost, SW_HIDE);
        return;
    }
    HDC hdc = GetDC(g_hwndRealToolbarHost);
    int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(g_hwndRealToolbarHost, hdc);
    int barHeight = MulDiv(26, dpiY, 96);
    if (g_hRtbFont) { DeleteObject(g_hRtbFont); g_hRtbFont = NULL; }
    g_hRtbFont = CreateFontW(-MulDiv(9, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    g_rtbSeparatorX.clear();
    bool havePrevGroup = false;
    int curX = 5;
    if (g_rtbState.address) {
        HWND hLbl = CreateWindowExW(0, L"Static", L"Address:", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE,
            curX, 0, 46, barHeight, g_hwndRealToolbarHost, NULL, GetModuleHandleW(NULL), NULL);
        SendMessageW(hLbl, WM_SETFONT, (WPARAM)g_hRtbFont, TRUE);
        curX += 46 + 2;
        g_hwndAddrEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            curX, 3, 130, barHeight - 6, g_hwndRealToolbarHost, (HMENU)(INT_PTR)IDC_RTB_ADDR_EDIT, GetModuleHandleW(NULL), NULL);
        SendMessageW(g_hwndAddrEdit, WM_SETFONT, (WPARAM)g_hRtbFont, TRUE);
        SetWindowSubclass(g_hwndAddrEdit, AddrEditSubclassProc, 1, 0);
        curX += 130 + 3;
        HWND hGo = CreateWindowExW(0, L"Button", L"Vai", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            curX, 2, 32, barHeight - 4, g_hwndRealToolbarHost, (HMENU)(INT_PTR)IDC_RTB_ADDR_GO, GetModuleHandleW(NULL), NULL);
        SendMessageW(hGo, WM_SETFONT, (WPARAM)g_hRtbFont, TRUE);
        MakeWin7OwnerDrawButton(hGo);
        curX += 32 + 8;
        havePrevGroup = true;
    }
    if (g_rtbState.links) {
        if (havePrevGroup) { g_rtbSeparatorX.push_back(curX - 5); curX += 4; }
        EnumerateFolderItems(FOLDERID_Links, g_linksNames, g_linksPaths, 12);
        g_hwndLinksToolbar = CreateFlatToolbarButtons(g_hwndRealToolbarHost, g_linksNames, IDC_RTB_LINKS_BTN_BASE, barHeight);
        if (g_hwndLinksToolbar) {
            SendMessageW(g_hwndLinksToolbar, WM_SETFONT, (WPARAM)g_hRtbFont, TRUE);
            RECT rc; GetWindowRect(g_hwndLinksToolbar, &rc);
            int w = rc.right - rc.left;
            SetWindowPos(g_hwndLinksToolbar, NULL, curX, 0, w, barHeight, SWP_NOZORDER);
            curX += w + 8;
        }
        havePrevGroup = true;
    }
    if (g_rtbState.desktop) {
        if (havePrevGroup) { g_rtbSeparatorX.push_back(curX - 5); curX += 4; }
        HWND hDeskBtn = CreateWindowExW(0, L"Button", L"Desktop \u00bb", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            curX, 2, 74, barHeight - 4, g_hwndRealToolbarHost, (HMENU)(INT_PTR)IDC_RTB_DESK_BTN, GetModuleHandleW(NULL), NULL);
        SendMessageW(hDeskBtn, WM_SETFONT, (WPARAM)g_hRtbFont, TRUE);
        MakeWin7OwnerDrawButton(hDeskBtn);
        curX += 74 + 8;
        havePrevGroup = true;
    }
    if (g_rtbState.tablet) {
        if (havePrevGroup) { g_rtbSeparatorX.push_back(curX - 5); curX += 4; }
        HWND hTabBtn = CreateWindowExW(0, L"Button", L"Pannello input", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            curX, 2, 84, barHeight - 4, g_hwndRealToolbarHost, (HMENU)(INT_PTR)IDC_RTB_TABLET_BTN, GetModuleHandleW(NULL), NULL);
        SendMessageW(hTabBtn, WM_SETFONT, (WPARAM)g_hRtbFont, TRUE);
        MakeWin7OwnerDrawButton(hTabBtn);
        curX += 84 + 5;
    } else {
        curX += 1;
    }
    SetWindowPos(g_hwndRealToolbarHost, NULL, 0, 0, curX, barHeight, SWP_NOMOVE | SWP_NOZORDER);
    PositionRealToolbarHost();
    ShowWindow(g_hwndRealToolbarHost, SW_SHOWNOACTIVATE);
}

static LRESULT CALLBACK RealToolbarHostWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc; GetClientRect(hwnd, &rc);
        HDC hMem = CreateCompatibleDC(hdc);
        HBITMAP hBmp = CreateCompatibleBitmap(hdc, rc.right > 1 ? rc.right : 1, rc.bottom > 1 ? rc.bottom : 1);
        HBITMAP hOldBmp = (HBITMAP)SelectObject(hMem, hBmp);
        FillVerticalGradient(hMem, rc, RGB(248, 249, 250), RGB(212, 218, 225));
        HPEN hHi = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
        HPEN hPenOld = (HPEN)SelectObject(hMem, hHi);
        MoveToEx(hMem, 0, 0, NULL); LineTo(hMem, rc.right, 0);
        SelectObject(hMem, hPenOld);
        DeleteObject(hHi);
        HPEN hBorder = CreatePen(PS_SOLID, 1, RGB(137, 142, 148));
        hPenOld = (HPEN)SelectObject(hMem, hBorder);
        HBRUSH hBrOld = (HBRUSH)SelectObject(hMem, GetStockObject(NULL_BRUSH));
        Rectangle(hMem, rc.left, rc.top, rc.right, rc.bottom);
        SelectObject(hMem, hBrOld);
        SelectObject(hMem, hPenOld);
        DeleteObject(hBorder);
        for (int sepX : g_rtbSeparatorX) {
            if (sepX <= rc.left || sepX >= rc.right - 2) continue;
            HPEN hDark = CreatePen(PS_SOLID, 1, RGB(170, 176, 182));
            HPEN hLight = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
            HPEN pOld = (HPEN)SelectObject(hMem, hDark);
            MoveToEx(hMem, sepX, 4, NULL); LineTo(hMem, sepX, rc.bottom - 4);
            SelectObject(hMem, hLight);
            MoveToEx(hMem, sepX + 1, 4, NULL); LineTo(hMem, sepX + 1, rc.bottom - 4);
            SelectObject(hMem, pOld);
            DeleteObject(hDark);
            DeleteObject(hLight);
        }
        BitBlt(hdc, 0, 0, rc.right, rc.bottom, hMem, 0, 0, SRCCOPY);
        SelectObject(hMem, hOldBmp);
        DeleteObject(hBmp);
        DeleteDC(hMem);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lp;
        if (dis->CtlType == ODT_BUTTON) {
            WCHAR text[64];
            GetWindowTextW(dis->hwndItem, text, 64);
            int state = (int)GetWindowLongPtrW(dis->hwndItem, GWLP_USERDATA);
            DrawWin7Button(dis->hDC, dis->rcItem, text, g_hRtbFont, state);
            return TRUE;
        }
        break;
    }
    case WM_CTLCOLORSTATIC: {
        static HBRUSH hBkBrush = NULL;
        if (!hBkBrush) hBkBrush = CreateSolidBrush(RGB(233, 237, 241));
        HDC hdcCtl = (HDC)wp;
        SetBkMode(hdcCtl, TRANSPARENT);
        return (LRESULT)hBkBrush;
    }
    case WM_CTLCOLOREDIT: {
        HDC hdcCtl = (HDC)wp;
        SetBkColor(hdcCtl, RGB(255, 255, 255));
        return (LRESULT)GetStockObject(WHITE_BRUSH);
    }
    case WM_COMMAND: {
        WORD id = LOWORD(wp);
        if (id == IDC_RTB_ADDR_GO) {
            WCHAR buf[1024];
            GetWindowTextW(g_hwndAddrEdit, buf, 1024);
            if (buf[0]) {
                LaunchShellPath(hwnd, buf);
                SetWindowTextW(g_hwndAddrEdit, L"");
            }
        } else if (id == IDC_RTB_DESK_BTN) {
            EnumerateFolderItems(FOLDERID_Desktop, g_desktopNames, g_desktopPaths, 40);
            HMENU hMenu = CreatePopupMenu();
            for (size_t i = 0; i < g_desktopNames.size(); i++)
                AppendMenuW(hMenu, MF_STRING, (UINT)(IDC_RTB_DESKMENU_BASE + i), g_desktopNames[i].c_str());
            if (g_desktopNames.empty())
                AppendMenuW(hMenu, MF_STRING | MF_GRAYED, 0, L"(vuota)");
            RECT rcBtn; GetWindowRect(GetDlgItem(hwnd, IDC_RTB_DESK_BTN), &rcBtn);
            SetForegroundWindow(hwnd);
            TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_BOTTOMALIGN, rcBtn.left, rcBtn.top, 0, hwnd, NULL);
            DestroyMenu(hMenu);
        } else if (id == IDC_RTB_TABLET_BTN) {
            WCHAR path[MAX_PATH];
            ExpandEnvironmentStringsW(L"%CommonProgramFiles%\\microsoft shared\\ink\\TabTip.exe", path, MAX_PATH);
            LaunchShellPath(hwnd, path);
        } else if (id >= IDC_RTB_LINKS_BTN_BASE && id < IDC_RTB_LINKS_BTN_BASE + 100) {
            size_t idx = id - IDC_RTB_LINKS_BTN_BASE;
            if (idx < g_linksPaths.size()) LaunchShellPath(hwnd, g_linksPaths[idx].c_str());
        } else if (id >= IDC_RTB_DESKMENU_BASE && id < IDC_RTB_DESKMENU_BASE + 100) {
            size_t idx = id - IDC_RTB_DESKMENU_BASE;
            if (idx < g_desktopPaths.size()) LaunchShellPath(hwnd, g_desktopPaths[idx].c_str());
        }
        break;
    }
    case WM_RTB_REBUILD:
        RebuildRealToolbarContents();
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    case WM_TIMER:
        if (wp == 1) PositionRealToolbarHost();
        break;
    case WM_DESTROY:
        KillTimer(hwnd, 1);
        break;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static ATOM RegisterRealToolbarHostClass() {
    static ATOM atom = 0;
    if (atom) return atom;
    WNDCLASSEXW wc = { sizeof(wc) };
    wc.lpfnWndProc = RealToolbarHostWndProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = L"CTP_RealToolbarHost";
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    atom = RegisterClassExW(&wc);
    return atom;
}

static void CALLBACK RtbWinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || !hwnd || event != EVENT_OBJECT_LOCATIONCHANGE) return;
    WCHAR cls[64];
    GetClassNameW(hwnd, cls, 64);
    if (_wcsicmp(cls, L"Shell_TrayWnd") == 0 || _wcsicmp(cls, L"TrayNotifyWnd") == 0)
        PositionRealToolbarHost();
}

static DWORD WINAPI RealToolbarThreadProc(LPVOID) {
    OleInitialize(NULL);
    INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_BAR_CLASSES | ICC_STANDARD_CLASSES };
    InitCommonControlsEx(&icex);
    RegisterRealToolbarHostClass();
    g_hwndRealToolbarHost = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, L"CTP_RealToolbarHost", L"",
        WS_POPUP | WS_CLIPCHILDREN, 0, 0, 10, 10, NULL, NULL, GetModuleHandleW(NULL), NULL);
    if (g_hwndRealToolbarHost) {
        RebuildRealToolbarContents();
        SetTimer(g_hwndRealToolbarHost, 1, 500, NULL);
        g_hTaskbarEventHook = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
            NULL, RtbWinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    }
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (g_hTaskbarEventHook) { UnhookWinEvent(g_hTaskbarEventHook); g_hTaskbarEventHook = NULL; }
    if (g_hwndRealToolbarHost && IsWindow(g_hwndRealToolbarHost)) DestroyWindow(g_hwndRealToolbarHost);
    g_hwndRealToolbarHost = NULL;
    if (g_hRtbFont) { DeleteObject(g_hRtbFont); g_hRtbFont = NULL; }
    OleUninitialize();
    return 0;
}

// Funzione per ottenere il comando dal menu contestuale della taskbar
static UINT GetToolbarCommandFromContextMenu(LPCWSTR toolbarName) {
    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!hTray) return 0;
    
    IShellFolder* psfDesktop = NULL;
    if (FAILED(SHGetDesktopFolder(&psfDesktop))) return 0;
    
    LPITEMIDLIST pidl = NULL;
    if (FAILED(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl))) {
        psfDesktop->Release();
        return 0;
    }
    
    IContextMenu* pcm = NULL;
    if (SUCCEEDED(psfDesktop->GetUIObjectOf(hTray, 1, (LPCITEMIDLIST*)&pidl, 
                                            IID_IContextMenu, NULL, (void**)&pcm))) {
        HMENU hPopup = CreatePopupMenu();
        if (SUCCEEDED(pcm->QueryContextMenu(hPopup, 0, 1, 0x7FFF, CMF_NORMAL))) {
            int count = GetMenuItemCount(hPopup);
            for (int i = 0; i < count; i++) {
                WCHAR menuText[256];
                if (GetMenuStringW(hPopup, i, menuText, 256, MF_BYPOSITION)) {
                    if (StrStrIW(menuText, L"Toolbar") || StrStrIW(menuText, L"Barre")) {
                        HMENU hSubMenu = GetSubMenu(hPopup, i);
                        if (hSubMenu) {
                            int subCount = GetMenuItemCount(hSubMenu);
                            for (int j = 0; j < subCount; j++) {
                                WCHAR subText[256];
                                if (GetMenuStringW(hSubMenu, j, subText, 256, MF_BYPOSITION)) {
                                    if (StrStrIW(subText, toolbarName)) {
                                        UINT cmd = GetMenuItemID(hSubMenu, j);
                                        DestroyMenu(hPopup);
                                        pcm->Release();
                                        CoTaskMemFree(pidl);
                                        psfDesktop->Release();
                                        return cmd;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        DestroyMenu(hPopup);
        pcm->Release();
    }
    
    CoTaskMemFree(pidl);
    psfDesktop->Release();
    return 0;
}

static void ApplyToolbars(bool addr, bool links, bool tablet, bool desk) {
    bool useNative = !ShouldUseCompanionToolbars();

    if (useNative) {
        // INDIRIZZI: Mantieni la tua implementazione nativa
        NativeDeskBandOp(CLSID_CTP_AddressBand, addr ? CTP_BAND_SHOW : CTP_BAND_HIDE, NULL);
        
        // LINKS e DESKTOP: Usa il menu contestuale di Windows invece delle API native
        if (links) {
            UINT cmd = GetToolbarCommandFromContextMenu(L"Links");
            if (cmd) {
                HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
                if (hTray) PostMessageW(hTray, WM_COMMAND, cmd, 0);
            }
        }
        if (desk) {
            UINT cmd = GetToolbarCommandFromContextMenu(L"Desktop");
            if (cmd) {
                HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
                if (hTray) PostMessageW(hTray, WM_COMMAND, cmd, 0);
            }
        }
        
        // Tablet PC: gestito via registro
        if (tablet) {
            TaskbarSettingsProvider::SetToolbarEnabled(L"TabletPC", true);
        }
        
        // Nascondi la companion window
        if (g_hwndRealToolbarHost && IsWindow(g_hwndRealToolbarHost)) {
            ShowWindow(g_hwndRealToolbarHost, SW_HIDE);
        }
    } else {
        // Percorso COMPANION: Indirizzi usa la tua implementazione
        TaskbarSettingsProvider::SetToolbarEnabled(L"Address", addr);
        
        // LINKS e DESKTOP: In modalità companion, prova comunque il menu contestuale
        if (links) {
            UINT cmd = GetToolbarCommandFromContextMenu(L"Links");
            if (cmd) {
                HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
                if (hTray) PostMessageW(hTray, WM_COMMAND, cmd, 0);
            } else {
                TaskbarSettingsProvider::SetToolbarEnabled(L"Links", true);
            }
        }
        if (desk) {
            UINT cmd = GetToolbarCommandFromContextMenu(L"Desktop");
            if (cmd) {
                HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
                if (hTray) PostMessageW(hTray, WM_COMMAND, cmd, 0);
            } else {
                TaskbarSettingsProvider::SetToolbarEnabled(L"Desktop", true);
            }
        }
        
        TaskbarSettingsProvider::SetToolbarEnabled(L"TabletPC", tablet);
        
        // Aggiorna la companion window (per Indirizzi)
        if (g_hwndRealToolbarHost && IsWindow(g_hwndRealToolbarHost))
            PostMessageW(g_hwndRealToolbarHost, WM_RTB_REBUILD, 0, 0);
    }

    SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Taskbar");
    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hTray) SendNotifyMessageW(hTray, WM_SETTINGCHANGE, 0, (LPARAM)L"Taskbar");
}
static void InitToolbarsList(HWND hList) {
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
    const CLSID* clsids[] = { &CLSID_CTP_AddressBand, &CLSID_CTP_LinksBand, NULL, &CLSID_CTP_DesktopBand };

    bool useNative = !ShouldUseCompanionToolbars();

    for (int i = 0; i < 4; i++) {
        LVITEMW lvi = {};
        lvi.mask = LVIF_TEXT;
        lvi.iItem = i;
        lvi.pszText = (LPWSTR)names[i];
        ListView_InsertItem(hList, &lvi);

        bool checked = TaskbarSettingsProvider::GetToolbarEnabled(keys[i]);
        if (useNative && clsids[i]) {
            bool shown = false;
            if (NativeDeskBandOp(*clsids[i], CTP_BAND_QUERY, &shown))
                checked = shown; // stato reale dalla taskbar
        }
        ListView_SetCheckState(hList, i, checked ? TRUE : FALSE);
    }
}
static const int kTaskbarCtls[] = {
    IDC_GRP_APPEARANCE, IDC_CHK_LOCK, IDC_CHK_HIDE, IDC_CHK_SMALL,
    IDC_TXT_LOCATION, IDC_COMBO_LOCATION, IDC_TXT_BUTTONS, IDC_COMBO_BUTTONS,
    IDC_GRP_NOTIF, IDC_TXT_NOTIF, IDC_BTN_CUST_NOTIF,
    IDC_GRP_AERO, IDC_TXT_AERO, IDC_CHK_AEROPEEK, IDC_LINK_HELP, 0
};
static const int kNavigationCtls[] = {
    IDC_GRP_CORNER_NAV, IDC_CHK_CHARMS, IDC_CHK_SWITCH_APPS, IDC_CHK_POWERSHELL,
    IDC_GRP_START_SCREEN, IDC_CHK_DESKTOP_START, IDC_CHK_BG_ON_START,
    IDC_CHK_START_DISPLAY, IDC_CHK_APPS_VIEW, IDC_CHK_SEARCH_EVERY,
    IDC_CHK_DESKTOP_FIRST, 0
};
static const int kNonFunctionalNavCtls[] = {
    IDC_CHK_CHARMS, IDC_CHK_SWITCH_APPS,
    IDC_CHK_DESKTOP_START, IDC_CHK_BG_ON_START, IDC_CHK_START_DISPLAY,
    IDC_CHK_APPS_VIEW, IDC_CHK_SEARCH_EVERY, IDC_CHK_DESKTOP_FIRST, 0
};
static const int kStartCtls[] = {
    IDC_TXT_START_INFO, IDC_BTN_START_CUST,
    IDC_TXT_POWER_LABEL, IDC_COMBO_POWER,
    IDC_GRP_PRIVACY, IDC_CHK_MRU_PROG, IDC_CHK_MRU_ITEMS, 0
};
static const int kToolbarCtls[] = { IDC_TXT_TOOLBARS_INFO, IDC_LST_TOOLBARS, 0 };

static void ShowGroup(HWND hwnd, const int* ids, bool show) {
    int cmd = show ? SW_SHOW : SW_HIDE;
    for (int i = 0; ids[i]; i++) {
        HWND h = GetDlgItem(hwnd, ids[i]);
        if (h) ShowWindow(h, cmd);
    }
}

static void ApplyNonFunctionalControlsState(HWND hwnd) {
    if (!IsWin81Profile()) return;
    bool shouldDisable = g_hideNonFunctional;
    for (int i = 0; kNonFunctionalNavCtls[i]; i++) {
        HWND h = GetDlgItem(hwnd, kNonFunctionalNavCtls[i]);
        if (h) {
            EnableWindow(h, !shouldDisable);
            if (shouldDisable) {
                SendMessageW(h, BM_SETCHECK, BST_UNCHECKED, 0);
            }
        }
    }
}

static void SwitchTab(HWND hwnd, int tab) {
    g_currentTab = tab;
    if (IsWin81Profile()) {
        ShowGroup(hwnd, kTaskbarCtls, tab == 0);
        ShowGroup(hwnd, kNavigationCtls, tab == 1);
        ShowGroup(hwnd, kStartCtls, tab == 2);
        ShowGroup(hwnd, kToolbarCtls, tab == 3);
        ApplyNonFunctionalControlsState(hwnd);
    } else {
        ShowGroup(hwnd, kNavigationCtls, false);
        ShowGroup(hwnd, kTaskbarCtls, tab == 0);
        ShowGroup(hwnd, kStartCtls, tab == 1);
        ShowGroup(hwnd, kToolbarCtls, tab == 2);
    }
}

static void UpdateSearchEverywhereState(HWND hwnd) {
    if (!IsWin81Profile()) return;
    HWND hAppsView = GetDlgItem(hwnd, IDC_CHK_APPS_VIEW);
    HWND hSearchEvery = GetDlgItem(hwnd, IDC_CHK_SEARCH_EVERY);
    if (hAppsView && hSearchEvery) {
        bool appsViewChecked = (SendMessageW(hAppsView, BM_GETCHECK, 0, 0) == BST_CHECKED);
        bool shouldEnable = appsViewChecked && !g_hideNonFunctional;
        EnableWindow(hSearchEvery, shouldEnable);
    }
}

static void BalanceTextAndCombo(HWND hwndDlg, int idStatic, int idCombo) {
    HWND hStatic = GetDlgItem(hwndDlg, idStatic);
    HWND hCombo = GetDlgItem(hwndDlg, idCombo);
    if (!hStatic || !hCombo) return;
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

static int MeasureWrappedTextHeightPx(HWND hCtrl, LPCWSTR text) {
    HFONT hFont = (HFONT)SendMessageW(hCtrl, WM_GETFONT, 0, 0);
    if (!hFont) hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    RECT rc; GetClientRect(hCtrl, &rc);
    HDC hdc = GetDC(hCtrl);
    HFONT hOld = (HFONT)SelectObject(hdc, hFont);
    RECT rcCalc = { 0, 0, rc.right, 0 };
    DrawTextW(hdc, text, -1, &rcCalc, DT_WORDBREAK | DT_CALCRECT | DT_LEFT);
    SelectObject(hdc, hOld);
    ReleaseDC(hCtrl, hdc);
    return rcCalc.bottom - rcCalc.top;
}

static void AutoSizeMultilineControl(HWND hwndDlg, int controlId, int minCyPx, const int* belowControlIds) {
    HWND hCtrl = GetDlgItem(hwndDlg, controlId);
    if (!hCtrl) return;
    WCHAR text[512];
    GetWindowTextW(hCtrl, text, 512);
    int neededCyPx = MeasureWrappedTextHeightPx(hCtrl, text);
    if (neededCyPx < minCyPx) neededCyPx = minCyPx;
    RECT rcCtrl; GetWindowRect(hCtrl, &rcCtrl);
    MapWindowPoints(NULL, hwndDlg, (LPPOINT)&rcCtrl, 2);
    int curCyPx = rcCtrl.bottom - rcCtrl.top;
    int deltaPx = neededCyPx - curCyPx;
    if (deltaPx == 0) return;
    SetWindowPos(hCtrl, NULL, rcCtrl.left, rcCtrl.top, rcCtrl.right - rcCtrl.left, neededCyPx, SWP_NOZORDER | SWP_NOACTIVATE);
    if (belowControlIds) {
        for (int i = 0; belowControlIds[i]; i++) {
            HWND h = GetDlgItem(hwndDlg, belowControlIds[i]);
            if (!h) continue;
            RECT r; GetWindowRect(h, &r);
            MapWindowPoints(NULL, hwndDlg, (LPPOINT)&r, 2);
            SetWindowPos(h, NULL, r.left, r.top + deltaPx, r.right - r.left, r.bottom - r.top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }
}

static void ApplySettings(HWND hwnd) {
    bool lock = (SendDlgItemMessageW(hwnd, IDC_CHK_LOCK, BM_GETCHECK, 0, 0) == BST_CHECKED);
    bool hide = (SendDlgItemMessageW(hwnd, IDC_CHK_HIDE, BM_GETCHECK, 0, 0) == BST_CHECKED);
    bool small_i = (SendDlgItemMessageW(hwnd, IDC_CHK_SMALL, BM_GETCHECK, 0, 0) == BST_CHECKED);
    bool aero = (SendDlgItemMessageW(hwnd, IDC_CHK_AEROPEEK, BM_GETCHECK, 0, 0) == BST_CHECKED);
    DWORD glom = (DWORD)SendDlgItemMessageW(hwnd, IDC_COMBO_BUTTONS, CB_GETCURSEL, 0, 0);
    TaskbarSettingsProvider::SetLockState(lock);
    TaskbarSettingsProvider::SetSmallIcons(small_i);
    TaskbarSettingsProvider::SetGlomLevel(glom);
    TaskbarSettingsProvider::SetAeroPeekEnabled(aero);
    APPBARDATA abd = { sizeof(APPBARDATA) };
    abd.hWnd = FindWindowW(L"Shell_TrayWnd", NULL);
    abd.lParam = hide ? ABS_AUTOHIDE : ABS_ALWAYSONTOP;
    SHAppBarMessage(ABM_SETSTATE, &abd);
    DWORD powerSel = (DWORD)SendDlgItemMessageW(hwnd, IDC_COMBO_POWER, CB_GETCURSEL, 0, 0);
    if (powerSel < 7)
        TaskbarSettingsProvider::SetPowerAction(kPowerValues[powerSel]);
    bool mruProg = (SendDlgItemMessageW(hwnd, IDC_CHK_MRU_PROG, BM_GETCHECK, 0, 0) == BST_CHECKED);
    bool mruItems = (SendDlgItemMessageW(hwnd, IDC_CHK_MRU_ITEMS, BM_GETCHECK, 0, 0) == BST_CHECKED);
    TaskbarSettingsProvider::SetStartMruProgs(mruProg);
    TaskbarSettingsProvider::SetStartMruItems(mruItems);
    if (IsWin81Profile()) {
        bool powershell = (SendDlgItemMessageW(hwnd, IDC_CHK_POWERSHELL, BM_GETCHECK, 0, 0) == BST_CHECKED);
        TaskbarSettingsProvider::SetPowerShellWinX(powershell);
        if (!g_hideNonFunctional) {
            bool charms = (SendDlgItemMessageW(hwnd, IDC_CHK_CHARMS, BM_GETCHECK, 0, 0) == BST_CHECKED);
            bool switchApps = (SendDlgItemMessageW(hwnd, IDC_CHK_SWITCH_APPS, BM_GETCHECK, 0, 0) == BST_CHECKED);
            bool desktopStart = (SendDlgItemMessageW(hwnd, IDC_CHK_DESKTOP_START, BM_GETCHECK, 0, 0) == BST_CHECKED);
            bool bgOnStart = (SendDlgItemMessageW(hwnd, IDC_CHK_BG_ON_START, BM_GETCHECK, 0, 0) == BST_CHECKED);
            bool startDisplay = (SendDlgItemMessageW(hwnd, IDC_CHK_START_DISPLAY, BM_GETCHECK, 0, 0) == BST_CHECKED);
            bool appsView = (SendDlgItemMessageW(hwnd, IDC_CHK_APPS_VIEW, BM_GETCHECK, 0, 0) == BST_CHECKED);
            bool searchEvery = (SendDlgItemMessageW(hwnd, IDC_CHK_SEARCH_EVERY, BM_GETCHECK, 0, 0) == BST_CHECKED);
            bool desktopFirst = (SendDlgItemMessageW(hwnd, IDC_CHK_DESKTOP_FIRST, BM_GETCHECK, 0, 0) == BST_CHECKED);
            TaskbarSettingsProvider::SetCharmsEnabled(charms);
            TaskbarSettingsProvider::SetCornerSwitchApps(switchApps);
            TaskbarSettingsProvider::SetDesktopFirst(desktopStart);
            TaskbarSettingsProvider::SetBackgroundOnStart(bgOnStart);
            TaskbarSettingsProvider::SetStartOnCurrentDisplay(startDisplay);
            TaskbarSettingsProvider::SetAppsViewAuto(appsView);
            TaskbarSettingsProvider::SetSearchEverywhere(searchEvery);
            TaskbarSettingsProvider::SetDesktopAppsFirst(desktopFirst);
        }
    }
    HWND hList = GetDlgItem(hwnd, IDC_LST_TOOLBARS);
    if (hList) {
        bool addr = (ListView_GetCheckState(hList, 0) != 0);
        bool links = (ListView_GetCheckState(hList, 1) != 0);
        bool tablet = (ListView_GetCheckState(hList, 2) != 0);
        bool desk = (ListView_GetCheckState(hList, 3) != 0);
        bool classicToolbarChecked = addr || links || desk;
        ApplyToolbars(addr, links, tablet, desk);
        
        // Se le toolbar companion sono disattivate via impostazioni, informiamo l'utente
        if (classicToolbarChecked && g_showToolbarWarning && ShouldUseCompanionToolbars()) {
            MessageBoxW(hwnd, g_str.toolbars_warn_text, g_str.toolbars_warn_title, MB_OK | MB_ICONINFORMATION);
        }
    }
    SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"TraySettings");
}

static void ShowAboutDialog(HWND parent) {
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
        g_hStartFontUi = CreateFontW(ptPx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        SetWindowTextW(hwnd, g_str.start_custom_title);
        const DialogControlBinding bindings[] = {
            { IDC_START_GRP_TILES, g_str.start_grp_tiles },
            { IDC_CHK_MORE_TILES, g_str.start_chk_more_tiles },
            { IDC_CHK_APP_LIST, g_str.start_chk_app_list },
            { IDC_CHK_RECENT_APPS, g_str.start_chk_recent_apps },
            { IDC_CHK_FULLSCREEN, g_str.start_chk_fullscreen },
            { IDC_CHK_RECENT_ITEMS, g_str.start_chk_recent_items },
            { IDC_CHK_ACCOUNT_NOTIF, g_str.start_chk_account_notif },
            { IDC_GRP_SEARCH, g_str.start_grp_search },
            { IDC_CHK_SEARCH_PROGRAMS, g_str.start_chk_search_programs },
            { IDC_CHK_SEARCH_FILES, g_str.start_chk_search_files },
            { IDC_START_GRP_FOLDERS, g_str.start_grp_folders },
            { IDC_CHK_FOLDER_SETTINGS, g_str.start_chk_folder_settings },
            { IDC_CHK_FOLDER_DOCS, g_str.start_chk_folder_docs },
            { IDC_CHK_FOLDER_DOWNLOADS, g_str.start_chk_folder_downloads },
            { IDC_CHK_FOLDER_MUSIC, g_str.start_chk_folder_music },
            { IDC_CHK_FOLDER_PICS, g_str.start_chk_folder_pics },
            { IDC_CHK_FOLDER_VIDEOS, g_str.start_chk_folder_videos },
            { IDC_CHK_FOLDER_NETWORK, g_str.start_chk_folder_network },
            { IDC_CHK_FOLDER_PERSONAL, g_str.start_chk_folder_personal },
            { IDC_START_STATIC_INFO, g_str.start_info_restart },
            { IDC_START_BTN_APPLY, g_str.btn_apply },
            { IDOK, g_str.btn_ok },
            { IDCANCEL, g_str.btn_cancel },
        };
        for (const auto& b : bindings)
            SetDlgItemTextW(hwnd, b.controlId, b.text);
        if (g_hStartFontUi) {
            SendMessageW(hwnd, WM_SETFONT, (WPARAM)g_hStartFontUi, TRUE);
            SetFontAllChildren(hwnd, g_hStartFontUi);
        }
        for (const auto& s : g_startSettings) {
            bool state = GetStartSettingState(s);
            bool locked = s.policyValue && IsSettingLockedByPolicy(s.policyValue);
            HWND hCtrl = GetDlgItem(hwnd, s.controlId);
            if (hCtrl) {
                SendMessageW(hCtrl, BM_SETCHECK, state ? BST_CHECKED : BST_UNCHECKED, 0);
                EnableWindow(hCtrl, !locked);
            }
        }
        for (const auto& f : g_startFolders) {
            bool state = TaskbarSettingsProvider::GetStartFolderVisible(f.folderRegValue);
            HWND hCtrl = GetDlgItem(hwnd, f.controlId);
            if (hCtrl) SendMessageW(hCtrl, BM_SETCHECK, state ? BST_CHECKED : BST_UNCHECKED, 0);
        }
        EnableWindow(GetDlgItem(hwnd, IDC_START_BTN_APPLY), FALSE);
        RECT rc; GetWindowRect(hwnd, &rc);
        int ww = rc.right - rc.left, wh = rc.bottom - rc.top;
        SetWindowPos(hwnd, NULL, (GetSystemMetrics(SM_CXSCREEN) - ww) / 2, (GetSystemMetrics(SM_CYSCREEN) - wh) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        LONG style = GetWindowLongW(hwnd, GWL_STYLE);
        style &= ~WS_THICKFRAME; style &= ~WS_MAXIMIZEBOX;
        SetWindowLongW(hwnd, GWL_STYLE, style);
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
        return TRUE;
    }
    case WM_COMMAND: {
        WORD id = LOWORD(wp);
        WORD act = HIWORD(wp);
        if (act == BN_CLICKED && id != IDOK && id != IDCANCEL && id != IDC_START_BTN_APPLY)
            EnableWindow(GetDlgItem(hwnd, IDC_START_BTN_APPLY), TRUE);
        if (id == IDOK) {
            SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_START_BTN_APPLY, BN_CLICKED), 0);
            if (IsWindow(hwnd)) DestroyWindow(hwnd);
        } else if (id == IDCANCEL) {
            DestroyWindow(hwnd);
        } else if (id == IDC_START_BTN_APPLY) {
            for (const auto& s : g_startSettings) {
                HWND hCtrl = GetDlgItem(hwnd, s.controlId);
                if (hCtrl && IsWindowEnabled(hCtrl)) {
                    bool checked = (SendMessageW(hCtrl, BM_GETCHECK, 0, 0) == BST_CHECKED);
                    SetStartSettingState(s, checked);
                }
            }
            for (const auto& f : g_startFolders) {
                HWND hCtrl = GetDlgItem(hwnd, f.controlId);
                if (hCtrl) {
                    bool checked = (SendMessageW(hCtrl, BM_GETCHECK, 0, 0) == BST_CHECKED);
                    TaskbarSettingsProvider::SetStartFolderVisible(f.folderRegValue, checked);
                }
            }
            SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Windows");
            MessageBoxW(hwnd, g_str.start_msg_saved, g_str.start_msg_saved_title, MB_OK | MB_ICONINFORMATION);
            EnableWindow(GetDlgItem(hwnd, IDC_START_BTN_APPLY), FALSE);
        }
        break;
    }
    case WM_DESTROY:
        if (g_hStartFontUi) { DeleteObject(g_hStartFontUi); g_hStartFontUi = NULL; }
        g_hwndStartCustom = NULL;
        InterlockedExchange(&g_startCustomOpen, 0);
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    }
    return FALSE;
}

static void ShowStartCustomDialog(HWND parent) {
    if (InterlockedExchange(&g_startCustomOpen, 1)) {
        HWND hw = g_hwndStartCustom;
        if (hw && IsWindow(hw)) { SetForegroundWindow(hw); if (IsIconic(hw)) ShowWindow(hw, SW_RESTORE); }
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
    if (hwnd) {
        ShowWindow(hwnd, SW_SHOWNORMAL);
        SetForegroundWindow(hwnd);
    } else {
        InterlockedExchange(&g_startCustomOpen, 0);
    }
}

static INT_PTR CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
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
        g_hFontUi = CreateFontW(ptPx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        SetWindowTextW(hwnd, g_str.title);
        HWND hTab = GetDlgItem(hwnd, IDC_TAB_MAIN);
        SendMessageW(hTab, TCM_DELETEALLITEMS, 0, 0);
        auto addTab = [&](int i, const WCHAR* s) {
            TCITEMW ti = { TCIF_TEXT, 0, 0, (LPWSTR)s };
            SendMessageW(hTab, TCM_INSERTITEMW, i, (LPARAM)&ti);
        };
        if (IsWin81Profile()) {
            addTab(0, g_str.tab_taskbar);
            addTab(1, g_str.tab_navigation);
            addTab(2, g_str.tab_start);
            addTab(3, g_str.tab_toolbars);
        } else {
            addTab(0, g_str.tab_taskbar);
            addTab(1, g_str.tab_start);
            addTab(2, g_str.tab_toolbars);
        }
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
        for (const auto& b : bindings) SetDlgItemTextW(hwnd, b.controlId, b.text);
        if (IsWin81Profile()) {
            SetDlgItemTextW(hwnd, IDC_GRP_CORNER_NAV, g_str.grp_corner_nav);
            SetDlgItemTextW(hwnd, IDC_CHK_CHARMS, g_str.chk_charms);
            SetDlgItemTextW(hwnd, IDC_CHK_SWITCH_APPS, g_str.chk_switch_apps);
            SetDlgItemTextW(hwnd, IDC_CHK_POWERSHELL, g_str.chk_powershell);
            SetDlgItemTextW(hwnd, IDC_GRP_START_SCREEN, g_str.grp_start_screen);
            SetDlgItemTextW(hwnd, IDC_CHK_DESKTOP_START, g_str.chk_desktop_start);
            SetDlgItemTextW(hwnd, IDC_CHK_BG_ON_START, g_str.chk_bg_on_start);
            SetDlgItemTextW(hwnd, IDC_CHK_START_DISPLAY, g_str.chk_start_display);
            SetDlgItemTextW(hwnd, IDC_CHK_APPS_VIEW, g_str.chk_apps_view);
            SetDlgItemTextW(hwnd, IDC_CHK_SEARCH_EVERY, g_str.chk_search_every);
            SetDlgItemTextW(hwnd, IDC_CHK_DESKTOP_FIRST, g_str.chk_desktop_first);
            SendDlgItemMessageW(hwnd, IDC_CHK_CHARMS, BM_SETCHECK, TaskbarSettingsProvider::GetCharmsEnabled() ? BST_CHECKED : BST_UNCHECKED, 0);
            SendDlgItemMessageW(hwnd, IDC_CHK_SWITCH_APPS, BM_SETCHECK, TaskbarSettingsProvider::GetCornerSwitchApps() ? BST_CHECKED : BST_UNCHECKED, 0);
            SendDlgItemMessageW(hwnd, IDC_CHK_POWERSHELL, BM_SETCHECK, TaskbarSettingsProvider::GetPowerShellWinX() ? BST_CHECKED : BST_UNCHECKED, 0);
            SendDlgItemMessageW(hwnd, IDC_CHK_DESKTOP_START, BM_SETCHECK, TaskbarSettingsProvider::GetDesktopFirst() ? BST_CHECKED : BST_UNCHECKED, 0);
            SendDlgItemMessageW(hwnd, IDC_CHK_BG_ON_START, BM_SETCHECK, TaskbarSettingsProvider::GetBackgroundOnStart() ? BST_CHECKED : BST_UNCHECKED, 0);
            SendDlgItemMessageW(hwnd, IDC_CHK_START_DISPLAY, BM_SETCHECK, TaskbarSettingsProvider::GetStartOnCurrentDisplay() ? BST_CHECKED : BST_UNCHECKED, 0);
            SendDlgItemMessageW(hwnd, IDC_CHK_APPS_VIEW, BM_SETCHECK, TaskbarSettingsProvider::GetAppsViewAuto() ? BST_CHECKED : BST_UNCHECKED, 0);
            SendDlgItemMessageW(hwnd, IDC_CHK_SEARCH_EVERY, BM_SETCHECK, TaskbarSettingsProvider::GetSearchEverywhere() ? BST_CHECKED : BST_UNCHECKED, 0);
            SendDlgItemMessageW(hwnd, IDC_CHK_DESKTOP_FIRST, BM_SETCHECK, TaskbarSettingsProvider::GetDesktopAppsFirst() ? BST_CHECKED : BST_UNCHECKED, 0);
        }
        if (g_hFontUi) {
            SendMessageW(hwnd, WM_SETFONT, (WPARAM)g_hFontUi, TRUE);
            SetFontAllChildren(hwnd, g_hFontUi);
            UpdateWindow(hwnd);
        }
        if (IsWin81Profile()) {
            static const int belowAero[] = { 0 };
            AutoSizeMultilineControl(hwnd, IDC_CHK_AEROPEEK, 12, belowAero);
            static const int belowPowershell[] = {
                IDC_GRP_START_SCREEN, IDC_CHK_DESKTOP_START, IDC_CHK_BG_ON_START,
                IDC_CHK_START_DISPLAY, IDC_CHK_APPS_VIEW, IDC_CHK_SEARCH_EVERY,
                IDC_CHK_DESKTOP_FIRST, 0
            };
            AutoSizeMultilineControl(hwnd, IDC_CHK_POWERSHELL, 12, belowPowershell);
        }
        BalanceTextAndCombo(hwnd, IDC_TXT_LOCATION, IDC_COMBO_LOCATION);
        BalanceTextAndCombo(hwnd, IDC_TXT_BUTTONS, IDC_COMBO_BUTTONS);
        BalanceTextAndCombo(hwnd, IDC_TXT_POWER_LABEL, IDC_COMBO_POWER);
        HWND hCL = GetDlgItem(hwnd, IDC_COMBO_LOCATION);
        SendMessageW(hCL, CB_RESETCONTENT, 0, 0);
        SendMessageW(hCL, CB_ADDSTRING, 0, (LPARAM)g_str.pos_left);
        SendMessageW(hCL, CB_ADDSTRING, 0, (LPARAM)g_str.pos_top);
        SendMessageW(hCL, CB_ADDSTRING, 0, (LPARAM)g_str.pos_right);
        SendMessageW(hCL, CB_ADDSTRING, 0, (LPARAM)g_str.pos_bottom);
        EnableWindow(hCL, TRUE);
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
        DWORD edge = TaskbarSettingsProvider::GetTaskbarEdge();
        bool aeroPeek = TaskbarSettingsProvider::GetAeroPeekEnabled();
        APPBARDATA abd = { sizeof(APPBARDATA) };
        abd.hWnd = FindWindowW(L"Shell_TrayWnd", NULL);
        bool isHide = (SHAppBarMessage(ABM_GETSTATE, &abd) & ABS_AUTOHIDE) != 0;
        SendDlgItemMessageW(hwnd, IDC_CHK_LOCK, BM_SETCHECK, (szMove == 0) ? BST_CHECKED : BST_UNCHECKED, 0);
        SendDlgItemMessageW(hwnd, IDC_CHK_HIDE, BM_SETCHECK, isHide ? BST_CHECKED : BST_UNCHECKED, 0);
        SendDlgItemMessageW(hwnd, IDC_CHK_SMALL, BM_SETCHECK, (szSmall != 0) ? BST_CHECKED : BST_UNCHECKED, 0);
        SendDlgItemMessageW(hwnd, IDC_CHK_AEROPEEK, BM_SETCHECK, aeroPeek ? BST_CHECKED : BST_UNCHECKED, 0);
        SendMessageW(hCL, CB_SETCURSEL, (edge < 4) ? (WPARAM)edge : 3, 0);
        if (IsWin81Profile()) {
            SendMessageW(hCB, CB_SETCURSEL, 0, 0);
        } else {
            SendMessageW(hCB, CB_SETCURSEL, (glom < 3) ? (WPARAM)glom : 0, 0);
        }
        DWORD curPower = TaskbarSettingsProvider::GetPowerAction();
        int powerIdx = 0;
        for (int i = 0; i < 7; i++) {
            if (kPowerValues[i] == curPower) {
                powerIdx = i;
                break;
            }
        }
        SendMessageW(hCP, CB_SETCURSEL, powerIdx, 0);
        SendDlgItemMessageW(hwnd, IDC_CHK_MRU_PROG, BM_SETCHECK, TaskbarSettingsProvider::GetStartMruProgs() ? BST_CHECKED : BST_UNCHECKED, 0);
        SendDlgItemMessageW(hwnd, IDC_CHK_MRU_ITEMS, BM_SETCHECK, TaskbarSettingsProvider::GetStartMruItems() ? BST_CHECKED : BST_UNCHECKED, 0);
        InitToolbarsList(GetDlgItem(hwnd, IDC_LST_TOOLBARS));
        EnableWindow(GetDlgItem(hwnd, IDC_BTN_APPLY), FALSE);
        SwitchTab(hwnd, 0);
        RECT rc; GetWindowRect(hwnd, &rc);
        int ww = rc.right - rc.left, wh = rc.bottom - rc.top;
        SetWindowPos(hwnd, NULL, (GetSystemMetrics(SM_CXSCREEN) - ww) / 4, (GetSystemMetrics(SM_CYSCREEN) - wh) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        LONG style = GetWindowLongW(hwnd, GWL_STYLE);
        style &= ~WS_THICKFRAME;
        style &= ~WS_MAXIMIZEBOX;
        SetWindowLongW(hwnd, GWL_STYLE, style);
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
        return TRUE;
    }
    case WM_GETMINMAXINFO: {
        MINMAXINFO* mmi = (MINMAXINFO*)lp;
        RECT rc;
        GetWindowRect(hwnd, &rc);
        mmi->ptMinTrackSize.x = mmi->ptMaxTrackSize.x = rc.right - rc.left;
        mmi->ptMinTrackSize.y = mmi->ptMaxTrackSize.y = rc.bottom - rc.top;
        return 0;
    }
    case WM_COMMAND: {
        WORD id = LOWORD(wp);
        WORD act = HIWORD(wp);
        if ((act == BN_CLICKED || act == CBN_SELCHANGE)
            && id != IDOK && id != IDCANCEL && id != IDC_BTN_APPLY
            && id != IDC_BTN_CUST_NOTIF && id != IDC_BTN_START_CUST)
            EnableWindow(GetDlgItem(hwnd, IDC_BTN_APPLY), TRUE);
        if (id == IDC_CHK_APPS_VIEW && act == BN_CLICKED) {
            UpdateSearchEverywhereState(hwnd);
        }
        if (id == IDOK) { ApplySettings(hwnd); if (IsWindow(hwnd)) DestroyWindow(hwnd); }
        else if (id == IDCANCEL) { DestroyWindow(hwnd); }
        else if (id == IDC_BTN_APPLY) { ApplySettings(hwnd); if (IsWindow(hwnd)) EnableWindow(GetDlgItem(hwnd, IDC_BTN_APPLY), FALSE); }
        else if (id == IDC_BTN_CUST_NOTIF){
            HINSTANCE hRes = ShellExecuteW(hwnd, L"open", L"shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}", NULL, NULL, SW_SHOW);
            if ((INT_PTR)hRes <= 32) {
                ShellExecuteW(hwnd, L"open", L"control.exe", L"/name Microsoft.NotificationAreaIcons", NULL, SW_SHOW);
            }
        }
        else if (id == IDC_BTN_START_CUST){
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
        if (hdr->idFrom == IDC_LINK_HELP && hdr->code == NM_CLICK)
            ShowAboutDialog(hwnd);
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
    addCtrl(TCS_TABS | WS_TABSTOP, 0, 6, 6, 250, 246, IDC_TAB_MAIN, L"SysTabControl32", L"");
    addCtrl(BS_GROUPBOX, 0, 12, 22, 238, 96, IDC_GRP_APPEARANCE, L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 18, 33, 226, 10, IDC_CHK_LOCK, L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 18, 45, 226, 10, IDC_CHK_HIDE, L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 18, 57, 226, 10, IDC_CHK_SMALL, L"Button", L"");
    addCtrl(SS_LEFT | SS_EDITCONTROL, 0, 18, 73, 110, 10, IDC_TXT_LOCATION, L"Static", L"");
    addCtrl(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 0, 132, 71, 112, 100, IDC_COMBO_LOCATION, L"ComboBox", L"");
    addCtrl(SS_LEFT | SS_EDITCONTROL, 0, 18, 89, 95, 18, IDC_TXT_BUTTONS, L"Static", L"");
    addCtrl(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 0, 80, 87, 164, 70, IDC_COMBO_BUTTONS, L"ComboBox", L"");
    addCtrl(BS_GROUPBOX, 0, 12, 120, 238, 46, IDC_GRP_NOTIF, L"Button", L"");
    addCtrl(SS_LEFT, 0, 18, 131, 148, 26, IDC_TXT_NOTIF, L"Static", L"");
    addCtrl(BS_PUSHBUTTON | WS_TABSTOP, 0, 172, 134, 70, 14, IDC_BTN_CUST_NOTIF, L"Button", L"");
    addCtrl(BS_GROUPBOX, 0, 12, 168, 238, 66, IDC_GRP_AERO, L"Button", L"");
    addCtrl(SS_LEFT, 0, 18, 179, 226, 30, IDC_TXT_AERO, L"Static", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 18, 213, 226, 12, IDC_CHK_AEROPEEK, L"Button", L"");
    addCtrl(WS_TABSTOP, 0, 12, 238, 238, 10, IDC_LINK_HELP, L"SysLink", L"");
    addCtrl(BS_GROUPBOX, 0, 12, 22, 238, 86, IDC_GRP_CORNER_NAV, L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP | BS_MULTILINE, 0, 18, 35, 226, 20, IDC_CHK_CHARMS, L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP | BS_MULTILINE, 0, 18, 57, 226, 20, IDC_CHK_SWITCH_APPS, L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP | BS_MULTILINE, 0, 18, 79, 226, 26, IDC_CHK_POWERSHELL, L"Button", L"");
    addCtrl(BS_GROUPBOX, 0, 12, 112, 238, 124, IDC_GRP_START_SCREEN, L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP | BS_MULTILINE, 0, 18, 125, 226, 20, IDC_CHK_DESKTOP_START, L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP | BS_MULTILINE, 0, 18, 147, 226, 12, IDC_CHK_BG_ON_START, L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP | BS_MULTILINE, 0, 18, 161, 226, 20, IDC_CHK_START_DISPLAY, L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP | BS_MULTILINE, 0, 18, 183, 226, 22, IDC_CHK_APPS_VIEW, L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP | BS_MULTILINE, 0, 32, 200, 212, 18, IDC_CHK_SEARCH_EVERY, L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP | BS_MULTILINE, 0, 18, 219, 226, 20, IDC_CHK_DESKTOP_FIRST, L"Button", L"");
    addCtrl(BS_DEFPUSHBUTTON | WS_TABSTOP, 0, 84, 271, 52, 14, IDOK, L"Button", L"OK");
    addCtrl(BS_PUSHBUTTON | WS_TABSTOP, 0, 140, 271, 52, 14, IDCANCEL, L"Button", L"Cancel");
    addCtrl(BS_PUSHBUTTON | WS_TABSTOP, 0, 196, 271, 58, 14, IDC_BTN_APPLY, L"Button", L"Apply");
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
        WaitForSingleObject(g_dialogThread, INFINITE);
        CloseHandle(g_dialogThread);
        g_dialogThread = NULL;
    }
    g_dialogThread = CreateThread(NULL, 0, DialogThreadProc, NULL, 0, NULL);
}

BOOL WINAPI ShellExecuteExW_hook(SHELLEXECUTEINFOW* pei) {
    if (IsChildProcess()) return ShellExecuteExW_orig(pei);
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
    if (IsChildProcess()) return ShellExecuteW_orig(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
    HookGuard guard;
    if (guard.IsReentrant()) return ShellExecuteW_orig(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
    if (lpFile && _wcsnicmp(lpFile, L"ms-settings:taskbar", 19) == 0) {
        ShowTaskbarProperties();
        return SHELL_EXECUTE_SUCCESS;
    }
    return ShellExecuteW_orig(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
}

BOOL Wh_ModInit() {
    InterlockedExchange(&g_dialogOpen, 0);
    InterlockedExchange(&g_startCustomOpen, 0);
    BuildChildEnvironment();
    LoadLanguageSetting();
    LoadProfileSetting();
    LoadToolbarModeSetting(); // Caricamento nuova opzione
    LoadToolbarWarningSetting();
    LoadHideNonFunctionalSetting();
    InitLocalization();
    EnsureThemeActCtx();
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
    g_realToolbarThread = CreateThread(NULL, 0, RealToolbarThreadProc, NULL, 0, &g_realToolbarThreadId);
    return TRUE;
}

void Wh_ModUninit() {
    if (g_realToolbarThreadId) {
        PostThreadMessageW(g_realToolbarThreadId, WM_QUIT, 0, 0);
    }
    if (g_realToolbarThread) {
        WaitForSingleObject(g_realToolbarThread, 3000);
        CloseHandle(g_realToolbarThread);
        g_realToolbarThread = NULL;
    }
    if (g_hwndMain && IsWindow(g_hwndMain))
        PostMessageW(g_hwndMain, WM_CLOSE, 0, 0);
    if (g_hwndStartCustom && IsWindow(g_hwndStartCustom))
        PostMessageW(g_hwndStartCustom, WM_CLOSE, 0, 0);
    if (g_dialogThread) {
        WaitForSingleObject(g_dialogThread, 5000);
        CloseHandle(g_dialogThread);
        g_dialogThread = NULL;
    }
    if (g_hFontUi) { DeleteObject(g_hFontUi); g_hFontUi = NULL; }
    if (g_hStartFontUi) { DeleteObject(g_hStartFontUi); g_hStartFontUi = NULL; }
    if (g_hActCtx != INVALID_HANDLE_VALUE) {
        ReleaseActCtx(g_hActCtx);
        g_hActCtx = INVALID_HANDLE_VALUE;
    }
}

void Wh_ModSettingsChanged() {
    LoadLanguageSetting();
    LoadProfileSetting();
    LoadToolbarModeSetting(); // Aggiornamento runtime nuova opzione
    LoadToolbarWarningSetting();
    LoadHideNonFunctionalSetting();
    InitLocalization();
    
    // Forza l'aggiornamento/la distruzione della barra ricreata a seconda della nuova preferenza
    if (g_hwndRealToolbarHost && IsWindow(g_hwndRealToolbarHost)) {
        PostMessageW(g_hwndRealToolbarHost, WM_RTB_REBUILD, 0, 0);
    }
    
    if (g_hwndMain && IsWindow(g_hwndMain) && IsWin81Profile()) {
        for (int i = 0; kNonFunctionalNavCtls[i]; i++) {
            HWND h = GetDlgItem(g_hwndMain, kNonFunctionalNavCtls[i]);
            if (h) {
                EnableWindow(h, !g_hideNonFunctional);
                if (g_hideNonFunctional) {
                    SendMessageW(h, BM_SETCHECK, BST_UNCHECKED, 0);
                }
            }
        }
        InvalidateRect(g_hwndMain, NULL, TRUE);
        UpdateWindow(g_hwndMain);
    }
}


