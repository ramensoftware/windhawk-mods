// ==WindhawkMod==
// @id              disk-usage-bar-customizer
// @name            Disk Usage Bar Customizer
// @name:ro         Personalizator bară de utilizare discuri
// @description     Customize everything about the disk usage bar from the This PC section in the File Explorer, including theme-aware colors, height, border, rounded corners and more.
// @description:ro  Personalizează orice ține de bara de utilizare a discurilor din secțiunea Acest PC din Explorer, inclusiv culori în funcție de temă, înălțime, bordură, colțuri rotunjite și mai multe.
// @version         1.1.0
// @author          Valer100
// @github          https://github.com/Valer100
// @include         explorer.exe
// @compilerOptions -luxtheme -lgdi32 -lgdiplus
// ==/WindhawkMod==


// ==WindhawkModReadme==
/*
# Disk Usage Bar Customizer
Customize everything about the disk usage bar from the This PC section in the File Explorer, including theme-aware colors, height, border, rounded corners and more.

This is a fork of the original [Disk Usage Bar Color](https://windhawk.net/mods/disk-usage-bar-color) mod made by [dirtyrazkl](https://github.com/dirtyrazkl).

**Warning:** This mod is not compatible with StartAllBack.


## Customization options
### General
- Show remaining space as progress instead of used space
- Custom warning & intermediate percentage thresholds

### Rendering
- Render using visual styles
- Render using dark mode parts when using visual styles

### Custom rendering
- Use system's accent color for the normal progress color
- Render bar border
- Height factor
- Corner radius 
- Percentage label overlay 
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

### Custom intermediate threshold
![Custom intermediate threshold](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/intermediate_progress_style.png)

### Show remaining space as progress instead of used space
![Show remaining space as progress instead of used space](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/remaining_space_as_progress.png)

### Rounded corners
![Rounded corners](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/rounded_corners.png)

### Percentage overlay
![Percentage overlay](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/percentage_overlay.png)
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- general:
  - remainingSpaceAsProgress: false
    $name: Show remaining space as progress instead of used space
    $name:ro: Afișează spațiul rămas ca progres în loc de spațiul utilizat

  - warningPercentageThreshold: 90
    $name: Warning percentage threshold
    $name:ro: Prag în procente pentru avertizare
    $description: >-
      Use the warning progress color/style when the warning threshold (in percents) is reached (default: 90%).
    $description:ro: >-
      Folosește culoarea/stilul de progres pentru avertizare atunci când pragul de avertizare (în procente) este atins (prestabilit: 90%).

  - intermediatePercentageThreshold: 0
    $name: Intermediate percentage threshold
    $name:ro: Prag în procente intermediar
    $description: >-
      Use the intermediate progress color/style when the intermediate threshold (in percents) is reached. This threshold must be lower than the warning threshold for the intermediate state to be displayed. Setting this threshold to 0 will make the intermediate state not being displayed on the usage bar (default: 0%).
    $description:ro: >-
      Folosește culoarea de progres intermediară/stilul de progres intermediar atunci când pragul intermediar (în procente) este atins. Acest prag trebuie să fie mai mic decât pragul de avertizare pentru ca starea intermediară să fie afișată. Setarea acestui prag la 0 va face ca starea intermediară să nu fie afișată pe bara de utilizare (prestabilit: 0%).

  $name: General
  $name:ro: General


- rendering:
  - renderUsingVisualStyles: false
    $name: Render using visual styles
    $name:ro: Randează folosind stiluri vizuale
    $description: >-
      Render the usage bar using the parts provided by the theme to match the system's appearance. You won't be able to customize the bar rendering if this option is enabled.
    $description:ro: >-
      Randează bara de utilizare folosind părțile furnizate de temă pentru a se potrivi cu aspectul sistemului. Nu vei putea personaliza randarea barei dacă această opțiune este activată.

  - darkModeVSRendering: true
    $name: Render using dark mode parts when using visual styles
    $name:ro: Randează folosind părți întunecate atunci când se folosesc stiluri vizuale
    $description: >-
      Render the usage bar using the dark mode parts from the "DarkMode_CopyEngine::Progress" class when dark mode is enabled. You must have Windows 11 build 26200.6899 or higher installed and the "Render using visual styles" option enabled for this to work.
    $description:ro: >-
      Randează bara de utilizare folosind părți întunecate din clasa "DarkMode_CopyEngine::Progress" atunci când modul întunecat este activat. Trebuie să ai instalat Windows 11, build-ul 26200.6899 sau mai recent și opțiunea "Randează folosind stiluri vizuale" activată pentru ca această opțiune să funcționeze.

  $name: Rendering
  $name:ro: Randare


- customRendering:
  - useSystemAccentColor: false
    $name: Use system's accent color for the normal progress color
    $name:ro: Folosește culoarea de accent a sistemului pentru culoarea normală a progresului

  - renderBarBorder: true
    $name: Render bar border
    $name:ro: Randează bordura barei

  - heightFactor: 100
    $name: Height factor
    $name:ro: Factor de înălțime
    $description: >-
      A factor that determines the height of the usage bar (in percents; default: 100%). The factor cannot be greater than 100%.
    $description:ro: >-
      Un factor care determină înălțimea barei de utilizare (în procente; prestabilit: 100%). Factorul nu poate să fie mai mare de 100%.

  - cornerRadiusFactor: 0
    $name: Corner radius factor
    $name:ro: Factor de rază a colțului
    $description: >-
      A factor that determines how rounded the bar's corners are (in percents; default: 0%). The factor cannot be greater than 100%.
    $description:ro: >-
      Un factor care determină cât de rotunjite sunt colțurile barei (în procente; prestabilit: 0%). Factorul nu poate să fie mai mare de 100%.

  - roundProgressRightCorners: true
    $name: Round progress' right corners
    $name:ro: Rotunjește colțurile din dreapta ale progresului

  - percentageLabel: dontShow
    $name: Percentage label
    $name:ro: Etichetă pentru procentaj
    $description: >-
      Show a label on the usage bar indicating the percentage of used or free space.
    $description:ro: >-
      Afișează o etichetă pe bara de utilizare care indică procentul spațiului utilizat sau rămas.
    $options:
    - dontShow: Don't show
    - usedSpace: Show used space
    - freeSpace: Show free space
    $options:ro:
    - dontShow: Nu afișa
    - usedSpace: Afișează spațiul utilizat
    - freeSpace: Afișează spațiul rămas

  - percentageLabelFont: Segoe UI Semibold
    $name: Percentage label font
    $name:ro: Fontul etichetei pentru procentaj

  - percentageLabelSize: 70
    $name: Percentage label font size factor
    $name:ro: Factor de dimensiune a fontului etichetei pentru procentaj
    $description: >-
      Font size as a factor of the maximum bar's height (in percents; default: 70%). This is independent of the "Height factor" setting, so the label stays legible even if the bar's height is very thin.
    $description:ro: >-
      Dimensiunea fontului ca un factor al înălțimii maxime a barei (în procente; prestabilit: 70%). Această setare este independentă de setarea "Factor de înălțime" pentru ca eticheta să rămână lizibilă chiar și atunci când înălțimea barei este foarte subțire.


  - lightModeColors:
    - barColor: "#E6E6E6"
      $name: Bar color
      $name:ro: Culoarea barei
      
    - barBorderColor: "#BCBCBC"
      $name: Bar border color
      $name:ro: Culoarea bordurii barei
      
    - progressColorNormal: "#0070CB"
      $name: Normal progress color
      $name:ro: Culoarea normală a progresului
    
    - progressColorIntermediate: "#9D5D00"
      $name: Intermediate progress color
      $name:ro: Culoarea intermediară a progresului

    - progressColorFull: "#C42B1C"
      $name: Warning progress color
      $name:ro: Culoarea de avertizare a progresului

    - percentageLabelColor: "#000000"
      $name: Percentage label color
      $name:ro: Culoarea etichetei pentru procentaj
        
    $name: Light mode colors
    $name:ro: Culori pentru modul luminos
  
  
  - darkModeColors:
    - barColor: "#383838"
      $name: Bar color
      $name:ro: Culoarea barei
      
    - barBorderColor: "#646464"
      $name: Bar border color
      $name:ro: Culoarea bordurii barei
      
    - progressColorNormal: "#60CDFF"
      $name: Normal progress color
      $name:ro: Culoarea normală a progresului

    - progressColorIntermediate: "#FCE100"
      $name: Intermediate progress color
      $name:ro: Culoarea intermediară a progresului
      
    - progressColorFull: "#FF3D53"
      $name: Warning progress color
      $name:ro: Culoarea de avertizare a progresului

    - percentageLabelColor: "#FFFFFF"
      $name: Percentage label color
      $name:ro: Culoarea etichetei pentru procentaj
      
    $name: Dark mode colors
    $name:ro: Culori pentru modul întunecat

  $name: Custom rendering
  $name:ro: Randare personalizată
  $description: >-
    These options will be ignored when the "Render using visual styles" option is enabled.
  $description:ro: >-
    Aceste opțiuni vor fi ignorate atunci când opțiunea "Randează folosind stiluri vizuale" este activată.
*/
// ==/WindhawkModSettings==


