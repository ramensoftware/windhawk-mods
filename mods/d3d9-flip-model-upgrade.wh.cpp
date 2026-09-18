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
**Important: close the game before this mod gets disabled, updated or
reloaded.** A game that runs with the mod keeps using the Direct3D 9Ex device
and the emulated textures, which only work while the mod is loaded, so unloading
the mod will most likely break or crash the game. That includes disabling the
mod, changing its process inclusion list, and **mod updates that Windhawk
installs automatically in the background**. If you play with this mod enabled,
consider turning off automatic updates for it. Changing the settings is fine,
they apply the next time the game creates or resets its device.

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

A game that is not DPI aware gets stretched by DWM on a scaled display, which
prevents Independent Flip as well. If that is the case, open the properties of
the game executable, and under Compatibility, Change high DPI settings, set the
high DPI scaling override to Application.

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
  Games that often lock or read back managed buffers may run slower.
- D3D9 replacements that don't use the system `d3d9.dll` (DXVK, d3d9on12
  wrappers) bypass this mod entirely.
- A Direct3D 9Ex device is never reported as lost. Games with logic that
  depends on losing the device after Alt+Tab may misbehave.
- Textures restored by a state block are not tracked. If the game modifies
  such a texture while it stays bound, the change shows up once the texture is
  bound again.
- The flip model can only present to the device window. Games that present the
  same swap chain to several windows will only draw to one of them.
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
    covering the monitor. The game's render resolution is kept and
    stretched, so the picture is distorted if the aspect ratios differ.
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

#ifndef D3DPRESENTFLAG_VIDEO
#define D3DPRESENTFLAG_VIDEO 0x00000010
#endif

#ifndef S_PRESENT_MODE_CHANGED
#define S_PRESENT_MODE_CHANGED MAKE_HRESULT(0, _FACD3D, 2167)
#endif

#ifndef S_PRESENT_OCCLUDED
#define S_PRESENT_OCCLUDED MAKE_HRESULT(0, _FACD3D, 2168)
#endif

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
bool g_deviceHooked;

// Devices are not removed when released, an entry is overwritten once a new
// device shows up at the same address.
struct DeviceSet {
    SRWLOCK lock = SRWLOCK_INIT;
    std::vector<IDirect3DDevice9*> devices;

    void Set(IDirect3DDevice9* dev, bool contained) {
        AcquireSRWLockExclusive(&lock);
        auto it = std::find(devices.begin(), devices.end(), dev);
        if (contained && it == devices.end()) {
            devices.push_back(dev);
        } else if (!contained && it != devices.end()) {
            devices.erase(it);
        }
        ReleaseSRWLockExclusive(&lock);
    }

    bool Contains(IDirect3DDevice9* dev) {
        AcquireSRWLockShared(&lock);
        bool found =
            std::find(devices.begin(), devices.end(), dev) != devices.end();
        ReleaseSRWLockShared(&lock);
        return found;
    }
};

// Devices with a FLIPEX implicit swap chain.
DeviceSet g_flipDevices;
// Devices which the game created as plain Direct3D 9 devices.
DeviceSet g_upgradedDevices;

// Set once the managed pool emulation is in use.
std::atomic<bool> g_emulatedTexturesUsed;

HMODULE g_d3d9Module;

thread_local bool g_inCreateDevice;
thread_local bool g_inReset;
thread_local bool g_inIsExDevice;

////////////////////////////////////////////////////////////////////////////////
// Helpers

void** GetVtbl(void* obj) {
    return *reinterpret_cast<void***>(obj);
}

void SetFlipDevice(IDirect3DDevice9* dev, bool flip) {
    g_flipDevices.Set(dev, flip);
}

bool IsFlipDevice(IDirect3DDevice9* dev) {
    return g_flipDevices.Contains(dev);
}

