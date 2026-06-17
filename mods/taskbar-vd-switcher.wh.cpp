// ==WindhawkMod==
// @id              taskbar-vd-switcher
// @name            Taskbar Virtual Desktop Switcher
// @description     Injects clickable buttons into the taskbar — one per virtual desktop — with configurable grid arrangement for direct switching.
// @version         1.5
// @author          sb4ssman
// @github          https://github.com/sb4ssman
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Virtual Desktop Switcher

Adds numbered buttons to the taskbar — one per virtual desktop. Click to switch directly.

![Default tray placement — three numbered buttons, first active](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/virtual-desktop-switcher/assets/simple3.png)

Buttons can be arranged into one or more rows with configurable fill order.

![Four desktops with master button](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/virtual-desktop-switcher/assets/simple4wmaster.png)

Works alongside other mods.

![Complex setup with other mods active](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/virtual-desktop-switcher/assets/vds-screenshot3.png)

The compact grid adapts to how many desktops you have.

![Taller taskbar with right-side grid and lower master button](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/virtual-desktop-switcher/assets/gridonrightwlowermaster.png)

The switcher can also sit near the Start button.

![Left of Start button](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/virtual-desktop-switcher/assets/left-of-start.png)

![Over Start, nudged above](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/virtual-desktop-switcher/assets/over-above-start.png)

![Right of Start with Start hidden](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/virtual-desktop-switcher/assets/right-of-start-hidden-start.png)

## Settings
- **Position** — system tray positions plus experimental Start-adjacent and Start-overlay positions
- **Start positions** — place buttons left of Start, over Start, or in a reserved space to the right of Start
- **Size** — button width × height in pixels; spacing between buttons
- **Grid mode** — smart automatic layout, single row/column, fixed rows, fixed columns, or fixed grid
- **Smart layout** — balanced, vertical pack, or horizontal pack behavior
- **Fill order** — column-first or row-first button order
- **Short group alignment** — when the last row/column is shorter, align it to start, center, or end
- **Active color** — hex color for the current-desktop button (e.g. `#4488FF`)
- **Inactive color** — hex color for other buttons (empty = system default)
- **Opacity** — 0–100; lower values let the taskbar show through
- **Shine effect** — subtle gradient highlight (applies when a color is set)
- **Label format** — numbers, roman numerals, dots, or custom comma-separated labels
- **Padding** — left and right margin around the button grid (px)
- **Text color** — foreground color for active and inactive buttons
- **Font size** — button label size in pt
- **Corner radius** — rounded corners (px)
- **Active bold** — bold the current desktop's label
- **Border** — border color and thickness
- **Hide when single** — hide the bar when only one desktop exists
- **Tooltips** — hover a button to see the desktop name
- **Master button** — optional extra button that opens Task View (Win+Tab) to preview, create, or close desktops

## Known limitations
- Multi-monitor: only the primary taskbar gets buttons.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position: "afterClock"
  $name: Position
  $description: Where in the system tray to inject the VD buttons
  $options:
  - "afterClock": "After clock (before Show Desktop)"
  - "beforeClock": "Before clock (after OmniButton)"
  - "beforeOmni": "Before OmniButton (wifi/vol/bat)"
  - "beforeIcons": "Before notification icons"
  - "afterShowDesktop": "After Show Desktop strip"
  - "nextToStart": "Left of Start button (experimental)"
  - "overStart": "Over Start button (experimental)"
  - "rightOfStart": "Right of Start button (experimental)"

- buttonWidth: 20
  $name: Button width (px)

- buttonHeight: 22
  $name: Button height (px)

- buttonSpacing: 2
  $name: Button spacing (px)
  $description: Gap between buttons in the grid

- gridMode: autoSmart
  $name: Grid mode
  $description: >-
    Choose how the button grid shape is selected. Auto smart picks a compact
    balanced layout that fits the available taskbar height. Fixed modes use
    the Rows and/or Columns settings below.
  $options:
  - autoSmart: Smart automatic
  - singleRow: Single row
  - singleColumn: Single column
  - fixedRows: Fixed rows
  - fixedColumns: Fixed columns
  - fixedGrid: Fixed rows and columns

- smartLayout: balanced
  $name: Smart layout
  $description: >-
    Used when Grid mode is Smart automatic. Balanced avoids awkward 3+1 layouts
    when a cleaner 2x2 is possible. Vertical pack uses available height.
    Horizontal pack prefers fewer rows.
  $options:
  - balanced: Balanced
  - packVertical: Pack vertical
  - packHorizontal: Pack horizontal

- fillOrder: rowFirst
  $name: Fill order
  $description: Whether desktop buttons fill across rows first or down columns first.
  $options:
  - rowFirst: Row-first (left to right, then down)
  - columnFirst: Column-first (top to bottom, then right)

- buttonRows: 0
  $name: Rows (0 = auto)
  $description: >-
    In Fixed rows and Fixed grid modes: sets the exact row count. In Smart
    automatic mode: acts as a maximum cap (0 = uncapped). Ignored in Single
    row and Single column modes.

- buttonColumns: 0
  $name: Columns (0 = auto)
  $description: >-
    In Fixed columns and Fixed grid modes: sets the exact column count. In Smart
    automatic mode: filters out layouts that would exceed this many columns (0 =
    no limit). Ignored in Single row and Single column modes. In row-first fill,
    3 columns with 4 desktops gives a 3+1 layout.

- activeColor: "#4488FF"
  $name: Active desktop color (hex, empty = system default)

- inactiveColor: ""
  $name: Inactive button color (hex, empty = system default)

- buttonOpacity: 100
  $name: Button opacity (0–100)
  $description: 100 = fully opaque; lower values let the taskbar show through

- shineEffect: false
  $name: Shine effect
  $description: Adds a subtle gradient highlight. Applies when a custom color is set.

- labelFormat: "number"
  $name: Label format
  $options:
  - "number": "Numbers  1  2  3"
  - "roman": "Roman numerals  I  II  III"
  - "dot": "Dots  ●  ○  ○"
  - "custom": "Custom labels"

- customLabels: ""
  $name: Custom labels (comma-separated, e.g. "H,W,M")
  $description: Used when label format is Custom. Falls back to numbers if labels run out.

- activeTextColor: ""
  $name: Active desktop text color (hex, empty = system default)

- inactiveTextColor: ""
  $name: Inactive button text color (hex, empty = system default)

- fontSize: 10
  $name: Font size (pt)

- cornerRadius: 4
  $name: Corner radius (px)
  $description: Rounded corners on buttons (0 = square, 4 = Windows default)

- activeBold: false
  $name: Bold active desktop label

- borderThickness: 0
  $name: Button border thickness (px)

- borderColor: ""
  $name: Button border color (hex, empty = system default)

- hideWhenSingle: false
  $name: Hide when only one desktop
  $description: Don't show the button bar when there is only one virtual desktop

- paddingLeft: 0
  $name: Padding left (px)
  $description: Extra space to the left of the button grid

- paddingRight: 2
  $name: Padding right (px)
  $description: Extra space to the right of the button grid

- gridVerticalOffset: 0
  $name: Vertical offset (px)
  $description: >-
    Nudge the entire button grid up (negative) or down (positive) from its
    centered position. 0 = auto-centered. Applies after automatic centering,
    so it works in combination with all grid and sliver settings.

- shortGroupAlign: "center"
  $name: Short column/row alignment
  $description: >-
    When the last column (column-first) or last row (row-first) has fewer
    buttons than the others, where to place those buttons within the available space.
  $options:
  - "start": "Start (top for columns, left for rows)"
  - "center": "Center"
  - "end": "End (bottom for columns, right for rows)"

- showMasterButton: false
  $name: Show master button
  $description: >-
    Adds a button that opens Task View (Win+Tab), where you can preview all
    desktops and create or close them.

- masterButtonLabel: "⊞"
  $name: Master button label
  $description: Text shown on the master button.

- masterButtonPosition: "after"
  $name: Master button position
  $options:
  - "before": "Column before desktop buttons"
  - "after": "Column after desktop buttons"
  - "bottom": "Sliver below desktop buttons"
  - "top": "Sliver above desktop buttons"

- masterButtonHeight: 6
  $name: Sliver height (px)
  $description: >-
    Row height of the master button when placed above or below the desktop buttons
    (Top or Bottom positions). Larger values cause the sliver to peek further past
    the taskbar edge. Not used in Before or After (column) positions.

- masterButtonWidth: 14
  $name: Master column width (px)
  $description: >-
    Column width of the master button when placed before or after the desktop
    buttons (Before or After positions). Not used in Top or Bottom (sliver) positions.

- masterButtonSpacing: 0
  $name: Sliver gap offset (px)
  $description: >-
    Only applies to Top or Bottom (sliver) positions. Extra space added between the
    sliver button and the desktop buttons, beyond the normal button spacing. 0 =
    no extra gap. Positive = sliver button retreats from desktop buttons (larger
    gap, smaller sliver). Negative = sliver button advances toward or overlaps the
    desktop buttons. Does not affect desktop button centering. To control how far
    the sliver peeks past the taskbar edge, adjust Sliver height.
*/
// ==/WindhawkModSettings==

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <atomic>
#include <list>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include <functional>
#include <algorithm>
#include <climits>
#include <cmath>

#include <windhawk_utils.h>
#include <combaseapi.h>
#include <winver.h>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Automation;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;

// ============================================================
// Settings
// ============================================================

struct ModSettings {
    std::wstring position      = L"afterClock";
    int buttonWidth            = 20;
    int buttonHeight           = 22;
    int buttonSpacing          = 2;
    int buttonRows             = 0;
    int buttonColumns          = 0;
    std::wstring activeColor   = L"#4488FF";
    std::wstring inactiveColor = L"";
    int buttonOpacity          = 70;
    bool shineEffect           = false;
    std::wstring labelFormat      = L"number";
    std::wstring customLabels     = L"";
    std::wstring activeTextColor  = L"";
    std::wstring inactiveTextColor= L"";
    int fontSize                  = 10;
    int cornerRadius              = 4;
    bool activeBold               = false;
    int borderThickness           = 0;
    std::wstring borderColor      = L"";
    bool hideWhenSingle           = false;
    int paddingLeft               = 0;
    int paddingRight              = 2;
    std::wstring gridMode            = L"autoSmart";
    std::wstring smartLayout         = L"balanced";
    std::wstring fillOrder           = L"rowFirst";
    std::wstring shortGroupAlign      = L"center";
    bool         showMasterButton     = false;
    std::wstring masterButtonLabel    = L"⊞"; // ⊞
    std::wstring masterButtonPosition = L"after";
    int          masterButtonHeight   = 6;
    int          masterButtonWidth    = 14;
    int          masterButtonSpacing  = 0;
    int          gridVerticalOffset   = 0;
};
ModSettings g_settings;

