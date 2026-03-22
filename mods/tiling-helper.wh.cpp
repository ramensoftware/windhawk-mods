// ==WindhawkMod==
// @id              tiling-helper
// @name            Tiling Helper
// @description     Tile windows on the current monitor with customizable layouts and hotkeys
// @version         2.0.0
// @author          u2x1
// @github          https://github.com/u2x1
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -luuid -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Tiling Helper

A focused window tiling tool for Windows 10/11. Tile windows on the current monitor with the `MasterStack` layout,
and keep resize-adjusted ratios per virtual desktop.

## Features
- Manual tiling hotkey
- Single layout: Master+Stack
- Adjustable margin, gap, and master size
- Remembers resize adjustments per virtual desktop + monitor (auto-retile can be disabled)
- Captures current window sizes when tiling (useful if auto-retile is off)

## Default Hotkeys
- Tile windows: Alt + D (always available)
- Pause/resume automatic tiling + reset memory: Alt + R

## Notes
- Tiling state (ratios/weights) is stored per virtual desktop and monitor.
- If Virtual Desktop APIs are unavailable, tiling still works but per-desktop memory is disabled.
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

- EnableTileNewWin: false
  $name: '[Tiling] Tile New Windows'
  $description: Automatically retile when a new window is created

- EnableResizeRetile: true
  $name: '[Tiling] Retile On Resize'
  $description: Automatically retile when a tiled window is resized

- RetileToggleKey: "R"
  $name: '[Tiling] Retile Toggle Key'
  $description: 'Key to pause/resume automatic tiling and reset tiling memory. Manual tiling remains available.'

- SwapMasterKey: "C"
  $name: '[Tiling] Switch Master Window Key'
  $description: 'Key to set another master window while tiling is not paused.'

- TilingModifier: alt
  $name: '[Tiling] Modifier'
  $description: Modifier keys for tiling hotkeys
  $options:
    - alt: Alt
    - ctrl: Ctrl
    - alt+shift: Alt + Shift
    - ctrl+alt: Ctrl + Alt
    - ctrl+shift: Ctrl + Shift

- TileKey: "D"
  $name: '[Tiling] Key'
  $description: 'Key to tile all windows on the current monitor. Examples: D, T, Space, `, -'

- TileMargin: 4
  $name: '[Tiling] Margin (pixels)'
  $description: Gap between tiled windows and screen edges (0-100)

- TileGap: 4
  $name: '[Tiling] Gap (pixels)'
  $description: Gap between adjacent tiled windows (0-100)

- LayoutOrientation: vertical
  $name: '[Tiling] Layout Orientation'
  $description: 'Master window position: horizontal (master on left, stack on right), vertical (master on top, stack on bottom)'
  $options:
    - vertical: Vertical (Master Top)
    - horizontal: Horizontal (Master Left)

- MasterPercent: 50
  $name: '[Tiling] Master Size (%)'
  $description: Size percentage of the master window in `masterh` layout (1-99)
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>
#include <initguid.h>
#include <objbase.h>
#include <objectarray.h>
#include <shobjidl.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
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
//=============================================================================

static const CLSID CLSID_ImmersiveShell = {
    0xC2F03A33, 0x21F5, 0x47FA, {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39}};

static const CLSID CLSID_VirtualDesktopManagerInternal = {
    0xC5E0CDCA, 0x7B6E, 0x41B2, {0x9F, 0xC4, 0xD9, 0x39, 0x75, 0xCC, 0x46, 0x7B}};

//=============================================================================
// Windows Version-Specific Interface IDs
//=============================================================================
struct VersionIIDs {
  IID managerInternal;  // IID for IVirtualDesktopManagerInternal
  IID virtualDesktop;   // IID for IVirtualDesktop
  bool usesHMonitor;    // Whether API methods require HMONITOR parameter
};

static const VersionIIDs g_versionIIDs[] = {
    // [0] Windows 10 (Build < 20348)
    {{0xF31574D6, 0xB682, 0x4CDC, {0xBD, 0x56, 0x18, 0x27, 0x86, 0x0A, 0xBE, 0xC6}},
     {0xFF72FFDD, 0xBE7E, 0x43FC, {0x9C, 0x03, 0xAD, 0x81, 0x68, 0x1E, 0x88, 0xE4}},
     false},

    // [1] Windows 10 (Build 20348 - 21999)
    {{0x094AFE11, 0x44F2, 0x4BA0, {0x97, 0x6F, 0x29, 0xA9, 0x7E, 0x26, 0x3E, 0xE0}},
     {0x62FDF88B, 0x11CA, 0x4AFB, {0x8B, 0xD8, 0x22, 0x96, 0xDF, 0xAE, 0x49, 0xE2}},
     true},

    // [2] Windows 11 (Build 22000 - 22482)
    {{0xB2F925B9, 0x5A0F, 0x4D2E, {0x9F, 0x4D, 0x2B, 0x15, 0x07, 0x59, 0x3C, 0x10}},
     {0x536D3495, 0xB208, 0x4CC9, {0xAE, 0x26, 0xDE, 0x81, 0x11, 0x27, 0x5B, 0xF8}},
     true},

    // [3] Windows 11 (Build 22621/22631/23H2)
    {{0xA3175F2D, 0x239C, 0x4BD2, {0x8A, 0xA0, 0xEE, 0xBA, 0x8B, 0x0B, 0x13, 0x8E}},
     {0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}},
     false},

    // [4] Windows 11 (Build 26100+ / 24H2)
    {{0x53F5CA0B, 0x158F, 0x4124, {0x90, 0x0C, 0x05, 0x71, 0x58, 0x06, 0x0B, 0x27}},
     {0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}},
     false},
};

//=============================================================================
// COM Interface Definitions (undocumented, reverse-engineered)
//=============================================================================
struct IVirtualDesktop : public IUnknown {
  virtual HRESULT STDMETHODCALLTYPE IsViewVisible(IUnknown*, BOOL*) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetId(GUID*) = 0;
};

struct IVirtualDesktopManagerInternal : public IUnknown {};

//=============================================================================
// Global State
//=============================================================================

static inline LONG RectW(const RECT& r) { return r.right - r.left; }
static inline LONG RectH(const RECT& r) { return r.bottom - r.top; }

enum class Axis { X, Y };

inline Axis OrthogonalAxis(Axis axis) {
  return axis == Axis::X ? Axis::Y : Axis::X;
}

inline LONG AxisStart(const RECT& rect, Axis axis) {
  return axis == Axis::X ? rect.left : rect.top;
}

inline LONG AxisEnd(const RECT& rect, Axis axis) {
  return axis == Axis::X ? rect.right : rect.bottom;
}

inline LONG AxisSpan(const RECT& rect, Axis axis) {
  return axis == Axis::X ? RectW(rect) : RectH(rect);
}

inline RECT ReplaceAxisRange(const RECT& rect, Axis axis, LONG start, LONG end) {
  RECT out = rect;
  if (axis == Axis::X) {
    out.left = start;
    out.right = end;
  } else {
    out.top = start;
    out.bottom = end;
  }
  return out;
}

struct DesktopApiState {
  IServiceProvider* serviceProvider = nullptr;
  IVirtualDesktopManagerInternal* managerInternal = nullptr;
  IVirtualDesktopManager* manager = nullptr;
  bool initialized = false;
  int windowsVersionIndex = 4;
};

struct HotkeyThreadState {
  HANDLE thread = nullptr;
  DWORD threadId = 0;
  HANDLE readyEvent = nullptr;
  volatile bool stopRequested = false;
};

struct SettingsState {
  UINT tilingModifiers = MOD_ALT;
  UINT tileKey = 'D';
  UINT retileToggleKey = 'R';
  UINT swapMasterKey = 'M';
  LONG tileMargin = 4;
  LONG tileGap = 4;
  LONG masterPercent = 50;
  bool enableResizeRetile = true;
  bool retileSuspended = false;
  bool enableTileNewWin = false;
  Axis masterAxis = Axis::Y;  // Y = horizontal (master left), X = vertical (master top)
};

// Hotkey IDs
enum HotkeyIds {
  HK_TILE = 1,
  HK_RETILE_TOGGLE = 2,
  HK_SWAP_MASTER = 3
};

// Messages to hotkey thread
constexpr UINT WM_APP_RETILE = WM_APP + 1;
constexpr UINT WM_APP_TILE = WM_APP + 2;
constexpr UINT WM_APP_PRUNE_DESTROY = WM_APP + 3;

