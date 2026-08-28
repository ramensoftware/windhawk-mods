// ==WindhawkMod==
// @id              faster-virtual-desktop-switching
// @name            Faster Virtual Desktop Switching
// @description     Removes the long pre-animation lag caused by oversized wallpaper thumbnail requests on Windows 11.
// @version         1.0
// @author          meteoni
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

#include <cstdint>

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

ThumbnailCacheGetThumbnail g_getThumbnailOriginal = nullptr;
HMODULE g_twinuiPcShell = nullptr;
HMODULE g_thumbnailCacheServer = nullptr;
uintptr_t g_targetFunctionStart = 0;
uintptr_t g_targetFunctionEnd = 0;
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

// Clamp only calls originating inside Windows' virtual desktop wallpaper
// helper. All other thumbnail-cache clients receive their original arguments.
__attribute__((noinline))
HRESULT STDMETHODCALLTYPE GetThumbnailHook(
    IThumbnailCacheMinimal* self,
    IUnknown* shellItem,
    UINT requestedSize,
    DWORD flags,
    IUnknown** sharedBitmap,
    DWORD* outFlags,
    WtsThumbnailId* thumbnailId) {
    const uintptr_t returnAddress = reinterpret_cast<uintptr_t>(
        __builtin_return_address(0));

    if (returnAddress >= g_targetFunctionStart &&
        returnAddress < g_targetFunctionEnd) {
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

// Resolve the target for the loaded Windows build, then obtain its exact code
// range from the x64 unwind table. No private function is detoured.
bool ResolveTargetFunctionRange() {
    g_twinuiPcShell = LoadLibraryExW(
        L"twinui.pcshell.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_twinuiPcShell) {
        Wh_Log(L"Failed to load twinui.pcshell.dll: %lu", GetLastError());
        return false;
    }

    void* targetFunction = nullptr;
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {
                L"protected: long __cdecl VirtualDesktopGestureWindow::MakeBackgroundThumbnailFromThumbnailCache(struct IDCompThumbnail *,struct IVirtualDesktop *,struct HSTRING__ *,struct tagRECT,struct IDCompThumbnail * *)",
            },
            &targetFunction,
            nullptr,
            false,
        },
    };

    if (!WindhawkUtils::HookSymbols(
            g_twinuiPcShell, symbolHooks, ARRAYSIZE(symbolHooks)) ||
        !targetFunction) {
        Wh_Log(L"Failed to resolve the virtual desktop wallpaper helper");
        return false;
    }

    DWORD64 imageBase = 0;
    PRUNTIME_FUNCTION runtimeFunction = RtlLookupFunctionEntry(
        reinterpret_cast<DWORD64>(targetFunction),
        &imageBase,
        nullptr);
    if (!runtimeFunction || !imageBase) {
        Wh_Log(L"Failed to obtain the virtual desktop helper's unwind range");
        return false;
    }

    const uintptr_t functionStart = static_cast<uintptr_t>(
        imageBase + runtimeFunction->BeginAddress);
    const uintptr_t functionEnd = static_cast<uintptr_t>(
        imageBase + runtimeFunction->EndAddress);
    const uintptr_t symbolAddress = reinterpret_cast<uintptr_t>(targetFunction);
    if (symbolAddress < functionStart || symbolAddress >= functionEnd ||
        functionEnd - functionStart > 0x4000) {
        Wh_Log(L"Resolved an invalid virtual desktop helper range");
        return false;
    }

    g_targetFunctionStart = functionStart;
    g_targetFunctionEnd = functionEnd;
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

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();

    if (!IsOrMayBecomeShellExplorer()) {
        Wh_Log(L"Skipping non-shell explorer.exe process %lu",
               GetCurrentProcessId());
        return TRUE;
    }

    if (!ResolveTargetFunctionRange() || !HookThumbnailCache()) {
        ReleaseModuleReferences();
        return FALSE;
    }

    Wh_Log(L"Initialized: maximumThumbnailSize=%ld",
           InterlockedCompareExchange(&g_maximumThumbnailSize, 0, 0));
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    // Windhawk calls this after removing registered hooks, so the implementation
    // DLLs can no longer be entered through this mod's detour.
    ReleaseModuleReferences();
}
