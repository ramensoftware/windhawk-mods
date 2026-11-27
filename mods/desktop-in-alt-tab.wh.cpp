// ==WindhawkMod==
// @id              desktop-in-alt-tab
// @name            Desktop in Alt+Tab
// @description     Adds a "Desktop" entry to Alt+Tab (with wallpaper thumbnail) that un-maximizes and minimizes all windows at once
// @version         1.4
// @author          K3M
// @github          https://github.com/yazanhani900-blip
// @include         explorer.exe
// @compilerOptions -luser32 -lshell32 -lgdi32 -ldwmapi -lole32 -luuid
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Desktop in Alt+Tab

This mod customizes Desktop behavior in Alt+Tab.

## Features
- Customize Desktop behavior in Alt+Tab.
*/
// ==/WindhawkModReadme==
#include <windows.h>
#include <wchar.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <wincodec.h>

static HWND g_hwnd = nullptr;
static HANDLE g_thread = nullptr;
static DWORD g_lastFire = 0;
static HBITMAP g_wall = nullptr;      // wallpaper at screen size (window thread only)
static int g_wallW = 0, g_wallH = 0;
static bool g_animSaved = false;
static int g_oldMinAnim = 1;
static const wchar_t kClass[] = L"WH_DesktopAltTabWnd";

#define WM_DWMSENDICONICTHUMBNAIL         0x0323
#define WM_DWMSENDICONICLIVEPREVIEWBITMAP 0x0326
#define TIMER_RESTORE_ANIM 1

// ---------- Wallpaper ----------

static HBITMAP LoadWallpaperBitmap(int sw, int sh) {
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = sw;
    bi.bmiHeader.biHeight = -sh;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bmp || !bits) return bmp;

    // Fallback: solid desktop color
    DWORD c = GetSysColor(COLOR_DESKTOP);
    DWORD px = 0xFF000000 | (GetRValue(c) << 16) | (GetGValue(c) << 8) | GetBValue(c);
    DWORD* p = (DWORD*)bits;
    for (int i = 0; i < sw * sh; i++) p[i] = px;

    wchar_t path[MAX_PATH] = {};
    SystemParametersInfoW(SPI_GETDESKWALLPAPER, MAX_PATH, path, 0);
    if (!path[0]) return bmp;

    IWICImagingFactory* f = nullptr;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                IID_IWICImagingFactory, (void**)&f)) || !f)
        return bmp;

    IWICBitmapDecoder* dec = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* conv = nullptr;
    IWICBitmapClipper* clip = nullptr;
    IWICBitmapScaler* scaler = nullptr;

    do {
        if (FAILED(f->CreateDecoderFromFilename(path, nullptr, GENERIC_READ,
                                                WICDecodeMetadataCacheOnDemand, &dec))) break;
        if (FAILED(dec->GetFrame(0, &frame))) break;
        UINT w = 0, h = 0;
        frame->GetSize(&w, &h);
        if (!w || !h) break;

        if (FAILED(f->CreateFormatConverter(&conv))) break;
        if (FAILED(conv->Initialize(frame, GUID_WICPixelFormat32bppPBGRA,
                                    WICBitmapDitherTypeNone, nullptr, 0,
                                    WICBitmapPaletteTypeCustom))) break;

        // "Fill" crop: cover the screen aspect ratio, centered
        double sa = (double)sw / sh, ia = (double)w / h;
        UINT cw, ch;
        if (ia > sa) { ch = h; cw = (UINT)(h * sa); }
        else         { cw = w; ch = (UINT)(w / sa); }
        WICRect rc = { (INT)((w - cw) / 2), (INT)((h - ch) / 2), (INT)cw, (INT)ch };

        if (FAILED(f->CreateBitmapClipper(&clip))) break;
        if (FAILED(clip->Initialize(conv, &rc))) break;
        if (FAILED(f->CreateBitmapScaler(&scaler))) break;
        if (FAILED(scaler->Initialize(clip, sw, sh, WICBitmapInterpolationModeFant))) break;
        scaler->CopyPixels(nullptr, sw * 4, sw * sh * 4, (BYTE*)bits);
    } while (false);

    if (scaler) scaler->Release();
    if (clip) clip->Release();
    if (conv) conv->Release();
    if (frame) frame->Release();
    if (dec) dec->Release();
    f->Release();
    return bmp;
}

