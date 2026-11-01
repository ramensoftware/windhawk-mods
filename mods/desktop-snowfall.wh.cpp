// ==WindhawkMod==
// @id              desktop-snowfall
// @name            Desktop Snowfall
// @description     Snow falls over your desktop and settles on top of windows and the taskbar
// @version         1.0
// @author          mikybuilds
// @github          https://github.com/mikybuilds
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ld2d1 -ld3d11 -ldxgi -ldcomp -ldwmapi -lgdi32 -luser32 -lole32 -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Snowfall

Snow falls over your desktop. Flakes land on the top edge of your windows and on
the taskbar and slowly build up snow drifts of different heights.

* The snow follows the window. Toss a window up and the snow flies up in loose
  clumps and lands back with a little bounce. Pull it down or shrink it from
  the top and the snow hangs in the air for a moment before it falls back onto
  the window.
* Make a window narrower and the snow on the cut-off part falls down.
* Optionally, the snow slides off windows moved sideways: it stays where it is,
  and the part the window slides out from under falls down.
* Sweep the snow off with the mouse cursor - it falls down to the windows below
  or to the taskbar.
* The wind blows powder off the drifts, and now and then a lump slides off a
  window corner.
* Works with the default taskbar, an auto-hidden taskbar (the snow rides along
  when it slides in and out) and custom taskbar styles (for example, floating
  "dock" themes of the Windows 11 Taskbar Styler mod): the snow lies on the
  visible taskbar parts.

The snow is drawn on the GPU on a transparent, click-through overlay, so it
never gets in the way of mouse input. The overlay is hidden automatically while
a fullscreen app (a game or a video) is in the foreground.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- intensity: 100
  $name: Snowfall intensity (%)
  $description: >-
    How heavily it snows. 100 is a calm snowfall, 300 is a blizzard, 0 stops
    the snowfall (0-600)
- speed: 100
  $name: Fall speed (%)
  $description: 100 is the default speed
- wind: 15
  $name: Wind (%)
  $description: Horizontal drift. Negative values blow to the left (-100 to 100)
- size: 100
  $name: Flake size (%)
  $description: 100 is the default size
- capHeight: 8
  $name: Snow cap height (pixels)
  $description: >-
    Maximum height of the snow that settles on windows and the taskbar. 0
    disables the caps.
- sweepWithCursor: true
  $name: Sweep snow with the cursor
  $description: Moving the mouse cursor through a snow cap pushes the snow off
- slideOff: false
  $name: Snow slides off moved windows
  $description: >-
    When a window is moved sideways, the snow stays where it is and the window
    slides out from under it: the part that ends up past the window edge falls
    down. When off, the snow moves along with the window.
- taskbarSnow: auto
  $name: Snow on the taskbar
  $description: >-
    Where the snow lies on the taskbar. "Automatic" uses the visible taskbar
    parts when a custom style makes the taskbar narrower than the screen.
  $options:
  - auto: Automatic
  - whole: Whole taskbar
  - parts: Visible parts only (buttons and tray)
  - off: No snow on the taskbar
- maxFps: 60
  $name: Frame rate limit
  $description: Lower values use less GPU and CPU (15-240)
- allMonitors: false
  $name: All monitors
  $description: Draw the snow on every monitor instead of only the primary one
- pauseInFullscreen: true
  $name: Hide in fullscreen apps
  $description: Hide the snow while a fullscreen app is in the foreground
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dwmapi.h>
#include <dxgi1_3.h>
#include <uiautomation.h>
#include <wrl/client.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <vector>

using Microsoft::WRL::ComPtr;

constexpr float kGravity = 2600.0f;      // px/s^2, for snow caps
constexpr float kHangSeconds = 0.12f;    // snow hangs in the air this long
constexpr float kTossFactor = 0.5f;      // share of the window speed snow keeps
constexpr float kMinTossSpeed = 250.0f;  // px/s
constexpr float kMaxTossSpeed = 550.0f;  // px/s
constexpr float kBounceSpeed = 350.0f;   // snow landing faster bounces
constexpr float kChunkMaxSpeed = 700.0f;
constexpr size_t kMaxChunks = 4000;
constexpr size_t kMaxPuffs = 800;
constexpr int kSweepRadius = 7;
constexpr int kChunkColumns = 3;
constexpr float kCapEdgeTaper = 8.0f;
constexpr float kSnowKernel[] = {0.1f, 0.25f, 0.5f, 0.8f, 1.0f,
                                 0.8f, 0.5f,  0.25f, 0.1f};
constexpr float kSnowKernelSum = 4.3f;

enum class TaskbarSnow { Auto, Whole, Parts, Off };

struct {
    std::atomic<int> intensity{100};
    std::atomic<bool> slideOff{false};
    std::atomic<int> speed{100};
    std::atomic<int> wind{15};
    std::atomic<int> size{100};
    std::atomic<int> capHeight{8};
    std::atomic<bool> sweepWithCursor{true};
    std::atomic<TaskbarSnow> taskbarSnow{TaskbarSnow::Auto};
    std::atomic<int> maxFps{60};
    std::atomic<bool> allMonitors{false};
    std::atomic<bool> pauseInFullscreen{true};
} g_settings;

HANDLE g_stopEvent;
HANDLE g_snowThread;
HANDLE g_taskbarThread;

void LoadSettings() {
    g_settings.intensity = std::clamp(Wh_GetIntSetting(L"intensity"), 0, 600);
    g_settings.slideOff = Wh_GetIntSetting(L"slideOff") != 0;
    g_settings.speed = std::clamp(Wh_GetIntSetting(L"speed"), 10, 500);
    g_settings.wind = std::clamp(Wh_GetIntSetting(L"wind"), -100, 100);
    g_settings.size = std::clamp(Wh_GetIntSetting(L"size"), 20, 400);
    g_settings.capHeight = std::clamp(Wh_GetIntSetting(L"capHeight"), 0, 40);
    g_settings.sweepWithCursor = Wh_GetIntSetting(L"sweepWithCursor") != 0;
    g_settings.maxFps = std::clamp(Wh_GetIntSetting(L"maxFps"), 15, 240);
    g_settings.allMonitors = Wh_GetIntSetting(L"allMonitors") != 0;
    g_settings.pauseInFullscreen = Wh_GetIntSetting(L"pauseInFullscreen") != 0;

    PCWSTR taskbarSnow = Wh_GetStringSetting(L"taskbarSnow");
    TaskbarSnow mode = TaskbarSnow::Auto;
    if (!wcscmp(taskbarSnow, L"whole")) {
        mode = TaskbarSnow::Whole;
    } else if (!wcscmp(taskbarSnow, L"parts")) {
        mode = TaskbarSnow::Parts;
    } else if (!wcscmp(taskbarSnow, L"off")) {
        mode = TaskbarSnow::Off;
    }
    Wh_FreeStringSetting(taskbarSnow);
    g_settings.taskbarSnow = mode;
}

static bool IsTaskbarClass(const wchar_t* cls) {
    return !wcscmp(cls, L"Shell_TrayWnd") ||
           !wcscmp(cls, L"Shell_SecondaryTrayWnd");
}

static bool IsShellWindowClass(const wchar_t* cls) {
    return !wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW");
}

////////////////////////////////////////////////////////////////////////////////
// Taskbar parts.
//
// Taskbar styles can make most of the taskbar window transparent, leaving only
// a floating "dock" and the tray visible. The visible parts are found with UI
// Automation on a background thread, and stored relative to the taskbar window
// so that the snow follows the taskbar while it slides in and out (auto-hide).

struct TaskbarParts {
    RECT frame;  // relative to the taskbar window
    RECT tray;
    bool hasFrame;
    bool hasTray;
    int windowWidth;
};

// CLSID_CUIAutomation, defined here to avoid linking with uuid.lib.
constexpr GUID kClsidCUIAutomation = {
    0xff48dba4, 0x60ef, 0x4201, {0xaa, 0x87, 0x54, 0x10, 0x3e, 0xef, 0x59, 0x4e}};

std::mutex g_taskbarPartsMutex;
std::unordered_map<HWND, TaskbarParts> g_taskbarParts;

static bool IsUsableRect(const RECT& rc) {
    return rc.right > rc.left && rc.bottom > rc.top &&
           rc.right - rc.left < 100000 && rc.bottom - rc.top < 100000;
}

