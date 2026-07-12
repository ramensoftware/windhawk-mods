// ==WindhawkMod==
// @id             win7-action-center-recreation
// @name           Windows 7/8.1 Action Center Recreation
// @description    Recreation of the Windows 7/8.1 Action Center Tray Icon and Flyout UI
// @version        1.1.6
// @author         babamohammed
// @include        explorer.exe
// @architecture   x86-64
// @compilerOptions -lgdi32 -luser32 -lshell32 -lwscapi -ldwmapi -lcrypt32 -luxtheme -lole32 -loleaut32 -lmsimg32 -ladvapi32
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*

# Windows 7/8.1 Action Center Recreation
This mod recreates the classic Windows 7/8.1 Action Center tray icon and flyout for modern Windows versions.
## Screenshots
Windows 7 theme

![Image](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win7act.png)
Windows 8.1 theme

![Image](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win8.1.webp)

## Features
- **Real-time Security Status**: The tray icon shows your system's security state at a glance. Green means everything is fine, yellow means warnings, red means critical issues.
- **Interactive Flyout**: Click the tray icon to open a flyout that displays all security issues. Click any issue to open the relevant settings or troubleshooting page.
- **Dark Mode**: Automatically adapts to the system's theme.
- **Rounded Corners**: Rounded corners are supported for a more similar look to the original Windows 7 flyout.
- **Classic Theme support**: Disable the "Rounded Corners" theme to make the flyout use a classic theme.

- **Hotkeys**:
  - `Ctrl+N`: Simulate a notification
  - `Ctrl+Shift+N`: Clear notifications
- **Languages**: English, Italian, Spanish, French, Russian are currently supported.


## How It Works

The mod monitors your system's security settings including Firewall, Antivirus, Windows Update, UAC, and Windows Defender. When an issue is detected, the tray icon changes color and the flyout shows the problem with a clickable link to fix it.

## Notes

- The mod runs inside Explorer and works on Windows 10 and 11.
- If the icon doesn't appear, try restarting Explorer or the mod.
## Credits 
- Yvor - Testing on Windows 10 21H2.
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- useRoundedCorners: true
  $name: Rounded Corners
  $description: Use rounded corners to faithfully recreate the original Windows 7 appearance.
- refreshInterval: 5000
  $name: Refresh Interval (ms)
  $description: How often to check security status (min 1000ms, 0 to disable).
- enableHotkey: false
  $name: Enable Hotkeys
  $description: Ctrl+N to simulate notification, Ctrl+Shift+N to clear.
- language: auto
  $name: Language
  $description: Interface language (auto, en, it, es, fr, ru).
  $options:
    - auto: Auto-detect
    - en: English
    - it: Italiano
    - es: Español
    - fr: Français
    - ru: Русский
*/
// ==/WindhawkModSettings==

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <winsvc.h>
#include <windowsx.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <wscapi.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <strsafe.h>
#include <uxtheme.h>
#include <process.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <mutex>

// ============================================================================
// Troubleshooting Control Panel Redirection (Ported from restore-classic-cpls)
// ============================================================================
std::unordered_map<HKEY, std::wstring> g_keyPaths;
std::unordered_set<HKEY> g_fakeHandles;
std::mutex g_keyPathsMutex;

static const std::wstring kTroubleshootingGuid = L"{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}";

std::wstring ToLower(const std::wstring& str) {
    std::wstring res = str;
    for (auto& c : res) c = towlower(c);
    return res;
}

