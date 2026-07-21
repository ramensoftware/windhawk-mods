// ==WindhawkMod==
// @id             win7-action-center-recreation
// @name           Windows 7/8.1 Action Center Recreation
// @description    This mod recreates the Windows 7/8.1 Action Center tray/flyout and restores the classic Security and Maintenance CPL links
// @version        1.8.0
// @author         babamohammed
// @github         https://github.com/babamohammed2022
// @include        explorer.exe
// @include        control.exe
// @architecture   x86-64
// @compilerOptions -lgdi32 -luser32 -lshell32 -lwscapi -ldwmapi -lole32 -loleaut32 -ladvapi32 -lshlwapi -lpropsys
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*

# Windows 7/8.1 Action Center Recreation
This mod recreates the classic Windows 7/8.1 Action Center tray icon and flyout for modern Windows versions.
## Screenshots
Windows 7 theme

![Image](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win7act.png)

Windows 8.1 theme

![Image](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win81act.png)

## Features
- **No File Replacement**: The mod's objective is to be stable and user-friendly and does not require the user to replace system files.
- **Real-time Security Status**: The tray icon shows your system's security state at a glance. Green means everything is fine, yellow means warnings, red means critical issues.
- **Interactive Flyout**: Click the tray icon to open a flyout that displays all security issues. Click any issue to open the relevant settings or troubleshooting page.
- **Rounded Corners**: Rounded corners are supported for a more similar look to the original Windows 7 flyout.
- **Classic Theme support**: Disable the "Rounded Corners" theme to make the flyout use a classic theme.
- **Light/Dark Theme**: Flyout and notification popup support light and dark themes. "Auto" follows the Windows light/dark mode setting and updates live when it changes; you can also force Light or Dark from the mod settings.
- **Balloon Notifications**: The mod displays balloon notifications when potential problems are detected, with detailed descriptions of issues found.
- **SmartScreen Check**: Monitors Windows Defender SmartScreen status and reports if it is disabled.
- **Privacy Mode**: The user can enable this mod to hide the eventual problems shown by the flyout.
- **Maintenance Checks**: Automatically checks Backup status, Windows Error Reporting status, Disk health, Battery level, pending Windows updates, RDP without NLA, and BitLocker protection.
  - The disk health check is **best-effort**: it queries SMART predicted-failure status per drive and, because the mod runs unelevated inside `explorer.exe`, some drives may not answer — in that case the check is simply skipped and never reports a false problem.
  - The battery check fires only on laptops running on battery power and warns when the charge drops to 20 % or below.
  - The pending-update check reads the standard CBS and Windows Update registry keys that Windows sets when a reboot is required to finish installing updates.
- **Startup Notification**: On every Windows session start, if problems are detected at the first check, a balloon notification is shown immediately regardless of cooldown, so you are never left unaware of existing issues after a reboot.
- **ESC to Close**: Press Escape to quickly close the flyout window.
- **Multiple Languages Support**: English, Italian, Spanish, French, Russian, Portuguese, German are currently supported.
- **Security and Maintenance CPL Links**: The mod restores the classic side-by-side **Troubleshooting** and **Recovery** entries on the Control Panel *Security and Maintenance* hub page (as on Windows 7/8.1). The labels follow the UI language (EN/IT/ES/FR/RU/PT/DE). Troubleshooting opens the system troubleshooter shell folder while Recovery opens the Recovery applet. 

## Hotkeys
These are the hotkeys that can be configured in the mod.
| Hotkey | Action |
|--------|--------|
| `Ctrl+N` | Simulate a notification |
| `Ctrl+Shift+N` | Clear notifications |


## How It Works

The mod monitors the system's security settings including Firewall, Antivirus, Windows Update, UAC, Windows Defender and other settings. When an issue is detected, the tray icon changes color and the flyout shows the problem with a clickable link to fix it.
The mod has been tested on Windows 10 1809, Windows 10 21H2 and Windows 11 23H2 and it is compatible with the native Windows 10 taskbar (native on Windows 10 and using ExplorerPatcher or similar methods on Windows 11).

## Known Limitations
- **Vertical taskbar (left/right edge)**: Flyout positioning on vertical taskbars 
  is best-effort as Windows 10's taskbar itself has inconsistent flyout behavior on vertical 
  taskbars, so perfect placement cannot be guaranteed in all configurations.
- **Hidden tray icon**: It is recommended to keep the Action Center icon visible 
  in the system tray rather than hidden in the notification overflow. When hidden, 
  positioning may be less accurate depending on the Windows version.
## Notes

- The mod runs inside Explorer and works on Windows 10 and 11.
- If the icon doesn't appear, try restarting Explorer or the mod.
- The Control Panel hub links activate when you open *Security and Maintenance* (`control /name Microsoft.ActionCenter`). No system files are modified on disk.
## Credits 
- Yvor - Testing on Windows 10 21H2 with the Windows 8.1 theme
- TheWolf - Testing on Windows 11 23H2
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- useRoundedCorners: true
  $name: Rounded corners
  $description: This setting enables rounded edges on the flyout (Windows 7 look). Turn this off for Classic theme or other styles that need square corners.
- refreshInterval: 5000
  $name: Status check interval (ms)
  $description: How often the tray icon re-checks security and maintenance (milliseconds). Use at least 1000. Set 0 to check only when Windows reports a change.
- enableHotkey: false
  $name: Enable hotkeys
  $description: Turn on keyboard shortcuts for testing (see the options below).
- enableNotificationSimulation: true
  $name: Test notifications (Ctrl+N)
  $description: When hotkeys are enabled, Ctrl+N shows a sample balloon and Ctrl+Shift+N clears it. Useful only for testing.
- privacyMode: false
  $name: Privacy mode
  $description: Always show the neutral tray icon and hide problems in the flyout. Handy on a shared screen.
- language: auto
  $name: Language
  $description: Language for the tray icon, flyout, and balloons. "Auto" follows Windows.
  $options:
    - auto: Auto (match Windows)
    - en: English
    - it: Italiano
    - es: Español
    - fr: Français
    - ru: Русский
    - pt: Português
    - de: Deutsch
- restoreCplHubLinks: true
  $name: Control Panel links
  $description: On the Security and Maintenance page, show Troubleshooting and Recovery side by side (classic layout). Turn off if you only want the tray flyout.
- useEmbeddedUifile: false
  $name: Control Panel layout fallback
  $description: Advanced. Only if those Control Panel links do not appear, try an alternate built-in page layout. Leave off in normal use.
- theme: auto
  $name: Theme
  $description: Flyout and notification theme. "Auto" follows the Windows light/dark mode setting.
  $options:
    - auto: Auto (follow Windows)
    - light: Light
    - dark: Dark
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
#include <shlwapi.h>
#include <winioctl.h>
#include <wbemidl.h>
#include <string>
#include <propsys.h>
#include <propkey.h>
#include <propvarutil.h>
#include <shobjidl.h>
#include <shlguid.h>

#define FLYOUT_OFFSET 8

/* Restituisce il DPI effettivo della finestra tramite GetDpiForWindow (Win10 1607+),
   con fallback a GetDeviceCaps per compatibilita' con versioni precedenti. */
static UINT GetWindowDpi(HWND hwnd) {
    typedef UINT (WINAPI *GetDpiForWindow_t)(HWND);
    static auto pfn = (GetDpiForWindow_t)GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "GetDpiForWindow");
    if (pfn && hwnd && IsWindow(hwnd)) {
        UINT dpi = pfn(hwnd);
        if (dpi >= 96) return dpi;
    }
    HDC hDC = GetDC(hwnd);
    UINT dpi = hDC ? (UINT)GetDeviceCaps(hDC, LOGPIXELSX) : 96;
    if (hDC) ReleaseDC(hwnd, hDC);
    return dpi ? dpi : 96;
}

/* Adjust a window's position to be pushed away from the taskbar (Aero Flyout Fix style).
   Usa GetDpiForWindow invece di GetDeviceCaps per gestire correttamente DPI non standard
   (es. 150% su Win11 25H2 con ExplorerPatcher).
   Risolto: ora usa il monitor della taskbar per consistentenza multimonitor. */
POINT AdjustWindowPosForTaskbar(HWND hWnd)
{
    UINT dpi = GetWindowDpi(hWnd);
    int offset = MulDiv(FLYOUT_OFFSET, (int)dpi, 96);

    RECT rc;
    GetWindowRect(hWnd, &rc);
    
    // Ottieni il monitor della taskbar per consistentenza multimonitor
    // Non usare MonitorFromWindow(hWnd) perche la finestra potrebbe essere a (0,0)
    HMONITOR hm = NULL;
    HWND hTrayWnd = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hTrayWnd) {
        hm = MonitorFromWindow(hTrayWnd, MONITOR_DEFAULTTONEAREST);
    }
    if (!hm) {
        hm = MonitorFromPoint({rc.left, rc.top}, MONITOR_DEFAULTTONEAREST);
    }

    MONITORINFO mi = { sizeof(MONITORINFO) };
    if (!GetMonitorInfoW(hm, &mi)) {
        // Fallback: ritorna posizione originale
        return { rc.left, rc.top };
    }

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
    Wh_Log(L"AdjustWindowPosForTaskbar: original={%d,%d} adjusted={%d,%d} monitor={%d,%d,%d,%d}",
           rc.left, rc.top, rc.left + dx, rc.top + dy,
           mi.rcWork.left, mi.rcWork.top, mi.rcWork.right, mi.rcWork.bottom);
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
#define PROBLEM_BALLOON_TIMER_ID   2003
#define PROBLEM_BALLOON_FALLBACK_MS 30000
#define PROBLEM_BALLOON_COOLDOWN_MS 30000
#define WM_TRAY_SHUTDOWN           (WM_USER + 602)
#define WM_SETTINGS_CHANGED        (WM_USER + 603)

// Compatibilita' con SDK che non espongono ancora questi flag.
#ifndef NIF_REALTIME
#define NIF_REALTIME               0x00000040
#endif
#ifndef NIIF_RESPECT_QUIET_TIME
#define NIIF_RESPECT_QUIET_TIME    0x00000080
#endif
#ifndef NIN_BALLOONSHOW
#define NIN_BALLOONSHOW            (WM_USER + 2)
#define NIN_BALLOONHIDE            (WM_USER + 3)
#define NIN_BALLOONTIMEOUT         (WM_USER + 4)
#define NIN_BALLOONUSERCLICK       (WM_USER + 5)
#endif
#define HOTKEY_ID_SIMULATE         9001
#define HOTKEY_ID_CLEAR            9002
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
#define MAX_DISPLAY_PROBLEMS       40
#define WM_CLOSE_FLYOUT_DELAY_MS   500
#define AUTOHIDE_INACTIVITY_MS     120000
#define THREAD_WAIT_TIMEOUT        500

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
typedef int (WINAPI *GdipCreateFromHDC_t)(HDC, void**);
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
static GdipCreateFromHDC_t pGdipCreateFromHDC = NULL;
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

// Cached GDI+ bitmaps for flyout icons
static void* g_pBmpFlyoutGood = NULL;
static void* g_pBmpFlyoutWarning = NULL;
static void* g_pBmpFlyoutAlert = NULL;
// GDI+ richiede che ogni stream PNG resti vivo quanto la relativa immagine.
static IStream* g_pStreamFlyoutGood = NULL;
static IStream* g_pStreamFlyoutWarning = NULL;
static IStream* g_pStreamFlyoutAlert = NULL;

static BOOL InitGdiPlusRendering() {
    if (g_hGdiPlus) return TRUE;
    g_hGdiPlus = LoadLibraryW(L"gdiplus.dll");
    if (!g_hGdiPlus) { Wh_Log(L"GDI+: failed to load gdiplus.dll"); return FALSE; }
    pGdipCreateBitmapFromHICON = (GdipCreateBitmapFromHICON_t)GetProcAddress(g_hGdiPlus, "GdipCreateBitmapFromHICON");
    pGdipCreateFromHDC = (GdipCreateFromHDC_t)GetProcAddress(g_hGdiPlus, "GdipCreateFromHDC");
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
    if (!pGdipCreateBitmapFromHICON || !pGdipCreateFromHDC || !pGdipSetInterpolationMode ||
        !pGdipDrawImageRectI || !pGdipDeleteGraphics || !pGdipCreateBitmapFromScan0 ||
        !pGdipGetImageGraphicsContext ||
        !pGdipSetPixelOffsetMode || !pGdipGraphicsClear || !pGdipCreateHBITMAPFromBitmap ||
        !pGdipDisposeImage || !pStartup || !pGdiplusShutdown || !pGdipSetSmoothingMode || !pGdipSetCompositingQuality) {
        Wh_Log(L"GDI+: missing function pointers"); FreeLibrary(g_hGdiPlus); g_hGdiPlus = NULL; return FALSE; }
    struct { DWORD Version; void* Callback; BOOL Suppress; } si = {1, NULL, FALSE};
    if (pStartup(&g_gdiplusToken, &si, NULL) != 0) {
        Wh_Log(L"GDI+: GdiplusStartup failed"); FreeLibrary(g_hGdiPlus); g_hGdiPlus = NULL; return FALSE; }
    return TRUE;
}

// Disegna direttamente i PNG ARGB nel DC con ricampionamento di alta qualita'.
// Evita il doppio passaggio PNG -> HICON -> DrawIconEx per le bandiere
// ID 0, 1 e 2, soprattutto con fattori DPI non interi.
static BOOL DrawGdipBitmapHighQuality(HDC hdc, void* bitmap,
                                      int x, int y, int width, int height) {
    if (!hdc || !bitmap || width <= 0 || height <= 0 ||
        !pGdipCreateFromHDC || !pGdipDrawImageRectI || !pGdipDeleteGraphics)
        return FALSE;

    void* graphics = NULL;
    if (pGdipCreateFromHDC(hdc, &graphics) != 0 || !graphics)
        return FALSE;

    // InterpolationModeHighQualityBicubic = 7
    // SmoothingModeHighQuality = 2
    // CompositingQualityHighQuality = 2
    // PixelOffsetModeHighQuality = 2
    if (pGdipSetInterpolationMode)   pGdipSetInterpolationMode(graphics, 7);
    if (pGdipSetSmoothingMode)       pGdipSetSmoothingMode(graphics, 2);
    if (pGdipSetCompositingQuality)  pGdipSetCompositingQuality(graphics, 2);
    if (pGdipSetPixelOffsetMode)     pGdipSetPixelOffsetMode(graphics, 2);

    int status = pGdipDrawImageRectI(graphics, bitmap, x, y, width, height);
    pGdipDeleteGraphics(graphics);
    return status == 0;
}

static BYTE B64Val(WCHAR c) {
    const WCHAR* p = wcschr(kBase64Tbl, c);
    return p ? (BYTE)(p - kBase64Tbl) : 0xFF;
}


// Helper to load GDI+ Bitmap directly from Base64 PNG string, bypassing HICON
static void ShutdownGdiPlus() {
    // Dispose cached GDI+ bitmaps BEFORE shutting down the runtime
    if (g_pBmpFlyoutGood) { if (pGdipDisposeImage) pGdipDisposeImage(g_pBmpFlyoutGood); g_pBmpFlyoutGood = NULL; }
    if (g_pStreamFlyoutGood) { g_pStreamFlyoutGood->Release(); g_pStreamFlyoutGood = NULL; }
    if (g_pBmpFlyoutWarning) { if (pGdipDisposeImage) pGdipDisposeImage(g_pBmpFlyoutWarning); g_pBmpFlyoutWarning = NULL; }
    if (g_pStreamFlyoutWarning) { g_pStreamFlyoutWarning->Release(); g_pStreamFlyoutWarning = NULL; }
    if (g_pBmpFlyoutAlert) { if (pGdipDisposeImage) pGdipDisposeImage(g_pBmpFlyoutAlert); g_pBmpFlyoutAlert = NULL; }
    if (g_pStreamFlyoutAlert) { g_pStreamFlyoutAlert->Release(); g_pStreamFlyoutAlert = NULL; }

    // Shutdown GDI+ runtime
    if (g_hGdiPlus) {
        if (pGdiplusShutdown && g_gdiplusToken) pGdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
        FreeLibrary(g_hGdiPlus);
        g_hGdiPlus = NULL;
    }

    // Reset ALL function pointers to prevent dangling pointers after FreeLibrary.
    // This is critical: after FreeLibrary the code segment is unmapped, so any
    // call through these pointers would be undefined behaviour / access violation.
    pGdipCreateBitmapFromHICON = NULL;
    pGdipCreateFromHDC = NULL;
    pGdipSetInterpolationMode = NULL;
    pGdipSetSmoothingMode = NULL;
    pGdipSetCompositingQuality = NULL;
    pGdipDrawImageRectI = NULL;
    pGdipDeleteGraphics = NULL;
    pGdipCreateBitmapFromScan0 = NULL;
    pGdipGetImageGraphicsContext = NULL;
    pGdipSetPixelOffsetMode = NULL;
    pGdipGraphicsClear = NULL;
    pGdipCreateHBITMAPFromBitmap = NULL;
    pGdipDisposeImage = NULL;
    pGdipCreateBitmapFromStream = NULL;
    pGdipCreateHICON = NULL;
    pGdiplusShutdown = NULL;

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
static void ShowProblemBalloon(void);
static void RemoveProblemBalloon(void);
static void UpdateTrayIcon(void);
static void AddTrayIcon(void);
static void ScheduleTrayIconRecovery(void);
static void PositionWindowNearTray(HWND hwnd);
static void InstallClickOutsideHook(void);
static void RemoveClickOutsideHook(void);
static void UpdateCachedTrayIconRect(void);
static void InstallKeyboardHook(void);
static void RemoveKeyboardHook(void);
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);
static void CreateFlyoutWindow(void);
static void CloseFlyout(HWND hwnd);
LRESULT CALLBACK ClickOutsideMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);
static void CleanupModResources(void);
static void EnsureTrayTooltip(void);
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
    int  theme; // 0=auto, 1=light, 2=dark
} g_Settings = { TRUE, TRUE, FALSE, 5000, FALSE, 0, 0 };