#include <windhawk_utils.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <versionhelpers.h>
#include <gdiplus.h>

using namespace Gdiplus;

// Undocumented functions
using fnGetThemeClass                      = HRESULT(WINAPI*)(HTHEME, LPWSTR, INT);
using fnGetImmersiveColorFromColorSetEx    = DWORD (WINAPI *)(DWORD, DWORD, BOOL, DWORD);
using fnGetImmersiveUserColorSetPreference = DWORD (WINAPI *)(BOOL, BOOL);

fnGetThemeClass GetThemeClass = nullptr;
fnGetImmersiveColorFromColorSetEx GetImmersiveColorFromColorSetEx = nullptr;
fnGetImmersiveUserColorSetPreference GetImmersiveUserColorSetPreference = nullptr;


static decltype(&DrawThemeBackground) DrawThemeBackground_orig = nullptr;


// General
static BOOL     g_remainingSpaceAsProgress       = FALSE;
static INT      g_warningThreshold               = 90;
static INT      g_intermediateThreshold          = 0;

// Rendering
static BOOL     g_renderUsingVisualStyles        = FALSE;
static BOOL     g_darkModeVSRendering            = TRUE;

// Custom rendering
static BOOL     g_useSystemAccentColor           = FALSE;
static BOOL     g_renderBarBorder                = TRUE;
static INT      g_heightFactor                   = 100;
static INT      g_cornerRadiusFactor             = 0;
static BOOL     g_roundProgressRightCorners      = TRUE;
static INT      g_percentageLabel                = 0;
static INT      g_percentageLabelSize            = 70;
static WindhawkUtils::StringSetting  g_percentageLabelFont;