bool EndsWith(const std::wstring& str, const std::wstring& suffix) {
    if (str.size() < suffix.size()) return false;
    return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::wstring GetTrackedPath(HKEY hKey) {
    if (!hKey) return L"";
    if ((uintptr_t)hKey == 0x80000000) return L"HKEY_CLASSES_ROOT";
    if ((uintptr_t)hKey == 0x80000001) return L"HKEY_CURRENT_USER";
    if ((uintptr_t)hKey == 0x80000002) return L"HKEY_LOCAL_MACHINE";
    
    std::lock_guard<std::mutex> lock(g_keyPathsMutex);
    auto it = g_keyPaths.find(hKey);
    if (it != g_keyPaths.end()) return it->second;
    return L"";
}

void TrackKey(HKEY hKey, const std::wstring& path) {
    if (!hKey || (uintptr_t)hKey >= 0x80000000) return;
    std::lock_guard<std::mutex> lock(g_keyPathsMutex);
    g_keyPaths[hKey] = path;
}

void UntrackKey(HKEY hKey) {
    if (!hKey || (uintptr_t)hKey >= 0x80000000) return;
    std::lock_guard<std::mutex> lock(g_keyPathsMutex);
    g_keyPaths.erase(hKey);
}

HKEY CreateFakeHandle(const std::wstring& path) {
    int* dummy = new int(1);
    HKEY fake = (HKEY)dummy;
    std::lock_guard<std::mutex> lock(g_keyPathsMutex);
    g_keyPaths[fake] = path;
    g_fakeHandles.insert(fake);
    return fake;
}

void FreeFakeHandle(HKEY hKey) {
    std::lock_guard<std::mutex> lock(g_keyPathsMutex);
    auto it = g_fakeHandles.find(hKey);
    if (it != g_fakeHandles.end()) {
        g_fakeHandles.erase(it);
        g_keyPaths.erase(hKey);
        delete (int*)hKey;  // guaranteed once: erased before delete
    }
}

bool IsTroubleshootingPath(const std::wstring& path) {
    std::wstring lower = ToLower(path);
    return lower.find(kTroubleshootingGuid) != std::string::npos;  // ✅ Correct
}

bool TryProvideValue(const std::wstring& path, const std::wstring& valueName, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData, LSTATUS& status) {
    if (!IsTroubleshootingPath(path)) return false;
    if (valueName == L"System.ControlPanel.Category") {
        // La chiave reale di Recovery usa REG_SZ "0", non REG_DWORD.
        // wscui.cpl si aspetta una stringa (VT_LPWSTR); un DWORD viene
        // scartato silenziosamente durante l'enumerazione.
        static const WCHAR kCategoryValue[] = L"0";
        const DWORD cbNeeded = sizeof(kCategoryValue); // include il terminatore null
        if (lpType) *lpType = REG_SZ;
        if (!lpData || *lpcbData < cbNeeded) { *lpcbData = cbNeeded; status = ERROR_MORE_DATA; return true; }
        memcpy(lpData, kCategoryValue, cbNeeded);
        *lpcbData = cbNeeded;
        status = ERROR_SUCCESS;
        return true;
    }
    return false;
}

typedef LSTATUS (WINAPI *RegOpenKeyExW_t)(HKEY, LPCWSTR, DWORD, REGSAM, PHKEY);
RegOpenKeyExW_t RegOpenKeyExW_orig;
LSTATUS WINAPI RegOpenKeyExW_hook(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) {
    LSTATUS status = RegOpenKeyExW_orig(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    if (status == ERROR_SUCCESS && phkResult && *phkResult) {
        std::wstring basePath = GetTrackedPath(hKey);
        std::wstring fullPath = basePath + (lpSubKey ? (basePath.empty() ? L"" : L"\\") + std::wstring(lpSubKey) : L"");
        if (lpSubKey && IsTroubleshootingPath(lpSubKey))
            Wh_Log(L"[TroubleshootDiag] RegOpenKeyExW aperta: %s", fullPath.c_str());
        TrackKey(*phkResult, fullPath);
    }
    return status;
}

typedef LSTATUS (WINAPI *RegCloseKey_t)(HKEY);
RegCloseKey_t RegCloseKey_orig;
LSTATUS WINAPI RegCloseKey_hook(HKEY hKey) {
    if (!hKey) return ERROR_SUCCESS;
    {
        std::lock_guard<std::mutex> lock(g_keyPathsMutex);
        auto it = g_fakeHandles.find(hKey);
        if (it != g_fakeHandles.end()) {
            g_fakeHandles.erase(it);
            g_keyPaths.erase(hKey);
            delete (int*)hKey;
            return ERROR_SUCCESS;  // never call RegCloseKey_orig on a fake handle
        }
    }
    LSTATUS status = RegCloseKey_orig(hKey);
    UntrackKey(hKey);
    return status;
}

typedef LSTATUS (WINAPI *RegQueryValueExW_t)(HKEY, LPCWSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
RegQueryValueExW_t RegQueryValueExW_orig;
LSTATUS WINAPI RegQueryValueExW_hook(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    std::wstring path = GetTrackedPath(hKey);
    if (!path.empty()) {
        if (IsTroubleshootingPath(path))
            Wh_Log(L"[TroubleshootDiag] RegQueryValueExW su %s, valore=%s", path.c_str(), lpValueName ? lpValueName : L"(null)");
        LSTATUS status;
        if (TryProvideValue(path, lpValueName ? lpValueName : L"", lpType, lpData, lpcbData, status)) return status;
    }
    return RegQueryValueExW_orig(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

typedef LSTATUS (WINAPI *RegGetValueW_t)(HKEY, LPCWSTR, LPCWSTR, DWORD, LPDWORD, PVOID, LPDWORD);
RegGetValueW_t RegGetValueW_orig;
LSTATUS WINAPI RegGetValueW_hook(HKEY hkey, LPCWSTR lpSubKey, LPCWSTR lpValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData) {
    std::wstring basePath = GetTrackedPath(hkey);
    std::wstring fullPath = basePath + (lpSubKey ? (basePath.empty() ? L"" : L"\\") + std::wstring(lpSubKey) : L"");
    bool isTroubleshootPath = (lpSubKey && IsTroubleshootingPath(lpSubKey)) || IsTroubleshootingPath(fullPath);
    if (isTroubleshootPath)
        Wh_Log(L"[TroubleshootDiag] RegGetValueW su %s, valore=%s", fullPath.c_str(), lpValue ? lpValue : L"(null)");
    if (isTroubleshootPath && lpValue && std::wstring(lpValue) == L"System.ControlPanel.Category") {
        static const WCHAR kCategoryValue[] = L"0";
        const DWORD cbNeeded = sizeof(kCategoryValue); // include il terminatore null
        if (pdwType) *pdwType = REG_SZ;
        if (!pvData || !pcbData || *pcbData < cbNeeded) { if (pcbData) *pcbData = cbNeeded; return ERROR_MORE_DATA; }
        memcpy(pvData, kCategoryValue, cbNeeded);
        *pcbData = cbNeeded;
        return ERROR_SUCCESS;
    }
    return RegGetValueW_orig(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
}

void InstallRegistryHooks() {
    HMODULE hKb = GetModuleHandleW(L"kernelbase.dll");
    if (!hKb) hKb = GetModuleHandleW(L"advapi32.dll");
    if (hKb) {
        WindhawkUtils::Wh_SetFunctionHookT((RegOpenKeyExW_t)GetProcAddress(hKb, "RegOpenKeyExW"), RegOpenKeyExW_hook, &RegOpenKeyExW_orig);
        WindhawkUtils::Wh_SetFunctionHookT((RegCloseKey_t)GetProcAddress(hKb, "RegCloseKey"), RegCloseKey_hook, &RegCloseKey_orig);
        WindhawkUtils::Wh_SetFunctionHookT((RegQueryValueExW_t)GetProcAddress(hKb, "RegQueryValueExW"), RegQueryValueExW_hook, &RegQueryValueExW_orig);
        WindhawkUtils::Wh_SetFunctionHookT((RegGetValueW_t)GetProcAddress(hKb, "RegGetValueW"), RegGetValueW_hook, &RegGetValueW_orig);
    }
}

// ============================================================================



// ============================================================================
// Diagnostica: log dell'albero dei controlli figli di una finestra (sostituto
// di Spy++/Inspect.exe quando non sono disponibili) — attivata con Ctrl+Alt+W
// ============================================================================
void LogWindowTreeRecursive(HWND hwnd, int depth) {
    if (!hwnd) return;
    WCHAR className[256] = {0};
    WCHAR text[256] = {0};
    GetClassNameW(hwnd, className, 256);
    GetWindowTextW(hwnd, text, 256);
    int ctrlId = GetDlgCtrlID(hwnd);
    RECT rc = {0,0,0,0};
    GetWindowRect(hwnd, &rc);
    std::wstring indent(depth * 2, L' ');
    Wh_Log(L"[WinTreeDiag]%s hwnd=0x%p class=%s id=%d text=\"%s\" rect=(%d,%d,%d,%d)",
           indent.c_str(), hwnd, className, ctrlId, text, rc.left, rc.top, rc.right, rc.bottom);
    HWND hChild = GetWindow(hwnd, GW_CHILD);
    while (hChild) {
        LogWindowTreeRecursive(hChild, depth + 1);
        hChild = GetWindow(hChild, GW_HWNDNEXT);
    }
}

void LogWindowTree(HWND hwndRoot) {
    if (!hwndRoot || !IsWindow(hwndRoot)) { Wh_Log(L"[WinTreeDiag] Finestra in primo piano non valida"); return; }
    WCHAR title[256] = {0};
    GetWindowTextW(hwndRoot, title, 256);
    Wh_Log(L"[WinTreeDiag] === Albero finestra in primo piano: \"%s\" ===", title);
    LogWindowTreeRecursive(hwndRoot, 0);
    Wh_Log(L"[WinTreeDiag] === Fine albero ===");
}

#define FLYOUT_OFFSET 8

/* Adjust a window's position to be pushed away from the taskbar (Aero Flyout Fix style) */
POINT AdjustWindowPosForTaskbar(HWND hWnd)
{
    HMONITOR hm = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    HDC hDC = GetDC(hWnd);
    int offset = MulDiv(FLYOUT_OFFSET, GetDeviceCaps(hDC, LOGPIXELSY), 96);
    ReleaseDC(hWnd, hDC);

    RECT rc;
    GetWindowRect(hWnd, &rc);

    MONITORINFO mi = { sizeof(MONITORINFO) };
    GetMonitorInfoW(hm, &mi);

    int dx = 0, dy = 0;
    long* plrc = (long*)&rc;
    long* plwrc = (long*)&mi.rcWork;
    for (int i = 0; i < 4; i++)
    {
        int curOffset = plwrc[i] - plrc[i];
        curOffset = (curOffset < 0) ? -curOffset : curOffset;

        if (curOffset < offset)
        {
            int *set = (i % 2 == 0) ? &dx : &dy;
            if (i > 1) *set -= offset - curOffset;
            else *set += offset - curOffset;
        }
    }
    return { rc.left + dx, rc.top + dy };
}
// ============================================================================
// Constants
// ============================================================================
#define FLYOUT_CLASS_NAME          L"Win7ActionCenterFlyoutClass"
#define TRAY_MESSAGE_CLASS_NAME    L"Win7ActionCenterTrayMsgClass"
#define NOTIFY_WINDOW_CLASS_NAME   L"Win7ActionCenterNotifyClass"
#define WM_TRAY_ICON_MSG           (WM_USER + 300)
#define WM_TRIGGER_FLYOUT          (WM_USER + 400)
#define WM_SIMULATE_NOTIFICATION   (WM_USER + 401)
#define WM_CLEAR_NOTIFICATIONS     (WM_USER + 402)
#define WM_SAFE_CLOSE              (WM_USER + 500)
#define WM_REFRESH_DATA            (WM_USER + 600)
#define WM_SECURITY_CHANGED        (WM_USER + 601)
#define TRAY_ICON_ID               3003
#define AUTOHIDE_TIMER_ID          2001
#define NOTIFY_TIMER_ID            2002
#define REFRESH_TIMER_ID           1001
#define TRAY_RETRY_TIMER_ID        1002
#define TRAY_HEALTH_TIMER_ID       1003
#define WM_TRAY_SHUTDOWN           (WM_USER + 602)
#define HOTKEY_ID_SIMULATE         9001
#define HOTKEY_ID_CLEAR            9002
#define HOTKEY_ID_LOGTREE          9003
#define ID_MENU_OPEN_AC            4001
#define ID_MENU_TROUBLESHOOT       4002

// {7D5A4B2F-1C8E-4A3B-9D2E-6F8A1B3C5D7E}
static const GUID TRAY_ICON_GUID =
    { 0x7d5a4b2f, 0x1c8e, 0x4a3b, { 0x9d, 0x2e, 0x6f, 0x8a, 0x1b, 0x3c, 0x5d, 0x7e } };

// Dimensioni base
#define BASE_WINDOW_WIDTH          291
#define BASE_WINDOW_HEIGHT         160
#define BASE_FOOTER_HEIGHT         48
#define BASE_NOTIFY_WIDTH          280
#define BASE_NOTIFY_HEIGHT         72
#define BASE_ICON_SIZE             20
#define BASE_HEADER_HEIGHT         44

// Dimensioni per altezza dinamica
#define BASE_LINE_HEIGHT           22          // Altezza di una riga di problema
#define BASE_DESCRIPTION_HEIGHT    42          // Altezza per la descrizione quando non ci sono problemi
#define BASE_MIN_PROBLEMS_HEIGHT   75          // Altezza minima per i problemi (2 righe)

// Limiti e timeout
#define MAX_PROBLEMS               8
#define MAX_DISPLAY_PROBLEMS       3
#define WM_CLOSE_FLYOUT_DELAY_MS   500
#define AUTOHIDE_INACTIVITY_MS     120000
#define THREAD_WAIT_TIMEOUT        500
#define LOG_PREFIX L"[Win7AC] "

// Colors - Light theme
#define COLOR_BG               RGB(255, 255, 255)
#define COLOR_TITLE            RGB(30, 57, 91)
#define COLOR_TEXT_DARK        RGB(32, 32, 32)
#define COLOR_LINK             RGB(0, 120, 215)
#define COLOR_LINK_HOVER       RGB(0, 80, 180)
#define COLOR_FOOTER_BG        RGB(240, 245, 252)
#define COLOR_BORDER_LINE1     RGB(204, 217, 234)
#define COLOR_BORDER_LINE2     RGB(255, 255, 255)
#define COLOR_NOTIFY_BG        RGB(255, 255, 255)
#define COLOR_NOTIFY_BORDER    RGB(185, 209, 234)
#define COLOR_NOTIFY_TITLE_BG  RGB(233, 240, 248)
#define COLOR_HEADER_BG        RGB(255, 255, 255)
#define COLOR_OK_TEXT          RGB(0, 120, 215)
#define COLOR_WARNING_TEXT     RGB(180, 140, 0)

// Colors - Dark theme
#define COLOR_DARK_BG               RGB(32, 32, 32)
#define COLOR_DARK_TITLE            RGB(240, 240, 240)
#define COLOR_DARK_TEXT             RGB(220, 220, 220)
#define COLOR_DARK_LINK             RGB(100, 170, 230)
#define COLOR_DARK_LINK_HOVER       RGB(130, 190, 250)
#define COLOR_DARK_FOOTER_BG        RGB(45, 45, 45)
#define COLOR_DARK_BORDER_LINE1     RGB(60, 60, 60)
#define COLOR_DARK_BORDER_LINE2     RGB(70, 70, 70)
#define COLOR_DARK_NOTIFY_BG        RGB(40, 40, 40)
#define COLOR_DARK_NOTIFY_BORDER    RGB(80, 80, 80)
#define COLOR_DARK_NOTIFY_TITLE_BG  RGB(50, 50, 50)
#define COLOR_DARK_HEADER_BG        RGB(38, 38, 38)
#define COLOR_DARK_OK_TEXT          RGB(100, 200, 100)

// Base64 decoder table
static const WCHAR kBase64Tbl[] = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
// ============================================================================
// GDI+ Rendering Subsystem (dynamic loading, from win7-network-flyout)
// ============================================================================
typedef int (WINAPI *GdipCreateBitmapFromHICON_t)(HICON, void**);
typedef int (WINAPI *GdipSetInterpolationMode_t)(void*, int);
typedef int (WINAPI *GdipSetSmoothingMode_t)(void*, int);
typedef int (WINAPI *GdipSetCompositingQuality_t)(void*, int);
typedef int (WINAPI *GdipDrawImageRectI_t)(void*, void*, int, int, int, int);
typedef int (WINAPI *GdipDeleteGraphics_t)(void*);
typedef int (WINAPI *GdipCreateBitmapFromScan0_t)(int, int, int, int, const void*, void**);
typedef int (WINAPI *GdipGetImageGraphicsContext_t)(void*, void**);
typedef int (WINAPI *GdipSetPixelOffsetMode_t)(void*, int);
typedef int (WINAPI *GdipGraphicsClear_t)(void*, unsigned int);
typedef int (WINAPI *GdipCreateHBITMAPFromBitmap_t)(void*, HBITMAP*, unsigned int);
typedef int (WINAPI *GdipDisposeImage_t)(void*);
typedef int (WINAPI *GdiplusStartup_t)(ULONG_PTR*, const void*, void*);
typedef void (WINAPI *GdiplusShutdown_t)(ULONG_PTR);

static HMODULE g_hGdiPlus = NULL;
static ULONG_PTR g_gdiplusToken = 0;
static GdipCreateBitmapFromHICON_t pGdipCreateBitmapFromHICON = NULL;
static GdipSetInterpolationMode_t pGdipSetInterpolationMode = NULL;
static GdipSetSmoothingMode_t pGdipSetSmoothingMode = NULL;
static GdipSetCompositingQuality_t pGdipSetCompositingQuality = NULL;
static GdipDrawImageRectI_t pGdipDrawImageRectI = NULL;
static GdipDeleteGraphics_t pGdipDeleteGraphics = NULL;
static GdipCreateBitmapFromScan0_t pGdipCreateBitmapFromScan0 = NULL;
static GdipGetImageGraphicsContext_t pGdipGetImageGraphicsContext = NULL;
static GdipSetPixelOffsetMode_t pGdipSetPixelOffsetMode = NULL;
static GdipGraphicsClear_t pGdipGraphicsClear = NULL;
static GdipCreateHBITMAPFromBitmap_t pGdipCreateHBITMAPFromBitmap = NULL;
typedef int (WINAPI *GdipCreateBitmapFromStream_t)(IStream*, void**);
typedef int (WINAPI *GdipCreateHICONFromBitmap_t)(void*, HICON*);
typedef int (WINAPI *GdipGetImageWidth_t)(void*, unsigned int*);
typedef int (WINAPI *GdipGetImageHeight_t)(void*, unsigned int*);
static GdipCreateBitmapFromStream_t pGdipCreateBitmapFromStream = NULL;
static GdipCreateHICONFromBitmap_t pGdipCreateHICON = NULL;
static GdipDisposeImage_t pGdipDisposeImage = NULL;
static GdiplusShutdown_t pGdiplusShutdown = NULL;
// AlphaBlend from msimg32.dll
typedef BOOL (WINAPI *AlphaBlend_t)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
static AlphaBlend_t pAlphaBlend = NULL;
static HMODULE g_hMsImg32 = NULL;

// Cached GDI+ bitmaps for flyout icons
static void* g_pBmpFlyoutGood = NULL;
static void* g_pBmpFlyoutWarning = NULL;
static void* g_pBmpFlyoutAlert = NULL;

static BOOL InitGdiPlusRendering() {
    if (g_hGdiPlus) return TRUE;
    g_hGdiPlus = LoadLibraryW(L"gdiplus.dll");
    if (!g_hGdiPlus) { Wh_Log(LOG_PREFIX L"GDI+: failed to load gdiplus.dll"); return FALSE; }
    pGdipCreateBitmapFromHICON = (GdipCreateBitmapFromHICON_t)GetProcAddress(g_hGdiPlus, "GdipCreateBitmapFromHICON");
    pGdipSetInterpolationMode = (GdipSetInterpolationMode_t)GetProcAddress(g_hGdiPlus, "GdipSetInterpolationMode");
    pGdipSetSmoothingMode = (GdipSetSmoothingMode_t)GetProcAddress(g_hGdiPlus, "GdipSetSmoothingMode");
    pGdipSetCompositingQuality = (GdipSetCompositingQuality_t)GetProcAddress(g_hGdiPlus, "GdipSetCompositingQuality");
    pGdipDrawImageRectI = (GdipDrawImageRectI_t)GetProcAddress(g_hGdiPlus, "GdipDrawImageRectI");
    pGdipDeleteGraphics = (GdipDeleteGraphics_t)GetProcAddress(g_hGdiPlus, "GdipDeleteGraphics");
    pGdipCreateBitmapFromScan0 = (GdipCreateBitmapFromScan0_t)GetProcAddress(g_hGdiPlus, "GdipCreateBitmapFromScan0");
    pGdipGetImageGraphicsContext = (GdipGetImageGraphicsContext_t)GetProcAddress(g_hGdiPlus, "GdipGetImageGraphicsContext");
    pGdipSetPixelOffsetMode = (GdipSetPixelOffsetMode_t)GetProcAddress(g_hGdiPlus, "GdipSetPixelOffsetMode");
    pGdipGraphicsClear = (GdipGraphicsClear_t)GetProcAddress(g_hGdiPlus, "GdipGraphicsClear");
    pGdipCreateHBITMAPFromBitmap = (GdipCreateHBITMAPFromBitmap_t)GetProcAddress(g_hGdiPlus, "GdipCreateHBITMAPFromBitmap");
    pGdipDisposeImage = (GdipDisposeImage_t)GetProcAddress(g_hGdiPlus, "GdipDisposeImage");
        pGdipCreateBitmapFromStream = (GdipCreateBitmapFromStream_t)GetProcAddress(g_hGdiPlus, "GdipCreateBitmapFromStream");
    auto pStartup = (GdiplusStartup_t)GetProcAddress(g_hGdiPlus, "GdiplusStartup");
    pGdiplusShutdown = (GdiplusShutdown_t)GetProcAddress(g_hGdiPlus, "GdiplusShutdown");
    if (!pGdipCreateBitmapFromHICON || !pGdipSetInterpolationMode || !pGdipDrawImageRectI ||
        !pGdipDeleteGraphics || !pGdipCreateBitmapFromScan0 || !pGdipGetImageGraphicsContext ||
        !pGdipSetPixelOffsetMode || !pGdipGraphicsClear || !pGdipCreateHBITMAPFromBitmap ||
        !pGdipDisposeImage || !pStartup || !pGdiplusShutdown || !pGdipSetSmoothingMode || !pGdipSetCompositingQuality) {
        Wh_Log(LOG_PREFIX L"GDI+: missing function pointers"); FreeLibrary(g_hGdiPlus); g_hGdiPlus = NULL; return FALSE; }
    struct { DWORD Version; void* Callback; BOOL Suppress; } si = {1, NULL, FALSE};
    if (pStartup(&g_gdiplusToken, &si, NULL) != 0) {
        Wh_Log(LOG_PREFIX L"GDI+: GdiplusStartup failed"); FreeLibrary(g_hGdiPlus); g_hGdiPlus = NULL; return FALSE; }
    // Load AlphaBlend
    g_hMsImg32 = LoadLibraryW(L"msimg32.dll");
    if (g_hMsImg32) pAlphaBlend = (AlphaBlend_t)GetProcAddress(g_hMsImg32, "AlphaBlend");
    Wh_Log(LOG_PREFIX L"GDI+ initialized successfully");
    return TRUE;
}
static BYTE B64Val(WCHAR c) {
    const WCHAR* p = wcschr(kBase64Tbl, c);
    return p ? (BYTE)(p - kBase64Tbl) : 0xFF;
}





// Helper to load GDI+ Bitmap directly from Base64 PNG string, bypassing HICON
static void ShutdownGdiPlus() {
    if (g_pBmpFlyoutGood) { if (pGdipDisposeImage) pGdipDisposeImage(g_pBmpFlyoutGood); g_pBmpFlyoutGood = NULL; }
    if (g_pBmpFlyoutWarning) { if (pGdipDisposeImage) pGdipDisposeImage(g_pBmpFlyoutWarning); g_pBmpFlyoutWarning = NULL; }
    if (g_pBmpFlyoutAlert) { if (pGdipDisposeImage) pGdipDisposeImage(g_pBmpFlyoutAlert); g_pBmpFlyoutAlert = NULL; }
    if (g_hGdiPlus) {
        if (pGdiplusShutdown && g_gdiplusToken) pGdiplusShutdown(g_gdiplusToken);
        FreeLibrary(g_hGdiPlus); g_hGdiPlus = NULL; g_gdiplusToken = 0;
        pGdiplusShutdown = NULL;
    }
    if (g_hMsImg32) { FreeLibrary(g_hMsImg32); g_hMsImg32 = NULL; pAlphaBlend = NULL; }
}



// ============================================================================
// RAII Guard Classes
// ============================================================================
class SRWGuard {
    SRWLOCK& m_lock; bool m_exclusive; bool m_released;
public:
    SRWGuard(SRWLOCK& lock, bool exclusive) : m_lock(lock), m_exclusive(exclusive), m_released(false) {
        if (m_exclusive) AcquireSRWLockExclusive(&m_lock); else AcquireSRWLockShared(&m_lock); }
    ~SRWGuard() { if (!m_released) release(); }
    void release() { if (m_released) return; if (m_exclusive) ReleaseSRWLockExclusive(&m_lock); else ReleaseSRWLockShared(&m_lock); m_released = true; }
    SRWGuard(const SRWGuard&) = delete; SRWGuard& operator=(const SRWGuard&) = delete;
};
class GdiObj {
    HGDIOBJ m_obj; bool m_owning;
public:
    explicit GdiObj(HGDIOBJ obj = NULL, bool owning = true) : m_obj(obj), m_owning(owning) {}
    ~GdiObj() { if (m_owning && m_obj) DeleteObject(m_obj); }
    HGDIOBJ get() const { return m_obj; }
    operator HGDIOBJ() const { return m_obj; }
    GdiObj& operator=(HGDIOBJ obj) { if (m_owning && m_obj) DeleteObject(m_obj); m_obj = obj; return *this; }
    HGDIOBJ detach() { HGDIOBJ o = m_obj; m_obj = NULL; return o; }
    GdiObj(const GdiObj&) = delete; GdiObj& operator=(const GdiObj&) = delete;
};
class RegKey {
    HKEY m_key;
public:
    RegKey() : m_key(NULL) {}
    ~RegKey() { close(); }
    void close() { if (m_key) { RegCloseKey(m_key); m_key = NULL; } }
    HKEY* operator&() { return &m_key; }
    operator HKEY() const { return m_key; }
    bool valid() const { return m_key != NULL; }
    RegKey(const RegKey&) = delete; RegKey& operator=(const RegKey&) = delete;
};
class ScopedHandle {
    HANDLE m_handle;
public:
    ScopedHandle() : m_handle(NULL) {}
    explicit ScopedHandle(HANDLE h) : m_handle(h) {}
    ~ScopedHandle() { close(); }
    void close() { if (m_handle && m_handle != INVALID_HANDLE_VALUE) { CloseHandle(m_handle); m_handle = NULL; } }
    HANDLE get() const { return m_handle; }
    operator bool() const { return m_handle != NULL && m_handle != INVALID_HANDLE_VALUE; }
    ScopedHandle& operator=(HANDLE h) { close(); m_handle = h; return *this; }
    HANDLE* operator&() { close(); return &m_handle; }
    ScopedHandle(const ScopedHandle&) = delete; ScopedHandle& operator=(const ScopedHandle&) = delete;
};
class SelectGuard {
    HDC m_hdc; HGDIOBJ m_old;
public:
    SelectGuard(HDC hdc, HGDIOBJ obj) : m_hdc(hdc), m_old(SelectObject(hdc, obj)) {}
    ~SelectGuard() { if (m_hdc && m_old) SelectObject(m_hdc, m_old); }
    SelectGuard(const SelectGuard&) = delete; SelectGuard& operator=(const SelectGuard&) = delete;
};
class DcStateGuard {
    HDC m_hdc; int m_saved;
public:
    explicit DcStateGuard(HDC hdc) : m_hdc(hdc), m_saved(SaveDC(hdc)) {}
    ~DcStateGuard() { if (m_hdc && m_saved) RestoreDC(m_hdc, m_saved); }
    DcStateGuard(const DcStateGuard&) = delete; DcStateGuard& operator=(const DcStateGuard&) = delete;
};

// ============================================================================
// Forward Declarations (for functions used before definition)
// ============================================================================
static int  CalculateFlyoutHeight(int activeProblems);
static void ShowBalloonNotification(int oldState, int newState);
static void UpdateTrayIcon(void);
static void AddTrayIcon(void);
static void ScheduleTrayIconRecovery(void);
static void PositionWindowNearTray(HWND hwnd);
static void InstallClickOutsideHook(void);
static void RemoveClickOutsideHook(void);
static void CreateFlyoutWindow(void);
static void CloseFlyout(HWND hwnd);
LRESULT CALLBACK ClickOutsideMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);
static void CleanupModResources(void);
static void OpenProblemAction(int problemType);

// Wait until the taskbar exists so Shell_NotifyIcon can succeed.
// Defined after GlobalContext so it can observe g_Ctx.isUninitializing.
static BOOL WaitForTaskbarReady(DWORD timeoutMs);

// ============================================================================
// Process Check
// ============================================================================
// True when this explorer.exe is (or will become) the main shell process.
// IMPORTANT: at boot / right after explorer restart, Shell_TrayWnd may not
// exist yet, so we must NOT require it here.
static BOOL IsMainExplorerProcess() {
    WCHAR exePath[MAX_PATH] = {};
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    WCHAR* name = wcsrchr(exePath, L'\\');
    name = name ? name + 1 : exePath;

    if (_wcsicmp(name, L"explorer.exe") != 0) return FALSE;

    // Prefer the process that owns the desktop shell window when available.
    HWND hShell = GetShellWindow();
    if (hShell) {
        DWORD shellPid = 0;
        GetWindowThreadProcessId(hShell, &shellPid);
        if (shellPid != 0 && shellPid != GetCurrentProcessId()) {
            // Another explorer instance already owns the desktop shell.
            return FALSE;
        }
    }

    // If the tray is not ready yet (boot / restart), still allow init so the
    // tray thread can wait for TaskbarCreated / Shell_TrayWnd.
    return TRUE;
}

// ============================================================================
// Settings
// ============================================================================
struct ModSettings {
    BOOL useRoundedCorners;
    BOOL enableNotificationSimulation;
    BOOL privacyMode;
    int  refreshInterval;
    BOOL enableHotkey;
    int  language;
} g_Settings = { TRUE, TRUE, FALSE, 5000, FALSE, 0 };

void LoadSettings() {
    g_Settings.useRoundedCorners = Wh_GetIntSetting(L"useRoundedCorners");
    g_Settings.enableNotificationSimulation = Wh_GetIntSetting(L"enableNotificationSimulation");
    g_Settings.privacyMode = Wh_GetIntSetting(L"privacyMode");
    g_Settings.refreshInterval = Wh_GetIntSetting(L"refreshInterval");
    g_Settings.enableHotkey = Wh_GetIntSetting(L"enableHotkey");
    LPCWSTR lang = Wh_GetStringSetting(L"language");
    if (lang) {
        if (_wcsicmp(lang, L"en") == 0) g_Settings.language = 1;
        else if (_wcsicmp(lang, L"it") == 0) g_Settings.language = 2;
        else if (_wcsicmp(lang, L"es") == 0) g_Settings.language = 3;
        else if (_wcsicmp(lang, L"fr") == 0) g_Settings.language = 4;
        else if (_wcsicmp(lang, L"ru") == 0) g_Settings.language = 5;
        else g_Settings.language = 0;
        Wh_FreeStringSetting(lang);
    }
    if (g_Settings.refreshInterval > 0 && g_Settings.refreshInterval < 1000)
        g_Settings.refreshInterval = 1000;
}

// ============================================================================
// DPI Scaling
// ============================================================================
static UINT g_dpi = 96;
static int g_ScaledWidth = BASE_WINDOW_WIDTH;
static int g_ScaledHeight = BASE_WINDOW_HEIGHT;
static int g_ScaledFooterHeight = BASE_FOOTER_HEIGHT;
static int g_ScaledNotifyWidth = BASE_NOTIFY_WIDTH;
static int g_ScaledNotifyHeight = BASE_NOTIFY_HEIGHT;
static int g_ScaledIconSize = BASE_ICON_SIZE;
static int g_ScaledHeaderHeight = BASE_HEADER_HEIGHT;
static int g_BorderPenWidth = 1;

static inline int ScaleDpi(int v) { return MulDiv(v, (int)g_dpi, 96); }
static inline int MaxInt(int a, int b) { return (a > b) ? a : b; }
void RecalcDpiMetrics(UINT dpi, int activeProblems = -1) {
    g_dpi = dpi ? dpi : 96;
    g_ScaledWidth = ScaleDpi(BASE_WINDOW_WIDTH);
    
    // Calcola altezza in base ai problemi (delega a CalculateFlyoutHeight per coerenza)
    if (activeProblems < 0) {
        // Default: nessun problema
        g_ScaledHeight = CalculateFlyoutHeight(0);
    } else {
        g_ScaledHeight = CalculateFlyoutHeight(activeProblems);
    }
    
    // Footer +3.5% vs original base height (keeps proportions of the rest of the UI).
    g_ScaledFooterHeight = MulDiv(ScaleDpi(BASE_FOOTER_HEIGHT), 1035, 1000);
    g_ScaledNotifyWidth = ScaleDpi(BASE_NOTIFY_WIDTH);
    g_ScaledNotifyHeight = ScaleDpi(BASE_NOTIFY_HEIGHT);
    g_ScaledIconSize = ScaleDpi(BASE_ICON_SIZE);
    g_ScaledHeaderHeight = ScaleDpi(BASE_HEADER_HEIGHT);
    g_BorderPenWidth = MaxInt(1, ScaleDpi(1));
}
int CalculateFlyoutHeight(int activeProblems) {

    // Keep footer height in sync with RecalcDpiMetrics (+3.5%).
    int footerH = MulDiv(ScaleDpi(BASE_FOOTER_HEIGHT), 1035, 1000);

    // Altezza minima di base (con o senza rounded corners)
    int minBaseHeight = ScaleDpi(160);
    if (g_Settings.useRoundedCorners) {
        minBaseHeight = ScaleDpi(205);
    }

    if (activeProblems == 0) {

        // Nessun problema: altezza fissa con descrizione
        int height = ScaleDpi(BASE_HEADER_HEIGHT + BASE_DESCRIPTION_HEIGHT + 10) + footerH;
        if (height < minBaseHeight) height = minBaseHeight;
        return height;

    } else {

        int displayCount = (activeProblems < MAX_DISPLAY_PROBLEMS) ? activeProblems : MAX_DISPLAY_PROBLEMS;

        int lineH = ScaleDpi(22);

        int maxRowsPerProblem = 3;

        // Calcoliamo l'altezza basandoci su quante righe effettivamente useremo
        // In mancanza di info sul wrapping qui, usiamo il caso peggiore (3 righe per problema)
        int totalRows = displayCount * maxRowsPerProblem;

        if (activeProblems > MAX_DISPLAY_PROBLEMS) {
            totalRows += 1;  // riga extra per "...e altri"
        }

        // Spaziatura tra problemi: 8% in più di lineH tra un problema e l'altro
        int gapBetween = lineH / 12;  // ~8% di lineH
        int problemsHeight = totalRows * lineH + (displayCount - 1) * gapBetween;

        int extraHeight = (activeProblems > MAX_DISPLAY_PROBLEMS) ? ScaleDpi(16) : 0;

        // Padding sopra e sotto la zona problemi
        int topPadding = ScaleDpi(12);
        int bottomPadding = ScaleDpi(8);

        int height = ScaleDpi(BASE_HEADER_HEIGHT) + topPadding + problemsHeight + extraHeight + bottomPadding + footerH;

        int minHeight = ScaleDpi(BASE_HEADER_HEIGHT + BASE_MIN_PROBLEMS_HEIGHT) + footerH;
        if (height < minHeight) height = minHeight;

        // Assicura che l'altezza sia almeno quella minima di base
        if (height < minBaseHeight) height = minBaseHeight;

        return height;
    }
}

// ============================================================================
// Dark Mode
// ============================================================================
BOOL IsDarkModeEnabled() {
    RegKey hKey;
    DWORD dwValue = 1, dwSize = sizeof(DWORD);
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExW(hKey, L"AppsUseLightTheme", NULL, NULL, (LPBYTE)&dwValue, &dwSize);
    }
    return (dwValue == 0);
}
// ============================================================================
// Localization
// ============================================================================
typedef enum {
    STR_ACTION_CENTER_TITLE, STR_LINK_OPEN_AC, STR_NOTIFY_TITLE,
    STR_MENU_TROUBLESHOOT, STR_MENU_UPDATE, STR_SUBTITLE_ALERT2,
    STR_SUBTITLE_ALERT1, STR_NO_PROBLEMS, STR_TIP_OK, STR_TIP_WARNING,
    STR_TIP_ALERT, STR_MSG_FIREWALL, STR_MSG_ANTIVIRUS, STR_MSG_UPDATE,
    STR_MSG_UAC, STR_NOTIFY_MESSAGE, STR_NOTIFY_SIMULATED, STR_MSG_AUTOUPDATE,
    STR_MSG_ANTISPYWARE, STR_MSG_INTERNET, STR_MSG_SERVICE, STR_MSG_DEFENDER,
    STR_AND_MORE, STR_TIP_NO_ISSUES, STR_TIP_ISSUES, STR_COUNT
} LocaleStringId;
typedef struct { LANGID langId; const WCHAR* strings[STR_COUNT]; } LocalePack;
static const LocalePack g_Locales[] = {
    // Inglese (0x0409) - COMPLETO E NATURALE
    { 0x0409, { 
        L"Action Center", 
        L"Open Action Center", 
        L"Action Center", 
        L"Troubleshooting", 
        L"Windows Update", 
        L"2 important messages", 
        L"1 important message", 
        L"No current issues detected.\nYou can use Action Center to review recent messages about your computer's status and find solutions to problems.", 
        L"Action Center", 
        L"Action Center", 
        L"Action Center", 
        L"Windows Firewall is turned off.", 
        L"Virus protection is off.", 
        L"Windows Update is not configured.", 
        L"User Account Control is off.", 
        L"Click to open Action Center.", 
        L"Action Center has detected new issues.", 
        L"Windows Update is not set to update automatically.", 
        L"Antispyware protection is off.", 
        L"Internet security settings need attention.", 
        L"Security Center service is not running.", 
        L"Windows Defender real-time protection is off.", 
        L"...and more", 
        L"No current issues detected.", 
        L"%d issues detected." 
    }},
    // Italiano (0x0410) - COMPLETO E FORMALE
    { 0x0410, { 
        L"Centro operativo", 
        L"Apri Centro operativo", 
        L"Centro operativo", 
        L"Risoluzione dei problemi", 
        L"Windows Update", 
        L"2 messaggi importanti", 
        L"1 messaggio importante", 
        L"Nessun problema rilevato.\n\u00C8 possibile utilizzare il Centro operativo per visualizzare i messaggi recenti sullo stato del computer e trovare soluzioni ai problemi.", 
        L"Centro operativo", 
        L"Centro operativo", 
        L"Centro operativo", 
        L"Windows Firewall \u00E8 disattivato.", 
        L"La protezione antivirus non \u00E8 attiva.", 
        L"Windows Update non \u00E8 configurato.", 
        L"Il controllo dell'account utente \u00E8 disattivato.", 
        L"Fare clic per aprire il Centro operativo.", 
        L"Il Centro operativo ha rilevato nuovi problemi.", 
        L"Windows Update non \u00E8 impostato per l'aggiornamento automatico.", 
        L"La protezione antispyware non \u00E8 attiva.", 
        L"Le impostazioni di sicurezza di Internet richiedono attenzione.", 
        L"Il servizio Centro sicurezza non \u00E8 in esecuzione.", 
        L"La protezione in tempo reale di Windows Defender non \u00E8 attiva.", 
        L"...e altri", 
        L"Nessun problema rilevato.", 
        L"%d problemi rilevati." 
    }},
    // Spagnolo (0x040A) - COMPLETO
    { 0x040A, { 
        L"Centro de actividades", 
        L"Abrir Centro de actividades", 
        L"Centro de actividades", 
        L"Soluci\u00F3n de problemas", 
        L"Windows Update", 
        L"2 mensajes importantes", 
        L"1 mensaje importante", 
        L"No se detectaron problemas.\nPuede usar el Centro de actividades para revisar los mensajes recientes sobre el estado de su equipo y encontrar soluciones a los problemas.", 
        L"Centro de actividades", 
        L"Centro de actividades", 
        L"Centro de actividades", 
        L"El Firewall de Windows est\u00E1 desactivado.", 
        L"La protecci\u00F3n antivirus est\u00E1 desactivada.", 
        L"Windows Update no est\u00E1 configurado.", 
        L"El Control de cuentas de usuario est\u00E1 desactivado.", 
        L"Haga clic para abrir el Centro de actividades.", 
        L"El Centro de actividades ha detectado nuevos problemas.", 
        L"Windows Update no est\u00E1 configurado para actualizarse autom\u00E1ticamente.", 
        L"La protecci\u00F3n antispyware est\u00E1 desactivada.", 
        L"La configuraci\u00F3n de seguridad de Internet necesita atenci\u00F3n.", 
        L"El servicio Centro de seguridad no se est\u00E1 ejecutando.", 
        L"La protecci\u00F3n en tiempo real de Windows Defender est\u00E1 desactivada.", 
        L"...y m\u00E1s", 
        L"No se detectaron problemas.", 
        L"%d problemas detectados." 
    }},
    // Francese (0x040C) - COMPLETO
    { 0x040C, { 
        L"Centre d'actions", 
        L"Ouvrir le Centre d'actions", 
        L"Centre d'actions", 
        L"R\u00E9solution des probl\u00E8mes", 
        L"Windows Update", 
        L"2 messages importants", 
        L"1 message important", 
        L"Aucun probl\u00E8me d\u00E9tect\u00E9.\nVous pouvez utiliser le Centre d'actions pour consulter les messages r\u00E9cents sur l'\u00E9tat de votre ordinateur et trouver des solutions aux probl\u00E8mes.", 
        L"Centre d'actions", 
        L"Centre d'actions", 
        L"Centre d'actions", 
        L"Le Pare-feu Windows est d\u00E9sactiv\u00E9.", 
        L"La protection antivirus est d\u00E9sactiv\u00E9e.", 
        L"Windows Update n'est pas configur\u00E9.", 
        L"Le Contr\u00F4le de compte d'utilisateur est d\u00E9sactiv\u00E9.", 
        L"Cliquez pour ouvrir le Centre d'actions.", 
        L"Le Centre d'actions a d\u00E9tect\u00E9 de nouveaux probl\u00E8mes.", 
        L"Windows Update n'est pas configur\u00E9 pour les mises \u00E0 jour automatiques.", 
        L"La protection anti-logiciels espions est d\u00E9sactiv\u00E9e.", 
        L"Les param\u00E8tres de s\u00E9curit\u00E9 Internet n\u00E9cessitent une attention.", 
        L"Le service Centre de s\u00E9curit\u00E9 n'est pas en cours d'ex\u00E9cution.", 
        L"La protection en temps r\u00E9el de Windows Defender est d\u00E9sactiv\u00E9e.", 
        L"...et plus", 
        L"Aucun probl\u00E8me d\u00E9tect\u00E9.", 
        L"%d probl\u00E8mes d\u00E9tect\u00E9s." 
    }},
    // Russo (0x0419) - COMPLETO
    { 0x0419, { 
        L"\u0426\u0435\u043D\u0442\u0440 \u0434\u0435\u0439\u0441\u0442\u0432\u0438\u0439", 
        L"\u041E\u0442\u043A\u0440\u044B\u0442\u044C \u0426\u0435\u043D\u0442\u0440 \u0434\u0435\u0439\u0441\u0442\u0432\u0438\u0439", 
        L"\u0426\u0435\u043D\u0442\u0440 \u0434\u0435\u0439\u0441\u0442\u0432\u0438\u0439", 
        L"\u0423\u0441\u0442\u0440\u0430\u043D\u0435\u043D\u0438\u0435 \u043D\u0435\u043F\u043E\u043B\u0430\u0434\u043E\u043A", 
        L"\u0426\u0435\u043D\u0442\u0440 \u043E\u0431\u043D\u043E\u0432\u043B\u0435\u043D\u0438\u0439", 
        L"2 \u0432\u0430\u0436\u043D\u044B\u0445 \u0441\u043E\u043E\u0431\u0449\u0435\u043D\u0438\u044F", 
        L"1 \u0432\u0430\u0436\u043D\u043E\u0435 \u0441\u043E\u043E\u0431\u0449\u0435\u043D\u0438\u0435", 
        L"\u041F\u0440\u043E\u0431\u043B\u0435\u043C \u043D\u0435 \u043E\u0431\u043D\u0430\u0440\u0443\u0436\u0435\u043D\u043E.\n\u0418\u0441\u043F\u043E\u043B\u044C\u0437\u0443\u0439\u0442\u0435 \u0426\u0435\u043D\u0442\u0440 \u0434\u0435\u0439\u0441\u0442\u0432\u0438\u0439 \u0434\u043B\u044F \u043F\u0440\u043E\u0441\u043C\u043E\u0442\u0440\u0430 \u043D\u043E\u0432\u044B\u0445 \u0441\u043E\u043E\u0431\u0449\u0435\u043D\u0438\u0439 \u043E \u0441\u043E\u0441\u0442\u043E\u044F\u043D\u0438\u0438 \u043A\u043E\u043C\u043F\u044C\u044E\u0442\u0435\u0440\u0430 \u0438 \u043F\u043E\u0438\u0441\u043A\u0430 \u0440\u0435\u0448\u0435\u043D\u0438\u0439 \u043F\u0440\u043E\u0431\u043B\u0435\u043C.", 
        L"\u0426\u0435\u043D\u0442\u0440 \u0434\u0435\u0439\u0441\u0442\u0432\u0438\u0439", 
        L"\u0426\u0435\u043D\u0442\u0440 \u0434\u0435\u0439\u0441\u0442\u0432\u0438\u0439", 
        L"\u0426\u0435\u043D\u0442\u0440 \u0434\u0435\u0439\u0441\u0442\u0432\u0438\u0439", 
        L"\u0411\u0440\u0435\u043D\u0434\u043C\u0430\u0443\u044D\u0440 Windows \u043E\u0442\u043A\u043B\u044E\u0447\u0435\u043D.", 
        L"\u0410\u043D\u0442\u0438\u0432\u0438\u0440\u0443\u0441\u043D\u0430\u044F \u0437\u0430\u0449\u0438\u0442\u0430 \u043E\u0442\u043A\u043B\u044E\u0447\u0435\u043D\u0430.", 
        L"Windows Update \u043D\u0435 \u043D\u0430\u0441\u0442\u0440\u043E\u0435\u043D.", 
        L"\u041A\u043E\u043D\u0442\u0440\u043E\u043B\u044C \u0443\u0447\u0435\u0442\u043D\u044B\u0445 \u0437\u0430\u043F\u0438\u0441\u0435\u0439 \u043E\u0442\u043A\u043B\u044E\u0447\u0435\u043D.", 
        L"\u041D\u0430\u0436\u043C\u0438\u0442\u0435, \u0447\u0442\u043E\u0431\u044B \u043E\u0442\u043A\u0440\u044B\u0442\u044C \u0426\u0435\u043D\u0442\u0440 \u0434\u0435\u0439\u0441\u0442\u0432\u0438\u0439.", 
        L"\u0426\u0435\u043D\u0442\u0440 \u0434\u0435\u0439\u0441\u0442\u0432\u0438\u0439 \u043E\u0431\u043D\u0430\u0440\u0443\u0436\u0438\u043B \u043D\u043E\u0432\u044B\u0435 \u043F\u0440\u043E\u0431\u043B\u0435\u043C\u044B.", 
        L"Windows Update \u043D\u0435 \u043D\u0430\u0441\u0442\u0440\u043E\u0435\u043D \u043D\u0430 \u0430\u0432\u0442\u043E\u043C\u0430\u0442\u0438\u0447\u0435\u0441\u043A\u043E\u0435 \u043E\u0431\u043D\u043E\u0432\u043B\u0435\u043D\u0438\u0435.", 
        L"\u0410\u043D\u0442\u0438\u0448\u043F\u0438\u043E\u043D\u0441\u043A\u0430\u044F \u0437\u0430\u0449\u0438\u0442\u0430 \u043E\u0442\u043A\u043B\u044E\u0447\u0435\u043D\u0430.", 
        L"\u041D\u0430\u0441\u0442\u0440\u043E\u0439\u043A\u0438 \u0431\u0435\u0437\u043E\u043F\u0430\u0441\u043D\u043E\u0441\u0442\u0438 \u0418\u043D\u0442\u0435\u0440\u043D\u0435\u0442\u0430 \u0442\u0440\u0435\u0431\u0443\u044E\u0442 \u0432\u043D\u0438\u043C\u0430\u043D\u0438\u044F.", 
        L"\u0421\u043B\u0443\u0436\u0431\u0430 \u0426\u0435\u043D\u0442\u0440\u0430 \u0431\u0435\u0437\u043E\u043F\u0430\u0441\u043D\u043E\u0441\u0442\u0438 \u043D\u0435 \u0437\u0430\u043F\u0443\u0449\u0435\u043D\u0430.", 
        L"\u0417\u0430\u0449\u0438\u0442\u0430 \u0432 \u0440\u0435\u0430\u043B\u044C\u043D\u043E\u043C \u0432\u0440\u0435\u043C\u0435\u043D\u0438 Windows Defender \u043E\u0442\u043A\u043B\u044E\u0447\u0435\u043D\u0430.", 
        L"... \u0438 \u0434\u0440\u0443\u0433\u0438\u0435", 
        L"\u041F\u0440\u043E\u0431\u043B\u0435\u043C \u043D\u0435 \u043E\u0431\u043D\u0430\u0440\u0443\u0436\u0435\u043D\u043E", 
        L"\u041E\u0431\u043D\u0430\u0440\u0443\u0436\u0435\u043D\u043E %d \u043F\u0440\u043E\u0431\u043B\u0435\u043C" 
    }},
};
static const LocalePack* g_CurrentLocalePack = &g_Locales[0];
#define LOC(id) (g_CurrentLocalePack->strings[id])
static const LocalePack* FindLocalePack(LANGID langId) {
    LANGID primaryLang = PRIMARYLANGID(langId);
    for (size_t i = 0; i < ARRAYSIZE(g_Locales); ++i) {
        if (g_Locales[i].langId == langId) return &g_Locales[i];
    }
    for (size_t i = 0; i < ARRAYSIZE(g_Locales); ++i) {
        if (PRIMARYLANGID(g_Locales[i].langId) == primaryLang) return &g_Locales[i];
    }
    return &g_Locales[0];
}


// ============================================================================
// Problem Types
// ============================================================================
enum ProblemType {
    PROB_NONE = 0, PROB_FIREWALL = 1, PROB_AUTOUPDATE, PROB_ANTIVIRUS,
    PROB_ANTISPYWARE, PROB_INTERNET, PROB_UAC, PROB_SERVICE, PROB_DEFENDER_RT
};

// ============================================================================
// Open Problem Action (Firewall or Troubleshooting)
// ============================================================================
void OpenProblemAction(int problemType) {
    Wh_Log(LOG_PREFIX L"Opening action for problem type: %d", problemType);
    
    // ONLY firewall opens directly the settings
    if (problemType == PROB_FIREWALL) {
        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_INVOKEIDLIST;
        sei.lpVerb = L"open";
        sei.lpFile = L"explorer.exe";
        sei.lpParameters = L"shell:::{4026492F-2F69-46B8-B9BF-5654FC07E423}";
        sei.nShow = SW_SHOWNORMAL;
        
        if (ShellExecuteExW(&sei)) {
            Wh_Log(LOG_PREFIX L"Opened Windows Defender Firewall");
        } else {
            Wh_Log(LOG_PREFIX L"Firewall shell command failed, using fallback");
            ShellExecuteW(NULL, L"open", L"control.exe", 
                         L"/name Microsoft.WindowsFirewall", NULL, SW_SHOWNORMAL);
        }
        return;
    }
    
    // ALL OTHER PROBLEMS -> Troubleshooting (like right-click)
    Wh_Log(LOG_PREFIX L"Opening Troubleshooting for problem type: %d", problemType);
    
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_INVOKEIDLIST;
    sei.lpVerb = L"open";
    sei.lpFile = L"explorer.exe";
    sei.lpParameters = L"shell:::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}";
    sei.nShow = SW_SHOWNORMAL;
    
    if (!ShellExecuteExW(&sei)) {
        Wh_Log(LOG_PREFIX L"Troubleshooting shell command failed, using fallback");
        ShellExecuteW(NULL, L"open", L"control.exe", 
                     L"/name Microsoft.Troubleshooting", NULL, SW_SHOWNORMAL);
    }
}

void DetermineLocale() {
    switch (g_Settings.language) {
        case 1: g_CurrentLocalePack = FindLocalePack(0x0409); break;
        case 2: g_CurrentLocalePack = FindLocalePack(0x0410); break;
        case 3: g_CurrentLocalePack = FindLocalePack(0x040A); break;
        case 4: g_CurrentLocalePack = FindLocalePack(0x040C); break;
        case 5: g_CurrentLocalePack = FindLocalePack(0x0419); break;
        default: g_CurrentLocalePack = FindLocalePack(GetUserDefaultUILanguage()); break;
    }
}

// ============================================================================
// Global State
// ============================================================================
enum SecurityState { STATE_GOOD = 0, STATE_WARNING = 1, STATE_ALERT = 2 };
struct GlobalContext {
    HWND hWndFlyout;
    HWND hWndMsgHandler;
    HWND hWndNotify;
    HANDLE hTrayThread;
    HANDLE hTrayReadyEvent;
    DWORD trayThreadId;
    UINT taskbarCreatedMessage;
    UINT trayRetryAttempt;
    BOOL trayIconAdded;
    volatile LONG refCount;
    volatile LONG isUninitializing;
    SRWLOCK srwLock;
    UINT_PTR refreshTimer;
    BOOL flyoutClassRegistered;
    BOOL trayMsgClassRegistered;
    BOOL notifyClassRegistered;
    BOOL darkMode;
    HANDLE hWscRegistration;
    HANDLE hRegMonitorThread;
    HANDLE hRegShutdownEvent;
    HANDLE hRegChangeEvent;
    volatile LONG regMonitorRunning;
} g_Ctx = {0};


// Wait until the taskbar exists so Shell_NotifyIcon can succeed.
// Returns TRUE if Shell_TrayWnd is available, FALSE on timeout / uninit.
static BOOL WaitForTaskbarReady(DWORD timeoutMs) {
    const DWORD step = 200;
    DWORD waited = 0;
    while (TRUE) {
        // Abort promptly if the mod is being unloaded (avoids UAF on late exit).
        if (g_Ctx.isUninitializing)
            return FALSE;
        HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
        if (hTray && IsWindow(hTray)) {
            // Give the notification area a brief moment to finish init.
            Sleep(150);
            return TRUE;
        }
        if (timeoutMs != INFINITE && waited >= timeoutMs)
            return FALSE;
        Sleep(step);
        waited += step;
    }
}
static NOTIFYICONDATAW g_nid = { 0 };
static int g_SecurityState = STATE_GOOD;
static BOOL g_IsHoveringLink = FALSE;
static BOOL g_FlyoutClosing = FALSE;
static BOOL g_NotifyShowing = FALSE;
static int g_SimulatedNotificationType = 0; // protected by srwLock
static int g_ActiveProblems = 0;             // protected by srwLock
static int g_ProblemTypes[MAX_PROBLEMS] = { 0 }; // protected by srwLock
static RECT g_rcFooterLink = { 0 };
static BOOL g_IsHoveringNoProblems = FALSE;  // hover state for the no-problems area

// Clickable problem links state
static int g_DisplayProblemCount = 0;
static int g_ProblemTypesDisplay[MAX_DISPLAY_PROBLEMS];
static RECT g_ProblemLinkRects[MAX_DISPLAY_PROBLEMS];
static int g_HoveredProblemIndex = -1;

// Windows version detection

// Flyout icons (GDI+ enhanced)
static HICON g_hFlyoutIconGood = NULL;
static HICON g_hFlyoutIconWarning = NULL;
static HICON g_hFlyoutIconAlert = NULL;
static HICON g_hShieldIcon = NULL;
static HHOOK g_hMouseHook = NULL;
static BOOL g_Initialized = FALSE;

// Fonts managed with RAII via GdiObj wrappers
static HFONT g_hFontTitle = NULL;
static HFONT g_hFontNormal = NULL;
static HFONT g_hFontBold = NULL;
static HFONT g_hFontLink = NULL;
static HFONT g_hFontSmall = NULL;

// ============================================================================
// Font Management
// ============================================================================
void FreeGlobalFonts() {
    if (g_hFontTitle) { DeleteObject(g_hFontTitle); g_hFontTitle = NULL; }
    if (g_hFontNormal) { DeleteObject(g_hFontNormal); g_hFontNormal = NULL; }
    if (g_hFontBold) { DeleteObject(g_hFontBold); g_hFontBold = NULL; }
    if (g_hFontLink) { DeleteObject(g_hFontLink); g_hFontLink = NULL; }
    if (g_hFontSmall) { DeleteObject(g_hFontSmall); g_hFontSmall = NULL; }
}
void InitGlobalFonts() {
    FreeGlobalFonts();
    int szTitle = -ScaleDpi(14);   // was 12, +13% -> 14
    int szNormal = -ScaleDpi(12);  // was 11, +13% -> 12
    int szSmall = -ScaleDpi(10);   // was  9, +13% -> 10
    g_hFontTitle = CreateFontW(szTitle, 0,0,0, FW_BOLD, 0,0,0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH|FF_DONTCARE, L"Segoe UI");
    g_hFontNormal = CreateFontW(szNormal, 0,0,0, FW_NORMAL, 0,0,0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH|FF_DONTCARE, L"Segoe UI");
    g_hFontBold = CreateFontW(szNormal, 0,0,0, FW_BOLD, 0,0,0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH|FF_DONTCARE, L"Segoe UI");
    g_hFontLink = CreateFontW(szNormal, 0,0,0, FW_NORMAL, 0,1,0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH|FF_DONTCARE, L"Segoe UI");
    g_hFontSmall = CreateFontW(szSmall, 0,0,0, FW_NORMAL, 0,0,0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH|FF_DONTCARE, L"Segoe UI");
}
// ============================================================================
// Icon Management - Embedded Base64 Icons from ActionCenter.dll Win8.1
// ============================================================================



// Embedded icons as base64 strings
static const WCHAR icon_id0_b64[] = L"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";
static const WCHAR icon_id1_b64[] = L"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";
static const WCHAR icon_id2_b64[] = L"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";
static const WCHAR icon_id4_b64[] = L"iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAKISURBVFhH7ZdPiBJRHMc9hB208pAZaYcOHoSgW5duoQihIWwe6mJ0MQg6SGARwmTZQoSX2JHFw9qpMIJOsyCI0C2IREhYL0J4EPyDmqKrqdPv9/a9bdh0ndHZ2ct+4YPv6Xu/33fG995vRge6ADwAOAq28TvNJE0uh0fAXeAmcBU4C6wkElikYn2F3AMuA0uJBKH5RZvNtk6bM1WtVmv5fH4nnU5/jUQin6xW6zqLAXgBxVJkYDqdiuPxWBwOh2K/3xfr9fpuIpHI6PX6FzTWdQyqRCsZ6Ha7YqfTEbPZ7E8aC9eUIqlioNVqiWaz+TWNdxoDy5VqBlgswADI1lEYeAJcBGRJVQOhUOgjiwngrrgEHNR54NReU2UDsCum8XhcYHEXcBvY69D4KxtoNpukXS6Xa7A9t91u9wbLwXA6ne+MRuNL2j8aA71ej4yZTCZ05j81Go0mzfsY0N5AMpncpnl9gLYG2u32b4vFws4LskA1NRCLxT7TnHcAopkGBoPBbiaT+RYOhz/4/f5Nr9fLs7Eej4cPBoPvU6lUFhZbQ66BQqGwQ2M8Bc4BRP8ZyOVy3/GT/XYYdrv9TSAQ2IIK+Qv78wyUSqWyZOXfAPZFApFRINanYJ3Hhw78rywAE7avAJzL5doQBOFHNBr9grVglgG4SxXJBZG9L9U8A9eARboPcCaT6RUePsVisSo1UKlUajzPC5JSvYaTDmqeAbnCrcTmcA6H463P59s0GAzsdjPmPqyQATT/MgZQWHhuAQ8BNh/B50dMfGhhIoNp/mUNSHUGwDWzX2wWSW0DinVi4MTAc4AbjUZ/jsvAM4DD4nNcBvC8Z0kZil8uVhGWRTRB7gSg4eu5TvcXhIRVJI4SYh0AAAAASUVORK5CYII=";
static const WCHAR icon_id5_b64[] = L"iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAQMSURBVFhHvZdtTFNXGMdNZjDTZhLsC7O4pWZlvqAze/GL21wWGhRBtymJug+YfUGyxESyBJeFpEMZyeL4YpQQPqxLlgg4dcsSjCykDhS0WBgqatfIpsDYSjvubW9b2gLPnuf0nNqgzpZe+Se/9N7ec/7P/5x7zz3tIpQe+QSxcuiYvlswJRdPhU+Rfcj7SAHyApKRmDFwifM02Y+sQuYlZsLrQ15eXj0/fKzGx8c9AwMDd9va2rpqamrOGI3GeuGBlCJpK60As7OzMD09DZFIBEKhEExMTEw1NjZ2ZGVlfcm9NpNpOsooQCAQAFmWobOz8xb3omcqLakSYHJyEnQ63VfcbwkZpyrVAggvZBmSsp5FgM+QXCQlqRqgqqqqRXgitCpWInOlRRbHD1UOgKtitqGhoV34PoWdSPyE+2ccwOfzsePh4WEPLs8LRUVFJ0UNQWFh4QmNRnOUnz+bAIqisDYzMzO850N5vV4fr3sIWfgAzc3NF3jdD5CFDSBJkt9gMIj3BXtAFzRAXV3dWV5zD8L02ADhcHiqo6PjWnV19emysrKm0tLSU6JtSUnJqYqKiu9sNlsnPmzeVAMMDg7e5R5HkOUI0yMB7Hb7dfoU1/4Ps9n8dXl5+be4Q96ncxFgYqQL+s/lQ//5fAh4HeByuYaTnvwtSELMiNdno0iC9nn60UH3yoAI0bEJsVoslpPt7e39tbW1P9JeIAL89vNbMHTRBPd6CsB5flMsaUBs7SfrSQFeQ56mA4g1Ozv7GL18hoaGxinAmPsncLRoQR6zQNC7A/padbCv+Pnvse1u1muOnhQgVdFSEn2sBevXfmO3rZRu//IKxIIfMWgWLtlWjGKo56jDXGUagEQbTzFysHLv0rOOVi0E/t6WCBDyxWcBA9AtfURqBGDCAllXT2vdLns+xEIfkxcjPgsbKICL2rDGSVIzQCWNVPFsh5jyYSJA1L8LQp5tYhYqWeMkqRIAjTU0QnfXWogFdkFU3vkwwGQx44+e9WIWNKwTl1oBjvS16XGkRRCVdkDk3+2JABGPhREcew+oDbVlnbgyDoCGOTQyGuHUX1shPPouhEfegcMH8xjhB1vijLwNf/auE7OQw7urEqDe+YMBFPebEHRtguCdDaDcLmAE72yMf0e4X4fA72+IWaAXE9MXiDUajcbmEwCNjDSi0R585Q7mg99pAn/fy+B3vJS4BQHnanZNufEqKDfXwMgVs5gFI3l8jlhp85lngKaBc7lYeDXIV40gX8kF+bIepG59IoDc8yK7RqEI+boJnGfYLDSRB70cRFFByn8uaCT/XDOzIlKXDqRfV4B0KSf+mUAbv9ZN6DGgAUa7TWwWyIO2RQrBZgJJ6+85mvS2Hl8O88HRou39D0aewSDtJRCqAAAAAElFTkSuQmCC";  // Tray: Bianca + Triangolo
static const WCHAR icon_id6_b64[] = L"iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAARLSURBVFhHvZd/SJx1HMcf2fxRuttynid3mjk7XWLLVcoG7o/ixAVa/pFBgzCKMPr1hxQWJl00GxQIJZuEf2QQFEYkBbOkS7AoEtO2GW1e2rEs5+6cZ5qPP+/d+/Pc97HLtna3c77hxfN9Hp/7vN/P9/n+eNSoTPIYcSukLde2TJHm0fA0eZjcS4qJhcQlozCUzPMYOUJyyDXJKKL8kZ2dfUw1L6vJycmLw8PDZ7u6uvqbm5s/cjgcx8wapJrErJgChEIhrK6uYmlpCQsLC/D7/Yvt7e29SUlJr6paZVI0FsUVYG5uDrOzs/B4PCOqloypmLQpAWZmZmC1Wl9X9ZKlcLTatABmLZJKotb1CPA8ySJRaVMDNDQ0fGjWJDIr7GSjMsj2cHOTA3BWhFpbW0+ada/C/SR8ourHHWB6etpoj4+PX+T07KmsrDxuepi4XK62tLS019T59QkwPz9v3LO2tqZ++Y8CgcC08n2ObH2Ajo6OHuVbQ7Y2QDAY/NNms5nrhTFAtzRAS0vLx8rzQWLosgF0XV/s7e39vrGx8YPa2tp3qqurT5j3VlVVnaivr3+vs7PTw8EWiAxwaeQM9BY3psoPQC9wYDYnAxMHS7B49ChGPvt0TNV4kewkhv4ToK+vb1CO5t/+D6fT+UZdXd27P3Z/8sebO5K7cXchUOwADh8CDu4D7iwASouAjETMp6fgZU3rfiIz/RH+dl1GIeVvPEUEss/LR4e8KxsxJe084q6oqDj+VdtbZydvsYaQvxu4zQ7kWICiHKCQ7fwsII/XUzUDXdMwtl1b+Wm/c33XvFKAO8jV9Chxv23P6IF9J3A7n3xfLlCyRwoBWamA7YZw+y4ncE8pkKAhwPNRTXsqXOLKAaLSrUmJR1CSD6TRZD+NTZXtNZ52Xswfuk9dBFZKC+FPS8F4OITxORdXAF++owm5fPr0bcCBYmBpUVWiXngWeObxcFtd/4LmZxQ/52Y2SY24Alwo4lPnZwJ21dU5u/4dQqTOOarxLY2HyAj5ja9IasQVYMpC04xkYHcijynwW3dgRoJsCMHvNXSTczQ+TaQHThGpEVeACTGjeYjmQZr7yFzlIeiqnhlErzkc7nbiJWObFcCXx+633YhLNJ5y3AT9gQpVidr4Kp6sw680nSDnGdxr2RZ/gFEOJB8L/sIA+l5OQ5EyfoUm70sPRQapqUCQ9w/z+imnI/5BKFNpkMV+YNEphjDNuOJhgNeGePRFhOD9YgJ5dUMJ4WkoKdzLy8srcoO0FVHrNBcVGd1fEwnSxuJyLgNOutw0NY/cEPBdxEL0EnHL5nOtAc4VOMoGE7QVbnP4RgzZEzLVZMCdJxeUsfA7GUhPWRl1la8vxbLem6YmMf9z4S28ufhLLs39NOinqRzl1YjhX8RL+BkED+/xusplf1mXbIsSwugJEte/533cpBikyZuejM9pyDZ8WRbp8qaB8AYWIU37G1xt2pFGvWvBAAAAAElFTkSuQmCC";  // Tray: Bianca + X Rossa

static HICON Base64ToIcon(const WCHAR* b64) {
    if (!b64 || !*b64) return NULL;
    
    int len = lstrlenW(b64);
    while (len > 0 && b64[len - 1] == L'=') len--;
    
    DWORD outLen = (len * 3) / 4;
    BYTE* data = (BYTE*)malloc(outLen);
    if (!data) return NULL;
    
    DWORD val = 0;
    int bits = -8, pos = 0;
    for (int i = 0; i < len; i++) {
        BYTE v = B64Val(b64[i]);
        if (v == 0xFF) continue;
        val = (val << 6) | v;
        bits += 6;
        if (bits >= 0) { data[pos++] = (val >> bits) & 0xFF; bits -= 8; }
    }
    
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, outLen);
    if (!hMem) { free(data); return NULL; }
    memcpy(GlobalLock(hMem), data, outLen);
    GlobalUnlock(hMem);
    free(data);
    
    IStream* stream = NULL;
    if (FAILED(CreateStreamOnHGlobal(hMem, TRUE, &stream))) {
        GlobalFree(hMem);
        return NULL;
    }
    
    HICON hIcon = NULL;
    if (g_hGdiPlus && pGdipCreateBitmapFromStream) {
        void* bmp = NULL;
        if (pGdipCreateBitmapFromStream(stream, &bmp) == 0 && bmp) {
            if (!pGdipCreateHICON) {
                pGdipCreateHICON = (GdipCreateHICONFromBitmap_t)GetProcAddress(g_hGdiPlus, "GdipCreateHICONFromBitmap");
            }
            if (pGdipCreateHICON) pGdipCreateHICON(bmp, &hIcon);
            pGdipDisposeImage(bmp);
        }
    }
    stream->Release();
    return hIcon;
}

// Decode Base64 PNG into a GDI+ Bitmap* (caller must pGdipDisposeImage).
static void* Base64ToGdipBitmap(const WCHAR* b64) {
    if (!b64 || !*b64 || !g_hGdiPlus || !pGdipCreateBitmapFromStream) return NULL;

    int len = lstrlenW(b64);
    while (len > 0 && b64[len - 1] == L'=') len--;

    DWORD outLen = (len * 3) / 4;
    BYTE* data = (BYTE*)malloc(outLen);
    if (!data) return NULL;

    DWORD val = 0;
    int bits = -8, pos = 0;
    for (int i = 0; i < len; i++) {
        BYTE v = B64Val(b64[i]);
        if (v == 0xFF) continue;
        val = (val << 6) | v;
        bits += 6;
        if (bits >= 0) { data[pos++] = (val >> bits) & 0xFF; bits -= 8; }
    }

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, outLen);
    if (!hMem) { free(data); return NULL; }
    memcpy(GlobalLock(hMem), data, outLen);
    GlobalUnlock(hMem);
    free(data);

    IStream* stream = NULL;
    if (FAILED(CreateStreamOnHGlobal(hMem, TRUE, &stream))) {
        GlobalFree(hMem);
        return NULL;
    }

    void* bmp = NULL;
    if (pGdipCreateBitmapFromStream(stream, &bmp) != 0)
        bmp = NULL;
    stream->Release();
    return bmp;
}

// Scale a GDI+ bitmap to size x size with high-quality interpolation.
// Pixel format 32bpp ARGB = 2498570 (PixelFormat32bppARGB).
// Scale a GDI+ bitmap to size x size with high-quality interpolation.
// PixelFormat32bppARGB = 0x26200A
static void* ScaleGdipBitmapHQ(void* src, int size) {
    if (!src || size <= 0 || !pGdipCreateBitmapFromScan0 || !pGdipGetImageGraphicsContext
        || !pGdipDrawImageRectI || !pGdipDeleteGraphics || !pGdipDisposeImage) return NULL;
    void* dst = NULL;
    if (pGdipCreateBitmapFromScan0(size, size, 0, 0x26200A, NULL, &dst) != 0 || !dst)
        return NULL;
    void* gfx = NULL;
    if (pGdipGetImageGraphicsContext(dst, &gfx) != 0 || !gfx) {
        pGdipDisposeImage(dst);
        return NULL;
    }
    // InterpolationModeHighQualityBicubic = 7
    // SmoothingModeHighQuality = 2
    // CompositingQualityHighQuality = 2
    // PixelOffsetModeHighQuality = 2
    if (pGdipSetInterpolationMode) pGdipSetInterpolationMode(gfx, 7);
    if (pGdipSetSmoothingMode) pGdipSetSmoothingMode(gfx, 2);
    if (pGdipSetCompositingQuality) pGdipSetCompositingQuality(gfx, 2);
    if (pGdipSetPixelOffsetMode) pGdipSetPixelOffsetMode(gfx, 2);
    if (pGdipGraphicsClear) pGdipGraphicsClear(gfx, 0x00000000);
    pGdipDrawImageRectI(gfx, src, 0, 0, size, size);
    pGdipDeleteGraphics(gfx);
    return dst;
}

// Create a single-size 32bpp-ARGB HICON from a GDI+ bitmap (already sized).
static HICON CreateHiconFromGdipBitmapSized(void* bmp, int size) {
    if (!bmp || !pGdipCreateHBITMAPFromBitmap) return NULL;

    HBITMAP hbmColor = NULL;
    if (pGdipCreateHBITMAPFromBitmap(bmp, &hbmColor, 0x00000000) != 0 || !hbmColor)
        return NULL;

    // 1-bit AND mask (zeros = opaque; modern shells use the 32bpp alpha channel)
    HBITMAP hbmMask = CreateBitmap(size, size, 1, 1, NULL);
    if (!hbmMask) {
        DeleteObject(hbmColor);
        return NULL;
    }
    HDC hdc = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdc);
    HGDIOBJ old = SelectObject(hdcMem, hbmMask);
    PatBlt(hdcMem, 0, 0, size, size, BLACKNESS);
    SelectObject(hdcMem, old);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdc);

    ICONINFO ii = {};
    ii.fIcon = TRUE;
    ii.hbmColor = hbmColor;
    ii.hbmMask = hbmMask;
    HICON hIcon = CreateIconIndirect(&ii);
    DeleteObject(hbmColor);
    DeleteObject(hbmMask);
    return hIcon;
}

