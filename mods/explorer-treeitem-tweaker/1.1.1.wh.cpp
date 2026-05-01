// ==WindhawkMod==
// @id              explorer-treeitem-tweaker
// @name            Explorer TreeItem Tweaker
// @description     Custom backgrounds and text colors for Explorer TreeView
// @version         1.1.1
// @author          Languster
// @github          https://github.com/Languster
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luxtheme -lcomctl32 -lgdi32 -lgdiplus
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer TreeItem Tweaker

**Explorer TreeItem Tweaker** is a Windhawk mod that customizes the appearance of tree items in the left navigation pane of Windows Explorer.

It is designed for people who want the navigation tree to look cleaner, more polished, and more comfortable to use.

**Recommended setup**  
For the best Explorer navigation pane experience, it is recommended to use this mod together with **[Explorer Navigation Tree Offset](https://windhawk.net/mods/explorer-navigation-pane-tweaks)** and **[Explorer TreeLine Killer](https://windhawk.net/mods/explorer-treeline-killer)**.

## ⚠️ Important: dark theme defaults

**The default settings are designed for the dark Windows system theme.**

For a correct and clean appearance on the light Windows theme, adjust the mod settings manually, especially the text color.

For example, for light theme text, try:

`R = 0, G = 0, B = 0`

## Before / After
![Before / After](https://raw.githubusercontent.com/Languster/assets-windhawk-mods/main/mods_assets/explorer-tree-item-tweaker-before-after.jpg)

## Width mode note
System Width mode (0) uses the original TreeView menu item background width and is not recommended if you use **Explorer Navigation Tree Offset**, in order to avoid background clipping.

Use **Width By Text** or **One Width For All** instead.

## Features
- customize background, border, and text colors for these states:
  - Hot
  - Selected
  - Selected Not Focus
  - Hot Selected
  - Normal Text
- adjust:
  - width
  - corner radius
  - vertical insets
  - item height
  - icon size
  - icon-text gap
  - Quick Access pin button hiding
  - custom font face and size
  - color
  - background opacity
  - border opacity
  - text color

## Why this is better than editing msstyles
Editing `msstyles` does not come close to this level of flexibility.

With this mod, you can:
- change settings directly in Windhawk
- control colors, opacity, and states much more precisely
- get cleaner shapes and smoother rounded corners
- avoid editing system theme resources

In practice, this is a much faster, more convenient, and more powerful way to customize the navigation tree.

## Compatibility
Tested and confirmed compatible with **Windows 11 versions 23H2 through 25H2**.
Other versions have not been tested.

## Important
- this mod changes only the tree view in the left navigation pane of Explorer
- it does not change the Explorer window background
- background, border, and text can be customized independently
- optional layout controls can change item height, icon size, and icon-text gap
- **Item Height** changes the total row height of tree items
- **Icon Size** changes the width and height of the navigation icons
- **Icon-Text Gap** adds space between the icon and the text
- **Hide Pin Button** hides the Quick Access pin button in the navigation tree
- **Custom Font** changes the tree item text font face and size
- **Text Font Weight** and **Text Font Style** can be configured separately for each text state

## Layout controls
- **Item Height** increases or decreases the total height of each navigation row
- **Icon-Text Gap** controls the horizontal spacing between the icon and the text
- **Icon Size** controls the width and height of navigation icons

Most layout options have their own enable switch.
When a switch is off, that option is not applied.

## Custom Font
The **Custom Font** block controls the common tree item font face and size before per-state text settings.

This block does not need a separate enable switch. Empty or `0` values keep the native Explorer font and the custom font code is not applied.

- **Font Face**: leave empty to keep the native Explorer font family
- **Font Size**: `0` = default/native size

Text weight and style are configured inside each text state block:

- **Font Weight**: `0` = default, `1` = semibold, `2` = bold
- **Font Style**: `0` = default, `1` = italic, `2` = underline, `3` = italic + underline

## Credits
Thanks to [crazyboyybs](https://github.com/crazyboyybs) for extending the functionality.
Thanks to [valinet](https://github.com/valinet) for the Hide Pin Button function.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Global:
  - CornerRadius: 18
    $name: Corner Radius
    $description: Controls how rounded the corners are
  - WidthMode: 2
    $name: Width Mode
    $description: Choose how item width is calculated. 0 = system, 1 = one width for all, 2 = width by text. Value 0 is not recommended when using Explorer Navigation Tree Offset
  - OneWidthForAll:
    - LeftMargin: 6
      $name: Left Margin
      $description: Left margin used when Width Mode is set to One width for all
    - RightMargin: 6
      $name: Right Margin
      $description: Right margin used when Width Mode is set to One width for all
    $name: One Width For All
  - WidthByText:
    - ExtraLeft: 38
      $name: Extra Left
      $description: Extra space added to the left when Width Mode is set to Width by text
    - ExtraRight: 10
      $name: Extra Right
      $description: Extra space added to the right when Width Mode is set to Width by text
    $name: Width By Text
  - VerticalInsets:
    - TopInset: 2
      $name: Top Inset
      $description: Adds space at the top of the shape
    - BottomInset: 2
      $name: Bottom Inset
      $description: Adds space at the bottom of the shape
    $name: Vertical Insets
  - ItemHeight:
    - Enabled: false
      $name: Enabled
      $description: Apply a custom row height
    - Value: 46
      $name: Item Height (px)
      $description: Sets the total height of each tree item row
    $name: Item Height
  - IconTextGap:
    - Enabled: false
      $name: Enabled
      $description: Apply custom spacing between the icon and the text
    - Value: 2
      $name: Icon-Text Gap (px)
      $description: Adds extra horizontal space after the icon
    $name: Icon-Text Gap
  - IconSize:
    - Enabled: false
      $name: Enabled
      $description: Apply a custom icon size
    - Value: 22
      $name: Icon Size (px)
      $description: Sets the width and height of navigation icons
    $name: Icon Size
  - HidePinButton:
    - Enabled: false
      $name: Enabled
      $description: Hides the Quick Access pin button in the navigation tree
    $name: Hide Pin Button
  $name: Global

- CustomFont:
  - FaceName: ""
    $name: Font Face
    $description: Font family name used for tree item text. Leave empty to keep the native Explorer navigation pane font
  - Size: 0
    $name: Font Size (pt)
    $description: Font size in points. Set 0 to keep the native size
  $name: Custom Font

- NormalText:
  - Enabled: true
    $name: Enabled
    $description: Enable a custom text color for the normal state
  - R: 210
    $name: Red
    $description: Red channel for the normal text color
  - G: 210
    $name: Green
    $description: Green channel for the normal text color
  - B: 210
    $name: Blue
    $description: Blue channel for the normal text color
  - Weight: 0
    $name: Font Weight
    $description: Font weight mode. 0 = default, 1 = semibold, 2 = bold
  - Style: 0
    $name: Font Style
    $description: Font style mode. 0 = default, 1 = italic, 2 = underline, 3 = italic + underline
  $name: Normal Text

- HotState:
  - Background:
    - Enabled: true
      $name: Enabled
      $description: Enable a custom background for the hot state
    - Opacity: 100
      $name: Opacity
      $description: Adjust transparency. Set 0 for no background
    - R: 38
      $name: Red
      $description: Red channel for the hot background color
    - G: 38
      $name: Green
      $description: Green channel for the hot background color
    - B: 38
      $name: Blue
      $description: Blue channel for the hot background color
    $name: Background
  - Border:
    - Enabled: true
      $name: Enabled
      $description: Enable a custom border for the hot state
    - Opacity: 100
      $name: Opacity
      $description: Adjust transparency. Set 0 for no border
    - R: 50
      $name: Red
      $description: Red channel for the hot border color
    - G: 50
      $name: Green
      $description: Green channel for the hot border color
    - B: 50
      $name: Blue
      $description: Blue channel for the hot border color
    $name: Border
  - Text:
    - Enabled: true
      $name: Enabled
      $description: Enable a custom text color for the hot state
    - R: 210
      $name: Red
      $description: Red channel for the hot text color
    - G: 210
      $name: Green
      $description: Green channel for the hot text color
    - B: 210
      $name: Blue
      $description: Blue channel for the hot text color
    - Weight: 0
      $name: Font Weight
      $description: Font weight mode for the hot state. 0 = default, 1 = semibold, 2 = bold
    - Style: 0
      $name: Font Style
      $description: Font style mode for the hot state. 0 = default, 1 = italic, 2 = underline, 3 = italic + underline
    $name: Text
  $name: Hot State

- SelectedState:
  - Background:
    - Enabled: true
      $name: Enabled
      $description: Enable a custom background for the selected state
    - Opacity: 100
      $name: Opacity
      $description: Adjust transparency. Set 0 for no background
    - R: 38
      $name: Red
      $description: Red channel for the selected background color
    - G: 38
      $name: Green
      $description: Green channel for the selected background color
    - B: 38
      $name: Blue
      $description: Blue channel for the selected background color
    $name: Background
  - Border:
    - Enabled: true
      $name: Enabled
      $description: Enable a custom border for the selected state
    - Opacity: 100
      $name: Opacity
      $description: Adjust transparency. Set 0 for no border
    - R: 50
      $name: Red
      $description: Red channel for the selected border color
    - G: 50
      $name: Green
      $description: Green channel for the selected border color
    - B: 50
      $name: Blue
      $description: Blue channel for the selected border color
    $name: Border
  - Text:
    - Enabled: true
      $name: Enabled
      $description: Enable a custom text color for the selected state
    - R: 210
      $name: Red
      $description: Red channel for the selected text color
    - G: 210
      $name: Green
      $description: Green channel for the selected text color
    - B: 210
      $name: Blue
      $description: Blue channel for the selected text color
    - Weight: 0
      $name: Font Weight
      $description: Font weight mode for the selected state. 0 = default, 1 = semibold, 2 = bold
    - Style: 0
      $name: Font Style
      $description: Font style mode for the selected state. 0 = default, 1 = italic, 2 = underline, 3 = italic + underline
    $name: Text
  $name: Selected State

- SelectedNotFocusState:
  - Background:
    - Enabled: true
      $name: Enabled
      $description: Enable a custom background for the selected not focus state
    - Opacity: 100
      $name: Opacity
      $description: Adjust transparency. Set 0 for no background
    - R: 34
      $name: Red
      $description: Red channel for the selected not focus background color
    - G: 34
      $name: Green
      $description: Green channel for the selected not focus background color
    - B: 34
      $name: Blue
      $description: Blue channel for the selected not focus background color
    $name: Background
  - Border:
    - Enabled: false
      $name: Enabled
      $description: Enable a custom border for the selected not focus state
    - Opacity: 100
      $name: Opacity
      $description: Adjust transparency. Set 0 for no border
    - R: 34
      $name: Red
      $description: Red channel for the selected not focus border color
    - G: 34
      $name: Green
      $description: Green channel for the selected not focus border color
    - B: 34
      $name: Blue
      $description: Blue channel for the selected not focus border color
    $name: Border
  - Text:
    - Enabled: true
      $name: Enabled
      $description: Enable a custom text color for the selected not focus state
    - R: 210
      $name: Red
      $description: Red channel for the selected not focus text color
    - G: 210
      $name: Green
      $description: Green channel for the selected not focus text color
    - B: 210
      $name: Blue
      $description: Blue channel for the selected not focus text color
    - Weight: 0
      $name: Font Weight
      $description: Font weight mode for the selected not focus state. 0 = default, 1 = semibold, 2 = bold
    - Style: 0
      $name: Font Style
      $description: Font style mode for the selected not focus state. 0 = default, 1 = italic, 2 = underline, 3 = italic + underline
    $name: Text
  $name: Selected Not Focus State

- HotSelectedState:
  - Background:
    - Enabled: true
      $name: Enabled
      $description: Enable a custom background for the hot selected state
    - Opacity: 100
      $name: Opacity
      $description: Adjust transparency. Set 0 for no background
    - R: 42
      $name: Red
      $description: Red channel for the hot selected background color
    - G: 42
      $name: Green
      $description: Green channel for the hot selected background color
    - B: 42
      $name: Blue
      $description: Blue channel for the hot selected background color
    $name: Background
  - Border:
    - Enabled: true
      $name: Enabled
      $description: Enable a custom border for the hot selected state
    - Opacity: 150
      $name: Opacity
      $description: Adjust transparency. Set 0 for no border
    - R: 56
      $name: Red
      $description: Red channel for the hot selected border color
    - G: 56
      $name: Green
      $description: Green channel for the hot selected border color
    - B: 56
      $name: Blue
      $description: Blue channel for the hot selected border color
    $name: Border
  - Text:
    - Enabled: true
      $name: Enabled
      $description: Enable a custom text color for the hot selected state
    - R: 210
      $name: Red
      $description: Red channel for the hot selected text color
    - G: 210
      $name: Green
      $description: Green channel for the hot selected text color
    - B: 210
      $name: Blue
      $description: Blue channel for the hot selected text color
    - Weight: 0
      $name: Font Weight
      $description: Font weight mode for the hot selected state. 0 = default, 1 = semibold, 2 = bold
    - Style: 0
      $name: Font Style
      $description: Font style mode for the hot selected state. 0 = default, 1 = italic, 2 = underline, 3 = italic + underline
    $name: Text
  $name: Hot Selected State
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <uxtheme.h>
#include <commctrl.h>
#include <windhawk_utils.h>
#include <gdiplus.h>
#include <algorithm>
#include <vector>
#include <utility>

#ifndef TMT_TEXTCOLOR
#define TMT_TEXTCOLOR 3803
#endif

using namespace Gdiplus;

#ifdef _WIN64
#define STDCALL __cdecl
#define SSTDCALL L"__cdecl"
#else
#define STDCALL __stdcall
#define SSTDCALL L"__stdcall"
#endif

static HMODULE g_hExplorerFrame = nullptr;

struct ExplorerState
{
    HWND hExplorer;
    HWND hTree;
};

struct TreeMetricsState
{
    HWND hTree;
    HIMAGELIST origNormal;
    HIMAGELIST ownedNormal;
    int originalItemHeight;
    bool originalItemHeightCaptured;
    HFONT originalFont;
    bool originalFontCaptured;
    LOGFONTW originalLogFont;
    bool originalLogFontCaptured;
    HFONT restoreFont;
    bool restoreFontApplied;
    HFONT customFont;
    bool customFontApplied;
    int customFontDpiY;
    int customFontSize;
    int customFontWeight;
    BOOL customFontItalic;
    BOOL customFontUnderline;
    wchar_t customFontFace[LF_FACESIZE];
};

struct StateStyle
{
    BOOL backgroundEnabled;
    int backgroundOpacity;
    int fillR;
    int fillG;
    int fillB;
    BOOL borderEnabled;
    int borderOpacity;
    int borderR;
    int borderG;
    int borderB;
    BOOL textColorEnabled;
    int textR;
    int textG;
    int textB;
    int textFontWeightMode;
    int textFontWeight;
    int textFontStyleMode;
    BOOL textFontItalic;
    BOOL textFontUnderline;
};

struct TextFontOverride
{
    int weightMode;
    int weight;
    int styleMode;
    BOOL italic;
    BOOL underline;
};

static int  g_WidthMode = 2;
static int  g_FullRowLeft = 6;
static int  g_FullRowRight = 6;
static int  g_ContentExtraLeft = 38;
static int  g_ContentExtraRight = 10;
static int  g_CornerRadius = 18;
static int  g_CommonTopInset = 2;
static int  g_CommonBottomInset = 2;

static BOOL g_ItemHeightEnabled = FALSE;
static int  g_ItemHeightValue = 46;
static BOOL g_IconTextGapEnabled = FALSE;
static int  g_IconTextGapValue = 2;
static BOOL g_IconSizeEnabled = FALSE;
static int  g_IconSizeValue = 22;
static BOOL g_HidePinButtonEnabled = FALSE;
static wchar_t g_CustomFontFace[LF_FACESIZE] = L"";
static int  g_CustomFontSize = 0;
static HFONT g_CustomTextFont = nullptr;
static int  g_CustomTextFontDpiY = 0;

static BOOL g_NormalTextEnabled = TRUE;
static int  g_NormalTextR = 210;
static int  g_NormalTextG = 210;
static int  g_NormalTextB = 210;
static int  g_NormalTextFontWeightMode = 0;
static int  g_NormalTextFontWeight = FW_NORMAL;
static int  g_NormalTextFontStyleMode = 0;
static BOOL g_NormalTextFontItalic = FALSE;
static BOOL g_NormalTextFontUnderline = FALSE;

static StateStyle g_HotStyle{};
static StateStyle g_SelectedStyle{};
static StateStyle g_SelectedNotFocusStyle{};
static StateStyle g_HotSelectedStyle{};

static decltype(&DrawThemeBackground) DrawThemeBackground_orig = nullptr;
static decltype(&DrawThemeBackgroundEx) DrawThemeBackgroundEx_orig = nullptr;
static decltype(&GetThemeColor) GetThemeColor_orig = nullptr;
static decltype(&DrawThemeTextEx) DrawThemeTextEx_orig = nullptr;
static decltype(&DrawTextW) DrawTextW_orig = nullptr;
static decltype(&DrawTextExW) DrawTextExW_orig = nullptr;
static decltype(&SendMessageW) SendMessageW_orig = nullptr;
static decltype(&CreateWindowExW) CreateWindowExW_orig = nullptr;

static HRESULT (STDCALL *CNscTree_SetStateImageList_orig)(void*, HIMAGELIST) = nullptr;

static ULONG_PTR g_GdiplusToken = 0;

static CRITICAL_SECTION g_StateLock;
static bool g_StateLockInitialized = false;
static std::vector<ExplorerState> g_States;
static std::vector<TreeMetricsState> g_MetricsStates;
static HWND g_RestartPromptWindow = nullptr;
static HANDLE g_hWorkerThread = nullptr;
static DWORD g_WorkerThreadId = 0;
static HWINEVENTHOOK g_hEventHookShow = nullptr;
static HWINEVENTHOOK g_hEventHookCreate = nullptr;
static HWINEVENTHOOK g_hEventHookHide = nullptr;
static HWINEVENTHOOK g_hEventHookDestroy = nullptr;

static thread_local bool g_HasPendingDrawTextFontOverride = false;
static thread_local TextFontOverride g_PendingDrawTextFontOverride{};

static ExplorerState* GetOrCreateState_NoLock(HWND hExplorer);
static TreeMetricsState* GetOrCreateTreeMetricsState_NoLock(HWND hTree);
static void ApplyCustomFontToTree(HWND hTree);
static void ApplyCustomFontToTrackedTrees();

static int ClampByte(int value)
{
    if (value < 0) return 0;
    if (value > 255) return 255;
    return value;
}

static Color MakeGdipColor(int a, int r, int g, int b)
{
    return Color(ClampByte(a), ClampByte(r), ClampByte(g), ClampByte(b));
}

static COLORREF MakeColorRef(int r, int g, int b)
{
    return RGB(ClampByte(r), ClampByte(g), ClampByte(b));
}

static void DestroyCustomTextFont()
{
    if (g_CustomTextFont)
    {
        DeleteObject(g_CustomTextFont);
        g_CustomTextFont = nullptr;
    }

    g_CustomTextFontDpiY = 0;
}

static int GetDpiYForWindowSafe(HWND hwnd)
{
    int dpiY = 96;

    if (hwnd && IsWindow(hwnd))
    {
        UINT dpi = GetDpiForWindow(hwnd);
        if (dpi > 0)
        {
            dpiY = (int)dpi;
        }
        else
        {
            HDC hdc = GetDC(hwnd);
            if (hdc)
            {
                int deviceDpi = GetDeviceCaps(hdc, LOGPIXELSY);
                if (deviceDpi > 0)
                    dpiY = deviceDpi;
                ReleaseDC(hwnd, hdc);
            }
        }
    }

    return dpiY;
}

static bool IsActiveCustomFontWeightMode(int mode)
{
    return mode == 1 || mode == 2;
}

static bool IsActiveCustomFontStyleMode(int mode)
{
    return mode >= 1 && mode <= 3;
}

static int FontWeightFromMode(int mode)
{
    switch (mode)
    {
        case 1:
            return 600;
        case 2:
            return FW_BOLD;
        case 0:
        default:
            return FW_NORMAL;
    }
}

static bool IsDefaultCustomFontSettings()
{
    bool defaultFace = !g_CustomFontFace[0];

    // Treat older default values from previous builds as no-op too.
    // This prevents the font from changing for users who only enable the option
    // while keeping the old default face/size stored in Windhawk settings.
    if (!defaultFace && g_CustomFontSize == 9)
    {
        defaultFace = lstrcmpiW(g_CustomFontFace, L"Segoe UI") == 0 ||
                      lstrcmpiW(g_CustomFontFace, L"Segoe UI Variable") == 0;
    }

    return defaultFace &&
           (g_CustomFontSize == 0 || g_CustomFontSize == 9);
}

static bool ShouldProcessCustomFont()
{
    if (!IsDefaultCustomFontSettings())
        return true;

    if (!g_StateLockInitialized)
        return false;

    bool hasAppliedCustomFont = false;

    EnterCriticalSection(&g_StateLock);
    for (const auto& s : g_MetricsStates)
    {
        if (s.customFontApplied)
        {
            hasAppliedCustomFont = true;
            break;
        }
    }
    LeaveCriticalSection(&g_StateLock);

    return hasAppliedCustomFont;
}

static bool GetFontLogFont(HFONT hFont, LOGFONTW* pLogFont)
{
    if (!pLogFont)
        return false;

    if (hFont && GetObjectW(hFont, sizeof(*pLogFont), pLogFont) == sizeof(*pLogFont))
        return true;

    return SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(*pLogFont), pLogFont, 0) != FALSE;
}

static HFONT CreateCustomTreeFont(HWND hTree, HFONT hBaseFont)
{
    if (IsDefaultCustomFontSettings())
        return nullptr;

    LOGFONTW lf{};
    if (!GetFontLogFont(hBaseFont, &lf))
        return nullptr;

    int dpiY = GetDpiYForWindowSafe(hTree);
    if (dpiY <= 0)
        dpiY = 96;

    if (g_CustomFontSize > 0)
        lf.lfHeight = -MulDiv(g_CustomFontSize, dpiY, 72);

    lf.lfQuality = CLEARTYPE_QUALITY;

    if (g_CustomFontFace[0])
        lstrcpynW(lf.lfFaceName, g_CustomFontFace, ARRAYSIZE(lf.lfFaceName));

    return CreateFontIndirectW(&lf);
}

static bool TreeCustomFontMatches(const TreeMetricsState* state)
{
    if (!state || !state->customFont)
        return false;

    return state->customFontSize == g_CustomFontSize &&
           lstrcmpW(state->customFontFace, g_CustomFontFace) == 0;
}

static void LoadStateStyle(StateStyle& style,
                           const wchar_t* backgroundEnabled,
                           const wchar_t* backgroundOpacity,
                           const wchar_t* fillR,
                           const wchar_t* fillG,
                           const wchar_t* fillB,
                           const wchar_t* borderEnabled,
                           const wchar_t* borderOpacity,
                           const wchar_t* borderR,
                           const wchar_t* borderG,
                           const wchar_t* borderB,
                           const wchar_t* textColorEnabled,
                           const wchar_t* textR,
                           const wchar_t* textG,
                           const wchar_t* textB,
                           const wchar_t* textWeight,
                           const wchar_t* textStyle)
{
    style.backgroundEnabled = Wh_GetIntSetting(backgroundEnabled);
    style.backgroundOpacity = Wh_GetIntSetting(backgroundOpacity);
    style.fillR = Wh_GetIntSetting(fillR);
    style.fillG = Wh_GetIntSetting(fillG);
    style.fillB = Wh_GetIntSetting(fillB);
    style.borderEnabled = Wh_GetIntSetting(borderEnabled);
    style.borderOpacity = Wh_GetIntSetting(borderOpacity);
    style.borderR = Wh_GetIntSetting(borderR);
    style.borderG = Wh_GetIntSetting(borderG);
    style.borderB = Wh_GetIntSetting(borderB);
    style.textColorEnabled = Wh_GetIntSetting(textColorEnabled);
    style.textR = Wh_GetIntSetting(textR);
    style.textG = Wh_GetIntSetting(textG);
    style.textB = Wh_GetIntSetting(textB);
    style.textFontWeightMode = Wh_GetIntSetting(textWeight);
    style.textFontWeight = FontWeightFromMode(style.textFontWeightMode);
    style.textFontStyleMode = Wh_GetIntSetting(textStyle);
    style.textFontItalic = (style.textFontStyleMode == 1 || style.textFontStyleMode == 3);
    style.textFontUnderline = (style.textFontStyleMode == 2 || style.textFontStyleMode == 3);
}

static bool ShouldSuggestExplorerRestart()
{
    FILETIME ftCreate{}, ftExit{}, ftKernel{}, ftUser{};
    if (!GetProcessTimes(GetCurrentProcess(), &ftCreate, &ftExit, &ftKernel, &ftUser))
        return false;

    FILETIME ftNow{};
    GetSystemTimeAsFileTime(&ftNow);

    ULARGE_INTEGER createTime{};
    createTime.LowPart = ftCreate.dwLowDateTime;
    createTime.HighPart = ftCreate.dwHighDateTime;

    ULARGE_INTEGER nowTime{};
    nowTime.LowPart = ftNow.dwLowDateTime;
    nowTime.HighPart = ftNow.dwHighDateTime;

    if (nowTime.QuadPart <= createTime.QuadPart)
        return false;

    const ULONGLONG elapsed100ns = nowTime.QuadPart - createTime.QuadPart;
    const ULONGLONG elapsedMs = elapsed100ns / 10000ULL;

    return elapsedMs > 15000ULL;
}

static HRESULT CALLBACK RestartPromptCallback(HWND hwnd, UINT msg, WPARAM, LPARAM, LONG_PTR)
{
    switch (msg)
    {
    case TDN_CREATED:
        g_RestartPromptWindow = hwnd;
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        break;

    case TDN_DESTROYED:
        g_RestartPromptWindow = nullptr;
        break;
    }

    return S_OK;
}

static void MaybeSuggestExplorerRestart()
{
    if (!ShouldSuggestExplorerRestart())
        return;

    TASKDIALOG_BUTTON buttons[] = {
        { 1001, L"Restart Explorer" },
        { 1002, L"Cancel" }
    };

    TASKDIALOGCONFIG config{};
    config.cbSize = sizeof(config);
    config.hwndParent = nullptr;
    config.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION | TDF_POSITION_RELATIVE_TO_WINDOW;
    config.pszWindowTitle = L"Explorer TreeItem Tweaker - Windhawk";
    config.pszContent = L"For correct color scheme operation, it is recommended to restart Explorer.";
    config.cButtons = ARRAYSIZE(buttons);
    config.pButtons = buttons;
    config.nDefaultButton = 1001;
    config.pfCallback = RestartPromptCallback;

    int pressedButton = 0;
    if (SUCCEEDED(TaskDialogIndirect(&config, &pressedButton, nullptr, nullptr)))
    {
        if (pressedButton == 1001)
        {
            STARTUPINFOW si{};
            si.cb = sizeof(si);
            PROCESS_INFORMATION pi{};

            wchar_t cmdLine[] = L"cmd.exe /c taskkill /f /im explorer.exe & start explorer.exe";
            if (CreateProcessW(
                    nullptr,
                    cmdLine,
                    nullptr,
                    nullptr,
                    FALSE,
                    CREATE_NO_WINDOW,
                    nullptr,
                    nullptr,
                    &si,
                    &pi))
            {
                CloseHandle(pi.hThread);
                CloseHandle(pi.hProcess);
            }
        }
    }
}

static void LoadSettings()
{
    g_WidthMode = Wh_GetIntSetting(L"Global.WidthMode");
    g_FullRowLeft = Wh_GetIntSetting(L"Global.OneWidthForAll.LeftMargin");
    g_FullRowRight = Wh_GetIntSetting(L"Global.OneWidthForAll.RightMargin");
    g_ContentExtraLeft = Wh_GetIntSetting(L"Global.WidthByText.ExtraLeft");
    g_ContentExtraRight = Wh_GetIntSetting(L"Global.WidthByText.ExtraRight");
    g_CornerRadius = Wh_GetIntSetting(L"Global.CornerRadius");
    g_CommonTopInset = Wh_GetIntSetting(L"Global.VerticalInsets.TopInset");
    g_CommonBottomInset = Wh_GetIntSetting(L"Global.VerticalInsets.BottomInset");

    g_ItemHeightEnabled = Wh_GetIntSetting(L"Global.ItemHeight.Enabled");
    g_ItemHeightValue = std::clamp(Wh_GetIntSetting(L"Global.ItemHeight.Value"), 16, 64);
    g_IconTextGapEnabled = Wh_GetIntSetting(L"Global.IconTextGap.Enabled");
    g_IconTextGapValue = std::clamp(Wh_GetIntSetting(L"Global.IconTextGap.Value"), 0, 32);
    g_IconSizeEnabled = Wh_GetIntSetting(L"Global.IconSize.Enabled");
    g_IconSizeValue = std::clamp(Wh_GetIntSetting(L"Global.IconSize.Value"), 8, 48);
    g_HidePinButtonEnabled = Wh_GetIntSetting(L"Global.HidePinButton.Enabled");

    PCWSTR customFontFace = Wh_GetStringSetting(L"CustomFont.FaceName");
    if (customFontFace && customFontFace[0])
        lstrcpynW(g_CustomFontFace, customFontFace, ARRAYSIZE(g_CustomFontFace));
    else
        g_CustomFontFace[0] = L'\0';
    if (customFontFace)
        Wh_FreeStringSetting(customFontFace);

    g_CustomFontSize = std::clamp(Wh_GetIntSetting(L"CustomFont.Size"), 0, 32);
    DestroyCustomTextFont();

    g_NormalTextEnabled = Wh_GetIntSetting(L"NormalText.Enabled");
    g_NormalTextR = Wh_GetIntSetting(L"NormalText.R");
    g_NormalTextG = Wh_GetIntSetting(L"NormalText.G");
    g_NormalTextB = Wh_GetIntSetting(L"NormalText.B");
    g_NormalTextFontWeightMode = Wh_GetIntSetting(L"NormalText.Weight");
    g_NormalTextFontWeight = FontWeightFromMode(g_NormalTextFontWeightMode);
    g_NormalTextFontStyleMode = Wh_GetIntSetting(L"NormalText.Style");
    g_NormalTextFontItalic = (g_NormalTextFontStyleMode == 1 || g_NormalTextFontStyleMode == 3);
    g_NormalTextFontUnderline = (g_NormalTextFontStyleMode == 2 || g_NormalTextFontStyleMode == 3);

    LoadStateStyle(g_HotStyle,
                   L"HotState.Background.Enabled", L"HotState.Background.Opacity", L"HotState.Background.R", L"HotState.Background.G", L"HotState.Background.B",
                   L"HotState.Border.Enabled", L"HotState.Border.Opacity", L"HotState.Border.R", L"HotState.Border.G", L"HotState.Border.B",
                   L"HotState.Text.Enabled", L"HotState.Text.R", L"HotState.Text.G", L"HotState.Text.B",
                   L"HotState.Text.Weight", L"HotState.Text.Style");

    LoadStateStyle(g_SelectedStyle,
                   L"SelectedState.Background.Enabled", L"SelectedState.Background.Opacity", L"SelectedState.Background.R", L"SelectedState.Background.G", L"SelectedState.Background.B",
                   L"SelectedState.Border.Enabled", L"SelectedState.Border.Opacity", L"SelectedState.Border.R", L"SelectedState.Border.G", L"SelectedState.Border.B",
                   L"SelectedState.Text.Enabled", L"SelectedState.Text.R", L"SelectedState.Text.G", L"SelectedState.Text.B",
                   L"SelectedState.Text.Weight", L"SelectedState.Text.Style");

    LoadStateStyle(g_SelectedNotFocusStyle,
                   L"SelectedNotFocusState.Background.Enabled", L"SelectedNotFocusState.Background.Opacity", L"SelectedNotFocusState.Background.R", L"SelectedNotFocusState.Background.G", L"SelectedNotFocusState.Background.B",
                   L"SelectedNotFocusState.Border.Enabled", L"SelectedNotFocusState.Border.Opacity", L"SelectedNotFocusState.Border.R", L"SelectedNotFocusState.Border.G", L"SelectedNotFocusState.Border.B",
                   L"SelectedNotFocusState.Text.Enabled", L"SelectedNotFocusState.Text.R", L"SelectedNotFocusState.Text.G", L"SelectedNotFocusState.Text.B",
                   L"SelectedNotFocusState.Text.Weight", L"SelectedNotFocusState.Text.Style");

    LoadStateStyle(g_HotSelectedStyle,
                   L"HotSelectedState.Background.Enabled", L"HotSelectedState.Background.Opacity", L"HotSelectedState.Background.R", L"HotSelectedState.Background.G", L"HotSelectedState.Background.B",
                   L"HotSelectedState.Border.Enabled", L"HotSelectedState.Border.Opacity", L"HotSelectedState.Border.R", L"HotSelectedState.Border.G", L"HotSelectedState.Border.B",
                   L"HotSelectedState.Text.Enabled", L"HotSelectedState.Text.R", L"HotSelectedState.Text.G", L"HotSelectedState.Text.B",
                   L"HotSelectedState.Text.Weight", L"HotSelectedState.Text.Style");
}

static bool IsTreeViewTheme(HTHEME hTheme)
{
    typedef HRESULT (WINAPI* GetThemeClass_t)(HTHEME, LPWSTR, int);
    static auto pGetThemeClass =
        (GetThemeClass_t)GetProcAddress(GetModuleHandleW(L"uxtheme.dll"), MAKEINTRESOURCEA(74));

    if (!pGetThemeClass || !hTheme)
        return false;

    wchar_t buf[256] = {};
    if (FAILED(pGetThemeClass(hTheme, buf, ARRAYSIZE(buf))))
        return false;

    return wcsstr(buf, L"TreeView") != nullptr;
}

static const StateStyle* GetStyleForState(INT iStateId)
{
    switch (iStateId)
    {
    case 2: return &g_HotStyle;
    case 3: return &g_SelectedStyle;
    case 5: return &g_SelectedNotFocusStyle;
    case 6: return &g_HotSelectedStyle;
    default: return nullptr;
    }
}

static bool TryGetOverrideTextColor(INT iStateId, COLORREF* pColor)
{
    if (!pColor)
        return false;

    if (iStateId == 1 && g_NormalTextEnabled)
    {
        *pColor = MakeColorRef(g_NormalTextR, g_NormalTextG, g_NormalTextB);
        return true;
    }

    const StateStyle* style = GetStyleForState(iStateId);
    if (style && style->textColorEnabled)
    {
        *pColor = MakeColorRef(style->textR, style->textG, style->textB);
        return true;
    }

    return false;
}

static bool TryGetOverrideTextFont(INT iStateId, TextFontOverride* pFont)
{
    if (!pFont)
        return false;

    if (iStateId == 1)
    {
        if (!IsActiveCustomFontWeightMode(g_NormalTextFontWeightMode) &&
            !IsActiveCustomFontStyleMode(g_NormalTextFontStyleMode))
        {
            return false;
        }

        pFont->weightMode = g_NormalTextFontWeightMode;
        pFont->weight = g_NormalTextFontWeight;
        pFont->styleMode = g_NormalTextFontStyleMode;
        pFont->italic = g_NormalTextFontItalic;
        pFont->underline = g_NormalTextFontUnderline;
        return true;
    }

    const StateStyle* style = GetStyleForState(iStateId);
    if (!style)
        return false;

    if (!IsActiveCustomFontWeightMode(style->textFontWeightMode) &&
        !IsActiveCustomFontStyleMode(style->textFontStyleMode))
    {
        return false;
    }

    pFont->weightMode = style->textFontWeightMode;
    pFont->weight = style->textFontWeight;
    pFont->styleMode = style->textFontStyleMode;
    pFont->italic = style->textFontItalic;
    pFont->underline = style->textFontUnderline;
    return true;
}

static HFONT CreateStateTextFont(HDC hdc, const TextFontOverride& fontOverride)
{
    if (!hdc)
        return nullptr;

    if (!IsActiveCustomFontWeightMode(fontOverride.weightMode) &&
        !IsActiveCustomFontStyleMode(fontOverride.styleMode))
    {
        return nullptr;
    }

    HFONT hCurrentFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);

    LOGFONTW lf{};
    if (!GetFontLogFont(hCurrentFont, &lf))
        return nullptr;

    if (IsActiveCustomFontWeightMode(fontOverride.weightMode))
        lf.lfWeight = fontOverride.weight;

    if (IsActiveCustomFontStyleMode(fontOverride.styleMode))
    {
        lf.lfItalic = fontOverride.italic;
        lf.lfUnderline = fontOverride.underline;
    }

    lf.lfQuality = CLEARTYPE_QUALITY;

    return CreateFontIndirectW(&lf);
}

static int GetStateTextExtraRightPadding(HDC hdc, const TextFontOverride& fontOverride)
{
    int dpiX = 96;
    if (hdc)
    {
        const int deviceDpiX = GetDeviceCaps(hdc, LOGPIXELSX);
        if (deviceDpiX > 0)
            dpiX = deviceDpiX;
    }

    int extraRight = 0;

    // Italic glyphs can overhang their logical text box on the right.
    // DrawText clips to the supplied rectangle, so give state-specific
    // styled text a DPI-aware safety margin.
    if (fontOverride.italic)
        extraRight += MulDiv(10, dpiX, 96);

    // Bold/Semibold can also be wider than Explorer's original text metrics.
    if (IsActiveCustomFontWeightMode(fontOverride.weightMode))
        extraRight += MulDiv(4, dpiX, 96);

    return std::max(extraRight, 0);
}

static int GetDrawTextLength(LPCWSTR pszText, INT cchText)
{
    if (!pszText)
        return 0;

    if (cchText >= 0)
        return cchText;

    return lstrlenW(pszText);
}

static int MeasureCurrentFontTextWidth(HDC hdc, LPCWSTR pszText, INT cchText)
{
    const int length = GetDrawTextLength(pszText, cchText);
    if (!hdc || !pszText || length <= 0)
        return 0;

    SIZE size{};
    if (GetTextExtentPoint32W(hdc, pszText, length, &size))
        return std::max(size.cx, 0L);

    RECT rcCalc{};
    if (DrawTextW_orig)
        DrawTextW_orig(hdc, pszText, length, &rcCalc, DT_SINGLELINE | DT_NOPREFIX | DT_CALCRECT);
    else
        DrawTextW(hdc, pszText, length, &rcCalc, DT_SINGLELINE | DT_NOPREFIX | DT_CALCRECT);

    return std::max(rcCalc.right - rcCalc.left, 0L);
}

static int GetStateTextExtraRightPaddingForText(HDC hdc,
                                                LPCWSTR pszText,
                                                INT cchText,
                                                int currentRectWidth,
                                                const TextFontOverride& fontOverride)
{
    int extraRight = GetStateTextExtraRightPadding(hdc, fontOverride);

    const int measuredWidth = MeasureCurrentFontTextWidth(hdc, pszText, cchText);
    if (measuredWidth > currentRectWidth)
        extraRight += measuredWidth - currentRectWidth;

    return std::max(extraRight, 0);
}

static RECT MakeStateTextDrawRect(HDC hdc,
                                  const RECT* pRect,
                                  DWORD textFlags,
                                  const TextFontOverride& fontOverride,
                                  LPCWSTR pszText = nullptr,
                                  INT cchText = -1)
{
    RECT rc = *pRect;

    if (!(textFlags & DT_CALCRECT))
    {
        const int currentWidth = std::max(rc.right - rc.left, 0L);
        if (pszText)
            rc.right += GetStateTextExtraRightPaddingForText(hdc, pszText, cchText, currentWidth, fontOverride);
        else
            rc.right += GetStateTextExtraRightPadding(hdc, fontOverride);
    }

    return rc;
}

static HRESULT STDCALL CNscTree_SetStateImageList_hook(void* pThis, HIMAGELIST himl)
{
    if (g_HidePinButtonEnabled)
        return S_OK;

    if (CNscTree_SetStateImageList_orig)
        return CNscTree_SetStateImageList_orig(pThis, himl);

    return S_OK;
}

static bool HookExplorerFrameSymbols()
{
    if (!g_hExplorerFrame)
        g_hExplorerFrame = LoadLibraryW(L"ExplorerFrame.dll");

    if (!g_hExplorerFrame)
    {
        Wh_Log(L"[Explorer TreeItem Tweaker] failed to load ExplorerFrame.dll");
        return true;
    }

    void** proxyAddress = nullptr;
    WindhawkUtils::SYMBOL_HOOK explorerFrameDllHooks[] = {
        {
            {
                L"public: virtual long "
                SSTDCALL
                L" CNscTree::SetStateImageList(struct _IMAGELIST *)"
            },
            &proxyAddress,
            nullptr,
            true
        }
    };

    if (!WindhawkUtils::HookSymbols(g_hExplorerFrame, explorerFrameDllHooks, ARRAYSIZE(explorerFrameDllHooks)))
    {
        Wh_Log(L"[Explorer TreeItem Tweaker] failed to resolve CNscTree::SetStateImageList");
        return true;
    }

    if (!proxyAddress)
    {
        Wh_Log(L"[Explorer TreeItem Tweaker] CNscTree::SetStateImageList was not found");
        return true;
    }

    if (!Wh_SetFunctionHook(proxyAddress,
                            (void*)CNscTree_SetStateImageList_hook,
                            (void**)&CNscTree_SetStateImageList_orig))
    {
        Wh_Log(L"[Explorer TreeItem Tweaker] failed to hook CNscTree::SetStateImageList");
    }

    return true;
}

static void ApplyPinButtonSettingToTree(HWND hTree)
{
    if (!g_HidePinButtonEnabled || !IsWindow(hTree) || !SendMessageW_orig)
        return;

    SendMessageW_orig(hTree, TVM_SETIMAGELIST, TVSIL_STATE, 0);
}

static void ApplyPinButtonSettingToTrackedTrees()
{
    if (!g_HidePinButtonEnabled)
        return;

    std::vector<HWND> trees;

    EnterCriticalSection(&g_StateLock);
    for (const auto& s : g_States)
    {
        if (IsWindow(s.hTree))
            trees.push_back(s.hTree);
    }
    LeaveCriticalSection(&g_StateLock);

    for (HWND hTree : trees)
        ApplyPinButtonSettingToTree(hTree);
}

static bool IsExplorerTopWindow(HWND hwnd)
{
    if (!IsWindow(hwnd))
        return false;
    wchar_t cls[128] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    return lstrcmpiW(cls, L"CabinetWClass") == 0 || lstrcmpiW(cls, L"ExploreWClass") == 0;
}

static HWND FindExplorerTopWindowFromChild(HWND hwnd)
{
    HWND root = GetAncestor(hwnd, GA_ROOT);
    return IsExplorerTopWindow(root) ? root : nullptr;
}

static bool IsLikelyNavTree(HWND hwnd)
{
    if (!IsWindow(hwnd))
        return false;

    wchar_t cls[128] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (lstrcmpiW(cls, WC_TREEVIEWW) != 0)
        return false;

    RECT rc{};
    if (!GetWindowRect(hwnd, &rc))
        return false;

    const int w = rc.right - rc.left;
    const int h = rc.bottom - rc.top;
    if (w < 120 || h < 200)
        return false;

    return FindExplorerTopWindowFromChild(hwnd) != nullptr;
}

static bool IsWindowOrChildOf(HWND hParent, HWND hwnd)
{
    if (!IsWindow(hParent) || !IsWindow(hwnd))
        return false;

    return hwnd == hParent || IsChild(hParent, hwnd);
}

static bool IsTreeActuallyTopVisible(HWND hTree)
{
    if (!IsWindow(hTree) || !IsWindowVisible(hTree))
        return false;

    RECT rc{};
    if (!GetWindowRect(hTree, &rc))
        return false;

    const int w = rc.right - rc.left;
    const int h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0)
        return false;

    int dx = w / 3;
    if (dx < 1)
        dx = 1;
    else if (dx > 24)
        dx = 24;

    int dy = h / 3;
    if (dy < 1)
        dy = 1;
    else if (dy > 24)
        dy = 24;

    POINT probes[] = {
        { rc.left + w / 2, rc.top + h / 2 },
        { rc.left + dx, rc.top + dy },
        { rc.left + dx, rc.bottom - dy },
        { rc.right - dx, rc.top + dy },
        { rc.right - dx, rc.bottom - dy },
    };

    for (POINT pt : probes)
    {
        if (pt.x < rc.left || pt.x >= rc.right || pt.y < rc.top || pt.y >= rc.bottom)
            continue;

        HWND hit = WindowFromPoint(pt);
        if (IsWindowOrChildOf(hTree, hit))
            return true;
    }

    return false;
}

struct FindTreeContext
{
    HWND topVisibleTree;
    HWND visibleTree;
    HWND anyTree;
};

static BOOL CALLBACK FindTreeEnumProc(HWND hwnd, LPARAM lParam)
{
    FindTreeContext* ctx = (FindTreeContext*)lParam;
    if (!ctx)
        return FALSE;

    if (IsLikelyNavTree(hwnd))
    {
        if (!ctx->anyTree)
            ctx->anyTree = hwnd;

        if (IsWindowVisible(hwnd) && !ctx->visibleTree)
            ctx->visibleTree = hwnd;

        if (IsTreeActuallyTopVisible(hwnd))
        {
            ctx->topVisibleTree = hwnd;
            return FALSE;
        }
    }

    return TRUE;
}

static HWND FindNavTreeInExplorer(HWND hExplorer)
{
    if (!IsExplorerTopWindow(hExplorer))
        return nullptr;

    FindTreeContext ctx{};
    EnumChildWindows(hExplorer, FindTreeEnumProc, (LPARAM)&ctx);

    if (ctx.topVisibleTree)
        return ctx.topVisibleTree;

    if (ctx.visibleTree)
        return ctx.visibleTree;

    return ctx.anyTree;
}

static bool AreAnyTreeMetricsEnabled()
{
    return g_ItemHeightEnabled || g_IconTextGapEnabled || g_IconSizeEnabled;
}

static bool IsEarlyNavTreeCandidate(HWND hTree)
{
    if (!IsWindow(hTree))
        return false;

    wchar_t cls[64] = {};
    if (!GetClassNameW(hTree, cls, ARRAYSIZE(cls)))
        return false;

    if (lstrcmpiW(cls, WC_TREEVIEWW) != 0)
        return false;

    return FindExplorerTopWindowFromChild(hTree) != nullptr;
}

static void RegisterTreeEarly(HWND hTree)
{
    if (!g_StateLockInitialized)
        return;

    if (!IsEarlyNavTreeCandidate(hTree))
        return;

    HWND hExplorer = FindExplorerTopWindowFromChild(hTree);
    if (!hExplorer)
        return;

    EnterCriticalSection(&g_StateLock);
    ExplorerState* s = GetOrCreateState_NoLock(hExplorer);
    if (s)
        s->hTree = hTree;
    GetOrCreateTreeMetricsState_NoLock(hTree);
    LeaveCriticalSection(&g_StateLock);

    if (!IsDefaultCustomFontSettings())
        ApplyCustomFontToTree(hTree);
}

static TreeMetricsState* FindTreeMetricsState_NoLock(HWND hTree)
{
    for (auto& s : g_MetricsStates)
    {
        if (s.hTree == hTree)
            return &s;
    }
    return nullptr;
}

static TreeMetricsState* GetOrCreateTreeMetricsState_NoLock(HWND hTree)
{
    TreeMetricsState* existing = FindTreeMetricsState_NoLock(hTree);
    if (existing)
        return existing;

    TreeMetricsState s{};
    s.hTree = hTree;
    s.origNormal = nullptr;
    s.ownedNormal = nullptr;
    s.originalItemHeight = 0;
    s.originalItemHeightCaptured = false;
    s.originalFont = nullptr;
    s.originalFontCaptured = false;
    ZeroMemory(&s.originalLogFont, sizeof(s.originalLogFont));
    s.originalLogFontCaptured = false;
    s.restoreFont = nullptr;
    s.restoreFontApplied = false;
    s.customFont = nullptr;
    s.customFontApplied = false;
    s.customFontDpiY = 0;
    s.customFontSize = 0;
    s.customFontWeight = 0;
    s.customFontItalic = FALSE;
    s.customFontUnderline = FALSE;
    s.customFontFace[0] = L'\0';
    g_MetricsStates.push_back(s);
    return &g_MetricsStates.back();
}

static bool IsTrackedTreeWindow(HWND hTree)
{
    if (!IsWindow(hTree))
        return false;

    bool match = false;
    EnterCriticalSection(&g_StateLock);
    for (const auto& s : g_States)
    {
        if (s.hTree == hTree)
        {
            match = true;
            break;
        }
    }
    LeaveCriticalSection(&g_StateLock);
    return match;
}

static HIMAGELIST BuildPaddedList(HIMAGELIST src, int cellW, int cellH, int drawW, int drawH)
{
    if (!src || cellW <= 0 || cellH <= 0 || drawW <= 0 || drawH <= 0)
        return nullptr;

    const int count = ImageList_GetImageCount(src);
    if (count <= 0)
        return nullptr;

    HIMAGELIST dst = ImageList_Create(cellW, cellH, ILC_COLOR32 | ILC_MASK, count, 0);
    if (!dst)
        return nullptr;

    HDC hScreen = GetDC(nullptr);
    HDC hMemDC = CreateCompatibleDC(hScreen);
    if (!hMemDC)
    {
        ReleaseDC(nullptr, hScreen);
        ImageList_Destroy(dst);
        return nullptr;
    }

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = cellW;
    bmi.bmiHeader.biHeight = -cellH;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    bool ok = true;
    for (int i = 0; i < count && ok; ++i)
    {
        void* bits = nullptr;
        HBITMAP hBmp = CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!hBmp)
        {
            ok = false;
            break;
        }

        HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hBmp);
        if (bits)
            ZeroMemory(bits, (size_t)cellW * (size_t)cellH * 4);

        HICON hIcon = ImageList_GetIcon(src, i, ILD_TRANSPARENT);
        if (hIcon)
        {
            DrawIconEx(hMemDC, 0, 0, hIcon, drawW, drawH, 0, nullptr, DI_NORMAL);
            DestroyIcon(hIcon);
        }
        else
        {
            ok = false;
        }

        SelectObject(hMemDC, hOldBmp);

        if (ok && ImageList_Add(dst, hBmp, nullptr) == -1)
            ok = false;

        DeleteObject(hBmp);
    }

    DeleteDC(hMemDC);
    ReleaseDC(nullptr, hScreen);

    if (!ok)
    {
        ImageList_Destroy(dst);
        return nullptr;
    }

    return dst;
}

