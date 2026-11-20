// ==WindhawkMod==
// @id              virtual-desktop-wallpaper-switch
// @name            Virtual Desktop Wallpaper Switch
// @description     A Windhawk mod to set unique backgrounds for each virtual desktop. (Project FluxDesk)
// @version         1.0.0
// @author          Dev-StudioCode
// @github          https://github.com/Dev-StudioCode
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lshlwapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# ⚡ Virtual Desktop Wallpaper Switch (Project FluxDesk)

A high-performance Windhawk mod injected directly into **`explorer.exe`** that assigns, preloads, and transitions distinct wallpapers across all Windows 11 virtual desktops.

### 🌟 Key Benefits Over External Desktop Apps
- **Zero-Latency In-Process Execution**: Runs inside Windows Explorer itself; no background `.exe` process polling or running separate message loops.
- **Instant Touchpad Gesture Preloading**: Uses undocumented internal COM interfaces (`IVirtualDesktopManagerInternal::SetDesktopWallpaper`) to register wallpapers directly into Desktop Window Manager (DWM). When performing a **4-finger swipe**, transitions are buttery-smooth with **zero black flicker**.
- **Native Event Sinks**: Hooks into shell notifications (`IVirtualDesktopNotificationService`) and intercepts virtual desktop switch calls without requiring external global hooks or polling timers.
- **Configurable**: Define custom wallpaper folders or assign explicit wallpapers per virtual desktop directly from the Windhawk settings panel.

### ⚙️ How It Works
1. When loaded into `explorer.exe`, `Wh_ModInit` locates the Windows Immersive Shell service provider.
2. It queries `IVirtualDesktopManagerInternal` and `IVirtualDesktopNotificationService`.
3. It preloads each virtual desktop with its designated wallpaper in DWM.
4. When you switch desktops (via 4-finger trackpad swipe, `Win+Ctrl+Left/Right`, or Task View), the notification callback and hooked switch methods automatically guarantee the correct wallpaper is active.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- wallpaperFolder: ""
  $name: Wallpaper Folder
  $description: Path to a folder containing wallpapers (.jpg, .png, .bmp, .webp). Images will be assigned across your virtual desktops.
- desktop1Wallpaper: ""
  $name: Desktop 1 Wallpaper
  $description: Specific wallpaper path for Virtual Desktop 1 (overrides folder mode if set).
- desktop2Wallpaper: ""
  $name: Desktop 2 Wallpaper
  $description: Specific wallpaper path for Virtual Desktop 2.
- desktop3Wallpaper: ""
  $name: Desktop 3 Wallpaper
  $description: Specific wallpaper path for Virtual Desktop 3.
- desktop4Wallpaper: ""
  $name: Desktop 4 Wallpaper
  $description: Specific wallpaper path for Virtual Desktop 4.
- shuffle: true
  $name: Shuffle Wallpapers
  $description: Randomize the order of wallpapers assigned from the folder.
- enablePreloading: true
  $name: Preload Wallpapers in DWM
  $description: Pre-registers wallpapers for all desktops so 4-finger touchpad swipes have zero black/blank frame latency.
- fallbackDesktopWallpaperAPI: true
  $name: Shell Wallpaper Fallback
  $description: Also updates active wallpaper via IDesktopWallpaper and SystemParametersInfo on desktop switch.
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <objbase.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <windows.h>

#if __has_include(<windhawk_api.h>)
#include <windhawk_api.h>
#elif __has_include("windhawk_api.h")
#include "windhawk_api.h"
#endif

#include <algorithm>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

// Ensure MIDL_INTERFACE is available on all compilers
#ifndef MIDL_INTERFACE
#define MIDL_INTERFACE(x) struct
#endif

// ============================================================================
// Undocumented Windows Shell COM Definitions & GUIDs
// ============================================================================

// Forward declarations
struct IVirtualDesktop;
struct IVirtualDesktopManagerInternal;
struct IVirtualDesktopNotification;
struct IVirtualDesktopNotificationService;

// CLSID_ImmersiveShell: {C2F03A33-21F5-47FA-B4BB-156362A2F239}
static const CLSID CLSID_ImmersiveShell = {
    0xC2F03A33,
    0x21F5,
    0x47FA,
    {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39}};

// CLSID_VirtualDesktopManagerInternal: {C5E0CDCA-7B6E-41B2-9FC4-D93975CC467B}
static const CLSID CLSID_VirtualDesktopManagerInternal = {
    0xC5E0CDCA,
    0x7B6E,
    0x41B2,
    {0x9F, 0xC4, 0xD9, 0x39, 0x75, 0xCC, 0x46, 0x7B}};

