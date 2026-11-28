// ==WindhawkMod==
// @id              desktop-window-keys
// @name            Desktop Window Keys
// @description     Move the active window between virtual desktops (Ctrl+Win+Shift+Left/Right) and pin it to all desktops (Up/Down)
// @version         0.3.0
// @author          Anqarium
// @github          https://github.com/Anqarium
// @homepage        https://github.com/Anqarium/desktop-window-keys
// @include         windhawk.exe
// @include         windhawk-mod.exe
// @include         windhawk-mod-uiaccess.exe
// @compilerOptions -lole32 -loleaut32 -luuid -ldwmapi -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Window Keys

Moves the active window between virtual desktops and pins it to all desktops with the keyboard.

| Hotkey | Action |
|---|---|
| `Ctrl+Win+Shift+Left` | Move the active window to the previous desktop |
| `Ctrl+Win+Shift+Right` | Move the active window to the next desktop |
| `Ctrl+Win+Shift+Up` | Pin the active window to all desktops |
| `Ctrl+Win+Shift+Down` | Unpin the active window |

Pinning is the same as "Show this window on all desktops" in Task View (Win+Tab).
Up and Down only set the state, so pressing Up twice keeps the window pinned.
A window pinned with the hotkey gets a small dot in the Windows accent color in its
top-right corner, also when maximized or full screen. The dot ignores the mouse and
disappears when the window is unpinned (with Down or from Task View) or closed.
Windows pinned only from Task View get no dot.

Moving wraps around: Right on the last desktop goes to the first one, Left on the
first desktop goes to the last one (same as the Virtual Desktop Hotkey Cycle mod).

With **Follow Moved Window** enabled you switch to the target desktop together with
the window; otherwise only the window moves and you stay where you are. The slide
animation of that switch is available on Windows 11 24H2 and later only; on older builds
the switch is instant. When following a wrap, the switch goes through Explorer's own
Win+Ctrl+Left/Right so the slide animation keeps the pressed direction; this needs
Virtual Desktop Hotkey Cycle to be enabled, otherwise on 24H2 and later the switch falls
back to a slide in the opposite direction (instant on older builds).
Windows blocks injected keys to elevated (administrator) windows unless the mod runs
with UIAccess, which Windhawk 2.0 provides through the `windhawk-mod-uiaccess.exe`
host; on Windhawk 1.7.3 a wrap from an elevated window slides the opposite way.

If another program already owns a hotkey, the registration failure is written to the
Windhawk log.

Select your Windows version in the settings (run `winver` to check the build number);
the undocumented virtual desktop COM interfaces change IDs between Windows builds.

## How it differs from similar mods

