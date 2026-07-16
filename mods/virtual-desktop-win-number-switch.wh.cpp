// ==WindhawkMod==
// @id              virtual-desktop-win-number-switch
// @name            Win+Number Virtual Desktop Switch
// @description     Remaps Win+1..9 from "activate Nth taskbar app" to Linux-style virtual desktop switching, with auto-create/auto-remove of desktops
// @version         2.9
// @author          Cospamos
// @github          https://github.com/Cospamos
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Win+Number Virtual Desktop Switch

Replaces the default Windows Win+1..9 behavior (activate the Nth app pinned/open
on the taskbar) with Linux-style virtual desktop switching:

- **Win+N** switches to virtual desktop N.
- **Win+Shift+N** moves the currently focused window to desktop N, and
  switches your view there too (can be turned off in settings to stay put).
- If desktop N does not exist yet, it (and any desktops before it) is **created
  automatically**.
- When you switch away from a desktop that has **no windows left on it**
  (windows pinned to all desktops don't count), that desktop is **removed
  automatically**.
- No taskbar flash asking you to switch back: before switching, the mod hands
  focus to Explorer's own taskbar window so its internal focus-restore isn't
  denied and doesn't fall back to flashing a taskbar button, then takes real
  focus for the actual target window itself right after.
- No "Desktop N" name overlay popping up on builds where that experimental
  animation is enabled.

## How it works

This mod runs as a dedicated background process (a Windhawk "tool mod") and
installs a low-level keyboard hook that intercepts Win+1..9 and Win+Shift+1..9
system-wide before Explorer's own taskbar shortcut handling sees them, then
talks to the undocumented Virtual Desktop COM interfaces to
switch/create/remove desktops and move windows between them.

## Important notes

- The "Desktop N" name overlay is a `XamlExplorerHostIslandWindow` hosted by
  explorer.exe, and its visibility is toggled through DirectComposition/DWM
  rather than `CreateWindowExW`/`ShowWindow`/`SetWindowPos` (confirmed by
  injecting into explorer.exe and hooking those calls directly - they never
  fired for it), so this mod can't veto it before it's ever painted. Instead
  it reacts to it via `SetWinEventHook` and hides it immediately, plus
  proactively re-hides the same window (it's reused, not recreated) right
  before the next switch starts. This can still show a very brief flicker in
  rare cases since it's reactive rather than fully preventive. Turn off
  "Hide the 'current desktop' name overlay" in settings if you'd rather
  Windows' default behavior.
- The Virtual Desktop COM interface IDs (GUIDs) and method layout change
  between Windows builds. This mod auto-detects your build from the registry
  and picks the matching interface set. If desktop switching silently does
  nothing, check the mod's debug log (Windhawk -> mod -> Logs) for the
  detected build number, and try overriding "Windows Version" manually in
  settings.
- Auto-create/auto-remove is verified on Windows 11 22621+ (23H2/24H2/25H2).
  On older Windows 10 builds the create/remove vtable slots are a best-effort
  guess and may need adjusting (see `createDesktopIndex`/`removeDesktopIndex`
  in `g_versionIIDs` in the source).
- Windows blocks a normal-privilege keyboard hook from seeing keystrokes while
  a higher-privilege ("Run as administrator") window has focus (UIPI). By
  default this mod never elevates itself - Win+N just won't intercept while
  an elevated app (Task Manager, an admin terminal, etc.) has focus. If you
  want it to work in that case too, turn on "Request admin rights for the
  background helper" in settings; it's off by default so the mod never asks
  for elevation without your explicit opt-in.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- WindowsVersion: auto
  $name: Windows Version
  $description: >-
    Leave on auto unless desktop switching doesn't work for you.
  $options:
    - auto: Auto-detect (recommended)
    - win10_old: Windows 10 (Build < 20348)
    - win10_20348: Windows 10 (Build 20348 - 21999)
    - win11_22000: Windows 11 (Build 22000 - 22482)
    - win11_22621: Windows 11 (Build 22621/22631/23H2)
    - win11_26100: Windows 11 (Build 26100+ / 24H2, 25H2)

- SwitchToDesktopOnMove: true
  $name: Switch to the desktop when moving a window
  $description: Turn off to stay on your current desktop instead of following the moved window.

- AutoCreateDesktop: true
  $name: Auto-create missing desktops
  $description: Creates desktop N (and any before it) if it doesn't exist yet.

- AutoRemoveEmptyDesktop: true
  $name: Auto-remove empty desktops
  $description: Removes a desktop automatically once it has no windows left on it.

- HideDesktopSwitchLabel: true
  $name: Hide the "current desktop" name overlay
  $description: >-
    Hides the "Desktop N" label some Windows 11 builds show when switching.
    Reactive, not instant - a brief flicker is still possible in rare cases.

- RequestElevatedHelper: false
  $name: Request admin rights for the background helper
  $description: >-
    Off by default - only enable if Win+N stops working while an elevated
    app (Task Manager, admin terminal, etc.) has focus. Causes one UAC
    prompt when the mod starts.

- WinReleaseDelayMs: 500
  $name: (TEMP) Win key un-stick delay (ms)
  $description: Tuning knob, will be removed.
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <objbase.h>
#include <objectarray.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <windows.h>
#include <cstdlib>
#include <utility>

#define SAFE_RELEASE(p) \
  do {                  \
    if (p) {            \
      (p)->Release();   \
      (p) = nullptr;    \
    }                   \
  } while (0)

//=============================================================================
// COM CLSIDs and IIDs for Virtual Desktop API (undocumented)
// Reverse-engineered from Windows shell components.
// Reference: https://github.com/Ciantic/VirtualDesktopAccessor
//            https://github.com/MScholtes/VirtualDesktop
//=============================================================================

static const CLSID CLSID_ImmersiveShell = {
    0xC2F03A33, 0x21F5, 0x47FA, {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39}};

static const CLSID CLSID_VirtualDesktopManagerInternal = {
    0xC5E0CDCA, 0x7B6E, 0x41B2, {0x9F, 0xC4, 0xD9, 0x39, 0x75, 0xCC, 0x46, 0x7B}};

static const CLSID CLSID_VirtualDesktopPinnedApps = {
    0xB5A399E7, 0x1C87, 0x46B8, {0x88, 0xE9, 0xFC, 0x57, 0x47, 0xB1, 0x71, 0xBD}};

static const IID IID_IApplicationViewCollection = {
    0x1841C6D7, 0x4F9D, 0x42C0, {0xAF, 0x41, 0x87, 0x47, 0x53, 0x8F, 0x10, 0xE5}};

static const IID IID_IVirtualDesktopPinnedApps = {
    0x4CE81583, 0x1E4C, 0x4632, {0xA6, 0x21, 0x07, 0xA5, 0x35, 0x43, 0x14, 0x8F}};

//=============================================================================
// Windows Version-Specific Interface IDs / VTable layout
//=============================================================================
struct VersionIIDs {
  IID managerInternal;
  IID virtualDesktop;
  bool usesHMonitor;
  int createDesktopIndex;  // IVirtualDesktopManagerInternal::CreateDesktop vtable slot
  int removeDesktopIndex;  // IVirtualDesktopManagerInternal::RemoveDesktop vtable slot
};

static const VersionIIDs g_versionIIDs[] = {
    // [0] Windows 10 (Build < 20348)
    {{0xF31574D6, 0xB682, 0x4CDC, {0xBD, 0x56, 0x18, 0x27, 0x86, 0x0A, 0xBE, 0xC6}},
     {0xFF72FFDD, 0xBE7E, 0x43FC, {0x9C, 0x03, 0xAD, 0x81, 0x68, 0x1E, 0x88, 0xE4}},
     false, 10, 11},

    // [1] Windows 10 (Build 20348 - 21999) -- create/remove indices best-effort
    {{0x094AFE11, 0x44F2, 0x4BA0, {0x97, 0x6F, 0x29, 0xA9, 0x7E, 0x26, 0x3E, 0xE0}},
     {0x62FDF88B, 0x11CA, 0x4AFB, {0x8B, 0xD8, 0x22, 0x96, 0xDF, 0xAE, 0x49, 0xE2}},
     true, 10, 11},

    // [2] Windows 11 (Build 22000 - 22482) -- create/remove indices best-effort
    {{0xB2F925B9, 0x5A0F, 0x4D2E, {0x9F, 0x4D, 0x2B, 0x15, 0x07, 0x59, 0x3C, 0x10}},
     {0x536D3495, 0xB208, 0x4CC9, {0xAE, 0x26, 0xDE, 0x81, 0x11, 0x27, 0x5B, 0xF8}},
     true, 10, 11},

    // [3] Windows 11 (Build 22621/22631/23H2)
    {{0xA3175F2D, 0x239C, 0x4BD2, {0x8A, 0xA0, 0xEE, 0xBA, 0x8B, 0x0B, 0x13, 0x8E}},
     {0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}},
     false, 10, 12},

    // [4] Windows 11 (Build 26100+ / 24H2, 25H2)
    {{0x53F5CA0B, 0x158F, 0x4124, {0x90, 0x0C, 0x05, 0x71, 0x58, 0x06, 0x0B, 0x27}},
     {0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}},
     false, 11, 13},
};

