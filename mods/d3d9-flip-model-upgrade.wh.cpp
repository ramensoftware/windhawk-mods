// ==WindhawkMod==
// @id              d3d9-flip-model-upgrade
// @name            D3D9 Flip Model Upgrade
// @description     Upgrades Direct3D 9 games to D3D9Ex with a FLIPEX swap chain so windowed/borderless games can reach Independent Flip
// @version         1.0
// @author          tria
// @github          https://github.com/triatomic
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# D3D9 Flip Model Upgrade
Direct3D 9 games that run windowed or borderless present with the legacy
*blt model*: every frame is copied and composed by DWM, which adds latency and
blocks VRR. Windows 11's "Optimizations for windowed games" only covers
DirectX 10/11, so D3D9 games are left behind.

This mod transparently creates the game's device as a **Direct3D 9Ex** device
with a **`D3DSWAPEFFECT_FLIPEX`** swap chain (the D3D9 flip model). Once the
game window covers the whole screen (borderless fullscreen), DWM can promote it
to **Independent Flip**, the same presentation path exclusive fullscreen uses.

## How to enable the mod for a program
This mod doesn't target any process by default. In Windhawk, go to the mod's
"Advanced" tab and scroll down to "Custom process inclusion list". In that box,
put the filename of the game's `.exe`, then click "Save" and (re)start the game.

## How to verify
Use [PresentMon](https://github.com/GameTechDev/PresentMon) (or an overlay that
shows the presentation mode, such as Special K or RTSS). A windowed D3D9 game
normally shows `Composed: Copy with GPU GDI`. With the mod it should show
`Hardware: Independent Flip` or `Hardware Composed: Independent Flip`.

Independent Flip requires the game window to cover the entire monitor with
nothing drawn on top of it. Run the game in its borderless mode, or turn on
**Force borderless fullscreen** in the mod settings.

## Exclusive fullscreen
Games running in exclusive fullscreen are left untouched by default: Windows'
Fullscreen Optimizations already give them flip presentation. Turn on **Force
borderless fullscreen** to convert them to an upgraded borderless window
instead.

## Known limitations
- Back buffer multisampling (MSAA requested directly on the swap chain) isn't
  supported by the flip model and is dropped. Use the **Keep MSAA** setting to
  skip the upgrade for such games instead.
- Games that lock the back buffer (some screenshot/readback code) lose that
  ability.
- Anything drawn into the game window with GDI disappears; the flip model only
  shows the Direct3D content.
- Games relying on `D3DSWAPEFFECT_COPY` keeping the previous frame's content
  may flicker.
- `D3DPOOL_MANAGED` doesn't exist in D3D9Ex, so managed textures are emulated
  with a system memory texture plus a default pool copy, and managed buffers
  are created in the default pool. The Ex device is also hidden from D3DX
  (`d3dx9_*.dll`), which otherwise refuses to load managed textures. A few
  games may misbehave or fail to reset their device after a resolution change.
- D3D9 replacements that don't use the system `d3d9.dll` (DXVK, d3d9on12
  wrappers) bypass this mod entirely.
- Disabling the mod while a game is running isn't supported; restart the game.
- No guarantees of anti-cheat compatibility.

Enable the mod's logging (Advanced → Debug logging) to see exactly what was
changed and any failure codes.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- backBufferCount: 2
  $name: Back buffer count
  $description: >-
    Minimum number of back buffers for the flip model swap chain (2-8). Microsoft
    recommends at least 2.
- vsync: unchanged
  $name: VSync
  $options:
  - unchanged: Game decides
  - on: Force on
  - off: Force off
- forceBorderless: false
  $name: Force borderless fullscreen
  $description: >-
    Turn both windowed and exclusive fullscreen requests into a borderless window
    covering the monitor. The game's render resolution is kept and stretched.
- maxFrameLatency: 0
  $name: Maximum frame latency
  $description: >-
    Number of frames the driver may queue (1-20). 0 leaves the default (3).
- flipModel: true
  $name: Use the flip model
  $description: >-
    For troubleshooting. Turn off to only upgrade to Direct3D 9Ex and keep the
    game's own swap effect.
- keepMsaa: false
  $name: Keep MSAA
  $description: >-
    If the game requests a multisampled back buffer, skip the upgrade instead of
    dropping the multisampling.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <d3d9.h>

#include <algorithm>
#include <atomic>
#include <mutex>
#include <string>
#include <vector>

#ifndef D3DPRESENT_BACK_BUFFERS_MAX_EX
#define D3DPRESENT_BACK_BUFFERS_MAX_EX 30
#endif

#ifndef D3DPRESENTFLAG_VIDEO
#define D3DPRESENTFLAG_VIDEO 0x00000010
#endif

// Own copies of the IIDs so that no dxguid import library is needed.
static const GUID kIID_IDirect3D9Ex = {
    0x02177241,
    0x69FC,
    0x400C,
    {0x8F, 0xF1, 0x93, 0xA4, 0x4D, 0xF6, 0x86, 0x1D}};
static const GUID kIID_IDirect3DDevice9Ex = {
    0xB18B10CE,
    0x2649,
    0x405A,
    {0x87, 0x0F, 0x95, 0xF7, 0x77, 0xD4, 0x31, 0x3A}};

// Vtable slots (d3d9.h declaration order, IUnknown = 0..2).
//   IDirect3D9:          16 CreateDevice
//   IDirect3D9Ex:        20 CreateDeviceEx
//   IDirect3DDevice9:    0 QueryInterface, 16 Reset, 17 Present, 23 CreateTexture,
//                        24 CreateVolumeTexture, 25 CreateCubeTexture,
//                        26 CreateVertexBuffer, 27 CreateIndexBuffer,
//                        30 UpdateSurface, 31 UpdateTexture, 65 SetTexture,
//                        81 DrawPrimitive, 82 DrawIndexedPrimitive,
//                        83 DrawPrimitiveUP, 84 DrawIndexedPrimitiveUP
//   IDirect3DDevice9Ex:  121 PresentEx, 132 ResetEx
//   IDirect3DSwapChain9: 3 Present
enum {
    kSlotCreateDevice = 16,
    kSlotCreateDeviceEx = 20,
    kSlotDeviceQueryInterface = 0,
    kSlotReset = 16,
    kSlotPresent = 17,
    kSlotSetTexture = 65,
    kSlotDrawPrimitive = 81,
    kSlotDrawIndexedPrimitive = 82,
    kSlotDrawPrimitiveUP = 83,
    kSlotDrawIndexedPrimitiveUP = 84,
    kSlotCreateTexture = 23,
    kSlotCreateVolumeTexture = 24,
    kSlotCreateCubeTexture = 25,
    kSlotCreateVertexBuffer = 26,
    kSlotCreateIndexBuffer = 27,
    kSlotUpdateSurface = 30,
    kSlotUpdateTexture = 31,
    kSlotPresentEx = 121,
    kSlotResetEx = 132,
    kSlotSwapChainPresent = 3,
};