* [Virtual Desktop Helper](https://windhawk.net/mods/virtual-desktop-helper) moves the
  active window only to a numbered desktop (1-9), and its previous/next hotkeys only
  switch desktops. Its pin hotkey is a toggle. This mod moves the window to the previous
  or next desktop with wrap-around, has separate pin and unpin hotkeys that set the state,
  and marks hotkey-pinned windows with a dot. It has no numbered-desktop hotkeys and its
  hotkeys are fixed.
* WinPager also has move-left/right hotkeys, but it always follows the window and
  supports Windows 11 24H2 and later only. This mod makes following optional and has
  interface IDs for Windows 10 and earlier Windows 11 builds (Windows Version setting).

## Credits

Based on the COM interface definitions, the Windows version IID table and the hotkey
thread of [Virtual Desktop Helper](https://github.com/ramensoftware/windhawk-mods/blob/dc9cd28419f63d2d86396bcb845cf8e307186d5e/mods/virtual-desktop-helper.wh.cpp)
by u2x1 (v2.5.0), itself based on
[VirtualDesktopAccessor](https://github.com/Ciantic/VirtualDesktopAccessor).
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
    - win11_22621: Windows 11 (Build 22621/22631 before .3085)
    - win11_22631_3085: Windows 11 (Build 22621/22631 from .3085 / current 23H2)
    - win11_26100: Windows 11 (Build 26100+ / 24H2)

- FollowMovedWindow: false
  $name: Follow Moved Window
  $description: Switch to the target desktop together with the moved window. When off, only the window moves.
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <objbase.h>
#include <objectarray.h>
#include <dwmapi.h>
#include <shobjidl.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <atomic>
#include <vector>

#define SAFE_RELEASE(p) \
  do {                  \
    if (p) {            \
      (p)->Release();   \
      (p) = nullptr;    \
    }                   \
  } while (0)

//=============================================================================
// COM CLSIDs and IIDs for the (undocumented) Virtual Desktop API.
// Reference: https://github.com/Ciantic/VirtualDesktopAccessor
//=============================================================================

// CLSID_ImmersiveShell: {C2F03A33-21F5-47FA-B4BB-156362A2F239}
static const CLSID CLSID_ImmersiveShell = {
    0xC2F03A33, 0x21F5, 0x47FA, {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39}};

// CLSID_VirtualDesktopManagerInternal: {C5E0CDCA-7B6E-41B2-9FC4-D93975CC467B}
static const CLSID CLSID_VirtualDesktopManagerInternal = {
    0xC5E0CDCA, 0x7B6E, 0x41B2, {0x9F, 0xC4, 0xD9, 0x39, 0x75, 0xCC, 0x46, 0x7B}};

// IID_IApplicationViewCollection: {1841C6D7-4F9D-42C0-AF41-8747538F10E5}
static const IID IID_IApplicationViewCollection = {
    0x1841C6D7, 0x4F9D, 0x42C0, {0xAF, 0x41, 0x87, 0x47, 0x53, 0x8F, 0x10, 0xE5}};

// CLSID_VirtualDesktopPinnedApps: {B5A399E7-1C87-46B8-88E9-FC5747B171BD}
// Same value on every supported build (Virtual Desktop Helper v2.5.0 uses one constant).
static const CLSID CLSID_VirtualDesktopPinnedApps = {
    0xB5A399E7, 0x1C87, 0x46B8, {0x88, 0xE9, 0xFC, 0x57, 0x47, 0xB1, 0x71, 0xBD}};

// IID_IVirtualDesktopPinnedApps: {4CE81583-1E4C-4632-A621-07A53543148F}
// Explicit constant: MinGW does not emit __uuidof for MIDL_INTERFACE (link error).
static const IID IID_IVirtualDesktopPinnedApps = {
    0x4CE81583, 0x1E4C, 0x4632, {0xA6, 0x21, 0x07, 0xA5, 0x35, 0x43, 0x14, 0x8F}};

// Microsoft changes these IIDs between Windows builds. usesHMonitor: some builds
// take an HMONITOR as the first argument of the manager methods.
// Source: Virtual Desktop Helper v2.5.0 (u2x1), VirtualDesktopAccessor.
// switchWithAnimationIndex: VTable index of SwitchDesktopWithAnimation, or -1 when
// unknown for that build (plain SwitchDesktop is used, which skips the animation).
// 24H2 (26100+) layout from MScholtes/VirtualDesktop VirtualDesktop11-24H2.cs (a725cbd)
// and VirtualDesktopAccessor (ceb6488): SwitchDesktopAndMoveForegroundView sits at 10,
// so SwitchDesktopWithAnimation is at 22. Do not guess indices for other builds: a
// wrong slot calls an unrelated method with mismatched arguments.
// 22621/22631 from .3085 (entry [5]) shares the 24H2 IIDs, so QueryService succeeds
// with either entry and a wrong setting is not detected. Its SwitchDesktopWithAnimation
// would be at 21 (no SwitchDesktopAndMoveForegroundView; MScholtes/VirtualDesktop
// VirtualDesktop11.cs, a725cbd), but that was never tried on a real 23H2 machine, so it
// stays -1 (plain SwitchDesktop) until verified. Slot 22 is used only on build 26100+
// (see GetSwitchWithAnimationIndex): on 23H2 it is GetLastActiveDesktop.
struct VersionIIDs {
  IID managerInternal;  // IVirtualDesktopManagerInternal
  IID virtualDesktop;   // IVirtualDesktop
  bool usesHMonitor;
  int switchWithAnimationIndex;
};

static const VersionIIDs g_versionIIDs[] = {
    // [0] Windows 10 (Build < 20348)
    {{0xF31574D6, 0xB682, 0x4CDC, {0xBD, 0x56, 0x18, 0x27, 0x86, 0x0A, 0xBE, 0xC6}},
     {0xFF72FFDD, 0xBE7E, 0x43FC, {0x9C, 0x03, 0xAD, 0x81, 0x68, 0x1E, 0x88, 0xE4}},
     false, -1},
    // [1] Windows 10 (Build 20348 - 21999)
    {{0x094AFE11, 0x44F2, 0x4BA0, {0x97, 0x6F, 0x29, 0xA9, 0x7E, 0x26, 0x3E, 0xE0}},
     {0x62FDF88B, 0x11CA, 0x4AFB, {0x8B, 0xD8, 0x22, 0x96, 0xDF, 0xAE, 0x49, 0xE2}},
     true, -1},
    // [2] Windows 11 (Build 22000 - 22482)
    {{0xB2F925B9, 0x5A0F, 0x4D2E, {0x9F, 0x4D, 0x2B, 0x15, 0x07, 0x59, 0x3C, 0x10}},
     {0x536D3495, 0xB208, 0x4CC9, {0xAE, 0x26, 0xDE, 0x81, 0x11, 0x27, 0x5B, 0xF8}},
     true, -1},
    // [3] Windows 11 (Build 22621/22631 before .3085)
    {{0xA3175F2D, 0x239C, 0x4BD2, {0x8A, 0xA0, 0xEE, 0xBA, 0x8B, 0x0B, 0x13, 0x8E}},
     {0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}},
     false, -1},
    // [4] Windows 11 (Build 26100+ / 24H2)
    {{0x53F5CA0B, 0x158F, 0x4124, {0x90, 0x0C, 0x05, 0x71, 0x58, 0x06, 0x0B, 0x27}},
     {0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}},
     false, 22},
    // [5] Windows 11 (Build 22621/22631 from .3085, current 23H2)
    {{0x53F5CA0B, 0x158F, 0x4124, {0x90, 0x0C, 0x05, 0x71, 0x58, 0x06, 0x0B, 0x27}},
     {0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}},
     false, -1},
};

// Default to Windows 11 24H2.
static int g_windowsVersionIndex = 4;

//=============================================================================
// COM interface definitions (reverse-engineered; VTable layout varies by build)
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

// VTable layout from Virtual Desktop Helper.
MIDL_INTERFACE("4CE81583-1E4C-4632-A621-07A53543148F")
IVirtualDesktopPinnedApps : public IUnknown {
  virtual HRESULT STDMETHODCALLTYPE IsAppIdPinned(LPCWSTR, BOOL*) = 0;
  virtual HRESULT STDMETHODCALLTYPE PinAppID(LPCWSTR) = 0;
  virtual HRESULT STDMETHODCALLTYPE UnpinAppID(LPCWSTR) = 0;
  virtual HRESULT STDMETHODCALLTYPE IsViewPinned(IApplicationView*, BOOL*) = 0;
  virtual HRESULT STDMETHODCALLTYPE PinView(IApplicationView*) = 0;
  virtual HRESULT STDMETHODCALLTYPE UnpinView(IApplicationView*) = 0;
};

// IVirtualDesktopManagerInternal VTable indices (IUnknown occupies 0-2).
static const int VTABLE_MOVE_VIEW_TO_DESKTOP = 4;
static const int VTABLE_GET_CURRENT_DESKTOP = 6;
static const int VTABLE_GET_DESKTOPS = 7;
static const int VTABLE_SWITCH_DESKTOP = 9;

//=============================================================================
// Global state
//=============================================================================

static IServiceProvider* g_pServiceProvider = nullptr;
static IVirtualDesktopManagerInternal* g_pDesktopManagerInternal = nullptr;
static IApplicationViewCollection* g_pViewCollection = nullptr;
static IVirtualDesktopPinnedApps* g_pPinnedApps = nullptr;  // optional: pin hotkeys only
static bool g_bInitialized = false;

static HANDLE g_hThread = nullptr;
static std::atomic<HWND> g_hMsgWnd = nullptr;
static std::atomic_bool g_hotkeyThreadRunning = false;
static std::atomic_bool g_settingsReloadPending = false;