static void UnionInto(RECT& dst, bool& has, const RECT& rc) {
    if (!has) {
        dst = rc;
        has = true;
    } else {
        UnionRect(&dst, &dst, &rc);
    }
}

static bool QueryTaskbarParts(IUIAutomation* uia,
                              IUIAutomationCondition* trueCondition,
                              HWND taskbar,
                              TaskbarParts* parts) {
    RECT windowRect;
    if (!GetWindowRect(taskbar, &windowRect)) {
        return false;
    }

    ComPtr<IUIAutomationElement> root;
    if (FAILED(uia->ElementFromHandle(taskbar, &root)) || !root) {
        return false;
    }

    ComPtr<IUIAutomationElementArray> children;
    if (FAILED(root->FindAll(TreeScope_Children, trueCondition, &children)) ||
        !children) {
        return false;
    }

    RECT frame = {}, tray = {};
    bool hasFrame = false, hasTray = false;

    int childCount = 0;
    children->get_Length(&childCount);
    for (int i = 0; i < childCount; i++) {
        ComPtr<IUIAutomationElement> child;
        if (FAILED(children->GetElement(i, &child)) || !child) {
            continue;
        }

        // The XAML content: the taskbar frame and the tray buttons.
        ComPtr<IUIAutomationElementArray> items;
        if (FAILED(child->FindAll(TreeScope_Children, trueCondition, &items)) ||
            !items) {
            continue;
        }

        int itemCount = 0;
        items->get_Length(&itemCount);
        for (int j = 0; j < itemCount; j++) {
            ComPtr<IUIAutomationElement> item;
            if (FAILED(items->GetElement(j, &item)) || !item) {
                continue;
            }

            BSTR className = nullptr;
            RECT rc;
            if (FAILED(item->get_CurrentClassName(&className)) || !className) {
                continue;
            }
            if (SUCCEEDED(item->get_CurrentBoundingRectangle(&rc)) &&
                IsUsableRect(rc) && rc.bottom - rc.top >= 10) {
                if (!wcscmp(className, L"Taskbar.TaskbarFrameAutomationPeer")) {
                    UnionInto(frame, hasFrame, rc);
                } else if (!wcsncmp(className, L"SystemTray.", 11)) {
                    UnionInto(tray, hasTray, rc);
                }
            }
            SysFreeString(className);
        }
    }

    // Nothing is found while an auto-hidden taskbar is hidden, and the result
    // is useless if the taskbar moved during the query (it was sliding).
    RECT windowRectAfter;
    if (!hasFrame || !GetWindowRect(taskbar, &windowRectAfter) ||
        !EqualRect(&windowRect, &windowRectAfter)) {
        return false;
    }

    OffsetRect(&frame, -windowRect.left, -windowRect.top);
    OffsetRect(&tray, -windowRect.left, -windowRect.top);
    parts->frame = frame;
    parts->tray = tray;
    parts->hasFrame = hasFrame;
    parts->hasTray = hasTray;
    parts->windowWidth = windowRect.right - windowRect.left;
    return true;
}

DWORD WINAPI TaskbarThread(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
        return 0;
    }

    {
        ComPtr<IUIAutomation> uia;
        ComPtr<IUIAutomationCondition> trueCondition;
        if (SUCCEEDED(CoCreateInstance(kClsidCUIAutomation, nullptr,
                                       CLSCTX_INPROC_SERVER,
                                       IID_PPV_ARGS(&uia))) &&
            SUCCEEDED(uia->CreateTrueCondition(&trueCondition))) {
            // Don't get stuck for long if the taskbar doesn't respond.
            ComPtr<IUIAutomation2> uia2;
            if (SUCCEEDED(uia.As(&uia2))) {
                uia2->put_ConnectionTimeout(1000);
                uia2->put_TransactionTimeout(1000);
            }

            do {
                TaskbarSnow mode = g_settings.taskbarSnow;
                if (mode != TaskbarSnow::Auto && mode != TaskbarSnow::Parts) {
                    continue;
                }

                std::vector<HWND> taskbars;
                EnumWindows(
                    [](HWND hwnd, LPARAM lParam) -> BOOL {
                        wchar_t cls[64];
                        if (GetClassName(hwnd, cls, ARRAYSIZE(cls)) &&
                            IsTaskbarClass(cls)) {
                            reinterpret_cast<std::vector<HWND>*>(lParam)
                                ->push_back(hwnd);
                        }
                        return TRUE;
                    },
                    reinterpret_cast<LPARAM>(&taskbars));

                std::unordered_map<HWND, TaskbarParts> previous;
                {
                    std::lock_guard<std::mutex> lock(g_taskbarPartsMutex);
                    previous = g_taskbarParts;
                }

                // The parts are stored relative to the taskbar window, so the
                // last known ones stay valid while the taskbar is hidden.
                std::unordered_map<HWND, TaskbarParts> found;
                for (HWND taskbar : taskbars) {
                    TaskbarParts parts;
                    if (QueryTaskbarParts(uia.Get(), trueCondition.Get(),
                                          taskbar, &parts)) {
                        found[taskbar] = parts;
                    } else if (auto it = previous.find(taskbar);
                               it != previous.end()) {
                        found[taskbar] = it->second;
                    }
                }

                std::lock_guard<std::mutex> lock(g_taskbarPartsMutex);
                g_taskbarParts = std::move(found);
            } while (WaitForSingleObject(g_stopEvent, 500) == WAIT_TIMEOUT);
        }
    }

    CoUninitialize();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// The snow.

struct Flake {
    float x, y;  // screen coordinates
    float size;
    float fallSpeed;
    float phase;
    float sway;
    float alpha;
};

// A lump of snow that fell off a cap.
struct Chunk {
    float x, y;
    float vx, vy;
    float size;
    float area;
    uintptr_t ignoreKey;  // the cap it came from
};

// Something that snow can lie on: a window, the taskbar or a part of it.
struct Body {
    uintptr_t key;
    HWND hwnd;
    RECT rc;
    bool snowCap;
};

// A puff of powder snow. It drifts with the wind and melts away.
struct Puff {
    float x, y;
    float vx, vy;
    float size;
    float life;
    float maxLife;
};

// One pixel column of a snow cap.
struct Column {
    float h = 0;     // snow height
    float lift = 0;  // how high the snow is above the body's top edge
    float vel = 0;   // vertical speed of the snow, px/s, down is positive
    float hang = 0;  // seconds left to hang in the air
};

struct Cap {
    HWND hwnd = nullptr;
    RECT rc = {};  // the body rect in the previous frame
    std::vector<Column> cols;
    uint32_t seed = 0;
    int offset = 0;  // keeps the drifts in place when the left edge moves
    float vx = 0, vy = 0;  // smoothed speed of the body
    float carry = 0;  // the fastest upward speed the snow was carried at
    float squash = 0, squashVel = 0;
    bool airborne = false;  // some of the snow is in the air
    unsigned seenFrame = 0;
};

static uintptr_t BodyKey(HWND hwnd, int part) {
    return (reinterpret_cast<uintptr_t>(hwnd) << 2) | part;
}

static uint32_t Hash32(uint32_t x) {
    x ^= x >> 16;
    x *= 0x7feb352d;
    x ^= x >> 15;
    x *= 0x846ca68b;
    x ^= x >> 16;
    return x;
}

// Smooth 1D value noise in [0, 1).
static float SmoothNoise(uint32_t seed, float x) {
    float cell = std::floor(x);
    uint32_t i = static_cast<uint32_t>(static_cast<int>(cell));
    float t = x - cell;
    t = t * t * (3.0f - 2.0f * t);
    float a = (Hash32(seed + i * 0x9E3779B1u) & 0xFFFFFF) / 16777216.0f;
    float b = (Hash32(seed + (i + 1) * 0x9E3779B1u) & 0xFFFFFF) / 16777216.0f;
    return a + (b - a) * t;
}

static float CapNoise(const Cap& cap, int column, uint32_t salt, float scale) {
    return SmoothNoise(cap.seed + salt * 0x51ED27u,
                       static_cast<float>(column + cap.offset) / scale);
}

