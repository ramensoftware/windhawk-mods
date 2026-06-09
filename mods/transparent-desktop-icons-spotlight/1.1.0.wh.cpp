// ==WindhawkMod==
// @id              transparent-desktop-icons-spotlight
// @name            Transparent Desktop Icons with Spotlight
// @description     Make desktop icons transparent when idle with an interactive spotlight effect. Hover over the desktop or select icons to reveal them with full clarity and customizable transitions.
// @version         1.1.0
// @author          drgutman
// @github          https://github.com/drgutman
// @homepage        https://instagram.com/drgutman
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -ldxgi -ld2d1 -ld3d11 -ldcomp -lgdi32 -lshcore -lwindowscodecs -lole32 -luuid -lshell32 -ladvapi32
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
  $description: Mouse Spotlight Edge Blur in pixels (0 = hard edge)
- idleDelay: 2000
  $name: Mouse Spotlight Idle Timeout (ms)
  $description: Time in milliseconds before the spotlight begins to fade out (0 = keep visible indefinitely while over desktop)
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
- toggleOnDoubleClick: true
  $name: Toggle Overlay on Double-Click
  $description: Double-click on empty desktop background to hide or show the overlay.
- toggleFadeInDuration: 600
  $name: Overlay Toggle Fade In Duration (ms)
  $description: How long it takes for the overlay to fade back in when toggled on.
- toggleFadeOutDuration: 600
  $name: Overlay Toggle Fade Out Duration (ms)
  $description: How long it takes for the overlay to fade out when toggled off.
*/
// ==/WindhawkModSettings==

#include <algorithm>
#include <commctrl.h>
#include <initguid.h>
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
#include <wincodec.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <knownfolders.h>
#include <string>

using namespace std::literals;
using Microsoft::WRL::ComPtr;

////////////////////////////////////////////////////////////////////////////////
// Timer IDs

#define TIMER_ID_STATE_POLL       2   // Heartbeat: revalidate desktop window handles (1s)
#define TIMER_ID_RECREATE_OVERLAY 4
#define TIMER_ID_WALLPAPER_UPDATE 5

////////////////////////////////////////////////////////////////////////////////
// Custom messages

UINT WM_APP_CLEANUP = 0;
UINT WM_APP_DEVICE_LOST = 0;
UINT WM_APP_TRIGGER_REFRESH = 0;
UINT WM_APP_SELECTION_UPDATE = 0;

////////////////////////////////////////////////////////////////////////////////
// Window / subclass identifiers

#define OVERLAY_WINDOW_CLASS  (L"TransparentDesktopSpotlight_" WH_MOD_ID)
#define MESSAGE_WINDOW_CLASS  (L"TransparentDesktopSpotlight_Message_" WH_MOD_ID)

#define SUBCLASS_ID_LV  1   // SysListView32
#define SUBCLASS_ID_SV  2   // SHELLDLL_DefView

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

    bool toggleOnDoubleClick;
    int toggleFadeInDuration;
    int toggleFadeOutDuration;
} g_settings;

////////////////////////////////////////////////////////////////////////////////
// Global state

// Window handles
HWND g_overlayWnd   = nullptr;
HWND g_messageWnd   = nullptr;
std::atomic<bool> g_unloading{false};
bool g_isRecreating = false;
std::atomic<bool> g_selectionUpdatePending{false};

// Cached overlay dimensions (written under g_renderMutex, read in render thread)
UINT g_overlayWidth  = 0;
UINT g_overlayHeight = 0;

// Subclassed desktop windows
HWND g_subclassedLV = nullptr;   // SysListView32
HWND g_subclassedSV = nullptr;   // SHELLDLL_DefView
HWND g_subclassedHost = nullptr;

// Background render thread
std::thread*        g_renderThread = nullptr;
std::atomic<bool>   g_threadStop{false};
std::mutex          g_renderMutex;
std::atomic<bool>   g_forceRender{true};
HANDLE              g_renderEvent = nullptr;

std::thread* g_regThread = nullptr;
HANDLE       g_hRegStopEvent = nullptr;

// DirectX pipeline resources
ComPtr<ID3D11Device>         g_d3dDevice;
ComPtr<IDXGIDevice>          g_dxgiDevice;
ComPtr<IDXGIFactory2>        g_dxgiFactory;
ComPtr<ID2D1Factory1>        g_d2dFactory;
ComPtr<ID2D1Device>          g_d2dDevice;
ComPtr<IDXGISwapChain1>      g_swapChain;
ComPtr<ID2D1DeviceContext>   g_dc;
ComPtr<ID2D1Bitmap1>         g_targetBitmap;
ComPtr<ID2D1Bitmap1>         g_spotlightMaskBitmap;
ComPtr<ID2D1Bitmap1>         g_selectionMaskBitmap;

ComPtr<IDCompositionDevice>  g_compositionDevice;
ComPtr<IDCompositionTarget>  g_compositionTarget;
ComPtr<IDCompositionVisual>  g_compositionVisual;

// Textures, brushes, and effects
ComPtr<ID2D1Bitmap>     g_wallpaperBitmap;      // Original wallpaper image (full size)
ComPtr<ID2D1Effect>     g_spotlightBlurEffect;
ComPtr<ID2D1Effect>     g_selectionBlurEffect;
ComPtr<ID2D1SolidColorBrush> g_spotlightBrush;
ComPtr<ID2D1SolidColorBrush> g_selectionBrush;
ComPtr<ID2D1BitmapBrush>     g_tileBrush;        // For tiled wallpaper

// Wallpaper state (protected by g_renderMutex)
bool                       g_hasWallpaper = false;          // True if a wallpaper image is set (not solid color)
D2D1_COLOR_F               g_desktopBgColor = {0.0f, 0.0f, 0.0f, 1.0f};
DESKTOP_WALLPAPER_POSITION g_wallpaperPosition = DWPOS_CENTER; // Enum replaces int style and bool tile
std::wstring               g_currentWallpaperPath;           // Path of currently loaded wallpaper (for change detection)
FILETIME                   g_lastWallpaperWriteTime = {0, 0}; // Track file modifications!
// WIC factory
ComPtr<IWICImagingFactory> g_wicFactory;

// Animation and render state (render thread, under g_renderMutex)
D2D1_POINT_2F g_smoothedMousePos  = {0.0f, 0.0f};
bool          g_isIdle             = true;
ULONGLONG     g_lastMouseTime      = 0;
float         g_spotlightFade      = 0.0f;

ULONGLONG         g_lastSelectionTime = 0;
float             g_selectionFade     = 0.0f;
std::vector<RECT> g_cachedSelectedRects;

// Cross-thread signals (render thread reads, UI thread writes)
std::atomic<bool> g_clickOnDesktop{false};
std::atomic<bool> g_selectionJustChanged{false};
std::atomic<bool> g_isMouseOverDesktop{false};

// UI thread state (under g_stateMutex)
std::mutex        g_stateMutex;
HWND              g_cachedHwndLV          = nullptr;
HWND              g_cachedHwndShellView   = nullptr;
HWND              g_cachedHwndDesktopHost = nullptr;
std::vector<RECT> g_selectedRects;
bool              g_isEditing             = false;

// DPI scaling factor
float g_dpiScale = 1.0f;

// Initialization state
std::atomic<bool> g_initialized_desktop{false};
std::atomic<bool> g_initialized{false};

// Cold-start retry tracking
std::atomic<int>       g_initAttempts{0};
std::atomic<ULONGLONG> g_lastInitFailure{0};

// Overlay toggle
std::atomic<bool> g_overlayBypassed{false};
float g_masterFade = 1.0f;

//

////////////////////////////////////////////////////////////////////////////////
// Forward declarations

void     CreateOverlayWindow();
void     CreateMessageWindow();
void     LoadSettings();
void     RefreshWallpaperAndStyle();   // New unified loader
HWND     GetDesktopListView();
void     UpdateSelectionRects();
void     RevalidateDesktopWindows();
bool     ApplyDesktopSubclasses(HWND hLV);
void     RemoveDesktopSubclasses();

bool InitWIC();
////////////////////////////////////////////////////////////////////////////////
// Utility functions

// Direct2D Gaussian Blur CLSID
static const IID kCLSID_D2D1GaussianBlur = {
    0x1feb6d69, 0x2fe6, 0x4ac9,
    {0x8c, 0x58, 0x1d, 0x7f, 0x93, 0xe7, 0xa6, 0xa5}};