// Build an in-memory multi-image .ICO (16 + 32 + 48) and extract the size that
// the Control Panel "Notification Area Icons" list uses (typically 32).
// The system tray continues to receive a dedicated SM_CXSMICON-sized handle
// from Base64ToTrayIcon, so tray pixels stay unchanged.
static BOOL BuildMultiSizeIcoBuffer(void* srcBmp, BYTE** outBuf, DWORD* outSize) {
    if (!srcBmp || !outBuf || !outSize || !pGdipCreateHBITMAPFromBitmap) return FALSE;
    *outBuf = NULL;
    *outSize = 0;

    const int kSizes[] = { 16, 32, 48 };
    const int kCount = 3;

#pragma pack(push, 1)
    struct IconDir {
        WORD idReserved;
        WORD idType;
        WORD idCount;
    };
    struct IconDirEntry {
        BYTE  bWidth;
        BYTE  bHeight;
        BYTE  bColorCount;
        BYTE  bReserved;
        WORD  wPlanes;
        WORD  wBitCount;
        DWORD dwBytesInRes;
        DWORD dwImageOffset;
    };
#pragma pack(pop)

    BYTE* dibBits[3] = { NULL, NULL, NULL };
    DWORD dibSize[3] = { 0, 0, 0 };
    BOOL ok = TRUE;

    for (int i = 0; i < kCount; i++) {
        int w = kSizes[i];
        int h = kSizes[i];
        void* scaled = ScaleGdipBitmapHQ(srcBmp, w);
        if (!scaled) { ok = FALSE; break; }

        HBITMAP hbm = NULL;
        if (pGdipCreateHBITMAPFromBitmap(scaled, &hbm, 0x00000000) != 0 || !hbm) {
            pGdipDisposeImage(scaled);
            ok = FALSE;
            break;
        }
        pGdipDisposeImage(scaled);

        DWORD xorStride = (DWORD)(w * 4);
        DWORD xorBytes = xorStride * (DWORD)h;
        DWORD andStride = (DWORD)(((w + 31) / 32) * 4);
        DWORD andBytes = andStride * (DWORD)h;
        dibSize[i] = sizeof(BITMAPINFOHEADER) + xorBytes + andBytes;
        dibBits[i] = (BYTE*)calloc(1, dibSize[i]);
        if (!dibBits[i]) {
            DeleteObject(hbm);
            ok = FALSE;
            break;
        }

        BITMAPINFOHEADER* bih = (BITMAPINFOHEADER*)dibBits[i];
        bih->biSize = sizeof(BITMAPINFOHEADER);
        bih->biWidth = w;
        bih->biHeight = h * 2; // XOR + AND
        bih->biPlanes = 1;
        bih->biBitCount = 32;
        bih->biCompression = BI_RGB;

        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = w;
        bmi.bmiHeader.biHeight = -h; // top-down read
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        BYTE* tmp = (BYTE*)malloc(xorBytes);
        if (!tmp) {
            DeleteObject(hbm);
            ok = FALSE;
            break;
        }
        HDC hdc = GetDC(NULL);
        int got = GetDIBits(hdc, hbm, 0, h, tmp, &bmi, DIB_RGB_COLORS);
        ReleaseDC(NULL, hdc);
        DeleteObject(hbm);
        if (got == 0) {
            free(tmp);
            ok = FALSE;
            break;
        }

        BYTE* xorDst = dibBits[i] + sizeof(BITMAPINFOHEADER);
        for (int y = 0; y < h; y++) {
            memcpy(xorDst + (SIZE_T)(h - 1 - y) * xorStride, tmp + (SIZE_T)y * xorStride, xorStride);
        }
        free(tmp);
        // AND mask left zeroed (opaque); alpha lives in XOR
    }

    if (!ok) {
        for (int i = 0; i < kCount; i++) if (dibBits[i]) free(dibBits[i]);
        return FALSE;
    }

    DWORD offset = (DWORD)(sizeof(IconDir) + kCount * sizeof(IconDirEntry));
    DWORD total = offset;
    for (int i = 0; i < kCount; i++) total += dibSize[i];

    BYTE* ico = (BYTE*)malloc(total);
    if (!ico) {
        for (int i = 0; i < kCount; i++) free(dibBits[i]);
        return FALSE;
    }

    IconDir* dir = (IconDir*)ico;
    dir->idReserved = 0;
    dir->idType = 1;
    dir->idCount = (WORD)kCount;

    IconDirEntry* entries = (IconDirEntry*)(ico + sizeof(IconDir));
    DWORD cur = offset;
    for (int i = 0; i < kCount; i++) {
        entries[i].bWidth = (BYTE)kSizes[i];
        entries[i].bHeight = (BYTE)kSizes[i];
        entries[i].bColorCount = 0;
        entries[i].bReserved = 0;
        entries[i].wPlanes = 1;
        entries[i].wBitCount = 32;
        entries[i].dwBytesInRes = dibSize[i];
        entries[i].dwImageOffset = cur;
        memcpy(ico + cur, dibBits[i], dibSize[i]);
        free(dibBits[i]);
        cur += dibSize[i];
    }

    *outBuf = ico;
    *outSize = total;
    return TRUE;
}

