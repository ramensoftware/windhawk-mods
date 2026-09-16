// ==WindhawkMod==
// @id              theme-aware-video-wallpaper-ffmpeg
// @name            Video Wallpaper (Follow Theme)
// @name:zh-CN      视频壁纸（跟随主题）
// @description     Auto switch video wallpaper based on Windows theme. Currently only tested on the latest Windows 11. Desktop icon layering issue and audio playback not yet resolved. Community contributions welcome.
// @description:zh-CN  根据 Windows 主题自动切换深浅色视频。仅在最新版 Win11 进行测试，尚未解决桌面图标层级问题和声音播放需求。有待大家一起开发。
// @version         1.0
// @author          wakhh@qq.com
// @github          https://github.com/wakhh
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldxgi -ld2d1 -ld3d11 -ldcomp -ldwmapi -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Theme Aware Video Wallpaper

Plays a video as desktop wallpaper via ffmpeg pipe + UpdateLayeredWindow.
Auto-switches between light/dark mode videos based on Windows theme.

### Requirements
- **ffmpeg.exe** (standalone binary, no extra DLLs/codecs needed)
- Better developers.

Hoping someone else will carry on development — PRs welcome.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ffmpegPath: ""
  $name: ffmpeg.exe path
  $name:zh-CN: ffmpeg.exe 路径
  $description: Full path to ffmpeg.exe
  $description:zh-CN: ffmpeg.exe 的完整路径
- lightVideoPath: ""
  $name: Light mode video / folder path
  $name:zh-CN: 浅色模式视频 / 文件夹路径
  $description: "Full path to video file OR folder containing videos (folder plays by latest modified time)"
  $description:zh-CN: "视频文件完整路径，或包含视频的文件夹路径（文件夹按修改时间最新排序播放）"
- darkVideoPath: ""
  $name: Dark mode video / folder path
  $name:zh-CN: 深色模式视频 / 文件夹路径
  $description: "Leave empty to reuse light mode"
  $description:zh-CN: "留空则复用浅色模式设置"
- sortMode: "0"
  $name: Folder playback sort order
  $name:zh-CN: 文件夹播放排序方式
  $description: "Sort order when playing videos from a folder"
  $description:zh-CN: "文件夹模式下视频的排序方式"
  $options:
    - "0": "Modified time (newest first)"
    - "1": "Modified time (oldest first)"
    - "2": "Name (A to Z)"
    - "3": "Name (Z to A)"
    - "4": "Created time (oldest first)"
    - "5": "Created time (newest first)"
    - "6": "Size (largest first)"
    - "7": "Size (smallest first)"
  $options:zh-CN:
    - "0": "修改时间（新到旧）"
    - "1": "修改时间（旧到新）"
    - "2": "文件名（A 到 Z）"
    - "3": "文件名（Z 到 A）"
    - "4": "创建时间（旧到新）"
    - "5": "创建时间（新到旧）"
    - "6": "文件大小（大到小）"
    - "7": "文件大小（小到大）"
- hwaccelMode: "0"
  $name: Hardware acceleration
  $name:zh-CN: 硬件加速
  $description: "D3D11VA hardware decoding via ffmpeg. May fail on some GPUs or codecs"
  $description:zh-CN: "通过 ffmpeg 使用 D3D11VA 硬件解码，部分显卡或编码可能不支持"
  $options:
    - "0": "Disabled (software decode)"
    - "1": "Enabled (D3D11VA)"
  $options:zh-CN:
    - "0": "关闭（纯 CPU 软解）"
    - "1": "开启（D3D11VA）"
- fps: 15
  $name: Frame rate (fps)
  $name:zh-CN: 帧率（fps）
  $description: "Output frame rate. Range 10-60. Lower values save CPU. Videos with a lower native fps will have frames duplicated"
  $description:zh-CN: "输出帧率。范围 10-60。值越低越省 CPU。视频原始帧率低于设置值时会重复帧"
- opacity: 100
  $name: Video window opacity (%)
  $name:zh-CN: 视频窗口不透明度（%）
  $description: "Overall transparency of the video wallpaper. 100 = fully opaque, lower values let the desktop show through."
  $description:zh-CN: "视频壁纸的整体不透明度。100 = 完全不透明，值越低桌面越明显。"
- scalingMode: "0"
  $name: Video scaling mode
  $name:zh-CN: 视频缩放模式
  $description: "How the video is scaled to fit the screen"
  $description:zh-CN: "视频如何缩放以适应屏幕"
  $options:
    - "0": "Cover (fill screen, keep ratio, crop edges)"
    - "1": "Contain (fit screen, keep ratio, pad bars)"
    - "2": "Center (original size, centered, pad bars)"
    - "3": "Stretch (fill screen, may distort)"
  $options:zh-CN:
    - "0": "填充（保持比例，裁剪边缘）"
    - "1": "适应（保持比例，可能有空置边缘）"
    - "2": "居中（原尺寸居中，可能有空置边缘）"
    - "3": "拉伸（填满屏幕，可能变形）"
- padColorMode: "black"
  $name: Pad color (for Contain/Center modes)
  $name:zh-CN: 空置边缘颜色（适应/居中模式）
  $description: "Color of the padding area around the video"
  $description:zh-CN: "视频周围空置边缘的颜色"
  $options:
    - "black": "Black"
    - "white": "White"
    - "dynamic": "Dynamic (white in light mode, black in dark mode)"
    - "transparent": "Transparent (shows desktop wallpaper underneath)"
    - "custom": "Custom color (set below)"
  $options:zh-CN:
    - "black": "黑色"
    - "white": "白色"
    - "dynamic": "动态（浅色模式白色，深色模式黑色）"
    - "transparent": "透明（露出底下的桌面壁纸）"
    - "custom": "自定义颜色（在下方设置）"
- padColorCustom: ""
  $name: Custom pad color (hex)
  $name:zh-CN: 自定义边缘颜色（十六进制）
  $description: "Custom color for padding when Pad color is set to Custom. 6-digit RGB or 8-digit ARGB. Examples: #FF0000 for red, #80FF0000 for semi-transparent red, #00000000 for fully transparent"
  $description:zh-CN: "空置边缘颜色设为自定义时使用。6 位 RGB 或 8 位 ARGB。例如 #FF0000 为红色，#80FF0000 为半透明红，#00000000 为全透明"
- applyMode: "on_next"
  $name: Settings/theme apply timing
  $name:zh-CN: 设置/主题生效时机
  $description: "When a setting is changed, theme switches, or files change"
  $description:zh-CN: "当设置改变、主题切换或文件变化时"
  $options:
    - "on_next": "Wait until current video finishes, then apply"
    - "instant": "Instant (stop current video, apply immediately)"
  $options:zh-CN:
    - "on_next": "等当前视频播完后生效"
    - "instant": "立即停止当前视频并生效"
*/
// ==/WindhawkModSettings==

#include <Windows.h>
#include <windhawk_utils.h>
#include <d2d1_1.h>
#include <d2d1helper.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dwmapi.h>
#include <dxgi1_3.h>
#include <wrl/client.h>
#include <vector>
#include <string>
#include <atomic>
#include <algorithm>