HWND GetDesktopListView() {
    HWND hShellView = nullptr;
    HWND hProgman = FindWindow(L"Progman", nullptr);
    if (hProgman)
        hShellView = FindWindowEx(hProgman, nullptr, L"SHELLDLL_DefView", nullptr);

    if (!hShellView) {
        HWND hWorkerW = nullptr;
        while ((hWorkerW = FindWindowEx(nullptr, hWorkerW, L"WorkerW", nullptr)) != nullptr) {
            hShellView = FindWindowEx(hWorkerW, nullptr, L"SHELLDLL_DefView", nullptr);
            if (hShellView) break;
        }
    }

    if (hShellView)
        return FindWindowEx(hShellView, nullptr, L"SysListView32", nullptr);

    return nullptr;
}

HWND GetDesktopParent() {
    HWND hLV = GetDesktopListView();
    if (hLV) {
        HWND hShellView = GetParent(hLV);
        if (hShellView)
            return GetParent(hShellView);
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// UI thread — selection rect computation

void UpdateSelectionRects() {
    HWND hLV = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        hLV = g_cachedHwndLV;
    }
    if (!hLV || !IsWindow(hLV)) return;

    bool isEditing = false;
    HWND hEdit = FindWindowEx(hLV, nullptr, L"Edit", nullptr);
    if (!hEdit) hEdit = FindWindowEx(hLV, nullptr, L"WC_EDIT", nullptr);
    if (hEdit && IsWindowVisible(hEdit))
        isEditing = true;

    static std::vector<RECT> s_rects;
    s_rects.clear();

    HWND overlay = g_overlayWnd;
    int count = (int)SendMessage(hLV, LVM_GETSELECTEDCOUNT, 0, 0);

    // If count is 0, the user has deselected everything. 
    // We intentionally ignore LVNI_FOCUSED here because Windows keeps a "ghost" 
    // focus anchor for keyboard navigation, but it's not actually selected.
    if (count > 0 && overlay) {
        // 1. Track all selected items
        int idx = -1;
        while ((idx = (int)SendMessage(hLV, LVM_GETNEXTITEM, idx, LVNI_SELECTED)) != -1) {
            RECT rcBounds = {}; 
            rcBounds.left = LVIR_BOUNDS;
            if (SendMessage(hLV, LVM_GETITEMRECT, idx, (LPARAM)&rcBounds)) {
                POINT ptTL = {rcBounds.left, rcBounds.top};
                POINT ptBR = {rcBounds.right, rcBounds.bottom};
                MapWindowPoints(hLV, overlay, &ptTL, 1);
                MapWindowPoints(hLV, overlay, &ptBR, 1);
                s_rects.push_back({ptTL.x, ptTL.y, ptBR.x, ptBR.y});
            }
        }

        // 2. Track the focused item (for edge cases where focus != selection)
        int focusedIdx = (int)SendMessage(hLV, LVM_GETNEXTITEM, -1, LVNI_FOCUSED);
        if (focusedIdx != -1) {
            RECT rcBounds = {};
            rcBounds.left = LVIR_BOUNDS;
            if (SendMessage(hLV, LVM_GETITEMRECT, focusedIdx, (LPARAM)&rcBounds)) {
                POINT ptTL = {rcBounds.left, rcBounds.top};
                POINT ptBR = {rcBounds.right, rcBounds.bottom};
                MapWindowPoints(hLV, overlay, &ptTL, 1);
                MapWindowPoints(hLV, overlay, &ptBR, 1);
                RECT r = {ptTL.x, ptTL.y, ptBR.x, ptBR.y};
                auto rectsEqual = [](const RECT& a, const RECT& b) {
                    return a.left == b.left && a.top == b.top && a.right == b.right && a.bottom == b.bottom;
                };
                if (std::find_if(s_rects.begin(), s_rects.end(), [&](const RECT& existing) { return rectsEqual(existing, r); }) == s_rects.end()) {
                    s_rects.push_back(r);
                }
            }
        }
    }
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        g_selectedRects = s_rects;
        g_isEditing     = isEditing;
    }

    g_selectionJustChanged.store(true, std::memory_order_relaxed);
    if (g_renderEvent) SetEvent(g_renderEvent);
}

////////////////////////////////////////////////////////////////////////////////
// UI thread — desktop window heartbeat (1s)

void RevalidateDesktopWindows() {
    HWND hLV = GetDesktopListView();

    if (!hLV || !IsWindow(hLV)) {
        RemoveDesktopSubclasses();
        std::lock_guard<std::mutex> lock(g_stateMutex);
        g_cachedHwndLV          = nullptr;
        g_cachedHwndShellView   = nullptr;
        g_cachedHwndDesktopHost = nullptr;
        return;
    }

    if (hLV == g_subclassedLV) return;

    RemoveDesktopSubclasses();
    ApplyDesktopSubclasses(hLV);
}

float GetMonitorDpiScale(HMONITOR monitor) {
    UINT dpiX = 96, dpiY = 96;
    if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)))
        return dpiX / 96.0f;
    return 1.0f;
}

HMONITOR GetWorkerWMonitor() {
    HWND hParent = GetDesktopParent();
    if (hParent)
        return MonitorFromWindow(hParent, MONITOR_DEFAULTTONEAREST);
    return MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);
}

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (LPCWSTR)GetCurrentModuleHandle, &module))
        return nullptr;
    return module;
}

// Reads TranscodedImageCache binary value for the guaranteed active image path (fixes Slideshows)

std::wstring ReadTranscodedImageCache() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_READ, &hKey) != ERROR_SUCCESS) return L"";
    DWORD type = 0, size = 0;
    if (RegQueryValueExW(hKey, L"TranscodedImageCache", nullptr, &type, nullptr, &size) != ERROR_SUCCESS || type != REG_BINARY) {
        RegCloseKey(hKey); return L"";
    }

    // SANITY CHECK: Prevent std::bad_alloc crash if registry is corrupted with massive size
    if (size == 0 || size > 1024 * 1024) { 
        RegCloseKey(hKey); return L""; 
    }

    std::vector<BYTE> buffer(size);
    RegQueryValueExW(hKey, L"TranscodedImageCache", nullptr, nullptr, buffer.data(), &size);
    RegCloseKey(hKey);

    if (size <= 24) return L""; // Skip 24-byte header
    const wchar_t* pathStart = reinterpret_cast<const wchar_t*>(buffer.data() + 24);
    size_t maxChars = (size - 24) / sizeof(wchar_t);
    std::wstring path(pathStart, wcsnlen(pathStart, maxChars));

    // SANITY CHECK: Ensure it looks like a valid path (Drive letter or UNC)
    if (path.length() < 3) return L"";
    if (path[1] == L':' && path[2] == L'\\') return path; // C:\...
    if (path[0] == L'\\' && path[1] == L'\\') return path; // \\server\...
    return L"";
}

// Reads exact background color string directly (fixes Solid Color swaps)
void ReadBackgroundColor(D2D1_COLOR_F& color) {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Colors", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        WCHAR rgb[32] = {};
        DWORD size = sizeof(rgb);
        if (RegQueryValueExW(hKey, L"Background", nullptr, nullptr, (LPBYTE)rgb, &size) == ERROR_SUCCESS) {
            int r = 0, g = 0, b = 0;
            if (swscanf_s(rgb, L"%d %d %d", &r, &g, &b) == 3)
                color = D2D1::ColorF(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
        }
        RegCloseKey(hKey);
    }
}


// Load wallpaper image from given path into a Direct2D bitmap.
// Returns true on success, false on failure (file missing, corrupt, etc.)
bool LoadWallpaperBitmap(const std::wstring& path, ComPtr<ID2D1Bitmap>& outBitmap) {
    if (!g_dc || path.empty()) return false;
    if (!g_wicFactory && !InitWIC()) return false;

    ComPtr<IWICBitmapDecoder> decoder;
    // We use GENERIC_READ. If Windows is actively saving the file, this will fail with ERROR_SHARING_VIOLATION.
    HRESULT hr = g_wicFactory->CreateDecoderFromFilename(path.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
    if (FAILED(hr)) {
        return false;
    }

    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr)) return false;

    ComPtr<IWICFormatConverter> converter;
    hr = g_wicFactory->CreateFormatConverter(&converter);
    if (FAILED(hr)) return false;

    hr = converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom);
    if (FAILED(hr)) return false;

    ComPtr<ID2D1Bitmap> bitmap;
    hr = g_dc->CreateBitmapFromWicBitmap(converter.Get(), nullptr, &bitmap);
    if (FAILED(hr)) return false;

    outBitmap = bitmap;
    return true;
}

