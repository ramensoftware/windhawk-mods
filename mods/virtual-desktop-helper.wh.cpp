// ==WindhawkMod==
// @id              virtual-desktop-helper
// @name            Virtual Desktop Helper
// @description     Switch virtual desktops, move windows between desktops, and pin windows with customizable hotkeys
// @version         2.2.0
// @author          u2x1
// @github          https://github.com/u2x1
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Virtual Desktop Helper

A comprehensive virtual desktop management tool for Windows 10/11.

Based on VD.ahk by FuPeiJiang.

## Features

### Virtual Desktop Management
- **Quick Switch**: Jump directly to any desktop (1-9) with a single hotkey
- **Move Windows**: Send the active window to any desktop instantly
- **Previous Desktop (Index)**: Switch to the previous desktop (idx-1, wraps around)
- **Next Desktop (Index)**: Switch to the next desktop (idx+1, wraps around)
- **Last Desktop**: Toggle back to the last visited desktop
- **Pin Windows**: Pin/unpin windows to appear on all desktops

## Default Hotkeys

| Action | Default Hotkey |
|--------|----------------|
| Switch to desktop 1-9 | `Alt + 1-9` |
| Move window to desktop 1-9 | `Alt + Shift + 1-9` |
| Switch to previous desktop (idx-1) | `Alt + Z` (configurable modifier) |
| Switch to next desktop (idx+1) | `Alt + X` (configurable modifier) |
| Toggle last desktop | `Alt + Q` (configurable modifier) |
| Pin/unpin window | `Alt + P` (configurable modifier) |

Note: The modifier for utility hotkeys (Previous/Next Desktop (Index), Last Desktop, Pin) can be changed in settings. Previous/next cycling is limited by the "Maximum Desktops" setting.

## Customization

Settings are organized by feature. Each feature has an **Enable** toggle and its associated key configuration:

### Hotkey Groups

- **[Switch Desktop]** - Alt+1-9 to switch desktops
- **[Move Window]** - Alt+Shift+1-9 to move windows between desktops
- **[Prev/Next Desktop]** - Previous/next by index and last-desktop toggle (Alt+Z, Alt+X, Alt+Q by default)
- **[Pin Window]** - Pin/unpin windows to all desktops (Alt+P by default)
### Key Binding Format

Hotkey fields accept any single character:

**Letters & Numbers:** A-Z, 0-9  
**Special Characters:**
- `` ` `` (backtick), `~` (tilde)
- `-` (minus), `=` (equals)
- `[`, `]`, `\` (backslash)
- `;` (semicolon), `'` (quote)
- `,` (comma), `.` (period), `/` (slash)
- Shifted versions: `!@#$%^&*()_+{}|:"<>?`

**Special Keys:** Type the name: `Tab`, `Space`, `Enter`