// The most snow a column can hold. It varies along the cap so that the snow
// settles in drifts of different heights instead of a flat strip.
static float SnowLimit(const Cap& cap, int column, float maxHeight) {
    float n = 0.65f * CapNoise(cap, column, 1, 70.0f) +
              0.35f * CapNoise(cap, column, 2, 23.0f);
    n = std::clamp((n - 0.5f) * 1.8f + 0.5f, 0.0f, 1.0f);
    return maxHeight * (0.3f + 0.7f * n);
}

class Overlay {
   public:
    ~Overlay();

    bool Create();
    void Run();

   private:
    bool CreateDeviceResources();
    void ReleaseDeviceResources();
    bool CreateTargetBitmap();
    bool CreateFlakeSprite();
    RECT ComputeArea();
    bool ApplyArea(const RECT& area);
    UINT ComputeSyncInterval();

    float Rand();
    void SpawnFlake(Flake& f, bool scatter);
    void SpawnChunk(float x, float y, float area, float vx, float vy,
                    uintptr_t ignoreKey);
    void SpawnPuff(float x, float y, float vx, float vy, float size,
                   float life);
    void DropColumns(Cap& cap, int first, int count);

    void CollectBodies();
    void AddTaskbarBodies(HWND hwnd, const RECT& rc);
    bool IsFullscreenAppActive();
    bool IsCoveredAbove(size_t bodyIndex, int x, int y);
    const Body* FindLanding(float x, float oldY, float newY,
                            uintptr_t ignoreKey);
    void AddSnow(uintptr_t key, int column, float amount);

    void UpdateCaps(float dt);
    bool InheritSnow(Cap& cap, unsigned lastSeen);
    void ResizeCap(Cap& cap, const RECT& rc);
    void MoveCap(Cap& cap, const RECT& rc, float dt);
    void Ambient(float dt);
    void Sweep(float dt);
    void UpdateFlakes(float dt);
    void UpdateChunks(float dt);
    void UpdatePuffs(float dt);
    bool Render();
    void DrawCap(const Cap& cap, const RECT& rc);

    HWND m_hwnd = nullptr;
    RECT m_area = {};
    int m_width = 0;
    int m_height = 0;
    bool m_visible = true;
    UINT m_syncInterval = 1;

    ComPtr<ID3D11Device> m_d3dDevice;
    ComPtr<ID2D1Factory1> m_d2dFactory;
    ComPtr<ID2D1Device> m_d2dDevice;
    ComPtr<ID2D1DeviceContext> m_ctx;
    ComPtr<IDXGISwapChain1> m_swapChain;
    HANDLE m_frameWait = nullptr;
    ComPtr<IDCompositionDevice> m_dcompDevice;
    ComPtr<IDCompositionTarget> m_dcompTarget;
    ComPtr<IDCompositionVisual> m_dcompVisual;
    ComPtr<ID2D1Bitmap1> m_targetBitmap;
    ComPtr<ID2D1Bitmap1> m_flakeSprite;
    ComPtr<ID2D1LinearGradientBrush> m_capBrush;

    std::vector<Flake> m_flakes;
    std::vector<Chunk> m_chunks;
    std::vector<Puff> m_puffs;
    std::vector<Body> m_bodies;
    std::unordered_map<uintptr_t, Cap> m_caps;

    POINT m_lastCursor = {};
    bool m_cursorValid = false;
    uint32_t m_rng = 0x9E3779B9;
    float m_time = 0;
    float m_frameTime = 1.0f / 60;
    unsigned m_frame = 0;
};

constexpr wchar_t kOverlayClass[] = L"WindhawkDesktopSnowfallOverlay";

Overlay::~Overlay() {
    ReleaseDeviceResources();
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        UnregisterClass(kOverlayClass, GetModuleHandle(nullptr));
    }
}

float Overlay::Rand() {
    m_rng ^= m_rng << 13;
    m_rng ^= m_rng >> 17;
    m_rng ^= m_rng << 5;
    return (m_rng & 0xFFFFFF) / 16777216.0f;
}

RECT Overlay::ComputeArea() {
    RECT rc;
    if (g_settings.allMonitors) {
        rc.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
        rc.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
        rc.right = rc.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
        rc.bottom = rc.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
    } else {
        MONITORINFO mi = {sizeof(mi)};
        GetMonitorInfo(MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY), &mi);
        rc = mi.rcMonitor;
    }
    return rc;
}

UINT Overlay::ComputeSyncInterval() {
    DWM_TIMING_INFO ti = {sizeof(ti)};
    double refresh = 60.0;
    if (SUCCEEDED(DwmGetCompositionTimingInfo(nullptr, &ti)) &&
        ti.rateRefresh.uiDenominator) {
        refresh = static_cast<double>(ti.rateRefresh.uiNumerator) /
                  ti.rateRefresh.uiDenominator;
    }
    int interval = static_cast<int>(std::lround(refresh / g_settings.maxFps));
    return static_cast<UINT>(std::clamp(interval, 1, 4));
}

bool Overlay::Create() {
    WNDCLASSEX wc = {sizeof(wc)};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = kOverlayClass;
    RegisterClassEx(&wc);

    m_area = ComputeArea();
    m_width = m_area.right - m_area.left;
    m_height = m_area.bottom - m_area.top;

    m_hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW |
            WS_EX_NOACTIVATE | WS_EX_NOREDIRECTIONBITMAP,
        kOverlayClass, L"Desktop Snowfall", WS_POPUP, m_area.left, m_area.top,
        m_width, m_height, nullptr, nullptr, wc.hInstance, nullptr);
    if (!m_hwnd) {
        return false;
    }

    // Needed for a layered window to be shown; the content itself comes from
    // DirectComposition.
    SetLayeredWindowAttributes(m_hwnd, 0, 255, LWA_ALPHA);

    if (!CreateDeviceResources()) {
        return false;
    }

    ShowWindow(m_hwnd, SW_SHOWNOACTIVATE);
    m_syncInterval = ComputeSyncInterval();
    return true;
}

bool Overlay::CreateDeviceResources() {
    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                   flags, nullptr, 0, D3D11_SDK_VERSION,
                                   &m_d3dDevice, nullptr, nullptr);
    if (FAILED(hr)) {
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, flags,
                               nullptr, 0, D3D11_SDK_VERSION, &m_d3dDevice,
                               nullptr, nullptr);
    }
    if (FAILED(hr)) {
        Wh_Log(L"D3D11CreateDevice failed: %08X", hr);
        return false;
    }

    ComPtr<IDXGIDevice> dxgiDevice;
    ComPtr<IDXGIAdapter> adapter;
    ComPtr<IDXGIFactory2> dxgiFactory;
    if (FAILED(m_d3dDevice.As(&dxgiDevice)) ||
        FAILED(dxgiDevice->GetAdapter(&adapter)) ||
        FAILED(adapter->GetParent(IID_PPV_ARGS(&dxgiFactory)))) {
        return false;
    }

    D2D1_FACTORY_OPTIONS options = {};
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                 __uuidof(ID2D1Factory1), &options,
                                 reinterpret_cast<void**>(
                                     m_d2dFactory.GetAddressOf()))) ||
        FAILED(m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice)) ||
        FAILED(m_d2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_ctx))) {
        Wh_Log(L"Direct2D initialization failed");
        return false;
    }

    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.Width = m_width;
    desc.Height = m_height;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    desc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
    hr = dxgiFactory->CreateSwapChainForComposition(m_d3dDevice.Get(), &desc,
                                                    nullptr, &m_swapChain);
    if (FAILED(hr)) {
        Wh_Log(L"CreateSwapChainForComposition failed: %08X", hr);
        return false;
    }

    // Render one frame per refresh at most, without queuing frames.
    ComPtr<IDXGISwapChain2> swapChain2;
    if (SUCCEEDED(m_swapChain.As(&swapChain2))) {
        swapChain2->SetMaximumFrameLatency(1);
        m_frameWait = swapChain2->GetFrameLatencyWaitableObject();
    }

    if (FAILED(DCompositionCreateDevice(dxgiDevice.Get(),
                                        IID_PPV_ARGS(&m_dcompDevice))) ||
        FAILED(m_dcompDevice->CreateTargetForHwnd(m_hwnd, TRUE,
                                                  &m_dcompTarget)) ||
        FAILED(m_dcompDevice->CreateVisual(&m_dcompVisual)) ||
        FAILED(m_dcompVisual->SetContent(m_swapChain.Get())) ||
        FAILED(m_dcompTarget->SetRoot(m_dcompVisual.Get())) ||
        FAILED(m_dcompDevice->Commit())) {
        Wh_Log(L"DirectComposition initialization failed");
        return false;
    }

    D2D1_GRADIENT_STOP stops[] = {
        {0.0f, D2D1::ColorF(0.99f, 0.995f, 1.0f, 0.97f)},
        {1.0f, D2D1::ColorF(0.78f, 0.85f, 0.94f, 0.97f)},
    };
    ComPtr<ID2D1GradientStopCollection> stopCollection;
    if (FAILED(m_ctx->CreateGradientStopCollection(stops, ARRAYSIZE(stops),
                                                   &stopCollection)) ||
        FAILED(m_ctx->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties({0, 0}, {0, 1}),
            stopCollection.Get(), &m_capBrush))) {
        return false;
    }

    return CreateFlakeSprite() && CreateTargetBitmap();
}

