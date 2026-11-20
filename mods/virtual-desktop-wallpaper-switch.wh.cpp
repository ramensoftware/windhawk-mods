// ==WindhawkMod==
// @id              virtual-desktop-wallpaper-switch
// @name            Virtual Desktop Wallpaper Switch
// @description     Set unique background wallpapers for each Windows 11 (24H2+) virtual desktop.
// @version         1.0.0
// @author          Dev-StudioCode
// @github          https://github.com/Dev-StudioCode
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -luuid -lruntimeobject -lshlwapi -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Virtual Desktop Wallpaper Switch

Assigns, preloads, and coordinates distinct background wallpapers across virtual desktops on Windows 11 Version 24H2 and newer (Build 26100+).

### Key Features
* **Per-Desktop Wallpapers**: Assigns individual background images per virtual workspace from a folder or specific overrides.
* **Touchpad Preloading**: Direct assignment through Windows 11's internal virtual desktop manager ensures textures are pre-cached for smooth trackpad swipe transitions.
* **Original Wallpaper Restoration**: Automatically saves your original desktop wallpapers and restores them when the mod is disabled or unloaded.
* **Safe In-Process Design**: Runs on a dedicated internal thread inside `explorer.exe` with zero vtable patching, avoiding system instability.

### Compatibility Note
* Designed specifically for Windows 11 Version 24H2 and newer (Build 26100+).
* If using **Wallpaper Engine**: Open Wallpaper Engine Settings -> General -> uncheck "Override wallpaper image" so Windows native per-desktop backgrounds remain active.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- wallpaperFolder: ""
  $name: Wallpaper Folder Path
  $description: Full path to a folder containing images to distribute across virtual desktops.
- desktop1Wallpaper: ""
  $name: Desktop 1 Wallpaper (Optional)
  $description: Specific image path to assign to virtual desktop 1.
- desktop2Wallpaper: ""
  $name: Desktop 2 Wallpaper (Optional)
  $description: Specific image path to assign to virtual desktop 2.
- desktop3Wallpaper: ""
  $name: Desktop 3 Wallpaper (Optional)
  $description: Specific image path to assign to virtual desktop 3.
- desktop4Wallpaper: ""
  $name: Desktop 4 Wallpaper (Optional)
  $description: Specific image path to assign to virtual desktop 4.
- shuffle: true
  $name: Shuffle Folder Wallpapers
  $description: Randomize wallpaper assignments across desktops from the folder.
- restoreOriginalOnUnload: true
  $name: Restore Original Wallpapers on Exit
  $description: Restore original desktop wallpapers when the mod is disabled or unloaded.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
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

constexpr IID IID_IUnknown_Local = {
    0x00000000, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
constexpr IID IID_IServiceProvider_Local = {
    0x6D5140C1, 0x7436, 0x11CE, {0x80, 0x34, 0x00, 0xAA, 0x00, 0x60, 0x09, 0xFA}};

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
        WindowsCreateString(str.c_str(), static_cast<UINT32>(str.length()), &hstr);
    }
    ~ScopedHString() {
        if (hstr) {
            WindowsDeleteString(hstr);
        }
    }
    operator HSTRING() const { return hstr; }
};

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

struct OriginalWallpaperEntry {
    GUID desktopId;
    std::wstring path;
};

std::vector<OriginalWallpaperEntry> g_originalWallpapers;
std::vector<std::wstring> g_folderWallpapers;

HANDLE g_hWorkerThread = nullptr;
DWORD g_workerThreadId = 0;
HANDLE g_hWorkerReadyEvent = nullptr;

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
// Wallpaper Backup, Resolution & Application
// ---------------------------------------------------------------------------

