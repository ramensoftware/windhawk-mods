// ==WindhawkMod==
// @id              virtual-desktop-wallpaper-switch
// @name            Virtual Desktop Wallpaper Switch
// @description     Set unique background wallpapers for each Windows 11 (24H2+) virtual desktop.
// @version         1.0.0
// @author          Dev-StudioCode
// @github          https://github.com/Dev-StudioCode
// @homepage        https://github.com/Dev-StudioCode/FluxDesk
// @include         windhawk.exe
// @compilerOptions -lole32 -lruntimeobject -lshlwapi -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Virtual Desktop Wallpaper Switch

Assigns, preloads, and coordinates distinct background wallpapers across virtual desktops on Windows 11 Version 24H2 and newer (Build 26100+).

### Key Features
* **Per-Desktop Wallpapers**: Assigns individual background images per virtual workspace from a folder or specific overrides.
* **Touchpad Preloading**: Direct assignment through Windows 11 internal virtual desktop manager ensures backgrounds are preloaded for smooth trackpad swipe transitions.
* **Persistent Wallpaper Backup**: Safely saves original desktop wallpaper paths to Windhawk mod storage before any assignment is applied, ensuring your original backgrounds can be restored even across Explorer restarts or reboots.
* **Isolated Tool Mod Design**: Runs as a dedicated background tool mod process (`windhawk.exe`) making COM calls to the Immersive Shell, guaranteeing Explorer stability with zero in-process hooks.

### System Changes and Persistence
* **What Windows Stores**: The mod assigns backgrounds using `IVirtualDesktopManagerInternal::SetDesktopWallpaper`. Windows persists per-desktop wallpapers in the user registry under `HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\VirtualDesktops\Desktops\{GUID}`.
* **Restoration on Exit**: When `restoreOriginalOnUnload` is enabled (default), unloading or disabling the mod restores your original desktop wallpapers from Windhawk storage and clears the saved backup entries. If disabled, the assigned wallpapers remain active in Windows settings and storage entries are cleared for a fresh baseline.

### Compatibility Note
* Designed specifically for Windows 11 Version 24H2 and newer (Build 26100+).
* If using **Wallpaper Engine**: Open Wallpaper Engine Settings -> General -> uncheck "Override wallpaper image" so Windows native per-desktop backgrounds remain active.

