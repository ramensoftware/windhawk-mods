// ==WindhawkMod==
// @id              translucent-windows
// @name            Translucent Windows
// @description     Enables native translucent effects in Windows 11
// @version         1.8.0
// @author          Undisputed00x
// @github          https://github.com/Undisputed00x
// @include         *
// @compilerOptions -ldwmapi -luxtheme -lcomctl32 -lgdi32 -ld2d1 -lmsimg32 -lshcore -lversion -ffp-exception-behavior=maytrap
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

### ⚠️ FAQ section below ⚠️
### ❗For any excluded process, if the global "New system colors" setting is enabled, please add the excluded processes to the process rules in the mod settings instead.❗

- ## Theme Customization
| **Default cleartype text** | **Greyscale text** |
|:---:|:---:|
| ![Cleartype](https://i.imgur.com/utajxyq.png) | ![Greyscale](https://i.imgur.com/0OelxZH.png) |

| **Default text** | **Alpha blended text** |
|:---:|:---:|
| ![Default](https://i.imgur.com/ZgrJMgP.png) | ![Composited](https://i.imgur.com/4lQU2a4.png) |

| **Default themed controls** | **Custom themed controls** |
|:---:|:---:|
| ![Default Theme](https://i.imgur.com/8hYI1DZ.png) | ![Custom Theme](https://i.imgur.com/vWbelew.png) |

- ## Translucent effects
| **Blur (AccentBlurBehind)** | **Acrylic (SystemBackdrop)** |
|:---:|:---:|
| ![AccentBlurBehind](https://i.imgur.com/tSf5ztk.png) | ![Acrylic SystemBackdrop](https://i.imgur.com/YNktLTu.png) |

| **Mica (SystemBackdrop)** | **MicaAlt (SystemBackdrop)** |
|:---:|:---:|
| ![Mica](https://i.imgur.com/1ciJJck.png) | ![MicaTabbed](https://i.imgur.com/5Dxj5PS.png) |

## Credits 
The custom theme is a close copy of the Rectify 11 theme created by [WinExperiments](https://github.com/WinExperiments).
#
The inspiration for this mod came from the awesome Windows effects customization projects of [Maplespe](https://github.com/Maplespe) and [ALTaleX531](https://github.com/ALTaleX531).
#
Thanks also to [m417z](https://github.com/m417z) for his help and input in completing the mod.

## FAQ

* ⚠️Use Windows 11 File Explorer Styler mod and select the Translucent Explorer11 theme
in order to get translucent WinUI parts of the new file explorer.⚠️

* ❗The new system colors setting option adjusts the [system colors](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getsyscolor) 
in order to blend with the background translucency / custom theme rendering. 
This setting option overrides any process exclusion as these colors are applied system-wide using 
the [SetSysColor](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setsyscolors) API.
Intercepting and changing the system colors in a proper way is quite difficult, more details
in another software project that faced the same problem: https://github.com/namazso/SecureUxTheme/issues/9#issuecomment-611897882 ❗

* ⚠️Set a process rule in the mod's settings with custom theme rendering disabled, in order to reset (if possible) the custom system colors to default for the target process.⚠️

* ❗The Windows custom theme rendering also fixes invisible text by restoring alpha and modifying text colors.
Extending effects to the entire window can result in text being barely readable or even invisible in some cases. 
Enabling HDR, 10bit color depth output, having a black color, or a white background behind the window can cause this. 
This is because most GDI rendering operations ignore or do not preserve alpha values.❗

* ⚠️Prerequisited windows settings to enable the background effects⚠️
    - Transparency effects enabled
    - Energy saver disabled
#
* ⚠️The background effects do not affect most modern windows (UWP/WinUI), 
apps with different front-end rendering (e.g Qt, Electron, Chromium etc.. programs) and native windows with hardcoded colors.⚠️

* ⚠️If parts of the Windows UI colors remain modified after disabling the modification, this is happening when new system colors are applied in a selected Windows custom theme.
Changing the theme to the default and vice versa fixes the problem. As a last resort, you can delete the registry key HKEY_CURRENT_USER\Control Panel\Colors and reboot.⚠️

* ⚠️ARM64 system is only partially supported.⚠️

* ❕The blur effect may show a bleeding effect at the edges of a window when maximized or snapped to the edge of the screen. 
This is caused by default by the AccentBlur API.❕

* ✨The mod works best on the default dark theme.✨

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- RenderingMod:
    - ThemeBackground: TRUE
      $name: 🔷 Windows theme custom rendering
      $description: >-
       Modifies parts of the Windows theme using the Direct2D graphics API and modifies 
       Windows GDI text rendering by patching the alpha channel and adjusting text colors.
        ✨It is recommended to enable this with background translucent effects.
    - SysColors: FALSE
      $name: 🔷 New system colors
      $description: >-
       Modifies additional system UI colors by calling SetSysColors API. (Requires Windows theme custom rendering)
        ⚠️For issues with excluded processes, use process rules in mod's settings. For more refer to the FAQ.
    - AccentColorControls: TRUE
      $name: 🔷 Windows theme accent colorizer
      $description: >-
       Paint with accent color parts of windows theme. (Requires Windows theme custom rendering)
  $name: 🔶 Theme Customization
- BackgroundEffects:
    - type: acrylicblur
      $name: 🔷 Background effects
      $description: >-
        Windows 11 version >= 22621.xxx (22H2) is required for SystemBackdrop effects.
      $options:
      - none: Default
      - acrylicblur: Blur (AccentBlurBehind)
      - acrylicsystem: Acrylic (SystemBackdrop)
      - mica: Mica (SystemBackdrop)
      - mica_tabbed: MicaAlt (SystemBackdrop)
    - AccentBlurBehind: "3A232323"
      $name: 🔷 AccentBlurBehind color blend
      $description: >-
        Blending color with blur background.
        Color in hexadecimal ARGB format e.g. 3A232323
  $name: 🔶 Translucent Effects
- FlyoutsEffects: TRUE
  $name: 🔶 Flyout effects
  $description: >-
    Expand the effects to Win32 flyouts (context menus, dropdown menus, tooltips)
     ✨It is recommended to enable this with both background translucent effects and Windows theme custom rendering.
- RuledPrograms:
    - - target: "mspaint.exe"
        $name: 🔶 Process
        $description: >-
         Entries can be process names, paths or subdirectories for example:
          • Taskmgr.exe
          • C:\Windows\System32\Taskmgr.exe
          • C:\Windows
      - RenderingMod:
          - ThemeBackground: FALSE
            $name: 🔷 Windows theme custom rendering
            $description: >-
              Modifies parts of the Windows theme using the Direct2D graphics API and modifies Windows GDI text rendering by patching the alpha channel and adjusting text colors.
               ✨It is recommended to enable this with background translucent effects.
          - AccentColorControls: FALSE
            $name: 🔷 Windows theme accent colorizer
            $description: >-
              Paint with accent color parts of windows theme. (Requires Windows theme custom rendering)
        $name: 🔶 Theme Customization
      - BackgroundEffects:
        - type: none
          $name: 🔷 Background translucent effects
          $description: >-
           Windows 11 version >= 22621.xxx (22H2) is required for SystemBackdrop effects.
          $options:
          - none: Default
          - acrylicblur: Blur (AccentBlurBehind)
          - acrylicsystem: Acrylic (SystemBackdrop)
          - mica: Mica (SystemBackdrop)
          - mica_tabbed: MicaAlt (SystemBackdrop)
        - AccentBlurBehind: "3A232323"
          $name: 🔷 AccentBlurBehind color blend
          $description: >-
           Blending color with blur background.
            Color in hexadecimal ARGB format e.g. 3A232323
        $name: 🔶 Translucent Effects
  $name: ⏩ Process Rules
  $description: >-
      Add rules to each specified process or processes from specific subdirectories
       ❗ Add process rules for the excluded process instead of using Windhawk's process exclusion when the "New system colors" global setting is enabled.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <vssym32.h>
#include <uxtheme.h>
#include <cmath>
#include <string>
#include <array>
#include <d2d1.h>
#include <wrl.h>
#include <ShellScalingApi.h>

#define RECTWIDTH(lprc)     ((lprc)->right - (lprc)->left)
#define RECTHEIGHT(lprc)    ((lprc)->bottom - (lprc)->top)

#ifdef _WIN64
#define THISCALL  __cdecl
#define _THISCALL L"__cdecl"
#else
#define THISCALL  __thiscall
#define _THISCALL L"__thiscall"
#endif

#ifdef _WIN64
#define STDCALL  __cdecl
#define _STDCALL L"__cdecl"
#else
#define STDCALL  __stdcall
#define _STDCALL L"__stdcall"
#endif

static constexpr UINT ENABLE = 1;
static constexpr UINT AUTO = 0; // DWMSBT_AUTO
//static constexpr UINT NONE = 1; // DWMSBT_NONE
static constexpr UINT MAINWINDOW = 2; // DWMSBT_MAINWINDOW
static constexpr UINT TRANSIENTWINDOW = 3; // DWMSBT_TRANSIENTWINDOW
static constexpr UINT TABBEDWINDOW = 4; // DWMSBT_TABBEDWINDOW

// Get DPI value from the primary monitor without dependance to DPI-aware API
// TODO: Get DPI per window monitor
UINT GetDpiFromMonitor()
{
    // Get monitor handle without the need of a window handle
    HMONITOR hPrimary = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
    UINT dpiX = USER_DEFAULT_SCREEN_DPI, dpiY = USER_DEFAULT_SCREEN_DPI;
    if (SUCCEEDED(GetDpiForMonitor(hPrimary, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)))
        return dpiY;
    return USER_DEFAULT_SCREEN_DPI;
}
UINT g_Dpi = GetDpiFromMonitor();

typedef HRESULT(WINAPI* pDrawTextWithGlow)(HDC hdcMem, LPWSTR pszText, UINT cch, RECT* pRect, DWORD dwFlags, COLORREF crText,
                                          COLORREF crGlow, UINT nGlowRadius, UINT nGlowIntensity, BOOL fPreMultiply,
                                          DTT_CALLBACK_PROC pfnDrawTextCallback, LPARAM lParam);
static auto DrawTextWithGlow = (pDrawTextWithGlow)GetProcAddress(GetModuleHandle(L"uxtheme.dll"), MAKEINTRESOURCEA(126));

// Detects system dark/light theme mode
typedef BOOL(WINAPI* pShouldSystemUseDarkMode)();
static auto ShouldSystemUseDarkMode = (pShouldSystemUseDarkMode)GetProcAddress(GetModuleHandle(L"uxtheme.dll"), MAKEINTRESOURCEA(138));
BOOL g_IsSysThemeDarkMode = ShouldSystemUseDarkMode();

// Treeview window handle used in our CNscTree_DrawDivider() hook to draw our alpha blended navigation pane divider
thread_local HWND tl_hwndTreeView = nullptr;

// Redirect per ruled program the system colors to hardcoded default ones 
// when custom system colors are applied in global settings.
BOOL g_DefaultSysColors = FALSE;
// Global system colors buffers like Windows does.
std::array<HBRUSH, COLOR_MENUBAR + 1> g_themeCachedCustomSysColorBrushes {nullptr};
std::array<HBRUSH, COLOR_MENUBAR + 1> g_themeCachedDefaultSysColorBrushes {nullptr};
SRWLOCK g_SysColorsLock = SRWLOCK_INIT;

// Helpers for resetting theming containers and attributes
std::wstring GetCurrentWindowsThemePath();
// Lock in order to safely reset theme cache and attributes on theme change
SRWLOCK g_ThemeChangeLock = SRWLOCK_INIT;
std::wstring g_LastThemePath = GetCurrentWindowsThemePath();

// Flag to prevent customizing text colors of Task Manager
BOOL g_InsideTaskMgrProc = FALSE;

using PUNICODE_STRING = PVOID;
constexpr auto MENUPOPUP_CLASS = L"#32768";
constexpr UINT THEMECLS_COMMONPROPS_PART = 0;

ATOM g_explorerStylerNoBackgroundEffectAtom = 0;

struct Settings{
    BOOL FillBg = FALSE;
    BOOL AccentColorize = FALSE;
    COLORREF AccentColor = 0xFFFFFFFF;
    BOOL TextAlphaBlend = FALSE;
    BOOL SetSystemColors = FALSE;
    COLORREF AccentBlurBehindClr = 0x00000000;
    BOOL FlyoutsEffects = FALSE;
    BOOL Unload = FALSE;

    enum BACKGROUNDTYPE
    {
        Default,
        AccentBlurBehind,
        AcrylicSystemBackdrop,
        Mica,
        MicaAlt,
    } BgType = Default;

} g_settings;

struct ACCENT_POLICY 
{
    INT AccentState;
    INT AccentFlags;
    INT GradientColor;
    INT AnimationId;
};

enum ACCENT_STATE
{
    ACCENT_STATE_DISABLED,
    ACCENT_STATE_ENABLE_GRADIENT,
    ACCENT_STATE_ENABLE_TRANSPARENTGRADIENT,
    ACCENT_STATE_ENABLE_BLURBEHIND,	// Removed in Windows 11 22H2+
    ACCENT_STATE_ENABLE_ACRYLICBLURBEHIND,
    ACCENT_STATE_ENABLE_HOSTBACKDROP,
    ACCENT_STATE_INVALID_STATE
};

enum ACCENT_FLAG
{
    ACCENT_FLAG_NONE,
    ACCENT_FLAG_ENABLE_MODERN_ACRYLIC_RECIPE = 1 << 1,	// Windows 11 22H2+
    ACCENT_FLAG_ENABLE_GRADIENT_COLOR = 1 << 1, // ACCENT_ENABLE_BLURBEHIND
    ACCENT_FLAG_ENABLE_FULLSCREEN = 1 << 2,
    ACCENT_FLAG_ENABLE_BORDER_LEFT = 1 << 5,
    ACCENT_FLAG_ENABLE_BORDER_TOP = 1 << 6,
    ACCENT_FLAG_ENABLE_BORDER_RIGHT = 1 << 7,
    ACCENT_FLAG_ENABLE_BORDER_BOTTOM = 1 << 8,
    ACCENT_FLAG_ENABLE_BLUR_RECT = 1 << 9,	// DwmpUpdateAccentBlurRect, it is conflicted with ACCENT_ENABLE_GRADIENT_COLOR when using ACCENT_ENABLE_BLURBEHIND
    ACCENT_FLAG_ENABLE_BORDER = ACCENT_FLAG_ENABLE_BORDER_LEFT | ACCENT_FLAG_ENABLE_BORDER_TOP 
    | ACCENT_FLAG_ENABLE_BORDER_RIGHT | ACCENT_FLAG_ENABLE_BORDER_BOTTOM
};

struct WINCOMPATTRDATA 
{
    DWORD Attrib;
    PVOID pvData;
    SIZE_T cbData;
};

enum WINDOWCOMPOSITIONATTRIB 
{
    WCA_UNDEFINED,
    WCA_NCRENDERING_ENABLED,
    WCA_NCRENDERING_POLICY,
    WCA_TRANSITIONS_FORCEDISABLED,
    WCA_ALLOW_NCPAINT,
    WCA_CAPTION_BUTTON_BOUNDS,
    WCA_NONCLIENT_RTL_LAYOUT,
    WCA_FORCE_ICONIC_REPRESENTATION,
    WCA_EXTENDED_FRAME_BOUNDS,
    WCA_HAS_ICONIC_BITMAP,
    WCA_THEME_ATTRIBUTES,
    WCA_NCRENDERING_EXILED,
    WCA_NCADORNMENTINFO,
    WCA_EXCLUDED_FROM_LIVEPREVIEW,
    WCA_VIDEO_OVERLAY_ACTIVE,
    WCA_FORCE_ACTIVEWINDOW_APPEARANCE,
    WCA_DISALLOW_PEEK,
    WCA_CLOAK,
    WCA_CLOAKED,
    WCA_ACCENT_POLICY,
    WCA_FREEZE_REPRESENTATION,
    WCA_EVER_UNCLOAKED,
    WCA_VISUAL_OWNER,
    WCA_HOLOGRAPHIC,
    WCA_EXCLUDED_FROM_DDA,
    WCA_PASSIVEUPDATEMODE,
    WCA_USEDARKMODECOLORS,
    WCA_CORNER_STYLE,
    WCA_PART_COLOR,
    WCA_DISABLE_MOVESIZE_FEEDBACK,
    WCA_LAST
};

typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
auto SetWindowCompositionAttribute = (pSetWindowCompositionAttribute) GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");

ID2D1Factory* g_d2dFactory = nullptr;

VOID InitDirect2D()
{
    if (!g_d2dFactory)
    {
        D2D1_FACTORY_OPTIONS options = {};
        D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, options, &g_d2dFactory);
    }
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT *puPtrLen)
{
    void *pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResourceW(hModule, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION);
    if (hResource)
    {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal)
        {
            void *pData = LockResource(hGlobal);
            if (pData)
            {
                if (!VerQueryValueW(pData, L"\\", &pFixedFileInfo, &uPtrLen)
                || uPtrLen == 0)
                {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen)
    {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO *)pFixedFileInfo;
}

/*
  * Loads comctl32.dll, version 6.0.
  * This uses an activation context that uses shell32.dll's manifest
  * to load 6.0, even in apps which don't have the proper manifest for
  * it.
  * From: https://github.com/ramensoftware/windhawk-mods/blob/main/mods/classic-list-group-fix.wh.cpp
*/
HMODULE LoadComCtlModule(void)
{
    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    ACTCTXW actCtx = { sizeof(actCtx) };
    actCtx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_HMODULE_VALID;
    actCtx.lpResourceName = MAKEINTRESOURCEW(124);
    actCtx.hModule = hShell32;
    HANDLE hActCtx = CreateActCtxW(&actCtx);
    ULONG_PTR ulCookie;
    ActivateActCtx(hActCtx, &ulCookie);
    HMODULE hComCtl = LoadLibraryExW(L"comctl32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    /**
      * Certain processes will ignore the activation context and load
      * comctl32.dll 5.82 anyway. If that occurs, just reject it.
      */
    VS_FIXEDFILEINFO *pVerInfo = GetModuleVersionInfo(hComCtl, nullptr);
    if (!pVerInfo || HIWORD(pVerInfo->dwFileVersionMS) < 6)
    {
        FreeLibrary(hComCtl);
        hComCtl = NULL;
    }
    DeactivateActCtx(0, ulCookie);
    ReleaseActCtx(hActCtx);
    FreeLibrary(hShell32);
    return hComCtl;
}

using NtUserCreateWindowEx_t = HWND(WINAPI*)(DWORD, PUNICODE_STRING, LPCWSTR, PUNICODE_STRING, DWORD, LONG, LONG, LONG, LONG, HWND, HMENU, HINSTANCE, LPVOID, DWORD, DWORD, DWORD, VOID*);
NtUserCreateWindowEx_t NtUserCreateWindowEx_Original;

static decltype(&DwmExtendFrameIntoClientArea) DwmExtendFrameIntoClientArea_orig = nullptr;
static decltype(&DwmSetWindowAttribute) DwmSetWindowAttribute_orig = nullptr;

static decltype(&DrawTextW) DrawTextW_orig = nullptr;
static decltype(&ExtTextOutW) ExtTextOutW_orig = nullptr;
static decltype(&DrawThemeText) DrawThemeText_orig = nullptr;
static decltype(&DrawThemeTextEx) DrawThemeTextEx_orig = nullptr;

static decltype(&GetThemeColor) GetThemeColor_orig = nullptr;
static decltype(&DrawThemeBackground) DrawThemeBackground_orig = nullptr;
static decltype(&DrawThemeBackgroundEx) DrawThemeBackgroundEx_orig = nullptr;
static decltype(&GetThemeMargins) GetThemeMargins_orig = nullptr;
static decltype(&GetThemeTransitionDuration) GetThemeTransitionDuration_orig = nullptr;
static decltype (&GetThemeFont) GetThemeFont_orig = nullptr;
static decltype(&GetSysColor) GetSysColor_orig = GetSysColor; // Assign the pointer to the original function as we call HookedSysColor() even if the function isn't hooked.
static decltype(&GetSysColorBrush) GetSysColorBrush_orig = GetSysColorBrush;
static decltype(&FillRect) FillRect_orig = nullptr;
static decltype(&DrawThemeEdge) DrawThemeEdge_orig = nullptr;
static decltype(&DefWindowProcW) DefWindowProc_orig = nullptr;

VOID NewWindowShown(HWND);
VOID HandleEffects(HWND hWnd);

std::wstring GetWindowClass(HWND hWnd)
{
    if (!hWnd)
        return L"";
    WCHAR buffer[MAX_PATH];
    GetClassNameW(hWnd, buffer, MAX_PATH);
    return buffer;
}

BOOL IsWindowClass(HWND hWnd, LPCWSTR className)
{
    if (!hWnd)
        return FALSE;
    return GetWindowClass(hWnd) == className;
}

BOOL IsWindowCloaked(HWND hwnd) {
    BOOL isCloaked = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &isCloaked,
                                           sizeof(isCloaked))) &&
           isCloaked;
}

BOOL isWindowFlyout(HWND hWnd)
{
    return IsWindowClass(hWnd, TOOLTIPS_CLASS) || IsWindowClass(hWnd, L"DropDown") || IsWindowClass(hWnd, L"ViewControlClass") 
           || IsWindowClass(hWnd, MENUPOPUP_CLASS) || IsWindowClass(hWnd, L"MicrosoftWindowsTooltip") ||IsWindowClass(hWnd, L"BaseBar"); // Support m417z Folder Hover Menu mod
}

BOOL IsWindowEligible(HWND hWnd) 
{   
    // store the treeview handle for alpha blending when drawing the divider in the navigation pane hook.
    if (g_settings.FillBg && IsWindowClass(hWnd, L"SysTreeView32"))
        tl_hwndTreeView = hWnd;
    
    if (isWindowFlyout(hWnd) && g_settings.FlyoutsEffects)
        return TRUE;
    
    LONG_PTR styleEx = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    
    HWND hParentWnd = GetAncestor(hWnd, GA_PARENT);
    if (hParentWnd && hParentWnd != GetDesktopWindow())
        return FALSE;
    
    BOOL hasTitleBar = (style & WS_CAPTION) == WS_CAPTION;
    BOOL hasCaptionButtons = (style & (WS_MINIMIZEBOX | WS_MAXIMIZEBOX)) != 0;
    BOOL hasSystemMenu = (style & WS_SYSMENU) != 0;
    BOOL hasThickFrame = (style & WS_THICKFRAME) == WS_THICKFRAME;
    BOOL isWindowCEF = (IsWindowClass(hWnd, L"Chrome_WidgetWin_1") || IsWindowClass(hWnd, L"Chrome_WidgetWin_0"));

    // https://devblogs.microsoft.com/oldnewthing/20200302-00/?p=103507
    // Allow containers of Windows Store apps (WinStore.exe, Settings.exe, etc.)
    // Allow also Chromium Embedded Framework (Brave.exe) created as cloaked.
    if (IsWindowCloaked(hWnd) && !IsWindowClass(hWnd, L"ApplicationFrameWindow") && !isWindowCEF)
        return FALSE;

    // Windows become disabled even when they are displayed (e.g. Recycle Bin) when a pop-up window opens in front.
    if (!IsWindowEnabled(hWnd) && !IsWindowVisible(hWnd))
        return FALSE;
    
    // Pass ineligible CEF windows like Discord/Vencord
    if (isWindowCEF && (hasCaptionButtons || hasTitleBar))
        return TRUE;
    // Fixes Snipping Tool recording
    if ((styleEx & WS_EX_NOACTIVATE) || (styleEx & WS_EX_TRANSPARENT))
        return FALSE;
    // Most top-level windows
    if ((style & WS_POPUPWINDOW) == WS_POPUPWINDOW || (style & WS_OVERLAPPEDWINDOW) == WS_OVERLAPPEDWINDOW 
       || (styleEx & WS_EX_DLGMODALFRAME) == WS_EX_DLGMODALFRAME) // || (styleEx & WS_EX_CONTROLPARENT) == WS_EX_CONTROLPARENT)
            return TRUE;
    // Overlapped windows like the Win32 progress window
    if (hasTitleBar && hasSystemMenu && (hasCaptionButtons || hasThickFrame))
        return TRUE;

    return FALSE;
}

std::wstring GetCurrentWindowsThemePath() 
{
    std::wstring themePath;
    HKEY hKey = nullptr;

    LSTATUS status = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes",
        0,
        KEY_READ,
        &hKey
    );

    if (status != ERROR_SUCCESS)
        return L"";

    // 2. Query the size of the string buffer first
    DWORD bufferSize = 0;
    status = RegQueryValueExW(
        hKey,
        L"CurrentTheme",
        nullptr,
        nullptr,
        nullptr, // Pass nullptr to just get the required size
        &bufferSize
    );

    // 3. Allocate the string buffer and fetch the actual data
    if (status == ERROR_SUCCESS && bufferSize > 0) 
    {
        // Resize our wstring to fit the data (bufferSize includes the null terminator)
        themePath.resize(bufferSize / sizeof(wchar_t) - 1);

        status = RegQueryValueExW(
            hKey,
            L"CurrentTheme",
            nullptr,
            nullptr,
            reinterpret_cast<LPBYTE>(&themePath[0]),
            &bufferSize
        );

        if (status != ERROR_SUCCESS)
            themePath.clear();
    }
    RegCloseKey(hKey);

    return themePath;
}

std::wstring GetProcStrFromPath(std::wstring path) {
    size_t pos = path.find_last_of(L"\\/");
    if (pos != std::wstring::npos && pos + 1 < path.length()) {
        path = path.substr(pos + 1);
    }

    if (!path.empty()) 
    {
        LCMapStringEx(
            LOCALE_NAME_USER_DEFAULT, 
            LCMAP_LOWERCASE,
            path.c_str(),
            path.length(),
            &path[0],
            path.length(),
            nullptr, nullptr, 0);
    }
    return path;
}

std::wstring GetCurrProcStr() {
    WCHAR modulePath[MAX_PATH];
    GetModuleFileNameW(NULL, modulePath, MAX_PATH);

    return GetProcStrFromPath(modulePath);
}

BOOL InExplorerProcess() {
    return GetCurrProcStr() == L"explorer.exe";
}

BOOL InTaskManagerProcess() {
    return GetCurrProcStr() == L"taskmgr.exe";
}

enum AccentColorShade
{
    SystemAccentColorLight3,
    SystemAccentColorLight2,
    SystemAccentColorLight1,
    SystemAccentColorBase,
    SystemAccentColorDark1,
    SystemAccentColorDark2,
    SystemAccentColorDark3,
    Unused,
    AccentColorCount
};

class AccentPalette
{
public:
    std::array<COLORREF, AccentColorCount> Colors;
    BOOL LoadAccentPalette();
    AccentPalette()
    {
        LoadAccentPalette();
    }
};
AccentPalette g_AccentPalette;

BOOL AccentPalette::LoadAccentPalette()
{
    const LPCWSTR kAccentRegPath = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent";
    const LPCWSTR kAccentPaletteValue = L"AccentPalette";

    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kAccentRegPath, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return FALSE;

    BYTE data[32] = {};
    DWORD dataSize = sizeof(data);
    DWORD type = 0;

    if (RegQueryValueExW(hKey, kAccentPaletteValue, nullptr, &type, data, &dataSize) != ERROR_SUCCESS || type != REG_BINARY || dataSize < AccentColorCount * 4)
    {
        RegCloseKey(hKey);
        return FALSE;
    }

    RegCloseKey(hKey);

    for (INT i = 0; i < AccentColorCount; ++i)
    {
        DWORD color = *reinterpret_cast<DWORD*>(&data[i * 4]);
        Colors[i] = color;
    }
    return TRUE;
}

BOOL GetAccentColor(COLORREF& outColor)
{
    // In some programs, e.g. snippingtool.exe, the default blue accent color is used instead of the Windows theme with DwmGetColorizationColor.
    // Use the immersive color API if available, fall back to DwmGetColorizationColor
    // https://github.com/ALTaleX531/TranslucentFlyouts/blob/017970cbac7b77758ab6217628912a8d551fcf7c/Common/ThemeHelper.hpp#L278
    static const auto s_GetImmersiveColorFromColorSetEx{reinterpret_cast<DWORD(WINAPI*)(DWORD dwImmersiveColorSet, DWORD dwImmersiveColorType, BOOL bIgnoreHighContrast, DWORD dwHighContrastCacheMode)>(GetProcAddress(GetModuleHandleW(L"UxTheme.dll"), MAKEINTRESOURCEA(95)))};
    static const auto s_GetImmersiveColorTypeFromName{reinterpret_cast<DWORD(WINAPI*)(LPCWSTR name)>(GetProcAddress(GetModuleHandleW(L"UxTheme.dll"), MAKEINTRESOURCEA(96)))};
    static const auto s_GetImmersiveUserColorSetPreference{reinterpret_cast<DWORD(WINAPI*)(BOOL bForceCheckRegistry, BOOL bSkipCheckOnFail)>(GetProcAddress(GetModuleHandleW(L"UxTheme.dll"), MAKEINTRESOURCEA(98)))};

    COLORREF AccentClr{ 0 };
    BOOL opaque = FALSE;
    
    if (s_GetImmersiveColorFromColorSetEx && s_GetImmersiveColorTypeFromName && s_GetImmersiveUserColorSetPreference) 
    {
        AccentClr = s_GetImmersiveColorFromColorSetEx(
            s_GetImmersiveUserColorSetPreference(FALSE, FALSE),
            s_GetImmersiveColorTypeFromName(L"ImmersiveStartHoverBackground"),
            TRUE,
            0
        );
        outColor = RGB((AccentClr & 0xFF), (AccentClr >> 8) & 0xFF, (AccentClr >> 16) & 0xFF);
        return TRUE;
    }
    else if (SUCCEEDED(DwmGetColorizationColor(&AccentClr, &opaque)))
    {
        outColor = RGB((AccentClr >> 16) & 0xFF, (AccentClr >> 8) & 0xFF,  AccentClr & 0xFF);
        return TRUE;
    }
    else
    {
        outColor = DWMWA_COLOR_DEFAULT;
        return FALSE;
    }
}

D2D1_COLOR_F MyD2D1Color(BYTE A, BYTE R, BYTE G, BYTE B)
{
    return D2D1_COLOR_F{
        static_cast<FLOAT>(R) / 255.0f,
        static_cast<FLOAT>(G) / 255.0f,
        static_cast<FLOAT>(B) / 255.0f,
        static_cast<FLOAT>(A) / 255.0f
    };
}

D2D1_COLOR_F MyD2D1Color(BYTE R, BYTE G, BYTE B)
{
    return MyD2D1Color(255, R, G, B);
}

D2D1_COLOR_F IsAccentColorPossibleD2D(BYTE A, BYTE R, BYTE G, BYTE B, AccentColorShade AccentShade = SystemAccentColorBase)
{
    if (g_settings.AccentColorize)
    {       
        // Change light/dark accent shades to dark/light depending theme dark mode
        /* if (ShouldSystemUseDarkMode() && AccentShade > 3)
            AccentShade = static_cast<AccentColorShade>(6 - AccentShade);
        else if (!ShouldSystemUseDarkMode() && AccentShade < 3)
            AccentShade = static_cast<AccentColorShade>(6 - AccentShade); */
        R = GetRValue(g_AccentPalette.Colors[(INT)AccentShade]);
        G = GetGValue(g_AccentPalette.Colors[(INT)AccentShade]);
        B = GetBValue(g_AccentPalette.Colors[(INT)AccentShade]);
        return MyD2D1Color(A, R, G, B);
    }
    else
        return MyD2D1Color(A, R, G, B);
}

D2D1_COLOR_F IsAccentColorPossibleD2D(BYTE R, BYTE G, BYTE B, AccentColorShade AccentShade = SystemAccentColorBase)
{
    if (g_settings.AccentColorize)
    {   
        // Change light/dark accent shades to dark/light depending theme dark mode
        /* if (ShouldSystemUseDarkMode() && AccentShade > 3)
            AccentShade = static_cast<AccentColorShade>(6 - AccentShade);
        else if (!ShouldSystemUseDarkMode() && AccentShade < 3)
            AccentShade = static_cast<AccentColorShade>(6 - AccentShade); */
        R = GetRValue(g_AccentPalette.Colors[(INT)AccentShade]);
        G = GetGValue(g_AccentPalette.Colors[(INT)AccentShade]);
        B = GetBValue(g_AccentPalette.Colors[(INT)AccentShade]);
        return MyD2D1Color(255, R, G, B);
    }
    else
        return MyD2D1Color(R, G, B);
}

HRESULT WINAPI HookedDwmSetWindowAttribute(HWND hWnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute)
{    
    if(!IsWindowEligible(hWnd))
        return DwmSetWindowAttribute_orig(hWnd, dwAttribute, pvAttribute, cbAttribute);
    
    // Popup menus (#32768) pass here by default to paint 
    // handle by the internal uxtheme function CThemeMenuPopup::EnableRoundedCorners()
    if (IsWindowClass(hWnd, MENUPOPUP_CLASS) && g_settings.FlyoutsEffects && dwAttribute == DWMWA_WINDOW_CORNER_PREFERENCE) {
        UINT menuCornerRadius = DWMWCP_ROUND;
        return DwmSetWindowAttribute_orig(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &menuCornerRadius, sizeof(menuCornerRadius));
    }
    
    if ((dwAttribute == DWMWA_SYSTEMBACKDROP_TYPE || dwAttribute == DWMWA_USE_HOSTBACKDROPBRUSH) && g_settings.BgType != g_settings.Default)
    {
        if (g_settings.BgType == g_settings.AccentBlurBehind)
            return DwmSetWindowAttribute_orig(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &AUTO, sizeof(UINT));
        if(g_settings.BgType == g_settings.AcrylicSystemBackdrop)
            return DwmSetWindowAttribute_orig(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &TRANSIENTWINDOW, sizeof(UINT));
        else if(g_settings.BgType == g_settings.MicaAlt)
            return DwmSetWindowAttribute_orig(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &TABBEDWINDOW, sizeof(UINT));
        else if(g_settings.BgType == g_settings.Mica)
            return DwmSetWindowAttribute_orig(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &MAINWINDOW, sizeof(UINT));
    }
    
    return DwmSetWindowAttribute_orig(hWnd, dwAttribute, pvAttribute, cbAttribute);
}

HRESULT WINAPI HookedDwmExtendFrameIntoClientArea(HWND hWnd, const MARGINS* pMarInset)
{
    if(!IsWindowEligible(hWnd))
        [[clang::musttail]]return DwmExtendFrameIntoClientArea_orig(hWnd, pMarInset);
    
    if(!IsWindowClass(hWnd, L"CASCADIA_HOSTING_WINDOW_CLASS")) {
        static const MARGINS margins = {-1, -1, -1, -1};
        [[clang::musttail]]return DwmExtendFrameIntoClientArea_orig(hWnd, &margins);
    }
    else
        [[clang::musttail]]return DwmExtendFrameIntoClientArea_orig(hWnd, pMarInset);
}

HWND WINAPI HookedNtUserCreateWindowEx(DWORD dwExStyle,
                                       PUNICODE_STRING UnsafeClassName,
                                       LPCWSTR         VersionedClass,
                                       PUNICODE_STRING UnsafeWindowName,
                                       DWORD           dwStyle,
                                       LONG            x,
                                       LONG            y,
                                       LONG            nWidth,
                                       LONG            nHeight,
                                       HWND            hWndParent,
                                       HMENU           hMenu,
                                       HINSTANCE       hInstance,
                                       LPVOID          lpParam,
                                       DWORD           dwShowMode,
                                       DWORD           dwUnknown1,
                                       DWORD           dwUnknown2,
                                       VOID*           qwUnknown3) 
{
    HWND hWnd = NtUserCreateWindowEx_Original(
        dwExStyle, UnsafeClassName, VersionedClass, UnsafeWindowName, dwStyle,
        x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam,
        dwShowMode, dwUnknown1, dwUnknown2, qwUnknown3);
    
    if(hWnd)
        NewWindowShown(hWnd);

    return hWnd;
}

std::wstring GetThemeClass(HTHEME hTheme) 
{
    typedef HRESULT(WINAPI* pGetThemeClass)(HTHEME, LPCTSTR, INT);
    static auto GetClassName = (pGetThemeClass)GetProcAddress(GetModuleHandleW(L"uxtheme"), MAKEINTRESOURCEA(74));

    std::wstring ret;
    if (GetClassName)
    {
        WCHAR buffer[255] = { 0 };
        HRESULT hr = GetClassName(hTheme, buffer, 255);
        return SUCCEEDED(hr) ? buffer : L"";
    }
    return ret;
}

// Alpha gamma correction LUT — built once at process startup.
// Applies sRGB inverse gamma ^(1/1.4) to coverage values, which:
// Brightens antialiased edge pixels
// Closely matches DrawTextWithGlow's CGamma table behavior
// Requires no RGB linearization — operates on alpha only
static std::array<BYTE, 256> g_textAlphaGammaLUT = {0};
VOID GenerateTextAlphaGammaLUT()
{
    for (INT i = 0; i < 256; ++i) 
    {
        float a = i * (1.0f / 255.0f);
        float g = powf(a, 1.0f / 1.4f);
        g_textAlphaGammaLUT[i] = (BYTE)(g * 255.0f + 0.5f);
    }
}

// BufferedPaintInit must be called once per thread before any BeginBufferedPaint
struct BPGuard {
    BPGuard()  { BufferedPaintInit(); }
    ~BPGuard() { BufferedPaintUnInit(); }
};
thread_local BPGuard tl_bpGuard;

BOOL WINAPI HookedExtTextOutW(
    HDC hdc,
    INT x,
    INT y,
    UINT options,
    const RECT* lprect,
    LPCWSTR lpString,
    UINT c,
    const INT* lpDx)
{
    if (!hdc || !lpString || !c)
        return ExtTextOutW_orig(hdc, x, y, options, lprect, lpString, c, lpDx);

    RECT textRect {0};
    SIZE textSize = {0};
    
    // Boundary calculations
    if (lprect)
        textRect = *lprect;
    else if (options & ETO_GLYPH_INDEX)
    {
        if (GetTextExtentPointI(hdc, (WORD*)lpString, c, &textSize))
        {
            textRect.left   = x;
            textRect.top    = y - textSize.cy;
            textRect.right  = x + textSize.cx;
            textRect.bottom = textSize.cy + y;
        }
        else
            return ExtTextOutW_orig(hdc, x, y, options, lprect, lpString, c, lpDx);
    }
    else if (options == ETO_IGNORELANGUAGE) {
        if(!GetClipBox(hdc, &textRect))
            return ExtTextOutW_orig(hdc, x, y, options, lprect, lpString, c, lpDx);
    }
    else if (options)
    {
        if (GetTextExtentPoint32W(hdc, lpString, c, &textSize))
        {
            textRect.left   = x;
            textRect.top    = y - textSize.cy;
            textRect.right  = x + textSize.cx;
            textRect.bottom = textSize.cy + y; 
        }
        else
            return ExtTextOutW_orig(hdc, x, y, options, lprect, lpString, c, lpDx);
    }
    else
        return ExtTextOutW_orig(hdc, x, y, options, lprect, lpString, c, lpDx);
    
    INT textRectWidth  = RECTWIDTH(&textRect);
    INT textRectHeight = RECTHEIGHT(&textRect);

    if (textRectWidth <= 0 || textRectHeight <= 0)
        return ExtTextOutW_orig(hdc, x, y, options, lprect, lpString, c, lpDx);

    // https://devblogs.microsoft.com/oldnewthing/20110520-00/?p=10613
    BP_PAINTPARAMS params = { sizeof(BP_PAINTPARAMS) };
    params.dwFlags = BPPF_ERASE | BPPF_NOCLIP;
    HDC memDC = nullptr;
    // Acquire OS cached bitmap
    HPAINTBUFFER hpb = BeginBufferedPaint(hdc, &textRect, BPBF_TOPDOWNDIB, &params, &memDC);
    if (!hpb) {
        Wh_Log(L"Failed BeginBufferedPaint error:0x%08x", GetLastError());
        return ExtTextOutW_orig(hdc, x, y, options, lprect, lpString, c, lpDx);
    }

    RGBQUAD* pPixels = nullptr;
    INT rowWidth = 0; // stride in RGBQUADs, not bytes
    if (FAILED(GetBufferedPaintBits(hpb, &pPixels, &rowWidth))) {
        EndBufferedPaint(hpb, FALSE); // was missing on this path - leaked hpb
        Wh_Log(L"Failed GetBufferedPaintBits error:0x%08x", GetLastError());
        return ExtTextOutW_orig(hdc, x, y, options, lprect, lpString, c, lpDx);
    }

    // Original DC coloring values
    COLORREF origTxtClr = GetTextColor(hdc);
    COLORREF origBkClr = GetBkColor(hdc);
    COLORREF highlightSysClr = GetSysColor(COLOR_HIGHLIGHT);
    BOOL HighlightedText = (options & ETO_OPAQUE && origBkClr == highlightSysClr);

    SelectObject(memDC, GetCurrentObject(hdc, OBJ_FONT));
    SetTextAlign(memDC, GetTextAlign(hdc));
    SetBkMode(memDC, TRANSPARENT);
    SetTextColor(memDC, RGB(255, 255, 255)); // White text mask

    if ((options & ETO_OPAQUE) && lprect) {
        HBRUSH brush = CreateSolidBrush(origBkClr);
        FillRect(hdc, lprect, brush);
        DeleteObject(brush);
    }

    WINBOOL res = ExtTextOutW_orig(memDC, x, y, options & ~ETO_OPAQUE, lprect, lpString, c, lpDx);

    // Compositing
    for (INT cy = 0; cy < textRectHeight; ++cy) {
        RGBQUAD* row = pPixels + (cy * rowWidth);
        for (INT cx = 0; cx < textRectWidth; ++cx) {
            RGBQUAD& p = row[cx];
            // Alpha extraction + gamma correction
            BYTE luma = (BYTE)((p.rgbBlue + (p.rgbGreen << 1) + p.rgbRed) >> 2);
            BYTE txtA = g_textAlphaGammaLUT[luma];
            if (txtA == 0 && !(options & ETO_OPAQUE))
                continue;
            // handle background transparency only for selected text
            BYTE bgWeight = HighlightedText ? (255 - txtA) : 0;
            p.rgbBlue     = (((GetBValue(origTxtClr) * txtA) + (GetBValue(origBkClr) * bgWeight)) >> 8);
            p.rgbGreen    = (((GetGValue(origTxtClr) * txtA) + (GetGValue(origBkClr) * bgWeight)) >> 8);
            p.rgbRed      = (((GetRValue(origTxtClr) * txtA) + (GetRValue(origBkClr) * bgWeight)) >> 8);
            p.rgbReserved = bgWeight ? 255 : txtA;
        }
    }

    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    AlphaBlend(hdc, textRect.left, textRect.top, textRectWidth, textRectHeight,
            memDC, textRect.left, textRect.top, textRectWidth, textRectHeight, blend);

    EndBufferedPaint(hpb, FALSE);
    return res;
}

// Bypass alpha blending operation done by default (e.g context menus, win32 adressbar) as it is done by our ExtTextOutW hook.
HRESULT WINAPI HookedDrawTextWithGlow(HDC hdcMem, LPWSTR pszText, UINT cch, RECT* pRect, DWORD dwFlags, COLORREF crText,
                                      COLORREF crGlow, UINT nGlowRadius, UINT nGlowIntensity, BOOL fPreMultiply,
                                      DTT_CALLBACK_PROC pfnDrawTextCallback, LPARAM lParam)
{
    if (pRect->right == pRect->left || pRect->bottom == pRect->top) { 
        if ((dwFlags & DT_CALCRECT) == 0)
            return DrawTextWithGlow(hdcMem, pszText, cch, pRect, dwFlags, crText, crGlow, nGlowRadius, nGlowIntensity, fPreMultiply, pfnDrawTextCallback, lParam);
    }

    // Do not mess with text using glow effects (if such exist in Win11)
    if (nGlowRadius > 0)
        return DrawTextWithGlow(hdcMem, pszText, cch, pRect, dwFlags, crText, crGlow, nGlowRadius, nGlowIntensity, fPreMultiply, pfnDrawTextCallback, lParam);

    SetTextColor(hdcMem, crText);
    SetBkColor(hdcMem, RGB(0, 0, 0));
    
    HRESULT hr = S_OK;
    
    if (pfnDrawTextCallback)
        hr = pfnDrawTextCallback(hdcMem, pszText, cch, pRect, dwFlags, lParam);
    else
        hr = DrawTextW(hdcMem, pszText, cch, pRect, dwFlags & ~DT_MODIFYSTRING);

    return hr;
}

INT WINAPI HookedDrawTextW(HDC hdc, LPCWSTR lpchText, INT cchText, LPRECT lprc, UINT format)
{   
    // Windows 11 context menus use Fluent icons as glyphs
    // Handled internally in the shell32 routine s_DrawGlyph()
    auto ContextMenuGlyphs = [&hdc, &lpchText]() {
        const WCHAR fluentIconGlyph_ChevronRightMed = 0xE974;
        const WCHAR fluentIconGlyph_ChevronLeftMed = 0xE973;
        const WCHAR fluentIconGlyph_AcceptMedium = 0xF78C;
        const WCHAR fluentIconGlyph_RadioBullet = 0xE915;
        
        if (*lpchText == fluentIconGlyph_ChevronRightMed || *lpchText == fluentIconGlyph_ChevronLeftMed 
            || *lpchText == fluentIconGlyph_AcceptMedium || *lpchText == fluentIconGlyph_RadioBullet)
                g_IsSysThemeDarkMode ? SetTextColor(hdc, RGB(255, 255, 255)) : SetTextColor(hdc, RGB(0, 0, 0));
    };

    // Catch and modify context menu fluent icons coloring
    if (cchText == 1)
        ContextMenuGlyphs();

    // Modify and convert hardcoded Syslink black text color into white (e.g. inside "Sharing" win32 Properties tab)
    if (!g_IsSysThemeDarkMode || (GetTextColor(hdc) & 0x00FFFFFF) != RGB(0, 0, 0))
        return DrawTextW_orig(hdc, lpchText, cchText, lprc, format);
    
    HWND hWnd = WindowFromDC(hdc);
    if (GetWindowClass(hWnd) == L"SysLink")
        SetTextColor(hdc, RGB(255, 255, 255));

    return DrawTextW_orig(hdc, lpchText, cchText, lprc, format);
}

HRESULT WINAPI HookedDrawThemeTextEx(HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, LPCWSTR pszText,
    INT cchText, DWORD dwTextFlags, LPRECT pRect, const DTTOPTS* pOptions)
{
    std::wstring ThemeClassName = GetThemeClass(hTheme);
    if (pOptions == nullptr) {
        DTTOPTS Options = { sizeof(DTTOPTS) };
        GetThemeColor(hTheme, iPartId, iStateId, TMT_TEXTCOLOR, &Options.crText);
        Options.dwFlags |= DTT_TEXTCOLOR;
        return DrawThemeTextEx_orig(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, (LPRECT)pRect, &Options);
    }

    if ((pOptions->dwFlags & DTT_CALCRECT))
        return DrawThemeTextEx_orig(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, (LPRECT)pRect, pOptions);
    
    DTTOPTS Options = { sizeof(DTTOPTS) };
    Options = *pOptions;
    GetThemeColor(hTheme, iPartId, iStateId, TMT_TEXTCOLOR, &Options.crText);
    Options.dwFlags |= DTT_TEXTCOLOR;
    return DrawThemeTextEx_orig(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, (LPRECT)pRect, &Options);

}

HRESULT WINAPI HookedDrawThemeText(HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, LPCTSTR pszText,
    INT cchText, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect) 
{
    DTTOPTS Options = { sizeof(DTTOPTS) };
    RECT Rect = *pRect;

    GetThemeColor(hTheme, iPartId, iStateId, TMT_TEXTCOLOR, &Options.crText);
    HRESULT ret = HookedDrawThemeTextEx(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, &Rect, &Options);

    return ret;
}

// https://github.com/ramensoftware/windhawk-mods/blob/15e5d9838349e4b927ed8ac5433e9894ff6cda28/mods/uxtheme-hook.wh.cpp#L90
typedef VOID(CALLBACK *Element_PaintBgT)(class Element*, HDC , class Value*, LPRECT, LPRECT, LPRECT, LPRECT);
Element_PaintBgT Element_PaintBg;
VOID CALLBACK Element_PaintBgHook(class Element* This, HDC hdc, class Value* value, LPRECT pRect, LPRECT pClipRect, LPRECT pExcludeRect, LPRECT pTargetRect)
{   
    Element_PaintBg(This, hdc, value, pRect, pClipRect, pExcludeRect, pTargetRect);

    //unsigned char byteValue = *(reinterpret_cast<unsigned char*>(value) + 8);
    if ((INT)(*(DWORD *)value << 26) >> 26 != 9 )
    {
        auto v44 = *((__int64 *)value + 1);
        auto v45 = (v44+20)& 7;
        // 6-> selection
        // 3-> hovered stuff
        // 4-> cpanel top bar and side bar (white image)
        // 1-> some new cp page style (cp_hub_frame)
        if (v45 == 4)
            FillRect(hdc, pRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        else
            return;
    }
    else
        return;
}

VOID CplDuiHook()
{
    WindhawkUtils::SYMBOL_HOOK dui70dll_hooks[] =
    {
        {
            {
                L"public: void __cdecl DirectUI::Element::PaintBackground(struct HDC__ *,class DirectUI::Value *,struct tagRECT const &,struct tagRECT const &,struct tagRECT const &,struct tagRECT const &)"
            },
            &Element_PaintBg,
            Element_PaintBgHook,
            FALSE
        },
    };

    HMODULE hDui = LoadLibraryEx(L"dui70.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    WindhawkUtils::HookSymbols(hDui, dui70dll_hooks, ARRAYSIZE(dui70dll_hooks));
}

constexpr INT SysColorElements[] = {
	COLOR_SCROLLBAR ,
    COLOR_BACKGROUND ,
    COLOR_ACTIVECAPTION ,
    COLOR_INACTIVECAPTION ,
    COLOR_MENU ,
    COLOR_WINDOW ,
    COLOR_WINDOWFRAME ,
    COLOR_MENUTEXT ,
    COLOR_WINDOWTEXT ,
    COLOR_CAPTIONTEXT ,
    COLOR_ACTIVEBORDER ,
    COLOR_INACTIVEBORDER ,
    COLOR_APPWORKSPACE ,
    COLOR_HIGHLIGHT ,
    COLOR_HIGHLIGHTTEXT ,
    COLOR_BTNFACE ,
    COLOR_BTNSHADOW ,
    COLOR_GRAYTEXT ,
    COLOR_BTNTEXT ,
    COLOR_INACTIVECAPTIONTEXT ,
    COLOR_BTNHIGHLIGHT ,
    COLOR_3DDKSHADOW ,
    COLOR_3DLIGHT ,
    COLOR_INFOTEXT ,
    COLOR_INFOBK ,
    COLOR_GRADIENTACTIVECAPTION ,
    COLOR_GRADIENTINACTIVECAPTION ,
    COLOR_MENUHILIGHT ,
    COLOR_MENUBAR,
    COLOR_HOTLIGHT
};

void ClearSysColorsRegKey() {
    RegDeleteTreeW(HKEY_CURRENT_USER, L"Control Panel\\Colors");
}

HTHEME SetThemeHandle(HWND hWnd, HTHEME& hTheme, LPCWSTR themeclass)
{
    return hTheme = OpenThemeData(hWnd, themeclass);
}

VOID RevertSysColors()
{
    HTHEME hThemeSysMetrics = nullptr;
    if (!SetThemeHandle(nullptr, hThemeSysMetrics, L"sysmetrics"))
        return;
    
    COLORREF aNewColors[ARRAYSIZE(SysColorElements)];

    for (UINT i = 0; i < ARRAYSIZE(SysColorElements); i++) 
        aNewColors[i] = GetThemeSysColor(hThemeSysMetrics, i); 
    SetSysColors(ARRAYSIZE(SysColorElements), SysColorElements, aNewColors); 

    CloseThemeData(hThemeSysMetrics);
    hThemeSysMetrics = nullptr;

    //ClearSysColorsRegKey();
}

static COLORREF GetDefaultSysColor(INT nIndex)
{
    if (nIndex == COLOR_SCROLLBAR)
        return RGB(200, 200, 200);
    else if (nIndex == COLOR_BACKGROUND || nIndex == COLOR_MENUTEXT || nIndex == COLOR_WINDOWTEXT || nIndex == COLOR_CAPTIONTEXT
            || nIndex == COLOR_BTNTEXT || nIndex == COLOR_INACTIVECAPTIONTEXT || nIndex == COLOR_INFOTEXT)
                return RGB(0, 0, 0);
    else if (nIndex == COLOR_ACTIVECAPTION)
        return RGB(153, 180, 209);
    else if (nIndex == COLOR_INACTIVECAPTION)
        return RGB (191, 205, 219);
    else if (nIndex == COLOR_MENU || nIndex == COLOR_BTNFACE || nIndex == COLOR_MENUBAR)  
        return RGB(240, 240, 240);
    else if (nIndex == COLOR_WINDOW || nIndex == COLOR_BTNHIGHLIGHT || nIndex == COLOR_INFOBK || nIndex == COLOR_HIGHLIGHTTEXT)
        return RGB(255, 255, 255);
    else if (nIndex == COLOR_WINDOWFRAME)
        return RGB(100, 100, 100);
    else if (nIndex == COLOR_ACTIVEBORDER)
        return RGB(180, 180, 180);
    else if (nIndex == COLOR_INACTIVEBORDER)
        return RGB(244, 247, 252);
    else if (nIndex == COLOR_APPWORKSPACE)
        return RGB(171, 171, 171);
    else if (nIndex == COLOR_HIGHLIGHT || nIndex == COLOR_MENUHILIGHT)
        return RGB(0, 120, 212);
    else if (nIndex == COLOR_BTNSHADOW)
        return RGB(160, 160, 160);
    else if (nIndex == COLOR_GRAYTEXT)
        return RGB(109, 109, 109);
    else if (nIndex == COLOR_3DDKSHADOW)
        return RGB(105, 105, 105);
    else if (nIndex == COLOR_3DLIGHT)
        return RGB(227, 227, 227);
    else if (nIndex == COLOR_HOTLIGHT)
        return RGB(0, 102, 204);
    else if (nIndex == COLOR_GRADIENTACTIVECAPTION)
        return RGB(185, 209, 234);
    else if (nIndex == COLOR_GRADIENTINACTIVECAPTION)
        return RGB(215, 228, 242);
    
    return GetSysColor_orig(nIndex);
}

static COLORREF GetCustomSysColor(INT nIndex)
{
    if (nIndex == COLOR_SCROLLBAR || nIndex == COLOR_BACKGROUND || nIndex == COLOR_MENU || nIndex == COLOR_WINDOW || nIndex == COLOR_INACTIVEBORDER || nIndex == COLOR_INFOBK ||
        nIndex == COLOR_MENUBAR)
        return RGB(0, 0, 0);
    else if (nIndex == COLOR_GRADIENTACTIVECAPTION || nIndex == COLOR_INACTIVECAPTION)
        return (g_settings.AccentColorize) ? g_settings.AccentColor : RGB(0, 0, 0);
    else if (nIndex == COLOR_ACTIVECAPTION || nIndex == COLOR_GRADIENTINACTIVECAPTION)
        return (g_settings.AccentColorize) ? g_settings.AccentColor : RGB(32, 32, 32);
    else if (nIndex == COLOR_ACTIVEBORDER)
        return RGB(32, 32, 32);
    else if (nIndex == COLOR_BTNSHADOW)
        return RGB(32, 32, 32);
    else if (nIndex == COLOR_WINDOWFRAME)
        return RGB(96, 96, 96);
    else if (nIndex == COLOR_BTNHIGHLIGHT)
        return RGB(64, 64, 64);
    else if (nIndex == COLOR_WINDOWTEXT || nIndex == COLOR_MENUTEXT || nIndex == COLOR_CAPTIONTEXT ||
             nIndex == COLOR_BTNTEXT || nIndex == COLOR_INFOTEXT || nIndex == COLOR_HIGHLIGHTTEXT)
        return RGB(255, 255, 255);
    else if (nIndex == COLOR_APPWORKSPACE)
        return RGB(8, 8, 8);
    else if (nIndex == COLOR_HIGHLIGHT || nIndex == COLOR_MENUHILIGHT)
        return (g_settings.AccentColorize) ? g_settings.AccentColor : RGB(0, 120, 215);
    else if (nIndex == COLOR_BTNFACE)
        return RGB(1, 1, 1);
    else if (nIndex == COLOR_GRAYTEXT)
        return RGB(128, 128, 128);
    else if (nIndex == COLOR_INACTIVECAPTIONTEXT)
        return RGB(160, 160, 160);
    else if (nIndex == COLOR_3DDKSHADOW)
        return RGB(16, 16, 16);
    else if (nIndex == COLOR_3DLIGHT)
        return RGB(4, 4, 4);
    else if (nIndex == COLOR_HOTLIGHT)
        return (g_settings.AccentColorize) ? g_settings.AccentColor : RGB(0, 148, 251);

    return GetSysColor_orig(nIndex);
}

COLORREF WINAPI HookedGetSysColor(INT nIndex) 
{
    if (g_DefaultSysColors)
        return GetDefaultSysColor(nIndex);
    else
        return GetCustomSysColor(nIndex);
}

HBRUSH WINAPI HookedGetSysColorBrush(INT nIndex) 
{
    if (nIndex < 0 || nIndex > COLOR_MENUBAR)
        return GetSysColorBrush_orig(nIndex);
    
    auto& cacheArray = g_DefaultSysColors ? g_themeCachedDefaultSysColorBrushes : g_themeCachedCustomSysColorBrushes;
    
    HBRUSH cachedBrush = cacheArray[nIndex];
    
    if (cachedBrush && GetObjectType(cachedBrush) == OBJ_BRUSH)
        return cachedBrush; 

    AcquireSRWLockExclusive(&g_SysColorsLock);
    
    HBRUSH& refSysBrush = cacheArray[nIndex];
    
    if (refSysBrush && GetObjectType(refSysBrush) != OBJ_BRUSH)
        refSysBrush = NULL; 

    if (!refSysBrush) {
        COLORREF color = HookedGetSysColor(nIndex);
        refSysBrush = CreateSolidBrush(color);
    }

    HBRUSH hbr = refSysBrush;
    ReleaseSRWLockExclusive(&g_SysColorsLock);
    
    return hbr;
}

VOID ColorizeSysColors()
{   
    // Stop recalling SetSysColors if syscolor changes have been applied.
    // SetSysColors redraws all top level windows causing flickering.
    if (GetSysColor_orig(COLOR_WINDOW) == RGB(0, 0, 0))
        return;
    
    COLORREF aNewColors[ARRAYSIZE(SysColorElements)];
    for (UINT i = 0; i < ARRAYSIZE(SysColorElements); i++)
        aNewColors[i] = GetCustomSysColor(SysColorElements[i]);
        
    SetSysColors(ARRAYSIZE(SysColorElements), SysColorElements, aNewColors);
}

HRESULT WINAPI HookedGetColorTheme(HTHEME hTheme, INT iPartId, INT iStateId, INT iPropId, COLORREF *pColor) 
{
    HRESULT hr = GetThemeColor_orig(hTheme, iPartId, iStateId, iPropId, pColor);
    std::wstring ThemeClassName = GetThemeClass(hTheme);

    if (ThemeClassName == L"ItemsView" && iPropId == TMT_TEXTCOLOR && ((iPartId == 4 && iStateId == 1) || iPartId == 5))
    {
        *pColor = (!g_IsSysThemeDarkMode && *pColor == 0x006D6D6D) ? RGB(0, 0, 0) : *pColor;
        return S_OK;
    }
    if (ThemeClassName == L"ListView" && iPropId == TMT_TEXTCOLOR && iPartId == LVP_LISTITEM)
    {
        *pColor = (g_IsSysThemeDarkMode && (iStateId == THEMECLS_COMMONPROPS_PART || iStateId == LISS_SELECTED)) ? RGB(255, 255, 255) : *pColor;
        *pColor = (!g_IsSysThemeDarkMode && *pColor == 0x006D6D6D) ? RGB(0, 0, 0) : *pColor;
        return S_OK;
    }
    else if (ThemeClassName == L"ListView" && iPartId == LVP_GROUPHEADER)
    {
        *pColor = (g_IsSysThemeDarkMode) ? RGB(255, 255, 255) : *pColor;
        return S_OK;
    }
    else if (ThemeClassName == L"PreviewPane" && (iPartId == 5 || iPartId == 7 || iPartId == 6) && iPropId == TMT_FILLCOLOR) {
        *pColor = (iPartId == 6) ? RGB(192, 192, 192) : RGB(255, 255, 255);
        return S_OK;
    }
    else if (ThemeClassName == L"PreviewPane" && iPropId == TMT_TEXTCOLOR) {
        *pColor = g_IsSysThemeDarkMode ? RGB(255, 255, 255) : *pColor;
        return S_OK;
    }  
    else if (ThemeClassName == L"ControlPanelStyle" && iPropId == TMT_TEXTCOLOR)
    {
        if ((iPartId == CPANEL_BODYTITLE || iPartId == CPANEL_GROUPTEXT || iPartId == CPANEL_MESSAGETEXT 
            || iPartId == CPANEL_BODYTEXT || iPartId == CPANEL_TITLE || iPartId == CPANEL_CONTENTPANELABEL) && iStateId == 0)
        {
            *pColor =  (g_IsSysThemeDarkMode) ? RGB(255, 255, 255) : *pColor;
            return S_OK;
        }
        else if (iPartId == CPANEL_SECTIONTITLELINK && (iStateId == CPCL_NORMAL || iStateId == CPCL_HOT))
        {
            *pColor = (g_IsSysThemeDarkMode) ? ((iStateId == CPCL_NORMAL) ? RGB(240, 255, 240) : RGB(224, 255, 224)) : *pColor;
            return S_OK;                   
        }
        else if (iPartId == CPANEL_CONTENTLINK || iPartId == CPANEL_HELPLINK)
        {
            *pColor = (g_IsSysThemeDarkMode) ? ((iStateId == CPHL_NORMAL) ? RGB(96, 205, 255) : (iStateId == CPHL_HOT) ? RGB(153, 236, 255) : 
                      (iStateId == CPHL_PRESSED) ? RGB(0, 148, 251) : RGB(96, 96, 96)) : *pColor;
            return S_OK;
        }
        else if (iPartId == CPANEL_TASKLINK) 
        {
            *pColor = (g_IsSysThemeDarkMode) ? ((iStateId == CPTL_NORMAL) ? RGB(190, 190, 190): (iStateId == CPTL_HOT) ? RGB(255, 255, 255) : 
                      (iStateId == CPTL_PRESSED) ? RGB(160, 160, 160) : (iStateId == CPTL_DISABLED) ? RGB(96, 96, 96) : RGB(255, 255, 255)) : *pColor;
            return S_OK;
        }
    }     
    else if (ThemeClassName == L"ControlPanelStyle" && iPropId == TMT_FILLCOLORHINT && (iPartId == CPANEL_CONTENTPANELINE && iStateId == 0))
    {
        *pColor = RGB(64, 64, 64);
        return S_OK;
    }
    else if (ThemeClassName == L"CommandModule" && iPropId == TMT_TEXTCOLOR)
    {
        // TASKBUTTON
        if(iPartId == 3 && iStateId == 1)
        {
            *pColor = *pColor = (g_IsSysThemeDarkMode && *pColor == RGB(0, 0, 0)) ? RGB(255, 255, 255) : *pColor;
            return S_OK;
        }
        // LYBRARYPANETOPVIEW
        else if (iPartId == 9)
        {
            *pColor = (iStateId == 1) ? RGB(96, 205, 255) : (iStateId == 2) ? RGB(153, 236, 255) : (iStateId == 3) ? RGB(0, 148, 251) : RGB(96, 96, 96);
            return S_OK;
        }  
    }
    else if (ThemeClassName == L"TaskDialogStyle" && iPropId == TMT_TEXTCOLOR)
    {
        if (iPartId == TDLG_MAININSTRUCTIONPANE) {
            *pColor = RGB(96, 205, 255);
            return S_OK;
        }
        else if (iPartId == TDLG_CONTENTPANE || iPartId == TDLG_VERIFICATIONTEXT) {
            *pColor =  (g_IsSysThemeDarkMode && *pColor == RGB(0, 0, 0)) ? RGB(255, 255, 255) : *pColor;
            return S_OK;
        }
    }
    else if (ThemeClassName == L"Button" && iPropId == TMT_TEXTCOLOR)
    {
        if (iPartId == BP_PUSHBUTTON && iStateId != PBS_DISABLED)
        {
            *pColor = (g_IsSysThemeDarkMode && *pColor == RGB(0, 0, 0)) ? RGB(255, 255, 255) : *pColor;
            return S_OK;
        }
        else if (iPartId != BP_PUSHBUTTON)
        {
            *pColor = (g_IsSysThemeDarkMode && *pColor == RGB(0, 0, 0)) ? RGB(192, 192, 192) : *pColor;
            return S_OK;
        }   
    }
    else if (ThemeClassName == L"Static")
    {
        *pColor = (g_IsSysThemeDarkMode && *pColor < RGB(16, 16, 16)) ? RGB(255, 255, 255) : *pColor;
        return S_OK;
    }
    else if (ThemeClassName == L"TreeView" && iPropId == TMT_TEXTCOLOR)
    {
        *pColor = (g_IsSysThemeDarkMode && *pColor == RGB(0, 0, 0)) ? RGB(255, 255, 255) : *pColor;
        return S_OK;
    }
    else if (ThemeClassName == L"Tab" && iPropId == TMT_TEXTCOLOR)
    {
        if (iStateId == CSTB_HOT)
            *pColor = (g_IsSysThemeDarkMode && *pColor == RGB(0, 0, 0)) ? RGB(224, 224, 224) : *pColor;
        else if (iStateId == CSTB_SELECTED)
            *pColor = (g_IsSysThemeDarkMode && *pColor == RGB(0, 0, 0)) ? RGB(255, 255, 255) : *pColor;
        else
            *pColor = (g_IsSysThemeDarkMode && *pColor == RGB(0, 0, 0)) ? RGB(192, 192, 192) : *pColor;
        return S_OK;
    }
    else if (ThemeClassName == L"Edit" && iPropId == TMT_TEXTCOLOR)
    {
        if (iPartId == 1) {
            *pColor = (g_IsSysThemeDarkMode && *pColor == RGB(0, 0, 0)) ? RGB(255, 255, 255) : *pColor;
            return S_OK;
        }
        else if (iPartId == THEMECLS_COMMONPROPS_PART) {
            *pColor = (g_IsSysThemeDarkMode && (*pColor & 0xff000000) == RGB(0, 0, 0)) ? RGB(255, 255, 255) : *pColor;
            return S_OK;
        }
    }
    else if (ThemeClassName == L"Header" && iPropId == TMT_TEXTCOLOR)
    {
        *pColor = (g_IsSysThemeDarkMode) ? RGB(255, 255, 255) : *pColor;
        return S_OK;
    }
    else if (ThemeClassName == L"SearchEditBox" && iPropId == TMT_TEXTCOLOR)
    {
        *pColor = (*pColor == 0x006d6d6d) ? ((g_IsSysThemeDarkMode) ? RGB(255, 255, 255) : RGB(0, 0, 0)) : *pColor;
        return S_OK;
    }
    else if (ThemeClassName == L"Combobox" && iPropId == TMT_TEXTCOLOR)
    {
        if (iStateId != CBXS_DISABLED)
            *pColor = (g_IsSysThemeDarkMode && *pColor == RGB(0, 0, 0)) ? RGB(255, 255, 255) : *pColor;
        return S_OK;
    }
    
    else if (ThemeClassName == L"Menu" && iPropId == TMT_TEXTCOLOR)
    {
        if (iPartId == MENU_BARITEM && (iStateId != MBI_DISABLED && iStateId != MBI_DISABLEDPUSHED)) {
            *pColor = g_IsSysThemeDarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0);
            return S_OK;
        }
        else if ((iPartId == MENU_POPUPITEM || iPartId == 27) && (iStateId != 3 && iStateId != 4)) {
            *pColor = g_IsSysThemeDarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0);
            return S_OK;
        }
    }
    else if (ThemeClassName == L"Menu" && (iPropId == TMT_FILLCOLOR || iPropId == TMT_FILLCOLORHINT))
    {
        *pColor = (g_settings.FlyoutsEffects) ? RGB(0, 0, 0) : (g_settings.FillBg) ? RGB(32, 32, 32) : *pColor;
        return S_OK;
    }
    else if ((ThemeClassName == L"Toolbar") && iPropId == TMT_TEXTCOLOR)
    {
        if (iPartId == THEMECLS_COMMONPROPS_PART && iStateId != TS_DISABLED) {
            *pColor = (g_IsSysThemeDarkMode) ? RGB(255, 255, 255) : RGB(0, 0, 0);
            return S_OK;
        }
        if (iStateId == TS_DISABLED) {
            *pColor = RGB(128,128,128);
            return S_OK;
        }
    }
    else if (ThemeClassName == L"Tooltip" && iPropId == TMT_TEXTCOLOR)
    {
        if (iPartId== TTP_STANDARD || iPartId == TTP_BALLOON) {
            *pColor = (g_IsSysThemeDarkMode) ? RGB(255, 255, 255) : RGB(0, 0, 0);
            return S_OK;
        }
        else if (iPartId == TTP_BALLOONTITLE) {
            *pColor = (g_IsSysThemeDarkMode) ? RGB(96, 205, 255) : *pColor;
            return hr;        
        }        
    }
    else if (ThemeClassName == L"DragDrop" && iPropId == TMT_TEXTCOLOR)
    {
        *pColor = (iStateId == 1) ? ((g_settings.AccentColorize) ? g_settings.AccentColor : RGB(96, 205, 255)) : RGB(255, 255, 255);
        return S_OK;
    }
    else if (ThemeClassName == L"ChartView")
    {
        if ((iPartId == 29 || iPartId == 31 || iPartId == 32 || iPartId == 33) && iStateId == 1) {
            if (iPropId == TMT_FILLCOLOR)
                *pColor = (g_settings.AccentColorize) ? g_settings.AccentColor : RGB(96,205,255);
            // Instead of the 1st byte of the DWORD/COLORREF variable, the last byte is used as the alpha of the fill color
            else if (iPropId == TMT_ALPHALEVEL)
                *pColor = RGB(255, 0, 0);
            return S_OK;
        }
        if ((iPartId == 34) && iStateId == 1) {
            if (iPropId == TMT_FILLCOLOR)
                *pColor = (g_settings.AccentColorize) ? g_settings.AccentColor : RGB(96,205,255);
            else if (iPropId == TMT_ALPHALEVEL)
                *pColor = RGB(96, 0, 0);
            return S_OK;
        }
    }
    else if (ThemeClassName == L"MonthCal") {
        return hr;
    }
    else if (ThemeClassName == L"AeroWizardStyle" && iPropId == TMT_TEXTCOLOR)
    {
        *pColor = RGB(255, 255, 255);
        return S_OK;
    }
    else if (ThemeClassName == L"ScrollbarStyle" && iPropId == TMT_FILLCOLORHINT)
    {
        *pColor = RGB(96, 96, 96);
        return S_OK;
    }
    else if (ThemeClassName == L"TaskManager")
    {
        switch (iPartId)
        {
            case 2: case 41:
            case 42:
                *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(21, 21, 21) : *pColor;
                break;
            case 3: case 20:
            case 26:
                *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(0, 0, 0) : *pColor;
                break;
            case 4:
                *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(8, 4, 0) : *pColor;
                break;
            case 5:
                if (iPropId == TMT_FILLCOLOR) *pColor = RGB(20, 8, 0);
                else if (iPropId == TMT_TEXTCOLOR) *pColor = RGB(0, 0, 0);
                break;
            case 6:
                if (iPropId == TMT_FILLCOLOR) *pColor = RGB(36, 12, 0);
                else if (iPropId == TMT_TEXTCOLOR) *pColor = RGB(12, 0, 0);
                break;
            case 7:
                if (iPropId == TMT_FILLCOLOR) *pColor = RGB(56, 16, 0);
                else if (iPropId == TMT_TEXTCOLOR) *pColor = RGB(24, 0, 0);
                break;
            case 8:
                if (iPropId == TMT_FILLCOLOR) *pColor = RGB(80, 20, 0);
                else if (iPropId == TMT_TEXTCOLOR) *pColor = RGB(40, 0, 0);
                break;
            case 9:
                if (iPropId == TMT_FILLCOLOR) *pColor = RGB(108, 24, 0);
                else if (iPropId == TMT_TEXTCOLOR) *pColor = RGB(60, 0, 0);
                break;
            case 10:
                if (iPropId == TMT_FILLCOLOR) *pColor = RGB(140, 24, 0);
                else if (iPropId == TMT_TEXTCOLOR) *pColor = RGB(84, 0, 0);
                break;
            case 11:
                if (iPropId == TMT_FILLCOLOR) *pColor = RGB(176, 32, 0);
                else if (iPropId == TMT_TEXTCOLOR) *pColor = RGB(112, 0, 0);
                break;
            case 12:
                if (iPropId == TMT_FILLCOLOR) *pColor = RGB(252, 104, 42);
                else if (iPropId == TMT_TEXTCOLOR) *pColor = RGB(140, 0, 0);
                break;
            case 13:
                *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(241, 112, 122) : *pColor;
                break;
            case 14: case 15:
            case 16: case 17:
            case 18: case 19:
            case 24: case 25:
                *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(255, 255, 255) : *pColor;
                break;
            case 21: *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(97, 113, 186) : *pColor; break;
            case 22: *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(68, 79, 125) : *pColor; break;
            case 23: *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(64, 64, 64) : *pColor; break;
            case 27: *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(32, 36, 44) : *pColor; break;
            case 28: *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(32, 40, 56) : *pColor; break;
            case 29: *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(32, 44, 68) : *pColor; break;
            case 30: *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(32, 48, 80) : *pColor; break;
            case 31: *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(32, 52, 92) : *pColor; break;
            case 32: *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(32, 52, 104) : *pColor; break;
            case 33: *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(32, 60, 116) : *pColor; break;
            case 34: *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(32, 64, 128) : *pColor; break;
            case 35: *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(32, 68, 140) : *pColor; break;
            case 36: *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(32, 72, 152) : *pColor; break;
            case 37: *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(32, 76, 164) : *pColor; break;
            case 38: *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(17, 125, 187) : *pColor; break;
            case 39: *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(34, 38, 55) : *pColor; break;
            case 40: *pColor = (iPropId == TMT_FILLCOLOR) ? RGB(35, 45, 71) : *pColor; break;
        }
        return S_OK;
    }
    else
    {
        if (iPropId == TMT_TEXTCOLOR)
        {
            *pColor = (g_IsSysThemeDarkMode && (*pColor & 0xff000000) == RGB(0, 0, 0)) ? RGB(255, 255, 255) : *pColor;
            return S_OK;
        }
        if (iPropId == TMT_FILLCOLOR)
        {
            *pColor = RGB(0,0,0);
            return S_OK;
        }
        else if (iPropId == TMT_FILLCOLORHINT)
        {
            *pColor = RGB(0,0,0);
            return S_OK;
        }
    }
    
    return hr;
}

HRESULT CreateBoundD2DRenderTarget(HDC hdc, LPCRECT pRect, ID2D1Factory* pFactory, ID2D1DCRenderTarget** ppRenderTarget)
{
    if (!pFactory || !ppRenderTarget)
        return FALSE;

    D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_SOFTWARE,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        NULL,
        NULL,
        D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
        D2D1_FEATURE_LEVEL_DEFAULT
    );

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> renderTarget;
    HRESULT hr = pFactory->CreateDCRenderTarget(&rtProps, &renderTarget);
    if (FAILED(hr)) {
        Wh_Log(L"Failed to create DC target [ERROR]: 0x%08X\n", hr);
        return hr;
    }

    hr = renderTarget->BindDC(hdc, pRect);
    if (FAILED(hr)) {
        Wh_Log(L"Failed to Bind DC target [ERROR]: 0x%08X\n", hr);
        return hr;
    }
    *ppRenderTarget = renderTarget.Detach();
    return S_OK;
}

class CThemeCache
{
public:
    std::array<HDC, 4> pushbutton;
    std::array<HDC, 8> radiobutton;
    std::array<HDC, 20> checkbutton;
    std::array<HDC, 4> commandlinkbutton;
    std::array<HDC, 3> commandlinkglyph;
    std::array<HDC, 14> listview;
    std::array<HDC, 4> scrollbar;
    std::array<HDC, 4> tab;
    std::array<HDC, 8> combobox;
    std::array<HDC, 4> editbox;
    std::array<HDC, 5> treeview;
    std::array<HDC, 8> treeviewglyph;
    std::array<HDC, 6> itemsview;
    std::array<HDC, 10> progressbar;
    std::array<HDC, 2> indeterminatebar;
    std::array<HDC, 2> trackbar;
    std::array<HDC, 24> trackbarthumb;
    std::array<HDC, 2> header;
    std::array<HDC, 1> previewseparator;
    std::array<HDC, 4> modulebutton;
    std::array<HDC, 4> modulelocationbutton;
    std::array<HDC, 8> modulesplitbutton;
    std::array<HDC, 12> navigationbutton;
    std::array<HDC, 1> navigationdivider;
    std::array<HDC, 5> toolbarbutton;
    std::array<HDC, 4> addressband;
    std::array<HDC, 4> menuitem;
    std::array<HDC, 1> dragdrop;
    std::array<HDC, 8> spin;

    BOOL CachePushButton(INT, INT);
    BOOL CacheRadioButton(LPCRECT, INT, INT);
    BOOL CacheCheckButton(LPCRECT, INT, INT);
    BOOL CacheCommandlinkButton(INT, INT);
    BOOL CacheCommandlinkGlyph(INT, INT);
    BOOL CacheListItem(INT, INT, INT);
    BOOL CacheListGroupHeader(INT, INT, INT);
    BOOL CacheScrollbar(INT, INT, INT);
    BOOL CacheScrollArrow(INT, INT);
    BOOL CacheTab(INT, INT);
    BOOL CacheCombobox(INT, INT, INT);
    BOOL CacheEditBox(INT, INT, INT);
    BOOL CacheTreeViewButton(INT, INT, INT);
    BOOL CacheTreeViewGlyph(INT, INT, INT, BOOL);
    BOOL CacheItemsView(INT, INT, INT);
    BOOL CacheProgressBar(INT, INT, INT);
    BOOL CacheIndeterminateBar(INT, INT);
    BOOL CacheTrackBar(INT, INT);
    BOOL CacheTrackBarThumb(INT, INT, INT);
    BOOL CacheTrackBarPointedThumb(INT, INT, INT);
    BOOL CacheHeader(INT, INT);
    BOOL CachePreviewPaneSeparator();
    BOOL CacheModuleButton(INT, INT);
    BOOL CacheModuleLocationButton(INT, INT);
    BOOL CacheModuleSplitButton(INT, INT, INT);
    BOOL CacheNavigationButton(INT, INT, INT);
    BOOL CacheNavigationDivider();
    BOOL CacheToolbarButton(INT, INT);
    BOOL CacheAddressBand(INT, INT);
    BOOL CacheMenuItem(INT, INT, INT);
    BOOL CacheDragDrop();
    BOOL CacheSpinButton(INT, INT, INT);

    BOOL CreateDIB(HDC& elementHdc, INT Width, INT Height)
    {
        if (elementHdc)
            DeleteHDC(elementHdc);

        if (!(elementHdc = CreateCompatibleDC(NULL)))
            return FALSE;

        BITMAPINFO bmi;
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = Width;
        bmi.bmiHeader.biHeight = -Height;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        
        VOID* pvBits;
        HBITMAP hBitmap = CreateDIBSection(elementHdc, &bmi, DIB_RGB_COLORS, &pvBits, nullptr, 0);
        if (!hBitmap)
            return FALSE;
        
        SelectObject(elementHdc, hBitmap);
        return TRUE;
    }

    VOID ClearCache()
    {
        for (HDC& hDC : pushbutton)
            DeleteHDC(hDC);
        for (HDC& hDC : radiobutton)
            DeleteHDC(hDC);
        for (HDC& hDC : checkbutton)
            DeleteHDC(hDC);
        for (HDC& hDC : commandlinkbutton)
            DeleteHDC(hDC);
        for (HDC& hDC : commandlinkglyph)
            DeleteHDC(hDC);
        for (HDC& hDC : listview)
            DeleteHDC(hDC);
        for (HDC& hDC : scrollbar)
            DeleteHDC(hDC);
        for (HDC& hDC : tab)
            DeleteHDC(hDC);
        for (HDC& hDC : combobox)
            DeleteHDC(hDC);
        for (HDC& hDC : editbox)
            DeleteHDC(hDC);
        for (HDC& hDC : treeview)
            DeleteHDC(hDC);
        for (HDC& hDC : treeviewglyph)
            DeleteHDC(hDC);
        for (HDC& hDC : itemsview)
            DeleteHDC(hDC);
        for (HDC& hDC : progressbar)
            DeleteHDC(hDC);
        for (HDC& hDC : indeterminatebar)
            DeleteHDC(hDC);
        for (HDC& hDC : trackbar)
            DeleteHDC(hDC);
        for (HDC& hDC : trackbarthumb)
            DeleteHDC(hDC);
        for (HDC& hDC : header)
            DeleteHDC(hDC);
        for (HDC& hDC : previewseparator)
            DeleteHDC(hDC);
        for (HDC& hDC : modulebutton)
            DeleteHDC(hDC);
        for (HDC& hDC : modulelocationbutton)
            DeleteHDC(hDC);
        for (HDC& hDC : modulesplitbutton)
            DeleteHDC(hDC);
        for (HDC& hDC : navigationbutton)
            DeleteHDC(hDC);
        for (HDC& hDC : navigationdivider)
            DeleteHDC(hDC);
        for (HDC& hDC : toolbarbutton)
            DeleteHDC(hDC);
        for (HDC& hDC : addressband)
            DeleteHDC(hDC);
        for (HDC& hDC : menuitem)
            DeleteHDC(hDC);
        for (HDC& hDC : dragdrop)
            DeleteHDC(hDC);
        for (HDC& hDC : spin)
            DeleteHDC(hDC);
    }

    VOID DeleteHDC(HDC& hDC)
    {
        if (hDC) {
            DeleteObject((HBITMAP)GetCurrentObject(hDC, OBJ_BITMAP));
            DeleteDC(std::exchange(hDC, nullptr));
        }
    }

    ~CThemeCache()
    {
        ClearCache();
    }
};
CThemeCache g_themeCache;

VOID DrawNineGridStretch(HDC hdc, HDC& srcDC, LPCRECT dstRect, INT left = 0, INT top = 0, INT right = 0, INT bottom = 0)
{
    if (!hdc || !srcDC)
        return;
    
    HBITMAP hBmp = (HBITMAP)GetCurrentObject(srcDC, OBJ_BITMAP);
    BITMAP bmp = {};
    GetObject(hBmp, sizeof(bmp), &bmp);

    INT srcW = bmp.bmWidth;
    INT srcH = bmp.bmHeight;
    INT dstW = dstRect->right - dstRect->left;
    INT dstH = dstRect->bottom - dstRect->top;

    left   = std::min(left, dstW);
    right  = std::min(right, dstW - left);
    top    = std::min(top, dstH);
    bottom = std::min(bottom, dstH - top);

    INT centerW = dstW - left - right;
    INT centerH = dstH - top - bottom;

    INT srcCenterW = srcW - left - right;
    INT srcCenterH = srcH - top - bottom;

    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

    // Full stretch
    if (left + right >= srcW || top + bottom >= srcH)
    {
        AlphaBlend(hdc, dstRect->left, dstRect->top, dstW, dstH,
                srcDC, 0, 0, srcW, srcH, blend);
        return;
    }
    // Short-circuit if the entire region is fully covered by the top-left corner
    if (dstW <= left && dstH <= top)
    {
        AlphaBlend(hdc, dstRect->left, dstRect->top, dstW, dstH,
                   srcDC, 0, 0, dstW, dstH, blend);
        return;
    }
    // Center
    if (centerW > 0 && centerH > 0 && srcCenterW > 0 && srcCenterH > 0)
    {
        AlphaBlend(hdc, dstRect->left + left, dstRect->top + top, centerW, centerH,
                   srcDC, left, top, srcCenterW, srcCenterH, blend);
    }
    // Top-left
    if (left > 0 && top > 0)
    {
        AlphaBlend(hdc, dstRect->left, dstRect->top, left, top,
                   srcDC, 0, 0, left, top, blend);
    }
    // Top
    if (centerW > 0 && top > 0 && srcCenterW > 0)
    {
        AlphaBlend(hdc, dstRect->left + left, dstRect->top, centerW, top,
                   srcDC, left, 0, srcCenterW, top, blend);
    }
    // Top-right
    if (right > 0 && top > 0)
    {
        AlphaBlend(hdc, dstRect->right - right, dstRect->top, right, top,
                   srcDC, srcW - right, 0, right, top, blend);
    }
    // Left
    if (left > 0 && centerH > 0 && srcCenterH > 0)
    {
        AlphaBlend(hdc, dstRect->left, dstRect->top + top, left, centerH,
                   srcDC, 0, top, left, srcCenterH, blend);
    }
    // Right
    if (right > 0 && centerH > 0 && srcCenterH > 0)
    {
        AlphaBlend(hdc, dstRect->right - right, dstRect->top + top, right, centerH,
                   srcDC, srcW - right, top, right, srcCenterH, blend);
    }
    // Bottom-left
    if (left > 0 && bottom > 0)
    {
        AlphaBlend(hdc, dstRect->left, dstRect->bottom - bottom, left, bottom,
                   srcDC, 0, srcH - bottom, left, bottom, blend);
    }
    // Bottom
    if (centerW > 0 && bottom > 0 && srcCenterW > 0)
    {
        AlphaBlend(hdc, dstRect->left + left, dstRect->bottom - bottom, centerW, bottom,
                   srcDC, left, srcH - bottom, srcCenterW, bottom, blend);
    }
    // Bottom-right
    if (right > 0 && bottom > 0)
    {
        AlphaBlend(hdc, dstRect->right - right, dstRect->bottom - bottom, right, bottom,
                   srcDC, srcW - right, srcH - bottom, right, bottom, blend);
    }
}

BOOL PaintScroll(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if ((iPartId == SBP_UPPERTRACKVERT || iPartId == SBP_LOWERTRACKVERT
    || iPartId == SBP_UPPERTRACKHORZ || iPartId == SBP_LOWERTRACKHORZ))
        return TRUE;
    if ((!g_d2dFactory ||(iPartId != SBP_THUMBBTNVERT && iPartId != SBP_THUMBBTNHORZ)))
        return FALSE;
    
    INT index = (iStateId == SCRBS_NORMAL) ? 0 : 1;
    if (iPartId == SBP_THUMBBTNHORZ) index += 2;

    if (!g_themeCache.scrollbar[index])
        if (!g_themeCache.CacheScrollbar(iPartId, iStateId, index))
            return FALSE;

    FillRect(hdc, pRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
    DrawNineGridStretch(hdc, g_themeCache.scrollbar[index], pRect, 8, 5, 8, 5);
    return TRUE;
}

BOOL CThemeCache::CacheScrollbar(INT iPartId, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    INT width = 17 * scale, height = 11 * scale;
    if (iPartId == SBP_THUMBBTNHORZ)
        width = 20 * scale, height = 17 * scale;
    FLOAT cornerRadius = 4.f * scale;

    if (!g_themeCache.CreateDIB(g_themeCache.scrollbar[stateIndex], width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.scrollbar[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    D2D1_RECT_F Rect;
    if (iPartId == SBP_THUMBBTNVERT && iStateId == SCRBS_NORMAL)
        Rect = D2D1::RectF(width*0.35, 0, width-width*0.35, height);
    else if (iPartId == SBP_THUMBBTNHORZ && iStateId == SCRBS_NORMAL)
        Rect = D2D1::RectF(0, height*0.35, width, height-height*0.35);
    else if (iPartId == SBP_THUMBBTNVERT)
        Rect = D2D1::RectF(width*0.25, 0, width-width*0.25, height);
    else if (iPartId == SBP_THUMBBTNHORZ)
        Rect = D2D1::RectF(0, height*0.25, width, height-height*0.25);
    
    D2D1_COLOR_F Color = (iStateId == SCRBS_NORMAL) ? MyD2D1Color(128, 160, 160, 160) : MyD2D1Color(160, 224, 224, 224);

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush = nullptr;
    pRenderTarget->CreateSolidColorBrush(Color, &brush);
    D2D1_ROUNDED_RECT rr = {Rect, cornerRadius, cornerRadius};

    pRenderTarget->BeginDraw();
    pRenderTarget->FillRoundedRectangle(&rr, brush.Get());
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintScrollBarArrows(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (iPartId != SBP_ARROWBTN || !g_d2dFactory)
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> dcRenderTarget = nullptr;
    if (FAILED(CreateBoundD2DRenderTarget(hdc, pRect, g_d2dFactory, &dcRenderTarget)))
        return FALSE;

    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT width = static_cast<FLOAT>RECTWIDTH(pRect);
    FLOAT height = static_cast<FLOAT>RECTHEIGHT(pRect);

    FLOAT triangleBaseWidth = 7.0f * scale;
    FLOAT triangleHeight = 4.5f * scale;
    FLOAT centerX = width / 2.0f;
    FLOAT centerY = height / 2.0f;

    D2D1_COLOR_F arrowColor;
    if (iStateId == 2 || iStateId == 6 || iStateId == 10 || iStateId == 14) {
        triangleBaseWidth = 8.0f * scale;
        triangleHeight = 5.5f * scale;
        arrowColor = MyD2D1Color(192, 224, 224, 224);
    }
    else if (iStateId == 4 || iStateId == 8 || iStateId == 12 || iStateId == 16)
        arrowColor = MyD2D1Color(192, 64, 64, 64);
    else 
        arrowColor = MyD2D1Color(128, 160, 160, 160);

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush = nullptr;
    dcRenderTarget->CreateSolidColorBrush(arrowColor, &brush);
    D2D1_POINT_2F points[6] = {};
    if ((iStateId > ABS_UPNORMAL && iStateId <= ABS_UPDISABLED) || iStateId == ABS_UPHOVER)
    {
        points[0] = D2D1::Point2F(centerX-1 - triangleBaseWidth / 2.0f, centerY + triangleHeight / 2.0f);
        points[1] = D2D1::Point2F(centerX-1 - triangleBaseWidth / 2.0f, centerY+2 + triangleHeight / 2.0f);
        points[2] = D2D1::Point2F(centerX + triangleBaseWidth / 2.0f, centerY+2 + triangleHeight / 2.0f);
        points[3] = D2D1::Point2F(centerX + triangleBaseWidth / 2.0f, centerY + triangleHeight / 2.0f);
        points[4] = D2D1::Point2F(centerX, centerY - triangleHeight / 2.0f);
        points[5] = D2D1::Point2F(centerX-1, centerY - triangleHeight / 2.0f);
    }
    else if ((iStateId > ABS_DOWNNORMAL && iStateId <= ABS_DOWNDISABLED) || iStateId == ABS_DOWNHOVER)
    {
        points[0] = D2D1::Point2F(centerX-1 - triangleBaseWidth / 2.0f, centerY - triangleHeight / 2.0f);
        points[1] = D2D1::Point2F(centerX-1 - triangleBaseWidth / 2.0f, centerY-2 - triangleHeight / 2.0f);
        points[2] = D2D1::Point2F(centerX + triangleBaseWidth / 2.0f, centerY-2 - triangleHeight / 2.0f);
        points[3] = D2D1::Point2F(centerX + triangleBaseWidth / 2.0f, centerY - triangleHeight / 2.0f);
        points[4] = D2D1::Point2F(centerX, centerY + triangleHeight / 2.0f);
        points[5] = D2D1::Point2F(centerX-1, centerY + triangleHeight / 2.0f);
    }
    else if ((iStateId > ABS_LEFTNORMAL && iStateId <= ABS_LEFTDISABLED) || iStateId == ABS_LEFTHOVER)
    {
        points[0] = D2D1::Point2F(centerX + triangleHeight / 2.0f, centerY - 1 - triangleBaseWidth / 2.0f);
        points[1] = D2D1::Point2F(centerX+2 + triangleHeight / 2.0f, centerY - 1 - triangleBaseWidth / 2.0f);
        points[2] = D2D1::Point2F(centerX+2 + triangleHeight / 2.0f, centerY + triangleBaseWidth / 2.0f);
        points[3] = D2D1::Point2F(centerX + triangleHeight / 2.0f, centerY + triangleBaseWidth / 2.0f);
        points[4] = D2D1::Point2F(centerX - triangleHeight / 2.0f, centerY);
        points[5] = D2D1::Point2F(centerX - triangleHeight / 2.0f, centerY-1);
    }
    else if ((iStateId > ABS_RIGHTNORMAL && iStateId <= ABS_RIGHTDISABLED) || iStateId == ABS_RIGHTHOVER)
    {
        points[0] = D2D1::Point2F(centerX - triangleHeight / 2.0f, centerY-1 - triangleBaseWidth / 2.0f);
        points[1] = D2D1::Point2F(centerX-2 - triangleHeight / 2.0f, centerY-1 - triangleBaseWidth / 2.0f);
        points[2] = D2D1::Point2F(centerX-2 - triangleHeight / 2.0f, centerY + triangleBaseWidth / 2.0f);
        points[3] = D2D1::Point2F(centerX - triangleHeight / 2.0f, centerY + triangleBaseWidth / 2.0f);
        points[4] = D2D1::Point2F(centerX + triangleHeight / 2.0f, centerY);
        points[5] = D2D1::Point2F(centerX + triangleHeight / 2.0f, centerY-1);
    }

    FillRect(hdc, pRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

    dcRenderTarget->BeginDraw();

    Microsoft::WRL::ComPtr<ID2D1PathGeometry> triangleGeo = nullptr;
    if (SUCCEEDED(g_d2dFactory->CreatePathGeometry(&triangleGeo)))
    {
        Microsoft::WRL::ComPtr<ID2D1GeometrySink> sink = nullptr;
        if (SUCCEEDED(triangleGeo->Open(&sink)))
        {
            sink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
            sink->AddLine(points[1]);
            sink->AddLine(points[2]);
            sink->AddLine(points[3]);
            sink->AddLine(points[4]);
            sink->AddLine(points[5]);
            sink->EndFigure(D2D1_FIGURE_END_CLOSED);
            sink->Close();

            dcRenderTarget->FillGeometry(triangleGeo.Get(), brush.Get());
        }
    }
    auto hr = dcRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintPushButton(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect, LPCRECT pClipRect)
{
    if (iPartId != BP_PUSHBUTTON || !g_d2dFactory)
        return FALSE;
    
    RECT clipRect{ *pRect };
    if (pClipRect)
        IntersectRect(&clipRect, &clipRect, pClipRect);

    INT index = (iStateId == PBS_HOT) ? 1 : (iStateId == PBS_PRESSED) ? 2
    : (iStateId == PBS_DISABLED) ? 3 : 0;

    if (!g_themeCache.pushbutton[index])
        if (!g_themeCache.CachePushButton(iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_themeCache.pushbutton[index], &clipRect, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CachePushButton(INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    INT width = 18, height = 18;
    FLOAT cornerRadius = 3.f * scale;

    if (!g_themeCache.CreateDIB(g_themeCache.pushbutton[stateIndex], width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.pushbutton[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    D2D1_ROUNDED_RECT rr = {
        D2D1::RectF(0.5f, 0.5f, (FLOAT)width - 0.5f, (FLOAT)height - 0.5f),
        cornerRadius, cornerRadius
    };

    D2D1_COLOR_F fillColor =
        (iStateId == PBS_HOT)      ? MyD2D1Color(128, 96, 96, 96) :
        (iStateId == PBS_PRESSED)  ? MyD2D1Color(180, 60, 60, 60)  :
        (iStateId == PBS_DISABLED) ? MyD2D1Color(64, 64, 64, 64)  :
                                     MyD2D1Color(96, 80, 80, 80);

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> fillBrush;
    pRenderTarget->CreateSolidColorBrush(fillColor, &fillBrush);
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> borderBrush;
    pRenderTarget->CreateSolidColorBrush(MyD2D1Color(96, 112, 112, 112), &borderBrush);
    
    pRenderTarget->BeginDraw();
    pRenderTarget->FillRoundedRectangle(&rr, fillBrush.Get());
    pRenderTarget->DrawRoundedRectangle(&rr, borderBrush.Get(), scale);
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintRadioButton(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (iPartId != BP_RADIOBUTTON || !g_d2dFactory)
        return FALSE;
    
    INT index = iStateId - 1;

    if (!g_themeCache.radiobutton[index])
        if (!g_themeCache.CacheRadioButton(pRect, iStateId, index))
            return FALSE;
    // Some theme parts are always fixed size so no stretching is needed
    DrawNineGridStretch(hdc, g_themeCache.radiobutton[index], pRect);
    return TRUE;
}

BOOL CThemeCache::CacheRadioButton(LPCRECT pRect,  INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT width = RECTWIDTH(pRect), height = RECTHEIGHT(pRect);
    if (!g_themeCache.CreateDIB(g_themeCache.radiobutton[stateIndex], width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, (INT)width, (INT)height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.radiobutton[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    FLOAT diameter = width - 1.f;
    FLOAT x = 0.5f, y = 0.5f;

    D2D1_ELLIPSE outerEllipse = D2D1::Ellipse(
        D2D1::Point2F(x + diameter / 2.f, y + diameter / 2.f),
        diameter / 2.f, diameter / 2.f
    );

    D2D1_COLOR_F borderColor, radioColor;
    D2D1_COLOR_F innerColor = MyD2D1Color(0, 0, 0);
    FLOAT innerRatio = 0.0f;

    switch (iStateId)
    {
        case CBS_UNCHECKEDNORMAL:
            borderColor = MyD2D1Color(96, 128, 128, 128);
            radioColor = MyD2D1Color(64, 64, 64, 64);
            break;
        case RBS_UNCHECKEDHOT:
            borderColor = MyD2D1Color(144, 144, 144);
            radioColor = MyD2D1Color(48, 144, 144, 144);
            break;
        case RBS_UNCHECKEDPRESSED:
            radioColor = MyD2D1Color(64, 64, 64);
            innerRatio = 0.3f;
            break;
        case RBS_UNCHECKEDDISABLED:
            borderColor = MyD2D1Color(64, 128, 128, 128);
            break;
        case RBS_CHECKEDNORMAL:
            borderColor = radioColor = IsAccentColorPossibleD2D(105, 205, 255, SystemAccentColorLight2);
            innerRatio = 0.4f;
            break;
        case RBS_CHECKEDHOT:
            borderColor = radioColor = IsAccentColorPossibleD2D(225, 105, 205, 255, SystemAccentColorLight3);
            innerRatio = 0.6f;
            break;
        case RBS_CHECKEDPRESSED:
            borderColor = radioColor = IsAccentColorPossibleD2D(192, 105, 205, 255, SystemAccentColorLight1);
            innerRatio = 0.33f;
            break;
        case RBS_CHECKEDDISABLED:
            borderColor = radioColor = MyD2D1Color(96, 96, 96);
            innerRatio = 0.3f;
            break;
    }

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush = nullptr;
    pRenderTarget->CreateSolidColorBrush(radioColor, &brush);

    pRenderTarget->BeginDraw();
    pRenderTarget->FillEllipse(outerEllipse, brush.Get());
    brush->SetColor(borderColor);
    pRenderTarget->DrawEllipse(outerEllipse, brush.Get(), scale);

    if (innerRatio > 0.f)
    {
        FLOAT innerDiameter = diameter * innerRatio;
        D2D1_ELLIPSE innerEllipse = D2D1::Ellipse(
            D2D1::Point2F(x + diameter / 2.f, y + diameter / 2.f),
            innerDiameter / 2.f, innerDiameter / 2.f
        );

        pRenderTarget->CreateSolidColorBrush(innerColor, &brush);
        pRenderTarget->FillEllipse(innerEllipse, brush.Get());
    }
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintCheckBox(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (iPartId != BP_CHECKBOX || !g_d2dFactory)
        return FALSE;
    
    INT index = iStateId - 1;

    if (!g_themeCache.checkbutton[index])
        if (!g_themeCache.CacheCheckButton(pRect, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_themeCache.checkbutton[index], pRect);
    return TRUE;
}

BOOL CThemeCache::CacheCheckButton(LPCRECT pRect, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT width = RECTWIDTH(pRect), height = RECTHEIGHT(pRect);
    FLOAT cornerRadius = 3.f * scale;

    if (!g_themeCache.CreateDIB(g_themeCache.checkbutton[stateIndex], width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, (INT)width, (INT)height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.checkbutton[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;
    
    D2D1_ROUNDED_RECT roundedRect = {
        D2D1::RectF(0, 0, width, height),
        cornerRadius, cornerRadius
    };

    D2D1_COLOR_F fillColor, borderColor;
    switch (iStateId) 
    {
        case CBS_UNCHECKEDNORMAL:
            borderColor = MyD2D1Color(96, 128, 128, 128);
            fillColor = MyD2D1Color(64, 96, 96, 96);
            break;
        case CBS_UNCHECKEDHOT:
            borderColor = MyD2D1Color(144, 144, 144);
            fillColor = MyD2D1Color(48, 144, 144, 144);
            break;
        case CBS_UNCHECKEDPRESSED:
            borderColor = MyD2D1Color(96, 144, 144, 144);
            fillColor = MyD2D1Color(48, 144, 144, 144);
            break;
        case CBS_UNCHECKEDDISABLED:
            borderColor = MyD2D1Color(64, 144, 144, 144);
            fillColor = MyD2D1Color(64, 128, 128, 128);
            break;
        case CBS_CHECKEDNORMAL: case CBS_MIXEDNORMAL:
        case CBS_IMPLICITNORMAL: case CBS_EXCLUDEDNORMAL:
            fillColor = IsAccentColorPossibleD2D(102, 206, 255, SystemAccentColorLight2);
            break;
        case CBS_CHECKEDHOT: case CBS_MIXEDHOT:
        case CBS_IMPLICITHOT: case CBS_EXCLUDEDHOT:
            fillColor = IsAccentColorPossibleD2D(224, 102, 206, 255, SystemAccentColorLight3);
            break;
        case CBS_CHECKEDPRESSED: case CBS_MIXEDPRESSED:
        case CBS_IMPLICITPRESSED: case CBS_EXCLUDEDPRESSED:
            fillColor = IsAccentColorPossibleD2D(192, 102, 206, 255, SystemAccentColorLight1);
            break;
        case CBS_CHECKEDDISABLED: case CBS_MIXEDDISABLED:
        case CBS_IMPLICITDISABLED: case CBS_EXCLUDEDDISABLED:
            fillColor = MyD2D1Color(96, 96, 96);
    }
    pRenderTarget->BeginDraw();

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> Brush = nullptr;
    pRenderTarget->CreateSolidColorBrush(fillColor, &Brush);
    pRenderTarget->FillRoundedRectangle(&roundedRect, Brush.Get());

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> glyphBrush = nullptr;
    pRenderTarget->CreateSolidColorBrush(MyD2D1Color(0, 0, 0), &glyphBrush);

    if (iStateId >= CBS_UNCHECKEDNORMAL && iStateId <= CBS_UNCHECKEDDISABLED)
    {
        Brush->SetColor(borderColor);
        pRenderTarget->DrawRoundedRectangle
        (D2D1_ROUNDED_RECT(D2D1::RectF(.5f, .5f, width - .5f, height - .5f), cornerRadius, cornerRadius), Brush.Get(), scale);
    }
    if (iStateId > CBS_UNCHECKEDDISABLED)
    {       
        if ((iStateId >= CBS_CHECKEDNORMAL && iStateId <= CBS_CHECKEDDISABLED) ||
            (iStateId >= CBS_IMPLICITNORMAL && iStateId <= CBS_IMPLICITDISABLED)) // Checkmark
        {
            FLOAT centerX = width/2.f - 2*scale;
            FLOAT centerY = height/2.f + 2.5*scale;
            FLOAT rightLen = width *.65f ;
            FLOAT leftLen  = width *.3f;

            DOUBLE dxyR = rightLen * 0.7071067;

            DOUBLE dxL = leftLen * 0.5;
            DOUBLE dyL = leftLen * 0.8660254;

            D2D1_POINT_2F ptTip   = D2D1::Point2F(centerX, centerY);
            D2D1_POINT_2F ptLeft  = D2D1::Point2F(ptTip.x - dxL, ptTip.y - dyL);
            D2D1_POINT_2F ptRight = D2D1::Point2F(ptTip.x + dxyR, ptTip.y - dxyR);

            pRenderTarget->DrawLine(ptLeft, ptTip, glyphBrush.Get(), scale * 1.2f);
            pRenderTarget->DrawLine(ptTip, ptRight, glyphBrush.Get(), scale * 1.2f);
        }
        if (iStateId >= CBS_EXCLUDEDNORMAL && iStateId <= CBS_EXCLUDEDDISABLED) // X
        {
            pRenderTarget->DrawLine((D2D1::Point2F(width *.3f, height/3.f)), (D2D1::Point2F(width *.7f, height/1.5f)), glyphBrush.Get());
            pRenderTarget->DrawLine((D2D1::Point2F(width *.3f, height/1.5f)), (D2D1::Point2F(width *.7f, height/3.f)), glyphBrush.Get()); 
        }
        if (iStateId >= CBS_MIXEDNORMAL && iStateId <= CBS_MIXEDDISABLED) // Minus
            pRenderTarget->DrawLine((D2D1::Point2F(width *.3f, height/2.f)), (D2D1::Point2F(width *.7f, height/2.f)), glyphBrush.Get());        
    }
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintGroupBox(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect, LPCRECT pClippedRect)
{
    if (!g_d2dFactory)
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    if (FAILED(CreateBoundD2DRenderTarget(hdc, pRect, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    const FLOAT radius = 4.0f;
    const FLOAT x = 0.5f;
    const FLOAT y = 0.5f;
    const FLOAT width = static_cast<FLOAT>RECTWIDTH(pRect) - 0.5f;
    const FLOAT h = static_cast<FLOAT>RECTHEIGHT(pRect) - 0.5f;

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
    pRenderTarget->CreateSolidColorBrush(MyD2D1Color(96, 96, 96), &brush);

    pRenderTarget->BeginDraw();

    Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry;
    Microsoft::WRL::ComPtr<ID2D1GeometrySink> sink;
    g_d2dFactory->CreatePathGeometry(&geometry);
    geometry->Open(&sink);
    
    if (!pClippedRect) // Top line if label does clip it
    {
        sink->BeginFigure(D2D1::Point2F(radius, y), D2D1_FIGURE_BEGIN_HOLLOW);
        sink->AddLine(D2D1::Point2F(width - radius, y));
    }
    else
        sink->BeginFigure(D2D1::Point2F(width - radius, y), D2D1_FIGURE_BEGIN_HOLLOW);

    sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(width, radius), D2D1::SizeF(radius, radius), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
    sink->AddLine(D2D1::Point2F(width, h - radius));
    sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(width - radius, h), D2D1::SizeF(radius, radius), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
    sink->AddLine(D2D1::Point2F(radius, h));
    sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(x, h - radius), D2D1::SizeF(radius, radius), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
    sink->AddLine(D2D1::Point2F(x, radius));
    sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(radius + 1.f, y), D2D1::SizeF(radius, radius), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
    sink->EndFigure(D2D1_FIGURE_END_OPEN);
    if (pClippedRect && (FLOAT)(pClippedRect->top) == (FLOAT)pRect->top) 
    {
        // Clipped rect sides
        const FLOAT cx = static_cast<FLOAT>(pClippedRect->left) + radius - .5f;
        const FLOAT cx2 = static_cast<FLOAT>(pClippedRect->right) - radius;
        // Top line right side of the label
        pRenderTarget->DrawLine(
            D2D1::Point2F(cx, .5f),
            D2D1::Point2F(cx2, .5f),
            brush.Get()
        );
    }
    sink->Close();
    pRenderTarget->DrawGeometry(geometry.Get(), brush.Get());
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}

    return TRUE;
}

BOOL PaintCommandLink(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (iPartId != BP_COMMANDLINK || !g_d2dFactory)
        return FALSE;
    
    INT index = (iStateId == CMDLS_NORMAL || iStateId == CMDLS_DISABLED) ? 0 : (iStateId == CMDLS_HOT) ? 1
    : (iStateId == CMDLS_PRESSED) ? 2 : 3;

    if (!g_themeCache.commandlinkbutton[index])
        if (!g_themeCache.CacheCommandlinkButton(iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_themeCache.commandlinkbutton[index], pRect, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheCommandlinkButton(INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    INT width = 18, height = 18;
    FLOAT cornerRadius = 4.f * scale;

    if (!g_themeCache.CreateDIB(g_themeCache.commandlinkbutton[stateIndex], width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.commandlinkbutton[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;
    
    D2D1_ROUNDED_RECT roundedRect = { D2D1::RectF(0, 0, width, height), cornerRadius, cornerRadius};
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
    pRenderTarget->BeginDraw();
    switch (iStateId)
    {
        case CMDLS_NORMAL:
        case CMDLS_DISABLED:
            pRenderTarget->CreateSolidColorBrush(MyD2D1Color(0, 0, 0, 0), &brush);
            pRenderTarget->FillRoundedRectangle(&roundedRect, brush.Get());
            break;
        case CMDLS_HOT:
            pRenderTarget->CreateSolidColorBrush(MyD2D1Color(96, 144, 144, 144), &brush);
            pRenderTarget->FillRoundedRectangle(&roundedRect, brush.Get());
            break;
        case CMDLS_PRESSED:
            pRenderTarget->CreateSolidColorBrush(MyD2D1Color(64, 144, 144, 144), &brush);
            pRenderTarget->FillRoundedRectangle(&roundedRect, brush.Get());
            break;
        case CMDLS_DEFAULTED:
        case CMDLS_DEFAULTED_ANIMATING:
            roundedRect = {D2D1::RectF(1.f * scale, 1.f * scale, width - 1.f * scale, height - 1.f * scale), cornerRadius - 1.f, cornerRadius - 1.f};
            pRenderTarget->CreateSolidColorBrush(MyD2D1Color(255, 255, 255), &brush);
            pRenderTarget->DrawRoundedRectangle(&roundedRect, brush.Get(), 2.f * scale);
            break;
    }
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintCommandLinkGlyph(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (iPartId != BP_COMMANDLINKGLYPH || !g_d2dFactory)
        return FALSE;
    
    INT index = (iStateId == CMDLGS_HOT) ? 1 : (iStateId == CMDLGS_PRESSED) ? 2
    : (iStateId == CMDLGS_DISABLED) ? 3 : 0;

    if (!g_themeCache.commandlinkglyph[index])
        if (!g_themeCache.CacheCommandlinkGlyph(iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_themeCache.commandlinkglyph[index], pRect);
    return TRUE;
}

BOOL CThemeCache::CacheCommandlinkGlyph(INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    INT x = 0;
    INT width = 20 * scale, height = 20 * scale;
    if (!g_themeCache.CreateDIB(g_themeCache.commandlinkglyph[stateIndex], width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.commandlinkglyph[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;
    
    FLOAT tailScale = 1.f;
    D2D1_COLOR_F arrowColor = MyD2D1Color(192, 192, 192);
    if (iStateId == CMDLGS_HOT || iStateId == CMDLGS_PRESSED) {
        arrowColor = MyD2D1Color(255, 255, 255);
        if (iStateId == CMDLGS_PRESSED)
            tailScale = 0.8f;
    }
    else if (iStateId == CMDLGS_DISABLED)
        arrowColor = MyD2D1Color(160, 160, 160);

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
    pRenderTarget->CreateSolidColorBrush(arrowColor, &brush);

    FLOAT centerY = height / 2.f;
    FLOAT tailLength = width * tailScale;
    FLOAT tailStartX = x;
    FLOAT tailEndX = tailStartX + tailLength;

    FLOAT headSpan = tailLength * 0.4f;
    FLOAT headOffset = headSpan * 0.7071f; // 45 degrees

    pRenderTarget->BeginDraw();
    pRenderTarget->DrawLine(
    D2D1::Point2F(x, centerY),
    D2D1::Point2F(tailLength, centerY),
    brush.Get(), 1.f
    );
    pRenderTarget->DrawLine(
        D2D1::Point2F(tailEndX - headOffset, centerY - headOffset),
        D2D1::Point2F(tailEndX, centerY),
        brush.Get(), 1.f
    );
    pRenderTarget->DrawLine(
        D2D1::Point2F(tailEndX - headOffset, centerY + headOffset),
        D2D1::Point2F(tailEndX, centerY),
        brush.Get()
    );  
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL SanitizeAddressCombobox(HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId)
{
    HTHEME hThemeAddressCB = nullptr;
    if (SetThemeHandle(WindowFromDC(hdc), hThemeAddressCB, L"AddressComposited::ComboBox")
    && (iPartId == CP_BORDER || iPartId == CP_TRANSPARENTBACKGROUND))
    {
        CloseThemeData(hThemeAddressCB);
        return TRUE;
    }
    return FALSE;
}

BOOL PaintCombobox(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory || (iPartId != CP_READONLY && iPartId != CP_BORDER))
        return FALSE;
    
    INT index = (iPartId == CP_READONLY) ? iStateId - 1 : iStateId + 3;
    
    if (!g_themeCache.combobox[index])
        if (!g_themeCache.CacheCombobox(iPartId, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_themeCache.combobox[index], pRect, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheCombobox(INT iPartId, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 3.f * scale;
    INT width = 18, height = 18;

    if (!g_themeCache.CreateDIB(g_themeCache.combobox[stateIndex], width, height))
        return FALSE;
    
    // Direct2D render target
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.combobox[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    D2D1_ROUNDED_RECT roundedRect = {D2D1::RectF(0.5, 0.5, width - .5f, height - .5f), cornerRadius, cornerRadius};

    pRenderTarget->BeginDraw();
    if (iPartId == CP_READONLY)
    {
        D2D1_COLOR_F fillColor = (iStateId == PBS_HOT)      ? MyD2D1Color(128, 96, 96, 96) : 
                                 (iStateId == PBS_PRESSED)  ? MyD2D1Color(180, 60, 60, 60) :
                                 (iStateId == PBS_DISABLED) ? MyD2D1Color(64, 64, 64, 64) :
                                                              MyD2D1Color(96, 80, 80, 80);
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> Brush;
        pRenderTarget->CreateSolidColorBrush(fillColor, &Brush);
        pRenderTarget->FillRoundedRectangle(&roundedRect, Brush.Get());

        Brush->SetColor(MyD2D1Color(96, 112, 112, 112));
        pRenderTarget->DrawRoundedRectangle(&roundedRect, Brush.Get(), scale);
    }
    else if (iPartId == CP_BORDER)
    {
        D2D1_COLOR_F borderColor = (iStateId == CBXS_HOT) ? MyD2D1Color(128, 160, 160, 160) : MyD2D1Color(96, 128, 128, 128);
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> borderBrush;
        pRenderTarget->CreateSolidColorBrush(borderColor, &borderBrush);
        pRenderTarget->DrawRoundedRectangle(&roundedRect, borderBrush.Get(), scale);

        if (iStateId == CBXS_PRESSED) 
        {
            borderBrush->SetColor(IsAccentColorPossibleD2D(105, 205, 255, SystemAccentColorLight2));
            pRenderTarget->DrawLine(
                D2D1::Point2F(cornerRadius/2 - 1.f * scale, height - 1.5f),
                D2D1::Point2F(width - cornerRadius/2 + 1.f *scale, height - 1.5f),
                borderBrush.Get()
            );
            pRenderTarget->DrawLine(
                D2D1::Point2F(2.f * scale, height - .5f),
                D2D1::Point2F(width - 2.f * scale, height - .5f),
                borderBrush.Get()
            );
        }
        else if (iStateId == CBXS_DISABLED)
        {
            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> Brush;
            pRenderTarget->CreateSolidColorBrush(MyD2D1Color(96, 80, 80, 80), &Brush);
            pRenderTarget->FillRoundedRectangle(&roundedRect, Brush.Get());
        }
    }
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL IsAddressInnerBackground(HTHEME hTheme, HDC hdc, INT iPartId)
{
    HTHEME hThemeAddress = NULL;
    if (SetThemeHandle(WindowFromDC(hdc), hThemeAddress, L"AddressComposited::Edit") && iPartId == EP_BACKGROUNDWITHBORDER)
    {
        CloseThemeData(hThemeAddress);
        return TRUE;
    }
    return FALSE;
}

BOOL PaintEditBox(HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory || (iPartId != EP_EDITBORDER_NOSCROLL && iPartId != EP_EDITBORDER_HSCROLL
    && iPartId != EP_EDITBORDER_VSCROLL && iPartId != EP_EDITBORDER_HVSCROLL && iPartId != EP_BACKGROUND
    && (!IsAddressInnerBackground(hTheme, hdc, iPartId))
    ))
        return FALSE;

    // Remove editbox white background flashing
    if (iPartId ==  EP_BACKGROUND) {
        FillRect(hdc, pRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        return TRUE;
    }
    INT index = (iPartId == EP_BACKGROUNDWITHBORDER) ? 3 : (iStateId == 1) ? 0 : iStateId - 2;

    if (!g_themeCache.editbox[index])
        if (!g_themeCache.CacheEditBox(iPartId, iStateId, index))
            return FALSE;
    // hide the borders of the inner black background of EP_BACKGROUNDWITHBORDER theme class by expanding the black drawing.
    RECT rc = (iPartId == EP_BACKGROUNDWITHBORDER) ? RECT{pRect->left-1, pRect->top-1, pRect->right+3,pRect->bottom+1} : *pRect;
    DrawNineGridStretch(hdc, g_themeCache.editbox[index], &rc, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheEditBox(INT iPartId, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 3.f * scale;
    INT x = 0, y = 0;
    INT width = 18, height = 18;
    if(!g_themeCache.CreateDIB(g_themeCache.editbox[stateIndex], width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.editbox[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    D2D1_ROUNDED_RECT rect = D2D1::RoundedRect(D2D1::RectF(x + .5f, y + .5f, width - .5f, height - .5f), cornerRadius, cornerRadius);
    pRenderTarget->BeginDraw();  
    if (iPartId == EP_BACKGROUNDWITHBORDER)
    {
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        pRenderTarget->CreateSolidColorBrush(g_IsSysThemeDarkMode ? MyD2D1Color(0, 0, 0) : MyD2D1Color(255, 255, 255), &brush);
        D2D1_RECT_F rc (0, 0, (FLOAT)width, (FLOAT)height);
        pRenderTarget->FillRectangle(&rc, brush.Get());
    }
    if (iStateId == ETS_NORMAL || iStateId == ETS_HOT)
    {
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        D2D1_COLOR_F borderColor = (iStateId == ETS_HOT) ? MyD2D1Color(128, 160, 160, 160) : MyD2D1Color(96, 112, 112, 112);
        pRenderTarget->CreateSolidColorBrush(borderColor, &brush);
        pRenderTarget->DrawRoundedRectangle(rect, brush.Get(), scale);
    }
    else if (iStateId == ETS_SELECTED)
    {
        FLOAT X = .5f;
        FLOAT Width = static_cast<FLOAT>(width) - .5f, Height = static_cast<FLOAT>(height) - .5f;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        pRenderTarget->CreateSolidColorBrush(MyD2D1Color(96, 112, 112, 112), &brush);
        pRenderTarget->DrawRoundedRectangle(rect, brush.Get(), scale);
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> linebrush;
        pRenderTarget->CreateSolidColorBrush(IsAccentColorPossibleD2D(105, 205, 255, SystemAccentColorLight2), &linebrush);
        pRenderTarget->DrawLine(D2D1::Point2F(cornerRadius/2 - 1.f * scale, Height - 1.f), D2D1::Point2F(width - cornerRadius/2 + 1.f * scale, Height - 1.f), linebrush.Get());
        pRenderTarget->DrawLine(D2D1::Point2F(X + 2.f * scale, Height), D2D1::Point2F(Width - 2.f * scale , Height), linebrush.Get());
    }
    else if (iStateId == ETS_DISABLED)
    {
        D2D1_ROUNDED_RECT rect = D2D1::RoundedRect(D2D1::RectF(x, y, width, height), cornerRadius, cornerRadius);
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        pRenderTarget->CreateSolidColorBrush(MyD2D1Color(96, 96, 96), &brush); 
        pRenderTarget->FillRoundedRectangle(rect, brush.Get());
    }
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintListBox(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory)
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    if (FAILED(CreateBoundD2DRenderTarget(hdc, pRect, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    D2D1_RECT_F rect((FLOAT)pRect->left, (FLOAT)pRect->top, (FLOAT)RECTWIDTH(pRect), (FLOAT)RECTHEIGHT(pRect));

    pRenderTarget->BeginDraw();

    if (iPartId == THEMECLS_COMMONPROPS_PART)
    {   
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> Brush;
        pRenderTarget->CreateSolidColorBrush(MyD2D1Color(96, 96, 96), &Brush);
        pRenderTarget->FillRectangle(&rect, Brush.Get());
    }
    else
    {
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> borderBrush = nullptr;
        D2D1_COLOR_F borderColor;
        switch (iStateId)
        {
        case LBPSH_NORMAL:
            borderColor = MyD2D1Color(160, 160, 160);
            break;
        case LBPSH_HOT:
        case LBPSH_FOCUSED:
            borderColor = IsAccentColorPossibleD2D(105, 205, 255, SystemAccentColorLight2);
            break;
        case LBPSH_DISABLED:
            borderColor = MyD2D1Color(96, 96, 96);
            break;
        default:
            borderColor = MyD2D1Color(160, 160, 160);
            break;
        }
        pRenderTarget->CreateSolidColorBrush(borderColor, &borderBrush);
        pRenderTarget->FillRectangle(&rect, borderBrush.Get());
    }
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintDropDownArrow(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect, BOOL addressPart)
{
    if (!g_d2dFactory || (iPartId != CP_DROPDOWNBUTTON && iPartId != CP_DROPDOWNBUTTONRIGHT
        && iPartId != CP_DROPDOWNBUTTONLEFT))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    if (FAILED(CreateBoundD2DRenderTarget(hdc, pRect, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
    D2D1_COLOR_F arrowColor;
    switch (iStateId)
    {
    case CBXSL_NORMAL:
        arrowColor = MyD2D1Color(192, 192, 192);
        break;
    case CBXS_HOT:
        arrowColor = MyD2D1Color(255, 255, 255);
        break;
    case CBXS_PRESSED:
        arrowColor = MyD2D1Color(160, 160, 160);
        break;
    case CBXS_DISABLED:
        arrowColor = MyD2D1Color(96, 96, 96);
        break;
    }

    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT width = static_cast<FLOAT>RECTWIDTH(pRect);
    FLOAT height = static_cast<FLOAT>RECTHEIGHT(pRect);
    FLOAT centerX = width / 2.f;
    FLOAT centerY = height / 2.f;

    FLOAT arrowLength = (addressPart) ? fminf(width, height) *  0.14f : fminf(width, height) *  0.25f;
    // 60 degree angle
    FLOAT dx = arrowLength * 0.866f;
    FLOAT dy = arrowLength * 0.5f;

    D2D1_POINT_2F ptTip   = D2D1::Point2F(centerX, centerY + dy);
    D2D1_POINT_2F ptLeft  = D2D1::Point2F(centerX - dx, centerY - dy);
    D2D1_POINT_2F ptRight = D2D1::Point2F(centerX + dx, centerY - dy);

    pRenderTarget->CreateSolidColorBrush(arrowColor, &brush);

    pRenderTarget->BeginDraw();
    pRenderTarget->DrawLine(ptLeft, ptTip, brush.Get(), scale*1.2f);
    pRenderTarget->DrawLine(ptRight, ptTip, brush.Get(), scale*1.2f);
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintTab(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (iPartId == TABP_PANE || !g_d2dFactory)
        return FALSE;

    INT index = (iStateId == TIS_NORMAL) ? 0 
              : (iStateId == TIS_HOT) ? 1 : (iStateId == TIS_DISABLED) ? 2 : 3;

    if (!g_themeCache.tab[index])
        if (!g_themeCache.CacheTab(iStateId, index))
            return FALSE;

    DrawNineGridStretch(hdc, g_themeCache.tab[index], pRect, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheTab(INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT width = 18, height = 18;

    if(!g_themeCache.CreateDIB(g_themeCache.tab[stateIndex], width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.tab[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    pRenderTarget->BeginDraw();
    if (iStateId == TIS_NORMAL)
    {
        D2D1_RECT_F rect{0, 0, (FLOAT)width, (FLOAT)height};
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        pRenderTarget->CreateSolidColorBrush(MyD2D1Color(0, 0, 0, 0), &brush);
        pRenderTarget->FillRectangle(rect, brush.Get());
    }
    else if (iStateId == TIS_HOT || iStateId == TIS_DISABLED)
    {
        D2D1_COLOR_F fillColor = (iStateId == TIS_HOT) ? MyD2D1Color(128, 96, 96, 96) : MyD2D1Color(96, 96, 96);
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        pRenderTarget->CreateSolidColorBrush(fillColor, &brush);
        D2D1_ROUNDED_RECT tabRect = D2D1::RoundedRect(D2D1::RectF(0, 0, width, height), cornerRadius, cornerRadius);
        pRenderTarget->FillRoundedRectangle(tabRect, brush.Get());
    }
    else if (iStateId == TIS_SELECTED || iStateId == TIS_FOCUSED)
    {
        const FLOAT desiredHeight = 2.0f + round(scale);       
        const FLOAT widthPadding  = 5.0f;
        const FLOAT verticalOffset = 1.0f;      
    
        FLOAT pillLeft   = widthPadding;
        FLOAT pillRight  = width - widthPadding;
        FLOAT pillBottom = height - verticalOffset;
        FLOAT pillTop    = pillBottom - desiredHeight;
        FLOAT pillRadius = 1.f + round(scale);

        D2D1_ROUNDED_RECT pillRect = D2D1::RoundedRect(D2D1::RectF(pillLeft, pillTop, pillRight, pillBottom),pillRadius, pillRadius);

        D2D1_COLOR_F pillColor = IsAccentColorPossibleD2D(102, 206, 255, SystemAccentColorLight2);
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        pRenderTarget->CreateSolidColorBrush(pillColor, &brush);
        pRenderTarget->FillRoundedRectangle(pillRect, brush.Get());
    }
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintTrackbar(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory || (iPartId != TKP_TRACK && iPartId != TKP_TRACKVERT))
        return FALSE;
    
    INT index = (iPartId == TKP_TRACK) ? 0 : 1;

    if (!g_themeCache.trackbar[index])
        if (!g_themeCache.CacheTrackBar(iPartId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_themeCache.trackbar[index], pRect, 2, 2, 2, 2);
    return TRUE;
}

BOOL CThemeCache::CacheTrackBar(INT iPartId, INT stateIndex)
{
    INT width = 6, height = 6;
    if(!g_themeCache.CreateDIB(g_themeCache.trackbar[stateIndex], width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.trackbar[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;
    
    pRenderTarget->BeginDraw();
    D2D1_ROUNDED_RECT body = D2D1::RoundedRect(D2D1::RectF(0, 0, width, height), 2.f, 2.f);
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
    pRenderTarget->CreateSolidColorBrush(MyD2D1Color(96, 96, 96), &brush);
    pRenderTarget->FillRoundedRectangle(&body, brush.Get());
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintTrackbarThumb(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory || (iPartId != TKP_THUMB && iPartId != TKP_THUMBVERT))
        return FALSE;
    
    if (iStateId == TUBS_FOCUSED) iStateId = 1;
    else if (iStateId == TUBS_DISABLED) iStateId = 4;
    INT index = (iPartId == TKP_THUMB) ? iStateId - 1 : iStateId + 3;

    if (!g_themeCache.trackbarthumb[index])
        if (!g_themeCache.CacheTrackBarThumb(iPartId, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_themeCache.trackbarthumb[index], pRect);
    return TRUE;
}

BOOL CThemeCache::CacheTrackBarThumb(INT iPartId, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 3.f * scale;
    INT width = 10 * scale, height = 21 * scale;

    if (iPartId == TKP_THUMBVERT)
        width = std::exchange(height, width);
    
    if(!g_themeCache.CreateDIB(g_themeCache.trackbarthumb[stateIndex], width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.trackbarthumb[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;
    
    D2D1_COLOR_F fillColor = (iStateId == TUBS_HOT) ? IsAccentColorPossibleD2D(102, 206, 255, SystemAccentColorLight3) : 
                             (iStateId == TUBS_PRESSED) ? IsAccentColorPossibleD2D(60, 110, 180, SystemAccentColorLight1) :
                             (iStateId == TUBS_DISABLED) ? MyD2D1Color(96, 96, 96) : MyD2D1Color(64, 64, 64);
    D2D1_ROUNDED_RECT body = D2D1::RoundedRect(D2D1::RectF(0, 0, width, height), cornerRadius, cornerRadius);
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
    pRenderTarget->CreateSolidColorBrush(fillColor, &brush);
    pRenderTarget->BeginDraw();
    pRenderTarget->FillRoundedRectangle(&body, brush.Get());
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintTrackBarPointedThumb(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory || (iPartId != TKP_THUMBBOTTOM && iPartId != TKP_THUMBTOP 
        && iPartId != TKP_THUMBLEFT && iPartId != TKP_THUMBRIGHT))
        return FALSE;

    if (iStateId == TUBS_FOCUSED) iStateId = 1;
    else if (iStateId == TUBS_DISABLED) iStateId = 4;
    INT index = (iPartId == TKP_THUMBBOTTOM) ? iStateId + 7 : (iPartId == TKP_THUMBTOP) ? iStateId + 11 :
                (iPartId == TKP_THUMBLEFT) ? iStateId + 15 : iStateId + 19;

    if (!g_themeCache.trackbarthumb[index])
        if (!g_themeCache.CacheTrackBarPointedThumb(iPartId, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_themeCache.trackbarthumb[index], pRect);
    return TRUE;
}

BOOL CThemeCache::CacheTrackBarPointedThumb(INT iPartId, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    INT width = 11 * scale, height = 19 * scale;

    if (iPartId == TKP_THUMBLEFT || iPartId == TKP_THUMBRIGHT)
        width = std::exchange(height, width);
    
    if(!g_themeCache.CreateDIB(g_themeCache.trackbarthumb[stateIndex], width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.trackbarthumb[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;
    
    D2D1_COLOR_F fillColor = (iStateId == TUBS_HOT) ? IsAccentColorPossibleD2D(102, 206, 255, SystemAccentColorLight3) : 
                             (iStateId == TUBS_PRESSED) ? IsAccentColorPossibleD2D(60, 110, 180, SystemAccentColorLight1) :
                             (iStateId == TUBS_DISABLED) ? MyD2D1Color(96, 96, 96) : MyD2D1Color(64, 64, 64);
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
    pRenderTarget->CreateSolidColorBrush(fillColor, &brush);

    FLOAT cx = width * 0.5f;
    FLOAT cy = height * 0.5f;
    Microsoft::WRL::ComPtr<ID2D1PathGeometry> triangleGeo;
    Microsoft::WRL::ComPtr<ID2D1GeometrySink> sink;

    pRenderTarget->BeginDraw();
    if (iPartId == TKP_THUMBBOTTOM)
    {
        FLOAT tipHeight = height * 0.3f;
        FLOAT bodyHeight = height - tipHeight;
        FLOAT bodyRadius = 2.f * scale;

        D2D1_ROUNDED_RECT body = D2D1::RoundedRect(D2D1::RectF(0, 0, width, bodyHeight), bodyRadius, bodyRadius);
        pRenderTarget->FillRoundedRectangle(body, brush.Get());

        D2D1_POINT_2F p1 = D2D1::Point2F(cx - width * 0.5f, bodyHeight - 1.f);
        D2D1_POINT_2F p2 = D2D1::Point2F(cx + width * 0.5f, bodyHeight - 1.f);
        D2D1_POINT_2F p3 = D2D1::Point2F(cx, height);

        g_d2dFactory->CreatePathGeometry(&triangleGeo);
        triangleGeo->Open(&sink);
        sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(p2);
        sink->AddLine(p3);
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        sink->Close();
        pRenderTarget->FillGeometry(triangleGeo.Get(), brush.Get());
    }
    else if (iPartId == TKP_THUMBTOP)
    {
        FLOAT tipHeight = height * 0.3f;
        FLOAT bodyY = tipHeight;
        FLOAT bodyRadius = 2.f * scale;

        D2D1_ROUNDED_RECT body = D2D1::RoundedRect(D2D1::RectF(0, bodyY, width, height), bodyRadius, bodyRadius);
        pRenderTarget->FillRoundedRectangle(body, brush.Get());

        D2D1_POINT_2F p1 = D2D1::Point2F(cx - width * 0.5f, tipHeight + 1.f);
        D2D1_POINT_2F p2 = D2D1::Point2F(cx + width * 0.5f, tipHeight + 1.f);
        D2D1_POINT_2F p3 = D2D1::Point2F(cx, 0);

        g_d2dFactory->CreatePathGeometry(&triangleGeo);
        triangleGeo->Open(&sink);
        sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(p2);
        sink->AddLine(p3);
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        sink->Close();

        pRenderTarget->FillGeometry(triangleGeo.Get(), brush.Get());
    }
    else if (iPartId == TKP_THUMBLEFT)
    {
        FLOAT tipWidth = width * 0.3f;
        FLOAT bodyRadius = 2.f * scale;

        D2D1_ROUNDED_RECT body = D2D1::RoundedRect(D2D1::RectF(tipWidth, 0, width, height), bodyRadius, bodyRadius);
        pRenderTarget->FillRoundedRectangle(body, brush.Get());

        D2D1_POINT_2F p1 = D2D1::Point2F(tipWidth + 1.f, cy - height * 0.5f);
        D2D1_POINT_2F p2 = D2D1::Point2F(tipWidth + 1.f, cy + height * 0.5f);
        D2D1_POINT_2F p3 = D2D1::Point2F(0, cy);

        g_d2dFactory->CreatePathGeometry(&triangleGeo);
        triangleGeo->Open(&sink);
        sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(p2);
        sink->AddLine(p3);
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        sink->Close();

        pRenderTarget->FillGeometry(triangleGeo.Get(), brush.Get());
    }
    else if (iPartId == TKP_THUMBRIGHT)
    {
        FLOAT tipWidth = width * 0.3f;
        FLOAT bodyWidth = width - tipWidth;
        FLOAT bodyRadius = 2.f * scale;

        D2D1_ROUNDED_RECT body = D2D1::RoundedRect(D2D1::RectF(0, 0, bodyWidth, height), bodyRadius, bodyRadius);
        pRenderTarget->FillRoundedRectangle(body, brush.Get());

        D2D1_POINT_2F p1 = D2D1::Point2F(bodyWidth - 1.f, cy - height * 0.5f);
        D2D1_POINT_2F p2 = D2D1::Point2F(bodyWidth - 1.f, cy + height * 0.5f);
        D2D1_POINT_2F p3 = D2D1::Point2F(width, cy);

        g_d2dFactory->CreatePathGeometry(&triangleGeo);
        triangleGeo->Open(&sink);
        sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(p2);
        sink->AddLine(p3);
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        sink->Close();

        pRenderTarget->FillGeometry(triangleGeo.Get(), brush.Get());
    }
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintProgressBar(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory)
        return FALSE;
    if (iPartId == PP_PULSEOVERLAY || iPartId == PP_MOVEOVERLAY || iPartId == PP_PULSEOVERLAYVERT || iPartId == PP_MOVEOVERLAYVERT)
        return TRUE;

    INT index = (iPartId == PP_FILL) ? iStateId - 1 : (iPartId == PP_FILLVERT) ? iStateId + 3 
              : (iPartId == PP_CHUNK || iPartId == PP_CHUNKVERT) ? 8 : 9;
    
    if (!g_themeCache.progressbar[index])
        if (!g_themeCache.CacheProgressBar(iPartId, iStateId, index))
            return FALSE;
    if (iPartId == PP_FILL)
        DrawNineGridStretch(hdc, g_themeCache.progressbar[index], pRect, 8, 10, 8, 10);
    else if (iPartId == PP_FILLVERT)
        DrawNineGridStretch(hdc, g_themeCache.progressbar[index], pRect, 10, 8, 10, 8);
    else
        DrawNineGridStretch(hdc, g_themeCache.progressbar[index], pRect, 8, 8, 9, 9);
    return TRUE;
}

BOOL CThemeCache::CacheProgressBar(INT iPartId, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 3.f * scale;
    INT x = 0, y = 0;
    INT width = 18, height = 18;

    if (iPartId == PP_FILL) 
        width = 50, height = 23;
    else if (iPartId == PP_FILLVERT)
        width = 23, height = 50;

    if(!g_themeCache.CreateDIB(g_themeCache.progressbar[stateIndex], width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { x, y, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.progressbar[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;
    
    D2D1_RECT_F rect = D2D1::RectF(0, 0, width, height);
    D2D1_ROUNDED_RECT rounded = D2D1::RoundedRect(rect, cornerRadius, cornerRadius);

    pRenderTarget->BeginDraw();
    if (iPartId == PP_BAR || iPartId == PP_BARVERT ||
        iPartId == PP_TRANSPARENTBAR || iPartId == PP_TRANSPARENTBARVERT)
    {
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        pRenderTarget->CreateSolidColorBrush(MyD2D1Color(96, 96, 96), &brush);
        pRenderTarget->FillRoundedRectangle(rounded, brush.Get());
    }
    else if (iPartId == PP_CHUNK || iPartId == PP_CHUNKVERT)
    {
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        pRenderTarget->CreateSolidColorBrush(IsAccentColorPossibleD2D(102, 206, 255, SystemAccentColorLight2), &brush);
        pRenderTarget->FillRoundedRectangle(rounded, brush.Get());
    }
    else if (iPartId == PP_FILL || iPartId == PP_FILLVERT)
    {
        BOOL isVertical = (iPartId == PP_FILLVERT);
        D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES props = {};
        props.startPoint = isVertical ? D2D1::Point2F(rect.right/2, rect.bottom)
                                      : D2D1::Point2F(rect.left, rect.bottom/2);
        props.endPoint   = isVertical ? D2D1::Point2F(rect.right/2, rect.top)
                                      : D2D1::Point2F(rect.right, rect.bottom/2);
        D2D1_GRADIENT_STOP stops[2];

        switch (iStateId)
        {
            case PBFS_NORMAL:
            {
                Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> solidBrush;
                pRenderTarget->CreateSolidColorBrush(IsAccentColorPossibleD2D(102, 206, 255, SystemAccentColorLight2), &solidBrush);
                pRenderTarget->FillRoundedRectangle(rounded, solidBrush.Get());
                break;
            }
            case PBFS_ERROR:
            {
                stops[0].color = MyD2D1Color(228, 48, 96);
                stops[0].position = 0.0f;
                stops[1].color = MyD2D1Color(255, 96, 81);
                stops[1].position = 1.0f;
                break;
            }
            case PBFS_PAUSED:
            {
                stops[0].color = MyD2D1Color(228, 128, 48);
                stops[0].position = 0.0f;
                stops[1].color = MyD2D1Color(237, 206, 80);
                stops[1].position = 1.0f;
                break;
            }
            case PBFS_PARTIAL:
            {
                stops[0].color = IsAccentColorPossibleD2D(0, 120, 215, SystemAccentColorBase);
                stops[0].position = 0.0f;
                stops[1].color = IsAccentColorPossibleD2D(64, 160, 255, SystemAccentColorLight2);
                stops[1].position = 1.0f;
                break;
            }
        }
        if (iStateId != PBFS_NORMAL)
        {
            Microsoft::WRL::ComPtr<ID2D1GradientStopCollection> gradientStops;
            pRenderTarget->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &gradientStops);

            Microsoft::WRL::ComPtr<ID2D1LinearGradientBrush> gradientBrush;
            pRenderTarget->CreateLinearGradientBrush(props, gradientStops.Get(), &gradientBrush);

            pRenderTarget->FillRoundedRectangle(rounded, gradientBrush.Get());
        }
    }
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintIndeterminateProgressBar(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory)
        return FALSE;
    if (iPartId != PP_MOVEOVERLAY && iPartId != PP_MOVEOVERLAYVERT)
        return TRUE;

    INT index = (iPartId == PP_MOVEOVERLAY) ? 0 : 1;
    
    // Make progress bar thin
    RECT overlayRect;
    if (iPartId == PP_MOVEOVERLAY)
    {
        INT overlayHeight = RECTHEIGHT(pRect) / 3;
        INT overlayY = (RECTHEIGHT(pRect) - overlayHeight) / 1.5f;
        overlayRect = RECT(pRect->left, overlayY, pRect->right, overlayY + overlayHeight);
    }
    else if (iPartId == PP_MOVEOVERLAYVERT)
    {
        FLOAT overlayWidth = RECTWIDTH(pRect) / 3.0f;
        FLOAT overlayX = (RECTWIDTH(pRect) - overlayWidth) / 1.5f;
        overlayRect = RECT(overlayX, pRect->top, overlayX + overlayWidth, pRect->bottom);
    }
    
    if (!g_themeCache.indeterminatebar[index])
        if (!g_themeCache.CacheIndeterminateBar(iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_themeCache.indeterminatebar[index], &overlayRect, 6, 6, 5, 5);
    return TRUE;
}

BOOL CThemeCache::CacheIndeterminateBar(INT iPartId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 3.f * scale;
    INT width = 12, height = 12;

    if (iPartId == PP_MOVEOVERLAYVERT)
        width = std::exchange(height, width);

    if(!g_themeCache.CreateDIB(g_themeCache.indeterminatebar[stateIndex], width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = {0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.indeterminatebar[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;
    
    D2D1_ROUNDED_RECT rounded = D2D1::RoundedRect(D2D1::RectF(0, 0, width, height), cornerRadius, cornerRadius);
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
    pRenderTarget->CreateSolidColorBrush(IsAccentColorPossibleD2D(102, 206, 255, SystemAccentColorLight2), &brush);

    pRenderTarget->BeginDraw();
    pRenderTarget->FillRoundedRectangle(&rounded, brush.Get());
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintListView(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory || (iPartId != THEMECLS_COMMONPROPS_PART && iPartId != LVP_LISTITEM 
        && iPartId != LVP_GROUPHEADER && iPartId != LVP_GROUPHEADERLINE && iPartId != LVP_COLUMNDETAIL))
        return FALSE;

    INT index;
    if (iPartId == THEMECLS_COMMONPROPS_PART || iPartId == LVP_LISTITEM) 
        index = (!iPartId) ? 0 : iStateId;
    else if (iPartId == LVP_GROUPHEADER)
    {
        if (iStateId == 2 || iStateId == 4 || iStateId == 6
        || iStateId == 8 || iStateId == 10) index = 7;
        else if (iStateId == 11 || iStateId == 15) index = 8;
        else if (iStateId == 12 || iStateId == 16) index = 9;
        else if (iStateId == 13) index = 10;
        else if (iStateId == 14) index = 11;
        else return FALSE; 
    }
    else if (iPartId == LVP_GROUPHEADERLINE) index = 12;
    else if (iPartId == LVP_COLUMNDETAIL) index = 13;
    else return FALSE;

    if (!g_themeCache.listview[index])
    {
        if (index <= 6)
        {
            if (!g_themeCache.CacheListItem(iPartId, iStateId, index))
                return FALSE;
        }
        else
            if (!g_themeCache.CacheListGroupHeader(iPartId, iStateId, index))
                return FALSE;
    }
    DrawNineGridStretch(hdc, g_themeCache.listview[index], pRect, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheListItem(INT iPartId, INT iStateId, INT stateIndex)
{
    if (iPartId != THEMECLS_COMMONPROPS_PART && iPartId != LVP_LISTITEM)
        return FALSE;
    
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT x = 0, y = 0;
    INT width = 18, height = 18;

    if(!g_themeCache.CreateDIB(g_themeCache.listview[stateIndex], width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.listview[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
    if (iPartId == THEMECLS_COMMONPROPS_PART)
    {
        D2D1_RECT_F rect = D2D1::RectF(x, y, width, height);
        pRenderTarget->CreateSolidColorBrush(MyD2D1Color(96, 96, 96), &brush);
        pRenderTarget->BeginDraw();
        pRenderTarget->FillRectangle(&rect, brush.Get());
        auto hr = pRenderTarget->EndDraw();
        if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    }
    else
    {
        D2D1_COLOR_F fillColor, borderColor;
        switch (iStateId)
        {
            case LISS_NORMAL:
                fillColor = MyD2D1Color(96, 144, 144, 144);
                borderColor = IsAccentColorPossibleD2D(102, 206, 255, SystemAccentColorLight2);
                break;
            case LISS_HOT:
                fillColor = MyD2D1Color(96, 144, 144, 144);
                break;
            case LISS_SELECTED:
                fillColor = MyD2D1Color(64, 144, 144, 144);
                break;
            case LISS_DISABLED:
                fillColor = MyD2D1Color(96, 144, 144, 144);
                borderColor = IsAccentColorPossibleD2D(102, 206, 255, SystemAccentColorLight2);
                break;
            case LISS_SELECTEDNOTFOCUS:
                fillColor = MyD2D1Color(32, 144, 144, 144);
                break;
            case LISS_HOTSELECTED:
                fillColor = MyD2D1Color(64, 144, 144, 144);
                borderColor = IsAccentColorPossibleD2D(102, 206, 255, SystemAccentColorLight2);
                break;
        }
        
        D2D1_ROUNDED_RECT rounded = D2D1::RoundedRect(D2D1::RectF(x, y, width , height), cornerRadius, cornerRadius);
        pRenderTarget->CreateSolidColorBrush(fillColor, &brush);
        pRenderTarget->BeginDraw();
        pRenderTarget->FillRoundedRectangle(&rounded, brush.Get());

        if (iStateId == LISS_HOTSELECTED || iStateId == LISS_NORMAL || iStateId == LISS_DISABLED)
        {
            x = y = 1.f;
            width = height -= 1.f;
            rounded = D2D1::RoundedRect(D2D1::RectF(x, y, width , height), cornerRadius - 1.f, cornerRadius - 1.f);
            brush->SetColor(borderColor);
            pRenderTarget->DrawRoundedRectangle(&rounded, brush.Get(), 2.f * scale);
        }
        auto hr = pRenderTarget->EndDraw();
        if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    }
    return TRUE;
}

BOOL CThemeCache::CacheListGroupHeader(INT iPartId, INT iStateId, INT stateIndex)
{
    if (iPartId != LVP_GROUPHEADER && iPartId != LVP_GROUPHEADERLINE && iPartId != LVP_COLUMNDETAIL)
        return FALSE;

    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT x = 0, y = 0;
    INT width = (iPartId == LVP_COLUMNDETAIL) ? 2 : 18, height = (iPartId == LVP_COLUMNDETAIL) ? 1 : 18;

    if (!g_themeCache.CreateDIB(g_themeCache.listview[stateIndex], width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.listview[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
    if (iPartId == LVP_COLUMNDETAIL)
    {
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        pRenderTarget->CreateSolidColorBrush(MyD2D1Color(128, 160, 160, 160), &brush);

        pRenderTarget->BeginDraw();
        pRenderTarget->DrawLine(D2D1_POINT_2F(width, y), D2D1_POINT_2F(width, height), brush.Get());
        auto hr = pRenderTarget->EndDraw();
        if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    }
    else if (iPartId == LVP_GROUPHEADER)
    {
        D2D1_COLOR_F fillColor = {};
        D2D1_COLOR_F borderColor = {};

        switch (iStateId)
        {
            case LVGH_OPENHOT: case LVGH_OPENSELECTEDHOT:
            case LVGH_OPENSELECTEDNOTFOCUSEDHOT: case LVGH_OPENMIXEDSELECTIONHOT:
            case LVGH_CLOSEHOT:
                fillColor = MyD2D1Color(96, 144, 144, 144);
                break;
            case LVGH_CLOSESELECTED:
            case LVGH_CLOSEMIXEDSELECTION:
                fillColor = MyD2D1Color(64, 144, 144, 144);
                break;
            case LVGHL_CLOSEMIXEDSELECTIONHOT:
            case LVGHL_CLOSESELECTEDHOT:
                fillColor = MyD2D1Color(64, 144, 144, 144);
                break;
            case LVGHL_CLOSESELECTEDNOTFOCUSED:
                borderColor = MyD2D1Color(255, 255, 255);
                break;
            case LVGHL_CLOSESELECTEDNOTFOCUSEDHOT:
                fillColor = MyD2D1Color(96, 144, 144, 144);
                borderColor = MyD2D1Color(255, 255, 255);
                break;
            default:
                return FALSE;
        }
        D2D1_ROUNDED_RECT rounded = D2D1::RoundedRect(D2D1::RectF(x, y, width , height), cornerRadius, cornerRadius);
        pRenderTarget->BeginDraw();
        if (iStateId != LVGHL_CLOSESELECTEDNOTFOCUSED)
        {
            pRenderTarget->CreateSolidColorBrush(fillColor, &brush);
            pRenderTarget->FillRoundedRectangle(&rounded, brush.Get());
        }

        if (iStateId == LVGHL_CLOSESELECTEDNOTFOCUSED || iStateId == LVGHL_CLOSESELECTEDNOTFOCUSEDHOT)
        {
            x = y = 1.f;
            width = height -= 1.f;
            rounded = D2D1::RoundedRect(D2D1::RectF(x, y, width , height), cornerRadius - 1.f, cornerRadius - 1.f);
            pRenderTarget->CreateSolidColorBrush(borderColor, &brush);
            pRenderTarget->DrawRoundedRectangle(&rounded, brush.Get(), 2.f * scale);
        }
        auto hr = pRenderTarget->EndDraw();
        if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    }
    else
    {
        D2D1_RECT_F rect = D2D1::RectF(x, y, width, height);
        pRenderTarget->CreateSolidColorBrush(MyD2D1Color(96, 160, 160, 160), &brush);
        pRenderTarget->BeginDraw();
        pRenderTarget->FillRectangle(&rect, brush.Get());
        auto hr = pRenderTarget->EndDraw();
        if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    }
    return TRUE;
}

BOOL PaintTreeViewButton(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory || (iPartId != THEMECLS_COMMONPROPS_PART && iPartId != TVP_TREEITEM))
        return FALSE;
    
    INT index = (iPartId == THEMECLS_COMMONPROPS_PART) ? 0 : (iStateId == TREIS_HOT) ? 1 : (iStateId == TREIS_SELECTED) ? 2 :
                (iStateId == TREIS_SELECTEDNOTFOCUS) ? 3 : 4;

    if (!g_themeCache.treeview[index])
        if (!g_themeCache.CacheTreeViewButton(iPartId, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_themeCache.treeview[index], pRect, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheTreeViewButton(INT iPartId, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 5.f * scale;
    FLOAT x = 0, y = 0;
    INT width = 18, height = 18;

    if (!g_themeCache.CreateDIB(g_themeCache.treeview[stateIndex], width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.treeview[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;
    
    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, width, height), cornerRadius, cornerRadius);
    pRenderTarget->BeginDraw();
    if (iPartId == THEMECLS_COMMONPROPS_PART)
    {
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> borderBrush;
        pRenderTarget->CreateSolidColorBrush(MyD2D1Color(96, 96, 96), &borderBrush);
        pRenderTarget->FillRectangle(D2D1::RectF(x, y, width, height), borderBrush.Get());
    }
    else if (iPartId == TVP_TREEITEM)
    {
        D2D1_COLOR_F fillColor = (iStateId == TREIS_HOT)              ? MyD2D1Color(96, 144, 144, 144) : 
                                 (iStateId == TREIS_SELECTED)         ? MyD2D1Color(64, 144, 144, 144) :
                                 (iStateId == TREIS_SELECTEDNOTFOCUS) ? MyD2D1Color(32, 144, 144, 144) :
                                                                        //TREIS_HOTSELECTED
                                                                        MyD2D1Color(64, 144, 144, 144); 

        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        pRenderTarget->CreateSolidColorBrush(fillColor, &brush);
        pRenderTarget->FillRoundedRectangle(&roundedRect, brush.Get());

        if (iStateId == TREIS_SELECTED || iStateId == TREIS_SELECTEDNOTFOCUS || iStateId == TREIS_HOTSELECTED)
        {
            FLOAT pillOffsetY = 7, pillWidth = 2.f + round(scale), pillRadius = 1.f + round(scale);
            brush->SetColor(IsAccentColorPossibleD2D(102, 206, 255, SystemAccentColorLight2));
            pRenderTarget->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(x, y + pillOffsetY, x + pillWidth, height - pillOffsetY), pillRadius, pillRadius),brush.Get());
        }
    }
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintTreeViewGlyph(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory || (iPartId != TVP_GLYPH && iPartId != TVP_HOTGLYPH))
        return FALSE;

    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT width = RECTWIDTH(pRect);

    // TreeView glyph symbols have a fixed bitmap size (marked as SIZINGTYPE=TRUESIZE)
    // The TreeView theme class has a bitmap size of 9x9px, while the Explorer::TreeView theme class has a bitmap size of 16x16px
    // Unfortunately, OpenThemeData only detects the parent TreeView theme class
    BOOL ExplorerTreeView = FALSE;
    if (width / (16 * scale) == 1)
        ExplorerTreeView = TRUE;

    INT index = (iPartId == TVP_GLYPH) ? index = iStateId - 1 : index = iStateId + 1;
    index = (ExplorerTreeView) ? index + 4 : index;

    if (!g_themeCache.treeviewglyph[index])
        if (!g_themeCache.CacheTreeViewGlyph(iPartId, iStateId, index, ExplorerTreeView))
            return FALSE;
    DrawNineGridStretch(hdc, g_themeCache.treeviewglyph[index], pRect);
    return TRUE;
}

BOOL CThemeCache::CacheTreeViewGlyph(INT iPartId, INT iStateId, INT stateIndex, BOOL ExplorerTreeView)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    INT width = 9 * scale;
    INT height = 9 * scale;

    if (ExplorerTreeView)
        width = height = 16 * scale;

    if (!g_themeCache.CreateDIB(g_themeCache.treeviewglyph[stateIndex], width, height))
        return FALSE;

    RECT rc {0, 0, width, height};
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.treeviewglyph[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;
    
    D2D1_COLOR_F arrowColor;
    if (iPartId == TVP_HOTGLYPH) {
        if (iStateId == HGLPS_CLOSED) arrowColor =  MyD2D1Color(255, 255, 255);
        else if (iStateId == HGLPS_OPENED) arrowColor = (g_IsSysThemeDarkMode) ? MyD2D1Color(192, 192, 192) : MyD2D1Color(128, 128, 128);
    }
    else if (iPartId == TVP_GLYPH) {
        if (iStateId == GLPS_CLOSED) arrowColor = (g_IsSysThemeDarkMode) ? MyD2D1Color(148, 148, 148) : MyD2D1Color(64, 64, 64);
        else if (iStateId == GLPS_OPENED) arrowColor = MyD2D1Color(255, 255, 255);
    }

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> arrowBrush;
    pRenderTarget->CreateSolidColorBrush(arrowColor, &arrowBrush);

    FLOAT centerX = width / 2.f;
    FLOAT centerY = height / 2.f;
    FLOAT arrowLength = (ExplorerTreeView) ? width * 0.3f : width * 0.55f;
    // 60 degrees
    FLOAT dx = arrowLength * 0.866f;
    FLOAT dy = arrowLength * 0.5f;

    D2D1_POINT_2F ptTip, ptLeft, ptRight;

    if (iStateId == GLPS_OPENED)
    {
        ptTip   = {centerX, centerY + dy};
        ptLeft  = {centerX - dx, centerY - dy};
        ptRight = {centerX + dx, centerY - dy};
    }
    else if (iStateId == GLPS_CLOSED)
    {
        ptTip   = { centerX + dy, centerY };
        ptLeft  = { centerX - dy, centerY - dx };
        ptRight = { centerX - dy, centerY + dx };
    }

    pRenderTarget->BeginDraw();

    pRenderTarget->DrawLine(ptLeft, ptTip, arrowBrush.Get(), 1.5f * scale);
    pRenderTarget->DrawLine(ptRight, ptTip, arrowBrush.Get(), 1.5f * scale);

    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintItemsView(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory || (iPartId != 1 && iPartId != 3 && iPartId != 6
        && (iPartId != 4 && (iStateId == 11 || iStateId == 12))))
        return FALSE;

    INT index = (iPartId == 1 && (iStateId % 2 == 1)) ? 0 :
                (iPartId == 1 && (iStateId % 2 == 0)) ? 1 : (iPartId == 6) ? iStateId + 1 : iStateId + 3;
    
    // New DarkTheme file conflict dialog buttons
    if (iPartId == 4 && iStateId == 11)
        return PaintListView(hdc, 1, 6, pRect);
    else if (iPartId == 4 && iStateId == 12)
        return PaintListView(hdc, 1, 2, pRect);
    
    if (!g_themeCache.itemsview[index])
        if (!g_themeCache.CacheItemsView(iPartId, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_themeCache.itemsview[index], pRect, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheItemsView(INT iPartId, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT x = 0, y = 0;
    INT width = 18, height = 18;

    if(!g_themeCache.CreateDIB(g_themeCache.itemsview[stateIndex], width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { x, y, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.itemsview[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    x = y += 1;
    width = height -= 1;

    pRenderTarget->BeginDraw();
    if (iPartId == 1)
    {
        if (iStateId == 1 || iStateId == 3)
        {
            D2D1_ROUNDED_RECT rect = D2D1::RoundedRect(D2D1::RectF(x, y, width, height), cornerRadius, cornerRadius);
            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
            pRenderTarget->CreateSolidColorBrush(IsAccentColorPossibleD2D(0, 96, 188, SystemAccentColorLight2), &brush);
            pRenderTarget->FillRoundedRectangle(rect, brush.Get());

            brush->SetColor(IsAccentColorPossibleD2D(0, 120, 215, SystemAccentColorLight2));
            pRenderTarget->DrawRoundedRectangle(rect, brush.Get(), 2.0f * scale);
        }
        else if (iStateId == 2 || iStateId == 4)
        {
            D2D1_ROUNDED_RECT rect = D2D1::RoundedRect(D2D1::RectF(x, y, width, height), cornerRadius, cornerRadius);
            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> fillBrush;
            pRenderTarget->CreateSolidColorBrush(IsAccentColorPossibleD2D(0, 96, 188, SystemAccentColorLight2), &fillBrush);
            pRenderTarget->FillRoundedRectangle(rect, fillBrush.Get());
        }
    }
    else if (iPartId == 3 || iPartId == 6)
    {
        if (iStateId == 1)
        {
            FLOAT radius = (iPartId == 6) ? 2.f * scale : 3.f * scale;
            D2D1_ROUNDED_RECT rect = D2D1::RoundedRect(D2D1::RectF(x, y, width, height), radius, radius);
            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> borderBrush;
            pRenderTarget->CreateSolidColorBrush(MyD2D1Color(255, 255, 255), &borderBrush);
            pRenderTarget->DrawRoundedRectangle(rect, borderBrush.Get(), 2.0f * scale);
        }
        else if (iStateId == 2)
        {
            D2D1_ROUNDED_RECT rect = D2D1::RoundedRect(D2D1::RectF(x-1, y-1, width+1, height+1), cornerRadius, cornerRadius);
            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> fillBrush;
            pRenderTarget->CreateSolidColorBrush(MyD2D1Color(128, 144, 144, 144), &fillBrush);
            pRenderTarget->FillRoundedRectangle(rect, fillBrush.Get());
        }
    }

    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintHeader(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory || iPartId != HP_HEADERITEM)
        return FALSE;

    if (iStateId % 3 == 1) return TRUE;
    INT index = (iStateId % 3 == 2) ? 0 : 1;
    
    if (!g_themeCache.header[index])
        if (!g_themeCache.CacheHeader(iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_themeCache.header[index], pRect, 12, 0, 11, 12);
    return TRUE;
}

BOOL CThemeCache::CacheHeader(INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 6.f * scale;
    INT x = 0, y = 0;
    INT width = 24, height = 24;

    if(!g_themeCache.CreateDIB(g_themeCache.header[stateIndex], width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { x, y, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.header[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    D2D1_COLOR_F fillColor;
    switch (iStateId)
    {
        case HIS_HOT: case HIS_SORTEDHOT:
        case HIS_ICONHOT: case HIS_ICONSORTEDHOT:
            fillColor = MyD2D1Color(96, 144, 144, 144);
            break;
        case HIS_PRESSED: case HIS_SORTEDPRESSED:
        case HIS_ICONPRESSED: case HIS_ICONSORTEDPRESSED:
            fillColor = MyD2D1Color(64, 144, 144, 144);
            break;
        case HIS_NORMAL: case HIS_SORTEDNORMAL:
        case HIS_ICONNORMAL: case HIS_ICONSORTEDNORMAL:
            return TRUE;
    }
    Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry;
    Microsoft::WRL::ComPtr<ID2D1GeometrySink> sink;
    pRenderTarget->BeginDraw();

    g_d2dFactory->CreatePathGeometry(&geometry);
    geometry->Open(&sink);
    sink->BeginFigure(D2D1::Point2F(x, y), D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLine(D2D1::Point2F(width, y));
    sink->AddLine(D2D1::Point2F(width, height - cornerRadius));
    sink->AddArc(D2D1::ArcSegment(
        D2D1::Point2F(width - cornerRadius, height),
        D2D1::SizeF(cornerRadius, cornerRadius), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
    sink->AddLine(D2D1::Point2F(cornerRadius, height));
    sink->AddArc(D2D1::ArcSegment(
        D2D1::Point2F(x, height - cornerRadius),
        D2D1::SizeF(cornerRadius, cornerRadius), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
    sink->AddLine(D2D1::Point2F(x, y));
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
    pRenderTarget->CreateSolidColorBrush(fillColor, &brush);
    pRenderTarget->FillGeometry(geometry.Get(), brush.Get());

    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintPreviewPaneSeparator(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory || (iPartId != 3 && iPartId != 4))
        return FALSE;

    if (!g_themeCache.previewseparator[0])
        if (!g_themeCache.CachePreviewPaneSeparator())
            return FALSE;
    
    RECT rc{pRect->left+1, pRect->top, pRect->right, pRect->bottom};
    DrawNineGridStretch(hdc, g_themeCache.previewseparator[0], &rc, 1, 0, 0, 0);
    return TRUE;
}

BOOL CThemeCache::CachePreviewPaneSeparator()
{
    INT x = 0, y = 0;
    INT width = 3, height = 3;
    if(!g_themeCache.CreateDIB(g_themeCache.previewseparator[0], width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { x, y, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.previewseparator[0], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
    pRenderTarget->CreateSolidColorBrush(g_IsSysThemeDarkMode ? MyD2D1Color(128, 160, 160, 160) : MyD2D1Color(128, 0, 0, 0), &brush);

    pRenderTarget->BeginDraw();
    pRenderTarget->DrawLine(D2D1_POINT_2F(x, y), D2D1_POINT_2F(x, height), brush.Get());
    auto hr = pRenderTarget->EndDraw();

    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintModuleButton(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if ((iPartId != 3) || !g_d2dFactory)
        return FALSE;
    // Let windows theme paint its (transparent) buttons
    if (iStateId == 1 || iStateId == 6) return FALSE;
    INT index = iStateId - 2; 

    if (!g_themeCache.modulebutton[index])
        if (!g_themeCache.CacheModuleButton(iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_themeCache.modulebutton[index], pRect, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheModuleButton(INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT x = 0, y = 0;
    INT width = 18, height = 18;

    if(!g_themeCache.CreateDIB(g_themeCache.modulebutton[stateIndex], width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { x, y, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.modulebutton[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    D2D1_COLOR_F fillColor;
    D2D1_COLOR_F borderColor = MyD2D1Color(255, 255, 255);
    FLOAT Border2pxOffset = 0.f;
    switch (iStateId) 
    {
        case 2: fillColor = MyD2D1Color(96, 144, 144, 144);
            break;
        case 3: fillColor = MyD2D1Color(64, 144, 144, 144);
            break;
        case 4: Border2pxOffset = 1.f * scale;
            break;
        case 5: 
            fillColor = MyD2D1Color(96, 144, 144, 144);
            Border2pxOffset = 1.f * scale;
            break;
    }

    D2D1_ROUNDED_RECT roundedRect = {
        D2D1::RectF(Border2pxOffset, Border2pxOffset,
                    width -Border2pxOffset, height -Border2pxOffset) ,
        cornerRadius, cornerRadius
    };

    pRenderTarget->BeginDraw();
    if (iStateId != 4)
    {
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> fillBrush;
        pRenderTarget->CreateSolidColorBrush(fillColor, &fillBrush);
        pRenderTarget->FillRoundedRectangle(&roundedRect, fillBrush.Get());
    }
    if (iStateId == 4 || iStateId == 5)
    {
        Border2pxOffset += 1.f;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> borderBrush;
        pRenderTarget->CreateSolidColorBrush(borderColor, &borderBrush);
        pRenderTarget->DrawRoundedRectangle(&roundedRect, borderBrush.Get(), Border2pxOffset);
    }
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintModuleLocation(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if ((iPartId != 9) || !g_d2dFactory)
        return FALSE;
    if (iStateId == 6) return FALSE;
    INT index = iStateId - 1; 

    if (!g_themeCache.modulelocationbutton[index])
        if (!g_themeCache.CacheModuleLocationButton(iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_themeCache.modulelocationbutton[index], pRect, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheModuleLocationButton(INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT x = 0, y = 0;
    INT width = 18, height = 18;
    if(!g_themeCache.CreateDIB(g_themeCache.modulelocationbutton[stateIndex], width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { x, y, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.modulelocationbutton[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;
    
    D2D1_COLOR_F fillColor;
    D2D1_COLOR_F borderColor = MyD2D1Color(255, 255, 255);
    FLOAT Border2pxOffset = 0.f;
    switch (iStateId) 
    {
        case 1:
            fillColor = MyD2D1Color(96, 78, 78, 78);
            borderColor = MyD2D1Color(96, 112, 112, 112);
            break;
        case 2:
            fillColor = MyD2D1Color(96, 96, 96, 96);
            borderColor = MyD2D1Color(96, 144, 144, 144);
            break;
        case 3:
            fillColor = MyD2D1Color(96, 88, 88, 88);
            borderColor = MyD2D1Color(96, 80, 80, 80);
            break;
        case 4:
            Border2pxOffset = 1.f * scale;
            borderColor = MyD2D1Color(255, 255, 255);
            break;
    }

    D2D1_ROUNDED_RECT roundedRect = {
        D2D1::RectF(Border2pxOffset, Border2pxOffset,
                    width -Border2pxOffset, height -Border2pxOffset) ,
        cornerRadius, cornerRadius
    };

    pRenderTarget->BeginDraw();
    if (iStateId != 4)
    {
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> fillBrush;
        pRenderTarget->CreateSolidColorBrush(fillColor, &fillBrush);
        pRenderTarget->FillRoundedRectangle(&roundedRect, fillBrush.Get());
    }

    Border2pxOffset += 1.f;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> borderBrush;
    pRenderTarget->CreateSolidColorBrush(borderColor, &borderBrush);
    pRenderTarget->DrawRoundedRectangle(&roundedRect, borderBrush.Get(), Border2pxOffset);

    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintModuleSplitButton(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory || (iPartId != 4 && iPartId != 5))
        return FALSE;
    if (iStateId == 1 || iStateId == 6) return FALSE;
    INT index = (iPartId == 4) ? iStateId - 2 : iStateId + 2; 

    if (!g_themeCache.modulesplitbutton[index])
        if (!g_themeCache.CacheModuleSplitButton(iPartId, iStateId, index))
            return FALSE;
    RECT newRc = (iPartId == 4 && iStateId == 4) ? RECT{pRect->left, pRect->top, pRect->right+2, pRect->bottom} : *pRect;
    DrawNineGridStretch(hdc, g_themeCache.modulesplitbutton[index], &newRc, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheModuleSplitButton(INT iPartId, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT x = 0, y = 0;
    INT width = 18, height = 18;

    if(!g_themeCache.CreateDIB(g_themeCache.modulesplitbutton[stateIndex], width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { x, y, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.modulesplitbutton[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    D2D1_COLOR_F fillColor;
    if (iStateId == 2) fillColor = MyD2D1Color(96, 144, 144, 144);
    else if (iStateId == 3 || iStateId == 5) fillColor = MyD2D1Color(64, 144, 144, 144);
    if (iStateId == 4) {
        y = x += 1.f;
        width = height -= 1;
    }

    pRenderTarget->BeginDraw();
    if (iPartId == 4)
    {
        Microsoft::WRL::ComPtr<ID2D1PathGeometry> path;
        Microsoft::WRL::ComPtr<ID2D1GeometrySink> sink;
        g_d2dFactory->CreatePathGeometry(&path);
        path->Open(&sink);

        sink->BeginFigure(D2D1::Point2F(width, y), D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(D2D1::Point2F(x + cornerRadius, y));
        sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(x, cornerRadius + y), D2D1::SizeF(cornerRadius, cornerRadius),
            0.f,
            D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
            D2D1_ARC_SIZE_SMALL
        ));
        sink->AddLine(D2D1::Point2F(x, height - cornerRadius));
        sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(x + cornerRadius, height), D2D1::SizeF(cornerRadius, cornerRadius),
            0.f,
            D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
            D2D1_ARC_SIZE_SMALL
        ));
        sink->AddLine(D2D1::Point2F(width, height));
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
        sink->Close();

        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        if (iStateId == 2 || iStateId == 3 || iStateId == 5)
        {
            pRenderTarget->CreateSolidColorBrush(fillColor, &brush);
            pRenderTarget->FillGeometry(path.Get(), brush.Get());
        }
        else if (iStateId == 4)
        {
            pRenderTarget->CreateSolidColorBrush(MyD2D1Color(255, 255, 255), &brush);
            pRenderTarget->DrawGeometry(path.Get(), brush.Get(), 2.f * scale);
        }
    }
    else if (iPartId == 5)
    {
        Microsoft::WRL::ComPtr<ID2D1PathGeometry> path;
        Microsoft::WRL::ComPtr<ID2D1GeometrySink> sink;
        g_d2dFactory->CreatePathGeometry(&path);
        path->Open(&sink);

        sink->BeginFigure(D2D1::Point2F(x, y), D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(D2D1::Point2F(width - cornerRadius, y));
        sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(width, y + cornerRadius), D2D1::SizeF(cornerRadius, cornerRadius),
            0.f,
            D2D1_SWEEP_DIRECTION_CLOCKWISE,
            D2D1_ARC_SIZE_SMALL
        ));
        sink->AddLine(D2D1::Point2F(width, height - cornerRadius));
        sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(width - cornerRadius, height), D2D1::SizeF(cornerRadius, cornerRadius),
            0.f,
            D2D1_SWEEP_DIRECTION_CLOCKWISE,
            D2D1_ARC_SIZE_SMALL
        ));
        sink->AddLine(D2D1::Point2F(x, height));
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
        sink->Close();

        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        if (iStateId == 2 || iStateId == 3 || iStateId == 5)
        {
            pRenderTarget->CreateSolidColorBrush(fillColor, &brush);
            pRenderTarget->FillGeometry(path.Get(), brush.Get());
        }
        else if (iStateId == 4)
        {
            pRenderTarget->CreateSolidColorBrush(MyD2D1Color(255, 255, 255), &brush);
            pRenderTarget->DrawGeometry(path.Get(), brush.Get(), 2.f * scale);
        }
    }
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintNavigationButton(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory)
        return FALSE;
    INT index = (iPartId == NAV_BACKBUTTON) ? iStateId - 1 : (iPartId == NAV_FORWARDBUTTON) ? iStateId + 3 : iStateId + 7;

    if (!g_themeCache.navigationbutton[index])
        if (!g_themeCache.CacheNavigationButton(iPartId, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_themeCache.navigationbutton[index], pRect);
    return TRUE;
}

BOOL CThemeCache::CacheNavigationButton(INT iPartId, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT x = 0, y = 0;
    INT width = 30 * scale, height = 30 * scale;
    
    if (iPartId == NAV_MENUBUTTON)
        width = 13 * scale, height = 27 * scale;
    
    if(!g_themeCache.CreateDIB(g_themeCache.navigationbutton[stateIndex], width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { x, y, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.navigationbutton[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;
    D2D1_COLOR_F fillColor, arrowColor;
    switch (iStateId) 
    {
        case NAV_BB_NORMAL:
            arrowColor = g_IsSysThemeDarkMode ? MyD2D1Color(255, 255, 255) : MyD2D1Color(32, 32, 32);
            fillColor = MyD2D1Color(0, 0, 0, 0);
            break;
        case NAV_BB_HOT:
            fillColor = MyD2D1Color(32, 255, 255, 255);
            arrowColor = g_IsSysThemeDarkMode ? MyD2D1Color(200, 255, 255, 255) : MyD2D1Color(200, 32, 32, 32);
            break;
        case NAV_BB_PRESSED:
            fillColor = MyD2D1Color(16, 255, 255, 255);
            arrowColor = g_IsSysThemeDarkMode ?MyD2D1Color(200, 160, 160, 160) : MyD2D1Color(200, 96, 96, 96);
            break;
        case NAV_BB_DISABLED:
            arrowColor = MyD2D1Color(160, 64, 64, 64);
            fillColor = MyD2D1Color(0, 0, 0, 0);
            break;
    }

    D2D1_ROUNDED_RECT Rect = D2D1::RoundedRect(D2D1::RectF(0, 0, width, height), cornerRadius, cornerRadius);
    pRenderTarget->BeginDraw();

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> fillBrush;
    pRenderTarget->CreateSolidColorBrush(fillColor, &fillBrush);
    pRenderTarget->FillRoundedRectangle(&Rect, fillBrush.Get());

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> arrowBrush;
    pRenderTarget->CreateSolidColorBrush(arrowColor, &arrowBrush);

    if (iPartId == NAV_BACKBUTTON)
    {
        FLOAT centerY = height / 2.f;
        FLOAT tailLength = width / 2.5f;
        FLOAT tailStartX = width - (tailLength / 1.5f);
        FLOAT tailEndX = tailStartX - tailLength;

        FLOAT headSpand = tailLength * .5f;
        FLOAT headOffset = headSpand * 0.866f;

        pRenderTarget->DrawLine(
        D2D1::Point2F(tailStartX, centerY),
        D2D1::Point2F(tailEndX+1.5f, centerY),
        arrowBrush.Get(), 1.5f
        );
        
        pRenderTarget->DrawLine(
            D2D1::Point2F(tailEndX + headOffset, centerY + headOffset),
            D2D1::Point2F(tailEndX, centerY),
            arrowBrush.Get(), 1.5f
        );

        pRenderTarget->DrawLine(
            D2D1::Point2F(tailEndX + headOffset, centerY - headOffset),
            D2D1::Point2F(tailEndX, centerY),
            arrowBrush.Get(), 1.5f
        );  
    }
    else if (iPartId == NAV_FORWARDBUTTON)
    {
        FLOAT centerY = height / 2.f;
        FLOAT tailLength = width / 2.5f;
        FLOAT tailStartX = tailLength / 1.5f;
        FLOAT tailEndX = tailStartX + tailLength;

        FLOAT headSpand = tailLength * .5f;
        FLOAT headOffset = headSpand * 0.866f;

        pRenderTarget->DrawLine(
        D2D1::Point2F(tailStartX, centerY),
        D2D1::Point2F(tailEndX-1.5f, centerY),
        arrowBrush.Get(), 1.5f
        );
        
        pRenderTarget->DrawLine(
            D2D1::Point2F(tailEndX - headOffset, centerY - headOffset),
            D2D1::Point2F(tailEndX, centerY),
            arrowBrush.Get(), 1.5f
        );

        pRenderTarget->DrawLine(
            D2D1::Point2F(tailEndX - headOffset, centerY + headOffset),
            D2D1::Point2F(tailEndX, centerY),
            arrowBrush.Get(), 2.f
        );  
    }
    else if (iPartId == NAV_MENUBUTTON)
    {
        FLOAT centerX = width / 2.f;
        FLOAT centerY = height / 2.f;

        FLOAT arrowLength = std::min(width, height) * 0.33f;
        // 60 degree angle
        FLOAT dx = arrowLength * 0.866f;
        FLOAT dy = arrowLength * 0.5f;

        D2D1_POINT_2F ptTip   = D2D1::Point2F(centerX, centerY + dy);
        D2D1_POINT_2F ptLeft  = D2D1::Point2F(centerX - dx, centerY - dy);
        D2D1_POINT_2F ptRight = D2D1::Point2F(centerX + dx, centerY - dy);

        pRenderTarget->DrawLine(ptLeft, ptTip, arrowBrush.Get(), 2.f);
        pRenderTarget->DrawLine(ptRight, ptTip, arrowBrush.Get(), 2.f);
    }
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintToolbarButton(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory || (iPartId != TP_BUTTON && iPartId != TP_DROPDOWNBUTTON && iPartId != TP_SPLITBUTTON))
        return FALSE;
    if (iStateId == TS_NORMAL || iStateId == TS_DISABLED || iStateId == TS_NEARHOT) return FALSE;

    INT index = (iStateId == TS_HOTCHECKED) ? 0 : (iStateId == TS_PRESSED) ? 1 : (iStateId == TS_CHECKED) ? 2 : 3;

    if (!g_themeCache.toolbarbutton[index])
        if (!g_themeCache.CacheToolbarButton(iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_themeCache.toolbarbutton[index], pRect, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheToolbarButton(INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT width = 18, height = 18;

    if (!g_themeCache.CreateDIB(g_themeCache.toolbarbutton[stateIndex], width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.toolbarbutton[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;
    D2D1_COLOR_F fillColor = (iStateId == TS_HOT || iStateId == TS_OTHERSIDEHOT) ? MyD2D1Color(96, 144, 144, 144) :
                             (iStateId == TS_PRESSED || iStateId == TS_CHECKED) ? MyD2D1Color(64, 144, 144, 144) : MyD2D1Color(80, 144, 144, 144);

    pRenderTarget->BeginDraw();

    D2D1_ROUNDED_RECT Rect = D2D1::RoundedRect(D2D1::RectF(0, 0, width, height), cornerRadius, cornerRadius);
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> fillBrush;
    pRenderTarget->CreateSolidColorBrush(fillColor, &fillBrush);
    pRenderTarget->FillRoundedRectangle(&Rect, fillBrush.Get());

    if (iStateId == TS_HOTCHECKED || iStateId == TS_CHECKED)
    {
        FLOAT pillOffset = width * 0.2f;
        D2D1_COLOR_F pillColor = IsAccentColorPossibleD2D(105, 205, 255, SystemAccentColorLight2);
        fillBrush->SetColor(pillColor);
        pRenderTarget->DrawLine(D2D1::Point2F(pillOffset, height-1), D2D1::Point2F(width - pillOffset, height-1), fillBrush.Get(), 2.0f);
    }
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintToolbarSplitDropDown(HDC hdc, INT iPartId,  INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory || iPartId != TP_SPLITBUTTONDROPDOWN)
        return FALSE;
    
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT width = RECTWIDTH(pRect), height = RECTHEIGHT(pRect);

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    if (FAILED(CreateBoundD2DRenderTarget(hdc, pRect, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    D2D1_COLOR_F fillColor = (iStateId == TS_HOT || iStateId == TS_HOTCHECKED || iStateId == TS_OTHERSIDEHOT) ? MyD2D1Color(96, 144, 144, 144) : 
                             (iStateId == TS_PRESSED || iStateId == TS_CHECKED) ? MyD2D1Color(64, 144, 144, 144) : MyD2D1Color(0, 0, 0, 0);

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
    pRenderTarget->CreateSolidColorBrush(fillColor, &brush);
    
    D2D1_ROUNDED_RECT Rect = D2D1::RoundedRect(D2D1::RectF(1.f, 0.f, (FLOAT)width, (FLOAT)height),cornerRadius, cornerRadius);
    FLOAT centerX = width/2.f + 1;
    FLOAT centerY = height/2.f;

    FLOAT arrowLen = width * .25f;
    FLOAT dx = arrowLen * 0.707f;

    pRenderTarget->BeginDraw();

    pRenderTarget->FillRoundedRectangle(&Rect, brush.Get());
    if (iStateId == TS_DISABLED) 
        brush->SetColor(MyD2D1Color(64, 64, 64));
    else
        brush->SetColor( g_IsSysThemeDarkMode ?  MyD2D1Color(255, 255, 255) : MyD2D1Color(0, 0, 0));

    if (iStateId == TS_PRESSED) {
        pRenderTarget->DrawLine(D2D1::Point2F(centerX , centerY + arrowLen/2.f), D2D1::Point2F(centerX - dx , centerY - arrowLen/2.f), brush.Get(), scale * 1.5f);
        pRenderTarget->DrawLine(D2D1::Point2F(centerX , centerY + arrowLen/2.f), D2D1::Point2F(centerX + dx, centerY - arrowLen/2.f), brush.Get(), scale * 1.5f);
    }
    else {
        pRenderTarget->DrawLine(D2D1::Point2F(centerX + arrowLen/2.f, centerY), D2D1::Point2F(centerX - arrowLen/2.f, centerY - dx), brush.Get(), scale * 1.5f);
        pRenderTarget->DrawLine(D2D1::Point2F(centerX + arrowLen/2.f, centerY), D2D1::Point2F(centerX - arrowLen/2.f, centerY + dx), brush.Get(), scale * 1.5f);
    }

    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintAddressBand(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory || iPartId != 1)
        return FALSE;
    INT index = iStateId - 1;

    if (!g_themeCache.addressband[index])
        if (!g_themeCache.CacheAddressBand(iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_themeCache.addressband[index], pRect, 12, 12, 11, 11);
    return TRUE;
}

BOOL CThemeCache::CacheAddressBand(INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 5.f * scale;
    INT width = 24, height = 24;

    if (!g_themeCache.CreateDIB(g_themeCache.addressband[stateIndex], width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.addressband[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;
    D2D1_COLOR_F fillColor, borderColor;
    switch (iStateId) 
    {
        case 1:
            fillColor = MyD2D1Color(48, 96, 96, 96);
            borderColor = MyD2D1Color(64, 255, 255, 255);
            break;
        case 2:
            fillColor = MyD2D1Color(96, 96, 96, 96);
            borderColor = MyD2D1Color(64, 255, 255, 255);
            break;
        case 3:
            fillColor = MyD2D1Color(24, 96, 96, 96);
            borderColor = MyD2D1Color(64, 255, 255, 255);
            break;
        case 4:
            fillColor = g_IsSysThemeDarkMode ? MyD2D1Color(0, 0, 0) : MyD2D1Color(255, 255, 255);
            borderColor = IsAccentColorPossibleD2D(105, 205, 255, SystemAccentColorLight2);
            break;
    }
    D2D1_ROUNDED_RECT Rect = D2D1::RoundedRect(D2D1::RectF(0, 0, width, height), cornerRadius, cornerRadius);
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> fillBrush;
    pRenderTarget->CreateSolidColorBrush(fillColor, &fillBrush);

    pRenderTarget->BeginDraw();

    pRenderTarget->FillRoundedRectangle(&Rect, fillBrush.Get());
    fillBrush->SetColor(borderColor);
    pRenderTarget->DrawLine(D2D1::Point2F(cornerRadius/2, height-.5f), D2D1::Point2F(width-cornerRadius/2, height-.5f), fillBrush.Get());
    pRenderTarget->DrawLine(D2D1::Point2F(cornerRadius/2 - 1.5f, height-1.5f), D2D1::Point2F(width - cornerRadius/2 + 1.5f, height-1.5f), fillBrush.Get());

    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintMenu(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    // Part:14 (Win10) - Part:27 (Win11)
    if (!g_d2dFactory || (iPartId != 27 && iPartId != MENU_POPUPITEM && iPartId != MENU_BARITEM && iPartId != MENU_POPUPSEPARATOR))
        return FALSE;
    if ((iPartId == 27 || iPartId == MENU_POPUPITEM) && iStateId != 2) return FALSE;
    if ((iPartId == MENU_BARITEM) && 
        (iStateId == MBI_NORMAL || iStateId == MBI_DISABLED || iStateId == MBI_DISABLEDPUSHED)) return FALSE;

    INT index = (iPartId == MENU_POPUPSEPARATOR) ? 0 : (iPartId == 27 || iPartId == MENU_POPUPITEM) ? 1 : (iPartId == MENU_BARITEM && iStateId == MBI_PUSHED) ?  3 : 2;

    if (!g_themeCache.menuitem[index])
        if (!g_themeCache.CacheMenuItem(iPartId, iStateId, index))
            return FALSE;
    if (iPartId != MENU_POPUPSEPARATOR)
        DrawNineGridStretch(hdc, g_themeCache.menuitem[index], pRect, 9, 9, 8, 8);
    else
        DrawNineGridStretch(hdc, g_themeCache.menuitem[index], pRect, 1, 5, 0, 0);
    return TRUE;
}

BOOL CThemeCache::CacheMenuItem(INT iPartId, INT iStateId, INT indexState)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT width = 18, height = 18;

    if (iPartId == MENU_POPUPSEPARATOR) {
        width = 1;
        height = 5;
    }

    if (!g_themeCache.CreateDIB(g_themeCache.menuitem[indexState], width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.menuitem[indexState], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;
    
    pRenderTarget->BeginDraw();
    
    if (iPartId == MENU_POPUPITEM || iPartId == 27)
    {
        D2D1_COLOR_F fillColor = IsAccentColorPossibleD2D(0, 160, 255, SystemAccentColorLight1);
        D2D1_ROUNDED_RECT Rect = D2D1::RoundedRect(D2D1::RectF(0, 0, width, height), cornerRadius, cornerRadius);
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> fillBrush;
        pRenderTarget->CreateSolidColorBrush(fillColor, &fillBrush);
        pRenderTarget->FillRoundedRectangle(&Rect, fillBrush.Get());
    }
    else if (iPartId == MENU_POPUPSEPARATOR) {
        D2D1_COLOR_F lineColor = (g_IsSysThemeDarkMode) ? MyD2D1Color(96, 255, 255, 255) : MyD2D1Color(64, 0, 0, 0);
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> lineBrush;
        pRenderTarget->CreateSolidColorBrush(lineColor, &lineBrush);

        pRenderTarget->DrawLine({0, (FLOAT)height/2}, {(FLOAT)width, (FLOAT)height/2}, lineBrush.Get());
    }
    else {
        D2D1_COLOR_F fillColor = (iStateId == MBI_PUSHED) ? MyD2D1Color(64, 144, 144, 144) : MyD2D1Color(128, 96, 96, 96);
        D2D1_ROUNDED_RECT Rect = D2D1::RoundedRect(D2D1::RectF(0, 0, width, height), cornerRadius, cornerRadius);
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> fillBrush;
        pRenderTarget->CreateSolidColorBrush(fillColor, &fillBrush);
        pRenderTarget->FillRoundedRectangle(&Rect, fillBrush.Get());
    }
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintDragDrop(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory || iPartId != DD_IMAGEBG)
        return FALSE;

    if (!g_themeCache.dragdrop[0])
        if (!g_themeCache.CacheDragDrop())
            return FALSE;
    DrawNineGridStretch(hdc, g_themeCache.dragdrop[0], pRect);
    return TRUE;
}

BOOL CThemeCache::CacheDragDrop()
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    INT width = 108, height = 108;
    FLOAT cornerRadius = 4.f * scale;

    if (!g_themeCache.CreateDIB(g_themeCache.dragdrop[0], width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.dragdrop[0], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;
    
    D2D1_ROUNDED_RECT roundedRect = { D2D1::RectF(0, 0, width, height), cornerRadius, cornerRadius};
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
    pRenderTarget->BeginDraw();

    pRenderTarget->CreateSolidColorBrush(MyD2D1Color(128, 96, 96, 96), &brush);
    pRenderTarget->FillRoundedRectangle(&roundedRect, brush.Get());

    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintSpinArrowGlyph(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory)
        return FALSE;

    INT width = RECTWIDTH(pRect);
    INT height = RECTHEIGHT(pRect);

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    if (FAILED(CreateBoundD2DRenderTarget(hdc, pRect, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    D2D1_COLOR_F arrowColor =
        (iStateId == UPS_HOT)      ? MyD2D1Color(255, 255, 255) :
        (iStateId == UPS_PRESSED)  ? MyD2D1Color(128, 128, 128)  :
        (iStateId == UPS_DISABLED) ? MyD2D1Color(64, 64, 64)  :
                                     MyD2D1Color(192, 192, 192);

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> arrowBrush;
    pRenderTarget->CreateSolidColorBrush(arrowColor, &arrowBrush);

    FLOAT centerX = width / 2.f;
    FLOAT centerY = height / 2.f;
    FLOAT arrowLength = (iPartId == SPNP_UP || iPartId == SPNP_DOWN) ? std::min(width, height) * 0.5f
                        : std::min(width, height) * 0.3f;
    FLOAT dx = arrowLength * 0.866f;
    FLOAT dy = arrowLength * .5f;

    D2D1_POINT_2F ptTip, ptLeft, ptRight;

    if (iPartId == SPNP_UP) {
        ptTip   = { centerX,      centerY - dy };
        ptLeft  = { centerX - dx, centerY + dy };
        ptRight = { centerX + dx, centerY + dy };
    }
    else if (iPartId == SPNP_DOWN) {
        ptTip   = {centerX, centerY + dy};
        ptLeft  = {centerX - dx, centerY - dy};
        ptRight = {centerX + dx, centerY - dy};
    }
    else if (iPartId == SPNP_DOWNHORZ)
    {
        ptTip   = { centerX - dy, centerY };
        ptLeft  = { centerX + dy, centerY - dx };
        ptRight = { centerX + dy, centerY + dx };
    }
    else if (iPartId == SPNP_UPHORZ)
    {
        ptTip   = { centerX + dy, centerY };
        ptLeft  = { centerX - dy, centerY - dx };
        ptRight = { centerX - dy, centerY + dx };
    }

    pRenderTarget->BeginDraw();

    pRenderTarget->DrawLine(ptLeft, ptTip, arrowBrush.Get(), 1.5f);
    pRenderTarget->DrawLine(ptRight, ptTip, arrowBrush.Get(), 1.5f);
    
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintSpin(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory)
        return FALSE;

    INT index = (iStateId == UPS_HOT) ? 1 : (iStateId == UPS_PRESSED) ? 2
            : (iStateId == UPS_DISABLED) ? 3 : 0;

    index = (iPartId == SPNP_DOWNHORZ || iPartId == SPNP_UPHORZ) ? index + 4 : index;
    
    // Clean previous paintings
    PatBlt(hdc, pRect->left, pRect->top, RECTWIDTH(pRect), RECTHEIGHT(pRect), BLACKNESS);

    if (!g_themeCache.spin[index])
        if (!g_themeCache.CacheSpinButton(iPartId, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_themeCache.spin[index], pRect, 6, 5, 6, 5);

    // Custom glyphs aren't cached due to no image stretching, draw them at runtime
    if (PaintSpinArrowGlyph(hdc, iPartId, iStateId, pRect))
        return TRUE;
    else {
        // Erase any previous custom drawing
        PatBlt(hdc, pRect->left, pRect->top, RECTWIDTH(pRect), RECTHEIGHT(pRect), BLACKNESS);
        return FALSE;
    }
}

BOOL CThemeCache::CacheSpinButton(INT iPartId, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    INT width = 12, height = 12;
    FLOAT cornerRadius = 2.f * scale;

    if (!g_themeCache.CreateDIB(g_themeCache.spin[stateIndex], width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.spin[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;
    
    D2D1_ROUNDED_RECT roundedRect = {{0.f, 0.f, (FLOAT)width, (FLOAT)height}, cornerRadius, cornerRadius};
    D2D1_RECT_F Rect = {0.f, 0.f, (FLOAT)width, (FLOAT)height};

    D2D1_COLOR_F fillColor =
        (iStateId == UPS_HOT)      ? MyD2D1Color(128, 96, 96, 96) :
        (iStateId == UPS_PRESSED)  ? MyD2D1Color(180, 60, 60, 60)  :
        (iStateId == UPS_DISABLED) ? MyD2D1Color(160, 0, 0, 0)  :
                                     MyD2D1Color(96, 80, 80, 80);

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> fillBrush;
    pRenderTarget->CreateSolidColorBrush(fillColor, &fillBrush);

    pRenderTarget->BeginDraw();

    if (iPartId == SPNP_UP || iPartId == SPNP_DOWN)
        pRenderTarget->FillRectangle(&Rect, fillBrush.Get());
    else
        pRenderTarget->FillRoundedRectangle(&roundedRect, fillBrush.Get());

    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

HRESULT WINAPI HookedDrawThemeBackground(
    HTHEME hTheme,
    HDC hdc,
    INT iPartId,
    INT iStateId,
    LPCRECT pRect,
    LPCRECT pClipRect)
{       
    std::wstring ThemeClassName = GetThemeClass(hTheme);

    if (ThemeClassName == L"ScrollBar")
    {
        if (PaintScroll(hdc, iPartId, iStateId, pRect))
            return S_OK;
        else if (PaintScrollBarArrows(hdc, iPartId, iStateId, pRect))
            return S_OK;
    }
    else if (ThemeClassName == L"Button")
    {
        if (PaintPushButton(hdc, iPartId, iStateId, pRect, pClipRect))
            return S_OK;
        else if (PaintRadioButton(hdc, iPartId, iStateId, pRect))
            return S_OK;
        else if (PaintCheckBox(hdc, iPartId, iStateId, pRect))
            return S_OK;
        else if (PaintCommandLink(hdc, iPartId, iStateId, pRect))
            return S_OK;
        else if (PaintCommandLinkGlyph(hdc, iPartId, iStateId, pRect))
            return S_OK;
        else if (iPartId == BP_GROUPBOX)
        {
            HTHEME hThemeGroupBox = nullptr;
            if (hTheme == SetThemeHandle(WindowFromDC(hdc), hThemeGroupBox, L"Button"))
            {
                if (PaintGroupBox(hdc, iPartId, iStateId, pRect, pClipRect)) {
                    CloseThemeData(hThemeGroupBox);
                    return S_OK;
                }
            }
            
            if (hThemeGroupBox)
                CloseThemeData(hThemeGroupBox);
        }
    }
    else if (ThemeClassName == L"Tab")
    {
        if (PaintTab(hdc, iPartId, iStateId, pRect))
            return S_OK;
    }
    else if (ThemeClassName == L"ComboBox")
    {
        // The Win32 address bar uses both the "Combobox" and "ComboBox" theme classes along with other classes
        // ComboBox is used when the address bar is selected, while combobox is used when the drop-down window is open
        if (SanitizeAddressCombobox(hTheme, hdc, iPartId, iStateId))
            return S_OK;
        else if (PaintDropDownArrow(hdc, iPartId, iStateId, pRect, TRUE))
            return S_OK;
    }
    else if (ThemeClassName == L"Combobox")
    {
        if (PaintCombobox(hdc, iPartId, iStateId, pRect))
            return S_OK;
        else if (PaintDropDownArrow(hdc, iPartId, iStateId, pRect, FALSE))
            return S_OK;
    }
    else if (ThemeClassName == L"Listbox")
    {
        if (PaintListBox(hdc, iPartId, iStateId, pRect))
            return S_OK;
    }
    else if (ThemeClassName == L"Edit")
    {
        if (PaintEditBox(hTheme, hdc, iPartId, iStateId, pRect))
            return S_OK;
    }
    else if (ThemeClassName == L"TrackBar")
    {
        if (PaintTrackbar(hdc, iPartId, iStateId, pRect))
            return S_OK;
        if (PaintTrackbarThumb(hdc, iPartId, iStateId, pRect))
            return S_OK;
        else if (PaintTrackBarPointedThumb(hdc, iPartId, iStateId, pRect))
            return S_OK;
    }
    else if (ThemeClassName == L"Progress")
    {
        // The exported GetThemeClass function does not provide
        // full string of theme class names of derived theme classes
        // Use the OpenThemeData API instead.
        HTHEME hThemeProgress = NULL;
        if (hTheme == SetThemeHandle(WindowFromDC(hdc), hThemeProgress, L"Indeterminate::Progress"))
        {
            if (PaintIndeterminateProgressBar(hdc, iPartId, iStateId, pRect))
            {
                CloseThemeData(hThemeProgress);
                return S_OK;
            }
            CloseThemeData(hThemeProgress);
        }
        else if (PaintProgressBar(hdc, iPartId, iStateId, pRect)) 
        {
            CloseThemeData(hThemeProgress);
            return S_OK;
        }
        if (hThemeProgress)
            CloseThemeData(hThemeProgress);
    } 
    else if (ThemeClassName == L"ListView")
    {
        if (PaintListView(hdc, iPartId, iStateId, pRect))
            return S_OK;
    }
    else if (ThemeClassName == L"TreeView")
    {
        if (PaintTreeViewButton(hdc, iPartId, iStateId, pRect))
            return S_OK;
        if (PaintTreeViewGlyph(hdc, iPartId, iStateId, pRect))
            return S_OK;
    }
    else if (ThemeClassName == L"Header")
    {
        if (PaintHeader(hdc, iPartId, iStateId, pRect))
            return S_OK;
    }
    else if (ThemeClassName == L"Navigation")
    {
        if (PaintNavigationButton(hdc, iPartId, iStateId, pRect))
            return S_OK;
    }
    else if (ThemeClassName == L"Toolbar")
    {
        HTHEME hThemeToolbar = NULL;
        if (SetThemeHandle(WindowFromDC(hdc), hThemeToolbar, L"BB::Toolbar"))
        {
            if (PaintToolbarSplitDropDown(hdc, iPartId, iStateId, pRect)) {
                CloseThemeData(hThemeToolbar);
                return S_OK;
            }
        }
        if (PaintToolbarButton(hdc, iPartId, iStateId, pRect))
            return S_OK;
    }
    else if (ThemeClassName == L"AddressBand" || ThemeClassName == L"SearchBox")
    {
        if (PaintAddressBand(hdc, iPartId, iStateId, pRect))
            return S_OK;
    }
    else if (ThemeClassName == L"Menu")
    {
        if (PaintMenu(hdc, iPartId, iStateId, pRect))
            return S_OK;
        // Force menu white glyphs
        else if (iPartId <= MENU_SYSTEMRESTORE && iPartId >= MENU_SYSTEMCLOSE && g_IsSysThemeDarkMode) {
            HTHEME hThemeMenu = NULL;
            if (SetThemeHandle(WindowFromDC(hdc), hThemeMenu, L"DarkMode::Menu")) {
                auto hr = DrawThemeBackground_orig(hThemeMenu, hdc, iPartId, iStateId, pRect, pClipRect);
                CloseThemeData(hThemeMenu);
                return hr;
            }
        }
    }
    else if (ThemeClassName == L"DragDrop")
    {
        if (PaintDragDrop(hdc, iPartId, iStateId, pRect))
            return S_OK;
    }
    else if (ThemeClassName == L"Spin")
    {
        if (PaintSpin(hdc, iPartId, iStateId, pRect))
            return S_OK;
    }

    HRESULT hr = DrawThemeBackground_orig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
    
    if((ThemeClassName == L"Rebar" && (iPartId == RP_BAND || iPartId == RP_BACKGROUND) && iStateId == 0)
        || (ThemeClassName == L"Header" && (iPartId == THEMECLS_COMMONPROPS_PART || (iPartId == HP_HEADERITEM && (iStateId == HIS_NORMAL || iStateId == HIS_SORTEDNORMAL || iStateId == HIS_ICONNORMAL))))
        || (ThemeClassName == L"TaskDialog" && iPartId == TDLG_FOOTNOTEPANE && iStateId == 0)
        || (ThemeClassName == L"Tab" && iPartId == TABP_PANE)
        || (ThemeClassName == L"Status" && iPartId == THEMECLS_COMMONPROPS_PART)
        || (ThemeClassName == L"Tooltip" && (iPartId == TTP_STANDARD || iPartId == TTP_BALLOON || iPartId == TTP_BALLOONSTEM)))
    {
        FillRect(hdc, pRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        return S_OK;
    }
    else if (ThemeClassName == L"Menu" && (iPartId == MENU_BARBACKGROUND || iPartId == MENU_BARITEM))
    {
        RECT clipRect{*pRect};
        if (pClipRect)
            IntersectRect(&clipRect, pRect, pClipRect);
        FillRect(hdc, &clipRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        return S_OK;
    }
    else if (ThemeClassName == L"Menu" && (iPartId == MENU_POPUPBACKGROUND || iPartId == MENU_POPUPBORDERS || iPartId == MENU_POPUPGUTTER || 
        iPartId == MENU_POPUPCHECKBACKGROUND || ((iPartId == MENU_POPUPITEM || iPartId == 27) && iStateId != MPI_HOT)))
    {
        RECT clipRect{*pRect};
        if (pClipRect)
            IntersectRect(&clipRect, pRect, pClipRect);
        if (g_settings.FlyoutsEffects)
            FillRect(hdc, &clipRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        else if (g_settings.FillBg) {
            HBRUSH brush = CreateSolidBrush(RGB(32, 32, 32));
            FillRect(hdc, &clipRect, brush);
            DeleteObject(brush);
        }
        return S_OK;
    }
    else if (ThemeClassName == L"Toolbar" && iPartId == THEMECLS_COMMONPROPS_PART) {
        HTHEME hThemeToolbar = nullptr;
        if ((SetThemeHandle(WindowFromDC(hdc), hThemeToolbar, L"Placesbar::Toolbar"))) {
            FillRect(hdc, pRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
            CloseThemeData(hThemeToolbar);
            return S_OK;
        }
    }
    return hr;
}

HRESULT WINAPI HookedDrawThemeBackgroundEx(
    HTHEME hTheme,
    HDC hdc,
    INT iPartId,
    INT iStateId,
    LPCRECT pRect,
    const DTBGOPTS* pOptions)
{    
    std::wstring ThemeClassName = GetThemeClass(hTheme);

    if (ThemeClassName == L"ListView")
    {
        if (PaintListView(hdc, iPartId, iStateId, pRect))
            return S_OK;
    }
    else if (ThemeClassName == L"Edit")
    {
        if (PaintEditBox(hTheme ,hdc, iPartId, iStateId, pRect))
            return S_OK;
    }
    else if (ThemeClassName == L"Button")
    {
        if (PaintPushButton(hdc, iPartId, iStateId, pRect, &pOptions->rcClip))
            return S_OK;
        else if (PaintRadioButton(hdc, iPartId, iStateId, pRect))
            return S_OK;
        else if (PaintCheckBox(hdc, iPartId, iStateId, pRect))
            return S_OK;
        else if (PaintCommandLink(hdc, iPartId, iStateId, pRect))
            return S_OK;
        else if (PaintCommandLinkGlyph(hdc, iPartId, iStateId, pRect))
            return S_OK;
    }
    else if (ThemeClassName == L"ItemsView")
    {
        if (PaintItemsView(hdc, iPartId, iStateId, pRect))
            return S_OK;
    }
    else if (ThemeClassName == L"Header")
    {
        if (PaintHeader(hdc, iPartId, iStateId, pRect))
            return S_OK;
    }
    else if (ThemeClassName == L"PreviewPane")
    {
        if (PaintPreviewPaneSeparator(hdc, iPartId, iStateId, pRect))
            return S_OK;
    }
    else if (ThemeClassName == L"Progress")
    {
        HTHEME hThemeProgress = NULL;
        if (hTheme == SetThemeHandle(WindowFromDC(hdc), hThemeProgress, L"Indeterminate::Progress"))
        {
            if (PaintIndeterminateProgressBar(hdc, iPartId, iStateId, pRect))
            {
                CloseThemeData(hThemeProgress);
                return S_OK;
            }
        }
        else if (PaintProgressBar(hdc, iPartId, iStateId, pRect)) 
        {
            CloseThemeData(hThemeProgress);
            return S_OK;
        }

        if (hThemeProgress)
            CloseThemeData(hThemeProgress);
    } 
    else if (ThemeClassName == L"CommandModule")
    {
        if (PaintModuleButton(hdc, iPartId, iStateId, pRect))
            return S_OK;
        else if (PaintModuleSplitButton(hdc, iPartId, iStateId, pRect))
            return S_OK;
        else if (PaintModuleLocation(hdc, iPartId, iStateId, pRect))
            return S_OK;
    }

    HRESULT hr = DrawThemeBackgroundEx_orig(hTheme, hdc, iPartId, iStateId, pRect, pOptions);

    if ((ThemeClassName == L"Rebar" && (iPartId == RP_BAND || iPartId == RP_BACKGROUND) && iStateId == 0) 
        || (ThemeClassName == L"TreeView" && iPartId == THEMECLS_COMMONPROPS_PART))
    {
        return S_OK;    
    }
    else if ((ThemeClassName == L"PreviewPane" && iPartId == 1)
        || (ThemeClassName == L"Header" && iPartId == THEMECLS_COMMONPROPS_PART)
        || (ThemeClassName == L"CommandModule" && iPartId == 1 && iStateId == 0)
        || (ThemeClassName == L"TaskDialog" && (iPartId == TDLG_CONTENTPANE || iPartId == TDLG_FOOTNOTESEPARATOR ||  iPartId == TDLG_FOOTNOTEPANE || iPartId == TDLG_SECONDARYPANEL) && iStateId == 0)
        || (ThemeClassName == L"TaskDialog" && iPartId == TDLG_PRIMARYPANEL)
        || (ThemeClassName == L"AeroWizard" && (iPartId == AW_TITLEBAR || iPartId == AW_HEADERAREA || iPartId == AW_CONTENTAREA || iPartId == AW_COMMANDAREA))
        || (ThemeClassName == L"CommonItemsDialog" && iPartId == 1)
        || (ThemeClassName == L"ControlPanel" && (iPartId == CPANEL_CONTENTPANE || iPartId == CPANEL_CONTENTPANELINE || iPartId == CPANEL_BANNERAREA || iPartId == CPANEL_LARGECOMMANDAREA || iPartId == CPANEL_SMALLCOMMANDAREA)))
    {
        FillRect(hdc, pRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        return S_OK;
    }
    return hr;
}

// Remove white rect below menubar (e.g. seen in mmc.exe)
HRESULT WINAPI HookedDrawThemeEdge(HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, const RECT *pDestRect, UINT uEdge, UINT uFlags, RECT *pContentRect)
{
    std::wstring ThemeClass = GetThemeClass(hTheme);

    if (ThemeClass == L"Rebar" && iPartId == RP_BAND) {
        FillRect(hdc, pContentRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        return S_OK;
    }

    return DrawThemeEdge_orig(hTheme, hdc, iPartId, iStateId, pDestRect, uEdge, uFlags, pContentRect);
}

HRESULT WINAPI HookedGetThemeMargins(HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, INT iPropId, RECT* prc, MARGINS *pMargins)
{
    std::wstring ThemeClassName = GetThemeClass(hTheme);

    auto ret = GetThemeMargins_orig(hTheme, hdc, iPartId, iStateId, iPropId, prc, pMargins);

    if (ThemeClassName == L"Tooltip" && iPartId == TTP_STANDARD) {
        if (iPropId == TMT_CONTENTMARGINS)
            *pMargins = {8, 8, 8, 8};
        else if (iPropId == TMT_CAPTIONMARGINS)
            *pMargins = {10, 10, 10, 10};
    }
    else if (ThemeClassName == L"Menu")
    {
        if (iPartId == MENU_POPUPITEM || iPartId == 27 || iPartId == 26) 
        {
            if (iPropId == TMT_CONTENTMARGINS)
                *pMargins = {2, 2, 4, 4};
            else if (iPropId == TMT_SIZINGMARGINS)
                *pMargins = {10, 10, 10, 10};
            else if (iPropId == 10000)
                *pMargins = {0, 0, 4, 4};
        }
        else if (iPartId == MENU_BARITEM) {
            if (iPropId == 10000)
                *pMargins = {9, 9, 3, 3};
        }
    }
    else if (ThemeClassName == L"Edit")
    {
        if (iPropId == TMT_SIZINGMARGINS)
            *pMargins = {8, 8, 8, 8};
    }   
    
    return ret;
}

HRESULT WINAPI HookedGetThemeFont (HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, INT iPropId, LOGFONTW* pFont)
{
    auto hr = GetThemeFont_orig(hTheme, hdc, iPartId, iStateId, iPropId, pFont);
    std::wstring ThemeClassName = GetThemeClass(hTheme);
    
    if (ThemeClassName == L"Menu" && iPropId == TMT_FONT) 
    {
        // Return if it's not the original font
        if (wcscmp(pFont->lfFaceName, L"Segoe UI"))
            return hr;
        wcscpy_s(pFont->lfFaceName, LF_FACESIZE, L"Segoe UI Variable Small");
        pFont->lfHeight = -13;
        pFont->lfWeight = 400;
        pFont->lfQuality = CLEARTYPE_QUALITY;
        pFont->lfPitchAndFamily = DEFAULT_PITCH;
    }
    else if (ThemeClassName == L"ControlPanelStyle" && iPartId == CPANEL_TITLE && iPropId == TMT_FONT) {
        wcscpy_s(pFont->lfFaceName, LF_FACESIZE, L"Segoe UI Variable Display Semib");
        pFont->lfHeight = -24;
    }
    return hr;
}

//https://github.com/ALTaleX531/TranslucentFlyouts/blob/master/TFMain/EffectHelper.hpp
// Required for flyouts with DWM SYSTEMBACKDROP effects
VOID TriggerWindowNCRendering(HWND hwnd)
{
    // NOTICE WINDOWS THAT WE HAVE ACTIVATED THE WINDOW
    DefWindowProcW(hwnd, WM_NCACTIVATE, TRUE, 0);
    //SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_DRAWFRAME | SWP_NOACTIVATE);
}

VOID DwmMakeWindowTransparent(HWND hwnd)
{
    DWM_BLURBEHIND bb{ DWM_BB_ENABLE | DWM_BB_BLURREGION | DWM_BB_TRANSITIONONMAXIMIZED, TRUE, CreateRectRgn(0, 0, -1, -1), TRUE };
    DwmEnableBlurBehindWindow(hwnd, &bb);
    DeleteObject(bb.hRgnBlur);
}

VOID EnableBlurBehind(HWND hWnd)
{
    // Does not interfere with the Windows Terminal, GameBar overlay
    if(!(IsWindowClass(hWnd, L"CASCADIA_HOSTING_WINDOW_CLASS") || IsWindowClass(hWnd, L"ApplicationFrameWindow")))
    {
        ACCENT_POLICY accentPolicy = {};
        WINCOMPATTRDATA winCompositionAttrib = {};
        DWM_BLURBEHIND dwmBlurBehindData = { };

        dwmBlurBehindData.fEnable = TRUE;
        dwmBlurBehindData.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION | DWM_BB_TRANSITIONONMAXIMIZED;
        // Blurs window client area
        HRGN hRgn = CreateRectRgn(0, 0, -1, -1);
        dwmBlurBehindData.hRgnBlur = hRgn;
        dwmBlurBehindData.fTransitionOnMaximized = TRUE;

        DwmEnableBlurBehindWindow(hWnd, &dwmBlurBehindData);
        DeleteObject(hRgn);

        accentPolicy.AccentState = ACCENT_STATE_ENABLE_ACRYLICBLURBEHIND;
        accentPolicy.GradientColor = g_settings.AccentBlurBehindClr;

        winCompositionAttrib.Attrib = WCA_ACCENT_POLICY;
        winCompositionAttrib.pvData = &accentPolicy;
        winCompositionAttrib.cbData = sizeof(accentPolicy);

        if (SetWindowCompositionAttribute)
            SetWindowCompositionAttribute(hWnd, &winCompositionAttrib);    
    }
}

static LRESULT WINAPI HookedDefWindowProcW(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{    
    if (msg == WM_SETTINGCHANGE) 
    {
        // System theme change
        if (lParam && wcscmp((LPCWSTR)lParam, L"ImmersiveColorSet") == 0) 
        {
            // Fetch the actual current theme file path from registry
            std::wstring currentTheme = GetCurrentWindowsThemePath();

            AcquireSRWLockExclusive(&g_ThemeChangeLock);

            if (currentTheme != g_LastThemePath) 
            {
                g_LastThemePath = currentTheme; 

                // Process the theme change
                g_themeCache.ClearCache();
                g_IsSysThemeDarkMode = ShouldSystemUseDarkMode();
                g_AccentPalette.LoadAccentPalette();
                
                if (g_settings.AccentColorize)
                    g_settings.AccentColorize = GetAccentColor(g_settings.AccentColor);

                AcquireSRWLockExclusive(&g_SysColorsLock);
                for (HBRUSH& brush : g_themeCachedCustomSysColorBrushes) {
                    if (brush) { 
                        DeleteObject(brush); brush = nullptr; 
                    }
                }
                ReleaseSRWLockExclusive(&g_SysColorsLock);
            }
            
            ReleaseSRWLockExclusive(&g_ThemeChangeLock);
        }
    }   

    if (IsWindowClass(hWnd, L"ViewControlClass") && msg == WM_NCPAINT) {
        UINT borderType = DWMWCP_ROUND;
        DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &borderType, sizeof(UINT));
    }
    return DefWindowProc_orig(hWnd, msg, wParam, lParam);
}

VOID HandleEffects(HWND hWnd)
{
    BOOL isFlyoutWindow = isWindowFlyout(hWnd);

    if (g_IsSysThemeDarkMode) 
        DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &ENABLE, sizeof(UINT));

    if(g_settings.BgType == g_settings.AccentBlurBehind)
        EnableBlurBehind(hWnd);
    else if (g_settings.BgType > g_settings.AccentBlurBehind)
    {
        if (isFlyoutWindow) {
            DwmMakeWindowTransparent(hWnd);
            TriggerWindowNCRendering(hWnd);
        }
        DwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &g_settings.BgType, sizeof(UINT));
    }

    if (!isFlyoutWindow && g_settings.BgType != g_settings.Default) {
        MARGINS margins = {-1, -1, -1, -1};
        DwmExtendFrameIntoClientArea(hWnd, &margins);
    }
    
    if (isFlyoutWindow) {
        UINT borderType = DWMWCP_ROUND;
        DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &borderType, sizeof(UINT));
    }
    
    return;
}

VOID NewWindowShown(HWND hWnd) 
{
    if(!IsWindowEligible(hWnd))
        return;        
    //else
        //Wh_Log(L"Eligible window: %p", hWnd);
    HandleEffects(hWnd);
}

VOID DwmExpandFrameIntoClientAreaHook() {
    WindhawkUtils::SetFunctionHook(DwmExtendFrameIntoClientArea, HookedDwmExtendFrameIntoClientArea, &DwmExtendFrameIntoClientArea_orig);
}

VOID DwmSetWindowAttributeHook() {
    WindhawkUtils::SetFunctionHook(DwmSetWindowAttribute, HookedDwmSetWindowAttribute, &DwmSetWindowAttribute_orig); 
}

HRESULT WINAPI HookedGetThemeTransitionDuration(HTHEME hTheme, INT iPartId, INT iStateIdFrom, INT iStateIdTo, INT iPropId, DWORD *pdwDuration)
{
    auto hr = GetThemeTransitionDuration_orig(hTheme, iPartId, iStateIdFrom, iStateIdTo, iPropId, pdwDuration);
    std::wstring ThemeClassStr = GetThemeClass(hTheme);
    
    if (ThemeClassStr == L"ScrollBar" && (iPartId == SBP_ARROWBTN || iPartId == SBP_THUMBBTNHORZ || iPartId == SBP_THUMBBTNVERT) && iStateIdTo == SCRBS_NORMAL)
        *pdwDuration = 40;
    else if (ThemeClassStr == L"ScrollBar" && (iPartId == SBP_ARROWBTN && iStateIdFrom == SCRBS_HOT && iStateIdTo == SCRBS_HOVER))
        *pdwDuration = 40;
    
    return hr;
}

LRESULT (STDCALL *CThemeMenu_MenuKeyboardMsgProc_orig)(INT, WPARAM, LPARAM);
LRESULT STDCALL HookedCThemeMenu_MenuKeyboardMsgProc_orig(INT code, WPARAM wParam, LPARAM lParam)
{
    auto res = CThemeMenu_MenuKeyboardMsgProc_orig(code, wParam, lParam);

    #ifdef _WIN64
        INT archOffset = 1;
    #else
        INT archOffset = 2;
    #endif

    UINT msg = *reinterpret_cast<DWORD*>(lParam + 16 / archOffset);
    HWND hWnd = *reinterpret_cast<HWND*>(lParam + 24 / archOffset);

    if (IsWindowClass(hWnd, MENUPOPUP_CLASS) && (msg == WM_NCPAINT || msg == WM_PRINT))
        HandleEffects(hWnd);

    return res;
}

HRESULT (__fastcall *_GetBrushesForPart_orig)(HTHEME, INT, COLORREF, HBITMAP*, HBRUSH*);
HRESULT __fastcall Hooked_GetBrushesForPart(HTHEME hTheme, INT iPartId, COLORREF Color, HBITMAP *phBitmap, HBRUSH *phBrush)
{
   std::wstring ThemeClass = GetThemeClass(hTheme);
    
    if (ThemeClass == L"Tab" && (iPartId == TABP_BODY || iPartId == TABP_AEROWIZARDBODY)) {
        if (!*phBrush || *phBrush != GetSysColorBrush(COLOR_WINDOW)) {
            *phBrush = GetSysColorBrush(COLOR_WINDOW);
            return S_OK;
        }
    }
    
    return _GetBrushesForPart_orig(hTheme, iPartId, Color, phBitmap, phBrush);
}

void (__fastcall *_BorderRect_orig)(HDC, COLORREF, LPRECT, INT, INT);
void __fastcall Hooked_BorderRect(HDC hdc, COLORREF color, LPRECT pRect, INT cxThickness, INT cyThickness)
{
    if (!pRect) return;

    auto ComposeRectBorders = [&](RECT rcBorder)
    {
        BP_PAINTPARAMS params = { sizeof(BP_PAINTPARAMS) };
        params.dwFlags = BPPF_ERASE | BPPF_NONCLIENT;
        HDC memDC = NULL;

        HPAINTBUFFER hpb = BeginBufferedPaint(hdc, &rcBorder, BPBF_TOPDOWNDIB, &params, &memDC);
        if (!hpb) {
            Wh_Log(L"Failed BeginBufferedPaint error:0x%08x", GetLastError());
            return _BorderRect_orig(hdc, color, pRect, cxThickness, cyThickness);
        }

        SetBkColor(memDC, color);
        ExtTextOutW(memDC, pRect->left, pRect->top, ETO_OPAQUE, pRect, NULL, NULL, NULL);

        BufferedPaintSetAlpha(hpb, pRect, 255);
        EndBufferedPaint(hpb, TRUE);
    };

    RECT rcBorder;
    // 1. Bottom Border
    rcBorder = *pRect;
    rcBorder.top = rcBorder.bottom - cyThickness;
    ComposeRectBorders(rcBorder);

    // 2. Right Border
    rcBorder = *pRect;
    rcBorder.left = rcBorder.right - cxThickness;
    ComposeRectBorders(rcBorder);

    // 3. Left Border
    rcBorder = *pRect;
    rcBorder.right = rcBorder.left + cxThickness;
    ComposeRectBorders(rcBorder);

    // 4. Top Border
    rcBorder = *pRect;
    rcBorder.bottom = rcBorder.top + cyThickness;
    ComposeRectBorders(rcBorder);
    return;
}

VOID UxThemeHooks(BOOL isFlyoutEffectEnabled)
{
    WindhawkUtils::SYMBOL_HOOK uxtheme_dll_hooks[] =
    {   
        // Inlined symbol in ARM64 system, avoid hooking.
        #ifndef _M_ARM64
        {
            {
                #ifdef _WIN64
                    L"void __cdecl _BorderRect(struct HDC__ *,unsigned long,struct tagRECT const *,int,int)"
                #else
                    L"void __stdcall _BorderRect(struct HDC__ *,unsigned long,struct tagRECT const *,int,int)"
                #endif
            },
            &_BorderRect_orig,
            Hooked_BorderRect,
            FALSE
        },
        #endif
        {
            {
                #ifdef _WIN64
                    L"long __cdecl _GetBrushesForPart(void *,int,int,struct HBITMAP__ * *,struct HBRUSH__ * *)"
                #else
                    L"long __stdcall _GetBrushesForPart(void *,int,int,struct HBITMAP__ * *,struct HBRUSH__ * *)"
                #endif
            },
            &_GetBrushesForPart_orig,
            Hooked_GetBrushesForPart,
            FALSE
        },
        
        {
            {
                #ifdef _WIN64
                    L"protected: static __int64 __cdecl CThemeMenu::MenuKeyboardMsgProc(int,unsigned __int64,__int64)"
                #else
                    L"protected: static long __stdcall CThemeMenu::MenuKeyboardMsgProc(int,unsigned int,long)"
                #endif
            },
            &CThemeMenu_MenuKeyboardMsgProc_orig,
            HookedCThemeMenu_MenuKeyboardMsgProc_orig,
            FALSE
        },
    };

    HMODULE hUxTheme = LoadLibraryEx(L"uxtheme.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hUxTheme) {
        Wh_Log(L"Failed to load uxtheme.dll");
        return;
    }

    // If flyout effects setting isn't enabled hook to all symbols except the last one -> CThemeMenu::MenuKeyboardMsgProc
    if (!WindhawkUtils::HookSymbols(hUxTheme, uxtheme_dll_hooks, !isFlyoutEffectEnabled ? ARRAYSIZE(uxtheme_dll_hooks) - 1 : ARRAYSIZE(uxtheme_dll_hooks))) {
        Wh_Log(L"Failed to hook one or more symbol functions in uxtheme.dll");
        return;
    }
}

VOID RestoreWindowCustomizations(HWND hWnd)
{
    if(!IsWindowEligible(hWnd))
        return;

    ACCENT_POLICY accentPolicy = {};
    WINCOMPATTRDATA winCompositionAttrib = {};
    DWM_BLURBEHIND dwmBlurBehindData = {};

    // Disabling AccentBlurBehind temp workaround
    dwmBlurBehindData.fEnable = FALSE;
    dwmBlurBehindData.dwFlags = DWM_BB_ENABLE;
    DwmEnableBlurBehindWindow(hWnd, &dwmBlurBehindData);

    accentPolicy.AccentState = ACCENT_STATE_DISABLED;

    winCompositionAttrib.Attrib = WCA_ACCENT_POLICY;
    winCompositionAttrib.pvData = &accentPolicy;
    winCompositionAttrib.cbData = sizeof(accentPolicy);

    if (SetWindowCompositionAttribute)
        SetWindowCompositionAttribute(hWnd, &winCompositionAttrib);
    
    DWM_SYSTEMBACKDROP_TYPE backdrop = DWMSBT_NONE;
    DwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE , &backdrop, sizeof(UINT));

    // Manually restore frame extension
    if(!(IsWindowClass(hWnd,  L"TaskManagerWindow") && g_settings.BgType != g_settings.Default))
    {
        MARGINS margins = { 0, 0, 0, 0 };
        DwmExtendFrameIntoClientArea(hWnd, &margins);
    }
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) 
{
    DWORD dwProcessId = 0;
    // Pass console window, it might be called from other processes like Clink:https://github.com/chrisant996/clink
    if ((!GetWindowThreadProcessId(hWnd, &dwProcessId) || dwProcessId != GetCurrentProcessId()) && !IsWindowClass(hWnd, L"ConsoleWindowClass")) 
        return TRUE;
    else
    {
        HWND hParentWnd = GetAncestor(hWnd, GA_PARENT);
        if (hParentWnd && hParentWnd != GetDesktopWindow())
            return TRUE;
        else if(g_settings.Unload)
            RestoreWindowCustomizations(hWnd);
        else
            NewWindowShown(hWnd);
    }
    return TRUE;
}

VOID ApplyForExistingWindows()
{
    EnumWindows(EnumWindowsProc, 0);
}

BOOL GetColorSetting(LPCWSTR hexColor, COLORREF& outColor) 
{
    if (!hexColor)
        return FALSE;
    if (hexColor[0] == L'0' && hexColor[1] == L'\0')
    {
        outColor = DWMWA_COLOR_NONE;
        return TRUE;
    }
    else if (hexColor[0] == L'1' && hexColor[1] == L'\0') 
    {
        outColor = DWMWA_COLOR_DEFAULT;
        return TRUE;
    }
    else if (hexColor[0] == L'2' && hexColor[1] == L'\0') 
    {
        if (g_settings.AccentColorize)
        {
            outColor =  g_settings.AccentColor;
            return TRUE;
        }
        if (GetAccentColor(outColor))
        {
            g_settings.AccentColor = outColor;
            return TRUE;
        }
        return FALSE;
    }
    else 
    {
        size_t len = wcslen(hexColor);
        if (len != 6 && len != 8)
        {
            Wh_Log(L"[ERROR] Invalid color length");
            return FALSE;
        }
        
        auto hexToByte = [](WCHAR c) -> INT {
            if (c >= L'0' && c <= L'9') return c - L'0';
            if (c >= L'A' && c <= L'F') return 10 + (c - L'A');
            if (c >= L'a' && c <= L'f') return 10 + (c - L'a');
            return -1;
        };

        BYTE alpha = 0x00;
        BYTE rgb[3] = { 0 };

        if (len == 8) 
        {
            alpha = 0XFF;
            INT alphaHigh = hexToByte(hexColor[0]);
            INT alphaLow  = hexToByte(hexColor[1]);
            if (alphaHigh < 0 || alphaLow < 0)
                return FALSE;
            alpha = (alphaHigh << 4) | alphaLow;
            hexColor += 2;
        }

        for (INT i = 0; i < 3; ++i) 
        {
            INT high = hexToByte(hexColor[i * 2]);
            INT low  = hexToByte(hexColor[i * 2 + 1]);
            if (high < 0 || low < 0)
                return FALSE;
            rgb[i] = (high << 4) | low;
        }

        outColor = (alpha << 24) | (rgb[2] << 16) | (rgb[1] << 8) | rgb[0];
        return TRUE;
    }
}

// ---------------------------------------------------------------------------------------------
// User32.dll internal operations in most cases use the gpsi pointer (global pointer shared info) in order to fetch useful attributes about the system session
// one of them being the system color buffer, gpsi pointing to offest 4568 to system COLORREFs and offset 4696 to system BRUSHES
//
// Kernel operations (e.g. win32kfull.sys) use: W32GetUserSessionState() + offset (<20016> as of Win11 26200.8655) + <System color offset>
//
// System color offsets:
//
//     -System colors-                      -System brushes-
//
// 4568: COLOR_SCROLLBAR                4696: COLOR_SCROLLBAR
// 4572: COLOR_BACKGROUND               4704: COLOR_BACKGROUND
// 4576: COLOR_ACTIVECAPTION            4712: COLOR_ACTIVECAPTION
// 4580: COLOR_INACTIVECAPTION          4720: COLOR_INACTIVECAPTION
// 4584: COLOR_MENU                     4728: COLOR_MENU
// 4588: COLOR_WINDOW                   4736: COLOR_WINDOW
// 4592: COLOR_WINDOWFRAME              4744: COLOR_WINDOWFRAME
// 4596: COLOR_MENUTEXT                 4752: COLOR_MENUTEXT
// 4600: COLOR_WINDOWTEXT               4760: COLOR_WINDOWTEXT
// 4604: COLOR_CAPTIONTEXT              4768: COLOR_CAPTIONTEXT
// 4608: COLOR_ACTIVEBORDER             4776: COLOR_ACTIVEBORDER
// 4612: COLOR_INACTIVEBORDER           4784: COLOR_INACTIVEBORDER
// 4616: COLOR_APPWORKSPACE             4792: COLOR_APPWORKSPACE
// 4620: COLOR_HIGHLIGHT                4800: COLOR_HIGHLIGHT
// 4624: COLOR_HIGHLIGHTTEXT            4808: COLOR_HIGHLIGHTTEXT
// 4628: COLOR_BTNFACE                  4816: COLOR_BTNFACE
// 4632: COLOR_BTNSHADOW                4824: COLOR_BTNSHADOW
// 4636: COLOR_GRAYTEXT                 4832: COLOR_GRAYTEXT
// 4640: COLOR_BTNTEXT                  4840: COLOR_BTNTEXT
// 4644: COLOR_INACTIVECAPTIONTEXT      4848: COLOR_INACTIVECAPTIONTEXT
// 4648: COLOR_BTNHIGHLIGHT             4856: COLOR_BTNHIGHLIGHT
// 4652: COLOR_3DDKSHADOW               4864: COLOR_3DDKSHADOW
// 4656: COLOR_3DLIGHT                  4872: COLOR_3DLIGHT
// 4660: COLOR_INFOTEXT                 4880: COLOR_INFOTEXT
// 4664: COLOR_INFOBK                   4888: COLOR_INFOBK
// 4668: COLOR_HOTLIGHT                 4896: COLOR_HOTLIGHT
// 4672: COLOR_GRADIENTACTIVECAPTION    4904: COLOR_GRADIENTACTIVECAPTION
// 4676: COLOR_GRADIENTACTIVECAPTION    4912: COLOR_GRADIENTACTIVECAPTION
// 4680: COLOR_MENUHIGHLIGHT            4920: COLOR_MENUHIGHLIGHT
// 4688: COLOR_MENUBAR                  4928: COLOR_MENUBAR
// ---------------------------------------------------------------------------------------------

// A considerable portion of coloring comes from CTL messages, replace the gpsi pointer inside user32 functions with system colors API getters
LRESULT MyRealDefWindowProcWorker(UINT msg, WPARAM wParam)
{
    COLORREF sysColorBk = 0;
    COLORREF sysColorTxt = 0;
    HBRUSH sysBrush = nullptr;
    if (msg == WM_CTLCOLOR || msg == WM_CTLCOLOREDIT || msg == WM_CTLCOLORLISTBOX)
    {
        sysColorBk = GetSysColor(COLOR_WINDOW);                                 // Default: *gpsi + 4588 (COLOR_WINDOW)
        sysColorTxt = g_IsSysThemeDarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0); // Default: *gpsi + 4600 (COLOR_WINDOWTEXT)
        sysBrush = GetSysColorBrush(COLOR_WINDOW);                              // Default: *gpsi + 4736 (COLOR_WINDOW)
    }
    else if (msg == WM_CTLCOLORMSGBOX || msg == WM_CTLCOLORDLG || msg == WM_CTLCOLORSTATIC)
    {
        sysColorBk = GetSysColor(COLOR_BTNFACE);                                // Default: *gpsi + 4628 (COLOR_BTNTEXT)
        sysColorTxt = g_IsSysThemeDarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0); // Default: *gpsi + 4600 (COLOR_WINDOWTEXT)
        sysBrush = GetSysColorBrush(COLOR_BTNFACE);                             // Default: *gpsi + 4816 (COLOR_BTNTEXT)
    }
    else if (msg == WM_CTLCOLORBTN)
    {
        sysColorBk = GetSysColor(COLOR_BTNFACE);                                // Default: *gpsi + 4628 (COLOR_BTNTEXT)
        sysColorTxt = g_IsSysThemeDarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0); // Default: *gpsi + 4816 (COLOR_BTNTEXT)
        sysBrush = GetSysColorBrush(COLOR_BTNFACE);                             // Default: *gpsi + 4816 (COLOR_BTNTEXT)
    }
    else if (msg == WM_CTLCOLORSCROLLBAR)
    {
        sysColorBk = GetSysColor(COLOR_BTNHIGHLIGHT);                           // Default: *gpsi + 4648 (COLOR_BTNHIGHLIGHT)
        sysColorTxt = g_IsSysThemeDarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0); // Default: *gpsi + 4840 (COLOR_BTNTEXT)
        sysBrush = GetSysColorBrush(COLOR_BTNHIGHLIGHT);                        // Default: *gpsi + 4856 (COLOR_BTNHIGHLIGHT)
    }

    HDC hdc = reinterpret_cast<HDC>(wParam);

    SetTextColor(hdc, sysColorTxt);
    SetBkColor(hdc, sysColorBk);
    return reinterpret_cast<LRESULT>(sysBrush);
}

#ifdef _WIN64
    LRESULT (__fastcall *RealDefWindowProcWorker_orig)(struct tagWND*, UINT, WPARAM, LPARAM, UINT);
    LRESULT __fastcall HookedRealDefWindowProcWorker(struct tagWND* pwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT flags)
    {
        switch (msg)
        {
            case WM_CTLCOLOR:
            case WM_CTLCOLORMSGBOX:
            case WM_CTLCOLOREDIT:
            case WM_CTLCOLORLISTBOX:
            case WM_CTLCOLORBTN:
            case WM_CTLCOLORDLG:
            case WM_CTLCOLORSCROLLBAR:
            case WM_CTLCOLORSTATIC:
            {
                LRESULT res = MyRealDefWindowProcWorker(msg, wParam);
                return res;
            }
        }

        return RealDefWindowProcWorker_orig(pwnd, msg, wParam, lParam, flags);
    }
#else
    LRESULT (__fastcall *RealDefWindowProcWorker_orig)(UINT, WPARAM, struct tagWND*, UINT, LPARAM, UINT);
    LRESULT __fastcall HookedRealDefWindowProcWorker(UINT msg, WPARAM wParam, struct tagWND* pwnd, UINT msg_dup, LPARAM lParam, UINT flags)
    {
        switch (msg)
        {
            case WM_CTLCOLOR:
            case WM_CTLCOLORMSGBOX:
            case WM_CTLCOLOREDIT:
            case WM_CTLCOLORLISTBOX:
            case WM_CTLCOLORBTN:
            case WM_CTLCOLORDLG:
            case WM_CTLCOLORSCROLLBAR:
            case WM_CTLCOLORSTATIC:
            {
                LRESULT res = MyRealDefWindowProcWorker(msg, wParam);
                return res;
            }
        }

        return RealDefWindowProcWorker_orig(msg, wParam, pwnd, msg_dup, lParam, flags);
    }
#endif

// Paints the background of message boxes
HBRUSH (STDCALL *MB_DlgProc_orig)(HWND, UINT, WPARAM, LPARAM);
HBRUSH STDCALL Hooked_MB_DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_CTLCOLORDLG || msg == WM_CTLCOLORSTATIC)
        return GetSysColorBrush(COLOR_WINDOW); // Default: gpsi + 4736 (COLOR_WINDOW)
    return MB_DlgProc_orig(hwnd, msg, wParam, lParam);
}

// Paints the lower part of message boxes
void (THISCALL *DrawCommandRectangle_orig)(HWND);
void THISCALL Hooked_DrawCommandRectangle(HWND hWnd)
{
    PAINTSTRUCT ps{};
    HDC hdc = BeginPaint(hWnd, &ps);
    if (!hdc)
        return;

    // Get window or client rectangle
    RECT rc{};
    GetClientRect(hWnd, &rc);

    // Match USER behavior
    HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW)); // Default: gpsi + 4736 (COLOR_WINDOW)
    HGDIOBJ oldBrush = SelectObject(hdc, hBrush);

    HPEN hPen = CreatePen(PS_NULL, 0, 0);
    HGDIOBJ oldPen = SelectObject(hdc, hPen);

    Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

    // Restore
    SelectObject(hdc, oldPen);
    DeleteObject(hPen);

    SelectObject(hdc, oldBrush);
    DeleteObject(hBrush);

    EndPaint(hWnd, &ps);
}

// Modifying the background and text color of the classic Win32 tooltip (e.g., the one that appears when hovering the pointer over title bar buttons) which uses the gpsi pointer.
// Additionally, enlarging the tooltip window and applying mod's effects.
void (__fastcall *RenderTooltip_orig)(HWND, HDC, HGDIOBJ*);
void __fastcall HookedRenderTooltip(HWND hWnd, HDC hdc, HGDIOBJ *a3)
{
    HGDIOBJ oldObj = SelectObject(hdc, a3[1]);
    
    LPCWCHAR lpString = reinterpret_cast<LPCWCHAR>(*a3);
    UINT cch = wcslen(lpString); 

    SIZE textSize;
    GetTextExtentPoint32W(hdc, lpString, static_cast<INT>(cch), &textSize);

    RECT clientRect {0};
    GetClientRect(hWnd, &clientRect);

    // Inflate tooltip window rect by x1.8
    if (clientRect.bottom < static_cast<LONG>(textSize.cy * 1.8)) 
    {
        RECT winRect {0};
        GetWindowRect(hWnd, &winRect);
        
        INT newWidth = static_cast<INT>((winRect.right - winRect.left) * 1.8);
        INT newHeight = static_cast<INT>((winRect.bottom - winRect.top) * 1.8);

        // Start with the X and Y coordinates the system originally gave it
        INT newX = winRect.left;
        INT newY = winRect.top;

        // Screen Edge Detection
        POINT cursorPos;
        GetCursorPos(&cursorPos);
        
        // Find out exactly which monitor the cursor is currently on
        HMONITOR hMonitor = MonitorFromPoint(cursorPos, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi = { sizeof(mi) };
        GetMonitorInfoW(hMonitor, &mi);

        // Check if our new width pushes it past the right edge of this monitor
        if ((newX + newWidth) > mi.rcMonitor.right)
            // Shift X leftwards so the right edge of the tooltip matches the screen edge
            // (Subtracting an extra 2 pixels so it doesn't touch the absolute physical bezel)
            newX = mi.rcMonitor.right - newWidth - 2;

        // Optional bonus: Do the same check for the bottom edge, just in case
        if ((newY + newHeight) > mi.rcMonitor.bottom)
            newY = mi.rcMonitor.bottom - newHeight - 2;

        // Apply the new position and dimensions
        SetWindowPos(hWnd, NULL, newX, newY, newWidth, newHeight,
                     SWP_NOZORDER | SWP_NOACTIVATE
                     );
        
        GetClientRect(hWnd, &clientRect);
    }

    COLORREF oldTextClr = SetTextColor(hdc, g_IsSysThemeDarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0)); // Default: gpsi + 4660 (COLOR_WINDOWTEXT)
    COLORREF oldBkClr = SetBkColor(hdc, RGB(0, 0, 0)); // Default: gpsi + 4888 (COLOR_INFOTEXT)

    INT textX = (clientRect.right - textSize.cx) / 2;
    INT textY = (clientRect.bottom - textSize.cy) / 2;
    
    if (textX < 0) textX = 2;
    if (textY < 0) textY = 1;

    ExtTextOutW(hdc, textX, textY, ETO_OPAQUE, &clientRect, lpString, cch, 0);
    
    SetTextColor(hdc, oldTextClr);
    SetBkColor(hdc, oldBkClr);
    SelectObject(hdc, oldObj);
    
    return;
}

VOID User32Hooks(BOOL areSysColorsApplied)
{
    WindhawkUtils::SYMBOL_HOOK user32_dll_hooks[] =
    {
        {
            {
                #ifdef _WIN64
                    L"__int64 __cdecl RealDefWindowProcWorker(struct tagWND *,unsigned int,unsigned __int64,__int64,unsigned long)"
                #else
                    L"long __stdcall RealDefWindowProcWorker(struct tagWND *,unsigned int,unsigned int,long,unsigned long)"
                #endif
            },
            &RealDefWindowProcWorker_orig,
            HookedRealDefWindowProcWorker,
            FALSE
        },
        {
            {
                #ifdef _WIN64
                    L"void __cdecl RenderTooltip(struct HWND__ *,struct HDC__ *,struct TooltipInfo *)"
                #else
                    L"void __stdcall RenderTooltip(struct HWND__ *,struct HDC__ *,struct TooltipInfo *)"
                #endif
            },
            &RenderTooltip_orig,
            HookedRenderTooltip,
            FALSE
        },
        {
            {
                #ifdef _WIN64
                    L"__int64 __cdecl MB_DlgProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
                #else
                    L"int __stdcall MB_DlgProc(struct HWND__ *,unsigned int,unsigned int,long)"
                #endif

            },
            &MB_DlgProc_orig,
            Hooked_MB_DlgProc,
            FALSE
        },
        {
            {
                #ifdef _WIN64
                    L"void __cdecl DrawCommandRectangle(struct HWND__ *)"
                #else
                    L"void __stdcall DrawCommandRectangle(struct HWND__ *)"
                #endif

            },
            &DrawCommandRectangle_orig,
            Hooked_DrawCommandRectangle,
            FALSE
        }
    };

    HMODULE hUser32 = LoadLibraryEx(L"user32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hUser32) {
        Wh_Log(L"Failed to load user32.dll");
        return;
    }

    // If SetSysColors API is executed then hook only the first two symbols of the array -> RealDefWindowProcWorker, RenderTooltip routines
    if (!WindhawkUtils::HookSymbols(hUser32, user32_dll_hooks, areSysColorsApplied ? 2 : ARRAYSIZE(user32_dll_hooks))) {
        Wh_Log(L"Failed to hook one or more symbol functions in user32.dll");
        return;
    }
}

void (__fastcall *SHThemeDrawText_orig)(void*, HDC, INT, INT, DTTOPTS*, LPCWSTR, LPRECT, INT, UINT, INT, __int64, COLORREF, COLORREF);
void __fastcall Hooked_SHThemeDrawText(void *a1, HDC a2, INT a3, INT a4, DTTOPTS *a5, LPCWSTR lpString, LPRECT lprc, INT a8, UINT format, INT a10, __int64 a11, COLORREF color, COLORREF a13)
{
    if (!g_InsideTaskMgrProc)
        color = g_IsSysThemeDarkMode && (color & 0x00ffffff) <= RGB(96, 96, 96) ? RGB(255, 255, 255) : !g_IsSysThemeDarkMode ? RGB(0, 0, 0) : color;
    
    SHThemeDrawText_orig(a1, a2, a3, a4, a5, lpString, lprc, a8, format, a10, a11, color, a13);
    return;

}

// Alpha blend highlighted text rectangle
void (__fastcall *SHThemeFillTextRect_orig)(HDC, LPRECT, COLORREF, INT);
void __fastcall HookedSHThemeFillTextRect(HDC hDC, LPRECT lprc, COLORREF color, INT sysColorCode)
{
    if (color != GetSysColor(COLOR_HIGHLIGHT))
        return SHThemeFillTextRect_orig(hDC, lprc, color, sysColorCode);
    
    BP_PAINTPARAMS params = { sizeof(BP_PAINTPARAMS) };
    params.dwFlags = BPPF_ERASE;

    HDC memDC = nullptr;
    HPAINTBUFFER hpb = BeginBufferedPaint(hDC, lprc, BPBF_TOPDOWNDIB, &params, &memDC); 
    if (!hpb) {
        Wh_Log(L"Failed BeginBufferedPaint error:0x%08x", GetLastError());
        return SHThemeFillTextRect_orig(hDC, lprc, color, sysColorCode); 
    }

    SHThemeFillTextRect_orig(memDC, lprc, color, sysColorCode);

    BufferedPaintMakeOpaque(hpb, lprc);
    EndBufferedPaint(hpb, TRUE); 
}

// Alpha blend highlighted text rectangle
COLORREF (__fastcall *FillRectClr_orig)(HDC, LPRECT, COLORREF);
COLORREF __fastcall HookedFillRectClr(HDC hdc, LPRECT lprect, COLORREF color)
{
    if (color != GetSysColor(COLOR_HIGHLIGHT))
        return FillRectClr_orig(hdc, lprect, color);
        
    BP_PAINTPARAMS params = { sizeof(BP_PAINTPARAMS) };
    params.dwFlags = BPPF_ERASE;

    HDC memDC = nullptr;
    HPAINTBUFFER hpb = BeginBufferedPaint(hdc, lprect, BPBF_TOPDOWNDIB, &params, &memDC);
    if (!hpb) {
        Wh_Log(L"Failed BeginBufferedPaint error:0x%08x", GetLastError());
        return FillRectClr_orig(hdc, lprect, color);
    }

    HBRUSH highlightedBrush = GetSysColorBrush(COLOR_HIGHLIGHT);
    FillRect(memDC, lprect, highlightedBrush);

    BufferedPaintMakeOpaque(hpb, lprect);
    EndBufferedPaint(hpb, TRUE); 
    
    return color;
}

// As of Win11 26200.8655 - Listbox background fill color return COLOR_WINDOW system color
// We return white color so the black text inside listboxes are readable on system light theme.
HBRUSH (__fastcall *ListBox_GetBrush_orig)(struct tagLBIV*, HBRUSH*);
HBRUSH __fastcall HookedListBox_GetBrush(struct tagLBIV *a1, HBRUSH *hbr)
{   
    // Default return brush: GetSysColorBrush(COLOR_WINDOW)
    HBRUSH ret = g_IsSysThemeDarkMode ? ListBox_GetBrush_orig(a1, hbr) : (HBRUSH)GetStockObject(WHITE_BRUSH);
    return ret;
}

// As of Win11 26200.8655 - The ComboBox control (e.g., the Windows address bar) draws an internal rectangle using a brush with the COLOR_WINDOW system color.
// Since the custom COLOR_WINDOW is black, whereas the rest of the ComboBox control's background is intended to be drawn in white,
// we intervene to correct this behavior in light theme mode.
void (__fastcall *ComboEx_OnDrawItem_orig)(struct COMBOEX*, struct tagDRAWITEMSTRUCT*);
void __fastcall HookedComboEx_OnDrawItem(struct COMBOEX *a1, struct tagDRAWITEMSTRUCT *a2)
{
    if (!g_IsSysThemeDarkMode) 
    {
        InflateRect(&a2->rcItem, 1, 1);
        FillRect(a2->hDC, &a2->rcItem, (HBRUSH)GetStockObject(WHITE_BRUSH));
        InflateRect(&a2->rcItem, -1, -1);
    }
    ComboEx_OnDrawItem_orig(a1, a2);
    return;
}

VOID Comctl32Hooks()
{
    WindhawkUtils::SYMBOL_HOOK comctl32_dll_hooks[] =
    {        
        {
            {
                #ifdef _WIN64
                    L"SHThemeDrawText"
                #else
                    L"_SHThemeDrawText@56"
                #endif
            },
            &SHThemeDrawText_orig,
            Hooked_SHThemeDrawText,
            FALSE
        },
        // Symbols are inlined in ARM64, avoid hooking.
        #ifndef _M_ARM64
            // SHThemeFillTextRect is 64-bit only
            // 32-bit version is implemented inside SHThemeDrawText
            #ifdef _WIN64
            {
                {
                    L"SHThemeFillTextRect"
                },
                &SHThemeFillTextRect_orig,
                HookedSHThemeFillTextRect,
                FALSE
            },
            #endif
            {
                {
                    #ifdef _WIN64
                        L"FillRectClr"
                    #else
                        L"_FillRectClr@12"
                    #endif
                },
                &FillRectClr_orig,
                HookedFillRectClr,
                FALSE
            },
            {
                {
                    #ifdef _WIN64
                        L"struct HBRUSH__ * __cdecl ListBox_GetBrush(struct tagLBIV *,struct HBRUSH__ * *)"
                    #else
                        L"struct HBRUSH__ * __stdcall ListBox_GetBrush(struct tagLBIV *,struct HBRUSH__ * *)"
                    #endif
                },
                &ListBox_GetBrush_orig,
                HookedListBox_GetBrush,
                FALSE
            },
        #endif
        {
            {
                #ifdef _WIN64
                    L"void __cdecl ComboEx_OnDrawItem(struct COMBOEX *,struct tagDRAWITEMSTRUCT *)"
                #else
                    L"void __stdcall ComboEx_OnDrawItem(struct COMBOEX *,struct tagDRAWITEMSTRUCT *)"
                #endif
            },
            &ComboEx_OnDrawItem_orig,
            HookedComboEx_OnDrawItem,
            FALSE
        },           
    };

    HMODULE hComCtl32 = LoadComCtlModule();
    if (!hComCtl32) {
        Wh_Log(L"Failed to load comctl32.dll");
        return;
    }

    if (!WindhawkUtils::HookSymbols(hComCtl32, comctl32_dll_hooks, ARRAYSIZE(comctl32_dll_hooks))) {
        Wh_Log(L"Failed to hook one or more symbol functions in comctl32.dll");
        return;
    }
}

BOOL CThemeCache::CacheNavigationDivider()
{
    FLOAT x = 0, y = 0;
    FLOAT width = 2, height = 2;

    if(!g_themeCache.CreateDIB(g_themeCache.navigationdivider[0], width, height))
        return FALSE;

    RECT rc = {(INT)x, (INT)y, (INT)width, (INT)height};
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    if (FAILED(CreateBoundD2DRenderTarget(g_themeCache.navigationdivider[0], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
    pRenderTarget->CreateSolidColorBrush(g_IsSysThemeDarkMode ? MyD2D1Color(96, 160, 160, 160) : MyD2D1Color(96, 0, 0, 0), &brush);

    pRenderTarget->BeginDraw();

    // 0.5f makes stroke height 1px
    pRenderTarget->DrawLine(D2D1_POINT_2F(x, y + .5f), D2D1_POINT_2F(width, y + .5f), brush.Get());

    HRESULT hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

// Render custom D2D alpha blended navigation pane divider
void (THISCALL *CNscTree_DrawDivider_orig)(class CNscTree* , HDC, struct _TREEITEM*);
void THISCALL Hooked_CNscTree_DrawDivider(CNscTree *__this, HDC hdc, struct _TREEITEM *hTreeItem)
{
    auto Fallback = [&](LPCWSTR errorMessage = NULL) {
        if (errorMessage)
            Wh_Log(L"%s", errorMessage);
        CNscTree_DrawDivider_orig(__this, hdc, hTreeItem);
        return;
    };

    // As of Win11 26200.8737 - hwnd offsets 64-bit: offset 50, 32-bit: offset 61
    auto GetTreeViewHWND = [&__this]() 
    {
        #ifdef _WIN64
            return reinterpret_cast<HWND*>(__this)[50];
        #else
            return reinterpret_cast<HWND>(reinterpret_cast<DWORD*>(__this)[61]);
        #endif
    };

    // TreeView window handle retrieved from NtUserCreateWindowEx hook
    HWND hwndTreeView = tl_hwndTreeView;
    if (!IsWindow(hwndTreeView) || !IsWindowClass(hwndTreeView, L"SysTreeView32"))
        hwndTreeView = GetTreeViewHWND();
    
    if (!IsWindow(hwndTreeView) || !IsWindowClass(hwndTreeView, L"SysTreeView32"))
        return Fallback(L"Not valid TreeView window");
    
    RECT treeItemRect = {0};
    *reinterpret_cast<HTREEITEM*>(&treeItemRect) = (HTREEITEM)hTreeItem;
    
    if (!SendMessageW(hwndTreeView, TVM_GETITEMRECT, 0, (LPARAM)&treeItemRect))
        return Fallback();
    
    if (!g_d2dFactory)
        return Fallback();

    if (!g_themeCache.navigationdivider[0] && !g_themeCache.CacheNavigationDivider())
        return Fallback();
    
    RECT lineRc = treeItemRect;
    INT middlePoint = RECTHEIGHT(&treeItemRect) / 4.f;
    lineRc.top = treeItemRect.top + middlePoint - 1;
    lineRc.bottom = treeItemRect.top + middlePoint + 1;
    lineRc.left = treeItemRect.left + RECTWIDTH(&treeItemRect) * 0.05f; // default horizontal bounds offset
    lineRc.right = treeItemRect.right - lineRc.left;

    DrawNineGridStretch(hdc, g_themeCache.navigationdivider[0], &lineRc, 1, 1, 0, 0);
    return;
}

VOID ExplorerFrameHooks()
{
    WindhawkUtils::SYMBOL_HOOK explorerframe_dll_hooks[] =
    {
        {
            {
                #ifdef _WIN64
                    L"private: void __cdecl CNscTree::DrawDivider(struct HDC__ *,struct _TREEITEM *)"
                #else
                    L"private: void __thiscall CNscTree::DrawDivider(struct HDC__ *,struct _TREEITEM *)"
                #endif
            },
            &CNscTree_DrawDivider_orig,
            Hooked_CNscTree_DrawDivider,
            FALSE
        },          
    };

    HMODULE hExplorerFrame = LoadLibraryEx(L"ExplorerFrame.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hExplorerFrame) {
        Wh_Log(L"Failed to load ExplorerFrame.dll");
        return;
    }

    if (!WindhawkUtils::HookSymbols(hExplorerFrame, explorerframe_dll_hooks, ARRAYSIZE(explorerframe_dll_hooks))) {
        Wh_Log(L"Failed to hook one or more symbol functions in ExplorerFrame.dll");
        return;
    }
}

// Branding images are painted using TransparentBlt() with white transparency mask.
// Paint everything except the image text into white in order to force transparency to the background.
void RecolorBrandingLogoBackground(HBITMAP hbm)
{
    if (!hbm) 
        return;

    BITMAP bm{};
    if (!GetObject(hbm, sizeof(bm), &bm)) 
        return;

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = bm.bmWidth;
    bmi.bmiHeader.biHeight      = bm.bmHeight;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC hdc = CreateCompatibleDC(nullptr);
    if (!hdc) 
        return;

    size_t pixels = static_cast<size_t>(bm.bmWidth) * bm.bmHeight;
    
    auto px = std::make_unique<COLORREF[]>(pixels);

    if (GetDIBits(hdc, hbm, 0, bm.bmHeight, px.get(), &bmi, DIB_RGB_COLORS))
    {
        // Check if the first pixel is RGB(240, 240, 240). If not, it's probably a custom image -> abort.
        if ((px[0] & 0x00FFFFFF) == 0x00F0F0F0)
        {
            // Target color to keep: RGB(0, 120, 212)
            constexpr COLORREF targetColor = 0x000078D4; 

            for (size_t i = 0; i < pixels; ++i)
            {
                if ((px[i] & 0x00FFFFFF) != targetColor)
                    px[i] = 0xFFFFFFFF; // Turn white
                else
                    px[i] = g_settings.AccentColorize ?
                            // bgr -> rgb 
                            (0xFF000000 | ((g_settings.AccentColor & 0x0000FF) << 16) | (g_settings.AccentColor & 0x00FF00) | ((g_settings.AccentColor & 0xFF0000) >> 16))
                            : px[i];
            }
            SetDIBits(hdc, hbm, 0, bm.bmHeight, px.get(), &bmi, DIB_RGB_COLORS);
        }
    }
    DeleteDC(hdc); 
}

// Intercept the windows branding logo image (e.g winver, shutdown dialog, regedit etc.)
// Winver loads the bitmap using LoadAboutBitmaps() routine, shutdown dialog using LoadBrandingBitmap().
HANDLE (STDCALL *BrandingLoadImage_orig)(LPCWSTR, UINT, UINT, INT, INT, UINT);
HANDLE STDCALL HookedBrandingLoadImage(LPCWSTR pszBrand, UINT uID, UINT type, INT cx, INT cy, UINT  fuLoad)
{   
    auto hImage = BrandingLoadImage_orig(pszBrand, uID, type, cx, cy, fuLoad);

    // The image resource is fetched from basebrd.dll resource image 121.
    if (!wcscmp(pszBrand, L"Basebrd") && uID == 121)
        RecolorBrandingLogoBackground(reinterpret_cast<HBITMAP>(hImage));
    return hImage;
}

VOID WinbrandHooks()
{
    WindhawkUtils::SYMBOL_HOOK winbrand_dll_hooks[] =
    {
        {
            {
                #ifdef _WIN64
                    L"BrandingLoadImage"
                #else
                    L"_BrandingLoadImage@24"
                #endif
            },
            &BrandingLoadImage_orig,
            HookedBrandingLoadImage,
            FALSE
        },
    };

    HMODULE hWinbrand = LoadLibraryEx(L"winbrand.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hWinbrand ) {
        Wh_Log(L"Failed to load winbrand.dll");
        return;
    }

    if (!WindhawkUtils::HookSymbols(hWinbrand , winbrand_dll_hooks, ARRAYSIZE(winbrand_dll_hooks))) {
        Wh_Log(L"Failed to hook one or more symbol functions in winbrand.dll");
        return;
    }
}

// Imitate the internal pseudohandle logic and replace gpsi pointer with GetSysColorBrush getter API.
BOOL WINAPI HookedFillRect(HDC hdc, LPCRECT lprc, HBRUSH hbr)
{    
    ULONG_PTR pseudoSystemBrush = (ULONG_PTR)hbr - 1;
    if (pseudoSystemBrush <= 30)
        return FillRect_orig(hdc, lprc, GetSysColorBrush((INT)pseudoSystemBrush));    

    return FillRect_orig(hdc, lprc, hbr);
}

// Paint the explorer dialogs bottom part background
BOOL (__fastcall *SetDarkThemeColors_orig)(void **, HDC);
BOOL __fastcall HookedSetDarkThemeColors(void **Brush, HDC hdc)
{
    SetBkColor(hdc, RGB(0, 0, 0));
    SetTextColor(hdc, g_IsSysThemeDarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0));
    //if ( !*Brush )
        *Brush = (void*)GetSysColorBrush(COLOR_WINDOW);
    return *Brush != nullptr;
}

// Paint the explorer dialogs editbox background
LRESULT (STDCALL *CFileNameComboBox_s_ComboBoxRootSubclass_orig)(HWND, UINT, HDC, LPARAM, UINT_PTR, DWORD_PTR);
LRESULT STDCALL HookedCFileNameComboBox_s_ComboBoxRootSubclass(HWND hWnd, UINT uMsg, HDC wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    auto ret = CFileNameComboBox_s_ComboBoxRootSubclass_orig(hWnd, uMsg, wParam, lParam, uIdSubclass, dwRefData);

    // Intercept the paint messages
    if (uMsg != WM_CTLCOLOREDIT && uMsg != WM_CTLCOLORLISTBOX && uMsg != WM_CTLCOLORSTATIC) 
        return ret;
    
    HDC hdc = reinterpret_cast<HDC>(wParam);
    SetBkColor(hdc, RGB(0, 0, 0));
    SetTextColor(hdc, g_IsSysThemeDarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0));
    return reinterpret_cast<LRESULT>(GetSysColorBrush(COLOR_WINDOW));
}

// Paint the explorer dialogs editbox background
BOOL (STDCALL *CComboBoxExBase_OnWinEvent_orig)(class CComboBoxExBase *, HWND, UINT, HDC, LPARAM, LRESULT*);
BOOL STDCALL HookedCComboBoxExBase_OnWinEvent(class CComboBoxExBase *__this, HWND hWnd, UINT uMsg, HDC hdc, LPARAM lParam, LRESULT* pResult)
{
    auto ret = CComboBoxExBase_OnWinEvent_orig(__this, hWnd, uMsg, hdc, lParam, pResult);

    // Intercept the paint messages
    if (uMsg != WM_CTLCOLOREDIT && uMsg != WM_CTLCOLORLISTBOX && uMsg != WM_CTLCOLORSTATIC) 
        return ret;

    if (ret == false && pResult != nullptr && *pResult != 0) 
    {
        // C. Overwrite the original SetBkColor and SetTextColor
        SetBkColor(hdc, RGB(0, 0, 0));          // Black Background
        SetTextColor(hdc, g_IsSysThemeDarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0));  // White Text

        // D. Replace the original Dark Gray brush in the out-parameter with our Black Brush
        *pResult = reinterpret_cast<LRESULT>(GetSysColorBrush(COLOR_WINDOW));
    }
    return ret;
}

VOID Comdlg32Hooks()
{
    WindhawkUtils::SYMBOL_HOOK comdlg32_dll_hooks[] =
    {
        
        {
            {
                #ifdef _WIN64
                    L"bool __cdecl SetDarkThemeColors(class wil::unique_any_t<class wil::details::unique_storage<struct wil::details::resource_policy<struct HBRUSH__ *,int (__cdecl*)(void *),&int __cdecl DeleteObject(void *),struct wistd::integral_constant<unsigned __int64,0>,struct HBRUSH__ *,struct HBRUSH__ *,0,std::nullptr_t> > > &,struct HDC__ *)"
                #else
                    L"bool __stdcall SetDarkThemeColors(class wil::unique_any_t<class wil::details::unique_storage<struct wil::details::resource_policy<struct HBRUSH__ *,int (__stdcall*)(void *),&int __stdcall DeleteObject(void *),struct wistd::integral_constant<unsigned int,0>,struct HBRUSH__ *,struct HBRUSH__ *,0,std::nullptr_t> > > &,struct HDC__ *)"
                #endif
            },
            &SetDarkThemeColors_orig,
            HookedSetDarkThemeColors,
            FALSE
        },
        
        {
            {
                #ifdef _WIN64
                    L"private: static __int64 __cdecl CFileNameComboBox::s_ComboBoxRootSubclass(struct HWND__ *,unsigned int,unsigned __int64,__int64,unsigned __int64,unsigned __int64)"
                #else
                    L"private: static long __stdcall CFileNameComboBox::s_ComboBoxRootSubclass(struct HWND__ *,unsigned int,unsigned int,long,unsigned int,unsigned long)"
                #endif
            },
            &CFileNameComboBox_s_ComboBoxRootSubclass_orig,
            HookedCFileNameComboBox_s_ComboBoxRootSubclass,
            FALSE
        },
        
        {
            {
                #ifdef _WIN64
                    L"public: virtual long __cdecl CComboBoxExBase::OnWinEvent(struct HWND__ *,unsigned int,unsigned __int64,__int64,__int64 *)"
                #else
                    L"public: virtual long __stdcall CComboBoxExBase::OnWinEvent(struct HWND__ *,unsigned int,unsigned int,long,long *)"
                #endif
            },
            &CComboBoxExBase_OnWinEvent_orig,
            HookedCComboBoxExBase_OnWinEvent,
            FALSE
        },
        
    };

    HMODULE hComDlg32 = LoadLibraryEx(L"comdlg32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hComDlg32 ) {
        Wh_Log(L"Failed to load comdlg32.dll");
        return;
    }

    if (!WindhawkUtils::HookSymbols(hComDlg32 , comdlg32_dll_hooks, ARRAYSIZE(comdlg32_dll_hooks))) {
        Wh_Log(L"Failed to hook one or more symbol functions in comdlg32.dll");
        return;
    }
}

VOID CustomRenderingHooks()
{
    InitDirect2D();
    #ifdef _WIN64
        CplDuiHook();
    #endif
    WindhawkUtils::SetFunctionHook(DefWindowProc, HookedDefWindowProcW, &DefWindowProc_orig);
    WindhawkUtils::SetFunctionHook(GetThemeColor, HookedGetColorTheme, &GetThemeColor_orig);   
    WindhawkUtils::SetFunctionHook(DrawThemeBackground, HookedDrawThemeBackground, &DrawThemeBackground_orig);
    WindhawkUtils::SetFunctionHook(DrawThemeBackgroundEx, HookedDrawThemeBackgroundEx, &DrawThemeBackgroundEx_orig);
    WindhawkUtils::SetFunctionHook(DrawThemeEdge, HookedDrawThemeEdge, &DrawThemeEdge_orig);
    ExplorerFrameHooks();
    Comctl32Hooks();
    if (g_IsSysThemeDarkMode)
        Comdlg32Hooks();
    WinbrandHooks();
    User32Hooks(g_settings.SetSystemColors);
    if (!g_settings.SetSystemColors) {
        WindhawkUtils::SetFunctionHook(FillRect, HookedFillRect, &FillRect_orig);
        WindhawkUtils::SetFunctionHook(GetSysColor, HookedGetSysColor, &GetSysColor_orig);
        WindhawkUtils::SetFunctionHook(GetSysColorBrush, HookedGetSysColorBrush, &GetSysColorBrush_orig);
    }
    WindhawkUtils::SetFunctionHook(DrawTextWithGlow, HookedDrawTextWithGlow, &DrawTextWithGlow);
    WindhawkUtils::SetFunctionHook(DrawTextW, HookedDrawTextW, &DrawTextW_orig);
    WindhawkUtils::SetFunctionHook(ExtTextOutW, HookedExtTextOutW, &ExtTextOutW_orig);
    WindhawkUtils::SetFunctionHook(DrawThemeText, HookedDrawThemeText, &DrawThemeText_orig);
    WindhawkUtils::SetFunctionHook(DrawThemeTextEx, HookedDrawThemeTextEx, &DrawThemeTextEx_orig);
    UxThemeHooks(g_settings.FlyoutsEffects);
    if (g_settings.FlyoutsEffects)
        WindhawkUtils::SetFunctionHook(GetThemeMargins, HookedGetThemeMargins, &GetThemeMargins_orig);
    WindhawkUtils::SetFunctionHook(GetThemeTransitionDuration, HookedGetThemeTransitionDuration, &GetThemeTransitionDuration_orig);
    WindhawkUtils::SetFunctionHook(GetThemeFont, HookedGetThemeFont, &GetThemeFont_orig);
}

VOID ApplyHooks()
{
    if(g_settings.FillBg)
        CustomRenderingHooks();
    if (g_settings.BgType != g_settings.Default) {
        DwmSetWindowAttributeHook();
        DwmExpandFrameIntoClientAreaHook();
    }        
}

// Normalizes a path: flips slashes, trims trailing slashes, lowercases.
std::wstring NormalizeRule(std::wstring path) 
{
    if (path.empty()) return path;

    // 1. Normalize slashes
    for (auto& ch : path)
        if (ch == L'/') 
            ch = L'\\';

    // 2. Trim trailing slashes (now guaranteed to be backslashes)
    while (!path.empty() && path.back() == L'\\')
        path.pop_back();
    
    if (path.empty()) 
        return path;

    // 3. Lowercase
    LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_LOWERCASE, 
                  path.c_str(), (INT)path.length(), &path[0], (INT)path.length(), 
                  nullptr, nullptr, 0);

    return path;
}

struct CurrentProcessInfo {
    std::wstring fullPath;   // (e.g. c:\windows\system32\notepad.exe)
    std::wstring fileName;   // (e.g. notepad.exe)
    std::wstring directory;  // (e.g. c:\windows\system32)
};

CurrentProcessInfo GetCurrentProcessInfo() 
{
    WCHAR modulePath[MAX_PATH];
    GetModuleFileNameW(nullptr, modulePath, MAX_PATH);

    CurrentProcessInfo info;
    info.fullPath = modulePath;

    // FIX 1: Strip Windows long path prefix if present, otherwise string matches fail
    if (info.fullPath.find(L"\\\\?\\") == 0) {
        info.fullPath.erase(0, 4);
    }

    // FIX 2: Process paths can occasionally contain forward slashes depending on launch method.
    for (auto& ch : info.fullPath) {
        if (ch == L'/') ch = L'\\';
    }

    // Lowercase the main string once
    LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_LOWERCASE, 
                  info.fullPath.c_str(), (INT)info.fullPath.length(), 
                  &info.fullPath[0], (INT)info.fullPath.length(), 
                  nullptr, nullptr, 0);

    // Extract substrings cleanly 
    size_t pos = info.fullPath.find_last_of(L'\\');
    if (pos != std::wstring::npos) {
        info.fileName = info.fullPath.substr(pos + 1);
        info.directory = info.fullPath.substr(0, pos);
    } else {
        info.fileName = info.fullPath;
        info.directory = L""; 
    }

    return info;
}

bool MatchesProcessRule(const std::wstring& rawEntry, const CurrentProcessInfo& proc) 
{
    std::wstring entry = NormalizeRule(rawEntry);
    if (entry.empty()) return false;

    // Exact full path or directory match
    if (entry == proc.fullPath || entry == proc.directory)
        return true;

    // Bare name, e.g. "mspaint.exe"
    if (entry.find(L'\\') == std::wstring::npos) {
        return entry == proc.fileName;
    }

    // FIX 3: Subfolder match. Use .find() == 0 for a bulletproof "starts_with" check.
    std::wstring prefix = entry + L"\\";
    if (proc.fullPath.find(prefix) == 0)
        return true;

    return false;
}

VOID LoadWindowProcessRules()
{
    CurrentProcessInfo currProc = GetCurrentProcessInfo();

    for (INT i = 0;; i++) 
    {
        auto program = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"RuledPrograms[%d].target", i));
        
        if (!*program)
            break; 

        if (MatchesProcessRule(program.get(), currProc))
        {
            g_settings.FillBg = Wh_GetIntSetting(L"RuledPrograms[%d].RenderingMod.ThemeBackground", i);
            
            BOOL globalSetting_CustomTheme = Wh_GetIntSetting(L"RenderingMod.ThemeBackground");
            if (!globalSetting_CustomTheme)
                GenerateTextAlphaGammaLUT();
            
            g_settings.AccentColorize = Wh_GetIntSetting(L"RuledPrograms[%d].RenderingMod.AccentColorControls", i);
            if (g_settings.AccentColorize)
                g_settings.AccentColorize = GetAccentColor(g_settings.AccentColor);
            
            BOOL globalSetting_SetSysColorAPI = Wh_GetIntSetting(L"RenderingMod.Syscolors");

            // Reset system colors to default values if the system color setting is disabled for the specific ruled process
            if (!g_settings.FillBg && globalSetting_SetSysColorAPI)
                g_DefaultSysColors = TRUE;
            
            // Hook all necessary APIs to restore system colors when SetSysColors API has been executed by the mod
            if (g_DefaultSysColors) {
                WindhawkUtils::SetFunctionHook(GetSysColor, HookedGetSysColor, &GetSysColor_orig);
                WindhawkUtils::SetFunctionHook(GetSysColorBrush, HookedGetSysColorBrush, &GetSysColorBrush_orig);               
                WindhawkUtils::SetFunctionHook(FillRect, HookedFillRect, &FillRect_orig);
                User32Hooks(g_settings.SetSystemColors);
            }   

            auto strStyle = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"RuledPrograms[%d].BackgroundEffects.type", i));
            if (0 == wcscmp(strStyle, L"acrylicblur"))
                g_settings.BgType = g_settings.AccentBlurBehind;
            else if (0 == wcscmp(strStyle, L"acrylicsystem"))
                g_settings.BgType = g_settings.AcrylicSystemBackdrop;
            else if (0 == wcscmp(strStyle, L"mica"))
                g_settings.BgType = g_settings.Mica;
            else if (0 == wcscmp(strStyle, L"mica_tabbed"))
                g_settings.BgType = g_settings.MicaAlt;
            else 
                g_settings.BgType = g_settings.Default;

            GetColorSetting(WindhawkUtils::StringSetting(Wh_GetStringSetting(L"RuledPrograms[%d].BackgroundEffects.AccentBlurBehind", i)), g_settings.AccentBlurBehindClr);
            
            break;
        }
    }
}

VOID LoadSettings()
{
    g_settings.AccentColorize = Wh_GetIntSetting(L"RenderingMod.AccentColorControls");
    if (g_settings.AccentColorize)
       g_settings.AccentColorize = GetAccentColor(g_settings.AccentColor);

    g_settings.FillBg = Wh_GetIntSetting(L"RenderingMod.ThemeBackground");
    if (g_settings.FillBg)
        GenerateTextAlphaGammaLUT();
    
    g_settings.SetSystemColors = Wh_GetIntSetting(L"RenderingMod.Syscolors");
    // SetSysColors API available only in theme customization
    if (g_settings.SetSystemColors && g_settings.FillBg)
        ColorizeSysColors();
    
    auto strStyle = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"BackgroundEffects.type"));
    if (0 == wcscmp(strStyle, L"acrylicblur"))
        g_settings.BgType = g_settings.AccentBlurBehind;
    else if (0 == wcscmp(strStyle, L"acrylicsystem"))
        g_settings.BgType = g_settings.AcrylicSystemBackdrop;
    else if (0 == wcscmp(strStyle, L"mica"))
        g_settings.BgType = g_settings.Mica;
    else if (0 == wcscmp(strStyle, L"mica_tabbed"))
        g_settings.BgType = g_settings.MicaAlt;
    else 
        g_settings.BgType = g_settings.Default;
    
    GetColorSetting(WindhawkUtils::StringSetting(Wh_GetStringSetting(L"BackgroundEffects.AccentBlurBehind")), g_settings.AccentBlurBehindClr);

    g_settings.FlyoutsEffects = Wh_GetIntSetting(L"FlyoutsEffects");
        
    LoadWindowProcessRules();
    
    ApplyHooks();
}

BOOL Wh_ModInit(VOID) 
{
    if (InExplorerProcess())
        g_explorerStylerNoBackgroundEffectAtom = AddAtom(L"WindhawkFileExplorerStylerNoBackgroundEffect");
    if (InTaskManagerProcess())
        g_InsideTaskMgrProc = TRUE;

    LoadSettings();

    HMODULE hModule = GetModuleHandle(L"win32u.dll");
    if (!hModule) 
        return FALSE;
        
    NtUserCreateWindowEx_t pNtUserCreateWindowEx = (NtUserCreateWindowEx_t)GetProcAddress(hModule, "NtUserCreateWindowEx");
    if (!pNtUserCreateWindowEx)
        return FALSE;
    
    WindhawkUtils::SetFunctionHook(pNtUserCreateWindowEx, HookedNtUserCreateWindowEx, &NtUserCreateWindowEx_Original);
    
    return TRUE;
}

VOID Wh_ModAfterInit() 
{
    #ifdef _WIN64
        const size_t OFFSET_SAME_TEB_FLAGS = 0x17EE;
    #else
        const size_t OFFSET_SAME_TEB_FLAGS = 0x0FCA;
    #endif
    BOOL isInitialThread = *(USHORT*)((BYTE*)NtCurrentTeb() + OFFSET_SAME_TEB_FLAGS) & 0x0400;
    if (!isInitialThread)
        ApplyForExistingWindows();
}

VOID Wh_ModUninit(VOID) 
{
    if (g_explorerStylerNoBackgroundEffectAtom)
        DeleteAtom(g_explorerStylerNoBackgroundEffectAtom);
     
    g_settings.Unload = TRUE;
    if (g_settings.FillBg && g_d2dFactory)
        g_d2dFactory->Release();
    
    if (g_settings.SetSystemColors)
        RevertSysColors();

    for (size_t i = 0; i < g_themeCachedDefaultSysColorBrushes.size(); i++) {
        HBRUSH& brushCustom = g_themeCachedCustomSysColorBrushes[i];
        HBRUSH& brushDefault = g_themeCachedDefaultSysColorBrushes[i];
        if (brushCustom) { 
            DeleteObject(brushCustom); 
            brushCustom = nullptr; 
        }
        if (brushDefault) { 
            DeleteObject(brushDefault); 
            brushDefault = nullptr; 
        }
    }

    ApplyForExistingWindows();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) 
{
    *bReload = TRUE;
    return TRUE;
}
