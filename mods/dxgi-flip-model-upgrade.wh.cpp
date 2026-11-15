// ==WindhawkMod==
// @id              dxgi-flip-model-upgrade
// @name            DXGI Flip Model Upgrade
// @description     Forces the DXGI flip model and tearing support in Direct3D 11 and 12 games, so windowed/borderless games can reach Independent Flip and run uncapped with VSync off
// @version         1.0
// @author          tria
// @github          https://github.com/triatomic
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# DXGI Flip Model Upgrade
**Important: restart the game after this mod gets disabled, updated or
reloaded while the game runs.** The game keeps its upgraded swap chains, which
need the mod for resizing and for keeping its render targets bound, so it may
show a frozen or black picture, or fail. That includes mod updates that
Windhawk installs automatically in the background. If you play with this mod
enabled, consider turning off automatic updates for it.

Many Direct3D 11 games present with the legacy *blt model*
(`DXGI_SWAP_EFFECT_DISCARD` or `SEQUENTIAL`): in a window or borderless window,
every frame is copied and composed by DWM, which adds latency and blocks
Independent Flip. PresentMon reports them as `Composed: Copy with GPU GDI`.
Many games also never enable tearing, so with VSync off they're still capped
at the refresh rate in a window.

This mod changes the swap chains the game creates:
- **Force flip model:** blt model swap chains of Direct3D 11 games use the flip
  model instead (`FLIP_DISCARD` or `FLIP_SEQUENTIAL`). Once the game window
  covers the whole screen, DWM can promote it to **Independent Flip**.
- **Force allow tearing:** flip model swap chains of Direct3D 11 and 12 games
  allow tearing, and frames presented with VSync off show immediately.
- **Maximum frame latency**, **Waitable swap chain** and **Force borderless**
  are off by default, see their descriptions in the settings.

Windows 11 has a similar system-wide option, "Optimizations for windowed
games", for Direct3D 10 and 11. This mod applies per game, also works on
Windows 10, and adds tearing.

## How to enable the mod for a program
This mod doesn't target any process by default. In Windhawk, go to the mod's
"Advanced" tab and scroll down to "Custom process inclusion list". In that box,
put the filename of the game's `.exe`, then click "Save" and (re)start the game.
Only swap chains created while the mod is loaded are changed.

## How to verify
Use [PresentMon](https://github.com/GameTechDev/PresentMon) (or an overlay that
shows the presentation mode, such as RTSS). With the flip model, the game should
show `Hardware: Independent Flip` or `Hardware Composed: Independent Flip` in
borderless fullscreen, and `Composed: Flip` in a smaller window. With tearing
and VSync off, the frame rate can go above the refresh rate. The log says what
was changed for each swap chain, and why something wasn't.

## Known limitations
- Not changed to the flip model: swap chains with a multisampled back buffer,
  swap chains created in exclusive fullscreen (those already get flip
  presentation through Fullscreen Optimizations), Direct3D 10 games, and back
  buffer formats the flip model doesn't support.
- sRGB back buffers aren't supported by the flip model, so their format becomes
  the non-sRGB one. Render target views the game creates for them still get the
  sRGB format, but a game that reads the back buffer format sees the other one.
- The flip model only shows what the game renders with Direct3D. Anything drawn
  into the game window with GDI disappears.
- Tearing doesn't apply in exclusive fullscreen.
- With **Force borderless**, the game runs at the desktop resolution, and its
  picture is stretched if it renders at another one. The game is told it's in
  fullscreen. Alt+Enter is turned off for those swap chains, as it would switch
  to exclusive fullscreen behind the mod's back.
- With **Waitable swap chain**, games that already use a waitable swap chain
  keep their own settings.
- If the game loads `dxgi.dll` only after it started, the mod's hooks are
  installed a moment after that, and a swap chain the game creates right away
  may keep the blt model. The log then shows no "Hooked" lines before that swap
  chain.

## Anti-cheat
**Don't use this mod in online games protected by anti-cheat.** It loads code
into the game and hooks DXGI and Direct3D 11 functions:
`CreateDXGIFactory`, `CreateDXGIFactory1` and `CreateDXGIFactory2`,
`IDXGIFactory::CreateSwapChain` and `IDXGIFactory2::CreateSwapChainForHwnd`,
`IDXGISwapChain::Present`, `Present1`, `ResizeBuffers`, `ResizeBuffers1`,
`SetFullscreenState`, `GetFullscreenState` and `ResizeTarget`,
`IDXGIDevice1::SetMaximumFrameLatency` and
`ID3D11Device::CreateRenderTargetView`. It hooks no Windows loader or kernel
functions, such as `LoadLibraryExW` in `kernelbase.dll`: if `dxgi.dll` isn't
loaded yet when the mod starts, the mod learns about it through a Windows DLL
load notification, and a thread of the mod installs the hooks. With **Force
borderless**, another thread changes the game window. Present hooks are also
how cheats draw overlays, so anti-cheat software may block the game, kick you
or ban your account. There are no guarantees for any game.

Enable the mod's logging (Advanced → Debug logging) to see what happens.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- forceFlipModel: true
  $name: Force flip model
  $description: |-
    Off: swap chains keep the presentation model the game asked for.
    On: blt model swap chains of Direct3D 11 games use the flip model, so that DWM can present them with Independent Flip. Not for multisampled back buffers or swap chains created in exclusive fullscreen.
    Applies to swap chains created afterwards, restart the game after changing it.
- forceAllowTearing: true
  $name: Force allow tearing
  $description: |-
    Off: tearing only if the game asks for it.
    On: flip model swap chains allow tearing, and frames presented with VSync off show immediately instead of waiting for the next refresh. Needed for uncapped frame rates and variable refresh rate in a window. Not in exclusive fullscreen.
    Applies to swap chains created afterwards, restart the game after changing it.
- maxFrameLatency: 0
  $name: Maximum frame latency
  $description: |-
    0: the game decides, usually up to 3 frames queued.
    1-3: at most that many frames queued, lower input latency, maybe a lower frame rate. Also used by the waitable swap chain.
- waitableSwapChain: false
  $name: Waitable swap chain
  $description: |-
    Off: frames queue up to the maximum frame latency.
    On: flip model swap chains wait at the start of each frame until the queue has room, which keeps input latency at its lowest. Games that already use a waitable swap chain keep their own.
    Applies to swap chains created afterwards, restart the game after changing it.
- forceBorderless: false
  $name: Force borderless
  $description: |-
    Off: exclusive fullscreen stays exclusive fullscreen.
    On: exclusive fullscreen requests become a borderless window covering the monitor, so that those games get the flip model and tearing too. Runs at the desktop resolution, and turns off Alt+Enter for those swap chains.
    Anti-cheat: adds a thread that changes the game window, see the readme.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <d3d11.h>
#include <dxgi1_6.h>

#include <algorithm>
#include <atomic>
#include <mutex>
#include <string>
#include <vector>

// Read by the hooks on any thread.
std::atomic<bool> g_forceFlipModel;
std::atomic<bool> g_forceAllowTearing;
std::atomic<int> g_maxFrameLatency;
std::atomic<bool> g_waitableSwapChain;
std::atomic<bool> g_forceBorderless;

std::mutex g_hookMutex;
std::atomic<bool> g_dxgiHooked;
HMODULE g_dxgiModule;

// Hooks can't be set anymore after Wh_ModBeforeUninit. Guarded by g_hookMutex.
bool g_unloading;

// Modules with hooked code, which must stay around while the mod is loaded.
// Guarded by g_hookMutex.
std::vector<HMODULE> g_pinnedModules;

// Set during a swap chain creation, which may call another hooked creation
// method internally.
thread_local bool g_inCreateSwapChain;

// The sRGB format the game asked for, on swap chains and back buffers whose
// format had to become the non-sRGB one for the flip model.
// {F1AE8A51-3E58-4FE2-8CBE-6BC781B08E29}
static const GUID kSrgbFormatGuid = {
    0xF1AE8A51,
    0x3E58,
    0x4FE2,
    {0x8C, 0xBE, 0x6B, 0xC7, 0x81, 0xB0, 0x8E, 0x29}};

