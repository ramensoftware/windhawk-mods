// ==WindhawkMod==
// @id              disable-folder-thumbnails
// @name            Disable Folder Thumbnails
// @description     Disable Explorer folder thumbnails while preserving file thumbnails.
// @version         1.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @compilerOptions -lole32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Folder Thumbnails

Disables thumbnail extraction and retrieval for ordinary filesystem
folders through Explorer's IThumbnailCache interface.

File thumbnail requests are passed through unchanged.

## Usage

Enable the mod, then restart Windows Explorer using Task Manager.

Leave Explorer's "Always show icons, never thumbnails" setting unchecked;
that setting would disable file thumbnails as well.

## Limitations

- Targets Explorer's IThumbnailCache implementation.
- Does not target virtual folders or archive files exposed as folders.
- Folder previews rendered through other shell mechanisms may be unaffected.
- Does not erase the thumbnail cache or modify folder customization.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shobjidl.h>
#include <thumbcache.h>

using GetThumbnail_t = HRESULT(STDMETHODCALLTYPE*)(
    IThumbnailCache* self,
    IShellItem* shellItem,
    UINT requestedSize,
    WTS_FLAGS flags,
    ISharedBitmap** thumbnail,
    WTS_CACHEFLAGS* cacheFlags,
    WTS_THUMBNAILID* thumbnailId
);

static GetThumbnail_t g_originalGetThumbnail = nullptr;

static bool IsFilesystemFolder(IShellItem* item)
{
    if (!item) {
        return false;
    }

    // SFGAO_STREAM distinguishes file-like items, including archives
    // exposed by the shell as browsable folders.
    constexpr SFGAOF mask =
        SFGAO_FOLDER | SFGAO_FILESYSTEM | SFGAO_STREAM;

    SFGAOF attributes = 0;
    HRESULT hr = item->GetAttributes(mask, &attributes);

    if (FAILED(hr)) {
        // If classification fails, preserve the normal behavior.
        return false;
    }

    return (attributes & (SFGAO_FOLDER | SFGAO_FILESYSTEM)) ==
               (SFGAO_FOLDER | SFGAO_FILESYSTEM) &&
           !(attributes & SFGAO_STREAM);
}

static HRESULT STDMETHODCALLTYPE GetThumbnail_Hook(
    IThumbnailCache* self,
    IShellItem* shellItem,
    UINT requestedSize,
    WTS_FLAGS flags,
    ISharedBitmap** thumbnail,
    WTS_CACHEFLAGS* cacheFlags,
    WTS_THUMBNAILID* thumbnailId)
{
    if (IsFilesystemFolder(shellItem)) {
        if (thumbnail) {
            *thumbnail = nullptr;
        }

        if (cacheFlags) {
            *cacheFlags = static_cast<WTS_CACHEFLAGS>(0);
        }

        if (thumbnailId) {
            ZeroMemory(thumbnailId, sizeof(*thumbnailId));
        }

        // Reject both cached and newly generated folder thumbnails.
        // Explorer can then use its normal icon fallback.
        return WTS_E_FAILEDEXTRACTION;
    }

    return g_originalGetThumbnail(
        self,
        shellItem,
        requestedSize,
        flags,
        thumbnail,
        cacheFlags,
        thumbnailId
    );
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Initializing folder-thumbnail suppression");

    HRESULT initHr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    bool mustUninitialize = SUCCEEDED(initHr);

    if (FAILED(initHr) && initHr != RPC_E_CHANGED_MODE) {
        Wh_Log(L"CoInitializeEx failed: 0x%08X",
               static_cast<UINT>(initHr));
        return FALSE;
    }

    // CLSID_LocalThumbnailCache:
    // {50EF4544-AC9F-4A8E-B21B-8A26180DB13F}
    static const CLSID thumbnailCacheClsid = {
        0x50EF4544,
        0xAC9F,
        0x4A8E,
        {0xB2, 0x1B, 0x8A, 0x26, 0x18, 0x0D, 0xB1, 0x3F}
    };

    IThumbnailCache* cache = nullptr;

    HRESULT hr = CoCreateInstance(
        thumbnailCacheClsid,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&cache)
    );

    if (FAILED(hr)) {
        Wh_Log(L"Creating thumbnail cache failed: 0x%08X",
               static_cast<UINT>(hr));

        if (mustUninitialize) {
            CoUninitialize();
        }

        return FALSE;
    }

    // IUnknown occupies slots 0–2.
    // IThumbnailCache::GetThumbnail occupies slot 3.
    void** vtable = *reinterpret_cast<void***>(cache);
    void* target = vtable[3];

    // Keep the implementation module loaded for the process lifetime.
    // Otherwise COM could unload it after releasing our cache instance.
    HMODULE implementationModule = nullptr;

    BOOL pinned = GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_PIN,
        reinterpret_cast<LPCWSTR>(target),
        &implementationModule
    );

    BOOL hooked = FALSE;

    if (pinned) {
        hooked = Wh_SetFunctionHook(
            target,
            reinterpret_cast<void*>(GetThumbnail_Hook),
            reinterpret_cast<void**>(&g_originalGetThumbnail)
        );
    } else {
        Wh_Log(L"Couldn't pin thumbnail-cache module: %lu",
               GetLastError());
    }

    cache->Release();

    if (mustUninitialize) {
        CoUninitialize();
    }


    return hooked;
}
