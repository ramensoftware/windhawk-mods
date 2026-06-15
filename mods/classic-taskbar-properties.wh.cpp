// ==WindhawkMod==
// @id              classic-taskbar-properties
// @name            Classic Taskbar and Start Menu Properties
// @description     Restores the classic Windows 7-style "Taskbar and Start Menu Properties" dialog with full functionality. Includes all three tabs (Taskbar, Start Menu, Toolbars). Primarily designed for Windows 10. Windows 11 support may be limited.
// @version         2.0.2
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @compilerOptions -lgdi32 -lcomctl32 -luser32 -lole32 -lshlwapi -lshell32
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Classic Taskbar and Start Menu Properties

This Windhawk mod restores the classic "Taskbar and Start Menu Properties" dialog inspired by Windows 7.

## Features

- **Lock/Unlock the taskbar**
- **Auto-hide the taskbar**
- **Use small icons**
- **Configure taskbar button grouping** (Always combine, Combine when full, Never combine)
- **Configure Aero Peek**
- **Start Menu customization** — only settings that actually work on Windows 10 are included
- **Toolbars tab** — Address, Links, Tablet PC Input Panel, Desktop
## Screenshots

![Classic Taskbar Properties Dialog](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/pic.PNG)

## Start Menu Settings (Windows 10)

| Setting | Registry Key | Notes |
|---------|-------------|-------|
| Show more tiles | Start_ShowMoreTiles | Works on Win10 |
| Show app list | Start_ShowAppList | Works on Win10 |
| Show recently added apps | Start_TrackProgs | Works on Win10 |
| Use Start full screen | ForceStartSize | Works on Win10 |
| Show recently opened items | Start_TrackDocs | Works on Win10 |
| Show account notifications | Start_NotifyNewApps | Works on Win10 |
| Search programs | Start_SearchPrograms | Works on Win10 |
| Search files | Start_SearchFiles | Works on Win10 |
| Folders on Start | Per-folder registry values | Win10 HKCU\...\Start |

## Compatibility

| Feature | Windows 10 21H2 | Windows 11 |
|---------|----------------|------------|
| Taskbar tab | Full support | Partial |
| Start Menu tab | Full support | May vary |
| Toolbars tab | Full support | Limited |
| Position change | Not available | Not available |

## Known Limitations

- Changing taskbar position is unavailable (risk of Explorer instability).
- Classic toolbars (Address, Links, Desktop) have limited support on newer Windows versions.
- Some Start Menu settings may require an Explorer restart or logout to take full effect.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Lingua / Language
  $description: User interface language. Use "auto" for automatic detection.
  $options:
    - auto: Automatica / Auto
    - it: Italiano
    - en: English
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <strsafe.h>

#ifndef SIID_TASKBAR
#define SIID_TASKBAR 39
#endif

namespace StuckRects {
    constexpr DWORD SETTINGS_EDGE_OFFSET = 12;
    constexpr DWORD MIN_SETTINGS_SIZE    = 16;
}

namespace DialogSizes {
    constexpr short MAIN_WIDTH   = 262;
    constexpr short MAIN_HEIGHT  = 290;
    constexpr short START_WIDTH  = 252;
    constexpr short START_HEIGHT = 310;
}

// Costante per ShellExecute successo (presa da settings-to-control-panel)
static const HINSTANCE SHELL_EXECUTE_SUCCESS = (HINSTANCE)33;

// Anti-rientranza hook (preso da settings-to-control-panel)
static thread_local int g_hookDepth = 0;

struct HookGuard {
    HookGuard() { ++g_hookDepth; }
    ~HookGuard() { --g_hookDepth; }
    bool IsReentrant() const { return g_hookDepth > 1; }
};

// Controllo processo figlio (preso da settings-to-control-panel)
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

static HWND          g_hwndMain        = NULL;
static HFONT         g_hFontUi         = NULL;
static HBRUSH        g_hBrushWindow    = NULL;
static HBRUSH        g_hBrushBtnFace   = NULL;
static LONG volatile g_dialogOpen      = 0;
static int           g_currentTab      = 0;

static HWND          g_hwndStartCustom = NULL;
static LONG volatile g_startCustomOpen = 0;
static HFONT         g_hStartFontUi    = NULL;
static HBRUSH        g_hStartBrushWindow = NULL;

static HANDLE        g_dialogThread    = NULL;

using ShellExecuteExW_t = BOOL(WINAPI*)(SHELLEXECUTEINFOW*);
static ShellExecuteExW_t ShellExecuteExW_orig = nullptr;

using ShellExecuteW_t = HINSTANCE(WINAPI*)(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, INT);
static ShellExecuteW_t ShellExecuteW_orig = nullptr;

static constexpr LPCWSTR kAdvKey    = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";
static constexpr LPCWSTR kPolicyKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer";
static constexpr LPCWSTR kStuckKey  = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StuckRects3";
static constexpr LPCWSTR kDWMKey    = L"Software\\Microsoft\\Windows\\DWM";
static constexpr LPCWSTR kSMKey     = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer";
static constexpr LPCWSTR kStartFolderKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Start";

static const WCHAR* kAddrBandGUID  = L"{01E04581-4EEE-11D0-BFE9-00AA005B4383}";
static const WCHAR* kLinksBandGUID = L"{0E5CBF21-15B6-11D2-9F3E-00C04FA31A86}";
static const WCHAR* kDeskBandGUID  = L"{D82BE2B0-5764-11D0-A96E-00C04FD705A2}";

static const DWORD kPowerValues[] = { 2, 4, 16, 64, 256, 512, 1024 };

#define IDC_TAB_MAIN            3000
#define IDC_CHK_LOCK            3001
#define IDC_CHK_HIDE            3002
#define IDC_CHK_SMALL           3003
#define IDC_COMBO_LOCATION      3004
#define IDC_COMBO_BUTTONS       3005
#define IDC_BTN_CUST_NOTIF      3006
#define IDC_CHK_AEROPEEK        3007
#define IDC_LINK_HELP           3008
#define IDC_BTN_APPLY           3009
#define IDC_GRP_APPEARANCE      4001
#define IDC_GRP_NOTIF           4002
#define IDC_GRP_AERO            4003
#define IDC_TXT_LOCATION        4004
#define IDC_TXT_BUTTONS         4005
#define IDC_TXT_NOTIF           4006
#define IDC_TXT_AERO            4007
#define IDC_TXT_START_INFO      5001
#define IDC_BTN_START_CUST      5002
#define IDC_TXT_POWER_LABEL     5003
#define IDC_COMBO_POWER         5004
#define IDC_GRP_PRIVACY         5005
#define IDC_CHK_MRU_PROG        5006
#define IDC_CHK_MRU_ITEMS       5007
#define IDC_TXT_TOOLBARS_INFO   6001
#define IDC_LST_TOOLBARS        6002

#define IDC_START_GRP_TILES        7001
#define IDC_CHK_MORE_TILES         7002
#define IDC_CHK_APP_LIST           7003
#define IDC_CHK_RECENT_APPS        7004
#define IDC_CHK_FULLSCREEN         7005
#define IDC_CHK_RECENT_ITEMS       7006
#define IDC_CHK_ACCOUNT_NOTIF      7007
#define IDC_GRP_SEARCH             7008
#define IDC_CHK_SEARCH_PROGRAMS    7009
#define IDC_CHK_SEARCH_FILES       7010
#define IDC_START_GRP_FOLDERS      7011
#define IDC_CHK_FOLDER_SETTINGS    7012
#define IDC_CHK_FOLDER_DOCS        7013
#define IDC_CHK_FOLDER_DOWNLOADS   7014
#define IDC_CHK_FOLDER_MUSIC       7015
#define IDC_CHK_FOLDER_PICS        7016
#define IDC_CHK_FOLDER_VIDEOS      7017
#define IDC_CHK_FOLDER_NETWORK     7018
#define IDC_CHK_FOLDER_PERSONAL    7019
#define IDC_START_BTN_APPLY        7020
#define IDC_START_STATIC_INFO      7021

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
    DWORD   defaultValue;
    bool    invertedLogic;
    int     controlId;
};

struct StartFolder {
    LPCWSTR folderRegValue;
    int     controlId;
};

struct DialogControlBinding {
    int          controlId;
    const WCHAR* text;
};

static const StartSetting g_startSettings[] = {
    { kAdvKey, L"Start_ShowMoreTiles",   NULL,                          0, false, IDC_CHK_MORE_TILES     },
    { kAdvKey, L"Start_ShowAppList",     NULL,                          1, false, IDC_CHK_APP_LIST       },
    { kAdvKey, L"Start_TrackProgs",      L"ClearRecentProgHistory",     1, false, IDC_CHK_RECENT_APPS    },
    { kAdvKey, L"ForceStartSize",        NULL,                          0, false, IDC_CHK_FULLSCREEN     },
    { kAdvKey, L"Start_TrackDocs",       L"ClearRecentDocsHistory",     1, false, IDC_CHK_RECENT_ITEMS   },
    { kAdvKey, L"Start_NotifyNewApps",   NULL,                          1, false, IDC_CHK_ACCOUNT_NOTIF  },
    { kAdvKey, L"Start_SearchPrograms",  NULL,                          1, false, IDC_CHK_SEARCH_PROGRAMS},
    { kAdvKey, L"Start_SearchFiles",     NULL,                          1, false, IDC_CHK_SEARCH_FILES   },
};