static constexpr UINT WM_APP_SETTINGS_CHANGED = WM_APP + 1;
static constexpr wchar_t kHotkeyWindowClassName[] = L"WindhawkDesktopWindowKeysHotkeys";

static bool g_followMovedWindow = false;

// Pin badge (see "Pin badge" below). Sizes are at 96 DPI and scaled per window.
static constexpr wchar_t kBadgeWindowClassName[] = L"WindhawkDesktopWindowKeysBadge";
static constexpr COLORREF kFallbackBadgeColor = RGB(0, 120, 215);  // if the accent color is unavailable
static constexpr int kBadgeSizePx = 8;
static constexpr int kBadgeMarginPx = 6;
static constexpr BYTE kBadgeAlpha = 200;
static constexpr UINT_PTR kBadgeCheckTimerId = 1;
static constexpr UINT kBadgeCheckIntervalMs = 1000;

struct PinBadge {
  HWND target;  // the pinned window
  HWND badge;   // our dot window
  int size;     // current size in physical pixels
  bool zOrderDenied;  // target has a higher integrity level (UIPI): shown only while it is in front
};
static std::vector<PinBadge> g_badges;  // hotkey thread only
static constexpr int kBadgeHookCount = 5;
static HWINEVENTHOOK g_badgeHooks[kBadgeHookCount] = {};

enum HotkeyIds { HK_MOVE_PREV = 1, HK_MOVE_NEXT = 2, HK_PIN = 3, HK_UNPIN = 4 };

// Ctrl+Win+Shift, fixed by design (see README).
static constexpr UINT kHotkeyModifiers = MOD_CONTROL | MOD_WIN | MOD_SHIFT | MOD_NOREPEAT;

//=============================================================================
// Helpers
//=============================================================================

// Access a VTable function by index for the undocumented COM interfaces.
template <typename T>
T GetVTableFunction(void* pInterface, int index) {
  return reinterpret_cast<T>((*reinterpret_cast<void***>(pInterface))[index]);
}

bool InitializeVirtualDesktopAPI();
bool ReinitializeVirtualDesktopAPI();

bool UsesHMonitorParameter() { return g_versionIIDs[g_windowsVersionIndex].usesHMonitor; }

int ParseWindowsVersion(PCWSTR str) {
  static const struct {
    PCWSTR name;
    int index;
  } kVersionMap[] = {
      {L"win10_old", 0}, {L"win10_20348", 1}, {L"win11_22000", 2}, {L"win11_22621", 3}, {L"win11_26100", 4},
      {L"win11_22631_3085", 5}};
  for (const auto& entry : kVersionMap) {
    if (str && wcscmp(str, entry.name) == 0) return entry.index;
  }
  return 4;
}

void LoadSettings() {
  PCWSTR version = Wh_GetStringSetting(L"WindowsVersion");
  g_windowsVersionIndex = ParseWindowsVersion(version);
  Wh_FreeStringSetting(version);

  g_followMovedWindow = Wh_GetIntSetting(L"FollowMovedWindow") != 0;
  Wh_Log(L"LoadSettings: versionIndex=%d, follow=%d", g_windowsVersionIndex, g_followMovedWindow);
}

//=============================================================================
// Virtual Desktop API initialization
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
    Wh_Log(L"Failed to get VirtualDesktopManagerInternal (wrong Windows version setting?): 0x%08X", hr);
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

  // Optional: moving windows keeps working when only the pin service is missing.
  hr = g_pServiceProvider->QueryService(CLSID_VirtualDesktopPinnedApps, IID_IVirtualDesktopPinnedApps,
                                        (void**)&g_pPinnedApps);
  if (FAILED(hr)) {
    Wh_Log(L"PinnedApps service not available (pin hotkeys disabled): 0x%08X", hr);
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
  // If Explorer is gone, Release() on its COM objects can hang: drop the pointers instead.
  HWND hShell = GetShellWindow();
  if (!hShell || !IsWindow(hShell)) {
    Wh_Log(L"Explorer not available, skipping COM cleanup to avoid hang");
    g_pPinnedApps = nullptr;
    g_pViewCollection = nullptr;
    g_pDesktopManagerInternal = nullptr;
    g_pServiceProvider = nullptr;
    g_bInitialized = false;
    return;
  }

  SAFE_RELEASE(g_pPinnedApps);
  SAFE_RELEASE(g_pViewCollection);
  SAFE_RELEASE(g_pDesktopManagerInternal);
  SAFE_RELEASE(g_pServiceProvider);
  g_bInitialized = false;
}

bool ReinitializeVirtualDesktopAPI() {
  CleanupVirtualDesktopAPI();
  return InitializeVirtualDesktopAPI();
}

// The COM proxies point at the old explorer.exe after it restarts.
bool IsServerGone(HRESULT hr) {
  return hr == RPC_E_DISCONNECTED || hr == HRESULT_FROM_WIN32(RPC_S_SERVER_UNAVAILABLE) ||
         hr == HRESULT_FROM_WIN32(RPC_S_CALL_FAILED);
}

// Direct proxy calls (view collection, pinned apps) do not go through CallManagerInternal,
// so they reconnect on their own: retry once after the server was gone. Callers must
// re-check g_pPinnedApps afterwards, since a reconnect replaces it.
HRESULT GetViewForHwnd(HWND hwnd, IApplicationView** view) {
  if (!g_pViewCollection) return E_POINTER;
  HRESULT hr = g_pViewCollection->GetViewForHwnd(hwnd, view);
  if (IsServerGone(hr) && ReinitializeVirtualDesktopAPI() && g_pViewCollection) {
    hr = g_pViewCollection->GetViewForHwnd(hwnd, view);
  }
  return hr;
}

// VTable call for methods returning a value, with the optional HMONITOR parameter
// (varies by Windows version). Reinitializes once in case explorer.exe restarted.
template <typename TResult>
HRESULT CallManagerInternal(int vtableIndex, TResult* outResult) {
  for (int attempt = 0; attempt < 2; ++attempt) {
    HRESULT hr;
    if (UsesHMonitorParameter()) {
      auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HMONITOR, TResult*)>(g_pDesktopManagerInternal,
                                                                                           vtableIndex);
      hr = pfn(g_pDesktopManagerInternal, nullptr, outResult);
    } else {
      auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, TResult*)>(g_pDesktopManagerInternal,
                                                                                vtableIndex);
      hr = pfn(g_pDesktopManagerInternal, outResult);
    }
    if (SUCCEEDED(hr) || attempt == 1 || !ReinitializeVirtualDesktopAPI()) return hr;
  }
  return E_FAIL;
}

