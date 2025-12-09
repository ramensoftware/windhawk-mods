// ==WindhawkMod==
// @id              virtual-desktop-helper
// @name            Virtual Desktop Helper
// @description     Go to a specific virtual desktop and move the active window to a specific virtual desktop
// @version         1.0.0
// @author          u2x1
// @github          https://github.com/u2x1
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Virtual Desktop Move Window

This mod allows you to go to a specific virtual desktop and move the active window to a specific virtual desktop using hotkeys.

Based on VD.ahk by FuPeiJiang.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- WindowsVersion: win11_26100
  $name: Windows Version
  $description: Select your Windows version for correct COM interface IDs
  $options:
    - win10_old: Windows 10 (Build < 20348)
    - win10_20348: Windows 10 (Build 20348 - 21999)
    - win11_22000: Windows 11 (Build 22000 - 22482)
    - win11_22621: Windows 11 (Build 22621/22631/23H2)
    - win11_26100: Windows 11 (Build 26100+ / 24H2)

- MoveWindowModifier: alt+shift
  $name: Move Window Hotkey Modifier
  $description: Modifier keys for moving active window to desktop (combined with 1-9)
  $options:
    - alt+shift: Alt + Shift
    - ctrl+alt: Ctrl + Alt
    - ctrl+shift: Ctrl + Shift

- SwitchDesktopModifier: alt
  $name: Switch Desktop Hotkey Modifier
  $description: Modifier keys for switching to desktop (combined with 1-9)
  $options:
    - alt: Alt
    - ctrl: Ctrl
    - alt+shift: Alt + Shift
    - ctrl+alt: Ctrl + Alt
    - ctrl+shift: Ctrl + Shift

- FollowMovedWindow: false
  $name: Follow Moved Window
  $description: Switch to the target desktop after moving a window

- MaxDesktops: 9
  $name: Maximum Desktop Number
  $description: Maximum number of desktops to register hotkeys for (1-9)
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <objbase.h>
#include <objectarray.h>
#include <shobjidl.h>
#include <winstring.h>

//=============================================================================
// Interface IDs and CLSIDs
// These vary by Windows build. Uncomment the appropriate section for your OS.
//=============================================================================

// CLSID for ImmersiveShell (same across all versions)
static const CLSID CLSID_ImmersiveShell = {
    0xC2F03A33, 0x21F5, 0x47FA, {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39}
};

// CLSID for VirtualDesktopManagerInternal service (same across all versions)
static const CLSID CLSID_VirtualDesktopManagerInternal = {
    0xC5E0CDCA, 0x7B6E, 0x41B2, {0x9F, 0xC4, 0xD9, 0x39, 0x75, 0xCC, 0x46, 0x7B}
};

// IID for IApplicationViewCollection
static const IID IID_IApplicationViewCollection = {
    0x1841C6D7, 0x4F9D, 0x42C0, {0xAF, 0x41, 0x87, 0x47, 0x53, 0x8F, 0x10, 0xE5}
};

// CLSID for VirtualDesktopPinnedApps service
static const CLSID CLSID_VirtualDesktopPinnedApps = {
    0xB5A399E7, 0x1C87, 0x46B8, {0x88, 0xE9, 0xFC, 0x57, 0x47, 0xB1, 0x71, 0xBD}
};

// CLSID for VirtualDesktopNotification service and IID for notification service interface
static const CLSID CLSID_VirtualDesktopNotificationService = {
    0xA501FDEC, 0x4A09, 0x464C, {0xAE, 0x4E, 0x1B, 0x9C, 0x21, 0xB8, 0x49, 0x18}
};
static const IID IID_IVirtualDesktopNotificationService = {
    0x0CD45E71, 0xD927, 0x4F15, {0x8B, 0x0A, 0x8F, 0xEF, 0x52, 0x53, 0x37, 0xBF}
};

// IID for the IVirtualDesktopNotification sink (value varies between Windows builds)
// static const IID IID_IVirtualDesktopNotification = { ... }  // Refer to VD.ahk for build-specific options

//=============================================================================
// Version-specific IIDs for IVirtualDesktopManagerInternal and IVirtualDesktop
// Selected at runtime based on settings
//=============================================================================

