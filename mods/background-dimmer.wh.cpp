// ==WindhawkMod==
// @id              background-dimmer
// @name            Background Dimmer
// @description     Dims unfocused windows with a solid tint, pattern or image, tuned live in a secondary configuration window
// @version         1.2.0
// @author          3DDinosaur
// @github          https://github.com/3DDinosaur
// @include         windhawk-mod-uiaccess.exe
// @include         windhawk-mod.exe
// @include         windhawk.exe
// @compilerOptions -lcomctl32 -lcomdlg32 -ldwmapi -lgdi32 -lgdiplus -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
![Background Dimmer](https://i.imgur.com/sWClZXZ.png)

# Background Dimmer

Dims inactive windows so the one you are working on stands out, with the background anywhere from readable to fully covered. Especially useful with side-by-side app layouts.

The dimming layer can be a solid tint, checkerboard, stripes at any angle, or a tiled image of your own as a custom pattern. Opacity, brightness, contrast, pattern size / image scale, rotation and base color are all adjustable, plus a fading out animation timing for the window that loses focus.

Apps that run as administrator need Windhawk 2 (currently in alpha). On Windhawk 1.x the layer can't move behind them, so when one of them gets focus, the previously active window stays undimmed.

## Live configuration

The look is tuned in the mod's secondary **Configuration window**, where every change shows up immediately on screen. It automatically opens on the first run, then, with a configured hotkey or the system tray icon.

**Tray icon** option enables it in the notification area. Clicking (de)activates dimming, right-click opens the configuration window.

Changes made in the configuration window are saved automatically. Dimming can be turned off with the **Active** checkbox in the window, optional on/off hotkey, or tray icon clicking.

## How the controls work

- **Opacity**: how strongly the layer covers the background.
- **Brightness**: overall lightness of the layer.
- **Contrast**: difference between light and dark parts of a pattern. At 0 the pattern disappears into a flat tint.
- **Size**: size of the checkerboard squares or the width of the stripes, in pixels.
- **Scale**: % size of the 'Image / Custom pattern' option.
- **Rotation**: angle of the stripes or the checkerboard, in degrees.
- **Fade**: fade animation, in milliseconds, for the window that loses focus.
- **Base color**: the layer tint. White gives neutral gray tones.
- **Image**: file used by 'Image / Custom pattern'.

Brightness and contrast set the two pattern tones as *brightness ± contrast / 2*, so one controls how dark the layer is and the other how visible the pattern, to give the widest combinations possible. On an image, brightness and contrast work the same way around its mid gray: at 50% brightness and 100% contrast, with a white base color, the image shows unchanged. Selecting 'Image / Custom pattern' always resets to these values to be tuned from. Switching back to another pattern restores the default brightness, contrast and base color.

## Behavior

- Whatever window has focus stays lit; everything behind it is dimmed.
- Dialogs (windows with title bar) and menus (without) can be toggled for parent-window background dimming. Some apps, like web-based ones, draw their own elements, which can't be detected.
- The taskbar can be dimmed too, lighting up whenever it, Start or Alt+Tab has focus.
- When the desktop has focus, the layer can either hide or cover everything.
- By default, the layer is left out of screenshots, recordings and screen sharing while it stays visible on your screen (Windows 10 version 2004 or later).
- Per monitor dimming, remembered by device and skipped by default on new secondary screens connections.
- For window exclusion use some 'always on top' solution like this [Windhawk's mod](https://windhawk.net/mods/always-on-top) or the PowerToys feature, as dynamic cutouts would be too expensive to run.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- configHotkey: Win+Ctrl+Shift+D
  $name: Configuration window hotkey
  $description: Opens the live configuration window. Win + Ctrl, Alt, Shift + key, like Win+Ctrl+Shift+D.
- toggleHotkey: ""
  $name: On/off hotkey
  $description: Toggles dimming on and off, same format as above (Win+Shift+D).
- trayIcon: false
  $name: Tray icon
  $description: Shows an icon in the notification area. Click to toggle dimming on and off, secondary-click to open the configuration window.
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
const wchar_t* const kPatternNames[PAT_COUNT] = {L"Solid", L"Checkerboard", L"Stripes", L"Image / Custom pattern"};

constexpr int SL_OPACITY = 0, SL_BRIGHTNESS = 1, SL_CONTRAST = 2, SL_SIZE = 3, SL_ANGLE = 4, SL_SCALE = 5, SL_FADE = 6,
              SL_COUNT = 7;

struct Appearance {
    int pattern;
    int slider[SL_COUNT];   // opacity/brightness/contrast/scale in %, size in px, angle in degrees, fade in ms
    int colorRgb;           // 0xRRGGBB
    bool hideOnDesktop;
    bool hideFromCapture;
    bool dimOwners;
    bool dimMenus;
    bool dimTaskbar;
    bool active;
    wchar_t image[MAX_PATH];
};

// configuration window defaults
const Appearance kDefaults = {PAT_STRIPES, {20, 0, 60, 20, 45, 100, 300}, 0xFFFFFF, true, true, true, false, false, true, L""};
const wchar_t* const kSliderKeys[SL_COUNT] = {L"opacity", L"brightness", L"contrast", L"size",
                                              L"angle",   L"scale",      L"fade"};
const int kSliderMin[SL_COUNT] = {0, 0, 0, 1, 0, 1, 0};
const int kSliderMax[SL_COUNT] = {100, 100, 100, 200, 180, 400, 1000};

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
HBITMAP g_imageSource;  // the Image / Custom pattern file as loaded, kept to rebuild the tile without rereading it
wchar_t g_imageSourcePath[MAX_PATH];
HWND g_fadeLayer;
bool g_fading;
float g_fadeProgress;   // 0..1
ULONGLONG g_fadeLastStep;
HWND g_fadeTarget;      // where the overlay goes when the fade ends: below this window, or HWND_TOP for the top
HWND g_fadeForeground;  // foreground window that started the fade
HWND g_pendingForeground;  // focused window still waiting to show before the overlay moves
ULONGLONG g_pendingSince;
HWND g_pingTarget;  // focused window asked to answer a WM_NULL, to know it finished loading
bool g_pingAnswered;
HWND g_waitedOut;   // focused window that already used up the 2 s wait
HWND g_taskbarLayers[8];   // one per taskbar, up to one per monitor
bool g_fastPoll;           // the reposition timer runs every 30 ms instead of 250 ms
ULONGLONG g_fastPollUntil;
bool g_overlayVisible;
bool g_shellFocused;
bool g_savePending;
HWND g_menus[8];  // open popup menus, oldest first: the first is the menu, the rest its submenus
int g_menuCount;
ULONG_PTR g_gdiplusToken;
HICON g_icon;
HICON g_trayIcon;
bool g_iconOwned, g_trayIconOwned;  // false for the shared IDI_APPLICATION icon, which must not be destroyed
bool g_trayAdded;
UINT g_wmTaskbarCreated;

// Connected monitors, left to right. Each is remembered by its device path, so rearranging or reconnecting
// monitors keeps which ones are dimmed.
constexpr int kMaxMonitors = 8;
struct Monitor {
    HMONITOR handle;
    RECT rc;
    bool primary;
    bool dim;
    wchar_t name[64];   // the monitor's own name ("DELL U2415"), or "Display" when Windows has none
    wchar_t key[16];    // storage key: "dim" and a hash of the device path
};
Monitor g_monitors[kMaxMonitors];
int g_monitorCount;
bool g_allMonitorsDim = true;  // nothing to cut out of the layers

constexpr wchar_t kMainClass[] = L"BackgroundDimmer_Main";
constexpr wchar_t kOverlayClass[] = L"BackgroundDimmer_Overlay";
constexpr wchar_t kConfigClass[] = L"BackgroundDimmer_Config";

enum { WM_APP_RELOAD = WM_APP + 1, WM_APP_EXIT, WM_APP_TRAY };
enum { HK_TOGGLE = 1, HK_CONFIG = 2 };
enum { TIMER_REPOSITION = 1, TIMER_SAVE = 2, TIMER_FIT = 3, TIMER_FADE = 4, TIMER_IMAGE_PATH = 5, TIMER_VALUES = 6,
       TIMER_PENDING = 7 };

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
    // Until 1.1 the image ignored brightness, contrast and color; these values keep showing it unchanged.
    if (g_ap.pattern == PAT_IMAGE && Wh_GetIntValue(L"scale", -1) < 0) {
        g_ap.slider[SL_BRIGHTNESS] = 50;
        g_ap.slider[SL_CONTRAST] = 100;
        g_ap.colorRgb = 0xFFFFFF;
    }
    g_ap.hideOnDesktop = Wh_GetIntValue(L"hideOnDesktop", kDefaults.hideOnDesktop) != 0;
    g_ap.hideFromCapture = Wh_GetIntValue(L"hideFromCapture", kDefaults.hideFromCapture) != 0;
    g_ap.dimOwners = Wh_GetIntValue(L"dimOwners", kDefaults.dimOwners) != 0;
    g_ap.dimMenus = Wh_GetIntValue(L"dimMenus", kDefaults.dimMenus) != 0;
    g_ap.dimTaskbar = Wh_GetIntValue(L"dimTaskbar", kDefaults.dimTaskbar) != 0;
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
    Wh_SetIntValue(L"dimTaskbar", g_ap.dimTaskbar);
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

/// Parses "Win+Ctrl+K" into MOD_* modifiers and a virtual key. False if empty, invalid, or not Win plus another
/// modifier: without Win it would take over typing keys (Shift+letter), and Windows uses most Win+letter keys.
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
    return *vk != 0 && (*mods & MOD_WIN) && (*mods & (MOD_CONTROL | MOD_ALT | MOD_SHIFT));
}

void RegisterHotkeyText(int id, const wchar_t* text) {
    UnregisterHotKey(g_hwndMain, id);
    if (!text[0])
        return;
    UINT mods, vk;
    if (!ParseHotkey(text, &mods, &vk))
        Wh_Log(L"Invalid hotkey (needs Win, Ctrl/Alt/Shift and one key): %s", text);
    else if (!RegisterHotKey(g_hwndMain, id, mods | MOD_NOREPEAT, vk))
        Wh_Log(L"Hotkey already in use: %s", text);
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

/// The Image / Custom pattern file, loaded once per path through GDI+ (a 32-bit DIB section), or nullptr.
HBITMAP ImageSource() {
    if (g_imageSource && _wcsicmp(g_imageSourcePath, g_ap.image) == 0)
        return g_imageSource;
    if (g_imageSource)
        DeleteObject(g_imageSource);
    g_imageSource = nullptr;
    g_imageSourcePath[0] = L'\0';
    if (!g_ap.image[0] || GetFileAttributesW(g_ap.image) == INVALID_FILE_ATTRIBUTES)
        return nullptr;
    void* gpBitmap = nullptr;
    if (GdipCreateBitmapFromFile(g_ap.image, &gpBitmap) != 0 || !gpBitmap)
        return nullptr;
    GdipCreateHBITMAPFromBitmap(gpBitmap, &g_imageSource, 0xFF000000);
    GdipDisposeImage(gpBitmap);
    if (g_imageSource)
        wcscpy_s(g_imageSourcePath, g_ap.image);
    return g_imageSource;
}

/// Pattern tile from the image: scaled, then brightness/contrast applied around mid gray and tinted by the base
/// color, the same way the stripes and checkerboard tones are built.
// Copied into a compatible bitmap at the end: CreatePatternBrush does not accept a DIB section on every Windows
// version.
HBITMAP BuildImageTile(HBITMAP src) {
    BITMAP bm;
    if (!GetObject(src, sizeof(bm), &bm))
        return nullptr;
    int w = bm.bmWidth, h = abs(bm.bmHeight);
    // Both sides by the same factor, capped so the longer side is at most 4096 px.
    int scale = g_ap.slider[SL_SCALE];
    int longer = w > h ? w : h;
    if ((long long)longer * scale > 4096LL * 100)
        scale = (int)(4096LL * 100 / longer);
    int tw = Clamp(MulDiv(w, scale, 100), 1, 4096);
    int th = Clamp(MulDiv(h, scale, 100), 1, 4096);

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = tw;
    bi.bmiHeader.biHeight = -th;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP scaled = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!scaled)
        return nullptr;

    HDC screen = GetDC(nullptr);
    HDC m1 = CreateCompatibleDC(screen);
    HDC m2 = CreateCompatibleDC(screen);
    HBITMAP tile = CreateCompatibleBitmap(screen, tw, th);
    ReleaseDC(nullptr, screen);
    if (!tile) {
        DeleteDC(m1);
        DeleteDC(m2);
        DeleteObject(scaled);
        return nullptr;
    }
    HGDIOBJ o1 = SelectObject(m1, src);
    HGDIOBJ o2 = SelectObject(m2, scaled);
    SetStretchBltMode(m2, HALFTONE);
    SetBrushOrgEx(m2, 0, 0, nullptr);
    StretchBlt(m2, 0, 0, tw, th, m1, 0, 0, w, h, SRCCOPY);
    GdiFlush();

    int k = Pct255(g_ap.slider[SL_CONTRAST]);
    int lvl = Pct255(g_ap.slider[SL_BRIGHTNESS]);
    int tint[3] = {g_ap.colorRgb & 0xFF, (g_ap.colorRgb >> 8) & 0xFF, (g_ap.colorRgb >> 16) & 0xFF};
    BYTE* px = (BYTE*)bits;
    for (size_t i = 0, n = (size_t)tw * th; i < n; i++, px += 4)
        for (int c = 0; c < 3; c++)
            px[c] = (BYTE)(Clamp(lvl + (px[c] - 128) * k / 255, 0, 255) * tint[c] / 255);

    SelectObject(m1, tile);
    BitBlt(m1, 0, 0, tw, th, m2, 0, 0, SRCCOPY);
    SelectObject(m1, o1);
    SelectObject(m2, o2);
    DeleteDC(m1);
    DeleteDC(m2);
    DeleteObject(scaled);
    return tile;
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

/// Every layer window that exists: the overlay, the fade layers and the taskbar layers. Returns the count.
int AllLayers(HWND* out) {
    int n = 0;
    out[n++] = g_hwndOverlay;
    out[n++] = g_fadeLayer;
    for (HWND w : g_taskbarLayers)
        out[n++] = w;
    int kept = 0;
    for (int i = 0; i < n; i++)
        if (out[i])
            out[kept++] = out[i];
    return kept;
}

void InvalidateLayers() {
    HWND layers[2 + ARRAYSIZE(g_taskbarLayers)];
    for (int i = 0, n = AllLayers(layers); i < n; i++)
        InvalidateRect(layers[i], nullptr, TRUE);
}

void BuildBrush() {
    HBITMAP src = g_ap.pattern == PAT_IMAGE ? ImageSource() : nullptr;
    HBITMAP tile = src ? BuildImageTile(src) : nullptr;
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
    InvalidateLayers();
}

/// Brightness, contrast or color changed: only the palette is rebuilt when a texture is in use.
void UpdateColors() {
    if (!g_texture) {
        BuildBrush();
        return;
    }
    ApplyTexturePalette();
    InvalidateLayers();
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

void ApplyCaptureExclusionTo(HWND hwnd) {
    if (!CaptureExclusionSupported())
        return;
    if (!SetWindowDisplayAffinity(hwnd, g_ap.hideFromCapture ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE))
        Wh_Log(L"SetWindowDisplayAffinity failed: %u", GetLastError());
}

void ApplyCaptureExclusion() {
    HWND layers[2 + ARRAYSIZE(g_taskbarLayers)];
    for (int i = 0, n = AllLayers(layers); i < n; i++)
        ApplyCaptureExclusionTo(layers[i]);
}

/// Opacity for the overlay and the taskbar layers; the fade layers take theirs from each fade step.
void ApplyOpacity() {
    BYTE alpha = (BYTE)Pct255(g_ap.slider[SL_OPACITY]);
    SetLayeredWindowAttributes(g_hwndOverlay, 0, alpha, LWA_ALPHA);
    for (HWND w : g_taskbarLayers)
        if (w)
            SetLayeredWindowAttributes(w, 0, alpha, LWA_ALPHA);
}

/// A click-through layer window that paints the current pattern, hidden and fully transparent until placed.
HWND CreateLayer(bool topmost) {
    HWND hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW |
                                    (topmost ? WS_EX_TOPMOST : 0),
                                kOverlayClass, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, GetModuleHandleW(nullptr),
                                nullptr);
    if (hwnd) {
        SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
        ApplyCaptureExclusionTo(hwnd);
    }
    return hwnd;
}

bool IsTopmost(HWND hwnd) { return (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0; }

// ───────────── Monitors ─────────────

/// The monitor's name and device path from the display configuration, for the GDI device it shows
/// ("\\.\DISPLAY1"). False when Windows doesn't report them (remote sessions, some drivers).
bool MonitorIdentity(const wchar_t* gdiDevice, wchar_t* name, size_t nameChars, wchar_t* path, size_t pathChars) {
    UINT32 pathCount = 0, modeCount = 0;
    if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount, &modeCount) != ERROR_SUCCESS)
        return false;
    auto* paths = (DISPLAYCONFIG_PATH_INFO*)calloc(pathCount, sizeof(DISPLAYCONFIG_PATH_INFO));
    auto* modes = (DISPLAYCONFIG_MODE_INFO*)calloc(modeCount, sizeof(DISPLAYCONFIG_MODE_INFO));
    bool found = false;
    if (paths && modes &&
        QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths, &modeCount, modes, nullptr) == ERROR_SUCCESS) {
        for (UINT32 i = 0; i < pathCount && !found; i++) {
            DISPLAYCONFIG_SOURCE_DEVICE_NAME source = {};
            source.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
            source.header.size = sizeof(source);
            source.header.adapterId = paths[i].sourceInfo.adapterId;
            source.header.id = paths[i].sourceInfo.id;
            if (DisplayConfigGetDeviceInfo(&source.header) != ERROR_SUCCESS ||
                wcscmp(source.viewGdiDeviceName, gdiDevice) != 0)
                continue;
            DISPLAYCONFIG_TARGET_DEVICE_NAME target = {};
            target.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
            target.header.size = sizeof(target);
            target.header.adapterId = paths[i].targetInfo.adapterId;
            target.header.id = paths[i].targetInfo.id;
            if (DisplayConfigGetDeviceInfo(&target.header) != ERROR_SUCCESS || !target.monitorDevicePath[0])
                continue;
            wcsncpy_s(name, nameChars, target.monitorFriendlyDeviceName, _TRUNCATE);
            wcsncpy_s(path, pathChars, target.monitorDevicePath, _TRUNCATE);
            found = true;
        }
    }
    free(paths);
    free(modes);
    return found;
}

