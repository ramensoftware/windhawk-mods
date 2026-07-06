// ==WindhawkMod==
// @id              disk-usage-bar-customizer
// @name            Disk Usage Bar Customizer
// @description     Customize everything about the disk usage bar from the This PC section in the File Explorer, including theme-aware colors, height, border and more.
// @version         1.0.0
// @author          Valer100
// @github          https://github.com/Valer100
// @include         explorer.exe
// @compilerOptions -luxtheme -lgdi32
// ==/WindhawkMod==


// ==WindhawkModReadme==
/*
# Disk Usage Bar Customizer
Customize everything about the disk usage bar from the This PC section in the File Explorer, including theme-aware colors, height, border and more.

This is a fork of the original [Disk Usage Bar Color](https://windhawk.net/mods/disk-usage-bar-color) mod made by [dirtyrazkl](https://github.com/dirtyrazkl).


## Customization options
### General
- Show remaining space as progress instead of used space
- Custom warning percentage threshold

### Rendering
- Render using visual styles
- Render using dark mode parts when using visual styles

### Custom rendering
- Use system's accent color for the normal progress color
- Render bar border
- Height factor
- Custom light & dark mode colors


## Screenshots
### Colors adapting to the system's theme
![System colors light mode](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/light_mode_default.png)

![System colors dark mode](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/dark_mode_default.png)

### Accent color as the normal progress color
![Accent color light mode](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/light_mode_accent.png)

![Accent color dark mode](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/dark_mode_accent.png)

### Custom colors
![Custom colors](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/custom_colors.png)

### No border
![No border](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/custom_colors_no_border.png)

### Custom height
![Custom height](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/custom_height.png)

### Custom warning threshold
![Custom warning threshold](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/custom_warning_threshold.png)

### Show remaining space as progress instead of used space
![Show remaining space as progress instead of used space](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/remaining_space_as_progress.png)
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- general:
  - remainingSpaceAsProgress: false
    $name: Show remaining space as progress instead of used space

  - warningPercentageThreshold: 90
    $name: Warning percentage threshold
    $description: >-
      Use the warning progress color when the warning percentage threshold (in percents) is reached (default: 90%).

  $name: General


- rendering:
  - renderUsingVisualStyles: false
    $name: Render using visual styles
    $description: >-
      Render the usage bar using the parts provided by the theme to match the system's appearance. You won't be able to customize the bar rendering if this option is enabled.

  - darkModeVSRendering: true
    $name: Render using dark mode parts when using visual styles
    $description: >-
      Render the usage bar using the dark mode parts from the "DarkMode_CopyEngine::Progress" class when dark mode is enabled. You must have Windows 11 build 26200.6899 or higher installed and the "Render using visual styles" option enabled for this to work.

  $name: Rendering


- customRendering:
  - useSystemAccentColor: false
    $name: Use system's accent color for the normal progress color
    $description: >-
      Use system's accent color for the normal progress color.

  - renderBarBorder: true
    $name: Render bar border

  - heightFactor: 100
    $name: Height factor
    $description: >-
      A factor that determines the height of the usage bar (in percents; default: 100%). The factor cannot be greater than 100%.


  - lightModeColors:
    - barColor: "#E6E6E6"
      $name: Bar color
      $description: >-
        Hex color code for the empty part of the bar (default: #E6E6E6).
      
    - barBorderColor: "#BCBCBC"
      $name: Bar border color
      $description: >-
        Hex color code for the bar's border (default: #BCBCBC). Not appliable when "Render bar border" is disabled.
      
    - progressColorNormal: "#0070CB"
      $name: Normal progress color
      $description: >-
        Hex color code for the normal disk usage bar progress (default: #0070CB). Not appliable when "Use system's accent color for the normal progress color" is enabled.
      
    - progressColorFull: "#C42B1C"
      $name: Full/Warning progress color
      $description: >-
        Hex color code for the bar progress when a drive is nearly full (default: #C42B1C).
  
    $name: Light mode colors
  
  
  - darkModeColors:
    - barColor: "#383838"
      $name: Bar color
      $description: >-
        Hex color code for the empty part of the bar (default: #383838).
      
    - barBorderColor: "#646464"
      $name: Bar border color
      $description: >-
        Hex color code for the bar's border (default: #646464). Not appliable when "Render bar border" is disabled.
      
    - progressColorNormal: "#60CDFF"
      $name: Normal progress color
      $description: >-
        Hex color code for the normal disk usage bar progress (default: #60CDFF). Not appliable when "Use system's accent color for the normal progress color" is enabled.
      
    - progressColorFull: "#FF3D53"
      $name: Full/Warning progress color
      $description: >-
        Hex color code for the bar progress when a drive is nearly full (default: #FF3D53).
  
    $name: Dark mode colors

  $name: Custom rendering
  $description: >-
    These options will be ignored when the "Render using visual styles" option is enabled.
*/
// ==/WindhawkModSettings==