// Windows 10 Build < 20348 (older Win10)
static const IID IID_IVirtualDesktopManagerInternal_Win10Old = {
    0xF31574D6, 0xB682, 0x4CDC, {0xBD, 0x56, 0x18, 0x27, 0x86, 0x0A, 0xBE, 0xC6}
};
static const IID IID_IVirtualDesktop_Win10Old = {
    0xFF72FFDD, 0xBE7E, 0x43FC, {0x9C, 0x03, 0xAD, 0x81, 0x68, 0x1E, 0x88, 0xE4}
};

// Windows 10 Build 20348 - 21999
static const IID IID_IVirtualDesktopManagerInternal_Win10_20348 = {
    0x094AFE11, 0x44F2, 0x4BA0, {0x97, 0x6F, 0x29, 0xA9, 0x7E, 0x26, 0x3E, 0xE0}
};
static const IID IID_IVirtualDesktop_Win10_20348 = {
    0x62FDF88B, 0x11CA, 0x4AFB, {0x8B, 0xD8, 0x22, 0x96, 0xDF, 0xAE, 0x49, 0xE2}
};

// Windows 11 Build 22000 - 22482
static const IID IID_IVirtualDesktopManagerInternal_Win11_22000 = {
    0xB2F925B9, 0x5A0F, 0x4D2E, {0x9F, 0x4D, 0x2B, 0x15, 0x07, 0x59, 0x3C, 0x10}
};
static const IID IID_IVirtualDesktop_Win11_22000 = {
    0x536D3495, 0xB208, 0x4CC9, {0xAE, 0x26, 0xDE, 0x81, 0x11, 0x27, 0x5B, 0xF8}
};

// Windows 11 Build 22621.2215+ / 22631.3085+ / 23H2+
static const IID IID_IVirtualDesktopManagerInternal_Win11_22621 = {
    0xA3175F2D, 0x239C, 0x4BD2, {0x8A, 0xA0, 0xEE, 0xBA, 0x8B, 0x0B, 0x13, 0x8E}
};
static const IID IID_IVirtualDesktop_Win11_22621 = {
    0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}
};

// Windows 11 Build 26100+ (24H2)
static const IID IID_IVirtualDesktopManagerInternal_Win11_26100 = {
    0x53F5CA0B, 0x158F, 0x4124, {0x90, 0x0C, 0x05, 0x71, 0x58, 0x06, 0x0B, 0x27}
};
static const IID IID_IVirtualDesktop_Win11_26100 = {
    0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}
};

// VTable indices (same for most versions)
#define VT_IDX_MoveViewToDesktop  4
#define VT_IDX_GetCurrentDesktop  6
#define VT_IDX_GetDesktops        7
#define VT_IDX_SwitchDesktop      9

// Windows version enum
enum class WindowsVersion {
    Win10Old,       // < 20348, no HMONITOR
    Win10_20348,    // 20348-21999, uses HMONITOR
    Win11_22000,    // 22000-22482, uses HMONITOR
    Win11_22621,    // 22621/22631/23H2, no HMONITOR
    Win11_26100     // 26100+ (24H2), no HMONITOR
};

// Runtime-selected IIDs and settings
static WindowsVersion g_windowsVersion = WindowsVersion::Win11_26100;
static const IID* g_pIID_IVirtualDesktopManagerInternal = &IID_IVirtualDesktopManagerInternal_Win11_26100;
static const IID* g_pIID_IVirtualDesktop = &IID_IVirtualDesktop_Win11_26100;

//=============================================================================
// Interface Definitions (VTable-based)
// Using custom names to avoid conflicts with system headers
//=============================================================================

// IVirtualDesktop interface (internal, undocumented)
// VTable order: QueryInterface, AddRef, Release, IsViewVisible, GetId, ...
MIDL_INTERFACE("3F07F4BE-B107-441A-AF0F-39D82529072C")
IVirtualDesktopInternal : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE IsViewVisible(IUnknown* pView, BOOL* pfVisible) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetId(GUID* pGuid) = 0;
    // Additional methods vary by version
};

// IApplicationView interface (simplified - we only need a few methods)
// The actual interface has many more methods, but we use it as IUnknown and call via vtable
MIDL_INTERFACE("372E1D3B-38D3-42E4-A15B-8AB2B178F513")
IApplicationViewVD : public IUnknown {
public:
    // Methods 0-2: IUnknown (QueryInterface, AddRef, Release)
    // Method 3+: IApplicationView specific methods
    // We don't need to define all - we just pass this as a pointer
};

