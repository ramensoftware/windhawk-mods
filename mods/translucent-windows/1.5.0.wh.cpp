// ==WindhawkMod==
// @id              translucent-windows
// @name            Translucent Windows
// @description     Enables native translucent effects in Windows 11
// @version         1.5.0
// @author          Undisputed00x
// @github          https://github.com/Undisputed00x
// @include         *
// @compilerOptions -ldwmapi -luxtheme -lcomctl32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

### Blur (AccentBlurBehind)
![AccentBlurBehind](https://i.imgur.com/S8OGRvc.png)
### Acrylic (SystemBackdrop)
![Acrylic SystemBackdrop](https://i.imgur.com/66cAiEC.png)
### Mica (SystemBackdrop)
![Mica](https://i.imgur.com/7BGDhJa.png)
### MicaAlt (SystemBackdrop)
![MicaTabbed](https://i.imgur.com/EeviFqO.png)

# FAQ

* Prerequisited windows settings to enable the background effects
    - Transparency effects enabled
    - Energy saver disabled
#
* The background effects do not affect modern windows (UWP/WinUI)

* It is highly recommended to use the mod with black/dark window themes like the Rectify11 "Dark theme with Mica".

* Extending effects to the entire window can result in text being unreadable or even invisible in some cases. 
Light mode theme, HDR enabled, black color or white background behind the window can cause this. 
This is because most GDI rendering operations do not preserve alpha values.

* Blur effect may show a bleeding effect at the edges of a window when 
maximized or snapped to the edge of the screen, this is caused by default.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ThemeBackground: FALSE
  $name: Optimize windows theme
  $description: >-
    Fills with black color windows theme parts in order to nullify the alpha of those elements to render a clear translucent effect.
- TextAlphaBlend: FALSE
  $name: Text alpha blending
  $description: >-
    Alpha blends Windows GDI text rendering. It may not affect all rendered text, use with caution as it may cause problems with some applications (ExplorerBlurMica implementation)
- type: none
  $name: Effects
  $description: >-
     Windows 11 version >= 22621.xxx (22H2) is required.
  $options:
  - none: Default
  - acrylicblur: Blur (AccentBlurBehind)
  - acrylicsystem: Acrylic (SystemBackdrop)
  - mica: Mica (SystemBackdrop)
  - mica_tabbed: MicaAlt (SystemBackdrop)
- AccentBlurBehind: "0F000000"
  $name: AccentBlurBehind color blend
  $description: >-
    Blending color with blur background.
     Color in hexadecimal ARGB format e.g. 0F000000
- ImmersiveDarkTitle: TRUE
  $name: Immersive darkmode titlebar
  $description: >-
    Enables the flag for immersive darkmode titlebars, affects the tintcolor of SystemBackdrop effects and caption buttons color state
- ExtendFrame: FALSE
  $name: Extend effects into entire window
  $description: >-
    Extends the effects into the entire window background using DwmExtendFrameIntoClientArea.
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
    - MenuBorderColor: FALSE
      $name: Extend colored borders to classic context menus
      $description: Enable this option to get colored borders on windows classic context menus.
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
      - ThemeBackground: FALSE
        $name: Optimize windows theme
        $description: >-
         Fills with black color windows theme parts in order to nullify the alpha of those elements to render a clear translucent effect.
      - TextAlphaBlend: FALSE
        $name: Text alpha blending
        $description: >-
         Alpha blends Windows GDI text rendering. It may not affect all rendered text, use with caution as it may cause problems with some applications (ExplorerBlurMica implementation)
      - type: none
        $name: Effects
        $description: >-
         Windows 11 version >= 22621.xxx (22H2) is required.
        $options:
          - none: Default
          - acrylicblur: Blur (AccentBlurBehind)
          - acrylicsystem: Acrylic (SystemBackdrop)
          - mica: Mica (SystemBackdrop)
          - mica_tabbed: MicaAlt (SystemBackdrop)
      - AccentBlurBehind: "0F000000"
        $name: AccentBlurBehind color blend
        $description: >-
          Blending color with blur background.
           Color in hexadecimal ARGB format e.g. 0F000000
      - ImmersiveDarkTitle: FALSE
        $name: Immersive darkmode titlebar
        $description: >-
            Enables the flag for immersive darkmode titlebars, affects the tintcolor of SystemBackdrop effects and caption buttons color state
      - ExtendFrame: FALSE
        $name: Extend effects into entire window
        $description: >-
          Extends the effects into the entire window background using DwmExtendFrameIntoClientArea.
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
#include <windows.h>
#include <functional>
#include <cmath>
#include <random>
#include <string>
#include <mutex>
#include <unordered_set>
#include <commctrl.h>
#include <wingdi.h>


static UINT ENABLE = 1;
static constexpr UINT AUTO = 0; // DWMSBT_AUTO
static constexpr UINT NONE = 1; // DWMSBT_NONE
static constexpr UINT MAINWINDOW = 2; // DWMSBT_MAINWINDOW
static constexpr UINT TRANSIENTWINDOW = 3; // DWMSBT_TRANSIENTWINDOW
static constexpr UINT TABBEDWINDOW = 4; // DWMSBT_TABBEDWINDOW

static constexpr UINT DEFAULT = 0; // DWMWCP_DEFAULT
static constexpr UINT DONTROUND = 1; // DWMWCP_DONOTROUND
static constexpr UINT SMALLROUND = 3; // DWMWCP_ROUNDSMALL

UINT g_msgRainbowTimer = RegisterWindowMessage(L"Rainbow_effect");

enum {
    RAINBOW_LOAD,
    RAINBOW_UNLOAD
};

std::wstring g_RainbowPropStr = L"Windhawk_TranslucentMod_Rainbow";
std::mutex g_rainbowWindowsMutex;
std::unordered_set<HWND> g_rainbowWindows;

thread_local BOOL g_DrawThemeTextExEntry;

typedef HRESULT(WINAPI* pDrawTextWithGlow)(HDC, LPCWSTR, UINT, const RECT*, DWORD, COLORREF, COLORREF, UINT, UINT, BOOL, DTT_CALLBACK_PROC, LPARAM);
static auto DrawTextWithGlow = (pDrawTextWithGlow)GetProcAddress(GetModuleHandle(L"uxtheme.dll"), MAKEINTRESOURCEA(126));

thread_local HHOOK g_callWndProcHook;
std::mutex g_allCallWndProcHooksMutex;
std::unordered_set<HHOOK> g_allCallWndProcHooks;

using PUNICODE_STRING = PVOID;

struct Settings{
    BOOL FillBg = FALSE;
    BOOL TextAlphaBlend = FALSE;
    COLORREF AccentBlurBehindClr = 0x00000000;
    BOOL ImmersiveDarkmode = TRUE;
    BOOL ExtendFrame = FALSE;
    BOOL Unload = FALSE;
    BOOL TitlebarFlag = FALSE;
    BOOL CaptionTextFlag = FALSE;
    BOOL BorderFlag = FALSE;
    BOOL MenuBorderFlag = FALSE;
    COLORREF TitlebarActiveColor = DWMWA_COLOR_DEFAULT;
    COLORREF CaptionActiveTextColor = DWMWA_COLOR_DEFAULT;
    COLORREF BorderActiveColor = DWMWA_COLOR_DEFAULT;

    COLORREF g_TitlebarColor = DWMWA_COLOR_DEFAULT;
    COLORREF g_CaptionColor = DWMWA_COLOR_DEFAULT;
    COLORREF g_BorderColor = DWMWA_COLOR_DEFAULT;

    COLORREF TitlebarInactiveColor = DWMWA_COLOR_DEFAULT;
    COLORREF CaptionInactiveTextColor = DWMWA_COLOR_DEFAULT;
    COLORREF BorderInactiveColor = DWMWA_COLOR_DEFAULT;
    
    float RainbowSpeed = 1.0f;

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
        DefaultRounded,
        NotRounded,
        SmallRounded = 3
    } CornerPref = DefaultRounded;

} g_settings;

/*https://gist.github.com/xv/43bd4c944202a593ac8ec6daa299b471*/
struct ACCENT_POLICY 
{
    int AccentState;
    int AccentFlags;
    int GradientColor;
    int AnimationId;
};

struct WINCOMPATTRDATA 
{
    DWORD Attrib;
    PVOID pvData;
    SIZE_T cbData;
};

enum WINDOWCOMPOSITIONATTRIB 
{
    WCA_ACCENT_POLICY = 19
};

ACCENT_POLICY accent = {};
WINCOMPATTRDATA attrib = {};
DWM_BLURBEHIND bb = { 0 };

// Credits to @m417z
double g_recipCyclesPerSecond;

void TimerInitialize() {
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    double cyclesPerSecond = static_cast<double>(freq.QuadPart);
    g_recipCyclesPerSecond = 1.0 / cyclesPerSecond;
}

double TimerGetCycles() {
    LARGE_INTEGER T1;
    QueryPerformanceCounter(&T1);
    return static_cast<double>(T1.QuadPart);
}

double TimerGetSeconds() {
    return TimerGetCycles() * g_recipCyclesPerSecond;
}

using NtUserCreateWindowEx_t = HWND(WINAPI*)(DWORD, PUNICODE_STRING, LPCWSTR, PUNICODE_STRING, DWORD, LONG, LONG, LONG, LONG, HWND, HMENU, HINSTANCE, LPVOID, DWORD, DWORD, DWORD, VOID*);
NtUserCreateWindowEx_t NtUserCreateWindowEx_Original;

static decltype(&DwmExtendFrameIntoClientArea) DwmExtendFrameIntoClientArea_orig = nullptr;
static decltype(&DwmSetWindowAttribute) DwmSetWindowAttribute_orig = nullptr;

static decltype(&DrawTextW) DrawTextW_orig = nullptr;
static decltype(&DrawTextExW) DrawTextExW_orig = nullptr;
static decltype(&ExtTextOutW) ExtTextOutW_orig = nullptr;
static decltype(&DrawThemeText) DrawThemeText_orig = nullptr;
static decltype(&DrawThemeTextEx) DrawThemeTextEx_orig = nullptr;

static decltype(&GetSysColor) GetSysColor_orig = nullptr;
static decltype(&GetThemeBitmap) GetThemeBitmap_orig = nullptr;
static decltype(&GetThemeColor) GetThemeColor_orig = nullptr;
static decltype(&DrawThemeBackground) DrawThemeBackground_orig = nullptr;
static decltype(&DrawThemeBackgroundEx) DrawThemeBackgroundEx_orig = nullptr;

void NewWindowShown(HWND); 

BOOL IsWindowClass(HWND hWnd, LPCWSTR ClassName)
{
    WCHAR ClassNameBuffer[256]; 
    GetClassNameW(hWnd, ClassNameBuffer, sizeof(ClassNameBuffer));
    if(!wcscmp(ClassName, ClassNameBuffer))
        return TRUE;
    return FALSE;
}

BOOL IsWindowEligible(HWND hWnd) 
{   
    LONG_PTR styleEx = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    
    // Fixes Snipping Tool rec
    if ((styleEx & WS_EX_NOACTIVATE) || (styleEx & WS_EX_TRANSPARENT))
        return FALSE;

    HWND hParentWnd = GetAncestor(hWnd, GA_PARENT);
    if (hParentWnd && hParentWnd != GetDesktopWindow())
        return FALSE;

    BOOL hasTitleBar = (style & WS_BORDER) && (style & WS_DLGFRAME);

    if ((styleEx & WS_EX_TOOLWINDOW) && !hasTitleBar)
        return FALSE;

    if ((style & WS_POPUP) && !hasTitleBar)
        return FALSE;

    // Don't block CEF apps
    if (!((IsWindowClass(hWnd, L"Chrome_WidgetWin_1") || IsWindowClass(hWnd, L"Chrome_WidgetWin_0")) || style & WS_POPUP)
        && !(style & WS_THICKFRAME || style & WS_MINIMIZEBOX || style & WS_MAXIMIZEBOX))
        return FALSE;
    
    return TRUE;
}


HRESULT WINAPI HookedDwmSetWindowAttribute(HWND hWnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute)
{
    if(dwAttribute == DWMWA_BORDER_COLOR && IsWindowClass(hWnd, L"#32768") && g_settings.MenuBorderFlag)
            return DwmSetWindowAttribute_orig(hWnd, DWMWA_BORDER_COLOR, &g_settings.BorderActiveColor, sizeof(COLORREF));
            
    if(!IsWindowEligible(hWnd))
        return DwmSetWindowAttribute_orig(hWnd, dwAttribute, pvAttribute, cbAttribute);
    
    if(!g_settings.TitlebarFlag)
    {
        if (dwAttribute == DWMWA_SYSTEMBACKDROP_TYPE || dwAttribute == DWMWA_USE_HOSTBACKDROPBRUSH)
        {
            if (g_settings.BgType == g_settings.AccentBlurBehind)
                return DwmSetWindowAttribute_orig(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &NONE, sizeof(UINT));
            if(g_settings.BgType == g_settings.AcrylicSystemBackdrop)
                return DwmSetWindowAttribute_orig(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &TRANSIENTWINDOW, sizeof(UINT));
            else if(g_settings.BgType == g_settings.MicaAlt)
                return DwmSetWindowAttribute_orig(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &TABBEDWINDOW, sizeof(UINT));
            else if(g_settings.BgType == g_settings.Mica)
                return DwmSetWindowAttribute_orig(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &MAINWINDOW, sizeof(UINT));
        }
    }
    else if (dwAttribute == DWMWA_CAPTION_COLOR || (dwAttribute == DWMWA_SYSTEMBACKDROP_TYPE 
        && (IsWindowClass(hWnd, L"CabinetWClass") || IsWindowClass(hWnd, L"TaskManagerWindow"))))
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
        // Override Win11 Taskmgr, explorer, aerowizard calls
        if(IsWindowClass(hWnd, L"CabinetWClass") || IsWindowClass(hWnd, L"NativeHWNDHost") || IsWindowClass(hWnd, L"TaskManagerWindow"))
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
    typedef HRESULT(WINAPI* pGetThemeClass)(HTHEME, LPCTSTR, int);
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

// https://github.com/Maplespe/ExplorerBlurMica/blob/79c0ef4d017e32890e107ff98113507f831608b6/ExplorerBlurMica/HookDef.cpp#L829
bool AlphaBuffer(HDC hdc, LPCRECT pRc, std::function<void(HDC)> fun)
{
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

    BP_PAINTPARAMS bpParam;
    bpParam.cbSize = sizeof(BP_PAINTPARAMS);
    bpParam.dwFlags = BPPF_ERASE;
    bpParam.prcExclude = nullptr;
    bpParam.pBlendFunction = &bf;
    
    HDC hDC = nullptr;
    HPAINTBUFFER pbuffer = BeginBufferedPaint(hdc, pRc, BPBF_TOPDOWNDIB, &bpParam, &hDC);
    
    if (pbuffer && hDC && fun)
    {
        // Set the original DC information
        SelectObject(hDC, GetCurrentObject(hdc, OBJ_FONT));
        SetBkMode(hDC, GetBkMode(hdc));
        SetBkColor(hDC, GetBkColor(hdc));
        SetTextAlign(hDC, GetTextAlign(hdc));
        SetTextCharacterExtra(hDC, GetTextCharacterExtra(hdc));
        //SetTextColor(hDC, GetTextColor(hdc));

        fun(hDC);

        EndBufferedPaint(pbuffer, TRUE);
        return true;
    }
    return false;
}

// Fix Alpha of DrawTextW
// https://github.com/Maplespe/ExplorerBlurMica/blob/79c0ef4d017e32890e107ff98113507f831608b6/ExplorerBlurMica/HookDef.cpp#L859
int WINAPI HookedDrawTextW(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) 
{
    if (format & DT_CALCRECT || g_DrawThemeTextExEntry)
        return DrawTextW_orig(hdc, lpchText, cchText, lprc, format);

    HRESULT hr = S_OK;
    auto fun = [&](HDC hDC) {
        hr = DrawTextWithGlow(hDC, lpchText, cchText, lprc, format,
            GetTextColor(hdc), 0, 0, 0, 0,
            [](HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT format, LPARAM lParam) WINAPI
            {
                return DrawTextW_orig(hdc, lpchText, cchText, lprc, format);
            },
            0);
    };

    if (!AlphaBuffer(hdc, lprc, fun))
        hr = DrawTextW_orig(hdc, lpchText, cchText, lprc, format);

    return hr;
}

// Fix Alpha of DrawTextExW
// https://github.com/Maplespe/ExplorerBlurMica/blob/79c0ef4d017e32890e107ff98113507f831608b6/ExplorerBlurMica/HookDef.cpp#L887
int WINAPI HookedDrawTextExW(HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT format, LPDRAWTEXTPARAMS lpdtp) 
{
    thread_local bool isCurThread = false;

    if (!lpdtp && !(format & DT_CALCRECT) && !isCurThread && g_DrawThemeTextExEntry) 
    {
        isCurThread = true;
        auto ret = HookedDrawTextW(hdc, lpchText, cchText, lprc, format);
        isCurThread = false;
        return ret;
    }
    
    return DrawTextExW_orig(hdc, lpchText, cchText, lprc, format, lpdtp);
}

// https://github.com/Maplespe/ExplorerBlurMica/blob/79c0ef4d017e32890e107ff98113507f831608b6/ExplorerBlurMica/HookDef.cpp#L904
BOOL WINAPI HookedExtTextOutW(HDC hdc, int x, int y, UINT option, const RECT* lprect, LPCWSTR lpString, UINT c, const INT* lpDx)
{
    std::wstring str;
    if (lpString) str = lpString;


    if (!(option & ETO_IGNORELANGUAGE) && !(option & ETO_GLYPH_INDEX) && 
        !str.empty() && g_DrawThemeTextExEntry)
    {
        RECT rect = { 0 };
        if (lprect)
            rect = *lprect;
        
        // DT_NOPREFIX prevents special char '&' from rendering an extra underline or invisible char.
        if (!(option & ETO_OPAQUE || option & ETO_CLIPPED))
            DrawTextW_orig(hdc, lpString, c, &rect, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_CALCRECT | DT_NOPREFIX);
        
        if (!lpDx) {
            HookedDrawTextW(hdc, lpString, c, &rect, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);
        }
        else
        {
            RECT rc = { x, y, x + (rect.right - rect.left), y + (rect.bottom - rect.top) };

            if (option & ETO_CLIPPED)
            {
                SaveDC(hdc);
                IntersectClipRect(hdc, rect.left, rect.top, rect.right, rect.bottom);
            }

            COLORREF crText = GetTextColor(hdc);
            HRESULT hr = S_OK;

            auto DrawTextCallback = [](HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT format, LPARAM lParam) WINAPI {
                return DrawTextW_orig(hdc, lpchText, cchText, lprc, format);};

            //Draw text in batches
            auto fun = [&](HDC hDC) 
            {
                std::wstring batchStr;
                bool batch = true;
                batchStr += lpString[0];

                int srcExtra = GetTextCharacterExtra(hdc);
                SetTextCharacterExtra(hDC, lpDx[0]);

                RECT batchRc = rc;
                for (UINT i = 0; i < c; i++)
                {
                    if (i != 0) 
                    {
                        if (lpDx[i] == lpDx[i - 1]) 
                        {
                            if (!batch)
                            {
                                batch = true;
                                SetTextCharacterExtra(hDC, lpDx[i]);
                            }
                            batchStr += lpString[i];
                        }
                        else
                        {
                            // Draw the previous batch first
                            hr = DrawTextWithGlow(hDC, batchStr.c_str(), (int)batchStr.length(), &batchRc, 
                                DT_LEFT | DT_TOP | DT_SINGLELINE, crText, 0, 0, 0, 0, DrawTextCallback, 0);
                            //hr = _DrawThemeTextEx_.Org(hTheme, hDC, 0, 0, batchStr.c_str(), batchStr.length(), DT_LEFT | DT_TOP | DT_SINGLELINE, &batchRc, &dtop);

                            batch = false;
                            batchStr = lpString[i];
                            SetTextCharacterExtra(hDC, lpDx[i]);
                            batchRc.left = rc.left;
                        }
                    }

                    if (i == c - 1)
                    {
                        hr = DrawTextWithGlow(hDC, batchStr.c_str(), (int)batchStr.length(), &batchRc, 
                            DT_LEFT | DT_TOP | DT_SINGLELINE, crText, 0, 0, 0, 0, DrawTextCallback, 0);
                        //hr = _DrawThemeTextEx_.Org(hTheme, hDC, 0, 0, batchStr.c_str(), batchStr.length(), DT_LEFT | DT_TOP | DT_SINGLELINE, &batchRc, &dtop);
                    }

                    rc.left += lpDx[i];
                }
                SetTextCharacterExtra(hDC, srcExtra);
            };

            if (!AlphaBuffer(hdc, &rc, fun))
                hr = ExtTextOutW_orig(hdc, x, y, option, lprect, lpString, c, lpDx);

            if (option & ETO_CLIPPED)
                RestoreDC(hdc, -1);
        }
        return TRUE;
    }
    
    return ExtTextOutW_orig(hdc, x, y, option, lprect, lpString, c, lpDx);
}

// Prevent recursive calls within the DrawText class API and perform Alpha repair
// https://github.com/Maplespe/ExplorerBlurMica/blob/79c0ef4d017e32890e107ff98113507f831608b6/ExplorerBlurMica/HookDef.cpp#L1085
HRESULT WINAPI HookedDrawThemeTextEx(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText,
        int cchText, DWORD dwTextFlags, LPCRECT pRect, const DTTOPTS* pOptions)
{
    if (pOptions && !(pOptions->dwFlags & DTT_CALCRECT) && !(pOptions->dwFlags & DTT_COMPOSITED))
    {
        HRESULT hr = S_OK;
        auto fun = [&](HDC hDC) {
            //SetCurrentThreadDrawing(true);
            g_DrawThemeTextExEntry = TRUE;

            COLORREF color = pOptions->crText;
            if (!(dwTextFlags & DTT_TEXTCOLOR))
                GetThemeColor(hTheme, iPartId, iStateId, TMT_TEXTCOLOR, &color);

            hr = DrawTextWithGlow(hDC, pszText, cchText, pRect, dwTextFlags,
                color, 0, 0, 0, 0, pOptions->pfnDrawTextCallback, pOptions->lParam);
            
            g_DrawThemeTextExEntry = FALSE;
            //SetCurrentThreadDrawing(false);
        };

        if (!AlphaBuffer(hdc, pRect, fun))
            return DrawThemeTextEx_orig(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, (LPRECT)pRect, pOptions);

        return hr;
    }
   
    return DrawThemeTextEx_orig(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, (LPRECT)pRect, pOptions);
}

// Convert to DrawThemeTextEx call
// https://github.com/Maplespe/ExplorerBlurMica/blob/79c0ef4d017e32890e107ff98113507f831608b6/ExplorerBlurMica/HookDef.cpp#L1072
HRESULT WINAPI HookedDrawThemeText(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCTSTR pszText,
    int cchText, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect) 
{
    DTTOPTS Options = { sizeof(DTTOPTS) };
    RECT Rect = *pRect;

    GetThemeColor(hTheme, iPartId, iStateId, TMT_TEXTCOLOR, &Options.crText);
    HRESULT ret = HookedDrawThemeTextEx(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, &Rect, &Options);

    return ret;
}

DWORD WINAPI HookedGetSysColor(int nIndex)
{
    if (nIndex == COLOR_HOTLIGHT)
        return RGB(0, 148, 251);
    
    return GetSysColor_orig(nIndex);
}

#ifdef _WIN64
#define STDCALL  __cdecl
#define SSTDCALL L"__cdecl"
#else
#define STDCALL  __stdcall
#define SSTDCALL L"__stdcall"
#endif

typedef unsigned __int64 QWORD;

// https://github.com/ramensoftware/windhawk-mods/blob/15e5d9838349e4b927ed8ac5433e9894ff6cda28/mods/uxtheme-hook.wh.cpp#L90
typedef VOID(STDCALL *Element_PaintBgT)(class Element*, HDC , class Value*, LPRECT, LPRECT, LPRECT, LPRECT);
Element_PaintBgT Element_PaintBg;
VOID STDCALL Element_PaintBgHook(class Element* This, HDC hdc, class Value* value, LPRECT pRect, LPRECT pClipRect, LPRECT pExcludeRect, LPRECT pTargetRect)
{
    //unsigned char byteValue = *(reinterpret_cast<unsigned char*>(value) + 8);
    if ((int)(*(DWORD *)value << 26) >> 26 != 9 )
    {
        auto v44 = *((QWORD *)value + 1);
        auto v45 = (v44+20)& 7;
        // 6-> selection
        // 3-> hovered stuff
        // 4-> cpanel top bar and side bar (white image)
        // 1-> some new cp page style (cp_hub_frame)
        if (v45==4)
        { 
            HWND wnd = WindowFromDC(hdc);
            HTHEME hTh = OpenThemeData(wnd, L"ControlPanel");
            COLORREF clrBg;
            GetThemeColor(hTh, 2, 0, TMT_FILLCOLOR, &clrBg);
            HBRUSH SolidBrush = CreateSolidBrush(clrBg);
            FillRect(hdc, pRect, SolidBrush);
            DeleteObject(SolidBrush);
            CloseThemeData(hTh);
        }
        else
        {
            Element_PaintBg(This, hdc, value, pRect, pClipRect, pExcludeRect, pTargetRect);
        }
    }
    else
    {
        Element_PaintBg(This, hdc, value, pRect, pClipRect, pExcludeRect, pTargetRect);
    }
}

void CplDuiHook()
{
    WindhawkUtils::SYMBOL_HOOK dui70dll_hooks[] =
    {
        {
            {L"public: void " SSTDCALL " DirectUI::Element::PaintBackground(struct HDC__ *,class DirectUI::Value *,struct tagRECT const &,struct tagRECT const &,struct tagRECT const &,struct tagRECT const &)"},
            &Element_PaintBg,
            Element_PaintBgHook,
            false
        },
    };

    HMODULE hDui = LoadLibraryW(L"dui70.dll");
    WindhawkUtils::HookSymbols(hDui, dui70dll_hooks, ARRAYSIZE(dui70dll_hooks));
}

constexpr int aSysElements[] = {
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
    COLOR_HOTLIGHT ,
    COLOR_GRADIENTACTIVECAPTION ,
    COLOR_GRADIENTINACTIVECAPTION ,
    COLOR_MENUHILIGHT ,
    COLOR_MENUBAR
};

HTHEME hTh = nullptr;

void SetCurrentTheme(LPCWSTR themeclass)
{
    if (hTh != nullptr)
        hTh = nullptr;

    hTh = OpenThemeData(NULL, themeclass); 
}

void RevertSysColors()
{
    SetCurrentTheme(L"globals");

    COLORREF aNewColors[ARRAYSIZE(aSysElements)];
    for (UINT i = 0; i < ARRAYSIZE(aSysElements); i++)
    {
        aNewColors[i] = GetThemeSysColor(hTh, i);
    }
    SetSysColors(ARRAYSIZE(aSysElements), aSysElements, aNewColors);

    CloseThemeData(hTh);
    hTh = nullptr;
}

void ColorizeSysColors()
{
    COLORREF aNewColors[ARRAYSIZE(aSysElements)];

    for (UINT i = 0; i < ARRAYSIZE(aSysElements); i++)
    {
        if (i == COLOR_SCROLLBAR || i == COLOR_BACKGROUND || i == COLOR_INACTIVECAPTION 
            || i == COLOR_MENU || i == COLOR_WINDOW || i == COLOR_INACTIVEBORDER || i == COLOR_INFOBK
            || i == COLOR_GRADIENTACTIVECAPTION || i == COLOR_MENUBAR)
                aNewColors[i] = RGB(0, 0, 0);
        else if (i == COLOR_ACTIVECAPTION || i == COLOR_ACTIVEBORDER || i == COLOR_BTNSHADOW
            ||i == COLOR_GRADIENTINACTIVECAPTION)
                aNewColors[i] = RGB(32, 32, 32);
        else if (i == COLOR_WINDOWFRAME || i == COLOR_BTNHIGHLIGHT)
            aNewColors[i] = RGB(64, 64, 64);
        else if (i == COLOR_MENUTEXT || i == COLOR_CAPTIONTEXT || i == COLOR_HIGHLIGHTTEXT 
            || i == COLOR_BTNTEXT | i == COLOR_INFOTEXT)
                aNewColors[i] = RGB(220, 220, 220);
        else if (i == COLOR_WINDOWTEXT)
            aNewColors[i] = RGB(240, 240, 240);
        else if (i == COLOR_APPWORKSPACE)
            aNewColors[i] = RGB(8, 8, 8);
        else if (i == COLOR_HIGHLIGHT || i == COLOR_MENUHILIGHT)
            aNewColors[i] = RGB(0, 120, 215);
        else if (i == COLOR_BTNFACE)
            aNewColors[i] = RGB(1, 1, 1);
        else if (i == COLOR_GRAYTEXT)
            aNewColors[i] = RGB(128, 128, 128);
        else if (i == COLOR_INACTIVECAPTIONTEXT)
            aNewColors[i] = RGB(160, 160, 160);
        else if (i == COLOR_3DDKSHADOW)
            aNewColors[i] = RGB(16, 16, 16);
        else if (i == COLOR_3DLIGHT)
            aNewColors[i] = RGB(4, 4, 4);
        else if (i == COLOR_HOTLIGHT)
            aNewColors[i] = RGB(0, 148, 251);        
    }
    SetSysColors(ARRAYSIZE(aSysElements), aSysElements, aNewColors);
}

HRESULT WINAPI HookedGetThemeBitmap(
    HTHEME hTheme,
    int iPartId,
    int iStateId,
    int iPropId,
    ULONG dwFlags,
    HBITMAP* phBitmap)
{
    std::wstring ThemeClassName = GetThemeClass(hTheme);

    if (ThemeClassName == L"Tab" && iPartId == 10)
    {    
        BITMAP bm = {};
        if (!GetObject(*phBitmap, sizeof(bm), &bm))
            return false;

        if (bm.bmBitsPixel != 32)
            return false;

        int size = bm.bmWidth * bm.bmHeight * 4;
        BYTE* pBits = new BYTE[size];
        if (!pBits)
            return false;

        if (GetBitmapBits(*phBitmap, size, pBits) != size)
        {
            delete[] pBits;
            return false;
        }

        for (int y = 0; y < bm.bmHeight; y++)
        {
            BYTE* pPixel = pBits + bm.bmWidth * 4 * y;
            for (int x = 0; x < bm.bmWidth; x++, pPixel += 4)
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

HRESULT WINAPI HookedGetColorTheme(HTHEME hTheme, int iPartId, int iStateId, int iPropId, COLORREF *pColor) 
{
    HRESULT hr = GetThemeColor_orig(hTheme, iPartId, iStateId, iPropId, pColor);
    
    std::wstring ThemeClassName = GetThemeClass(hTheme);
    
    if (ThemeClassName == L"Header" && iPartId == 1 && iStateId != 0  && iPropId == TMT_TEXTCOLOR)
    {
        *pColor = RGB(192, 192, 192);
        return S_OK;
    } 
    else if (ThemeClassName == L"PreviewPane" && (iPartId == 5 || iPartId == 7) && iPropId == TMT_FILLCOLOR)
    {
        *pColor = RGB(255, 255, 255);
        return S_OK;
    } 
    else if (ThemeClassName == L"PreviewPane" && iPartId == 6 && iPropId == TMT_FILLCOLOR)
    {
        *pColor = RGB(192, 192, 192);
        return S_OK;
    } 
    /* Control Panel */
    else if (ThemeClassName == L"ControlPanelStyle" && iPropId == TMT_TEXTCOLOR)
    {
        // BODYTITLE, GROUPTEXT, MESSAGETEXT, BODYTEXT , TITLE, CONTENTPANELLABEL
        if ((iPartId == 19 || iPartId == 9 || iPartId == 15 
            || iPartId == 6 || iPartId == 5 || iPartId == 4) && iStateId == 0)
        {
            *pColor =  RGB(255, 255, 255);
            return S_OK;
        }
        // SECTIONTITLELINK
        if (iPartId == 11)
        {
            if (iStateId == 1)
            {
                *pColor = RGB(240, 255, 240);
                return S_OK;
            }
            else if (iStateId == 2)
            {
                *pColor = RGB(224, 255, 224);
                return S_OK;
            }                    
        }
        // CONTENTLINK, HELPLINK
        if (iPartId == 10 || iPartId == 7)
        {
            if (iStateId == 1)
            {
                *pColor = RGB(96, 205, 255);
                return S_OK;
            }
            else if (iStateId == 2)
            {
                *pColor = RGB(153, 236, 255);
                return S_OK;
            }
            else if (iStateId == 3)
            {
                *pColor = RGB(0, 148, 251);
                return S_OK;
            }
            else if (iStateId == 4)
            {
                *pColor = RGB(96, 96, 96);
                return S_OK;
            }
        }
        // TASKLIST
        if (iPartId == 8)
        {
            if (iStateId == 1)
            {
                *pColor = RGB(190, 190, 190);
                return S_OK;
            }
            else if (iStateId == 2)
            {
                *pColor = RGB(255, 255, 255);
                return S_OK;
            }
            else if (iStateId == 3)
            {
                *pColor = RGB(160, 160, 160);
                return S_OK;
            }
            else if (iStateId == 4)
            {
                *pColor = RGB(96, 96, 96);
                return S_OK;
            }
            else if (iStateId == 5)
            {
                *pColor = RGB(255, 255, 255);
                return S_OK;
            }
        }
    }     
    else if (ThemeClassName == L"ControlPanelStyle" && iPropId == TMT_FILLCOLORHINT)
    {
        // CONTENTPANELINE
        if (iPartId == 17 && iStateId == 0)
        {
            *pColor = RGB(64, 64, 64);
            return S_OK;
        }
    }
    else if (ThemeClassName == L"ControlPanel" && iPropId == TMT_FILLCOLOR)
    {
        // CONTENTPANE
        if(iPartId == 2 && iStateId == 0)
        {
            *pColor = RGB(0, 0, 0);
            return S_OK;
        }
    }
    else if (ThemeClassName == L"CommandModule" && iPropId == TMT_TEXTCOLOR)
    {
        // TASKBUTTON
        if(iPartId == 3 && iStateId == 1)
        {
            *pColor = RGB(255, 255, 255);
            return S_OK;
        }
        // LYBRARYPANETOPVIEW
        else if (iPartId == 9)
        {
            if (iStateId == 1)
            {
                *pColor = RGB(96, 205, 255);
                return S_OK;
            }
            else if (iStateId == 2)
            {
                *pColor = RGB(153, 236, 255);
                return S_OK;
            }
            else if (iStateId == 3)
            {
                *pColor = RGB(0, 148, 251);
                return S_OK;
            }
            else if (iStateId == 6)
            {
                *pColor = RGB(96, 96, 96);
                return S_OK;
            }
        }  
    }
    else if (((ThemeClassName == L"Button" && iPartId != 2) || ThemeClassName == L"Tab" || ThemeClassName == L"Combobox" 
        || ThemeClassName == L"Toolbar") && iPropId == TMT_TEXTCOLOR)
    {
        return hr;
    }
    else if (ThemeClassName == L"Menu" || ThemeClassName == L"ChartView" || ThemeClassName == L"TaskManager")
    {
        return hr;
    }
    else
    {
        if (iPropId == TMT_TEXTCOLOR)
        {
            *pColor = RGB(255,255,255);
            return S_OK;
        }
        else if (iPropId == TMT_FILLCOLOR)
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

HRESULT WINAPI HookedDrawThemeBackground(
    HTHEME hTheme,
    HDC hdc,
    int iPartId,
    int iStateId,
    LPCRECT pRect,
    LPCRECT pClipRect)
{    
    HRESULT hr = DrawThemeBackground_orig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
    
    std::wstring ThemeClassName = GetThemeClass(hTheme);

    if(((ThemeClassName == L"AddressBand" || ThemeClassName == L"SearchBox") && iPartId == 1 && (iStateId == 1 || iStateId == 2))
        || (ThemeClassName == L"Rebar" && (iPartId == 3 || iPartId == 6) && iStateId == 0)
        || (ThemeClassName == L"Header" && (iPartId == 0 || (iPartId == 1 && (iStateId == 1 || iStateId == 4 || iStateId == 7 ))))
        || (ThemeClassName == L"TaskDialog" && iPartId == 15 && iStateId == 0)
        || (ThemeClassName == L"Tab" && iPartId == 9)
        || (ThemeClassName == L"Status" && iPartId == 0)
        || (ThemeClassName == L"Edit" && (iPartId == 3 || iPartId == 5))
        || (ThemeClassName == L"Tooltip" && iPartId == 1)
        /*|| (ThemeClassName == L"Menu" && iPartId == 7)*/)
    {
        FillRect(hdc, pRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        return S_OK;
    }
    else 
    {
        HTHEME hTh = OpenThemeData(NULL, L"Tab");
        HBITMAP bm {};
        GetThemeBitmap(hTh, 10, 0, TMT_DIBDATA, GBF_DIRECT, &bm);
        CloseThemeData(hTh);
    }

    return hr;
}

HRESULT WINAPI HookedDrawThemeBackgroundEx(
    HTHEME hTheme,
    HDC hdc,
    int iPartId,
    int iStateId,
    LPCRECT pRect,
    const DTBGOPTS* pOptions)
{
    HRESULT hr = DrawThemeBackgroundEx_orig(hTheme, hdc, iPartId, iStateId, pRect, pOptions);

    std::wstring ThemeClassName = GetThemeClass(hTheme);

    if ((ThemeClassName == L"Rebar" && (iPartId == 3 || iPartId == 6) && iStateId == 0) 
        || (ThemeClassName == L"TreeView" && iPartId == 0))
    {
        return S_OK;    
    }
    else if ((ThemeClassName == L"PreviewPane" && (iPartId == 1 || iPartId == 3))
        || (ThemeClassName == L"Header" && (iPartId == 0 || (iPartId == 1 && 
        (iStateId == 1 || iStateId == 4 || iStateId == 7 ))))
        || (ThemeClassName == L"CommandModule" && iPartId == 1 && iStateId == 0)
        || (ThemeClassName == L"TaskDialog" && (iPartId == 4 || iPartId == 15 || iPartId == 8) && iStateId == 0)
        || (ThemeClassName == L"TaskDialog" && iPartId == 1)
        || (ThemeClassName == L"AeroWizard" && (iPartId == 1 || iPartId == 2 || iPartId == 3 || iPartId == 4))
        || (ThemeClassName == L"CommonItemsDialog" && iPartId == 1)
        || (ThemeClassName == L"ControlPanel" && (iPartId == 2 || iPartId == 17 || iPartId == 18 || iPartId == 12 || iPartId == 13))
        || (ThemeClassName == L"PreviewPane" && (iPartId == 3 || iPartId == 1)))
    {
        FillRect(hdc, pRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        return S_OK;
    }
    
    return hr;
}

void ApplyFrameExtension(HWND hWnd) 
{
    MARGINS margins = {-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(hWnd, &margins);
}

void EnableBlurBehind(HWND hWnd)
{
    // Does not interfere with the Windows Terminal, GameBar overlay
    if(!(IsWindowClass(hWnd, L"CASCADIA_HOSTING_WINDOW_CLASS") || IsWindowClass(hWnd, L"ApplicationFrameWindow")))
    {
        typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);

        bb.fEnable = TRUE;
        bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
        // Blurs window client area
        HRGN hRgn = CreateRectRgn(0, 0, -1, -1);
        bb.hRgnBlur = hRgn;

        DwmEnableBlurBehindWindow(hWnd, &bb);
        DeleteObject(hRgn);

        accent.AccentState = 4;
        accent.GradientColor = g_settings.AccentBlurBehindClr;
        //Change caption button color state
        //accent.AccentFlags |= 3584;

        attrib.Attrib = WCA_ACCENT_POLICY;
        attrib.pvData = &accent;
        attrib.cbData = sizeof(accent);
        
        auto SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)
            GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");
        if (SetWindowCompositionAttribute) 
            SetWindowCompositionAttribute(hWnd, &attrib);
    }
}

void SetSystemBackdrop(HWND hWnd, const UINT type)
{
    DwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE , &type, sizeof(UINT));
}

void EnableColoredTitlebar(HWND hWnd)
{
    g_settings.g_TitlebarColor = g_settings.TitlebarActiveColor;
    DwmSetWindowAttribute(hWnd, DWMWA_CAPTION_COLOR, &g_settings.g_TitlebarColor, sizeof(COLORREF));
}

void EnableCaptionTextColor(HWND hWnd)
{
    g_settings.g_CaptionColor = g_settings.CaptionActiveTextColor;
    DwmSetWindowAttribute(hWnd, DWMWA_TEXT_COLOR, &g_settings.g_CaptionColor, sizeof(COLORREF));
}

void EnableColoredBorder(HWND hWnd)
{
    g_settings.g_BorderColor = g_settings.BorderActiveColor;
    DwmSetWindowAttribute(hWnd, DWMWA_BORDER_COLOR, &g_settings.g_BorderColor, sizeof(COLORREF));
}

void SetCornerType(HWND hWnd, const UINT type)
{
    DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &type , sizeof(UINT));
}

static COLORREF HSLToRGB(float h, float s, float l) {
    float c = (1.0f - fabs(2.0f * l - 1.0f)) * s;
    float x = c * (1.0f - fabs(fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = l - c / 2.0f;

    float r_prime, g_prime, b_prime;
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

void CALLBACK MyRainbowTimerProc(HWND, UINT, UINT_PTR t_id, DWORD)
{
    HWND WndHwnd= nullptr;
    {
        std::lock_guard<std::mutex> guard(g_rainbowWindowsMutex);
        for(auto& hWnd : g_rainbowWindows)
        {
            HANDLE value = GetPropW(hWnd, g_RainbowPropStr.c_str());
            if (value && (UINT_PTR)value == t_id)
            {
                WndHwnd = hWnd;
                break;
            }
        }
    }

    if (WndHwnd)
    {
	    // Credits to @m417z
        std::mt19937 gen((UINT_PTR)WndHwnd);
        std::uniform_real_distribution<> distr(0.0, 360.0);
        double InitialHueOffset = distr(gen);

        double wndHue = fmod(InitialHueOffset + TimerGetSeconds() * g_settings.RainbowSpeed, 360.0);

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
}

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
    if (nCode != HC_ACTION) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    switch (cwp->message)
    {
        case WM_ACTIVATE:
        {
            bool isMinimized = HIWORD(cwp->wParam);
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
        case WM_DESTROY:
        {       
            HANDLE value = RemovePropW(cwp->hwnd, g_RainbowPropStr.c_str());
            if (value)
            {
                KillTimer(NULL, (UINT_PTR)value);
                {
                    std::lock_guard<std::mutex> guard(g_rainbowWindowsMutex);
                    g_rainbowWindows.erase(cwp->hwnd);
                }
            }
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
                        HANDLE value = GetPropW(cwp->hwnd, g_RainbowPropStr.c_str());
                        if (!value)
                        {
                            UINT_PTR timersId = SetTimer(NULL, NULL, 32, MyRainbowTimerProc);
                            if (timersId)
                            {
                                SetPropW(cwp->hwnd, g_RainbowPropStr.c_str(), (HANDLE)timersId);
                                {
                                    std::lock_guard<std::mutex> guard(g_rainbowWindowsMutex);
                                    g_rainbowWindows.insert(cwp->hwnd);
                                }
                            }
                        }
                        break;
                    }
                    case RAINBOW_UNLOAD:
                    {
                        HANDLE value = RemovePropW(cwp->hwnd, g_RainbowPropStr.c_str());
                        if (value)
                            KillTimer(NULL, (UINT_PTR)value);                  
                        break;
                    }
                }
            }
            break;
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        if (g_callWndProcHook) {
            std::lock_guard<std::mutex> guard(g_allCallWndProcHooksMutex);

            auto it = g_allCallWndProcHooks.find(g_callWndProcHook);
            if (it != g_allCallWndProcHooks.end()) {
                UnhookWindowsHookEx(g_callWndProcHook);
                g_allCallWndProcHooks.erase(it);
            }
        }
        break;

    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}

void NewWindowShown(HWND hWnd) 
{
    if(!IsWindowEligible(hWnd))
        return;

    if(g_settings.ExtendFrame)
        ApplyFrameExtension(hWnd);

    if(g_settings.CaptionTextFlag && !g_settings.CaptionRainbowFlag)
        EnableCaptionTextColor(hWnd);
    
    if(g_settings.BorderFlag && !g_settings.BorderRainbowFlag)
        EnableColoredBorder(hWnd);
        
    if(!g_settings.TitlebarFlag)
    {
        if (g_settings.ImmersiveDarkmode)
            DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &ENABLE, sizeof(UINT));
        if(g_settings.BgType == g_settings.AccentBlurBehind)
            EnableBlurBehind(hWnd);
        else if(g_settings.BgType == g_settings.AcrylicSystemBackdrop)
            SetSystemBackdrop(hWnd, TRANSIENTWINDOW);
        else if(g_settings.BgType == g_settings.Mica)
            SetSystemBackdrop(hWnd, MAINWINDOW);
        else if(g_settings.BgType == g_settings.MicaAlt)
            SetSystemBackdrop(hWnd, TABBEDWINDOW);
    }
    else if (!g_settings.TitlebarRainbowFlag)
        EnableColoredTitlebar(hWnd);

    if (g_settings.CornerPref  == g_settings.NotRounded)
        SetCornerType(hWnd, DONTROUND);
    else if (g_settings.CornerPref  == g_settings.SmallRounded)
        SetCornerType(hWnd, SMALLROUND);
    
    if(g_settings.BorderFlag || g_settings.CaptionTextFlag || g_settings.TitlebarFlag)
    {
        {
            std::lock_guard<std::mutex> guard(g_allCallWndProcHooksMutex);   
            DWORD dwThreadId = GetWindowThreadProcessId(hWnd, NULL);
            HHOOK callWndProcHook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc, nullptr, dwThreadId);
            if (callWndProcHook) {
                g_callWndProcHook = callWndProcHook;
                g_allCallWndProcHooks.insert(callWndProcHook);
            }
        }
        if (g_settings.BorderRainbowFlag || g_settings.CaptionRainbowFlag || g_settings.TitlebarRainbowFlag)
            SendMessage(hWnd, g_msgRainbowTimer, RAINBOW_LOAD, 0);
    }
    
}

void DwmExpandFrameIntoClientAreaHook()
{
    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandle(L"dwmapi.dll"), "DwmExtendFrameIntoClientArea"),
                       (void*)HookedDwmExtendFrameIntoClientArea,
                       (void**)&DwmExtendFrameIntoClientArea_orig);
}

void DwmSetWindowAttributeHook()
{
    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandle(L"dwmapi.dll"), "DwmSetWindowAttribute"),
                       (void*)HookedDwmSetWindowAttribute,
                       (void**)&DwmSetWindowAttribute_orig); 
}

void FillBackgroundElements()
{
    ColorizeSysColors();
    CplDuiHook();
    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandle(L"user32.dll"), "GetSysColor"), (void*)HookedGetSysColor, (void**)&GetSysColor_orig);
    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandle(L"uxtheme.dll"), "GetThemeBitmap"), (void*)HookedGetThemeBitmap, (void**)&GetThemeBitmap_orig);
    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandle(L"uxtheme.dll"), "GetThemeColor"), (void*)HookedGetColorTheme, (void**)&GetThemeColor_orig);   
    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandle(L"uxtheme.dll"), "DrawThemeBackground"), (void*)HookedDrawThemeBackground, (void**)&DrawThemeBackground_orig);
    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandle(L"uxtheme.dll"), "DrawThemeBackgroundEx"), (void*)HookedDrawThemeBackgroundEx, (void**)&DrawThemeBackgroundEx_orig);
}

