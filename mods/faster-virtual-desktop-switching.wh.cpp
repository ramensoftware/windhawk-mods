// ==WindhawkMod==
// @id              faster-virtual-desktop-switching
// @name            Faster Virtual Desktop Switching
// @description     Removes the long pre-animation lag caused by oversized wallpaper thumbnail requests on Windows 11.
// @version         1.0
// @author          meteoni
// @github          https://github.com/Meteony
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Faster Virtual Desktop Switching

Removes the long-standing pre-animation lag caused by oversized wallpaper thumbnail requests on Windows 11.

![GIF](https://raw.githubusercontent.com/Meteony/meteoni-assets/main/faster-virtual-desktop-switching/faster-virtual-desktop-switching.gif)

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
  $description: Requested size used for the transition wallpaper. Microsoft documents 1024 as the maximum supported value.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <objbase.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

namespace {

struct WtsThumbnailId {
    BYTE value[16];
};

struct IThumbnailCacheMinimal : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetThumbnail(
        IUnknown* shellItem,
        UINT requestedSize,
        DWORD flags,
        IUnknown** sharedBitmap,
        DWORD* outFlags,
        WtsThumbnailId* thumbnailId) = 0;
};

constexpr GUID kClsidLocalThumbnailCache = {
    0x50ef4544, 0xac9f, 0x4a8e,
    {0xb2, 0x1b, 0x8a, 0x26, 0x18, 0x0d, 0xb1, 0x3f}};
constexpr GUID kIidThumbnailCache = {
    0xf676c15d, 0x596a, 0x4ce2,
    {0x82, 0x34, 0x33, 0x99, 0x6f, 0x44, 0x5d, 0xb1}};

using ThumbnailCacheGetThumbnail = HRESULT(STDMETHODCALLTYPE*)(
    IThumbnailCacheMinimal* self,
    IUnknown* shellItem,
    UINT requestedSize,
    DWORD flags,
    IUnknown** sharedBitmap,
    DWORD* outFlags,
    WtsThumbnailId* thumbnailId);
using MakeBackgroundThumbnailFromThumbnailCache = HRESULT(*)(
    void* self,
    void* dcompThumbnail,
    void* virtualDesktop,
    void* wallpaperPath,
    RECT bounds,
    void** result);
using LoadLibraryExW_t = decltype(&LoadLibraryExW);

ThumbnailCacheGetThumbnail g_getThumbnailOriginal = nullptr;
MakeBackgroundThumbnailFromThumbnailCache
    g_makeBackgroundThumbnailOriginal = nullptr;
LoadLibraryExW_t g_loadLibraryExWOriginal = nullptr;
HMODULE g_twinuiPcShell = nullptr;
HMODULE g_thumbnailCacheServer = nullptr;
thread_local int g_makeBackgroundThumbnailDepth = 0;
constexpr LONG kCorePending = 0;
constexpr LONG kCoreInitializing = 1;
constexpr LONG kCoreInitialized = 2;
constexpr LONG kCoreFailed = -1;
volatile LONG g_coreInitializationState = kCorePending;
volatile LONG g_maximumThumbnailSize = 1024;

// Load settings atomically because Explorer can switch desktops while the
// Windhawk settings callback runs on another thread.
void LoadSettings() {
    int maximumSize = Wh_GetIntSetting(L"maximumThumbnailSize");
    if (maximumSize < 64) {
        maximumSize = 64;
    } else if (maximumSize > 1024) {
        maximumSize = 1024;
    }
    InterlockedExchange(&g_maximumThumbnailSize, maximumSize);
}

// Return true for the shell process, including early Explorer startup before
// its shell window exists. Separate folder processes are skipped once known.
bool IsOrMayBecomeShellExplorer() {
    HWND shellWindow = GetShellWindow();
    if (!shellWindow) {
        return true;
    }

    DWORD shellProcessId = 0;
    GetWindowThreadProcessId(shellWindow, &shellProcessId);
    return shellProcessId == GetCurrentProcessId();
}

HRESULT MakeBackgroundThumbnailFromThumbnailCacheHook(
    void* self,
    void* dcompThumbnail,
    void* virtualDesktop,
    void* wallpaperPath,
    RECT bounds,
    void** result) {
    ++g_makeBackgroundThumbnailDepth;
    const HRESULT resultCode = g_makeBackgroundThumbnailOriginal(
        self,
        dcompThumbnail,
        virtualDesktop,
        wallpaperPath,
        bounds,
        result);
    --g_makeBackgroundThumbnailDepth;
    return resultCode;
}

// Clamp only calls made synchronously by Windows' virtual desktop wallpaper
// helper. All other thumbnail-cache clients receive their original arguments.
HRESULT STDMETHODCALLTYPE GetThumbnailHook(
    IThumbnailCacheMinimal* self,
    IUnknown* shellItem,
    UINT requestedSize,
    DWORD flags,
    IUnknown** sharedBitmap,
    DWORD* outFlags,
    WtsThumbnailId* thumbnailId) {
    if (g_makeBackgroundThumbnailDepth > 0) {
        const UINT maximumSize = static_cast<UINT>(
            InterlockedCompareExchange(&g_maximumThumbnailSize, 0, 0));
        if (requestedSize > maximumSize) {
            requestedSize = maximumSize;
        }
    }

    return g_getThumbnailOriginal(
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
    if (!module) {
        return false;
    }

    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {
                L"protected: long __cdecl VirtualDesktopGestureWindow::MakeBackgroundThumbnailFromThumbnailCache(struct IDCompThumbnail *,struct IVirtualDesktop *,struct HSTRING__ *,struct tagRECT,struct IDCompThumbnail * *)",
            },
            reinterpret_cast<void**>(&g_makeBackgroundThumbnailOriginal),
            reinterpret_cast<void*>(
                MakeBackgroundThumbnailFromThumbnailCacheHook),
            false,
        },
    };

    if (!WindhawkUtils::HookSymbols(
            module, symbolHooks, ARRAYSIZE(symbolHooks)) ||
        !g_makeBackgroundThumbnailOriginal) {
        Wh_Log(L"Failed to hook the virtual desktop wallpaper helper");
        return false;
    }
    return true;
}

