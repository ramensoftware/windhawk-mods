// ==WindhawkMod==
// @id              win10-taskbar-24h2-26100
// @name            Classic (Win32) taskbar for Win11 24H2+ (all in one)
// @description     Installs Win10-style taskbar version 26100.1 (Win 11 24H2 RTM) and fixes its functionality
// @version         1.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @include         sndvol.exe
// @include         userinit.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32 -luser32 -lshell32 -lole32 -luxtheme -lurlmon -lbcrypt
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic (Win32) Taskbar for Win11 24H2+ (All-in-One)

This mod downloads the Windows 10-style taskbar from Microsoft's symbol server and installs it directly into the mod's storage space. **No system files are modified.** 

The version of Explorer used here is sourced from Windows 11 24H2 RTM (build 26100.1). Because Microsoft removed the system tray from this version, this mod completely reimplements it from scratch. To simplify installation, this is an "all-in-one" package that already incorporates the functionality of other mods required to make the Win10-style taskbar work correctly.

While similar to [Win10 taskbar on Win11 24H2 or later](https://windhawk.net/mods/win10-taskbar-on-win11-24h2), this mod features the following differences:
* It uses the actual Windows 11 24H2 RTM (Germanium) `explorer.exe`.
* It is an all-in-one solution (all necessary supporting mods are built-in).
* UWP apps display normally on the taskbar alongside Win32 applications.
* The system tray is completely custom-built, meaning other mods targeting the notification area **will not work**.
* The language indicator is currently unavailable, but will be added in a future update.

To restore the network icon, you can install the [Restore legacy network tray](https://windhawk.net/mods/restore-legacy-network-tray) mod, or use any other network indicator of your choice.

**Important Notes:**
* **Internet access is required** during the mod's initial launch.
* To restart Explorer, you must run `userinit.exe` instead of `explorer.exe`.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- trayEnabled: true
  $name: Enable Custom Win32 System Tray & Clock
- showActionCenterIcon: true
  $name: Show Action Center icon in System Tray
- showSeconds: false
  $name: Show Seconds in Tray Clock
- hoverEffect: false
  $name: Highlight icons/clock under the mouse
- recolorIcons: true
  $name: Recolor white monochrome icons on light background
- legacyPowerFlyout: true
  $name: Legacy (Win32) power/battery flyout
- quickSettingsFallback: true
  $name: Open Quick Settings for the power/battery icon (only when the legacy power flyout is off)
- showDesktopButton: false
  $name: Show the "Show desktop" button (separator and area right of the clock)
- iconSizePx: 16
  $name: Tray Icon Size (base pixels at 96 DPI)
- iconPaddingPx: 2
  $name: Tray Icon Padding (base pixels at 96 DPI, total space between icons)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <uxtheme.h>
#include <bcrypt.h>
#include <urlmon.h>
#include <algorithm>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>
#include <unknwn.h>
#include <objbase.h>
#include <string>
#include <vector>
#include <mutex>

// ============================================================================
// Globals & Settings
// ============================================================================
enum class ProcessType { Explorer, SndVol, Userinit };
static ProcessType g_processType = ProcessType::Explorer;

struct ModSettings {
    bool trayEnabled = true, showActionCenterIcon = true, showSeconds = false;
    bool hoverEffect = false, recolorIcons = true, legacyPowerFlyout = true;
    bool quickSettingsFallback = true, showDesktopButton = false;
    int iconSizePx = 16, iconPaddingPx = 2;
} g_settings;

static HMODULE g_hExplorer = nullptr, g_hShell32 = nullptr, g_hBthprops = nullptr, g_hExplorerFrame = nullptr;
static volatile LONG g_stopThread = 0;
static HANDLE g_hInitThread = nullptr;

static bool IsZeroGuid(const GUID& g) {
    static constexpr GUID kZero = {};
    return memcmp(&g, &kZero, sizeof(GUID)) == 0;
}
static bool IsSameGuid(const GUID& a, const GUID& b) { return memcmp(&a, &b, sizeof(GUID)) == 0; }

static constexpr GUID kGuidVolume       = {0x7820ae73, 0x23e3, 0x4229, {0x82, 0xc1, 0xe4, 0x1c, 0xb6, 0x7d, 0x5b, 0x9c}};
static constexpr GUID kGuidPower        = {0x7820ae75, 0x23e3, 0x4229, {0x82, 0xc1, 0xe4, 0x1c, 0xb6, 0x7d, 0x5b, 0x9c}};
static constexpr GUID kGuidActionCenter = {0x7820ae81, 0x23e3, 0x4229, {0x82, 0xc1, 0xe4, 0x1c, 0xb6, 0x7d, 0x5b, 0x9c}};

// ============================================================================
// Part 0: userinit.exe (Download & Verify) & Fake Explorer Path
// ============================================================================
static constexpr wchar_t kExplorerSha256[] = L"d1a80fbac49a3d561a7942b21017538e1ba7795cfa5f75d6777f7a5d656ea58f";
static constexpr wchar_t kExplorerUrl[]    = L"https://msdl.microsoft.com/download/symbols/explorer.exe/9DA063643ee000/explorer.exe";

static HANDLE g_explorerFile = INVALID_HANDLE_VALUE;
static std::once_flag g_setupFlag;
static std::wstring g_shellPath;

static bool VerifySha256(HANDLE hFile) {
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    BCRYPT_HASH_HANDLE hHash = nullptr;
    if (BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) != 0) return false;

    DWORD objLen = 0, cb = 0;
    BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PBYTE)&objLen, sizeof(objLen), &cb, 0);
    std::vector<BYTE> obj(objLen);
    BYTE hash[32] = {}, buf[65536];
    bool ok = false;

    if (objLen && BCryptCreateHash(hAlg, &hHash, obj.data(), objLen, nullptr, 0, 0) == 0) {
        SetFilePointerEx(hFile, {}, nullptr, FILE_BEGIN);
        bool rOk = true;
        while (ReadFile(hFile, buf, sizeof(buf), &cb, nullptr) && cb > 0)
            if (BCryptHashData(hHash, buf, cb, 0) != 0) { rOk = false; break; }
        ok = rOk && BCryptFinishHash(hHash, hash, 32, 0) == 0;
        BCryptDestroyHash(hHash);
    }
    BCryptCloseAlgorithmProvider(hAlg, 0);
    if (!ok) return false;

    static const wchar_t kHex[] = L"0123456789abcdef";
    wchar_t hex[65];
    for (int i = 0; i < 32; ++i) {
        hex[i * 2]     = kHex[hash[i] >> 4];
        hex[i * 2 + 1] = kHex[hash[i] & 0xF];
    }
    hex[64] = 0;
    return _wcsicmp(hex, kExplorerSha256) == 0;
}

static void EnsureShellReadyImpl() {
    WCHAR dir[MAX_PATH];
    if (!Wh_GetModStoragePath(dir, MAX_PATH)) return;
    std::wstring sDir = dir, finalPath = sDir + L"\\explorer.exe", tmpPath = sDir + L"\\explorer.exe.download";

    auto openAndVerify = [](const std::wstring& p, DWORD access) -> HANDLE {
        HANDLE h = CreateFileW(p.c_str(), access, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h != INVALID_HANDLE_VALUE && !VerifySha256(h)) { CloseHandle(h); h = INVALID_HANDLE_VALUE; }
        return h;
    };

    HANDLE hFile = openAndVerify(finalPath, GENERIC_READ);
    if (hFile == INVALID_HANDLE_VALUE) {
        DeleteFileW(tmpPath.c_str());
        if (SUCCEEDED(URLDownloadToFileW(nullptr, kExplorerUrl, tmpPath.c_str(), 0, nullptr))) {
            hFile = openAndVerify(tmpPath, GENERIC_READ | DELETE);
            if (hFile != INVALID_HANDLE_VALUE) {
                size_t nb = finalPath.size() * sizeof(WCHAR);
                std::vector<BYTE> rb(sizeof(FILE_RENAME_INFO) + nb);
                auto* ri = reinterpret_cast<FILE_RENAME_INFO*>(rb.data());
                ri->ReplaceIfExists = TRUE;
                ri->FileNameLength = (DWORD)nb;
                memcpy(ri->FileName, finalPath.c_str(), nb);
                if (!SetFileInformationByHandle(hFile, FileRenameInfo, ri, (DWORD)rb.size())) {
                    CloseHandle(hFile);
                    hFile = INVALID_HANDLE_VALUE;
                }
            }
        }
        DeleteFileW(tmpPath.c_str());
    }
    if (hFile == INVALID_HANDLE_VALUE) return;

    g_explorerFile = hFile;
    g_shellPath = finalPath;

    WCHAR loc[LOCALE_NAME_MAX_LENGTH], sysRoot[MAX_PATH];
    if (GetUserDefaultLocaleName(loc, ARRAYSIZE(loc)) && ExpandEnvironmentStringsW(L"%systemroot%", sysRoot, MAX_PATH)) {
        std::wstring locDir = sDir + L"\\" + loc;
        CreateDirectoryW(locDir.c_str(), nullptr);
        CopyFileW((std::wstring(sysRoot) + L"\\" + loc + L"\\explorer.exe.mui").c_str(),
                  (locDir + L"\\explorer.exe.mui").c_str(), TRUE);
    }
}

using GetModuleFileNameW_t = decltype(&GetModuleFileNameW);
static GetModuleFileNameW_t g_origGetModuleFileNameW = nullptr;

static DWORD WINAPI GetModuleFileNameW_Hook(HMODULE hModule, LPWSTR lpFilename, DWORD nSize) {
    if (!hModule) {
        WCHAR sysRoot[MAX_PATH];
        GetEnvironmentVariableW(L"SystemRoot", sysRoot, MAX_PATH);
        return (DWORD)wsprintfW(lpFilename, L"%s\\explorer.exe", sysRoot);
    }
    return g_origGetModuleFileNameW(hModule, lpFilename, nSize);
}

using RegQueryValueExW_t = decltype(&RegQueryValueExW);
static RegQueryValueExW_t g_origRegQueryValueExW = nullptr;

static LONG WINAPI RegQueryValueExW_Hook(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType,
                                         LPBYTE lpData, LPDWORD lpcbData) {
    if (g_processType == ProcessType::Userinit) {
        if (lpValueName && lstrcmpiW(lpValueName, L"Shell") == 0) {
            std::call_once(g_setupFlag, EnsureShellReadyImpl);
            if (!g_shellPath.empty()) {
                if (lpType) *lpType = REG_SZ;
                DWORD needed = (DWORD)((g_shellPath.size() + 1) * sizeof(WCHAR));
                if (!lpcbData) return ERROR_SUCCESS;
                if (!lpData) { *lpcbData = needed; return ERROR_SUCCESS; }
                if (*lpcbData < needed) { *lpcbData = needed; return ERROR_MORE_DATA; }
                memcpy(lpData, g_shellPath.c_str(), needed);
                *lpcbData = needed;
                return ERROR_SUCCESS;
            }
        }
    } else if (g_settings.legacyPowerFlyout && lpValueName && lstrcmpiW(lpValueName, L"UseWin32BatteryFlyout") == 0) {
        if (lpType) *lpType = REG_DWORD;
        if (lpData) {
            if (!lpcbData || *lpcbData < sizeof(DWORD)) {
                if (lpcbData) *lpcbData = sizeof(DWORD);
                return ERROR_MORE_DATA;
            }
            *(DWORD*)lpData = 1;
        }
        if (lpcbData) *lpcbData = sizeof(DWORD);
        return ERROR_SUCCESS;
    }
    return g_origRegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

// ============================================================================
// SndVol.exe flyout positioning hook
// ============================================================================
using SetWindowPos_t = decltype(&SetWindowPos);
static SetWindowPos_t g_origSetWindowPos = nullptr;

static bool IsSndVolFlyoutWnd(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return false;
    LONG style = GetWindowLongW(hWnd, GWL_STYLE);
    // Из-за тем оформления стандартные диалоги могут получать рамки и системные меню.
    // Ослабляем проверку: любой диалог не-потомок в sndvol - это флайаут
    if (style & WS_CHILD) return false;
    WCHAR cls[32] = {};
    return GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) && wcscmp(cls, L"#32770") == 0;
}