// Unified wallpaper and style refresh.
// Uses the modern IDesktopWallpaper API to guarantee 100% accurate reads
// Bypasses registry cache files entirely.
// Must be called while holding g_renderMutex.
void RefreshWallpaperAndStyle() {
    if (!g_dc) return;

    // THREADING CONTRACT: Must be called while holding g_renderMutex AND on the
    // Explorer UI thread (STA). CoCreateInstance for IDesktopWallpaper requires STA.
    // All current call sites satisfy both constraints:
    //   - CreateSwapChainResources (called from CreateOverlayWindow, UI thread, under mutex)
    //   - WM_WINDOWPOSCHANGED handler (overlay wndproc, UI thread, under mutex)
    //   - TIMER_ID_WALLPAPER_UPDATE handler (message wndproc, UI thread, under mutex)
    // Do not add call sites from the render thread or any non-UI thread.
    HRESULT hrCom = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);


    DWORD bgType = 0; 
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Wallpapers", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD size = sizeof(bgType);
        RegQueryValueExW(hKey, L"BackgroundType", nullptr, nullptr, (LPBYTE)&bgType, &size);
        RegCloseKey(hKey);
    }

    // 1. Force a direct color read (bypasses COM cache)
    D2D1_COLOR_F newColor = {0.0f, 0.0f, 0.0f, 1.0f};
    ReadBackgroundColor(newColor);
    bool colorChanged = (newColor.r != g_desktopBgColor.r || newColor.g != g_desktopBgColor.g || newColor.b != g_desktopBgColor.b);
    g_desktopBgColor = newColor;

    if (bgType == 1) { 
        g_hasWallpaper = false;
        g_wallpaperBitmap.Reset();
        g_tileBrush.Reset();
        g_currentWallpaperPath.clear();
        g_lastWallpaperWriteTime = {0, 0};
        
        if (colorChanged) g_forceRender = true;
    } else {
        // 2. Force a direct image path read (bypasses COM cache for Slideshows)
        std::wstring newPath = ReadTranscodedImageCache();
        
        // Fallback if cache is completely empty
        if (newPath.empty()) {
            if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
                WCHAR pathBuf[MAX_PATH] = {};
                DWORD size = sizeof(pathBuf);
                if (RegQueryValueExW(hKey, L"Wallpaper", nullptr, nullptr, (LPBYTE)pathBuf, &size) == ERROR_SUCCESS) {
                    newPath = pathBuf;
                }
                RegCloseKey(hKey);
            }
        }

        // 3. We STILL use COM just for the Fit position (Center, Stretch, etc) because that works well
        DESKTOP_WALLPAPER_POSITION pos = DWPOS_CENTER;
        HRESULT coInitHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        ComPtr<IDesktopWallpaper> pDesktopWallpaper;
        if (SUCCEEDED(CoCreateInstance(CLSID_DesktopWallpaper, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pDesktopWallpaper)))) {
            pDesktopWallpaper->GetPosition(&pos);
        }
        if (SUCCEEDED(coInitHr)) CoUninitialize();
        

        // 4. File Timestamp Check
        WIN32_FILE_ATTRIBUTE_DATA fileInfo;
        FILETIME newWriteTime = {0, 0};
        if (GetFileAttributesExW(newPath.c_str(), GetFileExInfoStandard, &fileInfo)) {
            newWriteTime = fileInfo.ftLastWriteTime;
        }

        bool pathChanged = (newPath != g_currentWallpaperPath);
        bool timeChanged = (CompareFileTime(&newWriteTime, &g_lastWallpaperWriteTime) != 0);
        bool posChanged = (pos != g_wallpaperPosition);

        g_wallpaperPosition = pos;

        if (pathChanged || timeChanged || posChanged) {
            ComPtr<ID2D1Bitmap> newBitmap;
            if (!newPath.empty() && LoadWallpaperBitmap(newPath, newBitmap)) {
                g_wallpaperBitmap = newBitmap;
                g_hasWallpaper = true;
                g_currentWallpaperPath = newPath;
                g_lastWallpaperWriteTime = newWriteTime;
            } else {
                if (g_messageWnd) SetTimer(g_messageWnd, TIMER_ID_WALLPAPER_UPDATE, 300, nullptr);
            }

            g_tileBrush.Reset();
            if (g_hasWallpaper && g_wallpaperPosition == DWPOS_TILE && g_wallpaperBitmap) {
                D2D1_BITMAP_BRUSH_PROPERTIES brushProps = D2D1::BitmapBrushProperties(
                    D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                g_dc->CreateBitmapBrush(g_wallpaperBitmap.Get(), brushProps, &g_tileBrush);
            }
        } else {
        }
    }

    g_forceRender = true;
    if (g_renderEvent) SetEvent(g_renderEvent);

    // Clean up COM if we initialized it
    if (SUCCEEDED(hrCom)) CoUninitialize();
}

////////////////////////////////////////////////////////////////////////////////
// Desktop subclassing

LRESULT CALLBACK ListViewSubclassProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
LRESULT CALLBACK ShellViewSubclassProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

bool ApplyDesktopSubclasses(HWND hLV) {
    if (!hLV || !IsWindow(hLV)) return false;

    HWND hSV   = GetParent(hLV);
    HWND hHost = hSV ? GetParent(hSV) : nullptr;
    if (!hSV) return false;

    if (!SetWindowSubclass(hLV, ListViewSubclassProc, SUBCLASS_ID_LV, 0))
        return false;

    if (!SetWindowSubclass(hSV, ShellViewSubclassProc, SUBCLASS_ID_SV, 0)) {
        RemoveWindowSubclass(hLV, ListViewSubclassProc, SUBCLASS_ID_LV);
        return false;
    }

    g_subclassedLV = hLV;
    g_subclassedSV = hSV;

    std::lock_guard<std::mutex> lock(g_stateMutex);
    g_cachedHwndLV          = hLV;
    g_cachedHwndShellView   = hSV;
    g_cachedHwndDesktopHost = hHost;

    return true;
}

void RemoveDesktopSubclasses() {
    if (g_subclassedSV) {
        RemoveWindowSubclass(g_subclassedSV, ShellViewSubclassProc, SUBCLASS_ID_SV);
        g_subclassedSV = nullptr;
    }
    if (g_subclassedLV) {
        RemoveWindowSubclass(g_subclassedLV, ListViewSubclassProc, SUBCLASS_ID_LV);
        g_subclassedLV = nullptr;
    }
}

static void HandleDesktopMouseMove(HWND hWnd) {
    if (!g_isMouseOverDesktop.load(std::memory_order_relaxed))
        g_isMouseOverDesktop.store(true, std::memory_order_relaxed);
    TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, hWnd, 0 };
    TrackMouseEvent(&tme);
    if (g_renderEvent) SetEvent(g_renderEvent);
}

static void HandleDesktopMouseLeave() {
    POINT pt; GetCursorPos(&pt);
    HWND hUnder = WindowFromPoint(pt);
    HWND hLV = nullptr, hSV = nullptr, hHost = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        hLV = g_cachedHwndLV; hSV = g_cachedHwndShellView; hHost = g_cachedHwndDesktopHost;
    }
    bool stillOver = (hUnder == hLV || hUnder == hSV || hUnder == hHost || hUnder == g_overlayWnd);
    if (!stillOver && hUnder && hLV) {
        if (GetAncestor(hUnder, GA_PARENT) == hLV) stillOver = true;
    }
    if (!stillOver) {
        g_isMouseOverDesktop.store(false, std::memory_order_relaxed);
        if (g_renderEvent) SetEvent(g_renderEvent);
    }
}