// IApplicationViewCollection interface
MIDL_INTERFACE("1841C6D7-4F9D-42C0-AF41-8747538F10E5")
IApplicationViewCollectionVD : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetViews(IObjectArray** ppViews) = 0;                        // 3
    virtual HRESULT STDMETHODCALLTYPE GetViewsByZOrder(IObjectArray** ppViews) = 0;               // 4
    virtual HRESULT STDMETHODCALLTYPE GetViewsByAppUserModelId(LPCWSTR id, IObjectArray** ppViews) = 0; // 5
    virtual HRESULT STDMETHODCALLTYPE GetViewForHwnd(HWND hwnd, IApplicationViewVD** ppView) = 0;   // 6
    // More methods follow but we don't need them
};

// IVirtualDesktopNotification interface (subset used by VD.ahk)
MIDL_INTERFACE("C179334C-4295-40D3-BEA1-C654D965605A")
IVirtualDesktopNotificationVD : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopCreated(IVirtualDesktopInternal* pDesktop) = 0; // 3
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyBegin(IVirtualDesktopInternal* pDesktopDestroying, IVirtualDesktopInternal* pDesktopFallback) = 0; // 4
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyFailed(IVirtualDesktopInternal* pDesktopDestroying, IVirtualDesktopInternal* pDesktopFallback) = 0; // 5
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyed(IVirtualDesktopInternal* pDesktopDestroyed, IVirtualDesktopInternal* pDesktopFallback) = 0; // 6
    virtual HRESULT STDMETHODCALLTYPE ViewVirtualDesktopChanged(IApplicationViewVD* pView) = 0; // 7
    virtual HRESULT STDMETHODCALLTYPE CurrentVirtualDesktopChanged(IVirtualDesktopInternal* pOldDesktop, IVirtualDesktopInternal* pNewDesktop) = 0; // 8
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopNameChanged(IVirtualDesktopInternal* pDesktop, HSTRING desktopName) = 0; // 9
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopWallpaperChanged(IVirtualDesktopInternal* pDesktop, HSTRING wallpaperPath) = 0; // 10
};

// Notification service interface
MIDL_INTERFACE("0CD45E71-D927-4F15-8B0A-8FEF525337BF")
IVirtualDesktopNotificationServiceVD : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE Register(IVirtualDesktopNotificationVD* pNotification, DWORD* pdwCookie) = 0; // 3
    virtual HRESULT STDMETHODCALLTYPE Unregister(DWORD dwCookie) = 0; // 4
};

// VirtualDesktopPinnedApps interface
MIDL_INTERFACE("4CE81583-1E4C-4632-A621-07A53543148F")
IVirtualDesktopPinnedAppsVD : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE IsAppIdPinned(LPCWSTR appId, BOOL* pbPinned) = 0; // 3
    virtual HRESULT STDMETHODCALLTYPE PinAppID(LPCWSTR appId) = 0; // 4
    virtual HRESULT STDMETHODCALLTYPE UnpinAppID(LPCWSTR appId) = 0; // 5
    virtual HRESULT STDMETHODCALLTYPE IsViewPinned(IApplicationViewVD* pView, BOOL* pbPinned) = 0; // 6
    virtual HRESULT STDMETHODCALLTYPE PinView(IApplicationViewVD* pView) = 0; // 7
    virtual HRESULT STDMETHODCALLTYPE UnpinView(IApplicationViewVD* pView) = 0; // 8
};

//=============================================================================
// IVirtualDesktopManagerInternal
// This interface varies significantly between Windows versions.
// The VTable indices are defined above based on version.
//=============================================================================

// We define a minimal interface and use direct vtable calls for version-specific methods
MIDL_INTERFACE("53F5CA0B-158F-4124-900C-057158060B27")
IVirtualDesktopManagerInternalVD : public IUnknown {
public:
    // We'll use vtable calls directly since the interface varies
};

//=============================================================================
// Global Variables
//=============================================================================

static IServiceProvider* g_pServiceProvider = nullptr;
static IVirtualDesktopManagerInternalVD* g_pDesktopManagerInternal = nullptr;
static IApplicationViewCollectionVD* g_pApplicationViewCollection = nullptr;
static IVirtualDesktopManager* g_pDesktopManager = nullptr;
static bool g_bInitialized = false;
static HANDLE g_hHotkeyThread = nullptr;
static DWORD g_hotkeyThreadId = 0;
static HANDLE g_hHotkeyReadyEvent = nullptr;