// Per-desktop + monitor state
struct GuidHash {
  size_t operator()(const GUID& guid) const {
    const uint32_t* data = reinterpret_cast<const uint32_t*>(&guid);
    size_t hash = 0;
    for (int i = 0; i < 4; ++i) {
      hash ^= std::hash<uint32_t>{}(data[i]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
  }
};

struct GuidEqual {
  bool operator()(const GUID& a, const GUID& b) const { return IsEqualGUID(a, b); }
};

struct DesktopMonitorKey {
  GUID desktopId;
  HMONITOR monitor;
};

struct DesktopMonitorKeyHash {
  size_t operator()(const DesktopMonitorKey& key) const {
    size_t hash = GuidHash{}(key.desktopId);
    hash ^= std::hash<void*>{}(key.monitor) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    return hash;
  }
};

struct DesktopMonitorKeyEqual {
  bool operator()(const DesktopMonitorKey& a, const DesktopMonitorKey& b) const {
    return IsEqualGUID(a.desktopId, b.desktopId) && a.monitor == b.monitor;
  }
};

struct TilingState {
  std::vector<HWND> windows;
  double masterRatio = 0.5;
  std::vector<double> stackWeights;
};

struct TilingStore {
  std::unordered_map<DesktopMonitorKey, TilingState, DesktopMonitorKeyHash, DesktopMonitorKeyEqual> tilingStateMap;
  SRWLOCK tilingStateLock = SRWLOCK_INIT;
  std::unordered_set<HWND> untiledWindows;
  SRWLOCK untiledWindowsLock = SRWLOCK_INIT;
  SRWLOCK moveSizeRectsLock = SRWLOCK_INIT;
  volatile LONG retileInProgress = 0;
  std::unordered_map<HWND, RECT> moveSizeStartRects;
  std::unordered_map<HWND, RECT> moveSizeEndRects;
};

struct HookState {
  HWINEVENTHOOK moveSizeHook = nullptr;
  HWINEVENTHOOK minimizeHook = nullptr;
  HWINEVENTHOOK hideDestroyHook = nullptr;
  HWINEVENTHOOK cloakHook = nullptr;
};

static DesktopApiState g_desktopApi;
static HotkeyThreadState g_hotkeyThread;
static SettingsState g_settings;
static TilingStore g_store;
static HookState g_hooks;

//=============================================================================
//Move detection
//=============================================================================

static inline bool Differs(LONG a, LONG b, LONG tol = 1) {
  return (a > b) ? (a - b > tol) : (b - a > tol);
}

bool IsMoveOnlyRectChange(const RECT& before, const RECT& after, LONG tol = 1) {
  bool xChanged = Differs(before.left, after.left, tol);
  bool yChanged = Differs(before.top, after.top, tol);
  bool wChanged = Differs(RectW(before), RectW(after), tol);
  bool hChanged = Differs(RectH(before), RectH(after), tol);
  return (xChanged || yChanged) && !wChanged && !hChanged;
}

//=============================================================================
// Internal Module Interfaces
//=============================================================================
struct RetileContext;
struct WindowEventContext;
// Desktop API
bool InitializeVirtualDesktopAPI();
bool GetCurrentDesktopId(GUID* outGuid);
bool GetWindowDesktopIdSafe(HWND hwnd, GUID* outGuid);
void CleanupVirtualDesktopAPI();
// Layout + tiling
bool IsWindowCloaked(HWND hwnd);
bool IsTileEligible(HWND hwnd, HMONITOR targetMonitor);
std::vector<HWND> CollectTileWindows(HMONITOR monitor);
void TileWindows(HWND preferredWindow, bool allowWhenSuspended = false);
void SwapMaster();
// Store helpers
bool TryGetSavedState(const DesktopMonitorKey& key, TilingState* outState);
void StoreTilingState(const DesktopMonitorKey& key, const TilingState& state);
void StoreOrEraseTilingState(const DesktopMonitorKey& key, const TilingState& state);
bool GetTiledWorkArea(HMONITOR monitor, RECT* outRect);
bool TryGetDesktopMonitorContext(HWND hwnd, DesktopMonitorKey* outKey, RECT* outWorkArea);
void ClearMoveSizeRects(HWND hwnd);
bool TakeResizeRects(HWND hwnd, RECT* outStartRect, RECT* outEndRect);
void ResetTilingStateMemory();
// Engine
static HWND PruneDestroyedAndPickAnchor(HWND deadHwnd);
static bool HandleTrivialState(const DesktopMonitorKey& key, TilingState& state, const RECT& workArea);
void RetileFromResize(HWND hwnd);
void OnWindowResizeEnd(HWND hwnd);
// Events
void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD);
void InstallWinEventHooks();
void RemoveWinEventHooks();
// Runtime
void LoadSettings();
DWORD WINAPI HotkeyThreadProc(LPVOID);
bool StartHotkeyThread();
void StopHotkeyThread();

//=============================================================================
// Helper Functions
//=============================================================================

template <typename T>
T GetVTableFunction(void* pInterface, int index) {
  return reinterpret_cast<T>((*reinterpret_cast<void***>(pInterface))[index]);
}

bool UsesHMonitorParameter() { return g_versionIIDs[g_desktopApi.windowsVersionIndex].usesHMonitor; }

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

LONG ReadClampedIntSetting(PCWSTR name, LONG minVal, LONG maxVal, LONG defaultVal) {
  LONG value = Wh_GetIntSetting(name);
  return value >= minVal && value <= maxVal ? value : defaultVal;
}

inline double ClampDouble(double value, double minVal, double maxVal) {
  if (value < minVal) return minVal;
  if (value > maxVal) return maxVal;
  return value;
}

bool GetWindowFrameRect(HWND hwnd, RECT* outRect) {
  if (!hwnd || !outRect) return false;
  if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, outRect, sizeof(*outRect)))) {
    return true;
  }
  return GetWindowRect(hwnd, outRect) != FALSE;
}

bool GetMonitorWorkArea(HMONITOR monitor, RECT* outRect) {
  if (!monitor || !outRect) return false;
  MONITORINFO monitorInfo = {sizeof(monitorInfo)};
  if (!GetMonitorInfoW(monitor, &monitorInfo)) return false;
  *outRect = monitorInfo.rcWork;
  return true;
}

bool HaveSameWindowSet(const std::vector<HWND>& a, const std::vector<HWND>& b) {
  if (a.size() != b.size()) return false;
  for (HWND hwndA : a) {
    bool found = false;
    for (HWND hwndB : b) {
      if (hwndA == hwndB) {
        found = true;
        break;
      }
    }
    if (!found) return false;
  }
  return true;
}

std::vector<double> DefaultWeights(size_t count) {
  return std::vector<double>(count, 1.0);
}

bool ContainsWindow(const std::vector<HWND>& windows, HWND hwnd) {
  for (HWND w : windows) {
    if (w == hwnd) return true;
  }
  return false;
}

HWND NormalizeTrackedWindow(HWND hwnd) {
  if (!hwnd) return nullptr;
  HWND root = GetAncestor(hwnd, GA_ROOTOWNER);
  return root ? root : hwnd;
}

bool IsWindowMarkedUntiled(HWND hwnd) {
  hwnd = NormalizeTrackedWindow(hwnd);
  if (!hwnd) return false;

  AcquireSRWLockShared(&g_store.untiledWindowsLock);
  bool isUntiled = g_store.untiledWindows.find(hwnd) != g_store.untiledWindows.end();
  ReleaseSRWLockShared(&g_store.untiledWindowsLock);

  return isUntiled;
}

void MarkWindowUntiled(HWND hwnd) {
  hwnd = NormalizeTrackedWindow(hwnd);
  if (!hwnd) return;

  AcquireSRWLockExclusive(&g_store.untiledWindowsLock);
  g_store.untiledWindows.insert(hwnd);
  ReleaseSRWLockExclusive(&g_store.untiledWindowsLock);
}

void ClearWindowUntiled(HWND hwnd) {
  hwnd = NormalizeTrackedWindow(hwnd);
  if (!hwnd) return;

  AcquireSRWLockExclusive(&g_store.untiledWindowsLock);
  g_store.untiledWindows.erase(hwnd);
  ReleaseSRWLockExclusive(&g_store.untiledWindowsLock);
}

void RemoveUntiledWindows(std::vector<HWND>& windows) {
  AcquireSRWLockShared(&g_store.untiledWindowsLock);
  windows.erase(std::remove_if(windows.begin(), windows.end(), [&](HWND hwnd) {
                  return g_store.untiledWindows.find(NormalizeTrackedWindow(hwnd)) != g_store.untiledWindows.end();
                }),
                windows.end());
  ReleaseSRWLockShared(&g_store.untiledWindowsLock);
}

HWND ResolveEquivalentWindow(HWND hwnd, const std::vector<HWND>& candidates) {
  if (!hwnd || !IsWindow(hwnd)) return nullptr;

  DWORD pid = 0;
  GetWindowThreadProcessId(hwnd, &pid);

  wchar_t className[128] = {};
  GetClassNameW(hwnd, className, ARRAYSIZE(className));

  wchar_t title[256] = {};
  GetWindowTextW(hwnd, title, ARRAYSIZE(title));

  HWND bestMatch = nullptr;
  for (HWND candidate : candidates) {
    if (!candidate || candidate == hwnd || !IsWindow(candidate)) continue;

    DWORD candPid = 0;
    GetWindowThreadProcessId(candidate, &candPid);
    if (candPid != pid) continue;

    wchar_t candClass[128] = {};
    GetClassNameW(candidate, candClass, ARRAYSIZE(candClass));
    if (_wcsicmp(candClass, className) != 0) continue;

    wchar_t candTitle[256] = {};
    GetWindowTextW(candidate, candTitle, ARRAYSIZE(candTitle));

    if (_wcsicmp(candTitle, title) == 0 || (candTitle[0] == 0 && title[0] == 0)) {
      return candidate;
    }

    if (!bestMatch) bestMatch = candidate;
  }

  return bestMatch;
}

HWND ResolveToTiledWindow(HWND hwnd, const std::vector<HWND>& candidates) {
  if (!hwnd || !IsWindow(hwnd)) return nullptr;

  if (ContainsWindow(candidates, hwnd)) {
    return hwnd;
  }

  HWND rootOwner = GetAncestor(hwnd, GA_ROOTOWNER);
  if (rootOwner && ContainsWindow(candidates, rootOwner)) {
    return rootOwner;
  }

  HWND owner = GetWindow(hwnd, GW_OWNER);
  while (owner) {
    if (ContainsWindow(candidates, owner)) {
      return owner;
    }
    owner = GetWindow(owner, GW_OWNER);
  }

  return ResolveEquivalentWindow(hwnd, candidates);
}

std::vector<HWND> CollectTileWindows(HMONITOR monitor) {
  std::vector<HWND> windows;
  if (!monitor) return windows;

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

  return windows;
}

struct WindowInfo {
  HWND hwnd;
  RECT rect;
};

LONG ClampSpanMin1(LONG value) {
  return value < 1 ? 1 : value;
}

size_t FindWindowIndex(const std::vector<HWND>& windows, HWND hwnd) {
  for (size_t i = 0; i < windows.size(); ++i) {
    if (windows[i] == hwnd) return i;
  }
  return (size_t)-1;
}

TilingState MakeDefaultState(const std::vector<HWND>& windows) {
  TilingState state;
  state.windows = windows;
  state.masterRatio = ClampDouble(g_settings.masterPercent / 100.0, 0.1, 0.9);
  return state;
}

void NormalizeState(TilingState& state) {
  state.masterRatio = ClampDouble(state.masterRatio, 0.1, 0.9);

  size_t stackCount = state.windows.empty() ? 0 : state.windows.size() - 1;
  if (state.stackWeights.size() != stackCount) {
    state.stackWeights = DefaultWeights(stackCount);
  }
}

std::vector<WindowInfo> CollectWindowInfo(const std::vector<HWND>& windows, const RECT& fallbackRect,
                                          HMONITOR monitor = nullptr) {
  std::vector<WindowInfo> infos;
  infos.reserve(windows.size());

  for (HWND hwnd : windows) {
    RECT rect = {};
    if (!GetWindowFrameRect(hwnd, &rect)) {
      rect = fallbackRect;
    }
    if (monitor && MonitorFromRect(&rect, MONITOR_DEFAULTTONULL) != monitor) {
      rect = fallbackRect;
    }
    infos.push_back({hwnd, rect});
  }

  return infos;
}