static void ReloadWallpaper() {
    if (g_wall) { DeleteObject(g_wall); g_wall = nullptr; }
    g_wallW = GetSystemMetrics(SM_CXSCREEN);
    g_wallH = GetSystemMetrics(SM_CYSCREEN);
    g_wall = LoadWallpaperBitmap(g_wallW, g_wallH);
}

static void SendThumbnail(HWND hwnd, int maxW, int maxH) {
    if (!g_wall || maxW <= 0 || maxH <= 0) return;

    double aspect = (double)g_wallW / g_wallH;
    int w, h;
    if ((double)maxW / maxH > aspect) { h = maxH; w = (int)(h * aspect); }
    else                              { w = maxW; h = (int)(w / aspect); }
    if (w < 1) w = 1;
    if (h < 1) h = 1;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP thumb = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!thumb) return;

    HDC screen = GetDC(nullptr);
    HDC dst = CreateCompatibleDC(screen);
    HDC src = CreateCompatibleDC(screen);
    HGDIOBJ oldDst = SelectObject(dst, thumb);
    HGDIOBJ oldSrc = SelectObject(src, g_wall);
    SetStretchBltMode(dst, HALFTONE);
    SetBrushOrgEx(dst, 0, 0, nullptr);
    StretchBlt(dst, 0, 0, w, h, src, 0, 0, g_wallW, g_wallH, SRCCOPY);
    SelectObject(src, oldSrc);
    SelectObject(dst, oldDst);
    DeleteDC(src);
    DeleteDC(dst);
    ReleaseDC(nullptr, screen);

    DwmSetIconicThumbnail(hwnd, thumb, 0);
    DeleteObject(thumb);
}

// ---------- Minimize all at once (v1.4) ----------

#define MAX_TARGETS 256
static HWND g_targets[MAX_TARGETS];
static int g_targetCount = 0;

static BOOL CALLBACK CollectProc(HWND h, LPARAM) {
    if (g_targetCount >= MAX_TARGETS) return FALSE;
    if (h == g_hwnd) return TRUE;
    if (!IsWindowVisible(h) || IsIconic(h)) return TRUE;

    LONG ex = GetWindowLongW(h, GWL_EXSTYLE);
    if (ex & WS_EX_TOOLWINDOW) return TRUE;
    if (GetWindow(h, GW_OWNER) && !(ex & WS_EX_APPWINDOW)) return TRUE;

    // Skip hidden UWP ghost windows
    BOOL cloaked = FALSE;
    DwmGetWindowAttribute(h, 14 /*DWMWA_CLOAKED*/, &cloaked, sizeof(cloaked));
    if (cloaked) return TRUE;

    // Skip the shell itself
    wchar_t cls[64] = {};
    GetClassNameW(h, cls, 64);
    if (!wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW") ||
        !wcscmp(cls, L"Shell_TrayWnd") || !wcscmp(cls, L"Shell_SecondaryTrayWnd"))
        return TRUE;

    g_targets[g_targetCount++] = h;
    return TRUE;
}

static void MinimizeAllInstant() {
    // 1) Turn off the minimize animation so everything drops together
    if (!g_animSaved) {
        ANIMATIONINFO ai = { sizeof(ai) };
        SystemParametersInfoW(SPI_GETANIMATION, sizeof(ai), &ai, 0);
        g_oldMinAnim = ai.iMinAnimate;
        if (g_oldMinAnim) {
            ai.iMinAnimate = 0;
            SystemParametersInfoW(SPI_SETANIMATION, sizeof(ai), &ai, 0);
            g_animSaved = true;
        }
    }

    // 2) Collect every target first so the minimize loop below is as tight as possible
    g_targetCount = 0;
    EnumWindows(CollectProc, 0);

    // 3) Un-maximize + minimize each window in one pass
    for (int i = 0; i < g_targetCount; i++) {
        HWND h = g_targets[i];
        if (IsHungAppWindow(h)) {
            ShowWindowAsync(h, SW_MINIMIZE);  // never block on a frozen app
            continue;
        }
        WINDOWPLACEMENT wp = { sizeof(wp) };
        if (GetWindowPlacement(h, &wp)) {
            wp.flags &= ~WPF_RESTORETOMAXIMIZED;  // comes back un-maximized
            wp.showCmd = SW_SHOWMINNOACTIVE;
            SetWindowPlacement(h, &wp);
        } else {
            ShowWindowAsync(h, SW_MINIMIZE);
        }
    }

    if (g_animSaved && g_hwnd)
        SetTimer(g_hwnd, TIMER_RESTORE_ANIM, 500, nullptr);
}