// Vtable slots, checked against the C declarations of the SDK headers.
enum {
    kSlotFactoryCreateSwapChain = 10,
    kSlotFactoryCreateSwapChainForHwnd = 15,
    kSlotSwapChainPresent = 8,
    kSlotSwapChainSetFullscreenState = 10,
    kSlotSwapChainGetFullscreenState = 11,
    kSlotSwapChainResizeBuffers = 13,
    kSlotSwapChainResizeTarget = 14,
    kSlotSwapChainPresent1 = 22,
    kSlotSwapChainResizeBuffers1 = 39,
    kSlotDeviceCreateRenderTargetView = 9,
    kSlotDxgiDeviceSetMaximumFrameLatency = 12,
};

void** GetVtbl(void* object) {
    return *reinterpret_cast<void***>(object);
}

template <typename T>
void SafeRelease(T*& object) {
    if (object) {
        object->Release();
        object = nullptr;
    }
}

// Swap chains compared by address only, entries are overwritten when a new
// swap chain shows up at the same address.
struct SwapChainSet {
    SRWLOCK lock = SRWLOCK_INIT;
    std::vector<void*> swapChains;

    void Set(void* swapChain, bool contained) {
        AcquireSRWLockExclusive(&lock);
        auto it = std::find(swapChains.begin(), swapChains.end(), swapChain);
        if (contained && it == swapChains.end()) {
            swapChains.push_back(swapChain);
        } else if (!contained && it != swapChains.end()) {
            swapChains.erase(it);
        }
        ReleaseSRWLockExclusive(&lock);
    }

    bool Contains(void* swapChain) {
        AcquireSRWLockShared(&lock);
        bool found = std::find(swapChains.begin(), swapChains.end(),
                               swapChain) != swapChains.end();
        ReleaseSRWLockShared(&lock);
        return found;
    }
};

// Swap chains changed from the blt model to the flip model.
SwapChainSet g_upgradedSwapChains;

// Swap chains in exclusive fullscreen as far as the game knows, but windowed
// in a borderless window.
SwapChainSet g_fakeFullscreenSwapChains;

// The frame latency waitable objects of swap chains the mod made waitable,
// which the mod waits on after each Present.
struct Waitable {
    void* swapChain;  // For comparing only.
    HANDLE handle;
};
SRWLOCK g_waitablesLock = SRWLOCK_INIT;
std::vector<Waitable> g_waitables;

// Replaces the waitable of swapChain, nullptr removes it. An entry left by a
// destroyed swap chain at the same address is closed here.
void SetWaitable(void* swapChain, HANDLE handle) {
    AcquireSRWLockExclusive(&g_waitablesLock);
    for (auto it = g_waitables.begin(); it != g_waitables.end(); ++it) {
        if (it->swapChain == swapChain) {
            CloseHandle(it->handle);
            g_waitables.erase(it);
            break;
        }
    }
    if (handle) {
        g_waitables.push_back({swapChain, handle});
    }
    ReleaseSRWLockExclusive(&g_waitablesLock);
}

HANDLE GetWaitable(void* swapChain) {
    AcquireSRWLockShared(&g_waitablesLock);
    HANDLE handle = nullptr;
    for (const Waitable& waitable : g_waitables) {
        if (waitable.swapChain == swapChain) {
            handle = waitable.handle;
            break;
        }
    }
    ReleaseSRWLockShared(&g_waitablesLock);
    return handle;
}

////////////////////////////////////////////////////////////////////////////////
// Hooking functions of several implementations

// A method may have different implementations, e.g. for Direct3D 11 and 12
// swap chains. Each gets its own instance of the hook, N, with its original.
constexpr int kMaxImplementations = 4;

template <typename T>
struct HookedMethod {
    void* targets[kMaxImplementations];
    T originals[kMaxImplementations];
};

// Must be called with g_hookMutex held.
void PinModuleLocked(void* address) {
    HMODULE module;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCWSTR)address, &module) ||
        std::find(g_pinnedModules.begin(), g_pinnedModules.end(), module) !=
            g_pinnedModules.end()) {
        return;
    }
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                           (LPCWSTR)address, &module)) {
        g_pinnedModules.push_back(module);
    }
}

// Must be called with g_hookMutex held. Returns true if a hook was set.
template <typename T>
bool HookImplementation(void* target,
                        HookedMethod<T>& hooked,
                        const T (&hooks)[kMaxImplementations],
                        PCWSTR name) {
    if (g_unloading) {
        return false;
    }
    for (int i = 0; i < kMaxImplementations; i++) {
        if (hooked.targets[i] == target) {
            return false;
        }
        if (!hooked.targets[i]) {
            hooked.targets[i] = target;
            PinModuleLocked(target);
            WindhawkUtils::SetFunctionHook((T)target, hooks[i],
                                           &hooked.originals[i]);
            Wh_Log(L"Hooked %s (%d)", name, i);
            return true;
        }
    }
    Wh_Log(L"Too many implementations of %s", name);
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// Borderless windows
//
// Changing a window's style sends messages to its thread and waits for them,
// and that thread may be waiting for the one presenting. So the game window is
// changed by a thread of the mod, which nothing waits for.

constexpr UINT kMakeBorderlessMessage = WM_APP + 1;
constexpr UINT kRestoreWindowMessage = WM_APP + 2;

// Only used by the window thread.
struct SavedWindow {
    HWND hWnd;
    LONG_PTR style;
    LONG_PTR exStyle;
    RECT rect;
};
std::vector<SavedWindow> g_savedWindows;

std::mutex g_windowThreadMutex;
HANDLE g_windowThread;
DWORD g_windowThreadId;

void ApplyBorderless(HWND hWnd) {
    if (!IsWindow(hWnd)) {
        return;
    }

    MONITORINFO monitorInfo = {sizeof(monitorInfo)};
    if (!GetMonitorInfoW(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST),
                         &monitorInfo)) {
        return;
    }

    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);

    bool saved = false;
    for (const SavedWindow& savedWindow : g_savedWindows) {
        saved = saved || savedWindow.hWnd == hWnd;
    }
    if (!saved) {
        SavedWindow savedWindow = {hWnd, style, exStyle};
        GetWindowRect(hWnd, &savedWindow.rect);
        g_savedWindows.push_back(savedWindow);
    }

    LONG_PTR newStyle =
        (style & ~(WS_OVERLAPPEDWINDOW | WS_DLGFRAME | WS_BORDER)) | WS_POPUP;
    LONG_PTR newExStyle =
        exStyle & ~(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE |
                    WS_EX_DLGMODALFRAME | WS_EX_STATICEDGE);

    UINT flags = SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS;
    if (newStyle != style || newExStyle != exStyle) {
        SetWindowLongPtrW(hWnd, GWL_STYLE, newStyle);
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, newExStyle);
        flags |= SWP_FRAMECHANGED;
    }

    const RECT& rc = monitorInfo.rcMonitor;
    SetWindowPos(hWnd, HWND_TOP, rc.left, rc.top, rc.right - rc.left,
                 rc.bottom - rc.top, flags);
    Wh_Log(L"Window %p borderless: %dx%d", hWnd, rc.right - rc.left,
           rc.bottom - rc.top);
}

void RestoreWindow(HWND hWnd) {
    for (auto it = g_savedWindows.begin(); it != g_savedWindows.end(); ++it) {
        if (it->hWnd != hWnd) {
            continue;
        }

        if (IsWindow(hWnd)) {
            // Whether it's minimized, maximized or visible stays as it is.
            constexpr LONG_PTR kStateStyles =
                WS_MINIMIZE | WS_MAXIMIZE | WS_VISIBLE;
            LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
            SetWindowLongPtrW(hWnd, GWL_STYLE,
                              (it->style & ~kStateStyles) |
                                  (style & kStateStyles));
            SetWindowLongPtrW(hWnd, GWL_EXSTYLE, it->exStyle);

            const RECT& rc = it->rect;
            SetWindowPos(hWnd, nullptr, rc.left, rc.top, rc.right - rc.left,
                         rc.bottom - rc.top,
                         SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED |
                             SWP_ASYNCWINDOWPOS);
            Wh_Log(L"Window %p restored", hWnd);
        }

        g_savedWindows.erase(it);
        return;
    }
}