static int g_windowsVersionIndex = 4;

//=============================================================================
// COM Interface Definitions (undocumented, reverse-engineered)
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
static HHOOK g_keyboardHook = nullptr;

static bool g_switchToDesktopOnMove = true;
static bool g_autoCreateDesktop = true;
static bool g_autoRemoveEmptyDesktop = true;
static bool g_hideDesktopSwitchLabel = true;
static int g_winReleaseDelayMs = 500;  // TEMP tuning knob, see WinReleaseDelayMs

static HWINEVENTHOOK g_desktopLabelHook = nullptr;
static volatile DWORD g_desktopLabelSuppressUntilTick = 0;
// The overlay window is reused across switches rather than recreated, so
// once we've seen it we can suppress it proactively before the next switch
// even starts, instead of only reacting after a fresh SHOW event.
static HWND g_knownDesktopLabelHwnd = nullptr;

#define WM_APP_SWITCH_DESKTOP (WM_APP + 1)
#define WM_APP_MOVE_WINDOW_TO_DESKTOP (WM_APP + 2)
static constexpr ULONG_PTR kInjectedKeyMarker = 0x57494E4Eu;  // 'WINN', tags our own synthetic input

static bool g_digitKeyDown[10] = {};
static bool g_digitHandledSinceWinDown = false;
// After we swallow the Win key-up (see LowLevelKeyboardProc), Windows' own
// key-state table never learns Win was released, so GetAsyncKeyState(VK_LWIN)
// would report "still down" forever. Rather than reject GetAsyncKeyState and
// track Win state by hand (which brings back its own fragility - see git
// history), we schedule a synthetic key-up a bit later to force that table
// back in sync ourselves. The delay is long enough that Explorer's own
// solo-tap detection (which only cares about near-instant timing right at
// the real key-up) won't mistake it for a fresh tap.
static volatile DWORD g_pendingWinReleaseTick = 0;
static volatile WORD g_pendingWinReleaseVk = 0;