static void RestoreAnimation() {
    if (!g_animSaved) return;
    ANIMATIONINFO ai = { sizeof(ai) };
    ai.iMinAnimate = g_oldMinAnim;
    SystemParametersInfoW(SPI_SETANIMATION, sizeof(ai), &ai, 0);
    g_animSaved = false;
}

static void FireOnce() {
    DWORD now = GetTickCount();
    if (now - g_lastFire < 250) return;  // avoid double-trigger
    g_lastFire = now;
    MinimizeAllInstant();
}

// ---------- Window ----------

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_DWMSENDICONICTHUMBNAIL:
        SendThumbnail(hwnd, HIWORD(lp), LOWORD(lp));
        return 0;

    case WM_DWMSENDICONICLIVEPREVIEWBITMAP:
        if (g_wall) DwmSetIconicLivePreviewBitmap(hwnd, g_wall, nullptr, 0);
        return 0;

    case WM_SETTINGCHANGE:
        if (wp == SPI_SETDESKWALLPAPER) {
            ReloadWallpaper();
            DwmInvalidateIconicBitmaps(hwnd);
        }
        break;

    case WM_DISPLAYCHANGE:
        ReloadWallpaper();
        DwmInvalidateIconicBitmaps(hwnd);
        break;

    case WM_TIMER:
        if (wp == TIMER_RESTORE_ANIM) {
            KillTimer(hwnd, TIMER_RESTORE_ANIM);
            RestoreAnimation();
        }
        return 0;

    case WM_SYSCOMMAND:
        // Alt+Tab asks to restore us: fire immediately, stay minimized
        if ((wp & 0xFFF0) == SC_RESTORE || (wp & 0xFFF0) == SC_MAXIMIZE) {
            FireOnce();
            return 0;  // block the restore, no animation
        }
        break;

    case WM_ACTIVATE:
        if (LOWORD(wp) != WA_INACTIVE) {
            // Ignore activation caused by clicking the taskbar button,
            // so right-click > Close window works
            POINT pt;
            GetCursorPos(&pt);
            HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
            RECT r;
            bool overTaskbar = tray && GetWindowRect(tray, &r) && PtInRect(&r, pt);
            bool mouseDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ||
                             (GetAsyncKeyState(VK_RBUTTON) & 0x8000);
            if (!overTaskbar && !mouseDown) {
                FireOnce();
                ShowWindow(hwnd, SW_SHOWMINNOACTIVE);
            }
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        KillTimer(hwnd, TIMER_RESTORE_ANIM);
        RestoreAnimation();
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static DWORD WINAPI WindowThread(LPVOID) {
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    HICON icon = nullptr;
    ExtractIconExW(L"shell32.dll", 34, &icon, nullptr, 1);  // desktop-style icon

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kClass;
    wc.hIcon = icon;
    RegisterClassW(&wc);

    ReloadWallpaper();

    g_hwnd = CreateWindowExW(
        WS_EX_APPWINDOW, kClass, L"Desktop",
        WS_OVERLAPPEDWINDOW,
        100, 100, 300, 200,
        nullptr, nullptr, wc.hInstance, nullptr);

    if (g_hwnd) {
        // Use our own bitmap (the wallpaper) for the Alt+Tab / taskbar thumbnail
        BOOL t = TRUE;
        DwmSetWindowAttribute(g_hwnd, 7 /*DWMWA_FORCE_ICONIC_REPRESENTATION*/, &t, sizeof(t));
        DwmSetWindowAttribute(g_hwnd, 10 /*DWMWA_HAS_ICONIC_BITMAP*/, &t, sizeof(t));

        ShowWindow(g_hwnd, SW_SHOWMINNOACTIVE);
        MSG m;
        while (GetMessageW(&m, nullptr, 0, 0) > 0) {
            TranslateMessage(&m);
            DispatchMessageW(&m);
        }
    }

    if (g_wall) { DeleteObject(g_wall); g_wall = nullptr; }
    UnregisterClassW(kClass, wc.hInstance);
    if (icon) DestroyIcon(icon);
    if (SUCCEEDED(hrCo)) CoUninitialize();
    return 0;
}

BOOL Wh_ModInit() {
    g_thread = CreateThread(nullptr, 0, WindowThread, nullptr, 0, nullptr);
    return g_thread != nullptr;
}

void Wh_ModUninit() {
    if (g_hwnd) PostMessageW(g_hwnd, WM_CLOSE, 0, 0);
    if (g_thread) {
        WaitForSingleObject(g_thread, 2000);
        CloseHandle(g_thread);
    }
}
