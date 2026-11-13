// ==WindhawkMod==
// @id              background-dimmer
// @name            Background Dimmer
// @description     Dims unfocused windows with a solid tint, pattern or image, tuned live in a secondary configuration window
// @version         1.1.0
// @author          3DDinosaur
// @github          https://github.com/3DDinosaur
// @include         windhawk-mod-uiaccess.exe
// @include         windhawk-mod.exe
// @include         windhawk.exe
// @compilerOptions -lcomctl32 -lcomdlg32 -ldwmapi -lgdi32 -lgdiplus -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
![Background Dimmer](https://i.imgur.com/HvfnpFn.png)

# Background Dimmer

Dims inactive windows so the active one stands out, while the background stays readable. Especially useful when working with windows side by side.

The dimming layer can be a solid tint, a checkerboard or stripes at any angle, or a tiled image of your choice. Opacity, brightness, contrast, pattern size, rotation and base color are all adjustable.

Windows that run as administrator need Windhawk 2 (currently in alpha). On Windhawk 1.x the layer can't move behind them, so when one of them gets focus, the previously active window stays undimmed.

## Live configuration

The look is tuned in the mod's own **configuration window**, where every change shows up immediately on screen. Open it with the **configuration hotkey** (**Win+Ctrl+Alt+D** by default, changeable in the mod settings).

With the **tray icon** option in the mod settings, an icon in the notification area also opens it with a click, and a right-click toggles the dimming on and off.

The window also opens by itself the first time the mod runs. If the configuration hotkey is empty or already taken by another program and the tray icon is off, it opens whenever the mod settings are saved. When it opens this way, without a keypress, Windows may keep it behind other windows: look for its flashing taskbar button.

Changes made in the configuration window are saved automatically. Dimming can be turned off with the **Active** checkbox in the window, or with an optional on/off hotkey.

## How the controls work

- **Opacity**: how strongly the layer covers the background.
- **Brightness**: overall lightness of the layer.
- **Contrast**: difference between light and dark parts of a pattern. At 0 the pattern disappears into a flat tint.
- **Size**: size of the checkerboard squares or the width of the stripes, in pixels.
- **Rotation**: angle of the stripes or the checkerboard, in degrees.
- **Base color**: tints the layer. White gives neutral gray tones.

Brightness and contrast set the two pattern tones as *brightness ± contrast / 2*, so one controls how dark the layer is and the other how visible the pattern is. Near the ends of the range a tone is clipped to black or white, which keeps the pattern visible over a black base, as in the default look.

## Behavior

- By default, a dialog also dims the window that owns it. An option in the configuration window keeps them lit together instead.
- While the taskbar, Start or Alt+Tab has focus, every window is dimmed.
- A menu keeps the window that opened it lit, unless the option to dim behind menus is turned on in the configuration window. That option works with standard Windows menus; apps that draw their own menus, such as Office and web-based apps, aren't detected.
- When the desktop has focus, the layer can either hide or cover everything (option in the configuration window).
- The mod runs in its own Windhawk process and does not inject into Explorer or any other program.
- By default, the layer is left out of screenshots, recordings and screen sharing while it stays visible on your screen (Windows 10 version 2004 or later). This can be turned off in the configuration window.
- The appearance is stored per computer, so all Windows users of the same PC share it.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- configHotkey: Win+Ctrl+Alt+D
  $name: Configuration window hotkey
  $description: Opens the live configuration window. Modifiers (Ctrl, Alt, Shift, Win) and one key, joined with + (Win+Ctrl+Alt+D). If empty and the tray icon is off, the configuration window opens whenever these settings are saved.
- toggleHotkey: ""
  $name: On/off hotkey
  $description: Turns the dimming on and off, same format as above.
- trayIcon: false
  $name: Tray icon
  $description: Shows an icon in the notification area. Click it to open the configuration window, right-click it to toggle dimming on and off.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <dwmapi.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <shellapi.h>
#include <shlobj.h>
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>

// ───────────── GDI+ (flat API, only to load PNG/JPG/GIF images) ─────────────

extern "C" {
struct GdiplusStartupInputFlat {
    UINT32 GdiplusVersion;
    void* DebugEventCallback;
    BOOL SuppressBackgroundThread;
    BOOL SuppressExternalCodecs;
};
int WINAPI GdiplusStartup(ULONG_PTR* token, const GdiplusStartupInputFlat* input, void* output);
void WINAPI GdiplusShutdown(ULONG_PTR token);
int WINAPI GdipCreateBitmapFromFile(const WCHAR* filename, void** bitmap);
int WINAPI GdipCreateHBITMAPFromBitmap(void* bitmap, HBITMAP* hbm, DWORD background);
int WINAPI GdipDisposeImage(void* image);
}

// ───────────── State ─────────────

enum Pattern { PAT_SOLID, PAT_CHECKER, PAT_STRIPES, PAT_IMAGE, PAT_COUNT };
const wchar_t* const kPatternNames[PAT_COUNT] = {L"Solid", L"Checkerboard", L"Stripes", L"Image"};

constexpr int SL_OPACITY = 0, SL_BRIGHTNESS = 1, SL_CONTRAST = 2, SL_SIZE = 3, SL_ANGLE = 4, SL_COUNT = 5;

struct Appearance {
    int pattern;
    int slider[SL_COUNT];   // opacity/brightness/contrast in %, size in px, angle in degrees
    int colorRgb;           // 0xRRGGBB
    bool hideOnDesktop;
    bool hideFromCapture;
    bool dimOwners;
    bool dimMenus;
    bool active;
    wchar_t image[MAX_PATH];
};

const Appearance kDefaults = {PAT_STRIPES, {20, 0, 60, 20, 45}, 0xFFFFFF, true, true, true, false, true, L""};
const wchar_t* const kSliderKeys[SL_COUNT] = {L"opacity", L"brightness", L"contrast", L"size", L"angle"};
const int kSliderMin[SL_COUNT] = {0, 0, 0, 1, 0};
const int kSliderMax[SL_COUNT] = {100, 100, 100, 200, 180};

struct ModSettings {
    wchar_t toggleHotkey[64];
    wchar_t configHotkey[64];
    bool trayIcon;
};

Appearance g_ap;
ModSettings g_settings;

HANDLE g_thread;
HWND g_hwndMain;      // hidden window: hotkeys, timers, broadcasts
HWND g_hwndOverlay;
HWND g_hwndConfig;
HWINEVENTHOOK g_hook;
HBRUSH g_brush;
HBITMAP g_brushBitmap;
HBITMAP g_texture;
bool g_overlayVisible;
bool g_configHotkeyActive;
bool g_savePending;
HWND g_menus[8];  // open popup menus, oldest first: the first is the menu, the rest its submenus
int g_menuCount;
ULONG_PTR g_gdiplusToken;
HICON g_icon;
HICON g_trayIcon;
bool g_iconOwned, g_trayIconOwned;  // false for the shared IDI_APPLICATION icon, which must not be destroyed
bool g_trayAdded;
UINT g_wmTaskbarCreated;

constexpr wchar_t kMainClass[] = L"BackgroundDimmer_Main";
constexpr wchar_t kOverlayClass[] = L"BackgroundDimmer_Overlay";
constexpr wchar_t kConfigClass[] = L"BackgroundDimmer_Config";

enum { WM_APP_RELOAD = WM_APP + 1, WM_APP_EXIT, WM_APP_TRAY };
enum { HK_TOGGLE = 1, HK_CONFIG = 2 };
enum { TIMER_REPOSITION = 1, TIMER_SAVE = 2, TIMER_FIT = 3 };

// ───────────── Persistence ─────────────

int Clamp(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

void LoadAppearance() {
    for (int i = 0; i < SL_COUNT; i++)
        g_ap.slider[i] = Clamp(Wh_GetIntValue(kSliderKeys[i], kDefaults.slider[i]), kSliderMin[i], kSliderMax[i]);
    int pattern = Wh_GetIntValue(L"texture", -1);
    if (pattern < 0) {
        // Until 1.0 the "pattern" key held: solid, checkerboard, horizontal, vertical, diagonal stripes, image.
        static const int kOldPattern[] = {PAT_SOLID, PAT_CHECKER, PAT_STRIPES, PAT_STRIPES, PAT_STRIPES, PAT_IMAGE};
        static const int kOldAngle[] = {45, 0, 0, 90, 45, 45};
        int old = Wh_GetIntValue(L"pattern", -1);
        pattern = kDefaults.pattern;
        if (old >= 0 && old < (int)ARRAYSIZE(kOldPattern)) {
            pattern = kOldPattern[old];
            g_ap.slider[SL_ANGLE] = kOldAngle[old];
            // The old diagonal stripes measured their width along the x axis, not across the stripe.
            if (old == 4)
                g_ap.slider[SL_SIZE] = Clamp((int)(g_ap.slider[SL_SIZE] * 0.7071f + 0.5f), 1, 200);
        }
    }
    g_ap.pattern = Clamp(pattern, 0, PAT_COUNT - 1);
    g_ap.colorRgb = Wh_GetIntValue(L"color", kDefaults.colorRgb) & 0xFFFFFF;
    g_ap.hideOnDesktop = Wh_GetIntValue(L"hideOnDesktop", kDefaults.hideOnDesktop) != 0;
    g_ap.hideFromCapture = Wh_GetIntValue(L"hideFromCapture", kDefaults.hideFromCapture) != 0;
    g_ap.dimOwners = Wh_GetIntValue(L"dimOwners", kDefaults.dimOwners) != 0;
    g_ap.dimMenus = Wh_GetIntValue(L"dimMenus", kDefaults.dimMenus) != 0;
    g_ap.active = Wh_GetIntValue(L"active", kDefaults.active) != 0;
    g_ap.image[0] = L'\0';
    Wh_GetStringValue(L"image", g_ap.image, ARRAYSIZE(g_ap.image));
}

void SaveAppearance() {
    g_savePending = false;
    Wh_SetIntValue(L"texture", g_ap.pattern);
    for (int i = 0; i < SL_COUNT; i++)
        Wh_SetIntValue(kSliderKeys[i], g_ap.slider[i]);
    Wh_SetIntValue(L"color", g_ap.colorRgb);
    Wh_SetIntValue(L"hideOnDesktop", g_ap.hideOnDesktop);
    Wh_SetIntValue(L"hideFromCapture", g_ap.hideFromCapture);
    Wh_SetIntValue(L"dimOwners", g_ap.dimOwners);
    Wh_SetIntValue(L"dimMenus", g_ap.dimMenus);
    Wh_SetIntValue(L"active", g_ap.active);
    Wh_SetStringValue(L"image", g_ap.image);
}

void ScheduleSave() {
    g_savePending = true;
    SetTimer(g_hwndMain, TIMER_SAVE, 800, nullptr);
}

void CopySetting(const wchar_t* name, wchar_t* dst, size_t dstChars) {
    PCWSTR value = Wh_GetStringSetting(name);
    wcsncpy_s(dst, dstChars, value, _TRUNCATE);
    Wh_FreeStringSetting(value);
}

void LoadModSettings() {
    CopySetting(L"toggleHotkey", g_settings.toggleHotkey, ARRAYSIZE(g_settings.toggleHotkey));
    CopySetting(L"configHotkey", g_settings.configHotkey, ARRAYSIZE(g_settings.configHotkey));
    g_settings.trayIcon = Wh_GetIntSetting(L"trayIcon") != 0;
}

// ───────────── Hotkeys ─────────────

UINT KeyFromName(const wchar_t* name) {
    size_t len = wcslen(name);
    if (len == 1) {
        wchar_t c = towupper(name[0]);
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9'))
            return c;
        SHORT scan = VkKeyScanW(name[0]);
        return scan == -1 ? 0 : LOBYTE(scan);
    }
    if ((name[0] == L'F' || name[0] == L'f') && len <= 3) {
        int n = _wtoi(name + 1);
        if (n >= 1 && n <= 24)
            return VK_F1 + n - 1;
    }
    static const struct { const wchar_t* name; UINT vk; } kNames[] = {
        {L"Space", VK_SPACE}, {L"Enter", VK_RETURN}, {L"Tab", VK_TAB}, {L"Esc", VK_ESCAPE},
        {L"Escape", VK_ESCAPE}, {L"Insert", VK_INSERT}, {L"Delete", VK_DELETE}, {L"Home", VK_HOME},
        {L"End", VK_END}, {L"PageUp", VK_PRIOR}, {L"PageDown", VK_NEXT}, {L"Up", VK_UP},
        {L"Down", VK_DOWN}, {L"Left", VK_LEFT}, {L"Right", VK_RIGHT}, {L"Pause", VK_PAUSE},
    };
    for (const auto& k : kNames)
        if (_wcsicmp(name, k.name) == 0)
            return k.vk;
    return 0;
}

/// Parses "Ctrl+Alt+K" into MOD_* modifiers and a virtual key. False if empty or invalid.
bool ParseHotkey(const wchar_t* text, UINT* mods, UINT* vk) {
    *mods = 0;
    *vk = 0;
    wchar_t buf[64];
    wcsncpy_s(buf, text, _TRUNCATE);
    wchar_t* ctx = nullptr;
    for (wchar_t* tok = wcstok_s(buf, L"+", &ctx); tok; tok = wcstok_s(nullptr, L"+", &ctx)) {
        while (iswspace(*tok))
            tok++;
        wchar_t* end = tok + wcslen(tok);
        while (end > tok && iswspace(end[-1]))
            *--end = L'\0';
        if (!*tok)
            continue;
        if (_wcsicmp(tok, L"Ctrl") == 0 || _wcsicmp(tok, L"Control") == 0)
            *mods |= MOD_CONTROL;
        else if (_wcsicmp(tok, L"Alt") == 0)
            *mods |= MOD_ALT;
        else if (_wcsicmp(tok, L"Shift") == 0)
            *mods |= MOD_SHIFT;
        else if (_wcsicmp(tok, L"Win") == 0 || _wcsicmp(tok, L"Windows") == 0)
            *mods |= MOD_WIN;
        else if (*vk || !(*vk = KeyFromName(tok)))
            return false;
    }
    return *vk != 0;
}

bool RegisterHotkeyText(int id, const wchar_t* text) {
    UnregisterHotKey(g_hwndMain, id);
    if (!text[0])
        return false;
    UINT mods, vk;
    if (!ParseHotkey(text, &mods, &vk)) {
        Wh_Log(L"Invalid hotkey: %s", text);
        return false;
    }
    if (!RegisterHotKey(g_hwndMain, id, mods | MOD_NOREPEAT, vk)) {
        Wh_Log(L"Hotkey already in use: %s", text);
        return false;
    }
    return true;
}

// ───────────── Pattern brush ─────────────

int Pct255(int pct) { return (pct * 255 + 50) / 100; }

int RgbToRef(int rgb) { return ((rgb & 0xFF) << 16) | (rgb & 0xFF00) | ((rgb >> 16) & 0xFF); }

/// Base color (0xRRGGBB) scaled by a 0–255 level, as a COLORREF.
COLORREF Shade(int rgb, int level) {
    level = Clamp(level, 0, 255);
    int r = ((rgb >> 16) & 0xFF) * level / 255;
    int g = ((rgb >> 8) & 0xFF) * level / 255;
    int b = (rgb & 0xFF) * level / 255;
    return RGB(r, g, b);
}

// Copied into a compatible bitmap: GDI+ returns a DIB section, which CreatePatternBrush does not accept on every Windows version.
HBITMAP LoadTile(const wchar_t* path) {
    if (!path[0] || GetFileAttributesW(path) == INVALID_FILE_ATTRIBUTES)
        return nullptr;
    void* gpBitmap = nullptr;
    if (GdipCreateBitmapFromFile(path, &gpBitmap) != 0 || !gpBitmap)
        return nullptr;
    HBITMAP src = nullptr;
    GdipCreateHBITMAPFromBitmap(gpBitmap, &src, 0xFF000000);
    GdipDisposeImage(gpBitmap);
    if (!src)
        return nullptr;

    BITMAP bm;
    GetObject(src, sizeof(bm), &bm);
    int w = bm.bmWidth, h = bm.bmHeight < 0 ? -bm.bmHeight : bm.bmHeight;
    HDC screen = GetDC(nullptr);
    HDC m1 = CreateCompatibleDC(screen);
    HDC m2 = CreateCompatibleDC(screen);
    HBITMAP dst = CreateCompatibleBitmap(screen, w, h);
    ReleaseDC(nullptr, screen);
    HGDIOBJ o1 = SelectObject(m1, src);
    HGDIOBJ o2 = SelectObject(m2, dst);
    BitBlt(m2, 0, 0, w, h, m1, 0, 0, SRCCOPY);
    SelectObject(m1, o1);
    SelectObject(m2, o2);
    DeleteDC(m1);
    DeleteDC(m2);
    DeleteObject(src);
    return dst;
}

// Soft square wave: +1 on even bands and -1 on odd ones, with t in band widths of s px. Edges blend over
// about 1 px, so stripes at any angle stay smooth.
float Wave(float t, float s) {
    float ph = t - 2 * floorf(t * 0.5f);
    float sign = ph < 1 ? 1.f : -1.f;
    if (ph >= 1)
        ph -= 1;
    return sign * fminf(1.f, 2 * fminf(ph, 1 - ph) * s);
}

/// 8-bit DIB the size of the virtual screen with the stripes or checkerboard as palette indexes (1 = dark,
/// 255 = light). Colors live only in the palette, so brightness, contrast and color changes don't re-render it.
/// Renders into `reuse` when it already has the virtual screen's size, and returns it; otherwise creates a new DIB.
HBITMAP BuildTexture(HBITMAP reuse) {
    int w = GetSystemMetrics(SM_CXVIRTUALSCREEN), h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    void* bits = nullptr;
    HBITMAP bmp = nullptr;
    DIBSECTION ds;
    if (reuse && GetObject(reuse, sizeof(ds), &ds) == sizeof(ds) && ds.dsBm.bmWidth == w &&
        abs(ds.dsBm.bmHeight) == h) {
        GdiFlush();
        bmp = reuse;
        bits = ds.dsBm.bmBits;
    } else {
        struct {
            BITMAPINFOHEADER header;
            RGBQUAD palette[256];
        } bi = {};
        bi.header.biSize = sizeof(bi.header);
        bi.header.biWidth = w;
        bi.header.biHeight = -h;
        bi.header.biPlanes = 1;
        bi.header.biBitCount = 8;
        bi.header.biCompression = BI_RGB;
        bi.header.biClrUsed = 256;
        bmp = CreateDIBSection(nullptr, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!bmp)
            return nullptr;
    }
    int stride = (w + 3) & ~3;
    int s = g_ap.slider[SL_SIZE];
    double angle = g_ap.slider[SL_ANGLE] * 3.14159265358979 / 180;
    double c = cos(angle), sn = sin(angle);
    bool checker = g_ap.pattern == PAT_CHECKER;

    // Positions across the bands are fixed point, 1/65536 px, wrapped to one period (2s px). One period of the
    // wave is tabulated at 1/64 px (index = phase >> 10), so each pixel costs an add, a compare and a lookup.
    const uint32_t period = (uint32_t)(2 * s) << 16;
    int count = 2 * s * 64;
    signed char* wave = (signed char*)malloc(count);
    if (!wave) {
        if (bmp != reuse)
            DeleteObject(bmp);
        return nullptr;
    }
    for (int i = 0; i < count; i++)
        wave[i] = (signed char)lroundf(Wave((i + 0.5f) / 64 / s, (float)s) * 127);
    auto wrap = [period](double px) {
        double v = fmod(px * 65536, (double)period);
        return (uint32_t)(v < 0 ? v + period : v) % period;
    };
    uint32_t stepV = wrap(sn), stepU = wrap(c);

    for (int y = 0; y < h; y++) {
        BYTE* row = (BYTE*)bits + (size_t)y * stride;
        uint32_t v = wrap(y * c);
        if (!checker) {
            for (int x = 0; x < w; x++) {
                row[x] = (BYTE)(128 + wave[v >> 10]);
                if ((v += stepV) >= period)
                    v -= period;
            }
            continue;
        }
        uint32_t u = wrap(-y * sn);
        for (int x = 0; x < w; x++) {
            row[x] = (BYTE)(128 + wave[v >> 10] * wave[u >> 10] / 127);
            if ((v += stepV) >= period)
                v -= period;
            if ((u += stepU) >= period)
                u -= period;
        }
    }
    free(wave);
    return bmp;
}

void ApplyTexturePalette() {
    int k = Pct255(g_ap.slider[SL_CONTRAST]);
    int lvl = Pct255(g_ap.slider[SL_BRIGHTNESS]);
    COLORREF dark = Shade(g_ap.colorRgb, lvl - k / 2);
    COLORREF light = Shade(g_ap.colorRgb, lvl + k - k / 2);
    RGBQUAD palette[256];
    for (int i = 0; i < 256; i++) {
        int t = i ? i - 1 : 0;
        palette[i].rgbRed = (BYTE)(GetRValue(dark) + (GetRValue(light) - GetRValue(dark)) * t / 254);
        palette[i].rgbGreen = (BYTE)(GetGValue(dark) + (GetGValue(light) - GetGValue(dark)) * t / 254);
        palette[i].rgbBlue = (BYTE)(GetBValue(dark) + (GetBValue(light) - GetBValue(dark)) * t / 254);
        palette[i].rgbReserved = 0;
    }
    HDC dc = CreateCompatibleDC(nullptr);
    HGDIOBJ old = SelectObject(dc, g_texture);
    SetDIBColorTable(dc, 0, 256, palette);
    SelectObject(dc, old);
    DeleteDC(dc);
}

void BuildBrush() {
    HBITMAP tile = g_ap.pattern == PAT_IMAGE ? LoadTile(g_ap.image) : nullptr;
    HBRUSH brush = tile ? CreatePatternBrush(tile) : nullptr;
    HBITMAP texture = g_ap.pattern == PAT_CHECKER || g_ap.pattern == PAT_STRIPES ? BuildTexture(g_texture) : nullptr;
    if (!brush && !texture)
        brush = CreateSolidBrush(Shade(g_ap.colorRgb, Pct255(g_ap.slider[SL_BRIGHTNESS])));
    if (g_brush)
        DeleteObject(g_brush);
    if (g_brushBitmap)
        DeleteObject(g_brushBitmap);
    if (g_texture && g_texture != texture)
        DeleteObject(g_texture);
    g_brush = brush;
    g_brushBitmap = tile;
    g_texture = texture;
    if (g_texture)
        ApplyTexturePalette();
    if (g_hwndOverlay)
        InvalidateRect(g_hwndOverlay, nullptr, TRUE);
}

/// Brightness, contrast or color changed: only the palette is rebuilt when a texture is in use.
void UpdateColors() {
    if (!g_texture) {
        BuildBrush();
        return;
    }
    ApplyTexturePalette();
    InvalidateRect(g_hwndOverlay, nullptr, TRUE);
}

#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif

// WDA_EXCLUDEFROMCAPTURE needs Windows 10 2004 (build 19041); older builds treat it as WDA_MONITOR, which
// blacks out the whole layer in screenshots and screen sharing.
bool CaptureExclusionSupported() {
    static int supported = -1;
    if (supported < 0) {
        using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOW*);
        auto rtlGetVersion = (RtlGetVersion_t)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion");
        OSVERSIONINFOW vi = {sizeof(vi)};
        supported = rtlGetVersion && rtlGetVersion(&vi) == 0 && vi.dwBuildNumber >= 19041;
    }
    return supported != 0;
}

void ApplyCaptureExclusion() {
    if (!CaptureExclusionSupported())
        return;
    if (!SetWindowDisplayAffinity(g_hwndOverlay, g_ap.hideFromCapture ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE))
        Wh_Log(L"SetWindowDisplayAffinity failed: %u", GetLastError());
}

void ApplyOpacity() {
    SetLayeredWindowAttributes(g_hwndOverlay, 0, (BYTE)Pct255(g_ap.slider[SL_OPACITY]), LWA_ALPHA);
}

// ───────────── Overlay placement ─────────────

void FitOverlay() {
    SetWindowPos(g_hwndOverlay, nullptr, GetSystemMetrics(SM_XVIRTUALSCREEN), GetSystemMetrics(SM_YVIRTUALSCREEN),
                 GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN),
                 SWP_NOZORDER | SWP_NOACTIVATE);
}

