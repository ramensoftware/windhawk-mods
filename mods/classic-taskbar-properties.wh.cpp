// ==WindhawkMod==
// @id classic-taskbar-properties
// @name Classic Taskbar and Start Menu Properties
// @description This mod recreates the classic "Taskbar and Start Menu Properties" dialog from Windows 7 (Taskbar, Start Menu, Toolbars tabs) in Windows 10 and 11.
// @version 2.9.1
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
- Address/Links/Tablet PC toolbars (Desktop toolbar is currently not functional)
- Vertical taskbar support: only **Top** and **Bottom** positions are functional.
  **Left** and **Right** are shown in the position combo box for reference but are
  currently disabled (greyed out) and cannot be selected, due to the added complexity
  of fully supporting those two edges.

## What This Mod Tries To Do
- Start Menu customization (where supported by the OS)

## What This Mod Does NOT Do
- It cannot guarantee 100% original behavior from older Windows versions

## Known Issues
- **Left/Right taskbar position**: currently disabled (greyed out, non-selectable) in the
  position combo box. Only Top and Bottom rotation is supported for now; Left/Right requires
  more work due to the added complexity and may be added in a future version.
- **Desktop toolbar**: Currently not available, more documentation is required for a proper implementation.
- **Some Start Menu settings**: May not work on all Windows versions
- **Tablet Input PC**: Works partially
- **Clock text layout when vertical**: the clock box is resized/stacked correctly, but its
  internal text re-wrap (two lines) is handled by undocumented Explorer code and may look
  slightly off; it will not overflow off-screen anymore.
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
        // UNICA fonte di verita': StuckRects3\Settings (byte offset 12),
        // la stessa usata realmente da Explorer.
        DWORD edge = 3; // default: bottom
        HKEY hk;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, kStuckKey, 0, KEY_READ, &hk) == ERROR_SUCCESS) {
            DWORD sz = 0;
            RegQueryValueExW(hk, L"Settings", NULL, NULL, NULL, &sz);
            if (sz >= StuckRects::SETTINGS_EDGE_OFFSET + sizeof(DWORD)) {
                std::vector<BYTE> d(sz);
                if (RegQueryValueExW(hk, L"Settings", NULL, NULL, d.data(), &sz) == ERROR_SUCCESS) {
                    edge = *reinterpret_cast<DWORD*>(&d[StuckRects::SETTINGS_EDGE_OFFSET]);
                    if (edge > 3) edge = 3;
                }
            }
            RegCloseKey(hk);
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

// ============================================================================
// DEFINIZIONI PER ADDRESS/LINKS TOOLBAR (meccanismo legacy)
// ============================================================================
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
    HRESULT hrInit = OleInitialize(NULL);
    bool needUninit = SUCCEEDED(hrInit);
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
            break;
        }
        case CTP_BAND_HIDE: {
            HRESULT r = pTray->HideDeskBand(band);
            ok = SUCCEEDED(r);
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
    }
    if (needUninit) OleUninitialize();
    return ok;
}

static void RefreshNativeDeskBandRegistration() {
    HRESULT hrInit = OleInitialize(NULL);
    bool needUninit = SUCCEEDED(hrInit);
    CTP_ITrayDeskBand* pTray = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_CTP_TrayDeskBand, NULL,
        CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
        IID_CTP_ITrayDeskBand, (void**)&pTray);
    if (SUCCEEDED(hr) && pTray) {
        pTray->DeskBandRegistrationChanged();
        pTray->Release();
    }
    if (needUninit) OleUninitialize();
}

static void SetFontAllChildren(HWND hwnd, HFONT hf) {
    EnumChildWindows(hwnd, SetFontChildProc, (LPARAM)hf);
}

// ============================================================================
// GESTIONE DESKTOP TOOLBAR - SHLoadInProc + registry fallback
// ============================================================================
static const CLSID CLSID_DesktopBand =
    {0xD82BE2B0,0x5764,0x11D0,{0xA9,0x6E,0x00,0xC0,0x4F,0xD7,0x05,0xA2}};

static bool IsNativeDesktopToolbarShown() {
    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!hTray) return false;

    HWND hReBar = FindWindowExW(hTray, NULL, L"ReBarWindow32", NULL);
    if (hReBar) {
        HWND hDesktopBand = FindWindowExW(hReBar, NULL, L"ToolbarWindow32", L"Desktop");
        if (hDesktopBand) return true;
    }

    HWND hDesktopBand = FindWindowExW(hTray, NULL, L"ToolbarWindow32", L"Desktop");
    return (hDesktopBand != NULL);
}

static bool ShowDesktopToolbarViaSHLoadInProc() {
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) return false;

    auto pSHLoadInProc = (HRESULT(WINAPI*)(REFCLSID))GetProcAddress(hShell32, "SHLoadInProc");
    if (!pSHLoadInProc) {
        Wh_Log(L"[DesktopToolbar] SHLoadInProc not found in shell32.dll");
        return false;
    }

    Wh_Log(L"[DesktopToolbar] Calling SHLoadInProc for Desktop band...");
    HRESULT hr = pSHLoadInProc(CLSID_DesktopBand);
    Wh_Log(L"[DesktopToolbar] SHLoadInProc returned 0x%08X", hr);

    if (SUCCEEDED(hr)) {
        HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
        if (hTray) {
            SendNotifyMessageW(hTray, WM_SETTINGCHANGE, 0, (LPARAM)L"Taskbar");
        }
        SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Taskbar");
        return true;
    }

    return false;
}

static bool ShowNativeDesktopToolbar() {
    Wh_Log(L"[DesktopToolbar] Show requested");

    if (ShowDesktopToolbarViaSHLoadInProc()) {
        TaskbarSettingsProvider::SetToolbarEnabled(L"Desktop", true);
        Wh_Log(L"[DesktopToolbar] Show successful via SHLoadInProc");
        return true;
    }

    Wh_Log(L"[DesktopToolbar] SHLoadInProc failed, using registry fallback");
    TaskbarSettingsProvider::SetToolbarEnabled(L"Desktop", true);

    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hTray) {
        SendNotifyMessageW(hTray, WM_SETTINGCHANGE, 0, (LPARAM)L"Taskbar");
    }
    SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Taskbar");
    return true;
}