BOOL CALLBACK AddMonitor(HMONITOR handle, HDC, LPRECT, LPARAM) {
    if (g_monitorCount >= kMaxMonitors)
        return FALSE;
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(handle, &mi))
        return TRUE;
    Monitor& m = g_monitors[g_monitorCount++];
    m = {};
    m.handle = handle;
    m.rc = mi.rcMonitor;
    m.primary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;
    wchar_t path[128] = {};
    if (!MonitorIdentity(mi.szDevice, m.name, ARRAYSIZE(m.name), path, ARRAYSIZE(path)))
        wcscpy_s(path, mi.szDevice);
    if (!m.name[0])
        wcscpy_s(m.name, L"Display");
    uint32_t hash = 2166136261u;  // FNV-1a, case-insensitive
    for (const wchar_t* c = path; *c; c++)
        hash = (hash ^ towupper(*c)) * 16777619u;
    swprintf_s(m.key, L"dim%08X", hash);
    return TRUE;
}

/// Reads the connected monitors, left to right. One seen for the first time is dimmed when it's the main monitor
/// and left lit otherwise; on the first run after updating, every monitor connected then stays dimmed, as before.
void LoadMonitors() {
    g_monitorCount = 0;
    EnumDisplayMonitors(nullptr, nullptr, AddMonitor, 0);
    for (int i = 1; i < g_monitorCount; i++)
        for (int j = i; j > 0 && (g_monitors[j].rc.left < g_monitors[j - 1].rc.left ||
                                  (g_monitors[j].rc.left == g_monitors[j - 1].rc.left &&
                                   g_monitors[j].rc.top < g_monitors[j - 1].rc.top));
             j--) {
            Monitor t = g_monitors[j];
            g_monitors[j] = g_monitors[j - 1];
            g_monitors[j - 1] = t;
        }
    bool updating = !Wh_GetIntValue(L"monitorsKnown", 0) && Wh_GetIntValue(L"initialized", 0);
    Wh_SetIntValue(L"monitorsKnown", 1);
    g_allMonitorsDim = true;
    for (int i = 0; i < g_monitorCount; i++) {
        Monitor& m = g_monitors[i];
        int saved = Wh_GetIntValue(m.key, -1);
        if (saved < 0) {
            saved = updating || m.primary;
            Wh_SetIntValue(m.key, saved);
        }
        m.dim = saved != 0;
        g_allMonitorsDim &= m.dim;
    }
}