static bool CalcSndVolFlyoutPos(int w, int h, int* outX, int* outY) {
    if (w <= 0 || h <= 0) return false;
    HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hTray) return false;

    DWORD tick = (DWORD)(UINT_PTR)GetPropW(hTray, L"WH_VolSet");
    if (!tick || (GetTickCount() - tick > 5000)) return false;

    POINT pt = {(int)(INT_PTR)GetPropW(hTray, L"WH_VolX"), (int)(INT_PTR)GetPropW(hTray, L"WH_VolY")};
    HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfoW(hMon, &mi)) return false;
    const RECT &work = mi.rcWork, &mon = mi.rcMonitor;

    auto clampRange = [](int v, int lo, int hi) { return std::clamp(v, lo, std::max(lo, hi)); };
    RECT tr = {};
    if (GetWindowRect(hTray, &tr) &&
        MonitorFromPoint({(tr.left + tr.right) / 2, (tr.top + tr.bottom) / 2}, MONITOR_DEFAULTTONEAREST) == hMon) {
        if ((tr.bottom - tr.top) <= (tr.right - tr.left)) {
            bool top = (tr.top + tr.bottom) < (mon.top + mon.bottom);
            *outX = clampRange(pt.x - w / 2, work.left, work.right - w);
            *outY = top ? std::max(work.top, tr.bottom) : std::min(work.bottom, tr.top) - h;
        } else {
            bool left = (tr.left + tr.right) < (mon.left + mon.right);
            *outX = left ? std::max(work.left, tr.right) : std::min(work.right, tr.left) - w;
            *outY = clampRange(pt.y - h / 2, work.top, work.bottom - h);
        }
    } else {
        *outX = clampRange(pt.x - w / 2, work.left, work.right - w);
        *outY = work.bottom - h;
    }
    return true;
}

static BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
    if (!(uFlags & SWP_HIDEWINDOW) && IsSndVolFlyoutWnd(hWnd)) {
        RECT rc = {};
        GetWindowRect(hWnd, &rc);
        int w = (uFlags & SWP_NOSIZE) ? (rc.right - rc.left) : cx;
        int h = (uFlags & SWP_NOSIZE) ? (rc.bottom - rc.top) : cy;
        int nx, ny;
        if (CalcSndVolFlyoutPos(w, h, &nx, &ny)) {
            X = nx;
            Y = ny;
            uFlags &= ~SWP_NOMOVE;
        }
    }
    return g_origSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

// ============================================================================
// Part 1: Explorer symbol & Taskbar Context Menu Fix hooks
// ============================================================================
using Feature_DisableWin10Taskbar_IsEnabled_t = bool (__cdecl *)(void*);
static Feature_DisableWin10Taskbar_IsEnabled_t g_origDisableWin10Taskbar = nullptr;

using CTray_InitializeTrayUIComponent_t = HRESULT (__cdecl *)(void*);
static CTray_InitializeTrayUIComponent_t g_origInitializeTrayUIComponent = nullptr;

using TrayUI_CreateInstance_t = HRESULT (__cdecl *)(void*, const GUID*, void**);
static TrayUI_CreateInstance_t g_pTrayUI_CreateInstance = nullptr;

static void* g_pTrayStatic = nullptr;

static bool __cdecl Hook_Feature_DisableWin10Taskbar_IsEnabled(void*) { return false; }

static HRESULT __cdecl Hook_CTray_InitializeTrayUIComponent(void* pThis) {
    if (g_pTrayUI_CreateInstance && g_pTrayStatic) {
        BYTE* pStatic = (BYTE*)g_pTrayStatic;
        IUnknown** ppTrayUI = (IUnknown**)(pStatic + 0x928);
        if (*ppTrayUI) {
            (*ppTrayUI)->Release();
            *ppTrayUI = nullptr;
        }
        return g_pTrayUI_CreateInstance(pStatic + 0x38, nullptr, (void**)ppTrayUI);
    }
    return g_origInitializeTrayUIComponent ? g_origInitializeTrayUIComponent(pThis) : E_FAIL;
}

#define IDM_SHOWDESKTOP     0x197
#define IDM_TASKMANAGER     0x1A4
#define IDM_LOCKTASKBAR     0x1A8
#define IDM_SETTINGS        0x19D
#define IDM_LOCKTOOLBARS    41484
#define IDS_SHOWDESKTOP     10113
#define IDS_TASKMANAGER     24743
#define IDS_SETTINGS        2128

using LoadMenuW_t = decltype(&LoadMenuW);
static LoadMenuW_t g_origLoadMenuW = nullptr;

using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);
static TrackPopupMenuEx_t g_origTrackPopupMenuEx = nullptr;

static const WCHAR* LoadStr(HMODULE hMod, UINT id, WCHAR* buf, int size, const WCHAR* fallback) {
    return (hMod && LoadStringW(hMod, id, buf, size) > 0) ? buf : fallback;
}

static const WCHAR* GetLockToolbarsText(WCHAR* buf, int size) {
    HMODULE hEf = g_hExplorerFrame ? g_hExplorerFrame : (g_hExplorerFrame = GetModuleHandleW(L"explorerframe.dll"));
    HMENU hMenu = hEf ? g_origLoadMenuW(hEf, MAKEINTRESOURCEW(264)) : nullptr;
    if (!hMenu) return L"Lock the taskbar";
    const WCHAR* res = L"Lock the taskbar";
    for (int i = 0; i < GetMenuItemCount(hMenu); i++) {
        if (HMENU hSub = GetSubMenu(hMenu, i)) {
            for (int j = 0; j < GetMenuItemCount(hSub); j++) {
                WCHAR text[256] = {};
                MENUITEMINFOW mii = {sizeof(mii), MIIM_ID | MIIM_STRING};
                mii.dwTypeData = text;
                mii.cch = 255;
                if (GetMenuItemInfoW(hSub, j, TRUE, &mii) && mii.wID == IDM_LOCKTOOLBARS) {
                    wcsncpy_s(buf, size, text, _TRUNCATE);
                    res = buf;
                    i = GetMenuItemCount(hMenu);
                    break;
                }
            }
        }
    }
    DestroyMenu(hMenu);
    return res;
}

static void EnhanceTaskbarMenu(HMENU hMenu) {
    HMENU hPopup = GetSubMenu(hMenu, 0);
    if (!hPopup) hPopup = hMenu;

    for (int i = GetMenuItemCount(hPopup) - 1; i >= 0; i--) {
        MENUITEMINFOW mii = {sizeof(mii), MIIM_FTYPE | MIIM_ID | MIIM_SUBMENU};
        if (GetMenuItemInfoW(hPopup, i, TRUE, &mii) && !(mii.fType & MFT_SEPARATOR) && !mii.hSubMenu)
            DeleteMenu(hPopup, i, MF_BYPOSITION);
    }

    WCHAR buf[256];
    AppendMenuW(hPopup, MF_STRING, IDM_SHOWDESKTOP, LoadStr(g_hShell32, IDS_SHOWDESKTOP, buf, 256, L"Show the desktop"));
    AppendMenuW(hPopup, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hPopup, MF_STRING, IDM_TASKMANAGER, LoadStr(g_hShell32, IDS_TASKMANAGER, buf, 256, L"Task Manager"));
    AppendMenuW(hPopup, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hPopup, MF_STRING, IDM_LOCKTASKBAR, GetLockToolbarsText(buf, 256));
    AppendMenuW(hPopup, MF_STRING, IDM_SETTINGS, LoadStr(g_hBthprops, IDS_SETTINGS, buf, 256, L"Taskbar settings"));
}

static HMENU WINAPI LoadMenuW_Hook(HINSTANCE hInstance, LPCWSTR lpMenuName) {
    if (IS_INTRESOURCE(lpMenuName) && (HMODULE)hInstance == g_hExplorer) {
        UINT id = (UINT)(ULONG_PTR)lpMenuName;
        if ((id == 205 || id == 206) && g_hShell32) {
            if (HMENU res = g_origLoadMenuW(g_hShell32, MAKEINTRESOURCEW(205))) {
                EnhanceTaskbarMenu(res);
                return res;
            }
        }
    }
    return g_origLoadMenuW(hInstance, lpMenuName);
}

static BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm) {
    BOOL ret = g_origTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);
    if (ret >= IDM_TASKMANAGER && (uFlags & TPM_RETURNCMD)) {
        WCHAR cls[32] = {};
        HWND root = GetAncestor(hWnd, GA_ROOT);
        if (root && GetClassNameW(root, cls, ARRAYSIZE(cls)) &&
            (_wcsicmp(cls, L"Shell_TrayWnd") == 0 || _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0) &&
            GetMenuState(hMenu, ret - IDM_TASKMANAGER + IDM_SHOWDESKTOP, MF_BYCOMMAND) != (UINT)-1 &&
            GetMenuState(hMenu, ret - IDM_TASKMANAGER + IDM_SETTINGS, MF_BYCOMMAND) != (UINT)-1) {
            WCHAR itemText[256], tmText[256];
            const WCHAR* expected = LoadStr(g_hShell32, IDS_TASKMANAGER, tmText, 256, L"Task Manager");
            if (GetMenuStringW(hMenu, ret, itemText, 256, MF_BYCOMMAND) > 0 && wcscmp(itemText, expected) == 0) {
                WCHAR path[MAX_PATH];
                GetSystemDirectoryW(path, ARRAYSIZE(path));
                wcscat_s(path, L"\\Taskmgr.exe");
                ShellExecuteW(nullptr, nullptr, path, nullptr, nullptr, SW_SHOWNORMAL);
                return 0;
            }
        }
    }
    return ret;
}

// ============================================================================
// Part 2: Shell_NotifyIcon protocol, Icon Persistence & Helpers
// ============================================================================
#pragma pack(push, 1)
struct NOTIFYICONDATA32W {
    DWORD cbSize, dwWnd;
    UINT uID, uFlags, uCallbackMessage;
    DWORD dwIcon;
    WCHAR szTip[128];
    DWORD dwState, dwStateMask;
    WCHAR szInfo[256];
    union { UINT uTimeout; UINT uVersion; };
    WCHAR szInfoTitle[64];
    DWORD dwInfoFlags;
    GUID guidItem;
    DWORD dwBalloonIcon;
};
struct TRAYNOTIFYDATAW {
    DWORD dwSignature, dwMessage;
    NOTIFYICONDATA32W nid;
};
#pragma pack(pop)

static constexpr DWORD TRAY_SIGNATURE = 0x34753423;
static constexpr UINT WM_MOD_INIT_TRAY = WM_USER + 0x2610;
static constexpr UINT WM_MOD_UNINIT_TRAY = WM_USER + 0x2611;
static constexpr UINT WM_MOD_RELAYOUT = WM_USER + 0x2612;
static constexpr UINT_PTR TIMER_ID_CLOCK = 0x26100;