enum class VSyncMode { unchanged, on, off };

struct {
    int backBufferCount;
    VSyncMode vsync;
    bool forceBorderless;
    int maxFrameLatency;
    bool keepMsaa;
    bool flipModel;
} g_settings;

std::mutex g_hookMutex;
std::atomic<bool> g_exportsHooked;
bool g_d3d9Hooked;
bool g_deviceHooked;

SRWLOCK g_flipDevicesLock = SRWLOCK_INIT;
std::vector<IDirect3DDevice9*> g_flipDevices;

thread_local bool g_inCreateDevice;
thread_local bool g_inReset;
thread_local bool g_inIsExDevice;

////////////////////////////////////////////////////////////////////////////////
// Helpers

void** GetVtbl(void* obj) {
    return *reinterpret_cast<void***>(obj);
}

void SetFlipDevice(IDirect3DDevice9* dev, bool flip) {
    AcquireSRWLockExclusive(&g_flipDevicesLock);
    auto it = std::find(g_flipDevices.begin(), g_flipDevices.end(), dev);
    if (flip && it == g_flipDevices.end()) {
        g_flipDevices.push_back(dev);
    } else if (!flip && it != g_flipDevices.end()) {
        g_flipDevices.erase(it);
    }
    ReleaseSRWLockExclusive(&g_flipDevicesLock);
}

bool IsFlipDevice(IDirect3DDevice9* dev) {
    AcquireSRWLockShared(&g_flipDevicesLock);
    bool found = std::find(g_flipDevices.begin(), g_flipDevices.end(), dev) !=
                 g_flipDevices.end();
    ReleaseSRWLockShared(&g_flipDevicesLock);
    return found;
}

bool IsExDevice(IDirect3DDevice9* dev) {
    // The QueryInterface hook must not interfere.
    g_inIsExDevice = true;
    IUnknown* ex = nullptr;
    HRESULT hr = dev->QueryInterface(kIID_IDirect3DDevice9Ex, (void**)&ex);
    g_inIsExDevice = false;
    if (FAILED(hr)) {
        return false;
    }
    ex->Release();
    return true;
}

bool ShouldUpgrade(const D3DPRESENT_PARAMETERS* pp) {
    if (!pp) {
        return false;
    }
    if (!pp->Windowed && !g_settings.forceBorderless) {
        return false;
    }
    if (g_settings.keepMsaa && pp->MultiSampleType != D3DMULTISAMPLE_NONE) {
        return false;
    }
    return true;
}

// Returns false if the params are left alone.
bool AdjustPresentParams(D3DPRESENT_PARAMETERS* pp) {
    if (!ShouldUpgrade(pp)) {
        return false;
    }

    Wh_Log(L"Requested: %ux%u fmt=%d count=%u msaa=%d swap=%d windowed=%d "
           L"flags=0x%X interval=0x%X",
           pp->BackBufferWidth, pp->BackBufferHeight, pp->BackBufferFormat,
           pp->BackBufferCount, pp->MultiSampleType, pp->SwapEffect,
           pp->Windowed, pp->Flags, pp->PresentationInterval);

    if (!g_settings.flipModel) {
        Wh_Log(L"Flip model disabled, keeping the params");
        return true;
    }

    if (!pp->Windowed) {
        pp->Windowed = TRUE;
        pp->FullScreen_RefreshRateInHz = 0;
    }

    pp->SwapEffect = D3DSWAPEFFECT_FLIPEX;

    UINT count = pp->BackBufferCount;
    if (count < (UINT)g_settings.backBufferCount) {
        count = g_settings.backBufferCount;
    }
    if (count > D3DPRESENT_BACK_BUFFERS_MAX_EX) {
        count = D3DPRESENT_BACK_BUFFERS_MAX_EX;
    }
    pp->BackBufferCount = count;

    if (pp->MultiSampleType != D3DMULTISAMPLE_NONE) {
        Wh_Log(L"Dropping back buffer MSAA, not supported by FLIPEX");
    }
    pp->MultiSampleType = D3DMULTISAMPLE_NONE;
    pp->MultiSampleQuality = 0;

    pp->Flags &= ~(D3DPRESENTFLAG_LOCKABLE_BACKBUFFER |
                   D3DPRESENTFLAG_DEVICECLIP | D3DPRESENTFLAG_VIDEO);

    switch (pp->BackBufferFormat) {
        case D3DFMT_R5G6B5:
        case D3DFMT_X1R5G5B5:
        case D3DFMT_A1R5G5B5:
            pp->BackBufferFormat = D3DFMT_X8R8G8B8;
            break;
        default:
            break;
    }

    switch (g_settings.vsync) {
        case VSyncMode::on:
            pp->PresentationInterval = D3DPRESENT_INTERVAL_ONE;
            break;
        case VSyncMode::off:
            pp->PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
            break;
        case VSyncMode::unchanged:
            break;
    }

    return true;
}

// Reports the runtime-filled values back to the game while keeping its own
// view of the swap effect and buffer count.
void WriteBackPresentParams(D3DPRESENT_PARAMETERS* gamePp,
                            const D3DPRESENT_PARAMETERS& usedPp) {
    if (!gamePp->BackBufferWidth) {
        gamePp->BackBufferWidth = usedPp.BackBufferWidth;
    }
    if (!gamePp->BackBufferHeight) {
        gamePp->BackBufferHeight = usedPp.BackBufferHeight;
    }
    if (gamePp->BackBufferFormat == D3DFMT_UNKNOWN) {
        gamePp->BackBufferFormat = usedPp.BackBufferFormat;
    }
    if (!gamePp->BackBufferCount) {
        gamePp->BackBufferCount = 1;
    }
}

void MakeBorderless(HWND hWnd) {
    if (!g_settings.forceBorderless || !hWnd || !IsWindow(hWnd)) {
        return;
    }

    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfoW(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST),
                         &mi)) {
        return;
    }

    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    style &= ~(WS_OVERLAPPEDWINDOW | WS_DLGFRAME | WS_BORDER);
    style |= WS_POPUP;
    SetWindowLongPtrW(hWnd, GWL_STYLE, style);

    const RECT& rc = mi.rcMonitor;
    SetWindowPos(hWnd, HWND_TOP, rc.left, rc.top, rc.right - rc.left,
                 rc.bottom - rc.top,
                 SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    Wh_Log(L"Made window %p borderless: %dx%d", hWnd, rc.right - rc.left,
           rc.bottom - rc.top);
}

