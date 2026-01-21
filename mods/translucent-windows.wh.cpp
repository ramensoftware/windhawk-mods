// ==WindhawkMod==
// @id              translucent-windows
// @name            Translucent Windows
// @description     Enables native translucent effects in Windows 11
// @version         1.7.2
// @author          Undisputed00x
// @github          https://github.com/Undisputed00x
// @include         *
// @compilerOptions -ldwmapi -luxtheme -lcomctl32 -lgdi32 -ld2d1 -lmsimg32 -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

# ⚠️ FAQ section below ⚠️

### Blur (AccentBlurBehind)
![AccentBlurBehind](https://i.imgur.com/tSf5ztk.png)
### Acrylic (SystemBackdrop)
![Acrylic SystemBackdrop](https://i.imgur.com/YNktLTu.png)
### Mica (SystemBackdrop)
![Mica](https://i.imgur.com/1ciJJck.png)
### MicaAlt (SystemBackdrop)
![MicaTabbed](https://i.imgur.com/5Dxj5PS.png)

# FAQ

* ⚠️Use Windows 11 File Explorer Styler mod and select the Translucent Explorer11 theme
in order to get translucent WinUI parts of the new file explorer.⚠️

* ❗The new system colors setting option adjusts the [system colors](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getsyscolor) 
in order to blend with the background translucency / custom theme rendering. 
This setting option overrides any process exclusion as these colors are applied system-wide using 
the [SetSysColor](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setsyscolors) API.
Intercepting and changing the system colors is quite difficult, more details
in another software project that faced the same problem: https://github.com/namazso/SecureUxTheme/issues/9#issuecomment-611897882 ❗

* ❗The Windows custom theme rendering also fixes invisible text by restoring alpha and modifying text colors.
Extending effects to the entire window can result in text being barely readable or even invisible in some cases. 
Enabling HDR, 10bit color depth output, having a black color, or a white background behind the window can cause this. 
This is because most GDI rendering operations ignore or do not preserve alpha values.❗

* ⚠️The background color of the properties window may remain unchanged after applying the Windows theme custom rendering.
Restart explorer.exe to change the background color.⚠️

* ⚠️Prerequisited windows settings to enable the background effects⚠️
    - Transparency effects enabled
    - Energy saver disabled
#
* ⚠️The background effects do not affect most modern windows (UWP/WinUI), 
apps with different front-end rendering (e.g Qt, Electron, Chromium etc.. programs) and native windows with hardcoded colors.⚠️

* ⚠️If parts of the Windows UI colors remain modified after disabling the modification, this is happening when new system colors are applied in a selected Windows custom theme.
Changing the theme to the default and vice versa fixes the problem. As a last resort, you can delete the registry key HKEY_CURRENT_USER\Control Panel\Colors and reboot.⚠️

* ❕The blur effect may show a bleeding effect at the edges of a window when maximized or snapped to the edge of the screen. 
This is caused by default by the AccentBlur API.❕

* ✨It is recommended to use the mod with a custom black/dark window themes like the Rectify11 "Dark theme with Mica".✨

* ✨The mod works best on the default dark theme.✨

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- RenderingMod:
    - ThemeBackground: FALSE
      $name: Windows theme custom rendering
      $description: >-
       Modifies parts of the Windows theme using the Direct2D graphics API and modifies 
       Windows GDI text rendering by patching the alpha channel and adjusting text colors.
        ✨It is recommended to enable this with background translucent effects.
    - SysColors: FALSE
      $name: New system colors
      $description: >-
       Modifies additional system UI colors by calling SetSysColors API.
        ⚠️For issues with excluded processes, refer to the FAQ.
         ✨It is recommended to enable this with Windows theme custom rendering.
    - AccentColorControls: FALSE
      $name: Windows theme accent colorizer
      $description: >-
       Paint with accent color parts of windows theme. (Requires Windows theme custom rendering)
  $name: Rendering Customization
- type: none
  $name: Background translucent effects
  $description: >-
     Windows 11 version >= 22621.xxx (22H2) is required for SystemBackdrop effects.
  $options:
  - none: Default
  - acrylicblur: Blur (AccentBlurBehind)
  - acrylicsystem: Acrylic (SystemBackdrop)
  - mica: Mica (SystemBackdrop)
  - mica_tabbed: MicaAlt (SystemBackdrop)
- AccentBlurBehind: "3A232323"
  $name: AccentBlurBehind color blend
  $description: >-
    Blending color with blur background.
     Color in hexadecimal ARGB format e.g. 3A232323
- ImmersiveDarkTitle: FALSE
  $name: Immersive darkmode titlebar
  $description: >-
    Affects the tintcolor of SystemBackdrop effects and the glyph color of the titlebar caption buttons.
     ✨It is recommended to enable this with background translucent effects.
- ExtendFrame: FALSE
  $name: Extend effects into entire window
  $description: >-
    Extends the effects into the entire window background using DwmExtendFrameIntoClientArea.
     ✨It is recommended to enable this with background translucent effects.
- CornerOption: default
  $name: Window corner type
  $description: >-
     Specifies the rounded corner preference for a window
      Windows 11 version >= 22000.xxx (21H2) is required.
  $options: 
  - default: Default
  - notrounded: Not Rounded
  - smallround: Small Corner
- RainbowSpeed: 1
  $name: Rainbow Speed
  $description: Adjust the refresh speed of the rainbow colors (Value between 1 and 100).
- TitlebarColor:
    - ColorTitlebar: FALSE
      $name: Enable
    - RainbowTitlebar: FALSE
      $name: Rainbow Titlebar
      $description: Enables rainbow effect on windows titlebar.
    - titlerbarstyles_active: "FF0000"
      $name: Focused window color
      $description: >-
       Color in hexadecimal RGB format e.g. Red = FF0000
        SystemAccentColor = 2
    - titlerbarstyles_inactive: "00FFFF"
      $name: Unfocused window color
      $description: >-
       Color in hexadecimal RGB format e.g. Red = FF0000
        SystemAccentColor = 2
  $name: Titlebar color
  $description: Windows 11 version >= 22000.xxx (21H2) is required. Overrides effects settings
- TitlebarTextColor:
    - ColorTitlebarText: FALSE
      $name: Enable
    - RainbowTextColor: FALSE
      $name: Rainbow Titlebar text
      $description: Enables rainbow effect on windows titlebar text.
    - titlerbarcolorstyles_active: "FF0000"
      $name: Focused window color
      $description: >-
       Color in hexadecimal RGB format e.g. Red = FF0000
        Default = 1
        SystemAccentColor = 2
    - titlerbarcolorstyles_inactive: "00FFFF"
      $name: Unfocused window color
      $description: >-
       Color in hexadecimal RGB format e.g. Red = FF0000
        Default = 1
        SystemAccentColor = 2
  $name: Titlebar text color
  $description: >-
     Windows 11 version >= 22000.xxx (21H2) is required.
      NOTE: This settings affects only Win32 windows. Since Win11 22H2 File Explorer changed titlebar text rendering to DirectWrite API. To modify the text color of File Explorer title bar, use Windhawk's File Explorer Styler mod.
- BorderColor:
    - ColorBorder: FALSE
      $name: Enable
    - RainbowBorder: FALSE
      $name: Rainbow Border
      $description: Enables rainbow effect on windows borders.
    - borderstyles_active: "FF0000"
      $name: Focused window color
      $description: >-
       Color in hexadecimal RGB format e.g. Red = FF0000
        Transparent = 0
        Default = 1
        SystemAccentColor = 2
    - borderstyles_inactive: "00FFFF"
      $name: Unfocused window color
      $description: >-
       Color in hexadecimal RGB format e.g. Red = FF0000
        Transparent = 0
        Default = 1
        SystemAccentColor = 2
  $name: Border color
  $description: >-
      Windows 11 version >= 22000.xxx (21H2) is required.
- RuledPrograms:
    - - target: "mspaint.exe"
        $name: Process
        $description: >-
         Entries can be process names or paths, for example:
          mspaint.exe
          C:\Windows\System32\notepad.exe
      - RenderingMod:
          - ThemeBackground: FALSE
            $name: Windows theme custom rendering
            $description: >-
              Modifies parts of the Windows theme using the Direct2D graphics API and modifies 
              Windows GDI text rendering by patching the alpha channel and adjusting text colors.
               ✨It is recommended to enable this with background translucent effects.
          - AccentColorControls: FALSE
            $name: Windows theme accent colorizer
            $description: >-
              Paint with accent color parts of windows theme. (Requires Windows theme custom rendering)
      - type: none
        $name: Background translucent effects
        $description: >-
         Windows 11 version >= 22621.xxx (22H2) is required for SystemBackdrop effects.
        $options:
          - none: Default
          - acrylicblur: Blur (AccentBlurBehind)
          - acrylicsystem: Acrylic (SystemBackdrop)
          - mica: Mica (SystemBackdrop)
          - mica_tabbed: MicaAlt (SystemBackdrop)
      - AccentBlurBehind: "3A232323"
        $name: AccentBlurBehind color blend
        $description: >-
          Blending color with blur background.
           Color in hexadecimal ARGB format e.g. 3A232323
      - ImmersiveDarkTitle: FALSE
        $name: Immersive darkmode titlebar
        $description: >-
            Affects the tintcolor of SystemBackdrop effects and the glyph color of the titlebar caption buttons.
             ✨It is recommended to enable this with background translucent effects.
      - ExtendFrame: FALSE
        $name: Extend effects into entire window
        $description: >-
          Extends the effects into the entire window background using DwmExtendFrameIntoClientArea.
           ✨It is recommended to enable this with background translucent effects.
      - CornerOption: default
        $name: Window corner type
        $description: >-
             Specifies the rounded corner preference for a window
              Windows 11 version >= 22000.xxx (21H2) is required.
        $options: 
        - default: Default
        - notrounded: Not Rounded
        - smallround: Small Corner
      - RainbowSpeed: 1
        $name: Rainbow Speed
        $description: Adjust the refresh speed of the rainbow colors (Value between 1 and 100).
      - TitlebarColor:
          - ColorTitlebar: FALSE
            $name: Enable
          - RainbowTitlebar: FALSE
            $name: Rainbow Titlebar
            $description: Enables rainbow effect on windows titlebar.
          - titlerbarstyles_active: "FF0000"
            $name: Focused window color
            $description: >-
             Color in hexadecimal RGB format e.g. Red = FF0000
              SystemAccentColor = 2
          - titlerbarstyles_inactive: "00FFFF"
            $name: Unfocused window color
            $description: >-
             Color in hexadecimal RGB format e.g. Red = FF0000
              SystemAccentColor = 2
        $name: Titlebar color
        $description: Windows 11 version >= 22000.xxx (21H2) is required. Overrides effects settings
      - TitlebarTextColor:
          - ColorTitlebarText: FALSE
            $name: Enable
          - RainbowTextColor: FALSE
            $name: Rainbow Titlebar text
            $description: Enables rainbow effect on windows titlebar text.
          - titlerbarcolorstyles_active: "FF0000"
            $name: Focused window color
            $description: >-
             Color in hexadecimal RGB format e.g. Red = FF0000
              Default = 1
              SystemAccentColor = 2
          - titlerbarcolorstyles_inactive: "00FFFF"
            $name: Unfocused window color
            $description: >-
             Color in hexadecimal RGB format e.g. Red = FF0000
              Default = 1
              SystemAccentColor = 2
        $name: Titlebar text color
        $description: >-
         Windows 11 version >= 22000.xxx (21H2) is required.
          NOTE: This settings affects only Win32 windows. Since Win11 22H2 File Explorer changed titlebar text rendering to DirectWrite API. To modify the text color of File Explorer title bar, use Windhawk's File Explorer Styler mod.
      - BorderColor:
          - ColorBorder: FALSE
            $name: Enable
          - RainbowBorder: FALSE
            $name: Rainbow Border
            $description: Enables rainbow effect on windows borders.
          - borderstyles_active: "FF0000"
            $name: Focused window color
            $description: >-
             Color in hexadecimal RGB format e.g. Red = FF0000
              Transparent = 0
              Default = 1
              SystemAccentColor = 2
          - borderstyles_inactive: "00FFFF"
            $name: Unfocused window color
            $description: >-
             Color in hexadecimal RGB format e.g. Red = FF0000
              Transparent = 0
              Default = 1
              SystemAccentColor = 2
        $name: Border color
        $description: >-
           Windows 11 version >= 22000.xxx (21H2) is required.
  $name: Process Rules
  $description: >-
      Add rules to each specified process
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <dwmapi.h>
#include <vssym32.h>
#include <uxtheme.h>
#include <random>
#include <string>
#include <array>
#include <mutex>
#include <unordered_set>
#include <d2d1.h>
#include <wrl.h>
#include <ShellScalingApi.h>


static UINT ENABLE = 1;
static constexpr UINT AUTO = 0; // DWMSBT_AUTO
//static constexpr UINT NONE = 1; // DWMSBT_NONE
static constexpr UINT MAINWINDOW = 2; // DWMSBT_MAINWINDOW
static constexpr UINT TRANSIENTWINDOW = 3; // DWMSBT_TRANSIENTWINDOW
static constexpr UINT TABBEDWINDOW = 4; // DWMSBT_TABBEDWINDOW

static constexpr UINT DEFAULT = 0; // DWMWCP_DEFAULT
static constexpr UINT DONTROUND = 1; // DWMWCP_DONOTROUND
//static constexpr UINT ROUND = 2; // DWMWCP_ROUND
//static constexpr UINT SMALLROUND = 3; // DWMWCP_ROUNDSMALL

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

UINT g_msgRainbowTimer = RegisterWindowMessage(L"Rainbow_effect");

enum {
    RAINBOW_LOAD,
    RAINBOW_UNLOAD,
    RAINBOW_PAUSED,
    RAINBOW_RESTART
};

struct RainbowData {
    DOUBLE initialHue = 0.0;
    DOUBLE baseTime = 0.0;        // when timer first started
    DOUBLE pausedTotal = 0.0;     // accumulated paused duration
    DOUBLE pausedStart = 0.0;     // when current pause began
    BOOL isPaused = FALSE;
    UINT_PTR timerId = 0;
};

std::wstring g_RainbowPropStr = L"Windhawk_TranslucentMod_Rainbow";
std::mutex g_rainbowWindowsMutex;
std::unordered_set<HWND> g_rainbowWindows;

thread_local BOOL g_DrawTextWithGlowEntry;

typedef HRESULT(WINAPI* pDrawTextWithGlow)(HDC, LPCWSTR, UINT, const RECT*, DWORD, COLORREF, COLORREF, UINT, UINT, BOOL, DTT_CALLBACK_PROC, LPARAM);
static auto DrawTextWithGlow = (pDrawTextWithGlow)GetProcAddress(GetModuleHandle(L"uxtheme.dll"), MAKEINTRESOURCEA(126));

// Detect system dark/light theme mode
typedef HRESULT(WINAPI* pShouldSystemUseDarkMode)();
static auto ShouldSystemUseDarkMode = (pShouldSystemUseDarkMode)GetProcAddress(GetModuleHandle(L"uxtheme.dll"), MAKEINTRESOURCEA(138));
HRESULT g_IsSysThemeDarkMode = ShouldSystemUseDarkMode();

thread_local HHOOK g_callWndProcHook;
std::mutex g_allCallWndProcHooksMutex;
std::unordered_set<HHOOK> g_allCallWndProcHooks;

std::mutex g_subclassedflyoutsmutex;
std::unordered_set<HWND> g_subclassedflyouts;

HTHEME g_hTheme = nullptr;
std::array<HBRUSH, COLOR_MENUBAR> g_cachedSysColorBrushes {nullptr};

using PUNICODE_STRING = PVOID;
constexpr auto MENUPOPUP_CLASS = L"#32768";

struct Settings{
    BOOL FillBg = FALSE;
    BOOL AccentColorize = FALSE;
    COLORREF AccentColor = 0xFFFFFFFF;
    BOOL TextAlphaBlend = FALSE;
    BOOL SetSystemColors = FALSE;
    COLORREF AccentBlurBehindClr = 0x00000000;
    BOOL ImmersiveDarkmode = FALSE;
    BOOL ExtendFrame = FALSE;
    BOOL Unload = FALSE;
    BOOL TitlebarFlag = FALSE;
    BOOL CaptionTextFlag = FALSE;
    BOOL BorderFlag = FALSE;
    COLORREF TitlebarActiveColor = DWMWA_COLOR_DEFAULT;
    COLORREF CaptionActiveTextColor = DWMWA_COLOR_DEFAULT;
    COLORREF BorderActiveColor = DWMWA_COLOR_DEFAULT;

    COLORREF g_TitlebarColor = DWMWA_COLOR_DEFAULT;
    COLORREF g_CaptionColor = DWMWA_COLOR_DEFAULT;
    COLORREF g_BorderColor = DWMWA_COLOR_DEFAULT;

    COLORREF TitlebarInactiveColor = DWMWA_COLOR_DEFAULT;
    COLORREF CaptionInactiveTextColor = DWMWA_COLOR_DEFAULT;
    COLORREF BorderInactiveColor = DWMWA_COLOR_DEFAULT;
    
    FLOAT RainbowSpeed = 1.0f;

    BOOL TitlebarRainbowFlag = FALSE;
    BOOL CaptionRainbowFlag = FALSE;
    BOOL BorderRainbowFlag = FALSE;

    enum BACKGROUNDTYPE
    {
        Default,
        AccentBlurBehind,
        AcrylicSystemBackdrop,
        Mica,
        MicaAlt,
    } BgType = Default;

    enum CORNERTYPE
    {
        NotRounded = 1,
        DefaultRounded = 2,
        SmallRounded = 3
    } CornerPref = DefaultRounded;

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

ACCENT_POLICY g_accent = {};
WINCOMPATTRDATA g_attrib = {};
DWM_BLURBEHIND g_bb = { 0 };

ID2D1Factory* g_d2dFactory = nullptr;

VOID InitDirect2D()
{
    if (!g_d2dFactory)
    {
        D2D1_FACTORY_OPTIONS options = {};
        D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, options, &g_d2dFactory);
    }
}

// Credits to @m417z
DOUBLE g_recipCyclesPerSecond;

VOID TimerInitialize() {
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    DOUBLE cyclesPerSecond = static_cast<DOUBLE>(freq.QuadPart);
    g_recipCyclesPerSecond = 1.0 / cyclesPerSecond;
}

DOUBLE TimerGetCycles() {
    LARGE_INTEGER T1;
    QueryPerformanceCounter(&T1);
    return static_cast<DOUBLE>(T1.QuadPart);
}

DOUBLE TimerGetSeconds() {
    return TimerGetCycles() * g_recipCyclesPerSecond;
}

using NtUserCreateWindowEx_t = HWND(WINAPI*)(DWORD, PUNICODE_STRING, LPCWSTR, PUNICODE_STRING, DWORD, LONG, LONG, LONG, LONG, HWND, HMENU, HINSTANCE, LPVOID, DWORD, DWORD, DWORD, VOID*);
NtUserCreateWindowEx_t NtUserCreateWindowEx_Original;

static decltype(&DwmExtendFrameIntoClientArea) DwmExtendFrameIntoClientArea_orig = nullptr;
static decltype(&DwmSetWindowAttribute) DwmSetWindowAttribute_orig = nullptr;
decltype(&TrackPopupMenuEx) TrackPopupMenuEx_orig;

static decltype(&DrawTextW) DrawTextW_orig = nullptr;
static decltype(&DrawTextExW) DrawTextExW_orig = nullptr;
static decltype(&ExtTextOutW) ExtTextOutW_orig = nullptr;
static decltype(&DrawThemeText) DrawThemeText_orig = nullptr;
static decltype(&DrawThemeTextEx) DrawThemeTextEx_orig = nullptr;

static decltype(&GetThemeBitmap) GetThemeBitmap_orig = nullptr;
static decltype(&GetThemeColor) GetThemeColor_orig = nullptr;
static decltype(&DrawThemeBackground) DrawThemeBackground_orig = nullptr;
static decltype(&DrawThemeBackgroundEx) DrawThemeBackgroundEx_orig = nullptr;
static decltype(&GetThemeMargins) GetThemeMargins_orig = nullptr;
static decltype(&GetSysColor) GetSysColor_orig = nullptr;
static decltype(&GetSysColorBrush) GetSysColorBrush_orig = nullptr;
static decltype(&DefWindowProcW) DefWindowProc_orig = nullptr;

VOID NewWindowShown(HWND);
VOID HandleEffects(HWND hWnd);

std::wstring GetWindowClass(HWND hWnd)
{
    WCHAR buffer[MAX_PATH];
    GetClassNameW(hWnd, buffer, MAX_PATH);
    return buffer;
}

BOOL IsWindowClass(HWND hWnd, LPCWSTR className)
{
    return GetWindowClass(hWnd) == className;
}

BOOL IsWindowEligible(HWND hWnd) 
{
    if (IsWindowClass(hWnd, TOOLTIPS_CLASS) || IsWindowClass(hWnd, L"DropDown") || IsWindowClass(hWnd, L"ViewControlClass"))
        return TRUE;   
    
    LONG_PTR styleEx = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);

    // Fixes Snipping Tool rec
    if ((styleEx & WS_EX_NOACTIVATE) || (styleEx & WS_EX_TRANSPARENT))
        return FALSE;
    
    HWND hParentWnd = GetAncestor(hWnd, GA_PARENT);
    if (hParentWnd && hParentWnd != GetDesktopWindow())
        return FALSE;

    BOOL hasTitleBar = (style & WS_BORDER) && (style & WS_DLGFRAME);

    if (!hasTitleBar && ((styleEx & WS_EX_TOOLWINDOW) ||
       (style & WS_POPUP) || (styleEx & WS_EX_APPWINDOW)))
        return FALSE;

    // Don't block CEF apps
    if (!((IsWindowClass(hWnd, L"Chrome_WidgetWin_1") || IsWindowClass(hWnd, L"Chrome_WidgetWin_0")) || style & WS_POPUP || styleEx & WS_EX_APPWINDOW || styleEx & WS_EX_DLGMODALFRAME)
        && !(style & WS_THICKFRAME || style & WS_MINIMIZEBOX || style & WS_MAXIMIZEBOX || style & 0x00000080l)) // Firefox dialog
        return FALSE;
    
    return TRUE;
}

BOOL IsWindowMaximizedOrFullscreen(HWND hWnd)
{
    WINDOWPLACEMENT wp{.length = sizeof(WINDOWPLACEMENT)};

    if (GetWindowPlacement(hWnd, &wp) && wp.showCmd == SW_SHOWMAXIMIZED)
        return TRUE;

    HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    if (!hMon)
        return FALSE;

    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfoW(hMon, &mi);

    RECT windowRect{};
    DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &windowRect,
                          sizeof(windowRect));

    if (EqualRect(&windowRect, &mi.rcMonitor))
        return TRUE;

    return FALSE;
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

struct HSL
{
    DOUBLE h; // Hue:        0–360
    DOUBLE s; // Saturation: 0–1
    DOUBLE l; // Lightness:  0–1
};

static HSL RGBToHSL(COLORREF color)
{
    DOUBLE r = GetRValue(color) / 255.0;
    DOUBLE g = GetGValue(color) / 255.0;
    DOUBLE b = GetBValue(color) / 255.0;

    DOUBLE maxc = std::max({ r, g, b });
    DOUBLE minc = std::min({ r, g, b });
    DOUBLE delta = maxc - minc;

    HSL hsl{};
    hsl.l = (maxc + minc) * 0.5;

    if (delta == 0.0)
    {
        // Gray (no saturation)
        hsl.h = 0.0;
        hsl.s = 0.0;
    }
    else
    {
        // Saturation
        hsl.s = (hsl.l > 0.5)
            ? delta / (2.0 - maxc - minc)
            : delta / (maxc + minc);

        // Hue
        if (maxc == r)
            hsl.h = (g - b) / delta + (g < b ? 6.0 : 0.0);
        else if (maxc == g)
            hsl.h = (b - r) / delta + 2.0;
        else
            hsl.h = (r - g) / delta + 4.0;

        hsl.h *= 60.0;
    }

    return hsl;
}

static COLORREF HSLToRGB(FLOAT h, FLOAT s, FLOAT l) {
    FLOAT c = (1.0f - fabs(2.0f * l - 1.0f)) * s;
    FLOAT x = c * (1.0f - fabs(fmod(h / 60.0f, 2.0f) - 1.0f));
    FLOAT m = l - c / 2.0f;

    FLOAT r_prime, g_prime, b_prime;
    if (0.0f <= h && h < 60.0f) {
        r_prime = c; g_prime = x; b_prime = 0.0f;
    } else if (60.0f <= h && h < 120.0f) {
        r_prime = x; g_prime = c; b_prime = 0.0f;
    } else if (120.0f <= h && h < 180.0f) {
        r_prime = 0.0f; g_prime = c; b_prime = x;
    } else if (180.0f <= h && h < 240.0f) {
        r_prime = 0.0f; g_prime = x; b_prime = c;
    } else if (240.0f <= h && h < 300.0f) {
        r_prime = x; g_prime = 0.0f; b_prime = c;
    } else {
        r_prime = c; g_prime = 0.0f; b_prime = x;
    }

    BYTE r = static_cast<BYTE>((r_prime + m) * 255.0f);
    BYTE g = static_cast<BYTE>((g_prime + m) * 255.0f);
    BYTE b = static_cast<BYTE>((b_prime + m) * 255.0f);
    return RGB(r, g, b);
}

BOOL GetAccentColor(COLORREF& outColor)
{
    // In some programs, e.g. snippingtool.exe, the default blue accent color is used instead of the Windows theme with DwmGetColorizationColor.
    // Use the immersive color API if available, fall back to DwmGetColorizationColor
    // https://github.com/ALTaleX531/TranslucentFlyouts/blob/017970cbac7b77758ab6217628912a8d551fcf7c/Common/ThemeHelper.hpp#L278
    static const auto s_GetImmersiveColorFromColorSetEx{ reinterpret_cast<DWORD(WINAPI*)(DWORD dwImmersiveColorSet, DWORD dwImmersiveColorType, BOOL bIgnoreHighContrast, DWORD dwHighContrastCacheMode)>(GetProcAddress(GetModuleHandleW(L"UxTheme.dll"), MAKEINTRESOURCEA(95))) };
    static const auto s_GetImmersiveColorTypeFromName{ reinterpret_cast<DWORD(WINAPI*)(LPCWSTR name)>(GetProcAddress(GetModuleHandleW(L"UxTheme.dll"), MAKEINTRESOURCEA(96))) };
    static const auto s_GetImmersiveUserColorSetPreference{ reinterpret_cast<DWORD(WINAPI*)(BOOL bForceCheckRegistry, BOOL bSkipCheckOnFail)>(GetProcAddress(GetModuleHandleW(L"UxTheme.dll"), MAKEINTRESOURCEA(98))) };

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
    // Popup menus (#32768) pass here by default to paint 
    // the window border and corners
    if (IsWindowClass(hWnd, MENUPOPUP_CLASS))
    {
        if (dwAttribute == DWMWA_BORDER_COLOR && g_settings.BorderFlag)
        {
            if (g_settings.BorderActiveColor == DWMWA_COLOR_NONE) {
                COLORREF Transparent = DWMWA_COLOR_NONE;
                return DwmSetWindowAttribute_orig(hWnd, DWMWA_BORDER_COLOR, &Transparent, sizeof(COLORREF));
            }
            else if (g_settings.BorderActiveColor != DWMWA_COLOR_DEFAULT)
                return DwmSetWindowAttribute_orig(hWnd, DWMWA_BORDER_COLOR, &g_settings.BorderActiveColor, sizeof(COLORREF));
        }
        else if (dwAttribute == DWMWA_WINDOW_CORNER_PREFERENCE && g_settings.BgType != g_settings.Default)
            return DwmSetWindowAttribute_orig(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &g_settings.CornerPref , sizeof(UINT));
    }
            
    if(!IsWindowEligible(hWnd))
        return DwmSetWindowAttribute_orig(hWnd, dwAttribute, pvAttribute, cbAttribute);
    
    if(!g_settings.TitlebarFlag)
    {
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
    }
    else if ((dwAttribute == DWMWA_CAPTION_COLOR || (dwAttribute == DWMWA_SYSTEMBACKDROP_TYPE 
        && (IsWindowClass(hWnd, L"CabinetWClass") || IsWindowClass(hWnd, L"TaskManagerWindow")))) && g_settings.TitlebarFlag)
            return DwmSetWindowAttribute_orig(hWnd, DWMWA_CAPTION_COLOR, &g_settings.g_TitlebarColor, sizeof(COLORREF));
    
    // Effects on VS Studio, Windows Terminal ...
    if(dwAttribute == DWMWA_BORDER_COLOR && g_settings.BorderFlag)
        return DwmSetWindowAttribute_orig(hWnd, DWMWA_BORDER_COLOR, &g_settings.g_BorderColor, sizeof(COLORREF));
    
    return DwmSetWindowAttribute_orig(hWnd, dwAttribute, pvAttribute, cbAttribute);
}

HRESULT WINAPI HookedDwmExtendFrameIntoClientArea(HWND hWnd, const MARGINS* pMarInset)
{
    if(!IsWindowEligible(hWnd))
        [[clang::musttail]]return DwmExtendFrameIntoClientArea_orig(hWnd, pMarInset);
    
    if(g_settings.ExtendFrame)
    {
        if (!IsWindowClass(hWnd, L"CASCADIA_HOSTING_WINDOW_CLASS") )
        {
            static const MARGINS margins = {-1, -1, -1, -1};
            [[clang::musttail]]return DwmExtendFrameIntoClientArea_orig(hWnd, &margins);
        }
    }
    [[clang::musttail]]return DwmExtendFrameIntoClientArea_orig(hWnd, pMarInset);
}

HWND WINAPI HookedNtUserCreateWindowEx(DWORD dwExStyle,
                                          PUNICODE_STRING UnsafeClassName,
                                          LPCWSTR VersionedClass,
                                          PUNICODE_STRING UnsafeWindowName,
                                          DWORD dwStyle,
                                          LONG x,
                                          LONG y,
                                          LONG nWidth,
                                          LONG nHeight,
                                          HWND hWndParent,
                                          HMENU hMenu,
                                          HINSTANCE hInstance,
                                          LPVOID lpParam,
                                          DWORD dwShowMode,
                                          DWORD dwUnknown1,
                                          DWORD dwUnknown2,
                                          VOID* qwUnknown3) 
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


// Fix Alpha of DrawTextW
// https://github.com/Maplespe/ExplorerBlurMica/blob/79c0ef4d017e32890e107ff98113507f831608b6/ExplorerBlurMica/HookDef.cpp#L859
INT WINAPI HookedDrawTextW(HDC hdc, LPCWSTR lpchText, INT cchText, LPRECT lprc, UINT format) 
{
    if (format & DT_CALCRECT || g_DrawTextWithGlowEntry)
        return DrawTextW_orig(hdc, lpchText, cchText, lprc, format);
    
    HRESULT hr = S_OK;
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

    BP_PAINTPARAMS bpParam;
    bpParam.cbSize = sizeof(BP_PAINTPARAMS);
    bpParam.dwFlags = BPPF_ERASE;
    bpParam.prcExclude = nullptr;
    bpParam.pBlendFunction = &bf;
    
    HDC hDC = nullptr;
    RECT PaintRect = *lprc;
    InflateRect(&PaintRect, 2, 0);
    HPAINTBUFFER pbuffer = BeginBufferedPaint(hdc, &PaintRect, BPBF_TOPDOWNDIB, &bpParam, &hDC);
    if (pbuffer && hDC)
    {
        g_DrawTextWithGlowEntry = TRUE;
        // Set the original DC information
        SelectObject(hDC, GetCurrentObject(hdc, OBJ_FONT));
        SetBkMode(hDC, GetBkMode(hdc));
        SetBkColor(hDC, GetBkColor(hdc));
        SetTextAlign(hDC, GetTextAlign(hdc));
        SetTextCharacterExtra(hDC, GetTextCharacterExtra(hdc));
        SetMapMode(hDC, GetMapMode(hdc));

        COLORREF color = GetTextColor(hdc);
        COLORREF GlowColor = 0xFFFFFFFF;
        INT GlowIntesity = 32;
        INT GlowRadius = 2;
        INT UnknownGlowParam = 255;
        HSL txtHsl = RGBToHSL(color);
        if (txtHsl.l >= 0.35) {
            GlowColor = 0xFF000000;
            if (txtHsl.l <= 0.8 && txtHsl.s <= 0.5)
                GlowIntesity = 16;
        }

        hr = DrawTextWithGlow(hDC, lpchText, cchText, lprc, format,
        color, GlowColor, GlowRadius, GlowIntesity, UnknownGlowParam,
        [](HDC hdc, LPWSTR lpchText, INT cchText, LPRECT lprc, UINT format, LPARAM lParam) WINAPI
        {
            return DrawTextW_orig(hdc, lpchText, cchText, lprc, format);
        },
        0);

        EndBufferedPaint(pbuffer, TRUE);
        g_DrawTextWithGlowEntry = FALSE;
        return (SUCCEEDED(hr)) ? TRUE : DrawTextW_orig(hdc, lpchText, cchText, lprc, format);
    }
    return DrawTextW_orig(hdc, lpchText, cchText, lprc, format);
}

BOOL WINAPI HookedDrawTextExW(HDC hdc, LPWSTR lpchText, INT cchText, LPRECT lprc, UINT format, LPDRAWTEXTPARAMS lpdtp)
{
    if (!lpdtp && !(format & DT_CALCRECT) && !g_DrawTextWithGlowEntry) {

        auto ret = HookedDrawTextW(hdc, lpchText, cchText, lprc, format);
        return ret;
    }
    return DrawTextExW_orig(hdc, lpchText, cchText, lprc, format, lpdtp);
}

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
    if (!hdc || (!lpString && options != ETO_OPAQUE) || g_DrawTextWithGlowEntry)
        return ExtTextOutW_orig(hdc, x, y, options, lprect, lpString, c, lpDx);
    
    if (options == (ETO_CLIPPED | ETO_OPAQUE) && lprect) {
        RECT rect = *lprect;
        auto ret = HookedDrawTextW(hdc, lpString, c, &rect, DT_LEFT | DT_TOP | DT_SINGLELINE);
        return (ret) ? TRUE : FALSE;
    }

    RECT textRect {0};
    SIZE textSize = {0};
    
    if (lprect) {
        // Pass one line (borders) drawings
        if (options == ETO_OPAQUE && 
        ((lprect->right-lprect->left) == 1 || (lprect->bottom-lprect->top) == 1))
            textRect = *lprect;
        else if (options != ETO_OPAQUE)
            textRect = *lprect;
    }
    else if (options == ETO_GLYPH_INDEX)
    {
        if (GetTextExtentPointI(hdc, (WORD*)lpString, c, &textSize))
        {
            textRect.left   = x;
            textRect.top    = y - textSize.cy;
            textRect.right  = x + textSize.cx ;
            textRect.bottom = textSize.cy + y;
        }
        else
            return ExtTextOutW_orig(hdc, x, y, options, lprect, lpString, c, lpDx);
    }
    else if (options == ETO_IGNORELANGUAGE)
    {
        if(!GetClipBox(hdc, &textRect)) // GetClipBox has a performance impact, not ideal
            return ExtTextOutW_orig(hdc, x, y, options, lprect, lpString, c, lpDx);
    }
    else
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
    
    INT textRectWidth = textRect.right - textRect.left;
    INT textRectHeight = textRect.bottom - textRect.top;

    if (textRectWidth <= 0 || textRectHeight <= 0)
        return ExtTextOutW_orig(hdc, x, y, options, lprect, lpString, c, lpDx);
    
    COLORREF origTextColor = GetTextColor(hdc);
    COLORREF origBkColor = GetBkColor(hdc);
    COLORREF highlightSysClr = GetSysColor(COLOR_HIGHLIGHT);
    
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = textRectWidth;
    bmi.bmiHeader.biHeight      = -textRectHeight;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    VOID* pixels = nullptr;
    HDC memDC = CreateCompatibleDC(hdc);
    if (!memDC)
        return ExtTextOutW_orig(hdc, x, y, options, lprect, lpString, c, lpDx);

    HBITMAP dib = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &pixels, NULL, 0);
    if (!dib) {
        DeleteDC(memDC);
        return ExtTextOutW_orig(hdc, x, y, options, lprect, lpString, c, lpDx);
   }
    HGDIOBJ oldBmp  = SelectObject(memDC, dib);
    HGDIOBJ oldFont = SelectObject(memDC, GetCurrentObject(hdc, OBJ_FONT));
    SetTextAlign(memDC, GetTextAlign(hdc));
    SetBkMode(memDC, TRANSPARENT);

    RECT pRect;
    if (lprect) {
        pRect = {lprect->left - textRect.left, lprect->top - textRect.top, 
        lprect->right - textRect.left, lprect->bottom - textRect.top};
    }
    
    // The opaque text background applied to the background of text within editboxes
    if (options & ETO_OPAQUE && lprect)
    {
        if (origBkColor == highlightSysClr) {
            FillRect(hdc, lprect, (HBRUSH)GetStockObject(BLACK_BRUSH));
            HBRUSH brush = CreateSolidBrush(origBkColor);
            FillRect(memDC, &pRect, brush);
            DeleteObject(brush);
        }
        else if (origBkColor != 0x00383838) // bypass editbox BkColor on open/save dialogs
        {
            FillRect(hdc, lprect, (HBRUSH)GetStockObject(BLACK_BRUSH));
            HBRUSH brush = CreateSolidBrush(origBkColor);
            FillRect(memDC, &pRect, brush);
            DeleteObject(brush);
        }
    }
        
    // White text mask
    SetTextColor(memDC, RGB(255, 255, 255));

    // Render text with removed the opaque background text flag, because it has already been done by us
    WINBOOL res = ExtTextOutW_orig(memDC, x - textRect.left, y - textRect.top, options &~ ETO_OPAQUE, &pRect, lpString, c, lpDx);

    BYTE txtR = GetRValue(origTextColor);
    BYTE txtG = GetGValue(origTextColor);
    BYTE txtB = GetBValue(origTextColor);

    BYTE dstB = !(options & ETO_OPAQUE) ? 0 : GetBValue(origBkColor);
    BYTE dstG = !(options & ETO_OPAQUE) ? 0 : GetGValue(origBkColor);
    BYTE dstR = !(options & ETO_OPAQUE) ? 0 : GetRValue(origBkColor);

    // Alpha Blend
    BYTE alpha;
    BYTE* p = (BYTE*)pixels;
    for (INT i = 0; i < textRectWidth * textRectHeight; ++i) {
        // Rec.709/sRGB luminance 
        alpha = (BYTE)((p[2] * 54 + p[1] * 183 + p[0] * 19) >> 8);
        if (alpha) 
        {
            p[0] = (BYTE)((txtB * alpha + dstB * (255 - alpha)) >> 8); // B
            p[1] = (BYTE)((txtG * alpha + dstG * (255 - alpha)) >> 8); // G
            p[2] = (BYTE)((txtR * alpha + dstR * (255 - alpha)) >> 8); // R
            // full alpha highlight background fixes antialiased pixel bleed
            p[3] = (options & ETO_OPAQUE && (origBkColor == highlightSysClr)) ? 255 : alpha; // A
        }
        p += 4;
    }

    // Blend composited text
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    AlphaBlend(hdc, textRect.left, textRect.top, textRectWidth, textRectHeight,
               memDC, 0, 0, textRectWidth, textRectHeight, blend);

    SelectObject(memDC, oldFont);
    SelectObject(memDC, oldBmp);
    DeleteObject(dib);
    DeleteDC(memDC);
    return res;
}

// Prevent recursive calls within the DrawText class API and perform Alpha repair
// https://github.com/Maplespe/ExplorerBlurMica/blob/79c0ef4d017e32890e107ff98113507f831608b6/ExplorerBlurMica/HookDef.cpp#L1085
HRESULT WINAPI HookedDrawThemeTextEx(HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, LPCWSTR pszText,
        INT cchText, DWORD dwTextFlags, LPRECT pRect, const DTTOPTS* pOptions)
{
    std::wstring ThemeClassName = GetThemeClass(hTheme);

    if (pOptions == nullptr) {
        DTTOPTS Options = { sizeof(DTTOPTS) };
        GetThemeColor(hTheme, iPartId, iStateId, TMT_TEXTCOLOR, &Options.crText);
        Options.dwFlags = DTT_TEXTCOLOR;
        return DrawThemeTextEx_orig(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, (LPRECT)pRect, &Options);
    }
    
    if (pOptions && !(pOptions->dwFlags & DTT_CALCRECT) && !((pOptions->dwFlags & DTT_COMPOSITED) && ThemeClassName != L"Menu"))
    {
        HRESULT hr;
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

        BP_PAINTPARAMS bpParam;
        bpParam.cbSize = sizeof(BP_PAINTPARAMS);
        bpParam.dwFlags = BPPF_ERASE;
        bpParam.prcExclude = nullptr;
        bpParam.pBlendFunction = &bf;
        
        HDC hDC = nullptr;
        // Fit glow into text rectangle
        RECT PaintRect = *pRect;
        InflateRect(&PaintRect, 2, 0);
        HPAINTBUFFER pbuffer = BeginBufferedPaint(hdc, &PaintRect, BPBF_TOPDOWNDIB, &bpParam, &hDC);
        if (pbuffer && hDC)
        {
            g_DrawTextWithGlowEntry = TRUE;
            // Set the original DC information
            SelectObject(hDC, GetCurrentObject(hdc, OBJ_FONT));
            SetBkMode(hDC, GetBkMode(hdc));
            SetBkColor(hDC, GetBkColor(hdc));
            SetTextAlign(hDC, GetTextAlign(hdc));
            SetTextCharacterExtra(hDC, GetTextCharacterExtra(hdc));
            //SetTextColor(hDC, GetTextColor(hdc));

            COLORREF color = pOptions->crText;
            GetThemeColor(hTheme, iPartId, iStateId, TMT_TEXTCOLOR, &color);
         
            COLORREF GlowColor = 0xFFFFFFFF;
            INT GlowIntesity = 32;
            INT GlowRadius = 2;
            INT UnknownGlowParam = 255;
            HSL txtHsl = RGBToHSL(color);
            if (txtHsl.l >= 0.35) {
                GlowColor = 0xFF000000;
                if (txtHsl.l <= 0.8 && txtHsl.s <= 0.5)
                    GlowIntesity = 16;
            }
    
            // Move the text so the glow doesn't get cut off
            OffsetRect(pRect, 1, 0);

            hr = DrawTextWithGlow(hDC, pszText, cchText, pRect, dwTextFlags,
            color, GlowColor, GlowRadius, GlowIntesity, UnknownGlowParam, pOptions->pfnDrawTextCallback, pOptions->lParam);

            EndBufferedPaint(pbuffer, TRUE);
            g_DrawTextWithGlowEntry = FALSE;
            return (SUCCEEDED(hr)) ? TRUE : FALSE;
        }
    }
    return DrawThemeTextEx_orig(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, (LPRECT)pRect, pOptions);
}

// Convert to DrawThemeTextEx call
// https://github.com/Maplespe/ExplorerBlurMica/blob/79c0ef4d017e32890e107ff98113507f831608b6/ExplorerBlurMica/HookDef.cpp#L1072
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

BOOL SetCurrentTheme(LPCWSTR themeclass)
{
    if (g_hTheme != nullptr)
        g_hTheme = nullptr;

    g_hTheme = OpenThemeData(NULL, themeclass);
    return (g_hTheme) ? TRUE : FALSE;
}

VOID RevertSysColors()
{
    if (!SetCurrentTheme(L"sysmetrics")) {
        CloseThemeData(g_hTheme);
        return;
    }
    COLORREF aNewColors[ARRAYSIZE(SysColorElements)];

    for (UINT i = 0; i < ARRAYSIZE(SysColorElements); i++)
    {
        aNewColors[i] = GetThemeSysColor(g_hTheme, i);
    }
    SetSysColors(ARRAYSIZE(SysColorElements), SysColorElements, aNewColors);

    CloseThemeData(g_hTheme);
    g_hTheme = nullptr;
}

static COLORREF GetCustomSysColor(INT nIndex) 
{
    if (nIndex == COLOR_SCROLLBAR || nIndex == COLOR_BACKGROUND || nIndex == COLOR_MENU ||
        nIndex == COLOR_WINDOW || nIndex == COLOR_INACTIVEBORDER || nIndex == COLOR_INFOBK ||
        nIndex == COLOR_MENUBAR)
        return RGB(0, 0, 0);
    else if (nIndex == COLOR_GRADIENTACTIVECAPTION || nIndex == COLOR_INACTIVECAPTION)
        return (g_settings.AccentColorize) ? g_settings.AccentColor : RGB(0, 0, 0);
    else if (nIndex == COLOR_ACTIVECAPTION || nIndex == COLOR_GRADIENTINACTIVECAPTION)
        return (g_settings.AccentColorize) ? g_settings.AccentColor : RGB(32, 32, 32);
    else if (nIndex == COLOR_ACTIVEBORDER || nIndex == COLOR_BTNSHADOW)
        return RGB(32, 32, 32);
    else if (nIndex == COLOR_WINDOWFRAME || nIndex == COLOR_BTNHIGHLIGHT)
        return RGB(64, 64, 64);
    else if (nIndex == COLOR_MENUTEXT || nIndex == COLOR_CAPTIONTEXT ||
             nIndex == COLOR_BTNTEXT || nIndex == COLOR_INFOTEXT || nIndex == COLOR_HIGHLIGHTTEXT)
        return RGB(220, 220, 220);
    else if (nIndex == COLOR_WINDOWTEXT)
        return RGB(198, 198, 198);
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

COLORREF WINAPI HookedGetSysColor(INT nIndex) {    
    return GetCustomSysColor(nIndex);
}

HBRUSH WINAPI HookedGetSysColorBrush(INT nIndex) {
    COLORREF color = GetCustomSysColor(nIndex);
    if (!g_cachedSysColorBrushes[nIndex])
        g_cachedSysColorBrushes[nIndex] = CreateSolidBrush(color);
    return g_cachedSysColorBrushes[nIndex];
}

VOID ColorizeSysColors()
{
    // Stop recalling SetSysColors if syscolor changes have been applied.
    // SetSysColors redraws all top level windows causing flickering.
    if (GetSysColor(5) == RGB(0, 0, 0))
    {
        if (g_settings.AccentColorize && GetSysColor(COLOR_HIGHLIGHT) == g_settings.AccentColor)
            return;
        else if (!g_settings.AccentColorize)
            return ;
    }
    
    COLORREF aNewColors[ARRAYSIZE(SysColorElements)];
    for (UINT i = 0; i < ARRAYSIZE(SysColorElements); i++)
        aNewColors[i] = GetCustomSysColor(SysColorElements[i]);

    SetSysColors(ARRAYSIZE(SysColorElements), SysColorElements, aNewColors);
}

HRESULT WINAPI HookedGetThemeBitmap(
    HTHEME hTheme,
    INT iPartId,
    INT iStateId,
    INT iPropId,
    ULONG dwFlags,
    HBITMAP* phBitmap)
{   
    std::wstring ThemeClassName = GetThemeClass(hTheme);

    if (ThemeClassName == L"Tab" && iPartId == 10)
    {    
        BITMAP bm = {};
        if (!GetObject(*phBitmap, sizeof(bm), &bm))
            return FALSE;

        if (bm.bmBitsPixel != 32)
            return FALSE;

        INT size = bm.bmWidth * bm.bmHeight * 4;
        BYTE* pBits = new BYTE[size];
        if (!pBits)
            return FALSE;

        if (GetBitmapBits(*phBitmap, size, pBits) != size)
        {
            delete[] pBits;
            return FALSE;
        }

        for (INT y = 0; y < bm.bmHeight; y++)
        {
            BYTE* pPixel = pBits + bm.bmWidth * 4 * y;
            for (INT x = 0; x < bm.bmWidth; x++, pPixel += 4)
            {
                pPixel[0] = 0; //B
                pPixel[1] = 0; //G
                pPixel[2] = 0; //R
                pPixel[3] = 0; //A
            }
        }

        SetBitmapBits(*phBitmap, size, pBits);
        delete[] pBits;
    }
    return GetThemeBitmap_orig(hTheme, iPartId, iStateId, iPropId, dwFlags, phBitmap);
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
    if (ThemeClassName == L"ListView" && iPropId == TMT_TEXTCOLOR && iPartId == 1)
    {
        *pColor = (!g_IsSysThemeDarkMode && *pColor == 0x006D6D6D) ? RGB(0, 0, 0) : *pColor;
        return S_OK;
    }
    else if (ThemeClassName == L"PreviewPane" && (iPartId == 5 || iPartId == 7 || iPartId == 6) && iPropId == TMT_FILLCOLOR) {
        *pColor = (iPartId == 6) ? RGB(192, 192, 192) : RGB(255, 255, 255);
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
            *pColor = RGB(96,205,255);
            return S_OK;
        }
        else if (iPartId == TDLG_CONTENTPANE) {
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
        *pColor = (g_IsSysThemeDarkMode && *pColor == RGB(0, 0, 0)) ? RGB(255, 255, 255) : *pColor;
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
        else if (iPartId == 0) {
            *pColor = RGB(255, 255, 255);
            return S_OK;
        }
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
            *pColor = (g_IsSysThemeDarkMode && *pColor == RGB(0, 0, 0)) ? RGB(255, 255, 255) : *pColor;
            return S_OK;
        }
        else if ((iPartId == MENU_POPUPITEM || iPartId == 27) && (iStateId != 3 && iStateId != 4)) {
            *pColor = (g_IsSysThemeDarkMode && *pColor == RGB(0, 0, 0)) ? RGB(255, 255, 255) : *pColor;
            return S_OK;
        }
    }
    else if (ThemeClassName == L"Menu" && (iPropId == TMT_FILLCOLOR || iPropId == TMT_FILLCOLORHINT))
    {
        if (iPartId == 10) {
            if (g_settings.BorderActiveColor == DWMWA_COLOR_NONE)
                *pColor = (g_settings.BgType) ? RGB(0, 0, 0) : (g_settings.FillBg) ? RGB(32, 32, 32) : *pColor;
        }
        else
            *pColor = (g_settings.BgType) ? RGB(0, 0, 0) : (g_settings.FillBg) ? RGB(32, 32, 32) : *pColor;
        return S_OK;
    }
    else if ((ThemeClassName == L"Toolbar") && iPropId == TMT_TEXTCOLOR)
    {
        if (iPartId == 0 && iStateId != TS_DISABLED) {
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
        if (iPartId== 1) {
            *pColor = (g_IsSysThemeDarkMode) ? RGB(255, 255, 255) : RGB(0, 0, 0);
            return S_OK;
        }
        else if (iPartId == 4)
            return hr;        
    }
    else if (ThemeClassName == L"DragDrop" && iPropId == TMT_TEXTCOLOR)
    {
        *pColor = (iStateId == 1) ? RGB(96, 205, 255) : RGB(255, 255, 255);
        return S_OK;
    }
    else if (ThemeClassName == L"ChartView")
    {
        if ((iPartId == 30 || iPartId == 31 || iPartId == 32) && iStateId == 1) {
            *pColor = (g_settings.AccentColorize) ? g_settings.AccentColor : RGB(32, 102, 128);
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
    else if (ThemeClassName == L"TaskManager")
    {
        switch (iPartId)
        {
            case 2: case 41:
            case 42:
                *pColor = RGB(21, 21, 21);
                break;
            case 3: case 20:
            case 26:
                *pColor = RGB(0, 0, 0);
                break;
            case 4:
                *pColor = RGB(8, 4, 0);
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
                *pColor = RGB(241, 112, 122);
                break;
            case 14: case 15:
            case 16: case 17:
            case 18: case 19:
            case 24: case 25:
                *pColor = RGB(255, 255, 255);
                break;
            case 21: *pColor = RGB(97, 113, 186); break;
            case 22: *pColor = RGB(68, 79, 125); break;
            case 23: *pColor = RGB(64, 64, 64); break;
            case 27: *pColor = RGB(32, 36, 44); break;
            case 28: *pColor = RGB(32, 40, 56); break;
            case 29: *pColor = RGB(32, 44, 68); break;
            case 30: *pColor = RGB(32, 48, 80); break;
            case 31: *pColor = RGB(32, 52, 92); break;
            case 32: *pColor = RGB(32, 52, 104); break;
            case 33: *pColor = RGB(32, 60, 116); break;
            case 34: *pColor = RGB(32, 64, 128); break;
            case 35: *pColor = RGB(32, 68, 140); break;
            case 36: *pColor = RGB(32, 72, 152); break;
            case 37: *pColor = RGB(32, 76, 164); break;
            case 38: *pColor = RGB(17, 125, 187); break;
            case 39: *pColor = RGB(34, 38, 55); break;
            case 40: *pColor = RGB(35, 45, 71); break;
        }
        return S_OK;
    }
    else
    {
        if (iPropId == TMT_TEXTCOLOR)
        {
            *pColor = (g_IsSysThemeDarkMode && *pColor == RGB(0, 0, 0) ) ? RGB(255, 255, 255) : *pColor;
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
    std::array<HDC, 6> itemsview;
    std::array<HDC, 10> progressbar;
    std::array<HDC, 2> indeterminatebar;
    std::array<HDC, 2> trackbar;
    std::array<HDC, 24> trackbarthumb;
    std::array<HDC, 2> header;
    std::array<HDC, 1> previewseperator;
    std::array<HDC, 4> modulebutton;
    std::array<HDC, 4> modulelocationbutton;
    std::array<HDC, 8> modulesplitbutton;
    std::array<HDC, 12> navigationbutton;
    std::array<HDC, 5> toolbarbutton;
    std::array<HDC, 4> addressband;
    std::array<HDC, 3> menuitem;
    std::array<HDC, 1> dragdrop;

    BOOL CachePushButton(HDC, INT, INT);
    BOOL CacheRadioButton(HDC, LPCRECT, INT, INT);
    BOOL CacheCheckButton(HDC, LPCRECT, INT, INT);
    BOOL CacheCommandlinkButton(HDC, INT, INT);
    BOOL CacheCommandlinkGlyph(HDC, INT, INT);
    BOOL CacheListItem(HDC, INT, INT, INT);
    BOOL CacheListGroupHeader(HDC, INT, INT, INT);
    BOOL CacheScrollbar(HDC, INT, INT, INT);
    BOOL CacheScrollArrow(HDC, INT, INT);
    BOOL CacheTab(HDC, INT, INT);
    BOOL CacheCombobox(HDC, INT, INT, INT);
    BOOL CacheEditBox(HDC, INT, INT, INT);
    BOOL CacheTreeView(HDC, INT, INT, INT);
    BOOL CacheItemsView(HDC, INT, INT, INT);
    BOOL CacheProgressBar(HDC, INT, INT, INT);
    BOOL CacheIndeterminateBar(HDC, INT, INT);
    BOOL CacheTrackBar(HDC, INT, INT);
    BOOL CacheTrackBarThumb(HDC, INT, INT, INT);
    BOOL CacheTrackBarPointedThumb(HDC, INT, INT, INT);
    BOOL CacheHeader(HDC, INT, INT);
    BOOL CachePreviewPaneSeperator(HDC);
    BOOL CacheModuleButton(HDC, INT, INT);
    BOOL CacheModuleLocationButton(HDC, INT, INT);
    BOOL CacheModuleSplitButton(HDC,INT, INT, INT);
    BOOL CacheNavigationButton(HDC, INT, INT, INT);
    BOOL CacheToolbarButton(HDC, INT, INT);
    BOOL CacheAddressBand(HDC, INT, INT);
    BOOL CacheMenuItem(HDC, INT, INT, INT);
    BOOL CacheDragDrop(HDC);

    BOOL CreateDIB(HDC& elementHdc, HDC hDC, INT Width, INT Height)
    {
        if (!elementHdc) {
            if (!(elementHdc = CreateCompatibleDC(hDC)))
                return FALSE;
        }
        else if (!(elementHdc = CreateCompatibleDC(NULL)))
            return FALSE;

        BITMAPINFO bmi;
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = Width;
        bmi.bmiHeader.biHeight = -Height;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        if (HBITMAP hOldBmp = (HBITMAP)SelectObject(elementHdc, nullptr))
            DeleteObject(hOldBmp);
        
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
        for (HDC& hDC : previewseperator)
            DeleteHDC(hDC);
        for (HDC& hDC : modulebutton)
            DeleteHDC(hDC);
        for (HDC& hDC : modulelocationbutton)
            DeleteHDC(hDC);
        for (HDC& hDC : modulesplitbutton)
            DeleteHDC(hDC);
        for (HDC& hDC : navigationbutton)
            DeleteHDC(hDC);
        for (HDC& hDC : toolbarbutton)
            DeleteHDC(hDC);
        for (HDC& hDC : addressband)
            DeleteHDC(hDC);
        for (HDC& hDC : menuitem)
            DeleteHDC(hDC);
        for (HDC& hDC : dragdrop)
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
CThemeCache g_cache;

VOID DrawNineGridStretch(HDC hdc, HDC& srcDC, LPCRECT dstRect, INT left = 0, INT top = 0, INT right = 0, INT bottom = 0)
{
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

    if (!g_cache.scrollbar[index])
        if (!g_cache.CacheScrollbar(hdc, iPartId, iStateId, index))
            return FALSE;
    // Make scrollbar thinner by moving the rect's left and right edges
    RECT rc = (iPartId == SBP_THUMBBTNVERT) ? 
    RECT(static_cast<LONG>(pRect->left + ((pRect->right - pRect->left) * 0.25f)), pRect->top, static_cast<LONG>(pRect->right - ((pRect->right - pRect->left) * 0.25f)), pRect->bottom)
    : RECT(pRect->left, static_cast<LONG>(pRect->top + ((pRect->bottom - pRect->top) * 0.25f)) , pRect->right, static_cast<LONG>(pRect->bottom - ((pRect->bottom - pRect->top) * 0.25f)));
    DrawNineGridStretch(hdc, g_cache.scrollbar[index], &rc, 6, 6, 6, 6);
    return TRUE;
}

BOOL CThemeCache::CacheScrollbar(HDC hdc, INT iPartId, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    INT width = 18, height = 18;
    FLOAT cornerRadius = 4.f * scale;

    if (!g_cache.CreateDIB(g_cache.scrollbar[stateIndex], hdc, width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.scrollbar[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    D2D1_RECT_F Rect{D2D1::Rect(0, 0, width, height)};
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
    FLOAT width = static_cast<FLOAT>(pRect->right - pRect->left);
    FLOAT height = static_cast<FLOAT>(pRect->bottom - pRect->top);

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
    if (iStateId >= ABS_UPNORMAL && iStateId <= ABS_UPDISABLED)
    {
        points[0] = D2D1::Point2F(centerX-1 - triangleBaseWidth / 2.0f, centerY + triangleHeight / 2.0f);
        points[1] = D2D1::Point2F(centerX-1 - triangleBaseWidth / 2.0f, centerY+2 + triangleHeight / 2.0f);
        points[2] = D2D1::Point2F(centerX + triangleBaseWidth / 2.0f, centerY+2 + triangleHeight / 2.0f);
        points[3] = D2D1::Point2F(centerX + triangleBaseWidth / 2.0f, centerY + triangleHeight / 2.0f);
        points[4] = D2D1::Point2F(centerX, centerY - triangleHeight / 2.0f);
        points[5] = D2D1::Point2F(centerX-1, centerY - triangleHeight / 2.0f);
    }
    else if (iStateId >= ABS_DOWNNORMAL && iStateId <= ABS_DOWNDISABLED)
    {
        points[0] = D2D1::Point2F(centerX-1 - triangleBaseWidth / 2.0f, centerY - triangleHeight / 2.0f);
        points[1] = D2D1::Point2F(centerX-1 - triangleBaseWidth / 2.0f, centerY-2 - triangleHeight / 2.0f);
        points[2] = D2D1::Point2F(centerX + triangleBaseWidth / 2.0f, centerY-2 - triangleHeight / 2.0f);
        points[3] = D2D1::Point2F(centerX + triangleBaseWidth / 2.0f, centerY - triangleHeight / 2.0f);
        points[4] = D2D1::Point2F(centerX, centerY + triangleHeight / 2.0f);
        points[5] = D2D1::Point2F(centerX-1, centerY + triangleHeight / 2.0f);
    }
    else if (iStateId >= ABS_LEFTNORMAL && iStateId <= ABS_LEFTDISABLED)
    {
        points[0] = D2D1::Point2F(centerX + triangleHeight / 2.0f, centerY - 1 - triangleBaseWidth / 2.0f);
        points[1] = D2D1::Point2F(centerX+2 + triangleHeight / 2.0f, centerY - 1 - triangleBaseWidth / 2.0f);
        points[2] = D2D1::Point2F(centerX+2 + triangleHeight / 2.0f, centerY + triangleBaseWidth / 2.0f);
        points[3] = D2D1::Point2F(centerX + triangleHeight / 2.0f, centerY + triangleBaseWidth / 2.0f);
        points[4] = D2D1::Point2F(centerX - triangleHeight / 2.0f, centerY);
        points[5] = D2D1::Point2F(centerX - triangleHeight / 2.0f, centerY-1);
    }
    else if (iStateId >= ABS_RIGHTNORMAL && iStateId <= ABS_RIGHTDISABLED)
    {
        points[0] = D2D1::Point2F(centerX - triangleHeight / 2.0f, centerY-1 - triangleBaseWidth / 2.0f);
        points[1] = D2D1::Point2F(centerX-2 - triangleHeight / 2.0f, centerY-1 - triangleBaseWidth / 2.0f);
        points[2] = D2D1::Point2F(centerX-2 - triangleHeight / 2.0f, centerY + triangleBaseWidth / 2.0f);
        points[3] = D2D1::Point2F(centerX - triangleHeight / 2.0f, centerY + triangleBaseWidth / 2.0f);
        points[4] = D2D1::Point2F(centerX + triangleHeight / 2.0f, centerY);
        points[5] = D2D1::Point2F(centerX + triangleHeight / 2.0f, centerY-1);
    }

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

    if (!g_cache.pushbutton[index])
        if (!g_cache.CachePushButton(hdc, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_cache.pushbutton[index], &clipRect, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CachePushButton(HDC hdc, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    INT width = 18, height = 18;
    FLOAT cornerRadius = 3.f * scale;

    if (!g_cache.CreateDIB(g_cache.pushbutton[stateIndex], hdc, width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.pushbutton[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
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

    if (!g_cache.radiobutton[index])
        if (!g_cache.CacheRadioButton(hdc, pRect, iStateId, index))
            return FALSE;
    // Some theme parts are always fixed size so no stretching is needed
    DrawNineGridStretch(hdc, g_cache.radiobutton[index], pRect);
    return TRUE;
}

BOOL CThemeCache::CacheRadioButton(HDC hdc, LPCRECT pRect,  INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT width = pRect->right-pRect->left, height = pRect->bottom-pRect->top;
    if (!g_cache.CreateDIB(g_cache.radiobutton[stateIndex], hdc, width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, (INT)width, (INT)height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.radiobutton[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
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

    if (!g_cache.checkbutton[index])
        if (!g_cache.CacheCheckButton(hdc, pRect, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_cache.checkbutton[index], pRect);
    return TRUE;
}

BOOL CThemeCache::CacheCheckButton(HDC hdc, LPCRECT pRect, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT width = pRect->right-pRect->left, height = pRect->bottom-pRect->top;
    FLOAT cornerRadius = 3.f * scale;

    if (!g_cache.CreateDIB(g_cache.checkbutton[stateIndex], hdc, width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, (INT)width, (INT)height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.checkbutton[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
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
    const FLOAT width = static_cast<FLOAT>(pRect->right - pRect->left) - 0.5f;
    const FLOAT h = static_cast<FLOAT>(pRect->bottom - pRect->top) - 0.5f;

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

    if (!g_cache.commandlinkbutton[index])
        if (!g_cache.CacheCommandlinkButton(hdc, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_cache.commandlinkbutton[index], pRect, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheCommandlinkButton(HDC hdc, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    INT width = 18, height = 18;
    FLOAT cornerRadius = 4.f * scale;

    if (!g_cache.CreateDIB(g_cache.commandlinkbutton[stateIndex], hdc, width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.commandlinkbutton[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
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
    return FALSE;
}

BOOL PaintCommandLinkGlyph(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (iPartId != BP_COMMANDLINKGLYPH || !g_d2dFactory)
        return FALSE;
    
    INT index = (iStateId == CMDLGS_HOT) ? 1 : (iStateId == CMDLGS_PRESSED) ? 2
    : (iStateId == CMDLGS_DISABLED) ? 3 : 0;

    if (!g_cache.commandlinkglyph[index])
        if (!g_cache.CacheCommandlinkGlyph(hdc, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_cache.commandlinkglyph[index], pRect);
    return TRUE;
}

BOOL CThemeCache::CacheCommandlinkGlyph(HDC hdc, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    INT x = 0;
    INT width = 20 * scale, height = 20 * scale;
    if (!g_cache.CreateDIB(g_cache.commandlinkglyph[stateIndex], hdc, width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.commandlinkglyph[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
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
    HTHEME MyhTheme = nullptr;
    if ((MyhTheme = OpenThemeData(WindowFromDC(hdc), L"AddressComposited::ComboBox")) 
    && (iPartId == CP_BORDER || iPartId == CP_TRANSPARENTBACKGROUND))
    {
        CloseThemeData(MyhTheme);
        return TRUE;
    }
    return FALSE;
}

BOOL PaintCombobox(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory || (iPartId != CP_READONLY && iPartId != CP_BORDER))
        return FALSE;
    
    INT index = (iPartId == CP_READONLY) ? iStateId - 1 : iStateId + 3;
    
    if (!g_cache.combobox[index])
        if (!g_cache.CacheCombobox(hdc, iPartId, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_cache.combobox[index], pRect, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheCombobox(HDC hdc, INT iPartId, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 3.f * scale;
    INT width = 18, height = 18;

    if (!g_cache.CreateDIB(g_cache.combobox[stateIndex], hdc, width, height))
        return FALSE;
    
    // Direct2D render target
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.combobox[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
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
    HTHEME theme = NULL;
    if ((theme = OpenThemeData(WindowFromDC(hdc), L"AddressComposited::Edit")) && iPartId == EP_BACKGROUNDWITHBORDER)
    {
        CloseThemeData(theme);
        return TRUE;
    }
    CloseThemeData(theme);
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

    if (!g_cache.editbox[index])
        if (!g_cache.CacheEditBox(hdc, iPartId, iStateId, index))
            return FALSE;
    // hide the borders of the inner black background of EP_BACKGROUNDWITHBORDER theme class by expanding the black drawing.
    RECT rc = (iPartId == EP_BACKGROUNDWITHBORDER) ? RECT{pRect->left-1, pRect->top-1, pRect->right+3,pRect->bottom+1} : *pRect;
    DrawNineGridStretch(hdc, g_cache.editbox[index], &rc, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheEditBox(HDC hdc, INT iPartId, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 3.f * scale;
    INT x = 0, y = 0;
    INT width = 18, height = 18;
    if(!g_cache.CreateDIB(g_cache.editbox[stateIndex], hdc, width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.editbox[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    D2D1_ROUNDED_RECT rect = D2D1::RoundedRect(D2D1::RectF(x + .5f, y + .5f, width - .5f, height - .5f), cornerRadius, cornerRadius);
    pRenderTarget->BeginDraw();  
    if (iPartId == EP_BACKGROUNDWITHBORDER)
    {
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        pRenderTarget->CreateSolidColorBrush(MyD2D1Color(0, 0, 0), &brush);
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

    ID2D1DCRenderTarget* pRenderTarget = nullptr;
    if (FAILED(CreateBoundD2DRenderTarget(hdc, pRect, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    D2D1_RECT_F rect((FLOAT)pRect->left, (FLOAT)pRect->top,
                    (FLOAT)pRect->right - pRect->left, (FLOAT)pRect->bottom - pRect->top);

    pRenderTarget->BeginDraw();

    if (iPartId == 0)
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

    ID2D1DCRenderTarget* pRenderTarget = nullptr;
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
    FLOAT width = static_cast<FLOAT>(pRect->right - pRect->left);
    FLOAT height = static_cast<FLOAT>(pRect->bottom - pRect->top);
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

    if (!g_cache.tab[index])
        if (!g_cache.CacheTab(hdc, iStateId, index))
            return FALSE;
    RECT newRc = {pRect->left-1, pRect->top-1, pRect->right+1, pRect->bottom+1};
    DrawNineGridStretch(hdc, g_cache.tab[index], &newRc, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheTab(HDC hdc, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT width = 18, height = 18;

    if(!g_cache.CreateDIB(g_cache.tab[stateIndex], hdc, width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.tab[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    pRenderTarget->BeginDraw();
    if (iStateId == TIS_NORMAL)
    {
        D2D1_RECT_F rect{0, 0, (FLOAT)width, (FLOAT)height};
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        pRenderTarget->CreateSolidColorBrush(MyD2D1Color(0, 0 , 0, 0), &brush);
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
        const FLOAT desiredHeight = 2.0f;       
        const FLOAT widthPadding  = 5.0f;
        const FLOAT verticalOffset = 2.0f;      
    
        FLOAT pillLeft   = widthPadding;
        FLOAT pillRight  = width - widthPadding;
        FLOAT pillBottom = height - verticalOffset;
        FLOAT pillTop    = pillBottom - desiredHeight;

        FLOAT pillHeight = desiredHeight;
        FLOAT radius     = pillHeight / 2.0f * scale;

        D2D1_ROUNDED_RECT pillRect = D2D1::RoundedRect(
            D2D1::RectF(pillLeft, pillTop, pillRight, pillBottom),
            radius, radius
        );

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

    if (!g_cache.trackbar[index])
        if (!g_cache.CacheTrackBar(hdc, iPartId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_cache.trackbar[index], pRect, 2, 2, 2, 2);
    return TRUE;
}

BOOL CThemeCache::CacheTrackBar(HDC hdc, INT iPartId, INT stateIndex)
{
    INT width = 6, height = 6;
    if(!g_cache.CreateDIB(g_cache.trackbar[stateIndex], hdc, width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.trackbar[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
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

    if (!g_cache.trackbarthumb[index])
        if (!g_cache.CacheTrackBarThumb(hdc, iPartId, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_cache.trackbarthumb[index], pRect);
    return TRUE;
}

BOOL CThemeCache::CacheTrackBarThumb(HDC hdc, INT iPartId, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 3.f * scale;
    INT width = 10 * scale, height = 21 * scale;

    if (iPartId == TKP_THUMBVERT)
        width = std::exchange(height, width);
    
    if(!g_cache.CreateDIB(g_cache.trackbarthumb[stateIndex], hdc, width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.trackbarthumb[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
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

    if (!g_cache.trackbarthumb[index])
        if (!g_cache.CacheTrackBarPointedThumb(hdc, iPartId, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_cache.trackbarthumb[index], pRect);
    return TRUE;
}

BOOL CThemeCache::CacheTrackBarPointedThumb(HDC hdc, INT iPartId, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    INT width = 11 * scale, height = 19 * scale;

    if (iPartId == TKP_THUMBLEFT || iPartId == TKP_THUMBRIGHT)
        width = std::exchange(height, width);
    
    if(!g_cache.CreateDIB(g_cache.trackbarthumb[stateIndex], hdc, width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.trackbarthumb[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
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
    
    if (!g_cache.progressbar[index])
        if (!g_cache.CacheProgressBar(hdc, iPartId, iStateId, index))
            return FALSE;
    if (iPartId == PP_FILL)
        DrawNineGridStretch(hdc, g_cache.progressbar[index], pRect, 8, 10, 8, 10);
    else if (iPartId == PP_FILLVERT)
        DrawNineGridStretch(hdc, g_cache.progressbar[index], pRect, 10, 8, 10, 8);
    else
        DrawNineGridStretch(hdc, g_cache.progressbar[index], pRect, 8, 8, 9, 9);
    return TRUE;
}

BOOL CThemeCache::CacheProgressBar(HDC hdc, INT iPartId, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 3.f * scale;
    INT x = 0, y = 0;
    INT width = 18, height = 18;

    if (iPartId == PP_FILL) 
        width = 50, height = 23;
    else if (iPartId == PP_FILLVERT)
        width = 23, height = 50;

    if(!g_cache.CreateDIB(g_cache.progressbar[stateIndex], hdc, width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { x, y, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.progressbar[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
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
        INT overlayHeight = (pRect->bottom - pRect->top) / 3;
        INT overlayY = ((pRect->bottom - pRect->top) - overlayHeight) / 1.5f;
        overlayRect = RECT(pRect->left, overlayY, pRect->right, overlayY + overlayHeight);
    }
    else if (iPartId == PP_MOVEOVERLAYVERT)
    {
        FLOAT overlayWidth = (pRect->right - pRect->left) / 3.0f;
        FLOAT overlayX = ((pRect->right - pRect->left) - overlayWidth) / 1.5f;
        overlayRect = RECT(overlayX, pRect->top, overlayX + overlayWidth, pRect->bottom);
    }
    
    if (!g_cache.indeterminatebar[index])
        if (!g_cache.CacheIndeterminateBar(hdc, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_cache.indeterminatebar[index], &overlayRect, 6, 6, 5, 5);
    return TRUE;
}

BOOL CThemeCache::CacheIndeterminateBar(HDC hdc, INT iPartId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 3.f * scale;
    INT width = 12, height = 12;

    if (iPartId == PP_MOVEOVERLAYVERT)
        width = std::exchange(height, width);

    if(!g_cache.CreateDIB(g_cache.indeterminatebar[stateIndex], hdc, width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = {0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.indeterminatebar[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
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
    if (!g_d2dFactory || (iPartId != 0 && iPartId != LVP_LISTITEM 
        && iPartId != LVP_GROUPHEADER && iPartId != LVP_GROUPHEADERLINE && iPartId != LVP_COLUMNDETAIL))
        return FALSE;

    INT index;
    if (iPartId == 0 || iPartId == LVP_LISTITEM) 
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

    if (!g_cache.listview[index])
    {
        if (index <= 6)
        {
            if (!g_cache.CacheListItem(hdc, iPartId, iStateId, index))
                return FALSE;
        }
        else
            if (!g_cache.CacheListGroupHeader(hdc, iPartId, iStateId, index))
                return FALSE;
    }
    DrawNineGridStretch(hdc, g_cache.listview[index], pRect, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheListItem(HDC hdc, INT iPartId, INT iStateId, INT stateIndex)
{
    if (iPartId != 0 && iPartId != LVP_LISTITEM)
        return FALSE;
    
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT x = 0, y = 0;
    INT width = 18, height = 18;

    if(!g_cache.CreateDIB(g_cache.listview[stateIndex], hdc, width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.listview[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
    if (iPartId == 0)
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

BOOL CThemeCache::CacheListGroupHeader(HDC hdc, INT iPartId, INT iStateId, INT stateIndex)
{
    if (iPartId != LVP_GROUPHEADER && iPartId != LVP_GROUPHEADERLINE && iPartId != LVP_COLUMNDETAIL)
        return FALSE;

    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT x = 0, y = 0;
    INT width = (iPartId == LVP_COLUMNDETAIL) ? 2 : 18, height = (iPartId == LVP_COLUMNDETAIL) ? 1 : 18;

    if (!g_cache.CreateDIB(g_cache.listview[stateIndex], hdc, width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.listview[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
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

BOOL PaintTreeView(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory || (iPartId != 0 && iPartId != TVP_TREEITEM))
        return FALSE;
    
    INT index = (iPartId == 0) ? 0 : (iStateId == TREIS_HOT) ? 1 : (iStateId == TREIS_SELECTED) ? 2 :
                (iStateId == TREIS_SELECTEDNOTFOCUS) ? 3 : 4;

    if (!g_cache.treeview[index])
        if (!g_cache.CacheTreeView(hdc, iPartId, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_cache.treeview[index], pRect, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheTreeView(HDC hdc, INT iPartId, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT x = 0, y = 0;
    INT width = 18, height = 18;

    if (!g_cache.CreateDIB(g_cache.treeview[stateIndex], hdc, width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.treeview[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;
    
    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, width, height), cornerRadius, cornerRadius);
    pRenderTarget->BeginDraw();
    if (iPartId == 0)
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
            FLOAT lineLength = height * 0.45f;
            FLOAT offsetY = (height - lineLength) / 2;
            brush->SetColor(IsAccentColorPossibleD2D(102, 206, 255, SystemAccentColorLight2));

            D2D1_POINT_2F p1 = D2D1::Point2F(x + 0.5f, y + offsetY);
            D2D1_POINT_2F p2 = D2D1::Point2F(x + 0.5f, y + offsetY + lineLength);
            pRenderTarget->DrawLine(p1, p2, brush.Get());
            p1.x += 1.0f; p2.x += 1.0f;
            pRenderTarget->DrawLine(p1, p2, brush.Get());
        }
    }
    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return TRUE;
}

BOOL PaintItemsView(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory || (iPartId != 1 && iPartId != 3 && iPartId != 6))
        return FALSE;

    INT index = (iPartId == 1 && (iStateId % 2 == 1)) ? 0 :
                (iPartId == 1 && (iStateId % 2 == 0)) ? 1 : (iPartId == 6) ? iStateId + 1 : iStateId + 3;
    
    if (!g_cache.itemsview[index])
        if (!g_cache.CacheItemsView(hdc, iPartId, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_cache.itemsview[index], pRect, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheItemsView(HDC hdc, INT iPartId, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT x = 0, y = 0;
    INT width = 18, height = 18;

    if(!g_cache.CreateDIB(g_cache.itemsview[stateIndex], hdc, width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { x, y, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.itemsview[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
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
    
    if (!g_cache.header[index])
        if (!g_cache.CacheHeader(hdc, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_cache.header[index], pRect, 12, 0, 11, 12);
    return TRUE;
}

BOOL CThemeCache::CacheHeader(HDC hdc, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 6.f * scale;
    INT x = 0, y = 0;
    INT width = 24, height = 24;

    if(!g_cache.CreateDIB(g_cache.header[stateIndex], hdc, width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { x, y, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.header[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
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

BOOL PaintPreviewPaneSeperator(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (!g_d2dFactory || (iPartId != 3 && iPartId != 4))
        return FALSE;

    if (!g_cache.previewseperator[0])
        if (!g_cache.CachePreviewPaneSeperator(hdc))
            return FALSE;
    
    RECT rc{pRect->left+1, pRect->top, pRect->right, pRect->bottom};
    DrawNineGridStretch(hdc, g_cache.previewseperator[0], &rc, 1, 0, 0, 0);
    return TRUE;
}

BOOL CThemeCache::CachePreviewPaneSeperator(HDC hdc)
{
    INT x = 0, y = 0;
    INT width = 3, height = 3;
    if(!g_cache.CreateDIB(g_cache.previewseperator[0], hdc, width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { x, y, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.previewseperator[0], &rc, g_d2dFactory, &pRenderTarget)))
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

    if (!g_cache.modulebutton[index])
        if (!g_cache.CacheModuleButton(hdc, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_cache.modulebutton[index], pRect, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheModuleButton(HDC hdc, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT x = 0, y = 0;
    INT width = 18, height = 18;

    if(!g_cache.CreateDIB(g_cache.modulebutton[stateIndex], hdc, width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { x, y, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.modulebutton[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
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

    if (!g_cache.modulelocationbutton[index])
        if (!g_cache.CacheModuleLocationButton(hdc, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_cache.modulelocationbutton[index], pRect, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheModuleLocationButton(HDC hdc, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT x = 0, y = 0;
    INT width = 18, height = 18;
    if(!g_cache.CreateDIB(g_cache.modulelocationbutton[stateIndex], hdc, width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { x, y, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.modulelocationbutton[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
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

    if (!g_cache.modulesplitbutton[index])
        if (!g_cache.CacheModuleSplitButton(hdc, iPartId, iStateId, index))
            return FALSE;
    RECT newRc = (iPartId == 4 && iStateId == 4) ? RECT{pRect->left, pRect->top, pRect->right+2, pRect->bottom} : *pRect;
    DrawNineGridStretch(hdc, g_cache.modulesplitbutton[index], &newRc, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheModuleSplitButton(HDC hdc, INT iPartId, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT x = 0, y = 0;
    INT width = 18, height = 18;

    if(!g_cache.CreateDIB(g_cache.modulesplitbutton[stateIndex], hdc, width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { x, y, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.modulesplitbutton[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
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

    if (!g_cache.navigationbutton[index])
        if (!g_cache.CacheNavigationButton(hdc, iPartId, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_cache.navigationbutton[index], pRect);
    return TRUE;
}

BOOL CThemeCache::CacheNavigationButton(HDC hdc, INT iPartId, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT x = 0, y = 0;
    INT width = 30 * scale, height = 30 * scale;
    
    if (iPartId == NAV_MENUBUTTON)
        width = 13 * scale, height = 27 * scale;
    
    if(!g_cache.CreateDIB(g_cache.navigationbutton[stateIndex], hdc, width, height))
        return FALSE;
    
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { x, y, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.navigationbutton[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
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

    if (!g_cache.toolbarbutton[index])
        if (!g_cache.CacheToolbarButton(hdc, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_cache.toolbarbutton[index], pRect, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheToolbarButton(HDC hdc, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT width = 18, height = 18;

    if (!g_cache.CreateDIB(g_cache.toolbarbutton[stateIndex], hdc, width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.toolbarbutton[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
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
    INT width = (pRect->right - pRect->left), height = (pRect->bottom - pRect->top);

    ID2D1DCRenderTarget* pRenderTarget = nullptr;
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

    if (!g_cache.addressband[index])
        if (!g_cache.CacheAddressBand(hdc, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_cache.addressband[index], pRect, 12, 12, 11, 11);
    return TRUE;
}

BOOL CThemeCache::CacheAddressBand(HDC hdc, INT iStateId, INT stateIndex)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 5.f * scale;
    INT width = 24, height = 24;

    if (!g_cache.CreateDIB(g_cache.addressband[stateIndex], hdc, width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.addressband[stateIndex], &rc, g_d2dFactory, &pRenderTarget)))
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
            fillColor = MyD2D1Color(0, 0, 0);
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
    if (!g_d2dFactory || (iPartId != 27 && iPartId != MENU_POPUPITEM && iPartId != MENU_BARITEM))
        return FALSE;
    if ((iPartId == 27 || iPartId == MENU_POPUPITEM) && iStateId != 2) return FALSE;
    if ((iPartId == MENU_BARITEM) && 
        (iStateId == MBI_NORMAL || iStateId == MBI_DISABLED || iStateId == MBI_DISABLEDPUSHED)) return FALSE;

    INT index = (iPartId == 27 || iPartId == MENU_POPUPITEM) ? 0 : (MBI_PUSHED) ? 1 : 2;

    if (!g_cache.menuitem[index])
        if (!g_cache.CacheMenuItem(hdc, iPartId, iStateId, index))
            return FALSE;
    DrawNineGridStretch(hdc, g_cache.menuitem[index], pRect, 9, 9, 8, 8);
    return TRUE;
}

BOOL CThemeCache::CacheMenuItem(HDC hdc, INT iPartId, INT iStateId, INT indexState)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    FLOAT cornerRadius = 4.f * scale;
    INT width = 18, height = 18;

    if (!g_cache.CreateDIB(g_cache.menuitem[indexState], hdc, width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.menuitem[indexState], &rc, g_d2dFactory, &pRenderTarget)))
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
    else 
    {
        D2D1_COLOR_F fillColor = (iPartId == MBI_PUSHED) ? MyD2D1Color(64, 144, 144, 144) : MyD2D1Color(128, 96, 96, 96);
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
    if (!g_d2dFactory || iPartId != 7)
        return FALSE;

    if (!g_cache.dragdrop[0])
        if (!g_cache.CacheDragDrop(hdc))
            return FALSE;
    DrawNineGridStretch(hdc, g_cache.dragdrop[0], pRect);
    return TRUE;
}

BOOL CThemeCache::CacheDragDrop(HDC hdc)
{
    FLOAT scale = (FLOAT)g_Dpi / USER_DEFAULT_SCREEN_DPI;
    INT width = 108, height = 108;
    FLOAT cornerRadius = 4.f * scale;

    if (!g_cache.CreateDIB(g_cache.dragdrop[0], hdc, width, height))
        return FALSE;

    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> pRenderTarget;
    RECT rc = { 0, 0, width, height};
    if (FAILED(CreateBoundD2DRenderTarget(g_cache.dragdrop[0], &rc, g_d2dFactory, &pRenderTarget)))
        return FALSE;
    
    D2D1_ROUNDED_RECT roundedRect = { D2D1::RectF(0, 0, width, height), cornerRadius, cornerRadius};
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
    pRenderTarget->BeginDraw();

    pRenderTarget->CreateSolidColorBrush(MyD2D1Color(128, 96, 96, 96), &brush);
    pRenderTarget->FillRoundedRectangle(&roundedRect, brush.Get());

    auto hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {Wh_Log(L"Failed D2D drawing [ERROR]: 0x%08X\n", hr); return FALSE;}
    return FALSE;
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
            HTHEME hGroupBoxTheme;
            if (hTheme == (hGroupBoxTheme = OpenThemeData(WindowFromDC(hdc), L"Button")))
            {
                if (PaintGroupBox(hdc, iPartId, iStateId, pRect, pClipRect)) {
                    CloseThemeData(hGroupBoxTheme);
                    return S_OK;
                }
            }
            CloseThemeData(hGroupBoxTheme);
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
        HTHEME theme = NULL;
        if (hTheme == (theme = OpenThemeData(WindowFromDC(hdc), L"Indeterminate::Progress")))
        {
            if (PaintIndeterminateProgressBar(hdc, iPartId, iStateId, pRect))
            {
                CloseThemeData(theme);
                return S_OK;
            }
            CloseThemeData(theme);
        }
        else if (PaintProgressBar(hdc, iPartId, iStateId, pRect)) 
        {
            CloseThemeData(theme);
            return S_OK;
        }
        CloseThemeData(theme);
    } 
    else if (ThemeClassName == L"ListView")
    {
        if (PaintListView(hdc, iPartId, iStateId, pRect))
            return S_OK;
    }
    else if (ThemeClassName == L"TreeView")
    {
        if (PaintTreeView(hdc, iPartId, iStateId, pRect))
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
        HTHEME theme = NULL;
        if ((theme = OpenThemeData(WindowFromDC(hdc), L"BB::Toolbar")))
        {
            if (PaintToolbarSplitDropDown(hdc, iPartId, iStateId, pRect)) {
                CloseThemeData(theme);
                return S_OK;
            }
            CloseThemeData(theme);
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
    }
    else if (ThemeClassName == L"DragDrop")
    {
        if (PaintDragDrop(hdc, iPartId, iStateId, pRect))
            return S_OK;
    }

    HRESULT hr = DrawThemeBackground_orig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
    
    if((ThemeClassName == L"Rebar" && (iPartId == 3 || iPartId == 6) && iStateId == 0)
        || (ThemeClassName == L"Header" && (iPartId == 0 || (iPartId == 1 && (iStateId == 1 || iStateId == 4 || iStateId == 7 ))))
        || (ThemeClassName == L"TaskDialog" && iPartId == 15 && iStateId == 0)
        || (ThemeClassName == L"Tab" && iPartId == 9)
        || (ThemeClassName == L"Status" && iPartId == 0)
        || (ThemeClassName == L"Tooltip" && iPartId == 1))
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
            ((iPartId == MENU_POPUPITEM || iPartId == 27) && iStateId != 2)))
    {
        
        RECT clipRect{*pRect};
        if (pClipRect)
            IntersectRect(&clipRect, pRect, pClipRect);
        if (g_settings.BgType)
            FillRect(hdc, &clipRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        else if (g_settings.FillBg) {
            HBRUSH brush = CreateSolidBrush(RGB(32, 32, 32));
            FillRect(hdc, &clipRect, brush);
            DeleteObject(brush);
        }
        return S_OK;
    }
    else if (ThemeClassName == L"Toolbar" && iPartId == 0) {
        HTHEME Toolbar;
        if ((Toolbar = OpenThemeData(WindowFromDC(hdc), L"Placesbar::Toolbar"))) {
            FillRect(hdc, pRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
            CloseThemeData(Toolbar);
            return S_OK;
        }
        CloseThemeData(Toolbar);
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
        if (PaintPreviewPaneSeperator(hdc, iPartId, iStateId, pRect))
            return S_OK;
    }
    else if (ThemeClassName == L"Progress")
    {
        HTHEME theme = NULL;
        if (hTheme == (theme = OpenThemeData(WindowFromDC(hdc), L"Indeterminate::Progress")))
        {
            if (PaintIndeterminateProgressBar(hdc, iPartId, iStateId, pRect))
            {
                CloseThemeData(theme);
                return S_OK;
            }
            CloseThemeData(theme);
        }
        else if (PaintProgressBar(hdc, iPartId, iStateId, pRect)) 
        {
            CloseThemeData(theme);
            return S_OK;
        }
        CloseThemeData(theme);
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

    if ((ThemeClassName == L"Rebar" && (iPartId == 3 || iPartId == 6) && iStateId == 0) 
        || (ThemeClassName == L"TreeView" && iPartId == 0))
    {
        return S_OK;    
    }
    else if ((ThemeClassName == L"PreviewPane" && iPartId == 1)
        || (ThemeClassName == L"Header" && iPartId == 0)
        || (ThemeClassName == L"CommandModule" && iPartId == 1 && iStateId == 0)
        || (ThemeClassName == L"TaskDialog" && (iPartId == 4 || iPartId == 17 ||  iPartId == 15 || iPartId == 8) && iStateId == 0)
        || (ThemeClassName == L"TaskDialog" && iPartId == 1)
        || (ThemeClassName == L"AeroWizard" && (iPartId == 1 || iPartId == 2 || iPartId == 3 || iPartId == 4))
        || (ThemeClassName == L"CommonItemsDialog" && iPartId == 1)
        || (ThemeClassName == L"ControlPanel" && (iPartId == 2 || iPartId == 17 || iPartId == 18 || iPartId == 12 || iPartId == 13)))
    {
        FillRect(hdc, pRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        return S_OK;
    }
    return hr;
}

HRESULT WINAPI HookedGetThemeMargins(HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, INT iPropId, RECT* prc, MARGINS *pMargins)
{
    std::wstring ThemeClassName = GetThemeClass(hTheme);

    auto ret = GetThemeMargins_orig(hTheme, hdc, iPartId, iStateId, iPropId, prc, pMargins);

    if (ThemeClassName == L"Tooltip" && iPartId == 1) {
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

VOID ApplyFrameExtension(HWND hWnd) 
{
    MARGINS margins = {-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(hWnd, &margins);
}

//https://github.com/ALTaleX531/TranslucentFlyouts/blob/master/TFMain/EffectHelper.hpp
// Required for flyouts with DWM SYSTEMBACKDROP effects
VOID TriggerWindowNCRendering(HWND hwnd)
{
    // NOTICE WINDOWS THAT WE HAVE ACTIVATED THE WINDOW
    DefWindowProcW(hwnd, WM_NCACTIVATE, TRUE, 0);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_DRAWFRAME | SWP_NOACTIVATE);
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
        g_bb.fEnable = TRUE;
        g_bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION | DWM_BB_TRANSITIONONMAXIMIZED;
        // Blurs window client area
        HRGN hRgn = CreateRectRgn(0, 0, -1, -1);
        g_bb.hRgnBlur = hRgn;
        g_bb.fTransitionOnMaximized = TRUE;

        DwmEnableBlurBehindWindow(hWnd, &g_bb);
        DeleteObject(hRgn);

        g_accent.AccentState = ACCENT_STATE_ENABLE_ACRYLICBLURBEHIND;
        g_accent.GradientColor = g_settings.AccentBlurBehindClr;
        //accent.AccentFlags = ACCENT_FLAG_ENABLE_BORDER;
        //accent.AnimationId = NULL;

        g_attrib.Attrib = WCA_ACCENT_POLICY;
        g_attrib.pvData = &g_accent;
        g_attrib.cbData = sizeof(g_accent);

        typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
        auto SetWindowCompositionAttribute = (pSetWindowCompositionAttribute) GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");
        if (SetWindowCompositionAttribute) 
            SetWindowCompositionAttribute(hWnd, &g_attrib);
    }
}

VOID SetSystemBackdrop(HWND hWnd, const UINT type) {
    DwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE , &type, sizeof(UINT));
}

VOID EnableColoredTitlebar(HWND hWnd) {
    g_settings.g_TitlebarColor = g_settings.TitlebarActiveColor;
    DwmSetWindowAttribute(hWnd, DWMWA_CAPTION_COLOR, &g_settings.g_TitlebarColor, sizeof(COLORREF));
}

VOID EnableCaptionTextColor(HWND hWnd) {
    g_settings.g_CaptionColor = g_settings.CaptionActiveTextColor;
    DwmSetWindowAttribute(hWnd, DWMWA_TEXT_COLOR, &g_settings.g_CaptionColor, sizeof(COLORREF));
}

VOID EnableColoredBorder(HWND hWnd) {
    g_settings.g_BorderColor = g_settings.BorderActiveColor;
    DwmSetWindowAttribute(hWnd, DWMWA_BORDER_COLOR, &g_settings.g_BorderColor, sizeof(COLORREF));
}

VOID SetCornerType(HWND hWnd, const UINT type) {
    DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &type , sizeof(UINT));
}

constexpr UINT MN_SIZEWINDOW{ 0x01E2 };

// https://github.com/ALTaleX531/TranslucentFlyouts/blob/017970cbac7b77758ab6217628912a8d551fcf7c/TFMain/MenuHandler.cpp#L236C31-L236C47
LRESULT CALLBACK FlyoutsSubclassProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR dwRefData)
{
    switch (uMsg)
    {
        case WM_PRINT:
        {   
            if (!IsWindowClass(hWnd, MENUPOPUP_CLASS))
                break;     
            // Let the system/theme draw first
            LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

            if (g_settings.BorderRainbowFlag) {
                SendMessage(hWnd, g_msgRainbowTimer, RAINBOW_LOAD, 0);
                break;
            }
            else if (!g_settings.BorderFlag || g_settings.BorderActiveColor == DWMWA_COLOR_DEFAULT)
                break;
            
            HDC hdc = reinterpret_cast<HDC>(wParam);
            // Remove desktop context menu white outline
            RECT paintRect{};
            GetWindowRect(hWnd, &paintRect);
            OffsetRect(&paintRect, -paintRect.left, -paintRect.top);
            
            if (g_settings.BorderActiveColor == DWMWA_COLOR_NONE)
                FrameRect(hdc, &paintRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
            else {
                HBRUSH brush = CreateSolidBrush(g_settings.BorderActiveColor);
                FrameRect(hdc, &paintRect, brush);
                DeleteObject(brush);
            }
            

            return result;
        }
        // The WM_PRINT command is not fired when the user selects a menu item
        // and then very quickly activates a new menu with the keyboard (SHIFT+F10)
        // or by right-clicking
        case WM_NCPAINT:
        {    
            if (g_settings.BorderRainbowFlag) {
                SendMessage(hWnd, g_msgRainbowTimer, RAINBOW_LOAD, 0);
                break;
            }
            else if (!g_settings.BorderFlag || g_settings.BorderActiveColor == DWMWA_COLOR_DEFAULT)
                break;
            
            HDC hdc = GetWindowDC(hWnd);
            if (wParam != NULLREGION && wParam != ERROR)
				SelectClipRgn(hdc, reinterpret_cast<HRGN>(wParam));
            
            RECT windowRect{};
            GetWindowRect(hWnd, &windowRect);

            MENUBARINFO mbi{ sizeof(MENUBARINFO) };
            GetMenuBarInfo(hWnd, OBJID_CLIENT, 0, &mbi);
            MARGINS mr{ mbi.rcBar.left - windowRect.left, windowRect.right - mbi.rcBar.right, mbi.rcBar.top - windowRect.top, windowRect.bottom - mbi.rcBar.bottom };

			RECT paintRect{};
			GetWindowRect(hWnd, &paintRect);
			OffsetRect(&paintRect, -paintRect.left, -paintRect.top);
            ExcludeClipRect(hdc, paintRect.left + mr.cxLeftWidth, paintRect.top + mr.cyTopHeight, paintRect.right - mr.cxRightWidth, paintRect.bottom - mr.cyBottomHeight);
            PatBlt(hdc, paintRect.left, paintRect.top, paintRect.right-paintRect.left, paintRect.bottom-paintRect.top, BLACKNESS);

            if (g_settings.BorderActiveColor == DWMWA_COLOR_NONE)
                FrameRect(hdc, &paintRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
            else {
                HBRUSH brush = CreateSolidBrush(g_settings.BorderActiveColor);
                FrameRect(hdc, &paintRect, brush);
                DeleteObject(brush);
            }

            return 0;
        }
        case MN_SIZEWINDOW:
        {
            if (!IsWindowClass(hWnd, MENUPOPUP_CLASS))
                break;
            HandleEffects(hWnd);
            break;
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

VOID CALLBACK MyRainbowTimerProc(HWND, UINT, UINT_PTR t_id, DWORD)
{
    HWND WndHwnd= nullptr;
    RainbowData* pRainbowWndData = nullptr;
    {
        std::lock_guard<std::mutex> guard(g_rainbowWindowsMutex);
        for(auto& hWnd : g_rainbowWindows)
        {
            RainbowData* pTempRainbowWndData = reinterpret_cast<RainbowData*>(
                GetPropW(hWnd, g_RainbowPropStr.c_str()));
            
            if (pTempRainbowWndData && pTempRainbowWndData->timerId == t_id)
            {
                WndHwnd = hWnd;
                pRainbowWndData = pTempRainbowWndData;
                break;
            }
        }
    }

    if (!WndHwnd)
        return;
    
    DOUBLE tNow = TimerGetSeconds();
    DOUBLE effectiveTime = (tNow - pRainbowWndData->baseTime) - pRainbowWndData->pausedTotal;
    DOUBLE wndHue = fmod(pRainbowWndData->initialHue + effectiveTime * g_settings.RainbowSpeed, 360.0);

    if (g_settings.TitlebarRainbowFlag)
    {
        COLORREF titlebarColor = HSLToRGB(wndHue, 1.0f, 0.5f); 
        DwmSetWindowAttribute_orig(WndHwnd, DWMWA_CAPTION_COLOR, &titlebarColor, sizeof(COLORREF));
    }
    if (g_settings.CaptionRainbowFlag)
    {
        COLORREF captionColor;
        if (g_settings.TitlebarRainbowFlag)
            captionColor = HSLToRGB(fmod(wndHue + 120.0f, 360.0f), 1.0f, 0.5f);
        else
            captionColor = HSLToRGB(wndHue, 1.0f, 0.5f);
        DwmSetWindowAttribute_orig(WndHwnd, DWMWA_TEXT_COLOR, &captionColor, sizeof(COLORREF));
    }
    if (g_settings.BorderRainbowFlag)
    {
        COLORREF borderColor = HSLToRGB(wndHue, 1.0f, 0.5f);  
        DwmSetWindowAttribute_orig(WndHwnd, DWMWA_BORDER_COLOR, &borderColor, sizeof(COLORREF));
    }
}

VOID AddFlyoutsSubclass(HWND hWnd)
{
    auto it = g_subclassedflyouts.find(hWnd);
    if (it == g_subclassedflyouts.end()) {
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, FlyoutsSubclassProc, NULL)) {
            std::lock_guard<std::mutex> guard(g_subclassedflyoutsmutex);
            g_subclassedflyouts.insert(hWnd);
        }
    }
}

VOID RemoveOrUnloadFlyoutSubclass(HWND hWnd = nullptr, BOOL Clean = FALSE)
{
    if (Clean && !g_subclassedflyouts.empty())
    {
        std::unordered_set<HWND> subclassedflyouts;
        {
            std::lock_guard<std::mutex> guard(g_subclassedflyoutsmutex);
            subclassedflyouts = std::move(g_subclassedflyouts);
            g_subclassedflyouts.clear();
        }
        for(auto hwnd : subclassedflyouts)
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, FlyoutsSubclassProc);
        subclassedflyouts.clear();
    }
    else 
    {
        auto it = g_subclassedflyouts.find(hWnd);
        if (it != g_subclassedflyouts.end()) {
            std::lock_guard<std::mutex> guard(g_subclassedflyoutsmutex);
            g_subclassedflyouts.erase(hWnd);
        }
    }
}

VOID RemoveOrUnloadWindowsHooks(BOOL);

LRESULT CALLBACK CallWndProc(INT nCode, WPARAM wParam, LPARAM lParam)
{
    const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
    if (nCode != HC_ACTION) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    switch (cwp->message)
    {
        case WM_CREATE:
        {
            // Intercept flyouts
            if (!IsWindowClass(cwp->hwnd, MENUPOPUP_CLASS) && !IsWindowClass(cwp->hwnd, L"DropDown") && !IsWindowClass(cwp->hwnd, L"ViewControlClass"))
                break;
            AddFlyoutsSubclass(cwp->hwnd);
        }
        case WM_WINDOWPOSCHANGED:
        {
            if (!IsWindowEligible(cwp->hwnd))
                break;

            BOOL fullscreen = IsWindowMaximizedOrFullscreen(cwp->hwnd);
            // hide colored borders on fullscreen/maximized windows
            if (fullscreen) {
                UINT borderType;
                DwmGetWindowAttribute(cwp->hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &borderType, sizeof(UINT));
                if (borderType != DONTROUND)
                    DwmSetWindowAttribute(cwp->hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &DONTROUND, sizeof(UINT));
            }
            else if (!fullscreen) {
                UINT borderType;
                DwmGetWindowAttribute(cwp->hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &borderType, sizeof(UINT));
                if (borderType != g_settings.CornerPref)
                    DwmSetWindowAttribute(cwp->hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &g_settings.CornerPref, sizeof(UINT));
            }

            break;
        }
        case WM_ACTIVATE:
        {
            BOOL isMinimized = HIWORD(cwp->wParam);
            if (!isMinimized && IsWindowEligible(cwp->hwnd))
            {
                WORD activationState = LOWORD(cwp->wParam);

                if ((activationState == WA_ACTIVE || activationState == WA_CLICKACTIVE))
                {
                    if(g_settings.TitlebarFlag && !g_settings.TitlebarRainbowFlag)
                    {
                        g_settings.g_TitlebarColor = g_settings.TitlebarActiveColor;
                        DwmSetWindowAttribute(cwp->hwnd, DWMWA_CAPTION_COLOR, &g_settings.g_TitlebarColor, sizeof(COLORREF));
                    }
                    if(g_settings.CaptionTextFlag && !g_settings.CaptionRainbowFlag)
                    {
                        g_settings.g_CaptionColor = g_settings.CaptionActiveTextColor;
                        DwmSetWindowAttribute(cwp->hwnd, DWMWA_TEXT_COLOR, &g_settings.g_CaptionColor, sizeof(COLORREF));
                    }
                    if((g_settings.BorderFlag && !g_settings.BorderRainbowFlag))
                    {
                        g_settings.g_BorderColor = g_settings.BorderActiveColor;
                        DwmSetWindowAttribute(cwp->hwnd, DWMWA_BORDER_COLOR, &g_settings.BorderActiveColor, sizeof(COLORREF));
                    }
                }
                else if (activationState == WA_INACTIVE)
                {
                    // Workaround in order to bypass the neutral background color in SystemBackdrop effects on inactive windows
                    //if (g_settings.BgType > g_settings.AccentBlurBehind)
                        //DefWindowProcW(cwp->hwnd, WM_NCACTIVATE, TRUE, 0);
                
                    if(g_settings.TitlebarFlag && !g_settings.TitlebarRainbowFlag)
                    {
                        g_settings.g_TitlebarColor = g_settings.TitlebarInactiveColor;
                        DwmSetWindowAttribute(cwp->hwnd, DWMWA_CAPTION_COLOR, &g_settings.g_TitlebarColor, sizeof(COLORREF));
                    }
                    if(g_settings.CaptionTextFlag && !g_settings.CaptionRainbowFlag)
                    {
                        g_settings.g_CaptionColor = g_settings.CaptionInactiveTextColor;
                        DwmSetWindowAttribute(cwp->hwnd, DWMWA_TEXT_COLOR, &g_settings.g_CaptionColor, sizeof(COLORREF));
                    }
                    if((g_settings.BorderFlag && !g_settings.BorderRainbowFlag))
                    {
                        g_settings.g_BorderColor = g_settings.BorderInactiveColor;
                        DwmSetWindowAttribute(cwp->hwnd, DWMWA_BORDER_COLOR, &g_settings.BorderInactiveColor, sizeof(COLORREF));
                    }
                }
            }
            break;
        }
        case WM_ENTERSIZEMOVE:
        {
            // Pause rainbow effects while the window is being moved/resized.
            if (g_settings.BorderRainbowFlag || g_settings.CaptionRainbowFlag || g_settings.TitlebarRainbowFlag)
                SendMessage(cwp->hwnd, g_msgRainbowTimer, RAINBOW_PAUSED, 0);
            break;
        }
        case WM_EXITSIZEMOVE:
        {
            // Resume rainbow effects after move/resize ends.
            if (g_settings.BorderRainbowFlag || g_settings.CaptionRainbowFlag || g_settings.TitlebarRainbowFlag)
                SendMessage(cwp->hwnd, g_msgRainbowTimer, RAINBOW_RESTART, 0);
            break;
        }
        case WM_DESTROY:
        {       
           RainbowData* pRainbowWndData = reinterpret_cast<RainbowData*>(RemovePropW(cwp->hwnd, g_RainbowPropStr.c_str()));
            if (!pRainbowWndData)
                break;

            if (KillTimer(NULL, pRainbowWndData->timerId))
            {
                std::lock_guard<std::mutex> guard(g_rainbowWindowsMutex);
                g_rainbowWindows.erase(cwp->hwnd);
                Wh_Log(L"Timer termination success for window: %p", cwp->hwnd);
                delete pRainbowWndData;
            }
            else
                Wh_Log(L"[ERROR] Timer termination failure for window: %p", cwp->hwnd);

            RemoveOrUnloadFlyoutSubclass(cwp->hwnd);
            break;
        }
        default:
        {
            if (cwp->message == g_msgRainbowTimer)
            {
                switch (cwp->wParam)
                {
                    case RAINBOW_LOAD:
                    {
                        HANDLE Data = GetPropW(cwp->hwnd, g_RainbowPropStr.c_str());
                        if (Data)
                            break;
                        
                        RainbowData* pRainbowWndData = new RainbowData();
                        DOUBLE now = TimerGetSeconds();
                        
                        std::mt19937 gen((UINT_PTR)cwp->hwnd);
                        std::uniform_real_distribution<> distr(0.0, 360.0);
                        pRainbowWndData->initialHue = distr(gen);
                        pRainbowWndData->baseTime = now;
                        pRainbowWndData->pausedTotal = 0.0;
                        pRainbowWndData->pausedStart = 0.0;
                        pRainbowWndData->isPaused = FALSE;                        

                        pRainbowWndData->timerId = SetTimer(NULL, NULL, 32, MyRainbowTimerProc);
                        if (pRainbowWndData->timerId)
                        {
                            SetPropW(cwp->hwnd, g_RainbowPropStr.c_str(), reinterpret_cast<HANDLE>(pRainbowWndData));
                            {
                                std::lock_guard<std::mutex> guard(g_rainbowWindowsMutex);
                                g_rainbowWindows.insert(cwp->hwnd);
                            }
                            Wh_Log(L"Timer set success for window: %p", cwp->hwnd);
                        }
                        else {
                            Wh_Log(L"[ERROR] Timer set failure for window: %p", cwp->hwnd);
                            delete pRainbowWndData;
                        }
                        
                        break;
                    }
                    case RAINBOW_RESTART:
                    {
                        RainbowData* pRainbowWndData = reinterpret_cast<RainbowData*>(GetPropW(cwp->hwnd, g_RainbowPropStr.c_str()));
                        if (!pRainbowWndData)
                            break;

                        DOUBLE now = TimerGetSeconds();
                        if (pRainbowWndData->isPaused) {
                            pRainbowWndData->pausedTotal += now - pRainbowWndData->pausedStart;
                            pRainbowWndData->isPaused = FALSE;
                        }
                        
                        if (!pRainbowWndData->timerId) {
                            pRainbowWndData->timerId = SetTimer(NULL, NULL, 32, MyRainbowTimerProc);
                            if (!pRainbowWndData->timerId)
                                Wh_Log(L"[ERROR] Timer restart failure for window: %p", cwp->hwnd);
                        }

                        break;
                    }
                    case RAINBOW_PAUSED:
                    {
                        RainbowData* pRainbowWndData = reinterpret_cast<RainbowData*>(GetPropW(cwp->hwnd, g_RainbowPropStr.c_str()));
                        if (!pRainbowWndData)
                            break;

                        DOUBLE now = TimerGetSeconds();
                        if (!pRainbowWndData->isPaused) {
                            pRainbowWndData->pausedStart = now;
                            pRainbowWndData->isPaused = TRUE;
                        }
                        
                        if (pRainbowWndData->timerId) {
                            if (KillTimer(NULL, (UINT_PTR)pRainbowWndData->timerId))
                                pRainbowWndData->timerId = 0;
                            else
                                Wh_Log(L"[ERROR] Timer pause failure for window: %p", cwp->hwnd);
                        }
                            
                        break;
                    }
                    case RAINBOW_UNLOAD:
                    {
                        RainbowData* pRainbowWndData = reinterpret_cast<RainbowData*>(RemovePropW(cwp->hwnd, g_RainbowPropStr.c_str()));
                        if (!pRainbowWndData)
                            break;
                                                    
                        if (KillTimer(NULL, (UINT_PTR)pRainbowWndData->timerId))
                            Wh_Log(L"Timer unload success for window: %p", cwp->hwnd);
                        else
                            Wh_Log(L"[ERROR] Timer unload failure for window: %p", cwp->hwnd);

                        delete pRainbowWndData;
    
                        break;
                    }
                }
            }
            break;
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

VOID AddWindowHookPerWindowThread(HWND hWnd)
{   
    if(g_settings.BorderFlag || g_settings.CaptionTextFlag || g_settings.TitlebarFlag || g_settings.BgType != g_settings.Default)
    {
        {
            std::lock_guard<std::mutex> guard(g_allCallWndProcHooksMutex);   
            DWORD dwThreadId = GetWindowThreadProcessId(hWnd, NULL);
            HHOOK callWndProcHook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc, nullptr, dwThreadId);
            if (callWndProcHook) {
                g_callWndProcHook = callWndProcHook;
                g_allCallWndProcHooks.insert(callWndProcHook);
                Wh_Log(L"SetWindowsHookEx succeeded for thread %u", dwThreadId);
            }
            else
                Wh_Log(L"[ERROR] SetWindowsHookEx failed for thread %u", dwThreadId);
        }
        if (g_settings.BorderRainbowFlag || g_settings.CaptionRainbowFlag || g_settings.TitlebarRainbowFlag)
            SendMessage(hWnd, g_msgRainbowTimer, RAINBOW_LOAD, 0);
    }
}

VOID RemoveOrUnloadWindowsHooks(BOOL Clean)
{
    if (Clean) 
    {
        for (HHOOK hook : g_allCallWndProcHooks)
            UnhookWindowsHookEx(hook);
        g_allCallWndProcHooks.clear();
    }
    else if (g_callWndProcHook)
    {
        auto it = g_allCallWndProcHooks.find(g_callWndProcHook);
        if (it != g_allCallWndProcHooks.end()) {
            std::lock_guard<std::mutex> guard(g_allCallWndProcHooksMutex);
            UnhookWindowsHookEx(g_callWndProcHook);
            g_allCallWndProcHooks.erase(it);
        }
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        RemoveOrUnloadWindowsHooks(FALSE);
        break;

    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}

BOOL WINAPI HookedTrackPopupMenuEx(HMENU hMenu, UINT uFlags, INT x, INT y, HWND hWnd, LPTPMPARAMS lptpm)
{
    // System tray popup menus
    AddWindowHookPerWindowThread(hWnd);
    return TrackPopupMenuEx_orig(hMenu, uFlags, x, y, hWnd, lptpm);
}

static LRESULT WINAPI HookedDefWindowProcW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{  
    if (Msg == WM_DWMCOLORIZATIONCOLORCHANGED && g_settings.AccentColorize)
    {
        COLORREF oldAccentClr = g_settings.AccentColor;
        GetAccentColor(g_settings.AccentColor);
        if (oldAccentClr != g_settings.AccentColor) {
            ColorizeSysColors();
            g_AccentPalette.LoadAccentPalette();
            g_cache.ClearCache();
        }
    }
    return DefWindowProc_orig(hWnd, Msg, wParam, lParam);
}

VOID HandleEffects(HWND hWnd)
{
    if (g_settings.ExtendFrame && (!IsWindowClass(hWnd, TOOLTIPS_CLASS) && !IsWindowClass(hWnd, MENUPOPUP_CLASS) 
        && !IsWindowClass(hWnd, L"DropDown") && !IsWindowClass(hWnd, L"ViewControlClass")))
            ApplyFrameExtension(hWnd);

    if (g_settings.ImmersiveDarkmode)
        DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &ENABLE, sizeof(UINT));

    if(g_settings.BgType == g_settings.AccentBlurBehind)
        EnableBlurBehind(hWnd);
    else if (g_settings.BgType > g_settings.AccentBlurBehind)
    {
        if (IsWindowClass(hWnd, TOOLTIPS_CLASS) || IsWindowClass(hWnd, MENUPOPUP_CLASS) || IsWindowClass(hWnd, L"DropDown")){
                DwmMakeWindowTransparent(hWnd);
                TriggerWindowNCRendering(hWnd);
        }
        SetSystemBackdrop(hWnd, g_settings.BgType);
    }

    if(g_settings.BorderFlag && !g_settings.BorderRainbowFlag)
        EnableColoredBorder(hWnd);
    
    SetCornerType(hWnd, g_settings.CornerPref);
}

VOID NewWindowShown(HWND hWnd) 
{
    if(!IsWindowEligible(hWnd))
        return;        
    else
        Wh_Log(L"Eligible window: %p", hWnd);

    HandleEffects(hWnd);
    AddWindowHookPerWindowThread(hWnd);
}

VOID DwmExpandFrameIntoClientAreaHook(){
    WindhawkUtils::SetFunctionHook(DwmExtendFrameIntoClientArea, HookedDwmExtendFrameIntoClientArea, &DwmExtendFrameIntoClientArea_orig);
}

VOID DwmSetWindowAttributeHook(){
    WindhawkUtils::SetFunctionHook(DwmSetWindowAttribute, HookedDwmSetWindowAttribute, &DwmSetWindowAttribute_orig); 
}

VOID CustomRendering()
{
    InitDirect2D();
    #ifdef _WIN64
        CplDuiHook();
    #endif
    WindhawkUtils::SetFunctionHook(DefWindowProc, HookedDefWindowProcW, &DefWindowProc_orig);
    WindhawkUtils::SetFunctionHook(GetThemeBitmap, HookedGetThemeBitmap, &GetThemeBitmap_orig);
    WindhawkUtils::SetFunctionHook(GetThemeColor, HookedGetColorTheme, &GetThemeColor_orig);   
    WindhawkUtils::SetFunctionHook(DrawThemeBackground, HookedDrawThemeBackground, &DrawThemeBackground_orig);
    WindhawkUtils::SetFunctionHook(DrawThemeBackgroundEx, HookedDrawThemeBackgroundEx, &DrawThemeBackgroundEx_orig);
    if (!g_settings.SetSystemColors) {
        WindhawkUtils::SetFunctionHook(GetSysColor, HookedGetSysColor, &GetSysColor_orig);
        WindhawkUtils::SetFunctionHook(GetSysColorBrush, HookedGetSysColorBrush, &GetSysColorBrush_orig);
    }
    WindhawkUtils::SetFunctionHook(GetThemeMargins, HookedGetThemeMargins, &GetThemeMargins_orig);
    WindhawkUtils::SetFunctionHook(DrawTextW, HookedDrawTextW, &DrawTextW_orig);
    WindhawkUtils::SetFunctionHook(DrawTextExW, HookedDrawTextExW, &DrawTextExW_orig);
    WindhawkUtils::SetFunctionHook(ExtTextOutW, HookedExtTextOutW, &ExtTextOutW_orig);
    WindhawkUtils::SetFunctionHook(DrawThemeText, HookedDrawThemeText, &DrawThemeText_orig);
    WindhawkUtils::SetFunctionHook(DrawThemeTextEx, HookedDrawThemeTextEx, &DrawThemeTextEx_orig);
}

VOID RestoreWindowCustomizations(HWND hWnd)
{
    if(!IsWindowEligible(hWnd))
        return;
    
    // Manually restore frame extension
    if(!(IsWindowClass(hWnd,  L"TaskManagerWindow")))
    {
        MARGINS margins = { 0, 0, 0, 0 };
        DwmExtendFrameIntoClientArea(hWnd, &margins);
    }

    g_settings.BorderActiveColor = DWMWA_COLOR_DEFAULT;
    DwmSetWindowAttribute(hWnd, DWMWA_BORDER_COLOR , &g_settings.BorderActiveColor, sizeof(COLORREF));
    
    g_settings.TitlebarActiveColor = DWMWA_COLOR_DEFAULT;
    DwmSetWindowAttribute(hWnd, DWMWA_CAPTION_COLOR , &g_settings.TitlebarActiveColor, sizeof(COLORREF));

    g_settings.CaptionActiveTextColor = DWMWA_COLOR_DEFAULT;
    DwmSetWindowAttribute(hWnd, DWMWA_TEXT_COLOR , &g_settings.CaptionActiveTextColor, sizeof(COLORREF));

    if (g_settings.CornerPref != g_settings.DefaultRounded)
        DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &DEFAULT , sizeof(UINT));

    // Disabling AccentBlurBehind temp workaround
    g_bb.fEnable = FALSE;
    g_bb.hRgnBlur = NULL;
    DwmEnableBlurBehindWindow(hWnd, &g_bb);

    g_accent.AccentState = 0;

    g_attrib.Attrib = WCA_ACCENT_POLICY;
    g_attrib.pvData = &g_accent;
    g_attrib.cbData = sizeof(g_accent);

    typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
    auto SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");
    if (SetWindowCompositionAttribute) 
        SetWindowCompositionAttribute(hWnd, &g_attrib);
    
    DwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE , &AUTO, sizeof(UINT));
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

DOUBLE RainbowSpeedInput(INT input)
{
    if (input < 1 || input > 100)
        return 1.0f;

    return (DOUBLE)input * 3.6;    
}

VOID GetProcStrFromPath(std::wstring& path) {
    size_t pos = path.find_last_of(L"\\/");
    if (pos != std::wstring::npos && pos + 1 < path.length()) {
        path = path.substr(pos + 1);
    }

    if (!path.empty()) 
    {
        LCMapStringEx(
            LOCALE_NAME_USER_DEFAULT, 
            LCMAP_UPPERCASE,
            path.c_str(),
            path.length(),
            &path[0],
            path.length(),
            nullptr, nullptr, 0);
    }
}

VOID GetCurrProcInfo(std::wstring& outName, DWORD& outPID) {
    WCHAR modulePath[MAX_PATH];
    GetModuleFileNameW(NULL, modulePath, MAX_PATH);

    outName = modulePath;
    GetProcStrFromPath(outName);

    outPID = GetCurrentProcessId();
}

VOID ApplyHooks()
{
    if(g_settings.FillBg)
        CustomRendering();
    if (g_settings.SetSystemColors)
        ColorizeSysColors();
    if(g_settings.ExtendFrame)
        DwmExpandFrameIntoClientAreaHook();
    if (g_settings.TitlebarFlag || g_settings.BorderFlag || g_settings.CaptionTextFlag || g_settings.BgType != g_settings.Default)
        DwmSetWindowAttributeHook();
    if (g_settings.BgType != g_settings.Default) {
        // Intercept and subclass system tray popupmenus
        WindhawkUtils::SetFunctionHook(TrackPopupMenuEx, HookedTrackPopupMenuEx, &TrackPopupMenuEx_orig);
    }
}

VOID LoadWindowProcessRules()
{
    // Process Rules
    DWORD currProcID = NULL;
    std::wstring currproc = {};
    GetCurrProcInfo(currproc, currProcID);

    for (INT i = 0;; i++) 
    {
        auto program = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"RuledPrograms[%d].target", i));
        
        BOOL hasProgram = *program;
        if (hasProgram) 
        {
            std::wstring ruledproc = program.get();
            GetProcStrFromPath(ruledproc);
            
            if(currproc == ruledproc)
            {
                g_settings.AccentColorize = Wh_GetIntSetting(L"RuledPrograms[%d].RenderingMod.AccentColorControls", i);
                if (g_settings.AccentColorize)
                    g_settings.AccentColorize = GetAccentColor(g_settings.AccentColor);
                
                g_settings.FillBg = Wh_GetIntSetting(L"RuledPrograms[%d].RenderingMod.ThemeBackground", i);
                
                auto strStyle = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"RuledPrograms[%d].type", i));
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

                GetColorSetting(WindhawkUtils::StringSetting(Wh_GetStringSetting(L"RuledPrograms[%d].AccentBlurBehind", i)), g_settings.AccentBlurBehindClr);

                g_settings.ImmersiveDarkmode = Wh_GetIntSetting(L"RuledPrograms[%d].ImmersiveDarkTitle", i);

                g_settings.ExtendFrame = Wh_GetIntSetting(L"RuledPrograms[%d].ExtendFrame", i);

                auto strCornerType = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"RuledPrograms[%d].CornerOption", i));
                if (0 == wcscmp(strCornerType, L"notrounded"))
                    g_settings.CornerPref = g_settings.NotRounded;
                else if (0 == wcscmp(strCornerType, L"smallround"))
                    g_settings.CornerPref = g_settings.SmallRounded;
                else
                    g_settings.CornerPref = g_settings.DefaultRounded;
                
                g_settings.RainbowSpeed = RainbowSpeedInput(Wh_GetIntSetting(L"RuledPrograms[%d].RainbowSpeed", i));

                g_settings.TitlebarFlag = Wh_GetIntSetting(L"RuledPrograms[%d].TitlebarColor.ColorTitlebar", i);
                g_settings.TitlebarRainbowFlag = Wh_GetIntSetting(L"RuledPrograms[%d].TitlebarColor.RainbowTitlebar", i) && g_settings.TitlebarFlag;
                if(g_settings.TitlebarFlag)
                {
                    auto strTitlebarStyle = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"RuledPrograms[%d].TitlebarColor.Titlerbarstyles_active", i));
                    g_settings.TitlebarFlag = GetColorSetting(strTitlebarStyle, g_settings.TitlebarActiveColor);
                    strTitlebarStyle = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"RuledPrograms[%d].TitlebarColor.Titlerbarstyles_inactive", i));
                    g_settings.TitlebarFlag = GetColorSetting(strTitlebarStyle, g_settings.TitlebarInactiveColor) || g_settings.TitlebarFlag;

                    if ((g_settings.TitlebarActiveColor == DWMWA_COLOR_DEFAULT || g_settings.TitlebarActiveColor == DWMWA_COLOR_NONE) ||
                        (g_settings.TitlebarInactiveColor == DWMWA_COLOR_DEFAULT || g_settings.TitlebarInactiveColor == DWMWA_COLOR_NONE))
                            g_settings.TitlebarFlag = FALSE;
                }

                g_settings.CaptionTextFlag = Wh_GetIntSetting(L"RuledPrograms[%d].TitlebarTextColor.ColorTitlebarText", i);
                g_settings.CaptionRainbowFlag = Wh_GetIntSetting(L"RuledPrograms[%d].TitlebarTextColor.RainbowTextColor", i) && g_settings.CaptionTextFlag;
                if(g_settings.CaptionTextFlag)
                {
                    auto strTitlebarTextColorStyle = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"RuledPrograms[%d].TitlebarTextColor.titlerbarcolorstyles_active", i));
                    g_settings.CaptionTextFlag = GetColorSetting(strTitlebarTextColorStyle, g_settings.CaptionActiveTextColor);
                    strTitlebarTextColorStyle = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"RuledPrograms[%d].TitlebarTextColor.titlerbarcolorstyles_inactive", i));
                    g_settings.CaptionTextFlag = GetColorSetting(strTitlebarTextColorStyle, g_settings.CaptionInactiveTextColor) || g_settings.CaptionTextFlag;
                }

                g_settings.BorderFlag = Wh_GetIntSetting(L"RuledPrograms[%d].BorderColor.ColorBorder", i);

                g_settings.BorderFlag = Wh_GetIntSetting(L"RuledPrograms[%d].BorderColor.ColorBorder", i);
                g_settings.BorderRainbowFlag = Wh_GetIntSetting(L"RuledPrograms[%d].BorderColor.RainbowBorder", i) && g_settings.BorderFlag;
                if(g_settings.BorderFlag)
                {
                    auto strBorderStyle = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"RuledPrograms[%d].BorderColor.borderstyles_active", i));
                    g_settings.BorderFlag = GetColorSetting(strBorderStyle, g_settings.BorderActiveColor);
                    strBorderStyle = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"RuledPrograms[%d].BorderColor.borderstyles_inactive", i));
                    g_settings.BorderFlag = GetColorSetting(strBorderStyle, g_settings.BorderInactiveColor) || g_settings.BorderFlag;
                }
            }
        }

        if (!hasProgram) {
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
    
    g_settings.SetSystemColors = Wh_GetIntSetting(L"RenderingMod.Syscolors");
    
    auto strStyle = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"type"));
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
    
    GetColorSetting(WindhawkUtils::StringSetting(Wh_GetStringSetting(L"AccentBlurBehind")), g_settings.AccentBlurBehindClr);

    g_settings.ImmersiveDarkmode = Wh_GetIntSetting(L"ImmersiveDarkTitle");
    
    g_settings.ExtendFrame = Wh_GetIntSetting(L"ExtendFrame");
    
    auto strCornerType = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"CornerOption"));
    if (0 == wcscmp(strCornerType, L"notrounded"))
        g_settings.CornerPref = g_settings.NotRounded;
    else if (0 == wcscmp(strCornerType, L"smallround"))
        g_settings.CornerPref = g_settings.SmallRounded;
    else
        g_settings.CornerPref = g_settings.DefaultRounded;

    g_settings.RainbowSpeed = RainbowSpeedInput(Wh_GetIntSetting(L"RainbowSpeed"));

    g_settings.TitlebarFlag = Wh_GetIntSetting(L"TitlebarColor.ColorTitlebar");
    g_settings.TitlebarRainbowFlag = Wh_GetIntSetting(L"TitlebarColor.RainbowTitlebar") && g_settings.TitlebarFlag;
    if(g_settings.TitlebarFlag)
    {
        auto strTitlebarStyle = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"TitlebarColor.titlerbarstyles_active"));
        g_settings.TitlebarFlag = GetColorSetting(strTitlebarStyle, g_settings.TitlebarActiveColor);
        strTitlebarStyle = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"TitlebarColor.titlerbarstyles_inactive"));
        g_settings.TitlebarFlag = GetColorSetting(strTitlebarStyle, g_settings.TitlebarInactiveColor) || g_settings.TitlebarFlag;

        if ((g_settings.TitlebarActiveColor == DWMWA_COLOR_DEFAULT || g_settings.TitlebarActiveColor == DWMWA_COLOR_NONE) ||
            (g_settings.TitlebarInactiveColor == DWMWA_COLOR_DEFAULT || g_settings.TitlebarInactiveColor == DWMWA_COLOR_NONE))
                g_settings.TitlebarFlag = FALSE;
    }

    g_settings.CaptionTextFlag = Wh_GetIntSetting(L"TitlebarTextColor.ColorTitlebarText");
    g_settings.CaptionRainbowFlag = Wh_GetIntSetting(L"TitlebarTextColor.RainbowTextColor") && g_settings.CaptionTextFlag;
    if(g_settings.CaptionTextFlag)
    {
        auto strTitlebarTextColorStyle = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"TitlebarTextColor.titlerbarcolorstyles_active"));
        g_settings.CaptionTextFlag = GetColorSetting(strTitlebarTextColorStyle, g_settings.CaptionActiveTextColor);
        strTitlebarTextColorStyle = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"TitlebarTextColor.titlerbarcolorstyles_inactive"));
        g_settings.CaptionTextFlag = GetColorSetting(strTitlebarTextColorStyle, g_settings.CaptionInactiveTextColor) || g_settings.CaptionTextFlag;
    }

    g_settings.BorderFlag = Wh_GetIntSetting(L"BorderColor.ColorBorder");
    g_settings.BorderRainbowFlag = Wh_GetIntSetting(L"BorderColor.RainbowBorder") && g_settings.BorderFlag;
    if(g_settings.BorderFlag)
    {
        auto strBorderStyle = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"BorderColor.borderstyles_active"));
        g_settings.BorderFlag = GetColorSetting(strBorderStyle, g_settings.BorderActiveColor);
        strBorderStyle = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"BorderColor.borderstyles_inactive"));
        g_settings.BorderFlag = GetColorSetting(strBorderStyle, g_settings.BorderInactiveColor) || g_settings.BorderFlag;
    }

    LoadWindowProcessRules();
    
    ApplyHooks();   
}

BOOL Wh_ModInit(VOID) 
{
    TimerInitialize();

    LoadSettings();

    HMODULE hModule = GetModuleHandle(L"win32u.dll");
    if (!hModule) 
        return FALSE;
        
    NtUserCreateWindowEx_t pNtUserCreateWindowEx =
        (NtUserCreateWindowEx_t)GetProcAddress(hModule, "NtUserCreateWindowEx");
    if (!pNtUserCreateWindowEx)
        return FALSE;
    
    WindhawkUtils::SetFunctionHook(pNtUserCreateWindowEx,
                       HookedNtUserCreateWindowEx,
                       &NtUserCreateWindowEx_Original);
    
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
    g_settings.Unload = TRUE;
    if (g_settings.FillBg)
        g_d2dFactory->Release();
    if (g_settings.SetSystemColors)
        RevertSysColors();
    for (HBRUSH& brush : g_cachedSysColorBrushes)
        DeleteObject(brush);

    if (!g_rainbowWindows.empty())
    {
        std::unordered_set<HWND> RainbowWindows;
        {
            std::lock_guard<std::mutex> guard(g_rainbowWindowsMutex);

            RainbowWindows = std::move(g_rainbowWindows);
            g_rainbowWindows.clear();
        }

        if (g_settings.BorderRainbowFlag || g_settings.CaptionRainbowFlag || g_settings.TitlebarRainbowFlag)
        {
            for (auto hWnd : RainbowWindows)
                SendMessageW(hWnd, g_msgRainbowTimer, RAINBOW_UNLOAD, 0);
        }

        RainbowWindows.clear();
    }
    
    RemoveOrUnloadWindowsHooks(TRUE);
    RemoveOrUnloadFlyoutSubclass(nullptr, TRUE);    
    ApplyForExistingWindows();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) 
{
    Wh_Log(L"SettingsChanged");
    *bReload = TRUE;
    return TRUE;
}