static void LoadSettings() {
    auto Str = [](const wchar_t* k, const wchar_t* d) {
        PCWSTR p = Wh_GetStringSetting(k);
        std::wstring r = p ? p : d;
        Wh_FreeStringSetting(p);
        return r;
    };
    g_settings.position       = Str(L"position",      L"afterClock");
    g_settings.buttonWidth    = Wh_GetIntSetting(L"buttonWidth",   20);
    g_settings.buttonHeight   = Wh_GetIntSetting(L"buttonHeight",  22);
    g_settings.buttonSpacing  = Wh_GetIntSetting(L"buttonSpacing", 2);
    g_settings.buttonRows     = std::max(Wh_GetIntSetting(L"buttonRows",    0), 0);
    g_settings.buttonColumns  = std::max(Wh_GetIntSetting(L"buttonColumns", 0), 0);
    g_settings.activeColor    = Str(L"activeColor",   L"#4488FF");
    g_settings.inactiveColor  = Str(L"inactiveColor", L"");
    g_settings.buttonOpacity  = Wh_GetIntSetting(L"buttonOpacity", 100);
    g_settings.shineEffect    = Wh_GetIntSetting(L"shineEffect",   0) != 0;
    g_settings.labelFormat       = Str(L"labelFormat",       L"number");
    g_settings.customLabels      = Str(L"customLabels",      L"");
    g_settings.activeTextColor   = Str(L"activeTextColor",   L"");
    g_settings.inactiveTextColor = Str(L"inactiveTextColor", L"");
    g_settings.fontSize          = Wh_GetIntSetting(L"fontSize",         10);
    g_settings.cornerRadius      = Wh_GetIntSetting(L"cornerRadius",      4);
    g_settings.activeBold        = Wh_GetIntSetting(L"activeBold",        0) != 0;
    g_settings.borderThickness   = Wh_GetIntSetting(L"borderThickness",   0);
    g_settings.borderColor       = Str(L"borderColor",       L"");
    g_settings.hideWhenSingle    = Wh_GetIntSetting(L"hideWhenSingle",    0) != 0;
    g_settings.paddingLeft       = Wh_GetIntSetting(L"paddingLeft",       0);
    g_settings.paddingRight      = Wh_GetIntSetting(L"paddingRight",      2);
    g_settings.gridMode             = Str(L"gridMode",        L"autoSmart");
    g_settings.smartLayout          = Str(L"smartLayout",     L"balanced");
    g_settings.fillOrder            = Str(L"fillOrder",       L"rowFirst");
    g_settings.shortGroupAlign      = Str(L"shortGroupAlign",      L"center");
    g_settings.showMasterButton     = Wh_GetIntSetting(L"showMasterButton",  0) != 0;
    g_settings.masterButtonLabel    = Str(L"masterButtonLabel",    L"⊞");
    g_settings.masterButtonPosition = Str(L"masterButtonPosition", L"after");
    g_settings.masterButtonHeight   = std::max(1, Wh_GetIntSetting(L"masterButtonHeight", 6));
    g_settings.masterButtonWidth    = std::max(1, Wh_GetIntSetting(L"masterButtonWidth",  14));
    g_settings.masterButtonSpacing  = Wh_GetIntSetting(L"masterButtonSpacing", 0);
    g_settings.gridVerticalOffset   = Wh_GetIntSetting(L"gridVerticalOffset",  0);
}

// ============================================================
// Globals
// ============================================================

static std::atomic<bool> g_unloading{false};
static HWND              g_taskbarWnd      = nullptr;
static Grid              g_buttonGrid      = nullptr;
static FrameworkElement  g_injectionParent = nullptr;
static int               g_injectedColumn  = -1;
static bool              g_startOverlayMode = false;
static FrameworkElement  g_startOverlayRoot = nullptr;
static FrameworkElement  g_startOverlayStart = nullptr;
static winrt::event_token g_startOverlayLayoutToken{};
static FrameworkElement  g_taskItemsPanel = nullptr;
static Thickness         g_taskItemsPanelOriginalMargin{};
static double            g_startButtonOriginalX = -1.0;
static std::atomic<int>  g_currentDesktop{0};
static std::atomic<int>  g_desktopCount{1};

static HANDLE g_notificationThread    = nullptr;
static HANDLE g_notificationStopEvent = nullptr;
static DWORD  g_notificationCookie    = 0;

static HANDLE g_retryThread    = nullptr;
static HANDLE g_retryStopEvent = nullptr;

static std::atomic<bool> g_systemTrayModuleHooked{false};
static std::atomic<int>  g_activeSwitchThreads{0};
static std::list<FrameworkElement::Loaded_revoker> g_autoRevokerList;

// Forward declarations
static void ApplyAllSettings();
static void ApplyAllSettingsOnWindowThread();
static void RebuildOrUpdate(bool fullRebuild);
static void RemoveButtonGrid();
static void StopNotificationThread();
static void StopRetryThread();
static void HandleLoadedModuleIfSystemTray(HMODULE hModule, LPCWSTR lpLibFileName);

// ============================================================
// Explorer / twinui build detection
// ============================================================

static WORD g_explorerBuild    = 0;
static WORD g_explorerRevision = 0;
static WORD g_twinuiBuild      = 0;

static void DetectExplorerBuild() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    DWORD dummy;
    DWORD sz = GetFileVersionInfoSizeW(path, &dummy);
    if (!sz) return;
    std::vector<BYTE> buf(sz);
    if (!GetFileVersionInfoW(path, 0, sz, buf.data())) return;
    VS_FIXEDFILEINFO* fi = nullptr; UINT fs = 0;
    if (!VerQueryValueW(buf.data(), L"\\", (void**)&fi, &fs)) return;
    g_explorerBuild    = HIWORD(fi->dwFileVersionLS);
    g_explorerRevision = LOWORD(fi->dwFileVersionLS);
    Wh_Log(L"[Init] Explorer build %u rev %u", g_explorerBuild, g_explorerRevision);
}

static VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;
    HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) || uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }
    if (puPtrLen) *puPtrLen = uPtrLen;
    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

static bool LoadTwinuiBuild() {
    if (g_twinuiBuild) return true;
    HMODULE h = GetModuleHandleW(L"twinui.pcshell.dll");
    if (!h) return false;
    VS_FIXEDFILEINFO* fi = GetModuleVersionInfo(h, nullptr);
    if (!fi) return false;
    g_twinuiBuild = HIWORD(fi->dwFileVersionLS);
    Wh_Log(L"[VD] twinui.pcshell.dll build %u", g_twinuiBuild);
    return true;
}

// Order matters: SystemTray.dll is the new home (Win11 Insider 26200+);
// older builds have the symbols in Taskbar.View.dll.
static HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandleW(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandleW(L"Taskbar.View.dll");
        if (module) {
            // Starting with Taskbar.View.dll 2604.x, the SystemTray types moved
            // out into SystemTray.dll — don't hook this version.
            VS_FIXEDFILEINFO* fi = GetModuleVersionInfo(module, nullptr);
            WORD moduleMajor = fi ? HIWORD(fi->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
                Wh_Log(L"[Hooks] Skipping Taskbar.View.dll version %d", moduleMajor);
                module = nullptr;
            }
        }
    }
    if (!module)
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    return module;
}

// ============================================================
// GetTaskbarXamlRoot boilerplate (from vertical-omnibutton)
// ============================================================

using RunFromWindowThreadProc_t = void (*)(void*);

static bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
    static const UINT kMsg = RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Param { RunFromWindowThreadProc_t proc; void* procParam; };
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!dwThreadId) return false;
    if (dwThreadId == GetCurrentThreadId()) { proc(procParam); return true; }
    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
        if (nCode == HC_ACTION) {
            const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
            if (cwp->message == RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID)) {
                auto* p = (Param*)cwp->lParam;
                p->proc(p->procParam);
            }
        }
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }, nullptr, dwThreadId);
    if (!hook) return false;
    Param param{ proc, procParam };
    SendMessage(hWnd, kMsg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return true;
}

static HWND FindCurrentProcessTaskbarWnd() {
    HWND result = nullptr;
    EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
        DWORD pid; WCHAR cls[32];
        if (GetWindowThreadProcessId(hWnd, &pid) && pid == GetCurrentProcessId() &&
            GetClassName(hWnd, cls, ARRAYSIZE(cls)) && _wcsicmp(cls, L"Shell_TrayWnd") == 0) {
            *reinterpret_cast<HWND*>(lParam) = hWnd; return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&result));
    return result;
}

using CTaskBand_GetTaskbarHost_t = void* (WINAPI*)(void* pThis, void* taskbarHostSharedPtr);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

using TaskbarHost_FrameHeight_t = int (WINAPI*)(void* pThis);
TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void (WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

static void* CTaskBand_ITaskListWndSite_vftable = nullptr;

static XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    // Guard: symbols must be resolved before any dereference.
    if (!CTaskBand_GetTaskbarHost_Original || !TaskbarHost_FrameHeight_Original ||
        !std__Ref_count_base__Decref_Original)
        return nullptr;

    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) return nullptr;
    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    // Guard: taskBand is null during early startup before the taskband stores its this-pointer.
    if (!taskBand) return nullptr;
    void* taskBandForSite = taskBand;
    for (int i = 0; *(void**)taskBandForSite != CTaskBand_ITaskListWndSite_vftable; i++) {
        if (i == 20) return nullptr;
        taskBandForSite = (void**)taskBandForSite + 1;
    }
    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForSite, taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) return nullptr;
    size_t offset = 0x10;
#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0]==0x48 && b[1]==0x83 && b[2]==0xEC && b[4]==0x48 &&
            b[5]==0x83 && b[6]==0xC1 && b[7]<=0x7F)
            offset = b[7];
        else
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00)
            offset = (p[3] >> 12) & 0xFF;
        else
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
    }
#else
#error "Unsupported architecture"
#endif
    auto* iunk = *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] + offset);
    // Guard: iunk is null during early startup before the TaskbarElement is set at offset.
    if (!iunk) {
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }
    FrameworkElement taskbarElem = nullptr;
    iunk->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(taskbarElem));
    auto result = taskbarElem ? taskbarElem.XamlRoot() : nullptr;
    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
    return result;
}

// ============================================================
// XAML helpers
// ============================================================

static FrameworkElement FindChildRecursive(FrameworkElement const& element,
    std::function<bool(FrameworkElement)> const& cb, int maxDepth = 20)
{
    int n = VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < n && maxDepth > 0; i++) {
        auto child = VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (cb(child)) return child;
        auto found = FindChildRecursive(child, cb, maxDepth - 1);
        if (found) return found;
    }
    return nullptr;
}

// ============================================================
// VD COM notification infrastructure
// ============================================================

const CLSID CLSID_ImmersiveShell = {
    0xc2f03a33,0x21f5,0x47fa,{0xb4,0xbb,0x15,0x63,0x62,0xa2,0xf2,0x39}
};
const GUID SID_VirtualDesktopNotificationService = {
    0xa501fdec,0x4a09,0x464c,{0xae,0x4e,0x1b,0x9c,0x21,0xb8,0x49,0x18}
};
const GUID IID_IVirtualDesktopNotificationService_G = {
    0x0cd45e71,0xd927,0x4f15,{0x8b,0x0a,0x8f,0xef,0x52,0x53,0x37,0xbf}
};

MIDL_INTERFACE("0CD45E71-D927-4F15-8B0A-8FEF525337BF")
IVirtualDesktopNotificationService_I : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Register(IUnknown*, DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Unregister(DWORD) = 0;
};

struct NotifConfig {
    int64_t iidPart1 = 0, iidPart2 = 0;
    int methodCount = 0, createdIdx = -1, destroyedIdx = -1, currentChangedIdx = -1;
    bool hasMonitors = false;
};

struct NotifObject {
    void** vtable  = nullptr;
    LONG refCount  = 1;
};

static NotifConfig GetNotifConfig() {
    if (g_explorerBuild < 22000) return {};
    if (g_explorerBuild < 22483 || (g_explorerBuild == 22621 && g_explorerRevision < 2215))
        return { 5481970284372180562ll, -1679294552252794956ll, 13, 7, 9, 11, true };
    if (g_explorerBuild < 22631 || (g_explorerBuild == 22631 && g_explorerRevision < 3085))
        return { 5123538856297626140ll,  8491238173783613346ll, 14, 6, 8, 10, false };
    return     { 5308375338100058445ll, -2401892766147978065ll, 14, 6, 8, 10, false };
}