void SortWindowInfoByAxis(std::vector<WindowInfo>& infos, Axis axis) {
  std::stable_sort(infos.begin(), infos.end(), [axis](const WindowInfo& a, const WindowInfo& b) {
    return AxisStart(a.rect, axis) < AxisStart(b.rect, axis);
  });
}

size_t PickMasterWindowIndex(const std::vector<WindowInfo>& infos, const RECT& workArea, Axis masterAxis,
                             HWND preferredFirstWin) {
  struct Candidate {
    long long score;
    long long axisPos;
    size_t index;
  };

  std::vector<Candidate> candidates;
  candidates.reserve(infos.size());

  for (size_t i = 0; i < infos.size(); ++i) {
    const RECT& rect = infos[i].rect;
    long long edges[] = {
        (long long)std::llabs((long long)rect.left - (long long)workArea.left),
        (long long)std::llabs((long long)rect.top - (long long)workArea.top),
        (long long)std::llabs((long long)rect.right - (long long)workArea.right),
        (long long)std::llabs((long long)rect.bottom - (long long)workArea.bottom),
    };
    std::sort(edges, edges + 4);

    candidates.push_back({
        .score = edges[0] + edges[1] + edges[2],
        .axisPos = (long long)std::llabs((long long)AxisStart(rect, masterAxis) -
                                         (long long)AxisStart(workArea, masterAxis)),
        .index = i,
    });
  }

  std::stable_sort(candidates.begin(), candidates.end(), [preferredFirstWin, &infos](const Candidate& a,
                                                                                      const Candidate& b) {
    bool aPreferred = preferredFirstWin && infos[a.index].hwnd == preferredFirstWin;
    bool bPreferred = preferredFirstWin && infos[b.index].hwnd == preferredFirstWin;
    if (aPreferred != bPreferred) return aPreferred;
    if (a.score != b.score) return a.score < b.score;
    return a.axisPos < b.axisPos;
  });

  return candidates.empty() ? 0 : candidates.front().index;
}

TilingState BuildStateFromWindows(const RECT& workArea, const std::vector<HWND>& windows,
                                  HMONITOR monitor, HWND preferredFirstWin = nullptr) {
  TilingState state = MakeDefaultState(windows);
  if (windows.empty()) return state;

  std::vector<WindowInfo> infos = CollectWindowInfo(windows, workArea, monitor);
  size_t masterIndex = PickMasterWindowIndex(infos, workArea, g_settings.masterAxis, preferredFirstWin);

  WindowInfo master = infos[masterIndex];

  std::vector<WindowInfo> stack;
  stack.reserve(infos.size() - 1);
  for (size_t i = 0; i < infos.size(); ++i) {
    if (i != masterIndex) stack.push_back(infos[i]);
  }
  SortWindowInfoByAxis(stack, OrthogonalAxis(g_settings.masterAxis));

  state.windows.clear();
  state.windows.push_back(master.hwnd);
  for (const auto& item : stack) state.windows.push_back(item.hwnd);

  LONG totalSize = AxisSpan(workArea, g_settings.masterAxis);
  if (totalSize > g_settings.tileGap + 1) {
    LONG masterSize = ClampSpanMin1(AxisSpan(master.rect, g_settings.masterAxis));
    state.masterRatio = ClampDouble((double)masterSize / (double)(totalSize - g_settings.tileGap), 0.1, 0.9);
  }

  state.stackWeights.clear();
  for (const auto& item : stack) {
    LONG stackWidth = ClampSpanMin1(AxisSpan(item.rect, OrthogonalAxis(g_settings.masterAxis)));
    state.stackWeights.push_back((double)stackWidth);
  }

  NormalizeState(state);
  return state;
}

std::vector<LONG> ComputeWeightedSizes(LONG totalSize, LONG gap, const std::vector<double>& weights) {
  size_t count = weights.size();
  std::vector<LONG> sizes(count, 0);
  if (count == 0) return sizes;
  LONG available = totalSize - gap * (LONG)(count - 1);
  if (available <= 0) return sizes;

  double sum = 0.0;
  for (double w : weights) sum += (w > 0.0 ? w : 0.0);
  if (sum <= 0.0) sum = static_cast<double>(count);

  LONG used = 0;
  double remainingSum = sum;
  for (size_t i = 0; i < count; ++i) {
    double w = weights[i];
    if (w <= 0.0) w = 1.0;
    LONG remainingSlots = (LONG)(count - i - 1);
    LONG size = 0;
    if (i == count - 1) {
      size = available - used;

    } else {
      double ratio = w / remainingSum;
      size = static_cast<LONG>(std::llround(static_cast<double>(available - used) * ratio));
      if (size < 1) size = 1;
      LONG maxSize = available - used - remainingSlots;
      if (size > maxSize) size = maxSize;
    }
    sizes[i] = size;
    used += size;
    remainingSum -= w;
    if (remainingSum <= 0.0) remainingSum = 1.0;
  }
  return sizes;
}

std::vector<LONG> ComputeWeightedSizesWithFixed(
    LONG totalSize, LONG gap, const std::vector<double>& weights,
    size_t fixedIndex, LONG fixedSize) {

  const size_t count = weights.size();
  std::vector<LONG> sizes(count, 0);
  if (count == 0) return sizes;
  if (fixedIndex >= count) return ComputeWeightedSizes(totalSize, gap, weights);

  const LONG available = totalSize - gap * (LONG)(count - 1);
  if (available <= 0) return sizes;

  // Clamp fixed size so remaining windows can still be >=1
  const LONG minFixed = 1;
  const LONG maxFixed = std::max<LONG>(1, available - (LONG)(count - 1));
  fixedSize = std::clamp(fixedSize, minFixed, maxFixed);

  sizes[fixedIndex] = fixedSize;

  LONG remaining = available - fixedSize;
  if (count == 1) return sizes;

  // Sum weights for non-fixed slots
  double sum = 0.0;
  for (size_t i = 0; i < count; ++i) {
    if (i == fixedIndex) continue;
    sum += (weights[i] > 0.0 ? weights[i] : 1.0);
  }
  if (sum <= 0.0) sum = (double)(count - 1);

  LONG used = 0;
  double remainingSum = sum;

  // Distribute remaining across non-fixed indices (no gap math here)
  for (size_t i = 0; i < count; ++i) {
    if (i == fixedIndex) continue;

    // how many non-fixed slots remain after i?
    LONG slotsLeft = 0;
    for (size_t j = i + 1; j < count; ++j) if (j != fixedIndex) ++slotsLeft;

    double w = (weights[i] > 0.0 ? weights[i] : 1.0);
    LONG size = 0;

    if (slotsLeft == 0) {
      size = remaining - used;
    } else {
      const double ratio = w / remainingSum;
      size = (LONG)std::llround((double)(remaining - used) * ratio);
      if (size < 1) size = 1;

      const LONG maxSize = (remaining - used) - slotsLeft; // leave >=1 for each remaining slot
      if (size > maxSize) size = maxSize;
    }

    sizes[i] = size;
    used += size;
    remainingSum -= w;
    if (remainingSum <= 0.0) remainingSum = 1.0;
  }

  return sizes;
}

static std::vector<LONG> ComputeWeightedSizesWithFixedAnchored(
    LONG totalSize, LONG gap, const std::vector<double>& weights,
    const std::vector<LONG>& currentSizes, size_t fixedIndex, LONG fixedSize,
    bool preserveBeforeFixed, bool preserveAfterFixed) {
  const size_t count = weights.size();
  if (count == 0 || currentSizes.size() != count || fixedIndex >= count ||
      preserveBeforeFixed == preserveAfterFixed) {
    return ComputeWeightedSizesWithFixed(totalSize, gap, weights, fixedIndex, fixedSize);
  }

  const LONG available = totalSize - gap * (LONG)(count - 1);
  if (available <= 0) return std::vector<LONG>(count, 0);

  const LONG minFixed = 1;
  const LONG maxFixed = std::max<LONG>(1, available - (LONG)(count - 1));
  fixedSize = std::clamp(fixedSize, minFixed, maxFixed);

  std::vector<LONG> sizes(count, 0);
  sizes[fixedIndex] = fixedSize;

  LONG reserved = fixedSize;
  size_t freeStart = 0;
  size_t freeEnd = count;

  if (preserveBeforeFixed) {
    freeStart = fixedIndex + 1;
    for (size_t i = 0; i < fixedIndex; ++i) {
      LONG preserved = std::max<LONG>(1, currentSizes[i]);
      sizes[i] = preserved;
      reserved += preserved;
    }
  } else {
    freeEnd = fixedIndex;
    for (size_t i = fixedIndex + 1; i < count; ++i) {
      LONG preserved = std::max<LONG>(1, currentSizes[i]);
      sizes[i] = preserved;
      reserved += preserved;
    }
  }

  const size_t freeCount = freeEnd - freeStart;
  if (freeCount == 0) {
    return reserved == available ? sizes
                                 : ComputeWeightedSizesWithFixed(totalSize, gap, weights, fixedIndex, fixedSize);
  }

  const LONG remaining = available - reserved;
  if (remaining < (LONG)freeCount) {
    return ComputeWeightedSizesWithFixed(totalSize, gap, weights, fixedIndex, fixedSize);
  }

  double sum = 0.0;
  for (size_t i = freeStart; i < freeEnd; ++i) {
    sum += (weights[i] > 0.0 ? weights[i] : 1.0);
  }
  if (sum <= 0.0) sum = static_cast<double>(freeCount);

  LONG used = 0;
  double remainingSum = sum;
  for (size_t i = freeStart; i < freeEnd; ++i) {
    double w = (weights[i] > 0.0 ? weights[i] : 1.0);
    LONG slotsLeft = (LONG)(freeEnd - i - 1);
    LONG size = 0;

    if (slotsLeft == 0) {
      size = remaining - used;
    } else {
      double ratio = w / remainingSum;
      size = static_cast<LONG>(std::llround(static_cast<double>(remaining - used) * ratio));
      if (size < 1) size = 1;
      LONG maxSize = remaining - used - slotsLeft;
      if (size > maxSize) size = maxSize;
    }

    sizes[i] = size;
    used += size;
    remainingSum -= w;
    if (remainingSum <= 0.0) remainingSum = 1.0;
  }

  return sizes;
}

