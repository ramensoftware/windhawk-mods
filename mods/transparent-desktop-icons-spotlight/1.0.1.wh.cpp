// ==WindhawkMod==
// @id              transparent-desktop-icons-spotlight
// @name            Transparent Desktop Icons with Spotlight
// @description     Make desktop icons transparent when idle with an interactive spotlight effect. Hover over the desktop or select icons to reveal them with full clarity and customizable transitions.
// @version         1.0.1
// @author          drgutman
// @github          https://github.com/drgutman
// @homepage        https://instagram.com/drgutman
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -ldxgi -ld2d1 -ld3d11 -ldcomp -lgdi32 -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Transparent Desktop Icons with Spotlight

Makes desktop icons semi-transparent when idle, revealing the wallpaper beneath.
When you move your mouse over the desktop, a customizable spotlight effect
illuminates the area around your cursor, revealing icons with full clarity.

As the cursor leaves the desktop or remains idle, the spotlight fades and icons
gradually become transparent again. Selected icons remain opaque with their own 
customizable independent timeouts, blurs, and fade durations.


**Demonstration**: in this case the selected icons have a timeout (set it to 0 to keep them active)

![Demonstration:](https://i.imgur.com/7cTTHTN.gif)


## Features

* Dedicated render thread for flawless, vsync-matched 60/144+ FPS performance.
* 0% background GPU/CPU usage when the desktop is idle.
* Independent GPU Blur processing for both the cursor spotlight and selected icon areas.
* Customizable shapes (square to circle) for both the spotlight and selected icon areas.
* Click-through architecture using WS_EX_TRANSPARENT ensures no interference with native OS interaction.

**Limitations and known bugs:** 

  * This mod captures the static Windows wallpaper for performance reasons. 
    It does not currently support live/animated wallpapers (e.g., Wallpaper Engine, Lively).
  * When changing themes, heavier ones (with very large wallpapers) tend to create a ghost of the last wallpaper.
    The blink that happens after 4 seconds after a theme/wallpaper change (or the delay until new settings are applied) is to accommodate "Captured Motion" theme. 
  * Multi-monitor systems might not display correctly, as I am currently using only one monitor
    Will try to implement full multi-monitor support in the future


*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- idleOpacity: 170
  $name: Idle Opacity
  $description: How much the icons are dimmed when idle (0 = icons fully visible, 255 = icons fully hidden)
- spotlightRadius: 320
  $name: Mouse Spotlight Radius
  $description: Size of the mouse spotlight in pixels
- spotlightRoundness: 60
  $name: Mouse Spotlight Roundness (%)
  $description: Shape of the spotlight (0 = square, 100 = circular)
- spotlightBlur: 30
  $name: Mouse Spotlight Edge Blur
  $description: Blur radius for soft spotlight edges in pixels (0 = hard edge)
- idleDelay: 2000
  $name: Mouse Spotlight Idle Timeout (ms)
  $description: Time in milliseconds before the spotlight begins to fade out
- spotlightFadeInDuration: 100
  $name: Mouse Spotlight Fade In Duration (ms)
  $description: How long it takes for the spotlight to fade in when mouse enters the desktop area
- spotlightFadeOutDuration: 2000
  $name: Mouse Spotlight Fade Out Duration (ms)
  $description: How long it takes for the spotlight to fade out when mouse leaves the desktop area
- mouseSmoothing: 0
  $name: Mouse Smoothing
  $description: Smoothing factor for spotlight following (0-20, 0 = no smoothing, 20 = very smooth)
- selectionOpacity: 255
  $name: Icon Selection Opacity
  $description: How visible icons become when selected (0 = transparent, 255 = fully visible)
- selectionPadding: 30
  $name: Icon Selection Padding
  $description: Extra space around selected icons in pixels
- selectionRoundness: 20
  $name: Icon Selection Roundness (%)
  $description: How rounded the icon selection area is (0 = square, 100 = circular)
- selectionBlur: 20
  $name: Icon Selection Edge Blur
  $description: Blur radius for the icon selection reveal area
- selectionTimeout: 2000
  $name: Icon Selection Timeout (ms)
  $description: Time to keep the icon visible after deselection or when the mouse leaves the desktop area (0 = keep visible indefinitely while selected)
- selectionFadeInDuration: 250
  $name: Icon Selection Fade In Duration (ms)
  $description: How long the icon selection area takes to fade in when icon is selected
- selectionFadeOutDuration: 2000
  $name: Icon Selection Fade Out Duration (ms)
  $description: How long the icon selection area takes to fade out after deselection
*/
// ==/WindhawkModSettings==

#include <algorithm>
#include <commctrl.h>
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <d2d1helper.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dxgi1_3.h>
#include <shellscalingapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <wrl/client.h>
#include <atomic>
#include <mutex>
#include <thread>
#include <cmath>
#include <vector>

using namespace std::literals;
using Microsoft::WRL::ComPtr;

// Timer IDs
#define TIMER_ID_STATE_POLL 2
#define TIMER_ID_RECREATE_OVERLAY 4
#define TIMER_ID_WALLPAPER_UPDATE 5

#define WM_APP_CLEANUP (WM_APP + 1)
#define WM_APP_DEVICE_LOST (WM_APP + 2)
#define WM_APP_INIT (WM_APP + 3)

#define OVERLAY_WINDOW_CLASS (L"TransparentDesktopSpotlight_" WH_MOD_ID)
#define MESSAGE_WINDOW_CLASS L"TransparentDesktopSpotlight_Message_" WH_MOD_ID

// Direct2D Gaussian Blur CLSID
static const IID kCLSID_D2D1GaussianBlur = {
    0x1feb6d69, 0x2fe6, 0x4ac9,
    {0x8c, 0x58, 0x1d, 0x7f, 0x93, 0xe7, 0xa6, 0xa5}};

////////////////////////////////////////////////////////////////////////////////
// Settings

struct Settings {
    int idleOpacity;
    int spotlightRadius;
    int spotlightRoundness;
    int spotlightBlur;
    int idleDelay;
    int spotlightFadeInDuration;
    int spotlightFadeOutDuration;
    int mouseSmoothing;
    
    int selectionOpacity;
    int selectionPadding;
    int selectionRoundness;
    int selectionBlur;
    int selectionTimeout;
    int selectionFadeInDuration;
    int selectionFadeOutDuration;
} g_settings;

////////////////////////////////////////////////////////////////////////////////
// Global state

// Window handles
HWND g_overlayWnd = nullptr;
HWND g_messageWnd = nullptr;
std::atomic<bool> g_unloading{false};
bool g_isRecreating = false;

// Background Render Thread
std::thread* g_renderThread = nullptr;
std::atomic<bool> g_threadStop{false};
std::mutex g_renderMutex;
std::atomic<bool> g_forceRender{true};
HANDLE g_renderEvent = nullptr;

// DirectX pipeline resources
ComPtr<ID3D11Device> g_d3dDevice;
ComPtr<IDXGIDevice> g_dxgiDevice;
ComPtr<IDXGIFactory2> g_dxgiFactory;
ComPtr<ID2D1Factory1> g_d2dFactory;
ComPtr<ID2D1Device> g_d2dDevice;
ComPtr<IDXGISwapChain1> g_swapChain;
ComPtr<ID2D1DeviceContext> g_dc;
ComPtr<ID2D1Bitmap1> g_targetBitmap;

ComPtr<ID2D1Bitmap1> g_spotlightMaskBitmap; 
ComPtr<ID2D1Bitmap1> g_selectionMaskBitmap; 

ComPtr<IDCompositionDevice> g_compositionDevice;
ComPtr<IDCompositionTarget> g_compositionTarget;
ComPtr<IDCompositionVisual> g_compositionVisual;

// Textures, Brushes, and Filters
ComPtr<ID2D1Bitmap> g_wallpaperBitmap;
ComPtr<ID2D1Effect> g_spotlightBlurEffect;
ComPtr<ID2D1Effect> g_selectionBlurEffect;
ComPtr<ID2D1SolidColorBrush> g_spotlightBrush;
ComPtr<ID2D1SolidColorBrush> g_selectionBrush;

// Animations and states (Render Thread)
D2D1_POINT_2F g_smoothedMousePos = {0.0f, 0.0f};
bool g_isIdle = true;
ULONGLONG g_lastMouseTime = 0;
float g_spotlightFade = 0.0f; 
bool g_isMouseMoving = false;

ULONGLONG g_lastSelectionTime = 0;
float g_selectionFade = 0.0f;
std::vector<RECT> g_cachedSelectedRects;

// UI Thread State polling
std::mutex g_stateMutex;
HWND g_cachedHwndLV = nullptr;
HWND g_cachedHwndShellView = nullptr;
HWND g_cachedHwndDesktopHost = nullptr;
std::vector<RECT> g_selectedRects;
bool g_isEditing = false;

// DPI scaling factor
float g_dpiScale = 1.0f;

// Initialization synchronization
std::atomic<bool> g_initialized_desktop{false};
std::atomic<bool> g_initialized{false};
WCHAR g_lastKnownWallpaperPath[MAX_PATH] = {};

// Cold-start retry tracking. g_initAttempts counts consecutive failed WM_APP_INIT
// handling; g_lastInitFailure records the GetTickCount64 of the most recent failure.
// On success both reset. Used to switch to a longer (5s) backoff after a few quick
// attempts so we don't peg a timer forever on a permanently broken environment.
std::atomic<int> g_initAttempts{0};
std::atomic<ULONGLONG> g_lastInitFailure{0};

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
void CreateOverlayWindow();
void CreateMessageWindow();
void LoadSettings();
void CaptureWallpaperBitmap();
HWND GetDesktopListView();

////////////////////////////////////////////////////////////////////////////////
// Utility functions

HWND GetDesktopParent() {
    HWND hLV = GetDesktopListView();
    if (hLV) {
        HWND hShellView = GetParent(hLV); 
        if (hShellView) {
            return GetParent(hShellView); 
        }
    }
    return nullptr;
}

float GetMonitorDpiScale(HMONITOR monitor) {
    UINT dpiX = 96, dpiY = 96;
    if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        return dpiX / 96.0f;
    }
    return 1.0f;
}