static const int MAX_DESKTOP_COUNT = 9;
static const UINT HOTKEY_ID_MOVE_ACTIVE_BASE = 1;
static const UINT HOTKEY_ID_GO_TO_BASE = HOTKEY_ID_MOVE_ACTIVE_BASE + MAX_DESKTOP_COUNT;
static bool g_moveHotkeysRegistered[MAX_DESKTOP_COUNT] = {};
static bool g_goHotkeysRegistered[MAX_DESKTOP_COUNT] = {};

// Settings
static UINT g_moveModifiers = MOD_ALT | MOD_SHIFT;
static UINT g_switchModifiers = MOD_ALT;
static bool g_followMovedWindow = false;
static int g_maxDesktops = 9;

bool MoveWindowToDesktopNum(HWND hwnd, int desktopNum);
bool MoveActiveWindowToDesktopNum(int desktopNum);
bool GoToDesktopNum(int desktopNum);

//=============================================================================
// Settings Helper Functions
//=============================================================================

// Check if current Windows version uses HMONITOR parameter
bool UseHMonitorParam() {
    return g_windowsVersion == WindowsVersion::Win10_20348 ||
           g_windowsVersion == WindowsVersion::Win11_22000;
}

// Parse modifier string to Windows modifier flags
UINT ParseModifiers(PCWSTR modifierStr) {
    UINT mods = 0;
    if (wcsstr(modifierStr, L"alt")) mods |= MOD_ALT;
    if (wcsstr(modifierStr, L"ctrl")) mods |= MOD_CONTROL;
    if (wcsstr(modifierStr, L"shift")) mods |= MOD_SHIFT;
    return mods;
}

// Load settings from Windhawk
void LoadSettings() {
    // Load Windows version
    PCWSTR versionStr = Wh_GetStringSetting(L"WindowsVersion");
    if (wcscmp(versionStr, L"win10_old") == 0) {
        g_windowsVersion = WindowsVersion::Win10Old;
        g_pIID_IVirtualDesktopManagerInternal = &IID_IVirtualDesktopManagerInternal_Win10Old;
        g_pIID_IVirtualDesktop = &IID_IVirtualDesktop_Win10Old;
    } else if (wcscmp(versionStr, L"win10_20348") == 0) {
        g_windowsVersion = WindowsVersion::Win10_20348;
        g_pIID_IVirtualDesktopManagerInternal = &IID_IVirtualDesktopManagerInternal_Win10_20348;
        g_pIID_IVirtualDesktop = &IID_IVirtualDesktop_Win10_20348;
    } else if (wcscmp(versionStr, L"win11_22000") == 0) {
        g_windowsVersion = WindowsVersion::Win11_22000;
        g_pIID_IVirtualDesktopManagerInternal = &IID_IVirtualDesktopManagerInternal_Win11_22000;
        g_pIID_IVirtualDesktop = &IID_IVirtualDesktop_Win11_22000;
    } else if (wcscmp(versionStr, L"win11_22621") == 0) {
        g_windowsVersion = WindowsVersion::Win11_22621;
        g_pIID_IVirtualDesktopManagerInternal = &IID_IVirtualDesktopManagerInternal_Win11_22621;
        g_pIID_IVirtualDesktop = &IID_IVirtualDesktop_Win11_22621;
    } else {
        g_windowsVersion = WindowsVersion::Win11_26100;
        g_pIID_IVirtualDesktopManagerInternal = &IID_IVirtualDesktopManagerInternal_Win11_26100;
        g_pIID_IVirtualDesktop = &IID_IVirtualDesktop_Win11_26100;
    }
    Wh_FreeStringSetting(versionStr);

    // Load move window modifier
    PCWSTR moveModStr = Wh_GetStringSetting(L"MoveWindowModifier");
    g_moveModifiers = ParseModifiers(moveModStr);
    if (g_moveModifiers == 0) g_moveModifiers = MOD_ALT | MOD_SHIFT;
    Wh_FreeStringSetting(moveModStr);

    // Load switch desktop modifier
    PCWSTR switchModStr = Wh_GetStringSetting(L"SwitchDesktopModifier");
    g_switchModifiers = ParseModifiers(switchModStr);
    if (g_switchModifiers == 0) g_switchModifiers = MOD_ALT;
    Wh_FreeStringSetting(switchModStr);

    // Load follow moved window setting
    g_followMovedWindow = Wh_GetIntSetting(L"FollowMovedWindow") != 0;

    // Load max desktops
    g_maxDesktops = Wh_GetIntSetting(L"MaxDesktops");
    if (g_maxDesktops < 1) g_maxDesktops = 1;
    if (g_maxDesktops > MAX_DESKTOP_COUNT) g_maxDesktops = MAX_DESKTOP_COUNT;

    Wh_Log(L"Settings loaded: version=%d, moveModifiers=0x%X, switchModifiers=0x%X, follow=%d, maxDesktops=%d",
           (int)g_windowsVersion, g_moveModifiers, g_switchModifiers, g_followMovedWindow, g_maxDesktops);
}

