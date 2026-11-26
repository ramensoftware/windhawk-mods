// ==WindhawkMod==
// @id              lockscreen-wallpaper
// @name            Lock Screen Custom Wallpaper
// @description     Set any static image (JPG, PNG, BMP, WEBP) as your Windows lock screen background. Works without admin prompts. Includes file picker.
// @version         2.1
// @author          Siddharthxd99
// @github          https://github.com/Siddharthxd99
// @include         explorer.exe
// @include         winlogon.exe
// @compilerOptions -lgdiplus -lcomdlg32 -luser32 -ladvapi32 -lgdi32 -lole32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Lock Screen Custom Wallpaper

Replaces the Windows lock screen background with **any image** you choose.
Uses the same technique as Wallpaper Engine. No UAC prompts, no admin rights needed.

## How it works
- **explorer.exe** (user context): reads your setting and converts your image to JPEG
- **winlogon.exe** (SYSTEM context): writes the lock screen registry keys

## How to use
1. In Windhawk settings, paste the full path to your image in **Wallpaper Path**
2. Click **Save**
3. Press **Win + L** to see it on the lock screen

## Supported formats
JPG, PNG, BMP, WEBP

## Notes
- The converted JPEG is saved to `%ProgramData%\LockWallpaperMod\lock.jpg`
- Disabling/uninstalling the mod restores the default lock screen background
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- wallpaperPath: ""
  $name: Wallpaper Path
  $description: Enter the full path to your image (e.g., C:\Images\wall.jpg)
- stretchMode: 0
  $name: Stretch Mode
  $description: "0=Fill  1=Fit(letterbox)  2=Stretch  3=Center  4=Tile"
- opacity: 255
  $name: Opacity (0-255)
  $description: 255 = fully opaque, 0 = invisible
*/
// ==/WindhawkModSettings==

// ─── Version targeting MUST come before ALL Windows headers ───────────────────
#ifndef _WIN32_WINNT
#  define _WIN32_WINNT   0x0A00
#endif
#ifndef NTDDI_VERSION
#  define NTDDI_VERSION  0x0A000000
#endif
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <gdiplus.h>
#include <commdlg.h>
#include <string>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "advapi32.lib")

using namespace Gdiplus;

// ─── Registry key paths ───────────────────────────────────────────────────────
// Method 1: Group Policy — highest priority, works on all builds
static constexpr wchar_t kGPOKey[]   = L"SOFTWARE\\Policies\\Microsoft\\Windows\\Personalization";
static constexpr wchar_t kGPOVal[]   = L"LockScreenImage";

// Method 2: PersonalizationCSP — modern / MDM path (Windows 10 1703+)
static constexpr wchar_t kCSPKey[]   = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\PersonalizationCSP";
static constexpr wchar_t kCSPPath[]  = L"LockScreenImagePath";
static constexpr wchar_t kCSPUrl[]   = L"LockScreenImageUrl";
static constexpr wchar_t kCSPStat[]  = L"LockScreenImageStatus";

// Fixed JPEG output path — both explorer and winlogon share this
static constexpr wchar_t kJpegPath[] = L"C:\\ProgramData\\LockWallpaperMod\\lock.jpg";
static constexpr wchar_t kJpegDir[]  = L"C:\\ProgramData\\LockWallpaperMod";

// Named event: explorer sets this after saving JPEG; winlogon waits for it
static constexpr wchar_t kReadyEvt[] = L"Local\\LockWallpaperModJpegReady";

// ─── Process context detection ────────────────────────────────────────────────
static bool g_isWinlogon = false;
static ULONG_PTR g_gdiplusToken = 0;