**Examples:**
- Enter `F` → binds to Alt+F
- Enter `~` → binds to Alt+` (backtick)
- Enter `!` → binds to Alt+1 (shifted)
- Enter `Tab` → binds to Alt+Tab

**Example:** If you only want Alt+1-9 for switching desktops:
1. Disable all options except "[Switch Desktop] Enable"
2. All other hotkeys will be unregistered

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

- EnableSwitchDesktop: true
  $name: '[Switch Desktop] Enable'
  $description: Enable Alt+1-9 (or chosen modifier) to switch desktops

- SwitchDesktopModifier: alt
  $name: '[Switch Desktop] Modifier'
  $description: Modifier keys for switching to desktop (combined with number keys 1-9)
  $options:
    - alt: Alt
    - ctrl: Ctrl
    - alt+shift: Alt + Shift
    - ctrl+alt: Ctrl + Alt
    - ctrl+shift: Ctrl + Shift

- EnableMoveWindow: true
  $name: '[Move Window] Enable'
  $description: Enable Alt+Shift+1-9 (or chosen modifier) to move windows between desktops

- MoveWindowModifier: alt+shift
  $name: '[Move Window] Modifier'
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

- EnablePrevNextDesktop: true
  $name: '[Prev/Next Desktop] Enable'
  $description: Enable hotkeys to cycle between desktops

- UtilityModifier: alt
  $name: '[Prev/Next Desktop] Modifier'
  $description: Modifier keys for previous/next desktop hotkeys
  $options:
    - alt: Alt
    - ctrl: Ctrl
    - alt+shift: Alt + Shift
    - ctrl+alt: Ctrl + Alt
    - ctrl+shift: Ctrl + Shift

- PrevDesktopKey: "Q"
  $name: '[Prev/Next Desktop] Last Desktop Key'
  $description: 'Key to toggle back to the last visited desktop. Examples: Q, F, ~, !, [, Tab, Space'

- PrevIndexKey: "Z"
  $name: '[Prev/Next Desktop] Previous Desktop (Index) Key'
  $description: 'Key to switch to the previous desktop by index (idx-1, wraps around). Examples: Z, A, <, ,, Tab'

- NextDesktopKey: "X"
  $name: '[Prev/Next Desktop] Next Key'
  $description: 'Key to switch to the next desktop (wraps around). Examples: X, E, N, Z, @, ], Enter'

- EnablePinWindow: true
  $name: '[Pin Window] Enable'
  $description: Enable hotkey to pin/unpin windows to all desktops

- PinKey: "P"
  $name: '[Pin Window] Key'
  $description: 'Key to pin/unpin the active window to all desktops. Examples: P, W, #, ;, \\'

*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <objbase.h>
#include <objectarray.h>
#include <oleacc.h>
#include <shobjidl.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <unordered_map>

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
// HK_PIN (20): Pin/unpin current window
// HK_NEXT (21): Switch to next desktop (wrap around)
// HK_PREV_INDEX (22): Switch to previous desktop by index (wrap around)
enum HotkeyIds {
  HK_MOVE_BASE = 1,
  HK_SWITCH_BASE = 10,
  HK_PREV = 19,
  HK_PIN = 20,
  HK_NEXT = 21,
  HK_PREV_INDEX = 22
};

static UINT g_prevDesktopKey = 'Q';
static UINT g_prevIndexKey = 'Z';
static UINT g_nextDesktopKey = 'X';
static UINT g_pinKey = 'P';

static bool g_enableSwitchDesktop = true;
static bool g_enableMoveWindow = true;
static bool g_enablePrevNextDesktop = true;
static bool g_enablePinWindow = true;

// Per-desktop state: tracks last focused window for each virtual desktop
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

//=============================================================================
// Helper Functions
//=============================================================================

// Access VTable function by index for undocumented COM interfaces
template <typename T>
T GetVTableFunction(void* pInterface, int index) {
  return reinterpret_cast<T>((*reinterpret_cast<void***>(pInterface))[index]);
}

bool InitializeVirtualDesktopAPI();

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

// Parse single character to virtual key code
// Supports A-Z, 0-9, and special characters
UINT ParseSingleCharKey(PCWSTR str) {
  if (!str || !str[0]) return 0;
  wchar_t c = str[0];

  // Letters A-Z (and a-z)
  if (c >= L'A' && c <= L'Z') return c;
  if (c >= L'a' && c <= L'z') return c - L'a' + L'A';

  // Numbers 0-9
  if (c >= L'0' && c <= L'9') return c;

  // Number row symbols
  if (c == L'!') return '1';
  if (c == L'@') return '2';
  if (c == L'#') return '3';
  if (c == L'$') return '4';
  if (c == L'%') return '5';
  if (c == L'^') return '6';
  if (c == L'&') return '7';
  if (c == L'*') return '8';
  if (c == L'(') return '9';
  if (c == L')') return '0';

  // Other common special characters
  if (c == L'`' || c == L'~') return VK_OEM_3;      // Grave/tilde key
  if (c == L'-' || c == L'_') return VK_OEM_MINUS;  // Minus/underscore
  if (c == L'=' || c == L'+') return VK_OEM_PLUS;   // Equals/plus (VK_OEM_PLUS is same as VK_ADD on some keyboards, use VK_OEM_NEC_EQUAL for some layouts)
  if (c == L'[' || c == L'{') return VK_OEM_4;      // Left bracket
  if (c == L']' || c == L'}') return VK_OEM_6;      // Right bracket
  if (c == L'\\' || c == L'|') return VK_OEM_5;     // Backslash/pipe
  if (c == L';' || c == L':') return VK_OEM_1;      // Semicolon/colon
  if (c == L'\'' || c == L'"') return VK_OEM_7;     // Quote
  if (c == L',' || c == L'<') return VK_OEM_COMMA;  // Comma
  if (c == L'.' || c == L'>') return VK_OEM_PERIOD; // Period
  if (c == L'/' || c == L'?') return VK_OEM_2;      // Slash

  // Space
  if (c == L' ') return VK_SPACE;

  return 0;
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

  // Enable/disable toggles
  g_enableSwitchDesktop = Wh_GetIntSetting(L"EnableSwitchDesktop") != 0;
  g_enableMoveWindow = Wh_GetIntSetting(L"EnableMoveWindow") != 0;
  g_enablePrevNextDesktop = Wh_GetIntSetting(L"EnablePrevNextDesktop") != 0;
  g_enablePinWindow = Wh_GetIntSetting(L"EnablePinWindow") != 0;

  // Load hotkey settings (single character input supporting A-Z, 0-9, and special chars)
  g_prevDesktopKey = ReadStringSetting(L"PrevDesktopKey", ParseSingleCharKey, (UINT)'Q');
  g_prevIndexKey = ReadStringSetting(L"PrevIndexKey", ParseSingleCharKey, (UINT)'Z');
  g_nextDesktopKey = ReadStringSetting(L"NextDesktopKey", ParseSingleCharKey, (UINT)'X');
  g_pinKey = ReadStringSetting(L"PinKey", ParseSingleCharKey, (UINT)'P');
}