static bool IsOurNotifIface(REFIID riid) {
    auto cfg = GetNotifConfig();
    if (!cfg.methodCount) return false;
    auto p = reinterpret_cast<const int64_t*>(&riid);
    return p[0] == cfg.iidPart1 && p[1] == cfg.iidPart2;
}

static HRESULT STDMETHODCALLTYPE Notif_QI(NotifObject* p, REFIID riid, void** ppv) {
    if (!ppv) return E_POINTER; *ppv = nullptr;
    static const GUID IID_IUnknown_ = {0,0,0,{0xc0,0,0,0,0,0,0,0x46}};
    if (InlineIsEqualGUID(riid, IID_IUnknown_) || IsOurNotifIface(riid)) {
        *ppv = p; InterlockedIncrement(&p->refCount); return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG STDMETHODCALLTYPE Notif_AddRef(NotifObject* p) {
    return (ULONG)InterlockedIncrement(&p->refCount);
}
static ULONG STDMETHODCALLTYPE Notif_Release(NotifObject* p) {
    LONG r = InterlockedDecrement(&p->refCount);
    if (r == 0) { delete[] p->vtable; delete p; }
    return (ULONG)std::max(r, 0L);
}
static HRESULT STDMETHODCALLTYPE Notif_HandleUpdate(bool fullRebuild) {
    if (g_unloading || !g_taskbarWnd) return S_OK;
    RunFromWindowThread(g_taskbarWnd, [](void* p) {
        if (!g_unloading) RebuildOrUpdate((bool)(intptr_t)p);
    }, (void*)fullRebuild);
    return S_OK;
}
static HRESULT STDMETHODCALLTYPE Notif_NoOp() { return S_OK; }
static HRESULT STDMETHODCALLTYPE Notif_CountChanged(NotifObject*) { return Notif_HandleUpdate(true); }
static HRESULT STDMETHODCALLTYPE Notif_CurrentChanged(NotifObject*) { return Notif_HandleUpdate(false); }
static HRESULT STDMETHODCALLTYPE Notif_CurrentChangedWithMonitors(NotifObject*, void*, void*, void*) {
    return Notif_HandleUpdate(false);
}

static NotifObject* CreateNotifObject() {
    auto cfg = GetNotifConfig();
    if (cfg.methodCount == 0 || cfg.currentChangedIdx < 0) return nullptr;
    auto* obj = new (std::nothrow) NotifObject();
    if (!obj) return nullptr;
    obj->vtable = new (std::nothrow) void*[cfg.methodCount];
    if (!obj->vtable) { delete obj; return nullptr; }
    for (int i = 0; i < cfg.methodCount; i++) obj->vtable[i] = (void*)&Notif_NoOp;
    obj->vtable[0] = (void*)&Notif_QI;
    obj->vtable[1] = (void*)&Notif_AddRef;
    obj->vtable[2] = (void*)&Notif_Release;
    if (cfg.createdIdx >= 0)   obj->vtable[cfg.createdIdx]   = (void*)&Notif_CountChanged;
    if (cfg.destroyedIdx >= 0) obj->vtable[cfg.destroyedIdx] = (void*)&Notif_CountChanged;
    obj->vtable[cfg.currentChangedIdx] = cfg.hasMonitors
        ? (void*)&Notif_CurrentChangedWithMonitors
        : (void*)&Notif_CurrentChanged;
    return obj;
}

static NotifObject* g_notifObject = nullptr;

static DWORD WINAPI NotificationThreadProc(void*) {
    auto cfg = GetNotifConfig();
    if (cfg.methodCount == 0) {
        Wh_Log(L"[Notif] Unsupported build (explorer %u)", g_explorerBuild);
        return 0;
    }
    if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))) return 0;

    IServiceProvider* svc = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER,
                                IID_IServiceProvider, (void**)&svc)) || !svc) {
        CoUninitialize(); return 0;
    }
    IVirtualDesktopNotificationService_I* notifSvc = nullptr;
    svc->QueryService(SID_VirtualDesktopNotificationService,
                      IID_IVirtualDesktopNotificationService_G, (void**)&notifSvc);
    svc->Release();
    if (!notifSvc) { CoUninitialize(); return 0; }

    g_notifObject = CreateNotifObject();
    if (!g_notifObject) { notifSvc->Release(); CoUninitialize(); return 0; }

    HRESULT hr = notifSvc->Register(reinterpret_cast<IUnknown*>(g_notifObject), &g_notificationCookie);
    if (FAILED(hr)) {
        Wh_Log(L"[Notif] Register failed: 0x%08X", hr);
        Notif_Release(g_notifObject); g_notifObject = nullptr;
        notifSvc->Release(); CoUninitialize(); return 0;
    }
    Wh_Log(L"[Notif] Registered, cookie=%lu", g_notificationCookie);

    MSG msg;
    while (!g_unloading) {
        DWORD w = MsgWaitForMultipleObjects(1, &g_notificationStopEvent, FALSE, INFINITE, QS_ALLINPUT);
        if (w == WAIT_OBJECT_0) break;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    }

    if (g_notificationCookie) { notifSvc->Unregister(g_notificationCookie); g_notificationCookie = 0; }
    if (g_notifObject)        { Notif_Release(g_notifObject); g_notifObject = nullptr; }
    notifSvc->Release();
    CoUninitialize();
    return 0;
}

static void StartNotificationThread() {
    if (g_notificationThread) return;
    g_notificationStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_notificationThread    = CreateThread(nullptr, 0, NotificationThreadProc, nullptr, 0, nullptr);
    if (!g_notificationThread) {
        CloseHandle(g_notificationStopEvent); g_notificationStopEvent = nullptr;
    }
}

static void StopNotificationThread() {
    if (g_notificationStopEvent) SetEvent(g_notificationStopEvent);
    if (g_notificationThread) {
        // Pump sent messages while waiting so that if Wh_ModUninit is called from
        // the UI thread and the notification thread is mid-SendMessage, the sent
        // message can be delivered and the notification thread can then exit.
        // PeekMessage(PM_NOREMOVE) processes incoming sent messages without
        // consuming posted messages from the queue.
        DWORD result;
        do {
            result = MsgWaitForMultipleObjects(1, &g_notificationThread, FALSE, INFINITE, QS_SENDMESSAGE);
            if (result == WAIT_OBJECT_0 + 1) {
                MSG msg;
                PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);
            }
        } while (result == WAIT_OBJECT_0 + 1);
        CloseHandle(g_notificationThread); g_notificationThread = nullptr;
    }
    if (g_notificationStopEvent) {
        CloseHandle(g_notificationStopEvent); g_notificationStopEvent = nullptr;
    }
}

static void StopRetryThread() {
    if (g_retryStopEvent) SetEvent(g_retryStopEvent);
    if (g_retryThread) {
        WaitForSingleObject(g_retryThread, 12000);
        CloseHandle(g_retryThread); g_retryThread = nullptr;
    }
    if (g_retryStopEvent) {
        CloseHandle(g_retryStopEvent); g_retryStopEvent = nullptr;
    }
}

// ============================================================
// Desktop state — registry
// ============================================================

static std::vector<BYTE> ReadRegBinary(const wchar_t* path, const wchar_t* name) {
    DWORD type = 0, size = 0;
    if (RegGetValueW(HKEY_CURRENT_USER, path, name, RRF_RT_REG_BINARY, &type, nullptr, &size) != ERROR_SUCCESS || !size)
        return {};
    std::vector<BYTE> buf(size);
    if (RegGetValueW(HKEY_CURRENT_USER, path, name, RRF_RT_REG_BINARY, &type, buf.data(), &size) != ERROR_SUCCESS)
        return {};
    buf.resize(size);
    return buf;
}

