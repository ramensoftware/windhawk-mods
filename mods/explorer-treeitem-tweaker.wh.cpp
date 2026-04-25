// ==WindhawkMod==
// @id              explorer-treeitem-tweaker
// @name            Explorer TreeItem Tweaker
// @description     Custom backgrounds and text colors for Explorer TreeView
// @version         1.0.0
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

## ⚠️ Important: dark theme defaults

**The default settings are designed for the dark Windows system theme.**

For a correct and clean appearance on the light Windows theme, adjust the mod settings manually, especially the text color.

For example, for light theme text, try:

`R = 0, G = 0, B = 0`

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

## Recommended setup
For the best Explorer navigation pane experience, it is recommended to use this mod together with **Explorer TreeLine Killer** and **Explorer Navigation Pane Tweaks**.

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

## Layout controls
- **Item Height** increases or decreases the total height of each navigation row
- **Icon-Text Gap** controls the horizontal spacing between the icon and the text
- **Icon Size** controls the width and height of navigation icons

Each option has its own enable switch.
When a switch is off, that option is not applied.

## Before / After
**Before**

![Before](https://raw.githubusercontent.com/Languster/windhawk-mods/main/mods_assets/explorer-treeitem-tweaker-before.jpg)

**After**

![After](https://raw.githubusercontent.com/Languster/windhawk-mods/main/mods_assets/explorer-treeitem-tweaker-after.jpg)

## Credits
Thanks to [crazyboyybs](https://github.com/crazyboyybs) for extending the functionality.
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
    $description: Choose how item width is calculated. 0 = system, 1 = one width for all, 2 = width by text
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
  $name: Global

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

#ifndef TMT_TEXTCOLOR
#define TMT_TEXTCOLOR 3803
#endif

using namespace Gdiplus;

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

static BOOL g_NormalTextEnabled = TRUE;
static int  g_NormalTextR = 210;
static int  g_NormalTextG = 210;
static int  g_NormalTextB = 210;

static StateStyle g_HotStyle{};
static StateStyle g_SelectedStyle{};
static StateStyle g_SelectedNotFocusStyle{};
static StateStyle g_HotSelectedStyle{};

static decltype(&DrawThemeBackground) DrawThemeBackground_orig = nullptr;
static decltype(&DrawThemeBackgroundEx) DrawThemeBackgroundEx_orig = nullptr;
static decltype(&GetThemeColor) GetThemeColor_orig = nullptr;
static decltype(&DrawThemeTextEx) DrawThemeTextEx_orig = nullptr;
static decltype(&SendMessageW) SendMessageW_orig = nullptr;
static decltype(&CreateWindowExW) CreateWindowExW_orig = nullptr;

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

static ExplorerState* GetOrCreateState_NoLock(HWND hExplorer);
static TreeMetricsState* GetOrCreateTreeMetricsState_NoLock(HWND hTree);

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
                           const wchar_t* textB)
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

    g_NormalTextEnabled = Wh_GetIntSetting(L"NormalText.Enabled");
    g_NormalTextR = Wh_GetIntSetting(L"NormalText.R");
    g_NormalTextG = Wh_GetIntSetting(L"NormalText.G");
    g_NormalTextB = Wh_GetIntSetting(L"NormalText.B");

    LoadStateStyle(g_HotStyle,
                   L"HotState.Background.Enabled", L"HotState.Background.Opacity", L"HotState.Background.R", L"HotState.Background.G", L"HotState.Background.B",
                   L"HotState.Border.Enabled", L"HotState.Border.Opacity", L"HotState.Border.R", L"HotState.Border.G", L"HotState.Border.B",
                   L"HotState.Text.Enabled", L"HotState.Text.R", L"HotState.Text.G", L"HotState.Text.B");

    LoadStateStyle(g_SelectedStyle,
                   L"SelectedState.Background.Enabled", L"SelectedState.Background.Opacity", L"SelectedState.Background.R", L"SelectedState.Background.G", L"SelectedState.Background.B",
                   L"SelectedState.Border.Enabled", L"SelectedState.Border.Opacity", L"SelectedState.Border.R", L"SelectedState.Border.G", L"SelectedState.Border.B",
                   L"SelectedState.Text.Enabled", L"SelectedState.Text.R", L"SelectedState.Text.G", L"SelectedState.Text.B");

    LoadStateStyle(g_SelectedNotFocusStyle,
                   L"SelectedNotFocusState.Background.Enabled", L"SelectedNotFocusState.Background.Opacity", L"SelectedNotFocusState.Background.R", L"SelectedNotFocusState.Background.G", L"SelectedNotFocusState.Background.B",
                   L"SelectedNotFocusState.Border.Enabled", L"SelectedNotFocusState.Border.Opacity", L"SelectedNotFocusState.Border.R", L"SelectedNotFocusState.Border.G", L"SelectedNotFocusState.Border.B",
                   L"SelectedNotFocusState.Text.Enabled", L"SelectedNotFocusState.Text.R", L"SelectedNotFocusState.Text.G", L"SelectedNotFocusState.Text.B");

    LoadStateStyle(g_HotSelectedStyle,
                   L"HotSelectedState.Background.Enabled", L"HotSelectedState.Background.Opacity", L"HotSelectedState.Background.R", L"HotSelectedState.Background.G", L"HotSelectedState.Background.B",
                   L"HotSelectedState.Border.Enabled", L"HotSelectedState.Border.Opacity", L"HotSelectedState.Border.R", L"HotSelectedState.Border.G", L"HotSelectedState.Border.B",
                   L"HotSelectedState.Text.Enabled", L"HotSelectedState.Text.R", L"HotSelectedState.Text.G", L"HotSelectedState.Text.B");
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

struct FindTreeContext
{
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

        if (ctx->visibleTree)
            return FALSE;
    }

    return TRUE;
}

static HWND FindNavTreeInExplorer(HWND hExplorer)
{
    if (!IsExplorerTopWindow(hExplorer))
        return nullptr;

    FindTreeContext ctx{};
    EnumChildWindows(hExplorer, FindTreeEnumProc, (LPARAM)&ctx);

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
            g_MetricsStates.erase(g_MetricsStates.begin() + i);
        }
        else
            ++i;
    }

    LeaveCriticalSection(&g_StateLock);

    for (HIMAGELIST hList : ownedListsToDestroy)
        ImageList_Destroy(hList);
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

    if (AreAnyTreeMetricsEnabled())
        ApplyMetricsToTree(currentTree);
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

    if (result && IsWindowVisible(result))
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
    HWND result = nullptr;

    EnterCriticalSection(&g_StateLock);
    for (const auto& s : g_States)
    {
        if (IsWindow(s.hTree) && IsWindowVisible(s.hTree))
        {
            result = s.hTree;
            break;
        }
    }
    LeaveCriticalSection(&g_StateLock);

    return result;
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