void LayoutStripFromSizes(const RECT& area, Axis axis, const std::vector<LONG>& sizes, std::vector<RECT>& outRects) {
  outRects.resize(sizes.size());
  if (sizes.empty()) return;

  LONG position = AxisStart(area, axis);
  LONG finalEnd = AxisEnd(area, axis);
  for (size_t i = 0; i < sizes.size(); ++i) {
    LONG end = (i == sizes.size() - 1) ? finalEnd : position + sizes[i];
    outRects[i] = ReplaceAxisRange(area, axis, position, end);
    position = end + g_settings.tileGap;
  }
}

void LayoutStripWeighted(const RECT& area, size_t count, Axis axis, const std::vector<double>& weights,
                         std::vector<RECT>& outRects) {
  outRects.resize(count);
  if (count == 0) return;
  if (count == 1) {
    outRects[0] = area;
    return;
  }

  LONG totalSize = AxisSpan(area, axis);
  if (totalSize <= g_settings.tileGap * (LONG)(count - 1)) {
    outRects.assign(count, area);
    return;
  }

  std::vector<double> normalizedWeights = weights;
  if (normalizedWeights.size() != count) normalizedWeights = DefaultWeights(count);
  LayoutStripFromSizes(area, axis, ComputeWeightedSizes(totalSize, g_settings.tileGap, normalizedWeights), outRects);
}

void LayoutMasterStackWeighted(const RECT& area, size_t windowCount, std::vector<RECT>& outRects, Axis masterAxis,
                               double masterRatio, const std::vector<double>& stackWeights) {
  outRects.resize(windowCount);
  if (windowCount == 0) return;
  if (windowCount == 1) {
    outRects[0] = area;
    return;
  }

  LONG totalSize = AxisSpan(area, masterAxis);
  if (totalSize <= g_settings.tileGap + 1) {
    outRects.assign(windowCount, area);
    return;
  }

  LONG masterSize = (LONG)std::llround((double)(totalSize - g_settings.tileGap) * ClampDouble(masterRatio, 0.1, 0.9));
  masterSize = std::clamp(masterSize, 1L, totalSize - g_settings.tileGap - 1);

  LONG areaStart = AxisStart(area, masterAxis);
  LONG masterEnd = areaStart + masterSize;
  RECT masterRect = ReplaceAxisRange(area, masterAxis, areaStart, masterEnd);
  RECT stackArea = ReplaceAxisRange(area, masterAxis, masterEnd + g_settings.tileGap, AxisEnd(area, masterAxis));

  outRects[0] = masterRect;
  std::vector<RECT> stackRects;
  LayoutStripWeighted(stackArea, windowCount - 1, OrthogonalAxis(masterAxis), stackWeights, stackRects);
  for (size_t i = 0; i < stackRects.size(); ++i) {
    outRects[i + 1] = stackRects[i];
  }
}

void LayoutStateRects(const RECT& workArea, TilingState& state, std::vector<RECT>& outRects) {
  NormalizeState(state);
  LayoutMasterStackWeighted(workArea, state.windows.size(), outRects, g_settings.masterAxis,
                            state.masterRatio, state.stackWeights);
}

//=============================================================================
// Virtual Desktop API (minimal for per-desktop memory)
//=============================================================================

static const int VTABLE_GET_CURRENT_DESKTOP = 6;

bool InitializeVirtualDesktopAPIOnce() {
  HRESULT hr = CoCreateInstance(CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER, IID_IServiceProvider,
                                (void**)&g_desktopApi.serviceProvider);
  if (FAILED(hr) || !g_desktopApi.serviceProvider) {
    Wh_Log(L"Failed to create ImmersiveShell: 0x%08X", hr);
    return false;
  }

  hr = g_desktopApi.serviceProvider->QueryService(CLSID_VirtualDesktopManagerInternal,
                                        g_versionIIDs[g_desktopApi.windowsVersionIndex].managerInternal,
                                        (void**)&g_desktopApi.managerInternal);
  if (FAILED(hr) || !g_desktopApi.managerInternal) {
    Wh_Log(L"Failed to get VirtualDesktopManagerInternal: 0x%08X", hr);
    SAFE_RELEASE(g_desktopApi.serviceProvider);
    return false;
  }

  hr = CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_INPROC_SERVER, IID_IVirtualDesktopManager,
                        (void**)&g_desktopApi.manager);
  if (FAILED(hr) || !g_desktopApi.manager) {
    Wh_Log(L"Failed to create VirtualDesktopManager: 0x%08X", hr);
    SAFE_RELEASE(g_desktopApi.managerInternal);
    SAFE_RELEASE(g_desktopApi.serviceProvider);
    return false;
  }

  return true;
}

void CleanupVirtualDesktopAPI() {
  HWND hShell = GetShellWindow();
  if (!hShell || !IsWindow(hShell)) {
    Wh_Log(L"Explorer not available, skipping COM cleanup to avoid hang");
    g_desktopApi.manager = nullptr;
    g_desktopApi.managerInternal = nullptr;
    g_desktopApi.serviceProvider = nullptr;
    g_desktopApi.initialized = false;
    return;
  }

  SAFE_RELEASE(g_desktopApi.manager);
  SAFE_RELEASE(g_desktopApi.managerInternal);
  SAFE_RELEASE(g_desktopApi.serviceProvider);
  g_desktopApi.initialized = false;
}

bool InitializeVirtualDesktopAPI() {
  if (g_desktopApi.initialized) return true;
  if (InitializeVirtualDesktopAPIOnce()) {
    g_desktopApi.initialized = true;
    return true;
  }
  return false;
}

bool ReinitializeVirtualDesktopAPI() {
  CleanupVirtualDesktopAPI();
  return InitializeVirtualDesktopAPI();
}

template <typename TResult>
HRESULT CallManagerInternal(int vtableIndex, TResult* outResult) {
  if (UsesHMonitorParameter()) {
    auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HMONITOR, TResult*)>(g_desktopApi.managerInternal,
                                                                                         vtableIndex);
    HRESULT hr = pfn(g_desktopApi.managerInternal, nullptr, outResult);
    if (FAILED(hr) && ReinitializeVirtualDesktopAPI()) {
      pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HMONITOR, TResult*)>(g_desktopApi.managerInternal,
                                                                                      vtableIndex);
      hr = pfn(g_desktopApi.managerInternal, nullptr, outResult);
    }
    return hr;
  } else {
    auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, TResult*)>(g_desktopApi.managerInternal, vtableIndex);
    HRESULT hr = pfn(g_desktopApi.managerInternal, outResult);
    if (FAILED(hr) && ReinitializeVirtualDesktopAPI()) {
      pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, TResult*)>(g_desktopApi.managerInternal, vtableIndex);
      hr = pfn(g_desktopApi.managerInternal, outResult);
    }
    return hr;
  }
}

bool GetCurrentDesktopId(GUID* outGuid) {
  if (!outGuid) return false;
  if (!g_desktopApi.managerInternal) return false;

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

bool GetWindowDesktopIdSafe(HWND hwnd, GUID* outGuid) {
  if (!outGuid) return false;
  if (!InitializeVirtualDesktopAPI() || !g_desktopApi.manager) return false;
  return SUCCEEDED(g_desktopApi.manager->GetWindowDesktopId(hwnd, outGuid));
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

static bool IsWindowTrackedInAnyState(HWND hwnd) {
  hwnd = NormalizeTrackedWindow(hwnd);
  if (!hwnd) return false;

  bool tracked = false;

  AcquireSRWLockShared(&g_store.tilingStateLock);
  for (const auto& kv : g_store.tilingStateMap) {
    const TilingState& st = kv.second;
    // Fast membership test (linear, but st.windows is small)
    if (ContainsWindow(st.windows, hwnd)) {
      tracked = true;
      break;
    }
  }
  ReleaseSRWLockShared(&g_store.tilingStateLock);

  return tracked;
}

static bool CouldBeTileEligible(HWND hwnd) {
  if (!hwnd) return false;
  if (IsWindowMarkedUntiled(hwnd)) return false;
  if (GetAncestor(hwnd, GA_ROOT) != hwnd) return false;
  if (GetWindow(hwnd, GW_OWNER)) return false;

  LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
  if (style & WS_CHILD) return false;
  if (!(style & WS_SIZEBOX) && !IsWindowTrackedInAnyState(hwnd)) return false;

  LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
  if (exStyle & WS_EX_TOOLWINDOW) return false;
  if (exStyle & WS_EX_NOACTIVATE) return false;

  wchar_t className[64] = {};
  if (GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
    for (const auto* ignoredClass : kIgnoredWindowClasses) {
      if (_wcsicmp(className, ignoredClass) == 0) return false;
    }
  }

  return true;
}

bool IsTileEligible(HWND hwnd, HMONITOR targetMonitor) {
  // window might become temporarily disabled
  // (example: VSCode unstaged commit dialog)
  if (!IsWindowTrackedInAnyState(hwnd) && !IsWindowEnabled(hwnd)) return false;

  if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) return false;
  if (!CouldBeTileEligible(hwnd)) return false;
  if (IsWindowCloaked(hwnd)) return false;

  // Virtual desktop filter: other desktops' windows may not yet be cloaked
  // at the moment we enumerate (race between uncloak/cloak during switch).
  if (g_desktopApi.manager) {
    BOOL onCurrent = FALSE;
    if (SUCCEEDED(g_desktopApi.manager->IsWindowOnCurrentVirtualDesktop(hwnd, &onCurrent)) && !onCurrent)
      return false;
  }

  RECT frameRect;
  if (!SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &frameRect, sizeof(frameRect)))) {
    if (!GetWindowRect(hwnd, &frameRect)) return false;
  }

  return frameRect.right > frameRect.left && frameRect.bottom > frameRect.top &&
         MonitorFromRect(&frameRect, MONITOR_DEFAULTTONULL) == targetMonitor;
}

void PlaceWindow(HWND hwnd, const RECT& targetRect) {
  if (IsZoomed(hwnd)) ShowWindow(hwnd, SW_RESTORE);

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
               SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING); 
}