// Same, for methods taking one argument.
template <typename TArg>
HRESULT CallManagerInternalWithArg(int vtableIndex, TArg arg) {
  for (int attempt = 0; attempt < 2; ++attempt) {
    HRESULT hr;
    if (UsesHMonitorParameter()) {
      auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HMONITOR, TArg)>(g_pDesktopManagerInternal,
                                                                                      vtableIndex);
      hr = pfn(g_pDesktopManagerInternal, nullptr, arg);
    } else {
      auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, TArg)>(g_pDesktopManagerInternal,
                                                                            vtableIndex);
      hr = pfn(g_pDesktopManagerInternal, arg);
    }
    if (SUCCEEDED(hr) || attempt == 1 || !ReinitializeVirtualDesktopAPI()) return hr;
  }
  return E_FAIL;
}

//=============================================================================
// Virtual desktop operations
//=============================================================================

IObjectArray* GetDesktops() {
  if (!g_pDesktopManagerInternal) return nullptr;
  IObjectArray* desktops = nullptr;
  HRESULT hr = CallManagerInternal(VTABLE_GET_DESKTOPS, &desktops);
  if (FAILED(hr)) Wh_Log(L"GetDesktops failed: 0x%08X", hr);
  return desktops;
}

// Returns the desktop at |index| (caller releases), or nullptr.
IVirtualDesktop* GetDesktopByIndex(IObjectArray* desktops, UINT index) {
  IVirtualDesktop* desktop = nullptr;
  desktops->GetAt(index, g_versionIIDs[g_windowsVersionIndex].virtualDesktop, (void**)&desktop);
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

// Index of the desktop with |desktopId| in |desktops|, or -1.
int FindDesktopIndex(IObjectArray* desktops, UINT count, const GUID& desktopId) {
  for (UINT i = 0; i < count; ++i) {
    IVirtualDesktop* desktop = GetDesktopByIndex(desktops, i);
    if (!desktop) continue;
    GUID guid;
    bool match = SUCCEEDED(desktop->GetId(&guid)) && IsEqualGUID(guid, desktopId);
    desktop->Release();
    if (match) return static_cast<int>(i);
  }
  return -1;
}

// The real Windows build number. RtlGetVersion is not affected by the compatibility manifest.
DWORD GetWindowsBuild() {
  using RtlGetVersion_t = LONG(WINAPI*)(PRTL_OSVERSIONINFOW);
  HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
  auto pRtlGetVersion = ntdll ? reinterpret_cast<RtlGetVersion_t>(GetProcAddress(ntdll, "RtlGetVersion")) : nullptr;
  RTL_OSVERSIONINFOW info = {sizeof(info)};
  return pRtlGetVersion && pRtlGetVersion(&info) == 0 ? info.dwBuildNumber : 0;
}

// The animated-switch slot of the selected entry, or -1 when it does not match the
// real build. The 24H2 slot (22) is GetLastActiveDesktop on 23H2: the Windows Version
// setting can be wrong (or outdated after an upgrade) without any error, and calling
// the wrong slot corrupts memory.
int GetSwitchWithAnimationIndex() {
  int index = g_versionIIDs[g_windowsVersionIndex].switchWithAnimationIndex;
  if (index == 22 && GetWindowsBuild() < 26100) {
    Wh_Log(L"Animated switch slot is for 24H2 but this is an older build; using plain switch");
    return -1;
  }
  return index;
}

// Switches with the system slide animation when the build's VTable slot is known.
bool SwitchToDesktop(IVirtualDesktop* desktop) {
  int animationIndex = GetSwitchWithAnimationIndex();
  HRESULT hr = CallManagerInternalWithArg(animationIndex >= 0 ? animationIndex : VTABLE_SWITCH_DESKTOP, desktop);
  if (FAILED(hr)) Wh_Log(L"SwitchToDesktop failed: 0x%08X", hr);
  return SUCCEEDED(hr);
}

bool MoveViewToDesktop(IApplicationView* view, IVirtualDesktop* desktop) {
  auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, IApplicationView*, IVirtualDesktop*)>(
      g_pDesktopManagerInternal, VTABLE_MOVE_VIEW_TO_DESKTOP);
  HRESULT hr = pfn(g_pDesktopManagerInternal, view, desktop);
  if (FAILED(hr)) Wh_Log(L"MoveViewToDesktop failed: 0x%08X", hr);
  return SUCCEEDED(hr);
}

// Integrity level of a process token, or -1 when it cannot be read.
static DWORD GetTokenIntegrity(HANDLE token) {
  BYTE buffer[TOKEN_INTEGRITY_LEVEL_MAX_SIZE];
  DWORD size = 0;
  if (!GetTokenInformation(token, TokenIntegrityLevel, buffer, sizeof(buffer), &size)) return (DWORD)-1;
  PSID sid = reinterpret_cast<TOKEN_MANDATORY_LABEL*>(buffer)->Label.Sid;
  return *GetSidSubAuthority(sid, *GetSidSubAuthorityCount(sid) - 1);
}

// UIPI silently drops injected input aimed at a window of a higher integrity level
// (e.g. an elevated app) unless this process has UIAccess. SendInput still reports
// success, so check up front instead of waiting for a switch that never happens.
// A process whose token cannot be read is treated as higher (typically elevated).
bool CanInjectInputTo(HWND hwnd) {
  HANDLE ownToken = nullptr;
  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &ownToken)) return true;
  DWORD uiAccess = 0, size = 0;
  GetTokenInformation(ownToken, TokenUIAccess, &uiAccess, sizeof(uiAccess), &size);
  DWORD ownIntegrity = GetTokenIntegrity(ownToken);
  CloseHandle(ownToken);
  if (uiAccess) return true;

  DWORD pid = 0;
  GetWindowThreadProcessId(hwnd, &pid);
  HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
  if (!process) return false;
  HANDLE token = nullptr;
  bool canInject = false;
  if (OpenProcessToken(process, TOKEN_QUERY, &token)) {
    DWORD integrity = GetTokenIntegrity(token);
    canInject = integrity != (DWORD)-1 && integrity <= ownIntegrity;
    CloseHandle(token);
  }
  CloseHandle(process);
  return canInject;
}

