// ==WindhawkMod==
// @id              background-dimmer
// @name            Background Dimmer
// @description     Dims unfocused windows with a solid tint, pattern or image, tuned live in its own configuration window
// @version         1.0.0
// @author          3DDinosaur
// @github          https://github.com/3DDinosaur
// @include         windhawk.exe
// @compilerOptions -lcomctl32 -lcomdlg32 -lgdi32 -lgdiplus -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
![Background Dimmer](https://i.imgur.com/HvfnpFn.png)

# Background Dimmer

Dims inactive windows so the active one stands out, while the background stays readable. Especially useful when working with windows side by side.

The dimming layer can be a solid tint, checkerboard, horizontal, vertical or diagonal stripes, or a tiled image of your choice. Opacity, brightness, contrast, pattern size and base color are all adjustable.

## Live configuration

The look is tuned in the mod's own **configuration window**, where every change shows up immediately on screen. Open it with the **configuration hotkey** (**Win+Ctrl+Alt+D** by default, changeable in the mod settings).

The window also opens by itself the first time the mod runs. If the configuration hotkey is empty or already taken by another program, it opens whenever the mod settings are saved. When it opens this way, without a keypress, Windows may keep it behind other windows: look for its flashing taskbar button.

Changes made in the configuration window are saved automatically. Dimming can be turned off with the **Active** checkbox in the window, or with an optional on/off hotkey.

## How the controls work

- **Opacity**: how strongly the layer covers the background.
- **Brightness**: the overall lightness of the layer.
- **Contrast**: the difference between the light and dark parts of a pattern. At 0 the pattern disappears into a flat tint.
- **Size**: the size of the checkerboard squares or the width of the stripes, in pixels.
- **Base color**: tints the layer. White gives neutral gray tones.

Brightness and contrast set the two pattern tones as *brightness ± contrast / 2*, so one controls how dark the layer is and the other how visible the pattern is. Near the ends of the range a tone is clipped to black or white, which keeps the pattern visible over a black base, as in the default look.

## Behavior

- Dialogs stay lit together with the window that owns them.
- The taskbar, Start, Alt+Tab and menus never move the layer.
- When the desktop has focus, the layer can either hide or cover everything (option in the configuration window).
- The mod runs in its own Windhawk process and does not inject into Explorer or any other program.
- Optionally, the layer can be left out of screenshots, recordings and screen sharing while it stays visible on your screen (Windows 10 version 2004 or later).
- The appearance is stored per computer, so all Windows users of the same PC share it.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- configHotkey: Win+Ctrl+Alt+D
  $name: Configuration window hotkey
  $description: >-
    Opens the live configuration window. Modifiers (Ctrl, Alt, Shift, Win)
    and one key, joined with +, for example Win+Ctrl+Alt+D or Ctrl+Shift+F9. If
    empty, the configuration window opens whenever these settings are saved.
- toggleHotkey: ""
  $name: On/off hotkey
  $description: >-
    Turns the dimming on and off, same format as above. Leave empty for no
    hotkey.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
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

enum Pattern { PAT_SOLID, PAT_CHECKER, PAT_HSTRIPES, PAT_VSTRIPES, PAT_DIAGONAL, PAT_IMAGE, PAT_COUNT };
const wchar_t* const kPatternNames[PAT_COUNT] = {
    L"Solid", L"Checkerboard", L"Horizontal stripes", L"Vertical stripes", L"Diagonal stripes", L"Image",
};

constexpr int SL_OPACITY = 0, SL_BRIGHTNESS = 1, SL_CONTRAST = 2, SL_SIZE = 3, SL_COUNT = 4;

struct Appearance {
    int pattern;
    int slider[SL_COUNT];   // opacity/brightness/contrast in %, size in px
    int colorRgb;           // 0xRRGGBB
    bool hideOnDesktop;
    bool hideFromCapture;
    bool active;
    wchar_t image[MAX_PATH];
};

const Appearance kDefaults = {PAT_DIAGONAL, {20, 0, 60, 16}, 0xFFFFFF, true, false, true, L""};
const wchar_t* const kSliderKeys[SL_COUNT] = {L"opacity", L"brightness", L"contrast", L"size"};
const int kSliderMin[SL_COUNT] = {0, 0, 0, 1};
const int kSliderMax[SL_COUNT] = {100, 100, 100, 200};

struct ModSettings {
    wchar_t toggleHotkey[64];
    wchar_t configHotkey[64];
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
bool g_overlayVisible;
bool g_configHotkeyActive;
bool g_savePending;
ULONG_PTR g_gdiplusToken;
HICON g_icon;
bool g_iconExtracted;  // false when g_icon is the shared IDI_APPLICATION icon, which must not be destroyed

constexpr wchar_t kMainClass[] = L"BackgroundDimmer_Main";
constexpr wchar_t kOverlayClass[] = L"BackgroundDimmer_Overlay";
constexpr wchar_t kConfigClass[] = L"BackgroundDimmer_Config";

enum { WM_APP_RELOAD = WM_APP + 1, WM_APP_EXIT };
enum { HK_TOGGLE = 1, HK_CONFIG = 2 };
enum { TIMER_REPOSITION = 1, TIMER_SAVE = 2, TIMER_FIT = 3 };

// ───────────── Persistence ─────────────

int Clamp(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

void LoadAppearance() {
    g_ap.pattern = Clamp(Wh_GetIntValue(L"pattern", kDefaults.pattern), 0, PAT_COUNT - 1);
    for (int i = 0; i < SL_COUNT; i++)
        g_ap.slider[i] = Clamp(Wh_GetIntValue(kSliderKeys[i], kDefaults.slider[i]), kSliderMin[i], kSliderMax[i]);
    g_ap.colorRgb = Wh_GetIntValue(L"color", kDefaults.colorRgb) & 0xFFFFFF;
    g_ap.hideOnDesktop = Wh_GetIntValue(L"hideOnDesktop", kDefaults.hideOnDesktop) != 0;
    g_ap.hideFromCapture = Wh_GetIntValue(L"hideFromCapture", kDefaults.hideFromCapture) != 0;
    g_ap.active = Wh_GetIntValue(L"active", kDefaults.active) != 0;
    g_ap.image[0] = L'\0';
    Wh_GetStringValue(L"image", g_ap.image, ARRAYSIZE(g_ap.image));
}

void SaveAppearance() {
    g_savePending = false;
    Wh_SetIntValue(L"pattern", g_ap.pattern);
    for (int i = 0; i < SL_COUNT; i++)
        Wh_SetIntValue(kSliderKeys[i], g_ap.slider[i]);
    Wh_SetIntValue(L"color", g_ap.colorRgb);
    Wh_SetIntValue(L"hideOnDesktop", g_ap.hideOnDesktop);
    Wh_SetIntValue(L"hideFromCapture", g_ap.hideFromCapture);
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

void FillBox(HDC dc, int l, int t, int r, int b, COLORREF color) {
    RECT rc = {l, t, r, b};
    HBRUSH br = CreateSolidBrush(color);
    FillRect(dc, &rc, br);
    DeleteObject(br);
}

// Light band where (x+y) mod 2s < s. The polygons overshoot the tile by 1 px so no seam shows between tiles.
void DiagonalBands(HDC dc, int s, COLORREF color) {
    HBRUSH br = CreateSolidBrush(color);
    HGDIOBJ oldBrush = SelectObject(dc, br);
    HGDIOBJ oldPen = SelectObject(dc, GetStockObject(NULL_PEN));
    int e = 2 * s + 1;
    const int starts[2] = {0, 2 * s};
    for (int c0 : starts) {
        int c1 = c0 + s;
        POINT pts[4] = {{-1, c0 + 1}, {e, c0 - e}, {e, c1 - e}, {-1, c1 + 1}};
        Polygon(dc, pts, 4);
    }
    SelectObject(dc, oldPen);
    SelectObject(dc, oldBrush);
    DeleteObject(br);
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

void BuildBrush() {
    HBITMAP bmp = g_ap.pattern == PAT_IMAGE ? LoadTile(g_ap.image) : nullptr;
    if (!bmp) {
        int sz = g_ap.slider[SL_SIZE];
        int k = Pct255(g_ap.slider[SL_CONTRAST]);
        int lvl = Pct255(g_ap.slider[SL_BRIGHTNESS]);
        COLORREF dark = Shade(g_ap.colorRgb, lvl - k / 2);
        COLORREF light = Shade(g_ap.colorRgb, lvl + k - k / 2);
        int w = 2 * sz, h = 2 * sz;
        switch (g_ap.pattern) {
            case PAT_SOLID:
            case PAT_IMAGE: w = h = 8; break;
            case PAT_HSTRIPES: w = 8; break;
            case PAT_VSTRIPES: h = 8; break;
        }
        HDC screen = GetDC(nullptr);
        HDC mdc = CreateCompatibleDC(screen);
        bmp = CreateCompatibleBitmap(screen, w, h);
        ReleaseDC(nullptr, screen);
        HGDIOBJ old = SelectObject(mdc, bmp);
        switch (g_ap.pattern) {
            case PAT_SOLID:
            case PAT_IMAGE:
                FillBox(mdc, 0, 0, w, h, Shade(g_ap.colorRgb, lvl));
                break;
            case PAT_CHECKER:
                FillBox(mdc, 0, 0, w, h, dark);
                FillBox(mdc, 0, 0, sz, sz, light);
                FillBox(mdc, sz, sz, w, h, light);
                break;
            case PAT_HSTRIPES:
                FillBox(mdc, 0, 0, w, h, dark);
                FillBox(mdc, 0, 0, w, sz, light);
                break;
            case PAT_VSTRIPES:
                FillBox(mdc, 0, 0, w, h, dark);
                FillBox(mdc, 0, 0, sz, h, light);
                break;
            case PAT_DIAGONAL:
                FillBox(mdc, 0, 0, w, h, dark);
                DiagonalBands(mdc, sz, light);
                break;
        }
        SelectObject(mdc, old);
        DeleteDC(mdc);
    }
    HBRUSH brush = CreatePatternBrush(bmp);
    if (g_brush)
        DeleteObject(g_brush);
    if (g_brushBitmap)
        DeleteObject(g_brushBitmap);
    g_brush = brush;
    g_brushBitmap = bmp;
    if (g_hwndOverlay)
        InvalidateRect(g_hwndOverlay, nullptr, TRUE);
}

#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif

// The layer stays on screen but is left out of screenshots, recordings and screen sharing (Windows 10 2004+).
void ApplyCaptureExclusion() {
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

// Windows that never move the layer when they get focus (taskbar, Alt+Tab, Start, menus).
bool IsIgnoredClass(const wchar_t* cls) {
    static const wchar_t* const kIgnored[] = {
        L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd", L"XamlExplorerHostIslandWindow", L"MultitaskingViewFrame",
        L"ForegroundStaging", L"TaskListThumbnailWnd", L"NotifyIconOverflowWindow",
        L"TopLevelWindowForOverflowXamlIsland", L"Windows.UI.Core.CoreWindow", L"#32768", kMainClass,
    };
    for (const wchar_t* c : kIgnored)
        if (wcscmp(cls, c) == 0)
            return true;
    return false;
}

// Moves the layer to the top of the non-topmost band, unless it is already there.
void RaiseOverlayToTop() {
    HWND prev = GetWindow(g_hwndOverlay, GW_HWNDPREV);
    if (!prev || (GetWindowLongPtrW(prev, GWL_EXSTYLE) & WS_EX_TOPMOST))
        return;
    SetWindowPos(g_hwndOverlay, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
}

void Reposition() {
    if (!g_hwndOverlay)
        return;
    if (!g_ap.active) {
        ShowOverlay(false);
        return;
    }
    HWND fg = GetForegroundWindow();
    if (!fg || fg == g_hwndOverlay)
        return;
    wchar_t cls[256];
    if (!GetClassNameW(fg, cls, ARRAYSIZE(cls)) || IsIgnoredClass(cls))
        return;
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
    // Below the root owner, so a dialog and the window that owns it stay lit together.
    // Stops at the last visible owner: hidden owners (WinForms ShowInTaskbar=false, Java dialogs) aren't raised
    // with the window they own, so placing the layer below one would leave everything in between undimmed.
    HWND root = fg;
    for (HWND owner; (owner = GetWindow(root, GW_OWNER)) && IsWindowVisible(owner);)
        root = owner;
    if (GetWindowLongPtrW(root, GWL_EXSTYLE) & WS_EX_TOPMOST) {
        // Inserting the layer behind a topmost window would make it topmost too; go to the top of the normal band instead.
        RaiseOverlayToTop();
        return;
    }
    if (GetWindow(root, GW_HWNDNEXT) == g_hwndOverlay)
        return;
    SetWindowPos(g_hwndOverlay, root, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD) {
    Reposition();
}

// ───────────── Icon ─────────────

HICON LoadSystemIcon(int index, int size) {
    wchar_t path[MAX_PATH];
    GetSystemDirectoryW(path, MAX_PATH);
    wcscat_s(path, L"\\imageres.dll");
    HICON icon = nullptr;
    g_iconExtracted = SUCCEEDED(SHDefExtractIconW(path, index, 0, &icon, nullptr, size)) && icon;
    return g_iconExtracted ? icon : LoadIconW(nullptr, IDI_APPLICATION);
}

// ───────────── Configuration window ─────────────

enum {
    ID_PATTERN = 100,
    ID_SLIDER = 110,      // + SL_* index
    ID_VALUE = 120,       // + SL_* index
    ID_LABEL = 140,       // + row
    ID_SWATCH = 130,
    ID_COLOR,
    ID_IMAGE,
    ID_BROWSE,
    ID_HIDE_DESKTOP,
    ID_HIDE_CAPTURE,
    ID_ACTIVE,
    ID_RESET,
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
    BuildBrush();
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
const int kClientW = 440, kClientH = 370;

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
    MoveCtl(ID_LABEL + 5, lx, y + 4, 104, 20);
    MoveCtl(ID_SWATCH, cx, y + 2, 44, 22);
    MoveCtl(ID_COLOR, cx + 52, y, 96, 26);
    y += row + 2;
    MoveCtl(ID_LABEL + 6, lx, y + 4, 104, 20);
    MoveCtl(ID_IMAGE, cx, y + 1, 220, 24);
    MoveCtl(ID_BROWSE, cx + 226, y, 32, 26);
    y += row + 8;
    MoveCtl(ID_HIDE_DESKTOP, lx, y, kClientW - 2 * lx, 22);
    y += 26;
    MoveCtl(ID_HIDE_CAPTURE, lx, y, kClientW - 2 * lx, 22);
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
    const wchar_t* labels[] = {L"Pattern", L"Opacity", L"Brightness", L"Contrast", L"Size", L"Base color", L"Image"};
    for (int i = 0; i < 7; i++)
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
    if (fromSettingsChange && !g_configHotkeyActive)
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
    if (g_hwndOverlay)
        DestroyWindow(g_hwndOverlay);
    g_hwndOverlay = nullptr;
    DestroyWindow(g_hwndMain);
}

LRESULT CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TIMER:
            if (wParam == TIMER_REPOSITION) {
                Reposition();
            } else if (wParam == TIMER_SAVE) {
                KillTimer(hwnd, TIMER_SAVE);
                SaveAppearance();
            } else if (wParam == TIMER_FIT) {
                KillTimer(hwnd, TIMER_FIT);
                FitOverlay();
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
        case WM_ERASEBKGND:
            if (g_brush) {
                RECT rc;
                GetClientRect(hwnd, &rc);
                FillRect((HDC)wParam, &rc, g_brush);
            }
            return 1;
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

    g_icon = LoadSystemIcon(201, GetSystemMetricsForDpi(SM_CXICON, GetDpiForSystem()));

    LoadAppearance();

    HINSTANCE inst = GetModuleHandleW(nullptr);
    RegisterClasses(inst);

    // A regular (not message-only) window, so it receives the WM_DISPLAYCHANGE broadcast.
    g_hwndMain = CreateWindowExW(WS_EX_TOOLWINDOW, kMainClass, L"Background Dimmer", WS_POPUP, 0, 0, 0, 0, nullptr,
                                 nullptr, inst, nullptr);
    g_hwndOverlay = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
                                    kOverlayClass, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, inst, nullptr);
    // Read after g_hwndMain exists, so a settings change saved meanwhile is queued as WM_APP_RELOAD, not lost.
    LoadModSettings();
    if (g_hwndMain && g_hwndOverlay) {
        BuildBrush();
        ApplyOpacity();
        ApplyCaptureExclusion();
        FitOverlay();
        g_hook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, WinEventProc, 0, 0,
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
    g_brush = nullptr;
    g_brushBitmap = nullptr;
    if (g_icon && g_iconExtracted)
        DestroyIcon(g_icon);
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