//=============================================================================
// Helper Functions
//=============================================================================

template <typename T>
T GetVTableFunction(void* pInterface, int index) {
  return reinterpret_cast<T>((*reinterpret_cast<void***>(pInterface))[index]);
}

bool InitializeVirtualDesktopAPI();
bool UsesHMonitorParameter() { return g_versionIIDs[g_windowsVersionIndex].usesHMonitor; }

// Reads HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\CurrentBuildNumber and
// maps it to a g_versionIIDs bucket.
int DetectWindowsVersionIndex() {
  int result = 4;
  HKEY hKey;
  if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0,
                     KEY_READ, &hKey) == ERROR_SUCCESS) {
    WCHAR buf[32] = {};
    DWORD size = sizeof(buf);
    if (RegQueryValueExW(hKey, L"CurrentBuildNumber", nullptr, nullptr, (BYTE*)buf, &size) ==
        ERROR_SUCCESS) {
      int build = _wtoi(buf);
      Wh_Log(L"Detected Windows build: %d", build);
      if (build < 20348) result = 0;
      else if (build < 22000) result = 1;
      else if (build < 22621) result = 2;
      else if (build < 26100) result = 3;
      else result = 4;
    }
    RegCloseKey(hKey);
  }
  return result;
}

// UIPI blocks a lower-integrity process's WH_KEYBOARD_LL hook from seeing
// keystrokes while a higher-integrity (elevated) window has focus. If this
// mod's process isn't elevated, Win+N silently falls back to the default
// taskbar behavior whenever an admin app (Task Manager, an elevated
// terminal, Docker Desktop, etc.) is focused. Run Windhawk itself as
// Administrator to fix this system-wide.
bool IsProcessElevated() {
  bool elevated = false;
  HANDLE token = nullptr;
  if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
    TOKEN_ELEVATION elevation;
    DWORD size = sizeof(elevation);
    if (GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size)) {
      elevated = elevation.TokenIsElevated != 0;
    }
    CloseHandle(token);
  }
  return elevated;
}

int ParseWindowsVersion(PCWSTR str) {
  if (!str) return -1;
  static const std::pair<PCWSTR, int> kVersionMap[] = {
      {L"auto", -1},        {L"win10_old", 0},   {L"win10_20348", 1},
      {L"win11_22000", 2}, {L"win11_22621", 3}, {L"win11_26100", 4}};
  for (const auto& kv : kVersionMap) {
    if (wcscmp(str, kv.first) == 0) return kv.second;
  }
  return -1;
}

void LoadSettings() {
  PCWSTR version = Wh_GetStringSetting(L"WindowsVersion");
  g_windowsVersionIndex = ParseWindowsVersion(version);
  Wh_FreeStringSetting(version);
  if (g_windowsVersionIndex < 0) {
    g_windowsVersionIndex = DetectWindowsVersionIndex();
  }
  Wh_Log(L"Using Windows version bucket index: %d", g_windowsVersionIndex);

  g_switchToDesktopOnMove = Wh_GetIntSetting(L"SwitchToDesktopOnMove") != 0;
  g_autoCreateDesktop = Wh_GetIntSetting(L"AutoCreateDesktop") != 0;
  g_autoRemoveEmptyDesktop = Wh_GetIntSetting(L"AutoRemoveEmptyDesktop") != 0;
  g_hideDesktopSwitchLabel = Wh_GetIntSetting(L"HideDesktopSwitchLabel") != 0;
  g_winReleaseDelayMs = Wh_GetIntSetting(L"WinReleaseDelayMs");
}

//=============================================================================
// Virtual Desktop API Initialization
//=============================================================================

bool InitializeVirtualDesktopAPIOnce() {
  HRESULT hr = CoCreateInstance(CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER,
                                 IID_IServiceProvider, (void**)&g_pServiceProvider);
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

  hr = g_pServiceProvider->QueryService(IID_IApplicationViewCollection,
                                        IID_IApplicationViewCollection, (void**)&g_pViewCollection);
  if (FAILED(hr) || !g_pViewCollection) {
    Wh_Log(L"Failed to get ApplicationViewCollection: 0x%08X", hr);
    SAFE_RELEASE(g_pDesktopManagerInternal);
    SAFE_RELEASE(g_pServiceProvider);
    return false;
  }

  hr = CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_INPROC_SERVER,
                        IID_IVirtualDesktopManager, (void**)&g_pDesktopManager);
  if (FAILED(hr) || !g_pDesktopManager) {
    Wh_Log(L"Failed to create VirtualDesktopManager: 0x%08X", hr);
    SAFE_RELEASE(g_pViewCollection);
    SAFE_RELEASE(g_pDesktopManagerInternal);
    SAFE_RELEASE(g_pServiceProvider);
    return false;
  }

  // Optional - don't fail if unavailable, only used to ignore pinned windows.
  hr = g_pServiceProvider->QueryService(CLSID_VirtualDesktopPinnedApps, IID_IVirtualDesktopPinnedApps,
                                        (void**)&g_pPinnedApps);
  if (FAILED(hr)) {
    Wh_Log(L"PinnedApps service not available: 0x%08X", hr);
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

template <typename TResult>
HRESULT CallManagerInternal(int vtableIndex, TResult* outResult) {
  if (UsesHMonitorParameter()) {
    auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HMONITOR, TResult*)>(
        g_pDesktopManagerInternal, vtableIndex);
    HRESULT hr = pfn(g_pDesktopManagerInternal, nullptr, outResult);
    if (FAILED(hr) && ReinitializeVirtualDesktopAPI()) {
      pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HMONITOR, TResult*)>(
          g_pDesktopManagerInternal, vtableIndex);
      hr = pfn(g_pDesktopManagerInternal, nullptr, outResult);
    }
    return hr;
  } else {
    auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, TResult*)>(
        g_pDesktopManagerInternal, vtableIndex);
    HRESULT hr = pfn(g_pDesktopManagerInternal, outResult);
    if (FAILED(hr) && ReinitializeVirtualDesktopAPI()) {
      pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, TResult*)>(g_pDesktopManagerInternal,
                                                                            vtableIndex);
      hr = pfn(g_pDesktopManagerInternal, outResult);
    }
    return hr;
  }
}

