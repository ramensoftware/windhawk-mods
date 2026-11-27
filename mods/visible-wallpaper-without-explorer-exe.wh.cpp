// ==WindhawkMod==
// @id              visible-wallpaper-without-explorer-exe
// @name            Visible Wallpaper without explorer.exe
// @description     Restores the behavior of Windows 7 and prior where the wallpaper will still be shown even without explorer.exe
// @version         1.0
// @author          RandomGuy5747
// @github          https://github.com/RandomGuy5747
// @include         windhawk.exe
// @compilerOptions -lgdi32 -lole32 -lshell32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Visible Wallpaper without explorer.exe

This mod restores the behavior of Windows 7 and prior where the wallpaper will still be shown even without explorer.exe. I don't know why somebody needs a mod like this, maybe for transformations or something similar.

You can customize the check interval in the mod settings (default is 100 ms).

Screenshot:

![preview](https://raw.githubusercontent.com/RandomGuy5747/windhawk-preview/refs/heads/main/preview2.PNG)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- detection_interval_ms: 100
  $name: Wallpaper check interval (ms)
  $description: Interval between shell-state checks in milliseconds. You can use any value that is greater than 0. 0 is the fastest supported timer interval.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <objbase.h>
#include <wincodec.h>
#include <string>
#include <cwchar>
#include <algorithm>
#include <atomic>

// ============================================================
// WIC identifiers
// ============================================================

static const GUID kClsidWicImagingFactory =
{
    0xCACAF262, 0x9370, 0x4615,
    {0xA1, 0x3B, 0x9F, 0x55, 0x39, 0xDA, 0x4C, 0x0A}
};

static const GUID kIidWicImagingFactory =
{
    0xEC5EC8A9, 0xC395, 0x4314,
    {0x9C, 0x77, 0x54, 0xD7, 0xA9, 0x35, 0xFF, 0x70}
};

static const GUID kWicPixelFormat32bppBGR =
{
    0x6FDDC324, 0x4E03, 0x4BFE,
    {0xB1, 0x85, 0x3D, 0x77, 0x76, 0x8D, 0xC9, 0x0E}
};

// ============================================================
// Globals
// ============================================================

static HWND g_window = nullptr;
static HWND g_broadcastWindow = nullptr;
static HANDLE g_thread = nullptr;
static HANDLE g_readyEvent = nullptr;
static DWORD g_threadId = 0;
static bool g_threadInitResult = false;

static HBITMAP g_bitmap = nullptr;
static std::wstring g_wallpaperPath;
static FILETIME g_wallpaperLastWriteTime = {};
static DWORD g_wallpaperFileSizeHigh = 0;
static DWORD g_wallpaperFileSizeLow = 0;

static bool g_windowVisible = false;
static RECT g_lastWindowRect = {};

static DWORD g_wallpaperStyle = 10;
static bool g_tileWallpaper = false;

static const UINT_PTR TIMER_UPDATE = 1;
static const UINT WM_RECREATE_WINDOW = WM_APP + 11;
static const UINT WM_REFRESH_WALLPAPER = WM_APP + 12;
static const wchar_t g_windowClassName[] =
    L"WindhawkIndependentDesktopWallpaper";
static std::atomic<bool> g_threadStopping{false};

// Dedicated monitor tick. This timer belongs to the hidden controller window,
// not the wallpaper window, so Explorer cannot destroy or suspend the poller.
// Windows timers have a practical minimum interval of about 10 ms; a setting of
// 0 therefore selects that fastest supported interval rather than a literal 0.
static UINT g_updateInterval = 10;

static UINT GetConfiguredUpdateInterval()
{
    int delay = Wh_GetIntSetting(L"detection_interval_ms");
    delay = std::max(0, std::min(1000, delay));
    return static_cast<UINT>(std::max(10, delay));
}

// Keep the independent wallpaper visible briefly after the shell desktop
// window appears. This bridges the short interval before Explorer paints the
// desktop wallpaper. If the shell was already running when the mod starts,
// no grace period is used.
static const ULONGLONG SHELL_STARTUP_GRACE_PERIOD = 2500;
static bool g_shellStateInitialized = false;
static bool g_shellWasRunning = false;
static bool g_shellStartupGraceActive = false;
static ULONGLONG g_shellStartupDetectedAt = 0;


// ============================================================
// Get wallpaper path
// ============================================================

static bool GetWallpaperPath(
    WCHAR* path,
    DWORD pathCount)
{
    if (!path || pathCount == 0)
        return false;

    path[0] = L'\0';

    // SPI_GETDESKWALLPAPER reports the wallpaper currently selected by
    // Windows. Prefer it over the registry because the registry can briefly
    // contain an older value while the Personalization UI is applying a change.
    if (SystemParametersInfoW(
            SPI_GETDESKWALLPAPER,
            pathCount,
            path,
            0) &&
        path[0] != L'\0')
    {
        path[pathCount - 1] = L'\0';
        return true;
    }

    // Registry fallback for shell configurations where SPI_GETDESKWALLPAPER
    // does not return a usable path.
    HKEY desktopKey = nullptr;

    LONG result = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Control Panel\\Desktop",
        0,
        KEY_QUERY_VALUE,
        &desktopKey);

    if (result != ERROR_SUCCESS)
        return false;

    DWORD type = 0;
    DWORD bytes = pathCount * sizeof(WCHAR);

    result = RegQueryValueExW(
        desktopKey,
        L"Wallpaper",
        nullptr,
        &type,
        reinterpret_cast<BYTE*>(path),
        &bytes);

    RegCloseKey(desktopKey);

    if (result != ERROR_SUCCESS ||
        (type != REG_SZ && type != REG_EXPAND_SZ) ||
        path[0] == L'\0')
    {
        path[0] = L'\0';
        return false;
    }

    path[pathCount - 1] = L'\0';

    if (type == REG_EXPAND_SZ)
    {
        WCHAR expanded[MAX_PATH * 4] = {};

        DWORD length = ExpandEnvironmentStringsW(
            path,
            expanded,
            ARRAYSIZE(expanded));

        if (length > 0 && length <= ARRAYSIZE(expanded))
            wcscpy_s(path, pathCount, expanded);
    }

    return path[0] != L'\0';
}