HMONITOR GetWorkerWMonitor() {
    HWND hParent = GetDesktopParent();
    if (hParent) {
        return MonitorFromWindow(hParent, MONITOR_DEFAULTTONEAREST);
    }
    return MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);
}

HWND GetDesktopListView() {
    HWND hShellView = nullptr;
    HWND hProgman = FindWindow(L"Progman", nullptr);
    if (hProgman) {
        hShellView = FindWindowEx(hProgman, nullptr, L"SHELLDLL_DefView", nullptr);
    }

    if (!hShellView) {
        HWND hWorkerW = nullptr;
        while ((hWorkerW = FindWindowEx(nullptr, hWorkerW, L"WorkerW", nullptr)) != nullptr) {
            hShellView = FindWindowEx(hWorkerW, nullptr, L"SHELLDLL_DefView", nullptr);
            if (hShellView) break;
        }
    }

    if (hShellView) {
        return FindWindowEx(hShellView, nullptr, L"SysListView32", nullptr);
    }
    return nullptr;
}

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)GetCurrentModuleHandle, &module)) return nullptr;
    return module;
}

////////////////////////////////////////////////////////////////////////////////
// UI Thread Polling

void PollDesktopState() {
    static HWND s_hwndLV = nullptr;
    static HWND s_hwndShellView = nullptr;
    static HWND s_hwndDesktopHost = nullptr;

    if (!s_hwndLV || !IsWindow(s_hwndLV)) {
        s_hwndLV = GetDesktopListView();
        if (s_hwndLV) {
            s_hwndShellView = GetParent(s_hwndLV);
            s_hwndDesktopHost = s_hwndShellView ? GetParent(s_hwndShellView) : nullptr;
        } else {
            return;
        }
    }

    // Check for inline renaming (F2 Edit Control)
    bool isEditing = false;
    if (s_hwndLV) {
        HWND hEdit = FindWindowEx(s_hwndLV, nullptr, L"Edit", nullptr);
        if (!hEdit) hEdit = FindWindowEx(s_hwndLV, nullptr, L"WC_EDIT", nullptr);
        if (hEdit && IsWindowVisible(hEdit)) {
            isEditing = true;
        }
    }

    static std::vector<RECT> s_selected;
    s_selected.clear();

    if (s_hwndLV) {
        int selectedCount = (int)SendMessage(s_hwndLV, LVM_GETSELECTEDCOUNT, 0, 0);
        if (selectedCount > 0) {
            int idx = -1;
            while ((idx = (int)SendMessage(s_hwndLV, LVM_GETNEXTITEM, idx, LVNI_SELECTED)) != -1) {
                RECT rcBounds = {};
                rcBounds.left = LVIR_BOUNDS;
                if (SendMessage(s_hwndLV, LVM_GETITEMRECT, idx, (LPARAM)&rcBounds)) {
                    POINT ptTL = { rcBounds.left, rcBounds.top };
                    POINT ptBR = { rcBounds.right, rcBounds.bottom };
                    MapWindowPoints(s_hwndLV, g_overlayWnd, &ptTL, 1);
                    MapWindowPoints(s_hwndLV, g_overlayWnd, &ptBR, 1);
                    s_selected.push_back({ptTL.x, ptTL.y, ptBR.x, ptBR.y});
                }
            }
        }
    }

    // Wake render thread if things are happening
    POINT pt;
    GetCursorPos(&pt);
    static POINT s_lastScreenPosPoll = {0, 0};
    bool mouseMovedPoll = (pt.x != s_lastScreenPosPoll.x || pt.y != s_lastScreenPosPoll.y);
    s_lastScreenPosPoll = pt;

    static bool s_wasEditing = false;
    // g_selectedRects is only written from this (UI) thread below, and the
    // render thread only reads it under g_stateMutex. The comparison here is
    // safe without a lock because it runs on the same thread as the writer.
    bool selectionChanged = (s_selected.size() != g_selectedRects.size());
    if (!selectionChanged) {
        for (size_t i = 0; i < s_selected.size(); ++i) {
            if (s_selected[i].left != g_selectedRects[i].left || s_selected[i].right != g_selectedRects[i].right ||
                s_selected[i].top != g_selectedRects[i].top || s_selected[i].bottom != g_selectedRects[i].bottom) {
                selectionChanged = true; break;
            }
        }
    }

    if (mouseMovedPoll || selectionChanged || isEditing != s_wasEditing || g_forceRender) {
        if (g_renderEvent) SetEvent(g_renderEvent);
    }
    s_wasEditing = isEditing;

    std::lock_guard<std::mutex> lock(g_stateMutex);
    g_cachedHwndLV = s_hwndLV;
    g_cachedHwndShellView = s_hwndShellView;
    g_cachedHwndDesktopHost = s_hwndDesktopHost;
    g_selectedRects = s_selected;
    g_isEditing = isEditing;
}