// Light mode colors
static COLORREF g_barColorLight                  = 0x00E6E6E6;
static COLORREF g_barBorderColorLight            = 0x00BCBCBC;
static COLORREF g_progressColorNormalLight       = 0x00CB7000;
static COLORREF g_progressColorIntermediateLight = 0x00005D9D;
static COLORREF g_progressColorFullLight         = 0x001C2BC4;
static COLORREF g_percentageLabelColorLight      = 0x00000000;

// Dark mode colors
static COLORREF g_barColorDark                   = 0x00383838;
static COLORREF g_barBorderColorDark             = 0x00646464;
static COLORREF g_progressColorNormalDark        = 0x00FFCD60;
static COLORREF g_progressColorIntermediateDark  = 0x0000E1FC;
static COLORREF g_progressColorFullDark          = 0x00533DFF;
static COLORREF g_percentageLabelColorDark       = 0x00FFFFFF;

// Other
thread_local int g_barWidth  = 1;

static ULONG_PTR g_gdiplusToken = 0;
HTHEME g_darkHTheme = nullptr;
HMODULE g_uxtheme = nullptr;
HMODULE g_shell32 = nullptr;


static COLORREF LoadColorSetting(PCWSTR colorName, COLORREF fallback) {
    WindhawkUtils::StringSetting originalHexString = WindhawkUtils::StringSetting::make(colorName);
    PCWSTR hexString = originalHexString;

    if (hexString[0] == L'#') hexString++;

    if (wcslen(hexString) != 6)
        return fallback;

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

    if (h0 < 0 || h1 < 0 || h2 < 0 || h3 < 0 || h4 < 0 || h5 < 0)
        return fallback;

    return RGB((h0 << 4) | h1, (h2 << 4) | h3, (h4 << 4) | h5);
}


