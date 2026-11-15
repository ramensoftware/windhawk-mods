// ==WindhawkMod==
// @id              orbit-radial-launcher
// @name            ORBIT - Radial Launcher
// @description     A lightweight, animated radial application launcher for Windows.
// @version         0.7.0
// @author          Arihant Jain
// @github          https://github.com/ArihantJain2008
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lshell32 -lole32 -loleaut32 -luuid -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# ORBIT - Radial Launcher

Press the hotkey (default **Ctrl + Alt + Space**) or **hold the right mouse
button** (without moving) to open a radial menu at the mouse cursor.

Only one wheel can exist at a time. Clicking anywhere outside the wheel
closes it, no matter how it was opened (hotkey or right-click hold).

* **Mouse**: hover a slot, click to launch. Click the centre, right-click
  or click elsewhere to dismiss. When opened by holding right-click, just
  release the button over a slot to launch it.
* **Keyboard**: `1`-`8` launch a slot, arrow keys / Tab move the selection,
  Enter or Space launches it, Esc closes.
* The hovered slot springs up in size, lights up with a glow, and its name
  appears in a pill at the centre.
* Whole wedges are hit targets (not just the icon), so it is fast to use.
* Icons are pulled from the target application automatically. If none can be
  found, the first letter of the slot name is shown.
* The menu is clamped to the monitor and is DPI aware.

## File actions

Hold the right button on a file in Explorer or on the desktop and the wheel
shows actions for that file instead of apps:

* Images: convert to JPG / PNG / BMP (formats other than the current one)
* Word documents: convert to PDF (needs Microsoft Word installed)
* Any file(s): ZIP, Copy path
* **Apps**: switches the wheel to your normal app launcher

Converted files are saved next to the original and never overwrite an
existing file (a `.converted` suffix is added instead). If the selected
file is already highlighted, the whole selection is used (multi-file zip).

A quick right-click still opens the normal context menu. Windows running as
administrator are not intercepted.

## Configuring slots

Each slot is one string in the form `Name|Command|Arguments`
(arguments optional). Slots run clockwise starting at the top, up to 8.
Environment variables such as `%LOCALAPPDATA%` are expanded.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hotkey: "Ctrl+Alt+Space"
  $name: Hotkey
  $description: Modifiers (Ctrl, Alt, Shift, Win) plus a key (A-Z, 0-9, F1-F24, Space, Tab, Enter), joined with +
- holdEnabled: true
  $name: Hold right-click to open
  $description: Hold the right button still to open the wheel. A normal quick right-click still works.
- holdMs: 380
  $name: Hold time (ms)
  $description: How long to hold before the wheel opens (200-1500).
- fileActions: true
  $name: File actions
  $description: Holding right-click on a file in Explorer or the desktop shows convert/zip actions instead of apps.
- accentColor: "2F8CFF"
  $name: Accent colour
  $description: Hex colour, RRGGBB
- fadeMs: 140
  $name: Open animation duration (ms)
  $description: Fade and pop-in time. Set to 0 to disable the open animation.
- hoverScale: 26
  $name: Hover enlargement (%)
  $description: How much the hovered slot grows (0-60).
- slots:
  - 'Browser|msedge.exe'
  - 'Code|%LOCALAPPDATA%\Programs\Microsoft VS Code\Code.exe'
  - 'Files|explorer.exe'
  - 'Terminal|wt.exe'
  - 'Settings|ms-settings:'
  - 'Notepad|notepad.exe'
  $name: Slots
  $description: "Format: Name|Command|Arguments (arguments optional). Up to 8 slots, clockwise from the top."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>
#include <exdisp.h>
#include <servprov.h>
#include <uiautomation.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <string>
#include <vector>

// --------------------------------------------------
// Configuration constants
// --------------------------------------------------

static constexpr wchar_t WINDOW_CLASS[] = L"ORBIT_Overlay_Window";
// Only one explorer.exe process may own the wheel at a time.
static constexpr wchar_t SINGLETON_MUTEX[] = L"Local\\ORBIT_Radial_Launcher_Singleton";

static constexpr int HOTKEY_ID = 1;
static constexpr UINT_PTR TIMER_ANIM = 1;
static constexpr UINT_PTR TIMER_HOLD = 2;
static constexpr UINT_PTR TIMER_SEL = 3;
static constexpr UINT WM_ORBIT_RELOAD = WM_APP + 1;
static constexpr UINT WM_ORBIT_RELEASE = WM_APP + 2;
static constexpr ULONG_PTR ORBIT_INJECT = 0x4F524254;  // marks our own SendInput events

static constexpr int MAX_SLOTS = 8;
static constexpr int ICON_MAX = 80;
static constexpr float PI = 3.14159265f;

// Base geometry at 100% DPI (scaled at runtime).
static constexpr float BASE_SIZE = 232.0f;    // window size (includes glow room)
static constexpr float BASE_OUTER = 96.0f;    // ring radius
static constexpr float BASE_INNER = 24.0f;    // centre "cancel" radius
static constexpr float BASE_ORBIT = 63.0f;    // radius the bubbles sit on
static constexpr float BASE_BUBBLE = 20.0f;   // idle bubble radius
static constexpr float BASE_BORDER = 2.2f;    // ring thickness
static constexpr float BASE_ICON = 24.0f;     // icon size
static constexpr float PILL_HW = 34.0f;       // name pill half width
static constexpr float PILL_HH = 11.0f;       // name pill half height

// GUIDs defined locally so nothing depends on which import libs provide them.
static const GUID ORBIT_CLSID_UIA =
    {0xff48dba4, 0x60ef, 0x4201, {0xaa, 0x87, 0x54, 0x10, 0x3e, 0xef, 0x59, 0x4e}};
static const GUID ORBIT_IID_UIA =
    {0x30cbe57d, 0xd9d0, 0x452a, {0xab, 0x13, 0x7a, 0xc5, 0xac, 0x48, 0x25, 0xee}};
static const GUID ORBIT_CLSID_SHELLWINDOWS =
    {0x9ba05972, 0xf6a8, 0x11cf, {0xa4, 0x42, 0x00, 0xa0, 0xc9, 0x0a, 0x8f, 0x39}};
static const GUID ORBIT_IID_SHELLWINDOWS =
    {0x85cb6900, 0x4d95, 0x11cf, {0x96, 0x0c, 0x00, 0x80, 0xc7, 0xf4, 0xee, 0x85}};