// ============================================================
// Read wallpaper settings
// ============================================================

static DWORD ReadDesktopDwordString(
    const wchar_t* valueName,
    DWORD defaultValue)
{
    WCHAR buffer[32] = {};
    DWORD type = 0;
    DWORD bytes = sizeof(buffer);

    LONG result = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Control Panel\\Desktop",
        valueName,
        RRF_RT_REG_SZ | RRF_RT_REG_DWORD,
        &type,
        buffer,
        &bytes);

    if (result != ERROR_SUCCESS)
        return defaultValue;

    if (type == REG_DWORD &&
        bytes >= sizeof(DWORD))
    {
        return *reinterpret_cast<DWORD*>(buffer);
    }

    if (type == REG_SZ)
    {
        WCHAR* end = nullptr;

        unsigned long parsed =
            wcstoul(buffer, &end, 10);

        if (end != buffer)
            return static_cast<DWORD>(parsed);
    }

    return defaultValue;
}

static void ReadWallpaperLayoutSettings()
{
    g_wallpaperStyle =
        ReadDesktopDwordString(L"WallpaperStyle", 10);

    g_tileWallpaper =
        ReadDesktopDwordString(L"TileWallpaper", 0) != 0;
}

// ============================================================
// Load wallpaper
// ============================================================