// Activate the public thumbnail-cache COM class long enough to discover its
// implementation, then pin only the implementing DLL for the hook lifetime.
bool HookThumbnailCache() {
    const HRESULT initializeResult =
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const bool uninitializeCom = SUCCEEDED(initializeResult);
    if (FAILED(initializeResult) &&
        initializeResult != RPC_E_CHANGED_MODE) {
        Wh_Log(L"COM initialization failed: 0x%08lX",
               static_cast<unsigned long>(initializeResult));
        return false;
    }

    IThumbnailCacheMinimal* cache = nullptr;
    const HRESULT activationResult = CoCreateInstance(
        kClsidLocalThumbnailCache,
        nullptr,
        CLSCTX_INPROC_SERVER,
        kIidThumbnailCache,
        reinterpret_cast<void**>(&cache));
    if (FAILED(activationResult) || !cache) {
        Wh_Log(L"LocalThumbnailCache activation failed: 0x%08lX",
               static_cast<unsigned long>(activationResult));
        if (uninitializeCom) {
            CoUninitialize();
        }
        return false;
    }

    void** vtable = *reinterpret_cast<void***>(cache);
    void* getThumbnailImplementation = vtable[3];
    const BOOL modulePinned = GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        reinterpret_cast<LPCWSTR>(getThumbnailImplementation),
        &g_thumbnailCacheServer);
    const DWORD modulePinError = modulePinned ? ERROR_SUCCESS : GetLastError();

    bool hookRegistered = false;
    if (modulePinned) {
        hookRegistered = Wh_SetFunctionHook(
            getThumbnailImplementation,
            reinterpret_cast<void*>(GetThumbnailHook),
            reinterpret_cast<void**>(&g_getThumbnailOriginal)) != FALSE;
    }

    cache->Release();
    if (uninitializeCom) {
        CoUninitialize();
    }

    if (!modulePinned) {
        Wh_Log(L"Failed to pin the thumbnail-cache implementation: %lu",
               modulePinError);
        return false;
    }

    if (!hookRegistered || !g_getThumbnailOriginal) {
        Wh_Log(L"Failed to hook IThumbnailCache::GetThumbnail");
        FreeLibrary(g_thumbnailCacheServer);
        g_thumbnailCacheServer = nullptr;
        return false;
    }

    return true;
}