static const GUID ORBIT_IID_WEBBROWSERAPP =
    {0x0002df05, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
static const GUID ORBIT_IID_FOLDERVIEW2 =
    {0x1af3a467, 0x214f, 0x4298, {0x90, 0x8e, 0x06, 0xb0, 0x3e, 0x0b, 0x39, 0xf9}};
static const GUID ORBIT_IID_SHELLBROWSER =
    {0x000214e2, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
static const GUID ORBIT_IID_SERVICEPROVIDER =
    {0x6d5140c1, 0x7436, 0x11ce, {0x80, 0x34, 0x00, 0xaa, 0x00, 0x60, 0x09, 0x1a}};
static const GUID ORBIT_SID_TOPLEVEL =
    {0x4c96be40, 0x915c, 0x11cf, {0x99, 0xd3, 0x00, 0xaa, 0x00, 0x4a, 0xe8, 0x37}};

// --------------------------------------------------
// Types and global state
// --------------------------------------------------

enum {
    ACT_LAUNCH = 0,
    ACT_PNG,
    ACT_JPG,
    ACT_BMP,
    ACT_PDF,
    ACT_ZIP,
    ACT_COPY,
    ACT_APPS
};

enum HoldState { HS_IDLE, HS_PENDING, HS_HELD, HS_PASS };

struct Slot {
    wchar_t name[48];
    wchar_t command[520];
    wchar_t args[260];
    bool hasIcon;
    DWORD icon[ICON_MAX * ICON_MAX];  // premultiplied BGRA
    int action;                       // ACT_LAUNCH for app slots
};

static Slot g_slots[MAX_SLOTS];
static int g_slotCount = 0;

// Saved app slots while the wheel is showing file actions
static Slot g_appSlots[MAX_SLOTS];
static int g_appCount = 0;
static bool g_fileMode = false;
static std::vector<std::wstring> g_files;

// Thread / lifetime
static HANDLE g_threadHandle = nullptr;
static HANDLE g_readyEvent = nullptr;
static HANDLE g_quitEvent = nullptr;   // wakes a standby thread on unload
static HANDLE g_mutex = nullptr;       // cross-process single instance
static DWORD g_threadId = 0;
static HWND g_overlayWindow = nullptr;
static LONG g_threadStatus = 0;

// Hold-right-click
static HHOOK g_mouseHook = nullptr;
static HoldState g_hs = HS_IDLE;
static POINT g_holdPt = {};
static bool g_holdEnabled = true;
static bool g_fileActions = true;
static int g_holdMs = 380;

// Interaction
static int g_hovered = -1;
static bool g_trackingMouse = false;
static bool g_hotkeyRegistered = false;

// Animation
static BYTE g_alpha = 255;
static int g_fadeMs = 140;
static float g_hoverGrow = 0.26f;
static ULONGLONG g_shownAt = 0;
static ULONGLONG g_lastTick = 0;
static bool g_animRunning = false;
static float g_intro = 1.0f;          // normalised open animation time
static float g_hv[MAX_SLOTS];         // per slot hover spring position
static float g_hvel[MAX_SLOTS];       // per slot hover spring velocity
static float g_pill = 0.0f;           // name pill visibility
static float g_pillVel = 0.0f;
static int g_pillIdx = 0;             // slot whose name the pill shows

// Appearance
static float g_accentR = 47, g_accentG = 140, g_accentB = 255;

// Layout (computed from DPI)
static float g_scale = 1.0f;
static int g_size = 232;
static float g_outer = BASE_OUTER;
static float g_inner = BASE_INNER;
static float g_orbit = BASE_ORBIT;
static float g_bubble = BASE_BUBBLE;
static float g_border = BASE_BORDER;
static int g_iconPx = 24;

// Persistent backing surface
static HDC g_dibDC = nullptr;
static HBITMAP g_dib = nullptr;
static HGDIOBJ g_dibOld = nullptr;
static DWORD* g_pixels = nullptr;
static DWORD* g_base = nullptr;  // pre-rendered static layer (glow, body, ring)

// --------------------------------------------------
// Small helpers
// --------------------------------------------------

static inline float Clamp01(float v) {
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}

static inline int IMin(int a, int b) { return a < b ? a : b; }
static inline int IMax(int a, int b) { return a > b ? a : b; }

static inline float EaseOutBack(float t) {
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.0f;
    float u = t - 1.0f;
    return 1.0f + c3 * u * u * u + c1 * u * u;
}

static void TrimInPlace(wchar_t* s) {
    wchar_t* b = s;
    while (*b == L' ' || *b == L'\t') b++;
    size_t n = wcslen(b);
    while (n && (b[n - 1] == L' ' || b[n - 1] == L'\t')) b[--n] = 0;
    if (b != s) memmove(s, b, (n + 1) * sizeof(wchar_t));
}

static void StripQuotes(wchar_t* s) {
    size_t n = wcslen(s);
    if (n >= 2 && s[0] == L'"' && s[n - 1] == L'"') {
        memmove(s, s + 1, (n - 2) * sizeof(wchar_t));
        s[n - 2] = 0;
    }
}

// Floating point premultiplied pixel used by the static layer renderer.
struct Px { float r, g, b, a; };

static inline void Over(Px& d, float r, float g, float b, float a) {
    float ia = 1.0f - a;
    d.r = r * a + d.r * ia;
    d.g = g * a + d.g * ia;
    d.b = b * a + d.b * ia;
    d.a = a + d.a * ia;
}

static inline DWORD ToByte(float v) {
    return v <= 0.0f ? 0 : (v >= 255.0f ? 255 : (DWORD)(v + 0.5f));
}

// Composite a premultiplied source pixel over a premultiplied destination.
static inline void BlendPremul(DWORD* dst, DWORD src, DWORD mul) {
    DWORD sa = (((src >> 24) & 255) * mul + 127) / 255;
    if (!sa) return;
    DWORD sr = ((((src >> 16) & 255)) * mul + 127) / 255;
    DWORD sg = ((((src >> 8) & 255)) * mul + 127) / 255;
    DWORD sb = (((src & 255)) * mul + 127) / 255;

    DWORD d = *dst;
    DWORD ia = 255 - sa;
    DWORD oa = sa + (((d >> 24) & 255) * ia + 127) / 255;
    DWORD orr = sr + (((d >> 16) & 255) * ia + 127) / 255;
    DWORD og = sg + (((d >> 8) & 255) * ia + 127) / 255;
    DWORD ob = sb + ((d & 255) * ia + 127) / 255;
    if (oa > 255) oa = 255;
    if (orr > 255) orr = 255;
    if (og > 255) og = 255;
    if (ob > 255) ob = 255;
    *dst = (oa << 24) | (orr << 16) | (og << 8) | ob;
}

// Blend a straight (non-premultiplied) colour with alpha onto the premultiplied
// destination. r/g/b are 0..255, a is 0..1.
static inline void BlendF(DWORD* dst, float r, float g, float b, float a) {
    if (a <= 0.003f) return;
    if (a > 1.0f) a = 1.0f;
    DWORD d = *dst;
    float ia = 1.0f - a;
    float da = (float)((d >> 24) & 255);
    float dr = (float)((d >> 16) & 255);
    float dg = (float)((d >> 8) & 255);
    float db = (float)(d & 255);
    float outA = a * 255.0f + da * ia;
    float outR = r * a + dr * ia;
    float outG = g * a + dg * ia;
    float outB = b * a + db * ia;
    *dst = (ToByte(outA) << 24) | (ToByte(outR) << 16) |
           (ToByte(outG) << 8) | ToByte(outB);
}

// --------------------------------------------------
// Layout / geometry
// --------------------------------------------------

static void UpdateLayout() {
    HDC dc = GetDC(nullptr);
    int dpi = dc ? GetDeviceCaps(dc, LOGPIXELSX) : 96;
    if (dc) ReleaseDC(nullptr, dc);

    g_scale = dpi / 96.0f;
    if (g_scale < 1.0f) g_scale = 1.0f;
    if (g_scale > 3.0f) g_scale = 3.0f;

    g_size = (int)lroundf(BASE_SIZE * g_scale);
    g_outer = BASE_OUTER * g_scale;
    g_inner = BASE_INNER * g_scale;
    g_orbit = BASE_ORBIT * g_scale;
    g_bubble = BASE_BUBBLE * g_scale;
    g_border = BASE_BORDER * g_scale;
    g_iconPx = (int)lroundf(BASE_ICON * g_scale);
    if (g_iconPx > ICON_MAX) g_iconPx = ICON_MAX;
}

// Returns the wedge index for an offset from the centre. Slot 0 is centred
// on the top and slots proceed clockwise.
static int SectorAt(float dx, float dy, float* phiDeg = nullptr) {
    if (g_slotCount <= 0) return -1;
    float sector = 360.0f / g_slotCount;
    float theta = atan2f(dy, dx) * (180.0f / PI) + 90.0f + sector * 0.5f;
    theta = fmodf(theta, 360.0f);
    if (theta < 0.0f) theta += 360.0f;
    int idx = (int)(theta / sector);
    if (idx >= g_slotCount) idx = g_slotCount - 1;
    if (phiDeg) *phiDeg = theta - idx * sector;
    return idx;
}

static int HitTest(int x, int y) {
    float c = g_size / 2.0f;
    float dx = x + 0.5f - c;
    float dy = y + 0.5f - c;
    float d = sqrtf(dx * dx + dy * dy);
    if (d < g_inner || d > g_outer) return -1;
    return SectorAt(dx, dy);
}

// True if a screen point lies inside the wheel's circle (not just its
// bounding square, whose corners are transparent and click-through).
static bool PointInWheel(POINT pt) {
    if (!g_overlayWindow) return false;
    RECT r;
    if (!GetWindowRect(g_overlayWindow, &r)) return false;
    float cx = (r.left + r.right) / 2.0f;
    float cy = (r.top + r.bottom) / 2.0f;
    float dx = pt.x + 0.5f - cx;
    float dy = pt.y + 0.5f - cy;
    return sqrtf(dx * dx + dy * dy) <= g_outer + 1.0f;
}

// --------------------------------------------------
// Backing surface
// --------------------------------------------------

static void DestroySurface() {
    if (g_dibDC) {
        if (g_dibOld) SelectObject(g_dibDC, g_dibOld);
        DeleteDC(g_dibDC);
    }
    if (g_dib) DeleteObject(g_dib);
    if (g_base) free(g_base);
    g_dibDC = nullptr;
    g_dib = nullptr;
    g_dibOld = nullptr;
    g_pixels = nullptr;
    g_base = nullptr;
}

static bool EnsureSurface() {
    DestroySurface();

    HDC screen = GetDC(nullptr);
    if (!screen) return false;

    g_dibDC = CreateCompatibleDC(screen);

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = g_size;
    bi.bmiHeader.biHeight = -g_size;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    g_dib = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, screen);

    g_base = (DWORD*)calloc((size_t)g_size * g_size, sizeof(DWORD));

    if (!g_dibDC || !g_dib || !bits || !g_base) {
        DestroySurface();
        return false;
    }

    g_dibOld = SelectObject(g_dibDC, g_dib);
    g_pixels = static_cast<DWORD*>(bits);
    return true;
}

static bool Present(HWND hwnd, BYTE alpha, const POINT* pos) {
    POINT src = {0, 0};
    SIZE sz = {g_size, g_size};

    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = alpha;
    blend.AlphaFormat = AC_SRC_ALPHA;

    BOOL ok = UpdateLayeredWindow(
        hwnd, nullptr, const_cast<POINT*>(pos), &sz,
        g_dibDC, &src, 0, &blend, ULW_ALPHA);

    if (!ok) {
        Wh_Log(L"ORBIT: UpdateLayeredWindow failed: %u", GetLastError());
        return false;
    }
    return true;
}

// --------------------------------------------------
// Text (GDI glyph coverage -> proper alpha)
// --------------------------------------------------

// GDI cannot write alpha, so text is rendered white-on-black into a scratch
// bitmap and its brightness is used as coverage for our premultiplied buffer.
static void DrawTextMasked(
    int cx, int cy, int w, int h,
    const wchar_t* text, int fontPx, int weight,
    BYTE r, BYTE g, BYTE b, DWORD mul
) {
    if (!g_pixels || w <= 0 || h <= 0 || mul == 0) return;

    HDC screen = GetDC(nullptr);
    if (!screen) return;
    HDC mdc = CreateCompatibleDC(screen);
    ReleaseDC(nullptr, screen);
    if (!mdc) return;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(mdc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bmp || !bits) {
        if (bmp) DeleteObject(bmp);
        DeleteDC(mdc);
        return;
    }

    HGDIOBJ oldBmp = SelectObject(mdc, bmp);

    HFONT font = CreateFontW(
        -fontPx, 0, 0, 0, weight, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    HGDIOBJ oldFont = SelectObject(mdc, font);

    SetBkMode(mdc, TRANSPARENT);
    SetTextColor(mdc, RGB(255, 255, 255));

    RECT rc = {0, 0, w, h};
    DrawTextW(mdc, text, -1, &rc,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE |
              DT_END_ELLIPSIS | DT_NOPREFIX);
    GdiFlush();

    const DWORD* src = static_cast<const DWORD*>(bits);
    int x0 = cx - w / 2;
    int y0 = cy - h / 2;

    for (int y = 0; y < h; y++) {
        int dy = y0 + y;
        if (dy < 0 || dy >= g_size) continue;
        for (int x = 0; x < w; x++) {
            int dx = x0 + x;
            if (dx < 0 || dx >= g_size) continue;

            DWORD cov = (src[y * w + x] >> 8) & 255;
            if (!cov) continue;

            DWORD pm =
                (cov << 24) |
                (((r * cov + 127) / 255) << 16) |
                (((g * cov + 127) / 255) << 8) |
                ((b * cov + 127) / 255);
            BlendPremul(&g_pixels[dy * g_size + dx], pm, mul);
        }
    }

    SelectObject(mdc, oldFont);
    DeleteObject(font);
    SelectObject(mdc, oldBmp);
    DeleteObject(bmp);
    DeleteDC(mdc);
}

// --------------------------------------------------
// Anti-aliased primitives (dynamic layer)
// --------------------------------------------------

static void FillCircle(float cx, float cy, float r,
                       float R, float G, float B, float A) {
    if (r <= 0.0f || A <= 0.003f) return;
    int x0 = IMax(0, (int)floorf(cx - r - 1.0f));
    int x1 = IMin(g_size - 1, (int)ceilf(cx + r + 1.0f));
    int y0 = IMax(0, (int)floorf(cy - r - 1.0f));
    int y1 = IMin(g_size - 1, (int)ceilf(cy + r + 1.0f));

    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            float dx = x + 0.5f - cx;
            float dy = y + 0.5f - cy;
            float d = sqrtf(dx * dx + dy * dy);
            float cov = Clamp01(r + 0.5f - d);
            if (cov > 0.0f) BlendF(&g_pixels[y * g_size + x], R, G, B, A * cov);
        }
    }
}