void Overlay::ReleaseDeviceResources() {
    if (m_ctx) {
        m_ctx->SetTarget(nullptr);
    }
    m_targetBitmap.Reset();
    m_flakeSprite.Reset();
    m_capBrush.Reset();
    m_dcompVisual.Reset();
    m_dcompTarget.Reset();
    m_dcompDevice.Reset();
    if (m_frameWait) {
        CloseHandle(m_frameWait);
        m_frameWait = nullptr;
    }
    m_swapChain.Reset();
    m_ctx.Reset();
    m_d2dDevice.Reset();
    m_d2dFactory.Reset();
    m_d3dDevice.Reset();
}

bool Overlay::CreateTargetBitmap() {
    ComPtr<IDXGISurface> surface;
    if (FAILED(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&surface)))) {
        return false;
    }

    D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED));
    if (FAILED(m_ctx->CreateBitmapFromDxgiSurface(surface.Get(), &props,
                                                  &m_targetBitmap))) {
        return false;
    }

    m_ctx->SetTarget(m_targetBitmap.Get());
    return true;
}

// A soft round flake with a faint bluish rim that keeps it visible on light
// windows. It's drawn once and then stamped for every flake.
bool Overlay::CreateFlakeSprite() {
    constexpr UINT32 kSize = 32;
    D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED));
    if (FAILED(m_ctx->CreateBitmap(D2D1::SizeU(kSize, kSize), nullptr, 0,
                                   &props, &m_flakeSprite))) {
        return false;
    }

    D2D1_GRADIENT_STOP stops[] = {
        {0.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f)},
        {0.42f, D2D1::ColorF(0.98f, 0.99f, 1.0f, 0.95f)},
        {0.6f, D2D1::ColorF(0.75f, 0.82f, 0.95f, 0.35f)},
        {1.0f, D2D1::ColorF(0.6f, 0.7f, 0.85f, 0.0f)},
    };
    ComPtr<ID2D1GradientStopCollection> stopCollection;
    ComPtr<ID2D1RadialGradientBrush> brush;
    if (FAILED(m_ctx->CreateGradientStopCollection(stops, ARRAYSIZE(stops),
                                                   &stopCollection)) ||
        FAILED(m_ctx->CreateRadialGradientBrush(
            D2D1::RadialGradientBrushProperties({kSize / 2.0f, kSize / 2.0f},
                                                {0, 0}, kSize / 2.0f,
                                                kSize / 2.0f),
            stopCollection.Get(), &brush))) {
        return false;
    }

    m_ctx->SetTarget(m_flakeSprite.Get());
    m_ctx->BeginDraw();
    m_ctx->Clear(D2D1::ColorF(0, 0, 0, 0));
    m_ctx->FillRectangle(D2D1::RectF(0, 0, kSize, kSize), brush.Get());
    HRESULT hr = m_ctx->EndDraw();
    m_ctx->SetTarget(nullptr);
    return SUCCEEDED(hr);
}

bool Overlay::ApplyArea(const RECT& area) {
    int width = area.right - area.left;
    int height = area.bottom - area.top;
    if (width <= 0 || height <= 0) {
        return false;
    }

    m_ctx->SetTarget(nullptr);
    m_targetBitmap.Reset();
    if (FAILED(m_swapChain->ResizeBuffers(
            2, width, height, DXGI_FORMAT_B8G8R8A8_UNORM,
            DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT)) ||
        !CreateTargetBitmap()) {
        return false;
    }

    m_area = area;
    m_width = width;
    m_height = height;
    SetWindowPos(m_hwnd, HWND_TOPMOST, area.left, area.top, width, height,
                 SWP_NOACTIVATE);

    for (Flake& f : m_flakes) {
        SpawnFlake(f, true);
    }
    m_chunks.clear();
    m_puffs.clear();
    return true;
}

void Overlay::SpawnFlake(Flake& f, bool scatter) {
    float depth = Rand();
    f.size = (0.8f + depth * depth * 2.7f) * g_settings.size / 100.0f;
    f.fallSpeed = 35.0f + f.size * 22.0f;
    f.phase = Rand() * 6.2832f;
    f.sway = 6.0f + Rand() * 14.0f;
    f.alpha = 0.45f + 0.5f * Rand();
    f.x = m_area.left + Rand() * m_width;
    f.y = scatter ? m_area.top + Rand() * m_height
                  : m_area.top - f.size * 2 - Rand() * 40.0f;
}

void Overlay::SpawnChunk(float x, float y, float area, float vx, float vy,
                         uintptr_t ignoreKey) {
    if (m_chunks.size() >= kMaxChunks || area < 0.5f) {
        return;
    }

    Chunk c;
    c.x = x;
    c.y = y;
    c.vx = vx;
    c.vy = vy;
    c.area = area;
    c.size = std::clamp(std::sqrt(area) * 0.55f, 1.0f, 3.5f);
    c.ignoreKey = ignoreKey;
    m_chunks.push_back(c);
}

void Overlay::SpawnPuff(float x, float y, float vx, float vy, float size,
                        float life) {
    if (m_puffs.size() >= kMaxPuffs) {
        return;
    }
    m_puffs.push_back({x, y, vx, vy, size, life, life});
}

// Turns snow columns that lost their support (the cap's position is from the
// previous frame) into falling chunks.
void Overlay::DropColumns(Cap& cap, int first, int count) {
    for (int i = 0; i < count; i += kChunkColumns) {
        float area = 0, top = 0, lift = 0;
        for (int k = i; k < std::min(i + kChunkColumns, count); k++) {
            const Column& col = cap.cols[first + k];
            area += col.h;
            top = std::max(top, col.h);
            lift = std::max(lift, col.lift);
        }
        SpawnChunk(cap.rc.left + first + i + 1.0f, cap.rc.top - lift - top / 2,
                   area, (Rand() - 0.5f) * 30.0f, 0, 0);
    }
}

void Overlay::AddTaskbarBodies(HWND hwnd, const RECT& rc) {
    TaskbarSnow mode = g_settings.taskbarSnow;
    if (mode == TaskbarSnow::Off) {
        return;
    }

    if (mode != TaskbarSnow::Whole) {
        TaskbarParts parts;
        bool found = false;
        {
            std::lock_guard<std::mutex> lock(g_taskbarPartsMutex);
            auto it = g_taskbarParts.find(hwnd);
            if (it != g_taskbarParts.end()) {
                parts = it->second;
                found = true;
            }
        }

        if (found && parts.windowWidth == rc.right - rc.left) {
            bool useParts =
                mode == TaskbarSnow::Parts ||
                parts.frame.right - parts.frame.left < parts.windowWidth * 0.85;
            if (useParts) {
                RECT frame = parts.frame;
                OffsetRect(&frame, rc.left, rc.top);
                m_bodies.push_back({BodyKey(hwnd, 1), hwnd, frame, true});
                if (parts.hasTray) {
                    RECT tray = parts.tray;
                    OffsetRect(&tray, rc.left, rc.top);
                    m_bodies.push_back({BodyKey(hwnd, 2), hwnd, tray, true});
                }
                return;
            }
        }
    }

    m_bodies.push_back({BodyKey(hwnd, 0), hwnd, rc, true});
}