// CLSID_VirtualDesktopNotificationService:
// {A501FDEC-4A09-464C-AE4E-1B9C21B84918}
static const CLSID CLSID_VirtualDesktopNotificationService = {
    0xA501FDEC,
    0x4A09,
    0x464C,
    {0xAE, 0x4E, 0x1B, 0x9C, 0x21, 0xB8, 0x49, 0x18}};

// IID_IVirtualDesktopNotificationService:
// {0CD45E71-D927-4F15-8B0A-8FEF525337BF}
static const IID IID_IVirtualDesktopNotificationService = {
    0x0CD45E71,
    0xD927,
    0x4F15,
    {0x8B, 0x0A, 0x8F, 0xEF, 0x52, 0x53, 0x37, 0xBF}};

// IID_IVirtualDesktopNotification: {C179334C-4295-40D3-BEA1-C654D965605A}
static const IID IID_IVirtualDesktopNotification = {
    0xC179334C,
    0x4295,
    0x40D3,
    {0xBE, 0xA1, 0xC6, 0x54, 0xD9, 0x65, 0x60, 0x5A}};

// IID_IVirtualDesktopManagerInternal (Windows 11 22H2 / 23H2 / 24H2):
// {53F5CA0B-158F-4124-900C-057158060B27}
static const IID IID_IVirtualDesktopManagerInternal_Win11 = {
    0x53F5CA0B,
    0x158F,
    0x4124,
    {0x90, 0x0C, 0x05, 0x71, 0x58, 0x06, 0x0B, 0x27}};

// IID_IVirtualDesktopManagerInternal (Windows 11 21H2 Fallback):
// {B2F925B9-5A0F-4D12-BE43-08B7E470D461}
static const IID IID_IVirtualDesktopManagerInternal_Win11_21H2 = {
    0xB2F925B9,
    0x5A0F,
    0x4D12,
    {0xBE, 0x43, 0x08, 0xB7, 0xE4, 0x70, 0xD4, 0x61}};

// IID_IVirtualDesktop (Windows 11): {3F07F4BE-B107-441A-AF0F-39D82529072C}
static const IID IID_IVirtualDesktop_Win11 = {
    0x3F07F4BE,
    0xB107,
    0x441A,
    {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}};

// IID_IObjectArray: {92CA9DCD-5622-4BBA-A805-5E9F541BD8C9}
static const IID IID_IObjectArray_Local = {
    0x92CA9DCD,
    0x5622,
    0x4BBA,
    {0xA8, 0x05, 0x5E, 0x9F, 0x54, 0x1B, 0xD8, 0xC9}};

// CLSID_DesktopWallpaper: {C2CF03AC-5759-460B-8848-822D425CE796}
static const CLSID CLSID_DesktopWallpaper_Local = {
    0xC2CF03AC,
    0x5759,
    0x460B,
    {0x88, 0x48, 0x82, 0x2D, 0x42, 0x5C, 0xE7, 0x96}};

// IID_IDesktopWallpaper: {B92CA5F7-3480-4E4A-A9FB-4F5572429DE7}
static const IID IID_IDesktopWallpaper_Local = {
    0xB92CA5F7,
    0x3480,
    0x4E4A,
    {0xA9, 0xFB, 0x4F, 0x55, 0x72, 0x42, 0x9D, 0xE7}};

// ============================================================================
// WinRT String Function Types (Dynamic binding to prevent linking hurdles)
// ============================================================================
typedef struct HSTRING__ *HSTRING;
typedef HRESULT(WINAPI *WindowsCreateString_t)(PCWSTR sourceString, UINT length,
                                               HSTRING *string);
typedef HRESULT(WINAPI *WindowsDeleteString_t)(HSTRING string);
typedef PCWSTR(WINAPI *WindowsGetStringRawBuffer_t)(HSTRING string,
                                                    UINT *length);

static WindowsCreateString_t g_pfnWindowsCreateString = nullptr;
static WindowsDeleteString_t g_pfnWindowsDeleteString = nullptr;
static WindowsGetStringRawBuffer_t g_pfnWindowsGetStringRawBuffer = nullptr;

