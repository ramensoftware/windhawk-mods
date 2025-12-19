// ==WindhawkMod==
// @id              virtual-desktop-helper
// @name            Virtual Desktop Helper
// @description     Switch virtual desktops, move windows between desktops, pin windows, and tile windows with customizable hotkeys
// @version         2.0.0
// @author          u2x1
// @github          https://github.com/u2x1
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -luuid -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Virtual Desktop Helper

A comprehensive virtual desktop management tool with window tiling support for Windows 10/11.

Based on VD.ahk by FuPeiJiang.

## Features

### Virtual Desktop Management
- **Quick Switch**: Jump directly to any desktop (1-9) with a single hotkey
- **Move Windows**: Send the active window to any desktop instantly
- **Previous Desktop**: Toggle back to the last visited desktop
- **Pin Windows**: Pin/unpin windows to appear on all desktops

### Window Tiling
- **Manual Tiling**: Tile all windows on the current monitor with one hotkey
- **Multiple Layouts**: 6 different tiling layouts to choose from
- **Customizable Gaps**: Adjust margins and gaps between windows

## Default Hotkeys

| Action | Default Hotkey |
|--------|----------------|
| Switch to desktop 1-9 | `Alt + 1-9` |
| Move window to desktop 1-9 | `Alt + Shift + 1-9` |
| Toggle previous desktop | `Alt + Q` (configurable modifier) |
| Pin/unpin window | `Alt + P` (configurable modifier) |
| Tile windows | `Alt + D` (configurable modifier) |
| Cycle layout | `Alt + L` (configurable modifier) |

Note: The modifier for utility hotkeys (Previous Desktop, Pin, Tile, Layout) can be changed in settings.

## Tiling Layouts

1. **Master + Stack (Vertical)**: One large master window on the left, others stacked on the right
2. **Master + Stack (Horizontal)**: One large master window on top, others in a row below
3. **Columns**: All windows in equal vertical columns
4. **Rows**: All windows in equal horizontal rows
5. **BSP (Binary Space Partition)**: Recursive binary split layout
6. **Monocle**: All windows fullscreen (stacked)

## Windows Version Support

Select your Windows version in settings for correct functionality:
- Windows 10 (Build < 20348)
- Windows 10 (Build 20348 - 21999)
- Windows 11 (Build 22000 - 22482)
- Windows 11 (Build 22621/22631/23H2)
- Windows 11 (Build 26100+ / 24H2)

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- WindowsVersion: win11_26100
  $name: Windows Version
  $description: Select your Windows version for correct COM interface IDs. Run "winver" to check your build number.
  $options:
    - win10_old: Windows 10 (Build < 20348)
    - win10_20348: Windows 10 (Build 20348 - 21999)
    - win11_22000: Windows 11 (Build 22000 - 22482)
    - win11_22621: Windows 11 (Build 22621/22631/23H2)
    - win11_26100: Windows 11 (Build 26100+ / 24H2)

- SwitchDesktopModifier: alt
  $name: Switch Desktop Modifier
  $description: Modifier keys for switching to desktop (combined with number keys 1-9)
  $options:
    - alt: Alt
    - ctrl: Ctrl
    - alt+shift: Alt + Shift
    - ctrl+alt: Ctrl + Alt
    - ctrl+shift: Ctrl + Shift

- MoveWindowModifier: alt+shift
  $name: Move Window Modifier
  $description: Modifier keys for moving active window to desktop (combined with number keys 1-9)
  $options:
    - alt+shift: Alt + Shift
    - ctrl+alt: Ctrl + Alt
    - ctrl+shift: Ctrl + Shift

- FollowMovedWindow: false
  $name: Follow Moved Window
  $description: Automatically switch to the target desktop after moving a window

- MaxDesktops: 9
  $name: Maximum Desktops
  $description: Number of desktops to register hotkeys for (1-9). Set lower if you use fewer desktops to avoid hotkey conflicts.

- PrevDesktopKey: Q
  $name: Previous Desktop Key
  $description: Key to toggle back to the previous desktop (used with Utility Modifier)
  $options:
    - Q: Q
    - Grave: "` (Backtick)"
    - Tab: Tab

- PinKey: P
  $name: Pin Window Key
  $description: Key to pin/unpin the active window to all desktops (used with Utility Modifier)
  $options:
    - P: P
    - W: W

- DefaultLayout: master_stack
  $name: Default Tiling Layout
  $description: The initial tiling layout when the mod starts
  $options:
    - master_stack: Master + Stack (Vertical)
    - master_stack_h: Master + Stack (Horizontal)
    - columns: Columns
    - rows: Rows
    - bsp: BSP (Binary Space Partition)
    - monocle: Monocle (Fullscreen)

- TileKey: D
  $name: Tile Key
  $description: Key to tile all windows on the current monitor (used with Utility Modifier)
  $options:
    - D: D
    - T: T
    - Space: Space

- LayoutKey: L
  $name: Layout Cycle Key
  $description: Key to cycle through tiling layouts (used with Utility Modifier)
  $options:
    - L: L
    - Tab: Tab

- UtilityModifier: alt
  $name: Utility Hotkey Modifier
  $description: Modifier keys for utility hotkeys (Previous Desktop, Pin, Tile, Layout)
  $options:
    - alt: Alt
    - ctrl: Ctrl
    - alt+shift: Alt + Shift
    - ctrl+alt: Ctrl + Alt
    - ctrl+shift: Ctrl + Shift

- TileMargin: 4
  $name: Tile Margin (pixels)
  $description: Gap between tiled windows and screen edges (0-100)

- TileGap: 4
  $name: Tile Gap (pixels)
  $description: Gap between adjacent tiled windows (0-100)

- MasterPercent: 50
  $name: Master Window Size (%)
  $description: Size percentage of the master window in Master+Stack layouts (1-99)
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>
#include <initguid.h>
#include <objbase.h>
#include <objectarray.h>
#include <shobjidl.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <unordered_map>
#include <vector>

#define SAFE_RELEASE(p) \
  do {                  \
    if (p) {            \
      (p)->Release();   \
      (p) = nullptr;    \
    }                   \
  } while (0)

//=============================================================================
// COM CLSIDs and IIDs for Virtual Desktop API (undocumented)
// These identifiers are reverse-engineered from Windows shell components.
// Reference: https://github.com/Ciantic/VirtualDesktopAccessor
//=============================================================================

// CLSID_ImmersiveShell: {C2F03A33-21F5-47FA-B4BB-156362A2F239}
// The main shell service provider for accessing virtual desktop interfaces
static const CLSID CLSID_ImmersiveShell = {
    0xC2F03A33, 0x21F5, 0x47FA, {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39}};

// CLSID_VirtualDesktopManagerInternal: {C5E0CDCA-7B6E-41B2-9FC4-D93975CC467B}
// Internal manager for virtual desktop operations (switch, move, etc.)
static const CLSID CLSID_VirtualDesktopManagerInternal = {
    0xC5E0CDCA, 0x7B6E, 0x41B2, {0x9F, 0xC4, 0xD9, 0x39, 0x75, 0xCC, 0x46, 0x7B}};

