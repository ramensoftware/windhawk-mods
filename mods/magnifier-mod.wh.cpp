// ==WindhawkMod==
// @id              magnifier-mod
// @name            Taskbar Magnifier Mod
// @description     Adds a magnifier window you can customize and dock at the top, bottom, left, or right of your screen.
// @version         0.5.1
// @author          00face
// @github          https://github.com/00face
// @homepage        https://hyaenahyaena.com
// @include         explorer.exe
// @compilerOptions -lgdi32 -lcomdlg32 -luxtheme -lgdiplus
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Magnifier Mod
This mod adds a magnifier window you can customize and dock at the top, bottom, left, or right of your screen and more.

## Features
- **Zoom Level**: Adjust the zoom level of the magnifier window.
- **Idle Update Interval**: Set the update interval when the cursor is idle.
- **Magnifier Height**: Customize the height of the magnifier window.
- **Magnifier Width**: Customize the width of the magnifier window.
- **Position**: Position the magnifier at the top, bottom, left, or right of the screen.
- **Use Default Height**: Use the default height (same as the taskbar height).
- **Use Default Width**: Use the default width (same as the taskbar width).
- **Padding**: Add padding at the top, right, bottom, and left of the magnifier.
- **Opacity**: Adjust the opacity level of the magnifier window.

# Changelog

## [0.5.1] - 2025-19-01
### Added
- Added support for vertical positions (left and right).
- Removed click-through feature.
- Removed border rounding feature.
- Updated metadata and settings to reflect changes.
- Updated readme with features and more information.

## [0.4.2] - 2025-16-01
### Added
- Initial release of the Taskbar Magnifier Mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- zoomLevel: 200
  $name: Zoom Level
  $description: The zoom level for the magnifier window (in percentage).
- idleUpdateInterval: 500
  $name: Idle Update Interval
  $description: The update interval when the cursor is idle (in milliseconds).
- magnifierHeight: 100
  $name: Magnifier Height
  $description: The height of the magnifier window.
- magnifierWidth: 100
  $name: Magnifier Width
  $description: The width of the magnifier window.
- position: bottom
  $name: Position
  $description: Position the magnifier at the top, bottom, left, or right of the screen.
- useDefaultHeight: true
  $name: Use Default Height
  $description: Use the default height (same as the taskbar height).
- useDefaultWidth: true
  $name: Use Default Width
  $description: Use the default width (same as the taskbar width).
- paddingTop: 0
  $name: Padding Top
  $description: Padding at the top of the magnifier.
- paddingRight: 0
  $name: Padding Right
  $description: Padding at the right of the magnifier.
- paddingBottom: 0
  $name: Padding Bottom
  $description: Padding at the bottom of the magnifier.
- paddingLeft: 0
  $name: Padding Left
  $description: Padding at the left of the magnifier.
- opacity: 128
  $name: Opacity
  $description: The opacity level of the magnifier window (0-255).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_api.h>
#include <shellapi.h>
#include <uxtheme.h>
#include <algorithm>
#include <thread>
#include <atomic>
#include <chrono>
#include <gdiplus.h>
#include <string>
#include <locale>
#include <codecvt>

#define WC_MAGNIFIER L"Magnifier"

typedef struct tagMAGRANGEINFO {
    float min;
    float max;
} MAGRANGEINFO;

typedef struct {
    DWORD cbSize;
    DWORD width;
    DWORD height;
} MAGIMAGEHEADER;

typedef struct {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} MAGRECTANGLE;

typedef BOOL (WINAPI *PFNMAGINIT)(void);
typedef BOOL (WINAPI *PFNMAGUNINIT)(void);
typedef BOOL (WINAPI *PFNMAGSETWINDOWSOURCE)(HWND hwnd, MAGRECTANGLE rect);
typedef BOOL (WINAPI *PFNMAGSETWINDOWTRANSFORM)(HWND hwnd, float *matrix);
typedef BOOL (WINAPI *PFNMAGSETWINDOWFILTERLIST)(HWND hwnd, DWORD dwFilterMode, int count, HWND *pHWND);

PFNMAGINIT pfnMagInit;
PFNMAGUNINIT pfnMagUninit;
PFNMAGSETWINDOWSOURCE pfnMagSetWindowSource;
PFNMAGSETWINDOWTRANSFORM pfnMagSetWindowTransform;
PFNMAGSETWINDOWFILTERLIST pfnMagSetWindowFilterList;

// Global variable for GDI+ token
ULONG_PTR gdiplusToken;