bool IsExDevice(IDirect3DDevice9* dev) {
    // The QueryInterface hook must not interfere.
    g_inIsExDevice = true;
    IUnknown* ex = nullptr;
    HRESULT hr =
        dev->QueryInterface(__uuidof(IDirect3DDevice9Ex), (void**)&ex);
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

// Returns false if the params are left alone. Whether the flip model was
// applied is reflected by pp->SwapEffect.
bool AdjustPresentParams(D3DPRESENT_PARAMETERS* pp) {
    if (!ShouldUpgrade(pp)) {
        return false;
    }

    Wh_Log(L"Requested: %ux%u fmt=%d count=%u msaa=%d swap=%d windowed=%d "
           L"flags=0x%X interval=0x%X",
           pp->BackBufferWidth, pp->BackBufferHeight, pp->BackBufferFormat,
           pp->BackBufferCount, pp->MultiSampleType, pp->SwapEffect,
           pp->Windowed, pp->Flags, pp->PresentationInterval);

    // Only the case with forceBorderless, see ShouldUpgrade.
    bool madeWindowed = !pp->Windowed;
    if (madeWindowed) {
        pp->Windowed = TRUE;
        pp->FullScreen_RefreshRateInHz = 0;
    }

    if (!g_settings.flipModel) {
        Wh_Log(L"Flip model disabled, keeping the swap effect");
        return madeWindowed;
    }

    pp->SwapEffect = D3DSWAPEFFECT_FLIPEX;

    if (pp->BackBufferCount < (UINT)g_settings.backBufferCount) {
        pp->BackBufferCount = g_settings.backBufferCount;
    }

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

    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    exStyle &= ~(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME |
                 WS_EX_STATICEDGE);
    SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle);

    const RECT& rc = mi.rcMonitor;
    // The window may belong to a thread that is waiting for this one.
    UINT flags = SWP_FRAMECHANGED | SWP_NOACTIVATE;
    if (GetWindowThreadProcessId(hWnd, nullptr) != GetCurrentThreadId()) {
        flags |= SWP_ASYNCWINDOWPOS;
    }
    SetWindowPos(hWnd, HWND_TOP, rc.left, rc.top, rc.right - rc.left,
                 rc.bottom - rc.top, flags);
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

void ApplyFrameLatency(IDirect3DDevice9Ex* dev) {
    if (g_settings.maxFrameLatency > 0) {
        HRESULT hr = dev->SetMaximumFrameLatency(g_settings.maxFrameLatency);
        Wh_Log(L"SetMaximumFrameLatency(%d): 0x%08X",
               g_settings.maxFrameLatency, hr);
    }
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

void ClearBoundTextures(IDirect3DDevice9* dev);

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
    ClearBoundTextures(dev);

    D3DPRESENT_PARAMETERS local = *pp;
    bool adjusted = AdjustPresentParams(&local);

    HRESULT hr = E_FAIL;
    if (adjusted) {
        g_inReset = true;
        hr = callOriginal(&local, nullptr);
        g_inReset = false;
        Wh_Log(L"Reset%s with adjusted params: 0x%08X", useEx ? L"Ex" : L"",
               hr);
    }

    // dev was verified to be an Ex device.
    IDirect3DDevice9Ex* devEx = (IDirect3DDevice9Ex*)dev;

    if (SUCCEEDED(hr)) {
        SetFlipDevice(dev, local.SwapEffect == D3DSWAPEFFECT_FLIPEX);
        WriteBackPresentParams(pp, local);
        ApplyFrameLatency(devEx);
        MakeBorderless(GetDeviceWindow(dev, &local));
        return hr;
    }

    g_inReset = true;
    hr = callOriginal(pp, mode);
    g_inReset = false;
    Wh_Log(L"Reset%s with the game's params: 0x%08X", useEx ? L"Ex" : L"", hr);
    if (SUCCEEDED(hr)) {
        SetFlipDevice(dev, pp->SwapEffect == D3DSWAPEFFECT_FLIPEX);
        ApplyFrameLatency(devEx);
        if (pp->Windowed) {
            MakeBorderless(GetDeviceWindow(dev, pp));
        }
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

// Success codes that only a Direct3D 9Ex device returns. A game written for
// Direct3D 9 never saw them, and may treat anything but D3D_OK as a failure.
bool IsExPresentStatus(HRESULT hr) {
    return hr == S_PRESENT_OCCLUDED || hr == S_PRESENT_MODE_CHANGED;
}

HRESULT MaskExPresentStatus(IDirect3DDevice9* dev, HRESULT hr) {
    if (IsExPresentStatus(hr) && g_upgradedDevices.Contains(dev)) {
        return D3D_OK;
    }
    return hr;
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
    HRESULT hr =
        IsFlipDevice(dev)
            ? Present_Original(dev, nullptr, nullptr, nullptr, nullptr)
            : Present_Original(dev, src, dst, wndOverride, dirty);
    return MaskExPresentStatus(dev, hr);
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
    D3DPRESENT_PARAMETERS pp = {};
    if (SUCCEEDED(swapChain->GetPresentParameters(&pp)) &&
        pp.SwapEffect == D3DSWAPEFFECT_FLIPEX) {
        src = nullptr;
        dst = nullptr;
        wndOverride = nullptr;
        dirty = nullptr;
    }

    HRESULT hr = SwapChainPresent_Original(swapChain, src, dst, wndOverride,
                                           dirty, flags);
    if (IsExPresentStatus(hr)) {
        IDirect3DDevice9* dev = nullptr;
        if (SUCCEEDED(swapChain->GetDevice(&dev))) {
            hr = MaskExPresentStatus(dev, hr);
            dev->Release();
        }
    }
    return hr;
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

void EnsureTextureHooks(IDirect3DTexture9* texture);
void EnsureTextureHooks(IDirect3DCubeTexture9* texture);
void EnsureTextureHooks(IDirect3DVolumeTexture9* texture);

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

    g_emulatedTexturesUsed = true;
    EnsureTextureHooks(systemTexture);

    *texture = systemTexture;
    return hr;
}

// Returns the AddRef'd default pool twin of an emulated managed texture.
IDirect3DBaseTexture9* GetVideoTexture(IDirect3DResource9* texture) {
    IDirect3DBaseTexture9* videoTexture = nullptr;
    DWORD size = sizeof(IDirect3DBaseTexture9*);
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
    if (dest && SUCCEEDED(dest->GetContainer(__uuidof(IDirect3DTexture9),
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

// Emulated textures currently bound, per device. A managed texture is uploaded
// when it's used for drawing, so it's uploaded when bound, and again before a
// draw call if the game modified it while it was bound.
//
// No references are held for bound textures, as that would keep the device
// alive. The pointers of released textures may linger, so they are only ever
// compared, never dereferenced. A texture waiting for upload is referenced, but
// only until the next draw call.
struct BoundTexture {
    IDirect3DBaseTexture9* systemTexture;  // For comparing only.
    IDirect3DBaseTexture9* videoTexture;   // For comparing only.
    IDirect3DBaseTexture9* dirtyTexture;   // systemTexture, AddRef'd.
};

constexpr int kBoundTextureCount = 16 + 5;

struct DeviceTextures {
    IDirect3DDevice9* device;
    BoundTexture bound[kBoundTextureCount];
};

SRWLOCK g_deviceTexturesLock = SRWLOCK_INIT;
std::vector<DeviceTextures> g_deviceTextures;
std::atomic<int> g_dirtyTextureCount;

// Set while the mod itself uploads a texture, Direct3D may unlock internally.
thread_local bool g_inTextureUpload;

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

// Must be called with g_deviceTexturesLock held.
DeviceTextures* FindDeviceTextures(IDirect3DDevice9* dev, bool create) {
    for (auto& deviceTextures : g_deviceTextures) {
        if (deviceTextures.device == dev) {
            return &deviceTextures;
        }
    }
    if (!create) {
        return nullptr;
    }
    g_deviceTextures.push_back({dev});
    return &g_deviceTextures.back();
}

// Must be called with g_deviceTexturesLock held. The returned texture has to
// be released once the lock is no longer held.
IDirect3DBaseTexture9* TakeDirtyTexture(BoundTexture& bound) {
    IDirect3DBaseTexture9* dirtyTexture = bound.dirtyTexture;
    if (dirtyTexture) {
        bound.dirtyTexture = nullptr;
        g_dirtyTextureCount--;
    }
    return dirtyTexture;
}

// texture must be a live texture.
void MarkTextureDirty(IDirect3DBaseTexture9* texture) {
    if (g_inTextureUpload) {
        return;
    }

    AcquireSRWLockExclusive(&g_deviceTexturesLock);
    for (auto& deviceTextures : g_deviceTextures) {
        for (auto& bound : deviceTextures.bound) {
            if (bound.systemTexture == texture && !bound.dirtyTexture) {
                texture->AddRef();
                bound.dirtyTexture = texture;
                g_dirtyTextureCount++;
            }
        }
    }
    ReleaseSRWLockExclusive(&g_deviceTexturesLock);
}

// Copies the dirty regions only, fails while the game holds a lock. texture
// must be a live texture.
HRESULT UploadTexture(IDirect3DDevice9* dev, IDirect3DBaseTexture9* texture) {
    IDirect3DBaseTexture9* videoTexture = GetVideoTexture(texture);
    if (!videoTexture) {
        return S_OK;
    }

    g_inTextureUpload = true;
    HRESULT hr = UpdateTexture_Original(dev, texture, videoTexture);
    g_inTextureUpload = false;
    videoTexture->Release();

    if (FAILED(hr)) {
        static std::atomic<int> logged;
        if (logged < 20) {
            logged++;
            Wh_Log(L"Upload failed: 0x%08X", hr);
        }
    }
    return hr;
}

void ClearBoundTextures(IDirect3DDevice9* dev) {
    std::vector<IDirect3DBaseTexture9*> dirtyTextures;

    AcquireSRWLockExclusive(&g_deviceTexturesLock);
    for (auto& deviceTextures : g_deviceTextures) {
        if (dev && deviceTextures.device != dev) {
            continue;
        }
        for (auto& bound : deviceTextures.bound) {
            if (auto* dirtyTexture = TakeDirtyTexture(bound)) {
                dirtyTextures.push_back(dirtyTexture);
            }
            bound = {};
        }
    }
    ReleaseSRWLockExclusive(&g_deviceTexturesLock);

    for (auto* dirtyTexture : dirtyTextures) {
        dirtyTexture->Release();
    }
}

void UploadDirtyTextures(IDirect3DDevice9* dev) {
    if (!g_dirtyTextureCount) {
        return;
    }

    IDirect3DBaseTexture9* dirtyTextures[kBoundTextureCount];
    int count = 0;

    AcquireSRWLockExclusive(&g_deviceTexturesLock);
    if (DeviceTextures* deviceTextures = FindDeviceTextures(dev, false)) {
        for (auto& bound : deviceTextures->bound) {
            if (auto* dirtyTexture = TakeDirtyTexture(bound)) {
                dirtyTextures[count++] = dirtyTexture;
            }
        }
    }
    ReleaseSRWLockExclusive(&g_deviceTexturesLock);

    for (int i = 0; i < count; i++) {
        // The same texture may be bound to several stages.
        bool uploaded = false;
        for (int j = 0; j < i; j++) {
            uploaded = uploaded || dirtyTextures[j] == dirtyTextures[i];
        }
        if (!uploaded && FAILED(UploadTexture(dev, dirtyTextures[i]))) {
            // Most likely still locked by the game.
            MarkTextureDirty(dirtyTextures[i]);
        }
    }

    for (int i = 0; i < count; i++) {
        dirtyTextures[i]->Release();
    }
}

using SetTexture_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,
                                                 DWORD,
                                                 IDirect3DBaseTexture9*);
SetTexture_t SetTexture_Original;
HRESULT STDMETHODCALLTYPE SetTexture_Hook(IDirect3DDevice9* dev,
                                          DWORD stage,
                                          IDirect3DBaseTexture9* texture) {
    if (!g_emulatedTexturesUsed) {
        return SetTexture_Original(dev, stage, texture);
    }

    IDirect3DBaseTexture9* videoTexture =
        texture ? GetVideoTexture(texture) : nullptr;

    bool uploaded = !videoTexture || SUCCEEDED(UploadTexture(dev, texture));
    HRESULT hr =
        SetTexture_Original(dev, stage, videoTexture ? videoTexture : texture);

    int index = BoundTextureIndex(stage);
    if (index >= 0) {
        IDirect3DBaseTexture9* dirtyTexture = nullptr;

        AcquireSRWLockExclusive(&g_deviceTexturesLock);
        if (DeviceTextures* deviceTextures =
                FindDeviceTextures(dev, videoTexture != nullptr)) {
            BoundTexture newBound = {texture, videoTexture};
            if (!videoTexture) {
                newBound = {};
                // The game may bind a twin it got from GetTexture, in which
                // case the texture is still in use.
                for (auto& bound : deviceTextures->bound) {
                    if (texture && bound.videoTexture == texture) {
                        newBound = {bound.systemTexture, bound.videoTexture};
                        break;
                    }
                }
            }

            BoundTexture& bound = deviceTextures->bound[index];
            dirtyTexture = TakeDirtyTexture(bound);
            bound = newBound;
        }
        ReleaseSRWLockExclusive(&g_deviceTexturesLock);

        if (dirtyTexture) {
            dirtyTexture->Release();
        }

        if (!uploaded) {
            MarkTextureDirty(texture);
        }
    }

    if (videoTexture) {
        videoTexture->Release();
    }
    return hr;
}

// The game modifying a bound texture is noticed by the following hooks. They
// are set when the first emulated texture of each type is created.

using TextureUnlockRect_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DTexture9*,
                                                        UINT);
TextureUnlockRect_t TextureUnlockRect_Original;
HRESULT STDMETHODCALLTYPE TextureUnlockRect_Hook(IDirect3DTexture9* texture,
                                                 UINT level) {
    HRESULT hr = TextureUnlockRect_Original(texture, level);
    MarkTextureDirty(texture);
    return hr;
}

using TextureAddDirtyRect_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DTexture9*,
                                                          const RECT*);
TextureAddDirtyRect_t TextureAddDirtyRect_Original;
HRESULT STDMETHODCALLTYPE TextureAddDirtyRect_Hook(IDirect3DTexture9* texture,
                                                   const RECT* dirtyRect) {
    HRESULT hr = TextureAddDirtyRect_Original(texture, dirtyRect);
    MarkTextureDirty(texture);
    return hr;
}

using CubeUnlockRect_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DCubeTexture9*,
                                                     D3DCUBEMAP_FACES,
                                                     UINT);
CubeUnlockRect_t CubeUnlockRect_Original;
HRESULT STDMETHODCALLTYPE CubeUnlockRect_Hook(IDirect3DCubeTexture9* texture,
                                              D3DCUBEMAP_FACES face,
                                              UINT level) {
    HRESULT hr = CubeUnlockRect_Original(texture, face, level);
    MarkTextureDirty(texture);
    return hr;
}

using CubeAddDirtyRect_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DCubeTexture9*,
                                                       D3DCUBEMAP_FACES,
                                                       const RECT*);
CubeAddDirtyRect_t CubeAddDirtyRect_Original;
HRESULT STDMETHODCALLTYPE CubeAddDirtyRect_Hook(IDirect3DCubeTexture9* texture,
                                                D3DCUBEMAP_FACES face,
                                                const RECT* dirtyRect) {
    HRESULT hr = CubeAddDirtyRect_Original(texture, face, dirtyRect);
    MarkTextureDirty(texture);
    return hr;
}

using VolumeUnlockBox_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DVolumeTexture9*,
                                                      UINT);
VolumeUnlockBox_t VolumeUnlockBox_Original;
HRESULT STDMETHODCALLTYPE VolumeUnlockBox_Hook(IDirect3DVolumeTexture9* texture,
                                               UINT level) {
    HRESULT hr = VolumeUnlockBox_Original(texture, level);
    MarkTextureDirty(texture);
    return hr;
}

using VolumeAddDirtyBox_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DVolumeTexture9*,
                                                        const D3DBOX*);
