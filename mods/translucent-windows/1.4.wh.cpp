// ==WindhawkMod==
// @id              translucent-windows
// @name            Translucent Windows
// @description     Enables native translucent effects in Windows 11
// @version         1.4
// @author          Undisputed00x
// @github          https://github.com/Undisputed00x
// @include         *
// @compilerOptions -ldwmapi -luxtheme -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

### Acrylic (BlurBehind)
![BlurBehind](https://i.imgur.com/S8OGRvc.png)
### Acrylic (SystemBackdrop)
![Acrylic SystemBackdrop](https://i.imgur.com/66cAiEC.png)
### Mica
![Mica](https://i.imgur.com/7BGDhJa.png)
### MicaAlt
![MicaTabbed](https://i.imgur.com/EeviFqO.png)

# FAQ

* Prerequisited windows settings to enable the background effects
    - Transparency effects enabled
    - Energy saver disabled
#
* It is highly recommended to use the mod with black/dark window themes like the perfect black Rectify11 theme.

* Extending effects to the entire window can result in text being unreadable or even invisible in some cases. 
Light mode theme, HDR enabled or white background behind the window can cause this. 
This is because some GDI rendering operations do not preserve alpha channel values.

* Acrylic blur behind effect may show a bleeding effect at the edges of a window when 
maximized or snapped to the edge of the screen, this is caused by default by the windows DWMAPI.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ThemeBackground: FALSE
  $name: Optimize windows theme
  $description: >-
      Fill with black color the file explorer background in order to render a clear translucent effect.

      ⚠ Be aware when changing this setting, the affected windows will need to be reopened to see the change.
- type: none
  $name: Effects
  $description: >-
      Windows 11 version >= 22621.xxx (22H2) is required.

      ⚠ Be aware when changing the effect type from Acrylic BlurBehind or back to Default, the affected windows will need to be reopened to see the change.
  $options:
  - none: Default
  - acrylicblur: Acrylic (BlurBehind)
  - acrylicsystem: Acrylic (SystemBackdrop)
  - mica: Mica
  - mica_tabbed: MicaAlt
- ExtendFrame: FALSE
  $name: Extend effects into entire window
  $description: >-
    Extends the effects into the entire window background using DwmExtendFrameIntoClientArea. (Required for BlurBehind)
- TitlebarColor:
    - ColorTitlebar: FALSE
      $name: Enable
    - titlerbarstyles_active: "FF0000"
      $name: Focused window color
      $description: Color in hexadecimal RGB format e.g. Red = FF0000 or SystemAccentColor = 1
    - titlerbarstyles_inactive: "00FFFF"
      $name: Unfocused window color
      $description: Color in hexadecimal RGB format e.g. Red = FF0000 or SystemAccentColor = 1
  $name: Titlebar color
  $description: Windows 11 version >= 22000.xxx (21H2) is required. Overrides effects settings
- TitlebarTextColor:
    - ColorTitlebarText: FALSE
      $name: Enable
    - titlerbarcolorstyles_active: "FF0000"
      $name: Focused window color
      $description: Color in hexadecimal RGB format e.g. Red = FF0000 or SystemAccentColor = 1
    - titlerbarcolorstyles_inactive: "00FFFF"
      $name: Unfocused window color
      $description: Color in hexadecimal RGB format e.g. Red = FF0000 or SystemAccentColor = 1
  $name: Titlebar text color
  $description: >-
      Windows 11 version >= 22000.xxx (21H2) is required.

      NOTE: This settings affects only Win32 windows. Since Win11 24H2 File Explorer changed titlebar text rendering to DirectWrite API. To modify the text color of File Explorer title bar, use Windhawk's File Explorer Styler mod.
- BorderColor:
    - ColorBorder: FALSE
      $name: Enable
    - borderstyles_active: "FF0000"
      $name: Focused window color
      $description: >-
        Color in hexadecimal RGB format e.g. Red = FF0000

        Transparent = 0
        
        SystemAccentColor = 1
    - borderstyles_inactive: "00FFFF"
      $name: Unfocused window color
      $description: >-
        Color in hexadecimal RGB format e.g. Red = FF0000

        Transparent = 0
        
        SystemAccentColor = 1
    - MenuBorderColor: FALSE
      $name: Extend colored borders to classic context menus and taskbar thumbnails
      $description: Enable this option to get colored borders on windows classic context menus and taskbar thumbnails
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
      - type: none
        $name: Effects
        $description: >-
            Windows 11 version >= 22621.xxx (22H2) is required.

            ⚠ Be aware when changing the effect type from Acrylic BlurBehind or back to Default, the affected windows will need to be reopened to see the change.
        $options:
          - none: Default
          - acrylicblur: Acrylic (BlurBehind)
          - acrylicsystem: Acrylic (SystemBackdrop)
          - mica: Mica
          - mica_tabbed: MicaAlt
      - ExtendFrame: FALSE
        $name: Extend effects into entire window
        $description: >-
          Extends the effects into the entire window background using DwmExtendFrameIntoClientArea. (Required for BlurBehind)
      - TitlebarColor:
          - ColorTitlebar: FALSE
            $name: Enable
          - titlerbarstyles_active: "FF0000"
            $name: Focused window color
            $description: Color in hexadecimal RGB format e.g. Red = FF0000 or SystemAccentColor = 1
          - titlerbarstyles_inactive: "00FFFF"
            $name: Unfocused window color
            $description: Color in hexadecimal RGB format e.g. Red = FF0000 or SystemAccentColor = 1
        $name: Titlebar color
        $description: Windows 11 version >= 22000.xxx (21H2) is required. Overrides effects settings
      - TitlebarTextColor:
          - ColorTitlebarText: FALSE
            $name: Enable
          - titlerbarcolorstyles_active: "FF0000"
            $name: Focused window color
            $description: Color in hexadecimal RGB format e.g. Red = FF0000 or SystemAccentColor = 1
          - titlerbarcolorstyles_inactive: "00FFFF"
            $name: Unfocused window color
            $description: Color in hexadecimal RGB format e.g. Red = FF0000 or SystemAccentColor = 1
        $name: Titlebar text color
        $description: >-
          Windows 11 version >= 22000.xxx (21H2) is required.

          NOTE: This settings affects only Win32 windows. Since Win11 24H2 File Explorer changed titlebar text rendering to DirectWrite API. To modify the text color of File Explorer title bar, use Windhawk's File Explorer Styler mod.
      - BorderColor:
          - ColorBorder: FALSE
            $name: Enable
          - borderstyles_active: "FF0000"
            $name: Focused window color
            $description: >-
              Color in hexadecimal RGB format e.g. Red = FF0000

              Transparent = 0
            
              SystemAccentColor = 1
          - borderstyles_inactive: "00FFFF"
            $name: Unfocused window color
            $description: >-
              Color in hexadecimal RGB format e.g. Red = FF0000

              Transparent = 0
            
              SystemAccentColor = 1
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
#include <string>
#include <mutex>
#include <unordered_set>
#include <commctrl.h>

static constexpr UINT USE_IMMERSIVE_DARK_MODE = 20; // DWMWA_USE_IMMERSIVE_DARK_MODE
static constexpr UINT CAPTION_COLOR = 35; // DWMWA_CAPTION_COLOR
static constexpr UINT CAPTION_TEXT_COLOR = 36; // DWMWA_TEXT_COLOR
static constexpr UINT BORDER_COLOR = 34; // DWMWA_BORDER_COLOR
static UINT ENABLE = 1;
static constexpr UINT SYSTEMBACKDROP_TYPE = 38; // DWMWA_SYSTEMBACKDROP_TYPE
static constexpr UINT NONE = 1; // DWMSBT_NONE
static constexpr UINT MAINWINDOW = 2; // DWMSBT_MAINWINDOW
static constexpr UINT TRANSIENTWINDOW = 3; // DWMSBT_TRANSIENTWINDOW
static constexpr UINT TABBEDWINDOW = 4; // DWMSBT_TABBEDWINDOW
static constexpr UINT COLOR_DEFAULT = 0xFFFFFFFF; // DWMWA_COLOR_DEFAULT
static constexpr UINT COLOR_NONE = 0xFFFFFFFE; // DWMWA_COLOR_NONE

std::mutex g_subclassedWindowsMutex;
std::unordered_set<HWND> g_subclassedWindows;

struct Settings{
    BOOL FillBg = FALSE;
    BOOL ExtendFrame = FALSE;
    BOOL Unload = FALSE;
    BOOL TitlebarFlag = FALSE;
    BOOL CaptionTextFlag = FALSE;
    BOOL BorderFlag = FALSE;
    BOOL MenuBorderFlag = FALSE;
    COLORREF TitlebarActiveColor = COLOR_DEFAULT;
    COLORREF CaptionActiveTextColor = COLOR_DEFAULT;
    COLORREF BorderActiveColor = COLOR_DEFAULT;

    COLORREF g_TitlebarColor = COLOR_DEFAULT;
    COLORREF g_CaptionColor = COLOR_DEFAULT;
    COLORREF g_BorderColor = COLOR_DEFAULT;

    COLORREF TitlebarInactiveColor = COLOR_DEFAULT;
    COLORREF CaptionInactiveTextColor = COLOR_DEFAULT;
    COLORREF BorderInactiveColor = COLOR_DEFAULT;

    enum BACKGROUNDTYPE
    {
        Default,
        BlurBehind,
        AcrylicSystemBackdrop,
        Mica,
        MicaAlt,
    } BgType = Default;

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


using PUNICODE_STRING = PVOID;

void NewWindowShown(HWND);
void GetProcStrFromPath(std::wstring&);
void GetCurrProcInfo(std::wstring&, DWORD&);
BOOL IsWindowEligible(HWND);
std::wstring GetThemeClass(HTHEME);
void DwmExpandFrameIntoClientAreaHook();
void DwmSetWindowAttributeHook();
void GetThemeColorHook();
HRESULT WINAPI HookedDwmSetWindowAttribute(HWND, DWORD, LPCVOID, DWORD);
HRESULT WINAPI HookedDwmExtendFrameIntoClientArea(HWND, const MARGINS*);
HRESULT WINAPI HookedGetColorTheme(HTHEME, int, int, int, COLORREF*);
HWND WINAPI HookedNtUserCreateWindowEx(DWORD dwExStyle, PUNICODE_STRING UnsafeClassName, LPCWSTR VersionedClass, PUNICODE_STRING UnsafeWindowName, DWORD dwStyle, LONG x, LONG y, LONG nWidth, LONG nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam, DWORD dwShowMode, DWORD dwUnknown1, DWORD dwUnknown2, VOID* qwUnknown3);
void FillBackgroundElements();
void ApplyFrameExtension(HWND);
void EnableBlurBehind(HWND);
void EnableSystemBackdropAcrylic(HWND);
void EnableMica(HWND);
void EnableMicaTabbed(HWND);
void EnableColoredTitlebar(HWND);
void EnableCaptionTextColor(HWND);
void EnableColoredBorder(HWND);
void ApplyForExistingWindows();
BOOL CALLBACK EnumWindowsProc(HWND, LPARAM);
BOOL IsWindowClass(HWND, LPCWSTR);
BOOL GetColorSetting(LPCWSTR, COLORREF&);
void RestoreWindowCustomizations(HWND);
LRESULT CALLBACK SubclassProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);
void LoadSettings();

using NtUserCreateWindowEx_t =
    HWND(WINAPI*)(DWORD dwExStyle,
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
                  VOID* qwUnknown3);
NtUserCreateWindowEx_t NtUserCreateWindowEx_Original;

using DwmExtendFrameIntoClientArea_t = 
    HRESULT(WINAPI*)(HWND hWnd,
                     const MARGINS* pMarInset);
DwmExtendFrameIntoClientArea_t OriginalDwmExtendFrameIntoClientArea = nullptr;

using GetThemeColor_t = 
    HRESULT(WINAPI*)(HTHEME hTheme,
                     int iPartId,
                     int iStateId,
                     int iPropId,
                     COLORREF* pColor);
GetThemeColor_t original_GetThemeColor = nullptr;

using DwmSetWindowAttribute_t = 
HRESULT(WINAPI*)(HWND,
                 DWORD,
                 LPCVOID,
                 DWORD);
DwmSetWindowAttribute_t originalDwmSetWindowAttribute = nullptr;

HRESULT WINAPI HookedDwmSetWindowAttribute(HWND hWnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute)
{
    if(!g_settings.TitlebarFlag)
    {
        if(g_settings.BgType == g_settings.BlurBehind && IsWindowClass(hWnd, L"CabinetWClass") && g_settings.ExtendFrame)
        {
            if(g_settings.CaptionTextFlag && dwAttribute == CAPTION_TEXT_COLOR)
                return originalDwmSetWindowAttribute(hWnd, CAPTION_TEXT_COLOR, &g_settings.g_CaptionColor, sizeof(g_settings.g_CaptionColor));
            
            if(g_settings.BorderFlag && dwAttribute == BORDER_COLOR)
                return originalDwmSetWindowAttribute(hWnd, BORDER_COLOR, &g_settings.g_BorderColor, sizeof(g_settings.g_BorderColor));
                
            return originalDwmSetWindowAttribute(hWnd, SYSTEMBACKDROP_TYPE, &NONE, sizeof(NONE));
        }           
        else if(dwAttribute == SYSTEMBACKDROP_TYPE)
        {
            if(g_settings.BgType == g_settings.AcrylicSystemBackdrop)
                return originalDwmSetWindowAttribute(hWnd, SYSTEMBACKDROP_TYPE, &TRANSIENTWINDOW, sizeof(TRANSIENTWINDOW));
            else if(g_settings.BgType == g_settings.MicaAlt)
                return originalDwmSetWindowAttribute(hWnd, SYSTEMBACKDROP_TYPE, &TABBEDWINDOW, sizeof(TABBEDWINDOW));
            else if(g_settings.BgType == g_settings.Mica)
                return originalDwmSetWindowAttribute(hWnd, SYSTEMBACKDROP_TYPE, &MAINWINDOW, sizeof(MAINWINDOW));
        }
    }
    else if((IsWindowClass(hWnd, L"CabinetWClass")|| IsWindowClass(hWnd, L"TaskManagerWindow")) && (dwAttribute == CAPTION_COLOR || dwAttribute == SYSTEMBACKDROP_TYPE))
            return originalDwmSetWindowAttribute(hWnd, CAPTION_COLOR, &g_settings.g_TitlebarColor, sizeof(g_settings.g_TitlebarColor));
    
    // Effects on VS Studio, Windows Terminal ...
    if(g_settings.BorderFlag && dwAttribute == BORDER_COLOR)
    {
        // Windows classic context menu & taskbar tnumbnail
        if(g_settings.MenuBorderFlag && (IsWindowClass(hWnd, L"#32768") || IsWindowClass(hWnd, L"TaskListThumbnailWnd")))
            return originalDwmSetWindowAttribute(hWnd, BORDER_COLOR, &g_settings.BorderActiveColor, sizeof(g_settings.BorderActiveColor));
        else if(!(IsWindowClass(hWnd, L"#32768") || IsWindowClass(hWnd, L"TaskListThumbnailWnd")))
            return originalDwmSetWindowAttribute(hWnd, BORDER_COLOR, &g_settings.g_BorderColor, sizeof(g_settings.g_BorderColor));
    }
    
    return originalDwmSetWindowAttribute(hWnd, dwAttribute, pvAttribute, cbAttribute);
}

HRESULT WINAPI HookedDwmExtendFrameIntoClientArea(HWND hWnd, const MARGINS* pMarInset)
{   
    if(g_settings.ExtendFrame)
    {
        // Override Win11 Taskmgr, explorer, aerowizard calls
        if(IsWindowClass(hWnd, L"CabinetWClass") || IsWindowClass(hWnd, L"NativeHWNDHost") || IsWindowClass(hWnd, L"TaskManagerWindow"))
        {
            MARGINS margins = {-1, -1, -1, -1};
            return OriginalDwmExtendFrameIntoClientArea(hWnd, &margins);
        }
    }
    return OriginalDwmExtendFrameIntoClientArea(hWnd, pMarInset);
}

HRESULT WINAPI HookedGetColorTheme(HTHEME hTheme, int iPartId, int iStateId, int iPropId, COLORREF *pColor) 
{
    HRESULT hr = original_GetThemeColor(hTheme, iPartId, iStateId, iPropId, pColor);
    
    std::wstring ThemeClassName = GetThemeClass(hTheme);
    // Set the background fill color to black to make the Blur effect transparent
    if (iPropId == TMT_FILLCOLOR) 
    {
        if(((ThemeClassName == L"ItemsView" || ThemeClassName == L"ExplorerStatusBar" || ThemeClassName == L"ExplorerNavPane")
                && (iPartId == 0 && iStateId == 0))
                || (ThemeClassName == L"ReadingPane" && iPartId == 1 && iStateId == 0)
                || (ThemeClassName == L"ProperTree" && iPartId == 2 && iStateId == 0))
        {
            *pColor = RGB(0, 0, 0);
        }
    }
    return hr;
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

void ApplyFrameExtension(HWND hWnd) 
{
    MARGINS margins = {-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(hWnd, &margins);
}

void EnableBlurBehind(HWND hWnd)
{
    // Does not interfere with the Windows Terminal own acrylic
    if(!(IsWindowClass(hWnd, L"CASCADIA_HOSTING_WINDOW_CLASS")))
    {
        typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);

        bb.fEnable = TRUE;
        bb.dwFlags = DWM_BB_ENABLE;

        DwmEnableBlurBehindWindow(hWnd, &bb);

        accent.AccentState = 3;

        attrib.Attrib = WCA_ACCENT_POLICY;
        attrib.pvData = &accent;
        attrib.cbData = sizeof(accent);
        
        auto SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)
            GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");
        if (SetWindowCompositionAttribute) 
            SetWindowCompositionAttribute(hWnd, &attrib);
    }
}

void EnableSystemBackdropAcrylic(HWND hWnd)
{
    DwmSetWindowAttribute(hWnd, USE_IMMERSIVE_DARK_MODE, &ENABLE, sizeof(ENABLE));
    DwmSetWindowAttribute(hWnd, SYSTEMBACKDROP_TYPE , &TRANSIENTWINDOW, sizeof(TRANSIENTWINDOW));
}

void EnableMica(HWND hWnd)
{
    DwmSetWindowAttribute(hWnd, USE_IMMERSIVE_DARK_MODE, &ENABLE, sizeof(ENABLE));
    DwmSetWindowAttribute(hWnd, SYSTEMBACKDROP_TYPE , &MAINWINDOW, sizeof(MAINWINDOW));
}

void EnableMicaTabbed(HWND hWnd)
{
    DwmSetWindowAttribute(hWnd, USE_IMMERSIVE_DARK_MODE, &ENABLE, sizeof(ENABLE));
    DwmSetWindowAttribute(hWnd, SYSTEMBACKDROP_TYPE , &TABBEDWINDOW, sizeof(TABBEDWINDOW));
}

void EnableColoredTitlebar(HWND hWnd)
{
    g_settings.g_TitlebarColor = g_settings.TitlebarActiveColor;
    DwmSetWindowAttribute(hWnd, CAPTION_COLOR, &g_settings.g_TitlebarColor, sizeof(g_settings.g_TitlebarColor));
}

void EnableCaptionTextColor(HWND hWnd)
{
    g_settings.g_CaptionColor = g_settings.CaptionActiveTextColor;
    DwmSetWindowAttribute(hWnd, CAPTION_TEXT_COLOR, &g_settings.g_CaptionColor, sizeof(g_settings.g_CaptionColor));
}

void EnableColoredBorder(HWND hWnd)
{
    g_settings.g_BorderColor = g_settings.BorderActiveColor;
    DwmSetWindowAttribute(hWnd, BORDER_COLOR, &g_settings.g_BorderColor, sizeof(g_settings.g_BorderColor));
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

    return TRUE;
}

BOOL IsWindowClass(HWND hWnd, LPCWSTR ClassName)
{
    WCHAR ClassNameBuffer[256]; 
    GetClassNameW(hWnd, ClassNameBuffer, sizeof(ClassNameBuffer));
    if(!wcscmp(ClassName, ClassNameBuffer))
        return TRUE;
    return FALSE;
}

LRESULT CALLBACK SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData)
{
    switch (uMsg)
    {
        case WM_ACTIVATE:
        {
            bool isMinimized = HIWORD(wParam);
            if (!isMinimized)
            {
                WORD activationState = LOWORD(wParam);

                if ((activationState == WA_ACTIVE || activationState == WA_CLICKACTIVE))
                {
                    if(g_settings.TitlebarFlag)
                    {
                        g_settings.g_TitlebarColor = g_settings.TitlebarActiveColor;
                        DwmSetWindowAttribute(hWnd, CAPTION_COLOR, &g_settings.g_TitlebarColor, sizeof(g_settings.g_TitlebarColor));
                    }
                    if(g_settings.CaptionTextFlag)
                    {
                        g_settings.g_CaptionColor = g_settings.CaptionActiveTextColor;
                        DwmSetWindowAttribute(hWnd, CAPTION_TEXT_COLOR, &g_settings.g_CaptionColor, sizeof(g_settings.g_CaptionColor));
                    }
                    if(g_settings.BorderFlag)
                    {
                        g_settings.g_BorderColor = g_settings.BorderActiveColor;
                        DwmSetWindowAttribute(hWnd, BORDER_COLOR, &g_settings.BorderActiveColor, sizeof(g_settings.BorderActiveColor));
                    }
                }
                else if (activationState == WA_INACTIVE)
                {
                    if(g_settings.TitlebarFlag)
                    {
                        g_settings.g_TitlebarColor = g_settings.TitlebarInactiveColor;
                        DwmSetWindowAttribute(hWnd, CAPTION_COLOR, &g_settings.g_TitlebarColor, sizeof(g_settings.g_TitlebarColor));
                    }
                    if(g_settings.CaptionTextFlag)
                    {
                        g_settings.g_CaptionColor = g_settings.CaptionInactiveTextColor;
                        DwmSetWindowAttribute(hWnd, CAPTION_TEXT_COLOR, &g_settings.g_CaptionColor, sizeof(g_settings.g_CaptionColor));
                    }
                    if(g_settings.BorderFlag)
                    {
                        g_settings.g_BorderColor = g_settings.BorderInactiveColor;
                        DwmSetWindowAttribute(hWnd, BORDER_COLOR, &g_settings.BorderInactiveColor, sizeof(g_settings.BorderInactiveColor));
                    }
                }
            }
            break;
        }
        case WM_NCDESTROY:
        {
            std::lock_guard<std::mutex> guard(g_subclassedWindowsMutex);
            g_subclassedWindows.erase(hWnd);
            break;
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void NewWindowShown(HWND hWnd) 
{
    if(!IsWindowEligible(hWnd))
        return;

    if(g_settings.ExtendFrame)
        ApplyFrameExtension(hWnd);

    if(g_settings.CaptionTextFlag)
        EnableCaptionTextColor(hWnd);

    if(g_settings.BorderFlag)
        EnableColoredBorder(hWnd); 

    if(!g_settings.TitlebarFlag)
    {
        if(g_settings.BgType == g_settings.BlurBehind && g_settings.ExtendFrame)
            EnableBlurBehind(hWnd);
        else if(g_settings.BgType == g_settings.AcrylicSystemBackdrop)
            EnableSystemBackdropAcrylic(hWnd);
        else if(g_settings.BgType == g_settings.Mica)
            EnableMica(hWnd);
        else if(g_settings.BgType == g_settings.MicaAlt)
            EnableMicaTabbed(hWnd);
    }
    else
        EnableColoredTitlebar(hWnd);
    
    if(g_settings.BorderFlag || g_settings.CaptionTextFlag || g_settings.TitlebarFlag)
        if(WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, SubclassProc, 0))
        {
            std::lock_guard<std::mutex> guard(g_subclassedWindowsMutex);
            g_subclassedWindows.insert(hWnd);
        }
}

void DwmExpandFrameIntoClientAreaHook()
{
    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandle(L"dwmapi.dll"), "DwmExtendFrameIntoClientArea"),
                       (void*)HookedDwmExtendFrameIntoClientArea,
                       (void**)&OriginalDwmExtendFrameIntoClientArea);
}

void GetThemeColorHook()
{
    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandle(L"uxtheme.dll"), "GetThemeColor"),
                       (void*)HookedGetColorTheme,
                       (void**)&original_GetThemeColor);   
}

void DwmSetWindowAttributeHook()
{
    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandle(L"dwmapi.dll"), "DwmSetWindowAttribute"),
                       (void*)HookedDwmSetWindowAttribute,
                       (void**)&originalDwmSetWindowAttribute); 
}