void Overlay::CollectBodies() {
    m_bodies.clear();
    EnumWindows(
        [](HWND hwnd, LPARAM lParam) -> BOOL {
            Overlay* self = reinterpret_cast<Overlay*>(lParam);
            if (hwnd == self->m_hwnd || !IsWindowVisible(hwnd) ||
                IsIconic(hwnd)) {
                return TRUE;
            }

            wchar_t cls[64];
            if (!GetClassName(hwnd, cls, ARRAYSIZE(cls)) ||
                IsShellWindowClass(cls)) {
                return TRUE;
            }

            bool isTaskbar = IsTaskbarClass(cls);
            LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
            if (!isTaskbar) {
                if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) {
                    return TRUE;
                }
                if ((exStyle & WS_EX_LAYERED) && (exStyle & WS_EX_TRANSPARENT)) {
                    return TRUE;
                }
            }

            DWORD cloaked = 0;
            DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                  sizeof(cloaked));
            if (cloaked) {
                return TRUE;
            }

            RECT rc;
            if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS,
                                             &rc, sizeof(rc)))) {
                GetWindowRect(hwnd, &rc);
            }
            if (rc.right - rc.left < 80 || rc.bottom - rc.top < 30) {
                return TRUE;
            }

            if (isTaskbar) {
                self->AddTaskbarBodies(hwnd, rc);
            } else {
                self->m_bodies.push_back(
                    {BodyKey(hwnd, 0), hwnd, rc, !IsZoomed(hwnd)});
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(this));
}

bool Overlay::IsFullscreenAppActive() {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == m_hwnd) {
        return false;
    }

    wchar_t cls[64];
    if (!GetClassName(fg, cls, ARRAYSIZE(cls)) || IsShellWindowClass(cls) ||
        IsTaskbarClass(cls)) {
        return false;
    }

    RECT rc;
    MONITORINFO mi = {sizeof(mi)};
    if (!GetWindowRect(fg, &rc) ||
        !GetMonitorInfo(MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST), &mi)) {
        return false;
    }

    if (rc.left > mi.rcMonitor.left || rc.top > mi.rcMonitor.top ||
        rc.right < mi.rcMonitor.right || rc.bottom < mi.rcMonitor.bottom) {
        return false;
    }

    // With an auto-hidden taskbar, a maximized window covers the whole
    // monitor too, but it isn't a fullscreen app.
    LONG style = GetWindowLong(fg, GWL_STYLE);
    return !(IsZoomed(fg) && (style & WS_CAPTION) == WS_CAPTION);
}

bool Overlay::IsCoveredAbove(size_t bodyIndex, int x, int y) {
    for (size_t j = 0; j < bodyIndex; j++) {
        const RECT& o = m_bodies[j].rc;
        if (x >= o.left && x < o.right && y >= o.top && y < o.bottom) {
            return true;
        }
    }
    return false;
}

// The topmost body under a falling particle decides what happens to it: the
// particle lands if it just crossed the body's top edge.
const Body* Overlay::FindLanding(float x, float oldY, float newY,
                                 uintptr_t ignoreKey) {
    int sx = static_cast<int>(std::floor(x));
    for (const Body& b : m_bodies) {
        if (sx < b.rc.left || sx >= b.rc.right || newY < b.rc.top ||
            newY >= b.rc.bottom) {
            continue;
        }
        if (b.key != ignoreKey && b.snowCap && oldY < b.rc.top) {
            return &b;
        }
        return nullptr;
    }
    return nullptr;
}

void Overlay::AddSnow(uintptr_t key, int column, float amount) {
    auto it = m_caps.find(key);
    if (it == m_caps.end()) {
        return;
    }

    Cap& cap = it->second;
    std::vector<Column>& cols = cap.cols;
    const int width = static_cast<int>(cols.size());
    const float maxHeight = static_cast<float>(g_settings.capHeight);

    // Let the pile relax so it doesn't form needles, and keep it under the
    // drift profile.
    constexpr int kReach = 14;
    int from = std::max(column - kReach, 1);
    int to = std::min(column + kReach, width - 2);
    if (from > to) {
        return;
    }

    float limit[2 * kReach + 1];
    for (int c = from; c <= to; c++) {
        limit[c - from] = SnowLimit(cap, c, maxHeight);
    }

    for (int i = 0; i < 9; i++) {
        int c = column + i - 4;
        if (c >= from && c <= to) {
            cols[c].h = std::min(limit[c - from],
                                 cols[c].h + amount * kSnowKernel[i]);
        }
    }

    for (int pass = 0; pass < 2; pass++) {
        for (int c = from; c <= to; c++) {
            float h = cols[c].h * 0.5f + (cols[c - 1].h + cols[c + 1].h) * 0.25f;
            cols[c].h = std::min(h, limit[c - from]);
        }
    }
}

// Keeps the columns in place when only one side of the body moved, and drops
// the snow from the part that's gone.
void Overlay::ResizeCap(Cap& cap, const RECT& rc) {
    int oldWidth = cap.rc.right - cap.rc.left;
    int newWidth = rc.right - rc.left;
    std::vector<Column>& cols = cap.cols;

    if (oldWidth == newWidth) {
        // Moved sideways. Optionally, the snow stays where it is and the
        // window slides out from under it.
        int dx = rc.left - cap.rc.left;
        int width = static_cast<int>(cols.size());
        if (dx == 0 || !g_settings.slideOff || width == 0) {
            return;
        }

        int n = std::min(std::abs(dx), width);
        if (dx > 0) {
            DropColumns(cap, 0, n);
            cols.erase(cols.begin(), cols.begin() + n);
            cols.resize(width, Column{});
        } else {
            DropColumns(cap, width - n, n);
            cols.resize(width - n);
            cols.insert(cols.begin(), n, Column{});
        }
        cap.offset += dx;
        return;
    }
    int dl = rc.left - cap.rc.left;
    int dr = rc.right - cap.rc.right;
    if (dl != 0 && dr != 0) {
        // Moved and resized at once (snapped, restored): stretch the snow.
        std::vector<Column> old = std::move(cols);
        cols.assign(newWidth, Column{});
        if (!old.empty()) {
            for (int c = 0; c < newWidth; c++) {
                size_t src = static_cast<size_t>(c) * old.size() / newWidth;
                cols[c].h = old[src].h;
            }
        }
        return;
    }

    if (dl > 0) {
        int n = std::min(dl, static_cast<int>(cols.size()));
        DropColumns(cap, 0, n);
        cols.erase(cols.begin(), cols.begin() + n);
    } else if (dl < 0) {
        cols.insert(cols.begin(), -dl, Column{});
    }
    cap.offset += dl;

    if (dr < 0) {  // the left edge didn't move
        int n = std::min(-dr, static_cast<int>(cols.size()));
        int first = static_cast<int>(cols.size()) - n;
        DropColumns(cap, first, n);
        cols.resize(first);
    }

    cols.resize(newWidth, Column{});
}

void Overlay::UpdateCaps(float dt) {
    if (g_settings.capHeight <= 0) {
        m_caps.clear();
        return;
    }

    std::vector<HWND> inheritedFrom;
    for (const Body& b : m_bodies) {
        if (!b.snowCap) {
            continue;
        }

        auto [it, inserted] = m_caps.try_emplace(b.key);
        Cap& cap = it->second;
        const int width = b.rc.right - b.rc.left;

        if (inserted || cap.seenFrame != m_frame - 1) {
            // New, or back after being minimized or hidden: just put the snow
            // on top.
            unsigned lastSeen = inserted ? 0 : cap.seenFrame;
            if (inserted) {
                cap.hwnd = b.hwnd;
                cap.seed = Hash32(static_cast<uint32_t>(b.key) ^
                                  static_cast<uint32_t>(b.key >> 32));
            }
            cap.rc = b.rc;
            cap.cols.resize(width);
            for (Column& col : cap.cols) {
                col.lift = col.vel = col.hang = 0;
            }
            cap.vx = cap.vy = cap.carry = 0;
            cap.squash = cap.squashVel = 0;
            cap.airborne = false;
            if (InheritSnow(cap, lastSeen)) {
                inheritedFrom.push_back(cap.hwnd);
            }
        } else {
            ResizeCap(cap, b.rc);
            MoveCap(cap, b.rc, dt);
        }

        cap.rc = b.rc;
        cap.seenFrame = m_frame;
    }

    // A taskbar switched between being a whole and being split into its
    // visible parts. Whatever snow didn't move over to the new caps falls.
    for (HWND hwnd : inheritedFrom) {
        for (auto it = m_caps.begin(); it != m_caps.end();) {
            Cap& cap = it->second;
            if (cap.hwnd == hwnd && cap.seenFrame != m_frame) {
                if (cap.seenFrame == m_frame - 1) {
                    DropColumns(cap, 0, static_cast<int>(cap.cols.size()));
                }
                it = m_caps.erase(it);
            } else {
                ++it;
            }
        }
    }

    if (m_frame % 60 == 0) {
        for (auto it = m_caps.begin(); it != m_caps.end();) {
            it = IsWindow(it->second.hwnd) ? std::next(it) : m_caps.erase(it);
        }
    }
}