HWND GetDeviceWindow(IDirect3DDevice9* dev, const D3DPRESENT_PARAMETERS* pp) {
    if (pp->hDeviceWindow) {
        return pp->hDeviceWindow;
    }
    D3DDEVICE_CREATION_PARAMETERS cp = {};
    if (SUCCEEDED(dev->GetCreationParameters(&cp))) {
        return cp.hFocusWindow;
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Device hooks

using Reset_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,
                                            D3DPRESENT_PARAMETERS*);
Reset_t Reset_Original;

using ResetEx_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9Ex*,
                                              D3DPRESENT_PARAMETERS*,
                                              D3DDISPLAYMODEEX*);
ResetEx_t ResetEx_Original;

void ClearBoundTextures();

// mode is only used when useEx is set.
HRESULT ResetCommon(IDirect3DDevice9* dev,
                    D3DPRESENT_PARAMETERS* pp,
                    D3DDISPLAYMODEEX* mode,
                    bool useEx) {
    auto callOriginal = [&](D3DPRESENT_PARAMETERS* p, D3DDISPLAYMODEEX* m) {
        return useEx ? ResetEx_Original((IDirect3DDevice9Ex*)dev, p, m)
                     : Reset_Original(dev, p);
    };

    if (g_inReset || !pp || !IsExDevice(dev)) {
        return callOriginal(pp, mode);
    }

    // Reset unbinds all textures.
    ClearBoundTextures();

    D3DPRESENT_PARAMETERS local = *pp;
    bool adjusted = AdjustPresentParams(&local);

    HRESULT hr = E_FAIL;
    if (adjusted) {
        g_inReset = true;
        hr = callOriginal(&local, nullptr);
        g_inReset = false;
        Wh_Log(L"Reset%s with FLIPEX: 0x%08X", useEx ? L"Ex" : L"", hr);
    }

    if (SUCCEEDED(hr)) {
        SetFlipDevice(dev, true);
        WriteBackPresentParams(pp, local);
        MakeBorderless(GetDeviceWindow(dev, &local));
        return hr;
    }

    g_inReset = true;
    hr = callOriginal(pp, mode);
    g_inReset = false;
    Wh_Log(L"Reset%s with the game's params: 0x%08X", useEx ? L"Ex" : L"", hr);
    if (SUCCEEDED(hr)) {
        SetFlipDevice(dev, false);
    }
    return hr;
}

HRESULT STDMETHODCALLTYPE Reset_Hook(IDirect3DDevice9* dev,
                                     D3DPRESENT_PARAMETERS* pp) {
    return ResetCommon(dev, pp, nullptr, false);
}

HRESULT STDMETHODCALLTYPE ResetEx_Hook(IDirect3DDevice9Ex* dev,
                                       D3DPRESENT_PARAMETERS* pp,
                                       D3DDISPLAYMODEEX* mode) {
    return ResetCommon(dev, pp, mode, true);
}

// FLIPEX requires NULL rects, dirty region and window override.

using Present_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,
                                              const RECT*,
                                              const RECT*,
                                              HWND,
                                              const RGNDATA*);
Present_t Present_Original;
HRESULT STDMETHODCALLTYPE Present_Hook(IDirect3DDevice9* dev,
                                       const RECT* src,
                                       const RECT* dst,
                                       HWND wndOverride,
                                       const RGNDATA* dirty) {
    if (IsFlipDevice(dev)) {
        return Present_Original(dev, nullptr, nullptr, nullptr, nullptr);
    }
    return Present_Original(dev, src, dst, wndOverride, dirty);
}

using PresentEx_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9Ex*,
                                                const RECT*,
                                                const RECT*,
                                                HWND,
                                                const RGNDATA*,
                                                DWORD);
PresentEx_t PresentEx_Original;
HRESULT STDMETHODCALLTYPE PresentEx_Hook(IDirect3DDevice9Ex* dev,
                                         const RECT* src,
                                         const RECT* dst,
                                         HWND wndOverride,
                                         const RGNDATA* dirty,
                                         DWORD flags) {
    if (IsFlipDevice(dev)) {
        return PresentEx_Original(dev, nullptr, nullptr, nullptr, nullptr,
                                  flags);
    }
    return PresentEx_Original(dev, src, dst, wndOverride, dirty, flags);
}

using SwapChainPresent_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DSwapChain9*,
                                                       const RECT*,
                                                       const RECT*,
                                                       HWND,
                                                       const RGNDATA*,
                                                       DWORD);
SwapChainPresent_t SwapChainPresent_Original;
HRESULT STDMETHODCALLTYPE SwapChainPresent_Hook(IDirect3DSwapChain9* swapChain,
                                                const RECT* src,
                                                const RECT* dst,
                                                HWND wndOverride,
                                                const RGNDATA* dirty,
                                                DWORD flags) {
    // Additional swap chains of an upgraded device keep the game's swap
    // effect, so check the swap chain itself.
    D3DPRESENT_PARAMETERS pp;
    if (SUCCEEDED(swapChain->GetPresentParameters(&pp)) &&
        pp.SwapEffect == D3DSWAPEFFECT_FLIPEX) {
        return SwapChainPresent_Original(swapChain, nullptr, nullptr, nullptr,
                                         nullptr, flags);
    }
    return SwapChainPresent_Original(swapChain, src, dst, wndOverride, dirty,
                                     flags);
}

// D3DPOOL_MANAGED isn't valid on an Ex device, so it's emulated: the game gets
// a system memory texture, which it can lock and fill at any time and whose
// memory stays put like the managed copy did, with a default pool twin attached
// as private data. SetTexture uploads the dirty regions to the twin and binds
// it instead. The twin is released together with the game's texture. Buffers
// simply go to the default pool, where they are lockable as is.

// {6F1B0B5E-3C7A-4D0B-9B0D-4A1E6B7F21C3}
static const GUID kShadowTextureGuid = {
    0x6F1B0B5E,
    0x3C7A,
    0x4D0B,
    {0x9B, 0x0D, 0x4A, 0x1E, 0x6B, 0x7F, 0x21, 0xC3}};

bool ConvertManagedPool(IDirect3DDevice9* dev, D3DPOOL* pool) {
    if (*pool != D3DPOOL_MANAGED || !IsExDevice(dev)) {
        return false;
    }
    *pool = D3DPOOL_DEFAULT;
    return true;
}