////////////////////////////////////////////////////////////////////////////////
// DirectX initialization

bool InitDirectX() {
    HRESULT hr;
    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
                           nullptr, 0, D3D11_SDK_VERSION, &g_d3dDevice, nullptr, nullptr);
    if (FAILED(hr)) return false;

    hr = g_d3dDevice.As(&g_dxgiDevice);
    if (FAILED(hr)) return false;

    hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&g_dxgiFactory));
    if (FAILED(hr)) return false;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, IID_PPV_ARGS(&g_d2dFactory));
    if (FAILED(hr)) return false;

    hr = g_d2dFactory->CreateDevice(g_dxgiDevice.Get(), &g_d2dDevice);
    if (FAILED(hr)) return false;

    return true;
}

void UninitDirectX() {
    g_d2dDevice.Reset();
    g_d2dFactory.Reset();
    g_dxgiFactory.Reset();
    g_dxgiDevice.Reset();
    g_d3dDevice.Reset();
}

////////////////////////////////////////////////////////////////////////////////
// Wallpaper capture utilizing GDI PaintDesktop

void CaptureWallpaperBitmap() {
    if (!g_overlayWnd || !g_dc) return;

    RECT rc;
    GetClientRect(g_overlayWnd, &rc);
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0) return;

    HDC hdcScreen = GetDC(nullptr);
    if (!hdcScreen) return;
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    if (!hdcMem) { ReleaseDC(nullptr, hdcScreen); return; }

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pvBits = nullptr;
    HBITMAP hBmp = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pvBits, nullptr, 0);
    if (!hBmp) { DeleteDC(hdcMem); ReleaseDC(nullptr, hdcScreen); return; }
    HGDIOBJ hOldBmp = SelectObject(hdcMem, hBmp);

    HWND hIconHost = nullptr;
    HWND hWorkerW = nullptr;
    while ((hWorkerW = FindWindowEx(nullptr, hWorkerW, L"WorkerW", nullptr)) != nullptr) {
        if (FindWindowEx(hWorkerW, nullptr, L"SHELLDLL_DefView", nullptr)) {
            hIconHost = hWorkerW;
            break;
        }
    }
    
    HWND hProgman = FindWindow(L"Progman", nullptr);
    if (!hIconHost && hProgman) {
        if (FindWindowEx(hProgman, nullptr, L"SHELLDLL_DefView", nullptr))
            hIconHost = hProgman;
    }

    HWND hWallpaperWnd = nullptr;
    if (hIconHost) {
        RECT rcIconHost = {};
        GetWindowRect(hIconHost, &rcIconHost);

        hWorkerW = nullptr;
        while ((hWorkerW = FindWindowEx(nullptr, hWorkerW, L"WorkerW", nullptr)) != nullptr) {
            if (hWorkerW == hIconHost) continue;
            if (!IsWindowVisible(hWorkerW)) continue;
            RECT rcWorker = {};
            GetWindowRect(hWorkerW, &rcWorker);
            if (rcWorker.left == rcIconHost.left && rcWorker.top == rcIconHost.top &&
                rcWorker.right == rcIconHost.right && rcWorker.bottom == rcIconHost.bottom) {
                hWallpaperWnd = hWorkerW;
                break;
            }
        }
    }

    if (!hWallpaperWnd) hWallpaperWnd = hProgman;

    bool captured = false;
    if (hWallpaperWnd) {
        HWND hDefView = FindWindowEx(hWallpaperWnd, nullptr, L"SHELLDLL_DefView", nullptr);
        
        bool overlayWasVisible = g_overlayWnd && IsWindowVisible(g_overlayWnd);
        bool defViewWasVisible = hDefView && IsWindowVisible(hDefView);

        if (overlayWasVisible) ShowWindow(g_overlayWnd, SW_HIDE);
        if (defViewWasVisible) ShowWindow(hDefView, SW_HIDE);

        captured = PrintWindow(hWallpaperWnd, hdcMem, PW_RENDERFULLCONTENT) != 0;

        if (defViewWasVisible) ShowWindow(hDefView, SW_SHOW);
        if (overlayWasVisible) ShowWindow(g_overlayWnd, SW_SHOWNA);
    }
    if (!captured) {
        POINT ptOrg = {0, 0};
        ClientToScreen(g_overlayWnd, &ptOrg);
        SetWindowOrgEx(hdcMem, ptOrg.x, ptOrg.y, nullptr);
        PaintDesktop(hdcMem);
    }

    GdiFlush();

    D2D1_BITMAP_PROPERTIES bitmapProps = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));
    g_wallpaperBitmap.Reset();
    g_dc->CreateBitmap(D2D1::SizeU(w, h), pvBits, w * 4, bitmapProps, &g_wallpaperBitmap);

    SelectObject(hdcMem, hOldBmp);
    DeleteObject(hBmp);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);

}