static bool DetectWinlogon() {
    wchar_t path[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    std::wstring s(path);
    for (wchar_t& c : s) c = towlower(c);
    return s.find(L"winlogon.exe") != std::wstring::npos;
}

// ─── Registry helpers ─────────────────────────────────────────────────────────
static bool RegWriteSz(HKEY root, const wchar_t* key, const wchar_t* val,
                       const std::wstring& data) {
    HKEY hk = nullptr;
    if (RegCreateKeyExW(root, key, 0, nullptr, 0,
                        KEY_WRITE, nullptr, &hk, nullptr) != ERROR_SUCCESS)
        return false;
    LONG r = RegSetValueExW(hk, val, 0, REG_SZ,
        reinterpret_cast<const BYTE*>(data.c_str()),
        static_cast<DWORD>((data.size() + 1) * sizeof(wchar_t)));
    RegCloseKey(hk);
    return r == ERROR_SUCCESS;
}

static bool RegWriteDword(HKEY root, const wchar_t* key, const wchar_t* val,
                          DWORD data) {
    HKEY hk = nullptr;
    if (RegCreateKeyExW(root, key, 0, nullptr, 0,
                        KEY_WRITE, nullptr, &hk, nullptr) != ERROR_SUCCESS)
        return false;
    LONG r = RegSetValueExW(hk, val, 0, REG_DWORD,
        reinterpret_cast<const BYTE*>(&data), sizeof(data));
    RegCloseKey(hk);
    return r == ERROR_SUCCESS;
}

static void RegDeleteVal(HKEY root, const wchar_t* key, const wchar_t* val) {
    HKEY hk = nullptr;
    if (RegOpenKeyExW(root, key, 0, KEY_WRITE, &hk) != ERROR_SUCCESS) return;
    RegDeleteValueW(hk, val);
    RegCloseKey(hk);
}

static std::wstring RegReadSz(HKEY root, const wchar_t* key, const wchar_t* val) {
    HKEY hk = nullptr;
    if (RegOpenKeyExW(root, key, 0, KEY_READ, &hk) != ERROR_SUCCESS) return L"";
    wchar_t buf[MAX_PATH] = {};
    DWORD sz   = sizeof(buf);
    DWORD type = REG_SZ;
    RegQueryValueExW(hk, val, nullptr, &type, reinterpret_cast<BYTE*>(buf), &sz);
    RegCloseKey(hk);
    return std::wstring(buf);
}

// ─────────────────────────────────────────────────────────────────────────────
//  WINLOGON CONTEXT  (SYSTEM — writes HKLM)
// ─────────────────────────────────────────────────────────────────────────────

// Writes HKLM registry keys pointing to the JPEG file.
// Called from winlogon.exe which runs as SYSTEM → no elevation needed.
static void WinlogonApplyRegistry() {
    // Verify JPEG is ready
    if (GetFileAttributesW(kJpegPath) == INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"[LockWallpaper|winlogon] JPEG not found at %s. "
               L"Set a valid path in Windhawk settings first.", kJpegPath);
        return;
    }

    std::wstring jp(kJpegPath);

    // Clear both keys first — a genuine value change forces Windows to
    // drop its cached lock screen image and reload from disk.
    RegDeleteVal(HKEY_LOCAL_MACHINE, kGPOKey, kGPOVal);
    RegDeleteVal(HKEY_LOCAL_MACHINE, kCSPKey, kCSPPath);
    RegDeleteVal(HKEY_LOCAL_MACHINE, kCSPKey, kCSPUrl);
    RegDeleteVal(HKEY_LOCAL_MACHINE, kCSPKey, kCSPStat);
    Sleep(150);   // small pause so Windows sees the delete before the rewrite

    // Method 1: Group Policy (SYSTEM can write this)
    bool gpo = RegWriteSz(HKEY_LOCAL_MACHINE, kGPOKey, kGPOVal, jp);

    // Method 2: PersonalizationCSP
    bool c1  = RegWriteSz   (HKEY_LOCAL_MACHINE, kCSPKey, kCSPPath, jp);
    bool c2  = RegWriteSz   (HKEY_LOCAL_MACHINE, kCSPKey, kCSPUrl,  jp);
    bool c3  = RegWriteDword(HKEY_LOCAL_MACHINE, kCSPKey, kCSPStat, 1);

    Wh_Log(L"[LockWallpaper|winlogon] HKLM write: GPO=%s  CSP(path=%s url=%s status=%s)",
           gpo ? L"OK" : L"FAIL",
           c1  ? L"OK" : L"FAIL",
           c2  ? L"OK" : L"FAIL",
           c3  ? L"OK" : L"FAIL");

    if (!gpo && !c1)
        Wh_Log(L"[LockWallpaper|winlogon] ERROR: All registry writes failed!");

    // Broadcast policy change so Windows reloads immediately
    DWORD_PTR res = 0;
    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                        reinterpret_cast<LPARAM>(L"Policy"),
                        SMTO_ABORTIFHUNG, 2000, &res);

    Wh_Log(L"[LockWallpaper|winlogon] Done. Lock screen updated — press Win+L to verify.");
}

// ─────────────────────────────────────────────────────────────────────────────
//  EXPLORER CONTEXT  (user — file picker, GDI+ conversion, JPEG save)
// ─────────────────────────────────────────────────────────────────────────────

struct Settings {
    std::wstring wallpaperPath;
    int          stretchMode = 0;
    BYTE         opacity     = 255;
} g_settings;