// create(usage, pool, levels, &texture) calls the original creation function.
template <typename T, typename CreateFn>
HRESULT CreateManagedEmulation(UINT levels,
                               DWORD usage,
                               T** texture,
                               CreateFn create) {
    T* videoTexture = nullptr;
    HRESULT hr = create(usage, D3DPOOL_DEFAULT, levels, &videoTexture);
    if (FAILED(hr)) {
        Wh_Log(L"Default pool twin failed: 0x%08X (usage=0x%X)", hr, usage);
        return hr;
    }

    // Only the top level of an auto generated mip chain is accessible.
    UINT systemLevels = (usage & D3DUSAGE_AUTOGENMIPMAP) ? 1 : levels;

    T* systemTexture = nullptr;
    hr = create(0, D3DPOOL_SYSTEMMEM, systemLevels, &systemTexture);
    if (SUCCEEDED(hr)) {
        hr = systemTexture->SetPrivateData(
            kShadowTextureGuid, static_cast<IDirect3DBaseTexture9*>(videoTexture),
            sizeof(IUnknown*), D3DSPD_IUNKNOWN);
        if (FAILED(hr)) {
            systemTexture->Release();
        }
    }

    // Now owned by the system memory texture, if all went well.
    videoTexture->Release();

    if (FAILED(hr)) {
        Wh_Log(L"System memory texture failed: 0x%08X", hr);
        return hr;
    }

    *texture = systemTexture;
    return hr;
}

// {85C31227-3DE5-4F00-9B3A-F11AC38C18B5}
static const GUID kIID_IDirect3DTexture9 = {
    0x85C31227,
    0x3DE5,
    0x4F00,
    {0x9B, 0x3A, 0xF1, 0x1A, 0xC3, 0x8C, 0x18, 0xB5}};

// Returns the AddRef'd default pool twin of an emulated managed texture.
IDirect3DBaseTexture9* GetVideoTexture(IDirect3DResource9* texture) {
    IDirect3DBaseTexture9* videoTexture = nullptr;
    DWORD size = sizeof(videoTexture);
    if (FAILED(texture->GetPrivateData(kShadowTextureGuid, &videoTexture,
                                       &size))) {
        return nullptr;
    }
    return videoTexture;
}

// Some games upload to their managed textures with UpdateTexture and
// UpdateSurface. The emulated texture is in system memory, which isn't a valid
// destination, so the twin receives the data instead.

using UpdateTexture_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,
                                                    IDirect3DBaseTexture9*,
                                                    IDirect3DBaseTexture9*);
UpdateTexture_t UpdateTexture_Original;
HRESULT STDMETHODCALLTYPE UpdateTexture_Hook(IDirect3DDevice9* dev,
                                             IDirect3DBaseTexture9* source,
                                             IDirect3DBaseTexture9* dest) {
    IDirect3DBaseTexture9* videoTexture = dest ? GetVideoTexture(dest) : nullptr;
    HRESULT hr =
        UpdateTexture_Original(dev, source, videoTexture ? videoTexture : dest);

    if (videoTexture) {
        videoTexture->Release();
    }
    return hr;
}

using UpdateSurface_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,
                                                    IDirect3DSurface9*,
                                                    const RECT*,
                                                    IDirect3DSurface9*,
                                                    const POINT*);
UpdateSurface_t UpdateSurface_Original;
HRESULT STDMETHODCALLTYPE UpdateSurface_Hook(IDirect3DDevice9* dev,
                                             IDirect3DSurface9* source,
                                             const RECT* sourceRect,
                                             IDirect3DSurface9* dest,
                                             const POINT* destPoint) {
    IDirect3DSurface9* videoSurface = nullptr;

    IDirect3DTexture9* container = nullptr;
    if (dest && SUCCEEDED(dest->GetContainer(kIID_IDirect3DTexture9,
                                             (void**)&container))) {
        if (IDirect3DBaseTexture9* videoTexture = GetVideoTexture(container)) {
            DWORD levelCount = container->GetLevelCount();
            for (DWORD level = 0; level < levelCount && !videoSurface;
                 level++) {
                IDirect3DSurface9* surface = nullptr;
                if (SUCCEEDED(container->GetSurfaceLevel(level, &surface))) {
                    if (surface == dest) {
                        ((IDirect3DTexture9*)videoTexture)
                            ->GetSurfaceLevel(level, &videoSurface);
                    }
                    surface->Release();
                }
            }
            videoTexture->Release();
        }
        container->Release();
    }

    HRESULT hr = UpdateSurface_Original(dev, source, sourceRect,
                                        videoSurface ? videoSurface : dest,
                                        destPoint);

    if (videoSurface) {
        videoSurface->Release();
    }
    return hr;
}

// Emulated textures currently bound, AddRef'd. A managed texture is uploaded
// when it's used for drawing, so binding time is too early: the game may fill
// the texture after binding it, or keep it bound while refilling it.
struct BoundTexture {
    IDirect3DBaseTexture9* systemTexture;
    IDirect3DBaseTexture9* videoTexture;
};
constexpr int kBoundTextureCount = 16 + 5;
BoundTexture g_boundTextures[kBoundTextureCount];
std::atomic<int> g_boundTextureCount;

int BoundTextureIndex(DWORD stage) {
    if (stage < 16) {
        return stage;
    }
    // D3DDMAPSAMPLER, D3DVERTEXTEXTURESAMPLER0..3.
    if (stage >= 256 && stage <= 260) {
        return 16 + (stage - 256);
    }
    return -1;
}

// Takes over the videoTexture reference.
void SetBoundTexture(DWORD stage,
                     IDirect3DBaseTexture9* systemTexture,
                     IDirect3DBaseTexture9* videoTexture) {
    int index = BoundTextureIndex(stage);
    if (index < 0) {
        if (videoTexture) {
            videoTexture->Release();
        }
        return;
    }

    BoundTexture& bound = g_boundTextures[index];
    if (bound.systemTexture) {
        bound.systemTexture->Release();
        bound.videoTexture->Release();
        g_boundTextureCount--;
    }

    bound.systemTexture = systemTexture;
    bound.videoTexture = videoTexture;
    if (systemTexture) {
        systemTexture->AddRef();
        g_boundTextureCount++;
    }
}

void ClearBoundTextures() {
    for (int i = 0; i < kBoundTextureCount; i++) {
        BoundTexture& bound = g_boundTextures[i];
        if (bound.systemTexture) {
            bound.systemTexture->Release();
            bound.videoTexture->Release();
            bound = {};
            g_boundTextureCount--;
        }
    }
}

using SetTexture_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,
                                                 DWORD,
                                                 IDirect3DBaseTexture9*);
SetTexture_t SetTexture_Original;
HRESULT STDMETHODCALLTYPE SetTexture_Hook(IDirect3DDevice9* dev,
                                          DWORD stage,
                                          IDirect3DBaseTexture9* texture) {
    IDirect3DBaseTexture9* videoTexture =
        texture ? GetVideoTexture(texture) : nullptr;
    if (!videoTexture) {
        SetBoundTexture(stage, nullptr, nullptr);
        return SetTexture_Original(dev, stage, texture);
    }

    HRESULT hr = SetTexture_Original(dev, stage, videoTexture);
    SetBoundTexture(stage, texture, videoTexture);
    return hr;
}