static void LoadSettings() {
    // General
    g_remainingSpaceAsProgress       = Wh_GetIntSetting(L"general.remainingSpaceAsProgress");
    g_warningThreshold               = Wh_GetIntSetting(L"general.warningPercentageThreshold");
    g_intermediateThreshold          = Wh_GetIntSetting(L"general.intermediatePercentageThreshold");

    // Rendering
    g_renderUsingVisualStyles        = Wh_GetIntSetting(L"rendering.renderUsingVisualStyles");
    g_darkModeVSRendering            = Wh_GetIntSetting(L"rendering.darkModeVSRendering");

    // Custom rendering
    g_useSystemAccentColor           = Wh_GetIntSetting(L"customRendering.useSystemAccentColor");
    g_renderBarBorder                = Wh_GetIntSetting(L"customRendering.renderBarBorder");
    g_heightFactor                   = Wh_GetIntSetting(L"customRendering.heightFactor");
    g_cornerRadiusFactor             = Wh_GetIntSetting(L"customRendering.cornerRadiusFactor");
    g_roundProgressRightCorners      = Wh_GetIntSetting(L"customRendering.roundProgressRightCorners");
    g_percentageLabelFont            = WindhawkUtils::StringSetting::make(L"customRendering.percentageLabelFont");
    g_percentageLabelSize            = Wh_GetIntSetting(L"customRendering.percentageLabelSize");

    PCWSTR percentageLabelMode = Wh_GetStringSetting(L"customRendering.percentageLabel");
    
    if (wcscmp(percentageLabelMode, L"usedSpace") == 0) g_percentageLabel = 1;
    else if (wcscmp(percentageLabelMode, L"freeSpace") == 0) g_percentageLabel = 2;
    else g_percentageLabel = 0;

    Wh_FreeStringSetting(percentageLabelMode);

    if (g_heightFactor > 100) g_heightFactor = 100;
    else if (g_heightFactor < 0) g_heightFactor = 0;

    if (g_cornerRadiusFactor > 100) g_cornerRadiusFactor = 100;
    else if (g_cornerRadiusFactor < 0) g_cornerRadiusFactor = 0;

    if (g_percentageLabelSize > 100) g_percentageLabelSize = 100;
    else if (g_percentageLabelSize < 1) g_percentageLabelSize = 1;

    // Light mode colors
    g_barColorLight                  = LoadColorSetting(L"customRendering.lightModeColors.barColor",                  0x00E6E6E6);
    g_barBorderColorLight            = LoadColorSetting(L"customRendering.lightModeColors.barBorderColor",            0x00BCBCBC);
    g_progressColorNormalLight       = LoadColorSetting(L"customRendering.lightModeColors.progressColorNormal",       0x00CB7000);
    g_progressColorIntermediateLight = LoadColorSetting(L"customRendering.lightModeColors.progressColorIntermediate", 0x00005D9D);
    g_progressColorFullLight         = LoadColorSetting(L"customRendering.lightModeColors.progressColorFull",         0x001C2BC4);
    g_percentageLabelColorLight      = LoadColorSetting(L"customRendering.lightModeColors.percentageLabelColor",      0x00000000);

    // Dark mode colors
    g_barColorDark                   = LoadColorSetting(L"customRendering.darkModeColors.barColor",                   0x00383838);
    g_barBorderColorDark             = LoadColorSetting(L"customRendering.darkModeColors.barBorderColor",             0x00646464);
    g_progressColorNormalDark        = LoadColorSetting(L"customRendering.darkModeColors.progressColorNormal",        0x00FFCD60);
    g_progressColorIntermediateDark  = LoadColorSetting(L"customRendering.darkModeColors.progressColorIntermediate",  0x0000E1FC);
    g_progressColorFullDark          = LoadColorSetting(L"customRendering.darkModeColors.progressColorFull",          0x00533DFF);
    g_percentageLabelColorDark       = LoadColorSetting(L"customRendering.darkModeColors.percentageLabelColor",       0x00FFFFFF);
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


static int GetCornerRadius(const RECT& rect) {
    int height = rect.bottom - rect.top;
    if (height <= 0 || g_cornerRadiusFactor <= 0) return 0;

    int radius = ((height / 2) * g_cornerRadiusFactor + 50) / 100;
    return (radius < 1) ? 1 : radius;
}


static void FillRoundedRect(
    HDC hdc, const RECT& rect, int radius, COLORREF color, bool roundLeft, bool roundRight
) {
    if (rect.right <= rect.left || rect.bottom <= rect.top) return;

    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);

    SolidBrush brush(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)));

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    if (radius <= 0 || (!roundLeft && !roundRight)) {
        graphics.FillRectangle(&brush, rect.left, rect.top, width, height);
        return;
    }

    int diameter = radius * 2;

    GraphicsPath path;
    path.AddArc(rect.left, rect.top, diameter, diameter, 180, 90);
    path.AddArc(rect.right - diameter, rect.top, diameter, diameter, 270, 90);
    path.AddArc(rect.right - diameter, rect.bottom - diameter, diameter, diameter, 0, 90);
    path.AddArc(rect.left, rect.bottom - diameter, diameter, diameter, 90, 90);
    path.CloseFigure();

    graphics.FillPath(&brush, &path);

    if (!roundLeft)
        graphics.FillRectangle(&brush, rect.left, rect.top, radius, height);

    if (!roundRight)
        graphics.FillRectangle(&brush, rect.right - radius, rect.top, radius, height);
}


