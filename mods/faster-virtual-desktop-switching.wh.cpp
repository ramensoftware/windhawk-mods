// ==WindhawkMod==
// @id              faster-virtual-desktop-switching
// @name            Faster Virtual Desktop Switching
// @description     Removes the long pre-animation lag caused by oversized wallpaper thumbnail requests on Windows 11.
// @version         1.0
// @author          meteoni
// @github          https://github.com/Meteony
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Faster Virtual Desktop Switching

Removes the long-standing pre-animation lag caused by oversized wallpaper thumbnail requests on Windows 11.

![GIF](https://raw.githubusercontent.com/Meteony/meteoni-assets/main/faster-virtual-desktop-switching/faster-virtual-desktop-switching.gif)

_Note: This can make the wallpaper look softer during the switch. The mod is a no-op on Windows 10._

Windows 11 can synchronously request a full-monitor-sized wallpaper thumbnail
before beginning a virtual desktop transition. On a high-DPI display with specific wallpaper
configurations, that request can take roughly 136-169 ms when warm and more than one second 
when cold (exact figure depends).

This mod caps that request to the maximum size documented for `IThumbnailCache`, which is 
1024 pixels. In testing, that netted a latency reduction of ~150ms, effectively eliminating 
the long-standing pre-animation stutter issue. 

Laptop users who use three- or four-finger swipe gestures can especially benefit from this mod. 

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- maximumThumbnailSize: 1024
  $name: Maximum thumbnail size
  $description: Requested size used for the transition wallpaper, clamped to the range 64-1024. Microsoft documents 1024 as the maximum supported value.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <atomic>

namespace {

struct WtsThumbnailId {
    BYTE value[16];
};

using ThumbnailCacheGetThumbnail = HRESULT(STDMETHODCALLTYPE*)(
    void* self,
    void* shellItem,
    UINT requestedSize,
    DWORD flags,
    void** sharedBitmap,
    DWORD* outFlags,
    WtsThumbnailId* thumbnailId);
using MakeBackgroundThumbnailFromThumbnailCache = HRESULT(STDMETHODCALLTYPE*)(
    void* self,
    void* dcompThumbnail,
    void* virtualDesktop,
    void* wallpaperPath,
    RECT bounds,
    void** result);
using LoadLibraryExW_t = decltype(&LoadLibraryExW);

// Every target needs its own original pointer. Reusing one across registrations
// would make an earlier detour call whichever target was registered last.
ThumbnailCacheGetThumbnail g_thumbcacheGetThumbnailOriginal = nullptr;
ThumbnailCacheGetThumbnail g_thumbcacheApiGetThumbnailOriginal = nullptr;
ThumbnailCacheGetThumbnail g_windowsStorageGetThumbnailOriginal = nullptr;
ThumbnailCacheGetThumbnail g_windowsStorageApiGetThumbnailOriginal = nullptr;
MakeBackgroundThumbnailFromThumbnailCache
    g_makeBackgroundThumbnailOriginal = nullptr;
LoadLibraryExW_t g_loadLibraryExWOriginal = nullptr;
thread_local int g_makeBackgroundThumbnailDepth = 0;
std::atomic_bool g_twinuiPcShellHookAttempted{false};
std::atomic_bool g_thumbcacheHookAttempted{false};
std::atomic_bool g_windowsStorageHookAttempted{false};
std::atomic_bool g_thumbnailClampLogged{false};
std::atomic<int> g_maximumThumbnailSize{1024};

// Load settings atomically because Explorer can switch desktops while the
// Windhawk settings callback runs on another thread.
void LoadSettings() {
    int maximumSize = Wh_GetIntSetting(L"maximumThumbnailSize");
    if (maximumSize < 64) {
        maximumSize = 64;
    } else if (maximumSize > 1024) {
        maximumSize = 1024;
    }
    g_maximumThumbnailSize.store(maximumSize);
    g_thumbnailClampLogged.store(false);
}

bool IsWindows11OrGreater() {
    using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOW*);
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    auto rtlGetVersion = ntdll
        ? reinterpret_cast<RtlGetVersion_t>(
              GetProcAddress(ntdll, "RtlGetVersion"))
        : nullptr;
    if (!rtlGetVersion) {
        return false;
    }

    OSVERSIONINFOW versionInfo{};
    versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
    return rtlGetVersion(&versionInfo) == 0 &&
           versionInfo.dwMajorVersion >= 10 &&
           versionInfo.dwBuildNumber >= 22000;
}

HRESULT STDMETHODCALLTYPE MakeBackgroundThumbnailFromThumbnailCacheHook(
    void* self,
    void* dcompThumbnail,
    void* virtualDesktop,
    void* wallpaperPath,
    RECT bounds,
    void** result) {
    struct DepthGuard {
        DepthGuard() {
            ++g_makeBackgroundThumbnailDepth;
        }

        ~DepthGuard() {
            --g_makeBackgroundThumbnailDepth;
        }
    } depthGuard;

    return g_makeBackgroundThumbnailOriginal(
        self,
        dcompThumbnail,
        virtualDesktop,
        wallpaperPath,
        bounds,
        result);
}

UINT ClampTransitionThumbnailSize(UINT requestedSize) {
    if (g_makeBackgroundThumbnailDepth <= 0) {
        return requestedSize;
    }

    const UINT maximumSize =
        static_cast<UINT>(g_maximumThumbnailSize.load());
    if (requestedSize <= maximumSize) {
        return requestedSize;
    }

    if (!g_thumbnailClampLogged.exchange(true)) {
        Wh_Log(L"Clamped virtual desktop wallpaper thumbnail from %u to %u",
               requestedSize,
               maximumSize);
    }
    return maximumSize;
}

HRESULT CallThumbnailCacheGetThumbnail(
    ThumbnailCacheGetThumbnail original,
    void* self,
    void* shellItem,
    UINT requestedSize,
    DWORD flags,
    void** sharedBitmap,
    DWORD* outFlags,
    WtsThumbnailId* thumbnailId) {
    requestedSize = ClampTransitionThumbnailSize(requestedSize);
    return original(
        self,
        shellItem,
        requestedSize,
        flags,
        sharedBitmap,
        outFlags,
        thumbnailId);
}

// Clamp only calls made synchronously by Windows' virtual desktop wallpaper
// helper. All other thumbnail-cache clients receive their original arguments.
HRESULT STDMETHODCALLTYPE ThumbcacheGetThumbnailHook(
    void* self,
    void* shellItem,
    UINT requestedSize,
    DWORD flags,
    void** sharedBitmap,
    DWORD* outFlags,
    WtsThumbnailId* thumbnailId) {
    return CallThumbnailCacheGetThumbnail(
        g_thumbcacheGetThumbnailOriginal,
        self,
        shellItem,
        requestedSize,
        flags,
        sharedBitmap,
        outFlags,
        thumbnailId);
}

HRESULT STDMETHODCALLTYPE ThumbcacheApiGetThumbnailHook(
    void* self,
    void* shellItem,
    UINT requestedSize,
    DWORD flags,
    void** sharedBitmap,
    DWORD* outFlags,
    WtsThumbnailId* thumbnailId) {
    return CallThumbnailCacheGetThumbnail(
        g_thumbcacheApiGetThumbnailOriginal,
        self,
        shellItem,
        requestedSize,
        flags,
        sharedBitmap,
        outFlags,
        thumbnailId);
}

HRESULT STDMETHODCALLTYPE WindowsStorageGetThumbnailHook(
    void* self,
    void* shellItem,
    UINT requestedSize,
    DWORD flags,
    void** sharedBitmap,
    DWORD* outFlags,
    WtsThumbnailId* thumbnailId) {
    return CallThumbnailCacheGetThumbnail(
        g_windowsStorageGetThumbnailOriginal,
        self,
        shellItem,
        requestedSize,
        flags,
        sharedBitmap,
        outFlags,
        thumbnailId);
}

HRESULT STDMETHODCALLTYPE WindowsStorageApiGetThumbnailHook(
    void* self,
    void* shellItem,
    UINT requestedSize,
    DWORD flags,
    void** sharedBitmap,
    DWORD* outFlags,
    WtsThumbnailId* thumbnailId) {
    return CallThumbnailCacheGetThumbnail(
        g_windowsStorageApiGetThumbnailOriginal,
        self,
        shellItem,
        requestedSize,
        flags,
        sharedBitmap,
        outFlags,
        thumbnailId);
}

// Hook the outer wallpaper helper so the thumbnail-cache detour can identify
// its dynamic call context without relying on compiler code layout.
bool HookBackgroundThumbnailHelper(HMODULE module) {
    if (!module || g_twinuiPcShellHookAttempted.exchange(true)) {
        return false;
    }

    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {
                L"protected: long __cdecl VirtualDesktopGestureWindow::MakeBackgroundThumbnailFromThumbnailCache(struct IDCompThumbnail *,struct IVirtualDesktop *,struct HSTRING__ *,struct tagRECT,struct IDCompThumbnail * *)",
            },
            &g_makeBackgroundThumbnailOriginal,
            MakeBackgroundThumbnailFromThumbnailCacheHook,
            false,
        },
    };

    const bool symbolsHooked = WindhawkUtils::HookSymbols(
        module, symbolHooks, ARRAYSIZE(symbolHooks));
    if (!g_makeBackgroundThumbnailOriginal) {
        Wh_Log(L"Failed to hook the virtual desktop wallpaper helper");
        return false;
    }
    if (!symbolsHooked) {
        Wh_Log(L"HookSymbols reported a failure after registering the "
               L"virtual desktop wallpaper helper hook");
    }
    return true;
}