void UploadBoundTextures(IDirect3DDevice9* dev) {
    if (!g_boundTextureCount) {
        return;
    }

    for (int i = 0; i < kBoundTextureCount; i++) {
        BoundTexture& bound = g_boundTextures[i];
        if (!bound.systemTexture) {
            continue;
        }

        // Copies the dirty regions only, fails while the game holds a lock.
        HRESULT hr = UpdateTexture_Original(dev, bound.systemTexture,
                                            bound.videoTexture);
        if (FAILED(hr)) {
            static std::atomic<int> logged;
            if (logged < 20) {
                logged++;
                Wh_Log(L"Upload failed: 0x%08X", hr);
            }
        }
    }
}

using DrawPrimitive_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,
                                                    D3DPRIMITIVETYPE,
                                                    UINT,
                                                    UINT);
DrawPrimitive_t DrawPrimitive_Original;
HRESULT STDMETHODCALLTYPE DrawPrimitive_Hook(IDirect3DDevice9* dev,
                                             D3DPRIMITIVETYPE type,
                                             UINT startVertex,
                                             UINT primitiveCount) {
    UploadBoundTextures(dev);
    return DrawPrimitive_Original(dev, type, startVertex, primitiveCount);
}

using DrawIndexedPrimitive_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,
                                                           D3DPRIMITIVETYPE,
                                                           INT,
                                                           UINT,
                                                           UINT,
                                                           UINT,
                                                           UINT);
DrawIndexedPrimitive_t DrawIndexedPrimitive_Original;
HRESULT STDMETHODCALLTYPE DrawIndexedPrimitive_Hook(IDirect3DDevice9* dev,
                                                    D3DPRIMITIVETYPE type,
                                                    INT baseVertexIndex,
                                                    UINT minVertexIndex,
                                                    UINT numVertices,
                                                    UINT startIndex,
                                                    UINT primitiveCount) {
    UploadBoundTextures(dev);
    return DrawIndexedPrimitive_Original(dev, type, baseVertexIndex,
                                         minVertexIndex, numVertices,
                                         startIndex, primitiveCount);
}

using DrawPrimitiveUP_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,
                                                      D3DPRIMITIVETYPE,
                                                      UINT,
                                                      const void*,
                                                      UINT);
DrawPrimitiveUP_t DrawPrimitiveUP_Original;
HRESULT STDMETHODCALLTYPE DrawPrimitiveUP_Hook(IDirect3DDevice9* dev,
                                               D3DPRIMITIVETYPE type,
                                               UINT primitiveCount,
                                               const void* vertexData,
                                               UINT vertexStride) {
    UploadBoundTextures(dev);
    return DrawPrimitiveUP_Original(dev, type, primitiveCount, vertexData,
                                    vertexStride);
}

using DrawIndexedPrimitiveUP_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,
                                                             D3DPRIMITIVETYPE,
                                                             UINT,
                                                             UINT,
                                                             UINT,
                                                             const void*,
                                                             D3DFORMAT,
                                                             const void*,
                                                             UINT);
DrawIndexedPrimitiveUP_t DrawIndexedPrimitiveUP_Original;
HRESULT STDMETHODCALLTYPE
DrawIndexedPrimitiveUP_Hook(IDirect3DDevice9* dev,
                            D3DPRIMITIVETYPE type,
                            UINT minVertexIndex,
                            UINT numVertices,
                            UINT primitiveCount,
                            const void* indexData,
                            D3DFORMAT indexDataFormat,
                            const void* vertexData,
                            UINT vertexStride) {
    UploadBoundTextures(dev);
    return DrawIndexedPrimitiveUP_Original(dev, type, minVertexIndex,
                                           numVertices, primitiveCount,
                                           indexData, indexDataFormat,
                                           vertexData, vertexStride);
}

using CreateTexture_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,
                                                    UINT,
                                                    UINT,
                                                    UINT,
                                                    DWORD,
                                                    D3DFORMAT,
                                                    D3DPOOL,
                                                    IDirect3DTexture9**,
                                                    HANDLE*);
CreateTexture_t CreateTexture_Original;
HRESULT STDMETHODCALLTYPE CreateTexture_Hook(IDirect3DDevice9* dev,
                                             UINT width,
                                             UINT height,
                                             UINT levels,
                                             DWORD usage,
                                             D3DFORMAT format,
                                             D3DPOOL pool,
                                             IDirect3DTexture9** texture,
                                             HANDLE* sharedHandle) {
    if (!texture || !ConvertManagedPool(dev, &pool)) {
        return CreateTexture_Original(dev, width, height, levels, usage, format,
                                      pool, texture, sharedHandle);
    }

    return CreateManagedEmulation(
        levels, usage, texture,
        [&](DWORD usage, D3DPOOL pool, UINT levels, IDirect3DTexture9** out) {
            return CreateTexture_Original(dev, width, height, levels, usage,
                                          format, pool, out, nullptr);
        });
}

using CreateVolumeTexture_t =
    HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,
                                UINT,
                                UINT,
                                UINT,
                                UINT,
                                DWORD,
                                D3DFORMAT,
                                D3DPOOL,
                                IDirect3DVolumeTexture9**,
                                HANDLE*);
CreateVolumeTexture_t CreateVolumeTexture_Original;
HRESULT STDMETHODCALLTYPE
CreateVolumeTexture_Hook(IDirect3DDevice9* dev,
                         UINT width,
                         UINT height,
                         UINT depth,
                         UINT levels,
                         DWORD usage,
                         D3DFORMAT format,
                         D3DPOOL pool,
                         IDirect3DVolumeTexture9** texture,
                         HANDLE* sharedHandle) {
    if (!texture || !ConvertManagedPool(dev, &pool)) {
        return CreateVolumeTexture_Original(dev, width, height, depth, levels,
                                            usage, format, pool, texture,
                                            sharedHandle);
    }

    return CreateManagedEmulation(
        levels, usage, texture,
        [&](DWORD usage, D3DPOOL pool, UINT levels,
            IDirect3DVolumeTexture9** out) {
            return CreateVolumeTexture_Original(dev, width, height, depth,
                                                levels, usage, format, pool,
                                                out, nullptr);
        });
}

using CreateCubeTexture_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,
                                                        UINT,
                                                        UINT,
                                                        DWORD,
                                                        D3DFORMAT,
                                                        D3DPOOL,
                                                        IDirect3DCubeTexture9**,
                                                        HANDLE*);