void SwapMaster() {
  if (g_settings.retileSuspended) return;

  HWND fg = GetForegroundWindow();
  if (!fg) return;

  DesktopMonitorKey key{};
  RECT workArea = {};
  if (!TryGetDesktopMonitorContext(fg, &key, &workArea)) return;

  if (InterlockedCompareExchange(&g_store.retileInProgress, 1, 0) != 0) return;

  bool didSwap = false;
  HWND oldMaster = nullptr;
  HWND resolved = nullptr;
  RECT rMaster{}, rResolved{};

  AcquireSRWLockExclusive(&g_store.tilingStateLock);
  do {
    auto it = g_store.tilingStateMap.find(key);
    if (it == g_store.tilingStateMap.end() || it->second.windows.size() < 2) break;

    TilingState& state = it->second;
    resolved = ResolveToTiledWindow(fg, state.windows);
    size_t index = FindWindowIndex(state.windows, resolved);
    if (index == (size_t)-1 || index == 0) break;

    oldMaster = state.windows[0];
    if (!GetWindowFrameRect(oldMaster, &rMaster) || !GetWindowFrameRect(resolved, &rResolved)) break;

    std::swap(state.windows[0], state.windows[index]);
    didSwap = true;
  } while (false);
  ReleaseSRWLockExclusive(&g_store.tilingStateLock);

  if (didSwap) {
    PlaceWindow(oldMaster, rResolved);
    PlaceWindow(resolved, rMaster);
    ClearMoveSizeRects(oldMaster);
    ClearMoveSizeRects(resolved);
  }

  InterlockedExchange(&g_store.retileInProgress, 0);
}

// Small helpers for TileWindows()
static inline long long RectAreaLL(const RECT& r) {
  long long w = (long long)r.right - (long long)r.left;
  long long h = (long long)r.bottom - (long long)r.top;
  if (w <= 0 || h <= 0) return 0;
  return w * h;
}
// Also helper for TileWindows()
static inline long long WindowAreaOnWorkArea(HWND hwnd, const RECT& workArea) {
  RECT r{};
  if (!GetWindowFrameRect(hwnd, &r)) return 0;

  RECT inter{};
  if (!IntersectRect(&inter, &r, &workArea)) return 0;

  return RectAreaLL(inter);
}

bool TryGetSavedState(const DesktopMonitorKey& key, TilingState* outState) {
  AcquireSRWLockShared(&g_store.tilingStateLock);
  auto it = g_store.tilingStateMap.find(key);
  bool found = it != g_store.tilingStateMap.end();
  if (found && outState) {
    *outState = it->second;
  }
  ReleaseSRWLockShared(&g_store.tilingStateLock);
  return found;
}

void StoreTilingState(const DesktopMonitorKey& key, const TilingState& state) {
  AcquireSRWLockExclusive(&g_store.tilingStateLock);
  g_store.tilingStateMap[key] = state;
  ReleaseSRWLockExclusive(&g_store.tilingStateLock);
}

void StoreOrEraseTilingState(const DesktopMonitorKey& key, const TilingState& state) {
  AcquireSRWLockExclusive(&g_store.tilingStateLock);
  if (state.windows.size() > 1) {
    g_store.tilingStateMap[key] = state;
  } else {
    g_store.tilingStateMap.erase(key);
  }
  ReleaseSRWLockExclusive(&g_store.tilingStateLock);
}

bool GetTiledWorkArea(HMONITOR monitor, RECT* outRect) {
  RECT monitorWork = {};
  if (!outRect || !GetMonitorWorkArea(monitor, &monitorWork)) return false;

  *outRect = {monitorWork.left + g_settings.tileMargin, monitorWork.top + g_settings.tileMargin,
              monitorWork.right - g_settings.tileMargin, monitorWork.bottom - g_settings.tileMargin};
  return outRect->right > outRect->left && outRect->bottom > outRect->top;
}

bool TryGetDesktopMonitorContext(HWND hwnd, DesktopMonitorKey* outKey, RECT* outWorkArea) {
  if (!hwnd || !outKey || !outWorkArea) return false;

  HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
  if (!monitor || !GetTiledWorkArea(monitor, outWorkArea)) return false;

  GUID desktopId = {};
  if (!GetWindowDesktopIdSafe(hwnd, &desktopId)) return false;

  *outKey = DesktopMonitorKey{desktopId, monitor};
  return true;
}

void ClearMoveSizeRects(HWND hwnd) {
  AcquireSRWLockExclusive(&g_store.moveSizeRectsLock);
  g_store.moveSizeStartRects.erase(hwnd);
  g_store.moveSizeEndRects.erase(hwnd);
  ReleaseSRWLockExclusive(&g_store.moveSizeRectsLock);
}

bool TakeResizeRects(HWND hwnd, RECT* outStartRect, RECT* outEndRect) {
  bool haveRects = false;

  AcquireSRWLockExclusive(&g_store.moveSizeRectsLock);
  auto itStart = g_store.moveSizeStartRects.find(hwnd);
  auto itEnd = g_store.moveSizeEndRects.find(hwnd);

  if (itStart != g_store.moveSizeStartRects.end() && itEnd != g_store.moveSizeEndRects.end()) {
    if (outStartRect) *outStartRect = itStart->second;
    if (outEndRect) *outEndRect = itEnd->second;
    haveRects = true;
  }

  if (itStart != g_store.moveSizeStartRects.end()) {
    g_store.moveSizeStartRects.erase(itStart);
  }
  if (itEnd != g_store.moveSizeEndRects.end()) {
    g_store.moveSizeEndRects.erase(itEnd);
  }
  ReleaseSRWLockExclusive(&g_store.moveSizeRectsLock);

  return haveRects;
}

bool ShouldCaptureLayout(const RECT& workArea, const std::vector<HWND>& windows, bool hasDesktopId) {
  if (!hasDesktopId) return true;

  const long long workAreaArea = RectAreaLL(workArea);
  long long sumArea = 0;
  for (HWND hwnd : windows) {
    sumArea += WindowAreaOnWorkArea(hwnd, workArea);
  }

  const long long lo = (workAreaArea * 85) / 100;
  const long long hi = windows.size() <= 2 ? (workAreaArea * 115) / 100 : (workAreaArea * 105) / 100;
  return sumArea >= lo && sumArea <= hi;
}

HMONITOR ResolveTileMonitor(HWND preferredWindow) {
  if (preferredWindow && IsWindow(preferredWindow)) {
    RECT rect = {};
    if (GetWindowFrameRect(preferredWindow, &rect)) {
      HMONITOR preferredMonitor = MonitorFromRect(&rect, MONITOR_DEFAULTTONULL);
      if (preferredMonitor) return preferredMonitor;
    }

    HMONITOR preferredMonitor = MonitorFromWindow(preferredWindow, MONITOR_DEFAULTTONULL);
    if (preferredMonitor) return preferredMonitor;
  }

  HWND foregroundWindow = GetForegroundWindow();
  HMONITOR monitor = foregroundWindow ? MonitorFromWindow(foregroundWindow, MONITOR_DEFAULTTONULL) : nullptr;

  if (!monitor) {
    POINT cursorPos;
    if (GetCursorPos(&cursorPos)) {
      monitor = MonitorFromPoint(cursorPos, MONITOR_DEFAULTTONEAREST);
    }
  }

  return monitor;
}

void TileWindows(HWND preferredWindow, bool allowWhenSuspended) {
  if (g_settings.retileSuspended && !allowWhenSuspended) return;

  HMONITOR monitor = ResolveTileMonitor(preferredWindow);
  RECT workArea = {};
  if (!monitor || !GetTiledWorkArea(monitor, &workArea)) return;

  std::vector<HWND> windows = CollectTileWindows(monitor);
  if (windows.empty()) return;

  GUID desktopId = {};
  bool hasDesktopId = InitializeVirtualDesktopAPI() && GetCurrentDesktopId(&desktopId);
  DesktopMonitorKey key{desktopId, monitor};

  TilingState state = MakeDefaultState(windows);
  TilingState savedState;
  bool hasSavedState = hasDesktopId && TryGetSavedState(key, &savedState);
  bool canReuseSavedState = hasSavedState && HaveSameWindowSet(savedState.windows, windows);
  HWND preferredFirstWin = hasSavedState && !savedState.windows.empty() ? savedState.windows.front() : nullptr;

  if (canReuseSavedState) {
    state = savedState;
  } else if (hasSavedState) {
    // Preserve masterRatio and existing stackWeights when new windows join
    // Start from savedState and add new windows
    state = savedState;
    // Calculate average weight for new windows
    double avgWeight = 1.0;
    if (!state.stackWeights.empty()) {
      double sum = 0.0;
      for (double w : state.stackWeights) sum += (w > 0.0 ? w : 1.0);
      avgWeight = sum / state.stackWeights.size();
    }
    // Add new windows to the end (they become stack windows with average weight)
    for (HWND hwnd : windows) {
      if (!ContainsWindow(state.windows, hwnd)) {
        state.windows.push_back(hwnd);
        state.stackWeights.push_back(avgWeight);
      }
    }
    // Remove windows that no longer exist
    for (size_t i = state.windows.size(); i > 0; --i) {
      if (!ContainsWindow(windows, state.windows[i - 1])) {
        state.windows.erase(state.windows.begin() + i - 1);
        // Remove corresponding stackWeight
        // For master (i==1): remove first stackWeight since second window becomes new master
        // For stack window (i>1): remove stackWeights[i-2]
        if (i == 1 && !state.stackWeights.empty()) {
          state.stackWeights.erase(state.stackWeights.begin());
        } else if (i > 1 && i - 2 < state.stackWeights.size()) {
          state.stackWeights.erase(state.stackWeights.begin() + i - 2);
        }
      }
    }
    NormalizeState(state);
  } else if (ShouldCaptureLayout(workArea, windows, hasDesktopId)) {
    TilingState capturedState = BuildStateFromWindows(workArea, windows, monitor, preferredFirstWin);
    if (!capturedState.windows.empty()) {
      state = std::move(capturedState);
    }
  }

  std::vector<RECT> windowRects;
  LayoutStateRects(workArea, state, windowRects);
  for (size_t i = 0; i < state.windows.size(); ++i) {
    PlaceWindow(state.windows[i], windowRects[i]);
  }

  if (hasDesktopId) {
    StoreOrEraseTilingState(key, state);
  }

  Wh_Log(L"Tiled %zu windows", state.windows.size());
}