// Moves the snow from the other caps of the same window that were seen more
// recently than this one.
bool Overlay::InheritSnow(Cap& cap, unsigned lastSeen) {
    bool inherited = false;
    for (auto& [key, other] : m_caps) {
        if (&other == &cap || other.hwnd != cap.hwnd ||
            other.seenFrame != m_frame - 1 || other.seenFrame <= lastSeen) {
            continue;
        }

        for (size_t c = 0; c < other.cols.size(); c++) {
            int dst = other.rc.left + static_cast<int>(c) - cap.rc.left;
            if (dst >= 0 && dst < static_cast<int>(cap.cols.size())) {
                cap.cols[dst].h = std::max(cap.cols[dst].h, other.cols[c].h);
                other.cols[c].h = 0;
            }
        }
        inherited = true;
    }
    return inherited;
}

void Overlay::MoveCap(Cap& cap, const RECT& rc, float dt) {
    const float dTop = static_cast<float>(rc.top - cap.rc.top);
    const float prevVy = cap.vy;
    float vxNow = 0;
    if (rc.right - rc.left == cap.rc.right - cap.rc.left) {
        vxNow = (rc.left - cap.rc.left) / dt;
    }
    cap.vy = cap.vy * 0.5f + dTop / dt * 0.5f;
    cap.vx = cap.vx * 0.5f + vxNow * 0.5f;

    // Thrown up: the snow rides up with the body, and flies on when the body
    // slows down.
    bool launch = false;
    float tossSpeed = 0;
    if (cap.vy < cap.carry) {
        cap.carry = cap.vy;
    } else if (cap.carry < -kMinTossSpeed && cap.vy > cap.carry * 0.35f) {
        launch = true;
        tossSpeed = std::max(cap.carry * kTossFactor, -kMaxTossSpeed);
        cap.carry = 0;
    } else if (cap.vy > -kMinTossSpeed * 0.5f) {
        cap.carry = 0;
    }

    // Pulled down faster than before (shrunk from the top, moved down): the
    // snow keeps its speed and falls after the body, and after a sudden jump
    // it hangs in the air for a moment.
    const float excess = dTop - std::max(prevVy, 0.0f) * dt;
    const bool drop = excess > 3.0f;
    const float hang =
        excess > 8.0f
            ? kHangSeconds * std::clamp((excess - 8.0f) / 20.0f, 0.3f, 1.0f)
            : 0.0f;

    if (!cap.airborne && !launch && !drop) {
        return;
    }

    std::vector<Column>& cols = cap.cols;
    const int width = static_cast<int>(cols.size());
    float impact = 0;
    bool anyInAir = false;

    for (int c = 0; c < width; c++) {
        Column& col = cols[c];
        bool inAir = col.lift > 0 || col.vel < 0 || col.hang > 0;
        // All columns, even the empty ones, move together, so that the snow
        // in the air stays one connected shape.
        if (inAir) {
            // The snow in the air stays where it is while the body moves.
            col.lift += dTop;
        } else if (drop) {
            col.lift = dTop;
            col.vel = std::max(prevVy, 0.0f);
            col.hang = hang * (0.8f + 0.4f * CapNoise(cap, c, 3, 60.0f));
        } else if (launch) {
            // The cap flies up as one piece; loose bits break off below.
            col.vel = tossSpeed;
            col.lift = 0;
        } else {
            continue;
        }

        if (col.lift < 0) {
            // The body caught up with the snow from below and pushes it.
            col.lift = 0;
            col.hang = 0;
            col.vel = std::max(std::min(col.vel, cap.vy), -kMaxTossSpeed);
        }

        if (col.hang > 0) {
            col.hang -= dt;
            anyInAir = true;
            continue;
        }

        col.vel += kGravity * dt;
        col.lift -= col.vel * dt;
        if (col.lift <= 0) {
            col.lift = 0;
            if (col.vel > kBounceSpeed) {
                impact = std::max(impact, col.vel);
                if (col.vel > 550.0f && Rand() < 0.06f) {
                    SpawnPuff(rc.left + c + 0.5f, rc.top - col.h,
                              (Rand() - 0.5f) * 90.0f,
                              -40.0f - Rand() * 90.0f, 0.7f + Rand() * 0.8f,
                              0.5f + Rand() * 0.4f);
                }
                col.vel *= -0.2f;
            } else {
                col.vel = 0;
            }
        }
        if (col.lift > 0 || col.vel < 0) {
            anyInAir = true;
        }
    }
    cap.airborne = anyInAir;

    if (launch) {
        cap.squashVel += 1.5f;  // stretched by the throw

        // A few lumps and some powder break off and fly a bit higher than
        // the cap; a hard throw breaks off more.
        float breakShare =
            0.06f + std::clamp((-tossSpeed / kTossFactor - 900.0f) / 2500.0f,
                               0.0f, 0.25f);
        for (int c = 0; c + kChunkColumns <= width; c += kChunkColumns) {
            float top = 0;
            for (int k = c; k < c + kChunkColumns; k++) {
                top = std::max(top, cols[k].h);
            }
            if (top < 1.0f) {
                continue;
            }
            if (Rand() < breakShare) {
                float area = 0;
                for (int k = c; k < c + kChunkColumns; k++) {
                    area += cols[k].h * 0.35f;
                    cols[k].h *= 0.65f;
                }
                SpawnChunk(rc.left + c + 1.0f, rc.top - top, area,
                           cap.vx * 0.3f + (Rand() - 0.5f) * 100.0f,
                           tossSpeed * (1.1f + 0.4f * Rand()), 0);
            }
            if (Rand() < 0.04f) {
                SpawnPuff(rc.left + c + 1.0f, rc.top - top,
                          (Rand() - 0.5f) * 60.0f,
                          tossSpeed * (0.4f + 0.5f * Rand()),
                          0.6f + Rand() * 0.8f, 0.6f + Rand() * 0.5f);
            }
        }
    }

    if (impact > 0) {
        cap.squashVel -= std::min(impact * 0.003f, 4.0f);  // squashed
    }
}

// Small things that happen to the snow at rest: the wind blows powder off the
// drifts, and now and then a lump slides off a window corner.
void Overlay::Ambient(float dt) {
    const float maxHeight = static_cast<float>(g_settings.capHeight);
    const float wind = g_settings.wind * 0.6f;
    const float powderRate = 0.4f + std::fabs(wind) / 30.0f;  // per 1000 px

    for (size_t i = 0; i < m_bodies.size(); i++) {
        const Body& b = m_bodies[i];
        auto it = m_caps.find(b.key);
        if (it == m_caps.end()) {
            continue;
        }

        Cap& cap = it->second;

        // The squash and stretch of the whole cap settles like a spring.
        if (cap.squash != 0 || cap.squashVel != 0) {
            cap.squashVel += (-220.0f * cap.squash - 16.0f * cap.squashVel) * dt;
            cap.squash = std::clamp(cap.squash + cap.squashVel * dt, -0.4f,
                                    0.35f);
            if (std::fabs(cap.squash) < 0.002f &&
                std::fabs(cap.squashVel) < 0.02f) {
                cap.squash = cap.squashVel = 0;
            }
        }

        std::vector<Column>& cols = cap.cols;
        const int width = static_cast<int>(cols.size());
        if (cap.airborne || width < 40) {
            continue;
        }

        if (Rand() < powderRate * width / 1000.0f * dt) {
            int c = static_cast<int>(Rand() * width);
            float x = static_cast<float>(b.rc.left + c);
            float y = b.rc.top - cols[c].h;
            if (cols[c].h > 0.6f * SnowLimit(cap, c, maxHeight) &&
                !IsCoveredAbove(i, static_cast<int>(x),
                                static_cast<int>(y) - 1)) {
                SpawnPuff(x, y, wind * (0.5f + Rand()) + (Rand() - 0.5f) * 20.0f,
                          -15.0f - Rand() * 30.0f, 0.5f + Rand() * 0.7f,
                          1.0f + Rand() * 1.2f);
            }
        }

        // A lump slides off one of the corners.
        constexpr int kLump = 6;
        if (Rand() < 0.05f * dt) {
            bool right = Rand() < 0.5f;
            int first = right ? width - kLump - 2 : 2;
            float area = 0;
            for (int c = first; c < first + kLump; c++) {
                area += cols[c].h * 0.5f;
            }
            float x = static_cast<float>(right ? b.rc.right - 2 : b.rc.left + 2);
            if (area > kLump * 0.3f * maxHeight &&
                !IsCoveredAbove(i, static_cast<int>(x), b.rc.top - 2)) {
                for (int c = first; c < first + kLump; c++) {
                    cols[c].h *= 0.5f;
                }
                SpawnChunk(x, b.rc.top - 2.0f, area,
                           (right ? 1.0f : -1.0f) * (25.0f + Rand() * 30.0f),
                           -20.0f, b.key);
            }
        }
    }
}