static void RingCircle(float cx, float cy, float r, float w,
                       float R, float G, float B, float A) {
    if (r <= 0.0f || A <= 0.003f) return;
    float ext = r + w + 1.0f;
    int x0 = IMax(0, (int)floorf(cx - ext));
    int x1 = IMin(g_size - 1, (int)ceilf(cx + ext));
    int y0 = IMax(0, (int)floorf(cy - ext));
    int y1 = IMin(g_size - 1, (int)ceilf(cy + ext));

    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            float dx = x + 0.5f - cx;
            float dy = y + 0.5f - cy;
            float d = sqrtf(dx * dx + dy * dy);
            float cov = Clamp01(w * 0.5f + 0.5f - fabsf(d - r));
            if (cov > 0.0f) BlendF(&g_pixels[y * g_size + x], R, G, B, A * cov);
        }
    }
}

// Soft glow that is full strength up to r0 and fades out over `ext`.
// Clipped so it never spills outside the menu ring.
static void GlowCircle(float cx, float cy, float r0, float ext,
                       float R, float G, float B, float A) {
    if (A <= 0.003f) return;
    float c = g_size / 2.0f;
    float clipR = g_outer - g_border;
    float reach = r0 + ext;
    int x0 = IMax(0, (int)floorf(cx - reach));
    int x1 = IMin(g_size - 1, (int)ceilf(cx + reach));
    int y0 = IMax(0, (int)floorf(cy - reach));
    int y1 = IMin(g_size - 1, (int)ceilf(cy + reach));

    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            float dx = x + 0.5f - cx;
            float dy = y + 0.5f - cy;
            float d = sqrtf(dx * dx + dy * dy);
            if (d > reach) continue;

            float t = d <= r0 ? 1.0f : 1.0f - (d - r0) / ext;
            float ex = x + 0.5f - c;
            float ey = y + 0.5f - c;
            float dc = sqrtf(ex * ex + ey * ey);
            float clip = Clamp01(clipR - dc);
            BlendF(&g_pixels[y * g_size + x], R, G, B, A * t * t * clip);
        }
    }
}

// Draws a premultiplied icon scaled to `ds` pixels (bilinear), centred on fx,fy.
static void DrawIconScaled(const DWORD* icon, int px,
                           float fx, float fy, float ds, float mul) {
    if (ds < 1.0f || mul <= 0.003f) return;
    float x0f = fx - ds * 0.5f;
    float y0f = fy - ds * 0.5f;
    int xs = IMax(0, (int)floorf(x0f));
    int xe = IMin(g_size - 1, (int)ceilf(x0f + ds));
    int ys = IMax(0, (int)floorf(y0f));
    int ye = IMin(g_size - 1, (int)ceilf(y0f + ds));

    auto T = [&](int xx, int yy) -> DWORD {
        return (xx < 0 || yy < 0 || xx >= px || yy >= px) ? 0u : icon[yy * px + xx];
    };

    for (int y = ys; y <= ye; y++) {
        for (int x = xs; x <= xe; x++) {
            float u = ((x + 0.5f - x0f) / ds) * px - 0.5f;
            float v = ((y + 0.5f - y0f) / ds) * px - 0.5f;
            int iu = (int)floorf(u);
            int iv = (int)floorf(v);
            float fu = u - iu;
            float fv = v - iv;

            DWORD t00 = T(iu, iv), t10 = T(iu + 1, iv);
            DWORD t01 = T(iu, iv + 1), t11 = T(iu + 1, iv + 1);
            float w00 = (1 - fu) * (1 - fv), w10 = fu * (1 - fv);
            float w01 = (1 - fu) * fv, w11 = fu * fv;

            float a = ((t00 >> 24) & 255) * w00 + ((t10 >> 24) & 255) * w10 +
                      ((t01 >> 24) & 255) * w01 + ((t11 >> 24) & 255) * w11;
            if (a < 0.5f) continue;
            float r = ((t00 >> 16) & 255) * w00 + ((t10 >> 16) & 255) * w10 +
                      ((t01 >> 16) & 255) * w01 + ((t11 >> 16) & 255) * w11;
            float g = ((t00 >> 8) & 255) * w00 + ((t10 >> 8) & 255) * w10 +
                      ((t01 >> 8) & 255) * w01 + ((t11 >> 8) & 255) * w11;
            float b = (t00 & 255) * w00 + (t10 & 255) * w10 +
                      (t01 & 255) * w01 + (t11 & 255) * w11;

            DWORD* dst = &g_pixels[y * g_size + x];
            DWORD d = *dst;
            float ia = 1.0f - (a * mul) / 255.0f;
            float oA = a * mul + ((d >> 24) & 255) * ia;
            float oR = r * mul + ((d >> 16) & 255) * ia;
            float oG = g * mul + ((d >> 8) & 255) * ia;
            float oB = b * mul + (d & 255) * ia;
            *dst = (ToByte(oA) << 24) | (ToByte(oR) << 16) |
                   (ToByte(oG) << 8) | ToByte(oB);
        }
    }
}

// --------------------------------------------------
// Rendering
// --------------------------------------------------

// Static layer: outer glow, dark body, orbit track and glowing ring.
// Only needs redrawing when the layout or accent colour changes.
static void RenderBase() {
    if (!g_base) return;

    const int size = g_size;
    const float half = size / 2.0f;
    const float s = g_scale;
    const float R = g_outer;

    const float ar = g_accentR, ag = g_accentG, ab = g_accentB;
    const float lr = ar + (255.0f - ar) * 0.35f;
    const float lg = ag + (255.0f - ag) * 0.35f;
    const float lb = ab + (255.0f - ab) * 0.35f;

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            float dx = x + 0.5f - half;
            float dy = y + 0.5f - half;
            float d = sqrtf(dx * dx + dy * dy);

            Px p = {0, 0, 0, 0};

            // Outer glow.
            if (d > R - 1.0f) {
                float ga = 0.55f * expf(-fmaxf(d - R, 0.0f) / (5.5f * s));
                ga *= Clamp01((half - d) / (10.0f * s));
                Over(p, ar, ag, ab, ga);
            }

            float mask = Clamp01(R + 0.5f - d);
            if (mask > 0.0f) {
                // Body with a faint radial gradient.
                float tt = Clamp01(1.0f - d / R);
                Over(p, 5.0f + 9.0f * tt, 8.0f + 16.0f * tt, 16.0f + 34.0f * tt,
                     0.96f * mask);

                // Soft inner glow hugging the ring.
                float ig = 0.22f * expf(-(R - d) / (11.0f * s));
                Over(p, ar, ag, ab, ig * mask);

                // Faint orbit track.
                float tr = Clamp01(0.9f - fabsf(d - g_orbit));
                Over(p, 255, 255, 255, 0.05f * tr);

                // Ring.
                float bc = Clamp01(g_border * 0.5f + 0.5f -
                                   fabsf(d - (R - g_border * 0.5f)));
                Over(p, lr, lg, lb, 0.98f * bc);
            }

            g_base[y * size + x] =
                (ToByte(p.a * 255.0f) << 24) |
                (ToByte(p.r) << 16) |
                (ToByte(p.g) << 8) |
                ToByte(p.b);
        }
    }
}

static void DrawPill(float p) {
    if (p <= 0.01f || g_pillIdx < 0 || g_pillIdx >= g_slotCount) return;
    p = Clamp01(p);

    const float s = g_scale;
    const float k = 0.80f + 0.20f * p;
    const float hw = PILL_HW * s * k;
    const float hh = PILL_HH * s * k;
    const float L = hw - hh;
    const float ext = 10.0f * s;
    const float c = g_size / 2.0f;

    const float ar = g_accentR, ag = g_accentG, ab = g_accentB;
    const float lr = ar + (255.0f - ar) * 0.45f;
    const float lg = ag + (255.0f - ag) * 0.45f;
    const float lb = ab + (255.0f - ab) * 0.45f;
    const float fr = ar * 0.45f, fg = ag * 0.70f, fb = ab * 0.95f;

    int xs = IMax(0, (int)floorf(c - hw - ext));
    int xe = IMin(g_size - 1, (int)ceilf(c + hw + ext));
    int ys = IMax(0, (int)floorf(c - hh - ext));
    int ye = IMin(g_size - 1, (int)ceilf(c + hh + ext));

    for (int y = ys; y <= ye; y++) {
        for (int x = xs; x <= xe; x++) {
            float dx = x + 0.5f - c;
            float dy = y + 0.5f - c;
            float q = fabsf(dx) - L;
            float d = q < 0.0f ? fabsf(dy) : sqrtf(q * q + dy * dy);
            if (d > hh + ext) continue;

            DWORD* dst = &g_pixels[y * g_size + x];

            float ga = 0.34f * p * expf(-fmaxf(d - hh, 0.0f) / (4.0f * s));
            BlendF(dst, ar, ag, ab, ga);

            float cov = Clamp01(hh + 0.5f - d);
            if (cov > 0.0f) {
                float t = Clamp01((dy + hh) / (2.0f * hh));
                float sh = 1.0f - 0.34f * t;
                BlendF(dst, fr * sh, fg * sh, fb * sh, 0.98f * cov * p);

                float edge = Clamp01(1.0f - (hh - d) / (1.5f * s));
                BlendF(dst, lr, lg, lb, 0.50f * edge * cov * p);
            }
        }
    }

    DWORD mul = (DWORD)(p * 255.0f + 0.5f);
    DrawTextMasked((int)c, (int)c, (int)(2.0f * (hw - 4.0f * s)), (int)(2.0f * hh),
                   g_slots[g_pillIdx].name, (int)(12.0f * s), FW_SEMIBOLD,
                   255, 255, 255, mul);
}