static bool ApplyMetricsToTree(HWND hTree);
static void ReapplyMetricsToTrackedTrees();
static HWND WINAPI HookedCreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
static LRESULT WINAPI HookedSendMessageW(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static ExplorerState* GetOrCreateState_NoLock(HWND hExplorer)
{
    for (auto& s : g_States)
    {
        if (s.hExplorer == hExplorer)
            return &s;
    }
    ExplorerState s{};
    s.hExplorer = hExplorer;
    s.hTree = nullptr;
    g_States.push_back(s);
    return &g_States.back();
}

static void CleanupDeadStates()
{
    std::vector<HIMAGELIST> ownedListsToDestroy;
    std::vector<HFONT> customFontsToDestroy;
    std::vector<HFONT> restoreFontsToDestroy;

    EnterCriticalSection(&g_StateLock);

    for (size_t i = 0; i < g_States.size(); )
    {
        if (!IsWindow(g_States[i].hExplorer) || !IsWindow(g_States[i].hTree))
            g_States.erase(g_States.begin() + i);
        else
            ++i;
    }

    for (size_t i = 0; i < g_MetricsStates.size(); )
    {
        if (!IsWindow(g_MetricsStates[i].hTree))
        {
            if (g_MetricsStates[i].ownedNormal)
                ownedListsToDestroy.push_back(g_MetricsStates[i].ownedNormal);
            if (g_MetricsStates[i].customFont)
                customFontsToDestroy.push_back(g_MetricsStates[i].customFont);
            if (g_MetricsStates[i].restoreFont)
                restoreFontsToDestroy.push_back(g_MetricsStates[i].restoreFont);
            g_MetricsStates.erase(g_MetricsStates.begin() + i);
        }
        else
            ++i;
    }

    LeaveCriticalSection(&g_StateLock);

    for (HIMAGELIST hList : ownedListsToDestroy)
        ImageList_Destroy(hList);

    for (HFONT hFont : customFontsToDestroy)
        DeleteObject(hFont);

    for (HFONT hFont : restoreFontsToDestroy)
        DeleteObject(hFont);
}

static void UpdateExplorerState(HWND hExplorer)
{
    if (!IsExplorerTopWindow(hExplorer))
        return;
    HWND currentTree = FindNavTreeInExplorer(hExplorer);
    if (!currentTree)
        return;

    EnterCriticalSection(&g_StateLock);
    ExplorerState* s = GetOrCreateState_NoLock(hExplorer);
    if (s)
        s->hTree = currentTree;
    GetOrCreateTreeMetricsState_NoLock(currentTree);
    LeaveCriticalSection(&g_StateLock);

    ApplyCustomFontToTree(currentTree);

    if (AreAnyTreeMetricsEnabled())
        ApplyMetricsToTree(currentTree);

    ApplyPinButtonSettingToTree(currentTree);
}

static BOOL CALLBACK EnumTopWindowsProc(HWND hwnd, LPARAM)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId() && IsExplorerTopWindow(hwnd))
        UpdateExplorerState(hwnd);
    return TRUE;
}