CreateCubeTexture_t CreateCubeTexture_Original;
HRESULT STDMETHODCALLTYPE CreateCubeTexture_Hook(IDirect3DDevice9* dev,
                                                 UINT edgeLength,
                                                 UINT levels,
                                                 DWORD usage,
                                                 D3DFORMAT format,
                                                 D3DPOOL pool,
                                                 IDirect3DCubeTexture9** texture,
                                                 HANDLE* sharedHandle) {
    if (!texture || !ConvertManagedPool(dev, &pool)) {
        return CreateCubeTexture_Original(dev, edgeLength, levels, usage,
                                          format, pool, texture, sharedHandle);
    }

    return CreateManagedEmulation(
        levels, usage, texture,
        [&](DWORD usage, D3DPOOL pool, UINT levels,
            IDirect3DCubeTexture9** out) {
            return CreateCubeTexture_Original(dev, edgeLength, levels, usage,
                                              format, pool, out, nullptr);
        });
}

using CreateVertexBuffer_t =
    HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,
                                UINT,
                                DWORD,
                                DWORD,
                                D3DPOOL,
                                IDirect3DVertexBuffer9**,
                                HANDLE*);
CreateVertexBuffer_t CreateVertexBuffer_Original;
HRESULT STDMETHODCALLTYPE CreateVertexBuffer_Hook(IDirect3DDevice9* dev,
                                                  UINT length,
                                                  DWORD usage,
                                                  DWORD fvf,
                                                  D3DPOOL pool,
                                                  IDirect3DVertexBuffer9** vb,
                                                  HANDLE* sharedHandle) {
    ConvertManagedPool(dev, &pool);
    return CreateVertexBuffer_Original(dev, length, usage, fvf, pool, vb,
                                       sharedHandle);
}

using CreateIndexBuffer_t =
    HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,
                                UINT,
                                DWORD,
                                D3DFORMAT,
                                D3DPOOL,
                                IDirect3DIndexBuffer9**,
                                HANDLE*);
CreateIndexBuffer_t CreateIndexBuffer_Original;
HRESULT STDMETHODCALLTYPE CreateIndexBuffer_Hook(IDirect3DDevice9* dev,
                                                 UINT length,
                                                 DWORD usage,
                                                 D3DFORMAT format,
                                                 D3DPOOL pool,
                                                 IDirect3DIndexBuffer9** ib,
                                                 HANDLE* sharedHandle) {
    ConvertManagedPool(dev, &pool);
    return CreateIndexBuffer_Original(dev, length, usage, format, pool, ib,
                                      sharedHandle);
}

// D3DX refuses D3DPOOL_MANAGED if the device turns out to be an Ex device, so
// the game's texture loading silently fails. The Ex interface is hidden from
// D3DX, which makes it use the regular code path that ends in the hooks here.

using DeviceQueryInterface_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*,
                                                           REFIID,
                                                           void**);
DeviceQueryInterface_t DeviceQueryInterface_Original;

bool IsD3DXAddress(void* address, PCWSTR* moduleNameOut) {
    static thread_local WCHAR path[MAX_PATH];
    *moduleNameOut = L"?";

    HMODULE module;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCWSTR)address, &module) ||
        !GetModuleFileNameW(module, path, ARRAYSIZE(path))) {
        return false;
    }

    PCWSTR name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;
    *moduleNameOut = name;
    return _wcsnicmp(name, L"d3dx9", 5) == 0;
}

HRESULT STDMETHODCALLTYPE DeviceQueryInterface_Hook(IUnknown* self,
                                                    REFIID riid,
                                                    void** object) {
    if (!g_inIsExDevice && object &&
        IsEqualGUID(riid, kIID_IDirect3DDevice9Ex)) {
        PCWSTR moduleName;
        bool isD3DX = IsD3DXAddress(__builtin_return_address(0), &moduleName);

        static std::atomic<int> logged;
        if (logged < 30) {
            logged++;
            Wh_Log(L"QueryInterface(IDirect3DDevice9Ex) from %s%s", moduleName,
                   isD3DX ? L": hidden" : L"");
        }

        if (isD3DX) {
            *object = nullptr;
            return E_NOINTERFACE;
        }
    }

    return DeviceQueryInterface_Original(self, riid, object);
}

void EnsureDeviceHooks(IDirect3DDevice9Ex* dev) {
    std::lock_guard<std::mutex> guard(g_hookMutex);
    if (g_deviceHooked) {
        return;
    }
    g_deviceHooked = true;

    void** vtbl = GetVtbl(dev);
    Wh_SetFunctionHook(vtbl[kSlotDeviceQueryInterface],
                       (void*)DeviceQueryInterface_Hook,
                       (void**)&DeviceQueryInterface_Original);
    Wh_SetFunctionHook(vtbl[kSlotReset], (void*)Reset_Hook,
                       (void**)&Reset_Original);
    Wh_SetFunctionHook(vtbl[kSlotPresent], (void*)Present_Hook,
                       (void**)&Present_Original);
    Wh_SetFunctionHook(vtbl[kSlotUpdateSurface], (void*)UpdateSurface_Hook,
                       (void**)&UpdateSurface_Original);
    Wh_SetFunctionHook(vtbl[kSlotUpdateTexture], (void*)UpdateTexture_Hook,
                       (void**)&UpdateTexture_Original);
    Wh_SetFunctionHook(vtbl[kSlotSetTexture], (void*)SetTexture_Hook,
                       (void**)&SetTexture_Original);
    Wh_SetFunctionHook(vtbl[kSlotDrawPrimitive], (void*)DrawPrimitive_Hook,
                       (void**)&DrawPrimitive_Original);
    Wh_SetFunctionHook(vtbl[kSlotDrawIndexedPrimitive],
                       (void*)DrawIndexedPrimitive_Hook,
                       (void**)&DrawIndexedPrimitive_Original);
    Wh_SetFunctionHook(vtbl[kSlotDrawPrimitiveUP], (void*)DrawPrimitiveUP_Hook,
                       (void**)&DrawPrimitiveUP_Original);
    Wh_SetFunctionHook(vtbl[kSlotDrawIndexedPrimitiveUP],
                       (void*)DrawIndexedPrimitiveUP_Hook,
                       (void**)&DrawIndexedPrimitiveUP_Original);
    Wh_SetFunctionHook(vtbl[kSlotCreateTexture], (void*)CreateTexture_Hook,
                       (void**)&CreateTexture_Original);
    Wh_SetFunctionHook(vtbl[kSlotCreateVolumeTexture],
                       (void*)CreateVolumeTexture_Hook,
                       (void**)&CreateVolumeTexture_Original);
    Wh_SetFunctionHook(vtbl[kSlotCreateCubeTexture],
                       (void*)CreateCubeTexture_Hook,
                       (void**)&CreateCubeTexture_Original);
    Wh_SetFunctionHook(vtbl[kSlotCreateVertexBuffer],
                       (void*)CreateVertexBuffer_Hook,
                       (void**)&CreateVertexBuffer_Original);
    Wh_SetFunctionHook(vtbl[kSlotCreateIndexBuffer],
                       (void*)CreateIndexBuffer_Hook,
                       (void**)&CreateIndexBuffer_Original);
    Wh_SetFunctionHook(vtbl[kSlotPresentEx], (void*)PresentEx_Hook,
                       (void**)&PresentEx_Original);
    Wh_SetFunctionHook(vtbl[kSlotResetEx], (void*)ResetEx_Hook,
                       (void**)&ResetEx_Original);

    IDirect3DSwapChain9* swapChain = nullptr;
    if (SUCCEEDED(dev->GetSwapChain(0, &swapChain))) {
        Wh_SetFunctionHook(GetVtbl(swapChain)[kSlotSwapChainPresent],
                           (void*)SwapChainPresent_Hook,
                           (void**)&SwapChainPresent_Original);
        swapChain->Release();
    }

    Wh_ApplyHookOperations();
    Wh_Log(L"Device hooks installed");
}