struct TrayIconEntry {
    HWND hWnd = nullptr;
    UINT uID = 0, uCallbackMessage = 0, uVersion = 0;
    GUID guidItem = {};
    bool hasGuid = false;
    std::wstring persistKey;
    HICON hIcon = nullptr, hTinted = nullptr;
    COLORREF tintFor = 0;
    int monoKind = -1;
    DWORD dwState = 0;
    WCHAR szTip[128] = {};
    RECT rcSlot = {};
};

static std::mutex g_trayMutex;
static std::vector<TrayIconEntry> g_trayIcons;
static std::vector<std::wstring> g_savedOrder;

static HWND g_hTrayWnd = nullptr, g_hNotifyWnd = nullptr, g_hReBarWnd = nullptr, g_hTooltipWnd = nullptr;
static bool g_traySubclassed = false, g_notifySubclassed = false, g_rebarSubclassed = false;
static bool g_weCreatedNotifyWnd = false, g_weRegisteredClass = false;

static int g_rebarAnchorX = 0, g_rebarAnchorY = 0;
static bool g_rebarAnchorInit = false;
static DWORD g_rebarAllowMoveUntil = 0;

static int g_hoveredIconIdx = -1, g_dragIdx = -1, g_dropIdx = -1;
static bool g_clockHovered = false, g_trackingMouse = false, g_dragging = false;
static RECT g_rcClock = {}, g_rcDesktop = {};
static POINT g_dragStart = {};

static LRESULT CALLBACK ReBarWndSubclassProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

static int ScaleForDpi(HWND hWnd, int px) {
    UINT dpi = hWnd ? GetDpiForWindow(hWnd) : 96;
    return MulDiv(px, dpi ? (int)dpi : 96, 96);
}

static bool IsEntryVisible(const TrayIconEntry& e) { return !(e.dwState & NIS_HIDDEN) && e.hIcon; }

static void FreeEntryIcons(TrayIconEntry& e) {
    if (e.hIcon) { DestroyIcon(e.hIcon); e.hIcon = nullptr; }
    if (e.hTinted) { DestroyIcon(e.hTinted); e.hTinted = nullptr; }
}

static COLORREF MixColor(COLORREF a, COLORREF b, int pctB) {
    return RGB((GetRValue(a) * (100 - pctB) + GetRValue(b) * pctB) / 100,
               (GetGValue(a) * (100 - pctB) + GetGValue(b) * pctB) / 100,
               (GetBValue(a) * (100 - pctB) + GetBValue(b) * pctB) / 100);
}

static int Luma(COLORREF c) { return (299 * GetRValue(c) + 587 * GetGValue(c) + 114 * GetBValue(c)) / 1000; }

static void FillRectColor(HDC dc, const RECT& rc, COLORREF c) {
    HBRUSH br = CreateSolidBrush(c);
    FillRect(dc, &rc, br);
    DeleteObject(br);
}

static HFONT CreateTrayFont(HWND hWnd) {
    NONCLIENTMETRICSW ncm = {sizeof(ncm)};
    UINT dpi = hWnd ? GetDpiForWindow(hWnd) : 96;
    if (SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0, dpi ? dpi : 96))
        return CreateFontIndirectW(&ncm.lfMessageFont);
    return (HFONT)GetStockObject(DEFAULT_GUI_FONT);
}

static void GetClockStrings(WCHAR* time, int tn, WCHAR* date, int dn, WCHAR* longDate, int ln) {
    SYSTEMTIME st = {};
    GetLocalTime(&st);
    GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, g_settings.showSeconds ? 0 : TIME_NOSECONDS, &st, nullptr, time, tn);
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &st, nullptr, date, dn, nullptr);
    if (longDate) GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_LONGDATE, &st, nullptr, longDate, ln, nullptr);
}

static void OpenFlyoutUri(const WCHAR* uri) { ShellExecuteW(nullptr, L"open", uri, nullptr, nullptr, SW_SHOWNORMAL); }

static void InitTopDownBI(BITMAPINFO& bi, int w, int h) {
    bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
}

static HICON CreateActionCenterIcon() {
    const int sz = 32;
    BITMAPINFO bi;
    InitTopDownBI(bi, sz, sz);
    DWORD* bits = nullptr;
    HDC dc = CreateCompatibleDC(nullptr);
    HBITMAP hBmp = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, (void**)&bits, nullptr, 0);
    if (!hBmp || !bits) {
        if (hBmp) DeleteObject(hBmp);
        DeleteDC(dc);
        return nullptr;
    }
    HGDIOBJ oldBmp = SelectObject(dc, hBmp);
    RECT rc = {0, 0, sz, sz};
    FillRectColor(dc, rc, RGB(0, 0, 0));

    HFONT hFont = CreateFontW(-24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                              DEFAULT_PITCH | FF_DONTCARE, L"Segoe MDL2 Assets");
    HGDIOBJ oldFont = hFont ? SelectObject(dc, hFont) : nullptr;
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(255, 255, 255));
    WCHAR glyph[2] = {0xE91C, 0};
    DrawTextW(dc, glyph, 1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
    if (oldFont) SelectObject(dc, oldFont);
    if (hFont) DeleteObject(hFont);
    GdiFlush();

    bool anyPixel = false;
    for (int i = 0; i < sz * sz; ++i) {
        DWORD a = (bits[i] >> 16) & 0xFF;
        if (a) anyPixel = true;
        bits[i] = (a << 24) | 0x00FFFFFF;
    }
    if (!anyPixel) {
        for (int y = 5; y <= 22; ++y)
            for (int x = 4; x <= 27; ++x)
                if (y == 5 || y == 22 || x == 4 || x == 27 ||
                    ((y == 10 || y == 14 || y == 18) && x >= 8 && x <= 23))
                    bits[y * sz + x] = 0xFFFFFFFF;
        for (int i = 0; i < 5; ++i) {
            bits[(22 + i) * sz + 8] = 0xFFFFFFFF;
            bits[(26 - i) * sz + (8 + i)] = 0xFFFFFFFF;
        }
    }

    SelectObject(dc, oldBmp);
    DeleteDC(dc);

    HBITMAP hMask = CreateBitmap(sz, sz, 1, 1, nullptr);
    ICONINFO ii = {TRUE, 0, 0, hMask, hBmp};
    HICON hIcon = CreateIconIndirect(&ii);
    DeleteObject(hMask);
    DeleteObject(hBmp);
    return hIcon;
}

static std::wstring GetIconPersistKey(const TrayIconEntry& e) {
    WCHAR buf[128] = {};
    if (e.hasGuid && !IsZeroGuid(e.guidItem)) {
        StringFromGUID2(e.guidItem, buf, ARRAYSIZE(buf));
        return std::wstring(L"G:") + buf;
    }
    std::wstring exe = L"unknown";
    if (e.hWnd) {
        DWORD pid = 0;
        GetWindowThreadProcessId(e.hWnd, &pid);
        if (pid) {
            if (HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid)) {
                WCHAR path[MAX_PATH];
                DWORD sz = MAX_PATH;
                if (QueryFullProcessImageNameW(hProc, 0, path, &sz)) {
                    const WCHAR* name = wcsrchr(path, L'\\');
                    exe = name ? (name + 1) : path;
                    for (auto& ch : exe) ch = towlower(ch);
                }
                CloseHandle(hProc);
            }
        }
    }
    wsprintfW(buf, L":%u", e.uID);
    return L"P:" + exe + buf;
}

static void LoadSavedIconOrder() {
    std::lock_guard<std::mutex> lock(g_trayMutex);
    g_savedOrder.clear();
    WCHAR buf[8192] = {};
    if (Wh_GetStringValue(L"IconOrder", buf, ARRAYSIZE(buf)) > 0) {
        WCHAR* ctx = nullptr;
        for (WCHAR* tok = wcstok_s(buf, L";", &ctx); tok; tok = wcstok_s(nullptr, L";", &ctx))
            if (*tok) g_savedOrder.push_back(tok);
    }
}

static void SaveIconOrderLocked() {
    std::wstring s;
    for (const auto& k : g_savedOrder) {
        if (!k.empty()) {
            if (!s.empty()) s += L';';
            s += k;
        }
    }
    Wh_SetStringValue(L"IconOrder", s.c_str());
}

static int EnsureKeyInOrderLocked(const TrayIconEntry& e) {
    for (size_t i = 0; i < g_savedOrder.size(); ++i)
        if (g_savedOrder[i] == e.persistKey) return (int)i;

    // By default, keep Action Center at the far right when new icons are discovered
    if (!(e.hasGuid && IsSameGuid(e.guidItem, kGuidActionCenter)) && !g_savedOrder.empty()) {
        WCHAR acBuf[64] = {};
        StringFromGUID2(kGuidActionCenter, acBuf, ARRAYSIZE(acBuf));
        std::wstring acKey = std::wstring(L"G:") + acBuf;
        if (g_savedOrder.back() == acKey) {
            int pos = (int)g_savedOrder.size() - 1;
            g_savedOrder.insert(g_savedOrder.end() - 1, e.persistKey);
            SaveIconOrderLocked();
            return pos;
        }
    }
    g_savedOrder.push_back(e.persistKey);
    SaveIconOrderLocked();
    return (int)g_savedOrder.size() - 1;
}

static void InsertIconSortedLocked(const TrayIconEntry& e) {
    int rank = EnsureKeyInOrderLocked(e);
    auto it = g_trayIcons.begin();
    for (; it != g_trayIcons.end(); ++it)
        if (EnsureKeyInOrderLocked(*it) > rank) break;
    g_trayIcons.insert(it, e);
}

static int FindIconLocked(HWND hWnd, UINT uID, const GUID* guid) {
    if (guid && !IsZeroGuid(*guid)) {
        for (size_t i = 0; i < g_trayIcons.size(); ++i)
            if (g_trayIcons[i].hasGuid && IsSameGuid(g_trayIcons[i].guidItem, *guid)) return (int)i;
    }
    for (size_t i = 0; i < g_trayIcons.size(); ++i)
        if (g_trayIcons[i].hWnd == hWnd && g_trayIcons[i].uID == uID) return (int)i;
    return -1;
}

static void SyncActionCenterIconLocked() {
    int idx = FindIconLocked(nullptr, 0, &kGuidActionCenter);
    if (g_settings.showActionCenterIcon) {
        if (idx < 0) {
            TrayIconEntry e = {};
            e.guidItem = kGuidActionCenter;
            e.hasGuid = true;
            e.persistKey = GetIconPersistKey(e);
            e.hIcon = CreateActionCenterIcon();
            wcscpy_s(e.szTip, L"Action Center");
            InsertIconSortedLocked(e);
        } else {
            g_trayIcons[idx].dwState &= ~NIS_HIDDEN;
        }
    } else if (idx >= 0) {
        g_trayIcons[idx].dwState |= NIS_HIDDEN;
    }
}