static void InitialTrackAllExplorerWindows()
{
    EnumWindows(EnumTopWindowsProc, 0);
}

static void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD, HWND hwnd, LONG idObject, LONG, DWORD, DWORD)
{
    if (!hwnd || idObject != OBJID_WINDOW)
        return;

    CleanupDeadStates();

    if (IsExplorerTopWindow(hwnd))
    {
        UpdateExplorerState(hwnd);
        return;
    }

    if (IsLikelyNavTree(hwnd))
    {
        HWND hExplorer = FindExplorerTopWindowFromChild(hwnd);
        if (hExplorer)
            UpdateExplorerState(hExplorer);
        return;
    }

    HWND hExplorer = FindExplorerTopWindowFromChild(hwnd);
    if (hExplorer)
        UpdateExplorerState(hExplorer);
}

static HWND GetTrackedTreeForExplorer(HWND hExplorer)
{
    if (!IsExplorerTopWindow(hExplorer))
        return nullptr;

    HWND result = nullptr;

    EnterCriticalSection(&g_StateLock);
    for (const auto& s : g_States)
    {
        if (s.hExplorer == hExplorer && IsWindow(s.hTree))
        {
            result = s.hTree;
            break;
        }
    }
    LeaveCriticalSection(&g_StateLock);

    if (result && IsTreeActuallyTopVisible(result))
        return result;

    HWND refreshed = FindNavTreeInExplorer(hExplorer);
    if (refreshed)
    {
        UpdateExplorerState(hExplorer);
        return refreshed;
    }

    return result;
}