void RecreateBrushesAndMask(UINT width, UINT height) {
    if (!g_dc) return;

    D2D1_BITMAP_PROPERTIES1 maskProps = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET, 
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
    
    g_dc->CreateBitmap(D2D1::SizeU(width, height), nullptr, 0, &maskProps, &g_spotlightMaskBitmap);
    g_dc->CreateBitmap(D2D1::SizeU(width, height), nullptr, 0, &maskProps, &g_selectionMaskBitmap);

    g_dc->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 1.0f), &g_selectionBrush);
    g_dc->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 1.0f), &g_spotlightBrush);
}

bool CreateSwapChainResources(UINT width, UINT height) {
    HRESULT hr;
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
    if (FAILED(hr)) return false;

    hr = g_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &g_dc);
    if (FAILED(hr)) return false;

    ComPtr<IDXGISurface2> surface;
    hr = g_swapChain->GetBuffer(0, IID_PPV_ARGS(&surface));
    if (FAILED(hr)) return false;

    D2D1_BITMAP_PROPERTIES1 bitmapProperties = {};
    bitmapProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bitmapProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bitmapProperties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

    hr = g_dc->CreateBitmapFromDxgiSurface(surface.Get(), bitmapProperties, &g_targetBitmap);
    if (FAILED(hr)) return false;

    hr = DCompositionCreateDevice(g_dxgiDevice.Get(), IID_PPV_ARGS(&g_compositionDevice));
    if (FAILED(hr)) return false;

    hr = g_compositionDevice->CreateTargetForHwnd(g_overlayWnd, TRUE, &g_compositionTarget);
    if (FAILED(hr)) return false;

    hr = g_compositionDevice->CreateVisual(&g_compositionVisual);
    if (FAILED(hr)) return false;

    hr = g_compositionVisual->SetContent(g_swapChain.Get());
    if (FAILED(hr)) return false;

    hr = g_compositionTarget->SetRoot(g_compositionVisual.Get());
    if (FAILED(hr)) return false;

    hr = g_compositionDevice->Commit();
    if (FAILED(hr)) return false;

    g_dpiScale = GetMonitorDpiScale(GetWorkerWMonitor());

    g_dc->CreateEffect(kCLSID_D2D1GaussianBlur, &g_spotlightBlurEffect);
    if (g_spotlightBlurEffect) g_spotlightBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_SOFT);

    g_dc->CreateEffect(kCLSID_D2D1GaussianBlur, &g_selectionBlurEffect);
    if (g_selectionBlurEffect) g_selectionBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_SOFT);

    RecreateBrushesAndMask(width, height);
    CaptureWallpaperBitmap();

    return true;
}

void ReleaseSwapChainResources() {
    g_selectionBrush.Reset();
    g_spotlightBrush.Reset();
    g_spotlightMaskBitmap.Reset();
    g_selectionMaskBitmap.Reset();
    g_spotlightBlurEffect.Reset();
    g_selectionBlurEffect.Reset();
    g_wallpaperBitmap.Reset();
    g_targetBitmap.Reset();
    g_compositionVisual.Reset();
    g_compositionTarget.Reset();
    g_compositionDevice.Reset();
    g_dc.Reset();
    g_swapChain.Reset();
}

bool ResizeSwapChain(UINT width, UINT height) {
    if (!g_swapChain || !g_dc) return false;

    g_dc->SetTarget(nullptr);
    g_targetBitmap.Reset();
    g_spotlightMaskBitmap.Reset();
    g_selectionMaskBitmap.Reset();

    HRESULT hr = g_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr)) return false;

    ComPtr<IDXGISurface2> surface;
    hr = g_swapChain->GetBuffer(0, IID_PPV_ARGS(&surface));
    if (FAILED(hr)) return false;

    D2D1_BITMAP_PROPERTIES1 bitmapProperties = {};
    bitmapProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bitmapProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bitmapProperties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

    hr = g_dc->CreateBitmapFromDxgiSurface(surface.Get(), bitmapProperties, &g_targetBitmap);
    if (FAILED(hr)) return false;

    RecreateBrushesAndMask(width, height);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Background Render Thread Functions

