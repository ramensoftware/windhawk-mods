// ==WindhawkMod==
// @id              translucent-windows
// @name            Translucent Windows
// @description     Enables native translucent effects on windows
// @version         1.0
// @author          Undisputed00x
// @github          https://github.com/Undisputed00x
// @include         *
// @compilerOptions -ldwmapi -luxtheme
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
    
    ⚠ Be aware when disabling this option with Acrylic BlurBehind effects enabled, the affected windows will need to be reopened to see the change.
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>
#include <vssym32.h>
#include <uxtheme.h>
#include <string>

struct{
    BOOL FillBg = FALSE;
    BOOL ExtendFrame = FALSE;
    BOOL Unload = FALSE;
} g_settings;

enum BACKGROUNDTYPE
{
    Default,
    BlurBehind,
    AcrylicSystemBackdrop,
    Mica,
    MicaAlt,
} g_BgType = Default;

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

static const UINT USE_IMMERSIVE_DARK_MODE = 20; // DWMWA_USE_IMMERSIVE_DARK_MODE
static const UINT ENABLE = 1;
static const UINT SYSTEMBACKDROP_TYPE = 38; // DWM_SYSTEMBACKDROP_TYPE
static const UINT NONE = 1; // DWMSBT_NONE
static const UINT MAINWINDOW = 2; // DWMSBT_MAINWINDOW
static const UINT TRANSIENTWINDOW = 3; // DWMSBT_TRANSIENTWINDOW
static const UINT TABBEDWINDOW = 4; // DWMSBT_TABBEDWINDOW

using PUNICODE_STRING = PVOID;

void NewWindowShown(HWND);
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
void ApplyForExistingWindows();
BOOL CALLBACK EnumWindowsProc(HWND, LPARAM);
BOOL IsWindowClass(HWND, LPCWSTR);
void RestoreWindowCustomizations(HWND);
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
    // Override Win11 File Explorer, W11 Task Manager and W11 Snipping Tool calls
    if(g_BgType == BlurBehind && (IsWindowClass(hWnd, L"CabinetWClass")))
        return originalDwmSetWindowAttribute(hWnd, SYSTEMBACKDROP_TYPE, &NONE, sizeof(NONE));
    else if((IsWindowClass(hWnd, L"TaskManagerWindow") || IsWindowClass(hWnd, L"XamlWindow")) && dwAttribute == SYSTEMBACKDROP_TYPE)
    {
        if(g_BgType == AcrylicSystemBackdrop)
            return originalDwmSetWindowAttribute(hWnd, SYSTEMBACKDROP_TYPE, &TRANSIENTWINDOW, sizeof(TRANSIENTWINDOW));
        else if(g_BgType == MicaAlt)
            return originalDwmSetWindowAttribute(hWnd, SYSTEMBACKDROP_TYPE, &TABBEDWINDOW, sizeof(TABBEDWINDOW));
        else if(g_BgType == Mica)
            return originalDwmSetWindowAttribute(hWnd, SYSTEMBACKDROP_TYPE, &MAINWINDOW, sizeof(MAINWINDOW));
    }
    return originalDwmSetWindowAttribute(hWnd, dwAttribute, pvAttribute, cbAttribute);
}

HRESULT WINAPI HookedDwmExtendFrameIntoClientArea(HWND hWnd, const MARGINS* pMarInset)
{
    // Override Win11 Taskmgr, explorer, aerowizard calls
    if(IsWindowClass(hWnd, L"CabinetWClass") || IsWindowClass(hWnd, L"NativeHWNDHost") || IsWindowClass(hWnd, L"TaskManagerWindow"))
    {
        MARGINS margins = {-1, -1, -1, -1};
        return OriginalDwmExtendFrameIntoClientArea(hWnd, &margins);
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

void NewWindowShown(HWND hWnd) 
{
    if(g_BgType == BlurBehind && IsWindowEligible(hWnd) && g_settings.ExtendFrame)
    {
        ApplyFrameExtension(hWnd);
        EnableBlurBehind(hWnd);
    }
    if(g_BgType == AcrylicSystemBackdrop && IsWindowEligible(hWnd))
    {
        if(g_settings.ExtendFrame)  
            ApplyFrameExtension(hWnd);
        EnableSystemBackdropAcrylic(hWnd);
    }
    if(g_BgType == Mica && IsWindowEligible(hWnd))
    {
        if(g_settings.ExtendFrame)  
            ApplyFrameExtension(hWnd);
        EnableMica(hWnd);
    }
    else if(g_BgType == MicaAlt && IsWindowEligible(hWnd))
    {
        if(g_settings.ExtendFrame)  
            ApplyFrameExtension(hWnd);
        EnableMicaTabbed(hWnd);
    }
    else if(IsWindowEligible(hWnd) && g_settings.ExtendFrame)
        ApplyFrameExtension(hWnd);
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
    // Restore Frame Extension
    if(!(IsWindowClass(hWnd,  L"TaskManagerWindow")))
    {
        MARGINS margins = { 0, 0, 0, 0 };
        DwmExtendFrameIntoClientArea(hWnd, &margins);
    }
}

void LoadSettings(void)
{
    g_settings.FillBg = Wh_GetIntSetting(L"ThemeBackground");
    if(g_settings.FillBg)
        FillBackgroundElements();
    
    LPCWSTR pszStyle = Wh_GetStringSetting(L"type");
    if (0 == wcscmp(pszStyle, L"acrylicblur"))
        g_BgType = BlurBehind;
    else if (0 == wcscmp(pszStyle, L"acrylicsystem"))
        g_BgType = AcrylicSystemBackdrop;
    else if (0 == wcscmp(pszStyle, L"mica"))
        g_BgType = Mica;
    else if (0 == wcscmp(pszStyle, L"mica_tabbed"))
        g_BgType = MicaAlt;
    else 
        g_BgType = Default;
    
    g_settings.ExtendFrame = Wh_GetIntSetting(L"ExtendFrame");
    if(g_settings.ExtendFrame)
    {
        DwmExpandFrameIntoClientAreaHook();
        DwmSetWindowAttributeHook();
    }
    else if(g_BgType != BlurBehind)
        DwmSetWindowAttributeHook();
    
    Wh_FreeStringSetting(pszStyle);
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
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) 
{
    Wh_Log(L"SettingsChanged");
    *bReload = TRUE;
    return TRUE;
}