// Helper for window destroy handling
static HWND PruneDestroyedAndPickAnchor(HWND deadHwnd) {
  HWND fg = GetForegroundWindow();
  HWND anchor = nullptr;

  ClearMoveSizeRects(deadHwnd);

  // Get current desktopID
  GUID curDesk{};
  bool haveDesk = false;
  if (fg && IsWindow(fg)) {
    haveDesk = GetWindowDesktopIdSafe(fg, &curDesk);   // foreground's desktop
  }
  if (!haveDesk) {
    haveDesk = InitializeVirtualDesktopAPI() && GetCurrentDesktopId(&curDesk);  // fallback
  }
  if (!haveDesk) return nullptr; // Don't pick a fallback at all if getdesktop fails

  HMONITOR curMon = fg ? MonitorFromWindow(fg, MONITOR_DEFAULTTONULL) : nullptr;
  if (!curMon) {
    POINT p{};
    if (!GetCursorPos(&p)) p = POINT{0, 0};
    curMon = MonitorFromPoint(p, MONITOR_DEFAULTTONEAREST);
  }

  AcquireSRWLockExclusive(&g_store.tilingStateLock);

  for (auto it = g_store.tilingStateMap.begin(); it != g_store.tilingStateMap.end(); ) {
    const DesktopMonitorKey key = it->first;
    TilingState& st = it->second;

    // Erase dead hwnd & empty state
    // also marks whether current state has been touched or not (affected)
    size_t before = st.windows.size();
    st.windows.erase(std::remove(st.windows.begin(), st.windows.end(), deadHwnd), st.windows.end());
    bool affected = (st.windows.size() != before);

    if (st.windows.empty()) {
      it = g_store.tilingStateMap.erase(it);
      continue;
    }
    // ignoring last window (st.windows() = 1) case here
    // so that RetileOnResize() can pick up the erase + placement job later...

    // Pick anchor once: prefer foreground if it maps into this state's windows
    // otherwise: pick first entry in st.window with the same "monitor % desktopID" pair as fg
    bool keyMatches = (IsEqualGUID(key.desktopId, curDesk)) &&
                      (key.monitor == curMon);

    if (!anchor && keyMatches && affected) {
      if (fg && IsWindow(fg)) {
        HWND resolved = ResolveToTiledWindow(fg, st.windows);
        if (resolved && IsWindow(resolved)) anchor = resolved;
      }
      if (!anchor) {
        for (HWND w : st.windows) {
          if (w && IsWindow(w) && !IsIconic(w)) { anchor = w; break; }
        }
      }
      // If all windows are minimized (rare-case): 
      // skip minimiization check to at least return an anchor
      if (!anchor) {
        for (HWND w : st.windows) {
          if (w && IsWindow(w)) { anchor = w; break; }
        }
      }
  }

    ++it;
  }

  ReleaseSRWLockExclusive(&g_store.tilingStateLock);
  return anchor;
}

// Helper for HandleTrivialState()
static void EraseState(const DesktopMonitorKey & key){
  AcquireSRWLockExclusive(&g_store.tilingStateLock);
  g_store.tilingStateMap.erase(key);
  ReleaseSRWLockExclusive(&g_store.tilingStateLock);
}

// true: handled "<=1 windows in TilingState" case; false otherwise
// returns "true": caller should (probably) also return
static bool HandleTrivialState(const DesktopMonitorKey & key, TilingState & state, const RECT & workArea){
  if (state.windows.empty()){
    EraseState(key);
    return true;
  }

  else if (state.windows.size() == 1){
    PlaceWindow(state.windows[0], workArea);
    EraseState(key);
    return true;
  }

  return false;
}

static void LayoutMasterStackFromResize(const RECT& workArea, HWND resizedHwnd, bool haveResizeRects,
                                        const RECT& resizeStartRect, const RECT& resizeEndRect,
                                        TilingState& state, std::vector<RECT>& windowRects) {
  constexpr LONG kMinRetileSpan = 80;
  Axis masterAxis = g_settings.masterAxis;
  Axis stackAxis = OrthogonalAxis(masterAxis);
  LONG totalSize = AxisSpan(workArea, masterAxis);
  if (totalSize <= g_settings.tileGap + 1) return;

  size_t resizedIndex = FindWindowIndex(state.windows, resizedHwnd);
  if (resizedIndex == (size_t)-1) return;

  RECT resizedRect = {};
  if (!GetWindowFrameRect(resizedHwnd, &resizedRect)) return;

  LONG masterSize = std::max<LONG>(AxisSpan(resizedRect, masterAxis), kMinRetileSpan);
  if (resizedIndex != 0) {
    LONG candidateSize = AxisStart(resizedRect, masterAxis) - AxisStart(workArea, masterAxis) - g_settings.tileGap;
    LONG maxMaster = totalSize - g_settings.tileGap - 1;
    if (candidateSize >= 1 && candidateSize <= maxMaster) {
      masterSize = candidateSize;
    } else {
      RECT currentMasterRect = {};
      if (!GetWindowFrameRect(state.windows[0], &currentMasterRect)) return;
      masterSize = std::max<LONG>(AxisSpan(currentMasterRect, masterAxis), kMinRetileSpan);
    }
  }

  state.masterRatio = ClampDouble((double)masterSize / (double)(totalSize - g_settings.tileGap), 0.1, 0.9);

  LONG masterStart = AxisStart(workArea, masterAxis);
  LONG masterEnd = std::min<LONG>(masterStart + masterSize, AxisEnd(workArea, masterAxis) - g_settings.tileGap);
  RECT masterRect = ReplaceAxisRange(workArea, masterAxis, masterStart, masterEnd);
  RECT stackArea = ReplaceAxisRange(workArea, masterAxis, masterEnd + g_settings.tileGap, AxisEnd(workArea, masterAxis));

  size_t stackCount = state.windows.size() - 1;
  std::vector<LONG> measuredStackSizes;
  measuredStackSizes.reserve(stackCount);
  state.stackWeights.clear();
  state.stackWeights.reserve(stackCount);

  for (size_t i = 1; i < state.windows.size(); ++i) {
    RECT rect = {};
    if (!GetWindowFrameRect(state.windows[i], &rect)) {
      measuredStackSizes.push_back(1);
      state.stackWeights.push_back(1.0);
      continue;
    }

    LONG size = ClampSpanMin1(AxisSpan(rect, stackAxis));
    measuredStackSizes.push_back(size);
    state.stackWeights.push_back((double)size);
  }

  std::vector<LONG> stackSizes;
  if (stackCount > 0) {
    size_t fixedIndex = resizedIndex == 0 ? (size_t)-1 : resizedIndex - 1;
    LONG stackTotal = AxisSpan(stackArea, stackAxis);
    if (fixedIndex != (size_t)-1) {
      LONG fixedSize = std::max<LONG>(AxisSpan(resizedRect, stackAxis), kMinRetileSpan);

      bool preserveBeforeFixed = false;
      bool preserveAfterFixed = false;
      if (haveResizeRects) {
        LONG beforeStart = AxisStart(resizeStartRect, stackAxis);
        LONG beforeEnd = AxisEnd(resizeStartRect, stackAxis);
        LONG afterStart = AxisStart(resizeEndRect, stackAxis);
        LONG afterEnd = AxisEnd(resizeEndRect, stackAxis);
        bool startChanged = Differs(beforeStart, afterStart, 1);
        bool endChanged = Differs(beforeEnd, afterEnd, 1);

        preserveBeforeFixed = !startChanged && endChanged && fixedIndex > 0;
        preserveAfterFixed = startChanged && !endChanged && fixedIndex + 1 < stackCount;
      }

      stackSizes = (preserveBeforeFixed || preserveAfterFixed)
                       ? ComputeWeightedSizesWithFixedAnchored(stackTotal, g_settings.tileGap, state.stackWeights,
                                                              measuredStackSizes, fixedIndex, fixedSize,
                                                              preserveBeforeFixed, preserveAfterFixed)
                       : ComputeWeightedSizesWithFixed(stackTotal, g_settings.tileGap, state.stackWeights, fixedIndex, fixedSize);
    } else {
      stackSizes = ComputeWeightedSizes(stackTotal, g_settings.tileGap, state.stackWeights);
    }
  }

  windowRects.assign(state.windows.size(), workArea);
  windowRects[0] = masterRect;
  if (stackCount > 0) {
    std::vector<RECT> stackRects;
    LayoutStripFromSizes(stackArea, stackAxis, stackSizes, stackRects);
    for (size_t i = 0; i < stackRects.size(); ++i) {
      windowRects[i + 1] = stackRects[i];
    }
  }
}

struct RetileContext {
  DesktopMonitorKey key{};
  RECT workArea{};
  TilingState state;
  HWND resizedHwnd = nullptr;
  RECT resizeStartRect{};
  RECT resizeEndRect{};
  bool haveResizeRects = false;
  bool dragUntiledWindow = false;
};

bool StopIfTrivialState(RetileContext* context) {
  return HandleTrivialState(context->key, context->state, context->workArea);
}

void RemoveWindowFromState(TilingState* state, HWND hwnd) {
  state->windows.erase(std::remove(state->windows.begin(), state->windows.end(), hwnd), state->windows.end());
}

void RetargetResizedWindow(RetileContext* context) {
  if (context->state.windows.empty() || ContainsWindow(context->state.windows, context->resizedHwnd)) return;
  context->resizedHwnd = context->state.windows.front();
  context->state.stackWeights.clear();
}

bool TryBuildRetileContext(HWND hwnd, RetileContext* outContext) {
  if (!outContext || g_settings.retileSuspended || !IsWindow(hwnd)) return false;

  hwnd = NormalizeTrackedWindow(hwnd);
  if (!hwnd) return false;

  RetileContext context;
  if (!TryGetDesktopMonitorContext(hwnd, &context.key, &context.workArea)) return false;
  if (!TryGetSavedState(context.key, &context.state)) {
    Wh_Log(L"Tiling not set up for current desktop");
    return false;
  }

  context.resizedHwnd = hwnd;
  if (!ContainsWindow(context.state.windows, context.resizedHwnd)) {
    Wh_Log(L"Resized window not part of current tiling state");
    return false;
  }

  *outContext = std::move(context);
  return true;
}

bool PrepareRetileContext(RetileContext* context) {
  RemoveUntiledWindows(context->state.windows);
  return !StopIfTrivialState(context);
}