static void InitWinRTStringFunctions() {
  HMODULE hComBase = GetModuleHandleW(L"combase.dll");
  if (!hComBase)
    hComBase = LoadLibraryW(L"combase.dll");
  if (hComBase) {
    g_pfnWindowsCreateString =
        (WindowsCreateString_t)GetProcAddress(hComBase, "WindowsCreateString");
    g_pfnWindowsDeleteString =
        (WindowsDeleteString_t)GetProcAddress(hComBase, "WindowsDeleteString");
    g_pfnWindowsGetStringRawBuffer =
        (WindowsGetStringRawBuffer_t)GetProcAddress(
            hComBase, "WindowsGetStringRawBuffer");
  }
}

// Scoped HSTRING RAII wrapper
class ScopedHString {
private:
  HSTRING m_hstring = nullptr;

public:
  ScopedHString(PCWSTR str) {
    if (str && g_pfnWindowsCreateString) {
      g_pfnWindowsCreateString(str, (UINT)wcslen(str), &m_hstring);
    }
  }
  ~ScopedHString() {
    if (m_hstring && g_pfnWindowsDeleteString) {
      g_pfnWindowsDeleteString(m_hstring);
    }
  }
  HSTRING Get() const { return m_hstring; }
};

// ============================================================================
// COM Interface Declarations
// ============================================================================

MIDL_INTERFACE("3F07F4BE-B107-441A-AF0F-39D82529072C")
IVirtualDesktop : public IUnknown {
public:
  virtual HRESULT STDMETHODCALLTYPE IsViewVisible(IUnknown * pView,
                                                  BOOL * pfVisible) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetId(GUID * pGuid) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetName(HSTRING * pName) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetWallpaperPath(HSTRING * pPath) = 0;
  virtual HRESULT STDMETHODCALLTYPE IsRemote(BOOL * pfRemote) = 0;
};

MIDL_INTERFACE("53F5CA0B-158F-4124-900C-057158060B27")
IVirtualDesktopManagerInternal : public IUnknown {
public:
  virtual HRESULT STDMETHODCALLTYPE GetCount(UINT * pCount) = 0;
  virtual HRESULT STDMETHODCALLTYPE MoveViewToDesktop(
      IUnknown * pView, IVirtualDesktop * pDesktop) = 0;
  virtual HRESULT STDMETHODCALLTYPE CanViewMoveDesktops(IUnknown * pView,
                                                        BOOL * pfCanMove) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetCurrentDesktop(IVirtualDesktop *
                                                      *ppDesktop) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetDesktops(IObjectArray * *ppDesktops) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetAdjacentDesktop(
      IVirtualDesktop * pFrom, int direction, IVirtualDesktop **ppDesktop) = 0;
  virtual HRESULT STDMETHODCALLTYPE SwitchDesktop(IVirtualDesktop *
                                                  pDesktop) = 0;
  virtual HRESULT STDMETHODCALLTYPE SwitchDesktopAndMoveForegroundView(
      IVirtualDesktop * pDesktop) = 0;
  virtual HRESULT STDMETHODCALLTYPE CreateDesktop(IVirtualDesktop *
                                                  *ppDesktop) = 0;
  virtual HRESULT STDMETHODCALLTYPE MoveDesktop(IVirtualDesktop * pDesktop,
                                                int nIndex) = 0;
  virtual HRESULT STDMETHODCALLTYPE RemoveDesktop(
      IVirtualDesktop * pDesktop, IVirtualDesktop * pFallback) = 0;
  virtual HRESULT STDMETHODCALLTYPE FindDesktop(
      const GUID *pDesktopId, IVirtualDesktop **ppDesktop) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetDesktopSwitchIncludeExcludeViews(
      IVirtualDesktop * pDesktop, IObjectArray * *ppViews1,
      IObjectArray * *ppViews2) = 0;
  virtual HRESULT STDMETHODCALLTYPE SetDesktopName(IVirtualDesktop * pDesktop,
                                                   HSTRING hstrName) = 0;
  virtual HRESULT STDMETHODCALLTYPE SetDesktopWallpaper(
      IVirtualDesktop * pDesktop, HSTRING hstrPath) = 0;
  virtual HRESULT STDMETHODCALLTYPE UpdateWallpaperPathForAllDesktops(
      HSTRING hstrPath) = 0;
  virtual HRESULT STDMETHODCALLTYPE CopyDesktopState(IUnknown * pView0,
                                                     IUnknown * pView1) = 0;
  virtual HRESULT STDMETHODCALLTYPE CreateRemoteDesktop(
      HSTRING hstrPath, IVirtualDesktop * *ppDesktop) = 0;
  virtual HRESULT STDMETHODCALLTYPE SwitchRemoteDesktop(
      IVirtualDesktop * pDesktop, void *pSwitchType) = 0;
  virtual HRESULT STDMETHODCALLTYPE SwitchDesktopWithAnimation(IVirtualDesktop *
                                                               pDesktop) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetLastActiveDesktop(IVirtualDesktop *
                                                         *ppDesktop) = 0;
  virtual HRESULT STDMETHODCALLTYPE WaitForAnimationToComplete() = 0;
};