struct {
    float zoomLevel;
    int idleUpdateInterval;
    int magnifierHeight;
    int magnifierWidth;
    std::string position;
    bool useDefaultHeight;
    bool useDefaultWidth;
    int paddingTop;
    int paddingRight;
    int paddingBottom;
    int paddingLeft;
    int opacity;
    HWND hwndMagnifier;
    HWND hwndHost;
    HMODULE hMagnification;
    BOOL isInitialized;
    std::atomic<BOOL> isUpdating;
    std::thread magnifierThread;
    POINT prevCursorPos;
    DWORD lastUpdateTime;
    DWORD updateCount;
    int frameCount;
    DWORD startTime;
    bool isEnabled;
} settings = {0};

void UpdateMagnifierPosition();
void MagnifierThreadFunc();

void LoadSettings() {
    settings.zoomLevel = static_cast<float>(Wh_GetIntSetting(L"zoomLevel")) / 100.0f;
    settings.idleUpdateInterval = Wh_GetIntSetting(L"idleUpdateInterval");
    settings.magnifierHeight = Wh_GetIntSetting(L"magnifierHeight");
    settings.magnifierWidth = Wh_GetIntSetting(L"magnifierWidth");

    // Convert wide string to narrow string
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    settings.position = converter.to_bytes(Wh_GetStringSetting(L"position"));

    settings.useDefaultHeight = Wh_GetIntSetting(L"useDefaultHeight") != 0;
    settings.useDefaultWidth = Wh_GetIntSetting(L"useDefaultWidth") != 0;
    settings.paddingTop = Wh_GetIntSetting(L"paddingTop");
    settings.paddingRight = Wh_GetIntSetting(L"paddingRight");
    settings.paddingBottom = Wh_GetIntSetting(L"paddingBottom");
    settings.paddingLeft = Wh_GetIntSetting(L"paddingLeft");
    settings.opacity = Wh_GetIntSetting(L"opacity");
    Wh_Log(L"Settings loaded");
}

BOOL LoadMagnificationAPI() {
    settings.hMagnification = LoadLibrary(L"Magnification.dll");
    if (!settings.hMagnification) {
        Wh_Log(L"Failed to load Magnification.dll");
        return FALSE;
    }

    pfnMagInit = (PFNMAGINIT)GetProcAddress(settings.hMagnification, "MagInitialize");
    pfnMagUninit = (PFNMAGUNINIT)GetProcAddress(settings.hMagnification, "MagUninitialize");
    pfnMagSetWindowSource = (PFNMAGSETWINDOWSOURCE)GetProcAddress(settings.hMagnification, "MagSetWindowSource");
    pfnMagSetWindowTransform = (PFNMAGSETWINDOWTRANSFORM)GetProcAddress(settings.hMagnification, "MagSetWindowTransform");
    pfnMagSetWindowFilterList = (PFNMAGSETWINDOWFILTERLIST)GetProcAddress(settings.hMagnification, "MagSetWindowFilterList");

    if (!pfnMagInit || !pfnMagUninit || !pfnMagSetWindowSource || !pfnMagSetWindowTransform) {
        Wh_Log(L"Failed to get function addresses from Magnification.dll");
        FreeLibrary(settings.hMagnification);
        return FALSE;
    }

    return TRUE;
}

LRESULT CALLBACK HostWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            if (hdc) {
                RECT rect;
                GetClientRect(hwnd, &rect);

                Gdiplus::Graphics graphics(hdc);
                Gdiplus::Rect gdiRect(0, 0, rect.right - rect.left, rect.bottom - rect.top);
                Gdiplus::SolidBrush brush(Gdiplus::Color(settings.opacity, 255, 255, 255));
                graphics.FillRectangle(&brush, gdiRect);

                EndPaint(hwnd, &ps);
            }
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_DISPLAYCHANGE:
            if (settings.isInitialized) {
                UpdateMagnifierPosition();
            }
            return 0;
        case WM_KEYDOWN:
            if (wParam == VK_F5) {
                UpdateMagnifierPosition();
            }
            break;
        case WM_MOUSEMOVE:
            if (settings.isEnabled) {
                settings.isUpdating = TRUE;
                POINT currCursorPos;
                if (GetCursorPos(&currCursorPos)) {
                    if (currCursorPos.x != settings.prevCursorPos.x ||
                        currCursorPos.y != settings.prevCursorPos.y) {

                        settings.prevCursorPos = currCursorPos;

                        int sourceWidth = 400;
                        int sourceHeight = 100;

                        MAGRECTANGLE sourceRect = {
                            static_cast<LONG>(currCursorPos.x - (sourceWidth / 2)),
                            static_cast<LONG>(currCursorPos.y - (sourceHeight / 2)),
                            static_cast<LONG>(currCursorPos.x + (sourceWidth / 2)),
                            static_cast<LONG>(currCursorPos.y + (sourceHeight / 2))
                        };

                        RECT screenRect;
                        SystemParametersInfo(SPI_GETWORKAREA, 0, &screenRect, 0);
                        sourceRect.left = std::max(screenRect.left, sourceRect.left);
                        sourceRect.top = std::max(screenRect.top, sourceRect.top);
                        sourceRect.right = std::min(screenRect.right, sourceRect.right);
                        sourceRect.bottom = std::min(screenRect.bottom, sourceRect.bottom);

                        if (pfnMagSetWindowSource(settings.hwndMagnifier, sourceRect)) {
                            float matrix[9] = {
                                settings.zoomLevel, 0, 0,
                                0, settings.zoomLevel, 0,
                                0, 0, 1
                            };
                            pfnMagSetWindowTransform(settings.hwndMagnifier, matrix);

                            InvalidateRect(settings.hwndMagnifier, NULL, FALSE);
                            UpdateWindow(settings.hwndMagnifier);
                        }

                        settings.isUpdating = FALSE;
                    }
                }
            }
            break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