VolumeAddDirtyBox_t VolumeAddDirtyBox_Original;
HRESULT STDMETHODCALLTYPE
VolumeAddDirtyBox_Hook(IDirect3DVolumeTexture9* texture,
                       const D3DBOX* dirtyBox) {
    HRESULT hr = VolumeAddDirtyBox_Original(texture, dirtyBox);
    MarkTextureDirty(texture);
    return hr;
}

// D3DX and some games lock the surfaces of a texture instead.
void MarkSurfaceContainerDirty(IDirect3DSurface9* surface) {
    if (g_inTextureUpload) {
        return;
    }

    IDirect3DBaseTexture9* container = nullptr;
    if (SUCCEEDED(surface->GetContainer(__uuidof(IDirect3DBaseTexture9),
                                        (void**)&container))) {
        MarkTextureDirty(container);
        container->Release();
    }
}

using SurfaceUnlockRect_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DSurface9*);
SurfaceUnlockRect_t SurfaceUnlockRect_Original;
HRESULT STDMETHODCALLTYPE SurfaceUnlockRect_Hook(IDirect3DSurface9* surface) {
    HRESULT hr = SurfaceUnlockRect_Original(surface);
    MarkSurfaceContainerDirty(surface);
    return hr;
}

using SurfaceReleaseDC_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DSurface9*, HDC);
SurfaceReleaseDC_t SurfaceReleaseDC_Original;
HRESULT STDMETHODCALLTYPE SurfaceReleaseDC_Hook(IDirect3DSurface9* surface,
                                                HDC hdc) {
    HRESULT hr = SurfaceReleaseDC_Original(surface, hdc);
    MarkSurfaceContainerDirty(surface);
    return hr;
}