static int ReadDesktopCount() {
    DWORD sessionId = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);
    wchar_t sessionPath[256];
    swprintf_s(sessionPath, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\%lu\\VirtualDesktops", sessionId);
    for (auto* path : { (const wchar_t*)sessionPath, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops" }) {
        auto buf = ReadRegBinary(path, L"VirtualDesktopIDs");
        if (buf.size() >= 16) return (int)(buf.size() / 16);
    }
    return 1;
}

static int ReadCurrentDesktop() {
    DWORD sessionId = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);
    wchar_t sessionPath[256];
    swprintf_s(sessionPath, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\%lu\\VirtualDesktops", sessionId);

    std::vector<BYTE> ids;
    for (auto* path : { (const wchar_t*)sessionPath, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops" }) {
        ids = ReadRegBinary(path, L"VirtualDesktopIDs");
        if (ids.size() >= 16) break;
    }
    if (ids.empty()) return 0;

    GUID currentGuid{};
    bool gotCurrent = false;
    for (auto* path : { (const wchar_t*)sessionPath, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops" }) {
        auto buf = ReadRegBinary(path, L"CurrentVirtualDesktop");
        if (buf.size() >= 16) { memcpy(&currentGuid, buf.data(), 16); gotCurrent = true; break; }
        // Try REG_SZ form
        wchar_t strBuf[64]; DWORD sz = sizeof(strBuf), type;
        if (RegGetValueW(HKEY_CURRENT_USER, path, L"CurrentVirtualDesktop",
                         RRF_RT_REG_SZ, &type, strBuf, &sz) == ERROR_SUCCESS &&
            SUCCEEDED(CLSIDFromString(strBuf, &currentGuid))) { gotCurrent = true; break; }
    }
    if (!gotCurrent) return 0;

    int count = (int)(ids.size() / 16);
    for (int i = 0; i < count; i++) {
        GUID g; memcpy(&g, ids.data() + i * 16, 16);
        if (memcmp(&g, &currentGuid, 16) == 0) return i;
    }
    return 0;
}

// Read Windows display names for all desktops (registry Desktops\{GUID}\Name).
// Falls back to "Desktop N" when a desktop has no custom name.
static std::vector<std::wstring> ReadDesktopNames(int count) {
    DWORD sessionId = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);
    wchar_t sessionPath[256];
    swprintf_s(sessionPath, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\%lu\\VirtualDesktops", sessionId);

    std::vector<BYTE> ids;
    for (auto* path : { (const wchar_t*)sessionPath, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops" }) {
        ids = ReadRegBinary(path, L"VirtualDesktopIDs");
        if (ids.size() >= 16) break;
    }

    std::vector<std::wstring> names(count);
    for (int i = 0; i < count; i++) {
        names[i] = L"Desktop " + std::to_wstring(i + 1);
        if ((int)ids.size() >= (i + 1) * 16) {
            GUID g; memcpy(&g, ids.data() + i * 16, 16);
            wchar_t guidStr[64];
            StringFromGUID2(g, guidStr, ARRAYSIZE(guidStr));
            wchar_t regPath[300];
            swprintf_s(regPath, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops\\Desktops\\%ls", guidStr);
            wchar_t name[256]; DWORD sz = sizeof(name);
            if (RegGetValueW(HKEY_CURRENT_USER, regPath, L"Name", RRF_RT_REG_SZ, nullptr, name, &sz) == ERROR_SUCCESS && name[0])
                names[i] = name;
        }
    }
    return names;
}

// ============================================================
// Virtual desktop switching
// ============================================================

struct IVirtualDesktopManagerInternal_S : IUnknown {};
struct IVirtualDesktop_S : IUnknown {};

MIDL_INTERFACE("92CA9DCD-5622-4bba-A805-5E9F541BD8C9")
IObjectArray_Local : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetCount(UINT* pcObjects) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAt(UINT i, REFIID riid, void** ppv) = 0;
};

const CLSID CLSID_VirtualDesktopManagerInternal = {
    0xC5E0CDCA,0x7B6E,0x41B2,{0x9F,0xC4,0xD9,0x39,0x75,0xCC,0x46,0x7B}
};

void SwitchToDesktop(int targetIndex) {
    if (!LoadTwinuiBuild()) { Wh_Log(L"[VD] twinui.pcshell.dll not loaded"); return; }

    IID IID_VDMI, IID_VD;
    bool usesHMonitor;
    if      (g_twinuiBuild >= 26100) {
        IID_VDMI = {0x53F5CA0B,0x158F,0x4124,{0x90,0x0C,0x05,0x71,0x58,0x06,0x0B,0x27}};
        IID_VD   = {0x3F07F4BE,0xB107,0x441A,{0xAF,0x0F,0x39,0xD8,0x25,0x29,0x07,0x2C}};
        usesHMonitor = false;
    } else if (g_twinuiBuild >= 22621) {
        IID_VDMI = {0xA3175F2D,0x239C,0x4BD2,{0x8A,0xA0,0xEE,0xBA,0x8B,0x0B,0x13,0x8E}};
        IID_VD   = {0x3F07F4BE,0xB107,0x441A,{0xAF,0x0F,0x39,0xD8,0x25,0x29,0x07,0x2C}};
        usesHMonitor = false;
    } else if (g_twinuiBuild >= 22000) {
        IID_VDMI = {0xB2F925B9,0x5A0F,0x4D2E,{0x9F,0x4D,0x2B,0x15,0x07,0x59,0x3C,0x10}};
        IID_VD   = {0x536D3495,0xB208,0x4CC9,{0xAE,0x26,0xDE,0x81,0x11,0x27,0x5B,0xF8}};
        usesHMonitor = true;
    } else if (g_twinuiBuild >= 20348) {
        IID_VDMI = {0x094AFE11,0x44F2,0x4BA0,{0x97,0x6F,0x29,0xA9,0x7E,0x26,0x3E,0xE0}};
        IID_VD   = {0x62FDF88B,0x11CA,0x4AFB,{0x8B,0xD8,0x22,0x96,0xDF,0xAE,0x49,0xE2}};
        usesHMonitor = true;
    } else {
        IID_VDMI = {0xF31574D6,0xB682,0x4CDC,{0xBD,0x56,0x18,0x27,0x86,0x0A,0xBE,0xC6}};
        IID_VD   = {0xFF72FFDD,0xBE7E,0x43FC,{0x9C,0x03,0xAD,0x81,0x68,0x1E,0x88,0xE4}};
        usesHMonitor = false;
    }

    IServiceProvider* svc = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER,
                                IID_IServiceProvider, (void**)&svc)) || !svc)
        { Wh_Log(L"[VD] CoCreateInstance failed"); return; }

    IVirtualDesktopManagerInternal_S* mgr = nullptr;
    svc->QueryService(CLSID_VirtualDesktopManagerInternal, IID_VDMI, (void**)&mgr);
    svc->Release();
    if (!mgr) { Wh_Log(L"[VD] QueryService VDMI failed"); return; }

    IObjectArray_Local* arr = nullptr;
    if (usesHMonitor) {
        typedef HRESULT(STDMETHODCALLTYPE* FnM)(IVirtualDesktopManagerInternal_S*, HMONITOR, IObjectArray_Local**);
        ((FnM)(*(void***)mgr)[7])(mgr, nullptr, &arr);
    } else {
        typedef HRESULT(STDMETHODCALLTYPE* Fn)(IVirtualDesktopManagerInternal_S*, IObjectArray_Local**);
        ((Fn)(*(void***)mgr)[7])(mgr, &arr);
    }
    if (!arr) { mgr->Release(); Wh_Log(L"[VD] GetDesktops failed"); return; }

    UINT count = 0;
    arr->GetCount(&count);
    if (targetIndex < 0 || (UINT)targetIndex >= count) { arr->Release(); mgr->Release(); return; }

    IVirtualDesktop_S* target = nullptr;
    arr->GetAt((UINT)targetIndex, IID_VD, (void**)&target);
    arr->Release();
    if (!target) { mgr->Release(); Wh_Log(L"[VD] GetAt failed"); return; }

    if (usesHMonitor) {
        typedef HRESULT(STDMETHODCALLTYPE* FnM)(IVirtualDesktopManagerInternal_S*, HMONITOR, IVirtualDesktop_S*);
        ((FnM)(*(void***)mgr)[9])(mgr, nullptr, target);
    } else {
        typedef HRESULT(STDMETHODCALLTYPE* Fn)(IVirtualDesktopManagerInternal_S*, IVirtualDesktop_S*);
        ((Fn)(*(void***)mgr)[9])(mgr, target);
    }
    target->Release();
    mgr->Release();
    Wh_Log(L"[VD] Switched to desktop %d", targetIndex);
}

// ============================================================
// Button grid building
// ============================================================

static Brush ParseColorBrush(const std::wstring& hex) {
    if (hex.empty() || hex[0] != L'#') return nullptr;
    std::wstring h = hex.substr(1);
    if (h.size() == 6) h = L"FF" + h;
    if (h.size() != 8) return nullptr;
    UINT32 val = 0;
    for (wchar_t c : h) {
        val <<= 4;
        if      (c >= L'0' && c <= L'9') val |= (UINT32)(c - L'0');
        else if (c >= L'A' && c <= L'F') val |= (UINT32)(10 + c - L'A');
        else if (c >= L'a' && c <= L'f') val |= (UINT32)(10 + c - L'a');
        else return nullptr;
    }
    winrt::Windows::UI::Color color;
    color.A = (BYTE)(val >> 24); color.R = (BYTE)(val >> 16);
    color.G = (BYTE)(val >> 8);  color.B = (BYTE)(val);
    SolidColorBrush brush; brush.Color(color); return brush;
}

static std::wstring ToRoman(int n) {
    if (n <= 0 || n > 3999) return std::to_wstring(n);
    static const struct { int v; const wchar_t* s; } t[] = {
        {1000,L"M"},{900,L"CM"},{500,L"D"},{400,L"CD"},
        {100,L"C"},{90,L"XC"},{50,L"L"},{40,L"XL"},
        {10,L"X"},{9,L"IX"},{5,L"V"},{4,L"IV"},{1,L"I"}
    };
    std::wstring r;
    for (auto& [v, s] : t) { while (n >= v) { r += s; n -= v; } }
    return r;
}

static std::wstring GetButtonLabel(int idx, int current) {
    if (g_settings.labelFormat == L"dot")
        return (idx == current) ? L"●" : L"○";
    if (g_settings.labelFormat == L"roman")
        return ToRoman(idx + 1);
    if (g_settings.labelFormat == L"custom" && !g_settings.customLabels.empty()) {
        std::wistringstream ss(g_settings.customLabels);
        std::wstring token; int i = 0;
        while (std::getline(ss, token, L',')) { if (i++ == idx) return token; }
    }
    return std::to_wstring(idx + 1);
}

// Apply shine gradient to a base brush. Returns brush unchanged if no base color or shine off.
static Brush MakeShineBrush(Brush base) {
    if (!g_settings.shineEffect) return base;
    auto solid = base ? base.try_as<SolidColorBrush>() : nullptr;
    if (!solid) return base;
    auto c = solid.Color();

    LinearGradientBrush b;
    b.StartPoint({0.5, 0.0});
    b.EndPoint({0.5, 1.0});

    // Top: semi-transparent white highlight
    GradientStop g0; winrt::Windows::UI::Color shine{180,255,255,255};
    g0.Color(shine); g0.Offset(0.0); b.GradientStops().Append(g0);

    // Upper-mid: base color lightened slightly
    GradientStop g1;
    winrt::Windows::UI::Color light{c.A,
        (BYTE)std::min(255, (int)c.R + 35),
        (BYTE)std::min(255, (int)c.G + 35),
        (BYTE)std::min(255, (int)c.B + 35)};
    g1.Color(light); g1.Offset(0.42); b.GradientStops().Append(g1);

    // Lower: base color
    GradientStop g2; g2.Color(c); g2.Offset(0.52); b.GradientStops().Append(g2);

    // Bottom: slightly darker
    GradientStop g3;
    winrt::Windows::UI::Color dark{c.A,
        (BYTE)(c.R * 7 / 10), (BYTE)(c.G * 7 / 10), (BYTE)(c.B * 7 / 10)};
    g3.Color(dark); g3.Offset(1.0); b.GradientStops().Append(g3);

    return b;
}

// Layout calculation result — explicit rows/cols and offset for the short last group.
struct GridLayout {
    int rows        = 1;
    int cols        = 1;
    int shortOffset = 0;  // start row (column-first) or start column (row-first) for partial last group
};

static int GetAvailableRows(int count, bool logDetails = true) {
    int rows = count;
    RECT r{};
    HWND hWnd = g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
    if (hWnd && GetWindowRect(hWnd, &r)) {
        int taskbarH = (int)(r.bottom - r.top);
        bool sliverMode = g_settings.showMasterButton &&
                          (g_settings.masterButtonPosition == L"top" ||
                           g_settings.masterButtonPosition == L"bottom");
        // masterButtonSpacing is a button-within-row visual offset, not a layout dimension.
        int sliverTotal = g_settings.masterButtonHeight
                        + std::max(0, g_settings.buttonSpacing);
        if (sliverMode)
            taskbarH -= sliverTotal;
        int denom = std::max(1, g_settings.buttonHeight + std::max(0, g_settings.buttonSpacing));
        rows = std::max(1, taskbarH / denom);
        if (logDetails) {
            Wh_Log(L"[Layout] taskbarH=%d sliver=%d denom=%d -> %d rows available",
                   taskbarH + (sliverMode ? sliverTotal : 0),
                   sliverMode ? 1 : 0, denom, rows);
        }
    } else {
        if (logDetails)
            Wh_Log(L"[Layout] No taskbar window — using count=%d as row limit", count);
    }
    rows = std::min(rows, count);
    if (g_settings.buttonRows > 0)
        rows = std::min(rows, g_settings.buttonRows);
    return std::max(rows, 1);
}

static int LayoutScore(int rows, int cols, int count, int maxRows) {
    int capacity = rows * cols;
    int waste = capacity - count;
    // Only penalize wide layouts (cols > rows). Tall layouts (rows > cols) are
    // desirable in a taskbar — don't penalize them.
    int widePenalty = cols > rows ? (cols - rows) * 2 : 0;
    int score = waste * 10 + widePenalty;

    if (g_settings.smartLayout == L"packVertical")
        score -= rows * 20;
    else if (g_settings.smartLayout == L"packHorizontal")
        score += rows * 20;
    else {
        // Balanced default: prefer more rows (use available height) without waste.
        // Examples:
        //   4 buttons, 4 rows available -> 4x1 single column
        //   4 buttons, 3 rows available -> 2x2 (waste-free square beats 3+1 waste)
        //   5 buttons, 4 rows available -> 3x2 then 2x2... see scoring
        score -= rows * 3;
    }
    return score;
}

static GridLayout ComputeLayout(int count, bool logDetails = true) {
    GridLayout L;
    int maxRows = GetAvailableRows(count, logDetails);

    if (g_settings.gridMode == L"singleRow") {
        L.rows = 1;
        L.cols = count;
    } else if (g_settings.gridMode == L"singleColumn") {
        L.rows = count;
        L.cols = 1;
    } else if (g_settings.gridMode == L"fixedRows") {
        L.rows = std::min(std::max(g_settings.buttonRows, 1), count);
        L.cols = (count + L.rows - 1) / L.rows;
    } else if (g_settings.gridMode == L"fixedColumns") {
        L.cols = std::min(std::max(g_settings.buttonColumns, 1), count);
        L.rows = (count + L.cols - 1) / L.cols;
    } else if (g_settings.gridMode == L"fixedGrid") {
        L.rows = std::min(std::max(g_settings.buttonRows, 1), count);
        L.cols = (g_settings.buttonColumns > 0)
            ? std::min(g_settings.buttonColumns, count)
            : (count + L.rows - 1) / L.rows;
        if (L.rows * L.cols < count)
            L.rows = (count + L.cols - 1) / L.cols;
    } else {
        int bestRows = 1, bestCols = count, bestScore = INT_MAX;
        int firstRows = (maxRows > 1 && count > 1 && g_settings.smartLayout != L"packHorizontal")
            ? 2
            : 1;
        for (int rows = firstRows; rows <= maxRows; rows++) {
            int cols = (count + rows - 1) / rows;
            if (g_settings.buttonColumns > 0 && cols > g_settings.buttonColumns)
                continue;
            int score = LayoutScore(rows, cols, count, maxRows);
            if (score < bestScore) {
                bestScore = score;
                bestRows = rows;
                bestCols = cols;
            }
        }
        if (bestScore == INT_MAX && g_settings.buttonColumns > 0) {
            bestCols = std::min(g_settings.buttonColumns, count);
            bestRows = (count + bestCols - 1) / bestCols;
        }
        L.rows = bestRows;
        L.cols = bestCols;
    }

    L.rows = std::max(1, std::min(L.rows, count));
    L.cols = std::max(1, L.cols);
    while (L.rows * L.cols < count) {
        if (g_settings.gridMode == L"fixedColumns")
            L.rows++;
        else
            L.cols++;
    }

    if (logDetails) {
        Wh_Log(L"[Layout] count=%d -> %dx%d (mode=%s)", count, L.rows, L.cols,
               g_settings.gridMode.c_str());
    }

    bool rowFirst = (g_settings.fillOrder == L"rowFirst");
    if (rowFirst) {
        // Short last-row offset.
        int lastCount = count % L.cols;
        if (lastCount == 0) lastCount = L.cols;
        if (lastCount > 0 && lastCount < L.cols) {
            if      (g_settings.shortGroupAlign == L"center") L.shortOffset = (L.cols - lastCount) / 2;
            else if (g_settings.shortGroupAlign == L"end")    L.shortOffset = L.cols - lastCount;
        }
    } else {
        // Short last-column offset.
        int lastCount = count % L.rows;
        if (lastCount == 0) lastCount = L.rows;
        if (lastCount > 0 && lastCount < L.rows) {
            if      (g_settings.shortGroupAlign == L"center") L.shortOffset = (L.rows - lastCount) / 2;
            else if (g_settings.shortGroupAlign == L"end")    L.shortOffset = L.rows - lastCount;
        }
    }
    return L;
}

static void GetButtonGridPosition(int index, int count, const GridLayout& layout,
                                  int& row, int& col) {
    int localRow = 0, localCol = 0;
    int group = 0;
    bool rowFirst = (g_settings.fillOrder == L"rowFirst");

    if (rowFirst) {
        group = index / layout.cols;
        localCol = index % layout.cols;
        bool isLastRow = (group == (count - 1) / layout.cols);
        if (isLastRow)
            localCol += layout.shortOffset;

        row = group;
        col = localCol;
    } else {
        group = index / layout.rows;
        localRow = index % layout.rows;
        bool isLastCol = (group == (count - 1) / layout.rows);
        if (isLastCol)
            localRow += layout.shortOffset;

        row = localRow;
        col = group;
    }
}

static double EstimateButtonGridWidth(int count) {
    auto layout = ComputeLayout(count, false);
    bool hasMaster    = g_settings.showMasterButton;
    bool masterIsRow  = g_settings.masterButtonPosition == L"bottom" ||
                         g_settings.masterButtonPosition == L"top";
    int gridCols = layout.cols + (hasMaster && !masterIsRow ? 1 : 0);
    int gaps = std::max(0, gridCols - 1);
    int masterExtra = (hasMaster && !masterIsRow)
        ? (g_settings.masterButtonWidth - g_settings.buttonWidth)
        : 0;
    return (double)(gridCols * g_settings.buttonWidth + masterExtra +
                    gaps * g_settings.buttonSpacing);
}

static double EstimateButtonGridHeight(int count) {
    auto layout = ComputeLayout(count, false);
    bool hasMaster    = g_settings.showMasterButton;
    bool masterIsRow  = g_settings.masterButtonPosition == L"bottom" ||
                         g_settings.masterButtonPosition == L"top";
    int gridRows = layout.rows + (hasMaster && masterIsRow ? 1 : 0);
    int gaps = std::max(0, gridRows - 1);
    int masterExtra = (hasMaster && masterIsRow)
        ? (g_settings.masterButtonHeight - g_settings.buttonHeight)
        : 0;
    return (double)(gridRows * g_settings.buttonHeight + masterExtra +
                    gaps * g_settings.buttonSpacing);
}

// Apply shared visual style to a desktop button.
static void StyleButton(Button& btn, bool isActive,
    Brush activeBrush, Brush inactiveBrush,
    Brush activeTextBrush, Brush inactiveTextBrush,
    Brush borderBrush)
{
    btn.Padding({ 1.0, 0.0, 1.0, 0.0 });
    btn.FontSize((double)g_settings.fontSize);
    btn.HorizontalAlignment(HorizontalAlignment::Stretch);
    btn.VerticalAlignment(VerticalAlignment::Stretch);

    if (isActive && activeBrush)        btn.Background(activeBrush);
    else if (!isActive && inactiveBrush) btn.Background(inactiveBrush);

    if (isActive && activeTextBrush)        btn.Foreground(activeTextBrush);
    else if (!isActive && inactiveTextBrush) btn.Foreground(inactiveTextBrush);

    if (g_settings.activeBold)
        btn.FontWeight(isActive
            ? winrt::Windows::UI::Text::FontWeights::Bold()
            : winrt::Windows::UI::Text::FontWeights::Normal());

    { double r = (double)g_settings.cornerRadius; btn.CornerRadius({ r, r, r, r }); }

    if (borderBrush) {
        btn.BorderBrush(borderBrush);
        if (g_settings.borderThickness > 0) {
            double t = (double)g_settings.borderThickness;
            btn.BorderThickness({ t, t, t, t });
        }
    }
}

static Grid BuildButtonGrid(int count, int current) {
    auto layout = ComputeLayout(count);
    int rows = layout.rows;
    int cols = layout.cols;
    Wh_Log(L"[Layout] count=%d rows=%d cols=%d mode=%ls smart=%ls fill=%ls",
           count, rows, cols, g_settings.gridMode.c_str(),
           g_settings.smartLayout.c_str(), g_settings.fillOrder.c_str());

    bool hasMaster    = g_settings.showMasterButton;
    bool masterBefore = (g_settings.masterButtonPosition == L"before");
    bool masterBottom = (g_settings.masterButtonPosition == L"bottom");
    bool masterTop    = (g_settings.masterButtonPosition == L"top");
    bool masterIsRow  = masterBottom || masterTop;

    // Column-based placement: master gets an extra column to the left or right.
    // Row-based placement (top/bottom): master gets an extra sliver row spanning all desktop columns.
    int gridCols   = cols + (hasMaster && !masterIsRow ? 1 : 0);
    int gridRows   = rows + (hasMaster && masterIsRow  ? 1 : 0);
    int masterCol  = (hasMaster && !masterIsRow) ? (masterBefore ? 0 : cols) : -1;
    int masterRow  = (hasMaster && masterIsRow)  ? (masterBottom ? rows : 0) : -1;
    int deskColOff = (hasMaster && masterBefore) ? 1 : 0;
    int deskRowOff = (hasMaster && masterTop)    ? 1 : 0;

    Grid grid;
    grid.Name(L"VdSwitcherBar");
    grid.VerticalAlignment(VerticalAlignment::Center);
    if (g_settings.buttonSpacing > 0) {
        grid.ColumnSpacing((double)g_settings.buttonSpacing);
        grid.RowSpacing((double)g_settings.buttonSpacing);
    }
    if (g_settings.buttonOpacity < 100)
        grid.Opacity(std::max(0.0, std::min(1.0, g_settings.buttonOpacity / 100.0)));
    {
        // Explicit top-alignment: compute where the desktop button area should start
        // so it's vertically centered in the taskbar regardless of sliver presence.
        // VerticalAlignment::Center + Margin cannot handle cases where the grid
        // is taller than the taskbar slot (XAML clamps, causing distortion).
        int taskbarH = 0;
        {
            HWND hWnd2 = g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
            RECT r2{};
            if (hWnd2 && GetWindowRect(hWnd2, &r2))
                taskbarH = (int)(r2.bottom - r2.top);
        }
        double marginTop = (double)g_settings.gridVerticalOffset;
        if (taskbarH > 0) {
            int desktopAreaH = rows * g_settings.buttonHeight
                             + std::max(0, rows - 1) * std::max(0, g_settings.buttonSpacing);
            // Center the desktop button area, then shift up if the sliver is above it.
            marginTop += (double)((taskbarH - desktopAreaH) / 2);
            if (hasMaster && masterIsRow && masterTop)
                marginTop -= (double)(g_settings.masterButtonHeight
                                    + std::max(0, g_settings.buttonSpacing));
            grid.VerticalAlignment(VerticalAlignment::Top);
        }
        grid.Margin({ (double)g_settings.paddingLeft, marginTop, (double)g_settings.paddingRight, 0.0 });
    }

    for (int r = 0; r < gridRows; r++) {
        RowDefinition rd;
        bool isSliverRow = hasMaster && masterIsRow && (r == masterRow);
        double rowH = isSliverRow ? (double)g_settings.masterButtonHeight
                                  : (double)g_settings.buttonHeight;
        rd.Height({ rowH, GridUnitType::Pixel });
        grid.RowDefinitions().Append(rd);
    }
    for (int c = 0; c < gridCols; c++) {
        ColumnDefinition cd;
        bool isMasterCol = hasMaster && !masterIsRow && (c == masterCol);
        double colW = isMasterCol ? (double)g_settings.masterButtonWidth : (double)g_settings.buttonWidth;
        cd.Width({ colW, GridUnitType::Pixel });
        grid.ColumnDefinitions().Append(cd);
    }

    auto activeBrush       = MakeShineBrush(ParseColorBrush(g_settings.activeColor));
    auto inactiveBrush     = MakeShineBrush(ParseColorBrush(g_settings.inactiveColor));
    auto activeTextBrush   = ParseColorBrush(g_settings.activeTextColor);
    auto inactiveTextBrush = ParseColorBrush(g_settings.inactiveTextColor);
    auto borderBrush       = ParseColorBrush(g_settings.borderColor);
    auto desktopNames      = ReadDesktopNames(count);

    for (int i = 0; i < count; i++) {
        int btnCol, btnRow;
        GetButtonGridPosition(i, count, layout, btnRow, btnCol);
        btnRow += deskRowOff;
        btnCol += deskColOff;

        Button btn;
        btn.Name(L"VdBtn_" + std::to_wstring(i));
        btn.Content(winrt::box_value(GetButtonLabel(i, current)));
        StyleButton(btn, i == current, activeBrush, inactiveBrush,
                    activeTextBrush, inactiveTextBrush, borderBrush);
        ToolTipService::SetToolTip(btn, winrt::box_value(winrt::hstring(desktopNames[i])));

        int capturedIdx = i;
        btn.Click([capturedIdx](auto const&, auto const&) {
            if (g_unloading) return;
            // Dispatch to a background thread to avoid STA re-entrancy: when
            // SwitchToDesktop makes a LOCAL_SERVER COM call on the UI thread,
            // the STA message pump runs and can deliver the notification thread's
            // SendMessage re-entrantly, corrupting XAML state mid-click.
            g_activeSwitchThreads.fetch_add(1);
            HANDLE h = CreateThread(nullptr, 0, [](LPVOID p) -> DWORD {
                int idx = (int)(INT_PTR)p;
                CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
                if (!g_unloading) SwitchToDesktop(idx);
                CoUninitialize();
                g_activeSwitchThreads.fetch_sub(1);
                return 0;
            }, (LPVOID)(INT_PTR)capturedIdx, 0, nullptr);
            if (h) CloseHandle(h);
            else g_activeSwitchThreads.fetch_sub(1);
        });

        Grid::SetRow(btn, btnRow);
        Grid::SetColumn(btn, btnCol);

        // Short last column (column-first): integer (rows-1)/2 rounds to 0 for rows=2,
        // so the offset approach can't center. Span all desktop rows and use Margin.Top
        // for pixel-precise placement. Grid does not clip children.
        if (g_settings.fillOrder != L"rowFirst" && layout.rows > 1
                && g_settings.shortGroupAlign != L"start") {
            int lastCount = count % layout.rows;
            if (lastCount == 0) lastCount = layout.rows;
            if (lastCount < layout.rows) {
                int group = i / layout.rows;
                bool isLastGroup = (group == (count - 1) / layout.rows);
                if (isLastGroup) {
                    int k = i % layout.rows;  // 0-based index within this short column
                    double unitH = (double)(g_settings.buttonHeight + g_settings.buttonSpacing);
                    double topOff = (g_settings.shortGroupAlign == L"end")
                        ? unitH * (rows - lastCount)
                        : unitH * (rows - lastCount) / 2.0;
                    Grid::SetRow(btn, deskRowOff);
                    Grid::SetRowSpan(btn, rows);
                    btn.Height((double)g_settings.buttonHeight);
                    btn.VerticalAlignment(VerticalAlignment::Top);
                    btn.Margin({ 0.0, topOff + (double)k * unitH, 0.0, 0.0 });
                }
            }
        }

        // Short last row (row-first): same pixel-precise approach, using Margin.Left
        // and column span instead of Margin.Top and row span.
        if (g_settings.fillOrder == L"rowFirst" && layout.cols > 1
                && g_settings.shortGroupAlign != L"start") {
            int lastCount = count % layout.cols;
            if (lastCount == 0) lastCount = layout.cols;
            if (lastCount < layout.cols) {
                int group = i / layout.cols;
                bool isLastGroup = (group == (count - 1) / layout.cols);
                if (isLastGroup) {
                    int k = i % layout.cols;  // 0-based index within this short row
                    double unitW = (double)(g_settings.buttonWidth + g_settings.buttonSpacing);
                    double leftOff = (g_settings.shortGroupAlign == L"end")
                        ? unitW * (cols - lastCount)
                        : unitW * (cols - lastCount) / 2.0;
                    Grid::SetColumn(btn, deskColOff);
                    Grid::SetColumnSpan(btn, cols);
                    btn.Width((double)g_settings.buttonWidth);
                    btn.HorizontalAlignment(HorizontalAlignment::Left);
                    btn.Margin({ leftOff + (double)k * unitW, 0.0, 0.0, 0.0 });
                }
            }
        }

        grid.Children().Append(btn);
    }

    // Master button: spans all rows in its column.
    if (hasMaster) {
        Button masterBtn;
        masterBtn.Name(L"VdMasterBtn");
        masterBtn.Content(winrt::box_value(winrt::hstring(g_settings.masterButtonLabel)));
        StyleButton(masterBtn, false, inactiveBrush, inactiveBrush,
                    inactiveTextBrush, inactiveTextBrush, borderBrush);
        ToolTipService::SetToolTip(masterBtn,
            winrt::box_value(winrt::hstring(L"Task View (Win+Tab)")));
        if (masterIsRow) {
            // RowSpacing already gives buttonSpacing gap between all rows.
            // masterButtonSpacing is an additive offset on top of that natural gap.
            double extra = (double)g_settings.masterButtonSpacing;
            if (masterTop)    masterBtn.Margin({ 0.0, 0.0, 0.0, extra });
            else              masterBtn.Margin({ 0.0, extra, 0.0, 0.0 });
        }
        masterBtn.Click([](auto const&, auto const&) {
            if (g_unloading) return;
            INPUT inputs[4]{};
            inputs[0].type = INPUT_KEYBOARD; inputs[0].ki.wVk = VK_LWIN;
            inputs[1].type = INPUT_KEYBOARD; inputs[1].ki.wVk = VK_TAB;
            inputs[2].type = INPUT_KEYBOARD; inputs[2].ki.wVk = VK_TAB;  inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
            inputs[3].type = INPUT_KEYBOARD; inputs[3].ki.wVk = VK_LWIN; inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
        });
        if (masterIsRow) {
            Grid::SetRow(masterBtn, masterRow);
            Grid::SetColumn(masterBtn, 0);
            if (cols > 1) Grid::SetColumnSpan(masterBtn, cols);
        } else {
            Grid::SetColumn(masterBtn, masterCol);
            Grid::SetRow(masterBtn, deskRowOff);
            if (rows > 1) Grid::SetRowSpan(masterBtn, rows);
        }
        grid.Children().Append(masterBtn);
    }

    return grid;
}

// Update button highlights and labels in-place (no rebuild).
static void UpdateHighlights(int current) {
    if (!g_buttonGrid) return;
    auto activeBrush       = MakeShineBrush(ParseColorBrush(g_settings.activeColor));
    auto inactiveBrush     = MakeShineBrush(ParseColorBrush(g_settings.inactiveColor));
    auto activeTextBrush   = ParseColorBrush(g_settings.activeTextColor);
    auto inactiveTextBrush = ParseColorBrush(g_settings.inactiveTextColor);
    static const std::wstring kPrefix = L"VdBtn_";
    int n = (int)g_buttonGrid.Children().Size();
    for (int i = 0; i < n; i++) {
        auto btn = g_buttonGrid.Children().GetAt(i).try_as<Button>();
        if (!btn) continue;
        auto name = std::wstring(btn.Name());
        // Skip master button and anything that isn't a desktop button.
        if (name.size() <= kPrefix.size() || name.compare(0, kPrefix.size(), kPrefix) != 0)
            continue;
        int desktopIdx = _wtoi(name.c_str() + kPrefix.size());
        bool isActive = (desktopIdx == current);
        if (g_settings.labelFormat == L"dot")
            btn.Content(winrt::box_value(std::wstring(isActive ? L"●" : L"○")));
        Brush bg = isActive ? activeBrush : inactiveBrush;
        btn.Background(bg ? bg : nullptr);
        if (activeTextBrush || inactiveTextBrush) {
            Brush fg = isActive ? activeTextBrush : inactiveTextBrush;
            if (fg)
                btn.Foreground(fg);
            else
                btn.ClearValue(Control::ForegroundProperty());
        }
        if (g_settings.activeBold)
            btn.FontWeight(isActive
                ? winrt::Windows::UI::Text::FontWeights::Bold()
                : winrt::Windows::UI::Text::FontWeights::Normal());
    }
}

// ============================================================
// Injection into XAML tree
// ============================================================

static FrameworkElement FindStartButton(FrameworkElement root) {
    return FindChildRecursive(root, [](FrameworkElement fe) {
        if (winrt::get_class_name(fe) != L"Taskbar.ExperienceToggleButton")
            return false;
        return AutomationProperties::GetAutomationId(fe) == L"StartButton";
    });
}

static FrameworkElement FindTaskbarRootGrid(FrameworkElement root) {
    auto taskbarFrame = FindChildRecursive(root, [](FrameworkElement fe) {
        return winrt::get_class_name(fe) == L"Taskbar.TaskbarFrame";
    });
    if (!taskbarFrame) return nullptr;

    int n = VisualTreeHelper::GetChildrenCount(taskbarFrame);
    for (int i = 0; i < n; i++) {
        auto child = VisualTreeHelper::GetChild(taskbarFrame, i).try_as<FrameworkElement>();
        if (child && child.Name() == L"RootGrid")
            return child;
    }
    return nullptr;
}

static FrameworkElement FindTaskbarFrameRepeater(FrameworkElement rootGrid) {
    int n = VisualTreeHelper::GetChildrenCount(rootGrid);
    for (int i = 0; i < n; i++) {
        auto child = VisualTreeHelper::GetChild(rootGrid, i).try_as<FrameworkElement>();
        if (child && child.Name() == L"TaskbarFrameRepeater")
            return child;
    }
    return nullptr;
}

static void SetTaskItemsLeftMargin(double left) {
    if (!g_taskItemsPanel)
        return;

    auto margin = g_taskItemsPanel.Margin();
    if (std::fabs(margin.Left - left) <= 0.5)
        return;

    margin.Left = left;
    g_taskItemsPanel.Margin(margin);
}

static void SetStartButtonVisualOffset(double x) {
    if (!g_startOverlayStart)
        return;

    if (std::fabs(x) <= 0.5) {
        if (!g_startOverlayStart.RenderTransform())
            return;
        g_startOverlayStart.ClearValue(UIElement::RenderTransformProperty());
        return;
    }

    auto existing = g_startOverlayStart.RenderTransform().try_as<TranslateTransform>();
    if (existing && std::fabs(existing.X() - x) <= 0.5 && existing.Y() == 0.0)
        return;

    TranslateTransform tt;
    tt.X(x);
    tt.Y(0.0);
    g_startOverlayStart.RenderTransform(tt);
}

static double GetElementActualWidth(FrameworkElement const& element) {
    return element ? element.ActualWidth() : 0.0;
}

static double GetElementActualHeight(FrameworkElement const& element) {
    return element ? element.ActualHeight() : 0.0;
}

static void PositionButtonGridNearStart() {
    if (!g_buttonGrid || !g_startOverlayRoot || !g_startOverlayStart)
        return;

    int count = g_desktopCount.load();
    double gridW = EstimateButtonGridWidth(count);
    double gridH = EstimateButtonGridHeight(count);
    double outerGridW = gridW + (double)g_settings.paddingLeft +
                        (double)g_settings.paddingRight;
    const auto& pos = g_settings.position;

    bool startHidden = (g_startOverlayStart.Visibility() == Visibility::Collapsed);
    double startW = g_startOverlayStart.ActualWidth();
    double startH = g_startOverlayStart.ActualHeight();
    if (startW <= 0.0 && !startHidden) startW = 44.0;
    if (startH <= 0.0) startH = std::max((double)g_settings.buttonHeight, gridH);

    // x changes when TaskbarFrameRepeater.Margin.Left is pushed (start button moves with it).
    // y is unaffected by horizontal margin changes and is always valid for vertical centering.
    double x = 0.0;
    double y = 0.0;
    try {
        auto transform = g_startOverlayStart.TransformToVisual(g_startOverlayRoot);
        winrt::Windows::Foundation::Point origin{ 0.0f, 0.0f };
        auto p = transform.TransformPoint(origin);
        x = p.X;
        y = p.Y;
    } catch (...) {
    }

    double left = 0.0;
    double top  = 0.0;

    if (pos == L"overStart" || pos == L"aboveStart" || pos == L"belowStart") {
        // Grid overlays the Start button. Legacy above/below values are treated
        // as this mode; use gridVerticalOffset for vertical nudging.
        double anchorX = (g_startButtonOriginalX >= 0.0) ? g_startButtonOriginalX : x;
        left = anchorX + (double)g_settings.paddingLeft;
        top  = y + (startH - gridH) / 2.0;
        if (left < 0.0) left = 0.0;
        SetStartButtonVisualOffset(0.0);
    } else if (pos == L"rightOfStart") {
        // Grid sits immediately right of the start button and reserves room for
        // itself before taskbar items. TaskbarFrameRepeater.Margin.Left moves
        // Start too, so counter-shift Start visually back to its stable anchor.
        double anchorX = (g_startButtonOriginalX >= 0.0) ? g_startButtonOriginalX : x;
        left = anchorX + startW + (double)g_settings.paddingLeft;
        top  = y + (startH - gridH) / 2.0;
        if (left < 0.0) left = 0.0;

        if (g_taskItemsPanel) {
            double push = outerGridW + (double)g_settings.buttonSpacing;
            SetTaskItemsLeftMargin(g_taskItemsPanelOriginalMargin.Left + push);
            if (!startHidden)
                SetStartButtonVisualOffset(-push);
        } else {
            SetStartButtonVisualOffset(0.0);
        }
    } else {
        // nextToStart: anchor VD grid at the left edge; push TaskbarFrameRepeater
        // rightward so Start button and task items don't overlap the VD grid.
        // y from TransformToVisual is unaffected by Margin.Left changes, so it
        // stays valid for vertical centering even after we push the panel right.
        left = (double)g_settings.paddingLeft;
        top  = y + (startH - gridH) / 2.0;
        SetStartButtonVisualOffset(0.0);

        if (g_taskItemsPanel) {
            double neededLeft = g_taskItemsPanelOriginalMargin.Left +
                                outerGridW + (double)g_settings.buttonSpacing;
            SetTaskItemsLeftMargin(neededLeft);
        }
    }

    // gridVerticalOffset nudges the grid in all overlay modes
    top += (double)g_settings.gridVerticalOffset;

    double rootW = GetElementActualWidth(g_startOverlayRoot);
    double rootH = GetElementActualHeight(g_startOverlayRoot);
    if (rootW > 0.0 && left + gridW > rootW)
        left = std::max(0.0, rootW - gridW);
    if (rootH > 0.0 && top + gridH > rootH)
        top = std::max(0.0, rootH - gridH);
    if (top < 0.0) top = 0.0;

    g_buttonGrid.HorizontalAlignment(HorizontalAlignment::Left);
    g_buttonGrid.VerticalAlignment(VerticalAlignment::Top);
    auto current = g_buttonGrid.Margin();
    if (std::fabs(current.Left - left) > 0.5 ||
        std::fabs(current.Top - top) > 0.5 ||
        current.Right != 0.0 ||
        current.Bottom != 0.0) {
        g_buttonGrid.Margin({ left, top, 0.0, 0.0 });
    }
}

static bool InjectButtonGridNearStart(FrameworkElement root) {
    auto rootGrid = FindTaskbarRootGrid(root);
    if (!rootGrid) {
        Wh_Log(L"[Inject] Taskbar RootGrid not found");
        return false;
    }

    auto gridParent = rootGrid.try_as<Grid>();
    if (!gridParent) {
        Wh_Log(L"[Inject] Taskbar RootGrid is not a Grid");
        return false;
    }

    for (auto child : gridParent.Children()) {
        if (auto fe = child.try_as<FrameworkElement>(); fe && fe.Name() == L"VdSwitcherBar")
            return true;
    }

    auto startButton = FindStartButton(root);
    if (!startButton) {
        Wh_Log(L"[Inject] StartButton not found");
        return false;
    }

    int count   = ReadDesktopCount();
    int current = ReadCurrentDesktop();
    g_desktopCount.store(count);
    g_currentDesktop.store(current);

    if (g_settings.hideWhenSingle && count <= 1) {
        Wh_Log(L"[Inject] Skipping start overlay - hideWhenSingle, count=%d", count);
        return true;
    }

    auto grid = BuildButtonGrid(count, current);
    grid.IsHitTestVisible(true);
    Grid::SetColumn(grid, 0);
    Grid::SetColumnSpan(grid, std::max(1, (int)gridParent.ColumnDefinitions().Size()));
    Canvas::SetZIndex(grid, 1000);
    gridParent.Children().Append(grid);

    g_buttonGrid = grid;
    g_injectionParent = rootGrid;
    g_injectedColumn = -1;
    g_startOverlayMode = true;
    g_startOverlayRoot = rootGrid;
    g_startOverlayStart = startButton;

    // Capture TaskbarFrameRepeater for modes that reserve space near Start.
    if (auto repeater = FindTaskbarFrameRepeater(rootGrid)) {
        g_taskItemsPanel = repeater;
        g_taskItemsPanelOriginalMargin = repeater.Margin();
    }

    // Capture original start button x before any margin pushes — stable anchor.
    try {
        auto t = startButton.TransformToVisual(rootGrid);
        winrt::Windows::Foundation::Point o{ 0.0f, 0.0f };
        g_startButtonOriginalX = t.TransformPoint(o).X;
    } catch (...) {
        g_startButtonOriginalX = -1.0;
    }

    PositionButtonGridNearStart();
    g_startOverlayLayoutToken = rootGrid.LayoutUpdated(
        [](winrt::Windows::Foundation::IInspectable const&, auto const&) {
            if (!g_unloading)
                PositionButtonGridNearStart();
        });

    Wh_Log(L"[Inject] VdSwitcherBar near Start (%d desktops, current=%d)", count, current);
    return true;
}

static bool InjectButtonGrid(FrameworkElement root) {
    const auto& pos = g_settings.position;

    if (pos == L"nextToStart" || pos == L"aboveStart" || pos == L"overStart" ||
        pos == L"belowStart"  || pos == L"rightOfStart")
        return InjectButtonGridNearStart(root);

    FrameworkElement parent = FindChildRecursive(root, [](FrameworkElement fe) {
        return fe.Name() == L"SystemTrayFrameGrid";
    });
    if (!parent) {
        Wh_Log(L"[Inject] SystemTrayFrameGrid not found");
        return false;
    }

    // SystemTrayFrameGrid is a Grid with column-based layout. We must insert a new
    // ColumnDefinition and shift existing elements rather than relying on Children order.
    auto gridParent = parent.try_as<Grid>();
    if (!gridParent) { Wh_Log(L"[Inject] Parent is not a Grid"); return false; }

    // Already injected?
    for (auto child : gridParent.Children()) {
        if (auto fe = child.try_as<FrameworkElement>(); fe && fe.Name() == L"VdSwitcherBar") {
            // Re-acquire state in case it was lost (e.g., transient null from GetTaskbarXamlRoot
            // during RebuildOrUpdate caused g_buttonGrid to be cleared while grid stayed in tree).
            if (!g_buttonGrid) {
                g_buttonGrid = fe.try_as<Grid>();
                g_injectionParent = parent;
                g_injectedColumn = Grid::GetColumn(fe);
                Wh_Log(L"[Inject] Re-acquired existing VdSwitcherBar at col=%d", g_injectedColumn);
            }
            return true;
        }
    }

    int count   = ReadDesktopCount();
    int current = ReadCurrentDesktop();
    g_desktopCount.store(count);
    g_currentDesktop.store(current);

    if (g_settings.hideWhenSingle && count <= 1) {
        Wh_Log(L"[Inject] Skipping — hideWhenSingle, count=%d", count);
        return true;  // notification thread will watch for desktop additions
    }

    auto grid = BuildButtonGrid(count, current);

    // Find a named direct child of the tray grid.
    auto findNamedDirect = [&](const wchar_t* name) -> FrameworkElement {
        for (auto child : gridParent.Children()) {
            if (auto fe = child.try_as<FrameworkElement>(); fe && fe.Name() == name)
                return fe;
        }
        return nullptr;
    };

    // Map position setting → reference element + whether to insert after it.
    FrameworkElement refElem = nullptr;
    bool insertAfterRef = false;

    if      (pos == L"beforeOmni")
        refElem = findNamedDirect(L"ControlCenterButton");
    else if (pos == L"beforeClock")
        refElem = findNamedDirect(L"NotificationCenterButton");
    else if (pos == L"afterClock")
        refElem = findNamedDirect(L"ShowDesktopStack");
    else if (pos == L"afterShowDesktop") {
        refElem = findNamedDirect(L"ShowDesktopStack");
        insertAfterRef = true;
    }
    // beforeIcons → column 0 (refElem stays nullptr)

    int insertCol;
    if (insertAfterRef && refElem)
        insertCol = Grid::GetColumn(refElem) + 1;
    else if (refElem)
        insertCol = Grid::GetColumn(refElem);
    else
        insertCol = 0;  // beforeIcons: leftmost column in tray

    // Insert a new Auto-width column at insertCol.
    ColumnDefinition cd;
    cd.Width({ 1.0, GridUnitType::Auto });
    if ((uint32_t)insertCol < gridParent.ColumnDefinitions().Size())
        gridParent.ColumnDefinitions().InsertAt((uint32_t)insertCol, cd);
    else
        gridParent.ColumnDefinitions().Append(cd);

    // Shift every existing child whose column is >= insertCol to make room.
    // Elements that start before insertCol but span through it get their span widened
    // so they continue to cover the same original columns (plus the new one).
    for (auto child : gridParent.Children()) {
        auto fe = child.try_as<FrameworkElement>();
        if (!fe) continue;
        int col  = Grid::GetColumn(fe);
        int span = Grid::GetColumnSpan(fe);
        if (col >= insertCol)
            Grid::SetColumn(fe, col + 1);
        else if (col + span > insertCol)
            Grid::SetColumnSpan(fe, span + 1);
    }

    Grid::SetColumn(grid, insertCol);
    Canvas::SetZIndex(grid, 10000);
    gridParent.Children().Append(grid);
    g_buttonGrid      = grid;
    g_injectionParent = parent;
    g_injectedColumn  = insertCol;

    Wh_Log(L"[Inject] VdSwitcherBar at column=%d in %ls (%d desktops, current=%d)",
           insertCol, parent.Name().c_str(), count, current);
    return true;
}

static Grid FindLiveSystemTrayFrameGrid() {
    HWND hWnd = g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
    if (!hWnd) return nullptr;

    auto xamlRoot = GetTaskbarXamlRoot(hWnd);
    if (!xamlRoot) return nullptr;
    auto root = xamlRoot.Content().try_as<FrameworkElement>();
    if (!root) return nullptr;

    auto parent = FindChildRecursive(root, [](FrameworkElement fe) {
        return fe.Name() == L"SystemTrayFrameGrid";
    });
    return parent ? parent.try_as<Grid>() : nullptr;
}

static bool RemoveButtonGridFrom(Grid gridParent, int col) {
    if (!gridParent) return false;

    uint32_t removeIdx = (uint32_t)-1;
    int liveCol = col;
    for (uint32_t i = 0; i < gridParent.Children().Size(); i++) {
        auto fe = gridParent.Children().GetAt(i).try_as<FrameworkElement>();
        if (fe && fe.Name() == L"VdSwitcherBar") {
            removeIdx = i;
            liveCol = Grid::GetColumn(fe);
            break;
        }
    }
    if (removeIdx == (uint32_t)-1) return false;

    gridParent.Children().RemoveAt(removeIdx);

    if (liveCol >= 0) {
        uint32_t colU = (uint32_t)liveCol;
        if (colU < gridParent.ColumnDefinitions().Size())
            gridParent.ColumnDefinitions().RemoveAt(colU);
        for (auto child : gridParent.Children()) {
            auto fe = child.try_as<FrameworkElement>();
            if (!fe) continue;
            int c    = Grid::GetColumn(fe);
            int span = Grid::GetColumnSpan(fe);
            if (c > liveCol)
                Grid::SetColumn(fe, c - 1);
            else if (c < liveCol && c + span > liveCol)
                Grid::SetColumnSpan(fe, span - 1);
        }
    }
    return true;
}

static void RemoveButtonGrid() {
    if (g_startOverlayMode) {
        if (g_startOverlayRoot && g_startOverlayLayoutToken.value) {
            g_startOverlayRoot.LayoutUpdated(g_startOverlayLayoutToken);
            g_startOverlayLayoutToken = {};
        }

        auto gridParent = g_injectionParent ? g_injectionParent.try_as<Grid>() : nullptr;
        if (gridParent) {
            for (uint32_t i = 0; i < gridParent.Children().Size(); i++) {
                auto fe = gridParent.Children().GetAt(i).try_as<FrameworkElement>();
                if (fe && fe.Name() == L"VdSwitcherBar") {
                    gridParent.Children().RemoveAt(i);
                    break;
                }
            }
        }

        if (g_taskItemsPanel) {
            g_taskItemsPanel.Margin(g_taskItemsPanelOriginalMargin);
            g_taskItemsPanel = nullptr;
        }
        SetStartButtonVisualOffset(0.0);
        g_startButtonOriginalX = -1.0;

        g_buttonGrid = nullptr;
        g_injectionParent = nullptr;
        g_injectedColumn = -1;
        g_startOverlayMode = false;
        g_startOverlayRoot = nullptr;
        g_startOverlayStart = nullptr;
        return;
    }

    auto gridParent = FindLiveSystemTrayFrameGrid();
    if (!RemoveButtonGridFrom(gridParent, g_injectedColumn))
        Wh_Log(L"[Remove] VdSwitcherBar not found");

    g_buttonGrid      = nullptr;
    g_injectionParent = nullptr;
    g_injectedColumn  = -1;
}

// Rebuild button grid (full or highlight-only) on the UI thread.
static void RebuildOrUpdate(bool fullRebuild) {
    int count   = ReadDesktopCount();
    int current = ReadCurrentDesktop();
    bool countChanged = (count != g_desktopCount.load());
    g_desktopCount.store(count);
    g_currentDesktop.store(current);

    if (g_settings.hideWhenSingle) {
        if (count <= 1) {
            if (g_buttonGrid) RemoveButtonGrid();
            return;
        }
        if (!g_buttonGrid) {
            ApplyAllSettings();
            return;
        }
    }

    if (fullRebuild || countChanged) {
        if (!g_buttonGrid) { ApplyAllSettings(); return; }
        Grid gridParent{nullptr};
        uint32_t idx;
        if (g_startOverlayMode) {
            if (!g_injectionParent) { ApplyAllSettings(); return; }
            gridParent = g_injectionParent.try_as<Grid>();
            if (!gridParent || !gridParent.Children().IndexOf(g_buttonGrid, idx)) {
                g_buttonGrid = nullptr;
                g_injectionParent = nullptr;
                g_injectedColumn = -1;
                ApplyAllSettings();
                return;
            }
        } else {
            // Use the live XAML tree — g_injectionParent may be stale if Windows
            // rebuilt the tray after a desktop add/remove.
            gridParent = FindLiveSystemTrayFrameGrid();
            if (!gridParent) {
                // XAML tree temporarily inaccessible (e.g., mid-rebuild by Windows).
                // Do NOT null g_buttonGrid — the grid is still in the tree; we just
                // can't reach it right now. The next notification will retry.
                Wh_Log(L"[Rebuild] XAML tree not accessible, deferring");
                return;
            }
            if (!gridParent.Children().IndexOf(g_buttonGrid, idx)) {
                // Our grid is genuinely gone (tray was rebuilt). Reinject from scratch.
                g_buttonGrid = nullptr;
                g_injectionParent = nullptr;
                g_injectedColumn = -1;
                ApplyAllSettings();
                return;
            }
        }
        // Capture the live column BEFORE removing the old grid, so even if
        // g_injectedColumn is stale (columns were renumbered by Windows), we
        // reinsert at the correct position.
        int liveColumn = g_startOverlayMode ? 0 : Grid::GetColumn(g_buttonGrid);
        gridParent.Children().RemoveAt(idx);
        g_buttonGrid = BuildButtonGrid(count, current);
        if (g_startOverlayMode) {
            Grid::SetColumn(g_buttonGrid, 0);
            Grid::SetColumnSpan(g_buttonGrid, std::max(1, (int)gridParent.ColumnDefinitions().Size()));
            Canvas::SetZIndex(g_buttonGrid, 1000);
            g_buttonGrid.IsHitTestVisible(true);
        } else if (liveColumn >= 0) {
            Grid::SetColumn(g_buttonGrid, liveColumn);
            g_injectedColumn = liveColumn;
            Canvas::SetZIndex(g_buttonGrid, 10000);
        }
        gridParent.Children().InsertAt(idx, g_buttonGrid);
        if (g_startOverlayMode)
            PositionButtonGridNearStart();
    } else {
        UpdateHighlights(current);
    }
}

// ============================================================
// Apply / cleanup
// ============================================================

static void ApplyAllSettings() {
    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) { Wh_Log(L"[Apply] No taskbar window"); return; }
    g_taskbarWnd = hWnd;

    try {
        auto xamlRoot = GetTaskbarXamlRoot(hWnd);
        if (!xamlRoot) { Wh_Log(L"[Apply] GetTaskbarXamlRoot failed"); return; }
        auto root = xamlRoot.Content().try_as<FrameworkElement>();
        if (!root) { Wh_Log(L"[Apply] No XAML root content"); return; }

        if (InjectButtonGrid(root))
            StartNotificationThread();
        else
            Wh_Log(L"[Apply] Injection failed");
    } catch (...) {
        Wh_Log(L"[Apply] Exception during injection (XAML not ready)");
    }
}