// CLSID_VirtualDesktopPinnedApps: {B5A399E7-1C87-46B8-88E9-FC5747B171BD}
// Service for pinning windows/apps to all virtual desktops
static const CLSID CLSID_VirtualDesktopPinnedApps = {
    0xB5A399E7, 0x1C87, 0x46B8, {0x88, 0xE9, 0xFC, 0x57, 0x47, 0xB1, 0x71, 0xBD}};

// IID_IApplicationViewCollection: {1841C6D7-4F9D-42C0-AF41-8747538F10E5}
// Interface for accessing application views (windows) by HWND
static const IID IID_IApplicationViewCollection = {
    0x1841C6D7, 0x4F9D, 0x42C0, {0xAF, 0x41, 0x87, 0x47, 0x53, 0x8F, 0x10, 0xE5}};

// IID_IVirtualDesktopPinnedApps: {4CE81583-1E4C-4632-A621-07A53543148F}
// Interface for pin/unpin operations
static const IID IID_IVirtualDesktopPinnedApps = {
    0x4CE81583, 0x1E4C, 0x4632, {0xA6, 0x21, 0x07, 0xA5, 0x35, 0x43, 0x14, 0x8F}};

//=============================================================================
// Windows Version-Specific Interface IDs
// Microsoft changes these IIDs between Windows builds, requiring version detection.
// usesHMonitor: Some versions require HMONITOR parameter for multi-monitor support.
//=============================================================================
struct VersionIIDs {
  IID managerInternal;  // IID for IVirtualDesktopManagerInternal
  IID virtualDesktop;   // IID for IVirtualDesktop
  bool usesHMonitor;    // Whether API methods require HMONITOR parameter
};

static const VersionIIDs g_versionIIDs[] = {
    // [0] Windows 10 (Build < 20348)
    // IVirtualDesktopManagerInternal: {F31574D6-B682-4CDC-BD56-1827860ABEC6}
    // IVirtualDesktop: {FF72FFDD-BE7E-43FC-9C03-AD81681E88E4}
    {{0xF31574D6, 0xB682, 0x4CDC, {0xBD, 0x56, 0x18, 0x27, 0x86, 0x0A, 0xBE, 0xC6}},
     {0xFF72FFDD, 0xBE7E, 0x43FC, {0x9C, 0x03, 0xAD, 0x81, 0x68, 0x1E, 0x88, 0xE4}},
     false},

    // [1] Windows 10 (Build 20348 - 21999)
    // IVirtualDesktopManagerInternal: {094AFE11-44F2-4BA0-976F-29A97E263EE0}
    // IVirtualDesktop: {62FDF88B-11CA-4AFB-8BD8-2296DFAE49E2}
    {{0x094AFE11, 0x44F2, 0x4BA0, {0x97, 0x6F, 0x29, 0xA9, 0x7E, 0x26, 0x3E, 0xE0}},
     {0x62FDF88B, 0x11CA, 0x4AFB, {0x8B, 0xD8, 0x22, 0x96, 0xDF, 0xAE, 0x49, 0xE2}},
     true},

    // [2] Windows 11 (Build 22000 - 22482)
    // IVirtualDesktopManagerInternal: {B2F925B9-5A0F-4D2E-9F4D-2B1507593C10}
    // IVirtualDesktop: {536D3495-B208-4CC9-AE26-DE8111275BF8}
    {{0xB2F925B9, 0x5A0F, 0x4D2E, {0x9F, 0x4D, 0x2B, 0x15, 0x07, 0x59, 0x3C, 0x10}},
     {0x536D3495, 0xB208, 0x4CC9, {0xAE, 0x26, 0xDE, 0x81, 0x11, 0x27, 0x5B, 0xF8}},
     true},

    // [3] Windows 11 (Build 22621/22631/23H2)
    // IVirtualDesktopManagerInternal: {A3175F2D-239C-4BD2-8AA0-EEBA8B0B138E}
    // IVirtualDesktop: {3F07F4BE-B107-441A-AF0F-39D82529072C}
    {{0xA3175F2D, 0x239C, 0x4BD2, {0x8A, 0xA0, 0xEE, 0xBA, 0x8B, 0x0B, 0x13, 0x8E}},
     {0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}},
     false},

    // [4] Windows 11 (Build 26100+ / 24H2)
    // IVirtualDesktopManagerInternal: {53F5CA0B-158F-4124-900C-057158060B27}
    // IVirtualDesktop: {3F07F4BE-B107-441A-AF0F-39D82529072C} (same as 22621)
    {{0x53F5CA0B, 0x158F, 0x4124, {0x90, 0x0C, 0x05, 0x71, 0x58, 0x06, 0x0B, 0x27}},
     {0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}},
     false},
};

// Default to Windows 11 24H2 (index 4)
static int g_windowsVersionIndex = 4;

//=============================================================================
// COM Interface Definitions (undocumented, reverse-engineered)
// Note: VTable layouts vary by Windows version; IIDs selected at runtime
//=============================================================================

struct IVirtualDesktop : public IUnknown {
  virtual HRESULT STDMETHODCALLTYPE IsViewVisible(IUnknown*, BOOL*) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetId(GUID*) = 0;
};
struct IApplicationView : public IUnknown {};

MIDL_INTERFACE("1841C6D7-4F9D-42C0-AF41-8747538F10E5")
IApplicationViewCollection : public IUnknown {
  virtual HRESULT STDMETHODCALLTYPE GetViews(IObjectArray**) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetViewsByZOrder(IObjectArray**) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetViewsByAppUserModelId(LPCWSTR, IObjectArray**) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetViewForHwnd(HWND, IApplicationView**) = 0;
};

struct IVirtualDesktopManagerInternal : public IUnknown {};

MIDL_INTERFACE("4CE81583-1E4C-4632-A621-07A53543148F")
IVirtualDesktopPinnedApps : public IUnknown {
  virtual HRESULT STDMETHODCALLTYPE IsAppIdPinned(LPCWSTR, BOOL*) = 0;
  virtual HRESULT STDMETHODCALLTYPE PinAppID(LPCWSTR) = 0;
  virtual HRESULT STDMETHODCALLTYPE UnpinAppID(LPCWSTR) = 0;
  virtual HRESULT STDMETHODCALLTYPE IsViewPinned(IApplicationView*, BOOL*) = 0;
  virtual HRESULT STDMETHODCALLTYPE PinView(IApplicationView*) = 0;
  virtual HRESULT STDMETHODCALLTYPE UnpinView(IApplicationView*) = 0;
};

//=============================================================================
// Global State
//=============================================================================
static IServiceProvider* g_pServiceProvider = nullptr;
static IVirtualDesktopManagerInternal* g_pDesktopManagerInternal = nullptr;
static IApplicationViewCollection* g_pViewCollection = nullptr;
static IVirtualDesktopManager* g_pDesktopManager = nullptr;
static IVirtualDesktopPinnedApps* g_pPinnedApps = nullptr;
static bool g_bInitialized = false;

static HANDLE g_hThread = nullptr;
static DWORD g_threadId = 0;
static HANDLE g_hReadyEvent = nullptr;
static volatile bool g_stopHotkeyThread = false;