void UpdateMouseAndAnimations(float deltaTime) {
    POINT screenPos;
    GetCursorPos(&screenPos);

    HWND hWndAtCursor = WindowFromPoint(screenPos);

    static POINT s_lastScreenPos = {0, 0};
    bool mouseMoved = (screenPos.x != s_lastScreenPos.x || screenPos.y != s_lastScreenPos.y);
    s_lastScreenPos = screenPos;
    g_isMouseMoving = mouseMoved;

    POINT localPos = screenPos;
    ScreenToClient(g_overlayWnd, &localPos);

    float lerpFactor = 1.0f;
    if (g_settings.mouseSmoothing > 0) {
        float speed = 30.0f - g_settings.mouseSmoothing; 
        lerpFactor = 1.0f - std::exp(-speed * deltaTime);
    }

    bool overDesktop = false;
    bool isEditing = false;
    std::vector<RECT> currentSelectedRects;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        currentSelectedRects = g_selectedRects;
        isEditing = g_isEditing;
        
        // Ensure child controls (like F2 Edit) keep the spotlight active
        HWND parent = GetAncestor(hWndAtCursor, GA_PARENT);
        overDesktop = (hWndAtCursor == g_cachedHwndLV || 
                       hWndAtCursor == g_cachedHwndShellView || 
                       hWndAtCursor == g_cachedHwndDesktopHost || 
                       hWndAtCursor == g_overlayWnd ||
                       (g_cachedHwndLV && parent == g_cachedHwndLV));
    }

    // --- Spotlight Logic ---
    static bool s_wasOverDesktop = false;
    if (overDesktop && !s_wasOverDesktop) {
        g_smoothedMousePos = { (float)localPos.x, (float)localPos.y };
        g_lastMouseTime = GetTickCount64();
        g_isIdle = false;
    } else if (overDesktop && mouseMoved) {
        g_lastMouseTime = GetTickCount64();
        g_isIdle = false;
    }
    s_wasOverDesktop = overDesktop;

    g_smoothedMousePos.x += ((float)localPos.x - g_smoothedMousePos.x) * lerpFactor;
    g_smoothedMousePos.y += ((float)localPos.y - g_smoothedMousePos.y) * lerpFactor;

    ULONGLONG timeSinceLastMove = GetTickCount64() - g_lastMouseTime;
    if (!overDesktop || timeSinceLastMove > (ULONGLONG)g_settings.idleDelay) {
        g_isIdle = true;
    }

    float targetSpotFade = g_isIdle ? 0.0f : 1.0f;
    
    if (targetSpotFade > 0.5f) { // Fading IN
        if (g_settings.spotlightFadeInDuration <= 0) {
            g_spotlightFade = 1.0f;
        } else {
            float speed = 1.0f / (g_settings.spotlightFadeInDuration / 1000.0f);
            g_spotlightFade = (std::min)(g_spotlightFade + speed * deltaTime, 1.0f);
        }
    } else { // Fading OUT
        if (g_settings.spotlightFadeOutDuration <= 0) {
            g_spotlightFade = 0.0f;
        } else {
            float speed = 1.0f / (g_settings.spotlightFadeOutDuration / 1000.0f);
            g_spotlightFade = (std::max)(g_spotlightFade - speed * deltaTime, 0.0f);
        }
    }

    // --- Selection Logic ---
    if (!currentSelectedRects.empty()) {
        g_cachedSelectedRects = currentSelectedRects;
        if (!g_isIdle) {
            g_lastSelectionTime = GetTickCount64();
        }
    }

    float targetSelFade = 0.0f;
    
    if (!g_cachedSelectedRects.empty()) {
        if (isEditing) {
            // Keep infinite timeout while editing an icon
            targetSelFade = 1.0f;
            g_lastSelectionTime = GetTickCount64(); 
        } else if (g_settings.selectionTimeout <= 0) {
            targetSelFade = currentSelectedRects.empty() ? 0.0f : 1.0f;
        } else {
            ULONGLONG timeSinceInteraction = GetTickCount64() - g_lastSelectionTime;
            if (timeSinceInteraction <= (ULONGLONG)g_settings.selectionTimeout) {
                targetSelFade = 1.0f;
            } else {
                targetSelFade = 0.0f;
            }
        }
    }
    
    if (targetSelFade > 0.5f) { // Fading IN
        if (g_settings.selectionFadeInDuration <= 0) {
            g_selectionFade = 1.0f;
        } else {
            float speed = 1.0f / (g_settings.selectionFadeInDuration / 1000.0f);
            g_selectionFade = (std::min)(g_selectionFade + speed * deltaTime, 1.0f);
        }
    } else { // Fading OUT
        if (g_settings.selectionFadeOutDuration <= 0) {
            g_selectionFade = 0.0f;
        } else {
            float speed = 1.0f / (g_settings.selectionFadeOutDuration / 1000.0f);
            g_selectionFade = (std::max)(g_selectionFade - speed * deltaTime, 0.0f);
        }
    }

    if (currentSelectedRects.empty() && g_selectionFade <= 0.0f && targetSelFade < 0.5f) {
        g_cachedSelectedRects.clear();
    }
}