// Pick the best matching image from an in-memory .ICO and create an HICON of
// the requested size. Parses ICONDIRENTRY so CreateIconFromResourceEx gets the
// correct per-image byte length (not "rest of file").
static HICON ExtractIconSizeFromIcoBuffer(BYTE* ico, DWORD icoSize, int cx, int cy) {
    if (!ico || icoSize < 6) return NULL;

#pragma pack(push, 1)
    struct IconDir {
        WORD idReserved;
        WORD idType;
        WORD idCount;
    };
    struct IconDirEntry {
        BYTE  bWidth;
        BYTE  bHeight;
        BYTE  bColorCount;
        BYTE  bReserved;
        WORD  wPlanes;
        WORD  wBitCount;
        DWORD dwBytesInRes;
        DWORD dwImageOffset;
    };
#pragma pack(pop)

    if (icoSize < sizeof(IconDir)) return NULL;
    IconDir* dir = (IconDir*)ico;
    if (dir->idType != 1 || dir->idCount == 0) return NULL;
    if (icoSize < sizeof(IconDir) + dir->idCount * sizeof(IconDirEntry)) return NULL;

    IconDirEntry* entries = (IconDirEntry*)(ico + sizeof(IconDir));

    // Prefer exact size match; else nearest larger; else largest available.
    int best = -1;
    int bestDiff = 0x7fffffff;
    for (WORD n = 0; n < dir->idCount; n++) {
        int w = entries[n].bWidth ? entries[n].bWidth : 256;
        int h = entries[n].bHeight ? entries[n].bHeight : 256;
        if (entries[n].dwImageOffset >= icoSize) continue;
        if (entries[n].dwBytesInRes == 0) continue;
        if (entries[n].dwImageOffset + entries[n].dwBytesInRes > icoSize) continue;
        if (w == cx && h == cy) { best = (int)n; break; }
        int diff = (w - cx) * (w - cx) + (h - cy) * (h - cy);
        // Prefer not-smaller-than-requested when possible
        if (w >= cx && h >= cy) diff -= 100000;
        if (diff < bestDiff) { bestDiff = diff; best = (int)n; }
    }
    if (best < 0) return NULL;

    return CreateIconFromResourceEx(
        ico + entries[best].dwImageOffset,
        entries[best].dwBytesInRes,
        TRUE,
        0x00030000,
        cx,
        cy,
        LR_DEFAULTCOLOR);
}

// High-quality icon for Shell_NotifyIcon.
// Source PNGs are 32x32. We rebuild a multi-size ICO (16/32/48) with bicubic
// scaling, then materialize a sharp 32x32 HICON for the shell.
//
// Why 32x32 (not SM_CXSMICON):
//  - System tray scales 32 → 16 cleanly (same as before; tray look preserved).
//  - Control Panel "Icone area di notifica" list uses ~32px and was showing a
//    soft/sgranata glyph when the only available image was poorly converted.
static HICON Base64ToTrayIcon(const WCHAR* b64) {
    void* bmp = Base64ToGdipBitmap(b64);
    if (!bmp)
        return Base64ToIcon(b64);

    HICON hIcon = NULL;
    BYTE* icoBuf = NULL;
    DWORD icoSize = 0;

    if (BuildMultiSizeIcoBuffer(bmp, &icoBuf, &icoSize) && icoBuf) {
        // 32x32 is what the CPL list needs; tray will downscale for the area.
        hIcon = ExtractIconSizeFromIcoBuffer(icoBuf, icoSize, 32, 32);
        if (!hIcon)
            hIcon = ExtractIconSizeFromIcoBuffer(icoBuf, icoSize, 16, 16);
        free(icoBuf);
    }

    if (!hIcon) {
        void* scaled = ScaleGdipBitmapHQ(bmp, 32);
        if (scaled) {
            hIcon = CreateHiconFromGdipBitmapSized(scaled, 32);
            pGdipDisposeImage(scaled);
        }
    }

    pGdipDisposeImage(bmp);

    if (!hIcon)
        hIcon = Base64ToIcon(b64);
    return hIcon;
}

static BOOL FileExistsW(const WCHAR* path) {
    return GetFileAttributesW(path) != INVALID_FILE_ATTRIBUTES;
}

static BOOL LoadActionCenterIconFromDll(const WCHAR* dllName, int index, HICON* outIcon) {
    WCHAR fullPath[MAX_PATH];
    if (wcschr(dllName, L'\\') || wcschr(dllName, L'/')) {
        if (!FileExistsW(dllName)) return FALSE;
        StringCchCopyW(fullPath, MAX_PATH, dllName);
    } else {
        UINT len = GetSystemDirectoryW(fullPath, MAX_PATH);
        if (len == 0 || len >= MAX_PATH) return FALSE;
        StringCchCatW(fullPath, MAX_PATH, L"\\");
        StringCchCatW(fullPath, MAX_PATH, dllName);
        if (!FileExistsW(fullPath)) return FALSE;
    }
    HICON hSmall = NULL;
    int count = ExtractIconExW(fullPath, index, outIcon, &hSmall, 1);
    if (hSmall) DestroyIcon(hSmall);
    return (count > 0 && *outIcon != NULL);
}

HICON LoadActionCenterIcon(int index) {
    HICON hIcon = NULL;
    if (LoadActionCenterIconFromDll(L"ActionCenterCPL.dll", index, &hIcon)) return hIcon;
    if (LoadActionCenterIconFromDll(L"actioncenter.dll", index, &hIcon)) return hIcon;
    if (LoadActionCenterIconFromDll(L"shell32.dll", 56 + index, &hIcon)) return hIcon;
    return NULL;
}

void InitFlyoutIcons() {
    Wh_Log(LOG_PREFIX L"Loading flyout icons...");
    
    g_hFlyoutIconGood    = Base64ToIcon(icon_id0_b64);
    Wh_Log(LOG_PREFIX L"Flyout GOOD: %p", g_hFlyoutIconGood);
    
    g_hFlyoutIconWarning = Base64ToIcon(icon_id1_b64);
    Wh_Log(LOG_PREFIX L"Flyout WARNING: %p", g_hFlyoutIconWarning);
    
    g_hFlyoutIconAlert   = Base64ToIcon(icon_id2_b64);
    Wh_Log(LOG_PREFIX L"Flyout ALERT: %p", g_hFlyoutIconAlert);
    // Load UAC Shield specifically from imageres.dll index 73 (small icon)
    g_hShieldIcon = NULL;
    ExtractIconExW(L"imageres.dll", 73, NULL, &g_hShieldIcon, 1);
    if (!g_hShieldIcon) {
        // Fallbacks
        g_hShieldIcon = (HICON)LoadImageW(NULL, (LPCWSTR)32518, IMAGE_ICON, 16, 16, LR_SHARED); // IDI_SHIELD
        if (!g_hShieldIcon) ExtractIconExW(L"shell32.dll", 77, NULL, &g_hShieldIcon, 1);
    }
    
    // Fallback se NULL
    if (!g_hFlyoutIconGood)    { Wh_Log(LOG_PREFIX L"FALLBACK GOOD"); g_hFlyoutIconGood = LoadActionCenterIcon(0); }
    if (!g_hFlyoutIconWarning) { Wh_Log(LOG_PREFIX L"FALLBACK WARNING"); g_hFlyoutIconWarning = LoadActionCenterIcon(1); }
    if (!g_hFlyoutIconAlert)   { Wh_Log(LOG_PREFIX L"FALLBACK ALERT"); g_hFlyoutIconAlert = LoadActionCenterIcon(2); }
}
void FreeAllIcons() {
    if (g_hFlyoutIconGood)    { DestroyIcon(g_hFlyoutIconGood);    g_hFlyoutIconGood    = NULL; }
    if (g_hFlyoutIconWarning) { DestroyIcon(g_hFlyoutIconWarning); g_hFlyoutIconWarning = NULL; }
    if (g_hFlyoutIconAlert)   { DestroyIcon(g_hFlyoutIconAlert);   g_hFlyoutIconAlert   = NULL; }
    if (g_hShieldIcon)        { DestroyIcon(g_hShieldIcon);        g_hShieldIcon        = NULL; }
}
// ============================================================================
// Security State
// ============================================================================
static BOOL IsProblemTypeAlreadyDetected(int type) {
    for (int i = 0; i < g_ActiveProblems && i < MAX_PROBLEMS; i++) {
        if (g_ProblemTypes[i] == type) return TRUE;
    }
    return FALSE;
}
static BOOL AddProblem(int type, int* idx, int* criticalCount) {
    if (*idx >= MAX_PROBLEMS) return FALSE;
    if (IsProblemTypeAlreadyDetected(type)) return FALSE;
    g_ProblemTypes[(*idx)++] = type;
    if (type == PROB_FIREWALL || type == PROB_AUTOUPDATE || type == PROB_ANTIVIRUS) (*criticalCount)++;
    return TRUE;
}
static void CheckWscProvider(DWORD provider, int problemType, int* idx, int* criticalCount) {
    WSC_SECURITY_PROVIDER_HEALTH health;
    if (WscGetSecurityProviderHealth(provider, &health) == S_OK) {
        if (health == WSC_SECURITY_PROVIDER_HEALTH_POOR) AddProblem(problemType, idx, criticalCount);
    }
}
static void CheckDefenderRealtime(int* idx, int* criticalCount) {
    RegKey hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows Defender\\Real-Time Protection", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD dwDisabled = 0, dwSize = sizeof(DWORD);
        if (RegQueryValueExW(hKey, L"DisableRealtimeMonitoring", NULL, NULL, (LPBYTE)&dwDisabled, &dwSize) == ERROR_SUCCESS) {
            if (dwDisabled != 0) AddProblem(PROB_DEFENDER_RT, idx, criticalCount);
        }
    }
}
static void CheckUACRegistry(int* idx, int* criticalCount) {
    RegKey hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD dwEnableLUA = 1, dwSize = sizeof(DWORD);
        if (RegQueryValueExW(hKey, L"EnableLUA", NULL, NULL, (LPBYTE)&dwEnableLUA, &dwSize) == ERROR_SUCCESS) {
            if (dwEnableLUA == 0) AddProblem(PROB_UAC, idx, criticalCount);
        }
    }
}
static BOOL IsServiceStartDisabled(const WCHAR* serviceName) {
    // TRUE only when start type is SERVICE_DISABLED.
    // Any failure (no rights / missing service) returns FALSE so other checks continue.
    SC_HANDLE hSCM = OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT);
    if (!hSCM) {
        Wh_Log(LOG_PREFIX L"OpenSCManagerW failed for %s: %lu", serviceName, GetLastError());
        return FALSE;
    }
    SC_HANDLE hSvc = OpenServiceW(hSCM, serviceName, SERVICE_QUERY_CONFIG);
    if (!hSvc) {
        CloseServiceHandle(hSCM);
        return FALSE;
    }
    BOOL disabled = FALSE;
    DWORD needed = 0;
    QueryServiceConfigW(hSvc, NULL, 0, &needed);
    if (needed > 0 && needed < 64 * 1024) {
        BYTE* buf = (BYTE*)malloc(needed);
        if (buf) {
            QUERY_SERVICE_CONFIGW* cfg = (QUERY_SERVICE_CONFIGW*)buf;
            if (QueryServiceConfigW(hSvc, cfg, needed, &needed)) {
                disabled = (cfg->dwStartType == SERVICE_DISABLED);
            }
            free(buf);
        }
    }
    CloseServiceHandle(hSvc);
    CloseServiceHandle(hSCM);
    return disabled;
}