static const StartFolder g_startFolders[] = {
    { L"SettingsVisibility",   IDC_CHK_FOLDER_SETTINGS  },
    { L"DocumentsVisibility",  IDC_CHK_FOLDER_DOCS      },
    { L"DownloadsVisibility",  IDC_CHK_FOLDER_DOWNLOADS },
    { L"MusicVisibility",      IDC_CHK_FOLDER_MUSIC     },
    { L"PicturesVisibility",   IDC_CHK_FOLDER_PICS      },
    { L"VideosVisibility",     IDC_CHK_FOLDER_VIDEOS    },
    { L"NetworkVisibility",    IDC_CHK_FOLDER_NETWORK   },
    { L"UserFolderVisibility", IDC_CHK_FOLDER_PERSONAL  },
};

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
        LSTATUS r = RegCreateKeyExW(hRoot, sub, 0, NULL,
                                    REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hk, NULL);
        if (r != ERROR_SUCCESS) return false;
        r = RegSetValueExW(hk, name, 0, REG_DWORD, (const BYTE*)&v, sizeof(DWORD));
        RegCloseKey(hk);
        return (r == ERROR_SUCCESS);
    }

    static bool GetLockState()          { return RegGetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarSizeMove", 1) == 0; }
    static void SetLockState(bool lock) { RegSetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarSizeMove", lock ? 0 : 1); }

    static bool GetSmallIcons()          { return RegGetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarSmallIcons", 0) != 0; }
    static void SetSmallIcons(bool s)    { RegSetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarSmallIcons", s ? 1 : 0); }

    static DWORD GetGlomLevel()          { return RegGetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarGlomLevel", 0); }
    static void  SetGlomLevel(DWORD lvl) { RegSetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarGlomLevel", lvl); }

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
        return (RegGetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"DisablePreviewWindow", 0) == 0)
            && (RegGetDWordSafe(HKEY_CURRENT_USER, kDWMKey,  L"EnableAeroPeek",       1) != 0);
    }
    static void SetAeroPeekEnabled(bool e) {
        RegSetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"DisablePreviewWindow", e ? 0 : 1);
        RegSetDWordSafe(HKEY_CURRENT_USER, kDWMKey,  L"EnableAeroPeek",       e ? 1 : 0);
    }

    static DWORD GetPowerAction()        { return RegGetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"Start_PowerButtonAction", 2); }
    static void  SetPowerAction(DWORD v) { RegSetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"Start_PowerButtonAction", v); }

    static bool GetStartMruProgs()         { return RegGetDWordSafe(HKEY_CURRENT_USER, kSMKey, L"Start_TrackProgs", 1) != 0; }
    static bool GetStartMruItems()         { return RegGetDWordSafe(HKEY_CURRENT_USER, kSMKey, L"Start_TrackDocs",  1) != 0; }
    static void SetStartMruProgs(bool v)   { RegSetDWordSafe(HKEY_CURRENT_USER, kSMKey, L"Start_TrackProgs", v ? 1 : 0); }
    static void SetStartMruItems(bool v)   { RegSetDWordSafe(HKEY_CURRENT_USER, kSMKey, L"Start_TrackDocs",  v ? 1 : 0); }

    static bool GetStartFolderVisible(LPCWSTR valueName) {
        return RegGetDWordSafe(HKEY_CURRENT_USER, kStartFolderKey, valueName, 1) != 0;
    }
    static void SetStartFolderVisible(LPCWSTR valueName, bool visible) {
        RegSetDWordSafe(HKEY_CURRENT_USER, kStartFolderKey, valueName, visible ? 1 : 0);
    }

    static bool GetToolbarEnabled(LPCWSTR name) {
        if (wcscmp(name, L"TabletPC") == 0)
            return RegGetDWordSafe(HKEY_CURRENT_USER,
                L"Software\\Microsoft\\TabletTip\\1.7",
                L"TipbandDesiredVisibility", 0) != 0;

        const WCHAR* guid = nullptr;
        if      (wcscmp(name, L"Address") == 0) guid = kAddrBandGUID;
        else if (wcscmp(name, L"Links")   == 0) guid = kLinksBandGUID;
        else if (wcscmp(name, L"Desktop") == 0) guid = kDeskBandGUID;
        else return false;

        HKEY hk;
        if (RegOpenKeyExW(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Internet Explorer\\Toolbar",
            0, KEY_READ, &hk) == ERROR_SUCCESS)
        {
            DWORD sz = 0;
            LONG res = RegQueryValueExW(hk, guid, NULL, NULL, NULL, &sz);
            RegCloseKey(hk);
            return (res == ERROR_SUCCESS);
        }
        return false;
    }

    static void SetToolbarEnabled(LPCWSTR name, bool enable) {
        if (wcscmp(name, L"TabletPC") == 0) {
            RegSetDWordSafe(HKEY_CURRENT_USER,
                L"Software\\Microsoft\\TabletTip\\1.7",
                L"TipbandDesiredVisibility", enable ? 1 : 0);
            return;
        }
        const WCHAR* guid = nullptr;
        if      (wcscmp(name, L"Address") == 0) guid = kAddrBandGUID;
        else if (wcscmp(name, L"Links")   == 0) guid = kLinksBandGUID;
        else if (wcscmp(name, L"Desktop") == 0) guid = kDeskBandGUID;
        else return;

        HKEY hk;
        if (RegCreateKeyExW(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Internet Explorer\\Toolbar",
            0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE | KEY_READ, NULL, &hk, NULL) == ERROR_SUCCESS)
        {
            if (enable) RegSetValueExW(hk, guid, 0, REG_SZ, (const BYTE*)L"", sizeof(WCHAR));
            else        RegDeleteValueW(hk, guid);
            RegCloseKey(hk);
        }
    }
};

static bool IsSettingLockedByPolicy(LPCWSTR policyValue) {
    if (!policyValue) return false;
    return TaskbarSettingsProvider::RegGetDWordSafe(HKEY_CURRENT_USER, kPolicyKey, policyValue, 0) != 0
        || TaskbarSettingsProvider::RegGetDWordSafe(HKEY_LOCAL_MACHINE,
               L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer",
               policyValue, 0) != 0;
}