////////////////////////////////////////////////////////////////////////////////
// IDirect3D9 hooks

using Direct3DCreate9_t = IDirect3D9*(WINAPI*)(UINT);
Direct3DCreate9_t Direct3DCreate9_Original;

using Direct3DCreate9Ex_t = HRESULT(WINAPI*)(UINT, IDirect3D9Ex**);
Direct3DCreate9Ex_t Direct3DCreate9Ex_Original;

using CreateDeviceEx_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3D9Ex*,
                                                     UINT,
                                                     D3DDEVTYPE,
                                                     HWND,
                                                     DWORD,
                                                     D3DPRESENT_PARAMETERS*,
                                                     D3DDISPLAYMODEEX*,
                                                     IDirect3DDevice9Ex**);
CreateDeviceEx_t CreateDeviceEx_Original;
HRESULT STDMETHODCALLTYPE CreateDeviceEx_Hook(IDirect3D9Ex* d3d,
                                              UINT adapter,
                                              D3DDEVTYPE deviceType,
                                              HWND focusWindow,
                                              DWORD behaviorFlags,
                                              D3DPRESENT_PARAMETERS* pp,
                                              D3DDISPLAYMODEEX* mode,
                                              IDirect3DDevice9Ex** device) {
    bool nested = g_inCreateDevice;
    g_inCreateDevice = true;

    D3DPRESENT_PARAMETERS local;
    bool adjusted = false;
    if (pp && device && !(behaviorFlags & D3DCREATE_ADAPTERGROUP_DEVICE)) {
        local = *pp;
        adjusted = AdjustPresentParams(&local);
    }

    HRESULT hr = E_FAIL;
    if (adjusted) {
        hr = CreateDeviceEx_Original(d3d, adapter, deviceType, focusWindow,
                                     behaviorFlags, &local, nullptr, device);
        Wh_Log(L"CreateDeviceEx with FLIPEX: 0x%08X", hr);
    }

    if (SUCCEEDED(hr)) {
        EnsureDeviceHooks(*device);
        SetFlipDevice(*device, true);
        WriteBackPresentParams(pp, local);

        if (g_settings.maxFrameLatency > 0) {
            HRESULT hrLatency =
                (*device)->SetMaximumFrameLatency(g_settings.maxFrameLatency);
            Wh_Log(L"SetMaximumFrameLatency(%d): 0x%08X",
                   g_settings.maxFrameLatency, hrLatency);
        }

        MakeBorderless(local.hDeviceWindow ? local.hDeviceWindow : focusWindow);
    } else {
        hr = CreateDeviceEx_Original(d3d, adapter, deviceType, focusWindow,
                                     behaviorFlags, pp, mode, device);
        Wh_Log(L"CreateDeviceEx with the game's params: 0x%08X", hr);
        if (SUCCEEDED(hr) && device && *device) {
            // Still an Ex device, the managed pool conversion is needed.
            EnsureDeviceHooks(*device);
            SetFlipDevice(*device, false);
        }
    }

    g_inCreateDevice = nested;
    return hr;
}

using CreateDevice_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3D9*,
                                                   UINT,
                                                   D3DDEVTYPE,
                                                   HWND,
                                                   DWORD,
                                                   D3DPRESENT_PARAMETERS*,
                                                   IDirect3DDevice9**);

// The Ex and non-Ex objects may use different implementations, N selects the
// matching original.
CreateDevice_t CreateDevice_Original[2];

template <int N>
HRESULT STDMETHODCALLTYPE CreateDevice_Hook(IDirect3D9* d3d,
                                            UINT adapter,
                                            D3DDEVTYPE deviceType,
                                            HWND focusWindow,
                                            DWORD behaviorFlags,
                                            D3DPRESENT_PARAMETERS* pp,
                                            IDirect3DDevice9** device) {
    auto callOriginal = [&]() {
        HRESULT hr = CreateDevice_Original[N](d3d, adapter, deviceType,
                                              focusWindow, behaviorFlags, pp,
                                              device);
        if (SUCCEEDED(hr) && device && *device) {
            // In case a stale pointer of a released device is still listed.
            SetFlipDevice(*device, false);
        }
        return hr;
    };

    if (g_inCreateDevice || !device || !CreateDeviceEx_Original ||
        (behaviorFlags & D3DCREATE_ADAPTERGROUP_DEVICE) ||
        !ShouldUpgrade(pp)) {
        return callOriginal();
    }

    IDirect3D9Ex* d3dEx = nullptr;
    if (FAILED(d3d->QueryInterface(kIID_IDirect3D9Ex, (void**)&d3dEx))) {
        d3dEx = nullptr;
        HRESULT hr = Direct3DCreate9Ex_Original(D3D_SDK_VERSION, &d3dEx);
        if (FAILED(hr) || !d3dEx) {
            Wh_Log(L"Direct3DCreate9Ex failed: 0x%08X", hr);
            return callOriginal();
        }
    }

    // Goes through CreateDeviceEx_Hook which adjusts the params.
    IDirect3DDevice9Ex* deviceEx = nullptr;
    HRESULT hr = d3dEx->CreateDeviceEx(adapter, deviceType, focusWindow,
                                       behaviorFlags, pp, nullptr, &deviceEx);
    d3dEx->Release();

    if (FAILED(hr) || !deviceEx) {
        Wh_Log(L"Upgrade failed (0x%08X), creating a regular device", hr);
        return callOriginal();
    }

    if (!IsFlipDevice(deviceEx)) {
        // FLIPEX was refused, no reason to keep the Ex device.
        Wh_Log(L"FLIPEX refused, creating a regular device");
        deviceEx->Release();
        return callOriginal();
    }

    Wh_Log(L"CreateDevice upgraded to D3D9Ex + FLIPEX: %p", deviceEx);
    *device = deviceEx;
    return hr;
}