static bool ReadIconColorBits(const ICONINFO& ii, std::vector<DWORD>& px, int* pw, int* ph) {
    BITMAP bm = {};
    if (!ii.hbmColor || !GetObjectW(ii.hbmColor, sizeof(bm), &bm)) return false;
    if (bm.bmBitsPixel != 32 || bm.bmWidth <= 0 || bm.bmHeight <= 0 || bm.bmWidth > 512 || bm.bmHeight > 512) return false;
    px.assign((size_t)bm.bmWidth * bm.bmHeight, 0);
    BITMAPINFO bi;
    InitTopDownBI(bi, bm.bmWidth, bm.bmHeight);
    HDC dc = GetDC(nullptr);
    int got = GetDIBits(dc, ii.hbmColor, 0, bm.bmHeight, px.data(), &bi, DIB_RGB_COLORS);
    ReleaseDC(nullptr, dc);
    if (got != bm.bmHeight) return false;
    *pw = bm.bmWidth;
    *ph = bm.bmHeight;
    return true;
}

static int ClassifyIcon(HICON hIcon) {
    ICONINFO ii = {};
    if (!GetIconInfo(hIcon, &ii)) return 0;
    int res = 0, w = 0, h = 0;
    std::vector<DWORD> px;
    if (ReadIconColorBits(ii, px, &w, &h)) {
        int sig = 0, white = 0;
        bool anyAlpha = false;
        for (DWORD p : px) {
            int a = p >> 24;
            if (a) anyAlpha = true;
            if (a < 32) continue;
            int b = p & 255, g = (p >> 8) & 255, r = (p >> 16) & 255;
            sig++;
            if (std::max({r, g, b}) - std::min({r, g, b}) > 40) continue;
            if (std::min(255, ((r * 299 + g * 587 + b * 114) / 1000) * 255 / a) >= 200) white++;
        }
        if (anyAlpha && sig > 0 && white * 100 >= sig * 90) res = 1;
    }
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask) DeleteObject(ii.hbmMask);
    return res;
}

static HICON MakeTintedIcon(HICON hIcon, COLORREF fg) {
    ICONINFO ii = {};
    if (!GetIconInfo(hIcon, &ii)) return nullptr;
    HICON result = nullptr;
    std::vector<DWORD> px;
    int w = 0, h = 0;
    if (ReadIconColorBits(ii, px, &w, &h)) {
        for (DWORD& p : px) {
            int a = p >> 24;
            if (!a) continue;
            int f = (((p >> 16) & 255) * 299 + ((p >> 8) & 255) * 587 + (p & 255) * 114) / 1000;
            p = ((DWORD)a << 24) | ((DWORD)(GetRValue(fg) * f / 255) << 16) |
                ((DWORD)(GetGValue(fg) * f / 255) << 8) | (DWORD)(GetBValue(fg) * f / 255);
        }
        BITMAPINFO bi;
        InitTopDownBI(bi, w, h);
        void* bits = nullptr;
        HBITMAP hNew = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (hNew && bits) {
            memcpy(bits, px.data(), px.size() * sizeof(DWORD));
            ICONINFO ii2 = {TRUE, 0, 0, ii.hbmMask, hNew};
            result = CreateIconIndirect(&ii2);
        }
        if (hNew) DeleteObject(hNew);
    }
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask) DeleteObject(ii.hbmMask);
    return result;
}

static int CountVisibleIcons() {
    std::lock_guard<std::mutex> lock(g_trayMutex);
    for (size_t i = 0; i < g_trayIcons.size();) {
        if (g_trayIcons[i].hWnd && !IsWindow(g_trayIcons[i].hWnd)) {
            FreeEntryIcons(g_trayIcons[i]);
            g_trayIcons.erase(g_trayIcons.begin() + i);
        } else {
            ++i;
        }
    }
    int n = 0;
    for (const auto& it : g_trayIcons) if (IsEntryVisible(it)) n++;
    return n;
}

// ============================================================================
// Layout
// ============================================================================
static bool IsTrayVertical() {
    if (!g_hTrayWnd || !IsWindow(g_hTrayWnd)) return false;
    RECT r = {};
    GetClientRect(g_hTrayWnd, &r);
    return (r.bottom - r.top) > (r.right - r.left);
}

static int DesktopBtnSize(HWND hWnd) { return g_settings.showDesktopButton ? ScaleForDpi(hWnd, 10) : 0; }

struct ClockInfo {
    WCHAR time[64], date[64];
    int timeW, dateW, lineH;
    bool twoLines;
};

static int ClockExtent(HWND hWnd, bool vertical, int w, int h, ClockInfo* out) {
    ClockInfo ci = {};
    GetClockStrings(ci.time, 64, ci.date, 64, nullptr, 0);
    HDC dc = GetDC(hWnd);
    HFONT f = CreateTrayFont(hWnd);
    HGDIOBJ old = SelectObject(dc, f);
    SIZE st = {}, sd = {};
    GetTextExtentPoint32W(dc, ci.time, (int)wcslen(ci.time), &st);
    GetTextExtentPoint32W(dc, ci.date, (int)wcslen(ci.date), &sd);
    TEXTMETRICW tm = {};
    GetTextMetricsW(dc, &tm);
    SelectObject(dc, old);
    DeleteObject(f);
    ReleaseDC(hWnd, dc);

    ci.timeW = st.cx;
    ci.dateW = sd.cx;
    ci.lineH = tm.tmHeight;
    ci.twoLines = !vertical ? (h >= ScaleForDpi(hWnd, 30)) : (sd.cx + ScaleForDpi(hWnd, 6) <= w);
    if (out) *out = ci;
    return !vertical ? (ci.twoLines ? std::max(ci.timeW, ci.dateW) : ci.timeW) + ScaleForDpi(hWnd, 16)
                     : ci.lineH * (ci.twoLines ? 2 : 1) + ScaleForDpi(hWnd, 8);
}

static int GridCols(HWND hWnd, int w, int slot) {
    return std::max(1, (w - 2 * ScaleForDpi(hWnd, 2)) / std::max(1, slot));
}

static int CalcDesiredTrayExtent(HWND hNotify, bool vertical, int trayW, int trayH) {
    int slotW = ScaleForDpi(hNotify, g_settings.iconSizePx) + ScaleForDpi(hNotify, g_settings.iconPaddingPx);
    int visible = CountVisibleIcons();
    int sd = DesktopBtnSize(hNotify);
    if (!vertical) return ScaleForDpi(hNotify, 6) + visible * slotW + ClockExtent(hNotify, false, trayW, trayH, nullptr) + sd;
    int cols = GridCols(hNotify, trayW, slotW);
    return ScaleForDpi(hNotify, 4) + ((visible + cols - 1) / cols) * slotW + ClockExtent(hNotify, true, trayW, trayH, nullptr) + sd;
}

static bool ComputeNotifyRect(RECT* rc, bool* pVert) {
    if (!g_hTrayWnd || !g_hNotifyWnd || !IsWindow(g_hTrayWnd) || !IsWindow(g_hNotifyWnd)) return false;
    RECT rt = {};
    GetClientRect(g_hTrayWnd, &rt);
    int W = rt.right - rt.left, H = rt.bottom - rt.top;
    if (W <= 0 || H <= 0) return false;
    bool vert = H > W;
    int n = std::min(CalcDesiredTrayExtent(g_hNotifyWnd, vert, W, H), (vert ? H : W) / 2);
    *rc = !vert ? RECT{W - n, 0, W, H} : RECT{0, H - n, W, H};
    *pVert = vert;
    return true;
}

static void EnsureRebarAnchor() {
    if (g_rebarAnchorInit || !g_hReBarWnd || !IsWindow(g_hReBarWnd) || !g_hTrayWnd || !IsWindow(g_hTrayWnd)) return;
    RECT rr = {};
    GetWindowRect(g_hReBarWnd, &rr);
    POINT p = {rr.left, rr.top};
    ScreenToClient(g_hTrayWnd, &p);
    g_rebarAnchorX = p.x;
    g_rebarAnchorY = p.y;
    g_rebarAnchorInit = true;
}

static void FitRebarRect(bool vert, const RECT& rn, int* x, int* y, int* cx, int* cy) {
    int W = rn.right, H = rn.bottom;
    if (g_hTrayWnd && IsWindow(g_hTrayWnd)) {
        RECT rt = {};
        GetClientRect(g_hTrayWnd, &rt);
        if (rt.right > rt.left && rt.bottom > rt.top) {
            W = rt.right - rt.left;
            H = rt.bottom - rt.top;
        }
    }
    *x = g_rebarAnchorInit ? g_rebarAnchorX : 0;
    *y = g_rebarAnchorInit ? g_rebarAnchorY : 0;
    *cx = std::max(0, (!vert ? (int)rn.left : W) - *x);
    *cy = std::max(0, (!vert ? H : (int)rn.top) - *y);
}

static void EnsureReBarSubclassedOnUIThread() {
    if (!g_hTrayWnd || !IsWindow(g_hTrayWnd)) return;
    if (!g_hReBarWnd || !IsWindow(g_hReBarWnd)) {
        g_hReBarWnd = FindWindowExW(g_hTrayWnd, nullptr, L"ReBarWindow32", nullptr);
        g_rebarSubclassed = false;
    }
    if (g_hReBarWnd && !g_rebarSubclassed && SetWindowSubclass(g_hReBarWnd, ReBarWndSubclassProc, 1, 0))
        g_rebarSubclassed = true;
    EnsureRebarAnchor();
}