static bool GetStartSettingState(const StartSetting& s) {
    if (s.policyValue) {
        DWORD pol = TaskbarSettingsProvider::RegGetDWordSafe(HKEY_CURRENT_USER, kPolicyKey, s.policyValue, 0);
        if (!pol) pol = TaskbarSettingsProvider::RegGetDWordSafe(HKEY_LOCAL_MACHINE,
                            L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer",
                            s.policyValue, 0);
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

static void InitLocalization() {
    bool useItalian = false;
    
    if (wcscmp(g_language, L"it") == 0) {
        useItalian = true;
    } else if (wcscmp(g_language, L"en") == 0) {
        useItalian = false;
    } else {
        useItalian = ((GetUserDefaultUILanguage() & 0xFF) == 0x10);
    }

    if (useItalian) {
        StringCchCopyW(g_str.title,              128, L"Propriet\u00e0 della barra delle applicazioni e del menu Start");
        StringCchCopyW(g_str.tab_taskbar,         64, L"Barra delle applicazioni");
        StringCchCopyW(g_str.tab_start,           64, L"Menu Start");
        StringCchCopyW(g_str.tab_toolbars,        64, L"Barre degli strumenti");
        StringCchCopyW(g_str.btn_ok,              32, L"OK");
        StringCchCopyW(g_str.btn_cancel,          32, L"Annulla");
        StringCchCopyW(g_str.btn_apply,           32, L"Applica");
        StringCchCopyW(g_str.grp_appearance,      64, L"Aspetto della barra delle applicazioni");
        StringCchCopyW(g_str.chk_lock,            64, L"Blocca la barra delle applicazioni");
        StringCchCopyW(g_str.chk_hide,            64, L"Nascondi automaticamente la barra delle applicazioni");
        StringCchCopyW(g_str.chk_small,           64, L"Usa icone piccole");
        StringCchCopyW(g_str.txt_location,        64, L"Posizione sullo schermo:");
        StringCchCopyW(g_str.txt_buttons,         64, L"Pulsanti della barra\ndelle applicazioni:");
        StringCchCopyW(g_str.grp_notif,           64, L"Area di notifica");
        StringCchCopyW(g_str.txt_notif,          128, L"Consente di personalizzare le icone e le notifiche nell'area di notifica.");
        StringCchCopyW(g_str.btn_cust_notif,      32, L"Personalizza...");
        StringCchCopyW(g_str.grp_aero,            64, L"Anteprima del desktop con Aero Peek");
        StringCchCopyW(g_str.txt_aero,           256, L"Consente di visualizzare temporaneamente il desktop portando il puntatore sul pulsante Mostra desktop alla fine della barra delle applicazioni.");
        StringCchCopyW(g_str.chk_aero,            64, L"Usa Aero Peek per visualizzare l'anteprima del desktop");
        StringCchCopyW(g_str.link_help,          128, L"<a>Personalizzazione della barra delle applicazioni</a>");
        StringCchCopyW(g_str.pos_left,            32, L"A sinistra");
        StringCchCopyW(g_str.pos_top,             32, L"In alto");
        StringCchCopyW(g_str.pos_right,           32, L"A destra");
        StringCchCopyW(g_str.pos_bottom,          32, L"In basso");
        StringCchCopyW(g_str.btn_always_combine,  64, L"Combina sempre, nascondi etichette");
        StringCchCopyW(g_str.btn_combine_full,    64, L"Combina se la barra \u00e8 piena");
        StringCchCopyW(g_str.btn_never_combine,   64, L"Mai combinare");
        StringCchCopyW(g_str.start_info,         256, L"Per personalizzare l'aspetto dei collegamenti, delle icone e dei menu nel menu Start, fare clic su Personalizza.");
        StringCchCopyW(g_str.btn_start_cust,      32, L"Personalizza...");
        StringCchCopyW(g_str.txt_power_label,     64, L"Azione pulsante di alimentazione:");
        StringCchCopyW(g_str.power_shutdown,      32, L"Arresta il sistema");
        StringCchCopyW(g_str.power_restart,       32, L"Riavvia");
        StringCchCopyW(g_str.power_sleep,         32, L"Sospensione");
        StringCchCopyW(g_str.power_hibernate,     32, L"Ibernazione");
        StringCchCopyW(g_str.power_logoff,        32, L"Disconnetti");
        StringCchCopyW(g_str.power_lock,          32, L"Blocca");
        StringCchCopyW(g_str.power_switchuser,    32, L"Cambia utente");
        StringCchCopyW(g_str.grp_privacy,         32, L"Privacy");
        StringCchCopyW(g_str.chk_mru_prog,       128, L"Archivia e visualizza i programmi aperti di recente nel menu Start");
        StringCchCopyW(g_str.chk_mru_items,      128, L"Archivia e visualizza gli elementi aperti di recente nel menu Start e nella barra delle applicazioni");
        StringCchCopyW(g_str.toolbars_info,      128, L"Selezionare le barre degli strumenti da aggiungere alla barra delle applicazioni.");
        StringCchCopyW(g_str.toolbar_address,     32, L"Indirizzo");
        StringCchCopyW(g_str.toolbar_links,       32, L"Collegamenti");
        StringCchCopyW(g_str.toolbar_tabletpc,    32, L"Pannello input Tablet PC");
        StringCchCopyW(g_str.toolbar_desktop,     32, L"Desktop");
        StringCchCopyW(g_str.about_title,         64, L"Informazioni sulla mod");
        StringCchCopyW(g_str.about_text,        4096,
            L"Classic Taskbar Properties\r\n\r\n"
            L"Questa mod per Windhawk ripristina la classica finestra "
            L"\"Propriet\u00e0 della barra delle applicazioni e del menu Start\" "
            L"ispirata a Windows 7.\r\n\r\n"
            L"Funzionalit\u00e0 attualmente disponibili:\r\n"
            L"\r\n"
            L"- Blocca la barra delle applicazioni\r\n"
            L"- Nascondi automaticamente la barra delle applicazioni\r\n"
            L"- Usa icone piccole\r\n"
            L"- Configura la combinazione dei pulsanti della barra delle applicazioni\r\n"
            L"- Configura Aero Peek\r\n"
            L"- Accesso rapido alle impostazioni dell'area di notifica\r\n"
            L"\r\n"
            L"Limitazioni note:\r\n"
            L"\r\n"
            L"- La modifica della posizione della barra delle applicazioni non \u00e8 "
            L"attualmente disponibile.\r\n"
            L"- Personalizzazione del menu Start \u00e8 limitata.\r\n"
            L"- Le barre degli strumenti classiche hanno supporto limitato.\r\n"
            L"- Alcune impostazioni richiedono il riavvio di Explorer.");
        StringCchCopyW(g_str.warn_position_title, 64, L"Posizione barra delle applicazioni");
        StringCchCopyW(g_str.warn_position_text, 256, L"La modifica della posizione verr\u00e0 applicata al prossimo riavvio di Explorer.");
        StringCchCopyW(g_str.start_custom_title,  64, L"Personalizza menu Start");
        StringCchCopyW(g_str.start_grp_tiles,     64, L"Riquadri e comportamento");
        StringCchCopyW(g_str.start_chk_more_tiles,64, L"Mostra pi\u00f9 riquadri nel menu Start");
        StringCchCopyW(g_str.start_chk_app_list,  64, L"Mostra elenco app nel menu Start");
        StringCchCopyW(g_str.start_chk_recent_apps,64,L"Mostra app aggiunte di recente");
        StringCchCopyW(g_str.start_chk_fullscreen,64, L"Usa Start a schermo intero");
        StringCchCopyW(g_str.start_chk_recent_items,64,L"Mostra elementi recenti nelle Jump List");
        StringCchCopyW(g_str.start_chk_account_notif,64,L"Mostra notifiche account");
        StringCchCopyW(g_str.start_grp_search,    64, L"Ricerca");
        StringCchCopyW(g_str.start_chk_search_programs,64,L"Includi programmi nei risultati di ricerca");
        StringCchCopyW(g_str.start_chk_search_files,  64,L"Includi file nei risultati di ricerca");
        StringCchCopyW(g_str.start_grp_folders,   64, L"Cartelle da visualizzare in Start");
        StringCchCopyW(g_str.start_chk_folder_settings,32,L"Impostazioni");
        StringCchCopyW(g_str.start_chk_folder_docs,    32,L"Documenti");
        StringCchCopyW(g_str.start_chk_folder_downloads,32,L"Download");
        StringCchCopyW(g_str.start_chk_folder_music,  32,L"Musica");
        StringCchCopyW(g_str.start_chk_folder_pics,   32,L"Immagini");
        StringCchCopyW(g_str.start_chk_folder_videos, 32,L"Video");
        StringCchCopyW(g_str.start_chk_folder_network,32,L"Rete");
        StringCchCopyW(g_str.start_chk_folder_personal,32,L"Cartella personale");
        StringCchCopyW(g_str.start_info_restart,  256, L"Nota: alcune modifiche potrebbero richiedere il riavvio di Explorer per essere applicate.");
        StringCchCopyW(g_str.start_msg_saved,     512, L"Impostazioni del menu Start salvate.\n\nAlcune modifiche potrebbero richiedere il logout o il riavvio di Explorer.");
        StringCchCopyW(g_str.start_msg_saved_title,64, L"Impostazioni salvate");
    } else {
        StringCchCopyW(g_str.title,              128, L"Taskbar and Start Menu Properties");
        StringCchCopyW(g_str.tab_taskbar,         64, L"Taskbar");
        StringCchCopyW(g_str.tab_start,           64, L"Start Menu");
        StringCchCopyW(g_str.tab_toolbars,        64, L"Toolbars");
        StringCchCopyW(g_str.btn_ok,              32, L"OK");
        StringCchCopyW(g_str.btn_cancel,          32, L"Cancel");
        StringCchCopyW(g_str.btn_apply,           32, L"Apply");
        StringCchCopyW(g_str.grp_appearance,      64, L"Taskbar appearance");
        StringCchCopyW(g_str.chk_lock,            64, L"Lock the taskbar");
        StringCchCopyW(g_str.chk_hide,            64, L"Auto-hide the taskbar");
        StringCchCopyW(g_str.chk_small,           64, L"Use small icons");
        StringCchCopyW(g_str.txt_location,        64, L"Taskbar location:");
        StringCchCopyW(g_str.txt_buttons,         64, L"Taskbar buttons:");
        StringCchCopyW(g_str.grp_notif,           64, L"Notification area");
        StringCchCopyW(g_str.txt_notif,          128, L"Customize which icons and notifications appear.");
        StringCchCopyW(g_str.btn_cust_notif,      32, L"Customize...");
        StringCchCopyW(g_str.grp_aero,            64, L"Preview desktop with Aero Peek");
        StringCchCopyW(g_str.txt_aero,           256, L"Temporarily view the desktop when you move your mouse to the Show desktop button.");
        StringCchCopyW(g_str.chk_aero,            64, L"Use Aero Peek to preview the desktop");
        StringCchCopyW(g_str.link_help,          128, L"<a>How do I customize the taskbar?</a>");
        StringCchCopyW(g_str.pos_left,            32, L"Left");
        StringCchCopyW(g_str.pos_top,             32, L"Top");
        StringCchCopyW(g_str.pos_right,           32, L"Right");
        StringCchCopyW(g_str.pos_bottom,          32, L"Bottom");
        StringCchCopyW(g_str.btn_always_combine,  64, L"Always combine, hide labels");
        StringCchCopyW(g_str.btn_combine_full,    64, L"Combine when taskbar is full");
        StringCchCopyW(g_str.btn_never_combine,   64, L"Never combine");
        StringCchCopyW(g_str.start_info,         256, L"To customize how links, icons, and menus look in the Start menu, click Customize.");
        StringCchCopyW(g_str.btn_start_cust,      32, L"Customize...");
        StringCchCopyW(g_str.txt_power_label,     64, L"Power button action:");
        StringCchCopyW(g_str.power_shutdown,      32, L"Shut down");
        StringCchCopyW(g_str.power_restart,       32, L"Restart");
        StringCchCopyW(g_str.power_sleep,         32, L"Sleep");
        StringCchCopyW(g_str.power_hibernate,     32, L"Hibernate");
        StringCchCopyW(g_str.power_logoff,        32, L"Log off");
        StringCchCopyW(g_str.power_lock,          32, L"Lock");
        StringCchCopyW(g_str.power_switchuser,    32, L"Switch user");
        StringCchCopyW(g_str.grp_privacy,         32, L"Privacy");
        StringCchCopyW(g_str.chk_mru_prog,       128, L"Store and display recently opened programs in the Start menu");
        StringCchCopyW(g_str.chk_mru_items,      128, L"Store and display recently opened items in the Start menu and the taskbar");
        StringCchCopyW(g_str.toolbars_info,      128, L"Select which toolbars to add to the taskbar.");
        StringCchCopyW(g_str.toolbar_address,     32, L"Address");
        StringCchCopyW(g_str.toolbar_links,       32, L"Links");
        StringCchCopyW(g_str.toolbar_tabletpc,    32, L"Tablet PC Input Panel");
        StringCchCopyW(g_str.toolbar_desktop,     32, L"Desktop");
        StringCchCopyW(g_str.about_title,         64, L"About this mod");
        StringCchCopyW(g_str.about_text,        4096,
            L"Classic Taskbar Properties\r\n\r\n"
            L"This Windhawk mod restores the classic "
            L"\"Taskbar and Start Menu Properties\" dialog "
            L"inspired by Windows 7.\r\n\r\n"
            L"Currently available features:\r\n"
            L"\r\n"
            L"- Lock the taskbar\r\n"
            L"- Auto-hide the taskbar\r\n"
            L"- Use small icons\r\n"
            L"- Configure taskbar button grouping\r\n"
            L"- Configure Aero Peek\r\n"
            L"- Quick access to notification area settings\r\n"
            L"\r\n"
            L"Known limitations:\r\n"
            L"\r\n"
            L"- Taskbar position change is unavailable.\r\n"
            L"- Start menu customization is limited.\r\n"
            L"- Classic toolbars have limited support.\r\n"
            L"- Some settings require Explorer restart.");
        StringCchCopyW(g_str.warn_position_title, 64, L"Taskbar position");
        StringCchCopyW(g_str.warn_position_text, 256, L"The taskbar position change will be applied after restarting Explorer.");
        StringCchCopyW(g_str.start_custom_title,  64, L"Customize Start Menu");
        StringCchCopyW(g_str.start_grp_tiles,     64, L"Tiles and behavior");
        StringCchCopyW(g_str.start_chk_more_tiles,64, L"Show more tiles on Start");
        StringCchCopyW(g_str.start_chk_app_list,  64, L"Show app list in Start menu");
        StringCchCopyW(g_str.start_chk_recent_apps,64,L"Show recently added apps");
        StringCchCopyW(g_str.start_chk_fullscreen,64, L"Use Start full screen");
        StringCchCopyW(g_str.start_chk_recent_items,64,L"Show recently opened items in Jump Lists");
        StringCchCopyW(g_str.start_chk_account_notif,64,L"Show account notifications");
        StringCchCopyW(g_str.start_grp_search,    64, L"Search");
        StringCchCopyW(g_str.start_chk_search_programs,64,L"Include programs in search results");
        StringCchCopyW(g_str.start_chk_search_files,  64,L"Include files in search results");
        StringCchCopyW(g_str.start_grp_folders,   64, L"Folders to show on Start");
        StringCchCopyW(g_str.start_chk_folder_settings,32,L"Settings");
        StringCchCopyW(g_str.start_chk_folder_docs,    32,L"Documents");
        StringCchCopyW(g_str.start_chk_folder_downloads,32,L"Downloads");
        StringCchCopyW(g_str.start_chk_folder_music,  32,L"Music");
        StringCchCopyW(g_str.start_chk_folder_pics,   32,L"Pictures");
        StringCchCopyW(g_str.start_chk_folder_videos, 32,L"Videos");
        StringCchCopyW(g_str.start_chk_folder_network,32,L"Network");
        StringCchCopyW(g_str.start_chk_folder_personal,32,L"Personal folder");
        StringCchCopyW(g_str.start_info_restart,  256, L"Note: some changes may require an Explorer restart or logout to take full effect.");
        StringCchCopyW(g_str.start_msg_saved,     512, L"Start menu settings saved.\n\nSome changes may require logout or Explorer restart to take full effect.");
        StringCchCopyW(g_str.start_msg_saved_title,64, L"Settings saved");
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
    TaskbarSettingsProvider::SetToolbarEnabled(L"Address",  addr);
    TaskbarSettingsProvider::SetToolbarEnabled(L"Links",    links);
    TaskbarSettingsProvider::SetToolbarEnabled(L"TabletPC", tablet);
    TaskbarSettingsProvider::SetToolbarEnabled(L"Desktop",  desk);
    SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Taskbar");
    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hTray) SendNotifyMessageW(hTray, WM_SETTINGCHANGE, 0, (LPARAM)L"Taskbar");
}

static void InitToolbarsList(HWND hList) {
    ListView_SetExtendedListViewStyle(hList, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
    LVCOLUMNW col = {};
    col.mask = LVCF_WIDTH | LVCF_FMT;
    col.fmt  = LVCFMT_LEFT;
    col.cx   = 230;
    ListView_InsertColumn(hList, 0, &col);
    const WCHAR* names[] = { g_str.toolbar_address, g_str.toolbar_links, g_str.toolbar_tabletpc, g_str.toolbar_desktop };
    const WCHAR* keys[]  = { L"Address", L"Links", L"TabletPC", L"Desktop" };
    for (int i = 0; i < 4; i++) {
        LVITEMW lvi = {};
        lvi.mask    = LVIF_TEXT;
        lvi.iItem   = i;
        lvi.pszText = (LPWSTR)names[i];
        ListView_InsertItem(hList, &lvi);
        ListView_SetCheckState(hList, i, TaskbarSettingsProvider::GetToolbarEnabled(keys[i]) ? TRUE : FALSE);
    }
}

static const int kTaskbarCtls[] = {
    IDC_GRP_APPEARANCE, IDC_CHK_LOCK, IDC_CHK_HIDE, IDC_CHK_SMALL,
    IDC_TXT_LOCATION, IDC_COMBO_LOCATION, IDC_TXT_BUTTONS, IDC_COMBO_BUTTONS,
    IDC_GRP_NOTIF, IDC_TXT_NOTIF, IDC_BTN_CUST_NOTIF,
    IDC_GRP_AERO, IDC_TXT_AERO, IDC_CHK_AEROPEEK, IDC_LINK_HELP, 0
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

static void SwitchTab(HWND hwnd, int tab) {
    g_currentTab = tab;
    ShowGroup(hwnd, kTaskbarCtls, tab == 0);
    ShowGroup(hwnd, kStartCtls,   tab == 1);
    ShowGroup(hwnd, kToolbarCtls, tab == 2);
}

static void BalanceTextAndCombo(HWND hwndDlg, int idStatic, int idCombo) {
    HWND hStatic = GetDlgItem(hwndDlg, idStatic);
    HWND hCombo  = GetDlgItem(hwndDlg, idCombo);
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
    GetWindowRect(hCombo,  &rcCombo);
    MapWindowPoints(NULL, hwndDlg, (LPPOINT)&rcStatic, 2);
    MapWindowPoints(NULL, hwndDlg, (LPPOINT)&rcCombo,  2);

    int maxRight       = rcCombo.right;
    int newStaticWidth = size.cx + 8;
    int newComboX      = rcStatic.left + newStaticWidth;
    int newComboWidth  = maxRight - newComboX;

    if (newComboWidth < 70) {
        newComboWidth  = 70;
        newStaticWidth = (maxRight - rcStatic.left) - newComboWidth;
        newComboX      = rcStatic.left + newStaticWidth;
    }
    SetWindowPos(hStatic, NULL, rcStatic.left, rcStatic.top, newStaticWidth, rcStatic.bottom - rcStatic.top, SWP_NOZORDER | SWP_NOACTIVATE);
    SetWindowPos(hCombo,  NULL, newComboX,     rcCombo.top,  newComboWidth,  rcCombo.bottom  - rcCombo.top,  SWP_NOZORDER | SWP_NOACTIVATE);
}

static void ApplySettings(HWND hwnd) {
    bool lock    = (SendDlgItemMessageW(hwnd, IDC_CHK_LOCK,     BM_GETCHECK, 0, 0) == BST_CHECKED);
    bool hide    = (SendDlgItemMessageW(hwnd, IDC_CHK_HIDE,     BM_GETCHECK, 0, 0) == BST_CHECKED);
    bool small_i = (SendDlgItemMessageW(hwnd, IDC_CHK_SMALL,    BM_GETCHECK, 0, 0) == BST_CHECKED);
    bool aero    = (SendDlgItemMessageW(hwnd, IDC_CHK_AEROPEEK, BM_GETCHECK, 0, 0) == BST_CHECKED);
    DWORD glom   = (DWORD)SendDlgItemMessageW(hwnd, IDC_COMBO_BUTTONS, CB_GETCURSEL, 0, 0);

    TaskbarSettingsProvider::SetLockState(lock);
    TaskbarSettingsProvider::SetSmallIcons(small_i);
    TaskbarSettingsProvider::SetGlomLevel(glom);
    TaskbarSettingsProvider::SetAeroPeekEnabled(aero);

    APPBARDATA abd = { sizeof(APPBARDATA) };
    abd.hWnd   = FindWindowW(L"Shell_TrayWnd", NULL);
    abd.lParam = hide ? ABS_AUTOHIDE : ABS_ALWAYSONTOP;
    SHAppBarMessage(ABM_SETSTATE, &abd);

    DWORD powerSel = (DWORD)SendDlgItemMessageW(hwnd, IDC_COMBO_POWER, CB_GETCURSEL, 0, 0);
    if (powerSel < 7) TaskbarSettingsProvider::SetPowerAction(kPowerValues[powerSel]);

    bool mruProg  = (SendDlgItemMessageW(hwnd, IDC_CHK_MRU_PROG,  BM_GETCHECK, 0, 0) == BST_CHECKED);
    bool mruItems = (SendDlgItemMessageW(hwnd, IDC_CHK_MRU_ITEMS, BM_GETCHECK, 0, 0) == BST_CHECKED);
    TaskbarSettingsProvider::SetStartMruProgs(mruProg);
    TaskbarSettingsProvider::SetStartMruItems(mruItems);

    HWND hList = GetDlgItem(hwnd, IDC_LST_TOOLBARS);
    if (hList) {
        bool addr   = (ListView_GetCheckState(hList, 0) != 0);
        bool links  = (ListView_GetCheckState(hList, 1) != 0);
        bool tablet = (ListView_GetCheckState(hList, 2) != 0);
        bool desk   = (ListView_GetCheckState(hList, 3) != 0);
        ApplyToolbars(addr, links, tablet, desk);
    }

    SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"TraySettings");
}

static void ShowAboutDialog(HWND parent) {
    MessageBoxW(parent, g_str.about_text, g_str.about_title, MB_OK | MB_ICONINFORMATION);
}

// -----------------------------------------------------------------------
// Start Menu Customize dialog
// -----------------------------------------------------------------------

static INT_PTR CALLBACK StartCustomDlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_INITDIALOG: {
        g_hwndStartCustom = hwnd;
        if (g_hStartBrushWindow) { DeleteObject(g_hStartBrushWindow); g_hStartBrushWindow = NULL; }
        g_hStartBrushWindow = CreateSolidBrush(GetSysColor(COLOR_WINDOW));

        HDC hdc = GetDC(hwnd);
        int ptPx = -MulDiv(9, GetDeviceCaps(hdc, LOGPIXELSY), 72);
        ReleaseDC(hwnd, hdc);
        if (g_hStartFontUi) { DeleteObject(g_hStartFontUi); g_hStartFontUi = NULL; }
        g_hStartFontUi = CreateFontW(ptPx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

        SetWindowTextW(hwnd, g_str.start_custom_title);

        const DialogControlBinding bindings[] = {
            { IDC_START_GRP_TILES,         g_str.start_grp_tiles            },
            { IDC_CHK_MORE_TILES,          g_str.start_chk_more_tiles       },
            { IDC_CHK_APP_LIST,            g_str.start_chk_app_list         },
            { IDC_CHK_RECENT_APPS,         g_str.start_chk_recent_apps      },
            { IDC_CHK_FULLSCREEN,          g_str.start_chk_fullscreen       },
            { IDC_CHK_RECENT_ITEMS,        g_str.start_chk_recent_items     },
            { IDC_CHK_ACCOUNT_NOTIF,       g_str.start_chk_account_notif    },
            { IDC_GRP_SEARCH,              g_str.start_grp_search           },
            { IDC_CHK_SEARCH_PROGRAMS,     g_str.start_chk_search_programs  },
            { IDC_CHK_SEARCH_FILES,        g_str.start_chk_search_files     },
            { IDC_START_GRP_FOLDERS,       g_str.start_grp_folders          },
            { IDC_CHK_FOLDER_SETTINGS,     g_str.start_chk_folder_settings  },
            { IDC_CHK_FOLDER_DOCS,         g_str.start_chk_folder_docs      },
            { IDC_CHK_FOLDER_DOWNLOADS,    g_str.start_chk_folder_downloads },
            { IDC_CHK_FOLDER_MUSIC,        g_str.start_chk_folder_music     },
            { IDC_CHK_FOLDER_PICS,         g_str.start_chk_folder_pics      },
            { IDC_CHK_FOLDER_VIDEOS,       g_str.start_chk_folder_videos    },
            { IDC_CHK_FOLDER_NETWORK,      g_str.start_chk_folder_network   },
            { IDC_CHK_FOLDER_PERSONAL,     g_str.start_chk_folder_personal  },
            { IDC_START_STATIC_INFO,       g_str.start_info_restart         },
            { IDC_START_BTN_APPLY,         g_str.btn_apply                  },
            { IDOK,                        g_str.btn_ok                     },
            { IDCANCEL,                    g_str.btn_cancel                 },
        };
        for (const auto& b : bindings)
            SetDlgItemTextW(hwnd, b.controlId, b.text);

        if (g_hStartFontUi) {
            SendMessageW(hwnd, WM_SETFONT, (WPARAM)g_hStartFontUi, TRUE);
            SetFontAllChildren(hwnd, g_hStartFontUi);
        }

        for (const auto& s : g_startSettings) {
            bool state  = GetStartSettingState(s);
            bool locked = s.policyValue && IsSettingLockedByPolicy(s.policyValue);
            HWND hCtrl  = GetDlgItem(hwnd, s.controlId);
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
        SetWindowPos(hwnd, NULL,
            (GetSystemMetrics(SM_CXSCREEN) - ww) / 2,
            (GetSystemMetrics(SM_CYSCREEN) - wh) / 2,
            0, 0, SWP_NOSIZE | SWP_NOZORDER);

        LONG style = GetWindowLongW(hwnd, GWL_STYLE);
        style &= ~WS_THICKFRAME; style &= ~WS_MAXIMIZEBOX;
        SetWindowLongW(hwnd, GWL_STYLE, style);
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
        return TRUE;
    }
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSTATIC: {
        HDC hdc = (HDC)wp;
        SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        SetBkMode(hdc, TRANSPARENT);
        return (INT_PTR)g_hStartBrushWindow;
    }
    case WM_CTLCOLORBTN: {
        HDC hdc = (HDC)wp;
        SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        SetBkMode(hdc, TRANSPARENT);
        return (INT_PTR)g_hStartBrushWindow;
    }
    case WM_COMMAND: {
        WORD id  = LOWORD(wp);
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
        if (g_hStartFontUi)     { DeleteObject(g_hStartFontUi);     g_hStartFontUi     = NULL; }
        if (g_hStartBrushWindow){ DeleteObject(g_hStartBrushWindow); g_hStartBrushWindow= NULL; }
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
    BYTE* p   = buf;
    auto align4 = [](BYTE*& ptr) { ptr = (BYTE*)(((UINT_PTR)ptr + 3) & ~3); };

    LPDLGTEMPLATEW pDlg = (LPDLGTEMPLATEW)p;
    pDlg->style          = DS_SETFONT | DS_MODALFRAME | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU;
    pDlg->dwExtendedStyle= 0;
    pDlg->cdit           = 23;
    pDlg->x              = 0; pDlg->y = 0;
    pDlg->cx             = DialogSizes::START_WIDTH;
    pDlg->cy             = DialogSizes::START_HEIGHT;
    p += sizeof(DLGTEMPLATE);
    *(WORD*)p = 0; p += 2;
    *(WORD*)p = 0; p += 2;
    StringCchCopyW((WCHAR*)p, 1, L""); p += 2;
    *(WORD*)p = 8; p += 2;
    StringCchCopyW((WCHAR*)p, 10, L"Segoe UI");
    p += (lstrlenW(L"Segoe UI") + 1) * 2;

    auto addCtrl = [&](DWORD style, DWORD exStyle, short x, short y, short cx, short cy,
                       WORD id, LPCWSTR cls, LPCWSTR cap) {
        align4(p);
        LPDLGITEMTEMPLATE pi = (LPDLGITEMTEMPLATE)p;
        pi->style           = WS_CHILD | WS_VISIBLE | style;
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
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 16,  226, 11, IDC_CHK_MORE_TILES,    L"Button", g_str.start_chk_more_tiles);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 29,  226, 11, IDC_CHK_APP_LIST,      L"Button", g_str.start_chk_app_list);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 42,  226, 11, IDC_CHK_RECENT_APPS,   L"Button", g_str.start_chk_recent_apps);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 55,  226, 11, IDC_CHK_FULLSCREEN,    L"Button", g_str.start_chk_fullscreen);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 68,  226, 11, IDC_CHK_RECENT_ITEMS,  L"Button", g_str.start_chk_recent_items);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 81,  226, 11, IDC_CHK_ACCOUNT_NOTIF, L"Button", g_str.start_chk_account_notif);

    addCtrl(BS_GROUPBOX, 0, 6, 109, 240, 44, IDC_GRP_SEARCH, L"Button", g_str.start_grp_search);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 121, 226, 11, IDC_CHK_SEARCH_PROGRAMS, L"Button", g_str.start_chk_search_programs);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 134, 226, 11, IDC_CHK_SEARCH_FILES,    L"Button", g_str.start_chk_search_files);

    addCtrl(BS_GROUPBOX, 0, 6, 157, 240, 107, IDC_START_GRP_FOLDERS, L"Button", g_str.start_grp_folders);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 169, 110, 11, IDC_CHK_FOLDER_SETTINGS,  L"Button", g_str.start_chk_folder_settings);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 182, 110, 11, IDC_CHK_FOLDER_DOCS,       L"Button", g_str.start_chk_folder_docs);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 195, 110, 11, IDC_CHK_FOLDER_DOWNLOADS,  L"Button", g_str.start_chk_folder_downloads);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 12, 208, 110, 11, IDC_CHK_FOLDER_MUSIC,      L"Button", g_str.start_chk_folder_music);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 130, 169, 110, 11, IDC_CHK_FOLDER_PICS,      L"Button", g_str.start_chk_folder_pics);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 130, 182, 110, 11, IDC_CHK_FOLDER_VIDEOS,    L"Button", g_str.start_chk_folder_videos);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 130, 195, 110, 11, IDC_CHK_FOLDER_NETWORK,   L"Button", g_str.start_chk_folder_network);
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 130, 208, 110, 11, IDC_CHK_FOLDER_PERSONAL,  L"Button", g_str.start_chk_folder_personal);

    addCtrl(SS_LEFT, 0, 8, 268, 236, 18, IDC_START_STATIC_INFO, L"Static", L"");
    addCtrl(BS_DEFPUSHBUTTON | WS_TABSTOP, 0,  54, 291, 60, 13, IDOK,               L"Button", g_str.btn_ok);
    addCtrl(BS_PUSHBUTTON | WS_TABSTOP,    0, 118, 291, 60, 13, IDCANCEL,           L"Button", g_str.btn_cancel);
    addCtrl(BS_PUSHBUTTON | WS_TABSTOP,    0, 182, 291, 60, 13, IDC_START_BTN_APPLY,L"Button", g_str.btn_apply);

    HWND hwnd = CreateDialogIndirectParamW(GetModuleHandleW(NULL),
                                           (LPDLGTEMPLATE)buf, parent,
                                           StartCustomDlgProc, 0);
    delete[] buf;

    if (hwnd) {
        ShowWindow(hwnd, SW_SHOWNORMAL);
        SetForegroundWindow(hwnd);
    } else {
        InterlockedExchange(&g_startCustomOpen, 0);
    }
}