void RenderOverlay() {
    if (!g_dc || !g_targetBitmap || !g_spotlightMaskBitmap || !g_selectionMaskBitmap) return;

    RECT rc;
    GetClientRect(g_overlayWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;
    if (width == 0 || height == 0) return;

    // 1. Draw Spotlight Mask
    g_dc->SetTarget(g_spotlightMaskBitmap.Get());
    g_dc->BeginDraw();
    g_dc->Clear(D2D1::ColorF(0, 0, 0, 0));

    if (g_spotlightFade > 0.001f && g_spotlightBrush) {
        g_spotlightBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, g_spotlightFade));
        float radius = (float)g_settings.spotlightRadius * g_dpiScale;
        float cornerRadius = radius * (g_settings.spotlightRoundness / 100.0f);
        
        D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
            D2D1::RectF(g_smoothedMousePos.x - radius, g_smoothedMousePos.y - radius, 
                        g_smoothedMousePos.x + radius, g_smoothedMousePos.y + radius),
            cornerRadius, cornerRadius
        );
        g_dc->FillRoundedRectangle(roundedRect, g_spotlightBrush.Get());
    }
    g_dc->EndDraw();

    // 2. Draw Selection Mask
    g_dc->SetTarget(g_selectionMaskBitmap.Get());
    g_dc->BeginDraw();
    g_dc->Clear(D2D1::ColorF(0, 0, 0, 0));

    if (g_selectionFade > 0.001f && !g_cachedSelectedRects.empty() && g_selectionBrush) {
        float activeAlpha = (g_settings.selectionOpacity / 255.0f) * g_selectionFade;
        g_selectionBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, activeAlpha));
        
        float pad = (float)g_settings.selectionPadding * g_dpiScale;
        
        for (const auto& rcBounds : g_cachedSelectedRects) {
            float rw = ((rcBounds.right + pad) - (rcBounds.left - pad)) / 2.0f;
            float rh = ((rcBounds.bottom + pad) - (rcBounds.top - pad)) / 2.0f;
            float cornerRadius = (std::min)(rw, rh) * (g_settings.selectionRoundness / 100.0f);

            D2D1_ROUNDED_RECT rRect = D2D1::RoundedRect(
                D2D1::RectF((float)rcBounds.left - pad, (float)rcBounds.top - pad,
                            (float)rcBounds.right + pad, (float)rcBounds.bottom + pad),
                cornerRadius, cornerRadius
            );
            g_dc->FillRoundedRectangle(rRect, g_selectionBrush.Get());
        }
    }
    g_dc->EndDraw();

    // 3. Draw Final Composited Screen
    g_dc->SetTarget(g_targetBitmap.Get());
    g_dc->BeginDraw();
    g_dc->Clear(D2D1::ColorF(0, 0, 0, 0));

    if (g_wallpaperBitmap) {
        g_dc->DrawBitmap(g_wallpaperBitmap.Get(),
                         D2D1::RectF(0, 0, (float)width, (float)height),
                         g_settings.idleOpacity / 255.0f);
    }

    if (g_spotlightFade > 0.001f && g_spotlightBlurEffect) {
        g_spotlightBlurEffect->SetInput(0, g_spotlightMaskBitmap.Get());
        float blurVal = (std::max)(0.1f, (std::min)((float)g_settings.spotlightBlur * g_dpiScale, 100.0f));
        g_spotlightBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, blurVal);
        g_dc->DrawImage(g_spotlightBlurEffect.Get(), D2D1_INTERPOLATION_MODE_LINEAR, D2D1_COMPOSITE_MODE_DESTINATION_OUT);
    }

    if (g_selectionFade > 0.001f && g_selectionBlurEffect) {
        g_selectionBlurEffect->SetInput(0, g_selectionMaskBitmap.Get());
        float blurVal = (std::max)(0.1f, (std::min)((float)g_settings.selectionBlur * g_dpiScale, 100.0f));
        g_selectionBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, blurVal);
        g_dc->DrawImage(g_selectionBlurEffect.Get(), D2D1_INTERPOLATION_MODE_LINEAR, D2D1_COMPOSITE_MODE_DESTINATION_OUT);
    }

    g_dc->EndDraw();
}