static void ApplyAllSettingsOnWindowThread() {
    HWND hWnd = g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
    if (!hWnd) return;
    RunFromWindowThread(hWnd, [](void*) { ApplyAllSettings(); }, nullptr);
}

// ============================================================
// Hooks
// ============================================================

using IconView_IconView_t = void* (WINAPI*)(void* pThis);
IconView_IconView_t IconView_IconView_Original;

void* WINAPI IconView_IconView_Hook(void* pThis) {
    auto result = IconView_IconView_Original(pThis);
    if (g_unloading || g_buttonGrid) return result;

    // Defer until the element is live in the XAML tree. Calling ApplyAllSettings
    // immediately from the constructor fires before the XamlRoot is stable, causing
    // null dereferences and WinRT exceptions that propagate through WH_CALLWNDPROC
    // and crash the process on startup.
    FrameworkElement iconView = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(iconView));
    if (!iconView) {
        // Fallback: element isn't a FrameworkElement; try immediate path.
        ApplyAllSettingsOnWindowThread();
        return result;
    }

    g_autoRevokerList.emplace_back();
    auto autoRevokerIt = std::prev(g_autoRevokerList.end());
    *autoRevokerIt = iconView.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt](auto const&, auto const&) {
            g_autoRevokerList.erase(autoRevokerIt);
            if (!g_unloading && !g_buttonGrid)
                ApplyAllSettingsOnWindowThread();
        });

    return result;
}