bool ConsumeResizeOutcome(RetileContext* context) {
  context->haveResizeRects = TakeResizeRects(context->resizedHwnd, &context->resizeStartRect, &context->resizeEndRect);
  if (!context->haveResizeRects || !IsMoveOnlyRectChange(context->resizeStartRect, context->resizeEndRect)) {
    return true;
  }

  context->dragUntiledWindow = true;
  MarkWindowUntiled(context->resizedHwnd);
  RemoveWindowFromState(&context->state, context->resizedHwnd);
  if (StopIfTrivialState(context)) return false;

  RetargetResizedWindow(context);
  StoreTilingState(context->key, context->state);
  return true;
}

void RemoveStaleStateWindows(TilingState* state) {
  state->windows.erase(std::remove_if(state->windows.begin(), state->windows.end(), [&](HWND hwnd) {
                        RECT rect = {};
                        return !IsWindow(hwnd) || !GetWindowFrameRect(hwnd, &rect);
                      }),
                      state->windows.end());
}

bool PruneRetileState(RetileContext* context) {
  RemoveStaleStateWindows(&context->state);
  RemoveUntiledWindows(context->state.windows);
  if (StopIfTrivialState(context)) return false;

  RetargetResizedWindow(context);
  for (auto it = context->state.windows.begin(); it != context->state.windows.end();) {
    HWND window = *it;
    RECT rect = {};
    if (!GetWindowFrameRect(window, &rect)) {
      it = context->state.windows.erase(it);
      if (StopIfTrivialState(context)) return false;
      RetargetResizedWindow(context);
      continue;
    }

    if (MonitorFromRect(&rect, MONITOR_DEFAULTTONULL) == context->key.monitor) {
      ++it;
      continue;
    }

    if (!IsIconic(window)) {
      Wh_Log(L"Window not on monitor && not minimized; retile as fallback");
      TileWindows(nullptr, false);
      return false;
    }

    it = context->state.windows.erase(it);
    if (StopIfTrivialState(context)) return false;
    RetargetResizedWindow(context);
  }

  return true;
}

bool ApplyRetileContext(RetileContext* context) {
  std::vector<RECT> windowRects;
  LayoutMasterStackFromResize(context->workArea, context->resizedHwnd, context->haveResizeRects,
                              context->resizeStartRect, context->resizeEndRect, context->state, windowRects);
  if (windowRects.size() != context->state.windows.size()) return false;

  for (size_t i = 0; i < context->state.windows.size(); ++i) {
    PlaceWindow(context->state.windows[i], windowRects[i]);
  }

  StoreTilingState(context->key, context->state);
  return true;
}

void RetileFromResize(HWND hwnd) {
  RetileContext context;
  if (!TryBuildRetileContext(hwnd, &context)) return;
  if (!PrepareRetileContext(&context)) return;
  if (!ConsumeResizeOutcome(&context)) return;
  if (!g_settings.enableResizeRetile && !context.dragUntiledWindow) return;
  if (!PruneRetileState(&context)) return;
  ApplyRetileContext(&context);
}

void OnWindowResizeEnd(HWND hwnd) {
  if (g_settings.retileSuspended || !IsWindow(hwnd)) return;
  if (InterlockedCompareExchange(&g_store.retileInProgress, 1, 0) != 0) {
    return;
  }

  if (!g_hotkeyThread.threadId) {
    InterlockedExchange(&g_store.retileInProgress, 0);
    return;
  }

  if (!PostThreadMessage(g_hotkeyThread.threadId, WM_APP_RETILE, reinterpret_cast<WPARAM>(hwnd), 0)) {
    InterlockedExchange(&g_store.retileInProgress, 0);
  }
}

static void RequestTileWindows(HWND triggerHwnd) {
  if (g_settings.retileSuspended) return;
  if (!g_hotkeyThread.threadId) return;

  PostThreadMessage(g_hotkeyThread.threadId, WM_APP_TILE, reinterpret_cast<WPARAM>(triggerHwnd), 0);
}

static void RequestPruneDestroyed(HWND dead) {
  if (g_settings.retileSuspended) return;
  if (!dead) return;
  if (!g_hotkeyThread.threadId) return;
  if (InterlockedCompareExchange(&g_store.retileInProgress, 1, 0) != 0) return;

  if (!PostThreadMessage(g_hotkeyThread.threadId, WM_APP_PRUNE_DESTROY, (WPARAM)dead, 0)) {
    InterlockedExchange(&g_store.retileInProgress, 0);   
  }
}

struct WindowEventContext {
  DWORD event = 0;
  HWND hwnd = nullptr;
  HWND trackedHwnd = nullptr;
  HWND candidateHwnd = nullptr;
  bool tracked = false;
};

bool IsHandledWindowEvent(DWORD event) {
  switch (event) {
    case EVENT_SYSTEM_MOVESIZESTART:
    case EVENT_SYSTEM_MOVESIZEEND:
    case EVENT_SYSTEM_MINIMIZESTART:
    case EVENT_SYSTEM_MINIMIZEEND:
    case EVENT_OBJECT_DESTROY:
    case EVENT_OBJECT_SHOW:
    case EVENT_OBJECT_HIDE:
    case EVENT_OBJECT_UNCLOAKED:
      return true;
    default:
      return false;
  }
}

bool TryBuildWindowEventContext(DWORD event, HWND hwnd, LONG idObject, LONG idChild, WindowEventContext* outContext) {
  if (!outContext || !IsHandledWindowEvent(event)) return false;
  if (!hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) return false;

  outContext->event = event;
  outContext->hwnd = hwnd;
  outContext->trackedHwnd = NormalizeTrackedWindow(hwnd);
  outContext->candidateHwnd = outContext->trackedHwnd ? outContext->trackedHwnd : hwnd;
  outContext->tracked = outContext->trackedHwnd && IsWindowTrackedInAnyState(outContext->trackedHwnd);
  return true;
}

void CacheMoveSizeRect(HWND hwnd, bool isStartRect) {
  AcquireSRWLockExclusive(&g_store.moveSizeRectsLock);
  RECT rect = {};
  if (GetWindowFrameRect(hwnd, &rect)) {
    if (isStartRect) {
      g_store.moveSizeStartRects[hwnd] = rect;
    } else {
      g_store.moveSizeEndRects[hwnd] = rect;
    }
  }
  ReleaseSRWLockExclusive(&g_store.moveSizeRectsLock);
}

bool HandleMoveSizeEvent(const WindowEventContext& context) {
  if (context.event != EVENT_SYSTEM_MOVESIZESTART && context.event != EVENT_SYSTEM_MOVESIZEEND) return false;
  if (!context.trackedHwnd || !context.tracked) return true;

  CacheMoveSizeRect(context.trackedHwnd, context.event == EVENT_SYSTEM_MOVESIZESTART);
  if (context.event == EVENT_SYSTEM_MOVESIZEEND) {
    OnWindowResizeEnd(context.trackedHwnd);
  }
  return true;
}

bool HandleMinimizeStartEvent(const WindowEventContext& context) {
  if (context.event != EVENT_SYSTEM_MINIMIZESTART) return false;

  ClearWindowUntiled(context.hwnd);
  if (context.trackedHwnd && context.tracked && IsIconic(context.trackedHwnd)) {
    OnWindowResizeEnd(context.trackedHwnd);
  }
  return true;
}

bool HandleDestroyOrHideEvent(const WindowEventContext& context) {
  if (context.event != EVENT_OBJECT_DESTROY && context.event != EVENT_OBJECT_HIDE) return false;

  if (context.event == EVENT_OBJECT_DESTROY) {
    ClearWindowUntiled(context.hwnd);
  }
  if (context.tracked) {
    RequestPruneDestroyed(context.hwnd);
  }
  return true;
}

bool ShouldTileCandidateWindow(const WindowEventContext& context) {
  return g_settings.enableTileNewWin && !context.tracked && CouldBeTileEligible(context.candidateHwnd);
}

bool HandleTileNewWindowEvent(const WindowEventContext& context) {
  if (context.event == EVENT_SYSTEM_MINIMIZEEND) {
    if (ShouldTileCandidateWindow(context)) {
      RequestTileWindows(context.candidateHwnd);
    }
    return true;
  }

  if (context.event != EVENT_OBJECT_SHOW && context.event != EVENT_OBJECT_UNCLOAKED) {
    return false;
  }

  if (ShouldTileCandidateWindow(context)) {
    RequestTileWindows(context.candidateHwnd);
  }
  return true;
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD) {
  WindowEventContext context;
  if (!TryBuildWindowEventContext(event, hwnd, idObject, idChild, &context)) return;
  if (HandleMoveSizeEvent(context)) return;
  if (HandleMinimizeStartEvent(context)) return;
  if (HandleDestroyOrHideEvent(context)) return;
  HandleTileNewWindowEvent(context);
}