// Explorer's own Win+Ctrl+Left/Right slides in the pressed direction, while COM's
// SwitchDesktopWithAnimation picks the direction from the desktop indices, so a wrap
// (last -> first) would slide left. For wraps we therefore inject the native shortcut;
// Explorer wraps it only when the Virtual Desktop Hotkey Cycle mod (or similar) is
// active. Returns true once the current desktop became |targetId|.
bool SwitchByNativeShortcut(int direction, const GUID& targetId) {
  auto isDown = [](int vk) { return (GetAsyncKeyState(vk) & 0x8000) != 0; };
  bool lShift = isDown(VK_LSHIFT), rShift = isDown(VK_RSHIFT);
  bool ctrl = isDown(VK_CONTROL);
  bool win = isDown(VK_LWIN) || isDown(VK_RWIN);

  INPUT inputs[10] = {};
  UINT n = 0;
  auto key = [&](WORD vk, bool up, bool extended = false) {
    inputs[n].type = INPUT_KEYBOARD;
    inputs[n].ki.wVk = vk;
    inputs[n].ki.dwFlags = (up ? KEYEVENTF_KEYUP : 0) | (extended ? KEYEVENTF_EXTENDEDKEY : 0);
    ++n;
  };

  // Our hotkey is still held: release Shift so the arrow reaches Explorer as
  // Win+Ctrl+Arrow instead of re-triggering Ctrl+Win+Shift+Arrow, then restore it so
  // the next press while holding the modifiers moves a window again.
  if (lShift) key(VK_LSHIFT, true);
  if (rShift) key(VK_RSHIFT, true);
  if (!ctrl) key(VK_LCONTROL, false);
  if (!win) key(VK_LWIN, false);
  WORD arrow = direction > 0 ? VK_RIGHT : VK_LEFT;
  key(arrow, false, true);
  key(arrow, true, true);
  if (!win) key(VK_LWIN, true);
  if (!ctrl) key(VK_LCONTROL, true);
  if (lShift) key(VK_LSHIFT, false);
  if (rShift) key(VK_RSHIFT, false);

  if (SendInput(n, inputs, sizeof(INPUT)) != n) {
    Wh_Log(L"SendInput failed: %lu", GetLastError());
    return false;
  }

  // Explorer handles the shortcut asynchronously; wait briefly for the switch.
  for (int i = 0; i < 20; ++i) {
    Sleep(15);
    GUID currentId;
    if (GetCurrentDesktopId(&currentId) && IsEqualGUID(currentId, targetId)) return true;
  }
  Wh_Log(L"Native shortcut did not wrap (Virtual Desktop Hotkey Cycle not active?)");
  return false;
}

void FocusWindow(HWND hwnd) {
  if (IsIconic(hwnd)) ShowWindow(hwnd, SW_RESTORE);
  SetForegroundWindow(hwnd);
}

void RemoveBadge(HWND target);

// Move the active window to the previous (direction = -1) or next (+1) desktop,
// wrapping around at both ends.
bool MoveActiveWindowByOffset(int direction) {
  if (!InitializeVirtualDesktopAPI()) return false;

  HWND hwnd = GetForegroundWindow();
  if (!hwnd) {
    Wh_Log(L"No active window to move");
    return false;
  }

  GUID currentId = {};
  if (!GetCurrentDesktopId(&currentId)) return false;

  IObjectArray* desktops = GetDesktops();
  if (!desktops) return false;

  UINT count = 0;
  desktops->GetCount(&count);
  int currentIndex = count ? FindDesktopIndex(desktops, count, currentId) : -1;
  if (currentIndex < 0) {
    Wh_Log(L"Current desktop not found among %u desktops", count);
    desktops->Release();
    return false;
  }
  if (count < 2) {
    desktops->Release();
    return true;  // Nothing to move to.
  }

  int targetIndex = (currentIndex + direction + static_cast<int>(count)) % static_cast<int>(count);
  IVirtualDesktop* target = GetDesktopByIndex(desktops, targetIndex);
  desktops->Release();
  if (!target) {
    Wh_Log(L"Target desktop %d not found", targetIndex + 1);
    return false;
  }

  IApplicationView* view = nullptr;
  bool success = SUCCEEDED(GetViewForHwnd(hwnd, &view)) && view && MoveViewToDesktop(view, target);
  SAFE_RELEASE(view);

  if (success) {
    Wh_Log(L"Moved window to desktop %d of %u", targetIndex + 1, count);
    RemoveBadge(hwnd);  // moving to one desktop unpins the window
    if (g_followMovedWindow) {
      bool wrapped = direction > 0 ? targetIndex == 0 : targetIndex == static_cast<int>(count) - 1;
      GUID targetId = {};
      bool injectable = wrapped && CanInjectInputTo(hwnd);
      if (wrapped && !injectable) {
        Wh_Log(L"Active window has a higher integrity level; wrap slides the opposite way");
      }
      bool switched = injectable && SUCCEEDED(target->GetId(&targetId)) &&
                      SwitchByNativeShortcut(direction, targetId);
      if (switched || SwitchToDesktop(target)) {
        FocusWindow(hwnd);
      }
    }
  } else {
    Wh_Log(L"Could not move the active window (hwnd=0x%p)", hwnd);
  }

  target->Release();
  return success;
}

// The Windows accent color (Settings > Personalization > Colors) as a COLORREF.
COLORREF GetAccentColor() {
  DWORD argb = 0;
  BOOL opaque = FALSE;
  if (FAILED(DwmGetColorizationColor(&argb, &opaque))) return kFallbackBadgeColor;
  // 0xAARRGGBB -> COLORREF (0x00BBGGRR); the alpha byte is dropped.
  return RGB((argb >> 16) & 0xFF, (argb >> 8) & 0xFF, argb & 0xFF);
}

//=============================================================================
// Pin badge: a small click-through accent-colored dot in the top-right corner of
// each window pinned with the hotkey. It is our own window kept directly above the
// pinned window in the Z-order, so it shows on maximized and full-screen windows
// too and stays hidden behind windows that cover the pinned one. Everything here
// runs on the hotkey thread.
//=============================================================================