// The implementation moved between windows.storage.dll and thumbcache.dll.
// Hook each loaded copy independently, avoiding COM activation and a hard-coded
// vtable slot.
bool HookThumbnailCache(
    HMODULE module,
    std::atomic_bool* hookAttempted,
    PCWSTR moduleName,
    ThumbnailCacheGetThumbnail* getThumbnailOriginal,
    ThumbnailCacheGetThumbnail getThumbnailHook,
    ThumbnailCacheGetThumbnail* apiGetThumbnailOriginal,
    ThumbnailCacheGetThumbnail apiGetThumbnailHook) {
    if (!module || hookAttempted->exchange(true)) {
        return false;
    }

    // thumbcache.dll, windows.storage.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {
                L"public: virtual long __cdecl CThumbnailCache::GetThumbnail(struct IShellItem *,unsigned int,enum WTS_FLAGS,struct ISharedBitmap * *,enum WTS_CACHEFLAGS *,struct WTS_THUMBNAILID *)",
            },
            getThumbnailOriginal,
            getThumbnailHook,
            true,
        },
        {
            {
                L"public: virtual long __cdecl CThumbnailCacheAPI::GetThumbnail(struct IShellItem *,unsigned int,enum WTS_FLAGS,struct ISharedBitmap * *,enum WTS_CACHEFLAGS *,struct WTS_THUMBNAILID *)",
            },
            apiGetThumbnailOriginal,
            apiGetThumbnailHook,
            true,
        },
    };

    const bool symbolsHooked = WindhawkUtils::HookSymbols(
        module, symbolHooks, ARRAYSIZE(symbolHooks));
    const bool registeredHook = *getThumbnailOriginal ||
                                *apiGetThumbnailOriginal;
    if (!registeredHook) {
        Wh_Log(L"Failed to hook thumbnail cache in %s", moduleName);
        return false;
    }
    if (!symbolsHooked) {
        Wh_Log(L"HookSymbols reported a failure after registering a "
               L"thumbnail cache hook in %s",
               moduleName);
    }

    Wh_Log(L"Hooked thumbnail cache in %s (concrete=%d api=%d)",
           moduleName,
           *getThumbnailOriginal != nullptr,
           *apiGetThumbnailOriginal != nullptr);
    return true;
}