void FillBackgroundElements()
{
    GetThemeColorHook();
    /*TODO*/
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

void RestoreWindowCustomizations(HWND hWnd)
{
    // Manually restore frame extension
    if(!(IsWindowClass(hWnd,  L"TaskManagerWindow")))
    {
        MARGINS margins = { 0, 0, 0, 0 };
        DwmExtendFrameIntoClientArea(hWnd, &margins);
    }

    g_settings.BorderActiveColor = COLOR_DEFAULT;
    DwmSetWindowAttribute(hWnd, BORDER_COLOR , &g_settings.BorderActiveColor, sizeof(g_settings.BorderActiveColor));
    
    g_settings.TitlebarActiveColor = COLOR_DEFAULT;
    DwmSetWindowAttribute(hWnd, CAPTION_COLOR , &g_settings.TitlebarActiveColor, sizeof(g_settings.TitlebarActiveColor));

    g_settings.CaptionActiveTextColor = COLOR_DEFAULT;
    DwmSetWindowAttribute(hWnd, CAPTION_TEXT_COLOR , &g_settings.CaptionActiveTextColor, sizeof(g_settings.CaptionActiveTextColor));
}

BOOL GetColorSetting(LPCWSTR hexColor, COLORREF& outColor) 
{
    if (!hexColor)
        return FALSE;

    if (hexColor[0] == L'0' && hexColor[1] == L'\0') {
        outColor = COLOR_NONE;
        return TRUE;
    }
    else if (hexColor[0] == L'1' && hexColor[1] == L'\0') {
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
        outColor = COLOR_DEFAULT;
        return FALSE;
    }

    if (wcslen(hexColor) != 6)
        return FALSE;
    
    auto hexToByte = [](WCHAR c) -> int {
        if (c >= L'0' && c <= L'9') return c - L'0';
        if (c >= L'A' && c <= L'F') return 10 + (c - L'A');
        if (c >= L'a' && c <= L'f') return 10 + (c - L'a');
        return -1;
    };

    BYTE rgb[3];
    for (int i = 0; i < 3; ++i) {
        int high = hexToByte(hexColor[i * 2]);
        int low  = hexToByte(hexColor[i * 2 + 1]);
        if (high < 0 || low < 0)
            return FALSE;
        rgb[i] = (high << 4) | low;
    }

    outColor = RGB(rgb[0], rgb[1], rgb[2]);
    return TRUE;
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
    
    LPCWSTR pszStyle = Wh_GetStringSetting(L"type");
    if (0 == wcscmp(pszStyle, L"acrylicblur"))
        g_settings.BgType = g_settings.BlurBehind;
    else if (0 == wcscmp(pszStyle, L"acrylicsystem"))
        g_settings.BgType = g_settings.AcrylicSystemBackdrop;
    else if (0 == wcscmp(pszStyle, L"mica"))
        g_settings.BgType = g_settings.Mica;
    else if (0 == wcscmp(pszStyle, L"mica_tabbed"))
        g_settings.BgType = g_settings.MicaAlt;
    else 
        g_settings.BgType = g_settings.Default;
    
    g_settings.ExtendFrame = Wh_GetIntSetting(L"ExtendFrame");
    if(g_settings.ExtendFrame)
        DwmExpandFrameIntoClientAreaHook();

    DwmSetWindowAttributeHook();

    g_settings.TitlebarFlag = Wh_GetIntSetting(L"TitlebarColor.ColorTitlebar");
    if(g_settings.TitlebarFlag)
    {
        LPCWSTR pszTitlebarStyle = Wh_GetStringSetting(L"TitlebarColor.titlerbarstyles_active");
        g_settings.TitlebarFlag = GetColorSetting(pszTitlebarStyle, g_settings.TitlebarActiveColor);
        pszTitlebarStyle = Wh_GetStringSetting(L"TitlebarColor.titlerbarstyles_inactive");
        g_settings.TitlebarFlag = GetColorSetting(pszTitlebarStyle, g_settings.TitlebarInactiveColor);
        Wh_FreeStringSetting(pszTitlebarStyle);
    }

    g_settings.CaptionTextFlag = Wh_GetIntSetting(L"TitlebarTextColor.ColorTitlebarText");
    if(g_settings.CaptionTextFlag)
    {
        LPCWSTR pszTitlebarTextColorStyle = Wh_GetStringSetting(L"TitlebarTextColor.titlerbarcolorstyles_active");
        g_settings.CaptionTextFlag = GetColorSetting(pszTitlebarTextColorStyle, g_settings.CaptionActiveTextColor);
        pszTitlebarTextColorStyle = Wh_GetStringSetting(L"TitlebarTextColor.titlerbarcolorstyles_inactive");
        g_settings.CaptionTextFlag = GetColorSetting(pszTitlebarTextColorStyle, g_settings.CaptionInactiveTextColor);
        Wh_FreeStringSetting(pszTitlebarTextColorStyle);
    }

    g_settings.BorderFlag = Wh_GetIntSetting(L"BorderColor.ColorBorder");
    if(g_settings.BorderFlag)
    {
        LPCWSTR pszBorderStyle = Wh_GetStringSetting(L"BorderColor.borderstyles_active");
        g_settings.BorderFlag = GetColorSetting(pszBorderStyle, g_settings.BorderActiveColor);
        pszBorderStyle = Wh_GetStringSetting(L"BorderColor.borderstyles_inactive");
        g_settings.BorderFlag = GetColorSetting(pszBorderStyle, g_settings.BorderInactiveColor);
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
                LPCWSTR pszStyle = Wh_GetStringSetting(L"RuledPrograms[%d].type", i);
                if (0 == wcscmp(pszStyle, L"acrylicblur"))
                    g_settings.BgType = g_settings.BlurBehind;
                else if (0 == wcscmp(pszStyle, L"acrylicsystem"))
                    g_settings.BgType = g_settings.AcrylicSystemBackdrop;
                else if (0 == wcscmp(pszStyle, L"mica"))
                    g_settings.BgType = g_settings.Mica;
                else if (0 == wcscmp(pszStyle, L"mica_tabbed"))
                    g_settings.BgType = g_settings.MicaAlt;
                else 
                    g_settings.BgType = g_settings.Default;
                
                Wh_FreeStringSetting(pszStyle);

                g_settings.ExtendFrame = Wh_GetIntSetting(L"RuledPrograms[%d].ExtendFrame", i);
                if(g_settings.ExtendFrame)
                    DwmExpandFrameIntoClientAreaHook();

                g_settings.TitlebarFlag = Wh_GetIntSetting(L"RuledPrograms[%d].TitlebarColor.ColorTitlebar", i);
                if(g_settings.TitlebarFlag)
                {
                    LPCWSTR pszTitlebarStyle = Wh_GetStringSetting(L"RuledPrograms[%d].TitlebarColor.Titlerbarstyles_active", i);
                    g_settings.TitlebarFlag = GetColorSetting(pszTitlebarStyle, g_settings.TitlebarActiveColor);
                    pszTitlebarStyle = Wh_GetStringSetting(L"RuledPrograms[%d].TitlebarColor.Titlerbarstyles_inactive", i);
                    g_settings.TitlebarFlag = GetColorSetting(pszTitlebarStyle, g_settings.TitlebarInactiveColor);
                    Wh_FreeStringSetting(pszTitlebarStyle);
                }

                g_settings.CaptionTextFlag = Wh_GetIntSetting(L"RuledPrograms[%d].TitlebarTextColor.ColorTitlebarText", i);
                if(g_settings.CaptionTextFlag)
                {
                    LPCWSTR pszTitlebarTextColorStyle = Wh_GetStringSetting(L"RuledPrograms[%d].TitlebarTextColor.titlerbarcolorstyles_active", i);
                    g_settings.CaptionTextFlag = GetColorSetting(pszTitlebarTextColorStyle, g_settings.CaptionActiveTextColor);
                    pszTitlebarTextColorStyle = Wh_GetStringSetting(L"RuledPrograms[%d].TitlebarTextColor.titlerbarcolorstyles_inactive", i);
                    g_settings.CaptionTextFlag = GetColorSetting(pszTitlebarTextColorStyle, g_settings.CaptionInactiveTextColor);
                    Wh_FreeStringSetting(pszTitlebarTextColorStyle);
                }

                g_settings.BorderFlag = Wh_GetIntSetting(L"RuledPrograms[%d].BorderColor.ColorBorder", i);
                if(g_settings.BorderFlag)
                {
                    LPCWSTR pszBorderStyle = Wh_GetStringSetting(L"RuledPrograms[%d].BorderColor.borderstyles_active", i);
                    g_settings.BorderFlag = GetColorSetting(pszBorderStyle, g_settings.BorderActiveColor);
                    pszBorderStyle = Wh_GetStringSetting(L"RuledPrograms[%d].BorderColor.borderstyles_inactive", i);
                    g_settings.BorderFlag = GetColorSetting(pszBorderStyle, g_settings.BorderInactiveColor);
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
    ApplyForExistingWindows();

    std::unordered_set<HWND> subclassedWindows;
    {
        std::lock_guard<std::mutex> guard(g_subclassedWindowsMutex);
        subclassedWindows = std::move(g_subclassedWindows);
        g_subclassedWindows.clear();
    }

    for (HWND hWnd : subclassedWindows)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, SubclassProc);
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) 
{
    Wh_Log(L"SettingsChanged");
    *bReload = TRUE;
    return TRUE;
}