static UINT g_moveModifiers = MOD_ALT | MOD_SHIFT;
static UINT g_switchModifiers = MOD_ALT;
static UINT g_utilityModifiers = MOD_ALT;
static bool g_followMovedWindow = false;
static int g_maxDesktops = 9;

static GUID g_previousDesktopId = {};
static bool g_hasPreviousDesktop = false;

// Hotkey ID ranges:
// HK_MOVE_BASE (1-9): Move window to desktop 1-9
// HK_SWITCH_BASE (10-18): Switch to desktop 1-9
// HK_PREV (19): Toggle to previous desktop
// HK_TILE (20): Tile windows on current monitor
// HK_LAYOUT (21): Cycle through tiling layouts
// HK_PIN (22): Pin/unpin current window
enum HotkeyIds { HK_MOVE_BASE = 1, HK_SWITCH_BASE = 10, HK_PREV = 19, HK_TILE = 20, HK_LAYOUT = 21, HK_PIN = 22 };

static LONG g_tileMargin = 4;
static LONG g_tileGap = 4;
static LONG g_masterPercent = 50;

enum class TileLayout { MasterStack, Columns, Rows, MasterStackH, BSP, Monocle, COUNT };
static TileLayout g_currentLayout = TileLayout::MasterStack;

static UINT g_prevDesktopKey = 'Q';
static UINT g_tileKey = 'D';
static UINT g_layoutKey = 'L';
static UINT g_pinKey = 'P';

// Per-desktop state: tracks last focused window and layout for each virtual desktop
// Hash function for GUID to use in unordered_map
// Combines 4 uint32_t values using a standard hash combining technique (golden ratio based)
struct GuidHash {
  size_t operator()(const GUID& guid) const {
    const uint32_t* data = reinterpret_cast<const uint32_t*>(&guid);
    size_t hash = 0;
    for (int i = 0; i < 4; ++i) {  // GUID is 16 bytes = 4 x uint32_t
      // 0x9e3779b9 is the golden ratio constant for hash combining
      hash ^= std::hash<uint32_t>{}(data[i]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
  }
};

struct GuidEqual {
  bool operator()(const GUID& a, const GUID& b) const { return IsEqualGUID(a, b); }
};

static std::unordered_map<GUID, HWND, GuidHash, GuidEqual> g_desktopFocusMap;
static std::unordered_map<GUID, TileLayout, GuidHash, GuidEqual> g_desktopLayoutMap;

//=============================================================================
// Helper Functions
//=============================================================================

// Access VTable function by index for undocumented COM interfaces
template <typename T>
T GetVTableFunction(void* pInterface, int index) {
  return reinterpret_cast<T>((*reinterpret_cast<void***>(pInterface))[index]);
}

bool UsesHMonitorParameter() { return g_versionIIDs[g_windowsVersionIndex].usesHMonitor; }

UINT ParseModifiers(PCWSTR str) {
  UINT modifiers = 0;
  if (wcsstr(str, L"alt")) modifiers |= MOD_ALT;
  if (wcsstr(str, L"ctrl")) modifiers |= MOD_CONTROL;
  if (wcsstr(str, L"shift")) modifiers |= MOD_SHIFT;
  return modifiers;
}

template <typename T>
T LookupTable(PCWSTR str, const std::pair<PCWSTR, T>* table, size_t count, T defaultVal) {
  for (size_t i = 0; i < count; ++i) {
    if (wcscmp(str, table[i].first) == 0) return table[i].second;
  }
  return defaultVal;
}

UINT ParseKeyCode(PCWSTR str) {
  static const std::pair<PCWSTR, UINT> kKeyMap[] = {{L"Q", 'Q'},          {L"D", 'D'},      {L"T", 'T'},
                                                    {L"L", 'L'},          {L"P", 'P'},      {L"W", 'W'},
                                                    {L"Grave", VK_OEM_3}, {L"Tab", VK_TAB}, {L"Space", VK_SPACE}};
  return LookupTable(str, kKeyMap, _countof(kKeyMap), (UINT)0);
}

TileLayout ParseLayoutSetting(PCWSTR str) {
  static const std::pair<PCWSTR, TileLayout> kLayoutMap[] = {{L"master_stack", TileLayout::MasterStack},
                                                             {L"master_stack_h", TileLayout::MasterStackH},
                                                             {L"columns", TileLayout::Columns},
                                                             {L"rows", TileLayout::Rows},
                                                             {L"bsp", TileLayout::BSP},
                                                             {L"monocle", TileLayout::Monocle}};
  return LookupTable(str, kLayoutMap, _countof(kLayoutMap), TileLayout::MasterStack);
}

int ParseWindowsVersion(PCWSTR str) {
  static const std::pair<PCWSTR, int> kVersionMap[] = {
      {L"win10_old", 0}, {L"win10_20348", 1}, {L"win11_22000", 2}, {L"win11_22621", 3}, {L"win11_26100", 4}};
  return LookupTable(str, kVersionMap, _countof(kVersionMap), 4);
}

template <typename T, typename Parser>
T ReadStringSetting(PCWSTR name, Parser parser, T defaultVal) {
  PCWSTR str = Wh_GetStringSetting(name);
  T result = parser(str);
  Wh_FreeStringSetting(str);
  return result ? result : defaultVal;
}

UINT ReadModifierSetting(PCWSTR name, UINT defaultVal) {
  PCWSTR str = Wh_GetStringSetting(name);
  UINT result = ParseModifiers(str);
  Wh_FreeStringSetting(str);
  return result ? result : defaultVal;
}

void LoadSettings() {
  // Windows version
  PCWSTR version = Wh_GetStringSetting(L"WindowsVersion");
  g_windowsVersionIndex = ParseWindowsVersion(version);
  Wh_FreeStringSetting(version);

  // Modifier keys
  g_moveModifiers = ReadModifierSetting(L"MoveWindowModifier", MOD_ALT | MOD_SHIFT);
  g_switchModifiers = ReadModifierSetting(L"SwitchDesktopModifier", MOD_ALT);
  g_utilityModifiers = ReadModifierSetting(L"UtilityModifier", MOD_ALT);

  g_followMovedWindow = Wh_GetIntSetting(L"FollowMovedWindow") != 0;
  g_maxDesktops = Wh_GetIntSetting(L"MaxDesktops");
  if (g_maxDesktops < 1 || g_maxDesktops > 9) g_maxDesktops = 9;

  // Tiling settings
  g_tileMargin = Wh_GetIntSetting(L"TileMargin");
  if (g_tileMargin < 0 || g_tileMargin > 100) g_tileMargin = 4;
  g_tileGap = Wh_GetIntSetting(L"TileGap");
  if (g_tileGap < 0 || g_tileGap > 100) g_tileGap = 4;
  g_masterPercent = Wh_GetIntSetting(L"MasterPercent");
  if (g_masterPercent < 1 || g_masterPercent > 99) g_masterPercent = 50;

  // Layout and hotkey settings
  PCWSTR layout = Wh_GetStringSetting(L"DefaultLayout");
  g_currentLayout = ParseLayoutSetting(layout);
  Wh_FreeStringSetting(layout);

  g_prevDesktopKey = ReadStringSetting(L"PrevDesktopKey", ParseKeyCode, (UINT)'Q');
  g_tileKey = ReadStringSetting(L"TileKey", ParseKeyCode, (UINT)'D');
  g_layoutKey = ReadStringSetting(L"LayoutKey", ParseKeyCode, (UINT)'L');
  g_pinKey = ReadStringSetting(L"PinKey", ParseKeyCode, (UINT)'P');
}

//=============================================================================
// Virtual Desktop API Initialization
//=============================================================================

static const int API_INIT_MAX_RETRIES = 30;
static const int API_INIT_RETRY_DELAY_MS = 1000;

bool InitializeVirtualDesktopAPIOnce() {
  HRESULT hr = CoCreateInstance(CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER, IID_IServiceProvider,
                                (void**)&g_pServiceProvider);
  if (FAILED(hr) || !g_pServiceProvider) {
    Wh_Log(L"Failed to create ImmersiveShell: 0x%08X", hr);
    return false;
  }

  hr = g_pServiceProvider->QueryService(CLSID_VirtualDesktopManagerInternal,
                                        g_versionIIDs[g_windowsVersionIndex].managerInternal,
                                        (void**)&g_pDesktopManagerInternal);
  if (FAILED(hr) || !g_pDesktopManagerInternal) {
    Wh_Log(L"Failed to get VirtualDesktopManagerInternal: 0x%08X", hr);
    SAFE_RELEASE(g_pServiceProvider);
    return false;
  }

  hr = g_pServiceProvider->QueryService(IID_IApplicationViewCollection, IID_IApplicationViewCollection,
                                        (void**)&g_pViewCollection);
  if (FAILED(hr) || !g_pViewCollection) {
    Wh_Log(L"Failed to get ApplicationViewCollection: 0x%08X", hr);
    SAFE_RELEASE(g_pDesktopManagerInternal);
    SAFE_RELEASE(g_pServiceProvider);
    return false;
  }

  hr = CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_INPROC_SERVER, IID_IVirtualDesktopManager,
                        (void**)&g_pDesktopManager);
  if (FAILED(hr) || !g_pDesktopManager) {
    Wh_Log(L"Failed to create VirtualDesktopManager: 0x%08X", hr);
    SAFE_RELEASE(g_pViewCollection);
    SAFE_RELEASE(g_pDesktopManagerInternal);
    SAFE_RELEASE(g_pServiceProvider);
    return false;
  }

  // PinnedApps is optional - don't fail if unavailable
  hr = g_pServiceProvider->QueryService(CLSID_VirtualDesktopPinnedApps, IID_IVirtualDesktopPinnedApps,
                                        (void**)&g_pPinnedApps);
  if (FAILED(hr)) {
    Wh_Log(L"PinnedApps service not available (pin feature disabled): 0x%08X", hr);
    g_pPinnedApps = nullptr;
  }

  return true;
}

bool InitializeVirtualDesktopAPI() {
  if (g_bInitialized) return true;

  if (InitializeVirtualDesktopAPIOnce()) {
    g_bInitialized = true;
    return true;
  }
  return false;
}

void CleanupVirtualDesktopAPI() {
  SAFE_RELEASE(g_pPinnedApps);
  SAFE_RELEASE(g_pDesktopManager);
  SAFE_RELEASE(g_pViewCollection);
  SAFE_RELEASE(g_pDesktopManagerInternal);
  SAFE_RELEASE(g_pServiceProvider);
  g_bInitialized = false;
}

// Unified VTable call for methods with optional HMONITOR parameter (varies by Windows version)
template <typename TResult>
HRESULT CallManagerInternal(int vtableIndex, TResult* outResult) {
  if (UsesHMonitorParameter()) {
    auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HMONITOR, TResult*)>(g_pDesktopManagerInternal,
                                                                                         vtableIndex);
    return pfn(g_pDesktopManagerInternal, nullptr, outResult);
  } else {
    auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, TResult*)>(g_pDesktopManagerInternal, vtableIndex);
    return pfn(g_pDesktopManagerInternal, outResult);
  }
}