HWND CreateMagnifierHost() {
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = HostWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"MagnifierHostClass_Unique";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);

    if (!RegisterClassEx(&wc)) {
        return NULL;
    }

    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
        wc.lpszClassName,
        L"Magnifier Host",
        WS_POPUP | WS_VISIBLE,
        0, 0, 0, 0,
        NULL, NULL,
        wc.hInstance,
        NULL
    );

    if (hwnd) {
        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    }

    return hwnd;
}

HWND CreateMagnifierWindow(HWND hwndHost) {
    HWND hwnd = CreateWindowEx(
        WS_EX_LAYERED, // Add WS_EX_LAYERED style
        WC_MAGNIFIER,
        L"MagnifierWindow",
        WS_CHILD | WS_VISIBLE,
        0, 0, 0, 0,
        hwndHost,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    if (hwnd) {
        SetWindowTheme(hwnd, L"", L"");
        // Set the initial opacity
        SetLayeredWindowAttributes(hwnd, 0, settings.opacity, LWA_ALPHA);
    }

    return hwnd;
}

void UpdateMagnifierPosition() {
    if (!settings.hwndHost || !settings.hwndMagnifier || settings.isUpdating) {
        return;
    }

    HWND hwndTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    if (!hwndTaskbar) {
        return;
    }

    APPBARDATA appBarData = {0};
    appBarData.cbSize = sizeof(appBarData);
    RECT rectTaskbar;
    if (SHAppBarMessage(ABM_GETTASKBARPOS, &appBarData)) {
        rectTaskbar = appBarData.rc;
    } else if (!GetWindowRect(hwndTaskbar, &rectTaskbar)) {
        return;
    }

    int magnifierWidth = settings.useDefaultWidth ? (rectTaskbar.right - rectTaskbar.left) : settings.magnifierWidth;
    int magnifierHeight = settings.useDefaultHeight ? (rectTaskbar.bottom - rectTaskbar.top) : settings.magnifierHeight;
    int x = 0;
    int y = 0;

    if (settings.position == "top") {
        y = settings.paddingTop;
        magnifierWidth = GetSystemMetrics(SM_CXSCREEN);
    } else if (settings.position == "bottom") {
        y = rectTaskbar.top - magnifierHeight - settings.paddingBottom;
        magnifierWidth = GetSystemMetrics(SM_CXSCREEN);
    } else if (settings.position == "left") {
        x = settings.paddingLeft;
        y = settings.paddingTop;
        magnifierWidth = settings.useDefaultWidth ? (rectTaskbar.bottom - rectTaskbar.top) : settings.magnifierWidth;
        magnifierHeight = GetSystemMetrics(SM_CYSCREEN);
    } else if (settings.position == "right") {
        x = GetSystemMetrics(SM_CXSCREEN) - (settings.useDefaultWidth ? (rectTaskbar.bottom - rectTaskbar.top) : settings.magnifierWidth) - settings.paddingRight;
        y = settings.paddingTop;
        magnifierWidth = settings.useDefaultWidth ? (rectTaskbar.bottom - rectTaskbar.top) : settings.magnifierWidth;
        magnifierHeight = GetSystemMetrics(SM_CYSCREEN);
    }

    if (!SetWindowPos(settings.hwndHost, HWND_TOPMOST, x, y, magnifierWidth, magnifierHeight,
                      SWP_NOACTIVATE | SWP_SHOWWINDOW)) {
        return;
    }

    if (!SetWindowPos(settings.hwndMagnifier, NULL, 0, 0, magnifierWidth, magnifierHeight,
                      SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW)) {
        return;
    }

    // Update the opacity of the magnifier window
    SetLayeredWindowAttributes(settings.hwndMagnifier, 0, settings.opacity, LWA_ALPHA);

    POINT ptCursor;
    if (!GetCursorPos(&ptCursor)) {
        return;
    }

    int sourceWidth = 400;
    int sourceHeight = 100;

    MAGRECTANGLE sourceRect = {
        static_cast<LONG>(ptCursor.x - (sourceWidth / 2)),
        static_cast<LONG>(ptCursor.y - (sourceHeight / 2)),
        static_cast<LONG>(ptCursor.x + (sourceWidth / 2)),
        static_cast<LONG>(ptCursor.y + (sourceHeight / 2))
    };

    RECT screenRect;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &screenRect, 0);
    sourceRect.left = std::max(screenRect.left, sourceRect.left);
    sourceRect.top = std::max(screenRect.top, sourceRect.top);
    sourceRect.right = std::min(screenRect.right, sourceRect.right);
    sourceRect.bottom = std::min(screenRect.bottom, sourceRect.bottom);

    if (pfnMagSetWindowSource(settings.hwndMagnifier, sourceRect)) {
        float matrix[9] = {
            settings.zoomLevel, 0, 0,
            0, settings.zoomLevel, 0,
            0, 0, 1
        };
        pfnMagSetWindowTransform(settings.hwndMagnifier, matrix);

        InvalidateRect(settings.hwndMagnifier, NULL, FALSE);
        UpdateWindow(settings.hwndMagnifier);
    }
}