static void Render() {
    if (!g_pixels || !g_base) return;

    const int size = g_size;
    const float c = size / 2.0f;
    const float s = g_scale;
    const int n = g_slotCount;
    const float sector = 360.0f / (n > 0 ? n : 1);

    memcpy(g_pixels, g_base, (size_t)size * size * sizeof(DWORD));

    const float ar = g_accentR, ag = g_accentG, ab = g_accentB;
    const float lr = ar + (255.0f - ar) * 0.45f;
    const float lg = ag + (255.0f - ag) * 0.45f;
    const float lb = ab + (255.0f - ab) * 0.45f;

    // Draw the most "hovered" bubble last so it overlaps its neighbours.
    int order[MAX_SLOTS];
    for (int i = 0; i < n; i++) order[i] = i;
    for (int i = 1; i < n; i++) {
        int key = order[i];
        int j = i - 1;
        while (j >= 0 && g_hv[order[j]] > g_hv[key]) {
            order[j + 1] = order[j];
            j--;
        }
        order[j + 1] = key;
    }

    for (int k = 0; k < n; k++) {
        int i = order[k];
        float v = g_hv[i];
        float vc = Clamp01(v);

        // Staggered pop-in when the menu opens.
        float tt = g_fadeMs > 0 ? Clamp01((g_intro - i * 0.04f) / 0.7f) : 1.0f;
        float sc = EaseOutBack(tt);
        float al = Clamp01(tt * 1.6f);
        if (sc <= 0.01f) continue;

        float grow = 1.0f + g_hoverGrow * v;
        float rad = g_bubble * sc * grow;
        float orb = g_orbit * (0.72f + 0.28f * sc) + 3.0f * s * v;

        float ang = (-90.0f + i * sector) * PI / 180.0f;
        float fx = c + cosf(ang) * orb;
        float fy = c + sinf(ang) * orb;

        // Glow behind the hovered bubble.
        if (vc > 0.01f) {
            GlowCircle(fx, fy, rad, 14.0f * s, ar, ag, ab, 0.42f * vc * al);
        }

        // Idle look: soft dark-blue disc with a faint rim.
        FillCircle(fx, fy, rad, 150, 175, 220, 0.14f * (1.0f - vc) * al);
        RingCircle(fx, fy, rad - 0.5f * s, 1.0f * s, 170, 190, 230,
                   0.10f * (1.0f - vc) * al);

        // Hovered look: saturated accent disc with a bright rim.
        if (vc > 0.01f) {
            FillCircle(fx, fy, rad, ar * 0.35f, ag * 0.55f, ab * 0.85f,
                       0.92f * vc * al);
            RingCircle(fx, fy, rad - 0.9f * s, 1.8f * s, lr, lg, lb,
                       0.98f * vc * al);
        }

        // Icon (or fallback letter / short label in file mode).
        float iconMul = (0.80f + 0.20f * vc) * al;
        if (g_slots[i].hasIcon) {
            float ds = g_iconPx * sc * (1.0f + 0.5f * g_hoverGrow * v);
            DrawIconScaled(g_slots[i].icon, g_iconPx, fx, fy, ds, iconMul);
        } else {
            wchar_t letter[5] = {};
            if (g_fileMode) {
                lstrcpynW(letter, g_slots[i].name, 5);  // ZIP, PNG, Copy, Apps
            } else {
                letter[0] = g_slots[i].name[0];
                if (letter[0] >= L'a' && letter[0] <= L'z') letter[0] -= 32;
            }

            BYTE cr = (BYTE)(190.0f + 65.0f * vc);
            BYTE cg = (BYTE)(205.0f + 50.0f * vc);
            BYTE cb = (BYTE)(230.0f + 25.0f * vc);
            int box = (int)(g_iconPx * (g_fileMode ? 2.0f : 1.6f));
            int fontPx = (int)((g_fileMode ? 11.0f : 20.0f) * s * sc *
                               (1.0f + 0.5f * g_hoverGrow * v));
            if (fontPx < 4) continue;
            DrawTextMasked((int)fx, (int)fy, box, box, letter, fontPx, FW_BOLD,
                           cr, cg, cb, (DWORD)(iconMul * 255.0f + 0.5f));
        }
    }

    // Centre: idle dot when nothing is hovered, name pill when hovering.
    float idle = 1.0f - Clamp01(g_pill);
    if (idle > 0.01f) {
        FillCircle(c, c, 3.0f * s, ar, ag, ab, 0.9f * idle);
        RingCircle(c, c, 9.0f * s, 1.2f * s, ar, ag, ab, 0.35f * idle);
    }
    DrawPill(g_pill);
}

// --------------------------------------------------
// Settings
// --------------------------------------------------

static bool ParseSlot(const wchar_t* spec, Slot& slot) {
    wchar_t buf[900];
    lstrcpynW(buf, spec, ARRAYSIZE(buf));

    wchar_t* p1 = wcschr(buf, L'|');
    if (!p1) return false;
    *p1 = 0;

    wchar_t* cmd = p1 + 1;
    wchar_t empty[1] = {0};
    wchar_t* args = empty;

    wchar_t* p2 = wcschr(cmd, L'|');
    if (p2) {
        *p2 = 0;
        args = p2 + 1;
    }

    TrimInPlace(buf);
    TrimInPlace(cmd);
    TrimInPlace(args);

    if (!buf[0] || !cmd[0]) return false;

    lstrcpynW(slot.name, buf, ARRAYSIZE(slot.name));
    lstrcpynW(slot.command, cmd, ARRAYSIZE(slot.command));
    lstrcpynW(slot.args, args, ARRAYSIZE(slot.args));
    slot.hasIcon = false;
    slot.action = ACT_LAUNCH;
    return true;
}

static bool ParseHotkey(const wchar_t* text, UINT& mods, UINT& vk) {
    wchar_t buf[128];
    lstrcpynW(buf, text, ARRAYSIZE(buf));

    mods = MOD_NOREPEAT;
    vk = 0;

    wchar_t* p = buf;
    while (p && *p) {
        wchar_t* next = wcschr(p, L'+');
        if (next) {
            *next = 0;
            next++;
        }
        TrimInPlace(p);

        if (!_wcsicmp(p, L"ctrl") || !_wcsicmp(p, L"control")) {
            mods |= MOD_CONTROL;
        } else if (!_wcsicmp(p, L"alt")) {
            mods |= MOD_ALT;
        } else if (!_wcsicmp(p, L"shift")) {
            mods |= MOD_SHIFT;
        } else if (!_wcsicmp(p, L"win") || !_wcsicmp(p, L"windows")) {
            mods |= MOD_WIN;
        } else if (vk) {
            return false;  // two non-modifier keys
        } else if (!_wcsicmp(p, L"space")) {
            vk = VK_SPACE;
        } else if (!_wcsicmp(p, L"tab")) {
            vk = VK_TAB;
        } else if (!_wcsicmp(p, L"enter") || !_wcsicmp(p, L"return")) {
            vk = VK_RETURN;
        } else if (wcslen(p) == 1) {
            wchar_t ch = p[0];
            if (ch >= L'a' && ch <= L'z') ch -= 32;
            if ((ch >= L'A' && ch <= L'Z') || (ch >= L'0' && ch <= L'9')) {
                vk = (UINT)ch;
            } else {
                return false;
            }
        } else if ((p[0] == L'f' || p[0] == L'F') && p[1]) {
            int fn = _wtoi(p + 1);
            if (fn < 1 || fn > 24) return false;
            vk = VK_F1 + fn - 1;
        } else {
            return false;
        }

        p = next;
    }

    return vk != 0 && (mods & ~(UINT)MOD_NOREPEAT) != 0;
}

static void RegisterConfiguredHotkey() {
    if (g_hotkeyRegistered) {
        UnregisterHotKey(nullptr, HOTKEY_ID);
        g_hotkeyRegistered = false;
    }

    UINT mods = 0, vk = 0;
    PCWSTR text = Wh_GetStringSetting(L"hotkey");
    bool parsed = text && ParseHotkey(text, mods, vk);

    if (!parsed) {
        Wh_Log(L"ORBIT: Invalid hotkey '%s', using Ctrl+Alt+Space.",
               text ? text : L"");
        mods = MOD_CONTROL | MOD_ALT | MOD_NOREPEAT;
        vk = VK_SPACE;
    }
    if (text) Wh_FreeStringSetting(text);

    if (RegisterHotKey(nullptr, HOTKEY_ID, mods, vk)) {
        g_hotkeyRegistered = true;
    } else {
        Wh_Log(L"ORBIT: RegisterHotKey failed. Error: %u", GetLastError());
    }
}