// Vtable slots: 19 LockRect/LockBox, 20 UnlockRect/UnlockBox, 21 AddDirtyRect/
// AddDirtyBox for the textures; 14 UnlockRect, 16 ReleaseDC for the surface.
enum {
    kSlotTextureUnlock = 20,
    kSlotTextureAddDirty = 21,
    kSlotSurfaceUnlockRect = 14,
    kSlotSurfaceReleaseDC = 16,
};

// Hooks a function unless one of the other texture types, sharing the
// implementation, got there first.
std::vector<void*> g_hookedTextureTargets;

template <typename T>
void SetTextureFunctionHook(void* target, T hook, T* original) {
    auto& hookedTargets = g_hookedTextureTargets;
    if (std::find(hookedTargets.begin(), hookedTargets.end(), target) !=
        hookedTargets.end()) {
        return;
    }
    hookedTargets.push_back(target);
    WindhawkUtils::SetFunctionHook((T)target, hook, original);
}

void EnsureTextureHooks(IDirect3DTexture9* texture) {
    std::lock_guard<std::mutex> guard(g_hookMutex);
    static bool hooked;
    if (hooked) {
        return;
    }
    hooked = true;

    void** vtbl = GetVtbl(texture);
    SetTextureFunctionHook((void*)vtbl[kSlotTextureUnlock],
                           TextureUnlockRect_Hook, &TextureUnlockRect_Original);
    SetTextureFunctionHook((void*)vtbl[kSlotTextureAddDirty],
                           TextureAddDirtyRect_Hook,
                           &TextureAddDirtyRect_Original);

    IDirect3DSurface9* surface = nullptr;
    if (SUCCEEDED(texture->GetSurfaceLevel(0, &surface))) {
        void** surfaceVtbl = GetVtbl(surface);
        SetTextureFunctionHook((void*)surfaceVtbl[kSlotSurfaceUnlockRect],
                               SurfaceUnlockRect_Hook,
                               &SurfaceUnlockRect_Original);
        SetTextureFunctionHook((void*)surfaceVtbl[kSlotSurfaceReleaseDC],
                               SurfaceReleaseDC_Hook,
                               &SurfaceReleaseDC_Original);
        surface->Release();
    }

    Wh_ApplyHookOperations();
    Wh_Log(L"Texture hooks installed");
}