HRESULT CallManagerInternalWithArg(int vtableIndex, IVirtualDesktop* arg) {
  if (UsesHMonitorParameter()) {
    auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HMONITOR, IVirtualDesktop*)>(
        g_pDesktopManagerInternal, vtableIndex);
    return pfn(g_pDesktopManagerInternal, nullptr, arg);
  } else {
    auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, IVirtualDesktop*)>(
        g_pDesktopManagerInternal, vtableIndex);
    return pfn(g_pDesktopManagerInternal, arg);
  }
}

HRESULT CallCreateDesktop(IVirtualDesktop** outDesktop) {
  int idx = g_versionIIDs[g_windowsVersionIndex].createDesktopIndex;
  if (UsesHMonitorParameter()) {
    auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HMONITOR, IVirtualDesktop**)>(
        g_pDesktopManagerInternal, idx);
    return pfn(g_pDesktopManagerInternal, nullptr, outDesktop);
  } else {
    auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, IVirtualDesktop**)>(
        g_pDesktopManagerInternal, idx);
    return pfn(g_pDesktopManagerInternal, outDesktop);
  }
}

HRESULT CallRemoveDesktop(IVirtualDesktop* toRemove, IVirtualDesktop* fallback) {
  int idx = g_versionIIDs[g_windowsVersionIndex].removeDesktopIndex;
  auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, IVirtualDesktop*, IVirtualDesktop*)>(
      g_pDesktopManagerInternal, idx);
  return pfn(g_pDesktopManagerInternal, toRemove, fallback);
}

