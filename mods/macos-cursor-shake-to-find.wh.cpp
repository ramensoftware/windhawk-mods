// ==WindhawkMod==
// @id              macos-cursor-shake-to-find
// @name            macOS Cursor & Shake to Find (4K HD)
// @description     Replaces the default Windows cursor with macOS cursors and adds the smooth "Shake to Find" cursor magnification feature.
// @version         1.4.0
// @author          brcarvp
// @github          https://github.com/brcarvp
// @include         explorer.exe
// @compilerOptions -luser32 -lgdi32 -lgdiplus
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# macOS Cursor & Shake to Find (4K HD)

Transform your Windows mouse pointer into the sleek **macOS cursor** and unlock the native macOS **"Shake to Find"** feature. When you shake your mouse rapidly, the cursor smoothly enlarges in crisp 4K high-DPI quality and automatically returns to your macOS cursor afterward.

![macOS Cursors Preview](https://i.imgur.com/AH6rkDP.jpeg)

## Features

- **Full macOS Cursor Theme:** Automatically replaces standard Windows pointers (Arrow, Hand/Link, Text I-Beam, Busy, Resize arrows, etc.) with macOS `.cur` and `.ani` files.
- **Shake to Find:** Rapidly shaking the mouse triggers a smooth, 60 FPS bicubic-interpolated magnification overlay so you never lose track of your pointer on large or multi-monitor setups.
- **Persistent macOS Style:** Automatically preserves and restores your macOS cursor theme after shaking—never reverting back to the default white Windows pointer.

## How to Set Up the Cursor Folder

Because this mod uses external `.cur` and `.ani` cursor files, follow these steps before enabling it:

1. Download any **macOS Cursor Pack for Windows** (such as macOS Big Sur, Monterey, or Sonoma cursors).
2. Extract all `.cur` and `.ani` files into a dedicated folder on your drive, for example:
   `C:\Cursors\macOS`
3. Make sure your cursor files match these standard names inside that folder:
   - `Normal.cur` (Default pointer)
   - `Link.cur` (Hand / Link select)
   - `Text.cur` (Text select / I-Beam)
   - `Busy.ani` (Busy)
   - `Working.ani` (Working in background)
   - `Precision.cur` (Precision select)
   - `Alternate.cur` (Alternate select)
   - `Diagonal Resize 1.cur`, `Diagonal Resize 2.cur`
   - `Horizontal Resize.cur`, `Vertical Resize.cur`
   - `Move.cur`, `Unavailable.cur`
4. Open the mod **Settings** tab, set **macOS Cursors Folder Path** to `C:\Cursors\macOS` (or your custom folder path), and click **Save Settings**.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- cursorFolder: "C:\\Cursors\\macOS"
  $name: "macOS Cursors Folder Path"
  $description: "Full path to the folder containing your macOS .cur and .ani cursor files"
- applyMacCursors: true
  $name: "Apply macOS Cursor Theme"
  $description: "Replaces default Windows system cursors with the macOS files from the folder above"
- enableShakeToFind: true
  $name: "Enable Shake to Find"
  $description: "Temporarily enlarges the macOS cursor when shaking the mouse rapidly"
- maxScalePercent: 350
  $name: "Maximum Shake Scale (%)"
  $description: "Magnification percentage when shaking the mouse (350 = 3.5x normal size)"
- shakeSensitivity: 3
  $name: "Shake Sensitivity (Reversals)"
  $description: "Number of rapid direction changes required to trigger (2 = very easy, 4 = normal)"
- restoreDelayMs: 700
  $name: "Restore Delay (ms)"
  $description: "Time in milliseconds after stopping before the cursor shrinks back to normal"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <vector>
#include <cmath>
#include <atomic>

struct ModSettings {
    std::wstring cursorFolder;
    bool applyMacCursors;
    bool enableShakeToFind;
    float maxScale;
    int shakeSensitivity;
    int restoreDelayMs;
} g_settings;

struct CursorMap {
    DWORD ocrId;
    const wchar_t* fileName;
};

const CursorMap g_cursorMaps[] = {
    { 32512, L"Normal.cur" }, { 32513, L"Text.cur" }, { 32514, L"Busy.ani" },
    { 32515, L"Precision.cur" }, { 32516, L"Alternate.cur" }, { 32642, L"Diagonal Resize 1.cur" },
    { 32643, L"Diagonal Resize 2.cur" }, { 32644, L"Horizontal Resize.cur" },
    { 32645, L"Vertical Resize.cur" }, { 32646, L"Move.cur" }, { 32648, L"Unavailable.cur" },
    { 32649, L"Link.cur" }, { 32650, L"Working.ani" }
};

HANDLE g_thread = NULL;
std::atomic<bool> g_running{false};
HWND g_hwndOverlay = NULL;
Gdiplus::Bitmap* g_pHighResCursorBmp = nullptr;
int g_hotspotX = 0;
int g_hotspotY = 0;
int g_hiResBaseSize = 128;
bool g_sysCursorHidden = false;

HCURSOR CreateBlankCursor() {
    int w = GetSystemMetrics(SM_CXCURSOR);
    int h = GetSystemMetrics(SM_CYCURSOR);
    if (w <= 0) w = 32;
    if (h <= 0) h = 32;
    size_t sz = (static_cast<size_t>(w + 7) / 8) * h;
    std::vector<BYTE> andM(sz, 0xFF), xorM(sz, 0x00);
    return CreateCursor(GetModuleHandleW(NULL), 0, 0, w, h, andM.data(), xorM.data());
}

void PrepareHighResCursorBitmap(const std::wstring& arrowPath) {
    if (g_pHighResCursorBmp) {
        delete g_pHighResCursorBmp;
        g_pHighResCursorBmp = nullptr;
    }

    HCURSOR hHiRes = (HCURSOR)LoadImageW(NULL, arrowPath.c_str(), IMAGE_CURSOR, 128, 128, LR_LOADFROMFILE);
    if (!hHiRes) {
        hHiRes = (HCURSOR)LoadImageW(NULL, arrowPath.c_str(), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
    }
    if (!hHiRes) return;

    ICONINFO ii = { 0 };
    if (GetIconInfo(hHiRes, &ii)) {
        BITMAP bm = { 0 };
        GetObjectW(ii.hbmMask, sizeof(BITMAP), &bm);
        int w = bm.bmWidth > 0 ? bm.bmWidth : 128;
        int h = (ii.hbmColor ? bm.bmHeight : bm.bmHeight / 2);
        if (h <= 0) h = 128;

        g_hiResBaseSize = w;
        g_hotspotX = ii.xHotspot;
        g_hotspotY = ii.yHotspot;

        HDC hdcScreen = GetDC(NULL);
        HDC hdcMem = CreateCompatibleDC(hdcScreen);
        BITMAPINFO bmi = { { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB } };
        void* pBits = nullptr;
        HBITMAP hbm = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);

        if (hbm && pBits) {
            HBITMAP oldBm = (HBITMAP)SelectObject(hdcMem, hbm);
            DWORD* px = static_cast<DWORD*>(pBits);
            size_t count = static_cast<size_t>(w) * h;
            for (size_t i = 0; i < count; ++i) px[i] = 0x00000001;

            DrawIconEx(hdcMem, 0, 0, hHiRes, w, h, 0, NULL, DI_NORMAL);

            bool hasAlpha = false;
            for (size_t i = 0; i < count; ++i) {
                if (px[i] & 0xFF000000) { hasAlpha = true; break; }
            }
            if (!hasAlpha) {
                for (size_t i = 0; i < count; ++i) {
                    px[i] = (px[i] == 0x00000001) ? 0 : (px[i] | 0xFF000000);
                }
            }

            Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(w, h, PixelFormat32bppARGB);
            Gdiplus::BitmapData bmpData;
            Gdiplus::Rect rect(0, 0, w, h);
            if (bmp->LockBits(&rect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmpData) == Gdiplus::Ok) {
                memcpy(bmpData.Scan0, pBits, count * sizeof(DWORD));
                bmp->UnlockBits(&bmpData);
                g_pHighResCursorBmp = bmp;
            } else {
                delete bmp;
            }

            SelectObject(hdcMem, oldBm);
            DeleteObject(hbm);
        }

        DeleteDC(hdcMem);
        ReleaseDC(NULL, hdcScreen);
        if (ii.hbmMask) DeleteObject(ii.hbmMask);
        if (ii.hbmColor) DeleteObject(ii.hbmColor);
    }
    DestroyIcon(hHiRes);
}

void ApplyMacOSCursors() {
    if (!g_settings.applyMacCursors || g_settings.cursorFolder.empty()) return;

    std::wstring dir = g_settings.cursorFolder;
    if (dir.back() != L'\\' && dir.back() != L'/') dir += L'\\';

    for (const auto& item : g_cursorMaps) {
        std::wstring path = dir + item.fileName;
        if (GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES) {
            if (item.ocrId == 32512) {
                PrepareHighResCursorBitmap(path);
            }
            HCURSOR hCur = (HCURSOR)LoadImageW(NULL, path.c_str(), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
            if (hCur) {
                SetSystemCursor(hCur, item.ocrId);
            }
        }
    }
    g_sysCursorHidden = false;
}

void HideSystemCursorsForShake() {
    if (g_sysCursorHidden) return;
    HCURSOR hBlank = CreateBlankCursor();
    if (!hBlank) return;

    for (const auto& item : g_cursorMaps) {
        HCURSOR hCopy = CopyIcon(hBlank);
        if (hCopy && !SetSystemCursor(hCopy, item.ocrId)) {
            DestroyIcon(hCopy);
        }
    }
    DestroyCursor(hBlank);
    g_sysCursorHidden = true;
}

void DrawScaledOverlay4K(HWND hwnd, POINT pt, float scale) {
    if (!g_pHighResCursorBmp) return;

    int baseDisplaySize = GetSystemMetrics(SM_CXCURSOR);
    if (baseDisplaySize <= 0) baseDisplaySize = 32;

    int targetW = static_cast<int>(baseDisplaySize * scale);
    int targetH = static_cast<int>(baseDisplaySize * scale);

    float hotspotRatioX = static_cast<float>(g_hotspotX) / static_cast<float>(g_hiResBaseSize);
    float hotspotRatioY = static_cast<float>(g_hotspotY) / static_cast<float>(g_hiResBaseSize);

    int winX = pt.x - static_cast<int>(targetW * hotspotRatioX);
    int winY = pt.y - static_cast<int>(targetH * hotspotRatioY);

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    BITMAPINFO bmi = { { sizeof(BITMAPINFOHEADER), targetW, -targetH, 1, 32, BI_RGB } };
    void* pBits = nullptr;
    HBITMAP hbm = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);

    if (hbm && pBits) {
        HBITMAP oldBm = (HBITMAP)SelectObject(hdcMem, hbm);

        {
            Gdiplus::Graphics graphics(hdcMem);
            graphics.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
            graphics.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
            graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
            graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
            graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);

            graphics.DrawImage(g_pHighResCursorBmp, 0, 0, targetW, targetH);
        }

        DWORD* px = static_cast<DWORD*>(pBits);
        size_t count = static_cast<size_t>(targetW) * targetH;
        for (size_t i = 0; i < count; ++i) {
            DWORD c = px[i];
            BYTE a = (c >> 24) & 0xFF;
            if (a < 255) {
                BYTE r = (((c >> 16) & 0xFF) * a) / 255;
                BYTE g = (((c >> 8) & 0xFF) * a) / 255;
                BYTE b = ((c & 0xFF) * a) / 255;
                px[i] = (static_cast<DWORD>(a) << 24) | (static_cast<DWORD>(r) << 16) | (static_cast<DWORD>(g) << 8) | b;
            }
        }

        POINT ptZero = { 0, 0 }, ptWin = { winX, winY };
        SIZE sz = { targetW, targetH };
        BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
        UpdateLayeredWindow(hwnd, hdcScreen, &ptWin, &sz, hdcMem, &ptZero, 0, &blend, ULW_ALPHA);

        SelectObject(hdcMem, oldBm);
        DeleteObject(hbm);
    }

    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

typedef BOOL (WINAPI *pfn_SetThreadDpiAwarenessContext)(HANDLE);

DWORD WINAPI ShakeMonitorThread(LPVOID) {
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        auto pSetDpi = (pfn_SetThreadDpiAwarenessContext)GetProcAddress(hUser32, "SetThreadDpiAwarenessContext");
        if (pSetDpi) pSetDpi((HANDLE)-4);
    }

    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW), 0, DefWindowProcW, 0, 0, GetModuleHandleW(NULL), NULL, NULL, NULL, NULL, L"WhMacShakeOverlay4K", NULL };
    RegisterClassExW(&wc);

    g_hwndOverlay = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"", WS_POPUP, 0, 0, 1, 1, NULL, NULL, wc.hInstance, NULL);

    POINT lastPt{};
    GetCursorPos(&lastPt);
    int lastDir = 0, accumDx = 0, reversals = 0;
    ULONGLONG lastRevTime = 0, lastShakeTime = 0, lastEnforceTime = 0;
    float currentScale = 1.0f, targetScale = 1.0f;
    bool visible = false;

    while (g_running.load()) {
        POINT pt{};
        ULONGLONG now = GetTickCount64();

        if (g_settings.enableShakeToFind && GetCursorPos(&pt)) {
            int dx = pt.x - lastPt.x;
            lastPt = pt;

            if (now - lastRevTime > 320 && reversals > 0) {
                reversals = 0; lastDir = 0; accumDx = 0;
            }

            if (dx != 0) {
                int dir = (dx > 0) ? 1 : -1;
                if (dir == lastDir || lastDir == 0) {
                    accumDx += std::abs(dx);
                    if (lastDir == 0) { lastDir = dir; lastRevTime = now; }
                } else {
                    if (accumDx >= 20) {
                        reversals++;
                        lastRevTime = now;
                        if (reversals >= g_settings.shakeSensitivity) {
                            targetScale = g_settings.maxScale;
                            lastShakeTime = now;
                        }
                    }
                    accumDx = std::abs(dx);
                    lastDir = dir;
                }
            }

            if (targetScale > 1.0f && (now - lastShakeTime > static_cast<ULONGLONG>(g_settings.restoreDelayMs))) {
                targetScale = 1.0f;
            }

            float speed = (targetScale > currentScale) ? 0.35f : 0.18f;
            currentScale += (targetScale - currentScale) * speed;

            if (currentScale > 1.05f) {
                if (g_settings.applyMacCursors) HideSystemCursorsForShake();
                DrawScaledOverlay4K(g_hwndOverlay, pt, currentScale);
                if (!visible) {
                    SetWindowPos(g_hwndOverlay, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
                    visible = true;
                }
            } else {
                if (visible) {
                    ShowWindow(g_hwndOverlay, SW_HIDE);
                    visible = false;
                }
                if (g_sysCursorHidden) {
                    if (g_settings.applyMacCursors) ApplyMacOSCursors();
                    else { SystemParametersInfoW(SPI_SETCURSORS, 0, NULL, 0); g_sysCursorHidden = false; }
                }
            }
        }

        if (!visible && !g_sysCursorHidden && g_settings.applyMacCursors && (now - lastEnforceTime > 2000)) {
            lastEnforceTime = now;
            HCURSOR hCurSys = (HCURSOR)LoadImageW(NULL, MAKEINTRESOURCEW(32512), IMAGE_CURSOR, 0, 0, LR_SHARED);
            if (!hCurSys) ApplyMacOSCursors();
        }

        Sleep(16);
    }

    if (g_hwndOverlay) { DestroyWindow(g_hwndOverlay); g_hwndOverlay = NULL; }
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return 0;
}