//=============================================================================
// Virtual Desktop API Initialization
//=============================================================================

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

void CleanupVirtualDesktopAPI();
bool ReinitializeVirtualDesktopAPI();

bool InitializeVirtualDesktopAPI() {
  if (g_bInitialized) return true;

  if (InitializeVirtualDesktopAPIOnce()) {
    g_bInitialized = true;
    return true;
  }
  return false;
}

bool ReinitializeVirtualDesktopAPI() {
  CleanupVirtualDesktopAPI();
  return InitializeVirtualDesktopAPI();
}

void CleanupVirtualDesktopAPI() {
  // Check if Explorer is still running - if not, skip Release calls to avoid hangs
  HWND hShell = GetShellWindow();
  if (!hShell || !IsWindow(hShell)) {
    Wh_Log(L"Explorer not available, skipping COM cleanup to avoid hang");
    g_pPinnedApps = nullptr;
    g_pDesktopManager = nullptr;
    g_pViewCollection = nullptr;
    g_pDesktopManagerInternal = nullptr;
    g_pServiceProvider = nullptr;
    g_bInitialized = false;
    return;
  }

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
    HRESULT hr = pfn(g_pDesktopManagerInternal, nullptr, outResult);
    if (FAILED(hr) && ReinitializeVirtualDesktopAPI()) {
      // in case explorer.exe restarted
      pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HMONITOR, TResult*)>(g_pDesktopManagerInternal,
                                                                                      vtableIndex);
      hr = pfn(g_pDesktopManagerInternal, nullptr, outResult);
    }
    return hr;
  } else {
    auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, TResult*)>(g_pDesktopManagerInternal, vtableIndex);
    HRESULT hr = pfn(g_pDesktopManagerInternal, outResult);
    if (FAILED(hr) && ReinitializeVirtualDesktopAPI()) {
      pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, TResult*)>(g_pDesktopManagerInternal, vtableIndex);
      hr = pfn(g_pDesktopManagerInternal, outResult);
    }
    return hr;
  }
}

template <typename TArg>
HRESULT CallManagerInternalWithArg(int vtableIndex, TArg arg) {
  if (UsesHMonitorParameter()) {
    auto pfn =
        GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HMONITOR, TArg)>(g_pDesktopManagerInternal, vtableIndex);
    HRESULT hr = pfn(g_pDesktopManagerInternal, nullptr, arg);
    if (FAILED(hr) && ReinitializeVirtualDesktopAPI()) {
      pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HMONITOR, TArg)>(g_pDesktopManagerInternal, vtableIndex);
      hr = pfn(g_pDesktopManagerInternal, nullptr, arg);
    }
    return hr;
  } else {
    auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, TArg)>(g_pDesktopManagerInternal, vtableIndex);
    HRESULT hr = pfn(g_pDesktopManagerInternal, arg);
    if (FAILED(hr) && ReinitializeVirtualDesktopAPI()) {
      pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, TArg)>(g_pDesktopManagerInternal, vtableIndex);
      hr = pfn(g_pDesktopManagerInternal, arg);
    }
    return hr;
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