DWORD WINAPI WindowThreadProc(void* parameter) {
    // Creates the message queue before anyone posts to it.
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent((HANDLE)parameter);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == kMakeBorderlessMessage) {
            ApplyBorderless((HWND)msg.wParam);
        } else if (msg.message == kRestoreWindowMessage) {
            RestoreWindow((HWND)msg.wParam);
        }
    }
    return 0;
}

void PostWindowMessage(UINT message, HWND hWnd) {
    if (!hWnd) {
        return;
    }

    std::lock_guard<std::mutex> guard(g_windowThreadMutex);
    if (!g_windowThread) {
        HANDLE ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!ready) {
            return;
        }
        g_windowThread = CreateThread(nullptr, 0, WindowThreadProc, ready, 0,
                                      &g_windowThreadId);
        if (g_windowThread) {
            WaitForSingleObject(ready, INFINITE);
        }
        CloseHandle(ready);
        if (!g_windowThread) {
            Wh_Log(L"Creating the window thread failed");
            return;
        }
    }

    PostThreadMessageW(g_windowThreadId, message, (WPARAM)hWnd, 0);
}

void StopWindowThread() {
    std::lock_guard<std::mutex> guard(g_windowThreadMutex);
    if (!g_windowThread) {
        return;
    }
    PostThreadMessageW(g_windowThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(g_windowThread, INFINITE);
    CloseHandle(g_windowThread);
    g_windowThread = nullptr;
}

HWND GetSwapChainWindow(IDXGISwapChain* swapChain) {
    DXGI_SWAP_CHAIN_DESC desc;
    return SUCCEEDED(swapChain->GetDesc(&desc)) ? desc.OutputWindow : nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Formats and capabilities

bool IsFlipModel(DXGI_SWAP_EFFECT swapEffect) {
    return swapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL ||
           swapEffect == DXGI_SWAP_EFFECT_FLIP_DISCARD;
}

DXGI_FORMAT StripSrgb(DXGI_FORMAT format) {
    switch (format) {
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            return DXGI_FORMAT_B8G8R8A8_UNORM;
        default:
            return format;
    }
}

bool IsFlipModelFormat(DXGI_FORMAT format) {
    switch (format) {
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
            return true;
        default:
            return false;
    }
}

bool IsTearingSupported(IUnknown* factory) {
    // -1: unknown yet.
    static std::atomic<int> supported{-1};
    if (supported >= 0) {
        return supported;
    }

    BOOL tearing = FALSE;
    IDXGIFactory5* factory5 = nullptr;
    if (SUCCEEDED(factory->QueryInterface(__uuidof(IDXGIFactory5),
                                          (void**)&factory5))) {
        if (FAILED(factory5->CheckFeatureSupport(
                DXGI_FEATURE_PRESENT_ALLOW_TEARING, &tearing,
                sizeof(tearing)))) {
            tearing = FALSE;
        }
        SafeRelease(factory5);
    }

    Wh_Log(L"Tearing supported: %d", tearing);
    supported = tearing ? 1 : 0;
    return tearing;
}

bool IsD3D11Device(IUnknown* device) {
    ID3D11Device* device11 = nullptr;
    if (!device || FAILED(device->QueryInterface(__uuidof(ID3D11Device),
                                                 (void**)&device11))) {
        return false;
    }
    device11->Release();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// sRGB format bookkeeping

DXGI_FORMAT GetSrgbFormat(IDXGISwapChain* swapChain) {
    DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
    UINT size = sizeof(format);
    if (FAILED(swapChain->GetPrivateData(kSrgbFormatGuid, &size, &format)) ||
        size != sizeof(format)) {
        return DXGI_FORMAT_UNKNOWN;
    }
    return format;
}

// Kept on the swap chain for resizing, and on its back buffer for
// CreateRenderTargetView_Hook. DXGI_FORMAT_UNKNOWN removes it.
void SetSrgbFormat(IDXGISwapChain* swapChain, DXGI_FORMAT srgbFormat) {
    if (srgbFormat == DXGI_FORMAT_UNKNOWN) {
        swapChain->SetPrivateData(kSrgbFormatGuid, 0, nullptr);
        return;
    }

    swapChain->SetPrivateData(kSrgbFormatGuid, sizeof(srgbFormat),
                              &srgbFormat);

    // With Direct3D 11, buffer 0 is the one the game renders to, the flip
    // model rotates the buffers behind it.
    ID3D11Texture2D* backBuffer = nullptr;
    if (SUCCEEDED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                                       (void**)&backBuffer))) {
        backBuffer->SetPrivateData(kSrgbFormatGuid, sizeof(srgbFormat),
                                   &srgbFormat);
        backBuffer->Release();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Device hooks

// A render target view of a back buffer whose sRGB format was stripped gets it
// back. The flip model allows sRGB views of its non-sRGB buffers.
using CreateRenderTargetView_t =
    HRESULT(STDMETHODCALLTYPE*)(ID3D11Device*,
                                ID3D11Resource*,
                                const D3D11_RENDER_TARGET_VIEW_DESC*,
                                ID3D11RenderTargetView**);
HookedMethod<CreateRenderTargetView_t> g_createRenderTargetView;

template <int N>
HRESULT STDMETHODCALLTYPE
CreateRenderTargetView_Hook(ID3D11Device* device,
                            ID3D11Resource* resource,
                            const D3D11_RENDER_TARGET_VIEW_DESC* desc,
                            ID3D11RenderTargetView** view) {
    D3D11_RENDER_TARGET_VIEW_DESC srgbDesc;
    if (resource && (!desc || desc->Format == DXGI_FORMAT_UNKNOWN)) {
        DXGI_FORMAT srgbFormat = DXGI_FORMAT_UNKNOWN;
        UINT size = sizeof(srgbFormat);
        if (SUCCEEDED(resource->GetPrivateData(kSrgbFormatGuid, &size,
                                               &srgbFormat)) &&
            size == sizeof(srgbFormat)) {
            if (desc) {
                srgbDesc = *desc;
            } else {
                srgbDesc = {};
                srgbDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            }
            srgbDesc.Format = srgbFormat;
            desc = &srgbDesc;
        }
    }

    return g_createRenderTargetView.originals[N](device, resource, desc, view);
}

constexpr CreateRenderTargetView_t
    kCreateRenderTargetViewHooks[kMaxImplementations] = {
        CreateRenderTargetView_Hook<0>, CreateRenderTargetView_Hook<1>,
        CreateRenderTargetView_Hook<2>, CreateRenderTargetView_Hook<3>};

void EnsureDeviceHooks(IUnknown* device) {
    ID3D11Device* device11 = nullptr;
    if (!device || FAILED(device->QueryInterface(__uuidof(ID3D11Device),
                                                 (void**)&device11))) {
        return;
    }

    bool hooked;
    {
        std::lock_guard<std::mutex> guard(g_hookMutex);
        hooked = HookImplementation(
            GetVtbl(device11)[kSlotDeviceCreateRenderTargetView],
            g_createRenderTargetView, kCreateRenderTargetViewHooks,
            L"ID3D11Device::CreateRenderTargetView");
        if (hooked) {
            Wh_ApplyHookOperations();
        }
    }
    device11->Release();
}

// Frame latency of the device, used for swap chains that aren't waitable.
using SetMaximumFrameLatency_t = HRESULT(STDMETHODCALLTYPE*)(IDXGIDevice1*,
                                                             UINT);
HookedMethod<SetMaximumFrameLatency_t> g_setMaximumFrameLatency;

template <int N>
HRESULT STDMETHODCALLTYPE SetMaximumFrameLatency_Hook(IDXGIDevice1* device,
                                                      UINT latency) {
    int forced = g_maxFrameLatency;
    if (forced > 0) {
        latency = forced;
    }
    return g_setMaximumFrameLatency.originals[N](device, latency);
}

constexpr SetMaximumFrameLatency_t
    kSetMaximumFrameLatencyHooks[kMaxImplementations] = {
        SetMaximumFrameLatency_Hook<0>, SetMaximumFrameLatency_Hook<1>,
        SetMaximumFrameLatency_Hook<2>, SetMaximumFrameLatency_Hook<3>};

// The game may set its own latency later, the hook keeps the forced one.
void ApplyDeviceFrameLatency(IUnknown* device) {
    int latency = g_maxFrameLatency;
    IDXGIDevice1* dxgiDevice = nullptr;
    if (latency <= 0 || !device ||
        FAILED(device->QueryInterface(__uuidof(IDXGIDevice1),
                                      (void**)&dxgiDevice))) {
        return;
    }

    {
        std::lock_guard<std::mutex> guard(g_hookMutex);
        if (HookImplementation(
                GetVtbl(dxgiDevice)[kSlotDxgiDeviceSetMaximumFrameLatency],
                g_setMaximumFrameLatency, kSetMaximumFrameLatencyHooks,
                L"IDXGIDevice1::SetMaximumFrameLatency")) {
            Wh_ApplyHookOperations();
        }
    }

    HRESULT hr = dxgiDevice->SetMaximumFrameLatency(latency);
    Wh_Log(L"Device frame latency %d: 0x%08X", latency, hr);
    dxgiDevice->Release();
}

////////////////////////////////////////////////////////////////////////////////
// Swap chain hooks

UINT AdjustPresentFlags(IDXGISwapChain* swapChain, UINT interval, UINT flags) {
    bool force = g_forceAllowTearing && interval == 0;
    if (!force && !(flags & DXGI_PRESENT_ALLOW_TEARING)) {
        return flags;
    }

    // Only valid for swap chains created for it, and never in exclusive
    // fullscreen. A faked fullscreen is a window.
    DXGI_SWAP_CHAIN_DESC desc;
    BOOL fullscreen = FALSE;
    bool allowed = SUCCEEDED(swapChain->GetDesc(&desc)) &&
                   (desc.Flags & DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING) &&
                   (g_fakeFullscreenSwapChains.Contains(swapChain) ||
                    (SUCCEEDED(swapChain->GetFullscreenState(&fullscreen,
                                                             nullptr)) &&
                     !fullscreen));
    if (!allowed) {
        return flags & ~DXGI_PRESENT_ALLOW_TEARING;
    }
    return force ? flags | DXGI_PRESENT_ALLOW_TEARING : flags;
}

// The flip model unbinds the back buffer during Present. Games written for the
// blt model may set their render targets once, so they're put back afterwards,
// as Special K does.
template <typename F>
HRESULT PresentCommon(IDXGISwapChain* swapChain,
                      UINT interval,
                      UINT flags,
                      F callOriginal) {
    if (flags & DXGI_PRESENT_TEST) {
        return callOriginal(flags);
    }

    flags = AdjustPresentFlags(swapChain, interval, flags);

    // The start of the next frame, it waits until the queue has room.
    auto waitForQueue = [&](HRESULT hr) {
        if (HANDLE waitable = GetWaitable(swapChain)) {
            WaitForSingleObject(waitable, 1000);
        }
        return hr;
    };

    if ((flags & DXGI_PRESENT_DO_NOT_SEQUENCE) ||
        !g_upgradedSwapChains.Contains(swapChain)) {
        return waitForQueue(callOriginal(flags));
    }

    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    ID3D11RenderTargetView* views[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
    ID3D11DepthStencilView* depthView = nullptr;
    if (SUCCEEDED(swapChain->GetDevice(__uuidof(ID3D11Device),
                                       (void**)&device))) {
        device->GetImmediateContext(&context);
        if (context) {
            context->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
                                        views, &depthView);
        }
    }

    HRESULT hr = callOriginal(flags);

    if (context) {
        // Only the slots the game used, pixel shader UAVs share the others.
        UINT count = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
        while (count > 0 && !views[count - 1]) {
            count--;
        }
        if (count > 0 || depthView) {
            context->OMSetRenderTargets(count, views, depthView);
        }
        for (auto*& view : views) {
            SafeRelease(view);
        }
        SafeRelease(depthView);
        SafeRelease(context);
    }
    SafeRelease(device);

    return waitForQueue(hr);
}

using Present_t = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain*, UINT, UINT);
HookedMethod<Present_t> g_present;

template <int N>
HRESULT STDMETHODCALLTYPE Present_Hook(IDXGISwapChain* swapChain,
                                       UINT interval,
                                       UINT flags) {
    return PresentCommon(swapChain, interval, flags, [&](UINT newFlags) {
        return g_present.originals[N](swapChain, interval, newFlags);
    });
}

constexpr Present_t kPresentHooks[kMaxImplementations] = {
    Present_Hook<0>, Present_Hook<1>, Present_Hook<2>, Present_Hook<3>};

using Present1_t =
    HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain1*,
                                UINT,
                                UINT,
                                const DXGI_PRESENT_PARAMETERS*);
HookedMethod<Present1_t> g_present1;

template <int N>
HRESULT STDMETHODCALLTYPE Present1_Hook(IDXGISwapChain1* swapChain,
                                        UINT interval,
                                        UINT flags,
                                        const DXGI_PRESENT_PARAMETERS* params) {
    return PresentCommon(swapChain, interval, flags, [&](UINT newFlags) {
        return g_present1.originals[N](swapChain, interval, newFlags, params);
    });
}

constexpr Present1_t kPresent1Hooks[kMaxImplementations] = {
    Present1_Hook<0>, Present1_Hook<1>, Present1_Hook<2>, Present1_Hook<3>};

// Keeps a resize consistent with the changes made when the swap chain was
// created.
struct ResizeChange {
    bool upgraded;
    DXGI_FORMAT srgbFormat;
};

ResizeChange PrepareResize(IDXGISwapChain* swapChain,
                           UINT* bufferCount,
                           DXGI_FORMAT* format,
                           UINT* flags) {
    ResizeChange change = {g_upgradedSwapChains.Contains(swapChain),
                           DXGI_FORMAT_UNKNOWN};

    // The tearing and waitable flags can't change with a resize, and the game
    // may not know they were added.
    constexpr UINT kFixedFlags =
        DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING |
        DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
    DXGI_SWAP_CHAIN_DESC desc;
    if (SUCCEEDED(swapChain->GetDesc(&desc))) {
        *flags |= desc.Flags & kFixedFlags;
    }

    if (change.upgraded) {
        // 0 keeps the count, the flip model needs at least 2.
        if (*bufferCount == 1) {
            *bufferCount = 2;
        }

        // DXGI_FORMAT_UNKNOWN keeps the format.
        change.srgbFormat = GetSrgbFormat(swapChain);
        if (*format != DXGI_FORMAT_UNKNOWN) {
            DXGI_FORMAT stripped = StripSrgb(*format);
            change.srgbFormat =
                stripped != *format ? *format : DXGI_FORMAT_UNKNOWN;
            *format = stripped;
        }
    }

    return change;
}

void FinishResize(IDXGISwapChain* swapChain,
                  HRESULT hr,
                  const ResizeChange& change) {
    if (SUCCEEDED(hr) && change.upgraded) {
        // The back buffer is a new one.
        SetSrgbFormat(swapChain, change.srgbFormat);
    } else if (FAILED(hr)) {
        Wh_Log(L"ResizeBuffers failed: 0x%08X", hr);
    }
}

using ResizeBuffers_t = HRESULT(
    STDMETHODCALLTYPE*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
HookedMethod<ResizeBuffers_t> g_resizeBuffers;

template <int N>
HRESULT STDMETHODCALLTYPE ResizeBuffers_Hook(IDXGISwapChain* swapChain,
                                             UINT bufferCount,
                                             UINT width,
                                             UINT height,
                                             DXGI_FORMAT format,
                                             UINT flags) {
    ResizeChange change =
        PrepareResize(swapChain, &bufferCount, &format, &flags);
    HRESULT hr = g_resizeBuffers.originals[N](swapChain, bufferCount, width,
                                              height, format, flags);
    FinishResize(swapChain, hr, change);
    return hr;
}

constexpr ResizeBuffers_t kResizeBuffersHooks[kMaxImplementations] = {
    ResizeBuffers_Hook<0>, ResizeBuffers_Hook<1>, ResizeBuffers_Hook<2>,
    ResizeBuffers_Hook<3>};

using ResizeBuffers1_t = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain3*,
                                                     UINT,
                                                     UINT,
                                                     UINT,
                                                     DXGI_FORMAT,
                                                     UINT,
                                                     const UINT*,
                                                     IUnknown* const*);
HookedMethod<ResizeBuffers1_t> g_resizeBuffers1;

template <int N>
HRESULT STDMETHODCALLTYPE ResizeBuffers1_Hook(IDXGISwapChain3* swapChain,
                                              UINT bufferCount,
                                              UINT width,
                                              UINT height,
                                              DXGI_FORMAT format,
                                              UINT flags,
                                              const UINT* nodeMasks,
                                              IUnknown* const* queues) {
    ResizeChange change =
        PrepareResize(swapChain, &bufferCount, &format, &flags);
    HRESULT hr = g_resizeBuffers1.originals[N](
        swapChain, bufferCount, width, height, format, flags, nodeMasks,
        queues);
    FinishResize(swapChain, hr, change);
    return hr;
}

constexpr ResizeBuffers1_t kResizeBuffers1Hooks[kMaxImplementations] = {
    ResizeBuffers1_Hook<0>, ResizeBuffers1_Hook<1>, ResizeBuffers1_Hook<2>,
    ResizeBuffers1_Hook<3>};

// With Force borderless, exclusive fullscreen is a borderless window, and the
// game is told it's in fullscreen.
using SetFullscreenState_t = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain*,
                                                         BOOL,
                                                         IDXGIOutput*);