void LoadSettings() {
    g_Settings.useRoundedCorners = Wh_GetIntSetting(L"useRoundedCorners");
    g_Settings.enableNotificationSimulation = Wh_GetIntSetting(L"enableNotificationSimulation");
    g_Settings.privacyMode = Wh_GetIntSetting(L"privacyMode");
    g_Settings.refreshInterval = Wh_GetIntSetting(L"refreshInterval");
    g_Settings.enableHotkey = Wh_GetIntSetting(L"enableHotkey");
    // Wh_GetStringSetting returns L"" (never NULL) when the setting doesn't exist
    LPCWSTR lang = Wh_GetStringSetting(L"language");
    if (lang && *lang) {
        if (_wcsicmp(lang, L"en") == 0) g_Settings.language = 1;
        else if (_wcsicmp(lang, L"it") == 0) g_Settings.language = 2;
        else if (_wcsicmp(lang, L"es") == 0) g_Settings.language = 3;
        else if (_wcsicmp(lang, L"fr") == 0) g_Settings.language = 4;
        else if (_wcsicmp(lang, L"ru") == 0) g_Settings.language = 5;
        else if (_wcsicmp(lang, L"pt") == 0) g_Settings.language = 6;
        else if (_wcsicmp(lang, L"de") == 0) g_Settings.language = 7;
        else g_Settings.language = 0;
    } else {
        g_Settings.language = 0; // auto-detect
    }
    Wh_FreeStringSetting(lang);

    // Theme setting: 0=auto, 1=light, 2=dark
    LPCWSTR themeStr = Wh_GetStringSetting(L"theme");
    if (themeStr && *themeStr) {
        if (_wcsicmp(themeStr, L"light") == 0) g_Settings.theme = 1;
        else if (_wcsicmp(themeStr, L"dark") == 0) g_Settings.theme = 2;
        else g_Settings.theme = 0; // auto
    } else {
        g_Settings.theme = 0; // auto
    }
    Wh_FreeStringSetting(themeStr);

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
    
    // Footer +5.5% vs original base height (more breathing room for the link text).
    g_ScaledFooterHeight = MulDiv(ScaleDpi(BASE_FOOTER_HEIGHT), 1055, 1000);
    g_ScaledNotifyWidth = ScaleDpi(BASE_NOTIFY_WIDTH);
    g_ScaledNotifyHeight = ScaleDpi(BASE_NOTIFY_HEIGHT);
    g_ScaledIconSize = ScaleDpi(BASE_ICON_SIZE);
    g_ScaledHeaderHeight = ScaleDpi(BASE_HEADER_HEIGHT);
    g_BorderPenWidth = MaxInt(1, ScaleDpi(1));
}
int CalculateFlyoutHeight(int activeProblems) {

    // Keep footer height in sync with RecalcDpiMetrics (+3.5%).
    int footerH = MulDiv(ScaleDpi(BASE_FOOTER_HEIGHT), 1055, 1000);

    // Riduzione globale dell'altezza della finestra del flyout: -13%.
    // Applicata al valore finale e anche alle altezze minime, cosi' ogni
    // percorso (0 problemi / N problemi) resta proporzionato.
    const int kFlyoutHeightPctNum = 87;  // 87% dell'altezza originale
    const int kFlyoutHeightPctDen = 100;

    // Altezza minima di base (con o senza rounded corners)
    int minBaseHeight = ScaleDpi(160);
    if (g_Settings.useRoundedCorners) {
        minBaseHeight = ScaleDpi(205);
    }
    minBaseHeight = MulDiv(minBaseHeight, kFlyoutHeightPctNum, kFlyoutHeightPctDen);

    if (activeProblems == 0) {

        // Nessun problema: altezza fissa con descrizione
        int height = ScaleDpi(BASE_HEADER_HEIGHT + BASE_DESCRIPTION_HEIGHT + 10) + footerH;
        height = MulDiv(height, kFlyoutHeightPctNum, kFlyoutHeightPctDen);
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

        // Riduzione del 13% applicata al risultato finale
        height = MulDiv(height, kFlyoutHeightPctNum, kFlyoutHeightPctDen);

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

// Restituisce il dark mode EFFETTIVO combinando l'impostazione "theme" con
// l'auto-rilevamento di Windows: 0=auto (segue Windows), 1=light forzato,
// 2=dark forzato. Prima di questa funzione l'opzione "theme" veniva letta
// dalle impostazioni ma mai applicata ai colori del flyout/notifiche.
static BOOL GetEffectiveDarkMode(void) {
    if (g_Settings.theme == 1) return FALSE;   // Tema chiaro forzato
    if (g_Settings.theme == 2) return TRUE;    // Tema scuro forzato
    return IsDarkModeEnabled();                // Auto: segue Windows
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
    STR_AND_MORE, STR_TIP_NO_ISSUES, STR_TIP_ISSUES, STR_NOTIFY_PROBLEM,
    STR_MSG_SMARTSCREEN, STR_MSG_BACKUP, STR_MSG_WER, STR_MSG_DISK_HEALTH,
    STR_NOTIFY_ACTION, STR_TIP_RECOMMENDATION,
    STR_MSG_BATTERY, STR_MSG_UPDATE_PENDING,
    STR_MSG_RDP_NLA, STR_MSG_BITLOCKER,
    STR_NOTIFY_ACTION_NEW, STR_NOTIFY_ACTION_NEW_CRITICAL, STR_NOTIFY_ACTION_CRITICAL,
    STR_COUNT
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
        L"No current issues detected\nYou can use Action Center to review recent messages about your computer's status and find solutions to problems.", 
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
        L"No current issues detected", 
        L"%d issues detected.", 
        L"A problem has been detected. Please review your security status.",
        L"SmartScreen is turned off. Apps from the web won't be checked.",
        L"System backup is not configured or not running.",
        L"Windows Error Reporting service is disabled.",
        L"Disk health check recommended.",
        L"Open Action Center to review and fix issues.",
        L"Review your system status",
        L"Battery is low. Connect your device to a power source.",
        L"Windows updates are waiting. Restart your computer to apply them.",
        L"Remote Desktop is enabled without Network Level Authentication.",
        L"The system drive isn't protected by BitLocker.",
        L"Click to see what's new.",
        L"New critical issue detected. Click to review now.",
        L"Critical issue still present. Click to fix it."
    }},
    // Italiano (0x0410) 
    { 0x0410, { 
        L"Centro operativo", 
        L"Apri Centro operativo", 
        L"Centro operativo", 
        L"Risoluzione dei problemi", 
        L"Windows Update", 
        L"2 messaggi importanti", 
        L"1 messaggio importante", 
        L"Nessun problema rilevato\n\u00C8 possibile utilizzare il Centro operativo per visualizzare i messaggi recenti sullo stato del computer e trovare soluzioni ai problemi.", 
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
        L"Nessun problema rilevato", 
        L"%d problemi rilevati.", 
        L"\u00C8 stato rilevato un problema. Si prega di verificare lo stato di sicurezza del sistema.",
        L"SmartScreen \u00E8 disattivato. Le app dal web non verranno controllate.",
        L"Il backup del sistema non \u00E8 configurato o non \u00E8 in esecuzione.",
        L"Il servizio Segnalazione errori Windows \u00E8 disabilitato.",
        L"Controllo integrit\u00E0 disco consigliato.",
        L"Aprire il Centro operativo per verificare e risolvere i problemi.",
        L"Verifica lo stato del sistema",
        L"Batteria scarica. Collegare il dispositivo all\u2019alimentazione.",
        L"Aggiornamenti Windows in attesa. Riavviare il computer per applicarli.",
        L"Desktop remoto attivo senza Network Level Authentication.",
        L"L'unit\u00E0 di sistema non \u00E8 protetta da BitLocker.",
        L"Fare clic per vedere le novit\u00E0.",
        L"Nuovo problema critico rilevato. Fare clic per verificare subito.",
        L"Problema critico ancora presente. Fare clic per risolverlo."
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
        L"%d problemas detectados.", 
        L"Se ha detectado un problema. Revise el estado de seguridad del sistema.",
        L"SmartScreen est\u00E1 desactivado. No se comprobar\u00E1n las aplicaciones de la web.",
        L"La copia de seguridad del sistema no est\u00E1 configurada o no se est\u00E1 ejecutando.",
        L"El servicio de Informe de errores de Windows est\u00E1 deshabilitado.",
        L"Se recomienda comprobar el estado del disco.",
        L"Abra el Centro de actividades para revisar y solucionar los problemas.",
        L"Revise el estado del sistema",
        L"Bater\u00eda baja. Conecte el dispositivo a una fuente de alimentaci\u00f3n.",
        L"Actualizaciones de Windows pendientes. Reinicie el equipo para aplicarlas.",
        L"Escritorio remoto habilitado sin autenticaci\u00F3n de nivel de red.",
        L"La unidad del sistema no est\u00E1 protegida con BitLocker.",
        L"Haga clic para ver las novedades.",
        L"Nuevo problema cr\u00EDtico detectado. Haga clic para revisarlo ahora.",
        L"Problema cr\u00EDtico persistente. Haga clic para solucionarlo."
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
        L"%d probl\u00E8mes d\u00E9tect\u00E9s.", 
        L"Un probl\u00E8me a \u00E9t\u00E9 d\u00E9tect\u00E9. Veuillez v\u00E9rifier l'\u00E9tat de s\u00E9curit\u00E9.",
        L"SmartScreen est d\u00E9sactiv\u00E9. Les applications web ne seront pas v\u00E9rifi\u00E9es.",
        L"La sauvegarde du syst\u00E8me n'est pas configur\u00E9e ou n'est pas en cours d'ex\u00E9cution.",
        L"Le service Rapport d'erreurs Windows est d\u00E9sactiv\u00E9.",
        L"V\u00E9rification de l'int\u00E9grit\u00E9 du disque recommand\u00E9e.",
        L"Ouvrez le Centre d'actions pour v\u00E9rifier et r\u00E9soudre les probl\u00E8mes.",
        L"V\u00E9rifiez l'\u00E9tat du syst\u00E8me",
        L"Batterie faible. Branchez l\u2019appareil \u00E0 une source d\u2019alimentation.",
        L"Des mises \u00E0 jour Windows sont en attente. Red\u00E9marrez pour les appliquer.",
        L"Le Bureau \u00E0 distance est activ\u00E9 sans authentification au niveau du r\u00E9seau.",
        L"Le lecteur syst\u00E8me n'est pas prot\u00E9g\u00E9 par BitLocker.",
        L"Cliquez pour voir les nouveaut\u00E9s.",
        L"Nouveau probl\u00E8me critique d\u00E9tect\u00E9. Cliquez pour v\u00E9rifier maintenant.",
        L"Probl\u00E8me critique toujours pr\u00E9sent. Cliquez pour le r\u00E9soudre."
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
        L"\u041E\u0431\u043D\u0430\u0440\u0443\u0436\u0435\u043D\u043E %d \u043F\u0440\u043E\u0431\u043B\u0435\u043C", 
        L"\u041E\u0431\u043D\u0430\u0440\u0443\u0436\u0435\u043D\u0430 \u043F\u0440\u043E\u0431\u043B\u0435\u043C\u0430. \u041F\u0440\u043E\u0432\u0435\u0440\u044C\u0442\u0435 \u0441\u043E\u0441\u0442\u043E\u044F\u043D\u0438\u0435 \u0431\u0435\u0437\u043E\u043F\u0430\u0441\u043D\u043E\u0441\u0442\u0438 \u0441\u0438\u0441\u0442\u0435\u043C\u044B.",
        L"SmartScreen \u043E\u0442\u043A\u043B\u044E\u0447\u0451\u043D. \u041F\u0440\u0438\u043B\u043E\u0436\u0435\u043D\u0438\u044F \u0438\u0437 \u0438\u043D\u0442\u0435\u0440\u043D\u0435\u0442\u0430 \u043D\u0435 \u0431\u0443\u0434\u0443\u0442 \u043F\u0440\u043E\u0432\u0435\u0440\u044F\u0442\u044C\u0441\u044F.",
        L"\u0420\u0435\u0437\u0435\u0440\u0432\u043D\u043E\u0435 \u043A\u043E\u043F\u0438\u0440\u043E\u0432\u0430\u043D\u0438\u0435 \u0441\u0438\u0441\u0442\u0435\u043C\u044B \u043D\u0435 \u043D\u0430\u0441\u0442\u0440\u043E\u0435\u043D\u043E \u0438\u043B\u0438 \u043D\u0435 \u0432\u044B\u043F\u043E\u043B\u043D\u044F\u0435\u0442\u0441\u044F.",
        L"\u0421\u043B\u0443\u0436\u0431\u0430 \u043E\u0442\u0447\u0451\u0442\u0430\u043F\u0430 \u043E\u0431 \u043E\u0448\u0438\u0431\u043A\u0430\u0445 Windows \u043E\u0442\u043A\u043B\u044E\u0447\u0435\u043D\u0430.",
        L"\u0420\u0435\u043A\u043E\u043C\u0435\u043D\u0434\u0443\u0435\u0442\u0441\u044F \u043F\u0440\u043E\u0432\u0435\u0440\u0438\u0442\u044C \u0441\u043E\u0441\u0442\u043E\u044F\u043D\u0438\u0435 \u0434\u0438\u0441\u043A\u0430.",
        L"\u041E\u0442\u043A\u0440\u043E\u0439\u0442\u0435 \u0426\u0435\u043D\u0442\u0440 \u0434\u0435\u0439\u0441\u0442\u0432\u0438\u0439 \u0434\u043B\u044F \u043F\u0440\u043E\u0432\u0435\u0440\u043A\u0438 \u0438 \u0443\u0441\u0442\u0440\u0430\u043D\u0435\u043D\u0438\u044F \u043F\u0440\u043E\u0431\u043B\u0435\u043C.",
        L"\u041F\u0440\u043E\u0432\u0435\u0440\u044C\u0442\u0435 \u0441\u043E\u0441\u0442\u043E\u044F\u043D\u0438\u0435 \u0441\u0438\u0441\u0442\u0435\u043C\u044B",
        L"\u0417\u0430\u0440\u044F\u0434 \u0431\u0430\u0442\u0430\u0440\u0435\u0438 \u043D\u0438\u0437\u043A\u0438\u0439. \u041F\u043E\u0434\u043A\u043B\u044E\u0447\u0438\u0442\u0435 \u0443\u0441\u0442\u0440\u043E\u0439\u0441\u0442\u0432\u043E \u043A \u0438\u0441\u0442\u043E\u0447\u043D\u0438\u043A\u0443 \u043F\u0438\u0442\u0430\u043D\u0438\u044F.",
        L"\u041E\u0436\u0438\u0434\u0430\u044E\u0442\u0441\u044F \u043E\u0431\u043D\u043E\u0432\u043B\u0435\u043D\u0438\u044F Windows. \u041F\u0435\u0440\u0435\u0437\u0430\u0433\u0440\u0443\u0437\u0438\u0442\u0435 \u043A\u043E\u043C\u043F\u044C\u044E\u0442\u0435\u0440 \u0434\u043B\u044F \u0438\u0445 \u043F\u0440\u0438\u043C\u0435\u043D\u0435\u043D\u0438\u044F.",
        L"\u0423\u0434\u0430\u043B\u0451\u043D\u043D\u044B\u0439 \u0440\u0430\u0431\u043E\u0447\u0438\u0439 \u0441\u0442\u043E\u043B \u0432\u043A\u043B\u044E\u0447\u0451\u043D \u0431\u0435\u0437 \u043F\u0440\u043E\u0432\u0435\u0440\u043A\u0438 \u043F\u043E\u0434\u043B\u0438\u043D\u043D\u043E\u0441\u0442\u0438 \u043D\u0430 \u0443\u0440\u043E\u0432\u043D\u0435 \u0441\u0435\u0442\u0438 (NLA).",
        L"\u0421\u0438\u0441\u0442\u0435\u043C\u043D\u044B\u0439 \u0434\u0438\u0441\u043A \u043D\u0435 \u0437\u0430\u0449\u0438\u0449\u0451\u043D \u0448\u0438\u0444\u0440\u043E\u0432\u0430\u043D\u0438\u0435\u043C BitLocker.",
        L"\u041D\u0430\u0436\u043C\u0438\u0442\u0435, \u0447\u0442\u043E\u0431\u044B \u0443\u0437\u043D\u0430\u0442\u044C \u043F\u043E\u0434\u0440\u043E\u0431\u043D\u043E\u0441\u0442\u0438.",
        L"\u041E\u0431\u043D\u0430\u0440\u0443\u0436\u0435\u043D\u0430 \u043D\u043E\u0432\u0430\u044F \u043A\u0440\u0438\u0442\u0438\u0447\u0435\u0441\u043A\u0430\u044F \u043F\u0440\u043E\u0431\u043B\u0435\u043C\u0430. \u041D\u0430\u0436\u043C\u0438\u0442\u0435, \u0447\u0442\u043E\u0431\u044B \u043F\u0440\u043E\u0432\u0435\u0440\u0438\u0442\u044C \u0441\u0435\u0439\u0447\u0430\u0441.",
        L"\u041A\u0440\u0438\u0442\u0438\u0447\u0435\u0441\u043A\u0430\u044F \u043F\u0440\u043E\u0431\u043B\u0435\u043C\u0430 \u0432\u0441\u0451 \u0435\u0449\u0451 \u0430\u043A\u0442\u0443\u0430\u043B\u044C\u043D\u0430. \u041D\u0430\u0436\u043C\u0438\u0442\u0435, \u0447\u0442\u043E\u0431\u044B \u0438\u0441\u043F\u0440\u0430\u0432\u0438\u0442\u044C."
    }},
    { 0x0816, {
        L"Centro de A\u00E7\u00E3o",
        L"Abrir o Centro de A\u00E7\u00E3o",
        L"Centro de A\u00E7\u00E3o",
        L"Resolu\u00E7\u00E3o de Problemas",
        L"Windows Update",
        L"2 mensagens importantes",
        L"1 mensagem importante",
        L"N\u00E3o foram detetados problemas.\nPode utilizar o Centro de A\u00E7\u00E3o para rever as mensagens recentes sobre o estado do seu computador e encontrar solu\u00E7\u00F5es para os problemas.",
        L"Centro de A\u00E7\u00E3o",
        L"Centro de A\u00E7\u00E3o",
        L"Centro de A\u00E7\u00E3o",
        L"A Firewall do Windows est\u00E1 desativada.",
        L"A prote\u00E7\u00E3o antiv\u00EDrus est\u00E1 desativada.",
        L"O Windows Update n\u00E3o est\u00E1 configurado.",
        L"O Controlo de Conta de Utilizador est\u00E1 desativado.",
        L"Clique para abrir o Centro de A\u00E7\u00E3o.",
        L"O Centro de A\u00E7\u00E3o detetou novos problemas.",
        L"O Windows Update n\u00E3o est\u00E1 configurado para atualizar automaticamente.",
        L"A prote\u00E7\u00E3o antisspyware est\u00E1 desativada.",
        L"As defini\u00E7\u00F5es de seguran\u00E7a da Internet precisam de aten\u00E7\u00E3o.",
        L"O servi\u00E7o Centro de Seguran\u00E7a n\u00E3o est\u00E1 em execu\u00E7\u00E3o.",
        L"A prote\u00E7\u00E3o em tempo real do Windows Defender est\u00E1 desativada.",
        L"...e mais",
        L"N\u00E3o foram detetados problemas",
        L"%d problemas detetados.",
        L"Foi detetado um problema. Reveja o estado de seguran\u00E7a do sistema.",
        L"O SmartScreen est\u00E1 desativado. As aplica\u00E7\u00F5es da Web n\u00E3o ser\u00E3o verificadas.",
        L"A c\u00F3pia de seguran\u00E7a do sistema n\u00E3o est\u00E1 configurada ou n\u00E3o est\u00E1 em execu\u00E7\u00E3o.",
        L"O servi\u00E7o de Relato de Erros do Windows est\u00E1 desativado.",
        L"Recomenda-se a verifica\u00E7\u00E3o do estado do disco.",
        L"Abra o Centro de A\u00E7\u00E3o para rever e corrigir problemas.",
        L"Reveja o estado do sistema",
        L"Bateria fraca. Ligue o dispositivo a uma fonte de alimenta\u00E7\u00E3o.",
        L"Atualiza\u00E7\u00F5es do Windows pendentes. Reinicie o computador para as aplicar.",
        L"O Ambiente de Trabalho Remoto est\u00E1 ativado sem Autentica\u00E7\u00E3o ao N\u00EDvel de Rede.",
        L"A unidade de sistema n\u00E3o est\u00E1 protegida com BitLocker.",
        L"Clique para ver as novidades.",
        L"Novo problema cr\u00EDtico detetado. Clique para rever agora.",
        L"Problema cr\u00EDtico ainda presente. Clique para o resolver."
    }},
    { 0x0407, {
        L"Aktionscenter",
        L"Aktionscenter \u00F6ffnen",
        L"Aktionscenter",
        L"Problembehandlung",
        L"Windows Update",
        L"2 wichtige Meldungen",
        L"1 wichtige Meldung",
        L"Derzeit wurden keine Probleme erkannt.\nSie k\u00F6nnen das Aktionscenter verwenden, um aktuelle Meldungen zum Status Ihres Computers zu \u00FCberpr\u00FCfen und L\u00F6sungen f\u00FCr Probleme zu finden.",
        L"Aktionscenter",
        L"Aktionscenter",
        L"Aktionscenter",
        L"Die Windows-Firewall ist deaktiviert.",
        L"Der Virenschutz ist deaktiviert.",
        L"Windows Update ist nicht konfiguriert.",
        L"Die Benutzerkontensteuerung ist deaktiviert.",
        L"Klicken Sie, um das Aktionscenter zu \u00F6ffnen.",
        L"Das Aktionscenter hat neue Probleme erkannt.",
        L"Windows Update ist nicht f\u00FCr die automatische Aktualisierung festgelegt.",
        L"Der Antispywareschutz ist deaktiviert.",
        L"Die Internetsicherheitseinstellungen erfordern Aufmerksamkeit.",
        L"Der Dienst Sicherheitscenter wird nicht ausgef\u00FChrt.",
        L"Der Echtzeitschutz von Windows Defender ist deaktiviert.",
        L"...und mehr",
        L"Derzeit wurden keine Probleme erkannt",
        L"%d Probleme erkannt.",
        L"Es wurde ein Problem erkannt. Bitte \u00FCberpr\u00FCfen Sie den Sicherheitsstatus.",
        L"Der SmartScreen ist deaktiviert. Apps aus dem Web werden nicht \u00FCberpr\u00FCft.",
        L"Die Systemssicherung ist nicht konfiguriert oder nicht aktiv.",
        L"Der Dienst Windows-Fehlerberichterstattung ist deaktiviert.",
        L"\u00DCberpr\u00FCfung der Festplattenintegrit\u00E4t empfohlen.",
        L"\u00D6ffnen Sie das Aktionscenter, um Probleme zu \u00FCberpr\u00FCfen und zu beheben.",
        L"\u00DCberpr\u00FCfen Sie den Systemstatus",
        L"Der Akku ist schwach. Schlie\u00DFen Sie das Ger\u00E4t an eine Stromquelle an.",
        L"Windows-Updates stehen aus. Starten Sie den Computer neu, um sie anzuwenden.",
        L"Remotedesktop ist ohne Authentifizierung auf Netzwerkebene aktiviert.",
        L"Das Systemlaufwerk ist nicht durch BitLocker gesch\u00FCtzt.",
        L"Klicken Sie, um zu sehen, was neu ist.",
        L"Neues kritisches Problem erkannt. Klicken Sie, um es jetzt zu \u00FCberpr\u00FCfen.",
        L"Kritisches Problem besteht weiterhin. Klicken Sie, um es zu beheben."
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
    PROB_ANTISPYWARE, PROB_INTERNET, PROB_UAC, PROB_SERVICE, PROB_DEFENDER_RT,
    PROB_SMARTSCREEN, PROB_BACKUP, PROB_WER, PROB_DISK_HEALTH,
    PROB_BATTERY, PROB_UPDATE_PENDING,
    PROB_RDP_NLA, PROB_BITLOCKER
};

// ============================================================================
// MSDT Diagnostic Packs (with fallback to generic troubleshooter)
// ============================================================================

// Verifica se un diagnostic pack MSDT esiste sul sistema corrente prima di
// invocarlo. I pack vivono in %SystemRoot%\diagnostics\system\<PackId>.
// Se il pack non c'e' (rimosso in build piu' recenti, o SKU diverso),
// OpenMsdtDiagnostic ricade sullo shell folder generico dei troubleshooter,
// esattamente come fa gia' OpenProblemAction per i problemi non mappati.
static BOOL IsMsdtPackAvailable(const WCHAR* packId) {
    WCHAR path[MAX_PATH];
    WCHAR sysRoot[MAX_PATH];
    if (!GetEnvironmentVariableW(L"SystemRoot", sysRoot, MAX_PATH)) {
        StringCchCopyW(sysRoot, MAX_PATH, L"C:\\Windows");
    }
    StringCchPrintfW(path, MAX_PATH, L"%s\\diagnostics\\system\\%s", sysRoot, packId);
    DWORD attrs = GetFileAttributesW(path);
    return (attrs != INVALID_FILE_ATTRIBUTES) && (attrs & FILE_ATTRIBUTE_DIRECTORY);
}

static void OpenGenericTroubleshooter(void) {
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_INVOKEIDLIST;
    sei.lpVerb = L"open";
    sei.lpFile = L"explorer.exe";
    sei.lpParameters = L"shell:::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}";
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExW(&sei)) {
        Wh_Log(L"Troubleshooting shell command failed, using fallback");
        ShellExecuteW(NULL, L"open", L"control.exe",
                     L"/name Microsoft.Troubleshooting", NULL, SW_SHOWNORMAL);
    }
}

// Lancia un diagnostic pack MSDT specifico se disponibile, altrimenti ricade
// sull'elenco troubleshooter generico (stesso comportamento del ramo
// "ALL OTHER PROBLEMS" gia' presente in OpenProblemAction).
static void OpenMsdtDiagnostic(const WCHAR* packId) {
    if (IsMsdtPackAvailable(packId)) {
        WCHAR sysDir[MAX_PATH];
        WCHAR msdtPath[MAX_PATH];
        WCHAR params[64];
        UINT len = GetSystemDirectoryW(sysDir, MAX_PATH);
        if (len > 0 && len < MAX_PATH) {
            StringCchCopyW(msdtPath, MAX_PATH, sysDir);
            StringCchCatW(msdtPath, MAX_PATH, L"\\msdt.exe");
        } else {
            StringCchCopyW(msdtPath, MAX_PATH, L"msdt.exe");
        }
        StringCchPrintfW(params, ARRAYSIZE(params), L"-id %s", packId);

        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.fMask = SEE_MASK_FLAG_NO_UI;
        sei.lpVerb = L"open";
        sei.lpFile = msdtPath;
        sei.lpParameters = params;
        sei.nShow = SW_SHOWNORMAL;

        if (ShellExecuteExW(&sei)) {
            return; // successo
        }
        Wh_Log(L"msdt.exe -id %s failed to launch, falling back to generic troubleshooter", packId);
    } else {
        Wh_Log(L"MSDT pack %s not available on this system, falling back", packId);
    }
    OpenGenericTroubleshooter();
}

// ============================================================================
// Open Problem Action (Firewall or Troubleshooting)
// ============================================================================
void OpenProblemAction(int problemType) {
    
    // ONLY firewall opens directly the settings
    if (problemType == PROB_FIREWALL) {
        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_INVOKEIDLIST;
        sei.lpVerb = L"open";
        sei.lpFile = L"explorer.exe";
        sei.lpParameters = L"shell:::{4026492F-2F69-46B8-B9BF-5654FC07E423}";
        sei.nShow = SW_SHOWNORMAL;
        
        if (!ShellExecuteExW(&sei)) {
            Wh_Log(L"Firewall shell command failed, using fallback");
            ShellExecuteW(NULL, L"open", L"control.exe",
                         L"/name Microsoft.WindowsFirewall", NULL, SW_SHOWNORMAL);
        }
        return;
    }

    // SmartScreen -> Windows Security App & Browser control
    if (problemType == PROB_SMARTSCREEN) {
        // Try Windows Security > App & browser control (ms-settings URI on Win10/11)
        if (ShellExecuteW(NULL, L"open", L"ms-settings:windowsdefender-appbrowser",
                          NULL, NULL, SW_SHOWNORMAL) > (HINSTANCE)32) {
            return;
        }
        // Fallback: open Windows Security app
        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.fMask = SEE_MASK_FLAG_NO_UI;
        sei.lpVerb = L"open";
        sei.lpFile = L"explorer.exe";
        sei.lpParameters = L"shell:::{4026492F-2F69-46B8-B9BF-5654FC07E423}";
        sei.nShow = SW_SHOWNORMAL;
        ShellExecuteExW(&sei);
        return;
    }

    // Problemi con un diagnostic pack MSDT pertinente: prova quello, con
    // fallback automatico al troubleshooter generico se il pack manca o
    // il lancio fallisce.
    if (problemType == PROB_INTERNET) {
        OpenMsdtDiagnostic(L"NetworkDiagnosticsWeb");
        return;
    }
    if (problemType == PROB_DEFENDER_RT ||
        problemType == PROB_ANTIVIRUS ||
        problemType == PROB_ANTISPYWARE) {
        // Antivirus/antispyware in Windows moderno sono la stessa componente
        // Defender: riusano lo stesso pack di diagnostica di sicurezza.
        OpenMsdtDiagnostic(L"WindowsSecurityDiagnostic");
        return;
    }
    if (problemType == PROB_AUTOUPDATE || problemType == PROB_UPDATE_PENDING) {
        OpenMsdtDiagnostic(L"WindowsUpdateDiagnostic");
        return;
    }
    if (problemType == PROB_BATTERY) {
        OpenMsdtDiagnostic(L"PowerDiagnostic");
        return;
    }

    // ALL OTHER PROBLEMS -> Troubleshooting generico (comportamento invariato)
    OpenGenericTroubleshooter();
}
static LANGID g_LastDetectedUILang = 0;

void DetermineLocale() {
    switch (g_Settings.language) {
        case 1: g_CurrentLocalePack = FindLocalePack(0x0409); g_LastDetectedUILang = 0x0409; break;
        case 2: g_CurrentLocalePack = FindLocalePack(0x0410); g_LastDetectedUILang = 0x0410; break;
        case 3: g_CurrentLocalePack = FindLocalePack(0x040A); g_LastDetectedUILang = 0x040A; break;
        case 4: g_CurrentLocalePack = FindLocalePack(0x040C); g_LastDetectedUILang = 0x040C; break;
        case 5: g_CurrentLocalePack = FindLocalePack(0x0419); g_LastDetectedUILang = 0x0419; break;
        case 6: g_CurrentLocalePack = FindLocalePack(0x0816); g_LastDetectedUILang = 0x0816; break;
        case 7: g_CurrentLocalePack = FindLocalePack(0x0407); g_LastDetectedUILang = 0x0407; break;
        default: {
            LANGID ui = GetUserDefaultUILanguage();
            g_CurrentLocalePack = FindLocalePack(ui);
            g_LastDetectedUILang = ui;
            break;
        }
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

// Handle reale del modulo Windhawk. Usare explorer.exe come hInstance lascia
// le classi finestra associate al processo host e rende il hot-reload fragile.
static HINSTANCE g_hModInstance = NULL;
static volatile LONG g_WscCallbacksInFlight = 0;

static HINSTANCE GetModInstance(void) {
    return g_hModInstance ? g_hModInstance : (HINSTANCE)GetModuleHandleW(NULL);
}

// Applica il tema corrente alle finestre della mod: aggiorna il flag globale
// usato dalla paint routine, riapplica l'attributo DWM per la dark title bar e
// forza il repaint immediato. Da chiamare sul thread proprietario delle
// finestre (tray thread), ad es. da WM_SETTINGS_CHANGED o WM_SETTINGCHANGE.
// Re-paint all currently visible localized UI after a locale switch.
// Strings already shown in a balloon are refreshed the next time it appears.
static void RefreshLocalizedUI(void) {
    if (g_Ctx.isUninitializing) return;
    EnsureTrayTooltip();
    if (g_Ctx.hWndFlyout && IsWindow(g_Ctx.hWndFlyout) && IsWindowVisible(g_Ctx.hWndFlyout)) {
        InvalidateRect(g_Ctx.hWndFlyout, NULL, TRUE);
    }
    if (g_Ctx.hWndNotify && IsWindow(g_Ctx.hWndNotify) && IsWindowVisible(g_Ctx.hWndNotify)) {
        InvalidateRect(g_Ctx.hWndNotify, NULL, TRUE);
    }
}

static void ApplyThemeToWindows(void) {
    BOOL dark = GetEffectiveDarkMode();
    g_Ctx.darkMode = dark;
    if (g_Ctx.isUninitializing) return;

    BOOL useDark = dark;
    if (g_Ctx.hWndFlyout && IsWindow(g_Ctx.hWndFlyout)) {
        DwmSetWindowAttribute(g_Ctx.hWndFlyout, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDark, sizeof(useDark));
        InvalidateRect(g_Ctx.hWndFlyout, NULL, TRUE);
    }
    if (g_Ctx.hWndNotify && IsWindow(g_Ctx.hWndNotify)) {
        DwmSetWindowAttribute(g_Ctx.hWndNotify, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDark, sizeof(useDark));
        InvalidateRect(g_Ctx.hWndNotify, NULL, TRUE);
    }
}

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
// Stato del backoff del refresh periodico (vedi REFRESH_TIMER_ID in TrayMsgHandlerProc).
// Solo scrittura/lettura dal tray thread (proprietario del timer), niente lock necessario.
static DWORD g_RefreshNoChangeCount = 0;
static UINT_PTR g_RefreshCurrentInterval = 0;
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
static HICON g_hProblemBalloonIcon = NULL;  
static BOOL g_ProblemBalloonShowing = FALSE;
static RECT g_CachedTrayIconRect = {0}; // Cached tray icon rect for mouse hook
static DWORD g_LastProblemBalloonTick = 0;
static DWORD g_LastProblemBalloonSignature = 0;
static int g_LastProblemBalloonState = STATE_GOOD;
static HHOOK g_hMouseHook = NULL;
static HHOOK g_hKeyboardHook = NULL;
static BOOL g_Initialized = FALSE;
// TRUE for the very first RefreshSecurityState() call after mod startup.
// Forces a balloon notification even if the cooldown has not elapsed yet,
// so the user always gets a summary of existing problems on every boot.
static BOOL g_isStartupCheck = TRUE;

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
// ID 0/1/2 sono PNG RGBA validi e completi: le precedenti risorse ID 1 e 2
// contenevano dati IDAT corrotti, quindi la sgranatura non dipendeva dal resize.
// ID 1 usa un badge giallo di avviso; ID 2 un badge rosso con X.
static const WCHAR icon_id0_b64[] = L"iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAG60lEQVR42uWWW2xUxxnH/zNzztlz1rtee722MXbicjELGNtgCMEtgbYGAxVNglIqLg9tyi0NVNTxC0htkPpUCZWESG2ubdOmKZfgUqlAgEDUAAHXQHFxBDYX28Gs7bX34l3v5dxmTh9SEEkgKS31S7/H7xvN/PTNN//5A//vQV559fUdicTw07bNrxcUFLzwzIZ1b48mgJRIDD+9eNGivEuXO2Z2dnY2AfhSgDcOpic6XGwejqeWhfvipX2hWKi7K9J05tCTe+4bAAAud3TiZm8IwnEqv2jxthfaNZ/ft0XY9hYmSUqgwA1bMAwmaKlhDe+uWXCg7B/Hlv7ifgCoz5f321AoBO4IGIbR8syzP3LfbeFzz59caFvWRdu0nq+ZICtL5qjI88rIZgW0HA2lU6dAcP6zeSvOeO6rA+lUcmcgEKgvf/jh6ubm5lzbyN4url5zUHG56OOyi23SM9b8QLGKr9eNwfWbaew/ZcAWBMNRE4OhCEbiKag5brdlmM3zVrY8dWLXnNS/BSCE6M1kMr198YKqs1enTVcJ37tsxZJBWXJVctuZnRE2XFTFpCkT4fMyvPxmO2yqoWDsGCiaBlVzQcvRkEqmwc0sbMNqYBK7MH9ly+oPds1p/dJXAAA7d+5cdSO94Hfvn+yTkr09jqYpJK84AG+gCIGxhSgsciPU2YGPu/rhyfMiv7AAgbHF8PrzYVscsaFhhG8OIT4UR3okC6/fDyYxUEk+Qxl7jUqsnUqs/b3Xq80lmzrGUcYmUkZnEsbWSADAOT/mEX8LLZiRLD+d+kv0Sk/NnwOVa9Ym0hkMnr+MRLgPikKg5bhg6iYMXUc2lYHb6wGTZahuFW6PikzKhWxaByUOhC0A2HUA6kAAQgga1rfD4QLFhS6kdAdZQ8QYABw5ciS9pKGu8pEZk2r1bFqV6PW32s/LFbG+fn8mkQABQCjAGAWTKCRZguyS4dJUuFQVAGAZNoysAcs04fXlonb2BJSUeDC+3IeaqX7MrvWjOujDrGm5mFXlxekLCTgONki37kKW5X0g5Nv19fXFetac2daJrQD2OgLgwoHgDmybwzZtmLoBI6tDz2Sh5eRAViSoOeptoHBvPwzdhMejIS9fxdXrDuAADiFYu2oCmg/3wzL5qcO/mvQWvWMe3o9EIr3BYBDeXN+yZYs6WgC0CeFAcIBzB9wWsC0bpmF+ApDKwDJNUEahai7keDUoqgJJYUgNpxAdSqC7K4qBvgSSSQNPLHoILeej6Po4ZQrOnwUAduv0o0ePioaGhjGBwsBc03K0m6Hw1c6uwHsAvkPIJ1dAKQFlBIwxSDKDrChwqS64NBV5XgklRW4onnwYXMFINArKJIAARUUePLm0At03kjjREgYh2Hrs1zX7byvhrTBN8+2+/v4fBoMVJW1tHSsAfJMQhOCg9HNd+Ncw2kYWDbOK4PfJOHx6BMPRGHg2Ac3nRfm4EpSV5SLgd+Ho8W4MDKZBJWn/X//wyPbbSngnQGNjY9dA/8CFosIC+P3+6qX1N2ZQSvYRQuA4t+ZAwLY4LNOCP1fBxlVlgMPR/EEW4bi4AdAXM4m0EKaOob4ILrRcw/53/o6bvXEImx8Wtr36U1L8WWEwTGtvPB7PBidNzGOMLaaU7GMSAQX91DBOq3oIK5dPxZ/e7cXuQwMwTb5NVqSKXdsnN45ER47rIwaiAzEMx1IQXJjC5j8XnC899c7c7Oc+ozvDtoz9PT09P51WWTXh9JmLy+tqB186e3FMggvhczhAqITHGh7F+K/k4qVffgiHuRL+osDq7dvyD97awxFircPJFgBBwfl5AC+fO7K4+55K+Nl45dXX3qyb89XvHThwEl09fXNPtI77iW2Jxb6SEsycPx1WMoL2sxehuJRL3nzvE8f2fOvaf+oH2N2SCxcujLjd2nJfXoHr2rUbTmS4sKNsZt38sRPH40prGzrbOkDg7GGMPv7hgacG/itDcrekbVunQ6H+HpNNqzrX4f+uruud3W3tSEVjoNQxFYVuvdT6gx0PwhHRuyWbmppEOGIe+qjHg5he6o3GrBnJoSFdCLHHEajsubR+xwOzZPcs0JHfaNbZjd+Y3u0533ruo47LF1ZZRs+lB+0J6b0KTc/9+OqY/MGLC+ZNQHXl2OCs2iD5X5hS+gU1xwHeKCwssL8291F18pSa+tEGAIHzx2QyEamsDEJVpWXr1jWRUQVobGw0YrHYu7leFUVFubWUiVmjCgAAQohdQ0OD+uTg+Fw4vGHUATjnx8PhcHdFxXioqrRy/boN5aMKsGnTRmEY+kHbtjF9elUlZeSxUQUAgHQ6/WJXV3eitLQUsix/f9QBNm/eHBoY6P/9lStXDErI7gcJ8E8ZfjuCPNKFdAAAAABJRU5ErkJggg==";
static const WCHAR icon_id1_b64[] = L"iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAJBUlEQVR42uWXaXRV1RXHf/fc+8bkZXxJgARQyAAJJJIwiCJiwSCKQ5YGBdvVWiYFlUK+IO0yYr+0yxbBZbWt1modkGBKJxEQcUJAhIJggZSSROBlnl7y8t6707n9AKGIDLpWV7/0fD173/v77/0/e50D/+9L+fVvXlgTDvc8YFn2ifT09KcfXLzw9f8lgBYO9zxwy8yZKUeOHiurq6urAq4I8OLb/bmOLZf1dEcqWpu6s5tCXaGG+o6q3Zvv2vCtAQCOHqvj9KkQ0nGKLhdc/fRhX3Ja8kppWStVTXMH0/1YUqUtLLJ1s+fNkhl/y/l8++xffhsAkZyc8vtQKITtSHRd3/Pgkkf8Fwtc8fjHN1umecgyzMdLRrrcs671khJwEYtJfAk+sgtHI237yan37U78VhXoj/SuCwaD04cPG1ZcW1ubZOmxc5v3z3/b7fGIO1we9eF41LwxmOVl2uRBnDjdz6adOpZU6Ok0aAt10NcdwZvg95u6UTt17p67P1p/beQbAUgpT0Wj0VNN3eljPzs+5hqvYtdU3DerzaV5imzLmRiVFh7hJX90LskBledfPowlfKQPGYTb58Pr8+BL8BHp7cc2Yli6Wa5q6oEb5+65/8P11+694ikAWLdu3byT/TNe2fFxk9Z7qtHx+dxKSlaQQDCT4JAMMjL9hOqO8WV9M4kpAVIz0gkOySKQlopl2nS199B6up3u9m76+2IE0tJQNRWhuXYLVf2t0NTDQlMPv/tCsTHr4WNXC1XNFaooU1R1vgZg2/b2RPlpaMa43uG7In/t/GdjyZ+CRfMXhPujtO0/Sri1CbdbwZfgwYgb6PE4sUgUfyAR1eXC6/fiT/QSjXiI9ccRioO0JGBNBiajgKIolC86jGNLsjI8ROIOMV12qQBbt27tn1U+uWjCuPzSeKzfq4kTrx7e78rrampOi4bDKIAiQFUFqibQXBoujwuPz4vH6wXA1C30mI5pGASSkyidOJLBgxMZMTyZksI0JpamUVyQzPgxSYwfG2DXgTCOw2JtoBcul+stFOX26dOnZ8VjRtnBOh4DahwJtnSQtoNl2ViGhRHX0WNx4tEYvoQEXG4Nb4L3HFDrqWb0uEFioo+UVC/HTzjggKMoLJg3ktotzZiGvXPLc/mvivP8sKOjo+NUQUEBgaTkioqZx/YAB6V0kDbYtoNtSSzTwtCNMwCRKKZhIFSB1+chIeDD7XWjuVUiPRE628M01HfQ0hSmt1fnzplD2bO/k/ovI4a07SUA6sDft23bJsvLywcFM4JTDNPxnQ61Hq+rD74L3KMoZ1oghIJQFVRVRXOpuNxuPF4PHp+XlIDG4Ew/7sRUdNtNX2cnQtVAUcjMTOSu2Xk0nOzloz2tKAqPbf9dyaZzk3BgGYbxelNz80MFBXmDDx48dh/wHUUhhEP216pw1oyWHqN8fCZpyS627Oqjp7MLOxbGl5zI8KuHkJOTRDDNw7b3Gmhp60do2qYPXpvw1LlJeD7A8uXL61uaWw5kZqSTlpZWPHv6yXFCKG8pioLjDPhAYpk2pmGSluRm6bwccGxqP4zR2i1PglgbC0ekNHRSW1ah/v1m/ly7l9DpLhzL2iIt6/6vjOILB4NumDXd3d2xgvzcFFVVbxFCeUvVFATiK2YcM3YocysL+eM7p3hzcwuGYVe73FreG/5RVb2dfe8pkQZGJ/2FsuFHSJc7LNuUP5uovHTnzo1TYpcFsEx9U2NjY9OYonxUVaucXNp2SAgl7HDGjIrQuKF8EnmjcnjmV5+w70BzOB6Jzn5qSeqTv1iSYmw8guJIZ8Eo9/N1N0ywuel6xRnjXXtyVsbNq9fU/sF0zg6/SwKsWLGity/St1PVYGhOVnFykjHW7VZ3u1wqGcNymFpRTjRqsv6V9+lo6ztixPXxrz1V8jZANYg5G5G3B+doxcO/uKowHydjmGOPL7JG9LRToSg4T1T/x/gXBQDQ4/EX29pa+gqL8hCK8kBigmtv7vVTyJ1QxqFPPmfn1r3oMXODZVqTtm+49V8DeUU1KIATlXz/mtF4VA92fxilKB/H42bR2TD5tfvA19pgmbtCoeZGQx0zdt+xtDnxeLyu4eBhIp1dCOEYbrd47MjeH645P8cBRZmDXHkX6X4/SyaV4fz8BdQdn6I8sxJZnM+0xdOZtno1H9RUos7ZiH3JClRVVcnWDmPzF42JdMWzA51d5rje9va4lHKDIylqPLJozYU5G2sQgKObPDKphGDCIGS4D6WrF9xunKkTIeCn2qlG/KMQ57IVANBE30s+87OlN13TkLh/774vjh09MM/UG49cLHZAfdV3SQjYPFhWjIOJUAW4XaDrqHljkCOGMm35XkrXvsP+gSqISwFUrfjR8UGpbYdmTB1JcdGQgvGlBcqlYjdWnlFvdXJ34Uiy0jOQWChCgFBASsCHM64IFJWlZ5gvY8IBYQ68mJGRbl0/ZZJ31OiS6ZdSX1mDrK7Enehn1XVlgHLmqOkGRKJnPoSOGFuIk5PFvGV3UDBnI7ZTjRCXv604b/T2hjuKigrwerWKhQurlIupVxScvjjzxhVSkD0M2zEQmLDkXnj5pzByKDhRlIRU5A0TcLsdVgE88cEVAJYvX653dXW9kxTwkpmZVCpUOf7CmAFDuVSWTijBQT2rWIERuVBWCokpZ4tuIIoKcNJSuaeqgszVH17GAwNLSrm+vb0tPqpgRBKOXX7+Xk0l6urVyGUzmXZ1DqWDBuPYUVQJGBKq18D3VkBbG0gVTB3Fn4QcV4jfirIIcK4IYNv2e62trQ15eSPwerW5ixYuHn6hep+fH996E9Kdian6sbQAltuL1d6D1daFpSpYagDLFcDCjz19CuagLB6aX06aeiWAzZs3O7fddutVwWDwOk0TmU1Nof379u07VFOJ+vBzyOp7mZ0d5CcJfkTLSbSmECJ0EtHSgrgqE1GSi1BAhOoRTSFEUyNqSyeqEScQ10nWvsndvb+/f219fcPC7OzsZJfL9QPgtcoaJAqgUd8TYVXNZlTbQRHnWgd+L2gavL+Lr7hNSgimgMfD/m/8glmzZs0zzz77bHzZo48u+G8+Tv8NTu4OxreiPpsAAAAASUVORK5CYII=";
static const WCHAR icon_id2_b64[] = L"iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAJzklEQVR42uWXW3BV1RnH/2utvffZ5+RckpOTE0K4SLhEDIQQEEQFq4EAM9hCC8hNp4CAFToa0zrqg2j7okUj2BlthVodFeUmOhbkJiJYQC4SCEICkgTCSXKSk3Pj3PZtrT5AGFoQ9MWXfk979vpm1m+v77+//7eA//cgf/v76ppYLDrfNK1zubm5rz22ZNEHPyeAFItF50+aODH71On6EQ0NDdUAbgmwZktygLD4E9FIYlqwNVLYGggHmhpD1Qe2Tl33kwEA4HR9Ay62BMCFKLlZ8vLX6uwer+cZbprPMElSfLkOmJyhI0YLNSP60bDx/+p1fNeUV38KAPV4sv8ZCARgCQ5N0w4+9vjvHTdKfOr5fRNMwzhh6sbzw/rLyuS7VGS7ZKTTHPYsOwrvGAxuWX8aN+uA8yedQDIRX+Xz+Sr69ulTumnTJreppa8uzl24RbHZ6C9lG1uWSRn3+fJV/GJMD5y7mMTmrzWYnCDapaMjEMKlSAJqlsNhaPqmcbMP/mbvh3clfhQA57wllUq1tEZyhx4+O6RMJdb6abMmd8iSrcQyxagUN2GjKgYNHgCPi+HNd+pgUjtye/aAYrdDtdtgz7IjEU/C0tMwNaOSSezYfbMPzv3qw7sO3fIvAIBVq1bNuZAc/+7ufa1SvKVZ2O0Kyc73weXzw9czD3l+BwIN9Tjf2AZntgs5ebnw9cyHy5sD07AQ7owieLETkc4IkpfScHm9YBIDleQDlLG3qMTqqMTqdq4u1Scvq+9HGRtAGR1BGFsoAYBlWbuc/JvA+OHxvvsTn3WdaR72ia9k4aOxZAodR08jFmyFohDYs2zQMzq0TAbpRAoOlxNMlqE6VDicKlIJG9LJDCgR4CYHYI4BMAYEIISgcnEdhMWRn2dDIiOQ1niYAcD27duTkyvHlNw5fFB5Jp1UJXruvbqj8sBwa5s3FYuBACAUYIyCSRSSLEG2ybDZVdhUFQBgaCa0tAZD1+HyuFE+qj8KCpwo6uvBsDu8GFXuRWmxByOHuDFyqAv7j8UgBJZI3bWQZXkjCHmwoqIiP5PWR9Q24FkA6wUHLC7ALQHTtGDqJvSMBi2dQSaVhj0rC7IiQc1SrwIFW9qgZXQ4nXZk56g4e04AAhCE4NE5/bFpWxsM3fp62xuD3qPX6GF3KBRqKS4uhsvtmTZtYv1BALWcC3ALsCwBy+QwDRO6pl8GSKRg6Dooo1DtNmS57FBUBZLCkIgm0NUZQ1NjF9pbY4jHNfxqYm8cPNqFxvMJnVvW4wDAunffsWMHr6ys7OHL892rG8J+MRA829Do2wlgOiGXS0ApAWUEjDFIMoOsKLCpNtjsKrJdEgr8DijOHGiWgktdITDGQAiQ53dh6pSBaLoQx96DQRCCZ3f9Y9jmq52wO3Rd/6C1re13xcUDC2pr62cBeIAQBCBQeN0pXBGjqaVROdIPr0fGtv2XEAt1gqcisHvc6NuvAL16ueHLkbFz5/doD6VBJHnznvfvXHG1E14LUFVV1dje1n7Mn5cLr9dbOqXiwnBKyUZCCITo1gGHaVgwdANet4Klc3oBwsLHe1IIhq0Lgsgrk/E0RzqO8PlmHN93HJs31qIlEIdl8m2OUP3D13nBtaHpxvpIJHJ/8aAB2R0d306ilGyklDwBTv5LjEOG9sb0aYPx8ectaI0y5Pj9y2Wb/FLhipLJTveQOX497Fd5Biah6MjqzaOefjt8nafn/jVxMrkeYDMB62ojujZqamrc+fkF3w6+fWj/t9ZsPBEKs7GHT/S4YJncIwA4nAoqHhyNotvc+Gr3CQgqx7z5/rl5K8YecCC9dqDdObHIkQWfIoMRAgEgqhto0TScTsTPxIQ571WYh7sh2P8CbN++XaucOLG8T5/eZcH2rvx0OrrlYru3iIAM8PYqxJhJ90BLJPDF1m+ga+YpwlAxePWUsw5o2x7w5o0rU1XTqyiCKQoIAEYpshWZ9zIMK99u93ca5vShQnxZDX5xOUDZjfrzhAkTQg6HfYYnO9f2/fcXRCiaV99rxJj7eg4owplDtWiorQeBWOfSO6Yu3zG/8xzEJxVe/9hBXBhkZLlsUkbNjg5CHA4iDIOYuk6df3iSuYMdli+RdLTo+tS7hfL+n2EkpBsBmKaxPxBoa9bZkKFH6r0zM5lMQ1NtHRJdYVAqdEWhz75waMGqmYD1KdiDZXbn+H6MmtaI4XLvbZ9C/+4UAtMeAo9EAQH4V/4FnofnIHnP3Sx/+mxjtCs7b1esq4oAT9MbAVRXV/NgSN96stmJcKbQ1RU2hsc7OzOc83WCo6T51OKaDVdyFdAFRQ6HYLJMrK4u6Ce/g21ICQo/Xgea6726uRUOI/LKShiEssIsh3AS+lA1kCX9oE/TS2/bjcNL7y9rch49dORk/eljcwyt+RQACIAQwFoAuFyE3elTFGLKMjWbzyPw69ko3PABbGWl6HtgD5jXCyvUhbZHFiK192uw3Fzq5Fz4ZVthp26V0h8CqH7qybM9cjpOjB/XH6UlPYtHlheTG3i5LBPipIRAmCaYxwMrHEZg1sOw2oNgOTkQuo7gsiokd30JqaAAwrJACIGNECZBZNObzApCAGvy8nLNe+4drd4+eFjFdVoBuAD0yy2NgmcyAADfc0+D5ecDhIAoCnKeWAq5qB94NAqwy7q3AG4BGr35tCLWxuOxUElJMVRVmrZoUTW58uViOUDfBWJxbp2OGoaQAQ5JRv6qV+B+ZC544hJaZ85D5shR2MeMRs+174DmekENU6Q5R6ehRxRYJ24KUFVVpYXD4c/dLhV+v7ucMj7y2oEWgMiAr21JZ4hIp4Q6agTc82aDR2NonTkPiU8/Q9u8BcgcPQa1vAz5q9+AbFlWZyZD4tzaXgOE6K1mNs75h52dHZnbi4vcEFZl9/sXAEsARIHro7pErClIJYnv2Wu1//E5tC16HKm9/4bUpw/MrjBa587HpZ27kXzjLZ4wDXooFgMgXrnOjG4UlmV9EQwGmwYOLIKqSrMXL1rSt7sMLwDkZURiafCZX4Y6IiFJYubb7xraV/sEy/OBp9MgDgcQj4vYIwvN0I4vyL5EggZMbfHLMI7NANgtAZYtW8o1LbPFNE2UlQ0toYyM7V57EeDrAbYC5pF2bkzaGmxv/04IOSXLhHEOmVEhCyF0SkmTYUjb4jF+Rk8veQ3G6uWAtAGwpB8zuyeTyZWNjU2LCgsLPbIs/xbA+91rMwHrsrGYh56DUr4n0vGkPSLPyFWU21RCiQWBsK4HE8L63AB/81WYh5YD0ouAeUM3/KGoqal5XVGUxWfPnFm26vXX11x3bQPoiwC/8uyIA6UEzKXDspzA8ZeALgCYAbANV6wYAP4DGqandm5eBd4AAAAASUVORK5CYII=";
static const WCHAR icon_id4_b64[] = L"iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAKISURBVFhH7ZdPiBJRHMc9hB208pAZaYcOHoSgW5duoQihIWwe6mJ0MQg6SGARwmTZQoSX2JHFw9qpMIJOsyCI0C2IREhYL0J4EPyDmqKrqdPv9/a9bdh0ndHZ2ct+4YPv6Xu/33fG995vRge6ADwAOAq28TvNJE0uh0fAXeAmcBU4C6wkElikYn2F3AMuA0uJBKH5RZvNtk6bM1WtVmv5fH4nnU5/jUQin6xW6zqLAXgBxVJkYDqdiuPxWBwOh2K/3xfr9fpuIpHI6PX6FzTWdQyqRCsZ6Ha7YqfTEbPZ7E8aC9eUIqlioNVqiWaz+TWNdxoDy5VqBlgswADI1lEYeAJcBGRJVQOhUOgjiwngrrgEHNR54NReU2UDsCum8XhcYHEXcBvY69D4KxtoNpukXS6Xa7A9t91u9wbLwXA6ne+MRuNL2j8aA71ej4yZTCZ05j81Go0mzfsY0N5AMpncpnl9gLYG2u32b4vFws4LskA1NRCLxT7TnHcAopkGBoPBbiaT+RYOhz/4/f5Nr9fLs7Eej4cPBoPvU6lUFhZbQ66BQqGwQ2M8Bc4BRP8ZyOVy3/GT/XYYdrv9TSAQ2IIK+Qv78wyUSqWyZOXfAPZFApFRINanYJ3Hhw78rywAE7avAJzL5doQBOFHNBr9grVglgG4SxXJBZG9L9U8A9eARboPcCaT6RUePsVisSo1UKlUajzPC5JSvYaTDmqeAbnCrcTmcA6H463P59s0GAzsdjPmPqyQATT/MgZQWHhuAQ8BNh/B50dMfGhhIoNp/mUNSHUGwDWzX2wWSW0DinVi4MTAc4AbjUZ/jsvAM4DD4nNcBvC8Z0kZil8uVhGWRTRB7gSg4eu5TvcXhIRVJI4SYh0AAAAASUVORK5CYII=";
static const WCHAR icon_id5_b64[] = L"iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAQMSURBVFhHvZdtTFNXGMdNZjDTZhLsC7O4pWZlvqAze/GL21wWGhRBtymJug+YfUGyxESyBJeFpEMZyeL4YpQQPqxLlgg4dcsSjCykDhS0WBgqatfIpsDYSjvubW9b2gLPnuf0nNqgzpZe+Se/9N7ec/7P/5x7zz3tIpQe+QSxcuiYvlswJRdPhU+Rfcj7SAHyApKRmDFwifM02Y+sQuYlZsLrQ15eXj0/fKzGx8c9AwMDd9va2rpqamrOGI3GeuGBlCJpK60As7OzMD09DZFIBEKhEExMTEw1NjZ2ZGVlfcm9NpNpOsooQCAQAFmWobOz8xb3omcqLakSYHJyEnQ63VfcbwkZpyrVAggvZBmSsp5FgM+QXCQlqRqgqqqqRXgitCpWInOlRRbHD1UOgKtitqGhoV34PoWdSPyE+2ccwOfzsePh4WEPLs8LRUVFJ0UNQWFh4QmNRnOUnz+bAIqisDYzMzO850N5vV4fr3sIWfgAzc3NF3jdD5CFDSBJkt9gMIj3BXtAFzRAXV3dWV5zD8L02ADhcHiqo6PjWnV19emysrKm0tLSU6JtSUnJqYqKiu9sNlsnPmzeVAMMDg7e5R5HkOUI0yMB7Hb7dfoU1/4Ps9n8dXl5+be4Q96ncxFgYqQL+s/lQ//5fAh4HeByuYaTnvwtSELMiNdno0iC9nn60UH3yoAI0bEJsVoslpPt7e39tbW1P9JeIAL89vNbMHTRBPd6CsB5flMsaUBs7SfrSQFeQ56mA4g1Ozv7GL18hoaGxinAmPsncLRoQR6zQNC7A/padbCv+Pnvse1u1muOnhQgVdFSEn2sBevXfmO3rZRu//IKxIIfMWgWLtlWjGKo56jDXGUagEQbTzFysHLv0rOOVi0E/t6WCBDyxWcBA9AtfURqBGDCAllXT2vdLns+xEIfkxcjPgsbKICL2rDGSVIzQCWNVPFsh5jyYSJA1L8LQp5tYhYqWeMkqRIAjTU0QnfXWogFdkFU3vkwwGQx44+e9WIWNKwTl1oBjvS16XGkRRCVdkDk3+2JABGPhREcew+oDbVlnbgyDoCGOTQyGuHUX1shPPouhEfegcMH8xjhB1vijLwNf/auE7OQw7urEqDe+YMBFPebEHRtguCdDaDcLmAE72yMf0e4X4fA72+IWaAXE9MXiDUajcbmEwCNjDSi0R585Q7mg99pAn/fy+B3vJS4BQHnanZNufEqKDfXwMgVs5gFI3l8jlhp85lngKaBc7lYeDXIV40gX8kF+bIepG59IoDc8yK7RqEI+boJnGfYLDSRB70cRFFByn8uaCT/XDOzIlKXDqRfV4B0KSf+mUAbv9ZN6DGgAUa7TWwWyIO2RQrBZgJJ6+85mvS2Hl8O88HRou39D0aewSDtJRCqAAAAAElFTkSuQmCC";  // Tray: Bianca + Triangolo
static const WCHAR icon_id6_b64[] = L"iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAARLSURBVFhHvZd/SJx1HMcf2fxRuttynid3mjk7XWLLVcoG7o/ixAVa/pFBgzCKMPr1hxQWJl00GxQIJZuEf2QQFEYkBbOkS7AoEtO2GW1e2rEs5+6cZ5qPP+/d+/Pc97HLtna3c77hxfN9Hp/7vN/P9/n+eNSoTPIYcSukLde2TJHm0fA0eZjcS4qJhcQlozCUzPMYOUJyyDXJKKL8kZ2dfUw1L6vJycmLw8PDZ7u6uvqbm5s/cjgcx8wapJrErJgChEIhrK6uYmlpCQsLC/D7/Yvt7e29SUlJr6paZVI0FsUVYG5uDrOzs/B4PCOqloypmLQpAWZmZmC1Wl9X9ZKlcLTatABmLZJKotb1CPA8ySJRaVMDNDQ0fGjWJDIr7GSjMsj2cHOTA3BWhFpbW0+ada/C/SR8ourHHWB6etpoj4+PX+T07KmsrDxuepi4XK62tLS019T59QkwPz9v3LO2tqZ++Y8CgcC08n2ObH2Ajo6OHuVbQ7Y2QDAY/NNms5nrhTFAtzRAS0vLx8rzQWLosgF0XV/s7e39vrGx8YPa2tp3qqurT5j3VlVVnaivr3+vs7PTw8EWiAxwaeQM9BY3psoPQC9wYDYnAxMHS7B49ChGPvt0TNV4kewkhv4ToK+vb1CO5t/+D6fT+UZdXd27P3Z/8sebO5K7cXchUOwADh8CDu4D7iwASouAjETMp6fgZU3rfiIz/RH+dl1GIeVvPEUEss/LR4e8KxsxJe084q6oqDj+VdtbZydvsYaQvxu4zQ7kWICiHKCQ7fwsII/XUzUDXdMwtl1b+Wm/c33XvFKAO8jV9Chxv23P6IF9J3A7n3xfLlCyRwoBWamA7YZw+y4ncE8pkKAhwPNRTXsqXOLKAaLSrUmJR1CSD6TRZD+NTZXtNZ52Xswfuk9dBFZKC+FPS8F4OITxORdXAF++owm5fPr0bcCBYmBpUVWiXngWeObxcFtd/4LmZxQ/52Y2SY24Alwo4lPnZwJ21dU5u/4dQqTOOarxLY2HyAj5ja9IasQVYMpC04xkYHcijynwW3dgRoJsCMHvNXSTczQ+TaQHThGpEVeACTGjeYjmQZr7yFzlIeiqnhlErzkc7nbiJWObFcCXx+633YhLNJ5y3AT9gQpVidr4Kp6sw680nSDnGdxr2RZ/gFEOJB8L/sIA+l5OQ5EyfoUm70sPRQapqUCQ9w/z+imnI/5BKFNpkMV+YNEphjDNuOJhgNeGePRFhOD9YgJ5dUMJ4WkoKdzLy8srcoO0FVHrNBcVGd1fEwnSxuJyLgNOutw0NY/cEPBdxEL0EnHL5nOtAc4VOMoGE7QVbnP4RgzZEzLVZMCdJxeUsfA7GUhPWRl1la8vxbLem6YmMf9z4S28ufhLLs39NOinqRzl1YjhX8RL+BkED+/xusplf1mXbIsSwugJEte/533cpBikyZuejM9pyDZ8WRbp8qaB8AYWIU37G1xt2pFGvWvBAAAAAElFTkSuQmCC";  // Tray: Bianca + X Rossa

static const WCHAR icon_shield_b64[] = L"iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAEmUlEQVR4nLWVW4iVVRTHf3t/l/Ody5y5OeOMSjg4XcSmSDOKppuUEBrhQ4KpMfVig4UQSg9CQw+BYBJpQQ5EL/miQT5Elwlk1KAwb1jqIF7Gca7OnMucc75zvvvuQUfEZhot+sOGxdprrR9r7Q0L7kEHDx6s2rnzp+S95Ii7Cfpu377aeGNTVyDMV0ytEhRKcv+FvuEd27a9Yf9nQM/3PU+aicTnk6ph6dVsiY5ne8nlGzl/ad7R0dHSOx0dr575V4D+vr6WolvZXHZFZ0WvS5wdKlOwc2xZ0UNcy5GZbOHcwKL8eC7+6cXBse733319eFZAdjT7iDTDZ+yy+1yuUH4xkIladItsxePKcJGCXWRT+yHiagLCAM+PM1y8n/6x+rFc0ejxnPBILjf+Q2dnx9BUTX3KUErJsbHMV8MT2tLhnMJ2DapSUB0P8fyQIFSEkUJhgbIg9DGjMgsTJ1mwMDW3rBo3Zr2Wjb+cSHwCvPc3QH9/r1lxW9Jnhg0ypSIpQ6CImCy6FCs+o/kyjuegQnGzcQkYEEl0r0Q6HEcpmzBqbbx9KrcAmUxRBqokHS9JwXbwdUmu6FJ2fWzXZ6LooEkPFQQgfHBK4JTBD8H1oVyCRANCSm1aAIDv+3h+hF3xmAwCCmWPouPh+hEjBQ9dOISDpyEaAs+HQECkQwh4Dsp0kHd8m1sA67ITlJpDLwgjHD/AcT0mig5D+QoD2QoTxYDm6hBVmoCYC1KCJkBqN48g0gyUkP7tADllPLx2raeiIKeUoOKD7SkujDucGCgxng9QoQSho5saxCQkDUjFIGnetE0iMwWIzIwjUqE/WJ+yqUvYlGwbS+RprancuEPSVKW4nLNIxRRCj6FrMYTQEUiECinojfheNDgjwPU59fLi46+te/BPosoc8LNQ7oegjBIR43nJyq9XUKSWdEInHTdJxWLoQkMqjQ1P1WAEZ8/OCMgV1KGxsVTU2BhIdAc0E0IdZAD4WELiKhNPWoS6iTItAs3A9hXNqQRp07t+uO/0yWnfAKD35/Mnx/M1xz3VAFEJlAFaGkQCVAwlTAxDIx43SCZN4nEDJQWVEB5bmMDA/nHXri+uzwjo7t7kZwrG7sHCIpAeRC7IJGhVoFkgJIZpYFkmVsxESI1KEFFjmSybH/ojV6/s5Q7JOx1He//45uJI3TFHuw9EBYQBWgI0EyElpqkTixlIXcMNIxxPsKqtlipZ+PatzVt+nRWwZ88Wd2Q83HZudLGn4nNuRAgTlEAIQSxmoBs6kQLbVSxpqqK9RWV+P328C1CzAgA6Nqw+cumasWPAXgrpuWBWgV6LtGpJpxPELQPQmJdM8fbzDZSy17Zv3bq9b7pa+nROgAP7f/vIij3dZra1rWmuHwQnhybKLGiqJ+/XkAgF6x9PklQjn72wek33THVmBhz40Fu2bO+but5qeg+0rpo/N4+pTbKkZT7Kj/NodQGjcuXLJ15auXW60Uxp1pXZ1dWVWL68/YOGeqOzvk5LD0zWosqTE5mR/l1r163/GAj+Kf+ulj7A7t17H0qnq9ujoBSdOnbs8J7u7kt3m/u/6i8VOR/oDCBHYgAAAABJRU5ErkJggg==";

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
static void* Base64ToGdipBitmap(const WCHAR* b64, IStream** retainedStream = NULL) {
    if (retainedStream) *retainedStream = NULL;
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

    if (bmp && retainedStream) {
        // Il chiamante rilascera' lo stream solo dopo GdipDisposeImage.
        *retainedStream = stream;
    } else {
        stream->Release();
    }
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
//  - System tray scales 32 -> 16 cleanly (same as before; tray look preserved).
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


// Returns TRUE if the running OS is older than Windows 10 21H2 (build 19044).
// Uses VerifyVersionInfoW so it is NOT affected by the compatibility shim that
// makes GetVersionExW report 6.2 on Windows 8.1 and later.
static BOOL IsSystemOlderThanWin1021H2(void) {
    OSVERSIONINFOEXW osvi = { sizeof(osvi) };
    osvi.dwMajorVersion = 10;
    osvi.dwMinorVersion = 0;
    osvi.dwBuildNumber  = 19044; // Windows 10 21H2

    DWORDLONG mask = 0;
    mask = VerSetConditionMask(mask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    mask = VerSetConditionMask(mask, VER_MINORVERSION, VER_GREATER_EQUAL);
    mask = VerSetConditionMask(mask, VER_BUILDNUMBER,  VER_GREATER_EQUAL);

    // VerifyVersionInfoW returns TRUE only if ALL conditions hold:
    // Major >= 10 AND Minor >= 0 AND Build >= 19044.
    if (VerifyVersionInfoW(&osvi,
            VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER, mask))
        return FALSE; // Windows 10 21H2 or newer (includes Windows 11)

    return TRUE; // older than Windows 10 21H2
}
void InitFlyoutIcons() {

    // Conserva i PNG originali delle bandiere ID 0, 1 e 2. Il flyout li
    // disegna direttamente tramite GDI+ con la stessa pipeline HQ, evitando
    // la rasterizzazione aggiuntiva introdotta dalla conversione in HICON.
    if (g_pBmpFlyoutGood && pGdipDisposeImage) {
        pGdipDisposeImage(g_pBmpFlyoutGood);
        g_pBmpFlyoutGood = NULL;
    }
    if (g_pStreamFlyoutGood) {
        g_pStreamFlyoutGood->Release();
        g_pStreamFlyoutGood = NULL;
    }
    if (g_pBmpFlyoutWarning && pGdipDisposeImage) {
        pGdipDisposeImage(g_pBmpFlyoutWarning);
        g_pBmpFlyoutWarning = NULL;
    }
    if (g_pStreamFlyoutWarning) {
        g_pStreamFlyoutWarning->Release();
        g_pStreamFlyoutWarning = NULL;
    }
    if (g_pBmpFlyoutAlert && pGdipDisposeImage) {
        pGdipDisposeImage(g_pBmpFlyoutAlert);
        g_pBmpFlyoutAlert = NULL;
    }
    if (g_pStreamFlyoutAlert) {
        g_pStreamFlyoutAlert->Release();
        g_pStreamFlyoutAlert = NULL;
    }

    g_pBmpFlyoutGood = Base64ToGdipBitmap(
        icon_id0_b64, &g_pStreamFlyoutGood);
    g_pBmpFlyoutWarning = Base64ToGdipBitmap(
        icon_id1_b64, &g_pStreamFlyoutWarning);
    g_pBmpFlyoutAlert = Base64ToGdipBitmap(
        icon_id2_b64, &g_pStreamFlyoutAlert);

    
    g_hFlyoutIconGood    = Base64ToIcon(icon_id0_b64);
    
    g_hFlyoutIconWarning = Base64ToIcon(icon_id1_b64);
    
    g_hFlyoutIconAlert   = Base64ToIcon(icon_id2_b64);
        // Choose the shield icon based on the OS version:
    //  - Older than Windows 10 21H2 (build 19044): use the embedded base64 shield.
    //  - Windows 10 21H2 or newer (incl. Windows 11): use the original system shield.
    if (IsSystemOlderThanWin1021H2()) {
        g_hShieldIcon = Base64ToIcon(icon_shield_b64);
    } else {
        g_hShieldIcon = NULL;
    }
    if (!g_hShieldIcon) {
        // Original system shield (imageres.dll / IDI_SHIELD / shell32.dll)
        ExtractIconExW(L"imageres.dll", 73, NULL, &g_hShieldIcon, 1);
        if (!g_hShieldIcon) {
            HICON hSharedShield = (HICON)LoadImageW(
                NULL, (LPCWSTR)32518, IMAGE_ICON, 16, 16, LR_SHARED); // IDI_SHIELD
            if (hSharedShield) g_hShieldIcon = CopyIcon(hSharedShield);
            if (!g_hShieldIcon) ExtractIconExW(L"shell32.dll", 77, NULL, &g_hShieldIcon, 1);
        }
    }
    
    // Fallback se Base64 decode fallisce
    if (!g_hFlyoutIconGood)
        g_hFlyoutIconGood = LoadActionCenterIcon(0);
    if (!g_hFlyoutIconWarning)
        g_hFlyoutIconWarning = LoadActionCenterIcon(1);
    if (!g_hFlyoutIconAlert)
        g_hFlyoutIconAlert = LoadActionCenterIcon(2);
}
void FreeAllIcons() {
    if (g_hProblemBalloonIcon) { DestroyIcon(g_hProblemBalloonIcon); g_hProblemBalloonIcon = NULL; }
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
        Wh_Log(L"OpenSCManagerW failed for %s: %lu", serviceName, GetLastError());
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
                    AddProblem(PROB_AUTOUPDATE, idx, criticalCount);
                    return;
                }
            }
        }
    }

    // 2) Service start type: Update Orchestrator (UsoSvc) and legacy WU (wuauserv).
    // Only SERVICE_DISABLED counts as "updates off"; stopped-but-auto is normal.
    if (IsServiceStartDisabled(L"UsoSvc") || IsServiceStartDisabled(L"wuauserv")) {
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
        case PROB_SMARTSCREEN: return LOC(STR_MSG_SMARTSCREEN);
        case PROB_BACKUP:      return LOC(STR_MSG_BACKUP);
        case PROB_WER:         return LOC(STR_MSG_WER);
        case PROB_DISK_HEALTH:      return LOC(STR_MSG_DISK_HEALTH);
        case PROB_BATTERY:          return LOC(STR_MSG_BATTERY);
        case PROB_UPDATE_PENDING:   return LOC(STR_MSG_UPDATE_PENDING);
        case PROB_RDP_NLA:         return LOC(STR_MSG_RDP_NLA);
        case PROB_BITLOCKER:       return LOC(STR_MSG_BITLOCKER);
        default: return L"";
    }
}

// ============================================================================
// SmartScreen Check
// ============================================================================
static void CheckSmartScreen(int* idx, int* criticalCount) {
    // Check Windows Defender SmartScreen status via registry
    // HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer -> SmartScreenEnabled
    RegKey hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer",
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        WCHAR szValue[32] = {0};
        DWORD dwSize = sizeof(szValue);
        DWORD dwType = 0;
        if (RegQueryValueExW(hKey, L"SmartScreenEnabled", NULL, &dwType, (LPBYTE)szValue, &dwSize) == ERROR_SUCCESS) {
            // Value can be "On" or "Off" (REG_SZ)
            if (dwType == REG_SZ && _wcsicmp(szValue, L"Off") == 0) {
                AddProblem(PROB_SMARTSCREEN, idx, criticalCount);
                return;
            }
        }
        // Also check DWORD variant (some Windows versions)
        DWORD dwVal = 1;
        dwSize = sizeof(DWORD);
        if (RegQueryValueExW(hKey, L"SmartScreenEnabled", NULL, &dwType, (LPBYTE)&dwVal, &dwSize) == ERROR_SUCCESS) {
            if (dwType == REG_DWORD && dwVal == 0) {
                AddProblem(PROB_SMARTSCREEN, idx, criticalCount);
                return;
            }
        }
    }

    // Edge/Defender SmartScreen: HKCU\SOFTWARE\Microsoft\Edge\SmartScreenEnabled
    RegKey hKeyEdge;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
        L"SOFTWARE\\Microsoft\\Edge",
        0, KEY_READ, &hKeyEdge) == ERROR_SUCCESS) {
        DWORD dwVal = 1, dwSize = sizeof(DWORD);
        if (RegQueryValueExW(hKeyEdge, L"SmartScreenEnabled", NULL, NULL, (LPBYTE)&dwVal, &dwSize) == ERROR_SUCCESS) {
            if (dwVal == 0) {
                AddProblem(PROB_SMARTSCREEN, idx, criticalCount);
            }
        }
    }
}