// True only when the window is pinned; false also when it could not be determined.
bool IsWindowPinned(HWND hwnd) {
  for (int attempt = 0; attempt < 2; ++attempt) {
    if (!g_pPinnedApps || !g_pViewCollection) return false;
    IApplicationView* view = nullptr;
    if (FAILED(GetViewForHwnd(hwnd, &view)) || !view) return false;
    if (!g_pPinnedApps) {  // GetViewForHwnd reconnected and the pin service is gone
      view->Release();
      return false;
    }
    BOOL pinned = FALSE;
    HRESULT hr = g_pPinnedApps->IsViewPinned(view, &pinned);
    view->Release();
    if (SUCCEEDED(hr)) return pinned != FALSE;
    if (!IsServerGone(hr) || !ReinitializeVirtualDesktopAPI()) return false;
  }
  return false;
}

PinBadge* FindBadge(HWND target) {
  for (auto& badge : g_badges) {
    if (badge.target == target) return &badge;
  }
  return nullptr;
}

// Topmost must be set at creation: SetWindowPos(HWND_TOPMOST) on this window returns
// success but is silently ignored (verified with a test window).
HWND CreateBadgeWindow(bool topmost) {
  DWORD exStyle = WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
  if (topmost) exStyle |= WS_EX_TOPMOST;
  HWND badgeWnd = CreateWindowExW(exStyle, kBadgeWindowClassName, nullptr, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
                                  GetModuleHandle(nullptr), nullptr);
  if (!badgeWnd) {
    Wh_Log(L"Failed to create pin badge: %lu", GetLastError());
    return nullptr;
  }
  SetLayeredWindowAttributes(badgeWnd, 0, kBadgeAlpha, LWA_ALPHA);
  return badgeWnd;
}

void UpdateBadge(PinBadge& badge) {
  DWORD cloaked = 0;  // e.g. the window was moved to another desktop
  DwmGetWindowAttribute(badge.target, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
  if (cloaked || IsIconic(badge.target) || !IsWindowVisible(badge.target)) {
    ShowWindow(badge.badge, SW_HIDE);
    return;
  }

  // Extended frame bounds exclude the invisible resize borders of GetWindowRect.
  RECT rect;
  if (FAILED(DwmGetWindowAttribute(badge.target, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(rect)))) {
    GetWindowRect(badge.target, &rect);
  }

  UINT dpi = GetDpiForWindow(badge.target);
  if (!dpi) dpi = USER_DEFAULT_SCREEN_DPI;
  int size = MulDiv(kBadgeSizePx, dpi, USER_DEFAULT_SCREEN_DPI);
  int margin = MulDiv(kBadgeMarginPx, dpi, USER_DEFAULT_SCREEN_DPI);
  if (size != badge.size) {
    badge.size = size;
    SetWindowRgn(badge.badge, CreateEllipticRgn(0, 0, size + 1, size + 1), TRUE);  // the system owns the region
  }

  UINT flags = SWP_NOACTIVATE | SWP_SHOWWINDOW;
  HWND insertAfter;
  if (badge.zOrderDenied) {
    // Windows refuses to place our window next to an elevated one in the Z-order, so
    // float the badge over everything while the target is in front and hide it otherwise.
    if (GetForegroundWindow() != badge.target) {
      ShowWindow(badge.badge, SW_HIDE);
      return;
    }
    insertAfter = HWND_TOPMOST;  // already topmost since creation; keeps it on top of other topmost windows
  } else {
    // Insert the badge right below the window above the target, i.e. directly above it.
    insertAfter = GetWindow(badge.target, GW_HWNDPREV);
    if (insertAfter == badge.badge) {
      flags |= SWP_NOZORDER;
    } else if (!insertAfter) {
      insertAfter = (GetWindowLongPtr(badge.target, GWL_EXSTYLE) & WS_EX_TOPMOST) ? HWND_TOPMOST : HWND_TOP;
    }
  }

  if (!SetWindowPos(badge.badge, insertAfter, rect.right - margin - size, rect.top + margin, size, size, flags)) {
    DWORD error = GetLastError();
    if (error == ERROR_ACCESS_DENIED && !badge.zOrderDenied) {
      Wh_Log(L"Pin badge Z-order denied (elevated window 0x%p?); showing it only while in front", badge.target);
      HWND topmostBadge = CreateBadgeWindow(true);
      if (!topmostBadge) return;
      DestroyWindow(badge.badge);
      badge.badge = topmostBadge;
      badge.size = 0;  // the new window needs its region
      badge.zOrderDenied = true;
      UpdateBadge(badge);
      return;
    }
    Wh_Log(L"Failed to position pin badge: %lu", error);
  }
}

void UpdateAllBadges() {
  for (auto& badge : g_badges) UpdateBadge(badge);
}

void RemoveBadgeAt(size_t index);

void CALLBACK BadgeWinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD) {
  if (event == EVENT_SYSTEM_FOREGROUND) {
    UpdateAllBadges();  // activating a window changes the Z-order of the others
    return;
  }
  if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF) return;

  for (size_t i = 0; i < g_badges.size(); ++i) {
    if (g_badges[i].target != hwnd) continue;
    if (event == EVENT_OBJECT_DESTROY) {
      RemoveBadgeAt(i);
    } else {
      UpdateBadge(g_badges[i]);
    }
    return;
  }
}

// The event hooks and the check timer exist only while at least one badge is shown,
// so the global location-change stream is not processed when nothing is pinned.
void StartBadgeTracking() {
  if (g_badgeHooks[0]) return;
  constexpr DWORD kFlags = WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS;
  // Only the events the callback uses; a single wide range would also deliver name,
  // value, state and focus changes of every window in the system.
  static constexpr DWORD kBadgeEvents[kBadgeHookCount][2] = {
      {EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND},
      {EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND},
      {EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE},
      {EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE},
      {EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED},
  };
  for (int i = 0; i < kBadgeHookCount; ++i) {
    g_badgeHooks[i] = SetWinEventHook(kBadgeEvents[i][0], kBadgeEvents[i][1], nullptr, BadgeWinEventProc, 0, 0, kFlags);
    if (!g_badgeHooks[i]) Wh_Log(L"SetWinEventHook %d failed: %lu", i, GetLastError());
  }
  if (HWND msgWnd = g_hMsgWnd.load()) SetTimer(msgWnd, kBadgeCheckTimerId, kBadgeCheckIntervalMs, nullptr);
}