HookedMethod<SetFullscreenState_t> g_setFullscreenState;

template <int N>
HRESULT STDMETHODCALLTYPE SetFullscreenState_Hook(IDXGISwapChain* swapChain,
                                                  BOOL fullscreen,
                                                  IDXGIOutput* target) {
    // Real exclusive fullscreen, entered before Force borderless was turned
    // on, stays real, so that the game can leave it.
    BOOL realFullscreen = FALSE;
    if (fullscreen && g_forceBorderless &&
        !g_fakeFullscreenSwapChains.Contains(swapChain)) {
        swapChain->GetFullscreenState(&realFullscreen, nullptr);
    }

    if (fullscreen && g_forceBorderless && !realFullscreen) {
        g_fakeFullscreenSwapChains.Set(swapChain, true);
        PostWindowMessage(kMakeBorderlessMessage,
                          GetSwapChainWindow(swapChain));
        Wh_Log(L"Swap chain %p: borderless instead of exclusive fullscreen",
               swapChain);
        return S_OK;
    }

    if (!fullscreen && g_fakeFullscreenSwapChains.Contains(swapChain)) {
        g_fakeFullscreenSwapChains.Set(swapChain, false);
        PostWindowMessage(kRestoreWindowMessage,
                          GetSwapChainWindow(swapChain));
        return S_OK;
    }

    return g_setFullscreenState.originals[N](swapChain, fullscreen, target);
}

