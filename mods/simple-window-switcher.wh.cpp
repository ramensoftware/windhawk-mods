// ==WindhawkMod==
// @id              simple-window-switcher
// @name            Simple Window Switcher
// @description     Replaces the default Alt+Tab with a lightweight window switcher inspired by ExplorerPatcher's Simple Window Switcher
// @version         1.1
// @author          Lone
// @github          https://github.com/Louis047
// @include         windhawk.exe
// @include         explorer.exe
// @compilerOptions -ldwmapi -luxtheme -lgdi32 -lshlwapi -loleaut32 -lole32 -lcomctl32 -lgdiplus
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Simple Window Switcher
A lightweight Alt+Tab replacement for Windows, ported from the
[Simple Window Switcher](https://github.com/valinet/sws) project.
Additional improvements made by [Asteski](https://github.com/Asteski).

## Features
- Grid layout with live DWM thumbnail previews
- Different Task List and Header Content layouts
- Keyboard navigation (Tab/Shift+Tab/Shift/Backtick, Arrow keys, Enter, Esc)
- Mouse click to select, scroll wheel to cycle
- Alt+Ctrl+Tab sticky mode
- Theme support (None/Backdrop Acrylic)
- Works with elevated/admin applications
- Dark/light mode auto-detection
- Custom border colors with optional Windows accent color
- DPI-aware, multi-monitor aware
- Rounded corners for switcher and task thumbnails (optional)

## Screenshots

*Horizontal squared (default)*

![Horizontal default](https://raw.githubusercontent.com/Asteski/Windhawk-Mods/refs/heads/main/img/simple-window-switcher/4.png)

*Horizontal squared without thumbnails*

![Horizontal without thumbnails](https://raw.githubusercontent.com/Asteski/Windhawk-Mods/refs/heads/main/img/simple-window-switcher/3.png)

*Vertical small rounded*

![Vertical small](https://raw.githubusercontent.com/Asteski/Windhawk-Mods/58449dc268347949193f2c67b0b042d287c20bd5/img/simple-window-switcher/1.png)

*Vertical large rounded*

![Vertical large](https://raw.githubusercontent.com/Asteski/Windhawk-Mods/refs/heads/main/img/simple-window-switcher/2.png)

*Vertical rounded with centered task icons and titles*

![Vertical centered](https://raw.githubusercontent.com/Asteski/Windhawk-Mods/refs/heads/main/img/simple-window-switcher/6.png)

*Vertical squared with thumbnails*

![Vertical squared with thumbnails](https://raw.githubusercontent.com/Asteski/Windhawk-Mods/refs/heads/main/img/simple-window-switcher/5.png)

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- theme: none
  $name: Theme
  $description: Visual theme for the switcher background.
  $options:
  - none: None (transparent)
  - backdrop: Backdrop (Acrylic)
- opacity: 100
  $name: Background Opacity
  $description: Background opacity percentage (0-100), applies to None theme.
- colorScheme: system
  $name: Color Scheme
  $options:
  - system: Follow system setting
  - light: Light
  - dark: Dark
- cornerPreference: none
  $name: Corner Preference
  $description: Corner radius for the switcher window only.
  $options:
  - none: Do not round
  - round: Round
  - roundSmall: Round small
- taskRoundedCorners: false
  $name: Round Task Borders and Close Button
  $description: Apply small rounded corners to the selected task border and close button.
- showThumbnails: true
  $name: Show Thumbnails
  $description: Show DWM live thumbnail previews of windows.
- taskListOrientation: horizontal
  $name: Task List Orientation
  $description: Arrange tasks left-to-right or top-to-bottom.
  $options:
  - horizontal: Horizontal
  - vertical: Vertical
- headerContentOrientation: horizontal
  $name: Header Content Orientation
  $description: Orientation of the task header icon and title.
  $options:
  - horizontal: Horizontal
  - vertical: Vertical
- iconSize: small
  $name: Icon Size
  $description: Size of the header icon.
  $options:
  - small: Small
  - large: Large
- rowHeight: 230
  $name: Row Height
  $description: Total height of each thumbnail row in pixels (before DPI scaling). Default 230 matches ExplorerPatcher.
- rowWidth: 0
  $name: Row Width
  $description: Width of each thumbnail tile in pixels (before DPI scaling). Set to 0 for automatic width based on window aspect ratio.
- maxWidthPercent: 80
  $name: Maximum Width (percentage of screen width)
- maxHeightPercent: 80
  $name: Maximum Height (percentage of screen height)

- showDelay: 0
  $name: Show Delay (ms)
  $description: Delay in milliseconds before showing the switcher (0 = instant).
- scrollWheelBehavior: never
  $name: Scroll Wheel to Change Selection
  $options:
  - never: Never
  - always: Always
  - stickyOnly: Only in sticky mode
- backwardShortcut: altShiftTab
  $name: Backward Shortcut
  $description: Shortcut used to move backward in the switcher.
  $options:
  - altShiftTab: Alt+Shift+Tab (default)
  - altShift: Alt+Shift
  - altBacktick: Alt+Backtick
- borderColorDark: "#FFFFFF"
  $name: Border Color (Dark Mode)
  $description: Border color in HEX format for dark mode.
- borderColorLight: "#000000"
  $name: Border Color (Light Mode)
  $description: Border color in HEX format for light mode.
- useAccentColor: false
  $name: Use Accent Color for Borders
  $description: Use Windows accent color for selection and hover borders.
- centerTaskContent: false
  $name: Center Task Icon and Title
  $description: Center the icon and title together in each task row.
- primaryMonitorOnly: false
  $name: Always Display Switcher on Primary Monitor
- perMonitorWindows: false
  $name: Display Windows Only From the Monitor Containing the Cursor
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <windows.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <vssym32.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <propkey.h>
#include <shobjidl.h>
#include <knownfolders.h>
#include <windowsx.h>
#include <commctrl.h>
#include <appmodel.h>
#include <vector>
#include <atomic>
#include <map>
#include <string>
#include <gdiplus.h>

#define SWS_CLASSNAME       L"WindhawkSWS_Switcher"
#define SWS_ICON_SIZE       16
// EP-style nested padding layers (before DPI scaling)
#define SWS_MASTER_PADDING      20  // Outer margin of the entire switcher window
#define SWS_ELEMENT_PAD_TOP     5   // Vertical margin between cell border and content
#define SWS_ELEMENT_PAD_BOTTOM  5
#define SWS_ELEMENT_PAD_LEFT    2   // Horizontal margin between cell border and content
#define SWS_ELEMENT_PAD_RIGHT   2
#define SWS_PAD_TOP             7   // Inner distance from content area to thumbnail
#define SWS_PAD_BOTTOM          7
#define SWS_PAD_LEFT            7
#define SWS_PAD_RIGHT           7
#define SWS_PAD_DIVIDER         7   // Vertical divider between title row and thumbnail
#define SWS_ROW_TITLE_HEIGHT    30  // Height of icon+title row
#define SWS_MAX_TILE_ASPECT     2.0 // Max thumbnail width = thumbH * this
#define SWS_CONTOUR_SIZE        2
#define SWS_HIGHLIGHT_SIZE      2
#define SWS_HOTKEY_ALTTAB           1
#define SWS_HOTKEY_ALTSHIFTTAB      2
#define SWS_HOTKEY_ALTCTRLTAB       3
#define SWS_HOTKEY_ALTSHIFTCTRLTAB  4
#define SWS_HOTKEY_ALTBACKTICK      5
#define SWS_HOTKEY_RETRY_TIMER_ID   100
#define SWS_HOTKEY_RETRY_INTERVAL   2000
#define SWS_BG_DARK          RGB(32, 32, 32)
#define SWS_BG_LIGHT         RGB(243, 243, 243)
#define SWS_CONTOUR_DARK     RGB(255, 255, 255)
#define SWS_CONTOUR_LIGHT    RGB(0, 0, 0)
#define SWS_TEXT_DARK         RGB(255, 255, 255)
#define SWS_TEXT_LIGHT        RGB(0, 0, 0)
#define SWS_SHOW_DELAY_TIMER_ID 101

typedef BOOL (WINAPI *IsShellWindow_t)(HWND);
typedef HWND (WINAPI *GhostWindowFromHungWindow_t)(HWND);
struct ACCENT_POLICY { DWORD AccentState; DWORD AccentFlags; DWORD GradientColor; DWORD AnimationId; };
struct WINDOWCOMPOSITIONATTRIBDATA { DWORD dwAttrib; PVOID pvData; SIZE_T cbData; };
typedef BOOL(WINAPI *SetWindowCompositionAttribute_t)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

struct WindowEntry {
    HWND hWnd; HICON hIcon; WCHAR title[256]; HTHUMBNAIL hThumb;
    RECT rcCell; RECT rcThumb; RECT rcThumbActual;
    SIZE sourceSize;           // Raw DWM surface size
    RECT rcSourceCrop;         // Source crop rect for DWM_TNP_RECTSOURCE
    SIZE effectiveSourceSize;  // Source size after cropping invisible frame
};
struct Settings {
    WCHAR theme[32]; WCHAR colorScheme[32]; WCHAR cornerPreference[32]; WCHAR scrollWheelBehavior[32]; WCHAR taskListOrientation[32]; WCHAR headerContentOrientation[32]; WCHAR iconSize[32]; WCHAR backwardShortcut[32];
    WCHAR borderColorDark[16];
    WCHAR borderColorLight[16];
    int opacity; int rowHeight; int rowWidth;
    int maxWidthPercent; int maxHeightPercent; int showDelay;
    bool showThumbnails; bool useAccentColor; bool primaryMonitorOnly; bool perMonitorWindows; bool taskRoundedCorners;
    bool centerTaskContent;
};

static HWND g_hSwitcher = NULL;
static std::vector<WindowEntry> g_windows;
static int g_selectedIndex = 0, g_hoverIndex = -1;
static int g_layoutStartIndex = 0; // EP-style: first window index visible in the layout
static bool g_isVisible = false, g_isSticky = false, g_isDarkMode = false;
static HFONT g_hFont = NULL;
static HTHEME g_hTheme = NULL;
static UINT g_shellHookMsg = 0;
static int g_dpiX = 96, g_dpiY = 96;
static int g_winW = 0, g_winH = 0;
static bool g_hotkeysRegistered = false;
static HMONITOR g_hCurrentMonitor = NULL;
static Settings g_settings;
static HANDLE g_hSwitcherThread = NULL;
static DWORD g_dwSwitcherThreadId = 0;
static bool g_isExplorer = false;
static HANDLE g_restartExplorerPromptThread = NULL;
static std::atomic<HWND> g_restartExplorerPromptWindow{nullptr};
static IsShellWindow_t g_IsShellManagedWindow = nullptr;
static IsShellWindow_t g_IsShellFrameWindow = nullptr;
static GhostWindowFromHungWindow_t g_GhostWindowFromHungWindow = nullptr;
static GhostWindowFromHungWindow_t g_HungWindowFromGhostWindow = nullptr;
static SetWindowCompositionAttribute_t g_SetWindowCompositionAttribute = nullptr;
static ULONG_PTR g_gdiplusToken = 0;
static bool g_isCloseHovered = false;
static HANDLE g_explorerIpcThread = NULL;
static bool g_isPendingShow = false;
static RECT g_pendingSwitcherRect = {0, 0, 0, 0};

// Helpers

static bool ThemeIs(const WCHAR* v) { return wcscmp(g_settings.theme, v) == 0; }
static bool ScrollIs(const WCHAR* v) { return wcscmp(g_settings.scrollWheelBehavior, v) == 0; }
static bool LayoutIsVertical() { return wcscmp(g_settings.taskListOrientation, L"vertical") == 0; }
static bool HeaderOrientationIs(const WCHAR* v) { return wcscmp(g_settings.headerContentOrientation, v) == 0; }
static bool IconSizeIs(const WCHAR* v) { return wcscmp(g_settings.iconSize, v) == 0; }
static bool BackwardShortcutIs(const WCHAR* v) { return wcscmp(g_settings.backwardShortcut, v) == 0; }
static bool UseAltShiftTabBackward() { return BackwardShortcutIs(L"altShiftTab"); }
static bool UseAltShiftBackward() { return BackwardShortcutIs(L"altShift"); }
static bool UseAltBacktickBackward() { return BackwardShortcutIs(L"altBacktick"); }
static bool HeaderIsVertical() {
    return HeaderOrientationIs(L"vertical");
}
static int GetHeaderIconSizePx() {
    if (IconSizeIs(L"large")) {
        return MulDiv(32, g_dpiX, 96);
    }
    return MulDiv(SWS_ICON_SIZE, g_dpiX, 96);
}
static int GetHeaderTitleHeightPx() {
    return MulDiv(18, g_dpiY, 96);
}
static int GetHeaderRowHeightPx() {
    if (!HeaderIsVertical()) {
        return MulDiv(SWS_ROW_TITLE_HEIGHT, g_dpiY, 96);
    }

    int gap = MulDiv(4, g_dpiY, 96);
    return GetHeaderIconSizePx() + gap + GetHeaderTitleHeightPx();
}
static INT GetCornerPref() {
    if (wcscmp(g_settings.cornerPreference, L"none") == 0) return 1;
    if (wcscmp(g_settings.cornerPreference, L"roundSmall") == 0) return 3;
    return 2; // Default to round
}

static bool UseTaskRoundedCorners() {
    return g_settings.taskRoundedCorners;
}

static int GetTaskUiCornerRadiusPx() {
    if (!UseTaskRoundedCorners()) {
        return 0;
    }
    return MulDiv(4, g_dpiX, 96);
}

static int GetCloseButtonCornerRadiusPx() {
    return GetTaskUiCornerRadiusPx();
}
static bool ShouldUseDarkMode() {
    if (wcscmp(g_settings.colorScheme, L"light") == 0) return false;
    if (wcscmp(g_settings.colorScheme, L"dark") == 0) return true;
    DWORD val = 0, sz = sizeof(val);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, NULL, &val, &sz) == ERROR_SUCCESS) return val == 0;
    return true;
}
static COLORREF GetAccentColor() {
    DWORD col = 0, sz = sizeof(col);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
        L"AccentColor", RRF_RT_REG_DWORD, NULL, &col, &sz) == ERROR_SUCCESS) return col & 0x00FFFFFF;
    return RGB(0, 120, 215);
}

static bool ParseHexColor(const WCHAR* value, COLORREF* outColor) {
    if (!value) {
        return false;
    }

    const WCHAR* p = value;
    size_t len = wcslen(p);
    if (len == 7 && p[0] == L'#') {
        p++;
        len = 6;
    }

    if (len != 6) {
        return false;
    }

    unsigned int rgb = 0;
    if (swscanf_s(p, L"%06x", &rgb) != 1) {
        return false;
    }

    if (outColor) {
        *outColor = RGB((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
    }
    return true;
}

static bool ResolveAPIs() {
    HMODULE h = GetModuleHandleW(L"user32.dll");
    if (!h) return false;
    g_IsShellManagedWindow = (IsShellWindow_t)GetProcAddress(h, (LPCSTR)2574);
    g_IsShellFrameWindow = (IsShellWindow_t)GetProcAddress(h, (LPCSTR)2573);
    g_GhostWindowFromHungWindow = (GhostWindowFromHungWindow_t)GetProcAddress(h, "GhostWindowFromHungWindow");
    g_HungWindowFromGhostWindow = (GhostWindowFromHungWindow_t)GetProcAddress(h, "HungWindowFromGhostWindow");
    g_SetWindowCompositionAttribute = (SetWindowCompositionAttribute_t)GetProcAddress(h, "SetWindowCompositionAttribute");

    return true;
}

// Explorer restart prompt

constexpr WCHAR kRestartTitle[] = L"Simple Window Switcher - Windhawk";
constexpr WCHAR kRestartText[] = L"Explorer needs to be restarted for changes to take effect. Restart now?";

static HRESULT CALLBACK RestartPromptDialogCallback(HWND hwnd, UINT msg, WPARAM, LPARAM, LONG_PTR) {
    if (msg == TDN_CREATED) {
        g_restartExplorerPromptWindow = hwnd;
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    } else if (msg == TDN_DESTROYED) {
        g_restartExplorerPromptWindow = nullptr;
    }
    return S_OK;
}

static DWORD WINAPI RestartPromptThreadProc(LPVOID) {
    TASKDIALOGCONFIG tdc = {};
    tdc.cbSize = sizeof(tdc);
    tdc.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION;
    tdc.dwCommonButtons = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON;
    tdc.pszWindowTitle = kRestartTitle;
    tdc.pszMainIcon = TD_INFORMATION_ICON;
    tdc.pszContent = kRestartText;
    tdc.pfCallback = RestartPromptDialogCallback;

    int button;
    if (SUCCEEDED(TaskDialogIndirect(&tdc, &button, nullptr, nullptr)) && button == IDYES) {
        WCHAR cmd[] = L"cmd.exe /c \"timeout /t 1 /nobreak >nul & taskkill /F /IM explorer.exe & start explorer.exe\"";
        STARTUPINFO si = { .cb = sizeof(si) };
        PROCESS_INFORMATION pi = {};
        if (CreateProcess(nullptr, cmd, nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        }
    }
    return 0;
}

static void PromptForExplorerRestart() {
    if (g_restartExplorerPromptThread) {
        if (WaitForSingleObject(g_restartExplorerPromptThread, 0) != WAIT_OBJECT_0) return;
        CloseHandle(g_restartExplorerPromptThread);
    }
    g_restartExplorerPromptThread = CreateThread(
        nullptr, 0, RestartPromptThreadProc, nullptr, 0, nullptr);
}


// Window Filtering (ported from SWS)

static bool TestExStyle(HWND h, DWORD s) { return (s & (DWORD)GetWindowLongPtrW(h, GWL_EXSTYLE)) == s; }
static bool IsOwnerToolWindow(HWND hwnd) {
    HWND cur = hwnd, own = GetWindow(hwnd, GW_OWNER);
    while (!TestExStyle(cur, WS_EX_APPWINDOW) && own) {
        HWND prev = cur; cur = own; own = GetWindow(own, GW_OWNER);
        if (TestExStyle(cur, WS_EX_TOOLWINDOW))
            return !TestExStyle(prev, WS_EX_CONTROLPARENT) || own != NULL;
    }
    return false;
}
static bool IsReallyVisible(HWND h) { RECT r; GetWindowRect(h, &r); return IsWindowVisible(h) && !IsRectEmpty(&r); }
static bool IsGhosted(HWND h) { return g_GhostWindowFromHungWindow && g_GhostWindowFromHungWindow(h) != NULL; }
static bool ShouldListInAltTab(HWND hwnd) {
    if (!IsWindow(hwnd)) return false;
    DWORD ex = (DWORD)GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    HWND own = GetWindow(hwnd, GW_OWNER);
    bool ownVis = IsWindow(own) && IsWindowEnabled(own) && IsReallyVisible(own);
    bool noAct = (ex & WS_EX_NOACTIVATE) || (ex & WS_EX_TOOLWINDOW);
    if (ex & WS_EX_APPWINDOW) noAct = false;
    return IsReallyVisible(hwnd) && !noAct && ((ex & WS_EX_APPWINDOW) || (!ownVis && !IsOwnerToolWindow(hwnd))) && !IsGhosted(hwnd);
}
static bool IsAltTabWindow(HWND h) {
    if (!IsWindow(h)) return false;
    if (g_IsShellFrameWindow && g_IsShellFrameWindow(h) && !(g_GhostWindowFromHungWindow && g_GhostWindowFromHungWindow(h))) return true;
    if (g_IsShellManagedWindow && g_IsShellManagedWindow(h) && !GetPropW(h, L"Microsoft.Windows.ShellManagedWindowAsNormalWindow")) return false;
    if (GetPropW(h, L"valinet.ExplorerPatcher.ShellManagedWindow")) return false;
    return ShouldListInAltTab(h);
}


// Window Enumeration

static HICON TryGetUwpIconFromExplorer(HWND hWnd, int desiredSizePx);

static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
    auto* list = reinterpret_cast<std::vector<WindowEntry>*>(lParam);
    if (hWnd == g_hSwitcher) return TRUE;
    if (!IsAltTabWindow(hWnd)) return TRUE;
    BOOL cloaked = FALSE;
    DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (cloaked) return TRUE;
    if (g_settings.perMonitorWindows && g_hCurrentMonitor) {
        if (MonitorFromWindow(hWnd, MONITOR_DEFAULTTONULL) != g_hCurrentMonitor) return TRUE;
    }
    WindowEntry e = {};
    e.hWnd = hWnd;
    InternalGetWindowText(hWnd, e.title, 256);
    if (!e.title[0]) GetWindowTextW(hWnd, e.title, 256);
    e.hIcon = NULL;
    
    bool isUwp = false;
    if (g_IsShellFrameWindow && g_IsShellFrameWindow(hWnd)) {
        isUwp = true;
    }
    
    if (isUwp) {
        e.hIcon = TryGetUwpIconFromExplorer(hWnd, GetHeaderIconSizePx());
    }
    
    if (!e.hIcon) SendMessageTimeoutW(hWnd, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 100, (DWORD_PTR*)&e.hIcon);
    if (!e.hIcon) SendMessageTimeoutW(hWnd, WM_GETICON, ICON_SMALL2, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 100, (DWORD_PTR*)&e.hIcon);
    if (!e.hIcon) SendMessageTimeoutW(hWnd, WM_GETICON, ICON_SMALL, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 100, (DWORD_PTR*)&e.hIcon);
    if (!e.hIcon) e.hIcon = (HICON)GetClassLongPtrW(hWnd, GCLP_HICON);
    if (!e.hIcon) e.hIcon = (HICON)GetClassLongPtrW(hWnd, GCLP_HICONSM);
    if (!e.hIcon) e.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    list->push_back(e);
    return TRUE;
}

// === UWP Icon Extraction (Explorer IPC) ===

UINT g_WM_SWS_GET_UWP_ICON = 0;
std::map<std::wstring, HICON> g_uwpIconCache;

struct FindCoreWindowData { HWND coreHwnd; };

static BOOL CALLBACK FindCoreWindowProc(HWND hChild, LPARAM lParam) {
    auto* data = (FindCoreWindowData*)lParam;
    WCHAR cls[256];
    GetClassNameW(hChild, cls, 256);
    if (wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
        data->coreHwnd = hChild;
        Wh_Log(L"Explorer IPC: Found CoreWindow %p for UWP app", hChild);
        return FALSE;
    }
    return TRUE;
}

static HICON ResolveIconFromAumid(const WCHAR* aumid, int desiredSizePx) {
    HICON hIcon = NULL;
    Wh_Log(L"ResolveIconFromAumid: aumid=%s, desiredSizePx=%d", aumid, desiredSizePx);
    
    IShellItem* psi = NULL;
    HRESULT hr = SHCreateItemInKnownFolder(
            FOLDERID_AppsFolder, KF_FLAG_DONT_VERIFY,
            aumid, IID_PPV_ARGS(&psi));
    if (SUCCEEDED(hr) && psi) {
        Wh_Log(L"ResolveIconFromAumid: SHCreateItemInKnownFolder succeeded");
        IShellItemImageFactory* psiif = NULL;
        hr = psi->QueryInterface(IID_PPV_ARGS(&psiif));
        if (SUCCEEDED(hr) && psiif) {
            Wh_Log(L"ResolveIconFromAumid: QueryInterface(IShellItemImageFactory) succeeded");
            SIZE sz = { desiredSizePx, desiredSizePx };
            HBITMAP hBitmap = NULL;
            hr = psiif->GetImage(sz, SIIGBF_RESIZETOFIT | SIIGBF_ICONONLY, &hBitmap);
            if (SUCCEEDED(hr) && hBitmap) {
                Wh_Log(L"ResolveIconFromAumid: GetImage succeeded");
                HIMAGELIST hImageList = ImageList_Create(sz.cx, sz.cy, ILC_COLOR32, 1, 0);
                if (hImageList) {
                    if (ImageList_Add(hImageList, hBitmap, NULL) != -1) {
                        hIcon = ImageList_GetIcon(hImageList, 0, 0);
                        if (hIcon) Wh_Log(L"ResolveIconFromAumid: Successfully converted to HICON");
                        else Wh_Log(L"ResolveIconFromAumid: ImageList_GetIcon failed");
                    } else {
                        Wh_Log(L"ResolveIconFromAumid: ImageList_Add failed");
                    }
                    ImageList_Destroy(hImageList);
                } else {
                    Wh_Log(L"ResolveIconFromAumid: ImageList_Create failed");
                }
                DeleteObject(hBitmap);
            } else {
                Wh_Log(L"ResolveIconFromAumid: GetImage failed, hr=0x%08X", hr);
            }
            psiif->Release();
        } else {
            Wh_Log(L"ResolveIconFromAumid: QueryInterface failed, hr=0x%08X", hr);
        }
        psi->Release();
    } else {
        Wh_Log(L"ResolveIconFromAumid: SHCreateItemInKnownFolder failed, hr=0x%08X", hr);
    }
    
    // Fallback: SHParseDisplayName + SHGetFileInfo
    if (!hIcon) {
        Wh_Log(L"ResolveIconFromAumid: Falling back to SHParseDisplayName");
        WCHAR appsFolderPath[768];
        if (swprintf_s(appsFolderPath, L"shell:AppsFolder\\%s", aumid) > 0) {
            PIDLIST_ABSOLUTE pidl = NULL;
            hr = SHParseDisplayName(appsFolderPath, NULL, &pidl, 0, NULL);
            if (SUCCEEDED(hr) && pidl) {
                Wh_Log(L"ResolveIconFromAumid: SHParseDisplayName succeeded");
                SHFILEINFOW sfi = {};
                UINT flags = SHGFI_PIDL | SHGFI_ICON | (desiredSizePx > 24 ? SHGFI_LARGEICON : SHGFI_SMALLICON);
                if (SHGetFileInfoW((LPCWSTR)pidl, 0, &sfi, sizeof(sfi), flags)) {
                    hIcon = sfi.hIcon;
                    if (hIcon) Wh_Log(L"ResolveIconFromAumid: SHGetFileInfoW succeeded");
                    else Wh_Log(L"ResolveIconFromAumid: SHGetFileInfoW returned no icon");
                } else {
                    Wh_Log(L"ResolveIconFromAumid: SHGetFileInfoW failed");
                }
                CoTaskMemFree(pidl);
            } else {
                Wh_Log(L"ResolveIconFromAumid: SHParseDisplayName failed, hr=0x%08X", hr);
            }
        }
    }
    return hIcon;
}

LRESULT CALLBACK ExplorerIpcWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (g_WM_SWS_GET_UWP_ICON && uMsg == g_WM_SWS_GET_UWP_ICON) {
        HWND hWndTarget = (HWND)wParam;
        int desiredSizePx = (int)lParam;
        
        Wh_Log(L"Explorer IPC: Received icon request for HWND %p, size %d", hWndTarget, desiredSizePx);
        
        std::wstring aumid;
        
        {
            IPropertyStore* ps = NULL;
            if (SUCCEEDED(SHGetPropertyStoreForWindow(hWndTarget, IID_PPV_ARGS(&ps))) && ps) {
                PROPVARIANT pv;
                PropVariantInit(&pv);
                if (SUCCEEDED(ps->GetValue(PKEY_AppUserModel_ID, &pv)) && pv.vt == VT_LPWSTR && pv.pwszVal && pv.pwszVal[0]) {
                    aumid = pv.pwszVal;
                    Wh_Log(L"Explorer IPC: Got AUMID from PropertyStore = %s", aumid.c_str());
                }
                PropVariantClear(&pv);
                ps->Release();
            }
        }
        
        if (aumid.empty()) {
            Wh_Log(L"Explorer IPC: PropertyStore failed or empty, trying Process Handle fallback");
            FindCoreWindowData data = {0};
            EnumChildWindows(hWndTarget, FindCoreWindowProc, (LPARAM)&data);
            HWND hCore = data.coreHwnd ? data.coreHwnd : hWndTarget;
            
            DWORD pid = 0;
            GetWindowThreadProcessId(hCore, &pid);
            
            if (pid) {
                HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
                if (hProc) {
                    UINT32 aumidLen = 0;
                    LONG rc = GetApplicationUserModelId(hProc, &aumidLen, NULL);
                    if (rc == ERROR_INSUFFICIENT_BUFFER && aumidLen > 0) {
                        WCHAR* buf = new WCHAR[aumidLen];
                        if (GetApplicationUserModelId(hProc, &aumidLen, buf) == ERROR_SUCCESS) {
                            aumid = buf;
                            Wh_Log(L"Explorer IPC: Got AUMID from Process = %s", aumid.c_str());
                        }
                        delete[] buf;
                    } else {
                        Wh_Log(L"Explorer IPC: GetApplicationUserModelId failed, rc=%d", rc);
                    }
                    CloseHandle(hProc);
                } else {
                    Wh_Log(L"Explorer IPC: OpenProcess failed, err=%u", GetLastError());
                }
            }
        }
        
        if (!aumid.empty()) {
            Wh_Log(L"Explorer IPC: Got AUMID = %s", aumid.c_str());
            
            std::wstring cacheKey = aumid + L"_" + std::to_wstring(desiredSizePx);
            if (g_uwpIconCache.find(cacheKey) != g_uwpIconCache.end()) {
                Wh_Log(L"Explorer IPC: Returning cached icon %p", g_uwpIconCache[cacheKey]);
                return (LRESULT)g_uwpIconCache[cacheKey];
            }
            
            HICON hIcon = ResolveIconFromAumid(aumid.c_str(), desiredSizePx);
            if (hIcon) {
                Wh_Log(L"Explorer IPC: Resolved new icon %p", hIcon);
                g_uwpIconCache[cacheKey] = hIcon;
                return (LRESULT)hIcon;
            } else {
                Wh_Log(L"Explorer IPC: ResolveIconFromAumid failed");
            }
        } else {
            Wh_Log(L"Explorer IPC: Failed to obtain AUMID for UWP app");
        }
        return NULL;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

static DWORD WINAPI ExplorerIpcThread(LPVOID) {
    HRESULT hrCo = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = ExplorerIpcWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"WindhawkSWS_IpcWindow";
    RegisterClassW(&wc);
    
    CreateWindowExW(0, L"WindhawkSWS_IpcWindow", L"", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, wc.hInstance, NULL);
    
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    
    if (SUCCEEDED(hrCo)) CoUninitialize();
    return 0;
}

static HICON TryGetUwpIconFromExplorer(HWND hWnd, int desiredSizePx) {
    if (!g_WM_SWS_GET_UWP_ICON) {
        g_WM_SWS_GET_UWP_ICON = RegisterWindowMessageW(L"Windhawk_SWS_GetUwpIcon");
        Wh_Log(L"TryGetUwpIconFromExplorer: Registered message %u", g_WM_SWS_GET_UWP_ICON);
    }
    HWND hIpc = FindWindowW(L"WindhawkSWS_IpcWindow", NULL);
    if (hIpc) {
        DWORD_PTR res = 0;
        LRESULT sendRes = SendMessageTimeoutW(hIpc, g_WM_SWS_GET_UWP_ICON, (WPARAM)hWnd, desiredSizePx, SMTO_ABORTIFHUNG | SMTO_BLOCK, 1000, &res);
        Wh_Log(L"TryGetUwpIconFromExplorer: SendMessageTimeoutW to %p returned %ld, res = %p", hIpc, sendRes, res);
        return (HICON)res;
    } else {
        Wh_Log(L"TryGetUwpIconFromExplorer: WindhawkSWS_IpcWindow not found");
    }
    return NULL;
}

static void BuildWindowList() {
    for (auto& w : g_windows) if (w.hThumb) { DwmUnregisterThumbnail(w.hThumb); w.hThumb = NULL; }
    g_windows.clear();
    EnumWindows(EnumWindowsProc, (LPARAM)&g_windows);
}

// Layout + Thumbnails

static int DpiScale(int val, int dpi) { return MulDiv(val, dpi, 96); }

static HFONT CreateScaledFont(int dpiY) {
    NONCLIENTMETRICSW ncm = { sizeof(ncm) };
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    typedef BOOL(WINAPI* SPIFD)(UINT, UINT, PVOID, UINT, UINT);
    SPIFD sysParamInfoForDpi = hUser32 ? (SPIFD)GetProcAddress(hUser32, "SystemParametersInfoForDpi") : NULL;
    if (sysParamInfoForDpi) {
        sysParamInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0, dpiY);
    } else {
        SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
    }
    LOGFONTW lf = ncm.lfMessageFont;
    lf.lfWeight = FW_NORMAL;
    return CreateFontIndirectW(&lf);
}

static void RegisterThumbnailsEarly() {
    if (!g_settings.showThumbnails || !g_hSwitcher) return;
    for (auto& w : g_windows) {
        if (!w.hThumb) {
            if (SUCCEEDED(DwmRegisterThumbnail(g_hSwitcher, w.hWnd, &w.hThumb))) {
                SIZE src = {0}; DwmQueryThumbnailSourceSize(w.hThumb, &src);
                w.sourceSize = src;

                // Compute invisible frame crop using DWMWA_EXTENDED_FRAME_BOUNDS
                // This fixes thumbnail displacement for maximized windows where
                // the window extends beyond screen edges to hide the frame.
                // Skip for minimized windows: GetWindowRect/DWMWA_EXTENDED_FRAME_BOUNDS
                // return garbage coords (-32000) for iconic windows, causing zoom.
                if (!IsIconic(w.hWnd)) {
                    RECT wr = {0}, efb = {0};
                    GetWindowRect(w.hWnd, &wr);
                    if (SUCCEEDED(DwmGetWindowAttribute(w.hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &efb, sizeof(efb)))) {
                        int wrW = wr.right - wr.left, wrH = wr.bottom - wr.top;
                        if (wrW > 0 && wrH > 0 && src.cx > 0 && src.cy > 0) {
                            double sx = (double)src.cx / wrW;
                            double sy = (double)src.cy / wrH;
                            int ml = (int)((efb.left - wr.left) * sx);
                            int mt = (int)((efb.top - wr.top) * sy);
                            int mr = (int)((wr.right - efb.right) * sx);
                            int mb = (int)((wr.bottom - efb.bottom) * sy);
                            if (ml < 0) ml = 0; if (mt < 0) mt = 0;
                            if (mr < 0) mr = 0; if (mb < 0) mb = 0;
                            w.rcSourceCrop = { ml, mt, src.cx - mr, src.cy - mb };
                            w.effectiveSourceSize = { src.cx - ml - mr, src.cy - mt - mb };
                            if (w.effectiveSourceSize.cx <= 0 || w.effectiveSourceSize.cy <= 0) {
                                w.effectiveSourceSize = src;
                                w.rcSourceCrop = { 0, 0, src.cx, src.cy };
                            }
                        } else {
                            w.effectiveSourceSize = src;
                            w.rcSourceCrop = { 0, 0, src.cx, src.cy };
                        }
                    } else {
                        w.effectiveSourceSize = src;
                        w.rcSourceCrop = { 0, 0, src.cx, src.cy };
                    }
                } else {
                    w.effectiveSourceSize = src;
                    w.rcSourceCrop = { 0, 0, src.cx, src.cy };
                }
            } else {
                w.sourceSize = {0, 0};
                w.effectiveSourceSize = {0, 0};
                w.rcSourceCrop = {0, 0, 0, 0};
            }
        }
    }
}

static void ComputeLayout(HMONITOR hMon) {
    MONITORINFO mi = { sizeof(mi) }; GetMonitorInfoW(hMon, &mi);
    int monW = mi.rcWork.right - mi.rcWork.left, monH = mi.rcWork.bottom - mi.rcWork.top;
    UINT dpiX = 96, dpiY = 96;
    HMODULE hShcore = LoadLibraryW(L"shcore.dll");
    if (hShcore) {
        typedef HRESULT(WINAPI*GDPFM)(HMONITOR,int,UINT*,UINT*);
        auto fn = (GDPFM)GetProcAddress(hShcore, "GetDpiForMonitor");
        if (fn) fn(hMon, 0, &dpiX, &dpiY);
        FreeLibrary(hShcore);
    }
    g_dpiX = dpiX; g_dpiY = dpiY;

    int n = (int)g_windows.size();
    if (n == 0) { g_winW = 0; g_winH = 0; return; }

    // DPI-scale all EP padding constants
    int masterPad    = DpiScale(SWS_MASTER_PADDING, dpiX);
    int elemPadTop   = DpiScale(SWS_ELEMENT_PAD_TOP, dpiY);
    int elemPadBot   = DpiScale(SWS_ELEMENT_PAD_BOTTOM, dpiY);
    int elemPadLeft  = DpiScale(SWS_ELEMENT_PAD_LEFT, dpiX);
    int elemPadRight = DpiScale(SWS_ELEMENT_PAD_RIGHT, dpiX);
    int padTop       = DpiScale(SWS_PAD_TOP, dpiY);
    int padBot       = DpiScale(SWS_PAD_BOTTOM, dpiY);
    int padLeft      = DpiScale(SWS_PAD_LEFT, dpiX);
    int padRight     = DpiScale(SWS_PAD_RIGHT, dpiX);
    int padDivider   = DpiScale(SWS_PAD_DIVIDER, dpiY);
    int rowTitleH    = GetHeaderRowHeightPx();

    // EP: cbThumbnailAvailableHeight = cbRowHeight - cbRowTitleHeight - cbTopPadding - 2 * cbBottomPadding
    // All values are DPI-scaled at this point (matching EP lines 826-844)
    int scaledRowH = DpiScale(g_settings.rowHeight, dpiY);
    int thumbH = 0;
    if (g_settings.showThumbnails) {
        thumbH = scaledRowH - rowTitleH - padTop - 2 * padBot;
        if (thumbH < 0) thumbH = 0;
    }
    // EP: cbMaxTileWidth = cbRowHeight * MAX_TILE_WIDTH (computed before DPI, then scaled)
    int maxTileW = DpiScale((int)(g_settings.rowHeight * SWS_MAX_TILE_ASPECT), dpiX);

    // EP helper equivalents:
    // initialLeft  = elemPadLeft + padLeft
    // rightInc     = (padRight + elemPadRight) + initialLeft
    // initialTop   = elemPadTop + (padTop + rowTitleH + padDivider)
    // bottomInc    = (thumbH + padBot) + elemPadBot + initialTop
    int initialLeft = elemPadLeft + padLeft;
    int rightInc    = (padRight + elemPadRight) + initialLeft;
    int initialTop  = elemPadTop + (padTop + rowTitleH + padDivider);
    int bottomInc   = (thumbH + padBot) + elemPadBot + initialTop;

    int maxW = monW * g_settings.maxWidthPercent / 100;
    int maxH = monH * g_settings.maxHeightPercent / 100;

    int curX = initialLeft + masterPad;
    int curY = initialTop + masterPad;
    int placedCount = n; // Track how many windows were actually placed

    auto truncateRemaining = [&](int startIdx) {
        for (int jj = startIdx; jj < n; jj++) {
            int ji = (g_layoutStartIndex + jj) % n;
            g_windows[ji].sourceSize = {0, 0};
            g_windows[ji].rcCell = {0, 0, 0, 0};
            g_windows[ji].rcThumbActual = {0, 0, 0, 0};
            g_windows[ji].rcThumb = {0, 0, 0, 0};
            if (g_windows[ji].hThumb) {
                DwmUnregisterThumbnail(g_windows[ji].hThumb);
                g_windows[ji].hThumb = NULL;
            }
        }
        placedCount = startIdx;
    };

    if (!LayoutIsVertical()) {
        int maxRowW = 0;

        for (int idx = 0; idx < n; idx++) {
            int i = (g_layoutStartIndex + idx) % n;
            auto& w = g_windows[i];

            if (g_layoutStartIndex > 0 && idx > 0 && i < g_layoutStartIndex
                && ((g_layoutStartIndex + idx - 1) % n) >= g_layoutStartIndex
                && curX > initialLeft + masterPad) {
                if (curX - initialLeft > maxRowW) maxRowW = curX - initialLeft;
                curX = initialLeft + masterPad;
                if (curY + 2 * bottomInc - initialTop > maxH - masterPad) {
                    truncateRemaining(idx);
                    break;
                }
                curY = curY + bottomInc;
            }

            int width = 0;
            int original_width = 0;

            if (g_settings.showThumbnails && thumbH > 0) {
                if (w.effectiveSourceSize.cx > 0 && w.effectiveSourceSize.cy > 0) {
                    width = (int)((double)w.effectiveSourceSize.cx * thumbH / w.effectiveSourceSize.cy);
                } else {
                    width = thumbH;
                }
                if (width > maxTileW || width > w.effectiveSourceSize.cx) {
                    original_width = width;
                    if (width > maxTileW) width = maxTileW;
                    if (w.effectiveSourceSize.cx > 0 && width > w.effectiveSourceSize.cx) width = w.effectiveSourceSize.cx;
                }
            } else {
                width = DpiScale(160, dpiX);
            }

            if (g_settings.rowWidth > 0) {
                width = DpiScale(g_settings.rowWidth, dpiX);
            }

            if (curX + width + rightInc - initialLeft > maxW - masterPad && curX > initialLeft + masterPad) {
                if (curX - initialLeft > maxRowW) maxRowW = curX - initialLeft;
                curX = initialLeft + masterPad;

                if (curY + 2 * bottomInc - initialTop > maxH - masterPad) {
                    truncateRemaining(idx);
                    break;
                }

                curY = curY + bottomInc;
            }

            int actualThumbH = thumbH;
            if (original_width > 0 && thumbH > 0) {
                actualThumbH = (int)((double)width * thumbH / original_width);
            }

            w.rcCell.left   = curX - initialLeft + elemPadLeft;
            w.rcCell.top    = curY - initialTop + elemPadTop;
            w.rcCell.right  = curX + width + rightInc - initialLeft - elemPadRight;
            w.rcCell.bottom = curY + bottomInc - initialTop - elemPadBot;
            if (original_width > 0) {
                w.rcCell.bottom -= (thumbH - actualThumbH);
            }

            if (g_settings.showThumbnails) {
                w.rcThumbActual = { curX, curY, curX + width, curY + actualThumbH };
                w.rcThumb = w.rcThumbActual;
            }

            curX = curX + width + rightInc;
            placedCount = idx + 1;
        }

        if (curX - initialLeft > maxRowW) maxRowW = curX - initialLeft;
        g_winW = maxRowW + masterPad;
        g_winH = curY + bottomInc - initialTop + masterPad;
        if (g_winW > maxW) g_winW = maxW;

        for (int idx = 0; idx < placedCount; idx++) {
            int i = (g_layoutStartIndex + idx) % n;
            int rowTop = g_windows[i].rcCell.top;
            int rowMaxRight = 0;
            for (int jdx = idx; jdx < placedCount; jdx++) {
                int j = (g_layoutStartIndex + jdx) % n;
                if (g_windows[j].rcCell.top != rowTop) break;
                if (g_windows[j].rcCell.right > rowMaxRight) rowMaxRight = g_windows[j].rcCell.right;
            }
            int diff = (g_winW - masterPad > rowMaxRight) ? (g_winW - masterPad - rowMaxRight) / 2 : 0;
            if (diff > 0) {
                for (int jdx = idx; jdx < placedCount; jdx++) {
                    int j = (g_layoutStartIndex + jdx) % n;
                    if (g_windows[j].rcCell.top != rowTop) break;
                    g_windows[j].rcCell.left += diff;
                    g_windows[j].rcCell.right += diff;
                    g_windows[j].rcThumbActual.left += diff;
                    g_windows[j].rcThumbActual.right += diff;
                    g_windows[j].rcThumb.left += diff;
                    g_windows[j].rcThumb.right += diff;
                }
            }
            while (idx + 1 < placedCount && g_windows[(g_layoutStartIndex + idx + 1) % n].rcCell.top == rowTop) idx++;
        }
    } else {
        int curColMaxW = 0;
        int maxRight = 0;
        int maxBottom = 0;

        for (int idx = 0; idx < n; idx++) {
            int i = (g_layoutStartIndex + idx) % n;
            auto& w = g_windows[i];

            if (g_layoutStartIndex > 0 && idx > 0 && i < g_layoutStartIndex
                && ((g_layoutStartIndex + idx - 1) % n) >= g_layoutStartIndex
                && curY > initialTop + masterPad) {
                curY = initialTop + masterPad;
                curX = curX + curColMaxW + rightInc;
                curColMaxW = 0;
                if (curX + rightInc - initialLeft > maxW - masterPad) {
                    truncateRemaining(idx);
                    break;
                }
            }

            int width = 0;
            int original_width = 0;

            if (g_settings.showThumbnails && thumbH > 0) {
                if (w.effectiveSourceSize.cx > 0 && w.effectiveSourceSize.cy > 0) {
                    width = (int)((double)w.effectiveSourceSize.cx * thumbH / w.effectiveSourceSize.cy);
                } else {
                    width = thumbH;
                }
                if (width > maxTileW || width > w.effectiveSourceSize.cx) {
                    original_width = width;
                    if (width > maxTileW) width = maxTileW;
                    if (w.effectiveSourceSize.cx > 0 && width > w.effectiveSourceSize.cx) width = w.effectiveSourceSize.cx;
                }
            } else {
                width = DpiScale(160, dpiX);
            }

            if (g_settings.rowWidth > 0) {
                width = DpiScale(g_settings.rowWidth, dpiX);
            }

            if (curY + bottomInc - initialTop > maxH - masterPad && curY > initialTop + masterPad) {
                curY = initialTop + masterPad;
                curX = curX + curColMaxW + rightInc;
                curColMaxW = 0;
                if (curX + width + rightInc - initialLeft > maxW - masterPad) {
                    truncateRemaining(idx);
                    break;
                }
            }

            int actualThumbH = thumbH;
            if (original_width > 0 && thumbH > 0) {
                actualThumbH = (int)((double)width * thumbH / original_width);
            }

            w.rcCell.left   = curX - initialLeft + elemPadLeft;
            w.rcCell.top    = curY - initialTop + elemPadTop;
            w.rcCell.right  = curX + width + rightInc - initialLeft - elemPadRight;
            w.rcCell.bottom = curY + bottomInc - initialTop - elemPadBot;
            if (original_width > 0) {
                w.rcCell.bottom -= (thumbH - actualThumbH);
            }

            if (g_settings.showThumbnails) {
                w.rcThumbActual = { curX, curY, curX + width, curY + actualThumbH };
                w.rcThumb = w.rcThumbActual;
            }

            if (width > curColMaxW) curColMaxW = width;
            if (w.rcCell.right > maxRight) maxRight = w.rcCell.right;
            if (w.rcCell.bottom > maxBottom) maxBottom = w.rcCell.bottom;

            curY = curY + bottomInc;
            placedCount = idx + 1;
        }

        g_winW = maxRight + masterPad;
        g_winH = maxBottom + masterPad;
        if (g_winW > maxW) g_winW = maxW;
        if (g_winH > maxH) g_winH = maxH;

        for (int idx = 0; idx < placedCount; idx++) {
            int i = (g_layoutStartIndex + idx) % n;
            int colLeft = g_windows[i].rcCell.left;
            int colMaxBottom = 0;

            for (int jdx = idx; jdx < placedCount; jdx++) {
                int j = (g_layoutStartIndex + jdx) % n;
                if (g_windows[j].rcCell.left != colLeft) break;
                if (g_windows[j].rcCell.bottom > colMaxBottom) colMaxBottom = g_windows[j].rcCell.bottom;
            }

            int diff = (g_winH - masterPad > colMaxBottom) ? (g_winH - masterPad - colMaxBottom) / 2 : 0;
            if (diff > 0) {
                for (int jdx = idx; jdx < placedCount; jdx++) {
                    int j = (g_layoutStartIndex + jdx) % n;
                    if (g_windows[j].rcCell.left != colLeft) break;
                    g_windows[j].rcCell.top += diff;
                    g_windows[j].rcCell.bottom += diff;
                    g_windows[j].rcThumbActual.top += diff;
                    g_windows[j].rcThumbActual.bottom += diff;
                    g_windows[j].rcThumb.top += diff;
                    g_windows[j].rcThumb.bottom += diff;
                }
            }

            while (idx + 1 < placedCount && g_windows[(g_layoutStartIndex + idx + 1) % n].rcCell.left == colLeft) idx++;
        }
    }
}

static void RegisterThumbnails() {
    if (!g_settings.showThumbnails || !g_hSwitcher) return;
    for (auto& w : g_windows) {
        if (!w.hThumb) {
            if (SUCCEEDED(DwmRegisterThumbnail(g_hSwitcher, w.hWnd, &w.hThumb))) {
                SIZE src = {0}; DwmQueryThumbnailSourceSize(w.hThumb, &src);
                w.sourceSize = src;
            }
        }
        if (w.hThumb) {
            // Skip truncated windows with zero destination rect
            if (w.rcThumbActual.left == 0 && w.rcThumbActual.right == 0 &&
                w.rcThumbActual.top == 0 && w.rcThumbActual.bottom == 0) continue;
            DWM_THUMBNAIL_PROPERTIES p = {};
            p.dwFlags = DWM_TNP_SOURCECLIENTAREAONLY | DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_OPACITY;
            p.fSourceClientAreaOnly = FALSE;
            p.rcDestination = w.rcThumbActual;
            p.opacity = 255; p.fVisible = TRUE;
            // Only set DWM_TNP_RECTSOURCE when the crop is non-trivial (e.g. maximized
            // windows with invisible frame borders). Without it, DWM preserves the
            // window's visual style including rounded corners on Windows 11.
            bool needsCrop = (w.rcSourceCrop.left != 0 || w.rcSourceCrop.top != 0 ||
                              w.rcSourceCrop.right != w.sourceSize.cx || w.rcSourceCrop.bottom != w.sourceSize.cy);
            if (needsCrop) {
                p.dwFlags |= DWM_TNP_RECTSOURCE;
                p.rcSource = w.rcSourceCrop;
            }
            DwmUpdateThumbnailProperties(w.hThumb, &p);
        }
    }
}
static void UnregisterThumbnails() {
    for (auto& w : g_windows) if (w.hThumb) { DwmUnregisterThumbnail(w.hThumb); w.hThumb = NULL; }
}


// Drawing Helpers

static COLORREF GetContourColor() {
    if (g_settings.useAccentColor) return GetAccentColor();

    COLORREF parsed;
    if (g_isDarkMode) {
        if (ParseHexColor(g_settings.borderColorDark, &parsed)) return parsed;
    } else {
        if (ParseHexColor(g_settings.borderColorLight, &parsed)) return parsed;
    }

    return g_isDarkMode ? SWS_CONTOUR_DARK : SWS_CONTOUR_LIGHT;
}

static void MaskRectCorners(HDC hdc, const RECT& rc, int radiusPx) {
    if (radiusPx <= 0) {
        return;
    }

    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0) {
        return;
    }

    int r = radiusPx;
    if (r * 2 > w) r = w / 2;
    if (r * 2 > h) r = h / 2;
    if (r <= 0) {
        return;
    }

    COLORREF bg = g_isDarkMode ? SWS_BG_DARK : SWS_BG_LIGHT;
    // In layered mode, punch fully transparent corners to force thumbnail clipping.
    BYTE alpha = ThemeIs(L"none") ? 0 : 255;

    Gdiplus::Graphics graphics(hdc);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    Gdiplus::SolidBrush brush(Gdiplus::Color(alpha, GetRValue(bg), GetGValue(bg), GetBValue(bg)));

    int d = r * 2;
    Gdiplus::GraphicsPath cutTl, cutTr, cutBr, cutBl;

    cutTl.StartFigure();
    cutTl.AddLine((Gdiplus::REAL)rc.left, (Gdiplus::REAL)rc.top + r, (Gdiplus::REAL)rc.left, (Gdiplus::REAL)rc.top);
    cutTl.AddLine((Gdiplus::REAL)rc.left, (Gdiplus::REAL)rc.top, (Gdiplus::REAL)rc.left + r, (Gdiplus::REAL)rc.top);
    cutTl.AddArc((Gdiplus::REAL)rc.left, (Gdiplus::REAL)rc.top, (Gdiplus::REAL)d, (Gdiplus::REAL)d, 270, -90);
    cutTl.CloseFigure();

    cutTr.StartFigure();
    cutTr.AddLine((Gdiplus::REAL)rc.right - r, (Gdiplus::REAL)rc.top, (Gdiplus::REAL)rc.right, (Gdiplus::REAL)rc.top);
    cutTr.AddLine((Gdiplus::REAL)rc.right, (Gdiplus::REAL)rc.top, (Gdiplus::REAL)rc.right, (Gdiplus::REAL)rc.top + r);
    cutTr.AddArc((Gdiplus::REAL)rc.right - d, (Gdiplus::REAL)rc.top, (Gdiplus::REAL)d, (Gdiplus::REAL)d, 0, -90);
    cutTr.CloseFigure();

    cutBr.StartFigure();
    cutBr.AddLine((Gdiplus::REAL)rc.right, (Gdiplus::REAL)rc.bottom - r, (Gdiplus::REAL)rc.right, (Gdiplus::REAL)rc.bottom);
    cutBr.AddLine((Gdiplus::REAL)rc.right, (Gdiplus::REAL)rc.bottom, (Gdiplus::REAL)rc.right - r, (Gdiplus::REAL)rc.bottom);
    cutBr.AddArc((Gdiplus::REAL)rc.right - d, (Gdiplus::REAL)rc.bottom - d, (Gdiplus::REAL)d, (Gdiplus::REAL)d, 90, -90);
    cutBr.CloseFigure();

    cutBl.StartFigure();
    cutBl.AddLine((Gdiplus::REAL)rc.left + r, (Gdiplus::REAL)rc.bottom, (Gdiplus::REAL)rc.left, (Gdiplus::REAL)rc.bottom);
    cutBl.AddLine((Gdiplus::REAL)rc.left, (Gdiplus::REAL)rc.bottom, (Gdiplus::REAL)rc.left, (Gdiplus::REAL)rc.bottom - r);
    cutBl.AddArc((Gdiplus::REAL)rc.left, (Gdiplus::REAL)rc.bottom - d, (Gdiplus::REAL)d, (Gdiplus::REAL)d, 180, -90);
    cutBl.CloseFigure();

    graphics.FillPath(&brush, &cutTl);
    graphics.FillPath(&brush, &cutTr);
    graphics.FillPath(&brush, &cutBr);
    graphics.FillPath(&brush, &cutBl);
}

// Draw a sharp rectangular contour using StretchDIBits (EP's _DrawContour approach)
// direction: 1 = inner (shrinks inward), -1 = outer (grows outward)
static void DrawContour(HDC hdc, RECT rc, int contourSize, int direction) {
    COLORREF c = GetContourColor();
    BYTE r = GetRValue(c), g = GetGValue(c), b = GetBValue(c);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = 1; bi.bmiHeader.biHeight = 1;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
    RGBQUAD px = { b, g, r, 0xFF };

    int t = direction * (contourSize * g_dpiX / 96);

    int cornerRadius = GetTaskUiCornerRadiusPx();
    if (cornerRadius > 0 && direction > 0) {
        int penWidth = contourSize * g_dpiX / 96;
        if (penWidth < 1) penWidth = 1;

        RECT drawRc = rc;
        int width = drawRc.right - drawRc.left - penWidth;
        int height = drawRc.bottom - drawRc.top - penWidth;
        if (width <= 0 || height <= 0) {
            return;
        }

        if (cornerRadius * 2 > width) cornerRadius = width / 2;
        if (cornerRadius * 2 > height) cornerRadius = height / 2;

        Gdiplus::Graphics graphics(hdc);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        Gdiplus::Pen pen(Gdiplus::Color(255, r, g, b), (Gdiplus::REAL)penWidth);

        Gdiplus::REAL left = (Gdiplus::REAL)drawRc.left + penWidth / 2.0f;
        Gdiplus::REAL top = (Gdiplus::REAL)drawRc.top + penWidth / 2.0f;
        Gdiplus::REAL w = (Gdiplus::REAL)width;
        Gdiplus::REAL h = (Gdiplus::REAL)height;
        Gdiplus::REAL d = (Gdiplus::REAL)(cornerRadius * 2);
        Gdiplus::GraphicsPath path;
        path.AddArc(left, top, d, d, 180, 90);
        path.AddArc(left + w - d, top, d, d, 270, 90);
        path.AddArc(left + w - d, top + h - d, d, d, 0, 90);
        path.AddArc(left, top + h - d, d, d, 90, 90);
        path.CloseFigure();
        graphics.DrawPath(&pen, &path);
        return;
    }

    if (direction < 0) {
        // Outer contour (EP: SWS_CONTOUR_OUTER)
        StretchDIBits(hdc, rc.left + t, rc.top, -t, rc.bottom - rc.top, 0, 0, 1, 1, &px, &bi, DIB_RGB_COLORS, SRCCOPY);
        StretchDIBits(hdc, rc.right, rc.top, -t, rc.bottom - rc.top, 0, 0, 1, 1, &px, &bi, DIB_RGB_COLORS, SRCCOPY);
        StretchDIBits(hdc, rc.left + t, rc.top + t, (rc.right - rc.left) - t * 2, -t, 0, 0, 1, 1, &px, &bi, DIB_RGB_COLORS, SRCCOPY);
        StretchDIBits(hdc, rc.left + t, rc.bottom, (rc.right - rc.left) - t * 2, -t, 0, 0, 1, 1, &px, &bi, DIB_RGB_COLORS, SRCCOPY);
    } else {
        // Inner contour (EP: SWS_CONTOUR_INNER)
        StretchDIBits(hdc, rc.left, rc.top, t, rc.bottom - rc.top, 0, 0, 1, 1, &px, &bi, DIB_RGB_COLORS, SRCCOPY);
        StretchDIBits(hdc, rc.right - t, rc.top, t, rc.bottom - rc.top, 0, 0, 1, 1, &px, &bi, DIB_RGB_COLORS, SRCCOPY);
        StretchDIBits(hdc, rc.left, rc.top, rc.right - rc.left, t, 0, 0, 1, 1, &px, &bi, DIB_RGB_COLORS, SRCCOPY);
        StretchDIBits(hdc, rc.left, rc.bottom - t, rc.right - rc.left, t, 0, 0, 1, 1, &px, &bi, DIB_RGB_COLORS, SRCCOPY);
    }
}

// Shared drawing routine for both layered and buffered paint paths
static void DrawSwitcherContent(HDC hdc, bool fillBg) {
    RECT rcClient; GetClientRect(g_hSwitcher, &rcClient);
    int w = rcClient.right, h = rcClient.bottom;

    if (fillBg) {
        BYTE bgA = (BYTE)(g_settings.opacity * 255 / 100);
        COLORREF bgC = g_isDarkMode ? SWS_BG_DARK : SWS_BG_LIGHT;
        BYTE bgR = GetRValue(bgC), bgG = GetGValue(bgC), bgB = GetBValue(bgC);
        RGBQUAD bgPx = { (BYTE)(bgB*bgA/255), (BYTE)(bgG*bgA/255), (BYTE)(bgR*bgA/255), bgA };
        BITMAPINFO bgBi = {}; bgBi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bgBi.bmiHeader.biWidth = 1; bgBi.bmiHeader.biHeight = 1;
        bgBi.bmiHeader.biPlanes = 1; bgBi.bmiHeader.biBitCount = 32; bgBi.bmiHeader.biCompression = BI_RGB;
        StretchDIBits(hdc, 0, 0, w, h, 0, 0, 1, 1, &bgPx, &bgBi, DIB_RGB_COLORS, SRCCOPY);
    }

    HFONT hOldFont = (HFONT)SelectObject(hdc, g_hFont);
    SetBkMode(hdc, TRANSPARENT);

    // DPI-scale layout constants for drawing
    int padLeft    = DpiScale(SWS_PAD_LEFT, g_dpiX);
    int padTop     = DpiScale(SWS_PAD_TOP, g_dpiY);
    int rowTitleH  = GetHeaderRowHeightPx();
    int iconSz     = GetHeaderIconSizePx();
    int cornerRadius = GetTaskUiCornerRadiusPx();

    for (int i = 0; i < (int)g_windows.size(); i++) {
        auto& e = g_windows[i];

        // Skip truncated (not placed) windows
        if (e.rcCell.left == 0 && e.rcCell.right == 0 &&
            e.rcCell.top == 0 && e.rcCell.bottom == 0) continue;

        // Selection border: inner contour on rcCell
        if (i == g_selectedIndex) {
            DrawContour(hdc, e.rcCell, SWS_CONTOUR_SIZE, 1);
        }

        // Hover thumbnail border: outer contour on rcThumbActual (EP draws both independently)
        if (i == g_hoverIndex && g_settings.showThumbnails) {
            DrawContour(hdc, e.rcThumbActual, 1, -1);
        }

        if (g_settings.showThumbnails && cornerRadius > 0) {
            MaskRectCorners(hdc, e.rcThumbActual, cornerRadius);
            RECT inset = e.rcThumbActual;
            InflateRect(&inset, -1, -1);
            MaskRectCorners(hdc, inset, cornerRadius);
        }

        // Close button (positioned at top-right of the cell, in title area)
        if (i == g_hoverIndex) {
            int btnSz = DpiScale(24, g_dpiX);
            int bx = e.rcCell.right - padLeft - btnSz;
            int by = HeaderIsVertical() ? (e.rcCell.top + padTop)
                                        : (e.rcCell.top + padTop + (rowTitleH - btnSz) / 2);

            if (g_isCloseHovered) {
                // Red rounded background for close button
                Gdiplus::Graphics graphics(hdc);
                graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
                int btnRadius = GetCloseButtonCornerRadiusPx();
                Gdiplus::SolidBrush redBrush(Gdiplus::Color(255, 196, 43, 28));
                if (btnRadius > 0) {
                    if (btnRadius * 2 > btnSz) btnRadius = btnSz / 2;
                    Gdiplus::GraphicsPath path;
                    Gdiplus::REAL d = (Gdiplus::REAL)(btnRadius * 2);
                    path.AddArc((Gdiplus::REAL)bx, (Gdiplus::REAL)by, d, d, 180, 90);
                    path.AddArc((Gdiplus::REAL)(bx + btnSz) - d, (Gdiplus::REAL)by, d, d, 270, 90);
                    path.AddArc((Gdiplus::REAL)(bx + btnSz) - d, (Gdiplus::REAL)(by + btnSz) - d, d, d, 0, 90);
                    path.AddArc((Gdiplus::REAL)bx, (Gdiplus::REAL)(by + btnSz) - d, d, d, 90, 90);
                    path.CloseFigure();
                    graphics.FillPath(&redBrush, &path);
                } else {
                    graphics.FillRectangle(&redBrush, (Gdiplus::REAL)bx, (Gdiplus::REAL)by,
                                           (Gdiplus::REAL)btnSz, (Gdiplus::REAL)btnSz);
                }
            }

            // Draw X with GDI+ for smooth diagonal lines only
            Gdiplus::Graphics graphics(hdc);
            graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            COLORREF xc = g_isCloseHovered ? RGB(255, 255, 255) : GetContourColor();
            Gdiplus::Pen xPen(Gdiplus::Color(255, GetRValue(xc), GetGValue(xc), GetBValue(xc)), 1.5f * g_dpiX / 96.0f);
            int p = DpiScale(7, g_dpiX);
            graphics.DrawLine(&xPen, bx + p, by + p, bx + btnSz - p, by + btnSz - p);
            graphics.DrawLine(&xPen, bx + btnSz - p, by + p, bx + p, by + btnSz - p);
        }

        int closeBtnReserve = DpiScale(24, g_dpiX) + padLeft;
        // Keep centered header content stable: reserve close-button space consistently.
        // In vertical mode, never reserve space - close button overlays without displacement.
        int btnReserve = 0;
        if (!HeaderIsVertical()) {
            btnReserve = ((g_settings.centerTaskContent) || i == g_hoverIndex)
                     ? closeBtnReserve
                     : 0;
        }
        int contentLeft = e.rcCell.left + padLeft;
        int contentRight = e.rcCell.right - padLeft - btnReserve;
        if (contentRight < contentLeft) contentRight = contentLeft;

        int iconX = contentLeft;
        int iconY = e.rcCell.top + padTop + (rowTitleH - iconSz) / 2;
        int textLeft = iconX + iconSz + padLeft;
        int textRight = contentRight;
        int textTop = e.rcCell.top + padTop;
        int textBottom = textTop + rowTitleH;

        if (HeaderIsVertical()) {
            int availableW = contentRight - contentLeft;
            if (availableW < 0) availableW = 0;

            iconX = contentLeft + fmax(0, (availableW - iconSz) / 2);
            iconY = e.rcCell.top + padTop;

            int headerGap = DpiScale(4, g_dpiY);
            int textH = GetHeaderTitleHeightPx();
            textTop = iconY + iconSz + headerGap;
            textBottom = textTop + textH;
            textLeft = contentLeft;
            textRight = contentRight;
        } else if (g_settings.centerTaskContent) {
            int availableW = contentRight - contentLeft;
            if (availableW < 0) availableW = 0;

            int gap = padLeft;
            int textMaxW = availableW - iconSz - gap;
            if (textMaxW < 0) textMaxW = 0;

            int textW = 0;
            if (textMaxW > 0 && e.title[0]) {
                RECT rcMeasure = { 0, 0, textMaxW, rowTitleH };
                DrawTextW(hdc, e.title, -1, &rcMeasure,
                          DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX | DT_CALCRECT);
                textW = rcMeasure.right - rcMeasure.left;
                if (textW < 0) textW = 0;
                if (textW > textMaxW) textW = textMaxW;
            }

            int blockW = iconSz + ((textW > 0) ? gap : 0) + textW;
            if (blockW < availableW) {
                iconX = contentLeft + (availableW - blockW) / 2;
            }

            textLeft = iconX + iconSz + ((textW > 0) ? gap : 0);
            textRight = textLeft + textW;
        }

        // Icon
        if (e.hIcon) DrawIconEx(hdc, iconX, iconY, e.hIcon, iconSz, iconSz, 0, NULL, DI_NORMAL);

        // Title text
        RECT rcText = { textLeft, textTop, textRight, textBottom };
        if (rcText.right < rcText.left) rcText.right = rcText.left;
        if (g_hTheme) {
            DTTOPTS opts = { sizeof(DTTOPTS) };
            opts.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR;
            opts.crText = g_isDarkMode ? SWS_TEXT_DARK : SWS_TEXT_LIGHT;
            DrawThemeTextEx(g_hTheme, hdc, 0, 0, e.title, -1,
                DT_SINGLELINE | (HeaderIsVertical() ? DT_CENTER : DT_VCENTER) | DT_END_ELLIPSIS | DT_NOPREFIX, &rcText, &opts);
        } else {
            SetTextColor(hdc, g_isDarkMode ? SWS_TEXT_DARK : SWS_TEXT_LIGHT);
            DrawTextW(hdc, e.title, -1, &rcText,
                      DT_SINGLELINE | (HeaderIsVertical() ? DT_CENTER : DT_VCENTER) | DT_END_ELLIPSIS | DT_NOPREFIX);
        }
    }
    SelectObject(hdc, hOldFont);
}


// Rendering

static void PaintSwitcher() {
    if (!g_hSwitcher || !g_isVisible) return;
    if (ThemeIs(L"none")) {
        // Layered window path: draw to off-screen DIB, UpdateLayeredWindow
        RECT rc; GetClientRect(g_hSwitcher, &rc);
        int w = rc.right, h = rc.bottom;
        if (w <= 0 || h <= 0) return;
        HDC hdcScreen = GetDC(g_hSwitcher);
        HDC hdcMem = CreateCompatibleDC(hdcScreen);
        BITMAPINFO bmi = {}; bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = w; bmi.bmiHeader.biHeight = -h;
        bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32; bmi.bmiHeader.biCompression = BI_RGB;
        void* bits = NULL;
        HBITMAP hBmp = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
        HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hBmp);
        DrawSwitcherContent(hdcMem, true);
        POINT ptSrc = {0,0}; SIZE sz = {w, h};
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(g_hSwitcher, hdcScreen, NULL, &sz, hdcMem, &ptSrc, 0, &bf, ULW_ALPHA);
        SelectObject(hdcMem, hOld); DeleteObject(hBmp); DeleteDC(hdcMem);
        ReleaseDC(g_hSwitcher, hdcScreen);
    } else {
        // Acrylic: trigger WM_PAINT via InvalidateRect
        InvalidateRect(g_hSwitcher, NULL, TRUE);
        UpdateWindow(g_hSwitcher);
    }
}

// Switcher Show / Hide / Switch

static void ResetDwmAttributes() {
    // Reset acrylic
    if (g_SetWindowCompositionAttribute) {
        ACCENT_POLICY a = {}; a.AccentState = 0;
        WINDOWCOMPOSITIONATTRIBDATA d = {19, &a, sizeof(a)};
        g_SetWindowCompositionAttribute(g_hSwitcher, &d);
    }
}

static void GetOffscreenDelayPosition(int* x, int* y) {
    // Put the 1x1 window just outside the virtual screen bounds.
    // This avoids alpha/layered hacks while keeping the window shown/foreground.
    int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);

    *x = vx - 2;
    *y = vy - 2;
}

static void CancelPendingShow() {
    if (g_hSwitcher) {
        KillTimer(g_hSwitcher, SWS_SHOW_DELAY_TIMER_ID);
    }

    g_isPendingShow = false;
}

static void ShowPendingOffscreenWindow() {
    int x, y;
    GetOffscreenDelayPosition(&x, &y);

    // Show as 1x1 off-screen. No transparency/style changes needed.
    SetWindowPos(g_hSwitcher, HWND_TOPMOST, x, y, 1, 1, SWP_NOACTIVATE);

    ShowWindow(g_hSwitcher, SW_SHOWNA);
    SetForegroundWindow(g_hSwitcher);
}

static void RevealPendingSwitcher() {
    if (!g_isPendingShow || !g_hSwitcher) {
        return;
    }

    KillTimer(g_hSwitcher, SWS_SHOW_DELAY_TIMER_ID);

    g_isPendingShow = false;
    g_isVisible = true;

    int x = g_pendingSwitcherRect.left;
    int y = g_pendingSwitcherRect.top;
    int w = g_pendingSwitcherRect.right - g_pendingSwitcherRect.left;
    int h = g_pendingSwitcherRect.bottom - g_pendingSwitcherRect.top;

    SetWindowPos(g_hSwitcher, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE);

    RegisterThumbnails();
    PaintSwitcher();
}

static void ShowSwitcher(bool sticky) {
    UnregisterThumbnails(); BuildWindowList();
    if (g_windows.empty()) return;
    g_isDarkMode = ShouldUseDarkMode(); g_isSticky = sticky;
    POINT pt; GetCursorPos(&pt);
    HMONITOR hMon = g_settings.primaryMonitorOnly ?
        MonitorFromPoint({0,0}, MONITOR_DEFAULTTOPRIMARY) :
        MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    g_hCurrentMonitor = hMon;
    if (g_settings.perMonitorWindows) {
        UnregisterThumbnails(); BuildWindowList();
        if (g_windows.empty()) return;
    }
    RegisterThumbnailsEarly();
    ComputeLayout(hMon);
    if (g_winW <= 0 || g_winH <= 0) return;

    // Recreate font for current DPI
    if (g_hFont) { DeleteObject(g_hFont); g_hFont = NULL; }
    g_hFont = CreateScaledFont(g_dpiY);

    MONITORINFO mi = { sizeof(mi) }; GetMonitorInfoW(hMon, &mi);
    int cx = (mi.rcWork.left + mi.rcWork.right - g_winW) / 2;
    int cy = (mi.rcWork.top + mi.rcWork.bottom - g_winH) / 2;

    // Always reset DWM attributes first to avoid leftovers
    ResetDwmAttributes();

    // Apply theme (EP: sws_WindowSwitcher.c lines 510-570)
    // Step 1: Reset DWM frame and blur state (EP lines 510-524)
    MARGINS marGlassInset = { 0, 0, 0, 0 };
    DwmExtendFrameIntoClientArea(g_hSwitcher, &marGlassInset);

    LONG_PTR exs = GetWindowLongPtrW(g_hSwitcher, GWL_EXSTYLE);
    if (ThemeIs(L"none")) {
        SetWindowLongPtrW(g_hSwitcher, GWL_EXSTYLE, exs | WS_EX_LAYERED);
    } else {
        SetWindowLongPtrW(g_hSwitcher, GWL_EXSTYLE, exs & ~WS_EX_LAYERED);
        BOOL dark = g_isDarkMode;
        DwmSetWindowAttribute(g_hSwitcher, 20, &dark, sizeof(dark));
        if (ThemeIs(L"backdrop") && g_SetWindowCompositionAttribute) {
            // EP: blur = (dwOpacity / 100.0) * 255
            DWORD blur = (DWORD)((g_settings.opacity / 100.0) * 255);
            COLORREF bg = g_isDarkMode ? SWS_BG_DARK : SWS_BG_LIGHT;
            // EP: nColor = (Opacity << 24) | (Color & 0xFFFFFF)
            // COLORREF is 0x00BBGGRR, so this works directly
            ACCENT_POLICY accent = {};
            accent.AccentState = 4; // ACCENT_ENABLE_ACRYLICBLURBEHIND
            accent.AccentFlags = 0;
            accent.GradientColor = (blur << 24) | (bg & 0x00FFFFFF);
            WINDOWCOMPOSITIONATTRIBDATA data = {19, &accent, sizeof(accent)};
            g_SetWindowCompositionAttribute(g_hSwitcher, &data);
        }
        // EP: Set black background brush for DWM compositing (line 566)
        SetClassLongPtrW(g_hSwitcher, GCLP_HBRBACKGROUND, (LONG_PTR)GetStockObject(BLACK_BRUSH));
    }

    INT cp = GetCornerPref();
    DwmSetWindowAttribute(g_hSwitcher, 33, &cp, sizeof(cp));

    g_pendingSwitcherRect = {
        cx,
        cy,
        cx + g_winW,
        cy + g_winH
    };

    g_layoutStartIndex = 0; // Always start from the first window on initial show
    g_selectedIndex = (g_windows.size() > 1) ? 1 : 0;
    g_hoverIndex = -1;
    g_isCloseHovered = false;

    CancelPendingShow();

    if (g_settings.showDelay > 0 && !sticky) {
        g_isPendingShow = true;
        g_isVisible = false;

        ShowPendingOffscreenWindow();

        SetTimer(g_hSwitcher, SWS_SHOW_DELAY_TIMER_ID, g_settings.showDelay, NULL);
        return;
    }

    g_isPendingShow = false;
    g_isVisible = true;

    SetWindowPos(g_hSwitcher, HWND_TOPMOST, cx, cy, g_winW, g_winH, SWP_NOACTIVATE);
    ShowWindow(g_hSwitcher, SW_SHOWNA);
    SetForegroundWindow(g_hSwitcher);

    RegisterThumbnails();
    PaintSwitcher();
}

static void HideSwitcher() {
    CancelPendingShow();

    UnregisterThumbnails();
    if (g_hSwitcher) {
        ShowWindow(g_hSwitcher, SW_HIDE);
    }

    g_isVisible = false;
    g_isPendingShow = false;
    g_isSticky = false;
}

static void SwitchToSelected() {
    if (g_selectedIndex < 0 || g_selectedIndex >= (int)g_windows.size()) { HideSwitcher(); return; }
    HWND hT = g_windows[g_selectedIndex].hWnd;
    HideSwitcher();
    if (IsWindow(hT)) {
        HWND hP = GetLastActivePopup(hT);
        HWND hF = IsWindowVisible(hP) ? hP : hT;
        if (IsIconic(hF)) ShowWindow(hF, SW_RESTORE);
        SwitchToThisWindow(hF, TRUE);
    }
}

// Helper: check if a window is truncated (not placed in current layout)
static bool IsWindowTruncated(int idx) {
    auto& w = g_windows[idx];
    return w.rcCell.left == 0 && w.rcCell.right == 0 &&
           w.rcCell.top == 0 && w.rcCell.bottom == 0;
}

// Helper: recompute layout and reposition switcher window
static void RecomputeAndReposition() {
    UnregisterThumbnails();
    RegisterThumbnailsEarly();
    HMONITOR hMon = MonitorFromWindow(g_hSwitcher, MONITOR_DEFAULTTONEAREST);
    ComputeLayout(hMon);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfoW(hMon, &mi);
    int cx = mi.rcWork.left + (mi.rcWork.right - mi.rcWork.left - g_winW) / 2;
    int cy = mi.rcWork.top + (mi.rcWork.bottom - mi.rcWork.top - g_winH) / 2;

    g_pendingSwitcherRect = {
        cx,
        cy,
        cx + g_winW,
        cy + g_winH
    };

    if (g_isPendingShow) {
        int ox, oy;
        GetOffscreenDelayPosition(&ox, &oy);

        SetWindowPos(g_hSwitcher, HWND_TOPMOST, ox, oy, 1, 1, SWP_NOACTIVATE);
        return;
    }

    SetWindowPos(g_hSwitcher, HWND_TOPMOST, cx, cy, g_winW, g_winH, SWP_NOACTIVATE);
    RegisterThumbnails();
}

// Linear navigation: Tab, Shift+Tab, Left, Right, Hotkeys, Scroll
static void CycleLinear(int delta) {
    if (g_windows.empty()) return;
    int n = (int)g_windows.size();
    g_selectedIndex = ((g_selectedIndex + delta) % n + n) % n;

    // If the newly selected window is truncated, recompute layout
    if (IsWindowTruncated(g_selectedIndex)) {
        // Always try resetting to index 0 first (top-first view)
        g_layoutStartIndex = 0;
        RecomputeAndReposition();

        // If still truncated, scroll forward line-by-line until visible
        // (row in horizontal mode, column in vertical mode).
        int n2 = n;
        while (IsWindowTruncated(g_selectedIndex) && n2-- > 0) {
            int firstIdx = g_layoutStartIndex % n;
            int firstLineCoord = LayoutIsVertical() ? g_windows[firstIdx].rcCell.left : g_windows[firstIdx].rcCell.top;
            int newStart = g_layoutStartIndex;
            for (int k = 0; k < n; k++) {
                int wi = (g_layoutStartIndex + k) % n;
                if (IsWindowTruncated(wi)) break;
                int lineCoord = LayoutIsVertical() ? g_windows[wi].rcCell.left : g_windows[wi].rcCell.top;
                if (lineCoord != firstLineCoord) {
                    newStart = wi;
                    break;
                }
            }
            if (newStart == g_layoutStartIndex) {
                g_layoutStartIndex = g_selectedIndex;
            } else {
                g_layoutStartIndex = newStart;
            }
            RecomputeAndReposition();
        }
    }

    PaintSwitcher();
}

// Directional navigation: Up, Down (EP-style row-based with nearest-column match)
// Walks in layout placement order (from g_layoutStartIndex, wrapping) instead of raw list index.
static void CycleDirectional(int vertDelta) {
    if (g_windows.empty()) return;
    int n = (int)g_windows.size();
    bool verticalLayout = LayoutIsVertical();

    // Build layout-order mapping: layoutOrder[0] is the first window placed visually
    auto buildLayoutOrder = [&](std::vector<int>& order) {
        order.resize(n);
        for (int idx = 0; idx < n; idx++)
            order[idx] = (g_layoutStartIndex + idx) % n;
    };

    std::vector<int> layoutOrder;
    buildLayoutOrder(layoutOrder);

    // Find current selection's position in layout order
    int layoutPos = 0;
    for (int idx = 0; idx < n; idx++) {
        if (layoutOrder[idx] == g_selectedIndex) { layoutPos = idx; break; }
    }

    // Save current selection's line anchor and perpendicular center.
    RECT rcPrev = g_windows[g_selectedIndex].rcCell;
    int prevLineCoord = verticalLayout ? rcPrev.left : rcPrev.top;
    int prevPerpCenter = verticalLayout ? (rcPrev.top + rcPrev.bottom) / 2 : (rcPrev.left + rcPrev.right) / 2;

    // Walk direction in layout order: DOWN = +1 (visually next), UP = -1 (visually prev)
    int layoutDelta = vertDelta;
    int current = -1;
    bool foundDifferentRow = false;

    for (int step = 0; step < n; step++) {
        int nextPos = ((layoutPos + (step + 1) * layoutDelta) % n + n) % n;
        int windowIdx = layoutOrder[nextPos];

        if (nextPos == layoutPos) break; // Wrapped all the way around

        // Target window is off-screen — scroll layout to reveal it.
        if (IsWindowTruncated(windowIdx)) {
            // First try reset to 0 (handles wrap-to-top / DOWN from last row)
            g_layoutStartIndex = 0;
            RecomputeAndReposition();

            // If still truncated, scroll forward line-by-line.
            int attempts = n;
            while (IsWindowTruncated(windowIdx) && attempts-- > 0) {
                int firstIdx = g_layoutStartIndex % n;
                int firstLineCoord2 = verticalLayout ? g_windows[firstIdx].rcCell.left : g_windows[firstIdx].rcCell.top;
                int newStart = g_layoutStartIndex;
                for (int k = 0; k < n; k++) {
                    int wi = (g_layoutStartIndex + k) % n;
                    if (IsWindowTruncated(wi)) break;
                    int lineCoord = verticalLayout ? g_windows[wi].rcCell.left : g_windows[wi].rcCell.top;
                    if (lineCoord != firstLineCoord2) {
                        newStart = wi;
                        break;
                    }
                }
                if (newStart == g_layoutStartIndex) {
                    g_layoutStartIndex = windowIdx;
                } else {
                    g_layoutStartIndex = newStart;
                }
                RecomputeAndReposition();
            }

            // Rebuild layout order after recompute
            buildLayoutOrder(layoutOrder);
            current = windowIdx;
            foundDifferentRow = true;
            break;
        }

        int lineCoord = verticalLayout ? g_windows[windowIdx].rcCell.left : g_windows[windowIdx].rcCell.top;
        if (lineCoord != prevLineCoord) {
            current = windowIdx;
            foundDifferentRow = true;
            break;
        }
    }

    if (!foundDifferentRow) {
        // Only one line visible; nothing to jump to.
        return;
    }

    // Find current's position in layout order for row scanning
    int currentLayoutPos = 0;
    for (int idx = 0; idx < n; idx++) {
        if (layoutOrder[idx] == current) { currentLayoutPos = idx; break; }
    }

    // Found a window on a different line. Find nearest position match
    // on that line (x-match for horizontal mode, y-match for vertical mode).
    int targetLineCoord = verticalLayout ? g_windows[current].rcCell.left : g_windows[current].rcCell.top;
    int bestIndex = current;
    int bestDist = INT_MAX;

    // Scan forward in layout order from current to find all windows on the target line.
    for (int idx = currentLayoutPos; idx < n; idx++) {
        int wi = layoutOrder[idx];
        if (IsWindowTruncated(wi)) break;
        int lineCoord = verticalLayout ? g_windows[wi].rcCell.left : g_windows[wi].rcCell.top;
        if (lineCoord != targetLineCoord) break;

        int perpCenter = verticalLayout ?
            (g_windows[wi].rcCell.top + g_windows[wi].rcCell.bottom) / 2 :
            (g_windows[wi].rcCell.left + g_windows[wi].rcCell.right) / 2;
        int dist = abs(prevPerpCenter - perpCenter);
        if (dist < bestDist) {
            bestDist = dist;
            bestIndex = wi;
        }
    }

    // Scan backward in layout order from current to cover the full line.
    for (int idx = currentLayoutPos - 1; idx >= 0; idx--) {
        int wi = layoutOrder[idx];
        if (IsWindowTruncated(wi)) break;
        int lineCoord = verticalLayout ? g_windows[wi].rcCell.left : g_windows[wi].rcCell.top;
        if (lineCoord != targetLineCoord) break;

        int perpCenter = verticalLayout ?
            (g_windows[wi].rcCell.top + g_windows[wi].rcCell.bottom) / 2 :
            (g_windows[wi].rcCell.left + g_windows[wi].rcCell.right) / 2;
        int dist = abs(prevPerpCenter - perpCenter);
        if (dist < bestDist) {
            bestDist = dist;
            bestIndex = wi;
        }
    }

    g_selectedIndex = bestIndex;
    PaintSwitcher();
}

static int HitTest(int x, int y) {
    for (int i = 0; i < (int)g_windows.size(); i++) {
        RECT r = g_windows[i].rcCell;
        if (x >= r.left && x < r.right && y >= r.top && y < r.bottom) return i;
    }
    return -1;
}
static int HitTestThumb(int x, int y) {
    if (!g_settings.showThumbnails) return -1;
    for (int i = 0; i < (int)g_windows.size(); i++) {
        RECT r = g_windows[i].rcThumb;
        if (x >= r.left && x < r.right && y >= r.top && y < r.bottom) return i;
    }
    return -1;
}


// WndProc

static void SWS_RegisterHotkeys();

static LRESULT CALLBACK SwitcherWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_TIMER) {
        if (wParam == SWS_HOTKEY_RETRY_TIMER_ID) {
            SWS_RegisterHotkeys();
            return 0;
        }

        if (wParam == SWS_SHOW_DELAY_TIMER_ID) {
            RevealPendingSwitcher();
            return 0;
        }
    }
    if (uMsg == WM_HOTKEY) {
        int id = (int)wParam;
        bool isBackward = false;
        bool isCtrl = false;

        switch (id) {
        case SWS_HOTKEY_ALTTAB:
            break;
        case SWS_HOTKEY_ALTSHIFTTAB:
            if (!UseAltShiftTabBackward()) return 0;
            isBackward = true;
            break;
        case SWS_HOTKEY_ALTCTRLTAB:
            isCtrl = true;
            break;
        case SWS_HOTKEY_ALTSHIFTCTRLTAB:
            if (!UseAltShiftTabBackward()) return 0;
            isBackward = true;
            isCtrl = true;
            break;
        case SWS_HOTKEY_ALTBACKTICK:
            if (!UseAltBacktickBackward()) return 0;
            isBackward = true;
            break;
        default:
            return 0;
        }

        if (!g_isVisible && !g_isPendingShow) {
            ShowSwitcher(isCtrl);

            if (isBackward && g_windows.size() > 1) {
                g_selectedIndex = (int)g_windows.size() - 1;

                if (g_isVisible) {
                    PaintSwitcher();
                }
            }
        } else {
            if (g_isPendingShow && isCtrl) {
                g_isSticky = true;
            }

            CycleLinear(isBackward ? -1 : 1);

            if (g_isPendingShow) {
                RevealPendingSwitcher();
            }
        }
        return 0;
    }

    // WM_PAINT for Acrylic (non-layered) path
    if (uMsg == WM_PAINT && !ThemeIs(L"none") && g_isVisible) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc; GetClientRect(hWnd, &rc);
        BP_PAINTPARAMS params = { sizeof(params) };
        params.dwFlags = BPPF_ERASE;
        HDC hdcBuf = NULL;
        HPAINTBUFFER hBP = BeginBufferedPaint(hdc, &rc, BPBF_TOPDOWNDIB, &params, &hdcBuf);
        if (hBP) {
            DrawSwitcherContent(hdcBuf, false);
            // Do NOT call BufferedPaintSetAlpha here — it would force all pixels
            // to opaque (alpha=255), blocking the acrylic blur from showing through.
            // BPPF_ERASE already cleared the buffer to RGBA(0,0,0,0) = transparent.
            // Our contour/icon/text drawing sets correct per-pixel alpha.
            EndBufferedPaint(hBP, TRUE);
        }
        EndPaint(hWnd, &ps);
        return 0;
    }

    switch (uMsg) {
    case WM_KEYUP:
        if (g_isVisible && UseAltShiftBackward() && wParam == VK_TAB) {
            bool altDown = (GetKeyState(VK_MENU) & 0x8000) != 0;
            bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
            if (altDown && shiftDown) {
                return 0;
            }
        }

        if (wParam == VK_MENU && (g_isVisible || g_isPendingShow) && !g_isSticky) {
            SwitchToSelected();
            return 0;
        }
        if (wParam == VK_ESCAPE && g_isVisible) { HideSwitcher(); return 0; }
        if (wParam == VK_RETURN && g_isVisible) { SwitchToSelected(); return 0; }
        break;
    case WM_SYSKEYUP:
        if (g_isVisible && UseAltShiftBackward() && wParam == VK_TAB) {
            bool altDown = (GetKeyState(VK_MENU) & 0x8000) != 0;
            bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
            if (altDown && shiftDown) {
                return 0;
            }
        }

        if (wParam == VK_MENU && (g_isVisible || g_isPendingShow) && !g_isSticky) {
            SwitchToSelected();
            return 0;
        }
        break;
    case WM_SYSKEYDOWN: case WM_KEYDOWN:
        if (g_isVisible) {
            // Block Alt+Shift+Tab from reaching the system if setting is enabled
            if (UseAltShiftBackward() && wParam == VK_TAB) {
                bool altDown = (GetKeyState(VK_MENU) & 0x8000) != 0;
                bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                if (altDown && shiftDown) {
                    // Suppress native switcher
                    return 0;
                }
            }
            if (UseAltShiftBackward() &&
                (wParam == VK_SHIFT || wParam == VK_LSHIFT || wParam == VK_RSHIFT)) {
                bool isRepeat = (lParam & 0x40000000) != 0;
                bool altDown = (GetKeyState(VK_MENU) & 0x8000) != 0;
                if (!isRepeat && altDown) {
                    CycleLinear(-1);
                    return 0;
                }
            }

            if (wParam == VK_TAB) {
                bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                bool backward = UseAltShiftTabBackward() && shiftDown;
                CycleLinear(backward ? -1 : 1);
                return 0;
            }
            if (!LayoutIsVertical()) {
                if (wParam == VK_LEFT) { CycleLinear(-1); return 0; }
                if (wParam == VK_RIGHT) { CycleLinear(1); return 0; }
                if (wParam == VK_UP) { CycleDirectional(-1); return 0; }
                if (wParam == VK_DOWN) { CycleDirectional(1); return 0; }
            } else {
                if (wParam == VK_UP) { CycleLinear(-1); return 0; }
                if (wParam == VK_DOWN) { CycleLinear(1); return 0; }
                if (wParam == VK_LEFT) { CycleDirectional(-1); return 0; }
                if (wParam == VK_RIGHT) { CycleDirectional(1); return 0; }
            }
            if (wParam == VK_ESCAPE) { HideSwitcher(); return 0; }
            if (wParam == VK_RETURN || wParam == VK_SPACE) { SwitchToSelected(); return 0; }
        }
        break;
    // (Removed duplicate combined case for WM_SYSKEYUP and WM_KEYUP)
    case WM_MOUSEWHEEL:
        if (g_isVisible) {
            bool ok = ScrollIs(L"always") || (ScrollIs(L"stickyOnly") && g_isSticky);
            if (ok) { CycleLinear(GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? -1 : 1); return 0; }
        }
        break;
    case WM_MOUSEMOVE: {
        int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
        int idx = g_settings.showThumbnails ? HitTestThumb(x, y) : HitTest(x, y);
        if (idx < 0) idx = HitTest(x, y);
        
        bool closeHovered = false;
        if (idx >= 0) {
            auto& e = g_windows[idx];
            int padL = DpiScale(SWS_PAD_LEFT, g_dpiX);
            int padT = DpiScale(SWS_PAD_TOP, g_dpiY);
            int titleH = GetHeaderRowHeightPx();
            int btnSz = DpiScale(24, g_dpiX);
            int bx = e.rcCell.right - padL - btnSz;
            int by = HeaderIsVertical() ? (e.rcCell.top + padT)
                                        : (e.rcCell.top + padT + (titleH - btnSz) / 2);
            if (x >= bx && x <= bx + btnSz && y >= by && y <= by + btnSz) {
                closeHovered = true;
            }
        }
        
        if (idx != g_hoverIndex || closeHovered != g_isCloseHovered) {
            g_hoverIndex = idx;
            g_isCloseHovered = closeHovered;
            PaintSwitcher();
        }
        return 0;
    }
    case WM_LBUTTONUP: {
        int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
        int idx = HitTest(x, y);
        if (idx >= 0) {
            if (g_isCloseHovered && idx == g_hoverIndex) {
                PostMessage(g_windows[idx].hWnd, WM_CLOSE, 0, 0);
                g_windows.erase(g_windows.begin() + idx);
                if (g_windows.empty()) { HideSwitcher(); }
                else {
                    if (g_selectedIndex >= (int)g_windows.size()) g_selectedIndex = (int)g_windows.size() - 1;
                    UnregisterThumbnails();
                    RegisterThumbnailsEarly();
                    ComputeLayout(g_hCurrentMonitor);
                    // Resize and re-center the window to match new layout
                    MONITORINFO rmi = { sizeof(rmi) }; GetMonitorInfoW(g_hCurrentMonitor, &rmi);
                    int cx = (rmi.rcWork.left + rmi.rcWork.right - g_winW) / 2;
                    int cy = (rmi.rcWork.top + rmi.rcWork.bottom - g_winH) / 2;
                    SetWindowPos(g_hSwitcher, HWND_TOPMOST, cx, cy, g_winW, g_winH, SWP_NOACTIVATE);
                    RegisterThumbnails();
                    g_hoverIndex = -1;
                    g_isCloseHovered = false;
                    PaintSwitcher();
                }
            } else {
                g_selectedIndex = idx; 
                SwitchToSelected();
            }
        }
        return 0;
    }
    case WM_ACTIVATE:
        if (wParam == WA_INACTIVE && g_isVisible) { HideSwitcher(); return 0; }
        break;
    case WM_KILLFOCUS:
        if (g_isVisible) { HideSwitcher(); return 0; }
        break;
    case WM_ERASEBKGND: return 1;
    case WM_DESTROY: UnregisterThumbnails(); return 0;
    }

    if (g_shellHookMsg && uMsg == g_shellHookMsg && g_isVisible) {
        int code = (int)(wParam & 0x7FFF);
        if (code == HSHELL_WINDOWDESTROYED) {
            HWND hS = (HWND)lParam;
            for (int i = 0; i < (int)g_windows.size(); i++)
                if (g_windows[i].hWnd == hS) { ShowSwitcher(g_isSticky); break; }
        }
        return 0;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

// Hotkey Helpers

static void SWS_RegisterHotkeys() {
    if (g_hotkeysRegistered || !g_hSwitcher) return;
    BOOL r1 = RegisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTTAB, MOD_ALT, VK_TAB);
    BOOL r2 = RegisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTSHIFTTAB, MOD_ALT | MOD_SHIFT, VK_TAB);
    BOOL r3 = RegisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTCTRLTAB, MOD_ALT | MOD_CONTROL, VK_TAB);
    BOOL r4 = RegisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTSHIFTCTRLTAB, MOD_ALT | MOD_SHIFT | MOD_CONTROL, VK_TAB);
    BOOL r5 = RegisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTBACKTICK, MOD_ALT, VK_OEM_3);
    if (r1 && r2 && r3 && r4 && r5) {
        g_hotkeysRegistered = true;
        KillTimer(g_hSwitcher, SWS_HOTKEY_RETRY_TIMER_ID);
        Wh_Log(L"All hotkeys registered successfully");
    } else {
        if (r1) UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTTAB);
        if (r2) UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTSHIFTTAB);
        if (r3) UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTCTRLTAB);
        if (r4) UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTSHIFTCTRLTAB);
        if (r5) UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTBACKTICK);
        SetTimer(g_hSwitcher, SWS_HOTKEY_RETRY_TIMER_ID, SWS_HOTKEY_RETRY_INTERVAL, NULL);
        Wh_Log(L"Hotkey registration incomplete, retrying in %dms", SWS_HOTKEY_RETRY_INTERVAL);
    }
}
static void SWS_UnregisterHotkeys() {
    KillTimer(g_hSwitcher, SWS_HOTKEY_RETRY_TIMER_ID);
    if (!g_hotkeysRegistered || !g_hSwitcher) return;
    UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTTAB);
    UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTSHIFTTAB);
    UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTCTRLTAB);
    UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTSHIFTCTRLTAB);
    UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTBACKTICK);
    g_hotkeysRegistered = false;
    Wh_Log(L"Hotkeys unregistered");
}


