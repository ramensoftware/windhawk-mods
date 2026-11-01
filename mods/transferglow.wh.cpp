// ==WindhawkMod==
// @id              transferglow
// @name            TransferGlow
// @description     Custom appearance window with circular color wheel and effect intensity control; Windows 11 26H2 compatibility candidate.
// @version         1.5.0
// @author          digART
// @github          https://github.com/miguelelesbaocampos-cmyk
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32 -lgdiplus -lshlwapi -ladvapi32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

# File Operation Styler

A modern replacement for the standard Windows 11 file operation window.

File Operation Styler gives copy, move, delete, and recycle operations a cleaner modern layout while keeping the normal Windows file operation behavior.

![File Operation Styler](https://raw.githubusercontent.com/digart11/File-Operation-Styler/master/images/file-operation-styler.png)

### Interactive Controls

![File Operation Styler interactive controls](https://raw.githubusercontent.com/digart11/File-Operation-Styler/master/images/file-operation-styler-controls.png)

The large progress circle can pause and resume operations, while optional settings can hide the smaller controls and title-bar percentage for a cleaner layout.

### Default vs File Operation Styler

![Default vs File Operation Styler](https://raw.githubusercontent.com/digart11/File-Operation-Styler/master/images/file-operation-styler-compare.png)

### Themes

![File Operation Styler Themes](https://raw.githubusercontent.com/digart11/File-Operation-Styler/master/images/file-operation-styler-themes-1.2.png)

### Extra palette gallery (1.5.0)

| Theme | Background | Accent | Graph line / fill | Action surface |
|---|---|---|---|---|
| Ocean | `#0B1E2B` | `#2EC5D3` | `#62E0E6` / `#1E9DB2` | `#11303E` |
| Forest | `#14271E` | `#70D18A` | `#9DE6A6` / `#42AF67` | `#1C3827` |
| Aurora | `#171C35` | `#70E1D0` | `#A78BFA` / `#4FCABF` | `#252D4C` |
| Sakura | `#382632` | `#F28BAA` | `#FFADC5` / `#DF7192` | `#4A303D` |
| Sunset | `#302126` | `#FF9B54` | `#FFC46B` / `#EC734F` | `#472D2C` |
| Carbon | `#1B1D20` | `#8EA8C3` | `#B8C5D3` / `#71869B` | `#25292D` |
| Arctic | `#EDF5FA` | `#168AB8` | `#087FAB` / `#66BBDD` | `#D9E8F1` |
| Nebula | `#231A38` | `#CE70F5` | `#F080C2` / `#8B67E8` | `#332449` |

These palettes also define their text, inactive progress, graph surface and grid, and action borders independently.

## What's new in 1.5.0

- Added Ocean, Forest, Aurora, Sakura, Sunset, Carbon, Arctic, and Nebula as complete selectable themes.
- Added the new theme palette gallery above; all previous themes remain available.

## What's new in 1.4.1

- Buffered the custom appearance and color-wheel windows to avoid partial frames during repaint.
- Limited Explorer operation-window refreshes while dragging controls and kept a final refresh when the drag ends.
- Hardened appearance-window teardown and creation-failure cleanup.

## What's new in 1.4.0

- Added a separate appearance window with a custom circular hue wheel and saturation/value surface.
- Added a saved 0–100% slider that updates Acrylic, Custom Blur, and Mica tint in the current operation windows.
- Added the Ctrl + Alt + Shift + F12 shortcut to open the window from Explorer.
- Kept the operation hooks, themes, visual settings, and Windows 11 26H2 compatibility checks.

## What's new in 1.3.0

- Added Emerald, Violet, Nord, and Rose themes while retaining all previous themes.
- Added Mica and Solid presentation effects alongside Acrylic and Custom Blur.
- Made glass tint color and opacity apply to Acrylic, Blur, and Mica.
- Kept the Windows 11 26H2-era symbol and export validation and native fallback behavior.

## What's new in 1.2.2 (26H2 compatibility candidate)

- Made the private `COperationStatusTileRateCalculator::_CalculateRate` symbol optional.
- The visual skin can initialize when that private helper is absent from the shell32 PDB.
- If unavailable, native speed and remaining-time sampling are omitted; the UI uses its safe fallback.

## What's new in 1.2.1 (26H2 compatibility candidate)

- Added runtime Windows build diagnostics using RtlGetVersion.
- Added detailed shell32/DirectUI symbol-resolution diagnostics to identify why unsupported Explorer builds fall back or fail initialization.
- Preserved the original operation engine and native fallback behavior.
- This build is a compatibility candidate for Windows 11 26H2 (build 26300); it has not been runtime-tested on that exact build.

## What's new in 1.2.0

- Added a Frosted Glass theme with Windows Acrylic and Custom Blur styles.
- Added current-file progress while the circular indicator continues to show overall operation progress.
- Added interactive Pause / Resume behavior to the large progress circle.
- Added an option to hide the small Pause / Resume and Cancel buttons in the upper-right corner. The large progress-circle control and bottom Cancel button remain available.
- Added an option to hide the percentage from the file-operation window title.
- Improved multiple-operation support, including synchronized More / Fewer Details behavior.
- Improved fallback to the native Windows UI for conflicts, errors, and unsupported presentation states.
- Improved restoration and teardown when the mod is disabled, reloaded, or settings are changed.

## Features

- Modern copy, move, delete, and recycle operation window
- Circular overall progress indicator
- Current-file progress bar
- Transferred size, remaining items, speed, and estimated time
- Progress graph in More Details view
- Multiple file operations in the same window
- Pause, resume, and cancel controls
- Interactive Pause / Resume control in the large progress circle
- Option to hide the small Pause / Resume and Cancel buttons
- Option to hide the percentage from the window title
- Works with normal Windows conflict and error dialogs
- Built-in Blue Dark, Graphite, Midnight, Warm Dark, Light, System, Emerald, Violet, Nord, Rose, Ocean, Forest, Aurora, Sakura, Sunset, Carbon, Arctic, Nebula, and Frosted Glass themes
- Acrylic, Custom Blur, Mica, and Solid presentation effects for Frosted Glass
- Adjustable glass tint opacity and custom tint color
- One shared theme and effect color picker, adjustable effect opacity, fonts, text sizes, and progress thickness

## Customization

Turn on **Enable customization**, then choose a theme and configure fonts, text sizes, and progress styling in the Windhawk settings. To open the compact TransferGlow appearance window, focus any Explorer window and press **Ctrl + Alt + Shift + F12**. The window shows one horizontal 0–100% slider and one circular color preview. Click the preview to open the custom hue wheel and saturation/value surface. Color and intensity update the current operation windows and are saved automatically. The intensity slider affects Acrylic, Custom Blur, and Mica; Solid remains opaque. **Restore defaults** returns to the theme accent and 0% tint.

## Notes

File Operation Styler changes the appearance of the normal Windows file operation window only. Windows continues to handle the actual copy, move, delete, conflicts, errors, pause, resume, and cancel operations.

Theme, font, and layout setting changes apply after Windhawk reloads the mod. The appearance window's color and intensity changes repaint operation windows that are already open.

The supplied upstream source was tested on Windows 11 24H2 x64. This 1.2.1 compatibility candidate adds diagnostics for Windows 11 26H2 (including build 26300), but has not been runtime-tested on that build. The mod relies on private Explorer and Shell presentation interfaces; symbol names, function layouts, and DirectUI exports can change between Windows builds. If required interfaces cannot be resolved, initialization is stopped rather than applying guessed addresses, and Windows' native file-operation behavior remains the intended fallback.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showCurrentFileProgressBar: true
  $name: Show current-file progress bar
  $description: Show progress for the file currently being copied or moved.

- hideSmallControlButtons: false
  $name: Hide small control buttons
  $description: Hide the small Pause / Resume and Cancel controls in the upper-right corner. The progress circle and bottom Cancel button remain available.

- hideTitleBarPercentage: false
  $name: Hide title-bar percentage
  $description: Hide the normal progress percentage from the visible window title bar.


- customization:
  - enabled: false
    $name: Enable customization
    $description: Turn on themes and custom style settings. Focus an Explorer window and press Ctrl + Alt + Shift + F12 to open the color and effect controls.
  - preset: blueDark
    $name: Theme
    $description: Change color theme.
    $options:
    - blueDark: Blue Dark
    - graphite: Graphite
    - midnight: Midnight
    - warmDark: Warm Dark
    - light: Light
    - system: Windows / System
    - emerald: Emerald
    - violet: Violet
    - nord: Nord
    - rose: Rose
    - ocean: Ocean
    - forest: Forest
    - aurora: Aurora
    - sakura: Sakura
    - sunset: Sunset
    - carbon: Carbon
    - arctic: Arctic
    - nebula: Nebula
    - glass: Frosted Glass

  - glassEffect: acrylic
    $name: Frosted Glass effect
    $description: Choose the backdrop rendered behind the operation window.
    $options:
    - acrylic: Windows Acrylic
    - blur: Custom Blur
    - mica: Mica
    - solid: Solid
    #! $showIf: {preset: glass}

  - footerStyle: glass
    $name: Footer
    $description: Frosted Glass footer is available only when the Frosted Glass theme is selected.
    $options:
    - glass: Frosted Glass
    - solid: Solid
    #! $showIf: {preset: glass}
  - footerColorPreset: theme
    $name: Solid footer color preset
    $description: The Custom option uses the color chosen in the TransferGlow appearance window.
    $options:
    - theme: Theme default
    - blueDark: Blue Dark
    - graphite: Graphite
    - midnight: Midnight
    - warmDark: Warm Dark
    - light: Light
    - system: Windows / System
    - emerald: Emerald
    - violet: Violet
    - nord: Nord
    - rose: Rose
    - ocean: Ocean
    - forest: Forest
    - aurora: Aurora
    - sakura: Sakura
    - sunset: Sunset
    - carbon: Carbon
    - arctic: Arctic
    - nebula: Nebula
    - custom: Custom
    #! $showIf: {preset: glass, footerStyle: solid}

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
      $name: Summary text size
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
#include <dwmapi.h>
#include <gdiplus.h>
#include <shlwapi.h>
#include <windows.h>
#include <windowsx.h>

#include <uxtheme.h>
#include <shobjidl.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <condition_variable>
#include <cwchar>
#include <limits>
#include <mutex>
#include <process.h>
#include <string>
#include <vector>

struct COperationDataProvider;
struct COperationStatusTile;
struct COperationStatusTileRateCalculator;
struct OperationTileElement;

namespace DirectUI
{
    struct DUIXmlParser;
    struct Element;
    struct IClassInfo;
    struct PropertyInfo;
    struct Value;
} // namespace DirectUI

namespace
{
    HMODULE g_ownedOle32Module = nullptr;

    void FreeShellMemory(void *memory)
    {
        if (!memory)
            return;

        using CoTaskMemFree_t = void(WINAPI *)(LPVOID);

        static CoTaskMemFree_t freeFn = []() -> CoTaskMemFree_t
        {
            HMODULE ole32 = GetModuleHandleW(L"ole32.dll");
            if (!ole32)
            {
                ole32 = LoadLibraryExW(
                    L"ole32.dll",
                    nullptr,
                    LOAD_LIBRARY_SEARCH_SYSTEM32);

                if (ole32)
                {
                    g_ownedOle32Module = ole32;
                }
            }

            return ole32
                       ? reinterpret_cast<CoTaskMemFree_t>(
                             GetProcAddress(ole32, "CoTaskMemFree"))
                       : nullptr;
        }();

        if (freeFn)
            freeFn(memory);
    }

    void ShutdownShellMemoryApi()
    {
        if (g_ownedOle32Module)
        {
            FreeLibrary(g_ownedOle32Module);
            g_ownedOle32Module = nullptr;
        }
    }

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
    std::mutex g_presentationActivationMutex;
    std::condition_variable g_presentationActivationCondition;
    unsigned int g_presentationActivations{};
    std::atomic<unsigned long long> g_skinEventSequence{};
    std::atomic<unsigned long long> g_displayTransitionSequence{};

    class ScopedPresentationActivation
    {
    public:
        ScopedPresentationActivation()
        {
            std::lock_guard<std::mutex> lock(g_presentationActivationMutex);
            if (!g_unloading.load(std::memory_order_acquire))
            {
                ++g_presentationActivations;
                active_ = true;
            }
        }

        ~ScopedPresentationActivation()
        {
            if (!active_)
            {
                return;
            }

            std::lock_guard<std::mutex> lock(g_presentationActivationMutex);
            if (--g_presentationActivations == 0)
            {
                g_presentationActivationCondition.notify_all();
            }
        }

        ScopedPresentationActivation(
            ScopedPresentationActivation const &) = delete;
        ScopedPresentationActivation &operator=(
            ScopedPresentationActivation const &) = delete;

    private:
        bool active_ = false;
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

    using COperationDataProvider_WriteCurrentItem_t = HRESULT(__cdecl *)(
        COperationDataProvider *thisPtr,
        IShellItem *currentItem);
    COperationDataProvider_WriteCurrentItem_t
        COperationDataProvider_WriteCurrentItem_Original;

    using COperationDataProvider_WriteProgressValues_t = HRESULT(__cdecl *)(
        COperationDataProvider *thisPtr,
        unsigned long long value0,
        unsigned long long value1,
        unsigned long long value2,
        unsigned long long value3,
        unsigned long long value4,
        unsigned long long value5);
    COperationDataProvider_WriteProgressValues_t
        COperationDataProvider_WriteProgressValues_Original;

    using COperationStatusTile_RefreshDisplayProgress_t = void(__cdecl *)(
        COperationStatusTile *thisPtr,
        unsigned long long value0,
        unsigned long long value1);
    COperationStatusTile_RefreshDisplayProgress_t
        COperationStatusTile_RefreshDisplayProgress_Original;

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
        unsigned long long currentFileSize = 0;
        unsigned long long currentFileStartBytes = 0;
        unsigned long long currentFileCompletedBytes = 0;
        int currentFilePercent = 0;
        bool currentFileProgressValid = false;
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
        std::vector<DirectUI::Element *> nativeElementsHiddenByMod;
    };

    std::mutex g_transferSummaryMutex;
    std::vector<TransferSummaryState> g_transferSummaries;

    struct OperationDataBinding
    {
        COperationDataProvider *writer = nullptr;

        unsigned long long latestCompletedItems = 0;
        unsigned long long latestTotalItems = 0;
        unsigned long long latestCompletedBytes = 0;
        unsigned long long latestTotalBytes = 0;
        bool latestProgressValid = false;

        unsigned long long currentFileSize = 0;
        unsigned long long currentFileStartBytes = 0;
        bool currentFileValid = false;
        int sharedSlot = -1;
    };

    std::vector<OperationDataBinding> g_operationDataBindings;

    constexpr int kSharedProgressSlotCount = 16;
    constexpr ULONGLONG kSharedProgressFreshnessMs = 5000;

    struct SharedCurrentFileProgressSlot
    {
        volatile LONG sequence = 0;
        volatile LONG ownerPid = 0;
        unsigned long long writerKey = 0;

        unsigned long long completedItems = 0;
        unsigned long long totalItems = 0;
        unsigned long long completedBytes = 0;
        unsigned long long totalBytes = 0;

        unsigned long long currentFileSize = 0;
        unsigned long long currentFileStartBytes = 0;
        BOOL currentFileValid = FALSE;

        ULONGLONG updateTick = 0;
    };

    struct SharedCurrentFileProgressTable
    {
        SharedCurrentFileProgressSlot slots[kSharedProgressSlotCount];
    };

    HANDLE g_sharedProgressMapping = nullptr;
    SharedCurrentFileProgressTable *g_sharedProgressTable = nullptr;

    bool EnsureSharedProgressBridge()
    {
        if (g_sharedProgressTable)
        {
            return true;
        }

        HANDLE mapping = CreateFileMappingW(
            INVALID_HANDLE_VALUE,
            nullptr,
            PAGE_READWRITE,
            0,
            sizeof(SharedCurrentFileProgressTable),
            L"Local\\Windhawk.FileOperationStyler.CurrentFile.v1");

        if (!mapping)
        {
            return false;
        }

        auto *table = static_cast<SharedCurrentFileProgressTable *>(
            MapViewOfFile(
                mapping,
                FILE_MAP_ALL_ACCESS,
                0, 0,
                sizeof(SharedCurrentFileProgressTable)));

        if (!table)
        {
            CloseHandle(mapping);
            return false;
        }

        g_sharedProgressMapping = mapping;
        g_sharedProgressTable = table;
        return true;
    }

    void ShutdownSharedProgressBridge()
    {
        if (g_sharedProgressTable)
        {
            UnmapViewOfFile(g_sharedProgressTable);
            g_sharedProgressTable = nullptr;
        }

        if (g_sharedProgressMapping)
        {
            CloseHandle(g_sharedProgressMapping);
            g_sharedProgressMapping = nullptr;
        }
    }

    int EnsureSharedProgressSlot(
        OperationDataBinding &binding)
    {
        if (!binding.writer || !EnsureSharedProgressBridge())
        {
            return -1;
        }

        DWORD pid = GetCurrentProcessId();
        unsigned long long writerKey =
            reinterpret_cast<unsigned long long>(binding.writer);

        if (binding.sharedSlot >= 0 &&
            binding.sharedSlot < kSharedProgressSlotCount)
        {
            auto &slot =
                g_sharedProgressTable->slots[binding.sharedSlot];

            if (static_cast<DWORD>(slot.ownerPid) == pid &&
                slot.writerKey == writerKey)
            {
                return binding.sharedSlot;
            }

            binding.sharedSlot = -1;
        }

        ULONGLONG now = GetTickCount64();

        for (int i = 0; i < kSharedProgressSlotCount; ++i)
        {
            auto &slot = g_sharedProgressTable->slots[i];

            if (static_cast<DWORD>(slot.ownerPid) == pid &&
                slot.writerKey == writerKey)
            {
                binding.sharedSlot = i;
                return i;
            }
        }

        for (int i = 0; i < kSharedProgressSlotCount; ++i)
        {
            auto &slot = g_sharedProgressTable->slots[i];
            LONG existingPid = slot.ownerPid;
            bool stale =
                existingPid != 0 &&
                now >= slot.updateTick &&
                now - slot.updateTick > 30000;

            if (existingPid == 0 || stale)
            {
                if (InterlockedCompareExchange(
                        &slot.ownerPid,
                        static_cast<LONG>(pid),
                        existingPid) == existingPid)
                {
                    InterlockedIncrement(&slot.sequence);
                    slot.writerKey = writerKey;
                    slot.completedItems = 0;
                    slot.totalItems = 0;
                    slot.completedBytes = 0;
                    slot.totalBytes = 0;
                    slot.currentFileSize = 0;
                    slot.currentFileStartBytes = 0;
                    slot.currentFileValid = FALSE;
                    slot.updateTick = now;
                    InterlockedIncrement(&slot.sequence);

                    binding.sharedSlot = i;

                    return i;
                }
            }
        }

        return -1;
    }

    void PublishSharedProgress(
        OperationDataBinding &binding)
    {
        int index = EnsureSharedProgressSlot(binding);

        if (index < 0)
        {
            return;
        }

        auto &slot = g_sharedProgressTable->slots[index];

        InterlockedIncrement(&slot.sequence);

        slot.completedItems = binding.latestCompletedItems;
        slot.totalItems = binding.latestTotalItems;
        slot.completedBytes = binding.latestCompletedBytes;
        slot.totalBytes = binding.latestTotalBytes;
        slot.currentFileSize = binding.currentFileSize;
        slot.currentFileStartBytes =
            binding.currentFileStartBytes;
        slot.currentFileValid =
            binding.currentFileValid ? TRUE : FALSE;
        slot.updateTick = GetTickCount64();

        MemoryBarrier();
        InterlockedIncrement(&slot.sequence);
    }

    void ClearOperationDataBindingsLocked()
    {
        if (g_sharedProgressTable)
        {
            DWORD pid = GetCurrentProcessId();

            for (OperationDataBinding const &binding :
                 g_operationDataBindings)
            {
                if (!binding.writer ||
                    binding.sharedSlot < 0 ||
                    binding.sharedSlot >= kSharedProgressSlotCount)
                {
                    continue;
                }

                auto &slot =
                    g_sharedProgressTable->slots[binding.sharedSlot];

                unsigned long long writerKey =
                    reinterpret_cast<unsigned long long>(
                        binding.writer);

                if (static_cast<DWORD>(slot.ownerPid) != pid ||
                    slot.writerKey != writerKey)
                {
                    continue;
                }

                InterlockedIncrement(&slot.sequence);

                slot.writerKey = 0;
                slot.completedItems = 0;
                slot.totalItems = 0;
                slot.completedBytes = 0;
                slot.totalBytes = 0;
                slot.currentFileSize = 0;
                slot.currentFileStartBytes = 0;
                slot.currentFileValid = FALSE;
                slot.updateTick = 0;

                MemoryBarrier();
                InterlockedIncrement(&slot.sequence);

                // Release ownership only after the slot contents are stable.
                InterlockedExchange(&slot.ownerPid, 0);
            }
        }

        g_operationDataBindings.clear();
    }

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
        int compactPanelHeight = 84;
        int expandedPanelHeight = 144;
        int itemsY = 0;
        int speedY = 20;
        int progressY = 46;
        int progressHeight = 8;
        int graphY = 82;
        int graphHeight = 60;
        int compactTileHeight = 148;
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
        bool hideSmallControlButtons = false;
        bool hideTitleBarPercentage = false;
        std::wstring preset = L"blueDark";
        ThemePalette theme{};
        bool applyNativeColors = false;
        LayoutConfig layout{};
        TypographyConfig typography{};
        ElementConfig elements{};

        std::wstring glassEffect = L"acrylic";
        COLORREF glassTint = RGB(0, 0, 0);
        int glassStrength = 0;
        std::wstring footerStyle = L"glass";
        std::wstring footerColorPreset = L"theme";
        COLORREF footerColor = RGB(0, 0, 0);
        bool customFooterColor = false;
    };

    const LayoutConfig kDefaultLayout{};
    const TypographyConfig kDefaultTypography{};
    const ElementConfig kDefaultElements{};
    ModSettings g_settings{};
    bool g_showCurrentFileProgressBar = true;

    // The appearance window updates these two values atomically so Explorer's
    // operation-window threads can repaint without touching ModSettings.
    constexpr PCWSTR kAppearanceColorValueName = L"appearance.color";
    constexpr PCWSTR kAppearanceOpacityValueName = L"appearance.opacity";
    std::atomic<int> g_runtimeThemeColor{-1};
    std::atomic<int> g_runtimeGlassStrength{0};
    UINT g_appearanceChangedMessage = 0;
    HANDLE g_settingsUiThreadHandle = nullptr;
    DWORD g_settingsUiThreadId = 0;
    HANDLE g_settingsUiReadyEvent = nullptr;
    HWND g_settingsWindow = nullptr;
    bool g_settingsUiHotkeyRegistered = false;
    HINSTANCE g_settingsUiModule = nullptr;
    std::mutex g_settingsUiLifecycleMutex;

    void RefreshAppearanceWindows(bool refreshGlassTint);

    COLORREF CurrentGlassTint()
    {
        int selectedColor =
            g_runtimeThemeColor.load(std::memory_order_relaxed);
        return selectedColor >= 0
                   ? static_cast<COLORREF>(selectedColor)
                   : g_settings.glassTint;
    }

    int CurrentGlassStrength()
    {
        return std::clamp(
            g_runtimeGlassStrength.load(std::memory_order_relaxed), 0, 100);
    }

    COLORREF CurrentFooterColor()
    {
        int selectedColor =
            g_runtimeThemeColor.load(std::memory_order_relaxed);
        if (g_settings.footerColorPreset == L"custom")
        {
            return selectedColor >= 0
                       ? static_cast<COLORREF>(selectedColor)
                       : g_settings.theme.background;
        }

        return g_settings.footerColor;
    }

    bool HasCustomFooterColor()
    {
        return g_settings.footerColorPreset == L"custom" &&
               g_runtimeThemeColor.load(std::memory_order_relaxed) >= 0;
    }

    bool IsGlassTheme()
    {
        return g_settings.customizationEnabled &&
               g_settings.preset == L"glass";
    }

    bool UseGlassFooter()
    {
        return IsGlassTheme() &&
               g_settings.footerStyle != L"solid";
    }

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
        HIGHCONTRASTW highContrast{};
        highContrast.cbSize = sizeof(highContrast);
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
        HIGHCONTRASTW highContrast{};
        highContrast.cbSize = sizeof(highContrast);
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
        else if (preset == L"emerald")
        {
            theme.background = RGB(18, 43, 38);
            theme.primaryText = RGB(235, 249, 243);
            theme.secondaryText = RGB(150, 190, 174);
            theme.accent = RGB(52, 190, 139);
            theme.inactive = RGB(42, 76, 65);
            theme.graphSurface = RGB(13, 34, 30);
            theme.graphGrid = RGB(67, 105, 89);
            theme.graphLine = RGB(73, 207, 153);
            theme.graphFill = RGB(52, 190, 139);
            theme.actionSurface = RGB(27, 57, 48);
            theme.actionBorder = RGB(53, 91, 75);
            theme.actionText = theme.primaryText;
        }
        else if (preset == L"violet")
        {
            theme.background = RGB(37, 31, 54);
            theme.primaryText = RGB(246, 241, 255);
            theme.secondaryText = RGB(179, 164, 205);
            theme.accent = RGB(174, 126, 245);
            theme.inactive = RGB(68, 57, 91);
            theme.graphSurface = RGB(28, 23, 44);
            theme.graphGrid = RGB(100, 84, 128);
            theme.graphLine = RGB(190, 151, 255);
            theme.graphFill = RGB(157, 111, 226);
            theme.actionSurface = RGB(50, 40, 71);
            theme.actionBorder = RGB(82, 68, 110);
            theme.actionText = theme.primaryText;
        }
        else if (preset == L"nord")
        {
            theme.background = RGB(39, 46, 59);
            theme.primaryText = RGB(236, 239, 244);
            theme.secondaryText = RGB(174, 183, 196);
            theme.accent = RGB(136, 192, 208);
            theme.inactive = RGB(67, 76, 94);
            theme.graphSurface = RGB(33, 39, 51);
            theme.graphGrid = RGB(91, 102, 120);
            theme.graphLine = RGB(143, 188, 187);
            theme.graphFill = RGB(110, 158, 174);
            theme.actionSurface = RGB(48, 57, 72);
            theme.actionBorder = RGB(79, 91, 111);
            theme.actionText = theme.primaryText;
        }
        else if (preset == L"rose")
        {
            theme.background = RGB(49, 33, 41);
            theme.primaryText = RGB(252, 241, 245);
            theme.secondaryText = RGB(200, 161, 175);
            theme.accent = RGB(237, 119, 153);
            theme.inactive = RGB(83, 57, 69);
            theme.graphSurface = RGB(39, 26, 34);
            theme.graphGrid = RGB(117, 79, 96);
            theme.graphLine = RGB(247, 142, 173);
            theme.graphFill = RGB(216, 103, 139);
            theme.actionSurface = RGB(63, 43, 53);
            theme.actionBorder = RGB(101, 67, 83);
            theme.actionText = theme.primaryText;
        }
        else if (preset == L"ocean")
        {
            theme.background = RGB(11, 30, 43);
            theme.primaryText = RGB(234, 248, 250);
            theme.secondaryText = RGB(143, 187, 199);
            theme.accent = RGB(46, 197, 211);
            theme.inactive = RGB(32, 68, 82);
            theme.graphSurface = RGB(7, 23, 35);
            theme.graphGrid = RGB(45, 93, 109);
            theme.graphLine = RGB(98, 224, 230);
            theme.graphFill = RGB(30, 157, 178);
            theme.actionSurface = RGB(17, 48, 62);
            theme.actionBorder = RGB(37, 91, 106);
            theme.actionText = theme.primaryText;
            theme.graphGridAlpha = 78;
            theme.graphFillAlpha = 155;
            theme.graphReferenceAlpha = 205;
            theme.graphLabelBackgroundAlpha = 238;
        }
        else if (preset == L"forest")
        {
            theme.background = RGB(20, 39, 30);
            theme.primaryText = RGB(240, 248, 240);
            theme.secondaryText = RGB(163, 197, 167);
            theme.accent = RGB(112, 209, 138);
            theme.inactive = RGB(52, 82, 61);
            theme.graphSurface = RGB(13, 31, 22);
            theme.graphGrid = RGB(78, 117, 86);
            theme.graphLine = RGB(157, 230, 166);
            theme.graphFill = RGB(66, 175, 103);
            theme.actionSurface = RGB(28, 56, 39);
            theme.actionBorder = RGB(65, 103, 74);
            theme.actionText = theme.primaryText;
            theme.graphGridAlpha = 76;
            theme.graphFillAlpha = 155;
            theme.graphReferenceAlpha = 200;
            theme.graphLabelBackgroundAlpha = 238;
        }
        else if (preset == L"aurora")
        {
            theme.background = RGB(23, 28, 53);
            theme.primaryText = RGB(241, 244, 255);
            theme.secondaryText = RGB(171, 183, 221);
            theme.accent = RGB(112, 225, 208);
            theme.inactive = RGB(57, 68, 101);
            theme.graphSurface = RGB(17, 22, 43);
            theme.graphGrid = RGB(73, 85, 124);
            theme.graphLine = RGB(167, 139, 250);
            theme.graphFill = RGB(79, 202, 191);
            theme.actionSurface = RGB(37, 45, 76);
            theme.actionBorder = RGB(82, 97, 138);
            theme.actionText = theme.primaryText;
            theme.graphGridAlpha = 82;
            theme.graphFillAlpha = 150;
            theme.graphReferenceAlpha = 208;
            theme.graphLabelBackgroundAlpha = 240;
        }
        else if (preset == L"sakura")
        {
            theme.background = RGB(56, 38, 50);
            theme.primaryText = RGB(255, 242, 245);
            theme.secondaryText = RGB(218, 186, 198);
            theme.accent = RGB(242, 139, 170);
            theme.inactive = RGB(101, 70, 81);
            theme.graphSurface = RGB(44, 32, 41);
            theme.graphGrid = RGB(127, 90, 106);
            theme.graphLine = RGB(255, 173, 197);
            theme.graphFill = RGB(223, 113, 146);
            theme.actionSurface = RGB(74, 48, 61);
            theme.actionBorder = RGB(128, 83, 104);
            theme.actionText = theme.primaryText;
            theme.graphGridAlpha = 80;
            theme.graphFillAlpha = 158;
            theme.graphReferenceAlpha = 210;
            theme.graphLabelBackgroundAlpha = 240;
        }
        else if (preset == L"sunset")
        {
            theme.background = RGB(48, 33, 38);
            theme.primaryText = RGB(255, 245, 233);
            theme.secondaryText = RGB(217, 185, 163);
            theme.accent = RGB(255, 155, 84);
            theme.inactive = RGB(101, 65, 61);
            theme.graphSurface = RGB(36, 25, 30);
            theme.graphGrid = RGB(129, 80, 74);
            theme.graphLine = RGB(255, 196, 107);
            theme.graphFill = RGB(236, 115, 79);
            theme.actionSurface = RGB(71, 45, 44);
            theme.actionBorder = RGB(128, 80, 68);
            theme.actionText = theme.primaryText;
            theme.graphGridAlpha = 78;
            theme.graphFillAlpha = 155;
            theme.graphReferenceAlpha = 210;
            theme.graphLabelBackgroundAlpha = 240;
        }
        else if (preset == L"carbon")
        {
            theme.background = RGB(27, 29, 32);
            theme.primaryText = RGB(243, 245, 247);
            theme.secondaryText = RGB(165, 173, 182);
            theme.accent = RGB(142, 168, 195);
            theme.inactive = RGB(61, 66, 72);
            theme.graphSurface = RGB(18, 20, 22);
            theme.graphGrid = RGB(91, 98, 107);
            theme.graphLine = RGB(184, 197, 211);
            theme.graphFill = RGB(113, 134, 155);
            theme.actionSurface = RGB(37, 41, 45);
            theme.actionBorder = RGB(80, 88, 96);
            theme.actionText = theme.primaryText;
            theme.graphGridAlpha = 70;
            theme.graphFillAlpha = 140;
            theme.graphReferenceAlpha = 190;
            theme.graphLabelBackgroundAlpha = 235;
        }
        else if (preset == L"arctic")
        {
            theme.background = RGB(237, 245, 250);
            theme.primaryText = RGB(27, 48, 64);
            theme.secondaryText = RGB(88, 113, 132);
            theme.accent = RGB(22, 138, 184);
            theme.inactive = RGB(203, 220, 231);
            theme.graphSurface = RGB(228, 238, 245);
            theme.graphGrid = RGB(169, 192, 208);
            theme.graphLine = RGB(8, 127, 171);
            theme.graphFill = RGB(102, 187, 221);
            theme.actionSurface = RGB(217, 232, 241);
            theme.actionBorder = RGB(174, 196, 210);
            theme.actionText = theme.primaryText;
            theme.graphGridAlpha = 88;
            theme.graphFillAlpha = 155;
            theme.graphReferenceAlpha = 200;
            theme.graphLabelBackgroundAlpha = 242;
        }
        else if (preset == L"nebula")
        {
            theme.background = RGB(35, 26, 56);
            theme.primaryText = RGB(247, 241, 255);
            theme.secondaryText = RGB(192, 172, 215);
            theme.accent = RGB(206, 112, 245);
            theme.inactive = RGB(74, 58, 105);
            theme.graphSurface = RGB(25, 18, 40);
            theme.graphGrid = RGB(89, 68, 114);
            theme.graphLine = RGB(240, 128, 194);
            theme.graphFill = RGB(139, 103, 232);
            theme.actionSurface = RGB(51, 36, 73);
            theme.actionBorder = RGB(113, 86, 143);
            theme.actionText = theme.primaryText;
            theme.graphGridAlpha = 85;
            theme.graphFillAlpha = 158;
            theme.graphReferenceAlpha = 212;
            theme.graphLabelBackgroundAlpha = 241;
        }
        else
        {
            // Blue Dark: clearly blue-tinted body with a strong blue accent.
            // Glass shares this palette; only its host backdrop is different.
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


    bool ParseArgbColorValue(
        std::wstring value,
        COLORREF *color,
        BYTE *alpha)
    {
        if (!color || !alpha || value.empty() ||
            _wcsicmp(value.c_str(), L"auto") == 0)
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

        if (value.size() != 8)
        {
            return false;
        }

        wchar_t *end = nullptr;
        unsigned long argb = wcstoul(value.c_str(), &end, 16);
        if (!end || *end != L'\0' || argb > 0xFFFFFFFFUL)
        {
            return false;
        }

        *alpha = static_cast<BYTE>((argb >> 24) & 0xFF);
        *color = RGB((argb >> 16) & 0xFF,
                     (argb >> 8) & 0xFF,
                     argb & 0xFF);
        return true;
    }

    ThemePalette GetDrawingTheme(HWND)
    {
        // 0.12 skin boundary:
        // Presets/colors are consumed only by File Operation Styler's own
        // GDI+/HWND presentation surfaces. We intentionally do not skin the
        // native DirectUI operation tree, so the same palette code is portable
        // across machines whose Explorer backing surfaces differ.
        // Customization-off is system-derived, but never sampled from an
        // undocumented DirectUI-owned pixel. This keeps one deterministic
        // palette on machines whose native backing surfaces differ.
        ThemePalette theme = g_settings.applyNativeColors
                                 ? g_settings.theme
                                 : MakeSystemTheme();

        int selectedColor =
            g_runtimeThemeColor.load(std::memory_order_relaxed);
        if (g_settings.customizationEnabled && selectedColor >= 0)
        {
            COLORREF color = static_cast<COLORREF>(selectedColor);
            theme.accent = color;
            theme.graphLine = color;
            theme.graphFill = color;
        }

        return theme;
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

        g_showCurrentFileProgressBar =
            Wh_GetIntSetting(L"showCurrentFileProgressBar") != 0;

        g_settings.hideSmallControlButtons =
            Wh_GetIntSetting(L"hideSmallControlButtons") != 0;

        g_settings.hideTitleBarPercentage =
            Wh_GetIntSetting(L"hideTitleBarPercentage") != 0;

        g_settings.customizationEnabled =
            Wh_GetIntSetting(L"customization.enabled") != 0;

        g_settings.preset =
            GetStringSettingValue(L"customization.preset");
        if (g_settings.preset.empty())
        {
            g_settings.preset = L"blueDark";
        }

        g_settings.theme = MakePresetTheme(g_settings.preset);

        g_settings.glassEffect =
            GetStringSettingValue(
                L"customization.glassEffect");

        if (g_settings.glassEffect != L"blur" &&
            g_settings.glassEffect != L"acrylic" &&
            g_settings.glassEffect != L"mica" &&
            g_settings.glassEffect != L"solid")
        {
            g_settings.glassEffect = L"acrylic";
        }

        // Keep the base material tint as transparent black. The appearance
        // window's saved color is applied as a runtime override below.
        g_settings.glassTint = RGB(0, 0, 0);

        g_settings.glassStrength =
            GetClampedIntSetting(
                L"customization.glassTintStrength",
                0,
                100);

        int storedOpacity =
            Wh_GetIntValue(kAppearanceOpacityValueName, -1);
        g_runtimeGlassStrength.store(
            std::clamp(
                storedOpacity >= 0 ? storedOpacity : g_settings.glassStrength,
                0,
                100),
            std::memory_order_relaxed);

        g_settings.footerStyle =
            GetStringSettingValue(
                L"customization.footerStyle");

        if (g_settings.footerStyle != L"solid")
        {
            g_settings.footerStyle = L"glass";
        }

        g_settings.footerColorPreset =
            GetStringSettingValue(
                L"customization.footerColorPreset");

        if (g_settings.footerColorPreset.empty())
        {
            g_settings.footerColorPreset = L"theme";
        }

        g_settings.customFooterColor = false;

        if (g_settings.footerColorPreset == L"custom")
        {
            // The shared picker below supplies the custom footer color.
        }
        else if (g_settings.footerColorPreset == L"blueDark" ||
                 g_settings.footerColorPreset == L"graphite" ||
                 g_settings.footerColorPreset == L"midnight" ||
                 g_settings.footerColorPreset == L"warmDark" ||
                 g_settings.footerColorPreset == L"light" ||
                 g_settings.footerColorPreset == L"system" ||
                 g_settings.footerColorPreset == L"emerald" ||
                 g_settings.footerColorPreset == L"violet" ||
                 g_settings.footerColorPreset == L"nord" ||
                 g_settings.footerColorPreset == L"rose" ||
                 g_settings.footerColorPreset == L"ocean" ||
                 g_settings.footerColorPreset == L"forest" ||
                 g_settings.footerColorPreset == L"aurora" ||
                 g_settings.footerColorPreset == L"sakura" ||
                 g_settings.footerColorPreset == L"sunset" ||
                 g_settings.footerColorPreset == L"carbon" ||
                 g_settings.footerColorPreset == L"arctic" ||
                 g_settings.footerColorPreset == L"nebula")
        {
            ThemePalette footerTheme =
                MakePresetTheme(g_settings.footerColorPreset);

            g_settings.footerColor = footerTheme.background;
            g_settings.customFooterColor = true;
        }
        else
        {
            g_settings.footerColorPreset = L"theme";
        }
        COLORREF legacyColor = RGB(0, 0, 0);
        BYTE legacyColorAlpha = 0;
        bool parsedLegacyColor = ParseArgbColorValue(
            GetStringSettingValue(
                L"customization.colors.themeColor"),
            &legacyColor,
            &legacyColorAlpha);

        wchar_t savedColor[32]{};
        size_t savedColorLength = Wh_GetStringValue(
            kAppearanceColorValueName,
            savedColor,
            std::size(savedColor));
        bool hasSavedAppearanceColor = savedColorLength != 0;
        COLORREF savedColorValue = RGB(0, 0, 0);
        BYTE savedColorAlpha = 0;
        bool savedColorIsThemeDefault =
            hasSavedAppearanceColor && _wcsicmp(savedColor, L"theme") == 0;
        bool parsedSavedColor =
            hasSavedAppearanceColor && !savedColorIsThemeDefault &&
            ParseArgbColorValue(
                savedColor, &savedColorValue, &savedColorAlpha);

        bool legacyCustomColor =
            parsedLegacyColor &&
            (legacyColorAlpha != 0 || legacyColor != RGB(0, 0, 0));
        bool customThemeColor =
            hasSavedAppearanceColor
                ? parsedSavedColor
                : legacyCustomColor;
        COLORREF selectedThemeColor =
            parsedSavedColor ? savedColorValue : legacyColor;

        if (hasSavedAppearanceColor && savedColorIsThemeDefault)
        {
            customThemeColor = false;
        }

        g_runtimeThemeColor.store(
            customThemeColor ? static_cast<int>(selectedThemeColor) : -1,
            std::memory_order_relaxed);

        if (customThemeColor)
        {
            g_settings.glassTint = selectedThemeColor;
            if (g_settings.footerColorPreset == L"custom")
            {
                g_settings.footerColor = selectedThemeColor;
                g_settings.customFooterColor = true;
            }
        }

        g_settings.applyNativeColors =
            g_settings.customizationEnabled &&
            (g_settings.preset != L"system" || customThemeColor);

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
#define kAccentRingColor (GetDrawingTheme(nullptr).accent)
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
    constexpr COLORREF kDwmColorNone = 0xFFFFFFFE;
    constexpr DWORD kDwmwaSystemBackdropType = 38;
    constexpr int kDwmsbtAuto = 0;
    constexpr int kDwmsbtNone = 1;
    constexpr int kDwmsbtTransientWindow = 3;
    constexpr int kDwmsbtTabbedWindow = 4;

    constexpr int kWcaAccentPolicy = 19;
    constexpr int kAccentStateDisabled = 0;
    constexpr int kCustomAccentEnableBlurBehind = 3;

    constexpr wchar_t kGlassEffectAppliedProperty[] =
        L"Windhawk.FileOperationStyler.GlassEffectApplied";

    constexpr wchar_t kGlassAccentBlurAppliedProperty[] =
        L"Windhawk.FileOperationStyler.GlassAccentBlurApplied";

    struct GlassAccentPolicy
    {
        int accentState;
        int accentFlags;
        int gradientColor;
        int animationId;
    };

    struct GlassWindowCompositionAttributeData
    {
        int attribute;
        PVOID data;
        SIZE_T size;
    };

    using SetWindowCompositionAttribute_t =
        BOOL(WINAPI *)(
            HWND,
            GlassWindowCompositionAttributeData *);

    using DwmEnableBlurBehindWindow_t =
        HRESULT(WINAPI *)(
            HWND,
            DWM_BLURBEHIND const *);

    static thread_local bool g_glassTransparentInfoPanelPaint = false;

    using DwmSetWindowAttribute_t = HRESULT(WINAPI *)(
        HWND hwnd, DWORD attribute, LPCVOID value, DWORD valueSize);

    HMODULE g_ownedDwmApiModule;
    DwmSetWindowAttribute_t g_dwmSetWindowAttribute;
    DwmEnableBlurBehindWindow_t g_dwmEnableBlurBehindWindow;

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

            g_dwmEnableBlurBehindWindow =
                reinterpret_cast<DwmEnableBlurBehindWindow_t>(
                    GetProcAddress(
                        module,
                        "DwmEnableBlurBehindWindow"));
            if (owned)
            {
                g_ownedDwmApiModule = module;
            } });
        return g_dwmSetWindowAttribute;
    }

    void ShutdownDwmApi()
    {
        g_dwmSetWindowAttribute = nullptr;
        g_dwmEnableBlurBehindWindow = nullptr;
        if (g_ownedDwmApiModule)
        {
            FreeLibrary(g_ownedDwmApiModule);
            g_ownedDwmApiModule = nullptr;
        }
    }

    DwmEnableBlurBehindWindow_t GetDwmEnableBlurBehindWindow()
    {
        GetDwmSetWindowAttribute();
        return g_dwmEnableBlurBehindWindow;
    }

    SetWindowCompositionAttribute_t GetSetWindowCompositionAttribute()
    {
        static SetWindowCompositionAttribute_t function =
            reinterpret_cast<SetWindowCompositionAttribute_t>(
                GetProcAddress(
                    GetModuleHandleW(L"user32.dll"),
                    "SetWindowCompositionAttribute"));

        return function;
    }

    DWORD MakeGlassAccentGradientColor()
    {
        // ACCENT_POLICY uses AABBGGRR. Preserve the original subtle base tint
        // for Custom Blur; the shared paint path adds the adjustable opacity
        // layer for Acrylic, Custom Blur, and Mica.
        constexpr DWORD kCustomBlurBaseTintAlpha = 32;

        return (kCustomBlurBaseTintAlpha << 24) |
               (static_cast<DWORD>(CurrentGlassTint()) & 0x00FFFFFF);
    }

    void SetGlassAccentBlurBehind(
        HWND hostWindow,
        bool enabled,
        int accentState = kCustomAccentEnableBlurBehind)
    {
        if (!hostWindow || !IsWindow(hostWindow))
        {
            return;
        }

        if (!enabled)
        {
            // Never send ACCENT_STATE_DISABLED to a window that never had
            // our Blur recipe. Doing so can disturb the native Acrylic
            // SystemBackdrop path even though Acrylic itself is correct.
            if (!GetPropW(
                    hostWindow,
                    kGlassAccentBlurAppliedProperty))
            {
                return;
            }
        }

        SetWindowCompositionAttribute_t setCompositionAttribute =
            GetSetWindowCompositionAttribute();

        if (!setCompositionAttribute)
        {
            Wh_Log(
                L"Glass: SetWindowCompositionAttribute unavailable hwnd=%p",
                reinterpret_cast<void *>(hostWindow));
            return;
        }

        // ACCENT_ENABLE_BLURBEHIND = 3 and
        // ACCENT_ENABLE_ACRYLICBLURBEHIND = 4. The caller selects the
        // native material; the user tint is composited by the shared painter.
        GlassAccentPolicy accentPolicy{};
        accentPolicy.accentState =
            enabled
                ? accentState
                : kAccentStateDisabled;

        if (enabled)
        {
            accentPolicy.gradientColor =
                static_cast<int>(
                    MakeGlassAccentGradientColor());
        }

        GlassWindowCompositionAttributeData data{};
        data.attribute = kWcaAccentPolicy;
        data.data = &accentPolicy;
        data.size = sizeof(accentPolicy);

        if (!setCompositionAttribute(hostWindow, &data))
        {
            Wh_Log(
                L"Glass: SetWindowCompositionAttribute failed "
                L"hwnd=%p error=%lu",
                reinterpret_cast<void *>(hostWindow),
                GetLastError());
            return;
        }

        if (enabled)
        {
            SetPropW(
                hostWindow,
                kGlassAccentBlurAppliedProperty,
                reinterpret_cast<HANDLE>(1));
        }
        else
        {
            RemovePropW(
                hostWindow,
                kGlassAccentBlurAppliedProperty);
        }

        Wh_Log(
            L"Glass: AccentBlurBehind %s tint=#%02X%02X%02X strength=%d hwnd=%p",
            enabled ? L"enabled" : L"disabled",
            GetRValue(CurrentGlassTint()),
            GetGValue(CurrentGlassTint()),
            GetBValue(CurrentGlassTint()),
            CurrentGlassStrength(),
            reinterpret_cast<void *>(hostWindow));
    }
    struct GlassFrameMargins
    {
        int left;
        int right;
        int top;
        int bottom;
    };

    using DwmExtendFrameIntoClientArea_t =
        HRESULT(WINAPI *)(
            HWND hwnd,
            GlassFrameMargins const *margins);

    void SetGlassDwmBlurRegion(
        HWND hostWindow,
        bool enabled)
    {
        if (!hostWindow || !IsWindow(hostWindow))
        {
            return;
        }

        auto enableBlurBehind =
            GetDwmEnableBlurBehindWindow();

        if (!enableBlurBehind)
        {
            return;
        }

        HRGN blurRegion =
            enabled
                ? CreateRectRgn(0, 0, -1, -1)
                : nullptr;

        DWM_BLURBEHIND blurBehind{};
        blurBehind.dwFlags =
            DWM_BB_ENABLE |
            (enabled ? DWM_BB_BLURREGION : 0);
        blurBehind.fEnable = enabled ? TRUE : FALSE;
        blurBehind.hRgnBlur = blurRegion;

        HRESULT result =
            enableBlurBehind(
                hostWindow,
                &blurBehind);

        if (blurRegion)
        {
            DeleteObject(blurRegion);
        }

        if (FAILED(result))
        {
            Wh_Log(
                L"Glass: DwmEnableBlurBehindWindow failed "
                L"result=0x%08X hwnd=%p",
                static_cast<unsigned int>(result),
                reinterpret_cast<void *>(hostWindow));
        }
    }
    void SetGlassClientFrameExtension(
        HWND hostWindow,
        bool enabled)
    {
        if (!hostWindow || !IsWindow(hostWindow))
        {
            return;
        }

        HMODULE module =
            GetModuleHandleW(L"dwmapi.dll");

        if (!module)
        {
            return;
        }

        auto extendFrame =
            reinterpret_cast<DwmExtendFrameIntoClientArea_t>(
                GetProcAddress(
                    module,
                    "DwmExtendFrameIntoClientArea"));

        if (!extendFrame)
        {
            return;
        }

        GlassFrameMargins margins =
            enabled
                ? GlassFrameMargins{-1, -1, -1, -1}
                : GlassFrameMargins{0, 0, 0, 0};

        HRESULT result =
            extendFrame(
                hostWindow,
                &margins);

        if (FAILED(result))
        {
            Wh_Log(
                L"Glass: ExtendFrame failed result=0x%08X hwnd=%p",
                static_cast<unsigned int>(result),
                reinterpret_cast<void *>(hostWindow));
        }
    }

    void ApplyUnifiedHostChrome(HWND hostWindow)
    {
        if (!IsGlassTheme())
        {
            return;
        }

        DwmSetWindowAttribute_t setAttribute = GetDwmSetWindowAttribute();
        if (!setAttribute || !hostWindow || !IsWindow(hostWindow))
        {
            return;
        }

        int backdropType = kDwmsbtAuto;
        bool extendGlassFrame = true;

        if (g_settings.glassEffect == L"acrylic")
        {
            // Windows' transient-window system backdrop is its Acrylic material.
            backdropType = kDwmsbtTransientWindow;
            SetGlassDwmBlurRegion(hostWindow, false);
            SetGlassAccentBlurBehind(hostWindow, false);
        }
        else if (g_settings.glassEffect == L"blur")
        {
            // Custom Blur uses the classic Accent blur material.
            backdropType = kDwmsbtAuto;
            SetGlassDwmBlurRegion(hostWindow, true);
            SetGlassAccentBlurBehind(
                hostWindow, true, kCustomAccentEnableBlurBehind);
        }
        else if (g_settings.glassEffect == L"mica")
        {
            // DWMSBT_TABBEDWINDOW selects the Mica system backdrop.
            backdropType = kDwmsbtTabbedWindow;
            SetGlassDwmBlurRegion(hostWindow, false);
            SetGlassAccentBlurBehind(hostWindow, false);
        }
        else
        {
            // Solid keeps the same custom operation layout with an opaque
            // theme-colored client area and no DWM backdrop material.
            backdropType = kDwmsbtNone;
            extendGlassFrame = false;
            SetGlassDwmBlurRegion(hostWindow, false);
            SetGlassAccentBlurBehind(hostWindow, false);
        }

        HRESULT backdropResult =
            setAttribute(
                hostWindow,
                kDwmwaSystemBackdropType,
                &backdropType,
                sizeof(backdropType));

        if (FAILED(backdropResult))
        {
            Wh_Log(
                L"Glass: SYSTEMBACKDROP_TYPE effect=%s failed "
                L"result=0x%08X hwnd=%p",
                g_settings.glassEffect.c_str(),
                static_cast<unsigned int>(backdropResult),
                reinterpret_cast<void *>(hostWindow));
        }

        SetGlassClientFrameExtension(hostWindow, extendGlassFrame);

        SetPropW(
            hostWindow,
            kGlassEffectAppliedProperty,
            reinterpret_cast<HANDLE>(1));

        Wh_Log(
            L"Glass: applied effect=%s tint=#%02X%02X%02X opacity=%d "
            L"hwnd=%p",
            g_settings.glassEffect.c_str(),
            GetRValue(CurrentGlassTint()),
            GetGValue(CurrentGlassTint()),
            GetBValue(CurrentGlassTint()),
            CurrentGlassStrength(),
            reinterpret_cast<void *>(hostWindow));

        if (!IsIconic(hostWindow))
        {
            DefWindowProcW(hostWindow, WM_NCACTIVATE, TRUE, -1);
        }
    }
    void ApplyHostThemeColors(HWND hostWindow)
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
        // Suppress only Glass's native caption fill so DWM's Acrylic shows
        // through behind the native caption text and buttons.
        COLORREF captionColor =
            IsGlassTheme() && g_settings.glassEffect != L"solid"
                ? kDwmColorNone
                : kBackgroundColor;
        COLORREF textColor = kPrimaryTextColor;
        COLORREF borderColor = kInactiveRingColor;

        setAttribute(
            hostWindow,
            kDwmwaUseImmersiveDarkMode,
            &darkMode,
            sizeof(darkMode));

        setAttribute(
            hostWindow,
            kDwmwaCaptionColor,
            &captionColor,
            sizeof(captionColor));

        setAttribute(
            hostWindow,
            kDwmwaTextColor,
            &textColor,
            sizeof(textColor));

        setAttribute(
            hostWindow,
            kDwmwaBorderColor,
            &borderColor,
            sizeof(borderColor));
    }
    void ResetUnifiedHostChrome(HWND hostWindow)
    {
        if (!IsGlassTheme() && !ShouldApplyNativeColorOverrides())
        {
            return;
        }

        DwmSetWindowAttribute_t setAttribute = GetDwmSetWindowAttribute();
        if (!setAttribute || !hostWindow || !IsWindow(hostWindow))
        {
            return;
        }

        bool hadGlassEffect =
            RemovePropW(
                hostWindow,
                kGlassEffectAppliedProperty) != nullptr;

        if (hadGlassEffect || IsGlassTheme())
        {
            SetGlassAccentBlurBehind(hostWindow, false);
            SetGlassDwmBlurRegion(hostWindow, false);
            SetGlassClientFrameExtension(hostWindow, false);
        }

        BOOL systemDarkMode = IsWindowsAppsDarkMode() ? TRUE : FALSE;

        setAttribute(
            hostWindow,
            kDwmwaUseImmersiveDarkMode,
            &systemDarkMode,
            sizeof(systemDarkMode));

        COLORREF defaultColor = kDwmColorDefault;

        setAttribute(
            hostWindow,
            kDwmwaCaptionColor,
            &defaultColor,
            sizeof(defaultColor));

        setAttribute(
            hostWindow,
            kDwmwaTextColor,
            &defaultColor,
            sizeof(defaultColor));

        setAttribute(
            hostWindow,
            kDwmwaBorderColor,
            &defaultColor,
            sizeof(defaultColor));

        if (IsGlassTheme())
        {
            int backdropType = kDwmsbtAuto;
            setAttribute(
                hostWindow,
                kDwmwaSystemBackdropType,
                &backdropType,
                sizeof(backdropType));

            // Reset on the owning UI thread for native special states/unload.
            DefWindowProcW(hostWindow, WM_NCACTIVATE,
                           GetActiveWindow() == hostWindow, -1);
        }
    }

#define kDisplayModeFooterReserveHeight (ActiveLayout().footerReserveHeight)

    constexpr wchar_t kCircleWindowClass[] =
        L"Windhawk.FileOperationStyler.ProgressCircle.1.0.0";
    constexpr wchar_t kInfoPanelWindowClass[] =
        L"Windhawk.FileOperationStyler.OperationPresentation.1.0.0";
    constexpr wchar_t kFooterOverlayWindowClass[] =
        L"Windhawk.FileOperationStyler.FooterPresentation.1.0.0";

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
    constexpr WPARAM kRemoveProgressWindowSubclassCommand = 1;

    constexpr UINT kCurrentFileAnimationMessage = WM_APP + 0x51;
    constexpr UINT_PTR kCurrentFileAnimationTimer = 0xF0510020;
    constexpr UINT_PTR kCircleHoverAnimationTimer = 0xF0510021;
    constexpr UINT kCircleHoverAnimationIntervalMs = 16;
    constexpr UINT kPausedBreathingIntervalMs = 66;

    struct CurrentFileAnimation
    {
        double displayedPercent = 0.0;
        double targetPercent = 0.0;
        double displayedOverallPercent = 0.0;
        double targetOverallPercent = 0.0;
        ULONGLONG lastTick = 0;
        unsigned long long fileStartBytes = 0;
        unsigned long long fileSize = 0;
        bool identityValid = false;
        bool overallInitialized = false;
        bool timerRunning = false;
    };

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
        CurrentFileAnimation currentFileAnimation{};
        bool circleHovered = false;
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

    void RefreshAppearanceWindows(bool refreshGlassTint)
    {
        std::vector<HWND> hosts;
        std::vector<HWND> children;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            hosts.reserve(g_circles.size());
            children.reserve(g_circles.size() * 3);
            auto addUnique = [](std::vector<HWND> &collection, HWND window)
            {
                if (window &&
                    std::find(collection.begin(), collection.end(), window) ==
                        collection.end())
                {
                    collection.push_back(window);
                }
            };

            for (CircleState const &state : g_circles)
            {
                addUnique(hosts, state.hostWindow);
                addUnique(children, state.circleWindow);
                addUnique(children, state.infoWindow);
                addUnique(children, state.progressWindow);
            }
        }

        for (HWND host : hosts)
        {
            if (host && IsWindow(host))
            {
                if (refreshGlassTint && g_appearanceChangedMessage)
                {
                    PostMessageW(host, g_appearanceChangedMessage, 0, 0);
                }
                InvalidateRect(host, nullptr, FALSE);
            }
        }

        for (HWND child : children)
        {
            if (child && IsWindow(child))
            {
                InvalidateRect(child, nullptr, FALSE);
            }
        }
    }

    struct HostPresentationState
    {
        HWND hostWindow;
        bool sawNormalProgressCaption;
        bool specialOperationState;
        // Excludes incomplete registration so ordinary tile activation does
        // not enter the post-conflict measured-rate recovery path.
        bool nativeSpecialState;
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
    void InvalidateInfoPanelForTile(OperationTileElement *tile,
                                    bool notifyAnimation = true);
    void StopCurrentFileAnimation(HWND infoWindow);
    void ScheduleProgressCirclePosition(HWND hostWindow, PCWSTR reason);
    void HandleDeferredDisplaySnapshot(HWND hostWindow,
                                       unsigned long long transitionId);
    void ScheduleDeferredDisplaySnapshot(COperationStatusTile *owner,
                                         unsigned long long transitionId,
                                         bool requestedExpanded);
    bool ResizeOperationStatusWindowForMode(
        HWND hostWindow,
        bool expanded,
        unsigned long long transitionId);

    struct NativeProgressSnapshot
    {
        int position;
        int rangeLow;
        int rangeHigh;
        int percent;
        bool rangeValid;
    };

    NativeProgressSnapshot ReadNativeProgress(
        HWND progressWindow,
        int fallbackPercent);
    bool EnsureProgressCircle(
        OperationTileElement *tile,
        unsigned long long eventId,
        NativeProgressSnapshot const &progress);

    bool ApplyDisplayMode(COperationStatusTile *owner,
                          bool applyFinalHostGeometry,
                          unsigned long long transitionId = 0);
    void InitializeRegisteredDisplayMode(COperationStatusTile *owner);
    bool GetVerifiedCustomHostWindowHeight(HWND hostWindow,
                                           int nativeWindowHeight,
                                           int *targetWindowHeight);
    bool IsHostInSpecialOperationState(HWND hostWindow);
    bool IsNormalPresentationHierarchyValidForHost(HWND hostWindow,
                                                   bool *stateAvailable);
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
        double displayedOverallPercent = 0.0;
        double displayedCurrentFilePercent = 0.0;
        bool currentFileProgressValid = false;
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
        bool circleHovered = false;
        std::wstring description;
        std::wstring descriptionStart;
        std::wstring firstLocation;
        std::wstring descriptionMiddle;
        std::wstring secondLocation;
        std::wstring descriptionEnd;
        std::wstring currentItemName;
        std::wstring timeRemainingLabel;
        std::wstring itemsRemainingLabel;
        std::vector<double> rateHistory;
    };

    bool GetInfoPanelSnapshot(HWND infoWindow,
                              InfoPanelSnapshot *snapshot)
    {
        OperationTileElement *tile = nullptr;
        bool storedPaused = false;
        bool storedPausedKnown = false;
        CurrentFileAnimation animation{};
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
            snapshot->percent = std::clamp(it->progressPercent, 0, 100);
            storedPaused = it->paused;
            storedPausedKnown = it->pausedStateKnown;
            snapshot->circleHovered = it->circleHovered;
            animation = it->currentFileAnimation;

            snapshot->displayedOverallPercent =
                animation.overallInitialized
                    ? std::clamp(
                          animation.displayedOverallPercent,
                          0.0, 100.0)
                    : static_cast<double>(snapshot->percent);
        }

        // The per-tile state is authoritative after this operation's custom
        // Pause/Resume action succeeds. New tiles start running; never infer a
        // tile state from Explorer's localized or aggregate host caption.
        snapshot->paused = storedPausedKnown && storedPaused;

        TransferSummaryState stateCopy{};
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
            stateCopy = *it;
        }

        snapshot->completedBytes = stateCopy.completedBytes;
        snapshot->totalBytes = stateCopy.totalBytes;
        if (stateCopy.resumedFromSpecialState && snapshot->totalBytes > 0 &&
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
        snapshot->completedItems = stateCopy.completedItems;
        snapshot->totalItems = stateCopy.totalItems;
        snapshot->bytesValid = stateCopy.bytesValid;
        // A newly observed identity renders zero even if its animation-start
        // message hasn't been dispatched yet. Never show the previous file.
        if (animation.identityValid &&
            animation.fileStartBytes == stateCopy.currentFileStartBytes &&
            animation.fileSize == stateCopy.currentFileSize)
        {
            double realCurrentFilePercent = 0.0;

            if (stateCopy.currentFileSize > 0)
            {
                long double rawPercent =
                    static_cast<long double>(
                        stateCopy.currentFileCompletedBytes) *
                    100.0L /
                    static_cast<long double>(
                        stateCopy.currentFileSize);

                realCurrentFilePercent =
                    static_cast<double>(
                        std::clamp<long double>(
                            rawPercent, 0.0L, 100.0L));
            }

            snapshot->displayedCurrentFilePercent =
                std::min(
                    animation.displayedPercent,
                    realCurrentFilePercent);
        }
        snapshot->currentFileProgressValid =
            stateCopy.currentFileProgressValid;
        snapshot->itemsValid = stateCopy.itemsValid;
        bool useMeasuredRate =
            stateCopy.preferMeasuredRate &&
            stateCopy.measuredDisplayRateValid;
        if (useMeasuredRate)
        {
            snapshot->nativeRate = stateCopy.measuredDisplayRate;
            snapshot->nativeRateValid = true;
        }
        else if (stateCopy.nativeDisplayRateValid)
        {
            snapshot->nativeRate = stateCopy.nativeDisplayRate;
            snapshot->nativeRateValid = true;
        }
        else
        {
            snapshot->nativeRate = stateCopy.measuredDisplayRate;
            snapshot->nativeRateValid =
                stateCopy.measuredDisplayRateValid;
        }
        snapshot->expanded = stateCopy.expanded;
        snapshot->displayModeKnown = stateCopy.displayModeKnown;
        snapshot->deleteLike =
            stateCopy.deleteLikeKnown && stateCopy.deleteLike;
        snapshot->rateHistory = stateCopy.nativeRateHistory;
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
        snapshot->timeRemainingLabel =
            readDescriptionFragment(L"eltTimeRemainingLabel");
        snapshot->itemsRemainingLabel =
            readDescriptionFragment(L"eltItemsRemainingLabel");
        if (snapshot->timeRemainingLabel.empty())
        {
            snapshot->timeRemainingLabel = L"Time remaining:";
        }
        if (snapshot->itemsRemainingLabel.empty())
        {
            snapshot->itemsRemainingLabel = L"Items remaining:";
        }
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

    void SetCircleHoverState(HWND infoWindow, bool hovered)
    {
        if (!infoWindow || !IsWindow(infoWindow))
        {
            return;
        }

        OperationTileElement *tile = nullptr;
        bool changed = false;
        bool paused = false;

        {
            std::lock_guard<std::mutex> lock(g_circleMutex);

            auto it = std::find_if(
                g_circles.begin(),
                g_circles.end(),
                [infoWindow](CircleState const &state)
                {
                    return state.infoWindow == infoWindow;
                });

            if (it == g_circles.end())
            {
                return;
            }

            tile = it->tile;
            paused =
                it->pausedStateKnown &&
                it->paused;

            if (it->circleHovered != hovered)
            {
                it->circleHovered = hovered;
                changed = true;
            }
        }

        if (changed)
        {
            if (hovered)
            {
                SetTimer(
                    infoWindow,
                    kCircleHoverAnimationTimer,
                    kCircleHoverAnimationIntervalMs,
                    nullptr);
            }
            else if (paused)
            {
                SetTimer(
                    infoWindow,
                    kCircleHoverAnimationTimer,
                    kPausedBreathingIntervalMs,
                    nullptr);
            }
            else
            {
                KillTimer(
                    infoWindow,
                    kCircleHoverAnimationTimer);
            }

            if (tile)
            {
                InvalidateInfoPanelForTile(tile, false);
            }
        }
    }
    void ClearCircleHoverStatesForHost(HWND hostWindow)
    {
        if (!hostWindow)
        {
            return;
        }

        std::vector<HWND> infoWindows;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            for (CircleState const &state : g_circles)
            {
                if (state.hostWindow == hostWindow &&
                    state.infoWindow)
                {
                    infoWindows.push_back(state.infoWindow);
                }
            }
        }

        for (HWND infoWindow : infoWindows)
        {
            SetCircleHoverState(infoWindow, false);
        }
    }

    void DrawEmbeddedProgressCircle(Gdiplus::Graphics &graphics,
                                    UINT dpi,
                                    double displayProgress,
                                    ThemePalette const &theme,
                                    TypographyConfig const &type,
                                    bool paused = false,
                                    bool hovered = false)
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

        displayProgress = std::clamp(displayProgress, 0.0, 100.0);
        if (displayProgress > 0.0)
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

        int displayProgressText = std::clamp(
            static_cast<int>(displayProgress + 0.5),
            0, 100);

        wchar_t percentageText[16]{};
        wsprintfW(percentageText, L"%d%%", displayProgressText);

        Gdiplus::Font percentageFont(
            type.circleFont.c_str(),
            static_cast<Gdiplus::REAL>(
                ScaleForDpi(type.circlePercentSize, dpi)),
            Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::Font percentageFallback(
            L"Segoe UI",
            static_cast<Gdiplus::REAL>(
                ScaleForDpi(type.circlePercentSize, dpi)),
            Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
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
        // ------------------------------------------------------------
        // INTERACTIVE CIRCLE CENTER
        // ------------------------------------------------------------

        constexpr double kInteractionPi =
            3.14159265358979323846;

        double interactionPhase =
            static_cast<double>(
                GetTickCount64() % 800) /
            800.0;

        double interactionWave =
            0.5 -
            0.5 * std::cos(
                      interactionPhase *
                      2.0 *
                      kInteractionPi);

        Gdiplus::REAL centerX =
            ringLeft + diameter / 2.0f;

        Gdiplus::REAL centerY =
            ringTop + diameter / 2.0f;

        if (!paused && !hovered)
        {
            // Normal running state: use the original clean presentation.
            graphics.DrawString(
                percentageText,
                -1,
                selectedPercentageFont,
                percentageBounds,
                &centeredText,
                &primaryBrush);

            if (ActiveElements().showCompleteLabel)
            {
                graphics.DrawString(
                    L"Complete",
                    -1,
                    selectedLabelFont,
                    labelBounds,
                    &centeredText,
                    &secondaryBrush);
            }
        }
        else if (!paused && hovered)
        {
            // Hover state: show only the Pause affordance.
            Gdiplus::REAL iconScale =
                static_cast<Gdiplus::REAL>(
                    1.0 +
                    interactionWave * 0.08);

            Gdiplus::REAL halfHeight =
                static_cast<Gdiplus::REAL>(
                    ScaleForDpi(20, dpi)) *
                iconScale;

            Gdiplus::REAL separation =
                static_cast<Gdiplus::REAL>(
                    ScaleForDpi(9, dpi)) *
                iconScale;

            BYTE outerGlowAlpha =
                static_cast<BYTE>(
                    22 +
                    interactionWave * 14);

            BYTE innerGlowAlpha =
                static_cast<BYTE>(
                    44 +
                    interactionWave * 22);

            BYTE foregroundAlpha =
                static_cast<BYTE>(
                    165 +
                    interactionWave * 18);

            Gdiplus::Pen pauseOuterGlow(
                Gdiplus::Color(
                    outerGlowAlpha,
                    GetRValue(theme.accent),
                    GetGValue(theme.accent),
                    GetBValue(theme.accent)),
                static_cast<Gdiplus::REAL>(
                    ScaleForDpi(15, dpi)));

            pauseOuterGlow.SetStartCap(
                Gdiplus::LineCapRound);
            pauseOuterGlow.SetEndCap(
                Gdiplus::LineCapRound);

            Gdiplus::Pen pauseInnerGlow(
                Gdiplus::Color(
                    innerGlowAlpha,
                    GetRValue(theme.primaryText),
                    GetGValue(theme.primaryText),
                    GetBValue(theme.primaryText)),
                static_cast<Gdiplus::REAL>(
                    ScaleForDpi(12, dpi)));

            pauseInnerGlow.SetStartCap(
                Gdiplus::LineCapRound);
            pauseInnerGlow.SetEndCap(
                Gdiplus::LineCapRound);

            Gdiplus::Pen pausePen(
                Gdiplus::Color(
                    foregroundAlpha,
                    GetRValue(theme.primaryText),
                    GetGValue(theme.primaryText),
                    GetBValue(theme.primaryText)),
                static_cast<Gdiplus::REAL>(
                    ScaleForDpi(10, dpi)));

            pausePen.SetStartCap(
                Gdiplus::LineCapRound);
            pausePen.SetEndCap(
                Gdiplus::LineCapRound);

            auto drawPauseBars =
                [&](Gdiplus::Pen *pen)
            {
                graphics.DrawLine(
                    pen,
                    centerX - separation,
                    centerY - halfHeight,
                    centerX - separation,
                    centerY + halfHeight);

                graphics.DrawLine(
                    pen,
                    centerX + separation,
                    centerY - halfHeight,
                    centerX + separation,
                    centerY + halfHeight);
            };

            drawPauseBars(&pauseOuterGlow);
            drawPauseBars(&pauseInnerGlow);
            drawPauseBars(&pausePen);
        }
        else
        {
            // Paused operation.
            // Away from the mouse this is a STATE indicator:
            // Pause icon + "Paused". Hover changes the whole center
            // immediately into the available ACTION: Play + "Resume".
            if (!hovered)
            {
                Gdiplus::REAL iconScale =
                    static_cast<Gdiplus::REAL>(
                        0.88 + interactionWave * 0.16);

                Gdiplus::REAL pauseCenterY =
                    centerY -
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(5, dpi));

                Gdiplus::REAL halfHeight =
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(18, dpi)) *
                    iconScale;

                Gdiplus::REAL barOffset =
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(7, dpi)) *
                    iconScale;

                BYTE outerGlowAlpha =
                    static_cast<BYTE>(
                        22 + interactionWave * 30);

                Gdiplus::Pen pauseOuterGlow(
                    Gdiplus::Color(
                        outerGlowAlpha,
                        GetRValue(theme.accent),
                        GetGValue(theme.accent),
                        GetBValue(theme.accent)),
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(10, dpi)));

                pauseOuterGlow.SetStartCap(
                    Gdiplus::LineCapRound);
                pauseOuterGlow.SetEndCap(
                    Gdiplus::LineCapRound);

                Gdiplus::Pen pausePen(
                    Gdiplus::Color(
                        225,
                        GetRValue(theme.primaryText),
                        GetGValue(theme.primaryText),
                        GetBValue(theme.primaryText)),
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(7, dpi)));

                pausePen.SetStartCap(
                    Gdiplus::LineCapRound);
                pausePen.SetEndCap(
                    Gdiplus::LineCapRound);

                auto drawPausedBars =
                    [&](Gdiplus::Pen *pen)
                {
                    graphics.DrawLine(
                        pen,
                        centerX - barOffset,
                        pauseCenterY - halfHeight,
                        centerX - barOffset,
                        pauseCenterY + halfHeight);

                    graphics.DrawLine(
                        pen,
                        centerX + barOffset,
                        pauseCenterY - halfHeight,
                        centerX + barOffset,
                        pauseCenterY + halfHeight);
                };

                drawPausedBars(&pauseOuterGlow);
                drawPausedBars(&pausePen);

                if (ActiveElements().showCompleteLabel)
                {
                    Gdiplus::Font pausedFont(
                        type.circleLabelFont.c_str(),
                        static_cast<Gdiplus::REAL>(
                            ScaleForDpi(
                                type.circleLabelSize + 1,
                                dpi)),
                        Gdiplus::FontStyleBold,
                        Gdiplus::UnitPixel);

                    Gdiplus::Font pausedFallback(
                        L"Segoe UI",
                        static_cast<Gdiplus::REAL>(
                            ScaleForDpi(
                                type.circleLabelSize + 1,
                                dpi)),
                        Gdiplus::FontStyleBold,
                        Gdiplus::UnitPixel);

                    Gdiplus::Font *selectedPausedFont =
                        pausedFont.GetLastStatus() == Gdiplus::Ok
                            ? &pausedFont
                            : &pausedFallback;

                    Gdiplus::RectF pausedBounds(
                        ringLeft,
                        ringTop + diameter * 0.70f,
                        diameter,
                        static_cast<Gdiplus::REAL>(
                            ScaleForDpi(21, dpi)));

                    Gdiplus::SolidBrush pausedBrush(
                        Gdiplus::Color(
                            210,
                            GetRValue(theme.primaryText),
                            GetGValue(theme.primaryText),
                            GetBValue(theme.primaryText)));

                    graphics.DrawString(
                        L"Paused",
                        -1,
                        selectedPausedFont,
                        pausedBounds,
                        &centeredText,
                        &pausedBrush);
                }
            }
            else
            {
                // Hover is an immediate action preview.
                // No interpolation: icon and text change together.
                Gdiplus::REAL playScale =
                    static_cast<Gdiplus::REAL>(
                        0.94 + interactionWave * 0.12);

                Gdiplus::REAL playCenterY =
                    centerY -
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(5, dpi));

                Gdiplus::REAL halfHeight =
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(23, dpi)) *
                    playScale;

                Gdiplus::REAL halfWidth =
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(19, dpi)) *
                    playScale;

                Gdiplus::PointF playPoints[3] =
                    {
                        {centerX - halfWidth * 0.58f,
                         playCenterY - halfHeight},
                        {centerX - halfWidth * 0.58f,
                         playCenterY + halfHeight},
                        {centerX + halfWidth,
                         playCenterY}};

                Gdiplus::Pen playOuterGlow(
                    Gdiplus::Color(
                        static_cast<BYTE>(
                            32 + interactionWave * 28),
                        GetRValue(theme.accent),
                        GetGValue(theme.accent),
                        GetBValue(theme.accent)),
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(10, dpi)));

                playOuterGlow.SetLineJoin(
                    Gdiplus::LineJoinRound);

                graphics.DrawPolygon(
                    &playOuterGlow,
                    playPoints,
                    ARRAYSIZE(playPoints));

                Gdiplus::Pen playInnerGlow(
                    Gdiplus::Color(
                        static_cast<BYTE>(
                            60 + interactionWave * 28),
                        GetRValue(theme.primaryText),
                        GetGValue(theme.primaryText),
                        GetBValue(theme.primaryText)),
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(5, dpi)));

                playInnerGlow.SetLineJoin(
                    Gdiplus::LineJoinRound);

                graphics.DrawPolygon(
                    &playInnerGlow,
                    playPoints,
                    ARRAYSIZE(playPoints));

                Gdiplus::SolidBrush playBrush(
                    Gdiplus::Color(
                        230,
                        GetRValue(theme.primaryText),
                        GetGValue(theme.primaryText),
                        GetBValue(theme.primaryText)));

                graphics.FillPolygon(
                    &playBrush,
                    playPoints,
                    ARRAYSIZE(playPoints));

                Gdiplus::Pen playRoundedEdge(
                    Gdiplus::Color(
                        230,
                        GetRValue(theme.primaryText),
                        GetGValue(theme.primaryText),
                        GetBValue(theme.primaryText)),
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(4, dpi)));

                playRoundedEdge.SetLineJoin(
                    Gdiplus::LineJoinRound);

                graphics.DrawPolygon(
                    &playRoundedEdge,
                    playPoints,
                    ARRAYSIZE(playPoints));

                Gdiplus::Pen playHighlight(
                    Gdiplus::Color(
                        110,
                        255,
                        255,
                        255),
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(1, dpi)));

                playHighlight.SetLineJoin(
                    Gdiplus::LineJoinRound);

                graphics.DrawPolygon(
                    &playHighlight,
                    playPoints,
                    ARRAYSIZE(playPoints));

                if (ActiveElements().showCompleteLabel)
                {
                    Gdiplus::Font resumeFont(
                        type.circleLabelFont.c_str(),
                        static_cast<Gdiplus::REAL>(
                            ScaleForDpi(
                                type.circleLabelSize + 1,
                                dpi)),
                        Gdiplus::FontStyleBold,
                        Gdiplus::UnitPixel);

                    Gdiplus::Font resumeFallback(
                        L"Segoe UI",
                        static_cast<Gdiplus::REAL>(
                            ScaleForDpi(
                                type.circleLabelSize + 1,
                                dpi)),
                        Gdiplus::FontStyleBold,
                        Gdiplus::UnitPixel);

                    Gdiplus::Font *selectedResumeFont =
                        resumeFont.GetLastStatus() == Gdiplus::Ok
                            ? &resumeFont
                            : &resumeFallback;

                    Gdiplus::RectF resumeBounds(
                        ringLeft,
                        ringTop + diameter * 0.70f,
                        diameter,
                        static_cast<Gdiplus::REAL>(
                            ScaleForDpi(21, dpi)));

                    Gdiplus::SolidBrush resumeBrush(
                        Gdiplus::Color(
                            210,
                            GetRValue(theme.primaryText),
                            GetGValue(theme.primaryText),
                            GetBValue(theme.primaryText)));

                    graphics.DrawString(
                        L"Resume",
                        -1,
                        selectedResumeFont,
                        resumeBounds,
                        &centeredText,
                        &resumeBrush);
                }
            }
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

        // Normal child-window painting starts at 0,0. A Glass host can contain
        // several operation tiles, so render each logical panel at the origin
        // supplied by its host slot and keep its drawing inside that slot.
        graphics.TranslateTransform(
            static_cast<Gdiplus::REAL>(clientRect.left),
            static_cast<Gdiplus::REAL>(clientRect.top));

        graphics.SetClip(
            Gdiplus::RectF(
                0.0f,
                0.0f,
                static_cast<Gdiplus::REAL>(width),
                static_cast<Gdiplus::REAL>(height)),
            Gdiplus::CombineModeReplace);

        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        graphics.SetTextRenderingHint(
            Gdiplus::TextRenderingHintClearTypeGridFit);

        Gdiplus::SolidBrush backgroundBrush(Gdiplus::Color(
            255, GetRValue(theme.background), GetGValue(theme.background),
            GetBValue(theme.background)));
        if (!g_glassTransparentInfoPanelPaint)
        {
            graphics.FillRectangle(
                &backgroundBrush,
                0,
                0,
                width,
                height);
        }

        DrawEmbeddedProgressCircle(
            graphics,
            dpi,
            snapshot.displayedOverallPercent,
            theme,
            type,
            snapshot.paused,
            snapshot.circleHovered);

        int effectiveBodySize =
            type.bodySize +
            (g_glassTransparentInfoPanelPaint ? 1 : 0);

        PCWSTR effectiveBodyFont =
            g_glassTransparentInfoPanelPaint
                ? L"Segoe UI Semibold"
                : type.bodyFont.c_str();

        Gdiplus::Font detailFont(
            effectiveBodyFont,
            static_cast<Gdiplus::REAL>(ScaleForDpi(effectiveBodySize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font detailFallback(
            L"Segoe UI",
            static_cast<Gdiplus::REAL>(ScaleForDpi(effectiveBodySize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font *selectedFont =
            detailFont.GetLastStatus() == Gdiplus::Ok
                ? &detailFont
                : &detailFallback;

        Gdiplus::SolidBrush primaryBrush(Gdiplus::Color(
            255, GetRValue(theme.primaryText), GetGValue(theme.primaryText),
            GetBValue(theme.primaryText)));
        COLORREF infoSecondaryText =
            IsGlassTheme() ? RGB(184, 202, 218) : theme.secondaryText;

        Gdiplus::SolidBrush secondaryBrush(Gdiplus::Color(
            255, GetRValue(infoSecondaryText),
            GetGValue(infoSecondaryText), GetBValue(infoSecondaryText)));
        Gdiplus::SolidBrush linkBrush(Gdiplus::Color(
            255, GetRValue(theme.accent),
            GetGValue(theme.accent), GetBValue(theme.accent)));
        Gdiplus::Font linkFont(
            effectiveBodyFont,
            static_cast<Gdiplus::REAL>(ScaleForDpi(effectiveBodySize, dpi)),
            Gdiplus::FontStyleUnderline, Gdiplus::UnitPixel);
        Gdiplus::Font linkFallback(
            L"Segoe UI",
            static_cast<Gdiplus::REAL>(ScaleForDpi(effectiveBodySize, dpi)),
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
        int detailLineReserve = ScaleForDpi(12, dpi);

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
                    detailsOffset - ScaleForDpi(8, dpi)),
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
                detailsOffset + detailLineReserve +
                ScaleForDpi(kInfoPanelSpeedTop, dpi));
            drawInlineSegment(L"Speed: ", &secondaryBrush, speedY, &speedX);
            drawInlineSegment(rateValue, &primaryBrush, speedY, &speedX);
            drawInlineSegment(L"  \x2022  ", &secondaryBrush, speedY,
                              &speedX);
            drawInlineSegment(snapshot.timeRemainingLabel.c_str(),
                              &secondaryBrush, speedY, &speedX);
            drawInlineSegment(L" ", &secondaryBrush, speedY, &speedX);
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
                detailsOffset + detailLineReserve +
                ScaleForDpi(kInfoPanelItemsTop, dpi));
            drawInlineSegment(snapshot.itemsRemainingLabel.c_str(),
                              &secondaryBrush, itemsY, &itemsX);
            drawInlineSegment(L" ", &secondaryBrush, itemsY, &itemsX);
            drawInlineSegment(itemsValue, &primaryBrush, itemsY, &itemsX);
        }

        if (elements.showProgressBar &&
            g_showCurrentFileProgressBar)
        {
            Gdiplus::REAL progressTop = static_cast<Gdiplus::REAL>(
                detailsOffset + detailLineReserve +
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
                    std::clamp(
                        snapshot.currentFileProgressValid
                            ? snapshot.displayedCurrentFilePercent
                            : static_cast<double>(snapshot.percent),
                        0.0, 100.0)) /
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
        if (!g_settings.hideSmallControlButtons)
        {
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
        }

        if (!g_settings.hideSmallControlButtons &&
            elements.showCancel)
        {
            RECT cancelRect{};
            GetInfoPanelCancelRect(infoWindow, &cancelRect);
            Gdiplus::REAL cancelCenterX =
                static_cast<Gdiplus::REAL>(
                    cancelRect.left + cancelRect.right) /
                2.0f;
            Gdiplus::REAL cancelCenterY =
                static_cast<Gdiplus::REAL>(
                    cancelRect.top + cancelRect.bottom) /
                2.0f;
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
            int yPixels =
                chartTop + (chartHeight * row) / 4;
            Gdiplus::REAL y =
                static_cast<Gdiplus::REAL>(yPixels);
            graphics.DrawLine(&gridPen, 0.0f, y,
                              static_cast<Gdiplus::REAL>(chartWidth), y);
        }
        for (int column = 1; column < 6; ++column)
        {
            int xPixels =
                (chartWidth * column) / 6;
            Gdiplus::REAL x =
                static_cast<Gdiplus::REAL>(xPixels);
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

            int graphValueSize =
                type.graphValueSize + (IsGlassTheme() ? 1 : 0);
            Gdiplus::FontStyle graphValueStyle =
                IsGlassTheme() ? Gdiplus::FontStyleBold
                               : Gdiplus::FontStyleRegular;

            Gdiplus::Font graphValueFont(
                type.bodyFont.c_str(),
                static_cast<Gdiplus::REAL>(ScaleForDpi(graphValueSize, dpi)),
                graphValueStyle, Gdiplus::UnitPixel);
            Gdiplus::Font graphValueFallback(
                L"Segoe UI",
                static_cast<Gdiplus::REAL>(ScaleForDpi(graphValueSize, dpi)),
                graphValueStyle, Gdiplus::UnitPixel);
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

        if (g_settings.hideSmallControlButtons)
        {
            return;
        }

        RECT clientRect{};
        HWND geometryWindow = infoWindow;
        if (g_glassTransparentInfoPanelPaint)
        {
            HWND hostWindow = GetAncestor(infoWindow, GA_ROOT);
            if (hostWindow && IsWindow(hostWindow))
            {
                geometryWindow = hostWindow;
            }
        }
        UINT dpi = GetDpiForWindow(geometryWindow);
        if (!dpi || !GetClientRect(geometryWindow, &clientRect) ||
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

        if (g_settings.hideSmallControlButtons)
        {
            return;
        }

        RECT clientRect{};
        HWND geometryWindow = infoWindow;
        if (g_glassTransparentInfoPanelPaint)
        {
            HWND hostWindow = GetAncestor(infoWindow, GA_ROOT);
            if (hostWindow && IsWindow(hostWindow))
            {
                geometryWindow = hostWindow;
            }
        }
        UINT dpi = GetDpiForWindow(geometryWindow);
        if (!dpi || !GetClientRect(geometryWindow, &clientRect))
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

    void GetInfoPanelCircleActionRect(
        HWND infoWindow,
        RECT *circleRect)
    {
        if (!circleRect)
        {
            return;
        }

        SetRectEmpty(circleRect);

        if (!ActiveElements().showCircle ||
            !infoWindow ||
            !IsWindow(infoWindow))
        {
            return;
        }

        HWND geometryWindow = infoWindow;
        if (g_glassTransparentInfoPanelPaint)
        {
            HWND hostWindow = GetAncestor(infoWindow, GA_ROOT);
            if (hostWindow && IsWindow(hostWindow))
            {
                geometryWindow = hostWindow;
            }
        }

        UINT dpi = GetDpiForWindow(geometryWindow);
        if (!dpi)
        {
            dpi = USER_DEFAULT_SCREEN_DPI;
        }

        int diameter = ScaleForDpi(kCircleDiameter, dpi);
        int columnWidth = ScaleForDpi(kCircleColumnWidth, dpi);
        int ringLeft =
            (columnWidth - diameter) / 2 +
            ScaleForDpi(ActiveLayout().circleXOffset, dpi);
        int ringTop =
            ScaleForDpi(kCircleHostY + kCircleTop, dpi);

        // Slightly enlarge the hit target beyond the visible ring so the
        // circle feels easy to click without changing its appearance.
        int hitPadding = ScaleForDpi(6, dpi);

        circleRect->left = ringLeft - hitPadding;
        circleRect->top = ringTop - hitPadding;
        circleRect->right = ringLeft + diameter + hitPadding;
        circleRect->bottom = ringTop + diameter + hitPadding;
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

    bool InvokeDirectUiButtonDefaultAction(DirectUI::Element *element,
                                           PCWSTR actionName)
    {
        if (!element)
        {
            return false;
        }

        HMODULE dui70 = GetModuleHandleW(L"dui70.dll");
        if (!dui70)
        {
            Wh_Log(L"custom action=%s dui70.dll not loaded", actionName);
            return false;
        }

        FARPROC buttonGetClassInfoExport = GetProcAddress(
            dui70, "?GetClassInfoW@Button@DirectUI@@UEAAPEAUIClassInfo@2@XZ");
        auto buttonVtable = reinterpret_cast<void **>(GetProcAddress(
            dui70, "??_7Button@DirectUI@@6B@"));

        constexpr size_t kMaxButtonVtableSlots = 64;
        size_t getClassInfoSlot = kMaxButtonVtableSlots;
        for (size_t i = 0;
             buttonGetClassInfoExport && buttonVtable &&
             i < kMaxButtonVtableSlots;
             ++i)
        {
            if (buttonVtable[i] ==
                reinterpret_cast<void *>(buttonGetClassInfoExport))
            {
                getClassInfoSlot = i;
                break;
            }
        }

        using ButtonGetClassInfoPtr_t = DirectUI::IClassInfo *(*)();
        using ElementGetClassInfo_t =
            DirectUI::IClassInfo *(*)(DirectUI::Element *);
        using ClassInfoIsSubclassOf_t =
            bool (*)(DirectUI::IClassInfo const *, DirectUI::IClassInfo *);
        using ButtonDefaultAction_t = long (*)(DirectUI::Element *);

        auto buttonGetClassInfoPtr =
            reinterpret_cast<ButtonGetClassInfoPtr_t>(GetProcAddress(
                dui70,
                "?GetClassInfoPtr@Button@DirectUI@@SAPEAUIClassInfo@2@XZ"));
        auto classInfoIsSubclassOf =
            reinterpret_cast<ClassInfoIsSubclassOf_t>(GetProcAddress(
                dui70,
                "?IsSubclassOf@ClassInfoBase@DirectUI@@UEBA_NPEAUIClassInfo@2@@Z"));
        auto buttonDefaultAction =
            reinterpret_cast<ButtonDefaultAction_t>(GetProcAddress(
                dui70, "?DefaultAction@Button@DirectUI@@UEAAJXZ"));

        if (getClassInfoSlot == kMaxButtonVtableSlots ||
            !buttonGetClassInfoPtr || !classInfoIsSubclassOf ||
            !buttonDefaultAction)
        {
            Wh_Log(L"custom action=%s DirectUI Button validation exports "
                   L"unavailable",
                   actionName);
            return false;
        }

        auto elementVtable = *reinterpret_cast<void ***>(element);
        auto elementGetClassInfo =
            elementVtable
                ? reinterpret_cast<ElementGetClassInfo_t>(
                      elementVtable[getClassInfoSlot])
                : nullptr;
        DirectUI::IClassInfo *elementClassInfo =
            elementGetClassInfo ? elementGetClassInfo(element) : nullptr;
        DirectUI::IClassInfo *buttonClassInfo = buttonGetClassInfoPtr();
        if (!elementClassInfo || !buttonClassInfo ||
            !classInfoIsSubclassOf(elementClassInfo, buttonClassInfo))
        {
            Wh_Log(L"custom action=%s rejected: native element is not a "
                   L"DirectUI::Button",
                   actionName);
            return false;
        }

        // Preserve Explorer's semantic action mechanism after fail-closed
        // runtime class validation. Never synthesize mouse input here.
        long result = buttonDefaultAction(element);

        return result >= 0;
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

        return InvokeDirectUiButtonDefaultAction(actionElement, actionName);
    }

    void SetTilePausedPresentationState(
        OperationTileElement *tile,
        bool paused)
    {
        HWND infoWindow = nullptr;
        bool hovered = false;

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
            hovered = it->circleHovered;
        }

        if (infoWindow && IsWindow(infoWindow))
        {
            if (paused)
            {
                StopCurrentFileAnimation(infoWindow);

                // Paused is an intentionally animated state. Keep the
                // breathing effect active until the operation resumes.
                SetTimer(
                    infoWindow,
                    kCircleHoverAnimationTimer,
                    hovered
                        ? kCircleHoverAnimationIntervalMs
                        : kPausedBreathingIntervalMs,
                    nullptr);
            }
            else
            {
                // Hover still owns the animation timer while the pointer
                // remains over the interactive circle.
                if (hovered)
                {
                    SetTimer(
                        infoWindow,
                        kCircleHoverAnimationTimer,
                        kCircleHoverAnimationIntervalMs,
                        nullptr);
                }
                else
                {
                    KillTimer(
                        infoWindow,
                        kCircleHoverAnimationTimer);
                }

                PostMessageW(
                    infoWindow,
                    kCurrentFileAnimationMessage,
                    0,
                    0);
            }

            InvalidateInfoPanelForTile(tile, false);
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
            // The native DirectUI action has been accepted by Explorer.
            // Mirror the resulting Pause / Resume state in our presentation.
            SetTilePausedPresentationState(tile, !wasPaused);
        }

        return invoked;
    }

    void StopCurrentFileAnimation(HWND infoWindow)
    {
        // Called only by the owning window thread, including teardown.
        KillTimer(infoWindow, kCurrentFileAnimationTimer);
        std::lock_guard<std::mutex> lock(g_circleMutex);
        for (CircleState &state : g_circles)
        {
            if (state.infoWindow == infoWindow)
            {
                state.currentFileAnimation.timerRunning = false;
                break;
            }
        }
    }

    double AdvanceProgressChase(double displayed,
                                double target,
                                double elapsedMs)
    {
        target = std::clamp(target, 0.0, 100.0);

        // Progress must never visually run ahead of the most recent real value.
        if (displayed > target)
        {
            return target;
        }

        if (displayed >= target)
        {
            return displayed;
        }

        // Keep following the real value instead of completing a short fixed
        // animation and waiting. The elapsed-time calculation keeps motion
        // consistent even when WM_TIMER delivery isn't perfectly regular.
        double chaseAmount =
            std::clamp(elapsedMs / 140.0, 0.0, 0.30);

        double next =
            displayed + (target - displayed) * chaseAmount;

        // Stop chasing once the remaining difference is smaller than a
        // visible fraction of a pixel. This prevents the animation timer
        // from running after the progress is visually complete.
        if (target - next < 0.05)
        {
            next = target;
        }

        return std::min(next, target);
    }

    void AdvanceCurrentFileAnimation(CurrentFileAnimation &animation,
                                     unsigned long long fileStartBytes,
                                     unsigned long long fileSize,
                                     double currentFileTarget,
                                     double overallTarget,
                                     bool animateCurrentFile,
                                     bool animateOverall,
                                     ULONGLONG now)
    {
        double elapsedMs = 16.0;

        if (animation.lastTick != 0 && now >= animation.lastTick)
        {
            elapsedMs = static_cast<double>(
                std::min<ULONGLONG>(
                    now - animation.lastTick,
                    50));
        }

        animation.lastTick = now;

        if (animateOverall)
        {
            overallTarget =
                std::clamp(overallTarget, 0.0, 100.0);

            if (!animation.overallInitialized)
            {
                animation.overallInitialized = true;
                animation.displayedOverallPercent =
                    std::min(
                        animation.displayedOverallPercent,
                        overallTarget);
            }

            animation.targetOverallPercent = overallTarget;

            animation.displayedOverallPercent =
                AdvanceProgressChase(
                    animation.displayedOverallPercent,
                    animation.targetOverallPercent,
                    elapsedMs);
        }

        if (animateCurrentFile)
        {
            currentFileTarget =
                std::clamp(currentFileTarget, 0.0, 100.0);

            bool newFile =
                !animation.identityValid ||
                animation.fileStartBytes != fileStartBytes ||
                animation.fileSize != fileSize;

            if (newFile)
            {
                animation.identityValid = true;
                animation.fileStartBytes = fileStartBytes;
                animation.fileSize = fileSize;
                animation.displayedPercent = 0.0;
            }

            animation.targetPercent = currentFileTarget;

            animation.displayedPercent =
                AdvanceProgressChase(
                    animation.displayedPercent,
                    animation.targetPercent,
                    elapsedMs);
        }
        else
        {
            animation.identityValid = false;
            animation.displayedPercent = 0.0;
            animation.targetPercent = 0.0;
        }
    }

    void UpdateCurrentFileAnimation(HWND infoWindow, bool timerTick)
    {
        CircleState circle{};
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [infoWindow](CircleState const &state)
                { return state.infoWindow == infoWindow; });
            if (it == g_circles.end() || !it->tile)
            {
                KillTimer(infoWindow, kCurrentFileAnimationTimer);
                return;
            }
            circle = *it;
        }

        // KillTimer doesn't remove already queued WM_TIMER messages.
        if (timerTick && !circle.currentFileAnimation.timerRunning)
        {
            return;
        }

        bool animateCurrentFile =
            g_showCurrentFileProgressBar &&
            ActiveElements().showProgressBar;

        bool animateOverall =
            ActiveElements().showCircle;

        if (g_unloading.load(std::memory_order_acquire) ||
            (!animateCurrentFile && !animateOverall) ||
            circle.paused ||
            IsHostInSpecialOperationState(circle.hostWindow))
        {
            StopCurrentFileAnimation(infoWindow);
            return;
        }

        bool currentFileValid = false;
        unsigned long long fileStartBytes = 0;
        unsigned long long fileSize = 0;
        double currentFileTarget = 0.0;

        double overallTarget =
            static_cast<double>(
                std::clamp(circle.progressPercent, 0, 100));

        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(),
                g_transferSummaries.end(),
                [&circle](TransferSummaryState const &state)
                { return state.tile == circle.tile; });

            if (it != g_transferSummaries.end())
            {
                currentFileValid =
                    it->currentFileProgressValid &&
                    it->currentFileSize > 0;

                fileStartBytes = it->currentFileStartBytes;
                fileSize = it->currentFileSize;

                if (currentFileValid)
                {
                    long double rawCurrentFileTarget =
                        static_cast<long double>(
                            it->currentFileCompletedBytes) *
                        100.0L /
                        static_cast<long double>(
                            it->currentFileSize);

                    currentFileTarget =
                        static_cast<double>(
                            std::clamp<long double>(
                                rawCurrentFileTarget,
                                0.0L, 100.0L));
                }

                // Copy/Move overall progress follows transferred bytes.
                // Delete/Recycle must keep Explorer's native overall percent:
                // its byte accounting can reach the total before the actual
                // delete operation has finished.
                if (!(it->deleteLikeKnown && it->deleteLike) &&
                    it->bytesValid &&
                    it->totalBytes > 0)
                {
                    long double rawOverallTarget =
                        static_cast<long double>(
                            it->completedBytes) *
                        100.0L /
                        static_cast<long double>(
                            it->totalBytes);

                    overallTarget =
                        static_cast<double>(
                            std::clamp<long double>(
                                rawOverallTarget,
                                0.0L, 100.0L));
                }
            }
        }

        CurrentFileAnimation animation =
            circle.currentFileAnimation;

        bool wasRunning = animation.timerRunning;

        AdvanceCurrentFileAnimation(
            animation,
            fileStartBytes,
            fileSize,
            currentFileTarget,
            overallTarget,
            animateCurrentFile && currentFileValid,
            animateOverall,
            GetTickCount64());

        bool currentFileNeedsTimer =
            animateCurrentFile &&
            currentFileValid &&
            animation.displayedPercent <
                animation.targetPercent;

        bool overallNeedsTimer =
            animateOverall &&
            animation.displayedOverallPercent <
                animation.targetOverallPercent;

        animation.timerRunning =
            currentFileNeedsTimer ||
            overallNeedsTimer;

        if (animation.timerRunning &&
            !wasRunning &&
            !SetTimer(
                infoWindow,
                kCurrentFileAnimationTimer,
                16,
                nullptr))
        {
            // Resource failure falls back to truthful static progress.
            animation.displayedPercent =
                animation.targetPercent;
            animation.displayedOverallPercent =
                animation.targetOverallPercent;
            animation.timerRunning = false;
        }

        if (!animation.timerRunning)
        {
            KillTimer(
                infoWindow,
                kCurrentFileAnimationTimer);
        }

        {
            std::lock_guard<std::mutex> lock(g_circleMutex);

            auto it = std::find_if(
                g_circles.begin(),
                g_circles.end(),
                [infoWindow, &circle](CircleState const &state)
                {
                    return state.infoWindow == infoWindow &&
                           state.tile == circle.tile;
                });

            if (it == g_circles.end())
            {
                KillTimer(
                    infoWindow,
                    kCurrentFileAnimationTimer);
                return;
            }

            it->currentFileAnimation = animation;
        }

        if (animation.displayedPercent !=
                circle.currentFileAnimation.displayedPercent ||
            animation.displayedOverallPercent !=
                circle.currentFileAnimation.displayedOverallPercent ||
            animation.identityValid !=
                circle.currentFileAnimation.identityValid ||
            animation.overallInitialized !=
                circle.currentFileAnimation.overallInitialized)
        {
            // Circle and current-file bar share one buffered presentation and
            // therefore stay visually synchronized.
            InvalidateInfoPanelForTile(
                circle.tile,
                false);
        }
    }

    LRESULT CALLBACK InfoPanelWindowProc(HWND window,
                                         UINT message,
                                         WPARAM wParam,
                                         LPARAM lParam)
    {
        if (message == WM_DESTROY ||
            (g_removeHostSubclassMessage &&
             message == g_removeHostSubclassMessage))
        {
            StopCurrentFileAnimation(window);
            KillTimer(window, kCircleHoverAnimationTimer);
        }
        if (g_removeHostSubclassMessage &&
            message == g_removeHostSubclassMessage)
        {
            return DestroyWindow(window) ? TRUE : FALSE;
        }
        if (g_unloading.load(std::memory_order_acquire))
        {
            StopCurrentFileAnimation(window);
            return DefWindowProcW(window, message, wParam, lParam);
        }

        switch (message)
        {
        case kCurrentFileAnimationMessage:
            UpdateCurrentFileAnimation(window, false);
            return 0;
        case WM_TIMER:
            if (wParam == kCurrentFileAnimationTimer)
            {
                UpdateCurrentFileAnimation(window, true);
                return 0;
            }

            if (wParam == kCircleHoverAnimationTimer)
            {
                OperationTileElement *tile = nullptr;
                bool hovered = false;
                bool paused = false;

                {
                    std::lock_guard<std::mutex> lock(g_circleMutex);

                    auto it = std::find_if(
                        g_circles.begin(),
                        g_circles.end(),
                        [window](CircleState const &state)
                        {
                            return state.infoWindow == window;
                        });

                    if (it != g_circles.end())
                    {
                        tile = it->tile;
                        hovered = it->circleHovered;
                        paused =
                            it->pausedStateKnown &&
                            it->paused;
                    }
                }

                if (!hovered && !paused)
                {
                    KillTimer(
                        window,
                        kCircleHoverAnimationTimer);
                    return 0;
                }

                if (tile)
                {
                    InvalidateInfoPanelForTile(tile, false);
                }

                return 0;
            }
            break;
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
            if (IsHostInSpecialOperationState(
                    GetAncestor(window, GA_ROOT)))
            {
                return HTTRANSPARENT;
            }
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

            RECT circleRect{};
            GetInfoPanelCircleActionRect(window, &circleRect);
            if (PtInRect(&circleRect, point))
            {
                InvokeNativeActionFromInfoPanel(
                    window,
                    L"eltPauseButton",
                    L"circle-pause-resume");
                return 0;
            }

            return 0;
        }
        case WM_MOUSEMOVE:
        {
            TRACKMOUSEEVENT tracking{};
            tracking.cbSize = sizeof(tracking);
            tracking.dwFlags = TME_LEAVE;
            tracking.hwndTrack = window;
            TrackMouseEvent(&tracking);

            POINT point{
                static_cast<short>(LOWORD(lParam)),
                static_cast<short>(HIWORD(lParam))};
            RECT circleRect{};
            GetInfoPanelCircleActionRect(window, &circleRect);

            SetCircleHoverState(
                window,
                PtInRect(&circleRect, point) != FALSE);
            break;
        }
        case WM_MOUSELEAVE:
            SetCircleHoverState(window, false);
            return 0;
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
                RECT circleRect{};
                GetInfoPanelCircleActionRect(window, &circleRect);

                bool overCircle =
                    PtInRect(&circleRect, point) != FALSE;

                SetCircleHoverState(window, overCircle);

                if (PtInRect(&cancelRect, point) ||
                    PtInRect(&pauseRect, point) ||
                    overCircle)
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

    struct FooterOverlaySnapshot
    {
        bool expanded = false;
        std::wstring displayModeLabel;
        std::wstring cancelLabel;
    };

    bool GetFooterOverlaySnapshot(OperationTileElement *tile,
                                  FooterOverlaySnapshot *snapshot)
    {
        if (!tile || !snapshot)
        {
            return false;
        }

        TransferSummaryState stateCopy{};
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [tile](TransferSummaryState const &state)
                { return state.tile == tile; });
            if (it == g_transferSummaries.end() || !it->displayModeKnown)
            {
                return false;
            }
            stateCopy = *it;
        }

        snapshot->expanded = stateCopy.expanded;
        DirectUI::Element *displayModeButton =
            FindSkinElementWithAncestorFallback(
                stateCopy.operationTileRoot, stateCopy.tileHeaderRoot,
                stateCopy.operationTileRoot, L"eltDisplayModeBtn", true);
        snapshot->displayModeLabel = ReadDirectUiText(displayModeButton);
        if (snapshot->displayModeLabel.empty())
        {
            snapshot->displayModeLabel =
                snapshot->expanded ? L"Fewer details" : L"More details";
        }

        DirectUI::Element *cancelButton = FindSkinElement(
            stateCopy.operationTileRoot, stateCopy.tileHeaderRoot,
            L"eltCancelButton", false);
        snapshot->cancelLabel = ReadDirectUiText(cancelButton);
        if (snapshot->cancelLabel.empty())
        {
            snapshot->cancelLabel = L"Cancel";
        }
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
                                RECT const &clientRect,
                                bool glassHostPaint = false)
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

        FooterOverlaySnapshot snapshot{};
        if (!GetFooterOverlaySnapshot(
                GetFooterOverlayTile(footerWindow), &snapshot))
        {
            snapshot.displayModeLabel = L"More details";
            snapshot.cancelLabel = L"Cancel";
        }

        Gdiplus::Graphics graphics(deviceContext);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics.SetTextRenderingHint(
            Gdiplus::TextRenderingHintClearTypeGridFit);

        COLORREF footerBackground = theme.background;
        if (!glassHostPaint &&
            g_settings.customizationEnabled &&
            g_settings.preset == L"glass" &&
            g_settings.footerStyle == L"solid" &&
            HasCustomFooterColor())
        {
            footerBackground = CurrentFooterColor();
        }

        Gdiplus::SolidBrush backgroundBrush(Gdiplus::Color(
            255, GetRValue(footerBackground), GetGValue(footerBackground),
            GetBValue(footerBackground)));
        if (!glassHostPaint)
        {
            graphics.FillRectangle(&backgroundBrush, 0, 0, width, height);
        }

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

        COLORREF footerSecondaryText =
            IsGlassTheme() ? RGB(184, 202, 218) : theme.secondaryText;

        Gdiplus::SolidBrush secondaryBrush(Gdiplus::Color(
            255, GetRValue(footerSecondaryText),
            GetGValue(footerSecondaryText),
            GetBValue(footerSecondaryText)));
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
                                    255, GetRValue(footerSecondaryText),
                                    GetGValue(footerSecondaryText),
                                    GetBValue(footerSecondaryText)),
                                1.0f);

        if (snapshot.expanded)
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
            snapshot.displayModeLabel.c_str(),
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
            COLORREF buttonBorderColor =
                IsGlassTheme() ? RGB(92, 121, 145) : theme.actionBorder;
            Gdiplus::Pen buttonBorder(Gdiplus::Color(
                                          255, GetRValue(buttonBorderColor),
                                          GetGValue(buttonBorderColor),
                                          GetBValue(buttonBorderColor)),
                                      1.0f);
            graphics.FillRectangle(&buttonBrush, buttonBounds);
            graphics.DrawRectangle(&buttonBorder, buttonBounds);
            Gdiplus::StringFormat centered;
            centered.SetAlignment(Gdiplus::StringAlignmentCenter);
            centered.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            graphics.DrawString(snapshot.cancelLabel.c_str(), -1,
                                selectedFont, buttonBounds, &centered,
                                &actionTextBrush);
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

        return InvokeDirectUiButtonDefaultAction(
            displayModeButton, L"more-fewer-details");
    }

    LRESULT CALLBACK FooterOverlayWindowProc(HWND window,
                                             UINT message,
                                             WPARAM wParam,
                                             LPARAM lParam)
    {
        if (g_removeHostSubclassMessage &&
            message == g_removeHostSubclassMessage)
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
            if (IsHostInSpecialOperationState(
                    GetAncestor(window, GA_ROOT)))
            {
                return HTTRANSPARENT;
            }
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

    bool GetGlassFooterBounds(HWND hostWindow,
                              HWND *footerWindow,
                              RECT *hostBounds)
    {
        if (!UseGlassFooter() || IsHostInSpecialOperationState(hostWindow))
        {
            return false;
        }

        HWND footer = GetFooterOverlayWindow(hostWindow);
        RECT screenBounds{};
        if (!footer || !GetWindowRect(footer, &screenBounds))
        {
            return false;
        }
        POINT origin{screenBounds.left, screenBounds.top};
        if (!ScreenToClient(hostWindow, &origin))
        {
            return false;
        }
        *footerWindow = footer;
        *hostBounds = {origin.x, origin.y,
                       origin.x + screenBounds.right - screenBounds.left,
                       origin.y + screenBounds.bottom - screenBounds.top};
        return hostBounds->right > hostBounds->left &&
               hostBounds->bottom > hostBounds->top;
    }

    void DrawGlassHostFooter(HWND hostWindow, HDC deviceContext)
    {
        HWND footerWindow = nullptr;
        RECT bounds{};
        if (!GetGlassFooterBounds(hostWindow, &footerWindow, &bounds))
        {
            return;
        }

        int savedDc = SaveDC(deviceContext);
        if (!savedDc)
        {
            return;
        }
        if (SetViewportOrgEx(deviceContext, bounds.left, bounds.top, nullptr))
        {
            RECT local{0, 0, bounds.right - bounds.left,
                       bounds.bottom - bounds.top};
            if (IntersectClipRect(deviceContext, 0, 0,
                                  local.right, local.bottom) != ERROR)
            {
                DrawFooterOverlayFrame(
                    footerWindow, deviceContext, local, true);
            }
        }
        RestoreDC(deviceContext, savedDc);
    }

    bool RouteGlassFooterMouseMessage(HWND hostWindow,
                                      UINT message,
                                      WPARAM wParam,
                                      LPARAM lParam,
                                      LRESULT *result)
    {
        if (message != WM_LBUTTONUP && message != WM_SETCURSOR &&
            message != WM_MOUSEACTIVATE)
        {
            return false;
        }
        if (message != WM_LBUTTONUP && LOWORD(lParam) != HTCLIENT)
        {
            return false;
        }

        HWND footerWindow = nullptr;
        RECT bounds{};
        if (!GetGlassFooterBounds(hostWindow, &footerWindow, &bounds))
        {
            return false;
        }
        POINT point{};
        if (message == WM_LBUTTONUP)
        {
            point = {static_cast<short>(LOWORD(lParam)),
                     static_cast<short>(HIWORD(lParam))};
        }
        else if (!GetCursorPos(&point) ||
                 !ScreenToClient(hostWindow, &point))
        {
            return false;
        }
        if (!PtInRect(&bounds, point))
        {
            return false;
        }

        if (message == WM_LBUTTONUP)
        {
            lParam = MAKELPARAM(point.x - bounds.left, point.y - bounds.top);
        }
        else if (message == WM_SETCURSOR)
        {
            wParam = reinterpret_cast<WPARAM>(footerWindow);
        }
        // The hidden footer remains the existing command/geometry anchor.
        // Its procedure still handles hit testing, native commands, the hand
        // cursor and MA_NOACTIVATE. No transparent child surface is involved.
        *result = SendMessageW(footerWindow, message, wParam, lParam);
        return true;
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
        bool created = false;
        if (!footerWindow)
        {
            footerWindow = CreateWindowExW(
                WS_EX_NOACTIVATE | WS_EX_NOPARENTNOTIFY,
                kFooterOverlayWindowClass, nullptr,
                WS_CHILD | WS_CLIPSIBLINGS,
                0, overlayTop, overlayWidth, overlayHeight,
                hostWindow, nullptr, g_circleClassInstance, tile);
            created = footerWindow != nullptr;
        }
        if (!footerWindow)
        {
            return;
        }

        if (GetWindowLongPtrW(footerWindow, GWLP_USERDATA) !=
            reinterpret_cast<LONG_PTR>(tile))
        {
            SetWindowLongPtrW(
                footerWindow, GWLP_USERDATA,
                reinterpret_cast<LONG_PTR>(tile));
        }

        RECT oldBounds{};
        bool haveOldBounds = false;
        bool geometryChanged = true;
        RECT currentScreen{};
        if (GetWindowRect(footerWindow, &currentScreen))
        {
            POINT origin{currentScreen.left, currentScreen.top};
            if (ScreenToClient(hostWindow, &origin))
            {
                oldBounds = {origin.x, origin.y,
                             origin.x + currentScreen.right - currentScreen.left,
                             origin.y + currentScreen.bottom - currentScreen.top};
                haveOldBounds = true;
                geometryChanged =
                    origin.x != 0 || origin.y != overlayTop ||
                    oldBounds.right - oldBounds.left != overlayWidth ||
                    oldBounds.bottom - oldBounds.top != overlayHeight;
            }
        }

        bool glass = UseGlassFooter();
        bool isVisible = (GetWindowLongPtrW(footerWindow, GWL_STYLE) &
                          WS_VISIBLE) != 0;
        bool shouldShow = !glass;
        bool visibilityChanged = isVisible != shouldShow;
        if (!created && !geometryChanged && !visibilityChanged)
        {
            return;
        }

        if (geometryChanged || visibilityChanged)
        {
            UINT flags = SWP_NOACTIVATE | SWP_NOOWNERZORDER |
                         (glass ? SWP_HIDEWINDOW | SWP_NOZORDER
                                : SWP_SHOWWINDOW);
            if (!SetWindowPos(footerWindow, HWND_TOP,
                              0, overlayTop, overlayWidth, overlayHeight,
                              flags))
            {
                return;
            }
        }

        if (glass)
        {
            // The footer's Glass pixels live in the host. Repaint only the
            // old and new footer rectangles when placement actually changes.
            if (!IsHostInSpecialOperationState(hostWindow))
            {
                if (haveOldBounds && geometryChanged && !created)
                {
                    InvalidateRect(hostWindow, &oldBounds, FALSE);
                }
                RECT newBounds{0, overlayTop, overlayWidth,
                               overlayTop + overlayHeight};
                InvalidateRect(hostWindow, &newBounds, FALSE);
            }
        }
        else
        {
            InvalidateRect(footerWindow, nullptr, FALSE);
        }
    }

    void ForgetCircleWindow(HWND circleWindow);

    LRESULT CALLBACK ProgressCircleWindowProc(HWND window,
                                              UINT message,
                                              WPARAM wParam,
                                              LPARAM lParam)
    {
        if (g_removeHostSubclassMessage &&
            message == g_removeHostSubclassMessage)
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

        DWORD progressWindowThreadId =
            GetWindowThreadProcessId(progressWindow, nullptr);
        if (progressWindowThreadId != GetCurrentThreadId())
        {
            HWND hostWindow = GetAncestor(progressWindow, GA_ROOT);
            DWORD_PTR result = FALSE;
            if (hostWindow &&
                GetWindowThreadProcessId(hostWindow, nullptr) ==
                    progressWindowThreadId &&
                SendMessageTimeoutW(
                    hostWindow, g_removeHostSubclassMessage,
                    kRemoveProgressWindowSubclassCommand,
                    reinterpret_cast<LPARAM>(progressWindow),
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

        BOOL removed = RemoveWindowSubclass(
            progressWindow, NativeProgressWindowSubclassProc,
            kProgressWindowSubclassId);
        DWORD removeError = removed ? ERROR_SUCCESS : GetLastError();
        if (GetWindowSubclass(progressWindow,
                              NativeProgressWindowSubclassProc,
                              kProgressWindowSubclassId, &referenceData))
        {
            Wh_Log(L"Presentation teardown failed to remove progress "
                   L"subclass hwnd=%p error=%lu",
                   reinterpret_cast<void *>(progressWindow),
                   removeError);
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

    bool CircleStateRecordsHaveSameIdentity(CircleState const &candidate,
                                            CircleState const &state)
    {
        if (state.circleWindow)
        {
            return candidate.circleWindow == state.circleWindow;
        }
        if (state.progressWindow)
        {
            return candidate.progressWindow == state.progressWindow;
        }
        if (state.infoWindow)
        {
            return candidate.infoWindow == state.infoWindow;
        }
        return false;
    }

    void EraseCircleStateRecord(CircleState const &state)
    {
        std::lock_guard<std::mutex> lock(g_circleMutex);
        auto it = std::find_if(
            g_circles.begin(), g_circles.end(),
            [&state](CircleState const &candidate)
            {
                return CircleStateRecordsHaveSameIdentity(candidate, state);
            });
        if (it != g_circles.end())
        {
            g_circles.erase(it);
        }
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
                    return CircleStateRecordsHaveSameIdentity(candidate, state);
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
        if (!circleWindow)
        {
            return;
        }

        CircleState state{};
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [circleWindow](CircleState const &candidate)
                {
                    return candidate.circleWindow &&
                           candidate.circleWindow == circleWindow;
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
            else if (state.circleWindow)
            {
                ForgetCircleWindow(state.circleWindow);
            }
            else
            {
                EraseCircleStateRecord(state);
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

        // Explorer can use either the full "x / y (p%)" form or a transient
        // "x / y" form while returning from a native special state. This is
        // classification only; Explorer remains the owner of the caption.
        return wcsstr(text, L" / ") != nullptr;
    }

    bool IsKnownSpecialOperationCaption(PCWSTR caption)
    {
        if (!caption || !*caption)
        {
            return false;
        }

        // Non-authoritative English heuristics observed on this shell build.
        // DirectUI normal-presentation validation remains authoritative.
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

    void SetHostCaptionDrawingSuppressed(
        HWND hostWindow,
        bool suppressed)
    {
        if (!hostWindow || !IsWindow(hostWindow))
        {
            return;
        }

        WTA_OPTIONS options{};
        options.dwFlags =
            suppressed ? WTNCA_NODRAWCAPTION : 0;
        options.dwMask = WTNCA_NODRAWCAPTION;

        HRESULT result = SetWindowThemeAttribute(
            hostWindow,
            WTA_NONCLIENT,
            &options,
            sizeof(options));

        if (FAILED(result))
        {
            Wh_Log(
                L"Caption drawing update failed result=0x%08X hwnd=%p",
                static_cast<unsigned int>(result),
                reinterpret_cast<void *>(hostWindow));
        }
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
        if (!Element_GetVisible_Original ||
            !Element_SetVisible_Original ||
            !state.tile ||
            !state.operationTileRoot)
        {
            return;
        }

        auto find = [&state](PCWSTR name, bool headerFallback = false)
        {
            return FindSkinElement(
                state.operationTileRoot, state.tileHeaderRoot,
                name, headerFallback);
        };

        std::vector<DirectUI::Element *> elements{
            state.tileHeaderRoot,
            find(L"eltSummary"),
            find(L"eltDetails"),
            find(L"eltChartArea"),
            find(L"eltRateChart_New"),
            find(L"eltProgressBarContainer"),
            find(L"eltProgressBar")};

        if (customNormalMode)
        {
            std::vector<DirectUI::Element *> alreadyOwned;
            {
                std::lock_guard<std::mutex> lock(
                    g_transferSummaryMutex);

                auto it = std::find_if(
                    g_transferSummaries.begin(),
                    g_transferSummaries.end(),
                    [&state](TransferSummaryState const &candidate)
                    {
                        return candidate.tile == state.tile &&
                               candidate.operationTileRoot ==
                                   state.operationTileRoot;
                    });

                if (it == g_transferSummaries.end())
                {
                    return;
                }

                alreadyOwned = it->nativeElementsHiddenByMod;
            }

            std::vector<DirectUI::Element *> newlyHidden;

            for (DirectUI::Element *element : elements)
            {
                if (!element)
                {
                    continue;
                }

                bool owned =
                    std::find(
                        alreadyOwned.begin(),
                        alreadyOwned.end(),
                        element) != alreadyOwned.end();

                if (owned)
                {
                    // Explorer may have made an element visible again
                    // during an unrelated native refresh. We still own
                    // its original visible state, so suppress it again
                    // without changing the saved restoration record.
                    if (Element_GetVisible_Original(element))
                    {
                        Element_SetVisible_Original(element, false);
                    }
                    continue;
                }

                if (!Element_GetVisible_Original(element))
                {
                    // It was already hidden by Explorer. Do not claim
                    // ownership and never force it visible later.
                    continue;
                }

                HRESULT result =
                    Element_SetVisible_Original(element, false);

                if (SUCCEEDED(result))
                {
                    newlyHidden.push_back(element);
                }
            }

            if (newlyHidden.empty())
            {
                return;
            }

            bool stateStillLive = false;
            {
                std::lock_guard<std::mutex> lock(
                    g_transferSummaryMutex);

                auto it = std::find_if(
                    g_transferSummaries.begin(),
                    g_transferSummaries.end(),
                    [&state](TransferSummaryState const &candidate)
                    {
                        return candidate.tile == state.tile &&
                               candidate.operationTileRoot ==
                                   state.operationTileRoot;
                    });

                if (it != g_transferSummaries.end())
                {
                    stateStillLive = true;

                    for (DirectUI::Element *element : newlyHidden)
                    {
                        if (std::find(
                                it->nativeElementsHiddenByMod.begin(),
                                it->nativeElementsHiddenByMod.end(),
                                element) ==
                            it->nativeElementsHiddenByMod.end())
                        {
                            it->nativeElementsHiddenByMod.push_back(
                                element);
                        }
                    }
                }
            }

            if (!stateStillLive)
            {
                // Registration disappeared while applying suppression.
                // Undo only the changes made by this invocation.
                for (DirectUI::Element *element : newlyHidden)
                {
                    Element_SetVisible_Original(element, true);
                }
            }

            return;
        }

        std::vector<DirectUI::Element *> ownedElements;
        {
            std::lock_guard<std::mutex> lock(
                g_transferSummaryMutex);

            auto it = std::find_if(
                g_transferSummaries.begin(),
                g_transferSummaries.end(),
                [&state](TransferSummaryState const &candidate)
                {
                    return candidate.tile == state.tile &&
                           candidate.operationTileRoot ==
                               state.operationTileRoot;
                });

            if (it == g_transferSummaries.end())
            {
                return;
            }

            ownedElements.swap(it->nativeElementsHiddenByMod);
        }

        std::vector<DirectUI::Element *> failedRestores;

        for (DirectUI::Element *element : ownedElements)
        {
            if (!element ||
                std::find(elements.begin(), elements.end(), element) ==
                    elements.end())
            {
                // The old descendant is no longer part of this live
                // tile. Do not dereference or alter a replacement.
                continue;
            }

            if (Element_GetVisible_Original(element))
            {
                continue;
            }

            HRESULT result =
                Element_SetVisible_Original(element, true);

            if (FAILED(result))
            {
                failedRestores.push_back(element);
            }
        }

        if (!failedRestores.empty())
        {
            std::lock_guard<std::mutex> lock(
                g_transferSummaryMutex);

            auto it = std::find_if(
                g_transferSummaries.begin(),
                g_transferSummaries.end(),
                [&state](TransferSummaryState const &candidate)
                {
                    return candidate.tile == state.tile &&
                           candidate.operationTileRoot ==
                               state.operationTileRoot;
                });

            if (it != g_transferSummaries.end())
            {
                for (DirectUI::Element *element : failedRestores)
                {
                    if (std::find(
                            it->nativeElementsHiddenByMod.begin(),
                            it->nativeElementsHiddenByMod.end(),
                            element) ==
                        it->nativeElementsHiddenByMod.end())
                    {
                        it->nativeElementsHiddenByMod.push_back(
                            element);
                    }
                }
            }
        }
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

    void RestoreGlassDirectUiForHost(HWND hostWindow);

    void HideCustomPresentationForHost(HWND hostWindow)
    {
        SetHostCaptionDrawingSuppressed(hostWindow, false);

        // Special Explorer states must temporarily leave the
        // full-client Acrylic presentation before native DirectUI
        // becomes visible again.
        if (IsGlassTheme())
        {
            ResetUnifiedHostChrome(hostWindow);
        }

        RestoreNativePresentationForHost(hostWindow);

        if (IsGlassTheme())
        {
            RestoreGlassDirectUiForHost(hostWindow);
        }
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
                state.circleHovered = false;
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
            if (child && IsWindow(child))
            {
                StopCurrentFileAnimation(child);
                KillTimer(
                    child,
                    kCircleHoverAnimationTimer);
            }
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

    void ScheduleCustomReapplyForHost(HWND hostWindow,
                                      bool resumeTransferState)
    {
        SetHostCaptionDrawingSuppressed(
            hostWindow,
            g_settings.hideTitleBarPercentage);

        if (IsGlassTheme())
        {
            ApplyUnifiedHostChrome(hostWindow);
            // Returning from Explorer's native special view replaces the
            // whole client surface, even when the footer geometry is stable.
            InvalidateRect(hostWindow, nullptr, FALSE);
        }
        ApplyHostThemeColors(hostWindow);

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

        if (resumeTransferState)
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
        bool *leftSpecial,
        bool *leftNativeSpecial)
    {
        if (enteredSpecial)
        {
            *enteredSpecial = false;
        }
        if (leftSpecial)
        {
            *leftSpecial = false;
        }
        if (leftNativeSpecial)
        {
            *leftNativeSpecial = false;
        }

        if (!hostWindow)
        {
            return;
        }

        bool normalProgress = LooksLikeNativeProgressCaption(caption);
        bool transferSummary = LooksLikeTransferSummaryCaption(caption);
        bool knownSpecial = IsKnownSpecialOperationCaption(caption);

        // Caption shape is not authoritative: it is localized and can be an
        // aggregate host description. Validate the already-discovered normal
        // DirectUI presentation before taking g_hostPresentationMutex so the
        // presentation lock is never nested with tile/transfer registries.
        bool hierarchyStateAvailable = false;
        bool normalHierarchy = IsNormalPresentationHierarchyValidForHost(
            hostWindow, &hierarchyStateAvailable);
        bool multiTileHost = GetRegisteredTileCountForHost(hostWindow) > 1;
        bool deleteLikeHost = HostHasDeleteLikeOperation(hostWindow);
        bool sawNormalCaptionHint =
            normalProgress || transferSummary ||
            ((multiTileHost || deleteLikeHost) &&
             hierarchyStateAvailable && normalHierarchy);
        bool specialOperationState =
            knownSpecial || !hierarchyStateAvailable || !normalHierarchy;
        bool nativeSpecialState =
            knownSpecial ||
            (hierarchyStateAvailable && !normalHierarchy);

        std::lock_guard<std::mutex> lock(g_hostPresentationMutex);

        auto it = std::find_if(
            g_hostPresentationStates.begin(),
            g_hostPresentationStates.end(),
            [hostWindow](HostPresentationState const &state)
            { return state.hostWindow == hostWindow; });

        if (it == g_hostPresentationStates.end())
        {
            g_hostPresentationStates.push_back(
                {hostWindow, sawNormalCaptionHint, specialOperationState,
                 nativeSpecialState});
            if (enteredSpecial && specialOperationState)
            {
                *enteredSpecial = true;
            }
            return;
        }

        bool wasSpecial = it->specialOperationState;
        bool wasNativeSpecial = it->nativeSpecialState;
        if (sawNormalCaptionHint)
        {
            it->sawNormalProgressCaption = true;
        }

        // The language-independent normal hierarchy is authoritative. Any
        // missing, invalid, hidden, or ambiguous normal presentation fails
        // closed to Explorer's native UI. English captions remain only a
        // secondary special-state hint.
        it->specialOperationState = specialOperationState;
        it->nativeSpecialState = nativeSpecialState;

        if (enteredSpecial && !wasSpecial && it->specialOperationState)
        {
            *enteredSpecial = true;
        }
        if (leftSpecial && wasSpecial && !it->specialOperationState)
        {
            *leftSpecial = true;
            if (leftNativeSpecial)
            {
                *leftNativeSpecial = wasNativeSpecial;
            }
        }
    }

    void RefreshHostPresentationStateFromCaption(
        HWND hostWindow,
        PCWSTR captionHint)
    {
        bool enteredSpecial = false;
        bool leftSpecial = false;
        bool leftNativeSpecial = false;
        UpdateHostPresentationStateFromCaption(
            hostWindow, captionHint, &enteredSpecial, &leftSpecial,
            &leftNativeSpecial);

        if (enteredSpecial)
        {

            HideCustomPresentationForHost(hostWindow);
        }
        else if (leftSpecial)
        {

            ScheduleCustomReapplyForHost(hostWindow, leftNativeSpecial);
        }
    }

    void RefreshHostPresentationState(HWND hostWindow)
    {
        wchar_t caption[256]{};
        PCWSTR captionHint =
            GetWindowTextW(hostWindow, caption, ARRAYSIZE(caption))
                ? caption
                : nullptr;

        RefreshHostPresentationStateFromCaption(
            hostWindow, captionHint);
    }

    struct GlassBufferedPaintApi
    {
        using Init_t =
            HRESULT(WINAPI *)();

        using UnInit_t =
            HRESULT(WINAPI *)();

        using Begin_t =
            HPAINTBUFFER(WINAPI *)(
                HDC,
                RECT const *,
                BP_BUFFERFORMAT,
                BP_PAINTPARAMS *,
                HDC *);

        using End_t =
            HRESULT(WINAPI *)(
                HPAINTBUFFER,
                BOOL);

        HMODULE module = nullptr;
        Init_t init = nullptr;
        UnInit_t uninit = nullptr;
        Begin_t begin = nullptr;
        End_t end = nullptr;
    };

    GlassBufferedPaintApi g_glassBufferedPaintApi{};
    bool g_glassBufferedPaintApiInitialized = false;
    HMODULE g_ownedGlassBufferedPaintModule = nullptr;
    std::mutex g_glassBufferedPaintApiMutex;

    GlassBufferedPaintApi *GetGlassBufferedPaintApi()
    {
        std::lock_guard<std::mutex> lock(g_glassBufferedPaintApiMutex);

        GlassBufferedPaintApi &api = g_glassBufferedPaintApi;
        bool &initialized = g_glassBufferedPaintApiInitialized;

        if (!initialized)
        {
            initialized = true;

            api.module =
                GetModuleHandleW(L"uxtheme.dll");

            if (!api.module)
            {
                api.module =
                    LoadLibraryExW(
                        L"uxtheme.dll",
                        nullptr,
                        LOAD_LIBRARY_SEARCH_SYSTEM32);

                if (api.module)
                {
                    g_ownedGlassBufferedPaintModule = api.module;
                }
            }

            if (api.module)
            {
                api.init =
                    reinterpret_cast<GlassBufferedPaintApi::Init_t>(
                        GetProcAddress(
                            api.module,
                            "BufferedPaintInit"));

                api.uninit =
                    reinterpret_cast<GlassBufferedPaintApi::UnInit_t>(
                        GetProcAddress(
                            api.module,
                            "BufferedPaintUnInit"));

                api.begin =
                    reinterpret_cast<GlassBufferedPaintApi::Begin_t>(
                        GetProcAddress(
                            api.module,
                            "BeginBufferedPaint"));

                api.end =
                    reinterpret_cast<GlassBufferedPaintApi::End_t>(
                        GetProcAddress(
                            api.module,
                            "EndBufferedPaint"));

            }
        }

        if (!api.init ||
            !api.uninit ||
            !api.begin ||
            !api.end)
        {
            return nullptr;
        }

        return &api;
    }

    void ShutdownGlassBufferedPaintApi()
    {
        std::lock_guard<std::mutex> lock(g_glassBufferedPaintApiMutex);

        g_glassBufferedPaintApi = {};
        g_glassBufferedPaintApiInitialized = false;

        if (g_ownedGlassBufferedPaintModule)
        {
            FreeLibrary(g_ownedGlassBufferedPaintModule);
            g_ownedGlassBufferedPaintModule = nullptr;
        }
    }

    constexpr wchar_t kGlassDirectUiHiddenByModProperty[] =
        L"Windhawk.FileOperationStyler.GlassDirectUiHidden";

    void RestoreGlassDirectUiForHost(HWND hostWindow)
    {
        if (!hostWindow || !IsWindow(hostWindow))
        {
            return;
        }

        HWND directUi = nullptr;
        while ((directUi = FindWindowExW(
                    hostWindow,
                    directUi,
                    L"DirectUIHWND",
                    nullptr)) != nullptr)
        {
            if (!GetPropW(
                    directUi,
                    kGlassDirectUiHiddenByModProperty))
            {
                continue;
            }

            if (!IsWindowVisible(directUi))
            {
                ShowWindow(directUi, SW_SHOWNA);
            }

            if (IsWindowVisible(directUi))
            {
                RemovePropW(
                    directUi,
                    kGlassDirectUiHiddenByModProperty);

                InvalidateRect(
                    directUi,
                    nullptr,
                    TRUE);
            }
        }
    }

    bool DrawGlassHostProgressRing(HWND hostWindow, HDC deviceContext)
    {
        if (!hostWindow || !deviceContext)
        {
            return false;
        }

        double progressPercent = 0.0;
        bool found = false;

        {
            std::lock_guard<std::mutex> lock(g_circleMutex);

            for (CircleState const &state : g_circles)
            {
                if (state.hostWindow == hostWindow && state.tile)
                {
                    progressPercent =
                        state.currentFileAnimation.overallInitialized
                            ? std::clamp(
                                  state.currentFileAnimation
                                      .displayedOverallPercent,
                                  0.0, 100.0)
                            : static_cast<double>(
                                  std::clamp(
                                      state.progressPercent,
                                      0, 100));

                    found = true;
                    break;
                }
            }
        }

        if (!found)
        {
            return false;
        }

        UINT dpi = GetDpiForWindow(hostWindow);
        if (!dpi)
        {
            dpi = USER_DEFAULT_SCREEN_DPI;
        }

        ThemePalette theme =
            GetDrawingTheme(hostWindow);

        TypographyConfig const &type =
            ActiveTypography();

        Gdiplus::Graphics graphics(deviceContext);

        graphics.SetSmoothingMode(
            Gdiplus::SmoothingModeAntiAlias);

        graphics.SetPixelOffsetMode(
            Gdiplus::PixelOffsetModeHighQuality);

        graphics.SetTextRenderingHint(
            Gdiplus::TextRenderingHintAntiAliasGridFit);

        DrawEmbeddedProgressCircle(
            graphics,
            dpi,
            progressPercent,
            theme,
            type);

        return true;
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
                window,
                *reinterpret_cast<WINDOWPOS *>(lParam));
        }

        if (g_unloading.load(std::memory_order_acquire) &&
            message != g_removeHostSubclassMessage &&
            message != WM_NCDESTROY)
        {
            return DefSubclassProc(
                window,
                message,
                wParam,
                lParam);
        }

        if (g_appearanceChangedMessage &&
            message == g_appearanceChangedMessage)
        {
            if (IsGlassTheme())
            {
                ApplyUnifiedHostChrome(window);
            }
            InvalidateRect(window, nullptr, FALSE);
            return 0;
        }

        if (g_removeHostSubclassMessage &&
            message == g_removeHostSubclassMessage &&
            wParam == kRemoveProgressWindowSubclassCommand)
        {
            HWND progressWindow =
                reinterpret_cast<HWND>(lParam);

            if (!progressWindow ||
                !IsWindow(progressWindow))
            {
                return TRUE;
            }

            if (GetWindowThreadProcessId(
                    progressWindow,
                    nullptr) != GetCurrentThreadId())
            {
                return FALSE;
            }

            return RemoveProgressWindowSubclassForTeardown(
                       progressWindow)
                       ? TRUE
                       : FALSE;
        }

        if (g_removeHostSubclassMessage &&
            message == g_removeHostSubclassMessage)
        {
            // Restore Explorer's normal non-client caption drawing.
            SetHostCaptionDrawingSuppressed(window, false);

            // Restore anything hidden by the custom Glass presentation.
            RestoreGlassDirectUiForHost(window);

            // Restore Explorer's own normal presentation.
            RestoreNativePresentationForHost(window);

            // Settings reload happens after teardown, so the old theme still
            // selects which caption/backdrop state needs restoring here.
            ResetUnifiedHostChrome(window);

            RestoreHostNativeGeometry(window);

            if (!DestroyProgressCirclesForHost(window))
            {
                return FALSE;
            }

            CancelDeferredDisplaySnapshotsForHost(window);
            ForgetHostPresentationState(window);

            DWORD_PTR referenceData = 0;

            if (!RemoveWindowSubclass(
                    window,
                    OperationStatusWindowSubclassProc,
                    subclassId) &&
                GetWindowSubclass(
                    window,
                    OperationStatusWindowSubclassProc,
                    subclassId,
                    &referenceData))
            {
                Wh_Log(
                    L"Presentation teardown failed to remove host "
                    L"subclass hwnd=%p error=%lu",
                    reinterpret_cast<void *>(window),
                    GetLastError());

                return FALSE;
            }

            RemoveHostSubclassRecord(window);

            return TRUE;
        }

        if (message == g_positionCirclesMessage)
        {
            PCWSTR reason =
                TakeProgressCirclePositionReason(window);

            if (reason)
            {
                PositionProgressCirclesForHost(
                    window,
                    reason);

                ApplyNativeDisplayRatesForHost(window);
            }

            return 0;
        }

        if (message == g_logDisplayStateMessage)
        {
            HandleDeferredDisplaySnapshot(
                window,
                static_cast<unsigned long long>(wParam));

            return 0;
        }

        if (message == WM_NCDESTROY)
        {
            RestoreGlassDirectUiForHost(window);

            CancelDeferredDisplaySnapshotsForHost(window);
            ForgetHostPresentationState(window);
            ForgetHostNativeGeometry(window);

            if (!DestroyProgressCirclesForHost(window))
            {
                Wh_Log(
                    L"Presentation cleanup incomplete during host "
                    L"destruction hwnd=%p",
                    reinterpret_cast<void *>(window));
            }

            RemoveHostSubclassRecord(window);

            if (!RemoveWindowSubclass(
                    window,
                    OperationStatusWindowSubclassProc,
                    subclassId))
            {
                Wh_Log(
                    L"Presentation cleanup failed to remove destroying "
                    L"host subclass hwnd=%p error=%lu",
                    reinterpret_cast<void *>(window),
                    GetLastError());
            }
        }

        if (message == WM_ERASEBKGND &&
            IsGlassTheme() &&
            !IsHostInSpecialOperationState(window))
        {
            // Glass owns the complete visible client surface.
            // Prevent Explorer from erasing it immediately before
            // our buffered WM_PAINT, which can expose a blank frame.
            return 1;
        }

        if (IsGlassTheme() && message == WM_MOUSELEAVE)
        {
            ClearCircleHoverStatesForHost(window);
        }

        // In Glass mode the info panels are painted into the shared host
        // instead of shown as interactive child windows. Route Pause/Cancel
        // to the operation tile whose visual slot contains the mouse.
        if (IsGlassTheme() &&
            !IsHostInSpecialOperationState(window) &&
            (message == WM_LBUTTONUP ||
             message == WM_SETCURSOR ||
             message == WM_MOUSEMOVE))
        {
            if (message == WM_MOUSEMOVE)
            {
                TRACKMOUSEEVENT tracking{};
                tracking.cbSize = sizeof(tracking);
                tracking.dwFlags = TME_LEAVE;
                tracking.hwndTrack = window;
                TrackMouseEvent(&tracking);
            }
            struct GlassActionEntry
            {
                OperationTileElement *tile = nullptr;
                HWND infoWindow = nullptr;
            };

            std::vector<GlassActionEntry> actionEntries;

            {
                std::lock_guard<std::mutex> lock(g_circleMutex);

                for (CircleState const &state : g_circles)
                {
                    if (state.hostWindow == window &&
                        state.tile &&
                        state.infoWindow &&
                        IsWindow(state.infoWindow))
                    {
                        actionEntries.push_back(
                            {state.tile, state.infoWindow});
                    }
                }
            }

            POINT point{};
            bool havePoint = false;

            if (message == WM_LBUTTONUP ||
                message == WM_MOUSEMOVE)
            {
                point = {
                    static_cast<short>(LOWORD(lParam)),
                    static_cast<short>(HIWORD(lParam))};
                havePoint = true;
            }
            else if (LOWORD(lParam) == HTCLIENT &&
                     GetCursorPos(&point) &&
                     ScreenToClient(window, &point))
            {
                havePoint = true;
            }

            if (havePoint && !actionEntries.empty())
            {
                HWND glassInfoActionWindow = nullptr;
                POINT panelPoint = point;

                if (actionEntries.size() == 1)
                {
                    glassInfoActionWindow =
                        actionEntries.front().infoWindow;
                }
                else
                {
                    for (GlassActionEntry const &entry : actionEntries)
                    {
                        int slotTop = 0;
                        int slotBottom = 0;

                        if (!GetMultiTileSlotBounds(
                                entry.tile,
                                window,
                                &slotTop,
                                &slotBottom))
                        {
                            continue;
                        }

                        if (point.y >= slotTop &&
                            point.y < slotBottom)
                        {
                            glassInfoActionWindow =
                                entry.infoWindow;

                            // Existing hit-test rectangles are local to one
                            // logical info panel, not to the whole host.
                            panelPoint.y -= slotTop;
                            break;
                        }
                    }
                }

                if (glassInfoActionWindow)
                {
                    RECT cancelRect{};
                    RECT pauseRect{};
                    RECT circleRect{};

                    bool previousTransparentPaint =
                        g_glassTransparentInfoPanelPaint;
                    g_glassTransparentInfoPanelPaint = true;

                    GetInfoPanelCancelRect(
                        glassInfoActionWindow, &cancelRect);
                    GetInfoPanelPauseRect(
                        glassInfoActionWindow, &pauseRect);
                    GetInfoPanelCircleActionRect(
                        glassInfoActionWindow, &circleRect);

                    g_glassTransparentInfoPanelPaint =
                        previousTransparentPaint;

                    bool overCancel =
                        PtInRect(&cancelRect, panelPoint) != FALSE;
                    bool overPause =
                        PtInRect(&pauseRect, panelPoint) != FALSE;
                    bool overCircle =
                        PtInRect(&circleRect, panelPoint) != FALSE;

                    if (message == WM_SETCURSOR ||
                        message == WM_MOUSEMOVE)
                    {
                        for (GlassActionEntry const &entry : actionEntries)
                        {
                            SetCircleHoverState(
                                entry.infoWindow,
                                entry.infoWindow == glassInfoActionWindow &&
                                    overCircle);
                        }
                    }

                    if (overCancel || overPause || overCircle)
                    {
                        if (message == WM_SETCURSOR)
                        {
                            SetCursor(
                                LoadCursorW(nullptr, IDC_HAND));
                            return TRUE;
                        }

                        if (message == WM_MOUSEMOVE)
                        {
                            return DefSubclassProc(
                                window, message, wParam, lParam);
                        }

                        if (overCancel)
                        {
                            InvokeNativeActionFromInfoPanel(
                                glassInfoActionWindow,
                                L"eltCancelButton",
                                L"cancel-top-x");
                        }
                        else if (overCircle)
                        {
                            InvokeNativeActionFromInfoPanel(
                                glassInfoActionWindow,
                                L"eltPauseButton",
                                L"circle-pause-resume");
                        }
                        else
                        {
                            InvokeNativeActionFromInfoPanel(
                                glassInfoActionWindow,
                                L"eltPauseButton",
                                L"pause-resume");
                        }

                        return 0;
                    }
                }
            }
        }

        // If Glass routing reached this point, the pointer did not
        // remain over an actionable circle. Clear any tile that was
        // left hovered by a previous routed mouse message.
        if (IsGlassTheme() &&
            (message == WM_SETCURSOR ||
             message == WM_MOUSEMOVE))
        {
            ClearCircleHoverStatesForHost(window);
        }

        LRESULT footerResult = 0;
        if (RouteGlassFooterMouseMessage(
                window, message, wParam, lParam, &footerResult))
        {
            return footerResult;
        }

        if (message == WM_PAINT &&
            IsGlassTheme())
        {

            // Native conflict/permission/file-in-use pages own the
            // host while Explorer is in a special operation state.
            if (IsHostInSpecialOperationState(window))
            {
                return DefSubclassProc(
                    window, message, wParam, lParam);
            }

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

            GlassBufferedPaintApi *bp =
                GetGlassBufferedPaintApi();

            if (!bp)
            {
                Wh_Log(
                    L"Glass: buffered-paint API unavailable");

                EndPaint(window, &paint);
                RestoreGlassDirectUiForHost(window);
                return 0;
            }

            HRESULT initResult =
                bp->init();

            if (FAILED(initResult))
            {
                Wh_Log(
                    L"Glass: BufferedPaintInit failed result=0x%08X",
                    static_cast<unsigned int>(initResult));

                EndPaint(window, &paint);
                RestoreGlassDirectUiForHost(window);
                return 0;
            }

            BP_PAINTPARAMS params{};
            params.cbSize = sizeof(params);
            // Custom Blur depends on a transparent host surface. This clears
            // the top-down DIB to ARGB {0, 0, 0, 0} before any drawing.
            params.dwFlags = BPPF_ERASE;

            HDC bufferDc = nullptr;

            HPAINTBUFFER buffer =
                bp->begin(
                    paintDc,
                    &client,
                    BPBF_TOPDOWNDIB,
                    &params,
                    &bufferDc);

            if (!buffer || !bufferDc)
            {
                Wh_Log(L"Glass: BeginBufferedPaint failed");

                if (buffer)
                {
                    bp->end(buffer, FALSE);
                }

                bp->uninit();
                EndPaint(window, &paint);
                RestoreGlassDirectUiForHost(window);
                return 0;
            }

            // The custom paint surface is ready. Only now hide Explorer's
            // native DirectUI presentation so a paint failure can always
            // fall back to usable native content.
            HWND directUi = nullptr;
            while ((directUi = FindWindowExW(
                        window,
                        directUi,
                        L"DirectUIHWND",
                        nullptr)) != nullptr)
            {
                if (!IsWindowVisible(directUi))
                {
                    continue;
                }

                if (!SetPropW(
                        directUi,
                        kGlassDirectUiHiddenByModProperty,
                        reinterpret_cast<HANDLE>(1)))
                {
                    // Fail closed: without an ownership marker we
                    // cannot safely restore this HWND later.
                    continue;
                }

                ShowWindow(directUi, SW_HIDE);

                if (IsWindowVisible(directUi))
                {
                    // Hiding failed, so this HWND never became ours.
                    RemovePropW(
                        directUi,
                        kGlassDirectUiHiddenByModProperty);
                }
            }

            {
                if (g_settings.glassEffect == L"solid")
                {
                    Gdiplus::Graphics solidGraphics(bufferDc);
                    Gdiplus::SolidBrush solidBrush(
                        Gdiplus::Color(
                            255,
                            GetRValue(g_settings.theme.background),
                            GetGValue(g_settings.theme.background),
                            GetBValue(g_settings.theme.background)));

                    solidGraphics.FillRectangle(
                        &solidBrush,
                        0,
                        0,
                        client.right - client.left,
                        client.bottom - client.top);
                }
                else if (CurrentGlassStrength() > 0)
                {
                    // Paint one tint layer over the selected native material.
                    // This makes the same 0-100 opacity control work for
                    // Acrylic, Custom Blur, and Mica.
                    int tintAlpha =
                        CurrentGlassStrength() *
                        255 / 100;

                    Gdiplus::Graphics glassGraphics(bufferDc);
                    Gdiplus::SolidBrush tintBrush(
                        Gdiplus::Color(
                            tintAlpha,
                            GetRValue(CurrentGlassTint()),
                            GetGValue(CurrentGlassTint()),
                            GetBValue(CurrentGlassTint())));

                    glassGraphics.FillRectangle(
                        &tintBrush,
                        0,
                        0,
                        client.right - client.left,
                        client.bottom - client.top);
                }

                struct GlassInfoPanelPaintEntry
                {
                    OperationTileElement *tile = nullptr;
                    HWND infoWindow = nullptr;
                };

                std::vector<GlassInfoPanelPaintEntry> glassInfoPanels;

                {
                    std::lock_guard<std::mutex> lock(g_circleMutex);

                    for (CircleState const &state : g_circles)
                    {
                        if (state.hostWindow == window &&
                            state.tile &&
                            state.infoWindow &&
                            IsWindow(state.infoWindow))
                        {
                            glassInfoPanels.push_back(
                                {state.tile, state.infoWindow});
                        }
                    }
                }

                bool drewInfoPanel = false;

                if (!glassInfoPanels.empty())
                {
                    RECT hostClient{};

                    if (GetClientRect(window, &hostClient))
                    {
                        g_glassTransparentInfoPanelPaint = true;

                        bool multiTile =
                            glassInfoPanels.size() > 1;

                        for (GlassInfoPanelPaintEntry const &entry :
                             glassInfoPanels)
                        {
                            RECT panelRect = hostClient;

                            if (multiTile)
                            {
                                int slotTop = 0;
                                int slotBottom = 0;

                                if (!GetMultiTileSlotBounds(
                                        entry.tile,
                                        window,
                                        &slotTop,
                                        &slotBottom))
                                {
                                    continue;
                                }

                                panelRect.top = slotTop;
                                panelRect.bottom = slotBottom;
                            }

                            int savedDc = SaveDC(bufferDc);

                            if (savedDc != 0)
                            {
                                IntersectClipRect(
                                    bufferDc,
                                    panelRect.left,
                                    panelRect.top,
                                    panelRect.right,
                                    panelRect.bottom);

                                DrawInfoPanelFrame(
                                    entry.infoWindow,
                                    bufferDc,
                                    panelRect);

                                RestoreDC(bufferDc, savedDc);
                                drewInfoPanel = true;
                            }
                        }

                        g_glassTransparentInfoPanelPaint = false;
                    }
                }

                if (!drewInfoPanel)
                {
                    DrawGlassHostProgressRing(
                        window,
                        bufferDc);
                }

                DrawGlassHostFooter(window, bufferDc);

                HRESULT commitResult =
                    bp->end(buffer, TRUE);

                if (FAILED(commitResult))
                {
                    Wh_Log(
                        L"Glass: EndBufferedPaint commit failed "
                        L"result=0x%08X hwnd=%p",
                        static_cast<unsigned int>(commitResult),
                        reinterpret_cast<void *>(window));

                    // The custom frame did not commit. Restore only
                    // DirectUI HWNDs that carry our ownership marker.
                    RestoreGlassDirectUiForHost(window);

                    bp->uninit();
                    EndPaint(window, &paint);
                    return 0;
                }
            }

            bp->uninit();

            EndPaint(
                window,
                &paint);

            return 0;
        }
        LRESULT result = DefSubclassProc(window, message, wParam, lParam);
        if (message == WM_NCACTIVATE && IsGlassTheme() &&
            !g_unloading.load(std::memory_order_acquire) &&
            IsWindow(window) && !IsIconic(window) &&
            !IsHostInSpecialOperationState(window))
        {
            // Let Explorer process the real activation notification first.
            // Only DWM's appearance is held active; WM_ACTIVATE, focus, and
            // the native return value are preserved. No backdrop recreation.
            DefWindowProcW(window, WM_NCACTIVATE, TRUE, -1);
        }
        if (message == WM_SETTEXT || message == WM_SIZE ||
            message == WM_WINDOWPOSCHANGED)
        {
            RefreshHostPresentationState(window);

            // Explorer owns/localizes the caption. Its text is only a
            // secondary hint after native state processing; DirectUI normal-
            // hierarchy validation is the primary fallback signal.
        }
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
            // Explorer can restore its cached/native width after a
            // minimize/restore cycle. Reapply the verified custom
            // Glass geometry once the host is restored.
            if (wParam == SIZE_RESTORED &&
                IsGlassTheme() &&
                !IsHostInSpecialOperationState(window))
            {
                static thread_local bool repairingRestoreGeometry;

                if (!repairingRestoreGeometry)
                {
                    OperationTileElement *hostTile = nullptr;

                    {
                        std::lock_guard<std::mutex> lock(g_circleMutex);

                        auto circleIt = std::find_if(
                            g_circles.begin(),
                            g_circles.end(),
                            [window](CircleState const &candidate)
                            {
                                return candidate.hostWindow == window &&
                                       candidate.tile != nullptr;
                            });

                        if (circleIt != g_circles.end())
                        {
                            hostTile = circleIt->tile;
                        }
                    }

                    bool modeKnown = false;
                    bool expanded = false;

                    if (hostTile)
                    {
                        std::lock_guard<std::mutex> lock(
                            g_transferSummaryMutex);

                        auto stateIt = std::find_if(
                            g_transferSummaries.begin(),
                            g_transferSummaries.end(),
                            [hostTile](TransferSummaryState const &candidate)
                            {
                                return candidate.tile == hostTile &&
                                       candidate.displayModeKnown;
                            });

                        if (stateIt != g_transferSummaries.end())
                        {
                            modeKnown = true;
                            expanded = stateIt->expanded;
                        }
                    }

                    if (modeKnown)
                    {
                        repairingRestoreGeometry = true;
                        ResizeOperationStatusWindowForMode(
                            window, expanded, 0);
                        repairingRestoreGeometry = false;
                    }
                }
            }

            PositionProgressCirclesForHost(
                window, L"host-size-sync");
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
        if (g_removeHostSubclassMessage &&
            message == g_removeHostSubclassMessage)
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

    void InvalidateInfoPanelForTile(OperationTileElement *tile,
                                    bool notifyAnimation)
    {
        HWND infoWindow = nullptr;
        HWND hostWindow = nullptr;

        {
            std::lock_guard<std::mutex> lock(g_circleMutex);

            auto it = std::find_if(
                g_circles.begin(),
                g_circles.end(),
                [tile](CircleState const &state)
                {
                    return state.tile == tile;
                });

            if (it != g_circles.end())
            {
                infoWindow = it->infoWindow;
                hostWindow = it->hostWindow;
            }
        }

        bool hostIconic =
            hostWindow &&
            IsWindow(hostWindow) &&
            IsIconic(hostWindow);

        if (infoWindow && IsWindow(infoWindow))
        {
            // Progress may arrive outside this HWND's thread. Marshal all
            // animation state/timer changes to its existing window procedure.
            if (notifyAnimation &&
                !g_unloading.load(std::memory_order_acquire))
            {
                PostMessageW(infoWindow, kCurrentFileAnimationMessage, 0, 0);
            }
            if (!hostIconic)
            {
                InvalidateRect(infoWindow, nullptr, FALSE);
            }
        }

        if (IsGlassTheme() &&
            hostWindow &&
            IsWindow(hostWindow) &&
            !hostIconic)
        {
            InvalidateRect(
                hostWindow,
                nullptr,
                FALSE);
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

        if (IsGlassTheme())
        {
            if (infoWindow &&
                IsWindow(infoWindow) &&
                IsWindowVisible(infoWindow))
            {
                ShowWindow(infoWindow, SW_HIDE);
            }

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

    bool InstallAndTrackProgressWindowSubclass(
        OperationTileElement *tile,
        HWND progressWindow)
    {
        if (!tile || !progressWindow ||
            !SetWindowSubclass(
                progressWindow, NativeProgressWindowSubclassProc,
                kProgressWindowSubclassId, 0))
        {
            return false;
        }

        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (it != g_circles.end() &&
                (!it->progressWindow ||
                 it->progressWindow == progressWindow))
            {
                it->progressWindow = progressWindow;
                it->positionValid = false;
                return true;
            }
        }

        CircleState cleanupState{};
        cleanupState.progressWindow = progressWindow;
        cleanupState.hostWindow = GetAncestor(progressWindow, GA_ROOT);
        if (!CleanupCircleStateResources(&cleanupState, false))
        {
            RetainCleanupOnlyCircleState(
                cleanupState, L"unpublished-progress-subclass");
        }
        return false;
    }

    void PositionProgressCircle(OperationTileElement *tile, PCWSTR reason)
    {
        if (!tile)
        {
            return;
        }

        (void)reason;
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
        bool progressWindowChanged = false;
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
                progressWindowChanged = true;
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

        if (previousProgressWindow)
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto bindingIt = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (bindingIt != g_circles.end() &&
                bindingIt->progressWindow == previousProgressWindow)
            {
                bindingIt->progressWindow = nullptr;
                bindingIt->positionValid = false;
            }
        }

        if (progressWindowChanged)
        {
            if (!InstallAndTrackProgressWindowSubclass(
                    tile, latestProgressWindow))
            {
                Wh_Log(
                    L"Progress HWND rebind failed to install/track subclass "
                    L"hwnd=%p error=%lu",
                    reinterpret_cast<void *>(latestProgressWindow),
                    GetLastError());
            }
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
                if (state.hostWindow == hostWindow && state.tile)
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

        if (IsGlassTheme())
        {
            ApplyUnifiedHostChrome(hostWindow);
        }
        ApplyHostThemeColors(hostWindow);

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

        SetHostCaptionDrawingSuppressed(
            hostWindow,
            g_settings.hideTitleBarPercentage);

        // Capture Explorer's current native size before any mod-owned
        // resize can occur. Later genuine native size changes may
        // update this snapshot through WM_WINDOWPOSCHANGING.
        RECT initialNativeWindowRect{};
        if (GetWindowRect(hostWindow, &initialNativeWindowRect))
        {
            WINDOWPOS initialNativePosition{};
            initialNativePosition.cx =
                initialNativeWindowRect.right -
                initialNativeWindowRect.left;
            initialNativePosition.cy =
                initialNativeWindowRect.bottom -
                initialNativeWindowRect.top;

            if (initialNativePosition.cx > 0 &&
                initialNativePosition.cy > 0)
            {
                CaptureHostNativeGeometry(
                    hostWindow, initialNativePosition);
            }
        }
        else
        {
            Wh_Log(
                L"eventId=%llu initial native geometry capture failed "
                L"host=%p error=%lu",
                eventId,
                reinterpret_cast<void *>(hostWindow),
                GetLastError());
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
                &enteredSpecial, &leftSpecial, nullptr);
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

    bool TryGetDeleteLikeOperationKind(
        TransferSummaryState const &state,
        bool *deleteLike)
    {
        if (!state.operationTileRoot || !deleteLike)
        {
            return false;
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
            // Empty Recycle Bin can expose neither location string while
            // Explorer is still discovering the recycle-bin contents.
            // During that phase the discovered item/byte totals grow while
            // completed work remains zero.
            if (state.itemsValid &&
                state.bytesValid &&
                state.totalItems > 0 &&
                state.totalBytes > 0)
            {
                *deleteLike = true;
                return true;
            }

            return false;
        }

        // Copy/move expose a non-empty source and destination location. Delete
        // has only the source location; an empty second-location placeholder can
        // still exist, so content rather than element existence is decisive.
        *deleteLike = firstPresent && !secondPresent;
        return true;
    }

    void RefreshDeleteLikeOperationKind(
        COperationStatusTile *owner,
        TransferSummaryState const &state)
    {
        bool deleteLike = false;
        if (!owner || !TryGetDeleteLikeOperationKind(state, &deleteLike))
        {
            return;
        }

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
                TransferSummaryState state{};
                state.owner = owner;
                state.tile = tile;
                state.operationTileRoot = operationTileRoot;
                state.tileHeaderRoot = tileHeaderRoot;
                g_transferSummaries.push_back(state);
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

        if (hostWindow)
        {
            RefreshHostPresentationState(hostWindow);
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

        if (g_transferSummaries.empty())
        {
            ClearOperationDataBindingsLocked();
        }
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

    bool IsNormalPresentationHierarchyValidForHost(HWND hostWindow,
                                                   bool *stateAvailable)
    {
        if (stateAvailable)
        {
            *stateAvailable = false;
        }
        if (!hostWindow || !IsWindow(hostWindow) ||
            !Element_GetVisible_Original || !stateAvailable)
        {
            return false;
        }

        size_t registeredTileCount =
            GetRegisteredTileCountForHost(hostWindow);
        if (!registeredTileCount)
        {
            return false;
        }

        std::vector<TransferSummaryState> states;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            states = g_transferSummaries;
        }

        std::vector<TransferSummaryState> hostStates;
        for (TransferSummaryState const &state : states)
        {
            if (state.tile &&
                GetRegisteredCircleHost(state.tile) == hostWindow)
            {
                hostStates.push_back(state);
            }
        }
        if (hostStates.size() != registeredTileCount)
        {
            return false;
        }
        *stateAvailable = true;

        for (TransferSummaryState const &state : hostStates)
        {
            if (!state.operationTileRoot ||
                !Element_GetVisible_Original(state.operationTileRoot))
            {
                return false;
            }

            NormalProgressLayoutElements elements{};
            bool completeNormalLayout =
                DiscoverNormalProgressLayout(state, &elements);
            bool deleteLike = state.deleteLikeKnown && state.deleteLike;
            if (!state.deleteLikeKnown &&
                !TryGetDeleteLikeOperationKind(state, &deleteLike))
            {
                return false;
            }

            bool validHierarchy =
                deleteLike
                    ? ValidateDeleteLikeProgressHierarchy(elements, 0, false)
                    : completeNormalLayout &&
                          ValidateNormalProgressHierarchy(elements, 0, false);
            if (!validHierarchy || !elements.regularTile ||
                !Element_GetVisible_Original(elements.regularTile))
            {
                return false;
            }
        }

        return true;
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

        HWND nativeProgressWindow =
            OperationTileElement_GetProgressHWND_Original(state.tile);
        NativeProgressSnapshot initialProgress =
            ReadNativeProgress(nativeProgressWindow, 0);

        if (!EnsureProgressCircle(state.tile, 0, initialProgress))
        {
            // Keep the native presentation authoritative and allow a
            // later native update to retry activation.
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [owner](TransferSummaryState const &candidate)
                { return candidate.owner == owner; });
            if (it != g_transferSummaries.end() &&
                it->tile == state.tile)
            {
                it->displayModeKnown = false;
            }
            return;
        }

        HWND hostWindow = GetRegisteredCircleHost(state.tile);
        if (hostWindow && GetRegisteredTileCountForHost(hostWindow) > 1)
        {
            MarkHostForMeasuredMultiRate(hostWindow);
        }
        if (hostWindow)
        {
            RefreshHostPresentationState(hostWindow);
        }

        unsigned long long transitionId = ++g_displayTransitionSequence;
        ApplyDisplayMode(owner, false, transitionId);
        ScheduleDeferredDisplaySnapshot(owner, transitionId, expanded);
    }

    void RecordDisplayMode(COperationStatusTile *owner,
                           bool expanded,
                           unsigned long long transitionId)
    {
        OperationTileElement *sourceTile = nullptr;

        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(),
                g_transferSummaries.end(),
                [owner](TransferSummaryState const &state)
                { return state.owner == owner; });

            if (it == g_transferSummaries.end() ||
                !it->tile ||
                !it->operationTileRoot)
            {
                return;
            }

            sourceTile = it->tile;
        }

        HWND hostWindow =
            GetRegisteredCircleHost(sourceTile);

        std::vector<OperationTileElement *> hostTiles;

        if (hostWindow)
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);

            for (CircleState const &circle : g_circles)
            {
                if (circle.hostWindow == hostWindow &&
                    circle.tile)
                {
                    hostTiles.push_back(circle.tile);
                }
            }
        }

        std::vector<COperationStatusTile *> ownersToApply;

        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);

            for (TransferSummaryState &state :
                 g_transferSummaries)
            {
                bool sameHost =
                    !hostTiles.empty() &&
                    std::find(
                        hostTiles.begin(),
                        hostTiles.end(),
                        state.tile) != hostTiles.end();

                // If host lookup unexpectedly failed, preserve the
                // previous single-owner behavior instead of changing
                // unrelated transfer state.
                bool sourceOwnerFallback =
                    hostTiles.empty() &&
                    state.owner == owner;

                if ((!sameHost && !sourceOwnerFallback) ||
                    !state.owner ||
                    !state.tile ||
                    !state.operationTileRoot)
                {
                    continue;
                }

                state.displayModeKnown = true;
                state.expanded = expanded;
                ownersToApply.push_back(state.owner);
            }
        }

        // Re-layout every custom tile in this host. Do this outside
        // the state locks because ApplyDisplayMode performs window
        // and DirectUI work.
        for (COperationStatusTile *affectedOwner : ownersToApply)
        {
            ApplyDisplayMode(
                affectedOwner,
                false,
                transitionId);
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

            if (g_showCurrentFileProgressBar &&
                EnsureSharedProgressBridge())
            {
                ULONGLONG readNow = GetTickCount64();
                bool foundSharedCurrentFile = false;
                bool ambiguousSharedCurrentFile = false;
                unsigned long long bestItemDistance = ~0ULL;
                unsigned long long bestByteDistance = ~0ULL;
                unsigned long long bestFileSize = 0;
                unsigned long long bestFileStartBytes = 0;

                for (int sharedIndex = 0;
                     sharedIndex < kSharedProgressSlotCount;
                     ++sharedIndex)
                {
                    auto &shared =
                        g_sharedProgressTable->slots[sharedIndex];

                    LONG sequenceBefore = shared.sequence;

                    if ((sequenceBefore & 1) != 0)
                    {
                        continue;
                    }

                    MemoryBarrier();

                    unsigned long long sharedCompletedItems =
                        shared.completedItems;
                    unsigned long long sharedTotalItems =
                        shared.totalItems;
                    unsigned long long sharedCompletedBytes =
                        shared.completedBytes;
                    unsigned long long sharedTotalBytes =
                        shared.totalBytes;
                    unsigned long long sharedFileSize =
                        shared.currentFileSize;
                    unsigned long long sharedFileStartBytes =
                        shared.currentFileStartBytes;
                    BOOL sharedFileValid = shared.currentFileValid;
                    ULONGLONG sharedTick = shared.updateTick;

                    MemoryBarrier();

                    LONG sequenceAfter = shared.sequence;

                    if (sequenceBefore != sequenceAfter ||
                        (sequenceAfter & 1) != 0)
                    {
                        continue;
                    }

                    if (!sharedFileValid ||
                        sharedFileSize == 0 ||
                        sharedTotalItems != totalItems ||
                        sharedTotalBytes != totalBytes ||
                        readNow < sharedTick ||
                        readNow - sharedTick >
                            kSharedProgressFreshnessMs)
                    {
                        continue;
                    }

                    unsigned long long itemDistance =
                        sharedCompletedItems >= completedItems
                            ? sharedCompletedItems - completedItems
                            : completedItems - sharedCompletedItems;

                    unsigned long long byteDistance =
                        sharedCompletedBytes >= completedBytes
                            ? sharedCompletedBytes - completedBytes
                            : completedBytes - sharedCompletedBytes;

                    bool betterCandidate =
                        !foundSharedCurrentFile ||
                        itemDistance < bestItemDistance ||
                        (itemDistance == bestItemDistance &&
                         byteDistance < bestByteDistance);

                    bool equalCandidate =
                        foundSharedCurrentFile &&
                        itemDistance == bestItemDistance &&
                        byteDistance == bestByteDistance;

                    if (betterCandidate)
                    {
                        foundSharedCurrentFile = true;
                        ambiguousSharedCurrentFile = false;
                        bestItemDistance = itemDistance;
                        bestByteDistance = byteDistance;
                        bestFileSize = sharedFileSize;
                        bestFileStartBytes =
                            sharedFileStartBytes;
                    }
                    else if (equalCandidate &&
                             (sharedFileSize != bestFileSize ||
                              sharedFileStartBytes != bestFileStartBytes))
                    {
                        ambiguousSharedCurrentFile = true;
                    }
                }

                if (foundSharedCurrentFile &&
                    !ambiguousSharedCurrentFile)
                {
                    it->currentFileSize = bestFileSize;
                    it->currentFileStartBytes =
                        bestFileStartBytes;
                    it->currentFileProgressValid = true;

                    unsigned long long copied = 0;

                    if (completedBytes >=
                        it->currentFileStartBytes)
                    {
                        copied =
                            completedBytes -
                            it->currentFileStartBytes;
                    }

                    copied = std::min(
                        copied,
                        it->currentFileSize);

                    it->currentFileCompletedBytes = copied;

                    long double currentRawPercent =
                        static_cast<long double>(copied) *
                        100.0L /
                        static_cast<long double>(
                            it->currentFileSize);

                    it->currentFilePercent =
                        static_cast<int>(
                            std::clamp<long double>(
                                currentRawPercent,
                                0.0L,
                                100.0L));
                }
                else
                {
                    it->currentFileProgressValid = false;
                }
            }
            else
            {
                it->currentFileProgressValid = false;
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

        if (!owner)
        {
            return;
        }

        ApplyTransferSummary(owner);

        // Some operations, notably Empty Recycle Bin, register their tile
        // before Explorer has populated the native description/visibility
        // state. Retry classification and initial display-mode discovery on
        // later native progress updates instead of permanently giving up on
        // the first all-hidden transitional frame.
        TransferSummaryState state{};
        if (!CopyRegisteredTransferState(owner, &state) ||
            !state.tile ||
            !state.operationTileRoot)
        {
            return;
        }

        RefreshDeleteLikeOperationKind(owner, state);

        TransferSummaryState refreshedState{};
        if (CopyRegisteredTransferState(owner, &refreshedState) &&
            !refreshedState.displayModeKnown)
        {
            InitializeRegisteredDisplayMode(owner);
        }
    }

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
            if (progressSubclassNeeded)
            {
                if (!InstallAndTrackProgressWindowSubclass(
                        tile, progressWindow) &&
                    eventId)
                {
                    Wh_Log(
                        L"eventId=%llu circle progress subclass install/track "
                        L"failed error=%lu",
                        eventId, GetLastError());
                }
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

        // A custom presentation must never be activated until Explorer's
        // native compact/expanded state has been positively identified.
        // Existing circles may still be refreshed after activation.
        bool displayModeKnown = false;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto transferIt = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [tile](TransferSummaryState const &state)
                { return state.tile == tile; });
            displayModeKnown =
                transferIt != g_transferSummaries.end() &&
                transferIt->displayModeKnown;
        }

        if (!displayModeKnown)
        {
            return false;
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
            tile, circleWindow, infoWindow, nullptr, hostWindow,
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

        if (progressWindow)
        {
            if (SetWindowSubclass(
                    progressWindow, NativeProgressWindowSubclassProc,
                    kProgressWindowSubclassId, 0))
            {
                newState.progressWindow = progressWindow;
            }
            else
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

        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [tile](TransferSummaryState const &state)
                { return state.tile == tile; });
            if (it == g_transferSummaries.end())
            {
                return;
            }
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
                    DestroyWindowOnOwningThread(footerWindow,
                                                L"footer-overlay");
                }
            }
            else
            {
                // A multi-operation host can keep its old two-row height
                // after one tile disappears until Explorer performs another
                // native layout pass. Reapply our verified custom geometry
                // immediately from the remaining tile state.
                bool modeKnown = false;
                bool expanded = false;

                std::vector<OperationTileElement *> remainingTiles;
                {
                    std::lock_guard<std::mutex> lock(g_circleMutex);

                    for (CircleState const &remaining : g_circles)
                    {
                        if (remaining.hostWindow == state.hostWindow &&
                            remaining.tile)
                        {
                            remainingTiles.push_back(remaining.tile);
                        }
                    }
                }

                {
                    std::lock_guard<std::mutex> lock(
                        g_transferSummaryMutex);

                    for (TransferSummaryState const &summary :
                         g_transferSummaries)
                    {
                        if (!summary.tile ||
                            !summary.displayModeKnown)
                        {
                            continue;
                        }

                        if (std::find(
                                remainingTiles.begin(),
                                remainingTiles.end(),
                                summary.tile) ==
                            remainingTiles.end())
                        {
                            continue;
                        }

                        modeKnown = true;
                        expanded = expanded || summary.expanded;
                    }
                }

                if (modeKnown &&
                    GetWindowThreadProcessId(
                        state.hostWindow,
                        nullptr) == GetCurrentThreadId())
                {
                    ResizeOperationStatusWindowForMode(
                        state.hostWindow,
                        expanded,
                        0);
                }

                ScheduleProgressCirclePosition(
                    state.hostWindow,
                    L"tile-removed");

                InvalidateRect(
                    state.hostWindow,
                    nullptr,
                    FALSE);
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
            CircleState recordIdentity = state;
            if (CleanupCircleStateResources(&state, true))
            {
                EraseCircleStateRecord(recordIdentity);
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

        g_appearanceChangedMessage = RegisterWindowMessageW(
            L"Windhawk.TransferGlow.AppearanceChanged.1");
        if (!g_appearanceChangedMessage)
        {
            // The appearance window remains usable without this optional
            // refresh message; ordinary invalidation still updates painting.
            Wh_Log(L"Appearance refresh message registration failed error=%lu",
                   GetLastError());
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

    void RetryPendingInitialDisplayMode(COperationStatusTile *owner)
    {
        if (!owner ||
            g_unloading.load(std::memory_order_acquire))
        {
            return;
        }

        TransferSummaryState state{};
        if (!CopyRegisteredTransferState(owner, &state) ||
            !state.tile ||
            !state.operationTileRoot ||
            state.displayModeKnown)
        {
            return;
        }

        // Empty Recycle Bin can create/register the tile while its native
        // description and visibility state are still empty. Native tile
        // updates arrive later, so retry classification and initialization
        // only while the initial display mode is still unresolved.
        RefreshDeleteLikeOperationKind(owner, state);

        TransferSummaryState refreshedState{};
        if (CopyRegisteredTransferState(owner, &refreshedState) &&
            !refreshedState.displayModeKnown)
        {
            InitializeRegisteredDisplayMode(owner);
        }
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
            RetryPendingInitialDisplayMode(thisPtr);
        }
        return result;
    }

    HRESULT __cdecl COperationDataProvider_WriteCurrentItem_Hook(
        COperationDataProvider *thisPtr,
        IShellItem *currentItem)
    {
        bool trackCurrentFile = g_showCurrentFileProgressBar;

        if (trackCurrentFile && currentItem)
        {
            currentItem->AddRef();
        }

        HRESULT result =
            COperationDataProvider_WriteCurrentItem_Original(
                thisPtr, currentItem);

        if (trackCurrentFile &&
            !g_unloading.load(std::memory_order_acquire))
        {
            PWSTR filePath = nullptr;

            HRESULT pathHr = currentItem
                                 ? currentItem->GetDisplayName(
                                       SIGDN_FILESYSPATH, &filePath)
                                 : E_POINTER;

            unsigned long long fileSize = 0;
            BOOL fileExists = FALSE;

            if (SUCCEEDED(pathHr) && filePath)
            {
                WIN32_FILE_ATTRIBUTE_DATA data{};
                fileExists = GetFileAttributesExW(
                    filePath, GetFileExInfoStandard, &data);

                if (fileExists &&
                    !(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                    fileSize =
                        (static_cast<unsigned long long>(
                             data.nFileSizeHigh)
                         << 32) |
                        static_cast<unsigned long long>(
                            data.nFileSizeLow);
                }
            }

            {
                std::lock_guard<std::mutex> lock(g_transferSummaryMutex);

                auto binding = std::find_if(
                    g_operationDataBindings.begin(),
                    g_operationDataBindings.end(),
                    [thisPtr](OperationDataBinding const &entry)
                    { return entry.writer == thisPtr; });

                if (binding == g_operationDataBindings.end())
                {
                    OperationDataBinding entry{};
                    entry.writer = thisPtr;
                    g_operationDataBindings.push_back(entry);
                    binding = std::prev(g_operationDataBindings.end());
                }

                unsigned long long baseline = 0;

                if (binding->currentFileValid)
                {
                    // Sequential normal copy/move: the next file begins
                    // exactly where the previous file ended. This avoids
                    // losing the first chunk when WriteCurrentItem arrives
                    // slightly after aggregate byte progress has advanced.
                    baseline =
                        binding->currentFileStartBytes +
                        binding->currentFileSize;
                }
                else if (binding->latestProgressValid)
                {
                    baseline = binding->latestCompletedBytes;
                }

                binding->currentFileStartBytes = baseline;
                binding->currentFileSize = fileSize;
                binding->currentFileValid = fileSize > 0;
                PublishSharedProgress(*binding);
            }

            if (filePath)
            {
                FreeShellMemory(filePath);
            }
        }

        if (trackCurrentFile && currentItem)
        {
            currentItem->Release();
        }

        return result;
    }

    HRESULT __cdecl COperationDataProvider_WriteProgressValues_Hook(
        COperationDataProvider *thisPtr,
        unsigned long long value0,
        unsigned long long value1,
        unsigned long long value2,
        unsigned long long value3,
        unsigned long long value4,
        unsigned long long value5)
    {
        if (g_showCurrentFileProgressBar)
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);

            auto binding = std::find_if(
                g_operationDataBindings.begin(),
                g_operationDataBindings.end(),
                [thisPtr](OperationDataBinding const &entry)
                { return entry.writer == thisPtr; });

            if (binding == g_operationDataBindings.end())
            {
                OperationDataBinding entry{};
                entry.writer = thisPtr;
                g_operationDataBindings.push_back(entry);
                binding = std::prev(g_operationDataBindings.end());
            }

            binding->latestCompletedItems = value2;
            binding->latestTotalItems = value3;
            binding->latestCompletedBytes = value4;
            binding->latestTotalBytes = value5;
            binding->latestProgressValid = true;
            PublishSharedProgress(*binding);
        }

        return COperationDataProvider_WriteProgressValues_Original(
            thisPtr,
            value0, value1, value2,
            value3, value4, value5);
    }

    void __cdecl COperationStatusTile_RefreshDisplayProgress_Hook(
        COperationStatusTile *thisPtr,
        unsigned long long value0,
        unsigned long long value1)
    {

        COperationStatusTile_RefreshDisplayProgress_Original(
            thisPtr, value0, value1);

        RetryPendingInitialDisplayMode(thisPtr);
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
            RetryPendingInitialDisplayMode(thisPtr);
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

        TransferSummaryState preNativeState{};
        bool releasedNativeVisibilityForModeChange =
            ownerResolved && canonicalOwner &&
            CopyRegisteredTransferState(
                canonicalOwner, &preNativeState);

        if (releasedNativeVisibilityForModeChange)
        {
            // Give Explorer back exactly the elements we suppressed
            // before asking it to establish the new compact/expanded
            // visibility state.
            SetNativeDuplicatePresentation(
                preNativeState, false);
        }

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
        else if (ownerStillResolved &&
                 releasedNativeVisibilityForModeChange)
        {
            TransferSummaryState currentState{};
            if (CopyRegisteredTransferState(
                    canonicalOwner, &currentState) &&
                currentState.tile)
            {
                HWND hostWindow = nullptr;
                if (GetUniqueRegisteredCircleHost(
                        currentState.tile, &hostWindow) &&
                    hostWindow &&
                    !IsHostInSpecialOperationState(hostWindow) &&
                    IsSingleNormalProgressTileForHost(
                        currentState.tile, hostWindow))
                {
                    SetNativeDuplicatePresentation(
                        currentState, true);
                }
            }
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

        TransferSummaryState candidateState{};
        candidateState.owner = thisPtr;
        candidateState.tile = operationTile;
        candidateState.operationTileRoot = operationTileRoot;
        candidateState.tileHeaderRoot = state.tileHeaderRoot;

        NormalProgressLayoutElements layoutElements{};
        bool completeNormalLayout =
            DiscoverNormalProgressLayout(candidateState, &layoutElements);
        bool deleteLike = false;
        bool deleteLikeKnown =
            TryGetDeleteLikeOperationKind(candidateState, &deleteLike);
        bool hierarchyValid =
            deleteLikeKnown && deleteLike
                ? ValidateDeleteLikeProgressHierarchy(
                      layoutElements, eventId, false)
                : (completeNormalLayout &&
                   ValidateNormalProgressHierarchy(
                       layoutElements, eventId, false));
        if (!hierarchyValid)
        {
            Wh_Log(L"eventId=%llu presentation skipped "
                   L"reason=unsupported-layout deleteLike=%s",
                   eventId,
                   deleteLikeKnown && deleteLike ? L"yes" : L"no");
            return result;
        }

        // Register native state first. Custom HWNDs, Glass, positioning,
        // and host geometry are activated only after the native display
        // mode has been positively identified.
        ScopedPresentationActivation activationGuard;
        if (g_unloading.load(std::memory_order_acquire))
        {
            return result;
        }

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
        COperationDataProvider_WriteCurrentItem_t writeCurrentItem;
        COperationDataProvider_WriteProgressValues_t writeProgressValues;
        COperationStatusTile_RefreshDisplayProgress_t refreshDisplayProgress;
        COperationStatusTile_UpdateSummary_t updateSummary;
        COperationStatusTile_SetTileDisplayMode_t setTileDisplayMode;
        COperationStatusTileRateCalculator_CalculateRate_t calculateRate;
    };

    HMODULE g_ownedDui70Module = nullptr;
    HMODULE g_ownedShell32Module = nullptr;

    void ReleaseOwnedSkinTargetModules()
    {
        if (g_ownedShell32Module)
        {
            FreeLibrary(g_ownedShell32Module);
            g_ownedShell32Module = nullptr;
        }

        if (g_ownedDui70Module)
        {
            FreeLibrary(g_ownedDui70Module);
            g_ownedDui70Module = nullptr;
        }
    }
    bool ResolveSkinTargets(SkinTargets *targets)
    {
        HMODULE dui70 = GetModuleHandleW(L"dui70.dll");
        if (!dui70)
        {
            dui70 =
                LoadLibraryExW(
                    L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

            if (dui70)
            {
                g_ownedDui70Module = dui70;
            }
        }

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
            Wh_Log(L"Compatibility: required dui70 exports missing; "
                   L"this Windows build may have changed DirectUI ABI. "
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

        HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
        if (!shell32)
        {
            shell32 =
                LoadLibraryExW(
                    L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

            if (shell32)
            {
                g_ownedShell32Module = shell32;
            }
        }

        if (!shell32)
        {
            Wh_Log(L"Skin setup failed: unable to load shell32.dll "
                   L"error=%lu",
                   GetLastError());
            return false;
        }

        WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
            {
                {LR"(private: long __cdecl COperationStatusTile::_CreateTileElement(unsigned long,unsigned long,class DirectUI::Element *))"},
                &targets->createTileElement,
                nullptr,
                false,
            },
            {
                {LR"(public: static struct DirectUI::PropertyInfo const * __cdecl OperationTileElement::ProgressPositionProp(void))"},
                &targets->progressPositionProp,
                nullptr,
                false,
            },
            {
                {LR"(private: struct HWND__ * __cdecl OperationTileElement::_GetProgressHWND(void))"},
                &targets->getProgressHWND,
                nullptr,
                false,
            },
            {
                {LR"(public: virtual void __cdecl OperationTileElement::OnPropertyChanged(struct DirectUI::PropertyInfo const *,int,class DirectUI::Value *,class DirectUI::Value *))"},
                &targets->onPropertyChanged,
                nullptr,
                false,
            },
            {
                {LR"(public: virtual __cdecl OperationTileElement::~OperationTileElement(void))"},
                &targets->operationTileDestructor,
                nullptr,
                false,
            },
            {
                {LR"(private: void __cdecl COperationStatusTile::_RefreshDisplayProgress(unsigned __int64,unsigned __int64))"},
                &targets->refreshDisplayProgress,
                nullptr,
                false,
            },

            {
                {LR"(public: virtual long __cdecl COperationDataProvider::WriteCurrentItem(struct IShellItem *))"},
                &targets->writeCurrentItem,
                nullptr,
                false,
            },
            {
                {LR"(public: virtual long __cdecl COperationDataProvider::WriteProgressValues(unsigned __int64,unsigned __int64,unsigned __int64,unsigned __int64,unsigned __int64,unsigned __int64))"},
                &targets->writeProgressValues,
                nullptr,
                false,
            },
            {
                {LR"(private: long __cdecl COperationStatusTile::_UpdateRemainingItemsAndSize(unsigned __int64,unsigned __int64,unsigned __int64,unsigned __int64))"},
                &targets->updateRemainingItemsAndSize,
                nullptr,
                false,
            },
            {
                {LR"(private: long __cdecl COperationStatusTile::_UpdateSummary(unsigned short const *))"},
                &targets->updateSummary,
                nullptr,
                false,
            },
            {
                {LR"(public: virtual long __cdecl COperationStatusTile::SetTileDisplayMode(bool))"},
                &targets->setTileDisplayMode,
                nullptr,
                false,
            },
            {
                {LR"(private: double __cdecl COperationStatusTileRateCalculator::_CalculateRate(unsigned __int64,unsigned __int64,unsigned __int64,unsigned __int64,unsigned __int64,unsigned __int64,unsigned __int64,double *))"},
                &targets->calculateRate,
                nullptr,
                true, // Optional: some shell32 PDBs do not expose this private helper.
            },
        };

        if (!WindhawkUtils::HookSymbols(shell32, shell32DllHooks,
                                        ARRAYSIZE(shell32DllHooks)) ||
            !targets->createTileElement || !targets->progressPositionProp ||
            !targets->getProgressHWND || !targets->onPropertyChanged ||
            !targets->operationTileDestructor ||
            !targets->writeCurrentItem ||
            !targets->writeProgressValues ||
            !targets->refreshDisplayProgress ||
            !targets->updateRemainingItemsAndSize || !targets->updateSummary ||
            !targets->setTileDisplayMode)
        {
            Wh_Log(L"Compatibility: shell32 symbol resolution failed. "
                   L"CreateTile=%p ProgressProp=%p GetProgressHWND=%p "
                   L"OnPropertyChanged=%p Destructor=%p RefreshProgress=%p "
                   L"WriteCurrentItem=%p WriteProgressValues=%p "
                   L"UpdateRemaining=%p UpdateSummary=%p SetDisplayMode=%p",
                   reinterpret_cast<void *>(targets->createTileElement),
                   reinterpret_cast<void *>(targets->progressPositionProp),
                   reinterpret_cast<void *>(targets->getProgressHWND),
                   reinterpret_cast<void *>(targets->onPropertyChanged),
                   reinterpret_cast<void *>(targets->operationTileDestructor),
                   reinterpret_cast<void *>(targets->refreshDisplayProgress),
                   reinterpret_cast<void *>(targets->writeCurrentItem),
                   reinterpret_cast<void *>(targets->writeProgressValues),
                   reinterpret_cast<void *>(targets->updateRemainingItemsAndSize),
                   reinterpret_cast<void *>(targets->updateSummary),
                   reinterpret_cast<void *>(targets->setTileDisplayMode));
            Wh_Log(L"Compatibility: refusing to install partial hooks on "
                   L"an unverified Explorer build; native operation UI "
                   L"must remain in control.");
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
                targets.writeCurrentItem,
                COperationDataProvider_WriteCurrentItem_Hook,
                &COperationDataProvider_WriteCurrentItem_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook "
                   L"shell32!COperationDataProvider::WriteCurrentItem");
            return false;
        }

        if (!WindhawkUtils::SetFunctionHook(
                targets.writeProgressValues,
                COperationDataProvider_WriteProgressValues_Hook,
                &COperationDataProvider_WriteProgressValues_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook "
                   L"shell32!COperationDataProvider::WriteProgressValues");
            return false;
        }

        if (!WindhawkUtils::SetFunctionHook(
                targets.refreshDisplayProgress,
                COperationStatusTile_RefreshDisplayProgress_Hook,
                &COperationStatusTile_RefreshDisplayProgress_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook "
                   L"shell32!COperationStatusTile::_RefreshDisplayProgress");
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

        // This private helper is not present in every shell32 PDB/build.
        // The visual skin does not depend on it; without the hook, native
        // rate sampling is unavailable and the UI can show its safe
        // "Calculating..." fallback for speed/time remaining.
        if (targets.calculateRate)
        {
            if (!WindhawkUtils::SetFunctionHook(
                    targets.calculateRate,
                    COperationStatusTileRateCalculator_CalculateRate_Hook,
                    &COperationStatusTileRateCalculator_CalculateRate_Original))
            {
                Wh_Log(L"Compatibility: native rate hook could not be installed; "
                       L"continuing without native speed/time sampling");
            }
        }
        else
        {
            Wh_Log(L"Compatibility: _CalculateRate symbol unavailable; "
                   L"continuing without native speed/time sampling");
        }

        return true;
    }

} // namespace

// Report the actual OS build without relying on manifest-sensitive
// GetVersionEx behavior. This is diagnostic only: it does not assume that
// private Explorer symbols are compatible just because the build number
// looks familiar.
namespace
{
    void LogWindowsBuild()
    {
        using RtlGetVersion_t = LONG(WINAPI *)(OSVERSIONINFOW *);
        HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
        auto rtlGetVersion = ntdll
            ? reinterpret_cast<RtlGetVersion_t>(
                  GetProcAddress(ntdll, "RtlGetVersion"))
            : nullptr;

        if (!rtlGetVersion)
        {
            Wh_Log(L"Compatibility: RtlGetVersion unavailable; "
                   L"cannot report exact Windows build");
            return;
        }

        OSVERSIONINFOW version{};
        version.dwOSVersionInfoSize = sizeof(version);
        LONG status = rtlGetVersion(&version);
        if (status < 0)
        {
            Wh_Log(L"Compatibility: RtlGetVersion failed status=0x%08X",
                   static_cast<unsigned int>(status));
            return;
        }

        Wh_Log(L"Compatibility: Windows %lu.%lu build %lu platform=%lu "
               L"service-pack='%s'",
               version.dwMajorVersion, version.dwMinorVersion,
               version.dwBuildNumber, version.dwPlatformId,
               version.szCSDVersion);
        if (version.dwBuildNumber >= 26300)
        {
            Wh_Log(L"Compatibility: Windows 11 26H2-era build detected; "
                   L"private Explorer/DirectUI interfaces will be "
                   L"validated by symbol/export resolution");
        }
    }

    constexpr wchar_t kAppearanceWindowClassName[] =
        L"TransferGlow.AppearanceWindow.1";
    constexpr wchar_t kColorWheelWindowClassName[] =
        L"TransferGlow.ColorWheelWindow.1";
    constexpr int kAppearanceHotkeyId = 0x5447;
    constexpr int kAppearanceBaseWidth = 760;
    constexpr int kAppearanceBaseHeight = 440;
    constexpr int kColorWheelBaseWidth = 420;
    constexpr int kColorWheelBaseHeight = 440;
    constexpr int kDefaultAppearanceOpacity = 0;
    constexpr COLORREF kAppearanceBackground = RGB(17, 17, 17);
    constexpr COLORREF kAppearanceCard = RGB(25, 25, 25);
    constexpr COLORREF kAppearanceBorder = RGB(45, 45, 45);
    constexpr COLORREF kAppearanceText = RGB(240, 244, 250);
    constexpr COLORREF kAppearanceSecondary = RGB(164, 176, 194);

    enum class AppearanceDragMode
    {
        None,
        Hue,
        SaturationValue,
        Opacity
    };

    struct AppearanceWindowCreateParams;

    struct AppearanceWindowState
    {
        Gdiplus::Bitmap *hueWheel = nullptr;
        AppearanceWindowCreateParams *createParams = nullptr;
        HWND colorPickerWindow = nullptr;
        double hue = 210.0;
        double saturation = 0.7;
        double value = 0.9;
        int opacity = 0;
        AppearanceDragMode dragMode = AppearanceDragMode::None;
        bool hasCustomColor = false;
        ULONGLONG lastTintRefresh = 0;
        ULONGLONG lastOperationRefresh = 0;
        bool ownerDestroying = false;
    };

    struct AppearanceWindowCreateParams
    {
        AppearanceWindowState *state = nullptr;
        bool attached = false;
        bool cleanedUp = false;
    };

    Gdiplus::Color ToGdiColor(COLORREF color, BYTE alpha = 255)
    {
        return Gdiplus::Color(
            alpha, GetRValue(color), GetGValue(color), GetBValue(color));
    }

    COLORREF HsvToColor(double hue, double saturation, double value)
    {
        hue = std::fmod(hue, 360.0);
        if (hue < 0.0)
        {
            hue += 360.0;
        }

        saturation = std::clamp(saturation, 0.0, 1.0);
        value = std::clamp(value, 0.0, 1.0);
        double chroma = value * saturation;
        double section = hue / 60.0;
        double second = chroma * (1.0 - std::fabs(std::fmod(section, 2.0) - 1.0));
        double red = 0.0;
        double green = 0.0;
        double blue = 0.0;

        if (section < 1.0)
        {
            red = chroma;
            green = second;
        }
        else if (section < 2.0)
        {
            red = second;
            green = chroma;
        }
        else if (section < 3.0)
        {
            green = chroma;
            blue = second;
        }
        else if (section < 4.0)
        {
            green = second;
            blue = chroma;
        }
        else if (section < 5.0)
        {
            red = second;
            blue = chroma;
        }
        else
        {
            red = chroma;
            blue = second;
        }

        double match = value - chroma;
        return RGB(
            static_cast<BYTE>(std::lround((red + match) * 255.0)),
            static_cast<BYTE>(std::lround((green + match) * 255.0)),
            static_cast<BYTE>(std::lround((blue + match) * 255.0)));
    }

    void ColorToHsv(COLORREF color, double *hue, double *saturation,
                    double *value)
    {
        double red = GetRValue(color) / 255.0;
        double green = GetGValue(color) / 255.0;
        double blue = GetBValue(color) / 255.0;
        double maximum = std::max({red, green, blue});
        double minimum = std::min({red, green, blue});
        double delta = maximum - minimum;

        double selectedHue = 0.0;
        if (delta > 0.00001)
        {
            if (maximum == red)
            {
                selectedHue = 60.0 * std::fmod((green - blue) / delta, 6.0);
            }
            else if (maximum == green)
            {
                selectedHue = 60.0 * (((blue - red) / delta) + 2.0);
            }
            else
            {
                selectedHue = 60.0 * (((red - green) / delta) + 4.0);
            }
        }

        if (selectedHue < 0.0)
        {
            selectedHue += 360.0;
        }

        if (hue)
            *hue = selectedHue;
        if (saturation)
            *saturation = maximum <= 0.00001 ? 0.0 : delta / maximum;
        if (value)
            *value = maximum;
    }

    COLORREF ThemeDefaultAccent()
    {
        return g_settings.applyNativeColors
                   ? g_settings.theme.accent
                   : MakeSystemTheme().accent;
    }

    Gdiplus::Bitmap *CreateAppearanceHueWheel()
    {
        constexpr int size = 224;
        constexpr double center = (size - 1) / 2.0;
        constexpr double innerRadius = 89.0;
        constexpr double outerRadius = 108.0;
        auto bitmap = new Gdiplus::Bitmap(
            size, size, PixelFormat32bppARGB);
        if (bitmap->GetLastStatus() != Gdiplus::Ok)
        {
            delete bitmap;
            return nullptr;
        }

        Gdiplus::Rect bounds(0, 0, size, size);
        Gdiplus::BitmapData data{};
        if (bitmap->LockBits(
                &bounds,
                Gdiplus::ImageLockModeWrite,
                PixelFormat32bppARGB,
                &data) != Gdiplus::Ok)
        {
            delete bitmap;
            return nullptr;
        }

        for (int y = 0; y < size; ++y)
        {
            auto *row = reinterpret_cast<DWORD *>(
                static_cast<BYTE *>(data.Scan0) + y * data.Stride);
            for (int x = 0; x < size; ++x)
            {
                double dx = x - center;
                double dy = center - y;
                double radius = std::sqrt(dx * dx + dy * dy);
                double coverage = std::clamp(
                    std::min(radius - innerRadius, outerRadius - radius) *
                        1.7,
                    0.0,
                    1.0);
                if (coverage <= 0.0)
                {
                    row[x] = 0;
                    continue;
                }

                double hue = std::atan2(dy, dx) * 180.0 / 3.141592653589793;
                if (hue < 0.0)
                {
                    hue += 360.0;
                }
                COLORREF color = HsvToColor(hue, 1.0, 1.0);
                BYTE alpha = static_cast<BYTE>(
                    std::lround(coverage * 255.0));
                row[x] = Gdiplus::Color(
                             alpha,
                             GetRValue(color),
                             GetGValue(color),
                             GetBValue(color))
                             .GetValue();
            }
        }

        bitmap->UnlockBits(&data);
        return bitmap;
    }

    void FillRoundedRectangle(Gdiplus::Graphics &graphics,
                              Gdiplus::RectF rectangle,
                              float radius,
                              Gdiplus::Color color)
    {
        Gdiplus::GraphicsPath path;
        float diameter = radius * 2.0f;
        path.AddArc(rectangle.X, rectangle.Y, diameter, diameter, 180.0f, 90.0f);
        path.AddArc(rectangle.GetRight() - diameter, rectangle.Y,
                    diameter, diameter, 270.0f, 90.0f);
        path.AddArc(rectangle.GetRight() - diameter,
                    rectangle.GetBottom() - diameter,
                    diameter, diameter, 0.0f, 90.0f);
        path.AddArc(rectangle.X, rectangle.GetBottom() - diameter,
                    diameter, diameter, 90.0f, 90.0f);
        path.CloseFigure();
        Gdiplus::SolidBrush brush(color);
        graphics.FillPath(&brush, &path);
    }

    void DrawAppearanceText(Gdiplus::Graphics &graphics,
                            PCWSTR text,
                            float x,
                            float y,
                            float width,
                            float height,
                            float fontSize,
                            COLORREF color,
                            bool bold = false,
                            Gdiplus::StringAlignment alignment =
                                Gdiplus::StringAlignmentNear)
    {
        Gdiplus::Font font(
            L"Segoe UI", fontSize,
            bold ? Gdiplus::FontStyleBold : Gdiplus::FontStyleRegular,
            Gdiplus::UnitPixel);
        Gdiplus::SolidBrush brush(ToGdiColor(color));
        Gdiplus::RectF rectangle(x, y, width, height);
        Gdiplus::StringFormat format;
        format.SetAlignment(alignment);
        format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        graphics.DrawString(
            text, -1, &font, rectangle, &format, &brush);
    }

    COLORREF AppearanceSelectedColor(AppearanceWindowState const &state)
    {
        return HsvToColor(state.hue, state.saturation, state.value);
    }

    void DrawColorWheelSurface(Gdiplus::Graphics &graphics,
                               AppearanceWindowState const &state,
                               float centerX,
                               float centerY)
    {
        constexpr float wheelSize = 224.0f;
        Gdiplus::RectF wheelRect(
            centerX - wheelSize / 2.0f,
            centerY - wheelSize / 2.0f,
            wheelSize,
            wheelSize);
        if (state.hueWheel)
        {
            graphics.DrawImage(state.hueWheel, wheelRect);
        }

        constexpr float squareHalf = 56.0f;
        Gdiplus::RectF square(
            centerX - squareHalf,
            centerY - squareHalf,
            squareHalf * 2.0f,
            squareHalf * 2.0f);
        COLORREF hueColor = HsvToColor(state.hue, 1.0, 1.0);
        Gdiplus::SolidBrush hueBrush(ToGdiColor(hueColor));
        graphics.FillRectangle(&hueBrush, square);
        Gdiplus::LinearGradientBrush saturationGradient(
            square,
            Gdiplus::Color(255, 255, 255, 255),
            Gdiplus::Color(0, 255, 255, 255),
            Gdiplus::LinearGradientModeHorizontal);
        graphics.FillRectangle(&saturationGradient, square);
        Gdiplus::LinearGradientBrush valueGradient(
            square,
            Gdiplus::Color(0, 0, 0, 0),
            Gdiplus::Color(255, 0, 0, 0),
            Gdiplus::LinearGradientModeVertical);
        graphics.FillRectangle(&valueGradient, square);

        Gdiplus::Pen squareBorder(ToGdiColor(RGB(255, 255, 255), 170), 1.0f);
        graphics.DrawRectangle(&squareBorder, square);

        float markerX = square.X +
                        static_cast<float>(state.saturation) * square.Width;
        float markerY = square.Y +
                        static_cast<float>(1.0 - state.value) * square.Height;
        Gdiplus::Pen markerShadow(ToGdiColor(RGB(0, 0, 0), 220), 3.5f);
        Gdiplus::Pen markerLine(ToGdiColor(RGB(255, 255, 255)), 1.5f);
        graphics.DrawEllipse(
            &markerShadow, markerX - 7.0f, markerY - 7.0f, 14.0f, 14.0f);
        graphics.DrawEllipse(
            &markerLine, markerX - 7.0f, markerY - 7.0f, 14.0f, 14.0f);

        double hueRadians = state.hue * 3.141592653589793 / 180.0;
        float hueMarkerX = centerX +
                           static_cast<float>(std::cos(hueRadians) * 97.0);
        float hueMarkerY = centerY -
                           static_cast<float>(std::sin(hueRadians) * 97.0);
        Gdiplus::Pen hueMarkerShadow(ToGdiColor(RGB(0, 0, 0)), 4.0f);
        Gdiplus::Pen hueMarkerLine(ToGdiColor(RGB(255, 255, 255)), 2.0f);
        graphics.DrawEllipse(
            &hueMarkerShadow, hueMarkerX - 7.0f, hueMarkerY - 7.0f,
            14.0f, 14.0f);
        graphics.DrawEllipse(
            &hueMarkerLine, hueMarkerX - 7.0f, hueMarkerY - 7.0f,
            14.0f, 14.0f);
    }

    void DrawAppearanceWindow(HWND window,
                              AppearanceWindowState const &state,
                              HDC deviceContext)
    {
        RECT client{};
        GetClientRect(window, &client);
        UINT dpi = GetDpiForWindow(window);
        if (!dpi)
        {
            dpi = USER_DEFAULT_SCREEN_DPI;
        }
        float scale = static_cast<float>(dpi) / USER_DEFAULT_SCREEN_DPI;

        Gdiplus::Graphics graphics(deviceContext);
        graphics.SetPageUnit(Gdiplus::UnitPixel);
        graphics.ScaleTransform(scale, scale);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        graphics.SetTextRenderingHint(
            Gdiplus::TextRenderingHintAntiAliasGridFit);

        float logicalWidth = (client.right - client.left) / scale;
        float logicalHeight = (client.bottom - client.top) / scale;
        Gdiplus::SolidBrush background(ToGdiColor(kAppearanceBackground));
        graphics.FillRectangle(
            &background, 0.0f, 0.0f, logicalWidth, logicalHeight);

        DrawAppearanceText(
            graphics, L"TransferGlow", 28.0f, 18.0f, 350.0f, 31.0f,
            23.0f, kAppearanceText, true);
        DrawAppearanceText(
            graphics, L"PERSONALIZAÇÃO", 30.0f, 49.0f,
            400.0f, 20.0f, 10.0f, kAppearanceSecondary, true);

        Gdiplus::RectF card(22.0f, 82.0f, 716.0f, 302.0f);
        FillRoundedRectangle(
            graphics, card, 16.0f, ToGdiColor(kAppearanceCard));
        Gdiplus::Pen border(ToGdiColor(kAppearanceBorder), 1.0f);
        Gdiplus::GraphicsPath outline;
        outline.AddArc(22.0f, 82.0f, 32.0f, 32.0f, 180.0f, 90.0f);
        outline.AddArc(706.0f, 82.0f, 32.0f, 32.0f, 270.0f, 90.0f);
        outline.AddArc(706.0f, 352.0f, 32.0f, 32.0f, 0.0f, 90.0f);
        outline.AddArc(22.0f, 352.0f, 32.0f, 32.0f, 90.0f, 90.0f);
        outline.CloseFigure();
        graphics.DrawPath(&border, &outline);

        DrawAppearanceText(
            graphics, L"Intensidade / Transparência", 38.0f, 99.0f,
            450.0f, 24.0f, 14.0f, kAppearanceText, true);
        DrawAppearanceText(
            graphics, L"0%", 39.0f, 137.0f, 52.0f, 21.0f,
            10.5f, kAppearanceSecondary);
        std::wstring percentage = std::to_wstring(state.opacity) + L"%";
        DrawAppearanceText(
            graphics, percentage.c_str(), 315.0f, 134.0f,
            130.0f, 25.0f, 13.0f, kAppearanceText, true,
            Gdiplus::StringAlignmentCenter);
        DrawAppearanceText(
            graphics, L"100%", 666.0f, 137.0f, 52.0f, 21.0f,
            10.5f, kAppearanceSecondary, false,
            Gdiplus::StringAlignmentFar);

        constexpr float sliderStart = 40.0f;
        constexpr float sliderEnd = 720.0f;
        constexpr float sliderY = 171.0f;
        float sliderPosition = sliderStart +
                               (sliderEnd - sliderStart) *
                                   (state.opacity / 100.0f);
        FillRoundedRectangle(
            graphics,
            Gdiplus::RectF(sliderStart, sliderY - 4.5f,
                           sliderEnd - sliderStart, 9.0f),
            4.5f, ToGdiColor(RGB(53, 53, 53)));
        if (sliderPosition > sliderStart)
        {
            FillRoundedRectangle(
                graphics,
                Gdiplus::RectF(sliderStart, sliderY - 4.5f,
                               sliderPosition - sliderStart, 9.0f),
                4.5f, ToGdiColor(RGB(55, 139, 255)));
        }
        Gdiplus::SolidBrush thumb(ToGdiColor(RGB(55, 139, 255)));
        graphics.FillEllipse(
            &thumb, sliderPosition - 7.0f, sliderY - 7.0f, 14.0f, 14.0f);

        Gdiplus::Pen divider(ToGdiColor(RGB(45, 45, 45)), 1.0f);
        graphics.DrawLine(&divider, 38.0f, 207.0f, 722.0f, 207.0f);
        DrawAppearanceText(
            graphics, L"Cor personalizada", 38.0f, 221.0f,
            250.0f, 24.0f, 13.0f, kAppearanceText, true);

        COLORREF selectedColor = AppearanceSelectedColor(state);
        Gdiplus::SolidBrush swatch(ToGdiColor(selectedColor));
        Gdiplus::Pen swatchBorder(ToGdiColor(RGB(255, 255, 255)), 2.0f);
        graphics.FillEllipse(&swatch, 40.0f, 255.0f, 76.0f, 76.0f);
        graphics.DrawEllipse(&swatchBorder, 40.0f, 255.0f, 76.0f, 76.0f);
        DrawAppearanceText(
            graphics, L"Círculo de cores", 132.0f, 267.0f,
            320.0f, 25.0f, 13.0f, kAppearanceText, true);
        DrawAppearanceText(
            graphics, L"Clique para abrir e escolher qualquer cor.",
            132.0f, 292.0f, 390.0f, 24.0f, 10.5f,
            kAppearanceSecondary);

        FillRoundedRectangle(
            graphics, Gdiplus::RectF(582.0f, 333.0f, 136.0f, 31.0f),
            8.0f, ToGdiColor(RGB(31, 40, 53)));
        DrawAppearanceText(
            graphics, L"Restaurar padrões", 590.0f, 337.0f,
            120.0f, 22.0f, 10.0f, kAppearanceText, true,
            Gdiplus::StringAlignmentCenter);

        DrawAppearanceText(
            graphics,
            L"Ative Enable customization no Windhawk para aplicar a cor escolhida.",
            25.0f, 394.0f, 710.0f, 18.0f, 9.5f,
            kAppearanceSecondary);
        DrawAppearanceText(
            graphics,
            L"Intensidade: Acrylic, Blur e Mica. Solid permanece opaco.",
            25.0f, 412.0f, 710.0f, 18.0f, 9.5f,
            kAppearanceSecondary);
    }

    void DrawColorWheelWindow(HWND window,
                              AppearanceWindowState const &state,
                              HDC deviceContext)
    {
        RECT client{};
        GetClientRect(window, &client);
        UINT dpi = GetDpiForWindow(window);
        if (!dpi)
        {
            dpi = USER_DEFAULT_SCREEN_DPI;
        }
        float scale = static_cast<float>(dpi) / USER_DEFAULT_SCREEN_DPI;

        Gdiplus::Graphics graphics(deviceContext);
        graphics.SetPageUnit(Gdiplus::UnitPixel);
        graphics.ScaleTransform(scale, scale);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        graphics.SetTextRenderingHint(
            Gdiplus::TextRenderingHintAntiAliasGridFit);
        float logicalWidth = (client.right - client.left) / scale;
        float logicalHeight = (client.bottom - client.top) / scale;
        Gdiplus::SolidBrush background(ToGdiColor(kAppearanceBackground));
        graphics.FillRectangle(
            &background, 0.0f, 0.0f, logicalWidth, logicalHeight);

        DrawAppearanceText(
            graphics, L"Escolha uma cor", 22.0f, 15.0f,
            360.0f, 28.0f, 18.0f, kAppearanceText, true);
        DrawAppearanceText(
            graphics, L"Arraste na roda e na área interna para ajustar.",
            24.0f, 45.0f, 370.0f, 19.0f, 10.0f,
            kAppearanceSecondary);
        DrawColorWheelSurface(graphics, state, 210.0f, 183.0f);

        COLORREF selectedColor = AppearanceSelectedColor(state);
        Gdiplus::SolidBrush swatch(ToGdiColor(selectedColor));
        Gdiplus::Pen swatchBorder(ToGdiColor(RGB(255, 255, 255), 170), 1.5f);
        graphics.FillEllipse(&swatch, 32.0f, 326.0f, 38.0f, 38.0f);
        graphics.DrawEllipse(&swatchBorder, 32.0f, 326.0f, 38.0f, 38.0f);
        DrawAppearanceText(
            graphics, L"Prévia da cor", 83.0f, 330.0f,
            170.0f, 29.0f, 11.0f, kAppearanceText, true);
        FillRoundedRectangle(
            graphics, Gdiplus::RectF(286.0f, 328.0f, 100.0f, 34.0f),
            9.0f, ToGdiColor(RGB(42, 53, 70)));
        DrawAppearanceText(
            graphics, L"Concluído", 292.0f, 333.0f,
            88.0f, 24.0f, 10.5f, kAppearanceText, true,
            Gdiplus::StringAlignmentCenter);
        DrawAppearanceText(
            graphics, L"A cor é aplicada enquanto você escolhe.",
            28.0f, 382.0f, 365.0f, 20.0f, 9.5f,
            kAppearanceSecondary);
    }

    // GDI+ paints the complete client surface into a memory bitmap first.
    // Blitting that finished frame in WM_PAINT prevents the custom windows
    // from exposing partially rendered gradients when the mouse is dragged
    // quickly or Windows coalesces paint messages.
    template <typename DrawCallback>
    void DrawWindowBuffered(HWND window, HDC targetDc, DrawCallback draw)
    {
        RECT client{};
        if (!GetClientRect(window, &client))
        {
            return;
        }

        int width = client.right - client.left;
        int height = client.bottom - client.top;
        if (width <= 0 || height <= 0)
        {
            return;
        }

        HDC memoryDc = CreateCompatibleDC(targetDc);
        if (!memoryDc)
        {
            draw(targetDc);
            return;
        }

        HBITMAP bitmap = CreateCompatibleBitmap(targetDc, width, height);
        if (!bitmap)
        {
            DeleteDC(memoryDc);
            draw(targetDc);
            return;
        }

        HGDIOBJ previousBitmap = SelectObject(memoryDc, bitmap);
        if (!previousBitmap || previousBitmap == HGDI_ERROR)
        {
            DeleteObject(bitmap);
            DeleteDC(memoryDc);
            draw(targetDc);
            return;
        }

        draw(memoryDc);
        if (!BitBlt(targetDc, 0, 0, width, height,
                    memoryDc, 0, 0, SRCCOPY))
        {
            // Retain a usable UI if a driver or GDI allocation rejects the
            // buffer blit; this path is only a fallback for the current frame.
            draw(targetDc);
        }

        SelectObject(memoryDc, previousBitmap);
        DeleteObject(bitmap);
        DeleteDC(memoryDc);
    }

    bool PointInHueWheel(int x, int y, double centerX, double centerY)
    {
        double dx = x - centerX;
        double dy = centerY - y;
        double radius = std::sqrt(dx * dx + dy * dy);
        return radius >= 85.0 && radius <= 108.0;
    }

    void UpdateHueFromPoint(AppearanceWindowState *state,
                            int x,
                            int y,
                            double centerX,
                            double centerY)
    {
        if (!state)
            return;
        double dx = x - centerX;
        double dy = centerY - y;
        double hue = std::atan2(dy, dx) * 180.0 / 3.141592653589793;
        if (hue < 0.0)
        {
            hue += 360.0;
        }
        state->hue = hue;
    }

    bool PointInSaturationValueSquare(int x,
                                      int y,
                                      double centerX,
                                      double centerY)
    {
        return x >= centerX - 56.0 && x <= centerX + 56.0 &&
               y >= centerY - 56.0 && y <= centerY + 56.0;
    }

    void UpdateSaturationValueFromPoint(
        AppearanceWindowState *state,
        int x,
        int y,
        double centerX,
        double centerY)
    {
        if (!state)
            return;
        state->saturation =
            std::clamp((x - (centerX - 56.0)) / 112.0, 0.0, 1.0);
        state->value = 1.0 - std::clamp(
            (y - (centerY - 56.0)) / 112.0, 0.0, 1.0);
    }

    void UpdateOpacityFromPoint(AppearanceWindowState *state, int x)
    {
        if (!state)
            return;
        double normalized = std::clamp((x - 40.0) / 680.0, 0.0, 1.0);
        state->opacity = static_cast<int>(std::lround(normalized * 100.0));
    }

    bool SaveAppearanceColor(COLORREF color)
    {
        wchar_t value[16]{};
        wsprintfW(
            value, L"#FF%02X%02X%02X",
            GetRValue(color), GetGValue(color), GetBValue(color));
        return Wh_SetStringValue(kAppearanceColorValueName, value) != FALSE;
    }

    void SaveAppearanceWindowSettings(AppearanceWindowState const &state)
    {
        bool colorSaved = state.hasCustomColor
                              ? SaveAppearanceColor(
                                    AppearanceSelectedColor(state))
                              : Wh_SetStringValue(
                                    kAppearanceColorValueName,
                                    L"theme") != FALSE;
        BOOL opacitySaved = Wh_SetIntValue(
            kAppearanceOpacityValueName,
            std::clamp(state.opacity, 0, 100));
        if (!colorSaved || !opacitySaved)
        {
            Wh_Log(L"Appearance preferences could not be fully saved");
        }
    }

    void ApplyAppearanceWindowChanges(
        HWND window,
        AppearanceWindowState *state,
        bool refreshGlassTint,
        bool save,
        bool refreshOperationWindows = true)
    {
        if (!state)
            return;

        if (state->hasCustomColor)
        {
            g_runtimeThemeColor.store(
                static_cast<int>(AppearanceSelectedColor(*state)),
                std::memory_order_relaxed);
        }
        else
        {
            g_runtimeThemeColor.store(-1, std::memory_order_relaxed);
        }
        g_runtimeGlassStrength.store(
            std::clamp(state->opacity, 0, 100),
            std::memory_order_relaxed);

        if (save)
        {
            SaveAppearanceWindowSettings(*state);
        }

        if (refreshOperationWindows)
        {
            state->lastOperationRefresh = GetTickCount64();
            if (refreshGlassTint)
            {
                state->lastTintRefresh = state->lastOperationRefresh;
            }
            RefreshAppearanceWindows(refreshGlassTint);
        }
        if (window && IsWindow(window))
        {
            InvalidateRect(window, nullptr, FALSE);
        }
        if (state->colorPickerWindow && IsWindow(state->colorPickerWindow))
        {
            InvalidateRect(state->colorPickerWindow, nullptr, FALSE);
        }
    }

    void OpenColorPicker(HWND owner, AppearanceWindowState *state)
    {
        if (!owner || !IsWindow(owner) || !state)
        {
            return;
        }
        if (state->colorPickerWindow && IsWindow(state->colorPickerWindow))
        {
            ShowWindow(state->colorPickerWindow, SW_SHOWNORMAL);
            SetForegroundWindow(state->colorPickerWindow);
            return;
        }

        DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_POPUP;
        DWORD exStyle = WS_EX_TOOLWINDOW;
        UINT dpi = GetDpiForWindow(owner);
        if (!dpi)
        {
            dpi = USER_DEFAULT_SCREEN_DPI;
        }
        RECT outerRect{0, 0,
                       MulDiv(kColorWheelBaseWidth, dpi,
                              USER_DEFAULT_SCREEN_DPI),
                       MulDiv(kColorWheelBaseHeight, dpi,
                              USER_DEFAULT_SCREEN_DPI)};
        if (!AdjustWindowRectExForDpi(
                &outerRect, style, FALSE, exStyle, dpi))
        {
            AdjustWindowRectEx(&outerRect, style, FALSE, exStyle);
        }

        int width = outerRect.right - outerRect.left;
        int height = outerRect.bottom - outerRect.top;
        RECT ownerRect{};
        GetWindowRect(owner, &ownerRect);
        HMONITOR monitor = MonitorFromWindow(owner, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo{};
        monitorInfo.cbSize = sizeof(monitorInfo);
        GetMonitorInfoW(monitor, &monitorInfo);
        int x = ownerRect.right + 8;
        int y = ownerRect.top + (ownerRect.bottom - ownerRect.top - height) / 2;
        if (x + width > monitorInfo.rcWork.right)
        {
            x = ownerRect.left - width - 8;
        }
        x = std::clamp(
            x, static_cast<int>(monitorInfo.rcWork.left),
            std::max(static_cast<int>(monitorInfo.rcWork.left),
                     static_cast<int>(monitorInfo.rcWork.right) - width));
        y = std::clamp(
            y, static_cast<int>(monitorInfo.rcWork.top),
            std::max(static_cast<int>(monitorInfo.rcWork.top),
                     static_cast<int>(monitorInfo.rcWork.bottom) - height));

        HWND picker = CreateWindowExW(
            exStyle,
            kColorWheelWindowClassName,
            L"TransferGlow — Círculo de cores",
            style,
            x, y, width, height,
            owner, nullptr, g_settingsUiModule, state);
        if (!picker)
        {
            Wh_Log(L"Color wheel window creation failed error=%lu",
                   GetLastError());
            return;
        }

        ShowWindow(picker, SW_SHOWNORMAL);
        UpdateWindow(picker);
        SetForegroundWindow(picker);
    }

    void OpenAppearanceWindow()
    {
        if (g_settingsWindow && IsWindow(g_settingsWindow))
        {
            ShowWindow(g_settingsWindow, SW_SHOWNORMAL);
            SetForegroundWindow(g_settingsWindow);
            return;
        }

        auto state = new AppearanceWindowState{};
        state->hueWheel = CreateAppearanceHueWheel();
        if (!state->hueWheel)
        {
            delete state;
            Wh_Log(L"Appearance UI could not create its color wheel bitmap");
            return;
        }
        state->opacity = CurrentGlassStrength();
        int selectedColor =
            g_runtimeThemeColor.load(std::memory_order_relaxed);
        state->hasCustomColor = selectedColor >= 0;
        COLORREF color = state->hasCustomColor
                             ? static_cast<COLORREF>(selectedColor)
                             : ThemeDefaultAccent();
        ColorToHsv(color, &state->hue, &state->saturation, &state->value);

        DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
        DWORD exStyle = WS_EX_TOOLWINDOW;
        UINT dpi = GetDpiForSystem();
        if (!dpi)
        {
            dpi = USER_DEFAULT_SCREEN_DPI;
        }
        RECT outerRect{0, 0,
                       MulDiv(kAppearanceBaseWidth, dpi,
                              USER_DEFAULT_SCREEN_DPI),
                       MulDiv(kAppearanceBaseHeight, dpi,
                              USER_DEFAULT_SCREEN_DPI)};
        if (!AdjustWindowRectExForDpi(
                &outerRect, style, FALSE, exStyle, dpi))
        {
            AdjustWindowRectEx(&outerRect, style, FALSE, exStyle);
        }

        POINT cursor{};
        GetCursorPos(&cursor);
        HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo{};
        monitorInfo.cbSize = sizeof(monitorInfo);
        GetMonitorInfoW(monitor, &monitorInfo);
        int width = outerRect.right - outerRect.left;
        int height = outerRect.bottom - outerRect.top;
        int x = monitorInfo.rcWork.left +
                ((monitorInfo.rcWork.right - monitorInfo.rcWork.left) - width) / 2;
        int y = monitorInfo.rcWork.top +
                ((monitorInfo.rcWork.bottom - monitorInfo.rcWork.top) - height) / 2;

        AppearanceWindowCreateParams createParams{};
        createParams.state = state;
        HWND window = CreateWindowExW(
            exStyle,
            kAppearanceWindowClassName,
            L"TransferGlow — Aparência",
            style,
            x, y, width, height,
            nullptr, nullptr, g_settingsUiModule, &createParams);
        if (!window)
        {
            // WM_NCDESTROY owns cleanup if WM_NCCREATE accepted this state.
            // If creation failed earlier, release the allocation here.
            if (!createParams.attached || !createParams.cleanedUp)
            {
                state->createParams = nullptr;
                delete state->hueWheel;
                delete state;
            }
            Wh_Log(L"Appearance window creation failed error=%lu",
                   GetLastError());
            return;
        }

        // The stack create context is only valid during CreateWindowExW.
        state->createParams = nullptr;

        ShowWindow(window, SW_SHOWNORMAL);
        UpdateWindow(window);
        SetForegroundWindow(window);
    }

    LRESULT CALLBACK AppearanceWindowProc(
        HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        auto state = reinterpret_cast<AppearanceWindowState *>(
            GetWindowLongPtrW(window, GWLP_USERDATA));

        if (message == WM_NCCREATE)
        {
            auto create = reinterpret_cast<CREATESTRUCTW *>(lParam);
            auto createParams =
                static_cast<AppearanceWindowCreateParams *>(
                    create->lpCreateParams);
            if (!createParams || !createParams->state)
            {
                return FALSE;
            }
            state = createParams->state;
            createParams->attached = true;
            state->createParams = createParams;
            SetWindowLongPtrW(
                window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));
            g_settingsWindow = window;
            return TRUE;
        }

        switch (message)
        {
        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT:
        {
            PAINTSTRUCT paint{};
            HDC dc = BeginPaint(window, &paint);
            if (state)
            {
                DrawWindowBuffered(
                    window, dc,
                    [window, state](HDC target)
                    { DrawAppearanceWindow(window, *state, target); });
            }
            EndPaint(window, &paint);
            return 0;
        }

        case WM_LBUTTONDOWN:
        {
            if (!state)
                break;
            UINT dpi = GetDpiForWindow(window);
            if (!dpi)
                dpi = USER_DEFAULT_SCREEN_DPI;
            float scale = static_cast<float>(dpi) / USER_DEFAULT_SCREEN_DPI;
            int x = static_cast<int>(GET_X_LPARAM(lParam) / scale);
            int y = static_cast<int>(GET_Y_LPARAM(lParam) / scale);

            if (x >= 582 && x <= 718 && y >= 333 && y <= 364)
            {
                if (state->colorPickerWindow &&
                    IsWindow(state->colorPickerWindow))
                {
                    DestroyWindow(state->colorPickerWindow);
                }
                state->hasCustomColor = false;
                COLORREF defaultColor = ThemeDefaultAccent();
                ColorToHsv(defaultColor, &state->hue, &state->saturation,
                           &state->value);
                state->opacity = kDefaultAppearanceOpacity;
                ApplyAppearanceWindowChanges(window, state, true, true);
                return 0;
            }

            double previewDx = x - 78.0;
            double previewDy = y - 293.0;
            if (previewDx * previewDx + previewDy * previewDy <= 40.0 * 40.0)
            {
                OpenColorPicker(window, state);
                return 0;
            }
            else if (x >= 34 && x <= 726 && y >= 155 && y <= 190)
            {
                state->dragMode = AppearanceDragMode::Opacity;
                UpdateOpacityFromPoint(state, x);
            }
            else
            {
                break;
            }

            SetCapture(window);
            ApplyAppearanceWindowChanges(window, state, false, false);
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            if (!state || state->dragMode == AppearanceDragMode::None)
                break;
            UINT dpi = GetDpiForWindow(window);
            if (!dpi)
                dpi = USER_DEFAULT_SCREEN_DPI;
            float scale = static_cast<float>(dpi) / USER_DEFAULT_SCREEN_DPI;
            int x = static_cast<int>(GET_X_LPARAM(lParam) / scale);
            int y = static_cast<int>(GET_Y_LPARAM(lParam) / scale);
            if (state->dragMode == AppearanceDragMode::Opacity)
            {
                UpdateOpacityFromPoint(state, x);
            }

            ULONGLONG now = GetTickCount64();
            bool refreshOperationWindows =
                now - state->lastOperationRefresh >= 33;
            ApplyAppearanceWindowChanges(
                window, state, false, false, refreshOperationWindows);
            return 0;
        }

        case WM_LBUTTONUP:
            if (state && state->dragMode != AppearanceDragMode::None)
            {
                state->dragMode = AppearanceDragMode::None;
                if (GetCapture() == window)
                {
                    ReleaseCapture();
                }
                ApplyAppearanceWindowChanges(window, state, false, true);
                return 0;
            }
            break;

        case WM_CAPTURECHANGED:
            if (state && state->dragMode != AppearanceDragMode::None)
            {
                state->dragMode = AppearanceDragMode::None;
                ApplyAppearanceWindowChanges(window, state, false, true);
            }
            return 0;

        case WM_DPICHANGED:
        {
            RECT const *suggested = reinterpret_cast<RECT const *>(lParam);
            if (suggested)
            {
                SetWindowPos(
                    window, nullptr,
                    suggested->left, suggested->top,
                    suggested->right - suggested->left,
                    suggested->bottom - suggested->top,
                    SWP_NOZORDER | SWP_NOACTIVATE);
            }
            InvalidateRect(window, nullptr, FALSE);
            return 0;
        }

        case WM_CLOSE:
            DestroyWindow(window);
            return 0;

        case WM_NCDESTROY:
            if (state)
            {
                if (state->createParams)
                {
                    state->createParams->cleanedUp = true;
                    state->createParams = nullptr;
                }
                state->ownerDestroying = true;
                if (state->colorPickerWindow &&
                    IsWindow(state->colorPickerWindow))
                {
                    DestroyWindow(state->colorPickerWindow);
                }
                delete state->hueWheel;
                delete state;
            }
            if (g_settingsWindow == window)
            {
                g_settingsWindow = nullptr;
            }
            SetWindowLongPtrW(window, GWLP_USERDATA, 0);
            return DefWindowProcW(window, message, wParam, lParam);
        }

        return DefWindowProcW(window, message, wParam, lParam);
    }

    LRESULT CALLBACK ColorPickerWindowProc(
        HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        auto state = reinterpret_cast<AppearanceWindowState *>(
            GetWindowLongPtrW(window, GWLP_USERDATA));

        if (message == WM_NCCREATE)
        {
            auto create = reinterpret_cast<CREATESTRUCTW *>(lParam);
            state = static_cast<AppearanceWindowState *>(create->lpCreateParams);
            SetWindowLongPtrW(
                window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));
            if (state)
            {
                state->colorPickerWindow = window;
            }
            return TRUE;
        }

        switch (message)
        {
        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT:
        {
            PAINTSTRUCT paint{};
            HDC dc = BeginPaint(window, &paint);
            if (state)
            {
                DrawWindowBuffered(
                    window, dc,
                    [window, state](HDC target)
                    { DrawColorWheelWindow(window, *state, target); });
            }
            EndPaint(window, &paint);
            return 0;
        }

        case WM_LBUTTONDOWN:
        {
            if (!state)
                break;
            UINT dpi = GetDpiForWindow(window);
            if (!dpi)
                dpi = USER_DEFAULT_SCREEN_DPI;
            float scale = static_cast<float>(dpi) / USER_DEFAULT_SCREEN_DPI;
            int x = static_cast<int>(GET_X_LPARAM(lParam) / scale);
            int y = static_cast<int>(GET_Y_LPARAM(lParam) / scale);

            if (x >= 286 && x <= 386 && y >= 328 && y <= 362)
            {
                DestroyWindow(window);
                return 0;
            }

            if (PointInHueWheel(x, y, 210.0, 183.0))
            {
                state->dragMode = AppearanceDragMode::Hue;
                state->hasCustomColor = true;
                UpdateHueFromPoint(state, x, y, 210.0, 183.0);
            }
            else if (PointInSaturationValueSquare(x, y, 210.0, 183.0))
            {
                state->dragMode = AppearanceDragMode::SaturationValue;
                state->hasCustomColor = true;
                UpdateSaturationValueFromPoint(
                    state, x, y, 210.0, 183.0);
            }
            else
            {
                break;
            }

            SetCapture(window);
            ApplyAppearanceWindowChanges(
                g_settingsWindow, state, true, false);
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            if (!state || state->dragMode == AppearanceDragMode::None)
                break;
            UINT dpi = GetDpiForWindow(window);
            if (!dpi)
                dpi = USER_DEFAULT_SCREEN_DPI;
            float scale = static_cast<float>(dpi) / USER_DEFAULT_SCREEN_DPI;
            int x = static_cast<int>(GET_X_LPARAM(lParam) / scale);
            int y = static_cast<int>(GET_Y_LPARAM(lParam) / scale);
            if (state->dragMode == AppearanceDragMode::Hue)
            {
                UpdateHueFromPoint(state, x, y, 210.0, 183.0);
            }
            else if (state->dragMode == AppearanceDragMode::SaturationValue)
            {
                UpdateSaturationValueFromPoint(
                    state, x, y, 210.0, 183.0);
            }
            state->hasCustomColor = true;

            ULONGLONG now = GetTickCount64();
            bool refreshOperationWindows =
                now - state->lastOperationRefresh >= 33;
            bool refreshGlassTint = refreshOperationWindows &&
                                    now - state->lastTintRefresh >= 75;
            ApplyAppearanceWindowChanges(
                g_settingsWindow, state, refreshGlassTint, false,
                refreshOperationWindows);
            return 0;
        }

        case WM_LBUTTONUP:
            if (state && state->dragMode != AppearanceDragMode::None)
            {
                state->dragMode = AppearanceDragMode::None;
                if (GetCapture() == window)
                {
                    ReleaseCapture();
                }
                ApplyAppearanceWindowChanges(
                    g_settingsWindow, state, true, true);
                return 0;
            }
            break;

        case WM_CAPTURECHANGED:
            if (state && state->dragMode != AppearanceDragMode::None)
            {
                state->dragMode = AppearanceDragMode::None;
                ApplyAppearanceWindowChanges(
                    g_settingsWindow, state, true, true);
            }
            return 0;

        case WM_DPICHANGED:
        {
            RECT const *suggested = reinterpret_cast<RECT const *>(lParam);
            if (suggested)
            {
                SetWindowPos(
                    window, nullptr,
                    suggested->left, suggested->top,
                    suggested->right - suggested->left,
                    suggested->bottom - suggested->top,
                    SWP_NOZORDER | SWP_NOACTIVATE);
            }
            InvalidateRect(window, nullptr, FALSE);
            return 0;
        }

        case WM_CLOSE:
            DestroyWindow(window);
            return 0;

        case WM_NCDESTROY:
            if (state && state->colorPickerWindow == window)
            {
                state->colorPickerWindow = nullptr;
                if (!state->ownerDestroying && g_settingsWindow &&
                    IsWindow(g_settingsWindow))
                {
                    InvalidateRect(g_settingsWindow, nullptr, FALSE);
                }
            }
            SetWindowLongPtrW(window, GWLP_USERDATA, 0);
            return DefWindowProcW(window, message, wParam, lParam);
        }

        return DefWindowProcW(window, message, wParam, lParam);
    }

    bool IsExplorerProcess(DWORD processId)
    {
        HANDLE process = OpenProcess(
            PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
        if (!process)
        {
            return false;
        }

        wchar_t imagePath[MAX_PATH]{};
        DWORD imagePathChars = static_cast<DWORD>(std::size(imagePath));
        bool isExplorer = false;
        if (QueryFullProcessImageNameW(
                process, 0, imagePath, &imagePathChars))
        {
            isExplorer = _wcsicmp(PathFindFileNameW(imagePath),
                                  L"explorer.exe") == 0;
        }
        CloseHandle(process);
        return isExplorer;
    }

    unsigned __stdcall SettingsUiThreadProc(void *)
    {
        MSG message{};
        PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

        bool hotkeyRegistered = RegisterHotKey(
            nullptr,
            kAppearanceHotkeyId,
            MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT,
            VK_F12) != FALSE;
        if (!hotkeyRegistered)
        {
            Wh_Log(L"Appearance hotkey registration failed error=%lu",
                   GetLastError());
        }

        WNDCLASSEXW windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = AppearanceWindowProc;
        windowClass.hInstance = g_settingsUiModule;
        windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        windowClass.lpszClassName = kAppearanceWindowClassName;
        ATOM classAtom = RegisterClassExW(&windowClass);
        WNDCLASSEXW colorWheelClass = windowClass;
        colorWheelClass.lpfnWndProc = ColorPickerWindowProc;
        colorWheelClass.lpszClassName = kColorWheelWindowClassName;
        ATOM colorWheelClassAtom = RegisterClassExW(&colorWheelClass);
        if (!classAtom || !colorWheelClassAtom)
        {
            Wh_Log(L"Appearance window class registration failed error=%lu",
                   GetLastError());
            g_settingsUiHotkeyRegistered = false;
        }
        else
        {
            g_settingsUiHotkeyRegistered = hotkeyRegistered;
        }

        if (g_settingsUiReadyEvent)
        {
            SetEvent(g_settingsUiReadyEvent);
        }

        while (GetMessageW(&message, nullptr, 0, 0) > 0)
        {
            if (message.message == WM_HOTKEY &&
                message.wParam == kAppearanceHotkeyId)
            {
                HWND foreground = GetForegroundWindow();
                DWORD foregroundProcessId = 0;
                if (foreground &&
                    GetWindowThreadProcessId(
                        foreground, &foregroundProcessId) &&
                    IsExplorerProcess(foregroundProcessId))
                {
                    OpenAppearanceWindow();
                }
                continue;
            }

            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        if (g_settingsWindow && IsWindow(g_settingsWindow))
        {
            DestroyWindow(g_settingsWindow);
            g_settingsWindow = nullptr;
        }
        if (hotkeyRegistered)
        {
            UnregisterHotKey(nullptr, kAppearanceHotkeyId);
            g_settingsUiHotkeyRegistered = false;
        }
        if (classAtom)
        {
            UnregisterClassW(kAppearanceWindowClassName, g_settingsUiModule);
        }
        if (colorWheelClassAtom)
        {
            UnregisterClassW(
                kColorWheelWindowClassName, g_settingsUiModule);
        }
        return 0;
    }

    bool StartSettingsUi()
    {
        std::lock_guard<std::mutex> lock(g_settingsUiLifecycleMutex);
        if (g_settingsUiThreadHandle)
        {
            return true;
        }

        HMODULE module = nullptr;
        if (!GetModuleHandleExW(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                reinterpret_cast<PCWSTR>(AppearanceWindowProc),
                &module))
        {
            Wh_Log(L"Appearance UI could not identify its module error=%lu",
                   GetLastError());
            return false;
        }

        g_settingsUiModule = module;
        g_settingsUiReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!g_settingsUiReadyEvent)
        {
            Wh_Log(L"Appearance UI readiness event creation failed error=%lu",
                   GetLastError());
            return false;
        }

        unsigned threadId = 0;
        uintptr_t threadHandle = _beginthreadex(
            nullptr, 0, SettingsUiThreadProc, nullptr, 0, &threadId);
        if (!threadHandle)
        {
            Wh_Log(L"Appearance UI thread creation failed");
            CloseHandle(g_settingsUiReadyEvent);
            g_settingsUiReadyEvent = nullptr;
            return false;
        }

        g_settingsUiThreadHandle = reinterpret_cast<HANDLE>(threadHandle);
        g_settingsUiThreadId = threadId;
        DWORD waitResult = WaitForSingleObject(g_settingsUiReadyEvent, 5000);
        CloseHandle(g_settingsUiReadyEvent);
        g_settingsUiReadyEvent = nullptr;

        if (waitResult != WAIT_OBJECT_0 || !g_settingsUiHotkeyRegistered)
        {
            Wh_Log(L"Appearance UI unavailable; hotkey initialization failed");
            PostThreadMessageW(g_settingsUiThreadId, WM_QUIT, 0, 0);
            WaitForSingleObject(g_settingsUiThreadHandle, INFINITE);
            CloseHandle(g_settingsUiThreadHandle);
            g_settingsUiThreadHandle = nullptr;
            g_settingsUiThreadId = 0;
            return false;
        }

        Wh_Log(L"Appearance window ready: Ctrl+Alt+Shift+F12");
        return true;
    }

    void StopSettingsUi()
    {
        std::lock_guard<std::mutex> lock(g_settingsUiLifecycleMutex);
        if (!g_settingsUiThreadHandle)
        {
            return;
        }

        if (g_settingsUiThreadId)
        {
            PostThreadMessageW(g_settingsUiThreadId, WM_QUIT, 0, 0);
        }
        WaitForSingleObject(g_settingsUiThreadHandle, INFINITE);
        CloseHandle(g_settingsUiThreadHandle);
        g_settingsUiThreadHandle = nullptr;
        g_settingsUiThreadId = 0;
    }
}

BOOL Wh_ModInit()
{
    g_unloading.store(false, std::memory_order_release);
    LogWindowsBuild();
    Wh_Log(L"File Operation Styler " WH_MOD_VERSION
           L" initialization started");

    LoadSettings();

    if (!InitializeProgressCircleUi())
    {
        return FALSE;
    }

    SkinTargets targets{};
    if (!ResolveSkinTargets(&targets) || !InstallSkinHooks(targets))
    {
        ShutdownProgressCircleUi();
        ReleaseOwnedSkinTargetModules();
        return FALSE;
    }

    // The appearance window runs on its own message thread. Failure to claim
    // the optional hotkey does not affect the operation presentation hooks.
    StartSettingsUi();

    Wh_Log(L"File Operation Styler " WH_MOD_VERSION
           L" initialization complete");
    return TRUE;
}

void Wh_ModBeforeUninit()
{
    if (g_unloading.exchange(true, std::memory_order_acq_rel))
    {
        return;
    }

    StopSettingsUi();

    Wh_Log(L"File Operation Styler " WH_MOD_VERSION
           L" presentation teardown started");
    {
        std::unique_lock<std::mutex> lock(g_presentationActivationMutex);
        g_presentationActivationCondition.wait(
            lock, []
            { return g_presentationActivations == 0; });
    }
    DestroyAllProgressCircles();
    Wh_Log(L"File Operation Styler " WH_MOD_VERSION
           L" presentation teardown complete");
}

void Wh_ModUninit()
{
    StopSettingsUi();
    ShutdownProgressCircleUi();
    {
        std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
        g_transferSummaries.clear();
        ClearOperationDataBindingsLocked();
    }
    ClearSkinState();
    ReleaseOwnedSkinTargetModules();
    ShutdownSharedProgressBridge();
    ShutdownGlassBufferedPaintApi();
    ShutdownDwmApi();
    ShutdownShellMemoryApi();
    Wh_Log(L"File Operation Styler " WH_MOD_VERSION
           L" uninitialization complete");
}

BOOL Wh_ModSettingsChanged(BOOL *bReload)
{
    if (bReload)
    {
        *bReload = TRUE;
    }
    return TRUE;
}