bool GoToDesktopNum(int desktopNum, HWND preferredFocusHwnd = nullptr) {
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
    // If requested, prefer focusing a specific window (e.g. the one that was just moved).
    if (preferredFocusHwnd && IsEligibleWindow(preferredFocusHwnd) && g_pDesktopManager) {
      GUID preferredDesktopId;
      if (SUCCEEDED(g_pDesktopManager->GetWindowDesktopId(preferredFocusHwnd, &preferredDesktopId)) &&
          IsEqualGUID(preferredDesktopId, targetDesktopId)) {
        windowToFocus = preferredFocusHwnd;
      }
    }

    if (!windowToFocus) {
      auto it = g_desktopFocusMap.find(targetDesktopId);
      if (it != g_desktopFocusMap.end() && IsEligibleWindow(it->second)) {
        GUID windowDesktopId;
        if (g_pDesktopManager && SUCCEEDED(g_pDesktopManager->GetWindowDesktopId(it->second, &windowDesktopId))) {
          if (IsEqualGUID(windowDesktopId, targetDesktopId)) {
            windowToFocus = it->second;
          }
        }
      }
    }

    if (!windowToFocus) {
      windowToFocus = FindWindowOnDesktop(targetDesktopId, preferredFocusHwnd);
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
  if (!InitializeVirtualDesktopAPI()) return false;

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

bool SwitchToPreviousIndexDesktop() {
  Wh_Log(L"SwitchToPreviousIndexDesktop called");
  if (!InitializeVirtualDesktopAPI()) {
    Wh_Log(L"SwitchToPreviousIndexDesktop: API not initialized");
    return false;
  }

  GUID currentDesktopId = {};
  if (!GetCurrentDesktopId(&currentDesktopId)) {
    Wh_Log(L"SwitchToPreviousIndexDesktop: Failed to get current desktop ID");
    return false;
  }

  int currentIndex = GetDesktopIndexById(currentDesktopId);
  if (currentIndex < 0) {
    Wh_Log(L"SwitchToPreviousIndexDesktop: Invalid current index");
    return false;
  }

  IObjectArray* desktops = GetDesktops();
  if (!desktops) {
    Wh_Log(L"SwitchToPreviousIndexDesktop: Failed to get desktops");
    return false;
  }

  UINT desktopCount = 0;
  desktops->GetCount(&desktopCount);
  desktops->Release();

  Wh_Log(L"SwitchToPreviousIndexDesktop: currentIndex=%d, desktopCount=%u, maxDesktops=%d", currentIndex,
         desktopCount, g_maxDesktops);

  int cycleCount = (int)desktopCount;
  if (cycleCount > g_maxDesktops) cycleCount = g_maxDesktops;
  if (cycleCount <= 0) return false;

  int prevIndex = (currentIndex - 1 + cycleCount) % cycleCount;
  Wh_Log(L"SwitchToPreviousIndexDesktop: Switching to desktop %d", prevIndex + 1);
  return GoToDesktopNum(prevIndex + 1);
}

bool SwitchToNextDesktop() {
  Wh_Log(L"SwitchToNextDesktop called");
  if (!InitializeVirtualDesktopAPI()) {
    Wh_Log(L"SwitchToNextDesktop: API not initialized");
    return false;
  }

  GUID currentDesktopId = {};
  if (!GetCurrentDesktopId(&currentDesktopId)) {
    Wh_Log(L"SwitchToNextDesktop: Failed to get current desktop ID");
    return false;
  }

  int currentIndex = GetDesktopIndexById(currentDesktopId);
  if (currentIndex < 0) {
    Wh_Log(L"SwitchToNextDesktop: Invalid current index");
    return false;
  }

  IObjectArray* desktops = GetDesktops();
  if (!desktops) {
    Wh_Log(L"SwitchToNextDesktop: Failed to get desktops");
    return false;
  }

  UINT desktopCount = 0;
  desktops->GetCount(&desktopCount);
  desktops->Release();

  Wh_Log(L"SwitchToNextDesktop: currentIndex=%d, desktopCount=%u, maxDesktops=%d", currentIndex, desktopCount,
         g_maxDesktops);

  int cycleCount = (int)desktopCount;
  if (cycleCount > g_maxDesktops) cycleCount = g_maxDesktops;
  if (cycleCount <= 0) return false;

  int nextIndex = (currentIndex + 1) % cycleCount;
  Wh_Log(L"SwitchToNextDesktop: Switching to desktop %d", nextIndex + 1);
  return GoToDesktopNum(nextIndex + 1);
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
// Hotkey Thread
//=============================================================================

DWORD WINAPI HotkeyThreadProc(LPVOID) {
  g_threadId = GetCurrentThreadId();
  Wh_Log(L"Hotkey thread started, thread ID: %lu", g_threadId);

  HRESULT coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
  Wh_Log(L"CoInitializeEx result: 0x%08X", coHr);

  // Create message queue and signal ready immediately
  MSG msg;
  PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);
  SetEvent(g_hReadyEvent);

  if (!InitializeVirtualDesktopAPI()) {
    Wh_Log(L"Virtual Desktop API failed to initialize on startup");
  }

  if (g_enableMoveWindow) {
    for (int i = 1; i <= g_maxDesktops; ++i) {
      RegisterHotKey(nullptr, HK_MOVE_BASE + i - 1, g_moveModifiers, '0' + i);
    }
  }
  if (g_enableSwitchDesktop) {
    for (int i = 1; i <= g_maxDesktops; ++i) {
      RegisterHotKey(nullptr, HK_SWITCH_BASE + i - 1, g_switchModifiers, '0' + i);
    }
  }
  if (g_enablePrevNextDesktop) {
    RegisterHotKey(nullptr, HK_PREV, g_utilityModifiers, g_prevDesktopKey);
    RegisterHotKey(nullptr, HK_PREV_INDEX, g_utilityModifiers, g_prevIndexKey);
    RegisterHotKey(nullptr, HK_NEXT, g_utilityModifiers, g_nextDesktopKey);
  }
  if (g_enablePinWindow) {
    RegisterHotKey(nullptr, HK_PIN, g_utilityModifiers, g_pinKey);
  }
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

          // All other hotkeys require Virtual Desktop API
          if (!g_bInitialized && !InitializeVirtualDesktopAPI()) {
            Wh_Log(L"Hotkey ignored: API not initialized");
            continue;
          }

          if (hotkeyId >= HK_MOVE_BASE && hotkeyId < HK_MOVE_BASE + 9) {
            int desktopNum = hotkeyId - HK_MOVE_BASE + 1;
            HWND movedHwnd = GetForegroundWindow();
            bool moved = MoveActiveWindowToDesktopNum(desktopNum);
            if (moved && g_followMovedWindow) {
              GoToDesktopNum(desktopNum, movedHwnd);
            }
          } else if (hotkeyId >= HK_SWITCH_BASE && hotkeyId < HK_SWITCH_BASE + 9) {
            GoToDesktopNum(hotkeyId - HK_SWITCH_BASE + 1);
          } else if (hotkeyId == HK_PREV) {
            SwitchToPreviousDesktop();
          } else if (hotkeyId == HK_PREV_INDEX) {
            SwitchToPreviousIndexDesktop();
          } else if (hotkeyId == HK_NEXT) {
            SwitchToNextDesktop();
          } else if (hotkeyId == HK_PIN) {
            TogglePinWindow();
          }
        }
      }
    }
    // WAIT_TIMEOUT: just loop and check g_stopHotkeyThread
  }

