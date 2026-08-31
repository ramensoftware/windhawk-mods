// ==WindhawkMod==
// @id              file-operation-styler
// @name            File Operation Styler
// @description     Portable custom presentation for native Explorer file operations with a skin-safe unified presentation.
// @version         1.0.0
// @author          digART
// @github          https://github.com/digart11
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32 -lgdiplus -lshlwapi -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# File Operation Styler

A modern replacement for the standard Windows 11 file operation window.

File Operation Styler gives copy, move, delete, and recycle operations a cleaner modern layout while keeping the normal Windows file operation behavior.

![File Operation Styler](https://raw.githubusercontent.com/digart11/File-Operation-Styler/master/images/file-operation-styler.png)


### Default vs File Operation Styler

![Default vs File Operation Styler](https://raw.githubusercontent.com/digart11/File-Operation-Styler/master/images/file-operation-styler-compare.png)

### Themes

![File Operation Styler Themes](https://raw.githubusercontent.com/digart11/File-Operation-Styler/master/images/file-operation-styler-themes.png)
## Features

- Modern copy and move progress window
- Circular percentage indicator
- Transferred size, remaining items, speed, and estimated time
- Progress graph in More Details view
- Multiple file operations in the same window
- Pause, resume, and cancel controls
- Works with normal Windows conflict and error dialogs
- Several built-in themes
- Custom colors, fonts, text sizes, and progress thickness

## Customization

Choose one of the included themes or adjust a few basic options to create your own look.

## Notes

File Operation Styler changes the appearance of the normal file operation window only.  
Windows continues to handle the actual copy, move, delete, conflicts, and errors.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- customization:
  - enabled: false
    $name: Enable customization
    $description: Turn on themes and custom style settings.
  - preset: blueDark
    $name: Theme
    $description: Choose a theme, then change anything below if you want.
    $options:
    - blueDark: Blue Dark
    - graphite: Graphite
    - midnight: Midnight
    - warmDark: Warm Dark
    - light: Light
    - system: Windows / System
  - colors:
    - backgroundOverride: ""
      $name: Background
    - accentOverride: ""
      $name: Accent
      $description: Circle, progress bar, graph, and links.
    - primaryTextOverride: ""
      $name: Main text
      $description: Large numbers and values.
    - secondaryTextOverride: ""
      $name: Secondary text
      $description: Labels and smaller text.
    - inactiveOverride: ""
      $name: Track / inactive
      $description: Circle track and progress track.
    $name: Colors
    $description: "Leave blank to use the theme color. Enter a hex color such as #2D8BE0."
  - style:
    - circleThickness: 7
      $name: Circle thickness
    - progressThickness: 8
      $name: Progress bar thickness
    $name: Progress style
  - text:
    - fontPreset: default
      $name: Font
      $description: Choose one font for the whole window.
      $options:
      - default: Windows default
      - segoeUI: Segoe UI
      - segoeUIVariable: Segoe UI Variable
      - arial: Arial
      - calibri: Calibri
      - tahoma: Tahoma
      - verdana: Verdana
      - trebuchetMS: Trebuchet MS
      - georgia: Georgia
      - timesNewRoman: Times New Roman
      - consolas: Consolas
    - customFont: ""
      $name: Custom font
      $description: Optional. Enter an installed font name here to use it instead of the selection above.
    - bodySize: 11
      $name: Details text size
      $description: Source and destination, items, speed, time, Complete, and footer text.
    - summarySize: 23
      $name: Transfer total size
      $description: The large transferred / total line, for example 1.2 GB / 4.0 GB.
    - percentSize: 26
      $name: Circle percentage size
      $description: The percentage number inside the progress circle.
    $name: Text
  $name: Customization
*/
// ==/WindhawkModSettings==

// 0.12 architecture: Explorer remains the operation engine and native
// fallback, while normal-operation visuals are rendered in DPI-aware child
// windows owned by this mod. Native DirectUI controls stay alive underneath.

#include <windhawk_utils.h>

#include <commctrl.h>
#include <gdiplus.h>
#include <shlwapi.h>
#include <windows.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwchar>
#include <limits>
#include <mutex>
#include <string>
#include <vector>

struct COperationStatusTile;
struct COperationStatusTileRateCalculator;
struct OperationTileElement;

namespace DirectUI
{
    struct DUIXmlParser;
    struct Element;
    struct PropertyInfo;
    struct Value;
} // namespace DirectUI

namespace
{

    static_assert(sizeof(void *) == 8);
    static_assert(sizeof(unsigned long) == sizeof(ULONG));
    static_assert(sizeof(ATOM) == sizeof(unsigned short));

    struct SkinState
    {
        bool active;
        bool collecting;
        DirectUI::Element *operationTileRoot;
        DirectUI::Element *tileHeaderRoot;
    };

    thread_local SkinState g_skinState{};
    std::atomic<bool> g_unloading{};
    std::atomic<unsigned int> g_presentationActivations{};
    std::atomic<unsigned long long> g_skinEventSequence{};
    std::atomic<unsigned long long> g_displayTransitionSequence{};

    class ScopedPresentationActivation
    {
    public:
        ScopedPresentationActivation()
        {
            g_presentationActivations.fetch_add(
                1, std::memory_order_acq_rel);
        }

        ~ScopedPresentationActivation()
        {
            g_presentationActivations.fetch_sub(
                1, std::memory_order_acq_rel);
        }

        ScopedPresentationActivation(
            ScopedPresentationActivation const &) = delete;
        ScopedPresentationActivation &operator=(
            ScopedPresentationActivation const &) = delete;
    };

    void ClearSkinState()
    {
        ZeroMemory(&g_skinState, sizeof(g_skinState));
    }

    class ScopedTileSkin
    {
    public:
        ScopedTileSkin()
        {
            auto &state = g_skinState;
            if (state.active)
            {
                nestedCall_ = true;
                previousCollecting_ = state.collecting;
                state.collecting = false;
                return;
            }

            ClearSkinState();
            state.active = true;
            state.collecting = true;
            ownsSkin_ = true;
        }

        ~ScopedTileSkin()
        {
            if (ownsSkin_)
            {
                ClearSkinState();
            }
            else if (nestedCall_)
            {
                g_skinState.collecting = previousCollecting_;
            }
        }

        ScopedTileSkin(ScopedTileSkin const &) = delete;
        ScopedTileSkin &operator=(ScopedTileSkin const &) = delete;

        bool OwnsSkin() const
        {
            return ownsSkin_;
        }

    private:
        bool ownsSkin_ = false;
        bool nestedCall_ = false;
        bool previousCollecting_ = false;
    };

    // Exact verified x64 member-function ABI for the dui70.dll export:
    // ?CreateElement@DUIXmlParser@DirectUI@@QEAAJPEBGPEAVElement@2@1PEAKPEAPEAV32@@Z
    // The raw detour receives the implicit DUIXmlParser "this" pointer first.
    using DUIXmlParser_CreateElement_t = HRESULT(__cdecl *)(
        DirectUI::DUIXmlParser *parser,
        PCWSTR resourceName,
        DirectUI::Element *parent,
        DirectUI::Element *insertBefore,
        ULONG *deferCookie,
        DirectUI::Element **createdElement);
    DUIXmlParser_CreateElement_t DUIXmlParser_CreateElement_Original;

    HRESULT __cdecl DUIXmlParser_CreateElement_Hook(
        DirectUI::DUIXmlParser *parser,
        PCWSTR resourceName,
        DirectUI::Element *parent,
        DirectUI::Element *insertBefore,
        ULONG *deferCookie,
        DirectUI::Element **createdElement)
    {
        auto &state = g_skinState;
        if (g_unloading.load(std::memory_order_acquire) ||
            !state.active || !state.collecting)
        {
            return DUIXmlParser_CreateElement_Original(
                parser, resourceName, parent, insertBefore, deferCookie,
                createdElement);
        }

        bool isOperationTile = false;
        bool isTileHeader = false;
        if (resourceName && !IS_INTRESOURCE(resourceName))
        {
            isOperationTile = lstrcmpW(resourceName, L"idOperationTile") == 0;
            isTileHeader = lstrcmpW(resourceName, L"idTileHeader") == 0;
        }

        HRESULT result = DUIXmlParser_CreateElement_Original(
            parser, resourceName, parent, insertBefore, deferCookie,
            createdElement);

        if (SUCCEEDED(result) && createdElement && *createdElement)
        {
            if (isOperationTile)
            {
                state.operationTileRoot = *createdElement;
            }
            else if (isTileHeader)
            {
                state.tileHeaderRoot = *createdElement;
            }
        }

        return result;
    }

    // Verified DUI70!StrToID ABI. The returned atom is used only for the immediate
    // lookup and is never retained or hard-coded.
    using StrToID_t = ATOM(WINAPI *)(PCWSTR resourceName);
    StrToID_t StrToID_Original;

    // Exact verified x64 member-function ABI for:
    // ?FindDescendent@Element@DirectUI@@QEAAPEAV12@G@Z
    // The raw call includes the implicit DirectUI::Element "this" pointer first.
    using Element_FindDescendent_t = DirectUI::Element *(__cdecl *)(DirectUI::Element * thisPtr,
                                                                    unsigned short id);
    Element_FindDescendent_t Element_FindDescendent_Original;

    // Verified dui70.dll export:
    // ?GetParent@Element@DirectUI@@QEAAPEAV12@XZ
    using Element_GetParent_t = DirectUI::Element *(__cdecl *)(DirectUI::Element * thisPtr);
    Element_GetParent_t Element_GetParent_Original;

    using Value_Release_t = void(__cdecl *)(DirectUI::Value *thisPtr);
    Value_Release_t Value_Release_Original;

    // Verified dui70.dll export on the Windows 11 24H2 diagnostic target:
    // ?GetVisible@Element@DirectUI@@QEAA_NXZ
    using Element_GetVisible_t = bool(__cdecl *)(
        DirectUI::Element *thisPtr);
    Element_GetVisible_t Element_GetVisible_Original;

    // Visibility is the only native visual property changed in normal mode:
    // duplicate native content is hidden without changing its geometry, font,
    // colors, or ownership, and is restored for fallback/unload.
    using Element_SetVisible_t = HRESULT(__cdecl *)(
        DirectUI::Element *thisPtr,
        bool visible);
    Element_SetVisible_t Element_SetVisible_Original;

    // Optional read-only bridge. DirectUI text is copied immediately and is
    // never retained. The getter is optional so an export difference causes a
    // generic description, not an explorer.exe initialization failure.
    using Element_GetContentString_t = PCWSTR(__cdecl *)(
        DirectUI::Element *thisPtr,
        DirectUI::Value **valueToken);
    Element_GetContentString_t Element_GetContentString_Optional;

    // Exact build-matched x64 ABIs from shell32.pdb and DUI70 exports. The
    // OnPropertyChanged disassembly reads the second Value* with Value::GetInt,
    // then sends that integer to the native progress HWND with PBM_SETPOS.
    using OperationTileElement_ProgressPositionProp_t =
        DirectUI::PropertyInfo const *(__cdecl *)();
    OperationTileElement_ProgressPositionProp_t
        OperationTileElement_ProgressPositionProp_Original;

    using OperationTileElement_GetProgressHWND_t = HWND(__cdecl *)(
        OperationTileElement *thisPtr);
    OperationTileElement_GetProgressHWND_t
        OperationTileElement_GetProgressHWND_Original;

    using OperationTileElement_OnPropertyChanged_t = void(__cdecl *)(
        OperationTileElement *thisPtr,
        DirectUI::PropertyInfo const *property,
        int propertyIndex,
        DirectUI::Value *oldValue,
        DirectUI::Value *newValue);
    OperationTileElement_OnPropertyChanged_t
        OperationTileElement_OnPropertyChanged_Original;

    using OperationTileElement_Destructor_t = void(__cdecl *)(
        OperationTileElement *thisPtr);
    OperationTileElement_Destructor_t OperationTileElement_Destructor_Original;

    using COperationStatusTile_UpdateRemainingItemsAndSize_t =
        HRESULT(__cdecl *)(COperationStatusTile *thisPtr,
                           unsigned long long completedItems,
                           unsigned long long totalItems,
                           unsigned long long completedBytes,
                           unsigned long long totalBytes);
    COperationStatusTile_UpdateRemainingItemsAndSize_t
        COperationStatusTile_UpdateRemainingItemsAndSize_Original;

    using COperationStatusTile_UpdateSummary_t = HRESULT(__cdecl *)(
        COperationStatusTile *thisPtr,
        PCWSTR summary);
    COperationStatusTile_UpdateSummary_t COperationStatusTile_UpdateSummary_Original;

    using COperationStatusTile_SetTileDisplayMode_t = HRESULT(__cdecl *)(
        COperationStatusTile *thisPtr,
        bool expanded);
    COperationStatusTile_SetTileDisplayMode_t
        COperationStatusTile_SetTileDisplayMode_Original;

    using COperationStatusTileRateCalculator_CalculateRate_t = double(__cdecl *)(
        COperationStatusTileRateCalculator *thisPtr,
        unsigned long long value1,
        unsigned long long value2,
        unsigned long long value3,
        unsigned long long value4,
        unsigned long long value5,
        unsigned long long value6,
        unsigned long long value7,
        double *secondaryRate);
    COperationStatusTileRateCalculator_CalculateRate_t
        COperationStatusTileRateCalculator_CalculateRate_Original;

    // Windows 11 24H2 shell32.dll 10.0.26100.8972, PDB
    // 4907816C-76AB-D628-8BBE-01D3E8033EE9 age 1:
    // - COperationStatusTile's constructor writes the
    //   IOperationStatusTilePriv vftable at complete-object offset 0x18.
    // - SetTileDisplayMode is at RVA 0x3943A0. Its entry is a full method
    //   body, not an adjustor thunk, and it explicitly computes the complete
    //   object with LEA reg,[this-18h] before internal tile calls.
    constexpr ULONG_PTR kSetTileDisplayModeThisAdjustment = 0x18;

    struct TransferSummaryState
    {
        COperationStatusTile *owner;
        OperationTileElement *tile;
        DirectUI::Element *operationTileRoot;
        DirectUI::Element *tileHeaderRoot;
        unsigned long long completedBytes;
        unsigned long long totalBytes;
        bool bytesValid;
        bool displayModeKnown;
        bool expanded;
        double nativeDisplayRate = 0.0;
        bool nativeDisplayRateValid = false;
        unsigned long long completedItems = 0;
        unsigned long long totalItems = 0;
        bool itemsValid = false;
        bool deleteLikeKnown = false;
        bool deleteLike = false;
        bool preferMeasuredRate = false;
        double measuredDisplayRate = 0.0;
        bool measuredDisplayRateValid = false;
        ULONGLONG measuredSampleTick = 0;
        unsigned long long measuredSampleCompletedBytes = 0;
        unsigned long long measuredSampleCompletedItems = 0;
        bool measuredSampleInitialized = false;
        bool resumedFromSpecialState = false;
        std::vector<double> nativeRateHistory;
    };

    std::mutex g_transferSummaryMutex;
    std::vector<TransferSummaryState> g_transferSummaries;
    thread_local COperationStatusTile *g_nativeRateOwnerHint = nullptr;

    struct ThemePalette
    {
        COLORREF background = RGB(44, 44, 44);
        COLORREF primaryText = RGB(242, 244, 247);
        COLORREF secondaryText = RGB(154, 163, 174);
        COLORREF accent = RGB(64, 126, 170);
        COLORREF inactive = RGB(58, 65, 74);
        COLORREF graphSurface = RGB(36, 40, 46);
        COLORREF graphGrid = RGB(120, 128, 138);
        COLORREF graphLine = RGB(64, 126, 170);
        COLORREF graphFill = RGB(64, 126, 170);
        COLORREF actionSurface = RGB(52, 57, 64);
        COLORREF actionBorder = RGB(58, 65, 74);
        COLORREF actionText = RGB(242, 244, 247);
        int graphGridAlpha = 70;
        int graphFillAlpha = 150;
        int graphReferenceAlpha = 190;
        int graphLabelBackgroundAlpha = 235;
    };

    struct LayoutConfig
    {
        int windowWidth = 570;
        int leftColumnWidth = 156;
        int contentTopPadding = 8;
        int contentRightPadding = 24;
        int contentBottomPadding = 4;
        int tileVerticalMargin = 0;
        int circleColumnWidth = 156;
        int circleWindowHeight = 130;
        int circleDiameter = 118;
        int circleTop = 4;
        int circleXOffset = 0;
        int circleY = 8;
        int circleStroke = 7;
        int infoXOffset = 0;
        int infoTop = 72;
        int compactPanelHeight = 72;
        int expandedPanelHeight = 144;
        int itemsY = 0;
        int speedY = 20;
        int progressY = 46;
        int progressHeight = 8;
        int graphY = 82;
        int graphHeight = 60;
        int compactTileHeight = 136;
        int expandedTileHeight = 226;
        int footerReserveHeight = 52;
        int nativeChartAreaHeight = 60;
        int nativeChartTopMargin = 4;
        int nativeChartBottomMargin = 3;
        int nativeGraphHeight = 52;
        int cancelWidth = 78;
        int cancelHeight = 26;
        int cancelRightPadding = 0;
        int cancelBottomPadding = 8;
    };

    struct TypographyConfig
    {
        std::wstring bodyFont = L"Segoe UI Variable Text";
        std::wstring summaryFont = L"Segoe UI Variable Display";
        std::wstring circleFont = L"Segoe UI Variable Display";
        std::wstring circleLabelFont = L"Segoe UI Variable";
        std::wstring nativeFont = L"Segoe UI Variable";
        int circlePercentSize = 26;
        int circleLabelSize = 11;
        int bodySize = 11;
        int graphValueSize = 10;
        int summarySize = 23;
        int summaryWeight = 500;
        int summaryTopMargin = 26;
        int footerSize = 11;
        int headerWeight = 400;
        int nativeDetailSize = 14;
        int nativeValueWeight = 500;
        int nativeLabelWeight = 400;
        int actionSize = 17;
        int actionWeight = 500;
    };

    struct ElementConfig
    {
        bool showCircle = true;
        bool showCompleteLabel = true;
        bool showDescription = true;
        bool showSummary = true;
        bool showItems = true;
        bool showSpeedTime = true;
        bool showProgressBar = true;
        bool showGraph = true;
        bool showCancel = true;
    };

    struct ModSettings
    {
        bool customizationEnabled = false;
        std::wstring preset = L"blueDark";
        ThemePalette theme{};
        bool applyNativeColors = false;
        LayoutConfig layout{};
        TypographyConfig typography{};
        ElementConfig elements{};
    };

    const LayoutConfig kDefaultLayout{};
    const TypographyConfig kDefaultTypography{};
    const ElementConfig kDefaultElements{};
    ModSettings g_settings{};

    LayoutConfig const &ActiveLayout()
    {
        return g_settings.customizationEnabled ? g_settings.layout
                                               : kDefaultLayout;
    }

    TypographyConfig const &ActiveTypography()
    {
        return g_settings.customizationEnabled ? g_settings.typography
                                               : kDefaultTypography;
    }

    ElementConfig const &ActiveElements()
    {
        return g_settings.customizationEnabled ? g_settings.elements
                                               : kDefaultElements;
    }

    bool IsDarkColor(COLORREF color)
    {
        int luminance =
            (GetRValue(color) * 299 +
             GetGValue(color) * 587 +
             GetBValue(color) * 114) /
            1000;
        return luminance < 128;
    }

    bool IsWindowsAppsDarkMode()
    {
        HIGHCONTRASTW highContrast{sizeof(highContrast)};
        if (SystemParametersInfoW(
                SPI_GETHIGHCONTRAST, sizeof(highContrast),
                &highContrast, 0) &&
            (highContrast.dwFlags & HCF_HIGHCONTRASTON))
        {
            return IsDarkColor(GetSysColor(COLOR_WINDOW));
        }

        DWORD appsUseLightTheme = 1;
        DWORD size = sizeof(appsUseLightTheme);
        if (RegGetValueW(
                HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                L"AppsUseLightTheme",
                RRF_RT_REG_DWORD, nullptr,
                &appsUseLightTheme, &size) == ERROR_SUCCESS)
        {
            return appsUseLightTheme == 0;
        }

        return IsDarkColor(GetSysColor(COLOR_WINDOW));
    }

    ThemePalette MakeSystemTheme()
    {
        ThemePalette theme{};
        HIGHCONTRASTW highContrast{sizeof(highContrast)};
        bool highContrastEnabled =
            SystemParametersInfoW(
                SPI_GETHIGHCONTRAST, sizeof(highContrast),
                &highContrast, 0) &&
            (highContrast.dwFlags & HCF_HIGHCONTRASTON);

        if (highContrastEnabled)
        {
            theme.background = GetSysColor(COLOR_WINDOW);
            theme.primaryText = GetSysColor(COLOR_WINDOWTEXT);
            theme.secondaryText = GetSysColor(COLOR_GRAYTEXT);
            theme.accent = GetSysColor(COLOR_HIGHLIGHT);
            theme.inactive = GetSysColor(COLOR_3DSHADOW);
            theme.graphSurface = theme.background;
            theme.graphGrid = theme.secondaryText;
            theme.graphLine = theme.accent;
            theme.graphFill = theme.accent;
            theme.actionSurface = GetSysColor(COLOR_BTNFACE);
            theme.actionBorder = GetSysColor(COLOR_BTNSHADOW);
            theme.actionText = GetSysColor(COLOR_BTNTEXT);
            return theme;
        }

        bool dark = IsWindowsAppsDarkMode();
        if (dark)
        {
            theme.background = RGB(32, 32, 32);
            theme.primaryText = RGB(245, 245, 245);
            theme.secondaryText = RGB(174, 174, 174);
            theme.accent = GetSysColor(COLOR_HIGHLIGHT);
            theme.inactive = RGB(64, 64, 64);
            theme.graphSurface = RGB(27, 27, 27);
            theme.graphGrid = RGB(105, 105, 105);
            theme.graphLine = theme.accent;
            theme.graphFill = theme.accent;
            theme.actionSurface = RGB(48, 48, 48);
            theme.actionBorder = RGB(72, 72, 72);
            theme.actionText = theme.primaryText;
        }
        else
        {
            theme.background = GetSysColor(COLOR_WINDOW);
            theme.primaryText = GetSysColor(COLOR_WINDOWTEXT);
            theme.secondaryText = RGB(96, 96, 96);
            theme.accent = GetSysColor(COLOR_HIGHLIGHT);
            theme.inactive = RGB(210, 210, 210);
            theme.graphSurface = RGB(245, 245, 245);
            theme.graphGrid = RGB(175, 175, 175);
            theme.graphLine = theme.accent;
            theme.graphFill = theme.accent;
            theme.actionSurface = RGB(242, 242, 242);
            theme.actionBorder = RGB(205, 205, 205);
            theme.actionText = theme.primaryText;
        }

        return theme;
    }

    ThemePalette MakePresetTheme(std::wstring const &preset)
    {
        if (preset == L"system")
        {
            return MakeSystemTheme();
        }

        ThemePalette theme{};
        if (preset == L"graphite")
        {
            // Deliberately monochrome and darker than the Windows/system dark
            // palette so Graphite reads as an intentional skin, not "default".
            theme.background = RGB(25, 25, 25);
            theme.primaryText = RGB(246, 246, 246);
            theme.secondaryText = RGB(164, 164, 164);
            theme.accent = RGB(185, 185, 185);
            theme.inactive = RGB(61, 61, 61);
            theme.graphSurface = RGB(18, 18, 18);
            theme.graphGrid = RGB(92, 92, 92);
            theme.graphLine = RGB(205, 205, 205);
            theme.graphFill = RGB(126, 126, 126);
            theme.actionSurface = RGB(39, 39, 39);
            theme.actionBorder = RGB(72, 72, 72);
            theme.actionText = theme.primaryText;
        }
        else if (preset == L"midnight")
        {
            // Midnight: very dark navy, still visibly blue rather than gray.
            theme.background = RGB(8, 17, 31);
            theme.primaryText = RGB(239, 246, 255);
            theme.secondaryText = RGB(128, 150, 178);
            theme.accent = RGB(65, 145, 235);
            theme.inactive = RGB(27, 44, 65);
            theme.graphSurface = RGB(5, 12, 24);
            theme.graphGrid = RGB(48, 70, 96);
            theme.graphLine = RGB(79, 157, 245);
            theme.graphFill = RGB(55, 132, 220);
            theme.actionSurface = RGB(17, 30, 47);
            theme.actionBorder = RGB(38, 57, 79);
            theme.actionText = theme.primaryText;
        }
        else if (preset == L"warmDark")
        {
            theme.background = RGB(43, 39, 36);
            theme.primaryText = RGB(247, 242, 236);
            theme.secondaryText = RGB(177, 164, 151);
            theme.accent = RGB(196, 124, 73);
            theme.inactive = RGB(71, 63, 56);
            theme.graphSurface = RGB(34, 31, 28);
            theme.graphGrid = RGB(112, 99, 88);
            theme.graphLine = RGB(216, 139, 84);
            theme.graphFill = RGB(196, 124, 73);
            theme.actionSurface = RGB(61, 53, 47);
            theme.actionBorder = RGB(82, 70, 61);
            theme.actionText = theme.primaryText;
        }
        else if (preset == L"light")
        {
            theme.background = RGB(247, 247, 247);
            theme.primaryText = RGB(32, 32, 32);
            theme.secondaryText = RGB(96, 96, 96);
            theme.accent = RGB(0, 120, 212);
            theme.inactive = RGB(210, 210, 210);
            theme.graphSurface = RGB(238, 238, 238);
            theme.graphGrid = RGB(170, 170, 170);
            theme.graphLine = RGB(0, 108, 190);
            theme.graphFill = RGB(0, 120, 212);
            theme.actionSurface = RGB(235, 235, 235);
            theme.actionBorder = RGB(200, 200, 200);
            theme.actionText = theme.primaryText;
        }
        else
        {
            // Blue Dark: clearly blue-tinted body with a strong blue accent.
            theme.background = RGB(18, 38, 56);
            theme.primaryText = RGB(243, 248, 253);
            theme.secondaryText = RGB(151, 177, 199);
            theme.accent = RGB(45, 145, 230);
            theme.inactive = RGB(42, 67, 88);
            theme.graphSurface = RGB(12, 29, 44);
            theme.graphGrid = RGB(68, 96, 120);
            theme.graphLine = RGB(58, 158, 242);
            theme.graphFill = RGB(45, 145, 230);
            theme.actionSurface = RGB(28, 51, 70);
            theme.actionBorder = RGB(53, 78, 99);
            theme.actionText = theme.primaryText;
        }
        return theme;
    }

    std::wstring GetStringSettingValue(PCWSTR name)
    {
        PCWSTR raw = Wh_GetStringSetting(name);
        std::wstring value = raw ? raw : L"";
        if (raw)
        {
            Wh_FreeStringSetting(raw);
        }
        return value;
    }

    int GetClampedIntSetting(PCWSTR name, int minimum, int maximum)
    {
        return std::clamp(Wh_GetIntSetting(name), minimum, maximum);
    }

    bool ParseColorValue(std::wstring value, COLORREF *color)
    {
        if (!color)
        {
            return false;
        }

        if (value.empty() || _wcsicmp(value.c_str(), L"auto") == 0)
        {
            return false;
        }

        if (value[0] == L'#')
        {
            value.erase(value.begin());
        }
        else if (value.size() > 2 &&
                 value[0] == L'0' &&
                 (value[1] == L'x' || value[1] == L'X'))
        {
            value.erase(0, 2);
        }

        if (value.size() != 6)
        {
            return false;
        }

        wchar_t *end = nullptr;
        unsigned long rgb = wcstoul(value.c_str(), &end, 16);
        if (!end || *end != L'\0' || rgb > 0xFFFFFF)
        {
            return false;
        }

        *color = RGB((rgb >> 16) & 0xFF,
                     (rgb >> 8) & 0xFF,
                     rgb & 0xFF);
        return true;
    }

    bool ApplyColorOverride(PCWSTR name, COLORREF *target)
    {
        COLORREF parsed{};
        if (!ParseColorValue(GetStringSettingValue(name), &parsed))
        {
            return false;
        }
        *target = parsed;
        return true;
    }

    ThemePalette GetDrawingTheme(HWND)
    {
        // 0.12 skin boundary:
        // Presets/colors are consumed only by File Operation Styler's own
        // GDI+/HWND presentation surfaces. We intentionally do not skin the
        // native DirectUI operation tree, so the same palette code is portable
        // across machines whose Explorer backing surfaces differ.
        if (g_settings.applyNativeColors)
        {
            return g_settings.theme;
        }

        // Customization-off is system-derived, but never sampled from an
        // undocumented DirectUI-owned pixel. This keeps one deterministic
        // palette on machines whose native backing surfaces differ.
        return MakeSystemTheme();
    }

    bool ShouldApplyNativeColorOverrides()
    {
        return g_settings.applyNativeColors;
    }

    DirectUI::Element *FindSkinElement(
        DirectUI::Element *tileRoot,
        DirectUI::Element *tileHeaderRoot,
        PCWSTR name,
        bool allowHeaderFallback);

    void LoadSettings()
    {
        // Start from the proven 0.12 defaults. The public settings intentionally
        // expose only meaningful style controls; geometry/visibility internals
        // remain fixed so customization can't accidentally break the layout.
        g_settings = ModSettings{};

        g_settings.customizationEnabled =
            Wh_GetIntSetting(L"customization.enabled") != 0;

        g_settings.preset =
            GetStringSettingValue(L"customization.preset");
        if (g_settings.preset.empty())
        {
            g_settings.preset = L"blueDark";
        }

        g_settings.theme = MakePresetTheme(g_settings.preset);

        bool anyColorOverride = false;

        bool customBackground = ApplyColorOverride(
            L"customization.colors.backgroundOverride",
            &g_settings.theme.background);
        anyColorOverride |= customBackground;

        bool customAccent = ApplyColorOverride(
            L"customization.colors.accentOverride",
            &g_settings.theme.accent);
        anyColorOverride |= customAccent;

        bool customPrimary = ApplyColorOverride(
            L"customization.colors.primaryTextOverride",
            &g_settings.theme.primaryText);
        anyColorOverride |= customPrimary;

        bool customSecondary = ApplyColorOverride(
            L"customization.colors.secondaryTextOverride",
            &g_settings.theme.secondaryText);
        anyColorOverride |= customSecondary;

        bool customInactive = ApplyColorOverride(
            L"customization.colors.inactiveOverride",
            &g_settings.theme.inactive);
        anyColorOverride |= customInactive;

        // One Accent controls the visual identity instead of five separate
        // color pickers.
        if (customAccent)
        {
            g_settings.theme.graphLine = g_settings.theme.accent;
            g_settings.theme.graphFill = g_settings.theme.accent;
        }

        // Background means the whole custom body, including the graph surface.
        if (customBackground)
        {
            g_settings.theme.graphSurface = g_settings.theme.background;
        }

        if (customPrimary)
        {
            g_settings.theme.actionText = g_settings.theme.primaryText;
        }

        if (customInactive)
        {
            g_settings.theme.graphGrid = g_settings.theme.inactive;
            g_settings.theme.actionBorder = g_settings.theme.inactive;
        }

        g_settings.applyNativeColors =
            g_settings.customizationEnabled &&
            (g_settings.preset != L"system" || anyColorOverride);

        // Only two geometry controls are public: the two thicknesses people
        // can meaningfully tune without destabilizing the layout.
        auto &layout = g_settings.layout;
        layout.circleStroke =
            GetClampedIntSetting(
                L"customization.style.circleThickness", 1, 30);
        layout.progressHeight =
            GetClampedIntSetting(
                L"customization.style.progressThickness", 2, 20);

        layout.circleStroke =
            std::min(layout.circleStroke,
                     std::max(layout.circleDiameter / 3, 1));

        // Typography is grouped: one font and one normal-text size, with only
        // the two visually important large sizes exposed separately.
        auto &type = g_settings.typography;

        std::wstring selectedFont;
        std::wstring fontPreset =
            GetStringSettingValue(L"customization.text.fontPreset");

        if (fontPreset == L"segoeUI")
        {
            selectedFont = L"Segoe UI";
        }
        else if (fontPreset == L"segoeUIVariable")
        {
            selectedFont = L"Segoe UI Variable";
        }
        else if (fontPreset == L"arial")
        {
            selectedFont = L"Arial";
        }
        else if (fontPreset == L"calibri")
        {
            selectedFont = L"Calibri";
        }
        else if (fontPreset == L"tahoma")
        {
            selectedFont = L"Tahoma";
        }
        else if (fontPreset == L"verdana")
        {
            selectedFont = L"Verdana";
        }
        else if (fontPreset == L"trebuchetMS")
        {
            selectedFont = L"Trebuchet MS";
        }
        else if (fontPreset == L"georgia")
        {
            selectedFont = L"Georgia";
        }
        else if (fontPreset == L"timesNewRoman")
        {
            selectedFont = L"Times New Roman";
        }
        else if (fontPreset == L"consolas")
        {
            selectedFont = L"Consolas";
        }

        // A custom installed font name, when provided, wins over the list.
        std::wstring customFont =
            GetStringSettingValue(L"customization.text.customFont");
        if (!customFont.empty())
        {
            selectedFont = customFont;
        }

        if (!selectedFont.empty())
        {
            type.bodyFont = selectedFont;
            type.summaryFont = selectedFont;
            type.circleFont = selectedFont;
            type.circleLabelFont = selectedFont;
            type.nativeFont = selectedFont;
        }

        type.bodySize =
            GetClampedIntSetting(
                L"customization.text.bodySize", 8, 24);
        type.summarySize =
            GetClampedIntSetting(
                L"customization.text.summarySize", 14, 48);
        type.circlePercentSize =
            GetClampedIntSetting(
                L"customization.text.percentSize", 14, 56);

        // Keep related small text together instead of exposing every label.
        type.circleLabelSize = type.bodySize;
        type.footerSize = type.bodySize;
        type.graphValueSize = std::max(type.bodySize - 1, 7);

    }

#define kBackgroundColor (g_settings.theme.background)
#define kPrimaryTextColor (g_settings.theme.primaryText)
#define kSecondaryTextColor (g_settings.theme.secondaryText)
#define kInactiveRingColor (g_settings.theme.inactive)
#define kAccentRingColor (g_settings.theme.accent)
#define kGraphSurfaceColor (g_settings.theme.graphSurface)
#define kActionSurfaceColor (g_settings.theme.actionSurface)

#define kRequestedTileWidth (ActiveLayout().windowWidth)
#define kReservedLeftWidth (ActiveLayout().leftColumnWidth)
#define kContentTopPadding (ActiveLayout().contentTopPadding)
#define kContentRightPadding (ActiveLayout().contentRightPadding)
#define kCircleColumnWidth (ActiveLayout().circleColumnWidth)
#define kCircleWindowHeight (ActiveLayout().circleWindowHeight)
#define kCircleDiameter (ActiveLayout().circleDiameter)
#define kCircleTop (ActiveLayout().circleTop)
#define kCircleHostY (ActiveLayout().circleY)
#define kCircleStrokeWidth (ActiveLayout().circleStroke)

    // Windows 11 DWM attributes. Resolve DwmSetWindowAttribute dynamically so
    // the mod doesn't need an additional import library. Making the caption
    // use the same color as the client area removes the stock gray "tab"
    // look and makes OperationStatusWindow read as one continuous card.
    constexpr DWORD kDwmwaUseImmersiveDarkMode = 20;
    constexpr DWORD kDwmwaBorderColor = 34;
    constexpr DWORD kDwmwaCaptionColor = 35;
    constexpr DWORD kDwmwaTextColor = 36;
    constexpr COLORREF kDwmColorDefault = 0xFFFFFFFF;

    using DwmSetWindowAttribute_t = HRESULT(WINAPI *)(
        HWND hwnd, DWORD attribute, LPCVOID value, DWORD valueSize);

    HMODULE g_ownedDwmApiModule;
    DwmSetWindowAttribute_t g_dwmSetWindowAttribute;

    DwmSetWindowAttribute_t GetDwmSetWindowAttribute()
    {
        static std::once_flag initializeOnce;
        std::call_once(initializeOnce, []
        {
            HMODULE module = GetModuleHandleW(L"dwmapi.dll");
            bool owned = false;
            if (!module)
            {
                module = LoadLibraryExW(
                    L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
                owned = module != nullptr;
            }
            if (!module)
            {
                return;
            }

            g_dwmSetWindowAttribute =
                reinterpret_cast<DwmSetWindowAttribute_t>(
                GetProcAddress(module, "DwmSetWindowAttribute"));
            if (owned)
            {
                g_ownedDwmApiModule = module;
            }
        });
        return g_dwmSetWindowAttribute;
    }

    void ShutdownDwmApi()
    {
        g_dwmSetWindowAttribute = nullptr;
        if (g_ownedDwmApiModule)
        {
            FreeLibrary(g_ownedDwmApiModule);
            g_ownedDwmApiModule = nullptr;
        }
    }

    void ApplyUnifiedHostChrome(HWND hostWindow)
    {
        if (!ShouldApplyNativeColorOverrides())
        {
            return;
        }

        DwmSetWindowAttribute_t setAttribute = GetDwmSetWindowAttribute();
        if (!setAttribute || !hostWindow || !IsWindow(hostWindow))
        {
            return;
        }

        BOOL darkMode = IsDarkColor(kBackgroundColor) ? TRUE : FALSE;
        COLORREF captionColor = kBackgroundColor;
        COLORREF textColor = kPrimaryTextColor;
        COLORREF borderColor = kInactiveRingColor;
        setAttribute(hostWindow, kDwmwaUseImmersiveDarkMode,
                     &darkMode, sizeof(darkMode));
        setAttribute(hostWindow, kDwmwaCaptionColor,
                     &captionColor, sizeof(captionColor));
        setAttribute(hostWindow, kDwmwaTextColor,
                     &textColor, sizeof(textColor));
        setAttribute(hostWindow, kDwmwaBorderColor,
                     &borderColor, sizeof(borderColor));
    }

    void ResetUnifiedHostChrome(HWND hostWindow)
    {
        DwmSetWindowAttribute_t setAttribute = GetDwmSetWindowAttribute();
        if (!setAttribute || !hostWindow || !IsWindow(hostWindow))
        {
            return;
        }

        BOOL systemDarkMode = IsWindowsAppsDarkMode() ? TRUE : FALSE;
        setAttribute(hostWindow, kDwmwaUseImmersiveDarkMode,
                     &systemDarkMode, sizeof(systemDarkMode));

        COLORREF defaultColor = kDwmColorDefault;
        setAttribute(hostWindow, kDwmwaCaptionColor,
                     &defaultColor, sizeof(defaultColor));
        setAttribute(hostWindow, kDwmwaTextColor,
                     &defaultColor, sizeof(defaultColor));
        setAttribute(hostWindow, kDwmwaBorderColor,
                     &defaultColor, sizeof(defaultColor));
    }

#define kDisplayModeFooterReserveHeight (ActiveLayout().footerReserveHeight)

    constexpr wchar_t kCircleWindowClass[] =
        L"Windhawk.FileOperationStyler.ProgressCircle.0.12.11";
    constexpr wchar_t kInfoPanelWindowClass[] =
        L"Windhawk.FileOperationStyler.OperationPresentation.0.12.11";
    constexpr wchar_t kFooterOverlayWindowClass[] =
        L"Windhawk.FileOperationStyler.FooterPresentation.0.12.11";

#define kInfoPanelTop (ActiveLayout().infoTop)
#define kInfoPanelCommonHeight (ActiveLayout().compactPanelHeight)
#define kInfoPanelExpandedHeight (ActiveLayout().expandedPanelHeight)
#define kInfoPanelItemsTop (ActiveLayout().itemsY)
#define kInfoPanelSpeedTop (ActiveLayout().speedY)
#define kInfoPanelProgressTop (ActiveLayout().progressY)
#define kInfoPanelProgressHeight (ActiveLayout().progressHeight)
#define kInfoPanelChartTop (ActiveLayout().graphY)
#define kInfoPanelChartHeight (ActiveLayout().graphHeight)
#define kCompactRegularTileHeight (ActiveLayout().compactTileHeight)
#define kExpandedRegularTileHeight (ActiveLayout().expandedTileHeight)
#define kInfoPanelCancelWidth (ActiveLayout().cancelWidth)
#define kInfoPanelCancelHeight (ActiveLayout().cancelHeight)
#define kInfoPanelCancelRightPadding (ActiveLayout().cancelRightPadding)
#define kInfoPanelCancelBottomPadding (ActiveLayout().cancelBottomPadding)
#define kFooterOverlayHeight (ActiveLayout().footerReserveHeight)
    constexpr size_t kInfoPanelRateHistorySamples = 72;
    constexpr UINT_PTR kHostWindowSubclassId = 0xF0510010;
    constexpr UINT_PTR kProgressWindowSubclassId = 0xF0510011;

    struct CircleState
    {
        OperationTileElement *tile;
        HWND circleWindow;
        HWND infoWindow;
        HWND progressWindow;
        HWND hostWindow;
        int progressPercent;
        int progressRangeLow;
        int progressRangeHigh;
        bool progressRangeInitialized;
        bool progressRangeValid;
        bool paused;
        bool pausedStateKnown;
        unsigned long long eventId;
        int positionX;
        int positionY;
        int positionWidth;
        int positionHeight;
        bool positionValid;
    };

    struct HostPositionRequest
    {
        HWND hostWindow;
        PCWSTR reason;
    };

    struct DeferredDisplaySnapshot
    {
        COperationStatusTile *owner;
        OperationTileElement *tile;
        DirectUI::Element *operationTileRoot;
        HWND hostWindow;
        DWORD uiThreadId;
        unsigned long long transitionId;
        bool requestedExpanded;
    };

    std::mutex g_circleMutex;
    std::vector<CircleState> g_circles;
    std::vector<HWND> g_subclassedHosts;

    struct HostPresentationState
    {
        HWND hostWindow;
        bool sawNormalProgressCaption;
        bool specialOperationState;
    };

    struct HostNativeGeometry
    {
        HWND hostWindow;
        int width;
        int height;
        bool restorationAttempted;
    };

    std::mutex g_hostPresentationMutex;
    std::vector<HostPresentationState> g_hostPresentationStates;
    std::mutex g_hostNativeGeometryMutex;
    std::vector<HostNativeGeometry> g_hostNativeGeometries;
    thread_local HWND g_suppressNativeGeometryCaptureHost;
    thread_local HWND g_restoringNativeGeometryHost;
    std::vector<HostPositionRequest> g_hostPositionRequests;
    std::mutex g_displayDiagnosticMutex;
    std::vector<DeferredDisplaySnapshot> g_deferredDisplaySnapshots;
    HINSTANCE g_circleClassInstance;
    ATOM g_circleClassAtom;
    ATOM g_infoPanelClassAtom;
    ATOM g_footerOverlayClassAtom;
    ULONG_PTR g_gdiplusToken;
    UINT g_removeHostSubclassMessage;
    UINT g_positionCirclesMessage;
    UINT g_logDisplayStateMessage;

    class ScopedHostGeometryChange
    {
    public:
        ScopedHostGeometryChange(HWND hostWindow,
                                 bool restoringNativeGeometry)
            : m_previousCaptureSuppression(
                  g_suppressNativeGeometryCaptureHost),
              m_previousRestoration(g_restoringNativeGeometryHost)
        {
            g_suppressNativeGeometryCaptureHost = hostWindow;
            if (restoringNativeGeometry)
            {
                g_restoringNativeGeometryHost = hostWindow;
            }
        }

        ~ScopedHostGeometryChange()
        {
            g_suppressNativeGeometryCaptureHost =
                m_previousCaptureSuppression;
            g_restoringNativeGeometryHost = m_previousRestoration;
        }

        ScopedHostGeometryChange(ScopedHostGeometryChange const &) = delete;
        ScopedHostGeometryChange &operator=(
            ScopedHostGeometryChange const &) = delete;

    private:
        HWND m_previousCaptureSuppression;
        HWND m_previousRestoration;
    };

    void CaptureHostNativeGeometry(HWND hostWindow, WINDOWPOS const &position)
    {
        if (g_unloading.load(std::memory_order_acquire) || !hostWindow ||
            (position.flags & SWP_NOSIZE) ||
            position.cx <= 0 || position.cy <= 0 ||
            g_suppressNativeGeometryCaptureHost == hostWindow)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(g_hostNativeGeometryMutex);
        auto it = std::find_if(
            g_hostNativeGeometries.begin(), g_hostNativeGeometries.end(),
            [hostWindow](HostNativeGeometry const &geometry)
            { return geometry.hostWindow == hostWindow; });
        if (it == g_hostNativeGeometries.end())
        {
            g_hostNativeGeometries.push_back(
                {hostWindow, position.cx, position.cy, false});
        }
        else
        {
            it->width = position.cx;
            it->height = position.cy;
        }
    }

    void ForgetHostNativeGeometry(HWND hostWindow)
    {
        std::lock_guard<std::mutex> lock(g_hostNativeGeometryMutex);
        g_hostNativeGeometries.erase(
            std::remove_if(
                g_hostNativeGeometries.begin(),
                g_hostNativeGeometries.end(),
                [hostWindow](HostNativeGeometry const &geometry)
                { return geometry.hostWindow == hostWindow; }),
            g_hostNativeGeometries.end());
    }

    void RestoreHostNativeGeometry(HWND hostWindow)
    {
        HostNativeGeometry geometry{};
        bool found = false;
        {
            std::lock_guard<std::mutex> lock(g_hostNativeGeometryMutex);
            auto it = std::find_if(
                g_hostNativeGeometries.begin(),
                g_hostNativeGeometries.end(),
                [hostWindow](HostNativeGeometry const &candidate)
                { return candidate.hostWindow == hostWindow; });
            if (it == g_hostNativeGeometries.end())
            {
                g_hostNativeGeometries.push_back(
                    {hostWindow, 0, 0, true});
            }
            else
            {
                if (it->restorationAttempted)
                {
                    return;
                }
                it->restorationAttempted = true;
                geometry = *it;
                found = geometry.width > 0 && geometry.height > 0;
            }
        }

        if (!found)
        {
            Wh_Log(L"Presentation teardown has no captured native geometry "
                   L"host=%p",
                   reinterpret_cast<void *>(hostWindow));
            return;
        }

        ScopedHostGeometryChange geometryChange(hostWindow, true);
        if (!SetWindowPos(
                hostWindow, nullptr, 0, 0, geometry.width, geometry.height,
                SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE |
                    SWP_NOOWNERZORDER))
        {
            Wh_Log(L"Presentation teardown failed to restore native geometry "
                   L"host=%p width=%d height=%d error=%lu",
                   reinterpret_cast<void *>(hostWindow), geometry.width,
                   geometry.height, GetLastError());
        }
    }

    DirectUI::Element *FindSkinElement(DirectUI::Element *tileRoot,
                                       DirectUI::Element *tileHeaderRoot,
                                       PCWSTR name,
                                       bool allowHeaderFallback)
    {
        ATOM id = StrToID_Original(name);
        if (!id)
        {
            return nullptr;
        }

        DirectUI::Element *element =
            Element_FindDescendent_Original(tileRoot, id);
        if (!element && allowHeaderFallback && tileHeaderRoot)
        {
            element = Element_FindDescendent_Original(tileHeaderRoot, id);
        }

        return element;
    }

    DirectUI::Element *FindSkinElementWithAncestorFallback(
        DirectUI::Element *tileRoot,
        DirectUI::Element *tileHeaderRoot,
        DirectUI::Element *parentElement,
        PCWSTR name,
        bool allowHeaderFallback)
    {
        DirectUI::Element *element = FindSkinElement(
            tileRoot, tileHeaderRoot, name, allowHeaderFallback);
        if (element)
        {
            return element;
        }

        ATOM id = StrToID_Original(name);
        if (!id)
        {
            return nullptr;
        }

        // More/Fewer details is outside idOperationTile on this shell build.
        // Search only the verified CreateTileElement parent chain; do not
        // broaden the lookup process-wide.
        DirectUI::Element *searchRoot = parentElement;
        for (int depth = 0; searchRoot && depth < 6; ++depth)
        {
            element = Element_FindDescendent_Original(searchRoot, id);
            if (element)
            {
                return element;
            }
            searchRoot = Element_GetParent_Original(searchRoot);
        }

        return nullptr;
    }


    struct WindowLookupContext
    {
        unsigned long long eventId;
        HWND operationStatusWindow;
    };

    BOOL CALLBACK FindOperationStatusWindow(HWND window, LPARAM parameter)
    {
        auto *context = reinterpret_cast<WindowLookupContext *>(parameter);
        wchar_t className[64];
        int classNameLength =
            GetClassNameW(window, className, ARRAYSIZE(className));
        if (!classNameLength)
        {
            Wh_Log(L"eventId=%llu base-layout GetClassNameW failed hwnd=%p "
                   L"error=%lu",
                   context->eventId, reinterpret_cast<void *>(window),
                   GetLastError());
            return TRUE;
        }

        if (lstrcmpW(className, L"OperationStatusWindow") == 0)
        {
            context->operationStatusWindow = window;
            return FALSE;
        }

        return TRUE;
    }

    int ScaleForDpi(int value, UINT dpi)
    {
        return MulDiv(value, static_cast<int>(dpi), USER_DEFAULT_SCREEN_DPI);
    }

    HWND FindCurrentThreadOperationStatusWindow(unsigned long long eventId)
    {
        WindowLookupContext context{eventId, nullptr};
        SetLastError(ERROR_SUCCESS);
        BOOL enumResult = EnumThreadWindows(
            GetCurrentThreadId(), FindOperationStatusWindow,
            reinterpret_cast<LPARAM>(&context));
        if (!context.operationStatusWindow)
        {
            DWORD error = enumResult ? ERROR_NOT_FOUND : GetLastError();
            if (error == ERROR_SUCCESS)
            {
                error = ERROR_NOT_FOUND;
            }
            Wh_Log(L"eventId=%llu circle OperationStatusWindow lookup failed "
                   L"error=%lu",
                   eventId, error);
        }

        return context.operationStatusWindow;
    }

    void DestroyProgressCircle(OperationTileElement *tile);
    void PositionProgressCirclesForHost(HWND hostWindow, PCWSTR reason);
    void PositionInfoPanel(OperationTileElement *tile);
    void PositionFooterOverlay(OperationTileElement *tile);
    void InvalidateInfoPanelForTile(OperationTileElement *tile);
    void ScheduleProgressCirclePosition(HWND hostWindow, PCWSTR reason);
    void HandleDeferredDisplaySnapshot(HWND hostWindow,
                                       unsigned long long transitionId);
    void ScheduleDeferredDisplaySnapshot(COperationStatusTile *owner,
                                         unsigned long long transitionId,
                                         bool requestedExpanded);
    bool ApplyDisplayMode(COperationStatusTile *owner,
                          bool applyFinalHostGeometry,
                          unsigned long long transitionId = 0);
    void InitializeRegisteredDisplayMode(COperationStatusTile *owner);
    bool GetVerifiedCustomHostWindowHeight(HWND hostWindow,
                                           int nativeWindowHeight,
                                           int *targetWindowHeight);
    bool IsSingleNormalProgressTileForHost(OperationTileElement *tile,
                                           HWND hostWindow);
    void ApplyNativeDisplayRatesForHost(HWND hostWindow);
    void CancelDeferredDisplaySnapshotsForHost(HWND hostWindow);
    void CancelDeferredDisplaySnapshotsForTile(OperationTileElement *tile);
    HWND FindDescendantWindowByClass(HWND parent, PCWSTR className);
    HWND GetRegisteredCircleHost(OperationTileElement *tile);
    size_t GetRegisteredTileCountForHost(HWND hostWindow);
    void MarkHostForMeasuredMultiRate(HWND hostWindow)
    {
        if (!hostWindow)
        {
            return;
        }

        std::vector<OperationTileElement *> hostTiles;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            for (CircleState const &circle : g_circles)
            {
                if (circle.hostWindow == hostWindow && circle.tile)
                {
                    hostTiles.push_back(circle.tile);
                }
            }
        }

        if (hostTiles.size() < 2)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
        for (TransferSummaryState &state : g_transferSummaries)
        {
            if (std::find(hostTiles.begin(), hostTiles.end(), state.tile) ==
                hostTiles.end())
            {
                continue;
            }

            // In a multi-operation host the Shell rate-calculator callbacks
            // share one UI thread and don't expose a stable tile identity.
            // Don't risk assigning one operation's native rate to another.
            // Use the event-derived per-owner rate from
            // UpdateRemainingItemsAndSize instead.
            state.preferMeasuredRate = true;
            state.nativeDisplayRateValid = false;
            state.nativeRateHistory.clear();
        }
    }

    bool GetMultiTileSlotBounds(
        OperationTileElement *tile,
        HWND hostWindow,
        int *slotTop,
        int *slotBottom);
    void RefreshDeleteLikeOperationKind(
        COperationStatusTile *owner,
        TransferSummaryState const &state);
    bool HostHasDeleteLikeOperation(HWND hostWindow);
    void MarkHostForMeasuredMultiRate(HWND hostWindow);

    LRESULT CALLBACK NativeProgressWindowSubclassProc(
        HWND window,
        UINT message,
        WPARAM wParam,
        LPARAM lParam,
        UINT_PTR subclassId,
        DWORD_PTR referenceData);


    int GetCircleProgress(HWND circleWindow)
    {
        std::lock_guard<std::mutex> lock(g_circleMutex);
        for (auto const &state : g_circles)
        {
            if (state.circleWindow == circleWindow)
            {
                return state.progressPercent;
            }
        }
        return 0;
    }

    void DrawProgressCircleFrame(HWND circleWindow,
                                 HDC deviceContext,
                                 RECT const &clientRect)
    {
        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;
        UINT dpi = GetDpiForWindow(circleWindow);
        if (!dpi)
        {
            dpi = USER_DEFAULT_SCREEN_DPI;
        }

        ThemePalette theme = GetDrawingTheme(circleWindow);
        TypographyConfig const &type = ActiveTypography();

        Gdiplus::Graphics graphics(deviceContext);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

        Gdiplus::SolidBrush backgroundBrush(Gdiplus::Color(
            255, GetRValue(theme.background), GetGValue(theme.background),
            GetBValue(theme.background)));
        graphics.FillRectangle(&backgroundBrush, 0, 0, width, height);

        Gdiplus::REAL diameter =
            static_cast<Gdiplus::REAL>(ScaleForDpi(kCircleDiameter, dpi));
        Gdiplus::REAL ringLeft =
            (static_cast<Gdiplus::REAL>(width) - diameter) / 2.0f;
        Gdiplus::REAL ringTop =
            static_cast<Gdiplus::REAL>(ScaleForDpi(kCircleTop, dpi));
        Gdiplus::REAL strokeWidth = static_cast<Gdiplus::REAL>(
            ScaleForDpi(kCircleStrokeWidth, dpi));
        Gdiplus::RectF ringBounds(ringLeft + strokeWidth / 2.0f,
                                  ringTop + strokeWidth / 2.0f,
                                  diameter - strokeWidth,
                                  diameter - strokeWidth);

        Gdiplus::Pen inactivePen(
            Gdiplus::Color(255, GetRValue(theme.inactive),
                           GetGValue(theme.inactive),
                           GetBValue(theme.inactive)),
            strokeWidth);
        graphics.DrawEllipse(&inactivePen, ringBounds);

        int nativeProgress = GetCircleProgress(circleWindow);
        int displayProgress = std::clamp(nativeProgress, 0, 100);
        if (displayProgress > 0)
        {
            Gdiplus::Pen accentPen(
                Gdiplus::Color(255, GetRValue(theme.accent),
                               GetGValue(theme.accent),
                               GetBValue(theme.accent)),
                strokeWidth);
            accentPen.SetStartCap(Gdiplus::LineCapRound);
            accentPen.SetEndCap(Gdiplus::LineCapRound);
            graphics.DrawArc(&accentPen, ringBounds, -90.0f,
                             static_cast<Gdiplus::REAL>(displayProgress) *
                                 3.6f);
        }

        wchar_t percentageText[16];
        wsprintfW(percentageText, L"%d%%", displayProgress);
        Gdiplus::Font percentageFont(
            type.circleFont.c_str(),
            static_cast<Gdiplus::REAL>(ScaleForDpi(type.circlePercentSize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font percentageFallback(
            L"Segoe UI",
            static_cast<Gdiplus::REAL>(ScaleForDpi(type.circlePercentSize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font labelFont(
            type.circleLabelFont.c_str(),
            static_cast<Gdiplus::REAL>(ScaleForDpi(type.circleLabelSize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font labelFallback(
            L"Segoe UI",
            static_cast<Gdiplus::REAL>(ScaleForDpi(type.circleLabelSize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font *selectedPercentageFont =
            percentageFont.GetLastStatus() == Gdiplus::Ok
                ? &percentageFont
                : &percentageFallback;
        Gdiplus::Font *selectedLabelFont =
            labelFont.GetLastStatus() == Gdiplus::Ok ? &labelFont
                                                     : &labelFallback;

        Gdiplus::SolidBrush primaryBrush(Gdiplus::Color(
            255, GetRValue(theme.primaryText), GetGValue(theme.primaryText),
            GetBValue(theme.primaryText)));
        Gdiplus::SolidBrush secondaryBrush(Gdiplus::Color(
            255, GetRValue(theme.secondaryText),
            GetGValue(theme.secondaryText), GetBValue(theme.secondaryText)));
        Gdiplus::StringFormat centeredText;
        centeredText.SetAlignment(Gdiplus::StringAlignmentCenter);
        centeredText.SetLineAlignment(Gdiplus::StringAlignmentCenter);

        Gdiplus::RectF percentageBounds(
            ringLeft, ringTop + diameter * 0.29f, diameter,
            static_cast<Gdiplus::REAL>(ScaleForDpi(42, dpi)));
        Gdiplus::RectF labelBounds(
            ringLeft, ringTop + diameter * 0.59f, diameter,
            static_cast<Gdiplus::REAL>(ScaleForDpi(24, dpi)));
        graphics.DrawString(percentageText, -1, selectedPercentageFont,
                            percentageBounds, &centeredText, &primaryBrush);
        if (ActiveElements().showCompleteLabel)
        {
            graphics.DrawString(L"Complete", -1, selectedLabelFont,
                                labelBounds, &centeredText,
                                &secondaryBrush);
        }
    }

    void PaintProgressCircle(HWND circleWindow)
    {
        PAINTSTRUCT paint;
        HDC paintDeviceContext = BeginPaint(circleWindow, &paint);
        if (!paintDeviceContext)
        {
            return;
        }

        RECT clientRect;
        if (!GetClientRect(circleWindow, &clientRect))
        {
            EndPaint(circleWindow, &paint);
            return;
        }
        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;
        if (width <= 0 || height <= 0)
        {
            EndPaint(circleWindow, &paint);
            return;
        }

        HDC memoryDeviceContext = CreateCompatibleDC(paintDeviceContext);
        HBITMAP backBuffer = memoryDeviceContext
                                 ? CreateCompatibleBitmap(
                                       paintDeviceContext, width, height)
                                 : nullptr;
        HGDIOBJ previousBitmap = backBuffer
                                     ? SelectObject(memoryDeviceContext,
                                                    backBuffer)
                                     : nullptr;
        if (memoryDeviceContext && backBuffer && previousBitmap &&
            previousBitmap != HGDI_ERROR)
        {
            DrawProgressCircleFrame(circleWindow, memoryDeviceContext,
                                    clientRect);
            if (!BitBlt(paintDeviceContext, 0, 0, width, height,
                        memoryDeviceContext, 0, 0, SRCCOPY))
            {
                DrawProgressCircleFrame(circleWindow, paintDeviceContext,
                                        clientRect);
            }
            SelectObject(memoryDeviceContext, previousBitmap);
        }
        else
        {
            DrawProgressCircleFrame(circleWindow, paintDeviceContext,
                                    clientRect);
        }

        if (backBuffer)
        {
            DeleteObject(backBuffer);
        }
        if (memoryDeviceContext)
        {
            DeleteDC(memoryDeviceContext);
        }

        EndPaint(circleWindow, &paint);
    }

    std::wstring ReadDirectUiText(DirectUI::Element *element)
    {
        if (!element || !Element_GetContentString_Optional ||
            !Value_Release_Original)
        {
            return {};
        }

        DirectUI::Value *valueToken = nullptr;
        PCWSTR text =
            Element_GetContentString_Optional(element, &valueToken);
        std::wstring copied = text ? std::wstring(text) : std::wstring{};
        if (valueToken)
        {
            Value_Release_Original(valueToken);
        }
        return copied;
    }

    void AppendDescriptionFragment(std::wstring *description,
                                   std::wstring const &fragment)
    {
        if (!description || fragment.empty())
        {
            return;
        }

        if (!description->empty() && description->back() != L' ' &&
            fragment.front() != L' ' && fragment.front() != L',' &&
            fragment.front() != L'.' && fragment.front() != L':')
        {
            description->push_back(L' ');
        }
        description->append(fragment);
    }

    std::wstring ReadNativeOperationDescription(
        TransferSummaryState const &state)
    {
        std::wstring description;
        const PCWSTR fragments[] = {
            L"eltStartText",
            L"eltFirstLocation",
            L"eltMiddleText",
            L"eltSecondLocation",
            L"eltEndText",
        };

        for (PCWSTR name : fragments)
        {
            DirectUI::Element *element = FindSkinElement(
                state.operationTileRoot, state.tileHeaderRoot, name, true);
            AppendDescriptionFragment(&description, ReadDirectUiText(element));
        }

        if (!description.empty())
        {
            return description;
        }

        // A missing optional getter must fail visually, not functionally.
        // Keep the fallback truthful without guessing Copy versus Move.
        if (state.deleteLikeKnown && state.deleteLike)
        {
            return state.totalItems == 1 ? L"Deleting 1 item"
                                         : L"Deleting items";
        }
        return L"File operation in progress";
    }

    struct InfoPanelSnapshot
    {
        int percent = 0;
        unsigned long long completedBytes = 0;
        unsigned long long totalBytes = 0;
        unsigned long long completedItems = 0;
        unsigned long long totalItems = 0;
        bool bytesValid = false;
        bool itemsValid = false;
        double nativeRate = 0.0;
        bool nativeRateValid = false;
        bool expanded = false;
        bool displayModeKnown = false;
        bool deleteLike = false;
        bool paused = false;
        std::wstring description;
        std::wstring descriptionStart;
        std::wstring firstLocation;
        std::wstring descriptionMiddle;
        std::wstring secondLocation;
        std::wstring descriptionEnd;
        std::wstring currentItemName;
        std::vector<double> rateHistory;
    };

    bool GetInfoPanelSnapshot(HWND infoWindow,
                              InfoPanelSnapshot *snapshot)
    {
        OperationTileElement *tile = nullptr;
        HWND hostWindow = nullptr;
        bool storedPaused = false;
        bool storedPausedKnown = false;
        size_t hostTileCount = 0;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [infoWindow](CircleState const &state)
                { return state.infoWindow == infoWindow; });
            if (it == g_circles.end())
            {
                return false;
            }
            tile = it->tile;
            hostWindow = it->hostWindow;
            snapshot->percent = std::clamp(it->progressPercent, 0, 100);
            storedPaused = it->paused;
            storedPausedKnown = it->pausedStateKnown;

            for (CircleState const &state : g_circles)
            {
                if (state.hostWindow == hostWindow && state.tile)
                {
                    ++hostTileCount;
                }
            }
        }

        if (storedPausedKnown)
        {
            // Per-tile state is authoritative after this operation's custom
            // Pause/Resume control has been used.
            snapshot->paused = storedPaused;
        }
        else if (hostTileCount == 1)
        {
            // For a single operation Explorer's title accurately describes
            // that one tile, so it is a safe initialization source.
            wchar_t caption[160]{};
            if (hostWindow && IsWindow(hostWindow) &&
                GetWindowTextW(hostWindow, caption, ARRAYSIZE(caption)) > 0)
            {
                snapshot->paused = wcsstr(caption, L"Paused") != nullptr;

                // Preserve the known single-tile state so it remains correct
                // if a second operation is added later and the host caption
                // becomes aggregate ("2 Actions", "2 Paused Actions", etc.).
                std::lock_guard<std::mutex> lock(g_circleMutex);
                auto stateIt = std::find_if(
                    g_circles.begin(), g_circles.end(),
                    [tile](CircleState const &state)
                    { return state.tile == tile; });
                if (stateIt != g_circles.end())
                {
                    stateIt->paused = snapshot->paused;
                    stateIt->pausedStateKnown = true;
                }
            }
        }
        else
        {
            // A multi-operation caption describes the whole host, not this
            // specific tile. New tiles start running, so never infer a tile's
            // icon from captions such as "2 Paused Actions".
            snapshot->paused = false;
        }

        std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
        auto it = std::find_if(
            g_transferSummaries.begin(), g_transferSummaries.end(),
            [tile](TransferSummaryState const &state)
            { return state.tile == tile; });
        if (it == g_transferSummaries.end())
        {
            return false;
        }

        snapshot->completedBytes = it->completedBytes;
        snapshot->totalBytes = it->totalBytes;
        if (it->resumedFromSpecialState && snapshot->totalBytes > 0 &&
            snapshot->completedBytes == 0 && snapshot->percent > 0)
        {
            long double estimated =
                static_cast<long double>(snapshot->totalBytes) *
                static_cast<long double>(snapshot->percent) / 100.0L;
            snapshot->completedBytes =
                static_cast<unsigned long long>(
                    std::clamp<long double>(
                        estimated, 0.0L,
                        static_cast<long double>(snapshot->totalBytes)));
        }
        snapshot->completedItems = it->completedItems;
        snapshot->totalItems = it->totalItems;
        snapshot->bytesValid = it->bytesValid;
        snapshot->itemsValid = it->itemsValid;
        bool useMeasuredRate =
            it->preferMeasuredRate && it->measuredDisplayRateValid;
        if (useMeasuredRate)
        {
            snapshot->nativeRate = it->measuredDisplayRate;
            snapshot->nativeRateValid = true;
        }
        else if (it->nativeDisplayRateValid)
        {
            snapshot->nativeRate = it->nativeDisplayRate;
            snapshot->nativeRateValid = true;
        }
        else
        {
            snapshot->nativeRate = it->measuredDisplayRate;
            snapshot->nativeRateValid = it->measuredDisplayRateValid;
        }
        snapshot->expanded = it->expanded;
        snapshot->displayModeKnown = it->displayModeKnown;
        snapshot->deleteLike = it->deleteLikeKnown && it->deleteLike;
        snapshot->rateHistory = it->nativeRateHistory;
        TransferSummaryState stateCopy = *it;
        // DirectUI is queried only on this UI thread and its returned buffer is
        // copied immediately. No native string pointer crosses a paint pass.
        snapshot->description = ReadNativeOperationDescription(stateCopy);

        // Preserve Explorer's semantic description fragments separately.
        // The source and destination locations are rendered with the accent
        // link treatment, while the surrounding sentence stays secondary.
        auto readDescriptionFragment = [&stateCopy](PCWSTR name)
        {
            DirectUI::Element *element = FindSkinElement(
                stateCopy.operationTileRoot, stateCopy.tileHeaderRoot,
                name, true);
            return ReadDirectUiText(element);
        };
        snapshot->descriptionStart =
            readDescriptionFragment(L"eltStartText");
        snapshot->firstLocation =
            readDescriptionFragment(L"eltFirstLocation");
        snapshot->descriptionMiddle =
            readDescriptionFragment(L"eltMiddleText");
        snapshot->secondLocation =
            readDescriptionFragment(L"eltSecondLocation");
        snapshot->descriptionEnd =
            readDescriptionFragment(L"eltEndText");
        if (snapshot->expanded)
        {
            DirectUI::Element *currentItem = FindSkinElement(
                stateCopy.operationTileRoot, stateCopy.tileHeaderRoot,
                L"eltItemName", false);
            snapshot->currentItemName = ReadDirectUiText(currentItem);
        }
        return true;
    }

    void FormatRemainingTime(InfoPanelSnapshot const &snapshot,
                             wchar_t *buffer,
                             size_t bufferLength)
    {
        if (!buffer || !bufferLength)
        {
            return;
        }

        if (snapshot.deleteLike && snapshot.itemsValid &&
            snapshot.totalItems <= snapshot.completedItems)
        {
            lstrcpynW(buffer, L"0s", static_cast<int>(bufferLength));
            return;
        }
        if (!snapshot.deleteLike && snapshot.bytesValid &&
            snapshot.totalBytes <= snapshot.completedBytes)
        {
            lstrcpynW(buffer, L"0s", static_cast<int>(bufferLength));
            return;
        }

        if (!snapshot.nativeRateValid)
        {
            lstrcpynW(buffer, L"Calculating...",
                      static_cast<int>(bufferLength));
            return;
        }

        double secondsDouble = 0.0;
        if (snapshot.deleteLike)
        {
            if (!snapshot.itemsValid || snapshot.nativeRate < 0.01 ||
                snapshot.totalItems < snapshot.completedItems)
            {
                lstrcpynW(buffer, L"Calculating...",
                          static_cast<int>(bufferLength));
                return;
            }
            secondsDouble =
                static_cast<double>(snapshot.totalItems -
                                    snapshot.completedItems) /
                snapshot.nativeRate;
        }
        else
        {
            if (!snapshot.bytesValid || snapshot.nativeRate < 1.0 ||
                snapshot.totalBytes < snapshot.completedBytes)
            {
                lstrcpynW(buffer, L"Calculating...",
                          static_cast<int>(bufferLength));
                return;
            }
            secondsDouble =
                static_cast<double>(snapshot.totalBytes -
                                    snapshot.completedBytes) /
                snapshot.nativeRate;
        }
        unsigned long long seconds =
            static_cast<unsigned long long>(
                std::max(0.0, std::ceil(secondsDouble)));

        if (seconds < 60)
        {
            std::swprintf(buffer, bufferLength, L"%llus", seconds);
        }
        else if (seconds < 3600)
        {
            unsigned long long minutes = seconds / 60;
            unsigned long long remainder = seconds % 60;
            if (remainder)
            {
                std::swprintf(buffer, bufferLength, L"%llum %llus",
                              minutes, remainder);
            }
            else
            {
                std::swprintf(buffer, bufferLength, L"%llum", minutes);
            }
        }
        else
        {
            unsigned long long hours = seconds / 3600;
            unsigned long long minutes = (seconds % 3600) / 60;
            std::swprintf(buffer, bufferLength, L"%lluh %llum",
                          hours, minutes);
        }
    }

    void FillCapsule(Gdiplus::Graphics &graphics,
                     Gdiplus::Brush &brush,
                     Gdiplus::REAL x,
                     Gdiplus::REAL y,
                     Gdiplus::REAL width,
                     Gdiplus::REAL height)
    {
        if (width <= 0.0f || height <= 0.0f)
        {
            return;
        }

        // For very short progress values, keep the fill visually rounded
        // instead of producing a square sliver. Normal-width bars use true
        // semicircular end caps.
        if (width <= height)
        {
            graphics.FillEllipse(&brush, x, y, width, height);
            return;
        }

        Gdiplus::REAL radius = height / 2.0f;
        Gdiplus::GraphicsPath path;
        path.AddArc(x, y, height, height, 90.0f, 180.0f);
        path.AddLine(x + radius, y, x + width - radius, y);
        path.AddArc(x + width - height, y, height, height, 270.0f, 180.0f);
        path.AddLine(x + width - radius, y + height, x + radius, y + height);
        path.CloseFigure();
        graphics.FillPath(&brush, &path);
    }

    void GetInfoPanelPauseRect(HWND infoWindow, RECT *pauseRect);
    void GetInfoPanelCancelRect(HWND infoWindow, RECT *cancelRect);

    void DrawEmbeddedProgressCircle(Gdiplus::Graphics &graphics,
                                    UINT dpi,
                                    int displayProgress,
                                    ThemePalette const &theme,
                                    TypographyConfig const &type)
    {
        if (!ActiveElements().showCircle)
        {
            return;
        }

        // The progress ring is intentionally rendered into the SAME buffered
        // presentation HWND as the rest of the normal-operation content.
        // Older 0.12 builds used a separate sibling child HWND for the ring;
        // Explorer could restack OperationTileHost between the siblings while
        // moving or transitioning More/Fewer, making the ring flicker or
        // disappear. One surface means one Z-order owner and one paint pass.
        Gdiplus::REAL diameter =
            static_cast<Gdiplus::REAL>(ScaleForDpi(kCircleDiameter, dpi));
        Gdiplus::REAL columnWidth =
            static_cast<Gdiplus::REAL>(ScaleForDpi(kCircleColumnWidth, dpi));
        Gdiplus::REAL ringLeft =
            (columnWidth - diameter) / 2.0f +
            static_cast<Gdiplus::REAL>(
                ScaleForDpi(ActiveLayout().circleXOffset, dpi));
        Gdiplus::REAL ringTop = static_cast<Gdiplus::REAL>(
            ScaleForDpi(kCircleHostY + kCircleTop, dpi));
        Gdiplus::REAL strokeWidth = static_cast<Gdiplus::REAL>(
            ScaleForDpi(kCircleStrokeWidth, dpi));
        Gdiplus::RectF ringBounds(
            ringLeft + strokeWidth / 2.0f,
            ringTop + strokeWidth / 2.0f,
            diameter - strokeWidth,
            diameter - strokeWidth);

        Gdiplus::Pen inactivePen(
            Gdiplus::Color(255, GetRValue(theme.inactive),
                           GetGValue(theme.inactive),
                           GetBValue(theme.inactive)),
            strokeWidth);
        graphics.DrawEllipse(&inactivePen, ringBounds);

        displayProgress = std::clamp(displayProgress, 0, 100);
        if (displayProgress > 0)
        {
            Gdiplus::Pen accentPen(
                Gdiplus::Color(255, GetRValue(theme.accent),
                               GetGValue(theme.accent),
                               GetBValue(theme.accent)),
                strokeWidth);
            accentPen.SetStartCap(Gdiplus::LineCapRound);
            accentPen.SetEndCap(Gdiplus::LineCapRound);
            graphics.DrawArc(&accentPen, ringBounds, -90.0f,
                             static_cast<Gdiplus::REAL>(displayProgress) *
                                 3.6f);
        }

        wchar_t percentageText[16]{};
        wsprintfW(percentageText, L"%d%%", displayProgress);

        Gdiplus::Font percentageFont(
            type.circleFont.c_str(),
            static_cast<Gdiplus::REAL>(
                ScaleForDpi(type.circlePercentSize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font percentageFallback(
            L"Segoe UI",
            static_cast<Gdiplus::REAL>(
                ScaleForDpi(type.circlePercentSize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font labelFont(
            type.circleLabelFont.c_str(),
            static_cast<Gdiplus::REAL>(
                ScaleForDpi(type.circleLabelSize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font labelFallback(
            L"Segoe UI",
            static_cast<Gdiplus::REAL>(
                ScaleForDpi(type.circleLabelSize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font *selectedPercentageFont =
            percentageFont.GetLastStatus() == Gdiplus::Ok
                ? &percentageFont
                : &percentageFallback;
        Gdiplus::Font *selectedLabelFont =
            labelFont.GetLastStatus() == Gdiplus::Ok
                ? &labelFont
                : &labelFallback;

        Gdiplus::SolidBrush primaryBrush(Gdiplus::Color(
            255, GetRValue(theme.primaryText), GetGValue(theme.primaryText),
            GetBValue(theme.primaryText)));
        Gdiplus::SolidBrush secondaryBrush(Gdiplus::Color(
            255, GetRValue(theme.secondaryText),
            GetGValue(theme.secondaryText), GetBValue(theme.secondaryText)));
        Gdiplus::StringFormat centeredText;
        centeredText.SetAlignment(Gdiplus::StringAlignmentCenter);
        centeredText.SetLineAlignment(Gdiplus::StringAlignmentCenter);

        Gdiplus::RectF percentageBounds(
            ringLeft, ringTop + diameter * 0.29f, diameter,
            static_cast<Gdiplus::REAL>(ScaleForDpi(42, dpi)));
        Gdiplus::RectF labelBounds(
            ringLeft, ringTop + diameter * 0.59f, diameter,
            static_cast<Gdiplus::REAL>(ScaleForDpi(24, dpi)));
        graphics.DrawString(percentageText, -1, selectedPercentageFont,
                            percentageBounds, &centeredText, &primaryBrush);
        if (ActiveElements().showCompleteLabel)
        {
            graphics.DrawString(L"Complete", -1, selectedLabelFont,
                                labelBounds, &centeredText,
                                &secondaryBrush);
        }
    }

    void DrawInfoPanelFrame(HWND infoWindow,
                            HDC deviceContext,
                            RECT const &clientRect)
    {
        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;
        if (width <= 0 || height <= 0)
        {
            return;
        }

        UINT dpi = GetDpiForWindow(infoWindow);
        if (!dpi)
        {
            dpi = USER_DEFAULT_SCREEN_DPI;
        }

        InfoPanelSnapshot snapshot{};
        GetInfoPanelSnapshot(infoWindow, &snapshot);
        ThemePalette theme = GetDrawingTheme(infoWindow);
        TypographyConfig const &type = ActiveTypography();
        ElementConfig const &elements = ActiveElements();

        Gdiplus::Graphics graphics(deviceContext);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        graphics.SetTextRenderingHint(
            Gdiplus::TextRenderingHintClearTypeGridFit);

        Gdiplus::SolidBrush backgroundBrush(Gdiplus::Color(
            255, GetRValue(theme.background), GetGValue(theme.background),
            GetBValue(theme.background)));
        graphics.FillRectangle(&backgroundBrush, 0, 0, width, height);

        DrawEmbeddedProgressCircle(
            graphics, dpi, snapshot.percent, theme, type);

        Gdiplus::Font detailFont(
            type.bodyFont.c_str(),
            static_cast<Gdiplus::REAL>(ScaleForDpi(type.bodySize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font detailFallback(
            L"Segoe UI",
            static_cast<Gdiplus::REAL>(ScaleForDpi(type.bodySize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font *selectedFont =
            detailFont.GetLastStatus() == Gdiplus::Ok
                ? &detailFont
                : &detailFallback;

        Gdiplus::SolidBrush primaryBrush(Gdiplus::Color(
            255, GetRValue(theme.primaryText), GetGValue(theme.primaryText),
            GetBValue(theme.primaryText)));
        Gdiplus::SolidBrush secondaryBrush(Gdiplus::Color(
            255, GetRValue(theme.secondaryText),
            GetGValue(theme.secondaryText), GetBValue(theme.secondaryText)));
        Gdiplus::SolidBrush linkBrush(Gdiplus::Color(
            255, GetRValue(theme.accent),
            GetGValue(theme.accent), GetBValue(theme.accent)));
        Gdiplus::Font linkFont(
            type.bodyFont.c_str(),
            static_cast<Gdiplus::REAL>(ScaleForDpi(type.bodySize, dpi)),
            Gdiplus::FontStyleUnderline, Gdiplus::UnitPixel);
        Gdiplus::Font linkFallback(
            L"Segoe UI",
            static_cast<Gdiplus::REAL>(ScaleForDpi(type.bodySize, dpi)),
            Gdiplus::FontStyleUnderline, Gdiplus::UnitPixel);
        Gdiplus::Font *selectedLinkFont =
            linkFont.GetLastStatus() == Gdiplus::Ok
                ? &linkFont
                : &linkFallback;
        // Every visible coordinate is expressed in 96-DPI logical units and
        // converted exactly once at the HWND/GDI+ boundary. The native
        // DirectUI tree contributes data and actions, never primary geometry.
        int contentLeft = ScaleForDpi(
            kReservedLeftWidth + ActiveLayout().infoXOffset, dpi);
        int contentRight = std::max(
            contentLeft + 1,
            width - ScaleForDpi(kContentRightPadding, dpi));
        int contentWidth = std::max(contentRight - contentLeft, 1);
        int detailsOffset = ScaleForDpi(kInfoPanelTop, dpi);
        bool showCurrentItem =
            snapshot.expanded &&
            snapshot.currentItemName.find_first_not_of(L" \t\r\n") !=
                std::wstring::npos;
        int expandedDetailOffset =
            showCurrentItem ? ScaleForDpi(16, dpi) : 0;

        if (elements.showDescription)
        {
            // Keep the sentence itself subdued but render Explorer's source
            // and destination location fragments as link-like accent text.
            // They remain presentation-only here; native DirectUI owns the
            // operation and its semantic controls underneath.
            Gdiplus::RectF descriptionBounds(
                static_cast<Gdiplus::REAL>(contentLeft),
                static_cast<Gdiplus::REAL>(ScaleForDpi(10, dpi)),
                static_cast<Gdiplus::REAL>(
                    std::max(contentWidth - ScaleForDpi(84, dpi), 1)),
                static_cast<Gdiplus::REAL>(ScaleForDpi(20, dpi)));

            bool hasStructuredDescription =
                !snapshot.descriptionStart.empty() ||
                !snapshot.firstLocation.empty() ||
                !snapshot.descriptionMiddle.empty() ||
                !snapshot.secondLocation.empty() ||
                !snapshot.descriptionEnd.empty();

            if (hasStructuredDescription)
            {
                Gdiplus::GraphicsState clipState = graphics.Save();
                graphics.SetClip(descriptionBounds);

                Gdiplus::REAL x = descriptionBounds.X;
                Gdiplus::REAL y = descriptionBounds.Y;
                bool drewAnything = false;

                // Measure adjacent fragments typographically. The default
                // GDI+ MeasureString path adds extra side bearings to each
                // independently measured string, which looked like multiple
                // spaces between "from", the source link, "to", and the
                // destination link.
                Gdiplus::StringFormat segmentFormat(
                    Gdiplus::StringFormat::GenericTypographic());
                segmentFormat.SetFormatFlags(
                    Gdiplus::StringFormatFlagsNoWrap |
                    Gdiplus::StringFormatFlagsMeasureTrailingSpaces);

                auto trimDescriptionSegment =
                    [](std::wstring value)
                {
                    auto isSpace = [](wchar_t ch)
                    {
                        return ch == L' ' || ch == L'\t' ||
                               ch == L'\r' || ch == L'\n';
                    };

                    while (!value.empty() && isSpace(value.front()))
                    {
                        value.erase(value.begin());
                    }
                    while (!value.empty() && isSpace(value.back()))
                    {
                        value.pop_back();
                    }
                    return value;
                };

                auto measureAdvance =
                    [&](PCWSTR value, int length, Gdiplus::Font *font)
                    -> Gdiplus::REAL
                {
                    Gdiplus::RectF measured{};
                    graphics.MeasureString(
                        value, length, font,
                        Gdiplus::PointF(x, y),
                        &segmentFormat, &measured);
                    return measured.Width;
                };

                auto drawDescriptionSegment =
                    [&](std::wstring const &rawSegment,
                        Gdiplus::Font *font,
                        Gdiplus::SolidBrush *brush)
                {
                    if (!font || !brush)
                    {
                        return;
                    }

                    std::wstring segment =
                        trimDescriptionSegment(rawSegment);
                    if (segment.empty())
                    {
                        return;
                    }

                    bool punctuation =
                        segment.front() == L',' ||
                        segment.front() == L'.' ||
                        segment.front() == L':' ||
                        segment.front() == L';';

                    if (drewAnything && !punctuation)
                    {
                        Gdiplus::REAL spaceWidth =
                            measureAdvance(L" ", 1, selectedFont);
                        x += spaceWidth;
                    }

                    graphics.DrawString(
                        segment.c_str(), -1, font,
                        Gdiplus::PointF(x, y),
                        &segmentFormat, brush);
                    x += measureAdvance(
                        segment.c_str(), -1, font);
                    drewAnything = true;
                };

                drawDescriptionSegment(
                    snapshot.descriptionStart, selectedFont,
                    &secondaryBrush);
                drawDescriptionSegment(
                    snapshot.firstLocation, selectedLinkFont,
                    &linkBrush);
                drawDescriptionSegment(
                    snapshot.descriptionMiddle, selectedFont,
                    &secondaryBrush);
                drawDescriptionSegment(
                    snapshot.secondLocation, selectedLinkFont,
                    &linkBrush);
                drawDescriptionSegment(
                    snapshot.descriptionEnd, selectedFont,
                    &secondaryBrush);

                graphics.Restore(clipState);
            }
            else
            {
                Gdiplus::StringFormat descriptionFormat;
                descriptionFormat.SetFormatFlags(
                    Gdiplus::StringFormatFlagsNoWrap);
                descriptionFormat.SetTrimming(
                    Gdiplus::StringTrimmingEllipsisCharacter);
                graphics.DrawString(
                    snapshot.description.c_str(), -1,
                    selectedFont, descriptionBounds,
                    &descriptionFormat, &secondaryBrush);
            }
        }

        if (elements.showSummary)
        {
            wchar_t completedText[64]{};
            wchar_t totalText[64]{};
            wchar_t summaryText[160]{};
            if (snapshot.bytesValid &&
                StrFormatByteSizeW(
                    static_cast<LONGLONG>(snapshot.completedBytes),
                    completedText, ARRAYSIZE(completedText)) &&
                StrFormatByteSizeW(
                    static_cast<LONGLONG>(snapshot.totalBytes),
                    totalText, ARRAYSIZE(totalText)))
            {
                std::swprintf(summaryText, ARRAYSIZE(summaryText),
                              L"%s / %s", completedText, totalText);
            }
            else
            {
                lstrcpynW(summaryText, L"Calculating...",
                          ARRAYSIZE(summaryText));
            }

            Gdiplus::Font summaryFont(
                type.summaryFont.c_str(),
                static_cast<Gdiplus::REAL>(
                    ScaleForDpi(type.summarySize, dpi)),
                type.summaryWeight >= 600 ? Gdiplus::FontStyleBold
                                          : Gdiplus::FontStyleRegular,
                Gdiplus::UnitPixel);
            Gdiplus::Font summaryFallback(
                L"Segoe UI",
                static_cast<Gdiplus::REAL>(
                    ScaleForDpi(type.summarySize, dpi)),
                type.summaryWeight >= 600 ? Gdiplus::FontStyleBold
                                          : Gdiplus::FontStyleRegular,
                Gdiplus::UnitPixel);
            Gdiplus::Font *selectedSummaryFont =
                summaryFont.GetLastStatus() == Gdiplus::Ok
                    ? &summaryFont
                    : &summaryFallback;
            Gdiplus::StringFormat summaryFormat;
            summaryFormat.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
            summaryFormat.SetTrimming(
                Gdiplus::StringTrimmingEllipsisCharacter);
            Gdiplus::RectF summaryBounds(
                static_cast<Gdiplus::REAL>(contentLeft),
                static_cast<Gdiplus::REAL>(ScaleForDpi(27, dpi)),
                static_cast<Gdiplus::REAL>(
                    std::max(contentWidth - ScaleForDpi(84, dpi), 1)),
                static_cast<Gdiplus::REAL>(ScaleForDpi(38, dpi)));
            graphics.DrawString(summaryText, -1, selectedSummaryFont,
                                summaryBounds, &summaryFormat,
                                &primaryBrush);
        }

        if (showCurrentItem)
        {
            Gdiplus::StringFormat currentItemFormat;
            currentItemFormat.SetFormatFlags(
                Gdiplus::StringFormatFlagsNoWrap);
            currentItemFormat.SetTrimming(
                Gdiplus::StringTrimmingEllipsisCharacter);
            Gdiplus::RectF currentItemBounds(
                static_cast<Gdiplus::REAL>(contentLeft),
                static_cast<Gdiplus::REAL>(
                    detailsOffset - ScaleForDpi(6, dpi)),
                static_cast<Gdiplus::REAL>(contentWidth),
                static_cast<Gdiplus::REAL>(ScaleForDpi(20, dpi)));
            graphics.DrawString(
                snapshot.currentItemName.c_str(), -1, selectedFont,
                currentItemBounds, &currentItemFormat, &secondaryBrush);
        }

        wchar_t rateSize[64]{};
        wchar_t rateValue[80]{};
        wchar_t timeText[64]{};
        if (snapshot.deleteLike)
        {
            if (snapshot.nativeRateValid)
            {
                double rate = std::max(0.0, snapshot.nativeRate);
                double rounded = std::round(rate);
                if (std::fabs(rate - rounded) < 0.05)
                {
                    std::swprintf(rateValue, ARRAYSIZE(rateValue),
                                  L"%.0f items/s", rounded);
                }
                else
                {
                    std::swprintf(rateValue, ARRAYSIZE(rateValue),
                                  L"%.1f items/s", rate);
                }
            }
            else
            {
                lstrcpynW(rateValue, L"\x2014", ARRAYSIZE(rateValue));
            }
        }
        else
        {
            if (snapshot.nativeRateValid)
            {
                unsigned long long roundedRate =
                    static_cast<unsigned long long>(
                        std::max(0.0, snapshot.nativeRate) + 0.5);
                StrFormatByteSizeW(static_cast<LONGLONG>(roundedRate),
                                   rateSize, ARRAYSIZE(rateSize));
            }
            else
            {
                lstrcpynW(rateSize, L"\x2014", ARRAYSIZE(rateSize));
            }
            std::swprintf(rateValue, ARRAYSIZE(rateValue),
                          L"%s/s", rateSize);
        }
        FormatRemainingTime(snapshot, timeText, ARRAYSIZE(timeText));

        // Labels stay subdued like the Items remaining row; only the live
        // transfer values are promoted to the primary foreground.
        auto drawInlineSegment = [&](PCWSTR text,
                                     Gdiplus::SolidBrush *brush,
                                     Gdiplus::REAL y,
                                     Gdiplus::REAL *x)
        {
            if (!text || !*text || !brush || !x)
            {
                return;
            }
            Gdiplus::PointF origin(*x, y);
            Gdiplus::RectF measured{};
            graphics.MeasureString(text, -1, selectedFont, origin, &measured);
            graphics.DrawString(text, -1, selectedFont, origin, brush);
            *x += measured.Width;
        };

        if (elements.showSpeedTime)
        {
            Gdiplus::REAL speedX =
                static_cast<Gdiplus::REAL>(contentLeft);
            Gdiplus::REAL speedY = static_cast<Gdiplus::REAL>(
                detailsOffset + expandedDetailOffset +
                ScaleForDpi(kInfoPanelSpeedTop, dpi));
            drawInlineSegment(L"Speed: ", &secondaryBrush, speedY, &speedX);
            drawInlineSegment(rateValue, &primaryBrush, speedY, &speedX);
            drawInlineSegment(L"  \x2022  Time remaining: ", &secondaryBrush,
                              speedY, &speedX);
            drawInlineSegment(timeText, &primaryBrush, speedY, &speedX);
        }


        wchar_t remainingSize[64]{};
        wchar_t itemsValue[128]{};
        unsigned long long remainingItems = 0;
        unsigned long long remainingBytes = 0;
        if (snapshot.itemsValid &&
            snapshot.totalItems >= snapshot.completedItems)
        {
            remainingItems = snapshot.totalItems - snapshot.completedItems;
        }
        if (snapshot.bytesValid &&
            snapshot.totalBytes >= snapshot.completedBytes)
        {
            remainingBytes = snapshot.totalBytes - snapshot.completedBytes;
            StrFormatByteSizeW(static_cast<LONGLONG>(remainingBytes),
                               remainingSize, ARRAYSIZE(remainingSize));
        }

        if (snapshot.itemsValid && snapshot.bytesValid)
        {
            std::swprintf(itemsValue, ARRAYSIZE(itemsValue), L"%llu (%s)",
                          remainingItems, remainingSize);
        }
        else if (snapshot.itemsValid)
        {
            std::swprintf(itemsValue, ARRAYSIZE(itemsValue), L"%llu",
                          remainingItems);
        }
        else
        {
            lstrcpynW(itemsValue, L"Calculating...", ARRAYSIZE(itemsValue));
        }

        if (elements.showItems)
        {
            Gdiplus::REAL itemsX =
                static_cast<Gdiplus::REAL>(contentLeft);
            Gdiplus::REAL itemsY = static_cast<Gdiplus::REAL>(
                detailsOffset + expandedDetailOffset +
                ScaleForDpi(kInfoPanelItemsTop, dpi));
            drawInlineSegment(L"Items remaining: ", &secondaryBrush, itemsY,
                              &itemsX);
            drawInlineSegment(itemsValue, &primaryBrush, itemsY, &itemsX);
        }


        if (elements.showProgressBar)
        {
            Gdiplus::REAL progressTop = static_cast<Gdiplus::REAL>(
                detailsOffset + expandedDetailOffset +
                ScaleForDpi(kInfoPanelProgressTop, dpi));
            Gdiplus::REAL progressHeight = static_cast<Gdiplus::REAL>(
                ScaleForDpi(kInfoPanelProgressHeight, dpi));
            Gdiplus::REAL progressWidth =
                static_cast<Gdiplus::REAL>(contentWidth);
            Gdiplus::SolidBrush progressTrack(Gdiplus::Color(
                255, GetRValue(theme.inactive),
                GetGValue(theme.inactive), GetBValue(theme.inactive)));
            Gdiplus::SolidBrush progressFill(Gdiplus::Color(
                255, GetRValue(theme.accent),
                GetGValue(theme.accent), GetBValue(theme.accent)));
            FillCapsule(graphics, progressTrack,
                        static_cast<Gdiplus::REAL>(contentLeft), progressTop,
                        progressWidth, progressHeight);
            Gdiplus::REAL completedWidth =
                progressWidth *
                static_cast<Gdiplus::REAL>(
                    std::clamp(snapshot.percent, 0, 100)) /
                100.0f;
            if (completedWidth > 0.0f)
            {
                FillCapsule(graphics, progressFill,
                            static_cast<Gdiplus::REAL>(contentLeft),
                            progressTop,
                            completedWidth, progressHeight);
            }
        }


        // The visible control is deliberately minimal like Explorer's native
        // pause/resume affordance. Its click still invokes the live native
        // DirectUI action; only presentation is custom.
        RECT pauseRect{};
        GetInfoPanelPauseRect(infoWindow, &pauseRect);
        Gdiplus::Pen actionPen(Gdiplus::Color(
            255, GetRValue(theme.actionText), GetGValue(theme.actionText),
            GetBValue(theme.actionText)),
            static_cast<Gdiplus::REAL>(ScaleForDpi(2, dpi)));
        Gdiplus::REAL actionCenterX =
            static_cast<Gdiplus::REAL>(pauseRect.left + pauseRect.right) /
            2.0f;
        Gdiplus::REAL actionCenterY =
            static_cast<Gdiplus::REAL>(pauseRect.top + pauseRect.bottom) /
            2.0f;
        Gdiplus::REAL actionHalfHeight =
            static_cast<Gdiplus::REAL>(ScaleForDpi(7, dpi));
        if (snapshot.paused)
        {
            Gdiplus::GraphicsPath playPath;
            playPath.AddLine(actionCenterX - ScaleForDpi(4, dpi),
                             actionCenterY - actionHalfHeight,
                             actionCenterX + ScaleForDpi(6, dpi),
                             actionCenterY);
            playPath.AddLine(actionCenterX + ScaleForDpi(6, dpi),
                             actionCenterY,
                             actionCenterX - ScaleForDpi(4, dpi),
                             actionCenterY + actionHalfHeight);
            playPath.CloseFigure();
            Gdiplus::SolidBrush playBrush(Gdiplus::Color(
                255, GetRValue(theme.actionText),
                GetGValue(theme.actionText), GetBValue(theme.actionText)));
            graphics.FillPath(&playBrush, &playPath);
        }
        else
        {
            Gdiplus::REAL barOffset =
                static_cast<Gdiplus::REAL>(ScaleForDpi(3, dpi));
            graphics.DrawLine(&actionPen,
                              actionCenterX - barOffset,
                              actionCenterY - actionHalfHeight,
                              actionCenterX - barOffset,
                              actionCenterY + actionHalfHeight);
            graphics.DrawLine(&actionPen,
                              actionCenterX + barOffset,
                              actionCenterY - actionHalfHeight,
                              actionCenterX + barOffset,
                              actionCenterY + actionHalfHeight);
        }

        if (elements.showCancel)
        {
            RECT cancelRect{};
            GetInfoPanelCancelRect(infoWindow, &cancelRect);
            Gdiplus::REAL cancelCenterX =
                static_cast<Gdiplus::REAL>(
                    cancelRect.left + cancelRect.right) / 2.0f;
            Gdiplus::REAL cancelCenterY =
                static_cast<Gdiplus::REAL>(
                    cancelRect.top + cancelRect.bottom) / 2.0f;
            Gdiplus::REAL cancelHalf =
                static_cast<Gdiplus::REAL>(ScaleForDpi(5, dpi));
            graphics.DrawLine(
                &actionPen,
                cancelCenterX - cancelHalf,
                cancelCenterY - cancelHalf,
                cancelCenterX + cancelHalf,
                cancelCenterY + cancelHalf);
            graphics.DrawLine(
                &actionPen,
                cancelCenterX + cancelHalf,
                cancelCenterY - cancelHalf,
                cancelCenterX - cancelHalf,
                cancelCenterY + cancelHalf);
        }

        if (!snapshot.expanded || !elements.showGraph)
        {
            return;
        }

        int chartTop = detailsOffset +
                       ScaleForDpi(kInfoPanelChartTop, dpi);
        int chartHeight = ScaleForDpi(kInfoPanelChartHeight, dpi);
        int chartLeft = ScaleForDpi(kContentRightPadding, dpi);
        int chartWidth = std::max(contentRight - chartLeft, 0);
        if (chartTop + chartHeight > height || chartWidth <= 0)
        {
            return;
        }

        graphics.TranslateTransform(
            static_cast<Gdiplus::REAL>(chartLeft), 0.0f);

        Gdiplus::SolidBrush chartBackground(Gdiplus::Color(
            255, GetRValue(theme.graphSurface),
            GetGValue(theme.graphSurface), GetBValue(theme.graphSurface)));
        graphics.FillRectangle(&chartBackground, 0, chartTop,
                               chartWidth, chartHeight);

        Gdiplus::Pen gridPen(
            Gdiplus::Color(
                theme.graphGridAlpha,
                GetRValue(theme.graphGrid),
                GetGValue(theme.graphGrid),
                GetBValue(theme.graphGrid)),
            1.0f);
        for (int row = 1; row < 4; ++row)
        {
            Gdiplus::REAL y = static_cast<Gdiplus::REAL>(
                chartTop + (chartHeight * row) / 4);
            graphics.DrawLine(&gridPen, 0.0f, y,
                              static_cast<Gdiplus::REAL>(chartWidth), y);
        }
        for (int column = 1; column < 6; ++column)
        {
            Gdiplus::REAL x = static_cast<Gdiplus::REAL>(
                (chartWidth * column) / 6);
            graphics.DrawLine(&gridPen, x,
                              static_cast<Gdiplus::REAL>(chartTop), x,
                              static_cast<Gdiplus::REAL>(
                                  chartTop + chartHeight));
        }

        if (snapshot.rateHistory.empty())
        {
            return;
        }

        size_t firstUsableSample = 0;
        while (firstUsableSample + 1 < snapshot.rateHistory.size())
        {
            double rate = snapshot.rateHistory[firstUsableSample];
            if (std::isfinite(rate) && rate > 0.0)
            {
                break;
            }
            ++firstUsableSample;
        }

        std::vector<double> visibleHistory(
            snapshot.rateHistory.begin() + firstUsableSample,
            snapshot.rateHistory.end());
        if (visibleHistory.empty())
        {
            return;
        }

        double maximumRate = 1.0;
        for (double rate : visibleHistory)
        {
            if (std::isfinite(rate))
            {
                maximumRate = std::max(maximumRate, rate);
            }
        }

        size_t sampleCount = visibleHistory.size();
        std::vector<Gdiplus::PointF> linePoints;
        linePoints.reserve(sampleCount);
        for (size_t index = 0; index < sampleCount; ++index)
        {
            double rate = std::isfinite(visibleHistory[index])
                              ? std::max(0.0, visibleHistory[index])
                              : 0.0;
            Gdiplus::REAL x = sampleCount > 1
                                  ? static_cast<Gdiplus::REAL>(
                                        (static_cast<double>(index) /
                                         static_cast<double>(
                                             sampleCount - 1)) *
                                        (chartWidth - 1))
                                  : 0.0f;
            Gdiplus::REAL normalized =
                static_cast<Gdiplus::REAL>(rate / maximumRate);
            Gdiplus::REAL y =
                static_cast<Gdiplus::REAL>(chartTop + chartHeight - 1) -
                normalized *
                    static_cast<Gdiplus::REAL>(chartHeight - 2);
            linePoints.emplace_back(x, y);
        }

        if (linePoints.size() >= 2)
        {
            std::vector<Gdiplus::PointF> fillPoints;
            fillPoints.reserve(linePoints.size() + 2);
            fillPoints.emplace_back(
                linePoints.front().X,
                static_cast<Gdiplus::REAL>(chartTop + chartHeight));
            fillPoints.insert(fillPoints.end(), linePoints.begin(),
                              linePoints.end());
            fillPoints.emplace_back(
                linePoints.back().X,
                static_cast<Gdiplus::REAL>(chartTop + chartHeight));

            Gdiplus::SolidBrush chartFill(Gdiplus::Color(
                theme.graphFillAlpha, GetRValue(theme.graphFill),
                GetGValue(theme.graphFill), GetBValue(theme.graphFill)));
            Gdiplus::Pen chartLine(Gdiplus::Color(
                255, GetRValue(theme.graphLine),
                GetGValue(theme.graphLine), GetBValue(theme.graphLine)),
                1.5f);
            graphics.FillPolygon(&chartFill, fillPoints.data(),
                                 static_cast<INT>(fillPoints.size()));
            graphics.DrawLines(&chartLine, linePoints.data(),
                               static_cast<INT>(linePoints.size()));
        }

        // Native-inspired graph readout: current speed is represented by a
        // horizontal reference line at its actual level on the graph. The
        // numeric value belongs to that line, so it doesn't read as a second
        // copy of the status-row "Speed:" label.
        if (snapshot.nativeRateValid && maximumRate > 0.0)
        {
            double currentRate = std::clamp(snapshot.nativeRate, 0.0,
                                            maximumRate);
            Gdiplus::REAL normalized =
                static_cast<Gdiplus::REAL>(currentRate / maximumRate);
            Gdiplus::REAL referenceY =
                static_cast<Gdiplus::REAL>(chartTop + chartHeight - 1) -
                normalized *
                    static_cast<Gdiplus::REAL>(chartHeight - 2);

            Gdiplus::Pen referencePen(Gdiplus::Color(
                theme.graphReferenceAlpha, GetRValue(theme.secondaryText),
                GetGValue(theme.secondaryText),
                GetBValue(theme.secondaryText)),
                1.0f);
            graphics.DrawLine(&referencePen, 0.0f, referenceY,
                              static_cast<Gdiplus::REAL>(chartWidth),
                              referenceY);

            Gdiplus::Font graphValueFont(
                type.bodyFont.c_str(),
                static_cast<Gdiplus::REAL>(ScaleForDpi(type.graphValueSize, dpi)),
                Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
            Gdiplus::Font graphValueFallback(
                L"Segoe UI",
                static_cast<Gdiplus::REAL>(ScaleForDpi(type.graphValueSize, dpi)),
                Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
            Gdiplus::Font *selectedGraphValueFont =
                graphValueFont.GetLastStatus() == Gdiplus::Ok
                    ? &graphValueFont
                    : &graphValueFallback;

            Gdiplus::PointF measureOrigin(0.0f, 0.0f);
            Gdiplus::RectF measured{};
            graphics.MeasureString(rateValue, -1, selectedGraphValueFont,
                                   measureOrigin, &measured);

            Gdiplus::REAL labelX =
                std::max(0.0f,
                         static_cast<Gdiplus::REAL>(chartWidth) -
                             measured.Width -
                             static_cast<Gdiplus::REAL>(
                                 ScaleForDpi(5, dpi)));
            Gdiplus::REAL labelY =
                std::clamp(
                    referenceY -
                        static_cast<Gdiplus::REAL>(
                            ScaleForDpi(14, dpi)),
                    static_cast<Gdiplus::REAL>(chartTop + 1),
                    static_cast<Gdiplus::REAL>(
                        chartTop + chartHeight -
                        ScaleForDpi(14, dpi)));

            // Small background patch keeps the readout legible when the
            // history line passes behind it.
            Gdiplus::SolidBrush labelBackground(Gdiplus::Color(
                theme.graphLabelBackgroundAlpha, GetRValue(theme.graphSurface),
                GetGValue(theme.graphSurface),
                GetBValue(theme.graphSurface)));
            Gdiplus::RectF labelBackgroundRect(
                labelX - 2.0f, labelY,
                measured.Width + 4.0f,
                static_cast<Gdiplus::REAL>(ScaleForDpi(14, dpi)));
            graphics.FillRectangle(&labelBackground, labelBackgroundRect);
            graphics.DrawString(rateValue, -1, selectedGraphValueFont,
                                Gdiplus::PointF(labelX, labelY),
                                &primaryBrush);
        }
    }

    void PaintInfoPanel(HWND infoWindow)
    {
        PAINTSTRUCT paint{};
        HDC paintDeviceContext = BeginPaint(infoWindow, &paint);
        if (!paintDeviceContext)
        {
            return;
        }

        RECT clientRect{};
        if (!GetClientRect(infoWindow, &clientRect))
        {
            EndPaint(infoWindow, &paint);
            return;
        }

        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;
        HDC memoryDeviceContext =
            width > 0 && height > 0
                ? CreateCompatibleDC(paintDeviceContext)
                : nullptr;
        HBITMAP backBuffer =
            memoryDeviceContext
                ? CreateCompatibleBitmap(paintDeviceContext, width, height)
                : nullptr;
        HGDIOBJ previousBitmap =
            backBuffer
                ? SelectObject(memoryDeviceContext, backBuffer)
                : nullptr;

        if (memoryDeviceContext && backBuffer && previousBitmap &&
            previousBitmap != HGDI_ERROR)
        {
            DrawInfoPanelFrame(infoWindow, memoryDeviceContext, clientRect);
            BitBlt(paintDeviceContext, 0, 0, width, height,
                   memoryDeviceContext, 0, 0, SRCCOPY);
            SelectObject(memoryDeviceContext, previousBitmap);
        }
        else
        {
            DrawInfoPanelFrame(infoWindow, paintDeviceContext, clientRect);
        }

        if (backBuffer)
        {
            DeleteObject(backBuffer);
        }
        if (memoryDeviceContext)
        {
            DeleteDC(memoryDeviceContext);
        }
        EndPaint(infoWindow, &paint);
    }

    void GetInfoPanelCancelRect(HWND infoWindow, RECT *cancelRect)
    {
        if (!cancelRect)
        {
            return;
        }
        SetRectEmpty(cancelRect);

        RECT clientRect{};
        UINT dpi = GetDpiForWindow(infoWindow);
        if (!dpi || !GetClientRect(infoWindow, &clientRect) ||
            !ActiveElements().showCancel)
        {
            return;
        }

        LONG rightPadding = ScaleForDpi(kContentRightPadding, dpi);
        LONG controlSize = ScaleForDpi(32, dpi);
        cancelRect->right =
            std::max<LONG>(clientRect.right - rightPadding, 0L);
        cancelRect->left =
            std::max<LONG>(cancelRect->right - controlSize, 0L);
        cancelRect->top = ScaleForDpi(4, dpi);
        cancelRect->bottom = std::min<LONG>(
            cancelRect->top + controlSize, clientRect.bottom);
    }

    void GetInfoPanelPauseRect(HWND infoWindow, RECT *pauseRect)
    {
        if (!pauseRect)
        {
            return;
        }
        SetRectEmpty(pauseRect);

        RECT clientRect{};
        UINT dpi = GetDpiForWindow(infoWindow);
        if (!dpi || !GetClientRect(infoWindow, &clientRect))
        {
            return;
        }

        LONG rightPadding = ScaleForDpi(kContentRightPadding, dpi);
        LONG controlSize = ScaleForDpi(32, dpi);
        LONG gap = ScaleForDpi(4, dpi);

        RECT cancelRect{};
        GetInfoPanelCancelRect(infoWindow, &cancelRect);
        LONG rightEdge =
            cancelRect.right > cancelRect.left
                ? cancelRect.left - gap
                : std::max<LONG>(
                      clientRect.right - rightPadding, 0L);

        pauseRect->right = std::max<LONG>(rightEdge, 0L);
        pauseRect->left =
            std::max<LONG>(pauseRect->right - controlSize, 0L);
        pauseRect->top = ScaleForDpi(4, dpi);
        pauseRect->bottom = std::min<LONG>(
            pauseRect->top + controlSize, clientRect.bottom);
    }


    struct ChildWindowClassLookup
    {
        PCWSTR className;
        HWND window;
    };

    BOOL CALLBACK FindChildWindowByClass(HWND window, LPARAM parameter)
    {
        auto *lookup = reinterpret_cast<ChildWindowClassLookup *>(parameter);
        wchar_t className[64]{};
        if (GetClassNameW(window, className, ARRAYSIZE(className)) &&
            lstrcmpW(className, lookup->className) == 0)
        {
            lookup->window = window;
            return FALSE;
        }
        return TRUE;
    }

    HWND FindDescendantWindowByClass(HWND parent, PCWSTR className)
    {
        ChildWindowClassLookup lookup{className, nullptr};
        EnumChildWindows(parent, FindChildWindowByClass,
                         reinterpret_cast<LPARAM>(&lookup));
        return lookup.window;
    }

    bool InvokeNativeActionForTile(OperationTileElement *tile,
                                   PCWSTR elementName,
                                   PCWSTR actionName)
    {
        DirectUI::Element *operationTileRoot = nullptr;
        DirectUI::Element *tileHeaderRoot = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [tile](TransferSummaryState const &state)
                { return state.tile == tile; });
            if (it == g_transferSummaries.end())
            {
                return false;
            }
            operationTileRoot = it->operationTileRoot;
            tileHeaderRoot = it->tileHeaderRoot;
        }

        DirectUI::Element *actionElement = FindSkinElement(
            operationTileRoot, tileHeaderRoot, elementName, false);
        if (!actionElement)
        {
            Wh_Log(L"custom action=%s native element=%s not found tile=%p",
                   actionName, elementName, reinterpret_cast<void *>(tile));
            return false;
        }

        // Do not synthesize mouse input into OperationTileHost.
        // DirectUI::Button exposes DefaultAction(), which performs the
        // button's semantic default action directly.
        HMODULE dui70 = GetModuleHandleW(L"dui70.dll");
        if (!dui70)
        {
            Wh_Log(L"custom action=%s dui70.dll not loaded tile=%p",
                   actionName, reinterpret_cast<void *>(tile));
            return false;
        }

        using ButtonDefaultAction_t =
            long (*)(DirectUI::Element *);

        auto buttonDefaultAction =
            reinterpret_cast<ButtonDefaultAction_t>(
                GetProcAddress(
                    dui70,
                    "?DefaultAction@Button@DirectUI@@UEAAJXZ"));
        if (!buttonDefaultAction)
        {
            Wh_Log(L"custom action=%s DirectUI::Button::DefaultAction "
                   L"export not found tile=%p",
                   actionName, reinterpret_cast<void *>(tile));
            return false;
        }

        long result = buttonDefaultAction(actionElement);
        return result >= 0;
    }

    void SetTilePausedPresentationState(
        OperationTileElement *tile,
        bool paused)
    {
        HWND infoWindow = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (it == g_circles.end())
            {
                return;
            }

            it->paused = paused;
            it->pausedStateKnown = true;
            infoWindow = it->infoWindow;
        }

        if (infoWindow && IsWindow(infoWindow))
        {
            RedrawWindow(
                infoWindow, nullptr, nullptr,
                RDW_INVALIDATE | RDW_UPDATENOW);
        }
    }

    bool InvokeNativeActionFromInfoPanel(HWND infoWindow,
                                         PCWSTR elementName,
                                         PCWSTR actionName)
    {
        OperationTileElement *tile = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [infoWindow](CircleState const &state)
                { return state.infoWindow == infoWindow; });
            if (it == g_circles.end())
            {
                return false;
            }
            tile = it->tile;
        }

        bool isPauseResume =
            elementName &&
            lstrcmpW(elementName, L"eltPauseButton") == 0;
        bool wasPaused = false;
        bool pauseStateResolved = false;

        if (isPauseResume)
        {
            InfoPanelSnapshot beforeAction{};
            if (GetInfoPanelSnapshot(infoWindow, &beforeAction))
            {
                wasPaused = beforeAction.paused;
                pauseStateResolved = true;
            }
        }

        bool invoked =
            InvokeNativeActionForTile(tile, elementName, actionName);

        if (invoked && isPauseResume && pauseStateResolved)
        {
            // Explorer's top-level caption is aggregate in multi-op mode, so
            // it cannot tell us which tile changed. We already invoked the
            // correct tile's native button; mirror that successful toggle in
            // the presentation state immediately.
            SetTilePausedPresentationState(tile, !wasPaused);
        }

        return invoked;
    }

    LRESULT CALLBACK InfoPanelWindowProc(HWND window,
                                         UINT message,
                                         WPARAM wParam,
                                         LPARAM lParam)
    {
        if (message == g_removeHostSubclassMessage)
        {
            return DestroyWindow(window) ? TRUE : FALSE;
        }
        if (g_unloading.load(std::memory_order_acquire))
        {
            return DefWindowProcW(window, message, wParam, lParam);
        }

        switch (message)
        {
        case WM_PAINT:
            PaintInfoPanel(window);
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_NCHITTEST:
            // The normal-operation surface is intentionally opaque and owns
            // its hit area. This prevents invisible native buttons underneath
            // from being activated at machine-dependent coordinates.
            return HTCLIENT;
        case WM_LBUTTONUP:
        {
            POINT point{static_cast<short>(LOWORD(lParam)),
                        static_cast<short>(HIWORD(lParam))};
            RECT cancelRect{};
            GetInfoPanelCancelRect(window, &cancelRect);
            if (PtInRect(&cancelRect, point))
            {
                InvokeNativeActionFromInfoPanel(
                    window, L"eltCancelButton", L"cancel-top-x");
                return 0;
            }

            RECT pauseRect{};
            GetInfoPanelPauseRect(window, &pauseRect);
            if (PtInRect(&pauseRect, point))
            {
                InvokeNativeActionFromInfoPanel(
                    window, L"eltPauseButton", L"pause-resume");
                return 0;
            }
            return 0;
        }
        case WM_SETCURSOR:
        {
            POINT point{};
            if (GetCursorPos(&point))
            {
                ScreenToClient(window, &point);
                RECT cancelRect{};
                GetInfoPanelCancelRect(window, &cancelRect);
                RECT pauseRect{};
                GetInfoPanelPauseRect(window, &pauseRect);
                if (PtInRect(&cancelRect, point) ||
                    PtInRect(&pauseRect, point))
                {
                    SetCursor(LoadCursorW(nullptr, IDC_HAND));
                    return TRUE;
                }
            }
            break;
        }
        }
        return DefWindowProcW(window, message, wParam, lParam);
    }



    OperationTileElement *GetFooterOverlayTile(HWND footerWindow)
    {
        return reinterpret_cast<OperationTileElement *>(
            GetWindowLongPtrW(footerWindow, GWLP_USERDATA));
    }

    bool GetFooterExpandedState(OperationTileElement *tile,
                                bool *expanded)
    {
        if (!tile || !expanded)
        {
            return false;
        }

        std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
        auto it = std::find_if(
            g_transferSummaries.begin(), g_transferSummaries.end(),
            [tile](TransferSummaryState const &state)
            { return state.tile == tile; });
        if (it == g_transferSummaries.end() || !it->displayModeKnown)
        {
            return false;
        }

        *expanded = it->expanded;
        return true;
    }

    void GetFooterCancelRect(HWND footerWindow, RECT *cancelRect)
    {
        if (!cancelRect)
        {
            return;
        }
        SetRectEmpty(cancelRect);
        if (!ActiveElements().showCancel)
        {
            return;
        }

        RECT clientRect{};
        UINT dpi = GetDpiForWindow(footerWindow);
        if (!dpi || !GetClientRect(footerWindow, &clientRect))
        {
            return;
        }

        LONG width = ScaleForDpi(kInfoPanelCancelWidth, dpi);
        LONG height = ScaleForDpi(kInfoPanelCancelHeight, dpi);
        LONG rightPadding = ScaleForDpi(kContentRightPadding, dpi);
        cancelRect->right =
            std::max<LONG>(clientRect.right - rightPadding, 0L);
        cancelRect->left =
            std::max<LONG>(cancelRect->right - width, 0L);
        cancelRect->top = std::max<LONG>(
            (clientRect.bottom - clientRect.top - height) / 2, 0L);
        cancelRect->bottom = std::min<LONG>(
            cancelRect->top + height, clientRect.bottom);
    }

    void DrawFooterOverlayFrame(HWND footerWindow,
                                HDC deviceContext,
                                RECT const &clientRect)
    {
        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;
        if (width <= 0 || height <= 0)
        {
            return;
        }

        UINT dpi = GetDpiForWindow(footerWindow);
        if (!dpi)
        {
            dpi = USER_DEFAULT_SCREEN_DPI;
        }

        ThemePalette theme = GetDrawingTheme(footerWindow);
        TypographyConfig const &type = ActiveTypography();

        bool expanded = false;
        GetFooterExpandedState(GetFooterOverlayTile(footerWindow),
                               &expanded);

        Gdiplus::Graphics graphics(deviceContext);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics.SetTextRenderingHint(
            Gdiplus::TextRenderingHintClearTypeGridFit);

        Gdiplus::SolidBrush backgroundBrush(Gdiplus::Color(
            255, GetRValue(theme.background), GetGValue(theme.background),
            GetBValue(theme.background)));
        graphics.FillRectangle(&backgroundBrush, 0, 0, width, height);

        Gdiplus::Font footerFont(
            type.bodyFont.c_str(),
            static_cast<Gdiplus::REAL>(ScaleForDpi(type.footerSize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font footerFallback(
            L"Segoe UI",
            static_cast<Gdiplus::REAL>(ScaleForDpi(type.footerSize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font *selectedFont =
            footerFont.GetLastStatus() == Gdiplus::Ok
                ? &footerFont
                : &footerFallback;

        Gdiplus::SolidBrush secondaryBrush(Gdiplus::Color(
            255, GetRValue(theme.secondaryText),
            GetGValue(theme.secondaryText),
            GetBValue(theme.secondaryText)));
        Gdiplus::SolidBrush actionTextBrush(Gdiplus::Color(
            255, GetRValue(theme.actionText),
            GetGValue(theme.actionText), GetBValue(theme.actionText)));

        Gdiplus::REAL centerY =
            static_cast<Gdiplus::REAL>(height) / 2.0f;
        Gdiplus::REAL chevronX =
            static_cast<Gdiplus::REAL>(ScaleForDpi(40, dpi));
        Gdiplus::REAL half =
            static_cast<Gdiplus::REAL>(ScaleForDpi(4, dpi));
        Gdiplus::REAL rise =
            static_cast<Gdiplus::REAL>(ScaleForDpi(3, dpi));
        Gdiplus::Pen chevronPen(Gdiplus::Color(
            255, GetRValue(theme.secondaryText),
            GetGValue(theme.secondaryText),
            GetBValue(theme.secondaryText)),
            1.0f);

        if (expanded)
        {
            graphics.DrawLine(&chevronPen,
                              chevronX - half, centerY + rise,
                              chevronX, centerY - rise);
            graphics.DrawLine(&chevronPen,
                              chevronX, centerY - rise,
                              chevronX + half, centerY + rise);
        }
        else
        {
            graphics.DrawLine(&chevronPen,
                              chevronX - half, centerY - rise,
                              chevronX, centerY + rise);
            graphics.DrawLine(&chevronPen,
                              chevronX, centerY + rise,
                              chevronX + half, centerY - rise);
        }

        graphics.DrawString(
            expanded ? L"Fewer details" : L"More details",
            -1, selectedFont,
            Gdiplus::PointF(
                static_cast<Gdiplus::REAL>(ScaleForDpi(55, dpi)),
                centerY -
                    static_cast<Gdiplus::REAL>(ScaleForDpi(8, dpi))),
            &secondaryBrush);

        RECT cancelRect{};
        GetFooterCancelRect(footerWindow, &cancelRect);
        if (cancelRect.right > cancelRect.left &&
            cancelRect.bottom > cancelRect.top)
        {
            Gdiplus::RectF buttonBounds(
                static_cast<Gdiplus::REAL>(cancelRect.left),
                static_cast<Gdiplus::REAL>(cancelRect.top),
                static_cast<Gdiplus::REAL>(
                    cancelRect.right - cancelRect.left),
                static_cast<Gdiplus::REAL>(
                    cancelRect.bottom - cancelRect.top));
            Gdiplus::SolidBrush buttonBrush(Gdiplus::Color(
                255, GetRValue(theme.actionSurface),
                GetGValue(theme.actionSurface),
                GetBValue(theme.actionSurface)));
            Gdiplus::Pen buttonBorder(Gdiplus::Color(
                255, GetRValue(theme.actionBorder),
                GetGValue(theme.actionBorder),
                GetBValue(theme.actionBorder)), 1.0f);
            graphics.FillRectangle(&buttonBrush, buttonBounds);
            graphics.DrawRectangle(&buttonBorder, buttonBounds);
            Gdiplus::StringFormat centered;
            centered.SetAlignment(Gdiplus::StringAlignmentCenter);
            centered.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            graphics.DrawString(L"Cancel", -1, selectedFont, buttonBounds,
                                &centered, &actionTextBrush);
        }
    }

    bool InvokeNativeDisplayModeFromFooter(HWND footerWindow)
    {
        OperationTileElement *tile = GetFooterOverlayTile(footerWindow);
        if (!tile)
        {
            return false;
        }

        TransferSummaryState state{};
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [tile](TransferSummaryState const &candidate)
                { return candidate.tile == tile; });
            if (it == g_transferSummaries.end())
            {
                return false;
            }
            state = *it;
        }

        DirectUI::Element *displayModeButton =
            FindSkinElementWithAncestorFallback(
                state.operationTileRoot, state.tileHeaderRoot,
                state.operationTileRoot, L"eltDisplayModeBtn", true);
        if (!displayModeButton)
        {
            return false;
        }

        HMODULE dui70 = GetModuleHandleW(L"dui70.dll");
        if (!dui70)
        {
            return false;
        }

        using ButtonDefaultAction_t = long (*)(DirectUI::Element *);
        auto buttonDefaultAction =
            reinterpret_cast<ButtonDefaultAction_t>(
                GetProcAddress(
                    dui70,
                    "?DefaultAction@Button@DirectUI@@UEAAJXZ"));
        if (!buttonDefaultAction)
        {
            return false;
        }

        long result = buttonDefaultAction(displayModeButton);
        return result >= 0;
    }

    LRESULT CALLBACK FooterOverlayWindowProc(HWND window,
                                              UINT message,
                                              WPARAM wParam,
                                              LPARAM lParam)
    {
        if (message == g_removeHostSubclassMessage)
        {
            return DestroyWindow(window) ? TRUE : FALSE;
        }
        if (g_unloading.load(std::memory_order_acquire))
        {
            return DefWindowProcW(window, message, wParam, lParam);
        }

        switch (message)
        {
        case WM_NCCREATE:
        {
            auto *create =
                reinterpret_cast<CREATESTRUCTW *>(lParam);
            SetWindowLongPtrW(
                window, GWLP_USERDATA,
                reinterpret_cast<LONG_PTR>(
                    create->lpCreateParams));
            return TRUE;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT paint{};
            HDC paintDc = BeginPaint(window, &paint);
            if (!paintDc)
            {
                return 0;
            }

            RECT client{};
            if (!GetClientRect(window, &client))
            {
                EndPaint(window, &paint);
                return 0;
            }

            int width = client.right - client.left;
            int height = client.bottom - client.top;
            HDC memoryDc =
                width > 0 && height > 0
                    ? CreateCompatibleDC(paintDc)
                    : nullptr;
            HBITMAP backBuffer =
                memoryDc
                    ? CreateCompatibleBitmap(paintDc, width, height)
                    : nullptr;
            HGDIOBJ oldBitmap =
                backBuffer
                    ? SelectObject(memoryDc, backBuffer)
                    : nullptr;

            if (memoryDc && backBuffer && oldBitmap &&
                oldBitmap != HGDI_ERROR)
            {
                DrawFooterOverlayFrame(window, memoryDc, client);
                BitBlt(paintDc, 0, 0, width, height,
                       memoryDc, 0, 0, SRCCOPY);
                SelectObject(memoryDc, oldBitmap);
            }
            else
            {
                DrawFooterOverlayFrame(window, paintDc, client);
            }

            if (backBuffer)
            {
                DeleteObject(backBuffer);
            }
            if (memoryDc)
            {
                DeleteDC(memoryDc);
            }

            EndPaint(window, &paint);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_NCHITTEST:
            return HTCLIENT;
        case WM_LBUTTONUP:
        {
            POINT point{static_cast<short>(LOWORD(lParam)),
                        static_cast<short>(HIWORD(lParam))};
            RECT cancelRect{};
            GetFooterCancelRect(window, &cancelRect);
            if (PtInRect(&cancelRect, point))
            {
                InvokeNativeActionForTile(
                    GetFooterOverlayTile(window),
                    L"eltCancelButton", L"cancel");
            }
            else
            {
                InvokeNativeDisplayModeFromFooter(window);
            }
            return 0;
        }
        case WM_SETCURSOR:
            SetCursor(LoadCursorW(nullptr, IDC_HAND));
            return TRUE;
        }
        return DefWindowProcW(window, message, wParam, lParam);
    }

    HWND GetFooterOverlayWindow(HWND hostWindow)
    {
        return hostWindow && IsWindow(hostWindow)
                   ? FindWindowExW(hostWindow, nullptr,
                                   kFooterOverlayWindowClass,
                                   nullptr)
                   : nullptr;
    }

    void PositionFooterOverlay(OperationTileElement *tile)
    {
        if (!tile || !g_footerOverlayClassAtom)
        {
            return;
        }

        HWND hostWindow = GetRegisteredCircleHost(tile);
        if (!hostWindow || !IsWindow(hostWindow))
        {
            return;
        }

        RECT clientRect{};
        UINT dpi = GetDpiForWindow(hostWindow);
        if (!dpi || !GetClientRect(hostWindow, &clientRect))
        {
            return;
        }

        int overlayHeight =
            ScaleForDpi(kFooterOverlayHeight, dpi);
        int overlayTop =
            std::max(static_cast<int>(clientRect.bottom) -
                         overlayHeight - 1,
                     0);
        overlayHeight =
            std::max(static_cast<int>(clientRect.bottom) - overlayTop, 1);
        int overlayWidth =
            std::max(static_cast<int>(clientRect.right), 1);

        HWND footerWindow = GetFooterOverlayWindow(hostWindow);
        if (!footerWindow)
        {
            footerWindow = CreateWindowExW(
                WS_EX_NOACTIVATE | WS_EX_NOPARENTNOTIFY,
                kFooterOverlayWindowClass, nullptr,
                WS_CHILD | WS_CLIPSIBLINGS,
                0, overlayTop, overlayWidth, overlayHeight,
                hostWindow, nullptr, g_circleClassInstance, tile);
        }
        if (!footerWindow)
        {
            return;
        }

        SetWindowLongPtrW(
            footerWindow, GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(tile));

        SetWindowPos(
            footerWindow, HWND_TOP,
            0, overlayTop, overlayWidth, overlayHeight,
            SWP_NOACTIVATE | SWP_NOOWNERZORDER |
                SWP_SHOWWINDOW);
        InvalidateRect(footerWindow, nullptr, FALSE);
    }

    void ForgetCircleWindow(HWND circleWindow);

    LRESULT CALLBACK ProgressCircleWindowProc(HWND window,
                                              UINT message,
                                              WPARAM wParam,
                                              LPARAM lParam)
    {
        if (message == g_removeHostSubclassMessage)
        {
            return DestroyWindow(window) ? TRUE : FALSE;
        }
        if (g_unloading.load(std::memory_order_acquire) &&
            message != WM_NCDESTROY)
        {
            return DefWindowProcW(window, message, wParam, lParam);
        }

        switch (message)
        {
        case WM_PAINT:
            PaintProgressCircle(window);
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_NCDESTROY:
            ForgetCircleWindow(window);
            break;
        }

        return DefWindowProcW(window, message, wParam, lParam);
    }

    void RemoveHostSubclassRecord(HWND hostWindow)
    {
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find(g_subclassedHosts.begin(),
                                g_subclassedHosts.end(), hostWindow);
            if (it != g_subclassedHosts.end())
            {
                g_subclassedHosts.erase(it);
            }
            auto requestIt = std::find_if(
                g_hostPositionRequests.begin(), g_hostPositionRequests.end(),
                [hostWindow](HostPositionRequest const &request)
                {
                    return request.hostWindow == hostWindow;
                });
            if (requestIt != g_hostPositionRequests.end())
            {
                g_hostPositionRequests.erase(requestIt);
            }
        }
        ForgetHostNativeGeometry(hostWindow);
    }

    bool RemoveDestroyedHostRecords(HWND hostWindow)
    {
        std::lock_guard<std::mutex> lock(g_circleMutex);
        for (CircleState const &state : g_circles)
        {
            if (state.hostWindow == hostWindow &&
                ((state.circleWindow && IsWindow(state.circleWindow)) ||
                 (state.infoWindow && IsWindow(state.infoWindow)) ||
                 (state.progressWindow && IsWindow(state.progressWindow))))
            {
                return false;
            }
        }

        g_circles.erase(
            std::remove_if(
                g_circles.begin(), g_circles.end(),
                [hostWindow](CircleState const &state)
                { return state.hostWindow == hostWindow; }),
            g_circles.end());
        g_subclassedHosts.erase(
            std::remove(g_subclassedHosts.begin(),
                        g_subclassedHosts.end(), hostWindow),
            g_subclassedHosts.end());
        g_hostPositionRequests.erase(
            std::remove_if(
                g_hostPositionRequests.begin(),
                g_hostPositionRequests.end(),
                [hostWindow](HostPositionRequest const &request)
                { return request.hostWindow == hostWindow; }),
            g_hostPositionRequests.end());
        ForgetHostNativeGeometry(hostWindow);
        return true;
    }

    PCWSTR TakeProgressCirclePositionReason(HWND hostWindow)
    {
        std::lock_guard<std::mutex> lock(g_circleMutex);
        auto it = std::find_if(
            g_hostPositionRequests.begin(), g_hostPositionRequests.end(),
            [hostWindow](HostPositionRequest const &request)
            {
                return request.hostWindow == hostWindow;
            });
        if (it == g_hostPositionRequests.end())
        {
            return nullptr;
        }
        PCWSTR reason = it->reason;
        g_hostPositionRequests.erase(it);
        return reason;
    }

    bool RemoveProgressWindowSubclassForTeardown(HWND progressWindow)
    {
        if (!progressWindow || !IsWindow(progressWindow))
        {
            return true;
        }

        if (GetWindowThreadProcessId(progressWindow, nullptr) !=
            GetCurrentThreadId())
        {
            DWORD_PTR result = FALSE;
            if (SendMessageTimeoutW(
                    progressWindow, g_removeHostSubclassMessage, 0, 0,
                    SMTO_ABORTIFHUNG | SMTO_BLOCK, 1000, &result) &&
                result == TRUE)
            {
                return true;
            }

            Wh_Log(L"Presentation teardown owning-thread progress subclass "
                   L"removal was not acknowledged hwnd=%p",
                   reinterpret_cast<void *>(progressWindow));
            return false;
        }

        DWORD_PTR referenceData = 0;
        if (!GetWindowSubclass(progressWindow,
                               NativeProgressWindowSubclassProc,
                               kProgressWindowSubclassId, &referenceData))
        {
            return true;
        }

        if (!RemoveWindowSubclass(progressWindow,
                                  NativeProgressWindowSubclassProc,
                                  kProgressWindowSubclassId) &&
            GetWindowSubclass(progressWindow,
                              NativeProgressWindowSubclassProc,
                              kProgressWindowSubclassId, &referenceData))
        {
            Wh_Log(L"Presentation teardown failed to remove progress "
                   L"subclass hwnd=%p error=%lu",
                   reinterpret_cast<void *>(progressWindow),
                   GetLastError());
            return false;
        }

        return true;
    }

    bool DestroyWindowOnOwningThread(HWND window, PCWSTR role)
    {
        if (!window || !IsWindow(window))
        {
            return true;
        }

        if (GetWindowThreadProcessId(window, nullptr) !=
            GetCurrentThreadId())
        {
            DWORD_PTR result = FALSE;
            if (SendMessageTimeoutW(
                    window, g_removeHostSubclassMessage, 0, 0,
                    SMTO_ABORTIFHUNG | SMTO_BLOCK, 1000, &result) &&
                result == TRUE &&
                !IsWindow(window))
            {
                return true;
            }

            Wh_Log(L"Presentation teardown owning-thread destruction was "
                   L"not acknowledged role=%s hwnd=%p",
                   role, reinterpret_cast<void *>(window));
            return false;
        }

        if (!DestroyWindow(window) && IsWindow(window))
        {
            Wh_Log(L"Presentation teardown DestroyWindow failed role=%s "
                   L"hwnd=%p error=%lu",
                   role, reinterpret_cast<void *>(window), GetLastError());
            return false;
        }

        return true;
    }

    bool CleanupCircleStateResources(CircleState *state,
                                     bool destroyCircleAnchor)
    {
        if (!state)
        {
            return true;
        }

        if (!RemoveProgressWindowSubclassForTeardown(
                state->progressWindow))
        {
            return false;
        }
        state->progressWindow = nullptr;

        if (!DestroyWindowOnOwningThread(state->infoWindow,
                                         L"information-panel"))
        {
            return false;
        }
        state->infoWindow = nullptr;

        if (destroyCircleAnchor &&
            !DestroyWindowOnOwningThread(state->circleWindow,
                                         L"circle-anchor"))
        {
            return false;
        }

        return true;
    }

    void EraseCircleStateRecord(CircleState const &state)
    {
        std::lock_guard<std::mutex> lock(g_circleMutex);
        g_circles.erase(
            std::remove_if(
                g_circles.begin(), g_circles.end(),
                [&state](CircleState const &candidate)
                {
                    return candidate.circleWindow == state.circleWindow;
                }),
            g_circles.end());
    }

    void RetainCleanupOnlyCircleState(CircleState state,
                                      PCWSTR reason)
    {
        state.tile = nullptr;
        state.positionValid = false;
        bool shouldLog = false;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [&state](CircleState const &candidate)
                {
                    return candidate.circleWindow == state.circleWindow;
                });
            if (it == g_circles.end())
            {
                g_circles.push_back(state);
                shouldLog = true;
            }
            else
            {
                shouldLog = it->tile != nullptr;
                *it = state;
            }
        }

        if (shouldLog)
        {
            Wh_Log(L"Presentation cleanup retained for teardown reason=%s "
                   L"host=%p circle=%p info=%p progress=%p",
                   reason, reinterpret_cast<void *>(state.hostWindow),
                   reinterpret_cast<void *>(state.circleWindow),
                   reinterpret_cast<void *>(state.infoWindow),
                   reinterpret_cast<void *>(state.progressWindow));
        }
    }

    void ForgetCircleWindow(HWND circleWindow)
    {
        CircleState state{};
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [circleWindow](CircleState const &candidate)
                {
                    return candidate.circleWindow == circleWindow;
                });
            if (it == g_circles.end())
            {
                return;
            }
            state = *it;
        }

        if (!CleanupCircleStateResources(&state, false))
        {
            RetainCleanupOnlyCircleState(
                state, L"circle-anchor-destruction");
            return;
        }

        EraseCircleStateRecord(state);
    }

    bool DestroyProgressCirclesForHost(HWND hostWindow)
    {
        std::vector<CircleState> states;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            for (CircleState const &state : g_circles)
            {
                if (state.hostWindow == hostWindow)
                {
                    states.push_back(state);
                }
            }
        }

        // Every call is made by the owning host UI thread. Remove callbacks
        // into this module before destroying their custom lifetime anchors.
        for (CircleState const &state : states)
        {
            if (!RemoveProgressWindowSubclassForTeardown(
                    state.progressWindow))
            {
                return false;
            }
        }

        for (CircleState const &state : states)
        {
            if (!DestroyWindowOnOwningThread(state.infoWindow,
                                             L"information-panel"))
            {
                return false;
            }
            if (state.circleWindow && IsWindow(state.circleWindow))
            {
                if (!DestroyWindowOnOwningThread(state.circleWindow,
                                                 L"circle-anchor"))
                {
                    return false;
                }
            }
            else
            {
                ForgetCircleWindow(state.circleWindow);
            }
        }

        HWND footerWindow = GetFooterOverlayWindow(hostWindow);
        if (!DestroyWindowOnOwningThread(footerWindow, L"footer-overlay"))
        {
            return false;
        }

        return true;
    }

    void CancelDeferredDisplaySnapshotsForHost(HWND hostWindow)
    {
        std::lock_guard<std::mutex> lock(g_displayDiagnosticMutex);
        g_deferredDisplaySnapshots.erase(
            std::remove_if(
                g_deferredDisplaySnapshots.begin(),
                g_deferredDisplaySnapshots.end(),
                [hostWindow](DeferredDisplaySnapshot const &snapshot)
                { return snapshot.hostWindow == hostWindow; }),
            g_deferredDisplaySnapshots.end());
    }

    void CancelDeferredDisplaySnapshotsForTile(OperationTileElement *tile)
    {
        std::lock_guard<std::mutex> lock(g_displayDiagnosticMutex);
        g_deferredDisplaySnapshots.erase(
            std::remove_if(
                g_deferredDisplaySnapshots.begin(),
                g_deferredDisplaySnapshots.end(),
                [tile](DeferredDisplaySnapshot const &snapshot)
                { return snapshot.tile == tile; }),
            g_deferredDisplaySnapshots.end());
    }

    int GetSingleCirclePercentForHost(HWND hostWindow)
    {
        std::lock_guard<std::mutex> lock(g_circleMutex);
        int percent = -1;
        int matches = 0;
        for (auto const &state : g_circles)
        {
            if (state.hostWindow != hostWindow)
            {
                continue;
            }
            percent = std::clamp(state.progressPercent, 0, 100);
            ++matches;
            if (matches > 1)
            {
                return -1;
            }
        }
        return matches == 1 ? percent : -1;
    }

    bool LooksLikeNativeProgressCaption(PCWSTR text)
    {
        return text && *text && wcsstr(text, L"%") != nullptr &&
               wcsstr(text, L" / ") == nullptr;
    }

    bool LooksLikeTransferSummaryCaption(PCWSTR text)
    {
        if (!text || !*text)
        {
            return false;
        }

        // The custom body summary is a transferred/total pair. Explorer can
        // mirror either the full "x / y (p%)" form or a transient "x / y"
        // form into the top-level caption. Suppress both so the title remains
        // Explorer's native progress caption instead of flashing byte counts.
        return wcsstr(text, L" / ") != nullptr;
    }

    bool IsKnownSpecialOperationCaption(PCWSTR caption)
    {
        if (!caption || !*caption)
        {
            return false;
        }

        // These three were observed directly while testing this build.
        // The generic transition rule below does not depend on these English
        // strings once the host has shown a normal percentage caption.
        return lstrcmpW(caption, L"Replace or Skip Files") == 0 ||
               lstrcmpW(caption, L"File In Use") == 0 ||
               lstrcmpW(caption, L"Folder In Use") == 0 ||
               lstrcmpW(caption, L"Item Not Found") == 0;
    }

    bool IsHostInSpecialOperationState(HWND hostWindow)
    {
        if (!hostWindow)
        {
            return false;
        }

        std::lock_guard<std::mutex> lock(g_hostPresentationMutex);
        auto it = std::find_if(
            g_hostPresentationStates.begin(),
            g_hostPresentationStates.end(),
            [hostWindow](HostPresentationState const &state)
            { return state.hostWindow == hostWindow; });
        return it != g_hostPresentationStates.end() &&
               it->specialOperationState;
    }

    void ForgetHostPresentationState(HWND hostWindow)
    {
        std::lock_guard<std::mutex> lock(g_hostPresentationMutex);
        g_hostPresentationStates.erase(
            std::remove_if(
                g_hostPresentationStates.begin(),
                g_hostPresentationStates.end(),
                [hostWindow](HostPresentationState const &state)
                { return state.hostWindow == hostWindow; }),
            g_hostPresentationStates.end());
    }

    void SetNativeDuplicatePresentation(
        TransferSummaryState const &state,
        bool customNormalMode)
    {
        if (!Element_SetVisible_Original || !state.operationTileRoot)
        {
            return;
        }

        auto setVisible = [](DirectUI::Element *element, bool visible)
        {
            if (element)
            {
                Element_SetVisible_Original(element, visible);
            }
        };
        auto find = [&state](PCWSTR name, bool headerFallback = false)
        {
            return FindSkinElement(
                state.operationTileRoot, state.tileHeaderRoot,
                name, headerFallback);
        };

        if (customNormalMode)
        {
            setVisible(state.tileHeaderRoot, false);
            setVisible(find(L"eltSummary"), false);
            setVisible(find(L"eltDetails"), false);
            setVisible(find(L"eltChartArea"), false);
            setVisible(find(L"eltRateChart_New"), false);
            setVisible(find(L"eltProgressBarContainer"), false);
            setVisible(find(L"eltProgressBar"), false);
            return;
        }

        // Restore Explorer's own normal-mode visibility model before revealing
        // it for a special state or removing the mod.
        bool expanded = state.displayModeKnown && state.expanded;
        setVisible(state.tileHeaderRoot, true);
        setVisible(find(L"eltSummary"), true);
        setVisible(find(L"eltDetails"), expanded);
        setVisible(find(L"eltChartArea"), expanded);
        setVisible(find(L"eltRateChart_New"), expanded);
        setVisible(find(L"eltProgressBarContainer"), !expanded);
        setVisible(find(L"eltProgressBar"), !expanded);
    }

    void RestoreNativePresentationForHost(HWND hostWindow)
    {
        std::vector<TransferSummaryState> states;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            states = g_transferSummaries;
        }
        for (TransferSummaryState const &state : states)
        {
            if (state.tile && GetRegisteredCircleHost(state.tile) == hostWindow)
            {
                SetNativeDuplicatePresentation(state, false);
            }
        }
    }

    void HideCustomPresentationForHost(HWND hostWindow)
    {
        RestoreNativePresentationForHost(hostWindow);
        if (ShouldApplyNativeColorOverrides())
        {
            ResetUnifiedHostChrome(hostWindow);
            HWND operationTileHost =
                FindDescendantWindowByClass(
                    hostWindow, L"OperationTileHost");
            if (operationTileHost)
            {
                InvalidateRect(operationTileHost, nullptr, TRUE);
            }
        }

        std::vector<HWND> windows;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            for (CircleState &state : g_circles)
            {
                if (state.hostWindow != hostWindow)
                {
                    continue;
                }

                state.positionValid = false;
                if (state.circleWindow)
                {
                    windows.push_back(state.circleWindow);
                }
                if (state.infoWindow)
                {
                    windows.push_back(state.infoWindow);
                }
            }
        }

        for (HWND child : windows)
        {
            if (child && IsWindow(child) && IsWindowVisible(child))
            {
                ShowWindow(child, SW_HIDE);
            }
        }

        HWND footerWindow = GetFooterOverlayWindow(hostWindow);
        if (footerWindow && IsWindowVisible(footerWindow))
        {
            ShowWindow(footerWindow, SW_HIDE);
        }
    }

    void ScheduleCustomReapplyForHost(HWND hostWindow)
    {
        if (ShouldApplyNativeColorOverrides())
        {
            ApplyUnifiedHostChrome(hostWindow);
        }

        std::vector<OperationTileElement *> hostTiles;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            for (CircleState const &circle : g_circles)
            {
                if (circle.hostWindow == hostWindow && circle.tile)
                {
                    hostTiles.push_back(circle.tile);
                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            for (TransferSummaryState &state : g_transferSummaries)
            {
                if (std::find(hostTiles.begin(), hostTiles.end(), state.tile) ==
                    hostTiles.end())
                {
                    continue;
                }

                // After Replace/Skip and similar native interruption UIs,
                // Explorer can resume item progress while its byte-rate/
                // completed-byte counters temporarily remain at zero. Keep
                // using Explorer's actual operation engine, but switch our
                // presentation to the per-owner measured fallback until this
                // tile is destroyed.
                state.resumedFromSpecialState = true;
                state.preferMeasuredRate = true;
                state.nativeDisplayRateValid = false;
                state.measuredDisplayRate = 0.0;
                state.measuredDisplayRateValid = false;
                state.measuredSampleInitialized = false;
                state.nativeRateHistory.clear();
            }
        }

        std::vector<TransferSummaryState> states;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            states = g_transferSummaries;
        }

        for (TransferSummaryState const &state : states)
        {
            if (!state.owner || !state.tile || !state.displayModeKnown)
            {
                continue;
            }

            HWND registeredHost = GetRegisteredCircleHost(state.tile);
            if (registeredHost != hostWindow)
            {
                continue;
            }

            unsigned long long transitionId =
                ++g_displayTransitionSequence;
            ScheduleDeferredDisplaySnapshot(
                state.owner, transitionId, state.expanded);
        }

        ScheduleProgressCirclePosition(hostWindow, L"resume-normal");
    }

    bool HostHasDeleteLikeOperation(HWND hostWindow)
    {
        if (!hostWindow)
        {
            return false;
        }

        std::vector<TransferSummaryState> states;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            states = g_transferSummaries;
        }

        for (TransferSummaryState const &state : states)
        {
            if (!state.tile || !state.deleteLikeKnown || !state.deleteLike)
            {
                continue;
            }
            if (GetRegisteredCircleHost(state.tile) == hostWindow)
            {
                return true;
            }
        }
        return false;
    }

    void UpdateHostPresentationStateFromCaption(
        HWND hostWindow,
        PCWSTR caption,
        bool *enteredSpecial,
        bool *leftSpecial)
    {
        if (enteredSpecial)
        {
            *enteredSpecial = false;
        }
        if (leftSpecial)
        {
            *leftSpecial = false;
        }

        if (!hostWindow || !caption || !*caption)
        {
            return;
        }

        bool normalProgress = LooksLikeNativeProgressCaption(caption);
        bool transferSummary = LooksLikeTransferSummaryCaption(caption);
        bool knownSpecial = IsKnownSpecialOperationCaption(caption);

        // The host caption is not a per-operation error signal once Explorer
        // aggregates multiple live tiles (for example "2 Actions" /
        // "2 Paused Actions"). Delete can also republish its ordinary
        // operation description after a percentage caption. Resolve these
        // facts before taking g_hostPresentationMutex so the presentation
        // lock is never nested with the tile/transfer registries.
        bool multiTileHost = GetRegisteredTileCountForHost(hostWindow) > 1;
        bool deleteLikeHost = HostHasDeleteLikeOperation(hostWindow);

        std::lock_guard<std::mutex> lock(g_hostPresentationMutex);

        auto it = std::find_if(
            g_hostPresentationStates.begin(),
            g_hostPresentationStates.end(),
            [hostWindow](HostPresentationState const &state)
            { return state.hostWindow == hostWindow; });

        if (it == g_hostPresentationStates.end())
        {
            g_hostPresentationStates.push_back(
                {hostWindow, normalProgress, knownSpecial});
            if (enteredSpecial && knownSpecial)
            {
                *enteredSpecial = true;
            }
            return;
        }

        bool wasSpecial = it->specialOperationState;

        if (normalProgress)
        {
            it->sawNormalProgressCaption = true;
            it->specialOperationState = false;
        }
        else if (knownSpecial)
        {
            // Explicitly recognized conflict/error captions always win, even
            // for delete and multi-operation hosts.
            it->specialOperationState = true;
        }
        else if (transferSummary && wasSpecial)
        {
            // Explorer commonly resumes from Replace/Skip (and similar native
            // interruption UIs) by first publishing a transient byte-summary
            // caption such as "0 bytes / 80.5 KB", before it ever publishes a
            // "% complete" caption again. 0.10.90 left the host permanently
            // marked special at that point, so our circle/info panel stayed
            // hidden and Explorer's native graph/details remained visible.
            //
            // A transferred/total caption is part of the normal progress-tile
            // presentation, so after an already-confirmed special state it is
            // also a valid "resume normal" signal.
            it->sawNormalProgressCaption = true;
            it->specialOperationState = false;
        }
        else if (multiTileHost || deleteLikeHost)
        {
            // Aggregate multi-operation captions and normal delete captions
            // are not special states. This also repairs a false special-state
            // classification that may have happened just before the second
            // tile or delete-kind information became available.
            it->specialOperationState = false;
        }
        else if (!transferSummary && it->sawNormalProgressCaption)
        {
            // Keep the proven 0.10.81 generic fallback for ordinary
            // single copy/move operations: once a percentage caption was seen,
            // a later unrelated non-percentage caption is treated as Explorer
            // switching to a native interruption/conflict presentation.
            it->specialOperationState = true;
        }

        if (enteredSpecial && !wasSpecial && it->specialOperationState)
        {
            *enteredSpecial = true;
        }
        if (leftSpecial && wasSpecial && !it->specialOperationState)
        {
            *leftSpecial = true;
        }
    }

    void SyncHostCaptionFromCircle(OperationTileElement *tile, int percent)
    {
        HWND hostWindow = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (it != g_circles.end())
            {
                hostWindow = it->hostWindow;
            }
        }

        if (!hostWindow || !IsWindow(hostWindow) ||
            GetSingleCirclePercentForHost(hostWindow) < 0 ||
            IsHostInSpecialOperationState(hostWindow))
        {
            return;
        }

        wchar_t currentCaption[160]{};
        GetWindowTextW(hostWindow, currentCaption, ARRAYSIZE(currentCaption));

        wchar_t synchronizedCaption[80]{};
        bool paused = wcsstr(currentCaption, L"Paused") != nullptr;
        wsprintfW(synchronizedCaption,
                  paused ? L"Paused - %d%% complete" : L"%d%% complete",
                  std::clamp(percent, 0, 100));
        SetWindowTextW(hostWindow, synchronizedCaption);
    }

    LRESULT CALLBACK OperationStatusWindowSubclassProc(
        HWND window,
        UINT message,
        WPARAM wParam,
        LPARAM lParam,
        UINT_PTR subclassId,
        DWORD_PTR)
    {
        if (message == WM_WINDOWPOSCHANGING && lParam)
        {
            CaptureHostNativeGeometry(
                window, *reinterpret_cast<WINDOWPOS *>(lParam));
        }

        if (g_unloading.load(std::memory_order_acquire) &&
            message != g_removeHostSubclassMessage &&
            message != WM_NCDESTROY)
        {
            return DefSubclassProc(window, message, wParam, lParam);
        }

        if (message == WM_SETTEXT)
        {
            PCWSTR caption = reinterpret_cast<PCWSTR>(lParam);

            bool enteredSpecial = false;
            bool leftSpecial = false;
            UpdateHostPresentationStateFromCaption(
                window, caption, &enteredSpecial, &leftSpecial);

            if (enteredSpecial)
            {
                HideCustomPresentationForHost(window);
            }
            else if (leftSpecial)
            {
                ScheduleCustomReapplyForHost(window);
            }

            // Never rewrite Explorer's error/conflict caption.
            if (IsHostInSpecialOperationState(window))
            {
                return DefSubclassProc(window, message, wParam, lParam);
            }

            if (LooksLikeTransferSummaryCaption(caption))
            {
                // Explorer can publish a transient byte-summary caption before
                // the progress caption. Replace it with the same normalized
                // percentage used by the circle instead of merely suppressing
                // the update and leaving an older byte caption frozen.
                int percent = GetSingleCirclePercentForHost(window);
                if (percent >= 0)
                {
                    wchar_t synchronizedCaption[80]{};
                    bool paused = wcsstr(caption, L"Paused") != nullptr;
                    wsprintfW(synchronizedCaption,
                              paused ? L"Paused - %d%% complete"
                                     : L"%d%% complete",
                              percent);
                    return DefSubclassProc(
                        window, message, wParam,
                        reinterpret_cast<LPARAM>(synchronizedCaption));
                }
                return TRUE;
            }

            // With one operation tile, keep the title percentage synchronized
            // to the same normalized progress used by the circle/body. Explorer
            // can otherwise publish a visibly different percentage. For
            // multi-tile hosts, leave Explorer's aggregate caption untouched.
            int percent = GetSingleCirclePercentForHost(window);
            if (percent >= 0 && LooksLikeNativeProgressCaption(caption))
            {
                wchar_t synchronizedCaption[80]{};
                bool paused = wcsstr(caption, L"Paused") != nullptr;
                wsprintfW(synchronizedCaption,
                          paused ? L"Paused - %d%% complete"
                                 : L"%d%% complete",
                          percent);
                return DefSubclassProc(window, message, wParam,
                                       reinterpret_cast<LPARAM>(synchronizedCaption));
            }
        }

        if (message == g_removeHostSubclassMessage)
        {
            RestoreNativePresentationForHost(window);
            if (ShouldApplyNativeColorOverrides())
            {
                ResetUnifiedHostChrome(window);
            }
            RestoreHostNativeGeometry(window);

            if (!DestroyProgressCirclesForHost(window))
            {
                return FALSE;
            }

            CancelDeferredDisplaySnapshotsForHost(window);
            ForgetHostPresentationState(window);

            DWORD_PTR referenceData = 0;
            if (!RemoveWindowSubclass(
                    window, OperationStatusWindowSubclassProc, subclassId) &&
                GetWindowSubclass(
                    window, OperationStatusWindowSubclassProc, subclassId,
                    &referenceData))
            {
                Wh_Log(L"Presentation teardown failed to remove host "
                       L"subclass hwnd=%p error=%lu",
                       reinterpret_cast<void *>(window), GetLastError());
                return FALSE;
            }

            RemoveHostSubclassRecord(window);
            return TRUE;
        }

        if (message == g_positionCirclesMessage)
        {
            PCWSTR reason = TakeProgressCirclePositionReason(window);
            if (reason)
            {
                PositionProgressCirclesForHost(window, reason);
                ApplyNativeDisplayRatesForHost(window);
            }
            return 0;
        }

        if (message == g_logDisplayStateMessage)
        {
            HandleDeferredDisplaySnapshot(
                window, static_cast<unsigned long long>(wParam));
            return 0;
        }

        if (message == WM_NCDESTROY)
        {
            CancelDeferredDisplaySnapshotsForHost(window);
            ForgetHostPresentationState(window);
            ForgetHostNativeGeometry(window);
            if (!DestroyProgressCirclesForHost(window))
            {
                Wh_Log(L"Presentation cleanup incomplete during host "
                       L"destruction hwnd=%p",
                       reinterpret_cast<void *>(window));
            }
            RemoveHostSubclassRecord(window);
            if (!RemoveWindowSubclass(
                    window, OperationStatusWindowSubclassProc, subclassId))
            {
                Wh_Log(L"Presentation cleanup failed to remove destroying "
                       L"host subclass hwnd=%p error=%lu",
                       reinterpret_cast<void *>(window), GetLastError());
            }
        }

        LRESULT result = DefSubclassProc(window, message, wParam, lParam);
        if (message == WM_WINDOWPOSCHANGING && lParam)
        {
            auto *windowPosition = reinterpret_cast<WINDOWPOS *>(lParam);
            int nativeCy = windowPosition->cy;
            int customCy = 0;
            static thread_local bool applyingHeightOverride;
            if (!(windowPosition->flags & SWP_NOSIZE) &&
                !applyingHeightOverride &&
                g_restoringNativeGeometryHost != window)
            {
                applyingHeightOverride = true;
                bool verified = GetVerifiedCustomHostWindowHeight(
                    window, nativeCy, &customCy);
                applyingHeightOverride = false;
                if (verified)
                {
                    windowPosition->cy = customCy;

                    RECT currentWindow{};
                    RECT currentClient{};
                    UINT dpi = GetDpiForWindow(window);
                    if (dpi && GetWindowRect(window, &currentWindow) &&
                        GetClientRect(window, &currentClient))
                    {
                        int nonClientWidth =
                            (currentWindow.right - currentWindow.left) -
                            (currentClient.right - currentClient.left);
                        windowPosition->cx =
                            ScaleForDpi(kRequestedTileWidth, dpi) +
                            nonClientWidth;
                    }

                }
            }
        }
        if (message == WM_SIZE)
        {
            PositionProgressCirclesForHost(window, L"host-size-sync");
            ApplyNativeDisplayRatesForHost(window);
        }
        else if (message == WM_WINDOWPOSCHANGED)
        {
            // Explorer can restack OperationTileHost while the top-level
            // window moves. Repair our single opaque presentation surface in
            // the same message instead of one posted message later.
            PositionProgressCirclesForHost(window, L"host-position-sync");
            ApplyNativeDisplayRatesForHost(window);
        }
        else if (message == WM_DPICHANGED)
        {
            ScheduleProgressCirclePosition(window, L"dpi-change");
        }
        return result;
    }

    void DetachCircleFromProgressWindow(HWND progressWindow)
    {
        HWND hostWindow = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [progressWindow](CircleState const &state)
                { return state.progressWindow == progressWindow; });
            if (it == g_circles.end())
            {
                return;
            }

            hostWindow = it->hostWindow;
            it->progressWindow = nullptr;
            it->positionValid = false;
        }

        // More/Fewer Details can replace the native progress HWND while the
        // OperationTileElement itself stays alive. The circle belongs to the
        // tile, not to that temporary HWND. Keep the circle VISIBLE during
        // the handoff; hiding it here caused it to disappear permanently when
        // Explorer created the replacement progress HWND after our deferred
        // reposition message had already run.
        //
        // The tile-relative bounds remain valid across the transition, so the
        // existing circle can stay on screen and will rebind to the new native
        // progress HWND on the next layout/progress notification.
        if (hostWindow && IsWindow(hostWindow))
        {
            ScheduleProgressCirclePosition(hostWindow,
                                           L"progress-hwnd-recreated");
        }
    }

    LRESULT CALLBACK NativeProgressWindowSubclassProc(
        HWND window,
        UINT message,
        WPARAM wParam,
        LPARAM lParam,
        UINT_PTR subclassId,
        DWORD_PTR)
    {
        if (message == g_removeHostSubclassMessage)
        {
            if (!RemoveWindowSubclass(
                    window, NativeProgressWindowSubclassProc, subclassId))
            {
                Wh_Log(L"Presentation teardown failed to remove owning-thread "
                       L"progress subclass hwnd=%p error=%lu",
                       reinterpret_cast<void *>(window), GetLastError());
                return FALSE;
            }
            return TRUE;
        }

        LRESULT result = DefSubclassProc(window, message, wParam, lParam);
        if (message == WM_NCDESTROY)
        {
            RemoveWindowSubclass(window, NativeProgressWindowSubclassProc,
                                 subclassId);
            DetachCircleFromProgressWindow(window);
        }
        else if (message == WM_WINDOWPOSCHANGED)
        {
            HWND hostWindow = GetAncestor(window, GA_ROOT);
            if (hostWindow)
            {
                ScheduleProgressCirclePosition(hostWindow, L"tile-layout");
            }
        }
        return result;
    }

    bool GetCirclePlacement(CircleState const &state,
                            int *x,
                            int *y,
                            int *width,
                            int *height)
    {
        UINT dpi = GetDpiForWindow(state.hostWindow);
        if (!dpi)
        {
            return false;
        }

        *width = ScaleForDpi(kCircleColumnWidth, dpi);
        *height = ScaleForDpi(kCircleWindowHeight, dpi);

        // Use an explicit host-relative target position. DirectUI bounds are
        // deliberately not a fallback: an unsupported host fails closed.
        *x = ScaleForDpi(ActiveLayout().circleXOffset, dpi);
        *y = ScaleForDpi(kCircleHostY, dpi);
        if (GetRegisteredTileCountForHost(state.hostWindow) > 1)
        {
            int slotTop = 0;
            int slotBottom = 0;
            if (!GetMultiTileSlotBounds(
                    state.tile, state.hostWindow,
                    &slotTop, &slotBottom))
            {
                return false;
            }

            *y = slotTop + ScaleForDpi(kCircleHostY, dpi);
            if (*y + *height > slotBottom)
            {
                *height = std::max(slotBottom - *y, 1);
            }
            return true;
        }
        if (IsWindow(state.hostWindow))
        {
            return true;
        }
        return false;
    }


    HWND GetInfoPanelWindowForTile(OperationTileElement *tile)
    {
        std::lock_guard<std::mutex> lock(g_circleMutex);
        auto it = std::find_if(
            g_circles.begin(), g_circles.end(),
            [tile](CircleState const &state)
            { return state.tile == tile; });
        return it != g_circles.end() ? it->infoWindow : nullptr;
    }

    void InvalidateInfoPanelForTile(OperationTileElement *tile)
    {
        HWND infoWindow = GetInfoPanelWindowForTile(tile);
        if (infoWindow && IsWindow(infoWindow))
        {
            InvalidateRect(infoWindow, nullptr, FALSE);
        }
    }

    void PositionInfoPanel(OperationTileElement *tile)
    {
        HWND infoWindow = nullptr;
        HWND hostWindow = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (it == g_circles.end())
            {
                return;
            }
            infoWindow = it->infoWindow;
            hostWindow = it->hostWindow;
        }

        if (!infoWindow || !IsWindow(infoWindow) ||
            !hostWindow || !IsWindow(hostWindow))
        {
            return;
        }

        if (IsHostInSpecialOperationState(hostWindow))
        {
            if (IsWindowVisible(infoWindow))
            {
                ShowWindow(infoWindow, SW_HIDE);
            }
            return;
        }

        bool expanded = false;
        bool modeKnown = false;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [tile](TransferSummaryState const &state)
                { return state.tile == tile; });
            if (it != g_transferSummaries.end())
            {
                expanded = it->expanded;
                modeKnown = it->displayModeKnown;
            }
        }

        UINT dpi = GetDpiForWindow(hostWindow);
        RECT clientRect{};
        if (!dpi || !GetClientRect(hostWindow, &clientRect))
        {
            return;
        }

        int x = 0;
        int y = 0;
        int width = std::max(static_cast<int>(clientRect.right), 1);

        bool multiTileHost =
            GetRegisteredTileCountForHost(hostWindow) > 1;
        int height = 0;
        if (multiTileHost)
        {
            int slotTop = 0;
            int slotBottom = 0;
            if (!GetMultiTileSlotBounds(
                    tile, hostWindow, &slotTop, &slotBottom))
            {
                return;
            }

            y = slotTop;
            height = std::max(slotBottom - slotTop, 1);
        }
        else
        {
            int logicalHeight =
                modeKnown && expanded ? kExpandedRegularTileHeight
                                      : kCompactRegularTileHeight;
            height = ScaleForDpi(logicalHeight, dpi);
            int footerTop = std::max(
                static_cast<int>(clientRect.bottom) -
                    ScaleForDpi(kFooterOverlayHeight, dpi),
                1);
            height = std::max(std::min(height, footerTop), 1);
        }

        // Avoid forcing the child through SetWindowPos/InvalidateRect on
        // every host WM_WINDOWPOSCHANGED or rate update. While the top-level
        // window is being dragged, this child geometry normally doesn't
        // change at all; repeatedly restacking/repainting it causes visible
        // flashes.
        RECT currentScreen{};
        bool geometryChanged = true;
        if (GetWindowRect(infoWindow, &currentScreen))
        {
            POINT topLeft{currentScreen.left, currentScreen.top};
            if (ScreenToClient(hostWindow, &topLeft))
            {
                int currentWidth =
                    currentScreen.right - currentScreen.left;
                int currentHeight =
                    currentScreen.bottom - currentScreen.top;
                geometryChanged =
                    topLeft.x != x || topLeft.y != y ||
                    currentWidth != width || currentHeight != height;
            }
        }

        bool needsShow = !IsWindowVisible(infoWindow);
        UINT positionFlags =
            SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW;
        if (!geometryChanged)
        {
            // Explorer can restack OperationTileHost above this sibling during
            // More/Fewer transitions without changing either rectangle. Keep
            // the custom information surface above native normal-mode content
            // even when no move, resize, or repaint is otherwise required.
            positionFlags |= SWP_NOMOVE | SWP_NOSIZE;
        }

        if (SetWindowPos(infoWindow, HWND_TOP, x, y, width, height,
                         positionFlags))
        {
            // The footer divider is intentionally hidden in this layout.
            // Keep the custom panel as one continuous surface.
            SetWindowRgn(infoWindow, nullptr, TRUE);

            // Geometry/mode/show changes need one repaint. A pure Z-order
            // repair keeps the existing buffer and avoids drag-time flashing.
            if (geometryChanged || needsShow)
            {
                InvalidateRect(infoWindow, nullptr, FALSE);
            }
        }
    }

    void PositionProgressCircle(OperationTileElement *tile, PCWSTR reason)
    {
        HWND circleWindow = nullptr;

        HWND registeredHost = GetRegisteredCircleHost(tile);
        if (registeredHost &&
            IsHostInSpecialOperationState(registeredHost))
        {
            HideCustomPresentationForHost(registeredHost);
            return;
        }
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;
        bool placementAvailable = false;

        // A compact/expanded transition can swap the progress HWND without
        // changing the tile. Re-resolve it here, including while paused when
        // no progress-position notification may arrive.
        HWND latestProgressWindow =
            OperationTileElement_GetProgressHWND_Original(tile);
        if (latestProgressWindow && !IsWindow(latestProgressWindow))
        {
            latestProgressWindow = nullptr;
        }

        // The native progress HWND remains alive and visible underneath our
        // opaque normal-operation surface. When a special state is detected,
        // hiding our surface reveals Explorer's complete native presentation
        // without reconstructing native visibility.

        HWND previousProgressWindow = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto bindingIt = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (bindingIt != g_circles.end() && latestProgressWindow &&
                bindingIt->progressWindow != latestProgressWindow)
            {
                previousProgressWindow = bindingIt->progressWindow;
            }
        }

        if (previousProgressWindow && IsWindow(previousProgressWindow))
        {
            DWORD_PTR referenceData = 0;
            if (GetWindowSubclass(previousProgressWindow,
                                  NativeProgressWindowSubclassProc,
                                  kProgressWindowSubclassId,
                                  &referenceData) &&
                !RemoveWindowSubclass(previousProgressWindow,
                                      NativeProgressWindowSubclassProc,
                                      kProgressWindowSubclassId))
            {
                Wh_Log(L"Progress HWND rebind failed to remove old subclass "
                       L"hwnd=%p error=%lu",
                       reinterpret_cast<void *>(previousProgressWindow),
                       GetLastError());
                return;
            }
        }

        HWND progressWindowToSubclass = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto bindingIt = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (bindingIt != g_circles.end() && latestProgressWindow &&
                bindingIt->progressWindow != latestProgressWindow)
            {
                bindingIt->progressWindow = latestProgressWindow;
                bindingIt->positionValid = false;
                progressWindowToSubclass = latestProgressWindow;
            }
        }

        if (progressWindowToSubclass &&
            !SetWindowSubclass(progressWindowToSubclass,
                               NativeProgressWindowSubclassProc,
                               kProgressWindowSubclassId, 0))
        {
            Wh_Log(L"Progress HWND rebind failed to install new subclass "
                   L"hwnd=%p error=%lu",
                   reinterpret_cast<void *>(progressWindowToSubclass),
                   GetLastError());
        }

        CircleState placementState{};
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (it == g_circles.end())
            {
                return;
            }
            placementState = *it;
            circleWindow = it->circleWindow;
        }

        placementAvailable =
            GetCirclePlacement(placementState, &x, &y, &width, &height);

        if (!placementAvailable || !circleWindow || !IsWindow(circleWindow))
        {
            return;
        }

        // Keep the legacy circle HWND as a lifetime/registration anchor only.
        // The visible ring is painted inside infoWindow, eliminating the
        // sibling-Z-order race that made the ring disappear while moving the
        // Explorer window or toggling More/Fewer.
        if (IsWindowVisible(circleWindow))
        {
            ShowWindow(circleWindow, SW_HIDE);
        }

        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (it != g_circles.end() && it->circleWindow == circleWindow)
            {
                it->positionX = x;
                it->positionY = y;
                it->positionWidth = width;
                it->positionHeight = height;
                it->positionValid = true;
            }
        }

        PositionInfoPanel(tile);
        PositionFooterOverlay(tile);
    }

    void PositionProgressCirclesForHost(HWND hostWindow, PCWSTR reason)
    {
        std::vector<OperationTileElement *> tiles;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            for (auto const &state : g_circles)
            {
                if (state.hostWindow == hostWindow)
                {
                    tiles.push_back(state.tile);
                }
            }
        }
        for (OperationTileElement *tile : tiles)
        {
            PositionProgressCircle(tile, reason);
        }
    }

    void ScheduleProgressCirclePosition(HWND hostWindow, PCWSTR reason)
    {
        if (g_unloading.load(std::memory_order_acquire) ||
            !hostWindow || !g_positionCirclesMessage)
        {
            return;
        }

        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_hostPositionRequests.begin(),
                g_hostPositionRequests.end(),
                [hostWindow](HostPositionRequest const &request)
                {
                    return request.hostWindow == hostWindow;
                });
            if (it != g_hostPositionRequests.end())
            {
                it->reason = reason;
                return;
            }
            g_hostPositionRequests.push_back({hostWindow, reason});
        }

        if (!PostMessageW(hostWindow, g_positionCirclesMessage, 0, 0))
        {
            DWORD error = GetLastError();
            TakeProgressCirclePositionReason(hostWindow);
            Wh_Log(L"Circle position scheduling failed host=%p error=%lu",
                   reinterpret_cast<void *>(hostWindow), error);
        }
    }

    bool EnsureHostSubclass(HWND hostWindow, unsigned long long eventId)
    {
        if (g_unloading.load(std::memory_order_acquire))
        {
            return false;
        }

        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            if (std::find(g_subclassedHosts.begin(), g_subclassedHosts.end(),
                          hostWindow) != g_subclassedHosts.end())
            {
                return true;
            }
        }

        ApplyUnifiedHostChrome(hostWindow);

        if (!SetWindowSubclass(hostWindow, OperationStatusWindowSubclassProc,
                               kHostWindowSubclassId, 0))
        {
            Wh_Log(L"eventId=%llu circle SetWindowSubclass host failed "
                   L"error=%lu",
                   eventId, GetLastError());
            if (ShouldApplyNativeColorOverrides())
            {
                ResetUnifiedHostChrome(hostWindow);
            }
            return false;
        }

        bool shuttingDown = false;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            shuttingDown = g_unloading.load(std::memory_order_acquire);
            if (!shuttingDown)
            {
                g_subclassedHosts.push_back(hostWindow);
            }
        }

        if (shuttingDown)
        {
            DWORD_PTR referenceData = 0;
            bool subclassRemains =
                !RemoveWindowSubclass(
                    hostWindow, OperationStatusWindowSubclassProc,
                    kHostWindowSubclassId) &&
                GetWindowSubclass(
                    hostWindow, OperationStatusWindowSubclassProc,
                    kHostWindowSubclassId, &referenceData);
            if (subclassRemains)
            {
                std::lock_guard<std::mutex> lock(g_circleMutex);
                if (std::find(g_subclassedHosts.begin(),
                              g_subclassedHosts.end(), hostWindow) ==
                    g_subclassedHosts.end())
                {
                    // This is teardown tracking, not presentation activation.
                    // Wh_ModBeforeUninit will synchronously retry removal.
                    g_subclassedHosts.push_back(hostWindow);
                }
                Wh_Log(L"Presentation activation rollback failed to remove "
                       L"host subclass hwnd=%p error=%lu",
                       reinterpret_cast<void *>(hostWindow), GetLastError());
            }
            else
            {
                ForgetHostNativeGeometry(hostWindow);
            }
            if (ShouldApplyNativeColorOverrides())
            {
                ResetUnifiedHostChrome(hostWindow);
            }
            return false;
        }

        wchar_t currentCaption[160]{};
        if (GetWindowTextW(hostWindow, currentCaption,
                           ARRAYSIZE(currentCaption)) > 0)
        {
            bool enteredSpecial = false;
            bool leftSpecial = false;
            UpdateHostPresentationStateFromCaption(
                hostWindow, currentCaption,
                &enteredSpecial, &leftSpecial);
            if (enteredSpecial)
            {
                HideCustomPresentationForHost(hostWindow);
            }
        }

        return true;
    }

    HWND GetOperationStatusWindowForTile(HWND progressWindow,
                                         unsigned long long eventId)
    {
        HWND hostWindow = progressWindow
                              ? GetAncestor(progressWindow, GA_ROOT)
                              : nullptr;
        wchar_t className[64];
        if (hostWindow &&
            GetClassNameW(hostWindow, className, ARRAYSIZE(className)) &&
            lstrcmpW(className, L"OperationStatusWindow") == 0)
        {
            return hostWindow;
        }
        return FindCurrentThreadOperationStatusWindow(eventId);
    }

    bool CopyRegisteredTransferState(COperationStatusTile *owner,
                                     TransferSummaryState *state)
    {
        std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
        auto it = std::find_if(
            g_transferSummaries.begin(), g_transferSummaries.end(),
            [owner](TransferSummaryState const &candidate)
            { return candidate.owner == owner; });
        if (it == g_transferSummaries.end())
        {
            return false;
        }
        *state = *it;
        return true;
    }

    HWND GetRegisteredCircleHost(OperationTileElement *tile)
    {
        std::lock_guard<std::mutex> lock(g_circleMutex);
        auto it = std::find_if(
            g_circles.begin(), g_circles.end(),
            [tile](CircleState const &state)
            { return state.tile == tile; });
        return it != g_circles.end() ? it->hostWindow : nullptr;
    }

    size_t GetRegisteredTileCountForHost(HWND hostWindow)
    {
        if (!hostWindow)
        {
            return 0;
        }

        std::lock_guard<std::mutex> lock(g_circleMutex);
        size_t count = 0;
        for (CircleState const &state : g_circles)
        {
            if (state.hostWindow == hostWindow && state.tile)
            {
                ++count;
            }
        }
        return count;
    }

    bool GetMultiTileSlotBounds(
        OperationTileElement *tile,
        HWND hostWindow,
        int *slotTop,
        int *slotBottom)
    {
        if (!tile || !hostWindow || !IsWindow(hostWindow) ||
            !slotTop || !slotBottom)
        {
            return false;
        }

        std::vector<OperationTileElement *> hostTiles;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            for (CircleState const &state : g_circles)
            {
                if (state.hostWindow == hostWindow && state.tile)
                {
                    hostTiles.push_back(state.tile);
                }
            }
        }

        if (hostTiles.size() < 2)
        {
            return false;
        }

        auto tileIt = std::find(hostTiles.begin(), hostTiles.end(), tile);
        if (tileIt == hostTiles.end())
        {
            return false;
        }

        RECT clientRect{};
        UINT dpi = GetDpiForWindow(hostWindow);
        if (!dpi || !GetClientRect(hostWindow, &clientRect))
        {
            return false;
        }

        int clientHeight =
            std::max(static_cast<int>(clientRect.bottom - clientRect.top), 0);
        int footerReserve =
            ScaleForDpi(kDisplayModeFooterReserveHeight, dpi);
        int contentHeight = std::max(clientHeight - footerReserve, 0);

        // Explorer owns the multi-operation host height and lays its operation
        // rows into equal vertical slots above one shared footer. Do not use
        // the per-tile native progress HWND as a vertical datum: on this shell
        // build those HWNDs can move only a few dozen pixels apart even while
        // the DirectUI operation rows themselves are ~200px apart.
        int minimumSlotHeight = ScaleForDpi(120, dpi);
        if (contentHeight <
            minimumSlotHeight * static_cast<int>(hostTiles.size()))
        {
            return false;
        }

        size_t index =
            static_cast<size_t>(tileIt - hostTiles.begin());
        long long count =
            static_cast<long long>(hostTiles.size());

        *slotTop = static_cast<int>(
            (static_cast<long long>(contentHeight) *
             static_cast<long long>(index)) /
            count);
        *slotBottom = static_cast<int>(
            (static_cast<long long>(contentHeight) *
             static_cast<long long>(index + 1)) /
            count);

        return *slotBottom > *slotTop;
    }

    void RefreshDeleteLikeOperationKind(
        COperationStatusTile *owner,
        TransferSummaryState const &state)
    {
        if (!owner || !state.operationTileRoot)
        {
            return;
        }

        DirectUI::Element *firstLocation = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltFirstLocation", true);
        DirectUI::Element *secondLocation = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltSecondLocation", true);

        // Operation kind is data, not layout. Read the native description
        // strings instead of inferring meaning from machine-dependent bounds.
        bool firstPresent = !ReadDirectUiText(firstLocation).empty();
        bool secondPresent = !ReadDirectUiText(secondLocation).empty();
        if (!firstPresent && !secondPresent)
        {
            return;
        }

        // Copy/move expose a non-empty source and destination location. Delete
        // has only the source location; an empty second-location placeholder can
        // still exist, so content rather than element existence is decisive.
        bool deleteLike = firstPresent && !secondPresent;

        std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
        auto it = std::find_if(
            g_transferSummaries.begin(), g_transferSummaries.end(),
            [owner](TransferSummaryState const &candidate)
            { return candidate.owner == owner; });
        if (it == g_transferSummaries.end())
        {
            return;
        }

        it->deleteLikeKnown = true;
        it->deleteLike = deleteLike;
    }

    bool GetUniqueRegisteredCircleHost(OperationTileElement *tile,
                                       HWND *hostWindow)
    {
        std::lock_guard<std::mutex> lock(g_circleMutex);
        HWND match = nullptr;
        size_t matches = 0;
        for (CircleState const &state : g_circles)
        {
            if (state.tile == tile)
            {
                match = state.hostWindow;
                ++matches;
            }
        }
        if (matches != 1)
        {
            return false;
        }
        *hostWindow = match;
        return true;
    }

    bool IsLiveDisplayModeState(TransferSummaryState const &state)
    {
        if (!state.tile || !state.operationTileRoot)
        {
            return false;
        }

        HWND hostWindow = nullptr;
        if (!GetUniqueRegisteredCircleHost(state.tile, &hostWindow) ||
            !hostWindow || !IsWindow(hostWindow) ||
            GetWindowThreadProcessId(hostWindow, nullptr) !=
                GetCurrentThreadId())
        {
            return false;
        }

        wchar_t className[64]{};
        return GetClassNameW(hostWindow, className, ARRAYSIZE(className)) &&
               lstrcmpW(className, L"OperationStatusWindow") == 0;
    }

    bool ResolveDisplayModeOwner(COperationStatusTile *requestedOwner,
                                 unsigned long long transitionId,
                                 COperationStatusTile **canonicalOwner,
                                 bool logResult = true)
    {
        (void)transitionId;
        (void)logResult;
        std::vector<TransferSummaryState> states;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            states = g_transferSummaries;
        }

        auto resolveCandidate = [&states](COperationStatusTile *candidate)
            -> bool
        {
            TransferSummaryState match{};
            size_t matches = 0;
            for (TransferSummaryState const &state : states)
            {
                if (state.owner == candidate)
                {
                    match = state;
                    ++matches;
                }
            }
            return matches == 1 && IsLiveDisplayModeState(match);
        };

        if (resolveCandidate(requestedOwner))
        {
            *canonicalOwner = requestedOwner;
            return true;
        }

        ULONG_PTR requestedAddress =
            reinterpret_cast<ULONG_PTR>(requestedOwner);
        if (requestedAddress >= kSetTileDisplayModeThisAdjustment)
        {
            auto *adjustedCandidate = reinterpret_cast<COperationStatusTile *>(
                requestedAddress - kSetTileDisplayModeThisAdjustment);
            if (resolveCandidate(adjustedCandidate))
            {
                *canonicalOwner = adjustedCandidate;
                return true;
            }
        }

        *canonicalOwner = nullptr;
        return false;
    }


    void ScheduleDeferredDisplaySnapshot(COperationStatusTile *owner,
                                         unsigned long long transitionId,
                                         bool requestedExpanded)
    {
        if (g_unloading.load(std::memory_order_acquire) ||
            !g_logDisplayStateMessage)
        {
            return;
        }

        TransferSummaryState state{};
        if (!CopyRegisteredTransferState(owner, &state) || !state.tile ||
            !state.operationTileRoot)
        {
            return;
        }

        HWND progressWindow =
            OperationTileElement_GetProgressHWND_Original(state.tile);
        HWND hostWindow = progressWindow && IsWindow(progressWindow)
                              ? GetAncestor(progressWindow, GA_ROOT)
                              : GetRegisteredCircleHost(state.tile);
        DWORD currentThreadId = GetCurrentThreadId();
        if (!hostWindow || !IsWindow(hostWindow) ||
            GetWindowThreadProcessId(hostWindow, nullptr) != currentThreadId)
        {
            Wh_Log(L"MODE[%llu] DEFERRED schedule-skipped "
                   L"reason=invalid-or-cross-thread-host host=%p thread=%lu",
                   transitionId, reinterpret_cast<void *>(hostWindow),
                   currentThreadId);
            return;
        }

        DeferredDisplaySnapshot snapshot{
            owner, state.tile, state.operationTileRoot, hostWindow,
            currentThreadId, transitionId, requestedExpanded};
        {
            std::lock_guard<std::mutex> lock(g_displayDiagnosticMutex);
            g_deferredDisplaySnapshots.push_back(snapshot);
        }

        if (!PostMessageW(hostWindow, g_logDisplayStateMessage,
                          static_cast<WPARAM>(transitionId), 0))
        {
            DWORD error = GetLastError();
            std::lock_guard<std::mutex> lock(g_displayDiagnosticMutex);
            g_deferredDisplaySnapshots.erase(
                std::remove_if(
                    g_deferredDisplaySnapshots.begin(),
                    g_deferredDisplaySnapshots.end(),
                    [transitionId](DeferredDisplaySnapshot const &candidate)
                    { return candidate.transitionId == transitionId; }),
                g_deferredDisplaySnapshots.end());
            Wh_Log(L"MODE[%llu] DEFERRED PostMessage failed error=%lu",
                   transitionId, error);
        }
    }

    void HandleDeferredDisplaySnapshot(HWND hostWindow,
                                       unsigned long long transitionId)
    {
        DeferredDisplaySnapshot snapshot{};
        bool found = false;
        {
            std::lock_guard<std::mutex> lock(g_displayDiagnosticMutex);
            auto it = std::find_if(
                g_deferredDisplaySnapshots.begin(),
                g_deferredDisplaySnapshots.end(),
                [hostWindow, transitionId](
                    DeferredDisplaySnapshot const &candidate)
                {
                    return candidate.hostWindow == hostWindow &&
                           candidate.transitionId == transitionId;
                });
            if (it != g_deferredDisplaySnapshots.end())
            {
                snapshot = *it;
                g_deferredDisplaySnapshots.erase(it);
                found = true;
            }
        }

        if (!found)
        {
            return;
        }

        TransferSummaryState currentState{};
        HWND currentHostWindow = nullptr;
        bool currentRegistration =
            CopyRegisteredTransferState(snapshot.owner, &currentState) &&
            currentState.tile == snapshot.tile &&
            currentState.operationTileRoot == snapshot.operationTileRoot &&
            currentState.displayModeKnown &&
            currentState.expanded == snapshot.requestedExpanded &&
            GetCurrentThreadId() == snapshot.uiThreadId &&
            GetUniqueRegisteredCircleHost(currentState.tile,
                                          &currentHostWindow) &&
            currentHostWindow == hostWindow && IsWindow(hostWindow) &&
            GetWindowThreadProcessId(hostWindow, nullptr) ==
                snapshot.uiThreadId;
        if (!currentRegistration)
        {
            return;
        }

        ApplyDisplayMode(snapshot.owner, true, transitionId);
    }

    void RegisterTransferSummary(COperationStatusTile *owner,
                                 OperationTileElement *tile,
                                 DirectUI::Element *operationTileRoot,
                                 DirectUI::Element *tileHeaderRoot)
    {
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            if (g_unloading.load(std::memory_order_acquire))
            {
                return;
            }
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [owner](TransferSummaryState const &state)
                { return state.owner == owner; });
            if (it == g_transferSummaries.end())
            {
                g_transferSummaries.push_back(
                    {owner, tile, operationTileRoot, tileHeaderRoot,
                     0, 0, false, false, false});
            }
            else
            {
                it->tile = tile;
                it->operationTileRoot = operationTileRoot;
                it->tileHeaderRoot = tileHeaderRoot;
            }
        }

        HWND hostWindow = tile ? GetRegisteredCircleHost(tile) : nullptr;
        if (hostWindow && GetRegisteredTileCountForHost(hostWindow) > 1)
        {
            MarkHostForMeasuredMultiRate(hostWindow);
        }

        // Establish copy/move vs. delete before the first custom display-mode
        // application. That prevents a normal delete caption from being
        // mistaken for a special/error transition during startup.
        TransferSummaryState registeredState{};
        if (CopyRegisteredTransferState(owner, &registeredState))
        {
            RefreshDeleteLikeOperationKind(owner, registeredState);
        }

        // Reapply only state already associated with this exact live owner.
        // Unregistered display-mode subobject pointers are never retained.
        InitializeRegisteredDisplayMode(owner);
    }

    void RemoveTransferSummary(OperationTileElement *tile)
    {
        std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
        g_transferSummaries.erase(
            std::remove_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [tile](TransferSummaryState const &state)
                { return state.tile == tile; }),
            g_transferSummaries.end());
    }

    void ApplyTransferSummary(COperationStatusTile *owner)
    {
        OperationTileElement *tile = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [owner](TransferSummaryState const &candidate)
                { return candidate.owner == owner; });
            if (it == g_transferSummaries.end())
            {
                return;
            }
            tile = it->tile;
        }

        // The transferred/total summary is now rendered entirely by the
        // custom presentation. Never rewrite Explorer's native summary; it is
        // part of the untouched fallback revealed for special/error states.
        InvalidateInfoPanelForTile(tile);
    }

    void ApplyNativeDisplayRate(COperationStatusTile *owner)
    {
        TransferSummaryState state{};
        if (!CopyRegisteredTransferState(owner, &state) ||
            !state.nativeDisplayRateValid || !state.tile)
        {
            return;
        }

        // The custom info panel renders speed from the native Shell rate.
        // Do not repurpose Explorer's item-name link/value pair anymore.
        InvalidateInfoPanelForTile(state.tile);
    }

    void ApplyNativeDisplayRatesForHost(HWND hostWindow)
    {
        std::vector<TransferSummaryState> states;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            states = g_transferSummaries;
        }
        for (TransferSummaryState const &state : states)
        {
            HWND registeredHost = nullptr;
            if (state.nativeDisplayRateValid && state.owner && state.tile &&
                GetUniqueRegisteredCircleHost(state.tile, &registeredHost) &&
                registeredHost == hostWindow)
            {
                ApplyNativeDisplayRate(state.owner);
            }
        }
    }

    bool RecordNativeDisplayRateForOwner(
        COperationStatusTile *owner,
        double nativeDisplayRate)
    {
        if (!owner)
        {
            return false;
        }

        OperationTileElement *tile = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [owner](TransferSummaryState const &candidate)
                { return candidate.owner == owner; });
            if (it == g_transferSummaries.end() || !it->tile ||
                !it->operationTileRoot || it->preferMeasuredRate)
            {
                return false;
            }
            tile = it->tile;
        }

        HWND hostWindow = nullptr;
        if (!GetUniqueRegisteredCircleHost(tile, &hostWindow) ||
            !hostWindow || !IsWindow(hostWindow) ||
            GetWindowThreadProcessId(hostWindow, nullptr) !=
                GetCurrentThreadId())
        {
            return false;
        }

        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [owner, tile](TransferSummaryState const &candidate)
                { return candidate.owner == owner && candidate.tile == tile; });
            if (it == g_transferSummaries.end())
            {
                return false;
            }
            bool deleteLike =
                it->deleteLikeKnown && it->deleteLike;
            bool keepPreviousDeleteRate =
                deleteLike && nativeDisplayRate < 0.01 &&
                it->nativeDisplayRateValid &&
                it->nativeDisplayRate >= 0.01;

            if (!keepPreviousDeleteRate)
            {
                it->nativeDisplayRate = nativeDisplayRate;
                it->nativeDisplayRateValid = true;
                it->nativeRateHistory.push_back(nativeDisplayRate);
            }
            if (it->nativeRateHistory.size() > kInfoPanelRateHistorySamples)
            {
                it->nativeRateHistory.erase(
                    it->nativeRateHistory.begin(),
                    it->nativeRateHistory.begin() +
                        (it->nativeRateHistory.size() -
                         kInfoPanelRateHistorySamples));
            }
        }

        InvalidateInfoPanelForTile(tile);
        return true;
    }

    void RecordNativeDisplayRateForCurrentThread(double nativeDisplayRate)
    {
        std::vector<TransferSummaryState> states;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            states = g_transferSummaries;
        }

        COperationStatusTile *owner = nullptr;
        OperationTileElement *tile = nullptr;
        HWND matchedHostWindow = nullptr;
        size_t matches = 0;
        DWORD currentThreadId = GetCurrentThreadId();
        for (TransferSummaryState const &state : states)
        {
            HWND hostWindow = nullptr;
            if (!state.owner || !state.tile || !state.operationTileRoot ||
                !GetUniqueRegisteredCircleHost(state.tile, &hostWindow) ||
                !hostWindow || !IsWindow(hostWindow) ||
                GetWindowThreadProcessId(hostWindow, nullptr) !=
                    currentThreadId ||
                !IsSingleNormalProgressTileForHost(state.tile, hostWindow))
            {
                continue;
            }
            owner = state.owner;
            tile = state.tile;
            matchedHostWindow = hostWindow;
            ++matches;
        }
        if (matches != 1)
        {
            return;
        }

        bool recorded = false;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [owner, tile](TransferSummaryState const &candidate)
                {
                    return candidate.owner == owner &&
                           candidate.tile == tile;
                });
            if (it != g_transferSummaries.end())
            {
                it->nativeDisplayRate = nativeDisplayRate;
                it->nativeDisplayRateValid = true;
                it->nativeRateHistory.push_back(nativeDisplayRate);
                if (it->nativeRateHistory.size() >
                    kInfoPanelRateHistorySamples)
                {
                    it->nativeRateHistory.erase(
                        it->nativeRateHistory.begin(),
                        it->nativeRateHistory.begin() +
                            (it->nativeRateHistory.size() -
                             kInfoPanelRateHistorySamples));
                }
                recorded = true;
            }
        }
        if (recorded)
        {
            ScheduleProgressCirclePosition(matchedHostWindow,
                                           L"native-rate");
        }
    }

    bool IsElementDescendantOf(DirectUI::Element *element,
                               DirectUI::Element *ancestor);

    struct NormalProgressLayoutElements
    {
        DirectUI::Element *descriptionHeader;
        DirectUI::Element *summary;
        DirectUI::Element *details;
        DirectUI::Element *speedLabel;
        DirectUI::Element *speedValue;
        DirectUI::Element *timeRemainingLabel;
        DirectUI::Element *timeRemaining;
        DirectUI::Element *itemsRemainingLabel;
        DirectUI::Element *itemsRemaining;
        DirectUI::Element *chartArea;
        DirectUI::Element *rateChart;
        DirectUI::Element *progressBarContainer;
        DirectUI::Element *progressBar;
        DirectUI::Element *regularTile;
    };

    bool DiscoverNormalProgressLayout(
        TransferSummaryState const &state,
        NormalProgressLayoutElements *elements)
    {
        elements->descriptionHeader = state.tileHeaderRoot;
        elements->summary = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltSummary", false);
        elements->details = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltDetails", false);
        elements->speedLabel = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltItemNameLabel", false);
        elements->speedValue = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltItemName", false);
        elements->timeRemainingLabel = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltTimeRemainingLabel", false);
        elements->timeRemaining = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltTimeRemaining", false);
        elements->itemsRemainingLabel = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltItemsRemainingLabel", false);
        elements->itemsRemaining = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltItemsRemaining", false);
        elements->chartArea = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltChartArea", false);
        elements->rateChart = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltRateChart_New", false);
        elements->progressBarContainer = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltProgressBarContainer", false);
        elements->progressBar = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltProgressBar", false);
        elements->regularTile = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltRegularTile", false);
        return elements->descriptionHeader && elements->summary &&
               elements->details && elements->speedLabel &&
               elements->speedValue && elements->timeRemainingLabel &&
               elements->timeRemaining && elements->itemsRemainingLabel &&
               elements->itemsRemaining && elements->chartArea &&
               elements->rateChart && elements->progressBarContainer &&
               elements->progressBar && elements->regularTile;
    }

    bool ValidateDeleteLikeProgressHierarchy(
        NormalProgressLayoutElements const &elements,
        unsigned long long transitionId,
        bool logResult = true)
    {
        (void)transitionId;
        (void)logResult;
        // Delete/recycle tiles can omit the copy/move speed-value pair.
        // Require the stable structural core and validate every optional
        // native element that actually exists.
        if (!elements.descriptionHeader || !elements.summary ||
            !elements.details || !elements.regularTile)
        {
            return false;
        }

        auto underOrMissing = [](DirectUI::Element *element,
                                 DirectUI::Element *ancestor)
        {
            return !element ||
                   (ancestor && IsElementDescendantOf(element, ancestor));
        };

        bool valid =
            IsElementDescendantOf(elements.descriptionHeader,
                                  elements.regularTile) &&
            IsElementDescendantOf(elements.summary, elements.regularTile) &&
            IsElementDescendantOf(elements.details, elements.regularTile) &&
            underOrMissing(elements.speedLabel, elements.details) &&
            underOrMissing(elements.speedValue, elements.details) &&
            underOrMissing(elements.timeRemainingLabel, elements.details) &&
            underOrMissing(elements.timeRemaining, elements.details) &&
            underOrMissing(elements.itemsRemainingLabel, elements.details) &&
            underOrMissing(elements.itemsRemaining, elements.details) &&
            underOrMissing(elements.chartArea, elements.details) &&
            underOrMissing(elements.rateChart, elements.chartArea) &&
            underOrMissing(elements.progressBarContainer,
                           elements.regularTile) &&
            underOrMissing(elements.progressBar,
                           elements.progressBarContainer);

        return valid;
    }

    bool IsElementDescendantOf(DirectUI::Element *element,
                               DirectUI::Element *ancestor)
    {
        constexpr unsigned int kMaximumAncestryDepth = 32;
        DirectUI::Element *visited[kMaximumAncestryDepth]{};

        if (!element || !ancestor)
        {
            return false;
        }

        for (unsigned int depth = 0; depth < kMaximumAncestryDepth; ++depth)
        {
            if (!element)
            {
                return false;
            }
            for (unsigned int index = 0; index < depth; ++index)
            {
                if (visited[index] == element)
                {
                    return false;
                }
            }
            visited[depth] = element;

            if (element == ancestor)
            {
                return true;
            }
            element = Element_GetParent_Original(element);
        }
        return false;
    }

    bool ValidateNormalProgressHierarchy(
        NormalProgressLayoutElements const &elements,
        unsigned long long transitionId,
        bool logResult = true)
    {
        (void)transitionId;
        (void)logResult;

        bool headerUnderRegular = IsElementDescendantOf(
            elements.descriptionHeader, elements.regularTile);
        bool summaryUnderRegular = IsElementDescendantOf(
            elements.summary, elements.regularTile);
        bool detailsUnderRegular = IsElementDescendantOf(
            elements.details, elements.regularTile);
        bool speedLabelUnderDetails = IsElementDescendantOf(
            elements.speedLabel, elements.details);
        bool speedValueUnderDetails = IsElementDescendantOf(
            elements.speedValue, elements.details);
        bool timeLabelUnderDetails = IsElementDescendantOf(
            elements.timeRemainingLabel, elements.details);
        bool timeValueUnderDetails = IsElementDescendantOf(
            elements.timeRemaining, elements.details);
        bool itemsLabelUnderDetails = IsElementDescendantOf(
            elements.itemsRemainingLabel, elements.details);
        bool itemsValueUnderDetails = IsElementDescendantOf(
            elements.itemsRemaining, elements.details);
        bool chartAreaUnderDetails = IsElementDescendantOf(
            elements.chartArea, elements.details);
        bool rateChartUnderChartArea = IsElementDescendantOf(
            elements.rateChart, elements.chartArea);
        bool progressContainerUnderRegular = IsElementDescendantOf(
            elements.progressBarContainer, elements.regularTile);
        bool progressBarUnderContainer = IsElementDescendantOf(
            elements.progressBar, elements.progressBarContainer);

        bool valid = headerUnderRegular && summaryUnderRegular &&
                     detailsUnderRegular && speedLabelUnderDetails &&
                     speedValueUnderDetails && timeLabelUnderDetails &&
                     timeValueUnderDetails && itemsLabelUnderDetails &&
                     itemsValueUnderDetails && chartAreaUnderDetails &&
                     rateChartUnderChartArea &&
                     progressContainerUnderRegular &&
                     progressBarUnderContainer;

        return valid;
    }


    bool IsSingleNormalProgressTileForHost(OperationTileElement *tile,
                                           HWND hostWindow)
    {
        std::lock_guard<std::mutex> lock(g_circleMutex);
        size_t hostTiles = 0;
        size_t matchingTiles = 0;
        for (CircleState const &circle : g_circles)
        {
            if (circle.hostWindow == hostWindow)
            {
                ++hostTiles;
                if (circle.tile == tile)
                {
                    ++matchingTiles;
                }
            }
        }
        return hostTiles == 1 && matchingTiles == 1;
    }

    bool CalculateCustomHostWindowHeight(HWND hostWindow,
                                         bool expanded,
                                         int *targetWindowHeight,
                                         int *targetClientHeight = nullptr)
    {
        RECT windowRect{};
        RECT clientRect{};
        UINT dpi = GetDpiForWindow(hostWindow);
        if (!dpi || !GetWindowRect(hostWindow, &windowRect) ||
            !GetClientRect(hostWindow, &clientRect))
        {
            return false;
        }

        size_t tileCount = std::max<size_t>(
            GetRegisteredTileCountForHost(hostWindow), 1);
        int logicalTileHeight = expanded ? kExpandedRegularTileHeight
                                         : kCompactRegularTileHeight;
        int logicalClientHeight =
            logicalTileHeight * static_cast<int>(tileCount) +
            kDisplayModeFooterReserveHeight;
        int scaledClientHeight = ScaleForDpi(logicalClientHeight, dpi);
        int windowHeight = windowRect.bottom - windowRect.top;
        int clientHeight = clientRect.bottom - clientRect.top;
        *targetWindowHeight =
            scaledClientHeight + (windowHeight - clientHeight);
        if (targetClientHeight)
        {
            *targetClientHeight = scaledClientHeight;
        }
        return true;
    }

    bool GetVerifiedCustomHostWindowHeight(HWND hostWindow,
                                           int nativeWindowHeight,
                                           int *targetWindowHeight)
    {
        (void)nativeWindowHeight;

        if (IsHostInSpecialOperationState(hostWindow))
        {
            return false;
        }

        std::vector<TransferSummaryState> states;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            states = g_transferSummaries;
        }

        bool expanded = false;
        bool modeFound = false;
        for (TransferSummaryState const &state : states)
        {
            if (!state.displayModeKnown || !state.tile ||
                !state.operationTileRoot)
            {
                continue;
            }

            HWND registeredHost = nullptr;
            if (GetUniqueRegisteredCircleHost(state.tile, &registeredHost) &&
                registeredHost == hostWindow)
            {
                expanded = expanded || state.expanded;
                modeFound = true;
            }
        }
        if (!modeFound || GetRegisteredTileCountForHost(hostWindow) == 0)
        {
            return false;
        }

        return CalculateCustomHostWindowHeight(
            hostWindow, expanded, targetWindowHeight);
    }

    bool ResizeOperationStatusWindowForMode(
        HWND hostWindow,
        bool expanded,
        unsigned long long transitionId)
    {
        RECT windowRect{};
        RECT clientRect{};
        int targetWindowHeight = 0;
        int targetClientHeight = 0;
        if (!GetWindowRect(hostWindow, &windowRect) ||
            !GetClientRect(hostWindow, &clientRect) ||
            !CalculateCustomHostWindowHeight(
                hostWindow, expanded, &targetWindowHeight,
                &targetClientHeight))
        {
            Wh_Log(L"MODE[%llu] CUSTOM_LAYOUT resize-failed host=%p "
                   L"reason=window-rect error=%lu",
                   transitionId, reinterpret_cast<void *>(hostWindow),
                   GetLastError());
            return false;
        }

        int windowWidth = windowRect.right - windowRect.left;
        int windowHeight = windowRect.bottom - windowRect.top;
        int clientWidth = clientRect.right - clientRect.left;
        UINT dpi = GetDpiForWindow(hostWindow);
        if (!dpi)
        {
            return false;
        }
        int targetClientWidth = ScaleForDpi(kRequestedTileWidth, dpi);
        int nonClientWidth = windowWidth - clientWidth;
        int targetWindowWidth = targetClientWidth + nonClientWidth;

        if (windowHeight != targetWindowHeight ||
            windowWidth != targetWindowWidth)
        {
            ScopedHostGeometryChange geometryChange(hostWindow, false);
            if (!SetWindowPos(hostWindow, nullptr, 0, 0,
                              targetWindowWidth, targetWindowHeight,
                              SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE))
            {
                Wh_Log(L"MODE[%llu] CUSTOM_LAYOUT resize-failed host=%p "
                       L"targetClientWidth=%d targetClientHeight=%d "
                       L"targetWindowWidth=%d targetWindowHeight=%d "
                       L"error=%lu",
                       transitionId, reinterpret_cast<void *>(hostWindow),
                       targetClientWidth, targetClientHeight,
                       targetWindowWidth, targetWindowHeight, GetLastError());
                return false;
            }
        }

        RECT windowAfter{};
        RECT clientAfter{};
        if (!GetWindowRect(hostWindow, &windowAfter) ||
            !GetClientRect(hostWindow, &clientAfter))
        {
            return false;
        }
        int actualWindowWidth = windowAfter.right - windowAfter.left;
        int actualWindowHeight = windowAfter.bottom - windowAfter.top;
        int actualClientWidth = clientAfter.right - clientAfter.left;
        int actualClientHeight = clientAfter.bottom - clientAfter.top;
        bool sizeVerified = actualWindowWidth == targetWindowWidth &&
                            actualWindowHeight == targetWindowHeight &&
                            actualClientWidth == targetClientWidth &&
                            actualClientHeight == targetClientHeight;

        if (!sizeVerified)
        {
            Wh_Log(L"MODE[%llu] CUSTOM_LAYOUT resize-verification-failed "
                   L"host=%p targetClient=%dx%d actualClient=%dx%d "
                   L"targetWindow=%dx%d actualWindow=%dx%d",
                   transitionId, reinterpret_cast<void *>(hostWindow),
                   targetClientWidth, targetClientHeight,
                   actualClientWidth, actualClientHeight,
                   targetWindowWidth, targetWindowHeight,
                   actualWindowWidth, actualWindowHeight);
        }
        return sizeVerified;
    }

    bool ApplyDisplayMode(COperationStatusTile *owner,
                          bool applyFinalHostGeometry,
                          unsigned long long transitionId)
    {
        TransferSummaryState state{};
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [owner](TransferSummaryState const &candidate)
                { return candidate.owner == owner; });
            if (it == g_transferSummaries.end() || !it->displayModeKnown ||
                !it->tile || !it->operationTileRoot)
            {
                return false;
            }
            state = *it;
        }

        HWND hostWindow = nullptr;
        if (!GetUniqueRegisteredCircleHost(state.tile, &hostWindow) ||
            !hostWindow || !IsWindow(hostWindow) ||
            GetWindowThreadProcessId(hostWindow, nullptr) !=
                GetCurrentThreadId())
        {
            Wh_Log(L"MODE[%llu] CUSTOM_LAYOUT owner=%p result=skipped "
                   L"reason=invalid-tile-host",
                   transitionId, reinterpret_cast<void *>(owner));
            return false;
        }

        if (IsHostInSpecialOperationState(hostWindow))
        {
            HideCustomPresentationForHost(hostWindow);
            return false;
        }

        // Single-operation normal mode owns all visible body content. Hide
        // only duplicate native visuals; their live elements and actions stay
        // intact and are restored for Explorer's fallback presentations.
        if (IsSingleNormalProgressTileForHost(state.tile, hostWindow))
        {
            SetNativeDuplicatePresentation(state, true);
        }

        bool geometryApplied =
            !applyFinalHostGeometry ||
            ResizeOperationStatusWindowForMode(
                hostWindow, state.expanded, transitionId);

        PositionInfoPanel(state.tile);
        PositionFooterOverlay(state.tile);
        ScheduleProgressCirclePosition(hostWindow, L"display-mode");

        HWND infoWindow = GetInfoPanelWindowForTile(state.tile);
        HWND circleWindow = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto circleIt = std::find_if(
                g_circles.begin(), g_circles.end(),
                [&state](CircleState const &candidate)
                { return candidate.tile == state.tile; });
            if (circleIt != g_circles.end())
            {
                circleWindow = circleIt->circleWindow;
            }
        }

        if (infoWindow && IsWindow(infoWindow))
        {
            InvalidateRect(infoWindow, nullptr, FALSE);
        }
        if (circleWindow && IsWindow(circleWindow))
        {
            InvalidateRect(circleWindow, nullptr, FALSE);
        }
        HWND footerWindow = GetFooterOverlayWindow(hostWindow);
        if (footerWindow && IsWindow(footerWindow))
        {
            InvalidateRect(footerWindow, nullptr, FALSE);
        }

        bool infoVisible = infoWindow && IsWindow(infoWindow) &&
                           IsWindowVisible(infoWindow);
        return geometryApplied && infoVisible;
    }


    void InitializeRegisteredDisplayMode(COperationStatusTile *owner)
    {
        TransferSummaryState state{};
        if (!CopyRegisteredTransferState(owner, &state) || !state.tile ||
            !state.operationTileRoot)
        {
            return;
        }

        NormalProgressLayoutElements elements{};
        bool completeNormalLayout =
            DiscoverNormalProgressLayout(state, &elements);
        bool deleteLike = state.deleteLikeKnown && state.deleteLike;

        bool hierarchyValid =
            deleteLike
                ? ValidateDeleteLikeProgressHierarchy(elements, 0, false)
                : (completeNormalLayout &&
                   ValidateNormalProgressHierarchy(elements, 0, false));
        if (!hierarchyValid)
        {
            Wh_Log(L"INITIAL_MODE owner=%p result=skipped "
                   L"reason=unsupported-layout deleteLike=%s",
                   reinterpret_cast<void *>(owner),
                   deleteLike ? L"yes" : L"no");
            return;
        }

        bool detailsVisible =
            elements.details &&
            Element_GetVisible_Original(elements.details);
        bool progressBarVisible =
            elements.progressBar &&
            Element_GetVisible_Original(elements.progressBar);
        bool chartVisible =
            (elements.chartArea &&
             Element_GetVisible_Original(elements.chartArea)) ||
            (elements.rateChart &&
             Element_GetVisible_Original(elements.rateChart));

        bool nativeCompact = false;
        bool nativeExpanded = false;

        if (deleteLike)
        {
            // Before custom visibility is applied, the native details/chart
            // state is the most stable expanded-mode signal for delete/recycle.
            nativeExpanded = detailsVisible || chartVisible;
            nativeCompact = !nativeExpanded;
        }
        else
        {
            // Proven copy/move signature from 0.10.81.
            nativeCompact = !detailsVisible && progressBarVisible;
            nativeExpanded = detailsVisible && !progressBarVisible;
        }

        if (!nativeCompact && !nativeExpanded)
        {
            Wh_Log(L"INITIAL_MODE owner=%p result=skipped "
                   L"reason=unrecognized-native-visibility deleteLike=%s",
                   reinterpret_cast<void *>(owner),
                   deleteLike ? L"yes" : L"no");
            return;
        }

        bool expanded = nativeExpanded;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [owner](TransferSummaryState const &candidate)
                { return candidate.owner == owner; });
            if (it == g_transferSummaries.end() || it->tile != state.tile ||
                it->operationTileRoot != state.operationTileRoot)
            {
                return;
            }
            it->displayModeKnown = true;
            it->expanded = expanded;
        }

        unsigned long long transitionId = ++g_displayTransitionSequence;
        ApplyDisplayMode(owner, false, transitionId);
        ScheduleDeferredDisplaySnapshot(owner, transitionId, expanded);
    }

    void RecordDisplayMode(COperationStatusTile *owner,
                           bool expanded,
                           unsigned long long transitionId)
    {
        bool recorded = false;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [owner](TransferSummaryState const &state)
                { return state.owner == owner; });
            if (it != g_transferSummaries.end() && it->tile &&
                it->operationTileRoot)
            {
                it->displayModeKnown = true;
                it->expanded = expanded;
                recorded = true;
            }
        }
        if (recorded)
        {
            ApplyDisplayMode(owner, false, transitionId);
        }
    }

    void RecordTransferBytes(COperationStatusTile *owner,
                             unsigned long long completedItems,
                             unsigned long long totalItems,
                             unsigned long long completedBytes,
                             unsigned long long totalBytes)
    {
        OperationTileElement *tile = nullptr;
        ULONGLONG now = GetTickCount64();

        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [owner](TransferSummaryState const &state)
                { return state.owner == owner; });
            if (it == g_transferSummaries.end())
            {
                return;
            }

            bool deleteLike = it->deleteLikeKnown && it->deleteLike;

            if (!it->measuredSampleInitialized)
            {
                it->measuredSampleInitialized = true;
                it->measuredSampleTick = now;
                it->measuredSampleCompletedBytes = completedBytes;
                it->measuredSampleCompletedItems = completedItems;
            }
            else
            {
                ULONGLONG elapsedMs = now - it->measuredSampleTick;
                bool countersMovedForward =
                    completedBytes >= it->measuredSampleCompletedBytes &&
                    completedItems >= it->measuredSampleCompletedItems;

                if (!countersMovedForward || elapsedMs > 10000)
                {
                    // New phase/restart or a very long idle period: reset the
                    // sample rather than dividing fresh work by stale idle time.
                    it->measuredSampleTick = now;
                    it->measuredSampleCompletedBytes = completedBytes;
                    it->measuredSampleCompletedItems = completedItems;
                }
                else if (elapsedMs >= 100)
                {
                    unsigned long long deltaItems =
                        completedItems -
                        it->measuredSampleCompletedItems;
                    unsigned long long deltaBytes =
                        completedBytes -
                        it->measuredSampleCompletedBytes;

                    double presentationDelta =
                        deleteLike
                            ? static_cast<double>(deltaItems)
                            : static_cast<double>(deltaBytes);

                    if (!deleteLike && presentationDelta <= 0.0 &&
                        it->resumedFromSpecialState &&
                        deltaItems > 0 && totalItems > 0 &&
                        totalBytes > 0)
                    {
                        // Conflict-resume fallback: preserve Copy/Move's
                        // bytes/sec presentation by converting item progress
                        // through the operation's average bytes per item.
                        presentationDelta =
                            static_cast<double>(deltaItems) *
                            (static_cast<double>(totalBytes) /
                             static_cast<double>(totalItems));
                    }

                    if (presentationDelta > 0.0)
                    {
                        double instantaneousRate =
                            presentationDelta * 1000.0 /
                            static_cast<double>(elapsedMs);

                        if (std::isfinite(instantaneousRate) &&
                            instantaneousRate > 0.0)
                        {
                            // Light smoothing keeps the display readable while
                            // still becoming available within the first useful
                            // progress callbacks.
                            if (it->measuredDisplayRateValid)
                            {
                                it->measuredDisplayRate =
                                    it->measuredDisplayRate * 0.65 +
                                    instantaneousRate * 0.35;
                            }
                            else
                            {
                                it->measuredDisplayRate = instantaneousRate;
                                it->measuredDisplayRateValid = true;
                            }

                            if (it->preferMeasuredRate ||
                                !it->nativeDisplayRateValid)
                            {
                                it->nativeRateHistory.push_back(
                                    it->measuredDisplayRate);
                                if (it->nativeRateHistory.size() >
                                    kInfoPanelRateHistorySamples)
                                {
                                    it->nativeRateHistory.erase(
                                        it->nativeRateHistory.begin(),
                                        it->nativeRateHistory.begin() +
                                            (it->nativeRateHistory.size() -
                                             kInfoPanelRateHistorySamples));
                                }
                            }
                        }
                    }

                    it->measuredSampleTick = now;
                    it->measuredSampleCompletedBytes = completedBytes;
                    it->measuredSampleCompletedItems = completedItems;
                }
            }

            it->completedItems = completedItems;
            it->totalItems = totalItems;
            it->itemsValid = true;
            it->completedBytes = completedBytes;
            it->totalBytes = totalBytes;
            it->bytesValid = true;
            tile = it->tile;
        }

        ApplyTransferSummary(owner);
        if (tile)
        {
            InvalidateInfoPanelForTile(tile);
        }
    }

    void RecordNativeSummary(COperationStatusTile *owner, PCWSTR summary)
    {
        (void)summary;
        ApplyTransferSummary(owner);
    }

    void RefreshTransferSummaryForTile(OperationTileElement *tile)
    {
        COperationStatusTile *owner = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [tile](TransferSummaryState const &state)
                { return state.tile == tile; });
            if (it != g_transferSummaries.end())
            {
                owner = it->owner;
            }
        }
        if (owner)
        {
            ApplyTransferSummary(owner);
        }
    }

    struct NativeProgressSnapshot
    {
        int position;
        int rangeLow;
        int rangeHigh;
        int percent;
        bool rangeValid;
    };

    NativeProgressSnapshot ReadNativeProgress(HWND progressWindow,
                                              int fallbackPercent)
    {
        NativeProgressSnapshot snapshot{};
        snapshot.percent = std::clamp(fallbackPercent, 0, 100);
        if (!progressWindow)
        {
            return snapshot;
        }

        snapshot.position = static_cast<int>(SendMessageW(
            progressWindow, PBM_GETPOS, 0, 0));
        PBRANGE range{};
        SendMessageW(progressWindow, PBM_GETRANGE, FALSE,
                     reinterpret_cast<LPARAM>(&range));
        snapshot.rangeLow = range.iLow;
        snapshot.rangeHigh = range.iHigh;
        snapshot.rangeValid = snapshot.rangeHigh > snapshot.rangeLow;
        if (snapshot.rangeValid)
        {
            long long numerator =
                (static_cast<long long>(snapshot.position) -
                 snapshot.rangeLow) *
                100;
            long long denominator =
                static_cast<long long>(snapshot.rangeHigh) -
                snapshot.rangeLow;
            snapshot.percent = static_cast<int>(std::clamp(
                numerator / denominator, 0LL, 100LL));
        }
        return snapshot;
    }

    bool EnsureProgressCircle(OperationTileElement *tile,
                              unsigned long long eventId,
                              NativeProgressSnapshot const &progress)
    {
        if (g_unloading.load(std::memory_order_acquire) ||
            !tile || !g_circleClassAtom || !g_infoPanelClassAtom)
        {
            return false;
        }

        if (g_unloading.load(std::memory_order_acquire))
        {
            return false;
        }

        HWND progressWindow =
            OperationTileElement_GetProgressHWND_Original(tile);
        HWND hostWindow =
            GetOperationStatusWindowForTile(progressWindow, eventId);
        if (!hostWindow)
        {
            return false;
        }

        bool circleExists = false;
        bool repaintNeeded = false;
        bool progressSubclassNeeded = false;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (it != g_circles.end())
            {
                circleExists = true;
                bool percentageChanged =
                    it->progressPercent != progress.percent;
                it->progressPercent = progress.percent;
                it->progressRangeLow = progress.rangeLow;
                it->progressRangeHigh = progress.rangeHigh;
                it->progressRangeInitialized = true;
                it->progressRangeValid = progress.rangeValid;
                if (!it->progressWindow && progressWindow)
                {
                    it->progressWindow = progressWindow;
                    progressSubclassNeeded = true;
                }
                if (!it->eventId)
                {
                    it->eventId = eventId;
                }
                repaintNeeded = percentageChanged;
            }
        }
        if (circleExists)
        {
            if (progressSubclassNeeded &&
                !SetWindowSubclass(
                    progressWindow, NativeProgressWindowSubclassProc,
                    kProgressWindowSubclassId, 0) &&
                eventId)
            {
                Wh_Log(L"eventId=%llu circle SetWindowSubclass progress failed "
                       L"error=%lu",
                       eventId, GetLastError());
            }
            if (repaintNeeded)
            {
                // Circle and body now share the same buffered presentation
                // HWND, so one invalidation updates the percentage, ring and
                // body coherently without sibling-window flicker.
                InvalidateInfoPanelForTile(tile);
            }
            return true;
        }

        HWND circleWindow = CreateWindowExW(
            WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_NOPARENTNOTIFY,
            kCircleWindowClass, nullptr, WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 1, 1,
            hostWindow, nullptr, g_circleClassInstance, nullptr);
        if (!circleWindow)
        {
            Wh_Log(L"eventId=%llu circle CreateWindowExW failed error=%lu",
                   eventId, GetLastError());
            return false;
        }

        HWND infoWindow = CreateWindowExW(
            WS_EX_NOACTIVATE | WS_EX_NOPARENTNOTIFY,
            kInfoPanelWindowClass, nullptr, WS_CHILD | WS_CLIPSIBLINGS,
            0, 0, 1, 1, hostWindow, nullptr, g_circleClassInstance, nullptr);
        if (!infoWindow)
        {
            Wh_Log(L"eventId=%llu info-panel CreateWindowExW failed error=%lu",
                   eventId, GetLastError());
            CircleState cleanupState{
                tile, circleWindow, nullptr, nullptr, hostWindow,
                progress.percent, progress.rangeLow, progress.rangeHigh,
                true, progress.rangeValid,
                false, false,
                eventId, 0, 0, 0, 0, false};
            if (!CleanupCircleStateResources(&cleanupState, true))
            {
                RetainCleanupOnlyCircleState(
                    cleanupState, L"information-panel-creation-failure");
            }
            return false;
        }

        CircleState newState{
            tile, circleWindow, infoWindow, progressWindow, hostWindow,
            progress.percent, progress.rangeLow, progress.rangeHigh,
            true, progress.rangeValid,
            false, false,
            eventId, 0, 0, 0, 0, false};

        if (!EnsureHostSubclass(hostWindow, eventId))
        {
            // The progress subclass is installed only after the required host
            // subclass succeeds, so this HWND has no callback to remove.
            newState.progressWindow = nullptr;
            if (!CleanupCircleStateResources(&newState, true))
            {
                RetainCleanupOnlyCircleState(
                    newState, L"host-subclass-installation-failure");
            }
            return false;
        }

        bool progressSubclassed = true;
        if (progressWindow)
        {
            progressSubclassed = SetWindowSubclass(
                progressWindow, NativeProgressWindowSubclassProc,
                kProgressWindowSubclassId, 0);
            if (!progressSubclassed)
            {
                Wh_Log(L"eventId=%llu circle SetWindowSubclass progress failed "
                       L"error=%lu",
                       eventId, GetLastError());
            }
        }

        bool shuttingDown = false;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            shuttingDown = g_unloading.load(std::memory_order_acquire);
            if (!shuttingDown)
            {
                g_circles.push_back(newState);
            }
        }

        if (shuttingDown)
        {
            if (!progressSubclassed)
            {
                newState.progressWindow = nullptr;
            }
            if (!CleanupCircleStateResources(&newState, true))
            {
                RetainCleanupOnlyCircleState(
                    newState, L"activation-shutdown-race");
            }
            return false;
        }

        ScheduleProgressCirclePosition(hostWindow, L"tile-added");
        InvalidateRect(circleWindow, nullptr, FALSE);
        InvalidateRect(infoWindow, nullptr, FALSE);
        return true;
    }

    void UpdateProgressCircle(OperationTileElement *tile)
    {
        if (g_unloading.load(std::memory_order_acquire))
        {
            return;
        }

        ScopedPresentationActivation activationGuard;
        if (g_unloading.load(std::memory_order_acquire))
        {
            return;
        }

        int fallbackPercent = 0;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (it != g_circles.end())
            {
                fallbackPercent = it->progressPercent;
            }
        }

        HWND progressWindow =
            OperationTileElement_GetProgressHWND_Original(tile);
        NativeProgressSnapshot progress =
            ReadNativeProgress(progressWindow, fallbackPercent);
        EnsureProgressCircle(tile, 0, progress);
        RefreshTransferSummaryForTile(tile);
        InvalidateInfoPanelForTile(tile);
        SyncHostCaptionFromCircle(tile, progress.percent);
    }

    void DestroyProgressCircle(OperationTileElement *tile)
    {
        CircleState state{};
        bool found = false;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (it != g_circles.end())
            {
                state = *it;
                found = true;
            }
        }
        if (!found)
        {
            return;
        }

        if (!CleanupCircleStateResources(&state, true))
        {
            RetainCleanupOnlyCircleState(state, L"tile-destruction");
            return;
        }

        EraseCircleStateRecord(state);
        if (state.hostWindow && IsWindow(state.hostWindow))
        {
            if (GetRegisteredTileCountForHost(state.hostWindow) == 0)
            {
                HWND footerWindow =
                    GetFooterOverlayWindow(state.hostWindow);
                if (footerWindow && IsWindow(footerWindow))
                {
                    DestroyWindow(footerWindow);
                }
            }
            else
            {
                ScheduleProgressCirclePosition(state.hostWindow,
                                               L"tile-removed");
            }
        }
    }

    void DestroyAllProgressCircles()
    {
        // Teardown is deliberately synchronous. Each host subclass receives
        // the registered teardown message on its owning UI thread and does
        // not acknowledge it until native visibility is restored, progress
        // subclasses are removed, custom children are destroyed, and the host
        // subclass itself is gone. Never time out into a DLL unload while a
        // callback into this module can still exist.
        std::vector<HWND> reportedHostFailures;
        auto reportHostFailure =
            [&reportedHostFailures](HWND hostWindow, PCWSTR reason)
        {
            if (std::find(reportedHostFailures.begin(),
                          reportedHostFailures.end(), hostWindow) !=
                reportedHostFailures.end())
            {
                return;
            }
            reportedHostFailures.push_back(hostWindow);
            Wh_Log(L"Presentation teardown blocked host=%p reason=%s",
                   reinterpret_cast<void *>(hostWindow), reason);
        };

        for (;;)
        {
            std::vector<HWND> hosts;
            {
                std::lock_guard<std::mutex> lock(g_circleMutex);
                hosts = g_subclassedHosts;
            }

            if (hosts.empty())
            {
                break;
            }

            bool madeProgress = false;
            for (HWND hostWindow : hosts)
            {
                if (!hostWindow || !IsWindow(hostWindow))
                {
                    if (RemoveDestroyedHostRecords(hostWindow))
                    {
                        madeProgress = true;
                    }
                    else
                    {
                        reportHostFailure(
                            hostWindow,
                            L"live child/progress state for destroyed host");
                    }
                    continue;
                }

                DWORD_PTR result = FALSE;
                if (SendMessageTimeoutW(
                        hostWindow, g_removeHostSubclassMessage, 0, 0,
                        SMTO_ABORTIFHUNG | SMTO_BLOCK, 1000, &result) &&
                    result == TRUE)
                {
                    madeProgress = true;
                }
                else
                {
                    reportHostFailure(hostWindow,
                                      L"owning thread did not acknowledge");
                }
            }

            if (!madeProgress)
            {
                // A retry is safer than allowing Windhawk to unload the DLL
                // with a live window procedure or subclass. The owning UI
                // thread remains the only thread that performs destruction.
                Sleep(10);
            }
        }

        for (;;)
        {
            CircleState state{};
            bool circlesRemain = false;
            {
                std::lock_guard<std::mutex> lock(g_circleMutex);
                circlesRemain = !g_circles.empty();
                if (!circlesRemain)
                {
                    g_hostPositionRequests.clear();
                }
                else
                {
                    state = g_circles.front();
                }
            }

            if (!circlesRemain)
            {
                break;
            }

            // Provisional activation and normal tile cleanup retain their
            // CircleState until every callback-bearing resource is verified
            // gone. Retry such cleanup on each HWND's owning thread.
            if (CleanupCircleStateResources(&state, true))
            {
                EraseCircleStateRecord(state);
            }
            else
            {
                RetainCleanupOnlyCircleState(
                    state, L"global-teardown-retry");
                Sleep(10);
            }
        }

        {
            std::lock_guard<std::mutex> lock(g_hostPresentationMutex);
            g_hostPresentationStates.clear();
        }
        {
            std::lock_guard<std::mutex> lock(g_hostNativeGeometryMutex);
            g_hostNativeGeometries.clear();
        }
    }

    bool InitializeProgressCircleUi()
    {
        Gdiplus::GdiplusStartupInput startupInput;
        if (Gdiplus::GdiplusStartup(&g_gdiplusToken, &startupInput, nullptr) !=
            Gdiplus::Ok)
        {
            Wh_Log(L"Circle setup failed: GdiplusStartup failed");
            return false;
        }

        g_removeHostSubclassMessage = RegisterWindowMessageW(
            L"Windhawk.FileOperationStyler.RemoveHostSubclass.0.12.1");
        if (!g_removeHostSubclassMessage)
        {
            Wh_Log(L"Circle setup failed: RegisterWindowMessageW error=%lu",
                   GetLastError());
            Gdiplus::GdiplusShutdown(g_gdiplusToken);
            g_gdiplusToken = 0;
            g_removeHostSubclassMessage = 0;
            g_positionCirclesMessage = 0;
            g_logDisplayStateMessage = 0;
            return false;
        }
        g_positionCirclesMessage = RegisterWindowMessageW(
            L"Windhawk.FileOperationStyler.PositionPresentations.0.12.1");
        if (!g_positionCirclesMessage)
        {
            Wh_Log(L"Circle setup failed: position RegisterWindowMessageW "
                   L"error=%lu",
                   GetLastError());
            Gdiplus::GdiplusShutdown(g_gdiplusToken);
            g_gdiplusToken = 0;
            g_removeHostSubclassMessage = 0;
            return false;
        }

        g_logDisplayStateMessage = RegisterWindowMessageW(
            L"Windhawk.FileOperationStyler.LogDisplayState.0.12.1");
        if (!g_logDisplayStateMessage)
        {
            Wh_Log(L"Diagnostic setup failed: display-state "
                   L"RegisterWindowMessageW error=%lu",
                   GetLastError());
            Gdiplus::GdiplusShutdown(g_gdiplusToken);
            g_gdiplusToken = 0;
            g_removeHostSubclassMessage = 0;
            g_positionCirclesMessage = 0;
            return false;
        }

        HMODULE circleModule = nullptr;
        if (!GetModuleHandleExW(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                reinterpret_cast<PCWSTR>(ProgressCircleWindowProc),
                &circleModule))
        {
            Wh_Log(L"Circle setup failed: GetModuleHandleExW error=%lu",
                   GetLastError());
            Gdiplus::GdiplusShutdown(g_gdiplusToken);
            g_gdiplusToken = 0;
            g_removeHostSubclassMessage = 0;
            g_positionCirclesMessage = 0;
            g_logDisplayStateMessage = 0;
            return false;
        }
        g_circleClassInstance = circleModule;
        WNDCLASSEXW windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = ProgressCircleWindowProc;
        windowClass.hInstance = g_circleClassInstance;
        windowClass.lpszClassName = kCircleWindowClass;
        g_circleClassAtom = RegisterClassExW(&windowClass);
        if (!g_circleClassAtom)
        {
            Wh_Log(L"Circle setup failed: RegisterClassExW error=%lu",
                   GetLastError());
            Gdiplus::GdiplusShutdown(g_gdiplusToken);
            g_gdiplusToken = 0;
            g_removeHostSubclassMessage = 0;
            g_positionCirclesMessage = 0;
            g_logDisplayStateMessage = 0;
            return false;
        }

        WNDCLASSEXW infoClass{};
        infoClass.cbSize = sizeof(infoClass);
        infoClass.style = CS_HREDRAW | CS_VREDRAW;
        infoClass.lpfnWndProc = InfoPanelWindowProc;
        infoClass.hInstance = g_circleClassInstance;
        infoClass.lpszClassName = kInfoPanelWindowClass;
        g_infoPanelClassAtom = RegisterClassExW(&infoClass);
        if (!g_infoPanelClassAtom)
        {
            Wh_Log(L"Info-panel setup failed: RegisterClassExW error=%lu",
                   GetLastError());
            UnregisterClassW(kCircleWindowClass, g_circleClassInstance);
            g_circleClassAtom = 0;
            Gdiplus::GdiplusShutdown(g_gdiplusToken);
            g_gdiplusToken = 0;
            g_removeHostSubclassMessage = 0;
            g_positionCirclesMessage = 0;
            g_logDisplayStateMessage = 0;
            return false;
        }

        WNDCLASSEXW footerClass{};
        footerClass.cbSize = sizeof(footerClass);
        footerClass.style = CS_HREDRAW | CS_VREDRAW;
        footerClass.lpfnWndProc = FooterOverlayWindowProc;
        footerClass.hInstance = g_circleClassInstance;
        footerClass.lpszClassName = kFooterOverlayWindowClass;
        g_footerOverlayClassAtom = RegisterClassExW(&footerClass);
        if (!g_footerOverlayClassAtom)
        {
            Wh_Log(L"Footer-overlay setup failed: RegisterClassExW error=%lu",
                   GetLastError());
            UnregisterClassW(kInfoPanelWindowClass,
                             g_circleClassInstance);
            g_infoPanelClassAtom = 0;
            UnregisterClassW(kCircleWindowClass,
                             g_circleClassInstance);
            g_circleClassAtom = 0;
            Gdiplus::GdiplusShutdown(g_gdiplusToken);
            g_gdiplusToken = 0;
            g_removeHostSubclassMessage = 0;
            g_positionCirclesMessage = 0;
            g_logDisplayStateMessage = 0;
            return false;
        }

        return true;
    }

    void ShutdownProgressCircleUi()
    {
        // Wh_ModBeforeUninit performs the owning-thread teardown while hooks
        // are still installed. This is an idempotent verification for init
        // failure and final resource shutdown paths.
        DestroyAllProgressCircles();

        if (g_footerOverlayClassAtom)
        {
            if (!UnregisterClassW(kFooterOverlayWindowClass,
                                  g_circleClassInstance))
            {
                Wh_Log(L"Footer-overlay cleanup: "
                       L"UnregisterClassW failed error=%lu",
                       GetLastError());
            }
            g_footerOverlayClassAtom = 0;
        }

        if (g_infoPanelClassAtom)
        {
            if (!UnregisterClassW(kInfoPanelWindowClass,
                                  g_circleClassInstance))
            {
                Wh_Log(L"Info-panel cleanup: UnregisterClassW failed error=%lu",
                       GetLastError());
            }
            g_infoPanelClassAtom = 0;
        }
        if (g_circleClassAtom)
        {
            if (!UnregisterClassW(kCircleWindowClass,
                                  g_circleClassInstance))
            {
                Wh_Log(L"Circle cleanup: UnregisterClassW failed error=%lu",
                       GetLastError());
            }
            g_circleClassAtom = 0;
        }
        if (g_gdiplusToken)
        {
            Gdiplus::GdiplusShutdown(g_gdiplusToken);
            g_gdiplusToken = 0;
        }
        g_removeHostSubclassMessage = 0;
        g_positionCirclesMessage = 0;
        g_logDisplayStateMessage = 0;
        {
            std::lock_guard<std::mutex> lock(g_displayDiagnosticMutex);
            g_deferredDisplaySnapshots.clear();
        }
    }

    void __cdecl OperationTileElement_OnPropertyChanged_Hook(
        OperationTileElement *thisPtr,
        DirectUI::PropertyInfo const *property,
        int propertyIndex,
        DirectUI::Value *oldValue,
        DirectUI::Value *newValue)
    {
        bool isProgressPosition =
            property &&
            property == OperationTileElement_ProgressPositionProp_Original();

        OperationTileElement_OnPropertyChanged_Original(
            thisPtr, property, propertyIndex, oldValue, newValue);

        if (g_unloading.load(std::memory_order_acquire) ||
            !isProgressPosition)
        {
            return;
        }

        HWND progressWindow =
            OperationTileElement_GetProgressHWND_Original(thisPtr);
        if (!progressWindow)
        {
            return;
        }

        UpdateProgressCircle(thisPtr);
    }

    HRESULT __cdecl COperationStatusTile_UpdateRemainingItemsAndSize_Hook(
        COperationStatusTile *thisPtr,
        unsigned long long completedItems,
        unsigned long long totalItems,
        unsigned long long completedBytes,
        unsigned long long totalBytes)
    {
        COperationStatusTile *previousRateOwnerHint =
            g_nativeRateOwnerHint;
        g_nativeRateOwnerHint = thisPtr;
        HRESULT result =
            COperationStatusTile_UpdateRemainingItemsAndSize_Original(
                thisPtr, completedItems, totalItems, completedBytes,
                totalBytes);
        g_nativeRateOwnerHint = previousRateOwnerHint;
        if (!g_unloading.load(std::memory_order_acquire) &&
            SUCCEEDED(result))
        {
            RecordTransferBytes(thisPtr, completedItems, totalItems,
                                completedBytes, totalBytes);
        }
        return result;
    }

    HRESULT __cdecl COperationStatusTile_UpdateSummary_Hook(
        COperationStatusTile *thisPtr,
        PCWSTR summary)
    {
        HRESULT result = COperationStatusTile_UpdateSummary_Original(
            thisPtr, summary);
        if (!g_unloading.load(std::memory_order_acquire) &&
            SUCCEEDED(result))
        {
            RecordNativeSummary(thisPtr, summary);
        }
        return result;
    }

    double __cdecl COperationStatusTileRateCalculator_CalculateRate_Hook(
        COperationStatusTileRateCalculator *thisPtr,
        unsigned long long value1,
        unsigned long long value2,
        unsigned long long value3,
        unsigned long long value4,
        unsigned long long value5,
        unsigned long long value6,
        unsigned long long value7,
        double *secondaryRate)
    {
        double result = COperationStatusTileRateCalculator_CalculateRate_Original(
            thisPtr, value1, value2, value3, value4, value5, value6,
            value7, secondaryRate);
        if (!g_unloading.load(std::memory_order_acquire) &&
            std::isfinite(result) && result >= 0.0 &&
            result <= static_cast<double>(
                          std::numeric_limits<LONGLONG>::max()))
        {
            if (!g_nativeRateOwnerHint ||
                !RecordNativeDisplayRateForOwner(
                    g_nativeRateOwnerHint, result))
            {
                RecordNativeDisplayRateForCurrentThread(result);
            }
        }
        return result;
    }

    HRESULT __cdecl COperationStatusTile_SetTileDisplayMode_Hook(
        COperationStatusTile *thisPtr,
        bool expanded)
    {
        if (g_unloading.load(std::memory_order_acquire))
        {
            return COperationStatusTile_SetTileDisplayMode_Original(
                thisPtr, expanded);
        }

        unsigned long long transitionId = ++g_displayTransitionSequence;

        COperationStatusTile *canonicalOwner = nullptr;
        bool ownerResolved = ResolveDisplayModeOwner(
            thisPtr, transitionId, &canonicalOwner, false);

        HRESULT result = COperationStatusTile_SetTileDisplayMode_Original(
            thisPtr, expanded);

        COperationStatusTile *postNativeOwner = nullptr;
        bool ownerStillResolved =
            ownerResolved &&
            ResolveDisplayModeOwner(thisPtr, transitionId,
                                    &postNativeOwner, false) &&
            postNativeOwner == canonicalOwner;

        if (SUCCEEDED(result) && ownerStillResolved)
        {
            RecordDisplayMode(canonicalOwner, expanded, transitionId);
            ScheduleDeferredDisplaySnapshot(canonicalOwner, transitionId,
                                            expanded);
        }
        else if (!ownerStillResolved)
        {
            Wh_Log(L"MODE[%llu] DEFERRED schedule-skipped "
                   L"reason=owner-resolution-failed",
                   transitionId);
        }
        return result;
    }

    void __cdecl OperationTileElement_Destructor_Hook(
        OperationTileElement *thisPtr)
    {
        CancelDeferredDisplaySnapshotsForTile(thisPtr);
        RemoveTransferSummary(thisPtr);
        DestroyProgressCircle(thisPtr);
        OperationTileElement_Destructor_Original(thisPtr);
    }

    struct WindowResizeResult
    {
        int beforeWidth;
        int beforeHeight;
        int afterWidth;
        int afterHeight;
        bool hostResized;
        bool success;
    };

    WindowResizeResult EnsureOperationStatusWindowWidth(
        unsigned long long eventId)
    {
        WindowResizeResult resizeResult{};
        WindowLookupContext context{eventId, nullptr};
        SetLastError(ERROR_SUCCESS);
        BOOL enumResult = EnumThreadWindows(
            GetCurrentThreadId(), FindOperationStatusWindow,
            reinterpret_cast<LPARAM>(&context));
        if (!context.operationStatusWindow)
        {
            DWORD error = enumResult ? ERROR_NOT_FOUND : GetLastError();
            if (error == ERROR_SUCCESS)
            {
                error = ERROR_NOT_FOUND;
            }
            Wh_Log(L"eventId=%llu base-layout OperationStatusWindow lookup "
                   L"failed error=%lu",
                   eventId, error);
            return resizeResult;
        }

        RECT windowRect;
        if (!GetWindowRect(context.operationStatusWindow, &windowRect))
        {
            Wh_Log(L"eventId=%llu base-layout GetWindowRect before failed "
                   L"error=%lu",
                   eventId, GetLastError());
            return resizeResult;
        }

        resizeResult.beforeWidth = windowRect.right - windowRect.left;
        resizeResult.beforeHeight = windowRect.bottom - windowRect.top;
        resizeResult.afterWidth = resizeResult.beforeWidth;
        resizeResult.afterHeight = resizeResult.beforeHeight;

        RECT clientRect;
        if (!GetClientRect(context.operationStatusWindow, &clientRect))
        {
            Wh_Log(L"eventId=%llu base-layout GetClientRect before failed "
                   L"error=%lu",
                   eventId, GetLastError());
            return resizeResult;
        }

        UINT dpi = GetDpiForWindow(context.operationStatusWindow);
        if (!dpi)
        {
            Wh_Log(L"eventId=%llu base-layout GetDpiForWindow failed dpi=0",
                   eventId);
            return resizeResult;
        }

        int targetClientWidth =
            MulDiv(kRequestedTileWidth, static_cast<int>(dpi),
                   USER_DEFAULT_SCREEN_DPI);
        int currentClientWidth = clientRect.right - clientRect.left;
        if (currentClientWidth >= targetClientWidth)
        {
            resizeResult.success = true;
            return resizeResult;
        }

        int nonClientWidth = resizeResult.beforeWidth - currentClientWidth;
        int targetWindowWidth = targetClientWidth + nonClientWidth;
        BOOL resized = FALSE;
        {
            ScopedHostGeometryChange geometryChange(
                context.operationStatusWindow, false);
            resized = SetWindowPos(
                context.operationStatusWindow, nullptr, 0, 0,
                targetWindowWidth, resizeResult.beforeHeight,
                SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
        if (!resized)
        {
            Wh_Log(L"eventId=%llu base-layout SetWindowPos failed "
                   L"requestedWidth=%d error=%lu",
                   eventId, targetWindowWidth, GetLastError());
            return resizeResult;
        }

        resizeResult.hostResized = true;
        if (!GetWindowRect(context.operationStatusWindow, &windowRect))
        {
            Wh_Log(L"eventId=%llu base-layout GetWindowRect after failed "
                   L"error=%lu",
                   eventId, GetLastError());
            return resizeResult;
        }

        resizeResult.afterWidth = windowRect.right - windowRect.left;
        resizeResult.afterHeight = windowRect.bottom - windowRect.top;
        if (!GetClientRect(context.operationStatusWindow, &clientRect))
        {
            Wh_Log(L"eventId=%llu base-layout GetClientRect after failed "
                   L"error=%lu",
                   eventId, GetLastError());
            return resizeResult;
        }

        resizeResult.success =
            clientRect.right - clientRect.left >= targetClientWidth &&
            resizeResult.afterHeight == resizeResult.beforeHeight;
        return resizeResult;
    }

    // Exact verified x64 ABI encoded by the Microsoft public symbol:
    // ?_CreateTileElement@COperationStatusTile@@AEAAJKKPEAVElement@DirectUI@@@Z
    // The raw detour receives the implicit COperationStatusTile "this" pointer
    // first, followed by the two 32-bit unsigned-long arguments and Element*.
    using COperationStatusTile_CreateTileElement_t = HRESULT(__cdecl *)(
        COperationStatusTile *thisPtr,
        unsigned long arg1,
        unsigned long arg2,
        DirectUI::Element *parentElement);
    COperationStatusTile_CreateTileElement_t
        COperationStatusTile_CreateTileElement_Original;

    HRESULT __cdecl COperationStatusTile_CreateTileElement_Hook(
        COperationStatusTile *thisPtr,
        unsigned long arg1,
        unsigned long arg2,
        DirectUI::Element *parentElement)
    {
        if (g_unloading.load(std::memory_order_acquire))
        {
            return COperationStatusTile_CreateTileElement_Original(
                thisPtr, arg1, arg2, parentElement);
        }

        ScopedTileSkin skinScope;
        if (!skinScope.OwnsSkin())
        {
            return COperationStatusTile_CreateTileElement_Original(
                thisPtr, arg1, arg2, parentElement);
        }

        unsigned long long eventId = ++g_skinEventSequence;
        HRESULT result = COperationStatusTile_CreateTileElement_Original(
            thisPtr, arg1, arg2, parentElement);

        auto &state = g_skinState;
        state.collecting = false;

        if (g_unloading.load(std::memory_order_acquire))
        {
            return result;
        }

        if (FAILED(result))
        {
            Wh_Log(L"eventId=%llu presentation skipped "
                   L"reason=CreateTileElement-failed result=0x%08X",
                   eventId, static_cast<unsigned int>(result));
            return result;
        }

        DirectUI::Element *operationTileRoot = state.operationTileRoot;
        if (!operationTileRoot)
        {
            Wh_Log(L"eventId=%llu presentation skipped "
                   L"reason=idOperationTile-root-not-found",
                   eventId);
            return result;
        }

        auto *operationTile =
            reinterpret_cast<OperationTileElement *>(operationTileRoot);
        HWND nativeProgressWindow =
            OperationTileElement_GetProgressHWND_Original(operationTile);
        NativeProgressSnapshot initialProgress =
            ReadNativeProgress(nativeProgressWindow, 0);

        // Only the top-level HWND is sized. Native DirectUI geometry, fonts,
        // colors, and parentage are never modified; duplicate normal-mode
        // visuals are hidden later and restored for native fallback.
        ScopedPresentationActivation activationGuard;
        if (g_unloading.load(std::memory_order_acquire))
        {
            return result;
        }
        if (!EnsureProgressCircle(operationTile, eventId, initialProgress))
        {
            if (!g_unloading.load(std::memory_order_acquire))
            {
                Wh_Log(L"eventId=%llu presentation activation failed; "
                       L"native Explorer presentation retained",
                       eventId);
            }
            return result;
        }
        EnsureOperationStatusWindowWidth(eventId);
        RegisterTransferSummary(thisPtr, operationTile, operationTileRoot,
                                state.tileHeaderRoot);
        return result;
    }

    struct SkinTargets
    {
        DUIXmlParser_CreateElement_t parserCreate;
        StrToID_t strToID;
        Element_FindDescendent_t findDescendent;
        Element_GetParent_t getParent;
        Value_Release_t releaseValue;
        Element_GetVisible_t getVisible;
        Element_SetVisible_t setVisible;
        COperationStatusTile_CreateTileElement_t createTileElement;
        OperationTileElement_ProgressPositionProp_t progressPositionProp;
        OperationTileElement_GetProgressHWND_t getProgressHWND;
        OperationTileElement_OnPropertyChanged_t onPropertyChanged;
        OperationTileElement_Destructor_t operationTileDestructor;
        COperationStatusTile_UpdateRemainingItemsAndSize_t
            updateRemainingItemsAndSize;
        COperationStatusTile_UpdateSummary_t updateSummary;
        COperationStatusTile_SetTileDisplayMode_t setTileDisplayMode;
        COperationStatusTileRateCalculator_CalculateRate_t calculateRate;
    };

    bool ResolveSkinTargets(SkinTargets *targets)
    {
        HMODULE dui70 =
            LoadLibraryExW(L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!dui70)
        {
            Wh_Log(L"Skin setup failed: unable to load dui70.dll "
                   L"error=%lu",
                   GetLastError());
            return false;
        }

        constexpr PCSTR parserCreateSymbol =
            "?CreateElement@DUIXmlParser@DirectUI@@QEAAJPEBGPEAVElement@2@1PEAKPEAPEAV32@@Z";
        constexpr PCSTR findDescendentSymbol =
            "?FindDescendent@Element@DirectUI@@QEAAPEAV12@G@Z";
        constexpr PCSTR getParentSymbol =
            "?GetParent@Element@DirectUI@@QEAAPEAV12@XZ";
        constexpr PCSTR releaseValueSymbol =
            "?Release@Value@DirectUI@@QEAAXXZ";
        constexpr PCSTR getVisibleSymbol =
            "?GetVisible@Element@DirectUI@@QEAA_NXZ";
        constexpr PCSTR setVisibleSymbol =
            "?SetVisible@Element@DirectUI@@QEAAJ_N@Z";

        targets->parserCreate =
            reinterpret_cast<DUIXmlParser_CreateElement_t>(
                GetProcAddress(dui70, parserCreateSymbol));
        targets->strToID = reinterpret_cast<StrToID_t>(
            GetProcAddress(dui70, "StrToID"));
        targets->findDescendent =
            reinterpret_cast<Element_FindDescendent_t>(
                GetProcAddress(dui70, findDescendentSymbol));
        targets->getParent = reinterpret_cast<Element_GetParent_t>(
            GetProcAddress(dui70, getParentSymbol));
        targets->releaseValue = reinterpret_cast<Value_Release_t>(
            GetProcAddress(dui70, releaseValueSymbol));
        targets->getVisible = reinterpret_cast<Element_GetVisible_t>(
            GetProcAddress(dui70, getVisibleSymbol));
        targets->setVisible = reinterpret_cast<Element_SetVisible_t>(
            GetProcAddress(dui70, setVisibleSymbol));
        Element_GetContentString_Optional =
            reinterpret_cast<Element_GetContentString_t>(
                GetProcAddress(
                    dui70,
                    "?GetContentString@Element@DirectUI@@QEAAPEBGPEAPEAVValue@2@@Z"));
        // 0.12 requires read-only discovery/state exports plus SetVisible for
        // reversible suppression of duplicate normal-mode visuals.
        if (!targets->parserCreate || !targets->strToID ||
            !targets->findDescendent || !targets->getParent ||
            !targets->releaseValue || !targets->getVisible ||
            !targets->setVisible)
        {
            Wh_Log(L"Portable presentation setup failed: required dui70 "
                   L"exports missing "
                   L"ParserCreate=%p StrToID=%p FindDescendent=%p "
                   L"GetParent=%p ReleaseValue=%p GetVisible=%p "
                   L"SetVisible=%p",
                   reinterpret_cast<void *>(targets->parserCreate),
                   reinterpret_cast<void *>(targets->strToID),
                   reinterpret_cast<void *>(targets->findDescendent),
                   reinterpret_cast<void *>(targets->getParent),
                   reinterpret_cast<void *>(targets->releaseValue),
                   reinterpret_cast<void *>(targets->getVisible),
                   reinterpret_cast<void *>(targets->setVisible));
            return false;
        }

        HMODULE shell32 =
            LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!shell32)
        {
            Wh_Log(L"Skin setup failed: unable to load shell32.dll "
                   L"error=%lu",
                   GetLastError());
            return false;
        }

        WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
            {
                {LR"(?_CreateTileElement@COperationStatusTile@@AEAAJKKPEAVElement@DirectUI@@@Z)"},
                &targets->createTileElement,
                nullptr,
                false,
            },
            {
                {LR"(?ProgressPositionProp@OperationTileElement@@SAPEBUPropertyInfo@DirectUI@@XZ)"},
                &targets->progressPositionProp,
                nullptr,
                false,
            },
            {
                {LR"(?_GetProgressHWND@OperationTileElement@@AEAAPEAUHWND__@@XZ)"},
                &targets->getProgressHWND,
                nullptr,
                false,
            },
            {
                {LR"(?OnPropertyChanged@OperationTileElement@@UEAAXPEBUPropertyInfo@DirectUI@@HPEAVValue@3@1@Z)"},
                &targets->onPropertyChanged,
                nullptr,
                false,
            },
            {
                {LR"(??1OperationTileElement@@UEAA@XZ)"},
                &targets->operationTileDestructor,
                nullptr,
                false,
            },
            {
                {LR"(?_UpdateRemainingItemsAndSize@COperationStatusTile@@AEAAJ_K000@Z)"},
                &targets->updateRemainingItemsAndSize,
                nullptr,
                false,
            },
            {
                {LR"(?_UpdateSummary@COperationStatusTile@@AEAAJPEBG@Z)"},
                &targets->updateSummary,
                nullptr,
                false,
            },
            {
                {LR"(?SetTileDisplayMode@COperationStatusTile@@UEAAJ_N@Z)"},
                &targets->setTileDisplayMode,
                nullptr,
                false,
            },
            {
                {LR"(?_CalculateRate@COperationStatusTileRateCalculator@@AEAAN_K000000PEAN@Z)"},
                &targets->calculateRate,
                nullptr,
                false,
            },
        };

        WH_HOOK_SYMBOLS_OPTIONS options{};
        options.optionsSize = sizeof(options);
        options.noUndecoratedSymbols = TRUE;

        if (!WindhawkUtils::HookSymbols(shell32, shell32DllHooks,
                                        ARRAYSIZE(shell32DllHooks), &options) ||
            !targets->createTileElement || !targets->progressPositionProp ||
            !targets->getProgressHWND || !targets->onPropertyChanged ||
            !targets->operationTileDestructor ||
            !targets->updateRemainingItemsAndSize || !targets->updateSummary ||
            !targets->setTileDisplayMode || !targets->calculateRate)
        {
            Wh_Log(L"Skin setup failed: unable to resolve exact "
                   L"file-operation symbols CreateTile=%p ProgressProp=%p "
                   L"GetProgressHWND=%p OnPropertyChanged=%p Destructor=%p",
                   reinterpret_cast<void *>(targets->createTileElement),
                   reinterpret_cast<void *>(targets->progressPositionProp),
                   reinterpret_cast<void *>(targets->getProgressHWND),
                   reinterpret_cast<void *>(targets->onPropertyChanged),
                   reinterpret_cast<void *>(targets->operationTileDestructor));
            return false;
        }

        return true;
    }

    bool InstallSkinHooks(SkinTargets const &targets)
    {
        StrToID_Original = targets.strToID;
        Element_FindDescendent_Original = targets.findDescendent;
        Element_GetParent_Original = targets.getParent;
        Value_Release_Original = targets.releaseValue;
        Element_GetVisible_Original = targets.getVisible;
        Element_SetVisible_Original = targets.setVisible;
        OperationTileElement_ProgressPositionProp_Original =
            targets.progressPositionProp;
        OperationTileElement_GetProgressHWND_Original =
            targets.getProgressHWND;

        if (!WindhawkUtils::SetFunctionHook(
                targets.parserCreate, DUIXmlParser_CreateElement_Hook,
                &DUIXmlParser_CreateElement_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook "
                   L"dui70!DirectUI::DUIXmlParser::CreateElement");
            return false;
        }

        if (!WindhawkUtils::SetFunctionHook(
                targets.createTileElement,
                COperationStatusTile_CreateTileElement_Hook,
                &COperationStatusTile_CreateTileElement_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook "
                   L"shell32!COperationStatusTile::_CreateTileElement");
            return false;
        }

        if (!WindhawkUtils::SetFunctionHook(
                targets.onPropertyChanged,
                OperationTileElement_OnPropertyChanged_Hook,
                &OperationTileElement_OnPropertyChanged_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook "
                   L"shell32!OperationTileElement::OnPropertyChanged");
            return false;
        }

        if (!WindhawkUtils::SetFunctionHook(
                targets.operationTileDestructor,
                OperationTileElement_Destructor_Hook,
                &OperationTileElement_Destructor_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook "
                   L"shell32!OperationTileElement destructor");
            return false;
        }

        if (!WindhawkUtils::SetFunctionHook(
                targets.updateRemainingItemsAndSize,
                COperationStatusTile_UpdateRemainingItemsAndSize_Hook,
                &COperationStatusTile_UpdateRemainingItemsAndSize_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook native byte update");
            return false;
        }

        if (!WindhawkUtils::SetFunctionHook(
                targets.updateSummary,
                COperationStatusTile_UpdateSummary_Hook,
                &COperationStatusTile_UpdateSummary_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook native summary update");
            return false;
        }

        if (!WindhawkUtils::SetFunctionHook(
                targets.setTileDisplayMode,
                COperationStatusTile_SetTileDisplayMode_Hook,
                &COperationStatusTile_SetTileDisplayMode_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook native display mode");
            return false;
        }

        if (!WindhawkUtils::SetFunctionHook(
                targets.calculateRate,
                COperationStatusTileRateCalculator_CalculateRate_Hook,
                &COperationStatusTileRateCalculator_CalculateRate_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook native rate update");
            return false;
        }

        return true;
    }

} // namespace

BOOL Wh_ModInit()
{
    g_unloading.store(false, std::memory_order_release);
    Wh_Log(L"File Operation Styler 0.12.11 initialization started");

    LoadSettings();

    if (!InitializeProgressCircleUi())
    {
        return FALSE;
    }

    SkinTargets targets{};
    if (!ResolveSkinTargets(&targets) || !InstallSkinHooks(targets))
    {
        ShutdownProgressCircleUi();
        return FALSE;
    }

    Wh_Log(L"File Operation Styler 0.12.11 initialization complete");
    return TRUE;
}

void Wh_ModBeforeUninit()
{
    if (g_unloading.exchange(true, std::memory_order_acq_rel))
    {
        return;
    }

    Wh_Log(L"File Operation Styler 0.12.11 presentation teardown started");
    while (g_presentationActivations.load(std::memory_order_acquire) != 0)
    {
        Sleep(1);
    }
    DestroyAllProgressCircles();
    Wh_Log(L"File Operation Styler 0.12.11 presentation teardown complete");
}

void Wh_ModUninit()
{
    ShutdownProgressCircleUi();
    {
        std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
        g_transferSummaries.clear();
    }
    ClearSkinState();
    ShutdownDwmApi();
    Wh_Log(L"File Operation Styler 0.12.11 uninitialization complete");
}


BOOL Wh_ModSettingsChanged(BOOL *bReload)
{
    if (bReload)
    {
        *bReload = TRUE;
    }
    return TRUE;
}