void EnsureTextureHooks(IDirect3DCubeTexture9* texture) {
    std::lock_guard<std::mutex> guard(g_hookMutex);
    static bool hooked;
    if (hooked) {
        return;
    }
    hooked = true;

    void** vtbl = GetVtbl(texture);
    SetTextureFunctionHook((void*)vtbl[kSlotTextureUnlock], CubeUnlockRect_Hook,
                           &CubeUnlockRect_Original);
    SetTextureFunctionHook((void*)vtbl[kSlotTextureAddDirty],
                           CubeAddDirtyRect_Hook, &CubeAddDirtyRect_Original);

    Wh_ApplyHookOperations();
    Wh_Log(L"Cube texture hooks installed");
}

void EnsureTextureHooks(IDirect3DVolumeTexture9* texture) {
    std::lock_guard<std::mutex> guard(g_hookMutex);
    static bool hooked;
    if (hooked) {
        return;
    }
    hooked = true;

    void** vtbl = GetVtbl(texture);
    SetTextureFunctionHook((void*)vtbl[kSlotTextureUnlock],
                           VolumeUnlockBox_Hook, &VolumeUnlockBox_Original);
    SetTextureFunctionHook((void*)vtbl[kSlotTextureAddDirty],
                           VolumeAddDirtyBox_Hook, &VolumeAddDirtyBox_Original);

    Wh_ApplyHookOperations();
    Wh_Log(L"Volume texture hooks installed");
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
    UploadDirtyTextures(dev);
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
    UploadDirtyTextures(dev);
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
    UploadDirtyTextures(dev);
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
    UploadDirtyTextures(dev);
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

bool IsD3DXAddress(void* address) {
    HMODULE module;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCWSTR)address, &module)) {
        return false;
    }

    // D3DX asks for every texture it loads, only look at a module once.
    static std::atomic<HMODULE> d3dxModule, otherModule;
    if (module == d3dxModule) {
        return true;
    }
    if (module == otherModule) {
        return false;
    }

    WCHAR path[MAX_PATH];
    if (!GetModuleFileNameW(module, path, ARRAYSIZE(path))) {
        return false;
    }

    PCWSTR name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;
    bool isD3DX = _wcsnicmp(name, L"d3dx9", 5) == 0;
    (isD3DX ? d3dxModule : otherModule) = module;

    static std::atomic<int> logged;
    if (logged < 30) {
        logged++;
        Wh_Log(L"QueryInterface(IDirect3DDevice9Ex) from %s%s", name,
               isD3DX ? L": hidden" : L"");
    }
    return isD3DX;
}