LRESULT CALLBACK ListViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                                        UINT_PTR uId, DWORD_PTR dwRef) {
    if (!g_unloading) {
        switch (uMsg) {
            case WM_MOUSEMOVE:
                HandleDesktopMouseMove(hWnd);
                break;
            case WM_MOUSELEAVE:
                HandleDesktopMouseLeave();
                break;

            case WM_LBUTTONDBLCLK:
                {
                    bool suppressSpotlight = false;
                    if (g_settings.toggleOnDoubleClick) {
                        LVHITTESTINFO hti = {};
                        hti.pt.x = (short)LOWORD(lParam);
                        hti.pt.y = (short)HIWORD(lParam);
                        ListView_HitTest(hWnd, &hti);
                        if (hti.flags & LVHT_NOWHERE) {
                            bool nowBypassed = !g_overlayBypassed.load(std::memory_order_relaxed);
                            g_overlayBypassed.store(nowBypassed, std::memory_order_relaxed);
                            if (g_renderEvent) SetEvent(g_renderEvent);
                            suppressSpotlight = nowBypassed; // toggling OFF: don't wake the spotlight
                        }
                    }
                    if (!suppressSpotlight) {
                        g_clickOnDesktop.store(true, std::memory_order_relaxed);
                        if (g_renderEvent) SetEvent(g_renderEvent);
                    }
                }
                break;
                
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:

            case WM_MBUTTONDOWN:
                g_clickOnDesktop.store(true, std::memory_order_relaxed);
                if (g_renderEvent) SetEvent(g_renderEvent);
                break;

        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ShellViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                        UINT_PTR uId, DWORD_PTR dwRef) {
        if (!g_unloading) {
            switch (uMsg) {
                case WM_MOUSEMOVE:
                    HandleDesktopMouseMove(hWnd);
                    break;
                case WM_MOUSELEAVE:
                    HandleDesktopMouseLeave();
                    break;
                }

        // Forward critical desktop redraw broadcasts directly to our monitor
        if (uMsg == WM_SETTINGCHANGE || uMsg == WM_THEMECHANGED) {
            if (g_messageWnd) {
                PostMessageW(g_messageWnd, WM_APP_TRIGGER_REFRESH, 0, 0); // CHANGED THIS
            }
        }
        
        if (uMsg == WM_NOTIFY) {
            NMHDR* hdr = (NMHDR*)lParam;
            if (hdr && hdr->hwndFrom == g_subclassedLV) {
                switch (hdr->code) {
                    case LVN_ITEMCHANGED:
                        if (g_overlayWnd) {
                            bool expected = false;
                            // Only post if an update isn't already pending
                            if (g_selectionUpdatePending.compare_exchange_strong(expected, true)) {
                                PostMessageW(g_overlayWnd, WM_APP_SELECTION_UPDATE, 0, 0);
                            }
                        }
                        break;
                    case LVN_BEGINLABELEDIT:
                        {
                            std::lock_guard<std::mutex> lock(g_stateMutex);
                            g_isEditing = true;
                        }
                        g_selectionJustChanged.store(true, std::memory_order_relaxed);
                        if (g_renderEvent) SetEvent(g_renderEvent);
                        break;
                    case LVN_ENDLABELEDIT:
                        {
                            std::lock_guard<std::mutex> lock(g_stateMutex);
                            g_isEditing = false;
                        }
                        g_selectionJustChanged.store(true, std::memory_order_relaxed);
                        if (g_renderEvent) SetEvent(g_renderEvent);
                        break;
                }
            }
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// DirectX & WIC initialization

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

bool InitWIC() {
    if (g_wicFactory) return true;
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                  CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_wicFactory));
    return SUCCEEDED(hr);
}

////////////////////////////////////////////////////////////////////////////////
// Swap chain and D2D resources

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
    scd.Width        = width;
    scd.Height       = height;
    scd.Format       = DXGI_FORMAT_B8G8R8A8_UNORM;
    scd.SampleDesc.Count = 1;
    scd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount  = 2;
    scd.Scaling      = DXGI_SCALING_STRETCH;
    scd.SwapEffect   = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scd.AlphaMode    = DXGI_ALPHA_MODE_PREMULTIPLIED;

    hr = g_dxgiFactory->CreateSwapChainForComposition(g_dxgiDevice.Get(), &scd, nullptr, &g_swapChain);
    if (FAILED(hr)) return false;

    hr = g_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &g_dc);
    if (FAILED(hr)) return false;

    ComPtr<IDXGISurface2> surface;
    hr = g_swapChain->GetBuffer(0, IID_PPV_ARGS(&surface));
    if (FAILED(hr)) return false;

    D2D1_BITMAP_PROPERTIES1 bitmapProperties = {};
    bitmapProperties.pixelFormat.alphaMode  = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bitmapProperties.pixelFormat.format     = DXGI_FORMAT_B8G8R8A8_UNORM;
    bitmapProperties.bitmapOptions          = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

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
    if (g_spotlightBlurEffect)
        g_spotlightBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_SOFT);

    g_dc->CreateEffect(kCLSID_D2D1GaussianBlur, &g_selectionBlurEffect);
    if (g_selectionBlurEffect)
        g_selectionBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_SOFT);

    RecreateBrushesAndMask(width, height);
    RefreshWallpaperAndStyle();  // initial load

    return true;
}