void ShowOverlay(bool show) {
    if (show == g_overlayVisible)
        return;
    g_overlayVisible = show;
    ShowWindow(g_hwndOverlay, show ? SW_SHOWNOACTIVATE : SW_HIDE);
}

bool ClassIn(const wchar_t* cls, const wchar_t* const* list, size_t count) {
    for (size_t i = 0; i < count; i++)
        if (wcscmp(cls, list[i]) == 0)
            return true;
    return false;
}

// Transient windows that never move the layer: menus, taskbar thumbnails, the stand-in shown while switching.
bool IsIgnoredClass(const wchar_t* cls) {
    static const wchar_t* const kIgnored[] = {
        L"ForegroundStaging", L"TaskListThumbnailWnd", L"#32768", kMainClass,
    };
    return ClassIn(cls, kIgnored, ARRAYSIZE(kIgnored));
}

// Shell surfaces (taskbar, Start, Alt+Tab, notifications). They are topmost, so with the layer at the top of the
// normal band every regular window is dimmed while one of them has focus.
bool IsShellClass(const wchar_t* cls) {
    static const wchar_t* const kShell[] = {
        L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd", L"XamlExplorerHostIslandWindow", L"MultitaskingViewFrame",
        L"NotifyIconOverflowWindow", L"TopLevelWindowForOverflowXamlIsland", L"Windows.UI.Core.CoreWindow",
    };
    return ClassIn(cls, kShell, ARRAYSIZE(kShell));
}