template <typename TArg>
HRESULT CallManagerInternalWithArg(int vtableIndex, TArg arg) {
  if (UsesHMonitorParameter()) {
    auto pfn =
        GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HMONITOR, TArg)>(g_pDesktopManagerInternal, vtableIndex);
    return pfn(g_pDesktopManagerInternal, nullptr, arg);
  } else {
    auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, TArg)>(g_pDesktopManagerInternal, vtableIndex);
    return pfn(g_pDesktopManagerInternal, arg);
  }
}

//=============================================================================
// Virtual Desktop Operations
// IVirtualDesktopManagerInternal VTable indices (may vary slightly by version):
//   4 = MoveViewToDesktop, 6 = GetCurrentDesktop, 7 = GetDesktops, 9 = SwitchDesktop
//=============================================================================

static const int VTABLE_MOVE_VIEW_TO_DESKTOP = 4;
static const int VTABLE_GET_CURRENT_DESKTOP = 6;
static const int VTABLE_GET_DESKTOPS = 7;
static const int VTABLE_SWITCH_DESKTOP = 9;

IObjectArray* GetDesktops() {
  if (!g_pDesktopManagerInternal) return nullptr;
  IObjectArray* desktops = nullptr;
  HRESULT hr = CallManagerInternal(VTABLE_GET_DESKTOPS, &desktops);
  if (FAILED(hr)) Wh_Log(L"GetDesktops failed: 0x%08X", hr);
  return desktops;
}

IVirtualDesktop* GetDesktopByIndex(int index) {
  IObjectArray* desktops = GetDesktops();
  if (!desktops) return nullptr;

  UINT count = 0;
  desktops->GetCount(&count);

  IVirtualDesktop* desktop = nullptr;
  if (index >= 0 && (UINT)index < count) {
    desktops->GetAt(index, g_versionIIDs[g_windowsVersionIndex].virtualDesktop, (void**)&desktop);
  }
  desktops->Release();
  return desktop;
}

bool GetCurrentDesktopId(GUID* outGuid) {
  if (!g_pDesktopManagerInternal) return false;

  IVirtualDesktop* desktop = nullptr;
  HRESULT hr = CallManagerInternal(VTABLE_GET_CURRENT_DESKTOP, &desktop);
  if (FAILED(hr) || !desktop) {
    Wh_Log(L"GetCurrentDesktop failed: 0x%08X", hr);
    return false;
  }

  hr = desktop->GetId(outGuid);
  desktop->Release();
  return SUCCEEDED(hr);
}

int GetDesktopIndexById(const GUID& desktopId) {
  IObjectArray* desktops = GetDesktops();
  if (!desktops) return -1;

  UINT count = 0;
  desktops->GetCount(&count);

  for (UINT i = 0; i < count; ++i) {
    IVirtualDesktop* desktop = nullptr;
    if (SUCCEEDED(desktops->GetAt(i, g_versionIIDs[g_windowsVersionIndex].virtualDesktop, (void**)&desktop)) &&
        desktop) {
      GUID guid;
      bool match = SUCCEEDED(desktop->GetId(&guid)) && IsEqualGUID(guid, desktopId);
      desktop->Release();
      if (match) {
        desktops->Release();
        return i;
      }
    }
  }
  desktops->Release();
  return -1;
}