static bool HideNativeDesktopToolbar() {
    Wh_Log(L"[DesktopToolbar] Hide requested");

    TaskbarSettingsProvider::SetToolbarEnabled(L"Desktop", false);

    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hTray) {
        SendNotifyMessageW(hTray, WM_SETTINGCHANGE, 0, (LPARAM)L"Taskbar");
    }
    SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Taskbar");
    return true;
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
    StringCchCopyW(g_str.btn_ok, 32, L"OK");
    StringCchCopyW(g_str.btn_cancel, 32, useItalian ? L"Annulla" : L"Cancel");
    StringCchCopyW(g_str.btn_apply, 32, useItalian ? L"Applica" : L"Apply");
    StringCchCopyW(g_str.grp_appearance, 64, useItalian ? L"Aspetto della barra delle applicazioni" : L"Taskbar appearance");
    StringCchCopyW(g_str.chk_lock, 64, useItalian ? L"Blocca la barra delle applicazioni" : L"Lock the taskbar");
    StringCchCopyW(g_str.chk_hide, 64, useItalian ? L"Nascondi automaticamente la barra delle applicazioni" : L"Auto-hide the taskbar");
    StringCchCopyW(g_str.chk_small, 64, useItalian ? L"Usa icone piccole" : L"Use small icons");
    StringCchCopyW(g_str.txt_location, 64, useItalian ? L"Posizione sullo schermo:" : L"Taskbar location:");
    StringCchCopyW(g_str.txt_buttons, 64, useItalian ? L"Pulsanti della barra\ndelle applicazioni:" : L"Taskbar buttons:");
    StringCchCopyW(g_str.grp_notif, 64, useItalian ? L"Area di notifica" : L"Notification area");
    StringCchCopyW(g_str.txt_notif, 128, useItalian ? L"Consente di personalizzare le icone e le notifiche nell'area di notifica." : L"Customize which icons and notifications appear.");
    StringCchCopyW(g_str.btn_cust_notif, 32, useItalian ? L"Personalizza..." : L"Customize...");
    StringCchCopyW(g_str.grp_aero, 64, useItalian ? L"Anteprima del desktop con Aero Peek" : L"Preview desktop with Aero Peek");
    StringCchCopyW(g_str.chk_aero, 64, useItalian ? L"Usa Aero Peek per visualizzare l'anteprima del desktop" : L"Use Aero Peek to preview the desktop.");
    StringCchCopyW(g_str.txt_aero, 256, useItalian ? L"Consente di visualizzare temporaneamente il desktop portando il puntatore sul pulsante Mostra desktop alla fine della barra delle applicazioni." : L"Temporarily view the desktop when you move your mouse to the Show desktop button.");
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
    // Desktop Toolbar momentaneamente non disponibile - verrà ripristinata in futuro
    // quando l'implementazione sarà completata e testata adeguatamente.
    StringCchCopyW(g_str.toolbar_desktop, 32, useItalian ? L"Desktop" : L"Desktop");

    if (useItalian) {
        StringCchCopyW(g_str.about_title, 64, L"Informazioni sulla mod");
        StringCchCopyW(g_str.about_text, 4096,
            L"Classic Taskbar Properties\r\n\r\n"
            L"Questa mod per Windhawk ricrea la classica finestra \"Propriet\u00e0 della barra delle applicazioni e del menu Start\" ispirata alle versioni classiche di Windows.\r\n\r\n"
            L"Funzionalit\u00e0 attualmente disponibili:\r\n"
            L"- Blocca la barra delle applicazioni\r\n"
            L"- Nascondi automaticamente la barra delle applicazioni\r\n"
            L"- Usa icone piccole\r\n"
            L"- Configura la combinazione dei pulsanti della barra delle applicazioni\r\n"
            L"- Configura Aero Peek\r\n"
            L"- Accesso rapido alle impostazioni dell'area di notifica\r\n"
            L"- Barre degli strumenti native (Indirizzo, Collegamenti, Tablet PC)\r\n"
            L"- Rotazione barra delle applicazioni (solo Alto/Basso), incluse icone e area di notifica\r\n\r\n"
            L"Limitazioni note:\r\n"
            L"- Le posizioni Sinistra e Destra sono state rimosse per complessit\u00e0 implementativa.\r\n"
            L"- La toolbar Desktop non \u00e8 attualmente disponibile.\r\n"
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
            L"This Windhawk mod recreates the classic \"Taskbar and Start Menu Properties\" dialog inspired by classic Windows versions.\r\n\r\n"
            L"Currently available features:\r\n"
            L"- Lock the taskbar\r\n"
            L"- Auto-hide the taskbar\r\n"
            L"- Use small icons\r\n"
            L"- Configure taskbar button grouping\r\n"
            L"- Configure Aero Peek\r\n"
            L"- Quick access to notification area settings\r\n"
            L"- Native toolbars (Address, Links, Tablet PC)\r\n"
            L"- Taskbar rotation (Top/Bottom only), including icons and notification area\r\n\r\n"
            L"Known limitations:\r\n"
            L"- Left and Right positions have been removed due to implementation complexity.\r\n"
            L"- Desktop toolbar is not currently available.\r\n"
            L"- Some settings require Explorer restart.");
        StringCchCopyW(g_str.warn_position_title, 64, L"Taskbar position");
        StringCchCopyW(g_str.warn_position_text, 256, L"The taskbar position change will be applied after restarting Explorer.");
        StringCchCopyW(g_str.start_custom_title, 64, L"Customize Start Menu");
        StringCchCopyW(g_str.start_grp_tiles, 64, L"Tiles and behavior");
        StringCchCopyW(g_str.start_chk_more_tiles, 64, L"Show more tiles on Start");
    }

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

    NativeDeskBandOp(CLSID_CTP_AddressBand, addr ? CTP_BAND_SHOW : CTP_BAND_HIDE, NULL);
    TaskbarSettingsProvider::SetToolbarEnabled(L"Address", addr);

    NativeDeskBandOp(CLSID_CTP_LinksBand, links ? CTP_BAND_SHOW : CTP_BAND_HIDE, NULL);
    TaskbarSettingsProvider::SetToolbarEnabled(L"Links", links);

    if (desk) ShowNativeDesktopToolbar();
    else      HideNativeDesktopToolbar();
    TaskbarSettingsProvider::SetToolbarEnabled(L"Desktop", desk);

    TaskbarSettingsProvider::SetToolbarEnabled(L"TabletPC", tablet);

    SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Taskbar");
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

        if (i == 2) { // TabletPC
            shown = TaskbarSettingsProvider::GetToolbarEnabled(keys[i]);
            got = true;
        }
        else if (i == 3) { // Desktop
            checked = IsNativeDesktopToolbarShown();
        }
        else if (clsids[i]) {
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
    ShowGroup(hwnd, kStartCtls, tab == 1);
    ShowGroup(hwnd, kToolbarCtls, tab == 2);
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

// ============================================================================
// HOOK TrayUI - legge la posizione dal registro e si adatta automaticamente
// ============================================================================
static std::atomic<bool> g_taskbarPosHooksInstalled{false};

static DWORD GetTaskbarSideFromRegistry() {
    return TaskbarSettingsProvider::GetTaskbarEdge();
}

static void ClearRotatedIconCache();
static DWORD g_lastEdge = 3;

using TrayUI_GetStuckInfo_t = void(WINAPI*)(void* pThis, RECT* rect, DWORD* taskbarPos);
static TrayUI_GetStuckInfo_t TrayUI_GetStuckInfo_Original = nullptr;

using TrayUI__StuckTrayChange_t = void(WINAPI*)(void* pThis);
static TrayUI__StuckTrayChange_t TrayUI__StuckTrayChange_Original = nullptr;

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
    if (width > 0 && width < monitorWidth) {
        thickness = width;
    } else if (height > 0 && height < monitorHeight) {
        thickness = height;
    } else {
        thickness = height > 0 ? height : width;
    }
    if (thickness <= 0 || thickness > 300) thickness = 40;
    return thickness;
}

static void WINAPI TrayUI_GetStuckInfo_Hook(void* pThis, RECT* rect, DWORD* taskbarPos) {
    TrayUI_GetStuckInfo_Original(pThis, rect, taskbarPos);
    if (g_taskbarPosHooksInstalled) {
        *taskbarPos = GetTaskbarSideFromRegistry();
    }
}