static HWND GetAnyVisibleTrackedTree()
{
    HWND firstVisible = nullptr;
    HWND topVisible = nullptr;

    EnterCriticalSection(&g_StateLock);
    for (const auto& s : g_States)
    {
        if (IsWindow(s.hTree) && IsWindowVisible(s.hTree))
        {
            if (!firstVisible)
                firstVisible = s.hTree;

            if (IsTreeActuallyTopVisible(s.hTree))
            {
                topVisible = s.hTree;
                break;
            }
        }
    }
    LeaveCriticalSection(&g_StateLock);

    return topVisible ? topVisible : firstVisible;
}

static HWND GetPrimaryTrackedTree()
{
    HWND result = nullptr;

    EnterCriticalSection(&g_StateLock);
    for (const auto& s : g_States)
    {
        if (IsWindow(s.hTree))
        {
            result = s.hTree;
            break;
        }
    }
    LeaveCriticalSection(&g_StateLock);

    return result;
}

static HWND GetForegroundTrackedTree()
{
    HWND hForeground = GetForegroundWindow();
    if (!hForeground)
        return nullptr;

    HWND hExplorer = hForeground;
    if (!IsExplorerTopWindow(hExplorer))
        hExplorer = FindExplorerTopWindowFromChild(hForeground);

    if (!hExplorer)
        return nullptr;

    return GetTrackedTreeForExplorer(hExplorer);
}