//=============================================================================
// Virtual Desktop Operations
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
    if (SUCCEEDED(desktops->GetAt(i, g_versionIIDs[g_windowsVersionIndex].virtualDesktop,
                                   (void**)&desktop)) &&
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

IVirtualDesktop* CreateNewDesktop() {
  if (!g_pDesktopManagerInternal) return nullptr;
  IVirtualDesktop* desktop = nullptr;
  HRESULT hr = CallCreateDesktop(&desktop);
  if (FAILED(hr) || !desktop) {
    Wh_Log(L"CreateDesktop failed: 0x%08X", hr);
    return nullptr;
  }
  return desktop;
}

bool RemoveDesktopInternal(IVirtualDesktop* toRemove, IVirtualDesktop* fallback) {
  if (!g_pDesktopManagerInternal || !toRemove || !fallback) return false;
  HRESULT hr = CallRemoveDesktop(toRemove, fallback);
  if (FAILED(hr)) Wh_Log(L"RemoveDesktop failed: 0x%08X", hr);
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

bool EnsureDesktopCount(int n) {
  IObjectArray* desktops = GetDesktops();
  if (!desktops) return false;
  UINT count = 0;
  desktops->GetCount(&count);
  desktops->Release();

  while ((int)count < n) {
    IVirtualDesktop* created = CreateNewDesktop();
    if (!created) return false;
    created->Release();
    ++count;
    Wh_Log(L"Auto-created desktop #%u", count);
  }
  return true;
}

bool IsEligibleWindow(HWND hwnd) {
  if (!hwnd || !IsWindow(hwnd)) return false;
  LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
  if (!(style & WS_VISIBLE) || (style & WS_CHILD)) return false;
  if (GetWindowLongPtr(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) return false;
  return GetAncestor(hwnd, GA_ROOTOWNER) == hwnd;
}

bool IsWindowPinned(HWND hwnd) {
  if (!hwnd || !g_pViewCollection || !g_pPinnedApps) return false;

  IApplicationView* view = nullptr;
  if (FAILED(g_pViewCollection->GetViewForHwnd(hwnd, &view)) || !view) return false;

  BOOL isPinned = FALSE;
  HRESULT hr = g_pPinnedApps->IsViewPinned(view, &isPinned);
  view->Release();
  return SUCCEEDED(hr) && isPinned;
}

bool IsDesktopEmpty(const GUID& desktopId) {
  struct Ctx {
    GUID id;
    bool empty;
  } ctx{desktopId, true};

  EnumWindows(
      [](HWND hwnd, LPARAM lParam) WINAPI -> BOOL {
        auto* ctx = reinterpret_cast<Ctx*>(lParam);
        if (!IsEligibleWindow(hwnd) || IsWindowPinned(hwnd)) return TRUE;

        GUID windowDesktopId;
        if (g_pDesktopManager && SUCCEEDED(g_pDesktopManager->GetWindowDesktopId(hwnd, &windowDesktopId))) {
          if (IsEqualGUID(windowDesktopId, ctx->id)) {
            ctx->empty = false;
            return FALSE;
          }
        }
        return TRUE;
      },
      reinterpret_cast<LPARAM>(&ctx));

  return ctx.empty;
}

// Topmost (Z-order) eligible window that lives on the given desktop, if any.
HWND FindWindowOnDesktop(const GUID& desktopId) {
  struct Ctx {
    GUID id;
    HWND result;
  } ctx{desktopId, nullptr};

  EnumWindows(
      [](HWND hwnd, LPARAM lParam) WINAPI -> BOOL {
        auto* ctx = reinterpret_cast<Ctx*>(lParam);
        if (!IsEligibleWindow(hwnd)) return TRUE;

        GUID windowDesktopId;
        if (g_pDesktopManager && SUCCEEDED(g_pDesktopManager->GetWindowDesktopId(hwnd, &windowDesktopId))) {
          if (IsEqualGUID(windowDesktopId, ctx->id)) {
            ctx->result = hwnd;
            return FALSE;
          }
        }
        return TRUE;
      },
      reinterpret_cast<LPARAM>(&ctx));

  return ctx.result;
}

// Sends a harmless synthetic keystroke, tagged so our own hook ignores it.
// Besides letting us neutralize specific key effects (see call sites), this
// is also what makes ForceForegroundWindow's SetForegroundWindow call
// actually take effect: per MSDN, a process qualifies to set the foreground
// window if it "received the last input event ... by using SendInput".
void SendMarkedSyntheticKeystroke(WORD vk) {
  INPUT inputs[2] = {};
  inputs[0].type = INPUT_KEYBOARD;
  inputs[0].ki.wVk = vk;
  inputs[0].ki.dwExtraInfo = kInjectedKeyMarker;
  inputs[1] = inputs[0];
  inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
  SendInput(2, inputs, sizeof(INPUT));
}

// Releases a key we previously let Windows believe is still held (see the
// swallowed Win key-up in LowLevelKeyboardProc), without pressing it down
// first - we only need to correct the stuck "down" state, not simulate a
// fresh press. Marked so our own hook lets it flow straight through.
void SendMarkedSyntheticKeyUp(WORD vk) {
  INPUT input = {};
  input.type = INPUT_KEYBOARD;
  input.ki.wVk = vk;
  input.ki.dwFlags = KEYEVENTF_KEYUP;
  input.ki.dwExtraInfo = kInjectedKeyMarker;
  SendInput(1, &input, sizeof(INPUT));
}

// Even when SetForegroundWindow succeeds, Windows can still play a one-shot
// taskbar flash as a "courtesy" notification since the change came from a
// background process rather than direct user input. FLASHW_STOP cancels
// any flash state (pending or in progress) on a window's taskbar button.
void StopWindowFlash(HWND hwnd) {
  if (!hwnd || !IsWindow(hwnd)) return;
  FLASHWINFO fi = {};
  fi.cbSize = sizeof(fi);
  fi.hwnd = hwnd;
  fi.dwFlags = FLASHW_STOP;
  FlashWindowEx(&fi);
}

// A background process normally can't steal foreground focus - Windows just
// leaves the window looking active (on top, maximized) without it actually
// holding real input focus. If you then switch away without ever having
// clicked into it, Windows treats it as still "waiting" and flashes its
// taskbar button, which doesn't clear until you click back to it. Actually
// taking real foreground/input focus here (not just Z-order) prevents that.
void ForceForegroundWindow(HWND hwnd) {
  if (!hwnd || !IsWindow(hwnd)) return;

  if (IsIconic(hwnd)) {
    ShowWindow(hwnd, SW_RESTORE);
  }

  SendMarkedSyntheticKeystroke(VK_CONTROL);

  HWND currentForeground = GetForegroundWindow();
  DWORD foregroundThreadId = currentForeground ? GetWindowThreadProcessId(currentForeground, nullptr) : 0;
  DWORD currentThreadId = GetCurrentThreadId();

  bool attached = foregroundThreadId && foregroundThreadId != currentThreadId &&
                  AttachThreadInput(foregroundThreadId, currentThreadId, TRUE);

  SetForegroundWindow(hwnd);
  BringWindowToTop(hwnd);
  SetFocus(hwnd);

  if (attached) {
    AttachThreadInput(foregroundThreadId, currentThreadId, FALSE);
  }

  // Cancel any courtesy flash Windows might still queue on either side of
  // the switch. Explorer can schedule its own flash slightly after our
  // SwitchDesktop call returns, so a single immediate FLASHW_STOP can lose
  // the race - retry briefly to reliably catch it.
  for (int i = 0; i < 3; ++i) {
    StopWindowFlash(hwnd);
    if (currentForeground && currentForeground != hwnd) {
      StopWindowFlash(currentForeground);
    }
    Sleep(50);
  }
}

// The "Desktop N" name overlay (on Windows 11 builds with the experimental
// desktop-switch animation enabled) is a XamlExplorerHostIslandWindow hosted
// by explorer.exe. That window class is shared by several unrelated shell
// flyouts (volume, network, etc.), so instead of hiding it unconditionally,
// only hide instances that show up within a brief window right after a
// switch we ourselves triggered - identified live via SetWinEventHook while
// testing (see conversation), not via static reverse engineering.
void SuppressDesktopLabelWindow(HWND hwnd) {
  if (!hwnd || !IsWindow(hwnd)) return;
  // No blocking retry loop here on purpose: this runs on the same thread
  // that processes hotkey messages (including the next desktop switch), so
  // a Sleep-based retry loop stalls that thread and feels like a freeze on
  // fast repeated switches. A single fast hide is enough in practice since
  // the overlay's own lifecycle already generates several separate events
  // (create/show/hide/location-change) per switch, each of which re-enters
  // this function fresh - that gives repeated hide attempts spread over
  // time for free, without blocking anything ourselves.
  ShowWindow(hwnd, SW_HIDE);
  SetWindowPos(hwnd, nullptr, -32000, -32000, 0, 0,
               SWP_HIDEWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);
}

void CALLBACK DesktopSwitchLabelWinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject,
                                              LONG idChild, DWORD, DWORD) {
  if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !hwnd) {
    return;
  }

  bool withinSuppressWindow = (LONG)(GetTickCount() - g_desktopLabelSuppressUntilTick) <= 0;

  WCHAR className[64] = {};
  GetClassNameW(hwnd, className, ARRAYSIZE(className));
  bool isLabelClass = wcscmp(className, L"XamlExplorerHostIslandWindow") == 0;

  if (isLabelClass) {
    g_knownDesktopLabelHwnd = hwnd;  // remember it for next time, regardless of timing
  }

  if (isLabelClass && withinSuppressWindow) {
    SuppressDesktopLabelWindow(hwnd);
  }
}

void StartDesktopSwitchLabelWatcher() {
  HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
  DWORD explorerPid = 0;
  if (taskbar) {
    GetWindowThreadProcessId(taskbar, &explorerPid);
  }
  // Cover the whole object-visibility/lifecycle range (create, destroy, show,
  // hide, reorder, focus, selection*, state change, location change) since
  // the overlay's animation may re-assert visibility/position after the
  // initial show, not just fire a single EVENT_OBJECT_SHOW.
  g_desktopLabelHook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
                                       DesktopSwitchLabelWinEventProc, explorerPid, 0,
                                       WINEVENT_OUTOFCONTEXT);
  if (!g_desktopLabelHook) {
    Wh_Log(L"Failed to install desktop switch label watcher");
  }
}