static HANDLE g_ffmpegProc = NULL;
static HANDLE g_ffmpegWaitReg = NULL;
static HANDLE g_pipeRead = NULL;
static HANDLE g_pipeWrite = NULL;
static HWND g_videoHwnd = NULL;
static int g_frameSize = 0;
static std::vector<BYTE> g_frameBuf;
WCHAR g_ffmpegPath[MAX_PATH] = {0};
WCHAR g_lightPath[MAX_PATH] = {0};
WCHAR g_darkPath[MAX_PATH] = {0};
int g_opacity = 255;
int g_fps = 15;
int g_scalingMode = 0;
WCHAR g_padColorMode[32] = {L"black"};
WCHAR g_padColorCustom[32] = {0};
WCHAR g_applyMode[16] = {L"instant"};
int g_sortMode = 0;
int g_hwaccelMode = 0;
volatile bool g_pendingReload = false;
const bool g_pauseOnFullscreen = true;

HANDLE g_pipeThread = NULL;
std::atomic<bool> g_frameReady{false};
HANDLE g_dirChangeHandle = INVALID_HANDLE_VALUE;
bool g_classRegistered = false;
std::atomic<bool> g_running{true};
std::atomic<bool> g_isPaused{false};
bool g_lastIsDark = false;
HANDLE g_mutex = NULL;
std::vector<std::wstring> g_videoList;
size_t g_videoIndex = 0;
int g_consecutiveErrors = 0;
static const int MAX_CONSECUTIVE_ERRORS = 3;

HWND g_workerW = NULL;
HKEY g_hThemeKey = NULL;
HANDLE g_hThemeEvt = NULL;
bool g_lastDarkChecked = false;
bool g_prevFull = false;

UINT_PTR g_createVideoTimer = 0;
constexpr UINT WM_APP_CLEANUP = WM_APP + 1;
constexpr UINT WM_APP_FFMPEG_EXIT = WM_APP + 2;

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

[[clang::no_destroy]] Microsoft::WRL::ComPtr<ID3D11Device> g_d3dDevice;
[[clang::no_destroy]] Microsoft::WRL::ComPtr<IDXGIDevice> g_dxgiDevice;
[[clang::no_destroy]] Microsoft::WRL::ComPtr<IDXGIFactory2> g_dxgiFactory;
[[clang::no_destroy]] Microsoft::WRL::ComPtr<ID2D1Factory1> g_d2dFactory;
[[clang::no_destroy]] Microsoft::WRL::ComPtr<ID2D1Device> g_d2dDevice;
[[clang::no_destroy]] Microsoft::WRL::ComPtr<IDXGISwapChain1> g_swapChain;
[[clang::no_destroy]] Microsoft::WRL::ComPtr<ID2D1DeviceContext> g_dc;
[[clang::no_destroy]] Microsoft::WRL::ComPtr<IDCompositionDevice> g_compositionDevice;
[[clang::no_destroy]] Microsoft::WRL::ComPtr<IDCompositionTarget> g_compositionTarget;
[[clang::no_destroy]] Microsoft::WRL::ComPtr<IDCompositionVisual> g_compositionVisual;
[[clang::no_destroy]] Microsoft::WRL::ComPtr<ID2D1Bitmap> g_frameBitmap;

bool g_dxInitSucceeded = false;
bool g_dxInitialized = false;

constexpr UINT TIMER_ID_MANAGER = 1;
constexpr UINT TIMER_ID_RENDERER = 2;

bool PlayNext();
void RenderFrame();
void StartRendererTimer();
void StopRendererTimer();
void StartManagerTimer();
void StopManagerTimer();
void ManagerTick();
bool CreateVideoWindow();
void Wh_ModSettingsChanged();
HWND GetWorkerW();

HMODULE GetCurrentModuleHandle() {
    HMODULE module = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                      GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&GetCurrentModuleHandle, &module);
    return module;
}

bool IsFolderViewWnd(HWND hWnd) {
    WCHAR buffer[64];
    if (!GetClassName(hWnd, buffer, ARRAYSIZE(buffer)) ||
        _wcsicmp(buffer, L"SysListView32")) return false;
    if (!GetWindowText(hWnd, buffer, ARRAYSIZE(buffer)) ||
        _wcsicmp(buffer, L"FolderView")) return false;
    HWND hParent = GetAncestor(hWnd, GA_PARENT);
    if (!hParent) return false;
    if (!GetClassName(hParent, buffer, ARRAYSIZE(buffer)) ||
        _wcsicmp(buffer, L"SHELLDLL_DefView")) return false;
    if (GetWindowTextLength(hParent) > 0) return false;
    HWND hParent2 = GetAncestor(hParent, GA_PARENT);
    if (!hParent2) return false;
    if ((!GetClassName(hParent2, buffer, ARRAYSIZE(buffer)) ||
         _wcsicmp(buffer, L"Progman")) && hParent2 != GetShellWindow()) return false;
    return true;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
    static const UINT registeredMsg = RegisterWindowMessage(L"Windhawk_RunFromWindowThread_VidWallpaper");
    struct PARAM { RunFromWindowThreadProc_t proc; void* param; };
    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid) return false;
    if (tid == GetCurrentThreadId()) { proc(procParam); return true; }
    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == *(UINT*)&registeredMsg) {
                    PARAM* p = (PARAM*)cwp->lParam;
                    p->proc(p->param);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        }, nullptr, tid);
    if (!hook) return false;
    PARAM param = { proc, procParam };
    SendMessage(hWnd, registeredMsg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return true;
}

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
        dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd || !IsFolderViewWnd(hWnd)) return hWnd;
    if (!g_createVideoTimer) {
        g_createVideoTimer = SetTimer(nullptr, 0, 1000,
            [](HWND, UINT, UINT_PTR, DWORD) {
                KillTimer(nullptr, g_createVideoTimer);
                g_createVideoTimer = 0;
                if (!CreateVideoWindow()) {
                    Wh_Log(L"CreateVideoWindow failed in hook timer");
                    return;
                }
                Wh_ModSettingsChanged();
            });
    }
    return hWnd;
}

HWND GetProgmanWnd()
{
    HWND hProgman = FindWindowW(L"Progman", nullptr);
    if (!hProgman) return nullptr;
    DWORD progmanProcessId = 0;
    GetWindowThreadProcessId(hProgman, &progmanProcessId);
    if (progmanProcessId != GetCurrentProcessId()) return nullptr;
    return hProgman;
}

HWND GetWorkerW()
{
    HWND hProgman = GetProgmanWnd();
    if (!hProgman) return nullptr;

    SendMessage(hProgman, 0x052C, 0xD, 0);
    SendMessage(hProgman, 0x052C, 0xD, 1);

    HWND hWorkerW = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            if (!FindWindowExW(hWnd, nullptr, L"SHELLDLL_DefView", nullptr)) return TRUE;
            HWND hWorker = FindWindowExW(nullptr, hWnd, L"WorkerW", nullptr);
            if (hWorker) { *(HWND*)lParam = hWorker; return FALSE; }
            return TRUE;
        },
        (LPARAM)&hWorkerW);

    if (!hWorkerW) {
        SendMessage(hProgman, 0x052C, 0, 0);
        EnumWindows(
            [](HWND hWnd, LPARAM lParam) -> BOOL {
                if (!FindWindowExW(hWnd, nullptr, L"SHELLDLL_DefView", nullptr)) return TRUE;
                HWND hWorker = FindWindowExW(nullptr, hWnd, L"WorkerW", nullptr);
                if (hWorker) { *(HWND*)lParam = hWorker; return FALSE; }
                return TRUE;
            },
            (LPARAM)&hWorkerW);
    }

    if (!hWorkerW) hWorkerW = FindWindowExW(hProgman, nullptr, L"WorkerW", nullptr);
    if (!hWorkerW) hWorkerW = hProgman;
    return hWorkerW;
}