static DWORD WINAPI TrayUI_GetDockedRect_Hook(void* pThis, RECT* rect, BOOL param2) {
    DWORD ret = TrayUI_GetDockedRect_Original(pThis, rect, param2);
    if (!g_taskbarPosHooksInstalled) return ret;

    DWORD edge = GetTaskbarSideFromRegistry();
    if (edge == 3) return ret;

    HMONITOR monitor = MonitorFromRect(rect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(monitor, &mi);

    int thickness = ComputeTaskbarThickness(rect, mi);

    switch (edge) {
        case 0: // Left
            SetRect(rect, mi.rcMonitor.left, mi.rcMonitor.top,
                    mi.rcMonitor.left + thickness, mi.rcMonitor.bottom);
            break;
        case 1: // Top
            SetRect(rect, mi.rcMonitor.left, mi.rcMonitor.top,
                    mi.rcMonitor.right, mi.rcMonitor.top + thickness);
            break;
        case 2: // Right
            SetRect(rect, mi.rcMonitor.right - thickness, mi.rcMonitor.top,
                    mi.rcMonitor.right, mi.rcMonitor.bottom);
            break;
    }
    return ret;
}

static void WINAPI TrayUI_MakeStuckRect_Hook(void* pThis, RECT* rect, RECT* param2, SIZE param3, DWORD taskbarPos) {
    TrayUI_MakeStuckRect_Original(pThis, rect, param2, param3, taskbarPos);
    if (!g_taskbarPosHooksInstalled) return;

    DWORD edge = GetTaskbarSideFromRegistry();
    if (edge == 3) return;

    HMONITOR monitor = MonitorFromRect(rect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(monitor, &mi);

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
    if (hTray) {
        // Riposiziona PRIMA il contenitore radice (Shell_TrayWnd): questo
        // impila ReBarWindow32 e TrayNotifyWnd in colonna sotto il pulsante
        // Start quando la barra e' verticale. Solo DOPO ha senso far
        // ricalcolare il contenuto interno di ciascuno (icone, ecc.), che
        // altrimenti lavorerebbe su dimensioni non ancora corrette.
        PostMessageW(hTray, WM_SIZE, 0, 0);

        HWND hReBar = FindWindowExW(hTray, nullptr, L"ReBarWindow32", nullptr);
        if (hReBar) {
            PostMessageW(hReBar, WM_SIZE, 0, 0);
            HWND hTaskSw = FindWindowExW(hReBar, nullptr, L"MSTaskSwWClass", nullptr);
            if (hTaskSw) PostMessageW(hTaskSw, WM_SIZE, 0, 0);
        }
        HWND hTrayNotify = FindWindowExW(hTray, nullptr, L"TrayNotifyWnd", nullptr);
        if (hTrayNotify) PostMessageW(hTrayNotify, WM_SIZE, 0, 0);
        InvalidateRect(hTray, nullptr, TRUE);
    }
}

static bool InstallTaskbarPositionHooks() {
    HMODULE module = GetModuleHandleW(nullptr);
    if (!module) {
        Wh_Log(L"InstallTaskbarPositionHooks: GetModuleHandle(NULL) fallito");
        return false;
    }

    // explorer.exe
    WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
        {
            {LR"(public: virtual void __cdecl TrayUI::GetStuckInfo(struct tagRECT *,unsigned int *))"},
            &TrayUI_GetStuckInfo_Original,
            TrayUI_GetStuckInfo_Hook,
        },
        {
            {LR"(public: void __cdecl TrayUI::_StuckTrayChange(void))"},
            &TrayUI__StuckTrayChange_Original,
        },
        {
            {LR"(public: void __cdecl TrayUI::_HandleSettingChange(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &TrayUI__HandleSettingChange_Original,
            TrayUI__HandleSettingChange_Hook,
        },
        {
            {LR"(public: virtual unsigned int __cdecl TrayUI::GetDockedRect(struct tagRECT *,int))"},
            &TrayUI_GetDockedRect_Original,
            TrayUI_GetDockedRect_Hook,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::MakeStuckRect(struct tagRECT *,struct tagRECT const *,struct tagSIZE,unsigned int))"},
            &TrayUI_MakeStuckRect_Original,
            TrayUI_MakeStuckRect_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, explorerExeHooks, ARRAYSIZE(explorerExeHooks))) {
        Wh_Log(L"InstallTaskbarPositionHooks: HookSymbols fallito su explorer.exe");
        return false;
    }

    g_taskbarPosHooksInstalled = true;
    Wh_Log(L"InstallTaskbarPositionHooks: TUTTI e 5 gli hook installati con successo");
    return true;
}

struct RotatedIconCacheEntry {
    HICON originalIcon = nullptr;
    int size = 0;
    bool clockwise = false;
    HBITMAP rotatedBitmap = nullptr;
};

static std::vector<RotatedIconCacheEntry> g_rotatedIconCache;
static std::atomic<bool> g_modUnloading{false};
static bool g_rotateVerticalIcons = false;

static void ClearRotatedIconCache() {
    for (auto& e : g_rotatedIconCache)
        if (e.rotatedBitmap) DeleteObject(e.rotatedBitmap);
    g_rotatedIconCache.clear();
}

static bool IsEdgeVertical(DWORD edge) { return edge == 0 || edge == 2; } // left / right

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
            else           { dx = y; dy = w - 1 - x; }
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
    for (auto& e : g_rotatedIconCache)
        if (e.originalIcon == hIcon && e.size == size && e.clockwise == clockwise)
            return e.rotatedBitmap;

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
    if (!hbmIcon || !bits) { DeleteDC(hdcMem); ReleaseDC(nullptr, hdcScreen); return nullptr; }

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
    switch (tbcd->nmcd.dwDrawStage) {
    case CDDS_PREPAINT:
        return CDRF_NOTIFYITEMDRAW;
    case CDDS_ITEMPREPAINT: {
        if (g_modUnloading) return CDRF_DODEFAULT;

        HDC hdc = tbcd->nmcd.hdc;
        RECT rc = tbcd->nmcd.rc;
        int cmdId = (int)tbcd->nmcd.dwItemSpec;
        int btnIndex = (int)SendMessageW(hWndToolbar, TB_COMMANDTOINDEX, cmdId, 0);
        if (btnIndex < 0) return CDRF_DODEFAULT;

        TBBUTTON tbb{};
        if (!SendMessageW(hWndToolbar, TB_GETBUTTON, btnIndex, (LPARAM)&tbb))
            return CDRF_DODEFAULT;

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
                bool clockwise = (edge == 0); // left = clockwise, right = counter-clockwise
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
            if (edge == 0) ind.right = ind.left + 3;      // left
            else if (edge == 2) ind.left = ind.right - 3; // right
            FillRect(hdc, &ind, hb);
        }
        return CDRF_SKIPDEFAULT;
    }
    }
    return CDRF_DODEFAULT;
}