void MagnifierThreadFunc() {
    if (!LoadMagnificationAPI()) {
        return;
    }

    if (!pfnMagInit()) {
        Wh_Log(L"MagInitialize failed");
        return;
    }

    LoadSettings();

    settings.hwndHost = CreateMagnifierHost();
    if (!settings.hwndHost) {
        return;
    }

    settings.hwndMagnifier = CreateMagnifierWindow(settings.hwndHost);
    if (!settings.hwndMagnifier) {
        DestroyWindow(settings.hwndHost);
        return;
    }

    settings.isInitialized = TRUE;
    settings.lastUpdateTime = GetTickCount();
    settings.updateCount = 0;
    settings.isEnabled = true;

    UpdateMagnifierPosition();

    while (settings.isInitialized) {
        std::this_thread::sleep_for(std::chrono::milliseconds(8));

        if (!settings.isEnabled) {
            continue;
        }

        // Force update every idleUpdateInterval milliseconds even if the cursor hasn't moved
        static auto lastForceUpdateTime = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastForceUpdateTime).count() >= settings.idleUpdateInterval) {
            lastForceUpdateTime = now;
            UpdateMagnifierPosition();
        }
    }

    if (settings.hwndMagnifier) {
        DestroyWindow(settings.hwndMagnifier);
    }

    if (settings.hwndHost) {
        DestroyWindow(settings.hwndHost);
    }

    if (settings.isInitialized && pfnMagUninit) {
        pfnMagUninit();
    }

    if (settings.hMagnification) {
        FreeLibrary(settings.hMagnification);
    }

    UnregisterClass(L"MagnifierHostClass_Unique", GetModuleHandle(NULL));

    // Reset individual fields of the settings structure
    settings.zoomLevel = 0;
    settings.idleUpdateInterval = 0;
    settings.magnifierHeight = 0;
    settings.magnifierWidth = 0;
    settings.position = "";
    settings.useDefaultHeight = false;
    settings.useDefaultWidth = false;
    settings.paddingTop = 0;
    settings.paddingRight = 0;
    settings.paddingBottom = 0;
    settings.paddingLeft = 0;
    settings.opacity = 0;
    settings.hwndMagnifier = NULL;
    settings.hwndHost = NULL;
    settings.hMagnification = NULL;
    settings.isInitialized = FALSE;
    settings.isUpdating = FALSE;
    settings.prevCursorPos = {0, 0};
    settings.lastUpdateTime = 0;
    settings.updateCount = 0;
    settings.frameCount = 0;
    settings.startTime = 0;
    settings.isEnabled = false;
}

BOOL Wh_ModInit() {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    settings.magnifierThread = std::thread(MagnifierThreadFunc);
    return TRUE;
}

void Wh_ModUninit() {
    settings.isInitialized = FALSE;
    if (settings.magnifierThread.joinable()) {
        settings.magnifierThread.join();
    }
    Gdiplus::GdiplusShutdown(gdiplusToken);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (settings.isInitialized) {
        UpdateMagnifierPosition();
        // Update the opacity of the magnifier window
        if (settings.hwndMagnifier) {
            SetLayeredWindowAttributes(settings.hwndMagnifier, 0, settings.opacity, LWA_ALPHA);
        }
    }
}