void RunDxgiWorkaroundForExplorerPatcher()
{
    auto dxgiModule = GetModuleHandleW(L"dxgi.dll");
    if (!dxgiModule) return;

    WCHAR dxgiPath[MAX_PATH];
    DWORD len = GetModuleFileNameW(dxgiModule, dxgiPath, MAX_PATH);
    if (len == 0 || len == MAX_PATH) return;

    WCHAR epDxgiPath[MAX_PATH];
    GetWindowsDirectoryW(epDxgiPath, MAX_PATH);
    wcscat_s(epDxgiPath, L"\\dxgi.dll");

    if (_wcsicmp(dxgiPath, epDxgiPath) != 0) return;

    auto dxgiDeclare = (HRESULT(WINAPI*)())GetProcAddress(dxgiModule, "DXGIDeclareAdapterRemovalSupport");
    if (dxgiDeclare) dxgiDeclare();
}

bool InitDirectX()
{
    HRESULT hr;

    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
                           nullptr, 0, D3D11_SDK_VERSION, &g_d3dDevice, nullptr, nullptr);
    if (FAILED(hr)) {
        Wh_Log(L"D3D11CreateDevice HARDWARE failed: 0x%08X, trying WARP", hr);
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, flags,
                               nullptr, 0, D3D11_SDK_VERSION, &g_d3dDevice, nullptr, nullptr);
        if (FAILED(hr)) { Wh_Log(L"D3D11CreateDevice WARP also failed: 0x%08X", hr); return false; }
        Wh_Log(L"D3D11CreateDevice: using WARP (software)");
    }

    hr = g_d3dDevice.As(&g_dxgiDevice);
    if (FAILED(hr)) { Wh_Log(L"QueryInterface IDXGIDevice failed: 0x%08X", hr); return false; }

    hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&g_dxgiFactory));
    if (FAILED(hr)) { Wh_Log(L"CreateDXGIFactory2 failed: 0x%08X", hr); return false; }

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&g_d2dFactory));
    if (FAILED(hr)) { Wh_Log(L"D2D1CreateFactory failed: 0x%08X", hr); return false; }

    hr = g_d2dFactory->CreateDevice(g_dxgiDevice.Get(), &g_d2dDevice);
    if (FAILED(hr)) { Wh_Log(L"D2D CreateDevice failed: 0x%08X", hr); return false; }

    return true;
}

void UninitDirectX()
{
    g_frameBitmap.Reset();
    g_swapChain.Reset();
    g_dc.Reset();
    g_compositionVisual.Reset();
    g_compositionTarget.Reset();
    g_compositionDevice.Reset();
    g_d2dDevice.Reset();
    g_d2dFactory.Reset();
    g_dxgiFactory.Reset();
    g_dxgiDevice.Reset();
    g_d3dDevice.Reset();
    g_dxInitialized = false;
    g_dxInitSucceeded = false;
}

bool CreateSwapChainResources(UINT width, UINT height)
{
    HRESULT hr;

    DXGI_FORMAT targetFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    D2D1_ALPHA_MODE d2dAlphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;

    DXGI_SWAP_CHAIN_DESC1 scd = {};
    scd.Width = width;
    scd.Height = height;
    scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    scd.SampleDesc.Count = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 2;
    scd.Scaling = DXGI_SCALING_STRETCH;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

    hr = g_dxgiFactory->CreateSwapChainForComposition(g_dxgiDevice.Get(), &scd, nullptr, &g_swapChain);
    if (FAILED(hr)) { Wh_Log(L"CreateSwapChainForComposition failed: 0x%08X", hr); return false; }

    hr = g_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &g_dc);
    if (FAILED(hr)) { Wh_Log(L"CreateDeviceContext failed: 0x%08X", hr); return false; }

    Microsoft::WRL::ComPtr<IDXGISurface2> surface;
    hr = g_swapChain->GetBuffer(0, IID_PPV_ARGS(&surface));
    if (FAILED(hr)) { Wh_Log(L"GetBuffer failed: 0x%08X", hr); return false; }

    D2D1_BITMAP_PROPERTIES1 bitmapProperties = {};
    bitmapProperties.pixelFormat.alphaMode = d2dAlphaMode;
    bitmapProperties.pixelFormat.format = targetFormat;
    bitmapProperties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

    Microsoft::WRL::ComPtr<ID2D1Bitmap1> targetBitmap;
    hr = g_dc->CreateBitmapFromDxgiSurface(surface.Get(), bitmapProperties, &targetBitmap);
    if (FAILED(hr)) { Wh_Log(L"CreateBitmapFromDxgiSurface failed: 0x%08X", hr); return false; }

    g_dc->SetTarget(targetBitmap.Get());

    hr = DCompositionCreateDevice(g_dxgiDevice.Get(), IID_PPV_ARGS(&g_compositionDevice));
    if (FAILED(hr)) { Wh_Log(L"DCompositionCreateDevice failed: 0x%08X", hr); return false; }

    hr = g_compositionDevice->CreateTargetForHwnd(g_videoHwnd, TRUE, &g_compositionTarget);
    if (FAILED(hr)) { Wh_Log(L"CreateTargetForHwnd failed: 0x%08X", hr); return false; }

    hr = g_compositionDevice->CreateVisual(&g_compositionVisual);
    if (FAILED(hr)) { Wh_Log(L"CreateVisual failed: 0x%08X", hr); return false; }

    hr = g_compositionVisual->SetContent(g_swapChain.Get());
    if (FAILED(hr)) { Wh_Log(L"SetContent failed: 0x%08X", hr); return false; }

    hr = g_compositionTarget->SetRoot(g_compositionVisual.Get());
    if (FAILED(hr)) { Wh_Log(L"SetRoot failed: 0x%08X", hr); return false; }

    hr = g_compositionDevice->Commit();
    if (FAILED(hr)) { Wh_Log(L"Commit failed: 0x%08X", hr); return false; }

    D2D1_BITMAP_PROPERTIES frameBitmapProps = D2D1::BitmapProperties(
        D2D1::PixelFormat(targetFormat, d2dAlphaMode));
    hr = g_dc->CreateBitmap(D2D1::SizeU(width, height), nullptr, width * 4, frameBitmapProps, &g_frameBitmap);
    if (FAILED(hr)) { Wh_Log(L"CreateBitmap (frame) failed: 0x%08X", hr); return false; }

    return true;
}

void ReleaseSwapChainResources()
{
    g_frameBitmap.Reset();
    g_swapChain.Reset();
    g_dc.Reset();
    g_compositionVisual.Reset();
    g_compositionTarget.Reset();
    g_compositionDevice.Reset();
}