using LoadLibraryExW_t = HMODULE (WINAPI*)(LPCWSTR, HANDLE, DWORD);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE hModule = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (hModule && lpLibFileName)
        HandleLoadedModuleIfSystemTray(hModule, lpLibFileName);
    return hModule;
}

// ============================================================
// Symbol hook setup
// ============================================================

static bool HookTaskbarDllSymbols() {
    HMODULE h = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!h) return false;
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        { {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
          &CTaskBand_ITaskListWndSite_vftable },
        { {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
          &CTaskBand_GetTaskbarHost_Original },
        { {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
          &TaskbarHost_FrameHeight_Original },
        { {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
          &std__Ref_count_base__Decref_Original },
    };
    return WindhawkUtils::HookSymbols(h, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

static bool HookSystemTraySymbols(HMODULE hModule) {
    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK systemTrayHooks[] = {{
        {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
        &IconView_IconView_Original, IconView_IconView_Hook,
    }};
    if (!WindhawkUtils::HookSymbols(hModule, systemTrayHooks, ARRAYSIZE(systemTrayHooks))) {
        Wh_Log(L"[Hooks] HookSymbols failed");
        return false;
    }
    return true;
}

static void HandleLoadedModuleIfSystemTray(HMODULE hModule, LPCWSTR lpLibFileName) {
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == hModule &&
        !g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"[LoadLib] %s — hooking symbols", lpLibFileName);
        if (HookSystemTraySymbols(hModule))
            Wh_ApplyHookOperations();
    }
}

// ============================================================
// Windhawk lifecycle
// ============================================================

BOOL Wh_ModInit() {
    Wh_Log(L"[Init] VD Switcher v1.5");
    LoadSettings();
    DetectExplorerBuild();

    if (!HookTaskbarDllSymbols())
        Wh_Log(L"[Init] taskbar.dll hooks failed — GetTaskbarXamlRoot unavailable");

    if (HMODULE hSystemTray = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleHooked = true;
        if (!HookSystemTraySymbols(hSystemTray))
            Wh_Log(L"[Init] System tray symbol hooks failed");
    } else {
        Wh_Log(L"[Init] System tray module not loaded yet");
        HMODULE kernelbase = GetModuleHandleW(L"kernelbase.dll");
        auto pLoadLibraryExW = kernelbase
            ? reinterpret_cast<LoadLibraryExW_t>(GetProcAddress(kernelbase, "LoadLibraryExW"))
            : nullptr;
        if (pLoadLibraryExW)
            WindhawkUtils::Wh_SetFunctionHookT(pLoadLibraryExW,
                                               LoadLibraryExW_Hook,
                                               &LoadLibraryExW_Original);
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_systemTrayModuleHooked) {
        if (HMODULE hSystemTray = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"[AfterInit] System tray module found — hooking symbols");
                if (HookSystemTraySymbols(hSystemTray))
                    Wh_ApplyHookOperations();
            }
        }
    }
    if (g_systemTrayModuleHooked)
        ApplyAllSettingsOnWindowThread();

    g_retryStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_retryThread = CreateThread(nullptr, 0, [](void*) -> DWORD {
        for (int i = 0; i < 5 && !g_unloading; i++) {
            if (WaitForSingleObject(g_retryStopEvent, 2000) != WAIT_TIMEOUT) break;
            if (g_buttonGrid || g_unloading) break;
            Wh_Log(L"[AfterInit] Retry %d", i + 1);
            ApplyAllSettingsOnWindowThread();
        }
        return 0;
    }, nullptr, 0, nullptr);
}

void Wh_ModUninit() {
    g_unloading = true;
    Wh_Log(L"[Uninit]");

    // Drain any in-flight SwitchToDesktop background threads before stopping
    // the notification thread. Those threads access COM and mod globals; if they
    // outlive the DLL they crash. The click handler already guards with g_unloading,
    // so threads entering after this point exit immediately.
    while (g_activeSwitchThreads.load() > 0) Sleep(20);

    StopRetryThread();
    StopNotificationThread();

    // RunFromWindowThread is synchronous — blocks until the UI thread has removed the grid,
    // so all WinRT object lifetimes are safe and no FreeLibrary dance is needed.
    // Clear pending Loaded revokers on the UI thread so WinRT auto-revoke objects
    // are destroyed on the correct thread before the DLL is unloaded.
    if (g_taskbarWnd) {
        RunFromWindowThread(g_taskbarWnd, [](void*) {
            g_autoRevokerList.clear();
            RemoveButtonGrid();
        }, nullptr);
    } else {
        g_autoRevokerList.clear();
        RemoveButtonGrid();
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"[Settings] Changed");

    StopRetryThread();
    // Stop desktop-change callbacks before rebuilding tray columns. A late
    // callback during settings save can otherwise rebuild the old bar while the
    // UI thread is removing/reinserting columns.
    StopNotificationThread();

    HWND hWnd = g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
    if (!hWnd) return;

    RunFromWindowThread(hWnd, [](void*) {
        RemoveButtonGrid();
        ApplyAllSettings();
    }, nullptr);
}