#include <windhawk_utils.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <versionhelpers.h>

// Undocumented functions
using fnGetThemeClass                      = HRESULT(WINAPI*)(HTHEME, LPWSTR, INT);
using fnGetImmersiveColorFromColorSetEx    = DWORD (WINAPI *)(DWORD, DWORD, BOOL, DWORD);
using fnGetImmersiveUserColorSetPreference = DWORD (WINAPI *)(BOOL, BOOL);

fnGetThemeClass GetThemeClass = nullptr;
fnGetImmersiveColorFromColorSetEx GetImmersiveColorFromColorSetEx = nullptr;
fnGetImmersiveUserColorSetPreference GetImmersiveUserColorSetPreference = nullptr;


static decltype(&DrawThemeBackground) DrawThemeBackground_orig = nullptr;


// General
static BOOL     g_remainingSpaceAsProgress = FALSE;
static INT      g_warningThreshold         = 90;

// Rendering
static BOOL     g_renderUsingVisualStyles  = FALSE;
static BOOL     g_darkModeVSRendering      = TRUE;

// Custom rendering
static BOOL     g_useSystemAccentColor     = FALSE;
static BOOL     g_renderBarBorder          = TRUE;
static INT      g_heightFactor             = 100;

// Light mode colors
static COLORREF g_barColorLight            = 0x00E6E6E6;
static COLORREF g_barBorderColorLight      = 0x00BCBCBC;
static COLORREF g_progressColorNormalLight = 0x00CB7000;
static COLORREF g_progressColorFullLight   = 0x001C2BC4;

// Dark mode colors
static COLORREF g_barColorDark             = 0x00383838;
static COLORREF g_barBorderColorDark       = 0x00646464;
static COLORREF g_progressColorNormalDark  = 0x00FFCD60;
static COLORREF g_progressColorFullDark    = 0x00533DFF;

// Other
thread_local int g_barWidth = 1;
HTHEME g_darkHTheme = nullptr;
HMODULE g_uxtheme = nullptr;
HMODULE g_shell32 = nullptr;


static COLORREF LoadColorSetting(PCWSTR colorName, COLORREF fallback) {
    PCWSTR originalHexString = Wh_GetStringSetting(colorName);
    PCWSTR hexString = originalHexString;

    if (hexString[0] == L'#') hexString++;

    if (wcslen(hexString) != 6) {
        Wh_FreeStringSetting(originalHexString);
        return fallback;
    }

    auto h = [](wchar_t c) -> int {
        if (c >= L'0' && c <= L'9') return c - L'0';
        if (c >= L'A' && c <= L'F') return c - L'A' + 10;
        if (c >= L'a' && c <= L'f') return c - L'a' + 10;
        return -1;
    };

    int h0 = h(hexString[0]); 
    int h1 = h(hexString[1]);
    int h2 = h(hexString[2]);
    int h3 = h(hexString[3]);
    int h4 = h(hexString[4]);
    int h5 = h(hexString[5]);

    if (h0 < 0 || h1 < 0 || h2 < 0 || h3 < 0 || h4 < 0 || h5 < 0) {
        Wh_FreeStringSetting(originalHexString);
        return fallback;
    }

    Wh_FreeStringSetting(originalHexString);
    return RGB((h0 << 4) | h1, (h2 << 4) | h3, (h4 << 4) | h5);
}