// Settings

static void LoadSettings() {
    LPCWSTR v;
    v = Wh_GetStringSetting(L"theme");
    wcscpy_s(g_settings.theme, v ? v : L"none"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"colorScheme");
    wcscpy_s(g_settings.colorScheme, v ? v : L"system"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"cornerPreference");
    wcscpy_s(g_settings.cornerPreference, v ? v : L"round"); Wh_FreeStringSetting(v);
    g_settings.taskRoundedCorners = Wh_GetIntSetting(L"taskRoundedCorners");
    v = Wh_GetStringSetting(L"scrollWheelBehavior");
    wcscpy_s(g_settings.scrollWheelBehavior, v ? v : L"never"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"taskListOrientation");
    wcscpy_s(g_settings.taskListOrientation, v ? v : L"horizontal"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"headerContentOrientation");
    wcscpy_s(g_settings.headerContentOrientation, v ? v : L"horizontal"); Wh_FreeStringSetting(v);
    if (wcscmp(g_settings.headerContentOrientation, L"horizontal") != 0 &&
        wcscmp(g_settings.headerContentOrientation, L"vertical") != 0) {
        wcscpy_s(g_settings.headerContentOrientation, L"horizontal");
    }
    v = Wh_GetStringSetting(L"iconSize");
    wcscpy_s(g_settings.iconSize, v ? v : L"small"); Wh_FreeStringSetting(v);
    if (wcscmp(g_settings.iconSize, L"small") != 0 &&
        wcscmp(g_settings.iconSize, L"large") != 0) {
        wcscpy_s(g_settings.iconSize, L"small");
    }
    v = Wh_GetStringSetting(L"backwardShortcut");
    if (v) {
        wcscpy_s(g_settings.backwardShortcut, v);
        Wh_FreeStringSetting(v);
    } else {
        // Backward compatibility with previous boolean setting.
        wcscpy_s(g_settings.backwardShortcut,
                 Wh_GetIntSetting(L"enableAltShiftForBackward") ? L"altShift" : L"altShiftTab");
    }
    if (wcscmp(g_settings.backwardShortcut, L"altShiftTab") != 0 &&
        wcscmp(g_settings.backwardShortcut, L"altShift") != 0 &&
        wcscmp(g_settings.backwardShortcut, L"altBacktick") != 0) {
        wcscpy_s(g_settings.backwardShortcut, L"altShiftTab");
    }

    g_settings.opacity = Wh_GetIntSetting(L"opacity");
    if (g_settings.opacity <= 0 || g_settings.opacity > 100) g_settings.opacity = 90;
    g_settings.rowHeight = Wh_GetIntSetting(L"rowHeight");
    if (g_settings.rowHeight <= 0) g_settings.rowHeight = 230;
    g_settings.rowWidth = Wh_GetIntSetting(L"rowWidth");
    if (g_settings.rowWidth < 0) g_settings.rowWidth = 0;
    g_settings.showThumbnails = Wh_GetIntSetting(L"showThumbnails");
    g_settings.maxWidthPercent = Wh_GetIntSetting(L"maxWidthPercent");
    if (g_settings.maxWidthPercent <= 0 || g_settings.maxWidthPercent > 100) g_settings.maxWidthPercent = 80;
    g_settings.maxHeightPercent = Wh_GetIntSetting(L"maxHeightPercent");
    if (g_settings.maxHeightPercent <= 0 || g_settings.maxHeightPercent > 100) g_settings.maxHeightPercent = 80;

    g_settings.showDelay = Wh_GetIntSetting(L"showDelay");
    if (g_settings.showDelay < 0) g_settings.showDelay = 0;
    g_settings.useAccentColor = Wh_GetIntSetting(L"useAccentColor");
    g_settings.primaryMonitorOnly = Wh_GetIntSetting(L"primaryMonitorOnly");
    g_settings.perMonitorWindows = Wh_GetIntSetting(L"perMonitorWindows");
    g_settings.centerTaskContent = Wh_GetIntSetting(L"centerTaskContent");


    v = Wh_GetStringSetting(L"borderColorDark");
    wcscpy_s(g_settings.borderColorDark, v ? v : L"#FFFFFF"); Wh_FreeStringSetting(v);
    if (!ParseHexColor(g_settings.borderColorDark, nullptr)) {
        wcscpy_s(g_settings.borderColorDark, L"#FFFFFF");
    }

    v = Wh_GetStringSetting(L"borderColorLight");
    wcscpy_s(g_settings.borderColorLight, v ? v : L"#000000"); Wh_FreeStringSetting(v);
    if (!ParseHexColor(g_settings.borderColorLight, nullptr)) {
        wcscpy_s(g_settings.borderColorLight, L"#000000");
    }

}