void Overlay::Sweep(float dt) {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return;
    }

    POINT last = m_lastCursor;
    m_lastCursor = pt;
    if (!m_cursorValid) {
        m_cursorValid = true;
        return;
    }
    if (!g_settings.sweepWithCursor || (pt.x == last.x && pt.y == last.y)) {
        return;
    }

    const float maxHeight = static_cast<float>(g_settings.capHeight);
    const float cursorVx = std::clamp((pt.x - last.x) / dt, -1500.0f, 1500.0f);

    int x0 = std::min(last.x, pt.x) - kSweepRadius;
    int x1 = std::max(last.x, pt.x) + kSweepRadius;
    if (x1 - x0 > 300) {
        // The cursor jumped; only sweep where it is now.
        x0 = pt.x - kSweepRadius;
        x1 = pt.x + kSweepRadius;
    }

    for (size_t i = 0; i < m_bodies.size(); i++) {
        const Body& b = m_bodies[i];
        auto it = m_caps.find(b.key);
        if (it == m_caps.end()) {
            continue;
        }

        Cap& cap = it->second;
        const float top = static_cast<float>(b.rc.top);
        if (pt.y > top + 4 || pt.y < top - 400 || x1 < b.rc.left ||
            x0 >= b.rc.right || IsCoveredAbove(i, pt.x, b.rc.top - 2)) {
            continue;
        }

        std::vector<Column>& cols = cap.cols;
        int from = std::max(x0 - static_cast<int>(b.rc.left), 0);
        int to = std::min(x1 - static_cast<int>(b.rc.left),
                          static_cast<int>(cols.size()));
        for (int c = from; c < to; c += kChunkColumns) {
            float area = 0, height = 0;
            const float base = top - cols[c].lift;
            if (pt.y < base - maxHeight - 6 || pt.y > base + 4) {
                continue;
            }
            for (int k = c; k < std::min(c + kChunkColumns, to); k++) {
                area += cols[k].h;
                height = std::max(height, cols[k].h);
                cols[k].h = 0;
            }
            if (area < 0.5f) {
                continue;
            }
            SpawnChunk(b.rc.left + c + 1.0f, base - height / 2, area,
                       cursorVx * 0.35f + (Rand() - 0.5f) * 80.0f,
                       -Rand() * 120.0f, b.key);
            if (Rand() < 0.35f) {
                SpawnPuff(b.rc.left + c + 1.0f, base - height,
                          cursorVx * 0.2f + (Rand() - 0.5f) * 60.0f,
                          -30.0f - Rand() * 60.0f, 0.6f + Rand() * 0.7f,
                          0.5f + Rand() * 0.5f);
            }
        }
    }
}

void Overlay::UpdateFlakes(float dt) {
    m_time += dt;

    // 100% is 150 flakes per 1920x1080 of screen area.
    size_t target = static_cast<size_t>(
        g_settings.intensity * 1.5 *
        (static_cast<double>(m_width) * m_height) / (1920.0 * 1080.0));
    if (m_flakes.size() > target) {
        m_flakes.resize(target);
    }
    while (m_flakes.size() < target) {
        Flake f;
        SpawnFlake(f, true);
        m_flakes.push_back(f);
    }

    const float speedScale = g_settings.speed / 100.0f;
    const float wind = g_settings.wind * 0.6f;
    const bool caps = g_settings.capHeight > 0;
    const float left = static_cast<float>(m_area.left);
    const float right = static_cast<float>(m_area.right);

    for (Flake& f : m_flakes) {
        float oldY = f.y;
        f.y += f.fallSpeed * speedScale * dt;
        f.x += (wind * (0.6f + 0.4f * f.size / 3.5f) +
                std::sin(m_time * 1.3f + f.phase) * f.sway) *
               dt;

        if (f.x > right + 10) {
            f.x -= m_width + 20;
        } else if (f.x < left - 10) {
            f.x += m_width + 20;
        }

        if (caps) {
            if (const Body* b = FindLanding(f.x, oldY, f.y, 0)) {
                // Each flake lands on ~9 columns, so this is tuned for a full
                // cap to build up in about half a minute.
                AddSnow(b->key, static_cast<int>(f.x) - b->rc.left,
                        2.0f + 1.5f * f.size);
                SpawnFlake(f, false);
                continue;
            }
        }

        if (f.y > m_area.bottom + 5) {
            SpawnFlake(f, false);
        }
    }
}

void Overlay::UpdateChunks(float dt) {
    const float drag = std::exp(-2.0f * dt);
    size_t kept = 0;
    for (size_t i = 0; i < m_chunks.size(); i++) {
        Chunk c = m_chunks[i];
        float oldY = c.y;
        c.vy = std::min(c.vy + kGravity * 0.5f * dt, kChunkMaxSpeed);
        c.vx *= drag;
        c.x += c.vx * dt;
        c.y += c.vy * dt;

        if (const Body* b = FindLanding(c.x, oldY, c.y, c.ignoreKey)) {
            AddSnow(b->key, static_cast<int>(c.x) - b->rc.left,
                    c.area / kSnowKernelSum);
            if (c.vy > 450.0f && Rand() < 0.5f) {
                SpawnPuff(c.x, static_cast<float>(b->rc.top) - 2,
                          c.vx * 0.3f + (Rand() - 0.5f) * 70.0f,
                          -30.0f - Rand() * 50.0f, 0.5f + c.size * 0.3f,
                          0.4f + Rand() * 0.3f);
            }
            continue;
        }
        if (c.y > m_area.bottom + 10 || c.x < m_area.left - 50 ||
            c.x > m_area.right + 50) {
            continue;
        }
        m_chunks[kept++] = c;
    }
    m_chunks.resize(kept);
}

void Overlay::UpdatePuffs(float dt) {
    const float drag = std::exp(-3.0f * dt);
    const float wind = g_settings.wind * 0.6f;
    size_t kept = 0;
    for (size_t i = 0; i < m_puffs.size(); i++) {
        Puff p = m_puffs[i];
        p.life -= dt;
        if (p.life <= 0) {
            continue;
        }
        p.vx = p.vx * drag + wind * (1.0f - drag);
        p.vy = p.vy * drag + 40.0f * dt;
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        m_puffs[kept++] = p;
    }
    m_puffs.resize(kept);
}