void StopBadgeTracking() {
  for (auto& hook : g_badgeHooks) {
    if (hook) UnhookWinEvent(hook);
    hook = nullptr;
  }
  if (HWND msgWnd = g_hMsgWnd.load()) KillTimer(msgWnd, kBadgeCheckTimerId);
}

void RemoveBadgeAt(size_t index) {
  DestroyWindow(g_badges[index].badge);
  g_badges.erase(g_badges.begin() + index);
  if (g_badges.empty()) StopBadgeTracking();
}

void RemoveBadge(HWND target) {
  for (size_t i = 0; i < g_badges.size(); ++i) {
    if (g_badges[i].target == target) {
      RemoveBadgeAt(i);
      return;
    }
  }
}

void RemoveAllBadges() {
  while (!g_badges.empty()) RemoveBadgeAt(g_badges.size() - 1);
}

void ShowBadge(HWND target) {
  if (PinBadge* existing = FindBadge(target)) {
    UpdateBadge(*existing);
    return;
  }

  HWND badgeWnd = CreateBadgeWindow(false);
  if (!badgeWnd) return;

  g_badges.push_back({target, badgeWnd, 0, false});
  StartBadgeTracking();
  UpdateBadge(g_badges.back());
}

// Safety net for what the event hooks do not report: unpinning from Task View and
// windows that vanished without a destroy event.
// IsWindowPinned makes cross-process COM calls that pump messages, so a destroy event
// can remove a badge in the middle of this loop: iterate over a copy and remove by HWND
// (a no-op when the badge is already gone), never by a stored index.
void OnBadgeCheckTimer() {
  std::vector<HWND> targets;
  for (const auto& badge : g_badges) targets.push_back(badge.target);
  for (HWND target : targets) {
    if (!IsWindow(target) || !IsWindowPinned(target)) RemoveBadge(target);
  }
}

LRESULT CALLBACK BadgeWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC dc = BeginPaint(hwnd, &ps);
      HBRUSH brush = CreateSolidBrush(GetAccentColor());
      FillRect(dc, &ps.rcPaint, brush);
      DeleteObject(brush);
      EndPaint(hwnd, &ps);
      return 0;
    }
    case WM_DWMCOLORIZATIONCOLORCHANGED:
      InvalidateRect(hwnd, nullptr, TRUE);  // follow accent color changes
      return 0;
    case WM_NCHITTEST:
      return HTTRANSPARENT;
  }
  return DefWindowProc(hwnd, message, wParam, lParam);
}

// Pin the active window to all desktops (|pin| = true) or unpin it. Sets the state
// instead of toggling, so each hotkey has one predictable result.
bool SetActiveWindowPinned(bool pin) {
  if (!InitializeVirtualDesktopAPI()) return false;
  if (!g_pPinnedApps) {
    Wh_Log(L"Pin service not available");
    return false;
  }

  HWND hwnd = GetForegroundWindow();
  if (!hwnd) {
    Wh_Log(L"No active window to %s", pin ? L"pin" : L"unpin");
    return false;
  }

  // Retry once after explorer.exe restarted: the pin calls go straight to the proxy.
  HRESULT hr = E_FAIL;
  for (int attempt = 0; attempt < 2; ++attempt) {
    IApplicationView* view = nullptr;
    if (FAILED(GetViewForHwnd(hwnd, &view)) || !view) {
      Wh_Log(L"No application view for the active window (hwnd=0x%p)", hwnd);
      return false;
    }
    if (!g_pPinnedApps) {  // a reconnect inside GetViewForHwnd lost the pin service
      view->Release();
      Wh_Log(L"Pin service not available");
      return false;
    }

    BOOL isPinned = FALSE;
    hr = g_pPinnedApps->IsViewPinned(view, &isPinned);
    if (SUCCEEDED(hr) && (isPinned != FALSE) != pin) {
      hr = pin ? g_pPinnedApps->PinView(view) : g_pPinnedApps->UnpinView(view);
    }
    view->Release();

    if (!IsServerGone(hr) || !ReinitializeVirtualDesktopAPI()) break;
  }

  if (FAILED(hr)) {
    Wh_Log(L"Failed to %s window: 0x%08X", pin ? L"pin" : L"unpin", hr);
    return false;
  }
  Wh_Log(L"Window %s", pin ? L"pinned" : L"unpinned");
  if (pin) {
    ShowBadge(hwnd);
  } else {
    RemoveBadge(hwnd);
  }
  return true;
}

//=============================================================================
// Hotkey thread
//=============================================================================

void RegisterHotkeys(HWND hwnd) {
  BOOL ok = RegisterHotKey(hwnd, HK_MOVE_PREV, kHotkeyModifiers, VK_LEFT);
  Wh_Log(L"RegisterHotKey Ctrl+Win+Shift+Left: result=%d, error=%lu", ok, ok ? 0 : GetLastError());

  ok = RegisterHotKey(hwnd, HK_MOVE_NEXT, kHotkeyModifiers, VK_RIGHT);
  Wh_Log(L"RegisterHotKey Ctrl+Win+Shift+Right: result=%d, error=%lu", ok, ok ? 0 : GetLastError());

  ok = RegisterHotKey(hwnd, HK_PIN, kHotkeyModifiers, VK_UP);
  Wh_Log(L"RegisterHotKey Ctrl+Win+Shift+Up: result=%d, error=%lu", ok, ok ? 0 : GetLastError());

  ok = RegisterHotKey(hwnd, HK_UNPIN, kHotkeyModifiers, VK_DOWN);
  Wh_Log(L"RegisterHotKey Ctrl+Win+Shift+Down: result=%d, error=%lu", ok, ok ? 0 : GetLastError());
}

void UnregisterHotkeys(HWND hwnd) {
  UnregisterHotKey(hwnd, HK_MOVE_PREV);
  UnregisterHotKey(hwnd, HK_MOVE_NEXT);
  UnregisterHotKey(hwnd, HK_PIN);
  UnregisterHotKey(hwnd, HK_UNPIN);
}