static void LoadSettings() {
    // Hold-to-open and file actions.
    g_holdEnabled = Wh_GetIntSetting(L"holdEnabled") != 0;
    g_fileActions = Wh_GetIntSetting(L"fileActions") != 0;
    g_holdMs = Wh_GetIntSetting(L"holdMs");
    if (g_holdMs < 200) g_holdMs = 200;
    if (g_holdMs > 1500) g_holdMs = 1500;

    // Accent colour.
    g_accentR = 47; g_accentG = 140; g_accentB = 255;
    PCWSTR accent = Wh_GetStringSetting(L"accentColor");
    if (accent) {
        const wchar_t* s = accent;
        if (*s == L'#') s++;
        if (wcslen(s) == 6) {
            wchar_t* end = nullptr;
            unsigned long v = wcstoul(s, &end, 16);
            if (end && *end == 0) {
                g_accentR = (float)((v >> 16) & 255);
                g_accentG = (float)((v >> 8) & 255);
                g_accentB = (float)(v & 255);
            }
        }
        Wh_FreeStringSetting(accent);
    }

    g_fadeMs = Wh_GetIntSetting(L"fadeMs");
    if (g_fadeMs < 0) g_fadeMs = 0;
    if (g_fadeMs > 2000) g_fadeMs = 2000;

    int grow = Wh_GetIntSetting(L"hoverScale");
    if (grow < 0) grow = 0;
    if (grow > 60) grow = 60;
    g_hoverGrow = grow / 100.0f;

    // Slots.
    g_slotCount = 0;
    for (int i = 0; i < 32 && g_slotCount < MAX_SLOTS; i++) {
        PCWSTR spec = Wh_GetStringSetting(L"slots[%d]", i);
        if (!spec) continue;

        if (*spec) {
            if (ParseSlot(spec, g_slots[g_slotCount])) {
                g_slotCount++;
            } else {
                Wh_Log(L"ORBIT: Ignoring malformed slot: %s", spec);
            }
        }
        Wh_FreeStringSetting(spec);
    }

    Wh_Log(L"ORBIT: Loaded %d slot(s).", g_slotCount);
}

// --------------------------------------------------
// Icons
// --------------------------------------------------

static bool ResolveIconSource(const wchar_t* command, wchar_t* out, DWORD cch) {
    wchar_t expanded[MAX_PATH * 2];
    DWORD n = ExpandEnvironmentStringsW(command, expanded, ARRAYSIZE(expanded));
    if (!n || n > ARRAYSIZE(expanded)) return false;
    StripQuotes(expanded);

    if (GetFileAttributesW(expanded) != INVALID_FILE_ATTRIBUTES) {
        lstrcpynW(out, expanded, cch);
        return true;
    }

    if (SearchPathW(nullptr, expanded, L".exe", cch, out, nullptr)) {
        return true;
    }

    // Registered "App Paths" (e.g. msedge.exe).
    if (!wcspbrk(expanded, L"\\/:")) {
        wchar_t key[MAX_PATH * 2];
        const wchar_t* ext = wcsrchr(expanded, L'.');
        wsprintfW(key,
                  L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\%s%s",
                  expanded, ext ? L"" : L".exe");

        HKEY roots[2] = {HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER};
        for (HKEY root : roots) {
            DWORD size = cch * sizeof(wchar_t);
            if (RegGetValueW(root, key, nullptr,
                             RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ,
                             nullptr, out, &size) == ERROR_SUCCESS) {
                StripQuotes(out);
                if (GetFileAttributesW(out) != INVALID_FILE_ATTRIBUTES) {
                    return true;
                }
            }
        }
    }

    return false;
}

// Extracts true per-pixel alpha from a HICON by drawing it on black and on
// white backgrounds: alpha = 1 - (white - black), colour = black render.
static bool IconToPremulPixels(HICON icon, int px, DWORD* out) {
    HDC screen = GetDC(nullptr);
    if (!screen) return false;
    HDC mdc = CreateCompatibleDC(screen);
    ReleaseDC(nullptr, screen);
    if (!mdc) return false;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = px;
    bi.bmiHeader.biHeight = -px;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* onBlack = nullptr;
    void* onWhite = nullptr;
    HBITMAP bb = CreateDIBSection(mdc, &bi, DIB_RGB_COLORS, &onBlack, nullptr, 0);
    HBITMAP bw = CreateDIBSection(mdc, &bi, DIB_RGB_COLORS, &onWhite, nullptr, 0);

    bool ok = false;
    if (bb && bw && onBlack && onWhite) {
        const size_t count = (size_t)px * px;
        memset(onBlack, 0, count * 4);
        memset(onWhite, 0xFF, count * 4);

        HGDIOBJ old = SelectObject(mdc, bb);
        DrawIconEx(mdc, 0, 0, icon, px, px, 0, nullptr, DI_NORMAL);
        SelectObject(mdc, bw);
        DrawIconEx(mdc, 0, 0, icon, px, px, 0, nullptr, DI_NORMAL);
        SelectObject(mdc, old);
        GdiFlush();

        const DWORD* B = static_cast<const DWORD*>(onBlack);
        const DWORD* W = static_cast<const DWORD*>(onWhite);

        for (size_t i = 0; i < count; i++) {
            int dr = (int)((W[i] >> 16) & 255) - (int)((B[i] >> 16) & 255);
            int dg = (int)((W[i] >> 8) & 255) - (int)((B[i] >> 8) & 255);
            int db = (int)(W[i] & 255) - (int)(B[i] & 255);
            int a = 255 - (dr + dg + db) / 3;
            if (a < 0) a = 0;
            if (a > 255) a = 255;

            DWORD rgb = B[i] & 0x00FFFFFF;
            // Keep premultiplied colour within alpha.
            DWORD r = (rgb >> 16) & 255, g = (rgb >> 8) & 255, b = rgb & 255;
            if (r > (DWORD)a) r = a;
            if (g > (DWORD)a) g = a;
            if (b > (DWORD)a) b = a;
            out[i] = ((DWORD)a << 24) | (r << 16) | (g << 8) | b;
        }
        ok = true;
    }

    if (bb) DeleteObject(bb);
    if (bw) DeleteObject(bw);
    DeleteDC(mdc);
    return ok;
}

static void LoadSlotIcons() {
    for (int i = 0; i < g_slotCount; i++) {
        Slot& s = g_slots[i];
        s.hasIcon = false;

        wchar_t path[MAX_PATH * 2];
        if (!ResolveIconSource(s.command, path, ARRAYSIZE(path))) continue;

        SHFILEINFOW sfi = {};
        if (!SHGetFileInfoW(path, 0, &sfi, sizeof(sfi),
                            SHGFI_ICON | SHGFI_LARGEICON) || !sfi.hIcon) {
            continue;
        }

        s.hasIcon = IconToPremulPixels(sfi.hIcon, g_iconPx, s.icon);
        DestroyIcon(sfi.hIcon);
    }
}

// --------------------------------------------------
// Animation
// --------------------------------------------------

static void StartAnim(HWND hwnd) {
    if (g_animRunning) return;
    g_lastTick = GetTickCount64();
    SetTimer(hwnd, TIMER_ANIM, 15, nullptr);
    g_animRunning = true;
}

static void StopAnim(HWND hwnd) {
    KillTimer(hwnd, TIMER_ANIM);
    g_animRunning = false;
}

// One spring step. Returns true while still moving.
static bool StepSpring(float& pos, float& vel, float target,
                       float k, float c, float h) {
    float a = k * (target - pos) - c * vel;
    vel += a * h;
    pos += vel * h;
    if (fabsf(target - pos) < 0.002f && fabsf(vel) < 0.02f) {
        pos = target;
        vel = 0.0f;
        return false;
    }
    return true;
}

static void AnimTick(HWND hwnd) {
    ULONGLONG now = GetTickCount64();
    float dt = (float)(now - g_lastTick) / 1000.0f;
    g_lastTick = now;
    if (dt > 0.05f) dt = 0.05f;
    if (dt < 0.001f) dt = 0.001f;

    bool busy = false;

    // Open animation: fade plus staggered pop-in.
    if (g_fadeMs > 0) {
        float elapsed = (float)(now - g_shownAt);
        float ft = Clamp01(elapsed / g_fadeMs);
        float inv = 1.0f - ft;
        g_alpha = (BYTE)((1.0f - inv * inv * inv) * 255.0f + 0.5f);

        g_intro = elapsed / (g_fadeMs * 2.0f);
        if (g_intro >= 1.0f) g_intro = 1.0f;
        else busy = true;
    } else {
        g_alpha = 255;
        g_intro = 1.0f;
    }

    // Hover springs (slightly under-damped, so they overshoot a little).
    const int sub = 2;
    const float h = dt / sub;
    for (int i = 0; i < g_slotCount; i++) {
        float target = (i == g_hovered) ? 1.0f : 0.0f;
        for (int k = 0; k < sub; k++) {
            if (StepSpring(g_hv[i], g_hvel[i], target, 380.0f, 22.0f, h)) {
                busy = true;
            }
        }
    }

    float pillTarget = g_hovered >= 0 ? 1.0f : 0.0f;
    for (int k = 0; k < sub; k++) {
        if (StepSpring(g_pill, g_pillVel, pillTarget, 500.0f, 45.0f, h)) {
            busy = true;
        }
    }

    Render();
    Present(hwnd, g_alpha, nullptr);

    if (!busy) StopAnim(hwnd);
}

// --------------------------------------------------
// Show / hide / launch
// --------------------------------------------------

// Defined further down (file actions section).
static void ExitFileMode();
static void SwitchToApps(HWND hwnd);
static void RunFileAction(int act);

static void FocusOverlay(HWND hwnd) {
    HWND fg = GetForegroundWindow();
    DWORD fgThread = fg ? GetWindowThreadProcessId(fg, nullptr) : 0;
    DWORD me = GetCurrentThreadId();

    bool attached = false;
    if (fgThread && fgThread != me) {
        attached = AttachThreadInput(me, fgThread, TRUE) != FALSE;
    }

    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    if (attached) AttachThreadInput(me, fgThread, FALSE);
}

// Single place that closes the wheel, used by every dismissal path
// (hotkey, outside click, Esc, launch, focus loss, reload).
static void HideOverlay(HWND hwnd) {
    StopAnim(hwnd);
    ShowWindow(hwnd, SW_HIDE);
    g_hovered = -1;
    g_trackingMouse = false;
    ExitFileMode();
}