//=============================================================================
// Helper Functions
//=============================================================================

// Get vtable function pointer
template<typename T>
T GetVTableFunction(void* pInterface, int index) {
    void** vtable = *reinterpret_cast<void***>(pInterface);
    return reinterpret_cast<T>(vtable[index]);
}

// Initialize COM interfaces
bool InitializeVirtualDesktopAPI() {
    if (g_bInitialized) {
        return true;
    }

    HRESULT hr;

    // Create ImmersiveShell and get IServiceProvider
    hr = CoCreateInstance(
        CLSID_ImmersiveShell,
        nullptr,
        CLSCTX_LOCAL_SERVER,
        IID_IServiceProvider,
        reinterpret_cast<void**>(&g_pServiceProvider)
    );
    if (FAILED(hr) || !g_pServiceProvider) {
        Wh_Log(L"Failed to create ImmersiveShell: 0x%08X", hr);
        return false;
    }

    // Get IVirtualDesktopManagerInternal
    hr = g_pServiceProvider->QueryService(
        CLSID_VirtualDesktopManagerInternal,
        *g_pIID_IVirtualDesktopManagerInternal,
        reinterpret_cast<void**>(&g_pDesktopManagerInternal)
    );
    if (FAILED(hr) || !g_pDesktopManagerInternal) {
        Wh_Log(L"Failed to get IVirtualDesktopManagerInternal: 0x%08X", hr);
        return false;
    }

    // Get IApplicationViewCollection
    hr = g_pServiceProvider->QueryService(
        IID_IApplicationViewCollection,
        IID_IApplicationViewCollection,
        reinterpret_cast<void**>(&g_pApplicationViewCollection)
    );
    if (FAILED(hr) || !g_pApplicationViewCollection) {
        Wh_Log(L"Failed to get IApplicationViewCollection: 0x%08X", hr);
        return false;
    }

    // Get IVirtualDesktopManager (public API)
    hr = CoCreateInstance(
        CLSID_VirtualDesktopManager,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_IVirtualDesktopManager,
        reinterpret_cast<void**>(&g_pDesktopManager)
    );
    if (FAILED(hr) || !g_pDesktopManager) {
        Wh_Log(L"Failed to create IVirtualDesktopManager: 0x%08X", hr);
        return false;
    }

    g_bInitialized = true;
    Wh_Log(L"Virtual Desktop API initialized successfully");
    return true;
}

// Cleanup COM interfaces
void CleanupVirtualDesktopAPI() {
    if (g_pDesktopManager) {
        g_pDesktopManager->Release();
        g_pDesktopManager = nullptr;
    }
    if (g_pApplicationViewCollection) {
        g_pApplicationViewCollection->Release();
        g_pApplicationViewCollection = nullptr;
    }
    if (g_pDesktopManagerInternal) {
        g_pDesktopManagerInternal->Release();
        g_pDesktopManagerInternal = nullptr;
    }
    if (g_pServiceProvider) {
        g_pServiceProvider->Release();
        g_pServiceProvider = nullptr;
    }
    g_bInitialized = false;
}