void HandleHotkey(UINT hotkeyId) {
  if (!InitializeVirtualDesktopAPI()) {
    Wh_Log(L"Hotkey ignored: API not initialized");
    return;
  }

  if (hotkeyId == HK_MOVE_PREV) {
    MoveActiveWindowByOffset(-1);
  } else if (hotkeyId == HK_MOVE_NEXT) {
    MoveActiveWindowByOffset(+1);
  } else if (hotkeyId == HK_PIN) {
    SetActiveWindowPinned(true);
  } else if (hotkeyId == HK_UNPIN) {
    SetActiveWindowPinned(false);
  }
}

LRESULT CALLBACK HotkeyWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  if (message == WM_HOTKEY) {
    if (g_hotkeyThreadRunning) {
      HandleHotkey(static_cast<UINT>(wParam));
    }
    return 0;
  }

  if (message == WM_TIMER && wParam == kBadgeCheckTimerId) {
    OnBadgeCheckTimer();
    return 0;
  }

  if (message == WM_APP_SETTINGS_CHANGED) {
    if (!g_hotkeyThreadRunning) return 0;

    int previousVersionIndex = g_windowsVersionIndex;
    LoadSettings();

    if (previousVersionIndex != g_windowsVersionIndex) {
      CleanupVirtualDesktopAPI();
      if (!InitializeVirtualDesktopAPI()) {
        Wh_Log(L"Virtual Desktop API failed to initialize after settings change; hotkeys will retry on use");
      }
    }
    return 0;
  }

  return DefWindowProc(hwnd, message, wParam, lParam);
}

DWORD WINAPI HotkeyThreadProc(LPVOID) {
  Wh_Log(L"Hotkey thread started, thread ID: %lu", GetCurrentThreadId());

  HRESULT coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
  if (FAILED(coHr)) {
    Wh_Log(L"Hotkey thread COM initialization failed: 0x%08X", coHr);
    g_hotkeyThreadRunning = false;
    return 0;
  }

  // Badge positions come from other windows' physical-pixel bounds.
  SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

  HINSTANCE module = GetModuleHandle(nullptr);
  WNDCLASSEXW badgeClass = {
      .cbSize = sizeof(badgeClass),
      .lpfnWndProc = BadgeWindowProc,
      .hInstance = module,
      .lpszClassName = kBadgeWindowClassName,
  };
  if (!RegisterClassExW(&badgeClass)) {
    Wh_Log(L"Failed to register pin badge window class: %lu", GetLastError());  // pinning still works
  }

  WNDCLASSEXW windowClass = {
      .cbSize = sizeof(windowClass),
      .lpfnWndProc = HotkeyWindowProc,
      .hInstance = module,
      .lpszClassName = kHotkeyWindowClassName,
  };

  if (!RegisterClassExW(&windowClass)) {
    Wh_Log(L"Failed to register hotkey window class: %lu", GetLastError());
    CoUninitialize();
    g_hotkeyThreadRunning = false;
    return 0;
  }

  HWND hwnd = CreateWindowExW(0, kHotkeyWindowClassName, nullptr, 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, module,
                              nullptr);
  if (!hwnd) {
    Wh_Log(L"Failed to create hotkey message window: %lu", GetLastError());
    UnregisterClassW(kHotkeyWindowClassName, module);
    CoUninitialize();
    g_hotkeyThreadRunning = false;
    return 0;
  }

  g_hMsgWnd = hwnd;

  if (g_hotkeyThreadRunning) {
    if (g_settingsReloadPending.exchange(false)) {
      LoadSettings();
    }
    RegisterHotkeys(hwnd);
    Wh_Log(L"Hotkey thread ready");

    if (!InitializeVirtualDesktopAPI()) {
      Wh_Log(L"Virtual Desktop API failed to initialize on startup; hotkeys will retry on use");
    }
  }

  MSG msg;
  while (g_hotkeyThreadRunning && GetMessage(&msg, nullptr, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  RemoveAllBadges();  // before g_hMsgWnd is cleared: it also kills the check timer
  g_hMsgWnd = nullptr;
  UnregisterHotkeys(hwnd);
  CleanupVirtualDesktopAPI();
  DestroyWindow(hwnd);
  UnregisterClassW(kHotkeyWindowClassName, module);
  UnregisterClassW(kBadgeWindowClassName, module);
  CoUninitialize();

  g_hotkeyThreadRunning = false;
  Wh_Log(L"Hotkey thread stopped");
  return 0;
}

//=============================================================================
// Windhawk tool mod entry points
//=============================================================================

BOOL WhTool_ModInit() {
  Wh_Log(L"Desktop Window Keys initializing...");
  LoadSettings();
  g_settingsReloadPending = false;
  g_hotkeyThreadRunning = true;
  g_hThread = CreateThread(nullptr, 0, HotkeyThreadProc, nullptr, 0, nullptr);
  if (!g_hThread) {
    g_hotkeyThreadRunning = false;
    Wh_Log(L"Failed to create hotkey thread: %lu", GetLastError());
    return FALSE;
  }
  return TRUE;
}

void WhTool_ModUninit() {
  Wh_Log(L"Desktop Window Keys uninitializing...");
  g_hotkeyThreadRunning = false;
  if (HWND hwnd = g_hMsgWnd.load()) {
    PostMessage(hwnd, WM_QUIT, 0, 0);
  }

  if (g_hThread) {
    DWORD waitResult = WaitForSingleObject(g_hThread, 5000);
    if (waitResult != WAIT_OBJECT_0) {
      Wh_Log(L"Hotkey thread cleanup did not finish before process exit (wait=%lu)", waitResult);
    }
    CloseHandle(g_hThread);
    g_hThread = nullptr;
  }
}

void WhTool_ModSettingsChanged() {
  if (HWND hwnd = g_hMsgWnd.load()) {
    PostMessage(hwnd, WM_APP_SETTINGS_CHANGED, 0, 0);
    return;
  }

  // The hotkey thread has not created its window yet: it reloads settings on startup.
  g_settingsReloadPending = true;
  if (HWND hwnd = g_hMsgWnd.load(); hwnd && g_settingsReloadPending.exchange(false)) {
    PostMessage(hwnd, WM_APP_SETTINGS_CHANGED, 0, 0);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

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