HRESULT STDMETHODCALLTYPE DeviceQueryInterface_Hook(IUnknown* self,
                                                    REFIID riid,
                                                    void** object) {
    if (!g_inIsExDevice && object &&
        IsEqualGUID(riid, __uuidof(IDirect3DDevice9Ex))) {
        if (IsD3DXAddress(__builtin_return_address(0))) {
            *object = nullptr;
            return E_NOINTERFACE;
        }
    }

    return DeviceQueryInterface_Original(self, riid, object);
}

// Only one set of device functions is hooked. A device implemented by other
// functions would miss the managed pool emulation, so it must not be upgraded.
constexpr int kDeviceGuardSlots[] = {
    kSlotReset,         kSlotPresent,    kSlotCreateTexture,
    kSlotUpdateTexture, kSlotSetTexture, kSlotDrawIndexedPrimitive,
};
void* g_hookedDeviceFunctions[ARRAYSIZE(kDeviceGuardSlots)];

bool HasHookedDeviceFunctions(IDirect3DDevice9* dev) {
    void** vtbl = GetVtbl(dev);
    for (size_t i = 0; i < ARRAYSIZE(kDeviceGuardSlots); i++) {
        if (vtbl[kDeviceGuardSlots[i]] != g_hookedDeviceFunctions[i]) {
            return false;
        }
    }
    return true;
}