DWORD WINAPI HotkeyThreadProc(LPVOID) {
    g_hotkeyThreadId = GetCurrentThreadId();

    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        Wh_Log(L"CoInitializeEx failed in hotkey thread: 0x%08X", hr);
        if (g_hHotkeyReadyEvent) {
            SetEvent(g_hHotkeyReadyEvent);
        }
        return 1;
    }

    if (!InitializeVirtualDesktopAPI()) {
        Wh_Log(L"Failed to initialize Virtual Desktop API in hotkey thread");
        CoUninitialize();
        if (g_hHotkeyReadyEvent) {
            SetEvent(g_hHotkeyReadyEvent);
        }
        return 1;
    }

    MSG msg;
    PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);
    if (g_hHotkeyReadyEvent) {
        SetEvent(g_hHotkeyReadyEvent);
    }

    for (int desktopNum = 1; desktopNum <= g_maxDesktops; ++desktopNum) {
        UINT hotkeyId = HOTKEY_ID_MOVE_ACTIVE_BASE + static_cast<UINT>(desktopNum - 1);
        UINT vkKey = static_cast<UINT>('0' + desktopNum);
        bool registered = RegisterHotKey(nullptr, hotkeyId, g_moveModifiers, vkKey);
        g_moveHotkeysRegistered[desktopNum - 1] = registered;
        if (!registered) {
            Wh_Log(L"Failed to register move hotkey %d: %d", desktopNum, GetLastError());
        }
    }

    for (int desktopNum = 1; desktopNum <= g_maxDesktops; ++desktopNum) {
        UINT hotkeyId = HOTKEY_ID_GO_TO_BASE + static_cast<UINT>(desktopNum - 1);
        UINT vkKey = static_cast<UINT>('0' + desktopNum);
        bool registered = RegisterHotKey(nullptr, hotkeyId, g_switchModifiers, vkKey);
        g_goHotkeysRegistered[desktopNum - 1] = registered;
        if (!registered) {
            Wh_Log(L"Failed to register switch hotkey %d: %d", desktopNum, GetLastError());
        }
    }

    while (true) {
        BOOL getMessageResult = GetMessage(&msg, nullptr, 0, 0);
        if (getMessageResult == 0 || getMessageResult == -1) {
            break;
        }

        if (msg.message == WM_HOTKEY) {
            UINT hotkeyId = static_cast<UINT>(msg.wParam);
            if (hotkeyId >= HOTKEY_ID_MOVE_ACTIVE_BASE && hotkeyId < HOTKEY_ID_MOVE_ACTIVE_BASE + MAX_DESKTOP_COUNT) {
                int desktopNum = static_cast<int>(hotkeyId - HOTKEY_ID_MOVE_ACTIVE_BASE) + 1;
                if (MoveActiveWindowToDesktopNum(desktopNum) && g_followMovedWindow) {
                    GoToDesktopNum(desktopNum);
                }
                continue;
            }
            if (hotkeyId >= HOTKEY_ID_GO_TO_BASE && hotkeyId < HOTKEY_ID_GO_TO_BASE + MAX_DESKTOP_COUNT) {
                int desktopNum = static_cast<int>(hotkeyId - HOTKEY_ID_GO_TO_BASE) + 1;
                GoToDesktopNum(desktopNum);
                continue;
            }
        }
    }

    for (int desktopNum = 1; desktopNum <= MAX_DESKTOP_COUNT; ++desktopNum) {
        if (g_moveHotkeysRegistered[desktopNum - 1]) {
            UnregisterHotKey(nullptr, HOTKEY_ID_MOVE_ACTIVE_BASE + static_cast<UINT>(desktopNum - 1));
            g_moveHotkeysRegistered[desktopNum - 1] = false;
        }
        if (g_goHotkeysRegistered[desktopNum - 1]) {
            UnregisterHotKey(nullptr, HOTKEY_ID_GO_TO_BASE + static_cast<UINT>(desktopNum - 1));
            g_goHotkeysRegistered[desktopNum - 1] = false;
        }
    }

    CleanupVirtualDesktopAPI();
    CoUninitialize();

    return 0;
}

bool StartHotkeyThread() {
    if (g_hHotkeyThread) {
        return true;
    }

    g_hHotkeyReadyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_hHotkeyReadyEvent) {
        Wh_Log(L"Failed to create hotkey readiness event: %d", GetLastError());
        return false;
    }

    DWORD threadId = 0;
    g_hHotkeyThread = CreateThread(nullptr, 0, HotkeyThreadProc, nullptr, 0, &threadId);
    if (!g_hHotkeyThread) {
        Wh_Log(L"Failed to create hotkey thread: %d", GetLastError());
        CloseHandle(g_hHotkeyReadyEvent);
        g_hHotkeyReadyEvent = nullptr;
        return false;
    }

    DWORD waitResult = WaitForSingleObject(g_hHotkeyReadyEvent, 5000);
    CloseHandle(g_hHotkeyReadyEvent);
    g_hHotkeyReadyEvent = nullptr;

    if (waitResult != WAIT_OBJECT_0) {
        Wh_Log(L"Hotkey thread failed to initialize in time");
        PostThreadMessage(threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hHotkeyThread, INFINITE);
        CloseHandle(g_hHotkeyThread);
        g_hHotkeyThread = nullptr;
        g_hotkeyThreadId = 0;
        return false;
    }

    return true;
}