static void CheckAutoUpdateRegistry(int* idx, int* criticalCount) {
    // 1) Modern GPO: NoAutoUpdate (managed / enterprise environments)
    {
        RegKey hKey;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate\\AU",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            DWORD dwNoAuto = 0, dwSize = sizeof(DWORD);
            if (RegQueryValueExW(hKey, L"NoAutoUpdate", NULL, NULL, (LPBYTE)&dwNoAuto, &dwSize) == ERROR_SUCCESS) {
                if (dwNoAuto != 0) {
                    Wh_Log(LOG_PREFIX L"AutoUpdate: NoAutoUpdate policy is set");
                    AddProblem(PROB_AUTOUPDATE, idx, criticalCount);
                    return;
                }
            }
        }
    }

    // 2) Service start type: Update Orchestrator (UsoSvc) and legacy WU (wuauserv).
    // Only SERVICE_DISABLED counts as "updates off"; stopped-but-auto is normal.
    if (IsServiceStartDisabled(L"UsoSvc") || IsServiceStartDisabled(L"wuauserv")) {
        Wh_Log(LOG_PREFIX L"AutoUpdate: update service start type is DISABLED");
        AddProblem(PROB_AUTOUPDATE, idx, criticalCount);
        return;
    }

    // 3) Legacy AUOptions (Windows 7 / early 8 style). Silent fallback when absent.
    RegKey hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update",
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD dwAUOptions = 0, dwSize = sizeof(DWORD);
        if (RegQueryValueExW(hKey, L"AUOptions", NULL, NULL, (LPBYTE)&dwAUOptions, &dwSize) == ERROR_SUCCESS) {
            if (dwAUOptions == 1) {
                Wh_Log(LOG_PREFIX L"AutoUpdate: legacy AUOptions=1 (never check)");
                AddProblem(PROB_AUTOUPDATE, idx, criticalCount);
            }
        }
    }
}

const WCHAR* GetProblemText(int problemType) {
    switch (problemType) {
        case PROB_FIREWALL:    return LOC(STR_MSG_FIREWALL);
        case PROB_AUTOUPDATE:  return LOC(STR_MSG_AUTOUPDATE);
        case PROB_ANTIVIRUS:   return LOC(STR_MSG_ANTIVIRUS);
        case PROB_ANTISPYWARE: return LOC(STR_MSG_ANTISPYWARE);
        case PROB_INTERNET:    return LOC(STR_MSG_INTERNET);
        case PROB_UAC:         return LOC(STR_MSG_UAC);
        case PROB_SERVICE:     return LOC(STR_MSG_SERVICE);
        case PROB_DEFENDER_RT: return LOC(STR_MSG_DEFENDER);
        default: return L"";
    }
}

void CheckSecurityProviders() {
    SRWGuard guard(g_Ctx.srwLock, true); // exclusive write
    g_ActiveProblems = 0;
    ZeroMemory(g_ProblemTypes, sizeof(g_ProblemTypes));
    if (g_SimulatedNotificationType > 0) {
        g_SecurityState = STATE_ALERT;
        int idx = 0;
        switch (g_SimulatedNotificationType) {
            case 1: g_ProblemTypes[0] = PROB_FIREWALL; g_ProblemTypes[1] = PROB_ANTIVIRUS; idx = 2; break;
            case 2: g_ProblemTypes[0] = PROB_AUTOUPDATE; g_ProblemTypes[1] = PROB_FIREWALL; idx = 2; break;
            case 3: g_ProblemTypes[0] = PROB_ANTISPYWARE; g_ProblemTypes[1] = PROB_UAC; idx = 2; break;
            case 4: g_ProblemTypes[0] = PROB_DEFENDER_RT; g_ProblemTypes[1] = PROB_AUTOUPDATE; idx = 2; break;
        }
        g_ActiveProblems = idx;
        return;
    }
    if (g_Settings.privacyMode) { g_SecurityState = STATE_GOOD; return; }
    int idx = 0, criticalCount = 0;
    CheckWscProvider(WSC_SECURITY_PROVIDER_FIREWALL, PROB_FIREWALL, &idx, &criticalCount);
    CheckWscProvider(WSC_SECURITY_PROVIDER_AUTOUPDATE_SETTINGS, PROB_AUTOUPDATE, &idx, &criticalCount);
    CheckWscProvider(WSC_SECURITY_PROVIDER_ANTIVIRUS, PROB_ANTIVIRUS, &idx, &criticalCount);
    CheckWscProvider(WSC_SECURITY_PROVIDER_ANTISPYWARE, PROB_ANTISPYWARE, &idx, &criticalCount);
    CheckWscProvider(WSC_SECURITY_PROVIDER_INTERNET_SETTINGS, PROB_INTERNET, &idx, &criticalCount);
    CheckWscProvider(WSC_SECURITY_PROVIDER_USER_ACCOUNT_CONTROL, PROB_UAC, &idx, &criticalCount);
    CheckWscProvider(WSC_SECURITY_PROVIDER_SERVICE, PROB_SERVICE, &idx, &criticalCount);
    CheckDefenderRealtime(&idx, &criticalCount);
    CheckUACRegistry(&idx, &criticalCount);
    CheckAutoUpdateRegistry(&idx, &criticalCount);
    // Action Center Checks registry
    RegKey hKeyChecks;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Action Center\\Checks", 0, KEY_READ, &hKeyChecks) == ERROR_SUCCESS) {
        DWORD dwIdx = 0;
        WCHAR szSubKeyName[256];
        DWORD dwSubKeySize = 256;
        while (RegEnumKeyExW(hKeyChecks, dwIdx, szSubKeyName, &dwSubKeySize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS && idx < MAX_PROBLEMS) {
            RegKey hKeySub;
            if (RegOpenKeyExW(hKeyChecks, szSubKeyName, 0, KEY_READ, &hKeySub) == ERROR_SUCCESS) {
                DWORD dwSilent = 0, dwSize = sizeof(DWORD), dwState = 0;
                BOOL isSilent = (RegQueryValueExW(hKeySub, L"Silent", NULL, NULL, (LPBYTE)&dwSilent, &dwSize) == ERROR_SUCCESS && dwSilent != 0);
                dwSize = sizeof(DWORD);
                if (!isSilent && RegQueryValueExW(hKeySub, L"State", NULL, NULL, (LPBYTE)&dwState, &dwSize) == ERROR_SUCCESS && dwState != 0 && idx < MAX_PROBLEMS) {
                    // Language-agnostic identity: subkey name + optional Id/ProviderId/CheckId,
                    // then fall back to localized DisplayName substring matching (unchanged).
                    WCHAR szIdentity[512] = { 0 };
                    StringCchCopyW(szIdentity, 512, szSubKeyName);

                    WCHAR szExtra[256] = { 0 };
                    dwSize = sizeof(szExtra);
                    if (RegQueryValueExW(hKeySub, L"ProviderId", NULL, NULL, (LPBYTE)szExtra, &dwSize) == ERROR_SUCCESS && szExtra[0]) {
                        StringCchCatW(szIdentity, 512, L"|");
                        StringCchCatW(szIdentity, 512, szExtra);
                    }
                    dwSize = sizeof(szExtra); ZeroMemory(szExtra, sizeof(szExtra));
                    if (RegQueryValueExW(hKeySub, L"Id", NULL, NULL, (LPBYTE)szExtra, &dwSize) == ERROR_SUCCESS && szExtra[0]) {
                        StringCchCatW(szIdentity, 512, L"|");
                        StringCchCatW(szIdentity, 512, szExtra);
                    }
                    dwSize = sizeof(szExtra); ZeroMemory(szExtra, sizeof(szExtra));
                    if (RegQueryValueExW(hKeySub, L"CheckId", NULL, NULL, (LPBYTE)szExtra, &dwSize) == ERROR_SUCCESS && szExtra[0]) {
                        StringCchCatW(szIdentity, 512, L"|");
                        StringCchCatW(szIdentity, 512, szExtra);
                    }

                    CharLowerW(szIdentity);

                    // Well-known / locale-independent identity patterns (subkey often "{GUID}.check.N").
                    int mappedType = PROB_NONE;
                    if (wcsstr(szIdentity, L"e8433b72-5842-4d43-8645-bc2c35960837") ||
                        wcsstr(szIdentity, L"windowsupdate") ||
                        wcsstr(szIdentity, L"autoupdate")) {
                        mappedType = PROB_AUTOUPDATE;
                    } else if (wcsstr(szIdentity, L"b54924aa-401d-4e90-a50f-78ad0e4d4f6f") ||
                               wcsstr(szIdentity, L"62f80045-1321-4d7a-8522-0d2e1d3494e3") ||
                               wcsstr(szIdentity, L"firewall") ||
                               wcsstr(szIdentity, L"mpssvc")) {
                        mappedType = PROB_FIREWALL;
                    } else if (wcsstr(szIdentity, L"antivirus") ||
                               wcsstr(szIdentity, L"antimalware") ||
                               wcsstr(szIdentity, L"virusprotection")) {
                        mappedType = PROB_ANTIVIRUS;
                    } else if (wcsstr(szIdentity, L"antispyware") ||
                               wcsstr(szIdentity, L"spyware")) {
                        mappedType = PROB_ANTISPYWARE;
                    } else if (wcsstr(szIdentity, L"useraccountcontrol") ||
                               wcsstr(szIdentity, L"enablelua") ||
                               wcsstr(szIdentity, L"uac")) {
                        mappedType = PROB_UAC;
                    } else if (wcsstr(szIdentity, L"internetsettings") ||
                               wcsstr(szIdentity, L"inetsettings") ||
                               wcsstr(szIdentity, L"zonesecurity")) {
                        mappedType = PROB_INTERNET;
                    }

                    if (mappedType != PROB_NONE && !IsProblemTypeAlreadyDetected(mappedType)) {
                        AddProblem(mappedType, &idx, &criticalCount);
                    } else if (mappedType == PROB_NONE) {
                        // Last fallback: localized DisplayName substring matching (existing behavior)
                        WCHAR szLower[256] = { 0 }; dwSize = sizeof(szLower);
                        RegQueryValueExW(hKeySub, L"DisplayName", NULL, NULL, (LPBYTE)szLower, &dwSize);
                        if (!szLower[0]) StringCchCopyW(szLower, 256, szSubKeyName);
                        CharLowerW(szLower);
                        if ((wcsstr(szLower, L"firewall") || wcsstr(szLower, L"fw")) && !IsProblemTypeAlreadyDetected(PROB_FIREWALL)) AddProblem(PROB_FIREWALL, &idx, &criticalCount);
                        else if ((wcsstr(szLower, L"antivirus") || wcsstr(szLower, L"virus")) && !IsProblemTypeAlreadyDetected(PROB_ANTIVIRUS)) AddProblem(PROB_ANTIVIRUS, &idx, &criticalCount);
                        else if ((wcsstr(szLower, L"spyware") || wcsstr(szLower, L"malware")) && !IsProblemTypeAlreadyDetected(PROB_ANTISPYWARE)) AddProblem(PROB_ANTISPYWARE, &idx, &criticalCount);
                        else if ((wcsstr(szLower, L"uac") || wcsstr(szLower, L"account")) && !IsProblemTypeAlreadyDetected(PROB_UAC)) AddProblem(PROB_UAC, &idx, &criticalCount);
                        else if ((wcsstr(szLower, L"internet") || wcsstr(szLower, L"network")) && !IsProblemTypeAlreadyDetected(PROB_INTERNET)) AddProblem(PROB_INTERNET, &idx, &criticalCount);
                        else if ((wcsstr(szLower, L"update") || wcsstr(szLower, L"autoupdate")) && !IsProblemTypeAlreadyDetected(PROB_AUTOUPDATE)) AddProblem(PROB_AUTOUPDATE, &idx, &criticalCount);
                    }
                }
            }
            dwIdx++; dwSubKeySize = 256;
        }
    }
    g_ActiveProblems = idx;
    g_SecurityState = (criticalCount > 0) ? STATE_ALERT : ((idx > 0) ? STATE_WARNING : STATE_GOOD);
}

void RefreshSecurityState() {
    int prevState;
    int prevProblems;
    { SRWGuard g(g_Ctx.srwLock, false); 
        prevState = g_SecurityState; 
        prevProblems = g_ActiveProblems; 
    }
    CheckSecurityProviders();
    int newState;
    int newProblems;
    { SRWGuard g(g_Ctx.srwLock, false); 
        newState = g_SecurityState; 
        newProblems = g_ActiveProblems; 
    }
    
    // Se il numero di problemi è cambiato, aggiorna l'altezza del flyout
    if (prevProblems != newProblems && g_Ctx.hWndFlyout && IsWindow(g_Ctx.hWndFlyout) && IsWindowVisible(g_Ctx.hWndFlyout)) {
        int newHeight = CalculateFlyoutHeight(newProblems);
        // Non ricaricare MAI le icone durante WM_PAINT: causa riavvii di explorer.
        // Ricalcola solo le metriche DPI e ridimensiona.
        RecalcDpiMetrics(g_dpi, newProblems);
        // Aggiorna altezza solo se effettivamente cambiata
        if ((newHeight > g_ScaledHeight ? newHeight - g_ScaledHeight : g_ScaledHeight - newHeight) > 1) {
            SetWindowPos(g_Ctx.hWndFlyout, NULL, 0, 0, g_ScaledWidth, newHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
            g_ScaledHeight = newHeight;
        }
        InvalidateRect(g_Ctx.hWndFlyout, NULL, TRUE);
    }
    
    if (prevState != newState && !g_Ctx.isUninitializing) {
    // Aggiorna icona tray (operazione leggera)
    UpdateTrayIcon();
    ShowBalloonNotification(prevState, newState);
    
    // NON ricaricare MAI le icone flyout qui!
    // Le icone flyout sono già caricate all'avvio e NON cambiano.
    // Ricaricarle qui causa riavvii di Explorer.
    // FreeAllIcons();  // <-- RIMOSSO
    // InitFlyoutIcons();  // <-- RIMOSSO
    
    if (g_Ctx.hWndFlyout && IsWindow(g_Ctx.hWndFlyout) && IsWindowVisible(g_Ctx.hWndFlyout)) {
        InvalidateRect(g_Ctx.hWndFlyout, NULL, TRUE);
    }
}
}

// ============================================================================
// Balloon Notification
// ============================================================================
void ShowBalloonNotification(int oldState, int newState) {
    if (!g_Ctx.hWndMsgHandler || !IsWindow(g_Ctx.hWndMsgHandler)) return;
    if (newState <= oldState) return;
    // Non mostrare notifiche balloon: può causare instabilità di explorer
    // quando il tray icon è in stato modificato di frequente.
    // L'utente vedrà comunque il flyout aggiornato.
    Wh_Log(LOG_PREFIX L"State change %d -> %d (balloon disabled for stability)", oldState, newState);
}

// ============================================================================
// WSC Notifications
// ============================================================================
DWORD WINAPI WscChangeCallback(LPVOID lpParam) {
    if (g_Ctx.isUninitializing) return 0;
    if (g_Ctx.hWndMsgHandler && IsWindow(g_Ctx.hWndMsgHandler)) {
        PostMessageW(g_Ctx.hWndMsgHandler, WM_SECURITY_CHANGED, 0, 0);
    }
    return 0;
}
void RegisterWscNotifications() {
    if (g_Ctx.hWscRegistration) return;
    HRESULT hr = WscRegisterForChanges(NULL, &g_Ctx.hWscRegistration, WscChangeCallback, NULL);
    if (FAILED(hr)) { Wh_Log(LOG_PREFIX L"WscRegisterForChanges failed: 0x%08X", hr); g_Ctx.hWscRegistration = NULL; }
    else { Wh_Log(LOG_PREFIX L"WscRegisterForChanges registered"); }
}
void UnregisterWscNotifications() {
    HANDLE hReg = (HANDLE)InterlockedExchangePointer((PVOID*)&g_Ctx.hWscRegistration, NULL);
    if (hReg) {
        WscUnRegisterChanges(hReg);
        Wh_Log(LOG_PREFIX L"WSC notifications unregistered");
    }
}

// ============================================================================
// Registry Monitor Thread
// ============================================================================
DWORD WINAPI RegistryMonitorThread(LPVOID lpParam) {
    Wh_Log(LOG_PREFIX L"Registry monitor thread started");
    InterlockedExchange(&g_Ctx.regMonitorRunning, 1);
    HANDLE hEvents[2] = { g_Ctx.hRegShutdownEvent, g_Ctx.hRegChangeEvent };
    // Progressive backoff when Action Center\Checks is missing (common on some SKUs).
    // Starts at 200ms and doubles up to 5s so we don't spin forever at 5 Hz.
    DWORD missingKeyBackoffMs = 200;
    while (!g_Ctx.isUninitializing) {
        RegKey hKey;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Action Center\\Checks", 0, KEY_READ | KEY_NOTIFY, &hKey) != ERROR_SUCCESS) {
            // Interruptible sleep: wake early on shutdown.
            if (g_Ctx.hRegShutdownEvent) {
                DWORD wr = WaitForSingleObject(g_Ctx.hRegShutdownEvent, missingKeyBackoffMs);
                if (wr == WAIT_OBJECT_0 || g_Ctx.isUninitializing) break;
            } else {
                Sleep(missingKeyBackoffMs);
            }
            if (missingKeyBackoffMs < 5000) {
                DWORD next = missingKeyBackoffMs * 2;
                missingKeyBackoffMs = (next > 5000) ? 5000 : next;
            }
            continue;
        }
        // Key is available again — reset backoff for future disappearances.
        missingKeyBackoffMs = 200;
        ResetEvent(g_Ctx.hRegChangeEvent);
        LONG lr = RegNotifyChangeKeyValue(hKey, TRUE, REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_CHANGE_ATTRIBUTES, g_Ctx.hRegChangeEvent, TRUE);
        if (lr != ERROR_SUCCESS) {
            if (g_Ctx.hRegShutdownEvent)
                WaitForSingleObject(g_Ctx.hRegShutdownEvent, 100);
            else
                Sleep(100);
            continue;
        }
        if (g_Ctx.isUninitializing) break;
        DWORD wr = WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);
        if (g_Ctx.isUninitializing) break;
        if (wr == WAIT_OBJECT_0 + 1 && g_Ctx.hWndMsgHandler && IsWindow(g_Ctx.hWndMsgHandler)) {
            PostMessageW(g_Ctx.hWndMsgHandler, WM_SECURITY_CHANGED, 0, 0);
        }
    }
    InterlockedExchange(&g_Ctx.regMonitorRunning, 0);
    Wh_Log(LOG_PREFIX L"Registry monitor thread exiting");
    return 0;
}
void StartRegistryMonitor() {
    if (g_Ctx.hRegMonitorThread) return;
    g_Ctx.hRegShutdownEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    g_Ctx.hRegChangeEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (!g_Ctx.hRegShutdownEvent || !g_Ctx.hRegChangeEvent) {
        Wh_Log(LOG_PREFIX L"Failed to create registry monitor events"); return;
    }
    g_Ctx.hRegMonitorThread = CreateThread(NULL, 0, RegistryMonitorThread, NULL, 0, NULL);
    if (!g_Ctx.hRegMonitorThread) {
        Wh_Log(LOG_PREFIX L"Failed to create registry monitor thread");
        CloseHandle(g_Ctx.hRegShutdownEvent); g_Ctx.hRegShutdownEvent = NULL;
        CloseHandle(g_Ctx.hRegChangeEvent); g_Ctx.hRegChangeEvent = NULL;
    }
}
void StopRegistryMonitor() {
    if (g_Ctx.hRegShutdownEvent) SetEvent(g_Ctx.hRegShutdownEvent);
    if (g_Ctx.hRegMonitorThread) {
        DWORD wr = WaitForSingleObject(g_Ctx.hRegMonitorThread, 5000);
        if (wr == WAIT_TIMEOUT) {
            // Non chiudere eventi/handle mentre il thread li usa: durante
            // l'unload sarebbe un use-after-close. Il thread è event-driven e
            // deve terminare prima che il codice del mod venga scaricato.
            Wh_Log(LOG_PREFIX L"Waiting for registry monitor thread to exit");
            WaitForSingleObject(g_Ctx.hRegMonitorThread, INFINITE);
        }
        CloseHandle(g_Ctx.hRegMonitorThread);
        g_Ctx.hRegMonitorThread = NULL;
    }
    if (g_Ctx.hRegShutdownEvent) { CloseHandle(g_Ctx.hRegShutdownEvent); g_Ctx.hRegShutdownEvent = NULL; }
    if (g_Ctx.hRegChangeEvent) { CloseHandle(g_Ctx.hRegChangeEvent); g_Ctx.hRegChangeEvent = NULL; }
}
HICON LoadTrayIcon(BOOL alert) {
    int secState = g_SecurityState;
    int id;
    const WCHAR* b64;

    if (secState >= STATE_ALERT)          { id = 6; b64 = icon_id6_b64; }  // Bianca + X Rossa
    else if (secState >= STATE_WARNING)    { id = 5; b64 = icon_id5_b64; }  // Bianca + Triangolo
    else                                   { id = 4; b64 = icon_id4_b64; }  // Bianca OK

    HICON hIcon = Base64ToTrayIcon(b64);
    if (hIcon) return hIcon;

    // Fallbacks from system DLLs (large icon only; free the unused small handle).
    if (LoadActionCenterIconFromDll(L"actioncenter.dll", id, &hIcon) && hIcon)
        return hIcon;
    if (LoadActionCenterIconFromDll(L"ActionCenterCPL.dll", id, &hIcon) && hIcon)
        return hIcon;

    HICON hLarge = NULL, hSmall = NULL;
    ExtractIconExW(L"shell32.dll", 56 + id, &hLarge, &hSmall, 1);
    if (hSmall) DestroyIcon(hSmall);
    return hLarge;
}
// ============================================================================
// Hook per rilevare il riavvio di Explorer e reinizializzare l'icona
// ============================================================================
typedef HWND (WINAPI *CreateWindowExW_t)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
CreateWindowExW_t CreateWindowExW_orig = NULL;

static BOOL IsShellTrayWndClass(LPCWSTR lpClassName) {
    if (!lpClassName) return FALSE;
    // Class name may be an atom (low word only) — never dereference those.
    if (IS_INTRESOURCE(lpClassName)) return FALSE;
    return (wcscmp(lpClassName, L"Shell_TrayWnd") == 0);
}

HWND WINAPI CreateWindowExW_hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, 
                                  DWORD dwStyle, int X, int Y, int nWidth, int nHeight, 
                                  HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hwnd = CreateWindowExW_orig(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    // Rileva quando la taskbar viene ricreata (dopo riavvio di Explorer).
    // Do NOT call Shell_NotifyIcon from inside CreateWindowEx: the tray is not
    // fully ready yet. Schedule a delayed recovery on the message window instead.
    if (hwnd && IsShellTrayWndClass(lpClassName)) {
        Wh_Log(LOG_PREFIX L"Shell_TrayWnd created - scheduling delayed tray icon recovery");

        if (g_Ctx.hWndMsgHandler && IsWindow(g_Ctx.hWndMsgHandler) && !g_Ctx.isUninitializing) {
            g_Ctx.trayIconAdded = FALSE;
            g_Ctx.trayRetryAttempt = 0;
            KillTimer(g_Ctx.hWndMsgHandler, TRAY_RETRY_TIMER_ID);
            KillTimer(g_Ctx.hWndMsgHandler, TRAY_HEALTH_TIMER_ID);
            // Delay: notification area needs a moment after Shell_TrayWnd exists.
            SetTimer(g_Ctx.hWndMsgHandler, TRAY_RETRY_TIMER_ID, 500, NULL);
            SetTimer(g_Ctx.hWndMsgHandler, TRAY_HEALTH_TIMER_ID, 15000, NULL);
        }
    }

    return hwnd;
}
static void BuildTrayTooltip(WCHAR* buf, size_t bufSize) {
    if (!buf || bufSize == 0) return;
    int activeProblems = 0;
    {
        SRWGuard guard(g_Ctx.srwLock, false);
        activeProblems = g_ActiveProblems;
    }
    const WCHAR* title = LOC(STR_ACTION_CENTER_TITLE);
    if (activeProblems <= 0) {
        StringCchPrintfW(buf, bufSize, L"%s - %s", title, LOC(STR_TIP_NO_ISSUES));
    } else {
        WCHAR issuesBuf[128] = {0};
        StringCchPrintfW(issuesBuf, ARRAYSIZE(issuesBuf), LOC(STR_TIP_ISSUES), activeProblems);
        StringCchPrintfW(buf, bufSize, L"%s - %s", title, issuesBuf);
    }
}