static HWND ResolveTreeFromDrawContext(HDC hdc)
{
    if (!hdc)
        return GetAnyVisibleTrackedTree();

    HWND hwndFromDc = WindowFromDC(hdc);
    if (hwndFromDc && IsLikelyNavTree(hwndFromDc))
        return hwndFromDc;

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
    RECT fallback = *pRect;

    HWND hTree = ResolveTreeFromDrawContext(hdc);
    if (!hTree)
        return fallback;

    HTREEITEM hItem = ResolveItemFromDrawContext(hTree, iStateId, pRect);
    if (!hItem)
        return fallback;

    RECT rcItem{};
    if (!TreeView_GetItemRect(hTree, hItem, &rcItem, TRUE))
        return fallback;

    rcItem.left -= g_ContentExtraLeft;
    rcItem.right += g_ContentExtraRight;
    return rcItem;
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
    if (!TryGetOverrideTextColor(iStateId, &clr))
        return E_FAIL;

    DTTOPTS opts{};
    opts.dwSize = sizeof(opts);
    opts.dwFlags = DTT_TEXTCOLOR;
    opts.crText = clr;

    if (pOptions)
    {
        if (pOptions->dwFlags & DTT_COMPOSITED)
        {
            opts.dwFlags |= DTT_COMPOSITED;
            opts.iGlowSize = pOptions->iGlowSize;
        }

        if (pOptions->dwFlags & DTT_GLOWSIZE)
        {
            opts.dwFlags |= DTT_GLOWSIZE;
            opts.iGlowSize = pOptions->iGlowSize;
        }

        if (pOptions->dwFlags & DTT_APPLYOVERLAY)
            opts.dwFlags |= DTT_APPLYOVERLAY;

        if (pOptions->dwFlags & DTT_CALLBACK)
            opts.dwFlags |= DTT_CALLBACK;

        if (pOptions->dwFlags & DTT_CALCRECT)
            opts.dwFlags |= DTT_CALCRECT;

        if (pOptions->dwFlags & DTT_GRAYED)
            opts.dwFlags |= DTT_GRAYED;

        if (pOptions->dwFlags & DTT_TEXTCOLOR)
            opts.dwFlags |= DTT_TEXTCOLOR;
    }

    return DrawThemeTextEx_orig(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, pRect, &opts);
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
}

static DWORD WINAPI WorkerThreadProc(LPVOID)
{
    InitialTrackAllExplorerWindows();
    if (AreAnyTreeMetricsEnabled())
        ReapplyMetricsToTrackedTrees();

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
    EnterCriticalSection(&g_StateLock);
    for (auto& s : g_MetricsStates)
    {
        if (s.ownedNormal)
            ownedListsToDestroy.push_back(s.ownedNormal);
    }
    g_MetricsStates.clear();
    LeaveCriticalSection(&g_StateLock);

    for (HIMAGELIST hList : ownedListsToDestroy)
        ImageList_Destroy(hList);

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