// -----------------------------------------------------------------------
// Main dialog
// -----------------------------------------------------------------------

static INT_PTR CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_INITDIALOG: {
        HICON hIcon = GetSystemIcon(SIID_TASKBAR);
        if (hIcon) {
            SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
            SendMessageW(hwnd, WM_SETICON, ICON_BIG,   (LPARAM)hIcon);
        }
        g_hwndMain  = hwnd;
        g_currentTab = 0;

        if (g_hBrushBtnFace) { DeleteObject(g_hBrushBtnFace); g_hBrushBtnFace = NULL; }
        if (g_hBrushWindow)  { DeleteObject(g_hBrushWindow);  g_hBrushWindow  = NULL; }
        g_hBrushBtnFace = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
        g_hBrushWindow  = CreateSolidBrush(GetSysColor(COLOR_WINDOW));

        HDC hdc = GetDC(hwnd);
        int ptPx = -MulDiv(9, GetDeviceCaps(hdc, LOGPIXELSY), 72);
        ReleaseDC(hwnd, hdc);
        if (g_hFontUi) { DeleteObject(g_hFontUi); g_hFontUi = NULL; }
        g_hFontUi = CreateFontW(ptPx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

        SetWindowTextW(hwnd, g_str.title);

        HWND hTab = GetDlgItem(hwnd, IDC_TAB_MAIN);
        auto addTab = [&](int i, const WCHAR* s) {
            TCITEMW ti = { TCIF_TEXT, 0, 0, (LPWSTR)s };
            SendMessageW(hTab, TCM_INSERTITEMW, i, (LPARAM)&ti);
        };
        addTab(0, g_str.tab_taskbar);
        addTab(1, g_str.tab_start);
        addTab(2, g_str.tab_toolbars);

        const DialogControlBinding bindings[] = {
            { IDC_GRP_APPEARANCE,  g_str.grp_appearance  }, { IDC_CHK_LOCK,        g_str.chk_lock       },
            { IDC_CHK_HIDE,        g_str.chk_hide        }, { IDC_CHK_SMALL,       g_str.chk_small      },
            { IDC_TXT_LOCATION,    g_str.txt_location    }, { IDC_TXT_BUTTONS,     g_str.txt_buttons    },
            { IDC_GRP_NOTIF,       g_str.grp_notif       }, { IDC_TXT_NOTIF,       g_str.txt_notif      },
            { IDC_BTN_CUST_NOTIF,  g_str.btn_cust_notif  }, { IDC_GRP_AERO,        g_str.grp_aero       },
            { IDC_TXT_AERO,        g_str.txt_aero        }, { IDC_CHK_AEROPEEK,    g_str.chk_aero       },
            { IDC_LINK_HELP,       g_str.link_help       }, { IDC_TXT_START_INFO,  g_str.start_info     },
            { IDC_BTN_START_CUST,  g_str.btn_start_cust  }, { IDC_TXT_POWER_LABEL, g_str.txt_power_label},
            { IDC_GRP_PRIVACY,     g_str.grp_privacy     }, { IDC_CHK_MRU_PROG,    g_str.chk_mru_prog   },
            { IDC_CHK_MRU_ITEMS,   g_str.chk_mru_items   }, { IDC_TXT_TOOLBARS_INFO,g_str.toolbars_info },
            { IDOK,                g_str.btn_ok          }, { IDCANCEL,            g_str.btn_cancel     },
            { IDC_BTN_APPLY,       g_str.btn_apply       },
        };
        for (const auto& b : bindings) SetDlgItemTextW(hwnd, b.controlId, b.text);

        if (g_hFontUi) {
            SendMessageW(hwnd, WM_SETFONT, (WPARAM)g_hFontUi, TRUE);
            SetFontAllChildren(hwnd, g_hFontUi);
            UpdateWindow(hwnd);
        }

        BalanceTextAndCombo(hwnd, IDC_TXT_LOCATION,    IDC_COMBO_LOCATION);
        BalanceTextAndCombo(hwnd, IDC_TXT_BUTTONS,     IDC_COMBO_BUTTONS);
        BalanceTextAndCombo(hwnd, IDC_TXT_POWER_LABEL, IDC_COMBO_POWER);

        HWND hCL = GetDlgItem(hwnd, IDC_COMBO_LOCATION);
        SendMessageW(hCL, CB_ADDSTRING, 0, (LPARAM)g_str.pos_left);
        SendMessageW(hCL, CB_ADDSTRING, 0, (LPARAM)g_str.pos_top);
        SendMessageW(hCL, CB_ADDSTRING, 0, (LPARAM)g_str.pos_right);
        SendMessageW(hCL, CB_ADDSTRING, 0, (LPARAM)g_str.pos_bottom);

        HWND hCB = GetDlgItem(hwnd, IDC_COMBO_BUTTONS);
        SendMessageW(hCB, CB_ADDSTRING, 0, (LPARAM)g_str.btn_always_combine);
        SendMessageW(hCB, CB_ADDSTRING, 0, (LPARAM)g_str.btn_combine_full);
        SendMessageW(hCB, CB_ADDSTRING, 0, (LPARAM)g_str.btn_never_combine);

        HWND hCP = GetDlgItem(hwnd, IDC_COMBO_POWER);
        SendMessageW(hCP, CB_ADDSTRING, 0, (LPARAM)g_str.power_shutdown);
        SendMessageW(hCP, CB_ADDSTRING, 0, (LPARAM)g_str.power_restart);
        SendMessageW(hCP, CB_ADDSTRING, 0, (LPARAM)g_str.power_sleep);
        SendMessageW(hCP, CB_ADDSTRING, 0, (LPARAM)g_str.power_hibernate);
        SendMessageW(hCP, CB_ADDSTRING, 0, (LPARAM)g_str.power_logoff);
        SendMessageW(hCP, CB_ADDSTRING, 0, (LPARAM)g_str.power_lock);
        SendMessageW(hCP, CB_ADDSTRING, 0, (LPARAM)g_str.power_switchuser);

        DWORD szMove  = TaskbarSettingsProvider::RegGetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarSizeMove", 1);
        DWORD szSmall = TaskbarSettingsProvider::RegGetDWordSafe(HKEY_CURRENT_USER, kAdvKey, L"TaskbarSmallIcons", 0);
        DWORD glom    = TaskbarSettingsProvider::GetGlomLevel();
        DWORD edge    = TaskbarSettingsProvider::GetTaskbarEdge();
        bool  aeroPeek= TaskbarSettingsProvider::GetAeroPeekEnabled();

        APPBARDATA abd = { sizeof(APPBARDATA) };
        abd.hWnd = FindWindowW(L"Shell_TrayWnd", NULL);
        bool isHide = (SHAppBarMessage(ABM_GETSTATE, &abd) & ABS_AUTOHIDE) != 0;

        SendDlgItemMessageW(hwnd, IDC_CHK_LOCK,     BM_SETCHECK, (szMove == 0) ? BST_CHECKED : BST_UNCHECKED, 0);
        SendDlgItemMessageW(hwnd, IDC_CHK_HIDE,     BM_SETCHECK, isHide        ? BST_CHECKED : BST_UNCHECKED, 0);
        SendDlgItemMessageW(hwnd, IDC_CHK_SMALL,    BM_SETCHECK, (szSmall != 0)? BST_CHECKED : BST_UNCHECKED, 0);
        SendDlgItemMessageW(hwnd, IDC_CHK_AEROPEEK, BM_SETCHECK, aeroPeek      ? BST_CHECKED : BST_UNCHECKED, 0);
        SendMessageW(hCL, CB_SETCURSEL, (edge < 4) ? (WPARAM)edge : 3, 0);
        SendMessageW(hCB, CB_SETCURSEL, (glom < 3) ? (WPARAM)glom : 0, 0);

        DWORD curPower = TaskbarSettingsProvider::GetPowerAction();
        int powerIdx   = 0;
        for (int i = 0; i < 7; i++) { if (kPowerValues[i] == curPower) { powerIdx = i; break; } }
        SendMessageW(hCP, CB_SETCURSEL, powerIdx, 0);

        SendDlgItemMessageW(hwnd, IDC_CHK_MRU_PROG,  BM_SETCHECK, TaskbarSettingsProvider::GetStartMruProgs() ? BST_CHECKED : BST_UNCHECKED, 0);
        SendDlgItemMessageW(hwnd, IDC_CHK_MRU_ITEMS, BM_SETCHECK, TaskbarSettingsProvider::GetStartMruItems() ? BST_CHECKED : BST_UNCHECKED, 0);

        InitToolbarsList(GetDlgItem(hwnd, IDC_LST_TOOLBARS));
        EnableWindow(GetDlgItem(hwnd, IDC_BTN_APPLY), FALSE);
        SwitchTab(hwnd, 0);

        RECT rc; GetWindowRect(hwnd, &rc);
        int ww = rc.right - rc.left, wh = rc.bottom - rc.top;
        SetWindowPos(hwnd, NULL,
            (GetSystemMetrics(SM_CXSCREEN) - ww) / 2,
            (GetSystemMetrics(SM_CYSCREEN) - wh) / 2,
            0, 0, SWP_NOSIZE | SWP_NOZORDER);

        LONG style = GetWindowLongW(hwnd, GWL_STYLE);
        style &= ~WS_THICKFRAME; style &= ~WS_MAXIMIZEBOX;
        SetWindowLongW(hwnd, GWL_STYLE, style);
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
        return TRUE;
    }
    case WM_GETMINMAXINFO: {
        MINMAXINFO* mmi = (MINMAXINFO*)lp; RECT rc; GetWindowRect(hwnd, &rc);
        mmi->ptMinTrackSize.x = mmi->ptMaxTrackSize.x = rc.right - rc.left;
        mmi->ptMinTrackSize.y = mmi->ptMaxTrackSize.y = rc.bottom - rc.top;
        return 0;
    }
    case WM_CTLCOLORDLG: {
        SetBkColor((HDC)wp, GetSysColor(COLOR_WINDOW));
        return (INT_PTR)g_hBrushWindow;
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdc = (HDC)wp;
        SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        SetBkMode(hdc, TRANSPARENT);
        return (INT_PTR)g_hBrushWindow;
    }
    case WM_CTLCOLORBTN: {
        HDC hdc = (HDC)wp;
        SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        SetBkMode(hdc, TRANSPARENT);
        return (INT_PTR)g_hBrushWindow;
    }
    case WM_COMMAND: {
        WORD id  = LOWORD(wp);
        WORD act = HIWORD(wp);
        if ((act == BN_CLICKED || act == CBN_SELCHANGE)
            && id != IDOK && id != IDCANCEL && id != IDC_BTN_APPLY
            && id != IDC_BTN_CUST_NOTIF && id != IDC_BTN_START_CUST)
            EnableWindow(GetDlgItem(hwnd, IDC_BTN_APPLY), TRUE);

        if      (id == IDOK)              { ApplySettings(hwnd); if (IsWindow(hwnd)) DestroyWindow(hwnd); }
        else if (id == IDCANCEL)          { DestroyWindow(hwnd); }
        else if (id == IDC_BTN_APPLY)     { ApplySettings(hwnd); if (IsWindow(hwnd)) EnableWindow(GetDlgItem(hwnd, IDC_BTN_APPLY), FALSE); }
        else if (id == IDC_BTN_CUST_NOTIF){ ShellExecuteW(hwnd, L"open", L"shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}", NULL, NULL, SW_SHOW); }
        else if (id == IDC_BTN_START_CUST){ ShowStartCustomDialog(hwnd); }
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
            if ((nmlv->uChanged & LVIF_STATE) && ((nmlv->uNewState ^ nmlv->uOldState) & LVIS_STATEIMAGEMASK))
                EnableWindow(GetDlgItem(hwnd, IDC_BTN_APPLY), TRUE);
        }
        if (hdr->idFrom == IDC_LINK_HELP && hdr->code == NM_CLICK)
            ShowAboutDialog(hwnd);
        break;
    }
    case WM_DESTROY:
        if (g_hFontUi)      { DeleteObject(g_hFontUi);      g_hFontUi      = NULL; }
        if (g_hBrushBtnFace){ DeleteObject(g_hBrushBtnFace); g_hBrushBtnFace= NULL; }
        if (g_hBrushWindow) { DeleteObject(g_hBrushWindow);  g_hBrushWindow = NULL; }
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
    BYTE* buf = new BYTE[4096];
    BYTE* p   = buf;
    auto align4 = [](BYTE*& ptr) { ptr = (BYTE*)(((UINT_PTR)ptr + 3) & ~3); };

    LPDLGTEMPLATEW pDlg  = (LPDLGTEMPLATEW)p;
    pDlg->style          = DS_SETFONT | DS_MODALFRAME | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU;
    pDlg->dwExtendedStyle= 0;
    pDlg->cdit           = 28;
    pDlg->x              = 0; pDlg->y = 0;
    pDlg->cx             = DialogSizes::MAIN_WIDTH;
    pDlg->cy             = DialogSizes::MAIN_HEIGHT;
    p += sizeof(DLGTEMPLATE);
    *(WORD*)p = 0; p += 2;
    *(WORD*)p = 0; p += 2;
    StringCchCopyW((WCHAR*)p, 1, L""); p += 2;
    *(WORD*)p = 9; p += 2;
    StringCchCopyW((WCHAR*)p, 10, L"Segoe UI");
    p += (lstrlenW(L"Segoe UI") + 1) * 2;

    auto addCtrl = [&](DWORD style, DWORD exStyle, short x, short y, short cx, short cy,
                       WORD id, LPCWSTR cls, LPCWSTR cap) {
        align4(p);
        LPDLGITEMTEMPLATE pi = (LPDLGITEMTEMPLATE)p;
        pi->style            = WS_CHILD | WS_VISIBLE | style;
        pi->dwExtendedStyle  = exStyle;
        pi->x = x; pi->y = y; pi->cx = cx; pi->cy = cy; pi->id = id;
        p += sizeof(DLGITEMTEMPLATE);
        StringCchCopyW((WCHAR*)p, lstrlenW(cls) + 1, cls);
        p += (lstrlenW(cls) + 1) * 2;
        StringCchCopyW((WCHAR*)p, lstrlenW(cap) + 1, cap);
        p += (lstrlenW(cap) + 1) * 2;
        *(WORD*)p = 0; p += 2;
    };

    addCtrl(TCS_TABS | WS_TABSTOP, 0, 6, 6, 250, 246, IDC_TAB_MAIN, L"SysTabControl32", L"");
    addCtrl(BS_GROUPBOX, 0, 12, 22, 238, 96, IDC_GRP_APPEARANCE, L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 18, 33, 226, 10, IDC_CHK_LOCK,  L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 18, 45, 226, 10, IDC_CHK_HIDE,  L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 18, 57, 226, 10, IDC_CHK_SMALL, L"Button", L"");
    addCtrl(SS_LEFT | SS_EDITCONTROL, 0, 18,  73, 110, 10, IDC_TXT_LOCATION, L"Static", L"");
    addCtrl(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 0, 132, 71, 112, 100, IDC_COMBO_LOCATION, L"ComboBox", L"");
    addCtrl(SS_LEFT | SS_EDITCONTROL, 0, 18,  89,  95, 18, IDC_TXT_BUTTONS,  L"Static",   L"");
    addCtrl(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 0, 80, 87, 164, 70,  IDC_COMBO_BUTTONS,  L"ComboBox",  L"");
    addCtrl(BS_GROUPBOX, 0, 12, 120, 238, 46, IDC_GRP_NOTIF, L"Button", L"");
    addCtrl(SS_LEFT, 0, 18, 131, 148, 26, IDC_TXT_NOTIF, L"Static", L"");
    addCtrl(BS_PUSHBUTTON | WS_TABSTOP, 0, 172, 134, 70, 14, IDC_BTN_CUST_NOTIF, L"Button", L"");
    addCtrl(BS_GROUPBOX, 0, 12, 168, 238, 66, IDC_GRP_AERO, L"Button", L"");
    addCtrl(SS_LEFT, 0, 18, 179, 226, 30, IDC_TXT_AERO, L"Static", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 18, 213, 226, 12, IDC_CHK_AEROPEEK, L"Button", L"");
    addCtrl(WS_TABSTOP, 0, 12, 238, 238, 10, IDC_LINK_HELP, L"SysLink", L"");
    addCtrl(BS_DEFPUSHBUTTON | WS_TABSTOP, 0,  84, 271, 52, 14, IDOK,         L"Button", L"OK");
    addCtrl(BS_PUSHBUTTON | WS_TABSTOP,    0, 140, 271, 52, 14, IDCANCEL,     L"Button", L"Cancel");
    addCtrl(BS_PUSHBUTTON | WS_TABSTOP,    0, 196, 271, 58, 14, IDC_BTN_APPLY,L"Button", L"Apply");
    addCtrl(SS_LEFT, 0, 14,  22, 162, 26, IDC_TXT_START_INFO,  L"Static",   L"");
    addCtrl(BS_PUSHBUTTON | WS_TABSTOP, 0, 180, 22, 66, 14, IDC_BTN_START_CUST, L"Button", L"");
    addCtrl(SS_LEFT, 0, 14, 56, 100, 10, IDC_TXT_POWER_LABEL, L"Static", L"");
    addCtrl(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 0, 116, 54, 130, 80, IDC_COMBO_POWER, L"ComboBox", L"");
    addCtrl(BS_GROUPBOX, 0, 12, 74, 238, 62, IDC_GRP_PRIVACY, L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP | BS_MULTILINE, 0, 18,  86, 226, 18, IDC_CHK_MRU_PROG,  L"Button", L"");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP | BS_MULTILINE, 0, 18, 108, 226, 18, IDC_CHK_MRU_ITEMS, L"Button", L"");
    addCtrl(SS_LEFT, 0, 14, 22, 234, 18, IDC_TXT_TOOLBARS_INFO, L"Static", L"");
    addCtrl(LVS_REPORT | LVS_NOCOLUMNHEADER | LVS_SINGLESEL | WS_BORDER | WS_TABSTOP,
            0, 16, 44, 230, 165, IDC_LST_TOOLBARS, L"SysListView32", L"");

    HWND hwnd = CreateDialogIndirectParamW(GetModuleHandleW(NULL),
                                           (LPDLGTEMPLATE)buf, NULL, DlgProc, 0);
    delete[] buf;
    return hwnd;
}