bool MonitorDimmed(HMONITOR handle) {
    for (int i = 0; i < g_monitorCount; i++)
        if (g_monitors[i].handle == handle)
            return g_monitors[i].dim;
    return true;
}

/// Cuts the monitors left lit out of a layer placed at `r` (screen coordinates), or clears its region.
void ClipLayer(HWND layer, const RECT& r) {
    if (!layer)
        return;
    if (g_allMonitorsDim) {
        SetWindowRgn(layer, nullptr, TRUE);
        return;
    }
    HRGN region = CreateRectRgn(0, 0, r.right - r.left, r.bottom - r.top);
    for (int i = 0; i < g_monitorCount; i++) {
        if (g_monitors[i].dim)
            continue;
        const RECT& m = g_monitors[i].rc;
        HRGN cut = CreateRectRgn(m.left - r.left, m.top - r.top, m.right - r.left, m.bottom - r.top);
        CombineRgn(region, region, cut, RGN_DIFF);
        DeleteObject(cut);
    }
    SetWindowRgn(layer, region, TRUE);  // the window owns the region from here
}

// ───────────── Overlay placement ─────────────

void FitOverlay() {
    RECT r = {GetSystemMetrics(SM_XVIRTUALSCREEN), GetSystemMetrics(SM_YVIRTUALSCREEN), 0, 0};
    r.right = r.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
    r.bottom = r.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
    SetWindowPos(g_hwndOverlay, nullptr, r.left, r.top, r.right - r.left, r.bottom - r.top,
                 SWP_NOZORDER | SWP_NOACTIVATE);
    ClipLayer(g_hwndOverlay, r);
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

/// Whether a focused window counts as a menu: one that belongs to a visible window and has no title bar (menus
/// apps draw themselves, such as Office's, and panels like Chrome's extension popups). With a title bar it counts
/// as a dialog. Menus follow the menus option, dialogs the dialogs option.
bool IsMenuLikeWindow(HWND hwnd, const wchar_t* cls) {
    // Office's menus, also when they have no owner.
    static const wchar_t* const kAppMenus[] = {L"Net UI Tool Window", L"Net UI Tool Window Layered"};
    if (ClassIn(cls, kAppMenus, ARRAYSIZE(kAppMenus)))
        return true;
    if (wcscmp(cls, L"#32770") == 0)
        return false;  // native dialogs stay dialogs even without a title bar
    HWND owner = GetWindow(hwnd, GW_OWNER);
    return owner && IsWindowVisible(owner) && (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CAPTION) != WS_CAPTION;
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

// After every successful move of the overlay, checks its place every 30 ms for 2 s instead of every 250 ms.
// Some apps keep lifting windows over it on their own: Chrome raises the page window above the overlay every second
// while its "Leave site?" dialog is open, and at 250 ms that window stayed lit for a visible moment each time.
void NoteOverlayMoved() {
    g_fastPollUntil = GetTickCount64() + 2000;
    if (!g_fastPoll) {
        g_fastPoll = true;
        SetTimer(g_hwndMain, TIMER_REPOSITION, 30, nullptr);
    }
}

// Moves the layer to the top of the non-topmost band, unless it is already there.
void RaiseOverlayToTop() {
    HWND prev = GetWindow(g_hwndOverlay, GW_HWNDPREV);
    if (!prev || (GetWindowLongPtrW(prev, GWL_EXSTYLE) & WS_EX_TOPMOST))
        return;
    SetWindowPos(g_hwndOverlay, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
    prev = GetWindow(g_hwndOverlay, GW_HWNDPREV);
    if (!prev || IsTopmost(prev))
        NoteOverlayMoved();
}

// Puts the layer right below the window, or at the top of the normal band when the window is topmost:
// inserting the layer behind a topmost window would make it topmost too.
void PlaceBelow(HWND hwnd) {
    if (IsTopmost(hwnd)) {
        RaiseOverlayToTop();
        return;
    }
    if (GetWindow(hwnd, GW_HWNDNEXT) == g_hwndOverlay)
        return;
    SetWindowPos(g_hwndOverlay, hwnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
    // A move that can't work (below an admin window on Windhawk 1.x) must not keep the 30 ms polling going.
    if (GetWindow(hwnd, GW_HWNDNEXT) == g_hwndOverlay)
        NoteOverlayMoved();
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

// ───────────── Fade ─────────────

/// Covers the window with the layer, placed right above it in z-order, at the given alpha.
void CoverWindow(HWND layer, HWND target, BYTE alpha) {
    RECT r;
    if (FAILED(DwmGetWindowAttribute(target, DWMWA_EXTENDED_FRAME_BOUNDS, &r, sizeof(r))))
        GetWindowRect(target, &r);
    SetLayeredWindowAttributes(layer, 0, alpha, LWA_ALPHA);
    HWND above = GetWindow(target, GW_HWNDPREV);
    UINT flags = SWP_NOACTIVATE | SWP_SHOWWINDOW;
    if (above == layer)
        flags |= SWP_NOZORDER;
    HWND after = !above || IsTopmost(above) ? HWND_TOP : above;
    SetWindowPos(layer, after, r.left, r.top, r.right - r.left, r.bottom - r.top, flags);
    ClipLayer(layer, r);
}

void Reposition();

/// Ends the fade in its final state: the overlay moves under the new window before the fade layer is hidden, so
/// the previous window never shows lit in between. With `reposition`, the placement is then checked again.
void FinishFade(bool reposition) {
    KillTimer(g_hwndMain, TIMER_FADE);
    g_fading = false;
    if (g_fadeTarget == HWND_TOP)
        RaiseOverlayToTop();
    else if (g_fadeTarget && IsWindow(g_fadeTarget))
        PlaceBelow(g_fadeTarget);
    g_fadeTarget = nullptr;
    if (g_fadeLayer)
        ShowWindow(g_fadeLayer, SW_HIDE);
    if (reposition)
        Reposition();
}

// Advances by the time since the last step, but at most 50 ms: when the system stalls (a dialog loading), the
// fade pauses and then resumes instead of jumping to its end.
void FadeStep() {
    int duration = g_ap.slider[SL_FADE];
    ULONGLONG now = GetTickCount64();
    ULONGLONG step = now - g_fadeLastStep;
    g_fadeLastStep = now;
    if (duration > 0)
        g_fadeProgress += (float)(step < 50 ? step : 50) / duration;
    if (duration <= 0 || g_fadeProgress >= 1) {
        FinishFade(true);
        return;
    }
    int alpha = (int)(Pct255(g_ap.slider[SL_OPACITY]) * g_fadeProgress);
    SetLayeredWindowAttributes(g_fadeLayer, 0, (BYTE)alpha, LWA_ALPHA);
}

/// Fades out the window lit until now, before the overlay moves to `target` (below that window, or HWND_TOP for
/// the top of the normal band). Whatever gets focus is already above the overlay, so it lights up at once; the
/// overlay stays under the old window while a layer over it goes from clear to the overlay's opacity. False when
/// there is nothing to fade, so the caller moves the overlay right away.
bool StartFade(HWND oldRoot, HWND target) {
    if (!oldRoot || oldRoot == target || !IsWindowVisible(oldRoot) || IsIconic(oldRoot) || IsTopmost(oldRoot) ||
        (target != HWND_TOP && GetWindow(target, GW_HWNDNEXT) == g_hwndOverlay))
        return false;
    if (!g_fadeLayer && !(g_fadeLayer = CreateLayer(false)))
        return false;
    // The hidden fade layer tries the move first: below an admin window on Windhawk 1.x it fails, and the fade
    // would dim the old window only to light it up again when the overlay can't follow.
    if (target != HWND_TOP && !IsTopmost(target) &&
        !SetWindowPos(g_fadeLayer, target, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE))
        return false;
    CoverWindow(g_fadeLayer, oldRoot, 0);
    g_fading = true;
    g_fadeTarget = target;
    g_fadeForeground = GetForegroundWindow();
    g_fadeProgress = 0;
    g_fadeLastStep = GetTickCount64();
    SetTimer(g_hwndMain, TIMER_FADE, 15, nullptr);
    return true;
}

// ───────────── Taskbar layers ─────────────

/// Keeps one topmost layer right above each taskbar while the overlay shows and no shell surface has focus.
// Explorer raises the taskbar again now and then (clicks, auto-hide); the 250 ms timer puts the layer back on top.
void UpdateTaskbarLayers() {
    HWND bars[ARRAYSIZE(g_taskbarLayers)] = {};
    int count = 0;
    if (g_ap.dimTaskbar && g_ap.active && g_overlayVisible && !g_shellFocused) {
        bars[count] = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (bars[count])
            count++;
        for (HWND w = nullptr; count < (int)ARRAYSIZE(bars) &&
                               (w = FindWindowExW(nullptr, w, L"Shell_SecondaryTrayWnd", nullptr));)
            bars[count++] = w;
    }
    for (int i = 0; i < (int)ARRAYSIZE(g_taskbarLayers); i++) {
        HWND& layer = g_taskbarLayers[i];
        if (i >= count || !IsWindowVisible(bars[i]) ||
            !MonitorDimmed(MonitorFromWindow(bars[i], MONITOR_DEFAULTTONEAREST))) {
            if (layer && IsWindowVisible(layer))
                ShowWindow(layer, SW_HIDE);
            continue;
        }
        if (!layer) {
            layer = CreateLayer(true);
            if (!layer)
                continue;
            SetLayeredWindowAttributes(layer, 0, (BYTE)Pct255(g_ap.slider[SL_OPACITY]), LWA_ALPHA);
        }
        RECT bar, current;
        GetWindowRect(bars[i], &bar);
        GetWindowRect(layer, &current);
        HWND above = GetWindow(bars[i], GW_HWNDPREV);
        if (above == layer && EqualRect(&bar, &current) && IsWindowVisible(layer))
            continue;
        UINT flags = SWP_NOACTIVATE | SWP_SHOWWINDOW;
        if (above == layer)
            flags |= SWP_NOZORDER;
        SetWindowPos(layer, above ? above : HWND_TOPMOST, bar.left, bar.top, bar.right - bar.left,
                     bar.bottom - bar.top, flags);
        InvalidateRect(layer, nullptr, TRUE);
    }
}

// ───────────── Placement ─────────────

void CALLBACK PingAnswered(HWND, UINT, ULONG_PTR target, LRESULT) {
    if ((HWND)target == g_pingTarget) {
        g_pingAnswered = true;
        Reposition();
    }
}

/// With Fade on, whether the focused window is ready for the fade to play. Heavy dialogs (Save, Open) take focus
/// while hidden or transparent and keep their thread busy loading; the fade waits until the window shows and its
/// thread answers a WM_NULL, checking again every 30 ms, for at most 2 s.
bool ForegroundReady(HWND fg) {
    // The mod's own windows (configuration, color and file dialogs) need no wait, and for them
    // SendMessageCallbackW calls PingAnswered before it returns.
    if (fg == g_waitedOut || GetWindowThreadProcessId(fg, nullptr) == GetCurrentThreadId())
        return true;
    if (fg != g_pendingForeground) {
        g_pendingForeground = fg;
        g_pendingSince = GetTickCount64();
    }
    DWORD cloaked = 0;
    DwmGetWindowAttribute(fg, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    BYTE alpha = 255;
    DWORD flags = 0;
    bool transparent = (GetWindowLongPtrW(fg, GWL_EXSTYLE) & WS_EX_LAYERED) &&
                       GetLayeredWindowAttributes(fg, nullptr, &alpha, &flags) && (flags & LWA_ALPHA) && !alpha;
    bool ready = IsWindowVisible(fg) && !cloaked && !transparent;
    if (ready && g_pingTarget != fg) {
        g_pingTarget = fg;
        g_pingAnswered = false;
        if (!SendMessageCallbackW(fg, WM_NULL, 0, 0, PingAnswered, (ULONG_PTR)fg))
            g_pingAnswered = true;
    }
    if (ready && g_pingAnswered)
        return true;
    if (GetTickCount64() - g_pendingSince >= 2000) {
        g_waitedOut = fg;
        return true;
    }
    SetTimer(g_hwndMain, TIMER_PENDING, 30, nullptr);
    return false;
}

/// The lowest window above the overlay that can actually be seen: the one lit until now.
// Hidden windows sit in the z-order too: a Save dialog slips hidden WorkerW windows right above the overlay, and
// taking the plain GW_HWNDPREV then finds nothing to fade.
HWND LitWindowAboveOverlay() {
    for (HWND w = GetWindow(g_hwndOverlay, GW_HWNDPREV); w; w = GetWindow(w, GW_HWNDPREV)) {
        if (w == g_fadeLayer || !IsWindowVisible(w) || IsIconic(w))
            continue;
        DWORD cloaked = 0;
        DwmGetWindowAttribute(w, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
        RECT r;
        if (cloaked || !GetWindowRect(w, &r) || IsRectEmpty(&r))
            continue;
        return w;
    }
    return nullptr;
}

/// Moves the overlay below `target`, or to the top of the normal band for HWND_TOP, fading out the window that
/// was lit when the overlay was already showing. `focus` is the window or menu the placement follows.
// Fades only when `focus` changed: an app that lifts its own window over the overlay (Chrome's page behind its
// "Leave site?" dialog, every second) gets put back at once instead of replaying the fade each time.
void MoveOverlay(HWND target, bool wasVisible, HWND focus) {
    static HWND lastFocus;
    bool focusChanged = focus != lastFocus;
    lastFocus = focus;
    if (wasVisible && focusChanged && g_ap.slider[SL_FADE] > 0 && StartFade(LitWindowAboveOverlay(), target))
        return;
    if (target == HWND_TOP)
        RaiseOverlayToTop();
    else
        PlaceBelow(target);
}

void PlaceOverlay() {
    if (!g_ap.active) {
        ShowOverlay(false);
        return;
    }
    bool wasVisible = g_overlayVisible;
    if (g_ap.dimMenus && g_menuCount) {
        ShowOverlay(true);
        MoveOverlay(g_menus[0], wasVisible, g_menus[0]);
        return;
    }
    HWND fg = GetForegroundWindow();
    if (!fg || fg == g_hwndOverlay)
        return;
    wchar_t cls[256];
    if (!GetClassNameW(fg, cls, ARRAYSIZE(cls)) || IsIgnoredClass(cls))
        return;
    if (IsMenuLikeWindow(fg, cls)) {
        if (g_ap.dimMenus) {
            ShowOverlay(true);
            MoveOverlay(fg, wasVisible, fg);
        }
        return;
    }
    // A hidden foreground window (a tray app showing its menu) keeps the taskbar's state, so a menu opened from the
    // taskbar doesn't bring the taskbar layer back.
    if (IsWindowVisible(fg))
        g_shellFocused = IsShellClass(cls);
    if (IsShellClass(cls)) {
        ShowOverlay(true);
        MoveOverlay(HWND_TOP, wasVisible, fg);
        return;
    }
    if (wcscmp(cls, L"Progman") == 0 || wcscmp(cls, L"WorkerW") == 0) {
        if (g_ap.hideOnDesktop) {
            ShowOverlay(false);
        } else {
            ShowOverlay(true);
            MoveOverlay(HWND_TOP, wasVisible, fg);
        }
        return;
    }
    if (g_ap.slider[SL_FADE] > 0 && !ForegroundReady(fg))
        return;
    g_pendingForeground = nullptr;
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
    MoveOverlay(root, wasVisible, fg);
}

void Reposition() {
    if (!g_hwndOverlay)
        return;
    if (g_fading) {
        if (g_ap.active)
            return;
        FinishFade(false);
    }
    PlaceOverlay();
    UpdateTaskbarLayers();
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
    } else if (g_fading && hwnd != g_fadeForeground) {
        // A real switch cuts the running fade short and may start its own. Repeated events for the same window
        // and transient windows (the Alt+Tab stand-in, menus) don't.
        wchar_t cls[64];
        if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) && !IsIgnoredClass(cls))
            FinishFade(false);
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
    swprintf_s(nid.szTip, L"Background Dimmer: %s\nClick to turn %s, right-click to configure",
               g_ap.active ? L"on" : L"off", g_ap.active ? L"off" : L"on");
    if (g_trayAdded && Shell_NotifyIconW(NIM_MODIFY, &nid))
        return;
    g_trayAdded = Shell_NotifyIconW(NIM_ADD, &nid) != FALSE;
}

// ───────────── Configuration window ─────────────

void SyncMonitorControls();

enum {
    ID_PATTERN = 100,
    ID_SLIDER = 110,      // + SL_* index
    ID_VALUE = 120,       // + SL_* index: editable number
    ID_UNIT = 180,        // + SL_* index
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
    ID_DIM_TASKBAR,
    ID_MONITOR = 200,  // + index in g_monitors
};

// Row of each slider in the window: Size and Scale share one, and only the one that fits the pattern shows.
const int kSliderRow[SL_COUNT] = {0, 1, 2, 3, 4, 3, 5};
constexpr int kSliderRows = 6;

struct ConfigUi {
    HFONT font;
    HBRUSH swatchBrush;
    UINT dpi;
};
ConfigUi g_ui;

HWND Ctl(int id) { return GetDlgItem(g_hwndConfig, id); }

const wchar_t* const kSliderUnits[SL_COUNT] = {L"%", L"%", L"%", L"px", L"\u00B0", L"%", L"ms"};

/// Writes the slider values into the number fields, skipping a field whose text already reads the same value so
/// typing in it isn't disturbed.
void UpdateValueLabels() {
    for (int i = 0; i < SL_COUNT; i++) {
        wchar_t text[16];
        GetWindowTextW(Ctl(ID_VALUE + i), text, ARRAYSIZE(text));
        if (text[0] && _wtoi(text) == g_ap.slider[i])
            continue;
        swprintf_s(text, L"%d", g_ap.slider[i]);
        SetWindowTextW(Ctl(ID_VALUE + i), text);
    }
}

/// Applies a new slider value from the trackbar or the number field.
void SetSlider(int i, int value) {
    value = Clamp(value, kSliderMin[i], kSliderMax[i]);
    if (value == g_ap.slider[i])
        return;
    g_ap.slider[i] = value;
    SendMessageW(Ctl(ID_SLIDER + i), TBM_SETPOS, TRUE, value);
    if (i == SL_OPACITY)
        ApplyOpacity();
    else if (i == SL_BRIGHTNESS || i == SL_CONTRAST)
        UpdateColors();
    else if (i != SL_FADE)
        BuildBrush();
    ScheduleSave();
}

/// Takes the numbers typed in the fields. With `rewrite`, each field then shows its value as clamped to the range.
void ApplyValueFields(bool rewrite) {
    KillTimer(g_hwndMain, TIMER_VALUES);
    if (!g_hwndConfig)
        return;
    for (int i = 0; i < SL_COUNT; i++) {
        wchar_t text[16];
        GetWindowTextW(Ctl(ID_VALUE + i), text, ARRAYSIZE(text));
        if (text[0])
            SetSlider(i, _wtoi(text));
    }
    if (rewrite) {
        for (int i = 0; i < SL_COUNT; i++) {
            wchar_t text[16];
            swprintf_s(text, L"%d", g_ap.slider[i]);
            SetWindowTextW(Ctl(ID_VALUE + i), text);
        }
    }
}

void EnableSlider(int i, bool enable) {
    EnableWindow(Ctl(ID_SLIDER + i), enable);
    EnableWindow(Ctl(ID_VALUE + i), enable);
    EnableWindow(Ctl(ID_UNIT + i), enable);
}

// Solid ignores contrast, size and rotation; the image takes Scale in place of Size and can't rotate.
void UpdateEnabled() {
    bool image = g_ap.pattern == PAT_IMAGE;
    bool geometric = g_ap.pattern == PAT_CHECKER || g_ap.pattern == PAT_STRIPES;
    EnableSlider(SL_CONTRAST, g_ap.pattern != PAT_SOLID);
    EnableSlider(SL_SIZE, geometric);
    EnableSlider(SL_ANGLE, geometric);
    const int sizeSliders[] = {SL_SIZE, SL_SCALE};
    for (int i : sizeSliders) {
        int show = (i == SL_SCALE) == image ? SW_SHOW : SW_HIDE;
        ShowWindow(Ctl(ID_SLIDER + i), show);
        ShowWindow(Ctl(ID_VALUE + i), show);
        ShowWindow(Ctl(ID_UNIT + i), show);
    }
    SetWindowTextW(Ctl(ID_LABEL + 1 + kSliderRow[SL_SIZE]), image ? L"Scale" : L"Size");
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
    CheckDlgButton(g_hwndConfig, ID_DIM_TASKBAR, g_ap.dimTaskbar ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(g_hwndConfig, ID_ACTIVE, g_ap.active ? BST_CHECKED : BST_UNCHECKED);
    UpdateValueLabels();
    UpdateEnabled();
    UpdateSwatch();
    UpdateActiveLabel();
    SyncMonitorControls();
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
    g_imageSourcePath[0] = L'\0';  // picking the same file again reloads it
    g_ap.pattern = PAT_IMAGE;
    return true;
}

/// Takes the path typed or pasted in the Image field, without surrounding spaces or quotes ("Copy as path").
void ApplyImagePath() {
    KillTimer(g_hwndMain, TIMER_IMAGE_PATH);
    if (!g_hwndConfig)
        return;
    wchar_t text[MAX_PATH + 8];
    GetWindowTextW(Ctl(ID_IMAGE), text, ARRAYSIZE(text));
    wchar_t* start = text;
    while (iswspace(*start) || *start == L'"')
        start++;
    wchar_t* end = start + wcslen(start);
    while (end > start && (iswspace(end[-1]) || end[-1] == L'"'))
        *--end = L'\0';
    if (wcslen(start) >= MAX_PATH || wcscmp(start, g_ap.image) == 0)
        return;
    wcscpy_s(g_ap.image, start);
    BuildBrush();
    ScheduleSave();
}

/// The values that show the image unchanged, set when the image pattern is chosen so tuning starts from them.
/// Leaving the image pattern restores the defaults instead.
void ApplyImageDefaults() {
    g_ap.slider[SL_BRIGHTNESS] = 50;
    g_ap.slider[SL_CONTRAST] = 100;
    g_ap.colorRgb = 0xFFFFFF;
}

void OnPatternChanged() {
    int sel = (int)SendMessageW(Ctl(ID_PATTERN), CB_GETCURSEL, 0, 0);
    if (sel < 0)
        return;
    int previous = g_ap.pattern;
    g_ap.pattern = sel;
    if (sel == PAT_IMAGE && GetFileAttributesW(g_ap.image) == INVALID_FILE_ATTRIBUTES && !PickImage())
        g_ap.pattern = previous;
    if (g_ap.pattern == PAT_IMAGE && previous != PAT_IMAGE) {
        ApplyImageDefaults();
    } else if (previous == PAT_IMAGE && g_ap.pattern != PAT_IMAGE) {
        g_ap.slider[SL_BRIGHTNESS] = kDefaults.slider[SL_BRIGHTNESS];
        g_ap.slider[SL_CONTRAST] = kDefaults.slider[SL_CONTRAST];
        g_ap.colorRgb = kDefaults.colorRgb;
    }
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

// Layout in 96-dpi pixels, scaled to the DPI of the current monitor. Each connected monitor adds a row under
// Active.
const int kClientW = 440, kClientH = 520, kMonitorRow = 24;

int ClientHeight() { return kClientH + g_monitorCount * kMonitorRow; }

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
    for (int r = 0; r < kSliderRows; r++)
        MoveCtl(ID_LABEL + 1 + r, lx, y + r * row + 4, 104, 20);
    for (int i = 0; i < SL_COUNT; i++) {
        MoveCtl(ID_SLIDER + i, cx - 4, y + kSliderRow[i] * row, 210, 28);
        MoveCtl(ID_VALUE + i, cx + 210, y + kSliderRow[i] * row + 2, 46, 24);
        MoveCtl(ID_UNIT + i, cx + 260, y + kSliderRow[i] * row + 6, 30, 20);
    }
    y += kSliderRows * row;
    MoveCtl(ID_LABEL + 1 + kSliderRows, lx, y + 4, 104, 20);
    MoveCtl(ID_SWATCH, cx, y + 2, 44, 22);
    MoveCtl(ID_COLOR, cx + 52, y, 96, 26);
    y += row + 2;
    MoveCtl(ID_LABEL + 2 + kSliderRows, lx, y + 4, 104, 20);
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
    MoveCtl(ID_DIM_TASKBAR, lx, y, kClientW - 2 * lx, 22);
    y += 26;
    MoveCtl(ID_ACTIVE, lx, y, kClientW - 2 * lx, 22);
    y += 26;
    for (int i = 0; i < kMaxMonitors; i++)
        MoveCtl(ID_MONITOR + i, lx + 20, y + i * kMonitorRow, kClientW - 2 * lx - 20, 22);
    y += g_monitorCount * kMonitorRow + 12;
    MoveCtl(ID_RESET, lx, y, 140, 28);
    MoveCtl(IDCANCEL, kClientW - 14 - 92, y, 92, 28);
}

void ResizeConfigForDpi(UINT dpi, const RECT* suggested) {
    RECT rc = {0, 0, MulDiv(kClientW, (int)dpi, 96), MulDiv(ClientHeight(), (int)dpi, 96)};
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
    const wchar_t* labels[] = {L"Pattern", L"Opacity",  L"Brightness", L"Contrast", L"Size",
                               L"Rotation", L"Fade",    L"Base color", L"Image"};
    for (int i = 0; i < (int)ARRAYSIZE(labels); i++)
        AddCtl(L"STATIC", labels[i], SS_LEFT, ID_LABEL + i);

    HWND combo = AddCtl(WC_COMBOBOXW, L"", CBS_DROPDOWNLIST | WS_TABSTOP | WS_VSCROLL, ID_PATTERN);
    for (const wchar_t* name : kPatternNames)
        SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)name);

    for (int i = 0; i < SL_COUNT; i++) {
        HWND tb = AddCtl(TRACKBAR_CLASSW, L"", TBS_HORZ | TBS_NOTICKS | WS_TABSTOP, ID_SLIDER + i);
        SendMessageW(tb, TBM_SETRANGEMIN, FALSE, kSliderMin[i]);
        SendMessageW(tb, TBM_SETRANGEMAX, FALSE, kSliderMax[i]);
        SendMessageW(tb, TBM_SETPAGESIZE, 0, i == SL_FADE ? 50 : 5);
        AddCtl(L"EDIT", L"", ES_NUMBER | ES_RIGHT | ES_AUTOHSCROLL | WS_TABSTOP, ID_VALUE + i, WS_EX_CLIENTEDGE);
        AddCtl(L"STATIC", kSliderUnits[i], SS_LEFT, ID_UNIT + i);
    }

    AddCtl(L"STATIC", L"", SS_SUNKEN, ID_SWATCH);
    AddCtl(L"BUTTON", L"Choose…", BS_PUSHBUTTON | WS_TABSTOP, ID_COLOR);
    AddCtl(L"EDIT", L"", ES_AUTOHSCROLL | WS_TABSTOP, ID_IMAGE, WS_EX_CLIENTEDGE);
    AddCtl(L"BUTTON", L"…", BS_PUSHBUTTON | WS_TABSTOP, ID_BROWSE);
    AddCtl(L"BUTTON", L"Skip dimming while the desktop has focus", BS_AUTOCHECKBOX | WS_TABSTOP, ID_HIDE_DESKTOP);
    AddCtl(L"BUTTON", L"Hide the overlay from screenshots and screen sharing", BS_AUTOCHECKBOX | WS_TABSTOP,
           ID_HIDE_CAPTURE);
    AddCtl(L"BUTTON", L"Dialogs dim parent windows", BS_AUTOCHECKBOX | WS_TABSTOP, ID_DIM_OWNERS);
    AddCtl(L"BUTTON", L"Menus dim parent windows", BS_AUTOCHECKBOX | WS_TABSTOP, ID_DIM_MENUS);
    AddCtl(L"BUTTON", L"Taskbar dimming", BS_AUTOCHECKBOX | WS_TABSTOP, ID_DIM_TASKBAR);
    AddCtl(L"BUTTON", L"Active", BS_AUTOCHECKBOX | WS_TABSTOP, ID_ACTIVE);
    for (int i = 0; i < kMaxMonitors; i++)
        AddCtl(L"BUTTON", L"", BS_AUTOCHECKBOX | WS_TABSTOP, ID_MONITOR + i);
    AddCtl(L"BUTTON", L"Reset to defaults", BS_PUSHBUTTON | WS_TABSTOP, ID_RESET);
    AddCtl(L"BUTTON", L"Close", BS_DEFPUSHBUTTON | WS_TABSTOP, IDCANCEL);
}

/// One checkbox per connected monitor under Active: checked, it's dimmed. Shown in a window that's open when
/// monitors come and go, resized to the new count.
void SyncMonitorControls() {
    if (!g_hwndConfig)
        return;
    for (int i = 0; i < kMaxMonitors; i++) {
        HWND box = Ctl(ID_MONITOR + i);
        if (i >= g_monitorCount) {
            ShowWindow(box, SW_HIDE);
            continue;
        }
        const Monitor& m = g_monitors[i];
        wchar_t text[128];
        swprintf_s(text, L"%d. %s  (%d \u00D7 %d%s)", i + 1, m.name, (int)(m.rc.right - m.rc.left),
                   (int)(m.rc.bottom - m.rc.top), m.primary ? L", main" : L"");
        SetWindowTextW(box, text);
        CheckDlgButton(g_hwndConfig, ID_MONITOR + i, m.dim ? BST_CHECKED : BST_UNCHECKED);
        ShowWindow(box, SW_SHOW);
    }
    RECT client;
    GetClientRect(g_hwndConfig, &client);
    if (client.bottom != MulDiv(ClientHeight(), (int)g_ui.dpi, 96)) {
        RECT wr;
        GetWindowRect(g_hwndConfig, &wr);
        ResizeConfigForDpi(g_ui.dpi, &wr);
    }
}

void SetMonitorDim(int i, bool dim) {
    if (i >= g_monitorCount)
        return;
    g_monitors[i].dim = dim;
    Wh_SetIntValue(g_monitors[i].key, dim);
    g_allMonitorsDim = true;
    for (int j = 0; j < g_monitorCount; j++)
        g_allMonitorsDim &= g_monitors[j].dim;
    FitOverlay();
    if (g_fadeLayer && IsWindowVisible(g_fadeLayer)) {
        RECT r;
        GetWindowRect(g_fadeLayer, &r);
        ClipLayer(g_fadeLayer, r);
    }
    Reposition();
}

LRESULT CALLBACK ConfigProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_HSCROLL: {
            HWND tb = (HWND)lParam;
            for (int i = 0; i < SL_COUNT; i++) {
                if (tb != Ctl(ID_SLIDER + i))
                    continue;
                SetSlider(i, (int)SendMessageW(tb, TBM_GETPOS, 0, 0));
                UpdateValueLabels();
                return 0;
            }
            break;
        }
        case WM_COMMAND:
            if (LOWORD(wParam) >= ID_MONITOR && LOWORD(wParam) < ID_MONITOR + kMaxMonitors) {
                int i = LOWORD(wParam) - ID_MONITOR;
                SetMonitorDim(i, IsDlgButtonChecked(hwnd, ID_MONITOR + i) == BST_CHECKED);
                return 0;
            }
            // Number fields: applied after a pause in typing, and shown clamped when the field loses focus.
            if (LOWORD(wParam) >= ID_VALUE && LOWORD(wParam) < ID_VALUE + SL_COUNT) {
                if (HIWORD(wParam) == EN_CHANGE)
                    SetTimer(g_hwndMain, TIMER_VALUES, 500, nullptr);
                else if (HIWORD(wParam) == EN_KILLFOCUS)
                    ApplyValueFields(true);
                return 0;
            }
            switch (LOWORD(wParam)) {
                case ID_PATTERN:
                    if (HIWORD(wParam) == CBN_SELCHANGE)
                        OnPatternChanged();
                    return 0;
                case ID_COLOR:
                    PickColor();
                    return 0;
                case ID_IMAGE:
                    // Applied after a pause in typing, or when the field loses focus.
                    if (HIWORD(wParam) == EN_CHANGE)
                        SetTimer(g_hwndMain, TIMER_IMAGE_PATH, 500, nullptr);
                    else if (HIWORD(wParam) == EN_KILLFOCUS)
                        ApplyImagePath();
                    return 0;
                case ID_BROWSE: {
                    int previous = g_ap.pattern;
                    if (PickImage()) {
                        if (previous != PAT_IMAGE)
                            ApplyImageDefaults();
                        SyncControls();
                        BuildBrush();
                        ScheduleSave();
                    }
                    return 0;
                }
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
                case ID_DIM_TASKBAR:
                    g_ap.dimTaskbar = IsDlgButtonChecked(hwnd, ID_DIM_TASKBAR) == BST_CHECKED;
                    Reposition();
                    ScheduleSave();
                    return 0;
                case ID_ACTIVE:
                    SetActive(IsDlgButtonChecked(hwnd, ID_ACTIVE) == BST_CHECKED);
                    return 0;
                case ID_RESET:
                    ResetDefaults();
                    return 0;
                case IDOK:
                    // Enter in a text field: apply what was typed right away.
                    ApplyValueFields(true);
                    ApplyImagePath();
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
            // The controls still exist here: a path or number typed just before closing isn't lost.
            ApplyImagePath();
            ApplyValueFields(false);
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

void ApplyModSettings() {
    RegisterHotkeyText(HK_TOGGLE, g_settings.toggleHotkey);
    RegisterHotkeyText(HK_CONFIG, g_settings.configHotkey);
    if (g_hwndConfig)
        UpdateActiveLabel();
    UpdateTrayIcon();
}

void Shutdown() {
    // The configuration window first: closing it applies a value typed just before, which then gets saved.
    if (g_hwndConfig)
        DestroyWindow(g_hwndConfig);
    if (g_savePending)
        SaveAppearance();
    if (g_hook)
        UnhookWinEvent(g_hook);
    g_hook = nullptr;
    UnregisterHotKey(g_hwndMain, HK_TOGGLE);
    UnregisterHotKey(g_hwndMain, HK_CONFIG);
    RemoveTrayIcon();
    KillTimer(g_hwndMain, TIMER_FADE);
    g_fading = false;
    if (g_fadeLayer)
        DestroyWindow(g_fadeLayer);
    g_fadeLayer = nullptr;
    for (HWND& w : g_taskbarLayers) {
        if (w)
            DestroyWindow(w);
        w = nullptr;
    }
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
                if (g_fastPoll && GetTickCount64() >= g_fastPollUntil) {
                    g_fastPoll = false;
                    SetTimer(hwnd, TIMER_REPOSITION, 250, nullptr);
                }
            } else if (wParam == TIMER_SAVE) {
                KillTimer(hwnd, TIMER_SAVE);
                SaveAppearance();
            } else if (wParam == TIMER_FIT) {
                KillTimer(hwnd, TIMER_FIT);
                LoadMonitors();
                FitOverlay();
                SyncMonitorControls();
                if (g_texture)
                    BuildBrush();
            } else if (wParam == TIMER_FADE) {
                FadeStep();
            } else if (wParam == TIMER_IMAGE_PATH) {
                ApplyImagePath();
            } else if (wParam == TIMER_VALUES) {
                ApplyValueFields(false);
            } else if (wParam == TIMER_PENDING) {
                KillTimer(hwnd, TIMER_PENDING);
                Reposition();
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
                SetActive(!g_ap.active);
            else if (lParam == WM_RBUTTONUP)
                OpenConfig();
            return 0;
        case WM_APP_RELOAD:
            LoadModSettings();
            ApplyModSettings();
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
            // Every layer paints the pattern as seen from the virtual screen's origin, so a fade or taskbar layer
            // lines up exactly with the overlay around it.
            HDC dc = (HDC)wParam;
            RECT rc, wr;
            GetClientRect(hwnd, &rc);
            GetWindowRect(hwnd, &wr);
            int ox = wr.left - GetSystemMetrics(SM_XVIRTUALSCREEN), oy = wr.top - GetSystemMetrics(SM_YVIRTUALSCREEN);
            if (g_texture) {
                HDC mem = CreateCompatibleDC(dc);
                HGDIOBJ old = SelectObject(mem, g_texture);
                BitBlt(dc, 0, 0, rc.right, rc.bottom, mem, ox, oy, SRCCOPY);
                SelectObject(mem, old);
                DeleteDC(mem);
            } else if (g_brush) {
                BITMAP bm;
                if (g_brushBitmap && GetObject(g_brushBitmap, sizeof(bm), &bm) && bm.bmWidth && bm.bmHeight)
                    SetBrushOrgEx(dc, ((-ox % bm.bmWidth) + bm.bmWidth) % bm.bmWidth,
                                  ((-oy % bm.bmHeight) + bm.bmHeight) % bm.bmHeight, nullptr);
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
    g_hwndOverlay = CreateLayer(false);
    // Lets Explorer's TaskbarCreated broadcast through UIPI when the host runs with UIAccess.
    if (g_hwndMain)
        ChangeWindowMessageFilterEx(g_hwndMain, g_wmTaskbarCreated, MSGFLT_ALLOW, nullptr);
    // Read after g_hwndMain exists, so a settings change saved meanwhile is queued as WM_APP_RELOAD, not lost.
    LoadModSettings();
    if (g_hwndMain && g_hwndOverlay) {
        LoadMonitors();
        BuildBrush();
        ApplyOpacity();
        ApplyCaptureExclusion();
        FitOverlay();
        g_hook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_MENUPOPUPEND, nullptr, WinEventProc, 0, 0,
                                 WINEVENT_OUTOFCONTEXT);
        // Safety net for z-order changes no WinEvent reports, such as the active window being lowered or made
        // topmost without the foreground window changing.
        SetTimer(g_hwndMain, TIMER_REPOSITION, 250, nullptr);
        ApplyModSettings();
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
    if (g_imageSource)
        DeleteObject(g_imageSource);
    g_brush = nullptr;
    g_brushBitmap = nullptr;
    g_texture = nullptr;
    g_imageSource = nullptr;
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