static void Relayout() {
    if (!g_hTrayWnd || !IsWindow(g_hTrayWnd) || !g_hNotifyWnd || !IsWindow(g_hNotifyWnd)) return;
    EnsureReBarSubclassedOnUIThread();

    RECT rn = {};
    bool vert = false;
    if (!ComputeNotifyRect(&rn, &vert)) return;

    int nw = rn.right - rn.left, nh = rn.bottom - rn.top;
    int rx = 0, ry = 0, rcx = 0, rcy = 0;
    bool haveRebar = g_hReBarWnd && IsWindow(g_hReBarWnd);
    if (haveRebar) FitRebarRect(vert, rn, &rx, &ry, &rcx, &rcy);

    RECT rcCur = {};
    GetWindowRect(g_hNotifyWnd, &rcCur);
    POINT pc = {rcCur.left, rcCur.top};
    ScreenToClient(g_hTrayWnd, &pc);
    bool needNotify = (pc.x != rn.left || pc.y != rn.top || rcCur.right - rcCur.left != nw || rcCur.bottom - rcCur.top != nh);

    bool needRebar = false;
    if (haveRebar) {
        RECT rr = {};
        GetWindowRect(g_hReBarWnd, &rr);
        POINT pr = {rr.left, rr.top};
        ScreenToClient(g_hTrayWnd, &pr);
        needRebar = (pr.x != rx || pr.y != ry || rr.right - rr.left != rcx || rr.bottom - rr.top != rcy);
    }

    if (needNotify && needRebar) {
        HDWP dwp = BeginDeferWindowPos(2);
        if (dwp) dwp = DeferWindowPos(dwp, g_hNotifyWnd, HWND_TOP, rn.left, rn.top, nw, nh, SWP_NOACTIVATE | SWP_SHOWWINDOW);
        if (dwp) dwp = DeferWindowPos(dwp, g_hReBarWnd, nullptr, rx, ry, rcx, rcy, SWP_NOZORDER | SWP_NOACTIVATE);
        if (dwp) EndDeferWindowPos(dwp);
        else {
            SetWindowPos(g_hNotifyWnd, HWND_TOP, rn.left, rn.top, nw, nh, SWP_NOACTIVATE | SWP_SHOWWINDOW);
            SetWindowPos(g_hReBarWnd, nullptr, rx, ry, rcx, rcy, SWP_NOZORDER | SWP_NOACTIVATE);
        }
    } else if (needNotify) {
        SetWindowPos(g_hNotifyWnd, HWND_TOP, rn.left, rn.top, nw, nh, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    } else if (needRebar) {
        SetWindowPos(g_hReBarWnd, nullptr, rx, ry, rcx, rcy, SWP_NOZORDER | SWP_NOACTIVATE);
    }

    InvalidateRect(g_hNotifyWnd, nullptr, FALSE);
}

static void ScheduleClockTick(HWND hWnd) {
    SYSTEMTIME st = {};
    GetLocalTime(&st);
    UINT ms = 1000 - st.wMilliseconds + (!g_settings.showSeconds ? (59 - st.wSecond) * 1000 : 0);
    SetTimer(hWnd, TIMER_ID_CLOCK, ms + 20, nullptr);
}

static void ApplyNotifyIconDataLocked(TrayIconEntry& e, const NOTIFYICONDATA32W& nid) {
    if (HWND hTarget = (HWND)ULongToHandle(nid.dwWnd)) e.hWnd = hTarget;
    e.uID = nid.uID;
    if ((nid.uFlags & NIF_GUID) && !IsZeroGuid(nid.guidItem)) {
        e.guidItem = nid.guidItem;
        e.hasGuid = true;
    }
    if (e.persistKey.empty() || ((nid.uFlags & NIF_GUID) && e.hasGuid))
        e.persistKey = GetIconPersistKey(e);
    if (nid.uFlags & NIF_MESSAGE) e.uCallbackMessage = nid.uCallbackMessage;
    if (nid.uFlags & NIF_ICON) {
        if (HICON src = (HICON)LongToHandle((LONG)nid.dwIcon)) {
            if (HICON copy = CopyIcon(src)) {
                FreeEntryIcons(e);
                e.hIcon = copy;
                e.monoKind = -1;
            }
        }
    }
    if (nid.uFlags & NIF_TIP) wcsncpy_s(e.szTip, nid.szTip, _TRUNCATE);
    if (nid.uFlags & NIF_STATE) e.dwState = (e.dwState & ~nid.dwStateMask) | (nid.dwState & nid.dwStateMask);
}

static BOOL HandleTrayCopyData(PCOPYDATASTRUCT pcds) {
    if (!pcds || pcds->dwData != 1 || !pcds->lpData || pcds->cbData < sizeof(DWORD) * 2) return FALSE;
    const auto* p = reinterpret_cast<const TRAYNOTIFYDATAW*>(pcds->lpData);
    if (p->dwSignature != TRAY_SIGNATURE) return FALSE;

    const NOTIFYICONDATA32W& nid = p->nid;
    BOOL handled = FALSE;
    {
        std::lock_guard<std::mutex> lock(g_trayMutex);
        int idx = FindIconLocked((HWND)ULongToHandle(nid.dwWnd), nid.uID, (nid.uFlags & NIF_GUID) ? &nid.guidItem : nullptr);
        switch (p->dwMessage) {
            case NIM_ADD:
            case NIM_MODIFY:
                if (idx >= 0) ApplyNotifyIconDataLocked(g_trayIcons[idx], nid);
                else {
                    TrayIconEntry e = {};
                    ApplyNotifyIconDataLocked(e, nid);
                    InsertIconSortedLocked(e);
                }
                handled = TRUE;
                break;
            case NIM_DELETE:
                if (idx >= 0) {
                    FreeEntryIcons(g_trayIcons[idx]);
                    g_trayIcons.erase(g_trayIcons.begin() + idx);
                    handled = TRUE;
                }
                break;
            case NIM_SETVERSION:
                if (idx >= 0) {
                    g_trayIcons[idx].uVersion = nid.uVersion;
                    handled = TRUE;
                }
                break;
            case NIM_SETFOCUS:
                handled = TRUE;
                break;
        }
    }
    if (handled && g_hNotifyWnd && IsWindow(g_hNotifyWnd)) Relayout();
    return handled;
}

// ============================================================================
// Flyout positioning & Mouse forwarding
// ============================================================================
static bool SlotToScreen(const RECT& slot, RECT* out) {
    if (IsRectEmpty(&slot) || !g_hNotifyWnd || !IsWindow(g_hNotifyWnd)) return false;
    *out = slot;
    MapWindowPoints(g_hNotifyWnd, nullptr, (POINT*)out, 2);

    // Центрируем и уменьшаем границы передаваемого прямоугольника (хитбокса) до 24px.
    // Из-за теней тем оформления панель задач Win11 имеет высокие невидимые границы,
    // отталкиваясь от которых флайауты (питание и т.д.) "висят в воздухе". Стягивая хитбокс
    // к иконкам, мы заставляем флайауты корректно привязываться вплотную.
    int cx = (out->left + out->right) / 2;
    int cy = (out->top + out->bottom) / 2;
    int s = ScaleForDpi(g_hNotifyWnd, 24);
    
    if ((out->bottom - out->top) > s) {
        out->top = cy - s / 2;
        out->bottom = cy + s / 2;
    }
    if ((out->right - out->left) > s) {
        out->left = cx - s / 2;
        out->right = cx + s / 2;
    }

    return true;
}

using Shell_NotifyIconGetRect_t = HRESULT (WINAPI *)(const NOTIFYICONIDENTIFIER*, RECT*);
static Shell_NotifyIconGetRect_t g_origGetRect = nullptr;

static HRESULT WINAPI Shell_NotifyIconGetRect_Hook(const NOTIFYICONIDENTIFIER* id, RECT* rc) {
    if (id && rc && id->cbSize >= sizeof(NOTIFYICONIDENTIFIER)) {
        RECT slot = {};
        {
            std::lock_guard<std::mutex> lock(g_trayMutex);
            int idx = FindIconLocked(id->hWnd, id->uID, &id->guidItem);
            if (idx >= 0) slot = g_trayIcons[idx].rcSlot;
        }
        if (SlotToScreen(slot, rc)) return S_OK;
    }
    return g_origGetRect ? g_origGetRect(id, rc) : E_FAIL;
}

static void UpdateTooltipText(const WCHAR* text) {
    if (!g_hTooltipWnd || !IsWindow(g_hTooltipWnd) || !g_hNotifyWnd) return;
    TOOLINFOW ti = {sizeof(ti), 0, g_hNotifyWnd, 1, {}, nullptr, const_cast<LPWSTR>(text ? text : L"")};
    SendMessageW(g_hTooltipWnd, TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);
    SendMessageW(g_hTooltipWnd, TTM_ACTIVATE, (text && text[0]) ? TRUE : FALSE, 0);
}

static DWORD WINAPI LaunchSndvolThread(LPVOID lpParam) {
    POINT pt = *(POINT*)lpParam;
    delete (POINT*)lpParam;

    WCHAR sys32[MAX_PATH] = {}, cmdLine[1024] = {};
    GetSystemDirectoryW(sys32, MAX_PATH);
    wsprintfW(cmdLine, L"\"%s\\SndVol.exe\" -f %u", sys32, (DWORD)MAKELONG((SHORT)pt.x, (SHORT)pt.y));

    STARTUPINFOW si = {sizeof(si)};
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION pi = {};
    if (CreateProcessW(nullptr, cmdLine, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    return 0;
}

static void LaunchClassicVolume(POINT anchor) {
    static DWORD lastTick = 0;
    DWORD now = GetTickCount();
    if (now - lastTick < 400) return;
    lastTick = now;
    if (g_hTrayWnd && IsWindow(g_hTrayWnd)) {
        SetPropW(g_hTrayWnd, L"WH_VolX", (HANDLE)(INT_PTR)anchor.x);
        SetPropW(g_hTrayWnd, L"WH_VolY", (HANDLE)(INT_PTR)anchor.y);
        SetPropW(g_hTrayWnd, L"WH_VolSet", (HANDLE)(UINT_PTR)(now ? now : 1));
    }
    POINT* p = new POINT(anchor);
    if (HANDLE hThread = CreateThread(nullptr, 0, LaunchSndvolThread, p, 0, nullptr)) CloseHandle(hThread);
    else delete p;
}

static const GUID kClsidWin32Clock = {0x0A323554A, 0x0FE1, 0x4E49, {0xae, 0xe1, 0x67, 0x22, 0x46, 0x5d, 0x79, 0x9f}};
static const GUID kIidWin32Clock   = {0x7A5FCA8A, 0x76B1, 0x44C8, {0xa9, 0x7c, 0xe7, 0x17, 0x3c, 0xca, 0x5f, 0x4f}};

struct IWin32Clock : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE ShowWin32Clock(HWND hWnd, LPRECT lpRect) = 0;
};

static void HandleClockClick(HWND hWnd) {
    if (FindWindowW(L"ClockFlyoutWindow", nullptr)) return;
    RECT rc = g_rcClock;
    MapWindowPoints(hWnd, nullptr, (POINT*)&rc, 2);

    // Стяжка хитбокса, как и в SlotToScreen, чтобы окно часов не висело в воздухе
    // из-за высоких невидимых теней тем оформления в Win11
    int cx = (rc.left + rc.right) / 2;
    int cy = (rc.top + rc.bottom) / 2;
    int s = ScaleForDpi(hWnd, 24);
    
    if ((rc.bottom - rc.top) > s) {
        rc.top = cy - s / 2;
        rc.bottom = cy + s / 2;
    }
    if ((rc.right - rc.left) > s) {
        rc.left = cx - s / 2;
        rc.right = cx + s / 2;
    }

    IWin32Clock* p = nullptr;
    if (SUCCEEDED(CoCreateInstance(kClsidWin32Clock, nullptr, CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER, kIidWin32Clock, (void**)&p)) && p) {
        p->ShowWin32Clock(hWnd, &rc);
        p->Release();
        return;
    }
    OpenFlyoutUri(L"ms-actioncenter:");
}

static BOOL SendIconCallback(HWND hTarget, UINT uCallback, UINT uID, UINT uVersion, UINT event, POINT ptScreen) {
    WPARAM wp = (uVersion >= NOTIFYICON_VERSION_4) ? MAKEWPARAM((WORD)ptScreen.x, (WORD)ptScreen.y) : (WPARAM)uID;
    LPARAM lp = (uVersion >= NOTIFYICON_VERSION_4) ? MAKELPARAM((WORD)event, (WORD)uID) : (LPARAM)event;
    return PostMessageW(hTarget, uCallback, wp, lp);
}

static void PostToIcon(int iconIdx, UINT mouseMsg, POINT ptScreen, bool withSelect) {
    HWND hTarget = nullptr;
    UINT uID = 0, uCallback = 0, uVersion = 0;
    GUID guid = {};
    RECT slot = {};
    {
        std::lock_guard<std::mutex> lock(g_trayMutex);
        if (iconIdx < 0 || iconIdx >= (int)g_trayIcons.size()) return;
        const auto& it = g_trayIcons[iconIdx];
        hTarget = it.hWnd;
        uID = it.uID;
        uCallback = it.uCallbackMessage;
        uVersion = it.uVersion;
        guid = it.guidItem;
        slot = it.rcSlot;
    }

    if (IsSameGuid(guid, kGuidActionCenter)) {
        if (mouseMsg == WM_LBUTTONUP) OpenFlyoutUri(L"ms-actioncenter:");
        return;
    }

    if (!hTarget || !IsWindow(hTarget)) return;

    DWORD pid = 0;
    GetWindowThreadProcessId(hTarget, &pid);
    bool inExplorer = (pid == GetCurrentProcessId());

    if ((mouseMsg == WM_LBUTTONDOWN || mouseMsg == WM_LBUTTONUP) && inExplorer && IsSameGuid(guid, kGuidVolume)) {
        if (mouseMsg == WM_LBUTTONUP) {
            POINT anchor = ptScreen;
            RECT sr = {};
            if (SlotToScreen(slot, &sr)) anchor = {(sr.left + sr.right) / 2, (sr.top + sr.bottom) / 2};
            LaunchClassicVolume(anchor);
        }
        return;
    }

    if (mouseMsg == WM_LBUTTONUP && inExplorer && !g_settings.legacyPowerFlyout &&
        g_settings.quickSettingsFallback && IsSameGuid(guid, kGuidPower)) {
        OpenFlyoutUri(L"ms-actioncenter:controlcenter/&showFooter=true");
        return;
    }

    if (!uCallback) return;
    if (mouseMsg != WM_MOUSEMOVE && pid) AllowSetForegroundWindow(pid);

    bool v4 = uVersion >= NOTIFYICON_VERSION_4;
    if (mouseMsg == WM_RBUTTONUP && withSelect) {
        if (inExplorer) {
            SendIconCallback(hTarget, uCallback, uID, uVersion, v4 ? WM_CONTEXTMENU : WM_RBUTTONUP, ptScreen);
        } else {
            SendIconCallback(hTarget, uCallback, uID, uVersion, WM_RBUTTONUP, ptScreen);
            if (v4) SendIconCallback(hTarget, uCallback, uID, uVersion, WM_CONTEXTMENU, ptScreen);
        }
    } else {
        SendIconCallback(hTarget, uCallback, uID, uVersion, mouseMsg, ptScreen);
        if (withSelect && mouseMsg == WM_LBUTTONUP && v4)
            SendIconCallback(hTarget, uCallback, uID, uVersion, NIN_SELECT, ptScreen);
    }
}

// ============================================================================
// Painting & Hit testing
// ============================================================================
static void DrawTrayBackground(HWND hWnd, HDC dc, const RECT& rc, COLORREF* pBg, COLORREF* pFg) {
    COLORREF bg = GetSysColor(COLOR_BTNFACE), fg = GetSysColor(COLOR_BTNTEXT);
    FillRectColor(dc, rc, bg);
    RECT rcCopy = rc;
    if (IsThemeActive() && SUCCEEDED(DrawThemeParentBackground(hWnd, dc, &rcCopy))) {
        COLORREF px = GetPixel(dc, rc.left + 1, (rc.top + rc.bottom) / 2);
        if (px != CLR_INVALID) {
            bg = px;
            fg = (Luma(px) < 140) ? RGB(255, 255, 255) : RGB(0, 0, 0);
        }
    }
    *pBg = bg;
    *pFg = fg;
}

static void PaintTrayNotifyWnd(HWND hWnd, HDC hdc) {
    RECT rc = {};
    GetClientRect(hWnd, &rc);
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0) return;

    bool vert = IsTrayVertical();
    HDC mem = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, w, h);
    HGDIOBJ oldBmp = SelectObject(mem, bmp);

    COLORREF bg, fg;
    DrawTrayBackground(hWnd, mem, rc, &bg, &fg);
    COLORREF hoverCol = MixColor(bg, fg, 18);
    bool darkText = Luma(fg) < 128;

    HFONT font = CreateTrayFont(hWnd);
    HGDIOBJ oldFont = SelectObject(mem, font);
    SetBkMode(mem, TRANSPARENT);
    SetTextColor(mem, fg);

    int iconSize = ScaleForDpi(hWnd, g_settings.iconSizePx);
    int slotW = iconSize + ScaleForDpi(hWnd, g_settings.iconPaddingPx);
    int sdSize = DesktopBtnSize(hWnd);
    int margin = ScaleForDpi(hWnd, vert ? 4 : 6);

    ClockInfo ci;
    int clockExt = ClockExtent(hWnd, vert, w, h, &ci);
    int cols = vert ? GridCols(hWnd, w, slotW) : 1;
    int gridX0 = vert ? (w - cols * slotW) / 2 : 0;
    int x = margin, visIdx = 0;

    bool haveDrop = false;
    int dropX = 0, dropT = 0, dropB = 0;
    {
        std::lock_guard<std::mutex> lock(g_trayMutex);
        for (size_t i = 0; i < g_trayIcons.size(); ++i) {
            auto& it = g_trayIcons[i];
            if (!IsEntryVisible(it)) {
                SetRectEmpty(&it.rcSlot);
                continue;
            }
            RECT slot = !vert ? RECT{x, 0, x + slotW, h}
                              : RECT{gridX0 + (visIdx % cols) * slotW, margin + (visIdx / cols) * slotW,
                                     gridX0 + (visIdx % cols + 1) * slotW, margin + (visIdx / cols + 1) * slotW};
            if (!vert) x += slotW;
            visIdx++;
            it.rcSlot = slot;

            if ((g_settings.hoverEffect && (int)i == g_hoveredIconIdx && !g_dragging) || (g_dragging && (int)i == g_dragIdx))
                FillRectColor(mem, slot, hoverCol);

            HICON toDraw = it.hIcon;
            if (g_settings.recolorIcons && darkText) {
                if (it.monoKind < 0) it.monoKind = ClassifyIcon(it.hIcon);
                if (it.monoKind == 1) {
                    if (!it.hTinted || it.tintFor != fg) {
                        if (it.hTinted) DestroyIcon(it.hTinted);
                        it.hTinted = MakeTintedIcon(it.hIcon, fg);
                        it.tintFor = fg;
                    }
                    if (it.hTinted) toDraw = it.hTinted;
                }
            }
            DrawIconEx(mem, slot.left + (slotW - iconSize) / 2, slot.top + ((slot.bottom - slot.top) - iconSize) / 2,
                       toDraw, iconSize, iconSize, 0, nullptr, DI_NORMAL);

            if (g_dragging && (int)i == g_dropIdx) {
                haveDrop = true;
                dropX = (g_dropIdx > g_dragIdx) ? slot.right : slot.left;
                dropT = slot.top;
                dropB = slot.bottom;
            }
        }
    }
    if (haveDrop) {
        int s2 = ScaleForDpi(hWnd, 2);
        FillRectColor(mem, {dropX - 1, dropT + s2, dropX + 1, dropB - s2}, fg);
    }

    SetRectEmpty(&g_rcDesktop);
    if (sdSize > 0) {
        g_rcDesktop = vert ? RECT{0, h - sdSize, w, h} : RECT{w - sdSize, 0, w, h};
        int s4 = ScaleForDpi(hWnd, 4);
        RECT sep = vert ? RECT{s4, g_rcDesktop.top, w - s4, g_rcDesktop.top + 1}
                        : RECT{g_rcDesktop.left, s4, g_rcDesktop.left + 1, h - s4};
        FillRectColor(mem, sep, MixColor(bg, fg, 35));
    }

    RECT rcClock = vert ? RECT{0, h - sdSize - clockExt, w, h - sdSize} : RECT{x, 0, w - sdSize, h};
    SetRectEmpty(&g_rcClock);
    if (rcClock.left < rcClock.right && rcClock.top < rcClock.bottom) {
        g_rcClock = rcClock;
        if (g_settings.hoverEffect && g_clockHovered) FillRectColor(mem, rcClock, hoverCol);

        int top = rcClock.top + ((rcClock.bottom - rcClock.top) - (ci.twoLines ? 2 : 1) * ci.lineH) / 2;
        RECT r1 = {rcClock.left, top, rcClock.right, top + ci.lineH};
        DrawTextW(mem, ci.time, -1, &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        if (ci.twoLines) {
            RECT r2 = {rcClock.left, top + ci.lineH, rcClock.right, top + 2 * ci.lineH};
            DrawTextW(mem, ci.date, -1, &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        }
    }

    SelectObject(mem, oldFont);
    DeleteObject(font);
    BitBlt(hdc, 0, 0, w, h, mem, 0, 0, SRCCOPY);
    SelectObject(mem, oldBmp);
    DeleteObject(bmp);
    DeleteDC(mem);
}

enum HitKind { HIT_NONE, HIT_ICON, HIT_CLOCK, HIT_DESKTOP };

static HitKind HitTest(POINT pt, int* pIdx, WCHAR* tip, int tipN) {
    *pIdx = -1;
    {
        std::lock_guard<std::mutex> lock(g_trayMutex);
        for (size_t i = 0; i < g_trayIcons.size(); ++i) {
            if (IsEntryVisible(g_trayIcons[i]) && PtInRect(&g_trayIcons[i].rcSlot, pt)) {
                *pIdx = (int)i;
                if (tip) wcsncpy_s(tip, tipN, g_trayIcons[i].szTip, _TRUNCATE);
                return HIT_ICON;
            }
        }
    }
    if (!IsRectEmpty(&g_rcClock) && PtInRect(&g_rcClock, pt)) return HIT_CLOCK;
    if (!IsRectEmpty(&g_rcDesktop) && PtInRect(&g_rcDesktop, pt)) return HIT_DESKTOP;
    return HIT_NONE;
}

static int ComputeDropIndex(POINT pt) {
    std::lock_guard<std::mutex> lock(g_trayMutex);
    int best = -1;
    long long bestD = (long long)1 << 62;
    for (size_t i = 0; i < g_trayIcons.size(); ++i) {
        if (!IsEntryVisible(g_trayIcons[i])) continue;
        const RECT& r = g_trayIcons[i].rcSlot;
        if (PtInRect(&r, pt)) return (int)i;
        long long dx = (long long)pt.x - (r.left + r.right) / 2, dy = (long long)pt.y - (r.top + r.bottom) / 2;
        if (dx * dx + dy * dy < bestD) {
            bestD = dx * dx + dy * dy;
            best = (int)i;
        }
    }
    return best;
}

static void ResetDrag(HWND hWnd) {
    if (g_dragIdx >= 0 && GetCapture() == hWnd) ReleaseCapture();
    g_dragIdx = g_dropIdx = -1;
    g_dragging = false;
}

static LRESULT HandleNotifyWndMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool isSubclass) {
    switch (uMsg) {
        case WM_ERASEBKGND:
            return 1;

        case WM_WINDOWPOSCHANGING: {
            WINDOWPOS* wp = reinterpret_cast<WINDOWPOS*>(lParam);
            RECT rn = {};
            bool vert = false;
            if (wp && hWnd == g_hNotifyWnd && !(wp->flags & SWP_HIDEWINDOW) &&
                !((wp->flags & SWP_NOSIZE) && (wp->flags & SWP_NOMOVE)) && ComputeNotifyRect(&rn, &vert)) {
                wp->flags &= ~(SWP_NOMOVE | SWP_NOSIZE);
                wp->x = rn.left;
                wp->y = rn.top;
                wp->cx = rn.right - rn.left;
                wp->cy = rn.bottom - rn.top;
            }
            break;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps = {};
            HDC hdc = BeginPaint(hWnd, &ps);
            PaintTrayNotifyWnd(hWnd, hdc);
            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_PRINTCLIENT:
            PaintTrayNotifyWnd(hWnd, (HDC)wParam);
            return 0;

        case WM_SIZE:
            if (g_hTooltipWnd) {
                TOOLINFOW ti = {sizeof(ti), 0, hWnd, 1};
                GetClientRect(hWnd, &ti.rect);
                SendMessageW(g_hTooltipWnd, TTM_NEWTOOLRECTW, 0, (LPARAM)&ti);
            }
            break;

        case WM_THEMECHANGED:
        case WM_SETTINGCHANGE:
        case WM_SYSCOLORCHANGE:
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            InvalidateRect(hWnd, nullptr, FALSE);
            break;

        case WM_MOD_RELAYOUT:
            ScheduleClockTick(hWnd);
            Relayout();
            return 0;

        case WM_TIMER:
            if (wParam == TIMER_ID_CLOCK) {
                ScheduleClockTick(hWnd);
                Relayout();
                return 0;
            }
            break;

        case WM_CAPTURECHANGED:
            if (g_dragIdx >= 0) {
                g_dragIdx = g_dropIdx = -1;
                g_dragging = false;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            break;

        case WM_MOUSEMOVE: {
            if (!g_trackingMouse) {
                TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hWnd, 0};
                TrackMouseEvent(&tme);
                g_trackingMouse = true;
            }
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

            if (g_dragIdx >= 0 && (wParam & MK_LBUTTON)) {
                if (!g_dragging && (abs(pt.x - g_dragStart.x) > GetSystemMetrics(SM_CXDRAG) ||
                                    abs(pt.y - g_dragStart.y) > GetSystemMetrics(SM_CYDRAG)))
                    g_dragging = true;
                if (g_dragging) {
                    g_dropIdx = ComputeDropIndex(pt);
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
                return 0;
            }

            WCHAR tip[160] = {};
            int idx = -1;
            HitKind k = HitTest(pt, &idx, tip, ARRAYSIZE(tip));
            bool nClock = (k == HIT_CLOCK);
            int nIcon = (k == HIT_ICON) ? idx : -1;
            if (nIcon != g_hoveredIconIdx || nClock != g_clockHovered) {
                g_hoveredIconIdx = nIcon;
                g_clockHovered = nClock;
                if (nClock) {
                    WCHAR t[64], d[64], ld[128];
                    GetClockStrings(t, 64, d, 64, ld, 128);
                    UpdateTooltipText(ld);
                } else {
                    UpdateTooltipText(nIcon >= 0 ? tip : L"");
                }
                if (g_settings.hoverEffect) InvalidateRect(hWnd, nullptr, FALSE);
            }
            if (nIcon >= 0) {
                POINT ps = pt;
                ClientToScreen(hWnd, &ps);
                PostToIcon(nIcon, WM_MOUSEMOVE, ps, false);
            }
            return 0;
        }

        case WM_MOUSELEAVE:
            g_trackingMouse = false;
            if (g_hoveredIconIdx != -1 || g_clockHovered) {
                g_hoveredIconIdx = -1;
                g_clockHovered = false;
                UpdateTooltipText(L"");
                if (g_settings.hoverEffect) InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;

        case WM_LBUTTONDOWN: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int idx;
            HitKind k = HitTest(pt, &idx, nullptr, 0);
            if (k == HIT_ICON) {
                g_dragIdx = idx;
                g_dragging = false;
                g_dragStart = pt;
                SetCapture(hWnd);
            } else if (k == HIT_CLOCK) {
                HandleClockClick(hWnd);
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}, ps = pt;
            ClientToScreen(hWnd, &ps);

            if (g_dragIdx >= 0) {
                int from = g_dragIdx, to = g_dropIdx;
                bool wasDrag = g_dragging;
                ResetDrag(hWnd);
                if (wasDrag) {
                    if (to >= 0 && to != from) {
                        std::lock_guard<std::mutex> lock(g_trayMutex);
                        if (from < (int)g_trayIcons.size() && to < (int)g_trayIcons.size()) {
                            std::wstring keyFrom = g_trayIcons[from].persistKey;
                            std::wstring keyTo = g_trayIcons[to].persistKey;
                            TrayIconEntry e = g_trayIcons[from];
                            g_trayIcons.erase(g_trayIcons.begin() + from);
                            g_trayIcons.insert(g_trayIcons.begin() + to, e);

                            if (!keyFrom.empty() && !keyTo.empty() && keyFrom != keyTo) {
                                g_savedOrder.erase(std::remove(g_savedOrder.begin(), g_savedOrder.end(), keyFrom), g_savedOrder.end());
                                auto itTo = std::find(g_savedOrder.begin(), g_savedOrder.end(), keyTo);
                                if (itTo != g_savedOrder.end()) {
                                    if (from < to) ++itTo;
                                    g_savedOrder.insert(itTo, keyFrom);
                                } else {
                                    g_savedOrder.push_back(keyFrom);
                                }
                                SaveIconOrderLocked();
                            }
                        }
                    }
                } else {
                    PostToIcon(from, WM_LBUTTONDOWN, ps, false);
                    PostToIcon(from, WM_LBUTTONUP, ps, true);
                }
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }

            int idx;
            HitKind k = HitTest(pt, &idx, nullptr, 0);
            if (k == HIT_ICON) PostToIcon(idx, WM_LBUTTONUP, ps, false);
            else if (k == HIT_DESKTOP && g_hTrayWnd) PostMessageW(g_hTrayWnd, WM_COMMAND, 419, 0);
            return 0;
        }

        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_RBUTTONDBLCLK: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}, ps = pt;
            ClientToScreen(hWnd, &ps);
            int idx;
            if (HitTest(pt, &idx, nullptr, 0) == HIT_ICON) PostToIcon(idx, uMsg, ps, false);
            return 0;
        }

        case WM_RBUTTONUP: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}, ps = pt;
            ClientToScreen(hWnd, &ps);
            int idx;
            if (HitTest(pt, &idx, nullptr, 0) == HIT_ICON) {
                PostToIcon(idx, WM_RBUTTONUP, ps, true);
                return 0;
            }
        }
    }
    return isSubclass ? DefSubclassProc(hWnd, uMsg, wParam, lParam) : DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