void InstallWinEventHookRange(HWINEVENTHOOK* hook, DWORD eventMin, DWORD eventMax, PCWSTR errorMessage) {
  if (*hook) return;

  *hook = SetWinEventHook(eventMin, eventMax, nullptr, WinEventProc, 0, 0,
                          WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
  if (!*hook) Wh_Log(L"%s", errorMessage);
}

void RemoveWinEventHookIfNeeded(HWINEVENTHOOK* hook) {
  if (!*hook) return;
  UnhookWinEvent(*hook);
  *hook = nullptr;
}

void InstallWinEventHooks() {
  InstallWinEventHookRange(&g_hooks.moveSizeHook, EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND,
                           L"Failed to install move/size hook");
  InstallWinEventHookRange(&g_hooks.minimizeHook, EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND,
                           L"Failed to install minimization hook");
  InstallWinEventHookRange(&g_hooks.hideDestroyHook, EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE,
                           L"Failed to install hide/destroy hook");
  InstallWinEventHookRange(&g_hooks.cloakHook, EVENT_OBJECT_UNCLOAKED, EVENT_OBJECT_UNCLOAKED,
                           L"Failed to install uncloak hook");
}

void RemoveWinEventHooks() {
  RemoveWinEventHookIfNeeded(&g_hooks.moveSizeHook);
  RemoveWinEventHookIfNeeded(&g_hooks.minimizeHook);
  RemoveWinEventHookIfNeeded(&g_hooks.hideDestroyHook);
  RemoveWinEventHookIfNeeded(&g_hooks.cloakHook);
}

void ResetTilingStateMemory() {
  AcquireSRWLockExclusive(&g_store.tilingStateLock);
  g_store.tilingStateMap.clear();
  ReleaseSRWLockExclusive(&g_store.tilingStateLock);
}

//=============================================================================
// Hotkey Thread
//=============================================================================

// Parse single character to virtual key code
UINT ParseSingleCharKey(PCWSTR str) {
  if (!str || !str[0]) return 0;
  wchar_t c = str[0];

  if (c >= L'A' && c <= L'Z') return c;
  if (c >= L'a' && c <= L'z') return c - L'a' + L'A';
  if (c >= L'0' && c <= L'9') return c;

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

  if (c == L'`' || c == L'~') return VK_OEM_3;
  if (c == L'-' || c == L'_') return VK_OEM_MINUS;
  if (c == L'=' || c == L'+') return VK_OEM_PLUS;
  if (c == L'[' || c == L'{') return VK_OEM_4;
  if (c == L']' || c == L'}') return VK_OEM_6;
  if (c == L'\\' || c == L'|') return VK_OEM_5;
  if (c == L';' || c == L':') return VK_OEM_1;
  if (c == L'\'' || c == L'"') return VK_OEM_7;
  if (c == L',' || c == L'<') return VK_OEM_COMMA;
  if (c == L'.' || c == L'>') return VK_OEM_PERIOD;
  if (c == L'/' || c == L'?') return VK_OEM_2;
  if (c == L' ') return VK_SPACE;

  return 0;
}

void LoadSettings() {
  PCWSTR version = Wh_GetStringSetting(L"WindowsVersion");
  g_desktopApi.windowsVersionIndex = ParseWindowsVersion(version);
  Wh_FreeStringSetting(version);

  g_settings.tilingModifiers = ReadStringSetting(L"TilingModifier", ParseModifiers, (UINT)MOD_ALT);

  g_settings.tileMargin = ReadClampedIntSetting(L"TileMargin", 0, 100, 4);
  g_settings.tileGap = ReadClampedIntSetting(L"TileGap", 0, 100, 4);
  g_settings.masterPercent = ReadClampedIntSetting(L"MasterPercent", 1, 99, 50);

  g_settings.enableResizeRetile = Wh_GetIntSetting(L"EnableResizeRetile") != 0;
  g_settings.enableTileNewWin = Wh_GetIntSetting(L"EnableTileNewWin") != 0;
  g_settings.retileSuspended = false;

  g_settings.tileKey = ReadStringSetting(L"TileKey", ParseSingleCharKey, (UINT)'D');
  g_settings.retileToggleKey = ReadStringSetting(L"RetileToggleKey", ParseSingleCharKey, (UINT)'R');
  g_settings.swapMasterKey = ReadStringSetting(L"SwapMasterKey", ParseSingleCharKey, (UINT)'M');

  PCWSTR orientation = Wh_GetStringSetting(L"LayoutOrientation");
  g_settings.masterAxis = (wcscmp(orientation, L"vertical") == 0) ? Axis::X : Axis::Y;
  Wh_FreeStringSetting(orientation);
}

void RegisterConfiguredHotkeys() {
  RegisterHotKey(nullptr, HK_TILE, g_settings.tilingModifiers, g_settings.tileKey);
  RegisterHotKey(nullptr, HK_SWAP_MASTER, g_settings.tilingModifiers, g_settings.swapMasterKey);
  RegisterHotKey(nullptr, HK_RETILE_TOGGLE, g_settings.tilingModifiers, g_settings.retileToggleKey);
}

void UnregisterConfiguredHotkeys() {
  UnregisterHotKey(nullptr, HK_TILE);
  UnregisterHotKey(nullptr, HK_SWAP_MASTER);
  UnregisterHotKey(nullptr, HK_RETILE_TOGGLE);
}

void MaybeTileCurrentWorkplaceOnStartup() {
  if (!g_settings.enableTileNewWin) return;

  if (InterlockedCompareExchange(&g_store.retileInProgress, 1, 0) == 0) {
    TileWindows(nullptr, false);
    InterlockedExchange(&g_store.retileInProgress, 0);
  }
  Wh_Log(L"TileNewWin Enabled: Tiled current workplace on startup");
}

void ProcessRetileMessage(HWND resizedHwnd) {
  RetileFromResize(resizedHwnd);
  InterlockedExchange(&g_store.retileInProgress, 0);
}

void ProcessTileMessage(HWND triggerHwnd, bool allowWhenSuspended = false) {
  if (g_settings.retileSuspended && !allowWhenSuspended) return;
  if (InterlockedCompareExchange(&g_store.retileInProgress, 1, 0) != 0) return;

  TileWindows(triggerHwnd, allowWhenSuspended);
  InterlockedExchange(&g_store.retileInProgress, 0);
}

void ProcessPruneDestroyedMessage(HWND dead) {
  HWND anchor = PruneDestroyedAndPickAnchor(dead);
  if (anchor) RetileFromResize(anchor);
  InterlockedExchange(&g_store.retileInProgress, 0);
}

void ToggleRetileSuspension() {
  g_settings.retileSuspended = !g_settings.retileSuspended;
  if (g_settings.retileSuspended) {
    RemoveWinEventHooks();
    InterlockedExchange(&g_store.retileInProgress, 0);
    ResetTilingStateMemory();
    return;
  }

  InstallWinEventHooks();
}

void HandleHotkeyMessage(UINT hotkeyId) {
  switch (hotkeyId) {
    case HK_TILE:
      ProcessTileMessage(nullptr, true);
      return;
    case HK_SWAP_MASTER:
      SwapMaster();
      return;
    case HK_RETILE_TOGGLE:
      ToggleRetileSuspension();
      return;
    default:
      return;
  }
}

bool HandleHotkeyThreadMessage(const MSG& msg) {
  switch (msg.message) {
    case WM_QUIT:
      return false;
    case WM_APP_RETILE:
      ProcessRetileMessage(reinterpret_cast<HWND>(msg.wParam));
      return true;
    case WM_APP_TILE:
      ProcessTileMessage(reinterpret_cast<HWND>(msg.wParam));
      return true;
    case WM_APP_PRUNE_DESTROY:
      ProcessPruneDestroyedMessage(reinterpret_cast<HWND>(msg.wParam));
      return true;
    case WM_HOTKEY:
      HandleHotkeyMessage(static_cast<UINT>(msg.wParam));
      return true;
    default:
      return true;
  }
}

void CleanupHotkeyThread() {
  UnregisterConfiguredHotkeys();
  RemoveWinEventHooks();
  CleanupVirtualDesktopAPI();
  CoUninitialize();
}

DWORD WINAPI HotkeyThreadProc(LPVOID) {
  g_hotkeyThread.threadId = GetCurrentThreadId();
  Wh_Log(L"Hotkey thread started, thread ID: %lu", g_hotkeyThread.threadId);

  HRESULT coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
  Wh_Log(L"CoInitializeEx result: 0x%08X", coHr);

  MSG msg;
  PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);
  SetEvent(g_hotkeyThread.readyEvent);

  if (!g_settings.retileSuspended) {
    InstallWinEventHooks();
  }

  RegisterConfiguredHotkeys();
  Wh_Log(L"Hotkeys registered");
  MaybeTileCurrentWorkplaceOnStartup();

  while (!g_hotkeyThread.stopRequested) {
    if (MsgWaitForMultipleObjects(0, nullptr, FALSE, INFINITE, QS_ALLINPUT) != WAIT_OBJECT_0) {
      continue;
    }

    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      if (!HandleHotkeyThreadMessage(msg)) {
        CleanupHotkeyThread();
        return 0;
      }
    }
  }

  CleanupHotkeyThread();
  return 0;
}

bool StartHotkeyThread() {
  g_hotkeyThread.readyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
  g_hotkeyThread.thread = CreateThread(nullptr, 0, HotkeyThreadProc, nullptr, 0, nullptr);

  if (!g_hotkeyThread.thread || WaitForSingleObject(g_hotkeyThread.readyEvent, 5000) != WAIT_OBJECT_0) {
    Wh_Log(L"Failed to start hotkey thread");
    if (g_hotkeyThread.thread) {
      CloseHandle(g_hotkeyThread.thread);
      g_hotkeyThread.thread = nullptr;
    }
    CloseHandle(g_hotkeyThread.readyEvent);
    g_hotkeyThread.readyEvent = nullptr;
    return false;
  }

  CloseHandle(g_hotkeyThread.readyEvent);
  g_hotkeyThread.readyEvent = nullptr;
  return true;
}

void StopHotkeyThread() {
  g_hotkeyThread.stopRequested = true;
  InterlockedExchange(&g_store.retileInProgress, 0);

  g_desktopApi.initialized = false;

  if (g_hotkeyThread.threadId) {
    PostThreadMessage(g_hotkeyThread.threadId, WM_QUIT, 0, 0);
  }

  if (g_hotkeyThread.thread) {
    DWORD waitResult = WaitForSingleObject(g_hotkeyThread.thread, 5000);
    if (waitResult == WAIT_TIMEOUT) {
      Wh_Log(L"WARNING: Hotkey thread cleanup timeout, thread may be stuck in CoUninitialize");
    }
    CloseHandle(g_hotkeyThread.thread);
    g_hotkeyThread.thread = nullptr;
  }

  g_hotkeyThread.threadId = 0;
  g_hotkeyThread.stopRequested = false;
}

//=============================================================================
// Windhawk Tool Mod Entry Points
//=============================================================================

BOOL WhTool_ModInit() {
  Wh_Log(L"Tiling Helper mod initializing...");
  LoadSettings();
  if (!StartHotkeyThread()) {
    Wh_Log(L"Failed to start hotkey thread");
    return FALSE;
  }
  Wh_Log(L"Tiling Helper mod initialized successfully");
  return TRUE;
}

void WhTool_ModUninit() {
  Wh_Log(L"Tiling Helper mod uninitializing...");
  StopHotkeyThread();
  Wh_Log(L"Tiling Helper mod uninitialized");
}

void WhTool_ModSettingsChanged() {
  Wh_Log(L"Settings changed, reloading...");
  StopHotkeyThread();
  ResetTilingStateMemory();
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

void WINAPI EntryPoint_Hook() { ExitThread(0); }

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

  WCHAR commandLine[MAX_PATH + 2 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
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