static void DrawPercentageLabel(
    HDC hdc, RECT &rect, LPCWSTR fontFamily, int fontHeight, int percentage, COLORREF color
) {
    if (fontHeight == 0) return;

    HFONT font = CreateFontW(
        fontHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, 
        DEFAULT_PITCH | FF_DONTCARE, fontFamily
    );

    if (!font) return;

    WCHAR text[16];
    swprintf(text, 16, L"%d%%", percentage);

    HGDIOBJ oldFont = SelectObject(hdc, font);
    COLORREF oldColor = SetTextColor(hdc, color);
    int oldBkMode = SetBkMode(hdc, TRANSPARENT);
    
    SelectObject(hdc, font);
    SetTextColor(hdc, color);
    DrawTextW(hdc, text, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);

    SetBkMode(hdc, oldBkMode);
    SetTextColor(hdc, oldColor);
    SelectObject(hdc, oldFont);
    DeleteObject(font);
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
        INT progressStyle;
        RECT clipRect = *pRect;
        BOOL darkMode = AreAppsUsingDarkTheme();

        if (pClipRect) IntersectRect(&clipRect, &clipRect, pClipRect);

        if (g_renderUsingVisualStyles && g_darkModeVSRendering && darkMode && g_darkHTheme) 
            hTheme = g_darkHTheme;

        RECT fullBarRect = { 
            clipRect.left, clipRect.top, clipRect.left + g_barWidth, clipRect.bottom 
        };

        int maxBarHeight = clipRect.bottom - clipRect.top;
        int percentageLabelFontHeight = -(maxBarHeight * g_percentageLabelSize / 100);

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

            if (g_renderUsingVisualStyles) {
                if (usedPercentage >= g_warningThreshold)
                    progressStyle = PBFS_ERROR;
                else if (g_intermediateThreshold && usedPercentage >= g_intermediateThreshold)
                    progressStyle = PBFS_PAUSED;
                else 
                    progressStyle = PBFS_PARTIAL;

                DrawThemeBackground_orig(hTheme, hdc, PP_FILL, progressStyle, &clipRect, 0);
            } else {
                if (iStateId == PBFS_ERROR || iStateId == PBFS_PARTIAL) 
                    if (usedPercentage >= g_warningThreshold) {
                        color = (darkMode) ? g_progressColorFullDark : g_progressColorFullLight;
                    } else if (g_intermediateThreshold && usedPercentage >= g_intermediateThreshold) {
                        color = (darkMode) ? g_progressColorIntermediateDark : g_progressColorIntermediateLight;
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

                int radius = GetCornerRadius(clipRect);

                if (g_renderBarBorder) {
                    clipRect.top++; clipRect.left++; clipRect.bottom--; clipRect.right--;
                    if (radius > 0) radius--;
                }

                if (radius * 2 <= clipRect.right - clipRect.left)
                    FillRoundedRect(hdc, clipRect, radius, color, TRUE, g_roundProgressRightCorners);

                if (g_percentageLabel) {
                    DrawPercentageLabel(
                        hdc, fullBarRect, g_percentageLabelFont, percentageLabelFontHeight, 
                        (g_percentageLabel == 1) ? usedPercentage : (100 - usedPercentage),
                        (darkMode) ? g_percentageLabelColorDark : g_percentageLabelColorLight
                    );
                }
            }
            
            return S_OK;
        }

        else if (iPartId == PP_TRANSPARENTBAR) {
            g_barWidth  = clipRect.right - clipRect.left;
            if (g_barWidth < 1) g_barWidth = 1;
 
            if (g_renderUsingVisualStyles)
                DrawThemeBackground_orig(hTheme, hdc, PP_TRANSPARENTBAR, PBS_NORMAL, &clipRect, 0);
            else {
                int radius = GetCornerRadius(clipRect);

                if (g_renderBarBorder) {
                    FillRoundedRect(
                        hdc, clipRect, radius, (darkMode) ? g_barBorderColorDark : g_barBorderColorLight, 
                        true, true
                    );

                    clipRect.top++; clipRect.left++; clipRect.bottom--; clipRect.right--;
                    if (radius > 0) radius--;
                }

                FillRoundedRect(
                    hdc, clipRect, radius, (darkMode) ? g_barColorDark : g_barColorLight, 
                    true, true
                );
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
    GdiplusStartupInput gdiplusStartupInput;

    if (GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr) != 0)  // Status.Ok
        return FALSE;

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

    if (g_gdiplusToken) {
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}


void Wh_ModSettingsChanged() {
    LoadSettings();
    EnumWindows(RefreshExplorerCallback, 0);
}