void LoadSettings() {
    PCWSTR folder = Wh_GetStringSetting(L"cursorFolder");
    g_settings.cursorFolder = folder ? folder : L"";
    Wh_FreeStringSetting(folder);

    g_settings.applyMacCursors = Wh_GetIntSetting(L"applyMacCursors") != 0;
    g_settings.enableShakeToFind = Wh_GetIntSetting(L"enableShakeToFind") != 0;
    int pct = Wh_GetIntSetting(L"maxScalePercent");
    g_settings.maxScale = (pct >= 150 && pct <= 800) ? (pct / 100.0f) : 3.5f;
    int sens = Wh_GetIntSetting(L"shakeSensitivity");
    g_settings.shakeSensitivity = (sens >= 2 && sens <= 10) ? sens : 3;
    int delay = Wh_GetIntSetting(L"restoreDelayMs");
    g_settings.restoreDelayMs = (delay >= 200) ? delay : 700;
}

ULONG_PTR g_gdiplusToken = 0;

BOOL Wh_ModInit() {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);

    LoadSettings();
    ApplyMacOSCursors();
    g_running = true;
    g_thread = CreateThread(NULL, 0, ShakeMonitorThread, NULL, 0, NULL);
    return TRUE;
}

void Wh_ModUninit() {
    g_running = false;
    if (g_thread) { WaitForSingleObject(g_thread, 2000); CloseHandle(g_thread); g_thread = NULL; }
    if (g_pHighResCursorBmp) { delete g_pHighResCursorBmp; g_pHighResCursorBmp = nullptr; }
    if (g_gdiplusToken) { Gdiplus::GdiplusShutdown(g_gdiplusToken); g_gdiplusToken = 0; }
    SystemParametersInfoW(SPI_SETCURSORS, 0, NULL, 0);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_settings.applyMacCursors) ApplyMacOSCursors();
    else { SystemParametersInfoW(SPI_SETCURSORS, 0, NULL, 0); g_sysCursorHidden = false; }
}