void StopHotkeyThread() {
    if (g_hotkeyThreadId != 0) {
        PostThreadMessage(g_hotkeyThreadId, WM_QUIT, 0, 0);
    }

    if (g_hHotkeyThread) {
        WaitForSingleObject(g_hHotkeyThread, INFINITE);
        CloseHandle(g_hHotkeyThread);
        g_hHotkeyThread = nullptr;
    }

    g_hotkeyThreadId = 0;
}

// Get IObjectArray of desktops
IObjectArray* GetDesktops() {
    if (!g_pDesktopManagerInternal) return nullptr;

    IObjectArray* pDesktops = nullptr;
    HRESULT hr;

    // VTable index for GetDesktops
    // Signature varies by Windows version:
    // - Normal: HRESULT GetDesktops(IObjectArray** ppDesktops)
    // - HMONITOR: HRESULT GetDesktops(HMONITOR hMonitor, IObjectArray** ppDesktops)

    if (UseHMonitorParam()) {
        typedef HRESULT(STDMETHODCALLTYPE* GetDesktopsFunc)(void*, HMONITOR, IObjectArray**);
        auto pfnGetDesktops = GetVTableFunction<GetDesktopsFunc>(g_pDesktopManagerInternal, VT_IDX_GetDesktops);
        hr = pfnGetDesktops(g_pDesktopManagerInternal, nullptr, &pDesktops);
    } else {
        typedef HRESULT(STDMETHODCALLTYPE* GetDesktopsFunc)(void*, IObjectArray**);
        auto pfnGetDesktops = GetVTableFunction<GetDesktopsFunc>(g_pDesktopManagerInternal, VT_IDX_GetDesktops);
        hr = pfnGetDesktops(g_pDesktopManagerInternal, &pDesktops);
    }

    if (FAILED(hr)) {
        Wh_Log(L"GetDesktops failed: 0x%08X", hr);
        return nullptr;
    }

    return pDesktops;
}

// Get desktop by index (0-based)
IVirtualDesktopInternal* GetDesktopByIndex(int index) {
    IObjectArray* pDesktops = GetDesktops();
    if (!pDesktops) return nullptr;

    UINT count = 0;
    pDesktops->GetCount(&count);

    if (index < 0 || (UINT)index >= count) {
        pDesktops->Release();
        return nullptr;
    }

    IVirtualDesktopInternal* pDesktop = nullptr;
    HRESULT hr = pDesktops->GetAt(index, *g_pIID_IVirtualDesktop, reinterpret_cast<void**>(&pDesktop));
    pDesktops->Release();

    if (FAILED(hr)) {
        Wh_Log(L"GetAt failed: 0x%08X", hr);
        return nullptr;
    }

    return pDesktop;
}

// Get IApplicationView for a window
IApplicationViewVD* GetApplicationViewForHwnd(HWND hwnd) {
    if (!g_pApplicationViewCollection) return nullptr;

    IApplicationViewVD* pView = nullptr;
    HRESULT hr = g_pApplicationViewCollection->GetViewForHwnd(hwnd, &pView);

    if (FAILED(hr)) {
        Wh_Log(L"GetViewForHwnd failed: 0x%08X", hr);
        return nullptr;
    }

    return pView;
}

// Move view to desktop using IVirtualDesktopManagerInternal::MoveViewToDesktop
bool MoveViewToDesktop(IApplicationViewVD* pView, IVirtualDesktopInternal* pDesktop) {
    if (!g_pDesktopManagerInternal || !pView || !pDesktop) return false;

    // VTable index 4: MoveViewToDesktop
    // Signature: HRESULT MoveViewToDesktop(IApplicationView* pView, IVirtualDesktop* pDesktop)
    typedef HRESULT(STDMETHODCALLTYPE* MoveViewToDesktopFunc)(void*, IApplicationViewVD*, IVirtualDesktopInternal*);
    auto pfnMoveViewToDesktop = GetVTableFunction<MoveViewToDesktopFunc>(g_pDesktopManagerInternal, VT_IDX_MoveViewToDesktop);

    HRESULT hr = pfnMoveViewToDesktop(g_pDesktopManagerInternal, pView, pDesktop);
    if (FAILED(hr)) {
        Wh_Log(L"MoveViewToDesktop failed: 0x%08X", hr);
        return false;
    }

    return true;
}