/// Highest visible, non-minimized, non-cloaked window of the process in the normal (non-topmost) band, or nullptr.
/// Windows owned by a visible window are skipped, so their owner is returned instead (owned windows sit above it).
HWND TopWindowOfProcess(DWORD pid) {
    for (HWND w = GetTopWindow(nullptr); w; w = GetWindow(w, GW_HWNDNEXT)) {
        if (w == g_hwndOverlay || !IsWindowVisible(w) || IsIconic(w) ||
            (GetWindowLongPtrW(w, GWL_EXSTYLE) & WS_EX_TOPMOST))
            continue;
        HWND owner = GetWindow(w, GW_OWNER);
        if (owner && IsWindowVisible(owner))
            continue;
        DWORD wPid = 0;
        GetWindowThreadProcessId(w, &wPid);
        if (wPid != pid)
            continue;
        DWORD cloaked = 0;
        DwmGetWindowAttribute(w, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
        if (!cloaked)
            return w;
    }
    return nullptr;
}

// Moves the layer to the top of the non-topmost band, unless it is already there.
void RaiseOverlayToTop() {
    HWND prev = GetWindow(g_hwndOverlay, GW_HWNDPREV);
    if (!prev || (GetWindowLongPtrW(prev, GWL_EXSTYLE) & WS_EX_TOPMOST))
        return;
    SetWindowPos(g_hwndOverlay, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
}

// Puts the layer right below the window, or at the top of the normal band when the window is topmost:
// inserting the layer behind a topmost window would make it topmost too.
void PlaceBelow(HWND hwnd) {
    if (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) {
        RaiseOverlayToTop();
        return;
    }
    if (GetWindow(hwnd, GW_HWNDNEXT) == g_hwndOverlay)
        return;
    SetWindowPos(g_hwndOverlay, hwnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
}

void ForgetMenu(HWND hwnd) {
    int n = 0;
    for (int i = 0; i < g_menuCount; i++)
        if (g_menus[i] != hwnd)
            g_menus[n++] = g_menus[i];
    g_menuCount = n;
}

void TrackMenu(HWND hwnd, bool open) {
    ForgetMenu(hwnd);
    if (open && hwnd && g_menuCount < (int)ARRAYSIZE(g_menus))
        g_menus[g_menuCount++] = hwnd;
}

// Drops menus that closed without a MENUPOPUPEND. Only from the timer: right after MENUPOPUPSTART the menu
// may not be visible yet.
void PruneMenus() {
    int n = 0;
    for (int i = 0; i < g_menuCount; i++)
        if (IsWindow(g_menus[i]) && IsWindowVisible(g_menus[i]))
            g_menus[n++] = g_menus[i];
    g_menuCount = n;
}

void Reposition() {
    if (!g_hwndOverlay)
        return;
    if (!g_ap.active) {
        ShowOverlay(false);
        return;
    }
    if (g_ap.dimMenus && g_menuCount) {
        ShowOverlay(true);
        PlaceBelow(g_menus[0]);
        return;
    }
    HWND fg = GetForegroundWindow();
    if (!fg || fg == g_hwndOverlay)
        return;
    wchar_t cls[256];
    if (!GetClassNameW(fg, cls, ARRAYSIZE(cls)) || IsIgnoredClass(cls))
        return;
    if (IsShellClass(cls)) {
        ShowOverlay(true);
        RaiseOverlayToTop();
        return;
    }
    if (wcscmp(cls, L"Progman") == 0 || wcscmp(cls, L"WorkerW") == 0) {
        if (g_ap.hideOnDesktop) {
            ShowOverlay(false);
        } else {
            ShowOverlay(true);
            RaiseOverlayToTop();
        }
        return;
    }
    ShowOverlay(true);
    // With dimOwners off, below the root owner, so a dialog and the window that owns it stay lit together.
    // Stops at the last visible owner: hidden owners (WinForms ShowInTaskbar=false, Java dialogs, Excel's Find
    // dialog) aren't raised with the window they own, so the app's highest visible window stands in for them.
    HWND root = fg;
    if (!g_ap.dimOwners) {
        HWND owner;
        while ((owner = GetWindow(root, GW_OWNER)) && IsWindowVisible(owner))
            root = owner;
        if (owner) {
            DWORD pid = 0;
            GetWindowThreadProcessId(fg, &pid);
            if (HWND top = TopWindowOfProcess(pid))
                root = top;
        }
    }
    PlaceBelow(root);
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG, LONG, DWORD, DWORD) {
    if (event == EVENT_SYSTEM_MENUPOPUPSTART) {
        wchar_t cls[16];
        if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) || wcscmp(cls, L"#32768") != 0)
            return;
        TrackMenu(hwnd, true);
    } else if (event == EVENT_SYSTEM_MENUPOPUPEND) {
        TrackMenu(hwnd, false);
    } else if (event != EVENT_SYSTEM_FOREGROUND) {
        return;
    }
    Reposition();
}

// ───────────── Icon ─────────────

HICON LoadSystemIcon(int index, int size, bool* owned) {
    wchar_t path[MAX_PATH];
    GetSystemDirectoryW(path, MAX_PATH);
    wcscat_s(path, L"\\imageres.dll");
    HICON icon = nullptr;
    *owned = SUCCEEDED(SHDefExtractIconW(path, index, 0, &icon, nullptr, size)) && icon;
    return *owned ? icon : LoadIconW(nullptr, IDI_APPLICATION);
}

// ───────────── Tray icon ─────────────

void RemoveTrayIcon() {
    if (!g_trayAdded)
        return;
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = g_hwndMain;
    nid.uID = 1;
    Shell_NotifyIconW(NIM_DELETE, &nid);
    g_trayAdded = false;
}

/// Adds, updates or removes the tray icon to match the setting; the tooltip shows whether dimming is on.
void UpdateTrayIcon() {
    if (!g_settings.trayIcon) {
        RemoveTrayIcon();
        return;
    }
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = g_hwndMain;
    nid.uID = 1;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_APP_TRAY;
    nid.hIcon = g_trayIcon;
    swprintf_s(nid.szTip, L"Background Dimmer: %s\nRight-click to turn %s", g_ap.active ? L"on" : L"off",
               g_ap.active ? L"off" : L"on");
    if (g_trayAdded && Shell_NotifyIconW(NIM_MODIFY, &nid))
        return;
    g_trayAdded = Shell_NotifyIconW(NIM_ADD, &nid) != FALSE;
}

// ───────────── Configuration window ─────────────

enum {
    ID_PATTERN = 100,
    ID_SLIDER = 110,      // + SL_* index
    ID_VALUE = 120,       // + SL_* index
    ID_LABEL = 160,       // + row
    ID_SWATCH = 130,
    ID_COLOR,
    ID_IMAGE,
    ID_BROWSE,
    ID_HIDE_DESKTOP,
    ID_HIDE_CAPTURE,
    ID_ACTIVE,
    ID_RESET,
    ID_DIM_OWNERS,
    ID_DIM_MENUS,
};

struct ConfigUi {
    HFONT font;
    HBRUSH swatchBrush;
    UINT dpi;
};
ConfigUi g_ui;

HWND Ctl(int id) { return GetDlgItem(g_hwndConfig, id); }

void UpdateValueLabels() {
    for (int i = 0; i < SL_COUNT; i++) {
        wchar_t text[16];
        if (i == SL_SIZE)
            swprintf_s(text, L"%d px", g_ap.slider[i]);
        else if (i == SL_ANGLE)
            swprintf_s(text, L"%d\u00B0", g_ap.slider[i]);
        else
            swprintf_s(text, L"%d%%", g_ap.slider[i]);
        SetWindowTextW(Ctl(ID_VALUE + i), text);
    }
}

// Solid ignores contrast and size; image ignores everything but opacity.
void UpdateEnabled() {
    bool image = g_ap.pattern == PAT_IMAGE;
    bool flat = g_ap.pattern == PAT_SOLID || image;
    EnableWindow(Ctl(ID_SLIDER + SL_BRIGHTNESS), !image);
    EnableWindow(Ctl(ID_COLOR), !image);
    EnableWindow(Ctl(ID_SLIDER + SL_CONTRAST), !flat);
    EnableWindow(Ctl(ID_SLIDER + SL_SIZE), !flat);
    EnableWindow(Ctl(ID_SLIDER + SL_ANGLE), !flat);
    EnableWindow(Ctl(ID_HIDE_CAPTURE), CaptureExclusionSupported());
}

void UpdateSwatch() {
    if (g_ui.swatchBrush)
        DeleteObject(g_ui.swatchBrush);
    g_ui.swatchBrush = CreateSolidBrush(RgbToRef(g_ap.colorRgb));
    InvalidateRect(Ctl(ID_SWATCH), nullptr, TRUE);
}

void UpdateActiveLabel() {
    wchar_t text[96];
    if (g_settings.toggleHotkey[0])
        swprintf_s(text, L"Active  (%s)", g_settings.toggleHotkey);
    else
        wcscpy_s(text, L"Active");
    SetWindowTextW(Ctl(ID_ACTIVE), text);
}

void SyncControls() {
    if (!g_hwndConfig)
        return;
    SendMessageW(Ctl(ID_PATTERN), CB_SETCURSEL, g_ap.pattern, 0);
    for (int i = 0; i < SL_COUNT; i++)
        SendMessageW(Ctl(ID_SLIDER + i), TBM_SETPOS, TRUE, g_ap.slider[i]);
    SetWindowTextW(Ctl(ID_IMAGE), g_ap.image);
    CheckDlgButton(g_hwndConfig, ID_HIDE_DESKTOP, g_ap.hideOnDesktop ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(g_hwndConfig, ID_HIDE_CAPTURE, g_ap.hideFromCapture ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(g_hwndConfig, ID_DIM_OWNERS, g_ap.dimOwners ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(g_hwndConfig, ID_DIM_MENUS, g_ap.dimMenus ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(g_hwndConfig, ID_ACTIVE, g_ap.active ? BST_CHECKED : BST_UNCHECKED);
    UpdateValueLabels();
    UpdateEnabled();
    UpdateSwatch();
    UpdateActiveLabel();
}

void SetActive(bool active) {
    g_ap.active = active;
    if (g_hwndConfig)
        CheckDlgButton(g_hwndConfig, ID_ACTIVE, active ? BST_CHECKED : BST_UNCHECKED);
    Reposition();
    UpdateTrayIcon();
    ScheduleSave();
}

bool PickImage() {
    wchar_t file[MAX_PATH];
    wcscpy_s(file, g_ap.image);
    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_hwndConfig;
    ofn.lpstrFilter = L"Images (*.bmp;*.png;*.jpg;*.jpeg;*.gif)\0*.bmp;*.png;*.jpg;*.jpeg;*.gif\0All files\0*.*\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = L"Choose an image";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
    if (!GetOpenFileNameW(&ofn))
        return false;
    wcscpy_s(g_ap.image, file);
    g_ap.pattern = PAT_IMAGE;
    return true;
}

void OnPatternChanged() {
    int sel = (int)SendMessageW(Ctl(ID_PATTERN), CB_GETCURSEL, 0, 0);
    if (sel < 0)
        return;
    int previous = g_ap.pattern;
    g_ap.pattern = sel;
    if (sel == PAT_IMAGE && GetFileAttributesW(g_ap.image) == INVALID_FILE_ATTRIBUTES && !PickImage())
        g_ap.pattern = previous;
    SyncControls();
    BuildBrush();
    ScheduleSave();
}

void PickColor() {
    static COLORREF custom[16];
    CHOOSECOLORW cc = {};
    cc.lStructSize = sizeof(cc);
    cc.hwndOwner = g_hwndConfig;
    cc.rgbResult = RgbToRef(g_ap.colorRgb);
    cc.lpCustColors = custom;
    cc.Flags = CC_RGBINIT | CC_FULLOPEN;
    if (!ChooseColorW(&cc))
        return;
    g_ap.colorRgb = RgbToRef(cc.rgbResult);
    UpdateSwatch();
    UpdateColors();
    ScheduleSave();
}

void ResetDefaults() {
    wchar_t image[MAX_PATH];
    wcscpy_s(image, g_ap.image);
    g_ap = kDefaults;
    wcscpy_s(g_ap.image, image);
    SyncControls();
    ApplyOpacity();
    ApplyCaptureExclusion();
    BuildBrush();
    Reposition();
    UpdateTrayIcon();
    ScheduleSave();
}

int Scale(int v) { return MulDiv(v, (int)g_ui.dpi, 96); }

void MoveCtl(int id, int x, int y, int w, int h) {
    SetWindowPos(Ctl(id), nullptr, Scale(x), Scale(y), Scale(w), Scale(h), SWP_NOZORDER | SWP_NOACTIVATE);
}

BOOL CALLBACK SetFontProc(HWND child, LPARAM font) {
    SendMessageW(child, WM_SETFONT, (WPARAM)font, TRUE);
    return TRUE;
}

// Layout in 96-dpi pixels, scaled to the DPI of the current monitor.
const int kClientW = 440, kClientH = 454;

void LayoutConfig(UINT dpi) {
    g_ui.dpi = dpi;
    NONCLIENTMETRICSW ncm = {};
    ncm.cbSize = sizeof(ncm);
    SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0, dpi);
    HFONT font = CreateFontIndirectW(&ncm.lfMessageFont);
    EnumChildWindows(g_hwndConfig, SetFontProc, (LPARAM)font);
    if (g_ui.font)
        DeleteObject(g_ui.font);
    g_ui.font = font;

    const int lx = 14, cx = 124, row = 32;
    int y = 14;
    MoveCtl(ID_LABEL + 0, lx, y + 4, 104, 20);
    MoveCtl(ID_PATTERN, cx, y, 258, 200);
    y += row + 4;
    for (int i = 0; i < SL_COUNT; i++, y += row) {
        MoveCtl(ID_LABEL + 1 + i, lx, y + 4, 104, 20);
        MoveCtl(ID_SLIDER + i, cx - 4, y, 210, 28);
        MoveCtl(ID_VALUE + i, cx + 208, y + 4, 50, 20);
    }
    MoveCtl(ID_LABEL + 1 + SL_COUNT, lx, y + 4, 104, 20);
    MoveCtl(ID_SWATCH, cx, y + 2, 44, 22);
    MoveCtl(ID_COLOR, cx + 52, y, 96, 26);
    y += row + 2;
    MoveCtl(ID_LABEL + 2 + SL_COUNT, lx, y + 4, 104, 20);
    MoveCtl(ID_IMAGE, cx, y + 1, 220, 24);
    MoveCtl(ID_BROWSE, cx + 226, y, 32, 26);
    y += row + 8;
    MoveCtl(ID_HIDE_DESKTOP, lx, y, kClientW - 2 * lx, 22);
    y += 26;
    MoveCtl(ID_HIDE_CAPTURE, lx, y, kClientW - 2 * lx, 22);
    y += 26;
    MoveCtl(ID_DIM_OWNERS, lx, y, kClientW - 2 * lx, 22);
    y += 26;
    MoveCtl(ID_DIM_MENUS, lx, y, kClientW - 2 * lx, 22);
    y += 26;
    MoveCtl(ID_ACTIVE, lx, y, kClientW - 2 * lx, 22);
    y += 38;
    MoveCtl(ID_RESET, lx, y, 140, 28);
    MoveCtl(IDCANCEL, kClientW - 14 - 92, y, 92, 28);
}

void ResizeConfigForDpi(UINT dpi, const RECT* suggested) {
    RECT rc = {0, 0, MulDiv(kClientW, (int)dpi, 96), MulDiv(kClientH, (int)dpi, 96)};
    DWORD style = (DWORD)GetWindowLongPtrW(g_hwndConfig, GWL_STYLE);
    DWORD exStyle = (DWORD)GetWindowLongPtrW(g_hwndConfig, GWL_EXSTYLE);
    AdjustWindowRectExForDpi(&rc, style, FALSE, exStyle, dpi);
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    int x, y;
    if (suggested) {
        x = suggested->left;
        y = suggested->top;
    } else {
        POINT pt;
        GetCursorPos(&pt);
        MONITORINFO mi = {};
        mi.cbSize = sizeof(mi);
        GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi);
        x = mi.rcWork.left + (mi.rcWork.right - mi.rcWork.left - w) / 2;
        y = mi.rcWork.top + (mi.rcWork.bottom - mi.rcWork.top - h) / 2;
    }
    SetWindowPos(g_hwndConfig, nullptr, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
    // The window's current DPI, not the argument: the move above may have sent a nested WM_DPICHANGED.
    LayoutConfig(GetDpiForWindow(g_hwndConfig));
}

HWND AddCtl(const wchar_t* cls, const wchar_t* text, DWORD style, int id, DWORD exStyle = 0) {
    return CreateWindowExW(exStyle, cls, text, WS_CHILD | WS_VISIBLE | style, 0, 0, 0, 0, g_hwndConfig,
                           (HMENU)(INT_PTR)id, GetModuleHandleW(nullptr), nullptr);
}

void CreateConfigControls() {
    const wchar_t* labels[] = {L"Pattern", L"Opacity", L"Brightness", L"Contrast", L"Size", L"Rotation",
                               L"Base color", L"Image"};
    for (int i = 0; i < (int)ARRAYSIZE(labels); i++)
        AddCtl(L"STATIC", labels[i], SS_LEFT, ID_LABEL + i);

    HWND combo = AddCtl(WC_COMBOBOXW, L"", CBS_DROPDOWNLIST | WS_TABSTOP | WS_VSCROLL, ID_PATTERN);
    for (const wchar_t* name : kPatternNames)
        SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)name);

    for (int i = 0; i < SL_COUNT; i++) {
        HWND tb = AddCtl(TRACKBAR_CLASSW, L"", TBS_HORZ | TBS_NOTICKS | WS_TABSTOP, ID_SLIDER + i);
        SendMessageW(tb, TBM_SETRANGEMIN, FALSE, kSliderMin[i]);
        SendMessageW(tb, TBM_SETRANGEMAX, FALSE, kSliderMax[i]);
        SendMessageW(tb, TBM_SETPAGESIZE, 0, 5);
        AddCtl(L"STATIC", L"", SS_RIGHT, ID_VALUE + i);
    }

    AddCtl(L"STATIC", L"", SS_SUNKEN, ID_SWATCH);
    AddCtl(L"BUTTON", L"Choose…", BS_PUSHBUTTON | WS_TABSTOP, ID_COLOR);
    AddCtl(L"EDIT", L"", ES_AUTOHSCROLL | ES_READONLY, ID_IMAGE, WS_EX_CLIENTEDGE);
    AddCtl(L"BUTTON", L"…", BS_PUSHBUTTON | WS_TABSTOP, ID_BROWSE);
    AddCtl(L"BUTTON", L"Don't dim while the desktop has focus", BS_AUTOCHECKBOX | WS_TABSTOP, ID_HIDE_DESKTOP);
    AddCtl(L"BUTTON", L"Hide the layer from screenshots and screen sharing", BS_AUTOCHECKBOX | WS_TABSTOP,
           ID_HIDE_CAPTURE);
    AddCtl(L"BUTTON", L"Dim a window while one of its dialogs has focus", BS_AUTOCHECKBOX | WS_TABSTOP, ID_DIM_OWNERS);
    AddCtl(L"BUTTON", L"Dim everything behind an open menu", BS_AUTOCHECKBOX | WS_TABSTOP, ID_DIM_MENUS);
    AddCtl(L"BUTTON", L"Active", BS_AUTOCHECKBOX | WS_TABSTOP, ID_ACTIVE);
    AddCtl(L"BUTTON", L"Reset to defaults", BS_PUSHBUTTON | WS_TABSTOP, ID_RESET);
    AddCtl(L"BUTTON", L"Close", BS_DEFPUSHBUTTON | WS_TABSTOP, IDCANCEL);
}

LRESULT CALLBACK ConfigProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_HSCROLL: {
            HWND tb = (HWND)lParam;
            for (int i = 0; i < SL_COUNT; i++) {
                if (tb != Ctl(ID_SLIDER + i))
                    continue;
                int pos = (int)SendMessageW(tb, TBM_GETPOS, 0, 0);
                if (pos == g_ap.slider[i])
                    return 0;
                g_ap.slider[i] = pos;
                UpdateValueLabels();
                if (i == SL_OPACITY)
                    ApplyOpacity();
                else if (i == SL_BRIGHTNESS || i == SL_CONTRAST)
                    UpdateColors();
                else
                    BuildBrush();
                ScheduleSave();
                return 0;
            }
            break;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_PATTERN:
                    if (HIWORD(wParam) == CBN_SELCHANGE)
                        OnPatternChanged();
                    return 0;
                case ID_COLOR:
                    PickColor();
                    return 0;
                case ID_BROWSE:
                    if (PickImage()) {
                        SyncControls();
                        BuildBrush();
                        ScheduleSave();
                    }
                    return 0;
                case ID_HIDE_DESKTOP:
                    g_ap.hideOnDesktop = IsDlgButtonChecked(hwnd, ID_HIDE_DESKTOP) == BST_CHECKED;
                    Reposition();
                    ScheduleSave();
                    return 0;
                case ID_HIDE_CAPTURE:
                    g_ap.hideFromCapture = IsDlgButtonChecked(hwnd, ID_HIDE_CAPTURE) == BST_CHECKED;
                    ApplyCaptureExclusion();
                    ScheduleSave();
                    return 0;
                case ID_DIM_OWNERS:
                    g_ap.dimOwners = IsDlgButtonChecked(hwnd, ID_DIM_OWNERS) == BST_CHECKED;
                    Reposition();
                    ScheduleSave();
                    return 0;
                case ID_DIM_MENUS:
                    g_ap.dimMenus = IsDlgButtonChecked(hwnd, ID_DIM_MENUS) == BST_CHECKED;
                    Reposition();
                    ScheduleSave();
                    return 0;
                case ID_ACTIVE:
                    SetActive(IsDlgButtonChecked(hwnd, ID_ACTIVE) == BST_CHECKED);
                    return 0;
                case ID_RESET:
                    ResetDefaults();
                    return 0;
                case IDCANCEL:
                    DestroyWindow(hwnd);
                    return 0;
            }
            break;
        case WM_CTLCOLORSTATIC:
            if ((HWND)lParam == Ctl(ID_SWATCH))
                return (LRESULT)g_ui.swatchBrush;
            break;
        case WM_DPICHANGED:
            ResizeConfigForDpi(HIWORD(wParam), (const RECT*)lParam);
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            g_hwndConfig = nullptr;
            if (g_ui.font)
                DeleteObject(g_ui.font);
            if (g_ui.swatchBrush)
                DeleteObject(g_ui.swatchBrush);
            g_ui = {};
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

/// Opens the configuration window, or brings it to the front. Closing destroys it rather than hiding it.
void OpenConfig() {
    if (g_hwndConfig) {
        ShowWindow(g_hwndConfig, SW_RESTORE);
        SetForegroundWindow(g_hwndConfig);
        return;
    }
    // Created on the monitor under the cursor, so it starts with that monitor's DPI.
    POINT pt;
    GetCursorPos(&pt);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi);
    g_hwndConfig = CreateWindowExW(WS_EX_CONTROLPARENT | WS_EX_DLGMODALFRAME, kConfigClass, L"Background Dimmer",
                                   WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, mi.rcWork.left, mi.rcWork.top, 100, 100,
                                   nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!g_hwndConfig)
        return;
    CreateConfigControls();
    ResizeConfigForDpi(GetDpiForWindow(g_hwndConfig), nullptr);
    SyncControls();
    ShowWindow(g_hwndConfig, SW_SHOW);
    SetForegroundWindow(g_hwndConfig);
}

// ───────────── Main hidden window ─────────────

void ApplyModSettings(bool fromSettingsChange) {
    RegisterHotkeyText(HK_TOGGLE, g_settings.toggleHotkey);
    g_configHotkeyActive = RegisterHotkeyText(HK_CONFIG, g_settings.configHotkey);
    if (g_hwndConfig)
        UpdateActiveLabel();
    UpdateTrayIcon();
    if (fromSettingsChange && !g_configHotkeyActive && !g_trayAdded)
        OpenConfig();
}

void Shutdown() {
    if (g_savePending)
        SaveAppearance();
    if (g_hwndConfig)
        DestroyWindow(g_hwndConfig);
    if (g_hook)
        UnhookWinEvent(g_hook);
    g_hook = nullptr;
    UnregisterHotKey(g_hwndMain, HK_TOGGLE);
    UnregisterHotKey(g_hwndMain, HK_CONFIG);
    RemoveTrayIcon();
    if (g_hwndOverlay)
        DestroyWindow(g_hwndOverlay);
    g_hwndOverlay = nullptr;
    DestroyWindow(g_hwndMain);
}

LRESULT CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == g_wmTaskbarCreated && msg) {
        // Explorer restarted and the notification area is empty again.
        g_trayAdded = false;
        UpdateTrayIcon();
        return 0;
    }
    switch (msg) {
        case WM_TIMER:
            if (wParam == TIMER_REPOSITION) {
                PruneMenus();
                Reposition();
            } else if (wParam == TIMER_SAVE) {
                KillTimer(hwnd, TIMER_SAVE);
                SaveAppearance();
            } else if (wParam == TIMER_FIT) {
                KillTimer(hwnd, TIMER_FIT);
                FitOverlay();
                if (g_texture)
                    BuildBrush();
            }
            return 0;
        case WM_HOTKEY:
            if (wParam == HK_TOGGLE)
                SetActive(!g_ap.active);
            else if (wParam == HK_CONFIG)
                OpenConfig();
            return 0;
        case WM_DISPLAYCHANGE:
            SetTimer(hwnd, TIMER_FIT, 500, nullptr);
            return 0;
        case WM_APP_TRAY:
            if (lParam == WM_LBUTTONUP)
                OpenConfig();
            else if (lParam == WM_RBUTTONUP)
                SetActive(!g_ap.active);
            return 0;
        case WM_APP_RELOAD:
            LoadModSettings();
            ApplyModSettings(true);
            return 0;
        case WM_APP_EXIT:
            Shutdown();
            return 0;
        case WM_DESTROY:
            g_hwndMain = nullptr;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK OverlayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_ERASEBKGND: {
            HDC dc = (HDC)wParam;
            RECT rc;
            GetClientRect(hwnd, &rc);
            if (g_texture) {
                HDC mem = CreateCompatibleDC(dc);
                HGDIOBJ old = SelectObject(mem, g_texture);
                BitBlt(dc, 0, 0, rc.right, rc.bottom, mem, 0, 0, SRCCOPY);
                SelectObject(mem, old);
                DeleteDC(mem);
            } else if (g_brush) {
                FillRect(dc, &rc, g_brush);
            }
            return 1;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ───────────── Thread ─────────────

void RegisterClasses(HINSTANCE inst) {
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.hInstance = inst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);

    wc.lpfnWndProc = MainProc;
    wc.lpszClassName = kMainClass;
    RegisterClassExW(&wc);

    wc.lpfnWndProc = OverlayProc;
    wc.lpszClassName = kOverlayClass;
    RegisterClassExW(&wc);

    wc.lpfnWndProc = ConfigProc;
    wc.lpszClassName = kConfigClass;
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.hIcon = g_icon;
    RegisterClassExW(&wc);
}

DWORD WINAPI ThreadProc(void*) {
    INITCOMMONCONTROLSEX icc = {sizeof(icc), ICC_BAR_CLASSES | ICC_STANDARD_CLASSES};
    InitCommonControlsEx(&icc);
    GdiplusStartupInputFlat gdiplusInput = {1, nullptr, FALSE, FALSE};
    if (GdiplusStartup(&g_gdiplusToken, &gdiplusInput, nullptr) != 0)
        Wh_Log(L"GdiplusStartup failed: the Image pattern falls back to a solid tint");

    UINT systemDpi = GetDpiForSystem();
    g_icon = LoadSystemIcon(201, GetSystemMetricsForDpi(SM_CXICON, systemDpi), &g_iconOwned);
    g_trayIcon = LoadSystemIcon(201, GetSystemMetricsForDpi(SM_CXSMICON, systemDpi), &g_trayIconOwned);
    g_wmTaskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");

    LoadAppearance();

    HINSTANCE inst = GetModuleHandleW(nullptr);
    RegisterClasses(inst);

    // A regular (not message-only) window, so it receives the WM_DISPLAYCHANGE broadcast.
    g_hwndMain = CreateWindowExW(WS_EX_TOOLWINDOW, kMainClass, L"Background Dimmer", WS_POPUP, 0, 0, 0, 0, nullptr,
                                 nullptr, inst, nullptr);
    g_hwndOverlay = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
                                    kOverlayClass, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, inst, nullptr);
    // Lets Explorer's TaskbarCreated broadcast through UIPI when the host runs with UIAccess.
    if (g_hwndMain)
        ChangeWindowMessageFilterEx(g_hwndMain, g_wmTaskbarCreated, MSGFLT_ALLOW, nullptr);
    // Read after g_hwndMain exists, so a settings change saved meanwhile is queued as WM_APP_RELOAD, not lost.
    LoadModSettings();
    if (g_hwndMain && g_hwndOverlay) {
        BuildBrush();
        ApplyOpacity();
        ApplyCaptureExclusion();
        FitOverlay();
        g_hook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_MENUPOPUPEND, nullptr, WinEventProc, 0, 0,
                                 WINEVENT_OUTOFCONTEXT);
        // Safety net for z-order changes no WinEvent reports, such as the active window being lowered or made
        // topmost without the foreground window changing.
        SetTimer(g_hwndMain, TIMER_REPOSITION, 250, nullptr);
        ApplyModSettings(false);
        Reposition();
        if (!Wh_GetIntValue(L"initialized", 0)) {
            Wh_SetIntValue(L"initialized", 1);
            OpenConfig();
        }

        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            if (g_hwndConfig && IsDialogMessageW(g_hwndConfig, &msg))
                continue;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    } else {
        Wh_Log(L"Window creation failed: %u", GetLastError());
    }

    if (g_brush)
        DeleteObject(g_brush);
    if (g_brushBitmap)
        DeleteObject(g_brushBitmap);
    if (g_texture)
        DeleteObject(g_texture);
    g_brush = nullptr;
    g_brushBitmap = nullptr;
    g_texture = nullptr;
    if (g_icon && g_iconOwned)
        DestroyIcon(g_icon);
    if (g_trayIcon && g_trayIconOwned)
        DestroyIcon(g_trayIcon);
    UnregisterClassW(kConfigClass, inst);
    UnregisterClassW(kOverlayClass, inst);
    UnregisterClassW(kMainClass, inst);
    GdiplusShutdown(g_gdiplusToken);
    return 0;
}

// ───────────── Tool mod callbacks ─────────────

BOOL WhTool_ModInit() {
    g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
    return g_thread != nullptr;
}

void WhTool_ModSettingsChanged() {
    if (g_hwndMain)
        PostMessageW(g_hwndMain, WM_APP_RELOAD, 0, 0);
}

void WhTool_ModUninit() {
    if (g_hwndMain)
        PostMessageW(g_hwndMain, WM_APP_EXIT, 0, 0);
    if (g_thread) {
        WaitForSingleObject(g_thread, 5000);
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