MIDL_INTERFACE("C179334C-4295-40D3-BEA1-C654D965605A")
IVirtualDesktopNotification : public IUnknown {
public:
  virtual HRESULT STDMETHODCALLTYPE VirtualDesktopCreated(IVirtualDesktop *
                                                          pDesktop) = 0;
  virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyBegin(
      IVirtualDesktop * pDesktopDestroyed,
      IVirtualDesktop * pDesktopFallback) = 0;
  virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyFailed(
      IVirtualDesktop * pDesktopDestroyed,
      IVirtualDesktop * pDesktopFallback) = 0;
  virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyed(
      IVirtualDesktop * pDesktopDestroyed,
      IVirtualDesktop * pDesktopFallback) = 0;
  virtual HRESULT STDMETHODCALLTYPE ViewVirtualDesktopChanged(IUnknown *
                                                              pView) = 0;
  // On Windows 11 22H2+ this receives (IObjectArray* pMonitors,
  // IVirtualDesktop* pDesktopOld, IVirtualDesktop* pDesktopNew) On Windows 11
  // 21H2 / Windows 10 it receives (IVirtualDesktop* pDesktopOld,
  // IVirtualDesktop* pDesktopNew)
  virtual HRESULT STDMETHODCALLTYPE CurrentVirtualDesktopChanged(
      void *arg1, void *arg2, void *arg3) = 0;
};

MIDL_INTERFACE("0CD45E71-D927-4F15-8B0A-8FEF525337BF")
IVirtualDesktopNotificationService : public IUnknown {
public:
  virtual HRESULT STDMETHODCALLTYPE Register(
      IVirtualDesktopNotification * pNotification, DWORD * pdwCookie) = 0;
  virtual HRESULT STDMETHODCALLTYPE Unregister(DWORD dwCookie) = 0;
};

// ============================================================================
// Mod Configuration & State Management
// ============================================================================

struct ModSettings {
  std::wstring wallpaperFolder;
  std::wstring desktop1Wallpaper;
  std::wstring desktop2Wallpaper;
  std::wstring desktop3Wallpaper;
  std::wstring desktop4Wallpaper;
  bool shuffle = true;
  bool enablePreloading = true;
  bool fallbackDesktopWallpaperAPI = true;
};

static ModSettings g_settings;
static std::vector<std::wstring> g_folderWallpapers;
static std::unordered_map<std::wstring, std::wstring>
    g_desktopWallpaperMap; // GUID String -> Wallpaper Path
static IVirtualDesktopManagerInternal *g_pDesktopManager = nullptr;
static IVirtualDesktopNotificationService *g_pNotificationService = nullptr;
static DWORD g_dwNotificationCookie = 0;
static DWORD g_osBuildNumber = 0;

// Forward function declarations
void ScanWallpaperFolder();
void PreloadAllDesktops();
void ApplyWallpaperForDesktop(IVirtualDesktop *pDesktop);
void SetActiveWallpaper(const std::wstring &path);

// Convert GUID to std::wstring
static std::wstring GuidToString(const GUID &guid) {
  WCHAR szGuid[64] = {0};
  StringFromGUID2(guid, szGuid, 64);
  return std::wstring(szGuid);
}

// Detect Windows OS build
static DWORD DetectWindowsBuildNumber() {
  typedef LONG(NTAPI * RtlGetVersion_t)(PRTL_OSVERSIONINFOW);
  HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
  if (hNtdll) {
    RtlGetVersion_t pRtl =
        (RtlGetVersion_t)GetProcAddress(hNtdll, "RtlGetVersion");
    if (pRtl) {
      RTL_OSVERSIONINFOW osvi = {sizeof(osvi)};
      if (pRtl(&osvi) == 0) {
        return osvi.dwBuildNumber;
      }
    }
  }
  return 22621; // Default to Win 11 22H2
}