static HWND FindTrackedTreeUnderCursor()
{
    POINT pt{};
    if (!GetCursorPos(&pt))
        return nullptr;

    HWND result = nullptr;

    EnterCriticalSection(&g_StateLock);
    for (const auto& s : g_MetricsStates)
    {
        HWND hTree = s.hTree;
        if (!IsWindow(hTree) || !IsWindowVisible(hTree))
            continue;

        RECT rc{};
        if (!GetWindowRect(hTree, &rc))
            continue;

        if (pt.x >= rc.left && pt.x < rc.right && pt.y >= rc.top && pt.y < rc.bottom &&
            IsTreeActuallyTopVisible(hTree))
        {
            result = hTree;
            break;
        }
    }
    LeaveCriticalSection(&g_StateLock);

    return result;
}

static HWND ResolveTreeFromDrawContext(HDC hdc)
{
    if (!hdc)
        return GetAnyVisibleTrackedTree();

    HWND hwndFromDc = WindowFromDC(hdc);
    if (hwndFromDc && IsLikelyNavTree(hwndFromDc))
        return hwndFromDc;

    HWND underCursor = FindTrackedTreeUnderCursor();
    if (underCursor)
        return underCursor;

    HWND hExplorer = FindExplorerTopWindowFromChild(hwndFromDc);
    if (hExplorer)
    {
        HWND tracked = GetTrackedTreeForExplorer(hExplorer);
        if (tracked)
            return tracked;
    }

    HWND fromForeground = GetForegroundTrackedTree();
    if (fromForeground)
        return fromForeground;

    HWND anyVisible = GetAnyVisibleTrackedTree();
    if (anyVisible)
        return anyVisible;

    return GetPrimaryTrackedTree();
}

static HTREEITEM FindItemByVerticalPosition(HWND hTree, int y)
{
    if (!IsWindow(hTree))
        return nullptr;

    HTREEITEM hItem = TreeView_GetFirstVisible(hTree);
    while (hItem)
    {
        RECT rc{};
        if (TreeView_GetItemRect(hTree, hItem, &rc, TRUE))
        {
            if (y >= rc.top && y < rc.bottom)
                return hItem;
        }

        hItem = TreeView_GetNextVisible(hTree, hItem);
    }

    return nullptr;
}

static bool ItemMatchesDrawRect(HWND hTree, HTREEITEM hItem, const RECT* pRect)
{
    if (!IsWindow(hTree) || !hItem)
        return false;

    if (!pRect)
        return true;

    RECT rc{};
    if (!TreeView_GetItemRect(hTree, hItem, &rc, TRUE))
        return false;

    return !(pRect->bottom <= rc.top || pRect->top >= rc.bottom);
}

static HTREEITEM FindHotItemByCursor(HWND hTree)
{
    if (!IsWindow(hTree))
        return nullptr;

    POINT pt{};
    if (!GetCursorPos(&pt))
        return nullptr;

    RECT rcWindow{};
    if (!GetWindowRect(hTree, &rcWindow))
        return nullptr;

    if (pt.x < rcWindow.left || pt.x >= rcWindow.right || pt.y < rcWindow.top || pt.y >= rcWindow.bottom)
        return nullptr;

    if (!ScreenToClient(hTree, &pt))
        return nullptr;

    TVHITTESTINFO hti{};
    hti.pt = pt;
    TreeView_HitTest(hTree, &hti);
    return hti.hItem;
}

static HTREEITEM ResolveItemFromDrawContext(HWND hTree, INT iStateId, const RECT* pRect)
{
    if (!IsWindow(hTree))
        return nullptr;

    if (iStateId == 2)
    {
        HTREEITEM hHot = FindHotItemByCursor(hTree);
        if (ItemMatchesDrawRect(hTree, hHot, pRect))
            return hHot;
    }
    else if (iStateId == 6)
    {
        HTREEITEM hHot = FindHotItemByCursor(hTree);
        if (ItemMatchesDrawRect(hTree, hHot, pRect))
            return hHot;

        HTREEITEM hSelected = TreeView_GetSelection(hTree);
        if (ItemMatchesDrawRect(hTree, hSelected, pRect))
            return hSelected;
    }
    else if (iStateId == 3 || iStateId == 5)
    {
        HTREEITEM hSelected = TreeView_GetSelection(hTree);
        if (ItemMatchesDrawRect(hTree, hSelected, pRect))
            return hSelected;
    }

    if (pRect)
    {
        const int y = (pRect->top + pRect->bottom) / 2;
        HTREEITEM hItem = FindItemByVerticalPosition(hTree, y);
        if (hItem)
            return hItem;
    }

    return nullptr;
}