// RegisterHotKey hook for explorer.exe

static bool SWS_IsAltTabHotkey(UINT fsModifiers, UINT vk) {
    UINT baseMods = fsModifiers & ~MOD_NOREPEAT;
    if (vk == VK_TAB && (baseMods & MOD_ALT)) return true;
    return false;
}

typedef BOOL(WINAPI *RegisterHotKey_t)(HWND hWnd, int id, UINT fsModifiers, UINT vk);
static RegisterHotKey_t RegisterHotKey_Original;

static BOOL WINAPI RegisterHotKey_Hook(HWND hWnd, int id, UINT fsModifiers, UINT vk) {
    if (SWS_IsAltTabHotkey(fsModifiers, vk)) {
        Wh_Log(L"Blocked explorer RegisterHotKey for Alt+Tab variant (vk=0x%X, mod=0x%X)", vk, fsModifiers);
        SetLastError(0);
        return TRUE;
    }
    return RegisterHotKey_Original(hWnd, id, fsModifiers, vk);
}

// Background thread for tool mod process

static DWORD WINAPI SwitcherThread(LPVOID lpParam) {
    Wh_Log(L"SwitcherThread starting");
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    ResolveAPIs();
    LoadSettings();
    g_isDarkMode = ShouldUseDarkMode();

    BufferedPaintInit();

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASSEXW wc = { sizeof(wc) };
    wc.lpfnWndProc = SwitcherWndProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = SWS_CLASSNAME;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.style = CS_DBLCLKS;
    RegisterClassExW(&wc);

    DWORD exStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED;
    g_hSwitcher = CreateWindowExW(exStyle, SWS_CLASSNAME, L"",
        WS_POPUP, 0, 0, 0, 0, NULL, NULL, GetModuleHandleW(NULL), NULL);
    if (!g_hSwitcher) { Wh_Log(L"Failed to create switcher window"); return 1; }

    BOOL bExclude = TRUE;
    DwmSetWindowAttribute(g_hSwitcher, DWMWA_EXCLUDED_FROM_PEEK, &bExclude, sizeof(bExclude));

    g_hTheme = OpenThemeData(NULL, L"CompositedWindow::Window");
    g_shellHookMsg = RegisterWindowMessageW(L"SHELLHOOK");
    RegisterShellHookWindow(g_hSwitcher);

    g_hFont = CreateScaledFont(96);

    SWS_RegisterHotkeys();

    Wh_Log(L"Simple Window Switcher initialized, entering message loop");

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    SWS_UnregisterHotkeys();
    if (g_isVisible) HideSwitcher();
    UnregisterThumbnails();
    g_windows.clear();
    if (g_hSwitcher) { DeregisterShellHookWindow(g_hSwitcher); DestroyWindow(g_hSwitcher); g_hSwitcher = NULL; }
    UnregisterClassW(SWS_CLASSNAME, GetModuleHandleW(NULL));
    if (g_hFont) { DeleteObject(g_hFont); g_hFont = NULL; }
    if (g_hTheme) { CloseThemeData(g_hTheme); g_hTheme = NULL; }
    BufferedPaintUnInit();
    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }

    CoUninitialize();
    Wh_Log(L"SwitcherThread exiting");
    return 0;
}