static bool LoadWallpaper(bool forceReload = false)
{
    WCHAR path[MAX_PATH] = {};

    if (!GetWallpaperPath(path, ARRAYSIZE(path)))
        return false;

    DWORD previousStyle = g_wallpaperStyle;
    bool previousTileWallpaper = g_tileWallpaper;

    ReadWallpaperLayoutSettings();

    const bool layoutChanged =
        previousStyle != g_wallpaperStyle ||
        previousTileWallpaper != g_tileWallpaper;

    WIN32_FILE_ATTRIBUTE_DATA fileData = {};

    if (!GetFileAttributesExW(
            path,
            GetFileExInfoStandard,
            &fileData))
    {
        Wh_Log(
            L"Wallpaper file unavailable: %s (%lu)",
            path,
            GetLastError());

        return false;
    }

    std::wstring newPath(path);

    const bool sameFile =
        g_bitmap &&
        newPath == g_wallpaperPath &&
        CompareFileTime(
            &fileData.ftLastWriteTime,
            &g_wallpaperLastWriteTime) == 0 &&
        fileData.nFileSizeHigh == g_wallpaperFileSizeHigh &&
        fileData.nFileSizeLow == g_wallpaperFileSizeLow;

    if (sameFile && !forceReload)
    {
        if (layoutChanged && g_window)
            InvalidateRect(g_window, nullptr, FALSE);

        return true;
    }

    IWICImagingFactory* factory = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* converter = nullptr;

    HBITMAP newBitmap = nullptr;
    bool loaded = false;

    HRESULT hr = CoCreateInstance(
        kClsidWicImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        kIidWicImagingFactory,
        reinterpret_cast<void**>(&factory));

    do
    {
        if (FAILED(hr) || !factory)
            break;

        hr = factory->CreateDecoderFromFilename(
            path,
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnLoad,
            &decoder);

        if (FAILED(hr) || !decoder)
            break;

        hr = decoder->GetFrame(0, &frame);

        if (FAILED(hr) || !frame)
            break;

        hr = factory->CreateFormatConverter(&converter);

        if (FAILED(hr) || !converter)
            break;

        hr = converter->Initialize(
            frame,
            kWicPixelFormat32bppBGR,
            WICBitmapDitherTypeNone,
            nullptr,
            0.0,
            WICBitmapPaletteTypeCustom);

        if (FAILED(hr))
            break;

        UINT width = 0;
        UINT height = 0;

        hr = converter->GetSize(&width, &height);

        if (FAILED(hr) ||
            width == 0 || height == 0 ||
            width > 16384 || height > 16384)
        {
            break;
        }

        BITMAPINFO bitmapInfo = {};

        bitmapInfo.bmiHeader.biSize =
            sizeof(BITMAPINFOHEADER);

        bitmapInfo.bmiHeader.biWidth =
            static_cast<LONG>(width);

        bitmapInfo.bmiHeader.biHeight =
            -static_cast<LONG>(height);

        bitmapInfo.bmiHeader.biPlanes = 1;
        bitmapInfo.bmiHeader.biBitCount = 32;
        bitmapInfo.bmiHeader.biCompression = BI_RGB;

        void* pixelBuffer = nullptr;

        newBitmap = CreateDIBSection(
            nullptr,
            &bitmapInfo,
            DIB_RGB_COLORS,
            &pixelBuffer,
            nullptr,
            0);

        if (!newBitmap || !pixelBuffer)
            break;

        UINT stride = width * 4;
        UINT bufferSize = stride * height;

        hr = converter->CopyPixels(
            nullptr,
            stride,
            bufferSize,
            static_cast<BYTE*>(pixelBuffer));

        if (FAILED(hr))
            break;

        loaded = true;

    } while (false);

    if (converter)
        converter->Release();

    if (frame)
        frame->Release();

    if (decoder)
        decoder->Release();

    if (factory)
        factory->Release();

    if (!loaded)
    {
        if (newBitmap)
            DeleteObject(newBitmap);

        return false;
    }

    HBITMAP oldBitmap = g_bitmap;

    g_bitmap = newBitmap;
    g_wallpaperPath = newPath;

    g_wallpaperLastWriteTime = fileData.ftLastWriteTime;
    g_wallpaperFileSizeHigh = fileData.nFileSizeHigh;
    g_wallpaperFileSizeLow = fileData.nFileSizeLow;

    if (oldBitmap)
        DeleteObject(oldBitmap);

    Wh_Log(L"Wallpaper loaded: %s", path);

    if (g_window)
        InvalidateRect(g_window, nullptr, FALSE);

    return true;
}

// ============================================================
// Shell detection
// ============================================================

static bool IsShellRunning()
{
    // GetShellWindow returns the shell's desktop window for this session.
    // It becomes null when the desktop shell is no longer available.
    return GetShellWindow() != nullptr;
}