static DWORD WINAPI DialogThreadProc(LPVOID) {
    INITCOMMONCONTROLSEX icex = { sizeof(icex),
        ICC_TAB_CLASSES | ICC_LINK_CLASS | ICC_WIN95_CLASSES | ICC_LISTVIEW_CLASSES };
    InitCommonControlsEx(&icex);

    HWND hwnd = BuildAndShowDialog();
    if (!hwnd) { InterlockedExchange(&g_dialogOpen, 0); return 1; }
    ShowWindow(hwnd, SW_SHOWNORMAL);
    SetForegroundWindow(hwnd);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        if (!IsDialogMessageW(hwnd, &msg)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
        if (!IsWindow(hwnd)) break;
    }
    g_hwndMain = NULL;
    InterlockedExchange(&g_dialogOpen, 0);
    return 0;
}

static void ShowTaskbarProperties() {
    if (InterlockedExchange(&g_dialogOpen, 1)) {
        HWND hw = g_hwndMain;
        if (hw && IsWindow(hw)) { SetForegroundWindow(hw); if (IsIconic(hw)) ShowWindow(hw, SW_RESTORE); }
        return;
    }
    if (g_dialogThread) {
        WaitForSingleObject(g_dialogThread, INFINITE);
        CloseHandle(g_dialogThread);
        g_dialogThread = NULL;
    }
    g_dialogThread = CreateThread(NULL, 0, DialogThreadProc, NULL, 0, NULL);
}