// ============================================================================
// Maintenance Checks (non-critical, warning-level)
// ============================================================================

// Check if system backup is configured and running
static void CheckBackupStatus(int* idx, int* criticalCount) {
    // Windows Backup service (SDRSVC) or wbengine
    SC_HANDLE hSCM = OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT);
    if (!hSCM) return;

    BOOL backupOk = FALSE;
    // Check the main Windows Backup service
    SC_HANDLE hSvc = OpenServiceW(hSCM, L"SDRSVC", SERVICE_QUERY_CONFIG);
    if (hSvc) {
        DWORD needed = 0;
        QueryServiceConfigW(hSvc, NULL, 0, &needed);
        if (needed > 0 && needed < 64 * 1024) {
            BYTE* buf = (BYTE*)malloc(needed);
            if (buf) {
                QUERY_SERVICE_CONFIGW* cfg = (QUERY_SERVICE_CONFIGW*)buf;
                if (QueryServiceConfigW(hSvc, cfg, needed, &needed)) {
                    // If service is disabled, backup is not configured
                    if (cfg->dwStartType == SERVICE_DISABLED) {
                        backupOk = FALSE;
                    } else {
                        backupOk = TRUE; // service exists and is not disabled
                    }
                }
                free(buf);
            }
        }
        CloseServiceHandle(hSvc);
    }

    if (!backupOk) {
        // Also check wbengine (Windows Backup Engine)
        hSvc = OpenServiceW(hSCM, L"wbengine", SERVICE_QUERY_CONFIG);
        if (hSvc) {
            DWORD needed = 0;
            QueryServiceConfigW(hSvc, NULL, 0, &needed);
            if (needed > 0 && needed < 64 * 1024) {
                BYTE* buf = (BYTE*)malloc(needed);
                if (buf) {
                    QUERY_SERVICE_CONFIGW* cfg = (QUERY_SERVICE_CONFIGW*)buf;
                    if (QueryServiceConfigW(hSvc, cfg, needed, &needed)) {
                        if (cfg->dwStartType != SERVICE_DISABLED) {
                            backupOk = TRUE;
                        }
                    }
                    free(buf);
                }
            }
            CloseServiceHandle(hSvc);
        }
    }

    CloseServiceHandle(hSCM);

    if (!backupOk) {
        AddProblem(PROB_BACKUP, idx, criticalCount);
    }
}

// Check Windows Error Reporting service status
static void CheckWerStatus(int* idx, int* criticalCount) {
    SC_HANDLE hSCM = OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT);
    if (!hSCM) return;

    SC_HANDLE hSvc = OpenServiceW(hSCM, L"WerSvc", SERVICE_QUERY_CONFIG);
    if (hSvc) {
        DWORD needed = 0;
        QueryServiceConfigW(hSvc, NULL, 0, &needed);
        if (needed > 0 && needed < 64 * 1024) {
            BYTE* buf = (BYTE*)malloc(needed);
            if (buf) {
                QUERY_SERVICE_CONFIGW* cfg = (QUERY_SERVICE_CONFIGW*)buf;
                if (QueryServiceConfigW(hSvc, cfg, needed, &needed)) {
                    if (cfg->dwStartType == SERVICE_DISABLED) {
                        AddProblem(PROB_WER, idx, criticalCount);
                    }
                }
                free(buf);
            }
        }
        CloseServiceHandle(hSvc);
    }
    CloseServiceHandle(hSCM);
}