// Check the wallpaper's path, size, and timestamp immediately before showing
// the independent window. This catches silent file replacement without
// repeatedly reading and hashing the entire wallpaper in the background.
static void RefreshWallpaperBeforeShowing()
{
    WCHAR path[MAX_PATH] = {};

    if (!GetWallpaperPath(path, ARRAYSIZE(path)))
        return;

    WIN32_FILE_ATTRIBUTE_DATA fileData = {};
    if (!GetFileAttributesExW(
            path,
            GetFileExInfoStandard,
            &fileData))
    {
        return;
    }

    DWORD previousStyle = g_wallpaperStyle;
    bool previousTileWallpaper = g_tileWallpaper;
    ReadWallpaperLayoutSettings();

    const bool layoutChanged =
        previousStyle != g_wallpaperStyle ||
        previousTileWallpaper != g_tileWallpaper;

    const bool fileChanged =
        !g_bitmap ||
        std::wstring(path) != g_wallpaperPath ||
        CompareFileTime(
            &fileData.ftLastWriteTime,
            &g_wallpaperLastWriteTime) != 0 ||
        fileData.nFileSizeHigh != g_wallpaperFileSizeHigh ||
        fileData.nFileSizeLow != g_wallpaperFileSizeLow;

    if (fileChanged)
    {
        LoadWallpaper(true);
    }
    else if (layoutChanged && g_window)
    {
        InvalidateRect(g_window, nullptr, FALSE);
    }
}

// ============================================================
// Update window state
// ============================================================

static void UpdateWindowState()
{
    if (!g_window || !IsWindow(g_window))
        return;

    const ULONGLONG now = GetTickCount64();
    const bool shellRunning = IsShellRunning();

    // On the first check, distinguish an already-running desktop from a new
    // shell start. Only a new start receives the grace period; this avoids
    // showing a duplicate wallpaper when the mod is initialized on an
    // already-running desktop.
    if (!g_shellStateInitialized)
    {
        g_shellStateInitialized = true;
        g_shellWasRunning = shellRunning;

        if (shellRunning)
        {
            g_shellStartupGraceActive = false;
            g_shellStartupDetectedAt = 0;
        }
    }

    if (shellRunning)
    {
        if (!g_shellWasRunning)
        {
            g_shellWasRunning = true;
            g_shellStartupGraceActive = true;
            g_shellStartupDetectedAt = now;

            Wh_Log(
                L"Shell desktop window appeared: keeping independent wallpaper visible for %llu ms.",
                SHELL_STARTUP_GRACE_PERIOD);
        }

        // Do not hide our wallpaper immediately when the shell window appears.
        // Explorer can still need a short time to finish initializing and
        // painting the real desktop wallpaper.
        if (g_shellStartupGraceActive &&
            now - g_shellStartupDetectedAt < SHELL_STARTUP_GRACE_PERIOD)
        {
            // Fall through to the normal show/size logic below.
        }
        else
        {
            g_shellStartupGraceActive = false;

            if (g_windowVisible || IsWindowVisible(g_window))
            {
                ShowWindow(g_window, SW_HIDE);
                g_windowVisible = false;
                Wh_Log(L"Shell startup grace period ended: independent wallpaper hidden.");
            }

            return;
        }
    }
    else
    {
        if (g_shellWasRunning)
            Wh_Log(L"The shell desktop window is no longer available: independent wallpaper remains visible.");

        g_shellWasRunning = false;
        g_shellStartupGraceActive = false;
        g_shellStartupDetectedAt = 0;
    }

    // The shell is unavailable, or it has just started and is still inside the
    // grace period. Keep the independent wallpaper visible during this time.
    // Refresh metadata only when the wallpaper is about to become visible.
    if (!g_windowVisible || !IsWindowVisible(g_window))
        RefreshWallpaperBeforeShowing();

    if (!g_bitmap)
    {
        if (g_windowVisible || IsWindowVisible(g_window))
        {
            ShowWindow(g_window, SW_HIDE);
            g_windowVisible = false;
        }

        return;
    }

    int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    if (width <= 0 || height <= 0)
        return;

    RECT newRect = { x, y, x + width, y + height };
    bool boundsChanged =
        newRect.left != g_lastWindowRect.left ||
        newRect.top != g_lastWindowRect.top ||
        newRect.right != g_lastWindowRect.right ||
        newRect.bottom != g_lastWindowRect.bottom;

    if (!g_windowVisible || boundsChanged || !IsWindowVisible(g_window))
    {
        if (!SetWindowPos(
                g_window,
                HWND_BOTTOM,
                x,
                y,
                width,
                height,
                SWP_NOACTIVATE | SWP_SHOWWINDOW))
        {
            Wh_Log(L"SetWindowPos failed: %lu", GetLastError());
            return;
        }

        g_lastWindowRect = newRect;
        g_windowVisible = true;
        InvalidateRect(g_window, nullptr, FALSE);
        UpdateWindow(g_window);

        if (shellRunning)
        {
            Wh_Log(
                L"Wallpaper kept visible during shell startup (%d x %d).",
                width,
                height);
        }
        else
        {
            Wh_Log(
                L"Shell is unavailable: wallpaper shown (%d x %d).",
                width,
                height);
        }
    }
}