cleanup:
  if (g_enableMoveWindow) {
    for (int i = 0; i < 9; ++i) {
      UnregisterHotKey(nullptr, HK_MOVE_BASE + i);
    }
  }
  if (g_enableSwitchDesktop) {
    for (int i = 0; i < 9; ++i) {
      UnregisterHotKey(nullptr, HK_SWITCH_BASE + i);
    }
  }
  if (g_enablePrevNextDesktop) {
    UnregisterHotKey(nullptr, HK_PREV);
    UnregisterHotKey(nullptr, HK_PREV_INDEX);
    UnregisterHotKey(nullptr, HK_NEXT);
  }
  if (g_enablePinWindow) {
    UnregisterHotKey(nullptr, HK_PIN);
  }

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
  // Signal thread to stop
  g_stopHotkeyThread = true;

  // Disable operations to prevent new COM calls during cleanup
  g_bInitialized = false;

  if (g_threadId) {
    PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
  }

  if (g_hThread) {
    // Wait with timeout to avoid infinite hang on CoUninitialize
    DWORD waitResult = WaitForSingleObject(g_hThread, 5000);
    if (waitResult == WAIT_TIMEOUT) {
      Wh_Log(L"WARNING: Hotkey thread cleanup timeout, thread may be stuck in CoUninitialize");
      // Do NOT call TerminateThread - let Windows handle cleanup on process exit
      // to avoid corrupting COM state and potentially crashing Explorer
    }
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
  LoadSettings();
  if (!StartHotkeyThread()) {
    Wh_Log(L"Failed to restart hotkey thread after settings change");
  }
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