void StopDesktopSwitchLabelWatcher() {
  if (g_desktopLabelHook) {
    UnhookWinEvent(g_desktopLabelHook);
    g_desktopLabelHook = nullptr;
  }
}

bool GoToDesktopNum(int desktopNum) {
  if (!InitializeVirtualDesktopAPI() || desktopNum < 1 || desktopNum > 9) return false;

  if (g_hideDesktopSwitchLabel) {
    // Arm this before EnsureDesktopCount, not just before SwitchToDesktop:
    // creating a new desktop can itself trigger the overlay, before any
    // actual switch happens.
    g_desktopLabelSuppressUntilTick = GetTickCount() + 2000;
    // The overlay window is reused across switches - if we've seen it
    // before, hide it right now too, instead of waiting for the
    // WinEventHook notification to arrive after Explorer shows it again.
    if (g_knownDesktopLabelHwnd) {
      ShowWindow(g_knownDesktopLabelHwnd, SW_HIDE);
      SetWindowPos(g_knownDesktopLabelHwnd, nullptr, -32000, -32000, 0, 0,
                   SWP_HIDEWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);
    }
  }

  if (g_autoCreateDesktop && !EnsureDesktopCount(desktopNum)) {
    Wh_Log(L"Failed to create desktop(s) up to %d", desktopNum);
  }

  GUID oldDesktopId = {};
  bool hasOldId = GetCurrentDesktopId(&oldDesktopId);
  int oldIndex = hasOldId ? GetDesktopIndexById(oldDesktopId) : -1;

  IVirtualDesktop* targetDesktop = GetDesktopByIndex(desktopNum - 1);
  if (!targetDesktop) {
    Wh_Log(L"Desktop %d not available (not created?)", desktopNum);
    return false;
  }

  GUID targetId = {};
  bool hasTargetId = SUCCEEDED(targetDesktop->GetId(&targetId));

  if (hasOldId && hasTargetId && IsEqualGUID(oldDesktopId, targetId)) {
    targetDesktop->Release();
    return true;  // already there
  }

  // Hand focus to Explorer's own taskbar window right before switching.
  // SwitchDesktop tries to restore focus to a window on the target desktop
  // internally; if that attempt happens while a random background process
  // (us) holds the "last input" token, it gets denied and Explorer falls
  // back to flashing a taskbar button instead. Doing it in the shell's own
  // foreground context avoids that. We reclaim real focus for the actual
  // target window ourselves right after (see ForceForegroundWindow below).
  SendMarkedSyntheticKeystroke(VK_CONTROL);
  HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
  if (taskbar) {
    SetForegroundWindow(taskbar);
  }

  bool switched = SwitchToDesktop(targetDesktop);

  if (switched) {
    Wh_Log(L"Switched to desktop %d", desktopNum);

    if (hasTargetId) {
      HWND toFocus = FindWindowOnDesktop(targetId);
      if (toFocus) {
        ForceForegroundWindow(toFocus);
      }
    }

    if (g_autoRemoveEmptyDesktop && hasOldId && oldIndex >= 0 && IsDesktopEmpty(oldDesktopId)) {
      IVirtualDesktop* oldDesktop = GetDesktopByIndex(oldIndex);
      if (oldDesktop) {
        GUID checkId;
        if (SUCCEEDED(oldDesktop->GetId(&checkId)) && IsEqualGUID(checkId, oldDesktopId)) {
          if (RemoveDesktopInternal(oldDesktop, targetDesktop)) {
            Wh_Log(L"Removed empty desktop (was #%d)", oldIndex + 1);
          }
        }
        oldDesktop->Release();
      }
    }
  }

  targetDesktop->Release();
  return switched;
}