// ============================================================
// Paint wallpaper
// ============================================================

static void PaintWallpaper(HWND hwnd)
{
    PAINTSTRUCT paint = {};

    HDC hdc = BeginPaint(hwnd, &paint);

    if (!hdc)
    {
        EndPaint(hwnd, &paint);
        return;
    }

    RECT clientRect = {};
    GetClientRect(hwnd, &clientRect);

    int destWidth =
        clientRect.right - clientRect.left;

    int destHeight =
        clientRect.bottom - clientRect.top;

    FillRect(
        hdc,
        &clientRect,
        reinterpret_cast<HBRUSH>(
            GetStockObject(BLACK_BRUSH)));

    if (g_bitmap && destWidth > 0 && destHeight > 0)
    {
        HDC memoryDC = CreateCompatibleDC(hdc);

        if (memoryDC)
        {
            HGDIOBJ oldBitmap =
                SelectObject(memoryDC, g_bitmap);

            if (oldBitmap && oldBitmap != HGDI_ERROR)
            {
                BITMAP bitmapInfo = {};

                if (GetObjectW(
                        g_bitmap,
                        sizeof(bitmapInfo),
                        &bitmapInfo))
                {
                    int srcWidth = bitmapInfo.bmWidth;
                    int srcHeight = bitmapInfo.bmHeight;

                    if (srcWidth > 0 && srcHeight > 0)
                    {
                        if (g_tileWallpaper)
                        {
                            for (int y = 0; y < destHeight; y += srcHeight)
                            {
                                for (int x = 0; x < destWidth; x += srcWidth)
                                {
                                    int copyWidth =
                                        std::min(srcWidth, destWidth - x);

                                    int copyHeight =
                                        std::min(srcHeight, destHeight - y);

                                    BitBlt(
                                        hdc,
                                        x,
                                        y,
                                        copyWidth,
                                        copyHeight,
                                        memoryDC,
                                        0,
                                        0,
                                        SRCCOPY);
                                }
                            }
                        }
                        else if (g_wallpaperStyle == 0)
                        {
                            int x = (destWidth - srcWidth) / 2;
                            int y = (destHeight - srcHeight) / 2;

                            BitBlt(
                                hdc,
                                x,
                                y,
                                srcWidth,
                                srcHeight,
                                memoryDC,
                                0,
                                0,
                                SRCCOPY);
                        }
                        else if (g_wallpaperStyle == 2)
                        {
                            SetStretchBltMode(hdc, COLORONCOLOR);

                            StretchBlt(
                                hdc,
                                0, 0,
                                destWidth, destHeight,
                                memoryDC,
                                0, 0,
                                srcWidth, srcHeight,
                                SRCCOPY);
                        }
                        else if (g_wallpaperStyle == 6)
                        {
                            double scale = std::min(
                                static_cast<double>(destWidth) / srcWidth,
                                static_cast<double>(destHeight) / srcHeight);

                            int drawWidth =
                                static_cast<int>(srcWidth * scale);

                            int drawHeight =
                                static_cast<int>(srcHeight * scale);

                            int x = (destWidth - drawWidth) / 2;
                            int y = (destHeight - drawHeight) / 2;

                            SetStretchBltMode(hdc, HALFTONE);
                            SetBrushOrgEx(hdc, 0, 0, nullptr);

                            StretchBlt(
                                hdc,
                                x, y,
                                drawWidth, drawHeight,
                                memoryDC,
                                0, 0,
                                srcWidth, srcHeight,
                                SRCCOPY);
                        }
                        else if (g_wallpaperStyle == 22)
                        {
                            double scale = std::max(
                                static_cast<double>(destWidth) / srcWidth,
                                static_cast<double>(destHeight) / srcHeight);

                            int cropX = static_cast<int>(
                                ((srcWidth * scale - destWidth) / 2) / scale);

                            int cropY = static_cast<int>(
                                ((srcHeight * scale - destHeight) / 2) / scale);

                            int cropWidth =
                                static_cast<int>(destWidth / scale);

                            int cropHeight =
                                static_cast<int>(destHeight / scale);

                            SetStretchBltMode(hdc, HALFTONE);
                            SetBrushOrgEx(hdc, 0, 0, nullptr);

                            StretchBlt(
                                hdc,
                                0, 0,
                                destWidth, destHeight,
                                memoryDC,
                                cropX, cropY,
                                cropWidth, cropHeight,
                                SRCCOPY);
                        }
                        else
                        {
                            double destAspect =
                                static_cast<double>(destWidth) / destHeight;

                            double sourceAspect =
                                static_cast<double>(srcWidth) / srcHeight;

                            int cropX = 0;
                            int cropY = 0;
                            int cropWidth = srcWidth;
                            int cropHeight = srcHeight;

                            if (sourceAspect > destAspect)
                            {
                                cropWidth =
                                    static_cast<int>(srcHeight * destAspect);

                                cropX =
                                    (srcWidth - cropWidth) / 2;
                            }
                            else if (sourceAspect < destAspect)
                            {
                                cropHeight =
                                    static_cast<int>(srcWidth / destAspect);

                                cropY =
                                    (srcHeight - cropHeight) / 2;
                            }

                            SetStretchBltMode(hdc, HALFTONE);
                            SetBrushOrgEx(hdc, 0, 0, nullptr);

                            StretchBlt(
                                hdc,
                                0, 0,
                                destWidth, destHeight,
                                memoryDC,
                                cropX, cropY,
                                cropWidth, cropHeight,
                                SRCCOPY);
                        }
                    }
                }

                SelectObject(memoryDC, oldBitmap);
            }

            DeleteDC(memoryDC);
        }
    }

    EndPaint(hwnd, &paint);
}