// Switch to desktop using IVirtualDesktopManagerInternal::SwitchDesktop
bool SwitchToDesktop(IVirtualDesktopInternal* pDesktop) {
    if (!g_pDesktopManagerInternal || !pDesktop) return false;

    HRESULT hr;
    if (UseHMonitorParam()) {
        typedef HRESULT(STDMETHODCALLTYPE* SwitchDesktopFunc)(void*, HMONITOR, IVirtualDesktopInternal*);
        auto pfnSwitchDesktop = GetVTableFunction<SwitchDesktopFunc>(g_pDesktopManagerInternal, VT_IDX_SwitchDesktop);
        hr = pfnSwitchDesktop(g_pDesktopManagerInternal, nullptr, pDesktop);
    } else {
        typedef HRESULT(STDMETHODCALLTYPE* SwitchDesktopFunc)(void*, IVirtualDesktopInternal*);
        auto pfnSwitchDesktop = GetVTableFunction<SwitchDesktopFunc>(g_pDesktopManagerInternal, VT_IDX_SwitchDesktop);
        hr = pfnSwitchDesktop(g_pDesktopManagerInternal, pDesktop);
    }

    if (FAILED(hr)) {
        Wh_Log(L"SwitchDesktop failed: 0x%08X", hr);
        return false;
    }

    return true;
}

// Main function: Move window to desktop number (1-based, like VD.ahk)
bool MoveWindowToDesktopNum(HWND hwnd, int desktopNum) {
    if (!InitializeVirtualDesktopAPI()) {
        Wh_Log(L"Failed to initialize Virtual Desktop API");
        return false;
    }

    if (!hwnd || !IsWindow(hwnd)) {
        Wh_Log(L"Invalid window handle");
        return false;
    }

    // Convert 1-based to 0-based index
    int index = desktopNum - 1;

    // Get the target desktop
    IVirtualDesktopInternal* pDesktop = GetDesktopByIndex(index);
    if (!pDesktop) {
        Wh_Log(L"Desktop %d not found", desktopNum);
        return false;
    }

    // Get IApplicationView for the window
    IApplicationViewVD* pView = GetApplicationViewForHwnd(hwnd);
    if (!pView) {
        Wh_Log(L"Could not get ApplicationView for window");
        pDesktop->Release();
        return false;
    }

    // Move the view to the desktop
    bool success = MoveViewToDesktop(pView, pDesktop);

    pView->Release();
    pDesktop->Release();

    if (success) {
        Wh_Log(L"Successfully moved window to desktop %d", desktopNum);
    }

    return success;
}

// Switch to a desktop number (1-based)
bool GoToDesktopNum(int desktopNum) {
    if (!InitializeVirtualDesktopAPI()) {
        Wh_Log(L"Failed to initialize Virtual Desktop API");
        return false;
    }

    if (desktopNum <= 0) {
        Wh_Log(L"Invalid desktop number: %d", desktopNum);
        return false;
    }

    int index = desktopNum - 1;
    IVirtualDesktopInternal* pDesktop = GetDesktopByIndex(index);
    if (!pDesktop) {
        Wh_Log(L"Desktop %d not found", desktopNum);
        return false;
    }

    bool success = SwitchToDesktop(pDesktop);
    pDesktop->Release();

    if (success) {
        Wh_Log(L"Switched to desktop %d", desktopNum);
    }

    return success;
}

// Move active window to desktop
bool MoveActiveWindowToDesktopNum(int desktopNum) {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) {
        Wh_Log(L"No active window");
        return false;
    }
    return MoveWindowToDesktopNum(hwnd, desktopNum);
}



//=============================================================================
// Windhawk Tool Mod Entry Points
//=============================================================================

BOOL WhTool_ModInit() {
    Wh_Log(L"Virtual Desktop Helper mod initializing...");

    // Load settings first
    LoadSettings();

    if (!StartHotkeyThread()) {
        Wh_Log(L"Failed to register required hotkeys");
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

    // Stop hotkey thread
    StopHotkeyThread();

    // Reload settings
    LoadSettings();

    // Restart hotkey thread
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
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
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

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

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
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

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
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
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