void ReleaseSwapChainResources() {
    g_tileBrush.Reset();
    g_selectionBrush.Reset();
    g_spotlightBrush.Reset();
    g_spotlightMaskBitmap.Reset();
    g_selectionMaskBitmap.Reset();
    g_spotlightBlurEffect.Reset();
    g_selectionBlurEffect.Reset();

    // FIX: Reset the logical state so RefreshWallpaperAndStyle() 
    // knows it needs to reload the image from disk after a window recreation.
    g_hasWallpaper = false;
    g_currentWallpaperPath.clear();
    g_lastWallpaperWriteTime = {0, 0};

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
    bitmapProperties.pixelFormat.alphaMode  = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bitmapProperties.pixelFormat.format     = DXGI_FORMAT_B8G8R8A8_UNORM;
    bitmapProperties.bitmapOptions          = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

    hr = g_dc->CreateBitmapFromDxgiSurface(surface.Get(), bitmapProperties, &g_targetBitmap);
    if (FAILED(hr)) return false;

    RecreateBrushesAndMask(width, height);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Helper: compute source and target rectangles for wallpaper drawing
// refSize must be in LOGICAL coordinates (same space as render target / window)

void ComputeWallpaperRects(const D2D1_SIZE_F& imgSize,
                           const D2D1_SIZE_F& refSize,
                           DESKTOP_WALLPAPER_POSITION position,
                           D2D1_RECT_F& sourceRect,
                           D2D1_RECT_F& targetRect) {
    // Default: full source, full target
    sourceRect = D2D1::RectF(0, 0, imgSize.width, imgSize.height);
    targetRect = D2D1::RectF(0, 0, refSize.width, refSize.height);

    switch (position) {
        case DWPOS_CENTER: {
            // Center without scaling
            float left = (refSize.width - imgSize.width) / 2.0f;
            float top  = (refSize.height - imgSize.height) / 2.0f;
            targetRect = D2D1::RectF(left, top, left + imgSize.width, top + imgSize.height);
            break;
        }
        case DWPOS_FIT: {
            // Scale to fit entirely within screen (letterbox)
            float scale = std::min(refSize.width / imgSize.width, refSize.height / imgSize.height);
            float drawW = imgSize.width * scale;
            float drawH = imgSize.height * scale;
            float left = (refSize.width - drawW) / 2.0f;
            float top  = (refSize.height - drawH) / 2.0f;
            targetRect = D2D1::RectF(left, top, left + drawW, top + drawH);
            break;
        }
        case DWPOS_FILL: {
            float scaleX = refSize.width / imgSize.width;
            float scaleY = refSize.height / imgSize.height;
            float scale = std::max(scaleX, scaleY);

            float srcW = refSize.width / scale;
            float srcH = refSize.height / scale;
            
            // 1. Default mathematical crop (divided by 2)
            float srcX = (imgSize.width - srcW) / 2.0f;
            float srcY = (imgSize.height - srcH) / 2.0f;

            // 2. THE UNIVERSAL WINDOWS QUIRK
            // Windows Shell hardcodes a /3 divisor for the centering crop, 
            // completely ignoring standard mathematical centering (/2).
            // This applies at ALL DPI scales, including 100%.
            float screenAR = refSize.height / refSize.width;
            float correctCropH = imgSize.height - (imgSize.width * screenAR);
            float correctCropW = imgSize.width - (imgSize.height / screenAR);

            if (scaleX > scaleY) {
                srcY = correctCropH / 3.0f;
            } else if (scaleY > scaleX) {
                srcX = correctCropW / 3.0f;
            }

            // 3. RESIZE THEN CROP (The Painter's Pipeline)
            // Map the FULL source image to an oversized target rect.
            float drawW = imgSize.width * scale;
            float drawH = imgSize.height * scale;
            
            // Project the source offset into target coordinates
            float targetLeft = -(srcX * scale);
            float targetTop  = -(srcY * scale);

            sourceRect = D2D1::RectF(0, 0, imgSize.width, imgSize.height);
            targetRect = D2D1::RectF(targetLeft, targetTop, targetLeft + drawW, targetTop + drawH);
            break;
        }
        case DWPOS_STRETCH:
        default: {
            // Stretch to exactly fill (no aspect ratio preservation)
            // sourceRect = full image, targetRect = full screen (already set)
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Background render thread

void UpdateMouseAndAnimations(float deltaTime) {
    if (g_clickOnDesktop.exchange(false, std::memory_order_relaxed)) {
        g_isIdle       = false;
        g_lastMouseTime = GetTickCount64();
    }

    POINT screenPos;
    GetCursorPos(&screenPos);

    bool mouseMoved = false;
    {
        static POINT s_lastScreenPos = {0, 0};
        mouseMoved = (screenPos.x != s_lastScreenPos.x || screenPos.y != s_lastScreenPos.y);
        s_lastScreenPos = screenPos;
    }

    POINT localPos = screenPos;
    ScreenToClient(g_overlayWnd, &localPos);

    float lerpFactor = 1.0f;
    if (g_settings.mouseSmoothing > 0) {
        float speed = 30.0f - g_settings.mouseSmoothing;
        lerpFactor  = 1.0f - std::exp(-speed * deltaTime);
    }

    bool overDesktop = g_isMouseOverDesktop.load(std::memory_order_relaxed);
    bool isEditing   = false;
    std::vector<RECT> currentSelectedRects;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        currentSelectedRects = g_selectedRects;
        isEditing            = g_isEditing;
    }

    {
        static bool s_wasOverDesktop = false;
        if (overDesktop && !s_wasOverDesktop) {
            g_smoothedMousePos = {(float)localPos.x, (float)localPos.y};
            g_lastMouseTime    = GetTickCount64();
            g_isIdle           = false;
        } else if (overDesktop && mouseMoved) {
            g_lastMouseTime = GetTickCount64();
            g_isIdle        = false;
        }
        s_wasOverDesktop = overDesktop;
    }

    g_smoothedMousePos.x += ((float)localPos.x - g_smoothedMousePos.x) * lerpFactor;
    g_smoothedMousePos.y += ((float)localPos.y - g_smoothedMousePos.y) * lerpFactor;

    ULONGLONG timeSinceLastMove = GetTickCount64() - g_lastMouseTime;
    if (!overDesktop || (g_settings.idleDelay > 0 && timeSinceLastMove > (ULONGLONG)g_settings.idleDelay))
        g_isIdle = true;

    float targetSpotFade = g_isIdle ? 0.0f : 1.0f;

    if (targetSpotFade > 0.5f) {
        if (g_settings.spotlightFadeInDuration <= 0) {
            g_spotlightFade = 1.0f;
        } else {
            float speed     = 1.0f / (g_settings.spotlightFadeInDuration / 1000.0f);
            g_spotlightFade = (std::min)(g_spotlightFade + speed * deltaTime, 1.0f);
        }
    } else {
        if (g_settings.spotlightFadeOutDuration <= 0) {
            g_spotlightFade = 0.0f;
        } else {
            float speed     = 1.0f / (g_settings.spotlightFadeOutDuration / 1000.0f);
            g_spotlightFade = (std::max)(g_spotlightFade - speed * deltaTime, 0.0f);
        }
    }

    bool selChanged = g_selectionJustChanged.exchange(false, std::memory_order_relaxed);

    // 1. ONLY update the cached geometry if there is an active selection.
    // This preserves the last known position so the mask has something to draw during the fade-out.
    if (!currentSelectedRects.empty()) {
        g_cachedSelectedRects = currentSelectedRects;
        if (selChanged || !g_isIdle) {
            g_lastSelectionTime = GetTickCount64();
        }
    }

    // 2. Calculate target fade based on the REAL current selection state, not the cached geometry
    float targetSelFade = 0.0f;

    if (!currentSelectedRects.empty()) {
        if (isEditing) {
            targetSelFade       = 1.0f;
            g_lastSelectionTime = GetTickCount64();
        } else if (g_settings.selectionTimeout <= 0) {
            targetSelFade = 1.0f;
        } else {
            ULONGLONG timeSinceInteraction = GetTickCount64() - g_lastSelectionTime;
            targetSelFade = (timeSinceInteraction <= (ULONGLONG)g_settings.selectionTimeout) ? 1.0f : 0.0f;
        }
    }

    // 3. Animate the fade in/out
    if (targetSelFade > 0.5f) {
        if (g_settings.selectionFadeInDuration <= 0) {
            g_selectionFade = 1.0f;
        } else {
            float speed     = 1.0f / (g_settings.selectionFadeInDuration / 1000.0f);
            g_selectionFade = (std::min)(g_selectionFade + speed * deltaTime, 1.0f);
        }
    } else {
        if (g_settings.selectionFadeOutDuration <= 0) {
            g_selectionFade = 0.0f;
        } else {
            float speed     = 1.0f / (g_settings.selectionFadeOutDuration / 1000.0f);
            g_selectionFade = (std::max)(g_selectionFade - speed * deltaTime, 0.0f);
        }
    }

    // Master overlay toggle fade (independent of spotlight/selection)
    float targetMasterFade = g_overlayBypassed.load(std::memory_order_relaxed) ? 0.0f : 1.0f;
    if (targetMasterFade > 0.5f) {
        if (g_settings.toggleFadeInDuration <= 0) {
            g_masterFade = 1.0f;
        } else {
            float speed = 1.0f / (g_settings.toggleFadeInDuration / 1000.0f);
            g_masterFade = (std::min)(g_masterFade + speed * deltaTime, 1.0f);
        }
    } else {
        if (g_settings.toggleFadeOutDuration <= 0) {
            g_masterFade = 0.0f;
        } else {
            float speed = 1.0f / (g_settings.toggleFadeOutDuration / 1000.0f);
            g_masterFade = (std::max)(g_masterFade - speed * deltaTime, 0.0f);
        }
    }

    // 4. Clean up the cached geometry ONLY after the fade-out animation is completely finished
    if (currentSelectedRects.empty() && g_selectionFade <= 0.0f && targetSelFade < 0.5f) {
        g_cachedSelectedRects.clear();
    }
}

void RenderOverlay() {
    if (!g_dc || !g_targetBitmap || !g_spotlightMaskBitmap || !g_selectionMaskBitmap) return;

    UINT width  = g_overlayWidth;
    UINT height = g_overlayHeight;
    if (width == 0 || height == 0) return;


    // 1. Spotlight mask
    g_dc->SetTarget(g_spotlightMaskBitmap.Get());
    g_dc->BeginDraw();
    g_dc->Clear(D2D1::ColorF(0, 0, 0, 0));

    if (g_spotlightFade > 0.001f && g_spotlightBrush) {
        g_spotlightBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, g_spotlightFade));
        float radius       = (float)g_settings.spotlightRadius * g_dpiScale;
        float cornerRadius = radius * (g_settings.spotlightRoundness / 100.0f);

        D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(
            D2D1::RectF(g_smoothedMousePos.x - radius, g_smoothedMousePos.y - radius,
                        g_smoothedMousePos.x + radius, g_smoothedMousePos.y + radius),
            cornerRadius, cornerRadius);
        g_dc->FillRoundedRectangle(rr, g_spotlightBrush.Get());
    }
    g_dc->EndDraw();

    // 2. Selection mask
    g_dc->SetTarget(g_selectionMaskBitmap.Get());
    g_dc->BeginDraw();
    g_dc->Clear(D2D1::ColorF(0, 0, 0, 0));

    if (g_selectionFade > 0.001f && !g_cachedSelectedRects.empty() && g_selectionBrush) {
        float activeAlpha = (g_settings.selectionOpacity / 255.0f) * g_selectionFade;
        g_selectionBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, activeAlpha));
        float pad = (float)g_settings.selectionPadding * g_dpiScale;

        for (const auto& rcBounds : g_cachedSelectedRects) {
            float rw = ((rcBounds.right  + pad) - (rcBounds.left - pad)) / 2.0f;
            float rh = ((rcBounds.bottom + pad) - (rcBounds.top  - pad)) / 2.0f;
            float cornerRadius = (std::min)(rw, rh) * (g_settings.selectionRoundness / 100.0f);

            D2D1_ROUNDED_RECT rRect = D2D1::RoundedRect(
                D2D1::RectF((float)rcBounds.left   - pad, (float)rcBounds.top    - pad,
                            (float)rcBounds.right  + pad, (float)rcBounds.bottom + pad),
                cornerRadius, cornerRadius);
            g_dc->FillRoundedRectangle(rRect, g_selectionBrush.Get());
        }
    }

    g_dc->EndDraw();

    // 3. Final composited frame
    g_dc->SetTarget(g_targetBitmap.Get());
    g_dc->BeginDraw();
    
    // HIDE: Clear the target to fully transparent
    g_dc->Clear(D2D1::ColorF(0, 0, 0, 0));

    // PREPARE LAYER: We use a layer to apply the idleOpacity to the 
    // ENTIRE background + wallpaper uniformly. This prevents the alpha 
    // accumulation trap where the wallpaper ends up more opaque than the bg color.
    float idleAlpha = (g_settings.idleOpacity / 255.0f) * g_masterFade;
    D2D1_LAYER_PARAMETERS layerParams;
    layerParams.contentBounds = D2D1::InfiniteRect();
    layerParams.geometricMask = nullptr;
    layerParams.maskAntialiasMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
    layerParams.maskTransform = D2D1::Matrix3x2F::Identity();
    layerParams.opacity = idleAlpha;
    layerParams.opacityBrush = nullptr;
    layerParams.layerOptions = D2D1_LAYER_OPTIONS_NONE;
    
    g_dc->PushLayer(&layerParams, nullptr);

    // FILL: Background color at full opacity
    // (If there's no wallpaper, this is the only thing drawn. If there is, 
    // it provides the correct color for CENTER/FIT letterbox areas.)
    g_dc->Clear(g_desktopBgColor); // g_desktopBgColor already has alpha 1.0f

    // STAMP: Wallpaper image at full opacity
    if (g_hasWallpaper && g_wallpaperBitmap) {
        if (g_wallpaperPosition == DWPOS_TILE && g_tileBrush) {
            g_dc->FillRectangle(D2D1::RectF(0, 0, (float)width, (float)height), g_tileBrush.Get());
        } else if (g_wallpaperPosition == DWPOS_SPAN && g_overlayWnd) {
            float virtW = (float)GetSystemMetrics(SM_CXVIRTUALSCREEN);
            float virtH = (float)GetSystemMetrics(SM_CYVIRTUALSCREEN);

            POINT pt = {0, 0};
            ClientToScreen(g_overlayWnd, &pt);
            float offsetX = (float)(pt.x - GetSystemMetrics(SM_XVIRTUALSCREEN));
            float offsetY = (float)(pt.y - GetSystemMetrics(SM_YVIRTUALSCREEN));

            D2D1_SIZE_F imgSize = g_wallpaperBitmap->GetSize();
            float scale = std::max(virtW / imgSize.width, virtH / imgSize.height);
            float drawW = imgSize.width * scale;
            float drawH = imgSize.height * scale;
            float left = (virtW - drawW) / 2.0f;
            float top  = (virtH - drawH) / 2.0f;

            D2D1_RECT_F targetRect = D2D1::RectF(
                left - offsetX, top - offsetY,
                left - offsetX + drawW, top - offsetY + drawH);
            
            // FULL OPACITY (1.0f)
            g_dc->DrawBitmap(g_wallpaperBitmap.Get(), targetRect, 1.0f,
                             D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        } else {
                    // 1. Get RAW PIXELS for the math (ignores EXIF DPI metadata)
            D2D1_SIZE_U pixelSize = g_wallpaperBitmap->GetPixelSize();
            D2D1_SIZE_F imgSize = D2D1::SizeF((float)pixelSize.width, (float)pixelSize.height);
            D2D1_SIZE_F refSize = D2D1::SizeF((float)width, (float)height);
            D2D1_RECT_F sourceRect, targetRect;

            // Math is now perfectly aligned with Windows' raw-pixel engine
            ComputeWallpaperRects(imgSize, refSize, g_wallpaperPosition, sourceRect, targetRect);
            
            // ... debug logs ...

            // 2. Convert the raw-pixel sourceRect into DIPs for Direct2D
            float dpiX = 96.0f, dpiY = 96.0f;
            g_wallpaperBitmap->GetDpi(&dpiX, &dpiY);
            
            D2D1_RECT_F sourceRectDips = D2D1::RectF(
                sourceRect.left   * (96.0f / dpiX),
                sourceRect.top    * (96.0f / dpiY),
                sourceRect.right  * (96.0f / dpiX),
                sourceRect.bottom * (96.0f / dpiY)
            );

            // 3. Draw using the DIP-adjusted source rect
            g_dc->DrawBitmap(g_wallpaperBitmap.Get(), targetRect, 1.0f,
                            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, sourceRectDips);
        }
    }
    // REMOVED: The old "else { Solid color only }" block. 
    // g_dc->Clear(g_desktopBgColor) handles it natively.

    // UNHIDE / APPLY IDLE: PopLayer composites everything drawn above 
    // onto the target bitmap using the uniform `idleAlpha`.
    g_dc->PopLayer();

    // SHOW: Apply spotlight and selection masks (destination out)
    if (g_masterFade > 0.001f) {
        if (g_spotlightFade > 0.001f && g_spotlightBlurEffect) {
            g_spotlightBlurEffect->SetInput(0, g_spotlightMaskBitmap.Get());
            float blurVal = (std::max)(0.1f, (std::min)((float)g_settings.spotlightBlur * g_dpiScale, 100.0f));
            g_spotlightBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, blurVal);
            g_dc->DrawImage(g_spotlightBlurEffect.Get(), D2D1_INTERPOLATION_MODE_LINEAR,
                            D2D1_COMPOSITE_MODE_DESTINATION_OUT);
        }

        if (g_selectionFade > 0.001f && g_selectionBlurEffect) {
            g_selectionBlurEffect->SetInput(0, g_selectionMaskBitmap.Get());
            float blurVal = (std::max)(0.1f, (std::min)((float)g_settings.selectionBlur * g_dpiScale, 100.0f));
            g_selectionBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, blurVal);
            g_dc->DrawImage(g_selectionBlurEffect.Get(), D2D1_INTERPOLATION_MODE_LINEAR,
                            D2D1_COMPOSITE_MODE_DESTINATION_OUT);
        }
    }

    g_dc->EndDraw();
}