constexpr SetFullscreenState_t kSetFullscreenStateHooks[kMaxImplementations] =
    {SetFullscreenState_Hook<0>, SetFullscreenState_Hook<1>,
     SetFullscreenState_Hook<2>, SetFullscreenState_Hook<3>};

using GetFullscreenState_t = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain*,
                                                         BOOL*,
                                                         IDXGIOutput**);
HookedMethod<GetFullscreenState_t> g_getFullscreenState;

template <int N>
HRESULT STDMETHODCALLTYPE GetFullscreenState_Hook(IDXGISwapChain* swapChain,
                                                  BOOL* fullscreen,
                                                  IDXGIOutput** target) {
    HRESULT hr =
        g_getFullscreenState.originals[N](swapChain, fullscreen, target);
    if (SUCCEEDED(hr) && fullscreen && !*fullscreen &&
        g_fakeFullscreenSwapChains.Contains(swapChain)) {
        *fullscreen = TRUE;
        if (target && !*target) {
            swapChain->GetContainingOutput(target);
        }
    }
    return hr;
}

constexpr GetFullscreenState_t kGetFullscreenStateHooks[kMaxImplementations] =
    {GetFullscreenState_Hook<0>, GetFullscreenState_Hook<1>,
     GetFullscreenState_Hook<2>, GetFullscreenState_Hook<3>};

// Resizing the target of a windowed swap chain resizes its window, which must
// keep covering the monitor.
using ResizeTarget_t = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain*,
                                                   const DXGI_MODE_DESC*);
HookedMethod<ResizeTarget_t> g_resizeTarget;

template <int N>
HRESULT STDMETHODCALLTYPE ResizeTarget_Hook(IDXGISwapChain* swapChain,
                                            const DXGI_MODE_DESC* mode) {
    if (g_fakeFullscreenSwapChains.Contains(swapChain)) {
        return S_OK;
    }
    return g_resizeTarget.originals[N](swapChain, mode);
}

constexpr ResizeTarget_t kResizeTargetHooks[kMaxImplementations] = {
    ResizeTarget_Hook<0>, ResizeTarget_Hook<1>, ResizeTarget_Hook<2>,
    ResizeTarget_Hook<3>};