bool HandleLoadedModule(HMODULE module) {
    if (!module) {
        return false;
    }

    if (!g_twinuiPcShellHookAttempted.load() &&
        module == GetModuleHandleW(L"twinui.pcshell.dll")) {
        return HookBackgroundThumbnailHelper(module);
    }
    if (!g_thumbcacheHookAttempted.load() &&
        module == GetModuleHandleW(L"thumbcache.dll")) {
        return HookThumbnailCache(
            module,
            &g_thumbcacheHookAttempted,
            L"thumbcache.dll",
            &g_thumbcacheGetThumbnailOriginal,
            ThumbcacheGetThumbnailHook,
            &g_thumbcacheApiGetThumbnailOriginal,
            ThumbcacheApiGetThumbnailHook);
    }
    if (!g_windowsStorageHookAttempted.load() &&
        module == GetModuleHandleW(L"windows.storage.dll")) {
        return HookThumbnailCache(
            module,
            &g_windowsStorageHookAttempted,
            L"windows.storage.dll",
            &g_windowsStorageGetThumbnailOriginal,
            WindowsStorageGetThumbnailHook,
            &g_windowsStorageApiGetThumbnailOriginal,
            WindowsStorageApiGetThumbnailHook);
    }
    return false;
}

bool HandleAlreadyLoadedModules() {
    bool hooksRegistered = HandleLoadedModule(
        GetModuleHandleW(L"twinui.pcshell.dll"));
    hooksRegistered = HandleLoadedModule(
                          GetModuleHandleW(L"thumbcache.dll")) ||
                      hooksRegistered;
    hooksRegistered = HandleLoadedModule(
                          GetModuleHandleW(L"windows.storage.dll")) ||
                      hooksRegistered;
    return hooksRegistered;
}

HMODULE WINAPI LoadLibraryExWHook(
    LPCWSTR fileName, HANDLE file, DWORD flags) {
    HMODULE module = g_loadLibraryExWOriginal(fileName, file, flags);
    constexpr DWORD dataOnlyFlags =
        LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
        LOAD_LIBRARY_AS_IMAGE_RESOURCE;
    if (!module || (flags & dataOnlyFlags)) {
        return module;
    }

    if (HandleLoadedModule(module) && !Wh_ApplyHookOperations()) {
        Wh_Log(L"Failed to apply late-loaded hooks");
    }
    return module;
}

}  // namespace

BOOL Wh_ModInit() {
    if (!IsWindows11OrGreater()) {
        Wh_Log(L"Skipping unsupported Windows version");
        return FALSE;
    }

    LoadSettings();

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto loadLibraryExW = kernelBase
        ? reinterpret_cast<LoadLibraryExW_t>(
              GetProcAddress(kernelBase, "LoadLibraryExW"))
        : nullptr;
    if (!loadLibraryExW ||
        !WindhawkUtils::SetFunctionHook(
            loadLibraryExW,
            LoadLibraryExWHook,
            &g_loadLibraryExWOriginal)) {
        Wh_Log(L"Failed to hook kernelbase!LoadLibraryExW");
        return FALSE;
    }

    HandleAlreadyLoadedModules();
    if (g_twinuiPcShellHookAttempted.load() &&
        !g_makeBackgroundThumbnailOriginal) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (HandleAlreadyLoadedModules() && !Wh_ApplyHookOperations()) {
        Wh_Log(L"Failed to apply hooks in Wh_ModAfterInit");
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