static LRESULT CALLBACK CustomNotifyWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return HandleNotifyWndMessage(hWnd, uMsg, wParam, lParam, false);
}

static LRESULT CALLBACK NotifyWndSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR) {
    if (uMsg == WM_NCDESTROY) {
        RemoveWindowSubclass(hWnd, NotifyWndSubclassProc, uIdSubclass);
        g_notifySubclassed = false;
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }
    return HandleNotifyWndMessage(hWnd, uMsg, wParam, lParam, true);
}

// ============================================================================
// Creation & Subclasses on the UI thread
// ============================================================================
static void CreateOrAttachNotifyWndOnUIThread(HWND hTrayWnd) {
    if (g_hNotifyWnd && IsWindow(g_hNotifyWnd)) return;

    if (HWND hExisting = FindWindowExW(hTrayWnd, nullptr, L"TrayNotifyWnd", nullptr)) {
        g_hNotifyWnd = hExisting;
        g_weCreatedNotifyWnd = false;
        if (SetWindowSubclass(hExisting, NotifyWndSubclassProc, 1, 0)) g_notifySubclassed = true;
    } else {
        WNDCLASSEXW wc = {sizeof(wc)};
        if (!GetClassInfoExW(g_hExplorer, L"TrayNotifyWnd", &wc)) {
            wc = {sizeof(wc), CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, CustomNotifyWndProc, 0, 0,
                  g_hExplorer, nullptr, LoadCursorW(nullptr, IDC_ARROW), nullptr, nullptr, L"TrayNotifyWnd", nullptr};
            if (RegisterClassExW(&wc)) g_weRegisteredClass = true;
        }
        g_hNotifyWnd = CreateWindowExW(0, L"TrayNotifyWnd", L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                                       0, 0, 120, 40, hTrayWnd, nullptr, g_hExplorer, nullptr);
        if (!g_hNotifyWnd) {
            if (g_weRegisteredClass) {
                UnregisterClassW(L"TrayNotifyWnd", g_hExplorer);
                g_weRegisteredClass = false;
            }
            return;
        }
        g_weCreatedNotifyWnd = true;
        if (SetWindowSubclass(g_hNotifyWnd, NotifyWndSubclassProc, 1, 0)) g_notifySubclassed = true;
    }

    g_hTooltipWnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, TOOLTIPS_CLASSW, nullptr,
                                    WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX, CW_USEDEFAULT, CW_USEDEFAULT,
                                    CW_USEDEFAULT, CW_USEDEFAULT, g_hNotifyWnd, nullptr, g_hExplorer, nullptr);
    if (g_hTooltipWnd) {
        TOOLINFOW ti = {sizeof(ti), TTF_SUBCLASS, g_hNotifyWnd, 1, {}, nullptr, const_cast<LPWSTR>(L"")};
        GetClientRect(g_hNotifyWnd, &ti.rect);
        SendMessageW(g_hTooltipWnd, TTM_ADDTOOLW, 0, (LPARAM)&ti);
    }

    ScheduleClockTick(g_hNotifyWnd);
    Relayout();

    if (UINT msg = RegisterWindowMessageW(L"TaskbarCreated"))
        SendNotifyMessageW(HWND_BROADCAST, msg, 0, 0);
}