static void LoadSettings() {
    LPCWSTR raw = Wh_GetStringSetting(L"wallpaperPath");
    g_settings.wallpaperPath = raw ? raw : L"";
    Wh_FreeStringSetting(raw);

    g_settings.stretchMode   = Wh_GetIntSetting(L"stretchMode");

    int op = Wh_GetIntSetting(L"opacity");
    if (op < 0)   op = 0;
    if (op > 255) op = 255;
    g_settings.opacity = static_cast<BYTE>(op);

    Wh_Log(L"[LockWallpaper|explorer] Settings: path=%s stretch=%d opacity=%d",
           g_settings.wallpaperPath.c_str(), g_settings.stretchMode,
           static_cast<int>(g_settings.opacity));
}

static bool GetJpegEncoderClsid(CLSID* clsid) {
    UINT num = 0, sz = 0;
    GetImageEncodersSize(&num, &sz);
    if (!sz) return false;
    auto* info = static_cast<ImageCodecInfo*>(malloc(sz));
    if (!info) return false;
    GetImageEncoders(num, sz, info);
    bool found = false;
    for (UINT i = 0; i < num && !found; i++) {
        if (wcscmp(info[i].MimeType, L"image/jpeg") == 0) {
            *clsid = info[i].Clsid;
            found  = true;
        }
    }
    free(info);
    return found;
}