bool SwitchToDesktop(IVirtualDesktop* desktop) {
  if (!g_pDesktopManagerInternal || !desktop) return false;

  HRESULT hr = CallManagerInternalWithArg(VTABLE_SWITCH_DESKTOP, desktop);
  if (FAILED(hr)) Wh_Log(L"SwitchToDesktop failed: 0x%08X", hr);
  return SUCCEEDED(hr);
}

bool MoveViewToDesktop(IApplicationView* view, IVirtualDesktop* desktop) {
  if (!g_pDesktopManagerInternal || !view || !desktop) return false;

  auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, IApplicationView*, IVirtualDesktop*)>(
      g_pDesktopManagerInternal, VTABLE_MOVE_VIEW_TO_DESKTOP);
  HRESULT hr = pfn(g_pDesktopManagerInternal, view, desktop);

  if (FAILED(hr)) Wh_Log(L"MoveViewToDesktop failed: 0x%08X", hr);
  return SUCCEEDED(hr);
}

// Check if a window is eligible for virtual desktop operations
bool IsEligibleWindow(HWND hwnd) {
  if (!hwnd || !IsWindow(hwnd)) return false;
  LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
  if (!(style & WS_VISIBLE) || (style & WS_CHILD)) return false;
  if (GetWindowLongPtr(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) return false;
  return GetAncestor(hwnd, GA_ROOTOWNER) == hwnd;
}

// Find the topmost window on a specific virtual desktop
HWND FindWindowOnDesktop(const GUID& desktopId, HWND excludeWindow = nullptr) {
  struct EnumContext {
    GUID targetId;
    HWND excludeHwnd;
    HWND resultHwnd;
  } context = {desktopId, excludeWindow, nullptr};

  EnumWindows(
      [](HWND hwnd, LPARAM lParam) WINAPI -> BOOL {
        auto* ctx = reinterpret_cast<EnumContext*>(lParam);
        if (hwnd == ctx->excludeHwnd || !IsEligibleWindow(hwnd)) return TRUE;

        GUID windowDesktopId;
        if (g_pDesktopManager && SUCCEEDED(g_pDesktopManager->GetWindowDesktopId(hwnd, &windowDesktopId))) {
          if (IsEqualGUID(windowDesktopId, ctx->targetId)) {
            ctx->resultHwnd = hwnd;
            return FALSE;
          }
        }
        return TRUE;
      },
      reinterpret_cast<LPARAM>(&context));

  return context.resultHwnd;
}

void FocusWindow(HWND hwnd) {
  if (!hwnd) return;
  if (IsIconic(hwnd)) ShowWindow(hwnd, SW_RESTORE);
  SetForegroundWindow(hwnd);
}

bool GoToDesktopNum(int desktopNum) {
  if (!InitializeVirtualDesktopAPI() || desktopNum <= 0) return false;

  // Save current desktop info for "previous desktop" feature
  GUID currentDesktopId = {};
  bool hasCurrentDesktop = GetCurrentDesktopId(&currentDesktopId);
  HWND currentForeground = GetForegroundWindow();

  if (hasCurrentDesktop && currentForeground && IsEligibleWindow(currentForeground)) {
    g_desktopFocusMap[currentDesktopId] = currentForeground;
  }

  IVirtualDesktop* targetDesktop = GetDesktopByIndex(desktopNum - 1);
  if (!targetDesktop) {
    Wh_Log(L"Desktop %d not found", desktopNum);
    return false;
  }

  GUID targetDesktopId = {};
  bool hasTargetId = SUCCEEDED(targetDesktop->GetId(&targetDesktopId));

  // Already on target desktop
  if (hasTargetId && hasCurrentDesktop && IsEqualGUID(currentDesktopId, targetDesktopId)) {
    targetDesktop->Release();
    return true;
  }

  // Find window to focus on target desktop
  HWND windowToFocus = nullptr;
  if (hasTargetId) {
    auto it = g_desktopFocusMap.find(targetDesktopId);
    if (it != g_desktopFocusMap.end() && IsEligibleWindow(it->second)) {
      GUID windowDesktopId;
      if (g_pDesktopManager && SUCCEEDED(g_pDesktopManager->GetWindowDesktopId(it->second, &windowDesktopId))) {
        if (IsEqualGUID(windowDesktopId, targetDesktopId)) {
          windowToFocus = it->second;
        }
      }
    }
    if (!windowToFocus) {
      windowToFocus = FindWindowOnDesktop(targetDesktopId);
    }
  }

  bool success = SwitchToDesktop(targetDesktop);
  targetDesktop->Release();

  if (success) {
    if (windowToFocus) {
      FocusWindow(windowToFocus);
      if (hasTargetId) {
        g_desktopFocusMap[targetDesktopId] = windowToFocus;
      }
    }
    // Update previous desktop for toggle feature
    if (hasCurrentDesktop && hasTargetId && !IsEqualGUID(currentDesktopId, targetDesktopId)) {
      g_previousDesktopId = currentDesktopId;
      g_hasPreviousDesktop = true;
    }
    Wh_Log(L"Switched to desktop %d", desktopNum);
  }
  return success;
}

bool SwitchToPreviousDesktop() {
  if (!g_hasPreviousDesktop) return false;

  GUID currentId;
  if (!GetCurrentDesktopId(&currentId) || IsEqualGUID(currentId, g_previousDesktopId)) {
    return true;
  }

  int index = GetDesktopIndexById(g_previousDesktopId);
  if (index < 0) {
    g_hasPreviousDesktop = false;
    Wh_Log(L"Previous desktop no longer exists");
    return false;
  }
  return GoToDesktopNum(index + 1);
}

bool MoveActiveWindowToDesktopNum(int desktopNum) {
  if (!InitializeVirtualDesktopAPI()) return false;

  HWND hwnd = GetForegroundWindow();
  if (!hwnd) {
    Wh_Log(L"No active window to move");
    return false;
  }

  IVirtualDesktop* targetDesktop = GetDesktopByIndex(desktopNum - 1);
  if (!targetDesktop) {
    Wh_Log(L"Target desktop %d not found", desktopNum);
    return false;
  }

  IApplicationView* view = nullptr;
  g_pViewCollection->GetViewForHwnd(hwnd, &view);

  bool success = view && MoveViewToDesktop(view, targetDesktop);

  SAFE_RELEASE(view);
  targetDesktop->Release();

  if (success) {
    Wh_Log(L"Moved window to desktop %d", desktopNum);
  }
  return success;
}

bool TogglePinWindow() {
  if (!InitializeVirtualDesktopAPI() || !g_pPinnedApps) {
    Wh_Log(L"Pin feature not available");
    return false;
  }

  HWND hwnd = GetForegroundWindow();
  if (!hwnd) {
    Wh_Log(L"No active window to pin/unpin");
    return false;
  }

  IApplicationView* view = nullptr;
  if (FAILED(g_pViewCollection->GetViewForHwnd(hwnd, &view)) || !view) {
    Wh_Log(L"Failed to get view for window");
    return false;
  }

  BOOL isPinned = FALSE;
  g_pPinnedApps->IsViewPinned(view, &isPinned);

  HRESULT hr = isPinned ? g_pPinnedApps->UnpinView(view) : g_pPinnedApps->PinView(view);
  view->Release();

  if (SUCCEEDED(hr)) {
    Wh_Log(L"Window %s", isPinned ? L"unpinned" : L"pinned");
  } else {
    Wh_Log(L"Failed to %s window: 0x%08X", isPinned ? L"unpin" : L"pin", hr);
  }
  return SUCCEEDED(hr);
}

//=============================================================================
// Window Tiling
//=============================================================================

static const wchar_t* kIgnoredWindowClasses[] = {L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd",
                                                 L"Windows.UI.Core.CoreWindow"};

bool IsWindowCloaked(HWND hwnd) {
  BOOL cloaked = FALSE;
  DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
  return cloaked;
}

// Check if a window should be included in tiling
bool IsTileEligible(HWND hwnd, HMONITOR targetMonitor) {
  if (!IsWindowVisible(hwnd) || IsIconic(hwnd) || !IsWindowEnabled(hwnd)) return false;
  if (GetAncestor(hwnd, GA_ROOT) != hwnd || GetWindow(hwnd, GW_OWNER)) return false;

  LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
  if ((style & WS_CHILD) || !(style & WS_SIZEBOX)) return false;

  LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
  if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) return false;

  if (IsWindowCloaked(hwnd)) return false;

  wchar_t className[64];
  if (GetClassNameW(hwnd, className, 64)) {
    for (const auto* ignoredClass : kIgnoredWindowClasses) {
      if (_wcsicmp(className, ignoredClass) == 0) return false;
    }
  }

  RECT frameRect;
  if (!SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &frameRect, sizeof(frameRect)))) {
    if (!GetWindowRect(hwnd, &frameRect)) return false;
  }

  return frameRect.right > frameRect.left && frameRect.bottom > frameRect.top &&
         MonitorFromRect(&frameRect, MONITOR_DEFAULTTONULL) == targetMonitor;
}