static LRESULT CALLBACK ReBarWndSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR) {
    if (uMsg == WM_NCDESTROY) {
        RemoveWindowSubclass(hWnd, ReBarWndSubclassProc, uIdSubclass);
        g_rebarSubclassed = false;
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }
    if (uMsg == WM_WINDOWPOSCHANGING && g_hTrayWnd && IsWindow(g_hTrayWnd) && g_hNotifyWnd && IsWindow(g_hNotifyWnd)) {
        WINDOWPOS* wp = reinterpret_cast<WINDOWPOS*>(lParam);
        RECT rn = {};
        bool vert = false;
        if (wp && !(wp->flags & SWP_HIDEWINDOW) && !((wp->flags & SWP_NOSIZE) && (wp->flags & SWP_NOMOVE)) &&
            ComputeNotifyRect(&rn, &vert)) {
            RECT rr = {}, nr = {};
            GetWindowRect(hWnd, &rr);
            POINT p0 = {rr.left, rr.top};
            MapWindowPoints(nullptr, g_hTrayWnd, &p0, 1);
            GetWindowRect(g_hNotifyWnd, &nr);

            bool trayChanging = ((rn.right - rn.left) != (nr.right - nr.left)) || ((rn.bottom - rn.top) != (nr.bottom - nr.top));
            DWORD now = GetTickCount();
            bool forceAllow = !g_rebarAnchorInit || (g_rebarAllowMoveUntil != 0 && (INT32)(g_rebarAllowMoveUntil - now) >= 0);

            if (trayChanging && !forceAllow) {
                FitRebarRect(vert, rn, &wp->x, &wp->y, &wp->cx, &wp->cy);
                wp->flags = (wp->flags | SWP_NOMOVE) & ~SWP_NOSIZE;
            } else {
                int x = (wp->flags & SWP_NOMOVE) ? p0.x : wp->x;
                int y = (wp->flags & SWP_NOMOVE) ? p0.y : wp->y;
                RECT rt = {};
                GetClientRect(g_hTrayWnd, &rt);
                int W = (rt.right > rt.left) ? (rt.right - rt.left) : rn.right;
                int H = (rt.bottom > rt.top) ? (rt.bottom - rt.top) : rn.bottom;
                wp->flags &= ~(SWP_NOMOVE | SWP_NOSIZE);
                wp->x = g_rebarAnchorX = x;
                wp->y = g_rebarAnchorY = y;
                wp->cx = std::max(0, (!vert ? (int)rn.left : W) - x);
                wp->cy = std::max(0, (!vert ? H : (int)rn.top) - y);
                g_rebarAnchorInit = true;
            }
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static void UninitCustomTrayOnUIThread() {
    if (g_hTrayWnd && IsWindow(g_hTrayWnd)) {
        RemovePropW(g_hTrayWnd, L"WH_VolX");
        RemovePropW(g_hTrayWnd, L"WH_VolY");
        RemovePropW(g_hTrayWnd, L"WH_VolSet");
    }
    if (g_hTooltipWnd && IsWindow(g_hTooltipWnd)) DestroyWindow(g_hTooltipWnd);
    g_hTooltipWnd = nullptr;

    if (g_rebarSubclassed && g_hReBarWnd && IsWindow(g_hReBarWnd))
        RemoveWindowSubclass(g_hReBarWnd, ReBarWndSubclassProc, 1);
    g_rebarSubclassed = g_rebarAnchorInit = false;
    g_rebarAllowMoveUntil = 0;

    if (g_hNotifyWnd && IsWindow(g_hNotifyWnd)) {
        KillTimer(g_hNotifyWnd, TIMER_ID_CLOCK);
        if (g_notifySubclassed) RemoveWindowSubclass(g_hNotifyWnd, NotifyWndSubclassProc, 1);
        g_notifySubclassed = false;
        if (g_weCreatedNotifyWnd) DestroyWindow(g_hNotifyWnd);
        g_weCreatedNotifyWnd = false;
    }
    g_hNotifyWnd = nullptr;

    if (g_weRegisteredClass) {
        UnregisterClassW(L"TrayNotifyWnd", g_hExplorer);
        g_weRegisteredClass = false;
    }
}

static LRESULT CALLBACK TrayWndSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR) {
    if (uMsg == WM_NCDESTROY) {
        RemoveWindowSubclass(hWnd, TrayWndSubclassProc, uIdSubclass);
        g_traySubclassed = false;
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }
    if (uMsg == WM_MOD_UNINIT_TRAY) {
        UninitCustomTrayOnUIThread();
        RemoveWindowSubclass(hWnd, TrayWndSubclassProc, uIdSubclass);
        g_traySubclassed = false;
        return 0;
    }
    if (uMsg == WM_COPYDATA && HandleTrayCopyData(reinterpret_cast<PCOPYDATASTRUCT>(lParam))) return TRUE;
    if (uMsg == WM_SETTINGCHANGE && wParam == 0 && lParam && wcscmp((LPCWSTR)lParam, L"TraySettings") == 0) return 0;

    LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
    switch (uMsg) {
        case WM_SIZE:
        case WM_WINDOWPOSCHANGED:
        case WM_THEMECHANGED:
        case WM_SYSCOLORCHANGE:
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            Relayout();
            break;
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
            g_rebarAnchorInit = false;
            g_rebarAllowMoveUntil = GetTickCount() + 1500;
            Relayout();
            break;
        case WM_TIMECHANGE:
            if (g_hNotifyWnd && IsWindow(g_hNotifyWnd)) {
                ScheduleClockTick(g_hNotifyWnd);
                Relayout();
            }
            break;
        case WM_SETTINGCHANGE:
            g_rebarAllowMoveUntil = GetTickCount() + 1000;
            Relayout();
            break;
    }
    return res;
}

static void InitCustomTrayOnUIThread(HWND hTray) {
    if (g_traySubclassed || !SetWindowSubclass(hTray, TrayWndSubclassProc, 1, 0)) return;
    g_traySubclassed = true;
    EnsureReBarSubclassedOnUIThread();
    CreateOrAttachNotifyWndOnUIThread(hTray);
}

static LRESULT CALLBACK InitCallWndProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && lParam) {
        const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (cwp->hwnd == g_hTrayWnd && cwp->message == WM_MOD_INIT_TRAY) InitCustomTrayOnUIThread(g_hTrayWnd);
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static DWORD WINAPI TrayInitThread(LPVOID) {
    DWORD pid = GetCurrentProcessId();
    for (int i = 0; i < 150 && !g_stopThread; ++i) {
        if (!g_traySubclassed && g_settings.trayEnabled) {
            HWND hTray = nullptr;
            while ((hTray = FindWindowExW(nullptr, hTray, L"Shell_TrayWnd", nullptr)) != nullptr) {
                DWORD wndPid = 0, tid = GetWindowThreadProcessId(hTray, &wndPid);
                if (wndPid == pid && tid != 0) {
                    g_hTrayWnd = hTray;
                    if (HHOOK hk = SetWindowsHookExW(WH_CALLWNDPROC, InitCallWndProc, nullptr, tid)) {
                        SendMessageW(hTray, WM_MOD_INIT_TRAY, 0, 0);
                        UnhookWindowsHookEx(hk);
                    }
                    break;
                }
            }
        }
        if (g_traySubclassed && g_hNotifyWnd) break;
        Sleep(100);
    }
    return 0;
}

static void LoadSettings() {
    g_settings.trayEnabled = Wh_GetIntSetting(L"trayEnabled") != 0;
    g_settings.showActionCenterIcon = Wh_GetIntSetting(L"showActionCenterIcon") != 0;
    g_settings.showSeconds = Wh_GetIntSetting(L"showSeconds") != 0;
    g_settings.hoverEffect = Wh_GetIntSetting(L"hoverEffect") != 0;
    g_settings.recolorIcons = Wh_GetIntSetting(L"recolorIcons") != 0;
    g_settings.legacyPowerFlyout = Wh_GetIntSetting(L"legacyPowerFlyout") != 0;
    g_settings.quickSettingsFallback = Wh_GetIntSetting(L"quickSettingsFallback") != 0;
    g_settings.showDesktopButton = Wh_GetIntSetting(L"showDesktopButton") != 0;
    g_settings.iconSizePx = std::max(12, Wh_GetIntSetting(L"iconSizePx"));
    g_settings.iconPaddingPx = std::max(0, Wh_GetIntSetting(L"iconPaddingPx"));
}

// ============================================================================
// Windhawk entry points
// ============================================================================
BOOL Wh_ModInit() {
    WCHAR exePath[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    const WCHAR* exeName = wcsrchr(exePath, L'\\');
    exeName = exeName ? exeName + 1 : exePath;

    HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (!hKernelBase) hKernelBase = LoadLibraryW(L"kernelbase.dll");

    if (_wcsicmp(exeName, L"userinit.exe") == 0) {
        g_processType = ProcessType::Userinit;
        if (void* pRegQuery = hKernelBase ? (void*)GetProcAddress(hKernelBase, "RegQueryValueExW") : nullptr)
            Wh_SetFunctionHook(pRegQuery, (void*)RegQueryValueExW_Hook, (void**)&g_origRegQueryValueExW);
        return TRUE;
    }

    if (_wcsicmp(exeName, L"sndvol.exe") == 0) {
        g_processType = ProcessType::SndVol;
        Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPos_Hook, (void**)&g_origSetWindowPos);
        return TRUE;
    }

    g_processType = ProcessType::Explorer;
    LoadSettings();
    LoadSavedIconOrder();
    {
        std::lock_guard<std::mutex> lock(g_trayMutex);
        SyncActionCenterIconLocked();
    }

    g_hExplorer = GetModuleHandleW(nullptr);
    g_hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!g_hShell32) g_hShell32 = LoadLibraryW(L"shell32.dll");
    g_hBthprops = LoadLibraryExW(L"bthprops.cpl", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    g_hExplorerFrame = GetModuleHandleW(L"explorerframe.dll");

    // Fake Explorer path hook
    if (void* pGetModFn = hKernelBase ? (void*)GetProcAddress(hKernelBase, "GetModuleFileNameW") : nullptr)
        Wh_SetFunctionHook(pGetModFn, (void*)GetModuleFileNameW_Hook, (void**)&g_origGetModuleFileNameW);

    // Taskbar Context Menu Fix hooks
    Wh_SetFunctionHook((void*)LoadMenuW, (void*)LoadMenuW_Hook, (void**)&g_origLoadMenuW);
    Wh_SetFunctionHook((void*)TrackPopupMenuEx, (void*)TrackPopupMenuEx_Hook, (void**)&g_origTrackPopupMenuEx);

    WindhawkUtils::SYMBOL_HOOK explorerexe_hooks[] = {
        {{L"public: bool __cdecl wil::details::FeatureImpl<struct __WilFeatureTraits_Feature_DisableWin10Taskbar>::__private_IsEnabled(void)"},
         &g_origDisableWin10Taskbar, Hook_Feature_DisableWin10Taskbar_IsEnabled, false},
        {{L"private: long __cdecl CTray::InitializeTrayUIComponent(void)"},
         &g_origInitializeTrayUIComponent, Hook_CTray_InitializeTrayUIComponent, false},
        {{L"long __cdecl TrayUI_CreateInstance(struct ITrayUIHost *,struct _GUID const &,void * *)"},
         &g_pTrayUI_CreateInstance, nullptr, false},
        {{L"struct CTrayStatic c_trayStatic"}, &g_pTrayStatic, nullptr, false},
    };

    if (!WindhawkUtils::HookSymbols(g_hExplorer, explorerexe_hooks, ARRAYSIZE(explorerexe_hooks))) {
        Wh_Log(L"[ERROR] HookSymbols failed");
        return FALSE;
    }

    if (void* pRegQuery = hKernelBase ? (void*)GetProcAddress(hKernelBase, "RegQueryValueExW") : nullptr)
        Wh_SetFunctionHook(pRegQuery, (void*)RegQueryValueExW_Hook, (void**)&g_origRegQueryValueExW);

    if (void* pGetRect = g_hShell32 ? (void*)GetProcAddress(g_hShell32, "Shell_NotifyIconGetRect") : nullptr)
        Wh_SetFunctionHook(pGetRect, (void*)Shell_NotifyIconGetRect_Hook, (void**)&g_origGetRect);

    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_processType != ProcessType::Explorer) return;
    g_stopThread = 0;
    g_hInitThread = CreateThread(nullptr, 0, TrayInitThread, nullptr, 0, nullptr);
}

void Wh_ModSettingsChanged() {
    if (g_processType != ProcessType::Explorer) return;
    LoadSettings();
    {
        std::lock_guard<std::mutex> lock(g_trayMutex);
        SyncActionCenterIconLocked();
        for (auto& it : g_trayIcons) it.monoKind = -1;
    }
    if (g_hNotifyWnd && IsWindow(g_hNotifyWnd)) PostMessageW(g_hNotifyWnd, WM_MOD_RELAYOUT, 0, 0);
}

void Wh_ModUninit() {
    if (g_processType == ProcessType::Userinit) {
        if (g_explorerFile != INVALID_HANDLE_VALUE) {
            CloseHandle(g_explorerFile);
            g_explorerFile = INVALID_HANDLE_VALUE;
        }
        return;
    }
    if (g_processType != ProcessType::Explorer) return;

    InterlockedExchange(&g_stopThread, 1);
    if (g_hInitThread) {
        WaitForSingleObject(g_hInitThread, 2000);
        CloseHandle(g_hInitThread);
        g_hInitThread = nullptr;
    }
    if (g_traySubclassed && g_hTrayWnd && IsWindow(g_hTrayWnd))
        SendMessageW(g_hTrayWnd, WM_MOD_UNINIT_TRAY, 0, 0);

    {
        std::lock_guard<std::mutex> lock(g_trayMutex);
        for (auto& it : g_trayIcons) FreeEntryIcons(it);
        g_trayIcons.clear();
    }

    if (g_hBthprops) {
        FreeLibrary(g_hBthprops);
        g_hBthprops = nullptr;
    }
}