bool ResizeSwapChain(UINT width, UINT height)
{
    if (!g_swapChain || !g_dc) return false;

    g_dc->SetTarget(nullptr);

    HRESULT hr = g_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr)) {
        Wh_Log(L"ResizeBuffers failed: 0x%08X", hr);
        return false;
    }

    Microsoft::WRL::ComPtr<IDXGISurface2> surface;
    hr = g_swapChain->GetBuffer(0, IID_PPV_ARGS(&surface));
    if (FAILED(hr)) {
        Wh_Log(L"GetBuffer failed: 0x%08X", hr);
        return false;
    }

    D2D1_BITMAP_PROPERTIES1 bitmapProperties = {};
    bitmapProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bitmapProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bitmapProperties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

    Microsoft::WRL::ComPtr<ID2D1Bitmap1> targetBitmap;
    hr = g_dc->CreateBitmapFromDxgiSurface(surface.Get(), bitmapProperties, &targetBitmap);
    if (FAILED(hr)) {
        Wh_Log(L"CreateBitmapFromDxgiSurface failed: 0x%08X", hr);
        return false;
    }

    g_dc->SetTarget(targetBitmap.Get());

    g_frameSize = (int)(width * height * 4);
    g_frameBuf.clear();
    g_frameBuf.resize(g_frameSize);

    D2D1_BITMAP_PROPERTIES frameBitmapProps = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
    g_frameBitmap.Reset();
    hr = g_dc->CreateBitmap(D2D1::SizeU(width, height), nullptr, width * 4, frameBitmapProps, &g_frameBitmap);
    if (FAILED(hr)) {
        Wh_Log(L"CreateBitmap (frame) failed: 0x%08X", hr);
        return false;
    }

    return true;
}

bool EnsureDxInitialized()
{
    if (g_dxInitialized) return g_dxInitSucceeded;
    g_dxInitialized = true;
    RunDxgiWorkaroundForExplorerPatcher();
    g_dxInitSucceeded = InitDirectX();
    return g_dxInitSucceeded;
}

bool IsVideoExt(const WCHAR* ext)
{
    return ext && (_wcsicmp(ext, L".mp4") == 0 || _wcsicmp(ext, L".mkv") == 0 ||
        _wcsicmp(ext, L".mov") == 0 || _wcsicmp(ext, L".webm") == 0 ||
        _wcsicmp(ext, L".avi") == 0 || _wcsicmp(ext, L".m4v") == 0 ||
        _wcsicmp(ext, L".wmv") == 0);
}

struct VideoEntry {
    std::wstring path;
    std::wstring name;
    FILETIME writeTime;
    FILETIME createTime;
    ULONGLONG size;
};

std::vector<std::wstring> EnumVideos(const WCHAR* folder)
{
    std::vector<VideoEntry> entries;
    WCHAR path[MAX_PATH];
    swprintf_s(path, L"%s\\*", folder);
    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(path, &fd);
    if (hFind == INVALID_HANDLE_VALUE) return {};
    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            WCHAR ext[_MAX_EXT];
            _wsplitpath_s(fd.cFileName, NULL, 0, NULL, 0, NULL, 0, ext, _MAX_EXT);
            if (IsVideoExt(ext)) {
                WCHAR full[MAX_PATH];
                swprintf_s(full, L"%s\\%s", folder, fd.cFileName);
                ULONGLONG size = ((ULONGLONG)fd.nFileSizeHigh << 32) | fd.nFileSizeLow;
                entries.push_back({full, fd.cFileName, fd.ftLastWriteTime, fd.ftCreationTime, size});
            }
        }
    } while (FindNextFileW(hFind, &fd));
    FindClose(hFind);

    std::sort(entries.begin(), entries.end(), [](const VideoEntry& a, const VideoEntry& b) {
        switch (g_sortMode) {
            case 1:
                if (a.writeTime.dwHighDateTime != b.writeTime.dwHighDateTime)
                    return a.writeTime.dwHighDateTime < b.writeTime.dwHighDateTime;
                return a.writeTime.dwLowDateTime < b.writeTime.dwLowDateTime;
            case 2: return _wcsicmp(a.name.c_str(), b.name.c_str()) < 0;
            case 3: return _wcsicmp(a.name.c_str(), b.name.c_str()) > 0;
            case 4:
                if (a.createTime.dwHighDateTime != b.createTime.dwHighDateTime)
                    return a.createTime.dwHighDateTime < b.createTime.dwHighDateTime;
                return a.createTime.dwLowDateTime < b.createTime.dwLowDateTime;
            case 5:
                if (a.createTime.dwHighDateTime != b.createTime.dwHighDateTime)
                    return a.createTime.dwHighDateTime > b.createTime.dwHighDateTime;
                return a.createTime.dwLowDateTime > b.createTime.dwLowDateTime;
            case 6: return a.size > b.size;
            case 7: return a.size < b.size;
            case 0:
            default:
                if (a.writeTime.dwHighDateTime != b.writeTime.dwHighDateTime)
                    return a.writeTime.dwHighDateTime > b.writeTime.dwHighDateTime;
                return a.writeTime.dwLowDateTime > b.writeTime.dwLowDateTime;
        }
    });

    std::vector<std::wstring> result;
    result.reserve(entries.size());
    for (auto& e : entries) result.push_back(std::move(e.path));
    return result;
}

void CALLBACK OnFfmpegExit(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
    if (TimerOrWaitFired) return;
    HANDLE proc = (HANDLE)lpParam;
    DWORD code = 0;
    GetExitCodeProcess(proc, &code);
    if (g_videoHwnd) PostMessage(g_videoHwnd, WM_APP_FFMPEG_EXIT, code, 0);
}

void StopFfmpeg()
{
    if (g_ffmpegWaitReg) { UnregisterWait(g_ffmpegWaitReg); g_ffmpegWaitReg = NULL; }
    if (g_pipeThread) {
        if (g_pipeRead) { CloseHandle(g_pipeRead); g_pipeRead = NULL; }
        DWORD waitRet = WaitForSingleObject(g_pipeThread, 500);
        if (waitRet != WAIT_OBJECT_0) {
            Wh_Log(L"StopFfmpeg: pipe thread still alive after 500ms, terminating");
            TerminateThread(g_pipeThread, 0);
        }
        CloseHandle(g_pipeThread);
        g_pipeThread = NULL;
    }
    g_frameReady = false;
    if (g_pipeWrite) { CloseHandle(g_pipeWrite); g_pipeWrite = NULL; }
    if (g_pipeRead)  { CloseHandle(g_pipeRead);  g_pipeRead = NULL; }
    if (g_ffmpegProc){ TerminateProcess(g_ffmpegProc, 0); CloseHandle(g_ffmpegProc); g_ffmpegProc = NULL; }
}

bool IsDark()
{
    DWORD val = 1, cb = sizeof(val); HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExW(hKey, L"AppsUseLightTheme", NULL, NULL, (LPBYTE)&val, &cb);
        RegCloseKey(hKey);
    }
    return val == 0;
}