// Position a window, compensating for invisible DWM frame borders
void PlaceWindow(HWND hwnd, const RECT& targetRect) {
  if (IsZoomed(hwnd)) ShowWindow(hwnd, SW_RESTORE);

  // Compensate for invisible window borders (DWM frame)
  RECT windowRect, extendedFrame;
  LONG offsetLeft = 0, offsetTop = 0, offsetRight = 0, offsetBottom = 0;

  if (GetWindowRect(hwnd, &windowRect) &&
      SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &extendedFrame, sizeof(extendedFrame)))) {
    offsetLeft = extendedFrame.left - windowRect.left;
    offsetTop = extendedFrame.top - windowRect.top;
    offsetRight = windowRect.right - extendedFrame.right;
    offsetBottom = windowRect.bottom - extendedFrame.bottom;
  }

  SetWindowPos(hwnd, nullptr, targetRect.left - offsetLeft, targetRect.top - offsetTop,
               targetRect.right - targetRect.left + offsetLeft + offsetRight,
               targetRect.bottom - targetRect.top + offsetTop + offsetBottom,
               SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING | SWP_ASYNCWINDOWPOS);
}

//=============================================================================
// Tiling Layout Algorithms
//=============================================================================

// Master window on one side, remaining windows stacked on the other
void LayoutMasterStack(const RECT& area, size_t windowCount, std::vector<RECT>& outRects, bool horizontal) {
  outRects.resize(windowCount);
  if (windowCount == 0) return;
  if (windowCount == 1) {
    outRects[0] = area;
    return;
  }

  LONG totalSize = horizontal ? (area.bottom - area.top) : (area.right - area.left);
  LONG masterSize = (totalSize - g_tileGap) * g_masterPercent / 100;

  if (horizontal) {
    // Master on top, stack below in a row
    outRects[0] = {area.left, area.top, area.right, area.top + masterSize};
    LONG stackTop = area.top + masterSize + g_tileGap;
    LONG stackWidth = (area.right - area.left - (LONG)(windowCount - 2) * g_tileGap) / (LONG)(windowCount - 1);
    LONG x = area.left;
    for (size_t i = 1; i < windowCount; ++i) {
      LONG right = (i == windowCount - 1) ? area.right : x + stackWidth;
      outRects[i] = {x, stackTop, right, area.bottom};
      x = right + g_tileGap;
    }
  } else {
    // Master on left, stack on right
    outRects[0] = {area.left, area.top, area.left + masterSize, area.bottom};
    LONG stackLeft = area.left + masterSize + g_tileGap;
    LONG stackHeight = (area.bottom - area.top - (LONG)(windowCount - 2) * g_tileGap) / (LONG)(windowCount - 1);
    LONG y = area.top;
    for (size_t i = 1; i < windowCount; ++i) {
      LONG bottom = (i == windowCount - 1) ? area.bottom : y + stackHeight;
      outRects[i] = {stackLeft, y, area.right, bottom};
      y = bottom + g_tileGap;
    }
  }
}

// Equal-sized columns or rows
void LayoutGrid(const RECT& area, size_t windowCount, std::vector<RECT>& outRects, bool horizontal) {
  outRects.resize(windowCount);
  if (windowCount == 0) return;

  LONG totalSize = horizontal ? (area.bottom - area.top) : (area.right - area.left);
  LONG cellSize = (totalSize - (LONG)(windowCount - 1) * g_tileGap) / (LONG)windowCount;
  LONG position = horizontal ? area.top : area.left;

  for (size_t i = 0; i < windowCount; ++i) {
    LONG end = (i == windowCount - 1) ? (horizontal ? area.bottom : area.right) : position + cellSize;
    outRects[i] = horizontal ? RECT{area.left, position, area.right, end} : RECT{position, area.top, end, area.bottom};
    position = end + g_tileGap;
  }
}

// Binary Space Partition: recursively split space alternating vertical/horizontal
void LayoutBSP(const RECT& area, size_t startIndex, size_t count, int depth, std::vector<RECT>& outRects) {
  if (count == 0) return;
  if (count == 1) {
    outRects[startIndex] = area;
    return;
  }

  bool splitVertical = (depth % 2 == 0);
  LONG mid = splitVertical ? area.left + (area.right - area.left - g_tileGap) / 2
                           : area.top + (area.bottom - area.top - g_tileGap) / 2;

  outRects[startIndex] =
      splitVertical ? RECT{area.left, area.top, mid, area.bottom} : RECT{area.left, area.top, area.right, mid};

  RECT remaining = splitVertical ? RECT{mid + g_tileGap, area.top, area.right, area.bottom}
                                 : RECT{area.left, mid + g_tileGap, area.right, area.bottom};

  LayoutBSP(remaining, startIndex + 1, count - 1, depth + 1, outRects);
}

TileLayout GetCurrentDesktopLayout() {
  GUID currentDesktopId;
  if (GetCurrentDesktopId(&currentDesktopId)) {
    auto it = g_desktopLayoutMap.find(currentDesktopId);
    if (it != g_desktopLayoutMap.end()) {
      return it->second;
    }
  }
  return g_currentLayout;
}