// ============================================================================
// Wallpaper Resolution & Application Engine
// ============================================================================

void ScanWallpaperFolder() {
  g_folderWallpapers.clear();
  if (g_settings.wallpaperFolder.empty())
    return;

  std::wstring searchPattern = g_settings.wallpaperFolder;
  if (searchPattern.back() != L'\\' && searchPattern.back() != L'/') {
    searchPattern += L"\\";
  }

  const std::vector<std::wstring> extensions = {L"*.jpg", L"*.jpeg", L"*.png",
                                                L"*.bmp", L"*.webp"};
  for (const auto &ext : extensions) {
    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFileW((searchPattern + ext).c_str(), &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
      do {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
          g_folderWallpapers.push_back(searchPattern + findData.cFileName);
        }
      } while (FindNextFileW(hFind, &findData));
      FindClose(hFind);
    }
  }

  if (g_settings.shuffle && g_folderWallpapers.size() > 1) {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(g_folderWallpapers.begin(), g_folderWallpapers.end(), g);
  }

  Wh_Log(L"Scanned %zu wallpapers from folder: %s", g_folderWallpapers.size(),
         g_settings.wallpaperFolder.c_str());
}

// Determines the designated wallpaper for desktop index (0-based)
std::wstring GetWallpaperForIndex(size_t index) {
  if (index == 0 && !g_settings.desktop1Wallpaper.empty())
    return g_settings.desktop1Wallpaper;
  if (index == 1 && !g_settings.desktop2Wallpaper.empty())
    return g_settings.desktop2Wallpaper;
  if (index == 2 && !g_settings.desktop3Wallpaper.empty())
    return g_settings.desktop3Wallpaper;
  if (index == 3 && !g_settings.desktop4Wallpaper.empty())
    return g_settings.desktop4Wallpaper;

  if (!g_folderWallpapers.empty()) {
    return g_folderWallpapers[index % g_folderWallpapers.size()];
  }
  return L"";
}

// Active wallpaper application using IDesktopWallpaper & SystemParametersInfo
void SetActiveWallpaper(const std::wstring &path) {
  if (path.empty())
    return;

  // 1. Official Windows Shell IDesktopWallpaper COM interface
  IDesktopWallpaper *pWallpaper = nullptr;
  HRESULT hr =
      CoCreateInstance(CLSID_DesktopWallpaper_Local, nullptr,
                       CLSCTX_LOCAL_SERVER | CLSCTX_INPROC_SERVER,
                       IID_IDesktopWallpaper_Local, (void **)&pWallpaper);

  if (SUCCEEDED(hr) && pWallpaper) {
    pWallpaper->SetWallpaper(nullptr, path.c_str());
    pWallpaper->Release();
  } else {
    // 2. Win32 fallback to broadcast desktop refresh
    SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (PVOID)path.c_str(),
                          SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
  }
}

// Preload wallpapers across all virtual desktops into Windows DWM
void PreloadAllDesktops() {
  if (!g_pDesktopManager)
    return;

  IObjectArray *pDesktopsArray = nullptr;
  HRESULT hr = g_pDesktopManager->GetDesktops(&pDesktopsArray);
  if (FAILED(hr) || !pDesktopsArray) {
    Wh_Log(L"Failed to query desktops array: 0x%08X", hr);
    return;
  }

  UINT count = 0;
  pDesktopsArray->GetCount(&count);
  Wh_Log(L"Preloading wallpapers for %u virtual desktops into DWM...", count);

  for (UINT i = 0; i < count; i++) {
    IVirtualDesktop *pVd = nullptr;
    if (SUCCEEDED(pDesktopsArray->GetAt(i, IID_IVirtualDesktop_Win11,
                                        (void **)&pVd)) &&
        pVd) {
      GUID desktopId = {0};
      pVd->GetId(&desktopId);
      std::wstring strGuid = GuidToString(desktopId);

      std::wstring assignedPath = GetWallpaperForIndex(i);
      if (!assignedPath.empty()) {
        g_desktopWallpaperMap[strGuid] = assignedPath;

        // Core Windows 11 API: Assign wallpaper directly to the IVirtualDesktop
        // COM object
        ScopedHString hstrPath(assignedPath.c_str());
        if (hstrPath.Get()) {
          g_pDesktopManager->SetDesktopWallpaper(pVd, hstrPath.Get());
          Wh_Log(L"Desktop [%u] {%s} -> Preloaded '%s'", i + 1, strGuid.c_str(),
                 assignedPath.c_str());
        }
      }
      pVd->Release();
    }
  }

  pDesktopsArray->Release();
}