static RECT GetContentBasedRect(HDC hdc, INT iStateId, const RECT* pRect)
{
    RECT result = *pRect;

    HWND hTree = ResolveTreeFromDrawContext(hdc);
    if (!hTree)
        return result;

    HTREEITEM hItem = ResolveItemFromDrawContext(hTree, iStateId, pRect);
    if (!hItem)
        return result;

    RECT rcItem{};
    if (!TreeView_GetItemRect(hTree, hItem, &rcItem, TRUE))
        return result;

    // Width Mode = 2 should use the item's text rectangle only for horizontal
    // bounds. The vertical bounds must stay tied to the current theme draw pass
    // rect; otherwise tab changes, scrolling, or stale tree state can draw the
    // background at a wrong Y position and leave hover artifacts.
    //
    // Do not expand the background for per-state italic/bold text. State-specific
    // text can be wider than Explorer's normal metrics, but changing the
    // background width per state makes the item jump between hover/selected/normal.
    // The text draw rect gets its own clipping margin in MakeStateTextDrawRect().
    result.left = rcItem.left - g_ContentExtraLeft;
    result.right = rcItem.right + g_ContentExtraRight;
    return result;
}

static RECT MakeAdjustedRect(HDC hdc, INT iStateId, const RECT* pRect)
{
    RECT rc = *pRect;

    if (g_WidthMode == 1)
    {
        RECT clip{};
        if (GetClipBox(hdc, &clip) != ERROR)
        {
            rc.left = clip.left + g_FullRowLeft;
            rc.right = clip.right - g_FullRowRight;
        }
    }
    else if (g_WidthMode == 2)
    {
        rc = GetContentBasedRect(hdc, iStateId, pRect);
    }

    rc.top += g_CommonTopInset;
    rc.bottom -= g_CommonBottomInset;
    return rc;
}

static bool IsRectDrawable(const RECT& rc)
{
    return rc.right > rc.left && rc.bottom > rc.top;
}

static void BuildRoundedPath(GraphicsPath& path, const RECT& rc, int radius)
{
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    int r = radius;

    if (r < 0) r = 0;
    if (r * 2 > w) r = w / 2;
    if (r * 2 > h) r = h / 2;

    if (r <= 0)
    {
        path.AddRectangle(Rect(rc.left, rc.top, w, h));
        return;
    }

    const int d = r * 2;
    path.AddArc(rc.left, rc.top, d, d, 180.0f, 90.0f);
    path.AddArc(rc.right - d, rc.top, d, d, 270.0f, 90.0f);
    path.AddArc(rc.right - d, rc.bottom - d, d, d, 0.0f, 90.0f);
    path.AddArc(rc.left, rc.bottom - d, d, d, 90.0f, 90.0f);
    path.CloseFigure();
}

static void DrawStateBackgroundAA(HDC hdc, const RECT& rc, const StateStyle& style)
{
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetPixelOffsetMode(PixelOffsetModeHalf);
    g.SetCompositingQuality(CompositingQualityHighQuality);

    RECT rcDraw = rc;
    if (rcDraw.bottom > rcDraw.top)
        rcDraw.bottom -= 1;

    GraphicsPath path;
    BuildRoundedPath(path, rcDraw, g_CornerRadius);

    if (style.backgroundEnabled && style.backgroundOpacity > 0)
    {
        SolidBrush fillBrush(MakeGdipColor(style.backgroundOpacity, style.fillR, style.fillG, style.fillB));
        g.FillPath(&fillBrush, &path);
    }

    if (style.borderEnabled && style.borderOpacity > 0)
    {
        Pen borderPen(MakeGdipColor(style.borderOpacity, style.borderR, style.borderG, style.borderB), 1.0f);
        borderPen.SetAlignment(PenAlignmentInset);
        g.DrawPath(&borderPen, &path);
    }
}

static HRESULT HandleTreeDraw(HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect)
{
    if (iPartId != 1 || !pRect)
        return E_FAIL;

    const StateStyle* style = GetStyleForState(iStateId);
    if (!style)
        return E_FAIL;

    if (!style->backgroundEnabled)
        return E_FAIL;

    bool drawFill = style->backgroundOpacity > 0;
    bool drawBorder = style->borderEnabled && style->borderOpacity > 0;

    if (!drawFill && !drawBorder)
        return S_OK;

    RECT rc = MakeAdjustedRect(hdc, iStateId, pRect);
    if (!IsRectDrawable(rc))
        return E_FAIL;

    DrawStateBackgroundAA(hdc, rc, *style);
    return S_OK;
}

HRESULT WINAPI HookedDrawThemeBackground(HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect, LPCRECT pClipRect)
{
    if (IsTreeViewTheme(hTheme))
    {
        HRESULT hr = HandleTreeDraw(hdc, iPartId, iStateId, pRect);
        if (SUCCEEDED(hr))
            return S_OK;
    }
    return DrawThemeBackground_orig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
}

HRESULT WINAPI HookedDrawThemeBackgroundEx(HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect, const DTBGOPTS* pOptions)
{
    if (IsTreeViewTheme(hTheme))
    {
        HRESULT hr = HandleTreeDraw(hdc, iPartId, iStateId, pRect);
        if (SUCCEEDED(hr))
            return S_OK;
    }
    return DrawThemeBackgroundEx_orig(hTheme, hdc, iPartId, iStateId, pRect, pOptions);
}

static COLORREF GetEffectiveThemeTextColor(HTHEME hTheme,
                                         HDC hdc,
                                         INT iPartId,
                                         INT iStateId,
                                         const DTTOPTS* pOptions,
                                         bool hasTextColorOverride,
                                         COLORREF overrideColor)
{
    if (hasTextColorOverride)
        return overrideColor;

    if (pOptions && (pOptions->dwFlags & DTT_TEXTCOLOR))
        return pOptions->crText;

    COLORREF themeColor = CLR_INVALID;
    if (GetThemeColor_orig && SUCCEEDED(GetThemeColor_orig(hTheme, iPartId, iStateId, TMT_TEXTCOLOR, &themeColor)))
        return themeColor;

    return GetTextColor(hdc);
}

static HRESULT DrawTextWithStateFont(HTHEME hTheme,
                                      HDC hdc,
                                      INT iPartId,
                                      INT iStateId,
                                      LPCWSTR pszText,
                                      INT cchText,
                                      DWORD dwTextFlags,
                                      LPRECT pRect,
                                      const DTTOPTS* pOptions,
                                      const TextFontOverride& textFont,
                                      bool hasTextColorOverride,
                                      COLORREF overrideColor)
{
    if (!hdc || !pRect || !pszText)
        return E_FAIL;

    HFONT hCustomFont = CreateStateTextFont(hdc, textFont);
    if (!hCustomFont)
        return E_FAIL;

    HFONT hOldFont = (HFONT)SelectObject(hdc, hCustomFont);
    const int oldBkMode = SetBkMode(hdc, TRANSPARENT);
    const COLORREF oldTextColor = SetTextColor(
        hdc,
        GetEffectiveThemeTextColor(hTheme, hdc, iPartId, iStateId, pOptions, hasTextColorOverride, overrideColor)
    );

    RECT rcDraw = MakeStateTextDrawRect(hdc, pRect, dwTextFlags, textFont, pszText, cchText);

    if (DrawTextW_orig)
        DrawTextW_orig(hdc, pszText, cchText, &rcDraw, dwTextFlags);
    else
        DrawTextW(hdc, pszText, cchText, &rcDraw, dwTextFlags);

    SetTextColor(hdc, oldTextColor);
    SetBkMode(hdc, oldBkMode);

    if (hOldFont)
        SelectObject(hdc, hOldFont);

    DeleteObject(hCustomFont);
    return S_OK;
}

static HRESULT DrawCustomThemeText(HTHEME hTheme,
                                   HDC hdc,
                                   INT iPartId,
                                   INT iStateId,
                                   LPCWSTR pszText,
                                   INT cchText,
                                   DWORD dwTextFlags,
                                   LPRECT pRect,
                                   const DTTOPTS* pOptions)
{
    COLORREF clr = 0;
    const bool hasTextColorOverride = TryGetOverrideTextColor(iStateId, &clr);

    TextFontOverride textFont{};
    const bool hasTextFontOverride = TryGetOverrideTextFont(iStateId, &textFont);

    if (!hasTextColorOverride && !hasTextFontOverride)
        return E_FAIL;

    // DrawThemeTextEx can ignore an HFONT selected into the HDC on some Explorer paths.
    // Therefore state-specific weight/style is rendered directly with DrawTextW.
    if (hasTextFontOverride)
    {
        return DrawTextWithStateFont(hTheme,
                                     hdc,
                                     iPartId,
                                     iStateId,
                                     pszText,
                                     cchText,
                                     dwTextFlags,
                                     pRect,
                                     pOptions,
                                     textFont,
                                     hasTextColorOverride,
                                     clr);
    }

    DTTOPTS opts{};
    const DTTOPTS* pDrawOptions = pOptions;

    if (hasTextColorOverride)
    {
        if (pOptions)
            opts = *pOptions;
        else
            opts.dwSize = sizeof(opts);

        opts.dwSize = sizeof(opts);
        opts.dwFlags |= DTT_TEXTCOLOR;
        opts.crText = clr;
        pDrawOptions = &opts;
    }

    return DrawThemeTextEx_orig(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, pRect, pDrawOptions);
}

static void QueueDrawTextFontOverride(INT iStateId)
{
    TextFontOverride textFont{};
    if (TryGetOverrideTextFont(iStateId, &textFont))
    {
        g_PendingDrawTextFontOverride = textFont;
        g_HasPendingDrawTextFontOverride = true;
    }
}

static bool TakePendingDrawTextFontOverride(TextFontOverride* pFont)
{
    if (!pFont || !g_HasPendingDrawTextFontOverride)
        return false;

    *pFont = g_PendingDrawTextFontOverride;
    g_HasPendingDrawTextFontOverride = false;
    return true;
}

static HFONT SelectStateFontForDrawText(HDC hdc, const TextFontOverride& textFont)
{
    HFONT hCustomFont = CreateStateTextFont(hdc, textFont);
    if (!hCustomFont)
        return nullptr;

    SelectObject(hdc, hCustomFont);
    return hCustomFont;
}

int WINAPI HookedDrawTextW(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format)
{
    TextFontOverride textFont{};
    if (!TakePendingDrawTextFontOverride(&textFont))
        return DrawTextW_orig(hdc, lpchText, cchText, lprc, format);

    HFONT hOldFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
    HFONT hCustomFont = SelectStateFontForDrawText(hdc, textFont);

    RECT rcDraw{};
    LPRECT pDrawRect = lprc;
    if (lprc && !(format & DT_CALCRECT))
    {
        rcDraw = MakeStateTextDrawRect(hdc, lprc, format, textFont, lpchText, cchText);
        pDrawRect = &rcDraw;
    }

    int result = DrawTextW_orig(hdc, lpchText, cchText, pDrawRect, format);

    if (hOldFont)
        SelectObject(hdc, hOldFont);

    if (hCustomFont)
        DeleteObject(hCustomFont);

    return result;
}

int WINAPI HookedDrawTextExW(HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT format, LPDRAWTEXTPARAMS lpdtp)
{
    TextFontOverride textFont{};
    if (!TakePendingDrawTextFontOverride(&textFont))
        return DrawTextExW_orig(hdc, lpchText, cchText, lprc, format, lpdtp);

    HFONT hOldFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
    HFONT hCustomFont = SelectStateFontForDrawText(hdc, textFont);

    RECT rcDraw{};
    LPRECT pDrawRect = lprc;
    if (lprc && !(format & DT_CALCRECT))
    {
        rcDraw = MakeStateTextDrawRect(hdc, lprc, format, textFont, lpchText, cchText);
        pDrawRect = &rcDraw;
    }

    int result = DrawTextExW_orig(hdc, lpchText, cchText, pDrawRect, format, lpdtp);

    if (hOldFont)
        SelectObject(hdc, hOldFont);

    if (hCustomFont)
        DeleteObject(hCustomFont);

    return result;
}