// Battery check: warns when on battery power and charge is critically low.
// Only fires on laptops (ACLineStatus == 0); desktops and VMs are skipped
// (BATTERY_FLAG_NO_SYSTEM_BATTERY). Thresholds: <=10% = critical (STATE_ALERT),
// <=20% = warning (STATE_WARNING). Does not count as a criticalCount hit to
// avoid overriding genuine security alerts.
static void CheckBatteryStatus(int* idx, int* criticalCount) {
    SYSTEM_POWER_STATUS sps;
    if (!GetSystemPowerStatus(&sps)) return;
    // Skip desktops, VMs without battery, or unknown state
    if (sps.ACLineStatus == 255) return;           // unknown
    if (sps.BatteryFlag == 128) return;            // no battery
    if (sps.BatteryFlag == 255) return;            // unknown flag
    if (sps.ACLineStatus == 1) return;             // plugged in - all good
    // On battery
    BYTE pct = sps.BatteryLifePercent;
    if (pct == 255) return;                        // unknown percentage
    if (pct <= 20) {
        AddProblem(PROB_BATTERY, idx, criticalCount);
    }
}

// Windows Update pending-reboot check.
// Checks the two well-known registry keys that Windows sets when a reboot
// is required to finish installing updates. This is maintenance-level
// (warning only) and never bumps criticalCount.
static void CheckWindowsUpdatePending(int* idx, int* criticalCount) {
    // Key 1: CBS / component-based servicing reboot pending
    {
        HKEY hKey = NULL;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Component Based Servicing\\RebootPending",
                0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            AddProblem(PROB_UPDATE_PENDING, idx, criticalCount);
            return;
        }
    }
    // Key 2: Windows Update reboot required
    {
        HKEY hKey = NULL;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\RebootRequired",
                0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            AddProblem(PROB_UPDATE_PENDING, idx, criticalCount);
            return;
        }
    }

}

// Remote Desktop enabled but without Network Level Authentication: a real
// network-exposure risk, readable from the registry without elevation.
// This is treated as critical, same weight as firewall/antivirus/updates.
static void CheckRdpNla(int* idx, int* criticalCount) {
    RegKey hKeyRdp;
    DWORD dwDenyConnections = 1, dwSize = sizeof(DWORD);
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
        L"SYSTEM\\CurrentControlSet\\Control\\Terminal Server",
        0, KEY_READ, &hKeyRdp) != ERROR_SUCCESS) return;
    if (RegQueryValueExW(hKeyRdp, L"fDenyTSConnections", NULL, NULL,
        (LPBYTE)&dwDenyConnections, &dwSize) != ERROR_SUCCESS) return;
    if (dwDenyConnections != 0) return; // RDP disabled: nothing to flag

    RegKey hKeyWinstations;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
        L"SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\WinStations\\RDP-Tcp",
        0, KEY_READ, &hKeyWinstations) == ERROR_SUCCESS) {
        DWORD dwNla = 1;
        dwSize = sizeof(DWORD);
        if (RegQueryValueExW(hKeyWinstations, L"UserAuthentication", NULL, NULL,
                             (LPBYTE)&dwNla, &dwSize) == ERROR_SUCCESS && dwNla == 0) {
            AddProblem(PROB_RDP_NLA, idx, criticalCount);
            // RDP without NLA is a direct network attack surface: bump the
            // critical count even though AddProblem already counts it once
            // for the well-known types (firewall/autoupdate/antivirus).
            (*criticalCount)++;
        }
    }
}

// BitLocker check on the system drive via the shell property
// System.Volume.BitLockerProtection (SHCreateItemFromParsingName +
// IShellItem2::GetProperty). This is what Explorer itself uses for the drive
// padlock overlays and is queryable unelevated, unlike WMI
// Win32_EncryptableVolume which requires admin rights.
// Result is cached (10 min) to avoid hitting the property store on every
// refresh tick (tray UI thread).
static void CheckBitLocker(int* idx, int* criticalCount) {
    // Cache: encryption status doesn't change minute-to-minute
    static DWORD s_lastTick = 0;
    static bool s_hasCache = false;
    static bool s_isUnprotected = false;
    static bool s_checkedOnce = false;

    DWORD now = GetTickCount();
    const DWORD kCacheMs = 10 * 60 * 1000; // 10 minutes

    if (s_hasCache && s_checkedOnce && (now - s_lastTick < kCacheMs)) {
        if (s_isUnprotected) {
            AddProblem(PROB_BITLOCKER, idx, criticalCount);
        }
        return;
    }

    bool unprotected = false;
    bool gotResult = false;

    // Get system drive, e.g. "C:"
    WCHAR sysDrive[8] = {0};
    if (!GetEnvironmentVariableW(L"SystemDrive", sysDrive, ARRAYSIZE(sysDrive))) {
        // Fallback to C:
        StringCchCopyW(sysDrive, ARRAYSIZE(sysDrive), L"C:");
    }
    // Ensure trailing backslash for SHCreateItemFromParsingName
    WCHAR parsingName[MAX_PATH] = {0};
    StringCchCopyW(parsingName, ARRAYSIZE(parsingName), sysDrive);
    size_t len = wcslen(parsingName);
    if (len > 0 && parsingName[len-1] != L'\\') {
        if (len + 1 < ARRAYSIZE(parsingName)) {
            parsingName[len] = L'\\';
            parsingName[len+1] = L'\0';
        }
    }

    BOOL didCoInit = FALSE;
    HRESULT hrCo = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(hrCo) || hrCo == S_FALSE || hrCo == RPC_E_CHANGED_MODE) {
        didCoInit = SUCCEEDED(hrCo);
    }

    IShellItem2* pItem = NULL;
    HRESULT hr = SHCreateItemFromParsingName(parsingName, NULL, IID_PPV_ARGS(&pItem));
    if (SUCCEEDED(hr) && pItem) {
        PROPERTYKEY pk = {{0}};
        hr = PSGetPropertyKeyFromName(L"System.Volume.BitLockerProtection", &pk);
        if (SUCCEEDED(hr)) {
            PROPVARIANT var;
            PropVariantInit(&var);
            hr = pItem->GetProperty(pk, &var);
            if (SUCCEEDED(hr)) {
                if (var.vt == VT_I4 || var.vt == VT_UI4 || var.vt == VT_INT) {
                    int status = 0;
                    if (var.vt == VT_I4) status = var.lVal;
                    else if (var.vt == VT_UI4) status = (int)var.ulVal;
                    else if (var.vt == VT_INT) status = var.intVal;
                    else status = var.intVal;
                    gotResult = true;
                    if (!(status == 1 || status == 3 || status == 5)) {
                        if (status == 0) {
                            unprotected = true;
                        }
                    }
                } else if (var.vt == VT_EMPTY || var.vt == VT_NULL) {
                } else {
                    int status = var.intVal;
                    gotResult = true;
                    if (status == 0) unprotected = true;
                }
            }
            PropVariantClear(&var);
        }
        pItem->Release();
    }

    if (didCoInit) CoUninitialize();

    if (gotResult || !s_hasCache) {
        s_isUnprotected = unprotected;
        s_hasCache = true;
        s_lastTick = now;
        s_checkedOnce = true;
    }

    if (unprotected) {
        AddProblem(PROB_BITLOCKER, idx, criticalCount);
    }
}


// Best-effort disk health check via IOCTL_STORAGE_PREDICT_FAILURE (SMART).
// Deliberately kept ultra-simple: we ask each physical drive whether it is
// predicting its own failure. Running inside explorer.exe (no elevation), the
// IOCTL frequently returns ERROR_ACCESS_DENIED / is unsupported; in that case
// we simply skip the drive and report nothing. This never produces a false
// positive - it only flags a disk when Windows itself reports a predicted
// failure. See the README ("best-effort") note.
static void CheckDiskHealth(int* idx, int* criticalCount) {
    BOOL diskIssue = FALSE;

    // Probe a handful of physical drives (\\.\PhysicalDrive0..7).
    for (int drive = 0; drive < 8 && !diskIssue; drive++) {
        WCHAR path[64];
        StringCchPrintfW(path, ARRAYSIZE(path), L"\\\\.\\PhysicalDrive%d", drive);

        HANDLE hDisk = CreateFileW(path, 0,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
            OPEN_EXISTING, 0, NULL);
        if (hDisk == INVALID_HANDLE_VALUE) {
            // No such drive (or access denied): stop scanning higher indices
            // only when there is clearly no drive there.
            if (GetLastError() == ERROR_FILE_NOT_FOUND) {
                break;
            }
            continue;
        }

        STORAGE_PREDICT_FAILURE predict = {0};
        DWORD bytes = 0;
        if (DeviceIoControl(hDisk, IOCTL_STORAGE_PREDICT_FAILURE,
                NULL, 0, &predict, sizeof(predict), &bytes, NULL)) {
            if (predict.PredictFailure != 0) {
                diskIssue = TRUE;
            }
        }
        CloseHandle(hDisk);
    }

    if (diskIssue) {
        AddProblem(PROB_DISK_HEALTH, idx, criticalCount);
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
    CheckSmartScreen(&idx, &criticalCount);
    // Maintenance checks (non-critical, warning-level)
    CheckBackupStatus(&idx, &criticalCount);
    CheckWerStatus(&idx, &criticalCount);
    CheckDiskHealth(&idx, &criticalCount);
    CheckBatteryStatus(&idx, &criticalCount);
    CheckWindowsUpdatePending(&idx, &criticalCount);
    CheckRdpNla(&idx, &criticalCount);
    CheckBitLocker(&idx, &criticalCount);
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
                    } else if (wcsstr(szIdentity, L"smartscreen") ||
                               wcsstr(szIdentity, L"appcontrol")) {
                        mappedType = PROB_SMARTSCREEN;
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
    int prevProblemTypes[MAX_PROBLEMS];
    { SRWGuard g(g_Ctx.srwLock, false); 
        prevState = g_SecurityState; 
        prevProblems = g_ActiveProblems;
        memcpy(prevProblemTypes, g_ProblemTypes, sizeof(g_ProblemTypes));
    }
    CheckSecurityProviders();
    int newState;
    int newProblems;
    int newProblemTypes[MAX_PROBLEMS];
    { SRWGuard g(g_Ctx.srwLock, false); 
        newState = g_SecurityState; 
        newProblems = g_ActiveProblems;
        memcpy(newProblemTypes, g_ProblemTypes, sizeof(g_ProblemTypes));
    }
    
    // Se il numero di problemi è cambiato, aggiorna l'altezza del flyout
    if (prevProblems != newProblems && g_Ctx.hWndFlyout && IsWindow(g_Ctx.hWndFlyout) && IsWindowVisible(g_Ctx.hWndFlyout)) {
        int newHeight = CalculateFlyoutHeight(newProblems);
        RecalcDpiMetrics(g_dpi, newProblems);
        if ((newHeight > g_ScaledHeight ? newHeight - g_ScaledHeight : g_ScaledHeight - newHeight) > 1) {
            SetWindowPos(g_Ctx.hWndFlyout, NULL, 0, 0, g_ScaledWidth, newHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
            g_ScaledHeight = newHeight;
        }
        InvalidateRect(g_Ctx.hWndFlyout, NULL, TRUE);
    }
    
    // Detect if problems actually changed (not just state level)
    BOOL problemsChanged = FALSE;
    if (prevProblems != newProblems) {
        problemsChanged = TRUE;
    } else {
        // Compare problem types even if count is the same
        for (int i = 0; i < newProblems && i < MAX_PROBLEMS; i++) {
            if (newProblemTypes[i] != prevProblemTypes[i]) {
                problemsChanged = TRUE;
                break;
            }
        }
    }
    
    if ((prevState != newState || problemsChanged) && !g_Ctx.isUninitializing) {
        // Aggiorna icona tray quando lo stato O i problemi cambiano
        UpdateTrayIcon();
        EnsureTrayTooltip();
        
        // Show balloon when state worsens OR when new problems appear
        if (newState > prevState || (newProblems > prevProblems && newState > STATE_GOOD)) {
            ShowProblemBalloon();
        } else if (newState < prevState) {
            RemoveProblemBalloon();
        }
        
        if (g_Ctx.hWndFlyout && IsWindow(g_Ctx.hWndFlyout) && IsWindowVisible(g_Ctx.hWndFlyout)) {
            InvalidateRect(g_Ctx.hWndFlyout, NULL, TRUE);
        }
    }

    // Startup notification: on the very first check after mod load, show a
    // balloon unconditionally if problems exist, regardless of cooldown.
    // This guarantees the user is always notified at every Windows session start.
    if (g_isStartupCheck && !g_Ctx.isUninitializing) {
        g_isStartupCheck = FALSE;
        if (newState > STATE_GOOD && newProblems > 0) {
            // Clear cooldown so ShowProblemBalloon doesn't suppress the startup alert.
            g_LastProblemBalloonSignature = 0;
            g_LastProblemBalloonTick = 0;
            ShowProblemBalloon();
        }
    }
}



static DWORD ComputeProblemBalloonSignature(
        int secState, int activeProblems, const int* problemTypes) {
    // FNV-1a: sufficiente per riconoscere rapidamente notifiche identiche.
    DWORD hash = 2166136261u;
    hash ^= (DWORD)secState; hash *= 16777619u;
    hash ^= (DWORD)activeProblems; hash *= 16777619u;
    for (int i = 0; i < activeProblems && i < MAX_PROBLEMS; i++) {
        hash ^= (DWORD)problemTypes[i];
        hash *= 16777619u;
    }
    return hash;
}

// A problem counts as "critical" for balloon-wording purposes if it's one of
// the well-known high-impact categories (matches the weighting AddProblem/
// CheckSecurityProviders already use for firewall/autoupdate/antivirus, plus
// the new RDP-without-NLA check).
static BOOL IsCriticalProblemType(int problemType) {
    return problemType == PROB_FIREWALL || problemType == PROB_AUTOUPDATE ||
           problemType == PROB_ANTIVIRUS || problemType == PROB_RDP_NLA;
}

static void BuildProblemBalloonText(
        WCHAR* text, size_t textCount,
        int activeProblems, const int* problemTypes,
        BOOL isNewSinceLastBalloon) {
    if (!text || textCount == 0) return;
    text[0] = L'\0';
    if (activeProblems <= 0) {
        StringCchCopyW(text, textCount, LOC(STR_NOTIFY_PROBLEM));
        return;
    }

    int criticalShown = 0;
    for (int i = 0; i < activeProblems && i < MAX_PROBLEMS; i++) {
        if (IsCriticalProblemType(problemTypes[i])) criticalShown++;
    }

    // List up to 3 concrete problems (not just the first one) so the balloon
    // actually says what's wrong instead of a generic "N issues detected".
    const int kMaxShown = 3;
    int shown = (activeProblems < kMaxShown) ? activeProblems : kMaxShown;
    WCHAR body[512] = {0};
    for (int i = 0; i < shown; i++) {
        const WCHAR* line = GetProblemText(problemTypes[i]);
        if (!line || !line[0]) continue;
        if (body[0]) StringCchCatW(body, ARRAYSIZE(body), L"\n");
        StringCchCatW(body, ARRAYSIZE(body), line);
    }
    if (!body[0]) {
        // Defensive fallback: none of the mapped strings resolved (shouldn't
        // normally happen), keep the balloon meaningful instead of blank.
        StringCchCopyW(body, ARRAYSIZE(body), LOC(STR_NOTIFY_PROBLEM));
    }
    if (activeProblems > shown) {
        StringCchCatW(body, ARRAYSIZE(body), L"\n");
        StringCchCatW(body, ARRAYSIZE(body), LOC(STR_AND_MORE));
    }

    // Contextual closing line: brand-new problem vs. one that was already
    // showing at the last balloon, and whether anything critical is involved,
    // instead of always the same generic call-to-action.
    const WCHAR* tail;
    if (isNewSinceLastBalloon && criticalShown > 0)  tail = LOC(STR_NOTIFY_ACTION_NEW_CRITICAL);
    else if (isNewSinceLastBalloon)                   tail = LOC(STR_NOTIFY_ACTION_NEW);
    else if (criticalShown > 0)                       tail = LOC(STR_NOTIFY_ACTION_CRITICAL);
    else                                               tail = LOC(STR_NOTIFY_ACTION);

    StringCchPrintfW(text, textCount, L"%s\n%s", body, tail);
}

// Rilascia soltanto le risorse locali. Usata quando Windows comunica che il
// balloon e' gia' scomparso, quindi non serve un ulteriore NIM_MODIFY.
static void ReleaseProblemBalloonResources(void) {
    HWND hMsg = g_Ctx.hWndMsgHandler;
    if (hMsg && IsWindow(hMsg))
        KillTimer(hMsg, PROBLEM_BALLOON_TIMER_ID);

    g_ProblemBalloonShowing = FALSE;
    if (g_hProblemBalloonIcon) {
        DestroyIcon(g_hProblemBalloonIcon);
        g_hProblemBalloonIcon = NULL;
    }
}

static void RemoveProblemBalloon(void) {
    HWND hMsg = g_Ctx.hWndMsgHandler;
    if (hMsg && IsWindow(hMsg)) {
        KillTimer(hMsg, PROBLEM_BALLOON_TIMER_ID);

        // Chiede prima alla shell di chiudere il balloon, mantenendo valida
        // l'icona personalizzata fino al completamento di NIM_MODIFY.
        if (g_Ctx.trayIconAdded) {
            NOTIFYICONDATAW nid = g_nid;
            nid.cbSize = sizeof(NOTIFYICONDATAW);
            nid.uFlags |= NIF_INFO;
            nid.szInfo[0] = L'\0';
            nid.szInfoTitle[0] = L'\0';
            nid.dwInfoFlags = NIIF_NONE;
            nid.hBalloonIcon = NULL;
            Shell_NotifyIconW(NIM_MODIFY, &nid);
        }
    }

    ReleaseProblemBalloonResources();
}

void ShowProblemBalloon(void) {
    if (!g_Ctx.hWndMsgHandler || !IsWindow(g_Ctx.hWndMsgHandler)) return;
    if (!g_Ctx.trayIconAdded || g_Ctx.isUninitializing) return;

    // Snapshot coerente dello stato e dei problemi.
    int secState, activeProblems, problemTypes[MAX_PROBLEMS];
    {
        SRWGuard guard(g_Ctx.srwLock, false);
        secState = g_SecurityState;
        activeProblems = g_ActiveProblems;
        memcpy(problemTypes, g_ProblemTypes, sizeof(problemTypes));
    }
    if (secState <= STATE_GOOD || activeProblems <= 0) return;

    // Non accodare ripetutamente lo stesso avviso. NIF_REALTIME impedisce
    // inoltre a Windows di mostrare molto piu' tardi un balloon ormai vecchio.
    DWORD signature = ComputeProblemBalloonSignature(
        secState, activeProblems, problemTypes);
    DWORD now = GetTickCount();
    if (signature == g_LastProblemBalloonSignature &&
        secState == g_LastProblemBalloonState &&
        now - g_LastProblemBalloonTick < PROBLEM_BALLOON_COOLDOWN_MS) {
        return;
    }

    if (g_ProblemBalloonShowing || g_hProblemBalloonIcon)
        RemoveProblemBalloon();

    // Stessa bandiera del flyout: ID 1 per avviso, ID 2 per problema critico.
    HICON sourceIcon = g_hFlyoutIconGood;
    const WCHAR* sourceB64 = icon_id0_b64;
    if (secState >= STATE_ALERT) {
        sourceIcon = g_hFlyoutIconAlert;
        sourceB64 = icon_id2_b64;
    } else if (secState >= STATE_WARNING) {
        sourceIcon = g_hFlyoutIconWarning;
        sourceB64 = icon_id1_b64;
    }

    if (sourceIcon)
        g_hProblemBalloonIcon = CopyIcon(sourceIcon);
    if (!g_hProblemBalloonIcon)
        g_hProblemBalloonIcon = Base64ToIcon(sourceB64);

    NOTIFYICONDATAW nid = g_nid;
    nid.cbSize = sizeof(NOTIFYICONDATAW);
    nid.uFlags |= NIF_INFO | NIF_REALTIME;
    nid.uTimeout = 10000; // Windows recenti possono scegliere autonomamente.

    if (g_hProblemBalloonIcon) {
        nid.hBalloonIcon = g_hProblemBalloonIcon;
        nid.dwInfoFlags = NIIF_USER | NIIF_LARGE_ICON |
                          NIIF_RESPECT_QUIET_TIME;
    } else {
        nid.hBalloonIcon = g_nid.hIcon;
        nid.dwInfoFlags = NIIF_USER | NIIF_LARGE_ICON |
                          NIIF_RESPECT_QUIET_TIME;
    }

    // "New" means the exact set of active problems differs from whatever the
    // last balloon showed - covers both a brand-new issue appearing and a
    // previously-resolved one reappearing, not just a raw count change.
    BOOL isNewSinceLastBalloon = (signature != g_LastProblemBalloonSignature);

    StringCchCopyW(nid.szInfoTitle, ARRAYSIZE(nid.szInfoTitle),
                   LOC(STR_NOTIFY_TITLE));
    BuildProblemBalloonText(nid.szInfo, ARRAYSIZE(nid.szInfo),
                            activeProblems, problemTypes, isNewSinceLastBalloon);

    if (Shell_NotifyIconW(NIM_MODIFY, &nid)) {
        g_ProblemBalloonShowing = TRUE;
        g_LastProblemBalloonTick = now;
        g_LastProblemBalloonSignature = signature;
        g_LastProblemBalloonState = secState;

        // Fallback di sicurezza: normalmente Windows invia NIN_BALLOONHIDE o
        // NIN_BALLOONTIMEOUT e le risorse vengono rilasciate prima.
        SetTimer(g_Ctx.hWndMsgHandler, PROBLEM_BALLOON_TIMER_ID,
                 PROBLEM_BALLOON_FALLBACK_MS, NULL);
    } else {

        ReleaseProblemBalloonResources();
    }
}


// ============================================================================
// WSC Notifications
// ============================================================================
DWORD WINAPI WscChangeCallback(LPVOID lpParam) {
    InterlockedIncrement(&g_WscCallbacksInFlight);

    // Incrementiamo prima del controllo: il cleanup puo' attendere anche una
    // callback entrata nello stesso istante dell'unregister.
    if (!InterlockedCompareExchange(&g_Ctx.isUninitializing, 0, 0)) {
        HWND hMsg = g_Ctx.hWndMsgHandler;
        if (hMsg && IsWindow(hMsg))
            PostMessageW(hMsg, WM_SECURITY_CHANGED, 0, 0);
    }

    InterlockedDecrement(&g_WscCallbacksInFlight);
    return 0;
}

void RegisterWscNotifications() {
    HANDLE hLateRegistration = NULL;
    HRESULT hr = E_ABORT;

    {
        // Serializza la registrazione con Wh_ModUninit. L'handle viene prima
        // creato localmente, evitando che l'unregister possa perderlo.
        SRWGuard guard(g_Ctx.srwLock, true);
        if (g_Ctx.isUninitializing || g_Ctx.hWscRegistration) return;

        HANDLE hNew = NULL;
        hr = WscRegisterForChanges(NULL, &hNew, WscChangeCallback, NULL);
        if (SUCCEEDED(hr) && hNew) {
            if (g_Ctx.isUninitializing)
                hLateRegistration = hNew;
            else
                g_Ctx.hWscRegistration = hNew;
        }
    }

    if (hLateRegistration) {
        WscUnRegisterChanges(hLateRegistration);
        return;
    }
    if (FAILED(hr))
        Wh_Log(L"WscRegisterForChanges failed: 0x%08X", hr);
}

void UnregisterWscNotifications() {
    HANDLE hReg = NULL;
    {
        SRWGuard guard(g_Ctx.srwLock, true);
        hReg = g_Ctx.hWscRegistration;
        g_Ctx.hWscRegistration = NULL;
    }
    if (hReg) {
        WscUnRegisterChanges(hReg);
    }
}

static void WaitForWscCallbacksToDrain(void) {
    DWORD start = GetTickCount();
    BOOL logged = FALSE;
    while (InterlockedCompareExchange(&g_WscCallbacksInFlight, 0, 0) != 0) {
        if (!logged && GetTickCount() - start >= 2000) {
            logged = TRUE;
        }
        Sleep(1);
    }
}

// ============================================================================
// Registry Monitor Thread
// ============================================================================
DWORD WINAPI RegistryMonitorThread(LPVOID lpParam) {
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
        // Key is available again - reset backoff for future disappearances.
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
    return 0;
}
void StartRegistryMonitor() {
    // Impedisce una creazione tardiva dopo StopRegistryMonitor durante reload.
    SRWGuard lifecycleGuard(g_Ctx.srwLock, true);
    if (g_Ctx.isUninitializing || g_Ctx.hRegMonitorThread) return;
    g_Ctx.hRegShutdownEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    g_Ctx.hRegChangeEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (!g_Ctx.hRegShutdownEvent || !g_Ctx.hRegChangeEvent) {
        Wh_Log(L"Failed to create registry monitor events"); return;
    }
    g_Ctx.hRegMonitorThread = CreateThread(NULL, 0, RegistryMonitorThread, NULL, 0, NULL);
    if (!g_Ctx.hRegMonitorThread) {
        Wh_Log(L"Failed to create registry monitor thread");
        CloseHandle(g_Ctx.hRegShutdownEvent); g_Ctx.hRegShutdownEvent = NULL;
        CloseHandle(g_Ctx.hRegChangeEvent); g_Ctx.hRegChangeEvent = NULL;
    }
}
void StopRegistryMonitor() {
    // Si accoppia al lock di StartRegistryMonitor: gli handle sono ormai
    // completamente pubblicati oppure la creazione e' stata saltata.
    SRWGuard lifecycleGuard(g_Ctx.srwLock, true);
    if (g_Ctx.hRegShutdownEvent) SetEvent(g_Ctx.hRegShutdownEvent);
    if (g_Ctx.hRegMonitorThread) {
        DWORD wr = WaitForSingleObject(g_Ctx.hRegMonitorThread, 5000);
        if (wr == WAIT_TIMEOUT) {
            // Non chiudere eventi/handle mentre il thread li usa: durante
            // l'unload sarebbe un use-after-close. Il thread è event-driven e
            // deve terminare prima che il codice del mod venga scaricato.
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
        // Localized "something is wrong" message (same as the problem balloon).
        StringCchPrintfW(buf, bufSize, L"%s - %s", title, LOC(STR_NOTIFY_PROBLEM));
    }
}

static BOOL PublishTrayIcon(BOOL preferAdd) {
    if (g_Ctx.isUninitializing || !g_Ctx.hWndMsgHandler ||
        !IsWindow(g_Ctx.hWndMsgHandler)) return FALSE;

    // Shell_NotifyIcon fails until the notification area exists.
    if (!FindWindowW(L"Shell_TrayWnd", NULL)) {
        Wh_Log(L"PublishTrayIcon: Shell_TrayWnd not ready");
        return FALSE;
    }

    int secState;
    { SRWGuard g(g_Ctx.srwLock, false); secState = g_SecurityState; }

    HICON hNewIcon = LoadTrayIcon(secState >= STATE_ALERT);
    if (!hNewIcon) {
        Wh_Log(L"Unable to create tray icon");
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
                Wh_Log(L"Tray icon added without GUID (fallback)");
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
        Wh_Log(L"Shell_NotifyIcon failed: %lu", GetLastError());
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
            if (attempt == 1 || (attempt % 5) == 0)
            Wh_Log(L"Tray not ready yet (attempt %u), retry in %ums", attempt, delay);
        } else {
            Wh_Log(L"Tray icon recovery paused (no Shell_TrayWnd) after %u attempts", attempt);
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
        Wh_Log(L"Tray icon available after taskbar restart");
        KillTimer(g_Ctx.hWndMsgHandler, TRAY_HEALTH_TIMER_ID);
        SetTimer(g_Ctx.hWndMsgHandler, TRAY_HEALTH_TIMER_ID, 15000, NULL);
        return;
    }

    UINT attempt = ++g_Ctx.trayRetryAttempt;
    if (attempt >= 40) {
        Wh_Log(L"Tray icon recovery paused after %u attempts", attempt);
        return; // Il timer di health riproverà più tardi.
    }

    // Backoff esponenziale
    UINT delay = 150u << (attempt < 6 ? attempt : 6);
    if (delay > 8000) delay = 8000;
    SetTimer(g_Ctx.hWndMsgHandler, TRAY_RETRY_TIMER_ID, delay, NULL);

    if (attempt == 1 || (attempt % 5) == 0)
        Wh_Log(L"Retry attempt %u scheduled in %ums", attempt, delay);
}

// ============================================================================
// Notifications
// ============================================================================
void ClearNotifications() {
    RemoveProblemBalloon();
    g_LastProblemBalloonTick = 0;
    g_LastProblemBalloonSignature = 0;
    g_LastProblemBalloonState = STATE_GOOD;

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
    
    // Close notify popup if showing
    if (g_Ctx.hWndNotify && IsWindow(g_Ctx.hWndNotify) && g_NotifyShowing) {
        ShowWindow(g_Ctx.hWndNotify, SW_HIDE);
        KillTimer(g_Ctx.hWndNotify, NOTIFY_TIMER_ID);
        g_NotifyShowing = FALSE;
    }
    
    // Show balloon notification for the simulated problem
    ShowProblemBalloon();
    
    // Update flyout if open
    if (g_Ctx.hWndFlyout && IsWindow(g_Ctx.hWndFlyout) && IsWindowVisible(g_Ctx.hWndFlyout) && !g_Ctx.isUninitializing) {
        InvalidateRect(g_Ctx.hWndFlyout, NULL, TRUE);
    }
}

// ============================================================================
// Window Positioning
// ============================================================================

/* Restituisce il bordo della taskbar (ABE_BOTTOM/TOP/LEFT/RIGHT) tramite
   SHAppBarMessage. Se fallisce, cade su ABE_BOTTOM come default sicuro.
   outRect (opzionale) riceve il RECT fisico della taskbar.
   Risolto: ora restituisce anche l'hMonitor per supporto multimonitor corretto. */
static UINT GetTaskbarEdge(RECT* outRect = nullptr, HMONITOR* outMonitor = nullptr) {
    APPBARDATA abd = {};
    abd.cbSize = sizeof(APPBARDATA);
    abd.hWnd = FindWindowW(L"Shell_TrayWnd", NULL);
    if (abd.hWnd) {
        // Ottieni l'hMonitor dalla finestra della taskbar (non dalla finestra del flyout)
        if (outMonitor) {
            *outMonitor = MonitorFromWindow(abd.hWnd, MONITOR_DEFAULTTONEAREST);
        }
        if (SHAppBarMessage(ABM_GETTASKBARPOS, &abd)) {
            if (outRect) *outRect = abd.rc;
            Wh_Log(L"GetTaskbarEdge: edge=%u taskbarRect={%d,%d,%d,%d}",
                   abd.uEdge, abd.rc.left, abd.rc.top, abd.rc.right, abd.rc.bottom);
            return abd.uEdge;
        }
    }
    Wh_Log(L"GetTaskbarEdge: SHAppBarMessage failed, defaulting to ABE_BOTTOM");
    
    // Fallback: usa il monitor primario per multimonitor
    if (outMonitor) {
        *outMonitor = MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY);
    }
    if (outRect) SystemParametersInfoW(SPI_GETWORKAREA, 0, outRect, 0);
    return ABE_BOTTOM;
}

/* Ottiene l'area di lavoro (work area) dal monitor specificato, non da quello 
   del processo chiamante. Fondamentale per il supporto multimonitor. */
static BOOL GetWorkAreaFromMonitor(HMONITOR hMonitor, RECT* outWorkArea) {
    if (!hMonitor || !outWorkArea) return FALSE;
    
    MONITORINFO mi = { sizeof(MONITORINFO) };
    if (GetMonitorInfoW(hMonitor, &mi)) {
        *outWorkArea = mi.rcWork;
        Wh_Log(L"GetWorkAreaFromMonitor: monitor work area={%d,%d,%d,%d}",
               outWorkArea->left, outWorkArea->top, outWorkArea->right, outWorkArea->bottom);
        return TRUE;
    }
    
    // Fallback a SystemParametersInfo
    SystemParametersInfoW(SPI_GETWORKAREA, 0, outWorkArea, 0);
    return FALSE;
}

void PositionWindowNearTray(HWND hwnd) {
    // Rileva il DPI effettivo della finestra (non il globale g_dpi che potrebbe
    // essere stantio) e ricalcola le metriche prima di posizionare.
    // Fondamentale a DPI non standard (es. 150% su Win11 25H2 + ExplorerPatcher).
    UINT dpi = GetWindowDpi(hwnd);
    int activeProblems;
    { SRWGuard guard(g_Ctx.srwLock, false); activeProblems = g_ActiveProblems; }
    RecalcDpiMetrics(dpi, activeProblems);

    // Determina il bordo della taskbar e il monitor corretto per supporto multimonitor.
    // CRITICO: usiamo il monitor della taskbar, non quello della finestra (che potrebbe essere a (0,0))
    RECT taskbarRect = {};
    HMONITOR hTaskbarMonitor = NULL;
    UINT edge = GetTaskbarEdge(&taskbarRect, &hTaskbarMonitor);
    
    // Ottieni l'area di lavoro dal monitor della taskbar (fondamentale per multimonitor)
    RECT rcWork = {};
    if (!GetWorkAreaFromMonitor(hTaskbarMonitor, &rcWork)) {
        // Fallback se GetMonitorInfo fallisce
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);
    }

    NOTIFYICONIDENTIFIER nidIcon = { sizeof(NOTIFYICONIDENTIFIER) };
    nidIcon.hWnd = g_Ctx.hWndMsgHandler; nidIcon.uID = TRAY_ICON_ID; nidIcon.guidItem = TRAY_ICON_GUID;
    RECT rcIcon = { 0 };
    HRESULT hrRect = Shell_NotifyIconGetRect(&nidIcon, &rcIcon);
    Wh_Log(L"PositionWindowNearTray: dpi=%u ScaledSize=%dx%d edge=%u monitor=%p",
           dpi, g_ScaledWidth, g_ScaledHeight, edge, (void*)hTaskbarMonitor);
    Wh_Log(L"Shell_NotifyIconGetRect hr=0x%08X rcIcon={%d,%d,%d,%d}",
           (unsigned)hrRect, rcIcon.left, rcIcon.top, rcIcon.right, rcIcon.bottom);
    Wh_Log(L"Work area={%d,%d,%d,%d} taskbarRect={%d,%d,%d,%d}",
           rcWork.left, rcWork.top, rcWork.right, rcWork.bottom,
           taskbarRect.left, taskbarRect.top, taskbarRect.right, taskbarRect.bottom);

    // Usa l'icona tray se disponibile, altrimenti ancona al bordo della taskbar.
    // Questo assicura che il flyout appaia sullo stesso monitor dell'icona.
    POINT ptAnchor = { 0 };
    BOOL useIconPosition = (hrRect == S_OK && 
                           rcIcon.left != 0 && rcIcon.top != 0 &&
                           (rcIcon.right - rcIcon.left) > 0 && 
                           (rcIcon.bottom - rcIcon.top) > 0);
    
    if (useIconPosition) {
        // Posiziona l'anchor vicino all'icona tray (comportamento classico Win7)
        switch (edge) {
            case ABE_TOP:
                ptAnchor.x = rcIcon.left;
                ptAnchor.y = rcIcon.bottom;
                break;
            case ABE_LEFT:
                ptAnchor.x = rcIcon.right;
                ptAnchor.y = (rcIcon.top + rcIcon.bottom) / 2;
                break;
            case ABE_RIGHT:
                ptAnchor.x = rcIcon.left;
                ptAnchor.y = (rcIcon.top + rcIcon.bottom) / 2;
                break;
            default: // ABE_BOTTOM
                ptAnchor.x = rcIcon.left;
                ptAnchor.y = rcIcon.top;
                break;
        }
        Wh_Log(L"Using tray icon position: anchor={%d,%d}", ptAnchor.x, ptAnchor.y);
    } else {
        // Fallback: ancora al bordo della taskbar (angolo vicino alla system tray)
        switch (edge) {
            case ABE_TOP:
                ptAnchor.x = taskbarRect.right;
                ptAnchor.y = taskbarRect.bottom;
                break;
            case ABE_LEFT:
                ptAnchor.x = taskbarRect.right;
                ptAnchor.y = taskbarRect.bottom;
                break;
            case ABE_RIGHT:
                ptAnchor.x = taskbarRect.left;
                ptAnchor.y = taskbarRect.bottom;
                break;
            default: // ABE_BOTTOM
                ptAnchor.x = taskbarRect.right;
                ptAnchor.y = taskbarRect.top;
                break;
        }
        Wh_Log(L"Using taskbar edge position: anchor={%d,%d}", ptAnchor.x, ptAnchor.y);
    }

    // Flag TPM adattati al bordo: il flyout si apre sempre verso l'interno dello schermo.
    UINT tpmFlags;
    switch (edge) {
        case ABE_TOP:   tpmFlags = TPM_LEFTALIGN  | TPM_TOPALIGN    | TPM_VERTICAL; break;
        case ABE_LEFT:  tpmFlags = TPM_LEFTALIGN  | TPM_BOTTOMALIGN | TPM_VERTICAL; break;
        case ABE_RIGHT: tpmFlags = TPM_RIGHTALIGN | TPM_BOTTOMALIGN | TPM_VERTICAL; break;
        default:        tpmFlags = TPM_LEFTALIGN  | TPM_BOTTOMALIGN | TPM_VERTICAL; break;
    }

    SIZE szFlyout = { g_ScaledWidth, g_ScaledHeight };
    RECT rcExclude = taskbarRect;
    RECT rcResult = { 0 };
    BOOL bPopup = CalculatePopupWindowPosition(&ptAnchor, &szFlyout, tpmFlags, &rcExclude, &rcResult);
    Wh_Log(L"CalculatePopupWindowPosition ok=%d tpmFlags=0x%X anchor={%d,%d} rcResult={%d,%d,%d,%d}",
           bPopup, tpmFlags, ptAnchor.x, ptAnchor.y, rcResult.left, rcResult.top, rcResult.right, rcResult.bottom);
    
    if (bPopup) {
        // Verifica che il risultato sia sul monitor corretto (multimonitor safety check)
        HMONITOR hResultMonitor = MonitorFromPoint({rcResult.left, rcResult.top}, MONITOR_DEFAULTTONEAREST);
        if (hResultMonitor != hTaskbarMonitor && hTaskbarMonitor != NULL) {
            Wh_Log(L"Warning: popup on wrong monitor, using fallback positioning");
            bPopup = FALSE; // Forza fallback
        }
    }
    
    if (bPopup) {
        SetWindowPos(hwnd, HWND_TOPMOST, rcResult.left, rcResult.top, g_ScaledWidth, g_ScaledHeight, SWP_NOACTIVATE);
    } else {
        // Fallback manuale per ogni bordo - USA SEMPRE l'area di lavoro del monitor della taskbar
        int x, y;
        int offsetX = MulDiv(10, (int)dpi, 96);
        int offsetY = MulDiv(6, (int)dpi, 96);
        
        switch (edge) {
            case ABE_TOP:
                x = rcWork.right - g_ScaledWidth - offsetX;
                y = taskbarRect.bottom + offsetY;
                break;
            case ABE_LEFT:
                x = taskbarRect.right + offsetY;
                y = rcWork.bottom - g_ScaledHeight - offsetX;
                break;
            case ABE_RIGHT:
                x = taskbarRect.left - g_ScaledWidth - offsetY;
                y = rcWork.bottom - g_ScaledHeight - offsetX;
                break;
            default: // ABE_BOTTOM
                x = rcWork.right - g_ScaledWidth - offsetX;
                y = taskbarRect.top - g_ScaledHeight - offsetY;
                break;
        }
        
        // Multimonitor safety: assicurati che la finestra sia visibile sul monitor
        // Se x/y sono fuori dal work area, clampali
        if (x < rcWork.left) x = rcWork.left + offsetX;
        if (y < rcWork.top) y = rcWork.top + offsetY;
        if (x + g_ScaledWidth > rcWork.right) x = rcWork.right - g_ScaledWidth - offsetX;
        if (y + g_ScaledHeight > rcWork.bottom) y = rcWork.bottom - g_ScaledHeight - offsetY;
        
        Wh_Log(L"Fallback positioning: pos={%d,%d} workArea={%d,%d,%d,%d}", 
               x, y, rcWork.left, rcWork.top, rcWork.right, rcWork.bottom);
        SetWindowPos(hwnd, HWND_TOPMOST, x, y, g_ScaledWidth, g_ScaledHeight, SWP_NOACTIVATE);
    }

    // Applica sempre l'offset bordo taskbar usando il monitor corretto
    POINT pt = AdjustWindowPosForTaskbar(hwnd);
    SetWindowPos(hwnd, NULL, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
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
        ShowWindow(g_Ctx.hWndFlyout, SW_SHOWNOACTIVATE);
        UpdateWindow(g_Ctx.hWndFlyout);
        InvalidateRect(g_Ctx.hWndFlyout, NULL, TRUE);
        KillTimer(g_Ctx.hWndFlyout, AUTOHIDE_TIMER_ID);
        SetTimer(g_Ctx.hWndFlyout, AUTOHIDE_TIMER_ID, AUTOHIDE_INACTIVITY_MS, NULL);
        UpdateCachedTrayIconRect();
        InstallClickOutsideHook();
        InstallKeyboardHook();
        return;
    }
    CreateFlyoutWindow();
    if (g_Ctx.hWndFlyout) {
        CheckSecurityProviders();
        PositionWindowNearTray(g_Ctx.hWndFlyout);
        ShowWindow(g_Ctx.hWndFlyout, SW_SHOWNOACTIVATE);
        UpdateWindow(g_Ctx.hWndFlyout);
        AnimateWindow(g_Ctx.hWndFlyout, 180, AW_SLIDE | AW_VER_NEGATIVE);
        InvalidateRect(g_Ctx.hWndFlyout, NULL, TRUE);
        
        // Reset inactivity autohide timer every time the flyout is shown.
        KillTimer(g_Ctx.hWndFlyout, AUTOHIDE_TIMER_ID);
        SetTimer(g_Ctx.hWndFlyout, AUTOHIDE_TIMER_ID, AUTOHIDE_INACTIVITY_MS, NULL);
        
        UpdateCachedTrayIconRect();
        InstallClickOutsideHook();
        InstallKeyboardHook();
    }
}

// ============================================================================
// Mouse Hook (Click Outside) - Versione semplificata
// ============================================================================

// Cache the tray icon rect for use in the mouse hook (avoids cross-process call in WH_MOUSE_LL)
static void UpdateCachedTrayIconRect() {
    if (g_Ctx.hWndMsgHandler && g_Ctx.trayIconAdded) {
        NOTIFYICONIDENTIFIER nidIcon = { sizeof(NOTIFYICONIDENTIFIER) };
        nidIcon.hWnd = g_Ctx.hWndMsgHandler;
        nidIcon.uID = TRAY_ICON_ID;
        nidIcon.guidItem = TRAY_ICON_GUID;
        Shell_NotifyIconGetRect(&nidIcon, &g_CachedTrayIconRect);
    }
}

void InstallClickOutsideHook() {
    if (g_hMouseHook) return;
    
    // Usa WH_MOUSE_LL globale
    g_hMouseHook = SetWindowsHookExW(WH_MOUSE_LL, ClickOutsideMouseHookProc, GetModInstance(), 0);
    if (!g_hMouseHook) {
        Wh_Log(L"Failed to install mouse hook (error: %lu)", GetLastError());
    } else {
    }
}

void RemoveClickOutsideHook() {
    if (g_hMouseHook) {
        UnhookWindowsHookEx(g_hMouseHook);
        g_hMouseHook = NULL;
    }
}

// ============================================================================
// Keyboard Hook (Escape to close flyout)
// ============================================================================
void InstallKeyboardHook() {
    if (g_hKeyboardHook) return;
    g_hKeyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardHookProc, GetModInstance(), 0);
    if (!g_hKeyboardHook) {
        Wh_Log(L"Failed to install keyboard hook (error: %lu)", GetLastError());
    } else {
    }
}