// ============================================================
// Window procedure
// ============================================================

static LRESULT CALLBACK WallpaperWndProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
        case WM_PAINT:
            PaintWallpaper(hwnd);
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_NCHITTEST:
            return HTTRANSPARENT;

        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;


        case WM_TIMER:
        {
            if (wParam == TIMER_UPDATE &&
                (hwnd == g_broadcastWindow || hwnd == g_window))
            {
                // GetShellWindow is a lightweight session-specific check.
                UpdateWindowState();
            }

            return 0;
        }

        case WM_SETTINGCHANGE:
            if (hwnd == g_window && wParam == SPI_SETDESKWALLPAPER)
            {
                LoadWallpaper(true);
                UpdateWindowState();
            }
            return 0;

        case WM_DISPLAYCHANGE:
            UpdateWindowState();
            return 0;

        case WM_REFRESH_WALLPAPER:
            // Apply the updated delay on the wallpaper thread, where both
            // timer windows are owned. This avoids cross-thread timer changes.
            g_updateInterval = GetConfiguredUpdateInterval();

            if (g_broadcastWindow)
            {
                KillTimer(g_broadcastWindow, TIMER_UPDATE);
                SetTimer(g_broadcastWindow, TIMER_UPDATE, g_updateInterval, nullptr);
            }

            if (g_window)
            {
                KillTimer(g_window, TIMER_UPDATE);
                if (!g_broadcastWindow)
                    SetTimer(g_window, TIMER_UPDATE, g_updateInterval, nullptr);
            }

            Wh_Log(L"Wallpaper detection interval updated to %u ms.", g_updateInterval);
            LoadWallpaper(true);
            UpdateWindowState();
            return 0;

        case WM_QUERYENDSESSION:
            // Do not block logoff/shutdown. Keep painting until Windows ends
            // this session so the wallpaper can cover the Explorer exit gap.
            return TRUE;

        case WM_ENDSESSION:
            return 0;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            if (hwnd == g_broadcastWindow)
            {
                KillTimer(hwnd, TIMER_UPDATE);
                g_broadcastWindow = nullptr;

                // If the controller is unexpectedly destroyed, move polling
                // to the wallpaper window instead of losing process checks.
                if (g_window && !g_threadStopping.load())
                    SetTimer(g_window, TIMER_UPDATE, g_updateInterval, nullptr);

                return 0;
            }

            KillTimer(hwnd, TIMER_UPDATE);

            // The wallpaper host is always a top-level window, never a child
            // of Explorer. It therefore survives destruction of WorkerW/Progman.
            g_window = nullptr;
            g_windowVisible = false;
            g_lastWindowRect = {};

            if (g_threadStopping.load())
            {
                PostQuitMessage(0);
            }
            else if (!PostThreadMessageW(
                         g_threadId,
                         WM_RECREATE_WINDOW,
                         0,
                         0))
            {
                Wh_Log(
                    L"Could not queue wallpaper window recreation: %lu",
                    GetLastError());
            }

            return 0;
    }

    return DefWindowProcW(
        hwnd,
        message,
        wParam,
        lParam);
}