HRESULT WINAPI HookedGetThemeColor(HTHEME hTheme,
                                   int iPartId,
                                   int iStateId,
                                   int iPropId,
                                   COLORREF* pColor)
{
    HRESULT hr = GetThemeColor_orig(hTheme, iPartId, iStateId, iPropId, pColor);

    if (!pColor || iPropId != TMT_TEXTCOLOR)
        return hr;

    if (!IsTreeViewTheme(hTheme) || iPartId != 1)
        return hr;

    QueueDrawTextFontOverride(iStateId);

    COLORREF clr = 0;
    if (TryGetOverrideTextColor(iStateId, &clr))
    {
        *pColor = clr;
        return S_OK;
    }

    return hr;
}

HRESULT WINAPI HookedDrawThemeTextEx(HTHEME hTheme,
                                     HDC hdc,
                                     INT iPartId,
                                     INT iStateId,
                                     LPCWSTR pszText,
                                     INT cchText,
                                     DWORD dwFlags,
                                     LPRECT pRect,
                                     const DTTOPTS* pOptions)
{
    if (IsTreeViewTheme(hTheme) && iPartId == 1)
    {
        HRESULT hr = DrawCustomThemeText(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwFlags, pRect, pOptions);
        if (SUCCEEDED(hr))
            return hr;
    }

    return DrawThemeTextEx_orig(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwFlags, pRect, pOptions);
}


static void ApplyCustomFontToTree(HWND hTree)
{
    if (!IsWindow(hTree))
        return;

    const bool wantsCustomFont = !IsDefaultCustomFontSettings();

    HFONT currentFont = (HFONT)SendMessageW_orig(hTree, WM_GETFONT, 0, 0);
    HFONT baseFont = currentFont;
    HFONT fontToApply = nullptr;
    HFONT fontToDestroy = nullptr;
    HFONT restoreFontToDestroy = nullptr;
    LOGFONTW restoreLogFont{};
    bool needCreate = false;
    bool needCreateRestoreFont = false;
    bool shouldSendFont = false;
    bool shouldRedraw = false;

    EnterCriticalSection(&g_StateLock);
    TreeMetricsState* state = GetOrCreateTreeMetricsState_NoLock(hTree);
    if (state)
    {
        const bool currentIsCustomFont = state->customFont && currentFont == state->customFont;
        const bool currentIsRestoreFont = state->restoreFont && currentFont == state->restoreFont;
        const bool currentLooksNative = currentFont && !currentIsCustomFont && !currentIsRestoreFont;

        // Capture Explorer's native font as data, not only as an HFONT handle.
        // The handle can become stale after Explorer/theme refreshes, while a
        // LOGFONT snapshot lets us create a fresh default-looking font when the
        // custom settings are returned to 0.
        if (currentLooksNative)
        {
            if (!state->originalFontCaptured)
            {
                state->originalFont = currentFont;
                state->originalFontCaptured = true;
            }

            if (!state->originalLogFontCaptured &&
                GetFontLogFont(currentFont, &state->originalLogFont))
            {
                state->originalLogFontCaptured = true;
            }
        }

        if (state->originalFontCaptured)
            baseFont = state->originalFont;
        if (state->restoreFontApplied && state->restoreFont)
            baseFont = state->restoreFont;

        if (wantsCustomFont)
        {
            if (TreeCustomFontMatches(state))
            {
                fontToApply = state->customFont;
                shouldSendFont = currentFont != fontToApply;
            }
            else
            {
                needCreate = true;
            }
        }
        else
        {
            // All font controls are back to native/no-op values. If this mod
            // previously applied a custom font, replace it with a fresh copy of
            // the captured native LOGFONT instead of reusing a possibly stale
            // HFONT handle from Explorer.
            if (state->customFontApplied)
            {
                fontToDestroy = state->customFont;
                state->customFont = nullptr;
                state->customFontApplied = false;
                state->customFontDpiY = 0;
                state->customFontSize = 0;
                state->customFontWeight = 0;
                state->customFontItalic = FALSE;
                state->customFontUnderline = FALSE;
                state->customFontFace[0] = L'\0';

                if (state->originalLogFontCaptured)
                {
                    restoreLogFont = state->originalLogFont;
                    needCreateRestoreFont = true;
                }
                else if (state->originalFontCaptured)
                {
                    fontToApply = state->originalFont;
                    shouldSendFont = currentFont != fontToApply;
                }
            }
            else if (state->restoreFontApplied && state->restoreFont)
            {
                fontToApply = state->restoreFont;
                shouldSendFont = currentFont != fontToApply;
            }
        }
    }
    LeaveCriticalSection(&g_StateLock);

    if (needCreate)
    {
        HFONT newFont = CreateCustomTreeFont(hTree, baseFont);
        if (!newFont)
            return;

        SendMessageW_orig(hTree, WM_SETFONT, (WPARAM)newFont, TRUE);
        shouldRedraw = true;

        EnterCriticalSection(&g_StateLock);
        TreeMetricsState* state = GetOrCreateTreeMetricsState_NoLock(hTree);
        if (state)
        {
            fontToDestroy = state->customFont;
            restoreFontToDestroy = state->restoreFont;
            state->restoreFont = nullptr;
            state->restoreFontApplied = false;
            state->customFont = newFont;
            state->customFontApplied = true;
            state->customFontDpiY = GetDpiYForWindowSafe(hTree);
            state->customFontSize = g_CustomFontSize;
            state->customFontWeight = 0;
            state->customFontItalic = FALSE;
            state->customFontUnderline = FALSE;
            lstrcpynW(state->customFontFace, g_CustomFontFace, ARRAYSIZE(state->customFontFace));
        }
        LeaveCriticalSection(&g_StateLock);

        if (fontToDestroy && fontToDestroy != newFont)
            DeleteObject(fontToDestroy);
        if (restoreFontToDestroy && restoreFontToDestroy != newFont)
            DeleteObject(restoreFontToDestroy);
    }
    else if (needCreateRestoreFont)
    {
        HFONT newRestoreFont = CreateFontIndirectW(&restoreLogFont);
        if (!newRestoreFont)
            return;

        SendMessageW_orig(hTree, WM_SETFONT, (WPARAM)newRestoreFont, TRUE);
        shouldRedraw = true;

        EnterCriticalSection(&g_StateLock);
        TreeMetricsState* state = GetOrCreateTreeMetricsState_NoLock(hTree);
        if (state)
        {
            restoreFontToDestroy = state->restoreFont;
            state->restoreFont = newRestoreFont;
            state->restoreFontApplied = true;
        }
        LeaveCriticalSection(&g_StateLock);

        if (restoreFontToDestroy && restoreFontToDestroy != newRestoreFont)
            DeleteObject(restoreFontToDestroy);
    }
    else if (shouldSendFont)
    {
        SendMessageW_orig(hTree, WM_SETFONT, (WPARAM)fontToApply, TRUE);
        shouldRedraw = true;
    }

    if (fontToDestroy && !needCreate)
        DeleteObject(fontToDestroy);

    if (shouldRedraw)
    {
        InvalidateRect(hTree, nullptr, TRUE);
        RedrawWindow(hTree, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    }
}

static void ApplyCustomFontToTrackedTrees()
{
    if (!ShouldProcessCustomFont())
        return;

    std::vector<HWND> trees;

    EnterCriticalSection(&g_StateLock);
    for (const auto& s : g_States)
    {
        if (IsWindow(s.hTree))
            trees.push_back(s.hTree);
    }
    LeaveCriticalSection(&g_StateLock);

    for (HWND hTree : trees)
        ApplyCustomFontToTree(hTree);
}

static bool ApplyMetricsToTree(HWND hTree)
{
    if (!IsWindow(hTree))
        return false;

    const bool heightEnabled = !!g_ItemHeightEnabled;
    const bool iconMetricsEnabled = !!(g_IconTextGapEnabled || g_IconSizeEnabled);

    HIMAGELIST sourceNormal = nullptr;
    HIMAGELIST previousOwned = nullptr;
    int originalItemHeight = 0;
    bool originalItemHeightCaptured = false;

    EnterCriticalSection(&g_StateLock);
    TreeMetricsState* state = GetOrCreateTreeMetricsState_NoLock(hTree);
    if (state)
    {
        sourceNormal = state->origNormal;
        previousOwned = state->ownedNormal;
        originalItemHeight = state->originalItemHeight;
        originalItemHeightCaptured = state->originalItemHeightCaptured;
    }
    LeaveCriticalSection(&g_StateLock);

    if (!originalItemHeightCaptured)
    {
        originalItemHeight = (int)SendMessageW_orig(hTree, TVM_GETITEMHEIGHT, 0, 0);
        EnterCriticalSection(&g_StateLock);
        TreeMetricsState* state = GetOrCreateTreeMetricsState_NoLock(hTree);
        if (state && !state->originalItemHeightCaptured)
        {
            state->originalItemHeight = originalItemHeight;
            state->originalItemHeightCaptured = true;
        }
        LeaveCriticalSection(&g_StateLock);
    }

    if (heightEnabled)
        SendMessageW_orig(hTree, TVM_SETITEMHEIGHT, (WPARAM)g_ItemHeightValue, 0);
    else if (originalItemHeight > 0)
        SendMessageW_orig(hTree, TVM_SETITEMHEIGHT, (WPARAM)originalItemHeight, 0);

    if (iconMetricsEnabled)
    {
        HIMAGELIST currentNormal = (HIMAGELIST)SendMessageW_orig(hTree, TVM_GETIMAGELIST, TVSIL_NORMAL, 0);
        if (currentNormal && currentNormal != previousOwned)
        {
            sourceNormal = currentNormal;
            EnterCriticalSection(&g_StateLock);
            TreeMetricsState* state = GetOrCreateTreeMetricsState_NoLock(hTree);
            if (state)
                state->origNormal = currentNormal;
            LeaveCriticalSection(&g_StateLock);
        }

        if (sourceNormal)
        {
            int origW = 0;
            int origH = 0;
            ImageList_GetIconSize(sourceNormal, &origW, &origH);
            if (origW > 0 && origH > 0)
            {
                const int drawW = g_IconSizeEnabled ? g_IconSizeValue : origW;
                const int drawH = g_IconSizeEnabled ? g_IconSizeValue : origH;
                const int cellW = drawW + (g_IconTextGapEnabled ? g_IconTextGapValue : 0);
                const int cellH = std::max(drawH, origH);

                HIMAGELIST built = BuildPaddedList(sourceNormal, cellW, cellH, drawW, drawH);
                if (built)
                {
                    SendMessageW_orig(hTree, TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM)built);

                    HIMAGELIST ownedToDestroy = nullptr;
                    EnterCriticalSection(&g_StateLock);
                    TreeMetricsState* state = GetOrCreateTreeMetricsState_NoLock(hTree);
                    if (state)
                    {
                        ownedToDestroy = state->ownedNormal;
                        state->ownedNormal = built;
                    }
                    LeaveCriticalSection(&g_StateLock);

                    if (ownedToDestroy && ownedToDestroy != built)
                        ImageList_Destroy(ownedToDestroy);
                }
            }
        }
    }
    else
    {
        HIMAGELIST origNormal = nullptr;
        HIMAGELIST ownedNormal = nullptr;

        EnterCriticalSection(&g_StateLock);
        TreeMetricsState* state = FindTreeMetricsState_NoLock(hTree);
        if (state)
        {
            origNormal = state->origNormal;
            ownedNormal = state->ownedNormal;
            state->ownedNormal = nullptr;
        }
        LeaveCriticalSection(&g_StateLock);

        if (origNormal && ownedNormal)
        {
            SendMessageW_orig(hTree, TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM)origNormal);
            ImageList_Destroy(ownedNormal);
        }
    }

    InvalidateRect(hTree, nullptr, TRUE);
    RedrawWindow(hTree, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    return true;
}

static void ReapplyMetricsToTrackedTrees()
{
    std::vector<HWND> trees;

    EnterCriticalSection(&g_StateLock);
    for (const auto& s : g_States)
    {
        if (IsWindow(s.hTree))
            trees.push_back(s.hTree);
    }
    LeaveCriticalSection(&g_StateLock);

    for (HWND hTree : trees)
        ApplyMetricsToTree(hTree);
}