static BOOL PublishTrayIcon(BOOL preferAdd) {
    if (g_Ctx.isUninitializing || !g_Ctx.hWndMsgHandler ||
        !IsWindow(g_Ctx.hWndMsgHandler)) return FALSE;

    // Shell_NotifyIcon fails until the notification area exists.
    if (!FindWindowW(L"Shell_TrayWnd", NULL)) {
        Wh_Log(LOG_PREFIX L"PublishTrayIcon: Shell_TrayWnd not ready");
        return FALSE;
    }

    int secState;
    { SRWGuard g(g_Ctx.srwLock, false); secState = g_SecurityState; }

    HICON hNewIcon = LoadTrayIcon(secState >= STATE_ALERT);
    if (!hNewIcon) {
        Wh_Log(LOG_PREFIX L"Unable to create tray icon");
        return FALSE;
    }

    NOTIFYICONDATAW nid = {0};
    nid.cbSize = sizeof(nid);
    nid.hWnd = g_Ctx.hWndMsgHandler;
    nid.uID = TRAY_ICON_ID;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_GUID | NIF_SHOWTIP;
    nid.uCallbackMessage = WM_TRAY_ICON_MSG;
    nid.guidItem = TRAY_ICON_GUID;
    nid.hIcon = hNewIcon;
    WCHAR tipBuf[256] = {0};
    BuildTrayTooltip(tipBuf, ARRAYSIZE(tipBuf));
    StringCchCopyW(nid.szTip, ARRAYSIZE(nid.szTip), tipBuf);

    BOOL ok = FALSE;
    if (preferAdd || !g_Ctx.trayIconAdded) {
        // After explorer restart the GUID may still be registered against a
        // dead HWND. Delete first, then ADD cleanly.
        NOTIFYICONDATAW delNid = {0};
        delNid.cbSize = sizeof(delNid);
        delNid.hWnd = nid.hWnd;
        delNid.uID = nid.uID;
        delNid.uFlags = NIF_GUID;
        delNid.guidItem = nid.guidItem;
        Shell_NotifyIconW(NIM_DELETE, &delNid);

        ok = Shell_NotifyIconW(NIM_ADD, &nid);
        if (!ok) {
            // Retry without GUID (some tray rebuilds reject GUID reuse briefly)
            NOTIFYICONDATAW nidNoGuid = nid;
            nidNoGuid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP;
            ZeroMemory(&nidNoGuid.guidItem, sizeof(nidNoGuid.guidItem));
            ok = Shell_NotifyIconW(NIM_ADD, &nidNoGuid);
            if (ok) {
                nid = nidNoGuid;
                Wh_Log(LOG_PREFIX L"Tray icon added without GUID (fallback)");
            }
        }
        if (!ok) ok = Shell_NotifyIconW(NIM_MODIFY, &nid);
    } else {
        ok = Shell_NotifyIconW(NIM_MODIFY, &nid);
        if (!ok) {
            // MODIFY failed (icon lost after explorer restart) -> full re-add
            NOTIFYICONDATAW delNid = {0};
            delNid.cbSize = sizeof(delNid);
            delNid.hWnd = nid.hWnd;
            delNid.uID = nid.uID;
            delNid.uFlags = NIF_GUID;
            delNid.guidItem = nid.guidItem;
            Shell_NotifyIconW(NIM_DELETE, &delNid);
            ok = Shell_NotifyIconW(NIM_ADD, &nid);
            if (!ok) {
                NOTIFYICONDATAW nidNoGuid = nid;
                nidNoGuid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP;
                ZeroMemory(&nidNoGuid.guidItem, sizeof(nidNoGuid.guidItem));
                ok = Shell_NotifyIconW(NIM_ADD, &nidNoGuid);
                if (ok) nid = nidNoGuid;
            }
        }
    }

    if (!ok) {
        DestroyIcon(hNewIcon);
        g_Ctx.trayIconAdded = FALSE;
        Wh_Log(LOG_PREFIX L"Shell_NotifyIcon failed: %lu", GetLastError());
        return FALSE;
    }

    // Imposta la versione dopo ogni ADD/MODIFY riuscito
    NOTIFYICONDATAW versionNid = {0};
    versionNid.cbSize = sizeof(versionNid);
    versionNid.hWnd = nid.hWnd;
    versionNid.uID = nid.uID;
    versionNid.uFlags = (nid.uFlags & NIF_GUID) ? NIF_GUID : 0;
    versionNid.guidItem = nid.guidItem;
    versionNid.uVersion = NOTIFYICON_VERSION_4;
    Shell_NotifyIconW(NIM_SETVERSION, &versionNid);

    HICON hOldIcon = g_nid.hIcon;
    g_nid = nid;
    g_Ctx.trayIconAdded = TRUE;
    if (hOldIcon && hOldIcon != hNewIcon) DestroyIcon(hOldIcon);
    Wh_Log(LOG_PREFIX L"Tray icon published (preferAdd=%d)", (int)preferAdd);
    return TRUE;
}

void UpdateTrayIcon() {
    if (!PublishTrayIcon(FALSE)) ScheduleTrayIconRecovery();
}

void AddTrayIcon() {
    if (g_Ctx.isUninitializing) return;
    CheckSecurityProviders();
    if (!PublishTrayIcon(TRUE)) ScheduleTrayIconRecovery();
}

static BOOL IsTrayIconReachable() {
    if (!g_Ctx.trayIconAdded || !g_Ctx.hWndMsgHandler ||
        !IsWindow(g_Ctx.hWndMsgHandler)) return FALSE;
    NOTIFYICONIDENTIFIER id = { sizeof(id) };
    id.hWnd = g_Ctx.hWndMsgHandler;
    id.uID = TRAY_ICON_ID;
    id.guidItem = TRAY_ICON_GUID;
    RECT rc = {0};
    if (Shell_NotifyIconGetRect(&id, &rc) == S_OK) return TRUE;
    // Fallback without GUID (used when ADD fell back to non-GUID path)
    ZeroMemory(&id.guidItem, sizeof(id.guidItem));
    return Shell_NotifyIconGetRect(&id, &rc) == S_OK;
}
static void ScheduleTrayIconRecovery() {
    if (g_Ctx.isUninitializing || !g_Ctx.hWndMsgHandler ||
        !IsWindow(g_Ctx.hWndMsgHandler)) return;

    // Reset completo dello stato
    g_Ctx.trayIconAdded = FALSE;
    g_Ctx.trayRetryAttempt = 0;

    // Cancella e reimposta entrambi i timer
    KillTimer(g_Ctx.hWndMsgHandler, TRAY_RETRY_TIMER_ID);
    KillTimer(g_Ctx.hWndMsgHandler, TRAY_HEALTH_TIMER_ID);

    // First attempt soon; health check keeps retrying if explorer restarts.
    SetTimer(g_Ctx.hWndMsgHandler, TRAY_RETRY_TIMER_ID, 250, NULL);
    SetTimer(g_Ctx.hWndMsgHandler, TRAY_HEALTH_TIMER_ID, 15000, NULL);

    Wh_Log(LOG_PREFIX L"Tray icon recovery scheduled");
}

static void RunTrayIconRecoveryAttempt() {
    if (g_Ctx.isUninitializing) return;
    KillTimer(g_Ctx.hWndMsgHandler, TRAY_RETRY_TIMER_ID);

    // Wait until the taskbar exists before calling Shell_NotifyIcon.
    if (!FindWindowW(L"Shell_TrayWnd", NULL)) {
        UINT attempt = ++g_Ctx.trayRetryAttempt;
        UINT delay = (attempt < 10) ? 300 : 1000;
        if (attempt < 60) {
            SetTimer(g_Ctx.hWndMsgHandler, TRAY_RETRY_TIMER_ID, delay, NULL);
            Wh_Log(LOG_PREFIX L"Tray not ready yet (attempt %u), retry in %ums", attempt, delay);
        } else {
            Wh_Log(LOG_PREFIX L"Tray icon recovery paused (no Shell_TrayWnd) after %u attempts", attempt);
        }
        return;
    }

    // Forza un refresh dello stato di sicurezza prima di riprovare
    RefreshSecurityState();

    // Prefer a full re-add after explorer restart.
    if (PublishTrayIcon(TRUE)) {
        // IsTrayIconReachable can lag briefly after NIM_ADD; treat ADD success
        // as good enough, and let health timer re-check.
        g_Ctx.trayRetryAttempt = 0;
        Wh_Log(LOG_PREFIX L"Tray icon available after taskbar restart");
        KillTimer(g_Ctx.hWndMsgHandler, TRAY_HEALTH_TIMER_ID);
        SetTimer(g_Ctx.hWndMsgHandler, TRAY_HEALTH_TIMER_ID, 15000, NULL);
        return;
    }

    UINT attempt = ++g_Ctx.trayRetryAttempt;
    if (attempt >= 40) {
        Wh_Log(LOG_PREFIX L"Tray icon recovery paused after %u attempts", attempt);
        return; // Il timer di health riproverà più tardi.
    }

    // Backoff esponenziale
    UINT delay = 150u << (attempt < 6 ? attempt : 6);
    if (delay > 8000) delay = 8000;
    SetTimer(g_Ctx.hWndMsgHandler, TRAY_RETRY_TIMER_ID, delay, NULL);

    Wh_Log(LOG_PREFIX L"Retry attempt %u scheduled in %ums", attempt, delay);
}

// ============================================================================
// Notifications
// ============================================================================
void ClearNotifications() {
    { SRWGuard g(g_Ctx.srwLock, true); g_SimulatedNotificationType = 0; }
    CheckSecurityProviders();
    if (g_Ctx.hWndNotify && IsWindow(g_Ctx.hWndNotify) && !g_Ctx.isUninitializing) {
        ShowWindow(g_Ctx.hWndNotify, SW_HIDE);
        KillTimer(g_Ctx.hWndNotify, NOTIFY_TIMER_ID);
        g_NotifyShowing = FALSE;
    }
    if (!g_Ctx.isUninitializing) UpdateTrayIcon();
    if (g_Ctx.hWndFlyout && IsWindow(g_Ctx.hWndFlyout) && IsWindowVisible(g_Ctx.hWndFlyout) && !g_Ctx.isUninitializing) {
        InvalidateRect(g_Ctx.hWndFlyout, NULL, TRUE);
    }
}
void SimulateNotification(int type) {
    if (g_Ctx.isUninitializing) return;
    { SRWGuard g(g_Ctx.srwLock, true); g_SimulatedNotificationType = type; }
    CheckSecurityProviders();
    UpdateTrayIcon();
    
    // 🛑 RIMUOVI IL POPUP DI NOTIFICA - MOSTRA SOLO IL FLYOUT
    // Oppure chiudi il popup se era aperto
    if (g_Ctx.hWndNotify && IsWindow(g_Ctx.hWndNotify) && g_NotifyShowing) {
        ShowWindow(g_Ctx.hWndNotify, SW_HIDE);
        KillTimer(g_Ctx.hWndNotify, NOTIFY_TIMER_ID);
        g_NotifyShowing = FALSE;
    }
    
    // AGGIORNA IL FLYOUT SE È APERTO
    if (g_Ctx.hWndFlyout && IsWindow(g_Ctx.hWndFlyout) && IsWindowVisible(g_Ctx.hWndFlyout) && !g_Ctx.isUninitializing) {
        InvalidateRect(g_Ctx.hWndFlyout, NULL, TRUE);
    }
}

// ============================================================================
// Window Positioning
// ============================================================================
void PositionWindowNearTray(HWND hwnd) {
    NOTIFYICONIDENTIFIER nidIcon = { sizeof(NOTIFYICONIDENTIFIER) };
    nidIcon.hWnd = g_Ctx.hWndMsgHandler; nidIcon.uID = TRAY_ICON_ID; nidIcon.guidItem = TRAY_ICON_GUID;
    RECT rcIcon = { 0 }; POINT ptAnchor = { 0 };
    if (Shell_NotifyIconGetRect(&nidIcon, &rcIcon) == S_OK) {
        ptAnchor.x = rcIcon.right; ptAnchor.y = (rcIcon.top + rcIcon.bottom) / 2;
    } else {
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcIcon, 0);
        ptAnchor.x = rcIcon.right; ptAnchor.y = rcIcon.bottom;
    }
    SIZE szFlyout = { g_ScaledWidth, g_ScaledHeight };
    RECT rcExclude = rcIcon, rcResult = { 0 };
    if (CalculatePopupWindowPosition(&ptAnchor, &szFlyout, TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_VERTICAL, &rcExclude, &rcResult)) {
        SetWindowPos(hwnd, HWND_TOPMOST, rcResult.left, rcResult.top, g_ScaledWidth, g_ScaledHeight, SWP_NOACTIVATE);
    } else {
        RECT rcWork; SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);
        int x = rcWork.right - g_ScaledWidth - ScaleDpi(10);
        int y = rcWork.bottom - g_ScaledHeight - ScaleDpi(6);
        SetWindowPos(hwnd, HWND_TOPMOST, x, y, g_ScaledWidth, g_ScaledHeight, SWP_NOACTIVATE);
    }
}
void ToggleFlyout() {
    if (g_Ctx.isUninitializing) return;
    
    // Dismiss notification popup if showing (Win7 behavior)
    if (g_NotifyShowing && g_Ctx.hWndNotify && IsWindow(g_Ctx.hWndNotify)) {
        ShowWindow(g_Ctx.hWndNotify, SW_HIDE);
        KillTimer(g_Ctx.hWndNotify, NOTIFY_TIMER_ID);
        g_NotifyShowing = FALSE;
    }
    
    // Ricalcola altezza in base ai problemi correnti
    int activeProblems;
    { SRWGuard guard(g_Ctx.srwLock, false); activeProblems = g_ActiveProblems; }
    RecalcDpiMetrics(g_dpi, activeProblems);
    
    // Toggle: if visible -> close; if hidden (autohide) -> re-show; else create.
    if (g_Ctx.hWndFlyout && IsWindow(g_Ctx.hWndFlyout) && !g_FlyoutClosing) {
        if (IsWindowVisible(g_Ctx.hWndFlyout)) {
            CloseFlyout(g_Ctx.hWndFlyout);
            return;
        }
        // Was auto-hidden: re-show without recreating.
        CheckSecurityProviders();
        PositionWindowNearTray(g_Ctx.hWndFlyout);
        if (g_Settings.useRoundedCorners) {
            POINT pt = AdjustWindowPosForTaskbar(g_Ctx.hWndFlyout);
            SetWindowPos(g_Ctx.hWndFlyout, NULL, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
        ShowWindow(g_Ctx.hWndFlyout, SW_SHOWNOACTIVATE);
        UpdateWindow(g_Ctx.hWndFlyout);
        InvalidateRect(g_Ctx.hWndFlyout, NULL, TRUE);
        KillTimer(g_Ctx.hWndFlyout, AUTOHIDE_TIMER_ID);
        SetTimer(g_Ctx.hWndFlyout, AUTOHIDE_TIMER_ID, AUTOHIDE_INACTIVITY_MS, NULL);
        InstallClickOutsideHook();
        return;
    }
    CreateFlyoutWindow();
    if (g_Ctx.hWndFlyout) {
        CheckSecurityProviders();
        PositionWindowNearTray(g_Ctx.hWndFlyout);
        if (g_Settings.useRoundedCorners) {
            POINT pt = AdjustWindowPosForTaskbar(g_Ctx.hWndFlyout);
            SetWindowPos(g_Ctx.hWndFlyout, NULL, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
        ShowWindow(g_Ctx.hWndFlyout, SW_SHOWNOACTIVATE);
        UpdateWindow(g_Ctx.hWndFlyout);
        AnimateWindow(g_Ctx.hWndFlyout, 180, AW_SLIDE | AW_VER_NEGATIVE);
        InvalidateRect(g_Ctx.hWndFlyout, NULL, TRUE);
        
        // Reset inactivity autohide timer every time the flyout is shown.
        KillTimer(g_Ctx.hWndFlyout, AUTOHIDE_TIMER_ID);
        SetTimer(g_Ctx.hWndFlyout, AUTOHIDE_TIMER_ID, AUTOHIDE_INACTIVITY_MS, NULL);
        
        InstallClickOutsideHook();
    }
}

// ============================================================================
// Mouse Hook (Click Outside) - Versione semplificata
// ============================================================================
void InstallClickOutsideHook() {
    if (g_hMouseHook) return;
    
    // Usa WH_MOUSE_LL globale
    g_hMouseHook = SetWindowsHookExW(WH_MOUSE_LL, ClickOutsideMouseHookProc, GetModuleHandle(NULL), 0);
    if (!g_hMouseHook) {
        Wh_Log(LOG_PREFIX L"Failed to install mouse hook (error: %lu)", GetLastError());
    } else {
        Wh_Log(LOG_PREFIX L"Global mouse hook installed");
    }
}

void RemoveClickOutsideHook() {
    if (g_hMouseHook) {
        UnhookWindowsHookEx(g_hMouseHook);
        g_hMouseHook = NULL;
        Wh_Log(LOG_PREFIX L"Mouse hook removed");
    }
}

LRESULT CALLBACK ClickOutsideMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION &&
        (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN ||
         wParam == WM_MBUTTONDOWN || wParam == WM_NCLBUTTONDOWN || wParam == WM_NCRBUTTONDOWN)) {
        if (g_Ctx.hWndFlyout && IsWindow(g_Ctx.hWndFlyout) && IsWindowVisible(g_Ctx.hWndFlyout) && !g_FlyoutClosing) {
            MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lParam;
            RECT rcFlyout; GetWindowRect(g_Ctx.hWndFlyout, &rcFlyout);
            if (!PtInRect(&rcFlyout, pMouse->pt)) {
                RECT rcIcon; NOTIFYICONIDENTIFIER nidIcon = { sizeof(NOTIFYICONIDENTIFIER) };
                nidIcon.hWnd = g_Ctx.hWndMsgHandler; nidIcon.uID = TRAY_ICON_ID; nidIcon.guidItem = TRAY_ICON_GUID;
                BOOL overTrayIcon = (Shell_NotifyIconGetRect(&nidIcon, &rcIcon) == S_OK) && PtInRect(&rcIcon, pMouse->pt);
                if (!overTrayIcon) {
                    PostMessageW(g_Ctx.hWndFlyout, WM_SAFE_CLOSE, 0, 0);
                }
            }
        }
    }
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

// ============================================================================
// Flyout Window
// ============================================================================
void CloseFlyout(HWND hwnd) {
    if (g_FlyoutClosing || !hwnd || !IsWindow(hwnd)) return;
    g_FlyoutClosing = TRUE;
    RemoveClickOutsideHook();
    AnimateWindow(hwnd, 150, AW_HIDE);
    // DestroyWindow posts WM_DESTROY, which clears g_Ctx.hWndFlyout if hwnd matches.
    DestroyWindow(hwnd);
}

LRESULT CALLBACK FlyoutWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_NCHITTEST:
    {
        LRESULT lr = DefWindowProcW(hwnd, uMsg, wParam, lParam);
        switch (lr)
        {
            case HTTOP: case HTTOPRIGHT: case HTRIGHT: case HTBOTTOMRIGHT:
            case HTBOTTOM: case HTBOTTOMLEFT: case HTLEFT: case HTTOPLEFT:
                return HTBORDER;
            default: return lr;
        }
    }

    case WM_CREATE: {
        InterlockedIncrement(&g_Ctx.refCount);
        SetTimer(hwnd, AUTOHIDE_TIMER_ID, AUTOHIDE_INACTIVITY_MS, NULL);
        if (g_Ctx.darkMode) {
            BOOL useDark = TRUE;
            DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDark, sizeof(useDark));
        }

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
        break;
    }
    case WM_DPICHANGED: {
        RecalcDpiMetrics(HIWORD(wParam));
        InitGlobalFonts();
        RECT* prcNew = (RECT*)lParam;
        SetWindowPos(hwnd, NULL, prcNew->left, prcNew->top, prcNew->right - prcNew->left, prcNew->bottom - prcNew->top, SWP_NOZORDER | SWP_NOACTIVATE);
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }
    case WM_TIMER:
        if (wParam == AUTOHIDE_TIMER_ID) {
            // Inactivity timeout: hide instead of destroy (keeps window ready).
            RemoveClickOutsideHook();
            ShowWindow(hwnd, SW_HIDE);
            return 0;
        }
        break;
    case WM_ERASEBKGND: return 1;
    case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    case WM_SAFE_CLOSE: CloseFlyout(hwnd); return 0;
    case WM_CLOSE: 
        // Come network flyout: nascondi invece di distruggere
        ShowWindow(hwnd, SW_HIDE); 
        return 0;
    case WM_ACTIVATE:
        // Do NOT hide the flyout on deactivation.
        // Hovering the tray icon / taskbar can steal activation and was
        // causing rare spontaneous closes. Closing is handled by:
        //  - click-outside mouse hook
        //  - inactivity autohide timer (120s)
        //  - Escape / explicit close
        if (LOWORD(wParam) == WA_INACTIVE) {
            // Soft delay only as a safety net if the mouse hook fails; do not
            // force-hide immediately (that was the hover-close bug).
            KillTimer(hwnd, AUTOHIDE_TIMER_ID);
            SetTimer(hwnd, AUTOHIDE_TIMER_ID, AUTOHIDE_INACTIVITY_MS, NULL);
        } else {
            KillTimer(hwnd, AUTOHIDE_TIMER_ID);
            SetTimer(hwnd, AUTOHIDE_TIMER_ID, AUTOHIDE_INACTIVITY_MS, NULL);
        }
        break;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) { 
            ShowWindow(hwnd, SW_HIDE); 
            return 0; 
        }
        if (wParam == VK_RETURN || wParam == VK_SPACE) {
            ShellExecuteW(NULL, L"open", L"control.exe", L"/name Microsoft.ActionCenter", NULL, SW_SHOWNORMAL);
            ShowWindow(hwnd, SW_HIDE);
            return 0;
        }
        break;
    case WM_SETCURSOR:
        SetCursor(LoadCursor(NULL, (g_IsHoveringLink || g_HoveredProblemIndex >= 0) ? IDC_HAND : IDC_ARROW));
        return TRUE;
    case WM_LBUTTONDOWN: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        
        // Check click on problem links
        for (int i = 0; i < g_DisplayProblemCount; i++) {
            if (PtInRect(&g_ProblemLinkRects[i], pt)) {
                Wh_Log(LOG_PREFIX L"Clicked problem link: %d", g_ProblemTypesDisplay[i]);
                
                // Open appropriate action
                OpenProblemAction(g_ProblemTypesDisplay[i]);
                
                // Close flyout
                ShowWindow(hwnd, SW_HIDE);
                return 0;
            }
        }
        
        // "You can use Action Center..." text is intentionally non-clickable.

        // Check click on footer link (existing)
        if (PtInRect(&g_rcFooterLink, pt)) {
            ShellExecuteW(NULL, L"open", L"control.exe", L"/name Microsoft.ActionCenter", NULL, SW_SHOWNORMAL);
            ShowWindow(hwnd, SW_HIDE);
            return 0;
        }
        break;
    }
    case WM_MOUSEMOVE: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        

        // Keep the flyout open while the user is interacting with it.
        KillTimer(hwnd, AUTOHIDE_TIMER_ID);
        SetTimer(hwnd, AUTOHIDE_TIMER_ID, AUTOHIDE_INACTIVITY_MS, NULL);
        // Check hover on problem links
        int newHover = -1;
        for (int i = 0; i < g_DisplayProblemCount; i++) {
            if (PtInRect(&g_ProblemLinkRects[i], pt)) {
                newHover = i;
                break;
            }
        }
        
        // Update hover state if changed
        if (newHover != g_HoveredProblemIndex) {
            g_HoveredProblemIndex = newHover;
            InvalidateRect(hwnd, NULL, FALSE);
            
            // Change cursor
            SetCursor(LoadCursor(NULL, (newHover != -1) ? IDC_HAND : IDC_ARROW));
        }
        
        // Check hover on footer link (existing)
        RECT rcFooter = { 0, g_ScaledHeight - g_ScaledFooterHeight, g_ScaledWidth, g_ScaledHeight };
        BOOL was = g_IsHoveringLink;
        g_IsHoveringLink = PtInRect(&rcFooter, pt) != 0;

        // "You can use Action Center..." text has no hover effect.
        g_IsHoveringNoProblems = FALSE;

        if (was != g_IsHoveringLink) {
            InvalidateRect(hwnd, NULL, FALSE);
            BOOL anyHover = g_IsHoveringLink || (g_HoveredProblemIndex >= 0);
            SetCursor(LoadCursor(NULL, anyHover ? IDC_HAND : IDC_ARROW));
            TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0 }; TrackMouseEvent(&tme);
        }
        break;
    }
    case WM_MOUSELEAVE: 
        g_IsHoveringLink = FALSE;
        g_IsHoveringNoProblems = FALSE;
        g_HoveredProblemIndex = -1;
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        InvalidateRect(hwnd, NULL, FALSE); 
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (!hdc) { EndPaint(hwnd, &ps); break; }
        HDC hdcMem = CreateCompatibleDC(hdc);
        if (!hdcMem) { EndPaint(hwnd, &ps); break; }
        HBITMAP hbmMem = CreateCompatibleBitmap(hdc, g_ScaledWidth, g_ScaledHeight);
        if (!hbmMem) { DeleteDC(hdcMem); EndPaint(hwnd, &ps); break; }
        HBITMAP hOldBm = (HBITMAP)SelectObject(hdcMem, hbmMem);
        if (!hOldBm) { DeleteObject(hbmMem); DeleteDC(hdcMem); EndPaint(hwnd, &ps); break; }
        int borderW = g_BorderPenWidth;
        BOOL dark = g_Ctx.darkMode;
        COLORREF clrBg = dark ? COLOR_DARK_BG : COLOR_BG;
        COLORREF clrHeaderBg = dark ? COLOR_DARK_HEADER_BG : COLOR_HEADER_BG;
        COLORREF clrFooterBg = dark ? COLOR_DARK_FOOTER_BG : COLOR_FOOTER_BG;
        COLORREF clrBorderLine1 = dark ? COLOR_DARK_BORDER_LINE1 : COLOR_BORDER_LINE1;
        COLORREF clrTitle = dark ? COLOR_DARK_TITLE : COLOR_TITLE;
        COLORREF clrLink = dark ? COLOR_DARK_LINK : COLOR_LINK;
        COLORREF clrLinkHover = dark ? COLOR_DARK_LINK_HOVER : COLOR_LINK_HOVER;
        COLORREF clrOuterBorder = dark ? RGB(80,80,80) : RGB(112,128,144);
        COLORREF clrInnerBorder = dark ? RGB(50,50,50) : RGB(255,255,255);

        int padL = ScaleDpi(10), padR = ScaleDpi(10);
        int hdrH = g_ScaledHeaderHeight, ftrH = g_ScaledFooterHeight;
        
        // Sfondo
        RECT rc = {0,0,g_ScaledWidth,g_ScaledHeight};
        GdiObj hBrBg(CreateSolidBrush(clrBg)); FillRect(hdcMem, &rc, (HBRUSH)hBrBg.get());
        
        // Header
        RECT rcHdr = {0,0,g_ScaledWidth,hdrH};
        GdiObj hBrHdr(CreateSolidBrush(clrHeaderBg)); FillRect(hdcMem, &rcHdr, (HBRUSH)hBrHdr.get());
        { GdiObj hPen(CreatePen(PS_SOLID,borderW,clrBorderLine1)); SelectGuard sg(hdcMem,hPen); MoveToEx(hdcMem,0,hdrH,NULL); LineTo(hdcMem,g_ScaledWidth,hdrH); }
        
        // Footer
        RECT rcFtr = {0,g_ScaledHeight-ftrH,g_ScaledWidth,g_ScaledHeight};
        GdiObj hBrFtr(CreateSolidBrush(clrFooterBg)); FillRect(hdcMem, &rcFtr, (HBRUSH)hBrFtr.get());
        { GdiObj hPen1(CreatePen(PS_SOLID,borderW,clrBorderLine1)); SelectGuard sg(hdcMem,hPen1); MoveToEx(hdcMem,0,g_ScaledHeight-ftrH,NULL); LineTo(hdcMem,g_ScaledWidth,g_ScaledHeight-ftrH); }
        
        SetBkMode(hdcMem, TRANSPARENT);
        
        // Leggi stato
        int activeProblems, problemTypesCopy[MAX_PROBLEMS];
        { SRWGuard guard(g_Ctx.srwLock, false);
          activeProblems = g_ActiveProblems;
          memcpy(problemTypesCopy, g_ProblemTypes, sizeof(g_ProblemTypes)); }
        
        // Icona header
        HICON hIcon = g_hFlyoutIconGood; 
        int flagSize = ScaleDpi(34);
        DrawIconEx(hdcMem, padL, (hdrH - flagSize) / 2, hIcon, flagSize, flagSize, 0, NULL, DI_NORMAL);
        
        int txL = padL + flagSize + ScaleDpi(8);