void RenderLoop() {
    LARGE_INTEGER freq, lastTime, currentTime;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&lastTime);

    // State memory for Idle GPU Frame Skipping
    float s_lastRenderedSpotFade = -1.0f;
    float s_lastRenderedSelFade = -1.0f;
    D2D1_POINT_2F s_lastRenderedMousePos = {-1.0f, -1.0f};

    while (!g_threadStop) {
        QueryPerformanceCounter(&currentTime);
        float deltaTime = (float)(currentTime.QuadPart - lastTime.QuadPart) / freq.QuadPart;
        lastTime = currentTime;

        if (deltaTime > 0.1f) deltaTime = 0.1f; 

        bool frameRendered = false;
        ComPtr<IDXGISwapChain1> currentSwapChain;
        
        bool isFullyIdle = false;

        {
            std::lock_guard<std::mutex> lock(g_renderMutex);
            if (g_overlayWnd && !g_unloading) {
                UpdateMouseAndAnimations(deltaTime);

                // We skip rendering if the screen is perfectly static (nothing moving or fading)
                bool stateChanged = (std::abs(g_spotlightFade - s_lastRenderedSpotFade) > 0.001f) ||
                                    (std::abs(g_selectionFade - s_lastRenderedSelFade) > 0.001f) ||
                                    ((std::abs(g_smoothedMousePos.x - s_lastRenderedMousePos.x) > 0.1f ||
                                      std::abs(g_smoothedMousePos.y - s_lastRenderedMousePos.y) > 0.1f) &&
                                     (g_spotlightFade > 0.001f || !g_isIdle)) ||
                                    g_forceRender;
                                    
                // Check if the screen is completely blank AND the mouse is physically stationary
                isFullyIdle = (g_spotlightFade <= 0.001f && g_selectionFade <= 0.001f && g_isIdle && !g_isMouseMoving);

                if (stateChanged && g_swapChain && g_targetBitmap) {
                    RenderOverlay();
                    
                    s_lastRenderedSpotFade = g_spotlightFade;
                    s_lastRenderedSelFade = g_selectionFade;
                    s_lastRenderedMousePos = g_smoothedMousePos;
                    g_forceRender = false;
                    
                    currentSwapChain = g_swapChain; // hold ref safely for Present
                    frameRendered = true;
                }
            }
        } 

        if (frameRendered && currentSwapChain) {
            // This naturally VSync-blocks to native monitor refresh rate (60Hz, 144Hz, etc.)
            HRESULT hr = currentSwapChain->Present(1, 0); 
            if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
                if (g_messageWnd) PostMessage(g_messageWnd, WM_APP_DEVICE_LOST, 0, 0);
            }
        } else {
            // We skipped the frame. 
            // If completely blank, go to deep sleep (0% CPU). 
            // Otherwise, we sleep for 10ms so we can still track fade/idle timers without locking the CPU.
            DWORD sleepTime = isFullyIdle ? INFINITE : 10;
            WaitForSingleObject(g_renderEvent, sleepTime);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Window procedures

LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;

        case WM_TIMER:
            if (!g_unloading && wParam == TIMER_ID_STATE_POLL) {
                PollDesktopState();
            }
            return 0;

        case WM_WINDOWPOSCHANGED: {
            const WINDOWPOS* wp = (const WINDOWPOS*)lParam;
            if (!(wp->flags & SWP_NOSIZE) && !g_unloading) {
                std::lock_guard<std::mutex> lock(g_renderMutex);
                ResizeSwapChain(wp->cx, wp->cy);
                CaptureWallpaperBitmap();
                g_forceRender = true;
                if (g_renderEvent) SetEvent(g_renderEvent);
            }
            break;
        }
        case WM_DESTROY:
            g_threadStop = true;
            if (g_renderEvent) SetEvent(g_renderEvent);
            if (g_renderThread) {
                if (g_renderThread->joinable()) g_renderThread->join();
                delete g_renderThread;
                g_renderThread = nullptr;
            }
            
            {
                std::lock_guard<std::mutex> lock(g_renderMutex);
                ReleaseSwapChainResources();
            }
            
            g_overlayWnd = nullptr;
            if (!g_unloading && g_messageWnd && !g_isRecreating) {
                SetTimer(g_messageWnd, TIMER_ID_RECREATE_OVERLAY, 200, nullptr);
            }
            return 0;

        case WM_APP_CLEANUP:
            DestroyWindow(hWnd);
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK MessageWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_TIMER:
            if (!g_unloading && wParam == TIMER_ID_RECREATE_OVERLAY) {
                KillTimer(hWnd, TIMER_ID_RECREATE_OVERLAY);
                g_isRecreating = true;
                if (g_overlayWnd) { DestroyWindow(g_overlayWnd); }
                CreateOverlayWindow();
                g_isRecreating = false;
            }
            else if (!g_unloading && wParam == TIMER_ID_WALLPAPER_UPDATE) {
                KillTimer(hWnd, TIMER_ID_WALLPAPER_UPDATE);
                if (g_overlayWnd) {
                    std::lock_guard<std::mutex> lock(g_renderMutex);
                    CaptureWallpaperBitmap();
                    g_forceRender = true;
                    if (g_renderEvent) SetEvent(g_renderEvent);
                }
            }
            return 0;

        case WM_APP_DEVICE_LOST:
            if (!g_unloading && !g_isRecreating) {
                SetTimer(hWnd, TIMER_ID_RECREATE_OVERLAY, 100, nullptr);
            }
            return 0;

        case WM_SETTINGCHANGE:
            if (!g_unloading) {
                bool triggerUpdate = false;
                if (wParam == SPI_SETDESKWALLPAPER) {
                    triggerUpdate = true;
                } else if (lParam) {
                    LPCWSTR strParam = (LPCWSTR)lParam;
                    if (wcscmp(strParam, L"Desktop") == 0 ||
                        wcscmp(strParam, L"ImmersiveColorSet") == 0 ||
                        wcscmp(strParam, L"WindowsTheme") == 0 ||
                        wcscmp(strParam, L"WindowMetrics") == 0) {
                        triggerUpdate = true;
                    }
                }
                if (triggerUpdate && g_overlayWnd) {
                    SetTimer(hWnd, TIMER_ID_WALLPAPER_UPDATE, 4000, nullptr);
                }
            }
            break;

        case WM_DISPLAYCHANGE:
            if (!g_unloading) {
                SetTimer(hWnd, TIMER_ID_RECREATE_OVERLAY, 50, nullptr);
            }
            break;

        case WM_DESTROY:
            g_messageWnd = nullptr;
            return 0;

        case WM_APP_CLEANUP:
            DestroyWindow(hWnd);
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Setup

bool RegisterOverlayWindowClass() {
    WNDCLASS wc = {};
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = GetCurrentModuleHandle();
    wc.lpszClassName = OVERLAY_WINDOW_CLASS;
    if (RegisterClass(&wc) == 0) {
        return GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
    }
    return true;
}

void CreateOverlayWindow() {
    if (g_overlayWnd) return;

    auto scheduleRetry = [&]() {
        if (g_messageWnd && !g_unloading) {
            SetTimer(g_messageWnd, TIMER_ID_RECREATE_OVERLAY, 500, nullptr);
        }
    };

    if (!g_initialized) {
        if (!InitDirectX()) { scheduleRetry(); return; }
        g_initialized = true;
    } else {
        HRESULT hr = g_d3dDevice ? g_d3dDevice->GetDeviceRemovedReason() : E_FAIL;
        if (FAILED(hr)) {
            UninitDirectX();
            g_initialized = false;
            if (!InitDirectX()) { scheduleRetry(); return; }
            g_initialized = true;
        }
    }

    HWND hParent = GetDesktopParent();
    if (!hParent) { scheduleRetry(); return; }

    if (!RegisterOverlayWindowClass()) { scheduleRetry(); return; }

    RECT rc;
    GetWindowRect(hParent, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    g_overlayWnd = CreateWindowEx(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        OVERLAY_WINDOW_CLASS, nullptr, WS_CHILD | WS_VISIBLE, 0, 0, width,
        height, hParent, nullptr, GetCurrentModuleHandle(), nullptr);

    if (!g_overlayWnd) { scheduleRetry(); return; }

    SetWindowPos(g_overlayWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    {
        std::lock_guard<std::mutex> lock(g_renderMutex);
        if (CreateSwapChainResources(width, height)) {
            g_isIdle = true;
            g_lastMouseTime = GetTickCount64();
            g_spotlightFade = 0.0f;
            g_selectionFade = 0.0f;
            g_forceRender = true;
            
            POINT pt; GetCursorPos(&pt);
            ScreenToClient(g_overlayWnd, &pt);
            g_smoothedMousePos = { (float)pt.x, (float)pt.y };
            
            // Poll once instantly so the background thread has correct window handles on frame 1
            PollDesktopState();

            if (!g_renderEvent) {
                g_renderEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
            }
            g_threadStop = false;
            g_renderThread = new std::thread(RenderLoop);
            
            SetTimer(g_overlayWnd, TIMER_ID_STATE_POLL, 32, nullptr); 
        } else {
            DestroyWindow(g_overlayWnd);
            g_overlayWnd = nullptr;
            scheduleRetry();
        }
    }
}

bool RegisterMessageWindowClass() {
    WNDCLASS wc = {};
    wc.lpfnWndProc = MessageWndProc;
    wc.hInstance = GetCurrentModuleHandle();
    wc.lpszClassName = MESSAGE_WINDOW_CLASS;
    if (RegisterClass(&wc) == 0) {
        return GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
    }
    return true;
}

void CreateMessageWindow() {
    if (g_messageWnd) return;
    if (!RegisterMessageWindowClass()) return;
    g_messageWnd = CreateWindowEx(0, MESSAGE_WINDOW_CLASS, nullptr, 0, 0, 0, 0,
                                  0, nullptr, nullptr, GetCurrentModuleHandle(), nullptr);
}

////////////////////////////////////////////////////////////////////////////////
// Thread-Safe Initialization Hooks

using DispatchMessageW_t = decltype(&DispatchMessageW);
DispatchMessageW_t DispatchMessageW_Original;

LRESULT WINAPI DispatchMessageW_Hook(const MSG* lpMsg) {
    if (!g_unloading) {
        // 1. Handling Initialization Execution (Runs safely on target desktop thread)
        if (lpMsg->message == WM_APP_INIT && lpMsg->hwnd == nullptr) {
            CreateMessageWindow();
            if (g_messageWnd) {
                CreateOverlayWindow();
                if (g_overlayWnd) {
                    g_initAttempts.store(0, std::memory_order_relaxed);
                    g_lastInitFailure.store(0, std::memory_order_relaxed);
                } else {
                    g_initAttempts.fetch_add(1, std::memory_order_relaxed);
                    g_lastInitFailure.store(GetTickCount64(), std::memory_order_relaxed);
                    g_initialized_desktop = false; // Reset to allow discovery again
                }
            } else {
                g_initAttempts.fetch_add(1, std::memory_order_relaxed);
                g_lastInitFailure.store(GetTickCount64(), std::memory_order_relaxed);
                g_initialized_desktop = false; // Reset to allow discovery again
            }
        }
        // 2. Hardware-level Mouse Instant Wake
        else if (g_renderEvent &&
                 ((lpMsg->message >= WM_MOUSEFIRST && lpMsg->message <= WM_MOUSELAST) ||
                  lpMsg->message == 0x0245 /* WM_POINTERUPDATE */)) {
            SetEvent(g_renderEvent);
        }

        // 3. Desktop Window Discovery (Can be safely executed by ANY thread)
        if (!g_initialized_desktop) {
            ULONGLONG now = GetTickCount64();

            // Backoff logic: If we failed 5+ times, wait 5 seconds between searches
            bool useLongBackoff = g_initAttempts.load(std::memory_order_relaxed) > 5;
            ULONGLONG lastFail = g_lastInitFailure.load(std::memory_order_relaxed);
            bool backoffActive = (useLongBackoff && lastFail != 0 && (now - lastFail) < 5000);

            if (!backoffActive) {
                static std::atomic<ULONGLONG> s_lastCheck{0};
                ULONGLONG last = s_lastCheck.load(std::memory_order_relaxed);

                // Throttle searches to every 500ms
                if (now - last >= 500) {
                    s_lastCheck.store(now, std::memory_order_relaxed);

                    HWND hLV = GetDesktopListView();
                    if (hLV && IsWindowVisible(hLV)) {
                        DWORD tid = GetWindowThreadProcessId(hLV, nullptr);
                        if (tid != 0) {
                            // We found the desktop! ANY thread can safely post this 
                            // message directly to the desktop's thread queue.
                            if (PostThreadMessage(tid, WM_APP_INIT, 0, 0)) {
                                g_initialized_desktop = true; 
                            }
                        }
                    }
                }
            }
        }
    }
    return DispatchMessageW_Original(lpMsg);
}

////////////////////////////////////////////////////////////////////////////////
// Mod lifecycle

void LoadSettings() {
    Settings newSettings;
    newSettings.idleOpacity = std::clamp(Wh_GetIntSetting(L"idleOpacity"), 0, 255);
    newSettings.spotlightRadius = std::max(Wh_GetIntSetting(L"spotlightRadius"), 10);
    newSettings.spotlightRoundness = std::clamp(Wh_GetIntSetting(L"spotlightRoundness"), 0, 100);
    newSettings.spotlightBlur = std::max(Wh_GetIntSetting(L"spotlightBlur"), 0);
    newSettings.idleDelay = std::max(Wh_GetIntSetting(L"idleDelay"), 0);
    newSettings.spotlightFadeInDuration = std::max(Wh_GetIntSetting(L"spotlightFadeInDuration"), 0);
    newSettings.spotlightFadeOutDuration = std::max(Wh_GetIntSetting(L"spotlightFadeOutDuration"), 0);
    newSettings.mouseSmoothing = std::clamp(Wh_GetIntSetting(L"mouseSmoothing"), 0, 20);
    
    newSettings.selectionOpacity = std::clamp(Wh_GetIntSetting(L"selectionOpacity"), 0, 255);
    newSettings.selectionPadding = std::max(Wh_GetIntSetting(L"selectionPadding"), 0);
    newSettings.selectionRoundness = std::clamp(Wh_GetIntSetting(L"selectionRoundness"), 0, 100);
    newSettings.selectionBlur = std::max(Wh_GetIntSetting(L"selectionBlur"), 0);
    newSettings.selectionTimeout = std::max(Wh_GetIntSetting(L"selectionTimeout"), 0);
    newSettings.selectionFadeInDuration = std::max(Wh_GetIntSetting(L"selectionFadeInDuration"), 0);
    newSettings.selectionFadeOutDuration = std::max(Wh_GetIntSetting(L"selectionFadeOutDuration"), 0);

    std::lock_guard<std::mutex> lock(g_renderMutex);
    g_settings = newSettings;
    g_forceRender = true;
    if (g_renderEvent) SetEvent(g_renderEvent);
}

BOOL Wh_ModInit() {
    LoadSettings();
    WindhawkUtils::SetFunctionHook(DispatchMessageW, DispatchMessageW_Hook, &DispatchMessageW_Original);

    return TRUE;
}

void Wh_ModUninit() {
    g_unloading = true;
    
    if (g_overlayWnd) SendMessage(g_overlayWnd, WM_APP_CLEANUP, 0, 0);
    if (g_messageWnd) SendMessage(g_messageWnd, WM_APP_CLEANUP, 0, 0);
    
    UninitDirectX();
    
    if (g_renderEvent) {
        CloseHandle(g_renderEvent);
        g_renderEvent = nullptr;
    }
    
    UnregisterClass(OVERLAY_WINDOW_CLASS, GetCurrentModuleHandle());
    UnregisterClass(MESSAGE_WINDOW_CLASS, GetCurrentModuleHandle());
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