// Hook: ShellExecuteExW
BOOL WINAPI ShellExecuteExW_hook(SHELLEXECUTEINFOW* pei) {
    if (IsChildProcess()) return ShellExecuteExW_orig(pei);
    HookGuard guard;
    if (guard.IsReentrant()) return ShellExecuteExW_orig(pei);
    
    if (pei && pei->lpFile) {
        if (_wcsnicmp(pei->lpFile, L"ms-settings:taskbar", 19) == 0) {
            Wh_Log(L"Classic Taskbar Properties: ShellExecuteExW intercepted ms-settings:taskbar");
            ShowTaskbarProperties();
            if (pei->fMask & SEE_MASK_NOCLOSEPROCESS) pei->hProcess = NULL;
            pei->hInstApp = SHELL_EXECUTE_SUCCESS;
            return TRUE;
        }
    }
    return ShellExecuteExW_orig(pei);
}

// Hook: ShellExecuteW
HINSTANCE WINAPI ShellExecuteW_hook(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile,
                                     LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd) {
    if (IsChildProcess()) return ShellExecuteW_orig(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
    HookGuard guard;
    if (guard.IsReentrant()) return ShellExecuteW_orig(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
    
    if (lpFile && _wcsnicmp(lpFile, L"ms-settings:taskbar", 19) == 0) {
        Wh_Log(L"Classic Taskbar Properties: ShellExecuteW intercepted ms-settings:taskbar");
        ShowTaskbarProperties();
        return SHELL_EXECUTE_SUCCESS;
    }
    return ShellExecuteW_orig(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Classic Taskbar Properties v2.0.2 initializing...");
    
    InterlockedExchange(&g_dialogOpen,      0);
    InterlockedExchange(&g_startCustomOpen, 0);
    
    BuildChildEnvironment();
    
    LoadLanguageSetting();
    InitLocalization();

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

    Wh_Log(L"Classic Taskbar Properties initialized successfully");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Classic Taskbar Properties unloading...");
    
    if (g_hwndMain       && IsWindow(g_hwndMain))        PostMessageW(g_hwndMain,        WM_CLOSE, 0, 0);
    if (g_hwndStartCustom&& IsWindow(g_hwndStartCustom)) PostMessageW(g_hwndStartCustom, WM_CLOSE, 0, 0);
    if (g_dialogThread) {
        WaitForSingleObject(g_dialogThread, 5000);
        CloseHandle(g_dialogThread);
        g_dialogThread = NULL;
    }
    if (g_hFontUi)          { DeleteObject(g_hFontUi);          g_hFontUi          = NULL; }
    if (g_hStartFontUi)     { DeleteObject(g_hStartFontUi);     g_hStartFontUi     = NULL; }
    if (g_hBrushWindow)     { DeleteObject(g_hBrushWindow);     g_hBrushWindow     = NULL; }
    if (g_hBrushBtnFace)    { DeleteObject(g_hBrushBtnFace);    g_hBrushBtnFace    = NULL; }
    if (g_hStartBrushWindow){ DeleteObject(g_hStartBrushWindow);g_hStartBrushWindow= NULL; }
    
    Wh_Log(L"Classic Taskbar Properties unloaded");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading...");
    LoadLanguageSetting();
    InitLocalization();
    
    if (g_hwndMain && IsWindow(g_hwndMain)) {
        PostMessageW(g_hwndMain, WM_CLOSE, 0, 0);
    }
}