void EnsureSwapChainHooks(IDXGISwapChain* swapChain) {
    std::lock_guard<std::mutex> guard(g_hookMutex);

    void** vtbl = GetVtbl(swapChain);
    bool hooked = false;
    hooked = HookImplementation(vtbl[kSlotSwapChainPresent], g_present,
                                kPresentHooks, L"IDXGISwapChain::Present") ||
             hooked;
    hooked = HookImplementation(vtbl[kSlotSwapChainResizeBuffers],
                                g_resizeBuffers, kResizeBuffersHooks,
                                L"IDXGISwapChain::ResizeBuffers") ||
             hooked;
    hooked = HookImplementation(vtbl[kSlotSwapChainSetFullscreenState],
                                g_setFullscreenState, kSetFullscreenStateHooks,
                                L"IDXGISwapChain::SetFullscreenState") ||
             hooked;
    hooked = HookImplementation(vtbl[kSlotSwapChainGetFullscreenState],
                                g_getFullscreenState, kGetFullscreenStateHooks,
                                L"IDXGISwapChain::GetFullscreenState") ||
             hooked;
    hooked = HookImplementation(vtbl[kSlotSwapChainResizeTarget],
                                g_resizeTarget, kResizeTargetHooks,
                                L"IDXGISwapChain::ResizeTarget") ||
             hooked;

    IDXGISwapChain1* swapChain1 = nullptr;
    if (SUCCEEDED(swapChain->QueryInterface(__uuidof(IDXGISwapChain1),
                                            (void**)&swapChain1))) {
        hooked = HookImplementation(GetVtbl(swapChain1)[kSlotSwapChainPresent1],
                                    g_present1, kPresent1Hooks,
                                    L"IDXGISwapChain1::Present1") ||
                 hooked;
        SafeRelease(swapChain1);
    }

    IDXGISwapChain3* swapChain3 = nullptr;
    if (SUCCEEDED(swapChain->QueryInterface(__uuidof(IDXGISwapChain3),
                                            (void**)&swapChain3))) {
        hooked = HookImplementation(
                     GetVtbl(swapChain3)[kSlotSwapChainResizeBuffers1],
                     g_resizeBuffers1, kResizeBuffers1Hooks,
                     L"IDXGISwapChain3::ResizeBuffers1") ||
                 hooked;
        SafeRelease(swapChain3);
    }

    if (hooked) {
        Wh_ApplyHookOperations();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Swap chain creation

struct SwapChainChange {
    // The description was changed.
    bool changed = false;
    // Changed from the blt model to the flip model.
    bool upgraded = false;
    // The format the game asked for, if it had to become the non-sRGB one.
    DXGI_FORMAT srgbFormat = DXGI_FORMAT_UNKNOWN;
    // The waitable flag was added, the mod waits on the swap chain.
    bool waitableAdded = false;
    // Exclusive fullscreen was requested, it's created windowed instead.
    bool fakeFullscreen = false;
};

SwapChainChange AdjustSwapChain(IUnknown* factory,
                                IUnknown* device,
                                DXGI_SWAP_EFFECT* swapEffect,
                                UINT* bufferCount,
                                DXGI_FORMAT* format,
                                UINT sampleCount,
                                UINT* flags,
                                BOOL* windowed) {
    SwapChainChange change;

    if (g_forceBorderless && !*windowed) {
        *windowed = TRUE;
        change.fakeFullscreen = true;
        change.changed = true;
    }

    if (g_forceFlipModel && !IsFlipModel(*swapEffect)) {
        DXGI_FORMAT stripped = StripSrgb(*format);

        PCWSTR reason = nullptr;
        if (!IsD3D11Device(device)) {
            reason = L"not a Direct3D 11 device";
        } else if (!*windowed) {
            reason = L"created in exclusive fullscreen";
        } else if (sampleCount > 1) {
            reason = L"multisampled back buffer";
        } else if (!IsFlipModelFormat(stripped)) {
            reason = L"back buffer format";
        }

        if (reason) {
            Wh_Log(L"Not using the flip model (%s): swap effect %d, format %d, "
                   L"samples %u",
                   reason, *swapEffect, *format, sampleCount);
        } else {
            *swapEffect = *swapEffect == DXGI_SWAP_EFFECT_SEQUENTIAL
                              ? DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL
                              : DXGI_SWAP_EFFECT_FLIP_DISCARD;
            if (*bufferCount < 2) {
                *bufferCount = 2;
            }
            if (stripped != *format) {
                change.srgbFormat = *format;
                *format = stripped;
            }
            change.changed = true;
            change.upgraded = true;
        }
    }

    if (g_forceAllowTearing && IsFlipModel(*swapEffect) &&
        !(*flags & DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING) &&
        IsTearingSupported(factory)) {
        *flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
        change.changed = true;
    }

    // A game that made it waitable itself waits on it itself.
    if (g_waitableSwapChain && IsFlipModel(*swapEffect) &&
        !(*flags & DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT)) {
        *flags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        change.waitableAdded = true;
        change.changed = true;
    }

    if (change.changed) {
        Wh_Log(L"Swap chain: swap effect %d, buffers %u, format %d, flags 0x%X",
               *swapEffect, *bufferCount, *format, *flags);
    }

    return change;
}

void OnSwapChainCreated(IDXGISwapChain* swapChain,
                        IUnknown* device,
                        const SwapChainChange& change) {
    EnsureSwapChainHooks(swapChain);
    g_upgradedSwapChains.Set(swapChain, change.upgraded);
    g_fakeFullscreenSwapChains.Set(swapChain, change.fakeFullscreen);
    ApplyDeviceFrameLatency(device);

    HANDLE waitable = nullptr;
    IDXGISwapChain2* swapChain2 = nullptr;
    if (change.waitableAdded &&
        SUCCEEDED(swapChain->QueryInterface(__uuidof(IDXGISwapChain2),
                                            (void**)&swapChain2))) {
        int latency = g_maxFrameLatency;
        swapChain2->SetMaximumFrameLatency(latency > 0 ? latency : 1);
        waitable = swapChain2->GetFrameLatencyWaitableObject();
        SafeRelease(swapChain2);
        Wh_Log(L"Swap chain %p is waitable, latency %d", swapChain,
               latency > 0 ? latency : 1);
    }
    SetWaitable(swapChain, waitable);

    if (change.fakeFullscreen) {
        HWND hWnd = GetSwapChainWindow(swapChain);

        // DXGI's own Alt+Enter would switch to exclusive fullscreen.
        IDXGIFactory* factory = nullptr;
        if (SUCCEEDED(swapChain->GetParent(__uuidof(IDXGIFactory),
                                           (void**)&factory))) {
            factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
            SafeRelease(factory);
        }

        PostWindowMessage(kMakeBorderlessMessage, hWnd);
        Wh_Log(L"Swap chain %p: borderless instead of exclusive fullscreen",
               swapChain);
    }

    if (change.srgbFormat != DXGI_FORMAT_UNKNOWN) {
        // Before the game creates its render target views.
        EnsureDeviceHooks(device);
        SetSrgbFormat(swapChain, change.srgbFormat);
    }

    if (change.upgraded) {
        Wh_Log(L"Swap chain %p uses the flip model", swapChain);
    }
}

using CreateSwapChain_t = HRESULT(STDMETHODCALLTYPE*)(IDXGIFactory*,
                                                      IUnknown*,
                                                      DXGI_SWAP_CHAIN_DESC*,
                                                      IDXGISwapChain**);
HookedMethod<CreateSwapChain_t> g_createSwapChain;

template <int N>
HRESULT STDMETHODCALLTYPE CreateSwapChain_Hook(IDXGIFactory* factory,
                                               IUnknown* device,
                                               DXGI_SWAP_CHAIN_DESC* desc,
                                               IDXGISwapChain** swapChain) {
    auto& original = g_createSwapChain.originals[N];
    if (g_inCreateSwapChain || !desc || !swapChain) {
        return original(factory, device, desc, swapChain);
    }

    DXGI_SWAP_CHAIN_DESC local = *desc;
    SwapChainChange change = AdjustSwapChain(
        factory, device, &local.SwapEffect, &local.BufferCount,
        &local.BufferDesc.Format, local.SampleDesc.Count, &local.Flags,
        &local.Windowed);

    g_inCreateSwapChain = true;
    HRESULT hr = original(factory, device, change.changed ? &local : desc,
                          swapChain);
    if (FAILED(hr) && change.changed) {
        Wh_Log(L"CreateSwapChain with the changes failed (0x%08X), creating "
               L"it as the game asked",
               hr);
        change = {};
        hr = original(factory, device, desc, swapChain);
    }
    g_inCreateSwapChain = false;

    if (SUCCEEDED(hr) && *swapChain) {
        OnSwapChainCreated(*swapChain, device, change);
    }
    return hr;
}

constexpr CreateSwapChain_t kCreateSwapChainHooks[kMaxImplementations] = {
    CreateSwapChain_Hook<0>, CreateSwapChain_Hook<1>, CreateSwapChain_Hook<2>,
    CreateSwapChain_Hook<3>};

using CreateSwapChainForHwnd_t =
    HRESULT(STDMETHODCALLTYPE*)(IDXGIFactory2*,
                                IUnknown*,
                                HWND,
                                const DXGI_SWAP_CHAIN_DESC1*,
                                const DXGI_SWAP_CHAIN_FULLSCREEN_DESC*,
                                IDXGIOutput*,
                                IDXGISwapChain1**);
HookedMethod<CreateSwapChainForHwnd_t> g_createSwapChainForHwnd;

template <int N>
HRESULT STDMETHODCALLTYPE
CreateSwapChainForHwnd_Hook(IDXGIFactory2* factory,
                            IUnknown* device,
                            HWND hWnd,
                            const DXGI_SWAP_CHAIN_DESC1* desc,
                            const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* fullscreenDesc,
                            IDXGIOutput* output,
                            IDXGISwapChain1** swapChain) {
    auto& original = g_createSwapChainForHwnd.originals[N];
    if (g_inCreateSwapChain || !desc || !swapChain) {
        return original(factory, device, hWnd, desc, fullscreenDesc, output,
                        swapChain);
    }

    DXGI_SWAP_CHAIN_DESC1 local = *desc;
    BOOL windowed = !fullscreenDesc || fullscreenDesc->Windowed;
    SwapChainChange change = AdjustSwapChain(
        factory, device, &local.SwapEffect, &local.BufferCount, &local.Format,
        local.SampleDesc.Count, &local.Flags, &windowed);

    // No fullscreen description creates it windowed.
    g_inCreateSwapChain = true;
    HRESULT hr = original(factory, device, hWnd, change.changed ? &local : desc,
                          change.fakeFullscreen ? nullptr : fullscreenDesc,
                          output, swapChain);
    if (FAILED(hr) && change.changed) {
        Wh_Log(L"CreateSwapChainForHwnd with the changes failed (0x%08X), "
               L"creating it as the game asked",
               hr);
        change = {};
        hr = original(factory, device, hWnd, desc, fullscreenDesc, output,
                      swapChain);
    }
    g_inCreateSwapChain = false;

    if (SUCCEEDED(hr) && *swapChain) {
        OnSwapChainCreated(*swapChain, device, change);
    }
    return hr;
}

constexpr CreateSwapChainForHwnd_t
    kCreateSwapChainForHwndHooks[kMaxImplementations] = {
        CreateSwapChainForHwnd_Hook<0>, CreateSwapChainForHwnd_Hook<1>,
        CreateSwapChainForHwnd_Hook<2>, CreateSwapChainForHwnd_Hook<3>};

// Must be called with g_hookMutex held. Returns true if a hook was set.
bool HookFactoryLocked(IUnknown* factory) {
    bool hooked = false;

    IDXGIFactory* factory1 = nullptr;
    if (SUCCEEDED(factory->QueryInterface(__uuidof(IDXGIFactory),
                                          (void**)&factory1))) {
        hooked = HookImplementation(
                     GetVtbl(factory1)[kSlotFactoryCreateSwapChain],
                     g_createSwapChain, kCreateSwapChainHooks,
                     L"IDXGIFactory::CreateSwapChain") ||
                 hooked;
        SafeRelease(factory1);
    }

    IDXGIFactory2* factory2 = nullptr;
    if (SUCCEEDED(factory->QueryInterface(__uuidof(IDXGIFactory2),
                                          (void**)&factory2))) {
        hooked = HookImplementation(
                     GetVtbl(factory2)[kSlotFactoryCreateSwapChainForHwnd],
                     g_createSwapChainForHwnd, kCreateSwapChainForHwndHooks,
                     L"IDXGIFactory2::CreateSwapChainForHwnd") ||
                 hooked;
        SafeRelease(factory2);
    }

    return hooked;
}

void EnsureFactoryHooks(void* factory) {
    if (!factory) {
        return;
    }
    std::lock_guard<std::mutex> guard(g_hookMutex);
    if (HookFactoryLocked((IUnknown*)factory)) {
        Wh_ApplyHookOperations();
    }
}

////////////////////////////////////////////////////////////////////////////////
// dxgi.dll exports

// A factory the game creates may use another implementation than the one
// found when dxgi.dll was loaded.

using CreateDXGIFactory_t = HRESULT(WINAPI*)(REFIID, void**);
CreateDXGIFactory_t CreateDXGIFactory_Original;
HRESULT WINAPI CreateDXGIFactory_Hook(REFIID riid, void** factory) {
    HRESULT hr = CreateDXGIFactory_Original(riid, factory);
    if (SUCCEEDED(hr) && factory) {
        EnsureFactoryHooks(*factory);
    }
    return hr;
}

CreateDXGIFactory_t CreateDXGIFactory1_Original;
HRESULT WINAPI CreateDXGIFactory1_Hook(REFIID riid, void** factory) {
    HRESULT hr = CreateDXGIFactory1_Original(riid, factory);
    if (SUCCEEDED(hr) && factory) {
        EnsureFactoryHooks(*factory);
    }
    return hr;
}

using CreateDXGIFactory2_t = HRESULT(WINAPI*)(UINT, REFIID, void**);
CreateDXGIFactory2_t CreateDXGIFactory2_Original;
HRESULT WINAPI CreateDXGIFactory2_Hook(UINT flags, REFIID riid, void** factory) {
    HRESULT hr = CreateDXGIFactory2_Original(flags, riid, factory);
    if (SUCCEEDED(hr) && factory) {
        EnsureFactoryHooks(*factory);
    }
    return hr;
}

// Where the system DLL is: System32, or SysWOW64, under which 32-bit processes
// may have it recorded. Empty if unknown.
std::wstring GetSystemDllPath(bool wow64, PCWSTR name) {
    WCHAR dir[MAX_PATH];
    UINT len = wow64 ? GetSystemWow64DirectoryW(dir, ARRAYSIZE(dir))
                     : GetSystemDirectoryW(dir, ARRAYSIZE(dir));
    if (!len || len >= ARRAYSIZE(dir)) {
        return std::wstring();
    }
    return std::wstring(dir) + L"\\" + name;
}

// Only the system dxgi.dll is hooked. It's never loaded by the mod, as that
// would make the loader skip a dxgi.dll proxy in the game's folder.
HMODULE GetSystemModule(PCWSTR name) {
    for (bool wow64 : {false, true}) {
        std::wstring path = GetSystemDllPath(wow64, name);
        if (!path.empty()) {
            if (HMODULE module = GetModuleHandleW(path.c_str())) {
                return module;
            }
        }
    }
    return nullptr;
}

// Must be called with g_hookMutex held. Returns true if new hooks were set and
// need to be applied.
bool HookDxgiLocked() {
    if (g_dxgiHooked || g_unloading) {
        return false;
    }

    HMODULE module = GetSystemModule(L"dxgi.dll");
    if (!module) {
        return false;
    }
    g_dxgiHooked = true;

    auto createFactory =
        (CreateDXGIFactory_t)GetProcAddress(module, "CreateDXGIFactory");
    auto createFactory1 =
        (CreateDXGIFactory_t)GetProcAddress(module, "CreateDXGIFactory1");
    auto createFactory2 =
        (CreateDXGIFactory2_t)GetProcAddress(module, "CreateDXGIFactory2");
    if (!createFactory || !createFactory1) {
        Wh_Log(L"dxgi.dll exports not found");
        return false;
    }

    // The hooked code must stay around while the mod is loaded.
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                            (LPCWSTR)module, &g_dxgiModule)) {
        Wh_Log(L"Referencing dxgi.dll failed: %u", GetLastError());
    }

    // Factories, including the one Direct3D 11 creates with
    // CreateDXGIFactory2, are hooked when created. When dxgi.dll was already
    // loaded, the game may have one already, so the implementation is hooked
    // right away. The exports aren't hooked yet, this calls the original.
    IDXGIFactory1* factory = nullptr;
    HRESULT hr = createFactory1(__uuidof(IDXGIFactory1), (void**)&factory);
    if (SUCCEEDED(hr) && factory) {
        HookFactoryLocked(factory);
        SafeRelease(factory);
    } else {
        Wh_Log(L"CreateDXGIFactory1 failed: 0x%08X", hr);
    }

    WindhawkUtils::SetFunctionHook(createFactory, CreateDXGIFactory_Hook,
                                   &CreateDXGIFactory_Original);
    WindhawkUtils::SetFunctionHook(createFactory1, CreateDXGIFactory1_Hook,
                                   &CreateDXGIFactory1_Original);
    // Windows 8.1 and newer.
    if (createFactory2) {
        WindhawkUtils::SetFunctionHook(createFactory2, CreateDXGIFactory2_Hook,
                                       &CreateDXGIFactory2_Original);
    }

    Wh_Log(L"Hooked dxgi.dll (%p)", module);
    return true;
}