void TextRenderingHook()
{
    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandle(L"user32.dll"), "DrawTextW"), (void*)HookedDrawTextW, (void**)&DrawTextW_orig);
    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandle(L"user32.dll"), "DrawTextExW"), (void*)HookedDrawTextExW, (void**)&DrawTextExW_orig);
    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandle(L"gdi32.dll"), "ExtTextOutW"), (void*)HookedExtTextOutW, (void**)&ExtTextOutW_orig);
    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandle(L"uxtheme.dll"), "DrawThemeText"), (void*)HookedDrawThemeText, (void**)&DrawThemeText_orig);
    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandle(L"uxtheme.dll"), "DrawThemeTextEx"), (void*)HookedDrawThemeTextEx, (void**)&DrawThemeTextEx_orig);
}

void RestoreWindowCustomizations(HWND hWnd)
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
    bb.fEnable = FALSE;
    bb.hRgnBlur = NULL;
    DwmEnableBlurBehindWindow(hWnd, &bb);

    accent.AccentState = 0;

    attrib.Attrib = WCA_ACCENT_POLICY;
    attrib.pvData = &accent;
    attrib.cbData = sizeof(accent);

    typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
    auto SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");
    if (SetWindowCompositionAttribute) 
        SetWindowCompositionAttribute(hWnd, &attrib);
    
    DwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE , &AUTO, sizeof(UINT));
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) 
{
    DWORD dwProcessId = 0;
    if (!GetWindowThreadProcessId(hWnd, &dwProcessId) || dwProcessId != GetCurrentProcessId()) 
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

void ApplyForExistingWindows()
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
        DWORD colorization;
        BOOL opaque;
        if (SUCCEEDED(DwmGetColorizationColor(&colorization, &opaque))) 
        {
            outColor = RGB(
                (colorization >> 16) & 0xFF,
                (colorization >> 8) & 0xFF,
                colorization & 0xFF
            );
            return TRUE;
        }
        outColor = DWMWA_COLOR_DEFAULT;
        return FALSE;
    }
    else 
    {
        size_t len = wcslen(hexColor);
        if (len != 6 && len != 8)
            return FALSE;
        
        auto hexToByte = [](WCHAR c) -> int {
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
            int alphaHigh = hexToByte(hexColor[0]);
            int alphaLow  = hexToByte(hexColor[1]);
            if (alphaHigh < 0 || alphaLow < 0)
                return FALSE;
            alpha = (alphaHigh << 4) | alphaLow;
            hexColor += 2;
        }

        for (int i = 0; i < 3; ++i) 
        {
            int high = hexToByte(hexColor[i * 2]);
            int low  = hexToByte(hexColor[i * 2 + 1]);
            if (high < 0 || low < 0)
                return FALSE;
            rgb[i] = (high << 4) | low;
        }

        outColor = (alpha << 24) | (rgb[2] << 16) | (rgb[1] << 8) | rgb[0];
        return TRUE;
    
    }
}