// Moves the currently focused window to desktop N. By default also switches
// the view to desktop N right after (see SwitchToDesktopOnMove) - turn that
// setting off to keep the view on the current desktop instead.
bool MoveActiveWindowToDesktopNum(int desktopNum) {
  if (!InitializeVirtualDesktopAPI() || desktopNum < 1 || desktopNum > 9) return false;

  if (g_autoCreateDesktop && !EnsureDesktopCount(desktopNum)) {
    Wh_Log(L"Failed to create desktop(s) up to %d", desktopNum);
  }

  HWND hwnd = GetForegroundWindow();
  if (!hwnd || !IsEligibleWindow(hwnd)) {
    Wh_Log(L"No eligible active window to move");
    return false;
  }

  IVirtualDesktop* targetDesktop = GetDesktopByIndex(desktopNum - 1);
  if (!targetDesktop) {
    Wh_Log(L"Desktop %d not available (not created?)", desktopNum);
    return false;
  }

  IApplicationView* view = nullptr;
  if (!g_pViewCollection || FAILED(g_pViewCollection->GetViewForHwnd(hwnd, &view)) || !view) {
    Wh_Log(L"Failed to get view for active window");
    targetDesktop->Release();
    return false;
  }

  bool moved = MoveViewToDesktop(view, targetDesktop);
  if (moved) {
    Wh_Log(L"Moved active window to desktop %d", desktopNum);
  }

  view->Release();
  targetDesktop->Release();

  if (moved && g_switchToDesktopOnMove) {
    GoToDesktopNum(desktopNum);
  }

  return moved;
}

//=============================================================================
// Low-level keyboard hook: intercept Win+1..9 before Explorer's taskbar
// shortcut handling sees them.
//=============================================================================

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
  if (nCode == HC_ACTION) {
    KBDLLHOOKSTRUCT* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

    if (kb->dwExtraInfo != kInjectedKeyMarker) {
      bool isDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
      bool isUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

      // Explorer's "Start Menu on a solo Win tap" detection reacts to the
      // physical Win key-up itself. If Win and the digit are pressed in very
      // fast succession, our SendMarkedSyntheticKeystroke below can lose the
      // race against it - the synthetic key has to round-trip through
      // SendInput and get re-delivered to the hook chain, which can arrive
      // after Explorer's own check already ran off the real Win key-up.
      // Swallowing the Win key-up itself whenever we've already handled a
      // digit during this hold sidesteps the race entirely: Explorer never
      // even sees the key-up event that triggers that check. This does leave
      // Windows' own key-state table thinking Win is still down, so we
      // schedule a synthetic release a bit later (see HotkeyThreadProc) to
      // put it back in sync - by then the moment Explorer's detector cares
      // about has long passed.
      if (isUp && (kb->vkCode == VK_LWIN || kb->vkCode == VK_RWIN) && g_digitHandledSinceWinDown) {
        g_digitHandledSinceWinDown = false;
        g_pendingWinReleaseVk = (WORD)kb->vkCode;
        g_pendingWinReleaseTick = GetTickCount() + g_winReleaseDelayMs;
        return 1;
      }

      bool winHeld = (GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000);
      bool shiftHeld = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;

      if (winHeld && kb->vkCode >= '1' && kb->vkCode <= '9') {
        int digit = kb->vkCode - '0';
        if (isDown) {
          if (!g_digitKeyDown[digit]) {
            g_digitKeyDown[digit] = true;
            g_digitHandledSinceWinDown = true;
            SendMarkedSyntheticKeystroke(VK_CONTROL);  // extra safety net alongside the swallow above
            if (g_threadId) {
              PostThreadMessage(g_threadId,
                                 shiftHeld ? WM_APP_MOVE_WINDOW_TO_DESKTOP : WM_APP_SWITCH_DESKTOP,
                                 (WPARAM)digit, 0);
            }
          }
          // swallow: prevents Explorer's default Win(+Shift)+N taskbar behavior
          return 1;
        } else if (isUp) {
          g_digitKeyDown[digit] = false;
          return 1;
        }
      }
    }
  }
  return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

//=============================================================================
// Hotkey / message thread
//=============================================================================

DWORD WINAPI HotkeyThreadProc(LPVOID) {
  g_threadId = GetCurrentThreadId();
  Wh_Log(L"Thread started, thread ID: %lu", g_threadId);

  HRESULT coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
  Wh_Log(L"CoInitializeEx result: 0x%08X", coHr);

  MSG msg;
  PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);
  SetEvent(g_hReadyEvent);

  if (!InitializeVirtualDesktopAPI()) {
    Wh_Log(L"Virtual Desktop API failed to initialize on startup");
  }

  g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(nullptr), 0);
  if (!g_keyboardHook) {
    Wh_Log(L"Failed to install keyboard hook: %lu", GetLastError());
  }

  StartDesktopSwitchLabelWatcher();

  while (!g_stopHotkeyThread) {
    DWORD waitResult = MsgWaitForMultipleObjects(0, nullptr, FALSE, 100, QS_ALLINPUT);

    if (waitResult == WAIT_OBJECT_0) {
      while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
          goto cleanup;
        }
        if (msg.message == WM_APP_SWITCH_DESKTOP) {
          int digit = (int)msg.wParam;
          if (!g_bInitialized && !InitializeVirtualDesktopAPI()) {
            Wh_Log(L"Desktop switch ignored: API not initialized");
          } else {
            GoToDesktopNum(digit);
          }
        } else if (msg.message == WM_APP_MOVE_WINDOW_TO_DESKTOP) {
          int digit = (int)msg.wParam;
          if (!g_bInitialized && !InitializeVirtualDesktopAPI()) {
            Wh_Log(L"Move-to-desktop ignored: API not initialized");
          } else {
            MoveActiveWindowToDesktopNum(digit);
          }
        }
      }
    }

    if (g_pendingWinReleaseTick != 0 && (LONG)(GetTickCount() - g_pendingWinReleaseTick) >= 0) {
      SendMarkedSyntheticKeyUp(g_pendingWinReleaseVk);
      g_pendingWinReleaseTick = 0;
    }
  }