Source code, issue tracker & roadmap: [GitHub Repository](https://github.com/Dev-StudioCode/FluxDesk)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- wallpaperFolder: ""
  $name: Wallpaper Folder Path
  $description: 'Full path to a folder containing images to distribute across virtual desktops.'
- desktop1Wallpaper: ""
  $name: Desktop 1 Wallpaper (Optional)
  $description: 'Specific image path to assign to virtual desktop 1.'
- desktop2Wallpaper: ""
  $name: Desktop 2 Wallpaper (Optional)
  $description: 'Specific image path to assign to virtual desktop 2.'
- desktop3Wallpaper: ""
  $name: Desktop 3 Wallpaper (Optional)
  $description: 'Specific image path to assign to virtual desktop 3.'
- desktop4Wallpaper: ""
  $name: Desktop 4 Wallpaper (Optional)
  $description: 'Specific image path to assign to virtual desktop 4.'
- shuffle: true
  $name: Shuffle Folder Wallpapers
  $description: 'Randomize wallpaper assignments across desktops from the folder.'
- restoreOriginalOnUnload: true
  $name: Restore Original Wallpapers on Exit
  $description: 'Restore original desktop wallpapers when the mod is disabled or unloaded.'
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <servprov.h>
#include <objectarray.h>
#include <shlwapi.h>
#include <winstring.h>

#include <algorithm>
#include <atomic>
#include <random>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Shell COM Interface Declarations (Windows 11 24H2+ / Build 26100+)
// ---------------------------------------------------------------------------

namespace {

constexpr CLSID CLSID_ImmersiveShell = {
    0xC2F03A33, 0x21F5, 0x47FA, {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39}};
constexpr GUID SID_VirtualDesktopManagerInternal = {
    0xC5E0CDCA, 0x7B6E, 0x41B2, {0x9F, 0xC4, 0xD9, 0x39, 0x75, 0xCC, 0x46, 0x7B}};
constexpr GUID SID_VirtualDesktopNotificationService = {
    0xA501FDEC, 0x4A09, 0x464C, {0xAE, 0x4E, 0x1B, 0x9C, 0x21, 0xB8, 0x49, 0x18}};

constexpr IID IID_IVirtualDesktop = {
    0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}};
constexpr IID IID_IVirtualDesktopManagerInternal = {
    0x53F5CA0B, 0x158F, 0x4124, {0x90, 0x0C, 0x05, 0x71, 0x58, 0x06, 0x0B, 0x27}};
constexpr IID IID_IVirtualDesktopNotification = {
    0xB9E5E94D, 0x233E, 0x49AB, {0xAF, 0x5C, 0x2B, 0x45, 0x41, 0xC3, 0xAA, 0xDE}};
constexpr IID IID_IVirtualDesktopNotificationService = {
    0x0CD45E71, 0xD927, 0x4F15, {0x8B, 0x0A, 0x8F, 0xEF, 0x52, 0x53, 0x37, 0xBF}};

constexpr UINT WM_APP_RELOAD_SETTINGS = WM_APP + 101;
constexpr UINT WM_APP_DESKTOPS_CHANGED = WM_APP + 102;
constexpr UINT WM_APP_RECONNECT = WM_APP + 103;

constexpr UINT_PTR TIMER_CONNECT = 1001;

constexpr PCWSTR SYSTEM_DEFAULT_WALLPAPER_TAG = L"__SYSTEM_DEFAULT__";

struct IApplicationView : IUnknown {};

struct IVirtualDesktop : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE IsViewVisible(IApplicationView*, BOOL*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetId(GUID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetName(HSTRING*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetWallpaperPath(HSTRING*) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsRemote(BOOL*) = 0;
};

struct IVirtualDesktopManagerInternal : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetCount(UINT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveViewToDesktop(IApplicationView*, IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE CanViewMoveDesktops(IApplicationView*, BOOL*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentDesktop(IVirtualDesktop**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDesktops(IObjectArray**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAdjacentDesktop(IVirtualDesktop*, UINT, IVirtualDesktop**) = 0;
    virtual HRESULT STDMETHODCALLTYPE SwitchDesktop(IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SwitchDesktopAndMoveForegroundView(IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateDesktop(IVirtualDesktop**) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveDesktop(IVirtualDesktop*, UINT) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveDesktop(IVirtualDesktop*, IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindDesktop(const GUID*, IVirtualDesktop**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDesktopSwitchIncludeExcludeViews(IVirtualDesktop*, IObjectArray**, IObjectArray**) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDesktopName(IVirtualDesktop*, HSTRING) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDesktopWallpaper(IVirtualDesktop*, HSTRING) = 0;
    virtual HRESULT STDMETHODCALLTYPE UpdateWallpaperPathForAllDesktops(HSTRING) = 0;
    virtual HRESULT STDMETHODCALLTYPE CopyDesktopState(IApplicationView*, IApplicationView*) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateRemoteDesktop(HSTRING, IVirtualDesktop**) = 0;
    virtual HRESULT STDMETHODCALLTYPE SwitchRemoteDesktop(IVirtualDesktop*, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SwitchDesktopWithAnimation(IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLastActiveDesktop(IVirtualDesktop**) = 0;
    virtual HRESULT STDMETHODCALLTYPE WaitForAnimationToComplete() = 0;
};

struct IVirtualDesktopNotification : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopCreated(IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyBegin(IVirtualDesktop*, IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyFailed(IVirtualDesktop*, IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyed(IVirtualDesktop*, IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopMoved(IVirtualDesktop*, INT64, INT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopNameChanged(IVirtualDesktop*, HSTRING) = 0;
    virtual HRESULT STDMETHODCALLTYPE ViewVirtualDesktopChanged(IApplicationView*) = 0;
    virtual HRESULT STDMETHODCALLTYPE CurrentVirtualDesktopChanged(IVirtualDesktop*, IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopWallpaperChanged(IVirtualDesktop*, HSTRING) = 0;
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopSwitched(IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoteVirtualDesktopConnected(IVirtualDesktop*) = 0;
};

struct IVirtualDesktopNotificationService : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Register(IVirtualDesktopNotification*, DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Unregister(DWORD) = 0;
};

struct ScopedHString {
    HSTRING hstr = nullptr;
    ScopedHString(const std::wstring& str) {
        if (!str.empty()) {
            WindowsCreateString(str.c_str(), static_cast<UINT32>(str.length()), &hstr);
        }
    }
    ~ScopedHString() {
        if (hstr) {
            WindowsDeleteString(hstr);
        }
    }
    operator HSTRING() const { return hstr; }
};

inline bool IsDisconnectedHr(HRESULT hr) {
    return hr == RPC_E_DISCONNECTED ||
           hr == HRESULT_FROM_WIN32(RPC_S_SERVER_UNAVAILABLE) ||
           hr == HRESULT_FROM_WIN32(RPC_S_CALL_FAILED) ||
           hr == CO_E_OBJNOTCONNECTED;
}

// ---------------------------------------------------------------------------
// Mod Settings & State
// ---------------------------------------------------------------------------

struct Settings {
    std::wstring wallpaperFolder;
    std::wstring desktop1Wallpaper;
    std::wstring desktop2Wallpaper;
    std::wstring desktop3Wallpaper;
    std::wstring desktop4Wallpaper;
    bool shuffle = true;
    bool restoreOriginalOnUnload = true;
} g_settings;

std::vector<std::wstring> g_folderWallpapers;

HANDLE g_hWorkerThread = nullptr;
DWORD g_workerThreadId = 0;
HWND g_hWndMsg = nullptr;
UINT g_msgTaskbarCreated = 0;

IServiceProvider* g_pServiceProvider = nullptr;
IVirtualDesktopManagerInternal* g_pDesktopManager = nullptr;
IVirtualDesktopNotificationService* g_pNotificationService = nullptr;
class NotificationSink* g_pSink = nullptr;
DWORD g_dwCookie = 0;

// ---------------------------------------------------------------------------
// Settings Loader
// ---------------------------------------------------------------------------

void LoadSettings() {
    g_settings.wallpaperFolder =
        WindhawkUtils::StringSetting(Wh_GetStringSetting(L"wallpaperFolder")).get();
    g_settings.desktop1Wallpaper =
        WindhawkUtils::StringSetting(Wh_GetStringSetting(L"desktop1Wallpaper")).get();
    g_settings.desktop2Wallpaper =
        WindhawkUtils::StringSetting(Wh_GetStringSetting(L"desktop2Wallpaper")).get();
    g_settings.desktop3Wallpaper =
        WindhawkUtils::StringSetting(Wh_GetStringSetting(L"desktop3Wallpaper")).get();
    g_settings.desktop4Wallpaper =
        WindhawkUtils::StringSetting(Wh_GetStringSetting(L"desktop4Wallpaper")).get();

    g_settings.shuffle = Wh_GetIntSetting(L"shuffle") != 0;
    g_settings.restoreOriginalOnUnload = Wh_GetIntSetting(L"restoreOriginalOnUnload") != 0;

    g_folderWallpapers.clear();

    if (!g_settings.wallpaperFolder.empty()) {
        std::wstring searchPattern = g_settings.wallpaperFolder;
        if (searchPattern.back() != L'\\' && searchPattern.back() != L'/') {
            searchPattern += L'\\';
        }
        std::wstring basePath = searchPattern;
        searchPattern += L"*.*";

        WIN32_FIND_DATAW fd;
        HANDLE hFind = FindFirstFileW(searchPattern.c_str(), &fd);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    LPCWSTR ext = PathFindExtensionW(fd.cFileName);
                    if (ext && (_wcsicmp(ext, L".jpg") == 0 ||
                                _wcsicmp(ext, L".jpeg") == 0 ||
                                _wcsicmp(ext, L".png") == 0 ||
                                _wcsicmp(ext, L".bmp") == 0 ||
                                _wcsicmp(ext, L".webp") == 0)) {
                        g_folderWallpapers.push_back(basePath + fd.cFileName);
                    }
                }
            } while (FindNextFileW(hFind, &fd));
            FindClose(hFind);
        }

        if (g_settings.shuffle && g_folderWallpapers.size() > 1) {
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(g_folderWallpapers.begin(), g_folderWallpapers.end(), g);
        }
    }
}

// ---------------------------------------------------------------------------
// Wallpaper Helpers & Storage Management
// ---------------------------------------------------------------------------

std::wstring GetDesktopWallpaperPath(IVirtualDesktop* pDesktop) {
    HSTRING hPath = nullptr;
    std::wstring pathStr;
    if (SUCCEEDED(pDesktop->GetWallpaperPath(&hPath)) && hPath) {
        PCWSTR raw = WindowsGetStringRawBuffer(hPath, nullptr);
        if (raw) {
            pathStr = raw;
        }
        WindowsDeleteString(hPath);
    }
    return pathStr;
}

std::wstring GetConfiguredWallpaperForDesktop(UINT index) {
    switch (index) {
        case 0:
            if (!g_settings.desktop1Wallpaper.empty()) return g_settings.desktop1Wallpaper;
            break;
        case 1:
            if (!g_settings.desktop2Wallpaper.empty()) return g_settings.desktop2Wallpaper;
            break;
        case 2:
            if (!g_settings.desktop3Wallpaper.empty()) return g_settings.desktop3Wallpaper;
            break;
        case 3:
            if (!g_settings.desktop4Wallpaper.empty()) return g_settings.desktop4Wallpaper;
            break;
    }

    if (!g_folderWallpapers.empty()) {
        return g_folderWallpapers[index % g_folderWallpapers.size()];
    }

    return L"";
}

std::wstring GetStoredOriginal(PCWSTR guidStr) {
    std::wstring value(32768, L'\0');
    size_t len = Wh_GetStringValue(guidStr, value.data(), value.size());
    value.resize(len);
    return value;
}

void RestoreDesktopFromBackup(IVirtualDesktopManagerInternal* pMgr, IVirtualDesktop* pDesktop, PCWSTR guidStr) {
    std::wstring orig = GetStoredOriginal(guidStr);
    if (!orig.empty()) {
        if (orig == SYSTEM_DEFAULT_WALLPAPER_TAG) {
            ScopedHString hEmpty(L"");
            pMgr->SetDesktopWallpaper(pDesktop, hEmpty);
        } else {
            ScopedHString hOrig(orig);
            pMgr->SetDesktopWallpaper(pDesktop, hOrig);
        }
        Wh_DeleteValue(guidStr);
    }
}

void DisconnectShell();

void ApplyAllWallpapers(IVirtualDesktopManagerInternal* pMgr) {
    if (!pMgr) return;

    IObjectArray* pDesktops = nullptr;
    HRESULT hrDesktops = pMgr->GetDesktops(&pDesktops);
    if (FAILED(hrDesktops) || !pDesktops) {
        if (IsDisconnectedHr(hrDesktops) && g_hWndMsg) {
            PostMessageW(g_hWndMsg, WM_APP_RECONNECT, 0, 0);
        }
        return;
    }

    UINT count = 0;
    pDesktops->GetCount(&count);

    for (UINT i = 0; i < count; i++) {
        IVirtualDesktop* pDesktop = nullptr;
        if (SUCCEEDED(pDesktops->GetAt(i, IID_IVirtualDesktop, (void**)&pDesktop)) && pDesktop) {
            GUID id{};
            pDesktop->GetId(&id);
            WCHAR guidStr[40] = {0};
            StringFromGUID2(id, guidStr, ARRAYSIZE(guidStr));

            std::wstring targetWallpaper = GetConfiguredWallpaperForDesktop(i);
            std::wstring currentWallpaper = GetDesktopWallpaperPath(pDesktop);

            if (!targetWallpaper.empty()) {
                if (guidStr[0] != L'\0') {
                    std::wstring stored = GetStoredOriginal(guidStr);
                    if (stored.empty()) {
                        Wh_SetStringValue(
                            guidStr,
                            currentWallpaper.empty() ? SYSTEM_DEFAULT_WALLPAPER_TAG : currentWallpaper.c_str());
                    }
                }

                if (_wcsicmp(currentWallpaper.c_str(), targetWallpaper.c_str()) != 0) {
                    ScopedHString hPath(targetWallpaper);
                    pMgr->SetDesktopWallpaper(pDesktop, hPath);
                }
            } else {
                if (guidStr[0] != L'\0') {
                    RestoreDesktopFromBackup(pMgr, pDesktop, guidStr);
                }
            }
            pDesktop->Release();
        }
    }

    pDesktops->Release();
}

void RestoreOriginalWallpapers(IVirtualDesktopManagerInternal* pMgr) {
    if (!pMgr) return;

    IObjectArray* pDesktops = nullptr;
    if (FAILED(pMgr->GetDesktops(&pDesktops)) || !pDesktops) {
        return;
    }

    Wh_Log(L"Processing virtual desktop wallpapers unload routine");
    UINT count = 0;
    pDesktops->GetCount(&count);

    for (UINT i = 0; i < count; i++) {
        IVirtualDesktop* pDesktop = nullptr;
        if (SUCCEEDED(pDesktops->GetAt(i, IID_IVirtualDesktop, (void**)&pDesktop)) && pDesktop) {
            GUID id{};
            pDesktop->GetId(&id);
            WCHAR guidStr[40] = {0};
            if (StringFromGUID2(id, guidStr, ARRAYSIZE(guidStr)) > 0) {
                if (g_settings.restoreOriginalOnUnload) {
                    RestoreDesktopFromBackup(pMgr, pDesktop, guidStr);
                } else {
                    Wh_DeleteValue(guidStr);
                }
            }
            pDesktop->Release();
        }
    }

    pDesktops->Release();
}

// ---------------------------------------------------------------------------
// Shell Notification Sink (Async Message Dispatcher)
// ---------------------------------------------------------------------------

class NotificationSink final : public IVirtualDesktopNotification {
private:
    std::atomic<ULONG> m_refCount{1};
    HWND m_hWndTarget = nullptr;

public:
    NotificationSink(HWND hWndTarget) : m_hWndTarget(hWndTarget) {}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override {
        if (!ppvObject) return E_POINTER;
        if (riid == __uuidof(IUnknown) || riid == IID_IVirtualDesktopNotification) {
            *ppvObject = static_cast<IVirtualDesktopNotification*>(this);
            AddRef();
            return S_OK;
        }
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return ++m_refCount;
    }

    ULONG STDMETHODCALLTYPE Release() override {
        ULONG r = --m_refCount;
        if (!r) delete this;
        return r;
    }

    HRESULT STDMETHODCALLTYPE VirtualDesktopCreated(IVirtualDesktop*) override {
        if (m_hWndTarget) PostMessageW(m_hWndTarget, WM_APP_DESKTOPS_CHANGED, 0, 0);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyBegin(IVirtualDesktop*, IVirtualDesktop*) override {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyFailed(IVirtualDesktop*, IVirtualDesktop*) override {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyed(IVirtualDesktop*, IVirtualDesktop*) override {
        if (m_hWndTarget) PostMessageW(m_hWndTarget, WM_APP_DESKTOPS_CHANGED, 0, 0);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE VirtualDesktopMoved(IVirtualDesktop*, INT64, INT64) override {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE VirtualDesktopNameChanged(IVirtualDesktop*, HSTRING) override {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE ViewVirtualDesktopChanged(IApplicationView*) override {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE CurrentVirtualDesktopChanged(IVirtualDesktop*, IVirtualDesktop*) override {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE VirtualDesktopWallpaperChanged(IVirtualDesktop*, HSTRING) override {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE VirtualDesktopSwitched(IVirtualDesktop*) override {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE RemoteVirtualDesktopConnected(IVirtualDesktop*) override {
        return S_OK;
    }
};

// ---------------------------------------------------------------------------
// Shell Connection & Disconnection Management
// ---------------------------------------------------------------------------

void DisconnectShell() {
    if (g_pNotificationService && g_dwCookie) {
        g_pNotificationService->Unregister(g_dwCookie);
        g_dwCookie = 0;
    }

    if (g_pSink) {
        CoDisconnectObject(static_cast<IUnknown*>(g_pSink), 0);
        g_pSink->Release();
        g_pSink = nullptr;
    }

    if (g_pDesktopManager) {
        g_pDesktopManager->Release();
        g_pDesktopManager = nullptr;
    }

    if (g_pNotificationService) {
        g_pNotificationService->Release();
        g_pNotificationService = nullptr;
    }

    if (g_pServiceProvider) {
        g_pServiceProvider->Release();
        g_pServiceProvider = nullptr;
    }
}

HRESULT ConnectShell() {
    DisconnectShell();

    HRESULT hr = CoCreateInstance(
        CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER,
        IID_PPV_ARGS(&g_pServiceProvider));

    if (FAILED(hr) || !g_pServiceProvider) {
        return hr;
    }

    hr = g_pServiceProvider->QueryService(
        SID_VirtualDesktopManagerInternal,
        IID_IVirtualDesktopManagerInternal,
        (void**)&g_pDesktopManager);
    if (FAILED(hr) || !g_pDesktopManager) {
        DisconnectShell();
        return hr;
    }

    hr = g_pServiceProvider->QueryService(
        SID_VirtualDesktopNotificationService,
        IID_IVirtualDesktopNotificationService,
        (void**)&g_pNotificationService);
    if (FAILED(hr) || !g_pNotificationService) {
        DisconnectShell();
        return hr;
    }

    g_pSink = new NotificationSink(g_hWndMsg);
    HRESULT hrReg = g_pNotificationService->Register(g_pSink, &g_dwCookie);
    if (FAILED(hrReg)) {
        Wh_Log(L"Notification register failed: 0x%08X", hrReg);
    }

    return S_OK;
}

// ---------------------------------------------------------------------------
// Message Window Procedure & Event-Driven Message Loop
// ---------------------------------------------------------------------------

LRESULT CALLBACK MessageWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == g_msgTaskbarCreated && g_msgTaskbarCreated != 0) {
        if (g_pDesktopManager) {
            UINT count = 0;
            HRESULT hr = g_pDesktopManager->GetCount(&count);
            if (SUCCEEDED(hr)) {
                // Connection remains live; re-apply wallpapers without unnecessary restore
                ApplyAllWallpapers(g_pDesktopManager);
                return 0;
            }
        }
        // Connection died or not established; disconnect and schedule reconnect
        DisconnectShell();
        SetTimer(hWnd, TIMER_CONNECT, 500, nullptr);
        return 0;
    }

    switch (uMsg) {
        case WM_TIMER:
            if (wParam == TIMER_CONNECT) {
                KillTimer(hWnd, TIMER_CONNECT);
                HRESULT hr = ConnectShell();
                if (SUCCEEDED(hr)) {
                    ApplyAllWallpapers(g_pDesktopManager);
                } else if (hr == E_NOINTERFACE) {
                    Wh_Log(L"Virtual desktop COM interfaces unavailable on this Windows build: 0x%08X", hr);
                } else {
                    // Shell not yet ready; retry in 2000ms while keeping message queue active
                    SetTimer(hWnd, TIMER_CONNECT, 2000, nullptr);
                }
                return 0;
            }
            break;

        case WM_APP_RELOAD_SETTINGS:
            LoadSettings();
            if (g_pDesktopManager) {
                ApplyAllWallpapers(g_pDesktopManager);
            }
            return 0;

        case WM_APP_DESKTOPS_CHANGED:
            if (g_pDesktopManager) {
                ApplyAllWallpapers(g_pDesktopManager);
            }
            return 0;

        case WM_APP_RECONNECT:
            DisconnectShell();
            SetTimer(hWnd, TIMER_CONNECT, 1000, nullptr);
            return 0;

        case WM_DESTROY:
            KillTimer(hWnd, TIMER_CONNECT);
            return 0;
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    WNDCLASSW wc = {0};
    wc.lpfnWndProc = MessageWindowProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"Windhawk_VDWS_MsgWnd";

    if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"RegisterClassW failed: %lu", GetLastError());
        if (SUCCEEDED(hrCo)) CoUninitialize();
        return 1;
    }

    g_hWndMsg = CreateWindowExW(
        WS_EX_TOOLWINDOW, wc.lpszClassName, L"", WS_POPUP,
        0, 0, 0, 0, nullptr, nullptr, wc.hInstance, nullptr);

    if (!g_hWndMsg) {
        Wh_Log(L"CreateWindowExW failed: %lu", GetLastError());
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        if (SUCCEEDED(hrCo)) CoUninitialize();
        return 1;
    }

    g_msgTaskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");

    LoadSettings();

    // Start event-driven connection attempt without blocking message processing
    SetTimer(g_hWndMsg, TIMER_CONNECT, 50, nullptr);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // Unload path: restore original wallpapers only on shutdown
    KillTimer(g_hWndMsg, TIMER_CONNECT);
    if (g_pDesktopManager) {
        RestoreOriginalWallpapers(g_pDesktopManager);
    }
    DisconnectShell();

    if (g_hWndMsg) {
        DestroyWindow(g_hWndMsg);
        g_hWndMsg = nullptr;
    }
    UnregisterClassW(wc.lpszClassName, wc.hInstance);

    if (SUCCEEDED(hrCo)) {
        CoUninitialize();
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Tool Mod Dedicated Process Callbacks
// ---------------------------------------------------------------------------

BOOL WhTool_ModInit() {
    Wh_Log(L"Initializing Virtual Desktop Wallpaper Switch (Dedicated Tool Mod)");

    g_hWorkerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, &g_workerThreadId);
    if (!g_hWorkerThread) {
        Wh_Log(L"Failed to create worker thread: %lu", GetLastError());
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_hWndMsg) {
        PostMessageW(g_hWndMsg, WM_APP_RELOAD_SETTINGS, 0, 0);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninitializing Virtual Desktop Wallpaper Switch");

    if (g_hWorkerThread) {
        if (g_hWndMsg) {
            PostMessageW(g_hWndMsg, WM_QUIT, 0, 0);
        }
        WaitForSingleObject(g_hWorkerThread, 5000);
        CloseHandle(g_hWorkerThread);
        g_hWorkerThread = nullptr;
        g_workerThreadId = 0;
    }
}

}  // namespace

// ---------------------------------------------------------------------------
// Canonical Windhawk Tool Mod Launcher Boilerplate (Verbatim from Wiki)
// ---------------------------------------------------------------------------

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
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
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
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
