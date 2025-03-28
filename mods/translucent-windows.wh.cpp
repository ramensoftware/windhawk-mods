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
![BlurBehind](https://imgur.com/a/8dnPOCt)
### Acrylic (SystemBackdrop)
![Acrylic SystemBackdrop](https://imgur.com/a/KZTzP3Q)
### Mica
![Mica](https://imgur.com/a/TJ5pP3m)
### MicaAlt
![MicaTabbed](https://imgur.com/a/EaeW86Z)

# FAQ

* Prerequisited windows settings to enable the background effects
    - Transparency effects enabled
    - Energy saver disabled
#
* Extending effects to the entire window can result in text being unreadable or even invisible in some cases. 
Light mode theme, HDR enabled or white background behind the window can cause of this problem. 
This is because some GDI rendering operations do not preserve alpha channel values.

* Acrylic blur behind effect may show a bleeding effect at the edges of a window when 
maximized or snapped to the edge of the screen, this is caused by default by the windows DWMAPI.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ThemeBackground: TRUE
  $name: Optimize windows theme
  $description: Fill with black color the file explorer background in order to render a clear translucent effect
- type: Default
  $name: Effects
  $description: Windows 11 version >= 22621.xxx (22H2) is required
  $options:
  - none: Default
  - acrylicblur: Acrylic (BlurBehind)
  - acrylicsystem: Acrylic (SystemBackdrop)
  - mica: Mica
  - mica_tabbed: MicaAlt
- ExtendFrame: TRUE
  $name: Extend effects into entire window
  $description: Extends the effects into the entire window background using DwmExtendFrameIntoClientArea.
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>
#include <vssym32.h>
#include <uxtheme.h>
#include <string>

struct{
    BOOL FillBg = TRUE;
    BOOL ExtendFrame = TRUE;
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

void NewWindowShown(HWND);
BOOL IsWindowEligible(HWND);
std::wstring GetThemeClass(HTHEME);
void DwmExpandFrameIntoClientAreaHook();
void DwmSetWindowAttributeHook();
void GetThemeColorHook();
HRESULT WINAPI HookedDwmSetWindowAttribute(HWND, DWORD, LPCVOID, DWORD);
HRESULT WINAPI HookedDwmExtendFrameIntoClientArea(HWND, const MARGINS*);
HRESULT WINAPI HookedGetColorTheme(HTHEME, int, int, int, COLORREF*);
void FillBackgroundElements();
void ApplyFrameExtension(HWND);
void EnableBlurBehind(HWND);
void EnableSystemBackdropAcrylic(HWND);
void EnableMica(HWND);
void EnableMicaTabbed(HWND);
void ApplyForExistingWindows();
BOOL CALLBACK EnumWindowsProc(HWND, LPARAM);
void LoadSettings();

using PUNICODE_STRING = PVOID;

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

using RegQueryValueExW_t = LSTATUS (WINAPI*)(HKEY hKey,
                                          LPCWSTR lpValueName,
                                          LPDWORD lpReserved,
                                          LPDWORD lpType,
                                          LPBYTE lpData,
                                          LPDWORD lpcbData);
RegQueryValueExW_t OriginalRegQueryValueExW = nullptr;

HRESULT WINAPI HookedDwmSetWindowAttribute(HWND hWnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute)
{
    CHAR ExplorerClassName[256];
    GetClassNameA(hWnd, ExplorerClassName, sizeof(ExplorerClassName));
    CHAR TaskmgrClassName[256];
    GetClassNameA(hWnd, TaskmgrClassName, sizeof(TaskmgrClassName));

    //Override Win11, TaskMgr explorer calls
    if(g_BgType == BlurBehind && ((!strcmp(ExplorerClassName, "CabinetWClass"))))
    {
        DWM_SYSTEMBACKDROP_TYPE backdrop = DWMSBT_NONE;
        return originalDwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop));
    }
    else if(g_BgType == AcrylicSystemBackdrop && (!strcmp(TaskmgrClassName , "TaskManagerWindow")))
    {
        // Apply AcrylicSystemBackdrop to TaskMgr only when Windows tries to render default Mica
        if(dwAttribute == DWMWA_SYSTEMBACKDROP_TYPE)
        {
            DWM_SYSTEMBACKDROP_TYPE backdrop = DWMSBT_TRANSIENTWINDOW;
            return originalDwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop));
        }
    }
    return originalDwmSetWindowAttribute(hWnd, dwAttribute, pvAttribute, cbAttribute);
}

HRESULT WINAPI HookedDwmExtendFrameIntoClientArea(HWND hWnd, const MARGINS* pMarInset)
{
    CHAR ExplorerClassName[256];
    GetClassNameA(hWnd, ExplorerClassName, sizeof(ExplorerClassName));
    CHAR TaskmgrClassName[256];
    GetClassNameA(hWnd, TaskmgrClassName, sizeof(TaskmgrClassName));
    CHAR AeroWizardClassName[256];
    GetClassNameA(hWnd, AeroWizardClassName, sizeof(AeroWizardClassName));

    //Override Win11 Taskmgr, explorer calls
    if((!strcmp(ExplorerClassName, "CabinetWClass")) || (!strcmp(AeroWizardClassName , "NativeHWNDHost")) || (!strcmp(TaskmgrClassName , "TaskManagerWindow")))
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
    //Set the background fill color to black for the API to make the Blur effect transparent
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

/*https://github.com/Maplespe/ExplorerBlurMica/blob/79c0ef4d017e32890e107ff98113507f831608b6/ExplorerBlurMica/Helper.cpp#L126*/
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
    char TerminalClassName[256];
    GetClassNameA(hWnd, TerminalClassName, sizeof(TerminalClassName));
    // Bypass Windows Terminal as it glitches its own acrylic material
    if(strcmp(TerminalClassName, "CASCADIA_HOSTING_WINDOW_CLASS"))
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
    UINT type = 1;
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &type, sizeof(type));
    DWM_SYSTEMBACKDROP_TYPE backdrop = DWMSBT_TRANSIENTWINDOW;
    DwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE , &backdrop, sizeof(backdrop));
}