void SetCurrentDesktopLayout(TileLayout layout) {
  GUID currentDesktopId;
  if (GetCurrentDesktopId(&currentDesktopId)) {
    g_desktopLayoutMap[currentDesktopId] = layout;
  }
}

void TileWindows() {
  HWND foregroundWindow = GetForegroundWindow();
  HMONITOR monitor = foregroundWindow ? MonitorFromWindow(foregroundWindow, MONITOR_DEFAULTTONULL) : nullptr;

  if (!monitor) {
    POINT cursorPos;
    if (GetCursorPos(&cursorPos)) {
      monitor = MonitorFromPoint(cursorPos, MONITOR_DEFAULTTONEAREST);
    }
  }
  if (!monitor) return;

  MONITORINFO monitorInfo = {sizeof(monitorInfo)};
  if (!GetMonitorInfoW(monitor, &monitorInfo)) return;

  // Calculate work area with margins
  RECT workArea = {monitorInfo.rcWork.left + g_tileMargin, monitorInfo.rcWork.top + g_tileMargin,
                   monitorInfo.rcWork.right - g_tileMargin, monitorInfo.rcWork.bottom - g_tileMargin};

  if (workArea.right <= workArea.left || workArea.bottom <= workArea.top) return;

  // Collect eligible windows
  std::vector<HWND> windows;
  struct EnumContext {
    HMONITOR targetMonitor;
    std::vector<HWND>* windowList;
  } context = {monitor, &windows};

  EnumWindows(
      [](HWND hwnd, LPARAM lParam) WINAPI -> BOOL {
        auto* ctx = reinterpret_cast<EnumContext*>(lParam);
        if (IsTileEligible(hwnd, ctx->targetMonitor)) {
          ctx->windowList->push_back(hwnd);
        }
        return TRUE;
      },
      reinterpret_cast<LPARAM>(&context));

  if (windows.empty()) return;

  // Get layout for current desktop
  TileLayout layout = GetCurrentDesktopLayout();

  // Calculate and apply layout
  std::vector<RECT> windowRects(windows.size());
  switch (layout) {
    case TileLayout::MasterStack:
      LayoutMasterStack(workArea, windows.size(), windowRects, false);
      break;
    case TileLayout::MasterStackH:
      LayoutMasterStack(workArea, windows.size(), windowRects, true);
      break;
    case TileLayout::Columns:
      LayoutGrid(workArea, windows.size(), windowRects, false);
      break;
    case TileLayout::Rows:
      LayoutGrid(workArea, windows.size(), windowRects, true);
      break;
    case TileLayout::BSP:
      LayoutBSP(workArea, 0, windows.size(), 0, windowRects);
      break;
    case TileLayout::Monocle:
      windowRects.assign(windows.size(), workArea);
      break;
    default:
      break;
  }

  for (size_t i = 0; i < windows.size(); ++i) {
    PlaceWindow(windows[i], windowRects[i]);
  }
  Wh_Log(L"Tiled %zu windows with layout %d", windows.size(), static_cast<int>(layout));
}

//=============================================================================
// Background API Initialization Thread
// Retries API initialization in background if initial attempt fails
//=============================================================================

static volatile bool g_apiInitInProgress = false;
static volatile bool g_stopApiInitThread = false;
static HANDLE g_hApiInitThread = nullptr;

DWORD WINAPI ApiInitThreadProc(LPVOID) {
  Wh_Log(L"Background API initialization thread started");

  HRESULT coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
  if (FAILED(coHr)) {
    Wh_Log(L"Background thread CoInitializeEx failed: 0x%08X", coHr);
    g_apiInitInProgress = false;
    return 1;
  }

  for (int attempt = 1; attempt <= API_INIT_MAX_RETRIES; ++attempt) {
    if (g_stopApiInitThread || g_bInitialized) break;

    Wh_Log(L"API initialization attempt %d/%d", attempt, API_INIT_MAX_RETRIES);

    if (InitializeVirtualDesktopAPIOnce()) {
      g_bInitialized = true;
      Wh_Log(L"API initialized successfully on attempt %d", attempt);
      break;
    }

    if (attempt < API_INIT_MAX_RETRIES && !g_stopApiInitThread) {
      for (int i = 0; i < 10 && !g_stopApiInitThread; ++i) {
        Sleep(API_INIT_RETRY_DELAY_MS / 10);
      }
    }
  }

  if (!g_bInitialized && !g_stopApiInitThread) {
    Wh_Log(L"API initialization failed after %d attempts", API_INIT_MAX_RETRIES);
  }

  CoUninitialize();
  g_apiInitInProgress = false;
  return 0;
}

void StartBackgroundApiInit() {
  if (g_bInitialized || g_apiInitInProgress) return;

  g_stopApiInitThread = false;
  g_apiInitInProgress = true;
  g_hApiInitThread = CreateThread(nullptr, 0, ApiInitThreadProc, nullptr, 0, nullptr);
  if (!g_hApiInitThread) {
    g_apiInitInProgress = false;
  }
}

void StopBackgroundApiInit() {
  g_stopApiInitThread = true;
  if (g_hApiInitThread) {
    WaitForSingleObject(g_hApiInitThread, 3000);
    CloseHandle(g_hApiInitThread);
    g_hApiInitThread = nullptr;
  }
  g_apiInitInProgress = false;
}

//=============================================================================
// Hotkey Thread
//=============================================================================

DWORD WINAPI HotkeyThreadProc(LPVOID) {
  g_threadId = GetCurrentThreadId();
  Wh_Log(L"Hotkey thread started, thread ID: %lu", g_threadId);

  HRESULT coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
  Wh_Log(L"CoInitializeEx result: 0x%08X", coHr);

  // Try once synchronously, if it fails, start background retry
  if (!InitializeVirtualDesktopAPI()) {
    Wh_Log(L"Initial API init failed, starting background retry thread");
    StartBackgroundApiInit();
  }

  // Create message queue and signal ready immediately
  MSG msg;
  PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);
  SetEvent(g_hReadyEvent);

  for (int i = 1; i <= g_maxDesktops; ++i) {
    RegisterHotKey(nullptr, HK_MOVE_BASE + i - 1, g_moveModifiers, '0' + i);
    RegisterHotKey(nullptr, HK_SWITCH_BASE + i - 1, g_switchModifiers, '0' + i);
  }
  RegisterHotKey(nullptr, HK_PREV, g_utilityModifiers, g_prevDesktopKey);
  RegisterHotKey(nullptr, HK_PIN, g_utilityModifiers, g_pinKey);
  RegisterHotKey(nullptr, HK_TILE, g_utilityModifiers, g_tileKey);
  RegisterHotKey(nullptr, HK_LAYOUT, g_utilityModifiers, g_layoutKey);
  Wh_Log(L"Hotkeys registered");

  // Message loop - use MsgWaitForMultipleObjects to allow periodic check for stop signal
  while (!g_stopHotkeyThread) {
    // Wait for message or timeout (100ms) to check stop flag
    DWORD waitResult = MsgWaitForMultipleObjects(0, nullptr, FALSE, 100, QS_ALLINPUT);
    
    if (waitResult == WAIT_OBJECT_0) {
      // Messages available
      while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
          goto cleanup;
        }
        if (msg.message == WM_HOTKEY) {
          UINT hotkeyId = static_cast<UINT>(msg.wParam);

          if (hotkeyId >= HK_MOVE_BASE && hotkeyId < HK_MOVE_BASE + 9) {
            int desktopNum = hotkeyId - HK_MOVE_BASE + 1;
            if (MoveActiveWindowToDesktopNum(desktopNum) && g_followMovedWindow) {
              GoToDesktopNum(desktopNum);
            }
          } else if (hotkeyId >= HK_SWITCH_BASE && hotkeyId < HK_SWITCH_BASE + 9) {
            GoToDesktopNum(hotkeyId - HK_SWITCH_BASE + 1);
          } else if (hotkeyId == HK_PREV) {
            SwitchToPreviousDesktop();
          } else if (hotkeyId == HK_PIN) {
            TogglePinWindow();
          } else if (hotkeyId == HK_TILE) {
            TileWindows();
          } else if (hotkeyId == HK_LAYOUT) {
            TileLayout currentLayout = GetCurrentDesktopLayout();
            TileLayout newLayout =
                static_cast<TileLayout>((static_cast<int>(currentLayout) + 1) % static_cast<int>(TileLayout::COUNT));
            SetCurrentDesktopLayout(newLayout);
            TileWindows();
          }
        }
      }
    }
    // WAIT_TIMEOUT: just loop and check g_stopHotkeyThread
  }