bool IsForegroundFull()
{
    HWND fg = GetForegroundWindow();
    if (!fg) return false;
    if (fg == GetDesktopWindow() || fg == GetShellWindow()) return false;
    DWORD style = (DWORD)GetWindowLongW(fg, GWL_STYLE);
    if (!(style & WS_MAXIMIZE)) {
        RECT rc; GetWindowRect(fg, &rc);
        int w = rc.right - rc.left, h = rc.bottom - rc.top;
        if (w < GetSystemMetrics(SM_CXSCREEN) - 200 || h < GetSystemMetrics(SM_CYSCREEN) - 200) return false;
    }
    return true;
}

bool PathExists(const WCHAR* p) { return GetFileAttributesW(p) != INVALID_FILE_ATTRIBUTES; }

LRESULT CALLBACK VideoWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_TIMER:
            if (wParam == TIMER_ID_MANAGER) { ManagerTick(); return 0; }
            if (wParam == TIMER_ID_RENDERER) { if (g_opacity > 0) RenderFrame(); return 0; }
            break;
        case WM_WINDOWPOSCHANGED: {
            const WINDOWPOS* wp = (const WINDOWPOS*)lParam;
            if (!(wp->flags & SWP_NOSIZE) && g_d3dDevice) {
                if (ResizeSwapChain((UINT)wp->cx, (UINT)wp->cy)) {
                    if (g_opacity > 0) RenderFrame();
                }
            }
            break;
        }
        case WM_APP_CLEANUP:
            DestroyWindow(hwnd);
            return 0;
        case WM_APP_FFMPEG_EXIT: {
            DWORD code = (DWORD)wParam;
            if (code == 0) {
                g_consecutiveErrors = 0;
                Wh_Log(L"WM_APP_FFMPEG_EXIT: normal, moving to next");
            } else {
                g_consecutiveErrors++;
                Wh_Log(L"WM_APP_FFMPEG_EXIT: error=%lu consecutive=%d", code, g_consecutiveErrors);
            }
            if (g_consecutiveErrors >= MAX_CONSECUTIVE_ERRORS) {
                Wh_Log(L"Too many consecutive errors (%d), stopping auto-play", g_consecutiveErrors);
                StopFfmpeg();
            } else {
                PlayNext();
            }
            return 0;
        }
        case WM_DESTROY:
            Wh_Log(L"VideoWndProc WM_DESTROY");
            StopManagerTimer();
            StopRendererTimer();
            g_running = false;
            ReleaseSwapChainResources();
            g_videoHwnd = NULL;
            break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool CreateVideoWindow()
{
    if (g_videoHwnd && IsWindow(g_videoHwnd)) return true;

    if (g_opacity <= 0) {
        Wh_Log(L"CreateVideoWindow: opacity=0, skipping window creation");
        return false;
    }

    if (!EnsureDxInitialized()) { Wh_Log(L"DX init failed"); return false; }

    g_workerW = GetWorkerW();
    if (!g_workerW) { Wh_Log(L"CreateVideoWindow: GetWorkerW failed"); return false; }

    HWND hProgman = FindWindowW(L"Progman", nullptr);

RECT rc;
    GetWindowRect(g_workerW, &rc);
    int sw = rc.right - rc.left;
    int sh = rc.bottom - rc.top;

    if (!g_classRegistered) {
        WNDCLASSEXW wc = {sizeof(wc)};
        wc.lpfnWndProc = VideoWndProc;
        wc.hInstance = GetCurrentModuleHandle();
        wc.lpszClassName = L"VidWallpaperWnd";
        wc.hbrBackground = NULL;
        ATOM classAtom = RegisterClassExW(&wc);
        if (!classAtom) {
            DWORD err = GetLastError();
            if (err == ERROR_CLASS_ALREADY_EXISTS) {
                UnregisterClassW(L"VidWallpaperWnd", GetCurrentModuleHandle());
                classAtom = RegisterClassExW(&wc);
            }
        }
        if (!classAtom) { Wh_Log(L"RegisterClassExW failed"); return false; }
        g_classRegistered = true;
    }

    g_videoHwnd = CreateWindowExW(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_NOACTIVATE,
        L"VidWallpaperWnd", L"",
        WS_CHILD | WS_VISIBLE,
        0, 0, sw, sh,
        g_workerW, NULL, GetCurrentModuleHandle(), NULL);

    if (!g_videoHwnd) { Wh_Log(L"CreateWindowExW failed err=%lu", GetLastError()); return false; }

    if (!CreateSwapChainResources(sw, sh)) {
        Wh_Log(L"CreateVideoWindow: CreateSwapChainResources failed");
        DestroyWindow(g_videoHwnd);
        g_videoHwnd = NULL;
        return false;
    }

    g_frameSize = sw * sh * 4;
    if (g_frameBuf.size() != static_cast<size_t>(g_frameSize)) {
        g_frameBuf.resize(g_frameSize);
    }

    return true;
}

DWORD WINAPI PipeReaderThread(LPVOID)
{
    int sw = 0, sh = 0;
    if (g_videoHwnd) {
        RECT rc;
        GetClientRect(g_videoHwnd, &rc);
        sw = rc.right - rc.left;
        sh = rc.bottom - rc.top;
    }
    if (sw <= 0) {
        sw = GetSystemMetrics(SM_CXSCREEN);
        sh = GetSystemMetrics(SM_CYSCREEN);
    }
    g_frameSize = sw * sh * 4;
    if (g_frameBuf.size() != static_cast<size_t>(g_frameSize)) {
        g_frameBuf.resize(g_frameSize);
    }

    int frameCount = 0;
    while (g_running && g_pipeRead)
    {
        if (!g_frameSize) { Sleep(10); continue; }

        if (g_isPaused) { Sleep(1); continue; }

        while (true) {
            DWORD avail = 0;
            if (PeekNamedPipe(g_pipeRead, NULL, 0, NULL, &avail, NULL) && avail >= (DWORD)g_frameSize * 3) {
                DWORD skip = 0;
                while (skip < (DWORD)g_frameSize && g_running) {
                    DWORD got = 0;
                    if (!ReadFile(g_pipeRead, g_frameBuf.data() + skip, (DWORD)g_frameSize - skip, &got, NULL) || got == 0) break;
                    skip += got;
                }
                continue;
            }
            break;
        }

        DWORD totalRead = 0;
        while (totalRead < (DWORD)g_frameSize && g_running)
        {
            DWORD got = 0;
            BOOL okR = ReadFile(g_pipeRead, g_frameBuf.data() + totalRead, g_frameSize - totalRead, &got, NULL);
            if (!okR || got == 0) break;
            totalRead += got;
        }
        if (totalRead != (DWORD)g_frameSize) break;

        if (g_isPaused) continue;

        g_frameReady = true;
        frameCount++;
    }

    g_frameReady = false;
    return 0;
}

