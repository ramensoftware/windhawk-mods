// ==WindhawkMod==
// @id              dwm-custom-projection-border
// @name            DWM Custom Projection Border
// @description     Customize or disable the yellow border drawn by DWM when a capture is started using WinRT
// @version         1.0.0
// @author          erisune
// @github          https://github.com/erisune
// @license         GPL-3.0
// @include         dwm.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# DWM Custom Projection Border

This mod allows you to customize or disable the screen capture indicator, typically a yellow border,
that is drawn by DWM on top of a window or display that is being captured using WinRT, specifically `Windows.Graphics.Capture`.
This mod is particularly useful on Windows 10, which doesn't have a toggle to disable this border.

## ⚠ Important usage note ⚠

In order to use this mod, you must allow Windhawk to inject into the **dwm.exe**
system process. To do so, add it to the process inclusion list in the advanced
settings. If you do not do this, it will silently fail to inject.

![Advanced settings screenshot](https://i.imgur.com/LRhREtJ.png)

## Examples

**Pink border on Windows 11 26H1**:

![Pink border on Windows 11 26H1](https://raw.githubusercontent.com/erisune/misc/refs/heads/images/dwm-custom-projection-border/win11-pink-projection-border-visual.png)

**Disabled border on Windows 10 21H2**:

![Disabled border on Windows 10 21H2](https://raw.githubusercontent.com/erisune/misc/refs/heads/images/dwm-custom-projection-border/win10-disabled-projection-border-visual.png)

## Notes
* To update an active border after you change the settings, move the window or ultimately, try restarting the capture.
* To show the border on Windows 11 you may need to prevent other applications from turning it off, this toggle is in **Settings > Privacy and Security > Screenshot borders**.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- disable: false
  $name: Disable border
  $description: Completely disable the projection border visual
- outerOnly: false
  $name: Draw outer border only
  $description: DWM draws an outer border and an inner border by default
- outerBorder:
  - red: 255
    $name: Red value (0 - 255)
    $description: "Default: 255"
  - green: 221
    $name: Green value (0 - 255)
    $description: "Default: 221"
  - blue: 0
    $name: Blue value (0 - 255)
    $description: "Default: 0"
  - alpha: 191
    $name: Alpha value (0 - 255)
    $description: "Default: 191, High Contrast Mode: 255"
  - size: 2
    $name: Size
    $description: "Default: 2, High Contrast Mode: 4"
  $name: Outer border
- innerBorder:
  - red: 0
    $name: Red value (0 - 255)
    $description: "Default: 0"
  - green: 0
    $name: Green value (0 - 255)
    $description: "Default: 0"
  - blue: 0
    $name: Blue value (0 - 255)
    $description: "Default: 0"
  - alpha: 191
    $name: Alpha value (0 - 255)
    $description: "Default: 191"
  - size: 1
    $name: Size
    $description: "Default: 1"
  $name: Inner border
- margins:
  - left: 0
    $name: Left
  - right: 0
    $name: Right
  - top: 0
    $name: Top
  - bottom: 0
    $name: Bottom
  $name: Extra margins
  $description: Negative values are allowed
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <d3d9types.h>
#include <uxtheme.h>

struct {
    bool disableBorder;
    bool drawOuterOnly;
    D3DCOLORVALUE outerColor;
    D3DCOLORVALUE innerColor;
    unsigned int outerSize;
    unsigned int innerSize;
    MARGINS margins;
} g_modSettings;
thread_local bool g_outerBorderDrawn = false;
thread_local bool g_innerBorderDrawn = false;

bool IsHighContrastMode()
{
    HIGHCONTRAST hc = { sizeof(HIGHCONTRAST) };
    SystemParametersInfoW(SPI_GETHIGHCONTRAST, 0, &hc, 0);
    return hc.dwFlags & HCF_HIGHCONTRASTON;
}

void AdjustInnerRect(LPRECT lpRect)
{
    /* 
     * DWM already deflates the inner rect by 2 (or 4 in High Contrast mode)
     * in CProjectionBorderVisual::_UpdateInstructions so we undo that before
     * applying our own setting.
     */
    const int oldDeflate = IsHighContrastMode() ? 4 : 2;
    const int newDeflate = oldDeflate - g_modSettings.outerSize;
    InflateRect(lpRect, newDeflate, newDeflate);
}

HRESULT (*CProjectionBorderVisual__ValidateVisual_orig)(void *);
HRESULT CProjectionBorderVisual__ValidateVisual_hook(void *pThis) {
    g_outerBorderDrawn = false;
    return CProjectionBorderVisual__ValidateVisual_orig(pThis);
}

void (*CProjectionBorderVisual__UpdateRect_orig)(void *, LPRECT);
void CProjectionBorderVisual__UpdateRect_hook(void *pThis, LPRECT lpRect) {
    RECT newRect;
    CopyRect(&newRect, lpRect);
    newRect.left += g_modSettings.margins.cxLeftWidth;
    newRect.right += g_modSettings.margins.cxRightWidth;
    newRect.top += g_modSettings.margins.cyTopHeight;
    newRect.bottom += g_modSettings.margins.cyBottomHeight;
    CProjectionBorderVisual__UpdateRect_orig(pThis, &newRect);
}

/* 
 * Needed for accurate color conversions from user settings.
 * Alpha values are never converted to scRGB.
 */
void Convert_MilColorF_sRGB_To_MilColorF_scRGB(D3DCOLORVALUE *pColor)
{
    /* From dwmcore.dll */
    const float GammaLUT_sRGB_to_scRGB[256] = {
        0.0, 0.077399381, 0.15479876, 0.23219815, 0.30959752, 0.3869969, 0.4643963, 0.54179567,
        0.61919504, 0.69659442, 0.77399379, 0.85336661, 0.93750936, 1.0263028, 1.1198177, 1.2181232,
        1.3212868, 1.4293748, 1.5424525, 1.6605831, 1.7838296, 1.9122531, 2.0459142, 2.1848722,
        2.329185, 2.4789104, 2.634105, 2.7948239, 2.9611225, 3.1330545, 3.3106732, 3.4940312,
        3.6831801, 3.8781712, 4.0790548, 4.285881, 4.4986982, 4.717556, 4.942502, 5.1735835,
        5.4108477, 5.6543407, 5.9041085, 6.1601963, 6.4226494, 6.6915116, 6.9668274, 7.2486401,
        7.5369925, 7.8319283, 8.1334887, 8.4417152, 8.7566509, 9.0783358, 9.4068098, 9.742115,
        10.08429, 10.433375, 10.78941, 11.152432, 11.522482, 11.899597, 12.283815, 12.675175,
        13.073712, 13.479465, 13.89247, 14.312765, 14.740385, 15.175365, 15.617743, 16.067554,
        16.524834, 16.989614, 17.461933, 17.941824, 18.429321, 18.924459, 19.427273, 19.937792,
        20.456055, 20.98209, 21.515934, 22.057617, 22.607174, 23.164637, 23.730036, 24.303404,
        24.884773, 25.474176, 26.071642, 26.677204, 27.290892, 27.912737, 28.542768, 29.181021,
        29.82752, 30.4823, 31.145388, 31.816814, 32.496609, 33.184803, 33.88142, 34.586498,
        35.30006, 36.022141, 36.752762, 37.491955, 38.239746, 38.99617, 39.76125, 40.535011,
        41.31749, 42.108711, 42.908695, 43.71748, 44.535088, 45.361546, 46.196884, 47.041122,
        47.894299, 48.756428, 49.627548, 50.507675, 51.396843, 52.295078, 53.2024, 54.118843,
        55.044426, 55.979179, 56.92313, 57.876297, 58.838711, 59.810398, 60.791382, 61.781685,
        62.781338, 63.790363, 64.808784, 65.836624, 66.873917, 67.920677, 68.976936, 70.042717,
        71.118034, 72.202927, 73.297417, 74.401512, 75.515259, 76.638664, 77.771767, 78.914574,
        80.067123, 81.229431, 82.40152, 83.583412, 84.775139, 85.976723, 87.188179, 88.409531,
        89.640816, 90.882034, 92.133232, 93.394409, 94.665611, 95.946838, 97.238136, 98.539505,
        99.850983, 101.17258, 102.50433, 103.84625, 105.19836, 106.56069, 107.93326, 109.31608,
        110.70918, 112.11258, 113.52631, 114.95038, 116.38481, 117.82964, 119.28487, 120.75053,
        122.22665, 123.71323, 125.21032, 126.71791, 128.23605, 129.76474, 131.304, 132.85387,
        134.41435, 135.98549, 137.56728, 139.15974, 140.76291, 142.3768, 144.00143, 145.63683,
        147.283, 148.94, 150.6078, 152.28645, 153.97597, 155.67638, 157.38768, 159.10989,
        160.84306, 162.5872, 164.34232, 166.10844, 167.88557, 169.67377, 171.47301, 173.28333,
        175.10475, 176.9373, 178.78098, 180.63582, 182.50185, 184.37906, 186.26749, 188.16716,
        190.07808, 192.00026, 193.93375, 195.87854, 197.83467, 199.80214, 201.78098, 203.77119,
        205.77283, 207.78587, 209.81036, 211.84631, 213.89375, 215.95267, 218.02312, 220.10509,
        222.19861, 224.30371, 226.42039, 228.54869, 230.6886, 232.84015, 235.00337, 237.17827,
        239.36487, 241.56317, 243.77321, 245.995, 248.22855, 250.47389, 252.73103, 255.0
    };
    byte r = pColor->r * 255.0f;
    byte g = pColor->g * 255.0f;
    byte b = pColor->b * 255.0f;
    pColor->r = GammaLUT_sRGB_to_scRGB[r] / 255.0f;
    pColor->g = GammaLUT_sRGB_to_scRGB[g] / 255.0f;
    pColor->b = GammaLUT_sRGB_to_scRGB[b] / 255.0f;
}

HRESULT (*CProjectionBorderVisual___AddBorderInstructions_orig)(void *, LPRECT, UINT, D3DCOLORVALUE *);
HRESULT CProjectionBorderVisual___AddBorderInstructions_hook(
    void *pThis,
    LPRECT lpRect,
    UINT uSize,
    D3DCOLORVALUE *pColor)
{
    if (g_modSettings.disableBorder) {
        return S_OK;
    }

    if (g_modSettings.drawOuterOnly && g_outerBorderDrawn) {
        return S_OK;
    }

    UINT newSize;
    D3DCOLORVALUE newColor;
    if (!g_outerBorderDrawn) {
        g_outerBorderDrawn = true;
        newSize = g_modSettings.outerSize;
        newColor = g_modSettings.outerColor;
        return CProjectionBorderVisual___AddBorderInstructions_orig(pThis, lpRect, newSize, &newColor);
    }
    else {
        RECT newRect;
        CopyRect(&newRect, lpRect);
        AdjustInnerRect(&newRect);
        newSize = g_modSettings.innerSize;
        newColor = g_modSettings.innerColor;
        return CProjectionBorderVisual___AddBorderInstructions_orig(pThis, &newRect, newSize, &newColor);
    }
}

/* For Windows 11 26H1 */
HRESULT (*CProjectionBorderVisual___CreateOrUpdateBrush_orig)(void *, LPRECT, UINT, D3DCOLORVALUE *, void *);
HRESULT CProjectionBorderVisual___CreateOrUpdateBrush_hook(
    void *pThis,
    LPRECT lpRect,
    UINT uSize,
    D3DCOLORVALUE *pColor,
    void *pNineGridVisual)
{
    /* 
     * No color conversions are done in this version.
     * The default green value for the outer border here is 185.
     * Its internal value hasn't really changed, however in other
     * versions it's converted to scRGB.
     */
    if (g_modSettings.disableBorder) {
        RECT rcEmpty = { 0 };
        return CProjectionBorderVisual___CreateOrUpdateBrush_orig(pThis, &rcEmpty, uSize, pColor, pNineGridVisual);
    }
    else if (g_modSettings.drawOuterOnly && g_outerBorderDrawn) {
        RECT rcEmpty = { 0 };
        return CProjectionBorderVisual___CreateOrUpdateBrush_orig(pThis, &rcEmpty, uSize, pColor, pNineGridVisual);
    }
    else {
        UINT newSize;
        D3DCOLORVALUE newColor;
        if (!g_outerBorderDrawn) {
            g_outerBorderDrawn = true;
            newSize = g_modSettings.outerSize;
            newColor = g_modSettings.outerColor;
        }
        else {
            g_innerBorderDrawn = true;
            newSize = g_modSettings.innerSize;
            newColor = g_modSettings.innerColor;
        }
        return CProjectionBorderVisual___CreateOrUpdateBrush_orig(pThis, lpRect, newSize, &newColor, pNineGridVisual);
    }
}

/* For Windows 11 26H1, called shortly after CProjectionBorderVisual::_CreateOrUpdateBrush */
void (*CRectangleVisual__SetRect_orig)(void *, LPRECT);
void CRectangleVisual__SetRect_hook(void *pThis, LPRECT lpRect) {
    if (g_innerBorderDrawn) {
        g_innerBorderDrawn = false;
        RECT newRect;
        CopyRect(&newRect, lpRect);
        AdjustInnerRect(&newRect);
        return CRectangleVisual__SetRect_orig(pThis, &newRect);
    }
    else {
        return CRectangleVisual__SetRect_orig(pThis, lpRect);
    }
}

float Convert_IntSetting_To_MilColorF_sRGB(LPCWSTR setting)
{
    int value = Wh_GetIntSetting(setting);
    value = std::max(0, std::min(value, 255));
    return value / 255.0f;
}

void LoadSettings()
{
    g_modSettings.disableBorder = Wh_GetIntSetting(L"disable");
    g_modSettings.drawOuterOnly = Wh_GetIntSetting(L"outerOnly");

    g_modSettings.outerColor.r = Convert_IntSetting_To_MilColorF_sRGB(L"outerBorder.red");
    g_modSettings.outerColor.g = Convert_IntSetting_To_MilColorF_sRGB(L"outerBorder.green");
    g_modSettings.outerColor.b = Convert_IntSetting_To_MilColorF_sRGB(L"outerBorder.blue");
    g_modSettings.outerColor.a = Convert_IntSetting_To_MilColorF_sRGB(L"outerBorder.alpha");
    g_modSettings.outerSize = Wh_GetIntSetting(L"outerBorder.size");

    g_modSettings.innerColor.r = Convert_IntSetting_To_MilColorF_sRGB(L"innerBorder.red");
    g_modSettings.innerColor.g = Convert_IntSetting_To_MilColorF_sRGB(L"innerBorder.green");
    g_modSettings.innerColor.b = Convert_IntSetting_To_MilColorF_sRGB(L"innerBorder.blue");
    g_modSettings.innerColor.a = Convert_IntSetting_To_MilColorF_sRGB(L"innerBorder.alpha");
    g_modSettings.innerSize = Wh_GetIntSetting(L"innerBorder.size");

    if (CProjectionBorderVisual___AddBorderInstructions_orig) {
        Convert_MilColorF_sRGB_To_MilColorF_scRGB(&g_modSettings.outerColor);
        Convert_MilColorF_sRGB_To_MilColorF_scRGB(&g_modSettings.innerColor);
    }

    g_modSettings.margins.cxLeftWidth = Wh_GetIntSetting(L"margins.left");
    g_modSettings.margins.cxRightWidth = Wh_GetIntSetting(L"margins.right");
    g_modSettings.margins.cyTopHeight = Wh_GetIntSetting(L"margins.top");
    g_modSettings.margins.cyBottomHeight = Wh_GetIntSetting(L"margins.bottom");
}

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
}

WindhawkUtils::SYMBOL_HOOK uDWMDllHooks[] = {
    {
        {
            L"public: virtual long __cdecl CProjectionBorderVisual::ValidateVisual(void)"
        },
        &CProjectionBorderVisual__ValidateVisual_orig,
        CProjectionBorderVisual__ValidateVisual_hook,
        false
    },
    {
        {
            L"public: void __cdecl CProjectionBorderVisual::UpdateRect(struct tagRECT const &)"
        },
        &CProjectionBorderVisual__UpdateRect_orig,
        CProjectionBorderVisual__UpdateRect_hook,
        false
    },
    {
        {
            L"protected: long __cdecl CProjectionBorderVisual::_AddBorderInstructions(struct tagRECT const &,unsigned int,struct _D3DCOLORVALUE const &)"
        },
        &CProjectionBorderVisual___AddBorderInstructions_orig,
        CProjectionBorderVisual___AddBorderInstructions_hook,
        true
    },
    {
        {
            L"protected: long __cdecl CProjectionBorderVisual::_CreateOrUpdateBrush(struct tagRECT const &,unsigned int,struct _D3DCOLORVALUE const &,class CNineGridVisual *)"
        },
        &CProjectionBorderVisual___CreateOrUpdateBrush_orig,
        CProjectionBorderVisual___CreateOrUpdateBrush_hook,
        true
    },
    {
        {
            L"public: void __cdecl CRectangleVisual::SetRect(struct tagRECT const &)"
        },
        &CRectangleVisual__SetRect_orig,
        CRectangleVisual__SetRect_hook,
        true
    }
};

BOOL Wh_ModInit(void)
{
    Wh_Log(L"Init");

    HMODULE uDWMDll = LoadLibraryW(L"uDWM.dll");
    if (!uDWMDll)
    {
        Wh_Log(L"Failed to load uDWM.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(uDWMDll, uDWMDllHooks, ARRAYSIZE(uDWMDllHooks)))
    {
        Wh_Log(L"Failed to hook uDWM.dll");
        return FALSE;
    }

    LoadSettings();

    return TRUE;
}