void Overlay::DrawCap(const Cap& cap, const RECT& rc) {
    const std::vector<Column>& cols = cap.cols;
    int width = static_cast<int>(cols.size());
    if (width < 2) {
        return;
    }

    float highest = 0, liftSum = 0;
    int snowy = 0;
    for (const Column& col : cols) {
        highest = std::max(highest, col.h);
        if (col.h > 0.2f) {
            liftSum += col.lift;
            snowy++;
        }
    }
    if (highest < 0.2f) {
        return;
    }

    ComPtr<ID2D1PathGeometry> path;
    ComPtr<ID2D1GeometrySink> sink;
    if (FAILED(m_d2dFactory->CreatePathGeometry(&path)) ||
        FAILED(path->Open(&sink))) {
        return;
    }

    // The screen shows windows a frame later than they're reported, so the
    // snow is drawn a frame ahead while a window moves. Otherwise it would
    // trail behind, and sink into the window when it moves up.
    // Snow that slides off moved windows stays still on the screen.
    const float dx = g_settings.slideOff
                         ? 0.0f
                         : std::clamp(cap.vx * m_frameTime, -40.0f, 40.0f);
    const float dy = -std::min(std::fabs(cap.vy) * m_frameTime, 40.0f);
    const float left = rc.left + dx;
    const float top = rc.top + dy;
    const float scale = 1.0f + cap.squash;

    auto height = [&](int c) {
        // Round off the cap at the window corners.
        float edge = static_cast<float>(std::min(c, width - 1 - c));
        return cols[c].h * scale * std::clamp(edge / kCapEdgeTaper, 0.0f, 1.0f);
    };

    // One figure per run of snow, so that bare stretches aren't drawn as thin
    // lines while the snow is in the air.
    constexpr float kMinHeight = 0.3f;
    for (int c = 0; c < width;) {
        if (height(c) < kMinHeight) {
            c++;
            continue;
        }
        int first = c;
        while (c < width && height(c) >= kMinHeight) {
            c++;
        }
        int last = c - 1;

        float x0 = left + first;
        float x1 = left + last + 1.0f;
        sink->BeginFigure({x0, top - cols[first].lift},
                          D2D1_FIGURE_BEGIN_FILLED);
        for (int k = first; k <= last; k += 2) {
            sink->AddLine({left + k + 0.5f, top - cols[k].lift - height(k)});
        }
        sink->AddLine({x1 - 0.5f, top - cols[last].lift - height(last)});
        sink->AddLine({x1, top - cols[last].lift});
        if (cap.airborne) {
            for (int k = last; k >= first; k -= 2) {
                sink->AddLine({left + k + 0.5f, top - cols[k].lift});
            }
        }
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    }
    if (FAILED(sink->Close())) {
        return;
    }

    const float base = top - (snowy ? liftSum / snowy : 0.0f);
    m_capBrush->SetStartPoint({0, base - highest * scale});
    m_capBrush->SetEndPoint({0, base});
    m_ctx->FillGeometry(path.Get(), m_capBrush.Get());
}

bool Overlay::Render() {
    m_ctx->BeginDraw();
    m_ctx->SetTransform(D2D1::Matrix3x2F::Translation(
        static_cast<float>(-m_area.left), static_cast<float>(-m_area.top)));
    m_ctx->Clear(D2D1::ColorF(0, 0, 0, 0));

    // Draw from the bottom window to the top one. Each window first erases the
    // snow of the windows below it that it covers.
    bool anyDrawn = false;
    for (size_t i = m_bodies.size(); i-- > 0;) {
        const Body& b = m_bodies[i];
        if (anyDrawn) {
            m_ctx->PushAxisAlignedClip(
                D2D1::RectF(static_cast<float>(b.rc.left),
                            static_cast<float>(b.rc.top),
                            static_cast<float>(b.rc.right),
                            static_cast<float>(b.rc.bottom)),
                D2D1_ANTIALIAS_MODE_ALIASED);
            m_ctx->Clear(D2D1::ColorF(0, 0, 0, 0));
            m_ctx->PopAxisAlignedClip();
        }

        if (!b.snowCap) {
            continue;
        }
        auto it = m_caps.find(b.key);
        if (it != m_caps.end()) {
            DrawCap(it->second, b.rc);
            anyDrawn = true;
        }
    }

    for (const Chunk& c : m_chunks) {
        float r = c.size * 2.0f;
        m_ctx->DrawBitmap(m_flakeSprite.Get(),
                          D2D1::RectF(c.x - r, c.y - r, c.x + r, c.y + r), 0.95f,
                          D2D1_INTERPOLATION_MODE_LINEAR);
    }

    for (const Puff& p : m_puffs) {
        float r = p.size * 2.0f;
        float alpha = 0.7f * std::min(1.0f, 2.5f * p.life / p.maxLife);
        m_ctx->DrawBitmap(m_flakeSprite.Get(),
                          D2D1::RectF(p.x - r, p.y - r, p.x + r, p.y + r),
                          alpha, D2D1_INTERPOLATION_MODE_LINEAR);
    }

    for (const Flake& f : m_flakes) {
        float r = f.size * 2.0f;
        m_ctx->DrawBitmap(m_flakeSprite.Get(),
                          D2D1::RectF(f.x - r, f.y - r, f.x + r, f.y + r),
                          f.alpha, D2D1_INTERPOLATION_MODE_LINEAR);
    }

    if (FAILED(m_ctx->EndDraw())) {
        return false;
    }

    HRESULT hr = m_swapChain->Present(m_syncInterval, 0);
    return SUCCEEDED(hr) || hr == DXGI_STATUS_OCCLUDED;
}

void Overlay::Run() {
    LARGE_INTEGER freq, last;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&last);

    for (;;) {
        DWORD result;
        if (m_visible && m_frameWait) {
            HANDLE handles[] = {g_stopEvent, m_frameWait};
            result = WaitForMultipleObjects(2, handles, FALSE, 200);
        } else {
            // Hidden: just check now and then whether it's time to come back.
            result = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE,
                                               m_visible ? 16 : 100,
                                               QS_ALLINPUT);
        }
        if (result == WAIT_OBJECT_0) {
            break;
        }

        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        float dt = static_cast<float>(now.QuadPart - last.QuadPart) /
                   freq.QuadPart;
        last = now;
        dt = std::clamp(dt, 0.001f, 0.05f);
        m_frameTime = m_frameTime * 0.9f + dt * 0.1f;
        m_frame++;

        bool fullscreen =
            g_settings.pauseInFullscreen && IsFullscreenAppActive();
        if (fullscreen == m_visible) {
            m_visible = !fullscreen;
            ShowWindow(m_hwnd, m_visible ? SW_SHOWNOACTIVATE : SW_HIDE);
            m_cursorValid = false;
        }
        if (!m_visible) {
            continue;
        }

        if (m_frame % 60 == 0) {
            RECT area = ComputeArea();
            if (!EqualRect(&area, &m_area)) {
                ApplyArea(area);
            }
            m_syncInterval = ComputeSyncInterval();
            // Other topmost windows (the taskbar, for one) can end up above
            // the overlay.
            SetWindowPos(m_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }

        CollectBodies();
        UpdateCaps(dt);
        Ambient(dt);
        Sweep(dt);
        UpdateFlakes(dt);
        UpdateChunks(dt);
        UpdatePuffs(dt);

        if (!Render()) {
            // The GPU device was lost (driver update, adapter change).
            Wh_Log(L"Recreating device resources");
            ReleaseDeviceResources();
            if (!CreateDeviceResources()) {
                Wh_Log(L"Failed to recreate device resources");
                break;
            }
        }
    }
}

DWORD WINAPI SnowThread(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    // Only the explorer.exe process that owns the taskbar shows the snow.
    for (;;) {
        HWND tray = FindWindow(L"Shell_TrayWnd", nullptr);
        if (tray) {
            DWORD pid = 0;
            GetWindowThreadProcessId(tray, &pid);
            if (pid != GetCurrentProcessId()) {
                return 0;
            }
            break;
        }
        if (WaitForSingleObject(g_stopEvent, 1000) == WAIT_OBJECT_0) {
            return 0;
        }
    }

    g_taskbarThread = CreateThread(nullptr, 0, TaskbarThread, nullptr, 0,
                                   nullptr);

    {
        Overlay overlay;
        if (overlay.Create()) {
            overlay.Run();
        } else {
            Wh_Log(L"Failed to create the overlay");
        }
    }

    if (g_taskbarThread) {
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_taskbarThread, INFINITE);
        CloseHandle(g_taskbarThread);
        g_taskbarThread = nullptr;
    }
    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }

    g_snowThread = CreateThread(nullptr, 0, SnowThread, nullptr, 0, nullptr);
    if (!g_snowThread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    SetEvent(g_stopEvent);
    WaitForSingleObject(g_snowThread, INFINITE);
    CloseHandle(g_snowThread);
    CloseHandle(g_stopEvent);
    g_snowThread = nullptr;
    g_stopEvent = nullptr;
}