cleanup:
  for (int i = 0; i < 9; ++i) {
    UnregisterHotKey(nullptr, HK_MOVE_BASE + i);
    UnregisterHotKey(nullptr, HK_SWITCH_BASE + i);
  }
  for (int hk : {HK_PREV, HK_PIN, HK_TILE, HK_LAYOUT}) UnregisterHotKey(nullptr, hk);

  CleanupVirtualDesktopAPI();
  CoUninitialize();
  return 0;
}

bool StartHotkeyThread() {
  g_hReadyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
  g_hThread = CreateThread(nullptr, 0, HotkeyThreadProc, nullptr, 0, nullptr);

  if (!g_hThread || WaitForSingleObject(g_hReadyEvent, 5000) != WAIT_OBJECT_0) {
    Wh_Log(L"Failed to start hotkey thread");
    if (g_hThread) {
      CloseHandle(g_hThread);
      g_hThread = nullptr;
    }
    CloseHandle(g_hReadyEvent);
    g_hReadyEvent = nullptr;
    return false;
  }

  CloseHandle(g_hReadyEvent);
  g_hReadyEvent = nullptr;
  return true;
}

void StopHotkeyThread() {
  StopBackgroundApiInit();

  // Signal thread to stop
  g_stopHotkeyThread = true;
  
  if (g_threadId) {
    PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
  }
  if (g_hThread) {
    WaitForSingleObject(g_hThread, INFINITE);
    CloseHandle(g_hThread);
    g_hThread = nullptr;
  }
  g_threadId = 0;
  g_stopHotkeyThread = false;
}

//=============================================================================
// Windhawk Tool Mod Entry Points
//=============================================================================

BOOL WhTool_ModInit() {
  Wh_Log(L"Virtual Desktop Helper mod initializing...");
  LoadSettings();
  if (!StartHotkeyThread()) {
    Wh_Log(L"Failed to start hotkey thread");
    return FALSE;
  }
  Wh_Log(L"Virtual Desktop Helper mod initialized successfully");
  return TRUE;
}

void WhTool_ModUninit() {
  Wh_Log(L"Virtual Desktop Helper mod uninitializing...");
  StopHotkeyThread();
  Wh_Log(L"Virtual Desktop Helper mod uninitialized");
}

void WhTool_ModSettingsChanged() {
  Wh_Log(L"Settings changed, reloading...");
  StopHotkeyThread();
  g_desktopFocusMap.clear();
  g_desktopLayoutMap.clear();
  LoadSettings();
  StartHotkeyThread();
}

//=============================================================================
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk-mods/pull/1916
//
// The mod will load and run in a dedicated windhawk.exe process.
//=============================================================================

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
  Wh_Log(L">");
  ExitThread(0);
}

BOOL Wh_ModInit() {
  bool isService = false;
  bool isToolModProcess = false;
  bool isCurrentToolModProcess = false;
  int argc;
  LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
  if (!argv) {
    Wh_Log(L"CommandLineToArgvW failed");
    return FALSE;
  }

  for (int i = 1; i < argc; i++) {
    if (wcscmp(argv[i], L"-service") == 0) {
      isService = true;
      break;
    }
  }

  for (int i = 1; i < argc - 1; i++) {
    if (wcscmp(argv[i], L"-tool-mod") == 0) {
      isToolModProcess = true;
      if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
        isCurrentToolModProcess = true;
      }
      break;
    }
  }

  LocalFree(argv);

  if (isService) {
    return FALSE;
  }

  if (isCurrentToolModProcess) {
    g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
    if (!g_toolModProcessMutex) {
      Wh_Log(L"CreateMutex failed");
      ExitProcess(1);
    }

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
      Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
      ExitProcess(1);
    }

    if (!WhTool_ModInit()) {
      ExitProcess(1);
    }

    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
    IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

    DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
    void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

    Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
    return TRUE;
  }

  if (isToolModProcess) {
    return FALSE;
  }

  g_isToolModProcessLauncher = true;
  return TRUE;
}

void Wh_ModAfterInit() {
  if (!g_isToolModProcessLauncher) {
    return;
  }

  WCHAR currentProcessPath[MAX_PATH];
  switch (GetModuleFileName(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath))) {
    case 0:
    case ARRAYSIZE(currentProcessPath):
      Wh_Log(L"GetModuleFileName failed");
      return;
  }

  WCHAR
  commandLine[MAX_PATH + 2 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
  swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

  HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
  if (!kernelModule) {
    kernelModule = GetModuleHandle(L"kernel32.dll");
    if (!kernelModule) {
      Wh_Log(L"No kernelbase.dll/kernel32.dll");
      return;
    }
  }

  using CreateProcessInternalW_t = BOOL(WINAPI*)(
      HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
      LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
      LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation,
      PHANDLE hRestrictedUserToken);
  CreateProcessInternalW_t pCreateProcessInternalW =
      (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");
  if (!pCreateProcessInternalW) {
    Wh_Log(L"No CreateProcessInternalW");
    return;
  }

  STARTUPINFO si{
      .cb = sizeof(STARTUPINFO),
      .dwFlags = STARTF_FORCEOFFFEEDBACK,
  };
  PROCESS_INFORMATION pi;
  if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine, nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                               nullptr, nullptr, &si, &pi, nullptr)) {
    Wh_Log(L"CreateProcess failed");
    return;
  }

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
  if (g_isToolModProcessLauncher) {
    return;
  }

  WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
  if (g_isToolModProcessLauncher) {
    return;
  }

  WhTool_ModUninit();
  ExitProcess(0);
}