// Returns true if new hooks were set and need to be applied. Hooking calls into
// dxgi.dll, which another thread may still be loading. Taking a reference waits
// until the loader is done with it. That happens before g_hookMutex is taken,
// so that the loader lock is never waited for while it's held.
bool HookDxgiIfLoaded() {
    HMODULE reference = nullptr;
    if (!g_dxgiHooked) {
        if (HMODULE module = GetSystemModule(L"dxgi.dll")) {
            WCHAR path[MAX_PATH];
            if (GetModuleFileNameW(module, path, ARRAYSIZE(path))) {
                reference = LoadLibraryExW(path, nullptr, 0);
            }
        }
    }

    bool hooked;
    {
        std::lock_guard<std::mutex> guard(g_hookMutex);
        hooked = HookDxgiLocked();
    }

    if (reference) {
        FreeLibrary(reference);
    }
    return hooked;
}

////////////////////////////////////////////////////////////////////////////////
// Waiting for dxgi.dll
//
// Hooking LoadLibraryExW to notice dxgi.dll being loaded makes games with
// anti-tamper protection hang, e.g. Warcraft III. A DLL load notification
// patches no code. It's called with the loader lock held, so it only signals
// the hook thread, which hooks dxgi.dll right after.

struct LdrUnicodeString {
    USHORT length;
    USHORT maximumLength;
    PWSTR buffer;
};