// Must be called with g_hookMutex held.
void HookD3D9Vtbl(void** vtbl, bool isEx) {
    void* createDevice = vtbl[kSlotCreateDevice];

    static void* hookedCreateDevice[2];
    if (createDevice != hookedCreateDevice[0] &&
        createDevice != hookedCreateDevice[1]) {
        if (!hookedCreateDevice[0]) {
            hookedCreateDevice[0] = createDevice;
            CreateDevice_t hook = CreateDevice_Hook<0>;
            Wh_SetFunctionHook(createDevice, (void*)hook,
                               (void**)&CreateDevice_Original[0]);
        } else if (!hookedCreateDevice[1]) {
            hookedCreateDevice[1] = createDevice;
            CreateDevice_t hook = CreateDevice_Hook<1>;
            Wh_SetFunctionHook(createDevice, (void*)hook,
                               (void**)&CreateDevice_Original[1]);
        }
    }

    if (isEx && !CreateDeviceEx_Original) {
        Wh_SetFunctionHook(vtbl[kSlotCreateDeviceEx],
                           (void*)CreateDeviceEx_Hook,
                           (void**)&CreateDeviceEx_Original);
    }
}

void EnsureD3D9Hooks(IDirect3D9* d3d, bool isEx) {
    std::lock_guard<std::mutex> guard(g_hookMutex);
    if (g_d3d9Hooked) {
        return;
    }
    g_d3d9Hooked = true;

    HookD3D9Vtbl(GetVtbl(d3d), isEx);

    if (!isEx) {
        // A throwaway Ex object for the CreateDeviceEx address.
        IDirect3D9Ex* d3dEx = nullptr;
        HRESULT hr = Direct3DCreate9Ex_Original(D3D_SDK_VERSION, &d3dEx);
        if (SUCCEEDED(hr) && d3dEx) {
            HookD3D9Vtbl(GetVtbl(d3dEx), true);
            d3dEx->Release();
        } else {
            Wh_Log(L"Direct3DCreate9Ex failed (0x%08X), can't upgrade", hr);
        }
    }

    Wh_ApplyHookOperations();
    Wh_Log(L"IDirect3D9 hooks installed");
}

IDirect3D9* WINAPI Direct3DCreate9_Hook(UINT sdkVersion) {
    IDirect3D9* d3d = Direct3DCreate9_Original(sdkVersion);
    Wh_Log(L"Direct3DCreate9(%u): %p", sdkVersion, d3d);
    if (d3d) {
        EnsureD3D9Hooks(d3d, false);
    }
    return d3d;
}

HRESULT WINAPI Direct3DCreate9Ex_Hook(UINT sdkVersion, IDirect3D9Ex** d3d) {
    HRESULT hr = Direct3DCreate9Ex_Original(sdkVersion, d3d);
    Wh_Log(L"Direct3DCreate9Ex(%u): 0x%08X", sdkVersion, hr);
    if (SUCCEEDED(hr) && d3d && *d3d) {
        EnsureD3D9Hooks(*d3d, true);
    }
    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// d3d9.dll load handling

// Only the system d3d9.dll is hooked. It's never loaded by the mod, as that
// would make the loader skip a d3d9.dll proxy in the game's folder.
HMODULE GetSystemD3D9Module() {
    WCHAR dir[MAX_PATH];

    UINT len = GetSystemDirectoryW(dir, ARRAYSIZE(dir));
    if (len && len < ARRAYSIZE(dir)) {
        std::wstring path = std::wstring(dir) + L"\\d3d9.dll";
        if (HMODULE module = GetModuleHandleW(path.c_str())) {
            return module;
        }
    }

    // 32-bit processes may have it recorded under SysWOW64.
    len = GetSystemWow64DirectoryW(dir, ARRAYSIZE(dir));
    if (len && len < ARRAYSIZE(dir)) {
        std::wstring path = std::wstring(dir) + L"\\d3d9.dll";
        if (HMODULE module = GetModuleHandleW(path.c_str())) {
            return module;
        }
    }

    return nullptr;
}

// Returns true if new hooks were set and need to be applied.
bool HookD3D9ExportsIfLoaded() {
    std::lock_guard<std::mutex> guard(g_hookMutex);
    if (g_exportsHooked) {
        return false;
    }

    HMODULE module = GetSystemD3D9Module();
    if (!module) {
        return false;
    }

    auto create9 =
        (Direct3DCreate9_t)GetProcAddress(module, "Direct3DCreate9");
    auto create9Ex =
        (Direct3DCreate9Ex_t)GetProcAddress(module, "Direct3DCreate9Ex");
    if (!create9 || !create9Ex) {
        Wh_Log(L"d3d9.dll exports not found");
        g_exportsHooked = true;
        return false;
    }

    // The hooked code must stay around.
    HMODULE pinned;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        (LPCWSTR)create9, &pinned);

    g_exportsHooked = true;
    WindhawkUtils::SetFunctionHook(create9, Direct3DCreate9_Hook,
                                       &Direct3DCreate9_Original);
    WindhawkUtils::SetFunctionHook(create9Ex, Direct3DCreate9Ex_Hook,
                                       &Direct3DCreate9Ex_Original);
    Wh_Log(L"Hooked d3d9.dll exports (%p)", module);
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !g_exportsHooked && HookD3D9ExportsIfLoaded()) {
        Wh_ApplyHookOperations();
    }
    return module;
}

////////////////////////////////////////////////////////////////////////////////

void LoadSettings() {
    int count = Wh_GetIntSetting(L"backBufferCount");
    g_settings.backBufferCount = count < 2 ? 2 : count > 8 ? 8 : count;

    PCWSTR vsync = Wh_GetStringSetting(L"vsync");
    g_settings.vsync = VSyncMode::unchanged;
    if (wcscmp(vsync, L"on") == 0) {
        g_settings.vsync = VSyncMode::on;
    } else if (wcscmp(vsync, L"off") == 0) {
        g_settings.vsync = VSyncMode::off;
    }
    Wh_FreeStringSetting(vsync);

    g_settings.forceBorderless = Wh_GetIntSetting(L"forceBorderless");

    int latency = Wh_GetIntSetting(L"maxFrameLatency");
    g_settings.maxFrameLatency = latency < 0 ? 0 : latency > 20 ? 20 : latency;

    g_settings.keepMsaa = Wh_GetIntSetting(L"keepMsaa");
    g_settings.flipModel = Wh_GetIntSetting(L"flipModel");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    // Hooks set here are applied by Windhawk once Wh_ModInit returns.
    HookD3D9ExportsIfLoaded();

    HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (LoadLibraryExW_t)GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    // In case d3d9.dll was loaded while the mod was initializing.
    if (HookD3D9ExportsIfLoaded()) {
        Wh_ApplyHookOperations();
    }
}

void Wh_ModSettingsChanged() {
    // Takes effect the next time the game creates or resets its device.
    LoadSettings();
}