if (activeProblems > 0) {
    // "N important messages" in blu e bold (prima riga)
    WCHAR headerBuf[64] = {0};
    const WCHAR* singular = LOC(STR_SUBTITLE_ALERT1);
    const WCHAR* wordPart = wcschr(singular, L' ');
    if (wordPart) {
        const WCHAR* base = (activeProblems == 1) ? singular : LOC(STR_SUBTITLE_ALERT2);
        const WCHAR* wp = wcschr(base, L' ');
        if (wp) {
            StringCchPrintfW(headerBuf, ARRAYSIZE(headerBuf), L"%d%s", activeProblems, wp);
        } else {
            StringCchPrintfW(headerBuf, ARRAYSIZE(headerBuf), L"%d %s", activeProblems, base);
        }
    } else {
        StringCchPrintfW(headerBuf, ARRAYSIZE(headerBuf), L"%d %s", activeProblems, singular);
    }
    
    // Prima riga: "N important messages" in blu e bold
    SelectGuard sg(hdcMem, g_hFontBold);
    SetTextColor(hdcMem, clrLink);  // BLU
    RECT rcT = {txL, ScaleDpi(2), g_ScaledWidth - padR, ScaleDpi(22)};
    DrawTextW(hdcMem, headerBuf, -1, &rcT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    
    // Seconda riga: "N total messages" in blu (non bold)
    SelectGuard sg2(hdcMem, g_hFontNormal);
    SetTextColor(hdcMem, clrLink);  // BLU (stesso colore dei link)
    WCHAR totalBuf[64] = {0};
    const WCHAR* totalText;
    switch (g_CurrentLocalePack->langId) {
        case 0x0410: // Italiano
            totalText = (activeProblems == 1) ? L"messaggio totale" : L"messaggi totali";
            break;
        case 0x040A: // Español
            totalText = (activeProblems == 1) ? L"mensaje total" : L"mensajes totales";
            break;
        case 0x040C: // Français
            totalText = (activeProblems == 1) ? L"message total" : L"messages totaux";
            break;
        case 0x0419: // Русский
            if (activeProblems % 10 == 1 && activeProblems % 100 != 11)
                totalText = L"\u0432\u0441\u0435\u0433\u043E \u0441\u043E\u043E\u0431\u0449\u0435\u043D\u0438\u0435";   // сообщение
            else if (activeProblems % 10 >= 2 && activeProblems % 10 <= 4 && (activeProblems % 100 < 10 || activeProblems % 100 >= 20))
                totalText = L"\u0432\u0441\u0435\u0433\u043E \u0441\u043E\u043E\u0431\u0449\u0435\u043D\u0438\u044F";   // сообщения
            else
                totalText = L"\u0432\u0441\u0435\u0433\u043E \u0441\u043E\u043E\u0431\u0449\u0435\u043D\u0438\u0439";   // сообщений
            break;
        default:     // English
            totalText = (activeProblems == 1) ? L"total message" : L"total messages";
            break;
    }
    StringCchPrintfW(totalBuf, ARRAYSIZE(totalBuf), L"%d %s", activeProblems, totalText);
    
    // Riduci lo spazio tra le righe del 5% (da ScaleDpi(22) a ScaleDpi(21))
    RECT rcTotal = {txL, ScaleDpi(21), g_ScaledWidth - padR, ScaleDpi(41)};
    DrawTextW(hdcMem, totalBuf, -1, &rcTotal, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
} else {
        // "Action Center" in blu e bold
        SelectGuard sg(hdcMem, g_hFontBold);
        SetTextColor(hdcMem, clrLink);
        RECT rcTitleLine = {txL, ScaleDpi(3), g_ScaledWidth - padR, ScaleDpi(22)};
        DrawTextW(hdcMem, LOC(STR_ACTION_CENTER_TITLE), -1, &rcTitleLine, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        
        // "No current issues detected." in blu (non bold)
        SelectGuard sg2(hdcMem, g_hFontNormal);
        SetTextColor(hdcMem, clrLink);
        RECT rcSubLine = {txL, ScaleDpi(22), g_ScaledWidth - padR, ScaleDpi(42)};
        const WCHAR* npText = LOC(STR_NO_PROBLEMS);
        const WCHAR* npNewline = wcschr(npText, L'\n');
        if (npNewline) {
            WCHAR npLine1[128] = {0};
            int npLen = (int)(npNewline - npText);
            if (npLen > 127) npLen = 127;
            StringCchCopyNW(npLine1, ARRAYSIZE(npLine1), npText, npLen);
            DrawTextW(hdcMem, npLine1, -1, &rcSubLine, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        } else {
            DrawTextW(hdcMem, npText, -1, &rcSubLine, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        }
    }

        // ============================================================
        // MESSAGGI / PROBLEMI CON WRAPPING E ALTEZZA DINAMICA
        // ============================================================
        int msgY = hdrH + ScaleDpi(12);
        int msgL = padL + ScaleDpi(4);
        int msgR = g_ScaledWidth - padR;
        g_DisplayProblemCount = 0;
        
        if (activeProblems == 0) {
            // Descrizione statica (non cliccabile)
            const WCHAR* fullText = LOC(STR_NO_PROBLEMS);
            const WCHAR* newline = wcschr(fullText, L'\n');
            if (newline) {
                const WCHAR* line2 = newline + 1;
                SelectObject(hdcMem, g_hFontNormal);
                SetTextColor(hdcMem, dark ? clrTitle : RGB(80, 80, 80));
                RECT rcLine2 = {msgL, msgY, msgR, g_ScaledHeight - g_ScaledFooterHeight - ScaleDpi(4)};
                DrawTextW(hdcMem, line2, -1, &rcLine2, DT_LEFT | DT_WORDBREAK);
            }
        } else {
            int displayCount = (activeProblems < MAX_DISPLAY_PROBLEMS) ? activeProblems : MAX_DISPLAY_PROBLEMS;
            int lineH = ScaleDpi(22);
            int maxWidth = msgR - msgL - ScaleDpi(22) - ScaleDpi(4);
            int rowHeights[MAX_DISPLAY_PROBLEMS] = {0};
            
            // Prima passata: calcola quante righe servono per ogni problema
            for (int i = 0; i < displayCount; i++) {
                const wchar_t* msgText = GetProblemText(problemTypesCopy[i]);
                if (!msgText || !msgText[0]) continue;
                
                SIZE textSize;
                SelectObject(hdcMem, g_hFontNormal);
                GetTextExtentExPointW(hdcMem, msgText, lstrlenW(msgText), maxWidth, NULL, NULL, &textSize);
                
                int neededRows = 1;
                if (textSize.cx > maxWidth && maxWidth > 0) {
                    neededRows = (textSize.cx + maxWidth - 1) / maxWidth;
                    if (neededRows > 3) neededRows = 3;
                }
                rowHeights[i] = neededRows * lineH + ScaleDpi(4);
            }
            
            // Spaziatura 8% tra un problema e l'altro (in aggiunta al padding interno di ScaleDpi(4))
            int gapBetweenProblems = lineH / 12;  // ~8% di lineH (22 * 0.08 = 1.76)
            
            // Disegna i problemi con wrapping
            int currentY = msgY;
            for (int i = 0; i < displayCount; i++) {
                const wchar_t* msgText = GetProblemText(problemTypesCopy[i]);
                if (!msgText || !msgText[0]) continue;
                int rowHeight = rowHeights[i];
                int rowTop = currentY;
                int rowBottom = rowTop + rowHeight;
                
                RECT rcRowFull = {0, rowTop, g_ScaledWidth, rowBottom};
                RECT rcLink = {msgL + ScaleDpi(22), rowTop, msgR, rowBottom};
                
                g_ProblemLinkRects[i] = rcRowFull;
                g_ProblemTypesDisplay[i] = problemTypesCopy[i];
                g_DisplayProblemCount = i + 1;
                BOOL isHovering = (g_HoveredProblemIndex == i);
                if (isHovering) {
                    COLORREF hoverBg     = dark ? RGB(40, 40, 50)    : RGB(228, 241, 252);
                    COLORREF hoverBorder = dark ? RGB(60, 80, 120)   : RGB(174, 212, 243);
                    
                    RECT rcHover = rcRowFull;
                    rcHover.left += ScaleDpi(2);
                    rcHover.right -= ScaleDpi(2);
                    
                    HBRUSH hBrHov = CreateSolidBrush(hoverBg);
                    HPEN   hPenHov = CreatePen(PS_SOLID, 1, hoverBorder);
                    HPEN   hOldPenH  = (HPEN)SelectObject(hdcMem, hPenHov);
                    HBRUSH hOldBrH   = (HBRUSH)SelectObject(hdcMem, hBrHov);
                    RoundRect(hdcMem, rcHover.left, rcHover.top, rcHover.right, rcHover.bottom, 3, 3);
                    SelectObject(hdcMem, hOldPenH); 
                    SelectObject(hdcMem, hOldBrH);
                    DeleteObject(hBrHov); 
                    DeleteObject(hPenHov);
                    SetCursor(LoadCursor(NULL, IDC_HAND));
                }
                if (g_hShieldIcon) {
    // Calcola l'altezza effettiva del testo per allineare lo scudo
    // Usa l'altezza della riga di testo singola (lineH) come riferimento
    int iconSize = ScaleDpi(16);
    int textHeight = lineH;  // Altezza di una riga di testo
    int shieldY = rowTop + (rowHeight - iconSize) / 2;
    
    // Se la riga è più alta di una singola riga di testo, centra lo scudo
    // sulla PRIMA riga di testo, non su tutta l'altezza della riga
    if (rowHeight > textHeight + ScaleDpi(4)) {
        // Centra sulla prima riga di testo (le righe successive sono wrapping)
        shieldY = rowTop + (textHeight - iconSize) / 2;
    }
    
    DrawIconEx(hdcMem, msgL, shieldY,
              g_hShieldIcon, iconSize, iconSize, 0, NULL, DI_NORMAL);
}
                SelectObject(hdcMem, g_hFontNormal);
                SetTextColor(hdcMem, clrLink);
                DrawTextW(hdcMem, msgText, -1, &rcLink,
                         DT_LEFT | DT_TOP | DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX | DT_END_ELLIPSIS);
                
                currentY += rowHeight + gapBetweenProblems;
            }
            
            if (activeProblems > MAX_DISPLAY_PROBLEMS) {
                SelectGuard sg(hdcMem, g_hFontSmall); 
                SetTextColor(hdcMem, clrLink);
                RECT rcMore = {msgL + ScaleDpi(22), currentY, 
                               msgR, currentY + ScaleDpi(16)};
                DrawTextW(hdcMem, LOC(STR_AND_MORE), -1, &rcMore, DT_LEFT | DT_SINGLELINE);
            }
        }

        // ============================================================
        // FOOTER LINK
        // ============================================================
        { SelectGuard sg(hdcMem, g_IsHoveringLink ? g_hFontLink : g_hFontNormal);
          SetTextColor(hdcMem, g_IsHoveringLink ? clrLinkHover : clrLink);
          
          RECT rcClient; GetClientRect(hwnd, &rcClient);
          int currentFtrH = g_ScaledFooterHeight;
          RECT rcFtrDynamic = { 0, rcClient.bottom - currentFtrH, rcClient.right, rcClient.bottom };

          if (g_Settings.useRoundedCorners) {
              int offset15 = (currentFtrH * 15) / 100;
              rcFtrDynamic.top += offset15;
              rcFtrDynamic.bottom += offset15;
          }

          // Raise "Open Action Center" text by 1.5% of footer height.
          int raise15 = (currentFtrH * 15) / 1000; // 1.5%
          if (raise15 < 1) raise15 = 1;
          rcFtrDynamic.top -= raise15;
          rcFtrDynamic.bottom -= raise15;

          g_rcFooterLink = rcFtrDynamic;
          DrawTextW(hdcMem, LOC(STR_LINK_OPEN_AC), -1, &rcFtrDynamic, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); }
        
        // ============================================================
        // BORDI
        // ============================================================
        { GdiObj hBrNull(GetStockObject(NULL_BRUSH), false); SelectGuard sgBr(hdcMem, hBrNull);
          { GdiObj hPenOuter(CreatePen(PS_SOLID,borderW,clrOuterBorder)); SelectGuard sgPen(hdcMem,hPenOuter); Rectangle(hdcMem,0,0,g_ScaledWidth,g_ScaledHeight); }
          { GdiObj hPenInner(CreatePen(PS_SOLID,borderW,clrInnerBorder)); SelectGuard sgPen(hdcMem,hPenInner); Rectangle(hdcMem,borderW,borderW,g_ScaledWidth-borderW,g_ScaledHeight-borderW); } }
        
        BitBlt(hdc,0,0,g_ScaledWidth,g_ScaledHeight,hdcMem,0,0,SRCCOPY);
        SelectObject(hdcMem, hOldBm); DeleteObject(hbmMem); DeleteDC(hdcMem);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_DESTROY:
        RemoveClickOutsideHook();
        g_FlyoutClosing = FALSE; 
        g_IsHoveringLink = FALSE;
        if (g_Ctx.hWndFlyout == hwnd)
            g_Ctx.hWndFlyout = NULL;
        KillTimer(hwnd, AUTOHIDE_TIMER_ID);
        InterlockedDecrement(&g_Ctx.refCount);
        break;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}
void CreateFlyoutWindow() {
    if (g_Ctx.hWndFlyout && IsWindow(g_Ctx.hWndFlyout)) return;
    if (!g_Ctx.flyoutClassRegistered) {
        WNDCLASSEXW wc = { sizeof(WNDCLASSEXW) };
        wc.lpfnWndProc = FlyoutWndProc; wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = FLYOUT_CLASS_NAME; wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        if (RegisterClassExW(&wc)) g_Ctx.flyoutClassRegistered = TRUE;
    }
    
    // Calcola altezza dinamica in base ai problemi attivi
    int activeProblems;
    { SRWGuard guard(g_Ctx.srwLock, false); activeProblems = g_ActiveProblems; }
    
    // Aggiorna le metriche con l'altezza calcolata
    RecalcDpiMetrics(g_dpi, activeProblems);
    
    int flyoutHeight = g_ScaledHeight;
    int flyoutWidth = g_ScaledWidth;
    
    DWORD dwExStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
    DWORD dwStyle = g_Settings.useRoundedCorners ? (WS_POPUP | WS_THICKFRAME) : WS_POPUP;
    
    if (g_Settings.useRoundedCorners) {
        RECT rcWin = { 0, 0, flyoutWidth, flyoutHeight };
        AdjustWindowRectEx(&rcWin, dwStyle, FALSE, dwExStyle);
        int w = rcWin.right - rcWin.left;
        int h = rcWin.bottom - rcWin.top;
        g_Ctx.hWndFlyout = CreateWindowExW(dwExStyle, FLYOUT_CLASS_NAME, L"Win7 AC", dwStyle, 
            0, 0, w, h, 
            NULL, NULL, GetModuleHandle(NULL), NULL);
    } else {
        g_Ctx.hWndFlyout = CreateWindowExW(dwExStyle, FLYOUT_CLASS_NAME, L"Win7 AC", dwStyle, 
            0, 0, flyoutWidth, flyoutHeight, 
            NULL, NULL, GetModuleHandle(NULL), NULL);
    }
    if (!g_Ctx.hWndFlyout) { Wh_Log(LOG_PREFIX L"Flyout creation failed: %lu", GetLastError()); return; }
    
    // Posiziona SUBITO il flyout accanto all'icona tray, prima di mostrarlo.
    // Questo evita che appaia in (0,0) per un frame e poi salti.
    PositionWindowNearTray(g_Ctx.hWndFlyout);
}


// ============================================================================
// Notify Popup Window
// ============================================================================
LRESULT CALLBACK NotifyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_NCHITTEST:
    {
        LRESULT lr = DefWindowProcW(hwnd, uMsg, wParam, lParam);
        switch (lr)
        {
            case HTTOP: case HTTOPRIGHT: case HTRIGHT: case HTBOTTOMRIGHT:
            case HTBOTTOM: case HTBOTTOMLEFT: case HTLEFT: case HTTOPLEFT:
                return HTBORDER;
            default: return lr;
        }
    }

    case WM_CREATE:
        RecalcDpiMetrics(GetDpiForWindow(hwnd));
        if (g_Ctx.darkMode) { BOOL useDark = TRUE; DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDark, sizeof(useDark)); }
        break;
    case WM_DPICHANGED: {
        RecalcDpiMetrics(HIWORD(wParam)); InitGlobalFonts();
        RECT* prcNew = (RECT*)lParam;
        SetWindowPos(hwnd, NULL, prcNew->left, prcNew->top, prcNew->right-prcNew->left, prcNew->bottom-prcNew->top, SWP_NOZORDER|SWP_NOACTIVATE);
        InvalidateRect(hwnd, NULL, TRUE); break;
    }
    case WM_TIMER:
        if (wParam == NOTIFY_TIMER_ID) { KillTimer(hwnd, NOTIFY_TIMER_ID); ShowWindow(hwnd, SW_HIDE); g_NotifyShowing = FALSE; }
        break;
    case WM_ERASEBKGND: return 1;
    case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    case WM_SETCURSOR: SetCursor(LoadCursor(NULL, IDC_HAND)); return TRUE;
    case WM_LBUTTONDOWN:
        ShellExecuteW(NULL, L"open", L"control.exe", L"/name Microsoft.ActionCenter", NULL, SW_SHOWNORMAL);
        ShowWindow(hwnd, SW_HIDE); KillTimer(hwnd, NOTIFY_TIMER_ID); g_NotifyShowing = FALSE;
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
        if (!hdc) { EndPaint(hwnd, &ps); break; }
        HDC hdcMem = CreateCompatibleDC(hdc);
        if (!hdcMem) { EndPaint(hwnd, &ps); break; }
        HBITMAP hbmMem = CreateCompatibleBitmap(hdc, g_ScaledNotifyWidth, g_ScaledNotifyHeight);
        if (!hbmMem) { DeleteDC(hdcMem); EndPaint(hwnd, &ps); break; }
        HBITMAP hOldBm = (HBITMAP)SelectObject(hdcMem, hbmMem);
        if (!hOldBm) { DeleteObject(hbmMem); DeleteDC(hdcMem); EndPaint(hwnd, &ps); break; }
        int iconSize = g_ScaledIconSize, borderW = g_BorderPenWidth;
        BOOL dark = g_Ctx.darkMode;
        COLORREF clrNotifyBg = dark ? COLOR_DARK_NOTIFY_BG : COLOR_NOTIFY_BG;
        COLORREF clrNotifyBorder = dark ? COLOR_DARK_NOTIFY_BORDER : COLOR_NOTIFY_BORDER;
        COLORREF clrNotifyTitleBg = dark ? COLOR_DARK_NOTIFY_TITLE_BG : COLOR_NOTIFY_TITLE_BG;
        COLORREF clrTitle = dark ? COLOR_DARK_TITLE : COLOR_TITLE;
        COLORREF clrText = dark ? COLOR_DARK_TEXT : COLOR_TEXT_DARK;
        COLORREF clrLink = dark ? COLOR_DARK_LINK : COLOR_LINK;
        RECT rc = {0,0,g_ScaledNotifyWidth,g_ScaledNotifyHeight};
        GdiObj hBrBg(CreateSolidBrush(clrNotifyBg)); FillRect(hdcMem, &rc, (HBRUSH)hBrBg.get());
        { GdiObj hPen(CreatePen(PS_SOLID,borderW,clrNotifyBorder)); SelectGuard sgPen(hdcMem,hPen);
          GdiObj hBrNull(GetStockObject(NULL_BRUSH),false); SelectGuard sgBr(hdcMem,hBrNull);
          Rectangle(hdcMem,0,0,g_ScaledNotifyWidth,g_ScaledNotifyHeight); }
        { GdiObj hBrTitle(CreateSolidBrush(clrNotifyTitleBg)); SelectGuard sg(hdcMem,hBrTitle);
          Rectangle(hdcMem,borderW,borderW,g_ScaledNotifyWidth-borderW,ScaleDpi(22)); }
        SetBkMode(hdcMem, TRANSPARENT);
        { SelectGuard sg(hdcMem, g_hFontBold); SetTextColor(hdcMem, clrTitle);
          RECT rcT = {ScaleDpi(8),ScaleDpi(2),g_ScaledNotifyWidth-ScaleDpi(8),ScaleDpi(20)}; DrawTextW(hdcMem, LOC(STR_NOTIFY_TITLE), -1, &rcT, DT_LEFT|DT_SINGLELINE); }
        int secState; { SRWGuard guard(g_Ctx.srwLock, false); secState = g_SecurityState; }
        HICON hIcon = (secState >= STATE_ALERT) ? g_hFlyoutIconAlert :
                     ((secState >= STATE_WARNING) ? g_hFlyoutIconWarning : g_hFlyoutIconGood);
        DrawIconEx(hdcMem, ScaleDpi(10), ScaleDpi(26), hIcon, iconSize, iconSize, 0, NULL, DI_NORMAL);
        int txL = ScaleDpi(10) + iconSize + ScaleDpi(6);
        { SelectGuard sg(hdcMem, g_hFontSmall); SetTextColor(hdcMem, clrText);
          RECT rcM = {txL,ScaleDpi(26),g_ScaledNotifyWidth-ScaleDpi(8),ScaleDpi(44)};
          DrawTextW(hdcMem, (secState > STATE_GOOD) ? LOC(STR_NOTIFY_SIMULATED) : LOC(STR_NOTIFY_MESSAGE), -1, &rcM, DT_LEFT|DT_WORDBREAK); }
        { SelectGuard sg(hdcMem, g_hFontLink); SetTextColor(hdcMem, clrLink);
          RECT rcL = {txL,ScaleDpi(44),g_ScaledNotifyWidth-ScaleDpi(8),ScaleDpi(58)};
          DrawTextW(hdcMem, LOC(STR_LINK_OPEN_AC), -1, &rcL, DT_LEFT|DT_SINGLELINE); }
        BitBlt(hdc,0,0,g_ScaledNotifyWidth,g_ScaledNotifyHeight,hdcMem,0,0,SRCCOPY);
        SelectObject(hdcMem, hOldBm); DeleteObject(hbmMem); DeleteDC(hdcMem);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE) { KillTimer(hwnd, NOTIFY_TIMER_ID); SetTimer(hwnd, NOTIFY_TIMER_ID, 1500, NULL); }
        break;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

void CreateNotifyWindow() {
    if (g_Ctx.hWndNotify && IsWindow(g_Ctx.hWndNotify)) return;
    if (!g_Ctx.notifyClassRegistered) {
        WNDCLASSEXW wc = { sizeof(WNDCLASSEXW) };
        wc.lpfnWndProc = NotifyWndProc; wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = NOTIFY_WINDOW_CLASS_NAME; wc.hCursor = LoadCursor(NULL, IDC_HAND);
        if (RegisterClassExW(&wc)) g_Ctx.notifyClassRegistered = TRUE;
    }
    g_Ctx.hWndNotify = CreateWindowExW(WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE, NOTIFY_WINDOW_CLASS_NAME, L"", WS_POPUP, 0,0,g_ScaledNotifyWidth,g_ScaledNotifyHeight, NULL,NULL,GetModuleHandle(NULL),NULL);
    if (!g_Ctx.hWndNotify) Wh_Log(LOG_PREFIX L"Notify window creation failed: %lu", GetLastError());
}

// ============================================================================
// Tray Message Handler
// ============================================================================
// ============================================================================
// Tray Message Handler
// ============================================================================
LRESULT CALLBACK TrayMsgHandlerProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (g_Ctx.taskbarCreatedMessage && uMsg == g_Ctx.taskbarCreatedMessage) {
        Wh_Log(LOG_PREFIX L"TaskbarCreated received; scheduling tray icon re-creation");

        // After explorer restart the notification area may still be initializing.
        // Schedule a short delayed recovery instead of racing Shell_NotifyIcon.
        g_Ctx.trayIconAdded = FALSE;
        g_Ctx.trayRetryAttempt = 0;
        KillTimer(hwnd, TRAY_RETRY_TIMER_ID);
        KillTimer(hwnd, TRAY_HEALTH_TIMER_ID);
        SetTimer(hwnd, TRAY_RETRY_TIMER_ID, 300, NULL);
        SetTimer(hwnd, TRAY_HEALTH_TIMER_ID, 15000, NULL);
        return 0;
    }
    if (uMsg == WM_TIMER) {
        if (wParam == REFRESH_TIMER_ID) {
            if (!g_Ctx.isUninitializing) RefreshSecurityState();
            return 0;
        }
        if (wParam == TRAY_RETRY_TIMER_ID) {
            RunTrayIconRecoveryAttempt();
            return 0;
        }
        if (wParam == TRAY_HEALTH_TIMER_ID) {
            if (!g_Ctx.isUninitializing && !IsTrayIconReachable())
                ScheduleTrayIconRecovery();
            return 0;
        }
    }
    if (uMsg == WM_TRAY_SHUTDOWN) {
        // La finestra e l'icona vengono distrutte ordinatamente nel cleanup
        // del thread, dopo l'uscita dal message loop.
        PostQuitMessage(0);
        return 0;
    }
    if (uMsg == WM_TRAY_ICON_MSG) {
        if (LOWORD(lParam) == WM_LBUTTONUP) { 
            PostMessageW(hwnd, WM_TRIGGER_FLYOUT, 0, 0); 
            return 0; 
        }
        if (LOWORD(lParam) == WM_RBUTTONUP) {
            static DWORD lastMenuTime = 0;
            if (GetTickCount() - lastMenuTime < 500) return 0;
            lastMenuTime = GetTickCount();

            if (g_Ctx.hWndFlyout && IsWindow(g_Ctx.hWndFlyout) && IsWindowVisible(g_Ctx.hWndFlyout)) {
                PostMessageW(g_Ctx.hWndFlyout, WM_SAFE_CLOSE, 0, 0);
            }
            if (g_Ctx.isUninitializing) return 0;
            POINT pt; GetCursorPos(&pt);
            HMENU hMenu = CreatePopupMenu();
            if (hMenu) {
                InsertMenuW(hMenu, 0, MF_BYPOSITION|MF_STRING, ID_MENU_OPEN_AC, LOC(STR_LINK_OPEN_AC));
                InsertMenuW(hMenu, 1, MF_BYPOSITION|MF_STRING, ID_MENU_TROUBLESHOOT, LOC(STR_MENU_TROUBLESHOOT));
                SetForegroundWindow(hwnd);
                TrackPopupMenu(hMenu, TPM_LEFTALIGN|TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
                PostMessageW(hwnd, WM_NULL, 0, 0);
                DestroyMenu(hMenu);
            }
            return 0;
        }
        return 0;
    }
    if (uMsg == WM_TRIGGER_FLYOUT) { 
        ToggleFlyout(); 
        return 0; 
    }
    if (uMsg == WM_REFRESH_DATA) { 
        if (!g_Ctx.isUninitializing) RefreshSecurityState(); 
        return 0; 
    }
    if (uMsg == WM_SECURITY_CHANGED) { 
        if (!g_Ctx.isUninitializing) { 
            Wh_Log(LOG_PREFIX L"Security change notification"); 
            RefreshSecurityState(); 
        } 
        return 0; 
    }
    if (uMsg == WM_SIMULATE_NOTIFICATION) { 
        if (g_Settings.enableNotificationSimulation && !g_Ctx.isUninitializing) 
            SimulateNotification((int)wParam); 
        return 0; 
    }
    if (uMsg == WM_CLEAR_NOTIFICATIONS) { 
        if (!g_Ctx.isUninitializing) ClearNotifications(); 
        return 0; 
    }
    if (uMsg == WM_COMMAND) {
        if (LOWORD(wParam) == ID_MENU_OPEN_AC) 
            ShellExecuteW(NULL, L"open", L"control.exe", L"/name Microsoft.ActionCenter", NULL, SW_SHOWNORMAL);
        else if (LOWORD(wParam) == ID_MENU_TROUBLESHOOT) 
            ShellExecuteW(NULL, L"open", L"explorer.exe", L"shell:::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}", NULL, SW_SHOWNORMAL);
        return 0;
    }
    if (uMsg == WM_HOTKEY) {
        if (g_Ctx.isUninitializing) return 0;
        if (wParam == HOTKEY_ID_SIMULATE && g_Settings.enableNotificationSimulation) { 
            static int c = 0; 
            c = (c % 4) + 1; 
            PostMessageW(hwnd, WM_SIMULATE_NOTIFICATION, c, 0); 
            return 0; 
        }
        if (wParam == HOTKEY_ID_CLEAR) { 
            PostMessageW(hwnd, WM_CLEAR_NOTIFICATIONS, 0, 0); 
            return 0; 
        }
        if (wParam == HOTKEY_ID_LOGTREE) { 
            LogWindowTree(GetForegroundWindow()); 
            return 0; 
        }
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}
void EnsureTrayTooltip() {
    if (!g_Ctx.hWndMsgHandler || !IsWindow(g_Ctx.hWndMsgHandler)) return;
    if (!g_Ctx.trayIconAdded) {
        AddTrayIcon();
        return;
    }
    
    WCHAR tipBuf[256] = {0};
    BuildTrayTooltip(tipBuf, ARRAYSIZE(tipBuf));
    const wchar_t* tip = tipBuf;
    
    // Usa NIM_MODIFY con NIF_TIP e NIF_SHOWTIP per aggiornare il tooltip
    NOTIFYICONDATAW nid = {0};
    nid.cbSize = sizeof(NOTIFYICONDATAW);
    nid.hWnd = g_Ctx.hWndMsgHandler;
    nid.uID = TRAY_ICON_ID;
    nid.uFlags = NIF_TIP | NIF_GUID | NIF_SHOWTIP;
    nid.guidItem = TRAY_ICON_GUID;
    StringCchCopyW(nid.szTip, ARRAYSIZE(nid.szTip), tip);
    
    Shell_NotifyIconW(NIM_MODIFY, &nid);
}
// ============================================================================
// Tray Thread
// ============================================================================
DWORD WINAPI TrayThreadProc(LPVOID lpParam) {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (!g_Ctx.trayMsgClassRegistered) {
        WNDCLASSEXW mwc = { sizeof(WNDCLASSEXW) };
        mwc.lpfnWndProc = TrayMsgHandlerProc; mwc.hInstance = GetModuleHandle(NULL);
        mwc.lpszClassName = TRAY_MESSAGE_CLASS_NAME;
        if (RegisterClassExW(&mwc)) g_Ctx.trayMsgClassRegistered = TRUE;
    }

    // TaskbarCreated è un broadcast inviato alle finestre top-level.
    // Usiamo WS_POPUP per riceverlo e ripristinare l'icona dopo il riavvio di Explorer.
    g_Ctx.taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");
    g_Ctx.hWndMsgHandler = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, TRAY_MESSAGE_CLASS_NAME, L"",
        WS_POPUP, 0, 0, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);

    if (!g_Ctx.hWndMsgHandler) {
        if (g_Ctx.hTrayReadyEvent) SetEvent(g_Ctx.hTrayReadyEvent);
        CoUninitialize();
        return 1;
    }
    if (g_Ctx.hTrayReadyEvent) SetEvent(g_Ctx.hTrayReadyEvent);

    // At Windows startup / explorer restart the tray may not exist yet.
    // Wait a bit, then add; recovery timers cover the rest.
    if (!WaitForTaskbarReady(15000)) {
        Wh_Log(LOG_PREFIX L"Shell_TrayWnd not ready within 15s — scheduling recovery");
    }
    AddTrayIcon();
    if (!g_Ctx.trayIconAdded) {
        ScheduleTrayIconRecovery();
    }
    EnsureTrayTooltip();
    RegisterWscNotifications();
    StartRegistryMonitor();

    if (g_Settings.refreshInterval > 0)
        g_Ctx.refreshTimer = SetTimer(g_Ctx.hWndMsgHandler, REFRESH_TIMER_ID, g_Settings.refreshInterval, NULL);

    // Timer di autoriparazione per riavvio taskbar (in caso TaskbarCreated non venga ricevuto)
    SetTimer(g_Ctx.hWndMsgHandler, TRAY_HEALTH_TIMER_ID, 15000, NULL);

    if (g_Settings.enableHotkey) {
        RegisterHotKey(g_Ctx.hWndMsgHandler, HOTKEY_ID_SIMULATE, MOD_CONTROL, 'N');
        RegisterHotKey(g_Ctx.hWndMsgHandler, HOTKEY_ID_CLEAR, MOD_CONTROL | MOD_SHIFT, 'N');
    }
    RegisterHotKey(g_Ctx.hWndMsgHandler, HOTKEY_ID_LOGTREE, MOD_CONTROL | MOD_ALT, 'W');

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // Thread cleanup
    if (g_Settings.enableHotkey) {
        UnregisterHotKey(g_Ctx.hWndMsgHandler, HOTKEY_ID_SIMULATE);
        UnregisterHotKey(g_Ctx.hWndMsgHandler, HOTKEY_ID_CLEAR);
    }
    UnregisterHotKey(g_Ctx.hWndMsgHandler, HOTKEY_ID_LOGTREE);
    if (g_Ctx.refreshTimer) { KillTimer(g_Ctx.hWndMsgHandler, REFRESH_TIMER_ID); g_Ctx.refreshTimer = 0; }
    KillTimer(g_Ctx.hWndMsgHandler, TRAY_RETRY_TIMER_ID);
    KillTimer(g_Ctx.hWndMsgHandler, TRAY_HEALTH_TIMER_ID);
    UnregisterWscNotifications();
    Shell_NotifyIconW(NIM_DELETE, &g_nid);
    if (g_nid.hIcon) { DestroyIcon(g_nid.hIcon); g_nid.hIcon = NULL; }
    DestroyWindow(g_Ctx.hWndMsgHandler); g_Ctx.hWndMsgHandler = NULL;
    CoUninitialize();
    return 0;
}

// ============================================================================
void CleanupModResources() {
    Wh_Log(LOG_PREFIX L"CleanupModResources: starting");
    // 1. Signal uninitializing — prevents re-entry, tells threads to exit
    InterlockedExchange(&g_Ctx.isUninitializing, 1L);
    // 2. Remove mouse hook immediately (no thread affinity)
    RemoveClickOutsideHook();
    // 3. Close flyout on its owning thread — just post, don't wait
    if (g_Ctx.hWndFlyout && IsWindow(g_Ctx.hWndFlyout))
        PostMessageW(g_Ctx.hWndFlyout, WM_SAFE_CLOSE, 0, 0);
    // 4. Stop notification sources (thread-safe via InterlockedExchangePointer)
    UnregisterWscNotifications();
    // 5. Signal registry monitor thread to exit (event-driven, instant)
    StopRegistryMonitor();
    // 6. Post WM_QUIT to tray thread's message window
    if (g_Ctx.hWndMsgHandler && IsWindow(g_Ctx.hWndMsgHandler))
        PostMessageW(g_Ctx.hWndMsgHandler, WM_TRAY_SHUTDOWN, 0, 0);
    // 7. Wait for tray thread (short timeout — 500ms max)
    if (g_Ctx.hTrayThread) {
        DWORD wr = WaitForSingleObject(g_Ctx.hTrayThread, 5000);
        if (wr == WAIT_TIMEOUT) {
            Wh_Log(LOG_PREFIX L"Waiting for tray thread to exit");
            WaitForSingleObject(g_Ctx.hTrayThread, INFINITE);
        }
        CloseHandle(g_Ctx.hTrayThread);
        g_Ctx.hTrayThread = NULL;
    }
    // 8. Destroy any remaining windows (should be gone, but be safe).
    // Snapshot HWNDs then null the globals first so concurrent WndProcs see NULL.
    {
        HWND hFly = g_Ctx.hWndFlyout;   g_Ctx.hWndFlyout = NULL;
        HWND hMsg = g_Ctx.hWndMsgHandler; g_Ctx.hWndMsgHandler = NULL;
        HWND hNtf = g_Ctx.hWndNotify;   g_Ctx.hWndNotify = NULL;
        if (hFly && IsWindow(hFly)) DestroyWindow(hFly);
        if (hMsg && IsWindow(hMsg)) DestroyWindow(hMsg);
        if (hNtf && IsWindow(hNtf)) DestroyWindow(hNtf);
    }
    // 9. Free GDI+ resources (bitmap cache)
    ShutdownGdiPlus();
    // 10. Free icons and fonts
    FreeAllIcons();
    FreeGlobalFonts();
    // 11. Unregister window classes
    if (g_Ctx.flyoutClassRegistered) { UnregisterClassW(FLYOUT_CLASS_NAME, GetModuleHandle(NULL)); g_Ctx.flyoutClassRegistered = FALSE; }
    if (g_Ctx.trayMsgClassRegistered) { UnregisterClassW(TRAY_MESSAGE_CLASS_NAME, GetModuleHandle(NULL)); g_Ctx.trayMsgClassRegistered = FALSE; }
    if (g_Ctx.notifyClassRegistered) { UnregisterClassW(NOTIFY_WINDOW_CLASS_NAME, GetModuleHandle(NULL)); g_Ctx.notifyClassRegistered = FALSE; }
    // 12. Zero out context
    ZeroMemory(&g_Ctx, sizeof(g_Ctx));
    g_Initialized = FALSE;
    Wh_Log(LOG_PREFIX L"CleanupModResources: complete");
}
BOOL Wh_ModInit(void) {
    Wh_Log(LOG_PREFIX L"Win7 Action Center - Initializing...");
    
    // Allow init even if Shell_TrayWnd is not ready yet (boot / explorer restart).
    if (!IsMainExplorerProcess()) { 
        Wh_Log(LOG_PREFIX L"Not shell explorer.exe, skipping");
        return TRUE; 
    }
    
    InstallRegistryHooks();
    
    // Aggiungi hook per CreateWindowExW per rilevare la ricreazione della taskbar
    if (!CreateWindowExW_orig) {
        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        if (hUser32) {
            WindhawkUtils::Wh_SetFunctionHookT(
                (CreateWindowExW_t)GetProcAddress(hUser32, "CreateWindowExW"), 
                CreateWindowExW_hook, 
                &CreateWindowExW_orig);
            Wh_Log(LOG_PREFIX L"CreateWindowExW hook installed for Shell_TrayWnd detection");
        }
    }
    
    ZeroMemory(&g_Ctx, sizeof(g_Ctx));
    LoadSettings();
    DetermineLocale();
    InitializeSRWLock(&g_Ctx.srwLock);
    g_Ctx.darkMode = IsDarkModeEnabled();
    Wh_Log(LOG_PREFIX L"Dark mode: %s", g_Ctx.darkMode ? L"enabled" : L"disabled");
    HDC hScreenDC = GetDC(NULL);
    UINT dpi = hScreenDC ? (UINT)GetDeviceCaps(hScreenDC, LOGPIXELSX) : 96;
    if (hScreenDC) ReleaseDC(NULL, hScreenDC);
    
    // Inizializza con 0 problemi (stato iniziale)
    RecalcDpiMetrics(dpi, 0);
    InitGlobalFonts();

    // GDI+ deve essere inizializzato prima di decodificare le PNG Base64.
    if (!InitGdiPlusRendering()) {
        Wh_Log(LOG_PREFIX L"GDI+ init failed — will use DrawIconEx fallback");
    }
    InitFlyoutIcons();
    CreateNotifyWindow();
    g_Ctx.hTrayThread = CreateThread(NULL, 0, TrayThreadProc, NULL, 0, &g_Ctx.trayThreadId);
    if (!g_Ctx.hTrayThread) {
        Wh_Log(LOG_PREFIX L"Failed to create tray thread — cleaning up");
        CleanupModResources();
        return FALSE;
    }
    g_Initialized = TRUE;
    Wh_Log(LOG_PREFIX L"Initialization complete");
    return TRUE;
}

void Wh_ModSettingsChanged(void) {
    LoadSettings();
    DetermineLocale();
    g_Ctx.darkMode = IsDarkModeEnabled();
    if (g_Ctx.hWndMsgHandler && IsWindow(g_Ctx.hWndMsgHandler) && !g_Ctx.isUninitializing) {
        if (g_Settings.enableHotkey) {
            RegisterHotKey(g_Ctx.hWndMsgHandler, HOTKEY_ID_SIMULATE, MOD_CONTROL, 'N');
            RegisterHotKey(g_Ctx.hWndMsgHandler, HOTKEY_ID_CLEAR, MOD_CONTROL | MOD_SHIFT, 'N');
        } else {
            UnregisterHotKey(g_Ctx.hWndMsgHandler, HOTKEY_ID_SIMULATE);
            UnregisterHotKey(g_Ctx.hWndMsgHandler, HOTKEY_ID_CLEAR);
        }
        if (g_Settings.refreshInterval > 0) {
            if (g_Ctx.refreshTimer) KillTimer(g_Ctx.hWndMsgHandler, g_Ctx.refreshTimer);
            g_Ctx.refreshTimer = SetTimer(g_Ctx.hWndMsgHandler, REFRESH_TIMER_ID, g_Settings.refreshInterval, NULL);
        } else if (g_Ctx.refreshTimer) {
            KillTimer(g_Ctx.hWndMsgHandler, g_Ctx.refreshTimer);
            g_Ctx.refreshTimer = 0;
        }
    }
    if (g_Ctx.hWndFlyout && IsWindow(g_Ctx.hWndFlyout) && !g_Ctx.isUninitializing)
        InvalidateRect(g_Ctx.hWndFlyout, NULL, TRUE);
}

void Wh_ModUninit(void) {
    Wh_Log(LOG_PREFIX L"Wh_ModUninit called");
    CleanupModResources();
    Wh_Log(LOG_PREFIX L"Uninitialization complete");
}