// ============================================================
// Create/recreate the wallpaper host window
// ============================================================

static bool CreateWallpaperHostWindow(HINSTANCE instance)
{
    g_window = CreateWindowExW(
        WS_EX_TOOLWINDOW |
        WS_EX_NOACTIVATE |
        WS_EX_TRANSPARENT,
        g_windowClassName,
        L"Explorer-Independent Desktop Wallpaper",
        WS_POPUP,
        0, 0, 1, 1,
        nullptr,
        nullptr,
        instance,
        nullptr);

    if (!g_window)
    {
        Wh_Log(
            L"CreateWindowExW failed: %lu",
            GetLastError());

        return false;
    }

    if (!g_broadcastWindow &&
        !SetTimer(g_window, TIMER_UPDATE, g_updateInterval, nullptr))
    {
        Wh_Log(L"Fallback process-monitor timer failed: %lu", GetLastError());
    }

    return true;
}

// A separate hidden top-level controller owns the lightweight shell-state
// timer. It is never parented to Explorer and therefore survives Explorer's
// desktop-window teardown during the Windows 7 logoff sequence.
static bool CreateBroadcastListenerWindow(HINSTANCE instance)
{
    g_broadcastWindow = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        g_windowClassName,
        L"Explorer-Independent Wallpaper Notifications",
        WS_POPUP,
        0, 0, 1, 1,
        nullptr,
        nullptr,
        instance,
        nullptr);

    if (!g_broadcastWindow)
    {
        Wh_Log(
            L"Could not create wallpaper notification listener: %lu",
            GetLastError());
        return false;
    }

    // The controller stays hidden. Its configurable timer checks the current
    // session's shell state regardless of wallpaper-window visibility.
    if (!SetTimer(g_broadcastWindow, TIMER_UPDATE, g_updateInterval, nullptr))
    {
        Wh_Log(L"Could not start independent wallpaper process monitor: %lu", GetLastError());
        DestroyWindow(g_broadcastWindow);
        g_broadcastWindow = nullptr;
        return false;
    }

    return true;
}

// ============================================================
// Wallpaper UI thread
// ============================================================