void RenderFrame()
{
    if (!g_frameReady) return;
    if (g_opacity <= 0) { g_frameReady = false; return; }
    if (g_isPaused) return;
    if (!g_running) return;
    if (!g_videoHwnd || !IsWindow(g_videoHwnd)) return;
    if (!g_dc || !g_frameBitmap || !g_swapChain) return;
    if (!g_frameSize) return;

    int sw = 0, sh = 0;
    RECT rc;
    if (GetClientRect(g_videoHwnd, &rc)) {
        sw = rc.right - rc.left;
        sh = rc.bottom - rc.top;
    }
    if (sw <= 0 || sh <= 0) return;

    HRESULT hr;
    hr = g_frameBitmap->CopyFromMemory(nullptr, g_frameBuf.data(), sw * 4);
    if (FAILED(hr)) {
        Wh_Log(L"RenderFrame: CopyFromMemory failed 0x%08X", hr);
        return;
    }

    g_dc->BeginDraw();
    g_dc->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
    if (g_opacity >= 255) {
        g_dc->DrawBitmap(
            g_frameBitmap.Get(),
            D2D1::RectF(0, 0, (FLOAT)sw, (FLOAT)sh),
            1.0f,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            D2D1::RectF(0, 0, (FLOAT)sw, (FLOAT)sh));
    } else {
        g_dc->DrawBitmap(
            g_frameBitmap.Get(),
            D2D1::RectF(0, 0, (FLOAT)sw, (FLOAT)sh),
            (FLOAT)g_opacity / 255.0f,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            D2D1::RectF(0, 0, (FLOAT)sw, (FLOAT)sh));
    }
    hr = g_dc->EndDraw();
    if (FAILED(hr)) {
        Wh_Log(L"RenderFrame: EndDraw failed 0x%08X", hr);
        return;
    }

    if (!g_running) return;
    hr = g_swapChain->Present(0, 0);
    if (FAILED(hr)) {
        Wh_Log(L"RenderFrame: Present failed 0x%08X", hr);
        return;
    }

    g_frameReady = false;
}