void EnableMica(HWND hWnd)
{
    UINT type = 1;
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &type, sizeof(type));
    DWM_SYSTEMBACKDROP_TYPE backdrop = DWMSBT_MAINWINDOW;
    DwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE , &backdrop, sizeof(backdrop));
}

void EnableMicaTabbed(HWND hWnd)
{
    UINT type = 1;
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &type, sizeof(type));
    DWM_SYSTEMBACKDROP_TYPE backdrop = DWMSBT_TABBEDWINDOW;
    DwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE , &backdrop, sizeof(backdrop));
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

void NewWindowShown(HWND hWnd) 
{
    //BlurBehind requires FrameExtension  
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
        else
            NewWindowShown(hWnd);
    }
    return TRUE;
}

void ApplyForExistingWindows()
{
    EnumWindows(EnumWindowsProc, 0);
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

BOOL Wh_ModAfterInit() 
{
    #ifdef _WIN64
        const size_t OFFSET_SAME_TEB_FLAGS = 0x17EE;
    #else
        const size_t OFFSET_SAME_TEB_FLAGS = 0x0FCA;
    #endif
    bool isInitialThread = *(USHORT*)((BYTE*)NtCurrentTeb() + OFFSET_SAME_TEB_FLAGS) & 0x0400;
    if (!isInitialThread)
        ApplyForExistingWindows();
    return TRUE;
}

void Wh_ModUninit(void) 
{

}

BOOL Wh_ModSettingsChanged(BOOL* bReload) 
{
    Wh_Log(L"SettingsChanged");
    LoadSettings();
    *bReload = TRUE;
    return TRUE;
}