// React to desktop switch and apply assigned wallpaper
void ApplyWallpaperForDesktop(IVirtualDesktop *pDesktop) {
  if (!pDesktop)
    return;

  GUID desktopId = {0};
  pDesktop->GetId(&desktopId);
  std::wstring strGuid = GuidToString(desktopId);

  std::wstring wallpaperPath;
  auto it = g_desktopWallpaperMap.find(strGuid);
  if (it != g_desktopWallpaperMap.end()) {
    wallpaperPath = it->second;
  } else {
    // Find index if not cached yet
    if (g_pDesktopManager) {
      IObjectArray *pDesktops = nullptr;
      if (SUCCEEDED(g_pDesktopManager->GetDesktops(&pDesktops)) && pDesktops) {
        UINT count = 0;
        pDesktops->GetCount(&count);
        for (UINT i = 0; i < count; i++) {
          IVirtualDesktop *pTest = nullptr;
          if (SUCCEEDED(pDesktops->GetAt(i, IID_IVirtualDesktop_Win11,
                                         (void **)&pTest)) &&
              pTest) {
            GUID testId = {0};
            pTest->GetId(&testId);
            if (IsEqualGUID(testId, desktopId)) {
              wallpaperPath = GetWallpaperForIndex(i);
              g_desktopWallpaperMap[strGuid] = wallpaperPath;
              pTest->Release();
              break;
            }
            pTest->Release();
          }
        }
        pDesktops->Release();
      }
    }
  }

  if (!wallpaperPath.empty()) {
    Wh_Log(L"Desktop switch event -> Active Desktop {%s} applying '%s'",
           strGuid.c_str(), wallpaperPath.c_str());

    // Update via internal manager
    if (g_pDesktopManager) {
      ScopedHString hstr(wallpaperPath.c_str());
      if (hstr.Get()) {
        g_pDesktopManager->SetDesktopWallpaper(pDesktop, hstr.Get());
      }
    }

    // Apply via shell fallback for immediate refresh
    if (g_settings.fallbackDesktopWallpaperAPI) {
      SetActiveWallpaper(wallpaperPath);
    }
  }
}

// ============================================================================
// IVirtualDesktopNotification Implementation
// ============================================================================