// Converts the source image to a screen-resolution JPEG and saves it.
// After saving, sets an event so winlogon knows to write the registry.
static bool PrepareAndSaveJpeg() {
    if (g_settings.wallpaperPath.empty()) {
        Wh_Log(L"[LockWallpaper|explorer] No path. Paste a path in settings.");
        return false;
    }
    if (GetFileAttributesW(g_settings.wallpaperPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"[LockWallpaper|explorer] File not found: %s",
               g_settings.wallpaperPath.c_str());
        return false;
    }

    // ── Fix race condition: reset the event NOW so winlogon blocks until
    //    we signal it AFTER the new JPEG is fully written. ─────────────────
    {
        HANDLE ev = CreateEventW(nullptr, TRUE, FALSE, kReadyEvt);
        if (ev) { ResetEvent(ev); CloseHandle(ev); }
    }

    // Load source image
    Image* src = Image::FromFile(g_settings.wallpaperPath.c_str());
    if (!src || src->GetLastStatus() != Ok) {
        Wh_Log(L"[LockWallpaper|explorer] GDI+ failed to load: %s",
               g_settings.wallpaperPath.c_str());
        delete src;
        return false;
    }

    int srcW = static_cast<int>(src->GetWidth());
    int srcH = static_cast<int>(src->GetHeight());
    int scrW = GetSystemMetrics(SM_CXSCREEN);
    int scrH = GetSystemMetrics(SM_CYSCREEN);

    Wh_Log(L"[LockWallpaper|explorer] Source %dx%d  Screen %dx%d  Stretch=%d",
           srcW, srcH, scrW, scrH, g_settings.stretchMode);

    // Render onto screen-sized 32-bit bitmap
    Bitmap bmp(scrW, scrH, PixelFormat32bppRGB);
    {
        Graphics g(&bmp);
        g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
        g.SetSmoothingMode(SmoothingModeHighQuality);
        g.SetPixelOffsetMode(PixelOffsetModeHighQuality);
        g.SetCompositingQuality(CompositingQualityHighQuality);

        SolidBrush black(Color(255, 0, 0, 0));
        g.FillRectangle(&black, 0, 0, scrW, scrH);

        float alpha = static_cast<float>(g_settings.opacity) / 255.0f;
        ColorMatrix cm = {{
            {1,0,0,0,0},{0,1,0,0,0},{0,0,1,0,0},
            {0,0,0,alpha,0},{0,0,0,0,1}
        }};
        ImageAttributes attrs;
        attrs.SetColorMatrix(&cm, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);

        int dX = 0, dY = 0, dW = scrW, dH = scrH;

        switch (g_settings.stretchMode) {
            case 0: { // Fill
                float sw = static_cast<float>(scrW) / srcW;
                float sh = static_cast<float>(scrH) / srcH;
                float s  = (sw > sh) ? sw : sh;
                dW = static_cast<int>(srcW * s);
                dH = static_cast<int>(srcH * s);
                dX = (scrW - dW) / 2;
                dY = (scrH - dH) / 2;
                break;
            }
            case 1: { // Fit
                float sw = static_cast<float>(scrW) / srcW;
                float sh = static_cast<float>(scrH) / srcH;
                float s  = (sw < sh) ? sw : sh;
                dW = static_cast<int>(srcW * s);
                dH = static_cast<int>(srcH * s);
                dX = (scrW - dW) / 2;
                dY = (scrH - dH) / 2;
                break;
            }
            case 2: break;
            case 3: {
                dW = srcW; dH = srcH;
                dX = (scrW - srcW) / 2;
                dY = (scrH - srcH) / 2;
                break;
            }
            case 4: {
                for (int tx = 0; tx < scrW; tx += srcW)
                    for (int ty = 0; ty < scrH; ty += srcH)
                        g.DrawImage(src, Rect(tx, ty, srcW, srcH),
                                    0, 0, srcW, srcH, UnitPixel, &attrs);
                delete src;
                goto save;
            }
            default: break;
        }
        g.DrawImage(src, Rect(dX, dY, dW, dH),
                    0, 0, srcW, srcH, UnitPixel, &attrs);
    }
    delete src;

save:
    CLSID jpegClsid;
    if (!GetJpegEncoderClsid(&jpegClsid)) {
        Wh_Log(L"[LockWallpaper|explorer] JPEG encoder not found");
        return false;
    }
    EncoderParameters ep;
    ep.Count                       = 1;
    ep.Parameter[0].Guid           = EncoderQuality;
    ep.Parameter[0].Type           = EncoderParameterValueTypeLong;
    ep.Parameter[0].NumberOfValues = 1;
    ULONG quality                  = 95;
    ep.Parameter[0].Value          = &quality;

    CreateDirectoryW(kJpegDir, nullptr);

    // ── Cache bust: delete the old file first so Windows can't serve
    //    a stale cached version of lock.jpg. ───────────────────────────────
    DeleteFileW(kJpegPath);

    Status st = bmp.Save(kJpegPath, &jpegClsid, &ep);
    if (st != Ok) {
        Wh_Log(L"[LockWallpaper|explorer] Failed to save JPEG (GDI+ status=%d)",
               static_cast<int>(st));
        return false;
    }
    Wh_Log(L"[LockWallpaper|explorer] JPEG saved: %s", kJpegPath);

    // ── Signal winlogon AFTER the new JPEG is fully on disk ──────────────
    {
        HANDLE ev = CreateEventW(nullptr, TRUE, FALSE, kReadyEvt);
        if (ev) { SetEvent(ev); CloseHandle(ev); }
    }

    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Mod lifecycle
// ─────────────────────────────────────────────────────────────────────────────

BOOL Wh_ModInit() {
    g_isWinlogon = DetectWinlogon();
    Wh_Log(L"[LockWallpaper] Init v2.1 [%s]",
           g_isWinlogon ? L"winlogon/SYSTEM" : L"explorer/user");

    if (g_isWinlogon) {
        // SYSTEM context: just apply registry if JPEG is already there
        WinlogonApplyRegistry();
        return TRUE;
    }

    // Explorer context: full flow
    GdiplusStartupInput gdi;
    GdiplusStartup(&g_gdiplusToken, &gdi, nullptr);

    LoadSettings();
    PrepareAndSaveJpeg();   // saves JPEG; signals event for winlogon

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"[LockWallpaper] Uninit [%s]",
           g_isWinlogon ? L"winlogon/SYSTEM" : L"explorer/user");

    if (g_isWinlogon) {
        // Clean up HKLM so Windows reverts to default lock screen
        RegDeleteVal(HKEY_LOCAL_MACHINE, kGPOKey,  kGPOVal);
        RegDeleteVal(HKEY_LOCAL_MACHINE, kCSPKey,  kCSPPath);
        RegDeleteVal(HKEY_LOCAL_MACHINE, kCSPKey,  kCSPUrl);
        RegDeleteVal(HKEY_LOCAL_MACHINE, kCSPKey,  kCSPStat);
        DWORD_PTR res = 0;
        SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                            reinterpret_cast<LPARAM>(L"Policy"),
                            SMTO_ABORTIFHUNG, 2000, &res);
        Wh_Log(L"[LockWallpaper|winlogon] Registry cleaned, default lock screen restored.");
        return;
    }

    GdiplusShutdown(g_gdiplusToken);
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"[LockWallpaper] Settings changed [%s]",
           g_isWinlogon ? L"winlogon/SYSTEM" : L"explorer/user");

    if (g_isWinlogon) {
        // Wait for explorer to finish converting + saving the JPEG
        HANDLE ev = CreateEventW(nullptr, TRUE, FALSE, kReadyEvt);
        if (ev) {
            Wh_Log(L"[LockWallpaper|winlogon] Waiting for JPEG to be ready...");
            WaitForSingleObject(ev, 8000);   // up to 8 seconds
            CloseHandle(ev);
        }
        WinlogonApplyRegistry();
        return;
    }

    // Explorer context
    LoadSettings();
    PrepareAndSaveJpeg();
}