// Balance local module references on every initialization failure. Hook
// removal is owned by Windhawk and occurs before Wh_ModUninit.
void ReleaseModuleReferences() {
    if (g_thumbnailCacheServer) {
        FreeLibrary(g_thumbnailCacheServer);
        g_thumbnailCacheServer = nullptr;
    }

    if (g_twinuiPcShell) {
        FreeLibrary(g_twinuiPcShell);
        g_twinuiPcShell = nullptr;
    }
}

bool InitializeCore(HMODULE twinuiPcShell, bool applyHookOperations) {
    if (InterlockedCompareExchange(
            &g_coreInitializationState,
            kCoreInitializing,
            kCorePending) != kCorePending) {
        return InterlockedCompareExchange(
                   &g_coreInitializationState, 0, 0) == kCoreInitialized;
    }

    if (!IsOrMayBecomeShellExplorer()) {
        Wh_Log(L"Skipping non-shell explorer.exe process %lu",
               GetCurrentProcessId());
        InterlockedExchange(&g_coreInitializationState, kCoreFailed);
        return false;
    }

    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
            reinterpret_cast<LPCWSTR>(twinuiPcShell),
            &g_twinuiPcShell)) {
        Wh_Log(L"Failed to retain twinui.pcshell.dll: %lu", GetLastError());
        InterlockedExchange(&g_coreInitializationState, kCoreFailed);
        return false;
    }

    if (!HookBackgroundThumbnailHelper(twinuiPcShell) ||
        !HookThumbnailCache()) {
        ReleaseModuleReferences();
        InterlockedExchange(&g_coreInitializationState, kCoreFailed);
        return false;
    }

    if (applyHookOperations) {
        SetLastError(ERROR_SUCCESS);
        if (!Wh_ApplyHookOperations()) {
            Wh_Log(L"Failed to apply late-loaded hooks: %lu", GetLastError());
            InterlockedExchange(&g_coreInitializationState, kCoreFailed);
            return false;
        }
    }

    InterlockedExchange(&g_coreInitializationState, kCoreInitialized);
    Wh_Log(L"Initialized: maximumThumbnailSize=%ld",
           InterlockedCompareExchange(&g_maximumThumbnailSize, 0, 0));
    return true;
}

HMODULE WINAPI LoadLibraryExWHook(
    LPCWSTR fileName, HANDLE file, DWORD flags) {
    HMODULE module = g_loadLibraryExWOriginal(fileName, file, flags);
    constexpr DWORD dataOnlyFlags =
        LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
        LOAD_LIBRARY_AS_IMAGE_RESOURCE;
    if (module && !(flags & dataOnlyFlags) &&
        InterlockedCompareExchange(&g_coreInitializationState, 0, 0) ==
            kCorePending &&
        GetModuleHandleW(L"twinui.pcshell.dll") == module) {
        InitializeCore(module, true);
    }
    return module;
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();

    if (!IsOrMayBecomeShellExplorer()) {
        Wh_Log(L"Skipping non-shell explorer.exe process %lu",
               GetCurrentProcessId());
        return TRUE;
    }

    if (HMODULE twinuiPcShell =
            GetModuleHandleW(L"twinui.pcshell.dll")) {
        return InitializeCore(twinuiPcShell, false) ? TRUE : FALSE;
    }

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

    return TRUE;
}

void Wh_ModAfterInit() {
    if (InterlockedCompareExchange(
            &g_coreInitializationState, 0, 0) != kCorePending) {
        return;
    }

    if (HMODULE twinuiPcShell =
            GetModuleHandleW(L"twinui.pcshell.dll")) {
        InitializeCore(twinuiPcShell, true);
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    // Windhawk calls this after removing registered hooks, so the implementation
    // DLLs can no longer be entered through this mod's detour.
    ReleaseModuleReferences();
}