class FluxDeskNotificationListener : public IVirtualDesktopNotification {
private:
  LONG m_refCount = 1;

public:
  // IUnknown implementation
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                           void **ppvObject) override {
    if (!ppvObject)
      return E_POINTER;
    if (riid == IID_IUnknown || riid == IID_IVirtualDesktopNotification) {
      *ppvObject = static_cast<IVirtualDesktopNotification *>(this);
      AddRef();
      return S_OK;
    }
    *ppvObject = nullptr;
    return E_NOINTERFACE;
  }

  ULONG STDMETHODCALLTYPE AddRef() override {
    return InterlockedIncrement(&m_refCount);
  }

  ULONG STDMETHODCALLTYPE Release() override {
    ULONG ref = InterlockedDecrement(&m_refCount);
    if (ref == 0)
      delete this;
    return ref;
  }

  // IVirtualDesktopNotification callbacks
  HRESULT STDMETHODCALLTYPE
  VirtualDesktopCreated(IVirtualDesktop *pDesktop) override {
    Wh_Log(L"Notification: Virtual Desktop Created");
    if (g_settings.enablePreloading) {
      PreloadAllDesktops();
    }
    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE
  VirtualDesktopDestroyBegin(IVirtualDesktop *pDesktopDestroyed,
                             IVirtualDesktop *pDesktopFallback) override {
    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE
  VirtualDesktopDestroyFailed(IVirtualDesktop *pDesktopDestroyed,
                              IVirtualDesktop *pDesktopFallback) override {
    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE
  VirtualDesktopDestroyed(IVirtualDesktop *pDesktopDestroyed,
                          IVirtualDesktop *pDesktopFallback) override {
    Wh_Log(L"Notification: Virtual Desktop Destroyed");
    if (g_settings.enablePreloading) {
      PreloadAllDesktops();
    }
    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE
  ViewVirtualDesktopChanged(IUnknown *pView) override {
    return S_OK;
  }

  // Triggered on desktop switch!
  HRESULT STDMETHODCALLTYPE CurrentVirtualDesktopChanged(void *arg1, void *arg2,
                                                         void *arg3) override {
    Wh_Log(L"Notification: CurrentVirtualDesktopChanged fired!");

    IVirtualDesktop *pNewDesktop = nullptr;

    // Windows 11 Build >= 22621 (22H2+): (IObjectArray* pMonitors,
    // IVirtualDesktop* pOld, IVirtualDesktop* pNew)
    if (g_osBuildNumber >= 22621) {
      pNewDesktop = static_cast<IVirtualDesktop *>(arg3);
    } else {
      // Windows 11 21H2 / Windows 10: (IVirtualDesktop* pOld, IVirtualDesktop*
      // pNew)
      pNewDesktop = static_cast<IVirtualDesktop *>(arg2);
    }

    // Safe fallback: Query current desktop directly from manager if pointer was
    // not resolved
    if (!pNewDesktop && g_pDesktopManager) {
      g_pDesktopManager->GetCurrentDesktop(&pNewDesktop);
      if (pNewDesktop) {
        ApplyWallpaperForDesktop(pNewDesktop);
        pNewDesktop->Release();
        return S_OK;
      }
    }

    if (pNewDesktop) {
      ApplyWallpaperForDesktop(pNewDesktop);
    }

    return S_OK;
  }
};

static FluxDeskNotificationListener *g_pNotificationListener = nullptr;

// ============================================================================
// Direct In-Process Hook for IVirtualDesktopManagerInternal::SwitchDesktop
// ============================================================================

typedef HRESULT(STDMETHODCALLTYPE *SwitchDesktopWithAnimation_t)(
    IVirtualDesktopManagerInternal *pThis, IVirtualDesktop *pDesktop);

static SwitchDesktopWithAnimation_t g_pfnOriginalSwitchDesktopWithAnimation =
    nullptr;

HRESULT STDMETHODCALLTYPE Hook_SwitchDesktopWithAnimation(
    IVirtualDesktopManagerInternal *pThis, IVirtualDesktop *pDesktop) {
  Wh_Log(L"Hook: SwitchDesktopWithAnimation intercepted!");
  if (pDesktop) {
    ApplyWallpaperForDesktop(pDesktop);
  }
  return g_pfnOriginalSwitchDesktopWithAnimation(pThis, pDesktop);
}

// ============================================================================
// Windhawk Settings Loader
// ============================================================================

static void LoadSettings() {
  PCWSTR strFolder = Wh_GetStringSetting(L"wallpaperFolder");
  g_settings.wallpaperFolder = strFolder ? strFolder : L"";
  Wh_FreeStringSetting(strFolder);

  PCWSTR strWp1 = Wh_GetStringSetting(L"desktop1Wallpaper");
  g_settings.desktop1Wallpaper = strWp1 ? strWp1 : L"";
  Wh_FreeStringSetting(strWp1);

  PCWSTR strWp2 = Wh_GetStringSetting(L"desktop2Wallpaper");
  g_settings.desktop2Wallpaper = strWp2 ? strWp2 : L"";
  Wh_FreeStringSetting(strWp2);

  PCWSTR strWp3 = Wh_GetStringSetting(L"desktop3Wallpaper");
  g_settings.desktop3Wallpaper = strWp3 ? strWp3 : L"";
  Wh_FreeStringSetting(strWp3);

  PCWSTR strWp4 = Wh_GetStringSetting(L"desktop4Wallpaper");
  g_settings.desktop4Wallpaper = strWp4 ? strWp4 : L"";
  Wh_FreeStringSetting(strWp4);

  g_settings.shuffle = Wh_GetIntSetting(L"shuffle");
  g_settings.enablePreloading = Wh_GetIntSetting(L"enablePreloading");
  g_settings.fallbackDesktopWallpaperAPI =
      Wh_GetIntSetting(L"fallbackDesktopWallpaperAPI");

  Wh_Log(L"Settings loaded. Folder: '%s', Preload: %d",
         g_settings.wallpaperFolder.c_str(), g_settings.enablePreloading);

  ScanWallpaperFolder();
  if (g_settings.enablePreloading && g_pDesktopManager) {
    PreloadAllDesktops();
  }
}

// ============================================================================
// Windhawk Mod Lifecycle Entry Points
// ============================================================================

BOOL Wh_ModInit() {
  Wh_Log(L"Initializing Virtual Desktop Wallpaper Switch (FluxDesk)...");

  g_osBuildNumber = DetectWindowsBuildNumber();
  Wh_Log(L"Detected Windows Build: %u", g_osBuildNumber);

  InitWinRTStringFunctions();

  // Explorer hosts the Immersive Shell Service Provider in-process
  IServiceProvider *pServiceProvider = nullptr;
  HRESULT hr = CoCreateInstance(
      CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER | CLSCTX_INPROC_SERVER,
      IID_IServiceProvider, (void **)&pServiceProvider);

  if (FAILED(hr) || !pServiceProvider) {
    Wh_Log(L"Failed to obtain ImmersiveShell IServiceProvider: 0x%08X", hr);
    return FALSE;
  }

  // 1. Query IVirtualDesktopManagerInternal
  hr = pServiceProvider->QueryService(CLSID_VirtualDesktopManagerInternal,
                                      IID_IVirtualDesktopManagerInternal_Win11,
                                      (void **)&g_pDesktopManager);

  if (FAILED(hr) || !g_pDesktopManager) {
    // Fallback for Windows 11 21H2 GUID
    pServiceProvider->QueryService(
        CLSID_VirtualDesktopManagerInternal,
        IID_IVirtualDesktopManagerInternal_Win11_21H2,
        (void **)&g_pDesktopManager);
  }

  if (g_pDesktopManager) {
    Wh_Log(L"Successfully connected to IVirtualDesktopManagerInternal!");

    // Optional: Install vtable hook on SwitchDesktopWithAnimation
    void **vtable = *(void ***)g_pDesktopManager;
    // Index 19 is SwitchDesktopWithAnimation in Windows 11 22H2/23H2 vtable
    if (vtable && vtable[19]) {
      Wh_SetFunctionHook(vtable[19], (void *)Hook_SwitchDesktopWithAnimation,
                         (void **)&g_pfnOriginalSwitchDesktopWithAnimation);
      Wh_Log(L"Hooked SwitchDesktopWithAnimation at vtable[19]");
    }
  } else {
    Wh_Log(L"Warning: Could not bind IVirtualDesktopManagerInternal");
  }

  // 2. Query IVirtualDesktopNotificationService and register listener
  hr = pServiceProvider->QueryService(CLSID_VirtualDesktopNotificationService,
                                      IID_IVirtualDesktopNotificationService,
                                      (void **)&g_pNotificationService);

  if (SUCCEEDED(hr) && g_pNotificationService) {
    g_pNotificationListener = new FluxDeskNotificationListener();
    hr = g_pNotificationService->Register(g_pNotificationListener,
                                          &g_dwNotificationCookie);
    if (SUCCEEDED(hr)) {
      Wh_Log(L"Registered IVirtualDesktopNotification listener (Cookie: %u)",
             g_dwNotificationCookie);
    } else {
      Wh_Log(L"Failed to register IVirtualDesktopNotification: 0x%08X", hr);
    }
  } else {
    Wh_Log(L"Failed to query IVirtualDesktopNotificationService: 0x%08X", hr);
  }

  pServiceProvider->Release();

  // 3. Load settings and perform initial preloading
  LoadSettings();

  Wh_Log(
      L"Virtual Desktop Wallpaper Switch (FluxDesk) successfully initialized!");
  return TRUE;
}

void Wh_ModAfterInit() {
  Wh_Log(L"Wh_ModAfterInit: Mod loaded and active in explorer.exe");
}

BOOL Wh_ModSettingsChanged(BOOL *bReload) {
  Wh_Log(L"Mod settings changed callback received.");
  LoadSettings();
  if (bReload) {
    *bReload = FALSE;
  }
  return TRUE;
}

void Wh_ModUninit() {
  Wh_Log(L"Unloading Virtual Desktop Wallpaper Switch (FluxDesk)...");

  // Unregister notification listener
  if (g_pNotificationService && g_dwNotificationCookie != 0) {
    g_pNotificationService->Unregister(g_dwNotificationCookie);
    g_dwNotificationCookie = 0;
    Wh_Log(L"Unregistered IVirtualDesktopNotification listener.");
  }

  if (g_pNotificationListener) {
    g_pNotificationListener->Release();
    g_pNotificationListener = nullptr;
  }

  if (g_pNotificationService) {
    g_pNotificationService->Release();
    g_pNotificationService = nullptr;
  }

  if (g_pDesktopManager) {
    g_pDesktopManager->Release();
    g_pDesktopManager = nullptr;
  }

  Wh_Log(L"Virtual Desktop Wallpaper Switch unloaded cleanly.");
}