double RainbowSpeedInput(int input)
{
    if (input < 1 || input > 100)
        return 1.0f;

    return (double)input * 3.6;    
}

void GetProcStrFromPath(std::wstring& path) {
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

void GetCurrProcInfo(std::wstring& outName, DWORD& outPID) {
    WCHAR modulePath[MAX_PATH];
    GetModuleFileNameW(NULL, modulePath, MAX_PATH);

    outName = modulePath;
    GetProcStrFromPath(outName);

    outPID = GetCurrentProcessId();
}

void LoadSettings(void)
{
    g_settings.FillBg = Wh_GetIntSetting(L"ThemeBackground");
    if(g_settings.FillBg)
        FillBackgroundElements();

    g_settings.TextAlphaBlend = Wh_GetIntSetting(L"TextAlphaBlend");
    if(g_settings.TextAlphaBlend)
        TextRenderingHook();

    LPCWSTR pszStyle = Wh_GetStringSetting(L"type");
    if (0 == wcscmp(pszStyle, L"acrylicblur"))
        g_settings.BgType = g_settings.AccentBlurBehind;
    else if (0 == wcscmp(pszStyle, L"acrylicsystem"))
        g_settings.BgType = g_settings.AcrylicSystemBackdrop;
    else if (0 == wcscmp(pszStyle, L"mica"))
        g_settings.BgType = g_settings.Mica;
    else if (0 == wcscmp(pszStyle, L"mica_tabbed"))
        g_settings.BgType = g_settings.MicaAlt;
    else 
        g_settings.BgType = g_settings.Default;

    GetColorSetting(Wh_GetStringSetting(L"AccentBlurBehind"), g_settings.AccentBlurBehindClr);

    g_settings.ImmersiveDarkmode = Wh_GetIntSetting(L"ImmersiveDarkTitle");
    
    g_settings.ExtendFrame = Wh_GetIntSetting(L"ExtendFrame");
    if(g_settings.ExtendFrame)
        DwmExpandFrameIntoClientAreaHook();
    
    LPCWSTR pszCornerType = Wh_GetStringSetting(L"CornerOption");
    if (0 == wcscmp(pszCornerType, L"notrounded"))
        g_settings.CornerPref = g_settings.NotRounded;
    else if (0 == wcscmp(pszCornerType, L"smallround"))
        g_settings.CornerPref = g_settings.SmallRounded;
    else
        g_settings.CornerPref = g_settings.DefaultRounded;
    Wh_FreeStringSetting(pszCornerType);

    DwmSetWindowAttributeHook();

    g_settings.RainbowSpeed = RainbowSpeedInput(Wh_GetIntSetting(L"RainbowSpeed"));

    g_settings.TitlebarFlag = Wh_GetIntSetting(L"TitlebarColor.ColorTitlebar");
    g_settings.TitlebarRainbowFlag = Wh_GetIntSetting(L"TitlebarColor.RainbowTitlebar") && g_settings.TitlebarFlag;
    if(g_settings.TitlebarFlag)
    {
        LPCWSTR pszTitlebarStyle = Wh_GetStringSetting(L"TitlebarColor.titlerbarstyles_active");
        g_settings.TitlebarFlag = GetColorSetting(pszTitlebarStyle, g_settings.TitlebarActiveColor);
        pszTitlebarStyle = Wh_GetStringSetting(L"TitlebarColor.titlerbarstyles_inactive");
        g_settings.TitlebarFlag = GetColorSetting(pszTitlebarStyle, g_settings.TitlebarInactiveColor) || g_settings.TitlebarFlag;

        if ((g_settings.TitlebarActiveColor == DWMWA_COLOR_DEFAULT || g_settings.TitlebarActiveColor == DWMWA_COLOR_NONE) ||
            (g_settings.TitlebarInactiveColor == DWMWA_COLOR_DEFAULT || g_settings.TitlebarInactiveColor == DWMWA_COLOR_NONE))
                g_settings.TitlebarFlag = FALSE;

        Wh_FreeStringSetting(pszTitlebarStyle);
    }

    g_settings.CaptionTextFlag = Wh_GetIntSetting(L"TitlebarTextColor.ColorTitlebarText");
    g_settings.CaptionRainbowFlag = Wh_GetIntSetting(L"TitlebarTextColor.RainbowTextColor") && g_settings.CaptionTextFlag;
    if(g_settings.CaptionTextFlag)
    {
        LPCWSTR pszTitlebarTextColorStyle = Wh_GetStringSetting(L"TitlebarTextColor.titlerbarcolorstyles_active");
        g_settings.CaptionTextFlag = GetColorSetting(pszTitlebarTextColorStyle, g_settings.CaptionActiveTextColor);
        pszTitlebarTextColorStyle = Wh_GetStringSetting(L"TitlebarTextColor.titlerbarcolorstyles_inactive");
        g_settings.CaptionTextFlag = GetColorSetting(pszTitlebarTextColorStyle, g_settings.CaptionInactiveTextColor) || g_settings.CaptionTextFlag;

        Wh_FreeStringSetting(pszTitlebarTextColorStyle);
    }

    g_settings.BorderFlag = Wh_GetIntSetting(L"BorderColor.ColorBorder");
    g_settings.BorderRainbowFlag = Wh_GetIntSetting(L"BorderColor.RainbowBorder") && g_settings.BorderFlag;
    if(g_settings.BorderFlag)
    {
        LPCWSTR pszBorderStyle = Wh_GetStringSetting(L"BorderColor.borderstyles_active");
        g_settings.BorderFlag = GetColorSetting(pszBorderStyle, g_settings.BorderActiveColor);
        pszBorderStyle = Wh_GetStringSetting(L"BorderColor.borderstyles_inactive");
        g_settings.BorderFlag = GetColorSetting(pszBorderStyle, g_settings.BorderInactiveColor) || g_settings.BorderFlag;

        Wh_FreeStringSetting(pszBorderStyle);
    }

    g_settings.MenuBorderFlag = Wh_GetIntSetting(L"BorderColor.MenuBorderColor");

    Wh_FreeStringSetting(pszStyle);

    // Process Rules

    DWORD currProcID = NULL;
    std::wstring currproc = {};
    GetCurrProcInfo(currproc, currProcID);

    for (int i = 0;; i++) 
    {
        PCWSTR program = Wh_GetStringSetting(L"RuledPrograms[%d].target", i);
        
        BOOL hasProgram = *program;
        if (hasProgram) 
        {
            std::wstring ruledproc = program;
            GetProcStrFromPath(ruledproc);
            
            if(currproc == ruledproc)
            {
                g_settings.FillBg = Wh_GetIntSetting(L"RuledPrograms[%d].ThemeBackground", i);
                if(g_settings.FillBg)
                    FillBackgroundElements();
                
                g_settings.TextAlphaBlend = Wh_GetIntSetting(L"RuledPrograms[%d].TextAlphaBlend", i);
                if(g_settings.TextAlphaBlend)
                    TextRenderingHook();

                LPCWSTR pszStyle = Wh_GetStringSetting(L"RuledPrograms[%d].type", i);
                if (0 == wcscmp(pszStyle, L"acrylicblur"))
                    g_settings.BgType = g_settings.AccentBlurBehind;
                else if (0 == wcscmp(pszStyle, L"acrylicsystem"))
                    g_settings.BgType = g_settings.AcrylicSystemBackdrop;
                else if (0 == wcscmp(pszStyle, L"mica"))
                    g_settings.BgType = g_settings.Mica;
                else if (0 == wcscmp(pszStyle, L"mica_tabbed"))
                    g_settings.BgType = g_settings.MicaAlt;
                else 
                    g_settings.BgType = g_settings.Default;
                
                Wh_FreeStringSetting(pszStyle);

                GetColorSetting(Wh_GetStringSetting(L"RuledPrograms[%d].AccentBlurBehind", i), g_settings.AccentBlurBehindClr);

                g_settings.ImmersiveDarkmode = Wh_GetIntSetting(L"RuledPrograms[%d].ImmersiveDarkTitle", i);

                g_settings.ExtendFrame = Wh_GetIntSetting(L"RuledPrograms[%d].ExtendFrame", i);
                if(g_settings.ExtendFrame)
                    DwmExpandFrameIntoClientAreaHook();

                LPCWSTR pszCornerType = Wh_GetStringSetting(L"RuledPrograms[%d].CornerOption", i);
                if (0 == wcscmp(pszCornerType, L"notrounded"))
                    g_settings.CornerPref = g_settings.NotRounded;
                else if (0 == wcscmp(pszCornerType, L"smallround"))
                    g_settings.CornerPref = g_settings.SmallRounded;
                else
                    g_settings.CornerPref = g_settings.DefaultRounded;
                Wh_FreeStringSetting(pszCornerType);
                
                g_settings.RainbowSpeed = RainbowSpeedInput(Wh_GetIntSetting(L"RuledPrograms[%d].RainbowSpeed", i));

                g_settings.TitlebarFlag = Wh_GetIntSetting(L"RuledPrograms[%d].TitlebarColor.ColorTitlebar", i);
                g_settings.TitlebarRainbowFlag = Wh_GetIntSetting(L"RuledPrograms[%d].TitlebarColor.RainbowTitlebar", i) && g_settings.TitlebarFlag;
                if(g_settings.TitlebarFlag)
                {
                    LPCWSTR pszTitlebarStyle = Wh_GetStringSetting(L"RuledPrograms[%d].TitlebarColor.Titlerbarstyles_active", i);
                    g_settings.TitlebarFlag = GetColorSetting(pszTitlebarStyle, g_settings.TitlebarActiveColor);
                    pszTitlebarStyle = Wh_GetStringSetting(L"RuledPrograms[%d].TitlebarColor.Titlerbarstyles_inactive", i);
                    g_settings.TitlebarFlag = GetColorSetting(pszTitlebarStyle, g_settings.TitlebarInactiveColor) || g_settings.TitlebarFlag;

                    if ((g_settings.TitlebarActiveColor == DWMWA_COLOR_DEFAULT || g_settings.TitlebarActiveColor == DWMWA_COLOR_NONE) ||
                        (g_settings.TitlebarInactiveColor == DWMWA_COLOR_DEFAULT || g_settings.TitlebarInactiveColor == DWMWA_COLOR_NONE))
                            g_settings.TitlebarFlag = FALSE;

                    Wh_FreeStringSetting(pszTitlebarStyle);
                }

                g_settings.CaptionTextFlag = Wh_GetIntSetting(L"RuledPrograms[%d].TitlebarTextColor.ColorTitlebarText", i);
                g_settings.CaptionRainbowFlag = Wh_GetIntSetting(L"RuledPrograms[%d].TitlebarTextColor.RainbowTextColor", i) && g_settings.CaptionTextFlag;
                if(g_settings.CaptionTextFlag)
                {
                    LPCWSTR pszTitlebarTextColorStyle = Wh_GetStringSetting(L"RuledPrograms[%d].TitlebarTextColor.titlerbarcolorstyles_active", i);
                    g_settings.CaptionTextFlag = GetColorSetting(pszTitlebarTextColorStyle, g_settings.CaptionActiveTextColor);
                    pszTitlebarTextColorStyle = Wh_GetStringSetting(L"RuledPrograms[%d].TitlebarTextColor.titlerbarcolorstyles_inactive", i);
                    g_settings.CaptionTextFlag = GetColorSetting(pszTitlebarTextColorStyle, g_settings.CaptionInactiveTextColor) || g_settings.CaptionTextFlag;
                    Wh_FreeStringSetting(pszTitlebarTextColorStyle);
                }

                g_settings.BorderFlag = Wh_GetIntSetting(L"RuledPrograms[%d].BorderColor.ColorBorder", i);

                g_settings.BorderFlag = Wh_GetIntSetting(L"RuledPrograms[%d].BorderColor.ColorBorder", i);
                g_settings.BorderRainbowFlag = Wh_GetIntSetting(L"RuledPrograms[%d].BorderColor.RainbowBorder", i) && g_settings.BorderFlag;
                if(g_settings.BorderFlag)
                {
                    LPCWSTR pszBorderStyle = Wh_GetStringSetting(L"RuledPrograms[%d].BorderColor.borderstyles_active", i);
                    g_settings.BorderFlag = GetColorSetting(pszBorderStyle, g_settings.BorderActiveColor);
                    pszBorderStyle = Wh_GetStringSetting(L"RuledPrograms[%d].BorderColor.borderstyles_inactive", i);
                    g_settings.BorderFlag = GetColorSetting(pszBorderStyle, g_settings.BorderInactiveColor) || g_settings.BorderFlag;
                    Wh_FreeStringSetting(pszBorderStyle);
                }

                g_settings.MenuBorderFlag = g_settings.BorderFlag;
            }
        }

        Wh_FreeStringSetting(program);

        if (!hasProgram) {
            break;
        }
    }    
}

BOOL Wh_ModInit(void) 
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

    Wh_SetFunctionHook((void*)pNtUserCreateWindowEx,
                       (void*)HookedNtUserCreateWindowEx,
                       (void**)&NtUserCreateWindowEx_Original);

    return TRUE;
}