struct LdrDllLoadedData {
    ULONG flags;
    const LdrUnicodeString* fullDllName;
    const LdrUnicodeString* baseDllName;
    PVOID dllBase;
    ULONG sizeOfImage;
};

constexpr ULONG kLdrDllLoaded = 1;

using LdrDllNotification_t = VOID(CALLBACK*)(ULONG,
                                             const LdrDllLoadedData*,
                                             PVOID);
using LdrRegisterDllNotification_t = LONG(NTAPI*)(ULONG,
                                                  LdrDllNotification_t,
                                                  PVOID,
                                                  PVOID*);
using LdrUnregisterDllNotification_t = LONG(NTAPI*)(PVOID);

PVOID g_dllNotificationCookie;
HANDLE g_dxgiLoadedEvent;
HANDLE g_hookThreadStopEvent;
HANDLE g_hookThread;

// The paths the system dxgi.dll may be recorded under. Set before the
// notification is registered, so that the callback only compares strings.
std::wstring g_systemDllPaths[2];
std::atomic<int> g_loadedPathIndex{-1};

bool PathEquals(const LdrUnicodeString& string, const std::wstring& path) {
    return !path.empty() && string.length == path.size() * sizeof(WCHAR) &&
           _wcsnicmp(string.buffer, path.c_str(), path.size()) == 0;
}

VOID CALLBACK OnDllNotification(ULONG reason,
                                const LdrDllLoadedData* data,
                                PVOID context) {
    if (reason != kLdrDllLoaded || !data || !data->fullDllName ||
        !data->fullDllName->buffer) {
        return;
    }

    // Only the system dxgi.dll, not a proxy of the game with the same name.
    for (int i = 0; i < (int)ARRAYSIZE(g_systemDllPaths); i++) {
        if (PathEquals(*data->fullDllName, g_systemDllPaths[i])) {
            g_loadedPathIndex = i;
            SetEvent(g_dxgiLoadedEvent);
            return;
        }
    }
}

DWORD WINAPI HookThreadProc(void* parameter) {
    HANDLE events[] = {g_hookThreadStopEvent, g_dxgiLoadedEvent};
    while (!g_dxgiHooked) {
        if (WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE,
                                   INFINITE) != WAIT_OBJECT_0 + 1) {
            break;
        }

        // It's already loaded, so this only waits until the loader is done
        // with it, so that it's initialized, and keeps it loaded meanwhile.
        HMODULE reference = LoadLibraryExW(
            g_systemDllPaths[g_loadedPathIndex].c_str(), nullptr, 0);
        if (!reference) {
            continue;
        }

        if (HookDxgiIfLoaded()) {
            Wh_ApplyHookOperations();
        }
        FreeLibrary(reference);
    }
    return 0;
}

void CloseWatchEvents() {
    if (g_dxgiLoadedEvent) {
        CloseHandle(g_dxgiLoadedEvent);
        g_dxgiLoadedEvent = nullptr;
    }
    if (g_hookThreadStopEvent) {
        CloseHandle(g_hookThreadStopEvent);
        g_hookThreadStopEvent = nullptr;
    }
}

bool StartWatchingDxgiLoad() {
    for (int i = 0; i < (int)ARRAYSIZE(g_systemDllPaths); i++) {
        g_systemDllPaths[i] = GetSystemDllPath(i == 1, L"dxgi.dll");
    }

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    auto registerNotification =
        ntdll ? (LdrRegisterDllNotification_t)GetProcAddress(
                    ntdll, "LdrRegisterDllNotification")
              : nullptr;

    g_dxgiLoadedEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_hookThreadStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!registerNotification || !g_dxgiLoadedEvent ||
        !g_hookThreadStopEvent ||
        registerNotification(0, OnDllNotification, nullptr,
                             &g_dllNotificationCookie) != 0) {
        g_dllNotificationCookie = nullptr;
        // Wh_ModInit fails, so nothing else would close them.
        CloseWatchEvents();
        Wh_Log(L"Registering for DLL load notifications failed");
        return false;
    }
    return true;
}

// The notification callback and the thread are in this module, so this must
// happen before it's unloaded.
void StopWatchingDxgiLoad() {
    if (g_dllNotificationCookie) {
        HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
        auto unregisterNotification =
            (LdrUnregisterDllNotification_t)GetProcAddress(
                ntdll, "LdrUnregisterDllNotification");
        if (unregisterNotification) {
            unregisterNotification(g_dllNotificationCookie);
        }
        g_dllNotificationCookie = nullptr;
    }

    if (g_hookThread) {
        SetEvent(g_hookThreadStopEvent);
        WaitForSingleObject(g_hookThread, INFINITE);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
    }

    CloseWatchEvents();
}

////////////////////////////////////////////////////////////////////////////////

void LoadSettings() {
    g_forceFlipModel = Wh_GetIntSetting(L"forceFlipModel");
    g_forceAllowTearing = Wh_GetIntSetting(L"forceAllowTearing");
    int latency = Wh_GetIntSetting(L"maxFrameLatency");
    g_maxFrameLatency = latency < 0 ? 0 : latency > 3 ? 3 : latency;
    g_waitableSwapChain = Wh_GetIntSetting(L"waitableSwapChain");
    g_forceBorderless = Wh_GetIntSetting(L"forceBorderless");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    // Hooks set here are applied by Windhawk once Wh_ModInit returns.
    HookDxgiIfLoaded();
    if (g_dxgiHooked) {
        return TRUE;
    }

    return StartWatchingDxgiLoad();
}

void Wh_ModAfterInit() {
    // In case dxgi.dll was loaded while the mod was initializing.
    if (HookDxgiIfLoaded()) {
        Wh_ApplyHookOperations();
    }

    // Hooks can only be applied from now on.
    if (!g_dxgiHooked && g_dllNotificationCookie) {
        g_hookThread =
            CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, nullptr);
        if (!g_hookThread) {
            Wh_Log(L"Creating the hook thread failed");
        }
    }
}

void Wh_ModBeforeUninit() {
    // Hooks can't be set anymore after this, also from game threads.
    {
        std::lock_guard<std::mutex> guard(g_hookMutex);
        g_unloading = true;
    }
    StopWatchingDxgiLoad();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    // Its code is in this module.
    StopWindowThread();

    // The hooks are gone by now.
    if (g_dxgiModule) {
        FreeLibrary(g_dxgiModule);
        g_dxgiModule = nullptr;
    }
    for (HMODULE module : g_pinnedModules) {
        FreeLibrary(module);
    }
    g_pinnedModules.clear();

    // Nothing waits on them anymore.
    AcquireSRWLockExclusive(&g_waitablesLock);
    for (const Waitable& waitable : g_waitables) {
        CloseHandle(waitable.handle);
    }
    g_waitables.clear();
    ReleaseSRWLockExclusive(&g_waitablesLock);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