// ============================================================================
// Helper: trova l'indice della banda del ReBar che ospita hWndChild.
// ============================================================================
static int FindRebarBandIndexForChild(HWND hReBar, HWND hWndChild) {
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

// ============================================================================
// TaskSwSubclassProc - gestisce la colonna verticale dei pulsanti dei
// programmi aperti (MSTaskSwWClass).
//
// Nota tecnica importante: TB_SETROWS ha effetto SOLO se (a) la toolbar ha
// lo stile TBSTYLE_WRAPABLE e (b) la finestra e' GIA' stretta (larghezza =
// spessore verticale) nel momento in cui il messaggio viene inviato, perche'
// comctl32 decide il wrap in base alla larghezza CORRENTE della finestra.
// Per questo qui aggiungiamo lo stile e restringiamo fisicamente la
// finestra PRIMA di chiamare TB_SETROWS.
//
// IMPORTANTE (architettura anti-sfarfallio): il ricalcolo VERO E PROPRIO del
// layout non avviene piu' qui ne' in TrayNotifySubclassProc separatamente.
// In precedenza ogni finestra (MSTaskSwWClass, TrayNotifyWnd, Shell_TrayWnd)
// aveva un proprio timer indipendente che eseguiva SetWindowPos per conto
// suo: pur essendo "debounced" singolarmente, restavano comunque TRE
// passate di ridisegno separate e non sincronizzate, ognuna delle quali
// produce un repaint visibile immediato di Windows - la causa reale dello
// sfarfallio "le icone appaiono e scompaiono a scatti".
//
// Ora invece TUTTE le finestre subclassate si limitano ad "armare" un unico
// timer sul contenitore radice (Shell_TrayWnd). Quando quel timer scatta,
// DoApplyVerticalTaskbarLayoutAtomic() esegue l'INTERO ricalcolo (posizione
// di ReBarWindow32/TrayNotifyWnd, colonna dei pulsanti aperti, colonna delle
// icone tray) dentro un unico blocco protetto da WM_SETREDRAW, seguito da UN
// solo RedrawWindow finale: un solo repaint pulito invece di N a scatti.
// ============================================================================
static constexpr UINT_PTR kTimerIdMasterLayout = 0x57A0;
static constexpr UINT kLayoutDebounceMs = 50;

static void ArmMasterLayoutTimer(HWND hShellTrayWnd) {
    if (!hShellTrayWnd || !IsWindow(hShellTrayWnd) || g_modUnloading) return;
    SetTimer(hShellTrayWnd, kTimerIdMasterLayout, kLayoutDebounceMs, nullptr);
}

// Layout interno di MSTaskSwWClass (pulsanti dei programmi aperti impilati
// in colonna singola). Estratta come funzione pura, richiamata UNA sola
// volta dal ricalcolo atomico centralizzato (vedi
// DoApplyVerticalTaskbarLayoutAtomic), non piu' da un proprio timer
// indipendente.
static void ApplyTaskSwInternalLayout(HWND hWnd, bool vertical, int thickness) {
    static thread_local HWND s_lastHwnd = nullptr;
    static thread_local DWORD s_lastEdgeApplied = 0xFFFFFFFF;
    static thread_local int s_lastThicknessApplied = -1;

    DWORD edge = TaskbarSettingsProvider::GetTaskbarEdge();

    if (vertical) {
        bool alreadyApplied = (s_lastHwnd == hWnd &&
                               s_lastEdgeApplied == edge &&
                               s_lastThicknessApplied == thickness);
        Wh_Log(L"[LayoutDump] ApplyTaskSwInternalLayout vertical=1 thickness=%d alreadyApplied=%d (cache: hwnd=0x%p edge=%d thick=%d)",
               thickness, alreadyApplied ? 1 : 0, s_lastHwnd, s_lastEdgeApplied, s_lastThicknessApplied);
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
                SetWindowPos(hWnd, NULL, 0, 0, thickness, curH,
                             SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
            }

            RECT rowsRect{};
            SendMessageW(hWnd, TB_SETROWS, MAKEWPARAM(btnCount, FALSE), (LPARAM)&rowsRect);
            SendMessageW(hWnd, TB_AUTOSIZE, 0, 0);

            HWND hReBar = GetParent(hWnd);
            if (hReBar) {
                int bandIndex = FindRebarBandIndexForChild(hReBar, hWnd);
                if (bandIndex >= 0) {
                    REBARBANDINFOW rbbiCur{};
                    rbbiCur.cbSize = sizeof(rbbiCur);
                    rbbiCur.fMask = RBBIM_CHILDSIZE;
                    bool needsUpdate = true;
                    if (SendMessageW(hReBar, RB_GETBANDINFOW, bandIndex, (LPARAM)&rbbiCur)) {
                        needsUpdate = (rbbiCur.cyMinChild != (UINT)thickness ||
                                      rbbiCur.cxMinChild != (UINT)thickness);
                    }
                    if (needsUpdate) {
                        REBARBANDINFOW rbbi{};
                        rbbi.cbSize = sizeof(rbbi);
                        rbbi.fMask = RBBIM_CHILDSIZE;
                        rbbi.cyMinChild = thickness;
                        rbbi.cxMinChild = thickness;
                        SendMessageW(hReBar, RB_SETBANDINFOW, bandIndex, (LPARAM)&rbbi);
                    }
                }
            }

            RECT rcFinal{};
            GetWindowRect(hWnd, &rcFinal);
            DWORD styleFinal = (DWORD)GetWindowLongPtrW(hWnd, GWL_STYLE);
            Wh_Log(L"[LayoutDump] ApplyTaskSwInternalLayout FATTO: btnCount=%d newSize=%d rectFinale=(%d,%d,%d,%d) styleWrapable=%d bandIndex=%d",
                   btnCount, newSize, rcFinal.left, rcFinal.top, rcFinal.right, rcFinal.bottom,
                   (styleFinal & TBSTYLE_WRAPABLE) ? 1 : 0, hReBar ? FindRebarBandIndexForChild(hReBar, hWnd) : -99);

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
    switch (uMsg) {
    case WM_SIZE: {
        LRESULT ret = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        if (!g_modUnloading) {
            ArmMasterLayoutTimer(GetAncestor(hWnd, GA_ROOT));
        }
        return ret;
    }
    case WM_NCDESTROY:
        RemoveWindowSubclass(hWnd, (SUBCLASSPROC)TaskSwSubclassProc, uIdSubclass);
        break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}




static LRESULT CALLBACK ReBarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR uIdSubclass) {
    if (uMsg == WM_NOTIFY) {
        NMHDR* nm = (NMHDR*)lParam;
        if (nm->code == NM_CUSTOMDRAW) {
            WCHAR cls[32];
            if (GetClassNameW(nm->hwndFrom, cls, ARRAYSIZE(cls)) &&
                _wcsicmp(cls, L"MSTaskSwWClass") == 0 && !g_modUnloading) {
                DWORD edge = TaskbarSettingsProvider::GetTaskbarEdge();
                if (IsEdgeVertical(edge))
                    return HandleTaskSwCustomDraw(nm->hwndFrom, (NMTBCUSTOMDRAW*)lParam, edge);
            }
        }
    } else if (uMsg == WM_NCDESTROY) {
        RemoveWindowSubclass(hWnd, (SUBCLASSPROC)ReBarSubclassProc, uIdSubclass);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ============================================================================
// Area di notifica (system tray) in verticale.
//
// Struttura reale (confermata via dump su Windows 10 19044):
//   Shell_TrayWnd
//    +- TrayNotifyWnd
//        +- Button (freccetta icone nascoste)
//        +- ToolbarWindow32 (elevazione, di solito vuota)
//        +- SysPager -> ToolbarWindow32 (icone tray vere e proprie)
//        +- TrayInputIndicatorWClass (IME/lingua, opzionale)
//        +- TrayClockWClass (orologio)
//        +- TrayShowDesktopButtonWClass
//
// Tutte queste classi sono documentate e stabili su Windows 7/8/8.1/10:
// il codice sotto le cerca per NOME DI CLASSE (non per indice fisso), quindi
// si adatta anche se qualche elemento manca (es. nessuna lingua IME
// installata) invece di aspettarsi sempre esattamente lo stesso set.
// ============================================================================
struct TrayChildNaturalSize {
    HWND hwnd = nullptr;
    int naturalLength = 0; // larghezza quando orizzontale
};

static std::vector<TrayChildNaturalSize> g_trayChildSizes;

static int GetOrCacheTrayChildNaturalLength(HWND hChild, int currentWidth, int currentHeight, bool isHorizontalNow) {
    for (auto& e : g_trayChildSizes) {
        if (e.hwnd == hChild) {
            if (isHorizontalNow && currentWidth > 0) {
                e.naturalLength = currentWidth;
            }
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
        g_trayChildSizes.end());
}

// BUGFIX (contenitore ReBarWindow32 schiacciato a 0px): la lunghezza totale
// di TrayNotifyWnd NON deve mai essere letta da GetWindowRect(hTrayNotify)
// stesso, perche' durante la rotazione Explorer puo' aver GIA' alterato
// quella finestra (es. allungata a piena altezza schermo) PRIMA che il
// nostro codice intervenga - leggerla significa fidarsi di un valore
// contaminato. La lunghezza corretta si ottiene invece SOMMANDO le
// dimensioni cache di ciascun figlio diretto (che riflettono la loro
// dimensione naturale reale, tracciata in orizzontale da
// GetOrCacheTrayChildNaturalLength), esattamente come poi fara' davvero
// ApplyTrayNotifyInternalLayout quando li impilera' uno sotto l'altro.
static int EstimateTrayNotifyTotalLength(HWND hTrayNotify) {
    if (!hTrayNotify) return 0;
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
    if (!hToolbar) return;
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

    // FONDAMENTALE: TB_SETROWS ha effetto SOLO se la finestra della toolbar
    // e' GIA' stretta (larghezza = thickness) nel momento in cui viene
    // inviato, perche' comctl32 decide quante icone entrano per riga in
    // base alla larghezza CORRENTE della finestra, non al parametro
    // richiesto. Se non la restringiamo prima, la toolbar resta larga
    // quanto il suo layout orizzontale naturale, TB_SETROWS viene ignorato
    // silenziosamente, e quando il chiamante restringe poi il contenitore
    // esterno allo spessore della barra, tutte le icone che eccedono quella
    // larghezza vengono semplicemente CLIPPATE (non ridisegnate altrove) -
    // e' esattamente il sintomo "solo alcune icone visibili".
    //
    // BUGFIX (icone giganti, 768px l'una): qui in precedenza usavamo
    // GetWindowRect per leggere l'altezza CORRENTE della toolbar (curH) e
    // la riapplicavamo come vincolo prima del wrap. Il problema e' che,
    // durante la rotazione, Explorer stesso (nel suo tentativo nativo
    // parziale) puo' aver GIA' allungato questa finestra all'intera
    // altezza dello schermo PRIMA che il nostro codice intervenga: quel
    // valore "corrente" e' quindi gia' corrotto (es. 768px), e imponendolo
    // come vincolo prima del TB_SETROWS otteniamo una singola riga alta
    // quanto l'intero schermo diviso i bottoni (768/3 = 256px ciascuno,
    // arrotondato dal layout interno a valori ancora piu' estremi nei
    // log osservati). La dimensione corretta di riferimento e' invece
    // SEMPRE newSize*btnCount (numero di bottoni per la dimensione
    // bottone che abbiamo appena impostato con TB_SETBUTTONSIZE) - un
    // valore che calcoliamo noi stessi e non dipende da quanto Explorer
    // ha gia' alterato la finestra.
    int expectedH = newSize * btnCount;
    SetWindowPos(hToolbar, NULL, 0, 0, thickness, expectedH,
                 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    RECT rowsRect{};
    SendMessageW(hToolbar, TB_SETROWS, MAKEWPARAM(btnCount, FALSE), (LPARAM)&rowsRect);
    SendMessageW(hToolbar, TB_AUTOSIZE, 0, 0);
}



static void RestoreToolbarHorizontalRow(HWND hToolbar) {
    if (!hToolbar) return;
    LONG_PTR curStyle = GetWindowLongPtrW(hToolbar, GWL_STYLE);
    if (curStyle & TBSTYLE_WRAPABLE) {
        SetWindowLongPtrW(hToolbar, GWL_STYLE, curStyle & ~TBSTYLE_WRAPABLE);
    }
    RECT rowsRect{};
    SendMessageW(hToolbar, TB_SETROWS, MAKEWPARAM(1, FALSE), (LPARAM)&rowsRect);
    SendMessageW(hToolbar, TB_AUTOSIZE, 0, 0);
}

// Layout interno di TrayNotifyWnd (icone/orologio impilati in colonna).
// Estratta come funzione pura, richiamata UNA sola volta dal ricalcolo
// atomico centralizzato (vedi DoApplyVerticalTaskbarLayoutAtomic), non piu'
// da un proprio timer indipendente.
static void ApplyTrayNotifyInternalLayout(HWND hWnd, bool vertical, int thickness) {
    PurgeDestroyedTrayChildCacheEntries();

    Wh_Log(L"[LayoutDump] ApplyTrayNotifyInternalLayout INIZIO vertical=%d thickness=%d", vertical ? 1 : 0, thickness);

    int pos = 0;
    int childIndex = 0;
    HWND hChild = GetWindow(hWnd, GW_CHILD);
    while (hChild) {
        HWND hNext = GetWindow(hChild, GW_HWNDNEXT);
        WCHAR clsDbg[64] = {};
        GetClassNameW(hChild, clsDbg, ARRAYSIZE(clsDbg));
        Wh_Log(L"[LayoutDump]   figlio #%d class=%s hwnd=0x%p", childIndex, clsDbg, hChild);
        // BUGFIX (causa reale delle icone mancanti): IsWindowVisible()
        // risultava SEMPRE false anche per figli realmente visibili sullo
        // schermo (es. il pulsante Button della freccetta icone nascoste),
        // esattamente come per ReBarWindow32/TrayNotifyWnd sopra. Il filtro
        // "if (IsWindowVisible(hChild))" bloccava quindi il riposizionamento
        // di TUTTI i 6 figli, confermato nei log da "pos finale=0" per ogni
        // ciclo. Ora processiamo sempre tutti i figli restituiti da
        // GetWindow; un figlio davvero senza contenuto reale (es. la
        // toolbar di elevazione vuota) semplicemente occupera' 1 pixel
        // (il minimo forzato da std::max(naturalLength, 1) sotto), un
        // effetto collaterale trascurabile rispetto al bug precedente.
        {
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

            int naturalLength = GetOrCacheTrayChildNaturalLength(
                hChild, curW, curH, !vertical);

            Wh_Log(L"[LayoutDump]     curW=%d curH=%d hInnerToolbar=0x%p naturalLength=%d pos=%d",
                   curW, curH, hInnerToolbar, naturalLength, pos);

            if (vertical) {
                int newW = thickness;
                int newH = std::max(naturalLength, 1);

                if (hInnerToolbar) {
                    // Applica prima il layout a colonna singola alla toolbar
                    // interna: dopo TB_AUTOSIZE, la toolbar riporta la sua
                    // altezza REALE necessaria per mostrare TUTTE le icone
                    // impilate (numero di bottoni x dimensione bottone).
                    // Usiamo quella, non la stima cache, per essere certi
                    // che il contenitore esterno (SysPager/hChild) sia
                    // sempre abbastanza alto da non clippare icone.
                    int btnCountDbg = (int)SendMessageW(hInnerToolbar, TB_BUTTONCOUNT, 0, 0);
                    ForceToolbarVerticalColumn(hInnerToolbar, thickness);

                    RECT rcInner{};
                    GetWindowRect(hInnerToolbar, &rcInner);
                    int innerH = rcInner.bottom - rcInner.top;
                    if (innerH > newH) newH = innerH;

                    Wh_Log(L"[LayoutDump]     hInnerToolbar btnCount=%d dopo ForceToolbarVerticalColumn rect=(%d,%d,%d,%d) innerH=%d",
                           btnCountDbg, rcInner.left, rcInner.top, rcInner.right, rcInner.bottom, innerH);

                    // Il contenitore deve essere ALMENO tanto alto quanto la
                    // toolbar che ospita, altrimenti la clippa comunque.
                    SetWindowPos(hInnerToolbar, NULL, 0, 0, thickness, innerH,
                                 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
                }

                SetWindowPos(hChild, NULL, 0, pos, newW, newH,
                             SWP_NOZORDER | SWP_NOACTIVATE);
                Wh_Log(L"[LayoutDump]     hChild posizionato a (0,%d) dimensioni %dx%d", pos, newW, newH);
                pos += newH;
            } else {
                if (hInnerToolbar) RestoreToolbarHorizontalRow(hInnerToolbar);
            }
        }
        childIndex++;
        hChild = hNext;
    }


    Wh_Log(L"[LayoutDump] ApplyTrayNotifyInternalLayout FINE, totale figli=%d, pos finale=%d", childIndex, pos);
}

static LRESULT CALLBACK TrayNotifySubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR uIdSubclass) {
    switch (uMsg) {
    case WM_SIZE: {
        LRESULT ret = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        if (!g_modUnloading) {
            ArmMasterLayoutTimer(GetAncestor(hWnd, GA_ROOT));
        }
        return ret;
    }
    case WM_NCDESTROY:
        RemoveWindowSubclass(hWnd, (SUBCLASSPROC)TrayNotifySubclassProc, uIdSubclass);
        break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ============================================================================
// Shell_TrayWnd / Shell_SecondaryTrayWnd (il contenitore radice della taskbar)
//
// Problema 1: quando la barra e' verticale, il pulsante Start (finestra
// "Start") viene sempre riposizionato correttamente da Explorer, ma i suoi
// altri due figli diretti - ReBarWindow32 (contiene i pulsanti dei
// programmi aperti) e TrayNotifyWnd (contiene l'area di notifica) - NON
// vengono ridimensionati/riposizionati da Explorer stesso quando la barra
// passa da orizzontale a verticale: restano con le dimensioni orizzontali
// originarie (larghi quanto lo schermo, alti quanto lo spessore), quindi
// finiscono per essere in pratica "fuori" dalla striscia stretta e
// invisibili.
//
// Problema 2 (sfarfallio): applicare le correzioni sopra con TRE timer
// indipendenti (uno per Shell_TrayWnd, uno per ReBarWindow32/MSTaskSwWClass,
// uno per TrayNotifyWnd), ognuno dei quali esegue il proprio SetWindowPos
// per conto suo in un momento leggermente diverso, produce comunque piu'
// passate di ridisegno visibili separate: e' la causa reale delle icone che
// "appaiono e scompaiono a scatti", indipendentemente da quanto breve sia
// il debounce di ciascun timer preso singolarmente.
//
// Soluzione: un SOLO timer, armato da qualunque delle tre finestre (vedi
// ArmMasterLayoutTimer, chiamata da TaskSwSubclassProc e
// TrayNotifySubclassProc), che quando scatta esegue l'INTERO ricalcolo
// (posizione di ReBarWindow32/TrayNotifyWnd + layout interno di entrambe le
// toolbar) in un'unica funzione, racchiusa tra WM_SETREDRAW FALSE/TRUE +
// UN solo RedrawWindow finale. Questo e' il pattern standard Win32 per
// riposizionare piu' finestre correlate senza sfarfallio: durante il blocco
// WM_SETREDRAW, Windows accumula le modifiche senza ridisegnare nulla sullo
// schermo; il RedrawWindow finale produce un solo repaint pulito invece di
// N repaint intermedi a scatti.
//
// Le classi usate (Shell_TrayWnd, Start, ReBarWindow32, TrayNotifyWnd) sono
// documentate e stabili su tutta la famiglia Win32 classica (Windows
// 7/8/8.1/10), quindi il codice si adatta a qualunque build di questa
// famiglia invece di assumere offset/dimensioni fisse.
// ============================================================================
static void DoApplyVerticalTaskbarLayoutAtomic(HWND hShellTrayWnd) {
    static thread_local bool s_inResize = false;
    if (g_modUnloading || s_inResize || !IsWindow(hShellTrayWnd)) return;
    s_inResize = true;

    DWORD edge = TaskbarSettingsProvider::GetTaskbarEdge();
    bool vertical = IsEdgeVertical(edge);

    Wh_Log(L"[LayoutDump] === INIZIO ciclo === edge=%d vertical=%d", edge, vertical ? 1 : 0);

    // Blocca tutti i repaint intermedi: da qui fino al RedrawWindow finale,
    // qualunque SetWindowPos su queste finestre non produce alcun flash
    // visibile, perche' Windows accumula le modifiche senza disegnarle.
    SendMessageW(hShellTrayWnd, WM_SETREDRAW, FALSE, 0);
    HWND hReBar = FindWindowExW(hShellTrayWnd, NULL, L"ReBarWindow32", NULL);
    HWND hTrayNotify = FindWindowExW(hShellTrayWnd, NULL, L"TrayNotifyWnd", NULL);
    Wh_Log(L"[LayoutDump] hReBar=0x%p hTrayNotify=0x%p", hReBar, hTrayNotify);
    if (hReBar) SendMessageW(hReBar, WM_SETREDRAW, FALSE, 0);
    if (hTrayNotify) SendMessageW(hTrayNotify, WM_SETREDRAW, FALSE, 0);

    if (vertical) {
        RECT rcClient{};
        GetClientRect(hShellTrayWnd, &rcClient);
        int thickness = rcClient.right - rcClient.left;
        int totalLength = rcClient.bottom - rcClient.top;
        if (thickness < 16 || thickness > 300) thickness = 40;

        Wh_Log(L"[LayoutDump] rcClient=(%d,%d,%d,%d) thickness=%d totalLength=%d",
               rcClient.left, rcClient.top, rcClient.right, rcClient.bottom,
               thickness, totalLength);

        // BUGFIX (causa reale delle icone mancanti): in precedenza qui
        // leggevamo la posizione REALE del pulsante Start con GetWindowRect
        // + ScreenToClient per calcolare da dove far partire ReBarWindow32.
        // Durante la transizione orizzontale->verticale, pero', il pulsante
        // Start puo' trovarsi ancora fisicamente nelle sue vecchie
        // coordinate (schermo) mentre Shell_TrayWnd ha gia' la nuova
        // geometria verticale: la conversione produceva valori incoerenti
        // (es. startBottom = 768, cioe' l'intera altezza schermo, quando il
        // valore atteso era circa 40-48). Con startBottom errato,
        // reBarLength risultava 0: il rebar veniva schiacciato a zero
        // pixel di altezza e tutte le icone al suo interno restavano
        // invisibili (confermato dai log: "reBarLength=0" e "pos finale=0"
        // su ApplyTrayNotifyInternalLayout).
        //
        // Il pulsante Start, quando la barra e' verticale, e' sempre un
        // quadrato thickness x thickness (stessa larghezza della barra,
        // stessa altezza della larghezza): non serve leggerne le
        // coordinate reali, possiamo semplicemente assumere che occupi le
        // prime "thickness" unita' e far partire ReBarWindow32 subito dopo.
        // Questo e' deterministico e non dipende da uno stato di
        // transizione temporaneo.
        int startBottom = thickness;

        Wh_Log(L"[LayoutDump] startBottom (fisso = thickness) = %d", startBottom);

        // BUGFIX 2 (causa reale, insieme al bug sopra): IsWindowVisible()
        // su ReBarWindow32/TrayNotifyWnd risultava SEMPRE false nei log,
        // anche quando la barra era orizzontale e visibile sullo schermo
        // (style catturato: 0x5600B25D, che NON ha il bit WS_VISIBLE
        // impostato secondo IsWindowVisible). Explorer evidentemente
        // gestisce la visibilita' effettiva di questi controlli in modo
        // diverso dal flag standard. Il risultato era che TUTTI i controlli
        // "if (IsWindowVisible(...))" sottostanti bloccavano silenziosamente
        // il riposizionamento dei contenitori e di tutti i loro figli.
        // Da qui in poi consideriamo questi due contenitori sempre da
        // gestire se FindWindowExW li ha trovati (se esistono, vanno
        // riposizionati, a prescindere dal flag di visibilita').

        // IMPORTANTE (ordine delle operazioni): ReBarWindow32 e' un
        // controllo nativo che ridispone DA SOLO la propria banda interna
        // ogni volta che viene ridimensionato (SetWindowPos). Se
        // applicassimo prima il layout interno di MSTaskSwWClass e SOLO
        // DOPO ridimensionassimo il contenitore ReBarWindow32, quest'ultimo
        // SetWindowPos farebbe scattare la logica nativa del rebar, che
        // ridispone la banda con la sua logica di default, CANCELLANDO il
        // lavoro appena fatto sul contenuto interno. Per questo motivo
        // l'ordine corretto e' sempre: PRIMA dimensionare/posizionare i
        // CONTENITORI (ReBarWindow32, TrayNotifyWnd), POI applicare il
        // layout interno (colonna dei pulsanti, colonna delle icone tray),
        // cosi' che l'ultima parola sul contenuto la abbia sempre la
        // nostra funzione, non la logica nativa del rebar innescata da un
        // resize successivo.
        //
        // BUGFIX: non leggiamo piu' la lunghezza di TrayNotifyWnd dalle sue
        // dimensioni CORRENTI (GetWindowRect), perche' durante la
        // rotazione Explorer puo' averle gia' alterate/contaminate (es.
        // allungate a piena altezza schermo) prima del nostro intervento.
        // La calcoliamo invece sommando le dimensioni cache dei singoli
        // figli, che sono la stessa fonte di verita' che
        // ApplyTrayNotifyInternalLayout usera' davvero per impilarli.
        int trayNotifyLength = EstimateTrayNotifyTotalLength(hTrayNotify);
        Wh_Log(L"[LayoutDump] trayNotifyLength stimato dalla cache figli = %d", trayNotifyLength);
        if (trayNotifyLength <= 0 || trayNotifyLength > totalLength) {
            trayNotifyLength = std::min(187, totalLength / 2);
            Wh_Log(L"[LayoutDump] trayNotifyLength fuori range, fallback a %d", trayNotifyLength);
        }

        int reBarTop = startBottom;
        int reBarLength = totalLength - startBottom - trayNotifyLength;
        if (reBarLength < 0) reBarLength = 0;

        Wh_Log(L"[LayoutDump] Applico: reBarTop=%d reBarLength=%d trayNotifyLength=%d",
               reBarTop, reBarLength, trayNotifyLength);

        // 1) PRIMA i contenitori. Nota: non filtriamo piu' con
        //    IsWindowVisible (vedi BUGFIX 2 sopra) - se FindWindowExW ha
        //    trovato la finestra, la riposizioniamo.
        if (hReBar) {
            BOOL ok1 = SetWindowPos(hReBar, NULL, 0, reBarTop, thickness, reBarLength,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            RECT rcAfter{};
            GetWindowRect(hReBar, &rcAfter);
            MapWindowPoints(HWND_DESKTOP, hShellTrayWnd, (LPPOINT)&rcAfter, 2);
            Wh_Log(L"[LayoutDump] SetWindowPos(hReBar) ok=%d -> rect risultante (client-rel)=(%d,%d,%d,%d)",
                   ok1, rcAfter.left, rcAfter.top, rcAfter.right, rcAfter.bottom);
        }

        if (hTrayNotify) {
            int trayNotifyTop = totalLength - trayNotifyLength;
            if (trayNotifyTop < reBarTop) trayNotifyTop = reBarTop;
            BOOL ok2 = SetWindowPos(hTrayNotify, NULL, 0, trayNotifyTop, thickness, trayNotifyLength,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            RECT rcAfter{};
            GetWindowRect(hTrayNotify, &rcAfter);

            MapWindowPoints(HWND_DESKTOP, hShellTrayWnd, (LPPOINT)&rcAfter, 2);
            Wh_Log(L"[LayoutDump] SetWindowPos(hTrayNotify) ok=%d trayNotifyTop=%d -> rect risultante (client-rel)=(%d,%d,%d,%d)",
                   ok2, trayNotifyTop, rcAfter.left, rcAfter.top, rcAfter.right, rcAfter.bottom);
        }

        // 2) POI il contenuto interno, che ha sempre l'ultima parola sulla
        //    disposizione dei pulsanti/icone, perche' nessun altro resize
        //    dei contenitori avviene dopo questo punto.
        if (hReBar) {
            HWND hTaskSw = FindWindowExW(hReBar, NULL, L"MSTaskSwWClass", NULL);
            Wh_Log(L"[LayoutDump] hTaskSw=0x%p", hTaskSw);
            if (hTaskSw) {
                int btnCountBefore = (int)SendMessageW(hTaskSw, TB_BUTTONCOUNT, 0, 0);
                ApplyTaskSwInternalLayout(hTaskSw, true, thickness);
                RECT rcTaskSwAfter{};
                GetWindowRect(hTaskSw, &rcTaskSwAfter);
                Wh_Log(L"[LayoutDump] MSTaskSwWClass btnCount=%d dopo ApplyTaskSwInternalLayout rect=(%d,%d,%d,%d)",
                       btnCountBefore, rcTaskSwAfter.left, rcTaskSwAfter.top, rcTaskSwAfter.right, rcTaskSwAfter.bottom);
            }
        }
        if (hTrayNotify) {
            ApplyTrayNotifyInternalLayout(hTrayNotify, true, thickness);
            RECT rcTrayNotifyAfter{};
            GetWindowRect(hTrayNotify, &rcTrayNotifyAfter);
            Wh_Log(L"[LayoutDump] TrayNotifyWnd dopo ApplyTrayNotifyInternalLayout rect=(%d,%d,%d,%d)",
                   rcTrayNotifyAfter.left, rcTrayNotifyAfter.top, rcTrayNotifyAfter.right, rcTrayNotifyAfter.bottom);
        }
    } else {

        // Layout orizzontale: ripristina il contenuto interno delle due
        // toolbar. Le posizioni/dimensioni dei contenitori restano gestite
        // da Explorer nativamente in questo caso.
        if (hReBar) {
            HWND hTaskSw = FindWindowExW(hReBar, NULL, L"MSTaskSwWClass", NULL);
            if (hTaskSw) ApplyTaskSwInternalLayout(hTaskSw, false, 0);
        }
        if (hTrayNotify) {
            ApplyTrayNotifyInternalLayout(hTrayNotify, false, 0);
        }
    }

    // Riabilita il ridisegno e forza UN solo repaint pulito, ricorsivo su
    // tutti i figli appena modificati.
    if (hTrayNotify) SendMessageW(hTrayNotify, WM_SETREDRAW, TRUE, 0);
    if (hReBar) SendMessageW(hReBar, WM_SETREDRAW, TRUE, 0);
    SendMessageW(hShellTrayWnd, WM_SETREDRAW, TRUE, 0);
    RedrawWindow(hShellTrayWnd, NULL, NULL,
                 RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_ERASE | RDW_UPDATENOW);

    Wh_Log(L"[LayoutDump] === FINE ciclo ===");

    s_inResize = false;
}


static LRESULT CALLBACK ShellTrayWndSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR uIdSubclass) {
    switch (uMsg) {
    case WM_SIZE: {
        LRESULT ret = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        if (!g_modUnloading) {
            ArmMasterLayoutTimer(hWnd);
        }
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

static HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                         DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                         HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y,
                                          nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hWnd && lpClassName && !IS_INTRESOURCE(lpClassName)) {
        if (_wcsicmp(lpClassName, L"MSTaskSwWClass") == 0) {
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, TaskSwSubclassProc, 0);
        } else if (_wcsicmp(lpClassName, L"ReBarWindow32") == 0 && hWndParent && IsTaskbarWindowClass(hWndParent)) {
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, ReBarSubclassProc, 0);
        } else if (_wcsicmp(lpClassName, L"TrayNotifyWnd") == 0 && hWndParent && IsTaskbarWindowClass(hWndParent)) {
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, TrayNotifySubclassProc, 0);
        } else if (IsTaskbarWindowClass(hWnd)) {
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, ShellTrayWndSubclassProc, 0);
        }
    }
    return hWnd;
}

static BOOL CALLBACK SubclassExistingChildrenProc(HWND hWnd, LPARAM lParam) {
    WCHAR cls[32];
    if (GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) {
        if (_wcsicmp(cls, L"MSTaskSwWClass") == 0)
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, TaskSwSubclassProc, 0);
        else if (_wcsicmp(cls, L"ReBarWindow32") == 0)
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, ReBarSubclassProc, 0);
        else if (_wcsicmp(cls, L"TrayNotifyWnd") == 0)
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, TrayNotifySubclassProc, 0);
    }
    EnumChildWindows(hWnd, SubclassExistingChildrenProc, 0);
    return TRUE;
}

static BOOL CALLBACK UnsubclassExistingChildrenProc(HWND hWnd, LPARAM lParam) {
    WCHAR cls[32];
    if (GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) {
        if (_wcsicmp(cls, L"MSTaskSwWClass") == 0)
            RemoveWindowSubclass(hWnd, (SUBCLASSPROC)TaskSwSubclassProc, 0);
        else if (_wcsicmp(cls, L"ReBarWindow32") == 0)
            RemoveWindowSubclass(hWnd, (SUBCLASSPROC)ReBarSubclassProc, 0);
        else if (_wcsicmp(cls, L"TrayNotifyWnd") == 0)
            RemoveWindowSubclass(hWnd, (SUBCLASSPROC)TrayNotifySubclassProc, 0);
    }
    EnumChildWindows(hWnd, UnsubclassExistingChildrenProc, 0);
    return TRUE;
}

static BOOL CALLBACK SubclassExistingTaskbarsEnumProc(HWND hWnd, LPARAM lParam) {
    if (IsTaskbarWindowClass(hWnd)) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, ShellTrayWndSubclassProc, 0);
        EnumChildWindows(hWnd, SubclassExistingChildrenProc, 0);
    }
    return TRUE;
}

static void SubclassExistingTaskbars() {
    EnumWindows(SubclassExistingTaskbarsEnumProc, 0);
}

static BOOL CALLBACK UnsubclassExistingTaskbarsEnumProc(HWND hWnd, LPARAM lParam) {
    if (IsTaskbarWindowClass(hWnd)) {
        RemoveWindowSubclass(hWnd, (SUBCLASSPROC)ShellTrayWndSubclassProc, 0);
        EnumChildWindows(hWnd, UnsubclassExistingChildrenProc, 0);
    }
    return TRUE;
}

static void UnsubclassExistingTaskbars() {
    EnumWindows(UnsubclassExistingTaskbarsEnumProc, 0);
}

static void UpdateStuckRectsKey(LPCWSTR keyName, DWORD newEdge, DWORD edgeOffset) {
    WCHAR fullKey[256];
    swprintf_s(fullKey, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\%s", keyName);
    HKEY hk;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, fullKey, 0, KEY_READ | KEY_WRITE, &hk) == ERROR_SUCCESS) {
        DWORD sz = 0;
        RegQueryValueExW(hk, L"Settings", NULL, NULL, NULL, &sz);
        if (sz >= edgeOffset + 4) {
            BYTE* d = new BYTE[sz];
            if (RegQueryValueExW(hk, L"Settings", NULL, NULL, d, &sz) == ERROR_SUCCESS) {
                *(DWORD*)(d + edgeOffset) = newEdge;
                RegSetValueExW(hk, L"Settings", 0, REG_BINARY, d, sz);
            }
            delete[] d;
        }
        RegCloseKey(hk);
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

    TaskbarSettingsProvider::RegSetDWordSafe(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
        L"TaskbarSide", newEdge);
    UpdateAllStuckRects(newEdge);

    g_lastEdge = newEdge;

    HWND hTray = FindWindowExW(nullptr, nullptr, L"Shell_TrayWnd", nullptr);
    if (hTray) {
        SendMessageW(hTray, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);

        // Riposiziona PRIMA il contenitore radice (Shell_TrayWnd), che
        // impila ReBarWindow32/TrayNotifyWnd sotto il pulsante Start quando
        // la barra e' verticale. I successivi WM_SIZE sui singoli figli
        // ricalcolano poi il loro contenuto interno sulle dimensioni gia'
        // corrette.
        SendMessageW(hTray, WM_SIZE, 0, 0);

        HWND hReBar = FindWindowExW(hTray, NULL, L"ReBarWindow32", NULL);
        if (hReBar) {
            SendMessageW(hReBar, WM_SIZE, 0, 0);
            HWND hTaskSw = FindWindowExW(hReBar, NULL, L"MSTaskSwWClass", NULL);
            if (hTaskSw) {
                SendMessageW(hTaskSw, TB_AUTOSIZE, 0, 0);
                InvalidateRect(hTaskSw, NULL, TRUE);
            }
        }

        HWND hTrayNotify = FindWindowExW(hTray, NULL, L"TrayNotifyWnd", NULL);
        if (hTrayNotify) {
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
    Wh_Log(L"RotateTaskbarPosition: TaskbarSide=%d", newEdge);
}

static void ApplySettings(HWND hwnd) {
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

    // Mappa l'indice della ComboBox (0=Top, 1=Bottom) al valore edge corretto
    // Left (0) e Right (2) non sono disponibili nella UI, quindi mappiamo solo Top e Bottom
    // 0 = Top    -> edge 1
    // 1 = Bottom -> edge 3
    DWORD edge;
    if (locSel == 0) {
        edge = 1; // Top
    } else {
        edge = 3; // Bottom (default)
    }
    RotateTaskbarPosition(edge);

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

    HWND hList = GetDlgItem(hwnd, IDC_LST_TOOLBARS);
    if (hList) {
        bool addr = (ListView_GetCheckState(hList, 0) != 0);
        bool links = (ListView_GetCheckState(hList, 1) != 0);
        bool tablet = (ListView_GetCheckState(hList, 2) != 0);
            bool desk = false; // Desktop rimosso dalla UI
        ApplyToolbars(addr, links, tablet, desk);
    }

    SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"TraySettings");
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
        for (const auto& b : bindings) SetDlgItemTextW(hwnd, b.controlId, b.text);

        if (g_hFontUi) { SendMessageW(hwnd, WM_SETFONT, (WPARAM)g_hFontUi, TRUE); SetFontAllChildren(hwnd, g_hFontUi); }

        BalanceTextAndCombo(hwnd, IDC_TXT_LOCATION, IDC_COMBO_LOCATION);
        BalanceTextAndCombo(hwnd, IDC_TXT_BUTTONS, IDC_COMBO_BUTTONS);
        BalanceTextAndCombo(hwnd, IDC_TXT_POWER_LABEL, IDC_COMBO_POWER);

        // === INIZIO MODIFICA: ComboBox posizione. Left/Right rimosse per complessità non implementata. ===
        HWND hCL = GetDlgItem(hwnd, IDC_COMBO_LOCATION);
        SendMessageW(hCL, CB_RESETCONTENT, 0, 0);
        // Le opzioni Left e Right sono state rimosse in quanto la rotazione verticale
        // non è ancora completamente supportata. Verranno riaggiunte in futuro.
        // SendMessageW(hCL, CB_ADDSTRING, 0, (LPARAM)g_str.pos_left);   // Left
        SendMessageW(hCL, CB_ADDSTRING, 0, (LPARAM)g_str.pos_top);    // Indice 0 - Top
        // SendMessageW(hCL, CB_ADDSTRING, 0, (LPARAM)g_str.pos_right);  // Right
        SendMessageW(hCL, CB_ADDSTRING, 0, (LPARAM)g_str.pos_bottom); // Indice 1 - Bottom
        EnableWindow(hCL, TRUE);
        
        // Il subclassing per disabilitare alcune voci non è più necessario, ma lo lasciamo
        // commentato per quando Left/Right verranno ripristinate.
        // SetWindowSubclass(hCL, LocationComboSubclassProc, 0, 0);
        
        // Imposta la selezione corrente. Mappa i vecchi valori edge (0=Left, 1=Top, 2=Right, 3=Bottom)
        // ai nuovi indici della ComboBox (0=Top, 1=Bottom).
        DWORD edge = TaskbarSettingsProvider::GetTaskbarEdge();
        int newIndex = 1; // Default: Bottom
        if (edge == 1) newIndex = 0; // Top -> indice 0
        // Left (0) e Right (2) forzeranno il default Bottom (indice 1), che è il comportamento sicuro.
        
        SendMessageW(hCL, CB_SETCURSEL, (WPARAM)newIndex, 0);
        // === FINE MODIFICA ===

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
        bool isHide = (SHAppBarMessage(ABM_GETSTATE, &abd) & ABS_AUTOHIDE) != 0;

        SendDlgItemMessageW(hwnd, IDC_CHK_LOCK, BM_SETCHECK, (szMove == 0) ? BST_CHECKED : BST_UNCHECKED, 0);
        SendDlgItemMessageW(hwnd, IDC_CHK_HIDE, BM_SETCHECK, isHide ? BST_CHECKED : BST_UNCHECKED, 0);
        SendDlgItemMessageW(hwnd, IDC_CHK_SMALL, BM_SETCHECK, (szSmall != 0) ? BST_CHECKED : BST_UNCHECKED, 0);
        SendDlgItemMessageW(hwnd, IDC_CHK_AEROPEEK, BM_SETCHECK, aeroPeek ? BST_CHECKED : BST_UNCHECKED, 0);
        SendMessageW(hCB, CB_SETCURSEL, (glom < 3) ? (WPARAM)glom : 0, 0);

        DWORD curPower = TaskbarSettingsProvider::GetPowerAction();
        int powerIdx = 0;
        for (int i = 0; i < 7; i++) { if (kPowerValues[i] == curPower) { powerIdx = i; break; } }
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
        WORD id = LOWORD(wp); WORD act = HIWORD(wp);
        if ((act == BN_CLICKED || act == CBN_SELCHANGE) && id != IDOK && id != IDCANCEL && id != IDC_BTN_APPLY && id != IDC_BTN_CUST_NOTIF && id != IDC_BTN_START_CUST)
            EnableWindow(GetDlgItem(hwnd, IDC_BTN_APPLY), TRUE);

        if (id == IDOK) { ApplySettings(hwnd); if (IsWindow(hwnd)) DestroyWindow(hwnd); }
        else if (id == IDCANCEL) { DestroyWindow(hwnd); }
        else if (id == IDC_BTN_APPLY) { ApplySettings(hwnd); if (IsWindow(hwnd)) EnableWindow(GetDlgItem(hwnd, IDC_BTN_APPLY), FALSE); }
        else if (id == IDC_BTN_CUST_NOTIF) {
            HINSTANCE hRes = ShellExecuteW(hwnd, L"open", L"shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}", NULL, NULL, SW_SHOW);
            if ((INT_PTR)hRes <= 32) ShellExecuteW(hwnd, L"open", L"control.exe", L"/name Microsoft.NotificationAreaIcons", NULL, SW_SHOW);
        }
        else if (id == IDC_BTN_START_CUST) { ShowStartCustomDialog(hwnd); }
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
        if (hdr->idFrom == IDC_LINK_HELP && hdr->code == NM_CLICK) ShowAboutDialog(hwnd);
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
    if (g_taskbarPosHooksInstalled && g_lastEdge != 3) {
        TaskbarSettingsProvider::RegSetDWordSafe(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
            L"TaskbarSide", 3);
        UpdateAllStuckRects(3);

        g_modUnloading = true;
        UnsubclassExistingTaskbars();
        ClearRotatedIconCache();

        HWND hTray = FindWindowExW(nullptr, nullptr, L"Shell_TrayWnd", nullptr);
        if (hTray) {
            SendMessageW(hTray, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);
            SendMessageW(hTray, WM_SETTINGCHANGE, 0, (LPARAM)L"TraySettings");
        }
    }

    g_taskbarPosHooksInstalled = false;

    if (g_hwndMain && IsWindow(g_hwndMain))
        PostMessageW(g_hwndMain, WM_CLOSE, 0, 0);
    if (g_hwndStartCustom && IsWindow(g_hwndStartCustom))
        PostMessageW(g_hwndStartCustom, WM_CLOSE, 0, 0);

    if (g_dialogThread) {
        WaitForSingleObject(g_dialogThread, 1500);
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
    InitLocalization();
}