static HWND WINAPI HookedCreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hwnd = CreateWindowExW_orig(dwExStyle, lpClassName, lpWindowName,
                                     dwStyle, X, Y, nWidth, nHeight,
                                     hWndParent, hMenu, hInstance, lpParam);

    if (hwnd && lpClassName && !IS_INTRESOURCE(lpClassName) &&
        lstrcmpiW(lpClassName, WC_TREEVIEWW) == 0)
    {
        RegisterTreeEarly(hwnd);
    }

    return hwnd;
}

static LRESULT WINAPI HookedSendMessageW(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (!IsTrackedTreeWindow(hwnd))
        return SendMessageW_orig(hwnd, msg, wParam, lParam);

    if (msg == TVM_SETITEMHEIGHT)
    {
        if (!g_ItemHeightEnabled)
            return SendMessageW_orig(hwnd, msg, wParam, lParam);

        return SendMessageW_orig(hwnd, TVM_SETITEMHEIGHT, (WPARAM)g_ItemHeightValue, lParam);
    }

    if (msg == TVM_SETIMAGELIST && wParam == TVSIL_NORMAL)
    {
        HIMAGELIST srcList = (HIMAGELIST)lParam;
        if (!srcList)
            return SendMessageW_orig(hwnd, msg, wParam, lParam);

        HIMAGELIST ownedNormal = nullptr;
        EnterCriticalSection(&g_StateLock);
        TreeMetricsState* state = GetOrCreateTreeMetricsState_NoLock(hwnd);
        if (state)
        {
            ownedNormal = state->ownedNormal;
            if (srcList != state->ownedNormal)
                state->origNormal = srcList;
        }
        LeaveCriticalSection(&g_StateLock);

        if (srcList == ownedNormal)
            return SendMessageW_orig(hwnd, msg, wParam, lParam);

        if (!(g_IconTextGapEnabled || g_IconSizeEnabled))
            return SendMessageW_orig(hwnd, msg, wParam, lParam);

        int origW = 0;
        int origH = 0;
        ImageList_GetIconSize(srcList, &origW, &origH);
        if (origW <= 0 || origH <= 0)
            return SendMessageW_orig(hwnd, msg, wParam, lParam);

        const int drawW = g_IconSizeEnabled ? g_IconSizeValue : origW;
        const int drawH = g_IconSizeEnabled ? g_IconSizeValue : origH;
        const int cellW = drawW + (g_IconTextGapEnabled ? g_IconTextGapValue : 0);
        const int cellH = std::max(drawH, origH);

        HIMAGELIST built = BuildPaddedList(srcList, cellW, cellH, drawW, drawH);
        if (!built)
            return SendMessageW_orig(hwnd, msg, wParam, lParam);

        HIMAGELIST prevOwned = nullptr;
        EnterCriticalSection(&g_StateLock);
        TreeMetricsState* state2 = GetOrCreateTreeMetricsState_NoLock(hwnd);
        if (state2)
        {
            prevOwned = state2->ownedNormal;
            state2->ownedNormal = built;
        }
        LeaveCriticalSection(&g_StateLock);

        SendMessageW_orig(hwnd, msg, wParam, (LPARAM)built);
        if (prevOwned && prevOwned != built)
            ImageList_Destroy(prevOwned);
        return (LRESULT)nullptr;
    }

    return SendMessageW_orig(hwnd, msg, wParam, lParam);
}

static void RefreshTrackedTrees()
{
    std::vector<HWND> trees;
    std::vector<HWND> explorers;

    EnterCriticalSection(&g_StateLock);
    for (const auto& s : g_States)
    {
        if (IsWindow(s.hTree))
            trees.push_back(s.hTree);
        if (IsWindow(s.hExplorer))
            explorers.push_back(s.hExplorer);
    }
    LeaveCriticalSection(&g_StateLock);

    if (ShouldProcessCustomFont())
    {
        for (HWND hTree : trees)
            ApplyCustomFontToTree(hTree);
    }

    for (HWND hExplorer : explorers)
    {
        SendMessageW(hExplorer, WM_THEMECHANGED, 0, 0);
        InvalidateRect(hExplorer, nullptr, TRUE);
        RedrawWindow(hExplorer, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);
    }

    for (HWND hTree : trees)
    {
        SendMessageW(hTree, WM_THEMECHANGED, 0, 0);
        InvalidateRect(hTree, nullptr, TRUE);
        RedrawWindow(hTree, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);
    }

    ReapplyMetricsToTrackedTrees();
    ApplyPinButtonSettingToTrackedTrees();
}

static DWORD WINAPI WorkerThreadProc(LPVOID)
{
    InitialTrackAllExplorerWindows();
    ApplyCustomFontToTrackedTrees();
    if (AreAnyTreeMetricsEnabled())
        ReapplyMetricsToTrackedTrees();
    ApplyPinButtonSettingToTrackedTrees();

    DWORD pid = GetCurrentProcessId();
    g_hEventHookShow = SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW, nullptr, WinEventProc, pid, 0, WINEVENT_OUTOFCONTEXT);
    g_hEventHookCreate = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_CREATE, nullptr, WinEventProc, pid, 0, WINEVENT_OUTOFCONTEXT);
    g_hEventHookHide = SetWinEventHook(EVENT_OBJECT_HIDE, EVENT_OBJECT_HIDE, nullptr, WinEventProc, pid, 0, WINEVENT_OUTOFCONTEXT);
    g_hEventHookDestroy = SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY, nullptr, WinEventProc, pid, 0, WINEVENT_OUTOFCONTEXT);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hEventHookShow)
    {
        UnhookWinEvent(g_hEventHookShow);
        g_hEventHookShow = nullptr;
    }
    if (g_hEventHookCreate)
    {
        UnhookWinEvent(g_hEventHookCreate);
        g_hEventHookCreate = nullptr;
    }
    if (g_hEventHookHide)
    {
        UnhookWinEvent(g_hEventHookHide);
        g_hEventHookHide = nullptr;
    }
    if (g_hEventHookDestroy)
    {
        UnhookWinEvent(g_hEventHookDestroy);
        g_hEventHookDestroy = nullptr;
    }

    std::vector<HIMAGELIST> ownedListsToDestroy;
    std::vector<HFONT> customFontsToDestroy;
    std::vector<HFONT> restoreFontsToDestroy;
    std::vector<std::pair<HWND, HFONT>> fontsToRestore;

    EnterCriticalSection(&g_StateLock);
    for (auto& s : g_MetricsStates)
    {
        if (s.ownedNormal)
            ownedListsToDestroy.push_back(s.ownedNormal);
        if (s.customFont)
            customFontsToDestroy.push_back(s.customFont);
        if (s.restoreFont)
            restoreFontsToDestroy.push_back(s.restoreFont);
        if ((s.customFontApplied || s.restoreFontApplied) && s.originalFontCaptured && IsWindow(s.hTree))
            fontsToRestore.push_back({s.hTree, s.originalFont});
    }
    g_MetricsStates.clear();
    LeaveCriticalSection(&g_StateLock);

    for (const auto& restore : fontsToRestore)
        SendMessageW_orig(restore.first, WM_SETFONT, (WPARAM)restore.second, TRUE);

    for (HIMAGELIST hList : ownedListsToDestroy)
        ImageList_Destroy(hList);

    for (HFONT hFont : customFontsToDestroy)
        DeleteObject(hFont);

    for (HFONT hFont : restoreFontsToDestroy)
        DeleteObject(hFont);

    if (g_StateLockInitialized)
    {
        DeleteCriticalSection(&g_StateLock);
        g_StateLockInitialized = false;
    }
    return 0;
}

BOOL Wh_ModInit()
{
    InitializeCriticalSection(&g_StateLock);
    g_StateLockInitialized = true;

    LoadSettings();
    MaybeSuggestExplorerRestart();

    HookExplorerFrameSymbols();

    GdiplusStartupInput gdiplusStartupInput;
    if (GdiplusStartup(&g_GdiplusToken, &gdiplusStartupInput, nullptr) != Ok)
    {
        Wh_Log(L"[Explorer TreeItem Tweaker] GdiplusStartup failed");
        if (g_StateLockInitialized)
        {
            DeleteCriticalSection(&g_StateLock);
            g_StateLockInitialized = false;
        }
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(DrawThemeBackground, HookedDrawThemeBackground, &DrawThemeBackground_orig))
    {
        Wh_Log(L"[Explorer TreeItem Tweaker] failed to hook DrawThemeBackground");
        if (g_StateLockInitialized)
        {
            DeleteCriticalSection(&g_StateLock);
            g_StateLockInitialized = false;
        }
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(DrawThemeBackgroundEx, HookedDrawThemeBackgroundEx, &DrawThemeBackgroundEx_orig))
    {
        Wh_Log(L"[Explorer TreeItem Tweaker] failed to hook DrawThemeBackgroundEx");
        if (g_StateLockInitialized)
        {
            DeleteCriticalSection(&g_StateLock);
            g_StateLockInitialized = false;
        }
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(GetThemeColor, HookedGetThemeColor, &GetThemeColor_orig))
    {
        Wh_Log(L"[Explorer TreeItem Tweaker] failed to hook GetThemeColor");
        if (g_StateLockInitialized)
        {
            DeleteCriticalSection(&g_StateLock);
            g_StateLockInitialized = false;
        }
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(DrawThemeTextEx, HookedDrawThemeTextEx, &DrawThemeTextEx_orig))
    {
        Wh_Log(L"[Explorer TreeItem Tweaker] failed to hook DrawThemeTextEx");
        if (g_StateLockInitialized)
        {
            DeleteCriticalSection(&g_StateLock);
            g_StateLockInitialized = false;
        }
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(DrawTextW, HookedDrawTextW, &DrawTextW_orig))
        Wh_Log(L"[Explorer TreeItem Tweaker] failed to hook DrawTextW");

    if (!WindhawkUtils::SetFunctionHook(DrawTextExW, HookedDrawTextExW, &DrawTextExW_orig))
        Wh_Log(L"[Explorer TreeItem Tweaker] failed to hook DrawTextExW");

    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, HookedCreateWindowExW, &CreateWindowExW_orig))
    {
        Wh_Log(L"[Explorer TreeItem Tweaker] failed to hook CreateWindowExW");
        if (g_StateLockInitialized)
        {
            DeleteCriticalSection(&g_StateLock);
            g_StateLockInitialized = false;
        }
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(SendMessageW, HookedSendMessageW, &SendMessageW_orig))
    {
        Wh_Log(L"[Explorer TreeItem Tweaker] failed to hook SendMessageW");
        if (g_StateLockInitialized)
        {
            DeleteCriticalSection(&g_StateLock);
            g_StateLockInitialized = false;
        }
        return FALSE;
    }

    g_hWorkerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, &g_WorkerThreadId);
    if (!g_hWorkerThread)
    {
        Wh_Log(L"[Explorer TreeItem Tweaker] failed to create worker thread");
        if (g_StateLockInitialized)
        {
            DeleteCriticalSection(&g_StateLock);
            g_StateLockInitialized = false;
        }
        return FALSE;
    }

    Wh_Log(L"[Explorer TreeItem Tweaker] mod initialized");
    return TRUE;
}

void Wh_ModUninit()
{
    if (g_hWorkerThread)
    {
        g_ItemHeightEnabled = FALSE;
        g_IconTextGapEnabled = FALSE;
        g_IconSizeEnabled = FALSE;
        g_CustomFontFace[0] = L'\0';
        g_CustomFontSize = 0;
        ApplyCustomFontToTrackedTrees();
        ReapplyMetricsToTrackedTrees();
    }

    if (g_WorkerThreadId)
        PostThreadMessageW(g_WorkerThreadId, WM_QUIT, 0, 0);

    if (g_hWorkerThread)
    {
        WaitForSingleObject(g_hWorkerThread, 3000);
        CloseHandle(g_hWorkerThread);
        g_hWorkerThread = nullptr;
    }

    g_WorkerThreadId = 0;

    if (g_hExplorerFrame)
    {
        FreeLibrary(g_hExplorerFrame);
        g_hExplorerFrame = nullptr;
    }

    DestroyCustomTextFont();

    if (g_GdiplusToken)
    {
        GdiplusShutdown(g_GdiplusToken);
        g_GdiplusToken = 0;
    }
}

void Wh_ModSettingsChanged()
{
    LoadSettings();
    RefreshTrackedTrees();
}