static void ShowOverlay(HWND hwnd) {
    POINT cur = {};
    if (!GetCursorPos(&cur)) {
        Wh_Log(L"ORBIT: GetCursorPos failed.");
        return;
    }

    // Only ever one wheel: if it is already open, close it before reopening
    // at the new position.
    if (IsWindowVisible(hwnd)) {
        bool keepFileMode = g_fileMode;
        (void)keepFileMode;  // file mode is decided by the caller beforehand
        StopAnim(hwnd);
        ShowWindow(hwnd, SW_HIDE);
        g_hovered = -1;
        g_trackingMouse = false;
    }

    // Centre on the cursor but keep fully on the monitor.
    int x = cur.x - g_size / 2;
    int y = cur.y - g_size / 2;

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    HMONITOR mon = MonitorFromPoint(cur, MONITOR_DEFAULTTONEAREST);
    if (mon && GetMonitorInfoW(mon, &mi)) {
        if (x < mi.rcMonitor.left) x = mi.rcMonitor.left;
        if (y < mi.rcMonitor.top) y = mi.rcMonitor.top;
        if (x + g_size > mi.rcMonitor.right) x = mi.rcMonitor.right - g_size;
        if (y + g_size > mi.rcMonitor.bottom) y = mi.rcMonitor.bottom - g_size;
    }

    g_hovered = HitTest(cur.x - x, cur.y - y);

    // Reset animation state; an already-hovered slot starts fully lit.
    for (int i = 0; i < MAX_SLOTS; i++) {
        g_hv[i] = (i == g_hovered) ? 1.0f : 0.0f;
        g_hvel[i] = 0.0f;
    }
    g_pill = g_hovered >= 0 ? 1.0f : 0.0f;
    g_pillVel = 0.0f;
    g_pillIdx = g_hovered >= 0 ? g_hovered : 0;
    g_intro = g_fadeMs > 0 ? 0.0f : 1.0f;

    Render();

    POINT pos = {x, y};
    g_alpha = g_fadeMs > 0 ? 0 : 255;
    if (!Present(hwnd, g_alpha, &pos)) return;

    SetWindowPos(hwnd, HWND_TOPMOST, x, y, g_size, g_size, SWP_SHOWWINDOW);

    g_shownAt = GetTickCount64();
    g_animRunning = false;
    if (g_fadeMs > 0) StartAnim(hwnd);

    FocusOverlay(hwnd);
}

static void ToggleOverlay() {
    if (!g_overlayWindow) return;

    if (IsWindowVisible(g_overlayWindow)) {
        HideOverlay(g_overlayWindow);
    } else {
        ExitFileMode();
        g_files.clear();
        ShowOverlay(g_overlayWindow);
    }
}

static void LaunchSlot(HWND hwnd, int index) {
    if (index < 0 || index >= g_slotCount) return;

    // File-action wheel: run the action instead of launching an app.
    if (g_fileMode) {
        int act = g_slots[index].action;
        if (act == ACT_APPS) {
            SwitchToApps(hwnd);
            return;
        }
        HideOverlay(hwnd);
        RunFileAction(act);
        return;
    }

    const Slot& s = g_slots[index];
    HideOverlay(hwnd);

    wchar_t cmd[MAX_PATH * 2];
    wchar_t args[ARRAYSIZE(s.args) * 2];

    DWORD n = ExpandEnvironmentStringsW(s.command, cmd, ARRAYSIZE(cmd));
    if (!n || n > ARRAYSIZE(cmd)) lstrcpynW(cmd, s.command, ARRAYSIZE(cmd));

    n = ExpandEnvironmentStringsW(s.args, args, ARRAYSIZE(args));
    if (!n || n > ARRAYSIZE(args)) lstrcpynW(args, s.args, ARRAYSIZE(args));

    HINSTANCE r = ShellExecuteW(
        nullptr, L"open", cmd, args[0] ? args : nullptr,
        nullptr, SW_SHOWNORMAL);

    if ((INT_PTR)r <= 32) {
        Wh_Log(L"ORBIT: Failed to launch '%s' (code %d).",
               s.name, (int)(INT_PTR)r);
    } else {
        Wh_Log(L"ORBIT: Launched '%s'.", s.name);
    }
}

static void SetHovered(HWND hwnd, int idx) {
    if (idx == g_hovered) return;
    g_hovered = idx;
    if (idx >= 0) g_pillIdx = idx;
    StartAnim(hwnd);
}

// --------------------------------------------------
// Hold-right-click + file actions
// --------------------------------------------------

static void SendMouse(DWORD flags) {
    INPUT in = {};
    in.type = INPUT_MOUSE;
    in.mi.dwFlags = flags;
    in.mi.dwExtraInfo = ORBIT_INJECT;
    SendInput(1, &in, sizeof(in));
}

// Elevated windows reject our replayed clicks (UIPI), so never intercept them.
// Heuristic: if we cannot open the owning process token, assume it is elevated.
static bool CanIntercept(POINT pt) {
    HWND h = WindowFromPoint(pt);
    if (!h || h == g_overlayWindow) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (!pid) return false;
    if (pid == GetCurrentProcessId()) return true;
    HANDLE p = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!p) return false;
    HANDLE tok = nullptr;
    bool ok = OpenProcessToken(p, TOKEN_QUERY, &tok) != 0;
    if (tok) CloseHandle(tok);
    CloseHandle(p);
    return ok;
}