void EnsureDeviceHooks(IDirect3DDevice9Ex* dev) {
    std::lock_guard<std::mutex> guard(g_hookMutex);
    if (g_deviceHooked) {
        return;
    }
    g_deviceHooked = true;

    void** vtbl = GetVtbl(dev);
    for (size_t i = 0; i < ARRAYSIZE(kDeviceGuardSlots); i++) {
        g_hookedDeviceFunctions[i] = vtbl[kDeviceGuardSlots[i]];
    }

    WindhawkUtils::SetFunctionHook(
        (DeviceQueryInterface_t)vtbl[kSlotDeviceQueryInterface],
        DeviceQueryInterface_Hook, &DeviceQueryInterface_Original);
    WindhawkUtils::SetFunctionHook((Reset_t)vtbl[kSlotReset], Reset_Hook,
                                   &Reset_Original);
    WindhawkUtils::SetFunctionHook((Present_t)vtbl[kSlotPresent], Present_Hook,
                                   &Present_Original);
    WindhawkUtils::SetFunctionHook(
        (UpdateSurface_t)vtbl[kSlotUpdateSurface],
        UpdateSurface_Hook, &UpdateSurface_Original);
    WindhawkUtils::SetFunctionHook(
        (UpdateTexture_t)vtbl[kSlotUpdateTexture],
        UpdateTexture_Hook, &UpdateTexture_Original);
    WindhawkUtils::SetFunctionHook(
        (SetTexture_t)vtbl[kSlotSetTexture],
        SetTexture_Hook, &SetTexture_Original);
    WindhawkUtils::SetFunctionHook(
        (DrawPrimitive_t)vtbl[kSlotDrawPrimitive],
        DrawPrimitive_Hook, &DrawPrimitive_Original);
    WindhawkUtils::SetFunctionHook(
        (DrawIndexedPrimitive_t)vtbl[kSlotDrawIndexedPrimitive],
        DrawIndexedPrimitive_Hook, &DrawIndexedPrimitive_Original);
    WindhawkUtils::SetFunctionHook(
        (DrawPrimitiveUP_t)vtbl[kSlotDrawPrimitiveUP],
        DrawPrimitiveUP_Hook, &DrawPrimitiveUP_Original);
    WindhawkUtils::SetFunctionHook(
        (DrawIndexedPrimitiveUP_t)vtbl[kSlotDrawIndexedPrimitiveUP],
        DrawIndexedPrimitiveUP_Hook, &DrawIndexedPrimitiveUP_Original);
    WindhawkUtils::SetFunctionHook(
        (CreateTexture_t)vtbl[kSlotCreateTexture],
        CreateTexture_Hook, &CreateTexture_Original);
    WindhawkUtils::SetFunctionHook(
        (CreateVolumeTexture_t)vtbl[kSlotCreateVolumeTexture],
        CreateVolumeTexture_Hook, &CreateVolumeTexture_Original);
    WindhawkUtils::SetFunctionHook(
        (CreateCubeTexture_t)vtbl[kSlotCreateCubeTexture],
        CreateCubeTexture_Hook, &CreateCubeTexture_Original);
    WindhawkUtils::SetFunctionHook(
        (CreateVertexBuffer_t)vtbl[kSlotCreateVertexBuffer],
        CreateVertexBuffer_Hook, &CreateVertexBuffer_Original);
    WindhawkUtils::SetFunctionHook(
        (CreateIndexBuffer_t)vtbl[kSlotCreateIndexBuffer],
        CreateIndexBuffer_Hook, &CreateIndexBuffer_Original);
    WindhawkUtils::SetFunctionHook(
        (PresentEx_t)vtbl[kSlotPresentEx],
        PresentEx_Hook, &PresentEx_Original);
    WindhawkUtils::SetFunctionHook((ResetEx_t)vtbl[kSlotResetEx], ResetEx_Hook,
                                   &ResetEx_Original);

    IDirect3DSwapChain9* swapChain = nullptr;
    if (SUCCEEDED(dev->GetSwapChain(0, &swapChain))) {
        WindhawkUtils::SetFunctionHook(
            (SwapChainPresent_t)GetVtbl(swapChain)[kSlotSwapChainPresent],
            SwapChainPresent_Hook, &SwapChainPresent_Original);
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
        Wh_Log(L"CreateDeviceEx with adjusted params: 0x%08X", hr);
    }

    if (SUCCEEDED(hr)) {
        EnsureDeviceHooks(*device);
        g_upgradedDevices.Set(*device, false);
        ClearBoundTextures(*device);
        SetFlipDevice(*device, local.SwapEffect == D3DSWAPEFFECT_FLIPEX);
        WriteBackPresentParams(pp, local);
        ApplyFrameLatency(*device);
        MakeBorderless(local.hDeviceWindow ? local.hDeviceWindow : focusWindow);
    } else {
        hr = CreateDeviceEx_Original(d3d, adapter, deviceType, focusWindow,
                                     behaviorFlags, pp, mode, device);
        Wh_Log(L"CreateDeviceEx with the game's params: 0x%08X", hr);
        if (SUCCEEDED(hr) && device && *device) {
            // Still an Ex device, the managed pool conversion is needed.
            EnsureDeviceHooks(*device);
            g_upgradedDevices.Set(*device, false);
            ClearBoundTextures(*device);
            SetFlipDevice(*device,
                          pp && pp->SwapEffect == D3DSWAPEFFECT_FLIPEX);
            ApplyFrameLatency(*device);
            if (pp && pp->Windowed) {
                MakeBorderless(pp->hDeviceWindow ? pp->hDeviceWindow
                                                 : focusWindow);
            }
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
            g_upgradedDevices.Set(*device, false);
            ClearBoundTextures(*device);
        }
        return hr;
    };

    if (g_inCreateDevice || !device || !CreateDeviceEx_Original ||
        (behaviorFlags & D3DCREATE_ADAPTERGROUP_DEVICE) ||
        !ShouldUpgrade(pp)) {
        return callOriginal();
    }

    IDirect3D9Ex* d3dEx = nullptr;
    if (FAILED(d3d->QueryInterface(__uuidof(IDirect3D9Ex),
                                   (void**)&d3dEx))) {
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

    if (!HasHookedDeviceFunctions(deviceEx)) {
        Wh_Log(L"Unexpected device implementation, creating a regular device");
        deviceEx->Release();
        return callOriginal();
    }

    if (g_settings.flipModel && !IsFlipDevice(deviceEx)) {
        // FLIPEX was refused, no reason to keep the Ex device.
        Wh_Log(L"FLIPEX refused, creating a regular device");
        deviceEx->Release();
        return callOriginal();
    }

    Wh_Log(L"CreateDevice upgraded to D3D9Ex%s: %p",
           IsFlipDevice(deviceEx) ? L" + FLIPEX" : L"", deviceEx);
    g_upgradedDevices.Set(deviceEx, true);
    *device = deviceEx;
    return hr;
}

// Must be called with g_hookMutex held. Returns true if a hook was set.
bool HookD3D9Vtbl(void** vtbl, bool isEx) {
    bool hooked = false;

    auto createDevice = (CreateDevice_t)vtbl[kSlotCreateDevice];

    static CreateDevice_t hookedCreateDevice[2];
    if (createDevice != hookedCreateDevice[0] &&
        createDevice != hookedCreateDevice[1]) {
        if (!hookedCreateDevice[0]) {
            hookedCreateDevice[0] = createDevice;
            WindhawkUtils::SetFunctionHook(createDevice, CreateDevice_Hook<0>,
                                           &CreateDevice_Original[0]);
            hooked = true;
        } else if (!hookedCreateDevice[1]) {
            hookedCreateDevice[1] = createDevice;
            WindhawkUtils::SetFunctionHook(createDevice, CreateDevice_Hook<1>,
                                           &CreateDevice_Original[1]);
            hooked = true;
        }
    }

    static bool createDeviceExHooked;
    if (isEx && !createDeviceExHooked) {
        createDeviceExHooked = true;
        WindhawkUtils::SetFunctionHook(
            (CreateDeviceEx_t)vtbl[kSlotCreateDeviceEx], CreateDeviceEx_Hook,
            &CreateDeviceEx_Original);
        hooked = true;
    }

    return hooked;
}

// Called for every object, as the Ex and non-Ex ones may differ.
void EnsureD3D9Hooks(IDirect3D9* d3d, bool isEx) {
    std::lock_guard<std::mutex> guard(g_hookMutex);

    bool hooked = HookD3D9Vtbl(GetVtbl(d3d), isEx);

    static bool exObjectTried;
    if (!isEx && !exObjectTried) {
        exObjectTried = true;

        // A throwaway Ex object for the CreateDeviceEx address.
        IDirect3D9Ex* d3dEx = nullptr;
        HRESULT hr = Direct3DCreate9Ex_Original(D3D_SDK_VERSION, &d3dEx);
        if (SUCCEEDED(hr) && d3dEx) {
            hooked = HookD3D9Vtbl(GetVtbl(d3dEx), true) || hooked;
            d3dEx->Release();
        } else {
            Wh_Log(L"Direct3DCreate9Ex failed (0x%08X), can't upgrade", hr);
        }
    }

    if (hooked) {
        Wh_ApplyHookOperations();
        Wh_Log(L"IDirect3D9 hooks installed");
    }
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

    // The hooked code must stay around while the mod is loaded.
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)create9,
                       &g_d3d9Module);

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

    auto vsync = WindhawkUtils::StringSetting::make(L"vsync");
    g_settings.vsync = VSyncMode::unchanged;
    if (wcscmp(vsync, L"on") == 0) {
        g_settings.vsync = VSyncMode::on;
    } else if (wcscmp(vsync, L"off") == 0) {
        g_settings.vsync = VSyncMode::off;
    }

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
    if (!pKernelBaseLoadLibraryExW) {
        Wh_Log(L"LoadLibraryExW not found");
        return FALSE;
    }

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

void Wh_ModUninit() {
    Wh_Log(L">");

    // Textures waiting for upload stay referenced. This is not the render
    // thread of the game, and Direct3D 9 reference counting is only thread safe
    // for multithreaded devices. The game has to be closed at this point anyway,
    // see the readme.

    // The hooks are gone by now.
    if (g_d3d9Module) {
        FreeLibrary(g_d3d9Module);
        g_d3d9Module = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    // Takes effect the next time the game creates or resets its device.
    LoadSettings();
}