static void LoadSettings() {
    // General
    g_remainingSpaceAsProgress = Wh_GetIntSetting(L"general.remainingSpaceAsProgress");
    g_warningThreshold         = Wh_GetIntSetting(L"general.warningPercentageThreshold");

    // Rendering
    g_renderUsingVisualStyles  = Wh_GetIntSetting(L"rendering.renderUsingVisualStyles");
    g_darkModeVSRendering      = Wh_GetIntSetting(L"rendering.darkModeVSRendering");

    // Custom rendering
    g_useSystemAccentColor     = Wh_GetIntSetting(L"customRendering.useSystemAccentColor");
    g_renderBarBorder          = Wh_GetIntSetting(L"customRendering.renderBarBorder");
    g_heightFactor             = Wh_GetIntSetting(L"customRendering.heightFactor");

    if (g_heightFactor > 100) g_heightFactor = 100;
    else if (g_heightFactor < 0) g_heightFactor = 0;

    // Light mode colors
    g_barColorLight            = LoadColorSetting(L"customRendering.lightModeColors.barColor",            0x00E6E6E6);
    g_barBorderColorLight      = LoadColorSetting(L"customRendering.lightModeColors.barBorderColor",      0x00BCBCBC);
    g_progressColorNormalLight = LoadColorSetting(L"customRendering.lightModeColors.progressColorNormal", 0x00CB7000);
    g_progressColorFullLight   = LoadColorSetting(L"customRendering.lightModeColors.progressColorFull",   0x001C2BC4);

    // Dark mode colors
    g_barColorDark            = LoadColorSetting(L"customRendering.darkModeColors.barColor",              0x00383838);
    g_barBorderColorDark      = LoadColorSetting(L"customRendering.darkModeColors.barBorderColor",        0x00646464);
    g_progressColorNormalDark = LoadColorSetting(L"customRendering.darkModeColors.progressColorNormal",   0x00FFCD60);
    g_progressColorFullDark   = LoadColorSetting(L"customRendering.darkModeColors.progressColorFull",     0x00533DFF);
}


static bool AreAppsUsingDarkTheme() {
    DWORD value = 1;
    DWORD size = sizeof(value);

    LONG result = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme",
        RRF_RT_REG_DWORD, nullptr, &value, &size
    );

    return result == ERROR_SUCCESS && !value;
}


static COLORREF GetSystemAccentColorShade(int shade) {
    if (GetImmersiveColorFromColorSetEx && GetImmersiveUserColorSetPreference)
        return GetImmersiveColorFromColorSetEx(
            GetImmersiveUserColorSetPreference(FALSE, FALSE), shade, FALSE, 0
        );
    else
        return 0xFFFF00FF;
}