bool StartFfmpeg(const WCHAR* videoPath)
{
    StopFfmpeg();
    if (!g_ffmpegPath[0]) { Wh_Log(L"StartFfmpeg: ffmpegPath is empty! Set it in mod settings."); return false; }
    if (!videoPath || !*videoPath || !PathExists(videoPath)) return false;

    int sw = 0, sh = 0;
    if (g_videoHwnd) {
        RECT rc;
        GetClientRect(g_videoHwnd, &rc);
        sw = rc.right - rc.left;
        sh = rc.bottom - rc.top;
    }
    if (sw <= 0) {
        sw = GetSystemMetrics(SM_CXSCREEN);
        sh = GetSystemMetrics(SM_CYSCREEN);
    }

    SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};
    HANDLE hRead = NULL, hWrite = NULL;
    if (!CreatePipe(&hRead, &hWrite, &sa, 0)) { Wh_Log(L"CreatePipe failed"); StopFfmpeg(); return false; }

    HANDLE hErrRead = NULL, hErrWrite = NULL;
    if (!CreatePipe(&hErrRead, &hErrWrite, &sa, 0)) { Wh_Log(L"CreatePipe err failed"); StopFfmpeg(); return false; }

    SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(hErrRead, HANDLE_FLAG_INHERIT, 0);
    g_pipeRead = hRead; g_pipeWrite = hWrite;

    WCHAR cmd[MAX_PATH * 6];
    const WCHAR* vfArg = nullptr;

    WCHAR padColor[64] = {0};
    bool isPadTransparent = false;
    if (wcscmp(g_padColorMode, L"white") == 0) {
        wcscpy_s(padColor, L"color=white");
    } else if (wcscmp(g_padColorMode, L"black") == 0) {
        wcscpy_s(padColor, L"color=black");
    } else if (wcscmp(g_padColorMode, L"transparent") == 0) {
        wcscpy_s(padColor, L"color=0x00000000");
        isPadTransparent = true;
    } else if (wcscmp(g_padColorMode, L"dynamic") == 0) {
        wcscpy_s(padColor, g_lastIsDark ? L"color=black" : L"color=white");
    } else if (wcscmp(g_padColorMode, L"custom") == 0 && g_padColorCustom[0]) {
        const WCHAR* src = g_padColorCustom[0] == L'#' ? g_padColorCustom + 1 : g_padColorCustom;
        size_t len = wcslen(src);
        bool valid = (len == 6 || len == 8);
        if (valid) {
            for (size_t i = 0; i < len; i++) {
                WCHAR c = src[i];
                if (!((c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f') || (c >= L'A' && c <= L'F'))) { valid = false; break; }
            }
        }
        WCHAR hex[16] = {0};
        if (!valid) {
            Wh_Log(L"padColorCustom: invalid value '%s', fallback to black", g_padColorCustom);
            wcscpy_s(hex, L"0xFF000000");
        } else if (len == 6) {
            swprintf_s(hex, L"0xFF%s", src);
        } else {
            swprintf_s(hex, L"0x%s", src);
            if (src[0] == L'0' && src[1] == L'0') isPadTransparent = true;
        }
        swprintf_s(padColor, L"color=%s", hex);
    } else {
        wcscpy_s(padColor, L"color=black");
    }

    switch (g_scalingMode) {
        case 0: vfArg = L" -vf \"scale=%d:%d:force_original_aspect_ratio=increase,crop=%d:%d\""; break;
        case 1: vfArg = L" -vf \"scale=%d:%d:force_original_aspect_ratio=decrease,pad=%d:%d:(ow-iw)/2:(oh-ih)/2:%s\""; break;
        case 3: vfArg = nullptr; break;
        default: vfArg = nullptr; break;
    }
    const WCHAR* hwaccelArg = (g_hwaccelMode == 1) ? L" -hwaccel d3d11va" : L"";
    const WCHAR* pixFmt = L"bgra";

    bool hasVf = (vfArg != nullptr) || g_scalingMode == 2;

    if (hasVf) {
        WCHAR vfBuf[512];
        if (g_scalingMode == 2) {
            const WCHAR* colorVal = padColor;
            if (wcsncmp(padColor, L"color=", 6) == 0) colorVal = padColor + 6;
            swprintf_s(vfBuf,
                L" -vf \"color=c=%s:s=%dx%d:r=%d,format=%s[bg];[0:v]format=%s[v];[bg][v]overlay=(W-w)/2:(H-h)/2:shortest=1,format=%s\"",
                colorVal, sw, sh, g_fps, pixFmt, pixFmt, pixFmt);
        } else if (g_scalingMode == 0) {
            swprintf_s(vfBuf, vfArg, sw, sh, sw, sh);
        } else {
            swprintf_s(vfBuf, vfArg, sw, sh, sw, sh, padColor);
        }
        swprintf_s(cmd, L"\"%s\" -nostdin -hide_banner -loglevel error%s -i \"%s\" -an -f rawvideo -pix_fmt %s%s -r %d -",
            g_ffmpegPath, hwaccelArg, videoPath, pixFmt, vfBuf, g_fps);
    } else {
        swprintf_s(cmd, L"\"%s\" -nostdin -hide_banner -loglevel error%s -i \"%s\" -an -f rawvideo -pix_fmt %s -s %dx%d -r %d -",
            g_ffmpegPath, hwaccelArg, videoPath, pixFmt, sw, sh, g_fps);
    }

    STARTUPINFOW si = {sizeof(si)};
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hWrite;
    si.hStdError = hErrWrite;

    PROCESS_INFORMATION pi = {0};
    if (!CreateProcessW(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        Wh_Log(L"CreateProcess failed err=%lu", GetLastError());
        StopFfmpeg();
        return false;
    }
    CloseHandle(pi.hThread);
    CloseHandle(hWrite);
    CloseHandle(hErrWrite);
    g_pipeWrite = NULL;
    g_ffmpegProc = pi.hProcess;

    {
        char errBuf[2048] = {0};
        DWORD waited = 0;
        bool gotData = false;
        while (waited < 5000 && g_running) {
            DWORD avail = 0;
            if (PeekNamedPipe(hRead, NULL, 0, NULL, &avail, NULL) && avail > 0) {
                gotData = true;
                break;
            }
            Sleep(50); waited += 50;
        }
        DWORD errAvail = 0;
        BOOL errPeekOk = PeekNamedPipe(hErrRead, NULL, 0, NULL, &errAvail, NULL);
        if (errPeekOk && errAvail > 0) {
            DWORD got = 0;
            if (ReadFile(hErrRead, errBuf, sizeof(errBuf) - 1, &got, NULL) && got > 0) {
                errBuf[got] = 0;
                Wh_Log(L"ffmpeg stderr: %S", errBuf);
            }
        }
        CloseHandle(hErrRead);
        if (!gotData) {
            Wh_Log(L"StartFfmpeg: no data from ffmpeg after %dms, aborting", waited);
            StopFfmpeg(); return false;
        }
    }

    if (!g_videoHwnd || !IsWindow(g_videoHwnd)) {
        Wh_Log(L"StartFfmpeg: video window is not ready");
        StopFfmpeg();
        return false;
    }

    g_pipeThread = CreateThread(NULL, 0, PipeReaderThread, NULL, 0, NULL);
    RegisterWaitForSingleObject(&g_ffmpegWaitReg, g_ffmpegProc, OnFfmpegExit, g_ffmpegProc, INFINITE, WT_EXECUTEONLYONCE);
    StartRendererTimer();
    return true;
}

void ReloadVideoList()
{
    if (g_dirChangeHandle != INVALID_HANDLE_VALUE) { FindCloseChangeNotification(g_dirChangeHandle); g_dirChangeHandle = INVALID_HANDLE_VALUE; }

    g_videoList.clear(); g_videoIndex = 0; g_consecutiveErrors = 0;
    bool dark = IsDark();
    g_lastIsDark = dark;

    WCHAR target[MAX_PATH] = {0};
    if (dark) { if (g_darkPath[0]) wcscpy_s(target, g_darkPath); else wcscpy_s(target, g_lightPath); }
    else wcscpy_s(target, g_lightPath);

    DWORD attr = GetFileAttributesW(target);
    if (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY)) {
        g_videoList = EnumVideos(target);
        g_dirChangeHandle = FindFirstChangeNotificationW(target, FALSE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE);
    } else if (PathExists(target)) {
        g_videoList.emplace_back(target);
    }
}

void ReloadWallpaper()
{
    StopFfmpeg();
    if (g_opacity <= 0) {
        Wh_Log(L"ReloadWallpaper: opacity=0, skipping ffmpeg start");
        return;
    }
    if (!g_videoHwnd || !IsWindow(g_videoHwnd)) {
        if (!CreateVideoWindow()) {
            Wh_Log(L"ReloadWallpaper: CreateVideoWindow failed");
            return;
        }
    }
    ReloadVideoList();
    if (!g_videoList.empty()) PlayNext();
}

bool PlayNext()
{
    if (g_opacity <= 0) return false;

    if (g_pendingReload) {
        g_pendingReload = false;
        ReloadVideoList();
    }
    if (!g_videoHwnd || !IsWindow(g_videoHwnd)) {
        if (!CreateVideoWindow()) {
            Wh_Log(L"PlayNext: CreateVideoWindow failed");
            return false;
        }
    }
    if (g_videoList.empty()) return false;
    if (g_consecutiveErrors >= MAX_CONSECUTIVE_ERRORS) {
        Wh_Log(L"PlayNext: too many consecutive errors (%d), giving up. Reload to retry.", g_consecutiveErrors);
        return false;
    }
    const WCHAR* next = g_videoList[g_videoIndex++ % g_videoList.size()].c_str();
    bool ok = StartFfmpeg(next);
    if (!ok) {
        g_consecutiveErrors++;
    } else {
        StartManagerTimer();
    }
    return ok;
}

void Wh_ModSettingsChanged()
{
    PCWSTR s = Wh_GetStringSetting(L"ffmpegPath");
    WCHAR newFfmpegPath[MAX_PATH] = {0};
    if (s) { wcscpy_s(newFfmpegPath, MAX_PATH, s); Wh_FreeStringSetting(s); }

    s = Wh_GetStringSetting(L"lightVideoPath");
    WCHAR newLightPath[MAX_PATH] = {0};
    if (s) { wcscpy_s(newLightPath, MAX_PATH, s); Wh_FreeStringSetting(s); }

    s = Wh_GetStringSetting(L"darkVideoPath");
    WCHAR newDarkPath[MAX_PATH] = {0};
    if (s) { wcscpy_s(newDarkPath, MAX_PATH, s); Wh_FreeStringSetting(s); }

    s = Wh_GetStringSetting(L"sortMode");
    WCHAR sortStr[8] = {0};
    if (s) { wcscpy_s(sortStr, 8, s); Wh_FreeStringSetting(s); }
    int newSortMode = _wtoi(sortStr);
    if (newSortMode < 0 || newSortMode > 7) newSortMode = 0;

    int oldOpacity = g_opacity;
    int pct = Wh_GetIntSetting(L"opacity");
    int newOpacity = (pct >= 0 && pct <= 100) ? (pct * 255 / 100) : 255;
    bool oldWasZero = (oldOpacity <= 0);
    bool newIsZero = (newOpacity <= 0);

    int fps = Wh_GetIntSetting(L"fps");
    if (fps < 10) fps = 10;
    if (fps > 60) fps = 60;
    int oldFps = g_fps;
    g_fps = fps;

    s = Wh_GetStringSetting(L"scalingMode");
    WCHAR scalingStr[8] = {0};
    if (s) { wcscpy_s(scalingStr, 8, s); Wh_FreeStringSetting(s); }
    int scaling = _wtoi(scalingStr);
    if (scaling < 0 || scaling > 3) scaling = 0;
    int oldScaling = g_scalingMode;
    g_scalingMode = scaling;

    s = Wh_GetStringSetting(L"padColorMode");
    WCHAR newPadMode[32] = {0};
    if (s) { wcscpy_s(newPadMode, 32, s); Wh_FreeStringSetting(s); }
    if (!newPadMode[0]) wcscpy_s(newPadMode, 32, L"black");

    s = Wh_GetStringSetting(L"padColorCustom");
    WCHAR newPadCustom[32] = {0};
    if (s) { wcscpy_s(newPadCustom, 32, s); Wh_FreeStringSetting(s); }

    s = Wh_GetStringSetting(L"applyMode");
    WCHAR newApplyMode[16] = {0};
    if (s) { wcscpy_s(newApplyMode, 16, s); Wh_FreeStringSetting(s); }
    if (!newApplyMode[0]) wcscpy_s(newApplyMode, 16, L"instant");

    s = Wh_GetStringSetting(L"hwaccelMode");
    WCHAR hwaccelStr[4] = {0};
    if (s) { wcscpy_s(hwaccelStr, 4, s); Wh_FreeStringSetting(s); }
    int newHwaccel = _wtoi(hwaccelStr);
    if (newHwaccel < 0 || newHwaccel > 1) newHwaccel = 0;

    g_opacity = newOpacity;

    if (!oldWasZero && newIsZero) {
        Wh_Log(L"Opacity changed >0→0, stopping ffmpeg");
        StopFfmpeg();
        StopManagerTimer();
        StopRendererTimer();
        return;
    }

    bool needReload =
        wcscmp(newFfmpegPath, g_ffmpegPath) != 0 ||
        wcscmp(newLightPath, g_lightPath) != 0 ||
        wcscmp(newDarkPath, g_darkPath) != 0 ||
        newSortMode != g_sortMode ||
        newHwaccel != g_hwaccelMode ||
        g_fps != oldFps ||
        g_scalingMode != oldScaling ||
        wcscmp(newPadMode, g_padColorMode) != 0 ||
        wcscmp(newPadCustom, g_padColorCustom) != 0;

    wcscpy_s(g_ffmpegPath, MAX_PATH, newFfmpegPath);
    wcscpy_s(g_lightPath, MAX_PATH, newLightPath);
    wcscpy_s(g_darkPath, MAX_PATH, newDarkPath);
    g_sortMode = newSortMode;
    g_hwaccelMode = newHwaccel;
    wcscpy_s(g_padColorMode, 32, newPadMode);
    wcscpy_s(g_padColorCustom, 32, newPadCustom);
    wcscpy_s(g_applyMode, 16, newApplyMode);

    if (oldWasZero && !newIsZero) {
        Wh_Log(L"Opacity changed 0→%d%, creating window and starting", pct);
        if (!g_videoHwnd || !IsWindow(g_videoHwnd)) {
            if (!CreateVideoWindow()) {
                Wh_Log(L"Wh_ModSettingsChanged: CreateVideoWindow failed");
                return;
            }
        }
        ReloadWallpaper();
        return;
    }

    if (needReload) {
        if (wcscmp(newApplyMode, L"on_next") == 0 && g_ffmpegProc != NULL) {
            g_pendingReload = true;
        } else {
            ReloadWallpaper();
        }
    }
}

void StartManagerTimer() {
    if (!g_videoHwnd) return;
    SetTimer(g_videoHwnd, TIMER_ID_MANAGER, 200, nullptr);
}

void StopManagerTimer() {
    if (g_videoHwnd) KillTimer(g_videoHwnd, TIMER_ID_MANAGER);
}

void StartRendererTimer() {
    if (!g_videoHwnd) return;
    DWORD interval = g_fps > 0 ? 1000 / (DWORD)g_fps : 66;
    SetTimer(g_videoHwnd, TIMER_ID_RENDERER, interval, nullptr);
}

void StopRendererTimer() {
    if (g_videoHwnd) KillTimer(g_videoHwnd, TIMER_ID_RENDERER);
}

void ManagerTick() {
    if (!g_hThemeKey) {
        RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_NOTIFY, &g_hThemeKey);
        if (g_hThemeKey) g_hThemeEvt = CreateEventW(NULL, TRUE, FALSE, NULL);
        g_lastDarkChecked = IsDark();
    }

    if (g_hThemeKey && g_hThemeEvt) {
        ResetEvent(g_hThemeEvt);
        RegNotifyChangeKeyValue(g_hThemeKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, g_hThemeEvt, TRUE);
        DWORD waitRet = WaitForSingleObject(g_hThemeEvt, 0);
        if (waitRet == WAIT_OBJECT_0) {
            bool isDark = IsDark();
            if (isDark != g_lastDarkChecked) {
                g_lastDarkChecked = isDark;
                g_lastIsDark = isDark;
                bool isOnNext = (wcscmp(g_applyMode, L"on_next") == 0);
                if (isOnNext && g_ffmpegProc) { g_pendingReload = true; }
                else { ReloadWallpaper(); return; }
            }
        }
    }

    if (g_dirChangeHandle != INVALID_HANDLE_VALUE) {
        DWORD waitRet = WaitForSingleObject(g_dirChangeHandle, 0);
        if (waitRet == WAIT_OBJECT_0) {
            bool isOnNext = (wcscmp(g_applyMode, L"on_next") == 0);
            if (isOnNext && g_ffmpegProc) { g_pendingReload = true; }
            else { ReloadWallpaper(); return; }
            FindNextChangeNotification(g_dirChangeHandle);
        }
    }

    if (g_pauseOnFullscreen) {
        bool now = IsForegroundFull();
        if (now != g_prevFull) { g_isPaused = now; g_prevFull = now; }
    }
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Wh_ModInit: entering");
    g_mutex = CreateMutexW(NULL, TRUE, L"Local\\VidWallpaperOnlyOne");
    if (g_mutex && GetLastError() == ERROR_ALREADY_EXISTS) { CloseHandle(g_mutex); g_mutex = NULL; return FALSE; }
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original);
    Wh_Log(L"Wh_ModInit: OK");
    return TRUE;
}