static LRESULT CALLBACK MouseHookProc(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION && g_overlayWindow) {
        const MSLLHOOKSTRUCT* ms = (const MSLLHOOKSTRUCT*)lp;
        if (ms->dwExtraInfo != ORBIT_INJECT) {

            // Universal dismissal: any button press outside the wheel closes
            // it, whether it was opened by hotkey or by holding right-click.
            // The click is not swallowed, so it still reaches the app below.
            // A right press then falls through to start a fresh hold, which
            // moves the (single) wheel to the new spot.
            if ((wp == WM_LBUTTONDOWN || wp == WM_RBUTTONDOWN ||
                 wp == WM_MBUTTONDOWN || wp == WM_XBUTTONDOWN) &&
                IsWindowVisible(g_overlayWindow) && !PointInWheel(ms->pt)) {
                HideOverlay(g_overlayWindow);
            }

            if (g_holdEnabled) {
                switch (wp) {
                case WM_RBUTTONDOWN:
                    if (g_hs == HS_IDLE && !IsWindowVisible(g_overlayWindow) &&
                        CanIntercept(ms->pt)) {
                        g_hs = HS_PENDING;
                        g_holdPt = ms->pt;
                        SetTimer(g_overlayWindow, TIMER_HOLD, g_holdMs, nullptr);
                        return 1;  // swallow; replayed if it turns out to be a click
                    }
                    break;
                case WM_RBUTTONUP:
                    if (g_hs == HS_PENDING) {  // quick click: replay normally
                        KillTimer(g_overlayWindow, TIMER_HOLD);
                        g_hs = HS_IDLE;
                        SendMouse(MOUSEEVENTF_RIGHTDOWN);
                        SendMouse(MOUSEEVENTF_RIGHTUP);
                        return 1;
                    }
                    if (g_hs == HS_HELD) {  // release over a slot launches it
                        g_hs = HS_IDLE;
                        PostMessageW(g_overlayWindow, WM_ORBIT_RELEASE, 0, 0);
                        return 1;
                    }
                    if (g_hs == HS_PASS) g_hs = HS_IDLE;
                    break;
                case WM_MOUSEMOVE:
                    if (g_hs == HS_PENDING) {  // moved: it's a drag, give it back
                        int dx = ms->pt.x - g_holdPt.x, dy = ms->pt.y - g_holdPt.y;
                        if (dx * dx + dy * dy > 36) {
                            KillTimer(g_overlayWindow, TIMER_HOLD);
                            g_hs = HS_PASS;
                            SendMouse(MOUSEEVENTF_RIGHTDOWN);
                        }
                    }
                    break;
                }
            }
        }
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

// ---- Explorer selection ----

static bool IsFileView(HWND h) {
    if (!h) return false;
    HWND par = GetParent(h);
    if (!par) return false;
    wchar_t c1[64] = {}, c2[64] = {};
    GetClassNameW(h, c1, 64);
    GetClassNameW(par, c2, 64);
    return !wcscmp(c2, L"SHELLDLL_DefView") &&
           (!wcscmp(c1, L"DirectUIHWND") || !wcscmp(c1, L"SysListView32"));
}

static bool ItemUnderCursorSelected(POINT pt) {
    bool sel = false;
    IUIAutomation* ua = nullptr;
    if (SUCCEEDED(CoCreateInstance(ORBIT_CLSID_UIA, nullptr, CLSCTX_INPROC_SERVER,
                                   ORBIT_IID_UIA, (void**)&ua)) && ua) {
        IUIAutomationElement* el = nullptr;
        if (SUCCEEDED(ua->ElementFromPoint(pt, &el)) && el) {
            VARIANT v;
            VariantInit(&v);
            if (SUCCEEDED(el->GetCurrentPropertyValue(
                    UIA_SelectionItemIsSelectedPropertyId, &v)) &&
                v.vt == VT_BOOL && v.boolVal) {
                sel = true;
            }
            VariantClear(&v);
            el->Release();
        }
        ua->Release();
    }
    return sel;
}

// Reads the selection of one shell window. If defView is set, the window's
// active view must match it (picks the right tab in tabbed Explorer).
static bool ReadSelection(IDispatch* disp, HWND defView,
                          std::vector<std::wstring>& out) {
    bool ok = false;
    IServiceProvider* sp = nullptr;
    IShellBrowser* sb = nullptr;
    IShellView* sv = nullptr;
    IFolderView2* fv = nullptr;
    IShellItemArray* arr = nullptr;

    if (SUCCEEDED(disp->QueryInterface(ORBIT_IID_SERVICEPROVIDER, (void**)&sp)) && sp &&
        SUCCEEDED(sp->QueryService(ORBIT_SID_TOPLEVEL, ORBIT_IID_SHELLBROWSER, (void**)&sb)) && sb &&
        SUCCEEDED(sb->QueryActiveShellView(&sv)) && sv) {
        HWND vw = nullptr;
        sv->GetWindow(&vw);
        if (!defView || vw == defView) {
            if (SUCCEEDED(sv->QueryInterface(ORBIT_IID_FOLDERVIEW2, (void**)&fv)) && fv &&
                SUCCEEDED(fv->GetSelection(FALSE, &arr)) && arr) {
                DWORD cnt = 0;
                arr->GetCount(&cnt);
                for (DWORD i = 0; i < cnt; i++) {
                    IShellItem* it = nullptr;
                    if (SUCCEEDED(arr->GetItemAt(i, &it)) && it) {
                        PWSTR p = nullptr;
                        if (SUCCEEDED(it->GetDisplayName(SIGDN_FILESYSPATH, &p)) && p) {
                            out.push_back(p);
                            CoTaskMemFree(p);
                        }
                        it->Release();
                    }
                }
                ok = true;
            }
        }
    }
    if (arr) arr->Release();
    if (fv) fv->Release();
    if (sv) sv->Release();
    if (sb) sb->Release();
    if (sp) sp->Release();
    return ok;
}

static void GetExplorerSelection(HWND hit, std::vector<std::wstring>& out) {
    HWND defView = GetParent(hit);
    HWND root = GetAncestor(hit, GA_ROOT);
    wchar_t cls[64] = {};
    GetClassNameW(root, cls, 64);
    bool desktop = !wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW");

    IShellWindows* sw = nullptr;
    if (FAILED(CoCreateInstance(ORBIT_CLSID_SHELLWINDOWS, nullptr, CLSCTX_ALL,
                                ORBIT_IID_SHELLWINDOWS, (void**)&sw)) || !sw) {
        return;
    }

    if (desktop) {
        VARIANT v;
        VariantInit(&v);
        long h = 0;
        IDispatch* d = nullptr;
        if (SUCCEEDED(sw->FindWindowSW(&v, &v, SWC_DESKTOP, &h, SWFO_NEEDDISPATCH, &d)) && d) {
            ReadSelection(d, nullptr, out);
            d->Release();
        }
    } else {
        long n = 0;
        sw->get_Count(&n);
        for (long i = 0; i < n; i++) {
            VARIANT vi;
            VariantInit(&vi);
            vi.vt = VT_I4;
            vi.lVal = i;
            IDispatch* d = nullptr;
            if (SUCCEEDED(sw->Item(vi, &d)) && d) {
                bool done = false;
                IWebBrowserApp* wb = nullptr;
                if (SUCCEEDED(d->QueryInterface(ORBIT_IID_WEBBROWSERAPP, (void**)&wb)) && wb) {
                    SHANDLE_PTR hw = 0;
                    wb->get_HWND(&hw);
                    if ((HWND)hw == root) done = ReadSelection(d, defView, out);
                    wb->Release();
                }
                d->Release();
                if (done) break;
            }
        }
    }
    sw->Release();
}

// ---- Mode switching ----

static void AddSlot(int& n, const wchar_t* name, int act) {
    Slot& s = g_slots[n++];
    lstrcpynW(s.name, name, ARRAYSIZE(s.name));
    s.command[0] = 0;
    s.args[0] = 0;
    s.hasIcon = false;
    s.action = act;
}

static const wchar_t* ExtOf(const std::wstring& p) {
    const wchar_t* e = wcsrchr(p.c_str(), L'.');
    const wchar_t* sl = wcsrchr(p.c_str(), L'\\');
    return (e && (!sl || e > sl)) ? e + 1 : L"";
}

static void EnterFileMode() {
    memcpy(g_appSlots, g_slots, sizeof(g_slots));
    g_appCount = g_slotCount;

    int n = 0;
    const wchar_t* e = ExtOf(g_files[0]);
    bool jpg = !_wcsicmp(e, L"jpg") || !_wcsicmp(e, L"jpeg");
    bool png = !_wcsicmp(e, L"png");
    bool bmp = !_wcsicmp(e, L"bmp");
    bool img = jpg || png || bmp || !_wcsicmp(e, L"gif") ||
               !_wcsicmp(e, L"tif") || !_wcsicmp(e, L"tiff");

    if (img) {
        if (!jpg) AddSlot(n, L"JPG", ACT_JPG);
        if (!png) AddSlot(n, L"PNG", ACT_PNG);
        if (!bmp) AddSlot(n, L"BMP", ACT_BMP);
    } else if (!_wcsicmp(e, L"doc") || !_wcsicmp(e, L"docx")) {
        AddSlot(n, L"PDF", ACT_PDF);
    }
    AddSlot(n, L"ZIP", ACT_ZIP);
    AddSlot(n, L"Copy path", ACT_COPY);
    AddSlot(n, L"Apps", ACT_APPS);

    g_slotCount = n;
    g_fileMode = true;
}

static void ExitFileMode() {
    if (!g_fileMode) return;
    memcpy(g_slots, g_appSlots, sizeof(g_slots));
    g_slotCount = g_appCount;
    g_fileMode = false;
}

static void SwitchToApps(HWND hwnd) {
    ExitFileMode();
    for (int i = 0; i < MAX_SLOTS; i++) {
        g_hv[i] = 0.0f;
        g_hvel[i] = 0.0f;
    }
    g_hovered = -1;
    g_pill = 0.0f;
    g_pillVel = 0.0f;
    g_pillIdx = 0;
    Render();
    Present(hwnd, g_alpha, nullptr);
}

// ---- Running a file action ----

// Escapes a string for use inside a PowerShell single-quoted literal.
static std::wstring Esc(const std::wstring& s) {
    std::wstring r;
    for (wchar_t c : s) {
        r += c;
        if (c == L'\'') r += c;
    }
    return r;
}

static void RunFileAction(int act) {
    if (g_files.empty()) return;

    // Paths go through a UTF-8 temp file, never the command line.
    wchar_t tmp[MAX_PATH], list[MAX_PATH + 40];
    GetTempPathW(MAX_PATH, tmp);
    wsprintfW(list, L"%sorbit_%lu.txt", tmp, (unsigned long)GetTickCount());

    HANDLE f = CreateFileW(list, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return;
    for (const std::wstring& p : g_files) {
        std::wstring line = p + L"\r\n";
        int n = WideCharToMultiByte(CP_UTF8, 0, line.c_str(), -1,
                                    nullptr, 0, nullptr, nullptr);
        if (n <= 1) continue;
        std::string u(n, 0);
        WideCharToMultiByte(CP_UTF8, 0, line.c_str(), -1, &u[0], n, nullptr, nullptr);
        DWORD w;
        WriteFile(f, u.data(), n - 1, &w, nullptr);
    }
    CloseHandle(f);

    std::wstring lp = Esc(list);
    std::wstring head =
        L"$l=@(Get-Content -LiteralPath '" + lp +
        L"' -Encoding UTF8|Where-Object{$_}); Remove-Item -LiteralPath '" + lp + L"'; ";

    auto img = [&](const wchar_t* ext, const wchar_t* fmt) {
        return head +
            L"Add-Type -AssemblyName System.Drawing; foreach($p in $l){try{"
            L"$i=[System.Drawing.Image]::FromFile($p);"
            L"$o=[IO.Path]::ChangeExtension($p,'" + std::wstring(ext) + L"');"
            L"if($o -ieq $p){$i.Dispose();continue};"
            L"if(Test-Path -LiteralPath $o){$o=[IO.Path]::ChangeExtension($p,'converted." +
            std::wstring(ext) + L"')};"
            L"$i.Save($o,[System.Drawing.Imaging.ImageFormat]::" + std::wstring(fmt) +
            L");$i.Dispose()}catch{}}";
    };

    std::wstring script;
    switch (act) {
    case ACT_PNG: script = img(L"png", L"Png"); break;
    case ACT_JPG: script = img(L"jpg", L"Jpeg"); break;
    case ACT_BMP: script = img(L"bmp", L"Bmp"); break;
    case ACT_PDF:
        script = head +
            L"try{$w=New-Object -ComObject Word.Application;$w.Visible=$false;"
            L"foreach($p in $l){$o=[IO.Path]::ChangeExtension($p,'pdf');"
            L"if(Test-Path -LiteralPath $o){$o=[IO.Path]::ChangeExtension($p,'converted.pdf')};"
            L"$d=$w.Documents.Open($p);$d.ExportAsFixedFormat($o,17);$d.Close($false)}"
            L"}finally{if($w){$w.Quit()}}";
        break;
    case ACT_ZIP:
        script = head +
            L"$d=Split-Path -Parent $l[0];"
            L"$n=if($l.Count -eq 1){[IO.Path]::GetFileNameWithoutExtension($l[0])}else{'Archive'};"
            L"$z=Join-Path $d ($n+'.zip');"
            L"if(Test-Path -LiteralPath $z){$z=Join-Path $d ($n+'_'+(Get-Date -Format yyyyMMdd_HHmmss)+'.zip')};"
            L"Compress-Archive -LiteralPath $l -DestinationPath $z";
        break;
    case ACT_COPY:
        script = head + L"Set-Clipboard -Value ($l -join [Environment]::NewLine)";
        break;
    default:
        return;
    }

    std::wstring args =
        L"-NoProfile -ExecutionPolicy Bypass -WindowStyle Hidden -Command \"" +
        script + L"\"";
    ShellExecuteW(nullptr, L"open", L"powershell.exe", args.c_str(), nullptr, SW_HIDE);
}

// ---- Hold sequence ----

static void FinishHold(HWND hwnd) {
    ExitFileMode();
    g_files.clear();
    if (g_fileActions) {
        HWND h = WindowFromPoint(g_holdPt);
        if (IsFileView(h)) GetExplorerSelection(h, g_files);
    }
    if (!g_files.empty()) EnterFileMode();
    ShowOverlay(hwnd);
}

static void BeginHold(HWND hwnd) {
    if (g_fileActions) {
        HWND h = WindowFromPoint(g_holdPt);
        if (IsFileView(h) && !ItemUnderCursorSelected(g_holdPt)) {
            // Right-click normally selects the item under the cursor first.
            SendMouse(MOUSEEVENTF_LEFTDOWN);
            SendMouse(MOUSEEVENTF_LEFTUP);
            SetTimer(hwnd, TIMER_SEL, 90, nullptr);
            return;
        }
    }
    FinishHold(hwnd);
}

static void OnHoldRelease(HWND hwnd) {
    if (!IsWindowVisible(hwnd)) return;
    POINT cur;
    RECT r;
    GetCursorPos(&cur);
    GetWindowRect(hwnd, &r);
    int idx = HitTest(cur.x - r.left, cur.y - r.top);
    if (idx >= 0) LaunchSlot(hwnd, idx);
}

// --------------------------------------------------
// Window procedure
// --------------------------------------------------

static LRESULT CALLBACK OverlayWndProc(
    HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam
) {
    switch (msg) {

    case WM_MOUSEMOVE: {
        if (!g_trackingMouse) {
            TRACKMOUSEEVENT tme = {};
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            g_trackingMouse = true;
        }
        SetHovered(hwnd, HitTest(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
        return 0;
    }

    case WM_MOUSELEAVE:
        g_trackingMouse = false;
        SetHovered(hwnd, -1);
        return 0;

    case WM_LBUTTONUP: {
        int idx = HitTest(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        if (idx >= 0) {
            LaunchSlot(hwnd, idx);
        } else {
            HideOverlay(hwnd);  // hub or rim click = cancel
        }
        return 0;
    }

    case WM_RBUTTONUP:
        HideOverlay(hwnd);
        return 0;

    case WM_KEYDOWN: {
        int n = g_slotCount;

        if (wParam == VK_ESCAPE) {
            HideOverlay(hwnd);
            return 0;
        }

        if (wParam >= '1' && wParam <= '8') {
            LaunchSlot(hwnd, (int)(wParam - '1'));
            return 0;
        }
        if (wParam >= VK_NUMPAD1 && wParam <= VK_NUMPAD8) {
            LaunchSlot(hwnd, (int)(wParam - VK_NUMPAD1));
            return 0;
        }

        if (n > 0) {
            if (wParam == VK_RIGHT || wParam == VK_DOWN || wParam == VK_TAB) {
                bool back = (wParam == VK_TAB) && (GetKeyState(VK_SHIFT) < 0);
                int next = g_hovered < 0 ? (back ? n - 1 : 0)
                                         : (g_hovered + (back ? n - 1 : 1)) % n;
                SetHovered(hwnd, next);
                return 0;
            }
            if (wParam == VK_LEFT || wParam == VK_UP) {
                int next = g_hovered < 0 ? n - 1 : (g_hovered + n - 1) % n;
                SetHovered(hwnd, next);
                return 0;
            }
            if ((wParam == VK_RETURN || wParam == VK_SPACE) && g_hovered >= 0) {
                LaunchSlot(hwnd, g_hovered);
                return 0;
            }
        }
        break;
    }

    case WM_ACTIVATE:
        // Dismiss when the user switches away (clicks outside are handled
        // by the mouse hook, so this is only a second safety net).
        if (LOWORD(wParam) == WA_INACTIVE && IsWindowVisible(hwnd) &&
            GetTickCount64() - g_shownAt > 150) {
            HideOverlay(hwnd);
        }
        return 0;

    case WM_TIMER:
        if (wParam == TIMER_ANIM) {
            AnimTick(hwnd);
            return 0;
        }
        if (wParam == TIMER_HOLD) {
            KillTimer(hwnd, TIMER_HOLD);
            if (g_hs == HS_PENDING) {
                g_hs = HS_HELD;
                BeginHold(hwnd);
            }
            return 0;
        }
        if (wParam == TIMER_SEL) {
            KillTimer(hwnd, TIMER_SEL);
            if (g_hs == HS_HELD) FinishHold(hwnd);
            return 0;
        }
        break;

    case WM_ORBIT_RELEASE:
        OnHoldRelease(hwnd);
        return 0;

    case WM_ORBIT_RELOAD:
        HideOverlay(hwnd);
        KillTimer(hwnd, TIMER_HOLD);
        KillTimer(hwnd, TIMER_SEL);
        g_hs = HS_IDLE;
        LoadSettings();
        RegisterConfiguredHotkey();
        LoadSlotIcons();
        RenderBase();
        return 0;

    case WM_ERASEBKGND:
        return 1;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// --------------------------------------------------
// Overlay thread
// --------------------------------------------------

static DWORD WINAPI OverlayThreadProc(LPVOID) {
    // Make sure this thread has a message queue before signalling anyone.
    MSG msg = {};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    // Needed for ShellExecute, shell icon extraction, shell selection and UIA.
    bool comInit = SUCCEEDED(CoInitializeEx(
        nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE));

    HINSTANCE inst = GetModuleHandleW(nullptr);
    bool classRegistered = false;
    bool signalled = false;
    bool owner = false;

    auto signal = [&](LONG code) {
        if (signalled) return;
        InterlockedExchange(&g_threadStatus, code);
        SetEvent(g_readyEvent);
        signalled = true;
    };

    auto cleanup = [&]() {
        if (g_overlayWindow) {
            DestroyWindow(g_overlayWindow);
            g_overlayWindow = nullptr;
        }
        if (classRegistered) {
            UnregisterClassW(WINDOW_CLASS, inst);
            classRegistered = false;
        }
        DestroySurface();
        if (g_mutex) {
            if (owner) ReleaseMutex(g_mutex);
            CloseHandle(g_mutex);
            g_mutex = nullptr;
            owner = false;
        }
        if (comInit) CoUninitialize();
    };

    auto fail = [&](LONG code) -> DWORD {
        cleanup();
        signal(code);
        return 1;
    };

    // ---- Single instance across all explorer.exe processes ----
    g_mutex = CreateMutexW(nullptr, FALSE, SINGLETON_MUTEX);
    if (!g_mutex) {
        Wh_Log(L"ORBIT: CreateMutex failed. Error: %u", GetLastError());
        return fail(-5);
    }

    DWORD w = WaitForSingleObject(g_mutex, 0);
    if (w == WAIT_TIMEOUT) {
        // Another explorer.exe process already owns the wheel. Stay idle and
        // take over only if that process goes away.
        Wh_Log(L"ORBIT: Another instance is active, standing by.");
        signal(1);

        HANDLE waitFor[2] = {g_mutex, g_quitEvent};
        w = WaitForMultipleObjects(2, waitFor, FALSE, INFINITE);
        if (w != WAIT_OBJECT_0 && w != WAIT_ABANDONED_0) {
            cleanup();
            Wh_Log(L"ORBIT: Standby thread stopped.");
            return 0;
        }
        Wh_Log(L"ORBIT: Previous instance gone, taking over.");
    } else if (w != WAIT_OBJECT_0 && w != WAIT_ABANDONED_0) {
        return fail(-5);
    }
    owner = true;

    UpdateLayout();
    LoadSettings();

    if (!EnsureSurface()) {
        Wh_Log(L"ORBIT: Surface creation failed.");
        return fail(-4);
    }
    RenderBase();

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = inst;
    wc.lpszClassName = WINDOW_CLASS;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);

    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"ORBIT: RegisterClassEx failed. Error: %u", GetLastError());
        return fail(-1);
    }
    classRegistered = true;

    g_overlayWindow = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        WINDOW_CLASS, L"ORBIT", WS_POPUP,
        0, 0, g_size, g_size,
        nullptr, nullptr, inst, nullptr);

    if (!g_overlayWindow) {
        Wh_Log(L"ORBIT: CreateWindowEx failed. Error: %u", GetLastError());
        return fail(-2);
    }

    // A bad hotkey is not fatal: it is logged and can be fixed in settings.
    RegisterConfiguredHotkey();

    signal(1);
    Wh_Log(L"ORBIT: Overlay thread ready.");

    // Icons are loaded after signalling so mod init stays fast.
    LoadSlotIcons();

    // Installed last so slow icon loading can never stall the mouse.
    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseHookProc,
                                    GetModuleHandleW(nullptr), 0);
    if (!g_mouseHook) {
        Wh_Log(L"ORBIT: Mouse hook failed: %u", GetLastError());
    }

    while (true) {
        BOOL r = GetMessageW(&msg, nullptr, 0, 0);
        if (r <= 0) break;

        if (msg.message == WM_HOTKEY && msg.wParam == HOTKEY_ID) {
            ToggleOverlay();
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // Cleanup.
    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }

    if (g_hotkeyRegistered) {
        UnregisterHotKey(nullptr, HOTKEY_ID);
        g_hotkeyRegistered = false;
    }

    cleanup();

    Wh_Log(L"ORBIT: Overlay thread stopped.");
    return 0;
}

// --------------------------------------------------
// Windhawk entry points
// --------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"ORBIT: Initializing.");

    g_threadStatus = 0;

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_quitEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent || !g_quitEvent) {
        Wh_Log(L"ORBIT: CreateEvent failed. Error: %u", GetLastError());
        if (g_readyEvent) { CloseHandle(g_readyEvent); g_readyEvent = nullptr; }
        if (g_quitEvent) { CloseHandle(g_quitEvent); g_quitEvent = nullptr; }
        return FALSE;
    }

    g_threadHandle = CreateThread(
        nullptr, 0, OverlayThreadProc, nullptr, 0, &g_threadId);

    if (!g_threadHandle) {
        Wh_Log(L"ORBIT: CreateThread failed. Error: %u", GetLastError());
        CloseHandle(g_readyEvent);
        CloseHandle(g_quitEvent);
        g_readyEvent = nullptr;
        g_quitEvent = nullptr;
        return FALSE;
    }

    DWORD waitResult = WaitForSingleObject(g_readyEvent, 5000);

    if (waitResult != WAIT_OBJECT_0 ||
        InterlockedCompareExchange(&g_threadStatus, 0, 0) != 1) {

        Wh_Log(L"ORBIT: Overlay startup failed.");

        SetEvent(g_quitEvent);
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_threadHandle, INFINITE);

        CloseHandle(g_threadHandle);
        CloseHandle(g_readyEvent);
        CloseHandle(g_quitEvent);
        g_threadHandle = nullptr;
        g_readyEvent = nullptr;
        g_quitEvent = nullptr;
        return FALSE;
    }

    Wh_Log(L"ORBIT: Initialization successful.");
    return TRUE;
}

void Wh_ModSettingsChanged() {
    HWND hwnd = g_overlayWindow;
    if (hwnd) PostMessageW(hwnd, WM_ORBIT_RELOAD, 0, 0);
}

void Wh_ModUninit() {
    Wh_Log(L"ORBIT: Shutting down.");

    if (g_threadHandle) {
        if (g_quitEvent) SetEvent(g_quitEvent);  // wakes a standby thread
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);  // stops an active one
        WaitForSingleObject(g_threadHandle, INFINITE);
        CloseHandle(g_threadHandle);
        g_threadHandle = nullptr;
    }

    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }
    if (g_quitEvent) {
        CloseHandle(g_quitEvent);
        g_quitEvent = nullptr;
    }

    Wh_Log(L"ORBIT: Unloaded successfully.");
}