void BackupOriginalWallpapers(IVirtualDesktopManagerInternal* pMgr) {
    if (!g_originalWallpapers.empty()) {
        return; // Only capture initial state before any mod changes
    }

    IObjectArray* pDesktops = nullptr;
    if (FAILED(pMgr->GetDesktops(&pDesktops)) || !pDesktops) {
        return;
    }

    UINT count = 0;
    pDesktops->GetCount(&count);

    for (UINT i = 0; i < count; i++) {
        IVirtualDesktop* pDesktop = nullptr;
        if (SUCCEEDED(pDesktops->GetAt(i, IID_IVirtualDesktop, (void**)&pDesktop)) && pDesktop) {
            GUID id{};
            pDesktop->GetId(&id);

            HSTRING hPath = nullptr;
            std::wstring pathStr;
            if (SUCCEEDED(pDesktop->GetWallpaperPath(&hPath)) && hPath) {
                PCWSTR raw = WindowsGetStringRawBuffer(hPath, nullptr);
                if (raw) {
                    pathStr = raw;
                }
                WindowsDeleteString(hPath);
            }

            g_originalWallpapers.push_back({id, pathStr});
            pDesktop->Release();
        }
    }

    pDesktops->Release();
}

void RestoreOriginalWallpapers(IVirtualDesktopManagerInternal* pMgr) {
    if (!g_settings.restoreOriginalOnUnload || g_originalWallpapers.empty()) {
        return;
    }

    Wh_Log(L"Restoring original virtual desktop wallpapers...");
    for (const auto& entry : g_originalWallpapers) {
        IVirtualDesktop* pDesktop = nullptr;
        if (SUCCEEDED(pMgr->FindDesktop(&entry.desktopId, &pDesktop)) && pDesktop) {
            ScopedHString hPath(entry.path);
            pMgr->SetDesktopWallpaper(pDesktop, hPath);
            pDesktop->Release();
        }
    }
    g_originalWallpapers.clear();
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

void ApplyAllWallpapers(IVirtualDesktopManagerInternal* pMgr) {
    IObjectArray* pDesktops = nullptr;
    if (FAILED(pMgr->GetDesktops(&pDesktops)) || !pDesktops) {
        return;
    }

    UINT count = 0;
    pDesktops->GetCount(&count);

    for (UINT i = 0; i < count; i++) {
        IVirtualDesktop* pDesktop = nullptr;
        if (SUCCEEDED(pDesktops->GetAt(i, IID_IVirtualDesktop, (void**)&pDesktop)) && pDesktop) {
            std::wstring targetWallpaper = GetConfiguredWallpaperForDesktop(i);
            if (!targetWallpaper.empty()) {
                ScopedHString hPath(targetWallpaper);
                pMgr->SetDesktopWallpaper(pDesktop, hPath);
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
    DWORD m_targetThreadId = 0;

public:
    NotificationSink(DWORD targetThreadId) : m_targetThreadId(targetThreadId) {}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override {
        if (!ppvObject) return E_POINTER;
        if (riid == IID_IUnknown_Local || riid == IID_IVirtualDesktopNotification) {
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
        PostThreadMessageW(m_targetThreadId, WM_APP_DESKTOPS_CHANGED, 0, 0);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyBegin(IVirtualDesktop*, IVirtualDesktop*) override {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyFailed(IVirtualDesktop*, IVirtualDesktop*) override {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyed(IVirtualDesktop*, IVirtualDesktop*) override {
        PostThreadMessageW(m_targetThreadId, WM_APP_DESKTOPS_CHANGED, 0, 0);
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
// Worker Thread Procedure
// ---------------------------------------------------------------------------

DWORD WINAPI WorkerThreadProc(LPVOID) {
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    // Initialize thread message queue before signaling readiness
    MSG msgDummy;
    PeekMessageW(&msgDummy, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    IServiceProvider* pServiceProvider = nullptr;
    IVirtualDesktopManagerInternal* pDesktopManager = nullptr;
    IVirtualDesktopNotificationService* pNotificationService = nullptr;
    NotificationSink* pSink = nullptr;
    DWORD dwCookie = 0;

    // Retry connection while shell initializes
    for (int attempts = 0; attempts < 40; attempts++) {
        HRESULT hr = CoCreateInstance(
            CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER,
            IID_IServiceProvider_Local,
            (void**)&pServiceProvider);

        if (SUCCEEDED(hr) && pServiceProvider) {
            pServiceProvider->QueryService(
                SID_VirtualDesktopManagerInternal,
                IID_IVirtualDesktopManagerInternal,
                (void**)&pDesktopManager);

            pServiceProvider->QueryService(
                SID_VirtualDesktopNotificationService,
                IID_IVirtualDesktopNotificationService,
                (void**)&pNotificationService);

            if (pDesktopManager && pNotificationService) {
                break;
            }
        }
        Sleep(250);
    }

    if (!pDesktopManager || !pNotificationService) {
        Wh_Log(L"Could not connect to Windows Virtual Desktop services");
        if (g_hWorkerReadyEvent) SetEvent(g_hWorkerReadyEvent);
        if (pServiceProvider) pServiceProvider->Release();
        if (SUCCEEDED(hrCo)) CoUninitialize();
        return 1;
    }

    // 1. Initial backup of unmodified user wallpapers
    BackupOriginalWallpapers(pDesktopManager);

    // 2. Load settings and preload wallpapers
    LoadSettings();
    ApplyAllWallpapers(pDesktopManager);

    // 3. Register notification sink
    pSink = new NotificationSink(GetCurrentThreadId());
    HRESULT hrReg = pNotificationService->Register(pSink, &dwCookie);
    if (FAILED(hrReg)) {
        Wh_Log(L"Notification register failed: 0x%08X", hrReg);
    }

    // Signal initialization completion to Wh_ModInit
    if (g_hWorkerReadyEvent) {
        SetEvent(g_hWorkerReadyEvent);
    }

    // Message loop for thread-safe asynchronous notifications and settings updates
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_APP_RELOAD_SETTINGS) {
            LoadSettings();
            ApplyAllWallpapers(pDesktopManager);
        } else if (msg.message == WM_APP_DESKTOPS_CHANGED) {
            ApplyAllWallpapers(pDesktopManager);
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // Unregister notification sink
    if (pNotificationService && dwCookie) {
        pNotificationService->Unregister(dwCookie);
        dwCookie = 0;
    }

    if (pSink) {
        CoDisconnectObject(static_cast<IUnknown*>(pSink), 0);
        pSink->Release();
        pSink = nullptr;
    }

    // Restore original wallpapers
    RestoreOriginalWallpapers(pDesktopManager);

    if (pDesktopManager) {
        pDesktopManager->Release();
        pDesktopManager = nullptr;
    }
    if (pNotificationService) {
        pNotificationService->Release();
        pNotificationService = nullptr;
    }
    if (pServiceProvider) {
        pServiceProvider->Release();
        pServiceProvider = nullptr;
    }

    if (SUCCEEDED(hrCo)) {
        CoUninitialize();
    }
    return 0;
}

bool IsPrimaryExplorerProcess() {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argv) {
        for (int i = 1; i < argc; i++) {
            if (_wcsicmp(argv[i], L"/factory") == 0 ||
                _wcsicmp(argv[i], L"/separate") == 0) {
                LocalFree(argv);
                return false;
            }
        }
        LocalFree(argv);
    }
    return true;
}

}  // namespace

// ---------------------------------------------------------------------------
// Windhawk Lifecycle Entry Points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Virtual Desktop Wallpaper Switch (24H2+)");

    if (!IsPrimaryExplorerProcess()) {
        Wh_Log(L"Skipping non-primary Explorer process");
        return FALSE;
    }

    g_hWorkerReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hWorkerReadyEvent) {
        Wh_Log(L"Failed to create worker ready event");
        return FALSE;
    }

    g_hWorkerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, &g_workerThreadId);
    if (!g_hWorkerThread) {
        Wh_Log(L"Failed to create worker thread: %lu", GetLastError());
        CloseHandle(g_hWorkerReadyEvent);
        g_hWorkerReadyEvent = nullptr;
        return FALSE;
    }

    // Wait up to 10 seconds for worker thread initialization
    WaitForSingleObject(g_hWorkerReadyEvent, 10000);
    CloseHandle(g_hWorkerReadyEvent);
    g_hWorkerReadyEvent = nullptr;

    return TRUE;
}

void Wh_ModSettingsChanged() {
    if (g_workerThreadId) {
        PostThreadMessageW(g_workerThreadId, WM_APP_RELOAD_SETTINGS, 0, 0);
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Virtual Desktop Wallpaper Switch");

    if (g_hWorkerThread && g_workerThreadId) {
        PostThreadMessageW(g_workerThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hWorkerThread, 7000);
        CloseHandle(g_hWorkerThread);
        g_hWorkerThread = nullptr;
        g_workerThreadId = 0;
    }
}