cleanup:
  if (g_keyboardHook) {
    UnhookWindowsHookEx(g_keyboardHook);
    g_keyboardHook = nullptr;
  }
  StopDesktopSwitchLabelWatcher();

  CleanupVirtualDesktopAPI();
  CoUninitialize();
  return 0;
}

bool StartHotkeyThread() {
  g_hReadyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
  g_hThread = CreateThread(nullptr, 0, HotkeyThreadProc, nullptr, 0, nullptr);

  if (!g_hThread || WaitForSingleObject(g_hReadyEvent, 5000) != WAIT_OBJECT_0) {
    Wh_Log(L"Failed to start thread");
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
  g_stopHotkeyThread = true;
  g_bInitialized = false;

  if (g_threadId) {
    PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
  }

  if (g_hThread) {
    DWORD waitResult = WaitForSingleObject(g_hThread, 5000);
    if (waitResult == WAIT_TIMEOUT) {
      Wh_Log(L"WARNING: thread cleanup timeout, thread may be stuck in CoUninitialize");
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
  Wh_Log(L"Win+Number Virtual Desktop Switch initializing...");
  if (!IsProcessElevated()) {
    Wh_Log(L"WARNING: not running elevated - Win+N will silently fall back to the "
           L"default taskbar behavior while an admin-elevated app has focus (UIPI). "
           L"Run Windhawk as Administrator to fix this.");
  }
  LoadSettings();
  if (!StartHotkeyThread()) {
    Wh_Log(L"Failed to start thread");
    return FALSE;
  }
  Wh_Log(L"Initialized successfully");
  return TRUE;
}

void WhTool_ModUninit() {
  Wh_Log(L"Uninitializing...");
  StopHotkeyThread();
  Wh_Log(L"Uninitialized");
}

void WhTool_ModSettingsChanged() {
  Wh_Log(L"Settings changed, reloading...");
  StopHotkeyThread();
  LoadSettings();
  if (!StartHotkeyThread()) {
    Wh_Log(L"Failed to restart thread after settings change");
  }
}

//=============================================================================
// Windhawk tool mod boilerplate for mods that run in a dedicated windhawk.exe
// process instead of injecting into other processes. Context:
// https://github.com/ramensoftware/windhawk-mods/pull/1916
//=============================================================================

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
  Wh_Log(L">");
  ExitThread(0);
}

BOOL Wh_ModInit() {
  DWORD sessionId;
  if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0) {
    return FALSE;
  }

  bool isExcluded = false;
  bool isToolModProcess = false;
  bool isCurrentToolModProcess = false;
  int argc;
  LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
  if (!argv) {
    Wh_Log(L"CommandLineToArgvW failed");
    return FALSE;
  }

  for (int i = 1; i < argc; i++) {
    if (wcscmp(argv[i], L"-service") == 0 || wcscmp(argv[i], L"-service-start") == 0 ||
        wcscmp(argv[i], L"-service-stop") == 0) {
      isExcluded = true;
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

  if (isExcluded) {
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

  if (Wh_GetIntSetting(L"RequestElevatedHelper")) {
    WCHAR params[64 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR))];
    wsprintfW(params, L"-tool-mod \"%s\"", WH_MOD_ID);

    SHELLEXECUTEINFOW sei = {};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = L"runas";
    sei.lpFile = currentProcessPath;
    sei.lpParameters = params;
    sei.nShow = SW_HIDE;

    // If Windhawk itself is already elevated, this launches without a prompt.
    // Otherwise it shows one UAC prompt so the helper can see keystrokes even
    // while an elevated app has focus (opt-in via RequestElevatedHelper).
    if (!ShellExecuteExW(&sei)) {
      DWORD err = GetLastError();
      if (err == ERROR_CANCELLED) {
        Wh_Log(L"Elevation declined - Win+N won't intercept while an elevated app has focus");
      } else {
        Wh_Log(L"ShellExecuteExW failed to launch elevated helper: %lu", err);
      }
      return;
    }
    if (sei.hProcess) {
      CloseHandle(sei.hProcess);
    }
    return;
  }

  WCHAR commandLine[MAX_PATH + 2 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
  wsprintfW(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

  HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
  if (!kernelModule) {
    kernelModule = GetModuleHandle(L"kernel32.dll");
    if (!kernelModule) {
      Wh_Log(L"No kernelbase.dll/kernel32.dll");
      return;
    }
  }

  using CreateProcessInternalW_t = BOOL(WINAPI*)(
      HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
      LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
      WINBOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
      LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation,
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
  if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine, nullptr, nullptr, FALSE,
                               NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi, nullptr)) {
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