void RemoveKeyboardHook() {
    if (g_hKeyboardHook) {
        UnhookWindowsHookEx(g_hKeyboardHook);
        g_hKeyboardHook = NULL;
    }
}

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (g_Ctx.isUninitializing)
        return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);

    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* pKbd = (KBDLLHOOKSTRUCT*)lParam;
        if (pKbd->vkCode == VK_ESCAPE) {
            // Close flyout if visible
            if (g_Ctx.hWndFlyout && IsWindow(g_Ctx.hWndFlyout) && 
                IsWindowVisible(g_Ctx.hWndFlyout) && !g_FlyoutClosing) {
                PostMessageW(g_Ctx.hWndFlyout, WM_SAFE_CLOSE, 0, 0);
                return 1; // Swallow the key
            }
        }
    }
    return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK ClickOutsideMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (g_Ctx.isUninitializing)
        return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);

    if (nCode == HC_ACTION &&
        (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN ||
         wParam == WM_MBUTTONDOWN || wParam == WM_NCLBUTTONDOWN || wParam == WM_NCRBUTTONDOWN)) {
        if (g_Ctx.hWndFlyout && IsWindow(g_Ctx.hWndFlyout) && IsWindowVisible(g_Ctx.hWndFlyout) && !g_FlyoutClosing) {
            MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lParam;
            RECT rcFlyout; GetWindowRect(g_Ctx.hWndFlyout, &rcFlyout);
            if (!PtInRect(&rcFlyout, pMouse->pt)) {
                // Use cached tray icon rect (updated when flyout opens)
                // Avoids expensive cross-process Shell_NotifyIconGetRect call in WH_MOUSE_LL
                BOOL overTrayIcon = PtInRect(&g_CachedTrayIconRect, pMouse->pt);
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
    RemoveKeyboardHook();
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

        {
            BOOL pfEnabled = FALSE;
            if (DwmIsCompositionEnabled(&pfEnabled) == S_OK && pfEnabled) {
                DWMNCRENDERINGPOLICY pol = DWMNCRP_ENABLED;
                DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &pol, sizeof(pol));
                if (g_Settings.useRoundedCorners) {
                    DWM_WINDOW_CORNER_PREFERENCE cornerPref = DWMWCP_ROUND;
                    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPref, sizeof(cornerPref));
                }
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
            RemoveKeyboardHook();
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
        
        // Leggi problemi e stato in un unico snapshot coerente.
        int activeProblems, secState, problemTypesCopy[MAX_PROBLEMS];
        { SRWGuard guard(g_Ctx.srwLock, false);
          activeProblems = g_ActiveProblems;
          secState = g_SecurityState;
          memcpy(problemTypesCopy, g_ProblemTypes, sizeof(g_ProblemTypes)); }
        
        // Selezione dinamica della bandiera senza modificare la system tray:
        // ID 0 = buono, ID 1 = avviso, ID 2 = allarme.
        void* flagBitmap = g_pBmpFlyoutGood;
        HICON flagFallback = g_hFlyoutIconGood;
        if (secState >= STATE_ALERT) {
            flagBitmap = g_pBmpFlyoutAlert;
            flagFallback = g_hFlyoutIconAlert;
        } else if (secState >= STATE_WARNING) {
            flagBitmap = g_pBmpFlyoutWarning;
            flagFallback = g_hFlyoutIconWarning;
        }

        // Tutti e tre i PNG sono 32x32 e usano la stessa pipeline GDI+ HQ.
        int flagSize = ScaleDpi(32);
        int flagY = (hdrH - flagSize) / 2;
        if (!DrawGdipBitmapHighQuality(hdcMem, flagBitmap,
                                       padL, flagY, flagSize, flagSize)) {
            // Fallback HICON dello stesso stato se GDI+ non e' disponibile.
            DrawIconEx(hdcMem, padL, flagY, flagFallback,
                       flagSize, flagSize, 0, NULL, DI_NORMAL);
        }
        
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
    RECT rcT = {txL, ScaleDpi(5), g_ScaledWidth - padR, ScaleDpi(25)};
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
                totalText = L"\u0432\u0441\u0435\u0433\u043E \u0441\u043E\u043E\u0431\u0449\u0435\u043D\u0438\u0435";   // ????? Русский??
            else if (activeProblems % 10 >= 2 && activeProblems % 10 <= 4 && (activeProblems % 100 < 10 || activeProblems % 100 >= 20))
                totalText = L"\u0432\u0441\u0435\u0433\u043E \u0441\u043E\u043E\u0431\u0449\u0435\u043D\u0438\u044F";   // ????? Русский??
            else
                totalText = L"\u0432\u0441\u0435\u0433\u043E \u0441\u043E\u043E\u0431\u0449\u0435\u043D\u0438\u0439";   // ????? Русский??
            break;
                case 0x0816: // Portuguese
            totalText = (activeProblems == 1) ? L"mensagem total" : L"mensagens totais";
            break;
        case 0x0407: // German
            totalText = (activeProblems == 1) ? L"gesamte Meldung" : L"gesamte Meldungen";
            break;
        default:     // English
            totalText = (activeProblems == 1) ? L"total message" : L"total messages";
            break;
    }
    StringCchPrintfW(totalBuf, ARRAYSIZE(totalBuf), L"%d %s", activeProblems, totalText);
    
    // Riduci lo spazio tra le righe del 5% (da ScaleDpi(22) a ScaleDpi(21))
    RECT rcTotal = {txL, ScaleDpi(24), g_ScaledWidth - padR, ScaleDpi(44)};
    DrawTextW(hdcMem, totalBuf, -1, &rcTotal, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
} else {
        // "Action Center" in blu e bold
        SelectGuard sg(hdcMem, g_hFontBold);
        SetTextColor(hdcMem, clrLink);
        RECT rcTitleLine = {txL, ScaleDpi(5), g_ScaledWidth - padR, ScaleDpi(24)};
        DrawTextW(hdcMem, LOC(STR_ACTION_CENTER_TITLE), -1, &rcTitleLine, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        
        // "No current issues detected." in blu (non bold)
        SelectGuard sg2(hdcMem, g_hFontNormal);
        SetTextColor(hdcMem, clrLink);
        RECT rcSubLine = {txL, ScaleDpi(24), g_ScaledWidth - padR, ScaleDpi(44)};
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
        // FOOTER LINK - Spostato 4% più in basso
        // ============================================================
        { SelectGuard sg(hdcMem, g_IsHoveringLink ? g_hFontLink : g_hFontNormal);
          SetTextColor(hdcMem, g_IsHoveringLink ? clrLinkHover : clrLink);
          
          RECT rcClient; GetClientRect(hwnd, &rcClient);
          int currentFtrH = g_ScaledFooterHeight;
          
          // Calcola l'inset per il rounded corners (se attivo)
          int topInset = g_Settings.useRoundedCorners ? (currentFtrH * 12) / 100 : 0;
          
          int offsetPercent = 13;  
          int offsetPixels = (currentFtrH * offsetPercent) / 87;
          
          // Applica l'offset: aumenta topInset per spostare il testo verso il basso
          int adjustedTopInset = topInset + offsetPixels;
          
          RECT rcFtrDynamic = { 0, rcClient.bottom - currentFtrH + adjustedTopInset, rcClient.right, rcClient.bottom };

          g_rcFooterLink = rcFtrDynamic;
          DrawTextW(hdcMem, LOC(STR_LINK_OPEN_AC), -1, &rcFtrDynamic, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); }
        
        // ============================================================
        // BORDI (rimossi per look nativo Windows)
        // ============================================================
        // Il bordo è ora gestito interamente da DWM (DropShadow + Round Corners)
        
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
        wc.lpfnWndProc = FlyoutWndProc; wc.hInstance = GetModInstance();
        wc.lpszClassName = FLYOUT_CLASS_NAME; wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        if (RegisterClassExW(&wc)) {
            g_Ctx.flyoutClassRegistered = TRUE;
        } else {
            Wh_Log(L"Flyout class registration failed: %lu", GetLastError());
            return; // non usare una classe residua con una vecchia WndProc
        }
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
            NULL, NULL, GetModInstance(), NULL);
    } else {
        g_Ctx.hWndFlyout = CreateWindowExW(dwExStyle, FLYOUT_CLASS_NAME, L"Win7 AC", dwStyle, 
            0, 0, flyoutWidth, flyoutHeight, 
            NULL, NULL, GetModInstance(), NULL);
    }
    if (!g_Ctx.hWndFlyout) { Wh_Log(L"Flyout creation failed: %lu", GetLastError()); return; }
    
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
        InterlockedIncrement(&g_Ctx.refCount);
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
        void* notifyBitmap = g_pBmpFlyoutGood;
        HICON notifyFallback = g_hFlyoutIconGood;
        if (secState >= STATE_ALERT) {
            notifyBitmap = g_pBmpFlyoutAlert;
            notifyFallback = g_hFlyoutIconAlert;
        } else if (secState >= STATE_WARNING) {
            notifyBitmap = g_pBmpFlyoutWarning;
            notifyFallback = g_hFlyoutIconWarning;
        }
        if (!DrawGdipBitmapHighQuality(hdcMem, notifyBitmap,
                                       ScaleDpi(10), ScaleDpi(26),
                                       iconSize, iconSize)) {
            DrawIconEx(hdcMem, ScaleDpi(10), ScaleDpi(26), notifyFallback,
                       iconSize, iconSize, 0, NULL, DI_NORMAL);
        }
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
    case WM_DESTROY:
        KillTimer(hwnd, NOTIFY_TIMER_ID);
        if (g_Ctx.hWndNotify == hwnd) g_Ctx.hWndNotify = NULL;
        g_NotifyShowing = FALSE;
        InterlockedDecrement(&g_Ctx.refCount);
        break;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

void CreateNotifyWindow() {
    if (g_Ctx.hWndNotify && IsWindow(g_Ctx.hWndNotify)) return;
    if (g_Ctx.isUninitializing) return;
    if (g_Ctx.trayThreadId && GetCurrentThreadId() != g_Ctx.trayThreadId) {
        Wh_Log(L"CreateNotifyWindow rejected on non-tray thread");
        return;
    }
    if (!g_Ctx.notifyClassRegistered) {
        WNDCLASSEXW wc = { sizeof(WNDCLASSEXW) };
        wc.lpfnWndProc = NotifyWndProc; wc.hInstance = GetModInstance();
        wc.lpszClassName = NOTIFY_WINDOW_CLASS_NAME; wc.hCursor = LoadCursor(NULL, IDC_HAND);
        if (RegisterClassExW(&wc)) {
            g_Ctx.notifyClassRegistered = TRUE;
        } else {
            Wh_Log(L"Notify class registration failed: %lu", GetLastError());
            return; // evita una WndProc residua appartenente alla vecchia build
        }
    }
    g_Ctx.hWndNotify = CreateWindowExW(WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE, NOTIFY_WINDOW_CLASS_NAME, L"", WS_POPUP, 0,0,g_ScaledNotifyWidth,g_ScaledNotifyHeight, NULL,NULL,GetModInstance(),NULL);
    if (!g_Ctx.hWndNotify) Wh_Log(L"Notify window creation failed: %lu", GetLastError());
}

// ============================================================================
// Tray Message Handler
// ============================================================================
// ============================================================================
// Tray Message Handler
// ============================================================================
LRESULT CALLBACK TrayMsgHandlerProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (g_Ctx.taskbarCreatedMessage && uMsg == g_Ctx.taskbarCreatedMessage) {
        Wh_Log(L"TaskbarCreated received; scheduling tray icon re-creation");

        // After explorer restart the old balloon no longer belongs to a valid
        // tray icon. Release its local HICON before scheduling recovery.
        ReleaseProblemBalloonResources();

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
        if (wParam == PROBLEM_BALLOON_TIMER_ID) {
            RemoveProblemBalloon();
            return 0;
        }
        if (wParam == REFRESH_TIMER_ID) {
            if (!g_Ctx.isUninitializing) {
                int prevStateBk, prevProblemsBk;
                { SRWGuard g(g_Ctx.srwLock, false); prevStateBk = g_SecurityState; prevProblemsBk = g_ActiveProblems; }

                RefreshSecurityState();

                int newStateBk, newProblemsBk;
                { SRWGuard g(g_Ctx.srwLock, false); newStateBk = g_SecurityState; newProblemsBk = g_ActiveProblems; }

                // Backoff del refresh periodico: se lo stato resta invariato per
                // diversi controlli consecutivi, allunga gradualmente l'intervallo
                // (fino a 4x, cap 30s) per ridurre il carico CPU quando tutto e'
                // stabile. Alla prima variazione si torna subito all'intervallo base.
                // Non modifica alcun percorso di notifica/balloon: agisce solo sul
                // periodo del timer REFRESH_TIMER_ID.
                if (g_Settings.refreshInterval > 0 && hwnd && IsWindow(hwnd)) {
                    if (prevStateBk == newStateBk && prevProblemsBk == newProblemsBk) {
                        if (g_RefreshNoChangeCount < 0x7FFFFFFF) g_RefreshNoChangeCount++;
                    } else {
                        g_RefreshNoChangeCount = 0;
                    }

                    UINT_PTR baseInterval = (UINT_PTR)g_Settings.refreshInterval;
                    UINT_PTR desiredInterval = baseInterval;
                    if (g_RefreshNoChangeCount >= 12) {
                        UINT_PTR mul = 1 + (g_RefreshNoChangeCount - 12) / 12;
                        if (mul > 4) mul = 4;
                        desiredInterval = baseInterval * mul;
                        if (desiredInterval > 30000) desiredInterval = 30000;
                    }

                    if (desiredInterval != g_RefreshCurrentInterval) {
                        KillTimer(hwnd, REFRESH_TIMER_ID);
                        g_Ctx.refreshTimer = SetTimer(hwnd, REFRESH_TIMER_ID, (UINT)desiredInterval, NULL);
                        g_RefreshCurrentInterval = desiredInterval;
                    }
                }
            }
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
        // Tutto cio' che possiede una WndProc della mod viene distrutto qui,
        // sul thread proprietario, prima che Windhawk scarichi il codice.
        InterlockedExchange(&g_Ctx.isUninitializing, 1L);
        RemoveClickOutsideHook();

        KillTimer(hwnd, PROBLEM_BALLOON_TIMER_ID);
        KillTimer(hwnd, REFRESH_TIMER_ID);
        KillTimer(hwnd, TRAY_RETRY_TIMER_ID);
        KillTimer(hwnd, TRAY_HEALTH_TIMER_ID);
        RemoveProblemBalloon();

        HWND hFly = g_Ctx.hWndFlyout;
        if (hFly && IsWindow(hFly)) {
            // Nessuna AnimateWindow durante l'unload: distruzione sincrona.
            KillTimer(hFly, AUTOHIDE_TIMER_ID);
            DestroyWindow(hFly);
        }

        HWND hNotify = g_Ctx.hWndNotify;
        if (hNotify && IsWindow(hNotify)) {
            KillTimer(hNotify, NOTIFY_TIMER_ID);
            DestroyWindow(hNotify);
        }

        PostQuitMessage(0);
        return 0;
    }
    if (uMsg == WM_TRAY_ICON_MSG) {
        UINT trayEvent = LOWORD(lParam);

        if (trayEvent == NIN_BALLOONSHOW) {
            g_ProblemBalloonShowing = TRUE;
            return 0;
        }
        if (trayEvent == NIN_BALLOONHIDE ||
            trayEvent == NIN_BALLOONTIMEOUT) {
            ReleaseProblemBalloonResources();
            return 0;
        }
        if (trayEvent == NIN_BALLOONUSERCLICK) {
            RemoveProblemBalloon();
            // Il clic deve aprire, non chiudere, il flyout.
            if (!g_Ctx.hWndFlyout || !IsWindow(g_Ctx.hWndFlyout) ||
                !IsWindowVisible(g_Ctx.hWndFlyout)) {
                PostMessageW(hwnd, WM_TRIGGER_FLYOUT, 0, 0);
            }
            return 0;
        }

        if (trayEvent == WM_LBUTTONUP) { 
            if (g_ProblemBalloonShowing) RemoveProblemBalloon();
            PostMessageW(hwnd, WM_TRIGGER_FLYOUT, 0, 0); 
            return 0; 
        }
        if (trayEvent == WM_RBUTTONUP) {
            if (g_ProblemBalloonShowing) RemoveProblemBalloon();
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
            RefreshSecurityState(); 
        } 
        return 0; 
    }
    if (uMsg == WM_SIMULATE_NOTIFICATION) { 
        if (g_Settings.enableNotificationSimulation && !g_Ctx.isUninitializing) {
            SimulateNotification((int)wParam);
        }
        return 0; 
    }
    if (uMsg == WM_CLEAR_NOTIFICATIONS) { 
        if (!g_Ctx.isUninitializing) ClearNotifications(); 
        return 0; 
    }
    if (uMsg == WM_SETTINGS_CHANGED) {
        // Handle hotkey and timer updates from the tray thread (correct thread affinity)
        if (!g_Ctx.isUninitializing) {
            // Applica subito l'opzione theme (auto/light/dark) e forza il
            // repaint di flyout e popup di notifica con i nuovi colori.
            ApplyThemeToWindows();
            if (g_Settings.enableHotkey) {
                RegisterHotKey(hwnd, HOTKEY_ID_SIMULATE, MOD_CONTROL, 'N');
                RegisterHotKey(hwnd, HOTKEY_ID_CLEAR, MOD_CONTROL | MOD_SHIFT, 'N');
            } else {
                UnregisterHotKey(hwnd, HOTKEY_ID_SIMULATE);
                UnregisterHotKey(hwnd, HOTKEY_ID_CLEAR);
            }
            if (g_Settings.refreshInterval > 0) {
                if (g_Ctx.refreshTimer) KillTimer(hwnd, g_Ctx.refreshTimer);
                g_Ctx.refreshTimer = SetTimer(hwnd, REFRESH_TIMER_ID, g_Settings.refreshInterval, NULL);
                // L'utente ha (ri)applicato le impostazioni: riparti dall'intervallo
                // base invece di restare su un intervallo "rallentato" da un backoff
                // precedente.
                g_RefreshNoChangeCount = 0;
                g_RefreshCurrentInterval = (UINT_PTR)g_Settings.refreshInterval;
            } else if (g_Ctx.refreshTimer) {
                KillTimer(hwnd, g_Ctx.refreshTimer);
                g_Ctx.refreshTimer = 0;
                g_RefreshNoChangeCount = 0;
                g_RefreshCurrentInterval = 0;
            }
        }
        return 0;
    }
    if (uMsg == WM_SETTINGCHANGE) {
        // Il tema chiaro/scuro di Windows e' cambiato mentre la mod e' in
        // esecuzione (broadcast "ImmersiveColorSet"). In modalita' "auto" il
        // flyout segue subito il sistema, senza dover riaprire le impostazioni.
        if (!g_Ctx.isUninitializing && g_Settings.theme == 0 && lParam &&
            wcsstr((LPCWSTR)lParam, L"ImmersiveColorSet") &&
            GetEffectiveDarkMode() != g_Ctx.darkMode) {
            ApplyThemeToWindows();
        }
        // Live Windows display-language detection: when the mod language is
        // set to "Auto (match Windows)", follow the Windows display language
        // live without restarting the mod or reopening the settings.
        if (g_Settings.language == 0) {
            LANGID ui = GetUserDefaultUILanguage();
            if (ui != g_LastDetectedUILang) {
                DetermineLocale();
                RefreshLocalizedUI();
            }
        }
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
            // Il relativo handler aggiorna prima lo stato e mostra poi il
            // balloon, evitando che venga usata l'icona dello stato precedente.
            PostMessageW(hwnd, WM_SIMULATE_NOTIFICATION, c, 0); 
            return 0; 
        }
        if (wParam == HOTKEY_ID_CLEAR) { 
            PostMessageW(hwnd, WM_CLEAR_NOTIFICATIONS, 0, 0); 
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
        mwc.lpfnWndProc = TrayMsgHandlerProc; mwc.hInstance = GetModInstance();
        mwc.lpszClassName = TRAY_MESSAGE_CLASS_NAME;
        if (RegisterClassExW(&mwc)) {
            g_Ctx.trayMsgClassRegistered = TRUE;
        } else {
            Wh_Log(L"Tray message class registration failed: %lu", GetLastError());
            if (g_Ctx.hTrayReadyEvent) SetEvent(g_Ctx.hTrayReadyEvent);
            CoUninitialize();
            return 1; // non usare la WndProc appartenente alla build precedente
        }
    }

    // TaskbarCreated è un broadcast inviato alle finestre top-level.
    // Usiamo WS_POPUP per riceverlo e ripristinare l'icona dopo il riavvio di Explorer.
    g_Ctx.taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");
    g_Ctx.hWndMsgHandler = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, TRAY_MESSAGE_CLASS_NAME, L"",
        WS_POPUP, 0, 0, 0, 0, NULL, NULL, GetModInstance(), NULL);

    if (!g_Ctx.hWndMsgHandler) {
        if (g_Ctx.hTrayReadyEvent) SetEvent(g_Ctx.hTrayReadyEvent);
        CoUninitialize();
        return 1;
    }

    // La notify window appartiene ora allo stesso thread/message loop del
    // flyout e del tray handler, quindi puo' essere distrutta sincronicamente.
    CreateNotifyWindow();

    if (g_Ctx.hTrayReadyEvent) SetEvent(g_Ctx.hTrayReadyEvent);

    // At Windows startup / explorer restart the tray may not exist yet.
    // Wait a bit, then add; recovery timers cover the rest.
    if (!WaitForTaskbarReady(15000)) {
        Wh_Log(L"Shell_TrayWnd not ready within 15s - scheduling recovery");
    }
    // Se Wh_ModUninit e' arrivato durante l'attesa della taskbar, non
    // registrare nuove sorgenti asincrone dopo che il cleanup le ha fermate.
    if (!g_Ctx.isUninitializing) {
        AddTrayIcon();
        if (!g_Ctx.trayIconAdded)
            ScheduleTrayIconRecovery();
        EnsureTrayTooltip();
        RegisterWscNotifications();
        StartRegistryMonitor();

        if (g_Settings.refreshInterval > 0) {
            g_Ctx.refreshTimer = SetTimer(g_Ctx.hWndMsgHandler, REFRESH_TIMER_ID, g_Settings.refreshInterval, NULL);
            g_RefreshNoChangeCount = 0;
            g_RefreshCurrentInterval = (UINT_PTR)g_Settings.refreshInterval;
        }

        SetTimer(g_Ctx.hWndMsgHandler, TRAY_HEALTH_TIMER_ID, 15000, NULL);

        if (g_Settings.enableHotkey) {
            RegisterHotKey(g_Ctx.hWndMsgHandler, HOTKEY_ID_SIMULATE, MOD_CONTROL, 'N');
            RegisterHotKey(g_Ctx.hWndMsgHandler, HOTKEY_ID_CLEAR, MOD_CONTROL | MOD_SHIFT, 'N');
        }
    }

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // Thread cleanup. Fallback anche per un WM_QUIT che non sia passato
    // attraverso WM_TRAY_SHUTDOWN.
    RemoveClickOutsideHook();
    if (g_Ctx.hWndFlyout && IsWindow(g_Ctx.hWndFlyout))
        DestroyWindow(g_Ctx.hWndFlyout);
    if (g_Ctx.hWndNotify && IsWindow(g_Ctx.hWndNotify))
        DestroyWindow(g_Ctx.hWndNotify);

    if (g_Settings.enableHotkey) {
        UnregisterHotKey(g_Ctx.hWndMsgHandler, HOTKEY_ID_SIMULATE);
        UnregisterHotKey(g_Ctx.hWndMsgHandler, HOTKEY_ID_CLEAR);
    }
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

    // 1. Blocca immediatamente nuove operazioni e nuove registrazioni.
    InterlockedExchange(&g_Ctx.isUninitializing, 1L);

    // Remove keyboard hook immediately (no window context needed)
    RemoveKeyboardHook();

    // 2. Ferma le sorgenti esterne e attendi le callback gia' entrate.
    UnregisterWscNotifications();
    WaitForWscCallbacksToDrain();
    StopRegistryMonitor();

    // Se l'unload arriva immediatamente dopo CreateThread, attendi che il
    // tray thread abbia creato la propria message window (o sia fallito).
    if (g_Ctx.hTrayThread && g_Ctx.hTrayReadyEvent &&
        (!g_Ctx.hWndMsgHandler || !IsWindow(g_Ctx.hWndMsgHandler))) {
        WaitForSingleObject(g_Ctx.hTrayReadyEvent, 5000);
    }

    // 3. Chiedi al thread proprietario di distruggere TUTTE le sue finestre.
    // Non chiamiamo mai DestroyWindow da questo thread.
    BOOL shutdownPosted = FALSE;
    HWND hMsg = g_Ctx.hWndMsgHandler;
    if (hMsg && IsWindow(hMsg))
        shutdownPosted = PostMessageW(hMsg, WM_TRAY_SHUTDOWN, 0, 0);

    if (!shutdownPosted && g_Ctx.hTrayThread &&
        WaitForSingleObject(g_Ctx.hTrayThread, 0) == WAIT_TIMEOUT &&
        g_Ctx.trayThreadId) {
        Wh_Log(L"Tray message window unavailable; posting WM_QUIT to owner thread");
        PostThreadMessageW(g_Ctx.trayThreadId, WM_QUIT, 0, 0);
    }

    // 4. La fine del thread e' la barriera: dopo non esistono WndProc, timer
    // o low-level mouse hook della mod ancora in esecuzione.
    if (g_Ctx.hTrayThread) {
        DWORD wr = WaitForSingleObject(g_Ctx.hTrayThread, 5000);
        if (wr == WAIT_TIMEOUT) {
            Wh_Log(L"Waiting for tray thread to finish owner-thread cleanup");
            WaitForSingleObject(g_Ctx.hTrayThread, INFINITE);
        }
        CloseHandle(g_Ctx.hTrayThread);
        g_Ctx.hTrayThread = NULL;
    }

    // Il tray thread potrebbe aver completato una registrazione WSC tardiva
    // mentre il cleanup attendeva il lock. Dopo la sua uscita non possono più
    // nascere nuove registrazioni; eseguiamo quindi una seconda barriera.
    WaitForWscCallbacksToDrain();

    if (g_Ctx.hTrayReadyEvent) {
        CloseHandle(g_Ctx.hTrayReadyEvent);
        g_Ctx.hTrayReadyEvent = NULL;
    }

    // Nessun DestroyWindow cross-thread. Questi controlli sono diagnostici:
    // normalmente il thread terminato ha gia' invalidato tutti gli HWND.
    if ((g_Ctx.hWndFlyout && IsWindow(g_Ctx.hWndFlyout)) ||
        (g_Ctx.hWndNotify && IsWindow(g_Ctx.hWndNotify)) ||
        (g_Ctx.hWndMsgHandler && IsWindow(g_Ctx.hWndMsgHandler))) {
        Wh_Log(L"WARNING: a mod window survived tray-thread shutdown");
    }

    g_Ctx.hWndFlyout = NULL;
    g_Ctx.hWndNotify = NULL;
    g_Ctx.hWndMsgHandler = NULL;

    // 5. Solo dopo la barriera del thread e' sicuro liberare il rendering.
    ShutdownGdiPlus();
    FreeAllIcons();
    FreeGlobalFonts();

    // 6. Le classi appartengono al modulo della mod, non a explorer.exe.
    HINSTANCE hInst = GetModInstance();
    if (g_Ctx.flyoutClassRegistered) {
        if (!UnregisterClassW(FLYOUT_CLASS_NAME, hInst) &&
            GetLastError() != ERROR_CLASS_DOES_NOT_EXIST)
            Wh_Log(L"Unregister flyout class failed: %lu", GetLastError());
        g_Ctx.flyoutClassRegistered = FALSE;
    }
    if (g_Ctx.notifyClassRegistered) {
        if (!UnregisterClassW(NOTIFY_WINDOW_CLASS_NAME, hInst) &&
            GetLastError() != ERROR_CLASS_DOES_NOT_EXIST)
            Wh_Log(L"Unregister notify class failed: %lu", GetLastError());
        g_Ctx.notifyClassRegistered = FALSE;
    }
    if (g_Ctx.trayMsgClassRegistered) {
        if (!UnregisterClassW(TRAY_MESSAGE_CLASS_NAME, hInst) &&
            GetLastError() != ERROR_CLASS_DOES_NOT_EXIST)
            Wh_Log(L"Unregister tray class failed: %lu", GetLastError());
        g_Ctx.trayMsgClassRegistered = FALSE;
    }

    // 7. Ora non esistono piu' thread o finestre che possano leggere g_Ctx.
    ZeroMemory(&g_Ctx, sizeof(g_Ctx));
    g_Initialized = FALSE;
}

// ===========================================================================
// Security and Maintenance Control Panel hub links (DirectUI UIFILE patch)
// Restores Troubleshooting + Recovery on the classic Action Center CPL page.
// Keeps original atom IDs for stability when expanding Security/Maintenance.
// ===========================================================================

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------
bool g_useEmbeddedUifile = false;

bool g_cplRestoreHubLinks = true;

void CplLoadSettings() {
    g_cplRestoreHubLinks = Wh_GetIntSetting(L"restoreCplHubLinks") != 0;
    g_useEmbeddedUifile = Wh_GetIntSetting(L"useEmbeddedUifile") != 0;
}

// ---------------------------------------------------------------------------
// Localization
// ---------------------------------------------------------------------------
struct LangPack {
    WORD primaryLang;
    const wchar_t* heading;
    const wchar_t* tsTitle;
    const wchar_t* tsDesc;
    const wchar_t* recTitle;
    const wchar_t* recDesc;
};

static const LangPack g_langPacks[] = {
    {0x09, L"If the problem isn't listed, try one of these:", L"Troubleshooting", L"Find and fix problems with your computer.", L"Recovery", L"Refresh your PC without affecting your files, or reset it and start over."},
    {0x10, L"Se il problema non \u00e8 incluso nell'elenco, provare uno dei metodi seguenti:", L"Risoluzione dei problemi", L"Trovare e risolvere i problemi del computer.", L"Ripristino", L"Aggiorna il PC mantenendo i file o reimpostalo e ricomincia dall'inizio."},
    {0x0c, L"Si le probl\u00e8me n'est pas r\u00e9pertori\u00e9, essayez l'une des m\u00e9thodes suivantes :", L"R\u00e9solution des probl\u00e8mes", L"Rechercher et r\u00e9soudre les probl\u00e8mes de l'ordinateur.", L"R\u00e9cup\u00e9ration", L"Actualisez le PC sans affecter vos fichiers, ou r\u00e9initialisez-le et recommencez."},
    {0x0a, L"Si el problema no est\u00e1 en la lista, pruebe uno de estos m\u00e9todos:", L"Soluci\u00f3n de problemas", L"Buscar y solucionar problemas del equipo.", L"Recuperaci\u00f3n", L"Actualiza el PC sin afectar a los archivos o restabl\u00e9celo y empieza de nuevo."},
    {0x19, L"\u0415\u0441\u043b\u0438 \u043f\u0440\u043e\u0431\u043b\u0435\u043c\u0430 \u043d\u0435 \u0443\u043a\u0430\u0437\u0430\u043d\u0430 \u0432 \u0441\u043f\u0438\u0441\u043a\u0435, \u043f\u043e\u043f\u0440\u043e\u0431\u0443\u0439\u0442\u0435 \u043e\u0434\u0438\u043d \u0438\u0437 \u0441\u043b\u0435\u0434\u0443\u044e\u0449\u0438\u0445 \u0441\u043f\u043e\u0441\u043e\u0431\u043e\u0432:", L"\u0423\u0441\u0442\u0440\u0430\u043d\u0435\u043d\u0438\u0435 \u043d\u0435\u043f\u043e\u043b\u0430\u0434\u043e\u043a", L"\u041f\u043e\u0438\u0441\u043a \u0438 \u0443\u0441\u0442\u0440\u0430\u043d\u0435\u043d\u0438\u0435 \u043f\u0440\u043e\u0431\u043b\u0435\u043c \u0441 \u043a\u043e\u043c\u043f\u044c\u044e\u0442\u0435\u0440\u043e\u043c.", L"\u0412\u043e\u0441\u0441\u0442\u0430\u043d\u043e\u0432\u043b\u0435\u043d\u0438\u0435", L"\u041e\u0431\u043d\u043e\u0432\u0438\u0442\u0435 \u041f\u041a, \u0441\u043e\u0445\u0440\u0430\u043d\u0438\u0432 \u0444\u0430\u0439\u043b\u044b, \u0438\u043b\u0438 \u0441\u0431\u0440\u043e\u0441\u044c\u0442\u0435 \u0435\u0433\u043e \u0438 \u043d\u0430\u0447\u043d\u0438\u0442\u0435 \u0441\u043d\u0430\u0447\u0430\u043b\u0430."},
    {0x16, L"Se o problema n\u00E3o estiver na lista, experimente um destes m\u00E9todos:", L"Resolu\u00E7\u00E3o de Problemas", L"Encontrar e corrigir problemas do computador.", L"Recupera\u00E7\u00E3o", L"Atualize o PC sem afetar os ficheiros ou restaure-o e recomece do in\u00EDcio."},
    {0x07, L"Falls das Problem nicht aufgef\u00FChrt ist, versuchen Sie eine der folgenden Methoden:", L"Problembehandlung", L"Suchen und Beheben von Problemen mit dem Computer.", L"Wiederherstellung", L"Aktualisieren Sie den PC, ohne Ihre Dateien zu beeintr\u00E4chtigen, oder setzen Sie ihn zur\u00FCck und fangen Sie von vorn an."}
};

static const LangPack* GetLangPack() {
    WORD ui = PRIMARYLANGID(GetUserDefaultUILanguage());
    for (const auto& p : g_langPacks) {
        if (p.primaryLang == ui) {
            return &p;
        }
    }
    for (const auto& p : g_langPacks) {
        if (p.primaryLang == 0x09) {
            return &p;
        }
    }
    return &g_langPacks[0];
}

static const char g_uifile201[] =
    "<duixml>\r\n"
    "<stylesheets>\r\n"
    "<style resid=\"cp_style\">\r\n"
    "<Button accessible=\"true\" contentalign=\"wrapleft\"/>\r\n"
    "<Element overhang=\"false\" background=\"argb(0,0,0,0)\"/>\r\n"
    "<NavigateButton overhang=\"false\" background=\"argb(0,0,0,0)\"/>\r\n"
    "<CCPushButton transparent=\"true\" accessible=\"true\" minsize=\"size(76rp,23rp)\" font=\"gtf(CONTROLPANELSTYLE,14,0)\" margin=\"rect(6rp,0rp,0rp,0rp)\"/>\r\n"
    "<CCCheckBox transparent=\"true\" accessible=\"true\" font=\"gtf(CONTROLPANELSTYLE,6,0)\" foreground=\"gtc(CONTROLPANELSTYLE,6,0,3803)\" background=\"themeable(dtb(CONTROLPANEL,2,0),window)\"/>\r\n"
    "<ComboBox transparent=\"true\" accessible=\"true\" font=\"gtf(CONTROLPANELSTYLE,14,0)\"/>\r\n"
    "<CCRadioButton transparent=\"true\" accessible=\"true\" font=\"gtf(CONTROLPANELSTYLE,6,0)\" foreground=\"gtc(CONTROLPANELSTYLE,6,0,3803)\" background=\"themeable(dtb(CONTROLPANEL,2,0),window)\"/>\r\n"
    "<CCSysLink transparent=\"true\" accessible=\"true\" font=\"gtf(CONTROLPANELSTYLE,6,0)\" foreground=\"gtc(CONTROLPANELSTYLE,6,0,3803)\" background=\"themeable(dtb(CONTROLPANEL,2,0), window)\"/>\r\n"
    "<Edit transparent=\"true\" themedborder=\"true\" width=\"120rp\" accessible=\"true\" accrole=\"text\" height=\"20rp\" font=\"gtf(CONTROLPANELSTYLE,6,0)\" foreground=\"gtc(CONTROLPANELSTYLE,6,0,3803)\"/>\r\n"
    "<if class=\"cp_topbox\">\r\n"
    "<Element accessible=\"true\" accrole=\"client\" background=\"themeable(dtb(CONTROLPANEL,2,0),window)\"/>\r\n"
    "</if>\r\n"
    "<if class=\"cp_hub_frame\">\r\n"
    "<Element padding=\"rect(13rp,1rp,1rp,10rp)\"/>\r\n"
    "</if>\r\n"
    "<if class=\"cp_spoke_frame\">\r\n"
    "<Element padding=\"rect(0rp,19rp,0rp,10rp)\"/>\r\n"
    "</if>\r\n"
    "<if class=\"cp_help_glyph\">\r\n"
    "<Button height=\"18rp\" width=\"18rp\" content=\"icon(99,sysmetric(49),sysmetric(50),library(imageres.dll))\" padding=\"rect(1rp,1rp,1rp,1rp)\" cursor=\"hand\" accRole=\"link\" accdefaction=\"click\" accState=\"0x00100000\" tooltip=\"true\"/>\r\n"
    "<if keyfocused=\"true\">\r\n"
    "<Button contentalign=\"focusrect\"/>\r\n"
    "</if>\r\n"
    "</if>\r\n"
    "<if class=\"cp_content_pane\">\r\n"
    "<Element width=\"600rp\" padding=\"rect(10rp,0rp,10rp,0rp)\" background=\"themeable(dtb(CONTROLPANEL,2,0),window)\"/>\r\n"
    "</if>\r\n"
    "<if class=\"cp_content_instruction\">\r\n"
    "<Element contentalign=\"wrapleft\" foreground=\"gtc(CONTROLPANELSTYLE,5,0,3803)\" font=\"gtf(CONTROLPANELSTYLE, 5, 0)\" accessible=\"true\" accRole=\"statictext\"/>\r\n"
    "</if>\r\n"
    "<if class=\"cp_content_v_spacer\">\r\n"
    "<Element height=\"7rp\"/>\r\n"
    "</if>\r\n"
    "<if class=\"cp_content_text\">\r\n"
    "<Element font=\"gtf(CONTROLPANELSTYLE, 6, 0)\" foreground=\"gtc(CONTROLPANELSTYLE,6,0,3803)\" contentalign=\"wrapleft\" accessible=\"true\" accRole=\"statictext\"/>\r\n"
    "<PText font=\"gtf(CONTROLPANELSTYLE, 6, 0)\" foreground=\"gtc(CONTROLPANELSTYLE,6,0,3803)\" contentalign=\"wrapleft\" accessible=\"true\" accRole=\"statictext\"/>\r\n"
    "</if>\r\n"
    "<if class=\"cp_content_title_text\">\r\n"
    "<Element font=\"gtf(CONTROLPANELSTYLE, 19, 0)\" foreground=\"gtc(CONTROLPANELSTYLE,19,0,3803)\" contentalign=\"wrapleft\" accessible=\"true\" accRole=\"statictext\"/>\r\n"
    "</if>\r\n"
    "<if class=\"cp_help_link\">\r\n"
    "<Button accessible=\"true\" accRole=\"link\" accdefaction=\"click\" foreground=\"gtc(CONTROLPANELSTYLE,7,1,3803)\" font=\"gtf(CONTROLPANELSTYLE,7,1)\" overhang=\"false\"/>\r\n"
    "<Element accessible=\"true\" accRole=\"link\" accdefaction=\"click\" foreground=\"gtc(CONTROLPANELSTYLE,7,1,3803)\" font=\"gtf(CONTROLPANELSTYLE,7,1)\" overhang=\"false\"/>\r\n"
    "<if keyfocused=\"true\">\r\n"
    "<Button contentalign=\"wrapleft | focusrect\"/>\r\n"
    "<Element contentalign=\"wrapleft | focusrect\"/>\r\n"
    "</if>\r\n"
    "<if enabled=\"false\">\r\n"
    "<Button foreground=\"gtc(CONTROLPANELSTYLE,7,4,3803)\" font=\"gtf(CONTROLPANELSTYLE,7,4)\"/>\r\n"
    "<Element foreground=\"gtc(CONTROLPANELSTYLE,7,4,3803)\" font=\"gtf(CONTROLPANELSTYLE,7,4)\"/>\r\n"
    "</if>\r\n"
    "<if mousefocused=\"true\">\r\n"
    "<Button cursor=\"hand\" foreground=\"gtc(CONTROLPANELSTYLE,7,2,3803)\" font=\"gtf(CONTROLPANELSTYLE,7,2)\"/>\r\n"
    "<Element cursor=\"hand\" foreground=\"gtc(CONTROLPANELSTYLE,7,2,3803)\" font=\"gtf(CONTROLPANELSTYLE,7,2)\"/>\r\n"
    "</if>\r\n"
    "</if>\r\n"
    "<if class=\"cp_content_link\">\r\n"
    "<Button accessible=\"true\" accRole=\"link\" accdefaction=\"click\" foreground=\"gtc(CONTROLPANELSTYLE,10,1,3803)\" font=\"gtf(CONTROLPANELSTYLE,10,1)\" overhang=\"false\"/>\r\n"
    "<Element accessible=\"true\" accRole=\"link\" accdefaction=\"click\" foreground=\"gtc(CONTROLPANELSTYLE,10,1,3803)\" font=\"gtf(CONTROLPANELSTYLE,10,1)\" overhang=\"false\"/>\r\n"
    "<if keyfocused=\"true\">\r\n"
    "<Button contentalign=\"wrapleft | focusrect\"/>\r\n"
    "<Element contentalign=\"wrapleft | focusrect\"/>\r\n"
    "</if>\r\n"
    "<if enabled=\"false\">\r\n"
    "<Button foreground=\"gtc(CONTROLPANELSTYLE,10,4,3803)\" font=\"gtf(CONTROLPANELSTYLE,10,4)\"/>\r\n"
    "<Element foreground=\"gtc(CONTROLPANELSTYLE,10,4,3803)\" font=\"gtf(CONTROLPANELSTYLE,10,4)\"/>\r\n"
    "</if>\r\n"
    "<if mousefocused=\"true\">\r\n"
    "<Button cursor=\"hand\" foreground=\"gtc(CONTROLPANELSTYLE,10,2,3803)\" font=\"gtf(CONTROLPANELSTYLE,10,2)\"/>\r\n"
    "<Element cursor=\"hand\" foreground=\"gtc(CONTROLPANELSTYLE,10,2,3803)\" font=\"gtf(CONTROLPANELSTYLE,10,2)\"/>\r\n"
    "</if>\r\n"
    "</if>\r\n"
    "<if class=\"cp_content_divider_header\">\r\n"
    "<Element background=\"themeable(dtb(CONTROLPANEL,2,0),window)\" foreground=\"gtc(CONTROLPANELSTYLE,9,0,3803)\" font=\"gtf(CONTROLPANELSTYLE, 9, 0)\" padding=\"rect(0rp,0rp,2rp,0rp)\" contentalign=\"wrapleft\" accessible=\"true\" accRole=\"statictext\"/>\r\n"
    "</if>\r\n"
    "<if class=\"cp_content_divider_line\">\r\n"
    "<Element height=\"1rp\" width=\"565rp\" background=\"themeable(dtb(CONTROLPANEL,17,0),buttonshadow)\"/>\r\n"
    "</if>\r\n"
    "<if class=\"cp_content_banner_box\">\r\n"
    "<Element padding=\"rect(7rp,7rp,7rp,7rp)\" background=\"themeable(dtb(CONTROLPANEL,18,0),window)\" borderthickness=\"rect(1rp,1rp,1rp,1rp)\" bordercolor=\"gtc(CONTROLPANELSTYLE,17,0,3821)\"/>\r\n"
    "</if>\r\n"
    "<if class=\"cp_command_sink\">\r\n"
    "<Element layoutpos=\"bottom\" background=\"themeable(dtb(CONTROLPANEL,12,0),window)\"/>\r\n"
    "</if>\r\n"
    "<if class=\"cp_command_float\">\r\n"
    "<Element layoutpos=\"top\" background=\"themeable(dtb(CONTROLPANEL,13,0),window)\"/>\r\n"
    "</if>\r\n"
    "<if class=\"cp_command_button_box\">\r\n"
    "<Element padding=\"rect(0rp,10rp,10rp,10rp)\" width=\"600rp\"/> \r\n"
    "</if>\r\n"
    "</style>\r\n"
    "#pragma once\r\n"
    "<style resid=\"healthcenter_style\">\r\n"
    "<if class=\"hc_content_box\">\r\n"
    "<Element padding=\"rect(12rp,5rp,12rp,5rp)\" borderthickness=\"rect(1rp,1rp,1rp,1rp)\" bordercolor=\"gtc(CONTROLPANELSTYLE,17,0,3821)\"/>\r\n"
    "</if>\r\n"
    "<if class=\"hc_highcontrast_content_box\">\r\n"
    "<Element padding=\"rect(12rp,5rp,12rp,5rp)\" borderthickness=\"rect(1rp,1rp,1rp,1rp)\" bordercolor=\"activecaption\"/>\r\n"
    "</if>\r\n"
    "<if class=\"hc_red_content_banner_box\">\r\n"
    "<Element background=\"gradient(RGB(172,1,0),RGB(222,1,0),1)\" width=\"20rp\"/>\r\n"
    "</if>\r\n"
    "<if class=\"hc_yellow_content_banner_box\">\r\n"
    "<Element background=\"gradient(RGB(242,177,0),RGB(255,206,73),1)\" width=\"20rp\"/>\r\n"
    "</if>\r\n"
    "<if class=\"hc_highcontrast_content_banner_box\">\r\n"
    "<Element background=\"activecaption\" width=\"20rp\"/>\r\n"
    "</if>\r\n"
    "<if class=\"cp_content_v_spacer\">\r\n"
    "<Element height=\"7rp\"/>\r\n"
    "</if>\r\n"
    "<if class=\"ExpandoButtonText\">\r\n"
    "<Element font=\"gtf(CONTROLPANELSTYLE, 5, 0)\" foreground=\"gtc(CONTROLPANELSTYLE,7,1,3803)\" contentalign=\"wrapleft\" layoutpos=\"top\"/>\r\n"
    "<if keyfocused=\"true\">\r\n"
    "<Element contentalign=\"wrapleft | focusrect\"/>\r\n"
    "</if>\r\n"
    "<if id=\"atom(ExpandoTextExpanded)\" selected=\"false\">\r\n"
    "<Element layoutpos=\"none\" enabled=\"false\"/>\r\n"
    "</if>\r\n"
    "<if id=\"atom(ExpandoTextCollapsed)\" selected=\"true\">\r\n"
    "<Element layoutpos=\"none\" enabled=\"false\"/>\r\n"
    "</if>\r\n"
    "</if>\r\n"
    "<if class=\"ExpandoStatusText\">\r\n"
    "<Element font=\"gtf(CONTROLPANELSTYLE, 5, 0)\" foreground=\"gtc(CONTROLPANELSTYLE,6,0,3803)\" contentalign=\"wrapleft\" layoutpos=\"top\"/>\r\n"
    "</if>\r\n"
    "<if class=\"GroupHeader\">\r\n"
    "<if mousewithin=\"true\">\r\n"
    "<Element background=\"themeable(dtb(Explorer::ListView,1,2), threedlightshadow)\"/>\r\n"
    "</if>\r\n"
    "</if>\r\n"
    "<if class=\"ExpandButtonNormal\">\r\n"
    "<Button height=\"themeable(gtmet(TaskDialog, 13, 0, 2417), '21rp')\" width=\"themeable(gtmet(TaskDialog, 13, 0, 2416), '19rp')\" background=\"themeable(dtb(TaskDialog, 13, 1), dfc(3, 0x0001))\" margin=\"themeable(gtmar(TaskDialog, 20, 0, 3602), rect(0rp,0rp,10rp,0rp))\"/>\r\n"
    "<if keyfocused=\"true\">\r\n"
    "<Button contentalign=\"focusrect\"/>\r\n"
    "</if>\r\n"
    "<if selected=\"true\">\r\n"
    "<Button background=\"themeable(dtb(TaskDialog, 13, 4),dfc(3, 0x0000))\"/>\r\n"
    "</if>\r\n"
    "</if>\r\n"
    "<if class=\"ExpandButtonHover\">\r\n"
    "<Button height=\"themeable(gtmet(TaskDialog, 13, 0, 2417), '21rp')\" width=\"themeable(gtmet(TaskDialog, 13, 0, 2416), '19rp')\" background=\"themeable(dtb(TaskDialog, 13, 2), dfc(3, 0x0001 | 0x1000))\" margin=\"themeable(gtmar(TaskDialog, 20, 0, 3602), rect(0rp,0rp,10rp,0rp))\"/>\r\n"
    "<if keyfocused=\"true\">\r\n"
    "<Button contentalign=\"focusrect\"/>\r\n"
    "</if>\r\n"
    "<if selected=\"true\">\r\n"
    "<Button background=\"themeable(dtb(TaskDialog, 13, 5), dfc(3, 0x0000 | 0x1000))\"/>\r\n"
    "</if>\r\n"
    "</if>\r\n"
    "<if class=\"ExpandButtonPressed\">\r\n"
    "<Button height=\"themeable(gtmet(TaskDialog, 13, 0, 2417), '21rp')\" width=\"themeable(gtmet(TaskDialog, 13, 0, 2416), '19rp')\" background=\"themeable(dtb(TaskDialog, 13, 3), dfc(3, 0x0001 | 0x0200) )\" margin=\"themeable(gtmar(TaskDialog, 20, 0, 3602), rect(0rp,0rp,10rp,0rp))\"/>\r\n"
    "<if keyfocused=\"true\">\r\n"
    "<Button contentalign=\"focusrect\"/>\r\n"
    "</if>\r\n"
    "<if selected=\"true\">\r\n"
    "<Button background=\"themeable(dtb(TaskDialog, 13, 6), dfc(3, 0x0000| 0x0200))\"/>\r\n"
    "</if>\r\n"
    "</if>\r\n"
    "</style>\r\n"
    "</stylesheets>\r\n"
    "<Element resid=\"RedModule\" layoutpos=\"top\" class=\"hc_content_box\" layout=\"borderlayout()\" accessible=\"true\" accrole=\"pane\" accname=\"Important message\" padding=\"rect(0,0,0,0)\">\r\n"
    "<Element layoutpos=\"client\" id=\"atom(redNotificationBox)\" class=\"hc_content_box\" layout=\"borderlayout()\" padding=\"rect(0,0,0,0)\">\r\n"
    "<Element id=\"atom(redBanner)\" class=\"hc_red_content_banner_box\" layoutpos=\"left\"/>\r\n"
    "<Element layoutpos=\"client\" layout=\"borderlayout()\" padding=\"rect(12rp,14rp,14rp,7rp)\">\r\n"
    "<Element layoutpos=\"top\" layout=\"borderlayout()\">\r\n"
    "<Element layoutpos=\"bottom\" class=\"cp_content_v_spacer\"/>\r\n"
    "<Element layoutpos=\"right\" layout=\"flowlayout(0,2,1,3)\" sheet=\"cp_style\" contentalign=\"middleright\">\r\n"
    "<viewer>\r\n"
    "<NavigateButton id=\"atom(ActionButton)\" layout=\"borderlayout()\" layoutpos=\"right\">\r\n"
    "<CCPushButton id=\"atom(ButtonData)\" layoutpos=\"right\" active=\"mouse | keyboard\" accessible=\"true\" accrole=\"pushbutton\"/>\r\n"
    "</NavigateButton>\r\n"
    "</viewer>\r\n"
    "</Element>\r\n"
    "<Element layoutpos=\"left\" layout=\"borderlayout()\" sheet=\"cp_style\">\r\n"
    "<Element layoutpos=\"top\" layout=\"borderlayout()\">\r\n"
    "<Element layoutpos=\"top\" layout=\"flowlayout(1,0,0,0)\">\r\n"
    "<Element class=\"cp_content_title_text\" layoutpos=\"left\" id=\"atom(TitleData)\" accessible=\"true\" accrole=\"text\" padding=\"rect(0,0,5rp,0)\"/>\r\n"
    "<Element class=\"cp_content_title_text\" layoutpos=\"right\" id=\"atom(NotificationType)\" accessible=\"true\" accrole=\"text\"/>\r\n"
    "</Element>\r\n"
    "<Element layoutpos=\"top\" class=\"cp_content_v_spacer\"/>\r\n"
    "<Element layoutpos=\"top\" layout=\"flowlayout(0,0,0,0)\">\r\n"
    "<Element id=\"atom(DetailIcon)\" layoutpos=\"left\" accessible=\"true\" accrole=\"graphic\" margin=\"rect(0rp,0rp,5rp,0rp)\"/>\r\n"
    "<Element class=\"cp_content_text\" layoutpos=\"right\" id=\"atom(DetailData)\" contentalign=\"wrapleft\" accessible=\"true\" accrole=\"text\"/>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "<Element layoutpos=\"top\" layout=\"tablelayout(0,0,0,0,-60,1,0,-40)\" sheet=\"cp_style\">\r\n"
    "<NavigateButton layout=\"borderlayout()\">\r\n"
    "<Button class=\"cp_content_link\" id=\"atom(TurnOffWarning)\" layoutpos=\"left\" content=\"resstr(501)\" accessible=\"true\" accrole=\"link\"/>\r\n"
    "</NavigateButton>\r\n"
    "<Element layout=\"verticalflowlayout(1,1,1,2)\" layoutpos=\"right\">\r\n"
    "<NavigateButton layout=\"borderlayout()\">\r\n"
    "<Button class=\"cp_content_link\" id=\"atom(SecondaryLink)\" contentalign=\"wrapright\" accessible=\"true\" accrole=\"link\"/>\r\n"
    "</NavigateButton>\r\n"
    "<NavigateButton layout=\"borderlayout()\" layoutpos=\"right\">\r\n"
    "<Button class=\"cp_content_link\" id=\"atom(HelpNotification)\" contentalign=\"wrapright\" accessible=\"true\" accrole=\"link\"/>\r\n"
    "</NavigateButton>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "<Element layoutpos=\"bottom\" class=\"cp_content_v_spacer\"/>\r\n"
    "</Element>\r\n"
    "<Element resid=\"YellowModule\" layoutpos=\"top\" class=\"hc_content_box\" layout=\"borderlayout()\" accessible=\"true\" accrole=\"pane\" accname=\"Message\" padding=\"rect(0,0,0,0)\">\r\n"
    "<Element layoutpos=\"client\" id=\"atom(yellowNotificationBox)\" class=\"hc_content_box\" layout=\"borderlayout()\" padding=\"rect(0,0,0,0)\">\r\n"
    "<Element id=\"atom(yellowBanner)\" class=\"hc_red_content_banner_box\" layoutpos=\"left\"/>\r\n"
    "<Element layoutpos=\"client\" layout=\"borderlayout()\" padding=\"rect(12rp,14rp,14rp,7rp)\">\r\n"
    "<Element layoutpos=\"top\" layout=\"borderlayout()\">\r\n"
    "<Element layoutpos=\"bottom\" class=\"cp_content_v_spacer\"/>\r\n"
    "<Element layoutpos=\"right\" layout=\"flowlayout(0,2,1,3)\" sheet=\"cp_style\" contentalign=\"middleright\">\r\n"
    "<viewer>\r\n"
    "<NavigateButton id=\"atom(ActionButton)\" layout=\"borderlayout()\" layoutpos=\"right\">\r\n"
    "<CCPushButton id=\"atom(ButtonData)\" layoutpos=\"right\" active=\"mouse | keyboard\" accessible=\"true\" accrole=\"pushbutton\"/>\r\n"
    "</NavigateButton>\r\n"
    "</viewer>\r\n"
    "</Element>\r\n"
    "<Element layoutpos=\"left\" layout=\"borderlayout()\" sheet=\"cp_style\">\r\n"
    "<Element layoutpos=\"top\" layout=\"borderlayout()\">\r\n"
    "<Element class=\"cp_content_title_text\" layoutpos=\"top\" id=\"atom(TitleData)\" accessible=\"true\" accrole=\"text\"/>\r\n"
    "<Element layoutpos=\"top\" class=\"cp_content_v_spacer\"/>\r\n"
    "<Element layoutpos=\"top\" layout=\"flowlayout(0,0,0,0)\">\r\n"
    "<Element id=\"atom(DetailIcon)\" layoutpos=\"left\" accessible=\"true\" accrole=\"graphic\" margin=\"rect(0rp,0rp,5rp,0rp)\"/>\r\n"
    "<Element class=\"cp_content_text\" layoutpos=\"right\" id=\"atom(DetailData)\" contentalign=\"wrapleft\" accessible=\"true\" accrole=\"text\"/>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "<Element layoutpos=\"top\" layout=\"tablelayout(0,0,0,0,-60,1,0,-40)\" sheet=\"cp_style\">\r\n"
    "<NavigateButton layout=\"borderlayout()\">\r\n"
    "<Button class=\"cp_content_link\" id=\"atom(TurnOffWarning)\" layoutpos=\"left\" content=\"resstr(501)\" accessible=\"true\" accrole=\"link\"/>\r\n"
    "</NavigateButton>\r\n"
    "<Element layout=\"verticalflowlayout(1,1,1,2)\">\r\n"
    "<NavigateButton layout=\"borderlayout()\" layoutpos=\"right\">\r\n"
    "<Button class=\"cp_content_link\" id=\"atom(SecondaryLink)\" contentalign=\"wrapright\" accessible=\"true\" accrole=\"link\"/>\r\n"
    "</NavigateButton>\r\n"
    "<NavigateButton layout=\"borderlayout()\" layoutpos=\"right\">\r\n"
    "<Button class=\"cp_content_link\" id=\"atom(HelpNotification)\" contentalign=\"wrapright\" accessible=\"true\" accrole=\"link\"/>\r\n"
    "</NavigateButton>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "<Element layoutpos=\"bottom\" class=\"cp_content_v_spacer\"/>\r\n"
    "</Element>\r\n"
    "<Element resid=\"CheckModule\" layoutpos=\"top\" layout=\"borderlayout()\" accessible=\"true\" accrole=\"pane\" accname=\"Check\" sheet=\"cp_style\" padding=\"rect(0,0,0,20rp)\">\r\n"
    "<Element layoutpos=\"top\" layout=\"tablelayout(0,0,3,0,-70)\">\r\n"
    "<Element layoutpos=\"top\" layout=\"flowlayout(0,0,3,0)\" margin=\"rect(0,7rp,0,0)\">\r\n"
    "<Element class=\"cp_content_text\" id=\"atom(CheckTitle)\" contentalign=\"wrapleft\" padding=\"rect(0,0,7rp,0)\" accessible=\"true\" accrole=\"text\"/>\r\n"
    "<Element class=\"cp_content_text\" id=\"atom(CheckStatus)\" contentalign=\"wrapright\" accessible=\"true\" accrole=\"text\"/>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "<Element layoutpos=\"top\" layout=\"borderlayout()\" padding=\"rect(20rp,7rp,0,0)\">\r\n"
    "<Element layoutpos=\"top\" layout=\"flowlayout(0,2,0,0)\" margin=\"rect(20rp,7rp,0rp,0rp)\">\r\n"
    "<Element id=\"atom(CheckIcon)\" layoutpos=\"left\" accessible=\"true\" accrole=\"graphic\" margin=\"rect(0rp,0rp,5rp,0rp)\"/>\r\n"
    "<Element class=\"cp_content_text\" id=\"atom(CheckDescription)\" layoutpos=\"left\" accessible=\"true\" accrole=\"text\"/>\r\n"
    "</Element>\r\n"
    "<Element layoutpos=\"top\" layout=\"flowlayout()\" margin=\"rect(20rp,7rp,0rp,0rp)\">\r\n"
    "<NavigateButton layout=\"flowlayout(0,2,0,2)\">\r\n"
    "<Element layoutPos=\"left\" id=\"atom(CheckLinkIcon1)\" content=\"icon(78,sysmetric(49),sysmetric(50),library(imageres.dll))\" contentalign=\"middleleft\" accessible=\"true\" accrole=\"graphic\" accname=\"resstr(6)\" margin=\"rect(0rp,0rp,5rp,0rp)\"/>\r\n"
    "<Button layoutPos=\"left\" class=\"cp_content_link\" id=\"atom(CheckLink1)\" layoutpos=\"right\" accessible=\"true\" accrole=\"link\"/>\r\n"
    "</NavigateButton>\r\n"
    "<NavigateButton layout=\"flowlayout(0,2,0,2)\">\r\n"
    "<Element class=\"cp_content_text\" padding=\"rect(5rp,0rp,5rp,0rp)\" layoutpos=\"left\" accessible=\"false\" content=\"resstr(539)\"/>\r\n"
    "<Element layoutPos=\"left\" id=\"atom(CheckLinkIcon2)\" content=\"icon(78,sysmetric(49),sysmetric(50),library(imageres.dll))\" contentalign=\"middleleft\" accessible=\"true\" accrole=\"graphic\" accname=\"resstr(6)\" margin=\"rect(0rp,0rp,5rp,0rp)\"/>\r\n"
    "<Button layoutPos=\"left\" class=\"cp_content_link\" id=\"atom(CheckLink2)\" layoutpos=\"top\" accessible=\"true\" accrole=\"link\"/>\r\n"
    "</NavigateButton>\r\n"
    "<NavigateButton layout=\"flowlayout(0,2,0,2)\">\r\n"
    "<Element class=\"cp_content_text\" padding=\"rect(5rp,0rp,5rp,0rp)\" layoutpos=\"left\" accessible=\"false\" content=\"resstr(539)\"/>\r\n"
    "<Element layoutPos=\"left\" id=\"atom(CheckLinkIcon3)\" content=\"icon(78,sysmetric(49),sysmetric(50),library(imageres.dll))\" contentalign=\"middleleft\" accessible=\"true\" accrole=\"graphic\" accname=\"resstr(6)\" margin=\"rect(0rp,0rp,5rp,0rp)\"/>\r\n"
    "<Button layoutPos=\"left\" class=\"cp_content_link\" id=\"atom(CheckLink3)\" layoutpos=\"top\" accessible=\"true\" accrole=\"link\"/>\r\n"
    "</NavigateButton>\r\n"
    "<NavigateButton layout=\"flowlayout(0,2,0,2)\">\r\n"
    "<Element class=\"cp_content_text\" padding=\"rect(5rp,0rp,5rp,0rp)\" layoutpos=\"left\" accessible=\"false\" content=\"resstr(539)\"/>\r\n"
    "<Element layoutPos=\"left\" id=\"atom(CheckLinkIcon4)\" content=\"icon(78,sysmetric(49),sysmetric(50),library(imageres.dll))\" contentalign=\"middleleft\" accessible=\"true\" accrole=\"graphic\" accname=\"resstr(6)\" margin=\"rect(0rp,0rp,5rp,0rp)\"/>\r\n"
    "<Button layoutPos=\"left\" class=\"cp_content_link\" id=\"atom(CheckLink4)\" layoutpos=\"top\" accessible=\"true\" accrole=\"link\"/>\r\n"
    "</NavigateButton>\r\n"
    "</Element>\r\n"
    "<NavigateButton layout=\"borderlayout()\" layoutpos=\"top\" margin=\"rect(12rp,7rp,0rp,0rp)\">\r\n"
    "<Button class=\"cp_content_link\" id=\"atom(CheckHelp)\" layoutpos=\"left\" accessible=\"true\" accrole=\"link\"/>\r\n"
    "</NavigateButton>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "<HealthCenterCPLPage resid=\"main\" id=\"atom(Hub)\" layout=\"borderlayout()\">\r\n"
    "<Element sheet=\"cp_style\" class=\"cp_topbox\" layout=\"borderlayout()\" layoutpos=\"client\">\r\n"
    "<ScrollViewer xscrollable=\"false\" layoutpos=\"client\" sheet=\"common\">\r\n"
    "<Element layout=\"borderlayout()\" sheet=\"cp_style\" class=\"cp_hub_frame\" width=\"652rp\" padding=\"rect(0,0,0,0)\">\r\n"
    "<Element layoutpos=\"top\" layout=\"borderlayout()\">\r\n"
    "<Viewer layoutpos=\"right\">\r\n"
    "<Button class=\"cp_help_glyph\" id=\"atom(helpHub)\" accessible=\"true\" accrole=\"graphic\" accname=\"resstr(7)\"/>\r\n"
    "</Viewer>\r\n"
    "</Element>\r\n"
    "<FocusIndicator id=\"atom(FocusIndicator)\" firsttabtarget=\"atom(startLinks)\"/>\r\n"
    "<Element id=\"atom(startLinks)\" class=\"cp_cotent_pane\" layoutpos=\"left\" layout=\"borderlayout()\" padding=\"rect(26rp,0,26rp,20rp)\" accessible=\"true\" accname=\"Action Center panel\" accrole=\"pane\">\r\n"
    "<Element layoutpos=\"top\" layout=\"flowlayout()\">\r\n"
    "<Element class=\"cp_content_instruction\" accessible=\"true\" accrole=\"text\" content=\"resstr(4)\"/>\r\n"
    "</Element>\r\n"
    "<Element layoutpos=\"top\" class=\"cp_content_v_spacer\"/>\r\n"
    "<Element id=\"atom(NotInitialized)\" layoutpos=\"top\" layout=\"flowlayout()\">\r\n"
    "<Element class=\"cp_content_text\" content=\"resstr(500)\"/>\r\n"
    "</Element>\r\n"
    "<Element id=\"atom(NoActions)\" layoutpos=\"top\" layout=\"flowlayout()\">\r\n"
    "<Element class=\"cp_content_text\" content=\"resstr(502)\"/>\r\n"
    "</Element>\r\n"
    "<Element id=\"atom(AtRisk)\" layoutpos=\"top\" layout=\"flowlayout()\">\r\n"
    "<Element class=\"cp_content_text\" content=\"resstr(503)\"/>\r\n"
    "</Element>\r\n"
    "<Element layoutpos=\"top\" class=\"cp_content_v_spacer\"/>\r\n"
    "<Element layoutpos=\"top\" class=\"cp_content_v_spacer\"/>\r\n"
    "<Element layoutpos=\"top\" class=\"cp_content_v_spacer\"/>\r\n"
    "<Element layoutpos=\"top\" contentalign=\"middlecenter\" layout=\"borderlayout()\" accessible=\"true\" accrole=\"pane\" accname=\"Security section\">\r\n"
    "<Element layoutpos=\"top\" class=\"cp_content_divider_line\" width=\"700rp\"/>\r\n"
    "<WHCExpando id=\"atom(SecurityGroupExpando)\" sheet=\"healthcenter_style\" layout=\"borderlayout()\" layoutpos=\"top\">\r\n"
    "<Element id=\"atom(HeaderButton)\" class=\"GroupHeader\" layout=\"borderlayout()\" layoutpos=\"top\" padding=\"rect(0,7rp,0,7rp)\">\r\n"
    "<Accessiblebutton id=\"atom(SecurityGroupHeading)\" layout=\"borderlayout()\" layoutpos=\"top\" background=\"argb(0,0,0,0)\" accessible=\"true\" accname=\"resstr(564)\" accrole=\"pushbutton\">\r\n"
    "<Element layout=\"flowlayout(0,2,0,2)\" layoutpos=\"left\" padding=\"rect(10rp,0rp,0rp,0rp)\">\r\n"
    "<Element class=\"ExpandoButtonText\" content=\"resstr(509)\" shortcut=\"auto\"/>\r\n"
    "</Element>\r\n"
    "<Accessiblebutton id=\"atom(arrow)\" layoutpos=\"right\" layout=\"flowlayout()\" padding=\"rect(12rp,0,12rp,0)\" accessible=\"true\" accrole=\"pushbutton\" accname=\"resstr(511)\" accdesc=\"resstr(512)\">\r\n"
    "<Button id=\"atom(ExpandoButtonImage)\" class=\"ExpandButtonNormal\" layoutpos=\"top\" active=\"inactive\"/>\r\n"
    "</Accessiblebutton>\r\n"
    "</Accessiblebutton>\r\n"
    "</Element>\r\n"
    "<WHCRepeater id=\"atom(RedSecurityList)\" expand=\"RedModule\" layout=\"borderlayout()\" layoutpos=\"top\">\r\n"
    "<bind connect=\"ButtonData\" property=\"ButtonData\"/>\r\n"
    "<bind connect=\"TitleData\" property=\"TitleData\"/>\r\n"
    "<bind connect=\"NotificationType\" property=\"NotificationType\"/>\r\n"
    "<bind connect=\"DetailData\" property=\"DetailData\"/>\r\n"
    "<bind connect=\"SecondaryLink\" property=\"SecondaryLink\"/>\r\n"
    "<bind connect=\"DetailIcon\" property=\"DetailIcon\"/>\r\n"
    "<bind connect=\"HelpNotification\" property=\"HelpNotification\"/>\r\n"
    "<bind connect=\"TurnOffWarning\" property=\"TurnOffWarning\"/>\r\n"
    "</WHCRepeater>\r\n"
    "<Element id=\"atom(SecurityYellowExpanded)\" layoutpos=\"top\" layout=\"borderlayout()\">\r\n"
    "<WHCRepeater id=\"atom(YellowSecurityList)\" expand=\"YellowModule\" layout=\"borderlayout()\" layoutpos=\"top\">\r\n"
    "<bind connect=\"ButtonData\" property=\"ButtonData\"/>\r\n"
    "<bind connect=\"TitleData\" property=\"TitleData\"/>\r\n"
    "<bind connect=\"DetailData\" property=\"DetailData\"/>\r\n"
    "<bind connect=\"SecondaryLink\" property=\"SecondaryLink\"/>\r\n"
    "<bind connect=\"DetailIcon\" property=\"DetailIcon\"/>\r\n"
    "<bind connect=\"HelpNotification\" property=\"HelpNotification\"/>\r\n"
    "<bind connect=\"TurnOffWarning\" property=\"TurnOffWarning\"/>\r\n"
    "</WHCRepeater>\r\n"
    "</Element>\r\n"
    "<Element id=\"atom(SecurityOverflow)\" layoutpos=\"top\" layout=\"borderlayout()\" padding=\"rect(0,0,0,0)\" accessible=\"true\" accname=\"Overflow Notifications\" accrole=\"pane\">\r\n"
    "<Element id=\"atom(SecurityOverflowBox)\" class=\"hc_content_box\" layoutpos=\"top\" padding=\"rect(0,0,0,0)\" layout=\"borderlayout()\">\r\n"
    "<Element id=\"atom(SecurityOverflowBanner)\" class=\"hc_yellow_content_banner_box\" layoutpos=\"left\"/>\r\n"
    "<Element layoutpos=\"client\" layout=\"borderlayout()\" padding=\"rect(12rp,14rp,12rp,14rp)\">\r\n"
    "<Element layoutpos=\"right\" layout=\"borderlayout()\" sheet=\"cp_style\">\r\n"
    "<Element layoutpos=\"top\" layout=\"flowlayout(0,2,1,2)\">\r\n"
    "<viewer>\r\n"
    "<NavigateButton layout=\"borderlayout()\" layoutpos=\"left\">\r\n"
    "<CCPushButton id=\"atom(SecurityExpandOverflow)\" content=\"resstr(513)\" layoutpos=\"right\" active=\"mouse | keyboard\"/>\r\n"
    "</NavigateButton>\r\n"
    "</viewer>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "<Element layoutpos=\"left\" layout=\"borderlayout()\" sheet=\"cp_style\">\r\n"
    "<Element class=\"cp_content_text\" layoutpos=\"top\" id=\"atom(SecurityOverflowText)\"/>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "<Element layoutpos=\"bottom\" class=\"cp_content_v_spacer\"/>\r\n"
    "</Element>\r\n"
    "<Element id=\"atom(clipper)\" sheet=\"cp_style\" class=\"cp_content_pane\" layoutpos=\"bottom\" layout=\"borderlayout()\" padding=\"rect(12rp,10rp,0,7rp)\" animation=\"rectangleV | s | fast\">\r\n"
    "<WHCRepeater id=\"atom(SecurityCheckModule)\" expand=\"CheckModule\" layout=\"borderlayout()\" layoutpos=\"top\">\r\n"
    "<bind connect=\"CheckTitle\" property=\"CheckTitle\"/>\r\n"
    "<bind connect=\"CheckStatus\" property=\"CheckStatus\"/>\r\n"
    "<bind connect=\"CheckDescription\" property=\"CheckDescription\"/>\r\n"
    "<bind connect=\"CheckLink1\" property=\"CheckLink1\"/>\r\n"
    "<bind connect=\"CheckLinkIcon1\" property=\"CheckLinkIcon1\"/>\r\n"
    "<bind connect=\"CheckLink2\" property=\"CheckLink2\"/>\r\n"
    "<bind connect=\"CheckLinkIcon2\" property=\"CheckLinkIcon2\"/>\r\n"
    "<bind connect=\"CheckLink3\" property=\"CheckLink3\"/>\r\n"
    "<bind connect=\"CheckLinkIcon3\" property=\"CheckLinkIcon3\"/>\r\n"
    "<bind connect=\"CheckLink4\" property=\"CheckLink4\"/>\r\n"
    "<bind connect=\"CheckLinkIcon4\" property=\"CheckLinkIcon4\"/>\r\n"
    "<bind connect=\"CheckIcon\" property=\"CheckIcon\"/>\r\n"
    "<bind connect=\"CheckHelp\" property=\"CheckHelp\"/>\r\n"
    "</WHCRepeater>\r\n"
    "<CCSysLink id=\"atom(securityHelpLink)\" class=\"cp_content_text\" layoutpos=\"top\" content=\"resstr(37)\" accessible=\"true\" accname=\"resstr(555)\" accrole=\"link\" margin=\"rect(0,7rp,0,7rp)\"/>\r\n"
    "</Element>\r\n"
    "</WHCExpando>\r\n"
    "<Element id=\"atom(SecurityBottomLine)\" layoutpos=\"top\" class=\"cp_content_divider_line\" width=\"700rp\" animation=\"position | s | fast\"/>\r\n"
    "</Element>\r\n"
    "<Element id=\"atom(SectionBelow)\" layoutpos=\"top\" background=\"themeable(dtb(CONTROLPANEL,2,0),window)\" contentalign=\"middlecenter\" layout=\"borderlayout()\" accessible=\"true\" accrole=\"pane\" accname=\"Maintenance section\" animation=\"position | s | fast\">\r\n"
    "<WHCExpando id=\"atom(MaintenanceGroupExpando)\" sheet=\"healthcenter_style\" layout=\"borderlayout()\" layoutpos=\"top\">\r\n"
    "<Element id=\"atom(HeaderButton)\" class=\"GroupHeader\" layout=\"borderlayout()\" layoutpos=\"top\" padding=\"rect(0,7rp,0,7rp)\">\r\n"
    "<Accessiblebutton id=\"atom(MaintenanceGroupHeading)\" layout=\"borderlayout()\" layoutpos=\"top\" background=\"argb(0,0,0,0)\" accessible=\"true\" accname=\"resstr(565)\" accrole=\"pushbutton\">\r\n"
    "<Element layout=\"flowlayout(0,2,0,2)\" layoutpos=\"left\" padding=\"rect(10rp,0rp,0,0rp)\">\r\n"
    "<Element class=\"ExpandoButtonText\" content=\"resstr(504)\" shortcut=\"auto\"/>\r\n"
    "</Element>\r\n"
    "<Accessiblebutton id=\"atom(arrow)\" layoutpos=\"right\" layout=\"flowlayout()\" padding=\"rect(12rp,0,12rp,0)\" accessible=\"true\" accrole=\"pushbutton\" accname=\"resstr(506)\" accdesc=\"resstr(507)\">\r\n"
    "<Button id=\"atom(ExpandoButtonImage)\" class=\"ExpandButtonNormal\" layoutpos=\"top\" active=\"inactive\"/>\r\n"
    "</Accessiblebutton>\r\n"
    "</Accessiblebutton>\r\n"
    "</Element>\r\n"
    "<WHCRepeater id=\"atom(RedMaintenanceList)\" expand=\"RedModule\" layout=\"borderlayout()\" layoutpos=\"top\">\r\n"
    "<bind connect=\"ButtonData\" property=\"ButtonData\"/>\r\n"
    "<bind connect=\"TitleData\" property=\"TitleData\"/>\r\n"
    "<bind connect=\"NotificationType\" property=\"NotificationType\"/>\r\n"
    "<bind connect=\"DetailData\" property=\"DetailData\"/>\r\n"
    "<bind connect=\"SecondaryLink\" property=\"SecondaryLink\"/>\r\n"
    "<bind connect=\"DetailIcon\" property=\"DetailIcon\"/>\r\n"
    "<bind connect=\"HelpNotification\" property=\"HelpNotification\"/>\r\n"
    "<bind connect=\"TurnOffWarning\" property=\"TurnOffWarning\"/>\r\n"
    "</WHCRepeater>\r\n"
    "<Element id=\"atom(MaintenanceYellowExpanded)\" layoutpos=\"top\" layout=\"borderlayout()\">\r\n"
    "<WHCRepeater id=\"atom(YellowMaintenanceList)\" expand=\"YellowModule\" layout=\"borderlayout()\" layoutpos=\"top\">\r\n"
    "<bind connect=\"ButtonData\" property=\"ButtonData\"/>\r\n"
    "<bind connect=\"TitleData\" property=\"TitleData\"/>\r\n"
    "<bind connect=\"DetailData\" property=\"DetailData\"/>\r\n"
    "<bind connect=\"SecondaryLink\" property=\"SecondaryLink\"/>\r\n"
    "<bind connect=\"DetailIcon\" property=\"DetailIcon\"/>\r\n"
    "<bind connect=\"HelpNotification\" property=\"HelpNotification\"/>\r\n"
    "<bind connect=\"TurnOffWarning\" property=\"TurnOffWarning\"/>\r\n"
    "</WHCRepeater>\r\n"
    "</Element>\r\n"
    "<Element id=\"atom(MaintenanceOverflow)\" layoutpos=\"top\" layout=\"borderlayout()\" padding=\"rect(0,0,0,0)\" accessible=\"true\" accname=\"Overflow Notifications\" accrole=\"pane\">\r\n"
    "<Element id=\"atom(MaintenanceOverflowBox)\" class=\"hc_content_box\" layoutpos=\"top\" padding=\"rect(0,0,0,0)\" layout=\"borderlayout()\">\r\n"
    "<Element id=\"atom(MaintenanceOverflowBanner)\" class=\"hc_yellow_content_banner_box\" layoutpos=\"left\"/>\r\n"
    "<Element layoutpos=\"client\" layout=\"borderlayout()\" padding=\"rect(12rp,14rp,12rp,14rp)\">\r\n"
    "<Element layoutpos=\"right\" layout=\"borderlayout()\" sheet=\"cp_style\">\r\n"
    "<Element layoutpos=\"top\" layout=\"flowlayout(0,2,1,2)\">\r\n"
    "<viewer>\r\n"
    "<NavigateButton layout=\"borderlayout()\" layoutpos=\"left\">\r\n"
    "<CCPushButton id=\"atom(MaintenanceExpandOverflow)\" content=\"resstr(508)\" layoutpos=\"right\" active=\"mouse | keyboard\"/>\r\n"
    "</NavigateButton>\r\n"
    "</viewer>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "<Element layoutpos=\"left\" layout=\"borderlayout()\" sheet=\"cp_style\">\r\n"
    "<Element class=\"cp_content_text\" layoutpos=\"top\" id=\"atom(MaintenanceOverflowText)\"/>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "<Element layoutpos=\"bottom\" class=\"cp_content_v_spacer\"/>\r\n"
    "</Element>\r\n"
    "<Element id=\"atom(clipper)\" sheet=\"cp_style\" class=\"cp_content_pane\" layoutpos=\"bottom\" layout=\"borderlayout()\" padding=\"rect(12rp,10rp,0,7rp)\" animation=\"rectangleV | s | fast\">\r\n"
    "<WHCRepeater id=\"atom(MaintenanceCheckModule)\" expand=\"CheckModule\" layout=\"borderlayout()\" layoutpos=\"top\">\r\n"
    "<bind connect=\"CheckTitle\" property=\"CheckTitle\"/>\r\n"
    "<bind connect=\"CheckStatus\" property=\"CheckStatus\"/>\r\n"
    "<bind connect=\"CheckDescription\" property=\"CheckDescription\"/>\r\n"
    "<bind connect=\"CheckLink1\" property=\"CheckLink1\"/>\r\n"
    "<bind connect=\"CheckLinkIcon1\" property=\"CheckLinkIcon1\"/>\r\n"
    "<bind connect=\"CheckLink2\" property=\"CheckLink2\"/>\r\n"
    "<bind connect=\"CheckLinkIcon2\" property=\"CheckLinkIcon2\"/>\r\n"
    "<bind connect=\"CheckLink3\" property=\"CheckLink3\"/>\r\n"
    "<bind connect=\"CheckLinkIcon3\" property=\"CheckLinkIcon3\"/>\r\n"
    "<bind connect=\"CheckLink4\" property=\"CheckLink4\"/>\r\n"
    "<bind connect=\"CheckLinkIcon4\" property=\"CheckLinkIcon4\"/>\r\n"
    "<bind connect=\"CheckIcon\" property=\"CheckIcon\"/>\r\n"
    "<bind connect=\"CheckHelp\" property=\"CheckHelp\"/>\r\n"
    "</WHCRepeater>\r\n"
    "</Element>\r\n"
    "</WHCExpando>\r\n"
    "<Element id=\"atom(MaintenanceBottomLine)\" layoutpos=\"top\" class=\"cp_content_divider_line\" width=\"700rp\" animation=\"position | s | fast\"/>\r\n"
    "</Element>\r\n"
    "<Element id=\"atom(HavingAProblem)\" background=\"themeable(dtb(CONTROLPANEL,2,0),window)\" layoutpos=\"top\" layout=\"borderlayout()\" accessible=\"true\" accrole=\"pane\" accname=\"Solution box\" animation=\"position | s | fast\">\r\n"
    "<Element layoutpos=\"top\" class=\"cp_content_v_spacer\"/>\r\n"
    "<Element layoutpos=\"top\" class=\"cp_content_v_spacer\"/>\r\n"
    "<Element layoutpos=\"top\" class=\"cp_content_v_spacer\"/>\r\n"
    "<Element id=\"atom(SolutonBoxHeading)\" class=\"cp_content_text\" layoutpos=\"top\" content=\"If the problem isn't listed, try one of these:\"/>\r\n"
    "<Element id=\"atom(WhStaticPatched)\" layoutpos=\"none\" width=\"0\" height=\"0\"/>\r\n"
    "<Element layout=\"gridlayout(1,2)\" layoutpos=\"top\" padding=\"rect(0,21rp,0,0)\" background=\"themeable(dtb(CONTROLPANEL,2,0),window)\">\r\n"
    "<Element layout=\"flowlayout(0,0,0,0)\" padding=\"rect(12rp,0,12rp,0)\">\r\n"
    "<NavigateButton layout=\"borderlayout()\" layoutpos=\"left\" shellexecute=\"%SystemRoot%\\\\explorer.exe\" shellexecuteparams=\"shell:::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}\">\r\n"
    "<Button class=\"cp_content_link\" id=\"atom(RunTroubleshooting)\" layoutpos=\"left\" content=\"icon(1021, 48rp, 48rp, library(imageres.dll))\" accessible=\"true\" accname=\"Troubleshooting\" accrole=\"pushbutton\"/>\r\n"
    "</NavigateButton>\r\n"
    "<Element layout=\"borderlayout()\" layoutpos=\"right\" padding=\"rect(12rp,7rp,0,0)\">\r\n"
    "<NavigateButton layout=\"borderlayout()\" layoutpos=\"top\" shellexecute=\"%SystemRoot%\\\\explorer.exe\" shellexecuteparams=\"shell:::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}\">\r\n"
    "<Button class=\"cp_content_link\" id=\"atom(RunTroubleshooting)\" layoutpos=\"left\" content=\"Troubleshooting\" shortcut=\"auto\" accessible=\"true\" accrole=\"link\"/>\r\n"
    "</NavigateButton>\r\n"
    "<Element class=\"cp_content_text\" layoutpos=\"top\" content=\"Find and fix problems with your computer.\" padding=\"rect(0,5rp,0,0)\"/>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "<Element id=\"atom(SolutonBoxRecovery)\" layout=\"flowlayout(0,0,0,0)\" padding=\"rect(12rp,0,12rp,0)\">\r\n"
    "<NavigateButton layout=\"borderlayout()\" layoutpos=\"left\" shellexecute=\"%SystemRoot%\\\\System32\\\\control.exe\" shellexecuteparams=\"/name Microsoft.Recovery\">\r\n"
    "<Button class=\"cp_content_link\" id=\"atom(RestoreYourPC)\" layoutpos=\"left\" content=\"icon(1022, 48rp, 48rp, library(imageres.dll))\" accessible=\"true\" accname=\"Recovery\" accrole=\"pushbutton\"/>\r\n"
    "</NavigateButton>\r\n"
    "<Element layout=\"borderlayout()\" layoutpos=\"right\" padding=\"rect(12rp,7rp,25rp,0)\">\r\n"
    "<NavigateButton layout=\"borderlayout()\" layoutpos=\"top\" shellexecute=\"%SystemRoot%\\\\System32\\\\control.exe\" shellexecuteparams=\"/name Microsoft.Recovery\">\r\n"
    "<Button class=\"cp_content_link\" id=\"atom(RestoreYourPC)\" layoutpos=\"left\" content=\"Recovery\" shortcut=\"auto\" accessible=\"true\" accrole=\"link\"/>\r\n"
    "</NavigateButton>\r\n"
    "<Element class=\"cp_content_text\" layoutpos=\"top\" content=\"Refresh your PC without affecting your files, or reset it and start over.\" padding=\"rect(0,5rp,0,0)\"/>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "</ScrollViewer>\r\n"
    "</Element>\r\n"
    "</HealthCenterCPLPage>\r\n"
    "</duixml>\r\n";

static const char g_solutionBlock[] =
    "<Element id=\"atom(HavingAProblem)\" background=\"themeable(dtb(CONTROLPANEL,2,0),window)\" layoutpos=\"top\" layout=\"borderlayout()\" accessible=\"true\" accrole=\"pane\" accname=\"Solution box\" animation=\"position | s | fast\">\r\n"
    "<Element layoutpos=\"top\" class=\"cp_content_v_spacer\"/>\r\n"
    "<Element layoutpos=\"top\" class=\"cp_content_v_spacer\"/>\r\n"
    "<Element layoutpos=\"top\" class=\"cp_content_v_spacer\"/>\r\n"
    "<Element id=\"atom(SolutonBoxHeading)\" class=\"cp_content_text\" layoutpos=\"top\" content=\"@@WH_HEADING@@\"/>\r\n"
    "<Element id=\"atom(WhStaticPatched)\" layoutpos=\"none\" width=\"0\" height=\"0\"/>\r\n"
    "<Element layout=\"gridlayout(1,2)\" layoutpos=\"top\" padding=\"rect(0,21rp,0,0)\" background=\"themeable(dtb(CONTROLPANEL,2,0),window)\">\r\n"
    "<Element layout=\"flowlayout(0,0,0,0)\" padding=\"rect(12rp,0,12rp,0)\">\r\n"
    "<NavigateButton layout=\"borderlayout()\" layoutpos=\"left\" shellexecute=\"%SystemRoot%\\\\explorer.exe\" shellexecuteparams=\"shell:::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}\">\r\n"
    "<Button class=\"cp_content_link\" id=\"atom(RunTroubleshooting)\" layoutpos=\"left\" content=\"icon(1021, 48rp, 48rp, library(imageres.dll))\" accessible=\"true\" accname=\"@@WH_TS_TITLE@@\" accrole=\"pushbutton\"/>\r\n"
    "</NavigateButton>\r\n"
    "<Element layout=\"borderlayout()\" layoutpos=\"right\" padding=\"rect(12rp,7rp,0,0)\">\r\n"
    "<NavigateButton layout=\"borderlayout()\" layoutpos=\"top\" shellexecute=\"%SystemRoot%\\\\explorer.exe\" shellexecuteparams=\"shell:::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}\">\r\n"
    "<Button class=\"cp_content_link\" id=\"atom(RunTroubleshooting)\" layoutpos=\"left\" content=\"@@WH_TS_TITLE@@\" shortcut=\"auto\" accessible=\"true\" accrole=\"link\"/>\r\n"
    "</NavigateButton>\r\n"
    "<Element class=\"cp_content_text\" layoutpos=\"top\" content=\"@@WH_TS_DESC@@\" padding=\"rect(0,5rp,0,0)\"/>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "<Element id=\"atom(SolutonBoxRecovery)\" layout=\"flowlayout(0,0,0,0)\" padding=\"rect(12rp,0,12rp,0)\">\r\n"
    "<NavigateButton layout=\"borderlayout()\" layoutpos=\"left\" shellexecute=\"%SystemRoot%\\\\System32\\\\control.exe\" shellexecuteparams=\"/name Microsoft.Recovery\">\r\n"
    "<Button class=\"cp_content_link\" id=\"atom(RestoreYourPC)\" layoutpos=\"left\" content=\"icon(1022, 48rp, 48rp, library(imageres.dll))\" accessible=\"true\" accname=\"@@WH_REC_TITLE@@\" accrole=\"pushbutton\"/>\r\n"
    "</NavigateButton>\r\n"
    "<Element layout=\"borderlayout()\" layoutpos=\"right\" padding=\"rect(12rp,7rp,25rp,0)\">\r\n"
    "<NavigateButton layout=\"borderlayout()\" layoutpos=\"top\" shellexecute=\"%SystemRoot%\\\\System32\\\\control.exe\" shellexecuteparams=\"/name Microsoft.Recovery\">\r\n"
    "<Button class=\"cp_content_link\" id=\"atom(RestoreYourPC)\" layoutpos=\"left\" content=\"@@WH_REC_TITLE@@\" shortcut=\"auto\" accessible=\"true\" accrole=\"link\"/>\r\n"
    "</NavigateButton>\r\n"
    "<Element class=\"cp_content_text\" layoutpos=\"top\" content=\"@@WH_REC_DESC@@\" padding=\"rect(0,5rp,0,0)\"/>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "</Element>\r\n"
    "</Element>\r\n";


// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
static std::wstring BytesToWide(const std::string& bytes) {
    std::wstring out;
    if (bytes.empty()) {
        return out;
    }
    int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, bytes.data(),
                                (int)bytes.size(), nullptr, 0);
    UINT cp = CP_UTF8;
    if (n <= 0) {
        cp = CP_ACP;
        n = MultiByteToWideChar(cp, 0, bytes.data(), (int)bytes.size(), nullptr, 0);
        if (n <= 0) {
            return out;
        }
    }
    out.resize((size_t)n);
    MultiByteToWideChar(cp, 0, bytes.data(), (int)bytes.size(), out.data(), n);
    return out;
}