// Tool Mod callbacks

bool WhTool_ModInit() {
    Wh_Log(L"Simple Window Switcher: WhTool_ModInit");
    g_hSwitcherThread = CreateThread(NULL, 0, SwitcherThread, NULL, 0, &g_dwSwitcherThreadId);
    return g_hSwitcherThread != NULL;
}

void WhTool_ModUninit() {
    Wh_Log(L"Simple Window Switcher: WhTool_ModUninit");
    if (g_dwSwitcherThreadId)
        PostThreadMessage(g_dwSwitcherThreadId, WM_QUIT, 0, 0);
    if (g_hSwitcherThread) {
        WaitForSingleObject(g_hSwitcherThread, INFINITE);
        CloseHandle(g_hSwitcherThread);
        g_hSwitcherThread = NULL;
        g_dwSwitcherThreadId = 0;
    }
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Simple Window Switcher: WhTool_ModSettingsChanged");
    if (g_hSwitcher) {
        LoadSettings();
        if (g_isVisible) {
            ShowSwitcher(g_isSticky);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod boilerplate
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk lifecycle

static bool IsMainExplorer() {
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hTaskbar) {
        DWORD trayPid = 0;
        GetWindowThreadProcessId(hTaskbar, &trayPid);
        if (trayPid != GetCurrentProcessId()) {
            return false;
        }
    }
    return true;
}

static bool IsExplorerUptimeLarge() {
    FILETIME creationTime, exitTime, kernelTime, userTime;
    if (GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime)) {
        ULARGE_INTEGER creation;
        creation.LowPart = creationTime.dwLowDateTime;
        creation.HighPart = creationTime.dwHighDateTime;
        
        FILETIME systemTime;
        GetSystemTimeAsFileTime(&systemTime);
        ULARGE_INTEGER current;
        current.LowPart = systemTime.dwLowDateTime;
        current.HighPart = systemTime.dwHighDateTime;
        
        // 30 seconds = 300,000,000 intervals of 100ns
        if (current.QuadPart > creation.QuadPart && (current.QuadPart - creation.QuadPart) > 300000000ULL) {
            return true;
        }
    }
    return false;
}

BOOL Wh_ModInit() {
    WCHAR exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    WCHAR* exeName = wcsrchr(exePath, L'\\');
    exeName = exeName ? exeName + 1 : exePath;
    Wh_Log(L"SWS: Wh_ModInit in process: %s", exeName);

    // --- explorer.exe path: hook RegisterHotKey only ---
    if (_wcsicmp(exeName, L"explorer.exe") == 0) {
        g_isExplorer = true;
        Wh_Log(L"SWS: Loaded into explorer.exe, hooking RegisterHotKey");

        if (!g_WM_SWS_GET_UWP_ICON) {
            g_WM_SWS_GET_UWP_ICON = RegisterWindowMessageW(L"Windhawk_SWS_GetUwpIcon");
        }
        g_explorerIpcThread = CreateThread(NULL, 0, ExplorerIpcThread, NULL, 0, NULL);

        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        if (hUser32) {
            void* pRegisterHotKey = (void*)GetProcAddress(hUser32, "RegisterHotKey");
            if (pRegisterHotKey) {
                Wh_SetFunctionHook(pRegisterHotKey, (void*)RegisterHotKey_Hook, (void**)&RegisterHotKey_Original);
            }
        }

        // Check if Explorer has already registered standard hotkeys.
        // We use Alt+Tab as a probe. If it fails, Explorer is mid-session and already owns it.
        // We only do this for the main Explorer process, and only if it's been running for a while (>30s),
        // to avoid false prompts on system startup or when secondary Explorers are launched.
        if (!GetSystemMetrics(SM_SHUTTINGDOWN) && IsMainExplorer() && IsExplorerUptimeLarge()) {
            Wh_Log(L"SWS: Checking if Explorer is mid-session -> probing Alt+Tab");
            if (!RegisterHotKey(NULL, 0x1337, MOD_ALT, VK_TAB)) {
                Wh_Log(L"SWS: Alt+Tab failed -> Explorer is mid-session, prompting");
                PromptForExplorerRestart();
            } else {
                Wh_Log(L"SWS: Alt+Tab succeeded -> Explorer hasn't registered it, skipping prompt");
                UnregisterHotKey(NULL, 0x1337);
            }
        } else {
            Wh_Log(L"SWS: System shutting down or early startup/secondary explorer, skipping prompt");
        }

        return TRUE;
    }

    // --- windhawk.exe path: tool mod boilerplate ---
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isExplorer) {
        return;
    }

    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isExplorer) {
        HWND promptWnd = g_restartExplorerPromptWindow;
        if (promptWnd) PostMessage(promptWnd, WM_CLOSE, 0, 0);

        HWND hIpc = FindWindowW(L"WindhawkSWS_IpcWindow", NULL);
        if (hIpc) {
            PostMessageW(hIpc, WM_QUIT, 0, 0);
        }
        if (g_explorerIpcThread) {
            WaitForSingleObject(g_explorerIpcThread, 5000);
            CloseHandle(g_explorerIpcThread);
            g_explorerIpcThread = NULL;
        }
        for (auto& pair : g_uwpIconCache) {
            if (pair.second) DestroyIcon(pair.second);
        }
        g_uwpIconCache.clear();

        if (g_isExplorer && IsMainExplorer()) {
            if (!GetSystemMetrics(SM_SHUTTINGDOWN)) {
                PromptForExplorerRestart();
            }
        }

        if (g_restartExplorerPromptThread) {
            WaitForSingleObject(g_restartExplorerPromptThread, INFINITE);
            CloseHandle(g_restartExplorerPromptThread);
            g_restartExplorerPromptThread = NULL;
        }
        return;
    }

    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