void Wh_ModAfterInit()
{
    Wh_Log(L"Wh_ModAfterInit: entering");
    HWND hWorkerW = GetWorkerW();
    if (hWorkerW) {
        RunFromWindowThread(hWorkerW, [](void*) {
            if (!CreateVideoWindow()) {
                Wh_Log(L"CreateVideoWindow failed in AfterInit");
                return;
            }
            Wh_ModSettingsChanged();
        }, nullptr);
    } else {
        Wh_Log(L"Wh_ModAfterInit: WorkerW not found, waiting for FolderView hook");
    }
}

void Wh_ModUninit()
{
    Wh_Log(L"Wh_ModUninit: entering");
    g_running = false;
    StopFfmpeg();
    if (g_dirChangeHandle != INVALID_HANDLE_VALUE) { FindCloseChangeNotification(g_dirChangeHandle); g_dirChangeHandle = INVALID_HANDLE_VALUE; }

    if (HWND hProgman = GetProgmanWnd()) {
        RunFromWindowThread(hProgman, [](void*) {
            if (g_createVideoTimer) { KillTimer(nullptr, g_createVideoTimer); g_createVideoTimer = 0; }
            if (g_videoHwnd) { SendMessage(g_videoHwnd, WM_APP_CLEANUP, 0, 0); }
        }, nullptr);
    }

    ReleaseSwapChainResources();
    if (g_videoHwnd) { DestroyWindow(g_videoHwnd); g_videoHwnd = NULL; }
    if (g_classRegistered) {
        UnregisterClassW(L"VidWallpaperWnd", GetCurrentModuleHandle());
        g_classRegistered = false;
    }
    UninitDirectX();
    g_frameBuf.clear();
    g_frameSize = 0;
    g_workerW = NULL;
    if (g_hThemeKey) { RegCloseKey(g_hThemeKey); g_hThemeKey = NULL; }
    if (g_hThemeEvt) { CloseHandle(g_hThemeEvt); g_hThemeEvt = NULL; }
    if (g_mutex) { CloseHandle(g_mutex); g_mutex = NULL; }
    Wh_Log(L"Wh_ModUninit: done");
}