static DWORD WINAPI WallpaperThread(LPVOID)
{
    g_threadId = GetCurrentThreadId();

    HRESULT comResult = CoInitializeEx(
        nullptr,
        COINIT_APARTMENTTHREADED);

    if (FAILED(comResult))
    {
        Wh_Log(
            L"CoInitializeEx failed: 0x%08X",
            comResult);

        SetEvent(g_readyEvent);
        return 1;
    }

    HINSTANCE instance = GetModuleHandleW(nullptr);

    WNDCLASSEXW windowClass = {};

    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = WallpaperWndProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = g_windowClassName;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);

    if (!RegisterClassExW(&windowClass))
    {
        Wh_Log(
            L"RegisterClassExW failed: %lu",
            GetLastError());

        CoUninitialize();
        SetEvent(g_readyEvent);
        return 1;
    }

    if (!CreateBroadcastListenerWindow(instance))
        Wh_Log(L"Wallpaper notifications will use periodic polling only.");

    if (!CreateWallpaperHostWindow(instance))
    {
        if (g_broadcastWindow)
        {
            DestroyWindow(g_broadcastWindow);
            g_broadcastWindow = nullptr;
        }

        UnregisterClassW(g_windowClassName, instance);
        CoUninitialize();

        SetEvent(g_readyEvent);
        return 1;
    }

    LoadWallpaper(true);

    MSG message = {};

    PeekMessageW(
        &message,
        nullptr,
        WM_USER,
        WM_USER,
        PM_NOREMOVE);

    g_threadInitResult = true;
    SetEvent(g_readyEvent);

    UpdateWindowState();

    Wh_Log(L"Wallpaper host started.");

    while (true)
    {
        BOOL result = GetMessageW(
            &message,
            nullptr,
            0,
            0);

        if (result <= 0)
            break;

        if (message.message == WM_RECREATE_WINDOW)
        {
            if (!g_threadStopping.load() && !g_window)
            {
                if (CreateWallpaperHostWindow(instance))
                {
                    LoadWallpaper(true);
                    UpdateWindowState();

                    Wh_Log(L"Wallpaper host window recreated.");
                }
                else
                {
                    // A desktop parent can be in the middle of disappearing.
                    // Retry after a short pause rather than terminating the mod.
                    Sleep(500);

                    PostThreadMessageW(
                        g_threadId,
                        WM_RECREATE_WINDOW,
                        0,
                        0);
                }
            }

            continue;
        }

        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    if (g_window)
    {
        DestroyWindow(g_window);
        g_window = nullptr;
    }

    if (g_broadcastWindow)
    {
        DestroyWindow(g_broadcastWindow);
        g_broadcastWindow = nullptr;
    }

    if (g_bitmap)
    {
        DeleteObject(g_bitmap);
        g_bitmap = nullptr;
    }

    g_wallpaperPath.clear();

    g_threadId = 0;
    g_windowVisible = false;
    g_lastWindowRect = {};
    g_shellStateInitialized = false;
    g_shellWasRunning = false;
    g_shellStartupGraceActive = false;
    g_shellStartupDetectedAt = 0;

    UnregisterClassW(g_windowClassName, instance);

    CoUninitialize();

    Wh_Log(L"Wallpaper host stopped.");

    return 0;
}

// ============================================================
// Windhawk tool callbacks
// ============================================================

BOOL WhTool_ModInit()
{
    g_updateInterval = GetConfiguredUpdateInterval();
    Wh_Log(L"Wallpaper detection interval: %u ms.", g_updateInterval);

    g_threadStopping.store(false);

    g_readyEvent = CreateEventW(
        nullptr,
        TRUE,
        FALSE,
        nullptr);

    if (!g_readyEvent)
    {
        Wh_Log(
            L"CreateEventW failed: %lu",
            GetLastError());

        return FALSE;
    }

    g_threadInitResult = false;

    g_thread = CreateThread(
        nullptr,
        0,
        WallpaperThread,
        nullptr,
        0,
        nullptr);

    if (!g_thread)
    {
        Wh_Log(
            L"CreateThread failed: %lu",
            GetLastError());

        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;

        return FALSE;
    }

    WaitForSingleObject(g_readyEvent, INFINITE);

    CloseHandle(g_readyEvent);
    g_readyEvent = nullptr;

    if (!g_threadInitResult)
    {
        WaitForSingleObject(g_thread, INFINITE);

        CloseHandle(g_thread);
        g_thread = nullptr;

        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged()
{
    if (g_window)
    {
        PostMessageW(
            g_window,
            WM_REFRESH_WALLPAPER,
            0,
            0);
    }
}

void WhTool_ModUninit()
{
    g_threadStopping.store(true);

    if (g_thread)
    {
        if (g_threadId)
        {
            if (!PostThreadMessageW(
                    g_threadId,
                    WM_QUIT,
                    0,
                    0))
            {
                Wh_Log(
                    L"PostThreadMessageW failed: %lu",
                    GetLastError());
            }
        }

        WaitForSingleObject(g_thread, INFINITE);

        CloseHandle(g_thread);
        g_thread = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }
    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);
    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);
    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }
    WhTool_ModUninit();
    ExitProcess(0);
}