void RenderLoop() {
    LARGE_INTEGER freq, lastTime, currentTime;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&lastTime);

    float s_lastRenderedSpotFade  = -1.0f;
    float s_lastRenderedSelFade   = -1.0f;
    float s_lastRenderedMasterFade = 1.0f;

    D2D1_POINT_2F s_lastRenderedMousePos = {-1.0f, -1.0f};
    std::vector<RECT> s_lastRenderedSelectedRects; // Track geometry changes

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

                // Check if the actual selection rectangles changed
                bool selectionChanged = (g_cachedSelectedRects.size() != s_lastRenderedSelectedRects.size());
                if (!selectionChanged) {
                    for (size_t i = 0; i < g_cachedSelectedRects.size(); ++i) {
                        if (g_cachedSelectedRects[i].left != s_lastRenderedSelectedRects[i].left ||
                            g_cachedSelectedRects[i].top != s_lastRenderedSelectedRects[i].top ||
                            g_cachedSelectedRects[i].right != s_lastRenderedSelectedRects[i].right ||
                            g_cachedSelectedRects[i].bottom != s_lastRenderedSelectedRects[i].bottom) {
                            selectionChanged = true;
                            break;
                        }
                    }
                }

                bool stateChanged =
                    selectionChanged || // Force render if the selected icon moved
                    (std::abs(g_masterFade - s_lastRenderedMasterFade) > 0.001f) ||
                    (std::abs(g_spotlightFade - s_lastRenderedSpotFade) > 0.001f) ||
                    (std::abs(g_selectionFade - s_lastRenderedSelFade) > 0.001f)  ||
                    ((std::abs(g_smoothedMousePos.x - s_lastRenderedMousePos.x) > 0.1f ||
                      std::abs(g_smoothedMousePos.y - s_lastRenderedMousePos.y) > 0.1f) &&
                     (g_spotlightFade > 0.001f || !g_isIdle))                     ||
                    g_forceRender;

                float targetMaster = g_overlayBypassed.load(std::memory_order_relaxed) ? 0.0f : 1.0f;
                isFullyIdle = (g_spotlightFade <= 0.001f &&
                            g_selectionFade <= 0.001f &&
                            g_isIdle &&
                            std::abs(g_masterFade - targetMaster) <= 0.001f);

                if (stateChanged && g_swapChain && g_targetBitmap) {
                    RenderOverlay();

                    s_lastRenderedMasterFade = g_masterFade;
                    s_lastRenderedSpotFade  = g_spotlightFade;
                    s_lastRenderedSelFade   = g_selectionFade;
                    s_lastRenderedMousePos  = g_smoothedMousePos;
                    s_lastRenderedSelectedRects = g_cachedSelectedRects;
                    g_forceRender           = false;
                    currentSwapChain        = g_swapChain;
                    frameRendered           = true;
                }
            }
        }

        if (frameRendered && currentSwapChain) {
            HRESULT hr = currentSwapChain->Present(1, 0);
            if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
                if (g_messageWnd) PostMessage(g_messageWnd, WM_APP_DEVICE_LOST, 0, 0);
            }
        } else {
            DWORD sleepTime = isFullyIdle ? INFINITE : 10;
            WaitForSingleObject(g_renderEvent, sleepTime);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Window procedures

LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Handle dynamic messages before the switch (they can't be case labels)
    if (uMsg == WM_APP_SELECTION_UPDATE) {
        if (!g_unloading) { 
            g_selectionUpdatePending.store(false); 
            UpdateSelectionRects(); 
            }
        return 0;
    }
    if (uMsg == WM_APP_CLEANUP) { DestroyWindow(hWnd); return 0; }

    switch (uMsg) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;

        case WM_TIMER:
            if (!g_unloading) {
                if (wParam == TIMER_ID_STATE_POLL) {
                    RevalidateDesktopWindows();
                }
            }
            return 0;

        case WM_WINDOWPOSCHANGED: {
            const WINDOWPOS* wp = (const WINDOWPOS*)lParam;
            if (!(wp->flags & SWP_NOSIZE) && !g_unloading) {
                std::lock_guard<std::mutex> lock(g_renderMutex);
                g_overlayWidth  = (UINT)wp->cx;
                g_overlayHeight = (UINT)wp->cy;
                
                // FIX: Update DPI scale BEFORE refreshing wallpaper
                g_dpiScale = GetMonitorDpiScale(GetWorkerWMonitor());
                
                ResizeSwapChain(wp->cx, wp->cy);
                RefreshWallpaperAndStyle();
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
            if (!g_unloading && g_messageWnd && !g_isRecreating)
                SetTimer(g_messageWnd, TIMER_ID_RECREATE_OVERLAY, 200, nullptr);
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK MessageWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Handle dynamic messages before the switch
    if (uMsg == WM_APP_DEVICE_LOST) {
        if (!g_unloading && !g_isRecreating)
            SetTimer(hWnd, TIMER_ID_RECREATE_OVERLAY, 100, nullptr);
        return 0;
    }
    if (uMsg == WM_APP_TRIGGER_REFRESH || uMsg == WM_THEMECHANGED || uMsg == WM_SETTINGCHANGE) {
        if (!g_unloading && g_overlayWnd) {
            KillTimer(hWnd, TIMER_ID_WALLPAPER_UPDATE);
            SetTimer(hWnd, TIMER_ID_WALLPAPER_UPDATE, 200, nullptr);
        }
        return 0;
    }
    if (uMsg == WM_APP_CLEANUP) { DestroyWindow(hWnd); return 0; }

    switch (uMsg) {
        case WM_TIMER:
            if (!g_unloading) {
                if (wParam == TIMER_ID_RECREATE_OVERLAY) {
                    KillTimer(hWnd, TIMER_ID_RECREATE_OVERLAY);
                    g_isRecreating = true;
                    if (g_overlayWnd) DestroyWindow(g_overlayWnd);
                    CreateOverlayWindow();
                    g_isRecreating = false;
                } else if (wParam == TIMER_ID_WALLPAPER_UPDATE) {
                    KillTimer(hWnd, TIMER_ID_WALLPAPER_UPDATE);
                    if (g_overlayWnd) {
                        std::lock_guard<std::mutex> lock(g_renderMutex);
                        RefreshWallpaperAndStyle();
                    }
                }
            }
            return 0;

        case WM_DISPLAYCHANGE:
            if (!g_unloading)
                SetTimer(hWnd, TIMER_ID_RECREATE_OVERLAY, 50, nullptr);
            break;

        case WM_DESTROY:
            g_messageWnd = nullptr;
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Setup

bool RegisterOverlayWindowClass() {
    WNDCLASS wc = {};
    wc.lpfnWndProc   = OverlayWndProc;
    wc.hInstance     = GetCurrentModuleHandle();
    wc.lpszClassName = OVERLAY_WINDOW_CLASS;
    if (RegisterClass(&wc) == 0)
        return GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
    return true;
}

void CreateOverlayWindow() {
    if (g_overlayWnd) return;

    auto scheduleRetry = [&]() {
        if (g_messageWnd && !g_unloading)
            SetTimer(g_messageWnd, TIMER_ID_RECREATE_OVERLAY, 500, nullptr);
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

    HMONITOR hMon = MonitorFromWindow(hParent, MONITOR_DEFAULTTONEAREST);
    MONITORINFOEX mi;
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(hMon, &mi);
    
    DEVMODE dm;
    dm.dmSize = sizeof(dm);
    int width, height;
    if (EnumDisplaySettings(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm)) {
        width = dm.dmPelsWidth;
        height = dm.dmPelsHeight;
    } else {
        width = mi.rcMonitor.right - mi.rcMonitor.left;
        height = mi.rcMonitor.bottom - mi.rcMonitor.top;
    }

    int x = mi.rcMonitor.left - mi.rcWork.left;
    int y = mi.rcMonitor.top - mi.rcWork.top;

    g_overlayWnd = CreateWindowEx(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        OVERLAY_WINDOW_CLASS, nullptr, WS_CHILD | WS_VISIBLE, x, y, width, height,
        hParent, nullptr, GetCurrentModuleHandle(), nullptr);

    if (!g_overlayWnd) { scheduleRetry(); return; }


    SetWindowPos(g_overlayWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    {
        std::lock_guard<std::mutex> lock(g_renderMutex);

        g_overlayWidth  = (UINT)width;
        g_overlayHeight = (UINT)height;

        if (CreateSwapChainResources(width, height)) {
            g_isIdle        = true;
            g_lastMouseTime = GetTickCount64();
            g_spotlightFade = 0.0f;
            g_selectionFade = 0.0f;
            g_forceRender   = true;

            POINT pt; GetCursorPos(&pt);
            ScreenToClient(g_overlayWnd, &pt);
            g_smoothedMousePos = {(float)pt.x, (float)pt.y};

            if (!g_renderEvent)
                g_renderEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

            g_threadStop   = false;
            g_renderThread = new std::thread(RenderLoop);

            SetTimer(g_overlayWnd, TIMER_ID_STATE_POLL, 1000, nullptr);
        } else {
            DestroyWindow(g_overlayWnd);
            g_overlayWnd = nullptr;
            scheduleRetry();
            return;
        }
    }

    HWND hLV = GetDesktopListView();
    if (hLV) ApplyDesktopSubclasses(hLV);

    UpdateSelectionRects();
}

bool RegisterMessageWindowClass() {
    WNDCLASS wc = {};
    wc.lpfnWndProc   = MessageWndProc;
    wc.hInstance     = GetCurrentModuleHandle();
    wc.lpszClassName = MESSAGE_WINDOW_CLASS;
    if (RegisterClass(&wc) == 0)
        return GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
    return true;
}

void CreateMessageWindow() {
    if (g_messageWnd) return;
    if (!RegisterMessageWindowClass()) return;

    g_messageWnd = CreateWindowEx(0, MESSAGE_WINDOW_CLASS, nullptr, 0, 0, 0, 0, 0,
                                  nullptr, nullptr, GetCurrentModuleHandle(), nullptr);
}

////////////////////////////////////////////////////////////////////////////////
// Thread-local bootstrap hook (Zero ongoing overhead)
HHOOK g_bootstrapHook = nullptr;

LRESULT CALLBACK BootstrapHookProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION && !g_initialized_desktop.load()) {
        g_initialized_desktop.store(true);
        
        // Unhook immediately to eliminate ALL overhead
        HHOOK hookToUnhook = g_bootstrapHook;
        g_bootstrapHook = nullptr;
        if (hookToUnhook) {
            UnhookWindowsHookEx(hookToUnhook);
        }

        // Safely create windows on the correct UI thread
        CreateMessageWindow();
        if (g_messageWnd) {
            CreateOverlayWindow();
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

void BootstrapThread() {
    while (!g_unloading && !g_initialized_desktop.load()) {
        HWND hLV = GetDesktopListView();
        if (hLV && IsWindow(hLV)) {
            DWORD tid = GetWindowThreadProcessId(hLV, nullptr);
            if (tid != 0) {
                // Install hook ONLY on the Explorer UI thread
                g_bootstrapHook = SetWindowsHookExW(WH_GETMESSAGE, BootstrapHookProc, GetCurrentModuleHandle(), tid);
                if (g_bootstrapHook) {
                    // Wake up the thread's message pump to trigger the hook
                    PostThreadMessageW(tid, WM_NULL, 0, 0);
                    
                    // Wait to see if it succeeds
                    for (int i = 0; i < 10 && !g_initialized_desktop.load(); ++i) {
                        Sleep(500);
                    }
                    if (g_initialized_desktop.load()) break;
                }
            }
        }
        Sleep(2000); // Retry every 2 seconds if desktop isn't ready (e.g. cold boot)
    }
}


////////////////////////////////////////////////////////////////////////////////
// Mod lifecycle

void LoadSettings() {
    Settings s;
    s.idleOpacity              = std::clamp(Wh_GetIntSetting(L"idleOpacity"), 0, 255);
    s.spotlightRadius          = std::max(Wh_GetIntSetting(L"spotlightRadius"), 10);
    s.spotlightRoundness       = std::clamp(Wh_GetIntSetting(L"spotlightRoundness"), 0, 100);
    s.spotlightBlur            = std::max(Wh_GetIntSetting(L"spotlightBlur"), 0);
    s.idleDelay                = std::max(Wh_GetIntSetting(L"idleDelay"), 0);
    s.spotlightFadeInDuration  = std::max(Wh_GetIntSetting(L"spotlightFadeInDuration"), 0);
    s.spotlightFadeOutDuration = std::max(Wh_GetIntSetting(L"spotlightFadeOutDuration"), 0);
    s.mouseSmoothing           = std::clamp(Wh_GetIntSetting(L"mouseSmoothing"), 0, 20);

    s.selectionOpacity         = std::clamp(Wh_GetIntSetting(L"selectionOpacity"), 0, 255);
    s.selectionPadding         = std::max(Wh_GetIntSetting(L"selectionPadding"), 0);
    s.selectionRoundness       = std::clamp(Wh_GetIntSetting(L"selectionRoundness"), 0, 100);
    s.selectionBlur            = std::max(Wh_GetIntSetting(L"selectionBlur"), 0);
    s.selectionTimeout         = std::max(Wh_GetIntSetting(L"selectionTimeout"), 0);
    s.selectionFadeInDuration  = std::max(Wh_GetIntSetting(L"selectionFadeInDuration"), 0);
    s.selectionFadeOutDuration = std::max(Wh_GetIntSetting(L"selectionFadeOutDuration"), 0);

    s.toggleOnDoubleClick    = Wh_GetIntSetting(L"toggleOnDoubleClick") != 0;
    s.toggleFadeInDuration   = std::max(Wh_GetIntSetting(L"toggleFadeInDuration"), 0);
    s.toggleFadeOutDuration  = std::max(Wh_GetIntSetting(L"toggleFadeOutDuration"), 0);

    std::lock_guard<std::mutex> lock(g_renderMutex);
    g_settings    = s;
    g_forceRender = true;
    if (g_renderEvent) SetEvent(g_renderEvent);
}

// Define the Thread Agnostic flag just in case the compiler's older Windows SDK is missing it
#ifndef REG_NOTIFY_THREAD_AGNOSTIC
#define REG_NOTIFY_THREAD_AGNOSTIC 0x10000000
#endif

void RegistryWatcherThread() {
    HKEY hKeyDesktop = nullptr;
    HKEY hKeyColors = nullptr;
    HKEY hKeyWallpapers = nullptr;
    HKEY hKeyDWM = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_NOTIFY, &hKeyDesktop) != ERROR_SUCCESS) hKeyDesktop = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Colors", 0, KEY_NOTIFY, &hKeyColors) != ERROR_SUCCESS) hKeyColors = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Wallpapers", 0, KEY_NOTIFY, &hKeyWallpapers) != ERROR_SUCCESS) hKeyWallpapers = nullptr;
    // Catch modern Windows 11 solid color updates
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM", 0, KEY_NOTIFY, &hKeyDWM) != ERROR_SUCCESS) hKeyDWM = nullptr;

    HANDLE hEvents[5];
    hEvents[0] = g_hRegStopEvent;
    hEvents[1] = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    hEvents[2] = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    hEvents[3] = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    hEvents[4] = CreateEventW(nullptr, FALSE, FALSE, nullptr);

    DWORD notifyFlags = REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_THREAD_AGNOSTIC;

    while (!g_unloading) {
        if (hKeyDesktop) RegNotifyChangeKeyValue(hKeyDesktop, FALSE, notifyFlags, hEvents[1], TRUE);
        if (hKeyColors) RegNotifyChangeKeyValue(hKeyColors, FALSE, notifyFlags, hEvents[2], TRUE);
        if (hKeyWallpapers) RegNotifyChangeKeyValue(hKeyWallpapers, FALSE, notifyFlags, hEvents[3], TRUE);
        if (hKeyDWM) RegNotifyChangeKeyValue(hKeyDWM, FALSE, notifyFlags, hEvents[4], TRUE);

        DWORD res = WaitForMultipleObjects(5, hEvents, FALSE, INFINITE);

        if (res == WAIT_OBJECT_0 || g_unloading) {
            break;
        }
        if (res >= WAIT_OBJECT_0 + 1 && res <= WAIT_OBJECT_0 + 4) {
            if (g_messageWnd) {
                PostMessageW(g_messageWnd, WM_APP_TRIGGER_REFRESH, 0, 0); // CHANGED THIS
            }
            Sleep(50);
        }
    }

    if (hKeyDesktop) RegCloseKey(hKeyDesktop);
    if (hKeyColors) RegCloseKey(hKeyColors);
    if (hKeyWallpapers) RegCloseKey(hKeyWallpapers);
    if (hKeyDWM) RegCloseKey(hKeyDWM);
    for (int i=1; i<5; i++) CloseHandle(hEvents[i]);
}

std::thread* g_bootstrapThread = nullptr;

BOOL Wh_ModInit() {
    WM_APP_CLEANUP = RegisterWindowMessageW(L"TransparentDesktopSpotlight_Cleanup");
    WM_APP_DEVICE_LOST = RegisterWindowMessageW(L"TransparentDesktopSpotlight_DeviceLost");
    WM_APP_TRIGGER_REFRESH = RegisterWindowMessageW(L"TransparentDesktopSpotlight_Refresh");
    WM_APP_SELECTION_UPDATE = RegisterWindowMessageW(L"TransparentDesktopSpotlight_SelUpdate");

    LoadSettings();
    
    // Start the lightweight bootstrap thread
    g_bootstrapThread = new std::thread(BootstrapThread);

    g_hRegStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_regThread = new std::thread(RegistryWatcherThread);

    return TRUE;
}

void Wh_ModUninit() {
    g_unloading = true;

    if (g_bootstrapThread) {
        if (g_bootstrapThread->joinable()) g_bootstrapThread->join();
        delete g_bootstrapThread;
        g_bootstrapThread = nullptr;
    }

    if (g_hRegStopEvent) {
        SetEvent(g_hRegStopEvent);
    }
    if (g_regThread) {
        if (g_regThread->joinable()) g_regThread->join();
        delete g_regThread;
        g_regThread = nullptr;
    }
    if (g_hRegStopEvent) {
        CloseHandle(g_hRegStopEvent);
        g_hRegStopEvent = nullptr;
    }

    RemoveDesktopSubclasses();

    if (g_overlayWnd)  SendMessage(g_overlayWnd,  WM_APP_CLEANUP, 0, 0);
    if (g_messageWnd)  SendMessage(g_messageWnd,  WM_APP_CLEANUP, 0, 0);

    UninitDirectX();
    g_wicFactory.Reset();

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