static std::wstring XmlEscape(const wchar_t* s) {
    std::wstring out;
    if (!s) {
        return out;
    }
    for (const wchar_t* p = s; *p; ++p) {
        switch (*p) {
            case L'&':
                out += L"&amp;";
                break;
            case L'"':
                out += L"&quot;";
                break;
            case L'<':
                out += L"&lt;";
                break;
            case L'>':
                out += L"&gt;";
                break;
            // Strip control chars that could break DirectUI XML
            default:
                if (*p == L'\t' || *p == L'\n' || *p == L'\r' || *p >= 32) {
                    out.push_back(*p);
                }
                break;
        }
    }
    return out;
}

static void ReplaceAll(std::wstring& s, const std::wstring& from, const std::wstring& to) {
    if (from.empty() || s.empty()) {
        return;
    }
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::wstring::npos) {
        s.replace(pos, from.size(), to);
        pos += to.size();
        // safety: avoid infinite loop if to contains from
        if (to.find(from) != std::wstring::npos) {
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// Embedded resources (lazy)
// ---------------------------------------------------------------------------
std::wstring g_embeddedUifileW;
std::wstring g_solutionTemplateW;
bool g_embeddedReady = false;
bool g_templateReady = false;

bool EnsureEmbeddedUifile() {
    if (g_embeddedReady) {
        return !g_embeddedUifileW.empty();
    }
    g_embeddedReady = true;
    std::string raw = g_uifile201;
    if (raw.empty()) {
        Wh_Log(L"Empty embedded UIFILE");
        return false;
    }
    g_embeddedUifileW = BytesToWide(raw);
    return !g_embeddedUifileW.empty();
}

bool EnsureSolutionTemplate() {
    if (g_templateReady) {
        return !g_solutionTemplateW.empty();
    }
    g_templateReady = true;
    std::string raw = g_solutionBlock;
    if (raw.empty()) {
        Wh_Log(L"Empty solution template");
        return false;
    }
    g_solutionTemplateW = BytesToWide(raw);
    return !g_solutionTemplateW.empty();
}

std::wstring BuildLocalizedSolutionBlock() {
    if (!EnsureSolutionTemplate()) {
        return {};
    }
    const LangPack* pack = GetLangPack();
    if (!pack || !pack->heading || !pack->tsTitle || !pack->tsDesc || !pack->recTitle ||
        !pack->recDesc) {
        Wh_Log(L"Invalid lang pack");
        return {};
    }


    std::wstring block = g_solutionTemplateW;
    ReplaceAll(block, L"@@WH_HEADING@@", XmlEscape(pack->heading));
    ReplaceAll(block, L"@@WH_TS_TITLE@@", XmlEscape(pack->tsTitle));
    ReplaceAll(block, L"@@WH_TS_DESC@@", XmlEscape(pack->tsDesc));
    ReplaceAll(block, L"@@WH_REC_TITLE@@", XmlEscape(pack->recTitle));
    ReplaceAll(block, L"@@WH_REC_DESC@@", XmlEscape(pack->recDesc));

    // Refuse to return a block that still has unresolved tokens
    if (block.find(L"@@WH_") != std::wstring::npos) {
        Wh_Log(L"Unresolved localization tokens - abort block");
        return {};
    }
    // Must keep native atoms
    if (block.find(L"atom(HavingAProblem)") == std::wstring::npos ||
        block.find(L"atom(RunTroubleshooting)") == std::wstring::npos ||
        block.find(L"atom(RestoreYourPC)") == std::wstring::npos) {
        Wh_Log(L"Solution block missing required native atoms - abort");
        return {};
    }
    return block;
}

// ---------------------------------------------------------------------------
// Hub detection / validation
// ---------------------------------------------------------------------------
bool LooksLikeActionCenterHub(const std::wstring& xml) {
    // Check reasonable bounds for full or partial DirectUI hub page XML
    if (xml.size() < 600 || xml.size() > 4 * 1024 * 1024) {
        return false;
    }
    // HavingAProblem is the target element we replace; it must be present
    if (xml.find(L"atom(HavingAProblem)") == std::wstring::npos) {
        return false;
    }
    // Check if this XML is part of the Action Center hub page or solution box
    bool hasPage = xml.find(L"HealthCenterCPLPage") != std::wstring::npos;
    bool hasSec = xml.find(L"atom(SecurityGroupExpando)") != std::wstring::npos;
    bool hasMain = xml.find(L"atom(MaintenanceGroupExpando)") != std::wstring::npos;
    bool hasPatched = xml.find(L"WhStaticPatched") != std::wstring::npos;
    bool hasRed = xml.find(L"resid=\"RedModule\"") != std::wstring::npos;
    bool hasYellow = xml.find(L"resid=\"YellowModule\"") != std::wstring::npos;
    bool hasTs = xml.find(L"atom(RunTroubleshooting)") != std::wstring::npos;

    if (!hasPage && !hasSec && !hasMain && !hasPatched && !hasRed && !hasYellow && !hasTs) {
        return false;
    }
    return true;
}

static bool IsSelfClosingTag(const std::wstring& s, size_t gtPos) {
    if (gtPos == std::wstring::npos || gtPos == 0 || gtPos > s.size()) return false;
    // gtPos points at '>', check char(s) before it skipping whitespace
    size_t p = gtPos;
    if (p == 0) return false;
    --p; // char before '>'
    while (p > 0 && (s[p] == L' ' || s[p] == L'\t' || s[p] == L'\r' || s[p] == L'\n')) {
        if (p == 0) break;
        --p;
    }
    return s[p] == L'/';
}


bool ValidateHubXml(const std::wstring& xml, const std::wstring& originalInput = std::wstring()) {
    // 1. Check that our injected markers are present in the patched XML
    static const wchar_t* kPatchedRequired[] = {
        L"atom(HavingAProblem)",
        L"atom(RunTroubleshooting)",
        L"atom(RestoreYourPC)",
    };
    for (const wchar_t* m : kPatchedRequired) {
        if (xml.find(m) == std::wstring::npos) {
            Wh_Log(L"ValidateHubXml FAIL missing injected marker: %s", m);
            return false;
        }
    }

    // 2. Any structural section that was present in originalInput MUST STILL be present in xml
    static const wchar_t* kPreserved[] = {
        L"HealthCenterCPLPage",
        L"atom(SecurityGroupExpando)",
        L"atom(MaintenanceGroupExpando)",
        L"resid=\"RedModule\"",
        L"resid=\"YellowModule\"",
        L"resid=\"CheckModule\"",
        L"atom(RedSecurityList)",
        L"atom(YellowSecurityList)",
        L"atom(RedMaintenanceList)",
        L"atom(YellowMaintenanceList)",
        L"atom(SecurityCheckModule)",
        L"atom(MaintenanceCheckModule)",
        L"</duixml>",
    };
    for (const wchar_t* m : kPreserved) {
        if (!originalInput.empty() && originalInput.find(m) != std::wstring::npos && xml.find(m) == std::wstring::npos) {
            Wh_Log(L"ValidateHubXml FAIL accidentally removed section: %s", m);
            return false;
        } else if (originalInput.empty() && xml.find(m) == std::wstring::npos) {
            // If called without originalInput (or empty), only require core structure to avoid rejecting valid collapsed states
            if (_wcsicmp(m, L"HealthCenterCPLPage") == 0 || _wcsicmp(m, L"</duixml>") == 0) {
                Wh_Log(L"ValidateHubXml FAIL missing core structure: %s", m);
                return false;
            }
        }
    }

    // 3. Rough well-formedness: balanced Element open/close counts (robust self-close)
    size_t opens = 0, closes = 0;
    for (size_t i = 0; i + 8 < xml.size(); ++i) {
        if (xml[i] != L'<') {
            continue;
        }
        if (xml.compare(i, 8, L"<Element") == 0) {
            size_t gt = xml.find(L'>', i);
            if (gt != std::wstring::npos && IsSelfClosingTag(xml, gt)) {
                continue;
            }
            ++opens;
        } else if (xml.compare(i, 9, L"</Element") == 0) {
            ++closes;
        }
    }
    if (opens != closes) {
        Wh_Log(L"ValidateHubXml FAIL Element open=%zu close=%zu", opens, closes);
        return false;
    }

    // 4. Ordering check: if both Security and Maintenance are present, ensure order
    size_t sec = xml.find(L"atom(SecurityGroupExpando)");
    size_t man = xml.find(L"atom(MaintenanceGroupExpando)");
    size_t hav = xml.find(L"atom(HavingAProblem)");
    if (sec != std::wstring::npos && man != std::wstring::npos) {
        if (!(sec < man && man < hav)) {
            Wh_Log(L"ValidateHubXml FAIL order sec=%zu man=%zu hav=%zu", sec, man, hav);
            return false;
        }
    } else if (sec != std::wstring::npos) {
        if (sec >= hav) {
            Wh_Log(L"ValidateHubXml FAIL order sec=%zu hav=%zu", sec, hav);
            return false;
        }
    } else if (man != std::wstring::npos) {
        if (man >= hav) {
            Wh_Log(L"ValidateHubXml FAIL order man=%zu hav=%zu", man, hav);
            return false;
        }
    }

    return true;
}

size_t FindBalancedElementEnd(const std::wstring& s, size_t start) {
    if (start >= s.size() || s[start] != L'<') {
        return std::wstring::npos;
    }
    if (s.compare(start, 8, L"<Element") != 0) {
        return std::wstring::npos;
    }

    size_t i = start;
    int depth = 0;
    const size_t n = s.size();
    size_t steps = 0;
    const size_t kMaxSteps = n * 2;

    while (i < n && steps++ < kMaxSteps) {
        if (s[i] != L'<') { ++i; continue; }

        if (i + 1 < n && (s[i + 1] == L'!' || s[i + 1] == L'?')) {
            size_t gt = s.find(L'>', i);
            if (gt == std::wstring::npos) return std::wstring::npos;
            i = gt + 1;
            continue;
        }

        size_t gt = s.find(L'>', i);
        if (gt == std::wstring::npos) return std::wstring::npos;

        bool isClosing = (i + 1 < n && s[i + 1] == L'/');
        bool selfClose = false;
        if (!isClosing) {
            selfClose = IsSelfClosingTag(s, gt);
        }

        if (isClosing) {
            if (depth > 0) --depth;
            else depth = 0;
            i = gt + 1;
            if (depth == 0) return i;
            continue;
        } else {
            // opening
            if (!selfClose) ++depth;
            i = gt + 1;
            // edge: self-closing start element that is the target itself
            if (selfClose && depth == 0 && i > start) return i;
            continue;
        }
    }
    return std::wstring::npos;
}

// ---------------------------------------------------------------------------
// Patch
// ---------------------------------------------------------------------------
std::wstring PatchHubXml(const std::wstring& input) {
    std::wstring block = BuildLocalizedSolutionBlock();
    if (block.empty()) {
        Wh_Log(L"Empty localized block - no patch");
        return input;
    }

    const std::wstring marker = L"id=\"atom(HavingAProblem)\"";
    size_t secPos = input.find(L"atom(SecurityGroupExpando)");
    size_t manPos = input.find(L"atom(MaintenanceGroupExpando)");

    // Search for all id="atom(HavingAProblem)" occurrences and pick the correct target
    size_t bestStart = std::wstring::npos;
    size_t bestEnd = std::wstring::npos;

    size_t searchPos = 0;
    while ((searchPos = input.find(marker, searchPos)) != std::wstring::npos) {
        size_t start = input.rfind(L'<', searchPos);
        if (start != std::wstring::npos && input.compare(start, 8, L"<Element") == 0) {
            size_t end = FindBalancedElementEnd(input, start);
            if (end != std::wstring::npos && end > start && end <= input.size()) {
                // Ensure this block does not swallow Security or Maintenance sections
                bool swallowsSec = (secPos != std::wstring::npos && secPos >= start && secPos < end);
                bool swallowsMan = (manPos != std::wstring::npos && manPos >= start && manPos < end);
                if (!swallowsSec && !swallowsMan) {
                    // Prefer the occurrence that comes after Maintenance section if present
                    if (manPos != std::wstring::npos) {
                        if (start >= manPos) {
                            bestStart = start;
                            bestEnd = end;
                        } else if (bestStart == std::wstring::npos) {
                            bestStart = start;
                            bestEnd = end;
                        }
                    } else {
                        bestStart = start;
                        bestEnd = end;
                    }
                }
            }
        }
        searchPos += marker.size();
    }

    if (bestStart == std::wstring::npos || bestEnd == std::wstring::npos || bestEnd - bestStart > 32 * 1024) {
        Wh_Log(L"Could not find suitable HavingAProblem Element - no patch");
        return input;
    }

    std::wstring xml = input;
    xml.replace(bestStart, bestEnd - bestStart, block);

    if (!ValidateHubXml(xml, input)) {
        Wh_Log(L"Patched XML failed validation - keeping original");
        return input;
    }

    return xml;
}

// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------
#ifdef _WIN64
#define THISCALL __cdecl
#else
#define THISCALL __thiscall
#endif

using SetXML_t = HRESULT(THISCALL*)(void* pThis, const WCHAR* pszXML, HINSTANCE hRes,
                                    HINSTANCE hResTheme);
SetXML_t SetXML_Original = nullptr;

// Reentrancy: DirectUI may call SetXML nested; never patch re-entrantly.
static thread_local int g_inSetXmlHook = 0;

static HRESULT CallOriginalSetXML(void* pThis, const WCHAR* pszXML, HINSTANCE hRes,
                                  HINSTANCE hResTheme) {
    if (!SetXML_Original) {
        return E_FAIL;
    }
    return SetXML_Original(pThis, pszXML, hRes, hResTheme);
}


HRESULT THISCALL SetXML_Hook(void* pThis, const WCHAR* pszXML, HINSTANCE hRes,
                             HINSTANCE hResTheme) {
    if (!pszXML || !SetXML_Original || !g_cplRestoreHubLinks) {
        return CallOriginalSetXML(pThis, pszXML, hRes, hResTheme);
    }

    // Reentrancy guard: DirectUI can call SetXML nested (templates, includes)
    if (g_inSetXmlHook != 0) {
        return CallOriginalSetXML(pThis, pszXML, hRes, hResTheme);
    }

        // Fast pre-filter: avoid building std::wstring work for irrelevant fragments
    // using raw pointer search before any allocation.
    if (!wcsstr(pszXML, L"atom(HavingAProblem)")) {
        return CallOriginalSetXML(pThis, pszXML, hRes, hResTheme);
    }

    std::wstring xml(pszXML);

    if (!LooksLikeActionCenterHub(xml)) {
        return CallOriginalSetXML(pThis, pszXML, hRes, hResTheme);
    }

    g_inSetXmlHook++;

    // Applica la patch DIRETTAMENTE - sempre, anche se già contiene WhStaticPatched,
    // così la lingua viene ri-applicata ad ogni navigazione.
    std::wstring patched = PatchHubXml(xml);

    HRESULT hr;
    if (patched.empty() || patched == xml) {
        // Even if PatchHubXml returned original (already optimal), still call original
        hr = CallOriginalSetXML(pThis, pszXML, hRes, hResTheme);
    } else {
        hr = CallOriginalSetXML(pThis, patched.c_str(), hRes, hResTheme);
        if (FAILED(hr)) {
            Wh_Log(L"SetXML(patched) failed 0x%08X - retry original", (unsigned)hr);
            hr = CallOriginalSetXML(pThis, pszXML, hRes, hResTheme);
        } else {
            Wh_Log(L"SetXML patched HavingAProblem OK (in=%zu out=%zu)", xml.size(), patched.size());
        }
    }

    g_inSetXmlHook--;
    return hr;
}

using SetXMLFromResource_t = HRESULT(THISCALL*)(void* pThis, PCWSTR lpName, PCWSTR lpType,
                                                HMODULE hModule, HINSTANCE p4, HINSTANCE p5);
SetXMLFromResource_t SetXMLFromResource_Original = nullptr;

bool ModuleIsActionCenterCpl(HMODULE hModule) {
    if (!hModule) {
        return false;
    }
    wchar_t path[MAX_PATH];
    DWORD n = GetModuleFileNameW(hModule, path, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) {
        return false;
    }
    PCWSTR name = PathFindFileNameW(path);
    return name && _wcsicmp(name, L"ActionCenterCPL.dll") == 0;
}

bool ResourceNameIs201(PCWSTR lpName) {
    if (!lpName) {
        return false;
    }
    if (IS_INTRESOURCE(lpName)) {
        return (UINT_PTR)lpName == 201;
    }
    // Defensive: only short numeric names
    return lpName[0] != L'\0' && _wcsicmp(lpName, L"201") == 0;
}

static HRESULT SetXMLFromResource_EmbeddedBody(void* pThis, HMODULE hModule, HINSTANCE p4) {
    std::wstring source = g_embeddedUifileW;
    std::wstring patched = PatchHubXml(source);
    const std::wstring* use = &source;
    if (!patched.empty() && ValidateHubXml(patched, source)) {
        use = &patched;
    } else if (!ValidateHubXml(source, source)) {
        return E_FAIL;
    }
    return SetXML_Original(pThis, use->c_str(), hModule, p4);
}

static HRESULT SetXMLFromResource_ResourceBody(void* pThis, PCWSTR lpName, PCWSTR lpType,
                                               HMODULE hModule, HINSTANCE p4, HINSTANCE p5) {
    HRSRC hRsrc = FindResourceW(hModule, lpName, lpType);
    if (!hRsrc) return SetXMLFromResource_Original(pThis, lpName, lpType, hModule, p4, p5);
    HGLOBAL hGlobal = LoadResource(hModule, hRsrc);
    if (!hGlobal) return SetXMLFromResource_Original(pThis, lpName, lpType, hModule, p4, p5);
    const char* pData = (const char*)LockResource(hGlobal);
    DWORD dwSize = SizeofResource(hModule, hRsrc);
    if (!pData || dwSize == 0) return SetXMLFromResource_Original(pThis, lpName, lpType, hModule, p4, p5);

    std::string raw(pData, dwSize);
    std::wstring source = BytesToWide(raw);
    if (source.empty()) return SetXMLFromResource_Original(pThis, lpName, lpType, hModule, p4, p5);

    std::wstring patched = PatchHubXml(source);
    const std::wstring* use = &source;
    if (!patched.empty() && ValidateHubXml(patched, source)) {
        use = &patched;
    } else if (!ValidateHubXml(source, source)) {
        return SetXMLFromResource_Original(pThis, lpName, lpType, hModule, p4, p5);
    }
    return SetXML_Original(pThis, use->c_str(), hModule, p4);
}

HRESULT THISCALL SetXMLFromResource_Hook(void* pThis, PCWSTR lpName, PCWSTR lpType,
                                         HMODULE hModule, HINSTANCE p4, HINSTANCE p5) {
    if (!SetXMLFromResource_Original) {
        return E_FAIL;
    }

    if (!g_cplRestoreHubLinks || g_inSetXmlHook != 0 || !lpType || !SetXML_Original) {
        return SetXMLFromResource_Original(pThis, lpName, lpType, hModule, p4, p5);
    }

    if (_wcsicmp(lpType, L"UIFILE") != 0 || !ResourceNameIs201(lpName) ||
        !ModuleIsActionCenterCpl(hModule)) {
        return SetXMLFromResource_Original(pThis, lpName, lpType, hModule, p4, p5);
    }

    g_inSetXmlHook++;
    HRESULT hr = E_FAIL;
    if (g_useEmbeddedUifile && EnsureEmbeddedUifile()) {
        hr = SetXMLFromResource_EmbeddedBody(pThis, hModule, p4);
    } else {
        hr = SetXMLFromResource_ResourceBody(pThis, lpName, lpType, hModule, p4, p5);
    }
    if (FAILED(hr)) {
        Wh_Log(L"SetXMLFromResource path failed 0x%08X - stock resource load", (unsigned)hr);
        hr = SetXMLFromResource_Original(pThis, lpName, lpType, hModule, p4, p5);
    }
    g_inSetXmlHook--;
    return hr;
}

static bool g_cplHooksInstalled = false;

bool CplHookDui() {
    if (g_cplHooksInstalled && SetXML_Original) {
        return true;
    }

    HMODULE dui = GetModuleHandleW(L"dui70.dll");
    if (!dui) {
        dui = LoadLibraryExW(L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (!dui) {
        Wh_Log(L"Failed to load dui70.dll");
        return false;
    }

    const char* setXmlNames[] = {
        "?SetXML@DUIXmlParser@DirectUI@@QEAAJPEBGPEAUHINSTANCE__@@1@Z",
        "?SetXML@DUIXmlParser@DirectUI@@QAAJPBGPAUHINSTANCE__@@1@Z",
    };
    bool hookedSetXml = (SetXML_Original != nullptr);
    if (!hookedSetXml) {
        for (const char* name : setXmlNames) {
            if (FARPROC p = GetProcAddress(dui, name)) {
                Wh_SetFunctionHook((void*)p, (void*)SetXML_Hook, (void**)&SetXML_Original);
                Wh_Log(L"Hooked SetXML: %S", name);
                hookedSetXml = true;
                break;
            }
        }
    } else {
        hookedSetXml = true;
    }

    if (!hookedSetXml) {
        Wh_Log(L"Could not find DUIXmlParser::SetXML");
        return false;
    }

    // Hook _SetXMLFromResource as well - needed for both fallback modes and also
    // as safety net when Control Panel loads hub via resource path.
    const char* setFromResNames[] = {
#ifdef _WIN64
        "?_SetXMLFromResource@DUIXmlParser@DirectUI@@IEAAJPEBG0PEAUHINSTANCE__@@11@Z",
#endif
        "?_SetXMLFromResource@DUIXmlParser@DirectUI@@IAEJPBG0PAUHINSTANCE__@@11@Z",
    };
    if (!SetXMLFromResource_Original) {
        for (const char* name : setFromResNames) {
            if (FARPROC p = GetProcAddress(dui, name)) {
                Wh_SetFunctionHook((void*)p, (void*)SetXMLFromResource_Hook,
                                   (void**)&SetXMLFromResource_Original);
                Wh_Log(L"Hooked _SetXMLFromResource: %S", name);
                break;
            }
        }
    }

    g_cplHooksInstalled = (SetXML_Original != nullptr);
    return g_cplHooksInstalled;
}

static void CplInit(void) {
    CplLoadSettings();
    if (!g_cplRestoreHubLinks) {
        Wh_Log(L"CPL hub links: disabled in settings");
        return;
    }
    // Lazy decoding: EnsureSolutionTemplate / EnsureEmbeddedUifile are now decoded
    // on demand inside the hook path to avoid paying the conversion in processes
    // that never open the Security and Maintenance page.
    if (CplHookDui()) {
        Wh_Log(L"CPL hub links: hooks installed");
    } else {
        Wh_Log(L"CPL hub links: initial hook failed");
    }
}
static void CplSettingsChanged(void) {
    CplLoadSettings();

}

BOOL Wh_ModInit(void) {
    Wh_Log(L"Win7 Action Center - Initializing...");

    // Ricava l'HINSTANCE della DLL della mod usando un indirizzo nei suoi dati.
    HMODULE hThisModule = NULL;
    if (GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCWSTR)(const void*)&g_hModInstance, &hThisModule)) {
        g_hModInstance = (HINSTANCE)hThisModule;
    } else {
        g_hModInstance = (HINSTANCE)GetModuleHandleW(NULL);
        Wh_Log(L"Unable to resolve mod HINSTANCE; using process fallback");
    }
    InterlockedExchange(&g_WscCallbacksInFlight, 0);
    g_ProblemBalloonShowing = FALSE;
    g_LastProblemBalloonTick = 0;
    g_LastProblemBalloonSignature = 0;
    g_LastProblemBalloonState = STATE_GOOD;
    
    // Security and Maintenance CPL hub links (DirectUI) - independent of tray UI.
    // Initialize across all Explorer and Control Panel instances for stable navigation patching.
    CplInit();

    // Allow init even if Shell_TrayWnd is not ready yet (boot / explorer restart).
    if (!IsMainExplorerProcess()) { 
        Wh_Log(L"Not shell explorer.exe, skipping tray UI initialization");
        return TRUE; 
    }
    
    ZeroMemory(&g_Ctx, sizeof(g_Ctx));
    LoadSettings();
    DetermineLocale();
    InitializeSRWLock(&g_Ctx.srwLock);
    g_Ctx.darkMode = GetEffectiveDarkMode(); // rispetta l'opzione theme (auto/light/dark)
    HDC hScreenDC = GetDC(NULL);
    UINT dpi = hScreenDC ? (UINT)GetDeviceCaps(hScreenDC, LOGPIXELSX) : 96;
    if (hScreenDC) ReleaseDC(NULL, hScreenDC);
    
    // Inizializza con 0 problemi (stato iniziale)
    RecalcDpiMetrics(dpi, 0);
    InitGlobalFonts();

    // GDI+ deve essere inizializzato prima di decodificare le PNG Base64.
    if (!InitGdiPlusRendering()) {
        Wh_Log(L"GDI+ init failed - will use DrawIconEx fallback");
    }
    InitFlyoutIcons();

    // Il ready event chiude la race tra CreateThread e un unload immediato.
    g_Ctx.hTrayReadyEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!g_Ctx.hTrayReadyEvent)
        Wh_Log(L"Failed to create tray-ready event: %lu", GetLastError());

    // Tutte le finestre vengono create dal TrayThreadProc.
    g_Ctx.hTrayThread = CreateThread(NULL, 0, TrayThreadProc, NULL, 0, &g_Ctx.trayThreadId);
    if (!g_Ctx.hTrayThread) {
        Wh_Log(L"Failed to create tray thread - cleaning up");
        CleanupModResources();
        return FALSE;
    }
    if (g_Ctx.hTrayReadyEvent) {
        DWORD ready = WaitForSingleObject(g_Ctx.hTrayReadyEvent, 5000);
        if (ready == WAIT_OBJECT_0 &&
            (!g_Ctx.hWndMsgHandler || !IsWindow(g_Ctx.hWndMsgHandler))) {
            Wh_Log(L"Tray thread failed before creating its message window");
            CleanupModResources();
            return FALSE;
        }
    }

    g_Initialized = TRUE;
    Wh_Log(L"Initialization complete");
    return TRUE;
}

void Wh_ModSettingsChanged(void) {
    LoadSettings();
    DetermineLocale();
    CplSettingsChanged();
    EnsureTrayTooltip();
    if (g_Ctx.hWndMsgHandler && IsWindow(g_Ctx.hWndMsgHandler) && !g_Ctx.isUninitializing) {
        PostMessageW(g_Ctx.hWndMsgHandler, WM_SETTINGS_CHANGED, 0, 0);
    }
}

void Wh_ModUninit(void) {
    Wh_Log(L"Wh_ModUninit called");
    CleanupModResources();
    Wh_Log(L"Uninitialization complete");
}