HRESULT WINAPI HookedDrawThemeBackground(
    HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect, LPCRECT pClipRect
) {
    // I know the rect left point and shell32 caller checks are some really 
    // weird checks, but I have no idea for a better check that actually works 
    // and that can actually distinguish from Explorer's progress bar drawing 
    // inside an item from This PC's drive list and an actual progress bar 
    // control drawing. From my inspection, Explorer seems to custom draw a 
    // progress bar like this only inside the drive list from the This PC section.

    if ((iPartId != PP_FILL && iPartId != PP_TRANSPARENTBAR) || !pRect || pRect->left <= 0)
        return DrawThemeBackground_orig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);

    WCHAR themeClass[256] = {};

    BOOL isThemeClassValid = (
        GetThemeClass && SUCCEEDED(GetThemeClass(hTheme, themeClass, 256)) 
        && wcscmp(themeClass, L"Progress") == 0
    );

    if (isThemeClassValid) {
        HMODULE callerModule = nullptr;
        void* caller = __builtin_return_address(0);

        BOOL isCallerShell32 = GetModuleHandleEx(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, 
            reinterpret_cast<LPCWSTR>(caller), &callerModule
        ) && callerModule == g_shell32;

        if (!isCallerShell32)
            return DrawThemeBackground_orig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);

        COLORREF color;
        HBRUSH colorBrush;
        RECT clipRect = *pRect;
        BOOL darkMode = AreAppsUsingDarkTheme();

        if (pClipRect) IntersectRect(&clipRect, &clipRect, pClipRect);

        if (g_renderUsingVisualStyles && g_darkModeVSRendering && darkMode && g_darkHTheme) 
            hTheme = g_darkHTheme;

        if (!g_renderUsingVisualStyles) {
            int inset = (clipRect.bottom - clipRect.top) * (100 - g_heightFactor) / 200;

            clipRect.top = clipRect.top + inset;
            clipRect.bottom = clipRect.bottom - inset;
        }

        if (iPartId == PP_FILL) {
            int progressWidth = clipRect.right - clipRect.left;
            int usedPercentage = progressWidth * 100 / g_barWidth;

            if (g_remainingSpaceAsProgress)
                clipRect.right = clipRect.left + g_barWidth - progressWidth;

            if (g_renderUsingVisualStyles)
                DrawThemeBackground_orig(
                    hTheme, hdc, PP_FILL, (usedPercentage >= g_warningThreshold) ? PBFS_ERROR : PBFS_PARTIAL, 
                    &clipRect, 0
                );
            else {
                if (iStateId == PBFS_ERROR || iStateId == PBFS_PARTIAL) 
                    if (usedPercentage >= g_warningThreshold) {
                        color = (darkMode) ? g_progressColorFullDark : g_progressColorFullLight;
                    } else {
                        if (g_useSystemAccentColor)
                            if (IsWindows10OrGreater())
                                color = ((darkMode) ? GetSystemAccentColorShade(2) : GetSystemAccentColorShade(5)) & 0xFFFFFF;
                            else if (IsWindows8OrGreater())
                                color = GetSystemAccentColorShade(9) & 0xFFFFFF;
                            else
                                color = 0x00FF00FF;
                        else
                            color = (darkMode) ? g_progressColorNormalDark : g_progressColorNormalLight;
                    }
                else 
                    return DrawThemeBackground_orig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);

                if (g_renderBarBorder) {
                    clipRect.top++; clipRect.left++; clipRect.bottom--; clipRect.right--;
                }

                colorBrush = CreateSolidBrush(color);

                FillRect(hdc, &clipRect, colorBrush);
                DeleteObject(colorBrush);
            }
            
            return S_OK;
        }

        else if (iPartId == PP_TRANSPARENTBAR) {
            g_barWidth = clipRect.right - clipRect.left;
 
            if (g_renderUsingVisualStyles)
                DrawThemeBackground_orig(hTheme, hdc, PP_TRANSPARENTBAR, PBS_NORMAL, &clipRect, 0);
            else {
                if (g_renderBarBorder) {
                    colorBrush = CreateSolidBrush((darkMode) ? g_barBorderColorDark : g_barBorderColorLight);

                    FillRect(hdc, &clipRect, colorBrush);
                    DeleteObject(colorBrush);
                    
                    clipRect.top++; clipRect.left++; clipRect.bottom--; clipRect.right--;
                }

                colorBrush = CreateSolidBrush((darkMode) ? g_barColorDark : g_barColorLight);

                FillRect(hdc, &clipRect, colorBrush);
                DeleteObject(colorBrush);
            }
            
            return S_OK;
        }
    }
    
    return DrawThemeBackground_orig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
}


static BOOL CALLBACK RefreshExplorerCallback(HWND hwnd, LPARAM lParam) {
    DWORD pid;
    WCHAR windowClass[256];

    GetWindowThreadProcessId(hwnd, &pid); 

    if (
        pid == GetCurrentProcessId() && GetClassName(hwnd, windowClass, 256) 
        && wcscmp(windowClass, L"CabinetWClass") == 0
    )
        SendMessage(hwnd, WM_SETTINGCHANGE, SPI_SETHIGHCONTRAST, 0);
    
    return TRUE;
}


BOOL Wh_ModInit() {
    g_shell32 = GetModuleHandle(L"shell32.dll");
    g_uxtheme = GetModuleHandle(L"uxtheme.dll");

    g_darkHTheme = OpenThemeData(0, L"DarkMode_CopyEngine::Progress");
    
    GetThemeClass = (fnGetThemeClass)GetProcAddress(g_uxtheme, MAKEINTRESOURCEA(74));
    GetImmersiveColorFromColorSetEx = (fnGetImmersiveColorFromColorSetEx)GetProcAddress(g_uxtheme, MAKEINTRESOURCEA(95));
    GetImmersiveUserColorSetPreference = (fnGetImmersiveUserColorSetPreference)GetProcAddress(g_uxtheme, MAKEINTRESOURCEA(98));

    LoadSettings();
    WindhawkUtils::SetFunctionHook(DrawThemeBackground, HookedDrawThemeBackground, &DrawThemeBackground_orig);
    EnumWindows(RefreshExplorerCallback, 0);

    return TRUE;
}


void Wh_ModUninit() {
    if (g_darkHTheme) CloseThemeData(g_darkHTheme);

    EnumWindows(RefreshExplorerCallback, 0);
}


void Wh_ModSettingsChanged() {
    LoadSettings();
    EnumWindows(RefreshExplorerCallback, 0);
}