void Wh_ModAfterInit() 
{
    #ifdef _WIN64
        const size_t OFFSET_SAME_TEB_FLAGS = 0x17EE;
    #else
        const size_t OFFSET_SAME_TEB_FLAGS = 0x0FCA;
    #endif
    bool isInitialThread = *(USHORT*)((BYTE*)NtCurrentTeb() + OFFSET_SAME_TEB_FLAGS) & 0x0400;
    if (!isInitialThread)
        ApplyForExistingWindows();
}

void Wh_ModUninit(void) 
{ 
    g_settings.Unload = TRUE;
    RevertSysColors();

    std::unordered_set<HWND> RainbowWindows;
    {
        std::lock_guard<std::mutex> guard(g_rainbowWindowsMutex);

        RainbowWindows = std::move(g_rainbowWindows);
        g_rainbowWindows.clear();
    }

    for (auto hWnd : RainbowWindows)
        SendMessageW(hWnd, g_msgRainbowTimer, RAINBOW_UNLOAD, 0);
    
    {
        std::lock_guard<std::mutex> guard(g_allCallWndProcHooksMutex);

        for (HHOOK hook : g_allCallWndProcHooks) {
            UnhookWindowsHookEx(hook);
        }

        g_allCallWndProcHooks.clear();
    }
    
    ApplyForExistingWindows();

    RainbowWindows.clear();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) 
{
    Wh_Log(L"SettingsChanged");
    
    *bReload = TRUE;
    return TRUE;
}
